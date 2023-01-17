//
//  JSHttpRequest.cpp
//  HoolaiEngine
//
//  Created by zs_hoolai on 14-9-2.
//  Copyright (c) 2014年 ICT. All rights reserved.
//

#include "JSHttpRequest.h"
#include "HLHttpRequest.h"
#include "jsfriendapi.h"

NS_HL_BEGIN

template<> JSObject* JSCPPWrapper<JSHttpRequest, HLHttpRequest>::proto = NULL;
template<> JSClass* JSCPPWrapper<JSHttpRequest, HLHttpRequest>::jsclass = NULL;

JSBool JSHttpRequest::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc > 0)
    {
        JSString* arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        HLHttpRequest* req = new HLHttpRequest(s);
        JS_free(cx, s);
        if (argc > 1)
        {
            JSString* arg1 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[1]);
            s = JS_EncodeStringToUTF8(cx, arg1);
            req->setMethod(s);
            JS_free(cx, s);
            if (argc > 2)
            {
                req->setTimeoutInterval(JSVAL_TO_INT(JS_ARGV(cx, vp)[2]));
            }
        }
        JSHttpRequest* tmp = JSHttpRequest::getOrCreateWrapper(cx, req);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    JS_ReportError(cx, "JSHttpRequest: wrong args for constructor");
    return JS_FALSE;
}

JSBool JSHttpRequest::jsGetUrl(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSHttpRequest* thiz = (JSHttpRequest*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSHttpRequest::getUrl: native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(thiz->getNativeObject()->getUrl().c_str()));
    return JS_TRUE;
}

JSBool JSHttpRequest::jsGetMethod(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSHttpRequest* thiz = (JSHttpRequest*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSHttpRequest::getMethod: native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(thiz->getNativeObject()->getMethod().c_str()));
    return JS_TRUE;
}

JSBool JSHttpRequest::jsGetBody(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSHttpRequest* thiz = (JSHttpRequest*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSHttpRequest::getBody: native object already disposed");
        return JS_FALSE;
    }
    const std::vector<char>& body = thiz->getNativeObject()->getBody();
    void * contents = NULL;
    uint8_t * data = NULL;
    if (!JS_AllocateArrayBufferContents(cx, (uint32_t)body.size(), &contents, &data))
    {
        JS_ReportError(cx, "JSHttpRequest::getBody: alloc memory error");
        return JS_FALSE;
    }
    memcpy(data, &body[0], body.size());
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JS_NewArrayBufferWithContents(cx, contents)));
    
    return JS_TRUE;
}

JSBool JSHttpRequest::jsGetTimeoutInterval(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSHttpRequest* thiz = (JSHttpRequest*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSHttpRequest::getTimeoutInterval: native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, INT_TO_JSVAL(thiz->getNativeObject()->getTimeoutInterval()));
    return JS_TRUE;
}

JSBool JSHttpRequest::jsSetBody(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSHttpRequest* thiz = (JSHttpRequest*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSHttpRequest::setBody: native object already disposed");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jsval arg = JS_ARGV(cx, vp)[0];
        if (JSVAL_IS_STRING(arg))
        {
            char *s = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(arg));
            thiz->getNativeObject()->setBody(s, strlen(s));
            JS_free(cx, s);
            return JS_TRUE;
        }
        JSObject* argobj = JSVAL_TO_OBJECT(arg);
        if (JS_IsArrayBufferObject(argobj))
        {
            uint32_t len = JS_GetArrayBufferByteLength(argobj);
            uint8_t *data = JS_GetArrayBufferData(argobj);
            thiz->getNativeObject()->setBody((const char *)data, len);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSHttpRequest::setBody: wrong argument type");
        return JS_FALSE;
    }
    JS_ReportError(cx, "JSHttpRequest::setBody: wrong argument number");
    return JS_FALSE;
}

JSBool JSHttpRequest::jsSetMethod(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSHttpRequest* thiz = (JSHttpRequest*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSHttpRequest::setMethod: native object already disposed");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jsval arg = JS_ARGV(cx, vp)[0];
        if (JSVAL_IS_STRING(arg))
        {
            char *s = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(arg));
            thiz->getNativeObject()->setMethod(s);
            JS_free(cx, s);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSHttpRequest::setMethod: wrong argument type");
        return JS_FALSE;
    }
    JS_ReportError(cx, "JSHttpRequest::setMethod: wrong argument number");
    return JS_FALSE;
}

JSBool JSHttpRequest::jsSetHeader(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSHttpRequest* thiz = (JSHttpRequest*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSHttpRequest::setHeader: native object already disposed");
        return JS_FALSE;
    }
    if (argc == 2)
    {
        jsval arg0 = JS_ARGV(cx, vp)[0];
        jsval arg1 = JS_ARGV(cx, vp)[1];
        if (JSVAL_IS_STRING(arg0) && JSVAL_IS_STRING(arg1))
        {
            char *s0 = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(arg0));
            char *s1 = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(arg1));
            thiz->getNativeObject()->setHeader(s0, s1);
            JS_free(cx, s0);
            JS_free(cx, s1);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSHttpRequest::setHeader: wrong argument type");
        return JS_FALSE;
    }
    JS_ReportError(cx, "JSHttpRequest::setHeader: wrong argument number");
    return JS_FALSE;
}

