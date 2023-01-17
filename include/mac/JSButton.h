//
//  JSButton.h
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSBUTTON_H__
#define __JSBUTTON_H__

#include "HLButton.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
class HLButton;
class JSButton: public JSCPPWrapper<JSButton, HLButton>
{
public:
    enum
    {
        kZoomOnTouchDown,
        kButtonState,
        kNormalTitle,
        kSelectedTitle,
        kDisabledTitle,
        kHighlightedTitle
    };

    static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
    static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);

    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);

    static JSBool jsSetSelected(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetButtonState(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetEnabled(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetHighlighted(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTitleSize(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTitleColor(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetTitleLabel(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetImageView(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetNormalBackgroundImage(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetNormalBackgroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetNormalBackgroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetDisabledBackgroundImage(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetDisabledBackgroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetDisabledBackgroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetSelectedBackgroundImage(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetSelectedBackgroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetSelectedBackgroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetHighlightedBackgroundImage(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetHighlightedBackgroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetHighlightedBackgroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetNormalTitle(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetNormalTitle(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetSelectedTitle(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetSelectedTitle(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetDisabledTitle(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetDisabledTitle(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetHighlightedTitle(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetHighlightedTitle(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetNormalBackgroundSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetSelectedBackgroundSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetDisableBackgroundSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetHighlightedBackgroundSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetOnButtonClickListener(JSContext *cx, uint32_t argc, jsval *vp);
};

}

NS_HL_END

#endif
