//
//  HLEntitySystem.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-5-17.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLHLEntityManager_H__
#define __HLHLEntityManager_H__

#include "GameEngineConfig.h"
#include "HLNotificationCenter.h"
#include "HLGeometry.h"
#include "HLUtil.h"
#include "kazmath/mat4.h"
#include <map>
#if USE_JS_SCRIPTING
#include "JSEntitySystem.h"
#endif

NS_HL_BEGIN

#if USE_JS_SCRIPTING
class JSComponent;
#endif

NS_HL_END

#include "HLComponent.h"

#include <algorithm>

NS_HL_BEGIN

extern HLNotificationCenter entitySystemNotifyCenter;

class HLEntity;

class HLTouchEvent;
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
class HLMouseEvent;
#endif

class HLEntityManager;

namespace gui
{
    class HLEntityView;
}

typedef CMultiDelegate1<float> HLEntityUpdateHandler;
typedef CMultiDelegate0 HLEntityDrawHandler;
typedef CMultiDelegate1<HLTouchEvent*> HLEntityTouchHandler;
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
typedef CMultiDelegate1<HLMouseEvent*> HLEntityMouseHandler;
#endif
typedef CDelegate3<kmMat4*, const HLPoint&, bool*> HLEntityHitTestHandler;

//typedef void (HLComponent::*HLEntityUpdateCallback)(float);
//typedef struct 
//{
//    HLComponent* target;
//    HLEntityUpdateCallback func;
//} HLEntityUpdateComponentHandler;
//
//typedef void (HLComponent::*HLEntityDrawCallback)();
//typedef struct
//{
//    HLComponent* target;
//    HLEntityDrawCallback func;
//} HLEntityDrawComponentHandler;
//
//typedef void (HLComponent::*HLEntityTouchCallback)(HLTouchEvent*);
//typedef struct
//{
//    HLComponent* target;
//    HLEntityTouchCallback func;
//} HLTouchyDrawComponentHandler;
//
//#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
//typedef void (HLComponent::*HLEntityMouseCallback)(HLMouseEvent*);
//typedef struct
//{
//    HLComponent* target;
//    HLEntityMouseCallback func;
//} HLTouchyMouseComponentHandler;
//#endif
//
//typedef void (HLComponent::*HLEntityHitTestCallback)(kmMat4*, const HLPoint&, bool*);
//typedef struct
//{
//    HLComponent* target;
//    HLEntityHitTestCallback func;
//} HLHitTestDrawComponentHandler;

class HLEntity
{
    friend class HLDirector2D;
    friend class HLEntityManager;
    friend class HLSceneManager;
    friend class HLTouchDispatcher;
    friend class HLMouseDispatcher;
    friend class JSEntity;
    friend class gui::HLEntityView;
private:
    int mLastEvent;
    bool mVisible;
    bool mEnabled;
    bool mUserInteractionEnabled;
    bool mHitTestEnabled;
    bool mMultiTouchEnabled;
    std::list<HLComponent*> mComponents;
    std::map<FamilyId, HLComponent*> mComponentsMap;
//    std::map<FamilyId, HLComponent*> mComponents;
//    std::list<HLComponent*> mUpdatableComponents;
//    std::list<HLComponent*> mDrawableComponents;
//    std::list<HLComponent*> mTouchHandlerComponents;
//#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
//    std::list<HLComponent*> mMouseHandlerComponents;
//#endif
    HLEntityManager* mEntityManager;
//    std::map<std::string, HLComponent*> mProperties;
    std::string mName;
    HLEntity(std::string name, HLEntityManager *mgr):mLastEvent(0), mEnabled(true), mVisible(true), mEntityManager(mgr), mUserInteractionEnabled(false), mHitTestEnabled(true) { mName = name; }
    HLEntity(HLEntityManager *mgr):mLastEvent(0), mEnabled(true), mVisible(true), mEntityManager(mgr), mUserInteractionEnabled(false), mHitTestEnabled(true), mMultiTouchEnabled(false) {char tmp[25]; sprintf(tmp, "%lld", (unsigned long long)this); mName = std::string(tmp);}
public:
    HLEntityUpdateHandler onUpdate;
    HLEntityDrawHandler onDraw;
    HLEntityTouchHandler onTouchEvent;
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    HLEntityMouseHandler onMouseEvent;
#endif
    HLEntityHitTestHandler onHitTest;
    CMultiDelegate1<HLEntity*> onDestroy;
    
    HLEntityManager* getEntityManager()
    {
        return mEntityManager;
    }
    
    ~HLEntity();
    
    void setUserInteractionEnabled(bool b)
    {
        mUserInteractionEnabled = b;
    }
    
    bool userInteractionEnabled()
    {
        return mUserInteractionEnabled;
    }
    
