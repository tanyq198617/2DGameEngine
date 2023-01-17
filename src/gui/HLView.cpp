//
//  HLView.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-1-17.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLView.h"
#include "HLDirector2D.h"
#include "HLDrawingPrimitives.h"
#include "HLGL.h"
#include "HLGLMatrixStack.h"
#include <algorithm>
#include "GameEngineConfig.h"
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
#include "HLMouseDispatcher.h"
#else
#include "HLTouchDispatcher.h"
#endif
#include "HLImageView.h"
#include "HLLabel.h"

#if USE_JS_SCRIPTING
#include "JSView.h"
#endif

using namespace std;

NS_HL_BEGIN

namespace gui
{
    
#if USE_JS_SCRIPTING
    JSViewDragEventHandler::JSViewDragEventHandler():m_jsobject(NULL), jsobj(NULL)
    {
    }
    
    void JSViewDragEventHandler::onDragEvent(HLView* view, const HLViewDraggingEvent& event, HLViewDragSource* source, bool* ret)
    {
        if (jsobj)
        {
            JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
            JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
            jsval arg[3];
            arg[0] = value_to_jsval(view);
            arg[1] = value_to_jsval(event);
            arg[2] = value_to_jsval(source);
            jsval jsret;
            JSBool ok = JS_CallFunctionValue(cx, jsobj, fval, 3, arg, &jsret);
            if (ok == JS_FALSE)
            {
                if (!JS_ReportPendingException(cx))
                {
                    JS_ClearPendingException(cx);
                }
            }
            else
            {
                if (JSVAL_IS_BOOLEAN(jsret))
                {
                    *ret = JSVAL_TO_BOOLEAN(jsret);
                }
            }
        }
    }
    
    void JSViewDragEventHandler::setCallback(JSObject* obj, jsval func)
    {
        JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
        if (!m_jsobject)
        {
            JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
            m_jsobject = JS_NewObject(cx, NULL, NULL, NULL);
            JS_AddObjectRoot(cx, &m_jsobject);
        }
        jsobj = obj;
        fval = func;
        JS::RootedValue val(cx, OBJECT_TO_JSVAL(jsobj));
        JS_SetProperty(cx, m_jsobject, "target", val);
        JS::RootedValue val1(cx, fval);
        JS_SetProperty(cx, m_jsobject, "function", val1);
    }
    
    JSViewDragEventHandler::~JSViewDragEventHandler()
    {
        JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
        if (m_jsobject)
        {
            JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
            JS_RemoveObjectRoot(cx, &m_jsobject);
            JS_MaybeGC(cx);
        }
    }
#endif
    
    class TweenAnimation
    {
    public:
        HLView* _target;
        TweenAnimation(HLView* target):_target(target) {}
        virtual ~TweenAnimation() {}
        virtual void update(float progress) = 0;
    protected:
        template<typename T>
        void setProperty(HLView* target, std::string property, T value) {}
    };
    
    template<>
    void TweenAnimation::setProperty<HLPoint>(HLView* target, std::string property, HLPoint value)
    {
        if (!strcmp(property.c_str(), "position"))
        {
            target->setPosition(value);
        }
    }
    
    template<>
    void TweenAnimation::setProperty<HLSize>(HLView* target, std::string property, HLSize value)
    {
        if (!strcmp(property.c_str(), "size"))
        {
            target->setSize(value);
        }
    }
    
    template<>
    void TweenAnimation::setProperty<HLRect>(HLView* target, std::string property, HLRect value)
    {
        if (!strcmp(property.c_str(), "frame"))
        {
            target->setFrame(value);
        }
    }
    
    template<>
    void TweenAnimation::setProperty<float>(HLView* target, std::string property, float value)
    {
        if (!strcmp(property.c_str(), "imgrotation"))
        {
            ((HLImageView*)target)->setRotation(value);
        }
        else if (!strcmp(property.c_str(), "glowRadius"))
        {
            ((HLImageView*)target)->setGlowRadius(value);
        }
    }
    
