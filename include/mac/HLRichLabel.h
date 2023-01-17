//
//  HLRichLabel.h
//  HoolaiEngine
//
//  Created by tyq on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLRICHLABEL_H__
#define __HLRICHLABEL_H__

#include "HLMacros.h"
#include "HLView.h"
#include "HLFont.h"
#include "HLDelegate.h"
#include "HLTimer.h"

NS_HL_BEGIN

class HLStyledNode;

namespace gui
{
    class HLRichLabel;
    
    typedef CDelegate3<HLRichLabel*, std::string /*action*/, std::map<std::string, std::string /*params*/> > HLRichLabelDelegate;
    typedef CDelegate1<HLRichLabel*> HLRichLabelAnimDelegate;
    
    class HLRichLabel:public HLView
    {
    public:
        HLRichLabel(HLView* parent):HLView(parent) {initMembers();}
        HLRichLabel(HLView* parent, HLRect frame):HLView(parent, frame) {initMembers();}
        
        HLRichLabelDelegate delegate;
        
        virtual ~HLRichLabel();
        
        void setText(std::string text);
        std::string getText() {return mText;}
        
        void setFontSize(int size) {mFontSize = size;layoutText();}
        int getFontSize() {return mFontSize;}

        void setTextColor(color4B color) {mTextColor = color; mNeedsLayout = true;}
        color4B getTextColor() {return mTextColor;}
        
        void setLinkTextColor(color4B color) {mLinkTextColor = color; mNeedsLayout = true;}
        color4B getLinkTextColor() {return mLinkTextColor;}
        
        void setLinkUnderline(bool flag) {mUnderlineLink = flag;}
        bool getLinkUnderLine() {return mUnderlineLink;}
        
        void setTextStroke(bool stroke) { mTextStroke = stroke; }
        bool isTextStroke() {return mTextStroke;}
        void setTextStrokeColor(color4B color) {setTextStrokeColor(color.r, color.g, color.b, color.a);}
        void setTextStrokeColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
        {
//            if (currAnimations)
//            {
//                color4B color;
//                color.r = r; color.g = g; color.b = b; color.a = a;
//                color4B originStrokeColor;
//                originStrokeColor.r = mTextStrokeColor.r*255;
//                originStrokeColor.g = mTextStrokeColor.g*255;
//                originStrokeColor.b = mTextStrokeColor.b*255;
//                originStrokeColor.a = mTextStrokeColor.a*255;
//                propertyChanged(this, "textStrokeColor", originStrokeColor, color);
//                return;
//            }
            mTextStrokeColor.r = r/255.f; mTextStrokeColor.g = g/255.f; mTextStrokeColor.b = b/255.f; mTextStrokeColor.a = a/255.f;
        }
        color4B getTextStrokeColor() {color4B strokeColor; strokeColor.r = mTextStrokeColor.r*255; strokeColor.g = mTextStrokeColor.g*255; strokeColor.b = mTextStrokeColor.b*255; strokeColor.a = mTextStrokeColor.a*255; return strokeColor;}
        
        HLSize sizeThatFits();
        
        
        void setTextAlignment(int align) {mTextAlign = align; mNeedsLayout = true;}
        int getTextAlignment() {return mTextAlign;}
        
//        virtual HLView* hitTest(HLPoint p);
        virtual void onTouches(HLTouchEvent* touch);
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
        virtual void onMouseEvent(HLMouseEvent* mouseEvent);
#endif
        
        void setFont(HLFont* font) {HLASSERT(font != NULL, "setFont NULL"); mFont = font; mNeedsLayout = true;}
        
        virtual void onDraw();
        
        void startShowAnimation(float speed = 2, IDelegate1<HLRichLabel*>* delegate = NULL);
        void stopShowAnimation();
        void setAlpha(float alpha);
        bool isInAnimation() {return mAnimTimer;}
        HLRichLabelAnimDelegate animDelegate;
        
#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
        
    protected:
        virtual void onInternalEvent(const char* event);
        
    private:
        struct elementstyle
        {
            bool bold;
            bool underline;
            HLFont* font;
            bool isLink;
            std::string linkurl;
            color4B textColor;
            bool colored;
            int size;
        };
        
        void layout(HLStyledNode* node, struct elementstyle style);
        void layoutElement(HLStyledNode* node, struct elementstyle style);
        void layoutText(HLStyledNode* node, struct elementstyle style);
        void layoutImage(HLStyledNode* node, struct elementstyle style);
        void startLink(HLStyledNode* node, struct elementstyle style);
        void endLink(HLStyledNode* node, struct elementstyle style);
        void startUnderLine(HLStyledNode* node, struct elementstyle style);
        void endUnderLine(HLStyledNode* node, struct elementstyle style);
        void handleURL(std::string url);
        void genTextQuadAndIndeces(uint16_t* text, int length, struct elementstyle style);
        void breakLine();
        
        float mLinePadding;
        
        void initMembers();
        struct eventrect* getEventRect(HLPoint p);
        void layoutText();
//        void setupVBOandVAO();
        int mTextAlign;
        float mBrightness;
        HLSize mTextBoundingBox;
        std::vector<V3F_C4B_T2F_Quad> mQuads;
        std::map<HLFont*, std::map<GLuint, std::vector<GLushort> > > mIndices;
        std::map<HLFont*, std::map<GLuint, std::vector<GLushort> > > mBoldIndices; // 粗体单独渲染
//        std::vector<V3F_C4B_T2F_Quad> mImageQuads;
        std::map<GLuint, std::vector<GLushort> > mImageIndices;
        
        struct underline
        {
            int index;
            color4B color;
            std::vector<HLPoint> lines;
        };
        std::vector<struct underline> mUnderlines; // 下划线
        
        struct eventrect
        {
            int index;
            HLRect frame;
            std::string url;
        };
        std::vector<struct eventrect> mEventrects; // 超链点击区域
        
        struct imgtag
        {
            int quadIndex;
            int currIndex;
            HLPoint pos;
            HLSize size;
            std::vector<HLSpriteFrame*> images;
        };
        std::vector<struct imgtag> mImages;
        HLTimer* mImageAnimationTimer;
        void imageUpdate(HLTimer*, float);
        
        int findRectContainsPoint(HLPoint p);
        HLRect mHightlightRect;
        bool mNeedsLayout;
        
        std::string mText;
        color4B mTextColor;
        color4B mLinkTextColor;
        color4F mTextStrokeColor;
        bool mTextStroke;
        HLFont* mFont;
        int mFontSize;
        
//        std::vector<GLuint> mVAO;
//        std::vector<GLuint> mVBO;
        
        HLPoint mCurrentPos;
        int mCurrentLinePos;
        int mCurrentLineWidth;
        int mCurrentLineHeight;
        
        HLTimer* mAnimTimer;
        float mAnimSpeed;
        float mAnimPos;
        void animUpdate(HLTimer*, float);
        
        bool mUnderlineLink;
    };
}

NS_HL_END

#endif
