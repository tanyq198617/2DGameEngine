//
//  JSTexture.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-29.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "JSSpriteFrame.h"
#include "JSEntitySystem.h"
#include "JSConversions.h"
#include "JSTexture.h"

NS_HL_BEGIN

template<> JSClass* JSCPPWrapper<JSSpriteFrame, HLSpriteFrame>::jsclass = NULL;
template<> JSObject* JSCPPWrapper<JSSpriteFrame, HLSpriteFrame>::proto = NULL;

JSBool JSSpriteFrame::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);
    
    JSSpriteFrame* jtex = (JSSpriteFrame*)JS_GetPrivate(obj);
    
    if (!jtex) return JS_TRUE;
    
    switch (propId)
    {
        case kName:
            val.set(value_to_jsval(jtex->getNativeObject()->_name));
            break;
        case kTextureRect:
            val.set(HLRect_to_jsval(cx, jtex->getNativeObject()->_textureRect));
            break;
        case kSourceSize:
            val.set(HLSize_to_jsval(cx, jtex->getNativeObject()->_sourceSize));
            break;
        case kOffset:
            val.set(HLPoint_to_jsval(cx, jtex->getNativeObject()->_offset));
            break;
        case kTexture:
        {
            HLTexture* tex = jtex->getNativeObject()->_texture;
            if (tex)
            {
                val.setObjectOrNull(JSTexture::getOrCreateWrapper(cx, tex)->jsobject);
            }
            else
            {
                val.setNull();
            }
        }
            break;
        case kRotated:
            val.setBoolean(jtex->getNativeObject()->_rotated);
            break;
        default:
            break;
    }
    
    return JS_TRUE;
}

//JSBool JSSpriteFrame::jsPropertySet(JSContext *cx, JSObject *obj, jsid _id, JSBool strict, jsval *val)
//{
//    return JS_FALSE;
//}

