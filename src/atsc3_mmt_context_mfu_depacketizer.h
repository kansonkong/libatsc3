/*
 * ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER.h
 *
 *  Created on: Oct 3, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_H
#define ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_H

#include "atsc3_utils.h"

#include "atsc3_mmtp_packet_types.h"
#include "atsc3_mmt_context_mpu_depacketizer.h"
#include "atsc3_mmt_context_signalling_information_depacketizer.h"

#include "atsc3_packet_statistics.h"
//#include "atsc3_mmtp_parser.h"
//#include "atsc3_mmt_mpu_utils.h"
//#include "atsc3_lls_sls_monitor_output_buffer.h"
//#include "atsc3_lls_sls_monitor_output_buffer_utils.h"
//#include "atsc3_isobmff_tools.h"
//#include "atsc3_mmt_signalling_message.h"
//#include "atsc3_mmtp_packet_utils.h"


#ifdef __cplusplus
extern "C" {
#endif

//jjustman-2019-08-30 - TODO - refactor me
extern atsc3_global_statistics_t* atsc3_global_statistics;

/*
 * MFU specific callbacks
 * todo - include byte ranges for lost DU's?
 *
 */
typedef void (*atsc3_mmt_mpu_mfu_on_sample_complete_f)(uint16_t packet_id, block_t* mmt_mfu_sample);
typedef void (*atsc3_mmt_mpu_mfu_on_sample_corrupt_f) (uint16_t packet_id, block_t* mmt_mfu_sample);
typedef void (*atsc3_mmt_mpu_mfu_on_sample_missing_f) (uint16_t packet_id, block_t* mmt_mfu_sample);

typedef struct atsc3_mmt_mfu_context {
	mmtp_flow_t* mmtp_flow;
	udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;
	lls_slt_monitor_t* lls_slt_monitor;
	lls_sls_mmt_session_t* matching_lls_slt_mmt_session;

	mp_table* mp_table_last;

	//from ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H
	atsc3_mmt_mpu_on_sequence_number_change_f 						atsc3_mmt_mpu_on_sequence_number_change;

	//from ATSC3_MMT_CONTEXT_SIGNALLING_INFORMATION_DEPACKETIZER_H
	atsc3_mmt_signalling_information_on_mp_table_f 					atsc3_mmt_signalling_information_on_mp_table;
	atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_f 	atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor;

	//MFU specific callbacks
	atsc3_mmt_mpu_mfu_on_sample_complete_f 							atsc3_mmt_mpu_mfu_on_sample_complete;
	atsc3_mmt_mpu_mfu_on_sample_corrupt_f 							atsc3_mmt_mpu_mfu_on_sample_corrupt;
	atsc3_mmt_mpu_mfu_on_sample_missing_f 							atsc3_mmt_mpu_mfu_on_sample_missing;

} atsc3_mmt_mfu_context_t;


//wire up dummmy null callback(s) to prevent dispatcher from multiple if(..) checks...
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_new();

//we will return mmtp_mpu_packet if it was successfully persisted, otherwise it will be null'd out
mmtp_mpu_packet_t* mmtp_process_from_payload_with_context(udp_packet_t *udp_packet,
														  mmtp_mpu_packet_t* mmtp_mpu_packet,
														  atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context);

    
#ifdef __cplusplus
}
#endif
    

#endif /* ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_H */
