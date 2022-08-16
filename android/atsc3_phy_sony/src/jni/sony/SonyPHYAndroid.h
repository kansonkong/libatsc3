//
// Created by Jason Justman on 5/24/22.
//


#include <string.h>
#include <jni.h>
#include <thread>
#include <mutex>
#include <semaphore.h>

#ifdef __ANDROID__
#include <android/log.h>
#include <libusb.h>
#endif

using namespace std;

#include <Atsc3LoggingUtils.h>
#include <Atsc3JniEnv.h>
#include <atsc3_utils.h>
#include <atsc3_sl_tlv_demod_type.h>
#include <atsc3_alp_parser.h>

#include <atsc3_core_service_player_bridge.h>

#ifndef ANDROID_ATSC_3_0_SAMPLE_APP_A344_AND_PHY_SUPPORT_SONYPHYANDROID_H
#define ANDROID_ATSC_3_0_SAMPLE_APP_A344_AND_PHY_SUPPORT_SONYPHYANDROID_H

#include <Atsc3NdkPHYSonyStaticJniLoader.h>
#include <brUser.h>
#include <IT9300.h>
#include <it930x-core.h>

#define TLV_CIRCULAR_BUFFER_SIZE                 8192000             // TLV circular buffer size, calculated for 2 seconds of user-space interruption at ~15Mbit/sec -> 1.875 * 2 -> 4 MB
#define TLV_CIRCULAR_BUFFER_MIN_PROCESS_SIZE    (16 * 1024 * 2)         //CircularBuffer pending data size threshold for TLV depacketization processing, pinned at 8KB to match SL4000 ALP buffer
#define TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE  (16 * 1024 * 2)    //CircularBuffer block read size for depacketization callback processing ~ 65KB

#include "CircularBuffer.h"

#include <queue>
#include <condition_variable>
#include <semaphore.h>

static void readFromUsbDemodEndpointRxTs();
//jjustman-2022-05-24 - local scoped types

class SonyPHYAndroid : public IAtsc3NdkPHYClient {

public:
    static mutex CS_global_mutex;

    static libusb_device_handle* Libusb_device_handle;

    static int SONY_USB_ENDPOINT_RX;
    static int SONY_USB_ENDPOINT_TX;
    static int SONY_USB_ENDPOINT_RX_TS;
	static long ITE_93x_OPEN_CLEANUP_FROM_ERROR_LAST_REBOOT_TIMESTAMP;

	//jjustman-2022-05-24 - TODO: add libusb_device_handle for Endeavour ctx instance
    //static Endeavour            Endeavour_s; //FIXME: type aliasing against static!

    IT930x_Device*          dev = nullptr;
    Device_Context*         DC = nullptr;

    int internal_device_communication_test(Device_Context* DC);
    int internal_getEEPROMConfig(Device_Context* DC);
    int internal_it930x_initialize(Device_Context* DC, Byte br_idx);
    int internal_get_rx_id(Device_Context* DC);
    int internal_Demodulator_writeRegisters (Device_Context *DC, Processor processor, Byte option ,Dword registerAddress, Byte bufferLength, Byte* buffer, int br_idx, int ts_idx);
    int IT9300_setIgnoreFail (IN  Endeavour*      endeavour, IN  Byte            chip,IN  bool            bvalue) ;
    int internal_getFirmwareVersionFromFile(Device_Context *DC, Dword* version);
    int internal_CXD6801_writeRegister(Device_Context *DC, Processor processor, Byte option , Dword registerAddress, Byte bufferLength, Byte* buffer, int br_idx, int ts_idx);


    SonyPHYAndroid(JNIEnv* env, jobject jni_instance);

    virtual int  init()       override;
    virtual int  run()        override;
    virtual bool is_running() override;
    virtual int  stop()       override;
    virtual int  deinit()     override;

    virtual string get_sdk_version()  override;
    virtual string get_firmware_version() override;

    virtual int  download_bootloader_firmware(int fd, int device_type, string devicePath) override;
    virtual int  open(int fd, int device_type, string devicePath)   override;
    virtual int  tune(int freqKhz, int single_plp) override;
    virtual int  listen_plps(vector<uint8_t> plps) override;

    virtual ~SonyPHYAndroid();

	static volatile bool captureThreadShouldRun;
    static void RxDataCallback(unsigned char *data, long len);

    static void NotifyPlpSelectionChangeCallback(vector<uint8_t> plps, void* context);

    //jjustman-2020-08-23 - moving to public for now..
    uint64_t ip_completed_packets_parsed;
    uint64_t alp_total_bytes;
    uint64_t alp_total_LMTs_recv;

    mutex       plpConfigParams_mutex;
    MPLPData    plp_configuration_data = { };


    //jjustman-2021-06-07 - from demod_start
    string                  demodVersion;
    uint                    cpuStatus = 0;

    //jjustman-2021-03-02 - don't use this method...
    void dump_plp_list();



protected:
    void pinProducerThreadAsNeeded() override;
    void releasePinnedProducerThreadAsNeeded() override;
    Atsc3JniEnv* producerJniEnv = nullptr;

    void pinConsumerThreadAsNeeded() override;
    void releasePinnedConsumerThreadAsNeeded() override;
    Atsc3JniEnv* consumerJniEnv = nullptr;

