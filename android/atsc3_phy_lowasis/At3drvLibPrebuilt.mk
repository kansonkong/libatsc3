LOCAL_PATH := $(call my-dir)

#------------
# at3base

include $(CLEAR_VARS)

LIB_NAME:= libat3base
LOCAL_MODULE:= $(LIB_NAME)
LOCAL_SRC_FILES := prebuilt/libs/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
LOCAL_EXPORT_C_INCLUDES := $(LIB_NAME)/include
include $(PREBUILT_SHARED_LIBRARY)

#------------
# at3drv

include $(CLEAR_VARS)

LIB_NAME:= libat3drv
LOCAL_MODULE:= $(LIB_NAME)
LOCAL_SRC_FILES := prebuilt/libs/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
LOCAL_EXPORT_C_INCLUDES := $(LIB_NAME)/include
include $(PREBUILT_SHARED_LIBRARY)

#------------
# libusb-atlas

include $(CLEAR_VARS)

LIB_NAME:= libusb-atlas
LOCAL_MODULE:= $(LIB_NAME)
LOCAL_SRC_FILES := prebuilt/libs/$(TARGET_ARCH_ABI)/$(LIB_NAME).so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_MODULE)/include
include $(PREBUILT_SHARED_LIBRARY)

