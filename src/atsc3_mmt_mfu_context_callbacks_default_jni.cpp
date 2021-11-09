//
// Created by Jason Justman on 12/1/20.
//

#include "atsc3_mmt_mfu_context_callbacks_default_jni.h"

IAtsc3NdkMediaMMTBridge* Atsc3NdkMediaMMTBridge_ptr = NULL;

int _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_INFO_ENABLED = 1;
int _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG_ENABLED = 0;
int _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_TRACE_ENABLED = 0;

//only local inclusion
static lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;
static atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = NULL;

static uint32_t global_mfu_proccessed_count = 0;

void atsc3_ndk_media_mmt_bridge_init(IAtsc3NdkMediaMMTBridge* atsc3NdkMediaMMTBridge) {
    Atsc3NdkMediaMMTBridge_ptr = atsc3NdkMediaMMTBridge;
    printf("atsc3_ndk_media_mmt_bridge_init with Atsc3NdkMediaMMTBridge_ptr: %p", Atsc3NdkMediaMMTBridge_ptr);

    atsc3_ndk_media_mmt_bridge_reset_context();

    Atsc3NdkMediaMMTBridge_ptr->LogMsgF("atsc3_ndk_media_mmt_bridge_init - completed");

}

IAtsc3NdkMediaMMTBridge* atsc3_ndk_media_mmt_bridge_get_instance() {
    return Atsc3NdkMediaMMTBridge_ptr;
}

void atsc3_ndk_media_mmt_bridge_reset_context() {

    if(atsc3_mmt_mfu_context) {
        atsc3_mmt_mfu_context_free(&atsc3_mmt_mfu_context);
    }

    lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();

    atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_callbacks_default_jni_new();
}

/*
 * MMTP event callback: atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk
 *                      track init metadata (i.e. moov) is received
 *
 *      note: see atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present for movie fragment metadata (moof)
 *         atsc3_hevc_nals_record_dump("mmt_mpu_metadata", mmt_mpu_metadata);
 */
void atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_mpu_metadata) {

    uint16_t service_id = 0;
    if (atsc3_mmt_mfu_context && atsc3_mmt_mfu_context->matching_lls_sls_mmt_session) {
        service_id = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->service_id;
    }

    bool is_video_packet = ATSC3_MP_TABLE_IS_VIDEO_ASSET_TYPE_ANY(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type);

    bool is_audio_packet = (strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_AC_4_ID, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, 4) == 0) ||
                           (strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MHM1_ID, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, 4) == 0) ||
                           (strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MHM2_ID, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, 4) == 0) ||
                           (strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MP4A_ID, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, 4) == 0);

    bool is_stpp_packet =  (strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_IMSC1_ID, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, 4) == 0);

    _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG("atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk: service_id: %d, packet_id: %d, is_video_packet: %d, is_audio_packet: %d, is_stpp_packet: %d", service_id, packet_id, is_video_packet, is_audio_packet, is_stpp_packet);

    if(is_video_packet) {
        //manually extract our csd and NALs here
        atsc3_video_decoder_configuration_record_t* atsc3_video_decoder_configuration_record = atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t(mmt_mpu_metadata);

        //we will get either avc1 (avcC) NAL or hevc (hvcC) nals back
        if (atsc3_video_decoder_configuration_record) {

            atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record = atsc3_video_decoder_configuration_record;

            //set width/height to player
            if (atsc3_video_decoder_configuration_record->width && atsc3_video_decoder_configuration_record->height) {
                Atsc3NdkMediaMMTBridge_ptr->atsc3_setVideoWidthHeightFromTrak(packet_id, atsc3_video_decoder_configuration_record->width, atsc3_video_decoder_configuration_record->height);
            }

            if (atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record && atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record->hevc_nals_combined) {

                if (atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record->hevc_nals_combined->p_size) {
                    Atsc3NdkMediaMMTBridge_ptr->atsc3_onInitHEVC_NAL_Extracted(service_id, packet_id, mpu_sequence_number, block_Get(atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record->hevc_nals_combined), atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record->hevc_nals_combined->p_size);
                } else {
                    Atsc3NdkMediaMMTBridge_ptr->LogMsg("atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk - error, no NALs returned!");
                }
            }
        }
    } else if(is_audio_packet){
        atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record = atsc3_audio_decoder_configuration_record_parse_from_asset_type_and_block_t(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, mmt_mpu_metadata);
        if(atsc3_audio_decoder_configuration_record) {
            atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record = atsc3_audio_decoder_configuration_record;
            Atsc3NdkMediaMMTBridge_ptr->atsc3_onInitAudioDecoderConfigurationRecord(service_id, packet_id, mpu_sequence_number, atsc3_audio_decoder_configuration_record);
        }
    } else if(is_stpp_packet) {
        //jjustman-2020-12-17 - TODO - impl STPP callback
    }
}

void atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG("atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_ndk: packet_id: %d, asset_type: %.4s, mp_table_asset_row: %p",
                video_packet_id, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row);

    Atsc3NdkMediaMMTBridge_ptr->atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(video_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

void atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG("atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_ndk: packet_id: %d, asset_type: %.4s, mp_table_asset_row: %p",
                                                      audio_packet_id, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row);

    Atsc3NdkMediaMMTBridge_ptr->atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(audio_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

void atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG("atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_ndk: packet_id: %d, asset_type: %.4s, mp_table_asset_row: %p",
                                                      stpp_packet_id, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row);

    Atsc3NdkMediaMMTBridge_ptr->atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(stpp_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

//cant process MFU's without the NAL... we should ALWAYS listen for at least mpu metadata
//in as many MMT flows as possible
void atsc3_mmt_mpu_mfu_on_sample_complete_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_rebuilt) {

    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_context->get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential(atsc3_mmt_mfu_context, packet_id, mmtp_timestamp, mpu_sequence_number, sample_number);

    uint64_t mpu_timestamp_descriptor = 0;
    if(atsc3_mmt_mfu_mpu_timestamp_descriptor) {
        mpu_timestamp_descriptor = atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value;
    } else {
        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_WARN("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: mmt_mfu_sample: %p: returned null atsc3_mmt_mfu_mpu_timestamp_descriptor for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
    }

    uint16_t service_id = 0;
    if (atsc3_mmt_mfu_context->matching_lls_sls_mmt_session) {
        service_id = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->service_id;
    }

    if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record &&
        atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record &&
        atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record->hevc_nals_combined) {

        //get our raw byte sequence payload, android medicacodec decoder for HEVC needs this...
        block_t* mmt_mfu_sample_rbsp = atsc3_hevc_extract_mp4toannexb_filter_ffmpegImpl(mmt_mfu_sample, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record->hevc_nals_combined);

        block_Rewind(mmt_mfu_sample_rbsp);
        uint8_t* block_ptr = block_Get(mmt_mfu_sample_rbsp);
        uint32_t block_len = block_Len(mmt_mfu_sample_rbsp);

        if ((global_mfu_proccessed_count++ % 600) == 0) {
            Atsc3NdkMediaMMTBridge_ptr->LogMsgF("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: hevc_nals: global mfu count: %d, packet_id: %d, mpu: %d, sample: %d, orig len: %d, rbsp len: %d, mpu_timestamp_descriptor: %lu",
                                                global_mfu_proccessed_count,
                                                packet_id, mpu_sequence_number, sample_number,
                                                block_Len(mmt_mfu_sample), block_len,
                                                mpu_timestamp_descriptor);

        }

        Atsc3NdkMediaMMTBridge_ptr->atsc3_onMfuPacket(service_id, packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_rebuilt);

        block_Destroy(&mmt_mfu_sample_rbsp);

    } else {
        //audio and stpp don't need NAL start codes
        block_Rewind(mmt_mfu_sample);
        uint8_t *block_ptr = block_Get(mmt_mfu_sample);
        uint32_t block_len = block_Len(mmt_mfu_sample);

        if ((global_mfu_proccessed_count++ % 600) == 0) {
            Atsc3NdkMediaMMTBridge_ptr->LogMsgF("atsc3_mmt_mpu_mfu_on_sample_complete_ndk: non-nal mfu: global mfu count: %d, packet_id: %d, mpu: %d, sample: %d, len: %d, mpu_timestamp_descriptor: %lu",
                                            global_mfu_proccessed_count,
                                            packet_id, mpu_sequence_number, sample_number,
                                            block_len,
                                            mpu_timestamp_descriptor);
        }

        Atsc3NdkMediaMMTBridge_ptr->atsc3_onMfuPacket(service_id, packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_rebuilt);
    }
}

void atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {

    //cant process MFU's without the NAL... we should ALWAYS listen for at least mpu metadata
    //in as many MMT flows as possible

    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_context->get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential(atsc3_mmt_mfu_context, packet_id, mmtp_timestamp, mpu_sequence_number, sample_number);
    uint64_t mpu_timestamp_descriptor = 0;
    if(atsc3_mmt_mfu_mpu_timestamp_descriptor) {
        mpu_timestamp_descriptor = atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value;
    } else {
        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_WARN("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: mmt_mfu_sample: %p: returned null atsc3_mmt_mfu_mpu_timestamp_descriptor for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
    }

    if(block_Len(mmt_mfu_sample) < 32 ) {
        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_WARN("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: mmt_mfu_sample: %p: block len is < 32 for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
        return;
    }

    uint16_t service_id = 0;
    if (atsc3_mmt_mfu_context->matching_lls_sls_mmt_session) {
        service_id = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->service_id;
    }

    if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record &&
        atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record &&
        atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record->hevc_nals_combined) {

        block_t *mmt_mfu_sample_rbsp = atsc3_hevc_extract_mp4toannexb_filter_ffmpegImpl(mmt_mfu_sample, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record->hevc_nals_combined);
        if(mmt_mfu_sample_rbsp && block_Len(mmt_mfu_sample_rbsp)) {
            block_Rewind(mmt_mfu_sample_rbsp);
            uint8_t *block_ptr = block_Get(mmt_mfu_sample_rbsp);
            uint32_t block_len = block_Len(mmt_mfu_sample_rbsp);


            //if((global_mfu_proccessed_count++ % 600) == 0) {
            _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_INFO("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: total mfu count: %d, packet_id: %d, mpu: %d, sample: %d, sample ptr: %p, orig len: %d (i_pos: %d, p_size: %d), nal ptr: %p (p_buffer: %p), len: %d (i_pos: %d, p_size: %d)",
                                                    global_mfu_proccessed_count,
                                                    packet_id,
                                                    mpu_sequence_number,
                                                    sample_number,
                                                    mmt_mfu_sample, mmt_mfu_sample->i_pos, mmt_mfu_sample->p_size,
                                                    block_Len(mmt_mfu_sample),
                                                    block_ptr,
                                                    mmt_mfu_sample_rbsp->p_buffer,
                                                    block_len,
                                                    mmt_mfu_sample_rbsp->i_pos,
                                                    mmt_mfu_sample_rbsp->p_size);

            //}

            Atsc3NdkMediaMMTBridge_ptr->atsc3_onMfuPacketCorrupt(service_id, packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);

            block_Destroy(&mmt_mfu_sample_rbsp);
        } else {
            _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_ERROR("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: mmt_mfu_sample: %p (len: %d) - returned null mmt_mfu_sample_rbsp!", mmt_mfu_sample, mmt_mfu_sample ? mmt_mfu_sample->p_size : -1);
        }
    } else {
        block_Rewind(mmt_mfu_sample);
        uint8_t *block_ptr = block_Get(mmt_mfu_sample);
        uint32_t block_len = block_Len(mmt_mfu_sample);

        //audio and stpp don't need NAL start codes
        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_INFO("atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk: non NAL, packet_id: %d, mpu_sequence_number: %d, sample_number: %d, block_ptr: %p (p_buffer: %p), len: %d, char: %c %c %c %c",
                                                packet_id, mpu_sequence_number, sample_number,
                                                block_ptr,
                                                mmt_mfu_sample->p_buffer,
                                                block_len, block_ptr[0], block_ptr[1], block_ptr[2], block_ptr[3]);

        Atsc3NdkMediaMMTBridge_ptr->atsc3_onMfuPacketCorrupt(service_id, packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    }
}


void atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = NULL;

    if(!mmt_mfu_sample || !mmt_mfu_sample->p_size) {
        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_WARN("atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk: mmt_mfu_sample: %p has no data!", mmt_mfu_sample);
        return;
    }

    atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_context->get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential(atsc3_mmt_mfu_context, packet_id, mmtp_timestamp, mpu_sequence_number, sample_number);
    uint64_t mpu_timestamp_descriptor = 0;
    if(atsc3_mmt_mfu_mpu_timestamp_descriptor) {
        mpu_timestamp_descriptor = atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value;
    } else {
        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_WARN("atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk: mmt_mfu_sample: %p: returned null atsc3_mmt_mfu_mpu_timestamp_descriptor for packet_id: %d, mpu_sequence_number: %d", mmt_mfu_sample, packet_id, mpu_sequence_number);
    }
    //TODO: jjustman-2019-10-23: determine if we can still extract NAL's from this payload...

    uint16_t service_id = 0;
    if (atsc3_mmt_mfu_context->matching_lls_sls_mmt_session) {
        service_id = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->service_id;
    }

    if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record &&
       atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record &&
       atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record->hevc_nals_combined) {
            block_t *mmt_mfu_sample_rbsp = atsc3_hevc_extract_mp4toannexb_filter_ffmpegImpl(mmt_mfu_sample, atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record->hevc_nals_combined);
            if(mmt_mfu_sample_rbsp) {

                block_Rewind(mmt_mfu_sample_rbsp);
                uint8_t *block_ptr = block_Get(mmt_mfu_sample_rbsp);
                uint32_t block_len = block_Len(mmt_mfu_sample_rbsp);

                //if((global_mfu_proccessed_count++ % 600) == 0) {
                _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_INFO("atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk: total mfu count: %d, packet_id: %d, mpu: %d, sample: %d, sample ptr: %p, orig len: %d (i_pos: %d, p_size: %d), nal ptr: %p, len: %d (i_pos: %d, p_size: %d)",
                                                    global_mfu_proccessed_count,
                                                    packet_id,
                                                    mpu_sequence_number,
                                                    sample_number,
                                                    mmt_mfu_sample,
                                                    block_Len(mmt_mfu_sample), mmt_mfu_sample->i_pos,  mmt_mfu_sample->p_size,
                                                    block_ptr,
                                                    block_len,
                                                    mmt_mfu_sample_rbsp->i_pos,
                                                    mmt_mfu_sample_rbsp->p_size);

            Atsc3NdkMediaMMTBridge_ptr->atsc3_onMfuPacketCorruptMmthSampleHeader(service_id, packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);

            block_Destroy(&mmt_mfu_sample_rbsp);
        }
    } else {
        if (mmt_mfu_sample) {
            block_Rewind(mmt_mfu_sample);

            uint8_t *block_ptr = block_Get(mmt_mfu_sample);
            uint32_t block_len = block_Len(mmt_mfu_sample);

            _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_INFO("atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk: non NAL, packet_id: %d, mpu_sequence_number: %d, sample: %d, block: %p, len: %d, char: %c %c %c %c",
                                                    packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, block_ptr[0], block_ptr[1], block_ptr[2], block_ptr[3]);

            //audio and stpp don't need NAL start codes
            Atsc3NdkMediaMMTBridge_ptr->atsc3_onMfuPacketCorruptMmthSampleHeader(service_id, packet_id, mpu_sequence_number, sample_number, block_ptr, block_len, mpu_timestamp_descriptor, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
        } else {
            _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_ERROR("atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk: non NAL, packet_id: %d, mpu_sequence_number: %d, sample: %d - block is NULL!",
                                                     packet_id, mpu_sequence_number, sample_number);

        }
    }
}

void atsc3_mmt_mpu_mfu_on_sample_missing_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
    Atsc3NdkMediaMMTBridge_ptr->atsc3_onMfuSampleMissing(packet_id, mpu_sequence_number, sample_number);
}

void atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_movie_fragment_metadata) {
    uint32_t decoder_configuration_timebase = 1000000; //set as default to uS
    uint32_t extracted_sample_duration_us = 0;

    if(!mmt_movie_fragment_metadata || !mmt_movie_fragment_metadata->p_size) {
        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_WARN("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk: packet_id: %d, mpu_sequence_number: %d, mmt_movie_fragment_metadata: %p: returned null or no length!",
                                                packet_id, mpu_sequence_number, mmt_movie_fragment_metadata);
        return;
    }

    _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk: packet_id: %d, mpu_sequence_number: %d, asset_type: %s, atsc3_video_decoder_configuration_record: %p, atsc3_audio_decoder_configuration_record: %p, atsc3_stpp_decoder_configuration_record: %p",
                                                       packet_id,
                                                       mpu_sequence_number,
                                                       atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type,
                                                       atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record,
                                                       atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record,
                                                       atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record)

    if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record && atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->timebase) {
        decoder_configuration_timebase = atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->timebase;
    } else if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record && atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record->timebase) {
        decoder_configuration_timebase = atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record->timebase;
    } else if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record && atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record->timebase) {
        decoder_configuration_timebase = atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record->timebase;
    } else {
        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_WARN("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk: packet_id: %d, mpu_sequence_number: %d, mmt_movie_fragment_metadata: %p: using default decoder timebase of: %u",
                                                          packet_id, mpu_sequence_number, mmt_movie_fragment_metadata, decoder_configuration_timebase);
    }


    extracted_sample_duration_us = atsc3_mmt_movie_fragment_extract_sample_duration_us(mmt_movie_fragment_metadata, decoder_configuration_timebase);

    if(!extracted_sample_duration_us) {
        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_WARN("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk: packet_id: %d, mpu_sequence_number: %d, mmt_movie_fragment_metadata: %p, computed extracted_sample_duration_us was 0!",
                                                packet_id, mpu_sequence_number, mmt_movie_fragment_metadata);
        return;
    }

    //jjustman-2021-10-05 - ado yoga #16967 - fixup to keep track of our extracted sample duration for mpu_presentation_timestamp descriptor recovery in atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential
    if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record) {
        atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->sample_duration_us = extracted_sample_duration_us;
    } else if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record) {
        atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record->sample_duration_us = extracted_sample_duration_us;
    } else if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record) {
        atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record->sample_duration_us = extracted_sample_duration_us;
    }

    Atsc3NdkMediaMMTBridge_ptr->atsc3_onExtractedSampleDuration(packet_id, mpu_sequence_number, extracted_sample_duration_us);
}

