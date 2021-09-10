#include <jni.h>
#include <string>
#include "atsc3_logging_externs.h"

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_ngbp_libatsc3_middleware_android_Atsc3NdkCoreLogs_getAtsc3LogNames(JNIEnv *env, jobject thiz) {
    jobjectArray ret = env->NewObjectArray(ATSC3_CORE_LOGS_COUNT, env->FindClass("java/lang/String"), 0);

    for (int i = 0; i < ATSC3_CORE_LOGS_COUNT; i++) {
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(ATSC3_CORE_LOGS_STR[i]));
    }

    return ret;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_ngbp_libatsc3_middleware_android_Atsc3NdkCoreLogs_getAtsc3LogEnabledNames(JNIEnv *env, jobject thiz) {
    const char *names[ATSC3_CORE_LOGS_COUNT];
    size_t index = 0;
    for (int i = 0; i < ATSC3_CORE_LOGS_COUNT; i++) {
        if (*ATSC3_CORE_LOGS_PTR[i]) {
            names[index++] = ATSC3_CORE_LOGS_STR[i];
        }
    }

    jobjectArray ret = env->NewObjectArray(index, env->FindClass("java/lang/String"), 0);
    for (int i = 0; i < index; i++) {
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(names[i]));
    }

    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_ngbp_libatsc3_middleware_android_Atsc3NdkCoreLogs_setAtsc3LogEnabledByName(JNIEnv *env, jobject thiz, jstring name, jint value) {
    const char* c_name = env->GetStringUTFChars(name, 0);

    for (int i = 0; i < ATSC3_CORE_LOGS_COUNT; i++) {
        if (strcmp(c_name, ATSC3_CORE_LOGS_STR[i]) == 0) {
            (*ATSC3_CORE_LOGS_PTR[i]) = (int) value;
            break;
        }
    }

    env->ReleaseStringUTFChars(name, c_name);
}