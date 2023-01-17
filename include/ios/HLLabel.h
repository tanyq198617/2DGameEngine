//
//  HLLabel.h
//  HoolaiEngine
//
//  Created by zhao shuan on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLLABEL_H__
#define __HLLABEL_H__

#include "HLMacros.h"
#include "HLView.h"
#include "GameEngineConfig.h"
#include "HLFont.h"

NS_HL_BEGIN

namespace gui
{
    typedef enum {
        kTextHAlignmentLeft = 1,
        kTextHAlignmentCenter = 1<<1,
        kTextHAlignmentRight = 1<<2,
        kTextVAlignmentTop = 1<<3,
        kTextVAlignmentCenter = 1<<4,
        kTextVAlignmentBottom = 1<<5,
        kTextAlignmentCenter = kTextHAlignmentCenter | kTextVAlignmentCenter
    } HLTextAlignment;
    
    class HLLabel : public HLView
    {
    public:
        HLLabel(HLView* parent);
        HLLabel(HLView* parent, HLRect frame);
        HLLabel(HLView* parent, const HLLabel& copy);
        virtual ~HLLabel();
        
        void setText(int text);
        void setText(std::string text);
        std::string getText() {return mText;}
        void setFontSize(int size) {mFontSize = size; mNeedsLayout = true;}
        int getFontSize() {return mFontSize;}
        void setBrightness(float brightness) { mBrightness = brightness; }
        float getBrightness() {return mBrightness;}
        void setTextColor(color4B color) { setTextColor(color.r, color.g, color.b, color.a);}
        void setTextColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
        {
            if (currAnimations)
            {
                color4B color;
                color.r = r; color.g = g; color.b = b; color.a = a;
                propertyChanged(this, "textColor", mTextColorTL, color);
                return;
            }
            mTextColorTL.r = r; mTextColorTL.g = g; mTextColorTL.b = b; mTextColorTL.a = a; mTextColorBL = mTextColorBR = mTextColorTR = mTextColorTL; mNeedsLayout = true;
        }
        void setTextColorTL(color4B color);
        void setTextColorTR(color4B color);
        void setTextColorBL(color4B color);
        void setTextColorBR(color4B color);
        color4B getTextColorTL() {return mTextColorTL;}
        color4B getTextColorTR() {return mTextColorTR;}
        color4B getTextColorBL() {return mTextColorBL;}
        color4B getTextColorBR() {return mTextColorBR;}
        void setTextStroke(bool stroke) { mTextStroke = stroke; }
        bool isTextStroke() {return mTextStroke;}
        void setTextStrokeColor(color4B color) {setTextStrokeColor(color.r, color.g, color.b, color.a);}
        void setTextStrokeColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
        {
            if (currAnimations)
            {
                color4B color;
                color.r = r; color.g = g; color.b = b; color.a = a;
                color4B originStrokeColor;
                originStrokeColor.r = mTextStrokeColor.r*255;
                originStrokeColor.g = mTextStrokeColor.g*255;
                originStrokeColor.b = mTextStrokeColor.b*255;
                originStrokeColor.a = mTextStrokeColor.a*255;
                propertyChanged(this, "textStrokeColor", originStrokeColor, color);
                return;
            }
            mTextStrokeColor.r = r/255.f; mTextStrokeColor.g = g/255.f; mTextStrokeColor.b = b/255.f; mTextStrokeColor.a = a/255.f;
        }
        color4B getTextStrokeColor() {color4B strokeColor; strokeColor.r = mTextStrokeColor.r*255; strokeColor.g = mTextStrokeColor.g*255; strokeColor.b = mTextStrokeColor.b*255; strokeColor.a = mTextStrokeColor.a*255; return strokeColor;}
        void setTextAlignment(int align) {mTextAlign = align; mNeedsLayout = true;}
        int getTextAlignment() {return mTextAlign;}
        void setIsFontBold(bool bold) {mBold = bold;}
        bool isFontBold() {return mBold;}
        void resizeHeightToFits();
        HLSize sizeThatFits();
        void setFont(HLFont* font) {HLASSERT(font != NULL, "setFont NULL"); mFont = font; if(mText.length() > 0) setText(mText);}
        void setMultiline(bool b) {mMultilineFlag = b; mNeedsLayout = true;}
        virtual void onDraw();
        float getTextEndX() {return mTextEndX; };
#if USE_JS_SCRIPTING
        virtual JSCppWrapperBase* getOrCreateJSWrapper(JSContext* cx);
#endif
        
        virtual HLView* clone(HLView* parent);
        
    protected:
        virtual void onInternalEvent(const char* event);
    private:
        void initMembers();        
        void startNewLine(float &x, float &y, const float &rel_size, std::vector<int> &linePos);
        void genTextQuad(size_t index, size_t length, float &x, float &y, const float &rel_size, std::vector<int> &linePos, const int &textLen);
        void layoutText();
//#if HL_USE_VAO
//        void setupVBOandVAO();
//#else
//        void setupVBO();
//#endif
    private:
        int mTextAlign;
        float mBrightness;
        std::string mText;
        int mFontSize;
        color4B mTextColorTL;
        color4B mTextColorBL;
        color4B mTextColorTR;
        color4B mTextColorBR;
        HLSize mTextBoundingBox;
        uint16_t * mUnicodeText;
        HLFont* mFont;
        std::vector<V3F_C4B_T2F_Quad> mQuads;
        std::map<GLuint, std::vector<GLushort> > mIndices;
        bool mTextStroke;
        bool mBold;
        color4F mTextStrokeColor;
        bool mMultilineFlag;
        bool mNeedsLayout;
        float mTextEndX;
//#if HL_USE_VAO
//        std::vector<GLuint> mVAO;
//#endif
//        std::vector<GLuint> mVBO;
    };
}

NS_HL_END

#endif
