//
// Created by Jason Justman on 8/18/20.
//
#include <string.h>
#include <jni.h>
#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3NDKSTATICJNILOADER_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3NDKSTATICJNILOADER_H

JavaVM* atsc3_bridge_ndk_static_loader_get_javaVM();


#define _BRIDGE_NDK_STATIC_JNI_LOADER_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _BRIDGE_NDK_STATIC_JNI_LOADER_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN__VA_ARGS__);
#define _BRIDGE_NDK_STATIC_JNI_LOADER_INFO(...)    	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3NDKSTATICJNILOADER_H
