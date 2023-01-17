//
//  JSTexture.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-29.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __JSSPRITEFRAME_H__
#define __JSSPRITEFRAME_H__

#include "jsapi.h"
#include "HLSpriteFrame.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

class JSSpriteFrame: public JSCPPWrapper<JSSpriteFrame, HLSpriteFrame>
{
public:
    // readony properties
    enum {
        kName,
        kTextureRect,
        kOffset,
        kSourceSize,
        kTexture,
        kRotated
    };
	static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
    
    static JSBool jsGetSpriteFrameArray(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetSpriteFrameDictionary(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsPurgeCachedData(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRetain(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRelease(JSContext *cx, uint32_t argc, jsval *vp);
    
    static JSBool jsSetDisplayFrameForEntity(JSContext *cx, uint32_t argc, jsval *vp);
};

NS_HL_END

#endif
