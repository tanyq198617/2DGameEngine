//
//  JSButton.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSButton.h"
#include "JSView.h"
#include "JSTexture.h"
#include "JSSpriteFrame.h"
#include "JSImageView.h"
#include "JSLabel.h"
#include "HLStringUtil.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSButton, gui::HLButton>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSButton, gui::HLButton>::proto = NULL;

namespace gui
{
JSBool JSButton::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSButton* wrapper = (JSButton*)JS_GetPrivate(obj);
    if (!wrapper)
    {
        JS_ReportError(cx, "JSButton::jsPropertyGet native object already disposed");
        return JS_FALSE;
    }
    HLButton* view = wrapper->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
    case kZoomOnTouchDown:
    {
        val.setBoolean(view->isZoomOnTouchDown());
    }
    break;
    case kButtonState:
    {
        val.setInt32(view->getButtonState());
    }
    break;
    case kNormalTitle:
    {
        val.set(value_to_jsval(view->getNormalTitle().c_str()));
    }
    break;
    case kSelectedTitle:
    {
        val.set(value_to_jsval(view->getSelectedTitle().c_str()));
    }
    break;
    case kDisabledTitle:
    {
        val.set(value_to_jsval(view->getDisabledTitle().c_str()));
    }
    break;
    case kHighlightedTitle:
    {
        val.set(value_to_jsval(view->getHighlightedTitle().c_str()));
    }
    break;
    default:
        break;
    }
    return JS_TRUE;
}

JSBool JSButton::jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSButton* wrapper = (JSButton*)JS_GetPrivate(obj);
    if (!wrapper)
    {
        JS_ReportError(cx, "JSButton::jsPropertySet native object already disposed");
        return JS_FALSE;
    }
    HLButton* view = wrapper->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
    case kZoomOnTouchDown:
    {
        view->setZoomOnTouchDown(val.toBoolean());
    }
    break;
    case kButtonState:
    {
        view->setButtonState((HLButton::HLButtonState)(val.toInt32()));
    }
    break;
    case kNormalTitle:
    {
        char* s = JS_EncodeStringToUTF8(cx,val.toString());
        view->setNormalTitle(s);
        JS_free(cx, s);
    }
    break;
    case kSelectedTitle:
    {
        char* s = JS_EncodeStringToUTF8(cx,val.toString());
        view->setSelectedTitle(s);
        JS_free(cx, s);
    }
    break;
    case kDisabledTitle:
    {
        char* s = JS_EncodeStringToUTF8(cx,val.toString());
        view->setDisabledTitle(s);
        JS_free(cx, s);
    }
    break;
    case kHighlightedTitle:
    {
        char* s = JS_EncodeStringToUTF8(cx,val.toString());
        view->setHighlightedTitle(s);
        JS_free(cx, s);
    }
    break;
    default:
        break;
    }
    return JS_TRUE;
}

JSBool JSButton::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 0)
    {
        HLButton* view = new HLButton(NULL);
        JSButton* tmp = JSButton::getOrCreateWrapper(cx, view);
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
        HLButton* view = new HLButton((HLView*)parent->object);
        JSButton* tmp = JSButton::getOrCreateWrapper(cx, view);
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
        HLButton* view = new HLButton((HLView*)parent->object, frame);
        JSButton* tmp = JSButton::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    return JS_TRUE;
}

