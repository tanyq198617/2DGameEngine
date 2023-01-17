//
//  JSEditBox.cpp
//  HoolaiEngine
//
//  Created by cai wei on 23/4/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "JSEditBox.h"
#include "JSView.h"
#include "JSLabel.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSEditBox, gui::HLEditBox>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSEditBox, gui::HLEditBox>::proto = NULL;

namespace gui
{
JSBool JSEditBox::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSEditBox* jview = (JSEditBox*)JS_GetPrivate(obj);
    if (!jview)
    {
        return JS_TRUE;
    }
    HLEditBox* view = jview->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
    case kReturnKeyType:
    {
        val.setInt32(view->getKeyboardReturnType());
    }
    break;
    case kInputMode:
    {
        val.setInt32(view->getInputMode());
    }
    break;
    case kSecureInput:
    {
        val.setBoolean(view->isSecureInput());
    }
    break;
    default:
        break;
    }
    return JS_TRUE;
}

JSBool JSEditBox::jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    HLEditBox* view = ((JSEditBox*)JS_GetPrivate(obj))->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
    case kReturnKeyType:
    {
        view->setKeyboardReturnType((gui::KeyboardReturnType)val.toInt32());
    }
    break;
    case kInputMode:
    {
        view->setInputMode((gui::EditBoxInputMode)val.toInt32());
    }
    break;
    case kSecureInput:
    {
        view->setSecureInput(val.toBoolean());
    }
    break;
    default:
        break;
    }
    return JS_TRUE;
}

JSBool JSEditBox::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect frame = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSCppWrapperBase* parent = (JSCppWrapperBase*)JS_GetPrivate(arg0);
        if (!parent)
        {
            return JS_FALSE;
        }
        HLEditBox* view = new HLEditBox((HLView*)parent->object, frame);
        JSEditBox* tmp = JSEditBox::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    return JS_TRUE;
}

