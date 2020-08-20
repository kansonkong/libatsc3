//
// Created by Jason Justman on 8/7/20.
//

#ifndef LIBATSC3_ANDROID_ATSC3JNIENV_H
#define LIBATSC3_ANDROID_ATSC3JNIENV_H

#include <jni.h>

class Atsc3JniEnv
{
public:
    Atsc3JniEnv(JavaVM *jvm): mJvm(jvm) {
        int r = jvm->GetEnv((void **)&mJniEnv, JNI_VERSION_1_6);

        if (r == JNI_OK) {
            return;
        }

        r = jvm->AttachCurrentThread(&mJniEnv, 0);

        if (r == 0) {
            mAttached = true;
        }
    }
    virtual ~Atsc3JniEnv() {
        if (mJniEnv && mAttached)
            mJvm->DetachCurrentThread();
    }
    operator bool() {
        return mJniEnv != nullptr;
    }
    JNIEnv *Get() {
        return mJniEnv;
    }
private:
    JNIEnv *mJniEnv = nullptr;
    JavaVM *mJvm    = nullptr;
    bool mAttached  = false;

};

#endif //LIBATSC3_ANDROID_ATSC3JNIENV_H
