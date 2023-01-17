//
//  JSEntitySystem.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-25.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "JSEntitySystem.h"
#include "HLEntitySystem.h"
#include "HLClassRegister.h"
#include "HLUtil.h"
#include "HLNotificationCenter.h"
#include "JSTexture.h"
#include "HLTransform2DComponent.h"
#include "JSConversions.h"
#include "JSComponent.h"
#include "HLSkeletonComponent.h"
#include "HLView.h"
#include "HLParticleComponent.h"
#include "JSSceneManager.h"

NS_HL_BEGIN

template<> JSClass *JSCPPWrapper<JSEntityManager, HLEntityManager>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<JSEntityManager, HLEntityManager>::proto = NULL;

void JSEntityManager::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        JS_FN("createEntity", JSEntityManager::jsCreateEntity, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getEntity", JSEntityManager::jsGetEntity, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("destroyEntity", JSEntityManager::jsDestroyEntity, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("createAnonymousEntity", JSEntityManager::jsCreateAnonymousEntity, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getSceneManager", JSEntityManager::jsGetSceneManager, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FS_END
    };
    
    proto = JS_InitClass(cx,globalObj,NULL,jsclass,jsNoConstructor,0,properties,funcs,NULL,st_funcs);
}

JSBool JSEntityManager::jsCreateAnonymousEntity(JSContext *cx, uint32_t argc, jsval *vp)
{

    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEntityManager* thiz = (JSEntityManager*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "error(JSEntityManager::jsCreateAnonymousEntity): entity manager object is already freed");
        return JS_FALSE;
    }
    jsval* argv = JS_ARGV(cx, vp);
    JSEntity* entity;
    
    HLEntity* e = thiz->getNativeObject()->createAnonymousEntity(NULL);
    for (int i=0; i<argc; i++)
    {
        char *compname = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(argv[i]));
        HLComponent* comp =  (HLComponent*)g_factory.construct(compname);
        thiz->getNativeObject()->addComponent(e, StringUtil::BKDRHash(compname), comp);
        JS_free(cx, compname);
    }
    entity = JSEntity::getOrCreateWrapper(cx, e);//new JSEntity(e, out, thiz);
    
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(entity->jsobject));
    return JS_TRUE;
}

JSBool JSEntityManager::jsCreateEntity(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc > 0) 
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEntityManager* thiz = (JSEntityManager*)JS_GetPrivate(obj);
        if (!thiz)
        {
            JS_ReportError(cx, "error(JSEntityManager::jsCreateEntity): entity manager object is already freed");
            return JS_FALSE;
        }
        jsval* argv = JS_ARGV(cx, vp);
        JSEntity* entity;
        char * name = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(argv[0]));
        
        HLEntity* e = thiz->getNativeObject()->createEntity(name);
        JS_free(cx, name);
        for (int i=1; i<argc; i++) 
        {
            char *compname = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(argv[i]));
            HLComponent* comp =  (HLComponent*)g_factory.construct(compname);
            thiz->getNativeObject()->addComponent(e, StringUtil::BKDRHash(compname), comp);
            JS_free(cx, compname);
        }
        entity = JSEntity::getOrCreateWrapper(cx, e);//new JSEntity(e, out, thiz);

        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(entity->jsobject));
        return JS_TRUE;
    }
    JS_ReportError(cx, "jsCreateEntity: must call with at least one element");
    return JS_FALSE;
}

JSBool JSEntityManager::jsGetEntity(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1) 
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEntityManager* thiz = (JSEntityManager*)JS_GetPrivate(obj);
        if (!thiz)
        {
            JS_ReportError(cx, "error(JSEntityManager::jsGetEntity): entity manager object is already freed");
            return JS_FALSE;
        }
        JSString *arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        HLEntity*e = thiz->getNativeObject()->getEntity(narg0);
        JS_free(cx, narg0);
        if (e)
        {
            JSEntity *entity = JSEntity::getOrCreateWrapper(cx, e);
            
            JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(entity->jsobject));
            return JS_TRUE;
        }
        else
        {
            JS_SET_RVAL(cx, vp, JSVAL_NULL);
            return JS_TRUE;
        }
    }
    JS_ReportError(cx, "error(JSEntityManager::jsGetEntity): wrong argument number");
    return JS_FALSE;
}

JSBool JSEntityManager::jsDestroyEntity(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1) 
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEntityManager* thiz = (JSEntityManager*)JS_GetPrivate(obj);
        if (!thiz)
        {
            JS_ReportError(cx, "error(JSEntityManager::jsDestroyEntity): entity manager object is already freed");
            return JS_FALSE;
        }
        jsval* argv = JS_ARGV(cx, vp);

        if (argv->isObject())
        {
            JSObject *obj;
            JS_ConvertArguments(cx, 1, argv, "o", &obj);

            JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
            if (!entity)
            {
                JS_ReportError(cx, "error(JSEntityManager::jsDestroyEntity): entity is already freed");
                return JS_FALSE;
            }

            thiz->getNativeObject()->destroyEntity(entity->getNativeObject());
        }
        else 
        {
            JSString *arg0;
            JS_ConvertArguments(cx, 1, argv, "S", &arg0);
            char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
            thiz->getNativeObject()->destroyEntity(narg0);
            JS_free(cx, narg0);
        }
    }
    return JS_TRUE;
}

JSBool JSEntityManager::jsGetSceneManager(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEntityManager* thiz = (JSEntityManager*)JS_GetPrivate(obj);
    if (!thiz)
    {
        JS_ReportError(cx, "error(JSEntityManager::jsGetSceneManager): entity manager object is already freed");
        return JS_FALSE;
    }
    JSSceneManager* sceneMgr = JSSceneManager::getOrCreateWrapper(cx, thiz->getNativeObject()->getSceneManager());
    JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(sceneMgr->jsobject));
    return JS_TRUE;
}

