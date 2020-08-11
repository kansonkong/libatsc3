# Android.mk for libatsc3 Android sample app
#
#
# jjustman@ngbp.org - for libatsc3 inclusion 2019-09-28

# global pathing

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)

# ---------------------------
# pcre2 library

include $(CLEAR_VARS)
LOCAL_MODULE := libpcre
LOCAL_SRC_FILES := libpcre/libs/$(TARGET_ARCH_ABI)/libpcre2.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/libpcre/include
include $(PREBUILT_SHARED_LIBRARY)

# ---------------------------
# libatsc3 jni interface

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
# LOCAL_ALLOW_UNDEFINED_SYMBOLS=true

LOCAL_MODULE := atsc3_core

LIBATSC3C := \
    $(wildcard $(LOCAL_PATH)/../../../src/*.c)

LIBATSC3CPP := \
    $(wildcard $(LOCAL_PATH)/../../../src/*.cpp)

LIBATSC3PHYVIRTUALCPP := \
    $(wildcard $(LOCAL_PATH)/../../../src/phy/virtual*.cpp)

# jjustman-2020-08-10 - temporary - refactor this out...
LOCAL_SRC_FILES += \
    $(LIBATSC3C:$(LOCAL_PATH)/%=%)  \
    $(LIBATSC3CPP:$(LOCAL_PATH)/%=%) \
    $(LIBATSC3PHYVIRTUALCPP:$(LOCAL_PATH)/%=%)

#for libatsc3 application and phy interface includes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/application
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/phy

#for pcre2 include header
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libpcre/include

LOCAL_SHARED_LIBRARIES := libpcre

LOCAL_CFLAGS += -g -fpack-struct=8 -fPIC  \
                -D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__

LOCAL_LDLIBS := -ldl -llog -landroid -lz -lc

include $(BUILD_SHARED_LIBRARY)