void mmt_mpu_mfu_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_detected_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number) {
    //jjustman-2021-10-21 - hack

    uint16_t service_id = 0;
    if (atsc3_mmt_mfu_context && atsc3_mmt_mfu_context->matching_lls_sls_mmt_session) {
        service_id = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session->service_id;
    }

    Atsc3NdkMediaMMTBridge_ptr->atsc3_notify_sl_hdr_1_present(service_id, packet_id, mmtp_timestamp, mpu_sequence_number, sample_number);
}

void atsc3_mmt_signalling_information_on_userservicedescription_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_signalling_information_usbd_component_t* mmt_atsc3_usbd_message) {
    //vmatiash-2021-28-09 - TODO - impl JNI callback
}

void mmt_atsc3_message_content_type_video_stream_properties_descriptor_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_video_stream_properties_descriptor_t* mmt_atsc3_video_stream_properties_descriptor_message){
    for(int i=0; i < mmt_atsc3_video_stream_properties_descriptor_message->descriptor_header.number_of_assets; i++) {
        mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset_t* mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset = mmt_atsc3_video_stream_properties_descriptor_message->mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset_v.data[i];

        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG(" asset: %d: asset_id_length: %d, asset_id: %s, vCodec: %s",
                                i,
                                mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset->asset_header.asset_id_length,
                                mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset->asset_header.asset_id,
                                mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset->codec_code);

    }

    Atsc3NdkMediaMMTBridge_ptr->atsc3_onVideoStreamProperties(mmt_atsc3_video_stream_properties_descriptor_message);
}

