/*
 * atsc3_mmt_reconstitution_from_media_sample.c
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#include "atsc3_mmt_reconstitution_from_media_sample.h"

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

            if(lls_slt_monitor && lls_slt_monitor->lls_sls_mmt_monitor && lls_slt_monitor->lls_sls_mmt_monitor->service_id == matching_lls_slt_mmt_session->service_id) {

                __TRACE("Starting processing loop, current mpu_sequence_number is: %d, packet_sequence_number: %d", mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number, mmtp_payload->mmtp_mpu_type_packet_header.packet_sequence_number);

                udp_flow_packet_id_mpu_sequence_tuple_t* last_flow_reference = udp_flow_latest_mpu_sequence_number_add_or_replace(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_payload);
                __TRACE("update_last_packet_id_and_mpu_sequence_number: ptr: %p, last dst_ip_addr: %u, last dst_port: %hu, last packet_id: %u, last mpu_sequence_number: %u",
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
                       __INFO("Starting re-fragmenting because packet_id:mpu_sequence number changed, from a: %u:%u, v: %u:%u with %u:%u, processing a: %u:%u, v: %u:%u, min: %u",
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


                        //major refactoring
                       lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer_final_muxed_payload = atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_mpu_sequence_numbers(&udp_packet->udp_flow, udp_flow_latest_mpu_sequence_number_container, matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number-2, matching_lls_slt_mmt_session->to_process_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number-2, mmtp_sub_flow_vector, lls_slt_monitor->lls_sls_mmt_monitor);

                        if(lls_sls_monitor_output_buffer_final_muxed_payload) {
                            //mark both of these flows as having been processed
                            matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed = true;
                            matching_lls_slt_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video_processed = true;


                            if(true || lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
                                //todo, call atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box with forced init box
                            	//lls_sls_monitor_output_buffer_final_muxed_payload
                            	lls_sls_monitor_output_buffer_file_dump(lls_sls_monitor_output_buffer_final_muxed_payload, "mpu/", min_mpu_sequence_number);

                            }

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
                    __INFO("Updating audio packet_id: %u, matching_lls_slt_mmt_session: %p from %u to %u, to_process: %u, is_processed: %u",
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
                    __INFO("Updating video packet_id: %u, matching_lls_slt_mmt_session: %p from %u to %u, to_process: %u, is_processed: %u",
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
                __TRACE("mmtp_sub_flow was null, now: %p, resolved from sub_flow_vector and packet_id: %d",
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
                            //            __INFO("Beginning eviction pass for mpu: %u, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size: %lu", udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size)
                            int evicted_count = atsc3_mmt_mpu_clear_data_unit_payload_fragments(mmtp_sub_flow, mpu_fragments, data_unit_payload_fragments);
                            //            __INFO("Eviction pass for mpu: %u resulted in %u", udp_flow_last_packet_id_mpu_sequence_id->mpu_sequence_number_evict_range_start, evicted_count);
                        }
                    }
                }
            }
#endif





    } else if(mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x2) {

            global_stats->packet_counter_mmt_signaling++;
            __INFO("mmtp_packet_parse: processing mmt flow: %d.%d.%d.%d:(%u) packet_id: 0, signalling message", __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port));
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

            				__INFO("MPT message: checking packet_id: %u, asset_type: %u, default: %u, identifier: %s", mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");
            				if(mp_table_asset_row->identifier_mapping.asset_id.asset_id && strncasecmp("video", (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id, 5) == 0) {
            					matching_lls_slt_mmt_session->video_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
            				} else if(mp_table_asset_row->identifier_mapping.asset_id.asset_id && strncasecmp("audio", (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id, 5) == 0) {
            					matching_lls_slt_mmt_session->audio_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
            				}
            			}
            		}


            	} else {
            		__INFO("mmtp_packet_parse: Ignoring signal: 0x%x", mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type);
            	}
            }

    } else {
		_MMTP_WARN("mmtp_packet_parse: unknown payload type of 0x%x", mmtp_payload->mmtp_packet_header.mmtp_payload_type);
		global_stats->packet_counter_mmt_unknown++;
		goto cleanup;
    }

cleanup:

 //   mmtp_payload_fragments_union_free(mmtp_payload_p);
    mmtp_payload_p = NULL;

ret:
    return mmtp_payload;

}
