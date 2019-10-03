/*
 * ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER.h
 *
 *  Created on: Oct 3, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_H
#define ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_H

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"

#include "atsc3_mmtp_packet_types.h"
#include "atsc3_mmt_context_mpu_depacketizer.h"
#include "atsc3_mmt_context_signalling_information_depacketizer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MFU specific callbacks
 * todo - include byte ranges for lost DU's?
 *
 */
typedef void (*atsc3_mmt_mpu_mfu_on_sample_complete_f) (uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample);
typedef void (*atsc3_mmt_mpu_mfu_on_sample_corrupt_f)  (uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample);
typedef void (*atsc3_mmt_mpu_mfu_on_sample_missing_f)  (uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number);

typedef struct atsc3_mmt_mfu_context {
	udp_flow_t* 	udp_flow;

	mmtp_flow_t* 	mmtp_flow;

	udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;
	lls_slt_monitor_t* lls_slt_monitor;
	lls_sls_mmt_session_t* matching_lls_sls_mmt_session;

	mp_table_t* mp_table_last;

	//from ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H
	atsc3_mmt_mpu_on_sequence_number_change_f 						atsc3_mmt_mpu_on_sequence_number_change;

	//from ATSC3_MMT_CONTEXT_SIGNALLING_INFORMATION_DEPACKETIZER_H
	atsc3_mmt_signalling_information_on_mp_table_subset_f 			atsc3_mmt_signalling_information_on_mp_table_subset; 	//dispatched when table_id >= 0x11 (17) && table_id <= 0x19 (31)
	atsc3_mmt_signalling_information_on_mp_table_complete_f 		atsc3_mmt_signalling_information_on_mp_table_complete; 	//dispatched when table_id == 0x20 (32)

	atsc3_mmt_signalling_information_on_video_essence_packet_id_f	atsc3_mmt_signalling_information_on_video_essence_packet_id;
	atsc3_mmt_signalling_information_on_audio_essence_packet_id_f	atsc3_mmt_signalling_information_on_audio_essence_packet_id;

	atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_f		atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor;
	atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_f		atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor;

	atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_f 	atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor;

	//MFU specific callbacks
	atsc3_mmt_mpu_mfu_on_sample_complete_f 							atsc3_mmt_mpu_mfu_on_sample_complete;
	atsc3_mmt_mpu_mfu_on_sample_corrupt_f 							atsc3_mmt_mpu_mfu_on_sample_corrupt;
	atsc3_mmt_mpu_mfu_on_sample_missing_f 							atsc3_mmt_mpu_mfu_on_sample_missing;

} atsc3_mmt_mfu_context_t;


//wire up dummmy null callback(s) to prevent dispatcher from multiple if(..) checks...
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_new();


//Warning: cross boundary processing hooks with callback invocation - impl's in atsc3_mmt_context_mfu_depacketizer.c

void mmtp_mfu_process_from_payload_with_context(udp_packet_t *udp_packet,
												mmtp_mpu_packet_t* mmtp_mpu_packet,
												atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context);

//MMT signalling information processing
void mmt_signalling_message_process_with_context(udp_packet_t *udp_packet,
												mmtp_signalling_packet_t* mmtp_signalling_packet,
												atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context);



#define __MMT_CONTEXT_MPU_WARN(...)   		__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMT_CONTEXT_MPU_INFO(...)    		__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define __MMT_CONTEXT_MPU_SIGNAL_INFO(...)  if(_MMT_CONTEXT_MPU_SIGNAL_INFO_ENABLED) { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __MMT_CONTEXT_MPU_DEBUG(...)   		if(_MMT_CONTEXT_MPU_DEBUG_ENABLED) 		 { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); };
#define __MMT_CONTEXT_MPU_TRACE(...)  		if(_MMT_CONTEXT_MPU_TRACE_ENABLED) 		 { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); };

    
#ifdef __cplusplus
}
#endif
    

#endif /* ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_H */