    void setHitTestEnabled(bool b)
    {
        mHitTestEnabled = b;
    }
    
    bool hitTestEnalbed()
    {
        return mHitTestEnabled;
    }
    
    void setMultiTouchEnabled(bool b)
    {
        mMultiTouchEnabled = b;
    }
    
    bool multiTouchEnabled()
    {
        return mMultiTouchEnabled;
    }
    
    bool isVisible()
    {
        return mVisible;
    }
    
    bool isEnabled()
    {
        return mEnabled;
    }
    
    void setVisible(bool b) 
    {
        mVisible = b;
    }
    
    void setEnabled(bool b) 
    {
        mEnabled = b;
    }
    
    const char * name()
    {
        return mName.c_str();
    }
    
    void onInternalEvent(const char* event, void* info)
    {
        std::list<HLComponent*>::iterator itr;
        for (itr = mComponents.begin(); itr != mComponents.end(); ++itr)
        {
            (*itr)->onInternalEvent(event, info);
        }
    }
    
#if USE_JS_SCRIPTING
    template<typename T>
    void setJSProperty(const char* name, T value)
    {
        JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
        JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
        JSEntity *e = JSEntity::getOrCreateWrapper(cx, this);
        JS::RootedValue val(cx, value_to_jsval(value));
        JS_SetProperty(cx, e->jsobject, name, val);
    }
    
    JSComponent * getJSComponent(std::string name);
#endif
    
    template<typename T> void addComponent(T * comp, int pos = -1);
    template<typename T> void addComponent(int pos = -1);
    template<typename T> bool hasComponent();
    template<typename T> T* getComponent();
    template<typename T> std::list< T* > getComponents();
    template<typename T> void removeComponent();
    void removeComponent(HLComponent* component);
    template<typename T> T getProperty(std::string name)
    {
        std::list<HLComponent*>::iterator itr = mComponents.begin();
        for (; itr != mComponents.end(); ++itr)
        {
            if ((*itr)->hasProperty(name))
            {
                return (*itr)->getProperty<T>(name);
            }
        }
        HLASSERT(0, "Property %s does not exist", name.c_str());
    }
    template<typename T> void setProperty(std::string name, T val);
    bool hasProperty(std::string name)
    {
        std::list<HLComponent*>::iterator itr = mComponents.begin();
        for (; itr != mComponents.end(); ++itr)
        {
            if ((*itr)->hasProperty(name))
            {
                return true;
            }
        }
        return false;
    }
    
#if USE_JS_SCRIPTING
    jsval jsGetProperty(std::string name)
    {
        std::list<HLComponent*>::iterator itr = mComponents.begin();
        for (; itr != mComponents.end(); ++itr)
        {
            if ((*itr)->hasProperty(name))
            {
                return (*itr)->jsGetProperty(name);
            }
        }
        HLASSERT(0, "Property %s does not exist", name.c_str());
    }
    
    void jsSetProperty(std::string name, jsval value)
    {
        std::list<HLComponent*>::iterator itr = mComponents.begin();
        for (; itr != mComponents.end(); ++itr)
        {
            if ((*itr)->hasProperty(name))
            {
                (*itr)->jsSetProperty(name, value);
                return;
            }
        }
        JSContext *cx = JSScriptingCore::getSingleton()->getGlobalContext();
        JSAutoCompartment ac(cx, JSScriptingCore::getSingleton()->getGlobalJSObject());
        JSEntity *e = JSEntity::getOrCreateWrapper(cx, this);
        JS::RootedValue val(cx, value);
        JS_SetProperty(cx, e->jsobject, name.c_str(), val);
    }
#endif
    
    void dispatchTouchEvent(HLTouchEvent* event);
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    void dispatchMouseEvent(HLMouseEvent* event);
#endif
    HLEntity* hitTest(kmMat4* preTransform, const HLPoint& point);
};

class HLSceneManager;

class HLEntityManager
{
    friend class HLDirector2D;
    friend class JSEntityManager;
    friend class JSEntity;
    friend class HLSceneManager;
private:
    HLSceneManager *mSceneMgr;
    
    std::list<HLEntity*> mEntitiesToDestroy;
    
    HLEntityManager();
    HLEntityManager(const HLEntityManager&);
    HLEntityManager& operator=(const HLEntityManager&);
    
    std::multimap<FamilyId, HLEntity*> mComponentStore;
    std::map<std::string, HLEntity*> mEntityStore;
    HLEntityManager(HLSceneManager *sceneMgr):mSceneMgr(sceneMgr){}
    
    void addComponent(HLEntity *e, FamilyId familyId, HLComponent* comp, int pos = -1);
    
public:
    ~HLEntityManager();
    
    HLSceneManager* getSceneManager(){return mSceneMgr;};
    
