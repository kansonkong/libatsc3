#include "SRTRxSTLTPVirtualPHYAndroid.h"

SRTRxSTLTPVirtualPHYAndroid* srtRxSTLTPVirtualPHYAndroid = nullptr;
mutex SRTRxSTLTPVirtualPHYAndroid::CS_global_mutex;

SRTRxSTLTPVirtualPHYAndroid::SRTRxSTLTPVirtualPHYAndroid(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = env->NewGlobalRef(jni_instance);
    this->setRxUdpPacketProcessCallback(atsc3_core_service_bridge_process_packet_from_plp_and_block);
    this->setRxLinkMappingTableProcessCallback(atsc3_phy_jni_bridge_notify_link_mapping_table);

}

/*
 * #7559 - Fix for destructor invoking delete of producer and consumerJni's, these must be
 *         deleted under the threadlocal which instanitated its jniEnv reference
 *          if(this->producerJniEnv) {
                 delete this->producerJniEnv;
            }
            if(this->consumerJniEnv) {
                delete this->producerJniEnv;
            }

    if(this->env) {
        if(this->jni_instance_globalRef) {
            env->DeleteGlobalRef(this->jni_instance_globalRef);
            this->jni_instance_globalRef = nullptr;
        }
    }
 */
SRTRxSTLTPVirtualPHYAndroid::~SRTRxSTLTPVirtualPHYAndroid() {

    _SRTRXSTLTP_VIRTUAL_PHY_ANDROID_INFO("SRTRxSTLTPVirtualPHYAndroid::~SRTRxSTLTPVirtualPHYAndroid - enter: deleting with this: %p", this);
    this->stop();

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_clear_callback();
    }

    _SRTRXSTLTP_VIRTUAL_PHY_ANDROID_INFO("SRTRxSTLTPVirtualPHYAndroid::~SRTRxSTLTPVirtualPHYAndroid - exit: deleting with this: %p", this);
}

void SRTRxSTLTPVirtualPHYAndroid::pinProducerThreadAsNeeded() {
    producerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_virtual_static_loader_get_javaVM());
}

void SRTRxSTLTPVirtualPHYAndroid::releasePinnedProducerThreadAsNeeded() {
    if(producerJniEnv) {
        delete producerJniEnv;
        producerJniEnv = nullptr;
    }
}

void SRTRxSTLTPVirtualPHYAndroid::pinConsumerThreadAsNeeded() {
    _SRTRXSTLTP_VIRTUAL_PHY_ANDROID_INFO("SRTRxSTLTPVirtualPHYAndroid::pinConsumerThreadAsNeeded - enter: with this: %p, chained: atsc3_ndk_application_bridge_get_instance: %p", this, atsc3_ndk_application_bridge_get_instance());
    consumerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_virtual_static_loader_get_javaVM());
    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->pinConsumerThreadAsNeeded();
        _SRTRXSTLTP_VIRTUAL_PHY_ANDROID_INFO("SRTRxSTLTPVirtualPHYAndroid::pinConsumerThreadAsNeeded - after call to atsc3_ndk_application_bridge_get_instance()->pinConsumerThreadAsNeeded()");
    }
}

void SRTRxSTLTPVirtualPHYAndroid::releasePinnedConsumerThreadAsNeeded() {
    _SRTRXSTLTP_VIRTUAL_PHY_WARN("SRTRxSTLTPVirtualPHYAndroid::releasePinnedConsumerThreadAsNeeded");
//    if(consumerJniEnv) {
//        delete consumerJniEnv;
//        consumerJniEnv = nullptr;
//    }

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->releasePinnedConsumerThreadAsNeeded();
    }
}

//org.ngbp.libatsc3.middleware.android.phy.virtual.srt.SRTRxSTLTPVirtualPHYAndroid
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_init(JNIEnv* env, jobject instance) {
    lock_guard<mutex> srt_rx_stltp_virtual_phy_android_cctor_mutex_local(SRTRxSTLTPVirtualPHYAndroid::CS_global_mutex);

    _SRTRXSTLTP_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_init: start init, env: %p", env);

    srtRxSTLTPVirtualPHYAndroid = new SRTRxSTLTPVirtualPHYAndroid(env, instance);

    _SRTRXSTLTP_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_init: return, srtRxSTLTPVirtualPHYAndroid: %p", srtRxSTLTPVirtualPHYAndroid);

    return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_setSrtSourceConnectionString(JNIEnv* env, jobject instance, jstring srtSourceConnectionString) {
    lock_guard<mutex> srt_rx_stltp_virtual_phy_android_cctor_mutex_local(SRTRxSTLTPVirtualPHYAndroid::CS_global_mutex);

    if(!srtRxSTLTPVirtualPHYAndroid) {
        _SRTRXSTLTP_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_setSrtSourceConnectionString: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        return;
    }

    const char *srtSourceConnectionString_cstr = env->GetStringUTFChars(srtSourceConnectionString, NULL);
    _SRTRXSTLTP_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_setSrtSourceConnectionString: with: %s", srtSourceConnectionString_cstr);
    srtRxSTLTPVirtualPHYAndroid->set_srt_source_connection_string(srtSourceConnectionString_cstr);

    env->ReleaseStringUTFChars(srtSourceConnectionString, srtSourceConnectionString_cstr);
    return;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_run(JNIEnv* env, jobject instance) {
    lock_guard<mutex> srt_rx_stltp_virtual_phy_android_cctor_mutex_local(SRTRxSTLTPVirtualPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!srtRxSTLTPVirtualPHYAndroid) {
        _SRTRXSTLTP_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_run: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        return -1;
    }
    atsc3_core_service_application_bridge_reset_context();

    res = srtRxSTLTPVirtualPHYAndroid->run();
    _SRTRXSTLTP_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_run: returning res: %d", res);

    return res;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_is_1running(JNIEnv* env, jobject instance) {
    lock_guard<mutex> srt_rx_stltp_virtual_phy_android_cctor_mutex_local(SRTRxSTLTPVirtualPHYAndroid::CS_global_mutex);

    jboolean res = false;

    if(!srtRxSTLTPVirtualPHYAndroid) {
        _SRTRXSTLTP_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_run: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        return false;
    }
    res = srtRxSTLTPVirtualPHYAndroid->is_running();
    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_stop(JNIEnv* env, jobject instance) {

    lock_guard<mutex> srt_rx_stltp_virtual_phy_android_cctor_mutex_local(SRTRxSTLTPVirtualPHYAndroid::CS_global_mutex);
    int res = 0;
    if(!srtRxSTLTPVirtualPHYAndroid) {
        _SRTRXSTLTP_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_stop: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        return -1;
    }
    res = srtRxSTLTPVirtualPHYAndroid->stop();
    _SRTRXSTLTP_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_stop: returning res: %d", res);

    return res;
}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_deinit(JNIEnv* env, jobject instance) {
    lock_guard<mutex> srt_rx_stltp_virtual_phy_android_cctor_mutex_local(SRTRxSTLTPVirtualPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!srtRxSTLTPVirtualPHYAndroid) {
        _SRTRXSTLTP_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTRxSTLTPVirtualPHYAndroid_deinit: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        return -1;
    }

    srtRxSTLTPVirtualPHYAndroid->deinit();
    srtRxSTLTPVirtualPHYAndroid = nullptr;

    return res;
}