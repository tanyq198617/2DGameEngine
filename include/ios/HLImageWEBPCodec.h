//
//  HLImageWEBPCodec.h
//  HoolaiEngine
//
//  Created by zs_hoolai on 13-8-10.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLIMAGEWEBPCODEC_H__
#define __HLIMAGEWEBPCODEC_H__

#include "HLImageCodec.h"

NS_HL_BEGIN

class HLImageWEBPCodec: public HLImageCodec
{
private:
    unsigned char* mData;
public:
    HLImageWEBPCodec():mData(NULL) {}
    virtual ~HLImageWEBPCodec();
    virtual bool loadWithFile(std::string file);
};

NS_HL_END

#endif
