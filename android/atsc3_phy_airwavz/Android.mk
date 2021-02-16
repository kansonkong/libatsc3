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

# ---------------------------
include $(CLEAR_VARS)
LIB_NAME:= libusb1.0
LOCAL_MODULE:= $(LIB_NAME)
LOCAL_SRC_FILES := 	$(LOCAL_PATH)/../../airwavz_redzone_sdk/lib/android/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
include $(PREBUILT_SHARED_LIBRARY)
# ---------------------------


# ---------------------------
include $(CLEAR_VARS)
LIB_NAME:= libstdc++
LOCAL_MODULE:= $(LIB_NAME)
LOCAL_SRC_FILES := 	$(LOCAL_PATH)/../../airwavz_redzone_sdk/lib/android/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
include $(PREBUILT_SHARED_LIBRARY)
# ---------------------------

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

LOCAL_CFLAGS += -std=c++14 -g -O0 -fpack-struct=8 \
				-D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__ \
                -D__ANDROID__ -Dlinux
#
#LOCAL_LDLIBS += -ldl -lc++_shared -lstdc++  -llog -landroid -lz \
#				-latsc3_core -latsc3_bridge
#
#LOCAL_LDFLAGS += -fPIE -fPIC \

#LOCAL_LDLIBS += -ldl -latsc3_core -latsc3_bridge -llog -landroid
## 				-fPIE -fPIC

LOCAL_LDLIBS += -ldl -llog -landroid -lz \
				-latsc3_core -latsc3_bridge

				 # -lc++_shared

LOCAL_LDFLAGS +=-L $(LOCAL_PATH)/../atsc3_bridge/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/ \
				-L $(LOCAL_PATH)/../atsc3_core/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/

# redzone_api
# redzone_c_vdev_api
LOCAL_SHARED_LIBRARIES += redzone_c_api  redzone_api RedZoneATSC3Parsers usb1.0 stdc++ log

LOCAL_PREBUILDS := atsc3_core atsc3_bridge

include $(BUILD_SHARED_LIBRARY)

