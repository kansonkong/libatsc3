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
#include <pthread.h>

extern "C"  int raise(int sig) {
    int rc = pthread_kill(pthread_self(), sig);
    if (rc != 0) {
        //errno = rc;
        return -1;
  }
  return 0;
}

// Our unoptimized memcpy just calls the best bcopy available.
// (It's this way round rather than the opposite because we're based on BSD source.)
extern "C" void* memcpy(void* dst, const void* src, size_t n) {
  bcopy(src, dst, n);
  return dst;
}

time_t my_time = 0;
extern "C" time_t time(time_t* tloc) {
    return my_time;
}

//extern "C" int memcmp(const void* s1, const void* s2, size_t n) {
//    return memcmp(s1, s2, n);
//}
int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char*  p1   = (unsigned char*) s1;
    const unsigned char*  end1 = p1 + n;
    const unsigned char*  p2   = (unsigned char*) s2;
    int                   d = 0;
    for (;;) {
        if (d || p1 >= end1) break;
        d = (int)*p1++ - (int)*p2++;
        if (d || p1 >= end1) break;
        d = (int)*p1++ - (int)*p2++;
        if (d || p1 >= end1) break;
        d = (int)*p1++ - (int)*p2++;
        if (d || p1 >= end1) break;
        d = (int)*p1++ - (int)*p2++;
    }
    return d;
}
extern "C" void*  memset(void*  dst, int c, size_t n) {
    char*  q   = (char*) dst;
    char*  end = q + n;
    for (;;) {
        if (q >= end) break; *q++ = (char) c;
        if (q >= end) break; *q++ = (char) c;
        if (q >= end) break; *q++ = (char) c;
        if (q >= end) break; *q++ = (char) c;
    }
  return dst;
}


#endif