#include "Atsc3NdkPHYBridge.h"

Atsc3NdkPHYBridge* atsc3NdkPHYBridge = nullptr;

Atsc3NdkPHYBridge::Atsc3NdkPHYBridge(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = env->NewGlobalRef(jni_instance);
}

//jjustman-2020-08-19: TODO: get (or create) a pinned Atsc3JniEnv from pthread_cur
void Atsc3NdkPHYBridge::LogMsg(const char *msg)
{
    if (!mOnLogMsgId)
        return;

    Atsc3JniEnv env(mJavaVM);
    if (!env) {
        _NDK_PHY_BRIDGE_ERROR("LogMsg: error creating env pin!");
        return;
    }
    jstring js = env.Get()->NewStringUTF(msg);
    int r = env.Get()->CallIntMethod(jni_instance_globalRef, mOnLogMsgId, js);
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
    if (!atsc3_rf_phy_status_callback_ID)
        return;

    if (!pinnedStatusJniEnv) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge:atsc3_update_rf_stats: err on get jni env: Atsc3_Jni_Status_Thread_Env");
        return;
    }
    int r = pinnedStatusJniEnv->Get()->CallIntMethod(jni_instance_globalRef,
                                                     atsc3_rf_phy_status_callback_ID,
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


void Atsc3NdkPHYBridge::atsc3_update_rf_bw_stats(uint64_t total_pkts,
                                                    uint64_t total_bytes,
                                                    unsigned int total_lmts) {
    if (!atsc3_update_rf_bw_stats_ID)
        return;

    if (!pinnedStatusJniEnv) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge:atsc3_update_rf_bw_stats: err on get jni env: Atsc3_Jni_Status_Thread_Env");
        return;
    }
    int r = pinnedStatusJniEnv->Get()->CallIntMethod(jni_instance_globalRef,
                                                     atsc3_update_rf_bw_stats_ID,
                                                     total_pkts,
                                                     total_bytes,
                                                     total_lmts);
}

void Atsc3NdkPHYBridge::setRfPhyStatisticsViewVisible(bool isRfPhyStatisticsVisible) {
    rxStatusThreadShouldRun = isRfPhyStatisticsVisible;
}

int Atsc3NdkPHYBridge::pinCaptureThreadAsNeeded() {
    _NDK_PHY_BRIDGE_DEBUG("Atsc3NdkPHYBridge::pinCaptureThreadAsNeeded: mJavaVM: %p", mJavaVM);
    pinnedCaptureJniEnv = new Atsc3JniEnv(mJavaVM);
    return 0;
};

int Atsc3NdkPHYBridge::releasePinnedCaptureThreadAsNeeded() {
    _NDK_PHY_BRIDGE_DEBUG("Atsc3NdkPHYBridge::releasePinnedCaptureThreadAsNeeded: pinnedCaptureJniEnv: %p", pinnedCaptureJniEnv);

    if(pinnedCaptureJniEnv) {
        delete pinnedCaptureJniEnv;
    }
    return 0;
}

int Atsc3NdkPHYBridge::pinStatusThreadAsNeeded() {
    _NDK_PHY_BRIDGE_DEBUG("Atsc3NdkPHYBridge::pinStatusThreadAsNeeded: mJavaVM: %p", mJavaVM);
    pinnedStatusJniEnv = new Atsc3JniEnv(mJavaVM);
    return 0;
}

int Atsc3NdkPHYBridge::releasePinnedStatusThreadAsNeeded() {
    _NDK_PHY_BRIDGE_DEBUG("Atsc3NdkPHYBridge::releasePinnedStatusThreadAsNeeded: pinnedStatusJniEnv: %p", pinnedStatusJniEnv);

    if(pinnedStatusJniEnv) {
        delete pinnedStatusJniEnv;
    }
    return 0;
}



extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkPHYBridge_init(JNIEnv *env, jobject instance)
{
    atsc3NdkPHYBridge = new Atsc3NdkPHYBridge(env, instance);
    _NDK_PHY_BRIDGE_INFO("Java_org_ngbp_libatsc3_middleware_Atsc3NdkPHYBridge_Init: start init, env: %p", env);
    atsc3NdkPHYBridge->setJniClassReference("org/ngbp/libatsc3/middleware/Atsc3NdkPHYBridge");
    atsc3NdkPHYBridge->mJavaVM = atsc3_bridge_ndk_static_loader_get_javaVM();

    if(atsc3NdkPHYBridge->mJavaVM == NULL) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge_init: atsc3NdkPHYBridge->mJavaVM is NULL!");
        return -1;
    }

    jclass jniClassReference = atsc3NdkPHYBridge->getJniClassReference();

    if (jniClassReference == NULL) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge_init: cannot find atsc3NdkPHYBridge java class reference!");
        return -2;
    }

    atsc3NdkPHYBridge->mOnLogMsgId = env->GetMethodID(jniClassReference, "onLogMsg", "(Ljava/lang/String;)I");
    if (atsc3NdkPHYBridge->mOnLogMsgId == NULL) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge_init: cannot find 'onLogMsg' method id");
        return -1;
    }

    atsc3NdkPHYBridge->atsc3_rf_phy_status_callback_ID = env->GetMethodID(jniClassReference, "atsc3_rf_phy_status_callback", "(IIIIIIIIIIIIIII)I");
    if (atsc3NdkPHYBridge->atsc3_rf_phy_status_callback_ID == NULL) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge_init: cannot find 'atsc3_rf_phy_status_callback' method id");
        return -1;
    }

    //atsc3_update_rf_bw_stats_ID
    atsc3NdkPHYBridge->atsc3_update_rf_bw_stats_ID = env->GetMethodID(jniClassReference, "atsc3_updateRfBwStats", "(JJI)I");
    if (atsc3NdkPHYBridge->atsc3_update_rf_bw_stats_ID == NULL) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge_init: cannot find 'atsc3_update_rf_bw_stats_ID' method id");
        return -1;
    }
    
    atsc3_core_service_phy_bridge_init(atsc3NdkPHYBridge);
    _NDK_PHY_BRIDGE_INFO("Atsc3NdkPHYBridge_init: done, with atsc3NdkPHYBridge: %p", atsc3NdkPHYBridge);
    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkPHYBridge_setRfPhyStatisticsViewVisible(JNIEnv *env, jobject thiz, jboolean is_rf_phy_statistics_visible) {
    if(is_rf_phy_statistics_visible) {
        atsc3NdkPHYBridge->setRfPhyStatisticsViewVisible(true);
    } else {
        atsc3NdkPHYBridge->setRfPhyStatisticsViewVisible(false);
    }
    return 0;
}