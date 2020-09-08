#include "PcapSTLTPVirtualPHYAndroid.h"

PcapSTLTPVirtualPHYAndroid* pcapSTLTPVirtualPHYAndroid = nullptr;

PcapSTLTPVirtualPHYAndroid::PcapSTLTPVirtualPHYAndroid(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = env->NewGlobalRef(jni_instance);
    this->setRxUdpPacketProcessCallback(atsc3_core_service_bridge_process_packet_from_plp_and_block);
}

PcapSTLTPVirtualPHYAndroid::~PcapSTLTPVirtualPHYAndroid() {
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

void PcapSTLTPVirtualPHYAndroid::pinProducerThreadAsNeeded() {
    producerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_virtual_static_loader_get_javaVM());
}

void PcapSTLTPVirtualPHYAndroid::releasePinnedProducerThreadAsNeeded() {
    delete producerJniEnv;
    producerJniEnv = nullptr;
}

void PcapSTLTPVirtualPHYAndroid::pinConsumerThreadAsNeeded() {
    consumerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_virtual_static_loader_get_javaVM());
    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->pinConsumerThreadAsNeeded();
    }
}

void PcapSTLTPVirtualPHYAndroid::releasePinnedConsumerThreadAsNeeded() {
    delete consumerJniEnv;
    consumerJniEnv = nullptr;
}

//org.ngbp.libatsc3.middleware.android.phy.virtual.srt.PcapSTLTPVirtualPHYAndroid
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_init(JNIEnv* env, jobject instance)
{
    _PCAP_STLTP_VIRTUAL_PHY_WARN("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_init: start init, env: %p", env);

    pcapSTLTPVirtualPHYAndroid = new PcapSTLTPVirtualPHYAndroid(env, instance);

    _PCAP_STLTP_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_init: return, PcapSTLTPVirtualPHYAndroid: %p", pcapSTLTPVirtualPHYAndroid);

    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_open_1from_1capture(JNIEnv *env, jobject thiz, jstring filename) {

    if(!pcapSTLTPVirtualPHYAndroid) {
        _PCAP_STLTP_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_open_1from_1capture: error, pcapSTLTPVirtualPHYAndroid is NULL!");
        return -1;
    }

    int res = 0;
    const char *filename_cstr = env->GetStringUTFChars(filename, NULL);
    _PCAP_STLTP_VIRTUAL_PHY_WARN("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_open_1from_1capture: with filename: %s", filename_cstr);
    res = pcapSTLTPVirtualPHYAndroid->atsc3_pcap_replay_open_file(filename_cstr);

    env->ReleaseStringUTFChars(filename, filename_cstr);
    return res;
}

//extern "C" JNIEXPORT void JNICALL
//Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_setSrtSourceConnectionString(JNIEnv* env, jobject instance, jstring srtSourceConnectionString)
//{
//    if(!pcapSTLTPVirtualPHYAndroid) {
//        _PCAP_STLTP_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_setSrtSourceConnectionString: error, PcapSTLTPVirtualPHYAndroid is NULL!");
//        return;
//    }
//
//    const char *srtSourceConnectionString_cstr = env->GetStringUTFChars(srtSourceConnectionString, NULL);
//    _PCAP_STLTP_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_setSrtSourceConnectionString: with: %s", srtSourceConnectionString_cstr);
//    pcapSTLTPVirtualPHYAndroid->set_srt_source_connection_string(srtSourceConnectionString_cstr);
//
//    env->ReleaseStringUTFChars(srtSourceConnectionString, srtSourceConnectionString_cstr);
//    return;
//}
//

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_run(JNIEnv* env, jobject instance)
{
    _PCAP_STLTP_VIRTUAL_PHY_INFO("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_run");

    int res = 0;
    if(!pcapSTLTPVirtualPHYAndroid) {
        _PCAP_STLTP_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_run: error, PcapSTLTPVirtualPHYAndroid is NULL!");
        return -1;
    }
    atsc3_core_service_application_bridge_reset_context();

    res = pcapSTLTPVirtualPHYAndroid->run();
    _PCAP_STLTP_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_run: returning res: %d", res);

    return res;
}



extern "C" JNIEXPORT jboolean JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_is_1running(JNIEnv* env, jobject instance)
{
    _PCAP_STLTP_VIRTUAL_PHY_INFO("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_is_1running");

    jboolean res = false;

    if(!pcapSTLTPVirtualPHYAndroid) {
        _PCAP_STLTP_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_run: error, PcapSTLTPVirtualPHYAndroid is NULL!");
        return false;
    }
    res = pcapSTLTPVirtualPHYAndroid->is_running();
    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_stop(JNIEnv* env, jobject instance)
{
    _PCAP_STLTP_VIRTUAL_PHY_INFO("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_stop");

    int res = 0;
    if(!pcapSTLTPVirtualPHYAndroid) {
        _PCAP_STLTP_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_stop: error, PcapSTLTPVirtualPHYAndroid is NULL!");
        return -1;
    }
    res = pcapSTLTPVirtualPHYAndroid->stop();
    _PCAP_STLTP_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_stop: returning res: %d", res);

    return res;
}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_deinit(JNIEnv* env, jobject instance)
{
    _PCAP_STLTP_VIRTUAL_PHY_INFO("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_deinit");

    int res = 0;
    if(!pcapSTLTPVirtualPHYAndroid) {
        _PCAP_STLTP_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapSTLTPVirtualPHYAndroid_deinit: error, PcapSTLTPVirtualPHYAndroid is NULL!");
        return -1;
    }

    pcapSTLTPVirtualPHYAndroid->deinit();
    pcapSTLTPVirtualPHYAndroid = nullptr;

    return res;
}