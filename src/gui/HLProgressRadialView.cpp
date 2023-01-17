//
//  HLProgressRadialView.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-5-9.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLProgressRadialView.h"
#include "HLDrawingPrimitives.h"
#include "HLGLMatrixStack.h"
#include "HLImageView.h"
#include "JSProgressRadialView.h"
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif

NS_HL_BEGIN

namespace gui
{
    HLProgressRadialView::~HLProgressRadialView()
    {
#if USE_JS_SCRIPTING
        JSProgressRadialView::removeJSObject(this);
#endif
    }
    
    void HLProgressRadialView::onDraw()
    {
        if (mPercentage == 0)
        {
            std::list<HLView*>::const_iterator itr = mSubViews.begin();
            for (; itr != mSubViews.end(); ++itr)
            {
                if ((*itr)->isVisible() && (*itr) != (HLView*)mImageView)
                {
                    (*itr)->visit();
                }
            }
            return;
        }
//        if (mPercentage == 1)
//        {
//            if (mImageView)
//            {
//                mImageView->visit();
//            }
//            else
//            {
//                drawColor4F(mColor);
//                drawSolidRect(HLPointZero, HLPoint(mFrame.size.width, mFrame.size.height));
//            }
//            return;
//        }
        
        HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
        hlglEnable(GL_DEPTH_TEST);
        matrixStack->pushMatrix();
        drawColor4F(0,0,0,0);
        matrixStack->translatef(0, 0, 10);
        drawSolidPoly(&mVerts[0], (unsigned int)mVerts.size());
        matrixStack->popMatrix();
        if (mImageView)
        {
            mImageView->visit();
        }
        else
        {
            drawColor4F(mColor);
            drawSolidRect(HLPointZero, HLPoint(mFrame.size.width, mFrame.size.height));
        }
        hlglDisable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        std::list<HLView*>::const_iterator itr = mSubViews.begin();
        for (; itr != mSubViews.end(); ++itr)
        {
            if ((*itr)->isVisible() && (*itr) != (HLView*)mImageView)
            {
                (*itr)->visit();
            }
        }
    }
    
    void HLProgressRadialView::setProgress(float p)
    {
        if (p > 1)
            p = 1;
        if (p < 0)
            p = 0;
        if (mPercentage != p)
        {
            mPercentage = p;
            mVerts.clear();
            
            float maxLen = MAX(mFrame.size.width*1.5, mFrame.size.height*1.5);
            
            HLPoint mid(mFrame.size.width*mCenter.x, mFrame.size.height*(1-mCenter.y));
            
            mVerts.push_back(mid);
            
            float angle = mEndDegree - mStartDegree;
            
            if (angle <= 0)
            {
                angle += 360;
            }
            
            if (angle == 0)
            {
                angle = 360;
            }
            
            if (mType == kHLProgressRadialTypeCCW)
            {
                angle = 360 - angle;
                if (angle <= 0)
                {
                    angle += 360;
                }
            }
            angle *= mPercentage;
            angle = (360 - angle);
            int segments = roundf(angle/360*10);
            float step = angle / segments;
            for (int i = 0; i < segments + 1; ++i)
            {
                float angle1;
                if (mType == kHLProgressRadialTypeCW)
                {
                    angle1 = DEGREES_TO_RADIANS(mStartDegree - step*i);
                } else {
                    angle1 = DEGREES_TO_RADIANS(mStartDegree + step*i);
                }
                mVerts.push_back(HLPoint(mid.x + sinf(angle1) * maxLen, mid.y + cosf(angle1) * maxLen));
            }
            
            /*mVerts1.clear();
            if (p == 0)
            {
                return;
            }
            if (p == 1)
            {
                return;
            }
            float angle = (mEndDegree - mStartDegree)) * (1-mPercentage);
            HLPoint hit;
            HLPoint mid(mFrame.size.width*mCenter.x, mFrame.size.height*mCenter.y);
            float angle1 = atanf(mFrame.size.height/mFrame.size.width);
            if (angle <= M_PI)
            {
                mVerts.push_back(HLPoint(mid.x, mFrame.size.height));
                mVerts.push_back(mid);
            }
            else
            {
                if (mType == kHLProgressRadialTypeCW)
                {
                    mVerts1.push_back(HLPoint(0, 0));
                    mVerts1.push_back(HLPoint(mid.x, 0));
                    mVerts1.push_back(HLPoint(mid.x, mFrame.size.height));
                    mVerts1.push_back(HLPoint(0, mFrame.size.height));
                }
                else
                {
                    mVerts1.push_back(HLPoint(mid.x, 0));
                    mVerts1.push_back(HLPoint(mFrame.size.width, 0));
                    mVerts1.push_back(HLPoint(mFrame.size.width, mFrame.size.height));
                    mVerts1.push_back(HLPoint(mid.x, mFrame.size.height));
                }
            }
            if (angle <= angle1)
            {
                hit.y = mFrame.size.height;
                hit.x = mid.x+mid.y*tanf(angle);
                if (mType == kHLProgressRadialTypeCW)
                {
                    hit.x = mFrame.size.width - hit.x;
                    mVerts.push_back(hit);
                }
                else
                {
                    mVerts.push_back(hit);
                }
            }
            else if (angle <= M_PI_2)
            {
                hit.x = mFrame.size.width;
                hit.y = mid.y + mid.x*tanf(M_PI_2-angle);
                if (mType == kHLProgressRadialTypeCW)
                {
                    hit.x = mFrame.size.width - hit.x;
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(0, mFrame.size.height));
                }
                else
                {
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(mFrame.size.width, mFrame.size.height));
                }
            }
            else if (angle <= M_PI - angle1)
            {
                hit.x = mFrame.size.width;
                hit.y = mid.y - mid.x*tanf(angle-M_PI_2);
                if (mType == kHLProgressRadialTypeCW)
                {
                    hit.x = mFrame.size.width - hit.x;
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(0, mFrame.size.height));
                }
                else
                {
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(mFrame.size.width, mFrame.size.height));
                }
            }
            else if (angle <= M_PI)
            {
                hit.y = 0;
                hit.x = mid.x+mid.y*tanf(M_PI-angle);
                if (mType == kHLProgressRadialTypeCW)
                {
                    hit.x = mFrame.size.width - hit.x;
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(0, 0));
                    mVerts.push_back(HLPoint(0, mFrame.size.height));
                }
                else
                {
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(mFrame.size.width, 0));
                    mVerts.push_back(HLPoint(mFrame.size.width, mFrame.size.height));
                }
            }
            else if (angle <= M_PI + angle1)
            {
                hit.y = 0;
                hit.x = mid.x-mid.y*tanf(angle-M_PI);
                if (mType == kHLProgressRadialTypeCW)
                {
                    hit.x = mFrame.size.width - hit.x;
                }
                mVerts.push_back(hit);
                mVerts.push_back(mid);
                mVerts.push_back(HLPoint(mid.x, 0));
            }
            else if (angle <= M_PI + M_PI_2)
            {
                hit.x = 0;
                hit.y = mid.y - mid.x*tanf(M_PI + M_PI_2 - angle);
                if (mType == kHLProgressRadialTypeCW)
                {
                    hit.x = mFrame.size.width - hit.x;
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(mFrame.size.width, 0));
                    mVerts.push_back(HLPoint(mid.x, 0));
                    mVerts.push_back(mid);
                }
                else
                {
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(0, 0));
                    mVerts.push_back(HLPoint(mid.x, 0));
                    mVerts.push_back(mid);
                }
            }
            else if (angle <= M_PI + M_PI_2 + angle1)
            {
                hit.x = 0;
                hit.y = mid.y + mid.x*tanf(angle - M_PI - M_PI_2);
                if (mType == kHLProgressRadialTypeCW)
                {
                    hit.x = mFrame.size.width - hit.x;
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(mFrame.size.width, 0));
                    mVerts.push_back(HLPoint(mid.x, 0));
                    mVerts.push_back(mid);
                }
                else
                {
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(0, 0));
                    mVerts.push_back(HLPoint(mid.x, 0));
                    mVerts.push_back(mid);
                }
            }
            else
            {
                hit.y = mFrame.size.height;
                hit.x = mid.x-mid.y*tanf(2 * M_PI - angle);
                if (mType == kHLProgressRadialTypeCW)
                {
                    hit.x = mFrame.size.width - hit.x;
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(mFrame.size.width, mFrame.size.height));
                    mVerts.push_back(HLPoint(mFrame.size.width, 0));
                    mVerts.push_back(HLPoint(mid.x, 0));
                    mVerts.push_back(mid);
                }
                else
                {
                    mVerts.push_back(hit);
                    mVerts.push_back(HLPoint(0, mFrame.size.height));
                    mVerts.push_back(HLPoint(0, 0));
                    mVerts.push_back(HLPoint(mid.x, 0));
                    mVerts.push_back(mid);
                }
            }*/
        }
    }
    