template<> JSClass *JSCPPWrapper<JSEntity, HLEntity>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<JSEntity, HLEntity>::proto = NULL;
JSBool JSEntity::jsPropertyGet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);

	JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);

	if (!entity) return JS_TRUE;
    
    switch (propId) {
        case kUserInteractionEnabled:
        {
            val.setBoolean(entity->getNativeObject()->userInteractionEnabled());
        }
            break;
        case kVisible:
        {
            val.setBoolean(entity->getNativeObject()->isVisible());
        }
            break;
        case kName:
        {
            JSString *s = JS_NewStringCopyZ(cx, entity->getNativeObject()->mName.c_str());
            val.setString(s);
        }
            break;
        case kPosition:
        {
            HLPoint pos = entity->getNativeObject()->getProperty<HLPoint>("position");
            val.set(HLPoint_to_jsval(cx, pos));
        }
            break;
        case kZOrder:
        {
            val.setInt32(entity->getNativeObject()->getProperty<int>("zOrder"));
        }
            break;
        case kAnchorPoint:
        {
            HLPoint anchorPoint = entity->getNativeObject()->getProperty<HLPoint>("anchorPoint");
            val.set(HLPoint_to_jsval(cx, anchorPoint));
        }
            break;
        case kSize:
        {
            HLSize size = entity->getNativeObject()->getProperty<HLSize>("size");
            val.set(HLSize_to_jsval(cx, size));
        }
            break;
        case kScale:
        {
            val.setDouble(entity->getNativeObject()->getProperty<float>("scale"));
        }
            break;
        case kScaleX:
        {
            val.setDouble(entity->getNativeObject()->getProperty<float>("scaleX"));
        }
            break;
        case kScaleY:
        {
            val.setDouble(entity->getNativeObject()->getProperty<float>("scaleY"));
        }
            break;
        case kSkewX:
        {
            val.setDouble(entity->getNativeObject()->getProperty<float>("skewX"));
        }
            break;
        case kSkewY:
        {
            val.setDouble(entity->getNativeObject()->getProperty<float>("skewY"));
        }
            break;
        case kParent:
        {
            HLEntity * parent = entity->getNativeObject()->getProperty<HLEntity*>("parent");
            if (parent) {
                val.setObjectOrNull(getOrCreateWrapper(cx, parent)->jsobject);
            } else {
                val.setNull();
            }
        }
            break;
        case kChildren:
        {
            std::list<HLEntity*>&& children = entity->getNativeObject()->getProperty<std::list<HLEntity*>>("children");
            std::vector<jsval> values;
            for (std::list<HLEntity*>::iterator itr = children.begin(); itr != children.end(); ++itr)
            {
                values.push_back(OBJECT_TO_JSVAL(getOrCreateWrapper(cx, *itr)->jsobject));
            }
            JSObject* arr = JS_NewArrayObject(cx, (int)values.size(), &values[0]);
            val.setObject(*arr);
        }
            break;
        case kRotation:
        {
            val.setDouble(entity->getNativeObject()->getProperty<float>("rotation"));
        }
            break;
        case kTexture:
        {
            HLTexture* tex = entity->getNativeObject()->getProperty<HLTexture*>("texture");
            if (!tex)
            {
                val.setNull();
            }
            else
            {
                JSTexture *tmp = JSTexture::getOrCreateWrapper(cx, tex);

                val.set(OBJECT_TO_JSVAL(tmp->jsobject));
            }
        }
            break;
        case kTextureRect:
        {
            HLRect rect = entity->getNativeObject()->getProperty<HLRect>("textureRect");
            val.set(HLRect_to_jsval(cx, rect));
        }
            break;
        case kGray:
        {
            bool gray = entity->getNativeObject()->getProperty<bool>("gray");
            val.setBoolean(gray);
        }
            break;
        case kFlipX:
        {
            bool flipx = entity->getNativeObject()->getProperty<bool>("flipX");
            val.setBoolean(flipx);
        }
            break;
        case kFlipY:
        {
            bool flipy = entity->getNativeObject()->getProperty<bool>("flipY");
            val.setBoolean(flipy);
        }
            break;
        case kUntrimmedSize:
        {
            val.set(HLSize_to_jsval(cx, entity->getNativeObject()->getProperty<HLSize>("untrimmedSize")));
        }
            break;
        case kOffsetPosFromBL:
        {
            val.set(HLPoint_to_jsval(cx, entity->getNativeObject()->getProperty<HLPoint>("offsetPosFromBL")));
        }
            break;
        case kTextureRotated:
        {
            val.setBoolean(entity->getNativeObject()->getProperty<bool>("textureRotated"));
        }
            break;
        case kBrightness:
        {
            val.setDouble(entity->getNativeObject()->getProperty<float>("brightness"));
        }
            break;
//        case kText:
//        {
//            JSString *s = JS_NewStringCopyZ(cx, entity->getNativeObject()->getProperty<std::string>("text").c_str());
//            val.setString(s);
//        }
//            break;
        case kOutlineFile:
        {
            JSString *s = JS_NewStringCopyZ(cx, entity->getNativeObject()->getProperty<std::string>("outlinefile").c_str());
            val.setString(s);
        }
            break;
        case kColor:
        {
            color4B color = entity->getNativeObject()->getProperty<color4B>("color");
            val.set(color_to_jsval(cx, color));
        }
            break;
        case kColorPercent:
        {
            color4F color = entity->getNativeObject()->getProperty<color4F>("colorPercent");
            val.set(color_to_jsval(cx, color));
        }
            break;
        case kColorAmount:
        {
            color4F color = entity->getNativeObject()->getProperty<color4F>("colorAmount");
            val.set(color_to_jsval(cx, color));
        }
            break;
        case kSkelfile:
        {
            JSString *s = JS_NewStringCopyZ(cx, entity->getNativeObject()->getProperty<std::string>("skelfile").c_str());
            val.setString(s);
        }
            break;
        case kView:
        {
            gui::HLView* view = entity->getNativeObject()->getProperty<gui::HLView*>("view");
            if (view)
            {
                val.setObjectOrNull(view->getOrCreateJSWrapper(cx)->jsobject);
            }
            else
            {
                val.setNull();
            }
        }
            break;
        case kAlphaTest:
        {
            val.setDouble(entity->getNativeObject()->getProperty<float>("alphaTest"));
        }
            break;
        case kStencil:
        {
            HLEntity* entity1 = entity->getNativeObject()->getProperty<HLEntity*>("stencil");
            if (entity1)
            {
                JSEntity* jentity = JSEntity::getOrCreateWrapper(cx, entity1);
                val.setObjectOrNull(jentity->jsobject);
            }
            else
            {
                val.setNull();
            }
        }
            break;
        case kInverted:
        {
            val.setBoolean(entity->getNativeObject()->getProperty<bool>("inverted"));
        }
            break;
        case kActive:
        {
            val.setBoolean(entity->getNativeObject()->getProperty<bool>("active"));
        }
            break;
        case kCameraRect:
        {
            HLRect rect = entity->getNativeObject()->getProperty<HLRect>("cameraRect");
            val.set(HLRect_to_jsval(cx, rect));
        }
            break;
        case kCameraAnimation:
        {
            val.setBoolean(entity->getNativeObject()->getProperty<bool>("animation"));
        }
            break;
        case kOutlinePoints:
        {
            std::vector<HLPoint>& points = entity->getNativeObject()->getProperty<std::vector<HLPoint>& >("outlinepoints");
            if (points.empty())
            {
                val.setNull();
                break;
            }
            size_t size = points.size();
            jsval *vals = new jsval[points.size()];
            for (size_t i = 0; i < size; ++i)
            {
                vals[i] = HLPoint_to_jsval(cx, points[i]);
            }
            val.setObjectOrNull(JS_NewArrayObject(cx, size, vals));
            delete [] vals;
        }
            break;
        default:
            break;
    }
    
    return JS_TRUE;
}

