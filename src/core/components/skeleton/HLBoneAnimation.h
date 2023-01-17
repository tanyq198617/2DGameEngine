//
//  HLBoneAnimation.h
//  HoolaiEngine
//
//  Created by tyq on 2/22/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLBONEANIMATION_H__
#define __HLBONEANIMATION_H__

#include "HLMacros.h"
#include <math.h>
#include "skeleton.pb.h"

NS_HL_BEGIN

class HLSkeletonComponent;

namespace skeleton
{

class HLBoneAnimation
{
    friend class HLTween;
public:
    HLBoneAnimation(HLSkeletonComponent *skeleton):mSkeleton(skeleton),mScale(1),mIsComplete(true),mIsPause(false),mCurrentFrame(0),mFramesPerSecond(12),mSecondsPerFrame(0.083f),mDelta(0),mDuration(0) {}
    
    void play(void* animation, int duration, int durationTo = 0, float scale = 1, bool loop = false, float tweenEasing = NAN);
    
    void update(float dt);
    
    void setScale(float scale)
    {
        mScale = scale;
    }
    float getScale(float scale)
    {
        return mScale;
    }
    
    void setFramesPerSecond(int fps)
    {
        mFramesPerSecond = fps;
    }
    
    int getFramesPerSecond()
    {
        return mFramesPerSecond;
    }
    
    void pause()
    {
        mIsPause = true;
    }
    
    void resume()
    {
        mIsPause = false;
    }
    
    void stop()
    {
        mIsComplete = true;
        mCurrentFrame = 0;
    }
    
    const char* getCurrentActionName()
    {
        if (mAnimation && !mIsComplete)
        {
            return mAnimation->name().c_str();
        }
        return NULL;
    }
private:
    HLSkeletonComponent* mSkeleton;
    skeleton_animation* mAnimation;
    
    bool mIsPause;
    bool mIsComplete;
    float mCurrentFrame;
    int mTotalFrames;
    float mCurrentPercent;
    
    int mDurationTo;
    bool mLoop;
    float mScale;
    int mFramesPerSecond;
    float mSecondsPerFrame;
    float mDelta;
    float mDuration;
    
    bool mLoopReset;
    
};
    
}

NS_HL_END

#endif
