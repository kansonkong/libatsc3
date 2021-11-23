//
// Created by Jason Justman on 12/1/20.
//

#include "atsc3_mmt_context_signalling_information_callbacks_internal.h"

void atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_callback_internal(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
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
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mmtp_timestamp = mmtp_timestamp;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_sequence_number = mpu_sequence_number;

        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_ntp64 = mpu_presentation_time_ntp64;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_seconds = mpu_presentation_time_seconds;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_microseconds = mpu_presentation_time_microseconds;

        //jjustman-2020-11-19 - make sure to coerce our uS scalar (1000000) as long, otherwise our value will be implicity coerced into (uint32_t) instead of uint64_t 	mpu_presentation_time_as_us_value
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value = compute_seconds_microseconds_to_scalar64(mpu_presentation_time_seconds, mpu_presentation_time_microseconds);

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

/*
 * jjustman-2021-10-21 - quick hack...
 *
 * sei payload type for user data register == 0x04 (u8)?
 *
 * sl_hdr_info( payloadSize  (u8?)) {
itu_t_t35_country_code b(8)
terminal_provider_code u(16)
terminal_provider_oriented_code_message_idc u(8)
sl_hdr_mode_value_minus1 u(4)
sl_hdr_spec_major_version_idc u(4)
sl_hdr_spec_minor_version_idc u(7)

 looking for the followign pattern:

 04 ** b5 00 3a 00


 */

#define __SEI_PAYLOAD_TYPE_USER_DATA_REGISTER   0x04
#define __SEI_SL_HDR_1_ITU_T_T35_COUNTRY_CODE   0xB5
#define __SEI_SL_HDR_1_TERMINAL_PROVIDER_CODE   0x003A
//note, lower nibble
#define __SEI_SL_HDR_1_SPEC_MAJOR_VERSION_IDC   0x00

bool atsc3_mmt_mpu_mfu_on_sample_complete_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_internal(block_t* mmt_mfu_sample) {
    bool has_found_matching_sei_sl_hdr_1 = false;

    if(mmt_mfu_sample) {
        block_Rewind(mmt_mfu_sample);

        //we need at least 6 bytes to read..
        for(int i=0; !has_found_matching_sei_sl_hdr_1 && i < block_Remaining_size(mmt_mfu_sample) - 6; i++) {
            uint8_t check_sei_payload_type = block_Read_uint8(mmt_mfu_sample);

            if(check_sei_payload_type == __SEI_PAYLOAD_TYPE_USER_DATA_REGISTER) {

                uint8_t     sei_payload_size = block_Read_uint8(mmt_mfu_sample);
                uint8_t     check_itu_t_t35_country_code = block_Read_uint8(mmt_mfu_sample);
                uint16_t    check_terminal_provider_code = block_Read_uint16_ntohs(mmt_mfu_sample);
                uint8_t     sl_hdr_mode_value_minus1_u4_nibble = block_Read_uint8_bitlen(mmt_mfu_sample, 4);
                uint8_t     check_sl_hdr_spec_major_version_idc = block_Read_uint8_bitlen(mmt_mfu_sample, 4);

                __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_complete_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_internal: "
                                        "check_sei_payload_type: 0x%02x, sei_payload_size: 0x%02x, check_itu_t_t35_country_code: 0x%02x, check_terminal_provider_code: 0x%04x, "
                                        "sl_hdr_mode_value_minus1_u4_nibble: 0x%02x, check_sl_hdr_spec_major_version_idc: 0x%02x",
                                        check_sei_payload_type, sei_payload_size, check_itu_t_t35_country_code, check_terminal_provider_code, sl_hdr_mode_value_minus1_u4_nibble, check_sl_hdr_spec_major_version_idc);

                if(check_itu_t_t35_country_code == __SEI_SL_HDR_1_ITU_T_T35_COUNTRY_CODE && check_terminal_provider_code == __SEI_SL_HDR_1_TERMINAL_PROVIDER_CODE && check_sl_hdr_spec_major_version_idc == __SEI_SL_HDR_1_SPEC_MAJOR_VERSION_IDC) {
                    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_complete_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_internal: has_found_matching_sei_sl_hdr_1 at pos: %d", (mmt_mfu_sample->i_pos - 6));
                    has_found_matching_sei_sl_hdr_1 = true;
                    break;
                } else {
                    i += 5;
                }
            }
        }
    }

    return has_found_matching_sei_sl_hdr_1;
}