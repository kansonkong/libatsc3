/*
 * atsc3_mmt_reconstitution_from_media_sample.c
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 *
 *
 *
 *      TODO:
 *

A/331: 8.1.2.3 Synchronization:
The synchronization of MPUs shall be done by using timestamps referencing UTC
delivered by ATSC 3.0 PHY layer.

The MPU_timestamp_descriptor as defined in subclause 10.5.2 of ISO/IEC 23008-1 [37]
shall be used to represent the presentation time of the first media sample in
presentation order in each MPU.

The presentation time of each media sample of an MPU shall be calculated by
adding the presentation time of the first media sample of an MPU to the value
of the composition time of each sample in the MPU.

The rule to calculate the value of the composition time of media samples in an MPU
shall be be calculated by using the rule in the ISO BMFF specification [34].


23008-1 reference:

CRI table:

CRI descriptor: 0x0000

MPU_timestamp_descriptor: 0x0001

 */

#include "atsc3_mmt_context_mfu_depacketizer.h"

int _MMT_CONTEXT_MPU_SIGNAL_INFO_ENABLED = 0;
int _MMT_CONTEXT_MPU_DEBUG_ENABLED = 1;
int _MMT_CONTEXT_MPU_TRACE_ENABLED = 0;

//TODO: jjustman-2019-10-03 - refactor these out to proper impl's:

//MPU
void atsc3_mmt_mpu_on_sequence_number_change_noop(uint16_t packet_id, uint32_t mpu_sequence_number_old, uint32_t mpu_sequence_number_new) {
	//noop
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_on_sequence_number_change_noop: packet_id: %u, from %d, to %d", packet_id, mpu_sequence_number_old,  mpu_sequence_number_new);
}

//MP_table
void atsc3_mmt_signalling_information_on_mp_table_subset_noop(mp_table_t* mp_table) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_mp_table_subset_noop: mp_table: %p", mp_table);
}

//MP_table
void atsc3_mmt_signalling_information_on_mp_table_complete_noop(mp_table_t* mp_table) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_mp_table_complete_noop: mp_table: %p", mp_table);
}

//audio essence packet id extraction
void atsc3_mmt_signalling_information_on_audio_essence_packet_id_noop(uint16_t audio_packet_id) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_audio_packet_id_noop: audio_packet_id: %u", audio_packet_id);
}

//video essence packet_id extraction
void atsc3_mmt_signalling_information_on_video_essence_packet_id_noop(uint16_t video_packet_id) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_video_packet_id_noop: video_packet_id: %u", video_packet_id);
}


//audio packet id extraction with mpu_sequence and mpu_presentation_time
void atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_noop(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_noop: audio_packet_id: %u, mpu_sequence_number: %u, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
			audio_packet_id,
			mpu_sequence_number,
			mpu_presentation_time_ntp64,
			mpu_presentation_time_seconds,
			mpu_presentation_time_microseconds);
}

//video packet_id extraction with mpu_sequence and mpu_presentation_time
void atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_noop(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_noop: audio_packet_id: %u, mpu_sequence_number: %u, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
				audio_packet_id,
				mpu_sequence_number,
				mpu_presentation_time_ntp64,
				mpu_presentation_time_seconds,
				mpu_presentation_time_microseconds);
}


void atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_noop(uint16_t packet_id, uint32_t mpu_sequence_number, mmt_signalling_message_mpu_timestamp_descriptor_t* mmt_signalling_message_mpu_timestamp_descriptor) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_noop: packet_id: %u, mpu_sequence_number: %d, mmt_signalling_message_mpu_timestamp_descriptor: %p",
				packet_id,
				mpu_sequence_number,
				mmt_signalling_message_mpu_timestamp_descriptor);
}

//MFU callbacks

void atsc3_mmt_mpu_mfu_on_sample_complete_noop(uint16_t packet_id, block_t* mmt_mfu_sample) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_complete_noop: packet_id: %u, mmt_mfu_sample: %p, len: %d",
			packet_id,
			mmt_mfu_sample,
			mmt_mfu_sample->p_size);

}

