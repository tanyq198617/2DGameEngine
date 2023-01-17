//
//  HLEditBox.h
//  HoolaiEngine
//
//  Created by zhao shuan on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLEDITBOX_H__
#define __HLEDITBOX_H__

#include "HLMacros.h"
#include "HLView.h"
#include "HLLabel.h"
#include "HLDelegate.h"

NS_HL_BEGIN

class HLTimer;

namespace gui
{
    enum KeyboardReturnType
    {
        kKeyboardReturnTypeDefault = 0,
        kKeyboardReturnTypeNext,
        kKeyboardReturnTypeDone,
        kKeyboardReturnTypeSend,
        kKeyboardReturnTypeSearch,
        kKeyboardReturnTypeGo
    };
    
    enum EditBoxInputMode
    {
        kEditBoxInputModeAny = 0,
        kEditBoxInputModeAscii,
        kEditBoxInputModeEmailAddr,
        kEditBoxInputModeNumeric,
        kEditBoxInputModeNumberPad,
        kEditBoxInputModePhoneNumber,
        kEditBoxInputModeUrl,
        kEditBoxInputModeDecimal
    };
    
    class HLEditBox;
    
    typedef CDelegate1<HLEditBox*> EditBoxEventHandler;
    
    class HLEditBox : public HLView
    {
    public:
        HLEditBox(HLView* parent, HLRect frame, bool multiline = false);
        virtual ~HLEditBox();
        
        EditBoxEventHandler onEditBegin;
        EditBoxEventHandler onEditEnd;
        EditBoxEventHandler onTextChanged;
        EditBoxEventHandler onReturn;
        
        void setKeyboardReturnType(KeyboardReturnType type) {if (!mMultilineFlag) mReturnKeyType = type;}
        KeyboardReturnType getKeyboardReturnType() {return mReturnKeyType;}
        void setInputMode(EditBoxInputMode inputMode) {mInputMode = inputMode;}
        EditBoxInputMode getInputMode() {return mInputMode;}
        
        void active();
        void resignActive();
        bool isActive(){return mActive;};
        void setSecureInput(bool b);
        bool isSecureInput() {return mSecureInput;}
        std::string getText() {return mText;}
        std::string getPlaceHolder() {return mPlaceHolderLabel->getText();}
        HLLabel* getTextLabel() {return mLabel;}
        HLLabel* getPlaceHolderLabel() {return mPlaceHolderLabel;}
        void setPlaceHolder(std::string str) {mPlaceHolderLabel->setText(str);}
        
        void setText(const char* str);
        void setMarkedText(const char* str);
        
        virtual void onInternalEvent(const char* event);
        
        virtual void onTouches(HLTouchEvent* touch);
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        virtual void onMouseEvent(HLMouseEvent* mouseEvent);
#endif
        void insertText(std::string text);
        void deleteBackward();
        
        void clearText();
        
        void setIndicaterPos();
        
        int getMaxLength() {return mMaxLength;}
        void setMaxLength(int maxLen) {mMaxLength = maxLen;}
        
        const char* getCharInputFilter() {return mCharInputFilter.c_str();}
        void setCharInputFilter(const char* filter) {mCharInputFilter = filter;}
        
        virtual void onDraw();
        
        bool allowMultiLine() {return mMultilineFlag;}
        void setMultiline(bool flag) {mMultilineFlag = flag;mLabel->setMultiline(flag);}
        
#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
    private:
        void updateIndecater(HLTimer*, float);
        
        KeyboardReturnType mReturnKeyType;
        EditBoxInputMode mInputMode;
        
        std::string mText;
        std::string mCharInputFilter;
        int mMaxLength;
        HLView* mLabelView;
        HLLabel* mLabel;
        HLLabel* mMarkedLabel;
        bool mActive;
        bool mSecureInput;
        HLPoint mIndicaterPos;
        bool mShowIndicater;
        HLLabel* mPlaceHolderLabel;
        HLTimer * mTimer;
        bool mMultilineFlag;
    };
}

NS_HL_END

#endif
