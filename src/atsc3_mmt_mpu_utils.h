/*
 * atsc3_mmt_mpu_utils.h
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */
#include <stdbool.h>
#include "atsc3_listener_udp.h"

#include "atsc3_lls_types.h"
#include "atsc3_mmtp_types.h"
#include "atsc3_mmtp_parser.h"
#include "atsc3_mmt_mpu_parser.h"
#include "atsc3_player_ffplay.h"

#ifndef ATSC3_MMT_MPU_UTILS_H_
#define ATSC3_MMT_MPU_UTILS_H_

extern int _MMT_MPU_DEBUG_ENABLED;

/**
 *
 * build our last sequence references from the following two flows:
 *
 * 	<dip, dport> - for LLS flows
 * 				:packet_id, mpu_sequence_number>
 *
 *
 *
 *  2019-02-20 - in general, the tuple of <dst_ip, dst_port, dst_packet_id> should have mpu_sequence_numbers that increment by one and only one for every completed MPU.
 *  In cases of packet loss or retransmission, we may see larger gaps that may break this n < n+1 model.
 *
 *  Additionally, running MMT loops will cause the mpu_sequence_numbers to loop around and cause a failure of evictions.
 *  Instead, we will use a discontinuity window of at least a 2 mpu_sequence gap,
 *  	e.g. udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number - mmtp_packet->mmtp_mpu_type_packet_header.mpu_sequence_number >=4,
 *  	with a cumulative count of at least 50 fragments before switching to the "older" mpu_sequence_number
 *
 *  	This should also address the edge case of a mpu_sequence_number rollover from uint32_t to 0, which is allowed
 */

#define __MPU_FLOW_NEGATIVE_DISCONTINUITY_SEQUENCE_GAP_THRESHOLD 2
#define __MPU_FLOW_NEGATIVE_DISCONTINUITY_SEQUENCE_GAP_FRAGMENT_RECV_THRESHOLD 50


#if defined (__cplusplus)
extern "C" {
#endif


/**
 *  uint32_t		src_ip_addr;
	uint32_t		dst_ip_addr
	uint16_t		src_port;
	uint16_t		dst_port;
 */

#define udp_flow_match(a, b)  ((a->udp_flow.dst_ip_addr == b->udp_flow.dst_ip_addr) && (a->udp_flow.dst_port == b->udp_flow.dst_port))
#define udp_flow_match_from_udp_flow_t(a, b)  ((a->udp_flow.dst_ip_addr == b->dst_ip_addr) && (a->udp_flow.dst_port == b->dst_port))
#define udp_flow_and_packet_id_match(a, b) (udp_flow_match(a, b) && a->packet_id == b->packet_id)


//i miss stl containers...

udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container_t_init();
udp_flow_latest_mpu_sequence_number_container_t* udp_flow_find_matching_flows(udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, udp_flow_t* udp_flow_to_search);
void udp_flow_latest_mpu_sequence_number_container_t_release(udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container);
udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_latest_mpu_sequence_number_from_packet_id(udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, udp_packet_t* udp_packet, uint32_t packet_id);
udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_latest_mpu_sequence_number_add_or_replace(udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, udp_packet_t* udp_packet, mmtp_payload_fragments_union_t* mmtp_packet);

void udp_flow_force_negative_mpu_discontinuity_value(udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_matching_pkt_id, uint32_t new_old_mpu_sequence_number_to_force, mmtp_payload_fragments_union_t* mmtp_packet_fragments_to_evict);
void udp_flow_reset_negative_mpu_discontinuity_counters(udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_matching_pkt_id);

int atsc3_mmt_mpu_clear_data_unit_from_packet_subflow(mmtp_payload_fragments_union_t* mmtp_payload_fragments_union, uint32_t evict_range_start, uint32_t evict_range_end);
int atsc3_mmt_mpu_clear_data_unit_payload_fragments(mmtp_sub_flow_t* mmtp_sub_flow, mpu_fragments_t* mpu_fragments, mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments);

void mpu_dump_header(mmtp_payload_fragments_union_t* mmtp_payload);
void mpu_dump_flow(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload);
void mpu_dump_reconstitued(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload);
void mpu_push_to_output_buffer(pipe_ffplay_buffer_t* ffplay_buffer, mmtp_payload_fragments_union_t* mmtp_payload);

void mpu_push_to_output_buffer_no_locking(pipe_ffplay_buffer_t* ffplay_buffer, mmtp_payload_fragments_union_t* mmtp_payload);

void mpu_fragments_vector_shrink_to_fit(mpu_fragments_t* mpu_fragments);

udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple_clone(udp_flow_packet_id_mpu_sequence_tuple_t* from_udp_flow_packet_id_mpu_sequence_tuple);

void udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(udp_flow_packet_id_mpu_sequence_tuple_t** to_udp_flow_packet_id_mpu_sequence_tuple_p, udp_flow_packet_id_mpu_sequence_tuple_t* from_udp_flow_packet_id_mpu_sequence_tuple);

    
#if defined (__cplusplus)
}
#endif


#define __MMT_MPU_ERROR(...)   printf("%s:%d:ERROR :",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\r","\n");
#define __MMT_MPU_WARN(...)    printf("%s:%d:WARN: ",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\r","\n");
#define __MMT_MPU_INFO(...)    printf("%s:%d: ",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\r","\n");
#define __MMT_MPU_DEBUG(...)   if(_MMT_MPU_DEBUG_ENABLED) {printf("%s:%d:DEBUG: ",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\r","\n"); }

#endif /* ATSC3_MMT_MPU_UTILS_H_ */
