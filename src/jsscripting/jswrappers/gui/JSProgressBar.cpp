//
//  JSProgressBar.cpp
//  HoolaiEngine
//
//  Created by cai wei on 23-4-13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "JSProgressBar.h"
#include "JSView.h"
#include "JSTexture.h"
#include "JSSpriteFrame.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSProgressBar, gui::HLProgressBar>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSProgressBar, gui::HLProgressBar>::proto = NULL;

namespace gui
{
JSBool JSProgressBar::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSProgressBar* jview = (JSProgressBar*)JS_GetPrivate(obj);
    if (!jview)
    {
        JS_ReportError(cx, "JSProgressBar::jsPropertyGet native object already disposed");
        return JS_FALSE;
    }
    HLProgressBar* view = jview->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
    case kValue:
    {
        val.setInt32(view->getValue());
    }
    break;
    case kValue1:
    {
        val.setInt32(view->getValue1());
    }
    break;
    default:
        break;
    }
    return JS_TRUE;
}

JSBool JSProgressBar::jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSProgressBar* jview = (JSProgressBar*)JS_GetPrivate(obj);
    if (!jview)
    {
        JS_ReportError(cx, "JSProgressBar::jsPropertySet native object already disposed");
        return JS_FALSE;
    }
    HLProgressBar* view = jview->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
    case kValue:
    {
        view->setValue(val.toInt32());
    }
    break;
    case kValue1:
    {
        view->setValue1(val.toInt32());
    }
    break;
    default:
        break;
    }
    return JS_TRUE;
}

JSBool JSProgressBar::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc >= 2)
    {
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect frame = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSCppWrapperBase* parent = (JSCppWrapperBase*)JS_GetPrivate(arg0);
        if (!parent)
        {
            JS_ReportError(cx, "JSProgressBar::jsConstructor parent native object already disposed");
            return JS_FALSE;
        }
        if(argc == 2)
        {
            HLProgressBar* view = new HLProgressBar((HLView*)parent->object, frame);
            JSProgressBar* tmp = JSProgressBar::getOrCreateWrapper(cx, view);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
            return JS_TRUE;
        }
        else if(argc == 3)
        {
            HLProgressBar* view = new HLProgressBar((HLView*)parent->object, frame,
                JSVAL_TO_INT(JS_ARGV(cx, vp)[2]));
            JSProgressBar* tmp = JSProgressBar::getOrCreateWrapper(cx, view);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
            return JS_TRUE;
        }
    }
    return JS_TRUE;
}

void JSProgressBar::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        {"value", kValue, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSProgressBar::jsPropertyGet), JSOP_WRAPPER(JSProgressBar::jsPropertySet)},
        {"value1", kValue1, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSProgressBar::jsPropertyGet), JSOP_WRAPPER(JSProgressBar::jsPropertySet)},
        {0, 0, 0, 0, 0}
    };

    static JSFunctionSpec funcs[] =
    {
        JS_FN("setRange", JSProgressBar::jsSetRange, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setBackgroundImage", JSProgressBar::jsSetBackgroundImage, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setBackgroundImageTexture", JSProgressBar::jsSetBackgroundImageTexture, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setBackgroundImageSpriteFrame", JSProgressBar::jsSetBackgroundImageSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setForegroundImage", JSProgressBar::jsSetForegroundImage, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setForegroundImageTexture", JSProgressBar::jsSetForegroundImageTexture, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setForegroundImageSpriteFrame", JSProgressBar::jsSetForegroundImageSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setRepeatForegroundImage", JSProgressBar::jsSetRepeatForegroundImage, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setRepeatForegroundImageTexture", JSProgressBar::jsSetRepeatForegroundImageTexture, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setRepeatForegroundImageSpriteFrame", JSProgressBar::jsSetRepeatForegroundImageSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
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

    proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSProgressBar::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}

JSBool JSProgressBar::jsSetRange(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetRange native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setRange(JSVAL_TO_INT(JS_ARGV(cx, vp)[0]), JSVAL_TO_INT(JS_ARGV(cx, vp)[1]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSProgressBar::jsSetBackgroundImage(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
    if (!jProgressBar)
    {
        JS_ReportError(cx, "JSProgressBar::jsSetBackgroundImage native object already disposed");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        jProgressBar->getNativeObject()->setBackgroundImage(HLTexture::getTexture(narg0));
        JS_free(cx, narg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
    }
    else if (argc == 2)
    {
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        jProgressBar->getNativeObject()->setBackgroundImage(HLTexture::getTexture(narg0), rect);
        JS_free(cx, narg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
    }
    
    return JS_TRUE;
}

JSBool JSProgressBar::jsSetBackgroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetBackgroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetBackgroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setBackgroundImage(jTexture->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetBackgroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetBackgroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setBackgroundImage(jTexture->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSProgressBar::jsSetBackgroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetBackgroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetBackgroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setBackgroundImage(jSpriteFrame->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetBackgroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetBackgroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setBackgroundImage(jSpriteFrame->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSProgressBar::jsSetForegroundImage(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
    if (!jProgressBar)
    {
        JS_ReportError(cx, "JSProgressBar::jsSetForegroundImage native object already disposed");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        jProgressBar->getNativeObject()->setForegroundImage(HLTexture::getTexture(narg0));
        JS_free(cx, narg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
    }
    else if (argc == 2)
    {
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        jProgressBar->getNativeObject()->setForegroundImage(HLTexture::getTexture(narg0), rect);
        JS_free(cx, narg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
    }
    
    return JS_TRUE;
}

JSBool JSProgressBar::jsSetForegroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetForegroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetForegroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setForegroundImage(jTexture->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetForegroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetForegroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setForegroundImage(jTexture->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSProgressBar::jsSetForegroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetForegroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetForegroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setForegroundImage(jSpriteFrame->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetForegroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetForegroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setForegroundImage(jSpriteFrame->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSProgressBar::jsSetRepeatForegroundImage(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
    if (!jProgressBar)
    {
        JS_ReportError(cx, "JSProgressBar::jsSetRepeatForegroundImage native object already disposed");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        jProgressBar->getNativeObject()->setRepeatForegroundImage(HLTexture::getTexture(narg0));
        JS_free(cx, narg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
    }
    else if (argc == 2)
    {
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        jProgressBar->getNativeObject()->setRepeatForegroundImage(HLTexture::getTexture(narg0), rect);
        JS_free(cx, narg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
    }
    
    return JS_TRUE;
}

JSBool JSProgressBar::jsSetRepeatForegroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetRepeatForegroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetRepeatForegroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setRepeatForegroundImage(jTexture->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetRepeatForegroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetRepeatForegroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setRepeatForegroundImage(jTexture->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSProgressBar::jsSetRepeatForegroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetRepeatForegroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetRepeatForegroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setRepeatForegroundImage(jSpriteFrame->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressBar* jProgressBar = (JSProgressBar*)JS_GetPrivate(obj);
        if (!jProgressBar)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetRepeatForegroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSProgressBar::jsSetRepeatForegroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jProgressBar->getNativeObject()->setRepeatForegroundImage(jSpriteFrame->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}
}

NS_HL_END
