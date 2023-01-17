//
//  JSRichLabel.h
//  HoolaiEngine
//
//  Created by tyq on 13-7-30.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSRICHLABEL_H__
#define __JSRICHLABEL_H__

#include "HLLabel.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
    class HLRichLabel;
    
    class JSRichLabel: public JSCPPWrapper<JSRichLabel, HLRichLabel>
    {
    public:
        enum
        {
            kTextAlign,
//            kBrightness,
            kText,
            kFontSize,
            kTextStroke,
            kTextColor,
            kTextStrokeColor,
            kLinkTextColor,
            kUnderlineLink
        };
        
        static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
        static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);
        
        static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
        static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
        static JSBool jsSizeThatFits(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetDelegate(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsStartShowAnimation(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsStopShowAnimation(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetAlpha(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsIsInAnimation(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetAnimDelegate(JSContext *cx, uint32_t argc, jsval *vp);
    };
}

NS_HL_END

#endif
