//
//  JSRichLabel.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-7-30.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSRichLabel.h"
#include "HLRichLabel.h"
#include "JSView.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSRichLabel, gui::HLRichLabel>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSRichLabel, gui::HLRichLabel>::proto = NULL;

namespace gui
{

JSBool JSRichLabel::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSRichLabel* jview = (JSRichLabel*)JS_GetPrivate(obj);
    if (!jview)
    {
        JS_ReportError(cx, "JSRichLabel::jsPropertyGet native object already disposed");
        return JS_FALSE;
    }
    HLRichLabel* view = jview->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
        case kTextAlign:
        {
            val.setInt32(view->getTextAlignment());
        }
            break;
        case kText:
        {
            val.set(value_to_jsval(view->getText().c_str()));
        }
            break;
        case kFontSize:
        {
            val.setInt32(view->getFontSize());
        }
            break;
        case kTextStroke:
        {
            val.setBoolean(view->isTextStroke());
        }
            break;
        case kTextStrokeColor:
        {
            val.set(color_to_jsval(cx, view->getTextStrokeColor()));
        }
            break;
        case kTextColor:
        {
            val.set(color_to_jsval(cx, view->getTextColor()));
        }
            break;
        case kLinkTextColor:
        {
            val.set(color_to_jsval(cx, view->getLinkTextColor()));
        }
            break;
        case kUnderlineLink:
        {
            val.setBoolean(view->getLinkUnderLine());
        }
            break;
        default:
            break;
    }
    return JS_TRUE;
}

JSBool JSRichLabel::jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSRichLabel* jview = (JSRichLabel*)JS_GetPrivate(obj);
    if (!jview)
    {
        JS_ReportError(cx, "JSRichLabel::jsPropertySet native object already disposed");
        return JS_FALSE;
    }
    HLRichLabel* view = jview->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
        case kTextAlign:
        {
            view->setTextAlignment(val.toInt32());
        }
            break;
        case kText:
        {
            char* str = JS_EncodeStringToUTF8(cx,val.toString());
            view->setText(str);
            JS_free(cx, str);
        }
            break;
        case kFontSize:
        {
            view->setFontSize(val.toInt32());
        }
            break;
        case kTextStroke:
        {
            view->setTextStroke(val.toBoolean());
        }
            break;
        case kTextStrokeColor:
        {
            color4F color = jsval_to_color(cx, val);
            color4B color1 = {static_cast<GLubyte>(color.r*255),
                                static_cast<GLubyte>(color.g*255),
                                static_cast<GLubyte>(color.b*255),
                                static_cast<GLubyte>(color.a*255)};
            view->setTextStrokeColor(color1);
        }
            break;
        case kTextColor:
        {
            color4F color = jsval_to_color(cx, val);
            color4B color1 = {static_cast<GLubyte>(color.r*255),
                                static_cast<GLubyte>(color.g*255),
                                static_cast<GLubyte>(color.b*255),
                                static_cast<GLubyte>(color.a*255)};
            view->setTextColor(color1);
        }
            break;
        case kLinkTextColor:
        {
            color4F color = jsval_to_color(cx, val);
            color4B color1 = {static_cast<GLubyte>(color.r*255),
                static_cast<GLubyte>(color.g*255),
                static_cast<GLubyte>(color.b*255),
                static_cast<GLubyte>(color.a*255)};
            view->setLinkTextColor(color1);
        }
            break;
        case kUnderlineLink:
        {
            view->setLinkUnderline(val.toBoolean());
        }
            break;
        default:
            break;
    }
    return JS_TRUE;
}

