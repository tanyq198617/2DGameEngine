//
//  HLApkTools.h
//  HoolaiEngine
//
//  Created by liuxiaomeng on 13-10-16.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HoolaiEngine__HLApkTools__
#define __HoolaiEngine__HLApkTools__

#include "HLMacros.h"
#include "HLSingleton.h"

NS_HL_BEGIN

class HLApkTools: public HLSingleton<HLApkTools>
{
public:
    void installApk(const char* url);
};

NS_HL_END


#endif /* defined(__HoolaiEngine__HLApkTools__) */
