//
//  HLEntitySystem.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-17.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLEntitySystem.h"
#include "JSComponent.h"
#include "HLAnimateSystem.h"
#include "HLClassRegister.h"
#include "HLUtil.h"
#include "GameEngineConfig.h"
#include "HLTransform2DComponent.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif
#include "HLTouchDispatcher.h"
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
#include "HLMouseDispatcher.h"
#endif
#include <algorithm>

#if USE_JS_SCRIPTING
#include "JSEntitySystem.h"
#endif

#include "HLSceneView.h"

#include "HLSpriteFrame.h"

using namespace std;

NS_HL_BEGIN

HLNotificationCenter entitySystemNotifyCenter;

HLTweenAnimationHandler HLTweenSystem::animationDidStopCallback;

HLEntity::~HLEntity()
{
    if (mEntityManager->getSceneManager()->mSceneView)
    {
        mEntityManager->getSceneManager()->mSceneView->entityWillDestroy(this);
    }
    else
    {
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        HLMouseDispatcher::sharedMouseDispatcher()->entityWillDestroy(this);
#ifdef USE_TOUCH_EVENT
        HLTouchDispatcher::sharedTouchDispatcher()->entityWillDestroy(this);
#endif
#else
        HLTouchDispatcher::sharedTouchDispatcher()->entityWillDestroy(this);
#endif
    }
    std::list<HLComponent*>::iterator itr;
    for (itr = mComponents.begin(); itr != mComponents.end(); itr++)
    {
        delete (*itr);
    }
#if USE_JS_SCRIPTING
    JSEntity::removeJSObject(this);
#endif
}

HLEntity* HLEntityManager::createEntity(std::string name, const char* comps, ...)
{
    if (mEntityStore.find(name)!=mEntityStore.end())
    {
        HLASSERT(0, "entity name (%s) is duplicate\n", name.c_str());
    }
    HLEntity *entity = new HLEntity(name, this);
    mEntityStore.insert(make_pair(name, entity));
    va_list params;
    va_start(params, comps);
    while (comps) 
    {
        HLComponent* comp =  (HLComponent*)g_factory.construct(comps);
        if (!comp)
        {
            HLLOG("%s not found.\n", comps);
        }
//        comp->autorelease();
        addComponent(entity, StringUtil::BKDRHash(comps), comp);
        comps = va_arg(params, const char*);
    }
    va_end(params);
    return entity;
}

HLEntity* HLEntityManager::createAnonymousEntity(const char* comps, ...)
{
    HLEntity *entity = new HLEntity(this);
    mEntityStore.insert(make_pair(entity->mName, entity));
    va_list params;
    va_start(params, comps);
    while (comps)
    {
        HLComponent* comp =  (HLComponent*)g_factory.construct(comps);
        if (!comp)
        {
            HLLOG("%s not found.\n", comps);
        }
        //        comp->autorelease();
        addComponent(entity, StringUtil::BKDRHash(comps), comp);
        comps = va_arg(params, const char*);
    }
    va_end(params);
    return entity;
}

