//
//  JSConversions.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/4/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "JSConversions.h"
#include <assert.h>
#include <string>
#include "JSComponent.h"
#include "JSTexture.h"
#include "JSSpriteFrame.h"
#include "JSSceneManager.h"
#include "JSView.h"
#include "JSWidget.h"
#include "JSButton.h"
#include "JSLabel.h"
#include "JSImageView.h"
#include "JSScrollView.h"
#include "JSViewDND.h"
#include "JSTimer.h"
#include "JSAlertView.h"
#include "HLImageView.h"
#include "HLWidget.h"
#include "HLButton.h"
#include "JSHttpRequest.h"
#include "jsfriendapi.h"

NS_HL_BEGIN

extern uint16_t* utf16_from_utf8(const char* str_old);

template<>
jsval value_to_jsval<int32_t>(int32_t value)
{
    return INT_TO_JSVAL(value);
}

template<>
jsval value_to_jsval<uint32_t>(uint32_t value)
{
    return INT_TO_JSVAL(value);
}

template<>
jsval value_to_jsval<bool>(bool value)
{
    return BOOLEAN_TO_JSVAL(value);
}

template<>
jsval value_to_jsval<float>(float value)
{
    return DOUBLE_TO_JSVAL(value);
}

template<>
jsval value_to_jsval<double>(double value)
{
    return DOUBLE_TO_JSVAL(value);
}

template<>
jsval value_to_jsval<HLPoint>(HLPoint value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return HLPoint_to_jsval(cx, value);
}

template<>
jsval value_to_jsval<HLSize>(HLSize value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return HLSize_to_jsval(cx, value);
}

template<>
jsval value_to_jsval<HLRect>(HLRect value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return HLRect_to_jsval(cx, value);
}

template<>
jsval value_to_jsval<HLTouch>(HLTouch value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return HLTouch_to_jsval(cx, value);
}

