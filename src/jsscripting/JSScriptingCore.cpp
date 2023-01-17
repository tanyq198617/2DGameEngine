//
//  JSScriptingCore.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-25.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "JSScriptingCore.h"
#include "HLObject.h"
#include "HLFileUtil.h"
#include "JSEntitySystem.h"
#include "JSTexture.h"
#include "JSSpriteFrame.h"
#include "JSDirector2D.h"
#include "JSNotificationCenter.h"
#include "JSSceneManager.h"
#include "JSAnimationSystem.h"
#include "JSGUIManager.h"
#include "JSView.h"
#include "JSWidget.h"
#include "JSImageView.h"
#include "JSButton.h"
#include "JSLabel.h"
#include "JSRichLabel.h"
#include "JSToast.h"
#include "JSAlertView.h"
#include "JSScrollView.h"
#include "JSViewLoader.h"
#include "JSTimer.h"
#include "JSToolTip.h"
#include "JSEditBox.h"
#include "JSViewDND.h"
#include "JSProgressBar.h"
#include "JSProgressRadialView.h"
#include "JSSceneView.h"
#include "JSResourceManager.h"
#include "JSUserDefaults.h"
#include "JSApplication.h"
#include "JSHttpRequest.h"
#include "JSTouchDispatcher.h"
#include "GameEngineConfig.h"
#include "OpenThreads/Mutex"
#include "js/OldDebugAPI.h"
#include "jsfriendapi.h"
#include <sstream>
#if defined(TARGET_WIN32)
#include <io.h>
#include <WS2tcpip.h>
#define close closesocket
#else
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#endif
#include <pthread.h>
#include <thread>

#define JSB_DEBUGGER_PORT 5086

extern RegJSTypeFunc jsTypeRegFuncs[];

NS_HL_BEGIN

static std::string inData;
static std::string outData;
static std::vector<std::string> g_queue;
static OpenThreads::Mutex g_qMutex;
static OpenThreads::Mutex g_rwMutex;
static int clientSocket = -1;
static uint32_t s_nestedLoopLevel = 0;

static void* serverEntryPoint(void*);

std::map<void *, void *> jsobjmap;

void sc_finalize(JSFreeOp *freeOp, JSObject *obj) {
    return;
}

static void ReportException(JSContext *cx) {
    if (JS_IsExceptionPending(cx))
    {
        if (!JS_ReportPendingException(cx))
        {
            JS_ClearPendingException(cx);
        }
    }
}

JSScriptingCore* JSScriptingCore::instance = NULL;
#ifdef DEBUG
bool JSScriptingCore::debugAlert = true;
#endif

