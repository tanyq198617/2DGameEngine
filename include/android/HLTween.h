//
//  HLTween.h
//  HoolaiEngine
//
//  Created by zhao shuan on 2/22/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLTWEEN_H__
#define __HLTWEEN_H__

#include "HLMacros.h"
#include "skeleton.pb.h"
#include <math.h>
#include <limits>

NS_HL_BEGIN

namespace skeleton
{
    
class HLBone;
class HLBoneAnimation;

class HLTweenNode
{
    friend HLTweenNode operator-(const HLTweenNode& from, const HLTweenNode& to);
public:
    float x;
    float y;
    float scaleX;
    float scaleY;
    float skewX;
    float skewY;
    float alphaPercent;
    float redPercent;
    float greenPercent;
    float bluePercent;
    float alphaAmount;
    float redAmount;
    float greenAmount;
    float blueAmount;
    float brightness;
    int tweenRotate;
    
    HLTweenNode(float x=0, float y=0, float scaleX=1, float scaleY=1, float skewX=0, float skewY=0, float alphaPercent=1, float redPercent=1, float greenPercent=1, float bluePercent=1, float alphaAmount=0, float redAmount=0, float greenAmount=0, float blueAmount=0):
    x(x),y(y),scaleX(scaleX),scaleY(scaleY),skewX(skewX),skewY(skewY),alphaPercent(alphaPercent), redPercent(redPercent), greenPercent(greenPercent), bluePercent(bluePercent), alphaAmount(alphaAmount), redAmount(redAmount), greenAmount(greenAmount), blueAmount(blueAmount), brightness(1)
    {tweenRotate = 0;}
};

class HLFrameData: public HLTweenNode
{
public:
    skeleton_animation_bone_frame* frame;
    HLFrameData():frame(NULL){}
    HLFrameData(::hoolai::skeleton::skeleton_animation_bone_frame* _frame)
    {
        setFrameData(_frame);
    }
    
    void setFrameData(skeleton_animation_bone_frame* _frame)
    {
        if (_frame)
        {
            frame = _frame;
            x = frame->x();
            y = frame->y();
            scaleX = frame->scalex();
            scaleY = frame->scaley();
            skewX = frame->skewx();
            skewY = frame->skewy();
            alphaPercent = frame->alphapercent();
            redPercent = frame->redpercent();
            greenPercent = frame->greenpercent();
            bluePercent = frame->bluepercent();
            alphaAmount = frame->alphaamount();
            redAmount = frame->redamount();
            greenAmount = frame->greenamount();
            blueAmount = frame->blueamount();
            brightness = frame->brightness();
            tweenRotate = 0;
        }
        else
        {
            frame = NULL;
        }
    }
};
    
class HLTween
{
public:
    HLTweenNode node;
private:
    HLBone* mBone;
    HLBoneAnimation* mAnimation;
    HLTweenNode mFrom;
    HLTweenNode mBetween;
    
    int mToIndex;
    
    int mCurrentKeyFrameDuration;
    int mNumKeyFrames;
    
    skeleton_animation_bone *mAnimBoneData;
    float mFrameTweenEasing;
    
    bool mIsTweenKeyFrame;
    
public:
    HLTween(HLBone* bone, HLBoneAnimation* animation)
    {
        mToIndex = 0;
        mBone = bone;
        mAnimation = animation;
    }
    
    void play(void* animation, int duration, int durationTo = 0, float scale = 1, bool loop = false, float tweenEasing = NAN);
    void update();
    
private:
    void setBetween(const HLTweenNode& from, const HLTweenNode& to)
    {
        mFrom = from;
        mBetween = from - to;
    }
    
    void tweenNodeTo(float value)
    {
        node.x = mFrom.x + value * mBetween.x;
        node.y = mFrom.y + value * mBetween.y;
        node.scaleX = mFrom.scaleX + value*mBetween.scaleX;
        node.scaleY = mFrom.scaleY + value*mBetween.scaleY;
        node.skewX = mFrom.skewX + value*mBetween.skewX;
        node.skewY = mFrom.skewY + value*mBetween.skewY;
        node.alphaPercent = mFrom.alphaPercent + value*mBetween.alphaPercent;
        node.redPercent = mFrom.redPercent + value*mBetween.redPercent;
        node.greenPercent = mFrom.greenPercent + value*mBetween.greenPercent;
        node.bluePercent = mFrom.bluePercent + value*mBetween.bluePercent;
        node.alphaAmount = mFrom.alphaAmount + value*mBetween.alphaAmount;
        node.redAmount = mFrom.redAmount + value*mBetween.redAmount;
        node.greenAmount = mFrom.greenAmount + value*mBetween.greenAmount;
        node.blueAmount = mFrom.blueAmount + value*mBetween.blueAmount;
        node.brightness = mFrom.brightness + value*mBetween.brightness;
    }
    
    void updateFrameData();
    
    // easing -1 0 1 2: easing in, linear, easing out, easing in-out
    float getEaseValue(float value, float easing)
    {
        if (easing > 1) // ease in out
        {
            easing -= 1;
            easing = 1/(1-easing);
            value *= 2;
            value = value<1?0.5f*powf(value, easing):1.0f-0.5f*powf(2-value, easing);
        }
        else if (easing < 0) // ease in
        {
            easing = 1/(1-easing);
            value = powf(value, easing);
        }
        else // ease out
        {
            easing = -easing;
            easing = 1/(1-easing);
            value = 1 - powf(1 - value, easing);
        }
        return value;
    }
};
    
}

NS_HL_END

#endif
