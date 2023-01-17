//
//  HLImageView.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLImageView.h"
#include "HLTexture.h"
#include "HLProgram.h"
#include <stdlib.h>
#include "HLGLMatrixStack.h"
#include "HLRenderTexture.h"
#include "HLDirector2D.h"
#include "HLGaussianBlurProgram.h"
#include "HLDrawingPrimitives.h"
#include "HLTimer.h"

#if USE_JS_SCRIPTING
#include "JSImageView.h"
#endif

#ifndef WIN32
using std::min;
using std::max;
#endif

NS_HL_BEGIN

namespace gui
{
    HLImageView::HLImageView(HLView *parent):HLView(parent)
    {
        initMembers();
    }
    
    HLImageView::HLImageView(HLView *parent, HLRect frame):HLView(parent, frame)
    {
        initMembers();
    }
    
    HLImageView::HLImageView(HLView *parent, const HLImageView& copy):HLView(parent, copy)
    {
        mTintColor = copy.mTintColor;
        mSpriteFrame = new HLSpriteFrame();
        mSpriteFrame->_textureRect = HLRectZero;
        mSpriteFrame->_texture = NULL;
        mSpriteFrame->_offset = HLPointZero;
        mSpriteFrame->_sourceSize = HLSizeZero;
        mSpriteFrame->_rotated = false;
//        mSpriteFrame = new HLSpriteFrame();
//        mSpriteFrame->_name = copy.mSpriteFrame->_name;
//        mSpriteFrame->_offset = copy.mSpriteFrame->_offset;
//        mSpriteFrame->_rotated = copy.mSpriteFrame->_rotated;
//        mSpriteFrame->_sourceSize = copy.mSpriteFrame->_sourceSize;
//        mSpriteFrame->_texture = copy.mSpriteFrame->_texture;
//        if (mSpriteFrame->_texture)
//            mSpriteFrame->_texture->retain();
//        mSpriteFrame->_textureRect = copy.mSpriteFrame->_textureRect;
        mBrightness = copy.mBrightness;
        mShowGrayImage = copy.mShowGrayImage;
        mFlipX = copy.mFlipX;
        mFlipY = copy.mFlipY;
        mRepeatStretch = copy.mRepeatStretch;
//        mVAO = -1;
//        mVBO[0] = -1;
        mSrcFactor = copy.mSrcFactor;
        mDstFactor = copy.mDstFactor;
        mRotation = copy.mRotation;
        mMatrixDirty = true;
        setImage(copy.mSpriteFrame, copy.mCapInsets);
    }
    
    void HLImageView::initMembers()
    {
        mTintColor.r = mTintColor.b = mTintColor.g = mTintColor.a = 255;
        mGlowColor = mTintColor;
        mCapInsets = HLRectZero;
        mSpriteFrame = new HLSpriteFrame();
        mSpriteFrame->_textureRect = HLRectZero;
        mSpriteFrame->_texture = NULL;
        mSpriteFrame->_offset = HLPointZero;
        mSpriteFrame->_sourceSize = HLSizeZero;
        mSpriteFrame->_rotated = false;
        mBrightness = 1;
        mShowGrayImage = false;
        mFlipX = false;
        mFlipY = false;
        mRepeatStretch = false;
//        mVAO = -1;
//        mVBO[0] = -1;
        mSrcFactor = GL_SRC_ALPHA;
        mDstFactor = GL_ONE_MINUS_SRC_ALPHA;
        mRotation = 0;
        mMatrixDirty = true;
    }
    
    HLImageView::~HLImageView()
    {
#if USE_JS_SCRIPTING
        JSImageView::removeJSObject(this);
#endif
        if (mSpriteFrame)
        {
            mSpriteFrame->release();
//#if HL_USE_VAO
//            glDeleteVertexArrays(1, &mVAO);
//            glDeleteBuffers(2, &mVBO[0]);
//#endif
        }
        if (mCheckTimer)
        {
            mCheckTimer->cancel();
        }
    }
    
    void HLImageView::setTintColor(color4B color)
    {
        if (currAnimations)
        {
            propertyChanged(this, "tintColor", mTintColor, color);
            return;
        }
        mTintColor = color;
        if (mSpriteFrame->_texture)
        {
            updateQuads();
//            setupVBOandVAO();
        }
    }
    
    void HLImageView::setGlowColor(color4B color)
    {
        if (mGlowColor.r == color.r && mGlowColor.g == color.g && mGlowColor.b == color.b && mGlowColor.r == color.a)
        {
            if (currAnimations)
            {
                propertyChanged(this, "glowColor", mTintColor, color);
                return;
            }
            return;
        }
        mGlowColor = color;
        if (mGlowTexture) {
            mGlowTexture->release();
            mGlowTexture = nullptr;
        }
    }
    
    void HLImageView::checkLoading(HLTimer* timer, float dt)
    {
        if (!mSpriteFrame->_texture->loading())
        {
            mCheckTimer->cancel();
            mCheckTimer = nullptr;
            mSpriteFrame->_sourceSize = mSpriteFrame->_texture->getImageSize()*mSpriteFrame->_texture->getScale();
            mSpriteFrame->_textureRect.size = mSpriteFrame->_sourceSize;
            updateQuads();
        }
    }
    
