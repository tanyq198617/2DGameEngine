//
//  AppDelegate.h
//  HoolaiEngineMac
//
//  Created by tyq on 12-9-11.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "HLGLView.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet HLGLView *glView;

@end
