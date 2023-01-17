//
//  HLDialog.h
//  HoolaiEngine
//
//  Created by tyq on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLALERT_H__
#define __HLALERT_H__

#include "HLMacros.h"
#include "HLImageView.h"
#include "HLLabel.h"
#include "HLDelegate.h"
#include "HLButton.h"

NS_HL_BEGIN

class HLTimer;

namespace gui
{
    class HLAlertView;
    
    typedef CDelegate2<HLAlertView*, int> ButtonClickHandler;
    typedef CDelegate1<HLAlertView*> AlertEventHandler;
    
    typedef enum
    {
        kHLAlertViewButtonLayoutNormal,
        kHLAlertViewButtonLayoutStacked
    } HLAlertViewButtonLayout;
    
    class HLAlertView : public HLView
    {
    public:
        inline
        static HLAlertView* newAlert(const char* title, const char* message, const char* cancelButtonTitle, const char* otherButtonTitles, ...)
        {
            HLAlertView* alert = new HLAlertView();
            va_list args;
            va_start(args,otherButtonTitles);
            alert->createViewContent(title, message, cancelButtonTitle, otherButtonTitles, args);
            va_end(args);
            return alert;
        }
        inline
        static HLAlertView* newAlert(char* title, char* message, char* cancelButtonTitle, char* otherButtonTitles[])
        {
            HLAlertView* alert = new HLAlertView();
            alert->createViewContent(title, message, cancelButtonTitle, otherButtonTitles);
            return alert;
        }
        inline
        static HLAlertView* newAlert(HLView* content, HLButton* buttons, ...)
        {
            HLAlertView* alert = new HLAlertView();
            va_list args;
            va_start(args,buttons);
            alert->createViewContent(content, buttons, args);
            va_end(args);
            return alert;
        }
        inline
        static HLAlertView* newAlert(HLView* content, HLButton* buttons[])
        {
            HLAlertView* alert = new HLAlertView();
            alert->createViewContent(content, buttons);
            return alert;
        }
        
        ButtonClickHandler onButtonClicked;
        AlertEventHandler onCancel;
        void show(bool animation = true);
        void dismiss();
        virtual void visit();
        static void setDarkTheme(); // use white text color, provide dark background image yourself
        
        virtual HLView* clone(HLView *parent);
        
        CDelegate1<HLAlertView*> onDismiss;
    private:
        HLAlertView() : HLView(NULL), mButtonLayout(kHLAlertViewButtonLayoutNormal), mScale(1), mTimeElapsed(0), mTitleLabel(NULL), mMessageLabel(NULL), mTimer(NULL) { }
        virtual ~HLAlertView();
        void onButtonPress(HLButton *sender);
        virtual void createViewContent(const char* title, const char* message, const char* cancelButtonTitle, const char* otherButtonTitles, va_list args);
        virtual void createViewContent(char* title, char* message, char* cancelButtonTitle, char* otherButtonTitles[]);
        virtual void createViewContent(HLView* content, HLButton* buttons, va_list args);
        virtual void createViewContent(HLView* content, HLButton* buttons[]);
        virtual void layoutSubviews();
//        void close();
//        void destroySelf(HLTimer * timer, float dt);
        void pulse(HLTimer *timer , float dt);
        void pulse1(HLTimer *timer , float dt);
        void pulse2(HLTimer *timer , float dt);
        float mScaleTo;
        float mScale;
        float mTimeElapsed;
        HLImageView* mBackgroundImage;
        HLLabel* mTitleLabel;
        HLLabel* mMessageLabel;
        std::vector<HLButton*> mButtons;
        HLAlertViewButtonLayout mButtonLayout;
        HLTimer* mTimer;
        static bool lightTheme;
    };
}

NS_HL_END

#endif
