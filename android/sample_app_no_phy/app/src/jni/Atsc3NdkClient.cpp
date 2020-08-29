//
// Created by Jason Justman on 8/17/20.
//

#include <jni.h>

//Java to native methods
//jjustman-2020-08-17 - dummy file so that linkage of atsc3NdkClient will include libc++_shared.so
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}
