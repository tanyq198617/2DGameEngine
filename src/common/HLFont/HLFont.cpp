//
//  HLFont.cpp
//  HoolaiEngine
//
//  Created by tyq on 13-1-10.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLFont.h"
#include "HLFileUtil.h"
#include "HLUtil.h"
#include "HLGLConfiguration.h"
#include <zlib.h>
#include "HLProgram.h"
#include "HLDrawingPrimitives.h"
#include "HLResourceManager.h"

using namespace std;

NS_HL_BEGIN

extern uint16_t* utf16_from_utf8(const char* str_old);

map<string, HLFont*> fontinstances;

HLFont* HLFont::getFont(const char* font)
{
    if (!font)
    {
        if (fontinstances.empty())
        {
            return NULL;
        }
        return fontinstances.begin()->second;
    }
    map<string, HLFont*>::iterator itr = fontinstances.find(std::string(font));
    if (itr != fontinstances.end())
    {
        return itr->second;
    }
    else
    {
        HLFont *ft = new HLFont();
        const char* path = HLResourceManager::getSingleton()->getSysPathForFile(font);
        bool res = ft->load(path);
        if (!res)
        {
            return NULL;
        }
        fontinstances.insert(make_pair(font, ft));
        return ft;
    }
}

void HLFont::addFont(const char* font)
{
    if (font)
    {
        map<string, HLFont*>::iterator itr = fontinstances.find(std::string(font));
        if (itr == fontinstances.end())
        {
            HLFont *ft = new HLFont();
            const char* path = HLResourceManager::getSingleton()->getSysPathForFile(font);
            ft->load(path);
            fontinstances.insert(make_pair(font, ft));
            ft->addText("1234567890.");
        }
    }
}

HLFont* HLFont::defaultFont()
{
    map<string, HLFont*>::iterator itr = fontinstances.begin();
    if (itr != fontinstances.end())
    {
        return itr->second;
    }
    return NULL;
}

uint32_t HLFont::getCharSize() const
{
    return mCharSize;
}

int16_t HLFont::getDistBase() const
{
    return mDistBase;
}

int16_t HLFont::getDistScale() const
{
    return mDistScale;
}

std::pair<int32_t, uint32_t> const & HLFont::getCharIndexAdvance(uint16_t ch) const
{
    std::map<uint16_t, std::pair<int32_t, uint32_t> >::const_iterator itr = mCharIndexAdvance.find(ch);
    if (itr != mCharIndexAdvance.end())
    {
        return itr->second;
    }
    else
    {
        static std::pair<int32_t, uint32_t> ret(-1, 0);
        return ret;
    }
}

HLSize const HLFont::getTextureSize() const
{
    return HLSize(HLGLConfiguration::getSingleton()->getMaxTextureSize(), mTexSize);
}

int32_t HLFont::getCharIndex(uint16_t ch) const
{
    return getCharIndexAdvance(ch).first;
}

uint32_t HLFont::getCharAdvance(uint16_t ch) const
{
    return getCharIndexAdvance(ch).second;
}

HLFont::CharInfo const & HLFont::getCharInfo(uint16_t ch) const
{
    return mCharInfoMap.find(ch)->second;
}

HLFont::font_info const & HLFont::getCharFontInfo(int32_t index) const
{
    return mCharInfo[index];
}

void HLFont::addText(std::string text)
{
    uint16_t* unicode = utf16_from_utf8(text.c_str());
    
    updateTexture(unicode);
    
    delete [] unicode;
}

void HLFont::addText(uint16_t* text)
{
    updateTexture(text);
}

typedef struct
{
    GLfloat x,y;
    GLfloat texx,texy;
    GLubyte r,g,b,a;
} vert_texcoord_color;

