//
//  HLRenderTexture.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-3-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLRenderTexture.h"
#include "HLDirector2D.h"
#include "HLGLConfiguration.h"
#include "HLGLMatrixStack.h"
#include "HLView.h"
#include "HLResourceManager.h"
#include "png.h"
#include "webp/encode.h"

NS_HL_BEGIN

HLRenderTexture::HLRenderTexture(int pixelWidth, int pixelHeight, HLTexturePixelFormat eFormat, GLuint depthStencilFormat)
{
    float scale = HLDirector2D::getSingleton()->getWinSizeScale();
    
    int powW = 0;
    int powH = 0;
    
    void * data = NULL;
    
    if (HLGLConfiguration::getSingleton()->isSupportsNPOT())
    {
        powW = pixelWidth;
        powH = pixelHeight;
    }
    else
    {
        powW = (int)nextPOT(pixelWidth);
        powH = (int)nextPOT(pixelHeight);
    }
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mOldFBO);
    
    int bytesPerPixel = 4;
    switch (eFormat)
    {
        case kTexturePixelFormat_RGB888:
            bytesPerPixel = 3;
            break;
        case kTexturePixelFormat_RGB565:
        case kTexturePixelFormat_RGBA4444:
        case kTexturePixelFormat_AI88:
            bytesPerPixel = 2;
            break;
        case kTexturePixelFormat_A8:
        case kTexturePixelFormat_I8:
            bytesPerPixel = 1;
            break;
        default:
            break;
    }
    mBytesPerPixel = bytesPerPixel;
    mPixelFormat = eFormat;
    
    data = malloc(powW * powH * bytesPerPixel);
    memset(data, 0, powW * powH * bytesPerPixel);
    
    mPixelWidth = powW;
    mPixelHeight = powH;
    
    HLTexture* tex = new HLTexture();
    tex->loadWithRawData(data, eFormat, powW, powH, 1/scale);
    mSpriteFrame = new HLSpriteFrame();
    mSpriteFrame->_texture = tex;
    mSpriteFrame->_sourceSize = HLSize(pixelWidth, pixelHeight);
    mSpriteFrame->_offset = HLPointZero;
    mSpriteFrame->_rotated = false;
    mSpriteFrame->_textureRect.origin = HLPointZero;
    mSpriteFrame->_textureRect.size = HLSize(pixelWidth, pixelHeight);
    
    GLint oldRBO;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRBO);
    
    if (HLGLConfiguration::getSingleton()->checkForGLExtension("GL_QCOM"))
    {
        mTextureCopy = new HLTexture();
        mTextureCopy->loadWithRawData(data, eFormat, powW, powH, 1/scale);
    }
    else
    {
        mTextureCopy = NULL;
    }
    
    // generate FBO
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    
    // associate texture with FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSpriteFrame->_texture->getName(), 0);
    
    mDepthRenderBuffer = 0;
    if (depthStencilFormat)
    {
        glGenRenderbuffers(1, &mDepthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, (GLenum)depthStencilFormat, (GLsizei)powW, (GLsizei)powH);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer);
        // if depth format is the one with stencil part, bind same render buffer as stencil attachment
        if (depthStencilFormat == GL_DEPTH24_STENCIL8)
        {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer);
        }
    }

    
//    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//    {
//        printf("Could not attach texture to framebuffer\n");
//    }
    
    mSpriteFrame->_texture->setAliasTexParameters();
    
    glBindRenderbuffer(GL_RENDERBUFFER, oldRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mOldFBO);
    
    CHECK_GL_ERROR();
    
    free(data);
}

HLRenderTexture::HLRenderTexture(HLSpriteFrame* spf, HLTexturePixelFormat eFormat, GLuint depthStencilFormat)
{
    mSpriteFrame = spf;
    mSpriteFrame->retain();
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mOldFBO);
    
    GLint oldRBO;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRBO);
    
    int powW = mSpriteFrame->getTexture()->getImageSize().width * mSpriteFrame->getTexture()->getScale();
    int powH = mSpriteFrame->getTexture()->getImageSize().height * mSpriteFrame->getTexture()->getScale();
    
    if (HLGLConfiguration::getSingleton()->checkForGLExtension("GL_QCOM"))
    {
        void* data = malloc(powW * powH * 3);
        memset(data, 0, powW * powH * 3);
        mTextureCopy = new HLTexture();
        mTextureCopy->loadWithRawData(data, eFormat, powW, powH, 1/mSpriteFrame->getTexture()->getScale());
        free(data);
    }
    else
    {
        mTextureCopy = NULL;
    }
    
    // generate FBO
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    
    // associate texture with FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSpriteFrame->_texture->getName(), 0);
    
    mDepthRenderBuffer = 0;
    if (depthStencilFormat)
    {
        glGenRenderbuffers(1, &mDepthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, (GLenum)depthStencilFormat, (GLsizei)powW, (GLsizei)powH);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer);
        // if depth format is the one with stencil part, bind same render buffer as stencil attachment
        if (depthStencilFormat == GL_DEPTH24_STENCIL8)
        {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer);
        }
    }
    
