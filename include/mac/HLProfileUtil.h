//
//  HLProfileUtil.h
//  HoolaiEngine
//
//  Created by tyq on 13-3-26.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLPROFILEUTIL_H__
#define __HLPROFILEUTIL_H__

#include "HLMacros.h"
#if defined(TARGET_WIN32)
#include "hltime.h"
#else
#include <sys/time.h>
#endif

NS_HL_BEGIN

void logMemUsage();

class HLTimeProfiler
{
public:
    HLTimeProfiler();
    void reset();
    double elapsed();
private:
    struct timeval mLastTime;
};

NS_HL_END

#endif
