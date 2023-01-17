//
//  HLSmartLabel.cpp
//  HoolaiEngine
//
//  Created by liumingliang on 13-2-20.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLSmartLabel.h"
#include "HLProgram.h"
#include "HLGLConfiguration.h"
#include "HLStringUtil.h"
//#include "FontStyle.h"
NS_HL_BEGIN
using namespace hoolai;

extern uint16_t* utf16_from_utf8(const char* str_old);
extern int wcslen(const unsigned short* str);

using namespace hoolai;
using namespace std;

#ifdef WIN32
using std::max;
using std::min;
#endif

namespace gui
{
//    Font_Style::Font_()
//    {
//        
//    }
    
HLSmartLabel::HLSmartLabel(HLView* parent):HLView(parent)
{
    initMembers();
}
HLSmartLabel::HLSmartLabel(HLView* parent, HLRect frame):HLView(parent, frame)
{
    initMembers();
}
void HLSmartLabel::initMembers()
{
    mText = "";
//        mTextAlign = kTextAlignmentCenter;
    mTextBoundingBox = HLSizeZero;
    mUnicodeText = NULL;
    mFont = HLFont::getFont();
    mFontSize = 12;
#if HL_USE_VAO
    mVAO = -1;
#endif
    mVBO[0] = -1;
    mTextColorTL.r = mTextColorTL.b = mTextColorTL.g = mTextColorTL.a = 255;
    mTextColorBL = mTextColorTR = mTextColorBR = mTextColorTL;
    mTextStrokeColor.r = mTextStrokeColor.b = mTextStrokeColor.g = 0;
    mTextStrokeColor.a = 255;
    mTextStroke = false;
    mBrightness = 1.f;
    mBold = false;
    
    HLTexture * texs[5];
    for (int i=0; i<5; i++) {
        char mChar[10];
        sprintf(mChar, "h%d.png",i);
         hoolai::HLTexture *tex = hoolai::HLTexture::getTexture(mChar);
        tex->retain();
        texs[i]=tex;

    }
    setImage(texs);
    
//    string fTest="[0,3,2332,3,5,33336,7,80000],[0,3,2,3,5,6,7,8],[0,3,2,3,5,6,7,8]";
//    std::vector<int> vs;
//    StringUtil::SplitInt(vs,fTest,',');
    
    int index=0;
    int count=0;

//    if (fTest.find("],")!=-1) {
//        do  {
//            index=(int)fTest.find("],",index);
//            fTest.find(<#const char *__s#>, <#size_type __pos#>, <#size_type __n#>)
//            
//            fTest.find
//            count++;
//        }while (index!=std::string::npos) ;
//    }
  
    
    setText("<span color='#ff0000' size='12' action='test?a=1'>fffff与强者挑战是提高实力和磨练自己的最好办法。</span><span color='#00ff00' size='12' action='test?a=1'>与弱者切\e磋会让你的实力下降。</span><span color='#0000ff' size='12' action='test?a=1'>哦哦哦哦哦哦哦。</span>");

    
}
    

string HLSmartLabel::analyseText(string text)
{
    string appendString="";
    
    FontStyle* fontSytle=NULL;
    
    int findFontSytleBeginIndex=-1;
    int len =0;

    for (int i=0; i<text.size(); i++) {
        //find font sytle
//        FontStyle fontSytle;
        if (fontSytle==NULL) {
            fontSytle=new FontStyle();
        }
        int endCharIndex = isStringIncludeFontStyle(text,i,*fontSytle);
        if (endCharIndex>0)
        {
           string str= cutStringFromStringRange(text,i,endCharIndex);
            if (str.size()>0)
            {
             

                //add index to fontStyle
                string str2=getStringAndRemoveFontStyle(str,len,*fontSytle);
                if (str2.size()>0)
                {
//                    int startIndex=i;
//                    int
                    recordFontTypeToIndex(*fontSytle);
                    
                    i=endCharIndex-1;//-1 because of i++ 
                    appendString.append(str2.c_str());
                    
                    if (appendString.size()>0) {
                        uint16_t * unicodeText = utf16_from_utf8(appendString.c_str());
                        len = wcslen(unicodeText);
                    }
//                    addSubString(&appendString,&str2);
                    fontSytle=NULL;
                }
            }
        }
        
    }
    
    
    return appendString;
}
//    
int HLSmartLabel::isStringIncludeFontStyle(string _string,int index,FontStyle& fontSytle)
{
    int endIndex=-1;
    if(_string[index]=='<')
    {
        bool isFindFontStyleHead=false;
        int findFontSytleHeadBeginIndex=0;
        int findFontSytleHeadEndIndex=0;
//        int colroSytle=0;
//        int sizeSytle=16;
        
        for (int j=index+1; j<_string.size(); j++) {


            if (isFindFontStyleHead==false) {
                if (_string[j]==' ') {
                    continue;
                }else if(_string[j]=='s'&&_string[j+1]=='p'&&_string[j+2]=='a'&&_string[j+3]=='n'){
                    findFontSytleHeadBeginIndex=j;
//                    findFontSytleHeadEndIndex=
                
                    continue;
                }else if(_string[j]=='>'){
                    findFontSytleHeadEndIndex=j;
                    isFindFontStyleHead=true;
                }
            }else{
                if(_string[j-6]=='<'&&_string[j-5]=='/'&&_string[j-4]=='s'&&_string[j-3]=='p'&&_string[j-2]=='a'&&_string[j-1]=='n'&&_string[j]=='>'){
                    endIndex=j+1;
                    
//                   int indexColor=(int) _string.find("color='", findFontSytleHeadBeginIndex, findFontSytleHeadEndIndex-findFontSytleHeadBeginIndex);
//                    int indexSize=(int) _string.find("size='", findFontSytleHeadBeginIndex, findFontSytleHeadEndIndex-findFontSytleHeadBeginIndex);
                    
//                    string sff="Hello";
//                    
//                    cout<<(sff.find("ell")!=-1?"":"not ")<<"found\n";
//                    cout<<(sff.find('e')!=-1?"":"not ")<<"found\n";
                    
                           
                    string colorChar="color='";
                    string sizeChar="size='";
//                  int indexTest=  (int)_string.find("color='", index, 4);
                    string subString="";
                    subString.insert(0, _string, index, endIndex-index);

                    int indexColor2=(int) subString.find(colorChar.c_str())+(int)colorChar.size();
                    int indexSize2=(int) subString.find(sizeChar.c_str())+(int)sizeChar.size();
                    int indexSize3=(int)subString.find_first_of("'", indexSize2,10);
                                    
//                    _string.find
                    string strColor="";
                    strColor.insert(0, subString, indexColor2+1,6);//FFFFFF
                    string strSize="";
                    strSize.insert(0,subString, indexSize2,indexSize3-indexSize2);
//                    str.insert(0, _string, beginIndex, endIndex-beginIndex);

//                    string fontSizeString;
                 
                    fontSytle.size=atoi(strSize.c_str());
//                    sprintf(chdd, "%d",at);
                    fontSytle.colorR=(getHexToInt(strColor,0)<<4)+getHexToInt(strColor,1);
                    fontSytle.colorG=(getHexToInt(strColor,2)<<4)+getHexToInt(strColor,3);
                    fontSytle.colorB=(getHexToInt(strColor,4)<<4)+getHexToInt(strColor,5);
                    char mChar[20];
                    sprintf(mChar,"%d,%d",(fontSytle.colorR&0xff)<<16|(fontSytle.colorG&0xff)<<8|(fontSytle.colorB&0xff), fontSytle.size);
                    fontSytle.key=mChar;
                    
                    return endIndex;
                }else{
                    
                }
            }
          
        }
        
    }
    
    return -1;
}
    
int HLSmartLabel::getHexToInt(string strColor,int index)
{
    int t=0;

        for (int i=0; i<strColor.size(); i++) {
            if(strColor[i]>='a'&&strColor[i]<='f')
            {
                t=strColor[i]-'a'+10;
            }
            else{
                t=strColor[i]-'0';
            }
            if (i==index) {
                return t;
            }
        }
        return t;
}
    
string HLSmartLabel::cutStringFromStringRange(string _string,int beginIndex,int endIndex)
{
    string str="";
//    char * charConst = NULL;
    
    str.insert(0, _string, beginIndex, endIndex-beginIndex);
//    _string.find(str.c_str(), beginIndex, endIndex-1);
    
  //  str=charConst;
    
    return str;
}

//not support repeated fontStyle 
string HLSmartLabel::getStringAndRemoveFontStyle(string _string,int indexStart,FontStyle& fontStyle)
{
    string str="";
    int beginIndex=0;
    int endIndex=0;
    bool isStarBegin=true;
//    endIndex=length-beginIndex-strlen("</span>");

       for (int i=0; i<_string.size(); i++) {
           if (isStarBegin) {
               if (_string[i]=='>')//default first is font end >
               {
                   beginIndex=i+1;
                   isStarBegin=false;
               }
           }else{
               if (_string[i-1]=='<'&&_string[i]=='/'&&_string[i+1]=='s'&&_string[i+2]=='p'&&_string[i+3]=='a'&&_string[i+4]=='n'&&_string[i+5]=='>')
               {
                   endIndex=i-1;
                   break;
               }
           }
          
        
           
       }
    string text=  str.insert(0, _string, beginIndex, endIndex-beginIndex);

    uint16_t * unicodeText = utf16_from_utf8(text.c_str());
    int len = wcslen(unicodeText);

    vector<int>& Indexs=fontStyle.mIndex;
    for (int i=0; i<len; i++) {
        Indexs.push_back(i+indexStart);
    }
    
      return str;
}
string HLSmartLabel::recordFontTypeToIndex(FontStyle& fontStyle )
{
//    int fontColor;
//    int fontSize;
//    FontStyle style;
    
    //check it to find repeated style
    //mFontStorage
    bool hasSameKey=false;
    int index=-1;
    for (int i=0; i<mFontStorage.size(); i++) {
        if (!strcmp(fontStyle.key.c_str(), mFontStorage[i].key.c_str())) {
            hasSameKey=true;
            index=i;
        }
    }
    
    if (hasSameKey) {
        vector<int> &vectorIndexs= mFontStorage[index].mIndex;
        vector<int> &vectorIndexs2= fontStyle.mIndex;

        for (int i=0; i<vectorIndexs.size(); i++) {
            
            for (int j=0; j<vectorIndexs2.size(); j++) {
                if(vectorIndexs[i]==vectorIndexs2[j])
                {
                    continue;
                }else{
                    vectorIndexs.push_back(vectorIndexs2[j]);

                }
            }
           
        }
       
        
    }else{
        mFontStorage.push_back(fontStyle);
    }
    return "";
}
    
string HLSmartLabel::addSubString(string* stringMain,string* _string)
{
//        string1
    stringMain->append(_string->c_str());

//    stringMain.append(_string);
	return "";
}
    
void HLSmartLabel::setText(std::string _text)
{
    //cover input text to display text. record each char to fontStyle;
   string text= analyseText(_text);
    
    if (mText == text)
        return;
    mText = text;
    if (mUnicodeText)
    {
        delete mUnicodeText;
    }
    mUnicodeText = utf16_from_utf8(text.c_str());
    mFont->addText(mUnicodeText);
//    int len = wcslen(mUnicodeText);

    mTextColorTL.r = 255;
    mTextColorTL.b = 255;
    mTextColorTL.g = 255;
    mTextColorTL.a = 255;
    
   layoutText();
}
    
void HLSmartLabel::setImage(HLTexture* texture[])
{
    for (int i=0; i<5; i++) {
        mTexture[i]=texture[i];
    }
}
    
//xiaolv?
color4B HLSmartLabel::findColor(int charIndex)
{
    color4B color;
    color.a=255;
    color.r=255;
    color.g=255;
    color.b=255;
//    if (charIndex>25) {
//        printf("%d \n",charIndex);
//
//    }
//    printf("%d \n",charIndex);
    for( int i=0;i<mFontStorage.size();i++)
    {
      vector<int>& mIndexs=  mFontStorage[i].mIndex;
        for (int j=0; j<mIndexs.size(); j++) {
            if (mIndexs[j]==charIndex) {
                if (i==1) {
                    printf("%d %d %d\n",mFontStorage[i].colorR,mFontStorage[i].colorG,mFontStorage[i].colorB);
                    
                }
                color4B color;
                color.a=255;
                color.r=(mFontStorage[i].colorR&0xff);
                color.g=((mFontStorage[i].colorG&0xff));
                color.b=((mFontStorage[i].colorB&0xff));

                return color;
            }
        }
    }
    
    return color;
}
    
void HLSmartLabel::layoutText()
{
        if (mBounds.size == HLSizeZero)
        {
            return;
        }
        if (!mUnicodeText)
        {
            return;
        }
        if (mUnicodeText[0] == 0)
        {
            return;
        }
        int len = wcslen(mUnicodeText);
        mQuads.clear();
        mQuads.reserve(len);
        std::vector<int> linePos;
        mTextBoundingBox = HLSizeZero;
        
        int const maxTexSize = HLGLConfiguration::getSingleton()->getMaxTextureSize();
        float const rel_size = static_cast<float>(mFontSize) / mFont->getCharSize();
        float x = 0;
        float y = mBounds.size.height - mFontSize - 2;
        
        linePos.push_back(0);
        
        mTextBoundingBox.height = mFontSize;
        
        for (int i = 0; i < len; ++i)
        {
            std::pair<int32_t, uint32_t> const & offset_adv = mFont->getCharIndexAdvance(mUnicodeText[i]);
            float advance = (offset_adv.second&0xFFFF)*rel_size;
            if (mUnicodeText[i] == 10) // 换行 '\n'==10
            {
                const float size = mFont->getCharSize() * rel_size;
                int lineStart = linePos.back();
                linePos.push_back((int)mQuads.size());
                int lineEnd = linePos.back();
                mTextBoundingBox.height += size + 2;
                mTextBoundingBox.width = max(mTextBoundingBox.width, x);
                float lineWidth = x;
                // 调整上一行的水平对齐
                int xoffset = 0;
//                if (mTextAlign & kTextHAlignmentCenter)
//                {
//                    xoffset = (mBounds.size.width - lineWidth) / 2;
//                }
//                else if (mTextAlign & kTextHAlignmentRight)
//                {
//                    xoffset = mBounds.size.width - lineWidth;
//                }
                if (xoffset != 0)
                {
                    for (int j = lineStart; j < lineEnd; ++j)
                    {
                        mQuads[j].tl.vertices.x += xoffset;
                        mQuads[j].bl.vertices.x += xoffset;
                        mQuads[j].tr.vertices.x += xoffset;
                        mQuads[j].br.vertices.x += xoffset;
                    }
                }
                x = 0;
                y -= (size + 2);
            }
            if (offset_adv.first != -1)
            {
                HLFont::font_info const & ci = mFont->getCharFontInfo(offset_adv.first);
                
                const float left = ci.left * rel_size;
                const float top = ci.top * rel_size;
                const float size = mFont->getCharSize() * rel_size;
                if (x + left + advance > mBounds.size.width && i + 1 < len) // 换行
                {
                    int lineStart = linePos.back();
                    linePos.push_back((int)mQuads.size());
                    int lineEnd = linePos.back();
                    mTextBoundingBox.height += size + 2;
                    mTextBoundingBox.width = max(mTextBoundingBox.width, x);
                    float lineWidth = x;
                    // 调整上一行的水平对齐
                    int xoffset = 0;
//                    if (mTextAlign & kTextHAlignmentCenter)
//                    {
//                        xoffset = (mBounds.size.width - lineWidth) / 2;
//                    }
//                    else if (mTextAlign & kTextHAlignmentRight)
//                    {
//                        xoffset = mBounds.size.width - lineWidth;
//                    }
                    if (xoffset != 0)
                    {
                        for (int j = lineStart; j < lineEnd; ++j)
                        {
                            mQuads[j].tl.vertices.x += xoffset;
                            mQuads[j].bl.vertices.x += xoffset;
                            mQuads[j].tr.vertices.x += xoffset;
                            mQuads[j].br.vertices.x += xoffset;
                        }
                    }
                    x = 0;
                    y -= (size + 2);
                }
                HLRect fontPos(x+left, y-top, size, size);
                
                HLRect texRect;
                texRect.origin = mFont->getCharInfo(mUnicodeText[i]).rc;//mCharInfoMap.find(unicode[i])->second.rc;
                texRect.origin.x /= maxTexSize;
                texRect.origin.y /= mFont->getTextureSize().height;
                texRect.size.width = static_cast<float>(mFont->getCharSize())/maxTexSize;
                texRect.size.height = static_cast<float>(mFont->getCharSize())/mFont->getTextureSize().height;
               color4B colorFind=findColor(i);
                
                V3F_C4B_T2F_Quad quad;
                
                quad.tl.vertices.x = fontPos.origin.x;
                quad.tl.vertices.y = (fontPos.origin.y + fontPos.size.height);
                quad.tl.vertices.z = 0;
                quad.tl.texCoords.u = texRect.origin.x;
                quad.tl.texCoords.v = texRect.origin.y;
                quad.tl.colors = colorFind;//mTextColorTL;
                
                quad.bl.vertices.x = fontPos.origin.x;
                quad.bl.vertices.y = fontPos.origin.y;
                quad.bl.vertices.z = 0;
                quad.bl.texCoords.u = texRect.origin.x;
                quad.bl.texCoords.v = (texRect.origin.y + texRect.size.height);
                quad.bl.colors = colorFind;//mTextColorBL;
                
                quad.tr.vertices.x = (fontPos.origin.x + fontPos.size.width);
                quad.tr.vertices.y = (fontPos.origin.y + fontPos.size.height);
                quad.tr.vertices.z = 0;
                quad.tr.texCoords.u = (texRect.origin.x + texRect.size.width);
                quad.tr.texCoords.v = texRect.origin.y;
                quad.tr.colors = colorFind;//mTextColorTR;
                
                quad.br.vertices.x = (fontPos.origin.x + fontPos.size.width);
                quad.br.vertices.y = fontPos.origin.y;
                quad.br.vertices.z = 0;
                quad.br.texCoords.u = (texRect.origin.x + texRect.size.width);
                quad.br.texCoords.v = (texRect.origin.y + texRect.size.height);
                quad.br.colors = colorFind;//mTextColorBR;
                
                mQuads.push_back(quad);
                
            }
            x += advance;
        }
        
        int lineStart = linePos.back();
        len = (int)mQuads.size();
        
        // 调整最后一行的水平对齐
        int xoffset = 0;
//        if (mTextAlign & kTextHAlignmentCenter)
//        {
//            xoffset = (mBounds.size.width - x) / 2;
//        }
//        else if (mTextAlign & kTextHAlignmentRight)
//        {
//            xoffset = mBounds.size.width - x;
//        }
        if (xoffset != 0)
        {
            for (int j = lineStart; j < len; ++j)
            {
                mQuads[j].tl.vertices.x += xoffset;
                mQuads[j].bl.vertices.x += xoffset;
                mQuads[j].tr.vertices.x += xoffset;
                mQuads[j].br.vertices.x += xoffset;
            }
        }
        
        // 调整垂直对齐
        float yoffset = 0;
//        if (mTextAlign & kTextVAlignmentCenter)
//        {
//            yoffset = (mBounds.size.height - mTextBoundingBox.height)/2;
//        }
//        else if (mTextAlign & kTextVAlignmentBottom)
//        {
//            yoffset = mBounds.size.height - mTextBoundingBox.height;
//        }
        if (yoffset != 0)
        {
            for (int i = 0; i < len; ++i)
            {
                mQuads[i].tl.vertices.y -= yoffset;
                mQuads[i].bl.vertices.y -= yoffset;
                mQuads[i].tr.vertices.y -= yoffset;
                mQuads[i].br.vertices.y -= yoffset;
            }
        }
        
        mIndices.clear();
        mIndices.resize(len*6);
        for (int i = 0; i < len; i++)
        {
            const unsigned int i6 = i*6;
            const unsigned int i4 = i*4;
            mIndices[i6+0] = (GLushort) i4+0;
            mIndices[i6+1] = (GLushort) i4+1;
            mIndices[i6+2] = (GLushort) i4+2;
            
            mIndices[i6+5] = (GLushort) i4+1;
            mIndices[i6+4] = (GLushort) i4+2;
            mIndices[i6+3] = (GLushort) i4+3;
        }
#if HL_USE_VAO
        setupVBOandVAO();
#else
        setupVBO();
#endif
}
    
void HLSmartLabel::setupVBOandVAO()
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    
    const int size = (int)mQuads.size();
    
#define kQuadSize sizeof(mQuads[0].bl)
    
