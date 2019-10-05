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

#include "atsc3_mmt_reconstitution_from_media_sample.h"

int _MMT_RECON_FROM_SAMPLE_SIGNAL_INFO_ENABLED = 0;
int _MMT_RECON_FROM_SAMPLE_DEBUG_ENABLED = 0;
int _MMT_RECON_FROM_SAMPLE_TRACE_ENABLED = 0;
//
//static atsc3_global_statistics_t global_stats_internal;
//atsc3_global_statistics_t* atsc3_global_statistics = &global_stats_internal;


/**
 * jjustman-2019-03-30 - combine pending mpu_sequence_numbers until we have at least 1a and 1v packet to flush for decoder...
mmtp_sub_flow_vector_t* mmtp_sub_flow_vector,
		mmtp_payload_fragments_union_t** mmtp_payload_p,
 
 TODO: refactor udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container for mpu recon storage

 //jjustman-2019-08-31 - new atsc3_mmtp_packet_types.h refactoring here...
 */

//if we don't have a match, we will free mmtp_mmpu_packet and return NULL
mmtp_mpu_packet_t* mmtp_process_from_payload(mmtp_mpu_packet_t* mmtp_mpu_packet,
                               mmtp_flow_t *mmtp_flow,
                               lls_slt_monitor_t* lls_slt_monitor,
                               udp_packet_t *udp_packet,
                               udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,
                               lls_sls_mmt_session_t* matching_lls_sls_mmt_session) {

    //forward declare so our goto will compile
    mmtp_asset_flow_t* mmtp_asset_flow = NULL;
    mmtp_asset_t* mmtp_asset = NULL;

    mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container = NULL;
    mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = NULL;
    
    if(!lls_slt_monitor || !lls_slt_monitor->lls_sls_mmt_monitor || !(lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id == matching_lls_sls_mmt_session->service_id)) {
        goto packet_cleanup;
    }
    
    //clear out our last flow processed status
    matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed = false;
    matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed = false;

	//slightly more complex than needed
	//jjustman-2019-10-05: TODO - refactor this selector logic for mmtp_packet_id_packets_container

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
            
            if(lls_slt_monitor && lls_slt_monitor->lls_sls_mmt_monitor &&
            		lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id == matching_lls_sls_mmt_session->service_id &&
            		lls_slt_monitor->lls_sls_mmt_monitor->lls_mmt_session->sls_destination_ip_address == udp_packet->udp_flow.dst_ip_addr &&
            		lls_slt_monitor->lls_sls_mmt_monitor->lls_mmt_session->sls_destination_udp_port == udp_packet->udp_flow.dst_port) {

            	udp_flow_packet_id_mpu_sequence_tuple_t* last_flow_reference = udp_flow_latest_mpu_sequence_number_add_or_replace_and_check_for_rollover(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_mpu_packet, lls_slt_monitor, matching_lls_sls_mmt_session, mmtp_flow);

            	//see if we are an audio packet that rolled over
				if(lls_slt_monitor->lls_sls_mmt_monitor->audio_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
					//see if we have incremented our mpu_sequence_number with current mmtp_payload

					if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
						lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff_pending_mux = atsc3_isobmff_build_raw_mpu_from_single_sequence_number(&udp_packet->udp_flow,
														   udp_flow_latest_mpu_sequence_number_container,
														   matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id,
														   matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
														   &lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff,
                                                           mmtp_packet_id_packets_container);

						lls_sls_monitor_buffer_isobmff_intermediate_mmt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
																				lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id,
                                                                                 mmtp_mpu_packet->mmtp_packet_id,
                                                                                 matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
                                                                                 "a.orig");

						atsc3_isobmff_rebuild_track_mpu_from_sample_data(lls_sls_monitor_buffer_isobmff_pending_mux);
						ls_sls_monitor_buffer_isobmff_mmt_mpu_rebuilt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
																				lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id,
																				matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
																				"a.rebuilt");
                        
                        mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number);
                        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
                        //jjustman-2019-09-05 - this might be too agressive here
                        //mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);
                    }
                    //keep track of our last mpu_sequence_number...
					udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio, last_flow_reference);
                    
				} else if(lls_slt_monitor->lls_sls_mmt_monitor->video_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
					//see if we have incremented our mpu_sequence_number with current mmtp_payload

					if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
                        
						lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff_pending_mux = atsc3_isobmff_build_raw_mpu_from_single_sequence_number(&udp_packet->udp_flow,
														   udp_flow_latest_mpu_sequence_number_container,
														   matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id,
														   matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
														   &lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff,
                                                           mmtp_packet_id_packets_container);

						lls_sls_monitor_buffer_isobmff_intermediate_mmt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
																				  lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id,
																				  mmtp_mpu_packet->mmtp_packet_id,
																				  matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
																				  "v.orig");
						atsc3_isobmff_rebuild_track_mpu_from_sample_data(lls_sls_monitor_buffer_isobmff_pending_mux);
						ls_sls_monitor_buffer_isobmff_mmt_mpu_rebuilt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
																				lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id,
																				matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
																				"v.rebuilt");
                        mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number);
                        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
                        //jjustman-2019-09-05 - this might be too agressive here
                        //mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);

                    }

					//keep track of our last mpu_sequence_number...
           			udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video, last_flow_reference);
				}

				//if we have at least one block in the following, push it to the isobmff track joiner as usual
				if(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mmt_mpu_rebuilt_and_appending_for_isobmff_mux &&
                   lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mmt_mpu_rebuilt_and_appending_for_isobmff_mux) {

                    lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer_final_muxed_payload = atsc3_isobmff_build_joined_mmt_rebuilt_boxes(&lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer);
                    
                    if(lls_sls_monitor_output_buffer_final_muxed_payload) {
                        //mark both of these flows as having been processed
                        matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed = true;
                        matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed = true;
                        

                        if(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
                            //lls_sls_monitor_output_buffer_final_muxed_payload
                            lls_sls_monitor_output_buffer_mmt_file_dump(lls_sls_monitor_output_buffer_final_muxed_payload, "mpu/",
                                matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
                                matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number);
                        }

                        //http support output
                        if(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_enabled &&
                                lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer &&
                                lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_output_conntected) {
                            //&& lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer) {

                            lls_sls_monitor_reader_mutex_lock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_mutex);
                            lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->total_fragments_incoming_written++;

                            if(!lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming) {
                                lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming =  block_Duplicate(lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block);
                            } else {
                                block_Resize(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming, lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming->p_size + lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_size);
                                block_Write(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_incoming, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_size);
                            }

                            lls_sls_monitor_reader_mutex_unlock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_mutex);
                        }

                        //ffplay pipe output
                        if(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled && lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer) {

                            pipe_buffer_reader_mutex_lock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);

                            pipe_buffer_unsafe_push_block(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->p_buffer, lls_sls_monitor_output_buffer_final_muxed_payload->joined_isobmff_block->i_pos);

                            lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.has_written_init_box = true;

                            pipe_buffer_notify_semaphore_post(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);

                            //check to see if we have shutdown
                            lls_slt_monitor_check_and_handle_pipe_ffplay_buffer_is_shutdown(lls_slt_monitor);

                            pipe_buffer_reader_mutex_unlock(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);
                        }

                        lls_sls_monitor_output_buffer_reset_rebuilt_mpu_moof_and_fragment_position(&lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer);
                    }
                    //jjustman-2019-09-05 - TODO: fix this hack
                    //clear out packet_id=0
                    //mmtp_mpu_packet = NULL;
                    mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_new();
                    mmtp_mpu_packet->mmtp_packet_id = 0;

                    mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container_packet_zero = mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset, mmtp_mpu_packet);
                    mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container_packet_zero);
                }
            }
        } else {
            //non-timed
            atsc3_global_statistics->packet_counter_mmt_nontimed_mpu++;
        }
        
        goto ret;
        
    } else if(mmtp_mpu_packet->mmtp_payload_type == 0x2) {

		atsc3_global_statistics->packet_counter_mmt_signaling++;
		__MMT_RECON_FROM_SAMPLE_SIGNAL_INFO("mmtp_process_from_payload: processing mmt flow: %d.%d.%d.%d:(%u) packet_id: 0, signalling message", __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port));
		
    } else {
		__MMT_RECON_FROM_SAMPLE_WARN("mmtp_process_from_payload: unknown payload type of 0x%x", mmtp_mpu_packet->mmtp_payload_type);
		atsc3_global_statistics->packet_counter_mmt_unknown++;
		goto packet_cleanup;
    }

packet_cleanup:
    if(mmtp_mpu_packet) {
        __MMT_RECON_FROM_SAMPLE_TRACE("Cleaning up packet: %p", mmtp_mpu_packet);
        mmtp_mpu_packet_free(&mmtp_mpu_packet);
    }
    return NULL;

ret:
    return mmtp_mpu_packet;

}

