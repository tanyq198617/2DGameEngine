//
//  HLSpriteAnimateSystem.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-12.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLSPRITEANIMATESYSTEM_H__
#define __HLSPRITEANIMATESYSTEM_H__

#include "HLMacros.h"
#include <vector>
#include "HLObject.h"
#include "HLDelegate.h"

NS_HL_BEGIN

class HLEntity;
class HLArray;
class HLSpriteFrame;

class HLAnimation
{
public:
    float _elapsed;
    HLEntity* _target;
    float _duration;
    CDelegate0 _callback;
public:
    HLAnimation(float duration):_duration(duration) {_elapsed = 0;};
    HLAnimation():_duration(0),_elapsed(0) {};
    
    virtual ~HLAnimation() {};
    
    virtual void update(float progress) = 0;
    
    virtual bool step(float dt)
    {
        _elapsed += dt;
        update(MIN(1, _elapsed / _duration));
        if (_elapsed > _duration) {
            _callback();
            return false;
        }
        return true;
    }
    
    virtual void run(HLEntity* target);
};

class HLSpriteFrameAnimation:public HLAnimation
{
private:
    HLArray* _spriteFrames;
    bool _restoreTexture;
    HLSpriteFrame* _originSpriteFrame;
public:
    HLSpriteFrameAnimation(float duration, HLArray *spriteFrames, bool restore = false);
    static HLSpriteFrameAnimation* animation(float duration, HLArray *spriteFrames, bool restore = false)
    {
        HLSpriteFrameAnimation* anim = new HLSpriteFrameAnimation(duration, spriteFrames, restore);
        return anim;
    }
    virtual ~HLSpriteFrameAnimation();
    virtual void update(float progress);
    virtual void run(HLEntity* target);
};

class HLRepeateAnimation:public HLAnimation
{
private:
    HLAnimation* _innerAnimation;
    int _repeatCount;
    int _counter;
public:
    HLRepeateAnimation(HLAnimation* animation, int repeatCount)
    {
        _repeatCount = repeatCount;
        _counter = 0;
        _innerAnimation = animation;
        _duration = animation->_duration;
    }
    
    static HLRepeateAnimation* animation(HLAnimation* animation, int repeatCount) 
    {
        HLRepeateAnimation* anim = new HLRepeateAnimation(animation, repeatCount);
        return anim;
    }
    
    ~HLRepeateAnimation();
    
    virtual void update(float progress)
    {
        _innerAnimation->update(progress);
    }
    
    virtual bool step(float dt)
    {
        _elapsed += dt;
        update(MIN(1, _elapsed / _duration));
        if (_elapsed > _duration) {
            if (_repeatCount > 0) 
            {
                _counter++;
                if (_counter == _repeatCount)
                {
                    _callback();
                    return false;
                }
            }
            _elapsed = 0;
        }
        return true;
    }
    
    virtual void run(HLEntity* target);
};

class HLAnimateSystem
{
private:
    static std::vector<HLAnimation*> _animations;
public:
    static void addAnimation(HLAnimation* anim);
    static void stopAnimationsWithTarget(HLEntity* target);
    static void stopAllAnimations();
    static void update(float dt);
};
    
NS_HL_END

#endif
