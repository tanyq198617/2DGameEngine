//
//  HLHttpDownloadManager.cpp
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-8-9.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLHttpDownloadManager.h"
#include "HLResourceManager.h"
#include "HLProfileUtil.h"
#include "HLStringUtil.h"
#include "HLAutoreleasePool.h"
//#include <sys/xattr.h>
//#include <sys/stat.h>
#include "HLObject.h"
#include <pthread.h>
#if defined(TARGET_ANDROID) || defined(TARGET_WIN32)
#include "curl/curl.h"
#endif
#include <string.h>
#if defined(TARGET_MACOS) || defined(TARGET_IOS)
#include "semaphore_darwin.h"
#include "curl/curl.h"
#else
#include <semaphore.h>
#endif
#if defined(TARGET_WIN32)
#define strncasecmp strnicmp
#include <io.h>
#else
#include <unistd.h>
#endif

#define TIME_OUT 60

NS_HL_BEGIN

class HLHttpDownloadConnectionDelegate;

void* startAsyncDownload(void* p);

class HLHttpDownloadCallbackObject: public HLObject
{
public:
    std::string path;
    float progress;
    float speed;
};

class HLHttpDownloadConnectionDelegate : public HLObject
{
public:
    std::map<std::string, std::string> headers;
    std::string path;
    std::string url;
    FILE* fp;
    CDelegate3<std::string, float, float> delegate;
    size_t totalSize;
    size_t currentSize;
    size_t downloaded;
    HLTimeProfiler profiler;
    
public:
    HLHttpDownloadConnectionDelegate():fp(NULL),totalSize(0),currentSize(0),downloaded(0)
    {
        curl_global_init(CURL_GLOBAL_ALL);
    }
    
    void init(std::string _url, std::string _path, HTTPDownloadListener listener)
    {
        url = _url;
        
        if(_path.length())
        {
            path = _path;
        }
        else
        {
            std::string::size_type pos = _url.rfind('/');
            path = _url.substr(pos+1);
        }
        if (path[0] != '/')
        {
            path = HLResourceManager::getSingleton()->getWritablePath() + path;
        }
        path += ".tmp";
        
        delegate = listener;
        
        pthread_t threadid;
        int ret = pthread_create(&threadid, NULL, startAsyncDownload, this);
        if (-1 == ret)
        {
            printf("Error on creating thread!\n");
            abort();
        }
    }
    
    void updateDownloadProgress(HLObject* param)
    {
        HLHttpDownloadCallbackObject* object = (HLHttpDownloadCallbackObject*)param;
        delegate(object->path, object->progress, object->speed);
    }
};

void set_share_handle(CURL* curl_handle);

size_t header_parser(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    HLHttpDownloadConnectionDelegate* downloadDelegate = (HLHttpDownloadConnectionDelegate*)userdata;
    
    std::map<std::string, std::string> &headers = downloadDelegate->headers;
    std::string header((char*)ptr, 0, size * nmemb);
    std::string::size_type pos = header.find(':');
    if (pos != std::string::npos)
    {
        std::string name = header.substr(0, pos);
        std::string value = header.substr(pos+1);
        StringUtil::Trim(value);
        headers.insert(std::make_pair(name, value));
    }
    return size * nmemb;
}

