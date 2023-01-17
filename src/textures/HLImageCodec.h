//
//  HLImageCodec.h
//  HoolaiEngine
//
//  Created by tyq on 13-3-25.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLIMAGECODEC_H__
#define __HLIMAGECODEC_H__

#include "HLTexture.h"

NS_HL_BEGIN

struct MipmapPtr {
    unsigned char *address;
    unsigned int len;
};

class HLImageCodec
{
public:
    virtual ~HLImageCodec() {}
    HLTexturePixelFormat format;
    std::vector<MipmapPtr> mipmaps;
    unsigned int width;
    unsigned int height;
    bool hasPremultipliedAlpha;
    virtual bool loadWithFile(std::string file) {return false;}
};

NS_HL_END

#endif
