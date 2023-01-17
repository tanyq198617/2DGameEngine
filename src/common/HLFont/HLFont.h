//
//  HLFont.h
//  HoolaiEngine
//
//  Created by tyq on 13-1-10.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLFONT_H__
#define __HLFONT_H__

#include <iostream>
#include "HLMacros.h"
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include "HLGL.h"
#include "HLGeometry.h"
#include <stdint.h>

#ifndef FONT_VERSION
#define FONT_VERSION 1
#endif

#if defined(__GNUC__) || defined(_MSC_VER)
#define HAS_STRUCT_PACK
#endif

NS_HL_BEGIN

#ifdef HAS_STRUCT_PACK
#pragma pack(push, 1)
#endif
struct hlfont_header
{
    uint32_t magic;
    uint32_t version;
    uint32_t start_ptr;
    uint32_t validate_chars;
    uint32_t non_empty_chars;
    uint32_t char_size;
    
    int16_t base;
    int16_t scale;
};
#ifdef HAS_STRUCT_PACK
#pragma pack(pop)
#endif

class HLFont
{
public:
#ifdef HAS_STRUCT_PACK
#pragma pack(push, 1)
#endif
    struct font_info
    {
        int16_t top;
        int16_t left;
        uint16_t width;
        uint16_t height;
    };
#ifdef HAS_STRUCT_PACK
#pragma pack(pop)
#endif
    
    struct CharInfo
    {
        HLPoint rc;
//        uint64_t tick;
        GLuint texture;
    };
    
    static HLFont* getFont(const char* font = NULL);
    static void addFont(const char* font);
    static HLFont* defaultFont();
    
    uint32_t getCharSize() const;
    int16_t getDistBase() const;
    int16_t getDistScale() const;
    
    std::pair<int32_t, uint32_t> const & getCharIndexAdvance(uint16_t ch) const;
    int32_t getCharIndex(uint16_t ch) const;
    uint32_t getCharAdvance(uint16_t ch) const;
    CharInfo const & getCharInfo(uint16_t ch) const;
    font_info const & getCharFontInfo(int32_t index) const;
    HLSize const getTextureSize() const;
    void addText(std::string text);
    void addText(uint16_t* text);
    void bindTexture();
    
    void testdraw(std::string str, uint32_t size);
    
    void updateTexture();
    
    void setRenderThresHold(float value) {
        mThresHold = value;
    }
    float getRenderThresHold() {
        return mThresHold;
    }
    
private:
    HLFont();
    bool load(std::string const& file);
    void getDistanceData(uint8_t* p, uint32_t pitch, int32_t index);
    void updateTexture(uint16_t const * str);
    
    uint32_t mCharSize;
    int16_t mDistBase;
    int16_t mDistScale;
    std::vector<font_info> mCharInfo;
    std::vector<uint32_t> mDistancesAddr;
    uint32_t mDistancesCCZStartPtr;
//    std::vector<uint8_t> mDistancesCCZ;
    std::ifstream mFontInput;
    std::map<uint16_t, std::pair<int32_t, uint32_t> > mCharIndexAdvance;
    std::map<uint16_t, CharInfo> mCharInfoMap;
    std::list<std::pair<uint32_t, uint32_t> > mCharFreeList;
    
    GLuint mTexture;
    uint32_t m_char_pos_x;
    uint32_t m_char_pos_y;
    uint8_t* mTexData;
    std::vector<GLuint> mPreTexture;
    std::vector<uint8_t> mPreData;
    uint32_t mTexSize;
    
    bool mCurrentTextureNeedsUpdate;
    
    float mThresHold {-0.01};
//    uint64_t tick_;
};

NS_HL_END

#endif /* defined(__HoolaiEngine__HLFont__) */