void atsc3_mmt_mpu_mfu_on_sample_corrupt_noop(uint16_t packet_id, block_t* mmt_mfu_sample) {
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_corrupt_noop: packet_id: %u, mmt_mfu_sample: %p, len: %d",
				packet_id,
				mmt_mfu_sample,
				mmt_mfu_sample->p_size);}


void atsc3_mmt_mpu_mfu_on_sample_missing_noop(uint16_t packet_id, block_t* mmt_mfu_sample) {
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_missing_noop: packet_id: %u, mmt_mfu_sample: %p, len: %d",
				packet_id,
				mmt_mfu_sample,
				mmt_mfu_sample->p_size);}


atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_new() {
	atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = calloc(1, sizeof(atsc3_mmt_mfu_context_t));

	//MPU related callbacks
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change 						= &atsc3_mmt_mpu_on_sequence_number_change_noop;

	//signalling information callbacks
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_subset			= &atsc3_mmt_signalling_information_on_mp_table_subset_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_complete 		= &atsc3_mmt_signalling_information_on_mp_table_complete_noop;

	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_essence_packet_id 	= &atsc3_mmt_signalling_information_on_audio_essence_packet_id_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_essence_packet_id 	= &atsc3_mmt_signalling_information_on_video_essence_packet_id_noop;

	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_noop;

	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_noop;

	//MFU related callbacks
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete = &atsc3_mmt_mpu_mfu_on_sample_complete_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt 	= &atsc3_mmt_mpu_mfu_on_sample_corrupt_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_missing 	= &atsc3_mmt_mpu_mfu_on_sample_missing_noop;

	return atsc3_mmt_mfu_context;
}

/**
 * jjustman-2019-03-30 - combine pending mpu_sequence_numbers until we have at least 1a and 1v packet to flush for decoder...
mmtp_sub_flow_vector_t* mmtp_sub_flow_vector,
		mmtp_payload_fragments_union_t** mmtp_payload_p,
 
 TODO: refactor udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container for mpu recon storage

 //jjustman-2019-08-31 - new atsc3_mmtp_packet_types.h refactoring here...
  *
  * TODO: do not free mmtp_mpu_packet here
 */
