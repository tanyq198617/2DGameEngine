//
//  HLWidget.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-1-17.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLWidget.h"
#include "HLDirector2D.h"
#include "HLGUIManager.h"
#if USE_JS_SCRIPTING
#include "JSWidget.h"
#endif
#include "HLSpriteFrame.h"


NS_HL_BEGIN

namespace gui
{
    HLWidget::HLWidget():mPresentWidget(NULL), mParentWidget(NULL)
    {
        mParent = NULL;
        mEdgeInsets.top = mEdgeInsets.left = mEdgeInsets.bottom = mEdgeInsets.right = 0;
        mFrame.origin = HLPointZero;
        mFrame.size = HLDirector2D::getSingleton()->getWinSize();
        mBounds = mFrame;
        mUserInteractionEnabled = true;
        updateTransform();
    }
    
    HLWidget::~HLWidget()
    {
        HLSpriteFrame::purgeCachedData();
#if USE_JS_SCRIPTING
        JSWidget::removeJSObject(this);
#endif
    }
    
    HLView* HLWidget::hitTest(HLPoint p)
    {
        if (!mVisible)
        {
            return nullptr;
        }
        if (mPresentWidget)
        {
            HLView* res = mPresentWidget->hitTest(p);
            if (res)
            {
                return res;
            }
        }
        return HLView::hitTest(p);
    }
    
    void HLWidget::visit()
    {
        if (!mVisible)
        {
            return;
        }
        if (mPresentWidget)
        {
            mPresentWidget->visit();
        }
        else
        {
            HLView::visit();
        }
    }
    
    void HLWidget::showWidget(HLWidget* widget)
    {
        if (mPresentWidget == widget)
        {
            return;
        }
        if (mPresentWidget)
        {
            mPresentWidget->close();
        }
        mPresentWidget = widget;
        widget->mParentWidget = this;
        widget->onShow(widget);
        
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        HLMouseDispatcher::sharedMouseDispatcher()->needsHitTest();
#else
        HLTouchDispatcher::sharedTouchDispatcher()->needsHitTest();
#endif
    }

    void HLWidget::close(bool clean)
    {
        if (mPresentWidget)
        {
            mPresentWidget->close(clean);
        }
        onClose(this, clean);
        if (mParentWidget)
            mParentWidget->mPresentWidget = NULL;
        HLTouchDispatcher::sharedTouchDispatcher()->needsHitTest();
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        HLMouseDispatcher::sharedMouseDispatcher()->needsHitTest();
#endif
        if (clean)
            delete this;
    }
    
#if USE_JS_SCRIPTING
    JSCppWrapperBase* HLWidget::getOrCreateJSWrapper(JSContext* cx)
    {
        return JSWidget::getOrCreateWrapper(cx, this);
    }
#endif
    
    HLView* HLWidget::clone(HLView* parent)
    {
        return NULL;
    }
    
    void HLRootWidget::visit()
    {
        if (!mVisible)
        {
            return;
        }
        HLView::visit();
        if (mPresentWidget)
        {
            mPresentWidget->visit();
        }
        if (mMaskWidget)
        {
            mMaskWidget->visit();
        }
        if (mHUDWidget)
        {
            mHUDWidget->visit();
        }
        if (mAlerts.size() > 0)
        {
            HLWidget* widget = mAlerts.back();
            widget->visit();
        }
       
        if (mToastWidget)
        {
            mToastWidget->visit();
        }
    }
    
    HLView* HLRootWidget::hitTest(HLPoint p)
    {
        if (mAlerts.size() > 0)
        {
            HLView* res = mAlerts.back()->hitTest(p);
            if (res)
            {
                return res;
            }
        }
        if (mHUDWidget)
        {
            return this;
        }
        if (mMaskWidget)
        {
            HLView* res = mMaskWidget->hitTest(p);
            if (res)
            {
                return res;
            }
        }
        if (mPresentWidget)
        {
            HLView* res = mPresentWidget->hitTest(p);
            if (res)
            {
                return res;
            }
        }
        return HLView::hitTest(p);
    }

    HLRootWidget::HLRootWidget()
    {
        mParent = NULL;
        mFrame.origin = HLPointZero;
        mFrame.size = HLDirector2D::getSingleton()->getWinSize();
        mBounds = mFrame;
        mUserInteractionEnabled = false;
        updateTransform();
        mToastWidget = NULL;
        mMaskWidget = NULL;
        mHUDWidget = NULL;
    }
    
    HLWidget* HLRootWidget::getTopWidget()
    {
        HLWidget* topWidget = NULL;
        HLWidget* widget = this;
        
        while (widget)
        {
            topWidget = widget;
            widget = widget->getPresentWidget();
        }
        
        return topWidget;
    }
    
    void HLRootWidget::showMaskWidget(HLWidget* mask)
    {
        if (mMaskWidget && mMaskWidget != mask)
        {
            dismissMaskWidget();
        }
        mMaskWidget = mask;
    }
    
    HLWidget* HLRootWidget::getMaskWidget()
    {
        if (!mMaskWidget)
        {
            mMaskWidget = new HLWidget();
            mMaskWidget->setUserInteractionEnabled(false);
        }
        return mMaskWidget;
    }
    
    void HLRootWidget::dismissMaskWidget()
    {
        if (mMaskWidget)
        {
            mMaskWidget->close();
            mMaskWidget = NULL;
        }
    }
    
    void HLRootWidget::showHUDWidget(HLWidget* hud)
    {
        if (mHUDWidget && mHUDWidget != hud)
        {
            dismissHUDWidget();
        }
        mHUDWidget = hud;
    }
    
    void HLRootWidget::dismissHUDWidget()
    {
        if (mHUDWidget)
        {
            mHUDWidget->close();
            mHUDWidget = NULL;
        }
    }
}

NS_HL_END
