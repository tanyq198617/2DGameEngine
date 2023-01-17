//
//  HLTween.cpp
//  HoolaiEngine
//
//  Created by tyq on 2/22/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLTween.h"
#include "HLBone.h"
#include "HLBoneAnimation.h"
#include "HLSkeletonComponent.h"

NS_HL_BEGIN

namespace skeleton
{
    
HLTweenNode operator-(const HLTweenNode& from, const HLTweenNode& to)
{
    HLTweenNode res;
    res.x = to.x - from.x;
    res.y = to.y - from.y;
    res.scaleX = to.scaleX - from.scaleX;
    res.scaleY = to.scaleY - from.scaleY;
    res.skewX = to.skewX - from.skewX;
    res.skewY = to.skewY - from.skewY;
    res.alphaPercent = to.alphaPercent - from.alphaPercent;
    res.redPercent = to.redPercent - from.redPercent;
    res.greenPercent = to.greenPercent - from.greenPercent;
    res.bluePercent = to.bluePercent - from.bluePercent;
    res.alphaAmount = to.alphaAmount - from.alphaAmount;
    res.redAmount = to.redAmount - from.redAmount;
    res.greenAmount = to.greenAmount - from.greenAmount;
    res.blueAmount = to.blueAmount - from.blueAmount;
    res.brightness = to.brightness - from.brightness;
    if (to.tweenRotate)
    {
        res.skewX += to.tweenRotate * 360;
        res.skewY += to.tweenRotate * 360;
    }
    else
    {
//        res.skewX = fmodf(res.skewX, 360);
//        if (res.skewX > 180)
//        {
//            res.skewX -= 360;
//        }
//        if (res.skewX < -180)
//        {
//            res.skewX += 360;
//        }
//        res.skewY = fmodf(res.skewY, 360);
//        if (res.skewY > 180)
//        {
//            res.skewY -= 360;
//        }
//        if (res.skewY < -180)
//        {
//            res.skewY += 360;
//        }
    }
    return res;
}
    
void HLTween::play(void* animation, int duration, int durationTo, float scale, bool loop, float tweenEasing)
{
    mIsTweenKeyFrame = false;
    mAnimBoneData = (skeleton_animation_bone*)animation;
    
    mNumKeyFrames = mAnimBoneData->frames_size();
    
    if (mAnimBoneData->frames_size() == 0)
    {
        return;
    }
    
    if (durationTo)
    {
        mToIndex = -1;
        HLFrameData to(mAnimBoneData->mutable_frames(0));
        setBetween(node, to);
        mFrameTweenEasing = tweenEasing;
    }
    else
    {
//        if (mNumKeyFrames > 1)
//        {
//            HLFrameData from(mAnimBoneData->mutable_frames(0));
//            mCurrentKeyFrameDuration = from.frame->duration();
//            if (!from.frame->hide())
//            {
//                mFrameTweenEasing = from.frame->tweeneasing();
//                HLFrameData to(mAnimBoneData->mutable_frames(1));
//                setBetween(from, to);
//            }
//            mToIndex = 1;
//            mBone->hide = from.frame->hide();
//        }
//        else
//        {
//            HLFrameData from(mAnimBoneData->mutable_frames(0));
//            mCurrentKeyFrameDuration = from.frame->duration();
//            mFrameTweenEasing = from.frame->tweeneasing();
//            setBetween(from, from);
//            mToIndex = 0;
//            mBone->hide = from.frame->hide();
//        }
//        if (mAnimBoneData->mutable_frames(0)->has_container())
//        {
//            mBone->setContainer(mAnimBoneData->mutable_frames(0)->container());
//        }
//        if (mAnimBoneData->mutable_frames(0)->has_blend())
//        {
//            mBone->setBlend(mAnimBoneData->mutable_frames(0)->blend());
//        }
        if (mNumKeyFrames == 1)
        {
            HLFrameData from(mAnimBoneData->mutable_frames(0));
            mCurrentKeyFrameDuration = from.frame->duration();
            mFrameTweenEasing = from.frame->tweeneasing();
            setBetween(from, from);
            tweenNodeTo(0);
            mToIndex = 0;
            mBone->hide = from.frame->hide();
            
            if (mAnimBoneData->mutable_frames(0)->has_container())
            {
                mBone->setContainer(mAnimBoneData->mutable_frames(0)->container());
            }
            if (mAnimBoneData->mutable_frames(0)->has_blend())
            {
                mBone->setBlend(mAnimBoneData->mutable_frames(0)->blend());
            }
        }
        mToIndex = -1;
        mBone->update(0);
    }
}

void HLTween::updateFrameData()
{
	if (mAnimation->mDurationTo)
	{
		tweenNodeTo(mAnimation->mCurrentPercent);
		return;
	}
    if (mAnimBoneData->frames_size() == 0)
    {
        return;
    }
    static HLFrameData from;
    static HLFrameData to;
    
    bool keyframeChanged = false;
    
	if (mToIndex == -1)
	{
		if (mNumKeyFrames > 1)
		{
            from.setFrameData(mAnimBoneData->mutable_frames(0));
			mCurrentKeyFrameDuration = from.frame->duration();
			mFrameTweenEasing = from.frame->tweeneasing();
            to.setFrameData(mAnimBoneData->mutable_frames(1));
			setBetween(from, to);
			mToIndex = 1;
		}
		else
		{
            from.setFrameData(mAnimBoneData->mutable_frames(0));
			mCurrentKeyFrameDuration = from.frame->duration();
			mFrameTweenEasing = from.frame->tweeneasing();
			setBetween(from, from);
			mToIndex = 0;
		}
        keyframeChanged = true;
	}

	int fromIndex = mToIndex==0?mNumKeyFrames-1:mToIndex - 1;
	while (mCurrentKeyFrameDuration <= mAnimation->mCurrentFrame)
	{
		if (!mAnimation->mLoop && mToIndex==0)
		{
			break;
		}
		keyframeChanged = true;
		mCurrentKeyFrameDuration += mAnimBoneData->mutable_frames(mToIndex)->duration();
		fromIndex = mToIndex;
		mToIndex++;
		if (mToIndex == mNumKeyFrames)
		{
			mToIndex = 0;
			if (!mAnimation->mLoop)
			{
				break;
			}
		}
	}
	if (keyframeChanged)
	{
        skeleton_animation_bone_frame* frame = mAnimBoneData->mutable_frames(fromIndex);
        if (frame->has_container()) // update texture display
        {
            mBone->setContainer(frame->container());
        }
        if (frame->has_blend())
        {
            mBone->setBlend(frame->blend());
        }
        mBone->hide = frame->hide();
        
        from.setFrameData(mAnimBoneData->mutable_frames(fromIndex));
		if (from.frame->hide())
		{
			return;
		}
		mFrameTweenEasing = from.frame->tweeneasing();
		if (fromIndex == mNumKeyFrames-1 && !mAnimation->mLoop)
		{
            to.setFrameData(mAnimBoneData->mutable_frames(fromIndex));
			setBetween(from, to);
		}
		else
		{
            to.setFrameData(mAnimBoneData->mutable_frames(mToIndex));
			setBetween(from, to);
		}
	}

	if (isnan(mFrameTweenEasing))
	{
		tweenNodeTo(0);
		return;
	}

	float percent = (mAnimation->mCurrentFrame - (mCurrentKeyFrameDuration-mAnimBoneData->mutable_frames(fromIndex)->duration()))/(float)mAnimBoneData->mutable_frames(fromIndex)->duration();
	percent = MIN(percent, 1);
    percent = MAX(percent, 0);
	if (mFrameTweenEasing)
	{
		percent = getEaseValue(percent, mFrameTweenEasing);
	}
	tweenNodeTo(percent);
}

void HLTween::update()
{
    if (mAnimation->mLoopReset)
    {
        mToIndex = -1;
    }
    if (mNumKeyFrames > 1 || mAnimation->mDurationTo)
    {
        updateFrameData();
    }
}
    
}

NS_HL_END