    template<>
    void TweenAnimation::setProperty<color4B>(HLView* target, std::string property, color4B value)
    {
        if (!strcmp(property.c_str(), "textColor"))
        {
            ((HLLabel*)target)->setTextColor(value);
        }
        else if (!strcmp(property.c_str(), "textColorTL"))
        {
            ((HLLabel*)target)->setTextColorTL(value);
        }
        else if (!strcmp(property.c_str(), "textColorTR"))
        {
            ((HLLabel*)target)->setTextColorTR(value);
        }
        else if (!strcmp(property.c_str(), "textColorBL"))
        {
            ((HLLabel*)target)->setTextColorBL(value);
        }
        else if (!strcmp(property.c_str(), "textColorBR"))
        {
            ((HLLabel*)target)->setTextColorBR(value);
        }
        else if (!strcmp(property.c_str(), "textStrokeColor"))
        {
            ((HLLabel*)target)->setTextStrokeColor(value);
        }
        else if (!strcmp(property.c_str(), "tintColor"))
        {
            ((HLImageView*)target)->setTintColor(value);
        }
        else if (!strcmp(property.c_str(), "glowColor"))
        {
            ((HLImageView*)target)->setGlowColor(value);
        }
    }
    
    class TweenAnimations
    {
    private:
        int count;
        float accDt;
        int reverseCount;
    public:
        std::string name;
        float duration;
        std::vector<TweenAnimation*> animations;
        void *context;
        int repeat;
        bool stopped;
        HLAnimationCurve curve;
        float curveRate;
        bool autoreverse;
        HLViewAnimationHandler animationDelegate;
        
        TweenAnimations(std::string name, float duration, int repeat, void* context):name(name), duration(duration), repeat(repeat), context(context)
        {count = 0; stopped = false; accDt = 0; curve = HLAnimationCurveLinear; curveRate = 2; reverseCount = 0; autoreverse = false;}
        
        ~TweenAnimations()
        {
            std::vector<TweenAnimation*>::iterator itr;
            for (itr = animations.begin(); itr != animations.end(); itr++)
            {
                delete (*itr);
            }
        }
        
        inline
        bool update(float dt);
        
    };
    
    template<typename T>
    class HLTweenAnimation:public TweenAnimation
    {
    private:
        std::string _property;
        T _fromVal;
        T _toVal;
    public:
        HLTweenAnimation(HLView* target, std::string property, T fromVal, T toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
        virtual void update(float progress)
        {
            
        }
    };
    
    template <>
    class HLTweenAnimation<HLPoint>:public TweenAnimation
    {
    private:
        std::string _property;
        HLPoint _fromVal;
        HLPoint _toVal;
    public:
        HLTweenAnimation(HLView* target, std::string property, HLPoint fromVal, HLPoint toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
        virtual void update(float progress)
        {
            HLPoint val = _fromVal + (_toVal - _fromVal) * progress;
            setProperty<HLPoint>(_target, _property, val);
        }
    };
    
    template <>
    class HLTweenAnimation<HLSize>:public TweenAnimation
    {
    private:
        std::string _property;
        HLSize _fromVal;
        HLSize _toVal;
    public:
        HLTweenAnimation(HLView* target, std::string property, HLSize fromVal, HLSize toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
        virtual void update(float progress)
        {
            HLSize val(_fromVal.width + (_toVal.width-_fromVal.width)*progress, _fromVal.height + (_toVal.height-_fromVal.height)*progress);
            setProperty<HLSize>(_target, _property, val);
        }
    };
    
    template <>
    class HLTweenAnimation<HLRect>:public TweenAnimation
    {
    private:
        std::string _property;
        HLRect _fromVal;
        HLRect _toVal;
    public:
        HLTweenAnimation(HLView* target, std::string property, HLRect fromVal, HLRect toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
        virtual void update(float progress)
        {
            HLPoint p = _fromVal.origin + (_toVal.origin - _fromVal.origin) * progress;
            HLSize s(_fromVal.size.width + (_toVal.size.width-_fromVal.size.width)*progress, _fromVal.size.height + (_toVal.size.height-_fromVal.size.height)*progress);
            setProperty<HLRect>(_target, _property, HLRect(p, s));
        }
    };
    
    template <>
    class HLTweenAnimation<color4B>:public TweenAnimation
    {
    private:
        std::string _property;
        color4B _fromVal;
        color4B _toVal;
    public:
        HLTweenAnimation(HLView* target, std::string property, color4B fromVal, color4B toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
        virtual void update(float progress)
        {
            color4B color;
            color.r = _fromVal.r + (_toVal.r - _fromVal.r)*progress;
            color.g = _fromVal.g + (_toVal.g - _fromVal.g)*progress;
            color.b = _fromVal.b + (_toVal.b - _fromVal.b)*progress;
            color.a = _fromVal.a + (_toVal.a - _fromVal.a)*progress;
            setProperty<color4B>(_target, _property, color);
        }
    };
    
