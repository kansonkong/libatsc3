#include "Atsc3NdkPHYBridge.h"

#include <atsc3_lls_types.h>
#include <atsc3_pcap_type.h>
#include <atsc3_monitor_events_alc.h>

#include <atsc3_core_service_player_bridge.h>

Atsc3NdkPHYBridge api;

int Atsc3NdkPHYBridge::Init()
{
    atsc3_phy_player_bridge_init(&api);
    mbInit = true;
    return 0;
}

void Atsc3NdkPHYBridge::LogMsg(const char *msg)
{
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady() || !mOnLogMsgId)
        return;
    Atsc3JniEnv env(mJavaVM);
    if (!env) {
        NDK_PHY_BRIDGE_ERROR("!! err on get jni env");
        return;
    }
    jstring js = env.Get()->NewStringUTF(msg);
    int r = env.Get()->CallIntMethod(mClsDrvIntf, mOnLogMsgId, js);
    env.Get()->DeleteLocalRef(js);
}

void Atsc3NdkPHYBridge::LogMsg(const std::string &str)
{
    LogMsg(str.c_str());
}

void Atsc3NdkPHYBridge::LogMsgF(const char *fmt, ...)
{
    va_list v;
    char msg[1024];
    va_start(v, fmt);
    vsnprintf(msg, sizeof(msg)-1, fmt, v);
    msg[sizeof(msg)-1] = 0;
    va_end(v);
    LogMsg(msg);
}

void Atsc3NdkPHYBridge::atsc3_update_rf_stats(int32_t tuner_lock,
                                              int32_t rssi,
                                              uint8_t modcod_valid,
                                              uint8_t plp_fec_type,
                                              uint8_t plp_mod,
                                              uint8_t plp_cod,
                                              int32_t nRfLevel1000,
                                              int32_t nSnr1000,
                                              uint32_t ber_pre_ldpc_e7,
                                              uint32_t ber_pre_bch_e9,
                                              uint32_t fer_post_bch_e6,
                                              uint8_t demod_lock_status,
                                              uint8_t cpu_status,
                                              uint8_t plp_any,
                                              uint8_t plp_all) {

    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady() || !mOnLogMsgId)
        return;

    if (!Atsc3_Jni_Status_Thread_Env) {
        NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge:atsc3_update_rf_stats: err on get jni env: Atsc3_Jni_Status_Thread_Env");
        return;
    }
    int r = Atsc3_Jni_Status_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_rf_phy_status_callback_ID,
                                                              tuner_lock,
                                                              rssi,
                                                              modcod_valid,
                                                              plp_fec_type,
                                                              plp_mod,
                                                              plp_cod,
                                                              nRfLevel1000,
                                                              nSnr1000,
                                                              ber_pre_ldpc_e7,
                                                              ber_pre_bch_e9,
                                                              fer_post_bch_e6,
                                                              demod_lock_status,
                                                              cpu_status,
                                                              plp_any,
                                                              plp_all);

}


void Atsc3NdkPHYBridge::atsc3_update_rf_bw_stats(uint64_t total_pkts, uint64_t total_bytes,
                                                 unsigned int total_lmts) {
    if (!JReady() || !mOnLogMsgId)
        return;
    if (!Atsc3_Jni_Status_Thread_Env) {
        NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge:atsc3_update_rf_bw_stats: err on get jni env: Atsc3_Jni_Status_Thread_Env");
        return;
    }
    int r = Atsc3_Jni_Status_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_update_rf_bw_stats_ID, total_pkts, total_bytes, total_lmts);
}

//Java to native methods



extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiInit(JNIEnv *env, jobject instance, jobject drvIntf)
{
    printf("Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiInit: start init, env: %p\n", env);

    api.mJniEnv = env;

    env->GetJavaVM(&api.mJavaVM);
    if(api.mJavaVM == NULL) {
        NDK_PHY_BRIDGE_ERROR("!! no java vm");
        return -1;
    }

    jclass jClazz = env->FindClass("org/ngbp/libatsc3/middleware/Atsc3NdkPHYBridge");
    if (jClazz == NULL) {
        NDK_PHY_BRIDGE_ERROR("!! Cannot find org/ngbp/libatsc3/middleware/Atsc3NdkPHYBridge java class");
        return -1;
    }
    api.mOnLogMsgId = env->GetMethodID(jClazz, "onLogMsg", "(Ljava/lang/String;)I");
    if (api.mOnLogMsgId == NULL) {
        NDK_PHY_BRIDGE_ERROR("!! Cannot find 'onLogMsg' method id");
        return -1;
    }

    api.atsc3_rf_phy_status_callback_ID = env->GetMethodID(jClazz, "atsc3_rf_phy_status_callback", "(IIIIIIIIIIIIIII)I");
    if (api.atsc3_rf_phy_status_callback_ID == NULL) {
        NDK_PHY_BRIDGE_ERROR("!! Cannot find 'atsc3_rf_phy_status_callback' method id");
        return -1;
    }

    //atsc3_update_rf_bw_stats_ID
    api.atsc3_update_rf_bw_stats_ID = env->GetMethodID(jClazz, "atsc3_updateRfBwStats", "(JJI)I");
    if (api.atsc3_update_rf_bw_stats_ID == NULL) {
        NDK_PHY_BRIDGE_ERROR("!! Cannot find 'atsc3_update_rf_bw_stats_ID' method id");
        return -1;
    }


    api.jni_java_util_ArrayList = (jclass) env->NewGlobalRef(env->FindClass("java/util/ArrayList"));
    NDK_PHY_BRIDGE_ERROR("creating api.jni_java_util_ArrayList");

    api.jni_java_util_ArrayList_cctor = env->GetMethodID(api.jni_java_util_ArrayList, "<init>", "(I)V");
    NDK_PHY_BRIDGE_ERROR("creating api.jni_java_util_ArrayList_cctor");
    api.jni_java_util_ArrayList_add  = env->GetMethodID(api.jni_java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");
    NDK_PHY_BRIDGE_ERROR("creating api.jni_java_util_ArrayList_add");

    api.mClsDrvIntf = (jclass)(api.mJniEnv->NewGlobalRef(drvIntf));

    int r = api.Init();
    if (r)
        return r;

    api.LogMsg("Api init ok");

    //wire up atsc3_phy_mmt_player_bridge
    //Atsc3NdkPHYBridge* at3DrvIntf_ptr
    atsc3_phy_player_bridge_init(&api);

    printf("**** jni init OK");
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiPrepare(JNIEnv *env, jobject instance, jstring devlist_, jint d1, jint d2)
{
    printf("jni prepare");

    const char *devlist = env->GetStringUTFChars(devlist_, 0);
    int r = api.Prepare(devlist, (int)d1, (int)d2);
    env->ReleaseStringUTFChars(devlist_, devlist);
    return r;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiFwLoad(JNIEnv *env, jobject instance, jlong key)
{
    printf("jni fwload");

//    int r = api.FwLoad((AT3_DEV_KEY) key);
//    return r;

    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiOpen(JNIEnv *env, jobject instance, jint fd, jlong key)
{
    int bus = (key >> 8) & 0xFF;
    int addr = key & 0xFF;

    printf("Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiOpen: invoking open with fd: %d, key: %d, bus: %d, addr: %d",
            fd, key, bus, addr);

    api.Open(fd, bus, addr);

    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiTune(JNIEnv *env, jobject instance, jint freqKHz, jint plpid)
{
    printf("Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiTune::tune");

    return api.Tune(freqKHz, plpid);
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiStop(JNIEnv *env, jobject instance)
{
    printf("jni stop");
    return api.Stop();
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiClose(JNIEnv *env, jobject instance)
{
    printf("ApiClose:");
    return api.Close();
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiUninit(JNIEnv *env, jobject instance)
{
    printf("ApiUninit:");
    int r = api.Uninit();

    if (api.mClsDrvIntf) {
        api.mJniEnv->DeleteGlobalRef(api.mClsDrvIntf);
        api.mClsDrvIntf = nullptr;
    }

    return r;
}
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiReset(JNIEnv *env, jobject instance)
{
    printf("jni reset:");
    return api.Reset();
}

extern "C" JNIEXPORT jlongArray JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiFindDeviceKey(JNIEnv *env, jobject instance, jboolean bPreBootDevice)
{
//    printf("jni find %s devices\n", bPreBootDevice ? "preboot" : "atlas");
//    std::vector<AT3_DEV_KEY> vKeys = api.FindKeys(bPreBootDevice);

//    if (vKeys.empty())
//        return NULL;
    jlongArray arr;
//    arr = env->NewLongArray(vKeys.size());
//
//    std::vector<jlong> vTmp;
//    for (int i=0; i<vKeys.size(); i++)
//        vTmp.push_back(vKeys[i]);
//
//    env->SetLongArrayRegion(arr, 0, vKeys.size(), &vTmp[0]);
    return NULL;
}

//hacks

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_ApiSetPLP(JNIEnv *env, jobject thiz, jintArray a_plp_ids) {
    // TODO: implement ApiSetPLP()
//
//    jsize len = *env->GetArrayLength(a_plp_ids);
//    jint *a_body = *env->GetIntArrayElements(a_plp_ids, 0);
////    for (int i=0; i<len; i++) {
////        sum += body[i];
////    }
//    api.set_plp_settings(a_body, len);

    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_setRfPhyStatisticsViewVisible(JNIEnv *env, jobject thiz, jboolean is_rf_phy_statistics_visible) {
//    if(is_rf_phy_statistics_visible) {
//        Atsc3NdkPHYBridge::tunerStatusThreadShouldPollTunerStatus = true;
//    } else {
//        Atsc3NdkPHYBridge::tunerStatusThreadShouldPollTunerStatus = false;
//    }

    return 0;
}


int Atsc3NdkPHYBridge::ListenPLP1(int plp1) {
    //apiImpl.ListenPLP1(plp1);
    return 0;
}
