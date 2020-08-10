//
// Created by Jason Justman on 2019-09-27.
//

#ifndef LIBATSC3_PCAPDEMUXEDVIRTUALPHY_H
#define LIBATSC3_PCAPDEMUXEDVIRTUALPHY_H

#include <string>
#include <thread>
#include <map>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <list>

#include <sys/types.h>

using namespace std;

#define DEBUG 1

#include "../IAtsc3NdkPHYClient.h"

// libatsc3 type imports here
#include <atsc3_utils.h>
#include <atsc3_pcap_type.h>
#include <atsc3_route_package_utils.h>

/*
 * : public libatsc3_Iphy_mockable
 */

class PcapDemuxedVirtualPHY : public IAtsc3NdkPHYClient
{
public:
    PcapDemuxedVirtualPHY(): mbInit(false), mbLoop(false), mbRun(false) {    }

    int Init();
    int Open(int fd, int bus, int addr);
    int Prepare(const char *devinfo, int delim1, int delim2);
    int Tune(int freqKHz, int plpId);
    int TuneMultiplePLP(int freqKhz, vector<int> plpIds);
    int ListenPLP1(int plp1); //by default, we will always listen to PLP0, append additional PLP for listening

    int Stop();
    int Close();
    int Reset();
    int Uninit();

    /* phy callback method(s) */
    int atsc3_rx_callback_f(void*, uint64_t ullUser);

    /*
     * pcap methods
     */

    int atsc3_pcap_replay_open_file(const char *filename);
//    int atsc3_pcap_replay_open_file_from_assetManager(const char *filename, AAssetManager *mgr);
    int atsc3_pcap_thread_run();
    int atsc3_pcap_thread_stop();

    void LogMsg(const char *msg);
    void LogMsg(const std::string &msg);
    void LogMsgF(const char *fmt, ...);

    int pinFromRxCaptureThread();
    int pinFromRxProcessingThread();
    int pinFromRxStatusThread();


    int RxThread();
    Atsc3JniEnv* Atsc3_Jni_Capture_Thread_Env = NULL;
    Atsc3JniEnv* Atsc3_Jni_Processing_Thread_Env = NULL;
    Atsc3JniEnv* Atsc3_Jni_Status_Thread_Env = NULL;

private:
    bool mbInit;

    std::thread mhRxThread;

    bool mbLoop, mbRun;

    // statistics
    uint32_t s_ulLastTickPrint;
    uint64_t s_ullTotalBytes = 0;
    uint64_t s_ullTotalPkts;
    unsigned s_uTotalLmts = 0;
    std::map<std::string, unsigned> s_mapIpPort;
    int s_nPrevLmtVer = -1;
    uint32_t s_ulL1SecBase;

    //pcap replay context and locals
    int PcapProducerThreadParserRun();
    int PcapConsumerThreadRun();
    int PcapLocalCleanup();

    AAsset*                         pcap_replay_asset_ref_ptr = NULL;
    char*                           pcap_replay_filename = NULL;
    bool                            pcapThreadShouldRun;

    std::thread                     pcapProducerThreadPtr;
    Atsc3JniEnv*                    atsc3_jni_pcap_producer_thread_env = NULL;
    bool                            pcapProducerShutdown = true;

    std::thread                     pcapConsumerThreadPtr;
    Atsc3JniEnv*                    atsc3_jni_pcap_consumer_thread_env = NULL;
    bool                            pcapConsumerShutdown = true;

    atsc3_pcap_replay_context_t*    atsc3_pcap_replay_context = NULL;
    queue<block_t*>                 pcap_replay_buffer_queue;
    mutex                           pcap_replay_buffer_queue_mutex;
    condition_variable              pcap_replay_condition;

    //alc service monitoring
    vector<int>                     atsc3_slt_alc_additional_services_monitored;

public:
    jobject     global_pcap_asset_manager_ref = NULL;

public:
    // jni stuff
    JavaVM* mJavaVM = nullptr;    // Java VM
    JNIEnv* mJniEnv = nullptr;    // Jni Environment
    jclass mClsDrvIntf = nullptr; // java At3DrvInterface class

    bool JReady() {
        return mJavaVM && mJniEnv && mClsDrvIntf ? true : false;
    }

    jmethodID mOnLogMsgId = nullptr;  // java class method id

    jmethodID atsc3_rf_phy_status_callback_ID = nullptr;
    jmethodID atsc3_update_rf_bw_stats_ID = nullptr;

    //todo: refactor this out - ala https://gist.github.com/qiao-tw/6e43fb2311ee3c31752e11a4415deeb1

    jclass      jni_java_util_ArrayList = nullptr;
    jmethodID   jni_java_util_ArrayList_cctor = nullptr;
    jmethodID   jni_java_util_ArrayList_add = nullptr;


private:
    std::thread atsc3_rxStatusThread;
    void RxStatusThread();
    bool rxStatusThreadShouldRun;
};

#define NDK_PCAP_VIRTUAL_PHY_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);


#endif //LIBATSC3_PCAPDEMUXEDVIRTUALPHY_H
