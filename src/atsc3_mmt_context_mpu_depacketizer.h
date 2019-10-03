/*
 * ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H.h
 *
 *  Created on: Oct 3, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H
#define ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H

#include "atsc3_utils.h"
#include "atsc3_packet_statistics.h"
#include "atsc3_mmtp_parser.h"

#include "atsc3_mmt_mpu_utils.h"
#include "atsc3_lls_sls_monitor_output_buffer.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"
#include "atsc3_isobmff_tools.h"
#include "atsc3_mmtp_packet_types.h"
#include "atsc3_mmtp_packet_utils.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef void (*atsc3_mmt_mpu_on_sequence_number_change_f)(uint16_t packet_id, uint32_t mpu_sequence_number_old, uint32_t mpu_sequence_number_new);
//
////we will return mmtp_mpu_packet if it was successfully persisted, otherwise it will be null'd out
//mmtp_mpu_packet_t* mmtp_process_from_payload(mmtp_mpu_packet_t* mmtp_mpu_packet,
//                               mmtp_flow_t *mmtp_flow,
//                               lls_slt_monitor_t* lls_slt_monitor,
//                               udp_packet_t *udp_packet,
//                               udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,
//                               lls_sls_mmt_session_t* matching_lls_slt_mmt_session);

//void atsc3_mmt_reconstitution_free_from_udp_flow(mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, udp_flow_t* udp_flow, udp_flow_packet_id_mpu_sequence_tuple_t* last_udp_flow_packet_id_mpu_sequence_tuple);

#ifdef __cplusplus
}
#endif
    

#endif /* ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H */
