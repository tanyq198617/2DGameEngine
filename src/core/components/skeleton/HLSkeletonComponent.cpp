//
//  HLSkeletonComponent.cpp
//  HoolaiEngine
//
//  Created by tyq on 2/22/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLSkeletonComponent.h"
#include "HLFileUtil.h"
#include "HLBone.h"
#include "HLTexture.h"
#include "HLSpriteFrame.h"
#include "HLBoneAnimation.h"
#include "HLSpriteComponent.h"
#include <algorithm>

using namespace hoolai::skeleton;

NS_HL_BEGIN

INIT_FAMILYID(HLSkeletonComponent)

PROPERTIES_INITIAL(HLSkeletonComponent)
REGISTER_PROPERTY(HLSkeletonComponent, std::string, skelfile)
REGISTER_PROPERTY(HLSkeletonComponent, bool, gray)

HLSkeletonComponent::~HLSkeletonComponent()
{
    if(mAnimation)
    {
        delete mAnimation;
    }
    purgeTextureCache();
    std::map<std::string, HLBone*>::iterator itr;
    for (itr = mBones.begin(); itr != mBones.end(); ++itr)
    {
        delete itr->second;
    }
}

std::string HLSkeletonComponent::get_skelfile()
{
    return m_skelfile;
}

void HLSkeletonComponent::set_skelfile(std::string skelfile)
{
    HLASSERT(m_skelfile.length() == 0, "skelfile is already set");
    HLASSERT(skelfile.length() > 0, "invalid file name");
    
    m_skelfile = skelfile;
    char path[MAX_PATH];
    get_file_path(skelfile.c_str(), path);
    pathbase = path;
//    get_full_path(skelfile.c_str(), path);
    HLFileData *filedata = HLResourceManager::getSingleton()->getFileData(skelfile.c_str());
    if (!filedata)
    {
        delete filedata;
        printf("file (%s) not found\n", skelfile.c_str());
        return;
    }
    bool res = mSkeletonData.ParseFromArray(filedata->buffer, (int)filedata->size);
    delete filedata;
    if (!res)
    {
        printf("skeleton data parse error: %s", skelfile.c_str());
        return;
    }
    
    mAnimation = new HLBoneAnimation(this);
    int len = mSkeletonData.bones_size();
    for (int i=0; i<len; i++)
    {
        skeleton_bone* bone = mSkeletonData.mutable_bones(i);
        HLBone *b = new HLBone(this, bone);
        b->z = i;
        mBones.insert(make_pair(bone->name(), b));
    }
    std::map<std::string, HLBone*>::iterator itr;
    for (itr = mBones.begin(); itr != mBones.end(); itr++)
    {
        if (itr->second->mBoneData->has_parent())
        {
            HLBone *parent = mBones.find(itr->second->mBoneData->parent())->second;
            parent->mChildren.push_back(itr->second);
            itr->second->mParent = parent;
        }
        else
        {
            mBoneChildren.push_back(itr->second);
        }
    }
    len = mSkeletonData.contianers_size();
    for (int i=0; i<len; i++)
    {
        mContainers.insert(make_pair(mSkeletonData.mutable_contianers(i)->name(), mSkeletonData.mutable_contianers(i)));
    }
    len = mSkeletonData.images_size();
    for (int i=0; i<len; i++)
    {
        mImages.insert(make_pair(mSkeletonData.mutable_images(i)->name(), mSkeletonData.mutable_images(i)));
    }
    len = mSkeletonData.animations_size();
    for (int i=0; i<len; i++)
    {
        mAnimations.insert(make_pair(mSkeletonData.mutable_animations(i)->name(), mSkeletonData.mutable_animations(i)));
    }
    len = mSkeletonData.positions_size();
    for (int i=0; i<len; i++)
    {
        mRegPoits.insert(make_pair(mSkeletonData.mutable_positions(i)->name(), HLPoint(mSkeletonData.mutable_positions(i)->x(), mSkeletonData.mutable_positions(i)->y())));
    }
}

void HLSkeletonComponent::set_gray(bool gray)
{
    if (m_gray != gray)
    {
        m_gray = gray;
        if (this->mEntity->hasComponent<HLTransform2DComponent>())
        {
            auto children = this->mEntity->getComponent<HLTransform2DComponent>()->get_children();
            for (auto itr = children.begin(); itr != children.end(); ++itr)
            {
                HLEntity* entity = *itr;
                if (entity->hasComponent<HLSpriteComponent>())
                {
                    entity->getComponent<HLSpriteComponent>()->set_gray(gray);
                }
            }
        }
    }
}

bool HLSkeletonComponent::get_gray()
{
    return m_gray;
}

void HLSkeletonComponent::play(std::string action, bool prepared, bool loop, int durationTo, float scale, float tweenEasing)
{
    if (!mAnimation)
    {
        if (!loop)
        {
            onAnimationEvent(mEntity, action.c_str(), "complete");
        }
        return;
    }
    mPause = false;
    
    if (!prepared)
    {
        prepareTexturesForAction(action);
    }
    
    std::map<std::string, skeleton_animation*>::iterator itr = mAnimations.find(action);
    if (itr == mAnimations.end())
    {
        if (!loop)
        {
            onAnimationEvent(mEntity, action.c_str(), "complete");
        }
        return;
    }
    mAnimation->play(itr->second, itr->second->duration(), durationTo, scale, loop, tweenEasing);
}

