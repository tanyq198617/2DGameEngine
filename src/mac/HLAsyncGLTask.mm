//
//  HLAsyncGLTask.mm
//  HoolaiEngine
//
//  Created by tyq on 13-7-24.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLAsyncGLTask.h"
#include "HLAutoreleasePool.h"

NS_HL_BEGIN

dispatch_queue_t bgasyncgltaskqueue;
bool bgasyncgltaskqueue_initialized = false;

class HLAsyncGLTaskMainThreadCaller: public HLObject
{
private:
    HLAsyncGLTask* task;
public:
    HLAsyncGLTaskMainThreadCaller(HLAsyncGLTask* obj)
    {
        task = obj;
    }
    
    void mainThreadCallBack(HLObject*)
    {
        task->onTaskComplete(task);
        delete task;
        this->release();
    }
};

void HLAsyncGLTask::start()
{
    __block NSOpenGLContext* currContext = [NSOpenGLContext currentContext];
//    __block dispatch_queue_t currQueue = dispatch_get_current_queue();
    if (!bgasyncgltaskqueue_initialized)
    {
        bgasyncgltaskqueue_initialized = true;
        bgasyncgltaskqueue = dispatch_queue_create("bgasyncgltask", DISPATCH_QUEUE_PRIORITY_DEFAULT);
    }
    dispatch_async(bgasyncgltaskqueue, ^(){
        HLAutoreleasePool pool;
        NSOpenGLPixelFormatAttribute attrs[] =
        {
//            NSOpenGLPFAOffScreen,
//            NSOpenGLPFAWindow,
//            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFAStencilSize, 8,
            0
        };
        
        NSOpenGLPixelFormat* pixFmt = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
        NSOpenGLContext* context = [[NSOpenGLContext alloc] initWithFormat:pixFmt shareContext:currContext];
//        NSOpenGLView* view = [[NSOpenGLView alloc] initWithFrame:NSMakeRect(0,0,10,10)];
//        [view setOpenGLContext:context];
//        [context setView:view];
        [context makeCurrentContext];
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        CHECK_GL_ERROR();
        asyncTask(this);
        
        glFinish();
        
//        dispatch_sync(currQueue, ^(){
//            onTaskComplete(this);
//        });
        
        pool.purgePool();
        
        [NSOpenGLContext clearCurrentContext];
        context = nil;
//        delete this;
        
        HLAsyncGLTaskMainThreadCaller* threadCaller = new HLAsyncGLTaskMainThreadCaller(this);
        threadCaller->performFuncOnMainThread((CallFuncO)&HLAsyncGLTaskMainThreadCaller::mainThreadCallBack, NULL);
    });
}

NS_HL_END