static JSClass global_class = {
    "global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_DeletePropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, sc_finalize,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

void registTouchEventType(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue touchesBegin(cx, INT_TO_JSVAL(1));
    JS_SetProperty(cx, eventType, "TouchesBegin", touchesBegin);
    JS::RootedValue touchesMove(cx, INT_TO_JSVAL(2));
    JS_SetProperty(cx, eventType, "TouchesMove", touchesMove);
    JS::RootedValue touchesEnd(cx, INT_TO_JSVAL(3));
    JS_SetProperty(cx, eventType, "TouchesEnd", touchesEnd);
    JS::RootedValue touchesCancel(cx);
    touchesCancel = INT_TO_JSVAL(4);
    JS_SetProperty(cx, eventType, "TouchesCancel", touchesCancel);
    
    JS::RootedValue val(cx, OBJECT_TO_JSVAL(eventType));
    JS_SetProperty(cx, parent, "TouchEventType", val);
}

void registAnimationCurveType(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue easeInOut(cx, INT_TO_JSVAL(0));
    JS_SetProperty(cx, eventType, "EaseInOut", easeInOut);
    JS::RootedValue easeIn(cx, INT_TO_JSVAL(1));
    JS_SetProperty(cx, eventType, "EaseIn", easeIn);
    JS::RootedValue easeOut(cx, INT_TO_JSVAL(2));
    JS_SetProperty(cx, eventType, "EaseOut", easeOut);
    JS::RootedValue linear(cx, INT_TO_JSVAL(3));
    JS_SetProperty(cx, eventType, "Linear", linear);
    
    JS::RootedValue val(cx, OBJECT_TO_JSVAL(eventType));
    JS_SetProperty(cx, parent, "AnimationCurveType", val);
}

#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
void registMouseEventType(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue mouseDown(cx, INT_TO_JSVAL(1));
    JS_SetProperty(cx, eventType, "MouseDown", mouseDown);
    JS::RootedValue mouseMove(cx, INT_TO_JSVAL(2));
    JS_SetProperty(cx, eventType, "MouseMove", mouseMove);
    JS::RootedValue mouseDrag(cx, INT_TO_JSVAL(3));
    JS_SetProperty(cx, eventType, "MouseDrag", mouseDrag);
    JS::RootedValue mouseUp(cx, INT_TO_JSVAL(4));
    JS_SetProperty(cx, eventType, "MouseUp", mouseUp);
    JS::RootedValue rightMouseDown(cx, INT_TO_JSVAL(5));
    JS_SetProperty(cx, eventType, "RightMouseDown", rightMouseDown);
    JS::RootedValue rightMouseDrag(cx, INT_TO_JSVAL(6));
    JS_SetProperty(cx, eventType, "RightMouseDrag", rightMouseDrag);
    JS::RootedValue rightMouseUp(cx, INT_TO_JSVAL(7));
    JS_SetProperty(cx, eventType, "RightMouseUp", rightMouseUp);
    
    JS::RootedValue val(cx, OBJECT_TO_JSVAL(eventType));
    JS_SetProperty(cx, parent, "MouseEventType", val);
}
#endif

void registLanguages(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue val(cx);
    val.setInt32(0);
    JS_SetProperty(cx, eventType, "English", val);
    val.setInt32(1);
    JS_SetProperty(cx, eventType, "ChineseTaiwan", val);
    val.setInt32(2);
    JS_SetProperty(cx, eventType, "Chinese", val);
    val.setInt32(3);
    JS_SetProperty(cx, eventType, "French", val);
    val.setInt32(4);
    JS_SetProperty(cx, eventType, "Italian", val);
    val.setInt32(5);
    JS_SetProperty(cx, eventType, "German", val);
    val.setInt32(6);
    JS_SetProperty(cx, eventType, "Spanish", val);
    val.setInt32(7);
    JS_SetProperty(cx, eventType, "Russian", val);
    val.setInt32(8);
    JS_SetProperty(cx, eventType, "Korean", val);
    val.setInt32(9);
    JS_SetProperty(cx, eventType, "Japanese", val);
    val.setInt32(10);
    JS_SetProperty(cx, eventType, "Hungarian", val);
    val.setInt32(11);
    JS_SetProperty(cx, eventType, "Portuguese", val);
    val.setInt32(12);
    JS_SetProperty(cx, eventType, "Arabic", val);
    val.setInt32(9999);
    JS_SetProperty(cx, eventType, "Unknown", val);
    val = OBJECT_TO_JSVAL(eventType);
    JS_SetProperty(cx, parent, "LanguageType", val);
}

void registButtonStateType(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue jNormal(cx, INT_TO_JSVAL(0));
    JS_SetProperty(cx, eventType, "Normal", jNormal);
    JS::RootedValue jSelected(cx, INT_TO_JSVAL(1));
    JS_SetProperty(cx, eventType, "Selected", jSelected);
    JS::RootedValue jHighlighted(cx, INT_TO_JSVAL(2));
    JS_SetProperty(cx, eventType, "Highlighted", jHighlighted);
    JS::RootedValue jDisabled(cx, INT_TO_JSVAL(3));
    JS_SetProperty(cx, eventType, "Disabled", jDisabled);
    JS::RootedValue jButtonStateType(cx, OBJECT_TO_JSVAL(eventType));
    JS_SetProperty(cx, parent, "ButtonStateType", jButtonStateType);
}

void registTextAlignmentType(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue jHLeft(cx, INT_TO_JSVAL(1));
    JS_SetProperty(cx, eventType, "HLeft", jHLeft);
    JS::RootedValue jHCenter(cx, INT_TO_JSVAL(1<<1));
    JS_SetProperty(cx, eventType, "HCenter", jHCenter);
    JS::RootedValue jHRight(cx, INT_TO_JSVAL(1<<2));
    JS_SetProperty(cx, eventType, "HRight", jHRight);
    JS::RootedValue jVTop(cx, INT_TO_JSVAL(1<<3));
    JS_SetProperty(cx, eventType, "VTop", jVTop);
    JS::RootedValue jVCenter(cx, INT_TO_JSVAL(1<<4));
    JS_SetProperty(cx, eventType, "VCenter", jVCenter);
    JS::RootedValue jVBottom(cx, INT_TO_JSVAL(1<<5));
    JS_SetProperty(cx, eventType, "VBottom", jVBottom);
    JS::RootedValue jCenter(cx, INT_TO_JSVAL((1<<1)|(1<<4)));
    JS_SetProperty(cx, eventType, "Center", jCenter);
    JS::RootedValue jTextAlignmentType(cx, OBJECT_TO_JSVAL(eventType));
    JS_SetProperty(cx, parent, "TextAlignmentType", jTextAlignmentType);
}

void registScrollViewDirectionType(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue jHorizontal(cx, INT_TO_JSVAL(1));
    JS_SetProperty(cx, eventType, "Horizontal", jHorizontal);
    JS::RootedValue jVertical(cx, INT_TO_JSVAL(2));
    JS_SetProperty(cx, eventType, "Vertical", jVertical);
    JS::RootedValue jBoth(cx, INT_TO_JSVAL(3));
    JS_SetProperty(cx, eventType, "Both", jBoth);
    JS::RootedValue jScrollViewDirectionType(cx, OBJECT_TO_JSVAL(eventType));
    JS_SetProperty(cx, parent, "ScrollViewDirectionType", jScrollViewDirectionType);
}

void registKeyboardReturnType(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue jDefault(cx, INT_TO_JSVAL(0));
    JS_SetProperty(cx, eventType, "Default", jDefault);
    JS::RootedValue jDone(cx, INT_TO_JSVAL(1));
    JS_SetProperty(cx, eventType, "Done", jDone);
    JS::RootedValue jSend(cx, INT_TO_JSVAL(2));
    JS_SetProperty(cx, eventType, "Send", jSend);
    JS::RootedValue jSearch(cx, INT_TO_JSVAL(3));
    JS_SetProperty(cx, eventType, "Search", jSearch);
    JS::RootedValue jGo(cx, INT_TO_JSVAL(4));
    JS_SetProperty(cx, eventType, "Go", jGo);
    JS::RootedValue jKeyboardReturnType(cx, OBJECT_TO_JSVAL(eventType));
    JS_SetProperty(cx, parent, "KeyboardReturnType", jKeyboardReturnType);
}

void registEditBoxInputMode(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue jAny(cx, INT_TO_JSVAL(0));
    JS_SetProperty(cx, eventType, "Any", jAny);
    JS::RootedValue jEmailAddr(cx, INT_TO_JSVAL(1));
    JS_SetProperty(cx, eventType, "EmailAddr", jEmailAddr);
    JS::RootedValue jNumeric(cx, INT_TO_JSVAL(2));
    JS_SetProperty(cx, eventType, "Numeric", jNumeric);
    JS::RootedValue jPhoneNumber(cx, INT_TO_JSVAL(3));
    JS_SetProperty(cx, eventType, "PhoneNumber", jPhoneNumber);
    JS::RootedValue jUrl(cx, INT_TO_JSVAL(4));
    JS_SetProperty(cx, eventType, "Url", jUrl);
    JS::RootedValue jDecimal(cx, INT_TO_JSVAL(5));
    JS_SetProperty(cx, eventType, "Decimal", jDecimal);
    JS::RootedValue jEditBoxInputMode(cx, OBJECT_TO_JSVAL(eventType));
    JS_SetProperty(cx, parent, "EditBoxInputMode", jEditBoxInputMode);
}

void registViewDraggingEventType(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue jEntered(cx, INT_TO_JSVAL(0));
    JS_SetProperty(cx, eventType, "Entered", jEntered);
    JS::RootedValue jExited(cx, INT_TO_JSVAL(1));
    JS_SetProperty(cx, eventType, "Exited", jExited);
    JS::RootedValue jDropped(cx, INT_TO_JSVAL(2));
    JS_SetProperty(cx, eventType, "Dropped", jDropped);
    JS::RootedValue jAccepted(cx, INT_TO_JSVAL(3));
    JS_SetProperty(cx, eventType, "Accepted", jAccepted);
    JS::RootedValue jCanceled(cx, INT_TO_JSVAL(4));
    JS_SetProperty(cx, eventType, "Canceled", jCanceled);
    JS::RootedValue jViewDraggingEventType(cx, OBJECT_TO_JSVAL(eventType));
    JS_SetProperty(cx, parent, "ViewDraggingEventType", jViewDraggingEventType);
}

void registProgressBarStyleType(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue jNormal(cx, INT_TO_JSVAL(1));
    JS_SetProperty(cx, eventType, "Normal", jNormal);
    JS::RootedValue jImg(cx, INT_TO_JSVAL(1<<1));
    JS_SetProperty(cx, eventType, "Img", jImg);
    JS::RootedValue jRepeat(cx, INT_TO_JSVAL(1<<2));
    JS_SetProperty(cx, eventType, "Repeat", jRepeat);
    JS::RootedValue jProgressBarStyleType(cx, OBJECT_TO_JSVAL(eventType));
    JS_SetProperty(cx, parent, "ProgressBarStyleType", jProgressBarStyleType);
}

void registProgressBarTextStyle(JSContext *cx, JSObject *parent)
{
    JSObject *eventType = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue jNull(cx, INT_TO_JSVAL(0));
    JS_SetProperty(cx, eventType, "Null", jNull);
    JS::RootedValue jPercent(cx, INT_TO_JSVAL(1));
    JS_SetProperty(cx, eventType, "Percent", jPercent);
    JS::RootedValue jValue(cx, INT_TO_JSVAL(2));
    JS_SetProperty(cx, eventType, "Value", jValue);
    JS::RootedValue jProgressBarStyleType(cx, OBJECT_TO_JSVAL(eventType));
    JS_SetProperty(cx, parent, "ProgressBarTextStyle", jProgressBarStyleType);
}

JSObject* NewGlobalObject(JSContext* cx, bool debug = false)
{
	JS::CompartmentOptions options;
    options.setVersion(JSVERSION_LATEST);
    
    JS::RootedObject glob(cx, JS_NewGlobalObject(cx, &global_class, NULL, JS::DontFireOnNewGlobalHook, options));
    if (!glob) {
        return NULL;
    }
    JSAutoCompartment ac(cx, glob);
    JSBool ok = JS_TRUE;
    ok = JS_InitStandardClasses(cx, glob);
    if (ok)
        JS_InitReflect(cx, glob);
    if (ok && debug)
        ok = JS_DefineDebuggerObject(cx, glob);
    if (!ok)
        return NULL;
    
    JS_FireOnNewGlobalObject(cx, glob);
    
    return glob;
}

static JSPrincipals shellTrustedPrincipals = { 1 };

static JSBool
CheckObjectAccess(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JSAccessMode mode,
                  JS::MutableHandleValue vp)
{
    return JS_TRUE;
}

static JSSecurityCallbacks securityCallbacks = {
    CheckObjectAccess,
    NULL
};

JSScriptingCore::JSScriptingCore():debugGlobal(NULL)
{
    start();
}

extern std::map<JSObject*, JSNotificationWrapper*> notify_wrapper_map;

void JSScriptingCore::reset()
{
    for (auto itr = jsobjmap.begin(); itr != jsobjmap.end(); ++itr)
    {
        JSObject* obj = ((JSCppWrapperBase*)itr->second)->jsobject;
        JS_RemoveObjectRoot(cx, &obj);
    }
    jsobjmap.clear();
    
    HLNotificationCenter* notifyCenter = HLNotificationCenter::defaultCenter();
    for (auto itr = notify_wrapper_map.begin(); itr != notify_wrapper_map.end(); ++itr)
    {
        notifyCenter->removeObserver(itr->second);
    }
    notify_wrapper_map.clear();
    JS_DestroyContext(this->cx);
    this->cx = nullptr;
    JS_DestroyRuntime(this->rt);
    this->rt = nullptr;
    JS_ShutDown();
    
    start();
}

void JSScriptingCore::start()
{
    if (!JS_Init())
        return;
    
    //    JS_SetCStringsAreUTF8();
    this->rt = JS_NewRuntime(8L * 1024L * 1024L, JS_USE_HELPER_THREADS);
    JS_SetGCParameter(rt, JSGC_MAX_BYTES, 0xffffffff);
    
    JS_SetTrustedPrincipals(rt, &shellTrustedPrincipals);
    JS_SetSecurityCallbacks(rt, &securityCallbacks);
    JS_SetNativeStackQuota(rt, 500000);
    
    this->cx = JS_NewContext(rt, 8192);
    JS::ContextOptionsRef(cx).setTypeInference(true);
    JS::ContextOptionsRef(cx).setIon(true);
    JS::ContextOptionsRef(cx).setBaseline(true);
    
    
    JS_SetErrorReporter(this->cx, JSScriptingCore::reportError);
    global = NewGlobalObject(this->cx);
    
    JSAutoCompartment ac(this->cx, global);
    js::SetDefaultObjectForContext(this->cx, global);
    
    //    JS::RootedValue globalval(this->cx, OBJECT_TO_JSVAL(global));
    //    JS_SetProperty(this->cx, global, "global", globalval);
    
    // create the hoolai namespace
    JSObject *hoolai = JS_NewObject(this->cx, NULL, NULL, NULL);
    JS::RootedValue hlval(this->cx, OBJECT_TO_JSVAL(hoolai));
    JS_SetProperty(this->cx, global, "hoolai", hlval);
    
    JSEntityManager::jsCreateClass(this->cx, hoolai, "EntityManager");
    JSEntity::jsCreateClass(this->cx, hoolai, "Entity");
    JSTweenSystem::jsCreateClass(this->cx, hoolai, "TweenSystem");
    JSTexture::jsCreateClass(this->cx, hoolai, "Texture");
    JSSpriteFrame::jsCreateClass(this->cx, hoolai, "SpriteFrame");
    JSDirector2D::jsCreateClass(this->cx, hoolai, "Director2D");
    JSSceneManager::jsCreateClass(this->cx, hoolai, "SceneManager");
    JSNotificationCenter::jsCreateClass(this->cx, hoolai, "NotificationCenter");
    JSTimer::jsCreateClass(this->cx, hoolai, "Timer");
    JSSpriteFrameAnimation::jsCreateClass(this->cx, hoolai, "SpriteFrameAnimation");
    JSRepeateAnimation::jsCreateClass(this->cx, hoolai, "RepeateAnimation");
    JSResourceManager::jsCreateClass(this->cx, hoolai, "ResourceManager");
    JSUserDefaults::jsCreateClass(this->cx, hoolai, "UserDefaults");
    JSApplication::jsCreateClass(this->cx, hoolai, "Application");
    JSHttpRequest::jsCreateClass(this->cx, hoolai, "HttpRequest");
    JSTouchDispatcher::jsCreateClass(this->cx, hoolai, "TouchDispatcher");
    
    JSObject *gui = JS_NewObject(cx, NULL, NULL, NULL);
    JS::RootedValue guival(cx, OBJECT_TO_JSVAL(gui));
    JS_SetProperty(cx, hoolai, "gui", guival);
    gui::JSViewLoader::jsCreateClass(this->cx, gui, "ViewLoader");
    gui::JSGUIManager::jsCreateClass(this->cx, gui, "GUIManager");
    gui::JSView::jsCreateClass(this->cx, gui, "View");
    gui::JSWidget::jsCreateClass(this->cx, gui, "Widget");
    gui::JSRootWidget::jsCreateClass(this->cx, gui, "RootWidget");
    gui::JSImageView::jsCreateClass(this->cx, gui, "ImageView");
    gui::JSButton::jsCreateClass(this->cx, gui, "Button");
    gui::JSLabel::jsCreateClass(this->cx, gui, "Label");
    gui::JSRichLabel::jsCreateClass(this->cx, gui, "RichLabel");
    gui::JSToast::jsCreateClass(this->cx, gui, "Toast");
    gui::JSAlertView::jsCreateClass(this->cx, gui, "AlertView");
    gui::JSScrollView::jsCreateClass(this->cx, gui, "ScrollView");
    gui::JSToolTip::jsCreateClass(this->cx, gui, "ToolTip");
    gui::JSEditBox::jsCreateClass(this->cx, gui, "EditBox");
    gui::JSViewDND::jsCreateClass(this->cx, gui, "ViewDND");
    gui::JSProgressBar::jsCreateClass(this->cx, gui, "ProgressBar");
    gui::JSProgressRadialView::jsCreateClass(this->cx, gui, "ProgressRadialView");
    gui::JSSceneView::jsCreateClass(this->cx, gui, "SceneView");
    
    // register some global functions
    //    JS_DefineFunction(this->cx, global, "require", JSScriptingCore::executeScript, 0, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(this->cx, global, "forceGC", JSScriptingCore::forceGC, 0, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(this->cx, global, "maybeGC", JSScriptingCore::maybeGC, 0, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(this->cx, hoolai, "log", JSScriptingCore::log, 0, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(this->cx, global, "executeScript", JSScriptingCore::executeScript, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(this->cx, global, "addGCRootObject", JSScriptingCore::addRootJS, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(this->cx, global, "removeGCRootObject", JSScriptingCore::removeRootJS, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    
    registTouchEventType(this->cx, hoolai);
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    registMouseEventType(this->cx, hoolai);
#endif
    registAnimationCurveType(this->cx, hoolai);
    registButtonStateType(this->cx, hoolai);
    registTextAlignmentType(this->cx, hoolai);
    registScrollViewDirectionType(this->cx, hoolai);
    registKeyboardReturnType(this->cx, hoolai);
    registEditBoxInputMode(this->cx, hoolai);
    registViewDraggingEventType(this->cx, hoolai);
    registProgressBarStyleType(this->cx, hoolai);
    registProgressBarTextStyle(this->cx, hoolai);
    registLanguages(this->cx, hoolai);
    
    for (int i=0 ; ; i++)
    {
        if (jsTypeRegFuncs[i])
        {
            (*jsTypeRegFuncs[i])(this->cx, global);
        }
        else
        {
            break;
        }
    }
    
    runScript("scripts/hoolai/hljs.js");
}

void JSScriptingCore::runScript(const char* path)
{
    runScript(this->global, path);
}

void JSScriptingCore::runScript(JSObject  *jsobj, const char *path)
{
    if (!path)
    {
        return;
    }
    
    JSScript* script = compileScript(jsobj, path);
    
    JSBool ok;
    
    
    if (!script) {
         ReportException(cx);
        HLLOG("error run script: %s", path);
        return;
    }

    jsval rval;
    JSAutoCompartment ac(cx, jsobj);
    ok = JS_ExecuteScript(this->cx, jsobj, script, &rval);
    if (ok == JS_FALSE)
    {
        HLLOG("error run script: %s", path);
        if (!JS_ReportPendingException(cx))
        {
            JS_ClearPendingException(cx);
        }
    }
}

bool JSScriptingCore::evalString(const char *string, jsval *outVal)
{
    jsval rval;
    if (outVal == NULL) {
        outVal = &rval;
    }
    JSAutoCompartment ac(cx, global);
    
    JSScript* script = JS_CompileScript(cx, JS::RootedObject(cx, global), string, strlen(string), JS::CompileOptions(cx));
    if (script)
    {
        bool evaluatedOK = JS_ExecuteScript(cx, global, script, outVal);
        if (!evaluatedOK)
        {
            HLLOG("error evaluating script:\n%s", string);
        }
        return evaluatedOK;
    }
    
    return false;
}


JSScript* JSScriptingCore::compileScript(JSObject* jsobj, const char* path)
{
    if (!path)
    {
        return NULL;
    }
    
    std::string encodedPath = path;
    encodedPath += "b";
    
    JSAutoCompartment ac(cx, jsobj);
    JS::RootedScript script(cx);
    JS::RootedObject obj(cx, global);
    
    if (HLResourceManager::getSingleton()->fileExists(encodedPath.c_str()))
    {
        HLFileData* filedata = HLResourceManager::getSingleton()->getFileData(encodedPath.c_str());
        
        script = JS_DecodeScript(cx, filedata->buffer, (uint32_t)filedata->size, NULL, NULL);
        
        delete filedata;
    }
    else
    {
        HLFileData* filedata = HLResourceManager::getSingleton()->getFileData(path);
        
        if (!filedata)
        {
            HLLOG("file not found: %s", path);
        }
        else
        {
            JS::CompileOptions options(cx);
            options.setUTF8(true).setFileAndLine(path, 1);
            
            script = JS::Compile(cx, obj, options, ( const char *)filedata->buffer, filedata->size);
            
            delete filedata;
        }
    }
    
    return script;
}

JSScriptingCore::~JSScriptingCore()
{
    JS_DestroyContext(cx);
    JS_DestroyRuntime(rt);
    JS_ShutDown();
}

void JSScriptingCore::reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
    JSString* str = JS_NewUCStringCopyZ(cx, report->ucmessage);
    char* msg = JS_EncodeStringToUTF8(cx, str);
    HLLOG("%s:%u:%s\n",
          report->filename ? report->filename : "<no filename=\"filename\">",
          (unsigned int) report->lineno,
          msg);
    
    if (report->linebuf)
    {
        HLLOG("%s", report->linebuf);
    }
    
#ifdef DEBUG
    if (debugAlert)
    {
        hoolai::gui::HLAlertView* alert =
            hoolai::gui::HLAlertView::newAlert("error",
                                                StringUtil::Format("%s:%u:%s",
                                                                    report->filename ? report->filename : "no filename",
                                                                    (unsigned int) report->lineno,
                                                                    msg).c_str(),
                                                "OK", NULL);
        alert->show();
    }
#endif
    JS_free(cx, msg);
    JS_ClearPendingException(cx);
    JS_MaybeGC(cx);
}

