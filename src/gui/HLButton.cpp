//
//  HLButton.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLButton.h"
#include "HLTexture.h"
#include "HLGLMatrixStack.h"

#if USE_JS_SCRIPTING
#include "JSButton.h"
#endif

NS_HL_BEGIN

namespace gui
{
    HLButton::HLButton(HLView * parent):HLView(parent)
    {
        initMembers();
    }
    
    HLButton::HLButton(HLView *parent, HLRect frame):HLView(parent, frame)
    {
        initMembers();
    }
    
    HLButton::HLButton(HLView *parent, const HLButton& copy):HLView(parent, copy)
    {
        mNormalBackgroundTexture = NULL;
        mDisableBackgroundTexture = NULL;
        mSelectedBackgroundTexture = NULL;
        mHighlightedBackgroundTexture = NULL;
        mButtonState = kButtonStateNormal;
        mButtonStateBeforeHighlight = kButtonStateNormal;
        mBackgroundImageView = new HLImageView(this, *(copy.mBackgroundImageView));
        mTitleLabel = new HLLabel(this, *(copy.mTitleLabel));
        mUserInteractionEnabled = true;
        mNormalBackgroundTextureCapInsets = HLRectZero;
        mDisableBackgroundTextureCapInsets = HLRectZero;
        mSelectedBackgroundTextureCapInsets = HLRectZero;
        mHighlightedBackgroundTextureCapInsets = HLRectZero;
        mZoom = false;
        mZoomOnTouchDown = false;
        if (copy.mNormalBackgroundTexture)
        {
            setNormalBackgroundImage(copy.mNormalBackgroundTexture, copy.mNormalBackgroundTextureCapInsets);
        }
        if (copy.mDisableBackgroundTexture)
        {
            setDisabledBackgroundImage(copy.mDisableBackgroundTexture, copy.mDisableBackgroundTextureCapInsets);
        }
        if (copy.mSelectedBackgroundTexture)
        {
            setSelectedBackgroundImage(copy.mSelectedBackgroundTexture, copy.mSelectedBackgroundTextureCapInsets);
        }
        if (copy.mHighlightedBackgroundTexture)
        {
            setHighlightedBackgroundImage(copy.mHighlightedBackgroundTexture, copy.mHighlightedBackgroundTextureCapInsets);
        }
        setZoomOnTouchDown(copy.mZoomOnTouchDown);
    }
    
    HLButton::~HLButton()
    {
#if USE_JS_SCRIPTING
        JSButton::removeJSObject(this);
#endif
        if (mNormalBackgroundTexture)
        {
            mNormalBackgroundTexture->release();
        }
        if (mDisableBackgroundTexture)
        {
            mDisableBackgroundTexture->release();
        }
        if (mSelectedBackgroundTexture)
        {
            mSelectedBackgroundTexture->release();
        }
        if (mHighlightedBackgroundTexture)
        {
            mHighlightedBackgroundTexture->release();
        }
    }
    
    void HLButton::initMembers()
    {
        mNormalBackgroundTexture = NULL;
        mDisableBackgroundTexture = NULL;
        mSelectedBackgroundTexture = NULL;
        mHighlightedBackgroundTexture = NULL;
        mButtonState = kButtonStateNormal;
        mButtonStateBeforeHighlight = kButtonStateNormal;
        mBackgroundImageView = new HLImageView(this, HLRect(0,0,mBounds.size.width,mBounds.size.height));
        mTitleLabel = new HLLabel(this, HLRect(0,0,mBounds.size.width,mBounds.size.height));
        mTitleLabel->setFontSize(13);
        mUserInteractionEnabled = true;
        mNormalBackgroundTextureCapInsets = HLRectZero;
        mDisableBackgroundTextureCapInsets = HLRectZero;
        mSelectedBackgroundTextureCapInsets = HLRectZero;
        mHighlightedBackgroundTextureCapInsets = HLRectZero;
        mZoom = false;
        mZoomOnTouchDown = false;
    }
    
    void HLButton::setHighlighted(bool highlighted)
    {
        if (highlighted)
        {
            mButtonStateBeforeHighlight = mButtonState;
            mButtonState = kButtonStateHighlighted;
        }
        else
        {
            mButtonState = mButtonStateBeforeHighlight;
        }
        statusChanged();
    }
    
