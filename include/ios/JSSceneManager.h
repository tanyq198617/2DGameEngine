//
//  JSSceneManager.h
//  HoolaiEngine
//
//  Created by zhao shuan on 1/3/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __JSSCENEMANAGER_H__
#define __JSSCENEMANAGER_H__

#include "jsapi.h"
#include "HLSceneManager.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

class JSEntityManager;

class JSSceneManager: public JSCPPWrapper<JSSceneManager, HLSceneManager>
{
public:
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetEntityManager(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsAddEntity(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsAddOnActiveListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsAddOnDeactiveListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsAddOnDestroyListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetActiveCamera(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRemoveOnActiveListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRemoveOnDeactiveListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRemoveOnDestroyListener(JSContext *cx, uint32_t argc, jsval *vp);
};

NS_HL_END

#endif