void atsc3_mmt_signalling_information_on_caption_asset_descriptor_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_caption_asset_descriptor_t* mmt_atsc3_caption_asset_descriptor_message){
    for(int i=0; i < mmt_atsc3_caption_asset_descriptor_message->descriptor_header.number_of_assets; i++) {
        mmt_atsc3_message_content_type_caption_asset_descriptor_asset_t* mmt_atsc3_message_content_type_caption_asset_descriptor_asset = mmt_atsc3_caption_asset_descriptor_message->mmt_atsc3_message_content_type_caption_asset_descriptor_asset_v.data[i];

        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG(" asset: %d: asset_id_length: %d, asset_id: %s, language_length: %d, language: %s",
                                i,
                                mmt_atsc3_message_content_type_caption_asset_descriptor_asset->asset_header.asset_id_length,
                                mmt_atsc3_message_content_type_caption_asset_descriptor_asset->asset_header.asset_id,
                                mmt_atsc3_message_content_type_caption_asset_descriptor_asset->language_header.language_length,
                                mmt_atsc3_message_content_type_caption_asset_descriptor_asset->language_header.language);
    }

    Atsc3NdkMediaMMTBridge_ptr->atsc3_onCaptionAssetProperties(mmt_atsc3_caption_asset_descriptor_message);
}

