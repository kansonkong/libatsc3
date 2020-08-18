#include "SRTRxSTLTPVirtualPHYAndroid.h"

SRTRxSTLTPVirtualPHYAndroid* srtRxSTLTPVirtualPHYAndroid = nullptr;

SRTRxSTLTPVirtualPHYAndroid::SRTRxSTLTPVirtualPHYAndroid(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = env->NewGlobalRef(jni_instance);
    this->SetRxUdpPacketProcessCallback(atsc3_core_service_bridge_process_packet_from_plp_and_block);
}

SRTRxSTLTPVirtualPHYAndroid::~SRTRxSTLTPVirtualPHYAndroid() {
    if(this->env) {
        if(this->jni_instance_globalRef) {
            env->DeleteGlobalRef(this->jni_instance_globalRef);
            this->jni_instance_globalRef = nullptr;
        }
    }
    if(this->producerJniEnv) {
        delete this->producerJniEnv;
    }
    if(this->consumerJniEnv) {
        delete this->producerJniEnv;
    }
}

void SRTRxSTLTPVirtualPHYAndroid::pinProducerThreadAsNeeded() {
    producerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_virtual_static_loader_get_javaVM());
}

void SRTRxSTLTPVirtualPHYAndroid::releaseProducerThreadAsNeeded() {
    delete producerJniEnv;
    producerJniEnv = nullptr;
}

void SRTRxSTLTPVirtualPHYAndroid::pinConsumerThreadAsNeeded() {
    consumerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_virtual_static_loader_get_javaVM());
    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->pinFromRxProcessingThread();
    }
}

void SRTRxSTLTPVirtualPHYAndroid::releaseConsumerThreadAsNeeded() {
    delete consumerJniEnv;
    consumerJniEnv = nullptr;
}

//org.ngbp.libatsc3.middleware.android.phy.virtual.srt.SRTRxSTLTPVirtualPHYAndroid
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_init(JNIEnv* env, jobject instance)
{
    printf("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_init: start init, env: %p\n", env);

    srtRxSTLTPVirtualPHYAndroid = new SRTRxSTLTPVirtualPHYAndroid(env, instance);

    printf("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_init: return, env: %p\n", env);

    return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_setSrtSourceConnectionString(JNIEnv* env, jobject instance, jstring srtSourceConnectionString)
{
    if(!srtRxSTLTPVirtualPHYAndroid) {
        printf("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_setSrtSourceConnectionString: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        return;
    }

    const char *srtSourceConnectionString_cstr = env->GetStringUTFChars(srtSourceConnectionString, NULL);
    printf("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_setSrtSourceConnectionString: with: %s", srtSourceConnectionString_cstr);
    srtRxSTLTPVirtualPHYAndroid->set_srt_source_connection_string(srtSourceConnectionString_cstr);

    env->ReleaseStringUTFChars(srtSourceConnectionString, srtSourceConnectionString_cstr);
    return;
}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_run(JNIEnv* env, jobject instance)
{
    int res = 0;
    if(!srtRxSTLTPVirtualPHYAndroid) {
        printf("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_run: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        return -1;
    }
    res = srtRxSTLTPVirtualPHYAndroid->atsc3_srt_thread_run();
    printf("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_run: returning res: %d", res);

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_stop(JNIEnv* env, jobject instance)
{
    int res = 0;
    if(!srtRxSTLTPVirtualPHYAndroid) {
        printf("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_stop: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        return -1;
    }
    res = srtRxSTLTPVirtualPHYAndroid->atsc3_srt_thread_stop();
    printf("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_stop: returning res: %d", res);

    return res;
}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_deinit(JNIEnv* env, jobject instance)
{
    int res = 0;
    if(!srtRxSTLTPVirtualPHYAndroid) {
        printf("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_deinit: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        return -1;
    }

    delete srtRxSTLTPVirtualPHYAndroid;
    srtRxSTLTPVirtualPHYAndroid = nullptr;

    return res;
}