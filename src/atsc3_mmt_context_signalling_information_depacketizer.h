/*
 * ATSC3_MMT_CONTEXT_SIGNALLING_INFORMATION_DEPACKETIZER_H.h
 *
 *  Created on: Oct 3, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_CONTEXT_SIGNALLING_INFORMATION_DEPACKETIZER_H
#define ATSC3_MMT_CONTEXT_SIGNALLING_INFORMATION_DEPACKETIZER_H

#include "atsc3_utils.h"
#include "atsc3_packet_statistics.h"
#include "atsc3_mmtp_parser.h"

#include "atsc3_mmt_mpu_utils.h"
#include "atsc3_lls_sls_monitor_output_buffer.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"
#include "atsc3_isobmff_tools.h"
#include "atsc3_mmt_signalling_message.h"
#include "atsc3_mmtp_packet_types.h"
#include "atsc3_mmtp_packet_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

//INTERNAL event callback typedefs

typedef struct atsc3_mmt_mfu_context atsc3_mmt_mfu_context_t;
typedef struct atsc3_mmt_mfu_mpu_timestamp_descriptor atsc3_mmt_mfu_mpu_timestamp_descriptor_t;

typedef void (*__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_f)(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
typedef atsc3_mmt_mfu_mpu_timestamp_descriptor_t* (*atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_f)(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number);

//todo- jjustman-2019-10-03: add in flow for un-filtered callback listeners

//EXTERNAL event callback typedefs
typedef void (*atsc3_mmt_signalling_information_on_mp_table_subset_f)(mp_table_t* mp_table);
typedef void (*atsc3_mmt_signalling_information_on_mp_table_complete_f)(mp_table_t* mp_table);

typedef void (*atsc3_mmt_signalling_information_on_audio_essence_packet_id_f)(uint16_t audio_packet_id);
typedef void (*atsc3_mmt_signalling_information_on_video_essence_packet_id_f)(uint16_t video_packet_id);
typedef void (*atsc3_mmt_signalling_information_on_stpp_essence_packet_id_f) (uint16_t stpp_packet_id);

typedef void (*atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_f)(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
typedef void (*atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_f)(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
typedef void (*atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_f) (uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);


typedef void (*atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_f)(uint16_t packet_id, uint32_t mpu_sequence_number, mmt_signalling_message_mpu_timestamp_descriptor_t* mmt_signalling_message_mpu_timestamp_descriptor);

//we will return mmtp_mpu_packet if it was successfully persisted, otherwise it will be null'd out
//mmtp_mpu_packet_t* mmtp_process_from_payload(mmtp_mpu_packet_t* mmtp_mpu_packet,
//                               mmtp_flow_t *mmtp_flow,
//                               lls_slt_monitor_t* lls_slt_monitor,
//                               udp_packet_t *udp_packet,
//                               udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,
//                               lls_sls_mmt_session_t* matching_lls_slt_mmt_session);
//
//void atsc3_mmt_reconstitution_free_from_udp_flow(mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, udp_flow_t* udp_flow, udp_flow_packet_id_mpu_sequence_tuple_t* last_udp_flow_packet_id_mpu_sequence_tuple);

#ifdef __cplusplus
}
#endif

#endif /* ATSC3_MMT_CONTEXT_SIGNALLING_INFORMATION_DEPACKETIZER_H */
