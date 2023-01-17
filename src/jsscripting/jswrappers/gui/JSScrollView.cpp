//
//  JSScrollView.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSScrollView.h"
#include "JSView.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSScrollView, gui::HLScrollView>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSScrollView, gui::HLScrollView>::proto = NULL;

namespace gui
{
    
JSBool JSScrollView::jsPropertyGet(JSContext *cx, JS::HandleObject obj, JS::HandleId _id, JS::MutableHandleValue val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSScrollView* wrapper = (JSScrollView*)JS_GetPrivate(obj);
    if (!wrapper) {
        JS_ReportError(cx, "JSScrollView::jsPropertyGet native object already disposed");
        return JS_FALSE;
    }
    HLScrollView* view = wrapper->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
        case kContentOffset:
            val.set(HLPoint_to_jsval(cx, view->getContentOffset()));
            break;
        case kContentSize:
            val.set(HLSize_to_jsval(cx, view->getContentSize()));
            break;
        case kDirection:
            val.setInt32((int32_t)view->getDirection());
            break;
        case kPaging:
            val.setBoolean(view->isPagingEnabled());
            break;
        default:
            break;
    }
    return JS_TRUE;
}
JSBool JSScrollView::jsPropertySet(JSContext *cx, JS::HandleObject obj, JS::HandleId _id, JSBool strict, JS::MutableHandleValue val)
{
    int32_t propId = JSID_TO_INT(_id);
    JSScrollView* wrapper = (JSScrollView*)JS_GetPrivate(obj);
    if (!wrapper) {
        JS_ReportError(cx, "JSScrollView::jsPropertySet native object already disposed");
        return JS_FALSE;
    }
    HLScrollView* view = wrapper->getNativeObject();
    if (!view)
    {
        return JS_FALSE;
    }
    switch (propId)
    {
        case kContentOffset:
            view->setContentOffset(jsval_to_HLPoint(cx, val));
            break;
        case kContentSize:
            view->setContentSize(jsval_to_HLSize(cx, val));
            break;
        case kDirection:
            view->setDirection((HLScrollViewDirection)val.toInt32());
            break;
        case kPaging:
            view->setPagingEnabled(val.toBoolean());
            break;
        default:
            break;
    }
    return JS_TRUE;
}
    
JSBool JSScrollView::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 0)
    {
        HLScrollView* view = new HLScrollView(NULL);
        JSScrollView* tmp = JSScrollView::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    if (argc == 1)
    {
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JSScrollView* parent = (JSScrollView*)JS_GetPrivate(arg0);
        if (!parent)
        {
            JS_ReportError(cx, "JSScrollView::jsConstructor parent native object already disposed");
            return JS_FALSE;
        }
        HLScrollView* view = new HLScrollView(parent->getNativeObject());
        JSScrollView* tmp = JSScrollView::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject *arg0 = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        HLRect frame = jsval_to_HLRect(cx, JS_ARGV(cx, vp)[1]);
        JSScrollView* parent = (JSScrollView*)JS_GetPrivate(arg0);
        if (!parent)
        {
            return JS_FALSE;
        }
        HLScrollView* view = new HLScrollView(parent->getNativeObject(), frame);
        JSScrollView* tmp = JSScrollView::getOrCreateWrapper(cx, view);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        return JS_TRUE;
    }
    return JS_TRUE;
}

