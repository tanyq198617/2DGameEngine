//
//  JSSceneView.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-6-5.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSSceneView.h"
#include "JSSceneManager.h"
#include "HLSceneManager.h"
#include "JSView.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSSceneView, gui::HLSceneView>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSSceneView, gui::HLSceneView>::proto = NULL;

namespace gui
{
    
JSBool JSSceneView::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 0)
    {
        HLSceneView* view = new HLSceneView(NULL);
        JSSceneView* tmp = JSSceneView::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        view->setSceneManager(new HLSceneManager());
        return JS_TRUE;
    }
    if (argc == 1)
    {
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSCppWrapperBase* parent = (JSCppWrapperBase*)JS_GetPrivate(arg0);
        if (!parent)
        {
            return JS_FALSE;
        }
        HLSceneView* view = new HLSceneView((HLView*)parent->object);
        JSSceneView* tmp = JSSceneView::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        view->setSceneManager(new HLSceneManager());
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect frame = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSCppWrapperBase* parent = (JSCppWrapperBase*)JS_GetPrivate(arg0);
        if (!parent)
        {
            return JS_FALSE;
        }
        HLSceneView* view = new HLSceneView((HLView*)parent->object, frame);
        JSSceneView* tmp = JSSceneView::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        view->setSceneManager(new HLSceneManager());
        return JS_TRUE;
    }
    return JS_TRUE;
}

void JSSceneView::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        JS_FN("getSceneManager", JSSceneView::jsGetSceneManager, 0, JSPROP_PERMANENT | JSPROP_READONLY),
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
    
    proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSSceneView::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}

JSBool JSSceneView::jsGetSceneManager(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSSceneView* jSceneView = (JSSceneView*)JS_GetPrivate(obj);
    if (!jSceneView)
    {
        return JS_FALSE;
    }
    if (!jSceneView->getNativeObject())
    {
        return JS_FALSE;
    }
    HLSceneManager* sceneMgr = jSceneView->getNativeObject()->getSceneManager();
    JSObject* jscene = JSSceneManager::getOrCreateWrapper(cx, sceneMgr)->jsobject;
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jscene));
    return JS_TRUE;
}
    
}

NS_HL_END