void JSEditBox::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        {"returnKeyType", kReturnKeyType, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEditBox::jsPropertyGet), JSOP_WRAPPER(JSEditBox::jsPropertySet)},
        {"inputMode", kInputMode, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEditBox::jsPropertyGet), JSOP_WRAPPER(JSEditBox::jsPropertySet)},
        {"secureInput", kSecureInput, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEditBox::jsPropertyGet), JSOP_WRAPPER(JSEditBox::jsPropertySet)},
        {0, 0, 0, 0, 0}
    };

    static JSFunctionSpec funcs[] =
    {
        JS_FN("setKeyboardReturnType", JSEditBox::jsSetKeyboardReturnType, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getKeyboardReturnType", JSEditBox::jsGetKeyboardReturnType, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setInputMode", JSEditBox::jsSetInputMode, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getInputMode", JSEditBox::jsGetInputMode, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("active", JSEditBox::jsActive, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("resignActive", JSEditBox::jsResignActive, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setSecureInput", JSEditBox::jsSetSecureInput, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("isSecureInput", JSEditBox::jsIsSecureInput, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getText", JSEditBox::jsGetText, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getTextLabel", JSEditBox::jsGetTextLabel, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setPlaceHolder", JSEditBox::jsSetPlaceHolder, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getPlaceHolderLabel", JSEditBox::jsGetPlaceHolderLabel, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setText", JSEditBox::jsSetText, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("clearText", JSEditBox::jsClearText, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("insertText", JSEditBox::jsInsertText, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setCharInputFilter", JSEditBox::jsSetCharInputFilter, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setMaxLength", JSEditBox::jsSetMaxLength, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("deleteBackward", JSEditBox::jsDeleteBackward, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setMultiline", JSEditBox::jsSetMultiline, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setOnEditBeginListener", JSEditBox::jsSetOnEditBeginListener, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setOnEditEndListener", JSEditBox::jsSetOnEditEndListener, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setOnTextChangedListener", JSEditBox::jsSetOnTextChangedListener, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setOnReturnListener", JSEditBox::jsSetOnReturnListener, 1, JSPROP_PERMANENT | JSPROP_READONLY),
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

    proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSEditBox::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}

JSBool JSEditBox:: jsSetKeyboardReturnType(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
        if (!jEditBox)
        {
            return JS_FALSE;
        }
        jEditBox->getNativeObject()->setKeyboardReturnType((gui::KeyboardReturnType)(JSVAL_TO_INT(JS_ARGV(cx, vp)[0])));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSEditBox:: jsGetKeyboardReturnType(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if (!jEditBox)
    {
        return JS_FALSE;
    }
    gui::KeyboardReturnType mode = jEditBox->getNativeObject()->getKeyboardReturnType();
    JS_SET_RVAL(cx, vp, value_to_jsval(mode));
    return JS_TRUE;
}

JSBool JSEditBox:: jsSetInputMode(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
        if (!jEditBox)
        {
            return JS_FALSE;
        }
        jEditBox->getNativeObject()->setInputMode((gui::EditBoxInputMode)(JSVAL_TO_INT(JS_ARGV(cx, vp)[0])));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSEditBox:: jsGetInputMode(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if (!jEditBox)
    {
        return JS_FALSE;
    }
    gui::EditBoxInputMode mode = jEditBox->getNativeObject()->getInputMode();
    JS_SET_RVAL(cx, vp, value_to_jsval(mode));
    return JS_TRUE;
}

JSBool JSEditBox:: jsActive(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if (!jEditBox)
    {
        return JS_FALSE;
    }
    jEditBox->getNativeObject()->active();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSEditBox:: jsResignActive(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if (!jEditBox)
    {
        return JS_FALSE;
    }
    jEditBox->getNativeObject()->resignActive();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSEditBox:: jsSetSecureInput(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
        if (!jEditBox)
        {
            return JS_FALSE;
        }
        jEditBox->getNativeObject()->setSecureInput(JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSEditBox:: jsIsSecureInput(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if (!jEditBox)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jEditBox->getNativeObject()->isSecureInput()));
    return JS_TRUE;
}

JSBool JSEditBox:: jsGetText(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if (!jEditBox)
    {
        return JS_FALSE;
    }
    std::string str = jEditBox->getNativeObject()->getText();
    JS_SET_RVAL(cx, vp, value_to_jsval (str));
    return JS_TRUE;
}

JSBool JSEditBox:: jsGetTextLabel(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if (!jEditBox)
    {
        return JS_FALSE;
    }
    HLLabel *p = jEditBox->getNativeObject()->getTextLabel();
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JSLabel::getOrCreateWrapper(cx, p)->jsobject));
    return JS_TRUE;
}

JSBool JSEditBox:: jsSetPlaceHolder(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSString* arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
        if (!jEditBox)
        {
            return JS_FALSE;
        }
        char* s = JS_EncodeStringToUTF8(cx,arg0);
        jEditBox->getNativeObject()->setPlaceHolder(std::string(s));
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}
    
JSBool JSEditBox::jsGetPlaceHolderLabel(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if (!jEditBox)
    {
        return JS_FALSE;
    }
    HLLabel *p = jEditBox->getNativeObject()->getPlaceHolderLabel();
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JSLabel::getOrCreateWrapper(cx, p)->jsobject));
    return JS_TRUE;
}

    
JSBool JSEditBox::jsSetText(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSString* arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
        if (!jEditBox)
        {
            return JS_FALSE;
        }
        char* s = JS_EncodeStringToUTF8(cx,arg0);
        jEditBox->getNativeObject()->setText(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    return JS_TRUE;
}
    
JSBool JSEditBox::jsClearText(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if (!jEditBox)
    {
        return JS_FALSE;
    }
    jEditBox->getNativeObject()->clearText();
    return JS_TRUE;
}

JSBool JSEditBox:: jsInsertText(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSString* arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
        if (!jEditBox)
        {
            return JS_FALSE;
        }
        char* s = JS_EncodeStringToUTF8(cx,arg0);
        jEditBox->getNativeObject()->insertText(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}
    
JSBool JSEditBox::jsSetCharInputFilter(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSString* arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
        if (!jEditBox)
        {
            return JS_FALSE;
        }
        char* s = JS_EncodeStringToUTF8(cx,arg0);
        jEditBox->getNativeObject()->setCharInputFilter(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}
    
JSBool JSEditBox::jsSetMaxLength(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
        if (!jEditBox)
        {
            return JS_FALSE;
        }
        jEditBox->getNativeObject()->setMaxLength(JSVAL_TO_INT(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSEditBox:: jsDeleteBackward(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if (!jEditBox)
    {
        return JS_FALSE;
    }
    jEditBox->getNativeObject()->deleteBackward();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}
    
JSBool JSEditBox::jsSetMultiline(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
        if (!jEditBox)
        {
            return JS_FALSE;
        }
        jEditBox->getNativeObject()->setMultiline(JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSEditBox:: jsSetOnEditBeginListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if(!jEditBox)
    {
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jEditBox->getNativeObject()->onEditBegin = newDelegate<HLEditBox*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        jEditBox->getNativeObject()->onEditBegin = newDelegate<HLEditBox*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSEditBox:: jsSetOnEditEndListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if(!jEditBox)
    {
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jEditBox->getNativeObject()->onEditEnd = newDelegate<HLEditBox*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        jEditBox->getNativeObject()->onEditEnd = newDelegate<HLEditBox*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSEditBox:: jsSetOnTextChangedListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if(!jEditBox)
    {
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jEditBox->getNativeObject()->onTextChanged = newDelegate<HLEditBox*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        jEditBox->getNativeObject()->onTextChanged = newDelegate<HLEditBox*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSEditBox:: jsSetOnReturnListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEditBox* jEditBox = (JSEditBox*)JS_GetPrivate(obj);
    if(!jEditBox)
    {
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jEditBox->getNativeObject()->onReturn = newDelegate<HLEditBox*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        jEditBox->getNativeObject()->onReturn = newDelegate<HLEditBox*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}
}

NS_HL_END
