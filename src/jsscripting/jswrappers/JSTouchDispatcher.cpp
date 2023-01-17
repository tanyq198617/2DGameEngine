//
//  JSTouchDispatcher.cpp
//  HoolaiEngine
//
//  Created by tyq on 4/3/15.
//  Copyright (c) 2015 ICT. All rights reserved.
//

#include "JSTouchDispatcher.h"

NS_HL_BEGIN

void JSTouchDispatcher::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
{
    JSObject* obj = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue val(cx, OBJECT_TO_JSVAL(obj));
    JS_SetProperty(cx, globalObj, name, val);
    
    JS_DefineFunction(cx, obj, "needsHitTest", JSTouchDispatcher::jsNeedsHitTest, 0, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "setMultiTouchEnabled", JSTouchDispatcher::jsSetMultiTouchEnabled, 1, JSPROP_READONLY | JSPROP_READONLY);
}

JSBool JSTouchDispatcher::jsNeedsHitTest(JSContext *cx, uint32_t argc, jsval *vp)
{
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    HLMouseDispatcher::sharedMouseDispatcher()->needsHitTest();
#else
    HLTouchDispatcher::sharedTouchDispatcher()->needsHitTest();
#endif
    return JS_TRUE;
}

JSBool JSTouchDispatcher::jsSetMultiTouchEnabled(JSContext *cx, uint32_t argc, jsval *vp)
{
    bool value = false;
    if (argc == 1) {
        value = JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]);
    }
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
#else
    HLTouchDispatcher::sharedTouchDispatcher()->setMultiTouchEnabled(value);
#endif
    return JS_TRUE;
}

NS_HL_END
