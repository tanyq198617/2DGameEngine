//
//  HLResourceManager.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-1-17.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLResourceManager.h"
#include "HLUtil.h"
#include <fstream>
#include "md5.h"
#include <zlib.h>
#include <OpenThreads/Mutex>
#include <algorithm>
#if defined(TARGET_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif
#include "HLApplication.h"
#include <errno.h>
#ifdef TARGET_ANDROID
#include "android/asset_manager.h"
#include "android/asset_manager_jni.h"
#endif

#ifdef TARGET_ANDROID
extern AAssetManager* s_assetmanager;
#endif

NS_HL_BEGIN

#if defined(__GNUC__) || defined(_MSC_VER)
#define HAS_STRUCT_PACK
#endif

class FileReader
{
public:
    virtual ~FileReader() {}
    virtual bool open(const char* path) = 0;
    virtual void read(char* buffer, size_t size) = 0;
    virtual void seek(size_t offset) = 0;
    virtual void close() = 0;
};

class RawFileReader: public FileReader
{
private:
    ifstream mStream;
public:
    virtual ~RawFileReader()
    {
        mStream.close();
    }
    virtual bool open(const char* path)
    {
        mStream.open(path, ios_base::binary);
        if (mStream.fail())
        {
            mStream.close();
            return false;
        }
        return true;
    }
    virtual void read(char* buffer, size_t size)
    {
        mStream.read(buffer, size);
    }
    virtual void seek(size_t offset)
    {
        mStream.seekg(offset);
    }
    virtual void close()
    {
        mStream.close();
    }
};

#ifdef TARGET_ANDROID
class AndroidAssetReader: public FileReader
{
private:
    AAsset* mAsset {nullptr};
public:
    virtual ~AndroidAssetReader()
    {
        if (mAsset)
        {
            AAsset_close(mAsset);
        }
    }
    virtual bool open(const char* path)
    {
        HLLOG("AndroidAssetReader open %s", path);
        mAsset = AAssetManager_open(s_assetmanager, path, AASSET_MODE_UNKNOWN);
        HLLOG("AndroidAssetReader open 0x%x", mAsset);
        return mAsset != nullptr;
    }
    virtual void read(char* buffer, size_t size)
    {
        AAsset_read(mAsset, (void*)buffer, size);
    }
    virtual void seek(size_t offset)
    {
        AAsset_seek(mAsset, offset, SEEK_SET);
    }
    virtual void close()
    {
        AAsset_close(mAsset);
        mAsset = nullptr;
    }
};
#endif

class HLResourcePackageLoader
{
private:
#ifdef HAS_STRUCT_PACK
#pragma pack(push, 1)
#endif
    struct resfileindex
    {
        uint8_t key[16];
        uint16_t file;
        uint32_t offset;
        uint32_t size;
        uint8_t compressed;
        uint32_t relsize;
        uint8_t version;
        uint32_t retaincount;
    };
#ifdef HAS_STRUCT_PACK
#pragma pack(pop)
#endif
    std::string savepath;
    std::vector<struct resfileindex*> files;
    FileReader** fss;
    std::vector<std::string> pakfiles;
    
    OpenThreads::Mutex mMutex;
    
    static void retainFileIndex(struct resfileindex* index)
    {
        ++index->retaincount;
    }
    
    static void releaseFileIndex(struct resfileindex* index)
    {
        --index->retaincount;
        if (index->retaincount == 0)
        {
            delete index;
        }
    }
    
    static int kcmp(const uint8_t key1[], const uint8_t key2[])
    {
        for (int i = 0; i < 16; ++i)
        {
            if (key1[i] < key2[i])
            {
                return -1;
            }
            else if (key1[i] > key2[i])
            {
                return 1;
            }
        }
        return 0;
    }
    
    static int fcmp(const struct resfileindex& f1, const struct resfileindex& f2)
    {
        return kcmp(f1.key, f2.key);
    }
    
