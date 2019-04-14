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

/**
 * jjustman-2019-03-30 - combine pending mpu_sequence_numbers until we have at least 1a and 1v packet to flush for decoder...
 */
mmtp_payload_fragments_union_t* mmtp_process_from_payload(mmtp_sub_flow_vector_t* mmtp_sub_flow_vector,
		udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,
		lls_slt_monitor_t* lls_slt_monitor,
		udp_packet_t *udp_packet,
		mmtp_payload_fragments_union_t** mmtp_payload_p,
		lls_sls_mmt_session_t* matching_lls_slt_mmt_session) {

    mmtp_payload_fragments_union_t* mmtp_payload = * mmtp_payload_p;

    mpu_data_unit_payload_fragments_t* data_unit_payload_types = NULL;
    mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments = NULL; //technically this is mpu_fragments->media_fragment_unit_vector
    mpu_data_unit_payload_fragments_t* mpu_metadata_fragments =    NULL;
    mpu_data_unit_payload_fragments_t* movie_metadata_fragments  = NULL;
    mmtp_sub_flow_t* mmtp_sub_flow = NULL;

    //dump header, then dump applicable packet type
    //mmtp_packet_header_dump(mmtp_payload);

    if(mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x0) {
        global_stats->packet_counter_mmt_mpu++;

        if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_timed_flag == 1) {
            global_stats->packet_counter_mmt_timed_mpu++;

            if(!lls_slt_monitor || !lls_slt_monitor->lls_sls_mmt_monitor || !(lls_slt_monitor->lls_sls_mmt_monitor->service_id == matching_lls_slt_mmt_session->service_id)) {
                goto packet_cleanup;
            }
            
            if(lls_slt_monitor && lls_slt_monitor->lls_sls_mmt_monitor && lls_slt_monitor->lls_sls_mmt_monitor->service_id == matching_lls_slt_mmt_session->service_id) {

            	udp_flow_packet_id_mpu_sequence_tuple_t* last_flow_reference = udp_flow_latest_mpu_sequence_number_add_or_replace(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_payload);

            	//see if we are an audio packet that rolled over
				if(lls_slt_monitor->lls_sls_mmt_monitor->audio_packet_id == mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id) {
					//see if we have incremented our mpu_sequence_number with current mmtp_payload

					if(matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio && matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number < mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number) {
						lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff_pending_mux = atsc3_isobmff_build_raw_mpu_from_single_sequence_number(&udp_packet->udp_flow,
														   udp_flow_latest_mpu_sequence_number_container,
														   matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id,
														   matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
														   mmtp_sub_flow_vector,
														   &lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff);


						lls_sls_monitor_buffer_isobmff_intermediate_mmt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
								matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
								"a.orig");

						atsc3_isobmff_rebuild_track_mpu_from_sample_data(lls_sls_monitor_buffer_isobmff_pending_mux);
						ls_sls_monitor_buffer_isobmff_mmt_mpu_rebuilt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
														matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
														"a.rebuilt");

					}
					udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio, last_flow_reference);

				} else if(lls_slt_monitor->lls_sls_mmt_monitor->video_packet_id == mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id) {
					//see if we have incremented our mpu_sequence_number with current mmtp_payload

					if(matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video && matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number < mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number) {
                        
						lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff_pending_mux = atsc3_isobmff_build_raw_mpu_from_single_sequence_number(&udp_packet->udp_flow,
														   udp_flow_latest_mpu_sequence_number_container,
														   matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id,
														   matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
														   mmtp_sub_flow_vector,
														   &lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff);

						lls_sls_monitor_buffer_isobmff_intermediate_mmt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
														matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
														"v.orig");
						atsc3_isobmff_rebuild_track_mpu_from_sample_data(lls_sls_monitor_buffer_isobmff_pending_mux);
						ls_sls_monitor_buffer_isobmff_mmt_mpu_rebuilt_file_dump(lls_sls_monitor_buffer_isobmff_pending_mux, "mpu/",
														matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
														"v.rebuilt");

					} else {
						//noop
					}

					//keep track of our last mpu_sequence_number...
           			udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video, last_flow_reference);
				}

				//if we have at least one block in the following, push it to the isobmff track joiner as usual
				if(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mmt_mpu_rebuilt &&
						lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mmt_mpu_rebuilt) {


				lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer_final_muxed_payload = atsc3_isobmff_build_joined_mmt_rebuilt_boxes(&lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer);
                    
                    
				if(lls_sls_monitor_output_buffer_final_muxed_payload) {
					//mark both of these flows as having been processed
					matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed = true;
					matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed = true;


					if(true || lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
						//todo, call atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box with forced init box
						//lls_sls_monitor_output_buffer_final_muxed_payload
						lls_sls_monitor_output_buffer_mmt_file_dump(lls_sls_monitor_output_buffer_final_muxed_payload, "mpu/",
							matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
							matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number);
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
                
                    atsc3_mmt_reconstitution_free_from_udp_flow(mmtp_sub_flow_vector, matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio);
                    atsc3_mmt_reconstitution_free_from_udp_flow(mmtp_sub_flow_vector, matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video);
                    
                 	matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed = true;
					matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed = true;

                    }
                }
            }
        } else {
            //non-timed
            global_stats->packet_counter_mmt_nontimed_mpu++;
        }
        
        goto ret;
        
    } else if(mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x2) {

		global_stats->packet_counter_mmt_signaling++;
		__MMT_RECON_FROM_SAMPLE_SIGNAL_INFO("mmtp_packet_parse: processing mmt flow: %d.%d.%d.%d:(%u) packet_id: 0, signalling message", __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port));
		//mmtp_payload->mmtp_signalling_message_fragments.payload

		signaling_message_dump(mmtp_payload);
		for(int i=0; i < mmtp_payload->mmtp_signalling_message_fragments.mmt_signalling_message_vector.messages_n; i++) {
			mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmtp_payload->mmtp_signalling_message_fragments.mmt_signalling_message_vector.messages[i];
			if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MPT_message) {
				mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;
				//update our lls_sls_mmt_session
				if(matching_lls_slt_mmt_session && mp_table->number_of_assets) {
					for(int i=0; i < mp_table->number_of_assets; i++) {
						//slight hack, check the asset types and default_asset = 1
						mp_table_asset_row_t* mp_table_asset_row = &mp_table->mp_table_asset_row[i];

						__MMT_RECON_FROM_SAMPLE_SIGNAL_INFO("MPT message: checking packet_id: %u, asset_type: %s, default: %u, identifier: %s", mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");
						if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID, mp_table_asset_row->asset_type, 4) == 0) {
							matching_lls_slt_mmt_session->video_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
						} else if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MP4A_ID, mp_table_asset_row->asset_type, 4) == 0 || strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_AC_4_ID, mp_table_asset_row->asset_type, 4) == 0) {
							matching_lls_slt_mmt_session->audio_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
						}
					}
				}
                goto ret;
			} else {
				__MMT_RECON_FROM_SAMPLE_SIGNAL_INFO("mmtp_packet_parse: Ignoring signal: 0x%x", mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type);
			}
		}

    } else {
		__MMT_RECON_FROM_SAMPLE_WARN("mmtp_packet_parse: unknown payload type of 0x%x", mmtp_payload->mmtp_packet_header.mmtp_payload_type);
		global_stats->packet_counter_mmt_unknown++;
		goto packet_cleanup;
    }

