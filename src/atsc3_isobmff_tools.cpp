/*
 * atsc3_isobmff_tools.c
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 */

#include "atsc3_isobmff_tools.h"


int _ISOBMFF_TOOLS_DEBUG_ENABLED = 0;
int _ISOBMFF_TOOLS_TRACE_ENABLED = 0;

int _ISOBMFF_TOOLS_SIGNALLING_DEBUG_ENABLED = 0;

#ifdef __ENABLE_GPL_BENTO4__
#include "bento4/ISOBMFFTrackJoiner.h"
#endif


/**
 * query the corresponding packet id's to resolve our mpu_metadata and
 * rebuild our combined boxes and tracks
 */

#ifdef __ENABLE_GPL_BENTO4__
lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_alc_isobmff_fragment(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {

    AP4_DataBuffer* ap4_data_buffer;
    AP4_MemoryByteStream* ap4_memory_byte_stream;

    ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_alc_boxes(lls_sls_monitor_output_buffer, &ap4_data_buffer, &ap4_memory_byte_stream);

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
        	block_Destroy(&lls_sls_monitor_output_buffer->joined_isobmff_block);
            __ISOBMFF_TOOLS_ERROR("ISOBMFF_track_joiner: block_Resize returned NULL for size: %u, freeing and returning NULL", ap4_memory_byte_stream->GetDataSize());
            return NULL;
        }
    }

    block_Write(lls_sls_monitor_output_buffer->joined_isobmff_block, (uint8_t*)ap4_memory_byte_stream->GetData(), ap4_memory_byte_stream->GetDataSize());

    //release our handle for bento4 for the memory byte stream
    ap4_memory_byte_stream->Release();
    delete ap4_data_buffer;

    return lls_sls_monitor_output_buffer;
}
#else
lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_alc_isobmff_fragment(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
    return NULL;
}
#endif


