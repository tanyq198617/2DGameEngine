//
//  HLApplication.h
//  HoolaiEngine2D
//
//  Created by tyq on 12-5-23.
//  Copyright (c) 2012 ICT. All rights reserved.
//

#ifndef __HLAPPLICATION_H__
#define __HLAPPLICATION_H__

#include "HLMacros.h"
#include <vector>

NS_HL_BEGIN

/**
 @brief Enum the language type supported now
 */
typedef enum LanguageType
{
    kLanguageEnglish = 0,
    kLanguageChineseTaiwan,
    kLanguageChinese,
    kLanguageFrench,
    kLanguageItalian,
    kLanguageGerman,
    kLanguageSpanish,
    kLanguageRussian,
    kLanguageKorean,
    kLanguageJapanese,
    kLanguageHungarian,
    kLanguagePortuguese,
    kLanguageArabic,
    kLanguageTurkish,
    kLanguagePolish,
    kLanguageUnknown = 9999
} HLLanguageType;

class HLApplicationDelegate
{
public:
    virtual ~HLApplicationDelegate() {}
    virtual void applicationDidFinishLaunching() = 0;
    virtual void applicationDidEnterBackground() = 0;
    virtual void applicationWillEnterForeground() = 0;
    virtual void applicationWillTerminate() = 0;
};

class HLApplication
{
private:
    HLApplicationDelegate* _delegate;
    std::vector<HLLanguageType> _supportLanguages;
    HLLanguageType _currLanguage;
public:
    HLApplication(HLApplicationDelegate *delegate);
    ~HLApplication();
    void applicationDidFinishLaunching();
    void applicationDidEnterBackground();
    void applicationWillEnterForeground();
    void applicationWillTerminate();
    static HLApplication* sharedApplication();
    int run();
    void setAnimationInterval(double interval);
    void swapBuffers();
    HLLanguageType getCurrentLanguage();
    void setSupportLanguages(std::vector<HLLanguageType> languages) {_supportLanguages = languages;}
};

NS_HL_END

#endif
