# libastc3: phy_saankhya module
# Copyright (C) 2020, OneMedia 3.0
# jjustman@ngbp.org - 2020-08-19

# jjustman-2020-09-08 - Build Tagrget Sources/Linkage: SLAPI_BUILD_TARGET
#
# Set to markone to use for MarkONE handset
#			LIB_SL_API_CUST and INCLUDE_SL_API_CUST and no .o linkage for firmware
#
# or set to kailash to use for KAILASH dongle
# 			LIB_SL_API_REF and INCLUDE_SL_API_REF and include linkage for firmware

SLAPI_BUILD_TARGET = markone
# or kailash

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

LOCAL_MODULE := libusb_android

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

# jjustman-2020-08-19: NOTE regarding missing SONAME tag
#	...this was caused by missing SONAME tag on the generated shared library. When this tag is missing,
#    the Android build process will use the absolute path to the library instead of the device-specific path.
#
#	One has to explicitly specify the SONAME either via -soname or -install_name (macOS-specific) parameter to the linker.
#
#	For instance, this can happen when building shared libs from Go source, as mentioned in https://github.com/golang/go/issues/17807\#issuecomment-259930881.
#
#     objdump -p libNXP_Tuner_Lib.so
## ---------------------------
# NXP Tuner Libraries - prebuilt - reference to slsdk path
# jjustman-2020-12-31 - fixup for missing SONAME:  patchelf --set-soname libNXP_Tuner_Lib.so libNXP_Tuner_Lib.so
# $(LOCAL_PATH)/
include $(CLEAR_VARS)
LOCAL_MODULE := libNXP_Tuner_Lib-prebuilt
LOCAL_SRC_FILES := ../../saankhyalabs-slsdk/slapi/lib/android/$(TARGET_ARCH_ABI)/libNXP_Tuner_Lib.so
include $(PREBUILT_SHARED_LIBRARY)

# SiTune Libraries - prebuilt - reference to slsdk path
# jjustman-2020-12-31 - fixup for missing SONAME: patchelf --set-soname libSiTune_Tuner_Lib.so libSiTune_Tuner_Lib.so
# $(LOCAL_PATH)/
include $(CLEAR_VARS)
LOCAL_MODULE := libSiTune_Tuner_Lib-prebuilt
LOCAL_SRC_FILES := ../../saankhyalabs-slsdk/slapi/lib/android/$(TARGET_ARCH_ABI)/libSiTune_Tuner_Lib.so
include $(PREBUILT_SHARED_LIBRARY)
## ---------------------------

## ---------------------------
# FX3 firmware binary compilation - build fx3 f/w into atsc3NdkClient
#
#  slsdk-0.10 provides:
#	./slplf/src/slref/fx3s/bin/P3_Firmware_v2.3.3(SL_x0x0_EVB_SDIO).img
#	./slplf/src/slref/fx3s/bin/P3_Firmware_v2.1.3(SL_3000_EVB).img
#	./slplf/src/slref/fx3s/bin/P3_Firmware_v2.2.3(SL_4000_EVB).img
#	./slplf/src/slref/fx3s/bin/P3_Firmware_v3.2(KAILASH_DONGLE).img
#	./slplf/src/slref/fx3s/bin/P3_Firmware_v2.0.3(SL_3010_EVB).img
#	./slplf/src/slref/fx3s/bin/P3_Firmware_v3.2.2(KAILASH_DONGLE).img
#

# jjustman-2020-08-20 - todo - move this to a local path so we aren't fighting symbol names
# $(LOCAL_PATH)
LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE :=  P3_FW_BUILD
$(LOCAL_MODULE): P3_FW_BUILD_TARGET

# jjustman-2020-08-25 - this looks like a hack, but it addresses an issue with the symbol linkage from ld which is usually the full file pathname as the export
#
#  using full paths for LD results in symbol linkage ala:
#		_binary____________saankhyalabs_slsdk_slplf_src_slref_fx3s_bin_P3_Firmware_v3_1_KAILASH_DONGLE_img_size
#
# 	with "copy/cd/ld/
#		% objdump -t p3_firmware_KAILASH_DONGLE.o
#
#		p3_firmware_KAILASH_DONGLE.o:   file format ELF64-aarch64-little
#
#		SYMBOL TABLE:
#		0000000000000000 l    d  .data  00000000 .data
#		0000000000023cc0         .data  00000000 _binary___p3_firmware_KAILASH_DONGLE_img_end
#		0000000000023cc0         *ABS*  00000000 _binary___p3_firmware_KAILASH_DONGLE_img_size
#		0000000000000000         .data  00000000 _binary___p3_firmware_KAILASH_DONGLE_img_start
#		jjustman@sdg-komo-mac188 fx3s %

