//
//  HLComponent.h
//  HoolaiEngine
//
//  Created by tyq on 2/6/13.
//  Copyright (c) 2013 hoolai. All rights reserved.
//

#ifndef __HLCOMPONENT_H__
#define __HLCOMPONENT_H__

#define PROPERTIES_DECLARE \
public: \
static std::map<std::string, hoolai::PropertyBase *> _properties; \
virtual std::map<std::string, hoolai::PropertyBase *>& properties() {return _properties;}

#define PROPERTIES_INITIAL(CLASS) \
std::map<std::string, hoolai::PropertyBase *> CLASS::_properties;

#define PROPERTY_DECLARE(CLASS, TYPE, NAME) \
public:\
TYPE m_##NAME; \
static hoolai::HLPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME(); \
void set_##NAME(TYPE NAME);

#define PROPERTY_DECLARE_INIT(CLASS, TYPE, NAME, INITVALUE) \
public:\
TYPE m_##NAME{INITVALUE}; \
static hoolai::HLPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME(); \
void set_##NAME(TYPE NAME);

#define PROPERTY_READONLY_DECLARE(CLASS, TYPE, NAME) \
private:\
TYPE m_##NAME; \
static hoolai::HLPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME();

#define PROPERTY_READONLY_DECLARE_INIT(CLASS, TYPE, NAME, INITVALUE) \
private:\
TYPE m_##NAME{INITVALUE}; \
static hoolai::HLPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME();

//#define PROPERTY_DECLARE_REF(CLASS, TYPE, NAME) \
//private:\
//TYPE m_##NAME; \
//static hoolai::HLPropertyInfo<TYPE&> prop_##NAME; \
//public:\
//TYPE& get_##NAME(); \
//void set_##NAME(TYPE& NAME);
//
//#define PROPERTY_READONLY_DECLARE_REF(CLASS, TYPE, NAME) \
//private:\
//TYPE m_##NAME; \
//static hoolai::HLPropertyInfo<TYPE&> prop_##NAME; \
//public:\
//TYPE& get_##NAME();

#define PROPERTY_DECLARE_IMPLEMENT(CLASS, TYPE, NAME) \
private:\
TYPE m_##NAME; \
static hoolai::HLPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME() \
{ \
return m_##NAME; \
} \
void set_##NAME(TYPE NAME) \
{ \
m_##NAME = NAME; \
}

#define PROPERTY_DECLARE_IMPLEMENT_INIT(CLASS, TYPE, NAME, INITVALUE) \
private:\
TYPE m_##NAME{INITVALUE}; \
static hoolai::HLPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME() \
{ \
return m_##NAME; \
} \
void set_##NAME(TYPE NAME) \
{ \
m_##NAME = NAME; \
}

#define PROPERTY_READONLY_DECLARE_IMPLEMENT(CLASS, TYPE, NAME) \
private:\
TYPE m_##NAME; \
static hoolai::HLPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME() \
{ \
return m_##NAME; \
}

#define PROPERTY_READONLY_DECLARE_IMPLEMENT_INIT(CLASS, TYPE, NAME, INITVALUE) \
private:\
TYPE m_##NAME{INITVALUE}; \
static hoolai::HLPropertyInfo<TYPE> prop_##NAME; \
public:\
TYPE get_##NAME() \
{ \
return m_##NAME; \
}

//#define PROPERTY_DECLARE_IMPLEMENT_REF(CLASS, TYPE, NAME) \
//private:\
//TYPE m_##NAME; \
//static hoolai::HLPropertyInfo<TYPE&> prop_##NAME; \
//public:\
//TYPE& get_##NAME() \
//{ \
//return m_##NAME; \
//} \
//void set_##NAME(TYPE& NAME) \
//{ \
//m_##NAME = NAME; \
//}
//
//#define PROPERTY_READONLY_DECLARE_IMPLEMENT_REF(CLASS, TYPE, NAME) \
//private:\
//TYPE m_##NAME; \
//static hoolai::HLPropertyInfo<TYPE> prop_##NAME; \
//public:\
//TYPE& get_##NAME() \
//{ \
//return m_##NAME; \
//}

#define FAMILYID \
public: \
static const hoolai::FamilyId familyId; \
virtual hoolai::FamilyId getFamilyId() {return familyId;}

#define REGISTER_PROPERTY_DECLARE(CLASS, TYPE, NAME) static hoolai::HLPropertyInfo<TYPE> prop_##NAME;

