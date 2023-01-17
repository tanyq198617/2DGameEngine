//
//  JSNotification.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-4-23.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSNotificationCenter.h"
#include "HLNotificationCenter.h"
#include "JSConversions.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<JSNotificationCenter, HLNotificationCenter>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<JSNotificationCenter, HLNotificationCenter>::proto = NULL;

std::map<JSObject*, JSNotificationWrapper*> notify_wrapper_map;

void JSNotificationCenter::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
{
    if (!jsclass)
    {
        jsclass = (JSClass *)calloc(1, sizeof(JSClass));
    }
	jsclass->name = name;
	jsclass->addProperty = JS_PropertyStub;
	jsclass->delProperty = JS_DeletePropertyStub;
	jsclass->getProperty = JS_PropertyStub;
	jsclass->setProperty = JS_StrictPropertyStub;
	jsclass->enumerate = JS_EnumerateStub;
	jsclass->resolve = JS_ResolveStub;
	jsclass->convert = JS_ConvertStub;
	jsclass->finalize = jsFinalize;
	jsclass->flags = JSCLASS_HAS_PRIVATE;
    
    static JSPropertySpec properties[] = {
        {0, 0, 0, 0, 0}
    };
    
    static JSFunctionSpec funcs[] = {
        JS_FN("postNotification", JSNotificationCenter::jsPostNotification, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("addObserver", JSNotificationCenter::jsAddObserver, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("removeObserver", JSNotificationCenter::jsRemoveObserver, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FN("defaultCenter", JSNotificationCenter::jsDefaultCenter, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    proto = JS_InitClass(cx,globalObj,NULL,jsclass,jsNoConstructor,0,properties,funcs,NULL,st_funcs);
}

JSBool JSNotificationCenter::jsDefaultCenter(JSContext *cx, uint32_t argc, jsval *vp)
{
    HLNotificationCenter *center = HLNotificationCenter::defaultCenter();
    JSObject *o = JSNotificationCenter::getOrCreateWrapper(cx, center)->jsobject;
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(o));
    return JS_TRUE;
}

JSBool JSNotificationCenter::jsPostNotification(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = JS_THIS_OBJECT(cx, vp);
    HLNotificationCenter* thiz = ((JSNotificationCenter*)JS_GetPrivate(obj))->getNativeObject();
    if (argc == 1)
    {
        if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
        {
            JSString *s = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
            char* name = JS_EncodeStringToUTF8(cx, s);
            thiz->postNotification(name, NULL);
            JS_free(cx, name);
            return JS_TRUE;
        }
    }
    else if (argc == 2)
    {
        if (JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
        {
            JSString *s = JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]);
            char* name = JS_EncodeStringToUTF8(cx, s);
            thiz->postNotification(name, jsval_to_HLObject(cx, JS_ARGV(cx, vp)[1]));
            JS_free(cx, name);
            return JS_TRUE;
        }
    }
    return JS_FALSE;
}

JSBool JSNotificationCenter::jsAddObserver(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = JS_THIS_OBJECT(cx, vp);
    HLNotificationCenter* thiz = ((JSNotificationCenter*)JS_GetPrivate(obj))->getNativeObject();
    if (argc == 2)
    {
        char* name = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
        JSObject* o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]);
        JS::RootedValue prop(cx);
        if (JS_GetProperty(cx, o, "onNotification", &prop))
        {
            if (JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(prop)))
            {
                std::map<JSObject*, JSNotificationWrapper*>::iterator itr = notify_wrapper_map.find(o);
                JSNotificationWrapper *wrapper = NULL;
                if (itr == notify_wrapper_map.end())
                {
                    wrapper = new JSNotificationWrapper(o);
                    notify_wrapper_map.insert(std::make_pair(o, wrapper));
                }
                else
                {
                    wrapper = itr->second;
                }
                thiz->addObserver(name, wrapper);
                JS_free(cx, name);
                return JS_TRUE;
            }
        }
        JS_free(cx, name);
        JS_ReportError(cx, "please check object has function named onNotification");
    }
    return JS_FALSE;
}

JSBool JSNotificationCenter::jsRemoveObserver(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = JS_THIS_OBJECT(cx, vp);
    HLNotificationCenter* thiz = ((JSNotificationCenter*)JS_GetPrivate(obj))->getNativeObject();
    if (argc == 1)
    {
        JSObject* o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]);
        JS::RootedValue prop(cx);
        if (JS_GetProperty(cx, o, "onNotification", &prop))
        {
            if (JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(prop)))
            {
                std::map<JSObject*, JSNotificationWrapper*>::iterator itr = notify_wrapper_map.find(o);
                JSNotificationWrapper *wrapper = NULL;
                if (itr == notify_wrapper_map.end())
                {
                    return JS_TRUE;
                }
                else
                {
                    wrapper = itr->second;
                }
                notify_wrapper_map.erase(itr);
                thiz->removeObserver(wrapper);
                delete wrapper;
                return JS_TRUE;
            }
        }
    }
    else if (argc == 2)
    {
        char* name = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
        JSObject* o = JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]);
        JS::RootedValue prop(cx);
        if (JS_GetProperty(cx, o, "onNotification", &prop))
        {
            if (JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(prop)))
            {
                std::map<JSObject*, JSNotificationWrapper*>::iterator itr = notify_wrapper_map.find(o);
                JSNotificationWrapper *wrapper = NULL;
                if (itr == notify_wrapper_map.end())
                {
                    return JS_TRUE;
                }
                else
                {
                    wrapper = itr->second;
                }
                thiz->removeObserver(name, wrapper);
                std::multimap<std::string, HLNotificationInterface*>::iterator observeritr = thiz->_observers.begin();
                bool found = false;
                for (; observeritr != thiz->_observers.end(); ++itr)
                {
                    if (observeritr->second == wrapper)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    notify_wrapper_map.erase(itr);
                    delete wrapper;
                }
                JS_free(cx, name);
                return JS_TRUE;
            }
        }
        JS_free(cx, name);
    }
    return JS_FALSE;
}

void JSNotificationWrapper::onNotification(HLNotification * message)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
    JSObject* msg = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue name(cx, STRING_TO_JSVAL(JS_NewStringCopyN(cx, message->name.c_str(), message->name.length())));
    JS_SetProperty(cx, msg, "name", name);
    if (message->userInfo)
    {
        JS::RootedValue userInfo(cx, HLObject_to_jsval(cx, message->userInfo));
        JS_SetProperty(cx, msg, "userInfo", userInfo);
    }
    
    jsval arg = OBJECT_TO_JSVAL(msg);
    jsval rval;
    JS_CallFunctionName(cx, obj, "onNotification", 1, &arg, &rval);
}

NS_HL_END
