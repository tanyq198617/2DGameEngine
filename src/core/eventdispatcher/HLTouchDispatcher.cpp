//
//  HLTouchDispatcher.cpp
//  HoolaiEngine2D
//
//  Created by tyq on 12-6-13.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLTouchDispatcher.h"
#include "HLSceneManager.h"
#include "GameEngineConfig.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif
#include "HLView.h"
#include "HLGUIManager.h"
#include "HLViewDND.h"

NS_HL_BEGIN

namespace gui
{
    extern HLViewDragSource* dragsourceinstance;
}

void _setMultiTouchEnabled(bool enable);

HLTouchDispatcher HLTouchDispatcher::dispatcher;

void HLTouchDispatcher::setMultiTouchEnabled(bool enabled)
{
    cancelCurrentEventHandler();
    _setMultiTouchEnabled(enabled);
}

void HLTouchDispatcher::entityWillDestroy(HLEntity* entity)
{
    if (mCurrEntity == entity)
    {
        mCurrEntity = NULL;
        needsHitTest();
    }
}

void HLTouchDispatcher::viewWillDestroy(gui::HLView* view)
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

void HLTouchDispatcher::hitTest(const HLPoint& point)
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

void HLTouchDispatcher::dispatchTouchEventToEntity(HLEntity* entity, HLTouchEvent* event)
{
    bool isDispatchToCurrEntity = (mCurrEntity == entity);
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

void HLTouchDispatcher::touchesBegin(std::vector<HLTouch>& touches)
{
    struct timeval now;
    gettimeofday(&now, 0);
    
    for (std::list<CompleteTouchInfo>::iterator itr = mCompleteTouchInfos.begin(); itr != mCompleteTouchInfos.end(); )
    {
        float delta = (now.tv_sec - (*itr).timestamp.tv_sec) + (now.tv_usec - (*itr).timestamp.tv_usec) / 1000000.0f;
        if (delta > 0.5f)
        {
            itr = mCompleteTouchInfos.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
    
    HLPoint distance;
    
    std::vector<HLTouch>::iterator touchesitr = touches.begin();
    
    if (mEventDispatching == -1)
    {
        mId = 1;
        HLPoint loc = touches[0].location;
        if (!mNeedsHitTest)
        {
            mNeedsHitTest = true;
            touches[0].tapCount = 1;
            for (std::list<CompleteTouchInfo>::iterator itr = mCompleteTouchInfos.begin(); itr != mCompleteTouchInfos.end(); ++itr)
            {
                HLPoint distance = loc - (*itr).location;
                if (distance.x*distance.x+distance.y*distance.y < 25)
                {
                    touches[0].tapCount = (*itr).tapCount + 1;
                    mNeedsHitTest = false;
                    break;
                }
            }
        }
        
        if (mNeedsHitTest)
        {
            gui::HLView *lastView = mCurrView;
            HLEntity* lastEntity = mCurrEntity;
            hitTest(touches[0].location);
            if (lastView != mCurrView || lastEntity != mCurrEntity)
            {
                touches[0].tapCount = 1;
            }
        }
        
        ++touchesitr;
    }
    
    for (; touchesitr != touches.end(); ++touchesitr)
    {
        (*touchesitr).tapCount = 1;
        HLPoint loc = (*touchesitr).location;
        for (std::list<CompleteTouchInfo>::iterator itr = mCompleteTouchInfos.begin(); itr != mCompleteTouchInfos.end(); ++itr)
        {
            if (loc.dot((*itr).location) < 25)
            {
                (*touchesitr).tapCount = (*itr).tapCount + 1;
            }
        }
    }
    
    if (!mCurrView&&!mCurrEntity) 
    {
        return;
    }
    HLTouchEvent event;
    event._id = mId++;
    event.touches = touches;
    event.type = kHLTouchesBegin;
    
    touchEventHandler(&event);
    
    do
    {
        if (mCurrView)
        {
            if (!mCurrView->multiTouchEnabled())
            {
                if (mEventDispatching != -1)
                    break;
                if (touches.size() > 1)
                {
                    event.touches.clear();
                    event.touches.push_back(touches[0]);
                }
            }
            mCurrView->handleTouchEvent(&event);
        }
        else
        {
            if (mCurrEntity)
            {
                if (!mCurrEntity->multiTouchEnabled())
                {
                    if (mEventDispatching != -1)
                        break;
                    if (touches.size() > 1)
                    {
                        event.touches.clear();
                        event.touches.push_back(touches[0]);
                    }
                }
                dispatchTouchEventToEntity(mCurrEntity, &event);
            }
        }
    } while (0);
    
    if (mEventDispatching == -1)
    {
        mEventDispatching = touches[0]._id;
    }
    
    for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
    {
        mTouchEvents.insert(std::make_pair((*itr)._id, std::make_pair((*itr).tapCount, now)));
    }
}

void HLTouchDispatcher::touchesMove(std::vector<HLTouch>& touches)
{
    for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
    {
        std::map<int, std::pair<int, struct timeval> >::iterator res = mTouchEvents.find((*itr)._id);
        if (res != mTouchEvents.end())
        {
            (*itr).tapCount = res->second.first;
        }
    }
    
    if (gui::dragsourceinstance)
    {
        for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
        {
            if ((*itr)._id == mEventDispatching)
            {
                gui::dragsourceinstance->onDraggingMove((*itr).location);
                break;
            }
        }
        return;
    }
    
    if (!mCurrView&&!mCurrEntity) 
    {
        return;
    }
    
    HLTouchEvent event;
    event._id = mId++;
    event.touches = touches;
    event.type = kHLTouchesMove;
    
    touchEventHandler(&event);
    if (mCurrView)
    {
        if (!mCurrView->multiTouchEnabled())
        {
            event.touches.clear();
            for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
            {
                if ((*itr)._id == mEventDispatching)
                {
                    event.touches.push_back(*itr);
                    break;
                }
            }
            if (event.touches.empty())
            {
                return;
            }
        }
        mCurrView->handleTouchEvent(&event);
    }
    else
    {
        if (mCurrEntity)
        {
            if (!mCurrEntity->multiTouchEnabled())
            {
                event.touches.clear();
                for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
                {
                    if ((*itr)._id == mEventDispatching)
                    {
                        event.touches.push_back(*itr);
                        break;
                    }
                }
                if (event.touches.empty())
                {
                    return;
                }
            }
        }
        dispatchTouchEventToEntity(mCurrEntity, &event);
    }
}

void HLTouchDispatcher::touchesEnd(std::vector<HLTouch>& touches)
{
    CompleteTouchInfo info;
    
    for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
    {
        std::map<int, std::pair<int, struct timeval> >::iterator res = mTouchEvents.find((*itr)._id);
        if (res != mTouchEvents.end())
        {
            (*itr).tapCount = res->second.first;
            info.timestamp = res->second.second;
        }
        
        if ((*itr)._id == mEventDispatching)
        {
            lastPoint = (*itr).location;
        }
        
        info.location = (*itr).location;
        info.tapCount = (*itr).tapCount;
        mCompleteTouchInfos.push_back(info);
    }
    
    for (std::vector<HLTouch>::iterator it = touches.begin(); it != touches.end(); ++it)
    {
        std::map<int, std::pair<int, struct timeval> >::iterator res = mTouchEvents.find((*it)._id);
        if (res != mTouchEvents.end())
        {
            mTouchEvents.erase(res);
        }
    }
    
    if (gui::dragsourceinstance)
    {
        for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
        {
            if ((*itr)._id == mEventDispatching)
            {
                gui::dragsourceinstance->onDraggingEnd((*itr).location);
                break;
            }
        }
        
        if (mTouchEvents.empty())
        {
            mEventDispatching = -1;
        }
        
        return;
    }
    
    if (!mCurrView&&!mCurrEntity) 
    {
        if (mTouchEvents.empty())
        {
            mEventDispatching = -1;
        }
        return;
    }
    HLTouchEvent event;
    event._id = mId++;
    event.touches = touches;
    event.type = kHLTouchesEnd;
    
    touchEventHandler(&event);
    
    do
    {
        if (mCurrView)
        {
            if (!mCurrView->multiTouchEnabled())
            {
                event.touches.clear();
                for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
                {
                    if ((*itr)._id == mEventDispatching)
                    {
                        event.touches.push_back(*itr);
                        break;
                    }
                }
                if (event.touches.empty())
                {
                    break;
                }
            }
            mCurrView->handleTouchEvent(&event);
        }
        else
        {
            if (mCurrEntity)
            {
                if (!mCurrEntity->multiTouchEnabled())
                {
                    event.touches.clear();
                    for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
                    {
                        if ((*itr)._id == mEventDispatching)
                        {
                            event.touches.push_back(*itr);
                            break;
                        }
                    }
                    if (event.touches.empty())
                    {
                        break;
                    }
                }
            }
            dispatchTouchEventToEntity(mCurrEntity, &event);
        }
    } while (0);
    
    if (mTouchEvents.empty())
    {
        mEventDispatching = -1;
    }
}

void HLTouchDispatcher::touchesCancel(std::vector<HLTouch>& touches)
{
    for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
    {
        std::map<int, std::pair<int, struct timeval> >::iterator res = mTouchEvents.find((*itr)._id);
        if (res != mTouchEvents.end())
        {
            (*itr).tapCount = res->second.first;
        }
        
        if ((*itr)._id == mEventDispatching)
        {
            lastPoint = (*itr).location;
        }
    }
    
    for (std::vector<HLTouch>::iterator it = touches.begin(); it != touches.end(); ++it)
    {
        std::map<int, std::pair<int, struct timeval> >::iterator res = mTouchEvents.find((*it)._id);
        if (res != mTouchEvents.end())
        {
            mTouchEvents.erase(res);
        }
    }
    
    if (gui::dragsourceinstance)
    {
        for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
        {
            if ((*itr)._id == mEventDispatching)
            {
                gui::dragsourceinstance->onDraggingCanceled();
                break;
            }
        }
        
        if (mTouchEvents.empty())
        {
            mEventDispatching = -1;
        }
        
        return;
    }
    
    if (!mCurrView&&!mCurrEntity)
    {
        if (mTouchEvents.empty())
        {
            mEventDispatching = -1;
        }
        return;
    }
    HLTouchEvent event;
    event._id = mId++;
    event.touches = touches;
    event.type = kHLTouchesCancel;
    
    touchEventHandler(&event);
    
    do
    {
        if (mCurrView)
        {
            if (!mCurrView->multiTouchEnabled())
            {
                event.touches.clear();
                for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
                {
                    if ((*itr)._id == mEventDispatching)
                    {
                        event.touches.push_back(*itr);
                        break;
                    }
                }
                if (event.touches.empty())
                {
                    break;
                }
            }
            mCurrView->handleTouchEvent(&event);
        }
        else
        {
            if (mCurrEntity)
            {
                if (!mCurrEntity->multiTouchEnabled())
                {
                    event.touches.clear();
                    for (std::vector<HLTouch>::iterator itr = touches.begin(); itr != touches.end(); ++itr)
                    {
                        if ((*itr)._id == mEventDispatching)
                        {
                            event.touches.push_back(*itr);
                            break;
                        }
                    }
                    if (event.touches.empty())
                    {
                        break;
                    }
                }
            }
            dispatchTouchEventToEntity(mCurrEntity, &event);
        }
    } while (0);
    
    if (mTouchEvents.empty())
    {
        mEventDispatching = -1;
    }
}

NS_HL_END
