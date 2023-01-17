//
//  HLReachability.cpp
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-8-9.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

// TODO for android, win and other platform
#include "HLReachability.h"
#if defined(TARGET_ANDROID)
#include "jni/Java_com_hoolai_engine_HLHelper.h"
#endif

NS_HL_BEGIN

HLReachability* HLReachability::reachabilityWithHostName(const char* hostname)
{
    HLReachability* retVal = NULL;
    retVal = new HLReachability();
    retVal->autorelease();
    return retVal;
}

HLReachability* HLReachability::reachabilityWithAddress(const struct sockaddr_in* hostaddr)
{
    HLReachability* retVal = NULL;
    retVal = new HLReachability();
    retVal->autorelease();
    return retVal;
}

HLReachability* HLReachability::reachabilityForInternetConnection()
{
    HLReachability* retVal = NULL;
    retVal = new HLReachability();
    retVal->autorelease();
    return retVal;
}

HLReachability* HLReachability::reachabilityForLocalWifi()
{
    HLReachability* retVal = NULL;
    retVal = new HLReachability();
    retVal->autorelease();
    return retVal;
}

void HLReachability::startNotifier()
{
    
}

void HLReachability::stopNotifier()
{
    
}

NetworkStatus HLReachability::getCurrentReachabilityStatus()
{
#if defined(TARGET_ANDROID)
    int states = getNetWorkTypeFormJNI();
    
    if (states ==0)
    {
        return NotReachable;  
    }
    else if(states == 1)
    {
        return ReachableViaWiFi;
    }
    else
    {
        return ReachableViaWWAN;
    }
#elif defined(TARGET_WIN32)
	return ReachableViaWiFi;
#endif
}

HLReachability::~HLReachability()
{
    
}

NS_HL_END