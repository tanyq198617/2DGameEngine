//
//  JSResourceManager.h
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-10-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSRESOURCEMANAGER_H__
#define __JSRESOURCEMANAGER_H__

#include "jsapi.h"
#include "HLMacros.h"

NS_HL_BEGIN

class JSResourceManager
{
public:
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    static JSBool jsFileExists(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetUnicodeStringFromFile(JSContext *cx, uint32_t argc, jsval *vp);
};

NS_HL_END

#endif