JSBool JSEntity::jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val)
{
    int32_t propId = JSID_TO_INT(_id);

	JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);

	if (!entity) return JS_FALSE;
    
    switch (propId) {
        case kUserInteractionEnabled:
        {
            entity->getNativeObject()->setUserInteractionEnabled(val.toBoolean());
        }
            break;
        case kVisible:
        {
            entity->getNativeObject()->setVisible(val.toBoolean());
        }
            break;
        case kName:
        {
            // not allowed
            JS_ReportError(cx, "JSEntity::jsPropertySet: cannot set entity's name");
            return JS_FALSE;
        }
            break;
        case kPosition:
        {
            HLPoint p = jsval_to_HLPoint(cx, val);
            entity->getNativeObject()->setProperty("position", p);
        }
            break;
        case kZOrder:
        {
            entity->getNativeObject()->setProperty<int>("zOrder", val.toInt32());
        }
            break;
        case kAnchorPoint:
        {
            HLPoint p = jsval_to_HLPoint(cx, val);
            entity->getNativeObject()->setProperty("anchorPoint", p);
        }
            break;
        case kSize:
        {
            HLSize s = jsval_to_HLSize(cx, val);
            entity->getNativeObject()->setProperty("size", s);
        }
            break;
        case kScale:
        {
            entity->getNativeObject()->setProperty<float>("scale", val.toNumber());
        }
            break;
        case kScaleX:
        {
            entity->getNativeObject()->setProperty<float>("scaleX", val.toNumber());
        }
            break;
        case kScaleY:
        {
            entity->getNativeObject()->setProperty<float>("scaleY", val.toNumber());
        }
            break;
        case kSkewX:
        {
            entity->getNativeObject()->setProperty<float>("skewX", val.toNumber());
        }
            break;
        case kSkewY:
        {
            entity->getNativeObject()->setProperty<float>("skewY", val.toNumber());
        }
            break;
        case kRotation:
        {
            entity->getNativeObject()->setProperty<float>("rotation", val.toNumber());
        }
            break;
        case kTexture:
        {
            if (val.isObject())
            {
                JSTexture *p = (JSTexture *)JS_GetPrivate(&val.toObject());
                if (!p)
                {
                    JS_ReportError(cx, "JSEntity: setTexture error");
                    return JS_FALSE;
                }
                entity->getNativeObject()->setProperty("texture", p->getNativeObject());
            }
            else if (val.isNull())
            {
                entity->getNativeObject()->setProperty<HLTexture*>("texture", NULL);
            }
        }
            break;
        case kTextureRect:
        {
            HLRect r = jsval_to_HLRect(cx, val);
            entity->getNativeObject()->setProperty("textureRect", r);
        }
            break;
        case kGray:
        {
            entity->getNativeObject()->setProperty("gray", val.toBoolean());
        }
            break;
        case kFlipX:
        {
            entity->getNativeObject()->setProperty("flipX", val.toBoolean());
        }
            break;
        case kFlipY:
        {
            entity->getNativeObject()->setProperty("flipY", val.toBoolean());
        }
            break;
        case kUntrimmedSize:
        {
            entity->getNativeObject()->setProperty("untrimmedSize", jsval_to_HLSize(cx, val));
        }
            break;
        case kOffsetPosFromBL:
        {
            entity->getNativeObject()->setProperty("offsetPosFromBL", jsval_to_HLPoint(cx, val));
        }
            break;
        case kBrightness:
        {
            float brightness = 1;
            if (val.isInt32())
            {
                brightness = val.toInt32();
            }
            else if (val.isDouble())
            {
                brightness = val.toNumber();
            }
            entity->getNativeObject()->setProperty("brightness", brightness);
        }
            break;
        case kTextureRotated:
        {
            entity->getNativeObject()->setProperty<bool>("textureRotated", val.toBoolean());
        }
            break;
//        case kText:
//        {
//            JSString * str = val.toString(); entity->getNativeObject()->setProperty("text", std::string(JS_EncodeStringToUTF8(cx, str)));
//        }
//            break;
        case kOutlineFile:
        {
            char* s = JS_EncodeStringToUTF8(cx, val.toString());
            entity->getNativeObject()->setProperty("outlinefile", std::string(s));
            JS_free(cx, s);
        }
            break;
        case kColor:
        {
            color4F color = jsval_to_color(cx, val);
            color4B color1;
            color1.r = color.r*255;
            color1.g = color.g*255;
            color1.b = color.b*255;
            color1.a = color.a*255;
            entity->getNativeObject()->setProperty("color", color1);
        }
            break;
        case kColorPercent:
        {
            color4F color = jsval_to_color(cx, val);
            entity->getNativeObject()->setProperty("colorPercent", color);
        }
            break;
        case kColorAmount:
        {
            color4F color = jsval_to_color(cx, val);
            entity->getNativeObject()->setProperty("colorAmount", color);
        }
            break;
        case kSkelfile:
        {
            char* s = JS_EncodeStringToUTF8(cx, val.toString());
            entity->getNativeObject()->setProperty("skelfile", std::string(s));
            JS_free(cx, s);
        }
            break;
        case kView:
        {
            gui::HLView* view = (gui::HLView*)((JSCppWrapperBase*)JS_GetPrivate(val.toObjectOrNull()))->object;
            entity->getNativeObject()->setProperty("view", view);
        }
            break;
        case kAlphaTest:
        {
            float alphaTest = -1;
            if (val.isInt32())
            {
                alphaTest = val.toInt32();
            }
            else if (val.isDouble())
            {
                alphaTest = val.toNumber();
            }
            entity->getNativeObject()->setProperty("alphaTest", alphaTest);
        }
            break;
        case kStencil:
        {
            HLEntity* entity1 = (HLEntity*)((JSCppWrapperBase*)JS_GetPrivate(val.toObjectOrNull()))->object;
            entity->getNativeObject()->setProperty("stencil", entity1);
        }
            break;
        case kInverted:
        {
            entity->getNativeObject()->setProperty("inverted", val.toBoolean());
        }
            break;
        case kActive:
        {
            entity->getNativeObject()->setProperty("active", val.toBoolean());
        }
            break;
        case kCameraRect:
        {
            HLRect r = jsval_to_HLRect(cx, val);
            entity->getNativeObject()->setProperty("cameraRect", r);
        }
            break;
        case kCameraAnimation:
        {
            entity->getNativeObject()->setProperty("animation", val.toBoolean());
        }
            break;
        case kOutlinePoints:
        {
            JSObject* obj = val.toObjectOrNull();
            std::vector<HLPoint> points;
            if (!obj)
            {
                entity->getNativeObject()->setProperty<std::vector<HLPoint>& >("outlinepoints", points);
                break;
            }
            if (!JS_IsArrayObject(cx, obj))
            {
                entity->getNativeObject()->setProperty<std::vector<HLPoint>& >("outlinepoints", points);
                break;
            }
            uint32_t len;
            JS_GetArrayLength(cx, obj, &len);
            for (int i = 0; i < len; ++i)
            {
                JS::RootedValue vp(cx);
                JS_GetElement(cx, obj, i, &vp);
                points.push_back(jsval_to_HLPoint(cx, vp));
            }
            entity->getNativeObject()->setProperty<std::vector<HLPoint>& >("outlinepoints", points);
        }
            break;
        default:
            break;
    }
    
    return JS_TRUE;
}