//
//void mmtp_process_from_payload_with_context(udp_packet_t *udp_packet,
//											mmtp_mpu_packet_t* mmtp_mpu_packet,
//											atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context) {
//
//	atsc3_mmt_mfu_context->udp_flow = &udp_packet->udp_flow;
//
//
//	//borrow from our context
//	mmtp_flow_t *mmtp_flow = atsc3_mmt_mfu_context->mmtp_flow;
//	lls_slt_monitor_t* lls_slt_monitor = atsc3_mmt_mfu_context->lls_slt_monitor;
//	lls_sls_mmt_session_t* matching_lls_sls_mmt_session = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session;
//	udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container = atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container;
//
//
//    //forward declare so our goto will compile
//    mmtp_asset_flow_t* mmtp_asset_flow = NULL;
//    mmtp_asset_t* mmtp_asset = NULL;
//    mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container = NULL;
//    mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = NULL;
//
//    lls_sls_mmt_monitor_t* matching_lls_sls_mmt_monitor = lls_sls_mmt_monitor_find_from_service_id(lls_slt_monitor, matching_lls_sls_mmt_session->service_id);
//
//    if(!matching_lls_sls_mmt_monitor) {
//        __MMT_CONTEXT_MPU_WARN("mmtp_process_from_payload_with_context: service_id: %u, packet_id: %u, lls_slt_monitor size: %u, matching_lls_sls_mmt_monitor is NULL!",
//                    matching_lls_sls_mmt_session->service_id,
//                    mmtp_mpu_packet->mmtp_packet_id,
//                    lls_slt_monitor->lls_sls_mmt_monitor_v.count);
//
//
//        goto packet_cleanup;
//    }
//
//    //clear out our last flow processed status
//    matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed = false;
//    matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed = false;
//
//    //assign our mmtp_mpu_packet to asset/packet_id/mpu_sequence_number flow
//    mmtp_asset_flow = mmtp_flow_find_or_create_from_udp_packet(mmtp_flow, udp_packet);
//    mmtp_asset = mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow, matching_lls_sls_mmt_session);
//    mmtp_packet_id_packets_container = mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset, mmtp_mpu_packet);
//    mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_or_create_mpu_sequence_number_mmtp_mpu_packet_collection_from_mmt_mpu_packet(mmtp_packet_id_packets_container, mmtp_mpu_packet);
//
//    //persist our mmtp_mpu_packet for mpu reconstitution as per original libatsc3 design
//    mpu_sequence_number_mmtp_mpu_packet_collection_add_mmtp_mpu_packet(mpu_sequence_number_mmtp_mpu_packet_collection, mmtp_mpu_packet);
//
//    //TODO - this should never happen with this strong type
//    if(mmtp_mpu_packet->mmtp_payload_type == 0x0) {
//        atsc3_global_statistics->packet_counter_mmt_mpu++;
//
//        if(mmtp_mpu_packet->mpu_timed_flag == 1) {
//            atsc3_global_statistics->packet_counter_mmt_timed_mpu++;
//
//            if(matching_lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id == matching_lls_sls_mmt_session->service_id &&
//            		matching_lls_sls_mmt_session->sls_destination_ip_address == udp_packet->udp_flow.dst_ip_addr &&
//					matching_lls_sls_mmt_session->sls_destination_udp_port == udp_packet->udp_flow.dst_port) {
//
//
//            	udp_flow_packet_id_mpu_sequence_tuple_t* last_flow_reference = udp_flow_latest_mpu_sequence_number_add_or_replace_and_check_for_rollover(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_mpu_packet, lls_slt_monitor, matching_lls_sls_mmt_session);
//
//            	//see if we are an audio packet that rolled over
//				if(matching_lls_sls_mmt_monitor->audio_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
//					//see if we have incremented our mpu_sequence_number with current mmtp_payload
//
//					if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
//
//
//		            	atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change(mmtp_mpu_packet->mmtp_packet_id, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number, mmtp_mpu_packet->mpu_sequence_number);
//
//
////						lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff_pending_mux = atsc3_isobmff_build_raw_mpu_from_single_sequence_number(&udp_packet->udp_flow,
////														   udp_flow_latest_mpu_sequence_number_container,
////														   matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id,
////														   matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
////														   &lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff,
////                                                           mmtp_packet_id_packets_container);
//
////
////						lls_sls_monitor_buffer_isobmff_intermediate_mmt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
////																				lls_slt_monitor->lls_sls_mmt_monitor->service_id,
////                                                                                 mmtp_mpu_packet->mmtp_packet_id,
////                                                                                 matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
////                                                                                 "a.orig");
////
////						atsc3_isobmff_rebuild_track_mpu_from_sample_data(lls_sls_monitor_buffer_isobmff_pending_mux);
////						ls_sls_monitor_buffer_isobmff_mmt_mpu_rebuilt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
////																				lls_slt_monitor->lls_sls_mmt_monitor->service_id,
////																				matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
////																				"a.rebuilt");
////
//                        mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number);
//                        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
//                        //jjustman-2019-09-05 - this might be too agressive here
//                        //mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);
//                    }
//                    //keep track of our last mpu_sequence_number...
//					udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio, last_flow_reference);
//
//				} else if(matching_lls_sls_mmt_monitor->video_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
//					//see if we have incremented our mpu_sequence_number with current mmtp_payload
//
//					if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
//
//		            	atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change(mmtp_mpu_packet->mmtp_packet_id, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number, mmtp_mpu_packet->mpu_sequence_number);
//
//						//
////						lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff_pending_mux = atsc3_isobmff_build_raw_mpu_from_single_sequence_number(&udp_packet->udp_flow,
////														   udp_flow_latest_mpu_sequence_number_container,
////														   matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id,
////														   matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
////														   &lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff,
////                                                           mmtp_packet_id_packets_container);
////
////						lls_sls_monitor_buffer_isobmff_intermediate_mmt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
////																				  lls_slt_monitor->lls_sls_mmt_monitor->service_id,
////																				  mmtp_mpu_packet->mmtp_packet_id,
////																				  matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
////																				  "v.orig");
////						atsc3_isobmff_rebuild_track_mpu_from_sample_data(lls_sls_monitor_buffer_isobmff_pending_mux);
////						ls_sls_monitor_buffer_isobmff_mmt_mpu_rebuilt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
////																				lls_slt_monitor->lls_sls_mmt_monitor->service_id,
////																				matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
////																				"v.rebuilt");
////                        mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number);
//                        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
//                        //jjustman-2019-09-05 - this might be too agressive here
//                        //mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);
//
//                    }
//
//					//keep track of our last mpu_sequence_number...
//           			udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video, last_flow_reference);
//				}
//
//				//if we have at least one block in the following, push it to the isobmff track joiner as usual
//				if(matching_lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mmt_mpu_rebuilt_and_appending_for_isobmff_mux &&
//						matching_lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mmt_mpu_rebuilt_and_appending_for_isobmff_mux) {
//
////                    lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer_final_muxed_payload = atsc3_isobmff_build_joined_mmt_rebuilt_boxes(&lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer);
////
////                    if(lls_sls_monitor_output_buffer_final_muxed_payload) {
////                        //mark both of these flows as having been processed
////                        matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed = true;
////                        matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed = true;
////
////
////                        if(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
////                            //lls_sls_monitor_output_buffer_final_muxed_payload
////                            lls_sls_monitor_output_buffer_mmt_file_dump(lls_sls_monitor_output_buffer_final_muxed_payload, "mpu/",
////                                matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
////                                matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number);
////                        }
////
////                        //http support output
////                        if(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_enabled &&
////                                lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer &&
////                                lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_output_conntected) {
////                            //&& lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer) {
////
////                            lls_sls_monitor_reader_mutex_lock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_mutex);
////                            lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->total_fragments_incoming_written++;
////
////                            if(!lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming) {
////                                lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming =  block_Duplicate(lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block);
////                            } else {
////                                block_Resize(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming, lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming->p_size + lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_size);
////                                block_Write(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_size);
////                            }
////
////                            lls_sls_monitor_reader_mutex_unlock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_mutex);
////                        }
////
////                        //ffplay pipe output
////                        if(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled && lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer) {
////
////                            pipe_buffer_reader_mutex_lock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);
////
////                            pipe_buffer_unsafe_push_block(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->i_pos);
////
////                            lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.has_written_init_box = true;
////
////                            pipe_buffer_notify_semaphore_post(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);
////
////                            //check to see if we have shutdown
////                            lls_slt_monitor_check_and_handle_pipe_ffplay_buffer_is_shutdown(lls_slt_monitor);
////
////                            pipe_buffer_reader_mutex_unlock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);
////                        }
////
////                        lls_sls_monitor_output_buffer_reset_rebuilt_mpu_moof_and_fragment_position(&lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer);
////                    }
//                    //jjustman-2019-09-05 - TODO: fix this hack
//                    //clear out packet_id=0
//                    //mmtp_mpu_packet = NULL;
////                    mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_new();
////                    mmtp_mpu_packet->mmtp_packet_id = 0;
////
////                    mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container_packet_zero = mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset, mmtp_mpu_packet);
////                    mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container_packet_zero);
//                }
//            }
//        } else {
//            //non-timed
//            atsc3_global_statistics->packet_counter_mmt_nontimed_mpu++;
//        }
//
//        goto ret;
//
//    } else if(mmtp_mpu_packet->mmtp_payload_type == 0x2) {
//
//		atsc3_global_statistics->packet_counter_mmt_signaling++;
//		__MMT_CONTEXT_MPU_SIGNAL_INFO("mmtp_process_from_payload: processing mmt flow: %d.%d.%d.%d:(%u) packet_id: 0, signalling message", __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port));
//
//    } else {
//    	__MMT_CONTEXT_MPU_WARN("mmtp_process_from_payload: unknown payload type of 0x%x", mmtp_mpu_packet->mmtp_payload_type);
//		atsc3_global_statistics->packet_counter_mmt_unknown++;
//		goto packet_cleanup;
//    }
//
//packet_cleanup:
////    if(mmtp_mpu_packet) {
////    	__MMT_CONTEXT_MPU_TRACE("Cleaning up packet: %p", mmtp_mpu_packet);
////        mmtp_mpu_packet_free(&mmtp_mpu_packet);
////    }
//
//ret:
//    return;
//}