    template<typename T> void getEntities(std::vector<HLEntity*> &result) 
    {
        std::pair<std::multimap<FamilyId, HLEntity*>::iterator, std::multimap<FamilyId, HLEntity*>::iterator> iterPair = mComponentStore.equal_range(T::familyId);
        for(std::multimap<FamilyId, HLEntity*>::iterator iter = iterPair.first; iter != iterPair.second; ++iter) 
        {
            result.push_back(iter->second);
        }
    }
    
    HLEntity* getEntity(std::string name)
    {
        std::map<std::string, HLEntity*>::iterator itr = mEntityStore.find(name);
        if (itr != mEntityStore.end()) 
        {
            return itr->second;
        }
        return NULL;
    }
    
    template<typename T> void addComponent(HLEntity *e, T* comp, int pos)
    {
        addComponent(e, T::familyId, comp, pos);
    }
    
    template<typename T> void removeComponent(HLEntity *e)
    {
        // TODO assert component in enity
        removeComponent(e, e->getComponent<T>());
        
    }
    
    void removeComponent(HLEntity* e, HLComponent* comp)
    {
        std::list<HLComponent*>::iterator itr = std::find(e->mComponents.begin(), e->mComponents.end(), comp);//e->mComponents.find(T::familyId);
        HLASSERT(itr!=e->mComponents.end(), "entity dosnot contain specified component");
        comp->deactive();
        e->onInternalEvent("component_will_remove", comp);
        e->mComponents.erase(itr);
        std::map<FamilyId, HLComponent*>::iterator itr1 = e->mComponentsMap.find(comp->getFamilyId());
        if (itr1 != e->mComponentsMap.end() && itr1->second == comp)
        {
            e->mComponentsMap.erase(itr1);
        }
        std::pair<std::multimap<FamilyId, HLEntity*>::iterator, std::multimap<FamilyId, HLEntity*>::iterator> iterPair = mComponentStore.equal_range(comp->getFamilyId());
        for(std::multimap<FamilyId, HLEntity*>::iterator iter = iterPair.first; iter != iterPair.second; ++iter)
        {
            if (iter->second == e)
            {
                mComponentStore.erase(iter);
                break;
            }
        }
        delete comp;
    }
    
    HLEntity* createEntity(std::string name)
    {
        if (mEntityStore.find(name)!=mEntityStore.end()) 
        {
            return NULL;
        }
        HLEntity *entity = new HLEntity(name, this);
        mEntityStore.insert(make_pair(name, entity));
        return entity;
    }
    
    HLEntity* createEntity(std::string name, const char* comps, ...);
    HLEntity* createAnonymousEntity(const char* comps, ...);
    
    void destroyEntity(std::string name)
    {
        // TODO post notification for objects contains this entity
        destroyEntity(mEntityStore.find(name)->second);
    }
    
    void destroyEntity(HLEntity *entity);
    
private:
    void destroyEntities()
    {
        for (std::list<HLEntity*>::iterator itr = mEntitiesToDestroy.begin(); itr != mEntitiesToDestroy.end(); ++itr)
        {
            delete (*itr);
        }
        mEntitiesToDestroy.clear();
    }
};

