//
//  JSToolTip.cpp
//  HoolaiEngine
//
//  Created by cai wei on 23-4-13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "JSToolTip.h"
#include "JSView.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSToolTip, gui::HLToolTip>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSToolTip, gui::HLToolTip>::proto = NULL;

namespace gui
{
JSBool JSToolTip::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        HLRect frame = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[0]);
        HLToolTip* view = new HLToolTip(frame);
        JSToolTip* tmp = JSToolTip::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    return JS_TRUE;
}

void JSToolTip::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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

    static JSPropertySpec properties[] =
    {
        {0, 0, 0, 0, 0}
    };

    static JSFunctionSpec funcs[] =
    {
//        JS_FN("setEventDispatchRect", JSToolTip::jsSetEventDispatchRect, 1, JSPROP_PERMANENT | JSPROP_SHARED),
        JS_FN("show", JSToolTip::jsShow, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("dismiss", JSToolTip::jsDismiss, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setOnDismissListener", JSToolTip::jsSetOnDismissListener, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };

    static JSPropertySpec st_properties[] =
    {
        {0, 0, 0, 0, 0}
    };

    static JSFunctionSpec st_funcs[] =
    {
        JS_FS_END
    };

    proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSToolTip::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}


//JSBool JSToolTip::jsSetEventDispatchRect(JSContext *cx, uint32_t argc, jsval *vp)
//{
//    if(argc == 1)
//    {
//        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
//        JSToolTip* jToolTip = (JSToolTip*)JS_GetPrivate(obj);
//        if (!jToolTip)
//        {
//            return JS_FALSE;
//        }
//        jToolTip->getNativeObject()->setEventDispatchRect(jsval_to_HLRect(cx, JS_ARGV(cx,vp)[0]));
//        JS_SET_RVAL(cx, vp, JSVAL_VOID);
//    }
//    return JS_TRUE;
//}

JSBool JSToolTip::jsShow(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSToolTip* jToolTip = (JSToolTip*)JS_GetPrivate(obj);
    if (!jToolTip)
    {
        return JS_FALSE;
    }
    jToolTip->getNativeObject()->show();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSToolTip::jsDismiss(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSToolTip* jToolTip = (JSToolTip*)JS_GetPrivate(obj);
    if (!jToolTip)
    {
        return JS_FALSE;
    }
    jToolTip->getNativeObject()->dismiss();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSToolTip::jsSetOnDismissListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSToolTip* jToolTip = (JSToolTip*)JS_GetPrivate(obj);
    if (argc == 1)
    {
        jToolTip->getNativeObject()->onDismiss = newDelegate<HLToolTip*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        jToolTip->getNativeObject()->onDismiss = newDelegate<HLToolTip*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}
}

NS_HL_END