    void HLButton::onInternalEvent(const char* event)
    {
        if (!strcmp(event, "size_changed") || !strcmp(event, "frame_changed") || !strcmp(event, "bounds_changed"))
        {
            mBackgroundImageView->setSize(mBounds.size);
            mTitleLabel->setSize(mBounds.size);
        }
    }
    
    void HLButton::setNormalBackgroundImage(HLTexture* tex, HLRect capInsets)
    {
        if (!tex)
        {
            if (mNormalBackgroundTexture)
            {
                mNormalBackgroundTexture->release();
                mNormalBackgroundTexture = nullptr;
            }
            return;
        }
        tex->retain();
        if (mNormalBackgroundTexture)
        {
            mNormalBackgroundTexture->release();
        }
        mNormalBackgroundTexture = new HLSpriteFrame();
        mNormalBackgroundTexture->_texture = tex;
        mNormalBackgroundTexture->_rotated = false;
        mNormalBackgroundTexture->_sourceSize = tex->getImageSize()*tex->getScale();
        mNormalBackgroundTexture->_offset = HLPointZero;
        mNormalBackgroundTexture->_textureRect.origin = HLPointZero;
        mNormalBackgroundTexture->_textureRect.size = mNormalBackgroundTexture->_sourceSize;
        mNormalBackgroundTextureCapInsets = capInsets;
        if (mButtonState == kButtonStateNormal)
        {
            statusChanged();
        }
    }
    
