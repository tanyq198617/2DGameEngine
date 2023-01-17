//
//  JSLabel.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSLabel.h"
#include "JSView.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSLabel, gui::HLLabel>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSLabel, gui::HLLabel>::proto = NULL;

namespace gui
{
JSBool JSLabel::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSLabel* jview = ((JSLabel*)JS_GetPrivate(obj));
    if (!jview)
    {
        JS_ReportError(cx, "JSLabel::jsPropertyGet native object already disposed");
        return JS_FALSE;
    }
    HLLabel* view = jview->getNativeObject();
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
    case kBrightness:
    {
        val.setDouble(view->getBrightness());
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
    case kTextColorTL:
    {
        val.set(color_to_jsval(cx, view->getTextColorTL()));
    }
    break;
    case kTextColorBL:
    {
        val.set(color_to_jsval(cx, view->getTextColorBL()));
    }
    break;
    case kTextColorTR:
    {
        val.set(color_to_jsval(cx, view->getTextColorTR()));
    }
    break;
    case kTextColorBR:
    {
        val.set(color_to_jsval(cx, view->getTextColorBR()));
    }
    break;
    case kTextStroke:
    {
        val.setBoolean(view->isTextStroke());
    }
    break;
    case kBold:
    {
        val.setBoolean(view->isFontBold());
    }
    break;
    case kTextStrokeColor:
    {
        val.set(color_to_jsval(cx, view->getTextStrokeColor()));
    }
    break;
    case kTextColor:
    {
        val.set(color_to_jsval(cx, view->getTextColorTL()));
    }
    break;
    default:
        break;
    }
    return JS_TRUE;
}

JSBool JSLabel::jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSLabel* jview = ((JSLabel*)JS_GetPrivate(obj));
    if (!jview)
    {
        JS_ReportError(cx, "JSLabel::jsPropertySet native object already disposed");
        return JS_FALSE;
    }
    HLLabel* view = jview->getNativeObject();
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
    case kBrightness:
    {
        view->setBrightness(val.toNumber());
    }
    break;
    case kText:
    {
        JSString * jstr = val.toString();
        if (jstr)
        {
            char* str = JS_EncodeStringToUTF8(cx, jstr);
            view->setText(str);
            JS_free(cx, str);
        }
        else
        {
            view->setText("null");
        }
    }
    break;
    case kFontSize:
    {
        view->setFontSize(val.toInt32());
    }
    break;
    case kTextColorTL:
    {
        color4F color = jsval_to_color(cx, val);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        view->setTextColorTL(color1);
    }
    break;
    case kTextColorBL:
    {
        color4F color = jsval_to_color(cx, val);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        view->setTextColorBL(color1);
    }
    break;
    case kTextColorTR:
    {
        color4F color = jsval_to_color(cx, val);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        view->setTextColorTR(color1);
    }
    break;
    case kTextColorBR:
    {
        color4F color = jsval_to_color(cx, val);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        view->setTextColorBR(color1);
    }
    break;
    case kTextStroke:
    {
        view->setTextStroke(val.toBoolean());
    }
    break;
    case kBold:
    {
        view->setIsFontBold(val.toBoolean());
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
    default:
        break;
    }
    return JS_TRUE;
}

JSBool JSLabel::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 0)
    {
        HLLabel* view = new HLLabel(NULL);
        JSLabel* tmp = JSLabel::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
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
        HLLabel* view = new HLLabel((HLView*)parent->object);
        JSLabel* tmp = JSLabel::getOrCreateWrapper(cx, view);
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
            return JS_FALSE;
        }
        HLLabel* view = new HLLabel((HLView*)parent->object, frame);
        JSLabel* tmp = JSLabel::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    return JS_TRUE;
}

