//
//  HLProgressView.cpp
//  HoolaiEngine
//
//  Created by cai wei on 03/05/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLProgressIndicator.h"
#include "HLDrawingPrimitives.h"
#include "kazmath/GL/matrix.h"
#include "HLImageView.h"
#include "HLTexture.h"

NS_HL_BEGIN

namespace gui
{
HLProgressBar::HLProgressBar(HLView *parent, HLRect frame, int style):HLView(parent, frame)
{
    mMinimun = 0;
    mMaximun = 100;
    mValue = 0;
    mValue1 = 0;
    mStep = (frame.size.width-1)/(mMaximun-mMinimun);
    mValueLabel = NULL;
    mCapInsets = frame;
    mProgressBarStyle = style;
    mProgressBarTextStyle = kProgressBarTextStylePercent;
    mBackgroundImageView = NULL;
    mForegroundImageView = NULL;
    mRepeatForegroundImageView = NULL;
    mButtonImageView = NULL;
    mBackgroundTexture = NULL;
    mForegroundTexture = NULL;
    mRepeatForegroundTexture = NULL;
    mParentForegroundView = NULL;
    mParentRepeatForegroundView = NULL;

    if(mProgressBarStyle & kProgressBarStyleNormal)
    {
    }
    else if((mProgressBarStyle & kProgressBarStyleImg) || (mProgressBarStyle & kProgressBarStyleRepeat))
    {
        HLRect mRect = HLRect(0,0,mBounds.size.width,mBounds.size.height);
        mBackgroundImageView = new HLImageView(this,mRect);
//        setBackgroundImage(HLTexture::getTexture("ProgressBar_1.png"));
        if(mProgressBarStyle & kProgressBarStyleRepeat)
        {
            mParentRepeatForegroundView = new HLView(this, HLRectZero);
            mRepeatForegroundImageView = new HLImageView(mParentRepeatForegroundView, mRect);
            mParentRepeatForegroundView->setClipToBounds(true);
//            setRepeatForegroundImage(HLTexture::getTexture("ProgressBar_3.png"));
        }
        if(mProgressBarStyle & kProgressBarStyleImg)
        {
            mParentForegroundView = new HLView(this, HLRectZero);
            mForegroundImageView = new HLImageView(mParentForegroundView, mRect);
            mParentForegroundView->setClipToBounds(true);
//            setForegroundImage(HLTexture::getTexture("ProgressBar_2.png"));
        }
    }
    mValueLabel = new HLLabel(this, HLRect(0,0,mBounds.size.width,mBounds.size.height));
    mValueLabel->setFontSize(mCapInsets.size.height*3/4);
    color4B c1 = {0, 0, 0, 255};
    mValueLabel->setTextColor(c1);
    mValueLabel->setTextAlignment(kTextAlignmentCenter);
}

HLProgressBar::~HLProgressBar()
{
}

void HLProgressBar::setRange(int minimun,int maximun)
{
    mMinimun = MIN(minimun,maximun);
    mMaximun = MAX(minimun,maximun);
    mStep = (mCapInsets.size.width-1)/(mMaximun-mMinimun);
}

int HLProgressBar::getMinimun()
{
    return mMinimun;
}

int HLProgressBar::getMaximun()
{
    return mMaximun;
}

void HLProgressBar::setValue(int value)
{
    mValue = value;
    if(mValue > mMaximun)
    {
        mValue = mMaximun;
    }
    if(mValue < mMinimun)
    {
        mValue = mMinimun;
    }
}

int HLProgressBar::getValue()
{
    return mValue;
}

void HLProgressBar::setValue1(int value)
{
    mValue1 = value;
    if(mValue1 > mMaximun)
    {
        mValue1 = mMaximun;
    }
    if(mValue1 < mMinimun)
    {
        mValue1 = mMinimun;
    }
}

int HLProgressBar::getValue1()
{
    return mValue1;
}

HLLabel* HLProgressBar::getTextLabel()
{
    return mValueLabel;
}

void HLProgressBar::setTextStyle(int style)
{
    mProgressBarTextStyle = style;
}

int HLProgressBar::getTextStyle()
{
    return mProgressBarTextStyle;
}

void HLProgressBar::onInternalEvent(const char* event)
{
    if (!strcmp(event, "size_changed") || !strcmp(event, "frame_changed") || !strcmp(event, "bounds_changed"))
    {
        mBackgroundImageView->setSize(mBounds.size);
        mForegroundImageView->setSize(mBounds.size);
        mRepeatForegroundImageView->setSize(mBounds.size);
        mButtonImageView->setSize(mBounds.size);
        mValueLabel->setSize(mBounds.size);
    }
}

/*void HLProgressBar::onTouches(HLTouchEvent* touch)
{
    HLTouch& atouch = *touch->touches.begin();
    HLPoint p = touchLocationInView(&atouch);
    switch (touch->type)
    {
    case kHLTouchesBegin:
    case kHLTouchesMove:
    {
        if (p.x<0 || p.y<0 || p.x>mBounds.size.width || p.y>mBounds.size.height)
        {
        }
    }
    break;
    case kHLTouchesEnd:
    {
    }
    break;
    case kHLTouchesCancel:
    {
    }
    break;
    default:
        break;
    }
}

#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
void HLProgressBar::onMouseEvent(HLMouseEvent* mouseEvent)
{
}
#endif*/

void HLProgressBar::setBackgroundImage(HLTexture* tex, HLRect capInsets)
{
    tex->retain();
    if (mBackgroundTexture)
    {
        mBackgroundTexture->release();
    }
    mBackgroundTexture = new HLSpriteFrame();
    mBackgroundTexture->_texture = tex;
    mBackgroundTexture->_rotated = false;
    mBackgroundTexture->_sourceSize = tex->getImageSize()*tex->getScale();
    mBackgroundTexture->_offset = HLPointZero;
    mBackgroundTexture->_textureRect.origin = HLPointZero;
    mBackgroundTexture->_textureRect.size = mBackgroundTexture->_sourceSize;
    //mBackgroundTextureCapInsets = capInsets;
    if(mBackgroundImageView)
    {
        mBackgroundImageView->setImage(mBackgroundTexture);
    }
}

void HLProgressBar::setBackgroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets)
{
    spriteFrame->retain();
    if (mBackgroundTexture)
    {
        mBackgroundTexture->release();
    }
    mBackgroundTexture = spriteFrame;
    //mBackgroundTextureCapInsets = capInsets;
    if(mBackgroundImageView)
    {
        mBackgroundImageView->setImage(mBackgroundTexture);
    }
}

