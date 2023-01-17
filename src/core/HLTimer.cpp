//
//  HLTimer.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLTimer.h"
#include <algorithm>
#if USE_JS_SCRIPTING
#include "JSTimer.h"
#endif

NS_HL_BEGIN

HLTimer::HLTimer(float interval, bool repeate, int priority):repeate(repeate), interval(interval), elapsed(0), delta(0), canceled(false), priority(priority)
{
    HLTimerManager::getSingleton()->mTimersToAdd.push_back(this);
}

HLTimer::~HLTimer()
{
    do
    {
        std::list<HLTimer*>& timers = HLTimerManager::getSingleton()->mTimers;
        std::list<HLTimer*>::iterator res = std::find(timers.begin(), timers.end(), this);
        if (res == timers.end())
        {
            std::list<HLTimer*>& timersToAdd = HLTimerManager::getSingleton()->mTimersToAdd;
            res = std::find(timersToAdd.begin(), timersToAdd.end(), this);
            if (res == timersToAdd.end())
            {
                break;
            }
        }
        *res = NULL;
    } while (0);
#if USE_JS_SCRIPTING
    JSTimer::removeJSObject(this);
#endif
}

void HLTimer::update(float dt)
{
    if (canceled)
    {
        delete this;
        return;
    }
    elapsed += dt;
    delta += dt;
    if (elapsed >= interval)
    {
        delegate(this, delta);
        if (repeate && !canceled)
        {
            delta = 0;
            if (interval)
            {
                do
                {
                    elapsed -= interval;
                } while (elapsed > interval);
            }
            else
            {
                elapsed = 0;
            }
        }
        else
        {
            delete this;
        }
    }
}

bool timerCompare(const HLTimer* timer1, const HLTimer* timer2)
{
    if (timer1 == NULL || timer2 == NULL)
    {
        return false;
    }
    return timer1->priority > timer2->priority;
}

void HLTimerManager::update(float dt)
{
    if (mTimersToAdd.begin() != mTimersToAdd.end())
    {
        mTimers.insert(mTimers.end(), mTimersToAdd.begin(), mTimersToAdd.end());
        mTimersToAdd.clear();
        mTimers.sort(timerCompare);
    }
    
    std::list<HLTimer*>::iterator itr = mTimers.begin();
    while (itr != mTimers.end())
    {
        if (*itr == NULL)
        {
            itr = mTimers.erase(itr);
        }
        else
        {
            (*itr)->update(dt);
            ++itr;
        }
    }
}

void HLTimerManager::cancelAllTimers()
{
    for (auto itr = mTimersToAdd.begin(); itr != mTimersToAdd.end(); ++itr)
    {
        delete *itr;
    }
    mTimersToAdd.clear();
    for (auto itr = mTimers.begin(); itr != mTimers.end(); ++itr)
    {
        (*itr)->cancel();
    }
}

NS_HL_END
