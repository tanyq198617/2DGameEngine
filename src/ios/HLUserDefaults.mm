//
//  HLUserDefaults.mm
//  HoolaiEngine
//
//  Created by tyq on 13-3-19.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLUserDefaults.h"

NS_HL_BEGIN

bool HLUserDefaults::getBoolForKey(const char* key, bool defaultvalue)
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    NSString* str = [NSString stringWithUTF8String:key];
    if ([userDefaults objectForKey:str])
    {
        return [userDefaults boolForKey:str];
    }
    return defaultvalue;
}

int HLUserDefaults::getIntForKey(const char* key, int defaultvalue)
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    NSString* str = [NSString stringWithUTF8String:key];
    if ([userDefaults objectForKey:str])
    {
        return (int)[userDefaults integerForKey:str];
    }
    return defaultvalue;
}

float HLUserDefaults::getFloatForKey(const char* key, float defaultvalue)
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    NSString* str = [NSString stringWithUTF8String:key];
    if ([userDefaults objectForKey:str])
    {
        return [userDefaults floatForKey:str];
    }
    return defaultvalue;
}

std::string HLUserDefaults::getStringForKey(const char* key, std::string defaultvalue)
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    NSString* str = [NSString stringWithUTF8String:key];
    if ([userDefaults objectForKey:str])
    {
        return std::string([[userDefaults stringForKey:str] UTF8String]);
    }
    return defaultvalue;
}

void HLUserDefaults::setBoolForKey(const char* key, bool value)
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    NSString* str = [NSString stringWithUTF8String:key];
    [userDefaults setBool:value forKey:str];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

void HLUserDefaults::setIntForKey(const char* key, int value)
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    NSString* str = [NSString stringWithUTF8String:key];
    [userDefaults setInteger:value forKey:str];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

void HLUserDefaults::setFloatForKey(const char* key, float value)
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    NSString* str = [NSString stringWithUTF8String:key];
    [userDefaults setFloat:value forKey:str];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

void HLUserDefaults::setStringForKey(const char* key, std::string value)
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    NSString* str = [NSString stringWithUTF8String:key];
    NSString* str1 = [NSString stringWithUTF8String:value.c_str()];
    [userDefaults setValue:str1 forKey:str];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

NS_HL_END