//    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//    {
//        printf("Could not attach texture to framebuffer\n");
//    }
    
    mSpriteFrame->_texture->setAliasTexParameters();
    
    glBindRenderbuffer(GL_RENDERBUFFER, oldRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mOldFBO);
    
    CHECK_GL_ERROR();
}

HLRenderTexture::~HLRenderTexture()
{
    if (mSpriteFrame)
    {
        mSpriteFrame->release();
    }
    if (mTextureCopy)
    {
        mTextureCopy->release();
    }
    glDeleteFramebuffers(1, &mFBO);
    if (mDepthRenderBuffer)
    {
        glDeleteFramebuffers(1, &mDepthRenderBuffer);
    }
}

void HLRenderTexture::beginWith2DProjection(float w, float h, bool newMVMat)
{
    HLDirector2D* director = HLDirector2D::getSingleton();
    float winScale = director->getWinSizeScale();
//    HLSize winSize = director->getWinSize();
    HLSize texSize = mSpriteFrame->_texture->getImageSize()*mSpriteFrame->_texture->getScale();
//    float widthRatio = winSize.width / texSize.width;
//    float heightRatio = winSize.height / texSize.height;
    
    glViewport(0, 0, GLsizei(texSize.width), GLsizei(texSize.height));
    
    HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
    
    matrixStack->matrixMode(KM_GL_PROJECTION);
    matrixStack->pushMatrix();
    matrixStack->loadIdentity();
    kmMat4 orthoMatrix;
    kmMat4OrthographicProjection(&orthoMatrix, 0, w==0?texSize.width*winScale:w,
                                 0, h==0?texSize.height*winScale:h, -1024, 1024);
    matrixStack->multMatrix(&orthoMatrix);
    
    matrixStack->matrixMode(KM_GL_MODELVIEW);
    mRestoreMVMat = newMVMat;
    if (newMVMat)
    {
        matrixStack->pushMatrix();
        matrixStack->loadIdentity();
    }

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mOldFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    
    if (HLGLConfiguration::getSingleton()->checkForGLExtension("GL_QCOM"))
    {
        // bind a temporary texture so we can clear the render buffer without losing our texture
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureCopy->getName(), 0);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSpriteFrame->_texture->getName(), 0);
    }
}

void HLRenderTexture::clearColor(float r, float g, float b, float a)
{
    // save clear color
    GLfloat    clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE,clearColor);
    
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // restore clear color
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
}

void HLRenderTexture::clearDepth(float depthValue)
{
    GLfloat depthClearValue;
    glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depthClearValue);
    
    glClearDepth(depthValue);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glClearDepth(depthClearValue);
}

void HLRenderTexture::end()
{
    HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
    
    glFinish();
    glBindFramebuffer(GL_FRAMEBUFFER, mOldFBO);
    matrixStack->matrixMode(KM_GL_PROJECTION);
    matrixStack->popMatrix();
    matrixStack->matrixMode(KM_GL_MODELVIEW);
    if (mRestoreMVMat)
    {
        matrixStack->popMatrix();
    }
    
    HLDirector2D::getSingleton()->setViewPort();
}

void HLRenderTexture::renderView(gui::HLView* view)
{
    HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
    kmMat4 mat;
    kmMat4Inverse(&mat, view->getTransform());
    matrixStack->pushMatrix();
    matrixStack->multMatrix(&mat);
    view->visit();
    matrixStack->popMatrix();
}

