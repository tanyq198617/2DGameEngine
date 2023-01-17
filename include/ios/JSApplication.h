//
//  JSApplication.h
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-10-24.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSAPPLICATION_H__
#define __JSAPPLICATION_H__

#include "jsapi.h"
#include "HLMacros.h"

NS_HL_BEGIN

class JSApplication
{
public:
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    static JSBool jsGetCurrentLanguage(JSContext *cx, uint32_t argc, jsval *vp);
};

NS_HL_END

#endif
