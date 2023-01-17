//
//  HLUserDefaults.h
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-19.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#ifndef __HLUSERDEFAULTS_H__
#define __HLUSERDEFAULTS_H__

#include "HLMacros.h"
#include "HLSingleton.h"
#include <string>

NS_HL_BEGIN

class HLUserDefaults: public HLSingleton<HLUserDefaults>
{
public:
    bool getBoolForKey(const char* key, bool defaultvalue = false);
    int getIntForKey(const char* key, int defaultvalue = 0);
    float getFloatForKey(const char* key, float defaultvalue = 0);
    std::string getStringForKey(const char* key, std::string defaultvalue="");
    
    void setBoolForKey(const char* key, bool value);
    void setIntForKey(const char* key, int value);
    void setFloatForKey(const char* key, float value);
    void setStringForKey(const char* key, std::string value);
};

NS_HL_END

#endif
