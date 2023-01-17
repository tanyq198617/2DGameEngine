//
//  HLSmartLabel.h
//  HoolaiEngine
//
//  Created by liumingliang on 13-2-20.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HoolaiEngine__HLSmartLabel__
#define __HoolaiEngine__HLSmartLabel__

#include "HLTexture.h"
#include "HLView.h"
#include "HLFont.h"

NS_HL_BEGIN

namespace gui
{
class FontStyle{
    public:
    std::string key;
        int colorR;
        int colorG;
        int colorB;

        int size;
    std::vector<int> mIndex;
};
    

    
class HLSmartLabel :public HLView
{
public:
    HLSmartLabel(HLView* parent);
    HLSmartLabel(HLView * parent, HLRect frame);
    void initMembers();

    HLTexture* mTexture[5];
    
    void setImage(HLTexture* texture[]);
    
    std::string analyseText(std::string text);

    void setText(std::string text);

    void setupVBOandVAO();
   
    void drawAtPoint(HLPoint& point);
    
    virtual void onDraw();
    
    void layoutText();
    
    int isStringIncludeFontStyle(std::string _string,int i,FontStyle& fontSytle);
   
    int getHexToInt(std::string strColor,int index);
    //int getHexToInt(std::string strColor);

    std::string cutStringFromStringRange(std::string _string,int beginIndex,int endIndex);
    
//    std::string getStringAndRemoveFontStyle(std::string _string,FontStyle& fontStyle);
    std::string getStringAndRemoveFontStyle(std::string _string,int indexStart,FontStyle& fontStyle);

    std::string recordFontTypeToIndex(FontStyle& fontStyle );
    
    std::string addSubString(std::string* stringMain,std::string* _string);
    
    std::vector<FontStyle>mFontStorage;
//    int findColor(int charIndex);
    color4B findColor(int charIndex);

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
//    std::vector<V3F_C4B_T2F_Quad> mQuadsImageMap;
    std::map<std::string,std::vector<V3F_C4B_T2F_Quad> > mQuadsImageMap;
    std::vector<GLushort> mIndices;
    
    
    bool mTextStroke;
    bool mBold;
    color4F mTextStrokeColor;
    #if HL_USE_VAO
    GLuint mVAO;
    #endif
    GLuint mVBO[2];
    void drawText();

};
}
NS_HL_END

#endif /* defined(__HoolaiEngine__HLSmartLabel__) */
