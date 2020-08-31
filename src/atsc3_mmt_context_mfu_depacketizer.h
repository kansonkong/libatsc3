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
 * NOTE
 */
typedef void (*atsc3_mmt_mpu_mfu_on_sample_complete_f) (uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_rebuilt);
typedef void (*atsc3_mmt_mpu_mfu_on_sample_corrupt_f)  (uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);
typedef void (*atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_f) (uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample,  uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);
typedef void (*atsc3_mmt_mpu_mfu_on_sample_missing_f)  (uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number);

typedef struct atsc3_mmt_mfu_mpu_timestamp_descriptor {
	uint16_t 	packet_id;
	uint32_t 	mpu_sequence_number;
	uint64_t 	mpu_presentation_time_ntp64;
	uint32_t 	mpu_presentation_time_seconds;
	uint32_t 	mpu_presentation_time_microseconds;
	uint64_t 	mpu_presentation_time_as_us_value; //%" PRIu64

} atsc3_mmt_mfu_mpu_timestamp_descriptor_t;

typedef struct atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window {
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_mmt_mfu_mpu_timestamp_descriptor);
} atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window, atsc3_mmt_mfu_mpu_timestamp_descriptor);

typedef struct atsc3_mmt_mfu_context {
	//INTERNAL data structs
	udp_flow_t* 	udp_flow;
	mmtp_flow_t* 	mmtp_flow;

	udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;
	lls_slt_monitor_t* lls_slt_monitor;
	lls_sls_mmt_session_t* matching_lls_sls_mmt_session;

	mp_table_t* mp_table_last;
	atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window_t												packet_id_mpu_timestamp_descriptor_window;

	//INTERNAL event callbacks
	__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_f			__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor;

	//EXTERNAL helper methods
	atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_f										get_mpu_timestamp_from_packet_id_mpu_sequence_number;

	//EXTERNAL in-proc event callbacks

	//from ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H
	atsc3_mmt_mpu_on_sequence_number_change_f 															atsc3_mmt_mpu_on_sequence_number_change;
    atsc3_mmt_mpu_on_sequence_mpu_metadata_present_f               										atsc3_mmt_mpu_on_sequence_mpu_metadata_present;			//dispatched when a new mpu_metadata (init box) is present and re-constituted
    																														//use atsc3_hevc_nal_extractor to convert init to NAL's as needed for HEVC decoder
	//from ATSC3_MMT_CONTEXT_SIGNALLING_INFORMATION_DEPACKETIZER_H
	atsc3_mmt_signalling_information_on_mp_table_subset_f 												atsc3_mmt_signalling_information_on_mp_table_subset; 	//dispatched when table_id >= 0x11 (17) && table_id <= 0x19 (31)
	atsc3_mmt_signalling_information_on_mp_table_complete_f 											atsc3_mmt_signalling_information_on_mp_table_complete; 	//dispatched when table_id == 0x20 (32)

	atsc3_mmt_signalling_information_on_video_essence_packet_id_f										atsc3_mmt_signalling_information_on_video_essence_packet_id;
	atsc3_mmt_signalling_information_on_audio_essence_packet_id_f										atsc3_mmt_signalling_information_on_audio_essence_packet_id;
	atsc3_mmt_signalling_information_on_stpp_essence_packet_id_f										atsc3_mmt_signalling_information_on_stpp_essence_packet_id;

	atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_f					atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor;
	atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_f					atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor;
	atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_f					atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor;

	atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_f 										atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor;

	//MFU specific callbacks
	atsc3_mmt_mpu_mfu_on_sample_complete_f 																atsc3_mmt_mpu_mfu_on_sample_complete;
	atsc3_mmt_mpu_mfu_on_sample_corrupt_f 																atsc3_mmt_mpu_mfu_on_sample_corrupt;
	atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_f												atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header;
	atsc3_mmt_mpu_mfu_on_sample_missing_f 																atsc3_mmt_mpu_mfu_on_sample_missing;

	//Lastly, in the spirit of OOO MMT, movie fragment metadata comes last and should only be used as a last resort...
	atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_f                                         atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present;

} atsc3_mmt_mfu_context_t;

atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number);

//wire up dummmy null callback(s) to prevent dispatcher from multiple if(..) checks...
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_new();
void atsc3_mmt_mfu_context_free(atsc3_mmt_mfu_context_t** atsc3_mmt_mfu_context_p);



//Warning: cross boundary processing hooks with callback invocation - impl's in atsc3_mmt_context_mfu_depacketizer.c

void mmtp_mfu_process_from_payload_with_context(udp_packet_t *udp_packet,
												mmtp_mpu_packet_t* mmtp_mpu_packet,
												atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context);

//MFU re-constituion and emission in context
void mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container, uint32_t mpu_sequence_number, uint32_t mfu_sample_number_from_current_du, bool flush_all_fragmentss);


//MMT signalling information processing
void mmt_signalling_message_process_with_context(udp_packet_t *udp_packet,
												mmtp_signalling_packet_t* mmtp_signalling_packet,
												atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context);


//movie fragment sample duration parsing...non bento4 impl
uint32_t atsc3_mmt_movie_fragment_extract_sample_duration(block_t* mmt_movie_fragment_metadata);


#define __MMT_CONTEXT_MPU_ERROR(...)        __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMT_CONTEXT_MPU_WARN(...)   		__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMT_CONTEXT_MPU_INFO(...)    		__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define __MMT_CONTEXT_MPU_SIGNAL_INFO(...)  if(_MMT_CONTEXT_MPU_SIGNAL_INFO_ENABLED) { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __MMT_CONTEXT_MPU_DEBUG(...)   		if(_MMT_CONTEXT_MPU_DEBUG_ENABLED) 		 { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); };
#define __MMT_CONTEXT_MPU_TRACE(...)  		if(_MMT_CONTEXT_MPU_TRACE_ENABLED) 		 { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); };

    
#ifdef __cplusplus
}
#endif
    

#endif /* ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_H */
