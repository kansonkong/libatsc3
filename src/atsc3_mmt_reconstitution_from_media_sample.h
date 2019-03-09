/*
 * atsc3_mmt_reconstitution_from_media_sample.h
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_RECONSTITUTION_FROM_MEDIA_SAMPLE_H_
#define ATSC3_MMT_RECONSTITUTION_FROM_MEDIA_SAMPLE_H_

#include "atsc3_utils.h"
#include "atsc3_packet_statistics.h"
#include "atsc3_mmtp_types.h"
#include "atsc3_mmtp_parser.h"

#include "atsc3_mmt_signaling_message.h"
#include "atsc3_mmt_mpu_utils.h"
#include "atsc3_lls_sls_monitor_output_buffer.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"
#include "atsc3_isobmff_tools.h"
#ifdef __cplusplus
extern "C" {
#endif
extern global_atsc3_stats_t* global_stats;

mmtp_payload_fragments_union_t* mmtp_process_from_payload(mmtp_sub_flow_vector_t* mmtp_sub_flow_vector,
		udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,
		lls_slt_monitor_t* lls_slt_monitor,
		udp_packet_t *udp_packet,
		mmtp_payload_fragments_union_t** mmtp_payload_p,
		lls_sls_mmt_session_t* matching_lls_slt_mmt_session);


#ifdef __cplusplus
}
#endif
#endif /* ATSC3_MMT_RECONSTITUTION_FROM_MEDIA_SAMPLE_H_ */