void JSScriptingCore::debugProcessInput(const std::string& str)
{
    JSAutoCompartment ac(cx, debugGlobal);
    
    JSString* jsstr = JS_NewStringCopyZ(cx, str.c_str());
    jsval argv = STRING_TO_JSVAL(jsstr);
    jsval outval;
    
    JS_CallFunctionName(cx, debugGlobal, "processInput", 1, &argv, &outval);
}

static bool NS_ProcessNextEvent()
{
    g_qMutex.lock();
    size_t size = g_queue.size();
    g_qMutex.unlock();
    
    while (size > 0)
    {
        g_qMutex.lock();
        auto first = g_queue.begin();
        std::string str = *first;
        g_queue.erase(first);
        size = g_queue.size();
        g_qMutex.unlock();
        
        JSScriptingCore::getSingleton()->debugProcessInput(str);
    }
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    return true;
}

JSBool JSBDebug_enterNestedEventLoop(JSContext* cx, unsigned argc, jsval* vp)
{
    enum {
        NS_OK = 0,
        NS_ERROR_UNEXPECTED
    };
    
#define NS_SUCCEEDED(v) ((v) == NS_OK)
    
    int rv = NS_OK;
    
    uint32_t nestLevel = ++s_nestedLoopLevel;
    
    while (NS_SUCCEEDED(rv) && s_nestedLoopLevel >= nestLevel) {
        if (!NS_ProcessNextEvent())
        rv = NS_ERROR_UNEXPECTED;
    }
    
    HLASSERT(s_nestedLoopLevel <= nestLevel,
             "nested event didn't unwind properly");
    
    JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(s_nestedLoopLevel));
    return JS_TRUE;
}