void HLFont::testdraw(std::string str, uint32_t size)
{
    vert_texcoord_color vertlst;
    uint16_t* unicode = utf16_from_utf8(str.c_str());
    addText(unicode);
    color4B color;
    color.r = color.b = color.g = color.a = 255;
    float const rel_size = static_cast<float>(size) / mCharSize;
    float x = 0;
    int i;
    std::map<GLuint, std::vector<vert_texcoord_color> > vertMap;
    for (i = 0; unicode[i] != 0; i++)
    {
        std::pair<int32_t, uint32_t> const & offset_adv = getCharIndexAdvance(unicode[i]);
        if (offset_adv.first != -1)
        {
            font_info const & ci = getCharFontInfo(offset_adv.first);
            
            float left = ci.left * rel_size;
            float top = ci.top * rel_size;
            float size = mCharSize * rel_size;
            HLRect fontPos(x+left, -top, size, size);
            
            HLRect texRect;
            CharInfo& charInfo = mCharInfoMap.find(unicode[i])->second;
            texRect.origin = charInfo.rc;
            texRect.origin.x /= 512;
            texRect.origin.y /= 512;
            texRect.size.width = static_cast<float>(mCharSize)/512;
            texRect.size.height = static_cast<float>(mCharSize)/512;
            
            std::map<GLuint, std::vector<vert_texcoord_color> >::iterator itr = vertMap.find(charInfo.texture);
            if (itr == vertMap.end())
            {
                std::vector<vert_texcoord_color> v;
                vertMap.insert(std::make_pair(charInfo.texture, v));
                itr = vertMap.find(charInfo.texture);
            }
            
            // tl
            vertlst.x = fontPos.origin.x;
            vertlst.y = (fontPos.origin.y + fontPos.size.height);
            vertlst.texx = texRect.origin.x;
            vertlst.texy = texRect.origin.y;
            vertlst.r = color.r;
            vertlst.g = color.g;
            vertlst.b = color.b;
            vertlst.a = color.a;
            itr->second.push_back(vertlst);
            
            // bl
            vertlst.x = fontPos.origin.x;
            vertlst.y = fontPos.origin.y;
            vertlst.texx = texRect.origin.x;
            vertlst.texy = (texRect.origin.y + texRect.size.height);
            vertlst.r = color.r;
            vertlst.g = color.g;
            vertlst.b = color.b;
            vertlst.a = color.a;
            itr->second.push_back(vertlst);
            
            // tr
            vertlst.x = (fontPos.origin.x + fontPos.size.width);
            vertlst.y = (fontPos.origin.y + fontPos.size.height);
            vertlst.texx = (texRect.origin.x + texRect.size.width);
            vertlst.texy = texRect.origin.y;
            vertlst.r = color.r;
            vertlst.g = color.g;
            vertlst.b = color.b;
            vertlst.a = color.a;
            itr->second.push_back(vertlst);
            
            // br
            vertlst.x = (fontPos.origin.x + fontPos.size.width);
            vertlst.y = fontPos.origin.y;
            vertlst.texx = (texRect.origin.x + texRect.size.width);
            vertlst.texy = (texRect.origin.y + texRect.size.height);
            vertlst.r = color.r;
            vertlst.g = color.g;
            vertlst.b = color.b;
            vertlst.a = color.a;
            itr->second.push_back(vertlst);
            
            // tr
            vertlst.x = (fontPos.origin.x + fontPos.size.width);
            vertlst.y = (fontPos.origin.y + fontPos.size.height);
            vertlst.texx = (texRect.origin.x + texRect.size.width);
            vertlst.texy = texRect.origin.y;
            vertlst.r = color.r;
            vertlst.g = color.g;
            vertlst.b = color.b;
            vertlst.a = color.a;
            itr->second.push_back(vertlst);
            
            // bl
            vertlst.x = fontPos.origin.x;
            vertlst.y = fontPos.origin.y;
            vertlst.texx = texRect.origin.x;
            vertlst.texy = (texRect.origin.y + texRect.size.height);
            vertlst.r = color.r;
            vertlst.g = color.g;
            vertlst.b = color.b;
            vertlst.a = color.a;
            itr->second.push_back(vertlst);
        }
        x += (offset_adv.second&0xFFFF)*rel_size;
    }
    
    delete [] unicode;
    
    HLProgram* program = HLProgramManager::currentManager()->getProgram(kShaderPositionTextureA8Dist);
    program->use();
    program->setUniformForModelViewProjectionMatrix();
    hlglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    std::vector<HLProgramUniform>& uniforms = program->getUniforms();
    for (std::vector<HLProgramUniform>::iterator it = uniforms.begin(); it != uniforms.end(); ++it)
    {
        HLProgramUniform& uniform = *it;
        if (!strcmp(uniform.name, kUniformBrightness))
        {
            uniform.setUniform1f(1.f);
        }
        else if (!strcmp(uniform.name, kUniformBase))
        {
            uniform.setUniform1f(mDistBase / 32768.0f);
        }
        else if (!strcmp(uniform.name, kUniformScale))
        {
            uniform.setUniform1f(mDistScale / 32768.0f + 1);
        }
        else if (!strcmp(uniform.name, kUniformThreshold))
        {
            uniform.setUniform1f(mThresHold);
        }
    }
    
    glEnableVertexAttribArray(kVertexAttrib_Position);
    glEnableVertexAttribArray(kVertexAttrib_Color);
    glEnableVertexAttribArray(kVertexAttrib_TexCoords);
    
    std::map<GLuint, std::vector<vert_texcoord_color> >::iterator itr = vertMap.begin();
    
    for (; itr != vertMap.end(); ++itr)
    {
        hlglBindTexture(GL_TEXTURE_2D, itr->first);
        glVertexAttribPointer(kVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, sizeof(vert_texcoord_color), &(itr->second[0].x));
        glVertexAttribPointer(kVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(vert_texcoord_color),  &(itr->second[0].texx));
        glVertexAttribPointer(kVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vert_texcoord_color), &(itr->second[0].r));
        
        glDrawArrays(GL_TRIANGLES, 0, (int)itr->second.size());
    }
    
    CHECK_GL_ERROR();
}

