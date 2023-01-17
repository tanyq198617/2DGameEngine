//
//  JSToast.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSTOAST_H__
#define __JSTOAST_H__

#include "HLToast.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
class HLToast;

class JSToast: public JSCPPWrapper<JSToast, HLToast>
{
public:
    static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
    static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);
    
    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);

    static JSBool jsShow(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetLabel(JSContext *cx, uint32_t argc, jsval *vp);
};
}

NS_HL_END

#endif
