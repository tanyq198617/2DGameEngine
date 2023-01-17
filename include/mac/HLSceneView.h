//
//  HLEntityView.h
//  HoolaiEngine
//
//  Created by tyq on 3/3/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLENTITYVIEW_H__
#define __HLENTITYVIEW_H__

#include "HLView.h"

/*
 show entity, and pass touch&mouse events to entity
 TODO hit test and dispatch touch/mouse events
 */

NS_HL_BEGIN

class HLEntityManager;
class HLEntity;

class HLTimer;

namespace gui
{
    
    class HLSceneView: public HLView
    {
    public:
        HLSceneView(HLView* parent, HLRect frame):HLView(parent, frame), mSceneManager(NULL), mTimer(NULL), mEntityOnTouch(NULL) {}
        HLSceneView(HLView* parent):HLView(parent), mSceneManager(NULL), mTimer(NULL) {}
        
        ~HLSceneView();
        
        void setSceneManager(HLSceneManager* sceneMgr);
        
        HLSceneManager* getSceneManager()
        {
            return mSceneManager;
        }
        
        virtual void onTouches(HLTouchEvent* touch);
        
        void entityWillDestroy(HLEntity* entity);
        
#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
    protected:
        virtual void onDraw();
    private:
        void update(HLTimer* timer, float dt);
        void updateEntity(HLEntity* entity, float dt);
        void drawEntity(HLEntity* entity);
        HLSceneManager* mSceneManager;
        HLTimer* mTimer;
        HLEntity* mEntityOnTouch;
    };

}

NS_HL_END

#endif
