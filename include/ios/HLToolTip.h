//
//  HLToolTip.h
//  HoolaiEngine
//
//  Created by zhao shuan on 1/21/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLTOOLTIP_H__
#define __HLTOOLTIP_H__

#include "HLMacros.h"
#include "HLWidget.h"

NS_HL_BEGIN

namespace gui
{
    class HLToolTip : public HLView
    {
        typedef CDelegate1<HLToolTip*> HLToolTipEventHandler;
    public:
        HLToolTip(HLRect frame);
        virtual ~HLToolTip();
//        void setEventDispatchRect(HLRect rect) {mDispatchRect = rect;}
        void show(hoolai::gui::HLView* parent = NULL);
        void dismiss();
        HLToolTipEventHandler onDismiss;
        
        static void dismissCurrentToolTip();
        
        virtual void onTouches(HLTouchEvent* touch)
        {
            dismiss();
        }
        
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        virtual void onMouseEvent(HLMouseEvent* mouseEvent)
        {
            dismiss();
        }
#endif
        
#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
        
    private:
        bool mShown;
        HLRect mDispatchRect;
    };
}

NS_HL_END

#endif
