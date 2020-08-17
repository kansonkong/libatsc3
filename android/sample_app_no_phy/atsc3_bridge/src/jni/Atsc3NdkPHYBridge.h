//
// Created by Jason Justman on 2019-09-27.
//

#ifndef LIBATSC3_ATSC3NDKPHYBRIDGE_H
#define LIBATSC3_ATSC3NDKPHYBRIDGE_H

#include "Atsc3LoggingUtils.h"

#include <string.h>
#include <jni.h>
#include <thread>
#include <map>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <list>

#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

using namespace std;

#include "Atsc3JniEnv.h"

#define DEBUG 1

#define MODULE_NAME "intf"

// libatsc3 type imports here
#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>
#include <atsc3_pcap_type.h>
#include <atsc3_route_package_utils.h>
#include <phy/IAtsc3NdkPHYBridge.h>

/*
 * : public libatsc3_Iphy_mockable
 * : public IAtsc3NdkPHYClient
 */

/* phy callback method(s)
 * jjustman:2020-08-10 - TODO - refactor out
    int atsc3_rx_callback_f(void*, uint64_t ullUser);
*/

class Atsc3NdkPHYBridge : public IAtsc3NdkPHYBridge
{
public:
    Atsc3NdkPHYBridge(JavaVM* vm);

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

    void setJniInstance(jclass jniInstance) {  this->jniInstance = jniInstance; }
    jclass getJniInstance() { return this->jniInstance; }

private:
    // jni stuff
    JavaVM* javaVM = nullptr;       // Java VM
    JNIEnv* jniEnvPinned = nullptr;    // Jni Environment pinned to our "dispatcher" thread

    jclass jniInstance = nullptr;   // instance configured from Init() method for callbacks to be dispatched on

public:
    jmethodID mOnLogMsgId = nullptr;                     // java class method id
    jmethodID atsc3_rf_phy_status_callback_ID = nullptr; // java class method id for phy stats
    jmethodID atsc3_update_rf_bw_stats_ID = nullptr;     // java callback method id for by stats

private:

    Atsc3JniEnv* Atsc3_Jni_Status_Thread_Env = NULL;

    bool JReady() {
        return javaVM && jniEnvPinned && jniInstance ? true : false;
    }


    //todo: refactor this out - ala https://gist.github.com/qiao-tw/6e43fb2311ee3c31752e11a4415deeb1

    std::thread atsc3_rxStatusThread;
    void RxStatusThread();
    bool rxStatusThreadShouldRun;

};

#define NDK_PHY_BRIDGE_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define NDK_PHY_BRIDGE_INFO(...)    	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#endif //LIBATSC3_ATSC3NDKPHYBRIDGE_H
