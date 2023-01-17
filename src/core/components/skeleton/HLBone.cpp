//
//  HLBone.cpp
//  HoolaiEngine
//
//  Created by tyq on 2/22/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLBone.h"
#include "HLSkeletonComponent.h"
#include "HLTexture.h"
#include "HLSpriteFrame.h"
#include "HLSpriteComponent.h"
#include "HLColorAdvanceComponent.h"
#include "HLColorComponent.h"
#include "HLBlendFuncComponent.h"

NS_HL_BEGIN

namespace skeleton
{
    
HLBone::HLBone(HLSkeletonComponent* skeleton, skeleton_bone* boneData):mSkeleton(skeleton), x(0),y(0),skewX(0),skewY(0),scaleX(1),scaleY(1),mBoneData(boneData),mNode(NULL),mParent(NULL),mTween(this, skeleton->mAnimation)
    {
        mColorComp = NULL;
        mColorAdvComp = NULL;
        if (mSkeleton->mEntity->hasComponent<HLColorComponent>())
        {
            mColorComp = mSkeleton->mEntity->getComponent<HLColorComponent>();
        }
        else if (mSkeleton->mEntity->hasComponent<HLColorAdvanceComponent>())
        {
            mColorAdvComp = mSkeleton->mEntity->getComponent<HLColorAdvanceComponent>();
        }
    }
    
void HLBone::setContainer(std::string name)
{
    if (name.empty())
    {
        return;
    }
    std::map<std::string, skeleton_container*>::iterator itr = mSkeleton->mContainers.find(name);
    if (itr == mSkeleton->mContainers.end())
    {
        return;
    }
    if (!mNode)
    {
        mNode = mSkeleton->mEntity->getEntityManager()->createAnonymousEntity("HLTransform2DComponent", "HLSpriteComponent", "HLColorAdvanceComponent", "HLBlendFuncComponent", NULL);
        mNode->setHitTestEnabled(false);
//        mNode->removeFromParentAndCleanup(true);
//        mNode = NULL;
        mSkeleton->mEntity->getComponent<HLTransform2DComponent>()->addChild(mNode, z);
        mNode->getComponent<HLSpriteComponent>()->set_gray(mSkeleton->get_gray());
    }

    if (itr->second->images_size() == 0)
    {
        return;
    }
    std::map<std::string, skeleton_image*>::iterator imageitr = mSkeleton->mImages.find(itr->second->mutable_images(itr->second->imageindex())->name());
    if (imageitr != mSkeleton->mImages.end())
    {
        skeleton_image* image = imageitr->second;
        mNode->setProperty("size", HLSize(image->width(), image->height()));
        if (image->has_atlas())
        {
            HLSpriteFrame* spriteFrame = HLSpriteFrame::getSpriteFrame((mSkeleton->pathbase+image->atlas()).c_str(), image->path().c_str());
            if (spriteFrame)
            {
                spriteFrame->setDisplayFrameForEntity(mNode);
            }
        }
        else
        {
            HLTexture* tex = HLTexture::getTexture(mSkeleton->pathbase+image->path());
            if (tex)
            {
                mNode->setProperty("texture", tex);
                mNode->setProperty("offsetPosFromBL", HLPointZero);
                mNode->setProperty("untrimmedSize", HLSizeZero);
                mNode->setProperty("textureRotated", false);
            }
        }
    }
    
    float x = itr->second->mutable_images(itr->second->imageindex())->x();
    float y = itr->second->mutable_images(itr->second->imageindex())->y();
    mNode->setProperty("anchorPoint", HLPoint(x, y));
}
    
void HLBone::addEntity(HLEntity* child)
{
    if (!mNode)
    {
        mNode = mSkeleton->mEntity->getEntityManager()->createAnonymousEntity("HLTransform2DComponent", "HLSpriteComponent", "HLColorAdvanceComponent", "HLBlendFuncComponent", NULL);
        mNode->setHitTestEnabled(false);
        //        mNode->removeFromParentAndCleanup(true);
        //        mNode = NULL;
        mSkeleton->mEntity->getComponent<HLTransform2DComponent>()->addChild(mNode, z);
    }
    mNode->getComponent<HLTransform2DComponent>()->addChild(child);
}
    
void HLBone::setBlend(blendmode blend)
{
    if (!mNode)
    {
        return;
    }
    HLBlendFuncComponent* comp = mNode->getComponent<HLBlendFuncComponent>();
    switch (blend)
    {
        case multiply:
            comp->set_preMultiAlpha(false);
            comp->set_blendEquation(GL_FUNC_ADD);
            comp->set_sfactor(GL_DST_COLOR);
            comp->set_dfactor(GL_ONE_MINUS_SRC_ALPHA);
            break;
        case add:
            comp->set_preMultiAlpha(false);
            comp->set_blendEquation(GL_FUNC_ADD);
            comp->set_sfactor(GL_SRC_ALPHA);
            comp->set_dfactor(GL_ONE);
            break;
        case subtract:
            comp->set_preMultiAlpha(false);
            comp->set_blendEquation(GL_FUNC_REVERSE_SUBTRACT);
            comp->set_sfactor(GL_SRC_ALPHA);
            comp->set_dfactor(GL_ONE);
            break;
        case darken:
            comp->set_preMultiAlpha(false);
            comp->set_blendEquation(GL_FUNC_ADD);
            comp->set_sfactor(GL_DST_COLOR);
            comp->set_dfactor(GL_ZERO);
            break;
        case lighten:
            comp->set_preMultiAlpha(false);
            comp->set_blendEquation(GL_FUNC_ADD);
            comp->set_sfactor(GL_SRC_ALPHA);
            comp->set_dfactor(GL_ONE);
            break;
        case screen:
//            comp->set_blendEquation(GL_FUNC_ADD);
//            comp->set_sfactor(GL_SRC_ALPHA);
//            comp->set_dfactor(GL_ONE);
//            break;
            comp->set_preMultiAlpha(true);
            comp->set_blendEquation(GL_FUNC_ADD);
            comp->set_sfactor(GL_ONE);
            comp->set_dfactor(GL_ONE_MINUS_SRC_COLOR);
            break;
        default:
            comp->set_preMultiAlpha(false);
            comp->set_blendEquation(GL_FUNC_ADD);
            comp->set_sfactor(GL_SRC_ALPHA);
            comp->set_dfactor(GL_ONE_MINUS_SRC_ALPHA);
            break;
    }
}

void HLBone::update(float dt)
{
    mTween.update();
    
    if (mNode && hide)
    {
        mNode->setVisible(false);
    }
    else if (mNode)
    {
        mNode->setVisible(true);
        
        float transformX = x + mTween.node.x;
        float transformY = y + mTween.node.y;
        float transformSkewX = skewX + mTween.node.skewX;
        float transformSkewY = skewY + mTween.node.skewY;
        float transformScaleX = scaleX*mTween.node.scaleX;
        float transformScaleY = scaleY*mTween.node.scaleY;
        
        if (mParent)
        {
            float r = atan2f(transformY, transformX) + mParent->info.skewY;
            float len = sqrtf(transformX*transformX+transformY*transformY);
            transformX = len * cosf(r) + mParent->info.x;
            transformY = len * sinf(r) + mParent->info.y;
            transformSkewX += mParent->info.skewX;
            transformSkewY += mParent->info.skewY;
        }
        info.x = transformX;
        info.y = transformY;
        info.skewX = transformSkewX;
        info.skewY = transformSkewY;
        info.scaleX = transformScaleX;
        info.scaleY = transformScaleY;
        transformScaleX *= cosf(DEGREES_TO_RADIANS(transformSkewY)); // adjust scale to avoid side effects with skew
        transformScaleY *= cosf(DEGREES_TO_RADIANS(transformSkewX));
        
		HLTransform2DComponent* transComp = mNode->getComponent<HLTransform2DComponent>();
		transComp->set_position(HLPoint(transformX, transformY));
		transComp->set_skewX(transformSkewX);
		transComp->set_skewY(transformSkewY);
		transComp->set_scaleX(transformScaleX);
		transComp->set_scaleY(transformScaleY);
        
 /*       transComp->setProperty("position", HLPoint(transformX, transformY));
        transComp->setProperty("skewX", transformSkewX);
        transComp->setProperty("skewY", transformSkewY);
        transComp->setProperty("scaleX", transformScaleX);
        transComp->setProperty("scaleY", transformScaleY);*/
        
		HLColorAdvanceComponent* colorComp = mNode->getComponent<HLColorAdvanceComponent>();
        static color4F color;
        color.r = mTween.node.redPercent;
        color.g = mTween.node.greenPercent;
        color.b = mTween.node.bluePercent;
        color.a = mTween.node.alphaPercent;
        if (mColorAdvComp)
        {
            color4F color1 = mColorAdvComp->get_colorPercent();
            color.r *= color1.r;
            color.g *= color1.g;
            color.b *= color1.b;
            color.a *= color1.a;
        }
        else if (mColorComp)
        {
            color4B color1;
            color1 = mColorComp->get_color();
            color.r *= color1.r/255.f;
            color.g *= color1.g/255.f;
            color.b *= color1.b/255.f;
            color.a *= color1.a/255.f;
        }
		colorComp->set_colorPercent(color);
       // colorComp->setProperty("colorPercent", color);
        color.r = mTween.node.redAmount;
        color.g = mTween.node.greenAmount;
        color.b = mTween.node.blueAmount;
        color.a = mTween.node.alphaAmount;
        if (mColorAdvComp)
        {
            color4F color1 = mColorAdvComp->get_colorAmount();
            color.r += color1.r;
            color.g += color1.g;
            color.b += color1.b;
            color.a += color1.a;
        }
		colorComp->set_colorAmount(color);
       // colorComp->setProperty("colorAmount", color);
        mNode->getComponent<HLSpriteComponent>()->set_brightness(mTween.node.brightness);// setProperty("brightness", mTween.node.brightness);
    }
    
	std::vector<HLBone*>::iterator itr = mChildren.begin();
    for (; itr != mChildren.end(); ++itr)
    {
        (*itr)->update(dt);
    }
}
    
}

NS_HL_END
