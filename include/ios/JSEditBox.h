//
//  JSEditBox.h
//  HoolaiEngine
//
//  Created by cai wei on 23/4/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __JSEDITBOX_H__
#define __JSEDITBOX_H__

#include "HLEditBox.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
class HLEditBox;

class JSEditBox: public JSCPPWrapper<JSEditBox, HLEditBox>
{
public:
    enum
    {
        kReturnKeyType,
        kInputMode,
        kSecureInput
    };

    static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
    static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);
    
    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);

    static JSBool jsSetKeyboardReturnType(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetKeyboardReturnType(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetInputMode(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetInputMode(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsActive(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsResignActive(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetSecureInput(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsIsSecureInput(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetText(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetTextLabel(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetPlaceHolder(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetPlaceHolderLabel(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetText(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsClearText(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsInsertText(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetCharInputFilter(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetMaxLength(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsDeleteBackward(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetMultiline(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetOnEditBeginListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetOnEditEndListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetOnTextChangedListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetOnReturnListener(JSContext *cx, uint32_t argc, jsval *vp);
};
}

NS_HL_END

#endif
