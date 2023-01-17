//
//  JSComponent.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-26.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "JSComponent.h"
#include "JSEntitySystem.h"
#include "JSScriptingCore.h"
#include "HLTouchDispatcher.h"
#include "HLFileUtil.h"
#include "JSConversions.h"
#include "HLEntitySystem.h"
#include "HLTexture.h"

NS_HL_BEGIN

INIT_FAMILYID(JSComponent)

PROPERTIES_INITIAL(JSComponent)

JSComponent::JSComponent(const char* jsfile):m_elapsed(0),m_interval(0)
{
    m_jsfile = jsfile;
 
    // 获得js 上下文
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
    
    jsval fval;
    JSString * path = JS_NewStringCopyZ(cx, m_jsfile.c_str());
    jsval argv = STRING_TO_JSVAL(path);
    JS_CallFunctionName(cx, JSScriptingCore::getSingleton()->getGlobalJSObject(), "require", 1, &argv, &fval);
    HLASSERT(!fval.isNull(), "JSComponent: module not exsits!");
    m_jsobject = JS_NewObject(cx, NULL, NULL, NULL);
    jsval rval;
    JS_CallFunctionValue(cx, m_jsobject, fval, 0, NULL, &rval);
    if (JS_IsExceptionPending(cx))
    {
        if (!JS_ReportPendingException(cx))
        {
            JS_ClearPendingException(cx);
        }
    }
    JS_AddObjectRoot(cx, &m_jsobject);
}

JSComponent::~JSComponent()
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
    if (m_jsobject)
    {
        JS_RemoveObjectRoot(cx, &m_jsobject);
        JS_MaybeGC(cx);
    }
}

void JSComponent::onActive()
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
    
    JSEntity *e = JSEntity::getOrCreateWrapper(cx, mEntity);
    
    if (m_jsfile.length() == 0)
    {
        return;
    }
    
    JS::RootedValue v(cx, OBJECT_TO_JSVAL(e->jsobject));
    JS_SetProperty(cx, m_jsobject, "mEntity", v);
    
    JSBool found; JS_HasProperty(cx, m_jsobject, "onStart", &found);
    if (found) 
    {
        jsval rval;
        JS::RootedValue fval(cx);
        JS_GetProperty(cx, m_jsobject, "onStart", &fval);
        JS_CallFunctionValue(cx, m_jsobject, fval, 0, 0, &rval);
        
        if (JS_IsExceptionPending(cx))
        {
            if (!JS_ReportPendingException(cx))
            {
                JS_ClearPendingException(cx);
            }
        }
    }
    
    JS_HasProperty(cx, m_jsobject, "onUpdate", &found);
    if (found)
    {
        JS::RootedValue fval(cx);
        JS_GetProperty(cx, m_jsobject, "onUpdate", &fval);
        mUpdateFunc = fval.get();
        mEntity->onUpdate += newDelegate(this, &JSComponent::update);
        
        JS::RootedValue tmp(cx);
        JS_GetProperty(cx, m_jsobject, "updateInterval", &tmp);
        if (tmp.isDouble())
        {
            m_interval = tmp.toNumber();
        }
    }
    else
    {
        mUpdateFunc = JSVAL_NULL;
    }
    
    JS_HasProperty(cx, m_jsobject, "onTouches", &found);
    if (found)
    {
        JS::RootedValue fval(cx);
        JS_GetProperty(cx, m_jsobject, "onTouches", &fval);
        mTouchesFunc = fval.get();
        mEntity->onTouchEvent += newDelegate(this, &JSComponent::onTouches);
    }
    else
    {
        mTouchesFunc = JSVAL_NULL;
    }
    
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    JS_HasProperty(cx, m_jsobject, "onMouseEvent", &found);
    if (found)
    {
        JS::RootedValue fval(cx);
        JS_GetProperty(cx, m_jsobject, "onMouseEvent", &fval);
        mMouseFunc = fval.get();
        mEntity->onMouseEvent += newDelegate(this, &JSComponent::onMouseEvent);
    }
    else
    {
        mMouseFunc = JSVAL_NULL;
    }
#endif
    
    JS_MaybeGC(cx);
}

