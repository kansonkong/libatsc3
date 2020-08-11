# Android.mk for libatsc3 virtual phy support
#

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)

# import our prefab build from bridge
# $(call import-module,libatsc3_bridge)
$(call import-module,prefab/libatsc3_core)

# ---------------------------
# libatsc3 phy virtual interface

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := libatsc3_phy_virtual

LIBATSC3JNIPHYVIRTUALCPP := \
    $(wildcard $(LOCAL_PATH)/src/jni/*.cpp)

# jjustman-2020-08-10 - temporary - refactor this out...
LOCAL_SRC_FILES += \
    $(LIBATSC3JNIPHYVIRTUALCPP:$(LOCAL_PATH)/%=%)
#
##for libatsc3 application and phy interface includes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src
# prefab-fixup..
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libatsc3_bridge/src/jni/

#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../src/phy

LOCAL_CFLAGS += -g -fpack-struct=8 -fPIC  \
                -D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__

LOCAL_LDLIBS := -ldl -llog -landroid -lz


# jjustman-2020-08-10 - link in our atsc3_bridge prefab shared library
# LOCAL_SHARED_LIBRARIES := atsc3_bridge
LOCAL_SHARED_LIBRARIES := atsc3_core


include $(BUILD_SHARED_LIBRARY)
