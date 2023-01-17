//
//  HLTimer.h
//  HoolaiEngine
//
//  Created by tyq on 1/20/13.
//  Copyright (c) 2013 hoolai. All rights reserved.
//

#ifndef __HLTIMER_H__
#define __HLTIMER_H__

#include "HLMacros.h"
#include "HLSingleton.h"
#include <list>
#include "HLDelegate.h"

NS_HL_BEGIN

class HLTimer;

typedef CDelegate2<HLTimer*,float> HLTimerDelegate;

class HLTimer
{
    friend bool timerCompare(const HLTimer* timer1, const HLTimer* timer2);
public:
    HLTimer(float interval, bool repeate = false, int priority = 0);
    ~HLTimer();
    
    HLTimerDelegate delegate;
    
    void update(float dt);
    
    void cancel() {canceled = true;}
private:
    bool canceled;
    bool repeate;
    float interval;
    float elapsed;
    float delta;
    int priority;
};

class HLTimerManager : public HLSingleton<HLTimerManager>
{
    friend class HLSingleton<HLTimerManager>;
    friend class HLTimer;
public:
    void update(float dt);
    void cancelAllTimers();
private:
    std::list<HLTimer*> mTimers;
    std::list<HLTimer*> mTimersToAdd;
};

NS_HL_END

#endif