    static bool fcmp1(const struct resfileindex* f1, const struct resfileindex* f2)
    {
        int res = kcmp(f1->key, f2->key);
        if (res == 0)
        {
            return f1->version < f2->version;
        }
        return res < 0;
    }
    
    int findFileIndex(uint8_t key[])
    {
        int low = 0;
        int high = (int)files.size() - 1;
        while (low <= high)
        {
            int middle = (low+high)/2;
            int res = kcmp(reinterpret_cast<unsigned char*>(key), files[middle]->key);
            if (res == 0)
            {
                return middle;
            }
            else if (res < 0)
            {
                high = middle - 1;
            }
            else
            {
                low = middle + 1;
            }
        }
        return -1;
    }
    
public:
    HLResourcePackageLoader():fss(NULL)
    {
        header.version = 0;
        header.scale = 0.5;
    }
    
    HLResourcePackageLoader(const char* filepath):fss(NULL)
    {
        FileReader* reader = nullptr;
#ifdef TARGET_ANDROID
        if (strncmp(filepath, "apk://", 6) == 0) // read from android assets
        {
            reader = new AndroidAssetReader();
            if (!reader->open(filepath+6))
            {
                HLLOG("HLResourcePackageLoader: cannot open file in assets %s", filepath);
                delete reader;
                return;
            }
        }
#endif
        if (!reader)
        {
            reader = new RawFileReader();
            if (!reader->open(filepath))
            {
                HLLOG("HLResourcePackageLoader: cannot open file %s %s", filepath, strerror(errno));
                delete reader;
                return;
            }
        }
        char magic[6];
        reader->read(magic, 6);
        if (memcmp(magic, "!hlidx", 6))
        {
            HLLOG("HLResourcePackageLoader: invalid idx file %s", filepath);
            reader->close();
            delete reader;
            return;
        }
        reader->read(reinterpret_cast<char*>(&header), sizeof(header));
        LittleEndianToNative<sizeof(header.version)>(&header.version);
        LittleEndianToNative<sizeof(header.scale)>(&header.scale);
        memset(header.extend, 0, 24);
        uint16_t size;
        reader->read(reinterpret_cast<char*>(&size), 2);
        LittleEndianToNative<2>(&size);
        for (int i = 0; i < size; ++i)
        {
            uint8_t len;
            reader->read(reinterpret_cast<char*>(&len), 1);
            char* buffer = new char[len+5];
            buffer[len] = '.';
            buffer[len+1] = 'p';
            buffer[len+2] = 'a';
            buffer[len+3] = 'k';
            buffer[len+4] = '\0';
            reader->read(buffer, len);
            pakfiles.push_back(buffer);
            delete buffer;
        }
        uint32_t count;
        reader->read(reinterpret_cast<char*>(&count), 4);
        LittleEndianToNative<4>(&count);
        files.resize(count);
        for (int i = 0; i < count; ++i)
        {
            struct resfileindex *pointer = files[i] = new struct resfileindex;
            pointer->retaincount = 1;
            pointer->version = 0;
            reader->read(reinterpret_cast<char*>(pointer), 27);
            LittleEndianToNative<2>(&pointer->file);
            LittleEndianToNative<4>(&pointer->offset);
            LittleEndianToNative<4>(&pointer->size);
            if (pointer->compressed)
            {
                reader->read(reinterpret_cast<char*>(&pointer->relsize), 4);
                LittleEndianToNative<4>(&pointer->relsize);
            }
        }
        reader->close();
        delete reader;
        HLLOG("HLResourcePackageLoader version: %d", header.version);
    }
    
