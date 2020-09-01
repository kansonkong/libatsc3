//
// Created by Jason Justman on 8/19/20.
//
#include "Atsc3NdkPHYClientBase.h"

//provide a default dummy no-op impl for our Atsc3NdkPHYClientBase for optional methods

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_Atsc3NdkPHYClientBase_download_1bootloader_1firmware(JNIEnv *env, jobject thiz, jint fd, jstring devicePath) {
    return INT_MIN;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_Atsc3NdkPHYClientBase_open(JNIEnv *env, jobject thiz, jint fd, jstring devicePath) {
    return INT_MIN;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_Atsc3NdkPHYClientBase_open_1from_1capture(JNIEnv *env, jobject thiz, jstring filename) {
    return INT_MIN;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_Atsc3NdkPHYClientBase_tune(JNIEnv *env, jobject thiz, jint freq_khz, jint single_plp) {
    return INT_MIN;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_Atsc3NdkPHYClientBase_listen_1plps(JNIEnv *env, jobject thiz, jobject plps) {
    return INT_MIN;
}