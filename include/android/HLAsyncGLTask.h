//
//  HLAsyncGLTask.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-7-24.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLASYNCGLTASK_H__
#define __HLASYNCGLTASK_H__

#include "HLDelegate.h"

NS_HL_BEGIN

class HLAsyncGLTask
{
public:
    static uint32_t instance;
    
    HLAsyncGLTask() {++instance;}
    HLAsyncGLTask(IDelegate1<HLAsyncGLTask*>* task, IDelegate1<HLAsyncGLTask*>* callback)
    {
        ++instance;
        asyncTask = task;
        onTaskComplete = callback;
    }
    ~HLAsyncGLTask() {--instance;}
    CDelegate1<HLAsyncGLTask*> asyncTask;
    CDelegate1<HLAsyncGLTask*> onTaskComplete;
    void start();
    
    void* userData;
};

NS_HL_END

#endif /* defined(__HoolaiEngine__HLAsyncGLTask__) */
