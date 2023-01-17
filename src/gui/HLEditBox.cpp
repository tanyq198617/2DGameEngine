//
//  HLEditBox.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLEditBox.h"
#include "HLTimer.h"
#include "HLDrawingPrimitives.h"
#include <string>
#include "JSEditBox.h"

void __activeTextInputWithEditBox(hoolai::gui::HLEditBox* editBox);
void __deactiveTextInput();

NS_HL_BEGIN

int utf8_strlen (const char * p, int max);

namespace gui
{

HLEditBox::HLEditBox(HLView* parent, HLRect frame, bool multiline):HLView(parent, frame), mActive(false), mSecureInput(false), mShowIndicater(false), mTimer(NULL), mMultilineFlag(multiline), mMaxLength(0)
{
    color4B color;
    color.r = color.g = color.b = 0;
    color.a = 255;
    mLabelView = new HLView(this);
    mLabelView->setClipToBounds(true);
    mLabel = new HLLabel(mLabelView);
    mLabel->setMultiline(mMultilineFlag);
    mLabel->setTextColor(color);
    mLabel->setTextAlignment(kTextHAlignmentLeft | kTextVAlignmentCenter);
    mMarkedLabel = new HLLabel(mLabelView);
    mMarkedLabel->setVisible(false);
    mMarkedLabel->setMultiline(false);
    mMarkedLabel->setTextColor(color);
    mMarkedLabel->setTextAlignment(kTextHAlignmentLeft | kTextVAlignmentCenter);
    color.r = 0; color.g = color.b = 255; color.a = 100;
    mMarkedLabel->setBackgroundColor(color);
    mPlaceHolderLabel = new HLLabel(this);
    mPlaceHolderLabel->setTextAlignment(kTextHAlignmentLeft | kTextVAlignmentCenter);
    color.r = color.g = color.b = 100;
    color.a = 255;
    mPlaceHolderLabel->setTextColor(color);
    HLEdgeInsets insets;
    insets.top = insets.bottom = 0;
    insets.left = insets.right = 3;
    setEdgeInsets(insets);
    mUserInteractionEnabled = true;
    mReturnKeyType = kKeyboardReturnTypeDefault;
    mInputMode = kEditBoxInputModeAny;
}
    
HLEditBox::~HLEditBox()
{
#if USE_JS_SCRIPTING
    JSEditBox::removeJSObject(this);
#endif
    if (mActive)
    {
        resignActive();

    }
}
    
void HLEditBox::onInternalEvent(const char* event)
{
    if (!strcmp(event, "size_changed") || !strcmp(event, "frame_changed") || !strcmp(event, "edgeinsets_changed"))
    {
        HLRect labelframe(0, 0, mBounds.size.width, mBounds.size.height);
        mLabelView->setFrame(labelframe);
        mLabel->setFrame(labelframe);
        mPlaceHolderLabel->setFrame(labelframe);
        setIndicaterPos();
    }
}

void HLEditBox::onTouches(HLTouchEvent* touch)
{
    if (touch->type == kHLTouchesBegin)
    {
        active();
    }
}

#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
void HLEditBox::onMouseEvent(HLMouseEvent* mouseEvent)
{
    if (mouseEvent->type == kHLMouseDown)
    {
        active();
    }
}
#endif

void HLEditBox::active()
{
    if (mActive)
    {
        return;
    }
    mActive = true;
    if (mPlaceHolderLabel->isVisible())
    {
        mPlaceHolderLabel->setVisible(false);
    }
    __activeTextInputWithEditBox(this);
    onEditBegin(this);
#if !defined(TARGET_ANDROID)
    mTimer = new HLTimer(0.5, true);
    mShowIndicater = true;
    mTimer->delegate = newDelegate(this, &HLEditBox::updateIndecater);
    float width = mLabel->sizeThatFits().width;
    if (width > mBounds.size.width)
    {
        mLabel->setPosition(HLPoint(mBounds.size.width-width, 0));
    }
    setIndicaterPos();
#endif
}

void HLEditBox::updateIndecater(HLTimer*, float)
{
    mShowIndicater = !mShowIndicater;
}

void HLEditBox::resignActive()
{
    if (mText.length() == 0)
    {
        mPlaceHolderLabel->setVisible(true);
    }
    if (!mActive)
    {
        return;
    }
    mActive = false;
    onEditEnd(this);
#if !defined(TARGET_ANDROID)
    __deactiveTextInput();
    if (mTimer)
    {
        mTimer->cancel();
        mTimer = NULL;
    }
    mShowIndicater = false;
    mLabel->setPosition(HLPointZero);
#endif
}

void HLEditBox::setSecureInput(bool b)
{
    mSecureInput = b;
    if (mSecureInput)
    {
        if (mText.length())
        {
            std::string tmp;
            for (int i = 0; i < mText.length(); ++i)
            {
                tmp += "*";
            }
            mLabel->setText(tmp);
        }
    }
    else
    {
        mLabel->setText(mText);
    }
}

void HLEditBox::insertText(std::string text)
{
    if (!text.length())
    {
        return;
    }
    if (mPlaceHolderLabel->isVisible())
    {
        mPlaceHolderLabel->setVisible(false);
    }
    mText += text;
    if (mSecureInput)
    {
        int len = utf8_strlen(text.c_str(), -1);
        std::string tmp;
        for (int i = 0; i < len; ++i)
        {
            tmp += "*";
        }
        std::string s = mLabel->getText();
        mLabel->setText(s + tmp);
    }
    else
    {
        mLabel->setText(mText);
    }

    float width = mLabel->sizeThatFits().width;
    if (width > mBounds.size.width)
    {
        mLabel->setPosition(HLPoint(mBounds.size.width-width, 0));
    }
    setIndicaterPos();
    
    onTextChanged(this);
}

void HLEditBox::setText(const char* str)
{
    if (!strcmp(mText.c_str(), str))
    {
        return;
    }
    
    if (str[0])
    {
        mPlaceHolderLabel->setVisible(false);
    }
    else
    {
        mPlaceHolderLabel->setVisible(true);
    }
    
    mText = str;
    if (mSecureInput)
    {
        if (mText.length())
        {
            std::string tmp;
            for (int i = 0; i < mText.length(); ++i)
            {
                tmp += "*";
            }
            mLabel->setText(tmp);
        }
        else
        {
            mLabel->setText("");
        }
    }
    else
    {
        mLabel->setText(str);
    }
    
    setIndicaterPos();
    onTextChanged(this);
}
    
void HLEditBox::setMarkedText(const char* str)
{
    if (str && strlen(str) > 0)
    {
        mMarkedLabel->setText(str);
        HLSize size = mMarkedLabel->sizeThatFits();
        mMarkedLabel->setFrame(HLRect(mIndicaterPos.x, mIndicaterPos.y, size.width, size.height));
        mMarkedLabel->setVisible(true);
    }
    else
    {
        mMarkedLabel->setVisible(false);
    }
}

void HLEditBox::deleteBackward()
{
    if (mText.length() > 0)
    {
        int dellen = 0;
        size_t len = mText.length();
        if (len >= 6 && ((mText[len-6] & 0xfe) == 0xfc))
        {
            dellen = 6;
        }
        else if (len >= 5 && ((mText[len-5] & 0xfc) == 0xf8))
        {
            dellen = 5;
        }
        else if (len >= 4 && ((mText[len-4] & 0xf8) == 0xf0))
        {
            dellen = 4;
        }
        else if (len >= 3 && ((mText[len-3] & 0xf0) == 0xe0))
        {
            dellen = 3;
        }
        else if (len >= 2 && ((mText[len-2] & 0xe0) == 0xc0))
        {
            dellen = 2;
        }
        else
        {
            dellen = 1;
        }

        mText.erase(mText.length()-dellen, dellen);
        if (mSecureInput)
        {
            std::string s = mLabel->getText();
            s.erase(s.length()-1, 1);
            mLabel->setText(s);
        }
        else
        {
            mLabel->setText(mText);
        }

        float width = mLabel->sizeThatFits().width;
        if (width > mBounds.size.width)
        {
            mLabel->setPosition(HLPoint(mBounds.size.width-width, 0));
        }
        else
        {
            mLabel->setPosition(HLPointZero);
        }
        setIndicaterPos();
        
        onTextChanged(this);
    }
}

void HLEditBox::setIndicaterPos()
{
    int mAlign = mLabel->getTextAlignment();
    if (mLabel->getText().length() == 0)
    {
        if (mAlign&kTextHAlignmentLeft)
        {
            mIndicaterPos.x = 0;
        }
        else if (mAlign&kTextHAlignmentCenter)
        {
            mIndicaterPos.x = mLabel->getSize().width/2 + mLabel->getFontSize()/2;
        }
        else
        {
            mIndicaterPos.x = mLabel->getSize().width;
        }
        if (mAlign&kTextVAlignmentTop)
        {
            mIndicaterPos.y = mLabel->getSize().height - mLabel->getFontSize();
        }
        else if (mAlign&kTextVAlignmentCenter)
        {
            mIndicaterPos.y = mLabel->getSize().height/2 - mLabel->getFontSize()/2;
        }
        else
        {
            mIndicaterPos.y = 0;
        }
        return;
    }
    mIndicaterPos.x = mLabel->getPosition().x + mLabel->sizeThatFits().width;
    mIndicaterPos.y = mLabel->getPosition().y + mBounds.size.height-mLabel->sizeThatFits().height+2;
    if((mAlign&kTextHAlignmentCenter) || (mAlign&kTextHAlignmentRight) ||(mAlign&kTextHAlignmentLeft))
    {
        if(mAlign&kTextHAlignmentCenter)
        {
            mIndicaterPos.x = mLabel->getPosition().x + (mLabel->getSize().width + mLabel->getTextEndX())/2;
        }
        else if(mAlign&kTextHAlignmentRight)
        {
            mIndicaterPos.x = mLabel->getPosition().x + mLabel->getSize().width;
        }
        else if (mAlign&kTextHAlignmentLeft)
        {
            mIndicaterPos.x = mLabel->getPosition().x + mLabel->getTextEndX();

        }
    }
    if((mAlign&kTextVAlignmentCenter) || (mAlign&kTextVAlignmentBottom))
    {
        if(mAlign&kTextVAlignmentCenter)
        {
            if(mLabel->sizeThatFits().height > mLabel->getFontSize())
            {
                mIndicaterPos.y = mLabel->getPosition().y + (mBounds.size.height-mLabel->sizeThatFits().height+2)/2;
            }
            else
            {
                mIndicaterPos.y = mLabel->getPosition().y + (mBounds.size.height-mLabel->getFontSize())/2;
            }
        }
        else if(mAlign&kTextVAlignmentBottom)
        {
            mIndicaterPos.y = mLabel->getPosition().y;
        }
    }

    //mIndicaterPos.x = MAX(mIndicaterPos.x,mLabel->getPosition().x);
    //mIndicaterPos.x = MIN(mIndicaterPos.x,mLabel->getPosition().x + mLabel->getSize().width);
    //mIndicaterPos.y = MAX(mIndicaterPos.y,mLabel->getPosition().y + (int)mBounds.size.height%mLabel->getFontSize());
    //mIndicaterPos.y = MIN(mIndicaterPos.y,mLabel->getPosition().y + mBounds.size.height-mLabel->getFontSize());
}
    
void HLEditBox::clearText()
{
    mText = "";
    mLabel->setText(mText);
    
    setIndicaterPos();
    
    onTextChanged(this);
}


void HLEditBox::onDraw()
{
    if (mActive)
    {
        if (mShowIndicater)
        {
            hlglLineWidth(2);
            drawColor4F(0, 0, 1, 1);
            drawLine(mIndicaterPos, mIndicaterPos + HLPoint(0, mLabel->getFontSize()));
        }
    }

    HLView::onDraw();
}
    
#if USE_JS_SCRIPTING
JSCppWrapperBase* HLEditBox::getOrCreateJSWrapper(JSContext* cx)
{
    return JSEditBox::getOrCreateWrapper(cx, this);
}
#endif
}

NS_HL_END
