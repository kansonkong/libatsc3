/*
 * atsc3_isobmff_tools.h
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "atsc3_lls_types.h"
#include "atsc3_utils.h"
#include "atsc3_listener_udp.h"
#include "atsc3_mmt_mpu_utils.h"
#include "atsc3_player_ffplay.h"
#include "atsc3_lls_sls_monitor_output_buffer.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"

#include "atsc3_mmtp_packet_utils.h"

#ifndef ATSC3_ISOBMFF_TOOLS_H_
#define ATSC3_ISOBMFF_TOOLS_H_

#if defined (__cplusplus)
extern "C" {
#endif

extern int _ISOBMFF_TOOLS_DEBUG_ENABLED;
extern int _ISOBMFF_TOOLS_TRACE_ENABLED;
extern int _ISOBMFF_TOOLS_SIGNALLING_DEBUG_ENABLED;

//jdj-2019-04-05
lls_sls_monitor_buffer_isobmff_t* atsc3_isobmff_build_raw_mpu_from_single_sequence_number(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,
		uint16_t packet_id, uint32_t mpu_sequence_number, lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container);

lls_sls_monitor_buffer_isobmff_t* atsc3_isobmff_rebuild_track_mpu_from_sample_data(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff);

lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_alc_isobmff_fragment(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);
lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_mmt_isobmff_fragment(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);

lls_sls_monitor_buffer_isobmff_t* atsc3_isobmff_rebuild_track_mpu_from_sample_data(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff);

lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_mmt_rebuilt_boxes(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);

lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_flow(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, lls_sls_mmt_monitor_t* lls_sls_mmt_monitor);
//lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_mpu_sequence_numbers(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, uint32_t mpu_sequence_number_audio, uint32_t mpu_sequence_number_video, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, lls_sls_mmt_monitor_t* lls_sls_mmt_monitor);


#define __ISOBMFF_TOOLS_ERROR(...)  			__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __ISOBMFF_TOOLS_WARN(...)   			__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __ISOBMFF_TOOLS_INFO(...)    			__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define __ISOBMFF_TOOLS_DEBUG(...)  			if(_ISOBMFF_TOOLS_DEBUG_ENABLED) 			{ __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__) }
#define __ISOBMFF_TOOLS_SIGNALLING_DEBUG(...)  	if(_ISOBMFF_TOOLS_SIGNALLING_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__) }

#define __ISOBMFF_TOOLS_TRACE(...)   			if(_ISOBMFF_TOOLS_TRACE_ENABLED) 			{ __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__) }

#if defined (__cplusplus)
}
#endif
#endif /* ATSC3_ISOBMFF_TOOLS_H_ */
