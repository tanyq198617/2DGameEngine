//
//  HLSpriteAnimateSystem.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-12.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLAnimateSystem.h"
#include "HLSpriteFrame.h"
#if USE_JS_SCRIPTING
#include "JSAnimationSystem.h"
#endif
#include "HLTexture.h"
#include "HLEntitySystem.h"

NS_HL_BEGIN

std::vector<HLAnimation*> HLAnimateSystem::_animations;

void HLAnimation::run(HLEntity* target)
{
    _target = target;
    HLAnimateSystem::addAnimation(this);
}

void HLRepeateAnimation::run(HLEntity* target)
{
    _target = target;
    _innerAnimation->_target = target;
    HLAnimateSystem::addAnimation(this);
}

HLSpriteFrameAnimation::HLSpriteFrameAnimation(float duration, HLArray *spriteFrames, bool restore):HLAnimation(duration),_restoreTexture(restore)
{
    _spriteFrames = spriteFrames;
    _spriteFrames->retain();
    _originSpriteFrame = NULL;
}

HLSpriteFrameAnimation::~HLSpriteFrameAnimation()
{
    if (_originSpriteFrame)
    {
        _originSpriteFrame->setDisplayFrameForEntity(_target);
        _originSpriteFrame->release();
    }
    _spriteFrames->release();
#if USE_JS_SCRIPTING
    JSSpriteFrameAnimation::removeJSObject(this);
#endif
}

void HLSpriteFrameAnimation::run(HLEntity* target)
{
    if (_restoreTexture)
    {
        _originSpriteFrame = new HLSpriteFrame();
        _originSpriteFrame->_texture = target->getProperty<HLTexture*>("texture");
        if (_originSpriteFrame->_texture)
            _originSpriteFrame->_texture->retain();
        _originSpriteFrame->_textureRect = target->getProperty<HLRect>("textureRect");
        HLPoint offset = target->getProperty<HLPoint>("offsetPosFromBL");
        _originSpriteFrame->_offset.x = offset.x;
        _originSpriteFrame->_sourceSize = target->getProperty<HLSize>("untrimmedSize");
        _originSpriteFrame->_rotated = target->getProperty<bool>("textureRotated");
    }
    
    HLAnimation::run(target);
}

void HLSpriteFrameAnimation::update(float progress)
{
    int count = (int)_spriteFrames->_value.size();
    int idx = count * progress;
    if (idx >= count) 
    {
        idx = count - 1;
    }
    HLSpriteFrame* frame = (HLSpriteFrame*)_spriteFrames->objectAtIndex(idx);
    frame->setDisplayFrameForEntity(_target);
}

void HLAnimateSystem::update(float dt)
{
    std::vector<HLAnimation*>::iterator itr = _animations.begin();
    while (itr != _animations.end()) 
    {
        if (*itr)
        {
            if (!(*itr)->step(dt))
            {
                if (*itr)
                {
                    delete (*itr);
                    itr = _animations.erase(itr);
                }
            }
            else
                itr++;
        }
        else
        {
            itr = _animations.erase(itr);
        }
    }
}

HLRepeateAnimation::~HLRepeateAnimation()
{
    delete _innerAnimation;
#if USE_JS_SCRIPTING
    JSRepeateAnimation::removeJSObject(this);
#endif
}

void HLAnimateSystem::addAnimation(HLAnimation* anim)
{
    _animations.push_back(anim);
}

void HLAnimateSystem::stopAnimationsWithTarget(HLEntity* target)
{
    std::vector<HLAnimation*>::iterator itr = _animations.begin();
    while (itr != _animations.end())
    {
        if(*itr)
        {
            if ((*itr)->_target == target)
            {
                delete (*itr);
                *itr = NULL;
            }
        }
        ++itr;
    }
}

void HLAnimateSystem::stopAllAnimations()
{
    std::vector<HLAnimation*>::iterator itr = _animations.begin();
    while (itr != _animations.end())
    {
        delete (*itr);
        *itr = NULL;
    }
}

NS_HL_END