    HLResourcePackageLoader(HLFileData* data, bool needcopy = false):fss(NULL)
    {
        if (memcmp(data->buffer, "!hlidx", 6))
        {
            HLLOG("HLResourcePackageLoader: invalid idx file");
            return;
        }
        int index = 6;
        memcpy(&header, &data->buffer[index], sizeof(header));
        index += sizeof(header);
        LittleEndianToNative<sizeof(header.version)>(&header.version);
        LittleEndianToNative<sizeof(header.scale)>(&header.scale);
        memset(header.extend, 0, 24);
        uint16_t size;
        memcpy(&size, &data->buffer[index], 2);
        index += 2;
        LittleEndianToNative<2>(&size);
        for (int i = 0; i < size; ++i)
        {
            uint8_t len = data->buffer[index];
            index += 1;
            char* buffer = new char[len+5];
            memcpy(buffer, &data->buffer[index], len);
            index += len;
            buffer[len] = '.';
            buffer[len+1] = 'p';
            buffer[len+2] = 'a';
            buffer[len+3] = 'k';
            buffer[len+4] = '\0';
            pakfiles.push_back(buffer);
            delete buffer;
        }
        uint32_t count;
        memcpy(&count, &data->buffer[index], 4);
        index += 4;
        LittleEndianToNative<4>(&count);
        files.resize(count);
        for (int i = 0; i < count; ++i)
        {
            struct resfileindex *pointer = files[i] = new struct resfileindex;
            pointer->retaincount = 1;
            pointer->version = 0;
            memcpy(pointer, &data->buffer[index], 27);
            index += 27;
            LittleEndianToNative<4>(&pointer->file);
            LittleEndianToNative<4>(&pointer->offset);
            LittleEndianToNative<4>(&pointer->size);
            if (pointer->compressed)
            {
                memcpy(&pointer->relsize, &data->buffer[index], 4);
                index += 4;
                LittleEndianToNative<4>(&pointer->relsize);
            }
        }
#ifdef TARGET_ANDROID
        if (needcopy)
        {
            for (std::vector<std::string>::iterator itr = pakfiles.begin(); itr != pakfiles.end(); ++itr)
            {
                if (HLResourceManager::getSingleton()->fileInAppBundle((*itr).c_str()))
                {
                    HLResourceManager::getSingleton()->copyFileInAssetToWritablePath((*itr).c_str());
                }
            }
        }
#endif
    }
    
    void setSavePath(std::string path)
    {
        savepath = path;
    }
    
