//
//  JSTimer.h
//  HoolaiEngine
//
//  Created by tyq on 13-4-24.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSTIMER_H__
#define __JSTIMER_H__

#include "JSCPPWrapper.h"

NS_HL_BEGIN

class HLTimer;

class JSTimer: public JSCPPWrapper<JSTimer, HLTimer>
{
public:
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetDelegate(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsCancel(JSContext *cx, uint32_t argc, jsval *vp);
};

NS_HL_END

#endif
