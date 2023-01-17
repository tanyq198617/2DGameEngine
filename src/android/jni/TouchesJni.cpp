#include <android/log.h>
#include <jni.h>
#include "HLDirector2D.h"
#include "HLTouchDispatcher.h"

extern "C" {
    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLRenderer_nativeTouchesBegin(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
    {
        hoolai::HLTouch tmp;
        std::vector<hoolai::HLTouch> aTouches;
        
        int size = env->GetArrayLength(ids);
        jint id[size];
        jfloat x[size];
        jfloat y[size];
        
        env->GetIntArrayRegion(ids, 0, size, id);
        env->GetFloatArrayRegion(xs, 0, size, x);
        env->GetFloatArrayRegion(ys, 0, size, y);
        for (int i = 0; i < size; ++i)
        {
            hoolai::HLPoint p1;
            p1.x = x[i];
            p1.y = y[i];
            tmp.location = hoolai::HLDirector2D::getSingleton()->convertToGLPoint(p1);
            tmp._id = id[i];
            tmp.tapCount = 1;
            aTouches.push_back(tmp);
        }
        hoolai::HLTouchDispatcher::sharedTouchDispatcher()->touchesBegin(aTouches);
    }

    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLRenderer_nativeTouchesEnd(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
    {
        hoolai::HLTouch tmp;
        std::vector<hoolai::HLTouch> aTouches;
        
        int size = env->GetArrayLength(ids);
        jint id[size];
        jfloat x[size];
        jfloat y[size];
        
        env->GetIntArrayRegion(ids, 0, size, id);
        env->GetFloatArrayRegion(xs, 0, size, x);
        env->GetFloatArrayRegion(ys, 0, size, y);
        for (int i = 0; i < size; ++i)
        {
            hoolai::HLPoint p1;
            p1.x = x[i];
            p1.y = y[i];
            tmp.location = hoolai::HLDirector2D::getSingleton()->convertToGLPoint(p1);
            tmp._id = id[i];
            tmp.tapCount = 1;
            aTouches.push_back(tmp);
        }
        hoolai::HLTouchDispatcher::sharedTouchDispatcher()->touchesEnd(aTouches);
    }

    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLRenderer_nativeTouchesMove(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
    {
        hoolai::HLTouch tmp;
        std::vector<hoolai::HLTouch> aTouches;
        
        int size = env->GetArrayLength(ids);
        jint id[size];
        jfloat x[size];
        jfloat y[size];
        
        env->GetIntArrayRegion(ids, 0, size, id);
        env->GetFloatArrayRegion(xs, 0, size, x);
        env->GetFloatArrayRegion(ys, 0, size, y);
        for (int i = 0; i < size; ++i)
        {
            hoolai::HLPoint p1;
            p1.x = x[i];
            p1.y = y[i];
            tmp.location = hoolai::HLDirector2D::getSingleton()->convertToGLPoint(p1);
            tmp._id = id[i];
            tmp.tapCount = 1;
            aTouches.push_back(tmp);
        }
        hoolai::HLTouchDispatcher::sharedTouchDispatcher()->touchesMove(aTouches);
    }

    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLRenderer_nativeTouchesCancel(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
    {
        hoolai::HLTouch tmp;
        std::vector<hoolai::HLTouch> aTouches;
        
        int size = env->GetArrayLength(ids);
        jint id[size];
        jfloat x[size];
        jfloat y[size];
        
        env->GetIntArrayRegion(ids, 0, size, id);
        env->GetFloatArrayRegion(xs, 0, size, x);
        env->GetFloatArrayRegion(ys, 0, size, y);
        for (int i = 0; i < size; ++i)
        {
            hoolai::HLPoint p1;
            p1.x = x[i];
            p1.y = y[i];
            tmp.location = hoolai::HLDirector2D::getSingleton()->convertToGLPoint(p1);
            tmp._id = id[i];
            tmp.tapCount = 1;
            aTouches.push_back(tmp);
        }
        hoolai::HLTouchDispatcher::sharedTouchDispatcher()->touchesCancel(aTouches);
    }
}
