//
//  HLViewDragSource.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-14.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLVIEWDRAGSOURCE_H__
#define __HLVIEWDRAGSOURCE_H__

#include "HLMacros.h"
#include "HLGeometry.h"
#include <stdint.h>

NS_HL_BEGIN

class HLTexture;
class HLSpriteFrame;
class HLTouchDispatcher;
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
class HLMouseDispatcher;
#endif

namespace gui
{
    class HLImageView;
    class HLView;
    class HLWidget;
    
    typedef enum {
        kHLViewDraggingEntered,
        kHLViewDraggingExited,
        kHLViewDraggingDropped,
        kHLViewDraggingAccepted,
        kHLViewDraggingCanceled
    } HLViewDraggingEventType;
    
    typedef struct {
        HLViewDraggingEventType type;
        HLPoint location;
    } HLViewDraggingEvent;
    
    class HLViewDragSource
    {
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        friend class ::hoolai::HLMouseDispatcher;
#endif
        friend class ::hoolai::HLTouchDispatcher;
        friend class HLGUIManager;
    public:
        HLViewDragSource();
        ~HLViewDragSource();
        void doDragDrop(HLPoint p);
        void setDraggingView(HLView* view, float scale = 1.2, float opacity = 0.8);
        void setDraggingImage(HLTexture* tex, float scale = 1.2, float opacity = 0.8);
        void setDraggingImage(HLSpriteFrame* tex, float scale = 1.2, float opacity = 0.8);
        void setUserData(void* data) {mUserData = data;}
        void* getUserData() {return mUserData;}
        void setTag(uint32_t tag) {mTag = tag;}
        HLView* getSourceView() {return mSourceView;}
        uint32_t getTag() {return mTag;}
    private:
        void draw();
        void onDraggingMove(HLPoint p);
        void onDraggingEnd(HLPoint p);
        void onDraggingCanceled();
        void* mUserData;
        uint32_t mTag;
        HLImageView* mImageView;
        HLWidget* mWidget;
        HLView* mCurrDropTarget;
        HLView* mSourceView;
    };
}

NS_HL_END

#endif
