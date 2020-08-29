# Android.mk for libatsc3 Android sample app
#
#
# jjustman@ngbp.org - for libatsc3 inclusion 2019-09-28

# global pathing
MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)

#jjustman-2020-08-10 - super hack
LOCAL_PATH := $(MY_LOCAL_PATH)

##jjustman-2020-08-12 - remove prefab LOCAL_SRC_FILES := $(LOCAL_PATH)/../libatsc3_core/build/intermediates/prefab_package/debug/prefab/modules/atsc3_core/libs/android.$(TARGET_ARCH_ABI)/libatsc3_core.so
##vmatiash-2020-08-21 - module can't depend on libraries that is not built. This dependency specified in LOCAL_LDLIBS
#include $(CLEAR_VARS)
#LOCAL_MODULE := local-atsc3_core
#LOCAL_SRC_FILES := $(LOCAL_PATH)/../atsc3_core/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/libatsc3_core.so
#ifneq ($(MAKECMDGOALS),clean)
#include $(PREBUILT_SHARED_LIBRARY)
#endif

# ---------------------------
# libatsc3_bridge jni interface

include $(CLEAR_VARS)

LOCAL_MODULE := atsc3_bridge

LIBATSC3_JNIBRIDGE_CPP := \
    $(wildcard $(LOCAL_PATH)/src/jni/*.cpp) \
    $(wildcard $(LOCAL_PATH)/src/jni/phy/*.cpp) \

#jjustman-2020-08-19 - atsc3_logging_externs may not be needed

LOCAL_SRC_FILES += \
    $(LIBATSC3_JNIBRIDGE_CPP:$(LOCAL_PATH)/%=%) \
	$(LOCAL_PATH)/../../src/atsc3_logging_externs.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/src/jni \
	$(LOCAL_PATH)/src/jni/phy

#libatsc3 includes as needed
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/

# jjustman-2020-08-10 - hack-ish... needed for atsc3_pcre2_regex_utils.h
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../atsc3_core/libpcre/include

LOCAL_CFLAGS += -g -fpack-struct=8 -fPIC  \
                -D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__ -DDEBUG

LOCAL_LDFLAGS += -fPIE -fPIC -L \
					$(LOCAL_PATH)/../atsc3_core/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/

LOCAL_LDLIBS := -ldl -lc++_shared -llog -landroid -lz -latsc3_core


# LOCAL_SHARED_LIBRARIES := atsc3_core

include $(BUILD_SHARED_LIBRARY)