    bool updateWithLoader(HLResourcePackageLoader* loader)
    {
        HLLOG("updateWithLoader %d %d", header.version, loader->header.version);
        if (loader->pakfiles.size() == 0)
        {
            if (header.version < loader->header.version)
            {
                header.version = loader->header.version;
                return true;
            }
            return false;
        }
        int filesnum = (int)pakfiles.size();
        std::vector<uint16_t> ignorefiles;
        std::vector<uint16_t> ignorefilesidx;
        for (std::vector<std::string>::iterator itr = loader->pakfiles.begin(); itr != loader->pakfiles.end(); ++itr)
        {
            std::vector<std::string>::iterator itr1 = std::find(pakfiles.begin(), pakfiles.end(), *itr);
            if (itr1 != pakfiles.end())
            {
                ignorefiles.push_back((uint16_t)distance(loader->pakfiles.begin(), itr));
                ignorefilesidx.push_back((uint16_t)(itr1 - pakfiles.begin()));
            }
            else
            {
                pakfiles.push_back(*itr);
            }
        }
//        if (filesnum == pakfiles.size())
//        {
//            delete loader;
//            return false;
//        }
        std::vector<struct resfileindex*> tmp;
        for (std::vector<struct resfileindex*>::iterator itr = files.begin(); itr != files.end(); ++itr)
        {
            std::vector<uint16_t>::iterator itr1 = std::find(ignorefilesidx.begin(), ignorefilesidx.end(), (*itr)->file);
            if (itr1 != ignorefilesidx.end())
            {
                releaseFileIndex(*itr);
            }
            else
            {
                (*itr)->version = 0;
                tmp.push_back(*itr);
            }
        }
        std::vector<uint16_t> newfileindex;
        newfileindex.resize(loader->pakfiles.size());
        for (int i = 0; i < loader->pakfiles.size(); ++i)
        {
            std::vector<uint16_t>::iterator itr1 = std::find(ignorefiles.begin(), ignorefiles.end(), i);
            if (itr1 != ignorefiles.end())
            {
                newfileindex[i] = ignorefilesidx[itr1 - ignorefiles.begin()];
            }
            else
            {
                int k = filesnum + i;
                for (int j = 0; j < ignorefiles.size(); ++j)
                {
                    if (ignorefiles[j] < i)
                    {
                        --k;
                    }
                    else
                    {
                        break;
                    }
                }
                newfileindex[i] = k;
            }
        }
        
        for (std::vector<struct resfileindex*>::iterator itr = loader->files.begin(); itr != loader->files.end(); ++itr)
        {
            (*itr)->file = newfileindex[(*itr)->file];
            (*itr)->version = 1;
            retainFileIndex(*itr);
//            std::vector<uint16_t>::iterator itr1 = std::find(ignorefiles.begin(), ignorefiles.end(), (*itr)->file);
//            if (itr1 != ignorefiles.end())
//            {
//                (*itr)->file = ignorefilesidx[distance(ignorefiles.begin(), itr1)];
//            }
//            else
//            {
//                uint16_t fidx = (*itr)->file;
//                (*itr)->file += filesnum;
//                for (int i = 0; i < ignorefiles.size(); ++i)
//                {
//                    if (ignorefiles[i] < fidx)
//                    {
//                        --(*itr)->file;
//                    }
//                    else
//                    {
//                        break;
//                    }
//                }
//            }
//            int index = findFileIndex((*itr).key);
//            if (index >= 0)
//            {
//                if (header.version <= loader->header.version)
//                {
//                    files.erase(files.begin()+index);
//                    files.insert(files.begin()+index, *itr);
//                }
//            }
//            else
//            {
//                std::vector<struct resfileindex>::iterator insertpos;
//                for (insertpos = files.begin(); insertpos != files.end(); ++insertpos)
//                {
//                    if (!fcmp1(*insertpos, *itr))
//                    {
//                        break;
//                    }
//                }
//                files.insert(insertpos, *itr);
//            }
        }
        tmp.insert(tmp.end(), loader->files.begin(), loader->files.end());
        std::sort(tmp.begin(), tmp.end(), fcmp1);
        
        //erase duplicated entry
        files.clear();
        int count = (int)tmp.size();
        for (int i = 0; i < count;)
        {
            struct resfileindex* p = tmp[i];
            int j = i+1;
            while (j < count && fcmp(*p, *tmp[j]) == 0)
            {
                struct resfileindex* p1 = tmp[j];
                if (p1->version == 1 && header.version < loader->header.version)
                {
                    releaseFileIndex(p);
                    p = p1;
                    p->version = 0;
                }
                else
                {
                    releaseFileIndex(p1);
                }
                ++j;
            }
            files.push_back(p);
            i = j;
        }
        
        header.version = MAX(header.version, loader->header.version);
        
        if (fss)
        {
            for (int i = 0; i < filesnum; ++i)
            {
                //                fss[i].close();
//                fclose(fss[i]);
                if (fss[i])
                {
                    fss[i]->close();
                    delete fss[i];
                }
            }
            delete [] fss;
            fss = NULL;
        }
        return true;
    }
    
    bool updateWithFile(const char* path)
    {
        HLResourcePackageLoader* loader = new HLResourcePackageLoader(path);
        bool ret = updateWithLoader(loader);
        delete loader;
        return ret;
    }
    