void JSSpriteFrame::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        {"name", kName, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, JSOP_WRAPPER(JSSpriteFrame::jsPropertyGet), 0},
        {"textureRect", kTextureRect, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, JSOP_WRAPPER(JSSpriteFrame::jsPropertyGet), 0},
        {"offset", kOffset, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, JSOP_WRAPPER(JSSpriteFrame::jsPropertyGet), 0},
        {"sourceSize", kSourceSize, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, JSOP_WRAPPER(JSSpriteFrame::jsPropertyGet), 0},
        {"texture", kTexture, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, JSOP_WRAPPER(JSSpriteFrame::jsPropertyGet), 0},
        {"rotated", kRotated, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, JSOP_WRAPPER(JSSpriteFrame::jsPropertyGet), 0},
        {0, 0, 0, 0, 0}
    };
    
    static JSFunctionSpec funcs[] = {
        JS_FN("setDisplayFrameForEntity", JSSpriteFrame::jsSetDisplayFrameForEntity, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("retain", JSSpriteFrame::jsRetain, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("release", JSSpriteFrame::jsRelease, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FN("getSpriteFrameArray", JSSpriteFrame::jsGetSpriteFrameArray, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getSpriteFrameDictionary", JSSpriteFrame::jsGetSpriteFrameDictionary, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getSpriteFrame", JSSpriteFrame::jsGetSpriteFrame, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("purgeCachedData", JSSpriteFrame::jsPurgeCachedData, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    proto = JS_InitClass(cx,globalObj,NULL,jsclass,jsNoConstructor,0,properties,funcs,NULL,st_funcs);
}

JSBool JSSpriteFrame::jsGetSpriteFrameArray(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSString *arg0;
		JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char * s = JS_EncodeStringToUTF8(cx, arg0);
        HLArray *arr = HLSpriteFrame::getSpriteFrameArray(s);
        JS_free(cx, s);
        if (!arr)
        {
            JS_SET_RVAL(cx, vp, JSVAL_NULL);
            return JS_TRUE;
        }
        std::vector<HLObject*>::iterator itr;
        jsval *resarr = new jsval[arr->_value.size()];
        int i = 0;
        for (itr = arr->_value.begin(); itr != arr->_value.end(); itr++) 
        {
            JSSpriteFrame* tmp1 = JSSpriteFrame::getOrCreateWrapper(cx, (HLSpriteFrame *)(*itr));
            resarr[i] = OBJECT_TO_JSVAL(tmp1->jsobject);
            i++;
        }
        JSObject *tmp = JS_NewArrayObject(cx, (int)arr->_value.size(), resarr);
        delete[] resarr;
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp));
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for getSpriteFrameArray");
    return JS_FALSE;
}

JSBool JSSpriteFrame::jsGetSpriteFrameDictionary(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSString *arg0;
		JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* s = JS_EncodeStringToUTF8(cx, arg0);
        HLDictionary *d = HLSpriteFrame::getSpriteFrameDictionary(s);
        JS_free(cx, s);
        if (!d)
        {
            JS_SET_RVAL(cx, vp, JSVAL_NULL);
            return JS_TRUE;
        }
        std::map<std::string, HLObject*>::iterator itr;
        JSObject *tmp = JS_NewObject(cx, NULL, NULL, NULL);
        for (itr = d->_value.begin(); itr != d->_value.end(); itr++) 
        {
            JSSpriteFrame* tmp1 = JSSpriteFrame::getOrCreateWrapper(cx, (HLSpriteFrame *)itr->second);
            JS::RootedValue val(cx, OBJECT_TO_JSVAL(tmp1->jsobject));
            JS_SetProperty(cx, tmp, (itr->first).c_str(), val);
        }
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp));
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for getSpriteFrameDictionary");
    return JS_FALSE;
}

JSBool JSSpriteFrame::jsGetSpriteFrame(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 2)
    {
        JSString *arg0;
        JSString *arg1;
		JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "SS", &arg0, &arg1);
        char * s1 = JS_EncodeStringToUTF8(cx, arg0);
        char* s2 = JS_EncodeStringToUTF8(cx, arg1);
        HLSpriteFrame *frame = HLSpriteFrame::getSpriteFrame(s1, s2);
        JS_free(cx, s1);
        JS_free(cx, s2);
        if (frame) 
        {
            JSSpriteFrame* tmp = JSSpriteFrame::getOrCreateWrapper(cx, frame);
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(tmp->jsobject));
        }
        else 
        {
            JS_SET_RVAL(cx, vp, JSVAL_NULL);
        }
        return JS_TRUE;
    }
    JS_ReportError(cx, "wrong argument for getSpriteFrame");
    return JS_FALSE;
}

JSBool JSSpriteFrame::jsPurgeCachedData(JSContext *cx, uint32_t argc, jsval *vp)
{
    HLSpriteFrame::purgeCachedData();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSSpriteFrame::jsSetDisplayFrameForEntity(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);

    JSSpriteFrame* frame = (JSSpriteFrame*)JS_GetPrivate(obj);
    if (!frame)
    {
        JS_ReportError(cx, "JSSpriteFrame: native object already released");
        return JS_FALSE;
    }

    if (argc == 1)
    {
        JSObject *arg0;
		JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);

        JSEntity *entity = (JSEntity *)JS_GetPrivate(arg0);
        if (!entity)
        {
            return JS_FALSE;
        }

        frame->getNativeObject()->setDisplayFrameForEntity(entity->getNativeObject());
    }
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSSpriteFrame::jsRetain(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSSpriteFrame* frame = (JSSpriteFrame*)JS_GetPrivate(obj);
    if (!frame)
    {
        JS_ReportError(cx, "JSSpriteFrame: native object already released");
        return JS_FALSE;
    }
    
    frame->getNativeObject()->retain();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSSpriteFrame::jsRelease(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSSpriteFrame* frame = (JSSpriteFrame*)JS_GetPrivate(obj);
    if (!frame)
    {
        JS_ReportError(cx, "JSSpriteFrame: native object already released");
        return JS_FALSE;
    }
    
    frame->getNativeObject()->release();
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

NS_HL_END
