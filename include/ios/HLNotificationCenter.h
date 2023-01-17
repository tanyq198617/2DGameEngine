//
//  HLNotificationCenter.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-5-18.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLNOTIFICATIONCENTER_H__
#define __HLNOTIFICATIONCENTER_H__

#include "HLObject.h"
#include <OpenThreads/ReadWriteMutex>

NS_HL_BEGIN

class HLNotification:public HLObject
{
public:
    std::string name;
    HLObject* userInfo;
    HLNotification(std::string name, HLObject *userInfo)
    {
        if (userInfo) 
        {
            userInfo->retain();
        }
        
        this->name = name;
        this->userInfo = userInfo;
    }
    
    ~HLNotification()
    {
        if (userInfo) 
        {
            userInfo->release();
        }
        
    }
};

class HLNotificationInterface 
{
public:
	virtual void onNotification(HLNotification* message) = 0;
};

class HLNotificationCenter: public HLObject
{
    friend class JSNotificationCenter;
private:
    std::multimap<std::string, HLNotificationInterface*> _observers;
    mutable OpenThreads::ReadWriteMutex _mutex;
public:
    
    static HLNotificationCenter *defaultCenter()
    {
        static HLNotificationCenter* instance = NULL;
        if (!instance) {
            instance = new HLNotificationCenter();
        }
        return instance;
    }
    
    static HLNotificationCenter *internalCenter()
    {
        static HLNotificationCenter* instance = NULL;
        if (!instance) {
            instance = new HLNotificationCenter();
        }
        return instance;
    }
    
    void addObserver(std::string notifyname, HLNotificationInterface *observer);
    
    void removeObserver(std::string notifyname, HLNotificationInterface * observer);
    
    void removeObserver(HLNotificationInterface * observer);
    
    void postNotification(std::string notifyname, HLObject* message);
    
    void postOnMainThread(HLObject *message);
    
    void postNotificationIgnoreThread(std::string notifyname, HLObject* message);
};

NS_HL_END

#endif
