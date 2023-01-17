//
//  JSLabel.h
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSLABEL_H__
#define __JSLABEL_H__

#include "HLLabel.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
class HLLabel;

class JSLabel: public JSCPPWrapper<JSLabel, HLLabel>
{
public:
    enum
    {
        kTextAlign,
        kBrightness,
        kText,
        kFontSize,
        kTextColorTL,
        kTextColorBL,
        kTextColorTR,
        kTextColorBR,
        kTextStroke,
        kTextColor,
        kBold,
        kTextStrokeColor
    };

    static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
    static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);

    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);

    static JSBool jsSetText(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetText(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetFontSize(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetFontSize(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetBrightness(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetBrightness(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTextColor(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTextColorTL(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTextColorTR(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTextColorBL(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTextColorBR(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetTextColorTL(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetTextColorTR(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetTextColorBL(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetTextColorBR(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTextStroke(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsIsTextStroke(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTextStrokeColor(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetTextStrokeColor(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTextAlignment(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetTextAlignment(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetIsFontBold(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsIsFontBold(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsResizeHeightToFits(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSizeThatFits(JSContext *cx, uint32_t argc, jsval *vp);
    //static JSBool jsSetFont(JSContext *cx, uint32_t argc, jsval *vp);
};
}

NS_HL_END

#endif
