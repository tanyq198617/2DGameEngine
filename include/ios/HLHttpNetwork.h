//
//  HLHttpNetwork.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-2-27.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLHTTPNETWORK_H__
#define __HLHTTPNETWORK_H__

#include <list>
#include "HLHttpRequest.h"
#include "HLSingleton.h"
#include <pthread.h>

NS_HL_BEGIN

#if !(defined(TARGET_IOS) || defined(TARGET_MACOS))

class HLRequestQueue
{
public:
    HLRequestQueue();
    void insertRequest(HLHttpRequest* request);
	HLHttpRequest* removeRequest();
private:
    std::list<HLHttpRequest *> mList;
    pthread_mutex_t mutex;
};

#endif

class HLHttpNetwork:public HLSingleton<HLHttpNetwork>
{
public:
    HLHttpNetwork();
    ~HLHttpNetwork();
    void addRequest(HLHttpRequest* request);
#if !(defined(TARGET_IOS) || defined(TARGET_MACOS))
    HLHttpRequest* getRequest();
private:
    pthread_t workingtid;
	HLRequestQueue mQueue;
#endif
};

NS_HL_END

#endif
