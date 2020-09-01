//
// Created by Jason Justman on 8/19/20.
//

#include <string.h>
#include <jni.h>
#include <Atsc3LoggingUtils.h>
#include <Atsc3JniEnv.h>
#include <atsc3_utils.h>
#include <atsc3_sl_tlv_demod_type.h>
#include <atsc3_alp_parser.h>

#include <mutex>
#include <semaphore.h>
#include <pthread.h>
#include <atsc3_core_service_player_bridge.h>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SAANKHYAPHYANDROID_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SAANKHYAPHYANDROID_H

#include <Atsc3NdkPHYSaankhyaStaticJniLoader.h>


#define IF_OFFSET            (0.003453)          // User can Update as needed
#define CB_SIZE           (16*1024*100)   // Global  circular buffer size
#define BUFFER_SIZE       (16*1024*10)

#define __TLV_BUFFER_SIZE      1638

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

#include <cyusb.h>

#include <pthread.h>
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
    SL_Atsc3p0Perf_Diag_t perfDiag;
    SL_Atsc3p0Bsr_Diag_t  bsrDiag;
    SL_Atsc3p0L1B_Diag_t  l1bDiag;
    SL_Atsc3p0L1D_Diag_t  l1dDiag;

    void dump_plp_list();

    mutex SL_I2C_command_mutex;

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

    int slUnit;
    int tUnit;

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

    unsigned long int         cThread;
    unsigned long int         pThread;
    unsigned long int         dThread;
    unsigned long int         sThread;

    pthread_t   cThreadID;
    pthread_t   pThreadID;
    pthread_t   dThreadID;
    pthread_t   sThreadID;

    //hack
    static CircularBuffer cb;
    static mutex CircularBufferMutex;

    bool        captureThreadShouldRun = false;;
    bool        captureThreadIsRunning = false;

    bool        processThreadShouldRun = false;
    bool        processThreadIsRunning = false;

    bool        statusThreadShouldRun = false;
    bool        statusThreadIsRunning = false;

    //thread handling methods
    static void* CaptureThread(void* context);
    static void* ProcessThread(void* context);
    static void* TunerStatusThread(void* context); //TODO: jjustman-2019-11-30: merge with


    SL_ConfigResult_t configPlatformParams();

    void printToConsolePlfConfiguration(SL_PlatFormConfigParams_t cfgInfo);
    void printToConsoleDemodConfiguration(SL_DemodConfigInfo_t cfgInfo);

    void printToConsoleI2cError(SL_I2cResult_t err);
    void printToConsoleTunerError(SL_TunerResult_t err);
    void printToConsoleDemodError(SL_Result_t err);

    void handleCmdIfFailure(void);

    void resetProcessThreadStatistics();

    void processTLVFromCallback();
    char processDataCircularBufferForCallback[BUFFER_SIZE];

    block_t* atsc3_sl_tlv_block = NULL;
    mutex    atsc3_sl_tlv_block_mutex;
    void allocate_atsc3_sl_tlv_block();

    atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload = NULL;


};

#define _SAANKHYA_PHY_ANDROID_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _SAANKHYA_PHY_ANDROID_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _SAANKHYA_PHY_ANDROID_INFO(...)    	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _SAANKHYA_PHY_ANDROID_DEBUG(...)    __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define _SAANKHYA_PHY_ANDROID_TRACE(...)    __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);


#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SAANKHYAPHYANDROID_H
