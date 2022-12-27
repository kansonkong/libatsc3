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
#include "atsc3_mmt_context_signalling_information_callbacks_internal.h"

#include "atsc3_isobmff_box_parser_tools.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MFU specific callbacks
 * todo - include byte ranges for lost DU's?
 * NOTE
 */
typedef struct atsc3_mmt_mfu_context atsc3_mmt_mfu_context_t;

//jjustman-2021-10-21 - internal_sei: return sei message payload as needed...
typedef bool (*mmt_mpu_mfu_on_sample_complete_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_internal_f) (block_t* mmt_mfu_sample);
//external_sei:
typedef void (*mmt_mpu_mfu_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_detected_f) (atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number);

typedef void (*atsc3_mmt_mpu_mfu_on_sample_complete_f) (atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_rebuilt);
typedef void (*atsc3_mmt_mpu_mfu_on_sample_corrupt_f)  (atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);
typedef void (*atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_f) (atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample,  uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);
typedef void (*atsc3_mmt_mpu_mfu_on_sample_missing_f)  (atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number);

typedef void (*atsc3_mmt_signalling_information_on_userservicedescription_present_f)				(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_signalling_information_usbd_component_t* mmt_atsc3_usbd_message);
typedef bool (*atsc3_mmt_signalling_information_on_routecomponent_message_present_f)				(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_route_component_t* mmt_atsc3_route_component);
typedef void (*atsc3_mmt_signalling_information_on_held_message_present_f) 							(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_held_message_t* mmt_atsc3_held_message);

// atsc3_message_content_type: 0x0004 -  MMT_ATSC3_MESSAGE_CONTENT_TYPE_APPLICATION_EVENT_INFORMATION_A337:

typedef void (*atsc3_mmt_signalling_information_on_application_event_information_present_f)         (atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_application_event_information_t * mmt_atsc3_message_content_type_application_event_information);

typedef void (*atsc3_mmt_signalling_information_on_video_stream_properties_descriptor_present_f) 	(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_video_stream_properties_descriptor_t* mmt_atsc3_video_stream_properties_descriptor_message);

// atsc3_message_content_type: 0x0007 -  MMT_ATSC3_MESSAGE_CONTENT_TYPE_INBAND_EVENT_DESCRIPTOR_A337:
typedef void (*atsc3_mmt_signalling_information_on_inband_event_descriptor_present_f)               (atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_inband_event_descriptor_t* mmt_atsc3_message_content_type_inband_event_descriptor);

typedef void (*atsc3_mmt_signalling_information_on_caption_asset_descriptor_present_f) 				(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_caption_asset_descriptor_t* mmt_atsc3_caption_asset_descriptor_message);
typedef void (*atsc3_mmt_signalling_information_on_audio_stream_properties_descriptor_present_f) 	(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_audio_stream_properties_descriptor_t* mmt_atsc3_audio_stream_properties_descriptor_message);
typedef void (*atsc3_mmt_signalling_information_on_security_properties_descriptor_LAURL_present_f) 	(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_t* mmt_atsc3_security_properties_descriptor_LAURL_message);

/*
 * From: https://tools.ietf.org/html/rfc5905#section-6
 *
 * The 64-bit timestamp format is used in packet headers and other
   places with limited word size.  It includes a 32-bit unsigned seconds
   field spanning 136 years and a 32-bit fraction field resolving 232
   picoseconds.
 */
typedef struct atsc3_mmt_mfu_mpu_timestamp_descriptor {
	uint16_t 	packet_id;
	uint32_t	mmtp_timestamp;

	uint32_t 	mpu_sequence_number;

	uint64_t 	mpu_presentation_time_ntp64;
	uint32_t 	mpu_presentation_time_seconds;
	uint32_t 	mpu_presentation_time_microseconds;
	uint64_t 	mpu_presentation_time_as_us_value; //%" PRIu64

	//jjustman-2021-01-19 - for when we "recover" the mpu_presentation_time values by a differential of the most recent <packet_id, mpu_sequence_number, mpu_presentation_time_ntp64>
	bool 		mpu_presentation_time_computed_from_recovery_mmtp_timestamp_flag;

    //jjustman-2021-10-05 - YOGA #16967 - in non-recovery cases, this value should be 0, otherwise
    // keep track of our sample number if we have been created from an interpolated recovery timestamp descriptor
    // so we can properly re-base to a correct sample[0] mpu_presentation_t

    uint32_t	recovery_detected_at_mmtp_timestamp;
    uint32_t    recovery_interpolated_from_sample_number;
    uint32_t	recovery_interpolated_to_mmtp_timestamp_delta_ntp32;

	uint32_t	recovery_anchor_mmtp_timestamp;
	uint32_t	recovery_anchor_mpu_sequence_number;
	uint32_t    recovery_anchor_sample_number;
	uint64_t	recovery_anchor_mpu_presentation_time_ntp64;

} atsc3_mmt_mfu_mpu_timestamp_descriptor_t;

