//
//  JSEntitySystem.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-25.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __JSENTITYSYSTEM_H__
#define __JSENTITYSYSTEM_H__

#include "JSCPPWrapper.h"

NS_HL_BEGIN

class JSEntity;

class JSSceneManager;

//extern std::map<std::string, JSEntity *> jsentities;

class HLEntityManager;
class HLEntity;
class HLTweenSystem;

class JSEntityManager: public JSCPPWrapper<JSEntityManager, HLEntityManager>
{
public:
    static JSBool jsCreateEntity(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsCreateAnonymousEntity(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetEntity(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsDestroyEntity(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetSceneManager(JSContext *cx, uint32_t argc, jsval *vp);
    
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
};

class JSEntity: public JSCPPWrapper<JSEntity, HLEntity>
{
public:
    enum {
        kUserInteractionEnabled = 1,
        kVisible,
        kName,
		kPosition,
		kZOrder,
		kAnchorPoint,
		kSize,
        kScale,
		kScaleX,
		kScaleY,
		kSkewX,
		kSkewY,
		kRotation,
		kTransform,
		kParent,
        kChildren,
        kTexture,
        kTextureRotated,
        kTextureRect,
        kGray,
        kFlipX,
        kFlipY,
        kUntrimmedSize,
        kOffsetPosFromBL,
        kBrightness,
        kColor,
        kColorPercent,
        kColorAmount,
        kOutlineFile,
        kSkelfile,
        kView,
        kAlphaTest,
        kStencil,
        kInverted,
        kActive,
        kCameraRect,
        kCameraAnimation,
        kOutlinePoints
	};
    
    static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
	static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    
    static JSBool jsGetProperty(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetProperty(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetEntityManager(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsAddComponent(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsHasComponent(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetUserInteractionEnabled(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetVisible(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsAddScript(JSContext *cx, uint32_t argc, jsval *vp);
    
    static JSBool jsAddChild(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRemoveChild(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRemoveFromParent(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsPlaySkeletonAnimation(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsPauseSkeletonAnimation(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsResumeSkeletonAnimation(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetSkeletonAnimationDelegate(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetSkeletonRegisterPoint(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetSkeletonContainerImageIndex(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetSkeletonContainerImageName(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsPurgeSkeletonTextureCache(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSkeletonAddEntityToBone(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetComponent(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSendMessage(JSContext *cx, uint32_t argc, jsval *vp);
    
    static JSBool jsConvertToNodeSpace(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsConvertToWorldSpace(JSContext *cx, uint32_t argc, jsval *vp);
    
    static JSBool jsAddTouchEventListener(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsRemoveTouchEventListener(JSContext *cx, uint32_t argc, jsval *vp);
    
    static JSBool jsAddParticleComponent(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetBlendAdditive(JSContext *cx, uint32_t argc, jsval *vp);
};

class JSTweenSystem: public JSCPPWrapper<JSTweenSystem, HLTweenSystem>
{
public:
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);
    static JSBool jsStartAnimations(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetAutoReverse(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsCommitAnimations(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetAnimationCurve(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetAnimationCurveRate(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsStopAnimation(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsStopAllAnimations(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetAnimationDidStopCallback(JSContext *cx, uint32_t argc, jsval *vp);
};

NS_HL_END

#endif
