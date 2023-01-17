//
//  HLView.h
//  HoolaiEngine
//
//  Created by tyq on 13-1-17.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLVIEW_H__
#define __HLVIEW_H__

#include "HLMacros.h"
#include "HLGeometry.h"
#include "HLUtil.h"
#include <list>
#include "kazmath/mat4.h"
#include "HLTouchDispatcher.h"
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
#include "HLMouseDispatcher.h"
#endif
#include "HLDelegate.h"
#include "HLViewDND.h"

NS_HL_BEGIN

class HLGUIViewComponent;

#if USE_JS_SCRIPTING

class JSCppWrapperBase;

namespace gui
{
    class HLView;

    class JSViewDragEventHandler
    {
        friend class HLView;
    private:
        JSObject* jsobj;
        jsval fval;
        JSObject* m_jsobject;
        
    public:
        JSViewDragEventHandler();
        ~JSViewDragEventHandler();
        void setCallback(JSObject* obj, jsval func);
        
        void onDragEvent(HLView*, const HLViewDraggingEvent&, HLViewDragSource*, bool* /*ret*/);
    };
}

#endif

namespace gui
{
//    class HLView;
    
    
    
    
    class TweenAnimations;
    
    typedef enum
    {
        HLAnimationCurveEaseInOut,
        HLAnimationCurveEaseIn,
        HLAnimationCurveEaseOut,
        HLAnimationCurveLinear
    }HLAnimationCurve;
    
    typedef struct
    {
        float top;
        float left;
        float bottom;
        float right;
    } HLEdgeInsets;
    
    class HLViewDragSource;
    
    typedef CDelegate2<bool, void*> HLViewAnimationHandler;
    typedef CDelegate2<HLView*, HLTouchEvent*>  HLViewTouchHandler;
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    typedef CDelegate2<HLView*, HLMouseEvent*> HLViewMouseHandler;
#endif
    typedef CDelegate4<HLView*, const HLViewDraggingEvent&, HLViewDragSource*, bool* /*ret*/> HLViewDragEventHandler;
    
    class HLView
    {

        
        friend class HLGUIManager;
        friend class ::hoolai::HLGUIViewComponent;
        friend class ::hoolai::HLTouchDispatcher;
        friend class HLViewDragSource;
        friend class HLScrollView;
        friend class HLImageView;
        friend class HLLabel;
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        friend class ::hoolai::HLMouseDispatcher;
#endif
    public:
        HLView(HLView* parent);
        HLView(HLView* parent, HLRect frame);
        HLView(HLView* parent, const HLView& copy);
        virtual ~HLView();
        HLView* getParentView() {return mParent;}
        void setSize(HLSize size);
        void setSize(float width, float height);
        void setSizePercent(HLSize size);
        void setSizePercent(float width, float height);
        void setPosition(HLPoint pos);
        void setPosition(float x, float y);
        void setCenter(HLPoint pos);
        void setCenter(float x, float y);
        void setPositionPercent(HLPoint pos);
        void setPositionPercent(float x, float y);
        void setFrame(HLRect frame);
        void setEdgeInsets(HLEdgeInsets insets);
        const HLRect& getFrame() const {return mFrame;}
        const HLSize& getSize() const;
        const HLSize getSizePercent() const;
        const HLPoint& getPosition() const;
        const HLPoint getCenter() const;
        const HLPoint getPositionPercent() const;
        const HLEdgeInsets& getEdgeInsets() const { return mEdgeInsets; }
        
        void setVisible(bool visible);
        bool isVisible() const;
        
        void setUserInteractionEnabled(bool enable);
        bool userInteractionEnalbed() const;
        
        void setMultiTouchEnabled(bool enable);
        bool multiTouchEnabled() const;
        
        void setClipToBounds(bool clip);
        bool clipToBounds() const;
        
        void setBackgroundColor(color4B color);
        const color4B& getBackgroundColor() const;
        
        void setTag(int32_t tag);
        int32_t getTag() const;
        
        HLView* findViewWithTag(int32_t tag);
        
        void centerInParent();
        void centerHorizontallyInParent();
        void centerVerticallyInParent();
        void leftInParent();
        void rightInParent();
        void topInParent();
        void bottomInParent();
        
        void setUserData(void* data) {mUserData = data;}
        void* getUserData() {return mUserData;}
        
        virtual HLView* hitTest(HLPoint p);
        virtual void onTouches(HLTouchEvent* touch)
        {
            if (mParent && mParent->userInteractionEnalbed())
            {
                mParent->handleTouchEvent(touch);
            }
        }
        HLViewTouchHandler touchHandler;
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        virtual void onMouseEvent(HLMouseEvent* mouseEvent)
        {
            if (mParent->userInteractionEnalbed())
            {
                mParent->handleMouseEvent(mouseEvent);
            }
        }
        HLViewMouseHandler mouseHandler;
#endif
        
        HLViewDragEventHandler draggingEventHandler;
        
        virtual bool onDraggingEvent(const HLViewDraggingEvent& event, HLViewDragSource* dragsource) { return false; }
        
#if USE_JS_SCRIPTING
        void setJSDraggingHandler(JSObject* obj, jsval func);
#endif
        