    //    template <>
    //    class HLTweenAnimation<int>:public TweenAnimation
    //    {
    //    private:
    //        HLEntity* _target;
    //        std::string _property;
    //        int _fromVal;
    //        int _toVal;
    //    public:
    //        HLTweenAnimation(HLEntity* target, std::string property, int fromVal, int toVal):_target(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
    //        virtual void update(float progress)
    //        {
    //            int val = _fromVal + (_toVal - _fromVal) * progress;
    //            _target->setProperty<int>(_property, val);
    //        }
    //    };
    
    template <>
    class HLTweenAnimation<float>:public TweenAnimation
    {
    private:
        std::string _property;
        float _fromVal;
        float _toVal;
    public:
        HLTweenAnimation(HLView* target, std::string property, float fromVal, float toVal):TweenAnimation(target),  _property(property), _fromVal(fromVal), _toVal(toVal){}
        virtual void update(float progress)
        {
            float val = _fromVal + (_toVal - _fromVal) * progress;
            setProperty<float>(_target, _property, val);
        }
    };
    
    template<typename T>
    void HLView::propertyChanged(HLView* view, std::string property, T fromVal, T toVal)
    {
        if (!strcmp(property.c_str(), "position") ||
            !strcmp(property.c_str(), "size") ||
            !strcmp(property.c_str(), "frame") ||
            !strcmp(property.c_str(), "imgrotation") ||
            !strcmp(property.c_str(), "textColor") ||
            !strcmp(property.c_str(), "textColorTL") ||
            !strcmp(property.c_str(), "textColorTR") ||
            !strcmp(property.c_str(), "textColorBL") ||
            !strcmp(property.c_str(), "textColorBR") ||
            !strcmp(property.c_str(), "textStrokeColor") ||
            !strcmp(property.c_str(), "tintColor") ||
            !strcmp(property.c_str(), "glowColor") ||
            !strcmp(property.c_str(), "glowRadius"))
        {
            HLTweenAnimation<T> * anim = new HLTweenAnimation<T>(view, property, fromVal, toVal);
            currAnimations->animations.push_back(anim);
        }
    }
    
    HLViewAnimationHandler HLView::animationDidStopCallback;
    std::list<TweenAnimations*> HLView::allAnimations;
    TweenAnimations *HLView::currAnimations;
    
    bool TweenAnimations::update(float dt)
    {
        accDt += dt;
        
        bool reverse = false;
        if (autoreverse && reverseCount)
        {
            reverse = true;
        }
        
        if (accDt >= duration)
        {
            if (autoreverse)
            {
                reverseCount++;
                reverseCount %= 2;
            }
            std::vector<TweenAnimation*>::iterator itr;
            std::vector<TweenAnimation*> anims = animations;
            for (itr = anims.begin(); itr != anims.end(); itr++)
            {
                (*itr)->update(reverse?0:1);
            }
            if (repeat > 0)
            {
                count++;
                if (count == repeat)
                {
                    stopped = true;
                    animationDelegate(true, context);
                    return true;
                }
            }
            accDt = 0;
//            animationDelegate(false, context);
        }
        else
        {
            float percent = accDt/duration;
            std::vector<TweenAnimation*>::iterator itr;
            switch (curve) {
                case HLAnimationCurveEaseIn:
                    percent = powf(percent, curveRate);
                    break;
                case HLAnimationCurveEaseOut:
                    percent = 1 - powf(1 - percent, curveRate);
                    break;
                case HLAnimationCurveEaseInOut:
                    percent *= 2;
                    percent = percent<1? 0.5f * powf(percent, curveRate) : 1.0f - 0.5f * powf(2 - percent, curveRate);
                    break;
                default:
                    break;
            }
            for (itr = animations.begin(); itr != animations.end(); itr++)
            {
                (*itr)->update(reverse?1-percent:percent);
            }
        }
        return false;
    }

