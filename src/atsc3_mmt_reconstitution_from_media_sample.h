/*
 * atsc3_mmt_reconstitution_from_media_sample.h
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_RECONSTITUTION_FROM_MEDIA_SAMPLE_H_
#define ATSC3_MMT_RECONSTITUTION_FROM_MEDIA_SAMPLE_H_

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"

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

//jjustman-2019-08-30 - TODO - refactor me
extern atsc3_global_statistics_t* atsc3_global_statistics;
    
//mmtp_sub_flow_vector_t* mmtp_sub_flow_vector,
//mmtp_payload_fragments_union_t** mmtp_payload_p,
    //        udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,

//we will return mmtp_mpu_packet if it was successfully persisted, otherwise it will be null'd out
mmtp_mpu_packet_t* mmtp_process_from_payload(mmtp_mpu_packet_t* mmtp_mpu_packet,
                               mmtp_flow_t *mmtp_flow,
                               lls_slt_monitor_t* lls_slt_monitor,
                               udp_packet_t *udp_packet,
                               udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,
                               lls_sls_mmt_session_t* matching_lls_slt_mmt_session);

//void atsc3_mmt_reconstitution_free_from_udp_flow(mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, udp_flow_t* udp_flow, udp_flow_packet_id_mpu_sequence_tuple_t* last_udp_flow_packet_id_mpu_sequence_tuple);

#define __MMT_RECON_FROM_SAMPLE_ERROR(...)   		__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __MMT_RECON_FROM_SAMPLE_WARN(...)   		__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMT_RECON_FROM_SAMPLE_INFO(...)    		__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define __MMT_RECON_FROM_SAMPLE_SIGNAL_INFO(...)    if(_MMT_RECON_FROM_SAMPLE_SIGNAL_INFO_ENABLED) { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __MMT_RECON_FROM_SAMPLE_DEBUG(...)   		if(_MMT_RECON_FROM_SAMPLE_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); };
#define __MMT_RECON_FROM_SAMPLE_TRACE(...)  		if(_MMT_RECON_FROM_SAMPLE_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); };

#ifdef __cplusplus
}
#endif
    

#endif /* ATSC3_MMT_RECONSTITUTION_FROM_MEDIA_SAMPLE_H_ */
