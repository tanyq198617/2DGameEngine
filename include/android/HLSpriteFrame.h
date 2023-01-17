//
//  HLSpriteFrame.h
//  HoolaiEngine2D
//
//  Created by zhao shuan on 12-6-12.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLSPRITEFRAME_H__
#define __HLSPRITEFRAME_H__

#include "HLObject.h"
#include "HLGeometry.h"

NS_HL_BEGIN

class HLEntity;
class HLTexture;
class HLSpriteFrame:public HLObject
{
private:
    static std::map<std::string, HLDictionary*> spriteframemap;
public:
    std::string _name;
    HLRect _textureRect;
    HLPoint _offset;
    HLSize _sourceSize;
    HLTexture *_texture;
    bool _rotated;
    
    HLSpriteFrame() {};
    HLSpriteFrame(std::string name, HLRect textureRect, HLPoint offset, HLSize sourceSize):
        _name(name),_textureRect(textureRect),_offset(offset),_sourceSize(sourceSize),_rotated(false) {};
    ~HLSpriteFrame();
    
    static HLArray* getSpriteFrameArray(const char* file);
    static HLDictionary* getSpriteFrameDictionary(const char* file);
    static HLSpriteFrame* getSpriteFrame(const char* file, const char* name);
    static void purgeCachedData(const char* file = NULL);
    void setDisplayFrameForEntity(HLEntity* entity);
    HLTexture* getTexture();

};

NS_HL_END

#endif
