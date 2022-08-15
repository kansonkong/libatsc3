# libastc3: phy sony module
# jjustman@ngbp.org - 2022-08-11

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)

# ---------------------------
LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
# ---------------------------

## --------------------------
# build libusb_android
# jjustman-2022-05-24 - todo: refactor this out to a top level android_libusb module
## --------------------------
LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := libusb_android_sony

LOCAL_SRC_FILES += \
	$(LOCAL_PATH)/../../libusb_android/libusb/core.c \
	$(LOCAL_PATH)/../../libusb_android/libusb/descriptor.c \
	$(LOCAL_PATH)/../../libusb_android/libusb/hotplug.c \
	$(LOCAL_PATH)/../../libusb_android/libusb/io.c \
	$(LOCAL_PATH)/../../libusb_android/libusb/sync.c \
	$(LOCAL_PATH)/../../libusb_android/libusb/strerror.c \
	$(LOCAL_PATH)/../../libusb_android/libusb/os/linux_usbfs.c \
	$(LOCAL_PATH)/../../libusb_android/libusb/os/poll_posix.c \
	$(LOCAL_PATH)/../../libusb_android/libusb/os/threads_posix.c \
	$(LOCAL_PATH)/../../libusb_android/libusb/os/linux_netlink.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libusb_android/libusb
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libusb_android/android

LOCAL_CFLAGS += -D__ANDROID__ -Dlinux

#jjustman-2020-08-19 - fixup for invalid soname in .so
LOCAL_LDLIBS += -ldl -lc++_shared -llog -landroid

include $(BUILD_SHARED_LIBRARY)

# ---------------------------
# atsc3_phy_sony module

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := atsc3_phy_sony

##for libatsc3 application and phy interface includes
INCLUDE_LIBATSC3_HEADERS += \
	$(LOCAL_PATH)/../../src \
	$(LOCAL_PATH)/../../src/phy \
	$(LOCAL_PATH)/src/jni \
	$(LOCAL_PATH)/src/jni/utils/inc

INCLUDE_PHY_SONY := \
	$(LOCAL_PATH)/src/jni/sony

LIB_PHY_SONY := \
    $(wildcard $(LOCAL_PATH)/src/jni/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/jni/sony/*.cpp)

# sony demod api for:
#   CXD285X		CXD2880		CXD6801		DIB9090		MXL541C		MXL69X		Si2168B_47
INCLUDE_PHY_SONY_DEMOD_API := \
	$(LOCAL_PATH)/src/jni/sony/demod/api/CXD285X    \
	$(LOCAL_PATH)/src/jni/sony/demod/api/CXD2880    \
   	$(LOCAL_PATH)/src/jni/sony/demod/api/CXD6801    \
    $(LOCAL_PATH)/src/jni/sony/demod/api/DIB9090    \
    $(LOCAL_PATH)/src/jni/sony/demod/api/MXL541C    \
    $(LOCAL_PATH)/src/jni/sony/demod/api/MXL69X     \
    $(LOCAL_PATH)/src/jni/sony/demod/api/Si2168B_47

LIB_PHY_SONY_DEMOD_API := \
    $(wildcard $(LOCAL_PATH)/src/jni/sony/demod/api/CXD285X/*.c) \
    $(wildcard $(LOCAL_PATH)/src/jni/sony/demod/api/CXD2880/*.c) \
    $(wildcard $(LOCAL_PATH)/src/jni/sony/demod/api/CXD6801/*.c) \
    $(wildcard $(LOCAL_PATH)/src/jni/sony/demod/api/DIB9090/*.c) \
    $(wildcard $(LOCAL_PATH)/src/jni/sony/demod/api/MXL541C/*.c) \
    $(wildcard $(LOCAL_PATH)/src/jni/sony/demod/api/MXL69X/*.c) \
    $(wildcard $(LOCAL_PATH)/src/jni/sony/demod/api/Si2168B_47/*.c)

#ite 930x api includes and srcs
INCLUDE_PHY_SONY_ITE930X_API := \
	$(LOCAL_PATH)/src/jni/sony/ite930x/api

LIB_PHY_SONY_ITE930X_API := \
	$(wildcard $(LOCAL_PATH)/src/jni/sony/ite930x/api/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/jni/sony/ite930x/api/*.c)

#ite 930x core includes and srcs
INCLUDE_PHY_SONY_ITE930X_CORE := \
	$(LOCAL_PATH)/src/jni/sony/ite930x/core

LIB_PHY_SONY_ITE930X_CORE := \
	$(wildcard $(LOCAL_PATH)/src/jni/sony/ite930x/core/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/jni/sony/ite930x/core/*.c)


LIB_PHY_SONY_UTILS := \
    $(LOCAL_PATH)/src/jni/utils/CircularBuffer.c

# build our final list of includes and src files

LOCAL_C_INCLUDES +=                                 \
    $(INCLUDE_LIBATSC3_HEADERS)                     \
	$(LOCAL_PATH)/../atsc3_bridge/src/jni           \
	$(LOCAL_PATH)/../../libusb_android/libusb       \
	$(INCLUDE_PHY_SONY)                          \
	$(INCLUDE_PHY_SONY_DEMOD_API)                   \
	$(INCLUDE_PHY_SONY_ITE930X_API)                 \
	$(INCLUDE_PHY_SONY_ITE930X_CORE)

LOCAL_SRC_FILES +=                                  \
	$(LIB_PHY_SONY:$(LOCAL_PATH)/%=%)               \
	$(LIB_PHY_SONY_DEMOD_API:$(LOCAL_PATH)/%=%)      \
	$(LIB_PHY_SONY_ITE930X_API:$(LOCAL_PATH)/%=%)   \
	$(LIB_PHY_SONY_ITE930X_CORE:$(LOCAL_PATH)/%=%)  \
	$(LIB_PHY_SONY_UTILS:$(LOCAL_PATH)/%=%)

LOCAL_CFLAGS += -D__DISABLE_LIBPCAP__ \
				-D__DISABLE_ISOBMFF_LINKAGE__ \
				-D__DISABLE_NCURSES__ \
				-D__MOCK_PCAP_REPLAY__ \
				-D__LIBATSC3_ANDROID__ \
				-D__ANDROID__ \
				-Dlinux  \
				-D__ANDROID_ARCH_$(TARGET_ARCH_ABI)__

LOCAL_LDFLAGS += -L $(LOCAL_PATH)/../atsc3_bridge/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/ \
				 -L $(LOCAL_PATH)/../atsc3_core/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/

LOCAL_PREBUILDS := atsc3_core atsc3_bridge

LOCAL_LDLIBS += \
	-ldl -lc++_shared -llog -landroid -lz \
	-latsc3_core -latsc3_bridge


LOCAL_SHARED_LIBRARIES := \
	libusb_android_sony

include $(BUILD_SHARED_LIBRARY)