    void HLImageView::setImage(HLTexture* tex, HLRect capInsets)
    {
        if (!tex)
        {
            if (mSpriteFrame && mSpriteFrame->_texture)
            {
                mSpriteFrame->_texture->release();
                mSpriteFrame->_texture = NULL;
            }
            return;
        }
        if (tex && tex->hasPremultipliedAlpha())
        {
            mSrcFactor = GL_ONE;
        }
        else
        {
            mSrcFactor = GL_SRC_ALPHA;
        }
        mSpriteFrame->_offset = HLPointZero;
        mSpriteFrame->_sourceSize = tex->getImageSize()*tex->getScale();
        mSpriteFrame->_rotated = false;
        tex->retain();
        if (mSpriteFrame->_texture)
        {
            mSpriteFrame->_texture->release();
        }
        mSpriteFrame->_texture = tex;
        mSpriteFrame->_textureRect.origin = HLPointZero;
        mSpriteFrame->_textureRect.size = mSpriteFrame->_sourceSize;
        mCapInsets = capInsets;
        if (mSpriteFrame->_texture->loading())
        {
            if (mCheckTimer)
            {
                mCheckTimer->cancel();
            }
            mCheckTimer = new HLTimer(0.1, true);
            mCheckTimer->delegate = newDelegate(this, &HLImageView::checkLoading);
            return;
        }
        if (mBounds.size != HLSizeZero)
        {
            updateQuads();
        }
        
        if (mGlowTexture)
        {
            mGlowTexture->release();
            mGlowTexture = nullptr;
        }
    }
    
    void HLImageView::setImage(HLSpriteFrame* spriteFrame, HLRect capInsets)
    {
        if (!spriteFrame || !spriteFrame->_texture)
        {
            if (mSpriteFrame && mSpriteFrame->_texture)
            {
                mSpriteFrame->_texture->release();
                mSpriteFrame->_texture = NULL;
            }
            return;
        }
        if (mSpriteFrame->_texture && mSpriteFrame->_texture->hasPremultipliedAlpha())
        {
            mSrcFactor = GL_ONE;
        }
        else
        {
            mSrcFactor = GL_SRC_ALPHA;
        }
        spriteFrame->_texture->retain();
        if (mSpriteFrame->_texture)
        {
            mSpriteFrame->_texture->release();
        }
        mSpriteFrame->_textureRect = spriteFrame->_textureRect;
        mSpriteFrame->_texture = spriteFrame->_texture;
        mSpriteFrame->_offset = spriteFrame->_offset;
        mSpriteFrame->_sourceSize = spriteFrame->_sourceSize;
        mSpriteFrame->_rotated = spriteFrame->_rotated;

        mCapInsets = capInsets;
        if (mBounds.size != HLSizeZero)
        {
            updateQuads();
        }
        
        if (mGlowTexture)
        {
            mGlowTexture->release();
            mGlowTexture = nullptr;
        }
    }
    
    void HLImageView::setGlowRadius(float value)
    {
        value = roundf(value);
        if (currAnimations)
        {
            propertyChanged(this, "glowRadius", mGlowRadius, value);
            return;
        }
        if (value != mGlowRadius)
        {
            mGlowRadius = value;
            if (mGlowTexture)
            {
                mGlowTexture->release();
                mGlowTexture = nullptr;
            }
        }
    }
    
