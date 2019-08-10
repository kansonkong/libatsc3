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
#include "atsc3_mmtp_parser.h"

#include "atsc3_mmt_signaling_message.h"
#include "atsc3_mmt_mpu_utils.h"
#include "atsc3_lls_sls_monitor_output_buffer.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"
#include "atsc3_isobmff_tools.h"
#include "atsc3_mmtp_packet_types.h"
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

void atsc3_mmt_reconstitution_free_from_udp_flow(mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, udp_flow_t* udp_flow, udp_flow_packet_id_mpu_sequence_tuple_t* last_udp_flow_packet_id_mpu_sequence_tuple);

    
extern int _MMT_RECON_FROM_SAMPLE_DEBUG_ENABLED;
extern int _MMT_RECON_FROM_SAMPLE_TRACE_ENABLED;
    
#define __MMT_RECON_FROM_SAMPLE_PRINTLN(...) printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __MMT_RECON_FROM_SAMPLE_ERROR(...)   printf("%s:%d:ERROR :",__FILE__,__LINE__);__MMT_RECON_FROM_SAMPLE_PRINTLN(__VA_ARGS__);
#define __MMT_RECON_FROM_SAMPLE_WARN(...)    printf("%s:%d:WARN :",__FILE__,__LINE__);__MMT_RECON_FROM_SAMPLE_PRINTLN(__VA_ARGS__);
#define __MMT_RECON_FROM_SAMPLE_INFO(...)    printf("%s:%d:INFO :",__FILE__,__LINE__);__MMT_RECON_FROM_SAMPLE_PRINTLN(__VA_ARGS__);
#define __MMT_RECON_FROM_SAMPLE_SIGNAL_INFO(...)    if(_MMT_RECON_FROM_SAMPLE_SIGNAL_INFO_ENABLED) { printf("%s:%d:INFO :",__FILE__,__LINE__);__MMT_RECON_FROM_SAMPLE_PRINTLN(__VA_ARGS__); }
    
#define __MMT_RECON_FROM_SAMPLE_DEBUG(...)   if(_MMT_RECON_FROM_SAMPLE_DEBUG_ENABLED) { printf("%s:%d:DEBUG :",__FILE__,__LINE__);__MMT_RECON_FROM_SAMPLE_PRINTLN(__VA_ARGS__); };
#define __MMT_RECON_FROM_SAMPLE_TRACE(...)   if(_MMT_RECON_FROM_SAMPLE_TRACE_ENABLED) { printf("%s:%d:TRACE :",__FILE__,__LINE__);__MMT_RECON_FROM_SAMPLE_PRINTLN(__VA_ARGS__); };


#ifdef __cplusplus
}
#endif
    

#endif /* ATSC3_MMT_RECONSTITUTION_FROM_MEDIA_SAMPLE_H_ */
