# libatsc3: phy_airwavz module
# Copyright (C) 2020, OneMedia 3.0
# jjustman@ngbp.org - 2020-09-23

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)

# ---------------------------
LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
# ---------------------------



## --------------------------
# build libusb_android
## --------------------------
LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := libusb1.0

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
# Airwavz.tv RedZone Receiver SDK
# prebuilt libraries
# ---------------------------

# ---------------------------

include $(CLEAR_VARS)
LIB_NAME:= libredzone_api
LOCAL_MODULE:= $(LIB_NAME)
LOCAL_SRC_FILES := 	$(LOCAL_PATH)/../../airwavz_redzone_sdk/lib/android/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
include $(PREBUILT_SHARED_LIBRARY)
# ---------------------------

# ---------------------------
include $(CLEAR_VARS)
LIB_NAME:= libredzone_c_api
LOCAL_MODULE:= $(LIB_NAME)
LOCAL_SRC_FILES := 	$(LOCAL_PATH)/../../airwavz_redzone_sdk/lib/android/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
include $(PREBUILT_SHARED_LIBRARY)
# ---------------------------





# ---------------------------
include $(CLEAR_VARS)
LIB_NAME:= libredzone_c_vdev_api
LOCAL_MODULE:= $(LIB_NAME)
LOCAL_SRC_FILES := 	$(LOCAL_PATH)/../../airwavz_redzone_sdk/lib/android/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
include $(PREBUILT_SHARED_LIBRARY)
# ---------------------------

# ---------------------------
include $(CLEAR_VARS)
LIB_NAME:= libRedZoneATSC3Parsers
LOCAL_MODULE:= $(LIB_NAME)
LOCAL_SRC_FILES := 	$(LOCAL_PATH)/../../airwavz_redzone_sdk/lib/android/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
include $(PREBUILT_SHARED_LIBRARY)
# ---------------------------
#
# # ---------------------------
# include $(CLEAR_VARS)
# LIB_NAME:= libusb1.0
# LOCAL_MODULE:= $(LIB_NAME)
# LOCAL_SRC_FILES := 	$(LOCAL_PATH)/../../airwavz_redzone_sdk/lib/android/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
# include $(PREBUILT_SHARED_LIBRARY)
# # ---------------------------
#
#
# # ---------------------------
# include $(CLEAR_VARS)
# LIB_NAME:= libstdc++
# LOCAL_MODULE:= $(LIB_NAME)
# LOCAL_SRC_FILES := 	$(LOCAL_PATH)/../../airwavz_redzone_sdk/lib/android/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
# include $(PREBUILT_SHARED_LIBRARY)
# # ---------------------------

# ---------------------------
include $(CLEAR_VARS)
LIB_NAME:= liblog
LOCAL_MODULE:= $(LIB_NAME)
LOCAL_SRC_FILES := 	$(LOCAL_PATH)/../../airwavz_redzone_sdk/lib/android/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
include $(PREBUILT_SHARED_LIBRARY)
# ---------------------------

# ---------------------------
# libatsc3 airwavz phy implementation
# atsc3_phy_airwavz

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := atsc3_phy_airwavz

##for libatsc3 application and phy interface includes
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../src \
	$(LOCAL_PATH)/../../src/phy

# prefab-fixup.. for ndk phy bridge/application bridge
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../atsc3_bridge/src/jni

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/src/jni \
	$(LOCAL_PATH)/src/jni/airwavz \
	$(LOCAL_PATH)/../../airwavz_redzone_sdk/include

LIBATSC3_PHY_AIRWAVZ_CPP := \
    $(wildcard $(LOCAL_PATH)/src/jni/*.cpp) \
    $(wildcard $(LOCAL_PATH)/src/jni/airwavz/*.cpp)

LOCAL_SRC_FILES += \
    $(LIBATSC3_PHY_AIRWAVZ_CPP:$(LOCAL_PATH)/%=%)

# -O0 -fpack-struct=8
#  -std=c++14
# -fpack-struct=4
LOCAL_CFLAGS +=	-D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__LIBATSC3_ANDROID__ \
                -D__ANDROID__ -Dlinux
#
#LOCAL_LDLIBS += -ldl -lc++_shared -lstdc++  -llog -landroid -lz \
#				-latsc3_core -latsc3_bridge
#
#LOCAL_LDFLAGS += -fPIE -fPIC \

#LOCAL_LDLIBS += -ldl -latsc3_core -latsc3_bridge -llog -landroid
## 				-fPIE -fPIC

LOCAL_LDLIBS += -ldl -llog -landroid -lz \
				-latsc3_core -latsc3_bridge \
				-fPIE -fPIC

# -lc++_shared

LOCAL_LDFLAGS +=-L $(LOCAL_PATH)/../atsc3_bridge/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/ \
				-L $(LOCAL_PATH)/../atsc3_core/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/

# redzone_api
# redzone_c_vdev_api

#stdc++
# usb1.0
LOCAL_SHARED_LIBRARIES += libusb1.0 redzone_c_api  redzone_api RedZoneATSC3Parsers

LOCAL_PREBUILDS := atsc3_core atsc3_bridge

include $(BUILD_SHARED_LIBRARY)