    void HLButton::setNormalBackgroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets)
    {
        if (!spriteFrame)
        {
            if (mNormalBackgroundTexture)
            {
                mNormalBackgroundTexture->release();
                mNormalBackgroundTexture = nullptr;
            }
            return;
        }
        spriteFrame->retain();
        if (mNormalBackgroundTexture)
        {
            mNormalBackgroundTexture->release();
        }
        mNormalBackgroundTexture = spriteFrame;
        mNormalBackgroundTextureCapInsets = capInsets;
        if (mButtonState == kButtonStateNormal)
        {
            statusChanged();
        }
    }
    
    void HLButton::setDisabledBackgroundImage(HLTexture* tex, HLRect capInsets)
    {
        if (!tex)
        {
            if (mNormalBackgroundTexture)
            {
                mDisableBackgroundTexture->release();
                mDisableBackgroundTexture = nullptr;
            }
            return;
        }
        tex->retain();
        if (mDisableBackgroundTexture)
        {
            mDisableBackgroundTexture->release();
        }
        mDisableBackgroundTexture = new HLSpriteFrame();
        mDisableBackgroundTexture->_texture = tex;
        mDisableBackgroundTexture->_rotated = false;
        mDisableBackgroundTexture->_sourceSize = tex->getImageSize()*tex->getScale();
        mDisableBackgroundTexture->_offset = HLPointZero;
        mDisableBackgroundTexture->_textureRect.origin = HLPointZero;
        mDisableBackgroundTexture->_textureRect.size = mDisableBackgroundTexture->_sourceSize;
        mDisableBackgroundTextureCapInsets = capInsets;
        if (mButtonState == kButtonStateDisabled)
        {
            statusChanged();
        }
    }
    
    void HLButton::setDisabledBackgroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets)
    {
        if (!spriteFrame)
        {
            if (mNormalBackgroundTexture)
            {
                mDisableBackgroundTexture->release();
                mDisableBackgroundTexture = nullptr;
            }
            return;
        }
        spriteFrame->retain();
        if (mDisableBackgroundTexture)
        {
            mDisableBackgroundTexture->release();
        }
        mDisableBackgroundTexture = spriteFrame;
        mDisableBackgroundTextureCapInsets = capInsets;
        if (mButtonState == kButtonStateDisabled)
        {
            statusChanged();
        }
    }
    
    void HLButton::setSelectedBackgroundImage(HLTexture* tex, HLRect capInsets)
    {
        if (!tex)
        {
            if (mNormalBackgroundTexture)
            {
                mSelectedBackgroundTexture->release();
                mSelectedBackgroundTexture = nullptr;
            }
            return;
        }
        tex->retain();
        if (mSelectedBackgroundTexture)
        {
            mSelectedBackgroundTexture->release();
        }
        mSelectedBackgroundTexture = new HLSpriteFrame();
        mSelectedBackgroundTexture->_texture = tex;
        mSelectedBackgroundTexture->_rotated = false;
        mSelectedBackgroundTexture->_sourceSize = tex->getImageSize()*tex->getScale();
        mSelectedBackgroundTexture->_offset = HLPointZero;
        mSelectedBackgroundTexture->_textureRect.origin = HLPointZero;
        mSelectedBackgroundTexture->_textureRect.size = mSelectedBackgroundTexture->_sourceSize;
        mSelectedBackgroundTextureCapInsets = capInsets;
        if (mButtonState == kButtonStateSelected)
        {
            statusChanged();
        }
    }
    
    void HLButton::setSelectedBackgroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets)
    {
        if (!spriteFrame)
        {
            if (mNormalBackgroundTexture)
            {
                mSelectedBackgroundTexture->release();
                mSelectedBackgroundTexture = nullptr;
            }
            return;
        }
        spriteFrame->retain();
        if (mSelectedBackgroundTexture)
        {
            mSelectedBackgroundTexture->release();
        }
        mSelectedBackgroundTexture = spriteFrame;
        mSelectedBackgroundTextureCapInsets = capInsets;
        if (mButtonState == kButtonStateSelected)
        {
            statusChanged();
        }
    }
    
    void HLButton::setHighlightedBackgroundImage(HLTexture* tex, HLRect capInsets)
    {
        if (!tex)
        {
            if (mNormalBackgroundTexture)
            {
                mHighlightedBackgroundTexture->release();
                mHighlightedBackgroundTexture = nullptr;
            }
            return;
        }
        tex->retain();
        if (mHighlightedBackgroundTexture)
        {
            mHighlightedBackgroundTexture->release();
        }
        mHighlightedBackgroundTexture = new HLSpriteFrame();
        mHighlightedBackgroundTexture->_texture = tex;
        mHighlightedBackgroundTexture->_rotated = false;
        mHighlightedBackgroundTexture->_sourceSize = tex->getImageSize()*tex->getScale();
        mHighlightedBackgroundTexture->_offset = HLPointZero;
        mHighlightedBackgroundTexture->_textureRect.origin = HLPointZero;
        mHighlightedBackgroundTexture->_textureRect.size = mHighlightedBackgroundTexture->_sourceSize;
        mHighlightedBackgroundTextureCapInsets = capInsets;
        if (mButtonState == kButtonStateHighlighted)
        {
            statusChanged();
        }
    }
    
    void HLButton::setHighlightedBackgroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets)
    {
        if (!spriteFrame)
        {
            if (mNormalBackgroundTexture)
            {
                mHighlightedBackgroundTexture->release();
                mHighlightedBackgroundTexture = nullptr;
            }
            return;
        }
        spriteFrame->retain();
        if (mHighlightedBackgroundTexture)
        {
            mHighlightedBackgroundTexture->release();
        }
        mHighlightedBackgroundTexture = spriteFrame;
        mHighlightedBackgroundTextureCapInsets = capInsets;                                                                                   
        if (mButtonState == kButtonStateHighlighted)
        {
            statusChanged();
        }
    }
    
    void HLButton::statusChanged()
    {
        switch (mButtonState)
        {
            case kButtonStateNormal:
            {
                mTitleLabel->setText(mNormalTileText);
                if (mNormalBackgroundTexture)
                {
                    mBackgroundImageView->setImage(mNormalBackgroundTexture, mNormalBackgroundTextureCapInsets);
                    mBackgroundImageView->setShowGrayImage(false);
                    mBackgroundImageView->setBrightness(1);
                    mTitleLabel->setBrightness(1);
                }
                else
                {
                    mBackgroundImageView->setImage((HLSpriteFrame*)NULL);
                    mTitleLabel->setBrightness(1);
                }
            }
                break;
            case kButtonStateDisabled:
            {
                if (mDisableTitleText.length()>0)
                {
                    mTitleLabel->setText(mDisableTitleText);
                }
                if (mDisableBackgroundTexture)
                {
                    mBackgroundImageView->setImage(mDisableBackgroundTexture, mDisableBackgroundTextureCapInsets);
                    mBackgroundImageView->setShowGrayImage(false);
                    mBackgroundImageView->setBrightness(1);
                    mTitleLabel->setBrightness(1);
                }
                else if (mNormalBackgroundTexture)
                {
                    mBackgroundImageView->setShowGrayImage(true);
                    mBackgroundImageView->setBrightness(1);
                    mTitleLabel->setBrightness(1);
                }
                else
                {
                    mBackgroundImageView->setImage((HLSpriteFrame*)NULL);
                    mTitleLabel->setBrightness(1);
                }
            }
                break;
            case kButtonStateSelected:
            {
                if (mSelectedTitleText.length()>0)
                {
                    mTitleLabel->setText(mSelectedTitleText);
                }
                if (mSelectedBackgroundTexture)
                {
                    mBackgroundImageView->setImage(mSelectedBackgroundTexture, mSelectedBackgroundTextureCapInsets);
                    mBackgroundImageView->setShowGrayImage(false);
                    mBackgroundImageView->setBrightness(1);
                    mTitleLabel->setBrightness(1);
                }
                else if (mNormalBackgroundTexture)
                {
                    mBackgroundImageView->setShowGrayImage(false);
                    mBackgroundImageView->setBrightness(1.2);
                    mTitleLabel->setBrightness(1.2);
                }
                else
                {
                    mBackgroundImageView->setImage((HLSpriteFrame*)NULL);
                    mTitleLabel->setBrightness(1.2);
                }
            }
                break;
            case kButtonStateHighlighted:
            {
                if (mHighlightedTitleText.length()>0)
                {
                    mTitleLabel->setText(mHighlightedTitleText);
                }
                if (mHighlightedBackgroundTexture)
                {
                    mBackgroundImageView->setImage(mHighlightedBackgroundTexture, mHighlightedBackgroundTextureCapInsets);
                    mBackgroundImageView->setShowGrayImage(false);
                    mBackgroundImageView->setBrightness(1);
                    mTitleLabel->setBrightness(1);
                }
//                else if (mNormalBackgroundTexture)
//                {
//                    mBackgroundImageView->setShowGrayImage(false);
//                    mBackgroundImageView->setBrightness(0.8);
//                    mTitleLabel->setBrightness(0.8);
//                }
                else
                {
//                    mBackgroundImageView->setImage((HLSpriteFrame*)NULL);
                    mBackgroundImageView->setBrightness(0.8);
                    mTitleLabel->setBrightness(0.8);
                }
            }
                break;
            default:
                printf("unknown button state");
                abort();
                break;
        }
    }
    
    void HLButton::onTouches(HLTouchEvent* touch)
    {
        if (mButtonState == kButtonStateDisabled)
        {
            return;
        }
        HLTouch& atouch = *touch->touches.begin();
        HLPoint p = touchLocationInView(&atouch);
        switch (touch->type)
        {
            case kHLTouchesBegin:
                setHighlighted(true);
                if (mZoomOnTouchDown)
                {
                    mZoom = true;
                }
                break;
            case kHLTouchesMove:
            {
                if (p.x<0 || p.y<0 || p.x>mBounds.size.width || p.y>mBounds.size.height)
                {
                    if (mButtonState == kButtonStateHighlighted)
                    {
                        setHighlighted(false);
                        if (mZoomOnTouchDown)
                        {
                            mZoom = false;
                        }
                    }
                }
                else
                {
                    if (mButtonState != kButtonStateHighlighted)
                    {
                        setHighlighted(true);
                        if (mZoomOnTouchDown)
                        {
                            mZoom = true;
                        }
                    }
                }
            }
                break;
            case kHLTouchesEnd:
            {
                if (mButtonState == kButtonStateHighlighted)
                {
                    setHighlighted(false);
                    if (mZoomOnTouchDown)
                    {
                        mZoom = false;
                    }
                }
                if (p.x>=0 && p.y>=0 && p.x<=mBounds.size.width && p.y<mBounds.size.height)
                {
                    onButtonClick(this);
                }
            }
                break;
            case kHLTouchesCancel:
            {
                if (mButtonState == kButtonStateHighlighted)
                {
                    setHighlighted(false);
                    
                    if (mZoomOnTouchDown)
                    {
                        mZoom = false;
                    }
                }
            }
                break;
            default:
                break;
        }
    }
    
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    void HLButton::onMouseEvent(HLMouseEvent* mouseEvent)
    {
        if (mButtonState == kButtonStateDisabled)
        {
            return;
        }
        HLPoint p = mouseLocationInView(mouseEvent);
        switch (mouseEvent->type)
        {
            case kHLMouseDown:
                setHighlighted(true);
                if (mZoomOnTouchDown)
                {
                    mZoom = true;
                }
                break;
            case kHLMouseDrag:
            {
                if (p.x<0 || p.y<0 || p.x>mBounds.size.width || p.y>mBounds.size.height)
                {
                    if (mButtonState == kButtonStateHighlighted)
                    {
                        setHighlighted(false);
                        if (mZoomOnTouchDown)
                        {
                            mZoom = false;
                        }
                    }
                }
                else
                {
                    if (mButtonState != kButtonStateHighlighted)
                    {
                        setHighlighted(true);
                        if (mZoomOnTouchDown)
                        {
                            mZoom = true;
                        }
                    }
                }
            }
                break;
            case kHLMouseUp:
            {
                if (mButtonState == kButtonStateHighlighted)
                {
                    setHighlighted(false);
                    if (mZoomOnTouchDown)
                    {
                        mZoom = false;
                    }
                }
                if (p.x>=0 && p.y>=0 && p.x<=mBounds.size.width && p.y<mBounds.size.height)
                {
                    onButtonClick(this);
                }
            }
                break;
            case kHLMouseCancel:
            {
                if (mButtonState == kButtonStateHighlighted)
                {
                    setHighlighted(false);
                    if (mZoomOnTouchDown)
                    {
                        mZoom = false;
                    }
                }
            }
                break;
            default:
                break;
        }
    }