JSBool JSRichLabel::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 0)
    {
        HLRichLabel* view = new HLRichLabel(NULL);
        JSRichLabel* tmp = JSRichLabel::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    if (argc == 1)
    {
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSCppWrapperBase* parent = (JSCppWrapperBase*)JS_GetPrivate(arg0);
        if (!parent)
        {
            JS_ReportError(cx, "JSRichLabel: wrong parent argument");
            return JS_FALSE;
        }
        HLRichLabel* view = new HLRichLabel((HLView*)parent->object);
        JSRichLabel* tmp = JSRichLabel::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect frame = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSCppWrapperBase* parent = (JSCppWrapperBase*)JS_GetPrivate(arg0);
        if (!parent)
        {
            JS_ReportError(cx, "JSRichLabel: wrong parent argument");
            return JS_FALSE;
        }
        HLRichLabel* view = new HLRichLabel((HLView*)parent->object, frame);
        JSRichLabel* tmp = JSRichLabel::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    return JS_TRUE;
}

void JSRichLabel::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        {"textAlign", kTextAlign, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSRichLabel::jsPropertyGet), JSOP_WRAPPER(JSRichLabel::jsPropertySet)},
        {"text", kText, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSRichLabel::jsPropertyGet), JSOP_WRAPPER(JSRichLabel::jsPropertySet)},
        {"fontSize", kFontSize, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSRichLabel::jsPropertyGet), JSOP_WRAPPER(JSRichLabel::jsPropertySet)},
        {"textColor", kTextColor, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSRichLabel::jsPropertyGet), JSOP_WRAPPER(JSRichLabel::jsPropertySet)},
        {"textStroke", kTextStroke, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSRichLabel::jsPropertyGet), JSOP_WRAPPER(JSRichLabel::jsPropertySet)},
        {"textStrokeColor", kTextStrokeColor, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSRichLabel::jsPropertyGet), JSOP_WRAPPER(JSRichLabel::jsPropertySet)},
        {"linkTextColor", kLinkTextColor, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSRichLabel::jsPropertyGet), JSOP_WRAPPER(JSRichLabel::jsPropertySet)},
        {"underlineLink", kUnderlineLink, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSRichLabel::jsPropertyGet), JSOP_WRAPPER(JSRichLabel::jsPropertySet)},
        {0, 0, 0, 0, 0}
    };
    
    static JSFunctionSpec funcs[] =
    {
        JS_FN("sizeThatFits", JSRichLabel::jsSizeThatFits, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setDelegate", JSRichLabel::jsSetDelegate, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("startShowAnimation", JSRichLabel::jsStartShowAnimation, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("stopShowAnimation", JSRichLabel::jsStopShowAnimation, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setAlpha", JSRichLabel::jsSetAlpha, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("isInAnimation", JSRichLabel::jsIsInAnimation, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setAnimDelegate", JSRichLabel::jsSetAnimDelegate, 1, JSPROP_PERMANENT | JSPROP_READONLY),
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
    
    proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSRichLabel::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}

JSBool JSRichLabel::jsSizeThatFits(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSRichLabel* jLabel = (JSRichLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        JS_ReportError(cx, "JSRichLabel: wrong this object");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, HLSize_to_jsval(cx, jLabel->getNativeObject()->sizeThatFits()));
    return JS_TRUE;
}
    
JSBool JSRichLabel::jsSetDelegate(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSRichLabel* jLabel = (JSRichLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        JS_ReportError(cx, "JSRichLabel: wrong this object");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jLabel->getNativeObject()->delegate = newDelegate<HLRichLabel*, std::string, std::map<std::string, std::string>>(obj, JS_ARGV(cx, vp)[0]);
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        jLabel->getNativeObject()->delegate = newDelegate<HLRichLabel*, std::string, std::map<std::string, std::string>>(o, JS_ARGV(cx, vp)[1]);
    }
    return JS_TRUE;
}
    
JSBool JSRichLabel::jsStartShowAnimation(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSRichLabel* jLabel = (JSRichLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        JS_ReportError(cx, "JSRichLabel: wrong this object");
        return JS_FALSE;
    }
    if (argc > 0)
    {
        jLabel->getNativeObject()->startShowAnimation(JS_ARGV(cx, vp)[0].toNumber());
    }
    else
    {
        jLabel->getNativeObject()->startShowAnimation();
    }
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSRichLabel::jsStopShowAnimation(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSRichLabel* jLabel = (JSRichLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        JS_ReportError(cx, "JSRichLabel: wrong this object");
        return JS_FALSE;
    }
    jLabel->getNativeObject()->stopShowAnimation();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSRichLabel::jsSetAlpha(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSRichLabel* jLabel = (JSRichLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        JS_ReportError(cx, "JSRichLabel: wrong this object");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jLabel->getNativeObject()->setAlpha(JS_ARGV(cx, vp)[0].toNumber());
    }
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSRichLabel::jsIsInAnimation(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSRichLabel* jLabel = (JSRichLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        JS_ReportError(cx, "JSRichLabel: wrong this object");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(jLabel->getNativeObject()->isInAnimation()));
    return JS_TRUE;
}

JSBool JSRichLabel::jsSetAnimDelegate(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSRichLabel* jLabel = (JSRichLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        JS_ReportError(cx, "JSRichLabel: wrong this object");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jLabel->getNativeObject()->animDelegate = newDelegate<HLRichLabel*>(obj, JS_ARGV(cx, vp)[0]);
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        jLabel->getNativeObject()->animDelegate = newDelegate<HLRichLabel*>(o, JS_ARGV(cx, vp)[1]);
    }
    return JS_TRUE;
}
    
}

NS_HL_END
