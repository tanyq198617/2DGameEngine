#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include <string>
#include "JniHelper.h"
#include "Java_com_hoolai_Engine_HLHelper.h"
#include "HLEditBox.h"
#include "HLNotificationCenter.h"
#include "HLObject.h"

#define  LOG_TAG    "Java_com_hoolai_Engine_HLHelper.cpp"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#define  CLASS_NAME "com/hoolai/engine/HLHelper"

using namespace hoolai;
using namespace std;

hoolai::gui::HLEditBox* currEditBox;

void __activeTextInputWithEditBox(hoolai::gui::HLEditBox* editBox)
{
    if (currEditBox)
        currEditBox->resignActive();
    
    hoolai::JniMethodInfo t;
    if (hoolai::JniHelper::getStaticMethodInfo(t, CLASS_NAME, "showEditTextDialog", "(Ljava/lang/String;Ljava/lang/String;IIIIZLjava/lang/String;)V"))
    {
        currEditBox = editBox;
        
        jstring stringArg1 = t.env->NewStringUTF(currEditBox->getPlaceHolder().c_str());
        jstring stringArg2 = t.env->NewStringUTF(currEditBox->getText().c_str());
        jstring stringArg3 = t.env->NewStringUTF(currEditBox->getCharInputFilter());
        
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2, (int)currEditBox->getInputMode(), currEditBox->isSecureInput()?0:1, (int)currEditBox->getKeyboardReturnType(), currEditBox->getMaxLength(), currEditBox->allowMultiLine(), stringArg3);
        
        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(stringArg3);
        t.env->DeleteLocalRef(t.classID);
    }
}

void __deactiveTextInput()
{
    if (currEditBox)
    {
        currEditBox->resignActive();
        currEditBox = NULL;
    }
}


/*
 *huang chonggao 
 * get network connect state 
 * -1 is not connect
 *   -1无法获取  0无知网络  1 wifi  2 2G网络 3 3G网络 4其他网络
 */
int _netWorkState()
{
    JniMethodInfo t;
    if (hoolai::JniHelper::getStaticMethodInfo(t, "com/hoolai/engine/HLConnectUtil", "getNetWorkType", "()I"))
    {
       jint  jitype = t.env->CallStaticIntMethod(t.classID, t.methodID);
        return jitype;
    }
    return -1;
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLHelper_nativeSetEditTextDialogResult(JNIEnv * env, jobject obj, jbyteArray text, jboolean shouldreturn)
    {
        jsize  size = env->GetArrayLength(text);
        if (!currEditBox)
            return;
        hoolai::gui::HLEditBox* editBox = currEditBox;
        currEditBox = NULL;
        if (size > 0) {
            jbyte * data = (jbyte*)env->GetByteArrayElements(text, 0);
            char* pBuf = (char*)malloc(size+1);
            if (pBuf != NULL) {
                memcpy(pBuf, data, size);
                pBuf[size] = '\0';
                // pass data to edittext's delegate
                editBox->setText(pBuf);
                free(pBuf);
            }
            env->ReleaseByteArrayElements(text, data, 0);
        } else {
            editBox->setText("");
        }
        if (shouldreturn)
        {
            editBox->onReturn(editBox);
        }
        editBox->resignActive();
    }
    
    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLHelper_nativeDeactiveTextInput(JNIEnv * env, jobject obj)
    {
        __deactiveTextInput();
    }
      

}

static void* s_ctx = NULL;



void terminateProcessJNI()
{
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "terminateProcess", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

std::string getPackageNameJNI()
{
    JniMethodInfo t;
    std::string ret("");

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getPackageName", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }
    return ret;
}

std::string getCurrentLanguageJNI()
{
    JniMethodInfo t;
    std::string ret("");
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getCurrentLanguage", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }

    return ret;
}

// functions for UserDefault
bool getBoolForKeyJNI(const char* pKey, bool defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getBoolForKey", "(Ljava/lang/String;Z)Z")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, stringArg, defaultValue);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

int getIntegerForKeyJNI(const char* pKey, int defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getIntegerForKey", "(Ljava/lang/String;I)I")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jint ret = t.env->CallStaticIntMethod(t.classID, t.methodID, stringArg, defaultValue);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

float getFloatForKeyJNI(const char* pKey, float defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getFloatForKey", "(Ljava/lang/String;F)F")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jfloat ret = t.env->CallStaticFloatMethod(t.classID, t.methodID, stringArg, defaultValue);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

double getDoubleForKeyJNI(const char* pKey, double defaultValue)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getDoubleForKey", "(Ljava/lang/String;D)D")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        jdouble ret = t.env->CallStaticDoubleMethod(t.classID, t.methodID, stringArg);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
        
        return ret;
    }
    
    return defaultValue;
}

std::string getStringForKeyJNI(const char* pKey, const char* defaultValue)
{
    JniMethodInfo t;
    std::string ret("");

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getStringForKey", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;")) {
        jstring stringArg1 = t.env->NewStringUTF(pKey);
        jstring stringArg2 = t.env->NewStringUTF(defaultValue);
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, stringArg1, stringArg2);
        ret = JniHelper::jstring2string(str);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
        t.env->DeleteLocalRef(str);
        
        return ret;
    }
    
    return defaultValue;
}

void setBoolForKeyJNI(const char* pKey, bool value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setBoolForKey", "(Ljava/lang/String;Z)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setIntegerForKeyJNI(const char* pKey, int value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setIntegerForKey", "(Ljava/lang/String;I)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setFloatForKeyJNI(const char* pKey, float value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setFloatForKey", "(Ljava/lang/String;F)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setDoubleForKeyJNI(const char* pKey, double value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setDoubleForKey", "(Ljava/lang/String;D)V")) {
        jstring stringArg = t.env->NewStringUTF(pKey);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

void setStringForKeyJNI(const char* pKey, const char* value)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setStringForKey", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring stringArg1 = t.env->NewStringUTF(pKey);
        jstring stringArg2 = t.env->NewStringUTF(value);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg1);
        t.env->DeleteLocalRef(stringArg2);
    }
}

//安装apk
void installApkJNI(const char* pUrl)
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "installApk", "(Ljava/lang/String;)V")) {
        jstring stringArg = t.env->NewStringUTF(pUrl);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg);
        
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(stringArg);
    }
}

/**
 huang chonggao
* -1无法获取  0无知网络  1 wifi  2 2G网络 3 3G网络 4其他网络
*/
int getNetWorkTypeFormJNI()
{
    return _netWorkState();
}