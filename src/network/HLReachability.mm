//
//  HLReachability.cpp
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-8-9.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLReachability.h"

#import <sys/socket.h>
#import <netinet6/in6.h>
#import <arpa/inet.h>
#import <ifaddrs.h>
#import <netdb.h>

#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <SystemConfiguration/SystemConfiguration.h>

#include "HLNotificationCenter.h"

NS_HL_BEGIN

static void PrintReachabilityFlags(SCNetworkReachabilityFlags    flags, const char* comment)
{
#if kShouldPrintReachabilityFlags
    
    NSLog(@"Reachability Flag Status: %c%c %c%c%c%c%c%c%c %s\n",
#ifdef TARGET_IOS
          (flags & kSCNetworkReachabilityFlagsIsWWAN)               ? 'W' : '-',
#else
          'X',
#endif
          (flags & kSCNetworkReachabilityFlagsReachable)            ? 'R' : '-',
          
          (flags & kSCNetworkReachabilityFlagsTransientConnection)  ? 't' : '-',
          (flags & kSCNetworkReachabilityFlagsConnectionRequired)   ? 'c' : '-',
          (flags & kSCNetworkReachabilityFlagsConnectionOnTraffic)  ? 'C' : '-',
          (flags & kSCNetworkReachabilityFlagsInterventionRequired) ? 'i' : '-',
          (flags & kSCNetworkReachabilityFlagsConnectionOnDemand)   ? 'D' : '-',
          (flags & kSCNetworkReachabilityFlagsIsLocalAddress)       ? 'l' : '-',
          (flags & kSCNetworkReachabilityFlagsIsDirect)             ? 'd' : '-',
          comment
          );
#endif
}

static void ReachabilityCallback(SCNetworkReachabilityRef target, SCNetworkReachabilityFlags flags, void* info)
{
#pragma unused (target, flags)
    NSCAssert(info != NULL, @"info was NULL in ReachabilityCallback");
    
    HLReachability* noteObject = (HLReachability*) info;
    // Post a notification to notify the client that the network reachability changed.
    HLNotificationCenter::defaultCenter()->postNotification(kReachabilityChangedNotification, noteObject);
}

HLReachability* HLReachability::reachabilityWithHostName(const char* hostname)
{
    HLReachability* retVal = NULL;
    SCNetworkReachabilityRef reachability = SCNetworkReachabilityCreateWithName(NULL, hostname);
    if (reachability)
    {
        retVal = new HLReachability();
        retVal->autorelease();
        retVal->mLocalWifiRef = false;
        retVal->mReachabilityRef = (void*)reachability;
    }
    return retVal;
}

HLReachability* HLReachability::reachabilityWithAddress(const struct sockaddr_in* hostaddr)
{
    HLReachability* retVal = NULL;
    SCNetworkReachabilityRef reachability = SCNetworkReachabilityCreateWithAddress(NULL, (const struct sockaddr*)hostaddr);
    if (reachability)
    {
        retVal = new HLReachability();
        retVal->autorelease();
        retVal->mLocalWifiRef = false;
        retVal->mReachabilityRef = (void*)reachability;
    }
    return retVal;
}

HLReachability* HLReachability::reachabilityForInternetConnection()
{
    struct sockaddr_in zeroAddress;
    bzero(&zeroAddress, sizeof(zeroAddress));
    zeroAddress.sin_len = sizeof(zeroAddress);
    zeroAddress.sin_family = AF_INET;
    return reachabilityWithAddress(&zeroAddress);
}

HLReachability* HLReachability::reachabilityForLocalWifi()
{
    struct sockaddr_in localWifiAddress;
    bzero(&localWifiAddress, sizeof(localWifiAddress));
    localWifiAddress.sin_len = sizeof(localWifiAddress);
    localWifiAddress.sin_family = AF_INET;
    // IN_LINKLOCALNETNUM is defined in <netinet/in.h> as 169.254.0.0
    localWifiAddress.sin_addr.s_addr = htonl(IN_LINKLOCALNETNUM);
    HLReachability* retVal = reachabilityWithAddress(&localWifiAddress);
    if(retVal!= NULL)
    {
        retVal->mLocalWifiRef = true;
    }
    return retVal;
}

