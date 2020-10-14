//
// Created by Jason Justman on 09/23/20
//

#include <string.h>
#include <jni.h>
#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>

//jjustman-2020-09-23 - workaround for locale crash: https://android.googlesource.com/platform/ndk/+/master/tests/device/test-libc++-shared/jni/test_1.cc
#include <iostream>
#include <locale>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3NDKPHYAIRWAVZSTATICJNILOADER_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3NDKPHYAIRWAVZSTATICJNILOADER_H

JavaVM* atsc3_ndk_phy_airwavz_static_loader_get_javaVM();

#define _PHY_AIRWAVZ_NDK_STATIC_JNI_LOADER_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _PHY_AIRWAVZ_NDK_STATIC_JNI_LOADER_WARN(...)     	__LIBATSC3_TIMESTAMP_WARN__VA_ARGS__);
#define _PHY_AIRWAVZ_NDK_STATIC_JNI_LOADER_INFO(...)    	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3NDKPHYAIRWAVZSTATICJNILOADER_H