void JSButton::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
    jsclass->hasInstance = JSView::jsHasInstance;

    static JSPropertySpec properties[] =
    {
        {"zoomOnTouchDown", kZoomOnTouchDown, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSButton::jsPropertyGet), JSOP_WRAPPER(JSButton::jsPropertySet)},
        {"buttonState", kButtonState, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSButton::jsPropertyGet), JSOP_WRAPPER(JSButton::jsPropertySet)},
        {"normalTitle", kNormalTitle, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSButton::jsPropertyGet), JSOP_WRAPPER(JSButton::jsPropertySet)},
        {"selectedTitle", kSelectedTitle, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSButton::jsPropertyGet), JSOP_WRAPPER(JSButton::jsPropertySet)},
        {"disabledTitle", kDisabledTitle, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSButton::jsPropertyGet), JSOP_WRAPPER(JSButton::jsPropertySet)},
        {"highlightedTitle", kHighlightedTitle, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSButton::jsPropertyGet), JSOP_WRAPPER(JSButton::jsPropertySet)},
        {0, 0, 0, 0, 0}
    };

    static JSFunctionSpec funcs[] =
    {
        JS_FN("setSelected", JSButton::jsSetSelected, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setButtonState", JSButton::jsSetButtonState, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setEnabled", JSButton::jsSetEnabled, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setHighlighted", JSButton::jsSetHighlighted, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTitleSize", JSButton::jsSetTitleSize, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTitleColor", JSButton::jsSetTitleColor, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getTitleLabel", JSButton::jsGetTitleLabel, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getImageView", JSButton::jsGetImageView, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setNormalBackgroundImage", JSButton::jsSetNormalBackgroundImage, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setNormalBackgroundImageTexture", JSButton::jsSetNormalBackgroundImageTexture, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setNormalBackgroundImageSpriteFrame", JSButton::jsSetNormalBackgroundImageSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setDisabledBackgroundImage", JSButton::jsSetDisabledBackgroundImage, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setDisabledBackgroundImageTexture", JSButton::jsSetDisabledBackgroundImageTexture, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setDisabledBackgroundImageSpriteFrame", JSButton::jsSetDisabledBackgroundImageSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setSelectedBackgroundImage", JSButton::jsSetSelectedBackgroundImage, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setSelectedBackgroundImageTexture", JSButton::jsSetSelectedBackgroundImageTexture, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setSelectedBackgroundImageSpriteFrame", JSButton::jsSetSelectedBackgroundImageSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setHighlightedBackgroundImage", JSButton::jsSetHighlightedBackgroundImage, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setHighlightedBackgroundImageTexture", JSButton::jsSetHighlightedBackgroundImageTexture, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setHighlightedBackgroundImageSpriteFrame", JSButton::jsSetHighlightedBackgroundImageSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setNormalTitle", JSButton::jsSetNormalTitle, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getNormalTitle", JSButton::jsGetNormalTitle, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setSelectedTitle", JSButton::jsSetSelectedTitle, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getSelectedTitle", JSButton::jsGetSelectedTitle, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setDisabledTitle", JSButton::jsSetDisabledTitle, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getDisabledTitle", JSButton::jsGetDisabledTitle, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setHighlightedTitle", JSButton::jsSetHighlightedTitle, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getHighlightedTitle", JSButton::jsGetHighlightedTitle, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getNormalBackgroundSpriteFrame", JSButton::jsGetNormalBackgroundSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getSelectedBackgroundSpriteFrame", JSButton::jsGetSelectedBackgroundSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getDisableBackgroundSpriteFrame", JSButton::jsGetDisableBackgroundSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getHighlightedBackgroundSpriteFrame", JSButton::jsGetHighlightedBackgroundSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setOnButtonClickListener", JSButton::jsSetOnButtonClickListener, 2, JSPROP_PERMANENT | JSPROP_READONLY),
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

    proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSButton::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}

