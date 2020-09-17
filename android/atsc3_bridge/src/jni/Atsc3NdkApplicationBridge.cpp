#include "Atsc3NdkApplicationBridge.h"

Atsc3NdkApplicationBridge* apiAppBridge;

Atsc3NdkApplicationBridge::Atsc3NdkApplicationBridge(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = env->NewGlobalRef(jni_instance);
}

void Atsc3NdkApplicationBridge::atsc3_onMfuPacket(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_rebuilt)
{
    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //_NDK_APPLICATION_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = bridgeConsumerJniEnv->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_rebuilt);
    //_NDK_APPLICATION_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}


void Atsc3NdkApplicationBridge::atsc3_onMfuPacketCorrupt(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt)
{
    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //_NDK_APPLICATION_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = bridgeConsumerJniEnv->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketCorruptID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    //_NDK_APPLICATION_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}


void Atsc3NdkApplicationBridge::atsc3_onMfuPacketCorruptMmthSampleHeader(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt)
{
    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //_NDK_APPLICATION_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = bridgeConsumerJniEnv->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketCorruptMmthSampleHeaderID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    //_NDK_APPLICATION_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}

//push extracted HEVC nal's to MediaCodec for init
void Atsc3NdkApplicationBridge::atsc3_onInitHEVC_NAL_Extracted(uint16_t packet_id, uint32_t mpu_sequence_number, uint8_t* buffer, uint32_t bufferLen) {
    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__

    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_nal_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(jni_instance_globalRef, mOnInitHEVC_NAL_Extracted, is_video, mpu_sequence_number, jobjectGlobalByteBuffer, bufferLen);
    //_NDK_APPLICATION_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    //env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectByteBuffer = bridgeConsumerJniEnv->Get()->NewDirectByteBuffer(buffer, bufferLen);
    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, mOnInitHEVC_NAL_Extracted, packet_id, mpu_sequence_number, jobjectByteBuffer, bufferLen);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(jobjectByteBuffer);
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
    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID, video_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}


void Atsc3NdkApplicationBridge::atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID, audio_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}



void Atsc3NdkApplicationBridge::atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

    int r =  bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID, stpp_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

void Atsc3NdkApplicationBridge::atsc3_onMfuSampleMissing(uint16_t pcaket_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuSampleMissingID, pcaket_id, mpu_sequence_number, sample_number);
}

//jjustman-2020-08-19: TODO: get (or create) a pinned Atsc3JniEnv from pthread_cur
void Atsc3NdkApplicationBridge::LogMsg(const char *msg)
{
    if (!mJavaVM) {
        _NDK_APPLICATION_BRIDGE_ERROR("LogMsg: mJavaVM is NULL!");
        return;
    }

    Atsc3JniEnv env(mJavaVM);
    if (!env) {
        _NDK_APPLICATION_BRIDGE_ERROR("LogMsg: error creating env pin!");
        return;
    }
    jstring js = env.Get()->NewStringUTF(msg);
    int r = env.Get()->CallIntMethod(jni_instance_globalRef, mOnLogMsgId, js);
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

    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

    jstring js = bridgeConsumerJniEnv->Get()->NewStringUTF(msg);

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_on_alc_object_status_message_ID, js);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(js);
}

int Atsc3NdkApplicationBridge::pinConsumerThreadAsNeeded() {
    _NDK_APPLICATION_BRIDGE_INFO("Atsc3NdkApplicationBridge::pinConsumerThreadAsNeeded: mJavaVM: %p", mJavaVM);
    bridgeConsumerJniEnv = new Atsc3JniEnv(mJavaVM);
    return 0;
}

int Atsc3NdkApplicationBridge::releasePinnedConsumerThreadAsNeeded() {
    _NDK_APPLICATION_BRIDGE_INFO("Atsc3NdkApplicationBridge::releasePinnedConsumerThreadAsNeeded: bridgeConsumerJniEnv is: %p:", bridgeConsumerJniEnv);
    if(bridgeConsumerJniEnv) {
        delete bridgeConsumerJniEnv;
    }
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

    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }
    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onExtractedSampleDurationID,
                                                       packet_id,
                                                       mpu_sequence_number,
                                                       extracted_sample_duration_us);
}