    HLView::HLView(HLView* parent)
    {
        mFrame = HLRectZero;
        if (parent)
        {
            mFrame.size = parent->getSize();
        }
        mEdgeInsets.top = mEdgeInsets.left = mEdgeInsets.bottom = mEdgeInsets.right = 0;
        mBounds = mFrame;
        mBackgroundColor.r=mBackgroundColor.g=mBackgroundColor.b=mBackgroundColor.a = 0;
        mClipToBounds = false;
        mVisible = true;
        mUserInteractionEnabled = false;
        mTag = 0;
        mParent = NULL;
        if (parent)
        {
            parent->addSubview(this);
        }
        setNeedsUpdateTransform();
        mUserData = NULL;
        mUserInteractionEnabled = false;
        mMultiTouchEnabled = false;
    }
    
    HLView::HLView(HLView* parent, HLRect frame):mFrame(frame)
    {
        mBounds.origin = HLPointZero;
        mBounds.size = frame.size;
        mEdgeInsets.top = mEdgeInsets.left = mEdgeInsets.bottom = mEdgeInsets.right = 0;
        mBackgroundColor.r=mBackgroundColor.g=mBackgroundColor.b=mBackgroundColor.a = 0;
        mClipToBounds = false;
        mVisible = true;
        mUserInteractionEnabled = false;
        mTag = 0;
        mParent = NULL;
        if (parent)
        {
            parent->addSubview(this);
        }
        setNeedsUpdateTransform();
        mUserData = NULL;
        mUserInteractionEnabled = false;
        mMultiTouchEnabled = false;
    }
    
    HLView::HLView(HLView* parent, const HLView& copy)
    {
        mFrame = copy.mFrame;
        mBounds = copy.mBounds;
        mEdgeInsets = copy.mEdgeInsets;
        mBackgroundColor = copy.mBackgroundColor;
        mClipToBounds = copy.mClipToBounds;
        mVisible = copy.mVisible;
        mUserInteractionEnabled = copy.mUserInteractionEnabled;
        mTag = copy.mTag;
        mParent = NULL;
        if (parent)
        {
            parent->addSubview(this);
        }
        setNeedsUpdateTransform();
        mUserData = NULL;
        mMultiTouchEnabled = false;
    }
    
    HLView::~HLView()
    {
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        HLMouseDispatcher::sharedMouseDispatcher()->viewWillDestroy(this);
#else
        HLTouchDispatcher::sharedTouchDispatcher()->viewWillDestroy(this);
#endif
        list<HLView*>::iterator itr = mSubViews.begin();
        for (; itr != mSubViews.end(); ++itr)
        {
            delete *itr;
        }
#if USE_JS_SCRIPTING
        JSView::removeJSObject(this);
#endif
        std::list<TweenAnimations*>::iterator it;
        for (it = allAnimations.begin(); it != allAnimations.end(); ++it)
        {
            if ((*it) && !(*it)->stopped)
            {
                std::vector<TweenAnimation*>::iterator itr = (*it)->animations.begin();
                for (; itr != (*it)->animations.end(); ++itr)
                {
                    if ((*itr)->_target == this)
                    {
                        delete (*it);
                        *it = NULL;
                        break;
                    }
                }
            }
        }
    }
    
    void HLView::setNeedsUpdateTransform()
    {
        mNeedsUpdateTransform = true;
        for (std::list<HLView*>::iterator itr = mSubViews.begin(); itr != mSubViews.end(); ++itr)
        {
            (*itr)->setNeedsUpdateTransform();
        }
    }
    
    HLView* HLView::hitTest(HLPoint p)
    {
        if (!mVisible)
        {
            return NULL;
        }
        HLPoint p1 = convertToNodeSpace(p);
        p1.x += mEdgeInsets.left;
        p1.y += mEdgeInsets.bottom;
        if (p1.x < 0 || p1.y < 0 || p1.x > mFrame.size.width || p1.y > mFrame.size.height)
        {
            return NULL;
        }
        list<HLView*>::reverse_iterator itr = mSubViews.rbegin();
        for (; itr != mSubViews.rend(); ++itr)
        {
            HLView* res = (*itr)->hitTest(p);
            if (res)
            {
                return res;
            }
        }
        if (mUserInteractionEnabled)
        {
            return this;
        }
        return NULL;
    }
    
    void HLView::setSize(HLSize size)
    {
        setSize(size.width, size.height);
    }
    
