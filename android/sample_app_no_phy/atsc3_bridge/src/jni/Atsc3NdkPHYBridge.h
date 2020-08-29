//
// Created by Jason Justman on 2019-09-27.
//

#include <jni.h>
#include <string.h>
#include <sys/types.h>
#include <thread>
#include <map>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <list>

using namespace std;

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#ifndef LIBATSC3_ATSC3NDKPHYBRIDGE_H
#define LIBATSC3_ATSC3NDKPHYBRIDGE_H

#define MODULE_NAME "Atsc3NdkPHYBridge"

#include "Atsc3JniEnv.h"
#include "Atsc3LoggingUtils.h"

// libatsc3 type imports here
#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>
#include <atsc3_pcap_type.h>
#include <atsc3_route_package_utils.h>
#include <phy/IAtsc3NdkPHYBridge.h>

#include "Atsc3BridgeNdkStaticJniLoader.h"

#include <atsc3_core_service_player_bridge.h>

class Atsc3NdkPHYBridge : public IAtsc3NdkPHYBridge
{
public:
    Atsc3NdkPHYBridge(JNIEnv* env, jobject jni_instance);

    void LogMsg(const char *msg);
    void LogMsg(const std::string &msg);
    void LogMsgF(const char *fmt, ...);

    void atsc3_update_rf_stats(int32_t tuner_lock,    //1
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
                                       uint8_t demod_lock,
                                       uint8_t signal,
                                       uint8_t plp_any,
                                       uint8_t plp_all); //15

    void atsc3_update_rf_bw_stats(uint64_t total_pkts, uint64_t total_bytes, unsigned int total_lmts);

    void setRfPhyStatisticsViewVisible(bool isRfPhyStatisticsVisible);


private:
    JNIEnv* env = nullptr;
    jobject jni_instance_globalRef = nullptr;
    jclass jni_class_globalRef = nullptr;

public:
    JavaVM* mJavaVM = nullptr;    // Java VM, if we don't have a pinned thread context for dispatch

    void setJniClassReference(string jclass_name) {
        if(env) {
            jclass jclass_local = env->FindClass(jclass_name.c_str());
            jni_class_globalRef = reinterpret_cast<jclass>(env->NewGlobalRef(jclass_local));
        }
    }
    jclass getJniClassReference() {
        return jni_class_globalRef;
    }

    int pinCaptureThreadAsNeeded();
    int releasePinnedCaptureThreadAsNeeded();

    int pinStatusThreadAsNeeded();
    int releasePinnedStatusThreadAsNeeded();

    jmethodID mOnLogMsgId = nullptr;                     // java class method id
    jmethodID atsc3_rf_phy_status_callback_ID = nullptr; // java class method id for phy stats
    jmethodID atsc3_update_rf_bw_stats_ID = nullptr;     // java callback method id for by stats

    std::thread atsc3_rxStatusThread;

    void RxStatusThread();
    bool rxStatusThreadShouldRun;

protected:
    Atsc3JniEnv* pinnedCaptureJniEnv = nullptr;
    Atsc3JniEnv* pinnedStatusJniEnv = nullptr;

};

#define _NDK_PHY_BRIDGE_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _NDK_PHY_BRIDGE_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _NDK_PHY_BRIDGE_INFO(...)   	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _NDK_PHY_BRIDGE_DEBUG(...)   	__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define _NDK_PHY_BRIDGE_TRACE(...)   	__LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);


#endif //LIBATSC3_ATSC3NDKPHYBRIDGE_H