void Atsc3NdkApplicationBridge::atsc3_setVideoWidthHeightFromTrak(uint32_t width, uint32_t height) {

    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }
    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_setVideoWidthHeightFromTrakID, width, height);
}


void Atsc3NdkApplicationBridge::set_plp_settings(jint *a_plp_ids, jsize a_plp_size) {

    uint8_t* u_plp_ids = (uint8_t*)calloc(a_plp_size, sizeof(uint8_t));
    for(int i=0; i < a_plp_size; i++) {
        u_plp_ids[i] = (uint8_t)a_plp_ids[i];
    }
}

std::string Atsc3NdkApplicationBridge::get_android_temp_folder() {
    if(!apiAppBridge) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge::get_android_temp_folder - apiAppBridge is NULL!");
        return "";
    }
    Atsc3JniEnv env(mJavaVM);

    jmethodID getCacheDir = env.Get()->GetMethodID( jni_class_globalRef, "getCacheDir", "()Ljava/io/File;" );
    jobject cache_dir = env.Get()->CallObjectMethod(jni_instance_globalRef, getCacheDir );

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
    _NDK_APPLICATION_BRIDGE_INFO("atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_jni: tsi: %d, toi: %d, content_location: %s", tsi, toi, content_location);

    //len: nnn, lost DU: nnn
    atsc3_onAlcObjectStatusMessage("C: tsi: %d, toi: %d, content_location: %s", tsi, toi, content_location);
}

void Atsc3NdkApplicationBridge::atsc3_lls_sls_alc_on_route_mpd_patched_jni(uint16_t service_id) {
    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_lls_sls_alc_on_route_mpd_patched_ID, service_id);
}