    glGenBuffers(2, &mVBO[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0]) * size, &mQuads[0], GL_DYNAMIC_DRAW);
    
    // vertices
    glEnableVertexAttribArray(kVertexAttrib_Position);
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, vertices));
    
    // colors
    glEnableVertexAttribArray(kVertexAttrib_Color);
    glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, colors));
    
    // tex coords
    glEnableVertexAttribArray(kVertexAttrib_TexCoords);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, texCoords));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices[0]) * size * 6, &mIndices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR();

}
    int countSmartLabel =0;
void HLSmartLabel::onDraw()
{
    HLPoint point=  HLPoint(0, 0);
    drawAtPoint(point);
    drawText();

//    ++countSmartLabel;
//    std::cout<<countSmartLabel<<std::endl;
}
void HLSmartLabel::drawText()
    {
        if (mVBO[0] == -1 || mText.length() == 0 || mBounds.size == HLSizeZero)
        {
            return;
        }
        
        mFont->bindTexture();
        HLProgram *program = HLProgramManager::currentManager()->getProgram(mTextStroke?kShaderPositionTextureA8DistOutline:kShaderPositionTextureA8Dist);
        program->use();
        
        std::vector<HLProgramUniform>& uniforms = program->getUniforms();
        for (std::vector<HLProgramUniform>::iterator it = uniforms.begin(); it != uniforms.end(); ++it)
        {
            HLProgramUniform& uniform = *it;
            if (!strcmp(uniform.name, kUniformBrightness))
            {
                uniform.setUniform1f(mBrightness);
            }
            else if (!strcmp(uniform.name, kUniformBase))
            {
                uniform.setUniform1f(mFont->getDistBase() / 32768.0f);
            }
            else if (!strcmp(uniform.name, kUniformScale))
            {
                uniform.setUniform1f(mFont->getDistScale() / 32768.0f + 1);
            }
            else if (!strcmp(uniform.name, kUniformBorderOffset))
            {
                uniform.setUniform1f(mBold?0.01:0);
            }
            else if (!strcmp(uniform.name, kUniformOutlineColor))
            {
                uniform.setUniform4fv((GLfloat*)&mTextStrokeColor, 1);
            }
        }
        
        program->setUniformForModelViewProjectionMatrix();
        hlglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
#if HL_USE_VAO
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)mQuads.size()*6, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray( 0 );
#endif
        
        CHECK_GL_ERROR();
}
    
