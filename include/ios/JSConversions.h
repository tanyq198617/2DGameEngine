//
//  JSConversions.h
//  HoolaiEngine
//
//  Created by zhao shuan on 1/4/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __JSCONVERSIONS_H__
#define __JSCONVERSIONS_H__

#include "HLMacros.h"
#include "HLGeometry.h"
#include "HLEventType.h"
#include "jsapi.h"
#include "HLUtil.h"
#include "JSCPPWrapper.h"
#include <google/protobuf/message_lite.h>
#include "HLViewDND.h"
#include <list>

struct kmMat4;

NS_HL_BEGIN

class HLEntity;

class HLProgram;

template<typename ClassType, typename NativeType>
jsval value_to_jsval1(NativeType* value)
{
    if (!value)
        return JSVAL_NULL;
    if (!ClassType::jsclass || !ClassType::proto)
        return JSVAL_NULL;
    JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
    return OBJECT_TO_JSVAL(ClassType::getOrCreateWrapper(cx, value)->jsobject);
}

template<typename ClassType, typename NativeType>
NativeType* jsval_to_value1(jsval value)
{
    if (!ClassType::jsclass || !ClassType::proto)
        return nullptr;
    ClassType* wrapper = static_cast<ClassType*>(JS_GetPrivate(JSVAL_TO_OBJECT(value)));
    if (!wrapper)
        return nullptr;
    return wrapper->getNativeObject();
}

template<typename T>
jsval value_to_jsval(T value)
{
    return JSVAL_NULL;
}

template<typename T>
T jsval_to_value(jsval value)
{
 //   T val;
 //   return val;
}

class HLEntityManager;
class HLEntity;
class HLTweenSystem;
class HLTexture;
class HLSpriteFrame;
class HLSceneManager;
class HLObject;
class HLTimer;
class HLHttpRequest;

namespace gui {
    class HLView;
    class HLWidget;
    class HLLabel;
    class HLImageView;
    class HLButton;
    class HLViewDragSource;
    class HLAlertView;
    class HLScrollView;
}

template<>
jsval value_to_jsval<int32_t>(int32_t value);
template<>
jsval value_to_jsval<uint32_t>(uint32_t value);
template<>
jsval value_to_jsval<bool>(bool value);
template<>
jsval value_to_jsval<float>(float value);
template<>
jsval value_to_jsval<double>(double value);
template<>
jsval value_to_jsval<HLPoint>(HLPoint value);
template<>
jsval value_to_jsval<HLSize>(HLSize value);
template<>
jsval value_to_jsval<HLRect>(HLRect value);
template<>
jsval value_to_jsval<color4B>(color4B value);
template<>
jsval value_to_jsval<color4F>(color4F value);
template<>
jsval value_to_jsval<HLTouch>(HLTouch value);
template<>
jsval value_to_jsval<HLTouchEvent*>(HLTouchEvent* value);
template<>
jsval value_to_jsval<gui::HLViewDraggingEvent>(gui::HLViewDraggingEvent value);
template<>
jsval value_to_jsval<const char *>(const char * value);
template<>
jsval value_to_jsval<std::string>(std::string value);
template<>
jsval value_to_jsval<HLEntityManager*>(HLEntityManager* value);
template<>
jsval value_to_jsval<HLEntity*>(HLEntity* value);
template<>
jsval value_to_jsval<HLTweenSystem*>(HLTweenSystem* value);
template<>
jsval value_to_jsval<HLTexture*>(HLTexture* value);
template<>
jsval value_to_jsval<HLSpriteFrame*>(HLSpriteFrame* value);
template<>
jsval value_to_jsval<HLSceneManager*>(HLSceneManager* value);
template<>
jsval value_to_jsval<HLTimer*>(HLTimer* value);
template<>
jsval value_to_jsval<gui::HLView*>(gui::HLView* value);
template<>
jsval value_to_jsval<gui::HLButton*>(gui::HLButton* value);
template<>
jsval value_to_jsval<gui::HLWidget*>(gui::HLWidget* value);
template<>
jsval value_to_jsval<gui::HLLabel*>(gui::HLLabel* value);
template<>
jsval value_to_jsval<gui::HLImageView*>(gui::HLImageView* value);
template<>
jsval value_to_jsval<gui::HLViewDragSource*>(gui::HLViewDragSource* value);
template<>
jsval value_to_jsval< ::google::protobuf::MessageLite* >(::google::protobuf::MessageLite* value);
template<>
jsval value_to_jsval<gui::HLAlertView*>(gui::HLAlertView* value);
template<>
jsval value_to_jsval<gui::HLScrollView*>(gui::HLScrollView* value);
template<>
jsval value_to_jsval<HLHttpRequest*>(HLHttpRequest* value);
template<>
jsval value_to_jsval<std::vector<char>>(std::vector<char> value);
template<>
jsval value_to_jsval<std::vector<uint8_t>>(std::vector<uint8_t> value);
template<>
jsval value_to_jsval<void*>(void* value);
template<>
jsval value_to_jsval<std::list<HLEntity*>>(std::list<HLEntity*> value);
template<>
jsval value_to_jsval<std::map<std::string, std::string>>(std::map<std::string, std::string> value);

