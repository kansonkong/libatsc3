#include "Atsc3NdkPHYBridge.h"

Atsc3NdkPHYBridge* api = NULL;

Atsc3NdkPHYBridge::Atsc3NdkPHYBridge(JavaVM *vm) {
    this->javaVM = vm;
}

void Atsc3NdkPHYBridge::LogMsg(const char *msg)
{
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady() || !mOnLogMsgId)
        return;
    Atsc3JniEnv env(this->javaVM);
    if (!env) {
        NDK_PHY_BRIDGE_ERROR("!! err on get jni env");
        return;
    }
    jstring js = env.Get()->NewStringUTF(msg);
    int r = env.Get()->CallIntMethod(jniInstance, mOnLogMsgId, js);
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
    if (!JReady() || !atsc3_rf_phy_status_callback_ID)
        return;

    if (!Atsc3_Jni_Status_Thread_Env) {
        NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge:atsc3_update_rf_stats: err on get jni env: Atsc3_Jni_Status_Thread_Env");
        return;
    }
    int r = Atsc3_Jni_Status_Thread_Env->Get()->CallIntMethod(jniInstance,
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
    if (!JReady() || !atsc3_update_rf_bw_stats_ID)
        return;
    if (!Atsc3_Jni_Status_Thread_Env) {
        NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge:atsc3_update_rf_bw_stats: err on get jni env: Atsc3_Jni_Status_Thread_Env");
        return;
    }
    int r = Atsc3_Jni_Status_Thread_Env->Get()->CallIntMethod(jniInstance,
            atsc3_update_rf_bw_stats_ID,
            total_pkts,
            total_bytes,
            total_lmts);
}

void Atsc3NdkPHYBridge::setRfPhyStatisticsViewVisible(bool isRfPhyStatisticsVisible) {
    rxStatusThreadShouldRun = isRfPhyStatisticsVisible;
}

//Java to native methods

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    api = new Atsc3NdkPHYBridge(vm);
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass jClazz = env->FindClass("org/ngbp/libatsc3/middleware/Atsc3NdkPHYBridge");
    if (jClazz == NULL) {
        NDK_PHY_BRIDGE_ERROR("PHY_BRIDGE::JNI_OnLoad - Cannot find org/ngbp/libatsc3/middleware/Atsc3NdkPHYBridge java class");
        return -1;
    }

    api->mOnLogMsgId = env->GetMethodID(jClazz, "onLogMsg", "(Ljava/lang/String;)I");
    if (api->mOnLogMsgId == NULL) {
        NDK_PHY_BRIDGE_ERROR("PHY_BRIDGE::JNI_OnLoad - Cannot find 'onLogMsg' method id");
        return -1;
    }

    api->atsc3_rf_phy_status_callback_ID = env->GetMethodID(jClazz, "atsc3_rf_phy_status_callback", "(IIIIIIIIIIIIIII)I");
    if (api->atsc3_rf_phy_status_callback_ID == NULL) {
        NDK_PHY_BRIDGE_ERROR("PHY_BRIDGE::JNI_OnLoad - 'atsc3_rf_phy_status_callback' method id");
        return -1;
    }

    //atsc3_update_rf_bw_stats_ID
    api->atsc3_update_rf_bw_stats_ID = env->GetMethodID(jClazz, "atsc3_updateRfBwStats", "(JJI)I");
    if (api->atsc3_update_rf_bw_stats_ID == NULL) {
        NDK_PHY_BRIDGE_ERROR("PHY_BRIDGE::JNI_OnLoad - Cannot find 'atsc3_update_rf_bw_stats_ID' method id");
        return -1;
    }

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_init(JNIEnv *env, jobject instance)
{
    printf("Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_Init: start init, env: %p\n", env);

    api->setJniInstance((jclass) env->NewGlobalRef(instance));

    printf("atsc3NdkPHYBridge_Init: with jniInstance: %p", api->getJniInstance());
    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_atsc3NdkPHYBridge_setRfPhyStatisticsViewVisible(JNIEnv *env, jobject thiz, jboolean is_rf_phy_statistics_visible) {
    if(is_rf_phy_statistics_visible) {
        api->setRfPhyStatisticsViewVisible(true);
    } else {
        api->setRfPhyStatisticsViewVisible(false);
    }
    return 0;
}