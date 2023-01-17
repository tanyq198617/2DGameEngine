//
//  JSView.h
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSVIEW_H__
#define __JSVIEW_H__

#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
    class HLView;
    
    class JSView: public JSCPPWrapper<JSView, HLView>
    {
    public:
        enum {
            kFrame,
            kCenter,
            kPosition,
            kSize,
            kPositionInPercent,
            kSizeInPercent,
            kVisible,
            kUserInteractionEnabled,
            kClipToBounds,
            kBackgroundColor,
            kTag,
            kAnimationDelegate,
            kParent
        };
        
        static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
        static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);
        
        static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
        static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
        static bool jsHasInstance(JSContext *cx, JS::Handle<JSObject*> obj, JS::MutableHandle<JS::Value> vp, bool *bp);
        
        static JSBool jsCenterInParent(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsAddSubview(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsRemoveFromParent(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsRemoveSubview(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsRemoveAllSubviews(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsFindViewWithTag(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsBringSubviewToFront(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSendSubviewToBack(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsMoveSubviewBelowSubview(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsGetSubviews(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsConvertToNodeSpace(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsConvertToWorldSpace(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsTouchLocationInView(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsOnTouches(JSContext *cx, uint32_t argc, jsval *vp);
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        static JSBool jsMouseLocationInView(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsOnMouseEvent(JSContext *cx, uint32_t argc, jsval *vp);
#endif
        
        static JSBool jsStartAnimations(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetAnimationDidStopCallback(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetAutoReverse(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetAnimationCurve(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetAnimationCurveRate(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsStopAnimation(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsStopAllAnimations(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsCommitAnimations(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetTouchEventListener(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetDraggingEventListener(JSContext *cx, uint32_t argc, jsval *vp);
        
        static JSBool jsClone(JSContext *cx, uint32_t argc, jsval *vp);
    };
}

NS_HL_END

#endif