    bool removeWithLoader(HLResourcePackageLoader* loader)
    {
        int filesnum = (int)pakfiles.size();
        
        std::vector<uint16_t> rmfiles;
        for (std::vector<std::string>::iterator itr = loader->pakfiles.begin(); itr != loader->pakfiles.end(); ++itr)
        {
            std::vector<std::string>::iterator itr1 = std::find(pakfiles.begin(), pakfiles.end(), *itr);
            if (itr1 != pakfiles.end())
            {
                rmfiles.push_back((uint16_t)distance(pakfiles.begin(), itr1));
            }
        }
        
        delete loader;
        
        if (rmfiles.size() == 0)
        {
            return true;
        }
        
        std::vector<struct resfileindex*> tmp;
        for (std::vector<struct resfileindex*>::iterator itr = files.begin(); itr != files.end(); ++itr)
        {
            if (std::find(rmfiles.begin(), rmfiles.end(), (*itr)->file) != rmfiles.end())
            {
                releaseFileIndex(*itr);
            }
            else
            {
                tmp.push_back(*itr);
            }
        }
        files.clear();
        files.swap(tmp);
        
        if (fss)
        {
            for (int i = 0; i < filesnum; ++i)
            {
                //                fss[i].close();
                if (fss[i])
                {
                    fss[i]->close();
                    delete fss[i];
                }
                //                fclose(fss[i]);
            }
            delete [] fss;
            fss = NULL;
        }
        
        return true;
    }
    
    bool removeWithFile(const char* path)
    {
        HLResourcePackageLoader* loader = new HLResourcePackageLoader(path);
        if (loader->pakfiles.size() == 0)
        {
            delete loader;
            return false;
        }
        
        return removeWithLoader(loader);
        
    }
    
    void save()
    {
        if (savepath.length())
        {
            writeToFile(savepath.c_str());
        }
    }
    
    void writeToFile(const char* path)
    {
        if (files.size() == 0)
        {
            return;
        }
        ofstream out(path, ios_base::binary);
        if (out.fail())
        {
            HLLOG("HLResourcePackageLoader: failed to write file %s", path);
            return;
        }
        out.write("!hlidx", 6);
        uint32_t version = header.version;
        NativeToLittleEndian<sizeof(uint32_t)>(&version);
        out.write(reinterpret_cast<const char*>(&version), 4);
        float scale = header.scale;
        NativeToLittleEndian<sizeof(float)>(&scale);
        out.write(reinterpret_cast<const char*>(&scale), 4);
        out.write(header.extend, 24);
        uint16_t size = (uint16_t)pakfiles.size();
        NativeToLittleEndian<sizeof(uint16_t)>(&size);
        out.write(reinterpret_cast<const char*>(&size), 2);
        for (std::vector<std::string>::iterator itr = pakfiles.begin(); itr != pakfiles.end(); ++itr)
        {
            std::string s = (*itr).substr(0, (*itr).length()-4);
            unsigned char len = (unsigned char)s.length();
            out.write((const char*)&len, 1);
            out.write(s.c_str(), s.length());
        }
        uint32_t count = (uint32_t)files.size();
        NativeToLittleEndian<sizeof(uint32_t)>(&count);
        out.write(reinterpret_cast<const char*>(&count), 4);
        for (std::vector<resfileindex*>::iterator itr = files.begin(); itr != files.end(); ++itr)
        {
            const struct resfileindex &file = *(*itr);
            out.write(reinterpret_cast<const char*>(file.key), 16);
            char tmp[11];
            uint16_t* fl = reinterpret_cast<uint16_t*>(tmp);
            uint32_t* offset = reinterpret_cast<uint32_t*>(tmp+2);
            uint32_t* size = reinterpret_cast<uint32_t*>(tmp+6);
            *fl = file.file;
            NativeToLittleEndian<2>(fl);
            *offset = file.offset;
            NativeToLittleEndian<4>(offset);
            *size = file.size;
            NativeToLittleEndian<4>(size);
            tmp[10] = file.compressed;
            out.write(tmp, 11);
            if (file.compressed)
            {
                uint32_t relsize = file.relsize;
                NativeToLittleEndian<4>(&relsize);
                out.write(reinterpret_cast<const char*>(&relsize), 4);
            }
        }
        out.flush();
        out.close();
    }
    