void HLFont::bindTexture()
{
    hlglBindTexture(GL_TEXTURE_2D, mTexture);
}

HLFont::HLFont():mDistancesAddr(1, 0),mTexData(0),mTexture(0),m_char_pos_x(0),m_char_pos_y(0)
{
    mTexData = (uint8_t*)malloc(512 * 512);
    memset(mTexData, 0, 512 * 512);
}

bool HLFont::load(std::string const& file)
{
    mFontInput.open(file.c_str(), ios_base::binary|ios_base::in);
    if (mFontInput)
    {
        hlfont_header header;
        mFontInput.read(reinterpret_cast<char*>(&header), sizeof(header));
        LittleEndianToNative<sizeof(header.magic)>(&header.magic);
        LittleEndianToNative<sizeof(header.version)>(&header.version);
        LittleEndianToNative<sizeof(header.start_ptr)>(&header.start_ptr);
        LittleEndianToNative<sizeof(header.validate_chars)>(&header.validate_chars);
        LittleEndianToNative<sizeof(header.non_empty_chars)>(&header.non_empty_chars);
        LittleEndianToNative<sizeof(header.char_size)>(&header.char_size);
        LittleEndianToNative<sizeof(header.base)>(&header.base);
        LittleEndianToNative<sizeof(header.scale)>(&header.scale);
        if ('H'|'L'<<4|'N'<<8|'T'<<24==header.magic && FONT_VERSION==header.version)
        {
            mCharSize = header.char_size;
            mDistBase = header.base;
            mDistScale = header.scale;
            
            mDistancesCCZStartPtr = header.start_ptr;
            mFontInput.seekg(header.start_ptr, std::ios_base::beg);
            std::vector<std::pair<int32_t, int32_t> > temp_char_index(header.non_empty_chars);
            mFontInput.read(reinterpret_cast<char*>(&temp_char_index[0]), static_cast<std::streamsize>(temp_char_index.size() * sizeof(temp_char_index[0])));
            mDistancesCCZStartPtr += temp_char_index.size() * sizeof(temp_char_index[0]);
            std::vector<std::pair<int32_t, uint32_t> > temp_char_advance(header.validate_chars);
            mFontInput.read(reinterpret_cast<char*>(&temp_char_advance[0]), static_cast<std::streamsize>(temp_char_advance.size() * sizeof(temp_char_advance[0])));
            mDistancesCCZStartPtr += temp_char_advance.size() * sizeof(temp_char_advance[0]);
            std::vector<std::pair<int32_t, int32_t> >::iterator ci = temp_char_index.begin();
            for(;ci!=temp_char_index.end();ci++)
            {
                LittleEndianToNative<sizeof((*ci).first)>(&(*ci).first);
                LittleEndianToNative<sizeof((*ci).second)>(&(*ci).second);
                mCharIndexAdvance.insert(std::make_pair((*ci).first, std::make_pair((*ci).second, 0)));
            }
            std::vector<std::pair<int32_t, uint32_t> >::iterator ca = temp_char_advance.begin();
            for (; ca != temp_char_advance.end(); ++ca)
            {
                LittleEndianToNative<sizeof((*ca).first)>(&(*ca).first);
                LittleEndianToNative<sizeof((*ca).second)>(&(*ca).second);
                
                std::map<uint16_t, std::pair<int32_t, uint32_t> >::iterator itr2 = mCharIndexAdvance.find((*ca).first);
                if (itr2 != mCharIndexAdvance.end())
                {
                    itr2->second.second = (*ca).second;
                }
                else
                {
                    mCharIndexAdvance[(*ca).first] = std::make_pair(-1, (*ca).second);
                }
            }
            
            mCharInfo.resize(header.non_empty_chars);
            mFontInput.read(reinterpret_cast<char*>(&mCharInfo[0]), static_cast<std::streamsize>(mCharInfo.size() * sizeof(mCharInfo[0])));
            mDistancesCCZStartPtr += mCharInfo.size() * sizeof(mCharInfo[0]);
            
            std::vector<font_info>::iterator itr2 = mCharInfo.begin();
            for (; itr2 != mCharInfo.end(); ++itr2)
            {
                LittleEndianToNative<sizeof((*itr2).left)>(&(*itr2).left);
                LittleEndianToNative<sizeof((*itr2).top)>(&(*itr2).top);
                LittleEndianToNative<sizeof((*itr2).width)>(&(*itr2).width);
                LittleEndianToNative<sizeof((*itr2).height)>(&(*itr2).height);
            }
            
            mDistancesAddr.resize(header.non_empty_chars + 1);
            mFontInput.read(reinterpret_cast<char*>(&mDistancesAddr[0]), static_cast<std::streamsize>(mDistancesAddr.size() * sizeof(mDistancesAddr[0])));
            
            for (std::vector<uint32_t>::iterator itr3 = mDistancesAddr.begin(); itr3 != mDistancesAddr.end(); ++itr3)
            {
                LittleEndianToNative<sizeof((*itr3))>(&(*itr3));
            }
            mDistancesCCZStartPtr += mDistancesAddr.size() * sizeof(mDistancesAddr[0]);
            
            return true;
        }
    }
    return false;
}

