//
//  HLMouseDispatcher.cpp
//  HoolaiEngine
//
//  Created by tyq on 12-9-12.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLMouseDispatcher.h"
#include "HLSceneManager.h"
#include "GameEngineConfig.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif
#include "HLGUIManager.h"
#include "HLView.h"
#include "HLViewDND.h"
#ifdef USE_TOUCH_EVENT
#include "HLTouchDispatcher.h"
#endif

NS_HL_BEGIN

namespace gui
{
    extern HLViewDragSource* dragsourceinstance;
}

HLMouseDispatcher HLMouseDispatcher::dispatcher;

void HLMouseDispatcher::entityWillDestroy(HLEntity* entity)
{
    if (mCurrEntity == entity)
    {
        mCurrEntity = NULL;
        needsHitTest();
    }
}

void HLMouseDispatcher::viewWillDestroy(gui::HLView* view)
{
    if (mCurrView)
    {
        if (mCurrView == view)
        {
            mCurrView = NULL;
            needsHitTest();
        }
        else
        {
            mCurrView->viewWillDestroy(view);
        }
    }
    if (gui::dragsourceinstance)
    {
        if (view == gui::dragsourceinstance->mCurrDropTarget)
        {
            gui::dragsourceinstance->mCurrDropTarget = NULL;
        }
        if (view == gui::dragsourceinstance->mSourceView)
        {
            delete gui::dragsourceinstance;
        }
    }
}

void HLMouseDispatcher::dispathTouchEventToEntity(HLEntity* entity, HLTouchEvent* event)
{
    bool isDispatchToCurrEntity = mCurrEntity == entity;
    HLEntity* tmp = mCurrEntity;
    mCurrEntity = entity;
    
    bool touchHandled = false;
    HLEntityTouchHandler::ListDelegateIterator itr = mCurrEntity->onTouchEvent.begin();
    for (; itr != mCurrEntity->onTouchEvent.end(); itr++)
    {
        if (*itr)
        {
            (*itr)->invoke(event);
            touchHandled = true;
            if (isDispatchToCurrEntity && !mCurrEntity) // entity may be destroyed during event handling
            {
                return;
            }
            else if (!mCurrEntity)
            {
                mCurrEntity = tmp;
                return;
            }
        }
    }
    if (!touchHandled)
    {
        entity->dispatchTouchEvent(event);
    }
    mCurrEntity = tmp;
}

void HLMouseDispatcher::dispathMouseEventToEntity(HLEntity* entity, HLMouseEvent* event)
{
    bool isDispatchToCurrEntity = mCurrEntity == entity;
    HLEntity* tmp = mCurrEntity;
    mCurrEntity = entity;

    bool mouseHandled = false;
    HLEntityMouseHandler::ListDelegateIterator itr = mCurrEntity->onMouseEvent.begin();
    for (; itr != mCurrEntity->onMouseEvent.end(); itr++)
    {
        (*itr)->invoke(event);
        mouseHandled = true;
        if (isDispatchToCurrEntity && !mCurrEntity) // entity may be destroyed during event handling
        {
            return;
        }
        else if (!mCurrEntity)
        {
            mCurrEntity = tmp;
            return;
        }
    }
    if (!mouseHandled)
    {
        entity->dispatchMouseEvent(event);
    }
    mCurrEntity = tmp;
}

void HLMouseDispatcher::hitTest(const HLPoint& point)
{
    mNeedsHitTest = false;
    
//    struct timeval now;
//    gettimeofday(&now, 0);
//    float delta = (now.tv_sec - timestamp.tv_sec) + (now.tv_usec - timestamp.tv_usec) / 1000000.0f;
//    if (timestamp.tv_sec == 0 || delta > 0.5f)
//    {
//        mClickCount = 0;
//    }
//    gettimeofday(&timestamp, 0);
    
    gui::HLView* view = gui::HLGUIManager::getSingleton()->getRootWidget()->hitTest(point);
    
    if (view)
    {
        if (mCurrView != view)
        {
            mClickCount = 0;
        }
        mCurrView = view;
        mCurrEntity = NULL;
        return;
    }
    mCurrView = NULL;
    
    if(HLDirector2D::getSingleton()->getCurrentSceneManager())
    {
        HLEntity* entity = HLDirector2D::getSingleton()->getCurrentSceneManager()->hitTest(point);
        if (entity != mCurrEntity)
        {
            mClickCount = 0;
        }
        mCurrEntity = entity;
    }
}

void HLMouseDispatcher::onMouseEvent(HLMouseEvent& event)
{
    if ((event.type == kHLMouseDown || event.type == kHLRightMouseDown))
    {
        struct timeval now;
        gettimeofday(&now, 0);
        float delta = (now.tv_sec - timestamp.tv_sec) + (now.tv_usec - timestamp.tv_usec) / 1000000.0f;
        if (timestamp.tv_sec == 0 || delta > 0.5f)
        {
            mClickCount = 0;
            mNeedsHitTest = true;
        }
        gettimeofday(&timestamp, 0);
        
        HLPoint loc = event.location;
        if (!mNeedsHitTest && (abs(loc.x-lastPoint.x) > 5 || abs(loc.y-lastPoint.y) > 5))
        {
            mNeedsHitTest = true;
        }
        if (mNeedsHitTest)
        {
            hitTest(event.location);
        }
    }
    if (event.type == kHLMouseUp || event.type == kHLRightMouseUp)
    {
        lastPoint = event.location;
    }
    if (event.type == kHLMouseDown)
    {
        mClickCount++;
    }
    event.clickCount = mClickCount;
    if (gui::dragsourceinstance)
    {
        if (event.type == kHLMouseUp)
        {
            gui::dragsourceinstance->onDraggingEnd(event.location);
        }
        else
        {
            gui::dragsourceinstance->onDraggingMove(event.location);
        }
        return;
    }
    if (!mCurrView&&!mCurrEntity)
    {
        return;
    }

    ++mId;
    if (mId == INT_MAX)
    {
        mId = 1;
    }
    
#ifdef USE_TOUCH_EVENT
    HLTouchEvent touchEvent;
    touchEvent._id = mId;
    HLTouch touch;
    touch._id = 0;
    touch.location = event.location;
    touch.tapCount = event.clickCount;
    touchEvent.touches.push_back(touch);
    switch (event.type)
    {
        case kHLMouseDown:
            touchEvent.type = kHLTouchesBegin;
            break;
        case kHLMouseDrag:
            touchEvent.type = kHLTouchesMove;
            break;
        case kHLMouseUp:
            touchEvent.type = kHLTouchesEnd;
            break;
        case kHLMouseCancel:
            touchEvent.type = kHLTouchesCancel;
            break;
        default:
            return;
            break;
    }
    HLTouchDispatcher::sharedTouchDispatcher()->touchEventHandler(&touchEvent);
#else
    mouseEventHandler(&event);
#endif
    
    if (mCurrView)
    {
#ifdef USE_TOUCH_EVENT
        mCurrView->handleTouchEvent(&touchEvent);
#else
        mCurrView->handleMouseEvent(&event);
#endif
    }
    else
    {
#ifdef USE_TOUCH_EVENT
        dispathTouchEventToEntity(mCurrEntity, &touchEvent);
#else
        dispathMouseEventToEntity(mCurrEntity, &event);
#endif
    }
}

NS_HL_END
