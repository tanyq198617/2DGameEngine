//
//  HLApkTools.cpp
//  HoolaiEngine
//
//  Created by liuxiaomeng on 13-10-16.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLApkTools.h"
#include "HLResourceManager.h"
#include "jni/Java_com_hoolai_engine_HLHelper.h"

NS_HL_BEGIN

void HLApkTools::installApk(const char* url)
{
    const char* fullpath = HLResourceManager::getSingleton()->getSysPathForFile(url);
    installApkJNI(fullpath);
}

NS_HL_END