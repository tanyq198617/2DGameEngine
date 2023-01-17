//
//  HLDialog.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLAlertView.h"
#include "HLGUIManager.h"
#include "HLTexture.h"
#include "HLWidget.h"
#include "HLTimer.h"
#include "HLGLMatrixStack.h"
#include "HLDrawingPrimitives.h"
#include "HLDirector2D.h"

NS_HL_BEGIN

#define ALERT_MIN_WIDTH 284

#define kHLAlertView_LeftMargin	 10.0f
#define kHLAlertView_TopMargin	 15.0f
#define kHLAlertView_BottomMargin  15.0f
#define kHLAlertView_RowMargin	 5.0f
#define kHLAlertView_ColumnMargin  10.0f

#define kHLAlertView_ButtonHeight 30

namespace gui
{
    bool HLAlertView::lightTheme = true;
    
    void HLAlertView::createViewContent(const char* title, const char* message, const char* cancelButtonTitle, const char* otherButtonTitles, va_list args)
    {
        mBackgroundImage = new HLImageView(this);
        HLTexture* tex = HLTexture::getTexture("HLAlertViewBackground.png");
        mBackgroundImage->setImage(tex, HLRect(0.5, 0.484, 0, 0));
        mTitleLabel = new HLLabel(this);
        mTitleLabel->setFontSize(17);
        mTitleLabel->setText(title);
        if (lightTheme) {
            mTitleLabel->setTextColor(HLBlackColor4B);
        }
        mMessageLabel = new HLLabel(this);
        mMessageLabel->setText(message);
        mTitleLabel->setFontSize(15);
        if (lightTheme) {
            mMessageLabel->setTextColor(HLBlackColor4B);
        }
        if (cancelButtonTitle)
        {
            HLButton* b = new HLButton(this);
            b->setNormalTitle(cancelButtonTitle);
            tex = HLTexture::getTexture("HLAlertViewCancelButtonBackground.png");
            b->setNormalBackgroundImage(tex, HLRect(0.5,0.5,0,0));
            tex = HLTexture::getTexture("HLAlertViewButtonBackground_Highlighted.png");
            b->setHighlightedBackgroundImage(tex, HLRect(0.5,0.5,0,0));
            b->setTag(0);
            b->onButtonClick = newDelegate(this, &HLAlertView::onButtonPress);
            b->setZoomOnTouchDown(false);
            mButtons.push_back(b);
            b->setTitleSize(17);
            if (lightTheme) {
                b->setTitleColor(HLBlackColor4B);
            }
        }
        int index = 1;
        while (otherButtonTitles)
        {
            HLButton* b = new HLButton(this);
            b->setNormalTitle(otherButtonTitles);
            tex = HLTexture::getTexture("HLAlertViewButtonBackground.png");
            b->setNormalBackgroundImage(tex, HLRect(0.5,0.5,0,0));
            tex = HLTexture::getTexture("HLAlertViewButtonBackground_Highlighted.png");
            b->setHighlightedBackgroundImage(tex, HLRect(0.5,0.5,0,0));
            b->setTag(index);
            b->onButtonClick = newDelegate(this, &HLAlertView::onButtonPress);
            b->setZoomOnTouchDown(false);
            mButtons.push_back(b);
            b->setTitleSize(17);
            otherButtonTitles = va_arg(args, const char*);
            index++;
            if (lightTheme) {
                b->setTitleColor(HLBlackColor4B);
            }
        }
    }

    void HLAlertView::createViewContent(char* title, char* message, char* cancelButtonTitle, char* otherButtonTitles[])
    {
        mBackgroundImage = new HLImageView(this);
        HLTexture* tex = HLTexture::getTexture("HLAlertViewBackground.png");
        mBackgroundImage->setImage(tex, HLRect(0.5, 0.484, 0, 0));
        mTitleLabel = new HLLabel(this);
        mTitleLabel->setFontSize(17);
        mTitleLabel->setText(title);
        if (lightTheme) {
            mTitleLabel->setTextColor(HLBlackColor4B);
        }
        mMessageLabel = new HLLabel(this);
        mMessageLabel->setText(message);
        mMessageLabel->setFontSize(15);
        if (lightTheme) {
            mMessageLabel->setTextColor(HLBlackColor4B);
        }
        if (cancelButtonTitle)
        {
            HLButton* b = new HLButton(this);
            b->setNormalTitle(cancelButtonTitle);
            tex = HLTexture::getTexture("HLAlertViewCancelButtonBackground.png");
            b->setNormalBackgroundImage(tex, HLRect(0.5,0.5,0,0));
            tex = HLTexture::getTexture("HLAlertViewButtonBackground_Highlighted.png");
            b->setHighlightedBackgroundImage(tex, HLRect(0.5,0.5,0,0));
            b->setTag(0);
            b->onButtonClick = newDelegate(this, &HLAlertView::onButtonPress);
            b->setZoomOnTouchDown(false);
            mButtons.push_back(b);
            b->setTitleSize(17);
            if (lightTheme) {
                b->setTitleColor(HLBlackColor4B);
            }
        }
        if (otherButtonTitles)
        {
            int index = 0;
            while (otherButtonTitles[index])
            {
                HLButton* b = new HLButton(this);
                b->setNormalTitle(otherButtonTitles[index]);
                tex = HLTexture::getTexture("HLAlertViewButtonBackground.png");
                b->setNormalBackgroundImage(tex, HLRect(0.5,0.5,0,0));
                tex = HLTexture::getTexture("HLAlertViewButtonBackground_Highlighted.png");
                b->setHighlightedBackgroundImage(tex, HLRect(0.5,0.5,0,0));
                b->setTag(index+1);
                b->onButtonClick = newDelegate(this, &HLAlertView::onButtonPress);
                b->setZoomOnTouchDown(false);
                mButtons.push_back(b);
                b->setTitleSize(17);
                index++;
                if (lightTheme) {
                    b->setTitleColor(HLBlackColor4B);
                }
            }
        }
    }
    
