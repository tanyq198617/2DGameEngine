//
//  HLSkeletonComponent.h
//  HoolaiEngine
//
//  Created by tyq on 2/22/13.
//  Copyright (c) 2013 hoolai. All rights reserved.
//

#ifndef __HLSKELETONCOMPOENT_H__
#define __HLSKELETONCOMPOENT_H__

#include "HLEntitySystem.h"
#include "HLTransform2DComponent.h"
#include "HLDelegate.h"
#include "skeleton.pb.h"
#include <limits>

NS_HL_BEGIN

namespace skeleton
{
    class HLBone;
    class HLBoneAnimation;
}

class HLTexture;

typedef CDelegate3<HLEntity*, const char*, const char*> AnimationEventHandler;

class HLSkeletonComponent: public HLComponent
{
    friend class skeleton::HLBoneAnimation;
    friend class skeleton::HLBone;
    
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLSkeletonComponent, std::string, skelfile);
    PROPERTY_DECLARE(HLSpriteComponent, bool, gray);
    
public:
    AnimationEventHandler onAnimationEvent;
    
    HLSkeletonComponent():mAnimation(NULL),mPause(true),m_gray(false){}
    
    ~HLSkeletonComponent();
    
    virtual void onActive()
    {
        HLASSERT(mEntity->hasComponent<HLTransform2DComponent>(), "entity must have HLTransform2DComponent");
        mEntity->onUpdate += newDelegate(this, &HLSkeletonComponent::update);
        mEntity->onHitTest = newDelegate(this, &HLSkeletonComponent::hitTest);
    }
    
    virtual void onDeactive()
    {
        purgeTextureCache();
        mEntity->onUpdate -= newDelegate(this, &HLSkeletonComponent::update);
        if (mEntity->onHitTest == newDelegate(this, &HLSkeletonComponent::hitTest))
        {
            mEntity->onHitTest.clear();
        }
        
    }
    
    skeleton::HLBone* getBone(const char* name)
    {
        std::map<std::string, skeleton::HLBone*>::iterator itr = mBones.find(name);
        return itr==mBones.end()?NULL:itr->second;
    }
    
    const HLPoint& getRegisterPoint(const char* name)
    {
        std::map<std::string, HLPoint>::iterator itr = mRegPoits.find(name);
        if (itr != mRegPoits.end())
        {
            return itr->second;
        }
        return HLPointZero;
    }
    
    void addEntityToBone(const char* name, HLEntity* child);
    
    void hitTest(kmMat4* transform, const HLPoint& point, bool* ret);
    
    void play(std::string action, bool prepared = false, bool loop = false, int durationTo = 0, float scale = 1, float tweenEasing = NAN);
    
    const char* getCurrentAction();
    
    void prepareTexturesForAction(std::string action);
    
    void purgeTextureCache();
    
    void setContainerImageIndex(const char* name, int index);
    void setContainerImageName(const char* name, const char* img);
    
    void update(float dt);
    
    void pause() {mPause = true;}
    void resume() {mPause = false;}
    
private:
    std::vector<HLTexture*>mTexCache;
    std::vector<std::string> mSpriteframeCache;
    std::string pathbase;
    skeleton::HLBoneAnimation *mAnimation;
    std::map<std::string, skeleton::HLBone*> mBones;
    std::vector<skeleton::HLBone*> mBoneChildren;
    std::map<std::string, skeleton::skeleton_container*> mContainers;
    std::map<std::string, skeleton::skeleton_image*> mImages;
    std::map<std::string, skeleton::skeleton_animation*> mAnimations;
    std::map<std::string, HLPoint> mRegPoits;
    bool mPause;
    skeleton::skeleton mSkeletonData;
    
    FAMILYID
};

NS_HL_END

#endif
