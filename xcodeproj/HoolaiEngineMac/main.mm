//
//  main.m
//  HoolaiEngineMac
//
//  Created by tyq on 12-9-11.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "HLFileUtil.h"

int main(int argc, char *argv[])
{
    char *fullpath = (char*)malloc(strlen(argv[0])+10);
    hoolai::get_file_path(argv[0], fullpath);
    strcat(fullpath, "../Resources/");
    setenv("FILESYSTEM", fullpath, 1);
    free(fullpath);
    @autoreleasepool {
        return NSApplicationMain(argc, (const char **)argv);
    }
}
