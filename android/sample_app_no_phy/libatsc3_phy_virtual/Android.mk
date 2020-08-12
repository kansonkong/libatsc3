# Android.mk for libatsc3 virtual phy support
#

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)


# ---
# core library

include $(CLEAR_VARS)
LOCAL_MODULE := libatsc3_core
LOCAL_SRC_FILES := $(LOCAL_PATH)/../libatsc3_core/build/intermediates/prefab_package/debug/prefab/modules/atsc3_core/libs/android.$(TARGET_ARCH_ABI)/libatsc3_core.so
include $(PREBUILT_SHARED_LIBRARY)

#---

#
#include $(CLEAR_VARS)
#LOCAL_MODULE := libatsc3_bridge
#LOCAL_SRC_FILES := $(LOCAL_PATH)/../libatsc3_bridge/build/intermediates/prefab_package/debug/prefab/modules/atsc3_bridge/libs/android.$(TARGET_ARCH_ABI)/libatsc3_bridge.so
#include $(PREBUILT_SHARED_LIBRARY)

# ---
# libsrt library

include $(CLEAR_VARS)
LOCAL_MODULE := libsrt
LOCAL_SRC_FILES := $(LOCAL_PATH)/../libatsc3_core/libsrt/libs/$(TARGET_ARCH_ABI)/libsrt.so
# LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../libatsc3_core/libsrt/include
include $(PREBUILT_SHARED_LIBRARY)

#---openssl and libcrypto for srt support

include $(CLEAR_VARS)
LOCAL_MODULE := libssl
LOCAL_SRC_FILES += $(LOCAL_PATH)/../libatsc3_core/libsrt/libs/$(TARGET_ARCH_ABI)/libssl.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libcrypto
LOCAL_SRC_FILES += $(LOCAL_PATH)/../libatsc3_core/libsrt/libs/$(TARGET_ARCH_ABI)/libcrypto.so
include $(PREBUILT_SHARED_LIBRARY)


# import our prefab build from bridge
# $(call import-module,libatsc3_bridge)
# $(call import-module,prefab/libatsc3_bridge)


# ---------------------------
# libatsc3 phy virtual interface

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := libatsc3_phy_virtual

LIBATSC3JNIPHYVIRTUALCPP := \
    $(wildcard $(LOCAL_PATH)/../../../src/phy/virtual/*.cpp)

LIBATSC3JNI_SRT_PHYVIRTUALCPP := \
    $(wildcard $(LOCAL_PATH)/../../../src/phy/virtual/srt/*.cpp)


LIBATSC3JNI_SRT_CORE_PHYVIRTUALCPP := \
    $(wildcard $(LOCAL_PATH)/../../../srt/srtcore/*.cpp)

# jjustman-2020-08-10 - temporary - refactor this out...

# jjustman-2020-08-12 - super hack for impl for srt_c_api.cpp
LOCAL_SRC_FILES += \
    $(LIBATSC3JNIPHYVIRTUALCPP:$(LOCAL_PATH)/%=%) \
    $(LIBATSC3JNI_SRT_PHYVIRTUALCPP:$(LOCAL_PATH)/%=%) \
    $(LIBATSC3JNI_SRT_CORE_PHYVIRTUALCPP:$(LOCAL_PATH)/%=%) \
    $(LOCAL_PATH)/../../../srt/srtcore/srt_compat.c


##for libatsc3 application and phy interface includes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/phy
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/phy/virtual


# prefab-fixup.. for ndk phy bridge/application bridge
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libatsc3_bridge/src/jni

#libsrt prefab fixup for srt
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../srt/srtcore
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/phy/virtual/srt
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/phy/virtual/srt/haicrypt

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libatsc3_core/libsrt/include


# shared library missing -fPIC for srt
LOCAL_CFLAGS += -g -fpack-struct=8  \
                -D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__ \
                 -DSRT_VERSION=\"1.4.1\"

LOCAL_LDLIBS += -ldl -llog -landroid -lz
LOCAL_LDFLAGS += -fPIE -fPIC

# jjustman-2020-08-10 - link in our atsc3_bridge prefab shared library
# LOCAL_SHARED_LIBRARIES := atsc3_bridge
# atsc3_bridge
LOCAL_SHARED_LIBRARIES +=  libssl libcrypto atsc3_core  libsrt
LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)
