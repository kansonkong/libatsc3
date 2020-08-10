#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <atsc3_lls_types.h>
#include <atsc3_phy_mmt_player_bridge.h>
#include <atsc3_pcap_type.h>
#include <atsc3_monitor_events_alc.h>

#include "Atsc3NdkApplicationBridge.h"


#define ASSERT(cond,s) do { \
        if (!(cond)) { eprintf("%s: !! %s assert fail, line %d\n", __func__, s, __LINE__); \
            return -1; } \
        } while(0)

#define CHK_AR(ar,s) do { \
		if (ar) { eprintf("%s: !! %s, err %d, line %d\n", __func__, s, ar, __LINE__); \
			return -1; } \
		} while(0)
#define SHOW_AR(ar,s) do { \
		if (ar) { printf("%s: !! %s, err %d, line %d\n", __func__, s, ar, __LINE__); } \
		} while(0)

//
//using namespace std;
//vector<string> Split(const char *str, char delimiter = ' ') {
//    vector<string> vs;
//    if (!str) return vs;
//    do {
//        const char *begin = str;
//        while(*str != delimiter && *str)
//            str++;
//        vs.push_back(string(begin, str));
//    } while (0 != *str++);
//    return vs;
//}

Atsc3NdkApplicationBridge apiAppBridge;



void Atsc3NdkApplicationBridge::atsc3_onMfuPacket(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_rebuilt)
{
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //printf("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = Atsc3_Jni_Processing_Thread_Env->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_rebuilt);
    //printf("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}


void Atsc3NdkApplicationBridge::atsc3_onMfuPacketCorrupt(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt)
{
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //printf("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = Atsc3_Jni_Processing_Thread_Env->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketCorruptID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    //printf("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}


void Atsc3NdkApplicationBridge::atsc3_onMfuPacketCorruptMmthSampleHeader(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt)
{
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //printf("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = Atsc3_Jni_Processing_Thread_Env->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketCorruptMmthSampleHeaderID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    //printf("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}

//push extracted HEVC nal's to MediaCodec for init
void Atsc3NdkApplicationBridge::atsc3_onInitHEVC_NAL_Extracted(uint16_t packet_id, uint32_t mpu_sequence_number, uint8_t* buffer, uint32_t bufferLen) {
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__

    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_nal_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(mClsDrvIntf, mOnInitHEVC_NAL_Extracted, is_video, mpu_sequence_number, jobjectGlobalByteBuffer, bufferLen);
    //printf("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    //env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectByteBuffer = Atsc3_Jni_Processing_Thread_Env->Get()->NewDirectByteBuffer(buffer, bufferLen);
    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, mOnInitHEVC_NAL_Extracted, packet_id, mpu_sequence_number, jobjectByteBuffer, bufferLen);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(jobjectByteBuffer);
#endif
}


/**
 *  void atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
    void atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microsecond);
    void atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
 * @param msg
 */


//todo: jjustman-2019-10-20: fixme env should be thread-bound already...
void Atsc3NdkApplicationBridge::atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID, video_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
    //printf("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   //env.Get()->DeleteLocalRef(jobjectLocalByteBuffer);
}


void Atsc3NdkApplicationBridge::atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID, audio_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
    //printf("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    //env.Get()->DeleteLocalRef(jobjectLocalByteBuffer);
}



void Atsc3NdkApplicationBridge::atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    int r =  Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID, stpp_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
    //printf("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    //env.Get()->DeleteLocalRef(jobjectLocalByteBuffer);
}


void Atsc3NdkApplicationBridge::atsc3_onMfuSampleMissing(uint16_t pcaket_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuSampleMissingID, pcaket_id, mpu_sequence_number, sample_number);

}


void Atsc3NdkApplicationBridge::LogMsg(const char *msg)
{
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady() || !mOnLogMsgId)
        return;
    Atsc3JniEnv env(mJavaVM);
    if (!env) {
        eprintf("!! err on get jni env\n");
        return;
    }
    jstring js = env.Get()->NewStringUTF(msg);
    int r = env.Get()->CallIntMethod(mClsDrvIntf, mOnLogMsgId, js);
    env.Get()->DeleteLocalRef(js);
}

void Atsc3NdkApplicationBridge::LogMsg(const std::string &str)
{
    LogMsg(str.c_str());
}

void Atsc3NdkApplicationBridge::LogMsgF(const char *fmt, ...)
{
    va_list v;
    char msg[1024];
    va_start(v, fmt);
    vsnprintf(msg, sizeof(msg)-1, fmt, v);
    msg[sizeof(msg)-1] = 0;
    va_end(v);
    LogMsg(msg);
}

