//
//  HLToast.cpp
//  DivineComedy
//
//  Created by liumingliang on 13-3-26.
//  Copyright (c) 2013年 zhao shuan. All rights reserved.
//

#include "HLToast.h"
#include "HLGUIManager.h"
#include "HLTimer.h"
#include "kazmath/GL/matrix.h"
#include "HLWidget.h"
#include "HLDirector2D.h"

#if USE_JS_SCRIPTING
#include "JSToast.h"
#endif

NS_HL_BEGIN

namespace gui
{
    
    HLToast::HLToast(std::string _string):HLView(NULL, HLRect(0, 0, 400, 200))
    {
        initMember();
        setString(_string);
    }
    
    HLToast::~HLToast()
    {
        if (timer)
        {
            timer->cancel();
            timer = nullptr;
        }
#if USE_JS_SCRIPTING
        JSToast::removeJSObject(this);
#endif
    }
    
    void HLToast::initMember()
    {
        label = NULL;
        mTimeElapsed = 0;

    }
    void HLToast::setString(std::string _string)
    {
        setUserInteractionEnabled(false);
        
        if (label)
        {
            label->removeFromParent();
        }
        
        label = new HLRichLabel(this, HLRect(0, 0, 400, 100));
        
        label->setText(_string);
        label->setFontSize(20);

        label->setTextStroke(true);
        
        color4B c1 = {0, 0, 0, 255};
        
        label->setTextStrokeColor(c1);
        
        c1.r =255;
        c1.g =204;
        c1.b =51;
        c1.a = 255;
        
        label->setTextColor(c1);
    }

    HLRichLabel* HLToast::getLabel()
    {
        return label;
    }

    HLTimer *HLToast::mShowTimer = NULL;
    std::list<HLToast *> HLToast::mShowPendingList;
    
    void HLToast::show()
    {
        mShowPendingList.push_back(this);        
        if (!mShowTimer)
        {
            mShowTimer = new HLTimer(0,true);
            mShowTimer->delegate = newDelegate(&HLToast::update);
        }
    }
    
    void HLToast::update(HLTimer *timer111 , float dt) {
        static float time = 0;
        if (time > .2)
        {
            time = 0;
        }
        if (time == 0)
        {
            mShowPendingList.front()->showImpl();
            mShowPendingList.pop_front();
            if (mShowPendingList.begin() == mShowPendingList.end())
            {
                mShowTimer->cancel();
                mShowTimer = NULL;
            }
        }
        time += dt;
    }
    
    void HLToast::showImpl()
    {
        HLGUIManager::getSingleton()->getRootWidget()->addToast(this);
        centerInParent();
        timer = new HLTimer(0, true);
        timer->delegate = newDelegate(this, &HLToast::moveAndFade);
    }

    void HLToast::close()
    {
        timer->cancel();
        HLGUIManager::getSingleton()->getRootWidget()->removeToast(this);

    //    delete this;
    }


    void HLToast::visit()
    {
    //    if (!mVisible)
    //    {
    //        return;
    //    }
    //    if (mScale != 1)
    //    {
    //        kmGLPushMatrix();
    //        kmGLMultMatrix(getTransform());
    //        kmMat4 matrix;
    //        kmMat4Scaling(&matrix, mScale, mScale, 1);
    //        matrix.mat[12] = -mBounds.size.width * (mScale - 1)/2;
    //        matrix.mat[13] = -mBounds.size.height * (mScale - 1)/2;
    //        kmGLMultMatrix(&matrix);
    //        kmMat4Inverse(&matrix,getTransform());
    //        kmGLMultMatrix(&matrix);
    //    }
        HLView::visit();
    //    if (mScale != 1)
    //    {
    //        kmGLPopMatrix();
    //    }
    }

    void HLToast::moveAndFade(HLTimer *timer , float dt)
    {
        mTimeElapsed += dt;
        float percent = (mTimeElapsed / 0.12 * 1.1)*1;
        if (percent >= 15)
        {
            close();
            return;
        }
        else if(percent >= 6)
        {
//            //引擎暂时不支持半透明渐变化
//            color4B c1;
//            c1.r = 77; c1.g = 200; c1.b = 200; c1.a = 255*(15.f-percent)/(15.f-6.f);
//            label->setTextStrokeColor(c1);
//            
//            c1.r = c1.g = c1.b = 255;
            label->setAlpha((15.f-percent)/(15.f-6.f));
        }
        if (percent > 2) {
            label->setPosition(label->getPosition().x,label->getPosition().y-1);
        }
    }
    
#if USE_JS_SCRIPTING
    JSCppWrapperBase* HLToast::getOrCreateJSWrapper(JSContext* cx)
    {
        return JSToast::getOrCreateWrapper(cx, this);
    }
#endif
    
    HLView* HLToast::clone(HLView *parent)
    {
        return NULL;
    }
}

NS_HL_END

