//
//  HLLabel.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLLabel.h"
#include "HLGLConfiguration.h"
#include "HLProgram.h"

#if USE_JS_SCRIPTING
#include "JSLabel.h"
#endif

#ifndef WIN32
using std::min;
using std::max;
#endif

NS_HL_BEGIN

extern uint16_t* utf16_from_utf8(const char* str_old);
extern int wcslen(const unsigned short* str);

struct Range
{
    size_t location;
    size_t length;
};

namespace gui
{
HLLabel::HLLabel(HLView* parent):HLView(parent)
{
    initMembers();
}

HLLabel::HLLabel(HLView* parent, HLRect frame):HLView(parent, frame)
{
    initMembers();
}

HLLabel::HLLabel(HLView* parent, const HLLabel& copy):HLView(parent, copy)
{
    mTextAlign = copy.mTextAlign;
    mFont = copy.mFont;
    mFontSize = copy.mFontSize;
    mTextColorTL = copy.mTextColorTL;
    mTextColorBL = copy.mTextColorBL;
    mTextColorTR = copy.mTextColorTR;
    mTextColorBR = copy.mTextColorBR;
    mTextStroke = copy.mTextStroke;
    mTextStrokeColor = copy.mTextStrokeColor;
    mBrightness = copy.mBrightness;
    mBold = copy.mBold;
    mUnicodeText = NULL;
    mMultilineFlag = copy.mMultilineFlag;
    setText(copy.mText);
}

void HLLabel::initMembers()
{
    mText = "";
    mTextAlign = kTextAlignmentCenter;
    mTextBoundingBox = HLSizeZero;
    mUnicodeText = NULL;
    mFont = HLFont::getFont();
    mFontSize = 12;

    mTextColorTL.r = mTextColorTL.b = mTextColorTL.g = mTextColorTL.a = 255;
    mTextColorBL = mTextColorTR = mTextColorBR = mTextColorTL;
    mTextStrokeColor.r = mTextStrokeColor.b = mTextStrokeColor.g = 0;
    mTextStrokeColor.a = 1;
    mTextStroke = false;
    mBrightness = 1.f;
    mBold = false;
    mMultilineFlag = true;
}

HLLabel::~HLLabel()
{
#if USE_JS_SCRIPTING
    JSLabel::removeJSObject(this);
#endif
    if (mUnicodeText)
    {
        delete []mUnicodeText;
    }
//#if HL_USE_VAO
//    if (mVAO.size() > 0)
//    {
//        glDeleteVertexArrays((int)mVAO.size(), &mVAO[0]);
//    }
//#endif
//    if (mVBO.size() > 0)
//    {
//        glDeleteBuffers((int)mVBO.size(), &mVBO[0]);
//    }
}
    
void HLLabel::setTextColorTL(color4B color)
{
    if (currAnimations)
    {
        propertyChanged(this, "textColorTL", mTextColorTL, color);
        return;
    }
    mTextColorTL = color; mNeedsLayout = true;
}
    
void HLLabel::setTextColorTR(color4B color)
{
    if (currAnimations)
    {
        propertyChanged(this, "textColorTR", mTextColorTR, color);
        return;
    }
    mTextColorTR = color; mNeedsLayout = true;
}
    
void HLLabel::setTextColorBL(color4B color)
{
    if (currAnimations)
    {
        propertyChanged(this, "textColorBL", mTextColorBL, color);
        return;
    }
    mTextColorBL = color; mNeedsLayout = true;
}
    
void HLLabel::setTextColorBR(color4B color)
{
    if (currAnimations)
    {
        propertyChanged(this, "textColorBR", mTextColorBR, color);
        return;
    }
    mTextColorBR = color; mNeedsLayout = true;
}

void HLLabel::setText(int textNum)
{
    char textChar[20];
    sprintf(textChar, "%d", textNum);
    setText(textChar);
}

void HLLabel::setText(std::string text)
{
    if (mText == text)
        return;

    mText = text;
    
    if (mUnicodeText)
    {
        delete [] mUnicodeText;
    }
    mUnicodeText = utf16_from_utf8(text.c_str());
    mFont->addText(mUnicodeText);
    mNeedsLayout = true;
}

void HLLabel::onInternalEvent(const char* event)
{
    if (!strcmp(event, "size_changed") || !strcmp(event, "frame_changed") || !strcmp(event, "edgeinsets_changed"))
    {
        mNeedsLayout = true;
    }
}

void HLLabel::resizeHeightToFits()
{

}

HLSize HLLabel::sizeThatFits()
{
    if (mNeedsLayout)
        layoutText();
    return mTextBoundingBox;
}

Range getWordRange(const uint16_t* s, Range searchRange);
float getWordWidthWithFont(const uint16_t*s, size_t len, HLFont *font, int fontSize);
    
void HLLabel::startNewLine(float &x, float &y, const float &rel_size, std::vector<int> &linePos)
{
    const float size = mFont->getCharSize() * rel_size;
    int lineStart = linePos.back();
    linePos.push_back((int)mQuads.size());
    int lineEnd = linePos.back();
    mTextBoundingBox.height += size + 2;
    mTextBoundingBox.width = max(mTextBoundingBox.width, x);
    float lineWidth = x;
    // adjust the horizontal alignment the of last line
    int xoffset = 0;
    if (mTextAlign & kTextHAlignmentCenter)
    {
        xoffset = (mBounds.size.width - lineWidth) / 2;
    }
    else if (mTextAlign & kTextHAlignmentRight)
    {
        xoffset = mBounds.size.width - lineWidth;
    }
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
    
void HLLabel::genTextQuad(size_t index, size_t length, float &x, float &y, const float &rel_size, std::vector<int> &linePos, const int &textLen)
{
    for (size_t i = index; i < index+length; ++i)
    {
        std::pair<int32_t, uint32_t> const & offset_adv = mFont->getCharIndexAdvance(mUnicodeText[i]);
        float advance = (offset_adv.second&0xFFFF)*rel_size;
        
        if (offset_adv.first != -1)
        {
            HLFont::font_info const & ci = mFont->getCharFontInfo(offset_adv.first);
            
            const float left = ci.left * rel_size;
            const float top = ci.top * rel_size;
            const float size = mFont->getCharSize() * rel_size;
            HLRect fontPos(x+left, y-top, size, size);
            
            HLRect texRect;
            const HLFont::CharInfo & charInfo = mFont->getCharInfo(mUnicodeText[i]);
            texRect.origin = charInfo.rc;//mCharInfoMap.find(unicode[i])->second.rc;
            texRect.origin.x /= 512.f;
            texRect.origin.y /= 512.f;
            texRect.size.width = static_cast<float>(mFont->getCharSize())/512.f;
            texRect.size.height = static_cast<float>(mFont->getCharSize())/512.f;
            
            V3F_C4B_T2F_Quad quad;
            
            quad.tl.vertices.x = fontPos.origin.x;
            quad.tl.vertices.y = (fontPos.origin.y + fontPos.size.height);
            quad.tl.vertices.z = 0;
            quad.tl.texCoords.u = texRect.origin.x;
            quad.tl.texCoords.v = texRect.origin.y;
            quad.tl.colors = mTextColorTL;
            
            quad.bl.vertices.x = fontPos.origin.x;
            quad.bl.vertices.y = fontPos.origin.y;
            quad.bl.vertices.z = 0;
            quad.bl.texCoords.u = texRect.origin.x;
            quad.bl.texCoords.v = (texRect.origin.y + texRect.size.height);
            quad.bl.colors = mTextColorBL;
            
            quad.tr.vertices.x = (fontPos.origin.x + fontPos.size.width);
            quad.tr.vertices.y = (fontPos.origin.y + fontPos.size.height);
            quad.tr.vertices.z = 0;
            quad.tr.texCoords.u = (texRect.origin.x + texRect.size.width);
            quad.tr.texCoords.v = texRect.origin.y;
            quad.tr.colors = mTextColorTR;
            
            quad.br.vertices.x = (fontPos.origin.x + fontPos.size.width);
            quad.br.vertices.y = fontPos.origin.y;
            quad.br.vertices.z = 0;
            quad.br.texCoords.u = (texRect.origin.x + texRect.size.width);
            quad.br.texCoords.v = (texRect.origin.y + texRect.size.height);
            quad.br.colors = mTextColorBR;
            
            std::map<GLuint, std::vector<GLushort> >::iterator indiceIter = mIndices.find(charInfo.texture);
            if (indiceIter == mIndices.end())
            {
                std::vector<GLushort> v1;
                mIndices.insert(std::make_pair(charInfo.texture, v1));
                indiceIter = mIndices.find(charInfo.texture);
            }
            const size_t i4 = mQuads.size()*4;
            indiceIter->second.push_back(i4);
            indiceIter->second.push_back(i4+1);
            indiceIter->second.push_back(i4+2);
            indiceIter->second.push_back(i4+3);
            indiceIter->second.push_back(i4+2);
            indiceIter->second.push_back(i4+1);
            
            mQuads.push_back(quad);
        }
        x += advance;
        if (i == textLen-1)
        {
            mTextEndX = x;
        }
    }
}

void HLLabel::layoutText()
{
    mTextBoundingBox = HLSize(0, 2);
    if (mBounds.size == HLSizeZero)
    {
        return;
    }
    if (mText.length() == 0)
    {
        return;
    }
    if (!mUnicodeText)
    {
        return;
    }
    
    mNeedsLayout = false;
    
    int len = wcslen(mUnicodeText);
    
    mQuads.clear();
    mQuads.reserve(len);
    mIndices.clear();
    std::vector<int> linePos;

    float const rel_size = static_cast<float>(mFontSize) / mFont->getCharSize();
    float x = 0;
    float y = mBounds.size.height - mFontSize - 2;

    linePos.push_back(0);

    mTextBoundingBox.height += mFontSize;
    
    float currentLineWidth = 0;

    for (size_t i = 0; i < len; )
    {
        Range searchRange = {i, len - i};
        Range wordRange = getWordRange(mUnicodeText, searchRange);
        
        float wordWidth = getWordWidthWithFont(mUnicodeText+wordRange.location, wordRange.length, mFont, mFontSize);
        
        if (wordWidth > mBounds.size.width)
        {
            for (int j = 0; j < wordRange.length; ++j)
            {
                float letterWidth = getWordWidthWithFont(mUnicodeText+wordRange.location+j, 1, mFont, mFontSize);
                if (currentLineWidth + letterWidth > mBounds.size.width && mMultilineFlag) // start a new line
                {
                    startNewLine(x, y, rel_size, linePos);
                    currentLineWidth = 0;
                }
                currentLineWidth += letterWidth;
                genTextQuad(wordRange.location+j, 1, x, y, rel_size, linePos, len);
                ++i;
            }
        }
        else
        {
            if (currentLineWidth + wordWidth > mBounds.size.width && mMultilineFlag) // start a new line
            {
                startNewLine(x, y, rel_size, linePos);
                currentLineWidth = 0;
            }
            currentLineWidth += wordWidth;
            genTextQuad(wordRange.location, wordRange.length, x, y, rel_size, linePos, len);
            i = wordRange.location + wordRange.length;
        }
        
        if (mMultilineFlag && (mUnicodeText[wordRange.location+wordRange.length-1] == 10)) // start a new line
        {
            startNewLine(x, y, rel_size, linePos);
            currentLineWidth = 0;
        }
    }


    mTextBoundingBox.width = max(mTextBoundingBox.width, x);

    int lineStart = linePos.back();
    len = (int)mQuads.size();

    // adjust the horizontal alignment the of the last line
    int xoffset = 0;
    if (mTextAlign & kTextHAlignmentCenter)
    {
        xoffset = (mBounds.size.width - x) / 2;
    }
    else if (mTextAlign & kTextHAlignmentRight)
    {
        xoffset = mBounds.size.width - x;
    }
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

    // adjust the vertical alignment
    float yoffset = 0;
    if (mTextAlign & kTextVAlignmentCenter)
    {
        yoffset = (mBounds.size.height - mTextBoundingBox.height)/2;
    }
    else if (mTextAlign & kTextVAlignmentBottom)
    {
        yoffset = mBounds.size.height - mTextBoundingBox.height;
    }
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
}

void HLLabel::onDraw()
{
    if (mNeedsLayout)
        layoutText();
    
    if (mText.length() == 0 || mBounds.size == HLSizeZero || mQuads.empty())
    {
        HLView::onDraw();
        return;
    }

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
        else if (!strcmp(uniform.name, kUniformThreshold))
        {
            uniform.setUniform1f(mFont->getRenderThresHold());
        }
    }

    program->setUniformForModelViewProjectionMatrix();
    hlglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
#define kQuadSize sizeof(mQuads[0].bl)
    // vertices
    glEnableVertexAttribArray(kVertexAttrib_Position);
    glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) ((uint8_t*)&mQuads[0] + offsetof( V3F_C4B_T2F, vertices)));

    // colors
    glEnableVertexAttribArray(kVertexAttrib_Color);
    glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) ((uint8_t*)&mQuads[0] + offsetof( V3F_C4B_T2F, colors)));

    // tex coords
    glEnableVertexAttribArray(kVertexAttrib_TexCoords);
    glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) ((uint8_t*)&mQuads[0] + offsetof( V3F_C4B_T2F, texCoords)));
    
    std::map<GLuint, std::vector<GLushort> >::iterator itr;
    for (itr = mIndices.begin(); itr != mIndices.end(); ++itr)
    {
        hlglBindTexture(GL_TEXTURE_2D, itr->first);
        glDrawElements(GL_TRIANGLES, (GLsizei)itr->second.size(), GL_UNSIGNED_SHORT, &(itr->second[0]));
    }

    CHECK_GL_ERROR();

    HLView::onDraw();
}

#if USE_JS_SCRIPTING
JSCppWrapperBase* HLLabel::getOrCreateJSWrapper(JSContext* cx)
{
    return JSLabel::getOrCreateWrapper(cx, this);
}
#endif

HLView* HLLabel::clone(HLView* parent)
{
    HLLabel* view = new HLLabel(parent, *this);
    std::list<HLView*>::iterator itr = mSubViews.begin();
    for (; itr != mSubViews.end(); ++itr)
    {
        (*itr)->clone(view);
    }
    return view;
}
}

NS_HL_END
