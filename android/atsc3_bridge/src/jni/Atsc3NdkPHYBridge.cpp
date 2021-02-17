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

void Atsc3NdkPHYBridge::atsc3_notify_phy_error(const char* fmt, ...) {
    va_list v;
    char msg[1024];
    va_start(v, fmt);
    vsnprintf(msg, sizeof(msg)-1, fmt, v);
    msg[sizeof(msg)-1] = 0;
    va_end(v);

    _NDK_PHY_BRIDGE_WARN("Atsc3NdkPHYBridge::atsc3_notify_phy_error: %s", msg);

    if (!mOnPhyErrorId) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge::atsc3_notify_phy_error, unable to find mOnPhyErrorId callback method id!");
        return;
    }

    Atsc3JniEnv env(mJavaVM);
    if (!env) {
        _NDK_PHY_BRIDGE_ERROR("atsc3_notify_phy_error: error creating env pin!");
        return;
    }
    jstring js = env.Get()->NewStringUTF(msg);
    int r = env.Get()->CallIntMethod(jni_instance_globalRef, mOnPhyErrorId, js);
    env.Get()->DeleteLocalRef(js);
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

void Atsc3NdkPHYBridge::atsc3_update_rf_stats_from_atsc3_ndk_phy_client_rf_metrics_t(atsc3_ndk_phy_client_rf_metrics_t* atsc3_ndk_phy_client_rf_metrics) {
    JNIEnv* env = pinnedStatusJniEnv->Get();

    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!atsc3_rf_phy_status_callback_with_rf_phy_statistics_type_ID)
        return;

    if (!pinnedStatusJniEnv) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge:atsc3_update_rf_stats_from_atsc3_ndk_phy_client_rf_metrics_t: err on get jni env: pinnedStatusJniEnv");
        return;
    }

    jclass jcls = atsc3_nkd_phy_client_rf_metrics_jclass_global_ref;
    jobject jobj = env->AllocObject(jcls);

    if(!jobj) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge:atsc3_update_rf_stats_from_atsc3_ndk_phy_client_rf_metrics_t::err unable to allocate atsc3_nkd_phy_client_rf_metrics_jclass_global_ref instance jobj!");
        return;
    }

    //map fields...
    env->SetIntField(jobj, env->GetFieldID(jcls, "tuner_lock", "I"), atsc3_ndk_phy_client_rf_metrics->tuner_lock);
    env->SetIntField(jobj, env->GetFieldID(jcls, "demod_lock", "I"), atsc3_ndk_phy_client_rf_metrics->demod_lock);

    //some casting over the jni buffer
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_lock_any", "I"), (int32_t)atsc3_ndk_phy_client_rf_metrics->plp_lock_any);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_lock_all", "I"), (int32_t)atsc3_ndk_phy_client_rf_metrics->plp_lock_all);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_lock_by_setplp_index", "I"), (int32_t)atsc3_ndk_phy_client_rf_metrics->plp_lock_by_setplp_index);

    env->SetIntField(jobj, env->GetFieldID(jcls, "cpu_status", "I"), atsc3_ndk_phy_client_rf_metrics->cpu_status);

    env->SetIntField(jobj, env->GetFieldID(jcls, "rssi", "I"), atsc3_ndk_phy_client_rf_metrics->rssi);
    env->SetIntField(jobj, env->GetFieldID(jcls, "snr1000", "I"), atsc3_ndk_phy_client_rf_metrics->snr1000);
    env->SetIntField(jobj, env->GetFieldID(jcls, "rfLevel1000", "I"), atsc3_ndk_phy_client_rf_metrics->rfLevel1000);

    env->SetIntField(jobj, env->GetFieldID(jcls, "bootstrap_system_bw", "I"), (int32_t) atsc3_ndk_phy_client_rf_metrics->bootstrap_system_bw);
    env->SetIntField(jobj, env->GetFieldID(jcls, "bootstrap_ea_wakeup", "I"), (int32_t) atsc3_ndk_phy_client_rf_metrics->bootstrap_ea_wakeup);

    //jjustman-2020-12-24 - HACK - todo - fix me for proper typing
    atsc3_ndk_phy_client_rf_plp_metrics_t* atsc3_ndk_phy_client_rf_plp_metrics = nullptr;

    atsc3_ndk_phy_client_rf_plp_metrics = &atsc3_ndk_phy_client_rf_metrics->phy_client_rf_plp_metrics[0];
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_id_0", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_id);
    env->SetIntField(jobj, env->GetFieldID(jcls, "modcod_valid_0", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->modcod_valid);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_fec_type_0", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_fec_type);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_mod_0", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_mod);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_cod_0", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_cod);
    env->SetIntField(jobj, env->GetFieldID(jcls, "ber_pre_ldpc_0", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->ber_pre_ldpc);
    env->SetIntField(jobj, env->GetFieldID(jcls, "ber_pre_bch_0", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->ber_pre_bch);
    env->SetIntField(jobj, env->GetFieldID(jcls, "fer_post_bch_0", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->fer_post_bch);

    atsc3_ndk_phy_client_rf_plp_metrics = &atsc3_ndk_phy_client_rf_metrics->phy_client_rf_plp_metrics[1];
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_id_1", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_id);
    env->SetIntField(jobj, env->GetFieldID(jcls, "modcod_valid_1", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->modcod_valid);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_fec_type_1", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_fec_type);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_mod_1", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_mod);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_cod_1", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_cod);
    env->SetIntField(jobj, env->GetFieldID(jcls, "ber_pre_ldpc_1", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->ber_pre_ldpc);
    env->SetIntField(jobj, env->GetFieldID(jcls, "ber_pre_bch_1", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->ber_pre_bch);
    env->SetIntField(jobj, env->GetFieldID(jcls, "fer_post_bch_1", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->fer_post_bch);


    atsc3_ndk_phy_client_rf_plp_metrics = &atsc3_ndk_phy_client_rf_metrics->phy_client_rf_plp_metrics[2];
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_id_2", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_id);
    env->SetIntField(jobj, env->GetFieldID(jcls, "modcod_valid_2", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->modcod_valid);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_fec_type_2", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_fec_type);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_mod_2", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_mod);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_cod_2", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_cod);
    env->SetIntField(jobj, env->GetFieldID(jcls, "ber_pre_ldpc_2", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->ber_pre_ldpc);
    env->SetIntField(jobj, env->GetFieldID(jcls, "ber_pre_bch_2", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->ber_pre_bch);
    env->SetIntField(jobj, env->GetFieldID(jcls, "fer_post_bch_2", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->fer_post_bch);

    atsc3_ndk_phy_client_rf_plp_metrics = &atsc3_ndk_phy_client_rf_metrics->phy_client_rf_plp_metrics[3];
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_id_3", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_id);
    env->SetIntField(jobj, env->GetFieldID(jcls, "modcod_valid_3", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->modcod_valid);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_fec_type_3", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_fec_type);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_mod_3", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_mod);
    env->SetIntField(jobj, env->GetFieldID(jcls, "plp_cod_3", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->plp_cod);
    env->SetIntField(jobj, env->GetFieldID(jcls, "ber_pre_ldpc_3", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->ber_pre_ldpc);
    env->SetIntField(jobj, env->GetFieldID(jcls, "ber_pre_bch_3", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->ber_pre_bch);
    env->SetIntField(jobj, env->GetFieldID(jcls, "fer_post_bch_3", "I"), (int32_t) atsc3_ndk_phy_client_rf_plp_metrics->fer_post_bch);





    //invoke our callback with 'strong' type
    int r = pinnedStatusJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_rf_phy_status_callback_with_rf_phy_statistics_type_ID, jobj);


}


void Atsc3NdkPHYBridge::atsc3_update_rf_bw_stats(uint64_t total_pkts, uint64_t total_bytes, unsigned int total_lmts) {
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

    atsc3NdkPHYBridge->mOnPhyErrorId = env->GetMethodID(jniClassReference, "onPhyError", "(Ljava/lang/String;)I");
    if (atsc3NdkPHYBridge->mOnPhyErrorId == NULL) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge_init: cannot find 'onPhyError' method id");
        return -1;
    }

    atsc3NdkPHYBridge->atsc3_rf_phy_status_callback_ID = env->GetMethodID(jniClassReference, "atsc3_rf_phy_status_callback", "(IIIIIIIIIIIIIII)I");
    if (atsc3NdkPHYBridge->atsc3_rf_phy_status_callback_ID == NULL) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge_init: cannot find 'atsc3_rf_phy_status_callback' method id");
        return -1;
    }

    //jjustman-2020-12-24 - atsc3_rf_phy_status_callback_with_all_plps_ID
    atsc3NdkPHYBridge->atsc3_rf_phy_status_callback_with_rf_phy_statistics_type_ID = env->GetMethodID(jniClassReference, "atsc3_rf_phy_status_callback_with_rf_phy_statistics_type", "(Lorg/ngbp/libatsc3/middleware/android/phy/models/RfPhyStatistics;)I");
    if (atsc3NdkPHYBridge->atsc3_rf_phy_status_callback_with_rf_phy_statistics_type_ID == NULL) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge_init: cannot find 'atsc3_rf_phy_status_callback_with_all_plps_ID' method id");
        return -1;
    }

    atsc3NdkPHYBridge->atsc3_nkd_phy_client_rf_metrics_jclass_init_env = env->FindClass("org/ngbp/libatsc3/middleware/android/phy/models/RfPhyStatistics");

    if (atsc3NdkPHYBridge->atsc3_nkd_phy_client_rf_metrics_jclass_init_env == NULL) {
        _NDK_PHY_BRIDGE_ERROR("Atsc3NdkPHYBridge_init: cannot find 'RfPhyStatistics' class reference");
        return -1;
    } else {
        atsc3NdkPHYBridge->atsc3_nkd_phy_client_rf_metrics_jclass_global_ref = (jclass)(env->NewGlobalRef(atsc3NdkPHYBridge->atsc3_nkd_phy_client_rf_metrics_jclass_init_env));
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