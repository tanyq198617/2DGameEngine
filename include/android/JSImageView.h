//
//  JSImageView.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSIMAGEVIEW_H__
#define __JSIMAGEVIEW_H__

#include "HLImageView.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
class HLImageView;

class JSImageView: public JSCPPWrapper<JSImageView, HLImageView>
{
public:
    enum
    {
        kTintColor,
        kBrightness,
        kShowGrayImage,
        kFlipX,
        kFlipY,
        kRotation,
        kRepeatStretch,
        kGlowRadius,
        kGlowColor
    };

    static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
    static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);
    
    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);

    static JSBool jsSetImage(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetImageTexture(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
};
}

NS_HL_END

#endif
