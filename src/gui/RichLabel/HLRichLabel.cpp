//
//  HLRichLabel.cpp
//  HoolaiEngine
//
//  Created by tyq on 1/20/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include "HLRichLabel.h"
#include "HLLabel.h"
#include "HLStyledTextParser.h"
#include "HLStyledNode.h"
#include "HLStringUtil.h"
#include "HLProgram.h"
#include "HLDrawingPrimitives.h"
#include "HLTexture.h"
#include "HLSpriteFrame.h"

#if USE_JS_SCRIPTING
#include "JSRichLabel.h"
#endif

NS_HL_BEGIN

void openURL(std::string url);

struct Range
{
    size_t location;
    size_t length;
};

extern uint16_t* utf16_from_utf8(const char* str_old);
extern int wcslen(const unsigned short* str);

namespace gui
{
    Range getWordRange(const uint16_t* s, Range searchRange)
    {
        Range res;
        res.location = searchRange.location;
        res.length = 1;
        uint16_t c = s[searchRange.location];
        if (c > 255 && !(c >= 0x0400 && c <= 0x052F))
        {
            return res;
        }
        int length = wcslen(s);
        for (size_t i = searchRange.location; i < length && i < searchRange.location + searchRange.length; ++i)
        {
            c = s[i];
            if (c > 255 && !(c >= 0x0400 && c <= 0x052F))
            {
                res.length = i - searchRange.location;
                return res;
            }
            else if (c == ' ' || c == '\n' || c == '\t')
            {
                res.length = i + 1 - searchRange.location;
                return res;
            }
        }
        res.length = searchRange.length;
        return res;
    }
    
    float getWordWidthWithFont(const uint16_t*s, size_t len, HLFont *font, int fontSize)
    {
        float const rel_size = static_cast<float>(fontSize) / font->getCharSize();
        
        float width = 0;
        for (int i = 0; i < len; ++i)
        {
            std::pair<int32_t, uint32_t> const & offset_adv = font->getCharIndexAdvance(s[i]);
            float advance = (offset_adv.second&0xFFFF)*rel_size;
            width += advance;
        }
        return width;
    }
    
    HLRichLabel::~HLRichLabel()
    {
        if (mAnimTimer)
        {
            mAnimTimer->cancel();
        }
        for (std::vector<struct imgtag>::iterator itr = mImages.begin(); itr != mImages.end(); ++itr)
        {
            for (std::vector<HLSpriteFrame*>::iterator it = (*itr).images.begin(); it != (*itr).images.end(); ++it)
            {
                (*it)->release();
            }
        }
        
#if USE_JS_SCRIPTING
        JSRichLabel::removeJSObject(this);
#endif
//        if (mVAO.size() > 0)
//        {
//            glDeleteVertexArrays((int)mVAO.size(), &mVAO[0]);
//        }
//        if (mVBO.size() > 0)
//        {
//            glDeleteBuffers((int)mVBO.size(), &mVBO[0]);
//        }
    }
    
    void HLRichLabel::initMembers()
    {
        mTextAlign = kTextAlignmentCenter;
        mFont = HLFont::getFont();
        mFontSize = 12;
        mTextStroke = false;
        mBrightness = 1.f;
        
        mLinePadding = 2;
        
        mTextColor.r = mTextColor.b = mTextColor.g = mTextColor.a = 255;
        mTextStrokeColor.r = mTextStrokeColor.b = mTextStrokeColor.g = 0;
        mTextStrokeColor.a = 1;
        
        mLinkTextColor = mTextColor;
        
        mImageAnimationTimer = NULL;
        mAnimTimer = NULL;
        mAnimSpeed = 2;
        mAnimPos = 0;
        
        mUnderlineLink = true;
    }
    
    void HLRichLabel::onInternalEvent(const char* event)
    {
        if (!strcmp(event, "size_changed") || !strcmp(event, "frame_changed") || !strcmp(event, "edgeinsets_changed"))
        {
            mNeedsLayout = true;
        }
    }
    
    void HLRichLabel::setText(std::string text)
    {
        if (mText != text)
        {
            mText = text;
//            layoutText();
            mNeedsLayout = true;
        }
    }
    
    HLSize HLRichLabel::sizeThatFits()
    {
        if (mNeedsLayout)
        {
            layoutText();
        }
        return mTextBoundingBox;
    }
    
    struct eventrect* HLRichLabel::getEventRect(HLPoint p)
    {
        return NULL;
    }
    
