//
// Created by Jason Justman on 12/1/20.
//

#include "atsc3_mmt_context_mfu_depacketizer_callbacks_noop.h"


//MPU
void atsc3_mmt_mpu_on_sequence_number_change_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number_old, uint32_t mpu_sequence_number_new) {
    //noop
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_on_sequence_number_change_noop: packet_id: %u, from %d, to %d", packet_id, mpu_sequence_number_old,  mpu_sequence_number_new);
}

//MPU - init box (mpu_metadata) present
void atsc3_mmt_mpu_on_sequence_mpu_metadata_present_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_mpu_metadata) {
    //noop
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_on_sequence_mpu_metadata_present_noop: packet_id: %u, mpu_sequence_number: %u, mmt_mpu_metadata: %p, size: %d",
                            packet_id,
                            mpu_sequence_number,
                            mmt_mpu_metadata,
                            mmt_mpu_metadata->p_size);

    //todo: extract NALs for example here...
}

//MP_table
void atsc3_mmt_signalling_information_on_mp_table_subset_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mp_table_t* mp_table) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_mp_table_subset_noop: mp_table: %p", mp_table);
}

//MP_table
void atsc3_mmt_signalling_information_on_mp_table_complete_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mp_table_t* mp_table) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_mp_table_complete_noop: mp_table: %p", mp_table);
}

//audio essence packet id extraction
void atsc3_mmt_signalling_information_on_audio_essence_packet_id_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t audio_packet_id, mp_table_asset_row_t* mp_table_asset_row) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_audio_packet_id_noop: audio_packet_id: %u", audio_packet_id);
}

//video essence packet_id extraction
void atsc3_mmt_signalling_information_on_video_essence_packet_id_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t video_packet_id, mp_table_asset_row_t* mp_table_asset_row) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_video_packet_id_noop: video_packet_id: %u", video_packet_id);
}

//stpp essence packet_id extraction
void atsc3_mmt_signalling_information_on_stpp_essence_packet_id_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t stpp_packet_id, mp_table_asset_row_t* mp_table_asset_row) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_stpp_packet_id_noop: stpp_packet_id: %u", stpp_packet_id);
}


//audio packet id extraction with mpu_sequence and mpu_presentation_time
void atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_noop: audio_packet_id: %u, mpu_sequence_number: %u, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
                            audio_packet_id,
                            mpu_sequence_number,
                            mpu_presentation_time_ntp64,
                            mpu_presentation_time_seconds,
                            mpu_presentation_time_microseconds);
}

//video packet_id extraction with mpu_sequence and mpu_presentation_time
void atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_noop: video_packet_id: %u, mpu_sequence_number: %u, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
                            video_packet_id,
                            mpu_sequence_number,
                            mpu_presentation_time_ntp64,
                            mpu_presentation_time_seconds,
                            mpu_presentation_time_microseconds);
}

//stpp packet_id extraction with mpu_sequence and mpu_presentation_time
void atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_noop: stpp_packet_id: %u, mpu_sequence_number: %u, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
                            stpp_packet_id,
                            mpu_sequence_number,
                            mpu_presentation_time_ntp64,
                            mpu_presentation_time_seconds,
                            mpu_presentation_time_microseconds);
}


void atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, mmt_signalling_message_mpu_timestamp_descriptor_t* mmt_signalling_message_mpu_timestamp_descriptor) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_noop: packet_id: %u, mpu_sequence_number: %d, mmt_signalling_message_mpu_timestamp_descriptor: %p",
                            packet_id,
                            mpu_sequence_number,
                            mmt_signalling_message_mpu_timestamp_descriptor);
}

//MFU callbacks

void atsc3_mmt_mpu_mfu_on_sample_complete_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_rebuilt) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_complete_noop: packet_id: %u, mmtp_timestamp: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d, mfu_fragment count rebuilt: %d",
                            packet_id,
                            mmtp_timestamp,
                            mpu_sequence_number,
                            sample_number,
                            mmt_mfu_sample,
                            mmt_mfu_sample->p_size,
                            mfu_fragment_count_rebuilt);
}