packet_cleanup:

    mmtp_payload_fragments_union_free(mmtp_payload_p);
    mmtp_payload_p = NULL;

ret:
    return mmtp_payload;

}


void atsc3_mmt_reconstitution_free_from_udp_flow(mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple_t* last_udp_flow_packet_id_mpu_sequence_tuple) {
    //reap...clear out our "global" packet_id data_unit_payloads from the mpu fragments
    mpu_fragments_t* mpu_fragments = NULL;
    mmtp_sub_flow_t* mmtp_sub_flow = NULL;
    mpu_data_unit_payload_fragments_t* data_unit_payload_types = NULL;
    mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments = NULL;

    if(!mmtp_sub_flow) {
        //try and find our packet_id subflow to clean up any intermediate objects
        mmtp_sub_flow = mmtp_sub_flow_vector_get_or_set_packet_id(mmtp_sub_flow_vector, last_udp_flow_packet_id_mpu_sequence_tuple->packet_id);
        __MMT_RECON_FROM_SAMPLE_TRACE("mmtp_sub_flow was null, now: %p, resolved from sub_flow_vector and packet_id: %d",
                                      mmtp_sub_flow,
                                      last_udp_flow_packet_id_mpu_sequence_tuple->packet_id);
    }
    
    if(mmtp_sub_flow) {
        mpu_fragments = mpu_fragments_get_or_set_packet_id(mmtp_sub_flow, last_udp_flow_packet_id_mpu_sequence_tuple->packet_id);
        data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mpu_fragments->media_fragment_unit_vector, last_udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number);
        
        if(data_unit_payload_types && data_unit_payload_types->timed_fragments_vector.data) {
            data_unit_payload_fragments = &data_unit_payload_types->timed_fragments_vector;
            if(data_unit_payload_fragments) {
                __MMT_RECON_FROM_SAMPLE_INFO("Beginning eviction pass for mpu: %u, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size: %lu", last_udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size)
                int evicted_count = atsc3_mmt_mpu_clear_data_unit_payload_fragments(mmtp_sub_flow, mpu_fragments, data_unit_payload_fragments);
                __MMT_RECON_FROM_SAMPLE_INFO("Eviction pass for mpu: %u resulted in %u", last_udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number, evicted_count);
            }
        }
    }
}


