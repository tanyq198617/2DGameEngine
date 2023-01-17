//
//  JSDirector2D.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-29.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#ifndef __JSDirector2D_H__
#define __JSDirector2D_H__

#include "GameEngineConfig.h"

#if GAME_PROJECTION == PROJECTION_2D

#include "jsapi.h"
#include "HLDirector2D.h"
#include "JSSceneManager.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

class JSDirector2D
{
public:
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    static JSBool jsGetCurrentSceneManager(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetWinSize(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsConvertToGLPoint(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsConvertToScreenPoint(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRunWithSceneManager(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsReplaceSceneManager(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsPushSceneManager(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsPopSceneManager(JSContext *cx, uint32_t argc, jsval *vp);
};

NS_HL_END

#else

#endif

#endif