void mmtp_mfu_process_from_payload_with_context(udp_packet_t *udp_packet,
											mmtp_mpu_packet_t* mmtp_mpu_packet,
											atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context) {

	atsc3_mmt_mfu_context->udp_flow = &udp_packet->udp_flow;


	//borrow from our context
	mmtp_flow_t *mmtp_flow = atsc3_mmt_mfu_context->mmtp_flow;
	lls_slt_monitor_t* lls_slt_monitor = atsc3_mmt_mfu_context->lls_slt_monitor;
	lls_sls_mmt_session_t* matching_lls_sls_mmt_session = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session;
	udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container = atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container;


    //forward declare so our goto will compile
    mmtp_asset_flow_t* mmtp_asset_flow = NULL;
    mmtp_asset_t* mmtp_asset = NULL;
    mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container = NULL;
    mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = NULL;

    lls_sls_mmt_monitor_t* matching_lls_sls_mmt_monitor = lls_sls_mmt_monitor_find_from_service_id(lls_slt_monitor, matching_lls_sls_mmt_session->service_id);


    if(!matching_lls_sls_mmt_monitor) {
        __MMT_CONTEXT_MPU_WARN("mmtp_mfu_process_from_payload_with_context: service_id: %u, packet_id: %u, lls_slt_monitor size: %u, matching_lls_sls_mmt_monitor is NULL!",
                matching_lls_sls_mmt_session->service_id,
                mmtp_mpu_packet->mmtp_packet_id,
                lls_slt_monitor->lls_sls_mmt_monitor_v.count);

        goto packet_cleanup;
    }

    //assign our mmtp_mpu_packet to asset/packet_id/mpu_sequence_number flow
    mmtp_asset_flow = mmtp_flow_find_or_create_from_udp_packet(mmtp_flow, udp_packet);
    mmtp_asset = mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow, matching_lls_sls_mmt_session);
    mmtp_packet_id_packets_container = mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset, mmtp_mpu_packet);
    mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_or_create_mpu_sequence_number_mmtp_mpu_packet_collection_from_mmt_mpu_packet(mmtp_packet_id_packets_container, mmtp_mpu_packet);

    //persist our mmtp_mpu_packet for mpu reconstitution as per original libatsc3 design
    mpu_sequence_number_mmtp_mpu_packet_collection_add_mmtp_mpu_packet(mpu_sequence_number_mmtp_mpu_packet_collection, mmtp_mpu_packet);

    //TODO - this should never happen with this strong type
    if(mmtp_mpu_packet->mmtp_payload_type == 0x0) {
        atsc3_global_statistics->packet_counter_mmt_mpu++;

        if(mmtp_mpu_packet->mpu_timed_flag == 1) {
            atsc3_global_statistics->packet_counter_mmt_timed_mpu++;

            if(matching_lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id == matching_lls_sls_mmt_session->service_id &&
            		matching_lls_sls_mmt_session->sls_destination_ip_address == udp_packet->udp_flow.dst_ip_addr &&
					matching_lls_sls_mmt_session->sls_destination_udp_port == udp_packet->udp_flow.dst_port) {

            	udp_flow_packet_id_mpu_sequence_tuple_t* last_flow_reference = udp_flow_latest_mpu_sequence_number_add_or_replace_and_check_for_rollover(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_mpu_packet, lls_slt_monitor, matching_lls_sls_mmt_session);

            	char* essence_type = (matching_lls_sls_mmt_monitor->audio_packet_id == mmtp_mpu_packet->mmtp_packet_id) ? "a" : "v";

				//see if we are at the start of a new mfu sample
				if(mmtp_mpu_packet->mmthsample_header) {
					__MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_process_from_payload_with_context: new MFU.MMTHSample: packet_id: %u (%c), mpu_sequence_number: %u, mmthsample.samplenumber: %u, mmthsample.movie_fragment_sequence_number: %u, mmthsample.length: %u",
						mmtp_mpu_packet->mmtp_packet_id,
						*essence_type,
						mmtp_mpu_packet->mpu_sequence_number,
						mmtp_mpu_packet->mmthsample_header->samplenumber,
						mmtp_mpu_packet->mmthsample_header->movie_fragment_sequence_number,
						mmtp_mpu_packet->mmthsample_header->length);
				} else {
					__MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_process_from_payload_with_context: carry over MFU : packet_id: %u (%c), mpu_sequence_number: %u, mmtp_mpu_packet.samplenumber: %u, mmtp_mpu_packet.mpu_fragment_counter: %u, mmtp_mpu_packet.offset: %u",
						mmtp_mpu_packet->mmtp_packet_id,
						*essence_type,
						mmtp_mpu_packet->mpu_sequence_number,
						mmtp_mpu_packet->sample_number,
						mmtp_mpu_packet->mpu_fragment_counter,
						mmtp_mpu_packet->offset);
				}

                
                
                if(matching_lls_sls_mmt_monitor->video_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
                    if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change(mmtp_mpu_packet->mmtp_packet_id, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number, mmtp_mpu_packet->mpu_sequence_number);
                        mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number);
                        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
                        //keep track of our last mpu_sequence_number...
                    }
                    udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video, last_flow_reference);

                } else if(matching_lls_sls_mmt_monitor->audio_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
                    if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change(mmtp_mpu_packet->mmtp_packet_id, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number, mmtp_mpu_packet->mpu_sequence_number);
                        mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number);
                        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
                        //keep track of our last mpu_sequence_number...
                    }
                    udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio, last_flow_reference);
                }
            }
        } else {
            //non-timed
            atsc3_global_statistics->packet_counter_mmt_nontimed_mpu++;
        }

        goto ret;

    } else if(mmtp_mpu_packet->mmtp_payload_type == 0x2) {

		atsc3_global_statistics->packet_counter_mmt_signaling++;
		__MMT_CONTEXT_MPU_SIGNAL_INFO("mmtp_mfu_process_from_payload_with_context: processing mmt flow: %d.%d.%d.%d:(%u) packet_id: 0, signalling message", __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port));

    } else {
    	__MMT_CONTEXT_MPU_WARN("mmtp_mfu_process_from_payload_with_context: unknown payload type of 0x%x", mmtp_mpu_packet->mmtp_payload_type);
		atsc3_global_statistics->packet_counter_mmt_unknown++;
		goto packet_cleanup;
    }