size_t download_file(void *buffer, size_t size, size_t nmemb, void *userp)
{
    HLHttpDownloadConnectionDelegate* downloadDelegate = (HLHttpDownloadConnectionDelegate*)userp;
    if (downloadDelegate->totalSize == 0)
    {
        downloadDelegate->totalSize = atoll(downloadDelegate->headers["Content-Length"].c_str());
    }
    if (!downloadDelegate->fp)
    {
        downloadDelegate->totalSize = atoll(downloadDelegate->headers["Content-Length"].c_str());
        downloadDelegate->currentSize = 0;
        downloadDelegate->downloaded = 0;
        std::string contentRange = downloadDelegate->headers["Content-Range"];
        std::string lastModified = downloadDelegate->headers["Last-Modified"];
        
        if (!strncasecmp(contentRange.c_str(), "bytes */", 8))
        {
            std::string path1 = downloadDelegate->path.substr(0, downloadDelegate->path.length()-4);
            if (access(path1.c_str(), 0) == 0)
            {
                remove(path1.c_str());
            }
            rename(downloadDelegate->path.c_str(), path1.c_str());
            return 0;
        }
        
        if (lastModified.length())
        {
            std::string infopath = downloadDelegate->path+".info";
            if (access(downloadDelegate->path.c_str(), 0) == 0 && access(infopath.c_str(), 0) == 0)
            {// 对比未下载完的文件的版本
                HLFileData* file = new HLFileData(infopath.c_str(), "rb");
                if (lastModified.length() != file->size || memcmp(file->buffer, lastModified.c_str(), file->size)) // 重新下载
                {
                    downloadDelegate->fp = fopen(downloadDelegate->path.c_str(), "wb");
                    delete file;
                    downloadDelegate->totalSize = 0;
                    
                    FILE *fp = fopen((downloadDelegate->path+".info").c_str(), "wb");
                    fwrite(lastModified.c_str(), 1, lastModified.length(), fp);
                    fclose(fp);
                    return 0;
                }
                delete file;
            }
            else if (access(downloadDelegate->path.substr(0, downloadDelegate->path.length()-4).c_str(), 0) == 0) // 已存在下载完的文件
            {
                HLFileData* file = new HLFileData(infopath.c_str(), "rb");
                if (lastModified.length() != file->size || memcmp(file->buffer, lastModified.c_str(), file->size))
                {
                    downloadDelegate->fp = fopen(downloadDelegate->path.c_str(), "wb");
                    remove(downloadDelegate->path.substr(0, downloadDelegate->path.length()-4).c_str()); // 需要重新下载
                    delete file;
                    downloadDelegate->totalSize = 0;
                    
                    FILE *fp = fopen((downloadDelegate->path+".info").c_str(), "wb");
                    fwrite(lastModified.c_str(), 1, lastModified.length(), fp);
                    fclose(fp);
                    return 0;
                }
                else // 不需要下载
                {
                    delete file;
                    return 0;
                }
            }
            
            if (contentRange.length() > 0)
            {
                int location = -1;
                int length = 0;
                int i = 0;
                for (i = 0; i < contentRange.length(); ++i)
                {
                    char c = contentRange[i];
                    if (c >= '0' && c <= '9')
                    {
                        location = i;
                        break;
                    }
                }
                for (; i < contentRange.length(); ++i)
                {
                    char c = contentRange[i];
                    if (c < '0' || c > '9')
                    {
                        length = i - location;
                        break;
                    }
                }
                if (location != -1 && length > 0)
                {
                    downloadDelegate->currentSize = atoll(contentRange.substr(location, length).c_str());
                    if (downloadDelegate->currentSize > 0)
                    {
                        std::string::size_type pos = contentRange.find('/');
                        std::string total = contentRange.substr(pos+1);
                        downloadDelegate->totalSize = atoll(total.c_str());
                    }
                }
                downloadDelegate->fp = fopen(downloadDelegate->path.c_str(), "ab"); // 续传
            }
            else
            {
                downloadDelegate->fp = fopen(downloadDelegate->path.c_str(), "wb"); // 全新下载
            }
            
            FILE *fp = fopen((downloadDelegate->path+".info").c_str(), "wb");
            fwrite(lastModified.c_str(), 1, lastModified.length(), fp);
            fclose(fp);
        }
        
        if (!downloadDelegate->fp)
        {
            downloadDelegate->fp = fopen(downloadDelegate->path.c_str(), "wb");
        }
    }
    
    fwrite(buffer, size, nmemb, downloadDelegate->fp);
    size_t segsize = size * nmemb;
    downloadDelegate->downloaded += segsize;
    downloadDelegate->currentSize += segsize;
    
    if (downloadDelegate->currentSize < downloadDelegate->totalSize)
    {
        HLHttpDownloadCallbackObject* param = new HLHttpDownloadCallbackObject();
        param->path = downloadDelegate->path;
        param->progress = (downloadDelegate->currentSize)/(float)downloadDelegate->totalSize;
        param->speed = downloadDelegate->downloaded/downloadDelegate->profiler.elapsed();
        downloadDelegate->performFuncOnMainThread((CallFuncO)&HLHttpDownloadConnectionDelegate::updateDownloadProgress, param);
        param->release();
    }
    
    return segsize;
}

