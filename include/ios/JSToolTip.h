//
//  JSToolTip.h
//  HoolaiEngine
//
//  Created by cai wei on 23-4-13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __JSToolTip_H__
#define __JSToolTip_H__

#include "HLToolTip.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
class HLToolTip;

class JSToolTip: public JSCPPWrapper<JSToolTip, HLToolTip>
{
public:
    static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
    static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);
    
    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);

//    static JSBool jsSetEventDispatchRect(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsShow(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsDismiss(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetOnDismissListener(JSContext *cx, uint32_t argc, jsval *vp);
};
}

NS_HL_END

#endif
