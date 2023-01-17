#include "HLApplication.h"
#include "HLDirector2D.h"
#include "HLAutoreleasePool.h"
#include "jni/JniHelper.h"
#include "jni/Java_com_hoolai_engine_HLHelper.h"
#include <jni.h>

#include <EGL/egl.h>
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT = 0;
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT = 0;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT = 0;

NS_HL_BEGIN

void openURL(std::string url)
{
}

int HLApplication::run()
{
    glGenVertexArraysOESEXT = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
    glBindVertexArrayOESEXT = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
    glDeleteVertexArraysOESEXT = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
    
    static HLAutoreleasePool autoreleasePool;
    applicationDidFinishLaunching();
    return 0;
}

void HLApplication::setAnimationInterval(double interval)
{

}

void HLApplication::swapBuffers()
{
}

HLLanguageType HLApplication::getCurrentLanguage()
{
    if (_currLanguage == kLanguageUnknown)
    {
        std::string languageName = getCurrentLanguageJNI();
        const char* pLanguageName = languageName.c_str();
        HLLanguageType ret = kLanguageEnglish;
        
        if (0 == strcmp("zh", pLanguageName))
        {
            ret = kLanguageChinese;
        }
        else if (0 == strcmp("en", pLanguageName))
        {
            ret = kLanguageEnglish;
        }
        else if (0 == strcmp("fr", pLanguageName))
        {
            ret = kLanguageFrench;
        }
        else if (0 == strcmp("it", pLanguageName))
        {
            ret = kLanguageItalian;
        }
        else if (0 == strcmp("de", pLanguageName))
        {
            ret = kLanguageGerman;
        }
        else if (0 == strcmp("es", pLanguageName))
        {
            ret = kLanguageSpanish;
        }
        else if (0 == strcmp("ru", pLanguageName))
        {
            ret = kLanguageRussian;
        }
        else if (0 == strcmp("ko", pLanguageName))
        {
            ret = kLanguageKorean;
        }
        else if (0 == strcmp("ja", pLanguageName))
        {
            ret = kLanguageJapanese;
        }
        else if (0 == strcmp("hu", pLanguageName))
        {
            ret = kLanguageHungarian;
        }
        else if (0 == strcmp("pt", pLanguageName))
        {
            ret = kLanguagePortuguese;
        }
        else if (0 == strcmp("ar", pLanguageName))
        {
            ret = kLanguageArabic;
        }
        else if (0 == strcmp("tr", pLanguageName))
        {
            ret = kLanguageTurkish;
        }
        else if (0 == strcmp("pl", pLanguageName))
        {
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
