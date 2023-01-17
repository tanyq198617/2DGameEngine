//
//  HLUserDefaults.mm
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-19.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLUserDefaults.h"
#include "jni/Java_com_hoolai_engine_HLHelper.h"

NS_HL_BEGIN

bool HLUserDefaults::getBoolForKey(const char* key, bool defaultvalue)
{
    return getBoolForKeyJNI(key, defaultvalue);
}

int HLUserDefaults::getIntForKey(const char* key, int defaultvalue)
{
    return getIntegerForKeyJNI(key, defaultvalue);
}

float HLUserDefaults::getFloatForKey(const char* key, float defaultvalue)
{
    return getFloatForKeyJNI(key, defaultvalue);
}

std::string HLUserDefaults::getStringForKey(const char* key, std::string defaultvalue)
{
    return getStringForKeyJNI(key, defaultvalue.c_str());
}

void HLUserDefaults::setBoolForKey(const char* key, bool value)
{
    setBoolForKeyJNI(key, value);
}

void HLUserDefaults::setIntForKey(const char* key, int value)
{
    setIntegerForKeyJNI(key, value);
}

void HLUserDefaults::setFloatForKey(const char* key, float value)
{
    setFloatForKeyJNI(key, value);
}

void HLUserDefaults::setStringForKey(const char* key, std::string value)
{
    setStringForKeyJNI(key, value.c_str());
}

NS_HL_END