    void HLImageView::onInternalEvent(const char* event)
    {
        if (!strcmp(event, "size_changed") || !strcmp(event, "frame_changed") || !strcmp(event, "edgeinsets_changed"))
        {
            if (mSpriteFrame->_texture)
            {
                updateQuads();
//                setupVBOandVAO();
            }
        }
    }
    
//#if HL_USE_VAO
    void HLImageView::updateQuads()
    {
        mQuads.clear();
        mQuads.reserve(9);
        
        if (mCapInsets == HLRectZero) // no slice and stretch the entire image
        {
            HLPoint offset = HLPoint(mSpriteFrame->_offset.x, mSpriteFrame->_sourceSize.height-mSpriteFrame->_textureRect.size.height-mSpriteFrame->_offset.y);
            offset.x = offset.x*(mBounds.size.width/mSpriteFrame->_sourceSize.width);
            offset.y = offset.y*(mBounds.size.height/mSpriteFrame->_sourceSize.height);
            HLSize size;
            size.width = mSpriteFrame->_textureRect.size.width*(mBounds.size.width/mSpriteFrame->_sourceSize.width);
            size.height = mSpriteFrame->_textureRect.size.height*(mBounds.size.height/mSpriteFrame->_sourceSize.height);
            updateQuads(mSpriteFrame->_textureRect, HLRect(offset, size), mRepeatStretch);
        }
        else // slice to 9 parts
        {
            float l;
            float t;
            float w = mSpriteFrame->_sourceSize.width;
            float h = mSpriteFrame->_sourceSize.height;
            
            float scale = mBounds.size.width/mSpriteFrame->_sourceSize.width;
            
            if (mCapInsets.size.width == 0)
            {
                mCapInsets.size.width = 1/w;
            }
            if (mCapInsets.size.height == 0)
            {
                mCapInsets.size.height = 1/h;
            }
            HLRect capInsetsInternal; // texture rect of the center area
            if (mSpriteFrame->_rotated)
            {
                l = mSpriteFrame->_textureRect.origin.y - mSpriteFrame->_offset.x;
                t = mSpriteFrame->_textureRect.origin.x + mSpriteFrame->_textureRect.size.height + mSpriteFrame->_offset.y;
                capInsetsInternal.origin.x = l + mCapInsets.origin.x * w;
                capInsetsInternal.origin.y = t - mCapInsets.origin.y * h;
                capInsetsInternal.size.width = mCapInsets.size.width * w;
                capInsetsInternal.size.height = mCapInsets.size.height * h;
            }
            else
            {
                l = mSpriteFrame->_textureRect.origin.x - mSpriteFrame->_offset.x;
                t = mSpriteFrame->_textureRect.origin.y - mSpriteFrame->_offset.y;
                capInsetsInternal.origin.x = l + mCapInsets.origin.x * w;
                capInsetsInternal.origin.y = t + mCapInsets.origin.y * h;
                capInsetsInternal.size.width = mCapInsets.size.width * w;
                capInsetsInternal.size.height = mCapInsets.size.height * h;
            }
            HLRect capSizeRectInternal; // coordinate on screen in points of the center area
            capSizeRectInternal.origin.x = min(mBounds.size.width/2, (capInsetsInternal.origin.x-l)/mSpriteFrame->_texture->getScale());
            capSizeRectInternal.origin.y = min(mBounds.size.height/2, (h-capInsetsInternal.size.height-fabsf(capInsetsInternal.origin.y-t))/mSpriteFrame->_texture->getScale());
            float rightSize = min(mBounds.size.width/2, (w-capInsetsInternal.size.width-(capInsetsInternal.origin.x-l))/mSpriteFrame->_texture->getScale());
            float topSize = min(mBounds.size.height/2, fabsf(capInsetsInternal.origin.y-t)/mSpriteFrame->_texture->getScale());
            capSizeRectInternal.size.width = mBounds.size.width - capSizeRectInternal.origin.x - rightSize;
            capSizeRectInternal.size.height = mBounds.size.height - capSizeRectInternal.origin.y - topSize;
            
            if (mSpriteFrame->_rotated)
            {
                // center
                updateQuads(capInsetsInternal, capSizeRectInternal,
                            mRepeatStretch);
                // top
                updateQuads(HLRect(capInsetsInternal.origin.x,
                                   t,
                                   capInsetsInternal.size.width,
                                   t-capInsetsInternal.origin.y),
                            HLRect(capSizeRectInternal.origin.x,
                                   capSizeRectInternal.origin.y+capSizeRectInternal.size.height,
                                   capSizeRectInternal.size.width,
                                   mBounds.size.height-capSizeRectInternal.size.height-capSizeRectInternal.origin.y),
                            mRepeatStretch);
                // bottom
                updateQuads(HLRect(capInsetsInternal.origin.x,
                                   capInsetsInternal.origin.y-capInsetsInternal.size.height,
                                   capInsetsInternal.size.width,
                                   h-(t-capInsetsInternal.origin.y+capInsetsInternal.size.height)),
                            HLRect(capSizeRectInternal.origin.x,
                                   0,
                                   capSizeRectInternal.size.width,
                                   capSizeRectInternal.origin.y),
                            mRepeatStretch);
                // left
                updateQuads(HLRect(l,
                                   capInsetsInternal.origin.y,
                                   capInsetsInternal.origin.x - l,
                                   capInsetsInternal.size.height),
                            HLRect(0,
                                   capSizeRectInternal.origin.y,
                                   capSizeRectInternal.origin.x,
                                   capSizeRectInternal.size.height),
                            mRepeatStretch);
                // right
                updateQuads(HLRect(capInsetsInternal.origin.x+capInsetsInternal.size.width,
                                   capInsetsInternal.origin.y,
                                   w-(capInsetsInternal.origin.x-l+capInsetsInternal.size.width),
                                   capInsetsInternal.size.height),
                            HLRect(capSizeRectInternal.origin.x+capSizeRectInternal.size.width,
                                   capSizeRectInternal.origin.y,
                                   mBounds.size.width-capSizeRectInternal.size.width-capSizeRectInternal.origin.x,
                                   capSizeRectInternal.size.height),
                            mRepeatStretch);
                // top left
                updateQuads(HLRect(l,
                                   t,
                                   capInsetsInternal.origin.x-l,
                                   t-capInsetsInternal.origin.y),
                            HLRect(0,
                                   capSizeRectInternal.origin.y+capSizeRectInternal.size.height,
                                   capSizeRectInternal.origin.x,
                                   mBounds.size.height-capSizeRectInternal.origin.y-capSizeRectInternal.size.height));
                // top right
                updateQuads(HLRect(capInsetsInternal.origin.x+capInsetsInternal.size.width,
                                   t,
                                   w-(capInsetsInternal.origin.x-l+capInsetsInternal.size.width),
                                   t-capInsetsInternal.origin.y),
                            HLRect(capSizeRectInternal.origin.x+capSizeRectInternal.size.width,
                                   capSizeRectInternal.origin.y+capSizeRectInternal.size.height,
                                   mBounds.size.width-capSizeRectInternal.size.width-capSizeRectInternal.origin.x,
                                   mBounds.size.height-capSizeRectInternal.origin.y-capSizeRectInternal.size.height));
                // bottom left
                updateQuads(HLRect(l,
                                   capInsetsInternal.origin.y-capInsetsInternal.size.height,
                                   capInsetsInternal.origin.x-l,
                                   h-(t-capInsetsInternal.origin.y+capInsetsInternal.size.height)),
                            HLRect(0,
                                   0,
                                   capSizeRectInternal.origin.x,
                                   capSizeRectInternal.origin.y));
                // bottom right
                updateQuads(HLRect(capInsetsInternal.origin.x+capInsetsInternal.size.width,
                                   capInsetsInternal.origin.y-capInsetsInternal.size.height,
                                   w-(capInsetsInternal.origin.x-l+capInsetsInternal.size.width),
                                   h-(t-capInsetsInternal.origin.y+capInsetsInternal.size.height)),
                            HLRect(capSizeRectInternal.origin.x+capSizeRectInternal.size.width,
                                   0,
                                   mBounds.size.width-capSizeRectInternal.size.width-capSizeRectInternal.origin.x,
                                   capSizeRectInternal.origin.y));
            }
            else
            {
                // center
                updateQuads(capInsetsInternal, capSizeRectInternal,
                            mRepeatStretch);
                // top
                updateQuads(HLRect(capInsetsInternal.origin.x,
                                   t,
                                   capInsetsInternal.size.width,
                                   capInsetsInternal.origin.y-t),
                            HLRect(capSizeRectInternal.origin.x,
                                   capSizeRectInternal.origin.y+capSizeRectInternal.size.height,
                                   capSizeRectInternal.size.width,
                                   mBounds.size.height-capSizeRectInternal.size.height-capSizeRectInternal.origin.y),
                            mRepeatStretch);
                // bottom
                updateQuads(HLRect(capInsetsInternal.origin.x,
                                   capInsetsInternal.origin.y+capInsetsInternal.size.height,
                                   capInsetsInternal.size.width,
                                   h-(capInsetsInternal.origin.y-t+capInsetsInternal.size.height)),
                            HLRect(capSizeRectInternal.origin.x,
                                   0,
                                   capSizeRectInternal.size.width,
                                   capSizeRectInternal.origin.y),
                            mRepeatStretch);
                // left
                updateQuads(HLRect(l,
                                   capInsetsInternal.origin.y,
                                   capInsetsInternal.origin.x - l,
                                   capInsetsInternal.size.height),
                            HLRect(0,
                                   capSizeRectInternal.origin.y,
                                   capSizeRectInternal.origin.x,
                                   capSizeRectInternal.size.height),
                            mRepeatStretch);
                // right
                updateQuads(HLRect(capInsetsInternal.origin.x+capInsetsInternal.size.width,
                                   capInsetsInternal.origin.y,
                                   w-(capInsetsInternal.origin.x-l+capInsetsInternal.size.width),
                                   capInsetsInternal.size.height),
                            HLRect(capSizeRectInternal.origin.x+capSizeRectInternal.size.width,
                                   capSizeRectInternal.origin.y,
                                   mBounds.size.width-capSizeRectInternal.size.width-capSizeRectInternal.origin.x,
                                   capSizeRectInternal.size.height),
                            mRepeatStretch);
                // top left
                updateQuads(HLRect(l,
                                   t,
                                   capInsetsInternal.origin.x-l,
                                   capInsetsInternal.origin.y-t),
                            HLRect(0,
                                   capSizeRectInternal.origin.y+capSizeRectInternal.size.height,
                                   capSizeRectInternal.origin.x,
                                   mBounds.size.height-capSizeRectInternal.origin.y-capSizeRectInternal.size.height));
                // top right
                updateQuads(HLRect(capInsetsInternal.origin.x+capInsetsInternal.size.width,
                                   t,
                                   w-(capInsetsInternal.origin.x-l+capInsetsInternal.size.width),
                                   capInsetsInternal.origin.y-t),
                            HLRect(capSizeRectInternal.origin.x+capSizeRectInternal.size.width,
                                   capSizeRectInternal.origin.y+capSizeRectInternal.size.height,
                                   mBounds.size.width-capSizeRectInternal.size.width-capSizeRectInternal.origin.x,
                                   mBounds.size.height-capSizeRectInternal.origin.y-capSizeRectInternal.size.height));
                // bottom left
                updateQuads(HLRect(l,
                                   capInsetsInternal.origin.y+capInsetsInternal.size.height,
                                   capInsetsInternal.origin.x-l,
                                   h-(capInsetsInternal.origin.y-t+capInsetsInternal.size.height)),
                            HLRect(0,
                                   0,
                                   capSizeRectInternal.origin.x,
                                   capSizeRectInternal.origin.y));
                // bottom right
                updateQuads(HLRect(capInsetsInternal.origin.x+capInsetsInternal.size.width,
                                   capInsetsInternal.origin.y+capInsetsInternal.size.height,
                                   w-(capInsetsInternal.origin.x-l+capInsetsInternal.size.width),
                                   h-(capInsetsInternal.origin.y-t+capInsetsInternal.size.height)),
                            HLRect(capSizeRectInternal.origin.x+capSizeRectInternal.size.width,
                                   0,
                                   mBounds.size.width-capSizeRectInternal.size.width-capSizeRectInternal.origin.x,
                                   capSizeRectInternal.origin.y));
            }
        }
        
        int len = (int)mQuads.size();
        mIndices.clear();
        mIndices.resize(len*6);
        for (int i = 0; i < len; i++)
        {
            const unsigned int i6 = i*6;
            const unsigned int i4 = i*4;
            mIndices[i6+0] = (GLushort) i4+0;
            mIndices[i6+1] = (GLushort) i4+1;
            mIndices[i6+2] = (GLushort) i4+2;
            
            mIndices[i6+5] = (GLushort) i4+1;
            mIndices[i6+4] = (GLushort) i4+2;
            mIndices[i6+3] = (GLushort) i4+3;
        }
//        setupVBOandVAO();
    }
    