void JSEntity::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        {"userInteractionEnabled", kUserInteractionEnabled, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"visible", kVisible, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"name", kName, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"position", kPosition, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"zOrder", kZOrder, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"anchorPoint", kAnchorPoint, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"size", kSize, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"scale", kScale, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"scaleX", kScaleX, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"scaleY", kScaleY, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"skewX", kSkewX, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"skewY", kSkewY, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"parent", kParent, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, JSOP_WRAPPER(JSEntity::jsPropertyGet), {0}},
        {"children", kChildren, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, JSOP_WRAPPER(JSEntity::jsPropertyGet), {0}},
        {"rotation", kRotation, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"texture", kTexture, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"textureRotated", kTextureRotated, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"textureRect", kTextureRect, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"gray", kGray, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"flipX", kFlipX, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"flipY", kFlipY, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"untrimmedSize", kUntrimmedSize, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"offsetPosFromBL", kOffsetPosFromBL, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"brightness", kBrightness, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
//        {"text", kText, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
//        {"textHAlignment", kTextHAlignment, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
//        {"textVAlignment", kTextVAlignment, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"color", kColor, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"outlinefile", kOutlineFile, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"colorPercent", kColorPercent, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"colorAmount", kColorAmount, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"skelfile", kSkelfile, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"view", kView, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"alphaTest", kAlphaTest, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"stencil", kStencil, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"inverted", kInverted, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"active", kActive, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"cameraRect", kCameraRect, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"cameraAnimation", kCameraAnimation, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {"outlinepoints", kOutlinePoints, JSPROP_PERMANENT | JSPROP_SHARED, JSOP_WRAPPER(JSEntity::jsPropertyGet), JSOP_WRAPPER(JSEntity::jsPropertySet)},
        {0, 0, 0, 0, 0}
    };
    
    static JSFunctionSpec funcs[] = {
        JS_FN("getProperty", JSEntity::jsGetProperty, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setProperty", JSEntity::jsSetProperty, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("addComponent", JSEntity::jsAddComponent, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("hasComponent", JSEntity::jsHasComponent, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setUserInteractionEnabled", JSEntity::jsSetUserInteractionEnabled, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("addChild", JSEntity::jsAddChild, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("removeChild", JSEntity::jsRemoveChild, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("removeFromParent", JSEntity::jsRemoveFromParent, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setVisible", JSEntity::jsSetVisible, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("addScript", JSEntity::jsAddScript, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getEntityManager", JSEntity::jsGetEntityManager, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("sendMessage", JSEntity::jsSendMessage, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getComponent", JSEntity::jsGetComponent, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("playSkeletonAnimation", JSEntity::jsPlaySkeletonAnimation, 6, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("pauseSkeletonAnimation", JSEntity::jsPauseSkeletonAnimation, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("resumeSkeletonAnimation", JSEntity::jsResumeSkeletonAnimation, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setSkeletonAnimationDelegate", JSEntity::jsSetSkeletonAnimationDelegate, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("getSkeletonRegisterPoint", JSEntity::jsGetSkeletonRegisterPoint, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("purgeSkeletonTextureCache", JSEntity::jsPurgeSkeletonTextureCache, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("skeletonAddEntityToBone", JSEntity::jsSkeletonAddEntityToBone, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setSkeletonContainerImageIndex", JSEntity::jsSetSkeletonContainerImageIndex, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setSkeletonContainerImageName", JSEntity::jsSetSkeletonContainerImageName, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("addTouchEventListener", JSEntity::jsAddTouchEventListener, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("removeTouchEventListener", JSEntity::jsRemoveTouchEventListener, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("convertToNodeSpace", JSEntity::jsConvertToNodeSpace, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("convertToWorldSpace", JSEntity::jsConvertToWorldSpace, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("addParticleComponent", JSEntity::jsAddParticleComponent, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setBlendAdditive", JSEntity::jsSetBlendAdditive, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FS_END
    };
    
    proto = JS_InitClass(cx,globalObj,NULL,jsclass,jsNoConstructor,0,properties,funcs,NULL,st_funcs);
}

JSBool JSEntity::jsGetProperty(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEntity* thiz = (JSEntity*)JS_GetPrivate(obj);
    std::string name = jsval_to_value<std::string>(JS_ARGV(cx, vp)[0]);
    jsval val = thiz->getNativeObject()->jsGetProperty(name);
    JS_SET_RVAL(cx, vp, val);
    return JS_TRUE;
}

JSBool JSEntity::jsSetProperty(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEntity* thiz = (JSEntity*)JS_GetPrivate(obj);
    std::string name = jsval_to_value<std::string>(JS_ARGV(cx, vp)[0]);
    thiz->getNativeObject()->jsSetProperty(name, JS_ARGV(cx, vp)[1]);
    return JS_TRUE;
}

JSBool JSEntity::jsAddTouchEventListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEntity* thiz = (JSEntity*)JS_GetPrivate(obj);
    if (argc == 1)
    {
        thiz->getNativeObject()->onTouchEvent += newDelegate<HLTouchEvent*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        thiz->getNativeObject()->onTouchEvent += newDelegate<HLTouchEvent*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSEntity::jsRemoveTouchEventListener(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEntity* thiz = (JSEntity*)JS_GetPrivate(obj);
    if (argc == 1)
    {
        thiz->getNativeObject()->onTouchEvent -= newDelegate<HLTouchEvent*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        thiz->getNativeObject()->onTouchEvent -= newDelegate<HLTouchEvent*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSEntity::jsConvertToNodeSpace(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEntity* thiz = (JSEntity*)JS_GetPrivate(obj);
    if (argc == 1)
    {
        if (thiz->getNativeObject()->hasComponent<HLTransform2DComponent>())
        {
            HLPoint p = jsval_to_HLPoint(cx, JS_ARGV(cx, vp)[0]);
            p = thiz->getNativeObject()->getComponent<HLTransform2DComponent>()->convertToNodeSpace(p);
            JS_SET_RVAL(cx, vp, HLPoint_to_jsval(cx, p));
        }
        else
        {
            JS_SET_RVAL(cx, vp, JSVAL_NULL);
        }
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSEntity::jsConvertToWorldSpace(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEntity* thiz = (JSEntity*)JS_GetPrivate(obj);
    if (argc == 1)
    {
        if (thiz->getNativeObject()->hasComponent<HLTransform2DComponent>())
        {
            HLPoint p = jsval_to_HLPoint(cx, JS_ARGV(cx, vp)[0]);
            p = thiz->getNativeObject()->getComponent<HLTransform2DComponent>()->convertToWorldSpace(p);
            JS_SET_RVAL(cx, vp, HLPoint_to_jsval(cx, p));
        }
        else
        {
            JS_SET_RVAL(cx, vp, JSVAL_NULL);
        }
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSEntity::jsGetComponent(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
        if (!entity)
        {
            return JS_FALSE;
        }
        char * name = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
        JSComponent * jscomp = entity->getNativeObject()->getJSComponent(name);
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jscomp->getJSObject()));
        JS_free(cx, name);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSEntity::jsSendMessage(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc >= 1)
    {
        JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
        JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
        if (!entity)
        {
            return JS_FALSE;
        }
        std::list<JSComponent*> comps = entity->getNativeObject()->getComponents<JSComponent>();
        for (std::list<JSComponent*>::iterator itr = comps.begin(); itr != comps.end(); ++itr)
        {
            jsval msg = JS_ARGV(cx, vp)[0];
            if (!JSVAL_IS_STRING(msg))
            {
                JS_ReportError(cx, "the first argument must be the message name");
                return JS_FALSE;
            }
            char * str = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(msg));
            jsval rval;
            if ((*itr)->onMessage(cx, str, argc-1, argc==1?NULL:JS_ARGV(cx, vp)+1, &rval))
            {
                JS_SET_RVAL(cx, vp, rval);
            }
            JS_free(cx, str);
        }
        return JS_TRUE;
    }
    else
    {
        JS_ReportError(cx, "no arguments for sendMessage");
        return JS_FALSE;
    }
    JS_ReportError(cx, "no compoment accepts the message");
    return JS_FALSE;
}

JSBool JSEntity::jsPlaySkeletonAnimation(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }
    
    HLSkeletonComponent* comp = entity->getNativeObject()->getComponent<HLSkeletonComponent>();
    if (!comp)
    {
        return JS_FALSE;
    }
    
    if (argc > 0)
    {
        char *action = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(JS_ARGV(cx, vp)[0]));
        bool prepared = false;
        bool loop = false;
        int durationTo = 0;
        float scale = 1;
        float tweenEasing = NAN;
        if (argc > 1)
        {
            prepared = JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[1]);
        }
        if (argc > 2)
        {
            loop = JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[2]);
        }
        if (argc > 3)
        {
            durationTo = JSVAL_TO_INT(JS_ARGV(cx, vp)[3]);
        }
        if (argc > 4)
        {
            if (JSVAL_IS_INT(JS_ARGV(cx, vp)[4]))
            {
                scale = JSVAL_TO_INT(JS_ARGV(cx, vp)[4]);
            }
            else
            {
                scale = JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[4]);
            }
        }
        if (argc > 5)
        {
            if (JSVAL_IS_INT(JS_ARGV(cx, vp)[5]))
            {
                tweenEasing = JSVAL_TO_INT(JS_ARGV(cx, vp)[5]);
            }
            else
            {
                tweenEasing = JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[5]);
            }
        }
        comp->play(action, prepared, loop, durationTo, scale, tweenEasing);
        JS_free(cx, action);
    }
    
    return JS_TRUE;
}

JSBool JSEntity::jsPauseSkeletonAnimation(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }
    
    HLSkeletonComponent* comp = entity->getNativeObject()->getComponent<HLSkeletonComponent>();
    if (!comp)
    {
        return JS_FALSE;
    }
    comp->pause();
    return JS_TRUE;
}

JSBool JSEntity::jsResumeSkeletonAnimation(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }
    
    HLSkeletonComponent* comp = entity->getNativeObject()->getComponent<HLSkeletonComponent>();
    if (!comp)
    {
        return JS_FALSE;
    }
    comp->resume();
    return JS_TRUE;
}

JSBool JSEntity::jsSetSkeletonAnimationDelegate(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        JS_ReportError(cx, "jsSetSkeletonAnimationDelegate: entity not exist");
        return JS_FALSE;
    }
    
    HLSkeletonComponent* comp = entity->getNativeObject()->getComponent<HLSkeletonComponent>();
    if (!comp)
    {
        JS_ReportError(cx, "jsSetSkeletonAnimationDelegate: entity does not have HLSkeletonComponent");
        return JS_FALSE;
    }
    
    if (argc == 1)
    {
        if (JSVAL_IS_NULL(JS_ARGV(cx, vp)[0]))
        {
            comp->onAnimationEvent.clear();
        }
        else
        {
            comp->onAnimationEvent = newDelegate<HLEntity*, const char*, const char*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
        }
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        comp->onAnimationEvent = newDelegate<HLEntity*, const char*, const char*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSEntity::jsGetSkeletonRegisterPoint(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        JS_ReportError(cx, "jsGetSkeletonRegisterPoint: entity not exist");
        return JS_FALSE;
    }
    
    HLSkeletonComponent* comp = entity->getNativeObject()->getComponent<HLSkeletonComponent>();
    if (!comp)
    {
        JS_ReportError(cx, "jsGetSkeletonRegisterPoint: entity does not have HLSkeletonComponent");
        return JS_FALSE;
    }
    if (argc == 1)
    {
        JSString *arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        jsval ret = HLPoint_to_jsval(cx, comp->getRegisterPoint(narg0));
        JS_free(cx, narg0);
        JS_SET_RVAL(cx, vp, ret);
        return JS_TRUE;
    }
    JS_ReportError(cx, "jsGetSkeletonRegisterPoint arguments error");
    return JS_FALSE;
}

JSBool JSEntity::jsPurgeSkeletonTextureCache(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        JS_ReportError(cx, "jsGetSkeletonRegisterPoint: entity not exist");
        return JS_FALSE;
    }
    
    HLSkeletonComponent* comp = entity->getNativeObject()->getComponent<HLSkeletonComponent>();
    if (!comp)
    {
        JS_ReportError(cx, "JSEntity::jsPurgeSkeletonTextureCache entity does not have HLSkeletonComponent");
        return JS_FALSE;
    }
    comp->purgeTextureCache();
    return JS_TRUE;
}

JSBool JSEntity::jsSkeletonAddEntityToBone(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        JS_ReportError(cx, "jsGetSkeletonRegisterPoint: entity not exist");
        return JS_FALSE;
    }
    
    HLSkeletonComponent* comp = entity->getNativeObject()->getComponent<HLSkeletonComponent>();
    if (!comp)
    {
        JS_ReportError(cx, "JSEntity::jsPurgeSkeletonTextureCache entity does not have HLSkeletonComponent");
        return JS_FALSE;
    }
    
    if (argc == 2)
    {
        JSString* jstr;
        JSObject* obj1;
        JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "So", &jstr, &obj1);
        JSEntity* entity1 = (JSEntity*)JS_GetPrivate(obj1);
        char* bonename = JS_EncodeStringToUTF8(cx, jstr);
        comp->addEntityToBone(bonename, entity1->getNativeObject());
        JS_free(cx, bonename);
    }
    
    return JS_TRUE;
}

JSBool JSEntity::jsSetSkeletonContainerImageIndex(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        JS_ReportError(cx, "jsGetSkeletonRegisterPoint: entity not exist");
        return JS_FALSE;
    }
    
    HLSkeletonComponent* comp = entity->getNativeObject()->getComponent<HLSkeletonComponent>();
    if (!comp)
    {
        JS_ReportError(cx, "jsGetSkeletonRegisterPoint: entity does not have HLSkeletonComponent");
        return JS_FALSE;
    }
    if (argc == 2)
    {
        JSString* jstr;
        int index;
        JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "Si", &jstr, &index);
        char* container = JS_EncodeStringToUTF8(cx, jstr);
        comp->setContainerImageIndex(container, index);
        JS_free(cx, container);
        return JS_TRUE;
    }
    JS_ReportError(cx, "jsGetSkeletonRegisterPoint arguments error");
    return JS_FALSE;
}

JSBool JSEntity::jsSetSkeletonContainerImageName(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        JS_ReportError(cx, "jsGetSkeletonRegisterPoint: entity not exist");
        return JS_FALSE;
    }
    
    HLSkeletonComponent* comp = entity->getNativeObject()->getComponent<HLSkeletonComponent>();
    if (!comp)
    {
        JS_ReportError(cx, "jsGetSkeletonRegisterPoint: entity does not have HLSkeletonComponent");
        return JS_FALSE;
    }
    if (argc == 2)
    {
        JSString* jstr;
        JSString* name;
        JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "SS", &jstr, &name);
        char* container = JS_EncodeStringToUTF8(cx, jstr);
        char* name1 = JS_EncodeStringToUTF8(cx, name);
        comp->setContainerImageName(container, name1);
        JS_free(cx, container);
        JS_free(cx, name1);
        return JS_TRUE;
    }
    JS_ReportError(cx, "jsGetSkeletonRegisterPoint arguments error");
    return JS_FALSE;
}

JSBool JSEntity::jsAddComponent(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);

    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }

    if (argc == 1)
    {
        JSString *arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        HLComponent* comp =  (HLComponent*)g_factory.construct(narg0);
        entity->getNativeObject()->getEntityManager()->addComponent(entity->getNativeObject(), StringUtil::BKDRHash(narg0), comp);
        JS_free(cx, narg0);
    }
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    
    return JS_TRUE;
}