#ifdef __ENABLE_GPL_BENTO4__
lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_mmt_isobmff_fragment(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {

    AP4_DataBuffer* ap4_data_buffer;
    AP4_MemoryByteStream* ap4_memory_byte_stream;

    ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_mmt_boxes(lls_sls_monitor_output_buffer, &ap4_data_buffer, &ap4_memory_byte_stream);

    if(!ap4_memory_byte_stream || !ap4_memory_byte_stream->GetDataSize()) {
        __ISOBMFF_TOOLS_ERROR("atsc3_isobmff_build_joined_mmt_isobmff_fragment: returned %p, size: %u, returning NULL", ap4_memory_byte_stream, ap4_memory_byte_stream != NULL ? ap4_memory_byte_stream->GetDataSize() : 0);
        return NULL;
    }
    __ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_joined_mmt_isobmff_fragment: building return alloc of %u", ap4_memory_byte_stream->GetDataSize());

    if(!lls_sls_monitor_output_buffer->joined_isobmff_block) {
        lls_sls_monitor_output_buffer->joined_isobmff_block = block_Alloc(ap4_memory_byte_stream->GetDataSize());
    } else {
    	//rewind so our final output will be a clean mux
      	block_Rewind(lls_sls_monitor_output_buffer->joined_isobmff_block);

        if(!block_Resize(lls_sls_monitor_output_buffer->joined_isobmff_block, ap4_memory_byte_stream->GetDataSize())) {
        	block_Destroy(&lls_sls_monitor_output_buffer->joined_isobmff_block);
            __ISOBMFF_TOOLS_ERROR("atsc3_isobmff_build_joined_mmt_isobmff_fragment: block_Resize returned NULL for size: %u, freeing and returning NULL", ap4_memory_byte_stream->GetDataSize());
            return NULL;
        }
    }

    block_Write(lls_sls_monitor_output_buffer->joined_isobmff_block, (uint8_t*)ap4_memory_byte_stream->GetData(), ap4_memory_byte_stream->GetDataSize());
    ap4_memory_byte_stream->Release();
    delete ap4_data_buffer;

    return lls_sls_monitor_output_buffer;
}
#else
lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_mmt_isobmff_fragment(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
    return NULL;
}
#endif


#ifdef __ENABLE_GPL_BENTO4__
lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_mmt_rebuilt_boxes(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {

    AP4_DataBuffer* ap4_data_buffer;
    AP4_MemoryByteStream* ap4_memory_byte_stream;

    ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_mmt_rebuilt_boxes(lls_sls_monitor_output_buffer, &ap4_data_buffer, &ap4_memory_byte_stream);

    if(!ap4_memory_byte_stream || !ap4_memory_byte_stream->GetDataSize()) {
        __ISOBMFF_TOOLS_ERROR("atsc3_isobmff_build_joined_mmt_rebuilt_boxes: returned %p, size: %u, returning NULL", ap4_memory_byte_stream, ap4_memory_byte_stream != NULL ? ap4_memory_byte_stream->GetDataSize() : 0);
        return NULL;
    }
    __ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_joined_mmt_rebuilt_boxes: building return alloc of %u", ap4_memory_byte_stream->GetDataSize());

    if(!lls_sls_monitor_output_buffer->joined_isobmff_block) {
        lls_sls_monitor_output_buffer->joined_isobmff_block = block_Alloc(ap4_memory_byte_stream->GetDataSize());
    } else {
    	//rewind so our final output will be a clean mux
      	block_Rewind(lls_sls_monitor_output_buffer->joined_isobmff_block);

        if(!block_Resize(lls_sls_monitor_output_buffer->joined_isobmff_block, ap4_memory_byte_stream->GetDataSize())) {
        	block_Destroy(&lls_sls_monitor_output_buffer->joined_isobmff_block);
            __ISOBMFF_TOOLS_ERROR("atsc3_isobmff_build_joined_mmt_isobmff_fragment: block_Resize returned NULL for size: %u, freeing and returning NULL", ap4_memory_byte_stream->GetDataSize());
            return NULL;
        }
    }

    block_Write(lls_sls_monitor_output_buffer->joined_isobmff_block, (uint8_t*)ap4_memory_byte_stream->GetData(), ap4_memory_byte_stream->GetDataSize());
    ap4_memory_byte_stream->Release();
    delete ap4_data_buffer;
    return lls_sls_monitor_output_buffer;

}
#else
lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_mmt_rebuilt_boxes(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
    return NULL;
}
#endif


#ifdef __ENABLE_GPL_BENTO4__

lls_sls_monitor_buffer_isobmff_t* atsc3_isobmff_rebuild_track_mpu_from_sample_data(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff) {

    AP4_DataBuffer* ap4_data_buffer = NULL;
    AP4_MemoryByteStream* ap4_memory_byte_stream = NULL;

    uint32_t mdat_size_computed = ISOBMFF_rebuild_moof_from_sample_data(lls_sls_monitor_buffer_isobmff, &ap4_data_buffer, &ap4_memory_byte_stream);

    if(!mdat_size_computed || !ap4_memory_byte_stream || !ap4_memory_byte_stream->GetDataSize()) {
        __ISOBMFF_TOOLS_ERROR("atsc3_isobmff_build_joined_mmt_isobmff_fragment: returned mdat size: %u, ap4_memory_byte_stream: %p, returning NULL", mdat_size_computed, ap4_memory_byte_stream);
        return NULL;
    }

    //jjustman-2019-09-24 - always allocate to mmt_mpu_rebuilt_single, and append for isobmff final mux

    if(lls_sls_monitor_buffer_isobmff->mmt_mpu_rebuilt_single) {
        block_Destroy(&lls_sls_monitor_buffer_isobmff->mmt_mpu_rebuilt_single);
    }

    lls_sls_monitor_buffer_isobmff->mmt_mpu_rebuilt_single = block_Alloc(ap4_memory_byte_stream->GetDataSize());
    block_Write(lls_sls_monitor_buffer_isobmff->mmt_mpu_rebuilt_single, (uint8_t*)ap4_memory_byte_stream->GetData(), ap4_memory_byte_stream->GetDataSize());

    if(!lls_sls_monitor_buffer_isobmff->mmt_mpu_rebuilt_and_appending_for_isobmff_mux) {
        __ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_joined_mmt_isobmff_fragment: building return alloc of %u", ap4_memory_byte_stream->GetDataSize());

    	lls_sls_monitor_buffer_isobmff->mmt_mpu_rebuilt_and_appending_for_isobmff_mux = block_Alloc(ap4_memory_byte_stream->GetDataSize());
    } else {
    	//otherwise append..
    	__ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_joined_mmt_isobmff_fragment: appending return alloc of %u", ap4_memory_byte_stream->GetDataSize());

    }

    block_Write(lls_sls_monitor_buffer_isobmff->mmt_mpu_rebuilt_and_appending_for_isobmff_mux, (uint8_t*)ap4_memory_byte_stream->GetData(), ap4_memory_byte_stream->GetDataSize());
    ap4_memory_byte_stream->Release();
    delete ap4_data_buffer;

    return lls_sls_monitor_buffer_isobmff;

}
#else

lls_sls_monitor_buffer_isobmff_t* atsc3_isobmff_rebuild_track_mpu_from_sample_data(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff) {
    return NULL;
}
#endif

/*
 *
 * patch just this track_id and moof box as needed?
 *
 */

//
//lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_single_mmt_isobmff_fragment(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff) {
//
//    AP4_MemoryByteStream* ap4_memory_byte_stream;
//
//
////    ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_mmt_boxes(lls_sls_monitor_output_buffer, &ap4_memory_byte_stream);
////lls_sls_monitor_buffer_isobmff
//
//    if(!ap4_memory_byte_stream || !ap4_memory_byte_stream->GetDataSize()) {
//        __ISOBMFF_TOOLS_ERROR("atsc3_isobmff_build_single_mmt_isobmff_fragment: returned %p, size: %u, returning NULL", ap4_memory_byte_stream, ap4_memory_byte_stream != NULL ? ap4_memory_byte_stream->GetDataSize() : 0);
//        return NULL;
//    }
//    __ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_single_mmt_isobmff_fragment: building return alloc of %u", ap4_memory_byte_stream->GetDataSize());
//
//    if(!lls_sls_monitor_output_buffer->joined_isobmff_block) {
//        lls_sls_monitor_output_buffer->joined_isobmff_block = block_Alloc(ap4_memory_byte_stream->GetDataSize());
//    } else {
//    	//rewind so our final output will be a clean mux
//      	block_Rewind(lls_sls_monitor_output_buffer->joined_isobmff_block);
//
//        if(!block_Resize(lls_sls_monitor_output_buffer->joined_isobmff_block, ap4_memory_byte_stream->GetDataSize())) {
//            block_Release(&lls_sls_monitor_output_buffer->joined_isobmff_block);
//            __ISOBMFF_TOOLS_ERROR("atsc3_isobmff_build_joined_mmt_isobmff_fragment: block_Resize returned NULL for size: %u, freeing and returning NULL", ap4_memory_byte_stream->GetDataSize());
//            return NULL;
//        }
//    }
//
//    block_Write(lls_sls_monitor_output_buffer->joined_isobmff_block, (uint8_t*)ap4_memory_byte_stream->GetData(), ap4_memory_byte_stream->GetDataSize());
//    free (ap4_memory_byte_stream);
//    return lls_sls_monitor_output_buffer;
//
//}

/*
 * build our mdat box in front of our sample data
 * and pull the mdat box out of the flow moof payload
 */

lls_sls_monitor_buffer_isobmff_t* atsc3_isobmff_build_patched_mdat_fragment(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff) {
	lls_sls_monitor_buffer_isobmff_create_mdat_from_trun_sample_entries(lls_sls_monitor_buffer_isobmff);
	if(lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow) {
		uint32_t moof_size = lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->p_size;
		if(moof_size > 8) {
			if(lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->p_buffer[moof_size-4]=='m' &&
					lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->p_buffer[moof_size-3]=='d' &&
					lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->p_buffer[moof_size-2]=='a' &&
					lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->p_buffer[moof_size-1]=='t') {

				__ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_patched_mdat_fragment: removing trailing mdat from moof block from flow, original size: %u, new size: %u", moof_size, moof_size-8);

				//truncate the mdat box
				block_t* new_moof_from_flow = block_Duplicate_to_size(lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow, moof_size-8);
				block_Destroy(&lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow);
				lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow = new_moof_from_flow;
			}
		}
	}

	return lls_sls_monitor_buffer_isobmff;
}

lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_joined_patched_isobmff_fragment(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	lls_sls_monitor_buffer_isobmff_create_mdat_from_trun_sample_entries(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff);
	lls_sls_monitor_output_buffer_reset_trun(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff);

	lls_sls_monitor_buffer_isobmff_create_mdat_from_trun_sample_entries(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff);
	lls_sls_monitor_output_buffer_reset_trun(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff);

	return atsc3_isobmff_build_joined_mmt_isobmff_fragment(lls_sls_monitor_output_buffer);
}

/*
 * this is a big copy/paste warning
 */

#define __REFRAGMENT_INIT_BOX__ \
 \
 \
bool found_mpu_metadata_fragment_audio = false; \
bool found_mpu_metadata_fragment_video = false; \
 \
for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) { \
	udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i]; \
 \
	__ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_mpu_metadata_ftyp_box: Searching for MPU Metadata with %u:%u and packet_id: %u", udp_flow->dst_ip_addr, udp_flow->dst_port, udp_flow_packet_id_mpu_sequence_tuple->packet_id); \
 \
	mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id); \
 \
	if(mmtp_sub_flow && mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.size) { \
		int all_mpu_metadata_fragment_walk_count = mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.size-1; \
 \
 \
		while(all_mpu_metadata_fragment_walk_count >= 0 && !(found_mpu_metadata_fragment_audio && found_mpu_metadata_fragment_video)) { \
			mpu_metadata_fragments =  mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data[all_mpu_metadata_fragment_walk_count--]; \
 \
			if(mpu_metadata_fragments) { \
				mmtp_payload_fragments_union_t* mpu_metadata_fragment = NULL; \
 \
				/* rebuild if we are fragmented*/ \
				for(int i=0; i < mpu_metadata_fragments->timed_fragments_vector.size; i++) { \
					mpu_metadata_fragment = mpu_metadata_fragments->timed_fragments_vector.data[i]; \
 \
					if(mpu_metadata_fragment && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload && \
							mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos) { \
 \
/* todo - make sure we are in the same mpu sequence number...  not critical but helpful.. */ \
 if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) { \
							if(!found_mpu_metadata_fragment_audio && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->audio_packet_id) { \
								found_mpu_metadata_fragment_audio = lls_sls_monitor_output_buffer_init_block_flow_refragment(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff, mpu_metadata_fragment); \
							} else if(!found_mpu_metadata_fragment_video && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->video_packet_id) { \
								found_mpu_metadata_fragment_video = lls_sls_monitor_output_buffer_init_block_flow_refragment(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff, mpu_metadata_fragment); \
							} \
						} \
					} \
				} \
			} \
		} \
	} \
} \
 \
 if(found_mpu_metadata_fragment_audio) { \
	__ISOBMFF_TOOLS_WARN("found_mpu_metadata_fragment_audio, copying to audio_output_buffer_isobmff"); \
 	lls_sls_monitor_output_buffer_copy_init_block(&lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff, lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.init_block_flow_refragment); \
 } \
 if(found_mpu_metadata_fragment_video) { \
		__ISOBMFF_TOOLS_WARN("found_mpu_metadata_fragment_video, copying to video_output_buffer_isobmff"); \
 	lls_sls_monitor_output_buffer_copy_init_block(&lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff, lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.init_block_flow_refragment); \
 } \
\
if(!found_mpu_metadata_fragment_audio && !lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.init_block) { \
	__ISOBMFF_TOOLS_WARN("Returning - audio init_block data unit size is null!"); \
	return NULL; \
} \
if(!found_mpu_metadata_fragment_video && !lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.init_block) { \
	__ISOBMFF_TOOLS_WARN("Returning - video init_block data unit size is null!"); \
	return NULL; \
} \
 \




//udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,
//todo: jjustman-2019-09-05 - wrap this to be ATSC A/331 service_id aware

lls_sls_monitor_buffer_isobmff_t* atsc3_isobmff_build_raw_mpu_from_single_sequence_number(udp_flow_t* udp_flow,
                                                                                          udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,
                                                                                          uint16_t packet_id,
                                                                                          uint32_t mpu_sequence_number,
                                                                                          lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff,
                                                                                          mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container) {

	__ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_mpu_from_single_sequence_number: Starting to create MPU from flow: packet_id: %u, mpu_sequence_number: %u",
			packet_id,
			mpu_sequence_number);


	//only clear out trun sample, mdat and the current moof box
	lls_sls_lls_sls_monitor_buffer_isobmff_reset_moof_and_mdat_position(lls_sls_monitor_buffer_isobmff);
	lls_sls_monitor_buffer_isobmff->packet_id = packet_id;

    int total_fragments = 0;

    block_t* mpu_metadata_fragment = NULL;
    block_t* movie_fragment_metadata = NULL;
    block_t* mfu_mdat = NULL;

    __ISOBMFF_TOOLS_DEBUG("Searching for MPU Metadata with %u:%u and packet_id: %u, mpu_sequence_number: %u", udp_flow->dst_ip_addr, udp_flow->dst_port, packet_id, mpu_sequence_number);

    mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, mpu_sequence_number);
    if(!mpu_sequence_number_mmtp_mpu_packet_collection) {
        __ISOBMFF_TOOLS_DEBUG("Cannot find previous MPU sequence number for: %u", mpu_sequence_number);

        return NULL;
    }

    for(int i=0; i < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count; i++) {
        mmtp_mpu_packet_t* mmtp_mpu_packet = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[i];

        //assume no loss/re-sequencing for now..
        if(mmtp_mpu_packet->du_mpu_metadata_block) {
            if(!mpu_metadata_fragment) {
                __ISOBMFF_TOOLS_DEBUG("i: %u, psn: %u, Found MPU Metadata with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                      i, mmtp_mpu_packet->packet_sequence_number, udp_flow->dst_ip_addr, udp_flow->dst_port, mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->mpu_fragmentation_indicator);

                mpu_metadata_fragment = block_Duplicate(mmtp_mpu_packet->du_mpu_metadata_block);
            } else {
                __ISOBMFF_TOOLS_DEBUG("i: %u, psn: %u, Appending MPU Metadata with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                      i, mmtp_mpu_packet->packet_sequence_number, udp_flow->dst_ip_addr, udp_flow->dst_port, mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->mpu_fragmentation_indicator);
                block_Merge(mpu_metadata_fragment, mmtp_mpu_packet->du_mpu_metadata_block);
            }
        }
    }

    //not needed for now
    //found_mpu_metadata_fragment = lls_sls_monitor_output_buffer_init_block_flow_refragment(lls_sls_monitor_buffer_isobmff, mpu_metadata_fragment);

	if(mpu_metadata_fragment) {
		__ISOBMFF_TOOLS_DEBUG("found_mpu_metadata_fragment, copying to lls_sls_monitor_buffer_isobmff.init_block");
		lls_sls_monitor_output_buffer_copy_init_block(lls_sls_monitor_buffer_isobmff, mpu_metadata_fragment);
        block_Destroy(&mpu_metadata_fragment);
	} else {
		if(lls_sls_monitor_buffer_isobmff->init_block) {
			__ISOBMFF_TOOLS_INFO("init_block used from previous MPU!"); //todo - recover from last init frame
		} else {
			__ISOBMFF_TOOLS_WARN("init_block missing!");
            goto fail;
		}
	}



    for(int i=0; i < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count; i++) {
        mmtp_mpu_packet_t* mmtp_mpu_packet = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[i];

        //assume no loss/re-sequencing for now..
        if(mmtp_mpu_packet->du_movie_fragment_block) {
            if(!movie_fragment_metadata) {
                __ISOBMFF_TOOLS_DEBUG("i: %u, psn: %u, Found Movie Fragment Metadata with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                      i, mmtp_mpu_packet->packet_sequence_number, udp_flow->dst_ip_addr, udp_flow->dst_port, mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->mpu_fragmentation_indicator);

                movie_fragment_metadata = block_Duplicate(mmtp_mpu_packet->du_movie_fragment_block);
            } else {
                __ISOBMFF_TOOLS_DEBUG("i: %u, psn: %u, Appending Movie Fragment Metadata with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                      i, mmtp_mpu_packet->packet_sequence_number, udp_flow->dst_ip_addr, udp_flow->dst_port, mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->mpu_fragmentation_indicator);
                block_Merge(movie_fragment_metadata, mmtp_mpu_packet->du_movie_fragment_block);
            }
        }
    }

    if(!movie_fragment_metadata) {
        __ISOBMFF_TOOLS_WARN("movie_fragment_metadata missing!");
        goto fail;
    }

    lls_sls_monitor_output_buffer_copy_or_append_moof_block_from_flow(lls_sls_monitor_buffer_isobmff, movie_fragment_metadata);

    __ISOBMFF_TOOLS_DEBUG("data unit recon: ip: %u:%d, packet_id: %d, mpu_sequence_number: %d", udp_flow->dst_ip_addr, udp_flow->dst_port, packet_id, mpu_sequence_number);

    for(int i=0; i < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count; i++) {
        mmtp_mpu_packet_t* mmtp_mpu_packet = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[i];

        //assume no loss/re-sequencing for now..
        if(mmtp_mpu_packet->du_mfu_block) {
            __ISOBMFF_TOOLS_TRACE("Found mfu du with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                  udp_flow->dst_ip_addr, udp_flow->dst_port, packet_id, mpu_sequence_number, mmtp_mpu_packet->mpu_fragmentation_indicator);

            lls_sls_monitor_output_buffer_copy_and_recover_sample_fragment_block(lls_sls_monitor_buffer_isobmff, mmtp_mpu_packet);
        }
    }

    __ISOBMFF_TOOLS_SIGNALLING_DEBUG("signaling: checking flow");

    //TODO: jjustman-2019-09-05 - clear this flow...

    for(int i=0; i < mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count; i++) {
        mmtp_signalling_packet_t* mmtp_signalling_packet = mmtp_packet_id_packets_container->mmtp_signalling_packet_v.data[i];
        __ISOBMFF_TOOLS_SIGNALLING_DEBUG("signaling vector: idx: %u, messages_n is: %u", i, mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.count);

        for(int j=0; j < mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.count; j++) {
            mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.data[j];

            __ISOBMFF_TOOLS_SIGNALLING_DEBUG("checking signaling message: j idx: %u, message_id is %u", j, mmt_signalling_message_header_and_payload->message_header.message_id);

            if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MPT_message) {
                mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;

                if(mp_table->number_of_assets) {
                    for(int k=0; k < mp_table->number_of_assets; k++) {
                        //slight hack, check the asset types and default_asset = 1
                        mp_table_asset_row_t* mp_table_asset_row = &mp_table->mp_table_asset_row[k];

                        __ISOBMFF_TOOLS_SIGNALLING_DEBUG("MPT message: checking packet_id: %u, asset_type: %s, default: %u, identifier: %s",
                                mp_table_asset_row->mmt_general_location_info.packet_id,
                                mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag,
                                mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

                        if(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor && mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n) {
                            for(int l=0; l < mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n; l++) {
                                mmt_signalling_message_mpu_tuple_t* mmt_signaling_message_mpu_tuple = &mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[l];

                                if(mp_table_asset_row->mmt_general_location_info.packet_id == packet_id &&    mmt_signaling_message_mpu_tuple->mpu_sequence_number == mpu_sequence_number) {
                                    lls_sls_monitor_buffer_isobmff->mpu_presentation_time = mmt_signaling_message_mpu_tuple->mpu_presentation_time;
                                    lls_sls_monitor_buffer_isobmff->mpu_presentation_time_set = true;

                                    compute_ntp64_to_seconds_microseconds(lls_sls_monitor_buffer_isobmff->mpu_presentation_time, &lls_sls_monitor_buffer_isobmff->mpu_presentation_time_s, &lls_sls_monitor_buffer_isobmff->mpu_presentation_time_us);

                                    __ISOBMFF_TOOLS_SIGNALLING_DEBUG("setting packet_id: %u, mpu_sequence_number: %u to mpu_presentation_time: %" PRIu64 ", seconds: %u, ms: %u",
                                            mp_table_asset_row->mmt_general_location_info.packet_id,
                                            mpu_sequence_number,
                                            lls_sls_monitor_buffer_isobmff->mpu_presentation_time,
                                            lls_sls_monitor_buffer_isobmff->mpu_presentation_time_s,
                                            lls_sls_monitor_buffer_isobmff->mpu_presentation_time_us);


                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.count) {
    	__ISOBMFF_TOOLS_DEBUG("Returning: packet_id: %u, mpu_sequence_number: %u, du_count: %u",
				  packet_id,
				  mpu_sequence_number,
				  lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.count);

        lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff_pending_mux = atsc3_isobmff_build_patched_mdat_fragment(lls_sls_monitor_buffer_isobmff);

        if(mpu_metadata_fragment) {
            block_Destroy(&mpu_metadata_fragment);
        }

        if(movie_fragment_metadata) {
            block_Destroy(&movie_fragment_metadata);
        }

        if(mfu_mdat) {
            block_Destroy(&mfu_mdat);
        }

        return lls_sls_monitor_buffer_isobmff_pending_mux;
    }

fail:
    if(mpu_metadata_fragment) {
        block_Destroy(&mpu_metadata_fragment);
    }

    if(movie_fragment_metadata) {
        block_Destroy(&movie_fragment_metadata);
    }

    if(mfu_mdat) {
        block_Destroy(&mfu_mdat);
    }

    //otherwise, we were unable to rebuild this mpu
    __ISOBMFF_TOOLS_WARN("unable to rebuild: packet_id: %u, mpu_sequence_number: %u",
        	    			  packet_id,
    						  mpu_sequence_number);
    return NULL;

}




#ifdef __enable_dead_code__

lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_flow(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, lls_sls_mmt_monitor_t* lls_sls_mmt_monitor) {

	__ISOBMFF_TOOLS_TRACE("atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_flow: Starting to create ftyp_moof_mdat_box_from_flow: %u:%u", udp_flow->dst_ip_addr, udp_flow->dst_port);

	lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer = &lls_sls_mmt_monitor->lls_sls_monitor_output_buffer;
	lls_sls_monitor_output_buffer_reset_moof_and_fragment_position(lls_sls_monitor_output_buffer);

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


	__REFRAGMENT_INIT_BOX__

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
                __ISOBMFF_TOOLS_INFO("Movie Fragment Metadata: Found for fragment_metadata packet_id: %d, mpu_sequence_number: %u, fragmentation_indicator: %u, fragmentation_counter: %u", fragment_metadata->mmtp_packet_header->mmtp_packet_id, udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number, fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator, fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_counter);

                //rebuild if we are fragmented
                if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->video_packet_id) {
                	lls_sls_monitor_output_buffer_copy_or_append_moof_block_from_flow(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff, fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                } else if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->audio_packet_id) {
                	lls_sls_monitor_output_buffer_copy_or_append_moof_block_from_flow(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff, fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
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
		data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->media_fragment_unit_vector,
				udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number - 1 );

		if(data_unit_payload_types && data_unit_payload_types->timed_fragments_vector.size) {
			total_fragments = data_unit_payload_types->timed_fragments_vector.size;
			//push to mpu_push_output_buffer
			for(int i=0; i < total_fragments; i++) {
				mmtp_payload_fragments_union_t* data_unit = data_unit_payload_types->timed_fragments_vector.data[i];

				//only push to our correct mpu flow....
				if(data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->video_packet_id &&
						data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) {

					lls_sls_monitor_output_buffer_copy_and_recover_sample_fragment_block(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff, data_unit);

				} else if(data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->audio_packet_id && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id){
					lls_sls_monitor_output_buffer_copy_and_recover_sample_fragment_block(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff, data_unit);

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

    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.count || !lls_sls_monitor_output_buffer->video_output_buffer_isobmff.trun_sample_entry_v.count) {
        __ISOBMFF_TOOLS_WARN("Returning - data unit size is null: audio_du_size: %u, video_du_size: %u", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.count, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.trun_sample_entry_v.count);
        return NULL;
    }

    return atsc3_isobmff_build_joined_patched_isobmff_fragment(lls_sls_monitor_output_buffer);
}















lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_mpu_sequence_numbers(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, uint32_t mpu_sequence_number_audio, uint32_t mpu_sequence_number_video, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, lls_sls_mmt_monitor_t* lls_sls_mmt_monitor) {

	__ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_mpu_sequence_numbers: Starting to create ftyp_moof_mdat_box_from_flow: %u:%u, a: %u, v: %u",
			udp_flow->dst_ip_addr, udp_flow->dst_port,
			mpu_sequence_number_audio,
			mpu_sequence_number_video
			);

	//hack
	mpu_sequence_number_audio = mpu_sequence_number_audio ? mpu_sequence_number_audio - 2 : 0;

	mpu_sequence_number_video = mpu_sequence_number_video ? mpu_sequence_number_video - 2 : 0;

	lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer = &lls_sls_mmt_monitor->lls_sls_monitor_output_buffer;
	//do not clear so we can flush out mpu's quickly
	//until we can support mfu recon
	//lls_sls_monitor_output_buffer_reset_moof_and_fragment_position(lls_sls_monitor_output_buffer);

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
	__ISOBMFF_TOOLS_INFO("atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_mpu_sequence_numbers: Starting to create MPU Metadata init box from flow: %u:%u, mpu_sequence_number_audio: %u, mpu_sequence_number_video: %u",
						 udp_flow->dst_ip_addr, udp_flow->dst_port,
						 mpu_sequence_number_audio,
						 mpu_sequence_number_video);


	__REFRAGMENT_INIT_BOX__


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
                                     fragment_metadata->mmtp_packet_header->mmtp_packet_id,
                                     fragment_metadata->mmtp_mpu_type_packet_header.mpu_sequence_number,
                                     fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator,
                                     fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_counter);

                //rebuild if we are fragmented
                if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->video_packet_id) {
                	lls_sls_monitor_output_buffer_copy_or_append_moof_block_from_flow(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff, fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                } else if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->audio_packet_id) {
                	lls_sls_monitor_output_buffer_copy_or_append_moof_block_from_flow(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff, fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                }
            }
        } else {
//        	if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->video_packet_id && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_moof_box_pos) {
//                __ISOBMFF_TOOLS_WARN("Movie Fragment Metadata is NULL, using video previous box size for recon: %u", lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_moof_box_pos);
//                lls_sls_monitor_output_buffer_recover_from_last_video_moof_box(lls_sls_monitor_output_buffer);
//
//        	} else if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->audio_packet_id && lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_moof_box_pos) {
//				  __ISOBMFF_TOOLS_WARN("Movie Fragment Metadata is NULL, using audio previous box size for recon: %u", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_moof_box_pos);
//				of_box(lls_sls_monitor_output_buffer);
//        	}
        	//TODO: rebuild moof box
//			} else {
//				__ISOBMFF_TOOLS_WARN("Movie Fragment Metadata is NULL for packet_id: %u, mpu_sequence_id; %u", udp_flow_packet_id_mpu_sequence_tuple->packet_id, udp_flow_packet_id_mpu_sequence_tuple->packet_id);
//				//goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
//				return NULL;
//			}
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
                if(mpu_sequence_number_video && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->video_packet_id &&
                		data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) {
					lls_sls_monitor_output_buffer_copy_and_recover_sample_fragment_block(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff, data_unit);
                } else if(mpu_sequence_number_audio && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->audio_packet_id &&
                		data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id){
					lls_sls_monitor_output_buffer_copy_and_recover_sample_fragment_block(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff, data_unit);
                }
            }

//        } else {
//            __ISOBMFF_TOOLS_WARN("data unit size is null");
//            //goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
//            return NULL;
        }
    }


    //check for any mpt messages for mpu_timestamp_descriptor messages
    //mpu_sequence_number is not exposed on these messages, so iterate over our signalling_message_vector
    lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_set = false;
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_set = false;

    for(int f=0; f < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; f++) {
    	__ISOBMFF_TOOLS_SIGNALLING_DEBUG("checking flow: %u", f);

        udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[f];

        mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);
        if(!mmtp_sub_flow) {
			__ISOBMFF_TOOLS_SIGNALLING_DEBUG("no sub flow for packet_id: %u",  udp_flow_packet_id_mpu_sequence_tuple->packet_id);
        	continue;
        }

        mmtp_signalling_message_fragments_vector_t* mmtp_signalling_message_fragments_vector = &mmtp_sub_flow->mmtp_signalling_message_fragements_vector;

		for(int i=0; i < mmtp_signalling_message_fragments_vector->size; i++) {
			mmt_signalling_message_vector_t* mmt_signalling_message_vector = &mmtp_signalling_message_fragments_vector->data[i]->mmtp_signalling_message_fragments.mmt_signalling_message_vector;

			__ISOBMFF_TOOLS_SIGNALLING_DEBUG("checking signaling vector: idx: %u, messages_n is: %u", i, mmt_signalling_message_vector->messages_n);

			for(int j=0; j < mmt_signalling_message_vector->messages_n; j++) {
				mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmt_signalling_message_vector->messages[j];
				__ISOBMFF_TOOLS_SIGNALLING_DEBUG("checking signaling message: j idx: %u, message_id is %u", j, mmt_signalling_message_header_and_payload->message_header.message_id);

				if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MPT_message) {
					mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;

					if(mp_table->number_of_assets) {
						for(int k=0; k < mp_table->number_of_assets; k++) {
							//slight hack, check the asset types and default_asset = 1
							mp_table_asset_row_t* mp_table_asset_row = &mp_table->mp_table_asset_row[k];

							__ISOBMFF_TOOLS_SIGNALLING_DEBUG("MPT message: checking packet_id: %u, asset_type: %s, default: %u, identifier: %s", mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

							if(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor && mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n) {
								for(int l=0; l < mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n; l++) {
									mmt_signalling_message_mpu_tuple_t* mmt_signaling_message_mpu_tuple = &mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[l];



									if(mpu_sequence_number_audio && mp_table_asset_row->mmt_general_location_info.packet_id == lls_sls_mmt_monitor->audio_packet_id && mp_table_asset_row->mmt_general_location_info.packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id &&
											mmt_signaling_message_mpu_tuple->mpu_sequence_number == mpu_sequence_number_audio) {
										lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time = mmt_signaling_message_mpu_tuple->mpu_presentation_time;
										lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_set = true;
										compute_ntp64_to_seconds_microseconds(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_s, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_us);

										__ISOBMFF_TOOLS_SIGNALLING_DEBUG("setting audio packet_id: %u, mpu_sequence_number: %u to mpu_presentation_time: %llu, seconds: %u, ms: %u",
												mp_table_asset_row->mmt_general_location_info.packet_id, mpu_sequence_number_audio,
												lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time,
												lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_s, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_us);


									} else if(mpu_sequence_number_video && mp_table_asset_row->mmt_general_location_info.packet_id == lls_sls_mmt_monitor->video_packet_id && mp_table_asset_row->mmt_general_location_info.packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id &&
											mmt_signaling_message_mpu_tuple->mpu_sequence_number == mpu_sequence_number_video) {

										lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time = mmt_signaling_message_mpu_tuple->mpu_presentation_time;
										lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_set = true;

										compute_ntp64_to_seconds_microseconds(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_s, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_us);

										__ISOBMFF_TOOLS_SIGNALLING_DEBUG("setting video packet_id: %u, mpu_sequence_number: %u to mpu_presentation_time: %llu, seconds: %u, ms: %u",
												mp_table_asset_row->mmt_general_location_info.packet_id, mpu_sequence_number_video,
												lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time,
												lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_s, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_us);


									}
								}

							}
						}
					}
				}
			}
		}
    }


//    if(!(mpu_sequence_number_audio && !lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.count) ||
//    	(mpu_sequence_number_video && !lls_sls_monitor_output_buffer->video_output_buffer_isobmff.trun_sample_entry_v.count)) {
//           __ISOBMFF_TOOLS_WARN("short - data unit size is null: audio: mpu_sequence_number: %u, du_size: %u, video: mpu_sequence_number: %u, video_du_size: %u",
//        		   mpu_sequence_number_audio,
//        		   lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.count,
//				   mpu_sequence_number_video,
//				   lls_sls_monitor_output_buffer->video_output_buffer_isobmff.trun_sample_entry_v.count);
//           return NULL;
//    }

    if(lls_sls_monitor_output_buffer->joined_isobmff_block ||
    		(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.count &&
    				lls_sls_monitor_output_buffer->video_output_buffer_isobmff.trun_sample_entry_v.count)) {
    	lls_sls_monitor_output_buffer->should_flush_output_buffer = true;

    	  __ISOBMFF_TOOLS_INFO("Returning: should_flush_output_buffer: true - data unit size: audio: mpu_sequence_number: %u, du_count: %u, video: mpu_sequence_number: %u, du_count: %u",
    	        		   mpu_sequence_number_audio,
    	        		   lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.count,
    					   mpu_sequence_number_video,
    					   lls_sls_monitor_output_buffer->video_output_buffer_isobmff.trun_sample_entry_v.count);
    	return atsc3_isobmff_build_joined_patched_isobmff_fragment(lls_sls_monitor_output_buffer);
    } else {
    	//enqueue mdat for next mpu only if we just did a audio track...
//    	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.count) {
//    		__ISOBMFF_TOOLS_INFO("enqueing audio");
//       	 	 lls_sls_monitor_buffer_isobmff_create_mdat_from_trun_sample_entries(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff);
//     		block_t* queue_audio_output_buffer = lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff);
//     		//append this into lls_sls_monitor_output_buffer->audio_output_buffer_isobmff
//     		if(lls_sls_monitor_output_buffer->joined_isobmff_block) {
//         		block_Append(lls_sls_monitor_output_buffer->joined_isobmff_block, queue_audio_output_buffer);
//
//     		} else {
//     			lls_sls_monitor_output_buffer->joined_isobmff_block = block_Duplicate(queue_audio_output_buffer);
//     		}
//     		lls_sls_monitor_output_buffer_reset_audio_moof_and_fragment_position(lls_sls_monitor_output_buffer);
//    	} else if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.trun_sample_entry_v.count) {
//    		__ISOBMFF_TOOLS_INFO("enqueing video");
//         	lls_sls_monitor_buffer_isobmff_create_mdat_from_trun_sample_entries(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff);
//     		//video_output_buffer = lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff);
//
//    	}
    	}
    	return NULL;
   // }
}




lls_sls_monitor_output_buffer_t* atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box_from_mpu_sequence_numbers_both_required(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, uint32_t mpu_sequence_number_audio, uint32_t mpu_sequence_number_video, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, lls_sls_mmt_monitor_t* lls_sls_mmt_monitor) {

	lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer = &lls_sls_mmt_monitor->lls_sls_monitor_output_buffer;
	//only reset our output buffer in the callee

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


	__REFRAGMENT_INIT_BOX__


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
                                     fragment_metadata->mmtp_packet_header->mmtp_packet_id,
                                     fragment_metadata->mmtp_mpu_type_packet_header.mpu_sequence_number,
                                     fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator,
                                     fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_counter);

                //rebuild if we are fragmented
                if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->video_packet_id) {
                	lls_sls_monitor_output_buffer_copy_or_append_moof_block_from_flow(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff, fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                } else if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->audio_packet_id) {
                	lls_sls_monitor_output_buffer_copy_or_append_moof_block_from_flow(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff, fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                }
            }
        } else {
//        	if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->video_packet_id && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_moof_box_pos) {
//                __ISOBMFF_TOOLS_WARN("Movie Fragment Metadata is NULL, using video previous box size for recon: %u", lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_moof_box_pos);
//                lls_sls_monitor_output_buffer_recover_from_last_video_moof_box(lls_sls_monitor_output_buffer);
//
//        	} else if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->audio_packet_id && lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_moof_box_pos) {
//				  __ISOBMFF_TOOLS_WARN("Movie Fragment Metadata is NULL, using audio previous box size for recon: %u", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_moof_box_pos);
//				lls_sls_monitor_output_buffer_recover_from_last_audio_moof_box(lls_sls_monitor_output_buffer);
//
//			} else {
//				__ISOBMFF_TOOLS_WARN("Movie Fragment Metadata is NULL for packet_id: %u, mpu_sequence_id; %u", udp_flow_packet_id_mpu_sequence_tuple->packet_id, udp_flow_packet_id_mpu_sequence_tuple->packet_id);
//				//goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
//				return NULL;
//			}
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
					lls_sls_monitor_output_buffer_copy_and_recover_sample_fragment_block(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff, data_unit);
                } else if(data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->audio_packet_id && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id){
					lls_sls_monitor_output_buffer_copy_and_recover_sample_fragment_block(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff, data_unit);
                }
            }

        } else {
            __ISOBMFF_TOOLS_WARN("data unit size is null");
            //goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
            return NULL;
        }
    }


    //check for any mpt messages for mpu_timestamp_descriptor messages
    //mpu_sequence_number is not exposed on these messages, so iterate over our signalling_message_vector
    lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_set = false;
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_set = false;

    for(int f=0; f < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; f++) {
		__ISOBMFF_TOOLS_WARN("checking flow: %u", f);

        udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[f];

        mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

        mmtp_signalling_message_fragments_vector_t* mmtp_signalling_message_fragments_vector = &mmtp_sub_flow->mmtp_signalling_message_fragements_vector;

		for(int i=0; i < mmtp_signalling_message_fragments_vector->size; i++) {
			mmt_signalling_message_vector_t* mmt_signalling_message_vector = &mmtp_signalling_message_fragments_vector->data[i]->mmtp_signalling_message_fragments.mmt_signalling_message_vector;

			__ISOBMFF_TOOLS_WARN("checking signaling vector: idx: %u, messages_n is: %u", i, mmt_signalling_message_vector->messages_n);

			for(int j=0; j < mmt_signalling_message_vector->messages_n; j++) {
				mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmt_signalling_message_vector->messages[j];
				__ISOBMFF_TOOLS_WARN("checking signaling message: j idx: %u, message_id is %u", j, mmt_signalling_message_header_and_payload->message_header.message_id);

				if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MPT_message) {
					mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;

					if(mp_table->number_of_assets) {
						for(int k=0; k < mp_table->number_of_assets; k++) {
							//slight hack, check the asset types and default_asset = 1
							mp_table_asset_row_t* mp_table_asset_row = &mp_table->mp_table_asset_row[k];

							__ISOBMFF_TOOLS_WARN("MPT message: checking packet_id: %u, asset_type: %s, default: %u, identifier: %s", mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

							if(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor && mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n) {
								for(int l=0; l < mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n; l++) {
									mmt_signalling_message_mpu_tuple_t* mmt_signaling_message_mpu_tuple = &mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[l];



									if(mp_table_asset_row->mmt_general_location_info.packet_id == lls_sls_mmt_monitor->audio_packet_id && mp_table_asset_row->mmt_general_location_info.packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id &&
											mmt_signaling_message_mpu_tuple->mpu_sequence_number == mpu_sequence_number_audio) {
										lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time = mmt_signaling_message_mpu_tuple->mpu_presentation_time;
										lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_set = true;
										compute_ntp64_to_seconds_microseconds(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_s, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_us);

										__ISOBMFF_TOOLS_WARN("setting audio packet_id: %u, mpu_sequence_number: %u to mpu_presentation_time: %llu, seconds: %u, ms: %u",
												mp_table_asset_row->mmt_general_location_info.packet_id, mpu_sequence_number_audio,
												lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time,
												lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_s, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_us);


									} else if(mp_table_asset_row->mmt_general_location_info.packet_id == lls_sls_mmt_monitor->video_packet_id && mp_table_asset_row->mmt_general_location_info.packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id &&
											mmt_signaling_message_mpu_tuple->mpu_sequence_number == mpu_sequence_number_video) {

										lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time = mmt_signaling_message_mpu_tuple->mpu_presentation_time;
										lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_set = true;

										compute_ntp64_to_seconds_microseconds(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_s, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_us);

										__ISOBMFF_TOOLS_WARN("setting video packet_id: %u, mpu_sequence_number: %u to mpu_presentation_time: %llu, seconds: %u, ms: %u",
												mp_table_asset_row->mmt_general_location_info.packet_id, mpu_sequence_number_video,
												lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time,
												lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_s, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_us);


									}
								}

							}
						}
					}
				}
			}
		}
    }


    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.count || !lls_sls_monitor_output_buffer->video_output_buffer_isobmff.trun_sample_entry_v.count) {
           __ISOBMFF_TOOLS_WARN("Returning - data unit size is null: audio count: %u, video count: %u", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.count, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.trun_sample_entry_v.count);
           return NULL;
    }

    return atsc3_isobmff_build_joined_patched_isobmff_fragment(lls_sls_monitor_output_buffer);
}
#endif
