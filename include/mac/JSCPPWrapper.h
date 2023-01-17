//
//  JSCPPPointWrapper.h
//  HoolaiEngine
//
//  Created by tyq on 1/4/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __JSCPPOINTERWRAPPER_H__
#define __JSCPPOINTERWRAPPER_H__

#include "HLMacros.h"
#include "jsapi.h"
#include "JSScriptingCore.h"
#include <map>

NS_HL_BEGIN

extern std::map<void*, void *> jsobjmap;
extern bool _jsdebugging;

class JSCppWrapperBase
{
public:
    void* object;
    JSObject* jsobject;
};

JSBool jsNoConstructor(JSContext *cx, uint32_t argc, jsval *vp);

template<typename ClassType, typename NativeType>
class JSCPPWrapper: public JSCppWrapperBase
{
public:
    static JSClass *jsclass;
    static JSObject *proto;
    
    NativeType* getNativeObject()
    {
        return (NativeType*)object;
    }
    
    static ClassType* getOrCreateWrapper(JSContext *cx, NativeType *nativeObj)
    {
        if (!nativeObj)
            return nullptr;
        std::map<void *, void *>::iterator itr = jsobjmap.find(nativeObj);
        if (itr != jsobjmap.end())
        {
            return (ClassType*)itr->second;
        }
        JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
        JSObject *obj = JS_NewObject(cx, jsclass, proto, NULL);
        ClassType* wrapper = new ClassType();
        JS_SetPrivate(obj, wrapper);
        wrapper->object = nativeObj;
        wrapper->jsobject = obj;
        JS_AddObjectRoot(cx, &wrapper->jsobject);
        if (jsobjmap.find(nativeObj) != jsobjmap.end())
        {
            abort();
        }
        jsobjmap.insert(std::make_pair(nativeObj, wrapper));
        return wrapper;
    }
    
    static void jsFinalize(JSFreeOp *cx, JSObject *obj)
    {
    }
    
    static void removeJSObject(NativeType* nativeObj)
    {
        std::map<void *, void *>::iterator itr = jsobjmap.find(nativeObj);
        if (itr != jsobjmap.end())
        {
            ClassType* wrapper = (ClassType*)itr->second;
            JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
            JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
            JS_SetPrivate(wrapper->jsobject, NULL);
            JS::RootedValue val(cx, BOOLEAN_TO_JSVAL(true));
            JS_SetProperty(cx, wrapper->jsobject, "disposed", val);
            JS_RemoveObjectRoot(cx, &wrapper->jsobject);
            delete wrapper;
            jsobjmap.erase(itr);
            JS_MaybeGC(cx);
        }
    }
};

NS_HL_END

#endif
