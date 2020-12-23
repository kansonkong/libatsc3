//
// Created by Jason Justman on 12/1/20.
//

#include "atsc3_mmt_context_signalling_information_callbacks_internal.h"

void atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_callback_internal(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    while(atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count > 10) {
        atsc3_mmt_mfu_mpu_timestamp_descriptor_t *atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window_pop_atsc3_mmt_mfu_mpu_timestamp_descriptor(&atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window);

        __MMT_CONTEXT_MPU_TRACE("__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor, removing: packet_id: %d, mpu_sequence_number: %d", atsc3_mmt_mfu_mpu_timestamp_descriptor->packet_id, atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_sequence_number);

        atsc3_mmt_mfu_mpu_timestamp_descriptor_free(&atsc3_mmt_mfu_mpu_timestamp_descriptor);
    }

    __MMT_CONTEXT_MPU_TRACE("__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor, window count: %d, adding: packet_id: %d, mpu_sequence_number: %d", atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count, packet_id, mpu_sequence_number);

    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* matching_atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context, packet_id, mpu_sequence_number);

    if(!matching_atsc3_mmt_mfu_mpu_timestamp_descriptor) {
        atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_mpu_timestamp_descriptor_new();
        atsc3_mmt_mfu_mpu_timestamp_descriptor->packet_id = packet_id;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_sequence_number = mpu_sequence_number;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_microseconds = mpu_presentation_time_microseconds;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_ntp64 = mpu_presentation_time_ntp64;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_seconds = mpu_presentation_time_seconds;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_microseconds = mpu_presentation_time_microseconds;

        //jjustman-2020-11-19 - make sure to coerce our uS scalar (1000000) as long, otherwise our value will be implicity coerced into (uint32_t) instead of uint64_t 	mpu_presentation_time_as_us_value
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value = (uint64_t)mpu_presentation_time_seconds * (uint64_t)1000000L + (uint64_t)mpu_presentation_time_microseconds;

        atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window_add_atsc3_mmt_mfu_mpu_timestamp_descriptor(&atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window, atsc3_mmt_mfu_mpu_timestamp_descriptor);
    }
}

//
////MP_table
//void atsc3_mmt_signalling_information_on_mp_table_subset_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mp_table_t* mp_table) {
//    //noop;
//    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_mp_table_subset_noop: mp_table: %p", mp_table);
//}
//
////MP_table
//void atsc3_mmt_signalling_information_on_mp_table_complete_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mp_table_t* mp_table) {
//    //noop;
//    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_mp_table_complete_noop: mp_table: %p", mp_table);
//}


//audio essence packet id, type and mp_table assigning to our mmtp_packet_id_packets_container
void atsc3_mmt_signalling_information_on_audio_essence_packet_id_callback_internal(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t audio_packet_id, mp_table_asset_row_t* mp_table_asset_row) {
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_audio_essence_packet_id_callback_internal: audio_packet_id: %u, asset_type: %s", audio_packet_id, mp_table_asset_row->asset_type);

    //hack..
    memcpy(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, mp_table_asset_row->asset_type, 4);
    if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row) {
        atsc3_mmt_mp_table_asset_row_free_inner(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row);
        free(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row);
        atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row = NULL;
    }
    atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row = atsc3_mmt_mp_table_asset_row_duplicate(mp_table_asset_row);
}

//video essence packet_id extraction
void atsc3_mmt_signalling_information_on_video_essence_packet_id_callback_internal(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t video_packet_id, mp_table_asset_row_t* mp_table_asset_row) {
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_video_essence_packet_id_callback_internal: video_packet_id: %u, asset_type: %s", video_packet_id, mp_table_asset_row->asset_type);

    //hack..
    memcpy(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, mp_table_asset_row->asset_type, 4);
    if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row) {
        atsc3_mmt_mp_table_asset_row_free_inner(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row);
        free(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row);
        atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row = NULL;
    }
    atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row = atsc3_mmt_mp_table_asset_row_duplicate(mp_table_asset_row);
}

//stpp essence packet_id extraction
void atsc3_mmt_signalling_information_on_stpp_essence_packet_id_callback_internal(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t stpp_packet_id, mp_table_asset_row_t* mp_table_asset_row) {
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_video_essence_packet_id_callback_internal: stpp_packet_id: %u, asset_type: %s", stpp_packet_id, mp_table_asset_row->asset_type);

    //hack..
    memcpy(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->asset_type, mp_table_asset_row->asset_type, 4);
    if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row) {
        atsc3_mmt_mp_table_asset_row_free_inner(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row);
        free(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row);
        atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row = NULL;
    }
    atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->mp_table_asset_row = atsc3_mmt_mp_table_asset_row_duplicate(mp_table_asset_row);
}