    Atsc3JniEnv* alpConsumerJniEnv = nullptr;

    void pinStatusThreadAsNeeded() override;
    void releasePinnedStatusThreadAsNeeded() override;
    Atsc3JniEnv* statusJniEnv = nullptr;

    JNIEnv* env = nullptr;
    jobject jni_instance_globalRef = nullptr;

private:

    int libusb_fd = -1;


    static int Last_tune_freq;
    int                       demodStartStatus = 0;

    unsigned long long        llsPlpInfo = 0;
    unsigned long long        llsPlpMask = 0x1;
    int                       plpInfoVal = 0, plpllscount = 0;

    int                       last_l1bTimeInfoFlag = -1;
    uint64_t                  last_l1dTimeNs_value = 0;

    //uses      pinProducerThreadAsNeeded
    int         captureThread();
    std::thread captureThreadHandle;
//    static volatile bool captureThreadShouldRun;
    bool        captureThreadIsRunning = false;

    //uses      pinConsumerThreadAsNeeded
    int         processThread();
    std::thread processThreadHandle;
    bool        processThreadShouldRun = false;
    bool        processThreadIsRunning = false;
    int         processTLVFromCallbackInvocationCount = 0;

    //uses      pinConsumerThreadAsNeeded
    int         processAlpFromCircularBufferThread();
    std::thread processAlpThreadHandle;
    bool        processAlpThreadShouldRun = false;
    bool        processAlpThreadIsRunning = false;
    int         processAlpFromCallbackInvocationCount = 0;


    //uses      pinStatusThreadAsNeeded
    int         statusThread();
    std::thread statusThreadHandle;
    bool        statusThreadShouldRun = false;
    bool        statusThreadIsRunning = false;

    void        statusMetricsResetFromTuneChange();
    void        statusMetricsResetFromPLPListenChange();

    void        resetProcessThreadStatistics();

    //if this is our first loop after a Tune() command has completed, dump SL_DemodGetConfiguration
    bool        statusThreadFirstLoopAfterTuneComplete = false;

    //if this is our first loop after a Tune() command has completed AND...

    // we have RF_DemodLock, then get BSR_Diag
    bool        statusThreadFirstLoopAfterTuneComplete_HasBootstrapLock_for_BSR_Diag = false;

    // we have L1B_DemodLock, then get full L1B_Diag
    bool        statusThreadFirstLoopAfterTuneComplete_HasL1B_DemodLock_for_L1B_Diag = false;

    // we have L1D_DemodLock, then get full L1D diag data
    bool        statusThreadFirstLoopAfterTuneComplete_HasL1D_DemodLock_for_L1D_Diag = false;

    //hack

    static mutex                    CircularBufferMutex;
    static CircularBuffer           cb_tlv;

    queue<block_t*>                 tlv_buffer_queue_for_alp_extraction;
    mutex                           tlv_buffer_queue_for_alp_extraction_mutex;
    condition_variable              tlv_buffer_queue_for_alp_extraction_notification;


    //jjustman-2021-01-19 - used for when we are in a tuning operation and may have in-flight async RxDataCallbacks fired,
    //          if set to true, we should discard the TLV payload in RxDataCallback
    static atomic_bool cb_should_discard;

    void processTLVFromCallback();

    char tlv_processDataBufferForCallback[TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE];

    int32_t pre_parse_alp_packet_rollback_ptr_i_pos = -1;

    void                    allocate_atsc3_sl_tlv_block();

    block_t*                atsc3_sl_tlv_block = NULL;
    atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload = NULL;

    //jjustman-2021-06-07 # 11798: compute global/l1b/l1d/plpN SNR metrics
    double compute_snr(int snr_linear_scale);

};

#define _SONY_PHY_ANDROID_ERROR_NOTIFY_BRIDGE_INSTANCE(method, message, cmd_res) \
    if(atsc3_ndk_phy_bridge_get_instance()) { \
        atsc3_ndk_phy_bridge_get_instance()->atsc3_notify_phy_error("SONYPHYAndroid::%s - ERROR: %s, global_sl_res: %d, global_sl_i2c_res: %d, cmd res: %d", \
        method, message, global_sl_result_error_flag, global_sl_i2c_result_error_flag, cmd_res); \
    } \
    __LIBATSC3_TIMESTAMP_ERROR("SONYPHYAndroid::%s - ERROR: %s, global_sl_res: %d, global_sl_i2c_res: %d, cmd_res: %d", \
        method, message, global_sl_result_error_flag, global_sl_i2c_result_error_flag, cmd_res);


#define _SONY_PHY_ANDROID_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _SONY_PHY_ANDROID_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _SONY_PHY_ANDROID_INFO(...)    if(_SONY_PHY_ANDROID_INFO_ENABLED)      { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);  }
#define _SONY_PHY_ANDROID_DEBUG(...)   if(_SONY_PHY_ANDROID_DEBUG_ENABLED)     { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _SONY_PHY_ANDROID_TRACE(...)   if(_SONY_PHY_ANDROID_TRACE_ENABLED)     { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#endif //ANDROID_ATSC_3_0_SAMPLE_APP_A344_AND_PHY_SUPPORT_TOLKAPHYANDROID_H