    ~HLResourcePackageLoader()
    {
        for (std::vector<struct resfileindex*>::iterator itr = files.begin(); itr != files.end(); ++itr)
        {
            releaseFileIndex(*itr);
        }
        if (fss)
        {
            int count = (int)pakfiles.size();
            for (int i = 0; i < count; ++i)
            {
//                fss[i].close();
                if (fss[i])
                {
                    fss[i]->close();
                    delete fss[i];
                }
//                fclose(fss[i]);
            }
            delete [] fss;
        }
    }
    
    struct
    {
        uint32_t version;
        float scale;
        char extend[24];
    } header;
    
    int findFileIndex(const char* path)
    {
        ::MD5 md5;
        md5.GenerateMD5((unsigned char*)path, (int)strlen(path));
        
        int index = findFileIndex(reinterpret_cast<unsigned char*>(md5.m_data));
        if (index >= 0)
        {
            if (!fss)
            {
                size_t size = pakfiles.size();
                fss = new FileReader*[size];
                for (int i = 0; i < size; ++i)
                {
                    char fullpath[MAX_PATH];
                    HLResourceManager::getSingleton()->getSysPathForFile(pakfiles[i].c_str(), fullpath);
#ifdef TARGET_ANDROID
                    if (strncmp(fullpath, "apk://", 6) == 0) // read from android assets
                    {
                        fss[i] = new AndroidAssetReader();
                        fss[i]->open(fullpath+6);
                    }
                    else
                    {
#endif
                        fss[i] = new RawFileReader();
                        fss[i]->open(fullpath);
//                    fss[i].open(fullpath, ios_base::binary|ios_base::in);
#ifdef TARGET_ANDROID
                    }
#endif
                }
            }
            if (!fss[files[index]->file])
            {
                return -1;
            }
        }
        return index;
    }
    
    HLFileData* getFileData(const char* path)
    {
        return getFileData(findFileIndex(path));
    }
    
    HLFileData* getFileData(int index)
    {
        if (index < 0 || index >= files.size())
        {
            return NULL;
        }
        const struct resfileindex& file = *files[index];
        if (!fss[file.file])
        {
            return NULL;
        }
        unsigned char* buffer = new unsigned char[file.size];
//        fss[file.file].seekg(file.offset, ios_base::beg);
//        fss[file.file].read(reinterpret_cast<char*>(buffer), file.size);
        mMutex.lock();
        fss[file.file]->seek(file.offset);
        fss[file.file]->read((char*)buffer, file.size);
        mMutex.unlock();
        if (!file.compressed)
        {
            HLFileData* data = new HLFileData();
            data->size = file.size;
            data->buffer = buffer;
            return data;
        }
        else
        {
            unsigned char* buffer1 = new unsigned char[file.relsize];
            uLong destsize = file.relsize;
            uncompress(static_cast<Byte*>(buffer1), &destsize, buffer, (uLong)file.size);
            delete buffer;
            HLFileData* data = new HLFileData();
            data->size = destsize;
            data->buffer = buffer1;
            return data;
        }
    }
};

HLResourceManager::~HLResourceManager()
{
    if (mResLoader)
    {
        delete mResLoader;
    }
}

HLFileData* HLResourceManager::getFileData(const char* file)
{
    if (mResLoader)
    {
        HLFileData* res = mResLoader->getFileData(file);
        if (res)
        {
            return res;
        }
        
        char locPath[256];
        HLApplication* psharedApp = HLApplication::sharedApplication();
        const char* languageStr = getLanguageStr(psharedApp->getCurrentLanguage());
        strcpy(locPath, languageStr);
        strcat(locPath, ".lproj/");
        strcat(locPath, file);
        
        res = mResLoader->getFileData(locPath);
        if (res)
        {
            return res;
        }
    }
    
    char path[MAX_PATH];
    getSysPathForFile(file, path);
    if (path[0])
    {
#if defined(TARGET_ANDROID)
        if (strncmp(path, "apk://", 6) != 0) // not in assets
#endif
        {
            return new HLFileData(path, "rb");
        }
#if defined(TARGET_ANDROID)
        else
        {
            return getFileDataInAppBundle(path+6);
        }
#endif
    }
    
    return NULL;
}