void JSScrollView::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        {"contentOffset", kContentOffset, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSScrollView::jsPropertyGet), JSOP_WRAPPER(JSScrollView::jsPropertySet)},
        {"contentSize", kContentSize, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSScrollView::jsPropertyGet), JSOP_WRAPPER(JSScrollView::jsPropertySet)},
        {"direction", kDirection, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSScrollView::jsPropertyGet), JSOP_WRAPPER(JSScrollView::jsPropertySet)},
        {"paging", kPaging, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSScrollView::jsPropertyGet), JSOP_WRAPPER(JSScrollView::jsPropertySet)},
        {0, 0, 0, 0, 0}
    };

    static JSFunctionSpec funcs[] =
    {
        JS_FN("setContentOffset", JSScrollView::jsSetContentOffset, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getContentOffset", JSScrollView::jsGetContentOffset, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setContentOffsetInDuration", JSScrollView::jsSetContentOffsetInDuration, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setZoomScale", JSScrollView::jsSetZoomScale, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getZoomScale", JSScrollView::jsGetZoomScale, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setZoomScaleInDuration", JSScrollView::jsSetZoomScaleInDuration, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("minContainerOffset", JSScrollView::jsMinContainerOffset, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("maxContainerOffset", JSScrollView::jsMaxContainerOffset, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("isViewVisible", JSScrollView::jsIsViewVisible, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getContentView", JSScrollView::jsGetContentView, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("isDragging", JSScrollView::jsIsDragging, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("isTouchMoved", JSScrollView::jsIsTouchMoved, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("isBounceable", JSScrollView::jsIsBounceable, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setBounceable", JSScrollView::jsSetBounceable, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("isPagingEnabled", JSScrollView::jsIsPagingEnabled, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setPagingEnabled", JSScrollView::jsSetPagingEnabled, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getTouchEnable", JSScrollView::jsGetTouchEnable, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setTouchEnable", JSScrollView::jsSetTouchEnable, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getDirection", JSScrollView::jsGetDirection, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setDirection", JSScrollView::jsSetDirection, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getContentSize", JSScrollView::jsGetContentSize, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setContentSize", JSScrollView::jsSetContentSize, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setScrollViewDidDragDelegate", JSScrollView::jsSetScrollViewDidDragDelegate, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setScrollViewDidScrollDelegate", JSScrollView::jsSetScrollViewDidScrollDelegate, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setScrollViewDidEndScrollingAnimationDelegate", JSScrollView::jsSetScrollViewDidEndScrollingAnimationDelegate, 1, JSPROP_PERMANENT | JSPROP_READONLY),

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

    proto = JS_InitClass(cx,globalObj,JSView::proto,jsclass,JSScrollView::jsConstructor,0,properties,funcs,st_properties,st_funcs);
}

JSBool JSScrollView::jsSetContentOffset(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsSetContentOffset native object already disposed");
            return JS_FALSE;
        }
        HLPoint point = jsval_to_HLPoint(cx, JS_ARGV(cx, vp)[0]);
        jScrollView->getNativeObject()->setContentOffset(point);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsSetContentOffset native object already disposed");
            return JS_FALSE;
        }
        HLPoint point = jsval_to_HLPoint(cx, JS_ARGV(cx, vp)[0]);
        jScrollView->getNativeObject()->setContentOffset(point, JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[1]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSScrollView::jsGetContentOffset(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsGetContentOffset native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, HLPoint_to_jsval(cx, jScrollView->getNativeObject()->getContentOffset()));
    return JS_TRUE;
}

JSBool JSScrollView::jsSetContentOffsetInDuration(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsSetContentOffsetInDuration native object already disposed");
            return JS_FALSE;
        }
        HLPoint point = jsval_to_HLPoint(cx, JS_ARGV(cx, vp)[0]);
        jScrollView->getNativeObject()->setContentOffsetInDuration(point, JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[1]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSScrollView::jsSetZoomScale(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsSetZoomScale native object already disposed");
            return JS_FALSE;
        }
        jScrollView->getNativeObject()->setZoomScale(JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsSetZoomScale native object already disposed");
            return JS_FALSE;
        }
        jScrollView->getNativeObject()->setZoomScale(JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[0]),
                JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[1]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSScrollView::jsGetZoomScale(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsGetZoomScale native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(jScrollView->getNativeObject()->getZoomScale()));
    return JS_TRUE;
}

JSBool JSScrollView::jsSetZoomScaleInDuration(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsSetZoomScaleInDuration native object already disposed");
            return JS_FALSE;
        }
        jScrollView->getNativeObject()->setZoomScaleInDuration(JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[0]),
                JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[1]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSScrollView::jsMinContainerOffset(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsMinContainerOffset native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, HLPoint_to_jsval(cx, jScrollView->getNativeObject()->minContainerOffset()));
    return JS_TRUE;
}

JSBool JSScrollView::jsMaxContainerOffset(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsMaxContainerOffset native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, HLPoint_to_jsval(cx, jScrollView->getNativeObject()->maxContainerOffset()));
    return JS_TRUE;
}

JSBool JSScrollView::jsIsViewVisible(JSContext *cx, uint32_t argc, jsval *vp)
{
    if(argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsIsViewVisible native object already disposed");
            return JS_FALSE;
        }
        JSView* jview = (JSView*)JS_GetPrivate(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(jScrollView->getNativeObject()->isViewVisible(jview->getNativeObject())));
        return JS_TRUE;
    }
    return JS_TRUE;
}

JSBool JSScrollView::jsGetContentView(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsGetContentView native object already disposed");
        return JS_FALSE;
    }
    HLView *view = (HLView *)(jScrollView->getNativeObject()->getContentView());
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(view->getOrCreateJSWrapper(cx)->jsobject));
    return JS_TRUE;
}

JSBool JSScrollView::jsIsDragging(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsIsDragging native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jScrollView->getNativeObject()->isDragging()));
    return JS_TRUE;
}

JSBool JSScrollView::jsIsTouchMoved(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsIsTouchMoved native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jScrollView->getNativeObject()->isTouchMoved()));
    return JS_TRUE;
}