void HLProgressBar::setForegroundImage(HLTexture* tex, HLRect capInsets)
{
    tex->retain();
    if (mForegroundTexture)
    {
        mForegroundTexture->release();
    }
    mForegroundTexture = new HLSpriteFrame();
    mForegroundTexture->_texture = tex;
    mForegroundTexture->_rotated = false;
    mForegroundTexture->_sourceSize = tex->getImageSize()*tex->getScale();
    mForegroundTexture->_offset = HLPointZero;
    mForegroundTexture->_textureRect.origin = HLPointZero;
    mForegroundTexture->_textureRect.size = mForegroundTexture->_sourceSize;
    //mForegroundTextureCapInsets = capInsets;
    if(mForegroundImageView)
    {
        mForegroundImageView->setImage(mForegroundTexture);
    }
}

void HLProgressBar::setForegroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets)
{
    spriteFrame->retain();
    if (mForegroundTexture)
    {
        mForegroundTexture->release();
    }
    mForegroundTexture = spriteFrame;
    //mForegroundTextureCapInsets = capInsets;
    if(mForegroundImageView)
    {
        mForegroundImageView->setImage(mForegroundTexture);
    }
}

void HLProgressBar::setRepeatForegroundImage(HLTexture* tex, HLRect capInsets)
{
    tex->retain();
    if (mRepeatForegroundTexture)
    {
        mRepeatForegroundTexture->release();
    }
    mRepeatForegroundTexture = new HLSpriteFrame();
    mRepeatForegroundTexture->_texture = tex;
    mRepeatForegroundTexture->_rotated = false;
    mRepeatForegroundTexture->_sourceSize = tex->getImageSize()*tex->getScale();
    mRepeatForegroundTexture->_offset = HLPointZero;
    mRepeatForegroundTexture->_textureRect.origin = HLPointZero;
    mRepeatForegroundTexture->_textureRect.size = mRepeatForegroundTexture->_sourceSize;
    //mRepeatForegroundTextureCapInsets = capInsets;
    if(mRepeatForegroundImageView)
    {
        mRepeatForegroundImageView->setImage(mRepeatForegroundTexture);
    }
}