void JSLabel::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        {"textAlign", kTextAlign, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {"brightness", kBrightness, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {"text", kText, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {"fontSize", kFontSize, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {"textColorTL", kTextColorTL, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {"textColorBL", kTextColorBL, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {"textColorTR", kTextColorTR, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {"textColorBR", kTextColorBR, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {"textColor", kTextColor, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {"textStroke", kTextStroke, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {"bold", kBold, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {"textStrokeColor", kTextStrokeColor, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSLabel::jsPropertyGet), JSOP_WRAPPER(JSLabel::jsPropertySet)},
        {0, 0, 0, 0, 0}
    };

    static JSFunctionSpec funcs[] =
    {
        JS_FN("setText", JSLabel::jsSetText, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getText", JSLabel::jsGetText, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setFontSize", JSLabel::jsSetFontSize, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getFontSize", JSLabel::jsGetFontSize, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setBrightness", JSLabel::jsSetBrightness, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getBrightness", JSLabel::jsGetBrightness, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTextColor", JSLabel::jsSetTextColor, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTextColorTL", JSLabel::jsSetTextColorTL, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTextColorTR", JSLabel::jsSetTextColorTR, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTextColorBL", JSLabel::jsSetTextColorBL, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTextColorBR", JSLabel::jsSetTextColorBR, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getTextColorTL", JSLabel::jsGetTextColorTL, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getTextColorTR", JSLabel::jsGetTextColorTR, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getTextColorBL", JSLabel::jsGetTextColorBL, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getTextColorBR", JSLabel::jsGetTextColorBR, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTextStroke", JSLabel::jsSetTextStroke, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("isTextStroke", JSLabel::jsIsTextStroke, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTextStrokeColor", JSLabel::jsSetTextStrokeColor, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getTextStrokeColor", JSLabel::jsGetTextStrokeColor, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTextAlignment", JSLabel::jsSetTextAlignment, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getTextAlignment", JSLabel::jsGetTextAlignment, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setIsFontBold", JSLabel::jsSetIsFontBold, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("isFontBold", JSLabel::jsIsFontBold, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("resizeHeightToFits", JSLabel::jsResizeHeightToFits, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("sizeThatFits", JSLabel::jsSizeThatFits, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        //JS_FN("setFont", JSLabel::jsSetFont, 1, JSPROP_PERMANENT | JSPROP_SHARED),
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

    proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSLabel::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}

JSBool JSLabel::jsSetText(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSString* arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        char * s = JS_EncodeStringToUTF8(cx,arg0);
        jLabel->getNativeObject()->setText(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSLabel::jsGetText(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    std::string str = jLabel->getNativeObject()->getText();
    JS_SET_RVAL(cx, vp, value_to_jsval(str));
    return JS_TRUE;
}

JSBool JSLabel::jsSetFontSize(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        jLabel->getNativeObject()->setFontSize(JSVAL_TO_INT(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSLabel::jsGetFontSize(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jLabel->getNativeObject()->getFontSize()));
    return JS_TRUE;
}

JSBool JSLabel::jsSetBrightness(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        jLabel->getNativeObject()->setBrightness(JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSLabel::jsGetBrightness(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jLabel->getNativeObject()->getBrightness()));
    return JS_TRUE;
}

JSBool JSLabel::jsSetTextColor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        color4F color = jsval_to_color(cx, JS_ARGV(cx, vp)[0]);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        jLabel->getNativeObject()->setTextColor(color1);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if(argc == 4)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        GLubyte r = (GLubyte)JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
        GLubyte g = (GLubyte)JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
        GLubyte b = (GLubyte)JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
        GLubyte a = (GLubyte)JSVAL_TO_INT(JS_ARGV(cx, vp)[3]);
        jLabel->getNativeObject()->setTextColor(r,g,b,a);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    return JS_TRUE;
}

JSBool JSLabel::jsSetTextColorTL(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        color4F color = jsval_to_color(cx, JS_ARGV(cx, vp)[0]);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        jLabel->getNativeObject()->setTextColorTL(color1);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSLabel::jsSetTextColorTR(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        color4F color = jsval_to_color(cx, JS_ARGV(cx, vp)[0]);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        jLabel->getNativeObject()->setTextColorTR(color1);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSLabel::jsSetTextColorBL(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        color4F color = jsval_to_color(cx, JS_ARGV(cx, vp)[0]);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        jLabel->getNativeObject()->setTextColorBL(color1);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSLabel::jsSetTextColorBR(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        color4F color = jsval_to_color(cx, JS_ARGV(cx, vp)[0]);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        jLabel->getNativeObject()->setTextColorBR(color1);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSLabel::jsGetTextColorTL(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, color_to_jsval(cx, jLabel->getNativeObject()->getTextColorTL()));
    return JS_TRUE;
}

JSBool JSLabel::jsGetTextColorTR(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, color_to_jsval(cx, jLabel->getNativeObject()->getTextColorTR()));
    return JS_TRUE;
}

JSBool JSLabel::jsGetTextColorBL(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, color_to_jsval(cx, jLabel->getNativeObject()->getTextColorBL()));
    return JS_TRUE;
}

JSBool JSLabel::jsGetTextColorBR(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, color_to_jsval(cx, jLabel->getNativeObject()->getTextColorBR()));
    return JS_TRUE;
}

JSBool JSLabel::jsSetTextStroke(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        jLabel->getNativeObject()->setTextStroke(JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSLabel::jsIsTextStroke(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jLabel->getNativeObject()->isTextStroke()));
    return JS_TRUE;
}

JSBool JSLabel::jsSetTextStrokeColor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        color4F color = jsval_to_color(cx, JS_ARGV(cx, vp)[0]);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        jLabel->getNativeObject()->setTextStrokeColor(color1);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if(argc == 4)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        GLubyte r = (GLubyte)JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
        GLubyte g = (GLubyte)JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
        GLubyte b = (GLubyte)JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
        GLubyte a = (GLubyte)JSVAL_TO_INT(JS_ARGV(cx, vp)[3]);
        jLabel->getNativeObject()->setTextStrokeColor(r,g,b,a);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    return JS_TRUE;
}

JSBool JSLabel::jsGetTextStrokeColor(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, color_to_jsval(cx, jLabel->getNativeObject()->getTextStrokeColor()));
    return JS_TRUE;
}

JSBool JSLabel::jsSetTextAlignment(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        jLabel->getNativeObject()->setTextAlignment(JSVAL_TO_INT(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSLabel::jsGetTextAlignment(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jLabel->getNativeObject()->getTextAlignment()));
    return JS_TRUE;
}

JSBool JSLabel::jsSetIsFontBold(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        if (!jLabel)
        {
            return JS_FALSE;
        }
        jLabel->getNativeObject()->setIsFontBold(JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSLabel::jsIsFontBold(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jLabel->getNativeObject()->isFontBold()));
    return JS_TRUE;
}

JSBool JSLabel::jsResizeHeightToFits(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    jLabel->getNativeObject()->resizeHeightToFits();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSLabel::jsSizeThatFits(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
    if (!jLabel)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, HLSize_to_jsval(cx, jLabel->getNativeObject()->sizeThatFits()));
    return JS_TRUE;
}

/*JSBool JSLabel::jsSetFont(JSContext *cx, uint32_t argc, jsval *vp)
{
     if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSLabel* jLabel = (JSLabel*)JS_GetPrivate(obj);
        jLabel->getNativeObject()->setFont();
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}*/

}

NS_HL_END
