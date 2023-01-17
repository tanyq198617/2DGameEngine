//#include "text_input_node/CCIMEDispatcher.h"
#include "HLDirector2D.h"
#include "HLApplication.h"
#include "HLNotificationCenter.h"
#include "JniHelper.h"
#include <jni.h>

using namespace hoolai;

extern "C" {
    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLRenderer_nativeRender(JNIEnv* env)
    {
        HLDirector2D::getSingleton()->mainLoop();
    }

    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLRenderer_nativeOnPause()
    {
        HLApplication::sharedApplication()->applicationDidEnterBackground();

//        HLNotificationCenter::getInstance()->postNotification(EVENT_COME_TO_BACKGROUND, NULL);
    }

    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLRenderer_nativeOnResume()
    {
        HLApplication::sharedApplication()->applicationWillEnterForeground();
    }

//    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLRenderer_nativeInsertText(JNIEnv* env, jobject thiz, jstring text) {
//        const char* pszText = env->GetStringUTFChars(text, NULL);
////        cocos2d::IMEDispatcher::sharedDispatcher()->dispatchInsertText(pszText, strlen(pszText));
////        env->ReleaseStringUTFChars(text, pszText);
//        if (currEditBox)
//            currEditBox->insertText(pszText);
//    }
//
//    JNIEXPORT void JNICALL Java_com_hoolai_engine_HLRenderer_nativeDeleteBackward(JNIEnv* env, jobject thiz) {
////        cocos2d::IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
//        if (currEditBox)
//            currEditBox->deleteBackward();
//    }
//
//    JNIEXPORT jstring JNICALL Java_com_hoolai_engine_HLRenderer_nativeGetContentText() {
//        JNIEnv * env = 0;
//
//        if (JniHelper::getJavaVM()->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK || ! env) {
//            return 0;
//        }
//        const char * pszText = "";
//        if (currEditBox)
//            pszText = currEditBox->getText().c_str();
//        return env->NewStringUTF(pszText);
//    }
//    
//    JNIEXPORT jstring JNICALL Java_com_hoolai_engine_HLRenderer_textInputResignActive() {
//        if (currEditBox)
//        {
//            hoolai::gui::HLEditBox* editBox = currEditBox;
//            currEditBox = NULL;
//            editBox->resignActive();
//        }
//    }
}