    void HLImageView::updateQuads(HLRect texRect, HLRect drawRect, bool repeat)
    {
        HLSize imgSize = texRect.size / mSpriteFrame->_texture->getScale();
        if (mSpriteFrame->_rotated)
        {
            // adjust offset
            if (texRect.origin.x < mSpriteFrame->_textureRect.origin.y)
            {
                float offset = (texRect.origin.x - mSpriteFrame->_textureRect.origin.y)/mSpriteFrame->_texture->getScale();
                drawRect.origin.x -= offset;
                drawRect.size.width += offset;
                texRect.size.width += texRect.origin.x - mSpriteFrame->_textureRect.origin.y;
                texRect.origin.x = mSpriteFrame->_textureRect.origin.y;
            }
            else if(texRect.origin.x + texRect.size.width > mSpriteFrame->_textureRect.origin.y + mSpriteFrame->_textureRect.size.height)
            {
                float offset = (mSpriteFrame->_textureRect.origin.y + mSpriteFrame->_textureRect.size.height - texRect.origin.x - texRect.size.width)/mSpriteFrame->_texture->getScale();
                drawRect.size.width -= offset;
                texRect.size.width = mSpriteFrame->_textureRect.origin.y + mSpriteFrame->_textureRect.size.height - texRect.origin.x;
            }
            if (texRect.origin.y > mSpriteFrame->_textureRect.origin.x + mSpriteFrame->_textureRect.size.width)
            {
                float offset = (mSpriteFrame->_textureRect.origin.x + mSpriteFrame->_textureRect.size.width - texRect.origin.y)/mSpriteFrame->_texture->getScale();
                drawRect.size.height += offset;
                texRect.size.height += mSpriteFrame->_textureRect.origin.x + mSpriteFrame->_textureRect.size.width - texRect.origin.y;
                texRect.origin.y = mSpriteFrame->_textureRect.origin.x + mSpriteFrame->_textureRect.size.width;
            }
            else if(texRect.origin.y - texRect.size.height < mSpriteFrame->_textureRect.origin.x)
            {
                float offset = (texRect.origin.y - texRect.size.height - mSpriteFrame->_textureRect.origin.x)/mSpriteFrame->_texture->getScale();
                drawRect.origin.y -= offset;
                drawRect.size.height += offset;
                texRect.size.height = texRect.origin.y - mSpriteFrame->_textureRect.origin.x;
            }
            
            // convert texRect to percent
            HLSize imageSize = mSpriteFrame->_texture->getImageSize()*mSpriteFrame->_texture->getScale();
            texRect.origin.x /= imageSize.height;
            texRect.origin.y /= imageSize.width;
            texRect.size.width /= imageSize.height;
            texRect.size.height /= imageSize.width;
            if (!repeat)
            {
                // set quad
                V3F_C4B_T2F_Quad quad;
                quad.tl.vertices.x = drawRect.origin.x;
                quad.tl.vertices.y = drawRect.origin.y+drawRect.size.height;
                quad.tl.vertices.z = 0;
                quad.tl.colors = mTintColor;
                quad.tl.texCoords.u = texRect.origin.y;
                quad.tl.texCoords.v = texRect.origin.x;
                
                quad.bl.vertices.x = drawRect.origin.x;
                quad.bl.vertices.y = drawRect.origin.y;
                quad.bl.vertices.z = 0;
                quad.bl.colors = mTintColor;
                quad.bl.texCoords.u = texRect.origin.y-texRect.size.height;
                quad.bl.texCoords.v = texRect.origin.x;
                
                quad.tr.vertices.x = drawRect.origin.x+drawRect.size.width;
                quad.tr.vertices.y = drawRect.origin.y+drawRect.size.height;
                quad.tr.vertices.z = 0;
                quad.tr.colors = mTintColor;
                quad.tr.texCoords.u = texRect.origin.y;
                quad.tr.texCoords.v = texRect.origin.x+texRect.size.width;
                
                quad.br.vertices.x = drawRect.origin.x+drawRect.size.width;
                quad.br.vertices.y = drawRect.origin.y;
                quad.br.vertices.z = 0;
                quad.br.colors = mTintColor;
                quad.br.texCoords.u = texRect.origin.y-texRect.size.height;
                quad.br.texCoords.v = texRect.origin.x+texRect.size.width;
                
                mQuads.push_back(quad);
            }
            else
            {
                HLSize imageSize1 = mSpriteFrame->_texture->getImageSize();
                int repeatCountX = ceilf(drawRect.size.width / imgSize.width);
                int repeatCountY = ceilf(drawRect.size.height / imgSize.height);
                for (int y = 0; y < repeatCountY; y++)
                {
                    float miny = drawRect.origin.y+imgSize.height*y;
                    float maxy = MIN(drawRect.origin.y+imgSize.height*y+imgSize.height, drawRect.origin.y+drawRect.size.height);
                    float texH = (imgSize.height-(maxy-miny))/imageSize1.width;
                    for (int x = 0; x < repeatCountX; x++)
                    {
                        float minx = drawRect.origin.x+imgSize.width*x;
                        float maxx = MIN(drawRect.origin.x+imgSize.width*x+imgSize.width, drawRect.origin.x+drawRect.size.width);
                        float texW = (imgSize.width-(maxx-minx))/imageSize1.height;
                        
                        // set quad
                        V3F_C4B_T2F_Quad quad;
                        quad.tl.vertices.x = minx;
                        quad.tl.vertices.y = maxy;
                        quad.tl.vertices.z = 0;
                        quad.tl.colors = mTintColor;
                        quad.tl.texCoords.u = texRect.origin.y-texH;
                        quad.tl.texCoords.v = texRect.origin.x;
                        
                        quad.bl.vertices.x = minx;
                        quad.bl.vertices.y = miny;
                        quad.bl.vertices.z = 0;
                        quad.bl.colors = mTintColor;
                        quad.bl.texCoords.u = texRect.origin.y-texRect.size.height;
                        quad.bl.texCoords.v = texRect.origin.x;
                        
                        quad.tr.vertices.x = maxx;
                        quad.tr.vertices.y = maxy;
                        quad.tr.vertices.z = 0;
                        quad.tr.colors = mTintColor;
                        quad.tr.texCoords.u = texRect.origin.y-texH;
                        quad.tr.texCoords.v = texRect.origin.x+texRect.size.width-texW;
                        
                        quad.br.vertices.x = maxx;
                        quad.br.vertices.y = miny;
                        quad.br.vertices.z = 0;
                        quad.br.colors = mTintColor;
                        quad.br.texCoords.u = texRect.origin.y-texRect.size.height;
                        quad.br.texCoords.v = texRect.origin.x+texRect.size.width-texW;
                        
                        mQuads.push_back(quad);
                    }
                }
            }
        }
        else
        {
            // adjust offset
            if (texRect.origin.x < mSpriteFrame->_textureRect.origin.x)
            {
                float offset = (texRect.origin.x - mSpriteFrame->_textureRect.origin.x)/mSpriteFrame->_texture->getScale();
                drawRect.origin.x -= offset;
                drawRect.size.width += offset;
                texRect.size.width += texRect.origin.x - mSpriteFrame->_textureRect.origin.x;
                texRect.origin.x = mSpriteFrame->_textureRect.origin.x;
            }
            else if(texRect.origin.x + texRect.size.width > mSpriteFrame->_textureRect.origin.x + mSpriteFrame->_textureRect.size.width)
            {
                float offset = (mSpriteFrame->_textureRect.origin.x + mSpriteFrame->_textureRect.size.width - texRect.origin.x - texRect.size.width)/mSpriteFrame->_texture->getScale();
                drawRect.size.width -= offset;
                texRect.size.width = mSpriteFrame->_textureRect.origin.x + mSpriteFrame->_textureRect.size.width - texRect.origin.x;
            }
            if (texRect.origin.y < mSpriteFrame->_textureRect.origin.y)
            {
                float offset = (texRect.origin.y-mSpriteFrame->_textureRect.origin.y)/mSpriteFrame->_texture->getScale();
                drawRect.size.height += offset;
                texRect.size.height += texRect.origin.y - mSpriteFrame->_textureRect.origin.y;
                texRect.origin.y = mSpriteFrame->_textureRect.origin.y;
            }
            else if(texRect.origin.y + texRect.size.height > mSpriteFrame->_textureRect.origin.y + mSpriteFrame->_textureRect.size.height)
            {
                float offset = (mSpriteFrame->_textureRect.origin.y + mSpriteFrame->_textureRect.size.height - texRect.origin.y - texRect.size.height)/mSpriteFrame->_texture->getScale();
                drawRect.origin.y -= offset;
                drawRect.size.height += offset;
                texRect.size.height = mSpriteFrame->_textureRect.origin.y + mSpriteFrame->_textureRect.size.height - texRect.origin.y;
            }
            
            // convert texRect to percent
            HLSize imageSize = mSpriteFrame->_texture->getImageSize()*mSpriteFrame->_texture->getScale();
            texRect.origin.x /= imageSize.width;
            texRect.origin.y /= imageSize.height;
            texRect.size.width /= imageSize.width;
            texRect.size.height /= imageSize.height;
            if (!repeat)
            {
                // set quad
                V3F_C4B_T2F_Quad quad;
                quad.tl.vertices.x = drawRect.origin.x;
                quad.tl.vertices.y = drawRect.origin.y+drawRect.size.height;
                quad.tl.vertices.z = 0;
                quad.tl.colors = mTintColor;
                quad.tl.texCoords.u = texRect.origin.x;
                quad.tl.texCoords.v = texRect.origin.y;
                
                quad.bl.vertices.x = drawRect.origin.x;
                quad.bl.vertices.y = drawRect.origin.y;
                quad.bl.vertices.z = 0;
                quad.bl.colors = mTintColor;
                quad.bl.texCoords.u = texRect.origin.x;
                quad.bl.texCoords.v = texRect.origin.y+texRect.size.height;
                
                quad.tr.vertices.x = drawRect.origin.x+drawRect.size.width;
                quad.tr.vertices.y = drawRect.origin.y+drawRect.size.height;
                quad.tr.vertices.z = 0;
                quad.tr.colors = mTintColor;
                quad.tr.texCoords.u = texRect.origin.x+texRect.size.width;
                quad.tr.texCoords.v = texRect.origin.y;
                
                quad.br.vertices.x = drawRect.origin.x+drawRect.size.width;
                quad.br.vertices.y = drawRect.origin.y;
                quad.br.vertices.z = 0;
                quad.br.colors = mTintColor;
                quad.br.texCoords.u = texRect.origin.x+texRect.size.width;
                quad.br.texCoords.v = texRect.origin.y+texRect.size.height;
                
                mQuads.push_back(quad);
            }
            else
            {
                HLSize imageSize1 = mSpriteFrame->_texture->getImageSize();
                int repeatCountX = ceilf(drawRect.size.width / imgSize.width);
                int repeatCountY = ceilf(drawRect.size.height / imgSize.height);
                for (int y = 0; y < repeatCountY; y++)
                {
                    float miny = drawRect.origin.y+imgSize.height*y;
                    float maxy = MIN(drawRect.origin.y+imgSize.height*y+imgSize.height, drawRect.origin.y+drawRect.size.height);
                    float texH = (imgSize.height-(maxy-miny))/imageSize1.height;
                    for (int x = 0; x < repeatCountX; x++)
                    {
                        float minx = drawRect.origin.x+imgSize.width*x;
                        float maxx = MIN(drawRect.origin.x+imgSize.width*x+imgSize.width, drawRect.origin.x+drawRect.size.width);
                        float texW = (imgSize.width-(maxx-minx))/imageSize1.width;
                        
                        // set quad
                        V3F_C4B_T2F_Quad quad;
                        quad.tl.vertices.x = minx;
                        quad.tl.vertices.y = maxy;
                        quad.tl.vertices.z = 0;
                        quad.tl.colors = mTintColor;
                        quad.tl.texCoords.u = texRect.origin.x;
                        quad.tl.texCoords.v = texRect.origin.y+texH;
                        
                        quad.bl.vertices.x = minx;
                        quad.bl.vertices.y = miny;
                        quad.bl.vertices.z = 0;
                        quad.bl.colors = mTintColor;
                        quad.bl.texCoords.u = texRect.origin.x;
                        quad.bl.texCoords.v = texRect.origin.y+texRect.size.height;
                        
                        quad.tr.vertices.x = maxx;
                        quad.tr.vertices.y = maxy;
                        quad.tr.vertices.z = 0;
                        quad.tr.colors = mTintColor;
                        quad.tr.texCoords.u = texRect.origin.x+texRect.size.width-texW;
                        quad.tr.texCoords.v = texRect.origin.y+texH;
                        
                        quad.br.vertices.x = maxx;
                        quad.br.vertices.y = miny;
                        quad.br.vertices.z = 0;
                        quad.br.colors = mTintColor;
                        quad.br.texCoords.u = texRect.origin.x+texRect.size.width-texW;
                        quad.br.texCoords.v = texRect.origin.y+texRect.size.height;
                        
                        mQuads.push_back(quad);
                    }
                }
            }
            
            
        }
    }
 
