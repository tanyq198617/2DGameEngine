//
//  JSTexture2D.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-29.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __JSTEXTURE2D_H__
#define __JSTEXTURE2D_H__

#include "jsapi.h"
#include "HLTexture.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

class JSTexture:public JSCPPWrapper<JSTexture, HLTexture>
{
public:
    enum {
        kSize,
        kScale
    };
    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
	static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
//	static JSBool jsPropertySet(JSContext *cx, JSObject *obj, jsid _id, JSBool strict, jsval *val);
    static JSBool jsGetTexture(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsIsLoaded(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRetain(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRelease(JSContext *cx, uint32_t argc, jsval *vp);
    
	static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
};

NS_HL_END

#endif
