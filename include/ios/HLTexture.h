//
//  HLTexture.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-25.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLTEXTURE_H__
#define __HLTEXTURE_H__

#include "HLObject.h"
#include "HLGL.h"
#include "HLGeometry.h"
#include "HLResourceManager.h"
#include <OpenThreads/Mutex>

NS_HL_BEGIN

typedef enum {
    //! 32-bit texture: RGBA8888
    kTexturePixelFormat_RGBA8888,
    //! 24-bit texture: RGB888
    kTexturePixelFormat_RGB888,
    //! 16-bit texture without Alpha channel
    kTexturePixelFormat_RGB565,
    //! 8-bit textures used as masks
    kTexturePixelFormat_A8,
    //! 8-bit intensity texture
    kTexturePixelFormat_I8,
    //! 16-bit textures used as masks
    kTexturePixelFormat_AI88,
    //! 16-bit textures: RGBA4444
    kTexturePixelFormat_RGBA4444,
    //! 16-bit textures: RGB5A1
    kTexturePixelFormat_RGB5A1,
    //! 4-bit PVRTC-compressed texture: PVRTC4
#ifdef GL_IMG_texture_compression_pvrtc
#if GL_IMG_texture_compression_pvrtc
    kTexturePixelFormat_PVRTC4,
    //! 2-bit PVRTC-compressed texture: PVRTC2
    kTexturePixelFormat_PVRTC2,
    kTexturePixelFormat_PVRTC4_NOALPHA,
    kTexturePixelFormat_PVRTC2_NOALPHA,
#endif
#endif
#ifdef GL_OES_compressed_ETC1_RGB8_texture
#if GL_OES_compressed_ETC1_RGB8_texture
    kTexturePixelFormat_ETC1_RGB8,
#endif
#endif
} HLTexturePixelFormat;

typedef enum {
    kTextureType_2D,
    kTextureType_CubeMap
} HLTextureType;

typedef struct {
	GLuint	minFilter;
	GLuint	magFilter;
	GLuint	wrapS;
	GLuint	wrapT;
} TexParams;

class HLImageCodec;

class HLTexture: public HLObject
{
    friend class HLDirector2D;
    friend void* imageloadingthread(void* p);
private:
    static std::map<std::string, HLTexture*> texCache;
    static std::vector<GLuint> texturesToRemove;
    
    static void glPurgeTextures();
    
    // there's a bug in ReadWriteMutex, so just use mutex
    static OpenThreads::Mutex mMutex;
    
    std::string mFile;
    GLuint mName;
    HLSize mImageSize;
    float mScale;
    HLTextureType mTextureType;
    bool mHasPremultipliedAlpha;
    
    GLuint mAlphaTex;
    
    bool mLoading{false};
    
    HLImageCodec* loadImageCodec(std::string file);
    bool loadWithFileAsync(std::string file);
    bool loadWithFile(const std::string& file);
    void loadWithImageCodecOnMainThread(HLObject*);
public:
    HLTexture():mTextureType(kTextureType_2D), mName(0), mHasPremultipliedAlpha(false), mAlphaTex(0) { }
    virtual ~HLTexture();
    bool loadWithRawData(const void* data, HLTexturePixelFormat pixelFormat, unsigned int pixelsWidth, unsigned int pixelsHeight, float scale);
    bool loadWithImageCodec(HLImageCodec* ic, GLuint& tex);
    bool loadWithCubeMap(std::string name, std::string ext);
    
    bool loading() {return mLoading;}
    
    void drawAtPoint(HLPoint& point);
    
    void drawInRect(const HLRect& rect, bool repeate = false);
    
    static HLTexture* getTexture(const std::string& file, bool asyncload = false);
    
    GLuint getName()
    {
        return mName;
    }
    
    GLuint getAlphaTexture()
    {
        return mAlphaTex;
    }
    
    HLSize getImageSize()
    {
        return mImageSize;
    }
    
    float getScale()
    {
        return mScale;
    }
    
    bool hasPremultipliedAlpha()
    {
        return mHasPremultipliedAlpha;
    }
    
    void loadExtraAlphaIfExsits();
    
    void setAliasTexParameters()
    {
        hlglBindTexture(GL_TEXTURE_2D, mName);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    }
    
    void setAntiAliasTexParameters()
    {
        hlglBindTexture(GL_TEXTURE_2D, mName);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    
    void setTexParameters(TexParams &texParams)
    {
        hlglBindTexture( GL_TEXTURE_2D, mName );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texParams.minFilter );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texParams.magFilter );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texParams.wrapS );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texParams.wrapT );
    }
};

NS_HL_END

#endif
