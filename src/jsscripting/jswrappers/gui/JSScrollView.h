//
//  JSScrollView.h
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSSCROLLVIEW_H__
#define __JSSCROLLVIEW_H__

#include "HLScrollView.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
class HLScrollView;

class JSScrollView: public JSCPPWrapper<JSScrollView, HLScrollView>
{
public:
    enum {
        kContentOffset,
        kContentSize,
        kDirection,
        kPaging
    };
    static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
    static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);
    
    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);

    static JSBool jsSetContentOffset(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetContentOffset(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetContentOffsetInDuration(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetZoomScale(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetZoomScale(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetZoomScaleInDuration(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsMinContainerOffset(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsMaxContainerOffset(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsIsViewVisible(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetContentView(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsIsDragging(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsIsTouchMoved(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsIsBounceable(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetBounceable(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTouchEnable(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetTouchEnable(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsIsPagingEnabled(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetPagingEnabled(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetDirection(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetDirection(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetContentSize(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetContentSize(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetScrollViewDidDragDelegate(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetScrollViewDidScrollDelegate(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetScrollViewDidEndScrollingAnimationDelegate(JSContext *cx, uint32_t argc, jsval *vp);
};
}

NS_HL_END

#endif