// https://stackoverflow.com/questions/6343459/get-strings-used-in-java-from-jni
void Atsc3NdkApplicationBridge::atsc3_lls_sls_alc_on_package_extract_completed_callback_jni(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload) {
    if (!atsc3_lls_sls_alc_on_package_extract_completed_ID)
        return;

    if (!bridgeConsumerJniEnv) {
        _NDK_APPLICATION_BRIDGE_ERROR("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err on get jni env: bridgeConsumerJniEnv");
        return;
    }

    if(!atsc3_route_package_extracted_envelope_metadata_and_payload) {
        _NDK_APPLICATION_BRIDGE_ERROR("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err atsc3_route_package_extracted_envelope_metadata_and_payload is NULL");
        return;
    }

    if(!atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml || !atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml->p_buffer) {
        _NDK_APPLICATION_BRIDGE_ERROR("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml (or p_buffer) is NULL");
        return;
    }

    if(!atsc3_route_package_extracted_envelope_metadata_and_payload->package_name) {
    	_NDK_APPLICATION_BRIDGE_ERROR("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err atsc3_route_package_extracted_envelope_metadata_and_payload->package_name is NULL");
        return;
    }

    std::list<jstring> to_clean_jstrings;
    std::list<jobject> to_clean_jobject;

    //org.ngbp.libatsc3.middleware.android.a331.PackageExtractEnvelopeMetadataAndPayload
    jclass jcls = apiAppBridge->packageExtractEnvelopeMetadataAndPayload_jclass_global_ref;
    jobject jobj = bridgeConsumerJniEnv->Get()->AllocObject(jcls);

    if(!jobj) {
        _NDK_APPLICATION_BRIDGE_ERROR("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err unable to allocate packageExtractEnvelopeMetadataAndPayload_jclass_global_ref instance jobj!");
        return;
    }


    jfieldID packageName_valId = bridgeConsumerJniEnv->Get()->GetFieldID(jcls, "packageName", "Ljava/lang/String;");
    jstring packageName_payload = bridgeConsumerJniEnv->Get()->NewStringUTF(atsc3_route_package_extracted_envelope_metadata_and_payload->package_name);
    bridgeConsumerJniEnv->Get()->SetObjectField(jobj, packageName_valId, packageName_payload);
    to_clean_jstrings.push_back(packageName_payload);

    jfieldID tsi_valId = bridgeConsumerJniEnv->Get()->GetFieldID(jcls, "tsi", "I");
    bridgeConsumerJniEnv->Get()->SetIntField(jobj, tsi_valId, atsc3_route_package_extracted_envelope_metadata_and_payload->tsi);

    jfieldID toi_valId = bridgeConsumerJniEnv->Get()->GetFieldID(jcls, "toi", "I");
    bridgeConsumerJniEnv->Get()->SetIntField(jobj, toi_valId, atsc3_route_package_extracted_envelope_metadata_and_payload->toi);

    jfieldID appContextIdList_valId = bridgeConsumerJniEnv->Get()->GetFieldID(jcls, "appContextIdList", "Ljava/lang/String;");
    jstring appContextIdList_payload = bridgeConsumerJniEnv->Get()->NewStringUTF(atsc3_route_package_extracted_envelope_metadata_and_payload->app_context_id_list);
    bridgeConsumerJniEnv->Get()->SetObjectField(jobj, appContextIdList_valId, appContextIdList_payload);
    to_clean_jstrings.push_back(appContextIdList_payload);


    jfieldID packageExtractPath_valId = bridgeConsumerJniEnv->Get()->GetFieldID(jcls, "packageExtractPath", "Ljava/lang/String;");
    jstring packageExtractPath_payload = bridgeConsumerJniEnv->Get()->NewStringUTF(atsc3_route_package_extracted_envelope_metadata_and_payload->package_extract_path);
    bridgeConsumerJniEnv->Get()->SetObjectField(jobj, packageExtractPath_valId, packageExtractPath_payload);
    to_clean_jstrings.push_back(packageExtractPath_payload);


    jfieldID mbmsEnvelopeRawXml_valId = bridgeConsumerJniEnv->Get()->GetFieldID(jcls, "mbmsEnvelopeRawXml", "Ljava/lang/String;");
    jstring mbmsEnvelopeRawXml_payload = bridgeConsumerJniEnv->Get()->NewStringUTF((char*)atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml->p_buffer);
    bridgeConsumerJniEnv->Get()->SetObjectField(jobj, mbmsEnvelopeRawXml_valId, mbmsEnvelopeRawXml_payload);
    to_clean_jstrings.push_back(mbmsEnvelopeRawXml_payload);

    if(atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.count > 0) {

        jobject multipartRelatedPayloadList_jobject = bridgeConsumerJniEnv->Get()->NewObject(apiAppBridge->jni_java_util_ArrayList, apiAppBridge->jni_java_util_ArrayList_cctor, atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.count);

        for(int i=0; i < atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.count; i++) {
            atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.data[i];
            jobject jobj_multipart_related_payload_jobject = bridgeConsumerJniEnv->Get()->AllocObject(apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref);

            to_clean_jobject.push_back(jobj_multipart_related_payload_jobject);

            jfieldID contentLocation_valId = bridgeConsumerJniEnv->Get()->GetFieldID(apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "contentLocation", "Ljava/lang/String;");
            jstring contentLocation_jstring = bridgeConsumerJniEnv->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->sanitizied_content_location);
            bridgeConsumerJniEnv->Get()->SetObjectField(jobj_multipart_related_payload_jobject, contentLocation_valId, contentLocation_jstring);
            to_clean_jstrings.push_back(contentLocation_jstring);

            if(atsc3_mime_multipart_related_payload->content_type) {
                jfieldID contentType_valId = bridgeConsumerJniEnv->Get()->GetFieldID(apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "contentType", "Ljava/lang/String;");
                jstring contentType_jstring = bridgeConsumerJniEnv->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->content_type);
                bridgeConsumerJniEnv->Get()->SetObjectField(jobj_multipart_related_payload_jobject, contentType_valId, contentType_jstring);
                to_clean_jstrings.push_back(contentType_jstring);
            }

            if(atsc3_mime_multipart_related_payload->valid_from_string) {
                jfieldID validFrom_valId = bridgeConsumerJniEnv->Get()->GetFieldID(apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "validFrom", "Ljava/lang/String;");
                jstring validFrom_jstring = bridgeConsumerJniEnv->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->valid_from_string);
                bridgeConsumerJniEnv->Get()->SetObjectField(jobj_multipart_related_payload_jobject, validFrom_valId, validFrom_jstring);
                to_clean_jstrings.push_back(validFrom_jstring);
            }

            if(atsc3_mime_multipart_related_payload->valid_until_string) {
                jfieldID validUntil_valId = bridgeConsumerJniEnv->Get()->GetFieldID(apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "validUntil", "Ljava/lang/String;");
                jstring validUntil_jstring = bridgeConsumerJniEnv->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->valid_until_string);
                bridgeConsumerJniEnv->Get()->SetObjectField(jobj_multipart_related_payload_jobject, validUntil_valId, validUntil_jstring);
                to_clean_jstrings.push_back(validUntil_jstring);
            }

            //version

            jfieldID version_valId = bridgeConsumerJniEnv->Get()->GetFieldID(apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "version", "I");
            bridgeConsumerJniEnv->Get()->SetIntField(jobj_multipart_related_payload_jobject, version_valId, atsc3_mime_multipart_related_payload->version);


            if(atsc3_mime_multipart_related_payload->next_url_string) {
                jfieldID nextUrl_valId = bridgeConsumerJniEnv->Get()->GetFieldID(apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "nextUrl", "Ljava/lang/String;");
                jstring nextUrl_jstring = bridgeConsumerJniEnv->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->next_url_string);
                bridgeConsumerJniEnv->Get()->SetObjectField(jobj_multipart_related_payload_jobject, nextUrl_valId, nextUrl_jstring);
                to_clean_jstrings.push_back(nextUrl_jstring);
            }

            if(atsc3_mime_multipart_related_payload->avail_at_string) {
                jfieldID availAt_valId = bridgeConsumerJniEnv->Get()->GetFieldID(apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "availAt", "Ljava/lang/String;");
                jstring availAt_jstring = bridgeConsumerJniEnv->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->avail_at_string);
                bridgeConsumerJniEnv->Get()->SetObjectField(jobj_multipart_related_payload_jobject, availAt_valId, availAt_jstring);
                to_clean_jstrings.push_back(availAt_jstring);
            }

            //extractedSize
            jfieldID extractedSize_valId = bridgeConsumerJniEnv->Get()->GetFieldID(apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "extractedSize", "I");
            bridgeConsumerJniEnv->Get()->SetIntField(jobj_multipart_related_payload_jobject, extractedSize_valId, atsc3_mime_multipart_related_payload->extracted_size);

            bridgeConsumerJniEnv->Get()->CallBooleanMethod(multipartRelatedPayloadList_jobject, apiAppBridge->jni_java_util_ArrayList_add, jobj_multipart_related_payload_jobject);
        }

        jfieldID multipartRelatedPayloadList_valId = bridgeConsumerJniEnv->Get()->GetFieldID(jcls, "multipartRelatedPayloadList", "Ljava/util/List;");
        bridgeConsumerJniEnv->Get()->SetObjectField(jobj, multipartRelatedPayloadList_valId, multipartRelatedPayloadList_jobject);
    }

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_lls_sls_alc_on_package_extract_completed_ID, jobj);

    for (std::list<jstring>::iterator it=to_clean_jstrings.begin(); it != to_clean_jstrings.end(); ++it) {
        bridgeConsumerJniEnv->Get()->DeleteLocalRef(*it);
    }
    to_clean_jstrings.clear();

    for (std::list<jobject>::iterator it=to_clean_jobject.begin(); it != to_clean_jobject.end(); ++it) {
        bridgeConsumerJniEnv->Get()->DeleteLocalRef(*it);
    }
    to_clean_jobject.clear();

    bridgeConsumerJniEnv->Get()->DeleteLocalRef(jobj);
}


