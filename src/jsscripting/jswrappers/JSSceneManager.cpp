//
//  JSSceneManager.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/3/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "JSSceneManager.h"
#include "JSEntitySystem.h"

NS_HL_BEGIN

extern std::map<void *, void *> jsobjmap;

template<> JSClass* JSCPPWrapper<JSSceneManager, HLSceneManager>::jsclass = NULL;
template<> JSObject* JSCPPWrapper<JSSceneManager, HLSceneManager>::proto = NULL;

void JSSceneManager::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
	jsclass->finalize = NULL;
	jsclass->flags = JSCLASS_HAS_PRIVATE;
    
    static JSPropertySpec properties[] = {
        {0, 0, 0, 0, 0}
    };
    
    static JSFunctionSpec funcs[] = {
        JS_FN("getEntityManager", JSSceneManager::jsGetEntityManager, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("addEntity", JSSceneManager::jsAddEntity, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("addOnActiveListener", JSSceneManager::jsAddOnActiveListener, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("addOnDeactiveListener", JSSceneManager::jsAddOnDeactiveListener, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("addOnDestroyListener", JSSceneManager::jsAddOnDestroyListener, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getActiveCamera", JSSceneManager::jsGetActiveCamera, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("removeOnActiveListener", JSSceneManager::jsRemoveOnActiveListener, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("removeOnDeactiveListener", JSSceneManager::jsRemoveOnDeactiveListener, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("removeOnDestroyListener", JSSceneManager::jsRemoveOnDestroyListener, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FS_END
    };
    
    proto = JS_InitClass(cx,globalObj,NULL,jsclass,JSSceneManager::jsConstructor,0,properties,funcs,NULL,st_funcs);
}

JSBool JSSceneManager::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    HLSceneManager* mgr = new HLSceneManager();
    JSSceneManager* tmp = JSSceneManager::getOrCreateWrapper(cx, mgr);
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
    return JS_TRUE;
}

JSBool JSSceneManager::jsGetEntityManager(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSSceneManager* thiz = (JSSceneManager*)JS_GetPrivate(obj);
    if (!thiz)
    {
        return JS_FALSE;
    }
    JSEntityManager *entMgr = JSEntityManager::getOrCreateWrapper(cx, thiz->getNativeObject()->getEntityManager());
    
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(entMgr->jsobject));
    return JS_TRUE;
}

JSBool JSSceneManager::jsAddEntity(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSSceneManager* thiz = (JSSceneManager*)JS_GetPrivate(obj);
        if (!thiz)
        {
            return JS_FALSE;
        }
        JSObject* arg = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSEntity* jsentity = (JSEntity*)JS_GetPrivate(arg);
        if (!jsentity)
        {
            return JS_FALSE;
        }
        thiz->getNativeObject()->addEntity(jsentity->getNativeObject());
        return JS_TRUE;
    }
    JS_ReportError(cx, "JSSceneManager::jsAddEntity: wrong arguments");
    return JS_FALSE;
}

JSBool JSSceneManager::jsAddOnActiveListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSSceneManager* thiz = (JSSceneManager*)JS_GetPrivate(obj);
    if (argc == 1)
    {
        thiz->getNativeObject()->onActive += newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        thiz->getNativeObject()->onActive += newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSSceneManager::jsAddOnDeactiveListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSSceneManager* thiz = (JSSceneManager*)JS_GetPrivate(obj);
    if (argc == 1)
    {
        thiz->getNativeObject()->onDeactive += newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        thiz->getNativeObject()->onDeactive += newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSSceneManager::jsGetActiveCamera(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSSceneManager* thiz = (JSSceneManager*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "JSSceneManager::jsGetActiveCamera: native object already disposed");
        return JS_FALSE;
    }
    HLEntity* camera = thiz->getNativeObject()->getActiveCamera();
    if (camera)
    {
        jsval val = OBJECT_TO_JSVAL(JSEntity::getOrCreateWrapper(cx, camera)->jsobject);
        JS_SET_RVAL(cx, vp, val);
    }
    else
    {
        JS_SET_RVAL(cx, vp, JSVAL_NULL);
    }
    
    return JS_TRUE;
}

JSBool JSSceneManager::jsAddOnDestroyListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSSceneManager* thiz = (JSSceneManager*)JS_GetPrivate(obj);
    if (argc == 1)
    {
        thiz->getNativeObject()->onDestroy += newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        thiz->getNativeObject()->onDestroy += newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSSceneManager::jsRemoveOnActiveListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSSceneManager* thiz = (JSSceneManager*)JS_GetPrivate(obj);
    if (argc == 1)
    {
        thiz->getNativeObject()->onActive -= newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        thiz->getNativeObject()->onActive -= newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSSceneManager::jsRemoveOnDeactiveListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSSceneManager* thiz = (JSSceneManager*)JS_GetPrivate(obj);
    if (argc == 1)
    {
        thiz->getNativeObject()->onDeactive -= newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        thiz->getNativeObject()->onDeactive -= newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSSceneManager::jsRemoveOnDestroyListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSSceneManager* thiz = (JSSceneManager*)JS_GetPrivate(obj);
    if (argc == 1)
    {
        thiz->getNativeObject()->onDestroy -= newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        thiz->getNativeObject()->onDestroy -= newDelegate<HLSceneManager*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

NS_HL_END