    void HLView::setSize(float width, float height)
    {
        if (currAnimations)
        {
            propertyChanged(this, "size", mFrame.size, HLSize(width, height));
            return;
        }
        mFrame.size.width = width;
        mFrame.size.height = height;
        mBounds.size.width = width - mEdgeInsets.left - mEdgeInsets.right;
        mBounds.size.height = height - mEdgeInsets.top - mEdgeInsets.bottom;
        setNeedsUpdateTransform();
        onInternalEvent("size_changed");
    }
    
    void HLView::setSizePercent(HLSize size)
    {
        setSizePercent(size.width, size.height);
    }
    
    void HLView::setSizePercent(float width, float height)
    {
        if (currAnimations)
        {
            propertyChanged(this, "size", mFrame.size, HLSize(mParent->getSize().width * width, mParent->getSize().height * height));
            return;
        }
        mFrame.size.width = mParent->getSize().width * width;
        mFrame.size.height = mParent->getSize().height * height;
        mBounds.size.width = mFrame.size.width - mEdgeInsets.left - mEdgeInsets.right;
        mBounds.size.height = mFrame.size.height - mEdgeInsets.top - mEdgeInsets.bottom;
        setNeedsUpdateTransform();
        onInternalEvent("size_changed");
    }
    
    void HLView::setPosition(HLPoint pos)
    {
        if (currAnimations)
        {
            propertyChanged(this, "position", mFrame.origin, pos);
            return;
        }
        mFrame.origin = pos;
        setNeedsUpdateTransform();
        onInternalEvent("position_changed");
    }
    
    void HLView::setPosition(float x, float y)
    {
        if (currAnimations)
        {
            propertyChanged(this, "position", mFrame.origin, HLPoint(x, y));
            return;
        }
        mFrame.origin.x = x;
        mFrame.origin.y = y;
        setNeedsUpdateTransform();
        onInternalEvent("position_changed");
    }
    
    void HLView::setPositionPercent(HLPoint pos)
    {
        if (currAnimations)
        {
            propertyChanged(this, "position", mFrame.origin, HLPoint(mParent->getSize().width * pos.x, mParent->getSize().height * pos.y));
            return;
        }
        mFrame.origin.x = mParent->getSize().width * pos.x;
        mFrame.origin.y = mParent->getSize().height * pos.y;
        setNeedsUpdateTransform();
        onInternalEvent("position_changed");
    }
    
    void HLView::setPositionPercent(float x, float y)
    {
        if (currAnimations)
        {
            propertyChanged(this, "position", mFrame.origin, HLPoint(mParent->getSize().width * x, mParent->getSize().height * y));
            return;
        }
        mFrame.origin.x = mParent->getSize().width * x;
        mFrame.origin.y = mParent->getSize().height * y;
        setNeedsUpdateTransform();
        onInternalEvent("position_changed");
    }
    
    void HLView::setCenter(HLPoint pos)
    {
        setCenter(pos.x, pos.y);
    }
    
    void HLView::setCenter(float x, float y)
    {
        setPosition(x - mFrame.size.width / 2, y - mFrame.size.height / 2);
    }
    
    void HLView::setFrame(HLRect frame)
    {
        if (currAnimations)
        {
            propertyChanged(this, "frame", mFrame, frame);
            return;
        }
        mFrame = frame;
        mBounds.size.width = frame.size.width - mEdgeInsets.left - mEdgeInsets.right;
        mBounds.size.height = frame.size.height - mEdgeInsets.top - mEdgeInsets.bottom;
        setNeedsUpdateTransform();
        onInternalEvent("frame_changed");
    }
    
    void HLView::setEdgeInsets(HLEdgeInsets insets)
    {
        mEdgeInsets = insets;
        mBounds.origin.x = insets.left;
        mBounds.origin.y = insets.top;
        mBounds.size.width = mFrame.size.width - mEdgeInsets.left - mEdgeInsets.right;
        mBounds.size.height = mFrame.size.height - mEdgeInsets.top - mEdgeInsets.bottom;
        setNeedsUpdateTransform();
        onInternalEvent("edgeinsets_changed");
    }
    
    const HLSize& HLView::getSize() const
    {
        return mFrame.size;
    }
    
    const HLSize HLView::getSizePercent() const
    {
        HLSize size;
        size.width = mFrame.size.width / mParent->getSize().width;
        size.height = mFrame.size.height / mParent->getSize().height;
        return size;
    }
    