void JSComponent::onDeactive()
{
    if (mUpdateFunc != JSVAL_NULL)
    {
        mEntity->onUpdate -= newDelegate(this, &JSComponent::update);
    }
    if (mTouchesFunc != JSVAL_NULL)
    {
        mEntity->onTouchEvent -= newDelegate(this, &JSComponent::onTouches);
    }
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    if (mMouseFunc != JSVAL_NULL)
    {
        mEntity->onMouseEvent -= newDelegate(this, &JSComponent::onMouseEvent);
    }
#endif
    
    
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
    
    JSBool found; JS_HasProperty(cx, m_jsobject, "onStop", &found);
    if (found) 
    {
        jsval rval;
        JS::RootedValue fval(cx);
        JS_GetProperty(cx, m_jsobject, "onStop", &fval);
        JS_CallFunctionValue(cx, m_jsobject, fval, 0, 0, &rval);
        
        if (JS_IsExceptionPending(cx))
        {
            if (!JS_ReportPendingException(cx))
            {
                JS_ClearPendingException(cx);
            }
        }
    }
    
    JS_MaybeGC(cx);
}

void JSComponent::update(float delta)
{
    m_elapsed += delta;
    if (m_elapsed < m_interval)
    {
        return;
    }
    if (m_interval)
    {
        m_elapsed -= m_interval;
    }
    else
    {
        m_elapsed = 0;
    }
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());

    jsval rval;
    jsval jsdelta = DOUBLE_TO_JSVAL(delta);
    JS_CallFunctionValue(cx, m_jsobject , mUpdateFunc, 1, &jsdelta, &rval);
    
    if (JS_IsExceptionPending(cx))
    {
        if (!JS_ReportPendingException(cx))
        {
            JS_ClearPendingException(cx);
        }
    }
}

void JSComponent::onTouches(hoolai::HLTouchEvent* event)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
    
    jsval arg[2];
    arg[0] = INT_TO_JSVAL(event->type);
    int i = 0;
    int count = (int)event->touches.size();
    jsval *touches = new jsval[count];
    for (i = 0; i < count; i++) 
    {
        touches[i] = HLTouch_to_jsval(cx, event->touches[i]);
    }
    JSObject *array = JS_NewArrayObject(cx, count, touches);
    arg[1] = OBJECT_TO_JSVAL(array);
    
    jsval rval;
    JS_CallFunctionValue(cx, m_jsobject, mTouchesFunc, 2, arg, &rval);
    
    if (JS_IsExceptionPending(cx))
    {
        if (!JS_ReportPendingException(cx))
        {
            JS_ClearPendingException(cx);
        }
    }
    
    if (!rval.toBoolean())
    {
        mEntity->dispatchTouchEvent(event);
    }
    delete[] touches;
}

#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
void JSComponent::onMouseEvent(hoolai::HLMouseEvent* event)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
    
    jsval arg[2];
    arg[0] = INT_TO_JSVAL(event->type);
    arg[1] = HLMouseEvent_to_jsval(cx, *event);
    
    jsval rval;
    JS_CallFunctionValue(cx, m_jsobject, mMouseFunc, 2, arg, &rval);
    
    if (JS_IsExceptionPending(cx))
    {
        if (!JS_ReportPendingException(cx))
        {
            JS_ClearPendingException(cx);
        }
    }
    
    if (!rval.toBoolean())
    {
        mEntity->dispatchMouseEvent(event);
    }
}
#endif

JSBool JSComponent::onMessage(JSContext *cx, const char* message, uint32_t argc, jsval *vp, jsval *ret)
{
    JS::RootedValue fval(cx);
    JS_GetProperty(cx, m_jsobject, message, &fval);
    if (!JSVAL_IS_VOID(fval) && !JSVAL_IS_NULL(fval) && !JSVAL_IS_PRIMITIVE(fval) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(fval)))
    {
        JS_CallFunctionValue(cx, m_jsobject, fval, argc, vp, ret);
        if (JS_IsExceptionPending(cx))
        {
            if (!JS_ReportPendingException(cx))
            {
                JS_ClearPendingException(cx);
            }
        }
        return JS_TRUE;
    }
    return JS_FALSE;
}

NS_HL_END