    /*
    void HLImageView::setupVBOandVAO()
    {
        if (mVAO == -1)
        {
            glGenVertexArrays(1, &mVAO);
        }
        glBindVertexArray(mVAO);
        
        const int size = (int)mQuads.size();
        
#define kQuadSize sizeof(mQuads[0].bl)
        
        if (mVBO[0] == -1)
        {
            glGenBuffers(2, &mVBO[0]);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0]) * size, &mQuads[0], GL_DYNAMIC_DRAW);
        
        // vertices
        glEnableVertexAttribArray(kVertexAttrib_Position);
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, vertices));
        
        // colors
        glEnableVertexAttribArray(kVertexAttrib_Color);
        glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, colors));
        
        // tex coords
        glEnableVertexAttribArray(kVertexAttrib_TexCoords);
        glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, texCoords));
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices[0]) * size * 6, &mIndices[0], GL_STATIC_DRAW);
        
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        CHECK_GL_ERROR();
    }
     */
//#endif
    
    void HLImageView::generateGlowTexture()
    {
        float scale = HLDirector2D::getSingleton()->getWinSizeScale();
        GLfloat verticalPassTexelWidthOffset = scale/mFrame.size.width;
        GLfloat horizontalPassTexelWidthOffset = scale/mFrame.size.height;
        
        hlglBlendFunc(GL_ONE, GL_ZERO);
        
        const int rTexW = mFrame.size.width/scale + 2*mGlowRadius/scale;
        const int rTexH = mFrame.size.height/scale + 2*mGlowRadius/scale;
        
        HLRenderTexture pass0(rTexW, rTexH);
        pass0.beginWith2DProjection();
        HLGLMatrixStack::matrixStack()->translatef((rTexW*scale-mFrame.size.width)/2, (rTexH*scale-mFrame.size.height)/2, 0);
        HLProgram* program = HLProgramManager::currentManager()->getProgram(mSpriteFrame->_texture->getAlphaTexture()?kShaderPositionTextureColorWithExtraAlpha:kShaderPositionTextureColor);
        program->use();
        program->getUniforms()[0].setUniform1f(mBrightness);
        program->setUniformForModelViewProjectionMatrix();
        hlglBindTexture(GL_TEXTURE_2D, mSpriteFrame->_texture->getName());
        if (mSpriteFrame->_texture->getAlphaTexture())
        {
            hlglActiveTexture(GL_TEXTURE1);
            hlglBindTexture(GL_TEXTURE_2D, mSpriteFrame->_texture->getAlphaTexture());
            hlglActiveTexture(GL_TEXTURE0);
        }
        drawSelf();
        pass0.end();
        
        color4F glowColor = {mGlowColor.r/255.f, mGlowColor.g/255.f, mGlowColor.b/255.f, mGlowColor.a/255.f};
        
        HLRenderTexture pass1(rTexW, rTexH);
        pass1.beginWith2DProjection(1.f, 1.f);
        program = HLGaussianBlurProgram::getProgramOfRaiusForGlow(mGlowRadius);
        program->use();
        program->setUniformForModelViewProjectionMatrix();
        program->getUniform("u_texelOffset_x").setUniform1f(0.f);
        program->getUniform("u_texelOffset_y").setUniform1f(verticalPassTexelWidthOffset);
        program->getUniform("u_glowColor").setUniform4fv(&glowColor.r, 1);
        hlglBindTexture(GL_TEXTURE_2D, pass0.getSpriteFrame()->_texture->getName());
        const GLfloat imgVerts[] = {
            0.f, 0.f,
            1.f, 0.f,
            0.f, 1.f,
            1.f, 1.f
        };
        HLSize texSize = pass0.getSpriteFrame()->_texture->getImageSize() * pass0.getSpriteFrame()->_texture->getScale();
        HLSize frameSize = pass0.getSpriteFrame()->_textureRect.size;
        const GLfloat texCoords[] = {
            0.0f, 0.0f,
            frameSize.width/texSize.width, 0.0f,
            0.0f, frameSize.height/texSize.height,
            frameSize.width/texSize.width, frameSize.height/texSize.height,
        };
        glEnableVertexAttribArray(kVertexAttrib_Position);
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, imgVerts);
        glEnableVertexAttribArray(kVertexAttrib_TexCoords);
        glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        CHECK_GL_ERROR();
        pass1.end();
        
