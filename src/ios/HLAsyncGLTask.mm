//
//  HLAsyncGLTask.mm
//  HoolaiEngine
//
//  Created by tyq on 13-7-24.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#import <OpenGLES/EAGL.h>
#include "HLAsyncGLTask.h"
#include "HLAutoreleasePool.h"

NS_HL_BEGIN

dispatch_queue_t bgasyncgltaskqueue;
bool bgasyncgltaskqueue_initialized = false;

void HLAsyncGLTask::start()
{
    __block EAGLContext* currentContext = [EAGLContext currentContext];
    if (!bgasyncgltaskqueue_initialized)
    {
        bgasyncgltaskqueue_initialized = true;
        bgasyncgltaskqueue = dispatch_queue_create("bgasyncgltask", DISPATCH_QUEUE_PRIORITY_DEFAULT);
    }
    dispatch_async(bgasyncgltaskqueue, ^(){
        HLAutoreleasePool pool;
        EAGLContext* context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:[currentContext sharegroup]];
        [EAGLContext setCurrentContext:context];
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        CHECK_GL_ERROR();
        asyncTask(this);
        
        glFinish();
        pool.purgePool();
        
        dispatch_sync(dispatch_get_main_queue(), ^(){
            onTaskComplete(this);
        });
        [EAGLContext setCurrentContext:nil];
        context = nil;
        delete this;
    });
}

NS_HL_END
