//
//  HLNotificationCenter.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-18.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#include "HLNotificationCenter.h"
#include "HLAutoreleasePool.h"

#if defined(TARGET_ANDROID)
#include "Java_com_hoolai_engine_HLNotification.h"
#endif

using namespace std;

NS_HL_BEGIN

void HLNotificationCenter::addObserver(std::string notifyname, HLNotificationInterface *observer)
{
    OpenThreads::ScopedWriteLock lock(_mutex);
    pair<multimap<std::string, HLNotificationInterface*>::iterator, multimap<std::string, HLNotificationInterface*>::iterator> pir = _observers.equal_range(notifyname);
    multimap<std::string, HLNotificationInterface*>::iterator itr = pir.first;
    for (; itr != pir.second; itr++) 
    {
        if (itr->second == observer) 
        {
            return;
        }
    }
    _observers.insert(make_pair(notifyname, observer));
    
}

void HLNotificationCenter::removeObserver(std::string notifyname, HLNotificationInterface * observer)
{
    OpenThreads::ScopedWriteLock lock(_mutex);
    pair<multimap<std::string, HLNotificationInterface*>::iterator, multimap<std::string, HLNotificationInterface*>::iterator> pir = _observers.equal_range(notifyname);
    multimap<std::string, HLNotificationInterface*>::iterator itr = pir.first;
    for (; itr != pir.second; itr++) 
    {
        if (itr->second == observer) 
        {
//            _observers.erase(itr);
            itr->second = NULL;
            return;
        }
    }
}

void HLNotificationCenter::removeObserver(HLNotificationInterface * observer)
{
    OpenThreads::ScopedWriteLock lock(_mutex);
//    multimap<std::string, HLNotificationInterface*> tmp = _observers;
//    _observers.clear();
    multimap<std::string, HLNotificationInterface*>::iterator itr = _observers.begin();
    for (; itr != _observers.end(); ) 
    {
        if (itr->second == observer)
        {
            _observers.erase(itr++);
        }
        else
        {
            ++itr;
        }
    }
}

void HLNotificationCenter::postNotification(std::string notifyname, HLObject* message)
{
    HLAutoreleasePool pool;
    HLNotification *notify = new HLNotification(notifyname, message);
    notify->autorelease();
    performFuncOnMainThread((CallFuncO)(&HLNotificationCenter::postOnMainThread), notify);
}

void HLNotificationCenter::postOnMainThread(HLObject *message)
{
	HLNotification * msg = static_cast<HLNotification*>(message);
    _mutex.readLock();
    std::multimap<std::string, HLNotificationInterface*> copy = _observers;
    _mutex.readUnlock();
    pair<multimap<std::string, HLNotificationInterface*>::iterator, multimap<std::string, HLNotificationInterface*>::iterator> pir = copy.equal_range(msg->name);
    multimap<std::string, HLNotificationInterface*>::iterator itr = pir.first;
    for (; itr != pir.second; ) 
    {
        if (itr->second)
        {
            itr->second->onNotification(msg);
        }
        ++itr;
    }
    
#if defined(TARGET_ANDROID)
    postNotificationToJava(msg);
#endif
}

void HLNotificationCenter::postNotificationIgnoreThread(std::string notifyname, HLObject* message)
{
    HLNotification *notify = new HLNotification(notifyname, message);
    
    std::multimap<std::string, HLNotificationInterface*> tmp = _observers;
    pair<multimap<std::string, HLNotificationInterface*>::iterator, multimap<std::string, HLNotificationInterface*>::iterator> pir = tmp.equal_range(notify->name);
    multimap<std::string, HLNotificationInterface*>::iterator itr = pir.first;
    for (; itr != pir.second; itr++) 
    {
        itr->second->onNotification(notify);
    }
    
    notify->release();
}

NS_HL_END
