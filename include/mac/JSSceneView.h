//
//  JSSceneView.h
//  HoolaiEngine
//
//  Created by tyq on 13-6-5.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSSCENEVIEW_H__
#define __JSSCENEVIEW_H__

#include "JSCPPWrapper.h"
#include "HLSceneView.h"

NS_HL_BEGIN

namespace gui
{
    class JSSceneView : public JSCPPWrapper<JSSceneView, HLSceneView>
    {
    public:
        static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
        static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
        static JSBool jsGetSceneManager(JSContext *cx, uint32_t argc, jsval *vp);
    };
}

NS_HL_END

#endif
