//
//  JSResourceManager.cpp
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-10-14.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "JSResourceManager.h"
#include "HLResourceManager.h"
#include "JSConversions.h"

NS_HL_BEGIN

void JSResourceManager::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
{
    JSObject* obj = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue val(cx, OBJECT_TO_JSVAL(obj));
    JS_SetProperty(cx, globalObj, name, val);
    
    JS_DefineFunction(cx, obj, "fileExists", JSResourceManager::jsFileExists, 1, JSPROP_READONLY | JSPROP_READONLY);
    JS_DefineFunction(cx, obj, "getUnicodeStringFromFile", JSResourceManager::jsGetUnicodeStringFromFile, 1, JSPROP_READONLY | JSPROP_READONLY);
}

JSBool JSResourceManager::jsFileExists(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSString *arg0;
		JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        bool res = HLResourceManager::getSingleton()->fileExists(s);
        JS_free(cx, s);
        JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(res));
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for jsFileExists");
    return JS_FALSE;
}

JSBool JSResourceManager::jsGetUnicodeStringFromFile(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSString *arg0;
		JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char * s = JS_EncodeStringToUTF8(cx, arg0);
        HLFileData* data = HLResourceManager::getSingleton()->getFileData(s);
        if (data)
        {
            std::string str((char *)data->buffer, data->size);
            JS_SET_RVAL(cx, vp, value_to_jsval(str));
        }
        else
        {
            JS_SET_RVAL(cx, vp, JSVAL_NULL);
        }
        JS_free(cx, s);
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for jsGetUnicodeStringFromFile");
    return JS_FALSE;
}

NS_HL_END