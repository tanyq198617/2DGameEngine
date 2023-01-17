#include "JniHelper.h"
#include <android/log.h>
#include <string.h>

#ifdef DEBUG
#define  LOG_TAG    "JniHelper"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...) 
#endif

#define JAVAVM    hoolai::JniHelper::getJavaVM()

using namespace std;

extern "C"
{

    //////////////////////////////////////////////////////////////////////////
    // java vm helper function
    //////////////////////////////////////////////////////////////////////////

    static bool getEnv(JNIEnv **env)
    {
        bool bRet = false;

        do 
        {
            if (JAVAVM->GetEnv((void**)env, JNI_VERSION_1_4) != JNI_OK)
            {
                LOGD("Failed to get the environment using GetEnv()");
                break;
            }

            if (JAVAVM->AttachCurrentThread(env, 0) < 0)
            {
                LOGD("Failed to get the environment using AttachCurrentThread()");
                break;
            }

            bRet = true;
        } while (0);        

        return bRet;
    }

    static jclass getClassID_(const char *className, JNIEnv *env)
    {
        JNIEnv *pEnv = env;
        jclass ret = 0;

        do 
        {
            if (! pEnv)
            {
                if (! getEnv(&pEnv))
                {
                    break;
                }
            }
            
            ret = pEnv->FindClass(className);
            if (! ret)
            {
                 LOGD("Failed to find class of %s", className);
                break;
            }
        } while (0);

        return ret;
    }

    static bool getStaticMethodInfo_(hoolai::JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
    {
        jmethodID methodID = 0;
        JNIEnv *pEnv = 0;
        bool bRet = false;

        do 
        {
            if (! getEnv(&pEnv))
            {
                break;
            }

            jclass classID = getClassID_(className, pEnv);

            methodID = pEnv->GetStaticMethodID(classID, methodName, paramCode);
            if (! methodID)
            {
                LOGD("Failed to find static method id of %s", methodName);
                break;
            }

            methodinfo.classID = classID;
            methodinfo.env = pEnv;
            methodinfo.methodID = methodID;

            bRet = true;
        } while (0);

        return bRet;
    }

    static bool getMethodInfo_(hoolai::JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
    {
        jmethodID methodID = 0;
        JNIEnv *pEnv = 0;
        bool bRet = false;

        do 
        {
            if (! getEnv(&pEnv))
            {
                break;
            }

            jclass classID = getClassID_(className, pEnv);

            methodID = pEnv->GetMethodID(classID, methodName, paramCode);
            if (! methodID)
            {
                LOGD("Failed to find method id of %s", methodName);
                break;
            }

            methodinfo.classID = classID;
            methodinfo.env = pEnv;
            methodinfo.methodID = methodID;

            bRet = true;
        } while (0);

        return bRet;
    }
    
    static bool getMethodInfo1_(hoolai::JniMethodInfo &methodinfo, jobject obj, const char *methodName, const char *paramCode)
    {
        jmethodID methodID = 0;
        JNIEnv *pEnv = 0;
        bool bRet = false;
        
        do
        {
            if (! getEnv(&pEnv))
            {
                break;
            }
            
            jclass classID = pEnv->GetObjectClass(obj);
            
            methodID = pEnv->GetMethodID(classID, methodName, paramCode);
            if (! methodID)
            {
                LOGD("Failed to find method id of %s", methodName);
                break;
            }
            
            methodinfo.classID = classID;
            methodinfo.env = pEnv;
            methodinfo.methodID = methodID;
            
            bRet = true;
        } while (0);
        
        return bRet;
    }

    static string jstring2string_(jstring jstr)
    {
        if (jstr == NULL)
        {
            return "";
        }
        
        JNIEnv *env = 0;

        if (! getEnv(&env))
        {
            return 0;
        }

        const char* chars = env->GetStringUTFChars(jstr, NULL);
        string ret(chars);
        env->ReleaseStringUTFChars(jstr, chars);

        return ret;
    }
    
    static bool _setMultiTouchEnabled1(bool b)
    {
        jmethodID methodID = 0;
        JNIEnv *pEnv = 0;
        bool bRet = false;
        
        do
        {
            if (! getEnv(&pEnv))
            {
                break;
            }
            
            jclass classID = getClassID_("com/hoolai/engine/HLGLSurfaceView", pEnv);
            
            jfieldID fid = pEnv->GetStaticFieldID(classID, "mMultiTouchEnabled", "Z");
            
            if (fid == NULL)
            {
                break;
            }
            
            pEnv->SetStaticBooleanField(classID, fid, b);
            bRet = true;
        } while (0);
        return bRet;
    }
}

NS_HL_BEGIN

JavaVM* JniHelper::_psJavaVM = NULL;

JavaVM* JniHelper::getJavaVM()
{
    return _psJavaVM;
}

void JniHelper::setJavaVM(JavaVM *javaVM)
{
    _psJavaVM = javaVM;
}

jclass JniHelper::getClassID(const char *className, JNIEnv *env)
{
    return getClassID_(className, env);
}

bool JniHelper::getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
{
    return getStaticMethodInfo_(methodinfo, className, methodName, paramCode);
}

bool JniHelper::getMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
{
    return getMethodInfo_(methodinfo, className, methodName, paramCode);
}

bool getMethodInfo(JniMethodInfo &methodinfo, jobject obj, const char *methodName, const char *paramCode)
{
    return getMethodInfo1_(methodinfo, obj, methodName, paramCode);
}

string JniHelper::jstring2string(jstring str)
{
    return jstring2string_(str);
}

void _setMultiTouchEnabled(bool enable)
{
    _setMultiTouchEnabled1(enable);
}

NS_HL_END
