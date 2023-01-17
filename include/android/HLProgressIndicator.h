//
//  HLProgressView.h
//  HoolaiEngine
//
//  Created by cai wei on 03/05/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HoolaiEngine__HLProgressView__
#define __HoolaiEngine__HLProgressView__

#include <iostream>
#include "HLMacros.h"
#include "HLView.h"
#include "HLLabel.h"
#include "HLDelegate.h"

NS_HL_BEGIN

class HLTimer;

namespace gui
{
class HLProgressBar;

class HLProgressBar : public HLView
{
public:
    typedef enum
    {
        kProgressBarStyleNormal = 1,
        kProgressBarStyleImg = 1<<1,
        kProgressBarStyleRepeat = 1<<2,
    } HLProgressBarStyle;

    typedef enum
    {
        kProgressBarTextStyleNull,
        kProgressBarTextStylePercent,
        kProgressBarTextStyleValue,
    } HLProgressBarTextStyle;

    HLProgressBar(HLView* parent, HLRect frame,int style=kProgressBarStyleNormal);
    virtual ~HLProgressBar();

    void setRange(int minimun,int maximun);
    int getMinimun();
    int getMaximun();
    void setValue(int value);
    int getValue();
    void setValue1(int value);
    int getValue1();
    HLLabel* getTextLabel();
    void setTextStyle(int style);
    int getTextStyle();
    void onInternalEvent(const char* event);
/*    virtual void onTouches(HLTouchEvent* touch);
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    virtual void onMouseEvent(HLMouseEvent* mouseEvent);
#endif*/
    void setBackgroundImage(HLTexture* tex, HLRect capInsets = HLRectZero);
    void setBackgroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets = HLRectZero);
    void setForegroundImage(HLTexture* tex, HLRect capInsets = HLRectZero);
    void setForegroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets = HLRectZero);
    void setRepeatForegroundImage(HLTexture* tex, HLRect capInsets = HLRectZero);
    void setRepeatForegroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets = HLRectZero);
    virtual void onDraw();
private:
    int mMinimun;//进度条起点
    int mMaximun;//进度条终点
    int mValue;//进度条增长的值--该值介于mMinimun与mMaximun之间
    int mValue1;//进度条增长的值--该值介于mMinimun与mMaximun之间
    float mStep;//进度条步长
    HLLabel* mValueLabel;
    HLRect mCapInsets; // in percent
    int mProgressBarStyle;
    int mProgressBarTextStyle;
    HLImageView* mBackgroundImageView;
    HLImageView* mForegroundImageView;
    HLImageView* mRepeatForegroundImageView;
    HLImageView* mButtonImageView;
    HLSpriteFrame* mBackgroundTexture;
    HLSpriteFrame* mForegroundTexture;
    HLSpriteFrame* mRepeatForegroundTexture;
    HLView *mParentForegroundView;
    HLView *mParentRepeatForegroundView;
};
}

NS_HL_END
#endif /* defined(__HoolaiEngine__HLProgressView__) */