    void HLProgressRadialView::setImage(HLTexture* tex, HLRect capInset)
    {
        if (tex)
        {
            if (!mImageView)
            {
                mImageView = new HLImageView(this, HLRect(HLPointZero, mFrame.size));
            }
            mImageView->setImage(tex, capInset);
        }
        else
        {
            if (mImageView)
            {
                mImageView->removeFromParent();
            }
        }
    }
    
    void HLProgressRadialView::setImage(HLSpriteFrame* spf, HLRect capInset)
    {
        if (spf && spf->_texture)
        {
            if (!mImageView)
            {
                mImageView = new HLImageView(this, HLRect(HLPointZero, mFrame.size));
            }
            mImageView->setImage(spf, capInset);
        }
        else
        {
            if (mImageView)
            {
                mImageView->removeFromParent();
            }
        }
    }
    
    void HLProgressRadialView::onInternalEvent(const char* event)
    {
        if (!strcmp(event, "size_changed") || !strcmp(event, "frame_changed"))
        {
            mImageView->setSize(mFrame.size);
        }
    }
    
#if USE_JS_SCRIPTING
    JSCppWrapperBase* HLProgressRadialView::getOrCreateJSWrapper(JSContext* cx)
    {
        return JSProgressRadialView::getOrCreateWrapper(cx, this);
    }
#endif
    
    void HLProgressRadialView::setColor(color4F color)
    {
        if (mImageView)
        {
            color4B c1;
            c1.r = color.r * 255;
            c1.g = color.g * 255;
            c1.b = color.b * 255;
            c1.a = color.a * 255;
            mImageView->setTintColor(c1);
        }
        mColor = color;
    }
    
    void HLProgressRadialView::setStartAngle(float degree)
    {
        while (degree > 360) {
            degree -= 360;
        }
        while (degree < 0) {
            degree += 360;
        }
        mStartDegree = degree;
        setProgress(mPercentage);
    }
    
    void HLProgressRadialView::setEndAngle(float degree)
    {
        while (degree > 360) {
            degree -= 360;
        }
        while (degree < 0) {
            degree += 360;
        }
        mEndDegree = degree;
        setProgress(mPercentage);
    }
    
    void HLProgressRadialView::setCenterPoint(HLPoint center)
    {
        mCenter = center;
    }
}

NS_HL_END



