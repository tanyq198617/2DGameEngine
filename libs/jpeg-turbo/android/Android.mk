LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := jpeg_static
LOCAL_MODULE_FILENAME := jpeg
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libjpeg.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := jpeg_turbo_static
LOCAL_MODULE_FILENAME := jpeg_turbo
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libturbojpeg.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)