void HLEntity::dispatchTouchEvent(HLTouchEvent* event)
{
    if (mLastEvent == event->_id) //already dispatched
    {
        return;
    }
    mLastEvent = event->_id;
    
    std::list<HLEntity *> siblings;
    kmMat4 transform;
    kmMat4Assign(&transform, mEntityManager->getSceneManager()->currentCameraMatrix());
    
    if (hasProperty("parent")) 
    {
        HLEntity* parent = getProperty<HLEntity*>("parent");
        if (!parent) 
        {
#if GAME_PROJECTION == PROJECTION_2D
            siblings = HLDirector2D::getSingleton()->getCurrentSceneManager()->entities;
#else
            siblings = HLDirector3D::sharedDirector()->getRunningScene()->entities;
#endif
        }
        else {
            siblings = parent->getProperty<std::list<HLEntity *>>("children");
            kmMat4Assign(&transform, parent->getComponent<HLTransform2DComponent>()->nodeToWorldTransform());
        }
    }
    else 
    {
#if GAME_PROJECTION == PROJECTION_2D
        siblings = HLDirector2D::getSingleton()->getCurrentSceneManager()->entities;
#else
        siblings = HLDirector3D::sharedDirector()->getRunningScene()->entities;
#endif
    }
    
    bool findself = false;
    for (std::list<HLEntity *>::reverse_iterator itr = siblings.rbegin(); itr != siblings.rend(); itr++)
    {
        if (*itr == NULL)
        {
            continue;
        }
        if ((*itr) == this) 
        {
            findself = true;
            continue;
        }
        if (!findself) 
        {
            continue;
        }
        
        for (std::vector<HLTouch>::iterator it = event->touches.begin(); it != event->touches.end(); it++) 
        {
            HLEntity * e = NULL;
            e = (*itr)->hitTest(&transform, (*it).location);
            if (e) 
            {
                HLTouchDispatcher::sharedTouchDispatcher()->dispatchTouchEventToEntity(e, event);
                return;
            }
        }
    }
    
    if (hasProperty("parent")) 
    {
        HLEntity* parent = getProperty<HLEntity*>("parent");
        if (parent) 
        {
             HLTouchDispatcher::sharedTouchDispatcher()->dispatchTouchEventToEntity(parent, event);
        }
    }
}

#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
void HLEntity::dispatchMouseEvent(HLMouseEvent* event)
{
    if (mLastEvent == event->_id) //already dispatched
    {
        return;
    }
    mLastEvent = event->_id;
    
    std::list<HLEntity *> siblings;
    kmMat4 transform;
    kmMat4Identity(&transform);
    if (hasProperty("parent"))
    {
        HLEntity* parent = getProperty<HLEntity*>("parent");
        if (!parent)
        {
#if GAME_PROJECTION == PROJECTION_2D
            siblings = HLDirector2D::getSingleton()->getCurrentSceneManager()->entities;
#else
            siblings = HLDirector3D::sharedDirector()->getRunningScene()->entities;
#endif
        }
        else {
            siblings = parent->getProperty<std::list<HLEntity *>>("children");
            kmMat4Assign(&transform, parent->getComponent<HLTransform2DComponent>()->nodeToWorldTransform());
        }
    }
    else
    {
#if GAME_PROJECTION == PROJECTION_2D
        siblings = HLDirector2D::getSingleton()->getCurrentSceneManager()->entities;
#else
        siblings = HLDirector3D::sharedDirector()->getRunningScene()->entities;
#endif
    }
    
    bool findself = false;
    for (std::list<HLEntity *>::reverse_iterator itr = siblings.rbegin(); itr != siblings.rend(); itr++)
    {
        if ((*itr) == NULL)
        {
            continue;
        }
        if ((*itr) == this)
        {
            findself = true;
            continue;
        }
        if (!findself)
        {
            continue;
        }
        
//        for (std::vector<HLTouch>::iterator it = event->touches.begin(); it != event->touches.end(); it++)
//        {
            HLEntity * e = NULL;
            e = (*itr)->hitTest(&transform, event->location);
            if (e)
            {
                HLMouseDispatcher::sharedMouseDispatcher()->dispathMouseEventToEntity(e, event);
                return;
            }
//        }
    }
    
    if (hasProperty("parent"))
    {
        HLEntity* parent = getProperty<HLEntity*>("parent");
        if (parent)
        {
            HLMouseDispatcher::sharedMouseDispatcher()->dispathMouseEventToEntity(parent, event);
        }
    }
}
#endif

