//
//  JSTouchDispatcher.h
//  HoolaiEngine
//
//  Created by zhao shuan on 4/3/15.
//  Copyright (c) 2015 ICT. All rights reserved.
//

#ifndef __JSTOUCHDISPATCHER_H__
#define __JSTOUCHDISPATCHER_H__

#include "GameEngineConfig.h"

#if GAME_PROJECTION == PROJECTION_2D

#include "jsapi.h"
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
#include "HLMouseDispatcher.h"
#else
#include "HLTouchDispatcher.h"
#endif

#include "JSSceneManager.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

class JSTouchDispatcher
{
public:
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    static JSBool jsNeedsHitTest(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetMultiTouchEnabled(JSContext *cx, uint32_t argc, jsval *vp);
};

NS_HL_END

#else

#endif

#endif /* defined(__HoolaiEngine__JSTouchDispatcher__) */
