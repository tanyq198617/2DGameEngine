//
//  JSProgressRadialView.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-5-9.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSPROGRESSRADIAVIEW_H__
#define __JSPROGRESSRADIAVIEW_H__

#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
    class HLProgressRadialView;
    class JSProgressRadialView:public JSCPPWrapper<JSProgressRadialView, HLProgressRadialView>
    {
    public:
        static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
        static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
        
        enum {
            kProgress,
        };
        
        static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
        static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);
        
        static JSBool jsSetColor(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetImage(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetImageTexture(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetStartAngle(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetEndAngle(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetCenterPoint(JSContext *cx, uint32_t argc, jsval *vp);
    };
}

NS_HL_END

#endif