//alcCompleteObjectMsg
void Atsc3NdkApplicationBridge::atsc3_onAlcObjectStatusMessage(const char *fmt, ...)
{
    va_list v;
    char msg[1024];
    va_start(v, fmt);
    vsnprintf(msg, sizeof(msg)-1, fmt, v);
    msg[sizeof(msg)-1] = 0;
    va_end(v);


    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    jstring js = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(msg);

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_on_alc_object_status_message_ID, js);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(js);

}


int Atsc3NdkApplicationBridge::pinFromRxCaptureThread() {
    printf("Atsc3NdkPHYBridge::Atsc3_Jni_Processing_Thread_Env: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Processing_Thread_Env = new Atsc3JniEnv(mJavaVM);
    return 0;
};

int Atsc3NdkApplicationBridge::pinFromRxProcessingThread() {
    printf("Atsc3NdkPHYBridge::pinFromRxProcessingThread: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Processing_Thread_Env = new Atsc3JniEnv(mJavaVM);
    return 0;
}


int Atsc3NdkApplicationBridge::pinFromRxStatusThread() {
    printf("Atsc3NdkPHYBridge::pinFromRxStatusThread: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Status_Thread_Env = new Atsc3JniEnv(mJavaVM);
    return 0;
}

int Atsc3NdkApplicationBridge::atsc3_slt_selectService(int service_id) {
    int ret = -1;
    atsc3_lls_slt_service_t* atsc3_lls_slt_service = atsc3_phy_mmt_player_bridge_set_single_monitor_a331_service_id(service_id);
    if(atsc3_lls_slt_service && atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count) {
        ret = atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol;
    }

    return ret;
}



void Atsc3NdkApplicationBridge::atsc3_onExtractedSampleDuration(uint16_t packet_id, uint32_t mpu_sequence_number,
                                                                uint32_t extracted_sample_duration_us) {

    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady() || !mOnLogMsgId)
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }
    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onExtractedSampleDurationID,
                                                                  packet_id,
                                                                  mpu_sequence_number,
                                                                  extracted_sample_duration_us);
}



void Atsc3NdkApplicationBridge::atsc3_setVideoWidthHeightFromTrak(uint32_t width, uint32_t height) {

    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady() || !mOnLogMsgId)
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }
    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_setVideoWidthHeightFromTrakID, width, height);

}


void Atsc3NdkApplicationBridge::set_plp_settings(jint *a_plp_ids, jsize a_plp_size) {

    uint8_t* u_plp_ids = (uint8_t*)calloc(a_plp_size, sizeof(uint8_t));
    for(int i=0; i < a_plp_size; i++) {
        u_plp_ids[i] = (uint8_t)a_plp_ids[i];
    }

    //AT3DRV_FE_SetPLP
   // AT3DRV_FE_SetPLP(mhDevice, u_plp_ids, a_plp_size);

}

std::string Atsc3NdkApplicationBridge::get_android_temp_folder() {
    Atsc3JniEnv env(mJavaVM);

    jclass clazz = env.Get()->FindClass("org/ngbp/libatsc3/sampleapp/Atsc3NdkPHYBridge");

    jmethodID getCacheDir = env.Get()->GetMethodID( clazz, "getCacheDir", "()Ljava/io/File;" );
    jobject cache_dir = env.Get()->CallObjectMethod(mClsDrvIntf, getCacheDir );

    jclass fileClass = env.Get()->FindClass( "java/io/File" );
    jmethodID getPath = env.Get()->GetMethodID( fileClass, "getPath", "()Ljava/lang/String;" );
    jstring path_string = (jstring)env.Get()->CallObjectMethod( cache_dir, getPath );

    const char *path_chars = env.Get()->GetStringUTFChars( path_string, NULL );
    std::string temp_folder( path_chars );

    env.Get()->ReleaseStringUTFChars( path_string, path_chars );
    //app->activity->vm->DetachCurrentThread();
    return temp_folder;
}

//return -1 on service_id not found
//return -2 on duplicate additional service_id request
int Atsc3NdkApplicationBridge::atsc3_slt_alc_select_additional_service(int service_id) {
    //keep track of internally here which "additional service_id's" we have on monitor;

    bool is_monitoring_duplicate = false;
    for(int i=0; i < atsc3_slt_alc_additional_services_monitored.size() && !is_monitoring_duplicate; i++) {
        if(atsc3_slt_alc_additional_services_monitored.at(i) == service_id) {
            //duplicate request
            is_monitoring_duplicate = true;
            continue;
        }
    }

    if(is_monitoring_duplicate) {
        return -2;
    }
    atsc3_lls_slt_service_t* atsc3_lls_slt_service = atsc3_phy_mmt_player_bridge_add_monitor_a331_service_id(service_id);
    if(!atsc3_lls_slt_service) {
        return -1;
    }

    atsc3_slt_alc_additional_services_monitored.push_back(service_id);

    return 0;
}