void HLSmartLabel::drawAtPoint(HLPoint& point)
{
    GLubyte* colors=new GLubyte[16];
    memset(colors, 255, sizeof(GLubyte)*16);
    HLProgram* program=HLProgramManager::currentManager()->getProgram(kShaderPositionTextureColor);
    program->use();
    program->setUniformForModelViewProjectionMatrix();//every time??
    
    glUniform1f(glGetUniformLocation(program->getProgram(), "brightness"), 1);
    
    hlglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
       
    GLfloat coordinates[8];
    coordinates[0]=0;
    coordinates[1]=1;
    coordinates[2]=1;
    coordinates[3]=1;
    coordinates[4]=0;
    coordinates[5]=0;
    coordinates[6]=1;
    coordinates[7]=0;

    int textureWidth= mTexture[0]->getImageSize().width;
    
    int textureHeight= mTexture[0]->getImageSize().height;
    for (int j=0; j<10; j++) {

    for (int i=0; i<10; i++) {
    int index=i*j+j;
    index=index>4?4:index;
//        index=0;
    hlglBindTexture(GL_TEXTURE_2D,mTexture[index]->getName());

    HLPoint point2=HLPoint(point.x+i*textureWidth,point.y+j*textureHeight);

    GLfloat vertices[]={
        point2.x,point2.y,//1
        point2.x+textureWidth,point2.y,//2
        point2.x,point2.y+textureHeight,//3
        point2.x+textureWidth,point2.y+textureHeight,//4
    };
//        map<indexTotal,textureName>;
//        map<indexTotal,verticesXY>;

    //图片纹理绘制需要参数：总个数，textureNameIndex，verticesXY，（提升绑定绘制效率进行相同纹理进行排序）
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, coordinates);
    glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    }
    }
    
    delete colors;

    CHECK_GL_ERROR();
    
}
    
}

NS_HL_END