void HLProgressBar::setRepeatForegroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets)
{
    spriteFrame->retain();
    if (mRepeatForegroundTexture)
    {
        mRepeatForegroundTexture->release();
    }
    mRepeatForegroundTexture = spriteFrame;
    //mRepeatForegroundTextureCapInsets = capInsets;
    if(mRepeatForegroundImageView)
    {
        mRepeatForegroundImageView->setImage(mRepeatForegroundTexture);
    }
}

void HLProgressBar::onDraw()
{
   // int mPercent = (mValue-mMinimun)*100/(mMaximun-mMinimun);
    if(mProgressBarTextStyle == kProgressBarTextStylePercent)
    {
        char textChar[20];
        sprintf(textChar, "%d%s", (mValue-mMinimun)*100/(mMaximun-mMinimun),"%");
        mValueLabel->setText(textChar);
    }
    else if(mProgressBarTextStyle == kProgressBarTextStyleValue)
    {
        char textChar[40];
        sprintf(textChar, "%d/%d", mValue,mMaximun);
        mValueLabel->setText(textChar);
    }


    if(mProgressBarStyle & kProgressBarStyleNormal)
    {
        //simple Proress
        drawColor4F(0.7, 0.75, 0.56, 1);
        drawRect(HLPoint(0, 0), HLPoint(mBounds.size.width, mBounds.size.height));
        drawColor4F(1.0, 1.0, 1.0, 1);
        drawSolidRect(HLPoint(0, 0), HLPoint(mBounds.size.width, mBounds.size.height));
        if((mProgressBarStyle & kProgressBarStyleRepeat) && (mValue1 > mValue) && (mValue > mMinimun))
        {
            drawColor4F(0.3, 0.3, 0.3, 1);
            drawSolidRect(HLPoint(1, 1), HLPoint(mStep*(mValue1-mMinimun), mBounds.size.height - 1));
        }
        if(mValue > mMinimun)
        {
            drawColor4F(0.7, 0.84, 0.19, 1);
            drawSolidRect(HLPoint(1, 1), HLPoint(mStep*(mValue-mMinimun), mBounds.size.height - 1));
        }
    }
    else if((mProgressBarStyle & kProgressBarStyleImg) || (mProgressBarStyle & kProgressBarStyleRepeat))
    {
        float mWidth = mBounds.size.width*(mValue-mMinimun)/(mMaximun-mMinimun);
        if(mProgressBarStyle & kProgressBarStyleImg)
        {
            mParentForegroundView->setSize(mWidth,mBounds.size.height);
        }
        if(mProgressBarStyle & kProgressBarStyleRepeat)
        {
            float mWidth1 = mBounds.size.width*(mValue1-mMinimun)/(mMaximun-mMinimun);
            if(mWidth1 > mWidth)
            {
                mParentRepeatForegroundView->setFrame(HLRect(mWidth,0,mWidth1-mWidth,mBounds.size.height));
                ((HLView*)mRepeatForegroundImageView)->setFrame(HLRect(-mWidth,0,mBounds.size.width,mBounds.size.height));
            }
        }
    }
    
    HLView::onDraw();
}
}

NS_HL_END