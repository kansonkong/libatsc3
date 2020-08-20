//
// Created by Jason Justman on 8/10/20.
//

#include <android/log.h>

#ifndef LIBATSC3_ANDROID_ATSC3LOGGINGUTILS_H
#define LIBATSC3_ANDROID_ATSC3LOGGINGUTILS_H

#if DEBUG
    #define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, MODULE_NAME, __VA_ARGS__)
	#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , MODULE_NAME, __VA_ARGS__)
	#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , MODULE_NAME, __VA_ARGS__)
	#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , MODULE_NAME, __VA_ARGS__)
	#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , MODULE_NAME, __VA_ARGS__)
#else
#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#endif

//jjustman-2020-08-10 TODO - fix me when not in debug, complaning about no LOGD in global scope
//#define printf LOGD
//#define eprintf LOGE
#endif //LIBATSC3_ANDROID_ATSC3LOGGINGUTILS_H