# use release FX3 firmware P3_Firmware_v3.2.2(KAILASH_DONGLE).img
# $(shell cp $(LOCAL_PATH)/../../saankhyalabs-slsdk/slplf/src/slref/fx3s/bin/P3_Firmware_v3.1_KAILASH_DONGLE.img $(LOCAL_PATH)/prebuilt/firmware/fx3s/p3_firmware_KAILASH_DONGLE.img)
# 	$(shell cp $(LOCAL_PATH)/../../saankhyalabs-slsdk/slplf/src/slref/fx3s/bin/P3_Firmware_v3.2.2\(KAILASH_DONGLE\).img $(LOCAL_PATH)/prebuilt/firmware/fx3s/p3_firmware_KAILASH_DONGLE.img)
# 	$(info TARGET_LD is $(TARGET_LD))
#	$(info PATH for prebuilt fx3 is $(LOCAL_PATH)/prebuilt/firmware/fx3s/)
# jjustman-2020-12-31 - TODO: for building armeabi-v7a: $(TARGET_ARCH_ABI)
P3_FW_BUILD_TARGET:
	$(shell mkdir -p "$(LOCAL_PATH)/prebuilt/firmware/fx3s/")
	$(shell cp $(LOCAL_PATH)/../../saankhyalabs-slsdk/slplf/src/slref/fx3s/bin/P3_Firmware_v3.1_KAILASH_DONGLE.img $(LOCAL_PATH)/prebuilt/firmware/fx3s/p3_firmware_KAILASH_DONGLE.img)
	$(shell cd $(LOCAL_PATH)/prebuilt/firmware/fx3s/ && $(TARGET_LD) -r -b binary ./p3_firmware_KAILASH_DONGLE.img -o ./p3_firmware_KAILASH_DONGLE.o)
include $(BUILD_SHARED_LIBRARY)

# ---------------------------
# SL HEX payload binary resource object linkage
# iccmFile: iccm.hex

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE :=  SL_SDR_ICCM_BUILD
$(LOCAL_MODULE): SL_SDR_ICCM_BUILD_TARGET