HLEntity* HLEntity::hitTest(kmMat4* preTransform, const HLPoint& point)
{
    if (!mHitTestEnabled)
    {
        return NULL;
    }
    kmMat4 transform;
    if (hasProperty("transform")) 
    {
        kmMat4Multiply(&transform, preTransform, getProperty<kmMat4*>("transform"));
    }
    else 
    {
        kmMat4Assign(&transform, preTransform);
    }
    
    if (hasProperty("children")) 
    {
        std::list<HLEntity *> children = getProperty<std::list<HLEntity*>>("children");
        
        for (std::list<HLEntity *>::reverse_iterator itr = children.rbegin(); itr != children.rend(); itr++)
        {
            if (*itr != NULL)
            {
                if ((*itr)->hasProperty("zOrder") && (*itr)->getProperty<int>("zOrder") < 0)
                {
                    break;
                }
                HLEntity *res = (*itr)->hitTest(&transform, point);
                if (res)
                {
                    return res;
                }
            }
        }
    }
    
    if (mUserInteractionEnabled)
    {
        if (!onHitTest.empty())
        {
            bool ret;
            onHitTest(&transform, point, &ret);
            return ret?this:NULL;
        }
        else 
        {
            if (!hasProperty("size") || getProperty<HLSize>("size")==HLSizeZero) 
            {
                return NULL;
            }
//            if (mTouchHandlerComponents.size() > 0)
//            {
                kmMat4 transform1;
                kmMat4Inverse(&transform1, &transform);
//                float localx = (float)((double)transform.mat[0]*point.x*CONTENT_SCALE_FACTOR+(double)transform.mat[4]*point.y*CONTENT_SCALE_FACTOR+transform.mat[12])/CONTENT_SCALE_FACTOR;
//                float localy = (float)((double)transform.mat[1]*point.x*CONTENT_SCALE_FACTOR+(double)transform.mat[5]*point.y*CONTENT_SCALE_FACTOR+transform.mat[13])/CONTENT_SCALE_FACTOR;
                float localx = (float)((double)transform1.mat[0]*point.x+(double)transform1.mat[4]*point.y+transform1.mat[12]);
                float localy = (float)((double)transform1.mat[1]*point.x+(double)transform1.mat[5]*point.y+transform1.mat[13]);
                HLSize size = getProperty<HLSize>("size");
                if (localx>=0&&localy>=0&&localx<=size.width&&localy<=size.height) 
                {
                    return this;
                }
//            }
        }
    }
    
    if (hasProperty("children")) 
    {
        std::list<HLEntity *> children = getProperty<std::list<HLEntity*>>("children");
        
        for (std::list<HLEntity *>::reverse_iterator itr = children.rbegin(); itr != children.rend(); itr++)
        {
            if (*itr != NULL)
            {
                if ((*itr)->hasProperty("zOrder") && (*itr)->getProperty<int>("zOrder") >= 0)
                {
                    continue;
                }
                HLEntity *res = (*itr)->hitTest(&transform, point);
                if (res) 
                {
                    return res;
                }
            }
        }
    }
    return NULL;
}

#if USE_JS_SCRIPTING
JSComponent * HLEntity::getJSComponent(std::string name)
{
    std::list<HLComponent*>::iterator itr = mComponents.begin();
    for (; itr != mComponents.end(); ++itr)
    {
        if ((*itr)->getFamilyId() == JSComponent::familyId)
        {
            JSComponent* comp = static_cast<JSComponent*>(*itr);
            if (comp->getFileName() == name)
            {
                return comp;
            }
        }
    }
    return NULL;
}
#endif

TweenAnimations *HLTweenSystem::currAnimations = NULL;
std::vector<TweenAnimations*> HLTweenSystem::allAnimations;
std::vector<TweenAnimations*> HLTweenSystem::animationsToAdd;