//TODO: jjustman-2019-11-07 - add mutex here around additional_services_monitored collection
int Atsc3NdkApplicationBridge::atsc3_slt_alc_clear_additional_service_selections() {

    for(int i=0; i < atsc3_slt_alc_additional_services_monitored.size(); i++) {
        int to_remove_monitor_service_id = atsc3_slt_alc_additional_services_monitored.at(i);
        atsc3_phy_mmt_player_bridge_remove_monitor_a331_service_id(to_remove_monitor_service_id);
    }

    atsc3_slt_alc_additional_services_monitored.clear();

    return 0;
}

//use 2nd param to_match_content_type to filter down to MPD via const char* to_match_content_type
//application/dash+xml

/*
 * jjustman-2019-11-08 - note mbms_envelope might not have the proper content_type set, so check
 */
vector<string>
Atsc3NdkApplicationBridge::atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id(int service_id, const char* to_match_content_type) {

    vector<string> my_mbms_metadata_uri_values;
    atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = atsc3_slt_alc_get_sls_metadata_fragments_from_monitor_service_id(service_id);

    if(atsc3_sls_metadata_fragments && atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope && atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope->atsc3_mbms_metadata_item_v.count) {
        for(int i=0; i < atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope->atsc3_mbms_metadata_item_v.count; i++) {
            atsc3_mbms_metadata_item_t* atsc3_mbms_metadata_item = atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope->atsc3_mbms_metadata_item_v.data[i];
            if(atsc3_mbms_metadata_item->metadata_uri) {
                //if to_match_content_type is supplied, filter by match
                if(to_match_content_type && atsc3_mbms_metadata_item->content_type &&
                   strncasecmp(to_match_content_type, atsc3_mbms_metadata_item->content_type, strlen(to_match_content_type)) == 0 ) {
                    string my_metadata_route_service_temp_folder_name = atsc3_route_service_context_temp_folder_name(service_id) + atsc3_mbms_metadata_item->metadata_uri;
                    my_mbms_metadata_uri_values.push_back(my_metadata_route_service_temp_folder_name);
                }
            }
        }

    }

    //also walk thru

    if(atsc3_sls_metadata_fragments && atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance && atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count) {
        for(int i=0; i < atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count; i++) {
            atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i];
            if(atsc3_mime_multipart_related_payload->sanitizied_content_location) {
                //if to_match_content_type is supplied, filter by match
                if(to_match_content_type && atsc3_mime_multipart_related_payload->content_type &&
                   strncasecmp(to_match_content_type, atsc3_mime_multipart_related_payload->content_type, strlen(to_match_content_type)) == 0 ) {
                    string my_metadata_route_service_temp_folder_name = atsc3_route_service_context_temp_folder_name(service_id) + atsc3_mime_multipart_related_payload->sanitizied_content_location;
                    my_mbms_metadata_uri_values.push_back(my_metadata_route_service_temp_folder_name);
                }
            }
        }

    }

    return my_mbms_metadata_uri_values;
}


vector<string> Atsc3NdkApplicationBridge::atsc3_slt_alc_get_sls_route_s_tsid_fdt_file_content_locations_from_monitor_service_id(int service_id) {
    vector<string> my_fdt_file_content_location_values;
    atsc3_route_s_tsid_t* atsc3_route_s_tsid = atsc3_slt_alc_get_sls_route_s_tsid_from_monitor_service_id(service_id);

    if(atsc3_route_s_tsid) {
        for(int i=0; i < atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.count; i++) {
            atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_RS = atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.data[i];

            for(int j=0; j < atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.count; j++) {
                atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS = atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.data[j];

                if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow) {
                    atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance;

                    if(atsc3_fdt_instance) {
                        for(int k=0; k < atsc3_fdt_instance->atsc3_fdt_file_v.count; k++) {
                            atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[k];
                            if(atsc3_fdt_file && atsc3_fdt_file->content_location ) {

                                string my_fdt_file_content_location_local_context_path = atsc3_route_service_context_temp_folder_name(service_id) + atsc3_fdt_file->content_location;
                                my_fdt_file_content_location_values.push_back(my_fdt_file_content_location_local_context_path);
                                //TODO: jjustman-2019-11-07 - re-factor to use atsc3_fdt_file_t struct
                            }
                        }
                    }
                }
            }
        }
    }

    return my_fdt_file_content_location_values;
}

