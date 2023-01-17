//
//  JSProgressBar.h
//  HoolaiEngine
//
//  Created by cai wei on 2/5/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __JSPROGRESSBAR_H__
#define __JSPROGRESSBAR_H__

#include "HLProgressIndicator.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
class HLProgressBar;

class JSProgressBar: public JSCPPWrapper<JSProgressBar, HLProgressBar>
{
public:
    enum
    {
        kValue,
        kValue1
    };

    static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
    static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);

    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);

    static JSBool jsSetRange(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetBackgroundImage(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetBackgroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetBackgroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetForegroundImage(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetForegroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetForegroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetRepeatForegroundImage(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetRepeatForegroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetRepeatForegroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
};
}

NS_HL_END

#endif