    void HLRichLabel::layoutText()
    {
        if (mAnimTimer)
        {
            mAnimTimer->cancel();
            mAnimTimer = NULL;
        }
        
        if (mImageAnimationTimer)
        {
            mImageAnimationTimer->cancel();
            mImageAnimationTimer = NULL;
        }
        
        if (mText.length()==0)
        {
            return;
        }
        
        if (mBounds.size.width == 0)
        {
            return;
        }
        
        HLStyledTextParser parser;
        parser.parseXHTML(mText);
        HLStyledNode* rootnode = parser.getRootNode();
        
        mQuads.clear();
        mIndices.clear();
        mBoldIndices.clear();
        mUnderlines.clear();
        mImageIndices.clear();
        mEventrects.clear();
        for (std::vector<struct imgtag>::iterator itr = mImages.begin(); itr != mImages.end(); ++itr)
        {
            for (std::vector<HLSpriteFrame*>::iterator it = (*itr).images.begin(); it != (*itr).images.end(); ++it)
            {
                (*it)->release();
            }
        }
        mImages.clear();
        
        mCurrentPos.x = 0;
        mCurrentPos.y = mBounds.size.height - mFontSize - mLinePadding;
        mCurrentLinePos = 0;
        mCurrentLineWidth = 0;
        mCurrentLineHeight = 0;
        mTextBoundingBox = HLSizeZero;
        
//        HLStyledNode* node = rootnode;
        
//        while (node)
//        {
//            
        struct elementstyle style;
        style.bold = false;
        style.underline = false;
        style.font = mFont;
        style.isLink = false;
        style.textColor = mTextColor;
        style.colored = false;
        style.size = mFontSize;
        layout(rootnode, style);
//            if (!strcmp(node->getType(), "TEXT"))
//            {
//                layoutText(node, style);
//            }
//            else
//            {
//                layoutElement(node, style);
//            }
//            node = node->nextSibling;
//        }
        breakLine();
        
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
            int len = (int)mQuads.size();
            for (int i = 0; i < len; ++i)
            {
                mQuads[i].tl.vertices.y -= yoffset;
                mQuads[i].bl.vertices.y -= yoffset;
                mQuads[i].tr.vertices.y -= yoffset;
                mQuads[i].br.vertices.y -= yoffset;
            }
            
            len = (int)mUnderlines.size();
            for (int i = 0; i < len; ++i)
            {
                mUnderlines[i].lines[0].y -= yoffset;
                mUnderlines[i].lines[1].y -= yoffset;
            }
            
            len = (int)mEventrects.size();
            for (int i = 0; i < len; ++i)
            {
                mEventrects[i].frame.origin.y -= yoffset;
            }
            
            len = (int)mImages.size();
            for (int i = 0; i < len; ++i)
            {
                mImages[i].pos.y -= yoffset;
            }
        }
        
        delete rootnode;
        
        mNeedsLayout = false;
        
