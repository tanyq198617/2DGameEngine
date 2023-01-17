//
//  HLAsyncGLTask.mm
//  HoolaiEngine
//
//  Created by zhao shuan on 13-7-24.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLAsyncGLTask.h"
#include "HLAutoreleasePool.h"

NS_HL_BEGIN

// TODO call in another thread, and callback in gl thread
void HLAsyncGLTask::start()
{
//    HLAutoreleasePool pool;
    
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//    glDisable(GL_DEPTH_TEST);
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    HLLOG("HLAsyncGLTask1");
    
    asyncTask(this);
    
    HLLOG("HLAsyncGLTask2");
    
//    glFinish();
//    pool.purgePool();
    
    onTaskComplete(this);
    delete this;
    
    HLLOG("HLAsyncGLTask3");
}

NS_HL_END