void* startAsyncDownload(void* p)
{
    HLAutoreleasePool pool;
    
    HLHttpDownloadConnectionDelegate* downloadDelegate = (HLHttpDownloadConnectionDelegate*)p;
    
    if (downloadDelegate->path.empty() || downloadDelegate->url.empty())
    {
        return NULL;
    }
    
    size_t start_pos = 0;
    
    if (access(downloadDelegate->path.c_str(), 0) == 0)
    {
        FILE *fp = fopen(downloadDelegate->path.c_str(), "rb");
        fseek(fp, 0, SEEK_END);
        start_pos = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        fclose(fp);
    }
    
    CURL *curl = curl_easy_init();
    set_share_handle(curl);
    
    curl_easy_setopt(curl, CURLOPT_URL, downloadDelegate->url.c_str());
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, TIME_OUT);
//    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); // Need test
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_parser);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, downloadDelegate);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_file);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, downloadDelegate);
    if (start_pos)
    {
        struct curl_slist* cHeaders = curl_slist_append(NULL, StringUtil::Format("Range: bytes=%ld-", start_pos).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, cHeaders);
    }
    
    downloadDelegate->profiler.reset();
    CURLcode ret = curl_easy_perform(curl);
    HLLOG("CURL_Error_Code===>%d",ret);
    HLLOG("CURL_currentSize==%d=====totalSize==%d",downloadDelegate->currentSize,downloadDelegate->totalSize);
    long retcode = 0;
    if (ret == CURLE_OK)
    {
        HLLOG("CURLE_OK_ErrorCode_A==>%d",ret);
        ret = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
    }
    if (ret == CURLE_OK && retcode != 404 && downloadDelegate->currentSize == downloadDelegate->totalSize)
    {
        HLLOG("CURLE_OK_ErrorCode_B==>%d",ret);
        fclose(downloadDelegate->fp);
        downloadDelegate->fp = NULL;
        std::string path1 = downloadDelegate->path.substr(0, downloadDelegate->path.length()-4);
        if (access(path1.c_str(), 0) == 0)
        {
            remove(path1.c_str());
        }
        HLHttpDownloadCallbackObject* param = new HLHttpDownloadCallbackObject();
        if (rename(downloadDelegate->path.c_str(), path1.c_str()) < 0)
        {
            param->path = "[HLHttpDownloadManager]error: rename file failed";
            param->progress = -1;
            param->speed = 0;
            param->autorelease();
        }
        else
        {
            param->path = downloadDelegate->path;
            param->progress = 1;
            param->speed = downloadDelegate->downloaded/downloadDelegate->profiler.elapsed();
            param->autorelease();
        }
        downloadDelegate->performFuncOnMainThread((CallFuncO)&HLHttpDownloadConnectionDelegate::updateDownloadProgress, param);
        curl_easy_cleanup(curl);
    }
    else if (ret == CURLE_WRITE_ERROR)
    {
        if (downloadDelegate->fp) // 重新下载
        {
            HLLOG("CURLE_WRITE_ERROR_ErrorCode_A==>%d",ret);
            curl_easy_cleanup(curl);
            
            curl = curl_easy_init();
            set_share_handle(curl);
            
            curl_easy_setopt(curl, CURLOPT_URL, downloadDelegate->url.c_str());
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, TIME_OUT);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, downloadDelegate);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_file);
            
            downloadDelegate->profiler.reset();
            
            if (curl_easy_perform(curl) == CURLE_OK)
            {
                fclose(downloadDelegate->fp);
                downloadDelegate->fp = NULL;
                std::string path1 = downloadDelegate->path.substr(0, downloadDelegate->path.length()-4);
                if (access(path1.c_str(), 0) == 0)
                {
                    remove(path1.c_str());
                }
                rename(downloadDelegate->path.c_str(), path1.c_str());
                
                HLHttpDownloadCallbackObject* param = new HLHttpDownloadCallbackObject();
                param->path = downloadDelegate->path;
                param->progress = 1;
                param->speed = downloadDelegate->downloaded/downloadDelegate->profiler.elapsed();
                param->autorelease();
                downloadDelegate->performFuncOnMainThread((CallFuncO)&HLHttpDownloadConnectionDelegate::updateDownloadProgress, param);
                curl_easy_cleanup(curl);
            }
            else
            {
                HLLOG("CURLE_WRITE_ERROR_ErrorCode_B==>%d",ret);
                fclose(downloadDelegate->fp);
                downloadDelegate->fp = NULL;
                HLHttpDownloadCallbackObject* param = new HLHttpDownloadCallbackObject();
                param->path = "network error";
                HLLOG("CURL_network error_ErrorCode_A==>%d",ret);
                param->progress = -1;
                param->speed = -1;
                param->autorelease();
                downloadDelegate->performFuncOnMainThread((CallFuncO)&HLHttpDownloadConnectionDelegate::updateDownloadProgress, param);
                curl_easy_cleanup(curl);
            }
        }
        else
        {
            HLHttpDownloadCallbackObject* param = new HLHttpDownloadCallbackObject();
            param->path = downloadDelegate->path;
            param->progress = 1;
            param->speed = downloadDelegate->downloaded/downloadDelegate->profiler.elapsed();
            param->autorelease();
            downloadDelegate->performFuncOnMainThread((CallFuncO)&HLHttpDownloadConnectionDelegate::updateDownloadProgress, param);
            curl_easy_cleanup(curl);
        }
    }
    else
    {
//        if (access(downloadDelegate->path.c_str(), 0) == 0)
//        {
//            remove(downloadDelegate->path.c_str());
//        }
        HLLOG("CURLE_WRITE_ERROR_ErrorCode_C==>%d",ret);
        HLHttpDownloadCallbackObject* param = new HLHttpDownloadCallbackObject();
        if (retcode == 404) {
            param->path = "404 file not found";
        } else {
            param->path = "network error";
            HLLOG("CURL_network error_ErrorCode_B==>%d",ret);
        }
        param->progress = -1;
        param->speed = -1;
        param->autorelease();
        downloadDelegate->performFuncOnMainThread((CallFuncO)&HLHttpDownloadConnectionDelegate::updateDownloadProgress, param);
        curl_easy_cleanup(curl);
    }
    
    if (downloadDelegate->fp)
    {
        fclose( downloadDelegate->fp);
    }
    
    HLLOG("download finished");
    downloadDelegate->release();
    return NULL;
}

void HLHttpDownloadManager::addDownloadTask(std::string url, std::string path, HTTPDownloadListener listener)
{
    HLHttpDownloadConnectionDelegate* downLoadConn =  new HLHttpDownloadConnectionDelegate();
    downLoadConn->init(url, path, listener);
}

NS_HL_END