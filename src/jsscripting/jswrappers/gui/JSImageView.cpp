//
//  JSImageView.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSImageView.h"
#include "JSView.h"
#include "JSTexture.h"
#include "JSSpriteFrame.h"
#include "HLStringUtil.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSImageView, gui::HLImageView>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSImageView, gui::HLImageView>::proto = NULL;

namespace gui
{
JSBool JSImageView::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSImageView* jview = (JSImageView*)JS_GetPrivate(obj);
    if (!jview)
    {
        JS_ReportError(cx, "JSImageView::jsPropertyGet native object already disposed");
        return JS_FALSE;
    }
    HLImageView* view = jview->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
    case kTintColor:
    {
        val.set(color_to_jsval(cx, view->getTintColor()));
    }
    break;
    case kBrightness:
    {
        val.setDouble(view->getBrightness());
    }
    break;
    case kShowGrayImage:
    {
        val.setBoolean(view->isShowGrayImage());
    }
    break;
    case kFlipX:
    {
        val.setBoolean(view->isFlipX());
    }
    break;
    case kFlipY:
    {
        val.setBoolean(view->isFlipY());
    }
    break;
    case kRotation:
    {
        val.setDouble(view->getRotation());
    }
    break;
    case kRepeatStretch:
    {
        val.setBoolean(view->isRepeatStretch());
    }
    break;
    case kGlowRadius:
    {
        val.setNumber(view->getGlowRadius());
    }
        break;
    case kGlowColor:
    {
        val.set(color_to_jsval(cx, view->getGlowColor()));
    }
        break;
    default:
        break;
    }
    return JS_TRUE;
}

JSBool JSImageView::jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSImageView* jview = (JSImageView*)JS_GetPrivate(obj);
    if (!jview)
    {
        JS_ReportError(cx, "JSImageView::jsPropertySet native object already disposed");
        return JS_FALSE;
    }
    HLImageView* view = jview->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
    case kTintColor:
    {
        color4F color = jsval_to_color(cx, val);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
                            static_cast<GLubyte>(color.g*255),
                            static_cast<GLubyte>(color.b*255),
                            static_cast<GLubyte>(color.a*255)};
        view->setTintColor(color1);
    }
    break;
    case kBrightness:
    {
        view->setBrightness(val.toNumber());
    }
    break;
    case kShowGrayImage:
    {
        view->setShowGrayImage(val.toBoolean());
    }
    break;
    case kFlipX:
    {
        view->setFlipX(val.toBoolean());
    }
    break;
    case kFlipY:
    {
        view->setFlipY(val.toBoolean());
    }
    break;
    case kRotation:
    {
        view->setRotation(val.toNumber());
    }
    break;
    case kRepeatStretch:
    {
        view->setRepeatStretch(val.toBoolean());
    }
    break;
    case kGlowRadius:
    {
        view->setGlowRadius(val.toNumber());
    }
        break;
    case kGlowColor:
    {
        color4F color = jsval_to_color(cx, val);
        color4B color1 = {static_cast<GLubyte>(color.r*255),
            static_cast<GLubyte>(color.g*255),
            static_cast<GLubyte>(color.b*255),
            static_cast<GLubyte>(color.a*255)};
        view->setGlowColor(color1);
    }
        break;
    default:
        break;
    }
    return JS_TRUE;
}

JSBool JSImageView::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 0)
    {
        HLImageView* view = new HLImageView(NULL);
        JSImageView* tmp = JSImageView::getOrCreateWrapper(cx, view);
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
        HLImageView* view = new HLImageView((HLView*)parent->object);
        JSImageView* tmp = JSImageView::getOrCreateWrapper(cx, view);
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
        HLImageView* view = new HLImageView((HLView*)parent->object, frame);
        JSImageView* tmp = JSImageView::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    return JS_TRUE;
}