    void HLAlertView::createViewContent(HLView* content, HLButton* buttons, va_list args)
    {
        setSize(content->getSize());
        addSubview(content);
        int index = 0;
        while (buttons)
        {
            buttons->setTag(index);
            buttons->onButtonClick = newDelegate(this, &HLAlertView::onButtonPress);
            mButtons.push_back(buttons);
            buttons = va_arg(args, HLButton*);
            index++;
        }
    }
    
    void HLAlertView::createViewContent(HLView* content, HLButton* buttons[])
    {
        setSize(content->getSize());
        addSubview(content);
        int index = 0;
        while (buttons[index])
        {
            buttons[index]->setTag(index);
            buttons[index]->onButtonClick = newDelegate(this, &HLAlertView::onButtonPress);
            mButtons.push_back(buttons[index]);
            index++;
        }
    }
    
    void HLAlertView::setDarkTheme()
    {
        lightTheme = false;
    }
    
    void HLAlertView::layoutSubviews()
    {
        bool stacked = !(mButtonLayout == kHLAlertViewButtonLayoutNormal && mButtons.size() == 2);
        
        float maxWidth = mBounds.size.width - kHLAlertView_LeftMargin * 2;
        mTitleLabel->setSize(maxWidth, 20);
        float titleLabelHeight = mTitleLabel->sizeThatFits().height;
        mMessageLabel->setSize(maxWidth, 100);
        float messageLabelHeight = mMessageLabel->sizeThatFits().height;
        float buttonAreaHeight;
        if (stacked)
        {
            int buttonCount = (int)mButtons.size();
            buttonAreaHeight = kHLAlertView_ButtonHeight * buttonCount + kHLAlertView_RowMargin * (buttonCount - 1);
        }
        else
        {
            buttonAreaHeight = kHLAlertView_ButtonHeight;
        }
        
        float totalHeight = kHLAlertView_TopMargin + titleLabelHeight +
                            kHLAlertView_RowMargin + messageLabelHeight + kHLAlertView_RowMargin +
                            buttonAreaHeight + kHLAlertView_BottomMargin;
        
        if (totalHeight > mBounds.size.height)
        {
            totalHeight -= messageLabelHeight;
            messageLabelHeight = mBounds.size.height - totalHeight;
            // TODO mLabel加scrollview显示
            totalHeight = mBounds.size.height;
        }
        
        if (totalHeight != mBounds.size.height)
        {
            setSize(HLSize(mBounds.size.width, totalHeight));
        }
        
        float y = kHLAlertView_TopMargin;
        if (titleLabelHeight > 0)
        {
            mTitleLabel->setFrame(HLRect(kHLAlertView_LeftMargin, y, mBounds.size.width - kHLAlertView_LeftMargin*2, titleLabelHeight));
            y += titleLabelHeight + kHLAlertView_RowMargin;
        }
        
        if (messageLabelHeight > 0)
        {
            mMessageLabel->setFrame(HLRect(kHLAlertView_LeftMargin, y, mBounds.size.width - kHLAlertView_LeftMargin*2, messageLabelHeight));
            y += messageLabelHeight + kHLAlertView_RowMargin + 5;
        }
        
        float buttonHeight = kHLAlertView_ButtonHeight;
        if (stacked)
        {
            float buttonWidth = mBounds.size.width - (kHLAlertView_LeftMargin * 2);
            std::vector<HLButton*>::iterator iter;
            for (iter = mButtons.begin(); iter != mButtons.end(); ++iter)
            {
                (*iter)->setFrame(HLRect(kHLAlertView_LeftMargin, y, buttonWidth, buttonHeight));
                y += buttonHeight + kHLAlertView_RowMargin;
            }
        }
        else
        {
            int size = (int)mButtons.size();
            float buttonWidth = (mBounds.size.width - kHLAlertView_ColumnMargin*(size-1) - kHLAlertView_LeftMargin*2) / size;
            float x = kHLAlertView_LeftMargin;
            std::vector<HLButton*>::iterator iter;
            for (iter = mButtons.begin(); iter != mButtons.end(); ++iter)
            {
                (*iter)->setFrame(HLRect(x, y, buttonWidth, buttonHeight));
                x += buttonWidth + kHLAlertView_ColumnMargin;
            }
        }
        
        mBackgroundImage->setSize(mBounds.size);
    }
    
