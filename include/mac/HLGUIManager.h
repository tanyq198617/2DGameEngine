//
//  HLGUIManager.h
//  HoolaiEngine
//
//  Created by tyq on 13-1-17.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLGUIMANAGER_H__
#define __HLGUIMANAGER_H__

#include "HLMacros.h"
#include "HLSingleton.h"
#include "HLWidget.h"
#include <list>

NS_HL_BEGIN

namespace gui
{
    class HLGUIManager: public HLSingleton<HLGUIManager>
    {
        friend class HLSingleton<HLGUIManager>;
        friend class HLAlertView;
    public:
        void drawFrame();
        void update(float dt);
        HLRootWidget* getRootWidget()
        {
            return &mRootWidget;
        }
//        void onWinSizeChanged();
    private:
//        void updateWidgetTransform(HLWidget* widget);
//        void updateViewTransform(HLView *view);
        HLRootWidget mRootWidget;
        std::list<HLView*> mUpdateableView;
    };
}

NS_HL_END

#endif
