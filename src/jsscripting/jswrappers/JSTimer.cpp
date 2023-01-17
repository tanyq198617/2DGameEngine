//
//  JSTimer.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-24.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSTimer.h"
#include "HLTimer.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<JSTimer, HLTimer>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<JSTimer, HLTimer>::proto = NULL;

void JSTimer::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        JS_FN("setDelegate", JSTimer::jsSetDelegate, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("cancel", JSTimer::jsCancel, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FS_END
    };
    proto = JS_InitClass(cx,globalObj,NULL,jsclass,JSTimer::jsConstructor,0,properties,funcs,NULL,st_funcs);
}

JSBool JSTimer::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc > 0)
    {
        bool repeate = false;
        float interval;
        if (JSVAL_IS_INT(JS_ARGV(cx, vp)[0]))
        {
            interval = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
        }
        else
        {
            interval = JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[0]);
        }
        if (argc > 1)
        {
            repeate = JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[1]);
        }
        HLTimer *timer = new HLTimer(interval, repeate);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(getOrCreateWrapper(cx, timer)->jsobject));
        return JS_TRUE;
    }
    else
    {
        HLTimer *timer = new HLTimer(0);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(getOrCreateWrapper(cx, timer)->jsobject));
        return JS_TRUE;
    }
}

JSBool JSTimer::jsSetDelegate(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSTimer* thiz = (JSTimer*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "error(JSTimer::jsSetDelegate): timer object is already freed");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        thiz->getNativeObject()->delegate = newDelegate<HLTimer*, float>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        thiz->getNativeObject()->delegate = newDelegate<HLTimer*, float>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    JS_ReportError(cx, "error(JSTimer::jsSetDelegate): wrong arguments");
    return JS_FALSE;
}

JSBool JSTimer::jsCancel(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSTimer* thiz = (JSTimer*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "error(JSTimer::jsCancel): timer object is already freed");
        return JS_FALSE;
    }
    thiz->getNativeObject()->cancel();
    return JS_TRUE;
}

NS_HL_END
