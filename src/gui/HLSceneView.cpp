//
//  HLSceneView.cpp
//  HoolaiEngine
//
//  Created by tyq on 3/3/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLSceneView.h"
#include "kazmath/GL/matrix.h"
#include "HLTimer.h"
#include "HLSceneManager.h"
#include "HLEntitySystem.h"
#include "JSSceneView.h"

NS_HL_BEGIN

namespace gui
{
    HLSceneView::~HLSceneView()
    {
#if USE_JS_SCRIPTING
        JSSceneView::removeJSObject(this);
#endif
        if (mTimer)
        {
            mTimer->cancel();
        }
        if (mSceneManager)
        {
            mSceneManager->mSceneView = NULL;
            mSceneManager->onDestroy(mSceneManager);
            delete mSceneManager;
        }
    }
    
#if USE_JS_SCRIPTING
    JSCppWrapperBase* HLSceneView::getOrCreateJSWrapper(JSContext* cx)
    {
        return JSSceneView::getOrCreateWrapper(cx, this);
    }
#endif
    
    void HLSceneView::entityWillDestroy(HLEntity* entity)
    {
        if (mEntityOnTouch == entity)
        {
            mEntityOnTouch = NULL;
        }
    }
    
    void HLSceneView::setSceneManager(HLSceneManager *entMgr)
    {
        if (mSceneManager)
        {
            mSceneManager->mSceneView = NULL;
            mEntityOnTouch = NULL;
            mSceneManager->onDestroy(mSceneManager);
            delete mSceneManager;
        }
        mSceneManager = entMgr;
        if (!mSceneManager)
        {
            return;
        }
        mSceneManager->mSceneView = this;
        if (!mTimer)
        {
            mTimer = new HLTimer(0, true);
            mTimer->delegate = newDelegate(this, &HLSceneView::update);
        }
    }
    
    void HLSceneView::onDraw()
    {
        if (mSceneManager)
        {
            mSceneManager->draw();
        }
        HLView::onDraw();
    }
    
    void HLSceneView::update(HLTimer* timer, float dt)
    {
        if (mSceneManager)
        {
            mSceneManager->update(dt);
        }
        else
        {
            timer->cancel();
            mTimer = NULL;
        }
    }
    
    void HLSceneView::onTouches(HLTouchEvent* touch)
    {
        if (mSceneManager)
        {
            if (touch->type == kHLTouchesBegin)
            {
                HLEntity* e = mSceneManager->hitTest(touch->touches[0].location);
                mEntityOnTouch = e;
            }
            
            if (mEntityOnTouch)
            {
                mEntityOnTouch->onTouchEvent(touch);
            }
        }
    }
}

NS_HL_END