template<>
int32_t jsval_to_value<int32_t>(jsval value);
template<>
uint32_t jsval_to_value<uint32_t>(jsval value);
template<>
bool jsval_to_value<bool>(jsval value);
template<>
float jsval_to_value<float>(jsval value);
template<>
double jsval_to_value<double>(jsval value);
template<>
HLPoint jsval_to_value<HLPoint>(jsval value);
template<>
HLSize jsval_to_value<HLSize>(jsval value);
template<>
HLRect jsval_to_value<HLRect>(jsval value);
template<>
color4B jsval_to_value<color4B>(jsval value);
template<>
color4F jsval_to_value<color4F>(jsval value);
template<>
HLTouch jsval_to_value<HLTouch>(jsval value);
template<>
HLTouchEvent jsval_to_value<HLTouchEvent>(jsval value);
template<>
std::string jsval_to_value<std::string>(jsval value);
template<>
HLEntityManager* jsval_to_value<HLEntityManager*>(jsval value);
template<>
HLEntity* jsval_to_value<HLEntity*>(jsval value);
template<>
HLTweenSystem* jsval_to_value<HLTweenSystem*>(jsval value);
template<>
HLTexture* jsval_to_value<HLTexture*>(jsval value);
template<>
HLSpriteFrame* jsval_to_value<HLSpriteFrame*>(jsval value);
template<>
HLSceneManager* jsval_to_value<HLSceneManager*>(jsval value);
template<>
HLTimer* jsval_to_value<HLTimer*>(jsval value);
template<>
gui::HLView* jsval_to_value<gui::HLView*>(jsval value);
template<>
gui::HLButton* jsval_to_value<gui::HLButton*>(jsval value);
template<>
gui::HLWidget* jsval_to_value<gui::HLWidget*>(jsval value);
template<>
gui::HLLabel* jsval_to_value<gui::HLLabel*>(jsval value);
template<>
gui::HLImageView* jsval_to_value<gui::HLImageView*>(jsval value);
template<>
gui::HLViewDragSource* jsval_to_value<gui::HLViewDragSource*>(jsval value);
template<>
gui::HLAlertView* jsval_to_value<gui::HLAlertView*>(jsval value);
template<>
gui::HLScrollView* jsval_to_value<gui::HLScrollView*>(jsval value);
template<>
HLHttpRequest* jsval_to_value<HLHttpRequest*>(jsval value);
template<>
std::vector<char> jsval_to_value<std::vector<char>>(jsval value);
template<>
std::vector<uint8_t> jsval_to_value<std::vector<uint8_t>>(jsval value);
template<>
std::list<HLEntity*> jsval_to_value<std::list<HLEntity*>>(jsval value);
template<>
std::vector<HLPoint> jsval_to_value<std::vector<HLPoint>>(jsval value);
//{
//    std::vector<HLPoint> ret;
//    return ret;
//}
template<>
HLProgram* jsval_to_value<HLProgram*>(jsval value);
//{
//    return NULL;
//}
template<>
kmMat4* jsval_to_value<kmMat4*>(jsval value);
//{
//    return NULL;
//}

HLPoint jsval_to_HLPoint(JSContext *cx, jsval v);

jsval HLPoint_to_jsval(JSContext *cx, const HLPoint& p);

HLSize jsval_to_HLSize(JSContext *cx, jsval v);

jsval HLSize_to_jsval(JSContext *cx, const HLSize& s);

HLRect jsval_to_HLRect(JSContext *cx, jsval v);

jsval HLRect_to_jsval(JSContext *cx, const HLRect& r);

jsval HLTouch_to_jsval(JSContext *cx, const HLTouch& t);

HLTouch jsval_to_HLTouch(JSContext *cx, jsval v);

HLTouchEvent jsval_to_HLTouchEvent(JSContext *cx, jsval v);

jsval HLViewDraggingEvent_to_jsval(JSContext *cx, const gui::HLViewDraggingEvent& value);

#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
jsval HLMouseEvent_to_jsval(JSContext *cx, const HLMouseEvent& m);
HLMouseEvent jsval_to_HLMouseEvent(JSContext* cx, jsval v);
#endif

color4F jsval_to_color(JSContext *cx, jsval v);

jsval color_to_jsval(JSContext *cx, const color4F& color);
jsval color_to_jsval(JSContext *cx, const color4B& color);

jsval HLObject_to_jsval(JSContext *cx,  HLObject* o);
HLObject* jsval_to_HLObject(JSContext *cx, jsval v);

NS_HL_END

#endif 
