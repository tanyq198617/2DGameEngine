//
//  JSCPPPointWrapper.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/4/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "JSCPPWrapper.h"

using namespace std;

NS_HL_BEGIN

JSBool jsNoConstructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    return JS_FALSE;
}

NS_HL_END