packet_cleanup:
//    if(mmtp_mpu_packet) {
//    	__MMT_CONTEXT_MPU_TRACE("Cleaning up packet: %p", mmtp_mpu_packet);
//        mmtp_mpu_packet_free(&mmtp_mpu_packet);
//    }

ret:
    return;
}



/*
 * jjustman-2019-10-03: todo: filter by
 * 	ignore atsc3_mmt_mfu_context->lls_slt_monitor (or mmtp_flow, matching_lls_sls_mmt_session)
 *
 */

void mmt_signalling_message_process_with_context(udp_packet_t *udp_packet,
												 mmtp_signalling_packet_t* mmtp_signalling_packet,
												 atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context) {
	atsc3_mmt_mfu_context->udp_flow = &udp_packet->udp_flow;

	for(int i=0; i < mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.count; i++) {
		mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.data[i];

		//MPT_message, mp_table, dispatch either complete or subset of MPT_message table
		if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MPT_message) {
			mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;

			//dispatched when message_id >= 0x11 (17) && message_id <= 0x19 (31)
			if(mmt_signalling_message_header_and_payload->message_header.message_id >= MPT_message_start && mmt_signalling_message_header_and_payload->message_header.message_id < MPT_message_end) {
				__MMSM_TRACE("mmt_signalling_message_process_with_context: partial mp_table, message_id: 0x%02x", mmt_signalling_message_header_and_payload->message_header.message_id);
				atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_subset(mp_table);

			} else if(mmt_signalling_message_header_and_payload->message_header.message_id == MPT_message_end) {
				__MMSM_TRACE("mmt_signalling_message_process_with_context: complete mp_table, message_id: 0x%02x", mmt_signalling_message_header_and_payload->message_header.message_id);
				atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_complete(mp_table);

			} else {
				__MMSM_ERROR("mmt_signalling_message_process_with_context: MESSAGE_id_type == MPT_message but message_id not in bounds: 0x%02x", mmt_signalling_message_header_and_payload->message_header.message_id);
			}

			if(mp_table->number_of_assets) {
				for(int i=0; i < mp_table->number_of_assets; i++) {
					//slight hack, check the asset types and default_asset = 1
					mp_table_asset_row_t* mp_table_asset_row = &mp_table->mp_table_asset_row[i];

					uint32_t* mpu_sequence_number_p 			 = NULL;
					uint64_t* mpu_presentation_time_ntp64_p 	 = NULL;
					uint32_t  mpu_presentation_time_seconds 	 = 0;
					uint32_t  mpu_presentation_time_microseconds = 0;

					//try and extract mpu_presentation_timestamp first
					if(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor && mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n) {
						__MMSM_DEBUG("MPT message: checking packet_id: %u, mmt_signalling_message_mpu_timestamp_descriptor count is: %u",
								mp_table_asset_row->mmt_general_location_info.packet_id,
								mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n);

						for(int l=0; l < mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n; l++) {
							mmt_signalling_message_mpu_tuple_t* mmt_signaling_message_mpu_tuple = &mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[l];

							mpu_sequence_number_p = &mmt_signaling_message_mpu_tuple->mpu_sequence_number;
							mpu_presentation_time_ntp64_p = &mmt_signaling_message_mpu_tuple->mpu_presentation_time;

							compute_ntp64_to_seconds_microseconds(mmt_signaling_message_mpu_tuple->mpu_presentation_time, &mpu_presentation_time_seconds, &mpu_presentation_time_microseconds);

							__MMSM_DEBUG("packet_id: %u, mpu_sequence_number: %u to mpu_presentation_time: %llu, seconds: %u, ms: %u",
									mp_table_asset_row->mmt_general_location_info.packet_id,
									mmt_signaling_message_mpu_tuple->mpu_sequence_number,
									mmt_signaling_message_mpu_tuple->mpu_presentation_time,
									mpu_presentation_time_seconds,
									mpu_presentation_time_microseconds);
						}
					}

					//resolve packet_id's to matching essence types
					__MMSM_DEBUG("MPT message: checking packet_id: %u, asset_type: %s, default: %u, identifier: %s", mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					//mp_table_asset_row->asset_type == HEVC
					if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID, mp_table_asset_row->asset_type, 4) == 0) {
						atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_essence_packet_id(mp_table_asset_row->mmt_general_location_info.packet_id);
						if(mpu_sequence_number_p && mpu_presentation_time_ntp64_p) {
							atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor(mp_table_asset_row->mmt_general_location_info.packet_id,
									*mpu_sequence_number_p,
									*mpu_presentation_time_ntp64_p,
									mpu_presentation_time_seconds,
									mpu_presentation_time_microseconds);
						}
						__MMSM_DEBUG("MPT message: mmtp_flow: %p, setting video_packet_id: packet_id: %u, asset_type: %s, default: %u, identifier: %s",
								atsc3_mmt_mfu_context->mmtp_flow,
								mp_table_asset_row->mmt_general_location_info.packet_id,
								mp_table_asset_row->asset_type,
								mp_table_asset_row->default_asset_flag,
								mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					} else if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MP4A_ID, mp_table_asset_row->asset_type, 4) == 0 || strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_AC_4_ID, mp_table_asset_row->asset_type, 4) == 0) {
						//mp_table_asset_row->asset_type ==  MP4A || AC-4
						atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_essence_packet_id(mp_table_asset_row->mmt_general_location_info.packet_id);
						if(mpu_sequence_number_p && mpu_presentation_time_ntp64_p) {
							atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor(mp_table_asset_row->mmt_general_location_info.packet_id,
									*mpu_sequence_number_p,
									*mpu_presentation_time_ntp64_p,
									mpu_presentation_time_seconds,
									mpu_presentation_time_microseconds);
						}
						//matching_lls_sls_mmt_session->audio_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
						__MMSM_DEBUG("MPT message: mmtp_flow: %p, setting audio_packet_id: packet_id: %u, asset_type: %s, default: %u, identifier: %s",
								atsc3_mmt_mfu_context->mmtp_flow,
								mp_table_asset_row->mmt_general_location_info.packet_id,
								mp_table_asset_row->asset_type,
								mp_table_asset_row->default_asset_flag,
								mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					}
				}
			}
		} else {
			__MMSM_INFO("mmt_signalling_message_update_lls_sls_mmt_session: Ignoring signal: 0x%x", mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type);
		}
	}

}


