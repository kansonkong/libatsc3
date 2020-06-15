//
// Created by Jason Justman on 2019-09-27.
//

#ifndef LIBATSC3_ATSC3NDKCLIENT_H
#define LIBATSC3_ATSC3NDKCLIENT_H

#include <string.h>
#include <jni.h>
#include <thread>
#include <map>
#include <queue>
#include <mutex>
#include <semaphore.h>
using namespace std;

#include <android/log.h>
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>


//#include "at3drv_api.h"

#define DEBUG 1

#include "android/log.h"
#define MODULE_NAME "intf"

#include <atsc3_utils.h>
#include <atsc3_pcap_type.h>

/*
 * : public libatsc3_Iphy_mockable
 */


//--------------------------------------------------------------------------
//jjustman-2019-10-19: i don't think the mJvm->detatch is correct in the context of the CJniEnv destructor, see:
//https://developer.android.com/training/articles/perf-jni#threads_1
class CJniEnv
{
private:
    JNIEnv *mJniEnv = nullptr;
    JavaVM *mJvm = nullptr;
    bool mAttached = false;
public:
    CJniEnv(JavaVM *jvm): mJvm(jvm) {
        int r = jvm->GetEnv((void **)&mJniEnv, JNI_VERSION_1_4);
        if (r == JNI_OK) return;
        r = jvm->AttachCurrentThread(&mJniEnv, 0);
        if (r == 0) mAttached = true;
    }
    virtual ~CJniEnv() {
        if (mJniEnv && mAttached)
            mJvm->DetachCurrentThread();
    }
    operator bool() {
        return mJniEnv != nullptr;
    }
    JNIEnv *Get() {
        return mJniEnv;
    }
};

class Iatsc3NdkClient {
    public:
        virtual int Init() = 0;
        virtual int Open(int fd, int bus, int addr) = 0;
        virtual int Tune(int freqKhz, int plp) = 0;
        virtual int Stop()  = 0;
        virtual int Close()  = 0;

        virtual ~Iatsc3NdkClient() {};

};

class atsc3NdkClient : public Iatsc3NdkClient
{
public:
    atsc3NdkClient(): mbInit(false),mbLoop(false), mbRun(false) {    }

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
    int atsc3_pcap_replay_open_file_from_assetManager(const char *filename, AAssetManager *mgr);
    int atsc3_pcap_thread_run();
    int atsc3_pcap_thread_stop();


    void LogMsg(const char *msg);
    void LogMsg(const std::string &msg);
    void LogMsgF(const char *fmt, ...);

    /** atsc3 service methods **/
    int atsc3_slt_selectService(int service_id);

    void atsc3_onMfuPacket(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected);
    void atsc3_onMfuPacketCorrupt(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);
    void atsc3_onMfuPacketCorruptMmthSampleHeader(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);
    void atsc3_onInitHEVC_NAL_Extracted(uint16_t packet_id, uint32_t mpu_sequence_number, uint8_t* buffer, uint32_t bufferLen);

