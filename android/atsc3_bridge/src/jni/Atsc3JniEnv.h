//
// Created by Jason Justman on 8/7/20.
//

#ifndef LIBATSC3_ANDROID_ATSC3JNIENV_H
#define LIBATSC3_ANDROID_ATSC3JNIENV_H

#include <jni.h>
#include <stdlib.h>

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

    Atsc3JniEnv(JavaVM *jvm, const char *name): mJvm(jvm) {
        int r = jvm->GetEnv((void **)&mJniEnv, JNI_VERSION_1_6);

        if (r == JNI_OK) {
            return;
        }
        thread_name = strdup(name);

        args.version = JNI_VERSION_1_6;
        args.name = thread_name;
        args.group = NULL;

        r = jvm->AttachCurrentThread(&mJniEnv, &args);

        if (r == 0) {
            mAttached = true;
        }
    }
    virtual ~Atsc3JniEnv() {
        if (mJniEnv && mAttached) {
            mJvm->DetachCurrentThread();
        }
        if(thread_name) {
            free((void*)thread_name);
            thread_name = nullptr;
        }

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
    JavaVMAttachArgs args;
    bool mAttached  = false;
    char* thread_name = nullptr;


};

#endif //LIBATSC3_ANDROID_ATSC3JNIENV_H
