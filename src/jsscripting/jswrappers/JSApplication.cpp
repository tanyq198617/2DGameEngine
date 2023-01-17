//
//  JSApplication.cpp
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-10-24.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSApplication.h"
#include "HLApplication.h"

NS_HL_BEGIN

void JSApplication::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
{
    JSObject* obj = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue val(cx, OBJECT_TO_JSVAL(obj));
    JS_SetProperty(cx, globalObj, name, val);
    
    JS_DefineFunction(cx, obj, "getCurrentLanguage", JSApplication::jsGetCurrentLanguage, 0, JSPROP_READONLY | JSPROP_READONLY);
}

JSBool JSApplication::jsGetCurrentLanguage(JSContext *cx, uint32_t argc, jsval *vp)
{
    int language = (int)HLApplication::sharedApplication()->getCurrentLanguage();
    JS_SET_RVAL(cx, vp, INT_TO_JSVAL(language));
    return JS_TRUE;
}

NS_HL_END
