//
// Created by Jason Justman on 2022-08-11
//


#include <string.h>
#include <jni.h>
#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>

#ifndef LIBATSC3_ANDROID_ATSC3NDKPHYSONYSTATICJNILOADER_H
#define LIBATSC3_ANDROID_ATSC3NDKPHYSONYSTATICJNILOADER_H

JavaVM* atsc3_ndk_phy_sony_static_loader_get_javaVM();

#define _PHY_SONY_NDK_STATIC_JNI_LOADER_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _PHY_SONY_NDK_STATIC_JNI_LOADER_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN__VA_ARGS__);
#define _PHY_SONY_NDK_STATIC_JNI_LOADER_INFO(...)    	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#endif //LIBATSC3_ANDROID_ATSC3NDKPHYSONYSTATICJNILOADER_H