    const HLPoint& HLView::getPosition() const
    {
        return mFrame.origin;
    }
    
    const HLPoint HLView::getPositionPercent() const
    {
        HLPoint pos;
        pos.x = mFrame.origin.x / mParent->getSize().width;
        pos.y = mFrame.origin.y / mParent->getSize().height;
        return pos;
    }
    
    const HLPoint HLView::getCenter() const
    {
        return HLPoint(mFrame.origin.x + mFrame.size.width/2, mFrame.origin.y + mFrame.size.height/2);
    }
    
    void HLView::setVisible(bool visible)
    {
        if (mVisible != visible)
        {
            mVisible = visible;
            onInternalEvent("visible_changed");
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
            HLMouseDispatcher::sharedMouseDispatcher()->needsHitTest();
#else
            HLTouchDispatcher::sharedTouchDispatcher()->needsHitTest();
#endif
        }
    }
    
    bool HLView::isVisible() const
    {
        return mVisible;
    }
    
    void HLView::setUserInteractionEnabled(bool enable)
    {
        if (mUserInteractionEnabled != enable)
        {
            mUserInteractionEnabled = enable;
            onInternalEvent("userInteraction_changed");
            HLTouchDispatcher::sharedTouchDispatcher()->needsHitTest();
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
            HLMouseDispatcher::sharedMouseDispatcher()->needsHitTest();
#endif
        }
    }
    
    bool HLView::userInteractionEnalbed() const
    {
        return mUserInteractionEnabled;
    }
    
    void HLView::setMultiTouchEnabled(bool enable)
    {
        mMultiTouchEnabled = enable;
    }
    
    bool HLView::multiTouchEnabled() const
    {
        return mMultiTouchEnabled;
    }
    
    void HLView::setClipToBounds(bool clip)
    {
        mClipToBounds = clip;
        onInternalEvent("clipToBounds_changed");
    }
    
    bool HLView::clipToBounds() const
    {
        return mClipToBounds;
    }
    
    void HLView::setBackgroundColor(color4B color)
    {
        mBackgroundColor = color;
        onInternalEvent("backgroundcolor_changed");
    }
    
    const color4B& HLView::getBackgroundColor() const
    {
        return mBackgroundColor;
    }
    
    void HLView::setTag(int32_t tag)
    {
        mTag = tag;
        onInternalEvent("tag_changed");
    }
    
    int32_t HLView::getTag() const
    {
        return mTag;
    }
    
    HLView* HLView::findViewWithTag(int32_t tag)
    {
        list<HLView*>::const_iterator itr = mSubViews.begin();
        for (; itr != mSubViews.end(); ++itr)
        {
            if ((*itr)->getTag() == tag)
            {
                return *itr;
            }
        }
        return NULL;
    }
    
    void HLView::centerInParent()
    {
        float x = (mParent->getSize().width - mFrame.size.width) / 2;
        float y = (mParent->getSize().height - mFrame.size.height) / 2;
        setPosition(x, y);
    }
    
    void HLView::centerHorizontallyInParent()
    {
        float x = (mParent->getSize().width - mFrame.size.width) / 2;
        setPosition(x, mFrame.origin.y);
    }
    
    void HLView::centerVerticallyInParent()
    {
        float y = (mParent->getSize().height - mFrame.size.width) / 2;
        setPosition(mFrame.origin.x, y);
    }
    
    void HLView::leftInParent()
    {
        setPosition(0, mFrame.origin.y);
    }
    
    void HLView::rightInParent()
    {
        setPosition(mParent->getSize().width-mFrame.size.width, mFrame.origin.y);
    }
    
    void HLView::topInParent()
    {
        setPosition(mFrame.origin.x, 0);
    }
    
    void HLView::bottomInParent()
    {
        setPosition(mFrame.origin.x, mParent->getSize().height - mFrame.size.height);
    }
    
    void HLView::addSubview(HLView * view)
    {
        HLASSERT(view->mParent == NULL, "view already have parent");
        HLASSERT(!(view == this), "can not add self to subview");

        mSubViews.push_back(view);
        view->mParent = this;
        view->setNeedsUpdateTransform();
        onInternalEvent("subview_added");
        
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        HLMouseDispatcher::sharedMouseDispatcher()->needsHitTest();
#else
        HLTouchDispatcher::sharedTouchDispatcher()->needsHitTest();
#endif
    }
    
