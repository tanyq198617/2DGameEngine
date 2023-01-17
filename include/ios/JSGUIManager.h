//
//  JSGUIManager.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSGUIMANAGER_H__
#define __JSGUIMANAGER_H__

#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
    class HLGUIManager;
    
    class JSGUIManager: public JSCPPWrapper<JSGUIManager, HLGUIManager>
    {
    public:
		static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp) {return JS_FALSE;}
        static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
        static JSBool jsGetRootWidget(JSContext *cx, uint32_t argc, jsval *vp);
    };
}

NS_HL_END

#endif
