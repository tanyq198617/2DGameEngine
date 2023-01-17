//
//  JSAnimationSystem.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-5-11.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSAnimationSystem.h"
#include "HLAnimateSystem.h"
#include "JSSpriteFrame.h"
#include "JSEntitySystem.h"

NS_HL_BEGIN

template<> JSObject* JSCPPWrapper<JSSpriteFrameAnimation, HLSpriteFrameAnimation>::proto = NULL;
template<> JSClass* JSCPPWrapper<JSSpriteFrameAnimation, HLSpriteFrameAnimation>::jsclass = NULL;

template<> JSObject* JSCPPWrapper<JSRepeateAnimation, HLRepeateAnimation>::proto = NULL;
template<> JSClass* JSCPPWrapper<JSRepeateAnimation, HLRepeateAnimation>::jsclass = NULL;

JSBool JSSpriteFrameAnimation::jsAnimation(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc >= 2)
    {
        jsval arg1 = JS_ARGV(cx, vp)[0];
        float dur = 0;
        if (JSVAL_IS_INT(arg1))
        {
            dur = JSVAL_TO_INT(arg1);
        }
        else
        {
            dur = JSVAL_TO_DOUBLE(arg1);
        }
        JSObject* arg2 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]);
        if (!JS_IsArrayObject(cx, arg2))
        {
            JS_ReportError(cx, "spriteframeanimation constructor param error\n");
            return JS_FALSE;
        }
        bool restore = false;
        if (argc >= 3)
        {
            restore = JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[2]);
        }
        uint32_t len;
        JS_GetArrayLength(cx, arg2, &len);
        HLArray* arr = new HLArray();
        for (int i = 0; i < len; ++i)
        {
            JS::RootedValue v(cx);
            JS_GetElement(cx, arg2, i, &v);
            JSObject* ob = JSVAL_TO_OBJECT(v);
            JSSpriteFrame* jspf = (JSSpriteFrame*)JS_GetPrivate(ob);
            arr->addObject(jspf->getNativeObject());
        }
        HLSpriteFrameAnimation* anim = HLSpriteFrameAnimation::animation(dur, arr, restore);
        arr->release();
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JSSpriteFrameAnimation::getOrCreateWrapper(cx, anim)->jsobject));
        return JS_TRUE;
    }
    JS_ReportError(cx, "spriteframeanimation constructor param error\n");
    return JS_FALSE;
}

void JSSpriteFrameAnimation::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        JS_FN("run", JSSpriteFrameAnimation::jsRun, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setCallback", JSSpriteFrameAnimation::jsSetCallback, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FN("animation", JSSpriteFrameAnimation::jsAnimation, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
	proto = JS_InitClass(cx,globalObj,NULL,jsclass,jsNoConstructor,0,properties,funcs,NULL,st_funcs);
}

JSBool JSSpriteFrameAnimation::jsRun(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc >= 1)
    {
        JSObject* tobj = JS_THIS_OBJECT(cx, vp);
        JSSpriteFrameAnimation* thiz = (JSSpriteFrameAnimation*)JS_GetPrivate(tobj);
        if (!thiz->getNativeObject())
        {
            JS_ReportError(cx, "JSRepeateAnimation::jsRun: this is already freed\n");
            return JS_FALSE;
        }
        JSObject* obj = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSEntity* je = (JSEntity*)JS_GetPrivate(obj);
        if (!je->getNativeObject())
        {
            JS_ReportError(cx, "JSRepeateAnimation::jsRun: entity is null\n");
            return JS_FALSE;
        }
        thiz->getNativeObject()->run(je->getNativeObject());
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSSpriteFrameAnimation::jsSetCallback(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* tobj = JS_THIS_OBJECT(cx, vp);
    JSSpriteFrameAnimation* thiz = (JSSpriteFrameAnimation*)JS_GetPrivate(tobj);
    if (!thiz->getNativeObject())
    {
        JS_ReportError(cx, "JSRepeateAnimation::jsRun: this is already freed\n");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        thiz->getNativeObject()->_callback = newDelegate(tobj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        thiz->getNativeObject()->_callback = newDelegate(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
};

JSBool JSRepeateAnimation::jsAnimation(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        JSObject* arg1 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSCppWrapperBase* wrapper = (JSCppWrapperBase*)JS_GetPrivate(arg1);
        HLAnimation *anim = (HLAnimation*)wrapper->object;
        int arg2 = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
        HLRepeateAnimation* ranim = HLRepeateAnimation::animation(anim, arg2);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JSRepeateAnimation::getOrCreateWrapper(cx, ranim)->jsobject));
        return JS_TRUE;
    }
    JS_ReportError(cx, "repeateanimation constructor param error\n");
    return JS_FALSE;
}

void JSRepeateAnimation::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        JS_FN("run", JSRepeateAnimation::jsRun, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setCallback", JSSpriteFrameAnimation::jsSetCallback, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FN("animation", JSRepeateAnimation::jsAnimation, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
	proto = JS_InitClass(cx,globalObj,NULL,jsclass,jsNoConstructor,0,properties,funcs,NULL,st_funcs);
}

JSBool JSRepeateAnimation::jsRun(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc >= 1)
    {
        JSObject* tobj = JS_THIS_OBJECT(cx, vp);
        JSRepeateAnimation* thiz = (JSRepeateAnimation*)JS_GetPrivate(tobj);
        if (!thiz->getNativeObject())
        {
            JS_ReportError(cx, "JSRepeateAnimation::jsRun: this is already freed\n");
            return JS_FALSE;
        }
        JSObject* obj = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSEntity* je = (JSEntity*)JS_GetPrivate(obj);
        if (!je->getNativeObject())
        {
            JS_ReportError(cx, "JSRepeateAnimation::jsRun: entity is null\n");
            return JS_FALSE;
        }
        thiz->getNativeObject()->run(je->getNativeObject());
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSRepeateAnimation::jsSetCallback(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* tobj = JS_THIS_OBJECT(cx, vp);
    JSRepeateAnimation* thiz = (JSRepeateAnimation*)JS_GetPrivate(tobj);
    if (!thiz->getNativeObject())
    {
        JS_ReportError(cx, "JSRepeateAnimation::jsRun: this is already freed\n");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        thiz->getNativeObject()->_callback = newDelegate(tobj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        thiz->getNativeObject()->_callback = newDelegate(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
};

NS_HL_END