const char* HLSkeletonComponent::getCurrentAction()
{
    if (mAnimation)
    {
        return mAnimation->getCurrentActionName();
    }
    return NULL;
}

void HLSkeletonComponent::prepareTexturesForAction(std::string action)
{
    if (!mAnimation)
    {
        return;
    }
    std::map<std::string, skeleton_animation*>::iterator itr = mAnimations.find(action);
    if (itr == mAnimations.end())
    {
        return;
    }
    std::map<std::string, skeleton_container*>containers;
    skeleton_animation *anim = itr->second;
    int count = anim->bones_size();
    for (int i=0; i<count; i++)
    {
        skeleton_animation_bone* bone = anim->mutable_bones(i);
        int count1 = bone->frames_size();
        for (int j=0; j<count1; j++)
        {
            skeleton_animation_bone_frame* frame = bone->mutable_frames(j);
            if (frame->has_container())
            {
                std::map<std::string, skeleton_container*>::iterator res = mContainers.find(frame->container());
                if (res != mContainers.end())
                {
                    if (containers.find(frame->container()) == containers.end())
                    {
                        containers.insert(make_pair(frame->container(), res->second));
                        
                        std::map<std::string, skeleton_image*>::iterator imageitr = mImages.find(res->second->mutable_images(res->second->imageindex())->name());
                        if (imageitr != mImages.end())
                        {
                            skeleton_image* image = imageitr->second;
                            if (image->has_atlas())
                            {
                                if (std::find(mSpriteframeCache.begin(), mSpriteframeCache.end(), image->atlas()) == mSpriteframeCache.end())
                                {
                                    std::string fullpath = pathbase+image->atlas();
                                    HLSpriteFrame::getSpriteFrameDictionary(fullpath.c_str());
                                    mSpriteframeCache.push_back(fullpath);
                                }
//                                HLTexture* tex = HLSpriteFrame::getSpriteFrame((pathbase+image->atlas()).c_str(), image->path().c_str())->_texture;
//                                tex->retain();
//                                mTexCache.push_back(tex);
                            }
                            else
                            {
                                HLTexture* tex = HLTexture::getTexture(pathbase+image->path());
                                if (tex)
                                {
                                    tex->retain();
                                    mTexCache.push_back(tex);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void HLSkeletonComponent::purgeTextureCache()
{
    std::vector<HLTexture*>::iterator itr = mTexCache.begin();
    for (; itr != mTexCache.end(); ++itr)
    {
        (*itr)->release();
    }
    mTexCache.clear();
    std::vector<std::string>::iterator itr1 = mSpriteframeCache.begin();
    for (; itr1 != mSpriteframeCache.end(); ++itr1)
    {
        HLSpriteFrame::purgeCachedData((*itr1).c_str());
    }
}

void HLSkeletonComponent::setContainerImageName(const char* name, const char* img)
{
    if (!mAnimation)
    {
        return;
    }
    std::map<std::string, skeleton::skeleton_container*>::iterator itr = mContainers.find(name);
    if (itr != mContainers.end())
    {
        skeleton_container* container = itr->second;
        for (int i = 0; i < container->images_size(); ++i)
        {
            if (!strcmp(img, container->images(i).name().c_str()))
            {
                container->set_imageindex(i);
                return;
            }
        }
    }
}

void HLSkeletonComponent::setContainerImageIndex(const char* name, int index)
{
    if (!mAnimation)
    {
        return;
    }
    if (index < 0)
    {
        index = 0;
    }
    
	std::map<std::string, skeleton::skeleton_container*>::iterator itr = mContainers.find(name);
    if (itr != mContainers.end())
    {
        skeleton_container* container = itr->second;
        if (index >= container->images_size())
        {
            index = container->images_size()-1;
        }
        container->set_imageindex(index);
    }
}

void HLSkeletonComponent::update(float dt)
{
    if (mAnimation && !mPause)
    {
        mAnimation->update(dt);
    }
}

void HLSkeletonComponent::addEntityToBone(const char* name, HLEntity* child)
{
    auto itr = mBones.find(name);
    if (itr != mBones.end())
    {
        itr->second->addEntity(child);
    }
}

void HLSkeletonComponent::hitTest(kmMat4* transform, const HLPoint& point, bool* ret)
{
    std::map<std::string, skeleton::HLBone*>::iterator itr = mBones.begin();
    for (; itr != mBones.end(); ++itr)
    {
        HLBone* bone = itr->second;
        if (bone->mNode)
        {
            bone->mNode->setHitTestEnabled(true);
            bone->mNode->setUserInteractionEnabled(true);
            HLEntity* e = bone->mNode->hitTest(transform, point);
            bone->mNode->setHitTestEnabled(false);
            bone->mNode->setUserInteractionEnabled(false);
            if (e)
            {
                *ret = true;
                return;
            }
        }
    }
    *ret = false;
}

NS_HL_END
