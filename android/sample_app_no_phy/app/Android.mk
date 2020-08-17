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

LOCAL_CFLAGS += -g -fpack-struct=8 -fPIC  \
                -D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__

LOCAL_LDLIBS := -ldl -llog -landroid -lz

include $(BUILD_SHARED_LIBRARY)

# notes: jjustman-2019-11-26
#
# pack ndk structs for aligned access - http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka15414.html
# https://stackoverflow.com/questions/43559712/android-ndk-c-struct-member-access-causes-sigbus-signal-sigbus-illegal-ali
# cppFlags "-fpack-struct=8"