    void HLView::updateTransform()
    {
        float winSizeHeight = mParent?
                                mParent->mBounds.size.height:
                                HLDirector2D::getSingleton()->getWinSize().height;
        HLPoint p = mFrame.origin;
        p.x += mEdgeInsets.left;
        p.y += mFrame.size.height - mEdgeInsets.bottom;
        p.y = winSizeHeight - p.y;
        kmMat4Translation(&mTransform, p.x, p.y, 0);
    }
    
    kmMat4* HLView::getTransform()
    {
        if (mNeedsUpdateTransform)
        {
            updateTransform();
            mNeedsUpdateTransform = false;
        }
        return &mTransform;
    }
    
    void HLView::visit()
    {
        if (!mVisible)
        {
            return;
        }
        
        HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
        
        matrixStack->pushMatrix();
        matrixStack->multMatrix(getTransform());
        hlglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        if (mBackgroundColor.a != 0)
        {
            drawColor4B(mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, mBackgroundColor.a);
            drawSolidRect(HLPoint(-mEdgeInsets.left, -mEdgeInsets.bottom), HLPoint(mFrame.size.width - mEdgeInsets.left, mFrame.size.height - mEdgeInsets.bottom));
        }
        bool scissorEnabled = hlglIsEnabled(GL_SCISSOR_TEST);
        GLint scissorBox[4];
        if (mClipToBounds)
        {
            if (scissorEnabled)
            {
                hlglGetScissorBox(scissorBox);
            }
            else
            {
                hlglEnable(GL_SCISSOR_TEST);
            }
            float scale = HLDirector2D::getSingleton()->getWinSizeScale();
            kmMat4 trans;
            HLGLMatrixStack::matrixStack()->getMatrix(KM_GL_MODELVIEW, &trans);
//            kmMat4* transform = nodeToWorldTransform();
            // intersect two rects, when scissorEnabled
            HLSize winSizeInPixel = HLDirector2D::getSingleton()->getWinSizeInPixels();
            HLPoint loc = HLPoint(trans.mat[12], trans.mat[13]);
            loc = HLDirector2D::getSingleton()->convertToScreenPoint(loc)*CONTENT_SCALE_FACTOR;
            loc.y = winSizeInPixel.height - loc.y;
            if (scissorEnabled)
            {
                float left = MAX(loc.x, scissorBox[0]);
                float bottom = MAX(loc.y, scissorBox[1]);
                float right = MIN(loc.x+mBounds.size.width/scale, scissorBox[0]+scissorBox[2]);
                float top = MIN(loc.y+mBounds.size.height/scale, scissorBox[1]+scissorBox[3]);
                if (right > left && top > bottom)
                {
                    hlglScissor(roundf(left), roundf(bottom), roundf((right - left)), roundf((top - bottom)));
                }
                else
                {
                    matrixStack->popMatrix();
                    return;
                }
            }
            else
            {
                hlglScissor(roundf(loc.x), roundf(loc.y), mBounds.size.width/scale, mBounds.size.height/scale);
            }
        }
        onDraw();
        if (mClipToBounds)
        {
            if (scissorEnabled)
            {
                hlglScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
            }
            else
            {
                hlglDisable(GL_SCISSOR_TEST);
            }
        }
        matrixStack->popMatrix();
    }
    
    void HLView::onDraw()
    {
        list<HLView*>::const_iterator itr = mSubViews.begin();
        for (; itr != mSubViews.end(); ++itr)
        {
            if ((*itr)->isVisible())
            {
                (*itr)->visit();
            }
        }
    }
    
    void HLView::removeFromParent(bool cleanup)
    {
        mParent->removeSubview(this, cleanup);
    }
    
    void HLView::removeSubview(HLView * view, bool cleanup)
    {
        list<HLView*>::iterator itr = mSubViews.begin();
        for (; itr != mSubViews.end(); ++itr)
        {
            if (*itr == view)
            {
                mSubViews.erase(itr);
                if (cleanup)
                {
                    delete view;
                }
                else
                {
                    view->mParent = NULL;
                }

                return;
            }
        }
        onInternalEvent("subview_modified");
    }
    
    void HLView::removeAllSubviews(bool cleanup)
    {
        list<HLView*>::iterator itr = mSubViews.begin();
        if (cleanup)
        {
            for (; itr != mSubViews.end(); ++itr)
            {
                delete *itr;
            }
        }
        mSubViews.clear();
        onInternalEvent("subview_modified");
    }
    
