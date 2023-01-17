//
//  HLWidget.h
//  HoolaiEngine
//
//  Created by tyq on 13-1-17.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLWIDGET_H__
#define __HLWIDGET_H__

#include "HLMacros.h"
#include "HLView.h"
#include "HLDelegate.h"
#include "HLToast.h"
#include "HLAlertView.h"

#include <algorithm>

NS_HL_BEGIN

namespace gui
{
    class HLWidget;
    typedef CMultiDelegate1<HLWidget*> HLWidgetEventHandler;
    typedef CMultiDelegate2<HLWidget*, bool /*clean*/> HLWidgetCloseEventHandler;
    
    class HLWidget: public HLView
    {
        friend class HLGUIManager;
    public:
        HLWidgetEventHandler onShow;
        HLWidgetCloseEventHandler onClose;
        int num;
        HLWidget();
        ~HLWidget();
        virtual HLView* hitTest(HLPoint p);
        void showWidget(HLWidget* widget);
        virtual void visit();
        void close(bool clean = true);
        
        HLWidget* getPresentWidget() {return mPresentWidget;}
        HLWidget* getParentWidget() {return mParentWidget;}
        
#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
        
        virtual HLView* clone(HLView *parent);
        
        virtual void onTouches(HLTouchEvent* touch) {}

    protected:
        HLWidget* mPresentWidget;
        
        HLWidget* mParentWidget;
    };

    class HLRootWidget: public HLWidget
    {
        friend class HLGUIManager;
        friend class HLAlertView;
        friend class HLToolTip;
        friend class HLToast;
    public:
        virtual void visit();
        virtual HLView* hitTest(HLPoint p);
        
        void dismissAllAlerts()
        {
            while (mAlerts.size() > 0)
            {
                HLWidget* alert = mAlerts.back();
                HLAlertView* alertview = static_cast<HLAlertView*>(alert->getSubviews().front());
                alertview->onDismiss(alertview);
                alert->close();
                mAlerts.pop_back();
            }
        }
        
        void removeAllToasts()
        {
            delete mToastWidget;
            mToastWidget = NULL;
            if (HLToast::mShowTimer) {
                HLToast::mShowTimer->cancel();
                HLToast::mShowTimer = nullptr;
            }
            HLToast::mShowPendingList.clear();
        }
        
        bool hasAlerts()
        {
            return !mAlerts.empty();
        }
        
        HLWidget* getTopWidget();
        void showMaskWidget(HLWidget* mask);
        HLWidget* getMaskWidget();
        void dismissMaskWidget();
        void showHUDWidget(HLWidget* hud);
        void dismissHUDWidget();
        
    private:
        void showAlert(HLWidget* alert)
        {
            mAlerts.push_back(alert);
            HLTouchDispatcher::sharedTouchDispatcher()->needsHitTest();
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
            HLMouseDispatcher::sharedMouseDispatcher()->needsHitTest();
#endif
        }
        void dismissAlert(HLWidget* alert)
        {
            std::list<HLWidget*>::iterator itr = std::find(mAlerts.begin(), mAlerts.end(), alert);
            if (itr != mAlerts.end())
            {
                mAlerts.erase(std::find(mAlerts.begin(), mAlerts.end(), alert));
                HLAlertView* alertview = static_cast<HLAlertView*>(alert->getSubviews().front());
                alertview->onDismiss(alertview);
                alert->close();
            }
        }
        void addToast(HLToast* toast)
        {
            if (!mToastWidget)
            {
                mToastWidget = new HLWidget();
                mToastWidget->setUserInteractionEnabled(false);
            }
            mToastWidget->addSubview(toast);
        }
        void removeToast(HLToast* toast)
        {
            toast->removeFromParent();
            if (mToastWidget->getSubviews().size() == 0)
            {
                delete mToastWidget;
                mToastWidget = NULL;
            }
        }
        HLRootWidget();
    private:
        std::list<HLWidget*> mAlerts;
        HLWidget* mToastWidget;
        HLWidget* mMaskWidget;  // add a mask over other UIs, eg. beginners guide UI. it will show below the HUDWidget
        HLWidget* mHUDWidget; // it will show on the top (below alerts) of views to show loading indicator or something else
    };
    
}

NS_HL_END

#endif