#ifdef __not_used

mmtp_payload_fragments_union_t* mmtp_process_from_payload_synchronous_mpu_sequence_number(mmtp_sub_flow_vector_t* mmtp_sub_flow_vector,
		udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,
		lls_slt_monitor_t* lls_slt_monitor,
		udp_packet_t *udp_packet,
		mmtp_payload_fragments_union_t** mmtp_payload_p,
		lls_sls_mmt_session_t* matching_lls_slt_mmt_session) {

    mmtp_payload_fragments_union_t* mmtp_payload = * mmtp_payload_p;

    mpu_data_unit_payload_fragments_t* data_unit_payload_types = NULL;
    mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments = NULL; //technically this is mpu_fragments->media_fragment_unit_vector
    mpu_data_unit_payload_fragments_t* mpu_metadata_fragments =    NULL;
    mpu_data_unit_payload_fragments_t* movie_metadata_fragments  = NULL;
    mmtp_sub_flow_t* mmtp_sub_flow = NULL;

    //dump header, then dump applicable packet type
    //mmtp_packet_header_dump(mmtp_payload);

    if(mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x0) {
        global_stats->packet_counter_mmt_mpu++;

        if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_timed_flag == 1) {
            global_stats->packet_counter_mmt_timed_mpu++;

            if(lls_slt_monitor && lls_slt_monitor->lls_sls_mmt_monitor && lls_slt_monitor->lls_sls_mmt_monitor->service_id == matching_lls_slt_mmt_session->service_id) {

                __MMT_RECON_FROM_SAMPLE_TRACE("Starting processing loop, current mpu_sequence_number is: %d, packet_sequence_number: %d", mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number, mmtp_payload->mmtp_mpu_type_packet_header.packet_sequence_number);

                udp_flow_packet_id_mpu_sequence_tuple_t* last_flow_reference = udp_flow_latest_mpu_sequence_number_add_or_replace(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_payload);
                __MMT_RECON_FROM_SAMPLE_TRACE("update_last_packet_id_and_mpu_sequence_number: ptr: %p, last dst_ip_addr: %u, last dst_port: %hu, last packet_id: %u, last mpu_sequence_number: %u",
                        last_flow_reference,
                        last_flow_reference->udp_flow.dst_ip_addr,
                        last_flow_reference->udp_flow.dst_port,
                        last_flow_reference->packet_id,
                        last_flow_reference->mpu_sequence_number);

// this happens above also...
//                udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_last_packet_id_mpu_sequence_id = udp_flow_latest_mpu_sequence_number_from_packet_id(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id);


                //this is pretty large, we could also query udp_flow_latest_mpu_sequence_number_from_packet_id for both of our monitored packet id's
                if( matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio &&
                    matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio &&
                   !matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed &&
                    matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video &&
                    matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video &&
                   !matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed &&
                   (
                    (lls_slt_monitor->lls_sls_mmt_monitor->audio_packet_id == matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id &&
                     mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id == matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id && matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number < mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number) ||

                    (lls_slt_monitor->lls_sls_mmt_monitor->video_packet_id == matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id &&
                     mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id == matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id && matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number < mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number)
                    )) {

                       uint32_t min_mpu_sequence_number = __MIN(matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number, matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number);
                       __MMT_RECON_FROM_SAMPLE_INFO("Starting re-fragmenting because packet_id:mpu_sequence number changed, from a: %u:%u, v: %u:%u with %u:%u, processing a: %u:%u, v: %u:%u, min: %u",
                              matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id,
                              matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
                              matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id,
                              matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
                              mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
                              mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number,
                              matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio->packet_id,
                              matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number,
                              matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video->packet_id,
                              matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number,
                              min_mpu_sequence_number
                              );

                       int mpu_sequence_number_offset = -2;
                        //major refactoring
                       //TODO - use the proper decoding time instead of mpu_sequence_number
                       lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer_final_muxed_payload = atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_mpu_sequence_numbers(&udp_packet->udp_flow,
                    		   udp_flow_latest_mpu_sequence_number_container,
                    		   matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number + mpu_sequence_number_offset,
                    		   matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number + mpu_sequence_number_offset,
							   mmtp_sub_flow_vector, lls_slt_monitor->lls_sls_mmt_monitor);

                        if(lls_sls_monitor_output_buffer_final_muxed_payload) {
                            //mark both of these flows as having been processed
                            matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed = true;
                            matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed = true;


                            if(true || lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
                                //todo, call atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box with forced init box
                            	//lls_sls_monitor_output_buffer_final_muxed_payload
                            	lls_sls_monitor_output_buffer_file_dump(lls_sls_monitor_output_buffer_final_muxed_payload, "mpu/",
                            		matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number + mpu_sequence_number_offset,
                             		matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number + mpu_sequence_number_offset);
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
                        }
                    }


                //update our last references for mpu_sequence rollover until we process packet_id signaling messages only if our mpu_sequence_number has changed due to malloc/copy the flow reference
                if(lls_slt_monitor->lls_sls_mmt_monitor->audio_packet_id == last_flow_reference->packet_id &&
                   (!matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio || matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number != last_flow_reference->mpu_sequence_number )) {

                    matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed = false;
                    if(matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio) {
                        udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio, matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio);
                    }
                    __MMT_RECON_FROM_SAMPLE_INFO("Updating audio packet_id: %u, matching_lls_slt_mmt_session: %p from %u to %u, to_process: %u, is_processed: %u",
                           last_flow_reference->packet_id,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio ? matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number : -1,
                           last_flow_reference->mpu_sequence_number,
                           matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio ? matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number : -1,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed
                           );

                    udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio, last_flow_reference);

                } else if(lls_slt_monitor->lls_sls_mmt_monitor->video_packet_id == last_flow_reference->packet_id &&
                          (!matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video || matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number != last_flow_reference->mpu_sequence_number )) {

                    matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed = false;
                    if(matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video) {
                        udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video, matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video);
                    }
                    __MMT_RECON_FROM_SAMPLE_INFO("Updating video packet_id: %u, matching_lls_slt_mmt_session: %p from %u to %u, to_process: %u, is_processed: %u",
                           last_flow_reference->packet_id,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video ? matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number : -1 ,
                           last_flow_reference->mpu_sequence_number,
                           matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video ? matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number : -1,
                           matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed
                           );



                    udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video, last_flow_reference);
                }

            }
        } else {
            //non-timed
            global_stats->packet_counter_mmt_nontimed_mpu++;
        }



