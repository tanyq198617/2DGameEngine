//
//  HLFileUtil.cpp
//  HoolaiEngine
//
//  Created by tyq on 12-9-13.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLFileUtil.h"

#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif
#include "GameEngineConfig.h"

NS_HL_BEGIN

HLFileData::HLFileData(const char* filename, const char* mode)
: buffer(0)
, size(0), position(0)
{
    FILE *fp = fopen(filename, mode);
    
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        if (strchr(mode, 'b'))
        {
            buffer = new unsigned char[size];
            size = fread(buffer, sizeof(unsigned char), size, fp);
        }
        else
        {
            buffer = new unsigned char[size+1];
            size = fread(buffer, sizeof(unsigned char), size, fp);
            buffer[size] = '\0';
        }
        fclose(fp);
    }
    else
    {
        // TODO error
    }
}

void HLFileData::read(void* dst, size_t size)
{
    if (position + size > this->size)
    {
        size = this->size - position;
    }
    memcpy(dst, &buffer[position], size);
    position += size;
}

int HLFileData::readLine(void* dst)
{
    if (position >= size)
    {
        return -1;
    }
    int i = position;
    for (; i<size&&buffer[i]!='\n'; i++);
    int len = i - position;
    if (buffer[i-1]=='\r')
    {
        --len;
    }
    memcpy(dst, &buffer[position], len);
    position = i+1;
    return len;
}

/*!
 Extract path from a file path.
 
 \param[in] filepath The file path to use to extract the path.
 \param[in,out] path The return path.
 */
void get_file_path(const char *filepath, char *path)
{
	const char *t = NULL;
	
	unsigned int p;
    
#ifdef TARGET_WIN32
	strcpy(path, filepath);
	for (char* t1 = path;*t1!='\0';++t1)
	{
		if (*t1 == '\\')
		{
			*t1 = '/';
		}
	}
	filepath = (const char*)path;
#endif
	t = strrchr(filepath, '/');
	
	if(t)
	{
		p = (t - filepath) + 1;
		strncpy(path, filepath, p);
		path[ p ] = 0;
	}
	else
	{
		path[0] = '\0';
	}
}

void get_full_path(const char* file, char *fullpath)
{
#ifdef TARGET_WIN32
	if (file[1]==':'&&(file[2]=='\\'||file[2]=='/'))
	{
		strcpy(fullpath, file);
		return;
	}
#else
    if (file[0] == '/')
    {
        strcpy(fullpath, file);
        return;
    }
#endif
    strcpy(fullpath, getenv("FILESYSTEM"));
    strcat(fullpath, file);
#if ENABLE_RETINA_RESOURCE_SUFFIX
    char ext[10];
    char * location = strrchr(fullpath, '.');
    if (!location)
    {
        return;
    }
    strcpy(ext, location);

    if (CONTENT_SCALE_FACTOR == 2)
    {
        sprintf(location, "@2x%s", ext);
        if (access(fullpath, 0) == 0)
        {
            return;
        }
        strcpy(location, ext);
    }
    else
    {
        if (access(fullpath, 0) == 0)
        {
            return;
        }
        sprintf(location, "@2x%s", ext);
    }
#endif
}

float get_file_scale(const char* file)
{
#if ENABLE_RETINA_RESOURCE_SUFFIX
    char fullpath[PATH_MAX];
    get_full_path(file, fullpath);
    for (int i = strlen(fullpath) - 1; i > 3; i--)
    {
        if (fullpath[i] == '.')
        {
            if (fullpath[i - 1] == 'x' && fullpath[i - 2] == '2' && fullpath[i - 3] == '@')
                return 2.f;
            else
                return 1.f;
        }
    }
#endif
    return 1.f;
}

void get_full_path_from_relative_file(const char* file, const char* relativefilepath, char* fullpath)
{
    get_file_path(relativefilepath, fullpath);
    strcat(fullpath, file);
}


/*!
 Extract the file name from a file path.
 
 \param[in] filepath The file path to use to extract the file name.
 \param[in,out] filename The file name.
 
 */
void get_file_name(const char *filepath, char *filename)
{
	const char *t = NULL;
	
	t = strrchr(filepath, '/');
	
	if(t) strcpy(filename, t + 1);
	else strcpy(filename, filepath);
}

/*!
 Extract the extension of a file. And optionally convert it to uppercase.
 
 \param[in] filepath The file path to use.
 \param[in,out] ext If a valid extension is found this variable will be used as destination to store it.
 \param[in] uppercase Determine wheter or not ext should be converted to uppercase.
 */
void get_file_extension(const char *filepath, char *ext, unsigned char uppercase)
{
	const char *t = NULL;
	
	t = strrchr(filepath, '.');
	
	if(t) strcpy(ext, t + 1);
	
	if(uppercase)
	{
		unsigned int i = 0,
        l = strlen(ext);
		
		while(i != l)
		{
			ext[ i ] = toupper(ext[ i ]);
			++i;
		}
	}
}

NS_HL_END
