# Android.mk for libatsc3 Android sample app
#
#
# jjustman@ngbp.org - for libatsc3 inclusion 2019-09-28

# global pathing

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)

# ---------------------------
# libatsc3_bridge jni interface

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
# LOCAL_ALLOW_UNDEFINED_SYMBOLS=true

LOCAL_MODULE := libatsc3_bridge

LIBATSC3COREBRIDGECPP := \
    $(wildcard $(LOCAL_PATH)/src/cpp/*.cpp)

LIBATSC3JNIBRIDGECPP := \
    $(wildcard $(LOCAL_PATH)/src/jni/*.cpp)


# LIBUSB := \
#     $(wildcard $(LOCAL_PATH)/src/main/libusb/libusb/*.c)
#     src/main/jni/atsc3NdkApplicationBridge.cpp \
  #    src/main/jni/atsc3NdkPHYBridge.cpp \

# jjustman-2020-08-10 - temporary - refactor this out...
LOCAL_SRC_FILES += \
    $(LIBATSC3COREBRIDGECPP:$(LOCAL_PATH)/%=%)  \
    $(LIBATSC3JNIBRIDGECPP:$(LOCAL_PATH)/%=%)  \

#	$(LOCAL_PATH)/src/main/libusb/libusb/core.c \
#	$(LOCAL_PATH)/src/main/libusb/libusb/descriptor.c \
#	$(LOCAL_PATH)/src/main/libusb/libusb/hotplug.c \
#	$(LOCAL_PATH)/src/main/libusb/libusb/io.c \
#	$(LOCAL_PATH)/src/main/libusb/libusb/sync.c \
#	$(LOCAL_PATH)/src/main/libusb/libusb/strerror.c \
#	$(LOCAL_PATH)/src/main/libusb/libusb/os/linux_usbfs.c \
#	$(LOCAL_PATH)/src/main/libusb/libusb/os/poll_posix.c \
#	$(LOCAL_PATH)/src/main/libusb/libusb/os/threads_posix.c \
#	$(LOCAL_PATH)/src/main/libusb/libusb/os/linux_netlink.c

#udev:     $(LOCAL_PATH)/src/main/libusb/libusb/os/linux_usbfs.c \
#          $(LOCAL_PATH)/src/main/libusb/libusb/os/linux_udev.c
#                          -DUSE_UDEV

#LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/main/libusb
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/main/libusb/libusb
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/main/libusb/android

LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/jni
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/

# jjustman-2020-08-10 - hack-ish...
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libatsc3_core/libpcre/include



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