bool HLResourceManager::fileExists(const char* file)
{
    if (mResLoader)
    {
        if (mResLoader->findFileIndex(file) >= 0)
        {
            return true;
        }
        
        char locPath[256];
        HLApplication* psharedApp = HLApplication::sharedApplication();
        const char* languageStr = getLanguageStr(psharedApp->getCurrentLanguage());
        strcpy(locPath, languageStr);
        strcat(locPath, ".lproj/");
        strcat(locPath, file);
        
        if (mResLoader->findFileIndex(locPath) >= 0)
        {
            return true;
        }
    }
    
    char path[MAX_PATH];
    getSysPathForFile(file, path);
    if (path[0])
    {
        return true;
    }
    
    return false;
}

void HLResourceManager::addResourcePackage(const char* file)
{
    if (mResLoaders.find(file) != mResLoaders.end())
    {
        return;
    }
    if (fileExists(file)) // file exists
    {
        std::string idxfile(file);
        idxfile += ".i";
        idxfile = getSysPathForFile(idxfile.c_str());
        
        HLResourcePackageLoader* loader = nullptr;
        if (fileInAppBundle(file) && idxfile[0]) // compare versions first
        {
            HLResourcePackageLoader *loader1 = new HLResourcePackageLoader(getSysPathForFile(file));
            HLResourcePackageLoader *loader2 = new HLResourcePackageLoader(idxfile.c_str());
            if (loader1->header.version > loader2->header.version)
            {
                remove(idxfile.c_str());
                loader = loader1;
                delete loader2;
            }
            else
            {
                loader = loader2;
                delete loader1;
            }
        }
        
        if (!loader)
        {
            if (idxfile[0])
            {
                loader = new HLResourcePackageLoader(idxfile.c_str());
            }
            else
            {
                idxfile = mWritablePath + file;
                idxfile += ".i";
                loader = new HLResourcePackageLoader(getSysPathForFile(file));
            }
        }
        
        loader->setSavePath(idxfile);
        
        mResLoaders.insert(make_pair(file, loader));
        
        if (!mResLoader)
        {
            mResLoader = new HLResourcePackageLoader();
        }
        mResLoader->updateWithLoader(loader);
    }
}

void HLResourceManager::updateResourcePackage(const char* file, const char* patchfile)
{
    auto itr = mResLoaders.find(file);
    if (itr == mResLoaders.end())
    {
        return;
    }
    const char* path = getSysPathForFile(file);
    if (path[0] && mResLoader)
    {
        if (itr->second->updateWithFile(path))
        {
            itr->second->save();
        }
        mResLoader->updateWithLoader(itr->second);
        mResourceVersion = mResLoader->header.version;
    }
}

void HLResourceManager::removeResourcePackage(const char* file)
{
    auto itr = mResLoaders.find(file);
    if (itr == mResLoaders.end())
    {
        return;
    }
    const char* path = getSysPathForFile(file);
    if (path[0] && mResLoader)
    {
        mResLoader->removeWithLoader(itr->second);
        delete itr->second;
        remove(path);
        mResLoaders.erase(itr);
    }
    std::string idxfile(file);
    idxfile += ".i";
    idxfile = getSysPathForFile(idxfile.c_str());
    if (idxfile[0])
    {
        remove(idxfile.c_str());
    }
}

unsigned long HLResourceManager::getResourceVersionOfPackage(const char* file)
{
    auto itr = mResLoaders.find(file);
    if (itr == mResLoaders.end())
    {
        return 0;
    }
    return itr->second->header.version;
}

