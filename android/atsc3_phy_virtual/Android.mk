# Android.mk for libatsc3 virtual phy support
# jjustman-2020-10-29 - TODO: fix this gradle error when trying to clean the project, not honoring target arch
# Build command failed.
# Error while executing process /Users/jjustman/Library/Android/sdk/ndk/21.0.6113669/ndk-build with arguments {NDK_PROJECT_PATH=null APP_BUILD_SCRIPT=/Users/jjustman/Desktop/markone/Middleware/libatsc3/android/atsc3_phy_virtual/Android.mk NDK_APPLICATION_MK=/Users/jjustman/Desktop/markone/Middleware/libatsc3/android/atsc3_phy_virtual/Application.mk NDK_GRADLE_INJECTED_IMPORT_PATH=/Users/jjustman/Desktop/markone/Middleware/libatsc3/android/atsc3_phy_virtual/.cxx/ndkBuild/debug/prefab/x86_64 APP_ABI=x86_64 NDK_ALL_ABIS=x86_64 NDK_DEBUG=1 APP_PLATFORM=android-28 NDK_OUT=/Users/jjustman/Desktop/markone/Middleware/libatsc3/android/atsc3_phy_virtual/build/intermediates/ndkBuild/debug/obj NDK_LIBS_OUT=/Users/jjustman/Desktop/markone/Middleware/libatsc3/android/atsc3_phy_virtual/build/intermediates/ndkBuild/debug/lib clean}
# Android NDK: ERROR:/Users/jjustman/Desktop/markone/Middleware/libatsc3/android/atsc3_phy_virtual/Android.mk:CodornicesRq: LOCAL_SRC_FILES points to a missing file
# Android NDK: Check that /Users/jjustman/Desktop/markone/Middleware/libatsc3/android/atsc3_phy_virtual/../../codornicesrq/CodornicesRq-2.2-Android-x86_64/lib/libCodornicesRq.so exists  or that its path is correct
# fcntl(): Bad file descriptor
# /Users/jjustman/Library/Android/sdk/ndk/21.0.6113669/build/core/prebuilt-library.mk:45: *** Android NDK: Aborting    .  Stop.
#

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)


# ---
# codornicesrq
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
LOCAL_CFLAGS += -g -O1 -fpack-struct=8  \
                -D__DISABLE_LIBPCAP__ -D__DISABLE_ISOBMFF_LINKAGE__ -D__DISABLE_NCURSES__ \
                -D__MOCK_PCAP_REPLAY__ -D__LIBATSC3_ANDROID__ \
                 -DANDROID=1 -DHAI_ENABLE_SRT=1 -DHAI_PATCH=1 -DHAVE_INET_PTON=1 -DLINUX=1 -DSRT_ENABLE_APP_READER -DSRT_ENABLE_CLOSE_SYNCH -DSRT_ENABLE_ENCRYPTION -DSRT_VERSION=\"1.4.1\" -DUSE_OPENSSL=1 -D_GNU_SOURCE -Dsrt_shared_EXPORTS


LOCAL_LDLIBS += -ldl -lc++_shared -llog -landroid -lz \
				-latsc3_core -latsc3_bridge

LOCAL_LDFLAGS += -fPIE -fPIC \
				-L $(LOCAL_PATH)/../atsc3_bridge/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/ \
				-L $(LOCAL_PATH)/../atsc3_core/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/

$(info 'before local shared libs' $(MAKECMDGOALS))

ifneq ($(MAKECMDGOALS),clean)
	ifneq ($(MAKECMDGOALS),generateJsonModelDebug)
LOCAL_SHARED_LIBRARIES := libssl libcrypto libCodornicesRq
	endif
endif

include $(BUILD_SHARED_LIBRARY)

# libsrt atsc3_bridge atsc3_core
$(info 'before call import module with $(MAKECMDGOALS)' )

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),generateJsonModelDebug)
$(call import-module,prefab/openssl)
endif
endif