JSBool JSHttpRequest::jsGetHeaders(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSHttpRequest* thiz = (JSHttpRequest*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSHttpRequest::getHeaders: native object already disposed");
        return JS_FALSE;
    }
    const std::map<std::string, std::string>& headers = thiz->getNativeObject()->getHeaders();
    JSObject* ret = JS_NewObject(cx, NULL, NULL, NULL);
    for (std::map<std::string, std::string>::const_iterator itr = headers.begin(); itr != headers.end(); ++itr)
    {
        JS::RootedValue prop(cx);
        prop.set(value_to_jsval(itr->second));
        JS_SetProperty(cx, ret, itr->first.c_str(), prop);
    }
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(ret));
    return JS_TRUE;
}

JSBool JSHttpRequest::jsSetGlobalHeader(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        jsval arg0 = JS_ARGV(cx, vp)[0];
        jsval arg1 = JS_ARGV(cx, vp)[1];
        if (JSVAL_IS_STRING(arg0) && JSVAL_IS_STRING(arg1))
        {
            char *s0 = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(arg0));
            char *s1 = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(arg1));
            HLHttpRequest::setGlobalHeader(s0, s1);
            JS_free(cx, s0);
            JS_free(cx, s1);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSHttpRequest::setGlobalHeader: wrong argument type");
        return JS_FALSE;
    }
    JS_ReportError(cx, "JSHttpRequest::setGlobalHeader: wrong argument number");
    return JS_FALSE;
}

JSBool JSHttpRequest::jsStartRequest(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSHttpRequest* thiz = (JSHttpRequest*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSHttpRequest::startRequest: native object already disposed");
        return JS_FALSE;
    }
    thiz->getNativeObject()->startRequest();
    return JS_TRUE;
}

JSBool JSHttpRequest::jsSetResponseHandler(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSHttpRequest* thiz = (JSHttpRequest*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSHttpRequest::setResponseHandler: native object already disposed");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        thiz->getNativeObject()->onResponse = newDelegate<HLHttpRequest*, int, std::vector<char> >(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        thiz->getNativeObject()->onResponse = newDelegate<HLHttpRequest*, int, std::vector<char> >(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    JS_ReportError(cx, "JSHttpRequest::setResponseHandler: wrong argument number");
    return JS_FALSE;
}

JSBool JSHttpRequest::jsSetErrorHandler(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSHttpRequest* thiz = (JSHttpRequest*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSHttpRequest::setErrorHandler: native object already disposed");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        thiz->getNativeObject()->onError = newDelegate<HLHttpRequest*, const char*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        thiz->getNativeObject()->onError = newDelegate<HLHttpRequest*, const char*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    JS_ReportError(cx, "JSHttpRequest::setErrorHandler: wrong argument number");
    return JS_FALSE;
}

void JSHttpRequest::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
{
    if (!jsclass)
    {
        jsclass = (JSClass *)calloc(1, sizeof(JSClass));
    }
	jsclass->name = name;
	jsclass->addProperty = JS_PropertyStub;
	jsclass->delProperty = JS_DeletePropertyStub;
	jsclass->getProperty = JS_PropertyStub;
	jsclass->setProperty = JS_StrictPropertyStub;
	jsclass->enumerate = JS_EnumerateStub;
	jsclass->resolve = JS_ResolveStub;
	jsclass->convert = JS_ConvertStub;
	jsclass->finalize = jsFinalize;
	jsclass->flags = JSCLASS_HAS_PRIVATE;
    
    static JSPropertySpec properties[] = {
        {0, 0, 0, 0, 0}
    };
    static JSFunctionSpec funcs[] = {
        JS_FN("getUrl", JSHttpRequest::jsGetUrl, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getMethod", JSHttpRequest::jsGetMethod, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getBody", JSHttpRequest::jsGetBody, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getTimeoutInterval", JSHttpRequest::jsGetTimeoutInterval, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setBody", JSHttpRequest::jsSetBody, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setMethod", JSHttpRequest::jsSetMethod, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setHeader", JSHttpRequest::jsSetHeader, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getHeaders", JSHttpRequest::jsGetHeaders, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("startRequest", JSHttpRequest::jsStartRequest, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setResponseHandler", JSHttpRequest::jsSetResponseHandler, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setErrorHandler", JSHttpRequest::jsSetErrorHandler, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FN("setGlobalHeader", JSHttpRequest::jsSetGlobalHeader, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    proto = JS_InitClass(cx,globalObj,NULL,jsclass,jsConstructor,0,properties,funcs,NULL,st_funcs);
}

NS_HL_END