JSBool JSBDebug_exitNestedEventLoop(JSContext* cx, unsigned argc, jsval* vp)
{
    if (s_nestedLoopLevel > 0) {
        --s_nestedLoopLevel;
    } else {
        JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(0));
        return JS_TRUE;
    }
    
    JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(s_nestedLoopLevel));
    return JS_TRUE;
}

JSBool JSBDebug_getEventLoopNestLevel(JSContext* cx, unsigned argc, jsval* vp)
{
    JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(s_nestedLoopLevel));
    return JS_TRUE;
}

static void _clientSocketWriteAndClearString(std::string& s)
{
    send(clientSocket, s.c_str(), s.length(), 0);
    s.clear();
}

static void processInput(const std::string& data) {
    OpenThreads::ScopedLock<OpenThreads::Mutex> lk(g_qMutex);
    g_queue.push_back(data);
}

static void clearBuffers() {
    OpenThreads::ScopedLock<OpenThreads::Mutex> lk(g_qMutex);
    // only process input if there's something and we're not locked
    if (inData.length() > 0) {
        processInput(inData);
        inData.clear();
    }
    if (outData.length() > 0) {
        _clientSocketWriteAndClearString(outData);
    }
}

static void* serverEntryPoint(void*)
{
    // start a server, accept the connection and keep reading data from it
    struct addrinfo hints, *result = nullptr, *rp = nullptr;
    int s = 0;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    
    std::stringstream portstr;
    portstr << JSB_DEBUGGER_PORT;
    
    int err = 0;
    
#if defined(TARGET_WIN32)
    WSADATA wsaData;
    err = WSAStartup(MAKEWORD(2, 2),&wsaData);
#endif
    
    if ((err = getaddrinfo(NULL, portstr.str().c_str(), &hints, &result)) != 0) {
        HLLOG("getaddrinfo error : %s\n", gai_strerror(err));
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if ((s = socket(rp->ai_family, rp->ai_socktype, 0)) < 0) {
            continue;
        }
        int optval = 1;
        if ((setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval))) < 0) {
            close(s);
			HLLOG("debug server : error setting socket option SO_REUSEADDR");
            return NULL;
        }
        