void atsc3_mmt_mpu_mfu_on_sample_corrupt_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_corrupt_noop: packet_id: %u, mmtp_timestamp: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d, mfu_fragment count expected: %d, rebuilt %d",
                            packet_id,
                            mmtp_timestamp,
                            mpu_sequence_number,
                            sample_number,
                            mmt_mfu_sample,
                            mmt_mfu_sample->p_size,
                            mfu_fragment_count_expected,
                            mfu_fragment_count_rebuilt);
}



void atsc3_mmt_mpu_mfu_on_sample_missing_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_missing_noop: packet_id: %u, mpu_sequence_number: %u, sample_number: %u",
                            packet_id,
                            mpu_sequence_number,
                            sample_number);
}



void atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample,  uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_noop: packet_id: %u, mmtp_timestamp: %d, mpu_sequence_number: %u, sample_number: %u, mfu_fragment_count_expected: %u, mfu_fragment_count_rebuilt: %u",
                            packet_id,
                            mmtp_timestamp,
                            mpu_sequence_number,
                            sample_number,
                            mfu_fragment_count_expected,
                            mfu_fragment_count_rebuilt);
}

//MPU - init box (mpu_metadata) present
void atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_movie_fragment_metadata) {
    uint32_t decoder_configuration_timebase = 1000000; //set as default to uS
    uint32_t extracted_sample_duration_us = 0;

    if(!mmt_movie_fragment_metadata || !mmt_movie_fragment_metadata->p_size) {
        __MMT_CONTEXT_MPU_WARN("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop: packet_id: %d, mpu_sequence_number: %d, mmt_movie_fragment_metadata: %p: returned null or no length!",
                               packet_id, mpu_sequence_number, mmt_movie_fragment_metadata);
        return;
    }

    if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record && atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->timebase) {
        decoder_configuration_timebase = atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->timebase;
    } else if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record && atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record->timebase) {
        decoder_configuration_timebase = atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record->timebase;
    } else if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record && atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record->timebase) {
        decoder_configuration_timebase = atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record->timebase;
    }

    extracted_sample_duration_us = atsc3_mmt_movie_fragment_extract_sample_duration_us(mmt_movie_fragment_metadata, decoder_configuration_timebase);


    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop: packet_id: %u, mpu_sequence_number: %u, mmt_movie_fragment_metadata: %p, size: %d, extracted extracted_sample_duration_us: %d",
                            packet_id,
                            mpu_sequence_number,
                            mmt_movie_fragment_metadata,
                            mmt_movie_fragment_metadata->p_size,
                            extracted_sample_duration_us);
}


atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_callbacks_noop_new() {
    atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_internal_flows_new();

    //MPU related callbacks
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change 						= &atsc3_mmt_mpu_on_sequence_number_change_noop;
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present               = &atsc3_mmt_mpu_on_sequence_mpu_metadata_present_noop;

    //signalling information callbacks
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_subset			= &atsc3_mmt_signalling_information_on_mp_table_subset_noop;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_complete 		= &atsc3_mmt_signalling_information_on_mp_table_complete_noop;

    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_essence_packet_id 	= &atsc3_mmt_signalling_information_on_audio_essence_packet_id_noop;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_essence_packet_id 	= &atsc3_mmt_signalling_information_on_video_essence_packet_id_noop;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_essence_packet_id 	= &atsc3_mmt_signalling_information_on_stpp_essence_packet_id_noop;

    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_noop;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_noop;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor  = &atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_noop;

    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_noop;

    //MFU related callbacks
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete = &atsc3_mmt_mpu_mfu_on_sample_complete_noop;
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt 	= &atsc3_mmt_mpu_mfu_on_sample_corrupt_noop;
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_missing 	= &atsc3_mmt_mpu_mfu_on_sample_missing_noop;
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header = &atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_noop;

    //movie fragment related callbacks, as a last resort
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present = &atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop;

    return atsc3_mmt_mfu_context;
}
