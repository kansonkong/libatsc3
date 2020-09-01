#include "PcapDemuxedVirtualPHYAndroid.h"

PcapDemuxedVirtualPHYAndroid* pcapDemuxedVirtualPHYAndroid = nullptr;

PcapDemuxedVirtualPHYAndroid::PcapDemuxedVirtualPHYAndroid(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = env->NewGlobalRef(jni_instance);
    this->setRxUdpPacketProcessCallback(atsc3_core_service_bridge_process_packet_from_plp_and_block);
}

PcapDemuxedVirtualPHYAndroid::~PcapDemuxedVirtualPHYAndroid() {
    if(this->env) {
        if(this->jni_instance_globalRef) {
            env->DeleteGlobalRef(this->jni_instance_globalRef);
            this->jni_instance_globalRef = nullptr;
        }
    }
    //producer/consumer jni ref's must only be deleted from their calling thread
}

void PcapDemuxedVirtualPHYAndroid::pinProducerThreadAsNeeded() {
    producerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_virtual_static_loader_get_javaVM());
}

void PcapDemuxedVirtualPHYAndroid::releasePinnedProducerThreadAsNeeded() {
    delete producerJniEnv;
    producerJniEnv = nullptr;
}

void PcapDemuxedVirtualPHYAndroid::pinConsumerThreadAsNeeded() {
    consumerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_virtual_static_loader_get_javaVM());
    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->pinConsumerThreadAsNeeded();
    }
}

void PcapDemuxedVirtualPHYAndroid::releasePinnedConsumerThreadAsNeeded() {
    delete consumerJniEnv;
    consumerJniEnv = nullptr;
}

//org.ngbp.libatsc3.middleware.android.phy.virtual.srt.PcapDemuxedVirtualPHYAndroid
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_init(JNIEnv* env, jobject instance)
{
    _PCAP_DEMUXED_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_init: start init, env: %p", env);

    pcapDemuxedVirtualPHYAndroid = new PcapDemuxedVirtualPHYAndroid(env, instance);

    _PCAP_DEMUXED_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_init: return, PcapDemuxedVirtualPHYAndroid: %p", pcapDemuxedVirtualPHYAndroid);

    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_open_1from_1capture(JNIEnv *env, jobject thiz, jstring filename) {

    if(!pcapDemuxedVirtualPHYAndroid) {
        _PCAP_DEMUXED_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_open_1from_1capture: error, PcapDemuxedVirtualPHYAndroid is NULL!");
        return -1;
    }

    int res = 0;
    const char *filename_cstr = env->GetStringUTFChars(filename, NULL);
    _PCAP_DEMUXED_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_open_1from_1capture: with filename: %s", filename_cstr);
    res = pcapDemuxedVirtualPHYAndroid->atsc3_pcap_replay_open_file(filename_cstr);

    env->ReleaseStringUTFChars(filename, filename_cstr);
    return res;
}

//
//extern "C" JNIEXPORT void JNICALL
//Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_setSrtSourceConnectionString(JNIEnv* env, jobject instance, jstring srtSourceConnectionString)
//{
//    if(!pcapDemuxedVirtualPHYAndroid) {
//        _PCAP_DEMUXED_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_setSrtSourceConnectionString: error, PcapDemuxedVirtualPHYAndroid is NULL!");
//        return;
//    }
//
//    const char *srtSourceConnectionString_cstr = env->GetStringUTFChars(srtSourceConnectionString, NULL);
//    _PCAP_DEMUXED_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_setSrtSourceConnectionString: with: %s", srtSourceConnectionString_cstr);
//    pcapDemuxedVirtualPHYAndroid->set_srt_source_connection_string(srtSourceConnectionString_cstr);
//
//    env->ReleaseStringUTFChars(srtSourceConnectionString, srtSourceConnectionString_cstr);
//    return;
//}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_run(JNIEnv* env, jobject instance)
{
    int res = 0;
    if(!pcapDemuxedVirtualPHYAndroid) {
        _PCAP_DEMUXED_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_run: error, PcapDemuxedVirtualPHYAndroid is NULL!");
        return -1;
    }
    atsc3_core_service_application_bridge_reset_context();

    res = pcapDemuxedVirtualPHYAndroid->run();
    _PCAP_DEMUXED_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_run: returning res: %d", res);

    return res;
}



extern "C" JNIEXPORT jboolean JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_is_1running(JNIEnv* env, jobject instance)
{
    jboolean res = false;

    if(!pcapDemuxedVirtualPHYAndroid) {
        _PCAP_DEMUXED_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_run: error, PcapDemuxedVirtualPHYAndroid is NULL!");
        return false;
    }
    res = pcapDemuxedVirtualPHYAndroid->is_running();
    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_stop(JNIEnv* env, jobject instance)
{
    int res = 0;
    if(!pcapDemuxedVirtualPHYAndroid) {
        _PCAP_DEMUXED_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_stop: error, PcapDemuxedVirtualPHYAndroid is NULL!");
        return -1;
    }
    res = pcapDemuxedVirtualPHYAndroid->stop();
    _PCAP_DEMUXED_VIRTUAL_PHY_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_stop: returning res: %d", res);

    return res;
}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_deinit(JNIEnv* env, jobject instance)
{
    int res = 0;
    if(!pcapDemuxedVirtualPHYAndroid) {
        _PCAP_DEMUXED_VIRTUAL_PHY_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_PcapDemuxedVirtualPHYAndroid_deinit: error, PcapDemuxedVirtualPHYAndroid is NULL!");
        return -1;
    }

    pcapDemuxedVirtualPHYAndroid->deinit();
    pcapDemuxedVirtualPHYAndroid = nullptr;

    return res;
}