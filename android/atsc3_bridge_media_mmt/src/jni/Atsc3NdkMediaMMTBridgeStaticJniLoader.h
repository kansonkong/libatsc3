//Atsc3NdkMediaMMTBridge
// Created by Jason Justman on 8/18/20.
//
#include <string.h>
#include <jni.h>
#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>

#ifndef LIBATSC3_MEDIAMMTBRIDGE_ATSC3NDKSTATICJNILOADER_H
#define LIBATSC3_MEDIAMMTBRIDGE_ATSC3NDKSTATICJNILOADER_H

JavaVM* atsc3_ndk_media_mmt_bridge_static_loader_get_javaVM();

#define _MEDIA_MMT_BRIDGE_NDK_STATIC_JNI_LOADER_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _MEDIA_MMT_BRIDGE_NDK_STATIC_JNI_LOADER_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN__VA_ARGS__);
#define _MEDIA_MMT_BRIDGE_NDK_STATIC_JNI_LOADER_INFO(...)    	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#endif //LIBATSC3_MEDIAMMTBRIDGE_ATSC3NDKSTATICJNILOADER_H
