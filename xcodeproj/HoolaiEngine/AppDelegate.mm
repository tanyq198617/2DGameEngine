//
//  AppDelegate.m
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-4.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import "AppDelegate.h"
#import "EAGLView.h"
#import "RootViewController.h"
#include "AppDelegate.hpp"
#include "HLSpriteFrame.h"
#include "JSScriptingCore.h"

@implementation AppDelegate

@synthesize window = _window, rootViewController = _rootViewController;


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    // Override point for customization after application launch.
    self.window.backgroundColor = [UIColor whiteColor];
    
    RootViewController *controller = [[RootViewController alloc] init];
    controller.supportOrientation = UIInterfaceOrientationMaskPortrait;
    self.rootViewController = self.window.rootViewController = controller;
    [self.window addSubview:self.rootViewController.view];
    [self.window makeKeyAndVisible];
    
    hoolai::HLApplication *app = new hoolai::HLApplication(new ApplicationDelegate());
    app->run();
    
    return YES;
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    hoolai::HLSpriteFrame::purgeCachedData();
    hoolai::JSScriptingCore::forceGC(hoolai::JSScriptingCore::getSingleton()->getGlobalContext(), 0, NULL);
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    hoolai::HLApplication::sharedApplication()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
    hoolai::HLApplication::sharedApplication()->applicationWillEnterForeground();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