void Atsc3NdkApplicationBridge::atsc3_onSlsTablePresent(const char *sls_payload_xml) {
    if (!atsc3_onSlsTablePresent_ID) {
        _NDK_APPLICATION_BRIDGE_ERROR("atsc3_onSlsTablePresent_ID: %p", atsc3_onSlsTablePresent_ID);
        return;
    }

    if (!bridgeConsumerJniEnv) {
		_NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge::atsc3_onSlsHeldEmissionPresent: bridgeConsumerJniEnv is NULL");
        return;
    }
    if (!sls_payload_xml || !strlen(sls_payload_xml)) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge::atsc3_onSlsHeldEmissionPresent: sls_payload_xml is NULL!");
        return;
    }

    _NDK_APPLICATION_BRIDGE_INFO("Atsc3NdkApplicationBridge::atsc3_onSlsHeldEmissionPresent: sls_payload_xml is: %s", sls_payload_xml);


    jstring xml_payload = bridgeConsumerJniEnv->Get()->NewStringUTF(sls_payload_xml);
    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onSlsTablePresent_ID, xml_payload);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(xml_payload);
}



void Atsc3NdkApplicationBridge::atsc3_onAeatTablePresent(const char *aeat_payload_xml) {
    if (!atsc3_onAeatTablePresent_ID) {
        _NDK_APPLICATION_BRIDGE_ERROR("atsc3_onAeatTablePresent: %p", atsc3_onAeatTablePresent_ID);

        return;
    }

    if (!bridgeConsumerJniEnv) {
		_NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge::atsc3_onSlsHeldEmissionPresent: bridgeConsumerJniEnv is NULL");
        return;
    }

    jstring xml_payload = bridgeConsumerJniEnv->Get()->NewStringUTF(aeat_payload_xml);
    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onAeatTablePresent_ID, xml_payload);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(xml_payload);
}


