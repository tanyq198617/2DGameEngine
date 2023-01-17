LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
## This is the name of module the caller will use in LOCAL_STATIC_LIBRARIES
LOCAL_MODULE := icuuc_shared
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libicuuc_53.so
## Use LOCAL_EXPORT_CFLAGS to automatically export the correct flags (as necessary) to the calling module so the caller doesn't need to know the details.
LOCAL_STATIC_LIBRARIES := icudata_shared
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
## This is the name of module the caller will use in LOCAL_STATIC_LIBRARIES
LOCAL_MODULE := icudata_shared
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libicudata_53.so
## Use LOCAL_EXPORT_CFLAGS to automatically export the correct flags (as necessary) to the calling module so the caller doesn't need to know the details.
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
## This is the name of module the caller will use in LOCAL_STATIC_LIBRARIES
LOCAL_MODULE := icui18n_shared
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libicui18n_53.so
## Use LOCAL_EXPORT_CFLAGS to automatically export the correct flags (as necessary) to the calling module so the caller doesn't need to know the details.
LOCAL_STATIC_LIBRARIES := icuuc_shared
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
## This is the name of module the caller will use in LOCAL_STATIC_LIBRARIES
LOCAL_MODULE := icuio_shared
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libicuio_53.so
## Use LOCAL_EXPORT_CFLAGS to automatically export the correct flags (as necessary) to the calling module so the caller doesn't need to know the details.
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
## This is the name of module the caller will use in LOCAL_STATIC_LIBRARIES
LOCAL_MODULE := icule_shared
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libicule_53.so
## Use LOCAL_EXPORT_CFLAGS to automatically export the correct flags (as necessary) to the calling module so the caller doesn't need to know the details.
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
## This is the name of module the caller will use in LOCAL_STATIC_LIBRARIES
LOCAL_MODULE := iculx_shared
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libiculx_53.so
## Use LOCAL_EXPORT_CFLAGS to automatically export the correct flags (as necessary) to the calling module so the caller doesn't need to know the details.
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
## This is the name of module the caller will use in LOCAL_STATIC_LIBRARIES
LOCAL_MODULE := icutu_shared
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libicutu_53.so
## Use LOCAL_EXPORT_CFLAGS to automatically export the correct flags (as necessary) to the calling module so the caller doesn't need to know the details.
include $(PREBUILT_SHARED_LIBRARY)