JSBool JSScrollView::jsIsBounceable(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsIsBounceable native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jScrollView->getNativeObject()->isBounceable()));
    return JS_TRUE;
}

JSBool JSScrollView::jsSetBounceable(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsSetBounceable native object already disposed");
            return JS_FALSE;
        }
        jScrollView->getNativeObject()->setBounceable(JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}
    
JSBool JSScrollView::jsIsPagingEnabled(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsIsPagingEnabled native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jScrollView->getNativeObject()->isPagingEnabled()));
    return JS_TRUE;
}
    
JSBool JSScrollView::jsSetPagingEnabled(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsSetPagingEnabled native object already disposed");
            return JS_FALSE;
        }
        jScrollView->getNativeObject()->setPagingEnabled(JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSScrollView::jsGetDirection(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsGetDirection native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, INT_TO_JSVAL(jScrollView->getNativeObject()->getDirection()));
    return JS_TRUE;
}

JSBool JSScrollView::jsSetDirection(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsSetDirection native object already disposed");
            return JS_FALSE;
        }
        jScrollView->getNativeObject()->setDirection((HLScrollViewDirection)(JSVAL_TO_INT(JS_ARGV(cx, vp)[0])));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}

JSBool JSScrollView::jsSetContentSize(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsSetContentSize native object already disposed");
            return JS_FALSE;
        }
        HLSize size = jsval_to_HLSize(cx, JS_ARGV(cx, vp)[0]);
        jScrollView->getNativeObject()->setContentSize(size);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    return JS_TRUE;
}
    
JSBool JSScrollView::jsGetContentSize(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsGetContentSize native object already disposed");
        return JS_FALSE;
    }
    
    JS_SET_RVAL(cx, vp, HLSize_to_jsval(cx, jScrollView->getNativeObject()->getContentSize()));
    
    return JS_TRUE;
}
    
JSBool JSScrollView::jsSetScrollViewDidDragDelegate(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* scrollview = (JSScrollView*)JS_GetPrivate(obj);
    if (!scrollview)
    {
        JS_ReportError(cx, "jsSetScrollViewDidDragDelegate: native view not exist");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        scrollview->getNativeObject()->scrollViewDidDrag = newDelegate<HLScrollView*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        scrollview->getNativeObject()->scrollViewDidDrag = newDelegate<HLScrollView*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    JS_ReportError(cx, "jsSetScrollViewDidDragDelegate: wrong agrument number");
    return JS_FALSE;
}
    
JSBool JSScrollView::jsSetScrollViewDidScrollDelegate(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* scrollview = (JSScrollView*)JS_GetPrivate(obj);
    if (!scrollview)
    {
        JS_ReportError(cx, "jsSetScrollViewDidScrollDelegate: native view not exist");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        scrollview->getNativeObject()->scrollViewDidScroll = newDelegate<HLScrollView*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        scrollview->getNativeObject()->scrollViewDidScroll = newDelegate<HLScrollView*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    JS_ReportError(cx, "jsSetScrollViewDidScrollDelegate: wrong agrument number");
    return JS_FALSE;
}

JSBool JSScrollView::jsSetScrollViewDidEndScrollingAnimationDelegate(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* scrollview = (JSScrollView*)JS_GetPrivate(obj);
    if (!scrollview)
    {
        JS_ReportError(cx, "jsSetScrollViewDidEndScrollingAnimationDelegate: native view not exist");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        scrollview->getNativeObject()->scrollViewDidEndScrollingAnimation = newDelegate<HLScrollView*>(obj, JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        scrollview->getNativeObject()->scrollViewDidEndScrollingAnimation = newDelegate<HLScrollView*>(o, JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    JS_ReportError(cx, "jsSetScrollViewDidEndScrollingAnimationDelegate: wrong agrument number");
    return JS_FALSE;
}
    
JSBool JSScrollView::jsSetTouchEnable(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
        if (!jScrollView)
        {
            JS_ReportError(cx, "JSScrollView::jsSetTouchEnable native object already disposed");
            return JS_FALSE;
        }
        jScrollView->getNativeObject()->setTouchEnable(JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]));
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    
    return JS_TRUE;
}

JSBool JSScrollView::jsGetTouchEnable(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSScrollView* jScrollView = (JSScrollView*)JS_GetPrivate(obj);
    if (!jScrollView)
    {
        JS_ReportError(cx, "JSScrollView::jsGetTouchEnable native object already disposed");
        return JS_FALSE;
    }
    JS_SET_RVAL(cx, vp, value_to_jsval(jScrollView->getNativeObject()->getTouchEnable()));
    return JS_TRUE;
}

}

NS_HL_END