#define REGISTER_PROPERTY(CLASS, TYPE, NAME) hoolai::HLPropertyInfo<TYPE> CLASS::prop_##NAME(#TYPE, CLASS::_properties, #NAME, (void (HLComponent::*)(TYPE))&CLASS::set_##NAME, (TYPE (HLComponent::*)())&CLASS::get_##NAME);
#define REGISTER_PROPERTY_REF(CLASS, TYPE, NAME) hoolai::HLPropertyInfo<TYPE&> CLASS::prop_##NAME(#TYPE, CLASS::_properties, #NAME, (void (HLComponent::*)(TYPE&))&CLASS::set_##NAME, (TYPE& (HLComponent::*)())&CLASS::get_##NAME);
#define REGISTER_PROPERTY_READONLY(CLASS, TYPE, NAME) hoolai::HLPropertyInfo<TYPE> CLASS::prop_##NAME(#TYPE, CLASS::_properties, #NAME, NULL, (TYPE (HLComponent::*)())&CLASS::get_##NAME);
#define REGISTER_PROPERTY_READONLY_REF(CLASS, TYPE, NAME) hoolai::HLPropertyInfo<TYPE&> CLASS::prop_##NAME(#TYPE, CLASS::_properties, #NAME, NULL, (TYPE& (HLComponent::*)())&CLASS::get_##NAME);

#define INIT_FAMILYID(CLASS) const hoolai::FamilyId CLASS::familyId = hoolai::StringUtil::BKDRHash(#CLASS);

#include "HLGeometry.h"
#include "HLMacros.h"
#include "kazmath/mat4.h"
#include <string>
#include <map>
#include "HLUtil.h"
#include "HLStringUtil.h"
#include "JSConversions.h"
#include "HLDelegate.h"

NS_HL_BEGIN

typedef unsigned int FamilyId;

class HLComponent;
class HLEntity;
class HLEntityManager;

typedef CDelegate2<bool, void*> HLTweenAnimationHandler;

class TweenAnimation
{
public:
    HLEntity* _target;
    TweenAnimation(HLEntity* target):_target(target) {}
    virtual ~TweenAnimation() {}
    virtual void update(float progress) = 0;
};

typedef enum
{
    HLAnimationCurveEaseInOut,
    HLAnimationCurveEaseIn,
    HLAnimationCurveEaseOut,
    HLAnimationCurveLinear
} HLAnimationCurve;

class TweenAnimations
{
private:
    int count;
    float accDt;
    int reverseCount;
public:
    std::string name;
    float duration;
    std::vector<TweenAnimation*> animations;
    HLTweenAnimationHandler callback;
    void *context;
    int repeat;
    bool stopped;
    HLAnimationCurve curve;
    float curveRate;
    bool autoreverse;
    
    
    TweenAnimations(std::string name, float duration, int repeat, void* context):name(name), duration(duration), context(context), repeat(repeat)
    {count = 0; stopped = false; accDt = 0; curve = HLAnimationCurveLinear; curveRate = 2; reverseCount = 0; autoreverse = false;}
    
    ~TweenAnimations()
    {
        std::vector<TweenAnimation*>::iterator itr;
        for (itr = animations.begin(); itr != animations.end(); itr++)
        {
            delete (*itr);
        }
    }
    
    bool update(float dt)
    {
        accDt += dt;
        
        bool reverse = false;
        if (autoreverse && reverseCount)
        {
            reverse = true;
        }
        
        if (accDt >= duration)
        {
            if (autoreverse)
            {
                reverseCount++;
                reverseCount %= 2;
            }
            std::vector<TweenAnimation*>::iterator itr;
            std::vector<TweenAnimation*> anims = animations;
            for (itr = anims.begin(); itr != anims.end(); itr++)
            {
                (*itr)->update(reverse?0:1);
            }
            if (repeat > 0)
            {
                count++;
                if (count == repeat)
                {
                    stopped = true;
                    callback(true, context);
                    return true;
                }
            }
            accDt = 0;
            //            callback(false, context);
        }
        else
        {
            float percent = accDt/duration;
            std::vector<TweenAnimation*>::iterator itr;
            switch (curve) {
                case HLAnimationCurveEaseIn:
                    percent = powf(percent, curveRate);
                    break;
                case HLAnimationCurveEaseOut:
                    percent = 1 - powf(1 - percent, curveRate);
                    break;
                case HLAnimationCurveEaseInOut:
                    percent *= 2;
                    percent = percent<1? 0.5f * powf(percent, curveRate) : 1.0f - 0.5f * powf(2 - percent, curveRate);
                    break;
                default:
                    break;
            }
            for (itr = animations.begin(); itr != animations.end(); itr++)
            {
                (*itr)->update(reverse?1-percent:percent);
            }
        }
        return false;
    }
    
};

template<typename T>
class HLTweenAnimation:public TweenAnimation
{
private:
    std::string _property;
    T _fromVal;
    T _toVal;
public:
    HLTweenAnimation(HLEntity* target, std::string property, T fromVal, T toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
    virtual void update(float progress)
    {
        
    }
};

class HLTweenSystem
{
    friend class HLEntity;
private:
    static std::vector<TweenAnimations*> allAnimations;
    static std::vector<TweenAnimations*> animationsToAdd;
public:
    static TweenAnimations *currAnimations;
    static HLTweenAnimationHandler animationDidStopCallback;
    
