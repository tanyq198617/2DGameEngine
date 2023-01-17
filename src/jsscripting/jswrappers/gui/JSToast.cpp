//
//  JSToast.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSToast.h"
#include "JSView.h"
#include "JSLabel.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSToast, gui::HLToast>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSToast, gui::HLToast>::proto = NULL;

namespace gui
{
JSBool JSToast::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1 && JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
    {
        JSString* arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char * s = JS_EncodeStringToUTF8(cx,arg0);
        HLToast* view = new HLToast(s);
        JS_free(cx, s);
        JSToast* tmp = JSToast::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    JS_ReportError(cx, "hoolai.gui.Toast constructor argument error");
    return JS_FALSE;
}

void JSToast::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        JS_FN("show", JSToast::jsShow, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getLabel", JSToast::jsGetLabel, 1, JSPROP_PERMANENT | JSPROP_READONLY),
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

    proto = JS_InitClass(cx,globalObj,JSLabel::proto,jsclass,JSToast::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}

JSBool JSToast::jsShow(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSToast* jToast = (JSToast*)JS_GetPrivate(obj);
    if (!jToast)
    {
        return JS_FALSE;
    }
    jToast->getNativeObject()->show();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSToast::jsGetLabel(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSToast* jToast = (JSToast*)JS_GetPrivate(obj);
    if (!jToast)
    {
        return JS_FALSE;
    }
    HLRichLabel *p = jToast->getNativeObject()->getLabel();
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(p->getOrCreateJSWrapper(cx)->jsobject));
    return JS_TRUE;
}

}

NS_HL_END
