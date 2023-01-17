#ifndef __ANDROID_JNI_HELPER_H__
#define __ANDROID_JNI_HELPER_H__

#include <jni.h>
#include <string>
#include "HLMacros.h"

NS_HL_BEGIN

typedef struct JniMethodInfo_
{
    JNIEnv *    env;
    jclass      classID;
    jmethodID   methodID;
} JniMethodInfo;

class JniHelper
{
public:
    static JavaVM* getJavaVM();
    static void setJavaVM(JavaVM *javaVM);
    static jclass getClassID(const char *className, JNIEnv *env=0);
    static bool getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode);
    static bool getMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode);
    static bool getMethodInfo(JniMethodInfo &methodinfo, jobject obj, const char *methodName, const char *paramCode);
    static std::string jstring2string(jstring str);

private:
    static JavaVM *_psJavaVM;
};

NS_HL_END

#endif // __ANDROID_JNI_HELPER_H__
