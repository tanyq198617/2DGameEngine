//
//  JSAlertView.h
//  HoolaiEngine
//
//  Created by tyq on 13-4-13.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSALERTVIEW_H__
#define __JSALERTVIEW_H__

#include "HLAlertView.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
class HLAlertView;

class JSAlertView: public JSCPPWrapper<JSAlertView, HLAlertView>
{
public:
    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);

    static JSBool jsShow(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsDismiss(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetOnCancelClickListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetOnButtonClickListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetOnDismissListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetDarkTheme(JSContext *cx, uint32_t argc, jsval *vp); // static method
};    
}

NS_HL_END

#endif
