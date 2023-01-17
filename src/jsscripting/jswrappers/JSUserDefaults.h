//
//  JSUserDefaults.h
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-11-23.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSUSERDEFAULTS_H__
#define __JSUSERDEFAULTS_H__

#include "jsapi.h"
#include "HLMacros.h"

NS_HL_BEGIN

class JSUserDefaults
{
public:
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    static JSBool jsGetBoolForKey(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetIntForKey(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetFloatForKey(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetStringForKey(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetBoolForKey(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetIntForKey(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetFloatForKey(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetStringForKey(JSContext *cx, uint32_t argc, jsval *vp);
};

NS_HL_END

#endif
