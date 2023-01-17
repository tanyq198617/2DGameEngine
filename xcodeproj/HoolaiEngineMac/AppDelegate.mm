//
//  AppDelegate.m
//  HoolaiEngineMac
//
//  Created by tyq on 12-9-11.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import "AppDelegate.h"
#include "AppDelegate.hpp"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [self.window center];

    hoolai::HLApplication *app = new hoolai::HLApplication(new ApplicationDelegate());
    app->run();
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed: (NSApplication *) theApplication
{
	return YES;
}

@end