#if defined(TARGET_IOS)
		if ((setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval))) < 0) {
			close(s);
			HLLOG("debug server : error setting socket option SO_NOSIGPIPE");
			return NULL;
		}
#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        
        if ((bind(s, rp->ai_addr, rp->ai_addrlen)) == 0) {
            break;
        }
        close(s);
        s = -1;
    }
    if (s < 0 || rp == NULL) {
		HLLOG("debug server : error creating/binding socket");
        return NULL;
    }
    
    freeaddrinfo(result);
    
    listen(s, 1);
    
	while (true) {
        clientSocket = accept(s, NULL, NULL);
        
        if (clientSocket < 0)
        {
            HLLOG("debug server : error on accept");
            return NULL;
        }
        else
        {
            // read/write data
            HLLOG("debug server : client connected");
            
            inData = "connected";
            // process any input, send any output
            clearBuffers();
            
            char buf[1024] = {0};
            int readBytes = 0;
            while ((readBytes = recv(clientSocket, buf, sizeof(buf), 0)) > 0)
            {
                buf[readBytes] = '\0';
                // TRACE_DEBUGGER_SERVER("debug server : received command >%s", buf);
                
                // no other thread is using this
                inData.append(buf);
                // process any input, send any output
                clearBuffers();
            } // while(read)
            
            close(clientSocket);
        }
	} // while(true)
    
    return NULL;
}

