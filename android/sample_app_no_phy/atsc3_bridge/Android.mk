# Android.mk for libatsc3 Android sample app
#
#
# jjustman@ngbp.org - for libatsc3 inclusion 2019-09-28

# global pathing

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)

# import our prefab build from core
$(call import-module,prefab/libatsc3_core)

# ---------------------------
# libatsc3_bridge jni interface

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
# LOCAL_ALLOW_UNDEFINED_SYMBOLS=true

LOCAL_MODULE := atsc3_bridge

LIBATSC3COREBRIDGECPP := \
    $(wildcard $(LOCAL_PATH)/src/cpp/*.cpp)

LIBATSC3JNIBRIDGECPP := \
    $(wildcard $(LOCAL_PATH)/src/jni/*.cpp)

# jjustman-2020-08-10 - temporary - refactor this out...
LOCAL_SRC_FILES += \
    $(LIBATSC3COREBRIDGECPP:$(LOCAL_PATH)/%=%)  \
    $(LIBATSC3JNIBRIDGECPP:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/jni
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/

# jjustman-2020-08-10 - hack-ish...
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libatsc3_core/libpcre/include

LOCAL_CFLAGS += -g -fpack-struct=8 -fPIC  \
                -D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__

LOCAL_LDLIBS := -ldl -llog -landroid -lz

# jjustman-2020-08-10 - link in our atsc3_core prefab shared library
LOCAL_SHARED_LIBRARIES := atsc3_core

include $(BUILD_SHARED_LIBRARY)



