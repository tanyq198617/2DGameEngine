//
//  HLBMFont.h
//  HoolaiEngine
//
//  Created by tyq on 12-8-28.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLBMFONT_H__
#define __HLBMFONT_H__

#include "HLMacros.h"
#include "HLUtil.h"
#include <string>
#include <map>
#include <vector>
#include "HLGeometry.h"
#include "HLObject.h"

NS_HL_BEGIN

class HLTexture;

typedef struct _BMFontDef {
    // ID of the character
    unsigned int charId;
    // origin and size of the font
    HLRect rect;
    // The X amount the image should be offset when drawing the image (in pixels)
    short xOffset;
    // The Y amount the image should be offset when drawing the image (in pixels)
    short yOffset;
    // The amount to move the current position after drawing the character (in pixels)
    short xAdvance;
} BMFontDef;

typedef struct _KearningInfo {
    short first;
    short second;
    short amount;
} KerningInfo;

typedef struct _BMFontPadding {
    int left;
    int top;
    int right;
    int bottom;
} BMFontPadding;

class HLBMFont: public HLObject
{    
public:
    void setColor(int r, int g, int b, int a) {_color.r=r;_color.g=g;_color.b=b;_color.a=a;}
    float getHeight(){return _lineHeight;}
    void print(float, float, const char *,...);
    float widthForText(const char *,...);

    ~HLBMFont();
    
    static HLBMFont* getFont(std::string fntFile)
    {
        std::map<std::string, HLBMFont*>::iterator itr = fontCache.find(fntFile);
        if (itr != fontCache.end())
        {
            return itr->second;
        }
        HLBMFont *font = new HLBMFont(fntFile.c_str());
        fontCache.insert(make_pair(fntFile, font));
        return font;
    }
    
    static void purgeFontCache()
    {
        std::map<std::string, HLBMFont*>::iterator itr = fontCache.begin();
        while (itr != fontCache.end())
        {
            itr->second->release();
            itr++;
        }
        fontCache.clear();
    }
    
private:
    HLBMFont()
    {
        setColor(255,255,255,255);
        _kernCount = 0;
        _texture = NULL;
    };
    HLBMFont(const char* font)
    {
        setColor(255,255,255,255);
        _kernCount = 0;
        _texture = NULL;
        parseFontFile(font);
    }
    short _lineHeight;
    short _base;
    short _width;
    short _height;
    short _pages;
    short _outline;
    short _kernCount;
    BMFontPadding _padding;
    color4B _color;
    HLTexture* _texture;
    float _scale;
    
    bool parseFontFile(const char *);
    void parseCharacterDefinition(const char *, BMFontDef *);
    void parseInfoArguments(const char *);
    void parseCommonArguments(const char *);
    void parseImageFileName(const char *, const char *);
    void parseKerningEntry(const char *);
    int getKerningPair(int, int);
    float getStringWidth(const char *);
    static std::map<std::string, HLBMFont*> fontCache;
    
    
public:
    std::map<unsigned int, BMFontDef> _chars;
    std::map<unsigned int, KerningInfo> _kerningInfo;
    
};

NS_HL_END

#endif /* defined(__HLBMFONT_H__) */