void Atsc3NdkApplicationBridge::atsc3_onSlsHeldEmissionPresent(uint16_t service_id, const char *held_payload_xml) {
	if (!atsc3_onSlsHeldEmissionPresent_ID)
		return;

	if (!bridgeConsumerJniEnv) {
		_NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge::atsc3_onSlsHeldEmissionPresent: bridgeConsumerJniEnv is NULL");
		return;
	}

    jstring xml_payload = bridgeConsumerJniEnv->Get()->NewStringUTF(held_payload_xml);
    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onSlsHeldEmissionPresent_ID, service_id, xml_payload);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(xml_payload);
}


void Atsc3NdkApplicationBridge::atsc3_phy_notify_plp_selection_change_set_callback(atsc3_phy_notify_plp_selection_change_f atsc3_phy_notify_plp_selection_change, void* context) {
    this->atsc3_phy_notify_plp_selection_change = atsc3_phy_notify_plp_selection_change;
    this->atsc3_phy_notify_plp_selection_change_context = context;
}

void Atsc3NdkApplicationBridge::atsc3_phy_notify_plp_selection_change_clear_callback() {
    this->atsc3_phy_notify_plp_selection_change = nullptr;
}

void Atsc3NdkApplicationBridge::atsc3_phy_notify_plp_selection_changed(vector<uint8_t> plps_to_listen) {
    if(this->atsc3_phy_notify_plp_selection_change_context) {
        this->atsc3_phy_notify_plp_selection_change(plps_to_listen, this->atsc3_phy_notify_plp_selection_change_context);
    }

}


