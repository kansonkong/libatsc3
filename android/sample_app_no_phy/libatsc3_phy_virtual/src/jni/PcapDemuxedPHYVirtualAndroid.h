//
// Created by Jason Justman on 2019-09-27.
//

#ifndef LIBATSC3_ATSC3NDKPHYBRIDGE_DEMUXEDPCAPVIRTUALPHY_H
#define LIBATSC3_ATSC3NDKPHYBRIDGE_DEMUXEDPCAPVIRTUALPHY_H

#include <Atsc3LoggingUtils.h>
#include <Atsc3JniEnv.h>

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

#define DEBUG 1

#define MODULE_NAME "intf"

// libatsc3 type imports here
#include <atsc3_utils.h>
#include <atsc3_pcap_type.h>
#include <atsc3_route_package_utils.h>
#include <phy/virtual/PcapDemuxedVirtualPHY.h>

class PcapDemuxedVirtualPHYAndroid : public PcapDemuxedVirtualPHY
{
public:
    //PcapDemuxedVirtualPHYAndroid();

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

    /*
     * pcap methods
     */

    int atsc3_pcap_replay_open_file(const char *filename);
    int atsc3_pcap_replay_open_file_from_assetManager(const char *filename, AAssetManager *mgr);
    int atsc3_pcap_thread_run();
    int atsc3_pcap_thread_stop();

//
//    void LogMsg(const char *msg);
//    void LogMsg(const std::string &msg);
//    void LogMsgF(const char *fmt, ...);

    int pinFromRxCaptureThread();
    int pinFromRxProcessingThread();
    int pinFromRxStatusThread();


    int RxThread();
    Atsc3JniEnv* Atsc3_Jni_Capture_Thread_Env = NULL;
    Atsc3JniEnv* Atsc3_Jni_Processing_Thread_Env = NULL;
    Atsc3JniEnv* Atsc3_Jni_Status_Thread_Env = NULL;

protected:



    //pcap replay context and locals
    int PcapProducerThreadParserRun();
    int PcapConsumerThreadRun();
    int PcapLocalCleanup();

    AAsset*                         pcap_replay_asset_ref_ptr = NULL;

    Atsc3JniEnv*                    atsc3_jni_pcap_producer_thread_env = NULL;
    bool                            pcapProducerShutdown = true;

    Atsc3JniEnv*                    atsc3_jni_pcap_consumer_thread_env = NULL;
    bool                            pcapConsumerShutdown = true;

public:
    jobject                         global_pcap_asset_manager_ref = NULL;

    // jni stuff
    JavaVM* mJavaVM = nullptr;    // Java VM
    JNIEnv* mJniEnv = nullptr;    // Jni Environment
    jclass mClsDrvIntf = nullptr; // java At3DrvInterface class

    bool JReady() {
        return mJavaVM && mJniEnv && mClsDrvIntf ? true : false;
    }

    jmethodID mOnLogMsgId = nullptr;  // java class method id

    //todo: refactor this out - ala https://gist.github.com/qiao-tw/6e43fb2311ee3c31752e11a4415deeb1

    jclass      jni_java_util_ArrayList = nullptr;
    jmethodID   jni_java_util_ArrayList_cctor = nullptr;
    jmethodID   jni_java_util_ArrayList_add = nullptr;

};


#define NDK_PCAP_DEMUXED_VIRTUAL_PHY_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define NDK_PCAP_DEMUXED_VIRTUAL_PHY_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO(...)   	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define NDK_PCAP_DEMUXED_VIRTUAL_PHY_DEBUG(...)   	__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);



#endif //LIBATSC3_ATSC3NDKPHYBRIDGE_DEMUXEDPCAPVIRTUALPHY_H
