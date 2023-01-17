//
//  HLImagePVRCodec.h
//  HoolaiEngine
//
//  Created by tyq on 13-3-25.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#ifndef __HLIMAGEPVRCODEC_H__
#define __HLIMAGEPVRCODEC_H__

#include "HLImageCodec.h"

NS_HL_BEGIN

class HLImagePVRCodec: public HLImageCodec
{
private:
    unsigned char* mData;
public:
    HLImagePVRCodec():mData(NULL) {}
    virtual ~HLImagePVRCodec();
    virtual bool loadWithFile(std::string file);
};

NS_HL_END

#endif
