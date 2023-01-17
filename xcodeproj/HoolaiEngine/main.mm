//
//  main.m
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-4.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "AppDelegate.h"
#include "HLFileUtil.h"

int main(int argc, char *argv[])
{
    char *fullpath = (char*)malloc(strlen(argv[0])+1);
    hoolai::get_file_path(argv[0], fullpath);
    setenv("FILESYSTEM", fullpath, 1);
    free(fullpath);
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
