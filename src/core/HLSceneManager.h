//
//  HLScene.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-24.
//  Copyright (c) 2012 hoolai. All rights reserved.
//

#ifndef __HLSCENE_H__
#define __HLSCENE_H__

#include "HLMacros.h"
#include "HLEntitySystem.h"
#include "HLNotificationCenter.h"

#include "GameEngineConfig.h"

#include "HLDelegate.h"

#include <list>

#if defined(TARGET_MACOS)
#include <OpenThreads/ReadWriteMutex>
#endif

NS_HL_BEGIN

namespace gui
{
    class HLSceneView;
}

class HLSceneManager;

typedef CMultiDelegate1<HLSceneManager*> SceneHandler;

class HLSceneManager
{
    friend class HLEntityManager;
    friend class HLEntity;
    friend class gui::HLSceneView;
public:
    SceneHandler onActive;
    SceneHandler onDeactive;
    SceneHandler onDestroy;
    
private:
#if defined(TARGET_MACOS)
    mutable OpenThreads::ReadWriteMutex mMutex;
#endif
    HLEntityManager *mEntityMgr;
    
    HLEntity *mActiveCamera;
    kmMat4 mCameraMatrix;
    float mCameraChangeProgress;
    kmMat4 mOldCameraMatrix;
    
    gui::HLSceneView* mSceneView;
    
//#if GAME_PROJECTION == PROJECTION_3D
//    bool updateCamera;
//    
//    friend class HLCameraComponent;
//    
//public:
//    HLSceneManager():updateCamera(true),mCameraChangeProgress(true){mEntityManager=new HLEntityManager(this);}
//#else
public:
    HLSceneManager():mSceneView(NULL), mActiveCamera(NULL),mCameraChangeProgress(1){mEntityMgr=new HLEntityManager(this);kmMat4Identity(&mCameraMatrix);kmMat4Identity(&mOldCameraMatrix);};
//#endif
    
public:
    std::list<HLEntity*> entities;
    
    ~HLSceneManager();
    
    HLEntityManager* getEntityManager(){return mEntityMgr;};
    
    void updateEntity(HLEntity* entity, float deltaTime);
    void drawEntity(HLEntity* entity);
    
    void setActiveCamera(HLEntity* camera, bool active = true, bool slow = true);
    HLEntity* getActiveCamera() {return mActiveCamera;}
    
    void addEntity(HLEntity* entity)
    {
#if defined(TARGET_MACOS)
        OpenThreads::ScopedWriteLock lock(mMutex);
#endif
        entities.push_back(entity);
    }
    
    HLEntity* hitTest(const HLPoint& point);
    
    void update(float deltaTime);
    void draw();
    
//#if GAME_PROJECTION == PROJECTION_3D
//    void setActiveCamera(HLEntity* camera);
//#endif
    
    void updateCameraMatrix();
    
    kmMat4* currentCameraMatrix();
};

NS_HL_END

#endif
