#include "IMEJni.h"
#include "JniHelper.h"

#include <android/log.h>
#include <string.h>
#include <jni.h>

using namespace hoolai;

extern "C" {
    void setKeyboardStateJNI(int bOpen) {
        if (bOpen) {
            openKeyboardJNI();
        } else {
            closeKeyboardJNI();
        }
    }

    void openKeyboardJNI() {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, "com/hoolai/engine/HLGLSurfaceView", "openIMEKeyboard", "()V")) {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }

    void closeKeyboardJNI() {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, "com/hoolai/engine/HLGLSurfaceView", "closeIMEKeyboard", "()V")) {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
    }
}
