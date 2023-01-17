//
//  ScriptingCore.h
//  testmonkey
//
//  Created by Rolando Abarca on 3/14/12.
//  Copyright (c) 2012 Zynga Inc. All rights reserved.
//

#ifndef __JSSCRIPTINGCORE_H__
#define __JSSCRIPTINGCORE_H__

#include "jsapi.h"
#include "HLMacros.h"
#include "HLNotificationCenter.h"
#include <string>
#include <time.h>

typedef void(*RegJSTypeFunc)(JSContext *cx, JSObject *parent);

NS_HL_BEGIN

class HLTimer;

class JSScriptingCore
{
private:
    JSRuntime *rt;
    JSContext *cx;
    JSObject  *global;
    JSObject  *debugGlobal;
    static JSScriptingCore* instance;

    JSScriptingCore();
    
    void start();
    
#ifdef DEBUG
    static bool debugAlert;
#endif
public:
    ~JSScriptingCore();
    
    void reset();
    
    static JSScriptingCore* getSingleton(void)
    {
        if (!instance)
            instance = new JSScriptingCore();
        return instance;
    }
    
    static void terminate()
    {
        if (instance)
        {
            delete instance;
        }
    }
    
    static void showDebugAerlt(bool flag)
    {
#ifdef DEBUG
        debugAlert = flag;
#endif
    }

    /**
     * will eval the specified string
     * @param string The string with the javascript code to be evaluated
     * @param outVal The jsval that will hold the return value of the evaluation.
     * Can be NULL.
     */
    bool evalString(const char *string, jsval *outVal);

    /**
     * will run the specified string
     * @param string The path of the script to be run
     */
    void runScript(const char *path);
    
    void runScript(JSObject  *jsobj, const char *path);
    

    JSScript* compileScript(JSObject* jsobj, const char *path);


    /**
     * @return the global context
     */
    JSContext* getGlobalContext() 
    {
        return cx;
    }
    
    JSObject* getGlobalJSObject() 
    {
        return global;
    }
    
    void debugProcessInput(const std::string& str);
	void enableDebugger(const char* tag = NULL);
	JSObject* getDebugGlobal() { return debugGlobal; }

    /**
     * @param cx
     * @param message
     * @param report
     */
    static void reportError(JSContext *cx, const char *message, JSErrorReport *report);

    /**
     * Log something
     * @param cx
     * @param argc
     * @param vp
     */
    static JSBool log(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc > 0) {
            JSString *string = NULL;
            JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &string);
            if (string) {
                time_t t = time(0);
                static char tmp[64];
                strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %z",localtime(&t) );
                char *cstr = JS_EncodeStringToUTF8(cx, string);
                HLLOG("%s    %s", tmp, cstr);
                JS_free(cx, cstr);
            }
        }
        return JS_TRUE;
    };

    /**
     * run a script from script :)
     */
    static JSBool executeScript(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1) {
            JSString *string;
            if (JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &string) == JS_TRUE) {
                char * s = JS_EncodeStringToUTF8(cx, string);
                JSScriptingCore::getSingleton()->runScript(s);
                JS_free(cx, s);
            }
        }
        if (argc == 2) {
            JSString *string;
            JSObject *object;
            if (JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "oS", &object, &string) == JS_TRUE) {
                char * s = JS_EncodeStringToUTF8(cx, string);
                JSScriptingCore::getSingleton()->runScript(object, s);
                JS_free(cx, s);
            }
        }
        return JS_TRUE;
    };

    /**
     * Register an object as a member of the GC's root set, preventing
     * them from being GC'ed
     */
    static JSBool addRootJS(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1) {
            JSObject *o = NULL;
            if (JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &o) == JS_TRUE) {
                if (JS_AddObjectRoot(cx, &o) == JS_FALSE) {
                    HLLOG("something went wrong when setting an object to the root");
                }
            }
        }
        return JS_TRUE;
    };

    /**
     * removes an object from the GC's root, allowing them to be GC'ed if no
     * longer referenced.
     */
    static JSBool removeRootJS(JSContext *cx, uint32_t argc, jsval *vp)
    {
        if (argc == 1) {
            JSObject *o = NULL;
            if (JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &o) == JS_TRUE) {
                JS_RemoveObjectRoot(cx, &o);
            }
        }
        return JS_TRUE;
    };

    /**
     * Force a cycle of GC
     * @param cx
     * @param argc
     * @param vp
     */
    static JSBool forceGC(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JSRuntime *rt = JS_GetRuntime(cx);
        JS_GC(rt);
        return JS_TRUE;
    };
    
    static JSBool maybeGC(JSContext *cx, uint32_t argc, jsval *vp)
    {
        JS_MaybeGC(cx);
        return JS_TRUE;
    };
private:
    void update(HLTimer*, float);
};

NS_HL_END

#endif
