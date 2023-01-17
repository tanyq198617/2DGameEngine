//
//  HLReachability.h
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-8-9.
//  Copyright (c) 2013年 hoolai. All rights reserved.
//

#ifndef __HLREACHABILITY_H__
#define __HLREACHABILITY_H__

#include "HLObject.h"
#include "GameEngineConfig.h"
#if !defined(TARGET_WIN32)
#include <netinet/in.h>
#endif

NS_HL_BEGIN

enum NetworkStatus
{
    NotReachable = 0,
	ReachableViaWiFi,
	ReachableViaWWAN
};

#define kReachabilityChangedNotification "kNetworkReachabilityChangedNotification"

class HLReachability: public HLObject
{
public:
    static HLReachability* reachabilityWithHostName(const char* hostname);
    static HLReachability* reachabilityWithAddress(const struct sockaddr_in* hostaddr);
    static HLReachability* reachabilityForInternetConnection();
    static HLReachability* reachabilityForLocalWifi();
    
    void startNotifier();
    void stopNotifier();
    
    NetworkStatus getCurrentReachabilityStatus();
    
    virtual ~HLReachability();
private:
    
#if defined(TARGET_MACOS) || defined(TARGET_IOS)
    HLReachability():mReachabilityRef(NULL){}
    NetworkStatus localWiFiStatusForFlags(uint32_t flags);
    NetworkStatus networkStatusForFlags(uint32_t flags);
    bool mLocalWifiRef;
    void* mReachabilityRef;
#endif
};

NS_HL_END

#endif
