//
//  HLToolTip.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/21/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLToolTip.h"
#include "HLDirector2D.h"
#include "HLGUIManager.h"
#include "HLTimer.h"
#include "JSToolTip.h"

NS_HL_BEGIN

namespace gui
{
    HLToolTip* tipInstance = NULL;
    
    class HLToolTipParentView: public HLView
    {
    private:
        HLTimer* timer;
    public:
//        HLRect eventRect;
        HLToolTip* toolTip;
        
        virtual HLView* hitTest(HLPoint point)
        {
            if (mAcceptHitTest)
            {
                std::list<HLView*>::reverse_iterator itr = mSubViews.rbegin();
                for (; itr != mSubViews.rend(); ++itr)
                {
                    HLView* res = (*itr)->hitTest(point);
                    if (res)
                    {
                        return res;
                    }
                }
            }
            timer = new HLTimer(0);
            timer->delegate = newDelegate(this, &HLToolTipParentView::dismiss);
            return NULL;
        }
        
        void dismiss(HLTimer*,float)
        {
            this->removeFromParent();
        }
        
        HLToolTipParentView(HLView* parent, HLRect frame): HLView(parent, frame), timer(NULL), mViewOnTouch(NULL), mAcceptHitTest(true) { }
        
        virtual ~HLToolTipParentView()
        {
            toolTip->onDismiss(toolTip);
            if (timer)
            {
                timer->cancel();
            }
        }
        
        virtual void onTouches(HLTouchEvent* touch)
        {
//            if (touch->type == kHLTouchesEnd)
//            {
//                toolTip->onDismiss(toolTip);
//            }
//            HLPoint point = touch->touches[0].location;
//            if (touch->type == kHLTouchesBegin && eventRect.containsPoint(point))
//            {
//                mAcceptHitTest = false;
//                mViewOnTouch = mParent->hitTest(point);
//                mAcceptHitTest = true;
//            }
//            if (mViewOnTouch)
//            {
//                if (mViewOnTouch->touchHandler.empty())
//                {
//                    mViewOnTouch->onTouches(touch);
//                }
//                else{
//                    mViewOnTouch->touchHandler(mViewOnTouch, touch);
//                }
//            }
//            if (touch->type == kHLTouchesEnd)
//            {
//                this->removeFromParent();
//            }
        }
        
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        virtual void onMouseEvent(HLMouseEvent* mouseEvent)
        {
//            HLPoint point = mouseEvent->location;
//            if (mouseEvent->type == kHLMouseDown && eventRect.containsPoint(point))
//            {
//                mAcceptHitTest = false;
//                mViewOnTouch = mParent->hitTest(point);
//                mAcceptHitTest = true;
//            }
//            if (mViewOnTouch)
//            {
//                mViewOnTouch->onMouseEvent(mouseEvent);
//            }
//            if (mouseEvent->type == kHLMouseUp)
//            {
//                toolTip->onDismiss(toolTip);
//                this->removeFromParent();
//            }
        }
#endif
        
    private:
        HLView* mViewOnTouch;
        bool mAcceptHitTest;
    };
    
    HLToolTip::HLToolTip(HLRect frame):HLView(NULL, frame), mShown(false)
    {
        
    }
    
    HLToolTip::~HLToolTip()
    {
#if USE_JS_SCRIPTING
        JSToolTip::removeJSObject(this);
#endif
        if (tipInstance == this)
        {
            tipInstance = NULL;
        }
    }
    
    void HLToolTip::show(hoolai::gui::HLView* pParent)
    {
        if (mShown)
        {
            return;
        }
        mShown = true;
        
        if (tipInstance)
        {
            tipInstance->dismiss();
        }
        tipInstance = this;
        
        setUserInteractionEnabled(true);
        
        HLSize winSize = HLDirector2D::getSingleton()->getWinSize();
        HLToolTipParentView* parent = new HLToolTipParentView(NULL, HLRect(0, 0, winSize.width, winSize.height));
//        HLPoint position = convertToWorldSpace(HLPoint(0, mFrame.size.height));
//        parent->eventRect.origin = position;
//        parent->eventRect.size = mFrame.size;
        parent->toolTip = this;
        parent->setUserInteractionEnabled(true);
        parent->addSubview(this);
        
        if (pParent)
        {
            pParent->addSubview(parent);
        }
        else
        {
            if (HLGUIManager::getSingleton()->getRootWidget()->mAlerts.size() > 0)
            {
                HLGUIManager::getSingleton()->getRootWidget()->mAlerts.back()->addSubview(parent);
            }
            else if (HLGUIManager::getSingleton()->getRootWidget()->mMaskWidget)
            {
                HLWidget *widget = HLGUIManager::getSingleton()->getRootWidget()->mMaskWidget;
                while (widget->getPresentWidget())
                {
                    widget = widget->getPresentWidget();
                }
                widget->addSubview(parent);
            }
            else
            {
                HLGUIManager::getSingleton()->getRootWidget()->getTopWidget()->addSubview(parent);
            }
        }
        
        HLTouchDispatcher::sharedTouchDispatcher()->needsHitTest();
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        HLMouseDispatcher::sharedMouseDispatcher()->needsHitTest();
#endif
    }
    
    void HLToolTip::dismissCurrentToolTip()
    {
        if (tipInstance)
        {
            tipInstance->dismiss();
        }
    }
    
    void HLToolTip::dismiss()
    {
        this->getParentView()->removeFromParent();
//        HLTouchDispatcher::sharedTouchDispatcher()->needsHitTest();
//#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
//        HLMouseDispatcher::sharedMouseDispatcher()->needsHitTest();
//#endif
    }
    
#if USE_JS_SCRIPTING
    JSCppWrapperBase* HLToolTip::getOrCreateJSWrapper(JSContext* cx)
    {
        return JSToolTip::getOrCreateWrapper(cx, this);
    }
#endif
}

NS_HL_END
