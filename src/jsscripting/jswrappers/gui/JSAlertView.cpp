//
//  JSAlertView.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSAlertView.h"
#include "JSView.h"
#include "JSButton.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSAlertView, gui::HLAlertView>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSAlertView, gui::HLAlertView>::proto = NULL;

namespace gui
{
JSBool JSAlertView::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc >= 1)
    {
        if (JS_ARGV(cx, vp)[0].isObject())
        {
            bool res;
            JS_HasInstance(cx, JSView::proto, JS_ARGV(cx, vp)[0], &res);
            if (res)
            {
                HLButton** btnList = (HLButton**)malloc((argc)*sizeof(HLButton*));
                for (uint32_t i = 1; i < argc; ++i)
                {
                    JS_HasInstance(cx, JSButton::proto, JS_ARGV(cx, vp)[i], &res);
                    if (!res)
                    {
                        JS_ReportError(cx, "JSAlertView wrong arguments passed to the constructor");
                        free(btnList);
                        return JS_FALSE;
                    }
                    btnList[i-1] = ((JSButton*)JS_GetPrivate(JS_ARGV(cx, vp)[i].toObjectOrNull()))->getNativeObject();
                }
                btnList[argc-1] = 0;
                HLView* contentView = ((JSView*)JS_GetPrivate(JS_ARGV(cx, vp)[0].toObjectOrNull()))->getNativeObject();
                HLAlertView* view = HLAlertView::newAlert(contentView, btnList);
                JSAlertView* tmp = JSAlertView::getOrCreateWrapper(cx, view);
                JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
                free(btnList);
                return JS_TRUE;
            }
        }
    }
    if(argc >= 3)
    {
        char** pStrList = NULL;
        pStrList=new char* [argc+1];
        if(NULL == pStrList)
        {
            return JS_FALSE;
        }
        for(uint32_t i = 0; i<argc; i++)
        {
            JSString* arg = JSVAL_TO_STRING(JS_ARGV(cx, vp)[i]);
            std::string str = JS_EncodeStringToUTF8(cx,arg);
            int len = strlen(str.c_str());
            pStrList[i]=(char*)calloc((len+1),sizeof(char));
            strcpy(pStrList[i],str.c_str());
        }
        pStrList[argc] = NULL;
        HLAlertView* view = HLAlertView::newAlert(pStrList[0],pStrList[1],pStrList[2],&pStrList[3]);
        JSAlertView* tmp = JSAlertView::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        for(uint32_t i = 0; i<argc; i++)
        {
            if(pStrList[i] != NULL)
            {
                free(pStrList[i]);
                pStrList[i] = NULL;
            }
        }
        if(pStrList != NULL)
        {
            delete pStrList;
            pStrList = NULL;
        }
        return JS_TRUE;
    }

    JS_ReportError(cx, "JSAlertView wrong arguments passed to the constructor");
    return JS_FALSE;
}

void JSAlertView::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        JS_FN("show", JSAlertView::jsShow, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("dismiss", JSAlertView::jsDismiss, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setOnCancelClickListener", JSAlertView::jsSetOnCancelClickListener, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setOnButtonClickListener", JSAlertView::jsSetOnButtonClickListener, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setOnDismissListener", JSAlertView::jsSetOnDismissListener, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };

    static JSPropertySpec st_properties[] =
    {
        {0, 0, 0, 0, 0}
    };

    static JSFunctionSpec st_funcs[] =
    {
        JS_FN("setDarkTheme", JSAlertView::jsSetDarkTheme, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };

    proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSAlertView::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}

JSBool JSAlertView::jsShow(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSAlertView* jAlertView = (JSAlertView*)JS_GetPrivate(obj);
    if (!jAlertView)
    {
        return JS_FALSE;
    }
    jAlertView->getNativeObject()->show();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}
    
JSBool JSAlertView::jsDismiss(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSAlertView* jAlertView = (JSAlertView*)JS_GetPrivate(obj);
    if (!jAlertView)
    {
        return JS_FALSE;
    }
    jAlertView->getNativeObject()->dismiss();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSAlertView::jsSetOnCancelClickListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSAlertView* jAlertView = (JSAlertView*)JS_GetPrivate(obj);
    if(!jAlertView)
    {
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jAlertView->getNativeObject()->onCancel = newDelegate<HLAlertView*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        jAlertView->getNativeObject()->onCancel = newDelegate<HLAlertView*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSAlertView::jsSetOnButtonClickListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSAlertView* jAlertView = (JSAlertView*)JS_GetPrivate(obj);
    if(!jAlertView)
    {
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jAlertView->getNativeObject()->onButtonClicked = newDelegate<HLAlertView*, int>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        jAlertView->getNativeObject()->onButtonClicked = newDelegate<HLAlertView*, int>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}
    
JSBool JSAlertView::jsSetOnDismissListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSAlertView* jAlertView = (JSAlertView*)JS_GetPrivate(obj);
    if(!jAlertView)
    {
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jAlertView->getNativeObject()->onDismiss = newDelegate<HLAlertView*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        jAlertView->getNativeObject()->onDismiss = newDelegate<HLAlertView*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

/*JSBool JSAlertView::jsGetFontSize(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSAlertView* jAlertView = (JSAlertView*)JS_GetPrivate(obj);
    if (!jAlertView)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jAlertView->getNativeObject()->onButtonClicked));
    return JS_TRUE;
}

static JSBool jsGetButtonClickedId(JSContext *cx, uint32_t argc, jsval *vp);*/
    
JSBool JSAlertView::jsSetDarkTheme(JSContext *cx, uint32_t argc, jsval *vp)
{
    HLAlertView::setDarkTheme();
    return JS_TRUE;
}
}

NS_HL_END
