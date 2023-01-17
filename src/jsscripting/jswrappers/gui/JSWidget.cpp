//
//  JSWidget.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSWidget.h"
#include "JSView.h"
#include "HLWidget.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSWidget, gui::HLWidget>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSWidget, gui::HLWidget>::proto = NULL;

template<> JSClass * JSCPPWrapper<gui::JSRootWidget, gui::HLRootWidget>::jsclass = nullptr;
template<> JSObject * JSCPPWrapper<gui::JSRootWidget, gui::HLRootWidget>::proto = nullptr;

namespace gui
{
    
    JSBool JSWidget::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
    {
        HLWidget* view = new HLWidget();
        JSWidget* tmp = JSWidget::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    void JSWidget::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
            JS_FN("showWidget", JSWidget::jsShowWidget, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("close", JSWidget::jsClose, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("getPresentWidget", JSWidget::jsGetPresentWidget, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setOnCloseListener", JSWidget::jsSetOnCloseListener, 2, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FS_END
        };
        
        static JSPropertySpec st_properties[] = {
            {0, 0, 0, 0, 0}
        };
        
        static JSFunctionSpec st_funcs[] = {
            JS_FS_END
        };
        
        proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSWidget::jsConstructor,0,properties,funcs,st_properties,st_funcs);
    }
    
    JSBool JSWidget::jsShowWidget(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSWidget* thiz = (JSWidget*)JS_GetPrivate(obj);
            if (!thiz)
            {
                return JS_FALSE;
            }
            JSObject *arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);
            JSWidget* jview = (JSWidget*)JS_GetPrivate(arg0);
            if (!jview)
            {
                return JS_FALSE;
            }
            thiz->getNativeObject()->showWidget(jview->getNativeObject());
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        return JS_FALSE;
    }
    JSBool JSWidget::jsClose(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSWidget* thiz = (JSWidget*)JS_GetPrivate(obj);
        if (!thiz)
        {
            return JS_FALSE;
        }
        if (argc == 0)
        {
            thiz->getNativeObject()->close();
            return JS_TRUE;
        }
        else if (argc == 1)
        {
            JSBool arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "b", &arg0);
            thiz->getNativeObject()->close(arg0);
            return JS_TRUE;
        }
        return JS_FALSE;
    }
    JSBool JSWidget::jsGetPresentWidget(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSWidget* thiz = (JSWidget*)JS_GetPrivate(obj);
        if (!thiz)
        {
            return JS_FALSE;
        }
        HLWidget* res = thiz->getNativeObject()->getPresentWidget();
        if (res)
        {
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(getOrCreateWrapper(cx, res)->jsobject));
        }
        else
        {
            JS_SET_RVAL(cx, vp, JSVAL_NULL);
        }
        return JS_TRUE;
    }
    
    JSBool JSWidget::jsSetOnCloseListener(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSWidget* thiz = (JSWidget*)JS_GetPrivate(obj);
        if (argc == 1)
        {
            thiz->getNativeObject()->onClose += newDelegate<HLWidget*, bool>(obj, JS_ARGV(cx, vp)[0]);
            return JS_TRUE;
        }
        else if (argc == 2)
        {
            thiz->getNativeObject()->onClose += newDelegate<HLWidget*, bool>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
            return JS_TRUE;
        }
        return JS_FALSE;
    }
    
    void JSRootWidget::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
            JS_FN("getTopWidget", JSRootWidget::jsGetTopWidget, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("getMaskWidget", JSRootWidget::jsGetMaskWidget, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("showMaskWidget", JSRootWidget::jsShowMaskWidget, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("dismissMaskWidget", JSRootWidget::jsDismissMaskWidget, 2, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("showHUDWidget", JSRootWidget::jsShowHUDWidget, 2, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("dismissHUDWidget", JSRootWidget::jsDismissHUDWidget, 2, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FS_END
        };
        
        static JSPropertySpec st_properties[] = {
            {0, 0, 0, 0, 0}
        };
        
        static JSFunctionSpec st_funcs[] = {
            JS_FS_END
        };
        
        proto = JS_InitClass(cx,globalObj,JSWidget::proto,jsclass,jsNoConstructor,0,properties,funcs,st_properties,st_funcs);
    }
    
    JSBool JSRootWidget::jsGetTopWidget(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSRootWidget* thiz = (JSRootWidget*)JS_GetPrivate(obj);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(thiz->getNativeObject()->getTopWidget()->getOrCreateJSWrapper(cx)->jsobject));
        return JS_TRUE;
    }
    JSBool JSRootWidget::jsGetMaskWidget(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSRootWidget* thiz = (JSRootWidget*)JS_GetPrivate(obj);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(thiz->getNativeObject()->getMaskWidget()->getOrCreateJSWrapper(cx)->jsobject));
        return JS_TRUE;
    }
    JSBool JSRootWidget::jsShowMaskWidget(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSRootWidget* thiz = (JSRootWidget*)JS_GetPrivate(obj);
        
        if (argc == 1)
        {
            JSObject *arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);
            JSWidget* jview = (JSWidget*)JS_GetPrivate(arg0);
            if (!jview)
            {
                return JS_FALSE;
            }
            thiz->getNativeObject()->showMaskWidget(jview->getNativeObject());
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    JSBool JSRootWidget::jsDismissMaskWidget(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSRootWidget* thiz = (JSRootWidget*)JS_GetPrivate(obj);
        thiz->getNativeObject()->dismissMaskWidget();
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    JSBool JSRootWidget::jsShowHUDWidget(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSRootWidget* thiz = (JSRootWidget*)JS_GetPrivate(obj);
        if (argc == 1)
        {
            JSObject *arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);
            JSWidget* jview = (JSWidget*)JS_GetPrivate(arg0);
            if (!jview)
            {
                return JS_FALSE;
            }
            thiz->getNativeObject()->showHUDWidget(jview->getNativeObject());
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    JSBool JSRootWidget::jsDismissHUDWidget(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSRootWidget* thiz = (JSRootWidget*)JS_GetPrivate(obj);
        thiz->getNativeObject()->dismissHUDWidget();
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
}

NS_HL_END
