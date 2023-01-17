//
//  JSWidget.h
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSWIDGET_H__
#define __JSWIDGET_H__

#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
    
    class HLWidget;
    class HLRootWidget;
    
    class JSWidget: public JSCPPWrapper<JSWidget, HLWidget>
    {
    public:
        static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
        static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
        
        static JSBool jsShowWidget(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsClose(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsGetPresentWidget(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsSetOnCloseListener(JSContext *cx, uint32_t argc, jsval *vp);
    };
    
    class JSRootWidget : public JSCPPWrapper<JSRootWidget, HLRootWidget>
    {
    public:
        static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
        
        static JSBool jsGetTopWidget(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsGetMaskWidget(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsShowMaskWidget(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsDismissMaskWidget(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsShowHUDWidget(JSContext *cx, uint32_t argc, jsval *vp);
        static JSBool jsDismissHUDWidget(JSContext *cx, uint32_t argc, jsval *vp);
    };
}

NS_HL_END

#endif