    void atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
    void atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microsecond);
    void atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);

    int pinFromRxCaptureThread();
    int pinFromRxProcessingThread();
    int pinFromRxStatusThread();


    int RxThread();
    CJniEnv* Atsc3_Jni_Capture_Thread_Env = NULL;
    CJniEnv* Atsc3_Jni_Processing_Thread_Env = NULL;
    CJniEnv* Atsc3_Jni_Status_Thread_Env = NULL;

    void set_plp_settings(jint *a_plp_ids, jsize sa_plp_size);

    void atsc3_onExtractedSampleDuration(uint16_t packet_id, uint32_t mpu_sequence_number,
                                         uint32_t extracted_sample_duration_us);

    void atsc3_setVideoWidthHeightFromTrak(uint32_t width, uint32_t height);

    int atsc3_slt_alc_select_additional_service(int service_id);

    int atsc3_slt_alc_clear_additional_service_selections();

    vector<string>
    atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id(int service_id, const char* to_match_content_type);

    vector<string>
    atsc3_slt_alc_get_sls_route_s_tsid_fdt_file_content_locations_from_monitor_service_id(int service_id);

    void atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_jni(uint32_t tsi, uint32_t toi,
                                                                            char *content_location);

    void atsc3_lls_sls_alc_on_route_mpd_patched_jni(uint16_t service_id);

    void atsc3_onSlsTablePresent(const char *sls_payload_xml);

    void atsc3_onAlcObjectStatusMessage(const char *fmt, ...);


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
    CJniEnv*                        atsc3_jni_pcap_producer_thread_env = NULL;
    bool                            pcapProducerShutdown = true;

    std::thread                     pcapConsumerThreadPtr;
    CJniEnv*                        atsc3_jni_pcap_consumer_thread_env = NULL;
    bool                            pcapConsumerShutdown = true;


    atsc3_pcap_replay_context_t*    atsc3_pcap_replay_context = NULL;
    queue<block_t*>                 pcap_replay_buffer_queue;
    mutex                           pcap_replay_buffer_queue_mutex;
    condition_variable              pcap_replay_condition;

    //alc service monitoring
    vector<int>                     atsc3_slt_alc_additional_services_monitored;


public:
    jobject     global_pcap_asset_manager_ref = NULL;

private:
    //global env.Get()->NewGlobalRef(jobjectByteBuffer); for c alloc'd MFU's and NAL's
    std::vector<jobject> global_jobject_mfu_refs;
    std::vector<jobject> global_jobject_nal_refs;

public:
    void ResetStatstics() {
        s_ulLastTickPrint = 0;
        s_ullTotalBytes = s_ullTotalPkts = 0;
        s_uTotalLmts = 0;
        s_mapIpPort.clear();
        s_nPrevLmtVer = -1;
        s_ulL1SecBase = 0;
    }

public:
    // jni stuff
    JavaVM* mJavaVM = nullptr;    // Java VM
    JNIEnv* mJniEnv = nullptr;    // Jni Environment
    jclass mClsDrvIntf = nullptr; // java At3DrvInterface class

    bool JReady() {
        return mJavaVM && mJniEnv && mClsDrvIntf ? true : false;
    }

    jmethodID mOnLogMsgId = nullptr;  // java class method id

    jmethodID atsc3_onSlsTablePresent_ID = nullptr; //push LLS SLT table update
    jmethodID atsc3_onMfuPacketID = nullptr; //java method for pushing to a/v codec buffers
    jmethodID atsc3_onMfuPacketCorruptID = nullptr; //java method for pushing to a/v codec buffers
    jmethodID atsc3_onMfuPacketCorruptMmthSampleHeaderID = nullptr; //java method for pushing to a/v codec buffers
    jmethodID atsc3_onMfuSampleMissingID = nullptr;

    jmethodID mOnInitHEVC_NAL_Extracted = nullptr; //java method for pushing to a/v codec buffers

    jmethodID atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID = nullptr;  // java class method id
    jmethodID atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID = nullptr;  // java class method id
    jmethodID atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID = nullptr;  // java class method id

    jmethodID atsc3_rf_phy_status_callback_ID = nullptr;
    jmethodID atsc3_onExtractedSampleDurationID = nullptr;
    jmethodID atsc3_setVideoWidthHeightFromTrakID = nullptr;
    jmethodID atsc3_update_rf_bw_stats_ID = nullptr;

    jmethodID atsc3_lls_sls_alc_on_route_mpd_patched_ID = nullptr;
    jmethodID atsc3_on_alc_object_status_message_ID = nullptr;

    void atsc3_onMfuSampleMissing(uint16_t i, uint32_t i1, uint32_t i2);

    std::string get_android_temp_folder();

    //moving to "friend" scope
    void atsc3_update_rf_stats(   int32_t tuner_lock,    //1
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

private:
    std::thread atsc3_rxStatusThread;
    void RxStatusThread();
    bool rxStatusThreadShouldRun;


};




#endif //LIBATSC3_ATSC3NDKCLIENT_H
