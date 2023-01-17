#ifndef __Java_com_hoolai_Engine_HLHelper_H__
#define __Java_com_hoolai_Engine_HLHelper_H__

#include <string>

extern void terminateProcessJNI();
extern std::string getCurrentLanguageJNI();
extern std::string getPackageNameJNI();
// functions for UserDefault
extern bool getBoolForKeyJNI(const char* pKey, bool defaultValue);
extern int getIntegerForKeyJNI(const char* pKey, int defaultValue);
extern float getFloatForKeyJNI(const char* pKey, float defaultValue);
extern double getDoubleForKeyJNI(const char* pKey, double defaultValue);
extern std::string getStringForKeyJNI(const char* pKey, const char* defaultValue);
extern void setBoolForKeyJNI(const char* pKey, bool value);
extern void setIntegerForKeyJNI(const char* pKey, int value);
extern void setFloatForKeyJNI(const char* pKey, float value);
extern void setDoubleForKeyJNI(const char* pKey, double value);
extern void setStringForKeyJNI(const char* pKey, const char* value);
extern std::string getCurrentLanguageJNI();
extern int getNetWorkTypeFormJNI();
extern void installApkJNI(const char* pUrl);
#endif /* __Java_com_hoolai_Engine_HLHelper_H__ */