void JSImageView::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        {"tintColor", kTintColor, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSImageView::jsPropertyGet), JSOP_WRAPPER(JSImageView::jsPropertySet)},
        {"brightness", kBrightness, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSImageView::jsPropertyGet), JSOP_WRAPPER(JSImageView::jsPropertySet)},
        {"showGrayImage", kShowGrayImage, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSImageView::jsPropertyGet), JSOP_WRAPPER(JSImageView::jsPropertySet)},
        {"flipX", kFlipX, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSImageView::jsPropertyGet), JSOP_WRAPPER(JSImageView::jsPropertySet)},
        {"flipY", kFlipY, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSImageView::jsPropertyGet), JSOP_WRAPPER(JSImageView::jsPropertySet)},
        {"rotation", kRotation, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSImageView::jsPropertyGet), JSOP_WRAPPER(JSImageView::jsPropertySet)},
        {"repeatStretch", kRepeatStretch, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSImageView::jsPropertyGet), JSOP_WRAPPER(JSImageView::jsPropertySet)},
        {"glowRadius", kGlowRadius, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSImageView::jsPropertyGet), JSOP_WRAPPER(JSImageView::jsPropertySet)},
        {"glowColor", kGlowColor, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSImageView::jsPropertyGet), JSOP_WRAPPER(JSImageView::jsPropertySet)},
        {0, 0, 0, 0, 0}
    };

    static JSFunctionSpec funcs[] =
    {
        JS_FN("setImage", JSImageView::jsSetImage, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setImageTexture", JSImageView::jsSetImageTexture, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setImageSpriteFrame", JSImageView::jsSetImageSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
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

    proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSImageView::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}
    
JSBool JSImageView::jsSetImage(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSImageView* jImageView = (JSImageView*)JS_GetPrivate(obj);
    if (!jImageView)
    {
        JS_ReportError(cx, "JSImageView::jsSetImage native object already disposed");
        return JS_FALSE;
    }
    HLRect rect = HLRectZero;
    if (argc >= 1)
    {
        if (JSVAL_IS_NULL(JS_ARGV(cx, vp)[0]))
        {
            jImageView->getNativeObject()->setImage((HLTexture*)nullptr);
            return JS_TRUE;
        }
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        if (argc > 1) {
            rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        }
        HLTexture *tex = HLTexture::getTexture(narg0);
        HLSpriteFrame *sp = nullptr;
        if (!tex) {
            std::vector<std::string> split;
            StringUtil::Split(split, narg0, "/");
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
            jImageView->getNativeObject()->setImage(tex, rect);
        } else {
            jImageView->getNativeObject()->setImage(sp, rect);
        }
        
        JS_free(cx, narg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
    }
    
    return JS_TRUE;
}

JSBool JSImageView::jsSetImageTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSImageView* jImageView = (JSImageView*)JS_GetPrivate(obj);
        if (!jImageView)
        {
            JS_ReportError(cx, "JSImageView::jsSetImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            return JS_FALSE;
        }
        jImageView->getNativeObject()->setImage(jTexture->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSImageView* jImageView = (JSImageView*)JS_GetPrivate(obj);
        if (!jImageView)
        {
            JS_ReportError(cx, "JSImageView::jsSetImageTexture native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            JS_ReportError(cx, "JSImageView::jsSetImageTexture texture native object already disposed");
            return JS_FALSE;
        }
        jImageView->getNativeObject()->setImage(jTexture->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSImageView::jsSetImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSImageView* jImageView = (JSImageView*)JS_GetPrivate(obj);
        if (!jImageView)
        {
            JS_ReportError(cx, "JSImageView::jsSetImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSImageView::jsSetImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jImageView->getNativeObject()->setImage(jSpriteFrame->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSImageView* jImageView = (JSImageView*)JS_GetPrivate(obj);
        if (!jImageView)
        {
            JS_ReportError(cx, "JSImageView::jsSetImageSpriteFrame native object already disposed");
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            JS_ReportError(cx, "JSImageView::jsSetImageSpriteFrame spriteframe native object already disposed");
            return JS_FALSE;
        }
        jImageView->getNativeObject()->setImage(jSpriteFrame->getNativeObject(), rect);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}
}

NS_HL_END
