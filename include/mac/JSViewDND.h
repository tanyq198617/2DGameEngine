//
//  JSViewDND.h
//  HoolaiEngine
//
//  Created by cai wei on 26/4/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __JSVIEWDND_H__
#define __JSVIEWDND_H__

#include "HLViewDND.h"
#include "JSCPPWrapper.h"

NS_HL_BEGIN

namespace gui
{
//class HLImageView;
//class HLView;
//class HLWidget;
class HLViewDragSource;

class JSViewDND: public JSCPPWrapper<JSViewDND, HLViewDragSource>
{
public:
    /*enum
    {
        kTag
    };*/

    static JSBool jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val);
    static JSBool jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val);
    
    static JSBool jsConstructor(JSContext *cx, uint32_t argc, jsval *vp);
    static void jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name);

    static JSBool jsDoDragDrop(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetDraggingView(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetDraggingImage(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetDraggingImageTexture(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetDraggingImageSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetUserData(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetUserData(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsSetTag(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetSourceView(JSContext *cx, uint32_t argc, jsval *vp);
    static JSBool jsGetTag(JSContext *cx, uint32_t argc, jsval *vp);
};
}

NS_HL_END

#endif
