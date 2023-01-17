//
//  HLProgressRadialView.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-5-9.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLPROGRESSRADIALVIEW_H__
#define __HLPROGRESSRADIALVIEW_H__

#include "HLView.h"

NS_HL_BEGIN

namespace gui
{
    typedef enum {
        kHLProgressRadialTypeCCW,
        kHLProgressRadialTypeCW
    } HLProgressRadialType;
    
    class HLProgressRadialView: public HLView
    {
    public:
        HLProgressRadialView(HLView* parent, HLRect frame, HLProgressRadialType type = kHLProgressRadialTypeCCW):HLView(parent, frame), mType(type), mPercentage(0), mImageView(NULL)
        {
            mColor.r=mColor.g=mColor.b=0;mColor.a=0.5;
            mStartDegree = 0;
            mEndDegree = 0;
            mCenter.x = mCenter.y = 0.5;
        }
        HLProgressRadialView(HLView* parent, HLProgressRadialType type = kHLProgressRadialTypeCCW):HLView(parent), mType(type), mPercentage(0)
        {
            mColor.r=mColor.g=mColor.b=0;mColor.a=0.5;
            mStartDegree = 0;
            mEndDegree = 0;
            mCenter.x = mCenter.y = 0.5;
        }
        virtual ~HLProgressRadialView();
        float getProgress() {return mPercentage;}
        void setProgress(float p);
        
        void setColor(color4F color);
        void setImage(HLTexture* tex, HLRect capInset = HLRectZero);
        void setImage(HLSpriteFrame* spf, HLRect capInset = HLRectZero);
        
        void setStartAngle(float degree);
        void setEndAngle(float degree);
        void setCenterPoint(HLPoint center); // in percent
        
#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
        
    protected:
        virtual void onDraw();
        virtual void onInternalEvent(const char* event);
    private:
        HLProgressRadialType mType;
        float mPercentage;
        HLImageView *mImageView;
        color4F mColor;
        float mStartDegree;
        float mEndDegree;
        HLPoint mCenter;
        
        std::vector<HLPoint> mVerts;
    };
}

NS_HL_END

#endif