#ifdef __REAP
            //only perform evictions if our last_mpu and last_packet are different than the last eviction run...
            if(!udp_flow_last_packet_id_mpu_sequence_id || !udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_last_refragmentation_flush || (udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_last_refragmentation_flush - udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start <= 0)) {
                //bail on reaping this time...
                goto cleanup;
            }

            //clear out our "global" packet_id data_unit_payloads from the mpu fragments
            mpu_fragments_t* mpu_fragments = NULL;
            if(!mmtp_sub_flow) {
                //try and find our packet_id subflow to clean up any intermediate objects
                mmtp_sub_flow = mmtp_sub_flow_vector_get_or_set_packet_id(mmtp_sub_flow_vector, udp_flow_last_packet_id_mpu_sequence_id->packet_id);
                __MMT_RECON_FROM_SAMPLE_TRACE("mmtp_sub_flow was null, now: %p, resolved from sub_flow_vector and packet_id: %d",
                        mmtp_sub_flow,
                        udp_flow_last_packet_id_mpu_sequence_id->packet_id);
            }

            if(mmtp_sub_flow) {
                mpu_fragments = mpu_fragments_get_or_set_packet_id(mmtp_sub_flow, udp_flow_last_packet_id_mpu_sequence_id->packet_id);
            }

            if(mpu_fragments && udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start) {
                for(; udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start < udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_last_refragmentation_flush; udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start++) {
                    data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mpu_fragments->media_fragment_unit_vector, udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start);

                    if(data_unit_payload_types && data_unit_payload_types->timed_fragments_vector.data) {
                        data_unit_payload_fragments = &data_unit_payload_types->timed_fragments_vector;
                        if(data_unit_payload_fragments) {
                            //            __MMT_RECON_FROM_SAMPLE_INFO("Beginning eviction pass for mpu: %u, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size: %lu", udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size)
                            int evicted_count = atsc3_mmt_mpu_clear_data_unit_payload_fragments(mmtp_sub_flow, mpu_fragments, data_unit_payload_fragments);
                            //            __MMT_RECON_FROM_SAMPLE_INFO("Eviction pass for mpu: %u resulted in %u", udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start, evicted_count);
                        }
                    }
                }
            }
