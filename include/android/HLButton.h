//
//  HLButton.h
//  HoolaiEngine
//
//  Created by zhao shuan on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLBUTTON_H__
#define __HLBUTTON_H__

#include "HLMacros.h"
#include "HLLabel.h"
#include "HLImageView.h"
#include "HLDelegate.h"

NS_HL_BEGIN

namespace gui
{
    class HLButton: public HLView
    {
        typedef CDelegate1<HLButton*> HLButtonClickHandler;

    public:
        
        typedef enum
        {
            kButtonStateNormal,
            kButtonStateSelected,
            kButtonStateHighlighted,
            kButtonStateDisabled
        } HLButtonState;
        
        HLButton(HLView * parent);
        HLButton(HLView *parent, HLRect frame);
        HLButton(HLView *parent, const HLButton& copy);
        ~HLButton();
        virtual void onTouches(HLTouchEvent* touch);
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        virtual void onMouseEvent(HLMouseEvent* mouseEvent);
#endif
        void setZoomOnTouchDown(bool zoom) {mZoomOnTouchDown = zoom;}
        bool isZoomOnTouchDown() {return mZoomOnTouchDown;}
        void setSelected(bool selected) {mButtonState = selected?kButtonStateSelected:kButtonStateNormal; statusChanged();}
        HLButtonState getButtonState() {return mButtonState;}
        void setButtonState(HLButtonState state) {mButtonState = state;  statusChanged();}
        void setEnabled(bool enabled) {mButtonState = enabled?kButtonStateNormal:kButtonStateDisabled; statusChanged();}
        void setHighlighted(bool highlighted);
        void setTitleSize(int size){mTitleLabel->setFontSize(size);}
        void setTitleColor(color4B color){mTitleLabel->setTextColor(color);}
        HLLabel* getTitleLabel(){return mTitleLabel;}
        HLImageView* getImageView(){return mBackgroundImageView;}
        void setNormalBackgroundImage(HLTexture* tex, HLRect capInsets = HLRectZero);
        void setNormalBackgroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets = HLRectZero);
        void setDisabledBackgroundImage(HLTexture* tex, HLRect capInsets = HLRectZero);
        void setDisabledBackgroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets = HLRectZero);
        void setSelectedBackgroundImage(HLTexture* tex, HLRect capInsets = HLRectZero);
        void setSelectedBackgroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets = HLRectZero);
        void setHighlightedBackgroundImage(HLTexture* tex, HLRect capInsets = HLRectZero);
        void setHighlightedBackgroundImage(HLSpriteFrame* spriteFrame, HLRect capInsets = HLRectZero);
        void setNormalTitle(std::string title){mNormalTileText = title; if(mButtonState==kButtonStateNormal)statusChanged();}
        std::string getNormalTitle() {return mNormalTileText;}
        void setSelectedTitle(std::string title){mSelectedTitleText = title; if(mButtonState==kButtonStateSelected)statusChanged();}
        std::string getSelectedTitle() {return mSelectedTitleText;}
        void setDisabledTitle(std::string title){mDisableTitleText = title; if(mButtonState==kButtonStateDisabled)statusChanged();}
        std::string getDisabledTitle() {return mDisableTitleText;}
        void setHighlightedTitle(std::string title){mHighlightedTitleText = title; if(mButtonState==kButtonStateHighlighted)statusChanged();}
        std::string getHighlightedTitle() {return mHighlightedTitleText;}
        virtual void visit();
        
        HLButtonClickHandler onButtonClick;
        
#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
        
        virtual HLView* clone(HLView* parent);
        
    private:
        void statusChanged();
        void initMembers();
        virtual void onInternalEvent(const char* event);
        
        HLImageView* mBackgroundImageView;
        HLLabel* mTitleLabel;
        bool mZoomOnTouchDown;
        bool mZoom;
        
        HLButtonState mButtonState;
        HLButtonState mButtonStateBeforeHighlight;
        HLSpriteFrame* mNormalBackgroundTexture;
        HLSpriteFrame* mDisableBackgroundTexture;
        HLSpriteFrame* mSelectedBackgroundTexture;
        HLSpriteFrame* mHighlightedBackgroundTexture;
        HLRect mNormalBackgroundTextureCapInsets;
        HLRect mDisableBackgroundTextureCapInsets;
        HLRect mSelectedBackgroundTextureCapInsets;
        HLRect mHighlightedBackgroundTextureCapInsets;
        std::string mNormalTileText;
        std::string mSelectedTitleText;
        std::string mDisableTitleText;
        std::string mHighlightedTitleText;
    public:
        HLSpriteFrame* getNormalBackgroundSpriteFrame();
        HLSpriteFrame* getSelectedBackgroundSpriteFrame();
        HLSpriteFrame* getDisableBackgroundSpriteFrame();
        HLSpriteFrame* getHighlightedBackgroundSpriteFrame();

    };
}

NS_HL_END

#endif 