void HLReachability::startNotifier()
{
    BOOL retVal = NO;
    SCNetworkReachabilityContext    context = {0, this, NULL, NULL, NULL};
    SCNetworkReachabilityRef reachabilityRef = static_cast<SCNetworkReachabilityRef>(mReachabilityRef);
    if(SCNetworkReachabilitySetCallback(reachabilityRef, ReachabilityCallback, &context))
    {
        if(SCNetworkReachabilityScheduleWithRunLoop(reachabilityRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode))
        {
            retVal = YES;
        }
    }
}

void HLReachability::stopNotifier()
{
    SCNetworkReachabilityRef reachabilityRef = static_cast<SCNetworkReachabilityRef>(mReachabilityRef);
    if(reachabilityRef!= NULL)
    {
        SCNetworkReachabilityUnscheduleFromRunLoop(reachabilityRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    }
}

NetworkStatus HLReachability::getCurrentReachabilityStatus()
{
    HLASSERT(mReachabilityRef != NULL, "currentNetworkStatus called with NULL reachabilityRef");
    NetworkStatus retVal = NotReachable;
    SCNetworkReachabilityFlags flags;
    if (SCNetworkReachabilityGetFlags(static_cast<SCNetworkReachabilityRef>(mReachabilityRef), &flags))
    {
        if(mLocalWifiRef)
        {
            retVal = localWiFiStatusForFlags(flags);
        }
        else
        {
            retVal = networkStatusForFlags(flags);
        }
    }
    return retVal;
}

NetworkStatus HLReachability::localWiFiStatusForFlags(uint32_t flags)
{
    PrintReachabilityFlags(flags, "localWiFiStatusForFlags");
    
    NetworkStatus retVal = NotReachable;
    if((flags & kSCNetworkReachabilityFlagsReachable) && (flags & kSCNetworkReachabilityFlagsIsDirect))
    {
        retVal = ReachableViaWiFi;
    }
    return retVal;
}

NetworkStatus HLReachability::networkStatusForFlags(uint32_t flags)
{
    PrintReachabilityFlags(flags, "networkStatusForFlags");
    if ((flags & kSCNetworkReachabilityFlagsReachable) == 0)
    {
        // if target host is not reachable
        return NotReachable;
    }
    
    NetworkStatus retVal = NotReachable;
    
    if ((flags & kSCNetworkReachabilityFlagsConnectionRequired) == 0)
    {
        // if target host is reachable and no connection is required
        //  then we'll assume (for now) that your on Wi-Fi
        retVal = ReachableViaWiFi;
    }
    
    
    if ((((flags & kSCNetworkReachabilityFlagsConnectionOnDemand ) != 0) ||
         (flags & kSCNetworkReachabilityFlagsConnectionOnTraffic) != 0))
    {
        // ... and the connection is on-demand (or on-traffic) if the
        //     calling application is using the CFSocketStream or higher APIs
        
        if ((flags & kSCNetworkReachabilityFlagsInterventionRequired) == 0)
        {
            // ... and no [user] intervention is needed
            retVal = ReachableViaWiFi;
        }
    }
    
#if defined(TARGET_IOS)
    if ((flags & kSCNetworkReachabilityFlagsIsWWAN) == kSCNetworkReachabilityFlagsIsWWAN)
    {
        // ... but WWAN connections are OK if the calling application
        //     is using the CFNetwork (CFSocketStream?) APIs.
        retVal = ReachableViaWWAN;
    }
#endif
    return retVal;
}

HLReachability::~HLReachability()
{
    stopNotifier();
    SCNetworkReachabilityRef reachabilityRef = static_cast<SCNetworkReachabilityRef>(mReachabilityRef);
    if (reachabilityRef != NULL)
    {
        CFRelease(reachabilityRef);
    }
}

NS_HL_END