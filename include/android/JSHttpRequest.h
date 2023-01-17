//
//  JSHttpRequest.h
//  HoolaiEngine
//
//  Created by zs_hoolai on 14-9-2.
//  Copyright (c) 2014年 ICT. All rights reserved.
//

#ifndef __HoolaiEngine__JSHTTPREQUEST__
#define __HoolaiEngine__JSHTTPREQUEST__

#include "JSCPPWrapper.h"

NS_HL_BEGIN

class HLHttpRequest;

class JSHttpRequest:public JSCPPWrapper<JSHttpRequest, HLHttpRequest>
{
public:
    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);// praram: url, [method, timeout]
    static JSBool jsGetUrl(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetMethod(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetBody(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetTimeoutInterval(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetBody(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetMethod(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetHeader(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetHeaders(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetGlobalHeader(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsStartRequest(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetResponseHandler(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetErrorHandler(JSContext *cx, uint32_t argc, jsval *vp);
    
	static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
};

NS_HL_END

#endif /* defined(__HoolaiEngine__JSHttpRequest__) */
