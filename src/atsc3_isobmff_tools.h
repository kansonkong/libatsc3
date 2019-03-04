/*
 * atsc3_isobmff_tools.h
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 */


#if defined (__cplusplus)
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "atsc3_lls_types.h"
#include "atsc3_utils.h"
#include "atsc3_listener_udp.h"
#include "atsc3_mmt_mpu_utils.h"
#include "atsc3_player_ffplay.h"
#include "atsc3_lls_sls_monitor_output_buffer.h"


#if defined (__cplusplus)
}
#endif


#ifndef ATSC3_ISOBMFF_TOOLS_H_
#define ATSC3_ISOBMFF_TOOLS_H_

extern int _ISOBMFF_TOOLS_DEBUG_ENABLED;
extern int _ISOBMFF_TOOLS_TRACE_ENABLED;

#if defined (__cplusplus)
extern "C" {
#endif

lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_isobmff_fragment(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer);

lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_flow(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, lls_sls_mmt_monitor_t* lls_sls_mmt_monitor);
lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_mpu_sequence_numbers(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, uint32_t mpu_sequence_number_audio, uint32_t mpu_sequence_number_video, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, lls_sls_mmt_monitor_t* lls_sls_mmt_monitor);

#if defined (__cplusplus)
}
#endif


#define __ISOBMFF_TOOLS_ERROR(...)   printf("%s:%d:ERROR: %.4f: ",__FILE__,__LINE__, gt());printf(__VA_ARGS__);printf("\n");
#define __ISOBMFF_TOOLS_WARN(...)    printf("%s:%d:WARN : %.4f: ",__FILE__,__LINE__, gt());printf(__VA_ARGS__);printf("\n");
#define __ISOBMFF_TOOLS_INFO(...)    printf("%s:%d:INFO : %.4f: ",__FILE__,__LINE__, gt());printf(__VA_ARGS__);printf("\n");

#define __ISOBMFF_TOOLS_DEBUG(...)   if(_ISOBMFF_TOOLS_DEBUG_ENABLED) {printf("%s:%d:DEBUG: %.4f: ",__FILE__,__LINE__, gt());printf(__VA_ARGS__);printf("\n"); }

#define __ISOBMFF_TOOLS_TRACE(...)   if(_ISOBMFF_TOOLS_TRACE_ENABLED) {printf("%s:%d:TRACE: %.4f: ",__FILE__,__LINE__, gt());printf(__VA_ARGS__);printf("\n"); }

#endif /* ATSC3_ISOBMFF_TOOLS_H_ */
