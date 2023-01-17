//
//  RootViewController.m
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-23.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#import "RootViewController.h"
#import "EAGLView.h"
#include "HLDirector2D.h"
#include "HLGeometry.h"
#include "HLDirector2D.h"
#include "GameEngineConfig.h"

@interface RootViewController ()

@end

@implementation RootViewController

@synthesize supportOrientation, shouldAutoRotate, supportsRetina;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        supportOrientation = UIInterfaceOrientationMaskPortrait;
        shouldAutoRotate = YES;
        supportsRetina = YES;
        if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 5.0f)
        {
            [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillChangeFrame:) name:UIKeyboardWillChangeFrameNotification object:nil];
            [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardDidChangeFrame:) name:UIKeyboardDidChangeFrameNotification object:nil];
        }
    }
    return self;
}

- (void)loadView
{
    EAGLView *__glView = [[EAGLView alloc ] initWithFrame: [[UIApplication sharedApplication].delegate window].bounds
                                              pixelFormat: kEAGLColorFormatRGB565
                                              depthFormat: GL_DEPTH24_STENCIL8_OES
                                       preserveBackbuffer: NO
                                               sharegroup: nil
                                            multiSampling: NO
                                          numberOfSamples: 0
                                           supportsRetina:supportsRetina];
    [self setView:__glView];
    
    float width = __glView.bounds.size.width * [__glView contentScaleFactor];
    float height = __glView.bounds.size.height * [__glView contentScaleFactor];
    if (supportOrientation&(UIInterfaceOrientationMaskPortrait|UIInterfaceOrientationMaskPortraitUpsideDown)) {
        hoolai::HLSize winSize;
        winSize.width = MIN(width, height);
        winSize.height = MAX(width, height);
        hoolai::HLDirector2D::getSingleton()->reshapeProjection(winSize);
    }
    else
    {
        hoolai::HLSize winSize;
        winSize.height = MIN(width, height);
        winSize.width = MAX(width, height);
        hoolai::HLDirector2D::getSingleton()->reshapeProjection(winSize);
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotate
{
    return shouldAutoRotate;
}

- (NSUInteger)supportedInterfaceOrientations
{
    return supportOrientation;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    return (supportOrientation&UIInterfaceOrientationMaskPortrait && toInterfaceOrientation == UIDeviceOrientationPortrait) ||
            (supportOrientation&UIInterfaceOrientationMaskLandscapeLeft && toInterfaceOrientation == UIDeviceOrientationLandscapeLeft) ||
            (supportOrientation&UIInterfaceOrientationMaskLandscapeRight && toInterfaceOrientation == UIDeviceOrientationLandscapeRight) ||
            (supportOrientation&UIInterfaceOrientationMaskPortraitUpsideDown && toInterfaceOrientation == UIDeviceOrientationPortraitUpsideDown);

}

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

- (void)keyboardWillChangeFrame:(NSNotification*)aNotification
{
    hoolai::HLDirector2D::getSingleton()->pause();
}

- (void)keyboardDidChangeFrame:(NSNotification*)aNotification
{
    hoolai::HLDirector2D::getSingleton()->resume();
}

@end
