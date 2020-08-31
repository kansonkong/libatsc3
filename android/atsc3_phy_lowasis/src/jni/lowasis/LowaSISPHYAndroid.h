//
// Created by Jason Justman on 8/19/20.
//

#include <string.h>
#include <jni.h>
#include <thread>
#include <map>

#include <queue>
#include <mutex>
#include <semaphore.h>
using namespace std;

#include <Atsc3LoggingUtils.h>
#include <Atsc3JniEnv.h>
#include <atsc3_utils.h>
#include <atsc3_sl_tlv_demod_type.h>
#include <atsc3_alp_parser.h>

#include <atsc3_core_service_player_bridge.h>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_LOWASISPHYANDROID_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_LOWASISPHYANDROID_H

#include <Atsc3NdkPHYLowaSISStaticJniLoader.h>

#include <at3drv_api.h>

#include "atsc3_lowasis_phy_android_rxdata.h"

typedef void * (*THREADFUNCPTR)(void *);

class LowaSISPHYAndroid : public IAtsc3NdkPHYClient {

public:
    static mutex CS_global_muxtex;

    LowaSISPHYAndroid(JNIEnv* env, jobject jni_instance);

    virtual int  init()       override;
    virtual int  run()        override;
    virtual bool is_running() override;
    virtual int  stop()       override;
    virtual int  deinit()     override;

    virtual int  download_bootloader_firmware(int fd, string device_path) override;
    virtual int  open(int fd, string device_path)   override;
    virtual int  tune(int freqKhz, int single_plp) override;
    virtual int  listen_plps(vector<uint8_t> plps) override;

    virtual ~LowaSISPHYAndroid();

    static AT3RESULT RxCallbackStatic(S_RX_DATA *pData, uint64_t ullUser);
    AT3RESULT RxCallbackInstanceScoped(S_RX_DATA *pData);

    static void NotifyPlpSelectionChangeCallback(vector<uint8_t> plps, void* context);

    //jjustman-2020-08-23 - moving to public for now..
    uint64_t alp_completed_packets_parsed;
    uint64_t alp_total_bytes;
    uint64_t alp_total_LMTs_recv;

protected:
    void pinProducerThreadAsNeeded() override;
    void releasePinnedProducerThreadAsNeeded() override;
    Atsc3JniEnv* producerJniEnv = nullptr;

    void pinConsumerThreadAsNeeded() override;
    void releasePinnedConsumerThreadAsNeeded() override;
    Atsc3JniEnv* consumerJniEnv = nullptr;

    void pinStatusThreadAsNeeded() override;
    void releasePinnedStatusThreadAsNeeded() override;
    Atsc3JniEnv* statusJniEnv = nullptr;

    JNIEnv* env = nullptr;
    jobject jni_instance_globalRef = nullptr;

private:

    bool init_completed = false;
    bool is_tuned = false;


    //uses      pinProducerThreadAsNeeded
    int         captureThread();
    std::thread captureThreadHandle;
    bool        captureThreadShouldRun = false;
    bool        captureThreadIsRunning = false;

    //uses      pinConsumerThreadAsNeeded
    int         processThread();
    std::thread processThreadHandle;
    bool        processThreadShouldRun = false;
    bool        processThreadIsRunning = false;

    //uses      pinStatusThreadAsNeeded
    int         statusThread();
    std::thread statusThreadHandle;
    bool        statusThreadShouldRun = false;
    bool        statusThreadIsRunning = false;

    //internal buffering for AT3DRV_HandleRxData

    queue<atsc3_lowasis_phy_android_rxdata_t*>    lowasis_phy_rx_data_buffer_queue;
    mutex                                   lowasis_phy_rx_data_buffer_queue_mutex;
    condition_variable                      lowasis_phy_rx_data_buffer_condition;

    //LowaSIS api specific methods

    //jjustman-2020-08-24 - todo - refactor resetStatstics out?
    AT3_DEV_KEY mKey = 0;
    AT3_DEVICE mhDevice = 0;

    AT3_OPTION mAt3Opt = 0;

    uint32_t s_ulLastTickPrint;
    uint64_t s_ullTotalBytes = 0;
    uint64_t s_ullTotalPkts;
    unsigned s_uTotalLmts = 0;
    std::map<std::string, unsigned> s_mapIpPort;
    int s_nPrevLmtVer = -1;
    uint32_t s_ulL1SecBase;

    void resetStatstics();


};

#define _LOWASIS_PHY_ANDROID_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _LOWASIS_PHY_ANDROID_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _LOWASIS_PHY_ANDROID_INFO(...)    	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _LOWASIS_PHY_ANDROID_DEBUG(...)     __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define _LOWASIS_PHY_ANDROID_TRACE(...)     __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);


#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_LOWASISPHYANDROID_H