        mImageAnimationTimer = new HLTimer(0.2, true);
        mImageAnimationTimer->delegate = newDelegate(this, &HLRichLabel::imageUpdate);
        imageUpdate(NULL, 0);
        
//        setupVBOandVAO();
    }
    
    void HLRichLabel::layout(HLStyledNode* node, struct elementstyle style)
    {
        while (node)
        {
            if (!strcmp(node->getType(), "TEXT"))
            {
                layoutText(node, style);
            }
            else if (!strcmp(node->getType(), "IMG"))
            {
                layoutImage(node, style);
            }
            else
            {
                layoutElement(node, style);
            }
            node = node->nextSibling;
        }
    }
    
    void HLRichLabel::layoutElement(HLStyledNode* node, struct elementstyle style)
    {
        if (!strcmp(node->getType(), "LINK"))
        {
            style.isLink = true;
            style.linkurl = static_cast<HLStyledLinkNode*>(node)->href;
            if (!style.colored)
            {
                style.textColor = mLinkTextColor;
            }
        }
        else if (!strcmp(node->getType(), "FONT"))
        {
            HLStyledFontNode* fontnode = static_cast<HLStyledFontNode*>(node);
            if (fontnode->face.length())
            {
                HLFont* font = HLFont::getFont(fontnode->face.c_str());
                if (font)
                {
                    style.font = font;
                }
            }
            if (fontnode->size)
            {
                style.size = fontnode->size;
            }
            if (fontnode->color.r >= 0)
            {
                style.textColor.r = fontnode->color.r*255;
                style.textColor.g = fontnode->color.g*255;
                style.textColor.b = fontnode->color.b*255;
                style.textColor.a = fontnode->color.a*255;
                style.colored = true;
            }
        }
        else if (!strcmp(node->getType(), "BOLD"))
        {
            style.bold = true;
        }
        else if (!strcmp(node->getType(), "UNDERLINE"))
        {
            style.underline = true;
        }
        else if (!strcmp(node->getType(), "LINEBREAK"))
        {
            breakLine();
        }
        
        if (!strcmp(node->getType(), "LINK"))
        {
            startLink(node, style);
        }
        
        if (node->firstChild)
        {
            layout(node->firstChild, style);
        }
        
        if (!strcmp(node->getType(), "LINK"))
        {
            endLink(node, style);
        }
    }
    
    void HLRichLabel::layoutText(HLStyledNode* node, struct elementstyle style)
    {
        uint16_t* text = utf16_from_utf8(static_cast<HLStyledTextNode*>(node)->text.c_str());
        style.font->addText(text);
        int length = wcslen(text);
        
        size_t index = 0;
//        int lineStartIndex = 0;
//        float frameWidth = 0;
        mCurrentLineHeight = MAX(mCurrentLineHeight, style.size);
        
        startUnderLine(node, style);

        while (index < length)
        {
            Range searchRange;
            searchRange.location = index;
            searchRange.length = length - index;
            Range wordRange = getWordRange(text, searchRange);
            
            float wordWidth = getWordWidthWithFont(text+wordRange.location, wordRange.length, style.font, style.size);
            if (wordWidth > mBounds.size.width)
            {
                for (int i = 0; i < wordRange.length; ++i)
                {
                    float letterWidth = getWordWidthWithFont(text+wordRange.location+i, 1, style.font, style.size);
                    
                    if (mCurrentLineWidth + letterWidth > mBounds.size.width)
                    {
                        endLink(node, style);
                        endUnderLine(node, style);
                        breakLine();
                        startLink(node, style);
                        startUnderLine(node, style);
                        mCurrentLineHeight = MAX(mCurrentLineHeight, style.size);
                    }
                    mCurrentLineWidth += letterWidth;
                    genTextQuadAndIndeces(text+wordRange.location+i, 1, style);
                    ++index;
                }
            }
            else
            {
                if (mCurrentLineWidth + wordWidth > mBounds.size.width)
                {
                    endLink(node, style);
                    endUnderLine(node, style);
                    breakLine();
                    startLink(node, style);
                    startUnderLine(node, style);
                    mCurrentLineHeight = MAX(mCurrentLineHeight, style.size);
                }
                mCurrentLineWidth += wordWidth;
                genTextQuadAndIndeces(text+wordRange.location, (int)wordRange.length, style);
                index = wordRange.location + wordRange.length;
            }
        }
        endUnderLine(node, style);
        
        delete [] text;
    }
    
    void HLRichLabel::layoutImage(HLStyledNode* node, struct elementstyle style)
    {
        HLStyledImageNode* imgNode = static_cast<HLStyledImageNode*>(node);
        
        std::string::size_type pos = imgNode->src.find(":");
        
        struct imgtag imagestruct;
        imagestruct.currIndex = 0;
        
        int i4 = imagestruct.quadIndex = (int)mQuads.size();
        i4 *= 4;
        
        if (pos != std::string::npos)
        {
            std::string atlas = imgNode->src.substr(0, pos);
            std::string imagesstr = imgNode->src.substr(pos+1);
            std::vector<std::string> images;
            StringUtil::Split(images, imagesstr, ',');
            for (std::vector<std::string>::iterator itr = images.begin(); itr != images.end(); ++itr)
            {
                HLSpriteFrame* spriteFrame = HLSpriteFrame::getSpriteFrame(atlas.c_str(), (*itr).c_str());
                spriteFrame->retain();
                imagestruct.images.push_back(spriteFrame);
            }
        }
        else
        {
            std::vector<std::string> images;
            StringUtil::Split(images, imgNode->src, ',');
            for (std::vector<std::string>::iterator itr = images.begin(); itr != images.end(); ++itr)
            {
                HLTexture* tex = HLTexture::getTexture((*itr).c_str());
                tex->retain();
                HLSpriteFrame* spriteFrame = new HLSpriteFrame();
                spriteFrame->_texture = tex;
                spriteFrame->_rotated = false;
                spriteFrame->_sourceSize = tex->getImageSize()*tex->getScale();
                spriteFrame->_textureRect.origin = HLPointZero;
                spriteFrame->_textureRect.size = spriteFrame->_sourceSize;
                imagestruct.images.push_back(spriteFrame);
            }
        }
        
        if (!imagestruct.images.empty())
        {
            HLSize imgSize = imgNode->size;
            if (imgSize == HLSizeZero)
            {
                imgSize = imagestruct.images[0]->_sourceSize / imagestruct.images[0]->_texture->getScale();
            }
            imagestruct.size = imgSize;
            
            if (mCurrentLineWidth + imgSize.width > mBounds.size.width)
            {
                endLink(node, style);
                breakLine();
                startLink(node, style);
            }
            mCurrentLineHeight = MAX(mCurrentLineHeight, imgSize.height);
            mCurrentLineWidth += imgSize.width;
            imagestruct.pos = mCurrentPos;
            mCurrentPos.x += imgSize.width;
            
            std::map<GLuint, std::vector<GLushort> >::iterator itr;
            itr = mImageIndices.find(imagestruct.images[0]->_texture->getName());
            if (itr == mImageIndices.end())
            {
                std::vector<GLushort> v;
                itr = mImageIndices.insert(std::make_pair(imagestruct.images[0]->_texture->getName(), v)).first;
            }
            itr->second.push_back(i4);
            itr->second.push_back(i4+1);
            itr->second.push_back(i4+2);
            itr->second.push_back(i4+3);
            itr->second.push_back(i4+2);
            itr->second.push_back(i4+1);
            
            V3F_C4B_T2F_Quad quad;
            quad.tl.colors = HLWhiteColor4B;
            quad.bl.colors = HLWhiteColor4B;
            quad.tr.colors = HLWhiteColor4B;
            quad.br.colors = HLWhiteColor4B;
            mQuads.push_back(quad);
            
            mImages.push_back(imagestruct);
        }
    }
    
    void HLRichLabel::startLink(HLStyledNode* node, struct elementstyle style)
    {
        if (!style.isLink)
        {
            return;
        }
//        struct underline tmp;
//        tmp.index = (int)mQuads.size();
//        tmp.color = style.textColor;
//        tmp.lines.push_back(mCurrentPos);
//        mUnderlines.push_back(tmp);
        
        if (style.linkurl.length())
        {
            struct eventrect er;
            er.index = (int)mQuads.size();
            er.frame.origin = mCurrentPos;
            er.frame.size.height = style.size;
            er.url = style.linkurl;
            mEventrects.push_back(er);
        }
    }
    
    void HLRichLabel::endLink(HLStyledNode* node, struct elementstyle style)
    {
        if (!style.isLink)
        {
            return;
        }
//        mUnderlines.back().lines.push_back(HLPoint(mCurrentPos.x, mCurrentPos.y));
        if (style.linkurl.length())
        {
            mEventrects.back().frame.size.width = mCurrentPos.x - mEventrects.back().frame.origin.x;
        }
    }
    
    void HLRichLabel::startUnderLine(HLStyledNode* node, struct elementstyle style)
    {
        if (!(style.isLink && mUnderlineLink) && !style.underline)
        {
            return;
        }
        struct underline tmp;
        tmp.index = (int)mQuads.size();
        tmp.color = style.textColor;
        tmp.lines.push_back(mCurrentPos);
        mUnderlines.push_back(tmp);
    }
    
    void HLRichLabel::endUnderLine(HLStyledNode* node, struct elementstyle style)
    {
        if (!(style.isLink && mUnderlineLink) && !style.underline)
        {
            return;
        }
        mUnderlines.back().lines.push_back(HLPoint(mCurrentPos.x, mCurrentPos.y));
    }
    
    void HLRichLabel::imageUpdate(HLTimer* timer, float)
    {
        bool needAnimation = false;
        bool needUpdateTexture = false;
        
        for (std::vector<struct imgtag>::iterator itr = mImages.begin(); itr != mImages.end(); ++itr)
        {
            struct imgtag &image = (*itr);
            
            if (!timer || image.images.size() > 1)
            {
                V3F_C4B_T2F_Quad &quad = mQuads[image.quadIndex];
                HLSpriteFrame *spriteFrame = image.images[image.currIndex];
                
                HLSize imageSize = spriteFrame->_texture->getImageSize() * spriteFrame->_texture->getScale();
                
                HLSize drawSize = spriteFrame->_textureRect.size;
                if (spriteFrame->_rotated)
                {
                    float tmp = drawSize.width;
                    drawSize.width = drawSize.height;
                    drawSize.height = tmp;
                }
                
                float left = spriteFrame->_textureRect.origin.x/imageSize.width;
                float right = left + drawSize.width/imageSize.width;
                float top = spriteFrame->_textureRect.origin.y/imageSize.height;
                float bottom = top + drawSize.height/imageSize.height;
                
                if (spriteFrame->_rotated)
                {
                    quad.bl.texCoords.u = left;
                    quad.bl.texCoords.v = top;
                    quad.br.texCoords.u = left;
                    quad.br.texCoords.v = bottom;
                    quad.tl.texCoords.u = right;
                    quad.tl.texCoords.v = top;
                    quad.tr.texCoords.u = right;
                    quad.tr.texCoords.v = bottom;
                }
                else
                {
                    quad.bl.texCoords.u = left;
                    quad.bl.texCoords.v = bottom;
                    quad.br.texCoords.u = right;
                    quad.br.texCoords.v = bottom;
                    quad.tl.texCoords.u = left;
                    quad.tl.texCoords.v = top;
                    quad.tr.texCoords.u = right;
                    quad.tr.texCoords.v = top;
                }
                
                drawSize = spriteFrame->_textureRect.size;
                
                HLPoint offset = HLPoint(spriteFrame->_offset.x, spriteFrame->_sourceSize.height-spriteFrame->_textureRect.size.height-spriteFrame->_offset.y);
                HLSize size = image.size;
                offset.x = offset.x*(size.width/spriteFrame->_sourceSize.width);
                offset.y = offset.y*(size.height/spriteFrame->_sourceSize.height);
                drawSize.width *= (size.width/spriteFrame->_sourceSize.width);
                drawSize.height *= (size.height/spriteFrame->_sourceSize.height);
                
                offset += image.pos;
                
                quad.bl.vertices.x = offset.x;
                quad.bl.vertices.y = offset.y;
                quad.bl.vertices.z = 0;
                quad.br.vertices.x = offset.x+drawSize.width;
                quad.br.vertices.y = offset.y;
                quad.br.vertices.z = 0;
                quad.tl.vertices.x = offset.x;
                quad.tl.vertices.y = offset.y+drawSize.height;
                quad.tl.vertices.z = 0;
                quad.tr.vertices.x = offset.x+drawSize.width;
                quad.tr.vertices.y = offset.y+drawSize.height;
                quad.tr.vertices.z = 0;
            }
            
            if (image.images.size() > 1)
            {
                needAnimation = true;
                int preIndex = image.currIndex - 1;
                if (preIndex < 0)
                {
                    preIndex = (int)image.images.size() - 1;
                }
                if (timer && image.images[image.currIndex]->_texture != image.images[preIndex]->_texture)
                {
                    needUpdateTexture = true;
                }
                ++image.currIndex;
                if (image.currIndex >= image.images.size())
                {
                    image.currIndex = 0;
                }
            }
        }
        
        if (!needAnimation)
        {
            mImageAnimationTimer->cancel();
            mImageAnimationTimer = NULL;
            return;
        }
        
        if (needUpdateTexture)
        {
            mImageIndices.clear();
            
            for (std::vector<struct imgtag>::iterator itr = mImages.begin(); itr != mImages.end(); ++itr)
            {
                struct imgtag &imagestruct = *itr;
                int i4 = imagestruct.quadIndex * 4;
                std::map<GLuint, std::vector<GLushort> >::iterator it;
                int preIndex = imagestruct.currIndex - 1;
                if (preIndex < 0)
                {
                    preIndex = (int)imagestruct.images.size() - 1;
                }
                it = mImageIndices.find(imagestruct.images[preIndex]->_texture->getName());
                if (it == mImageIndices.end())
                {
                    std::vector<GLushort> v;
                    it = mImageIndices.insert(std::make_pair(imagestruct.images[preIndex]->_texture->getName(), v)).first;
                }
                it->second.push_back(i4);
                it->second.push_back(i4+1);
                it->second.push_back(i4+2);
                it->second.push_back(i4+3);
                it->second.push_back(i4+2);
                it->second.push_back(i4+1);
            }
        }
    }
    
    void HLRichLabel::genTextQuadAndIndeces(uint16_t* text, int length, struct elementstyle style)
    {
        float const rel_size = static_cast<float>(style.size) / style.font->getCharSize();
        
        for (int i = 0; i < length; ++i)
        {
            std::pair<int32_t, uint32_t> const & offset_adv = style.font->getCharIndexAdvance(text[i]);
            float advance = (offset_adv.second&0xFFFF)*rel_size;
            if (offset_adv.first != -1)
            {
                HLFont::font_info const & ci = mFont->getCharFontInfo(offset_adv.first);
                
                const float left = ci.left * rel_size;
                const float top = ci.top * rel_size;
                const float size = style.size;
                
                HLRect fontPos(mCurrentPos.x + left, mCurrentPos.y - top, size, size);
                
                HLRect texRect;
                const HLFont::CharInfo & charInfo = style.font->getCharInfo(text[i]);
                texRect.origin = charInfo.rc;//mCharInfoMap.find(unicode[i])->second.rc;
                texRect.origin.x /= 512;
                texRect.origin.y /= 512;
                texRect.size.width = static_cast<float>(style.font->getCharSize())/512;
                texRect.size.height = static_cast<float>(style.font->getCharSize())/512;
                
                V3F_C4B_T2F_Quad quad;
                
                quad.tl.vertices.x = fontPos.origin.x;
                quad.tl.vertices.y = (fontPos.origin.y + fontPos.size.height);
                quad.tl.vertices.z = 0;
                quad.tl.texCoords.u = texRect.origin.x;
                quad.tl.texCoords.v = texRect.origin.y;
                quad.tl.colors = style.textColor;
                
                quad.bl.vertices.x = fontPos.origin.x;
                quad.bl.vertices.y = fontPos.origin.y;
                quad.bl.vertices.z = 0;
                quad.bl.texCoords.u = texRect.origin.x;
                quad.bl.texCoords.v = (texRect.origin.y + texRect.size.height);
                quad.bl.colors = style.textColor;
                
                quad.tr.vertices.x = (fontPos.origin.x + fontPos.size.width);
                quad.tr.vertices.y = (fontPos.origin.y + fontPos.size.height);
                quad.tr.vertices.z = 0;
                quad.tr.texCoords.u = (texRect.origin.x + texRect.size.width);
                quad.tr.texCoords.v = texRect.origin.y;
                quad.tr.colors = style.textColor;
                
                quad.br.vertices.x = (fontPos.origin.x + fontPos.size.width);
                quad.br.vertices.y = fontPos.origin.y;
                quad.br.vertices.z = 0;
                quad.br.texCoords.u = (texRect.origin.x + texRect.size.width);
                quad.br.texCoords.v = (texRect.origin.y + texRect.size.height);
                quad.br.colors = style.textColor;
                
                int i4 = (int) mQuads.size()*4;
                mQuads.push_back(quad);
                
                std::map<HLFont*, std::map<GLuint, std::vector<GLushort> > >::iterator it;
                std::map<GLuint, std::vector<GLushort> >::iterator itr;
                if (style.bold)
                {
                    std::map<GLuint, std::vector<GLushort> >& map = mBoldIndices[style.font];
                    itr = map.find(charInfo.texture);
                    if (itr == map.end())
                    {
                        std::vector<GLushort> v1;
                        map.insert(std::make_pair(charInfo.texture, v1));
                        itr = map.find(charInfo.texture);
                    }
                }
                else
                {
                    std::map<GLuint, std::vector<GLushort> >& map = mIndices[style.font];
                    itr = map.find(charInfo.texture);
                    if (itr == map.end())
                    {
                        std::vector<GLushort> v1;
                        map.insert(std::make_pair(charInfo.texture, v1));
                        itr = map.find(charInfo.texture);
                    }
                }
                itr->second.push_back(i4);
                itr->second.push_back(i4+1);
                itr->second.push_back(i4+2);
                itr->second.push_back(i4+3);
                itr->second.push_back(i4+2);
                itr->second.push_back(i4+1);
            }
            
            mCurrentPos.x += advance;
        }
    }
    
    void HLRichLabel::breakLine()
    {
        if (mCurrentLineHeight == 0)
        {
            mCurrentLineHeight = mFontSize;
        }
        
        if (mCurrentLineHeight != mFontSize)
        {
            float offset = mCurrentLineHeight - mFontSize;
            mCurrentPos.y -= offset;
            if (mCurrentLinePos != mQuads.size())
            {
                int len = (int)mQuads.size();
                for (int i = mCurrentLinePos; i < len; ++i)
                {
                    mQuads[i].tl.vertices.y -= offset;
                    mQuads[i].bl.vertices.y -= offset;
                    mQuads[i].tr.vertices.y -= offset;
                    mQuads[i].br.vertices.y -= offset;
                }
                
                len = (int)mUnderlines.size();
                for (int i = 0; i < len; ++i)
                {
                    if (mUnderlines[i].index >= mCurrentLinePos)
                    {
                        mUnderlines[i].lines[0].y -= offset;
                        mUnderlines[i].lines[1].y -= offset;
                    }
                }
                
                len = (int)mEventrects.size();
                for (int i = 0; i < len; ++i)
                {
                    if (mEventrects[i].index >= mCurrentLinePos)
                    {
                        mEventrects[i].frame.origin.y -= offset;
                    }
                }
                
                len = (int)mImages.size();
                for (int i = 0; i < len; ++i)
                {
                    if (mImages[i].quadIndex >= mCurrentLinePos)
                    {
                        mImages[i].pos.y -= offset;
                    }
                }
            }
        }
        
        int xoffset = 0;
        if (mTextAlign & kTextHAlignmentCenter)
        {
            xoffset = (mBounds.size.width - mCurrentLineWidth) / 2;
        }
        else if (mTextAlign & kTextHAlignmentRight)
        {
            xoffset = mBounds.size.width - mCurrentLineWidth;
        }
        if (xoffset != 0)
        {
            int len = (int)mQuads.size();
            for (int j = mCurrentLinePos; j < len; ++j)
            {
                mQuads[j].tl.vertices.x += xoffset;
                mQuads[j].bl.vertices.x += xoffset;
                mQuads[j].tr.vertices.x += xoffset;
                mQuads[j].br.vertices.x += xoffset;
            }
            
            len = (int)mUnderlines.size();
            for (int i = 0; i < len; ++i)
            {
                if (mUnderlines[i].index >= mCurrentLinePos)
                {
                    mUnderlines[i].lines[0].x += xoffset;
                    mUnderlines[i].lines[1].x += xoffset;
                }
            }
            
            len = (int)mEventrects.size();
            for (int i = 0; i < len; ++i)
            {
                if (mEventrects[i].index >= mCurrentLinePos)
                {
                    mEventrects[i].frame.origin.x += xoffset;
                }
            }
            
            len = (int)mImages.size();
            for (int i = 0; i < len; ++i)
            {
                if (mImages[i].quadIndex >= mCurrentLinePos)
                {
                    mImages[i].pos.x += xoffset;
                }
            }
        }
        
        mTextBoundingBox.width = MAX(mTextBoundingBox.width, mCurrentLineWidth);
        mTextBoundingBox.height = mBounds.size.height - mCurrentPos.y + mLinePadding;
        
        mCurrentPos.x = 0;
        mCurrentPos.y -= mFontSize + mLinePadding;
        mCurrentLinePos = (int)mQuads.size();
        mCurrentLineWidth = 0;
        mCurrentLineHeight = 0;
    }
    