template <>
class HLTweenAnimation<HLPoint>:public TweenAnimation
{
private:
    std::string _property;
    HLPoint _fromVal;
    HLPoint _toVal;
public:
    HLTweenAnimation(HLEntity* target, std::string property, HLPoint fromVal, HLPoint toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
    virtual void update(float progress)
    {
        HLPoint val = _fromVal + (_toVal - _fromVal) * progress;
        _target->setProperty<HLPoint>(_property, val);
    }
};

template <>
class HLTweenAnimation<HLSize>:public TweenAnimation
{
private:
    std::string _property;
    HLSize _fromVal;
    HLSize _toVal;
public:
    HLTweenAnimation(HLEntity* target, std::string property, HLSize fromVal, HLSize toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
    virtual void update(float progress)
    {
        HLSize val(_fromVal.width + (_toVal.width-_fromVal.width)*progress, _fromVal.height + (_toVal.height-_fromVal.height)*progress);
        _target->setProperty<HLSize>(_property, val);
    }
};

template <>
class HLTweenAnimation<HLRect>:public TweenAnimation
{
private:
    std::string _property;
    HLRect _fromVal;
    HLRect _toVal;
public:
    HLTweenAnimation(HLEntity* target, std::string property, HLRect fromVal, HLRect toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
    virtual void update(float progress)
    {
        HLRect val;
        val.origin = _fromVal.origin + (_toVal.origin - _fromVal.origin) * progress;
        val.size = HLSize(_fromVal.size.width + (_toVal.size.width-_fromVal.size.width)*progress, _fromVal.size.height + (_toVal.size.height-_fromVal.size.height)*progress);
        _target->setProperty<HLRect>(_property, val);
    }
};

template <>
class HLTweenAnimation<int>:public TweenAnimation
{
private:
    std::string _property;
    int _fromVal;
    int _toVal;
public:
    HLTweenAnimation(HLEntity* target, std::string property, int fromVal, int toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
    virtual void update(float progress)
    {
        int val = _fromVal + (_toVal - _fromVal) * progress;
        _target->setProperty<int>(_property, val);
    }
};

template <>
class HLTweenAnimation<float>:public TweenAnimation
{
private:
    std::string _property;
    float _fromVal;
    float _toVal;
public:
    HLTweenAnimation(HLEntity* target, std::string property, float fromVal, float toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
    virtual void update(float progress)
    {
        float val = _fromVal + (_toVal - _fromVal) * progress;
        _target->setProperty<float>(_property, val);
    }
};

template <>
class HLTweenAnimation<color4B>:public TweenAnimation
{
private:
    std::string _property;
    color4B _fromVal;
    color4B _toVal;
public:
    HLTweenAnimation(HLEntity* target, std::string property, color4B fromVal, color4B toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
    virtual void update(float progress)
    {
        color4B val = {static_cast<GLubyte>(_fromVal.r + (_toVal.r - _fromVal.r) * progress),
                        static_cast<GLubyte>(_fromVal.g + (_toVal.g - _fromVal.g) * progress),
                        static_cast<GLubyte>(_fromVal.b + (_toVal.b - _fromVal.b) * progress),
                        static_cast<GLubyte>(_fromVal.a + (_toVal.a - _fromVal.a) * progress)};
        _target->setProperty<color4B>(_property, val);
    }
};

template <>
class HLTweenAnimation<color4F>:public TweenAnimation
{
private:
    std::string _property;
    color4F _fromVal;
    color4F _toVal;
public:
    HLTweenAnimation(HLEntity* target, std::string property, color4F fromVal, color4F toVal):TweenAnimation(target), _property(property), _fromVal(fromVal), _toVal(toVal){}
    virtual void update(float progress)
    {
        color4F val = {static_cast<float>(_fromVal.r + (_toVal.r - _fromVal.r) * progress),
                        static_cast<float>(_fromVal.g + (_toVal.g - _fromVal.g) * progress),
                        static_cast<float>(_fromVal.b + (_toVal.b - _fromVal.b) * progress),
                        static_cast<float>(_fromVal.a + (_toVal.a - _fromVal.a) * progress)};
        _target->setProperty<color4F>(_property, val);
    }
};

template<typename T> void HLEntity::setProperty(std::string name, T val)
{
    if (HLTweenSystem::currAnimations) 
    {
        if (HLTweenSystem::propertyChanged(this, name, getProperty<T>(name), val))
            return;
    }
#if USE_JS_SCRIPTING
    std::list<HLComponent*>::iterator itr = mComponents.begin();
    for (; itr != mComponents.end(); ++itr)
    {
        if ((*itr)->hasProperty(name))
        {
            (*itr)->setProperty<T>(name, val);
            return;
        }
    }
//    if (hasComponent<JSComponent>())
//    {
//        
//        getComponent<JSComponent>()->setProperty(name.c_str(), val);
//    }
    setJSProperty(name.c_str(), val);
#else
    std::list<HLComponent*>::iterator itr = mComponents.begin();
    for (; itr != mComponents.end(); ++itr)
    {
        if ((*itr)->hasProperty(name))
        {
            (*itr)->setProperty<T>(name, val);
            return;
        }
    }
#endif
}

template<typename T> void HLEntity::addComponent(T * comp, int pos)
{
    mEntityManager->addComponent<T>(this, comp, pos);
}

template<typename T> void HLEntity::addComponent(int pos)
{
    T* comp = new T();
    mEntityManager->addComponent<T>(this, comp, pos);
}

template<typename T> bool HLEntity::hasComponent()
{
    std::map<FamilyId, HLComponent*>::iterator itr = mComponentsMap.find(T::familyId);
    return itr != mComponentsMap.end();
}

template<typename T> T* HLEntity::getComponent()
{
    std::map<FamilyId, HLComponent*>::iterator itr = mComponentsMap.find(T::familyId);
    if (itr == mComponentsMap.end())
    {
        return NULL;
    }
    return static_cast<T*>(itr->second);
}

template<typename T> std::list< T* > HLEntity::getComponents()
{
    std::list< T* > res;
    std::list<HLComponent*>::iterator itr = mComponents.begin();
    for (; itr != mComponents.end(); ++itr) {
        if ((*itr)->getFamilyId() == T::familyId)
        {
            res.push_back(static_cast<T*>(*itr));
        }
    }
    return res;
}

template<typename T> void HLEntity::removeComponent()
{
    mEntityManager->removeComponent<T>(this);
}

NS_HL_END

#endif
