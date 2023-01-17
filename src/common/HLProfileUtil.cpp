//
//  HLProfileUtil.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-3-26.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLProfileUtil.h"
#include <stdio.h>
#if defined(TARGET_MACOS) || defined(TARGET_IOS)
#include "mach/mach.h" 
#endif

NS_HL_BEGIN

HLTimeProfiler::HLTimeProfiler()
{
    gettimeofday(&mLastTime, 0);
}

void HLTimeProfiler::reset()
{
    gettimeofday(&mLastTime, 0);
}

double HLTimeProfiler::elapsed()
{
    struct timeval now;
    gettimeofday(&now, 0);
    return (now.tv_sec - mLastTime.tv_sec) + (now.tv_usec - mLastTime.tv_usec) / 1000000.0;
}

#if defined(TARGET_MACOS) || defined(TARGET_IOS)
vm_size_t usedMemory(void)
{
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &size);
    return (kerr == KERN_SUCCESS) ? info.resident_size : 0; // size in bytes
}

vm_size_t freeMemory(void)
{
    mach_port_t host_port = mach_host_self();
    mach_msg_type_number_t host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    vm_size_t pagesize;
    vm_statistics_data_t vm_stat;
    
    host_page_size(host_port, &pagesize);
    (void) host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size);
    return vm_stat.free_count * pagesize;
}

void logMemUsage(void)
{
#ifdef DEBUG
    // compute memory usage and log if different by >= 100k
    static long prevMemUsage = 0;
    long curMemUsage = usedMemory();
    long memUsageDiff = curMemUsage - prevMemUsage;
    
    if (memUsageDiff > 100000 || memUsageDiff < -100000) {
        prevMemUsage = curMemUsage;
//        HLLOG("Memory used %7.1f (%+5.0f), free %7.1f kb", curMemUsage/1000.0f, memUsageDiff/1000.0f, freeMemory()/1000.0f);
    }
#endif
}
#else
void logMemUsage(void)
{
}
#endif

NS_HL_END