//    void HLRichLabel::setupVBOandVAO()
//    {
//        if (mVBO.size() == 0)
//        {
//            GLuint vbo;
//            glGenBuffers(1, &vbo);
//            mVBO.push_back(vbo);
//        }
//        glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0]) * mQuads.size(), &mQuads[0], GL_DYNAMIC_DRAW);
//        
//        int num = 0;//(int)(mIndices.size() + mBoldIndices.size());
//        std::map<HLFont*, std::map<GLuint, std::vector<GLushort> > >::iterator it;
//        for (it = mIndices.begin(); it != mIndices.end(); ++it)
//        {
//            num += it->second.size();
//        }
//        for (it = mBoldIndices.begin(); it != mBoldIndices.end(); ++it)
//        {
//            num += it->second.size();
//        }
//        
//        if (mVBO.size() - 1 < num)
//        {
//            int currSize = (int)mVBO.size();
//            mVBO.resize(num+1);
//            glGenBuffers(num+1-currSize, &mVBO[currSize]);
//        }
//        if (mVAO.size() < num)
//        {
//            int currSize = (int)mVAO.size();
//            mVAO.resize(num);
//            glGenVertexArrays(num-currSize, &mVAO[currSize]);
//        }
//        
//#define kQuadSize sizeof(mQuads[0].bl)
//        
//        std::map<GLuint, std::vector<GLushort> >::iterator iter;
//        
//        int i = 0;
//        for (it = mIndices.begin(); it != mIndices.end(); ++it)
//        {            
//            for (iter = it->second.begin(); iter != it->second.end(); ++iter, ++i)
//            {
//                glBindVertexArray(mVAO[i]);
//                
//                glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
//                
//                // vertices
//                glEnableVertexAttribArray(kVertexAttrib_Position);
//                glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, vertices));
//                
//                // colors
//                glEnableVertexAttribArray(kVertexAttrib_Color);
//                glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, colors));
//                
//                // tex coords
//                glEnableVertexAttribArray(kVertexAttrib_TexCoords);
//                glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, texCoords));
//                
//                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[i+1]);
//                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iter->second[0]) * iter->second.size(), &(iter->second[0]), GL_STATIC_DRAW);
//                
//                glBindVertexArray(0);
//                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//                glBindBuffer(GL_ARRAY_BUFFER, 0);
//            }
//        }
//        
//        for (it = mBoldIndices.begin(); it != mBoldIndices.end(); ++it)
//        {
//            for (iter = it->second.begin(); iter != it->second.end(); ++iter, ++i)
//            {
//                glBindVertexArray(mVAO[i]);
//                
//                glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
//                
//                // vertices
//                glEnableVertexAttribArray(kVertexAttrib_Position);
//                glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, vertices));
//                
//                // colors
//                glEnableVertexAttribArray(kVertexAttrib_Color);
//                glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, colors));
//                
//                // tex coords
//                glEnableVertexAttribArray(kVertexAttrib_TexCoords);
//                glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( V3F_C4B_T2F, texCoords));
//                
//                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[i+1]);
//                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iter->second[0]) * iter->second.size(), &(iter->second[0]), GL_STATIC_DRAW);
//                
//                glBindVertexArray(0);
//                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//                glBindBuffer(GL_ARRAY_BUFFER, 0);
//                
//                CHECK_GL_ERROR();
//            }
//        }
//    }
    
