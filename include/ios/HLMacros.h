//
//  HLMacros.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-5-4.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLMACROS_H__
#define __HLMACROS_H__

#include "GameEngineConfig.h"

#define SWAP(x, y, type)    \
{    type temp = (x);        \
    x = y; y = temp;        \
}

#define RANDOM_MINUS1_1() ((2.0f*((float)rand()/RAND_MAX))-1.0f)

#define RANDOM_0_1() ((float)rand()/RAND_MAX)

#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f)

#define RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f)

#define HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 
//#define SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
//#define SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)   
//#define SWAP_INT32_LITTLE_TO_HOST(i) ((HOST_IS_BIG_ENDIAN == true)? SWAP32(i) : (i) )
//#define SWAP_INT16_LITTLE_TO_HOST(i) ((HOST_IS_BIG_ENDIAN == true)? SWAP16(i) : (i) )
//#define SWAP_INT32_BIG_TO_HOST(i)    ((HOST_IS_BIG_ENDIAN == true)? (i) : SWAP32(i) )
//#define SWAP_INT16_BIG_TO_HOST(i)    ((HOST_IS_BIG_ENDIAN == true)? (i):  SWAP16(i) )

#define NS_HL_BEGIN                     namespace hoolai {
#define NS_HL_END                       }
#define USING_NS_HL                     using namespace hoolai

#ifdef DEBUG
#if defined(TARGET_ANDROID)
#include <android/log.h>
#define HLLOG(...)  __android_log_print(ANDROID_LOG_DEBUG,"Hoolai",__VA_ARGS__)
#elif defined(TARGET_WIN32)
void log_win32(const char*, ...);
#define HLLOG(format, ...)      log_win32(format, ##__VA_ARGS__)
#else
#define HLLOG(format, ...)      {printf("Hoolai: "); printf(format, ##__VA_ARGS__); printf("\n"); fflush(stdout);}
#endif
#define HLASSERT(cond, msg, ...)     if(!(cond)){printf(msg, ##__VA_ARGS__);abort();}
#else
#define HLLOG(format, ...)
#define HLASSERT(cond, msg, ...)
#endif

#ifndef FLT_EPSILON
#define FLT_EPSILON     1.192092896e-07F
#endif // FLT_EPSILON

#ifndef TARGET_IOS
#define CONTENT_SCALE_FACTOR 1
#else
#if GAME_PROJECTION == PROJECTION_2D
#define CONTENT_SCALE_FACTOR hoolai::HLDirector2D::getSingleton()->getContentScaleFactor()
#else
#define CONTENT_SCALE_FACTOR hoolai::HLDirector3D::getSingleton()->getContentScaleFactor()
#endif
#endif

#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif

#ifndef MAX
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif

#ifndef HL_SPRITE_DEBUG_DRAW
#define HL_SPRITE_DEBUG_DRAW 0
#endif

#define MAX_CHAR	64
#ifndef MAX_PATH
#ifdef PATH_MAX
#define MAX_PATH PATH_MAX
#else
#define MAX_PATH	512
#endif
#endif

#ifdef TARGET_WIN32
#if !defined(NAN)
#define NAN std::numeric_limits<float>::quiet_NaN()
#endif
#if !defined(isnan)
#define isnan   _isnan
#endif
#endif

#ifdef DEBUG
#define CHECK_GL_ERROR() { GLenum __error = glGetError(); if(__error) HLLOG("OpenGL error 0x%04X in file %s function %s line %d\n", __error, __FILE__, __FUNCTION__, __LINE__); }
#else
#define CHECK_GL_ERROR()
#endif

#define JSBool bool
#define JS_TRUE true
#define JS_FALSE false

#endif
