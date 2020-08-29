# Android.mk for libatsc3 Android sample app
#
#
# jjustman@ngbp.org - for libatsc3 inclusion 2019-09-28

# global pathing

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)


# libatsc3 jni interface

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
# LOCAL_ALLOW_UNDEFINED_SYMBOLS=true

LOCAL_MODULE := atsc3NdkClient

LOCAL_SRC_FILES := $(LOCAL_PATH)/src/jni/Atsc3NdkClient.cpp

LOCAL_CFLAGS += -g -fpack-struct=8 -fPIC  \
                -D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__ -DDEBUG

LOCAL_LDLIBS := -ldl -llog -landroid -lz

include $(BUILD_SHARED_LIBRARY)