JSBool JSButton::jsSetSelected(JSContext *cx, uint32_t argc, jsval *vp) 
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetSelected native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setSelected(JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsSetButtonState(JSContext *cx, uint32_t argc, jsval *vp) 
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetButtonState native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setButtonState((HLButton::HLButtonState)(JSVAL_TO_INT(JS_ARGV(cx, vp)[0])));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsSetEnabled(JSContext *cx, uint32_t argc, jsval *vp) 
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetEnabled native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setEnabled(JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsSetHighlighted(JSContext *cx, uint32_t argc, jsval *vp) 
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetHighlighted native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setHighlighted(JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsSetTitleSize(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetTitleSize native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setTitleSize(JSVAL_TO_INT(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsSetTitleColor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        color4F color = jsval_to_color(cx, JS_ARGV(cx, vp)[0]);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetTitleColor native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setTitleColor(color1);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsGetTitleLabel(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
    if (!jButton)
    {
        JS_ReportError(cx, "JSButton::jsGetTitleLabel native object already disposed");
        return JS_FALSE;
    }
    HLLabel *p = jButton->getNativeObject()->getTitleLabel();
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JSLabel::getOrCreateWrapper(cx, p)->jsobject));
    return JS_TRUE;
}

JSBool JSButton::jsGetImageView(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
    if (!jButton)
    {
        JS_ReportError(cx, "JSButton::jsGetImageView native object already disposed");
        return JS_FALSE;
    }
    HLImageView *p = jButton->getNativeObject()->getImageView();
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JSImageView::getOrCreateWrapper(cx, p)->jsobject));
    return JS_TRUE;
}

JSBool JSButton::jsSetNormalBackgroundImage(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc >= 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetNormalBackgroundImage native object already disposed");
            return JS_FALSE;
        }
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        HLTexture *tex = HLTexture::getTexture(s);
        HLSpriteFrame *sp = nullptr;
        HLRect rect(HLRectZero);
        if (argc > 1)
        {
            rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        }
        
        if (!tex) {
            std::vector<std::string> split;
            StringUtil::Split(split, s, "/");
            if (split.size() > 1) {
                std::string framename = split.back();
                split.pop_back();
                std::string file = split.front();
                auto itr = split.begin();
                ++itr;
                for (; itr != split.end(); ++itr) {
                    file += std::string("/") + (*itr);
                }
                file += ".bsf";
                sp = HLSpriteFrame::getSpriteFrame(file.c_str(), framename.c_str());
            }
        }
        if (tex) {
            jButton->getNativeObject()->setNormalBackgroundImage(tex, rect);
        } else {
            jButton->getNativeObject()->setNormalBackgroundImage(sp, rect);
        }
        
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}
    
JSBool JSButton::jsSetNormalBackgroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetNormalBackgroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSButton::jsSetNormalBackgroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setNormalBackgroundImage(jTexture->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetNormalBackgroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSButton::jsSetNormalBackgroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setNormalBackgroundImage(jTexture->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSButton::jsSetNormalBackgroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetNormalBackgroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSButton::jsSetNormalBackgroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setNormalBackgroundImage(jSpriteFrame->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetNormalBackgroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSButton::jsSetNormalBackgroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setNormalBackgroundImage(jSpriteFrame->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}
    
JSBool JSButton::jsSetDisabledBackgroundImage(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc >= 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetDisabledBackgroundImage native object already disposed");
            return JS_FALSE;
        }
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        HLTexture *tex = HLTexture::getTexture(s);
        HLSpriteFrame *sp = nullptr;
        HLRect rect(HLRectZero);
        if (argc > 1)
        {
            rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        }
        
        if (!tex) {
            std::vector<std::string> split;
            StringUtil::Split(split, s, "/");
            if (split.size() > 1) {
                std::string framename = split.back();
                split.pop_back();
                std::string file = split.front();
                auto itr = split.begin();
                ++itr;
                for (; itr != split.end(); ++itr) {
                    file += std::string("/") + (*itr);
                }
                file += ".bsf";
                sp = HLSpriteFrame::getSpriteFrame(file.c_str(), framename.c_str());
            }
        }
        if (tex) {
            jButton->getNativeObject()->setDisabledBackgroundImage(tex, rect);
        } else {
            jButton->getNativeObject()->setDisabledBackgroundImage(sp, rect);
        }
        
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsSetDisabledBackgroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetDisabledBackgroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSButton::jsSetDisabledBackgroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setDisabledBackgroundImage(jTexture->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetDisabledBackgroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSButton::jsSetDisabledBackgroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setDisabledBackgroundImage(jTexture->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSButton::jsSetDisabledBackgroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetDisabledBackgroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSButton::jsSetDisabledBackgroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setDisabledBackgroundImage(jSpriteFrame->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetDisabledBackgroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSButton::jsSetDisabledBackgroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setDisabledBackgroundImage(jSpriteFrame->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}
    
JSBool JSButton::jsSetSelectedBackgroundImage(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc >= 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetSelectedBackgroundImage native object already disposed");
            return JS_FALSE;
        }
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        HLTexture *tex = HLTexture::getTexture(s);
        HLSpriteFrame *sp = nullptr;
        HLRect rect(HLRectZero);
        if (argc > 1)
        {
            rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        }
        
        if (!tex) {
            std::vector<std::string> split;
            StringUtil::Split(split, s, "/");
            if (split.size() > 1) {
                std::string framename = split.back();
                split.pop_back();
                std::string file = split.front();
                auto itr = split.begin();
                ++itr;
                for (; itr != split.end(); ++itr) {
                    file += std::string("/") + (*itr);
                }
                file += ".bsf";
                sp = HLSpriteFrame::getSpriteFrame(file.c_str(), framename.c_str());
            }
        }
        if (tex) {
            jButton->getNativeObject()->setSelectedBackgroundImage(tex, rect);
        } else {
            jButton->getNativeObject()->setSelectedBackgroundImage(sp, rect);
        }
        
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsSetSelectedBackgroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetSelectedBackgroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSButton::jsSetSelectedBackgroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setSelectedBackgroundImage(jTexture->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetSelectedBackgroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSButton::jsSetSelectedBackgroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setSelectedBackgroundImage(jTexture->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSButton::jsSetSelectedBackgroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetSelectedBackgroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSButton::jsSetSelectedBackgroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setSelectedBackgroundImage(jSpriteFrame->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetSelectedBackgroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSButton::jsSetSelectedBackgroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setSelectedBackgroundImage(jSpriteFrame->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}
    
JSBool JSButton::jsSetHighlightedBackgroundImage(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc >= 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetHighlightedBackgroundImage native object already disposed");
            return JS_FALSE;
        }
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        HLTexture *tex = HLTexture::getTexture(s);
        HLSpriteFrame *sp = nullptr;
        HLRect rect(HLRectZero);
        if (argc > 1)
        {
            rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        }
        
        if (!tex) {
            std::vector<std::string> split;
            StringUtil::Split(split, s, "/");
            if (split.size() > 1) {
                std::string framename = split.back();
                split.pop_back();
                std::string file = split.front();
                auto itr = split.begin();
                ++itr;
                for (; itr != split.end(); ++itr) {
                    file += std::string("/") + (*itr);
                }
                file += ".bsf";
                sp = HLSpriteFrame::getSpriteFrame(file.c_str(), framename.c_str());
            }
        }
        if (tex) {
            jButton->getNativeObject()->setHighlightedBackgroundImage(tex, rect);
        } else {
            jButton->getNativeObject()->setHighlightedBackgroundImage(sp, rect);
        }
        
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsSetHighlightedBackgroundImageTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetHighlightedBackgroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSButton::jsSetHighlightedBackgroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setHighlightedBackgroundImage(jTexture->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetHighlightedBackgroundImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSButton::jsSetHighlightedBackgroundImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setHighlightedBackgroundImage(jTexture->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSButton::jsSetHighlightedBackgroundImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetHighlightedBackgroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSButton::jsSetHighlightedBackgroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setHighlightedBackgroundImage(jSpriteFrame->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetHighlightedBackgroundImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSButton::jsSetHighlightedBackgroundImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jButton->getNativeObject()->setHighlightedBackgroundImage(jSpriteFrame->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSButton::jsSetNormalTitle(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSString* arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetNormalTitle native object already disposed");
            return JS_FALSE;
        }
        char* s = JS_EncodeStringToUTF8(cx,arg0);
        jButton->getNativeObject()->setNormalTitle(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsGetNormalTitle(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
    if (!jButton)
    {
        JS_ReportError(cx, "JSButton::jsGetNormalTitle native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jButton->getNativeObject()->getNormalTitle().c_str()));
    return JS_TRUE;
}

JSBool JSButton::jsSetSelectedTitle(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSString* arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetSelectedTitle native object already disposed");
            return JS_FALSE;
        }
        char* s = JS_EncodeStringToUTF8(cx,arg0);
        jButton->getNativeObject()->setSelectedTitle(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsGetSelectedTitle(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
    if (!jButton)
    {
        JS_ReportError(cx, "JSButton::jsGetSelectedTitle native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jButton->getNativeObject()->getSelectedTitle().c_str()));
    return JS_TRUE;
}

JSBool JSButton::jsSetDisabledTitle(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSString* arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetDisabledTitle native object already disposed");
            return JS_FALSE;
        }
        char* s = JS_EncodeStringToUTF8(cx,arg0);
        jButton->getNativeObject()->setDisabledTitle(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsGetDisabledTitle(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
    if (!jButton)
    {
        JS_ReportError(cx, "JSButton::jsGetDisabledTitle native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jButton->getNativeObject()->getDisabledTitle().c_str()));
    return JS_TRUE;
}

JSBool JSButton::jsSetHighlightedTitle(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSString* arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
        if (!jButton)
        {
            JS_ReportError(cx, "JSButton::jsSetHighlightedTitle native object already disposed");
            return JS_FALSE;
        }
        char* s = JS_EncodeStringToUTF8(cx,arg0);
        jButton->getNativeObject()->setHighlightedTitle(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSButton::jsGetHighlightedTitle(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
    if (!jButton)
    {
        JS_ReportError(cx, "JSButton::jsGetHighlightedTitle native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jButton->getNativeObject()->getHighlightedTitle().c_str()));
    return JS_TRUE;
}

JSBool JSButton::jsGetNormalBackgroundSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
    if (!jButton)
    {
        JS_ReportError(cx, "JSButton::jsGetNormalBackgroundSpriteFrame native object already disposed");
        return JS_FALSE;
    }
    HLSpriteFrame *p = jButton->getNativeObject()->getNormalBackgroundSpriteFrame();
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JSSpriteFrame::getOrCreateWrapper(cx, p)->jsobject));
    return JS_TRUE;
}

JSBool JSButton::jsGetSelectedBackgroundSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
    if (!jButton)
    {
        JS_ReportError(cx, "JSButton::jsGetSelectedBackgroundSpriteFrame native object already disposed");
        return JS_FALSE;
    }
    HLSpriteFrame *p = jButton->getNativeObject()->getSelectedBackgroundSpriteFrame();
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JSSpriteFrame::getOrCreateWrapper(cx, p)->jsobject));
    return JS_TRUE;
}

JSBool JSButton::jsGetDisableBackgroundSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
    if (!jButton)
    {
        JS_ReportError(cx, "JSButton::jsGetDisableBackgroundSpriteFrame native object already disposed");
        return JS_FALSE;
    }
    HLSpriteFrame *p = jButton->getNativeObject()->getDisableBackgroundSpriteFrame();
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JSSpriteFrame::getOrCreateWrapper(cx, p)->jsobject));
    return JS_TRUE;
}

JSBool JSButton::jsGetHighlightedBackgroundSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
    if (!jButton)
    {
        JS_ReportError(cx, "JSButton::jsGetHighlightedBackgroundSpriteFrame native object already disposed");
        return JS_FALSE;
    }
    HLSpriteFrame *p = jButton->getNativeObject()->getHighlightedBackgroundSpriteFrame();
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JSSpriteFrame::getOrCreateWrapper(cx, p)->jsobject));
    return JS_TRUE;
}
    
JSBool JSButton::jsSetOnButtonClickListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSButton* jButton = (JSButton*)JS_GetPrivate(obj);
    if (!jButton)
    {
        JS_ReportError(cx, "JSButton::jsSetOnButtonClickListener native object already disposed");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        jButton->getNativeObject()->onButtonClick = newDelegate<HLButton*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        jButton->getNativeObject()->onButtonClick = newDelegate<HLButton*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

}

NS_HL_END
