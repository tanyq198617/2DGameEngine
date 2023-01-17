//
//  HLResourceManagerMac.mm
//  HoolaiEngine
//
//  Created by tyq on 13-7-16.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLResourceManager.h"
#include <sys/xattr.h>

NS_HL_BEGIN

void HLResourceManager::initResourcePathsNative()
{
    NSString* path = [[NSBundle mainBundle] resourcePath];
    addFileSearchPath([path UTF8String]);
    NSString *docPath = [NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString* dataPath = [[docPath stringByAppendingPathComponent:@"sq"] stringByAppendingPathComponent:@"data"];
    addFileSearchPath([dataPath UTF8String]);
    mWritablePath = [dataPath UTF8String];
    mWritablePath += "/";
    if (![[NSFileManager defaultManager] fileExistsAtPath:dataPath])
    {
        [[NSFileManager defaultManager] createDirectoryAtPath:dataPath withIntermediateDirectories:YES attributes:nil error:nil];
    }
}

HLFileData* HLResourceManager::getFileDataInAppBundle(const char* file)
{
    NSString* path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:[NSString stringWithUTF8String:file]];
    return getFileData([path UTF8String]);
}

bool HLResourceManager::fileInAppBundle(const char* file)
{
    NSString* path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:[NSString stringWithUTF8String:file]];
    return [[NSFileManager defaultManager] fileExistsAtPath:path];
}

NS_HL_END
