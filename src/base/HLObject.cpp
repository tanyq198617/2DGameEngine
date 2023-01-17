//
//  HLObject.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-4.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLObject.h"
#include "HLAutoreleasePool.h"
#include <assert.h>
#include "HLFileUtil.h"
#include <OpenThreads/Mutex>
#include <pthread.h>
#include "HLResourceManager.h"
#include "HLUtil.h"

NS_HL_BEGIN

extern OpenThreads::Mutex scheduleSelectorMutex;
extern std::vector<ThreadScheduleObject> pendingScheduleArray;

void HLObject::performFuncOnMainThread(CallFuncO func, HLObject * param)
{
    if (currentIsMainThread())
	{
        (this->*func)(param);
        return;
    }
    scheduleSelectorMutex.lock();
    this->retain();
    
    if (param)
    {
        param->retain();
    }
    ThreadScheduleObject ob(this, func, param);
//    lock & add to pending array
    pendingScheduleArray.push_back(ob);
    scheduleSelectorMutex.unlock();
}

void HLObject::autorelease() 
{
    HLAutoreleasePool *pool = HLAutoreleasePool::currentPool();
    HLASSERT(pool, "autorelease with no pool");
    if (pool) {
        pool->addObject(this);
    } else {
        HLLOG("warning autorelease with no pool");
    }
}

HLString* HLString::stringWithContentsOfFile(const char* file)
{
//    if (file[0] == '/') 
//    {
//        HLFileData data(file, "rb");
//        return stringWithData(data.buffer, data.size);
//    }
//    char path[MAX_PATH];
//    get_full_path(file, path);
//    HLFileData data(path, "rb");
//    return stringWithData(data.buffer, data.size);
    HLFileData* data = HLResourceManager::getSingleton()->getFileData(file);
    if (!data)
    {
        HLLOG("file (%s) does not exist\n", file);
        return NULL;
    }
    HLString* res = stringWithData(data->buffer, data->size);
    delete data;
    return res;
}

NS_HL_END
