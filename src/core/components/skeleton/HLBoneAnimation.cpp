//
//  HLBoneAnimation.cpp
//  HoolaiEngine
//
//  Created by tyq on 2/22/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLBoneAnimation.h"
#include "HLBone.h"
#include "HLSkeletonComponent.h"

NS_HL_BEGIN

namespace skeleton
{

void HLBoneAnimation::play(void* animation, int duration, int durationTo, float scale, bool loop, float tweenEasing)
{
    mIsComplete = false;
    mIsPause = false;
    mCurrentFrame = 0;
    mTotalFrames = duration;
    mScale = scale;
    
    mDurationTo = durationTo;
    
    mLoop = loop;
    
    mAnimation = (skeleton_animation*)animation;
    
    int count = mAnimation->bones_size();
    for (int i=0; i<count; i++)
    {
        std::map<std::string, HLBone*>::iterator res = mSkeleton->mBones.find(mAnimation->mutable_bones(i)->name());
        if (res != mSkeleton->mBones.end())
        {
            res->second->mTween.play(mAnimation->mutable_bones(i), duration, durationTo, scale, loop, tweenEasing);
        }
    }
}

void HLBoneAnimation::update(float dt)
{
    if (!mAnimation)
    {
        return;
    }
    
    if (mIsComplete || mIsPause)
    {
        return;
    }
    mDelta += dt;
    mDuration += dt;
    if (mDuration < mSecondsPerFrame/mScale)
    {
        return;
    }
    mDuration -= mSecondsPerFrame/mScale;
    
    mLoopReset = false;
    if (mTotalFrames <= 0)
    {
        mCurrentFrame = mTotalFrames = 1;
    }
    mCurrentFrame += mScale * mDelta * mFramesPerSecond;
    if (mDurationTo > 0) // tween between actions
    {
        mCurrentPercent = mCurrentFrame / mDurationTo;
        if (mCurrentPercent >= 1)
        {
            mCurrentFrame -= mDurationTo;
            mDurationTo = 0;
            mCurrentPercent = mCurrentFrame/mTotalFrames;
        }
    }
    else
    {
        mCurrentPercent = mCurrentFrame / mTotalFrames;
    }
    if (mCurrentPercent >= 1)
    {
        if (!mLoop)
        {
            mCurrentPercent = 1;
            mIsComplete = true;
            mCurrentFrame = mTotalFrames - 1;
        }
        else
        {
            mCurrentFrame = fmodf(mCurrentFrame, mTotalFrames);
            mCurrentPercent = mCurrentFrame / mTotalFrames;
            mLoopReset = true;
        }
    }
    
//    if (!mIsComplete) // 卡的情况下动画可能停到前面几帧而不是最后一帧
//    {       
        std::vector<HLBone*>::iterator itr;
        for (itr = mSkeleton->mBoneChildren.begin(); itr != mSkeleton->mBoneChildren.end(); ++itr)
        {
            (*itr)->update(mDelta);
        }
//    }
    
    if (mCurrentPercent==1)
    {
        if (mLoop)
        {
            mSkeleton->onAnimationEvent(mSkeleton->mEntity, mAnimation->name().c_str(), "loopComplete");
        }
        else
        {
            mSkeleton->onAnimationEvent(mSkeleton->mEntity, mAnimation->name().c_str(), "complete");
        }
    }
    
    mDelta = 0;
    
}
    
}

NS_HL_END
