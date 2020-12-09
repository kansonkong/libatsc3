//
// Created by Jason Justman on 8/18/20.
//

#include "Atsc3NdkPHYVirtualStaticJniLoader.h"

JavaVM* javaVM_scoped = nullptr;

JavaVM* atsc3_ndk_phy_virtual_static_loader_get_javaVM() {
    return javaVM_scoped;
}
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    javaVM_scoped = vm;
    return JNI_VERSION_1_6;
}

#ifdef __LIBC_libCodornicesRq_HACKS__
//jjustman-2020-12-01 - hacks for libCodornicesRq and armeabi-v7a
#include <signal.h>
#include <string.h>
#include <time.h>

extern "C"   int raise(int sig) {
    return raise(sig);
}

extern "C"  void* memcpy(void* dst, const void *src, size_t n) {
    return memcpy(dst, src, n);
}

extern "C" time_t time(time_t* tloc) {
    return time(tloc);
}

extern "C" int memcmp(const void* s1, const void* s2, size_t n) {
    return memcmp(s1, s2, n);
}
extern "C" void* memset(void* b, int c, size_t len) {
    return memset(b, c, len);
}


#endif