typedef struct atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window {
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_mmt_mfu_mpu_timestamp_descriptor);
} atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window, atsc3_mmt_mfu_mpu_timestamp_descriptor);


/*
 * atsc3_mmt_mfu_context:
 *
 *  See atsc3_mmt_mfu_context_callbacks_noop or atsc3_mmt_mfu_context_callbacks_default_jni for 'auto-wiring' of callbacks for MMTP processing events
 *
 */

typedef struct atsc3_mmt_mfu_context {

	//INTERNAL data structs
	udp_flow_t* 	                                    udp_flow;
    udp_flow_latest_mpu_sequence_number_container_t*    udp_flow_latest_mpu_sequence_number_container;

	mmtp_flow_t* 	                                    mmtp_flow;

    //active context
    mmtp_packet_id_packets_container_t*                 mmtp_packet_id_packets_container;

    lls_sls_mmt_session_t*                              matching_lls_sls_mmt_session;

    //holdover context information
	mp_table_t* mp_table_last;
	atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window_t														packet_id_mpu_timestamp_descriptor_window;

	//INTERNAL event callbacks
    atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_internal_f		        		atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_internal;


    struct atsc3_mmt_mfu_context_callbacks_internal_sei {
        mmt_mpu_mfu_on_sample_complete_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_internal_f   mmt_mpu_mfu_on_sample_complete_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_internal;
    } internal_sei;

    struct atsc3_mmt_mfu_context_callbacks_external_sei {
        mmt_mpu_mfu_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_detected_f                      mmt_mpu_mfu_sei_scan_for_sl_hdr_sei_itu_t_35_and_terminal_provider_code_detected;
    } external_sei;

	//EXTERNAL helper methods
	atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_f												get_mpu_timestamp_from_packet_id_mpu_sequence_number;
	atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential_f		get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential;

	//EXTERNAL in-proc event callbacks

	//from ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H
	atsc3_mmt_mpu_on_sequence_number_change_f 															atsc3_mmt_mpu_on_sequence_number_change;
    atsc3_mmt_mpu_on_sequence_mpu_metadata_present_f               										atsc3_mmt_mpu_on_sequence_mpu_metadata_present;			//dispatched when a new mpu_metadata (init box) is present and re-constituted
    																														//use atsc3_hevc_nal_extractor to convert init to NAL's as needed for HEVC decoder
	//from ATSC3_MMT_CONTEXT_SIGNALLING_INFORMATION_DEPACKETIZER_H
	//ISO23008-1 SI messages
	atsc3_mmt_signalling_information_on_mp_table_subset_f 												atsc3_mmt_signalling_information_on_mp_table_subset; 	//dispatched when table_id >= 0x11 (17) && table_id <= 0x19 (31)
	atsc3_mmt_signalling_information_on_mp_table_complete_f 											atsc3_mmt_signalling_information_on_mp_table_complete; 	//dispatched when table_id == 0x20 (32)

	atsc3_mmt_signalling_information_on_video_essence_packet_id_f										atsc3_mmt_signalling_information_on_video_essence_packet_id;
	atsc3_mmt_signalling_information_on_audio_essence_packet_id_f										atsc3_mmt_signalling_information_on_audio_essence_packet_id;
	atsc3_mmt_signalling_information_on_stpp_essence_packet_id_f										atsc3_mmt_signalling_information_on_stpp_essence_packet_id;

	atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_f					atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor;
	atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_f					atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor;
	atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_f					atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor;

	atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_f 										atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor;
	
	//A/331:2021 SI messages (e.g. table_id >=8100)

	//MFU specific callbacks
	atsc3_mmt_mpu_mfu_on_sample_complete_f 																atsc3_mmt_mpu_mfu_on_sample_complete;                   //REQUIRED: callback to decoder with a fully recovered MFU sample, no DU loss
	atsc3_mmt_mpu_mfu_on_sample_corrupt_f 																atsc3_mmt_mpu_mfu_on_sample_corrupt;                    //OPTIONAL: callback to decoder with a partially recovered MFU sample, intra DU loss
	atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_f												atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header;  //OPTIONAL: callback to decoder with a partially recovered MFU sample, missing MMTHSampleHeader information, entire MFU is suspect
	atsc3_mmt_mpu_mfu_on_sample_missing_f 																atsc3_mmt_mpu_mfu_on_sample_missing;                    //jjustman-2020-10-13: TODO: OPTIONAL: for statistics purposes of completely missing MFU samples (e.g. deep fade), for a RQF-like message

	//Lastly, in the spirit of OOO MMT, movie fragment metadata comes last and should only be used as a last resort...
	atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_f                                         atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present;

	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_UserServiceDescription: mmt_atsc3_message_content_type == 0x0001 -> mmt_atsc3_signalling_information_usbd_component_t
	atsc3_mmt_signalling_information_on_userservicedescription_present_f                                atsc3_mmt_signalling_information_on_userservicedescription_present;
	//return: assign_routecomponent_payload_to_context
	atsc3_mmt_signalling_information_on_routecomponent_message_present_f                                atsc3_mmt_signalling_information_on_routecomponent_message_present;
	mmt_atsc3_route_component_t*                                                                        mmt_atsc3_route_component_monitored;

	//jjustman-2021-09-14 - TODO: add in callbacks for AEI -> 0x0004

	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_HELD
    atsc3_mmt_signalling_information_on_held_message_present_f                                          atsc3_mmt_signalling_information_on_held_message_present;
    
    //MMT_ATSC3_MESSAGE_CONTENT_TYPE_APPLICATION_EVENT_INFORMATION_A337 - 0x0004
    atsc3_mmt_signalling_information_on_application_event_information_present_f                         atsc3_mmt_signalling_information_on_application_event_information_present;
	
	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_VIDEO_STREAM_PROPERTIES_DESCRIPTOR
	atsc3_mmt_signalling_information_on_video_stream_properties_descriptor_present_f                    mmt_atsc3_message_content_type_video_stream_properties_descriptor_present;

    //MMT_ATSC3_MESSAGE_CONTENT_TYPE_INBAND_EVENT_DESCRIPTOR_A337 - 0x0007
    atsc3_mmt_signalling_information_on_inband_event_descriptor_present_f                               atsc3_mmt_signalling_information_on_inband_event_descriptor_present;

	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR
	atsc3_mmt_signalling_information_on_caption_asset_descriptor_present_f                  	 		atsc3_mmt_signalling_information_on_caption_asset_descriptor_present;

	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_AUDIO_STREAM_PROPERTIES_DESCRIPTOR
	atsc3_mmt_signalling_information_on_audio_stream_properties_descriptor_present_f                    atsc3_mmt_signalling_information_on_audio_stream_properties_descriptor_present;

	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL
	atsc3_mmt_signalling_information_on_security_properties_descriptor_LAURL_present_f                  atsc3_mmt_signalling_information_on_security_properties_descriptor_LAURL_present;

	//jjustman-2020-12-24 - TODO: relax this tight coupling from atsc3 to mmt package
	//shared pointers that we don't own
    struct atsc3_mmt_mfu_context_transients {
    	//chained from lls_slt_monitor - see atsc3_lls_slt_monitor_free(...)
		lls_slt_monitor_t*                                  lls_slt_monitor;

		//mmtp_ specific transients are chained from vector_builder, and is part of a mmtp_flow collection (or sub-collection),
		//see atsc3_mmt_mfu_context_free impl: mmtp_flow_free_mmtp_asset_flow(atsc3_mmt_mfu_context->mmtp_flow);
		mmtp_asset_flow_t*                                  mmtp_asset_flow;
		mmtp_asset_t*                                       mmtp_asset;
	} transients;

} atsc3_mmt_mfu_context_t;

atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_internal_flows_new(void);

void atsc3_mmt_mfu_context_free(atsc3_mmt_mfu_context_t** atsc3_mmt_mfu_context_p);

mmtp_asset_t* atsc3_mmt_mfu_context_mfu_depacketizer_context_update_find_or_create_mmtp_asset(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, udp_packet_t* udp_packet, lls_slt_monitor_t* lls_slt_monitor, lls_sls_mmt_session_t* matching_lls_sls_mmt_session);

mmtp_packet_id_packets_container_t* atsc3_mmt_mfu_context_mfu_depacketizer_update_find_or_create_mmtp_packet_id_packets_container(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmtp_asset_t* mmtp_asset, mmtp_packet_header_t* mmtp_packet_header);

atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number);

//jjustman-2021-01-19 - get our mpu_timestamp_descriptor, either from the SI messsage or from recovering via mmtp_timestamp differential if our SI message was lost
//		note: injects a "synthetic" mpu_timestamp_descriptor for durability in the condition of a possibly sustained SI message loss
atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number);

//Warning: cross boundary processing hooks with callback invocation - impl's in atsc3_mmt_context_mfu_depacketizer.c

void mmtp_mfu_process_from_payload_with_context(udp_packet_t *udp_packet, mmtp_mpu_packet_t* mmtp_mpu_packet, atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context);

//MFU re-constituion and emission in context
void mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container, uint32_t mpu_sequence_number, uint32_t mfu_sample_number_from_current_du, bool flush_all_fragmentss);


//MMT signalling information processing
void mmt_signalling_message_dispatch_context_notification_callbacks(udp_packet_t *udp_packet, mmtp_signalling_packet_t* mmtp_signalling_packet, atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context);


//movie fragment sample duration parsing...non bento4 impl
uint32_t atsc3_mmt_movie_fragment_extract_sample_duration_us(block_t* mmt_movie_fragment_metadata, uint32_t decoder_configuration_timebase);


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