template<>
jsval value_to_jsval<color4B>(color4B value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSObject* tmp = JS_NewObject(cx, NULL, NULL, NULL);
    if (!tmp)
    {
        return JSVAL_NULL;
    }
    JSBool ok = JS_DefineProperty(cx, tmp, "r", DOUBLE_TO_JSVAL(value.r/255.f), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
    JS_DefineProperty(cx, tmp, "b", DOUBLE_TO_JSVAL(value.b/255.f), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
    JS_DefineProperty(cx, tmp, "g", DOUBLE_TO_JSVAL(value.g/255.f), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
    JS_DefineProperty(cx, tmp, "a", DOUBLE_TO_JSVAL(value.a/255.f), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

template<>
jsval value_to_jsval<color4F>(color4F value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSObject* tmp = JS_NewObject(cx, NULL, NULL, NULL);
    if (!tmp)
    {
        return JSVAL_NULL;
    }
    JSBool ok = JS_DefineProperty(cx, tmp, "r", DOUBLE_TO_JSVAL(value.r), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
    JS_DefineProperty(cx, tmp, "b", DOUBLE_TO_JSVAL(value.b), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
    JS_DefineProperty(cx, tmp, "g", DOUBLE_TO_JSVAL(value.g), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
    JS_DefineProperty(cx, tmp, "a", DOUBLE_TO_JSVAL(value.a), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

template<>
jsval value_to_jsval<HLTouchEvent*>(HLTouchEvent* value)
{
    if (!value)
        return JSVAL_NULL;
    
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();

    JSObject* tmp = JS_NewObject(cx, NULL, NULL, NULL);
    if (!tmp)
    {
        return JSVAL_NULL;
    }
    JS_DefineProperty(cx, tmp, "type", INT_TO_JSVAL(value->type), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    JS_DefineProperty(cx, tmp, "id", INT_TO_JSVAL(value->_id), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    int i = 0;
    int count = (int)value->touches.size();
    jsval *touches = new jsval[count];
    for (i = 0; i < count; i++)
    {
        touches[i] = HLTouch_to_jsval(cx, value->touches[i]);
    }
    JSObject *array = JS_NewArrayObject(cx, count, touches);
    JS_DefineProperty(cx, tmp, "touches", OBJECT_TO_JSVAL(array), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);

    delete[] touches;
    return OBJECT_TO_JSVAL(tmp);
}

template<>
jsval value_to_jsval<gui::HLViewDraggingEvent>(gui::HLViewDraggingEvent value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return HLViewDraggingEvent_to_jsval(cx, value);
}

template<>
jsval value_to_jsval<const char *>(const char * value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    uint16_t* uctxt = utf16_from_utf8(value);
    jsval ret = JSVAL_NULL;
    if (uctxt)
    {
        JSString *s = JS_NewUCStringCopyZ(cx, (jschar*)uctxt);
        ret = STRING_TO_JSVAL(s);
        delete [] uctxt;
    }
    return ret;
}

template<>
jsval value_to_jsval<std::string>(std::string value)
{
    return value_to_jsval(value.c_str());
}

template<>
jsval value_to_jsval<HLEntityManager*>(HLEntityManager* value)
{
    return value_to_jsval1<JSEntityManager, HLEntityManager>(value);
}

template<>
jsval value_to_jsval<HLEntity*>(HLEntity* value)
{
    return value_to_jsval1<JSEntity, HLEntity>(value);
}

template<>
jsval value_to_jsval<HLTweenSystem*>(HLTweenSystem* value)
{
    return value_to_jsval1<JSTweenSystem, HLTweenSystem>(value);
}

template<>
jsval value_to_jsval<HLTexture*>(HLTexture* value)
{
    return value_to_jsval1<JSTexture, HLTexture>(value);
}

template<>
jsval value_to_jsval<HLSpriteFrame*>(HLSpriteFrame* value)
{
    return value_to_jsval1<JSSpriteFrame, HLSpriteFrame>(value);
}

template<>
jsval value_to_jsval<HLSceneManager*>(HLSceneManager* value)
{
    return value_to_jsval1<JSSceneManager, HLSceneManager>(value);
}

template<>
jsval value_to_jsval<HLTimer*>(HLTimer* value)
{
    return value_to_jsval1<JSTimer, HLTimer>(value);
}

template<>
jsval value_to_jsval<gui::HLLabel*>(gui::HLLabel* value)
{
    if (!value)
        return JSVAL_NULL;
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return OBJECT_TO_JSVAL(value->getOrCreateJSWrapper(cx)->jsobject);
}

template<>
jsval value_to_jsval<gui::HLImageView*>(gui::HLImageView* value)
{
    if (!value)
        return JSVAL_NULL;
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return OBJECT_TO_JSVAL(value->getOrCreateJSWrapper(cx)->jsobject);
}

template<>
jsval value_to_jsval<gui::HLView*>(gui::HLView* value)
{
    if (!value)
        return JSVAL_NULL;
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return OBJECT_TO_JSVAL(value->getOrCreateJSWrapper(cx)->jsobject);
}

template<>
jsval value_to_jsval<gui::HLButton*>(gui::HLButton* value)
{
    if (!value)
        return JSVAL_NULL;
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return OBJECT_TO_JSVAL(value->getOrCreateJSWrapper(cx)->jsobject);
}

template<>
jsval value_to_jsval<gui::HLWidget*>(gui::HLWidget* value)
{
    if (!value)
        return JSVAL_NULL;
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return OBJECT_TO_JSVAL(value->getOrCreateJSWrapper(cx)->jsobject);
}

template<>
jsval value_to_jsval<gui::HLViewDragSource*>(gui::HLViewDragSource* value)
{
    return value_to_jsval1<gui::JSViewDND, gui::HLViewDragSource>(value);
}

template<>
jsval value_to_jsval<gui::HLScrollView*>(gui::HLScrollView* value)
{
    return value_to_jsval1<gui::JSScrollView, gui::HLScrollView>(value);
}

template<>
jsval value_to_jsval<gui::HLAlertView*>(gui::HLAlertView* value)
{
    return value_to_jsval1<gui::JSAlertView, gui::HLAlertView>(value);
}

template<>
jsval value_to_jsval<HLHttpRequest*>(HLHttpRequest* value)
{
    return value_to_jsval1<JSHttpRequest, HLHttpRequest>(value);
}

template<>
jsval value_to_jsval<std::vector<char>>(std::vector<char> value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    void * contents = NULL;
    uint8_t * data = NULL;
    if (!JS_AllocateArrayBufferContents(cx, (uint32_t)value.size(), &contents, &data))
    {
        return JSVAL_NULL;
    }
    memcpy(data, &value[0], value.size());
    return OBJECT_TO_JSVAL(JS_NewArrayBufferWithContents(cx, contents));
}

template<>
jsval value_to_jsval<std::vector<uint8_t>>(std::vector<uint8_t> value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    void * contents = NULL;
    uint8_t * data = NULL;
    if (!JS_AllocateArrayBufferContents(cx, (uint32_t)value.size(), &contents, &data))
    {
        return JSVAL_NULL;
    }
    memcpy(data, &value[0], value.size());
    return OBJECT_TO_JSVAL(JS_NewArrayBufferWithContents(cx, contents));
}

template<>
jsval value_to_jsval< ::google::protobuf::MessageLite* >(::google::protobuf::MessageLite* value)
{
    if (value)
    {
        return value->ToJSVAL();
    }
    return JSVAL_NULL;
}

template<>
jsval value_to_jsval<void*>(void* value)
{
    if (value)
    {
        JSObject* obj = (JSObject*)value;
        return OBJECT_TO_JSVAL(obj);
    }
    return JSVAL_NULL;
}

template<>
jsval value_to_jsval<std::list<HLEntity*>>(std::list<HLEntity*> value)
{
    std::vector<jsval> values;
    for (std::list<HLEntity*>::iterator itr = value.begin(); itr != value.end(); ++itr)
    {
        values.push_back(value_to_jsval(*itr));
    }
    JSContext* cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSObject* arr = JS_NewArrayObject(cx, (int)values.size(), &values[0]);
    jsval val;
    val.setObject(*arr);
    return val;
}

template<>
jsval value_to_jsval<std::map<std::string, std::string>>(std::map<std::string, std::string> value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSObject* obj = JS_NewObject(cx, NULL, NULL, NULL);
    for (auto itr=value.begin(); itr != value.end(); ++itr) {
        JS_DefineProperty(cx, obj, itr->first.c_str(), value_to_jsval(itr->second), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    }
    jsval val;
    val.setObject(*obj);
    return val;
}

template<>
int32_t jsval_to_value<int32_t>(jsval value)
{
    return JSVAL_TO_INT(value);
}

template<>
uint32_t jsval_to_value<uint32_t>(jsval value)
{
    return JSVAL_TO_INT(value);
}

template<>
bool jsval_to_value<bool>(jsval value)
{
    return JSVAL_TO_BOOLEAN(value);
}

template<>
float jsval_to_value<float>(jsval value)
{
    return value.toNumber();
}

template<>
double jsval_to_value<double>(jsval value)
{
    return value.toNumber();
}

template<>
HLPoint jsval_to_value<HLPoint>(jsval value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return jsval_to_HLPoint(cx, value);
}

template<>
HLSize jsval_to_value<HLSize>(jsval value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return jsval_to_HLSize(cx, value);
}

template<>
HLRect jsval_to_value<HLRect>(jsval value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return jsval_to_HLRect(cx, value);
}

template<>
color4B jsval_to_value<color4B>(jsval value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    color4F color = jsval_to_color(cx, value);
    color4B ret{static_cast<GLubyte>(color.r*255), static_cast<GLubyte>(color.g*255), static_cast<GLubyte>(color.b*255), static_cast<GLubyte>(color.a*255)};
    return ret;
}

template<>
color4F jsval_to_value<color4F>(jsval value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return jsval_to_color(cx, value);
}

template<>
HLTouch jsval_to_value<HLTouch>(jsval value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return jsval_to_HLTouch(cx, value);
}

template<>
HLTouchEvent jsval_to_value<HLTouchEvent>(jsval value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return jsval_to_HLTouchEvent(cx, value);
}


template<>
std::string jsval_to_value<std::string>(jsval value)
{
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    JSString *s = JSVAL_TO_STRING(value);
    char * str = JS_EncodeStringToUTF8(cx, s);
    std::string ret(str);
    JS_free(cx, str);
    
    return ret;
}

template<>
HLEntityManager* jsval_to_value<HLEntityManager*>(jsval value)
{
    return jsval_to_value1<JSEntityManager, HLEntityManager>(value);
}

template<>
HLEntity* jsval_to_value<HLEntity*>(jsval value)
{
    return jsval_to_value1<JSEntity, HLEntity>(value);
}

template<>
HLTweenSystem* jsval_to_value<HLTweenSystem*>(jsval value)
{
    return jsval_to_value1<JSTweenSystem, HLTweenSystem>(value);
}

template<>
HLTexture* jsval_to_value<HLTexture*>(jsval value)
{
    return jsval_to_value1<JSTexture, HLTexture>(value);
}

template<>
HLSpriteFrame* jsval_to_value<HLSpriteFrame*>(jsval value)
{
    return jsval_to_value1<JSSpriteFrame, HLSpriteFrame>(value);
}

template<>
HLSceneManager* jsval_to_value<HLSceneManager*>(jsval value)
{
    return jsval_to_value1<JSSceneManager, HLSceneManager>(value);
}

template<>
HLTimer* jsval_to_value<HLTimer*>(jsval value)
{
    return jsval_to_value1<JSTimer, HLTimer>(value);
}

template<>
gui::HLView* jsval_to_value<gui::HLView*>(jsval value)
{
    return jsval_to_value1<gui::JSView, gui::HLView>(value);
}

template<>
gui::HLButton* jsval_to_value<gui::HLButton*>(jsval value)
{
    return jsval_to_value1<gui::JSButton, gui::HLButton>(value);
}

template<>
gui::HLWidget* jsval_to_value<gui::HLWidget*>(jsval value)
{
    return jsval_to_value1<gui::JSWidget, gui::HLWidget>(value);
}

template<>
gui::HLLabel* jsval_to_value<gui::HLLabel*>(jsval value)
{
    return jsval_to_value1<gui::JSLabel, gui::HLLabel>(value);
}

template<>
gui::HLImageView* jsval_to_value<gui::HLImageView*>(jsval value)
{
    return jsval_to_value1<gui::JSImageView, gui::HLImageView>(value);
}

template<>
gui::HLViewDragSource* jsval_to_value<gui::HLViewDragSource*>(jsval value)
{
    return jsval_to_value1<gui::JSViewDND, gui::HLViewDragSource>(value);
}

template<>
gui::HLAlertView* jsval_to_value<gui::HLAlertView*>(jsval value)
{
    return jsval_to_value1<gui::JSAlertView, gui::HLAlertView>(value);
}

template<>
gui::HLScrollView* jsval_to_value<gui::HLScrollView*>(jsval value)
{
    return jsval_to_value1<gui::JSScrollView, gui::HLScrollView>(value);
}

template<>
HLHttpRequest* jsval_to_value<HLHttpRequest*>(jsval value)
{
    return jsval_to_value1<JSHttpRequest, HLHttpRequest>(value);
}

template<>
std::vector<char> jsval_to_value<std::vector<char>>(jsval value)
{
    std::vector<char> ret;
    if (!value.isObject()) {
        return ret;
    }
    JSObject* obj = JSVAL_TO_OBJECT(value);
    if (JS_IsArrayBufferObject(obj)) {
        uint32_t len = JS_GetArrayBufferByteLength(obj);
        ret.resize(len);
        memcpy(&ret[0], JS_GetArrayBufferData(obj), len);
    }
    return ret;
}

template<>
std::vector<uint8_t> jsval_to_value<std::vector<uint8_t>>(jsval value)
{
    std::vector<uint8_t> ret;
    if (!value.isObject()) {
        return ret;
    }
    JSObject* obj = JSVAL_TO_OBJECT(value);
    if (JS_IsArrayBufferObject(obj)) {
        uint32_t len = JS_GetArrayBufferByteLength(obj);
        ret.resize(len);
        memcpy(&ret[0], JS_GetArrayBufferData(obj), len);
    }
    return ret;
}

template<>
std::list<HLEntity*> jsval_to_value<std::list<HLEntity*>>(jsval value)
{
    std::list<HLEntity*> ret;
    JSContext* cx = JSScriptingCore::getSingleton()->getGlobalContext();
    if (value.isObject()) {
        JSObject* object = value.toObjectOrNull();
        if (JS_IsArrayObject(cx, object)) {
            uint32_t lengthp;
            JS_GetArrayLength(cx, object, &lengthp);
            for (int i = 0; i < lengthp; ++i)
            {
                JS::RootedValue vp(cx);
                JS_GetElement(cx, object, i, &vp);
                ret.push_back(jsval_to_value<HLEntity*>(vp.get()));
            }
        }
    }
    return ret;
}

template<>
std::vector<HLPoint> jsval_to_value<std::vector<HLPoint>>(jsval value)
{
    std::vector<HLPoint> ret;
    return ret;
}

template<>
HLProgram* jsval_to_value<HLProgram*>(jsval value)
{
    return NULL;
}

template<>
kmMat4* jsval_to_value<kmMat4*>(jsval value)
{
    return NULL;
}

HLPoint jsval_to_HLPoint(JSContext *cx, jsval v)
{
    JSObject *tmp = v.toObjectOrNull();
    assert(tmp);
    JS::RootedValue jsx(cx), jsy(cx);
    JSBool ok = JS_GetProperty(cx, tmp, "x", &jsx) &&
                JS_GetProperty(cx, tmp, "y", &jsy);
    assert(ok == JS_TRUE);
    return HLPoint(jsx.toNumber(), jsy.toNumber());
}

jsval HLPoint_to_jsval(JSContext *cx, const HLPoint& p)
{
    JSObject* tmp = JS_NewObject(cx, NULL, NULL, NULL);
    if (!tmp)
    {
        return JSVAL_NULL;
    }
    JSBool ok = JS_DefineProperty(cx, tmp, "x", DOUBLE_TO_JSVAL(p.x), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "y", DOUBLE_TO_JSVAL(p.y), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

HLSize jsval_to_HLSize(JSContext *cx, jsval v)
{
    JSObject *tmp = v.toObjectOrNull();
    assert(tmp);
    JS::RootedValue jsw(cx), jsh(cx);
    JSBool ok = JS_GetProperty(cx, tmp, "width", &jsw) &&
                JS_GetProperty(cx, tmp, "height", &jsh);
    assert(ok == JS_TRUE);
    return HLSize(jsw.toNumber(), jsh.toNumber());
}

jsval HLSize_to_jsval(JSContext *cx, const HLSize& s)
{
    JSObject* tmp = JS_NewObject(cx, NULL, NULL, NULL);
    if (!tmp)
    {
        return JSVAL_NULL;
    }
    JSBool ok = JS_DefineProperty(cx, tmp, "width", DOUBLE_TO_JSVAL(s.width), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "height", DOUBLE_TO_JSVAL(s.height), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

HLRect jsval_to_HLRect(JSContext *cx, jsval v)
{
    JSObject *tmp = v.toObjectOrNull();
    assert(tmp);
    JS::RootedValue jsx(cx), jsy(cx), jsw(cx), jsh(cx);
    JSBool ok = JS_GetProperty(cx, tmp, "x", &jsx) &&
                JS_GetProperty(cx, tmp, "y", &jsy) &&
                JS_GetProperty(cx, tmp, "width", &jsw) &&
                JS_GetProperty(cx, tmp, "height", &jsh);
    assert(ok == JS_TRUE);
    return HLRect(jsx.toNumber(), jsy.toNumber(), jsw.toNumber(), jsh.toNumber());
}

jsval HLRect_to_jsval(JSContext *cx, const HLRect& r)
{
    JSObject* tmp = JS_NewObject(cx, NULL, NULL, NULL);
    if (!tmp)
    {
        return JSVAL_NULL;
    }
    JSBool ok = JS_DefineProperty(cx, tmp, "x", DOUBLE_TO_JSVAL(r.origin.x), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "y", DOUBLE_TO_JSVAL(r.origin.y), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "width", DOUBLE_TO_JSVAL(r.size.width), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "height", DOUBLE_TO_JSVAL(r.size.height), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

jsval HLTouch_to_jsval(JSContext *cx, const HLTouch& t)
{
    JSObject* tmp = JS_NewObject(cx, NULL, NULL, NULL);
    if (!tmp)
    {
        return JSVAL_NULL;
    }
    JSBool ok = JS_DefineProperty(cx, tmp, "id", INT_TO_JSVAL(t._id), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "location", HLPoint_to_jsval(cx, t.location), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "tapCount", INT_TO_JSVAL(t.tapCount), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

HLTouch jsval_to_HLTouch(JSContext *cx, jsval v)
{
    JSObject *tmp = v.toObjectOrNull();
    assert(tmp);
    JS::RootedValue js1(cx), js2(cx), js3(cx);
    HLPoint location;
    JSBool ok = JS_GetProperty(cx, tmp, "id", &js1) &&
                JS_GetProperty(cx, tmp, "location", &js2) &&
                JS_GetProperty(cx, tmp, "tapCount", &js3);
    location = jsval_to_HLPoint(cx, js2);
    assert(ok == JS_TRUE);
    HLTouch touch;
    touch._id = js1.toInt32();
    touch.tapCount = js3.toInt32();
    touch.location = location;
    return touch;
}

HLTouchEvent jsval_to_HLTouchEvent(JSContext *cx, jsval v)
{
    HLTouchEvent event;
    JSObject* tmp = v.toObjectOrNull();
    JS::RootedValue id(cx), type(cx), touches(cx);
    JSBool ok = JS_GetProperty(cx, tmp, "id", &id) &&
                JS_GetProperty(cx, tmp, "type", &type);
                JS_GetProperty(cx, tmp, "touches", &touches);
    assert(ok == JS_TRUE);
    event._id = id.toInt32();
    event.type = (HLTouchEventType)type.toInt32();
    JSObject* arr = touches.toObjectOrNull();
    assert(arr);
    uint32_t len;
    JS_GetArrayLength(cx, arr, &len);
    for (uint32_t i = 0; i < len; ++i)
    {
        JS::RootedValue v(cx);
        JS_GetElement(cx, arr, i, &v);
        event.touches.push_back(jsval_to_HLTouch(cx, v));
    }
    return event;
}

jsval HLViewDraggingEvent_to_jsval(JSContext *cx, const gui::HLViewDraggingEvent& value)
{
    JSObject* tmp = JS_NewObject(cx, NULL, NULL, NULL);
    if (!tmp)
    {
        return JSVAL_NULL;
    }
    JSBool ok = JS_DefineProperty(cx, tmp, "type", INT_TO_JSVAL(value.type), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT)&&
                JS_DefineProperty(cx, tmp, "location", HLPoint_to_jsval(cx, value.location), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
jsval HLMouseEvent_to_jsval(JSContext *cx, const HLMouseEvent& m)
{
    JSObject* tmp = JS_NewObject(cx, NULL, NULL, NULL);
    if (!tmp)
    {
        return JSVAL_NULL;
    }
    JSBool ok = JS_DefineProperty(cx, tmp, "location", HLPoint_to_jsval(cx, m.location), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "type", INT_TO_JSVAL((int)m.type), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "clickCount", INT_TO_JSVAL(m.clickCount), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

HLMouseEvent jsval_to_HLMouseEvent(JSContext* cx, jsval v)
{
    JSObject *tmp = v.toObjectOrNull();
    assert(tmp);
    JS::RootedValue js1(cx), js2(cx), js3(cx), js4(cx);
    HLPoint location;
    JSBool ok = JS_GetProperty(cx, tmp, "location", &js2) &&
                JS_GetProperty(cx, tmp, "clickCount", &js3) &&
                JS_GetProperty(cx, tmp, "type", &js4);
    location = jsval_to_HLPoint(cx, js2);
    assert(ok == JS_TRUE);
    HLMouseEvent mouse;
    mouse.clickCount = js3.toInt32();
    mouse.location = location;
    mouse.type = (HLMouseEventType)js4.toInt32();
    return mouse;
}
#endif

color4F jsval_to_color(JSContext *cx, jsval v)
{
    JSObject *tmp = v.toObjectOrNull();
    assert(tmp);
    JS::RootedValue jsr(cx), jsg(cx), jsb(cx), jsa(cx);
    JSBool ok = JS_GetProperty(cx, tmp, "r", &jsr) &&
                JS_GetProperty(cx, tmp, "g", &jsg) &&
                JS_GetProperty(cx, tmp, "b", &jsb) &&
                JS_GetProperty(cx, tmp, "a", &jsa);
    assert(ok == JS_TRUE);
    color4F color;
    color.r = jsr.toNumber();
    color.g = jsg.toNumber();
    color.b = jsb.toNumber();
    color.a = jsa.toNumber();
    return color;
}

jsval color_to_jsval(JSContext *cx, const color4F& color)
{
    JSObject* tmp = JS_NewObject(cx, NULL, NULL, NULL);
    if (!tmp)
    {
        return JSVAL_NULL;
    }
    JSBool ok = JS_DefineProperty(cx, tmp, "r", DOUBLE_TO_JSVAL(color.r), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "g", DOUBLE_TO_JSVAL(color.g), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "b", DOUBLE_TO_JSVAL(color.b), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
                JS_DefineProperty(cx, tmp, "a", DOUBLE_TO_JSVAL(color.a), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    if (ok)
    {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

jsval color_to_jsval(JSContext *cx, const color4B& color)
{
    JSObject* tmp = JS_NewObject(cx, NULL, NULL, NULL);
    if (!tmp)
    {
        return JSVAL_NULL;
    }
    JSBool ok = JS_DefineProperty(cx, tmp, "r", DOUBLE_TO_JSVAL(color.r/255.f), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
    JS_DefineProperty(cx, tmp, "g", DOUBLE_TO_JSVAL(color.g/255.f), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
    JS_DefineProperty(cx, tmp, "b", DOUBLE_TO_JSVAL(color.b/255.f), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT) &&
    JS_DefineProperty(cx, tmp, "a", DOUBLE_TO_JSVAL(color.a/255.f), NULL, NULL, JSPROP_ENUMERATE|JSPROP_PERMANENT);
    if (ok) {
        return OBJECT_TO_JSVAL(tmp);
    }
    return JSVAL_NULL;
}

class JSObjectWrapper: public HLObject
{
public:
    JSObjectWrapper(jsval v):jsvalue(v) {_type = (HLObjectType)10;}
    jsval jsvalue;
};

jsval HLObject_to_jsval(JSContext *cx,  HLObject* o)
{
    if (o)
    {
        switch (o->getType())
        {
            case kHLObjectString:
            {
                return value_to_jsval(static_cast<HLString*>(o)->_value.c_str());
            }
                break;
            case kHLObjectNumber:
            {
                HLNumber* num = static_cast<HLNumber*>(o);
                if (num->_numtype == kHLNumberTypeDouble || num->_numtype == kHLNumberTypeFloat)
                {
                    return DOUBLE_TO_JSVAL(num->doubleValue());
                }
                else if (num->_numtype == kHLNumberTypeBool)
                {
                    return BOOLEAN_TO_JSVAL(num->boolValue());
                }
                else if (num->_numtype == kHLNumberTypeInt)
                {
                    return INT_TO_JSVAL(num->intValue());
                }
            }
                break;
            case 10:
            {
                return static_cast<JSObjectWrapper*>(o)->jsvalue;
            }
                break;
            // TODO Array Dictionary
            default:
                break;
        }
    }
    return JSVAL_NULL;
}

HLObject* jsval_to_HLObject(JSContext *cx, jsval v)
{
    if (JSVAL_IS_BOOLEAN(v))
    {
        HLObject* o = new HLNumber((bool)JSVAL_TO_BOOLEAN(v));
        o->autorelease();
        return o;
    }
    if (JSVAL_IS_DOUBLE(v))
    {
        HLObject *o = new HLNumber(JSVAL_TO_DOUBLE(v));
        o->autorelease();
        return o;
    }
    if (JSVAL_IS_INT(v))
    {
        HLObject *o = new HLNumber(JSVAL_TO_INT(v));
        o->autorelease();
        return o;
    }
    if (JSVAL_IS_STRING(v))
    {
        JSString *s = JSVAL_TO_STRING(v);
        char * str = JS_EncodeStringToUTF8(cx, s);
        HLObject *o = new HLString(str);
        o->autorelease();
        JS_free(cx, str);
        return o;
    }
    // TODO Array Dictionary
    if (!JSVAL_IS_PRIMITIVE(v) && !JSVAL_IS_NULL(v))
    {
        JSObjectWrapper* o = new JSObjectWrapper(v);
        o->autorelease();
        return o;
    }
    return NULL;
}

NS_HL_END
