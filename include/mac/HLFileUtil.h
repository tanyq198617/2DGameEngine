//
//  HLFileUtil.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-26.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLFILEUTIL_H__
#define __HLFILEUTIL_H__

#include "HLMacros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

NS_HL_BEGIN

class HLFileData
{
private:
    unsigned int position;
public:
    unsigned char* buffer;
    unsigned long size;
public:
    HLFileData(const char* filename, const char* mode);
    
    HLFileData()
    : buffer(0)
    , size(0), position(0)
    {}
    
    ~HLFileData()
    {
        if (buffer)
        {
            delete buffer;
        }
    }
    void seek(unsigned int pos)
    {
        position = pos;
    }
    void read(void* dst, size_t size);
    int readLine(void* dst);
};

void get_file_path(const char *filepath, char *path);
void get_full_path(const char* file, char *fullpath);
float get_file_scale(const char* file);
void get_full_path_from_relative_file(const char* file, const char* relativefilepath, char* fullpath);
void get_file_name(const char *filepath, char *filename);
void get_file_extension(const char *filepath, char *ext, unsigned char uppercase);



NS_HL_END

#endif