void HLEntityManager::destroyEntity(HLEntity *entity)
{
    if (!entity)
        return;
    
    if (std::find(mEntitiesToDestroy.begin(), mEntitiesToDestroy.end(), entity) != mEntitiesToDestroy.end())
        return;
    
    mEntitiesToDestroy.push_back(entity);
    
    HLASSERT(mEntityStore.find(entity->name())->second == entity, "cannot find entity");
    
    entity->onDestroy(entity);
    
    HLTweenSystem::stopAnimationsContainTarget(entity);
    
    // destroy children and remove from parent
    if (entity->hasComponent<HLTransform2DComponent>())
    {
        list<HLEntity*>& children = entity->getComponent<HLTransform2DComponent>()->getChildrenRef();
        for (std::list<HLEntity *>::iterator itr = children.begin(); itr != children.end(); itr++)
        {
            if (*itr)
                destroyEntity(*itr);
        }
        
        list<HLEntity*>& childrenToAdd = entity->getComponent<HLTransform2DComponent>()->_childrenToAdd;
        for (std::list<HLEntity *>::iterator itr = childrenToAdd.begin(); itr != childrenToAdd.end(); itr++)
        {
            if (*itr)
                destroyEntity(*itr);
        }
        childrenToAdd.clear();
        
        HLEntity* parent = entity->getProperty<HLEntity*>("parent");
        if (parent && parent->hasComponent<HLTransform2DComponent>())
        {
            list<HLEntity*>& parentchildren = parent->getComponent<HLTransform2DComponent>()->getChildrenRef();
            list<HLEntity*>::iterator itr = std::find(parentchildren.begin(), parentchildren.end(), entity);
            if (itr != parentchildren.end())
            {
                *itr = NULL;
            }
        }
    }
    
    // deactive components
    std::list<HLComponent*>::iterator itr1;
    for (itr1 = entity->mComponents.begin(); itr1 != entity->mComponents.end(); itr1++)
    {
        (*itr1)->deactive();
    }
    
    std::list<HLEntity*>::iterator itr = std::find(mSceneMgr->entities.begin(), mSceneMgr->entities.end(), entity);
    if (itr != mSceneMgr->entities.end())
    {
        *itr = NULL;
    }
    
    mEntityStore.erase(entity->name());
    std::multimap<FamilyId, HLEntity*>::iterator iter = mComponentStore.begin();
    while (iter != mComponentStore.end())
    {
        if (entity == iter->second)
        {
            mComponentStore.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
//    mComponentStore = tmp;
//    delete entity;
}

HLEntityManager::~HLEntityManager()
{
    // destroy entities in scene
    std::list<HLEntity*>::iterator itr = mSceneMgr->entities.begin();
    for (; itr != mSceneMgr->entities.end(); ++itr)
    {
        destroyEntity(*itr);
    }
    // destroy other entities
    std::map<std::string, HLEntity*>::iterator itr1 = mEntityStore.begin();
    while(itr1 != mEntityStore.end())
    {
        destroyEntity(itr1->second);
        itr1 = mEntityStore.begin();
    }
    destroyEntities();
    HLSpriteFrame::purgeCachedData();
#if USE_JS_SCRIPTING
    JSEntityManager::removeJSObject(this);
#endif
}

void HLEntityManager::addComponent(HLEntity * e, FamilyId familyId, HLComponent* comp, int pos)
{
    e->onInternalEvent("component_will_add", comp);
    mComponentStore.insert(std::pair<FamilyId, HLEntity*>(familyId, e));
    if (pos >= e->mComponents.size())
        pos = -1;
    if (pos == -1)
    {
        e->mComponents.push_back(comp);
    }
    else
    {
        std::list<HLComponent*>::iterator itr = e->mComponents.begin();
        std::advance(itr, pos);
        e->mComponents.insert(itr, comp);
    }
    e->mComponentsMap.insert(std::pair<FamilyId, HLComponent*>(familyId, comp));
    comp->active(e);
}

void HLTweenSystem::setAutoReverse(bool b)
{
    if (currAnimations)
    {
        currAnimations->autoreverse = b;
    }
}

void HLTweenSystem::update(float dt)
{
    if (animationsToAdd.begin() != animationsToAdd.end())
    {
        allAnimations.insert(allAnimations.end(), animationsToAdd.begin(), animationsToAdd.end());
        animationsToAdd.clear();
    }
    std::vector<TweenAnimations*>::iterator itr;
    for (itr = allAnimations.begin(); itr != allAnimations.end();)
    {
        if ((*itr) == NULL)
        {
            itr = allAnimations.erase(itr);
        }
        else if ((*itr)->update(dt))
        {
            delete (*itr);
            itr = allAnimations.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
}

void HLTweenSystem::commitAnimations()
{
    if (currAnimations->animations.size())
    {
        currAnimations->callback = animationDidStopCallback;
//        printf("commitAnimations %ld\n", currAnimations);
        animationsToAdd.push_back(currAnimations);
    }
    currAnimations = NULL;
    animationDidStopCallback.clear();
}

void HLTweenSystem::setAnimationCurve(HLAnimationCurve curve)
{
    if (currAnimations)
    {
        currAnimations->curve = curve;
    }
}

void HLTweenSystem::setAnimationCurveRate(float curveRate)
{
    if (currAnimations)
    {
        currAnimations->curveRate = curveRate;
    }
}

void HLTweenSystem::stopAnimation(std::string name)
{
    std::vector<TweenAnimations*>::iterator it;
    for (it = allAnimations.begin(); it != allAnimations.end(); )
    {
        TweenAnimations* anim = *it;
        if (anim && anim->name == name && !anim->stopped)
        {
            *it = NULL;
            anim->callback(false, anim->context);
            delete anim;
        }
        else
        {
            ++it;
        }
    }
    
    for (it = animationsToAdd.begin(); it != animationsToAdd.end(); )
    {
        TweenAnimations* anim = *it;
        if (anim && anim->name == name && !anim->stopped)
        {
            *it = NULL;
            anim->callback(false, anim->context);
            delete anim;
        }
        else
        {
            ++it;
        }
    }
}

void HLTweenSystem::stopAnimationsContainTarget(HLEntity* target)
{
    std::vector<TweenAnimations*>::iterator it;
    for (it = allAnimations.begin(); it != allAnimations.end(); )
    {
        TweenAnimations* anim = *it;
        if (anim&&!anim->stopped)
        {
            std::vector<TweenAnimation*>::iterator iter = anim->animations.begin();
            for (; iter != anim->animations.end(); ++iter)
            {
                if ((*iter)->_target == target)
                {
                    *it = NULL; // avoid call recursively
                    anim->callback(false, anim->context);
                    delete anim;
                    break;
                }
            }
        }
        ++it;
    }
    
    for (it = animationsToAdd.begin(); it != animationsToAdd.end(); )
    {
        TweenAnimations* anim = *it;
        if (anim)
        {
            std::vector<TweenAnimation*>::iterator iter = (anim)->animations.begin();
            for (; iter != anim->animations.end(); ++iter)
            {
                if ((*iter)->_target == target)
                {
                    *it = NULL;
                    anim->callback(false, anim->context);
                    delete anim;
                    break;
                }
            }
        }
        ++it;
    }
}

void HLTweenSystem::stopAllAnimations()
{
    std::vector<TweenAnimations*>::iterator it;
    for (it = allAnimations.begin(); it != allAnimations.end(); it++)
    {
        TweenAnimations* anim = *it;
        if (anim&&!anim->stopped)
        {
            *it = NULL;
            anim->callback(false, anim->context);
            delete anim;
        }
    }
    allAnimations.clear();
    
    for (it = animationsToAdd.begin(); it != animationsToAdd.end(); ++it)
    {
        TweenAnimations* anim = *it;
        if (anim)
        {
            *it = NULL;
            anim->callback(false, anim->context);
            delete anim;
        }
    }
    animationsToAdd.clear();
}

void HLTweenSystem::stopAllAnimationsWithEntityManager(HLEntityManager* mgr)
{
    std::vector<TweenAnimations*>::iterator it;
    for (it = allAnimations.begin(); it != allAnimations.end(); it++)
    {
        TweenAnimations* anim = *it;
        if (anim && !anim->stopped && anim->animations[0]->_target->getEntityManager() == mgr)
        {
            *it = NULL;
            anim->callback(false, anim->context);
            delete anim;
        }
    }
    
    for (it = animationsToAdd.begin(); it != animationsToAdd.end(); ++it)
    {
        TweenAnimations* anim = *it;
        if (anim==NULL)
            continue;
        std::vector<TweenAnimation*>::iterator iter = anim->animations.begin();
        for (; iter != anim->animations.end(); ++iter)
        {
            if ((*iter)->_target->getEntityManager() == mgr)
            {
                (*it) = NULL;
                anim->callback(false, anim->context);
                delete anim;
                break;
            }
        }
    }
}

NS_HL_END
