//
//  JSView.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSView.h"
#include "HLView.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<gui::JSView, gui::HLView>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<gui::JSView, gui::HLView>::proto = NULL;

namespace gui
{
    JSBool JSView::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
    {
        int32_t propId = JSID_TO_INT(_id);
        JSView *jview = (JSView*)JS_GetPrivate(obj);
        if (!jview)
        {
            JS_ReportError(cx, "JSView::jsPropertyGet native object already disposed");
            return JS_FALSE;
        }
        if (propId == kAnimationDelegate)
        {
            return JS_FALSE;
        }
        HLView* view = (jview)->getNativeObject();
        if (!view)
        {
            return JS_FALSE;
        }
        switch (propId)
        {
            case kFrame:
            {
                val.set(HLRect_to_jsval(cx, view->getFrame()));
            }
                break;
            case kCenter: 
            {
                val.set(HLPoint_to_jsval(cx, view->getCenter()));
            }
                break;
            case kPosition:
            {
                val.set(HLPoint_to_jsval(cx, view->getPosition()));
            }
                break;
            case kSize:
            {
                val.set(HLSize_to_jsval(cx, view->getSize()));
            }
                break;
            case kPositionInPercent:
            {
                val.set(HLPoint_to_jsval(cx, view->getPositionPercent()));
            }
                break;
            case kSizeInPercent:
            {
                val.set(HLSize_to_jsval(cx, view->getSizePercent()));
            }
                break;
            case kVisible:
            {
                val.setBoolean(view->isVisible());
            }
                break;
            case kUserInteractionEnabled:
            {
                val.setBoolean(view->userInteractionEnalbed());
            }
                break;
            case kClipToBounds:
            {
                val.setBoolean(view->clipToBounds());
            }
                break;
            case kBackgroundColor:
            {
                val.set(color_to_jsval(cx, view->getBackgroundColor()));
            }
                break;
            case kTag:
            {
                val.setInt32(view->getTag());
            }
                break;
            case kParent:
            {
                HLView* parent = view->getParentView();
                if (parent)
                {
                    val.setObject(*(parent->getOrCreateJSWrapper(cx)->jsobject));
                }
                else
                {
                    val.setNull();
                }
            }
                break;
            default:
                break;
        }
        return JS_TRUE;
    }
    JSBool JSView::jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val)
    {
        int32_t propId = JSID_TO_INT(_id);
        if (propId == kAnimationDelegate)
        {
            HLView::animationDidStopCallback = newDelegate<bool, void*>(obj, val.get());
            return JS_TRUE;
        }
        JSView *jview = (JSView*)JS_GetPrivate(obj);
        if (!jview)
        {
            JS_ReportError(cx, "JSView::jsPropertyGet native object already disposed");
            return JS_FALSE;
        }
		HLView* view = jview->getNativeObject();
        if (!view)
        {
            return JS_FALSE;
        }
        switch (propId)
        {
            case kFrame:
            {
                HLRect frame = jsval_to_HLRect(cx, val);
                view->setFrame(frame);
            }
                break;
            case kCenter: 
            {
                HLPoint center = jsval_to_HLPoint(cx, val);
                view->setCenter(center);
            }
                break;
            case kPosition:
            {
                HLPoint pos = jsval_to_HLPoint(cx, val);
                view->setPosition(pos);
            }
                break;
            case kSize:
            {
                HLSize size = jsval_to_HLSize(cx, val);
                view->setSize(size);
            }
                break;
            case kPositionInPercent:
            {
                HLPoint pos = jsval_to_HLPoint(cx, val);
                view->setPositionPercent(pos);
            }
                break;
            case kSizeInPercent:
            {
                HLSize size = jsval_to_HLSize(cx, val);
                view->setSizePercent(size);
            }
                break;
            case kVisible:
            {
                view->setVisible(val.toBoolean());
            }
                break;
            case kUserInteractionEnabled:
            {
                view->setUserInteractionEnabled(val.toBoolean());
            }
                break;
            case kClipToBounds:
            {
                view->setClipToBounds(val.toBoolean());
            }
                break;
            case kBackgroundColor:
            {
                color4F color = jsval_to_color(cx, val);
                color4B color1 = {static_cast<GLubyte>(color.r*255),
                                    static_cast<GLubyte>(color.g*255),
                                    static_cast<GLubyte>(color.b*255),
                                    static_cast<GLubyte>(color.a*255)};
                view->setBackgroundColor(color1);
            }
                break;
            case kTag:
            {
                view->setTag(val.toInt32());
            }
                break;
            default:
                break;
        }
        return JS_TRUE;
    }
    
    JSBool JSView::jsConstructor(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 0)
        {
            HLView* view = new HLView(NULL);
            JSView* tmp = JSView::getOrCreateWrapper(cx, view);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
            return JS_TRUE;
        }
        if (argc == 1)
        {
            JSObject *arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);
            JSCppWrapperBase* parent = (JSCppWrapperBase*)JS_GetPrivate(arg0);
            if (!parent)
            {
                JS_ReportError(cx, "JSView::jsConstructor parent native object already disposed");
                return JS_FALSE;
            }
            HLView* view = new HLView((HLView*)parent->object);
            JSView* tmp = JSView::getOrCreateWrapper(cx, view);
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
                JS_ReportError(cx, "JSView::jsConstructor parent native object already disposed");
                return JS_FALSE;
            }
            HLView* view = new HLView((HLView*)parent->object, frame);
            JSView* tmp = JSView::getOrCreateWrapper(cx, view);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
            return JS_TRUE;
        }
        return JS_FALSE;
    }
    
    bool JSView::jsHasInstance(JSContext *cx, JS::Handle<JSObject*> obj, JS::MutableHandle<JS::Value> vp, bool *bp)
    {
        *bp = true;
        return true;
    }
    
    void JSView::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        jsclass->hasInstance = jsHasInstance;
        jsclass->flags = JSCLASS_HAS_PRIVATE;
        
        static JSPropertySpec properties[] = {
            {"frame", kFrame, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSView::jsPropertyGet), JSOP_WRAPPER(JSView::jsPropertySet)},
            {"center", kCenter, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSView::jsPropertyGet), JSOP_WRAPPER(JSView::jsPropertySet)},
            {"position", kPosition, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSView::jsPropertyGet), JSOP_WRAPPER(JSView::jsPropertySet)},
            {"size", kSize, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSView::jsPropertyGet), JSOP_WRAPPER(JSView::jsPropertySet)},
            {"positionInPercent", kPositionInPercent, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSView::jsPropertyGet), JSOP_WRAPPER(JSView::jsPropertySet)},
            {"sizeInPercent", kSizeInPercent, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSView::jsPropertyGet), JSOP_WRAPPER(JSView::jsPropertySet)},
            {"visible", kVisible, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSView::jsPropertyGet), JSOP_WRAPPER(JSView::jsPropertySet)},
            {"userInteractionEnabled", kUserInteractionEnabled, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSView::jsPropertyGet), JSOP_WRAPPER(JSView::jsPropertySet)},
            {"clipToBounds", kClipToBounds, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSView::jsPropertyGet), JSOP_WRAPPER(JSView::jsPropertySet)},
            {"backgroundColor", kBackgroundColor, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSView::jsPropertyGet), JSOP_WRAPPER(JSView::jsPropertySet)},
            {"tag", kTag, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSView::jsPropertyGet), JSOP_WRAPPER(JSView::jsPropertySet)},
            {"parent", kParent, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, JSOP_WRAPPER(JSView::jsPropertyGet), {0, 0}},
            {0, 0, 0, 0, 0}
        };
        
        static JSFunctionSpec funcs[] = {
            JS_FN("centerInParent", JSView::jsCenterInParent, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("addSubview", JSView::jsAddSubview, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("removeFromParent", JSView::jsRemoveFromParent, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("removeSubview", JSView::jsRemoveSubview, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("removeAllSubviews", JSView::jsRemoveAllSubviews, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("findViewWithTag", JSView::jsFindViewWithTag, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("bringSubviewToFront", JSView::jsBringSubviewToFront, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("sendSubviewToBack", JSView::jsSendSubviewToBack, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("moveSubviewBelowSubview", JSView::jsMoveSubviewBelowSubview, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("getSubviews", JSView::jsGetSubviews, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("convertToNodeSpace", JSView::jsConvertToNodeSpace, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("convertToWorldSpace", JSView::jsConvertToWorldSpace, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("touchLocationInView", JSView::jsTouchLocationInView, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("onTouches", JSView::jsOnTouches, 1, JSPROP_PERMANENT | JSPROP_READONLY),
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
            JS_FN("mouseLocationInView", JSView::jsMouseLocationInView, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("onMouseEvent", JSView::jsOnMouseEvent, 1, JSPROP_PERMANENT | JSPROP_READONLY),
#endif
            JS_FN("setTouchEventListener", JSView::jsSetTouchEventListener, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setDraggingEventListener", JSView::jsSetDraggingEventListener, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("clone", JSView::jsClone, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FS_END
        };
        
        static JSPropertySpec st_properties[] = {
            {"animationDelegate", kAnimationDelegate, JSPROP_PERMANENT | JSPROP_SHARED, {0, 0}, JSOP_WRAPPER(JSView::jsPropertySet)},
            {0, 0, 0, 0, 0}
        };
        
        static JSFunctionSpec st_funcs[] = {
            JS_FN("startAnimations", JSView::jsStartAnimations, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setAnimationDidStopCallback", JSView::jsSetAnimationDidStopCallback, 2, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setAutoReverse", JSView::jsSetAutoReverse, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setAnimationCurve", JSView::jsSetAnimationCurve, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("setAnimationCurveRate", JSView::jsSetAnimationCurveRate, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("stopAnimation", JSView::jsStopAnimation, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("stopAllAnimations", JSView::jsStopAllAnimations, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FN("commitAnimations", JSView::jsCommitAnimations, 1, JSPROP_PERMANENT | JSPROP_READONLY),
            JS_FS_END
        };
        
        proto = JS_InitClass(cx,globalObj,NULL,jsclass,JSView::jsConstructor,0,properties,funcs,st_properties,st_funcs);
    }
    
    JSBool JSView::jsCenterInParent(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSView* thiz = (JSView*)JS_GetPrivate(obj);
        if (!thiz)
        {
            JS_ReportError(cx, "JSView::jsCenterInParent native object already disposed");
            return JS_FALSE;
        }
        thiz->getNativeObject()->centerInParent();
        return JS_TRUE;
    }
    
    JSBool JSView::jsAddSubview(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsAddSubview native object already disposed");
                return JS_FALSE;
            }
            JSObject *arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);
            JSView* jview = (JSView*)JS_GetPrivate(arg0);
            if (!jview)
            {
                JS_ReportError(cx, "JSView::jsAddSubview subview native object already disposed");
                return JS_FALSE;
            }
            thiz->getNativeObject()->addSubview(jview->getNativeObject());
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        return JS_FALSE;
    }
    JSBool JSView::jsRemoveFromParent(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSView* thiz = (JSView*)JS_GetPrivate(obj);
        if (!thiz)
        {
            JS_ReportError(cx, "JSView::jsRemoveFromParent native object already disposed");
            return JS_FALSE;
        }
        JSBool b = JS_TRUE;
        if (argc == 1) {
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "b", &b);
        }
        thiz->getNativeObject()->removeFromParent(b);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    JSBool JSView::jsRemoveSubview(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc >= 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsRemoveSubview native object already disposed");
                return JS_FALSE;
            }
            JSObject *arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);
            JSView* jview = (JSView*)JS_GetPrivate(arg0);
            if (!jview)
            {
                JS_ReportError(cx, "JSView::jsRemoveSubview subview native object already disposed");
                return JS_FALSE;
            }
            JSBool arg1 = JS_TRUE;
            if (argc >1)
            {
                arg1 = JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[1]);
            }
            thiz->getNativeObject()->removeSubview(jview->getNativeObject(), arg1);
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        return JS_FALSE;
    }
    JSBool JSView::jsRemoveAllSubviews(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSView* thiz = (JSView*)JS_GetPrivate(obj);
        if (!thiz)
        {
            JS_ReportError(cx, "JSView::jsRemoveAllSubviews native object already disposed");
            return JS_FALSE;
        }
        thiz->getNativeObject()->removeAllSubviews();
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    JSBool JSView::jsFindViewWithTag(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsFindViewWithTag native object already disposed");
                return JS_FALSE;
            }
            int32_t arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "i", &arg0);
            HLView* view = thiz->getNativeObject()->findViewWithTag(arg0);
            if (view)
                JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(view->getOrCreateJSWrapper(cx)->jsobject));
            else
                JS_SET_RVAL(cx, vp, JSVAL_NULL);
            return JS_TRUE;
        }
        return JS_FALSE;
    }
    JSBool JSView::jsBringSubviewToFront(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsBringSubviewToFront native object already disposed");
                return JS_FALSE;
            }
            JSObject *arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);
            JSView* jview = (JSView*)JS_GetPrivate(arg0);
            if (!jview)
            {
                JS_ReportError(cx, "JSView::jsBringSubviewToFront subview native object already disposed");
                return JS_FALSE;
            }
            thiz->getNativeObject()->bringSubviewToFront(jview->getNativeObject());
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        return JS_FALSE;
    }
    JSBool JSView::jsSendSubviewToBack(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsSendSubviewToBack native object already disposed");
                return JS_FALSE;
            }
            JSObject *arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);
            JSView* jview = (JSView*)JS_GetPrivate(arg0);
            if (!jview)
            {
                JS_ReportError(cx, "JSView::jsSendSubviewToBack subview native object already disposed");
                return JS_FALSE;
            }
            thiz->getNativeObject()->sendSubviewToBack(jview->getNativeObject());
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        return JS_FALSE;
    }
    JSBool JSView::jsMoveSubviewBelowSubview(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 2)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsMoveSubviewBelowSubview native object already disposed");
                return JS_FALSE;
            }
            JSObject *arg0, *arg1;
            JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "oo", &arg0, &arg1);
            JSView* jview1 = (JSView*)JS_GetPrivate(arg0);
            JSView* jview2 = (JSView*)JS_GetPrivate(arg1);
            if (!jview1 || !jview2)
            {
                JS_ReportError(cx, "JSView::jsMoveSubviewBelowSubview subivew native object already disposed");
                return JS_FALSE;
            }
            thiz->getNativeObject()->moveSubviewBelowSubivew(jview1->getNativeObject(), jview2->getNativeObject());
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        return JS_FALSE;
    }
    JSBool JSView::jsGetSubviews(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSView* thiz = (JSView*)JS_GetPrivate(obj);
        if (!thiz)
        {
            JS_ReportError(cx, "JSView::jsGetSubviews native object already disposed");
            return JS_FALSE;
        }
        const std::list<HLView*>& subviews = thiz->getNativeObject()->getSubviews();
        jsval *resarr = new jsval[subviews.size()];
        int i = 0;
        for (std::list<HLView*>::const_iterator itr = subviews.begin(); itr != subviews.end(); ++itr, ++i)
        {
            resarr[i] = OBJECT_TO_JSVAL((*itr)->getOrCreateJSWrapper(cx)->jsobject);
        }
        JSObject *tmp = JS_NewArrayObject(cx, (int)subviews.size(), resarr);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp));
        delete[] resarr;
        return JS_TRUE;
    }
    JSBool JSView::jsConvertToNodeSpace(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsConvertToNodeSpace native object already disposed");
                return JS_FALSE;
            }
            HLPoint p = jsval_to_HLPoint(cx, JS_ARGV(cx, vp)[0]);
            p = thiz->getNativeObject()->convertToNodeSpace(p);
            JS_SET_RVAL(cx, vp, HLPoint_to_jsval(cx, p));
            return JS_TRUE;
        }
        return JS_FALSE;
    }
    JSBool JSView::jsConvertToWorldSpace(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsConvertToWorldSpace native object already disposed");
                return JS_FALSE;
            }
            HLPoint p = jsval_to_HLPoint(cx, JS_ARGV(cx, vp)[0]);
            p = thiz->getNativeObject()->convertToWorldSpace(p);
            JS_SET_RVAL(cx, vp, HLPoint_to_jsval(cx, p));
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsConvertToWorldSpace wrong argument number");
        return JS_FALSE;
    }
    JSBool JSView::jsTouchLocationInView(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsTouchLocationInView native object already disposed");
                return JS_FALSE;
            }
            HLTouch t = jsval_to_HLTouch(cx, JS_ARGV(cx, vp)[0]);
            HLPoint p = thiz->getNativeObject()->touchLocationInView(&t);
            JS_SET_RVAL(cx, vp, HLPoint_to_jsval(cx, p));
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsTouchLocationInView wrong argument number");
        return JS_FALSE;
    }
    JSBool JSView::jsOnTouches(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsOnTouches native object already disposed");
                return JS_FALSE;
            }
            HLTouchEvent t = jsval_to_HLTouchEvent(cx, JS_ARGV(cx, vp)[0]);
            thiz->getNativeObject()->onTouches(&t);
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsOnTouches wrong argument number");
        return JS_FALSE;
    }
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    JSBool JSView::jsMouseLocationInView(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsMouseLocationInView native object already disposed");
                return JS_FALSE;
            }
            HLMouseEvent t = jsval_to_HLMouseEvent(cx, JS_ARGV(cx, vp)[0]);
            HLPoint p = thiz->getNativeObject()->mouseLocationInView(&t);
            JS_SET_RVAL(cx, vp, HLPoint_to_jsval(cx, p));
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsMouseLocationInView wrong argument number");
        return JS_FALSE;
    }
    JSBool JSView::jsOnMouseEvent(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
            JSView* thiz = (JSView*)JS_GetPrivate(obj);
            if (!thiz)
            {
                JS_ReportError(cx, "JSView::jsOnMouseEvent native object already disposed");
                return JS_FALSE;
            }
            HLMouseEvent t = jsval_to_HLMouseEvent(cx, JS_ARGV(cx, vp)[0]);
            thiz->getNativeObject()->onMouseEvent(&t);
            JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsOnMouseEvent wrong argument number");
        return JS_FALSE;
    }
