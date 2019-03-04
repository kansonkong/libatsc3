/*
 * atsc3_isobmff_tools.c
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 */

#include "atsc3_isobmff_tools.h"
#include "bento4/ISOBMFFTrackJoiner.h"

int _ISOBMFF_TOOLS_DEBUG_ENABLED = 0;
int _ISOBMFF_TOOLS_TRACE_ENABLED = 0;
/**
 * query the corresponding packet id's to resolve our mpu_metadata and
 * rebuild our combined boxes and tracks
 */


/**

 see also: atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_mpu_sequence_numbers
 **/

lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_patched_isobmff_fragment(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	lls_sls_monitor_buffer_isobmff_moov_patch_mdat_box(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff);
	lls_sls_monitor_buffer_isobmff_moov_patch_mdat_box(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff);

	AP4_MemoryByteStream* ap4_memory_byte_stream;

	ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_boxes(lls_sls_monitor_output_buffer, &ap4_memory_byte_stream);

	if(!ap4_memory_byte_stream || !ap4_memory_byte_stream->GetDataSize()) {
		__ISOBMFF_TOOLS_ERROR("ISOBMFF_track_joiner: returned %p, size: %u, returning NULL", ap4_memory_byte_stream, ap4_memory_byte_stream != NULL ? ap4_memory_byte_stream->GetDataSize() : 0);
		return NULL;
	}
	__ISOBMFF_TOOLS_DEBUG("building return alloc of %u", ap4_memory_byte_stream->GetDataSize());

	if(!lls_sls_monitor_output_buffer->joined_isobmff_block) {
		lls_sls_monitor_output_buffer->joined_isobmff_block = block_Alloc(ap4_memory_byte_stream->GetDataSize());
	} else {
		block_Rewind(lls_sls_monitor_output_buffer->joined_isobmff_block);

		if(!block_Resize(lls_sls_monitor_output_buffer->joined_isobmff_block, ap4_memory_byte_stream->GetDataSize())) {
			block_Release(&lls_sls_monitor_output_buffer->joined_isobmff_block);
			__ISOBMFF_TOOLS_ERROR("ISOBMFF_track_joiner: block_Resize returned NULL for size: %u, freeing and returning NULL", ap4_memory_byte_stream->GetDataSize());
			return NULL;
		}
	}

	block_Write(lls_sls_monitor_output_buffer->joined_isobmff_block, (uint8_t*)ap4_memory_byte_stream->GetData(), ap4_memory_byte_stream->GetDataSize());
	free (ap4_memory_byte_stream);

	return lls_sls_monitor_output_buffer;
}

lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_flow(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, lls_sls_mmt_monitor_t* lls_sls_mmt_monitor) {

	lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer = &lls_sls_mmt_monitor->lls_sls_monitor_output_buffer;
	lls_sls_monitor_output_buffer_reset_moov_and_fragment_position(lls_sls_monitor_output_buffer);

	mpu_data_unit_payload_fragments_t* data_unit_payload_types = NULL;
	mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments = NULL; //technically this is mpu_fragments->media_fragment_unit_vector
	mpu_data_unit_payload_fragments_t* mpu_metadata_fragments =	NULL;
	mpu_data_unit_payload_fragments_t* movie_metadata_fragments  = NULL;
	mmtp_sub_flow_t* mmtp_sub_flow = NULL;
	int total_fragments = 0;

	block_t* mpu_metadata_output_block_t = NULL;

	udp_flow_latest_mpu_sequence_number_container_t* udp_flow_matching_flows = udp_flow_find_matching_flows(udp_flow_latest_mpu_sequence_number_container, udp_flow);

	if(!udp_flow_latest_mpu_sequence_number_container || !udp_flow_latest_mpu_sequence_number_container->udp_flows_n) {
		__ISOBMFF_TOOLS_ERROR("atsc3_isobmff_build_mpu_metadata_ftyp_box: Unable to find flows for MPU metadata creation from %u:%u", udp_flow->dst_ip_addr, udp_flow->dst_port);
	}

    //build out ftyp/moov init box
	__ISOBMFF_TOOLS_TRACE("atsc3_isobmff_build_mpu_metadata_ftyp_box: Starting to create MPU Metadata init box from flow: %u:%u", udp_flow->dst_ip_addr, udp_flow->dst_port);

	for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
		udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];
		bool found_mpu_metadata_fragment = false;

		__ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_mpu_metadata_ftyp_box: Searching for MPU Metadata with %u:%u and packet_id: %u", udp_flow->dst_ip_addr, udp_flow->dst_port, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

		mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

		if(mmtp_sub_flow && mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.size) {
			int all_mpu_metadata_fragment_walk_count = mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.size-1;

			//mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data ?
			while(all_mpu_metadata_fragment_walk_count >= 0 && !found_mpu_metadata_fragment) {
				mpu_metadata_fragments =  mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data[all_mpu_metadata_fragment_walk_count--];

				if(mpu_metadata_fragments) {
					mmtp_payload_fragments_union_t* mpu_metadata_fragment = NULL;

					//rebuild if we are fragmented
					for(int i=0; i < mpu_metadata_fragments->timed_fragments_vector.size; i++) {
						mpu_metadata_fragment = mpu_metadata_fragments->timed_fragments_vector.data[i];

						if(mpu_metadata_fragment && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos ) {

							if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->video_packet_id && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) {
								if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos) {
									//reset if we are not a fragment reassembly
									if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 0 || mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 1) {
										lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos = 0;
									}
								}
								lls_sls_monitor_output_buffer_copy_video_init_block(lls_sls_monitor_output_buffer, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
								found_mpu_metadata_fragment = true;
								__ISOBMFF_TOOLS_DEBUG("found init box for audio, mpu_metadata packet_id: %d, mpu_sequence_number: %u", mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_sequence_number);

							} else if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->audio_packet_id && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) {
								if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos) {
									//reset if we are not a fragment reassembly
									if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 0 || mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 1) {
										lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos = 0;
									}
								}
								lls_sls_monitor_output_buffer_copy_audio_init_block(lls_sls_monitor_output_buffer, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
								found_mpu_metadata_fragment = true;

								__ISOBMFF_TOOLS_DEBUG("found init box for audio, mpu_metadata packet_id: %d, mpu_sequence_number: %u, fragmentation_indicator: %u, fragmentation_counter: %u", mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_sequence_number, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_counter);
							}
						}
					}
				}
			}

			if(!found_mpu_metadata_fragment) {
				__ISOBMFF_TOOLS_WARN("Unable to find init box, mpu_metadata from mmt_flow, packet_id: %u", udp_flow_packet_id_mpu_sequence_tuple->packet_id);
				return NULL;
			}
		}
	}

	//sanity check to make sure we got our init box
	if(!lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.init_box_pos || !lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.init_box_pos) {
		__ISOBMFF_TOOLS_WARN("Returning - int box data unit size is null: audio_recon_fragment_size: %u, video_recon_fragment_size: %u",
				lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.init_box_pos,
				lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.init_box_pos);

		return NULL;
	}
    
	for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
        
		udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];

        //remember, subflows are built in case of DU fragmentation - korean MMT samples have this edge case
		mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);
        if(!mmtp_sub_flow) {
            __ISOBMFF_TOOLS_WARN("mmtp_sub_flow for packet_id: %u is null", udp_flow_packet_id_mpu_sequence_tuple->packet_id);
            //goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
            return NULL;
        }
		movie_metadata_fragments = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->movie_fragment_metadata_vector, udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number);

		mmtp_payload_fragments_union_t* fragment_metadata = NULL;
		if(movie_metadata_fragments && movie_metadata_fragments->timed_fragments_vector.size) {
            for(int i=0; i < movie_metadata_fragments->timed_fragments_vector.size; i++) {
                fragment_metadata = movie_metadata_fragments->timed_fragments_vector.data[i];
                __ISOBMFF_TOOLS_INFO("Movie Fragment Metadata: Found for fragment_metadata packet_id: %d, mpu_sequence_number: %u, fragmentation_indicator: %u, fragmentation_counter: %u", fragment_metadata->mmtp_packet_header.mmtp_packet_id, udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number, fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator, fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_counter);

                //rebuild if we are fragmented
                if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->video_packet_id) {
					lls_sls_monitor_output_buffer_copy_video_moov_block(lls_sls_monitor_output_buffer, fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                } else if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->audio_packet_id) {
                	lls_sls_monitor_output_buffer_copy_audio_moov_block(lls_sls_monitor_output_buffer, fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                }
            }
        } else {
            __ISOBMFF_TOOLS_WARN("Movie Fragment Metadata is NULL for packet_id: %u", udp_flow_packet_id_mpu_sequence_tuple->packet_id);
            //goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
            return NULL;
        }
    }
   
	 for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
		udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];

		mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

		//hack for the previous sequence number which should be complete
		data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->media_fragment_unit_vector, udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number);

	   mmtp_payload_fragments_union_t* mpu_data_unit_payload_fragments_timed = NULL;

		if(data_unit_payload_types && data_unit_payload_types->timed_fragments_vector.size) {
			total_fragments = data_unit_payload_types->timed_fragments_vector.size;
			//push to mpu_push_output_buffer
			for(int i=0; i < total_fragments; i++) {
				mmtp_payload_fragments_union_t* data_unit = data_unit_payload_types->timed_fragments_vector.data[i];

				//only push to our correct mpu flow....
				if(data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->video_packet_id && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) {
					lls_sls_monitor_output_buffer_copy_video_fragment_block(lls_sls_monitor_output_buffer, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
				} else if(data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->audio_packet_id && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id){
					lls_sls_monitor_output_buffer_copy_audio_fragment_block(lls_sls_monitor_output_buffer, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
				} else {
		            __ISOBMFF_TOOLS_WARN("data unit recon - unknown packet_id: %u", udp_flow_packet_id_mpu_sequence_tuple->packet_id);
		        }
			}
		} else {
			__ISOBMFF_TOOLS_WARN("data unit size is null");
			//goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
			return NULL;
		}
	}

    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos || !lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos) {
        __ISOBMFF_TOOLS_WARN("Returning - data unit size is null: video_du_size: %u, audio_du_size: %u", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);
        return NULL;
    }

    return atsc3_isobmff_build_joined_patched_isobmff_fragment(lls_sls_monitor_output_buffer);
}

lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_mpu_sequence_numbers(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, uint32_t mpu_sequence_number_audio, uint32_t mpu_sequence_number_video, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, lls_sls_mmt_monitor_t* lls_sls_mmt_monitor) {
    
	lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer = &lls_sls_mmt_monitor->lls_sls_monitor_output_buffer;
	lls_sls_monitor_output_buffer_reset_moov_and_fragment_position(lls_sls_monitor_output_buffer);

	mpu_data_unit_payload_fragments_t* data_unit_payload_types = NULL;
    mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments = NULL; //technically this is mpu_fragments->media_fragment_unit_vector
    mpu_data_unit_payload_fragments_t* mpu_metadata_fragments =    NULL;
    mpu_data_unit_payload_fragments_t* movie_metadata_fragments  = NULL;
    mmtp_sub_flow_t* mmtp_sub_flow = NULL;
    int total_fragments = 0;
    
    block_t* mpu_metadata_output_block_t = NULL;
    
    udp_flow_latest_mpu_sequence_number_container_t* udp_flow_matching_flows = udp_flow_find_matching_flows(udp_flow_latest_mpu_sequence_number_container, udp_flow);
    
    if(!udp_flow_latest_mpu_sequence_number_container || !udp_flow_latest_mpu_sequence_number_container->udp_flows_n) {
        __ISOBMFF_TOOLS_ERROR("atsc3_isobmff_build_mpu_metadata_ftyp_box: Unable to find flows for MPU metadata creation from %u:%u", udp_flow->dst_ip_addr, udp_flow->dst_port);
    }
    
    //build out ftyp/moov init box
	__ISOBMFF_TOOLS_INFO("atsc3_isobmff_build_mpu_metadata_ftyp_box: Starting to create MPU Metadata init box from flow: %u:%u, mpu_sequence_number_audio: %u, mpu_sequence_number_video: %u",
						 udp_flow->dst_ip_addr, udp_flow->dst_port,
						 mpu_sequence_number_audio,
						 mpu_sequence_number_video);

	for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
		udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];
		bool found_mpu_metadata_fragment = false;

		__ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_mpu_metadata_ftyp_box: Searching for MPU Metadata with %u:%u and packet_id: %u", udp_flow->dst_ip_addr, udp_flow->dst_port, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

		mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

		if(mmtp_sub_flow && mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.size) {
			int all_mpu_metadata_fragment_walk_count = mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.size-1;

			//mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data ?
			while(all_mpu_metadata_fragment_walk_count >= 0 && !found_mpu_metadata_fragment) {
				mpu_metadata_fragments =  mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data[all_mpu_metadata_fragment_walk_count--];

				if(mpu_metadata_fragments) {
					mmtp_payload_fragments_union_t* mpu_metadata_fragment = NULL;

					//rebuild if we are fragmented
					for(int i=0; i < mpu_metadata_fragments->timed_fragments_vector.size; i++) {
						mpu_metadata_fragment = mpu_metadata_fragments->timed_fragments_vector.data[i];

						if(mpu_metadata_fragment && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos ) {

							if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->video_packet_id && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) {
								if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos) {
									//reset if we are not a fragment reassembly
									if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 0 || mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 1) {
										lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos = 0;
									}
								}
								lls_sls_monitor_output_buffer_copy_video_init_block(lls_sls_monitor_output_buffer, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
								found_mpu_metadata_fragment = true;
								__ISOBMFF_TOOLS_DEBUG("found init box for audio, mpu_metadata packet_id: %d, mpu_sequence_number: %u",
									mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id,
									mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_sequence_number);

							} else if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->audio_packet_id && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) {
								if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos) {
									//reset if we are not a fragment reassembly
									if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 0 || mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 1) {
										lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos = 0;
									}
								}
								lls_sls_monitor_output_buffer_copy_audio_init_block(lls_sls_monitor_output_buffer, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
								found_mpu_metadata_fragment = true;

								__ISOBMFF_TOOLS_DEBUG("found init box for audio, mpu_metadata packet_id: %d, mpu_sequence_number: %u, fragmentation_indicator: %u, fragmentation_counter: %u", mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id,
									mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_sequence_number,
									mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator,
									mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_counter);
							}
						}
					}
				}
			}

			if(!found_mpu_metadata_fragment) {
				__ISOBMFF_TOOLS_WARN("Unable to find init box, mpu_metadata from mmt_flow, packet_id: %u", udp_flow_packet_id_mpu_sequence_tuple->packet_id);
				return NULL;
			}
		}
	}

	//sanity check to make sure we got our init box
	if(!lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.init_box_pos || !lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.init_box_pos) {
	  __ISOBMFF_TOOLS_WARN("Returning - int box data unit size is null: audio_recon_fragment_size: %u, video_recon_fragment_size: %u",
			lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.init_box_pos,
			lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.init_box_pos);

	  return NULL;
	}

    
    for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
        
        udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];
        
        //remember, subflows are built in case of DU fragmentation - korean MMT samples have this edge case
        mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);
        
        if(!mmtp_sub_flow) {
            __ISOBMFF_TOOLS_WARN("mmtp_sub_flow for packet_id: %u is null", udp_flow_packet_id_mpu_sequence_tuple->packet_id);
            //goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
            return NULL;
        }
        
        if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->audio_packet_id) {
            movie_metadata_fragments = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->movie_fragment_metadata_vector, mpu_sequence_number_audio);
        } else if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->video_packet_id) {
            
            movie_metadata_fragments = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->movie_fragment_metadata_vector, mpu_sequence_number_video);
        } else {
            __ISOBMFF_TOOLS_WARN("data unit recon - unknown packet_id: %u", udp_flow_packet_id_mpu_sequence_tuple->packet_id);
        }
                                                                                                
        mmtp_payload_fragments_union_t* fragment_metadata = NULL;
        if(movie_metadata_fragments && movie_metadata_fragments->timed_fragments_vector.size) {
            for(int i=0; i < movie_metadata_fragments->timed_fragments_vector.size; i++) {
                fragment_metadata = movie_metadata_fragments->timed_fragments_vector.data[i];
                __ISOBMFF_TOOLS_INFO("Movie Fragment Metadata: Found for fragment_metadata packet_id: %d, mpu_sequence_number: %u, fragmentation_indicator: %u, fragmentation_counter: %u",
                                     fragment_metadata->mmtp_packet_header.mmtp_packet_id,
                                     fragment_metadata->mmtp_mpu_type_packet_header.mpu_sequence_number,
                                     fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator,
                                     fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_counter);
                
                //rebuild if we are fragmented
                if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->video_packet_id) {
					lls_sls_monitor_output_buffer_copy_video_moov_block(lls_sls_monitor_output_buffer, fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                } else if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->audio_packet_id) {
                	lls_sls_monitor_output_buffer_copy_audio_moov_block(lls_sls_monitor_output_buffer, fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                }
            }
        } else {
            __ISOBMFF_TOOLS_WARN("Movie Fragment Metadata is NULL for packet_id: %u", udp_flow_packet_id_mpu_sequence_tuple->packet_id);
            //goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
            return NULL;
        }
    }
    
    
    for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
        udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];
        
        mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);
        
        //hack for the previous sequence number which should be complete
        if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->audio_packet_id) {
            data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->media_fragment_unit_vector, mpu_sequence_number_audio);
        } else if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->video_packet_id) {
            data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->media_fragment_unit_vector, mpu_sequence_number_video);
        } else {
            __ISOBMFF_TOOLS_WARN("data unit recon - unknown packet_id: %u", udp_flow_packet_id_mpu_sequence_tuple->packet_id);
        }
        mmtp_payload_fragments_union_t* mpu_data_unit_payload_fragments_timed = NULL;
        
        if(data_unit_payload_types && data_unit_payload_types->timed_fragments_vector.size) {
            total_fragments = data_unit_payload_types->timed_fragments_vector.size;
            //push to mpu_push_output_buffer
            for(int i=0; i < total_fragments; i++) {
                mmtp_payload_fragments_union_t* data_unit = data_unit_payload_types->timed_fragments_vector.data[i];
                
                //only push to our correct mpu flow....
                if(data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->video_packet_id && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) {
					lls_sls_monitor_output_buffer_copy_video_fragment_block(lls_sls_monitor_output_buffer, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                } else if(data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->audio_packet_id && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id){
					lls_sls_monitor_output_buffer_copy_audio_fragment_block(lls_sls_monitor_output_buffer, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                }
            }
            
        } else {
            __ISOBMFF_TOOLS_WARN("data unit size is null");
            //goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
            return NULL;
        }
    }
    
    
    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos || !lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos) {
           __ISOBMFF_TOOLS_WARN("Returning - data unit size is null: video_du_size: %u, audio_du_size: %u", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);
           return NULL;
    }
    
    return atsc3_isobmff_build_joined_patched_isobmff_fragment(lls_sls_monitor_output_buffer);
}