void HLResourceManager::getSysPathForFile(const char* file, char path[])
{
    path[0] = '\0';
    memset(path, 0, MAX_PATH);
    
    if (file[0] == '/') // absolutepath
    {
        if (access(file, 0) == 0)
        {
            strcpy(path, file);
            return;
        }
        else
        {
            return;
        }
    }

#if defined(TARGET_WIN32)
	if (file[1] == ':' && (file[2] == '\\' || file[2] == '/')) // absolute path for windows
	{
		strcpy(path, file);
		return;
	}
#endif
    
    HLApplication* psharedApp = HLApplication::sharedApplication();
    const char* languageStr = getLanguageStr(psharedApp->getCurrentLanguage());
    
    std::vector<std::string>::reverse_iterator itr = mFileSearchPaths.rbegin();
    for (; itr != mFileSearchPaths.rend(); ++itr)
    {
        strcpy(path, (*itr).c_str());
        strcat(path, file);
        if (access(path, 0) == 0)
        {
            return;
        }

        // localization path
        strcpy(path, (*itr).c_str());
        strcat(path, languageStr);
        strcat(path, ".lproj/");
        strcat(path, file);
        if (access(path, 0) == 0)
        {
            return;
        }
    }
    
#if defined(TARGET_ANDROID)
    if (fileInAppBundle(file))
    {
        strcpy(path, "apk://");
        strcat(path, file);
        return;
    }
    
    strcpy(path, languageStr);
    strcat(path, ".lproj/");
    strcat(path, file);
    if (fileInAppBundle(path))
    {
        int len = strlen(path);
        memmove(path+6, path, len+1);
        strncpy(path, "apk://", 6);
        return;
    }
#endif
    
    path[0] = '\0';
}

const char* HLResourceManager::getSysPathForFile(const char* file)
{
    static char path[MAX_PATH];
//    path[0] = '\0';
    memset(path, 0, MAX_PATH);
    
    if (file[0] == '/') // absolutepath
    {
        if (access(file, 0) == 0)
        {
            strcpy(path, file);
            return path;
        }
        else
        {
            return path;
        }
    }
    
    HLApplication* psharedApp = HLApplication::sharedApplication();
    const char* languageStr = getLanguageStr(psharedApp->getCurrentLanguage());
    
    std::vector<std::string>::reverse_iterator itr = mFileSearchPaths.rbegin();
    for (; itr != mFileSearchPaths.rend(); ++itr)
    {
        strcpy(path, (*itr).c_str());
        strcat(path, file);
        if (access(path, 0) == 0)
        {
            return path;
        }

        // localization path
        strcpy(path, (*itr).c_str());
        strcat(path, languageStr);
        strcat(path, ".lproj/");
        strcat(path, file);
        if (access(path, 0) == 0)
        {
            return path;
        }
    }
    
#if defined(TARGET_ANDROID)
    if (fileInAppBundle(file))
    {
        strcpy(path, "apk://");
        strcat(path, file);
        return path;
    }
    
    strcpy(path, languageStr);
    strcat(path, ".lproj/");
    strcat(path, file);
    if (fileInAppBundle(path))
    {
        int len = strlen(path);
        memmove(path+6, path, len+1);
        strncpy(path, "apk://", 6);
        return path;
    }
#endif
    
    path[0] = '\0';
    
    return path;
}

void HLResourceManager::setResourceScale(float scale)
{
    mResourceScale = scale;
}

unsigned long HLResourceManager::getResourceVersion()
{
    return mResourceVersion;
}

void HLResourceManager::addFileSearchPath(const char* path)
{
    std::string str = path;
    if (str[str.length()-1] != '/')
    {
        str += "/";
    }
    mFileSearchPaths.push_back(str);
}

const char* HLResourceManager::getWritablePath()
{
    return mWritablePath.c_str();
}

void HLResourceManager::initResourcePaths()
{
    initResourcePathsNative();
}

float HLResourceManager::getResourceScale()
{
#ifdef TARGET_IOS
#if ENABLE_RETINA_RESOURCE_SUFFIX
    return 1.0f;
#else
    return mResourceScale;
#endif
#else
    return mResourceScale;
#endif
}

bool HLResourceManager::hasResourcePackage(const char* package)
{
    return mResLoaders.find(package) != mResLoaders.end();
}

NS_HL_END
