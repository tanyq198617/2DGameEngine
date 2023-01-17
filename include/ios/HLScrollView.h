//
//  HLScrollView.h
//  HoolaiEngine
//
//  Created by zhao shuan on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLSCROLLVIEW_H__
#define __HLSCROLLVIEW_H__

#include "HLMacros.h"
#include "HLView.h"
#include "HLDelegate.h"
#include "HLTouchDispatcher.h"
#include "HLTimer.h"
#include "GameEngineConfig.h"
#ifdef TARGET_WIN32
#include "hltime.h"
#endif

NS_HL_BEGIN

namespace gui
{
    class HLScrollView;
    
    typedef enum {
        kHLScrollViewDirectionHorizontal = 1,
        kHLScrollViewDirectionVertical = 2,
        kHLScrollViewDirectionBoth = 3
    } HLScrollViewDirection;
    
    class HLScrollView;
    class HLTableView;
    typedef CDelegate1<HLScrollView*> HLScrollViewEventHandler;
    
    class HLScrollView : public HLView
    {
        friend class HLTableView;
        friend class HLScrollViewContentView;
    public:
        HLScrollView(HLView * parent);
        HLScrollView(HLView * parent, HLRect frame);
        HLScrollView(HLView * parent, const HLScrollView& copy);
        virtual ~HLScrollView();
        
        HLScrollViewEventHandler scrollViewDidDrag;
        HLScrollViewEventHandler scrollViewDidScroll;
        HLScrollViewEventHandler scrollViewDidEndScrollingAnimation;
        HLScrollViewEventHandler scrollViewDidZoom;
        
        void setContentOffset(HLPoint offset, bool animated = false);
        HLPoint getContentOffset();
        void setContentOffsetInDuration(HLPoint offset, float dt);
        
        void setTouchEnable(bool enable){
            mTouchEnable = enable;
        }
        
        bool getTouchEnable() {
            return mTouchEnable;
        }
        
        void setZoomScale(float s);
        
        void setZoomScale(float s, bool animated);
        
        float getZoomScale();
        
        void setZoomScaleInDuration(float s, float dt);
        
        HLPoint minContainerOffset();
        
        HLPoint maxContainerOffset();
        
        bool isViewVisible(HLView *view);
        
        const HLView* getContentView() const {return mContainer;}
        
        bool isDragging(){return mDragging;}
        bool isTouchMoved(){return mTouchMoved;}
        bool isBounceable(){return mBounceable;}
        void setBounceable(bool b){mBounceable = b;}
        
        HLScrollViewDirection getDirection(){return mDirection;}
        void setDirection(HLScrollViewDirection direction){mDirection = direction;}
        
        void setContentSize(HLSize size);
        HLSize getContentSize() const;
        
        void setPagingEnabled(bool enable) {mPaging = enable;}
        bool isPagingEnabled() {return mPaging;}
        
        virtual void onTouches(HLTouchEvent* touch);
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        virtual void onMouseEvent(HLMouseEvent* mouseEvent);
#endif
        
        virtual HLView* hitTest(HLPoint p);
        
        virtual void visit();
        
        virtual bool onDraggingEvent(const HLViewDraggingEvent& event, HLViewDragSource* dragsource);
        
        virtual HLView* clone(HLView* parent);
        
#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
        
    protected:
        virtual void onInternalEvent(const char* event);
        virtual void viewWillDestroy(HLView* view)
        {
            if (view == mViewOnTouch)
            {
                mViewOnTouch = NULL;
            }
        }
    private:
        void initMembers();
        void relocateContainer(bool animated);
        void deaccelerateScrolling(HLTimer* timer, float dt);
        
        void performedAnimatedScroll(HLTimer* timer, float dt);
        void stoppedAnimatedScroll(bool finished, void* context);
        
        HLTimer* mAnimationTimer;
        float mZoomScale;
        HLScrollViewDirection mDirection;
        HLView* mContainer;
        bool mDragging;
        HLPoint mContentOffset;
        bool mTouchMoved;
        HLPoint mMaxInset;
        HLPoint mMinInset;
        bool mBounceable;
        bool mDeaccelerating;
        HLPoint mScrollDistance;
        HLPoint mScrollSpeed;
        HLPoint mTouchPoint;
        HLPoint mLastTouchPoint;
        struct timeval mTimestamp;
        struct timeval mScollStartTime;
        HLPoint mOriginalPoint;
        HLPoint mScrollStartPoint;
        float mTouchLength;
        float mMinScale;
        float mMaxScale;
        bool mTouchEnable;
        bool mPaging;
        
        HLView *mViewOnTouch;
    };
}

NS_HL_END

#endif