        virtual void visit();
        
        void removeFromParent(bool cleanup = true);
        void removeSubview(HLView * view, bool cleanup = true);
        void removeAllSubviews(bool cleanup = true);

        void bringSubviewToFront(HLView* view);
        void sendSubviewToBack(HLView* view);
        void moveSubviewBelowSubivew(HLView* view1, HLView* view2);
        const std::list<HLView*>& getSubviews() const {return mSubViews;}

        inline HLPoint convertToNodeSpace(HLPoint point)
        {
            kmMat4* transform = worldToNodeTransform();
            return HLPoint((float)((double)transform->mat[0]*point.x+(double)transform->mat[4]*point.y+transform->mat[12]),
                           (float)((double)transform->mat[1]*point.x+(double)transform->mat[5]*point.y+transform->mat[13]));
        }
        
        inline HLPoint convertToWorldSpace(HLPoint point)
        {
            kmMat4* transform = nodeToWorldTransform();
            return HLPoint((float)((double)transform->mat[0]*point.x+(double)transform->mat[4]*point.y+transform->mat[12]),
                           (float)((double)transform->mat[1]*point.x+(double)transform->mat[5]*point.y+transform->mat[13]));
        }
        
        kmMat4* nodeToWorldTransform()
        {
            kmMat4Assign(&mNodeToWorldTransform, getTransform());
            
            HLView* parent = mParent;
            if (parent != NULL)
            {
                kmMat4Multiply(&mNodeToWorldTransform, parent->nodeToWorldTransform(), &mNodeToWorldTransform);
                parent = parent->mParent;
            }
            return &mNodeToWorldTransform;
        }
        
        kmMat4* worldToNodeTransform()
        {
            kmMat4Inverse(&mWorldToNodeTransform, nodeToWorldTransform());
            return &mWorldToNodeTransform;
        }
        
        inline HLPoint touchLocationInView(HLTouch* touch)
        {
            HLPoint p = convertToNodeSpace(touch->location);
            p.y = mBounds.size.height - p.y;
            return p;
        }
        
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        inline HLPoint mouseLocationInView(HLMouseEvent* mouseEvent)
        {
            HLPoint p = convertToNodeSpace(mouseEvent->location);
            p.y = mBounds.size.height - p.y;
            return p;
        }
#endif
        
        void addSubview(HLView * view);
        
        static void startAnimations(std::string name, float duration, int repeat = 1, void* context = NULL);
        static void setAutoReverse(bool b);
        static void update(float dt);
        static void commitAnimations();
        static void setAnimationCurve(HLAnimationCurve curve);
        static void setAnimationCurveRate(float curveRate);
        static void stopAnimation(std::string name);
        static void stopAllAnimations();
        static HLViewAnimationHandler animationDidStopCallback;
        
        kmMat4* getTransform();
        
#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
        
        virtual HLView* clone(HLView* parent);
        
        void setNeedsUpdateTransform();
        
    protected:
        virtual void onDraw();
        virtual void onInternalEvent(const char* event) { }
        
        HLView() { mBackgroundColor.a = 0; mVisible = true; mClipToBounds = false; mUserInteractionEnabled = false; mTag = 0; }
        
        void updateTransform();
        
        virtual void viewWillDestroy(HLView* view) { }
        
    protected:
        HLRect mFrame;
        HLEdgeInsets mEdgeInsets;
        HLRect mBounds;
        HLView *mParent;
        std::list<HLView*> mSubViews;
        bool mVisible;
        bool mUserInteractionEnabled;
        bool mClipToBounds;
        color4B mBackgroundColor;
        int32_t mTag;
        
        kmMat4 mNodeToWorldTransform;
        kmMat4 mWorldToNodeTransform;
        
        bool mNeedsUpdateTransform;
        kmMat4 mTransform;
        
        void* mUserData;
        
        bool mMultiTouchEnabled;
    private:
        void handleTouchEvent(HLTouchEvent* event)
        {
            if (touchHandler.empty())
            {
                onTouches(event);
            }
            else
            {
                
                touchHandler(this, event);
            }
        }
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        void handleMouseEvent(HLMouseEvent* event)
        {
            if (mouseHandler.empty())
            {
                onMouseEvent(event);
            }
            else
            {
                mouseHandler(this, event);
            }
        }
#endif
        bool handleDraggingEvent(const HLViewDraggingEvent& event, HLViewDragSource* dragsource)
        {
            if (draggingEventHandler.empty())
            {
                return onDraggingEvent(event, dragsource);
            }
            else
            {
                bool ret = false;
                draggingEventHandler(this, event, dragsource, &ret);
                return ret;
            }
        }
        
#if USE_JS_SCRIPTING
        JSViewDragEventHandler jsviewdraghander;
#endif
        static std::list<TweenAnimations*> allAnimations;
        static TweenAnimations *currAnimations;
        template<typename T>
        static void propertyChanged(HLView* view, std::string property, T fromVal, T toVal);
    };
}

NS_HL_END

#endif
