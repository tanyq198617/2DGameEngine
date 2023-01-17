LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
## This is the name of module the caller will use in LOCAL_STATIC_LIBRARIES
LOCAL_MODULE := JavaScriptCore_static
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libJavaScriptCore.a
## Use LOCAL_EXPORT_CFLAGS to automatically export the correct flags (as necessary) to the calling module so the caller doesn't need to know the details.
LOCAL_EXPORT_CFLAGS := -DALMIXER_COMPILE_WITHOUT_SDL -DENABLE_ALMIXER_THREADS
## Since the .a isn't linked, it's link dependencies must be passed on to the calling project.
LOCAL_EXPORT_LDLIBS := -lm
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)