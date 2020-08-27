# Android.mk for libatsc3 virtual phy support
#

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)
MY_CUR_PATH := $(LOCAL_PATH)

#
## ---
## core library
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := libatsc3_core
#LOCAL_SRC_FILES := $(LOCAL_PATH)/../libatsc3_core/build/intermediates/prefab_package/debug/prefab/modules/atsc3_core/libs/android.$(TARGET_ARCH_ABI)/libatsc3_core.so
#ifneq ($(MAKECMDGOALS),clean)
#	include $(PREBUILT_SHARED_LIBRARY)
#endif
##---

##jjustman-2020-08-12 - remove prefab LOCAL_SRC_FILES := $(LOCAL_PATH)/../libatsc3_core/build/intermediates/prefab_package/debug/prefab/modules/atsc3_core/libs/android.$(TARGET_ARCH_ABI)/libatsc3_core.so
##vmatiash-2020-08-21 - module can't depend on libraries that is not built. This dependency specified in LOCAL_LDLIBS
#include $(CLEAR_VARS)
#LOCAL_MODULE := local-pv-atsc3_core
#LOCAL_SRC_FILES := $(LOCAL_PATH)/../atsc3_core/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/libatsc3_core.so
#ifneq ($(MAKECMDGOALS),clean)
#	ifneq ($(MAKECMDGOALS),generateJsonModelDebug)
#include $(PREBUILT_SHARED_LIBRARY)
#	endif
#endif


#
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := libatsc3_bridge
#LOCAL_SRC_FILES := $(LOCAL_PATH)/../libatsc3_bridge/build/intermediates/prefab_package/debug/prefab/modules/atsc3_bridge/libs/android.$(TARGET_ARCH_ABI)/libatsc3_bridge.so
#ifneq ($(MAKECMDGOALS),clean)
#	include $(PREBUILT_SHARED_LIBRARY)
#endif

##jjustman-2020-08-12 - remove prefab LOCAL_SRC_FILES := $(LOCAL_PATH)/../libatsc3_core/build/intermediates/prefab_package/debug/prefab/modules/atsc3_core/libs/android.$(TARGET_ARCH_ABI)/libatsc3_core.so
##vmatiash-2020-08-21 - module can't depend on libraries that is not built. This dependency specified in LOCAL_LDLIBS
#include $(CLEAR_VARS)
#LOCAL_MODULE := local-pv-atsc3_bridge
#LOCAL_SRC_FILES := $(LOCAL_PATH)/../atsc3_bridge/build/intermediates/ndkBuild/debug/obj/local/$(TARGET_ARCH_ABI)/libatsc3_bridge.so
#$(info $(MAKECMDGOALS))
#ifneq ($(MAKECMDGOALS),clean)
#	ifneq ($(MAKECMDGOALS),generateJsonModelDebug)
#include $(PREBUILT_SHARED_LIBRARY)
#	endif
#endif

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

# shared library missing -fPIC for srt

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

# jjustman-2020-08-10 - link in our atsc3_bridge prefab shared library
# LOCAL_SHARED_LIBRARIES := atsc3_bridge
# ifneq ($(MAKECMDGOALS),clean)

# libcrypto
$(info 'before local shared libs' $(MAKECMDGOALS))

ifneq ($(MAKECMDGOALS),clean)
	ifneq ($(MAKECMDGOALS),generateJsonModelDebug)
LOCAL_SHARED_LIBRARIES := libssl libcrypto
	endif
endif

include $(BUILD_SHARED_LIBRARY)

#as per https://android-developers.googleblog.com/2020/02/native-dependencies-in-android-studio-40.html
# 	ifneq ($(MAKECMDGOALS),generateJsonModelDebug)
# 	endif


# libsrt atsc3_bridge atsc3_core
$(info 'before call import module with $(MAKECMDGOALS)' )

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),generateJsonModelDebug)
$(call import-module,prefab/openssl)
endif
endif