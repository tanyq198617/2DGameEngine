//
//  HLImageView.h
//  HoolaiEngine
//
//  Created by zhao shuan on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLImageView_H__
#define __HLImageView_H__

#include "HLMacros.h"
#include "HLView.h"
#include "HLUtil.h"
#include "HLSpriteFrame.h"

NS_HL_BEGIN

namespace gui
{
    class HLImageView : public HLView
    {
    public:
        HLImageView(HLView *parent);
        HLImageView(HLView *parent, HLRect frame);
        HLImageView(HLView *parent, const HLImageView& copy);
        ~HLImageView();
        void setImage(HLTexture* tex, HLRect capInsets = HLRectZero);
        void setImage(HLSpriteFrame* spriteFrame, HLRect capInsets = HLRectZero);
        void setTintColor(color4B color);
        color4B getTintColor() {return mTintColor;}
        void setGlowColor(color4B color);
        color4B getGlowColor() {return mGlowColor;}
        void setBrightness(float brightness) { mBrightness = brightness; }
        float getBrightness() {return mBrightness;}
        void setShowGrayImage(bool b) { mShowGrayImage = b; }
        bool isShowGrayImage() {return mShowGrayImage;}
        void setFlipX(bool b) {if(mFlipX == b) return; mFlipX = b; if(mFlipX) mMatrixDirty = true;}
        void setFlipY(bool b) {if(mFlipY == b) return; mFlipY = b; if(mFlipY) mMatrixDirty = true;}
        bool isFlipX() {return mFlipX;}
        bool isFlipY() {return mFlipY;}
        void setRotation(float rot)
        {
            if (mRotation == rot)
            {
                return;
            }
            if (currAnimations)
            {
                propertyChanged(this, "imgrotation", mRotation, rot);
                return;
            }
            mRotation = rot;
            mMatrixDirty = true;
        }
        float getRotation() {return mRotation;}
        void setRepeatStretch(bool b) {mRepeatStretch = b; if(mSpriteFrame->_texture)updateQuads();}
        bool isRepeatStretch() {return mRepeatStretch;}
        void setBlendFunc(GLenum srcfactor, GLenum dstfactor) {mSrcFactor = srcfactor; mDstFactor = dstfactor;}
        void setGlowRadius(float value);
        float getGlowRadius() {return mGlowRadius;}
        virtual void onDraw();
        HLSpriteFrame* getSpriteFrame(){return mSpriteFrame;};

#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
        
        virtual HLView* clone(HLView* parent);
    protected:
        virtual void onInternalEvent(const char* event);
    private:
        void initMembers();
        void generateGlowTexture();
        void drawGlow();
        void drawSelf();
        color4B mTintColor;
        color4B mGlowColor;
        HLRect mCapInsets; // in percent
        HLSpriteFrame* mSpriteFrame;
        GLenum mSrcFactor;
        GLenum mDstFactor;
//        HLRect mTextureRect;
//        HLTexture* mTexture;
//        HLPoint mOffset;
//        HLSize mSourceSize;
//        bool mRotated;
        float mBrightness;
        bool mShowGrayImage;
        bool mFlipX;
        bool mFlipY;
        bool mMatrixDirty;
        kmMat4 mMatrix;
        float mRotation;
        bool mRepeatStretch;
        void updateQuads();
        void updateQuads(HLRect texRect, HLRect drawRect, bool repeat = false);
//#if HL_USE_VAO
//        void setupVBOandVAO();
//        GLuint mVAO;
//        GLuint mVBO[2];
//#endif
        std::vector<GLushort> mIndices;
        std::vector<V3F_C4B_T2F_Quad> mQuads;
        HLSpriteFrame* mGlowTexture{nullptr};
        float mGlowRadius{0.0};
        HLTimer* mCheckTimer{nullptr};
        void checkLoading(HLTimer* timer, float dt);
    };
}

NS_HL_END

#endif
