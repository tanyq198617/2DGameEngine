//
//  JSUserDefaults.cpp
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-11-23.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSUserDefaults.h"
#include "HLUserDefaults.h"
#include "JSConversions.h"

NS_HL_BEGIN

void JSUserDefaults::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
{
    JSObject* obj = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue val(cx, OBJECT_TO_JSVAL(obj));
    JS_SetProperty(cx, globalObj, name, val);
    
    JS_DefineFunction(cx, obj, "getBoolForKey", JSUserDefaults::jsGetBoolForKey, 2, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "getIntForKey", JSUserDefaults::jsGetIntForKey, 2, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "getFloatForKey", JSUserDefaults::jsGetFloatForKey, 2, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "getStringForKey", JSUserDefaults::jsGetStringForKey, 2, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "setBoolForKey", JSUserDefaults::jsSetBoolForKey, 2, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "setIntForKey", JSUserDefaults::jsSetIntForKey, 2, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "setFloatForKey", JSUserDefaults::jsSetFloatForKey, 2, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "setStringForKey", JSUserDefaults::jsSetStringForKey, 2, JSPROP_READONLY | JSPROP_READONLY);
}

JSBool JSUserDefaults::jsGetBoolForKey(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSString *arg0;
		JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        bool res = HLUserDefaults::getSingleton()->getBoolForKey(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(res));
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSString *arg0;
        JSBool arg1;
		JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "Sb", &arg0, &arg1);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        bool res = HLUserDefaults::getSingleton()->getBoolForKey(s, arg1);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(res));
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for jsGetBoolForKey");
    return JS_FALSE;
}

JSBool JSUserDefaults::jsGetIntForKey(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSString *arg0;
		JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        int res = HLUserDefaults::getSingleton()->getIntForKey(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(res));
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSString *arg0;
        int32_t arg1;
		JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "Si", &arg0, &arg1);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        int res = HLUserDefaults::getSingleton()->getIntForKey(s, arg1);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, INT_TO_JSVAL(res));
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for jsGetIntForKey");
    return JS_FALSE;
}

JSBool JSUserDefaults::jsGetFloatForKey(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSString *arg0;
		JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        float res = HLUserDefaults::getSingleton()->getFloatForKey(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(res));
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSString *arg0;
        double arg1;
		JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "Sd", &arg0, &arg1);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        double res = HLUserDefaults::getSingleton()->getFloatForKey(s, arg1);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(res));
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for jsGetFloatForKey");
    return JS_FALSE;
}

JSBool JSUserDefaults::jsGetStringForKey(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSString *arg0;
		JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        std::string res = HLUserDefaults::getSingleton()->getStringForKey(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, value_to_jsval(res));
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        JSString *arg0;
        JSString *arg1;
		JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "SS", &arg0, &arg1);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        char* s1 = JS_EncodeStringToUTF8(cx, arg1);
        std::string res = HLUserDefaults::getSingleton()->getStringForKey(s, s1);
        JS_free(cx, s);
        JS_free(cx, s1);
        JS_SET_RVAL(cx, vp, value_to_jsval(res));
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for jsGetStringForKey");
    return JS_FALSE;
}

JSBool JSUserDefaults::jsSetBoolForKey(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        JSString *arg0;
        JSBool arg1;
		JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "Sb", &arg0, &arg1);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        HLUserDefaults::getSingleton()->setBoolForKey(s, arg1);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for jsSetBoolForKey");
    return JS_FALSE;
}

JSBool JSUserDefaults::jsSetIntForKey(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        JSString *arg0;
        int32_t arg1;
		JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "Si", &arg0, &arg1);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        HLUserDefaults::getSingleton()->setIntForKey(s, arg1);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for jsSetIntForKey");
    return JS_FALSE;
}

JSBool JSUserDefaults::jsSetFloatForKey(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        JSString *arg0;
        double arg1;
		JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "Sd", &arg0, &arg1);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        HLUserDefaults::getSingleton()->setFloatForKey(s, arg1);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for jsSetFloatForKey");
    return JS_FALSE;
}

JSBool JSUserDefaults::jsSetStringForKey(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        JSString *arg0;
        JSString *arg1;
		JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "SS", &arg0, &arg1);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        char* s1 = JS_EncodeStringToUTF8(cx, arg1);
        HLUserDefaults::getSingleton()->setStringForKey(s, s1);
        JS_free(cx, s);
        JS_free(cx, s1);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for jsSetStringForKey");
    return JS_FALSE;
}

NS_HL_END