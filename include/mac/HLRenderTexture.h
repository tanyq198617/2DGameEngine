//
//  HLRenderTexture.h
//  HoolaiEngine
//
//  Created by tyq on 13-3-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLRENDERTEXTURE_H__
#define __HLRENDERTEXTURE_H__

#include "HLTexture.h"
#include "HLSpriteFrame.h"

NS_HL_BEGIN

namespace gui
{
    class HLView;
}

class HLRenderTexture
{
public:
    HLRenderTexture(int pixelWidth, int pixelHeight, HLTexturePixelFormat eFormat = kTexturePixelFormat_RGBA8888, GLuint depthStencilFormat = 0);
    HLRenderTexture(HLSpriteFrame* spf, HLTexturePixelFormat eFormat = kTexturePixelFormat_RGBA8888, GLuint depthStencilFormat = 0);
    ~HLRenderTexture();
    void beginWith2DProjection(float w = 0, float h = 0, bool newMVMat = true);
    void clearColor(float r, float g, float b, float a);
    void clearDepth(float depthValue);
    void end();
    void renderView(gui::HLView* view);
    HLSpriteFrame* getSpriteFrame() {return mSpriteFrame;}
    void saveToFile(const char* filename);
private:
    GLuint mFBO;
    GLuint mDepthRenderBuffer;
    GLint mOldFBO;
    HLSpriteFrame* mSpriteFrame;
    HLTexture* mTextureCopy;
    HLTexturePixelFormat mPixelFormat;
    int mPixelWidth;
    int mPixelHeight;
    int mBytesPerPixel;
    bool mRestoreMVMat{true};
};

NS_HL_END

#endif 
