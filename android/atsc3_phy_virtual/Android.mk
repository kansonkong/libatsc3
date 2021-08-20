# Android.mk for libatsc3 virtual phy support

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)

# ---
# codornicesrq
#  /Users/jjustman/Desktop/markone/Middleware/libatsc3/codornicesrq/CodornicesRq-2.2-Android-armeabi-v7a/lib
# patchelf --set-soname libCodornicesRq.so libCodornicesRq.so
# patchelf --replace-needed libc.so.6 libc.so libCodornicesRq.so
# objdump -x libCodornicesRq.so


# jjustman-2021-03-30 - TODO: use this as conditional compilation option for sample app
include $(CLEAR_VARS)
LOCAL_MODULE := libCodornicesRq
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../codornicesrq/CodornicesRq-2.2-Android-$(TARGET_ARCH_ABI)/lib/libCodornicesRq.so
include $(PREBUILT_SHARED_LIBRARY)
#---


# ---
# libsrt library
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := libsrt
#LOCAL_SRC_FILES := $(LOCAL_PATH)/../atsc3_core/libsrt/libs/$(TARGET_ARCH_ABI)/libsrt.so
## LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../libatsc3_core/libsrt/include
#include $(PREBUILT_SHARED_LIBRARY)

#---openssl and libcrypto for srt support
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := libssl
#LOCAL_SRC_FILES := $(LOCAL_PATH)/../atsc3_core/libsrt/libs/$(TARGET_ARCH_ABI)/libssl.so.1.1
#include $(PREBUILT_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE := libcrypto
#LOCAL_SRC_FILES := $(LOCAL_PATH)/../atsc3_core/libsrt/libs/$(TARGET_ARCH_ABI)/libcrypto.so
#include $(PREBUILT_SHARED_LIBRARY)


# import our prefab build from bridge
# $(call import-module,libatsc3_bridge)
# $(call import-module,prefab/libatsc3_bridge)


# ---------------------------
# libatsc3 phy virtual interface

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := libatsc3_phy_virtual

