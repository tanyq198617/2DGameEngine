//
//  HLGUIManager.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-1-17.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLGUIManager.h"
#include "HLGL.h"
#include "kazmath/GL/matrix.h"
#include <algorithm>
#include "HLDirector2D.h"
#include "HLViewDND.h"

NS_HL_BEGIN

namespace gui
{
    extern HLViewDragSource* dragsourceinstance;
    
    void HLGUIManager::drawFrame()
    {
        mRootWidget.visit();
        if (dragsourceinstance)
        {
            dragsourceinstance->draw();
        }
    }
    
//    void HLGUIManager::onWinSizeChanged()
//    {
//        updateWidgetTransform(&mRootWidget);
//    }
//    
//    void HLGUIManager::updateWidgetTransform(HLWidget* widget)
//    {
//        widget->setSize(HLDirector2D::getSingleton()->getWinSize());
//        updateViewTransform(widget);
//        if (widget->mPresentWidget)
//        {
//            updateWidgetTransform(widget->mPresentWidget);
//        }
//    }
//    
//    void HLGUIManager::updateViewTransform(HLView *view)
//    {
//        view->updateTransform();
//        std::list<HLView*>::iterator itr = view->mSubViews.begin();
//        while (itr != view->mSubViews.end())
//        {
//            if (*itr)
//            {
//                (*itr)->updateTransform();
//            }
//            ++itr;
//        }
//    }
}

NS_HL_END