JSBool JSBDebug_BufferWrite(JSContext* cx, unsigned argc, jsval* vp)
{
    if (argc == 1) {
        JSString *string = NULL;
        JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &string);
        char *cstr = JS_EncodeStringToUTF8(cx, string);
        // this is safe because we're already inside a lock (from clearBuffers)
        outData.append(cstr);
        JS_free(cx, cstr);
        _clientSocketWriteAndClearString(outData);
    }
    return JS_TRUE;
}

void JSScriptingCore::enableDebugger(const char* tag)
{
    if (debugGlobal == NULL)
    {
        JSAutoCompartment ac0(cx, global);
        
        JS_SetDebugMode(cx, JS_TRUE);
        
        debugGlobal = NewGlobalObject(cx, true);
        JS_AddObjectRoot(cx, &debugGlobal);
        JS::RootedObject rootedDebugObj(cx, debugGlobal);
        JS_WrapObject(cx, &rootedDebugObj);
        JSAutoCompartment ac(cx, debugGlobal);
        // these are used in the debug program
        JS_DefineFunction(cx, debugGlobal, "log", JSScriptingCore::log, 0, JSPROP_READONLY | JSPROP_PERMANENT);
        JS_DefineFunction(cx, debugGlobal, "_bufferWrite", JSBDebug_BufferWrite, 1, JSPROP_READONLY | JSPROP_PERMANENT);
        JS_DefineFunction(cx, debugGlobal, "_enterNestedEventLoop", JSBDebug_enterNestedEventLoop, 0, JSPROP_READONLY | JSPROP_PERMANENT);
        JS_DefineFunction(cx, debugGlobal, "_exitNestedEventLoop", JSBDebug_exitNestedEventLoop, 0, JSPROP_READONLY | JSPROP_PERMANENT);
        JS_DefineFunction(cx, debugGlobal, "_getEventLoopNestLevel", JSBDebug_getEventLoopNestLevel, 0, JSPROP_READONLY | JSPROP_PERMANENT);
        if (tag)
        {
            JS::RootedValue val(cx);
            JSString *str = JS_NewStringCopyZ(cx, tag);
            val.setString(str);
            JS_SetProperty(cx, debugGlobal, "_tag", val);
        }
        JSResourceManager::jsCreateClass(cx, debugGlobal, "ResourceManager");
        
        runScript(debugGlobal, "scripts_debugger/js_debugger.js");
        
        // prepare the debugger
        jsval argv = OBJECT_TO_JSVAL(global);
        jsval outval;
        JSBool ok = JS_CallFunctionName(cx, debugGlobal, "_prepareDebugger", 1, &argv, &outval);
        if (!ok) {
            JS_ReportPendingException(cx);
        }
        
        pthread_t thread;
        pthread_create(&thread, NULL, serverEntryPoint, NULL);
        
        HLTimer* timer = new HLTimer(0, true);
        timer->delegate = newDelegate(this, &JSScriptingCore::update);
    }
}

void JSScriptingCore::update(HLTimer*, float)
{
    g_qMutex.lock();
    size_t size = g_queue.size();
    g_qMutex.unlock();
    
    while (size > 0)
    {
        g_qMutex.lock();
        auto first = g_queue.begin();
        std::string str = *first;
        g_queue.erase(first);
        size = g_queue.size();
        g_qMutex.unlock();
        
        JSScriptingCore::getSingleton()->debugProcessInput(str);
    }
}

NS_HL_END
