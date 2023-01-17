//
//  HLToast.h
//  DivineComedy
//
//  Created by liumingliang on 13-3-26.
//  Copyright (c) 2013 zhao shuan. All rights reserved.
//

#ifndef __DivineComedy__HLToast__
#define __DivineComedy__HLToast__

#include "HLMacros.h"
#include "HLImageView.h"
#include "HLRichLabel.h"
#include "HLDelegate.h"
#include "HLButton.h"

NS_HL_BEGIN
class HLTimer;
namespace gui
{
    class HLToast:public HLView
    {
        friend class HLRootWidget;
    private:
        static HLTimer *mShowTimer;
        static std::list<HLToast*> mShowPendingList;
        
        void moveAndFade(HLTimer *timer , float dt);
        void initMember();

        float mTimeElapsed;
        void setString(std::string _string);
        void close();
        HLTimer *timer;
        HLRichLabel *label;
        
        void showImpl();
        static void update(HLTimer *timer , float dt);
    public:
        virtual ~HLToast();
        void show();
        HLToast(std::string _string);
        virtual void visit();

        HLRichLabel* getLabel();

#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
        
        virtual HLView* clone(HLView *parent);
    };
    
}
NS_HL_END

#endif /* defined(__DivineComedy__HLToast__) */
