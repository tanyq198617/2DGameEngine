//
//  JSNotification.h
//  HoolaiEngine
//
//  Created by tyq on 13-4-23.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __JSNOTIFICATIONCENTER_H__
#define __JSNOTIFICATIONCENTER_H__

#include "JSCPPWrapper.h"

NS_HL_BEGIN

class HLNotificationCenter;

class JSNotificationCenter: public JSCPPWrapper<JSNotificationCenter, HLNotificationCenter>
{
public:
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    
    static JSBool jsDefaultCenter(JSContext *cx, uint32_t argc, jsval *vp);
    
    static JSBool jsPostNotification(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsAddObserver(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRemoveObserver(JSContext *cx, uint32_t argc, jsval *vp);
};

class JSNotificationWrapper: public HLNotificationInterface
{
private:
    JSObject* obj;
public:
    JSNotificationWrapper(JSObject* obj):obj(obj) {}
    
    virtual void onNotification(HLNotification* message);
};

NS_HL_END

#endif
