//
//  HLBone.h
//  HoolaiEngine
//
//  Created by tyq on 2/22/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLBONE_H__
#define __HLBONE_H__

#include "HLTween.h"

NS_HL_BEGIN

class HLSkeletonComponent;
class HLEntity;
class HLColorComponent;
class HLColorAdvanceComponent;

namespace skeleton
{
    
class HLTween;

class HLBone
{
    friend class HLTween;
    friend class ::hoolai::HLSkeletonComponent;
    friend class HLBoneAnimation;
public:
    HLBone(HLSkeletonComponent* skeleton, skeleton_bone* boneData);
    
    void update(float dt);
    
    void setRotation(float rotation)
    {
        skewX = skewY = rotation;
    }
    
private:
    void setContainer(std::string);
    void setBlend(blendmode blend);
    void addEntity(HLEntity* child);
    
    HLSkeletonComponent* mSkeleton;
    HLColorComponent* mColorComp;
    HLColorAdvanceComponent* mColorAdvComp;
    float x;
    float y;
    float skewX;
    float skewY;
    float scaleX;
    float scaleY;
    int z;
    bool hide;
    
    struct{
        float x;
        float y;
        float skewX;
        float skewY;
        float scaleX;
        float scaleY;
    } info;
    skeleton_bone* mBoneData;
    HLEntity* mNode;
    HLTween mTween;
    HLBone* mParent;
    std::vector<HLBone*> mChildren;
};
    
}

NS_HL_END

#endif
