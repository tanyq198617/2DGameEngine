//
//  JSAnimationSystem.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-5-11.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSANIMATIONSYSTEM_H__
#define __JSANIMATIONSYSTEM_H__

#include "JSCPPWrapper.h"

NS_HL_BEGIN

class HLAnimateSystem;

class HLSpriteFrameAnimation;

class HLRepeateAnimation;

//class JSAnimationSystem : public JSCPPWrapper<JSAnimationSystem, HLAnimateSystem>
//{
//    
//};

class JSSpriteFrameAnimation : public JSCPPWrapper<JSSpriteFrameAnimation, HLSpriteFrameAnimation>
{
public:
    static JSBool jsAnimation(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    static JSBool jsRun(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetCallback(JSContext *cx, uint32_t argc, jsval *vp);
};

class JSRepeateAnimation : public JSCPPWrapper<JSRepeateAnimation, HLRepeateAnimation>
{
public:
    static JSBool jsAnimation(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    static JSBool jsRun(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetCallback(JSContext *cx, uint32_t argc, jsval *vp);
};

NS_HL_END

#endif
