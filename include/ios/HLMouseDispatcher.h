//
//  HLMouseDispatcher.h
//  HoolaiEngine
//
//  Created by zhao shuan on 12-9-12.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLMOUSEDISPATCHER_H__
#define __HLMOUSEDISPATCHER_H__

#include "HLMacros.h"
#include "HLGeometry.h"
#include "stdint.h"

#if defined(TARGET_WIN32)
#include "hltime.h"
#else
#include <sys/time.h>
#endif
#include "HLEventType.h"
#include "HLDelegate.h"

NS_HL_BEGIN

namespace gui
{
    class HLView;
}

class HLEntity;
class HLMouseDispatcher
{
    friend class HLEntity;
private:
    HLEntity* mCurrEntity;
    gui::HLView* mCurrView;
    static HLMouseDispatcher dispatcher;
    HLMouseDispatcher():mCurrEntity(0), mId(0), mNeedsHitTest(true),mClickCount(0) {timestamp.tv_sec = 0;};
    void dispathTouchEventToEntity(HLEntity*, HLTouchEvent*);
    void dispathMouseEventToEntity(HLEntity*, HLMouseEvent*);
    int mId;
    bool mNeedsHitTest;
    struct timeval timestamp;
    uint32_t mClickCount;
    HLPoint lastPoint;
public:
    static HLMouseDispatcher* sharedMouseDispatcher()
    {
        return &dispatcher;
    }
    
    CMultiDelegate1<HLMouseEvent*> mouseEventHandler;
    
    void entityWillDestroy(HLEntity* entity);
    void viewWillDestroy(gui::HLView* view);
    void needsHitTest() {mNeedsHitTest = true;}
    
    void hitTest(const HLPoint&);
    void onMouseEvent(HLMouseEvent&);
    
    void cancelCurrentEventHandler() {mCurrView = NULL; mCurrEntity = NULL; mNeedsHitTest = true;}
};

NS_HL_END

#endif
