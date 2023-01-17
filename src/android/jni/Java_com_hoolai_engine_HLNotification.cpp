//
//  java_com_hoolai_engine_HLNotification.cpp
//  HoolaiEngine
//
//  Created by huangchonggao on 13-10-16.
//  Copyright (c) 2013年 ICT. All rights reserved.
//
#include <jni.h>
#include <android/log.h>
#include <string>
#include "JniHelper.h"
#include "HLNotificationCenter.h"
#include "HLAutoreleasePool.h"
#include "HLObject.h"

#define  CLASS_NAME "com/hoolai/engine/HLNotificationCenter"

using namespace hoolai;

////huang chonggao 2013.9.25
//// jni notifiToEngine call in
//void __notifiToEngine(JNIEnv * env, jobject obj,jstring notiName,jstring pData,jint notiID)
//{
//    
//    std::string strName("");
//    strName= JniHelper::jstring2string(notiName);
//    std::string strPdata("");
//    strPdata= JniHelper::jstring2string(pData);
//    HLLOG("__notifiToEngine %s >> %s",strName.c_str(),strPdata.c_str());
//    HLString *msg=new HLString(strPdata);
//    msg->_ID=notiID;
//    HLNotificationCenter::defaultCenter()->postNotification(strName, msg);
//}

void _nativePostNotification(JNIEnv * env, jobject obj, jstring notiName, jobject msg)
{
    HLAutoreleasePool autoreleasePool;
    std::string strName("");
    strName= JniHelper::jstring2string(notiName);
    HLObject* param = NULL;
    if (msg != NULL)
    {
        jclass strclass = env->FindClass("java/lang/String");
        jclass intclass = env->FindClass("java/lang/Integer");
        jclass doubleclass = env->FindClass("java/lang/Double");
        jclass boolclass = env->FindClass("java/lang/Boolean");
        if (env->IsInstanceOf(msg, strclass))
        {
            HLString* str = new HLString(JniHelper::jstring2string((jstring)msg).c_str());
            str->autorelease();
            param = str;
        }
        else if (env->IsInstanceOf(msg, intclass))
        {
            jmethodID methodid = env->GetMethodID(intclass, "intValue", "()I");
            jint i = env->CallIntMethod(msg, methodid);
            HLNumber* num = new HLNumber(i);
            num->autorelease();
            param = num;
        }
        else if (env->IsInstanceOf(msg, doubleclass))
        {
            jmethodID methodid = env->GetMethodID(intclass, "doubleValue", "()D");
            jdouble i = env->CallDoubleMethod(msg, methodid);
            HLNumber* num = new HLNumber(i);
            num->autorelease();
            param = num;
        }
        else if (env->IsInstanceOf(msg, boolclass))
        {
            jmethodID methodid = env->GetMethodID(intclass, "booleanValue", "()Z");
            jboolean i = env->CallBooleanMethod(msg, methodid);
            HLNumber* num = new HLNumber(i);
            num->autorelease();
            param = num;
        }
        env->DeleteLocalRef(strclass);
        env->DeleteLocalRef(intclass);
        env->DeleteLocalRef(doubleclass);
        env->DeleteLocalRef(boolclass);
    }
    HLNotificationCenter::defaultCenter()->postNotification(strName, param);
}

extern "C"
{
    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLNotificationCenter_nativePostNotification(JNIEnv * env, jclass obj, jstring notiName, jobject msg)
    {
        _nativePostNotification(env, obj, notiName, msg);
    }
}

/*
 * notify java observers
 */
void postNotificationToJava(HLNotification* message)
{
    JniMethodInfo t;
    if (hoolai::JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onNotification", "(Ljava/lang/String;Ljava/lang/Object;)V"))
    {
        jstring notiName = t.env->NewStringUTF(message->name.c_str());
        jobject param = NULL;
        if (message->userInfo)
        {
            switch (message->userInfo->getType())
            {
                case kHLObjectString:
                {
                    param = (jobject)t.env->NewStringUTF(static_cast<HLString*>(message->userInfo)->_value.c_str());
                }
                    break;
                case kHLObjectNumber:
                {
                    HLNumber* num = static_cast<HLNumber*>(message->userInfo);
                    if (num->_numtype == kHLNumberTypeDouble || num->_numtype == kHLNumberTypeFloat)
                    {
                        jclass jintclass = t.env->FindClass("java/lang/Double");
                        jmethodID jmiInit = t.env->GetMethodID(jintclass, "<init>", "(D)V");
                        param = t.env->NewObject(jintclass, jmiInit, num->doubleValue());
                    }
                    else if (num->_numtype == kHLNumberTypeBool)
                    {
                        jclass jintclass = t.env->FindClass("java/lang/Boolean");
                        jmethodID jmiInit = t.env->GetMethodID(jintclass, "<init>", "(Z)V");
                        param = t.env->NewObject(jintclass, jmiInit, num->boolValue());
                    }
                    else if (num->_numtype == kHLNumberTypeInt)
                    {
                        jclass jintclass = t.env->FindClass("java/lang/Integer");
                        jmethodID jmiInit = t.env->GetMethodID(jintclass, "<init>", "(I)V");
                        param = t.env->NewObject(jintclass, jmiInit, num->intValue());
                    }
                }
                    break;
                    // TODO map and array
                default:
                    break;
            }
        }
        t.env->CallStaticVoidMethod(t.classID, t.methodID, notiName, param);
        if (param)
        {
            t.env->DeleteLocalRef(param);
        }
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(notiName);
    }
}