LIBATSC3JNI_LOCAL_PHYVIRTUALCPP := \
    $(wildcard $(LOCAL_PATH)/src/jni/*.cpp)

LIBATSC3JNI_LOCAL_SRT_PHYVIRTUALCPP := \
    $(wildcard $(LOCAL_PATH)/src/jni/srt/*.cpp)

LIBATSC3JNIPHYVIRTUALCPP := \
    $(wildcard $(LOCAL_PATH)/../../src/phy/virtual/*.cpp)

LIBATSC3JNI_SRT_PHYVIRTUALCPP := \
    $(wildcard $(LOCAL_PATH)/../../src/phy/virtual/srt/*.cpp)

LIBATSC3JNI_SRT_CORE_PHYVIRTUALCPP := \
    $(wildcard $(LOCAL_PATH)/../../srt/srtcore/*.cpp) \
    $(LOCAL_PATH)/../../srt/srtcore/srt_compat.c

# jjustman-2020-08-17 - note - renamed hcrypt_ut.c to z_hcrypt_ut.c as it is a unit test that does not compile and breaks the ndk build
LIBATSC3JNI_SRT_HAICRYPT_PHYVIRTUALCPP := \
    $(wildcard $(LOCAL_PATH)/../../srt/haicrypt/hcrypt*.c) \
    $(LOCAL_PATH)/../../srt/haicrypt/cryspr.c \
    $(LOCAL_PATH)/../../srt/haicrypt/cryspr-openssl.c



# jjustman-2020-08-10 - temporary - refactor this out...

# jjustman-2020-08-12 - super hack for impl for srt_c_api.cpp
LOCAL_SRC_FILES += \
    $(LIBATSC3JNI_LOCAL_PHYVIRTUALCPP:$(LOCAL_PATH)/%=%) \
    $(LIBATSC3JNI_LOCAL_SRT_PHYVIRTUALCPP:$(LOCAL_PATH)/%=%) \
    $(LIBATSC3JNIPHYVIRTUALCPP:$(LOCAL_PATH)/%=%) \
    $(LIBATSC3JNI_SRT_PHYVIRTUALCPP:$(LOCAL_PATH)/%=%) \
    $(LIBATSC3JNI_SRT_CORE_PHYVIRTUALCPP:$(LOCAL_PATH)/%=%) \
    $(LIBATSC3JNI_SRT_HAICRYPT_PHYVIRTUALCPP:$(LOCAL_PATH)/%=%)


LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/jni

##for libatsc3 application and phy interface includes
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../src \
	$(LOCAL_PATH)/../../src/phy \
	$(LOCAL_PATH)/../../src/phy/virtual

# prefab-fixup.. for ndk phy bridge/application bridge
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../atsc3_bridge/src/jni

#libsrt prefab fixup for srt
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../..
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../srt
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../srt/srtcore
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/phy/virtual/srt
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../src/phy/virtual/srt/haicrypt

# LOCAL_C_INCLUDES += $(LOCAL_PATH)/../atsc3_core/libsrt/include
# jjustman-2020-09-17 - raptorQ support
# jjustman-2020-09-30 - link against arch
# LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../codornicesrq/CodornicesRq-2.2-Linux-armv7l/include

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../codornicesrq/CodornicesRq-2.2-Android-arm64-v8a/include

#jjustman-2020-08-17 - special defines for SRT
# -fpack-struct=8 -fPIE -fPIC
LOCAL_CFLAGS += -D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__ \
                 -DANDROID=1 \
                 -DSRT_IMPORT_EVENT \
                 -DHAI_ENABLE_SRT=1 \
                 -DHAI_PATCH=1 \
                 -DHAVE_INET_PTON=1 \
                 -DLINUX=1 \
                 -DSRT_ENABLE_APP_READER \
                 -DSRT_ENABLE_CLOSE_SYNCH \
                 -DSRT_ENABLE_ENCRYPTION \
                 -DSRT_VERSION=\"1.4.1\" \
                 -DUSE_OPENSSL \
                 -DSRT_HAS_RAPTORQ=1 \
                 -Dsrt_shared_EXPORTS \
                 -D_GNU_SOURCE \

# 2020-12-17 - working on Sony tv... but crashes in NDK flow
# -DHCRYPT_DEV \
#-DENABLE_HAICRYPT_LOGGING

# -D_GNU_SOURCE \

# jjustman-2021-08-20 - -lssl -lcrypto  should be linked via prefab from $(call import-module,prefab/openssl)

LOCAL_LDLIBS += -ldl -llog -landroid -lz \
				-latsc3_core -latsc3_bridge \
				-lc++_shared

LOCAL_LDFLAGS += -fPIE -fPIC \
				-L $(LOCAL_PATH)/../atsc3_bridge/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/ \
				-L $(LOCAL_PATH)/../atsc3_core/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
#LOCAL_ARM_MODE := arm
#
#LOCAL_CFLAGS += -mhard-float -mfpu=vfp
#LOCAL_LDFLAGS += -Wl,--no-warn-mismatch -mfloat-abi=hard -mfpu=vfp

LOCAL_LDFLAGS += -Wl,--no-warn-mismatch -mfpu=vfp

LOCAL_CFLAGS += -D__LIBC_libCodornicesRq_HACKS__ -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -DLINUX
endif

$(info 'before local shared libs' $(MAKECMDGOALS))

ifneq ($(MAKECMDGOALS),clean)
	ifneq ($(MAKECMDGOALS),generateJsonModelDebug)
LOCAL_SHARED_LIBRARIES := libCodornicesRq ssl crypto
	endif
endif

include $(BUILD_SHARED_LIBRARY)

# libsrt atsc3_bridge atsc3_core
$(info 'before call import module with $(MAKECMDGOALS)' )

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),generateJsonModelDebug)

ifneq ($(call ndk-major-at-least,21),true)
    $(call import-add-path,$(NDK_GRADLE_INJECTED_IMPORT_PATH))
endif

$(call import-add-path,/out)

$(call import-module,prefab/openssl)
endif
endif