    HLAlertView::~HLAlertView()
    {
        if (mTimer)
        {
            mTimer->cancel();
        }
    }
    
    void HLAlertView::show(bool animation)
    {
        HLWidget *widget = new HLWidget();
        widget->addSubview(this);
        color4B color;
        color.r = color.g = color.b = 0;
        color.a = 100;
        widget->setBackgroundColor(color);
        if (mTitleLabel)
        {
            setSize(HLSize(ALERT_MIN_WIDTH, 358));
            layoutSubviews();
        }
        centerInParent();
        HLGUIManager::getSingleton()->getRootWidget()->showAlert(widget);
        if (animation)
        {
            mScale = 0.6;
            mScaleTo = 1.1;
            HLTimer *timer = new HLTimer(0, true);
            timer->delegate = newDelegate(this, &HLAlertView::pulse);
        }
    }
    
    void HLAlertView::dismiss()
    {
        HLGUIManager::getSingleton()->getRootWidget()->dismissAlert((HLWidget*)mParent);
    }
    
    void HLAlertView::onButtonPress(HLButton *sender)
    {
        onButtonClicked(this, sender->getTag());
        if (sender->getTag() == 0)
        {
            onCancel(this);
        }
        HLGUIManager::getSingleton()->getRootWidget()->dismissAlert((HLWidget*)mParent);
    }
    

//    void HLAlertView::close()
//    {
//        //HLTimer *timer = new HLTimer(0);
//        //timer->delegate = newDelegate(this, &HLAlertView::destroySelf); // destroy self in the next run loop
//        delete this;
//    }
    
//    void HLAlertView::destroySelf(HLTimer * timer, float dt)
//    {
//        this->removeFromParent();
//    }
    
    void HLAlertView::visit()
    {
        if (!mVisible)
        {
            return;
        }
        HLGLMatrixStack* matrixStack = HLGLMatrixStack::matrixStack();
        if (mScale != 1)
        {
            matrixStack->pushMatrix();
            matrixStack->multMatrix(getTransform());
            kmMat4 matrix;
            kmMat4Scaling(&matrix, mScale, mScale, 1);
            matrix.mat[12] = -mBounds.size.width * (mScale - 1)/2;
            matrix.mat[13] = -mBounds.size.height * (mScale - 1)/2;
            matrixStack->multMatrix(&matrix);
            kmMat4Inverse(&matrix, getTransform());
            matrixStack->multMatrix(&matrix);
        }
        HLView::visit();
        if (mScale != 1)
        {
            matrixStack->popMatrix();
        }
    }
    
    void HLAlertView::pulse(HLTimer *timer , float dt)
    {
        mTimeElapsed += dt;
        float percent = mTimeElapsed / 0.2 * 1.5;
        if (percent >= 1)
        {
            mScale = 1.1;
            timer->cancel();
            mTimer = new HLTimer(0, true);
            mTimeElapsed = 0;
            mScaleTo = 0.9;
            mTimer->delegate = newDelegate(this, &HLAlertView::pulse1);
        }
        else
        {
            mScale = 0.6 + (1.1 - 0.6)*percent;
        }
    }
    
    void HLAlertView::pulse1(HLTimer *timer , float dt)
    {
        mTimeElapsed += dt;
        float percent = mTimeElapsed / 0.067 * 1.5;
        if (percent >= 1)
        {
            mScale = 0.9;
            timer->cancel();
            mTimer = new HLTimer(0, true);
            mTimeElapsed = 0;
            mScaleTo = 1;
            mTimer->delegate = newDelegate(this, &HLAlertView::pulse2);
        }
        else
        {
            mScale = 1.1 - (1.1 - 0.9)*percent;
        }
    }
    
    void HLAlertView::pulse2(HLTimer *timer , float dt)
    {
        mTimeElapsed += dt;
        float percent = mTimeElapsed / 0.133 * 1.5;
        if (percent >= 1)
        {
            mScale = 1.0;
            timer->cancel();
            mTimer = NULL;
        }
        else
        {
            mScale = 0.9 + (1.0 - 0.9)*percent;
        }
    }
    
    HLView* HLAlertView::clone(HLView *parent)
    {
        return NULL;
    }
}

NS_HL_END
