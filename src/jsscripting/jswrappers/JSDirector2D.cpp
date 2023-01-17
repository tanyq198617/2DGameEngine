//
//  JSDirector2D.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-29.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "JSDirector2D.h"
#include "JSConversions.h"

#if GAME_PROJECTION == PROJECTION_2D
NS_HL_BEGIN

extern std::map<void *, void *> jsobjmap;

JSBool JSDirector2D::jsGetCurrentSceneManager(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 0)
    {
        HLSceneManager* sceneMgr = HLDirector2D::getSingleton()->getCurrentSceneManager();
        if (sceneMgr)
        {
            JSSceneManager* jsSceneMgr = JSSceneManager::getOrCreateWrapper(cx, sceneMgr);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsSceneMgr->jsobject));
        }
        else
        {
            JS_SET_RVAL(cx, vp, JSVAL_NULL);
        }
        return JS_TRUE;
    }
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    return JS_TRUE;
}

void JSDirector2D::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
{
    JSObject* obj = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue val(cx, OBJECT_TO_JSVAL(obj));
    JS_SetProperty(cx, globalObj, name, val);
    
    JS_DefineFunction(cx, obj, "getCurrentSceneManager", JSDirector2D::jsGetCurrentSceneManager, 0, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "getWinSize", JSDirector2D::jsGetWinSize, 0, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "convertToGLPoint", JSDirector2D::jsConvertToGLPoint, 1, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "convertToScreenPoint", JSDirector2D::jsConvertToScreenPoint, 1, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "runWithSceneManager", JSDirector2D::jsRunWithSceneManager, 1, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "replaceSceneManager", JSDirector2D::jsReplaceSceneManager, 1, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "pushSceneManager", JSDirector2D::jsPushSceneManager, 1, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "popSceneManager", JSDirector2D::jsPopSceneManager, 1, JSPROP_READONLY | JSPROP_READONLY);
    
//    jsClass = (JSClass *)calloc(1, sizeof(JSClass));
//    jsClass->name = name;
//	jsClass->addProperty = JS_PropertyStub;
//	jsclass->delProperty = JS_DeletePropertyStub;
//	jsClass->getProperty = JS_PropertyStub;
//	jsClass->setProperty = JS_StrictPropertyStub;
//	jsClass->enumerate = JS_EnumerateStub;
//	jsClass->resolve = JS_ResolveStub;
//	jsClass->convert = JS_ConvertStub;
//	jsClass->finalize = NULL;
//	jsClass->flags = JSCLASS_HAS_PRIVATE;
//    
//    static JSPropertySpec properties[] = {
//        {0, 0, 0, 0, 0}
//    };
//    
//    static JSFunctionSpec funcs[] = {
//        JS_FS_END
//    };
//    
//    static JSFunctionSpec st_funcs[] = {
//        JS_FN("currentSceneManager", JSDirector2D::jsCurrentSceneManager, 0, JSPROP_PERMANENT | JSPROP_SHARED),
//        JS_FN("getWinSize", JSDirector2D::jsGetWinSize, 0, JSPROP_PERMANENT | JSPROP_SHARED),
//        JS_FN("convertToGLPoint", JSDirector2D::jsConvertToGLPoint, 1, JSPROP_PERMANENT | JSPROP_SHARED),
//        JS_FN("convertToScreenpoint", JSDirector2D::jsConvertToScreenPoint, 1, JSPROP_PERMANENT | JSPROP_SHARED),
//        JS_FS_END
//    };
//    
//    jsObject = JS_InitClass(cx,globalObj,NULL,jsClass,jsNoConstructor,0,properties,funcs,NULL,st_funcs);
}

JSBool JSDirector2D::jsGetWinSize(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 0)
    {
        HLSize size = HLDirector2D::getSingleton()->getWinSize();
        JS_SET_RVAL(cx, vp, HLSize_to_jsval(cx, size));
        return JS_TRUE;
    }
    JS_SET_RVAL(cx, vp, JSVAL_NULL);
    return JS_FALSE;
}

JSBool JSDirector2D::jsConvertToGLPoint(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        HLPoint p = jsval_to_HLPoint(cx, JS_ARGV(cx, vp)[0]);
        p = HLDirector2D::getSingleton()->convertToGLPoint(p);
        JS_SET_RVAL(cx, vp, HLPoint_to_jsval(cx, p));
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSDirector2D::jsConvertToScreenPoint(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        HLPoint p = jsval_to_HLPoint(cx, JS_ARGV(cx, vp)[0]);
        p = HLDirector2D::getSingleton()->convertToScreenPoint(p);
        JS_SET_RVAL(cx, vp, HLPoint_to_jsval(cx, p));
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSDirector2D::jsRunWithSceneManager(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        // TODO check argument
        JSSceneManager* sceneMgr = (JSSceneManager*)JS_GetPrivate(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));
        HLDirector2D::getSingleton()->runWithSceneManager(sceneMgr->getNativeObject());
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSDirector2D::jsReplaceSceneManager(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSSceneManager* sceneMgr = (JSSceneManager*)JS_GetPrivate(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));
        HLDirector2D::getSingleton()->replaceSceneManager(sceneMgr->getNativeObject());
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSDirector2D::jsPushSceneManager(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSSceneManager* sceneMgr = (JSSceneManager*)JS_GetPrivate(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));
        HLDirector2D::getSingleton()->pushSceneManager(sceneMgr->getNativeObject());
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSDirector2D::jsPopSceneManager(JSContext *cx, uint32_t argc, jsval *vp)
{
    HLDirector2D::getSingleton()->popSceneManager();
    return JS_TRUE;
}

NS_HL_END
#else

#endif
