//
//  HLApplication_ios.mm
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-23.
//  Copyright (c) 2012年 ICT. All rights reserved.
//

#include "HLApplication.h"
#include "GameEngineConfig.h"
#if GAME_PROJECTION == PROJECTION_2D
#include "HLDirector2D.h"
#else
#include "HLDirector3D.h"
#endif
#include "HLAutoreleasePool.h"
#import "HLGLView.h"

namespace hoolai
{
    void openURL(std::string url)
    {
        [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];
    }
}

NSThread *runningThread = 0;

@interface HLMainLoopCaller : NSObject {
    CVDisplayLinkRef displayLink;
    int interval;
}
@property (readwrite) int interval;
-(void) startMainLoop;
-(void) doCaller: (id) sender;
-(void) setAnimationInterval:(double)interval;
+(id) sharedDirectorCaller;
+(void) destroy;
@end

@interface NSObject(CADisplayLink)
+(id) displayLinkWithTarget: (id)arg1 selector:(SEL)arg2;
-(void) addToRunLoop: (id)arg1 forMode: (id)arg2;
-(void) setFrameInterval: (int)interval;
-(void) invalidate;
@end

static id s_sharedDirectorCaller;

@implementation HLMainLoopCaller

@synthesize interval;

+(id) sharedDirectorCaller
{
	if (s_sharedDirectorCaller == nil)
	{
		s_sharedDirectorCaller = [HLMainLoopCaller new];
	}
	
	return s_sharedDirectorCaller;
}

+(void) destroy
{
	s_sharedDirectorCaller  = nil;
}

-(void) alloc
{
    interval = 1;
}

// This is the renderer output callback function
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    static hoolai::HLAutoreleasePool autoreleasePool;
    @autoreleasepool {
        [[HLGLView sharedGLView] lockOpenGLContext];
        if (!runningThread)
        {
            runningThread = [NSThread currentThread];
            hoolai::HLApplication::sharedApplication()->applicationDidFinishLaunching();
        }
        [(__bridge HLMainLoopCaller*)displayLinkContext doCaller:nil];
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:nil];
        [[HLGLView sharedGLView] unlockOpenGLContext];
    }
    return kCVReturnSuccess;
}

-(void) startMainLoop
{
    // CCDirector::setAnimationInterval() is called, we should invalide it first
    if( displayLink ) {
		CVDisplayLinkStop(displayLink);
		CVDisplayLinkRelease(displayLink);
		displayLink = NULL;
    }
    
#if GAME_PROJECTION == PROJECTION_2D
    hoolai::HLDirector2D::getSingleton()->startAnimation();
#else
    hoolai::HLDirector3D::sharedDirector()->startAnimation();
#endif
    
	// Create a display link capable of being used with all active displays
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
	// Set the renderer output callback function
	CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, (__bridge void*)self);
    HLGLView *glView = [HLGLView sharedGLView];
    CGLContextObj cglContext = (CGLContextObj)[[glView openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[glView pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    
    // Activate the display link
	CVDisplayLinkStart(displayLink);
}

-(void) setAnimationInterval:(double)intervalNew
{
//    // CCDirector::setAnimationInterval() is called, we should invalide it first
//    [displayLink invalidate];
//    displayLink = nil;
//    
//    self.interval = 60.0 * intervalNew;
//    
//    displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(doCaller:)];
//    [displayLink setFrameInterval: self.interval];
//    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

-(void) doCaller: (id) sender
{
#if GAME_PROJECTION == PROJECTION_2D
    hoolai::HLDirector2D::getSingleton()->mainLoop();
#else
    hoolai::HLDirector3D::sharedDirector()->mainLoop();
#endif
}

@end

NS_HL_BEGIN

int HLApplication::run()
{
    static HLAutoreleasePool autoreleasePool;
    [[HLMainLoopCaller sharedDirectorCaller] startMainLoop];
    return 0;
}

void HLApplication::setAnimationInterval(double interval)
{
    [[HLMainLoopCaller sharedDirectorCaller] setAnimationInterval:interval];
}

void HLApplication::swapBuffers()
{
    [[[HLGLView sharedGLView] openGLContext] flushBuffer];
}

HLLanguageType HLApplication::getCurrentLanguage()
{
    if (_currLanguage == kLanguageUnknown)
    {
        // get the current language and country config
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
        NSString *currentLanguage = [languages objectAtIndex:0];
        
        // get the current language code.(such as English is "en", Chinese is "zh" and so on)
        NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
        NSString * languageCode = [temp objectForKey:NSLocaleLanguageCode];
        
        HLLanguageType ret = kLanguageEnglish;
        if ([currentLanguage isEqualToString:@"zh-Hant"])
        {
            ret = kLanguageChineseTaiwan;
        }
        else if ([languageCode isEqualToString:@"zh"])
        {
            ret = kLanguageChinese;
        }
        else if ([languageCode isEqualToString:@"en"])
        {
            ret = kLanguageEnglish;
        }
        else if ([languageCode isEqualToString:@"fr"]){
            ret = kLanguageFrench;
        }
        else if ([languageCode isEqualToString:@"it"]){
            ret = kLanguageItalian;
        }
        else if ([languageCode isEqualToString:@"de"]){
            ret = kLanguageGerman;
        }
        else if ([languageCode isEqualToString:@"es"]){
            ret = kLanguageSpanish;
        }
        else if ([languageCode isEqualToString:@"ru"]){
            ret = kLanguageRussian;
        }
        else if ([languageCode isEqualToString:@"ko"]){
            ret = kLanguageKorean;
        }
        else if ([languageCode isEqualToString:@"ja"]){
            ret = kLanguageJapanese;
        }
        else if ([languageCode isEqualToString:@"hu"]){
            ret = kLanguageHungarian;
        }
        else if ([languageCode isEqualToString:@"pt"])
        {
            ret = kLanguagePortuguese;
        }
        else if ([languageCode isEqualToString:@"ar"])
        {
            ret = kLanguageArabic;
        }
        
        if (_supportLanguages.size() > 0)
        {
            if (std::find(_supportLanguages.begin(), _supportLanguages.end(), ret) != _supportLanguages.end())
            {
                _currLanguage = ret;
            }
            else
            {
                _currLanguage = _supportLanguages[0];
            }
        }
        else
        {
            _currLanguage = ret;
        }
    }
    return _currLanguage;
}

NS_HL_END
