//
//  JSViewDND.cpp
//  HoolaiEngine
//
//  Created by cai wei on 26/4/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "JSViewDND.h"
#include "JSView.h"
#include "JSTexture.h"
#include "JSSpriteFrame.h"
#include "JSConversions.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSViewDND, gui::HLViewDragSource>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSViewDND, gui::HLViewDragSource>::proto = NULL;

namespace gui
{
/*JSBool JSViewDND::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    HLViewDragSource* view = ((HLViewDragSource*)JS_GetPrivate(obj))->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
    case kTag:
    {
        val.setInt32(view->getTag());
    }
    break;
    default:
        break;
    }
    return JS_TRUE;
}

JSBool JSViewDND::jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    HLViewDragSource* view = ((HLViewDragSource*)JS_GetPrivate(obj))->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
    case kTag:
    {
        view->setTag(val.toInt32());
    }
    break;
    default:
        break;
    }
    return JS_TRUE;
}*/

JSBool JSViewDND::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    HLViewDragSource* view = new HLViewDragSource();
    JSViewDND* tmp = JSViewDND::getOrCreateWrapper(cx, view);
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
    return JS_TRUE;
}

void JSViewDND::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        //{"tag", kTag, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSViewDND::jsPropertyGet), JSOP_WRAPPER(JSViewDND::jsPropertySet)},
        {0, 0, 0, 0, 0}
    };

    static JSFunctionSpec funcs[] =
    {
        JS_FN("doDragDrop", JSViewDND::jsDoDragDrop, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setDraggingView", JSViewDND::jsSetDraggingView, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setDraggingImage", JSViewDND::jsSetDraggingImage, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setDraggingImageTexture", JSViewDND::jsSetDraggingImageTexture, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setDraggingImageSpriteFrame", JSViewDND::jsSetDraggingImageSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setUserData", JSViewDND::jsSetUserData, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getUserData", JSViewDND::jsGetUserData, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTag", JSViewDND::jsSetTag, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getSourceView", JSViewDND::jsGetSourceView, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getTag", JSViewDND::jsGetTag, 1, JSPROP_PERMANENT | JSPROP_READONLY),
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

    proto = JS_InitClass(cx,globalObj,0,jsclass,JSViewDND::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}

JSBool JSViewDND:: jsDoDragDrop(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
        if (!jViewDND)
        {
            return JS_FALSE;
        }
        jViewDND->getNativeObject()->doDragDrop(jsval_to_HLPoint(cx, JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSViewDND:: jsSetDraggingView(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
        if (!jViewDND)
        {
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSCppWrapperBase* jView = (JSCppWrapperBase*)JS_GetPrivate(arg0);
        if (!jView)
        {
            return JS_FALSE;
        }
        jViewDND->getNativeObject()->setDraggingView((HLView*)jView->object);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
        if (!jViewDND)
        {
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSView* jView = (JSView*)JS_GetPrivate(arg0);
        if (!jView)
        {
            return JS_FALSE;
        }
        jViewDND->getNativeObject()->setDraggingView(jView->getNativeObject(),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[1]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 3)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
        if (!jViewDND)
        {
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSView* jView = (JSView*)JS_GetPrivate(arg0);
        if (!jView)
        {
            return JS_FALSE;
        }
        jViewDND->getNativeObject()->setDraggingView(jView->getNativeObject(),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[1]),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[2]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;

}

JSBool JSViewDND:: jsSetDraggingImage(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
    if (!jViewDND)
    {
        return JS_FALSE;
    }
    if (argc == 1)
    {
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        jViewDND->getNativeObject()->setDraggingImage(HLTexture::getTexture(narg0));
        JS_free(cx, narg0);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
    }
    else if (argc == 2)
    {
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        jViewDND->getNativeObject()->setDraggingImage(HLTexture::getTexture(narg0),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[1]));
        JS_free(cx, narg0);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
    }
    else if (argc == 3)
    {
        JSString *arg0 = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
        HLRect rect = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        jViewDND->getNativeObject()->setDraggingImage(HLTexture::getTexture(narg0),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[1]),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[2]));
        JS_free(cx, narg0);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
    }

    return JS_TRUE;
}

JSBool JSViewDND:: jsSetDraggingImageTexture(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
        if (!jViewDND)
        {
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            return JS_FALSE;
        }
        jViewDND->getNativeObject()->setDraggingImage(jTexture->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
        if (!jViewDND)
        {
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            return JS_FALSE;
        }
        jViewDND->getNativeObject()->setDraggingImage(jTexture->getNativeObject(),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[1]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 3)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
        if (!jViewDND)
        {
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSTexture* jTexture = (JSTexture*)JS_GetPrivate(arg0);
        if (!jTexture)
        {
            return JS_FALSE;
        }
        jViewDND->getNativeObject()->setDraggingImage(jTexture->getNativeObject(),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[1]),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[2]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSViewDND:: jsSetDraggingImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
        if (!jViewDND)
        {
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            return JS_FALSE;
        }
        jViewDND->getNativeObject()->setDraggingImage(jSpriteFrame->getNativeObject());
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
        if (!jViewDND)
        {
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            return JS_FALSE;
        }
        jViewDND->getNativeObject()->setDraggingImage(jSpriteFrame->getNativeObject(),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[1]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 3)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
        if (!jViewDND)
        {
            return JS_FALSE;
        }
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSSpriteFrame* jSpriteFrame = (JSSpriteFrame*)JS_GetPrivate(arg0);
        if (!jSpriteFrame)
        {
            return JS_FALSE;
        }
        jViewDND->getNativeObject()->setDraggingImage(jSpriteFrame->getNativeObject(),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[1]),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[2]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSViewDND:: jsSetUserData(JSContext *cx, uint32_t argc, jsval *vp)
{
    return JS_TRUE;
}

JSBool JSViewDND:: jsGetUserData(JSContext *cx, uint32_t argc, jsval *vp)
{
    return JS_TRUE;
}

JSBool JSViewDND:: jsSetTag(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
        if (!jViewDND)
        {
            return JS_FALSE;
        }
        jViewDND->getNativeObject()->setTag(JSVAL_TO_INT(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSViewDND:: jsGetSourceView(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
    if (!jViewDND)
    {
        return JS_FALSE;
    }
    HLView *p = jViewDND->getNativeObject()->getSourceView();
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JSView::getOrCreateWrapper(cx, p)->jsobject));
    return JS_TRUE;
}

JSBool JSViewDND:: jsGetTag(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSViewDND* jViewDND = (JSViewDND*)JS_GetPrivate(obj);
    if (!jViewDND)
    {
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jViewDND->getNativeObject()->getTag()));
    return JS_TRUE;
}
}

NS_HL_END
