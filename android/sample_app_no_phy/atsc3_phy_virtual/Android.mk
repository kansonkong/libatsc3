# Android.mk for libatsc3 virtual phy support
#

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)

# ---------------------------
# libatsc3 phy virtual interface

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
# LOCAL_ALLOW_UNDEFINED_SYMBOLS=true

LOCAL_MODULE := atsc3NdkVirtualPhy

LIBATSC3JNIBRIDGECPP := \
    $(wildcard $(LOCAL_PATH)/src/jni/*.cpp)

# jjustman-2020-08-10 - temporary - refactor this out...
LOCAL_SRC_FILES += \
    $(LIBATSC3JNIBRIDGECPP:$(LOCAL_PATH)/%=%)  \
    $(LIBATSC3CPP:$(LOCAL_PATH)/%=%)

#for libatsc3 application and phy interface includes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/application
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/phy
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/phy/virtual

LOCAL_CFLAGS += -g -fpack-struct=8 -fPIC  \
                -D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__

LOCAL_LDLIBS := -ldl -llog -landroid -lz

include $(BUILD_SHARED_LIBRARY)
