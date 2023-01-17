//
//  HLGUIViewComponent.h
//  HoolaiEngine
//
//  Created by tyq on 2/15/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLGUIVIEWCOMPONENT_H__
#define __HLGUIVIEWCOMPONENT_H__

#include "HLEntitySystem.h"
#include "HLGeometry.h"
#include "HLWidget.h"

NS_HL_BEGIN

class HLGUIViewComponent:public HLComponent
{
    PROPERTIES_DECLARE
    PROPERTY_DECLARE(HLGUIViewComponent, gui::HLView*, view)
    
public:
    
    HLGUIViewComponent(){mViewContainer = new gui::HLView(NULL);mViewOnTouch = NULL; m_view = NULL;}
    ~HLGUIViewComponent(){delete mViewContainer;}
    
    virtual void onActive();
    virtual void onDeactive();
    
private:
    void hitTest(kmMat4* transform, const HLPoint& point, bool *ret);
    
    void onDraw();
    
    void onTouches(hoolai::HLTouchEvent* event);
    
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    void onMouseEvent(hoolai::HLMouseEvent* event);
#endif
    
private:
    gui::HLView* mViewContainer;
    gui::HLView* mViewOnTouch;
    
    FAMILYID
};

NS_HL_END

#endif