#endif
    
    void HLButton::visit()
    {
        if (!mVisible)
        {
            return;
        }
        HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
        if (mZoom)
        {
            matrixStack->pushMatrix();
            matrixStack->multMatrix(getTransform());
            kmMat4 matrix;
            kmMat4Scaling(&matrix, 1.1f, 1.1f, 1);
            matrix.mat[12] = -mBounds.size.width*0.05f;
            matrix.mat[13] = -mBounds.size.height*0.05f;
            matrixStack->multMatrix(&matrix);
            kmMat4Inverse(&matrix, getTransform());
            matrixStack->multMatrix(&matrix);
        }
        HLView::visit();
        if (mZoom)
        {
            matrixStack->popMatrix();
        }
    }
    
    HLSpriteFrame* HLButton::getNormalBackgroundSpriteFrame()
    {
        return mNormalBackgroundTexture;
    }
    HLSpriteFrame* HLButton::getSelectedBackgroundSpriteFrame()
    {
        return mSelectedBackgroundTexture;
    }
    HLSpriteFrame* HLButton::getDisableBackgroundSpriteFrame()
    {
        return mDisableBackgroundTexture;
    }
    HLSpriteFrame* HLButton::getHighlightedBackgroundSpriteFrame()
    {
        return mHighlightedBackgroundTexture;
    }
    
#if USE_JS_SCRIPTING
    JSCppWrapperBase* HLButton::getOrCreateJSWrapper(JSContext* cx)
    {
        return JSButton::getOrCreateWrapper(cx, this);
    }
#endif
    
    HLView* HLButton::clone(HLView* parent)
    {
        HLButton* view = new HLButton(parent, *this);
        std::list<HLView*>::iterator itr = mSubViews.begin();
        for (; itr != mSubViews.end(); ++itr)
        {
            if (*itr != mBackgroundImageView && *itr != mTitleLabel)
            {
                (*itr)->clone(view);
            }
        }
        return view;
    }
}

NS_HL_END