void atsc3_mmt_signalling_information_on_audio_stream_properties_descriptor_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_audio_stream_properties_descriptor_t* mmt_atsc3_audio_stream_properties_descriptor_message){
    for(int i=0; i < mmt_atsc3_audio_stream_properties_descriptor_message->descriptor_header.number_of_assets; i++) {
        mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_t* mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset = mmt_atsc3_audio_stream_properties_descriptor_message->mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_v.data[i];

        _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG(" asset: %d: asset_id_length: %d, asset_id: %s",
                                i,
                                mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset->asset_header.asset_id_length,
                                mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset->asset_header.asset_id);

        for(int j=0; j < mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset->num_presentations; j++) {
            mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_t* mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation = mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset->mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_v.data[j];

            if(mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->language_present) {
                //remember, we are _minus1
                _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG("\tlanguage present, num_languages: %d", mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->num_languages_minus1 + 1);

                for(int k=0; k < mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->num_languages_minus1 + 1; k++) {
                    mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_language_t* mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_language = mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_language_v.data[k];

                    _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG("\t\tlanguage length: %d, language: %s",
                                            mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_language->language_length, mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_language->language);
                }
            }

            if(mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->label_present) {
                _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG("\tlabel present, length: %d, value: %s",
                                        mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->label_length, mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->label_data_byte);

            }
        }
    }

    Atsc3NdkMediaMMTBridge_ptr->atsc3_onAudioStreamProperties(mmt_atsc3_audio_stream_properties_descriptor_message);
}

