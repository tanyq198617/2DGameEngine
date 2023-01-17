//
//  JSProgressRadialView.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-5-9.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSProgressRadialView.h"
#include "JSView.h"
#include "JSSpriteFrame.h"
#include "JSTexture.h"
#include "HLProgressRadialView.h"
#include "HLStringUtil.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSProgressRadialView, gui::HLProgressRadialView>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSProgressRadialView, gui::HLProgressRadialView>::proto = NULL;

namespace gui
{
    JSBool JSProgressRadialView::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 0)
        {
            HLProgressRadialView* view = new HLProgressRadialView(NULL);
            JSProgressRadialView* tmp = JSProgressRadialView::getOrCreateWrapper(cx, view);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        }
        if (argc == 1)
        {
            JSObject *arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);
            JSCppWrapperBase* parent = (JSCppWrapperBase*)JS_GetPrivate(arg0);
            if (!parent)
            {
                return JS_FALSE;
            }
            HLProgressRadialView* view = new HLProgressRadialView((HLView*)parent->object);
            JSProgressRadialView* tmp = JSProgressRadialView::getOrCreateWrapper(cx, view);
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
            HLProgressRadialView* view = new HLProgressRadialView((HLView*)parent->object, frame);
            JSProgressRadialView* tmp = JSProgressRadialView::getOrCreateWrapper(cx, view);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
            return JS_TRUE;
        }
        if (argc > 2)
        {
            JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
            HLRect frame = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
            int type = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
            JSCppWrapperBase* parent = (JSCppWrapperBase*)JS_GetPrivate(arg0);
            if (!parent)
            {
                return JS_FALSE;
            }
            HLProgressRadialView* view = new HLProgressRadialView((HLView*)parent->object, frame, (HLProgressRadialType)type);
            JSProgressRadialView* tmp = JSProgressRadialView::getOrCreateWrapper(cx, view);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
            return JS_TRUE;
        }
        
        return JS_TRUE;
    }
    
    void JSProgressRadialView::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
            {"progress", kProgress, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSProgressRadialView::jsPropertyGet), JSOP_WRAPPER(JSProgressRadialView::jsPropertySet)},
            {0, 0, 0, 0, 0}
        };
        
        static JSFunctionSpec funcs[] =
        {
            JS_FN("setColor", JSProgressRadialView::jsSetColor, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setImage", JSProgressRadialView::jsSetImage, 2, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setImageTexture", JSProgressRadialView::jsSetImageTexture, 2, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setImageSpriteFrame", JSProgressRadialView::jsSetImageSpriteFrame, 2, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setStartAngle", JSProgressRadialView::jsSetStartAngle, 2, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setEndAngle", JSProgressRadialView::jsSetEndAngle, 2, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setCenterPoint", JSProgressRadialView::jsSetCenterPoint, 2, JSPROP_PERMANENT | JSPROP_READONLY),
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
        
        proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSProgressRadialView::jsConstructor,0,properties,funcs,st_properties,st_funcs);
    }
    
    JSBool JSProgressRadialView::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
    {
        int32_t propId = JSID_TO_INT(_id);
        JSProgressRadialView* jview = (JSProgressRadialView*)JS_GetPrivate(obj);
        if (!jview)
        {
            JS_ReportError(cx, "JSProgressRadialView::jsPropertyGet native object already disposed");
            return JS_FALSE;
        }
        HLProgressRadialView* view = jview->getNativeObject();
        if (!view)
        {
            JS_ReportError(cx, "JSProgressRadialView::jsPropertyGet native object already disposed");
            return JS_FALSE;
        }
        switch (propId)
        {
            case kProgress:
                val.setDouble(view->getProgress());
                break;
                
        }
        return JS_TRUE;
    }
    
    JSBool JSProgressRadialView::jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val)
    {
        int32_t propId = JSID_TO_INT(_id);
        JSProgressRadialView* jview = (JSProgressRadialView*)JS_GetPrivate(obj);
        if (!jview)
        {
            JS_ReportError(cx, "JSProgressRadialView::jsPropertySet native object already disposed");
            return JS_FALSE;
        }
        HLProgressRadialView* view = jview->getNativeObject();
        if (!view)
        {
            JS_ReportError(cx, "JSProgressRadialView::jsPropertySet native object already disposed");
            return JS_FALSE;
        }
        switch (propId)
        {
            case kProgress:
                float progress = 0;
                if (val.isInt32())
                {
                    progress = val.toInt32();
                }
                else if (val.isDouble())
                {
                    progress = val.toNumber();
                }
                view->setProgress(progress);
                break;
        }
        return JS_TRUE;
    }
    
    JSBool JSProgressRadialView::jsSetColor(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSProgressRadialView* thiz = (JSProgressRadialView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                return JS_FALSE;
            }
            color4F p = jsval_to_color(cx, JS_ARGV(cx, vp)[0]);
            thiz->getNativeObject()->setColor(p);
            return JS_TRUE;
        }
        JS_ReportError(cx, "argument error");
        return JS_FALSE;
    }
    
    JSBool JSProgressRadialView::jsSetImage(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressRadialView* jImageView = (JSProgressRadialView*)JS_GetPrivate(obj);
        if (!jImageView)
        {
            return JS_FALSE;
        }
        HLRect rect = HLRectZero;
        if (argc >= 1)
        {
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
    
    JSBool JSProgressRadialView::jsSetImageTexture(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if(argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSProgressRadialView* jImageView = (JSProgressRadialView*)JS_GetPrivate(obj);
            if (!jImageView)
            {
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
            JSProgressRadialView* jImageView = (JSProgressRadialView*)JS_GetPrivate(obj);
            if (!jImageView)
            {
                return JS_FALSE;
            }
            JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
            HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
            JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
            if (!jTexture)
            {
                return JS_FALSE;
            }
            jImageView->getNativeObject()->setImage(jTexture->getNativeObject(), rect);
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        
        return JS_TRUE;
    }
    
    JSBool JSProgressRadialView::jsSetImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if(argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSProgressRadialView* jImageView = (JSProgressRadialView*)JS_GetPrivate(obj);
            if (!jImageView)
            {
                return JS_FALSE;
            }
            JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
            JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
            if (!jSpriteFrame)
            {
                return JS_FALSE;
            }
            jImageView->getNativeObject()->setImage(jSpriteFrame->getNativeObject());
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        else if (argc == 2)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSProgressRadialView* jImageView = (JSProgressRadialView*)JS_GetPrivate(obj);
            if (!jImageView)
            {
                return JS_FALSE;
            }
            JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
            HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
            JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
            if (!jSpriteFrame)
            {
                return JS_FALSE;
            }
            jImageView->getNativeObject()->setImage(jSpriteFrame->getNativeObject(), rect);
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        
        return JS_TRUE;
    }
    
    JSBool JSProgressRadialView::jsSetStartAngle(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressRadialView* jImageView = (JSProgressRadialView*)JS_GetPrivate(obj);
        if (!jImageView)
        {
            return JS_FALSE;
        }
        if (argc > 0) {
            jImageView->getNativeObject()->setStartAngle(JS_ARGV(cx, vp)[0].toNumber());
        }
        return JS_TRUE;
    }
    
    JSBool JSProgressRadialView::jsSetEndAngle(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressRadialView* jImageView = (JSProgressRadialView*)JS_GetPrivate(obj);
        if (!jImageView)
        {
            return JS_FALSE;
        }
        if (argc > 0) {
            jImageView->getNativeObject()->setEndAngle(JS_ARGV(cx, vp)[0].toNumber());
        }
        return JS_TRUE;
    }
    
    JSBool JSProgressRadialView::jsSetCenterPoint(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSProgressRadialView* jImageView = (JSProgressRadialView*)JS_GetPrivate(obj);
        if (!jImageView)
        {
            return JS_FALSE;
        }
        if (argc > 0) {
            jImageView->getNativeObject()->setCenterPoint(jsval_to_HLPoint(cx, JS_ARGV(cx, vp)[0]));
        }
        return JS_TRUE;
    }
}

NS_HL_END