void HLRenderTexture::saveToFile(const char* filename)
{
    std::string fullpath = filename;
    if (filename[0] != '/')
    {
        fullpath = StringUtil::Format("%s%s", HLResourceManager::getSingleton()->getWritablePath(), filename);
    }
    
    bool has_alpha = true;
    int bytesPerPixl = 4;
    
    if (mPixelFormat == kTexturePixelFormat_RGBA8888)
    {
        
    }
    else if (mPixelFormat == kTexturePixelFormat_RGB888)
    {
        has_alpha = false;
        bytesPerPixl = 3;
    }
    else
    {
        return;
    }
    
//    GLubyte *pBuffer = NULL;
    GLubyte *pTempData = NULL;
//    pBuffer = new GLubyte[mPixelWidth * mPixelHeight * 4];
    
    if(! (pTempData = new GLubyte[mPixelWidth * mPixelHeight * bytesPerPixl]))
    {
//        delete[] pBuffer;
//        pBuffer = NULL;
        return;
    }
    
    GLenum glformat = has_alpha?GL_RGBA:GL_RGB;
    GLenum gltype = GL_UNSIGNED_BYTE;
    
    this->beginWith2DProjection();
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0,0,mPixelWidth, mPixelHeight,glformat, gltype, pTempData);
    this->end();
    
    if (std::string::npos != fullpath.rfind(".png"))
    {
        
        png_byte color_type = has_alpha?PNG_COLOR_TYPE_RGB_ALPHA:PNG_COLOR_TYPE_RGB;
        int bit_depth = 8;
        
    //    for (int i = 0; i < mPixelHeight; ++i)
    //    {
    //        memcpy(&pBuffer[i * mPixelWidth * 4],
    //               &pTempData[(mPixelHeight - i - 1) * mPixelWidth * 4],
    //               mPixelWidth * 4);
    //    }
        
    //    delete [] pTempData;
        
        png_structp png_ptr;
        png_infop info_ptr;
        
        FILE *fp = fopen(fullpath.c_str(), "wb");
        if (!fp)
        {
            delete [] pTempData;
            return;
        }
        
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        
        if (!png_ptr)
        {
            delete [] pTempData;
            return;
        }
        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
        {
            delete [] pTempData;
            return;
        }
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            delete [] pTempData;
            return;
        }
        png_init_io(png_ptr, fp);
        
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            delete [] pTempData;
            return;
        }
        png_set_IHDR(png_ptr, info_ptr, mPixelWidth, mPixelHeight,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        
        png_write_info(png_ptr, info_ptr);
        
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            delete [] pTempData;
            return;
        }
        
        png_bytep * row_pointers = (png_bytep*)malloc(mPixelHeight*sizeof(png_bytep));
        for (int i = 0; i < mPixelHeight; ++i)
        {
            row_pointers[i] =  &pTempData[(mPixelHeight - i - 1) * mPixelWidth * bytesPerPixl];//(png_bytep)malloc(sizeof(unsigned char)*4*mPixelWidth);
    //        memcpy(row_pointers[i], &pBuffer[i * mPixelWidth * 4], 4*mPixelWidth);
        }
        
        delete [] pTempData;
        
        png_write_image(png_ptr, row_pointers);
        png_write_end(png_ptr, NULL);
        
    //    for (int j=0; j<mPixelHeight; j++)
    //        free(row_pointers[j]);
        free(row_pointers);
        
        fclose(fp);
    }
    else if (std::string::npos != fullpath.rfind(".webp"))
    {
        GLubyte *pBuffer = NULL;
        pBuffer = new GLubyte[mPixelWidth * mPixelHeight * bytesPerPixl];
        if (!pBuffer)
        {
            delete pTempData;
            return;
        }
        for (int i = 0; i < mPixelHeight; ++i)
        {
            memcpy(&pBuffer[i * mPixelWidth * bytesPerPixl],
                   &pTempData[(mPixelHeight - i - 1) * mPixelWidth * bytesPerPixl],
                   mPixelWidth * bytesPerPixl);
        }

        delete [] pTempData;
        
        FILE *fp = fopen(fullpath.c_str(), "wb");
        if (!fp)
        {
            delete [] pBuffer;
            return;
        }
        
        uint8_t * output;
        size_t outsize;
        if (has_alpha)
        {
            outsize = WebPEncodeBGRA(pBuffer, mPixelWidth, mPixelHeight, mPixelWidth * 4, 80, &output);
        }
        else
        {
            outsize = WebPEncodeRGB(pBuffer, mPixelWidth, mPixelHeight, mPixelWidth * 3, 80, &output);
        }
        
        fwrite(output, outsize, 1, fp);
        
        delete [] pBuffer;
        
        fclose(fp);
    }
}

NS_HL_END