JSBool JSEntity::jsHasComponent(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);

    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }

    if (argc == 1)
    {
        JSString *arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        if (entity->getNativeObject()->mComponentsMap.find(StringUtil::BKDRHash(narg0)) != entity->getNativeObject()->mComponentsMap.end())
        {
            JS_free(cx, narg0);
            JS_SET_RVAL(cx, vp, JSVAL_TRUE);
            return JS_TRUE;
        }
        JS_free(cx, narg0);
        JS_SET_RVAL(cx, vp, JSVAL_FALSE);
        return JS_TRUE;
    }
    JS_SET_RVAL(cx, vp, JSVAL_FALSE);
    return JS_TRUE;
}

JSBool JSEntity::jsSetUserInteractionEnabled(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);

    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }

    if (argc == 1)
    {
        JSBool arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "b", &arg0);
        entity->getNativeObject()->setUserInteractionEnabled(arg0);
    }
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSEntity::jsAddChild(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);

    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }

    if (!entity->getNativeObject()->hasComponent<HLTransform2DComponent>()) {
        JS_ReportError(cx, "jsAddChild: entity cannot have children");
        return JS_FALSE;
    }
    if (argc == 1) 
    {
        JSObject *arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);

        JSEntity* child = (JSEntity*)JS_GetPrivate(arg0);
        if (!child)
        {
            return JS_FALSE;
        }

        entity->getNativeObject()->getComponent<HLTransform2DComponent>()->addChild(child->getNativeObject());
    }
    else if (argc == 2)
    {
        JSObject *arg0;
        double arg1;
        JS_ConvertArguments(cx, 2, JS_ARGV(cx, vp), "od", &arg0, &arg1);

        JSEntity* child = (JSEntity*)JS_GetPrivate(arg0);
        if (!child)
        {
            return JS_FALSE;
        }

        entity->getNativeObject()->getComponent<HLTransform2DComponent>()->addChild(child->getNativeObject(), arg1);
    }
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSEntity::jsRemoveChild(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);

    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }

    if (!entity->getNativeObject()->hasComponent<HLTransform2DComponent>()) {
        JS_ReportError(cx, "jsRemoveChild: entity cannot have children");
        return JS_FALSE;
    }
    if (argc > 0)
    {
        JSObject *arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "o", &arg0);

        JSEntity* child = (JSEntity*)JS_GetPrivate(arg0);
        if (!child)
        {
            return JS_FALSE;
        }
        
        bool cleanup = false;
        if (argc > 1)
        {
            cleanup = JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[1]);
        }

        entity->getNativeObject()->getComponent<HLTransform2DComponent>()->removeChild(child->getNativeObject(), cleanup);
        
        JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSEntity::jsRemoveFromParent(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);

    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }

    if (!entity->getNativeObject()->hasComponent<HLTransform2DComponent>())
    {
        JS_ReportError(cx, "jsRemoveFromParent: entity cannot have children");
        return JS_FALSE;
    }
    
    bool cleanup = false;
    if (argc > 0)
    {
        cleanup = JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[0]);
    }
    entity->getNativeObject()->getComponent<HLTransform2DComponent>()->removeFromParent(cleanup);
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSEntity::jsSetVisible(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);

    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }

    if (argc == 1)
    {
        JSBool arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "b", &arg0);
        entity->getNativeObject()->setVisible(arg0);
    }
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSEntity::jsAddScript(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }
    
    if (argc == 1)
    {
        JSString *arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char* narg0 = JS_EncodeStringToUTF8(cx, arg0);
        JSComponent* com = new JSComponent(narg0);
        JS_free(cx, narg0);
        entity->getNativeObject()->addComponent(com);
    }
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

