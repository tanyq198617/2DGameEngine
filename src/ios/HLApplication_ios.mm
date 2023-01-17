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
#import "EAGLView.h"

namespace hoolai
{
    void openURL(std::string url)
    {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];
    }
}

@interface HLMainLoopCaller : NSObject {
    id displayLink;
    int interval;
}
@property (readwrite) int interval;
-(void) startMainLoop;
-(void) doCaller: (id) sender;
-(void) setAnimationInterval:(double)interval;
+(id) sharedDirectorCaller;
+(void) destroy;
@end

//@interface NSObject(CADisplayLink)
//+(id) displayLinkWithTarget: (id)arg1 selector:(SEL)arg2;
//-(void) addToRunLoop: (id)arg1 forMode: (id)arg2;
//-(void) setFrameInterval: (int)interval;
//-(void) invalidate;
//@end

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

-(void) startMainLoop
{
    // CCDirector::setAnimationInterval() is called, we should invalide it first
    [displayLink invalidate];
    displayLink = nil;
#if GAME_PROJECTION == PROJECTION_2D
    hoolai::HLDirector2D::getSingleton()->startAnimation();
#else
    hoolai::HLDirector3D::sharedDirector()->startAnimation();
#endif
    
    displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(doCaller:)];
    [displayLink setFrameInterval: self.interval];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

-(void) setAnimationInterval:(double)intervalNew
{
    // CCDirector::setAnimationInterval() is called, we should invalide it first
    [displayLink invalidate];
    displayLink = nil;
    
    self.interval = 60.0 * intervalNew;
    
    displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(doCaller:)];
    [displayLink setFrameInterval: self.interval];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
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
    applicationDidFinishLaunching();
    [[HLMainLoopCaller sharedDirectorCaller] startMainLoop];
    return 0;
}

void HLApplication::setAnimationInterval(double interval)
{
    [[HLMainLoopCaller sharedDirectorCaller] setAnimationInterval:interval];
}

void HLApplication::swapBuffers()
{
    [[EAGLView sharedEGLView] swapBuffers];
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
        else if ([languageCode isEqualToString:@"zh"] || [languageCode isEqualToString:@"zh-Hans"])
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
        else if ([languageCode isEqualToString:@"pt"] || [languageCode isEqualToString:@"pt-BR"] || [languageCode isEqualToString:@"pt-PT"]){
            ret = kLanguagePortuguese;
        }
        else if ([languageCode isEqualToString:@"ar"]){
            ret = kLanguageArabic;
        }
        else if ([languageCode isEqualToString:@"tr"] || [languageCode isEqualToString:@"tr-CY"] || [languageCode isEqualToString:@"tr-TR"]){
            ret = kLanguageTurkish;
        }
        else if ([languageCode isEqualToString:@"pl"] || [languageCode isEqualToString:@"pl-PL"]){
            ret = kLanguagePolish;
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