    void HLView::bringSubviewToFront(HLView* view)
    {
        std::list<HLView*>::iterator itr = std::find(mSubViews.begin(), mSubViews.end(), view);
        HLASSERT(itr != mSubViews.end(), "subview not found");
        mSubViews.erase(itr);
        mSubViews.push_back(view);
        onInternalEvent("subview_modified");
    }
    
    void HLView::sendSubviewToBack(HLView* view)
    {
        std::list<HLView*>::iterator itr = std::find(mSubViews.begin(), mSubViews.end(), view);
        HLASSERT(itr != mSubViews.end(), "subview not found");
        mSubViews.erase(itr);
        mSubViews.push_front(view);
        onInternalEvent("subview_modified");
    }
    
    void HLView::moveSubviewBelowSubivew(HLView* view1, HLView* view2)
    {
        std::list<HLView*>::iterator itr = std::find(mSubViews.begin(), mSubViews.end(), view2);
        std::list<HLView*>::iterator itr1 = std::find(mSubViews.begin(), mSubViews.end(), view1);
        HLASSERT(itr != mSubViews.end() && itr1 != mSubViews.end(), "view not found");
        mSubViews.erase(itr1);
        mSubViews.insert(itr, view1);
        onInternalEvent("subview_modified");
    }

    
    void HLView::startAnimations(std::string name, float duration, int repeat, void* context)
    {
        currAnimations = new TweenAnimations(name ,duration, repeat, context);
    }
    
    void HLView::commitAnimations()
    {
        currAnimations->animationDelegate = animationDidStopCallback;
        allAnimations.push_back(currAnimations);
        currAnimations = NULL;
        animationDidStopCallback.clear();
    }
    
    void HLView::setAutoReverse(bool b)
    {
        if (currAnimations)
        {
            currAnimations->autoreverse = b;
        }
    }
    
    void HLView::update(float dt)
    {
        std::list<TweenAnimations*>& anims = allAnimations;
        std::list<TweenAnimations*>::iterator itr;
        for (itr = anims.begin(); itr != anims.end();)
        {
            if ((*itr) == NULL)
            {
                itr = anims.erase(itr);
            }
            else if ((*itr)->update(dt))
            {
                delete (*itr);
                itr = anims.erase(itr);
            }
            else
            {
                ++itr;
            }
        }
    }
    
    void HLView::setAnimationCurve(HLAnimationCurve curve)
    {
        if (currAnimations)
        {
            currAnimations->curve = curve;
        }
    }
    
    void HLView::setAnimationCurveRate(float curveRate)
    {
        if (currAnimations)
        {
            currAnimations->curveRate = curveRate;
        }
    }
    
    void HLView::stopAnimation(std::string name)
    {
        std::list<TweenAnimations*>::iterator it;
        for (it = allAnimations.begin(); it != allAnimations.end(); ++it)
        {
            if ((*it) && (*it)->name == name && !(*it)->stopped)
            {
                (*it)->animationDelegate(false, (*it)->context);
                delete (*it);
                *it = NULL;
            }
        }
    }
    
    void HLView::stopAllAnimations()
    {
        std::list<TweenAnimations*>::iterator it;
        for (it = allAnimations.begin(); it != allAnimations.end(); it++)
        {
            if (!(*it)->stopped)
            {
                (*it)->animationDelegate(false, (*it)->context);
                delete (*it);
            }
        }
        allAnimations.clear();
    }
    
#if USE_JS_SCRIPTING
    JSCppWrapperBase* HLView::getOrCreateJSWrapper(JSContext* cx)
    {
        return JSView::getOrCreateWrapper(cx, this);
    }
    
    void HLView::setJSDraggingHandler(JSObject* obj, jsval func)
    {
        jsviewdraghander.setCallback(obj, func);
        draggingEventHandler = newDelegate(&jsviewdraghander, &JSViewDragEventHandler::onDragEvent);
    }
#endif
    
    HLView* HLView::clone(HLView* parent)
    {
        HLView* view = new HLView(parent, *this);
        std::list<HLView*>::iterator itr = mSubViews.begin();
        for (; itr != mSubViews.end(); ++itr)
        {
            (*itr)->clone(view);
        }
        return view;
    }
}

NS_HL_END
