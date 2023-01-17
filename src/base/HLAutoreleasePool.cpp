//
//  HLAutoreleasePool.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-4.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#include "HLAutoreleasePool.h"

#include <map>
#include <algorithm>
#include <pthread.h>
#include <OpenThreads/Mutex>

#include "GameEngineConfig.h"

bool appWillTerminate = false;

#ifdef TARGET_WIN32
bool operator<(const pthread_t& first, const pthread_t& second)
{
	return first.p < second.p;
    
}
#endif

NS_HL_BEGIN

std::map<pthread_t, std::list<HLAutoreleasePool *> > poolManager;

HLAutoreleasePool::HLAutoreleasePool() 
{
    pthread_t thread = pthread_self();
    std::map<pthread_t, std::list<HLAutoreleasePool *> >::iterator itr = poolManager.find(thread);
    if (itr != poolManager.end())
    {
        itr->second.push_back(this);
    }
    else
    {
        std::list<HLAutoreleasePool *> l;
        l.push_back(this);
        static OpenThreads::Mutex mutex; // for thread safe
        mutex.lock();
        poolManager.insert(std::make_pair(thread, l));
        mutex.unlock();
    }
}

HLAutoreleasePool::~HLAutoreleasePool()
{
    purgePool();
    if (appWillTerminate)
        return;
    pthread_t thread = pthread_self();
    std::map<pthread_t, std::list<HLAutoreleasePool *> >::iterator itr = poolManager.find(thread);
    HLASSERT(itr != poolManager.end(), "autoreleasepool may dealloc in wrong thread");
    std::list<HLAutoreleasePool *>& l = itr->second;
    
    std::list<HLAutoreleasePool *>::iterator itr1 = std::find(l.begin(), l.end(), this);
    HLASSERT(itr1 != l.end(), "autoreleasepool may dealloc in wrong thread");
    l.erase(itr1);
}

HLAutoreleasePool * HLAutoreleasePool::currentPool()
{
    pthread_t thread = pthread_self();
    std::map<pthread_t, std::list<HLAutoreleasePool *> >::iterator itr = poolManager.find(thread);
    if (itr != poolManager.end()) {
        return (itr->second).back();
    }
    return NULL;
}

void HLAutoreleasePool::purgePool()
{
    std::list<HLObject *>::iterator itr;
    for (itr = managedObjects.begin(); itr != managedObjects.end(); itr++) {
        if (*itr) {
            (*itr)->release();
        }
    }
    managedObjects.clear();
}

void HLAutoreleasePool::addObject(HLObject * o) 
{
    managedObjects.push_back(o);
}

NS_HL_END