    template<typename T>
    static bool propertyChanged(HLEntity* entity, std::string property, T fromVal, T toVal)
    {
        if (!strcmp(property.c_str(), "position") ||
            !strcmp(property.c_str(), "rotation") ||
            !strcmp(property.c_str(), "scale") ||
            !strcmp(property.c_str(), "size") ||
            !strcmp(property.c_str(), "color") ||
            !strcmp(property.c_str(), "scaleX") ||
            !strcmp(property.c_str(), "scaleY") ||
            !strcmp(property.c_str(), "textureRect") ||
            !strcmp(property.c_str(), "brightness") ||
            !strcmp(property.c_str(), "colorPercent") ||
            !strcmp(property.c_str(), "colorAmount"))
        {
            HLTweenAnimation<T> * anim = new HLTweenAnimation<T>(entity, property, fromVal, toVal);
            currAnimations->animations.push_back(anim);
            return true;
        }
        return false;
        //        HLTweenAnimation<T> * anim = new HLTweenAnimation<T>(entity, property, fromVal, toVal);
        //        currAnimations->animations.push_back(anim);
    }
    
    static void startAnimations(std::string name, float duration, int repeat = 1, void* context = NULL)
    {
        currAnimations = new TweenAnimations(name ,duration, repeat, context);
    }
    
    static void setAutoReverse(bool b);
    
    static void update(float dt);
    
    static void commitAnimations();
    
    static void setAnimationCurve(HLAnimationCurve curve);
    
    static void setAnimationCurveRate(float curveRate);
    
    static void stopAnimation(std::string name);
    
    static void stopAnimationsContainTarget(HLEntity* target);
    
    static void stopAllAnimations();
    
    static void stopAllAnimationsWithEntityManager(HLEntityManager* mgr);
};

typedef struct _PropertyBase
{
    std::string type;
#if USE_JS_SCRIPTING
    virtual jsval getJSValue(HLComponent* owner) = 0;
    virtual void setJSValue(jsval value, HLComponent* owner, std::string name) = 0;
#endif
} PropertyBase;

template<typename T>
struct HLPropertyInfo : public PropertyBase
{
    void (HLComponent::*setter)(T);
    T (HLComponent::*getter)();
    HLPropertyInfo(std::string type, std::map<std::string, hoolai::PropertyBase *>& props, std::string name, void (HLComponent::*setter)(T), T (HLComponent::*getter)())
    {
        this->type = type;
        this->setter = setter;
        this->getter = getter;
        props.insert(make_pair(name, reinterpret_cast<PropertyBase*>(this)));
    }
#if USE_JS_SCRIPTING
    virtual jsval getJSValue(HLComponent* owner)
    {
        if (getter)
        {
            return value_to_jsval<T>((owner->*getter)());
        }
        return JSVAL_NULL;
    }
    
    virtual void setJSValue(jsval value, HLComponent* owner, std::string name);
#endif
};

class HLTouchEvent;
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
class HLMouseEvent;
#endif

class HLComponent
{
    friend class HLEntity;
    friend class HLEntityManager;
protected:
    HLEntity *mEntity;
    
    void active(HLEntity* entity) {HLASSERT(!mEntity, "Component is already attached to an entity!");mEntity = entity; onActive();}
    void deactive() {onDeactive(); mEntity = 0;}
    
public:
    HLComponent(){mEntity = 0;};
    
    HLEntity* getOwner()
    {
        return mEntity;
    }
    
    virtual ~HLComponent() {}
    
    bool hasProperty(std::string name)
    {
        return properties().find(name) != properties().end();
    }
    
    template<typename T> T getProperty(std::string name)
    {
        HLPropertyInfo<T> * info = reinterpret_cast<HLPropertyInfo<T>*>(properties().find(name)->second);
        return (this->*info->getter)();
    }
    
    template<typename T> void setProperty(std::string name, T value)
    {
        HLPropertyInfo<T> * info = reinterpret_cast<HLPropertyInfo<T>*>(properties().find(name)->second);
        (this->*(info->setter))(value);
    }
    
#if USE_JS_SCRIPTING
    jsval jsGetProperty(std::string name)
    {
        PropertyBase* info = properties().find(name)->second;
        return info->getJSValue(this);
    }
    
    void jsSetProperty(std::string name, jsval value)
    {
        PropertyBase* info = properties().find(name)->second;
        info->setJSValue(value, this, name);
    }
#endif
    
    virtual void onActive() {}
    virtual void onDeactive() {}
    
    virtual void onInternalEvent(const char* event, void* info) {}
    
    virtual FamilyId getFamilyId() = 0;
    virtual std::map<std::string, PropertyBase *>& properties() = 0;
};

template<typename T>
void HLPropertyInfo<T>::setJSValue(jsval value, HLComponent* owner, std::string name)
{
    if (setter)
    {
        if (HLTweenSystem::currAnimations)
        {
            if (HLTweenSystem::propertyChanged(owner->getOwner(), name, (owner->*getter)(), jsval_to_value<T>(value)))
                return;
        }
        (owner->*setter)(jsval_to_value<T>(value));
    }
}

NS_HL_END


#endif
