//
//  JSTexture.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-29.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "JSTexture.h"
#include "JSConversions.h"

NS_HL_BEGIN

template<> JSObject* JSCPPWrapper<JSTexture, HLTexture>::proto = NULL;
template<> JSClass* JSCPPWrapper<JSTexture, HLTexture>::jsclass = NULL;

JSBool JSTexture::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
//    if (argc == 1)
//    {
//        JSString *arg0;
//		JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
//        HLTexture* tex = new HLTexture();
//        tex->loadWithFile(JS_EncodeStringToUTF8(cx, arg0));
//        JSTexture* tmp = JSTexture::getOrCreateWrapper(cx, tex);
//        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
//        return JS_TRUE;
//    }
    JS_ReportError(cx, "JSTexture: wrong constructor");
	return JS_FALSE;
}

JSBool JSTexture::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    
	JSTexture* jtex = (JSTexture*)JS_GetPrivate(obj);
    
	if (!jtex) return JS_TRUE;
    
    switch (propId)
    {
        case kSize:
            val.set(HLSize_to_jsval(cx, jtex->getNativeObject()->getImageSize()));
            break;
        case kScale:
            val.setDouble(jtex->getNativeObject()->getScale());
            break;
        default:
            break;
    }
    return JS_TRUE;
}

//JSBool JSTexture::jsPropertySet(JSContext *cx, JSObject *obj, jsid _id, JSBool strict, jsval *val)
//{
//    return JS_TRUE;
//}

void JSTexture::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        {"size", kSize, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, JSOP_WRAPPER(JSTexture::jsPropertyGet), 0},
        {"scale", kScale, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, JSOP_WRAPPER(JSTexture::jsPropertyGet), 0},
        {0, 0, 0, 0, 0}
    };
    
    static JSFunctionSpec funcs[] = {
        JS_FN("isLoaded", JSTexture::jsIsLoaded, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("retain", JSTexture::jsRetain, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("release", JSTexture::jsRelease, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FN("getTexture", JSTexture::jsGetTexture, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
	proto = JS_InitClass(cx,globalObj,NULL,jsclass,JSTexture::jsConstructor,0,properties,funcs,NULL,st_funcs);
}

JSBool JSTexture::jsGetTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc >= 1)
    {
        JSString *arg0;
		JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        bool ansyc = false;
        if (argc == 2) {
            ansyc = JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[1]);
        }
        HLTexture *tex = HLTexture::getTexture(s, ansyc);
        JS_free(cx, s);
        if (tex)
        {
            JSTexture* tmp = JSTexture::getOrCreateWrapper(cx, tex);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        }
        else
        {
            JS_SET_RVAL(cx, vp, JSVAL_NULL);
        }
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for spriteFrameFromCache");
    return JS_FALSE;
}

JSBool JSTexture::jsIsLoaded(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSTexture* tex = (JSTexture*)JS_GetPrivate(obj);
    if (!tex)
    {
        JS_ReportError(cx, "JSTexture: native object already released");
        return JS_FALSE;
    }
    
    JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(tex->getNativeObject()->getName()!=0));
    
    return JS_TRUE;
}

JSBool JSTexture::jsRetain(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSTexture* tex = (JSTexture*)JS_GetPrivate(obj);
    if (!tex)
    {
        JS_ReportError(cx, "JSTexture: native object already released");
        return JS_FALSE;
    }
    
    tex->getNativeObject()->retain();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSTexture::jsRelease(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSTexture* tex = (JSTexture*)JS_GetPrivate(obj);
    if (!tex)
    {
        JS_ReportError(cx, "JSTexture: native object already released");
        return JS_FALSE;
    }
    
    tex->getNativeObject()->release();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

NS_HL_END