#endif




        

    } else if(mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x2) {

		global_stats->packet_counter_mmt_signaling++;
		__MMT_RECON_FROM_SAMPLE_INFO("mmtp_packet_parse: processing mmt flow: %d.%d.%d.%d:(%u) packet_id: 0, signalling message", __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port));
		//mmtp_payload->mmtp_signalling_message_fragments.payload

		signaling_message_dump(mmtp_payload);
		for(int i=0; i < mmtp_payload->mmtp_signalling_message_fragments.mmt_signalling_message_vector.messages_n; i++) {
			mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmtp_payload->mmtp_signalling_message_fragments.mmt_signalling_message_vector.messages[i];
			if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MPT_message) {
				mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;
				//update our lls_sls_mmt_session
				if(matching_lls_slt_mmt_session && mp_table->number_of_assets) {
					for(int i=0; i < mp_table->number_of_assets; i++) {
						//slight hack, check the asset types and default_asset = 1
						mp_table_asset_row_t* mp_table_asset_row = &mp_table->mp_table_asset_row[i];

						__MMT_RECON_FROM_SAMPLE_INFO("MPT message: checking packet_id: %u, asset_type: %u, default: %u, identifier: %s", mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");
						if(mp_table_asset_row->identifier_mapping.asset_id.asset_id && strncasecmp("video", (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id, 5) == 0) {
							matching_lls_slt_mmt_session->video_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
						} else if(mp_table_asset_row->identifier_mapping.asset_id.asset_id && strncasecmp("audio", (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id, 5) == 0) {
							matching_lls_slt_mmt_session->audio_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
						}
					}
				}

			} else {
				__MMT_RECON_FROM_SAMPLE_INFO("mmtp_packet_parse: Ignoring signal: 0x%x", mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type);
			}
		}

    } else {
		__MMT_RECON_FROM_SAMPLE_WARN("mmtp_packet_parse: unknown payload type of 0x%x", mmtp_payload->mmtp_packet_header.mmtp_payload_type);
		global_stats->packet_counter_mmt_unknown++;
		goto cleanup;
    }

cleanup:

 //   mmtp_payload_fragments_union_free(mmtp_payload_p);
    mmtp_payload_p = NULL;

ret:
    return mmtp_payload;

}

#endif
