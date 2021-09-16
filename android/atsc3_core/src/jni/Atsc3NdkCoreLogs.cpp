#include <jni.h>
#include <string>
#include <map>
#include <vector>
#include "atsc3_logging_externs.h"

using namespace std;

map<string, int*> lookup;

extern "C"
JNIEXPORT void JNICALL
Java_org_ngbp_libatsc3_middleware_android_Atsc3NdkCoreLogs_init(JNIEnv *env, jobject thiz) {
    lookup.clear();
    for (int i = 0; i < ATSC3_CORE_LOGS_COUNT; i++) {
        lookup.insert(pair<string, int*>(string(ATSC3_CORE_LOGS_STR[i]), ATSC3_CORE_LOGS_PTR[i]));
    }
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_ngbp_libatsc3_middleware_android_Atsc3NdkCoreLogs_getAtsc3LogNames(JNIEnv *env, jobject thiz) {
    jobjectArray ret = env->NewObjectArray(ATSC3_CORE_LOGS_COUNT, env->FindClass("java/lang/String"), 0);

    int i = 0;
    map<string, int*>::iterator it;
    for(it = lookup.begin(); it != lookup.end(); it++, i++) {
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(it->first.c_str()));
    }

    return ret;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_ngbp_libatsc3_middleware_android_Atsc3NdkCoreLogs_getAtsc3LogEnabledNames(JNIEnv *env, jobject thiz) {
    vector<string> names;
    auto nit = names.begin();

    map<string, int*>::iterator it;
    for(it = lookup.begin(); it != lookup.end(); it++) {
        if (*it->second) {
            nit = names.insert(nit, it->first);
        }
    }

    jobjectArray ret = env->NewObjectArray(names.size(), env->FindClass("java/lang/String"), 0);
    int i = 0;
    for(nit = names.begin(); nit != names.end(); nit++, i++) {
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(nit->c_str()));
    }

    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_ngbp_libatsc3_middleware_android_Atsc3NdkCoreLogs_setAtsc3LogEnabledByName(JNIEnv *env, jobject thiz, jstring name, jint value) {
    const char* c_name = env->GetStringUTFChars(name, 0);

    try {
        *lookup.at(c_name) = value;
    } catch (const out_of_range& oor) {
        // ignore
    }

    env->ReleaseStringUTFChars(name, c_name);
}