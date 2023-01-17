#ifndef __ANDROID_IME_JNI_H__
#define __ANDROID_IME_JNI_H__

extern "C" {
    extern void setKeyboardStateJNI(int bOpen);
    extern void openKeyboardJNI();
    extern void closeKeyboardJNI();
}

#endif // __ANDROID_IME_JNI_H__