void Atsc3NdkApplicationBridge::atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_jni(uint32_t tsi, uint32_t toi, char *content_location) {
    //jjustman-2020-01-07: add in alc flow debugging
    printf("atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_jni: tsi: %d, toi: %d, content_location: %s", tsi, toi, content_location);

    //len: nnn, lost DU: nnn
    atsc3_onAlcObjectStatusMessage("C: tsi: %d, toi: %d, content_location: %s", tsi, toi, content_location);
}

void Atsc3NdkApplicationBridge::atsc3_lls_sls_alc_on_route_mpd_patched_jni(uint16_t service_id) {
    if (!JReady() || !atsc3_lls_sls_alc_on_route_mpd_patched_ID)
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }
    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_lls_sls_alc_on_route_mpd_patched_ID, service_id);

}

// https://stackoverflow.com/questions/6343459/get-strings-used-in-java-from-jni
void Atsc3NdkApplicationBridge::atsc3_lls_sls_alc_on_package_extract_completed_callback_jni(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload) {
    if (!JReady() || !atsc3_lls_sls_alc_on_package_extract_completed_ID)
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err on get jni env: Atsc3_Jni_Processing_Thread_Env\n");
        return;
    }

    if(!atsc3_route_package_extracted_envelope_metadata_and_payload) {
        eprintf("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err atsc3_route_package_extracted_envelope_metadata_and_payload is NULL\n");
        return;
    }

    if(!atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml || !atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml->p_buffer) {
        eprintf("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml (or p_buffer) is NULL\n");
        return;
    }

    if(!atsc3_route_package_extracted_envelope_metadata_and_payload->package_name) {
    	eprintf("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err atsc3_route_package_extracted_envelope_metadata_and_payload->package_name is NULL\n");
        return;
    }

    std::list<jstring> to_clean_jstrings;
    std::list<jobject> to_clean_jobject;

    //org.ngbp.libatsc3.middleware.android.a331.PackageExtractEnvelopeMetadataAndPayload
    jclass jcls = apiAppBridge.packageExtractEnvelopeMetadataAndPayload_jclass_global_ref;
    jobject jobj = Atsc3_Jni_Processing_Thread_Env->Get()->AllocObject(jcls);

    if(!jobj) {
        eprintf("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err unable to allocate packageExtractEnvelopeMetadataAndPayload_jclass_global_ref instance jobj!");
        return;
    }


    jfieldID packageName_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "packageName", "Ljava/lang/String;");
    jstring packageName_payload = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_route_package_extracted_envelope_metadata_and_payload->package_name);
    Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj, packageName_valId, packageName_payload);
    to_clean_jstrings.push_back(packageName_payload);

    jfieldID tsi_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "tsi", "I");
    Atsc3_Jni_Processing_Thread_Env->Get()->SetIntField(jobj, tsi_valId, atsc3_route_package_extracted_envelope_metadata_and_payload->tsi);

    jfieldID toi_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "toi", "I");
    Atsc3_Jni_Processing_Thread_Env->Get()->SetIntField(jobj, toi_valId, atsc3_route_package_extracted_envelope_metadata_and_payload->toi);

    jfieldID appContextIdList_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "appContextIdList", "Ljava/lang/String;");
    jstring appContextIdList_payload = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_route_package_extracted_envelope_metadata_and_payload->app_context_id_list);
    Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj, appContextIdList_valId, appContextIdList_payload);
    to_clean_jstrings.push_back(appContextIdList_payload);


    jfieldID packageExtractPath_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "packageExtractPath", "Ljava/lang/String;");
    jstring packageExtractPath_payload = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_route_package_extracted_envelope_metadata_and_payload->package_extract_path);
    Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj, packageExtractPath_valId, packageExtractPath_payload);
    to_clean_jstrings.push_back(packageExtractPath_payload);


    jfieldID mbmsEnvelopeRawXml_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "mbmsEnvelopeRawXml", "Ljava/lang/String;");
    jstring mbmsEnvelopeRawXml_payload = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF((char*)atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml->p_buffer);
    Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj, mbmsEnvelopeRawXml_valId, mbmsEnvelopeRawXml_payload);
    to_clean_jstrings.push_back(mbmsEnvelopeRawXml_payload);

    if(atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.count > 0) {

        jobject multipartRelatedPayloadList_jobject = Atsc3_Jni_Processing_Thread_Env->Get()->NewObject(apiAppBridge.jni_java_util_ArrayList, apiAppBridge.jni_java_util_ArrayList_cctor, atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.count);

        for(int i=0; i < atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.count; i++) {
            atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.data[i];
            jobject jobj_multipart_related_payload_jobject = Atsc3_Jni_Processing_Thread_Env->Get()->AllocObject(apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref);

            to_clean_jobject.push_back(jobj_multipart_related_payload_jobject);

            jfieldID contentLocation_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "contentLocation", "Ljava/lang/String;");
            jstring contentLocation_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->sanitizied_content_location);
            Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, contentLocation_valId, contentLocation_jstring);
            to_clean_jstrings.push_back(contentLocation_jstring);

            if(atsc3_mime_multipart_related_payload->content_type) {
                jfieldID contentType_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "contentType", "Ljava/lang/String;");
                jstring contentType_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->content_type);
                Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, contentType_valId, contentType_jstring);
                to_clean_jstrings.push_back(contentType_jstring);
            }

            if(atsc3_mime_multipart_related_payload->valid_from_string) {
                jfieldID validFrom_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "validFrom", "Ljava/lang/String;");
                jstring validFrom_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->valid_from_string);
                Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, validFrom_valId, validFrom_jstring);
                to_clean_jstrings.push_back(validFrom_jstring);
            }

            if(atsc3_mime_multipart_related_payload->valid_until_string) {
                jfieldID validUntil_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "validUntil", "Ljava/lang/String;");
                jstring validUntil_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->valid_until_string);
                Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, validUntil_valId, validUntil_jstring);
                to_clean_jstrings.push_back(validUntil_jstring);
            }

            //version

            jfieldID version_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "version", "I");
            Atsc3_Jni_Processing_Thread_Env->Get()->SetIntField(jobj_multipart_related_payload_jobject, version_valId, atsc3_mime_multipart_related_payload->version);


            if(atsc3_mime_multipart_related_payload->next_url_string) {
                jfieldID nextUrl_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "nextUrl", "Ljava/lang/String;");
                jstring nextUrl_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->next_url_string);
                Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, nextUrl_valId, nextUrl_jstring);
                to_clean_jstrings.push_back(nextUrl_jstring);
            }

            if(atsc3_mime_multipart_related_payload->avail_at_string) {
                jfieldID availAt_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "availAt", "Ljava/lang/String;");
                jstring availAt_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->avail_at_string);
                Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, availAt_valId, availAt_jstring);
                to_clean_jstrings.push_back(availAt_jstring);
            }

            //extractedSize
            jfieldID extractedSize_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "extractedSize", "I");
            Atsc3_Jni_Processing_Thread_Env->Get()->SetIntField(jobj_multipart_related_payload_jobject, extractedSize_valId, atsc3_mime_multipart_related_payload->extracted_size);

            Atsc3_Jni_Processing_Thread_Env->Get()->CallBooleanMethod(multipartRelatedPayloadList_jobject, apiAppBridge.jni_java_util_ArrayList_add, jobj_multipart_related_payload_jobject);
        }

        jfieldID multipartRelatedPayloadList_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "multipartRelatedPayloadList", "Ljava/util/List;");
        Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj, multipartRelatedPayloadList_valId, multipartRelatedPayloadList_jobject);
    }

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_lls_sls_alc_on_package_extract_completed_ID, jobj);

    for (std::list<jstring>::iterator it=to_clean_jstrings.begin(); it != to_clean_jstrings.end(); ++it) {
        Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(*it);
    }
    to_clean_jstrings.clear();

    for (std::list<jobject>::iterator it=to_clean_jobject.begin(); it != to_clean_jobject.end(); ++it) {
        Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(*it);
    }
    to_clean_jobject.clear();

    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(jobj);
}