void atsc3_mmt_signalling_information_on_security_properties_descriptor_LAURL_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_t* mmt_atsc3_security_properties_descriptor_LAURL_message){
    //vmatiash-2021-28-09 - TODO - impl JNI callback
}

void atsc3_mmt_signalling_information_on_mp_table_complete_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mp_table_t* mp_table) {
    if(mp_table->number_of_assets) {
        Atsc3NdkMediaMMTBridge_ptr->atsc3_onMpTableComplete(mp_table);
    }
}

atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_callbacks_default_jni_new() {
    atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_internal_flows_new();

    //jjustman-2021-10-21 - sl_hdr_1 ndk callback
    atsc3_mmt_mfu_context->external_sei.mmt_mpu_mfu_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_detected = &mmt_mpu_mfu_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_detected_ndk;
    //wire up atsc3_mmt_mpu_on_sequence_mpu_metadata_present to parse out our NALs as needed for android MediaCodec init
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present = &atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk;

    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete = &atsc3_mmt_mpu_mfu_on_sample_complete_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt = &atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk;

    //todo: search thru NAL's as needed here and discard anything that intra-NAL..
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header = &atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_missing = &atsc3_mmt_mpu_mfu_on_sample_missing_ndk;

    /*
     * TODO: jjustman-2019-10-20 - extend context callback interface with service_id
     */
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_ndk;

    //extract out one trun sampleduration for essence timing
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present = &atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk;

    //jjustman-2020-12-08 - external users will also need to wire up atsc3_mmt_signalling_information_on_routecomponent_message_present and atsc3_mmt_signalling_information_on_held_message_present
    //vmatiash-2021-28-09 - TODO - impl atsc3_mmt_signalling_information_on_userservicedescription_present and atsc3_mmt_signalling_information_on_security_properties_descriptor_LAURL_present_ndk

    //atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_userservicedescription_present = &atsc3_mmt_signalling_information_on_userservicedescription_present_ndk;
    atsc3_mmt_mfu_context->mmt_atsc3_message_content_type_video_stream_properties_descriptor_present = &mmt_atsc3_message_content_type_video_stream_properties_descriptor_present_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_caption_asset_descriptor_present = &atsc3_mmt_signalling_information_on_caption_asset_descriptor_present_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_stream_properties_descriptor_present = &atsc3_mmt_signalling_information_on_audio_stream_properties_descriptor_present_ndk;
    //atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_security_properties_descriptor_LAURL_present = &atsc3_mmt_signalling_information_on_security_properties_descriptor_LAURL_present_ndk;

    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_complete = &atsc3_mmt_signalling_information_on_mp_table_complete_ndk;

    return atsc3_mmt_mfu_context;
}

