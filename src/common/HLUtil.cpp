//
//  HLUtil.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-7.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#include "HLUtil.h"
#include <zlib.h>
#include "HLFileUtil.h"
#include "HLGeometry.h"
#include <stdint.h>
#include <pthread.h>

extern pthread_t mainthread;

NS_HL_BEGIN

bool currentIsMainThread()
{
    return pthread_equal(mainthread, pthread_self());
}

const char* getLanguageStr(HLLanguageType type)
{
    static const char* strs[] =
    {
        "en", "zh-Hant", "zh-Hans", "fr",
        "it", "de", "es", "ru", "ko", "ja",
        "hu", "pt", "ar", "tr", "pl"
    };
    if (type < 15)
    {
        return strs[type];
    }
    
    return "unknown";
}

NS_HL_END

float clampf(float value, float min_inclusive, float max_inclusive)
{
    if (min_inclusive > max_inclusive) {
        float tmp = min_inclusive;
        min_inclusive = max_inclusive;
        max_inclusive = tmp;
    }
    return value < min_inclusive ? min_inclusive : value < max_inclusive? value : max_inclusive;
}

struct CCZHeader {
    uint8_t            sig[4];                // signature. Should be 'CCZ!' 4 bytes
    uint16_t            compression_type;    // should 0
    uint16_t            version;            // should be 2 (although version type==1 is also supported)
    uint32_t             reserved;            // Reserverd for users.
    uint32_t            len;                // size of the uncompressed file
};

enum {
    CCZ_COMPRESSION_ZLIB,                // zlib format.
    CCZ_COMPRESSION_BZIP2,                // bzip2 format (not supported yet)
    CCZ_COMPRESSION_GZIP,                // gzip format (not supported yet)
    CCZ_COMPRESSION_NONE,                // plain (not supported yet)
};

//int inflateGZipFile(hoolai::HLFileData *data, unsigned char **out)
//{
//    int len;
//    unsigned int offset = 0;
//    
//    HLASSERT(out, "");
//    HLASSERT(&*out, "");
//    
//    gzFile inFile = gzopen(path, "rb");
//    if( inFile == NULL ) {
//        HLLOG("ZipUtils: error open gzip file: %s", path);
//        return 0;
//    }
//    
//    /* 512k initial decompress buffer */
//    unsigned int bufferSize = 512 * 1024;
//    unsigned int totalBufferSize = bufferSize;
//    
//    *out = (unsigned char*)malloc( bufferSize );
//    if( ! out ) 
//    {
//        HLLOG("ZipUtils: out of memory %s", path);
//        return 0;
//    }
//    
//    for (;;) {
//        len = gzread(inFile, *out + offset, bufferSize);
//        if (len < 0) 
//        {
//            HLLOG("ZipUtils: error in gzread %s", path);
//            free( *out );
//            *out = NULL;
//            return -1;
//        }
//        if (len == 0)
//        {
//            break;
//        }
//        
//        offset += len;
//        
//        // finish reading the file
//        if( (unsigned int)len < bufferSize )
//        {
//            break;
//        }
//        
//        bufferSize *= 2;
//        totalBufferSize += bufferSize;
//        unsigned char *tmp = (unsigned char*)realloc(*out, totalBufferSize );
//        
//        if( ! tmp ) 
//        {
//            HLLOG("ZipUtils: out of memory %s", path);
//            free( *out );
//            *out = NULL;
//            return 0;
//        }
//        
//        *out = tmp;
//    }
//    
//    if (gzclose(inFile) != Z_OK)
//    {
//        HLLOG("ZipUtils: gzclose failed %s", path);
//    }
//    
//    return offset;
//}

int inflateCCZFile(hoolai::HLFileData *data, unsigned char **out)
{
    HLASSERT(out, "");
    HLASSERT(&*out, "");
    
    // load file into memory
    unsigned char* compressed = NULL;
    
    unsigned long fileLen = 0;
//    char fullPath[MAX_PATH];
//    hoolai::get_full_path(path, fullPath);
//    hoolai::HLFileData data(fullPath, "rb");
    compressed = data->buffer;
    fileLen = data->size;
    
    if(NULL == compressed || 0 == fileLen) 
    {
        HLLOG("Error loading CCZ compressed file");
        return 0;
    }
    
    struct CCZHeader *header = (struct CCZHeader*) compressed;
    
    // verify header
    if( header->sig[0] != 'C' || header->sig[1] != 'C' || header->sig[2] != 'Z' || header->sig[3] != '!' ) 
    {
        HLLOG("Invalid CCZ file");
        return 0;
    }
    
    // verify header version
    BigEndianToNative<sizeof(header->version)>(&header->version);
//    unsigned int version = SWAP_INT16_BIG_TO_HOST(  );
    if( header->version > 2 )
    {
        HLLOG("Unsupported CCZ header format");
        return 0;
    }
    
    // verify compression format
    BigEndianToNative<sizeof(header->compression_type)>(&header->compression_type);
    if( header->compression_type != CCZ_COMPRESSION_ZLIB )
    {
        HLLOG("CCZ Unsupported compression method");
        return 0;
    }
    
    BigEndianToNative<sizeof(header->len)>(&header->len);
    
    *out = new unsigned char[header->len];
    if(! *out )
    {
        HLLOG("CCZ: Failed to allocate memory for texture");
        return 0;
    }
    
    
    unsigned long destlen = header->len;
    unsigned long source = (unsigned long) compressed + sizeof(*header);
    int ret = uncompress(*out, &destlen, (Bytef*)source, fileLen - sizeof(*header) );
    
    if( ret != Z_OK )
    {
        HLLOG("CCZ: Failed to uncompress data");
        free( *out );
        *out = NULL;
        return 0;
    }
    
    return header->len;
}

unsigned long nextPOT(unsigned long x)
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >>16);
    return x + 1;
}

void EndianSwitch(int size, void* p)
{
    uint8_t* bytes = static_cast<uint8_t*>(p);
    for (int i = 0; i < size/2; ++i)
    {
        std::swap(bytes[i], bytes[size-i-1]);
    }
    
}

int pnpoly(int nvert, hoolai::HLPoint *poly, hoolai::HLPoint test)
{
    int i, j, c = 0;
    for (i = 0, j = nvert-1; i < nvert; j = i++) {
        if ( ((poly[i].y>test.y) != (poly[j].y>test.y)) &&
            (test.x < (poly[j].x-poly[i].x) * (test.y-poly[i].y) / (poly[j].y-poly[i].y) + poly[i].x) )
            c = !c;
    }
    return c;
}

/*
#ifdef TARGET_WIN32
float roundf(float x)
{
	return floorf(x+0.5);
}
#endif
*/
