//
// Created by Jason Justman on 8/19/20.
//

#include <string.h>
#include <jni.h>
#include <thread>
#include <mutex>
#include <semaphore.h>
using namespace std;

#include <Atsc3LoggingUtils.h>
#include <Atsc3JniEnv.h>
#include <atsc3_utils.h>
#include <atsc3_sl_tlv_demod_type.h>
#include <atsc3_alp_parser.h>

#include <atsc3_core_service_player_bridge.h>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SAANKHYAPHYANDROID_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SAANKHYAPHYANDROID_H

#include <Atsc3NdkPHYSaankhyaStaticJniLoader.h>

#define IF_OFFSET            (0.003453)   // User can Update as needed

//TLV circular buffer sizing - use
//  unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);
//  and CircularBufferMutex_local.unlock();
//for concurrency protection
//
//#define TLV_CIRCULAR_BUFFER_SIZE                 4096000            // TLV circular buffer size, calculated for 2 seconds of user-space interruption at ~15Mbit/sec -> 1.875 * 2 -> 4 MB
//#define TLV_CIRCULAR_BUFFER_MIN_PROCESS_SIZE    (8 * 1024)          //CircularBuffer pending data size threshold for TLV depacketization processing, pinned at 8KB to match SL4000 ALP buffer
//#define TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE  (16 * 1024 * 4)     //CircularBuffer block read size for depacketization callback processing ~ 65KB

//jjustman-2020-11-06 rolling back to prev values-ish

#define TLV_CIRCULAR_BUFFER_SIZE                 4096000            // TLV circular buffer size, calculated for 2 seconds of user-space interruption at ~15Mbit/sec -> 1.875 * 2 -> 4 MB
#define TLV_CIRCULAR_BUFFER_MIN_PROCESS_SIZE    (16 * 1024 * 2)         //CircularBuffer pending data size threshold for TLV depacketization processing, pinned at 8KB to match SL4000 ALP buffer
#define TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE  (16 * 1024 * 2)    //CircularBuffer block read size for depacketization callback processing ~ 65KB

#include "CircularBuffer.h"
#include <sl_utils.h>
#include <sl_config.h>
#include <sl_demod.h>
#include <sl_tuner.h>
#include <sl_ts.h>
#include <sl_log.h>
#include <sl_i2c.h>
#include <sl_gpio.h>
#include <sl_demod.h>
#include <sl_utils.h>

typedef void * (*THREADFUNCPTR)(void *);

class SaankhyaPHYAndroid : public IAtsc3NdkPHYClient {

public:
    static mutex CS_global_mutex;

    SaankhyaPHYAndroid(JNIEnv* env, jobject jni_instance);

    virtual int  init()       override;
    virtual int  run()        override;
    virtual bool is_running() override;
    virtual int  stop()       override;
    virtual int  deinit()     override;

    virtual int  download_bootloader_firmware(int fd, string devicePath) override;
    virtual int  open(int fd, string devicePath)   override;
    virtual int  tune(int freqKhz, int single_plp) override;
    virtual int  listen_plps(vector<uint8_t> plps) override;

    virtual ~SaankhyaPHYAndroid();

    static void RxDataCallback(unsigned char *data, long len);

    int RxThread();

    static void NotifyPlpSelectionChangeCallback(vector<uint8_t> plps, void* context);

    //jjustman-2020-08-23 - moving to public for now..
    uint64_t alp_completed_packets_parsed;
    uint64_t alp_total_bytes;
    uint64_t alp_total_LMTs_recv;

    SL_PlpConfigParams_t plpInfo;
    SL_Atsc3p0Region_t   regionInfo;

    SL_Atsc3p0Perf_Diag_t perfDiag;
    SL_Atsc3p0Bsr_Diag_t  bsrDiag;
    SL_Atsc3p0L1B_Diag_t  l1bDiag;
    SL_Atsc3p0L1D_Diag_t  l1dDiag;

    void dump_plp_list();

    mutex SL_I2C_command_mutex;
    bool SL_I2C_last_command_extra_sleep;

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

    int slUnit = -1;
    int tUnit = -1;

    SL_PlatFormConfigParams_t getPlfConfig;
    SL_PlatFormConfigParams_t sPlfConfig;
    SL_CmdControlIf_t         cmdIf;
    SL_BbCapture_t            getbbValue = BB_CAPTURE_DISABLE;

    SL_LogResult_t            lres;
    SL_Result_t               slres;
    SL_TunerResult_t          tres;
    SL_ConfigResult_t         cres;
    SL_UtilsResult_t          utilsres;
    int                       demodStartStatus = 0;

    unsigned long long        llsPlpInfo = 0;
    unsigned long long        llsPlpMask = 0x1;
    int                       plpInfoVal = 0, plpllscount = 0;

    SL_DemodConfigInfo_t cfgInfo;

    SL_TunerConfig_t tunerCfg;
    SL_TunerConfig_t tunerGetCfg;
    SL_TunerDcOffSet_t tunerIQDcOffSet;
    SL_TunerSignalInfo_t tunerInfo;

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
    int         processTLVFromCallbackInvocationCount = 0;

    //uses      pinStatusThreadAsNeeded
    int         statusThread();
    std::thread statusThreadHandle;
    bool        statusThreadShouldRun = false;
    bool        statusThreadIsRunning = false;

    //hack
    static CircularBuffer cb;
    static mutex CircularBufferMutex;
    //jjustman-2021-01-19 - used for when we are in a tuning operation and may have in-flight async RxDataCallbacks fired,
    //          if set to true, we should discard the TLV payload in RxDataCallback
    static atomic_bool cb_should_discard;

    //thread handling methods

    SL_ConfigResult_t configPlatformParams();

    void printToConsolePlfConfiguration(SL_PlatFormConfigParams_t cfgInfo);
    void printToConsoleDemodConfiguration(SL_DemodConfigInfo_t cfgInfo);

    void printToConsoleI2cError(SL_I2cResult_t err);
    void printToConsoleTunerError(SL_TunerResult_t err);
    void printToConsoleDemodError(SL_Result_t err);

    void handleCmdIfFailure(void);

    void resetProcessThreadStatistics();

    void processTLVFromCallback();
    char processDataCircularBufferForCallback[TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE];

    block_t* atsc3_sl_tlv_block = NULL;
    mutex    atsc3_sl_tlv_block_mutex;
    void allocate_atsc3_sl_tlv_block();

    atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload = NULL;

    //jjustman-2021-02-04 - global error flag if i2c txn fails, usually due to demod crash
    static SL_Result_t      global_sl_result_error_flag;
    static SL_I2cResult_t   global_sl_i2c_result_error_flag;
};

#define _SAANKHYA_PHY_ANDROID_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _SAANKHYA_PHY_ANDROID_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _SAANKHYA_PHY_ANDROID_INFO(...)    	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _SAANKHYA_PHY_ANDROID_DEBUG(...)    __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define _SAANKHYA_PHY_ANDROID_TRACE(...)    __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);


#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SAANKHYAPHYANDROID_H
