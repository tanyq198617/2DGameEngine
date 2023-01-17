//
//  HLTouchDispatcher.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-6-13.
//  Copyright (c) 2012 hoolai. All rights reserved.
//

#ifndef __HLTOUCHDISPATCHER_H__
#define __HLTOUCHDISPATCHER_H__

#include "HLMacros.h"
#include <vector>
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
class HLTouchDispatcher
{
    friend class HLEntity;
private:
    HLEntity* mCurrEntity;
    gui::HLView* mCurrView;
    static HLTouchDispatcher dispatcher;
    HLTouchDispatcher():mCurrEntity(NULL),mNeedsHitTest(true),mClickCount(0),mEventDispatching(-1) {};
    void dispatchTouchEventToEntity(HLEntity*, HLTouchEvent*);
    bool mNeedsHitTest;
    uint32_t mClickCount;
    HLPoint lastPoint;
    int64_t mEventDispatching;
    std::map<int, std::pair<int, struct timeval> > mTouchEvents;
    typedef struct {
        int tapCount;
        HLPoint location;
        struct timeval timestamp;
    }CompleteTouchInfo;
    std::list<CompleteTouchInfo> mCompleteTouchInfos;
    int64_t mId;
public:
    static HLTouchDispatcher* sharedTouchDispatcher()
    {
        return &dispatcher;
    }
    
    void setMultiTouchEnabled(bool enabled);
    
    CMultiDelegate1<HLTouchEvent*> touchEventHandler;
    
    void entityWillDestroy(HLEntity* entity);
    void viewWillDestroy(gui::HLView* view);
    void needsHitTest() {mNeedsHitTest = true;}
    
    void hitTest(const HLPoint&);
    void touchesBegin(std::vector<HLTouch>&);
    void touchesMove(std::vector<HLTouch>&);
    void touchesEnd(std::vector<HLTouch>&);
    void touchesCancel(std::vector<HLTouch>&);
    
    void cancelCurrentEventHandler() {mCurrView = NULL; mCurrEntity = NULL; mNeedsHitTest = true;}
};

NS_HL_END

#endif
