//
//  JSComponent.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-6-26.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __JSComponent_H__
#define __JSComponent_H__

#include "GameEngineConfig.h"
#include "HLComponent.h"
#include "JSEntitySystem.h"
#include "JSConversions.h"

class JSScript;

NS_HL_BEGIN

class JSComponent: public HLComponent
{
    PROPERTIES_DECLARE
private:
    std::string m_jsfile; // js文件地址

//    JSFunction *m_init_func;
    
    JSObject *m_jsobject;// js对象
    
    jsval mUpdateFunc; // js对象？
    jsval mTouchesFunc;
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    jsval mMouseFunc;
#endif
    
    float m_elapsed;
    float m_interval;

public:
    JSComponent(const char* jsfile);
    virtual ~JSComponent();
    
    FAMILYID

public:
    virtual void onActive();
    virtual void onDeactive();
    
    template<typename T>
    void setProperty(const char* name, T value)
    {
        //        if (mEntity)
        //        {
        JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
        JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
        JS::RootedValue val(cx, value_to_jsval(value));
        JS_SetProperty(cx, m_jsobject, name, val);
        //        }
    }
    
    void update(float delta);
    
    void onTouches(hoolai::HLTouchEvent* event);
    
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    void onMouseEvent(hoolai::HLMouseEvent* event);
#endif
    
    JSBool onMessage(JSContext *cx, const char* message, uint32_t argc, jsval *vp, jsval *ret);
    
    const std::string& getFileName() {return m_jsfile;}
    
    JSObject* getJSObject() {return m_jsobject;}
};

NS_HL_END

#endif