JSBool JSEntity::jsGetEntityManager(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }
    
    if (entity && argc == 0)
    {
        JSEntityManager* jsEntMgr = JSEntityManager::getOrCreateWrapper(cx, entity->getNativeObject()->getEntityManager());
        JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsEntMgr->jsobject));
        return JS_TRUE;
    }
    
    JS_ReportError(cx, "jsGetEntityManager: error");
    return JS_FALSE;
}

JSBool JSEntity::jsAddParticleComponent(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }
    
    int num = 30;
    if (argc == 1)
    {
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "i", &num);
    }
    HLParticleComponent *comp = new HLParticleComponent(num);
    entity->getNativeObject()->addComponent(comp);
    return JS_TRUE;
}

JSBool JSEntity::jsSetBlendAdditive(JSContext *cx, uint32_t argc, jsval *vp)
{
    JSObject* obj = (JSObject *)JS_THIS_OBJECT(cx, vp);
    JSEntity* entity = (JSEntity*)JS_GetPrivate(obj);
    if (!entity)
    {
        return JS_FALSE;
    }
    
    if (entity->getNativeObject()->hasComponent<HLParticleComponent>())
    {
        if (argc == 1)
        {
            JSBool arg0;
            JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "b", &arg0);
            entity->getNativeObject()->getComponent<HLParticleComponent>()->setIsBlendAdditive(arg0);
        }
    }
    return JS_TRUE;
}