#endif
    
    JSBool JSView::jsStartAnimations(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc >= 2)
        {
            jsval* argv = JS_ARGV(cx, vp);
            char * name = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(argv[0]));
            double duration = argv[1].toNumber();
            if (argc > 3)
            {
                int repeate = argv[2].toInt32();
                JSObject* obj = argv[3].toObjectOrNull();
                HLView::startAnimations(name, duration, repeate, obj);
            }
            else if (argc == 3)
            {
                int repeate = argv[2].toInt32();
                HLView::startAnimations(name, duration, repeate);
            }
            else
            {
                HLView::startAnimations(name, duration);
            }
            JS_free(cx, name);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsStartAnimations wrong argument number");
        return JS_FALSE;
    }
    
    JSBool JSView::jsSetAnimationDidStopCallback(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            HLView::animationDidStopCallback = newDelegate<bool, void*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
            return JS_TRUE;
        }
        else if (argc == 2)
        {
            HLView::animationDidStopCallback = newDelegate<bool, void*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsSetAnimationDidStopCallback wrong argument number");
        return JS_FALSE;
    }
    
    JSBool JSView::jsSetAutoReverse(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSBool arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "b", &arg0);
            HLView::setAutoReverse(arg0);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsSetAutoReverse wrong argument number");
        return JS_FALSE;
    }
    
    JSBool JSView::jsSetAnimationCurve(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            int32_t arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "i", &arg0);
            HLView::setAnimationCurve((HLAnimationCurve)arg0);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsSetAnimationCurve wrong argument number");
        return JS_FALSE;
    }
    JSBool JSView::jsSetAnimationCurveRate(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            double arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "d", &arg0);
            HLView::setAnimationCurveRate(arg0);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsSetAnimationCurveRate wrong argument number");
        return JS_FALSE;
    }
    
    JSBool JSView::jsStopAnimation(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1)
        {
            JSString* arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
            char* s = JS_EncodeStringToUTF8(cx, arg0);
            HLView::stopAnimation(s);
            JS_free(cx, s);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsStopAnimation wrong argument number");
        return JS_FALSE;
    }
    
    JSBool JSView::jsStopAllAnimations(JSContext *cx, uint32_t argc, jsval *vp)
    {
        HLView::stopAllAnimations();
        return JS_TRUE;
    }
    
    JSBool JSView::jsCommitAnimations(JSContext *cx, uint32_t argc, jsval *vp)
    {
        HLView::commitAnimations();
        return JS_TRUE;
    }
    
    JSBool JSView::jsSetTouchEventListener(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSView* thiz = (JSView*)JS_GetPrivate(obj);
        if (argc == 1)
        {
            thiz->getNativeObject()->touchHandler = newDelegate<HLView*, HLTouchEvent*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
            return JS_TRUE;
        }
        else if (argc == 2)
        {
            thiz->getNativeObject()->touchHandler = newDelegate<HLView*, HLTouchEvent*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsStopAnimation wrong argument number");
        return JS_FALSE;
    }
    
    JSBool JSView::jsSetDraggingEventListener(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSView* jView = (JSView*)JS_GetPrivate(obj);
        if (!jView)
        {
            JS_ReportError(cx, "JSView::jsStartAnimations native object already disposed");
            return JS_FALSE;
        }
        if (argc == 1)
        {
            jView->getNativeObject()->setJSDraggingHandler(obj, JS_ARGV(cx, vp)[0]);
            return JS_TRUE;
        }
        else if (argc == 2)
        {
            JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
            jView->getNativeObject()->setJSDraggingHandler(o, JS_ARGV(cx, vp)[1]);
            return JS_TRUE;
        }
        JS_ReportError(cx, "JSView::jsStartAnimations wrong argument number");
        return JS_FALSE;
    }

    JSBool JSView::jsClone(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSView* jView = (JSView*)JS_GetPrivate(obj);
        if (!jView)
        {
            JS_ReportError(cx, "JSView::jsStartAnimations native object already disposed");
            return JS_FALSE;
        }
        
        if (argc > 0)
        {
            JSObject * o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
            JSView* v = (JSView*)JS_GetPrivate(o);
            HLView* view = jView->getNativeObject()->clone(v->getNativeObject());
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(view->getOrCreateJSWrapper(cx)->jsobject));
        }
        else
        {
            HLView* view = jView->getNativeObject()->clone(nullptr);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(view->getOrCreateJSWrapper(cx)->jsobject));
        }
        
        return JS_TRUE;
    }
}

NS_HL_END