void Atsc3NdkApplicationBridge::atsc3_sls_on_held_trigger_received_callback_jni(uint16_t service_id, const char *held_payload) {
	if (!JReady() || !atsc3_lls_sls_alc_on_route_mpd_patched_ID)
		return;

	if (!Atsc3_Jni_Processing_Thread_Env) {
		eprintf("!! err on get jni env\n");
		return;
	}

	atsc3_onAlcObjectStatusMessage("HELD: service_id: %d, xml:\n%s", service_id, held_payload);
}

void Atsc3NdkApplicationBridge::atsc3_onSlsTablePresent(const char *sls_payload_xml) {
    if (!JReady() || !atsc3_onSlsTablePresent_ID) {
        eprintf("err: JReady: %d, atsc3_onSlsTablePresent_ID: %d",  JReady(), atsc3_onSlsTablePresent_ID);

        return;
    }

    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    jstring xml_payload = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(sls_payload_xml);
    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onSlsTablePresent_ID, xml_payload);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(xml_payload);
}


//--------------------------------------------------------------------------

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_apiAppBridgeInit(JNIEnv *env, jobject instance, jobject drvIntf)
{
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_apiAppBridgeInit: start init, env: %p\n", env);

    apiAppBridge.mJniEnv = env;

    env->GetJavaVM(&apiAppBridge.mJavaVM);
    if(apiAppBridge.mJavaVM == NULL) {
        eprintf("!! no java vm\n");
        return -1;
    }

    jclass jClazz = env->FindClass("org/ngbp/libatsc3/sampleapp/Atsc3NdkPHYBridge");
    if (jClazz == NULL) {
        eprintf("!! Cannot find Atsc3NdkPHYBridge java class\n");
        return -1;
    }
    apiAppBridge.mOnLogMsgId = env->GetMethodID(jClazz, "onLogMsg", "(Ljava/lang/String;)I");
    if (apiAppBridge.mOnLogMsgId == NULL) {
        eprintf("!! Cannot find 'onLogMsg' method id\n");
        return -1;
    }

    //atsc3_onSlsTablePresent_ID
    apiAppBridge.atsc3_onSlsTablePresent_ID = env->GetMethodID(jClazz, "atsc3_onSlsTablePresent", "(Ljava/lang/String;)I");
    if (apiAppBridge.atsc3_onSlsTablePresent_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_onSlsTablePresent_ID' method id\n");
        return -1;
    }

    //java.nio.ByteBuffer, L: fully qualified class, J: long
    apiAppBridge.atsc3_onMfuPacketID = env->GetMethodID(jClazz, "atsc3_onMfuPacket", "(IIILjava/nio/ByteBuffer;IJI)I");
    if (apiAppBridge.atsc3_onMfuPacketID == NULL) {
        eprintf("!! Cannot find 'atsc3_onMfuPacket' method id\n");
        return -1;
    }
    //java.nio.ByteBuffer, L: fully qualified class, J: long
    apiAppBridge.atsc3_onMfuPacketCorruptID = env->GetMethodID(jClazz, "atsc3_onMfuPacketCorrupt", "(IIILjava/nio/ByteBuffer;IJII)I");
    if (apiAppBridge.atsc3_onMfuPacketCorruptID == NULL) {
        eprintf("!! Cannot find 'atsc3_onMfuPacketCorrupt' method id\n");
        return -1;
    }

    //java.nio.ByteBuffer, L: fully qualified class, J: long
    apiAppBridge.atsc3_onMfuPacketCorruptMmthSampleHeaderID = env->GetMethodID(jClazz, "atsc3_onMfuPacketCorruptMmthSampleHeader", "(IIILjava/nio/ByteBuffer;IJII)I");
    if (apiAppBridge.atsc3_onMfuPacketCorruptMmthSampleHeaderID == NULL) {
        eprintf("!! Cannot find 'atsc3_onMfuPacketCorruptMmthSampleHeaderID' method id\n");
        return -1;
    }


    //java.nio.ByteBuffer, L: fully qualified class, J: long
    apiAppBridge.atsc3_onMfuSampleMissingID = env->GetMethodID(jClazz, "atsc3_onMfuSampleMissing", "(III)I");
    if (apiAppBridge.atsc3_onMfuSampleMissingID == NULL) {
        eprintf("!! Cannot find 'atsc3_onMfuSampleMissingID' method id\n");
        return -1;
    }

    //java.nio.ByteBuffer
    apiAppBridge.mOnInitHEVC_NAL_Extracted = env->GetMethodID(jClazz, "atsc3_onInitHEVC_NAL_Packet", "(IILjava/nio/ByteBuffer;I)I");
    if (apiAppBridge.mOnInitHEVC_NAL_Extracted == NULL) {
        eprintf("!! Cannot find 'atsc3_onInitHEVC_NAL_Packet' method id\n");
        return -1;
    }

    /*
     *  public int atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(int video_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds);
     *  public int atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds);
     *  public int atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds) {
    */

    apiAppBridge.atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jClazz, "atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor", "(IIJII)I");
    if (apiAppBridge.atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID' method id\n");
        return -1;
    }
    apiAppBridge.atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jClazz, "atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor", "(IIJII)I");
    if (apiAppBridge.atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID' method id\n");
        return -1;
    }
    apiAppBridge.atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jClazz, "atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor", "(IIJII)I");
    if (apiAppBridge.atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID' method id\n");
        return -1;
    }

    apiAppBridge.atsc3_rf_phy_status_callback_ID = env->GetMethodID(jClazz, "atsc3_rf_phy_status_callback", "(IIIIIIIIIIIIIII)I");
    if (apiAppBridge.atsc3_rf_phy_status_callback_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_rf_phy_status_callback' method id\n");
        return -1;
    }

    //atsc3_onExtractedSampleDurationID
    apiAppBridge.atsc3_onExtractedSampleDurationID = env->GetMethodID(jClazz, "atsc3_onExtractedSampleDuration", "(III)I");
    if (apiAppBridge.atsc3_onExtractedSampleDurationID == NULL) {
        eprintf("!! Cannot find 'atsc3_onExtractedSampleDurationID' method id\n");
        return -1;
    }

    //atsc3_setVideoWidthHeightFromTrakID
    apiAppBridge.atsc3_setVideoWidthHeightFromTrakID = env->GetMethodID(jClazz, "atsc3_setVideoWidthHeightFromTrak", "(II)I");
    if (apiAppBridge.atsc3_setVideoWidthHeightFromTrakID == NULL) {
        eprintf("!! Cannot find 'atsc3_setVideoWidthHeightFromTrakID' method id\n");
        return -1;
    }

    //atsc3_update_rf_bw_stats_ID
    apiAppBridge.atsc3_update_rf_bw_stats_ID = env->GetMethodID(jClazz, "atsc3_updateRfBwStats", "(JJI)I");
    if (apiAppBridge.atsc3_update_rf_bw_stats_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_update_rf_bw_stats_ID' method id\n");
        return -1;
    }

    //atsc3_lls_sls_alc_on_route_mpd_patched_ID
    apiAppBridge.atsc3_lls_sls_alc_on_route_mpd_patched_ID = env->GetMethodID(jClazz, "atsc3_lls_sls_alc_on_route_mpd_patched", "(I)I");
    if (apiAppBridge.atsc3_lls_sls_alc_on_route_mpd_patched_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_lls_sls_alc_on_route_mpd_patched_ID' method id\n");
        return -1;
    }

    //atsc3_on_alc_object_status_message_ID
    apiAppBridge.atsc3_on_alc_object_status_message_ID = env->GetMethodID(jClazz, "atsc3_on_alc_object_status_message", "(Ljava/lang/String;)I");
    if (apiAppBridge.atsc3_on_alc_object_status_message_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_on_alc_object_status_message_ID' method id\n");
        return -1;
    }

    //jjustman-2020-07-27 - atsc3_lls_sls_alc_on_package_extract_completed_ID
    //org.ngbp.libatsc3.middleware.android.a331.PackageExtractEnvelopeMetadataAndPayload
	apiAppBridge.atsc3_lls_sls_alc_on_package_extract_completed_ID = env->GetMethodID(jClazz, "atsc3_lls_sls_alc_on_package_extract_completed", "(Lorg/ngbp/libatsc3/android/PackageExtractEnvelopeMetadataAndPayload;)I");
	if (apiAppBridge.atsc3_lls_sls_alc_on_package_extract_completed_ID == NULL) {
	   eprintf("!! Cannot find 'atsc3_lls_sls_alc_on_package_extract_completed_ID' method id\n");
	   return -1;
	}

	apiAppBridge.packageExtractEnvelopeMetadataAndPayload_jclass_init_env = env->FindClass("org/ngbp/libatsc3/android/PackageExtractEnvelopeMetadataAndPayload");

    if (apiAppBridge.packageExtractEnvelopeMetadataAndPayload_jclass_init_env == NULL) {
        eprintf("Cannot find 'packageExtractEnvelopeMetadataAndPayload_jclass' class reference\n");
        return -1;
    } else {
        apiAppBridge.packageExtractEnvelopeMetadataAndPayload_jclass_global_ref = (jclass)(env->NewGlobalRef(apiAppBridge.packageExtractEnvelopeMetadataAndPayload_jclass_init_env));
    }

    apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_init_env = env->FindClass("org/ngbp/libatsc3/android/PackageExtractEnvelopeMetadataAndPayload$MultipartRelatedPayload");
    if (apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_init_env == NULL) {
        eprintf("Cannot find 'packageExtractEnvelopeMetadataAndPayload$MultipartRelatedPayload_jclass_init_env' class reference\n");
        return -1;
    } else {
       apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref = (jclass)(env->NewGlobalRef(apiAppBridge.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_init_env));
    }

    apiAppBridge.jni_java_util_ArrayList = (jclass) env->NewGlobalRef(env->FindClass("java/util/ArrayList"));
    eprintf("creating apiAppBridge.jni_java_util_ArrayList");

    apiAppBridge.jni_java_util_ArrayList_cctor = env->GetMethodID(apiAppBridge.jni_java_util_ArrayList, "<init>", "(I)V");
    eprintf("creating apiAppBridge.jni_java_util_ArrayList_cctor");
    apiAppBridge.jni_java_util_ArrayList_add  = env->GetMethodID(apiAppBridge.jni_java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");
    eprintf("creating apiAppBridge.jni_java_util_ArrayList_add");

    apiAppBridge.mClsDrvIntf = (jclass)(apiAppBridge.mJniEnv->NewGlobalRef(drvIntf));

//    int r = apiAppBridge.Init();
//    if (r)
//        return r;

    apiAppBridge.LogMsg("apiAppBridge init ok");

    //wire up atsc3_phy_mmt_player_bridge
    //Atsc3NdkPHYBridge* at3DrvIntf_ptr
  //  atsc3_phy_player_bridge_init(&apiAppBridge);

    printf("**** jni init OK\n");
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_atsc3_1slt_1selectService(JNIEnv *env, jobject thiz,
                                                         jint service_id) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_atsc3_1slt_1selectService, service_id: %d\n", (int)service_id);
    int ret = apiAppBridge.atsc3_slt_selectService((int)service_id);

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_apiAppBridgeSetPLP(JNIEnv *env, jobject thiz, jintArray a_plp_ids) {
    // TODO: implement apiAppBridgeSetPLP()
//
//    jsize len = *env->GetArrayLength(a_plp_ids);
//    jint *a_body = *env->GetIntArrayElements(a_plp_ids, 0);
////    for (int i=0; i<len; i++) {
////        sum += body[i];
////    }
//    apiAppBridge.set_plp_settings(a_body, len);

    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_atsc3_1slt_1alc_1select_1additional_1service(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jint service_id) {

    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_atsc3_1slt_1alc_1select_1additional_1service, additional service_id: %d\n", (int)service_id);
    int ret = apiAppBridge.atsc3_slt_alc_select_additional_service((int)service_id);

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_atsc3_1slt_1alc_1clear_1additional_1service_1selections(
        JNIEnv *env, jobject thiz) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_atsc3_1slt_1alc_1clear_1additional_1service_1selections\n");
    int ret = apiAppBridge.atsc3_slt_alc_clear_additional_service_selections();
    return ret;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_atsc3_1slt_1alc_1get_1sls_1metadata_1fragments_1content_1locations_1from_1monitor_1service_1id(JNIEnv *env, jobject thiz, jint service_id, jstring to_match_content_type_string) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_atsc3_1slt_1alc_1get_1sls_1metadata_1fragments_1content_1locations_1from_1monitor_1service_1id, service_id: %d\n", (int)service_id);
    const char* to_match_content_type_weak = env->GetStringUTFChars(to_match_content_type_string, 0);

    vector<string> slt_alc_sls_metadata_fragment_content_locations = apiAppBridge.atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id((int)service_id, to_match_content_type_weak);

    jstring str;
    int i;

    jobjectArray slt_alc_sls_metadata_fragment_content_locations_jni = env->NewObjectArray(slt_alc_sls_metadata_fragment_content_locations.size(), env->FindClass("java/lang/String"),0);

    for(i=0; i < slt_alc_sls_metadata_fragment_content_locations.size(); i++) {
        str = env->NewStringUTF(slt_alc_sls_metadata_fragment_content_locations.at(i).c_str());
        env->SetObjectArrayElement(slt_alc_sls_metadata_fragment_content_locations_jni, i, str);
    }
    env->ReleaseStringUTFChars( to_match_content_type_string, to_match_content_type_weak );

    return slt_alc_sls_metadata_fragment_content_locations_jni;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_atsc3_1slt_1alc_1get_1sls_1route_1s_1tsid_1fdt_1file_1content_1locations_1from_1monitor_1service_1id(
        JNIEnv *env, jobject thiz, jint service_id) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkApplicationBridge_atsc3_1slt_1alc_1get_1sls_1route_1s_1tsid_1fdt_1file_1content_1locations_1from_1monitor_1service_1id, service_id: %d\n", (int)service_id);

    vector<string> slt_alc_sls_route_s_tsid_fdt_file_content_locations = apiAppBridge.atsc3_slt_alc_get_sls_route_s_tsid_fdt_file_content_locations_from_monitor_service_id((int)service_id);

    jstring str;
    int i;

    jobjectArray slt_alc_sls_route_s_tsid_fdt_file_content_locations_jni = env->NewObjectArray(slt_alc_sls_route_s_tsid_fdt_file_content_locations.size(), env->FindClass("java/lang/String"),0);

    for(i=0; i < slt_alc_sls_route_s_tsid_fdt_file_content_locations.size(); i++) {
        str = env->NewStringUTF(slt_alc_sls_route_s_tsid_fdt_file_content_locations.at(i).c_str());
        env->SetObjectArrayElement(slt_alc_sls_route_s_tsid_fdt_file_content_locations_jni, i, str);
    }

    return slt_alc_sls_route_s_tsid_fdt_file_content_locations_jni;
}