//    HLView* HLRichLabel::hitTest(HLPoint p)
//    {
//        
//    }
    
    int HLRichLabel::findRectContainsPoint(HLPoint p)
    {
        int len = (int)mEventrects.size();
        
        HLPoint p1 = convertToNodeSpace(p);
        
        for (int i = 0; i < len; ++i)
        {
            if (mEventrects[i].frame.containsPoint(p1))
            {
                return i;
            }
        }
        
        return -1;
    }
    
    void HLRichLabel::onTouches(HLTouchEvent* touch)
    {
        if (touch->type == kHLTouchesBegin || touch->type == kHLTouchesMove)
        {
            int index = findRectContainsPoint(touch->touches[0].location);
            if (index >= 0)
            {
                mHightlightRect = mEventrects[index].frame;
            }
            else
            {
                mHightlightRect = HLRectZero;
            }
        }
        else if (touch->type == kHLTouchesEnd)
        {
            int index = findRectContainsPoint(touch->touches[0].location);
            if (index >= 0)
            {
                handleURL(mEventrects[index].url);
            }
            mHightlightRect = HLRectZero;
        }
        else
        {
            mHightlightRect = HLRectZero;
        }
    }
    
#if defined(TARGET_MACOS) || defined(TARGET_WIN32)
    void HLRichLabel::onMouseEvent(HLMouseEvent* mouseEvent)
    {
        if (mouseEvent->type == kHLMouseDown || mouseEvent->type == kHLMouseDrag)
        {
            int index = findRectContainsPoint(mouseEvent->location);
            if (index >= 0)
            {
                mHightlightRect = mEventrects[index].frame;
            }
            else
            {
                mHightlightRect = HLRectZero;
            }
        }
        else if (mouseEvent->type == kHLMouseUp)
        {
            int index = findRectContainsPoint(mouseEvent->location);
            if (index >= 0)
            {
                handleURL(mEventrects[index].url);
            }
            mHightlightRect = HLRectZero;
        }
        else
        {
            mHightlightRect = HLRectZero;
        }
    }