void HLFont::getDistanceData(uint8_t* p, uint32_t pitch, int32_t index)
{
    std::vector<uint8_t> decoded(mCharSize * mCharSize);
    
    uint32_t size = static_cast<uint32_t>(mDistancesAddr[index + 1] - mDistancesAddr[index]);
    uint8_t * input = new uint8_t[size];
    mFontInput.seekg(mDistancesCCZStartPtr + mDistancesAddr[index], ios_base::beg);
    mFontInput.read(reinterpret_cast<char*>(input), size);
    
    uLong destsize = decoded.size();
    uncompress(static_cast<Byte*>(&decoded[0]), &destsize, input, (uLong)size);
    delete input;
    
    uint8_t const * char_data = &decoded[0];
    for (uint32_t y = 0; y < mCharSize; ++ y)
    {
        memcpy(p, char_data, mCharSize);
        p += pitch;
        char_data += mCharSize;
    }
}

void HLFont::updateTexture()
{
    size_t count = mPreTexture.size();
    for (size_t i = 0; i < count; ++i)
    {
        hlglBindTexture(GL_TEXTURE_2D, mPreTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &mPreData[i * 512 * 512]);
    }
    mPreTexture.clear();
    mPreData.clear();
    if (mCurrentTextureNeedsUpdate)
    {
        hlglBindTexture(GL_TEXTURE_2D, mTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, mTexData);
        mCurrentTextureNeedsUpdate = false;
    }
}

// alloc 512x512 textures, draw text with different textures separately
void HLFont::updateTexture(uint16_t const * str)
{
    int i = 0;
//    bool updated = false;
    
    if (mTexture == 0)
    {
        glGenTextures(1, &mTexture);
        hlglBindTexture(GL_TEXTURE_2D, mTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, mTexData);
    }
    else
    {
//        hlglBindTexture(GL_TEXTURE_2D, mTexture);
    }
    
    while (str[i])
    {
        int32_t offset = getCharIndex(str[i]);
        if (offset != -1)
        {
            std::map<uint16_t, CharInfo>::iterator cmiter = mCharInfoMap.find(str[i]);
            if (cmiter != mCharInfoMap.end()) // character already in texture
            {
                ++i;
                continue;
            }
            else
            {
                mCurrentTextureNeedsUpdate = true;
                CharInfo charInfo;
                charInfo.rc.x = m_char_pos_x;
                charInfo.rc.y = m_char_pos_y;
                charInfo.texture = mTexture;
                
                mCharInfoMap.insert(make_pair(str[i], charInfo));
                
                getDistanceData(mTexData+m_char_pos_x+m_char_pos_y*512, 512, offset);
//                glTexSubImage2D(mTexture, 0, m_char_pos_x*mCharSize, m_char_pos_y*mCharSize, mCharSize, mCharSize, GL_ALPHA, GL_UNSIGNED_BYTE, mTexData);
                m_char_pos_x += mCharSize;
                if (m_char_pos_x + mCharSize > 512)
                {
                    m_char_pos_x = 0;
                    m_char_pos_y += mCharSize;
                }
                if (m_char_pos_y + mCharSize > 512) // need new texture to load characters
                {
                    m_char_pos_y = 0;
                    
//                    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, mTexData);
                    
                    
                    mPreTexture.push_back(mTexture);
                    size_t size = mPreData.size();
                    mPreData.resize(size + 512 * 512);
                    memcpy(&mPreData[size], mTexData, 512 * 512);
                    
                    mCurrentTextureNeedsUpdate = false;
                    memset(mTexData, 0, 512 * 512);
                    
                    glGenTextures(1, &mTexture);
                    hlglBindTexture(GL_TEXTURE_2D, mTexture);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, mTexData);
                }
            }
        }
        
        ++i;
    }
    
//    if (updated)
//    {
//        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 512, 512, GL_ALPHA, GL_UNSIGNED_BYTE, mTexData);
//    }
    
    CHECK_GL_ERROR();
}


NS_HL_END