template<> JSClass *JSCPPWrapper<JSTweenSystem, HLTweenSystem>::jsclass = NULL;
template<> JSObject *JSCPPWrapper<JSTweenSystem, HLTweenSystem>::proto = NULL;

void JSTweenSystem::jsCreateClass(JSContext *cx, JSObject *globalObj, const char *name)
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
        JS_FS_END
    };
    
    static JSPropertySpec st_properties[] = {
//        {"delegate", 1, JSPROP_PERMANENT | JSPROP_SHARED, {0, 0}, JSOP_WRAPPER(JSTweenSystem::jsPropertySet)},
        {0, 0, 0, 0, 0}
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FN("startAnimations", JSTweenSystem::jsStartAnimations, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setAutoReverse", JSTweenSystem::jsSetAutoReverse, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("commitAnimations", JSTweenSystem::jsCommitAnimations, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setAnimationCurve", JSTweenSystem::jsSetAnimationCurve, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setAnimationCurveRate", JSTweenSystem::jsSetAnimationCurveRate, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("stopAnimation", JSTweenSystem::jsStopAnimation, 1, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("stopAllAnimations", JSTweenSystem::jsStopAllAnimations, 0, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FN("setAnimationDidStopCallback", JSTweenSystem::jsSetAnimationDidStopCallback, 2, JSPROP_PERMANENT | JSPROP_READONLY),
        JS_FS_END
    };
    
    proto = JS_InitClass(cx,globalObj,NULL,jsclass,jsNoConstructor,0,properties,funcs,st_properties,st_funcs);
}

