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
    NSString *docPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString* dataPath = [docPath stringByAppendingPathComponent:@"data"];
    addFileSearchPath([dataPath UTF8String]);
    mWritablePath = [dataPath UTF8String];
    mWritablePath += "/";
    if (![[NSFileManager defaultManager] fileExistsAtPath:dataPath])
    {
        [[NSFileManager defaultManager] createDirectoryAtPath:dataPath withIntermediateDirectories:NO attributes:nil error:nil];
        uint8_t attrValue = 1;
        setxattr([dataPath UTF8String], "com.apple.MobileBackup", &attrValue, sizeof(attrValue), 0, 0);
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

const char* HLResourceManager::getBundleFullPath()
{
    static NSString* path = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:@"/"];
    return [path UTF8String];
}

NS_HL_END
