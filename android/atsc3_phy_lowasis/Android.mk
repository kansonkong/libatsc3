# libastc3: phy_lowasis module
# Copyright (C) 2020, OneMedia 3.0
# jjustman@ngbp.org - 2020-08-19

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)

# ---------------------------
LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
# ---------------------------

# include the prebuilt lowaSIS c++ libraries for at3
include $(LOCAL_PATH)/At3drvLibPrebuilt.mk

# ---------------------------
# atsc3_phy_lowasis

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := atsc3_phy_lowasis

##for libatsc3 application and phy interface includes
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../src \
	$(LOCAL_PATH)/../../src/phy

# prefab-fixup.. for ndk phy bridge/application bridge
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../atsc3_bridge/src/jni

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/src/jni \
	$(LOCAL_PATH)/src/jni/lowasis \
	$(LOCAL_PATH)/prebuilt/include

LIBATSC3_PHY_LOWASIS_CPP := \
    $(wildcard $(LOCAL_PATH)/src/jni/*.cpp) \
    $(wildcard $(LOCAL_PATH)/src/jni/lowasis/*.cpp)

LOCAL_SRC_FILES += \
    $(LIBATSC3_PHY_LOWASIS_CPP:$(LOCAL_PATH)/%=%)

LOCAL_CFLAGS += -g -O0 -fpack-struct=8 \
                -D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__ \
                -D__ANDROID__ -Dlinux  \

#jjustman-2020-08-19 - fixup for invalid soname in .so
LOCAL_LDLIBS += -ldl -lc++_shared -llog -landroid -lz \
				-latsc3_core -latsc3_bridge

LOCAL_LDFLAGS += -fPIE -fPIC \
				-L $(LOCAL_PATH)/../atsc3_bridge/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/ \
				-L $(LOCAL_PATH)/../atsc3_core/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/

LOCAL_SHARED_LIBRARIES += at3drv at3base usb-atlas

LOCAL_PREBUILDS := atsc3_core atsc3_bridge

include $(BUILD_SHARED_LIBRARY)