//--------------------------------------------------------------------------

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_init(JNIEnv *env, jobject instance)
{
    apiAppBridge = new Atsc3NdkApplicationBridge(env, instance);
    _NDK_APPLICATION_BRIDGE_INFO("Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_init:  env: %p", env);
    apiAppBridge->setJniClassReference("org/ngbp/libatsc3/middleware/Atsc3NdkApplicationBridge");
    apiAppBridge->mJavaVM = atsc3_bridge_ndk_static_loader_get_javaVM();

    if(apiAppBridge->mJavaVM == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: apiAppBridge->mJavaVM is NULL!");
        return -1;
    }

    jclass jniClassReference = apiAppBridge->getJniClassReference();

    if (jniClassReference == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find Atsc3NdkApplicationBridge java class reference!");
        return -2;
    }

    apiAppBridge->mOnLogMsgId = env->GetMethodID(jniClassReference, "onLogMsg", "(Ljava/lang/String;)I");
    if (apiAppBridge->mOnLogMsgId == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("AppBridge_init - cannot find 'onLogMsg' method id");
        return -1;
    }

    //atsc3_onSlsTablePresent_ID
    apiAppBridge->atsc3_onSlsTablePresent_ID = env->GetMethodID(jniClassReference, "atsc3_onSlsTablePresent", "(Ljava/lang/String;)I");
    if (apiAppBridge->atsc3_onSlsTablePresent_ID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_onSlsTablePresent_ID' method id");
        return -1;
    }

    //atsc3_onAeatTablePresent_ID
     apiAppBridge->atsc3_onAeatTablePresent_ID = env->GetMethodID(jniClassReference, "atsc3_onAeatTablePresent", "(Ljava/lang/String;)I");
     if (apiAppBridge->atsc3_onAeatTablePresent_ID == NULL) {
         _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_onAeatTablePresent_ID' method id");
         return -1;
     }

     //atsc3_onSlsHeldEmissionPresent
     apiAppBridge->atsc3_onSlsHeldEmissionPresent_ID = env->GetMethodID(jniClassReference, "atsc3_onSlsHeldEmissionPresent", "(ILjava/lang/String;)I");
     if (apiAppBridge->atsc3_onSlsHeldEmissionPresent_ID == NULL) {
     	_NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_onSlsHeldEmissionPresent_ID' method id");
        	return -1;
     }

    //java.nio.ByteBuffer, L: fully qualified class, J: long
    apiAppBridge->atsc3_onMfuPacketID = env->GetMethodID(jniClassReference, "atsc3_onMfuPacket", "(IIILjava/nio/ByteBuffer;IJI)I");
    if (apiAppBridge->atsc3_onMfuPacketID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_onMfuPacket' method id");
        return -1;
    }
    //java.nio.ByteBuffer, L: fully qualified class, J: long
    apiAppBridge->atsc3_onMfuPacketCorruptID = env->GetMethodID(jniClassReference, "atsc3_onMfuPacketCorrupt", "(IIILjava/nio/ByteBuffer;IJII)I");
    if (apiAppBridge->atsc3_onMfuPacketCorruptID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("AppBridge_init - Atsc3NdkApplicationBridge_init: cannot find 'atsc3_onMfuPacketCorrupt' method id");
        return -1;
    }

    //java.nio.ByteBuffer, L: fully qualified class, J: long
    apiAppBridge->atsc3_onMfuPacketCorruptMmthSampleHeaderID = env->GetMethodID(jniClassReference, "atsc3_onMfuPacketCorruptMmthSampleHeader", "(IIILjava/nio/ByteBuffer;IJII)I");
    if (apiAppBridge->atsc3_onMfuPacketCorruptMmthSampleHeaderID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_onMfuPacketCorruptMmthSampleHeaderID' method id");
        return -1;
    }


    //java.nio.ByteBuffer, L: fully qualified class, J: long
    apiAppBridge->atsc3_onMfuSampleMissingID = env->GetMethodID(jniClassReference, "atsc3_onMfuSampleMissing", "(III)I");
    if (apiAppBridge->atsc3_onMfuSampleMissingID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_onMfuSampleMissingID' method id");
        return -1;
    }

    //java.nio.ByteBuffer
    apiAppBridge->mOnInitHEVC_NAL_Extracted = env->GetMethodID(jniClassReference, "atsc3_onInitHEVC_NAL_Packet", "(IILjava/nio/ByteBuffer;I)I");
    if (apiAppBridge->mOnInitHEVC_NAL_Extracted == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_onInitHEVC_NAL_Packet' method id");
        return -1;
    }

    /*
     *  public int atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(int video_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds);
     *  public int atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds);
     *  public int atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds) {
    */

    apiAppBridge->atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jniClassReference, "atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor", "(IIJII)I");
    if (apiAppBridge->atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID' method id");
        return -1;
    }
    apiAppBridge->atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jniClassReference, "atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor", "(IIJII)I");
    if (apiAppBridge->atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID' method id");
        return -1;
    }
    apiAppBridge->atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jniClassReference, "atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor", "(IIJII)I");
    if (apiAppBridge->atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID' method id");
        return -1;
    }

    //atsc3_onExtractedSampleDurationID
    apiAppBridge->atsc3_onExtractedSampleDurationID = env->GetMethodID(jniClassReference, "atsc3_onExtractedSampleDuration", "(III)I");
    if (apiAppBridge->atsc3_onExtractedSampleDurationID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_onExtractedSampleDurationID' method id");
        return -1;
    }

    //atsc3_setVideoWidthHeightFromTrakID
    apiAppBridge->atsc3_setVideoWidthHeightFromTrakID = env->GetMethodID(jniClassReference, "atsc3_setVideoWidthHeightFromTrak", "(II)I");
    if (apiAppBridge->atsc3_setVideoWidthHeightFromTrakID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_setVideoWidthHeightFromTrakID' method id");
        return -1;
    }

    //atsc3_lls_sls_alc_on_route_mpd_patched_ID
    apiAppBridge->atsc3_lls_sls_alc_on_route_mpd_patched_ID = env->GetMethodID(jniClassReference, "atsc3_lls_sls_alc_on_route_mpd_patched", "(I)I");
    if (apiAppBridge->atsc3_lls_sls_alc_on_route_mpd_patched_ID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_lls_sls_alc_on_route_mpd_patched_ID' method id");
        return -1;
    }

    //atsc3_on_alc_object_status_message_ID
    apiAppBridge->atsc3_on_alc_object_status_message_ID = env->GetMethodID(jniClassReference, "atsc3_on_alc_object_status_message", "(Ljava/lang/String;)I");
    if (apiAppBridge->atsc3_on_alc_object_status_message_ID == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_on_alc_object_status_message_ID' method id");
        return -1;
    }

    //jjustman-2020-07-27 - atsc3_lls_sls_alc_on_package_extract_completed_ID
    //org.ngbp.libatsc3.middleware.android.a331.PackageExtractEnvelopeMetadataAndPayload
	apiAppBridge->atsc3_lls_sls_alc_on_package_extract_completed_ID = env->GetMethodID(jniClassReference, "atsc3_lls_sls_alc_on_package_extract_completed", "(Lorg/ngbp/libatsc3/middleware/android/a331/PackageExtractEnvelopeMetadataAndPayload;)I");
	if (apiAppBridge->atsc3_lls_sls_alc_on_package_extract_completed_ID == NULL) {
	   _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'atsc3_lls_sls_alc_on_package_extract_completed_ID' method id");
	   return -1;
	}

	apiAppBridge->packageExtractEnvelopeMetadataAndPayload_jclass_init_env = env->FindClass("org/ngbp/libatsc3/middleware/android/a331/PackageExtractEnvelopeMetadataAndPayload");

    if (apiAppBridge->packageExtractEnvelopeMetadataAndPayload_jclass_init_env == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'packageExtractEnvelopeMetadataAndPayload_jclass' class reference");
        return -1;
    } else {
        apiAppBridge->packageExtractEnvelopeMetadataAndPayload_jclass_global_ref = (jclass)(env->NewGlobalRef(apiAppBridge->packageExtractEnvelopeMetadataAndPayload_jclass_init_env));
    }

    apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_init_env = env->FindClass("org/ngbp/libatsc3/middleware/android/a331/PackageExtractEnvelopeMetadataAndPayload$MultipartRelatedPayload");
    if (apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_init_env == NULL) {
        _NDK_APPLICATION_BRIDGE_ERROR("Atsc3NdkApplicationBridge_init: cannot find 'packageExtractEnvelopeMetadataAndPayload$MultipartRelatedPayload_jclass_init_env' class reference");
        return -1;
    } else {
       apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref = (jclass)(env->NewGlobalRef(apiAppBridge->packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_init_env));
    }

    apiAppBridge->jni_java_util_ArrayList = (jclass) env->NewGlobalRef(env->FindClass("java/util/ArrayList"));
    _NDK_APPLICATION_BRIDGE_TRACE("creating apiAppBridge->jni_java_util_ArrayList");

    apiAppBridge->jni_java_util_ArrayList_cctor = env->GetMethodID(apiAppBridge->jni_java_util_ArrayList, "<init>", "(I)V");
    _NDK_APPLICATION_BRIDGE_TRACE("Atsc3NdkApplicationBridge_init: creating method ref for apiAppBridge->jni_java_util_ArrayList_cctor");
    apiAppBridge->jni_java_util_ArrayList_add  = env->GetMethodID(apiAppBridge->jni_java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");
    _NDK_APPLICATION_BRIDGE_TRACE("Atsc3NdkApplicationBridge_init: creating method ref for  apiAppBridge->jni_java_util_ArrayList_add");




    atsc3_core_service_application_bridge_init(apiAppBridge);
    _NDK_APPLICATION_BRIDGE_INFO("Atsc3NdkApplicationBridge_init: done, with apiAppBridge: %p", apiAppBridge);



    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_atsc3_1slt_1selectService(JNIEnv *env, jobject thiz,
                                                         jint service_id) {
    _NDK_APPLICATION_BRIDGE_INFO("Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_atsc3_1slt_1selectService, service_id: %d\n", (int)service_id);
    int ret = apiAppBridge->atsc3_slt_selectService((int)service_id);

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_apiAppBridgeSetPLP(JNIEnv *env, jobject thiz, jintArray a_plp_ids) {
    // TODO: implement apiAppBridgeSetPLP()
//
//    jsize len = *env->GetArrayLength(a_plp_ids);
//    jint *a_body = *env->GetIntArrayElements(a_plp_ids, 0);
////    for (int i=0; i<len; i++) {
////        sum += body[i];
////    }
//    apiAppBridge->set_plp_settings(a_body, len);

    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_atsc3_1slt_1alc_1select_1additional_1service(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jint service_id) {

    _NDK_APPLICATION_BRIDGE_INFO("Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_atsc3_1slt_1alc_1select_1additional_1service, additional service_id: %d\n", (int)service_id);
    int ret = apiAppBridge->atsc3_slt_alc_select_additional_service((int)service_id);

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_atsc3_1slt_1alc_1clear_1additional_1service_1selections(
        JNIEnv *env, jobject thiz) {
    _NDK_APPLICATION_BRIDGE_INFO("Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_atsc3_1slt_1alc_1clear_1additional_1service_1selections");
    int ret = apiAppBridge->atsc3_slt_alc_clear_additional_service_selections();
    return ret;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_atsc3_1slt_1alc_1get_1sls_1metadata_1fragments_1content_1locations_1from_1monitor_1service_1id(JNIEnv *env, jobject thiz, jint service_id, jstring to_match_content_type_string) {
    _NDK_APPLICATION_BRIDGE_INFO("Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_atsc3_1slt_1alc_1get_1sls_1metadata_1fragments_1content_1locations_1from_1monitor_1service_1id, service_id: %d\n", (int)service_id);
    const char* to_match_content_type_weak = env->GetStringUTFChars(to_match_content_type_string, 0);

    vector<string> slt_alc_sls_metadata_fragment_content_locations = apiAppBridge->atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id((int)service_id, to_match_content_type_weak);

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
Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_atsc3_1slt_1alc_1get_1sls_1route_1s_1tsid_1fdt_1file_1content_1locations_1from_1monitor_1service_1id(
        JNIEnv *env, jobject thiz, jint service_id) {
    _NDK_APPLICATION_BRIDGE_INFO("Java_org_ngbp_libatsc3_middleware_Atsc3NdkApplicationBridge_atsc3_1slt_1alc_1get_1sls_1route_1s_1tsid_1fdt_1file_1content_1locations_1from_1monitor_1service_1id, service_id: %d\n", (int)service_id);

    vector<string> slt_alc_sls_route_s_tsid_fdt_file_content_locations = apiAppBridge->atsc3_slt_alc_get_sls_route_s_tsid_fdt_file_content_locations_from_monitor_service_id((int)service_id);

    jstring str;
    int i;

    jobjectArray slt_alc_sls_route_s_tsid_fdt_file_content_locations_jni = env->NewObjectArray(slt_alc_sls_route_s_tsid_fdt_file_content_locations.size(), env->FindClass("java/lang/String"),0);

    for(i=0; i < slt_alc_sls_route_s_tsid_fdt_file_content_locations.size(); i++) {
        str = env->NewStringUTF(slt_alc_sls_route_s_tsid_fdt_file_content_locations.at(i).c_str());
        env->SetObjectArrayElement(slt_alc_sls_route_s_tsid_fdt_file_content_locations_jni, i, str);
    }

    return slt_alc_sls_route_s_tsid_fdt_file_content_locations_jni;
}