SL_SDR_ICCM_BUILD_TARGET:
	$(shell mkdir -p "$(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/")
	$(shell cp $(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/bin/atsc3_aa/iccm.hex $(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/iccm.hex)
	$(shell cd $(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/ && $(TARGET_LD) -r -b binary ./iccm.hex -o ./iccm_hex.o)
include $(BUILD_SHARED_LIBRARY)

# dccmFile: dccm.hex

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE :=  SL_SDR_DCCM_BUILD
$(LOCAL_MODULE): SL_SDR_DCCM_BUILD_TARGET

SL_SDR_DCCM_BUILD_TARGET:
	$(shell mkdir -p "$(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/")
	$(shell cp $(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/bin/atsc3_aa/dccm.hex $(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/dccm.hex)
	$(shell cd $(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/ && $(TARGET_LD) -r -b binary ./dccm.hex -o ./dccm_hex.o)
include $(BUILD_SHARED_LIBRARY)

# secondaryFile: atsc3.hex

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE :=  SL_SDR_ATSC3_BUILD
$(LOCAL_MODULE): SL_SDR_ATSC3_BUILD_TARGET

SL_SDR_ATSC3_BUILD_TARGET:
	$(shell mkdir -p "$(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/")
	$(shell cp $(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/bin/atsc3_aa/atsc3.hex $(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/atsc3.hex)
	$(shell cd $(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/ && $(TARGET_LD) -r -b binary ./atsc3.hex -o ./atsc3_hex.o)
include $(BUILD_SHARED_LIBRARY)
# END of SL HEX payload binary resource object linkage
# ---------------------------


# ---------------------------
# atsc3_phy_saankhya module

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := atsc3_phy_saankhya

# prefab-fixup.. for ndk phy bridge/application bridge
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../atsc3_bridge/src/jni

##for libatsc3 application and phy interface includes
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../src \
	$(LOCAL_PATH)/../../src/phy \
	$(LOCAL_PATH)/src/jni \
	$(LOCAL_PATH)/src/jni/utils/inc

LIB_PHY_SAANKHYACPP := \
    $(wildcard $(LOCAL_PATH)/src/jni/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/jni/saankhya/*.cpp) \
    $(LOCAL_PATH)/src/jni/utils/CircularBuffer.c

LIB_SL_API := \
	$(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/src/sl_config.c \
    $(wildcard $(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/src/demod/*.c) \
    $(wildcard $(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/src/tuner/*.c) \
    $(wildcard $(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/src/tuner/nxp/*.c) \
    $(wildcard $(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/src/tuner/situne/*.c)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/inc \
	$(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/src/demod \
	$(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/src/tuner/nxp \
	$(LOCAL_PATH)/../../saankhyalabs-slsdk/slapi/src/tuner/situne \
	$(LOCAL_PATH)/../../saankhyalabs-slsdk/slplf/inc \
	$(LOCAL_PATH)/../../libusb_android/libusb

# jjustman-2020-09-08 - for FX3-USB based I/F
INCLUDE_SL_API_REF := \
	$(LOCAL_PATH)/../../saankhyalabs-slsdk/slplf/src/slref/fx3s

LIB_SL_API_REF := \
    $(wildcard $(LOCAL_PATH)/../../saankhyalabs-slsdk/slplf/src/slref/*.c) \
    $(wildcard $(LOCAL_PATH)/../../saankhyalabs-slsdk/slplf/src/slref/fx3s/*.c) \
    $(wildcard $(LOCAL_PATH)/../../saankhyalabs-slsdk/slplf/src/slref/fx3s/*.cpp)

# jjustman-2020-09-08 - for SDIO / SL_REF for BORQS
INCLUDE_SL_API_CUST := \
	$(LOCAL_PATH)/../../saankhyalabs-slsdk/slplf/src/slcust \
	$(LOCAL_PATH)/../../saankhyalabs-slsdk/slplf/src/slcust/inc

LIB_SL_API_CUST := \
    $(wildcard $(LOCAL_PATH)/../../saankhyalabs-slsdk/slplf/src/slcust/*.c)

LOCAL_CFLAGS += -D__DISABLE_LIBPCAP__ \
				-D__DISABLE_ISOBMFF_LINKAGE__ \
				-D__DISABLE_NCURSES__ \
				-D__MOCK_PCAP_REPLAY__ \
				-D__LIBATSC3_ANDROID__ \
				-D__ANDROID__ \
				-Dlinux  \
				-DSI_TUNER \
				-D__ANDROID_ARCH_$(TARGET_ARCH_ABI)__

LOCAL_LDFLAGS += -L $(LOCAL_PATH)/../atsc3_bridge/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/ \
				 -L $(LOCAL_PATH)/../atsc3_core/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/

LOCAL_PREBUILDS := atsc3_core atsc3_bridge

# jjustman-2020-09-08 - markone - using LIB_SL_API_CUST and INCLUDE_SL_API_CUST
ifeq ($(SLAPI_BUILD_TARGET), markone)

$(info Building MarkONE support)

	LOCAL_C_INCLUDES += \
		$(INCLUDE_SL_API_CUST)

	LOCAL_SRC_FILES += \
		$(LIB_SL_API:$(LOCAL_PATH)/%=%) \
		$(LIB_SL_API_CUST:$(LOCAL_PATH)/%=%) \
		$(LIB_PHY_SAANKHYACPP:$(LOCAL_PATH)/%=%)

	LOCAL_CFLAGS += -DSL_MARKONE \
					-DSL_USE_LINKED_FIRMWARE_EXTERN

	LOCAL_LDLIBS += -ldl -lc++_shared -llog -landroid -lz \
		-latsc3_core -latsc3_bridge \
		$(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/iccm_hex.o \
		$(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/dccm_hex.o \
		$(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/atsc3_hex.o

	LOCAL_SHARED_LIBRARIES := \
		SL_SDR_ICCM_BUILD SL_SDR_DCCM_BUILD SL_SDR_ATSC3_BUILD \
		libusb_android \
		libSiTune_Tuner_Lib-prebuilt \
		libNXP_Tuner_Lib-prebuilt
endif

# jjustman-2020-09-08 - using kailash INCLUDE_SL_API_REF and INCLUDE_SL_API_REF

ifeq ($(SLAPI_BUILD_TARGET), kailash)

$(info Building KAILASH support)

	# _binary_prebuilt_fw_atsc3_iccm_hex_... use objdump -t p3_firmware_KAILASH_DONGLE.o for investigation
	#				prebuilt/fw/fx3s/p3_firmware_KAILASH_DONGLE.o \
	#				prebuilt/fw/atsc3/iccm_hex.o \
	#				prebuilt/fw/atsc3/dccm_hex.o \
	#				prebuilt/fw/atsc3/atsc3_hex.o
	#

	#jjustman-2020-08-19 - fixup for invalid soname in .so

	LOCAL_C_INCLUDES += \
		$(INCLUDE_SL_API_REF)

	LOCAL_SRC_FILES += \
		$(LIB_SL_API:$(LOCAL_PATH)/%=%) \
		$(LIB_SL_API_REF:$(LOCAL_PATH)/%=%) \
		$(LIB_PHY_SAANKHYACPP:$(LOCAL_PATH)/%=%)

	LOCAL_CFLAGS += -DSL_KAILASH -DSL_USE_LINKED_FIRMWARE_EXTERN

	LOCAL_LDLIBS += -ldl -lc++_shared -llog -landroid -lz \
		-latsc3_core -latsc3_bridge \
		$(LOCAL_PATH)/prebuilt/firmware/fx3s/p3_firmware_KAILASH_DONGLE.o \
		$(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/iccm_hex.o \
		$(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/dccm_hex.o \
		$(LOCAL_PATH)/prebuilt/firmware/atsc3_aa/atsc3_hex.o

	LOCAL_SHARED_LIBRARIES := \
		P3_FW_BUILD \
		SL_SDR_ICCM_BUILD SL_SDR_DCCM_BUILD SL_SDR_ATSC3_BUILD \
		libusb_android \
		libSiTune_Tuner_Lib-prebuilt \
		libNXP_Tuner_Lib-prebuilt

endif

include $(BUILD_SHARED_LIBRARY)