JSBool JSTweenSystem::jsStartAnimations(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc >= 2)
    {
        jsval* argv = JS_ARGV(cx, vp);
        char * name = JS_EncodeStringToUTF8(cx, JSVAL_TO_STRING(argv[0]));
        double duration = argv[1].toNumber();
        if (argc > 3)
        {
            int repeate = argv[2].toInt32();
            JSObject* obj = argv[3].toObjectOrNull();
            HLTweenSystem::startAnimations(name, duration, repeate, obj);
        }
        else if (argc == 3)
        {
            int repeate = argv[2].toInt32();
            HLTweenSystem::startAnimations(name, duration, repeate);
        }
        else
        {
            HLTweenSystem::startAnimations(name, duration);
        }
        JS_free(cx, name);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSTweenSystem::jsSetAutoReverse(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSBool arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "b", &arg0);
        HLTweenSystem::setAutoReverse(arg0);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSTweenSystem::jsCommitAnimations(JSContext *cx, uint32_t argc, jsval *vp)
{
    HLTweenSystem::commitAnimations();
    return JS_TRUE;
}

JSBool JSTweenSystem::jsSetAnimationCurve(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        int32_t arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "i", &arg0);
        HLTweenSystem::setAnimationCurve((HLAnimationCurve)arg0);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSTweenSystem::jsSetAnimationCurveRate(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        double arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "d", &arg0);
        HLTweenSystem::setAnimationCurveRate(arg0);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSTweenSystem::jsStopAnimation(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        JSString* arg0;
        JS_ConvertArguments(cx, 1, JS_ARGV(cx, vp), "S", &arg0);
        char * s = JS_EncodeStringToUTF8(cx, arg0);
        HLTweenSystem::stopAnimation(s);
        JS_free(cx, s);
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSBool JSTweenSystem::jsStopAllAnimations(JSContext *cx, uint32_t argc, jsval *vp)
{
    HLTweenSystem::stopAllAnimations();
    return JS_TRUE;
}

JSBool JSTweenSystem::jsSetAnimationDidStopCallback(JSContext *cx, uint32_t argc, jsval *vp)
{
    if (argc == 1)
    {
        HLTweenSystem::animationDidStopCallback = newDelegate<bool, void*>(JSVAL_TO_OBJECT(JS_THIS(cx, vp)), JS_ARGV(cx, vp)[0]);
        return JS_TRUE;
    }
    else if (argc == 2)
    {
        HLTweenSystem::animationDidStopCallback = newDelegate<bool, void*>(JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[0]), JS_ARGV(cx, vp)[1]);
        return JS_TRUE;
    }
    return JS_FALSE;
}

//JSBool JSTweenSystem::jsPropertySet(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> _id, JSBool strict, JS::MutableHandle<JS::Value> val)
//{
//    int32_t propId = JSID_TO_INT(_id);
//    if (propId == 1) // delegate
//    {
//        HLTweenSystem::animationDidStopCallback = newDelegate<bool, void*>(obj, val.get());
//    }
//    return JS_TRUE;
//}

NS_HL_END