        HLRenderTexture pass2(rTexW, rTexH);
        pass2.beginWith2DProjection(1.f, 1.f);
        program = HLGaussianBlurProgram::getProgramOfRaiusForGlow(mGlowRadius);
        program->use();
        program->setUniformForModelViewProjectionMatrix();
        program->getUniform("u_texelOffset_x").setUniform1f(horizontalPassTexelWidthOffset);
        program->getUniform("u_texelOffset_y").setUniform1f(0.f);
        program->getUniform("u_glowColor").setUniform4fv(&glowColor.r, 1);
        
        glEnableVertexAttribArray(kVertexAttrib_Position);
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, imgVerts);
        glEnableVertexAttribArray(kVertexAttrib_TexCoords);
        glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
        hlglBindTexture(GL_TEXTURE_2D, pass1.getSpriteFrame()->_texture->getName());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        CHECK_GL_ERROR();
        pass2.end();
        
        mGlowTexture = pass2.getSpriteFrame();
        mGlowTexture->retain();
    }
    
    void HLImageView::drawGlow()
    {
        if (!mGlowTexture) {
            return;
        }
        
        const float rTexW = mFrame.size.width + 2*mGlowRadius;
        const float rTexH = mFrame.size.height + 2*mGlowRadius;
        
        const GLfloat offsetX = (mFrame.size.width-rTexW)/2;
        const GLfloat offsetY = (mFrame.size.height-rTexH)/2;
        
        const GLfloat imgVerts[] = {
            offsetX, offsetY,
            offsetX+rTexW, offsetY,
            offsetX, offsetY+rTexH,
            offsetX+rTexW, offsetY+rTexH
        };
        
        HLSize texSize = mGlowTexture->_texture->getImageSize() * mGlowTexture->_texture->getScale();
        HLSize frameSize = mGlowTexture->_textureRect.size;
        const GLfloat texCoords[] = {
            0.0f, 0.0f,
            frameSize.width/texSize.width, 0.0f,
            0.0f, frameSize.height/texSize.height,
            frameSize.width/texSize.width, frameSize.height/texSize.height,
        };
        static const GLubyte colors[] = {
            255, 255, 255, 255,
            255, 255, 255, 255,
            255, 255, 255, 255,
            255, 255, 255, 255
        };
        glEnableVertexAttribArray(kVertexAttrib_Position);
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, imgVerts);
        glEnableVertexAttribArray(kVertexAttrib_Color);
        glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);
        glEnableVertexAttribArray(kVertexAttrib_TexCoords);
        glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
        hlglBindTexture(GL_TEXTURE_2D, mGlowTexture->_texture->getName());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        CHECK_GL_ERROR();
    }
    
    void HLImageView::drawSelf()
    {
        const int size = (int)mQuads.size();
        if (size == 0)
        {
            return;
        }
        
#define kQuadSize sizeof(mQuads[0].bl)
        
        // vertices
        glEnableVertexAttribArray(kVertexAttrib_Position);
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) ((uint8_t*)&mQuads[0] + offsetof( V3F_C4B_T2F, vertices)));
        
        // colors
        glEnableVertexAttribArray(kVertexAttrib_Color);
        glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) ((uint8_t*)&mQuads[0] + offsetof( V3F_C4B_T2F, colors)));
        
        // tex coords
        glEnableVertexAttribArray(kVertexAttrib_TexCoords);
        glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) ((uint8_t*)&mQuads[0] + offsetof( V3F_C4B_T2F, texCoords)));
        
        glDrawElements(GL_TRIANGLES, size * 6, GL_UNSIGNED_SHORT, &mIndices[0]);
        
        CHECK_GL_ERROR();
    }
    
    void HLImageView::onDraw()
    {
        if (!mSpriteFrame->_texture || mSpriteFrame->_texture->getName()==0)
        {
            HLView::onDraw();
            return;
        }
        
        if (mGlowRadius > 0 && mGlowTexture == nullptr)
        {
            generateGlowTexture();
        }
        
        HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
        
        if (mFlipX || mFlipY || mRotation)
        {
            matrixStack->pushMatrix();
            if (mMatrixDirty)
            {
                mMatrixDirty = false;
                
                kmMat4Identity(&mMatrix);
                float c = 1, s = 0, x = mBounds.size.width/2, y = mBounds.size.height/2;
                float scaleX = mFlipX?-1:1;
                float scaleY = mFlipY?-1:1;
                if (mRotation)
                {
                    float radians = -DEGREES_TO_RADIANS(mRotation);
                    c = cosf(radians);
                    s = sinf(radians);
                }
                HLPoint anchor;
                anchor.x = x;
                anchor.y = y;
                x += c * -anchor.x * scaleX + -s * -anchor.y * scaleY;
                y += s * -anchor.x * scaleX + c * -anchor.y * scaleY;
                mMatrix.mat[0] = c * scaleX;
                mMatrix.mat[1] = s * scaleX;
                mMatrix.mat[4] = -s * scaleY;
                mMatrix.mat[5] = c * scaleY;
                mMatrix.mat[12] = x;
                mMatrix.mat[13] = y;
            }
            matrixStack->multMatrix(&mMatrix);
        }
        
        unsigned int programidx = 0;
        unsigned int programidx1 = 0;
        if (mShowGrayImage)
        {
            if (mSpriteFrame->_texture->getAlphaTexture())
            {
                programidx = kShaderPositionTextureColorGrayWithExtraAlpha;
                programidx1 = kShaderPositionTextureColorGray;
            }
            else
            {
                programidx = kShaderPositionTextureColorGray;
            }
        }
        else
        {
            if (mSpriteFrame->_texture->getAlphaTexture())
            {
                programidx = kShaderPositionTextureColorWithExtraAlpha;
                programidx1 = kShaderPositionTextureColor;
            }
            else
            {
                programidx = kShaderPositionTextureColor;
            }
        }
        HLProgram *program = HLProgramManager::currentManager()->getProgram(programidx);
        program->use();
        program->getUniforms()[0].setUniform1f(mBrightness);
        program->setUniformForModelViewProjectionMatrix();
        hlglBlendFunc(mSrcFactor, mDstFactor);
        
        if (mSpriteFrame->_texture->getAlphaTexture())
        {
            HLProgram *program = HLProgramManager::currentManager()->getProgram(programidx1);
            program->use();
            program->getUniforms()[0].setUniform1f(mBrightness);
            program->setUniformForModelViewProjectionMatrix();
        }
        
//#if HL_USE_VAO
//        glBindVertexArray(mVAO);
//        glDrawElements(GL_TRIANGLES, (GLsizei)mQuads.size()*6, GL_UNSIGNED_SHORT, 0);
//        glBindVertexArray( 0 );
//#endif
        
//        HLProgram *program = HLProgramManager::currentManager()->getProgram(kShaderPositionTextureGlow);
//        program->use();
//        program->getUniform("u_texelOffset_x").setUniform1f(0.01f);
//        program->getUniform("u_texelOffset_y").setUniform1f(0.01f);
//        program->getUniform("u_glowColor").setUniform4f(1.f, 1.f, 0.f, 1.f);
//        program->setUniformForModelViewProjectionMatrix();
//        hlglBlendFunc(mSrcFactor, mDstFactor);
        
        drawGlow();
        
        program->use();
        
        hlglBindTexture(GL_TEXTURE_2D, mSpriteFrame->_texture->getName());
        if (mSpriteFrame->_texture->getAlphaTexture())
        {
            hlglActiveTexture(GL_TEXTURE1);
            hlglBindTexture(GL_TEXTURE_2D, mSpriteFrame->_texture->getAlphaTexture());
            hlglActiveTexture(GL_TEXTURE0);
        }
        
        drawSelf();
        
        if (mFlipX || mFlipY || mRotation)
        {
            matrixStack->popMatrix();
        }
        
        HLView::onDraw();
    }
    
#if USE_JS_SCRIPTING
    JSCppWrapperBase* HLImageView::getOrCreateJSWrapper(JSContext* cx)
    {
        return JSImageView::getOrCreateWrapper(cx, this);
    }
#endif
    
    HLView* HLImageView::clone(HLView* parent)
    {
        HLImageView* view = new HLImageView(parent, *this);
        std::list<HLView*>::iterator itr = mSubViews.begin();
        for (; itr != mSubViews.end(); ++itr)
        {
            (*itr)->clone(view);
        }
        return view;
    }
}

NS_HL_END