#endif
    
    void HLRichLabel::handleURL(std::string url)
    {
        if (!strncmp(url.c_str(), "@action(", 8))
        {
            std::vector<std::string> res;
            StringUtil::Split(res, url, '?');
            std::string action = res[0];
            action = action.substr(8, action.length()-9);
            StringUtil::Trim(action);
            std::map<std::string, std::string> args;
            if (res.size() > 1)
            {
                std::string paramstr = res[1];
                res.clear();
                StringUtil::Split(res, paramstr, '&');
                std::vector<std::string>::iterator itr;
                std::vector<std::string> res1;
                for (itr = res.begin(); itr != res.end(); ++itr)
                {
                    res1.clear();
                    StringUtil::Split(res1, *itr, '=');
                    std::string name = res1[0];
                    std::string value;
                    if (res1.size() > 1)
                    {
                        value = res1[1];
                    }
                    args.insert(make_pair(name, value));
                }
            }
            delegate(this, action, args);
        }
        else
        {
            openURL(url);
        }
    }
    
    void HLRichLabel::onDraw()
    {
        if (mNeedsLayout)
        {
            layoutText();
        }
        
        if (mText.length() == 0 || mBounds.size == HLSizeZero)
        {
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
//            else if (!strcmp(uniform.name, kUniformBase))
//            {
//                uniform.setUniform1f(mFont->getDistBase() / 32768.0f);
//            }
//            else if (!strcmp(uniform.name, kUniformScale))
//            {
//                uniform.setUniform1f(mFont->getDistScale() / 32768.0f + 1);
//            }
            else if (!strcmp(uniform.name, kUniformBorderOffset))
            {
                uniform.setUniform1f(0);
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
        
        std::map<GLuint, std::vector<GLushort> >::iterator iter;
        std::map<HLFont*, std::map<GLuint, std::vector<GLushort> > >::iterator it;
        int i = 0;
        for (it = mIndices.begin(); it != mIndices.end(); ++it)
        {
            program->getUniform(kUniformBase).setUniform1f(it->first->getDistBase() / 32768.0f);
            program->getUniform(kUniformScale).setUniform1f(it->first->getDistScale() / 32768.0f + 1);
            for (iter = it->second.begin(); iter != it->second.end(); ++iter, ++i)
            {
                hlglBindTexture(GL_TEXTURE_2D, iter->first);
                glDrawElements(GL_TRIANGLES, (GLsizei)iter->second.size(), GL_UNSIGNED_SHORT, &iter->second[0]);
            }
        }

        for (it = mBoldIndices.begin(); it != mBoldIndices.end(); ++it)
        {
            program->getUniform(kUniformBase).setUniform1f(it->first->getDistBase() / 32768.0f);
            program->getUniform(kUniformScale).setUniform1f(it->first->getDistScale() / 32768.0f + 1);
            program->getUniform(kUniformBorderOffset).setUniform1f(0.01f);
            for (iter = it->second.begin(); iter != it->second.end(); ++iter, ++i)
            {
                hlglBindTexture(GL_TEXTURE_2D, iter->first);
                glDrawElements(GL_TRIANGLES, (GLsizei)iter->second.size(), GL_UNSIGNED_SHORT, &iter->second[0]);
            }
        }
        
        if (!mImageIndices.empty())
        {
            program = HLProgramManager::currentManager()->getProgram(kShaderPositionTextureColor);
            program->use();
            program->setUniformForModelViewProjectionMatrix();
            
            program->getUniforms()[0].setUniform1f(mBrightness);
            
//            // vertices
//            glEnableVertexAttribArray(kVertexAttrib_Position);
//            glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) ((uint8_t*)&mQuads[0] + offsetof( V3F_C4B_T2F, vertices)));
//            
//            // colors
//            glEnableVertexAttribArray(kVertexAttrib_Color);
//            glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) ((uint8_t*)&mQuads[0] + offsetof( V3F_C4B_T2F, colors)));
//            
//            // tex coords
//            glEnableVertexAttribArray(kVertexAttrib_TexCoords);
//            glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) ((uint8_t*)&mQuads[0] + offsetof( V3F_C4B_T2F, texCoords)));
            
            for (iter = mImageIndices.begin(); iter != mImageIndices.end(); ++iter)
            {
                hlglBindTexture(GL_TEXTURE_2D, iter->first);
                glDrawElements(GL_TRIANGLES, (GLsizei)iter->second.size(), GL_UNSIGNED_SHORT, &iter->second[0]);
            }
        }
        
//        std::map<GLuint, std::vector<GLushort> >::iterator iter;
//        std::map<HLFont*, std::map<GLuint, std::vector<GLushort> > >::iterator it;
//        int i = 0;
//        for (it = mIndices.begin(); it != mIndices.end(); ++it)
//        {
//            program->getUniform(kUniformBase).setUniform1f(it->first->getDistBase() / 32768.0f);
//            program->getUniform(kUniformScale).setUniform1f(it->first->getDistScale() / 32768.0f + 1);
//            for (iter = it->second.begin(); iter != it->second.end(); ++iter, ++i)
//            {
//                hlglBindTexture(GL_TEXTURE_2D, iter->first);
//                glBindVertexArray(mVAO[i]);
//                glDrawElements(GL_TRIANGLES, (GLsizei)iter->second.size(), GL_UNSIGNED_SHORT, 0);
//            }
//        }
//        
//        for (it = mBoldIndices.begin(); it != mBoldIndices.end(); ++it)
//        {
//            program->getUniform(kUniformBase).setUniform1f(it->first->getDistBase() / 32768.0f);
//            program->getUniform(kUniformScale).setUniform1f(it->first->getDistScale() / 32768.0f + 1);
//            program->getUniform(kUniformBorderOffset).setUniform1f(0.01f);
//            for (iter = it->second.begin(); iter != it->second.end(); ++iter, ++i)
//            {
//                hlglBindTexture(GL_TEXTURE_2D, iter->first);
//                glBindVertexArray(mVAO[i]);
//                glDrawElements(GL_TRIANGLES, (GLsizei)iter->second.size(), GL_UNSIGNED_SHORT, 0);
//            }
//        }
//        
//        glBindVertexArray( 0 );
        
        for (std::vector<struct underline>::iterator i = mUnderlines.begin(); i != mUnderlines.end(); ++i)
        {
            drawColor4B((*i).color);
            drawLine((*i).lines[0], (*i).lines[1]);
        }
        
        if (mHightlightRect != HLRectZero)
        {
            drawColor4F(1, 1, 1, 0.5f);
            drawSolidRect(mHightlightRect.origin, HLPoint(mHightlightRect.origin.x+mHightlightRect.size.width, mHightlightRect.origin.y+mHightlightRect.size.height));
        }
        
        CHECK_GL_ERROR();
        
        HLView::onDraw();
    }
    
    void HLRichLabel::startShowAnimation(float speed, IDelegate1<HLRichLabel*>* delegate)
    {
        mAnimSpeed = speed;
        if (delegate)
        {
            animDelegate = delegate;
        }
        
        if (mText.length() == 0)
        {
            return;
        }
        
        mAnimPos = 0;
        if (mAnimTimer)
        {
            mAnimTimer->cancel();
            mAnimTimer = NULL;
        }
        
        if (mNeedsLayout)
        {
            layoutText();
        }
        
        for (std::vector<V3F_C4B_T2F_Quad>::iterator itr = mQuads.begin(); itr != mQuads.end(); ++itr)
        {
            (*itr).tl.colors.a = 0;
            (*itr).bl.colors.a = 0;
            (*itr).tr.colors.a = 0;
            (*itr).br.colors.a = 0;
        }
//        glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0]) * mQuads.size(), &mQuads[0], GL_DYNAMIC_DRAW);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        for (std::vector<struct underline>::iterator itr = mUnderlines.begin(); itr != mUnderlines.end(); ++itr)
        {
            (*itr).color.a = 0;
        }
        
        mAnimTimer = new HLTimer(0.04, true);
        mAnimTimer->delegate = newDelegate(this, &HLRichLabel::animUpdate);
    }
    
    void HLRichLabel::stopShowAnimation()
    {
        if (mAnimTimer)
        {
            mAnimTimer->cancel();
            mAnimTimer = NULL;
        }
        else
        {
            return;
        }
        int pos = floorf(mAnimPos);
        int len = (int)mQuads.size();
        for (int i = pos; i < len; ++i)
        {
            mQuads[i].tl.colors.a = 255;
            mQuads[i].bl.colors.a = 255;
            mQuads[i].tr.colors.a = 255;
            mQuads[i].br.colors.a = 255;
        }
//        glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0]) * mQuads.size(), &mQuads[0], GL_DYNAMIC_DRAW);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        for (std::vector<struct underline>::iterator itr = mUnderlines.begin(); itr != mUnderlines.end(); ++itr)
        {
            (*itr).color.a = 255;
        }
        animDelegate(this);
    }
    
    void HLRichLabel::animUpdate(HLTimer*, float dt)
    {
        int prePos = floorf(mAnimPos);
        mAnimPos += mAnimSpeed*dt;
        int pos = floorf(mAnimPos);
        if (pos >= mQuads.size() - 1)
        {
            mAnimPos = prePos;
            stopShowAnimation();
            return;
        }
        for (int i = prePos; i < pos; ++i)
        {
            mQuads[i].tl.colors.a = 255;
            mQuads[i].bl.colors.a = 255;
            mQuads[i].tr.colors.a = 255;
            mQuads[i].br.colors.a = 255;
        }
        float percent = mAnimPos - pos;
        mQuads[pos].tl.colors.a = 255*percent;
        mQuads[pos].bl.colors.a = 255*percent;
        mQuads[pos].tr.colors.a = 255*percent;
        mQuads[pos].br.colors.a = 255*percent;
        
//        glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
//        glBufferSubData(GL_ARRAY_BUFFER, prePos*sizeof(mQuads[0]), (pos-prePos+1)*sizeof(mQuads[0]), &mQuads[prePos]);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    void HLRichLabel::setAlpha(float alpha)
    {
        if (mText.length() == 0)
        {
            return;
        }

        mTextStrokeColor.a = alpha;
        size_t count = mQuads.size();
        for (size_t i = 0; i < count; ++i)
        {
            mQuads[i].tl.colors.a = 255*alpha;
            mQuads[i].bl.colors.a = 255*alpha;
            mQuads[i].tr.colors.a = 255*alpha;
            mQuads[i].br.colors.a = 255*alpha;
        }
        
//        glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0]) * mQuads.size(), &mQuads[0], GL_DYNAMIC_DRAW);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
#if USE_JS_SCRIPTING
    JSCppWrapperBase* HLRichLabel::getOrCreateJSWrapper(JSContext* cx)
    {
        return JSRichLabel::getOrCreateWrapper(cx, this);
    }
#endif
}

NS_HL_END
