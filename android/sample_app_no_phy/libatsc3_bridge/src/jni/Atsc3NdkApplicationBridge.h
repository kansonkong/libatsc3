//
// Created by Jason Justman on 2019-09-27.
//

#ifndef LIBATSC3_ATSC3NDKAPPLICATIONBRIDGE_H
#define LIBATSC3_ATSC3NDKAPPLICATIONBRIDGE_H

#include "Atsc3LoggingUtils.h"

#include <string.h>
#include <thread>
#include <map>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <list>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sys/types.h>

#define DEBUG 1

#include "Atsc3JniEnv.h"

// libatsc3 type imports here
#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>
#include <atsc3_lls_types.h>
#include "atsc3_alc_rx.h"
#include <atsc3_pcap_type.h>
#include <atsc3_monitor_events_alc.h>
#include <atsc3_route_package_utils.h>
#include <atsc3_core_service_player_bridge.h>


#include <application/IAtsc3NdkApplicationBridge.h>

#define MODULE_NAME "intf"

/*]
 * : public libatsc3_Iphy_mockable 
 */


//--------------------------------------------------------------------------
//jjustman-2019-10-19: i don't think the mJvm->detatch is correct in the context of the Atsc3JniEnv destructor, see:
//https://developer.android.com/training/articles/perf-jni#threads_1


class Atsc3NdkApplicationBridge : public IAtsc3NdkApplicationBridge
{
public:
    Atsc3NdkApplicationBridge(): mbInit(false), mbLoop(false), mbRun(false) {    }
    
    /* phy callback method(s)
    int atsc3_rx_callback_f(void*, uint64_t ullUser);
    **/
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
    void atsc3_lls_sls_alc_on_package_extract_completed_callback_jni(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload_t);

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

    void atsc3_sls_on_held_trigger_received_callback_jni(uint16_t service_id, const char *held_payload);

    //int RxThread();

    int pinFromRxCaptureThread();
    int pinFromRxProcessingThread();
    int pinFromRxStatusThread();


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

    jmethodID atsc3_lls_sls_alc_on_package_extract_completed_ID = nullptr;

    jclass packageExtractEnvelopeMetadataAndPayload_jclass_init_env = nullptr;
    jclass packageExtractEnvelopeMetadataAndPayload_jclass_global_ref = nullptr;

    jclass packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_init_env = nullptr;
    jclass packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref = nullptr;


    //todo: refactor this out - ala https://gist.github.com/qiao-tw/6e43fb2311ee3c31752e11a4415deeb1

    jclass      jni_java_util_ArrayList = nullptr;
    jmethodID   jni_java_util_ArrayList_cctor = nullptr;
    jmethodID   jni_java_util_ArrayList_add = nullptr;

    void atsc3_onMfuSampleMissing(uint16_t i, uint32_t i1, uint32_t i2);

    std::string get_android_temp_folder();

};

#define NDK_APPLICATION_BRIDGE_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define NDK_APPLICATION_BRIDGE_INFO(...)   	    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);



#endif //LIBATSC3_ATSC3NDKAPPLICATIONBRIDGE_H
