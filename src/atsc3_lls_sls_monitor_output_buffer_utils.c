//
//  atsc3_lls_sls_monitor_output_buffer.c
//  cmd
//
//  Created by Jason Justman on 3/2/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

//todo: use box_t also



/**
 *
 *
 *
sending all the media units that are contained in that movie fragment.
At the receiver side, step g.3.i ensures that the movie fragment is
recovered appropriately by reordering the received data using the
MPU_sequence_number and the movie_fragment_sequence_number.
This is necessary if the receiver is operating in the Fragment mode or MPU mode,
where only complete movie fragments or complete MPUs are forwarded to the application. When operating in the very low delay mode, the receiver will forward every single MFU to the application. In this case, it has to make sure that the content supports this operation, so that MFUs will be self-describing and self-contained. In particular, the receiver should be able to recover the presentation timestamp of that MFU payload using the sample number, fragment_ sequence_number and MPU_sequence_number,

For fragments and items that cannot be recovered correctly by the time the
fixed end to end delivery delay passes, error concealment is performed on
the movie fragment or the partially recovered item.


5.13 Error resliancce

MMTP is optimized for the delivery of MPUs, which are ISOBMFF files.
 The delivery of the MPU is performed movie fragment by movie fragment,
 thus enabling fast start-up delays and fast access to the content.
 MMTP defines three different payload fragments for error resilience purpose.

— The MPU metadata: This information contains the metadata of the ISOBMFF file and the MPU.
The MPU metadata thus contains all codec configuration information and is crucial for the consumption of the whole MPU.
The MPU mode allows to mark the packets of the MPU metadata (usually only one or a few packets), so that the client can
clearly identify them and recognize if it has received it correctly. To provide for random access and enhance the probability of
 receiving the MPU metadata, the sender should send the metadata repeatedly and periodically throughout the transmission time of that MPU.

— The fragment metadata: This information contains the “moof” box and the skeleton of the “mdat” box.
This metadata provides information about the sample sizes and their timing and duration. This information is
important for all media samples of the current fragment. However, it may be possible to recover from loss of
fragment metadata and it is also possible to send it out of order. The sender may deliver the fragment metadata
repeatedly and interleaved with the packets that contain the media samples of that fragment to increase the probability
of correct reception and to enable random access inside a movie fragment.

— The MFU: An MFU contains a media unit from a sample of a particular movie fragment. The MFU also provides enough
information such as the sample number, the fragment sequence number and the position inside the media sample to
position the media unit on the timeline and inside the “mdat” box. It may also contain information about the
importance of that media unit for the decoding process. Based on that information, the sender, as well as
intermediate MMT entities, may undertake appropriate steps to enhance error resilience respective to the priority
and importance of the media unit. A media unit from a SAP is, for instance, more important than a media unit
for which there are no dependencies.

One of MMTP’s advantages is its ability to enable error robustness at the receiver side by enabling
the client to recover from packet losses and still generate a compliant MPU when needed.

When the MPU metadata is lost, the client should keep any correctly received data from that MPU until a new
copy of the MPU metadata is correctly received.

When a fragment metadata is lost, the client should use the information from previous fragments about
the sample durations to correctly reconstruct the lost “moof” box. It uses information from the received
MFUs to recover the movie fragment segment number. The offsets of the media data may be recovered later
using the start of a fragment as the baseline and the sample number and MFU sizes to reconstruct the “mdat” box as well.

When an MFU is lost, the loss can be discovered at the receiver based on the gap in the sequence numbers.

The missing MFU is replaced by a null value array in the “mdat” box, if at least one MFU of the same media sample
has been received correctly.

If the complete sample is lost, the space occupied by that media sample may be removed
completely and the information in the containing chunk, the “trun”, should be edited appropriately to adjust the
sample duration of the previous sample and the sample offsets of the following samples.


 */

#include "atsc3_lls_sls_monitor_output_buffer.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"


int _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG_ENABLED = 1;
int _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE_ENABLED = 1;

//
//int lls_sls_monitor_output_buffer_recover_from_last_video_moof_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
//	if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_moof_box_pos) {
//		lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos = lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_moof_box_pos;
//		lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_is_from_last_mpu = true;
//		lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_is_from_last_mpu_processed = false;
//
//		//reparse our trun sample table
//		block_t* moof_box = block_Alloc(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos);
//		block_Write(moof_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos);
//		trun_sample_entry_vector_t* trun_sample_entry_vector = parseMoofBoxForTrunSampleEntries(moof_box);
//
//		//clear out our sample size to be rebuilt later
//		for(int i=0; i < trun_sample_entry_vector->size; i++) {
//			trun_sample_entry_vector->data[i]->sample_size = 0;
//			trun_sample_entry_vector->data[i]->has_matching_sample = false;
//		}
//
//		lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_trun_sample_entry_vector = trun_sample_entry_vector;
//	}
//	return 0;
//}

//
//int lls_sls_monitor_output_buffer_recover_from_last_audio_moof_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
//	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_moof_box_pos) {
//		lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_moof_box_pos;
//		lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_is_from_last_mpu = true;
//		lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_is_from_last_mpu_processed = false;
//
//		//reparse our trun sample table
//		block_t* moof_box = block_Alloc(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos);
//		block_Write(moof_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos);
//		trun_sample_entry_vector_t* trun_sample_entry_vector = parseMoofBoxForTrunSampleEntries(moof_box);
//
//		//clear out our sample size to be rebuilt later
//		for(int i=0; i < trun_sample_entry_vector->size; i++) {
//			trun_sample_entry_vector->data[i]->sample_size = 0;
//			trun_sample_entry_vector->data[i]->has_matching_sample = false;
//		}
//
//		lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_trun_sample_entry_vector = trun_sample_entry_vector;
//	}
//	return 0;
//}




void lls_sls_monitor_output_buffer_reset_trun(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff) {

	lls_sls_monitor_buffer_isobmff_clear_trun_sample_entry(lls_sls_monitor_buffer_isobmff);


}

//keep our init box around in case we miss it in a packet drop..
void lls_sls_lls_sls_monitor_buffer_isobmff_reset_moof_and_mdat_position(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff) {
	lls_sls_monitor_buffer_isobmff_clear_trun_sample_entry(lls_sls_monitor_buffer_isobmff);
	lls_sls_monitor_buffer_isobmff->mpu_presentation_time_set = false;

	//clear out the inflight refragment
	block_Release(&lls_sls_monitor_buffer_isobmff->init_block_flow_refragment);
	block_Release(&lls_sls_monitor_buffer_isobmff->alc_moof_mdat_block);

	//keep our last moof box just in case we need to rebuild, we will clear out the carry-over block
	if(lls_sls_monitor_buffer_isobmff->mmt_moof_block) {

		if(lls_sls_monitor_buffer_isobmff->mmt_moof_block_previous_mpu) {
			block_Release(&lls_sls_monitor_buffer_isobmff->mmt_moof_block_previous_mpu);
		}
		//keep the flow box first, otherwise the reconstitued
		lls_sls_monitor_buffer_isobmff->mmt_moof_block_previous_mpu = lls_sls_monitor_buffer_isobmff->mmt_moof_block;
		lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow = NULL;
	} else if(lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow) {
		if(lls_sls_monitor_buffer_isobmff->mmt_moof_block_previous_mpu) {
			block_Release(&lls_sls_monitor_buffer_isobmff->mmt_moof_block_previous_mpu);
		}
		lls_sls_monitor_buffer_isobmff->mmt_moof_block_previous_mpu = lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow;
		lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow = NULL;
	}

	block_Release(&lls_sls_monitor_buffer_isobmff->mmt_mdat_block);
}

void lls_sls_monitor_output_buffer_reset_moof_and_fragment_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {

	lls_sls_monitor_buffer_isobmff_clear_trun_sample_entry(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff);
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_set = false;


	block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.alc_moof_mdat_block);
	block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block);
	block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_from_flow);

	block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_mdat_block);


	lls_sls_monitor_buffer_isobmff_clear_trun_sample_entry(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff);
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_set = false;
	block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.alc_moof_mdat_block);
	block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_moof_block);
	block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_moof_block_from_flow);

	block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_mdat_block);

    lls_sls_monitor_output_buffer->should_flush_output_buffer = false;
}


void lls_sls_monitor_output_buffer_reset_rebuilt_mpu_moof_and_fragment_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {

	lls_sls_monitor_buffer_isobmff_clear_trun_sample_entry(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff);
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_set = false;


	block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.alc_moof_mdat_block);
	block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block);
	block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_from_flow);

	block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_mdat_block);
	block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_mpu_rebuilt);


	lls_sls_monitor_buffer_isobmff_clear_trun_sample_entry(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff);
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_set = false;
	block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.alc_moof_mdat_block);
	block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_moof_block);
	block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_moof_block_from_flow);
	block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_mpu_rebuilt);

	block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_mdat_block);

}

void lls_sls_monitor_output_buffer_reset_all_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {

	//clear out any in-flight re-fragments
	//clear out any pending init_refragment_blocks if we have any lost packets
	block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block_flow_refragment);
	block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block_flow_refragment);
assert(true);

	block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block);
	block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block);

//	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_mpu_sequence_number_last_fragment = NULL;

//	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_mpu_sequence_number_last_fragment = NULL;

	lls_sls_monitor_output_buffer_reset_moof_and_fragment_position(lls_sls_monitor_output_buffer);
}




bool lls_sls_monitor_output_buffer_init_block_flow_refragment(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, mmtp_payload_fragments_union_t* mpu_metadata_fragment) {

	//lls_sls_monitor_output_buffer_copy_video_init_block(lls_sls_monitor_output_buffer, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
	if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 0) {
		//this is a full fragment, so copy over
		lls_sls_monitor_output_buffer_copy_init_block_flow_refragment(lls_sls_monitor_buffer_isobmff, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("completing init box for packet_id: %d, mpu_sequence_number: %u", mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_sequence_number);

		return true;
	} else if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 1) {
		//start of a fragment
		lls_sls_monitor_output_buffer_copy_init_block_flow_refragment(lls_sls_monitor_buffer_isobmff, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("found init w/ mpu_fragmentation_indicator=0x1 box for packet_id: %d, mpu_sequence_number: %u", mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_sequence_number);
		return false;
	} else if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 2) {
		//start of a fragment
		lls_sls_monitor_output_buffer_append_init_block_flow_refragment(lls_sls_monitor_buffer_isobmff, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("found init w/ mpu_fragmentation_indicator=0x2 box for packet_id: %d, mpu_sequence_number: %u", mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_sequence_number);
		return false;
	} else if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 3) {
			//start of a fragment
		lls_sls_monitor_output_buffer_append_init_block_flow_refragment(lls_sls_monitor_buffer_isobmff, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("completing init w/ mpu_fragmentation_indicator=0x3 box for packet_id: %d, mpu_sequence_number: %u", mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_sequence_number);
		return true;
	}

	return false;
}


int __lls_sls_monitor_output_buffer_check_and_copy(uint8_t* dest_box, uint32_t* dest_box_pos, uint32_t max_alloc_size, block_t* src) {

	uint32_t last_box_pos = *dest_box_pos;

	if(*dest_box_pos + src->i_pos > max_alloc_size) {
		//truncate and rewind if possible
		if(src->i_pos > max_alloc_size) {
			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("box: %p, unable to copy from src size: %u, buffer size: %u", dest_box, src->i_pos, max_alloc_size)
			return -1;
		}
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("box: %p, truncating box from pos: %u to 0, src size: %u", dest_box, last_box_pos, src->i_pos);
		*dest_box_pos = 0;
	    memcpy(&dest_box[*dest_box_pos], src->p_buffer, src->i_pos);
	    *dest_box_pos += src->i_pos;

		return -1 * last_box_pos;

	} else {
	    memcpy(&dest_box[*dest_box_pos], src->p_buffer, src->i_pos);

	    *dest_box_pos += src->i_pos;
	    __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE("box: %p, copied src size: %u to pos: %u", dest_box, src->i_pos, *dest_box_pos);
		return *dest_box_pos;
	}
}

int lls_sls_monitor_output_buffer_copy_init_block(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, block_t* init_block) {
	assert(init_block);
	assert(init_block->i_pos);

	if(lls_sls_monitor_buffer_isobmff->init_block) {
		block_Release(&lls_sls_monitor_buffer_isobmff->init_block);
	}
	lls_sls_monitor_buffer_isobmff->init_block = block_Duplicate(init_block);

	return lls_sls_monitor_buffer_isobmff->init_block->i_pos;
}

int lls_sls_monitor_output_buffer_copy_init_block_flow_refragment(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, block_t* init_block) {

	assert(init_block);
	assert(init_block->i_pos);

	if(lls_sls_monitor_buffer_isobmff->init_block_flow_refragment) {
		block_Release(&lls_sls_monitor_buffer_isobmff->init_block_flow_refragment);
	}
	lls_sls_monitor_buffer_isobmff->init_block_flow_refragment = block_Duplicate(init_block);

	return lls_sls_monitor_buffer_isobmff->init_block_flow_refragment->i_pos;
}


int lls_sls_monitor_output_buffer_append_init_block_flow_refragment(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, block_t* init_block) {

	block_Append(lls_sls_monitor_buffer_isobmff->init_block_flow_refragment, init_block);

	return lls_sls_monitor_buffer_isobmff->init_block_flow_refragment->i_pos;
}

int lls_sls_monitor_output_buffer_copy_audio_init_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_header) {

	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block) {
		block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block);
	}
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block = block_Duplicate(audio_isobmff_header);

	return lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block->i_pos;
}

//used for mmt 0x1 moof merging

int lls_sls_monitor_output_buffer_copy_audio_moof_block_from_flow(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_moof) {

	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_from_flow) {
			block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_from_flow);
	}
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_from_flow = block_Duplicate(audio_isobmff_moof);

	return lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_from_flow->i_pos;
}

/**
 * used from atsc3_alc_utils.c
 *
 */
int lls_sls_monitor_output_buffer_copy_audio_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_fragment) {
    
	assert(audio_isobmff_fragment);
	assert(audio_isobmff_fragment->i_pos);

	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.alc_moof_mdat_block) {
		block_Release(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.alc_moof_mdat_block);
	}
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.alc_moof_mdat_block = block_Duplicate(audio_isobmff_fragment);

	return lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.alc_moof_mdat_block->i_pos;
}

/**
 * used from atsc3_alc_utils.c
 *
 * copy and paste from above...
 */
int lls_sls_monitor_output_buffer_copy_video_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_fragment) {

	assert(video_isobmff_fragment);
	assert(video_isobmff_fragment->i_pos);

	if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.alc_moof_mdat_block) {
		block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.alc_moof_mdat_block);
	}
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.alc_moof_mdat_block = block_Duplicate(video_isobmff_fragment);

	return lls_sls_monitor_output_buffer->video_output_buffer_isobmff.alc_moof_mdat_block->i_pos;
}

/*
 *
 * support multi-gop route delivery and isobmff concatnation
 */

int lls_sls_monitor_output_buffer_merge_alc_fragment_block(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, block_t* isobmff_fragment) {

	assert(isobmff_fragment);
	assert(isobmff_fragment->i_pos);

	if(!lls_sls_monitor_buffer_isobmff->alc_moof_mdat_block) {
		lls_sls_monitor_buffer_isobmff->alc_moof_mdat_block = block_Duplicate(isobmff_fragment);
	} else {
		block_Append(lls_sls_monitor_buffer_isobmff->alc_moof_mdat_block, isobmff_fragment);
	}

	return lls_sls_monitor_buffer_isobmff->alc_moof_mdat_block->i_pos;
}


int lls_sls_monitor_output_buffer_copy_video_init_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_header) {

	assert(video_isobmff_header);
	assert(video_isobmff_header->i_pos);

	if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block) {
		block_Release(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block);
	}
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block = block_Duplicate(video_isobmff_header);

	return lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block->i_pos;
}



int lls_sls_monitor_output_buffer_copy_or_append_moof_block_from_flow(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, block_t* isobmff_moof_block) {

	if(lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow) {
		block_Append(lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow, isobmff_moof_block);
	} else {
		lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow = block_Duplicate(isobmff_moof_block);
	}

	return lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->i_pos;
}





void __data_unit_recover_null_pad_offset(mmtp_payload_fragments_union_t* data_unit) {
//	uint32_t to_null_size =	data_unit->mpu_data_unit_payload_fragments_timed.;
//	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: null pad: adding %u head to %u, sample: %u, fragment: %u, mpu_sequence_number: %u, packet_sequence_number: %u",
//			to_null_size,
//			data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos,
//			data_unit->mpu_data_unit_payload_fragments_timed.sample_number,
//			data_unit->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_counter,
//			data_unit->mmtp_mpu_type_packet_header.mpu_sequence_number,
//			data_unit->mmtp_mpu_type_packet_header.packet_sequence_number);
//
//	block_t* temp_mpu_data_unit_payload = block_Alloc(to_null_size + data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos);
//	temp_mpu_data_unit_payload->i_pos = to_null_size;
//	block_Write(temp_mpu_data_unit_payload, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos);
//	//hack - keep track of our old du size
//	data_unit->mmtp_mpu_type_packet_header.data_unit_length = data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;
//
//	block_Release(&data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
//	data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload = temp_mpu_data_unit_payload;
}


void __data_unit_recover_null_pad_tail(mmtp_payload_fragments_union_t* data_unit, uint32_t tail_pad_size) {
//	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: tail pad: adding %u tail to %u, sample: %u, fragment: %u, mpu_sequence_number: %u, packet_sequence_number: %u",
//			tail_pad_size,
//			data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos,
//			data_unit->mpu_data_unit_payload_fragments_timed.sample_number,
//			data_unit->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_counter,
//			data_unit->mmtp_mpu_type_packet_header.mpu_sequence_number,
//			data_unit->mmtp_mpu_type_packet_header.packet_sequence_number);
//
//	uint32_t new_tail_pos = tail_pad_size + data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;
//	block_t* temp_mpu_data_unit_payload = block_Alloc(new_tail_pos);
//	block_Write(temp_mpu_data_unit_payload, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos);
//	temp_mpu_data_unit_payload->i_pos = new_tail_pos;
//
//    //hack - keep track of our old du size
//	data_unit->mmtp_mpu_type_packet_header.data_unit_length = data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;
//
//	block_Release(&data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
//	data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload = temp_mpu_data_unit_payload;
}


void __data_unit_recover_null_pad_offset_range_same_sample_id(mmtp_payload_fragments_union_t* data_unit_from, mmtp_payload_fragments_union_t* data_unit_to) {
//	if(data_unit_from->mpu_data_unit_payload_fragments_timed.mpu_sample_number != data_unit_to->mpu_data_unit_payload_fragments_timed.mpu_sample_number ||
//		data_unit_from->mmtp_mpu_type_packet_header.mpu_sequence_number != data_unit_to->mmtp_mpu_type_packet_header.mpu_sequence_number) {
//		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: NULL PAD: sample/mpu numbers do not match: %u, %u", data_unit_from->mpu_data_unit_payload_fragments_timed.sample_number, data_unit_to->mpu_data_unit_payload_fragments_timed.sample_number);
//		return;
//	}

//
//    uint32_t to_null_size;
//    //todo - fix me
//    /**
//
//     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 6 as to_remove
//     atsc3_lls_sls_monitor_output_buffer_utils.c:546:WARN :DETECT: cross SAMPLE LAST OPEN: current mpu: 811, last frag counter: 18, last sample: 1, current sample: 8, current offset: 0
//     atsc3_lls_sls_monitor_output_buffer_utils.c:319:WARN :RECOVER: tail pad: adding 4844 tail to 1398, sample: 8, fragment: 1, mpu_sequence_number: 811, packet_sequence_number: 226474
//     atsc3_lls_sls_monitor_output_buffer_utils.c:546:WARN :DETECT: cross SAMPLE LAST OPEN: current mpu: 811, last frag counter: 1, last sample: 8, current sample: 1, current offset: 4296
//     atsc3_lls_sls_monitor_output_buffer_utils.c:299:WARN :RECOVER: null pad: adding 4296 head to 1432, sample: 1, fragment: 19, mpu_sequence_number: 811, packet_sequence_number: 225568
//     atsc3_lls_sls_monitor_output_buffer_utils.c:319:WARN :RECOVER: tail pad: adding 313152 tail to 5728, sample: 1, fragment: 19, mpu_sequence_number: 811, packet_sequence_number: 225568
//     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 1 as to_remove
//     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 2 as to_remove
//     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 3 as to_remove
//     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 4 as to_remove
//     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 5 as to_remove
//     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 6 as to_remove
//     atsc3_lls_sls_monitor_output_buffer_utils.c:546:WARN :DETECT: cross SAMPLE LAST OPEN: current mpu: 811, last frag counter: 19, last sample: 1, current sample: 8, current offset: 1432
//     atsc3_lls_sls_monitor_output_buffer_utils.c:299:WARN :RECOVER: null pad: adding 1432 head to 257, sample: 8, fragment: 0, mpu_sequence_number: 811, packet_sequence_number: 226475
//     atsc3_lls_sls_monitor_output_buffer_utils.c:319:WARN :RECOVER: tail pad: adding 4262 tail to 1432, sample: 38, fragment: 5, mpu_sequence_number: 811, packet_sequence_number: 223417
//     atsc3_lls_sls_monitor_output_buffer_utils.c:546:WARN :DETECT: cross SAMPLE LAST OPEN: current mpu: 811, last frag counter: 5, last sample: 38, current sample: 1, current offset: 21480
//     atsc3_lls_sls_monitor_output_buffer_utils.c:299:WARN :RECOVER: null pad: adding 21480 head to 1432, sample: 1, fragment: 17, mpu_sequence_number: 811, packet_sequence_number: 226947
//     atsc3_lls_sls_monitor_output_buffer_utils.c:491:WARN :DETECT: INTRA sample: 1, current packet_sequence_number: 225569, last packet_sequence_number: 226947, missing: 4294965917
//     atsc3_lls_sls_monitor_output_buffer_utils.c:357:WARN :RECOVER: null pad RANGE: from: sample number: 1, offset: 21480, to: sample_number: 1, offset: 5728, adding 4294950112 head to 1432, to packet_sequence_number: 225569, from packet_sequence_number: 226947, mpu_sequence_number: 811
//     atsc3_listener_metrics_ncurses(33521,0x700005b2b000) malloc: *** mach_vm_map(size=18446744073709539328) failed (error code=3)
//     *** error: can't allocate region
//     **/
//    if(data_unit_from->mmtp_mpu_type_packet_header.data_unit_length) {
//        //check for - wraparound
//        int32_t to_null_size_i = data_unit_to->mpu_data_unit_payload_fragments_timed.offset - (data_unit_from->mpu_data_unit_payload_fragments_timed.offset + data_unit_from->mpu_data_unit_payload_fragments_timed.data_unit_length);
//        if(to_null_size_i < 0) {
//            to_null_size = data_unit_to->mpu_data_unit_payload_fragments_timed.offset;
//        } else {
//            to_null_size = to_null_size_i;
//        }
//    }  else {
//        to_null_size = data_unit_to->mpu_data_unit_payload_fragments_timed.offset - (data_unit_from->mpu_data_unit_payload_fragments_timed.offset + data_unit_from->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_pos);
//    }
//    __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: null pad RANGE: from: sample number: %u, offset: %u, to: sample_number: %u, offset: %u, adding %u head to %u, to packet_sequence_number: %u, from packet_sequence_number: %u, mpu_sequence_number: %u",
//            data_unit_from->mpu_data_unit_payload_fragments_timed.sample_number,
//            data_unit_from->mpu_data_unit_payload_fragments_timed.offset,
//            data_unit_to->mpu_data_unit_payload_fragments_timed.sample_number,
//            data_unit_to->mpu_data_unit_payload_fragments_timed.offset,
//           to_null_size,
//           data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos,
//			data_unit_to->mmtp_mpu_type_packet_header.packet_sequence_number,
//			data_unit_from->mmtp_mpu_type_packet_header.packet_sequence_number,
//			data_unit_to->mmtp_mpu_type_packet_header.mpu_sequence_number);
//
//
//	block_t* temp_mpu_data_unit_payload = block_Alloc(to_null_size + data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos);
//	temp_mpu_data_unit_payload->i_pos = to_null_size;
//	block_Write(temp_mpu_data_unit_payload, data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos);
//
//    //hack - keep track of our old du size
//    data_unit_to->mmtp_mpu_type_packet_header.data_unit_length = data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;
//
//	block_Release(&data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
//	data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload = temp_mpu_data_unit_payload;
}

//mmtp_payload_fragments_union_t* video_last_data_unit = NULL;
//mmtp_payload_fragments_union_t* audio_last_data_unit = NULL;
//
//
//int lls_sls_monitor_output_buffer_copy_and_recover_audio_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, mmtp_payload_fragments_union_t* audio_data_unit) {
//
//	lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff = &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff;
//	block_t* audio_mpu_data_unit_payload = audio_data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload;
//
//	//find our matching entry and append...
//
//	trun_sample_entry_t* trun_sample_entry = NULL;
//
//	for(int i=0; i < lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.count; i++) {
//		if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.data[i]->movie_fragment_sequence_number == audio_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number) {
//			trun_sample_entry = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.data[i];
//		}
//	}
//	if(!trun_sample_entry) {
//		trun_sample_entry = trun_sample_entry_new();
//		trun_sample_entry->movie_fragment_sequence_number = audio_data_unit->mpu_data_unit_payload_fragments_timed.mmth_movie_fragment_sequence_number;
//		trun_sample_entry->sample_offset = audio_data_unit->mpu_data_unit_payload_fragments_timed.mmth_offset;
//		trun_sample_entry->sample_length = audio_data_unit->mpu_data_unit_payload_fragments_timed.mmth_length;
//		trun_sample_entry->sample = block_Alloc(trun_sample_entry->sample_length);
//		lls_sls_monitor_buffer_isobmff_add_trun_sample_entry(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff, trun_sample_entry);
//	}
//	//null out any missing offsets
//	block_Write(trun_sample_entry->sample, audio_mpu_data_unit_payload->p_buffer, audio_mpu_data_unit_payload->i_pos);
//
//	//from hint box
//	//	trun_sample_entry->sample_duration = audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_duration;
//
//	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("added trun entry: sample index %u is now sample_size: %u", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.trun_sample_entry_v.count, trun_sample_entry->sample_length);
//
//	return trun_sample_entry->sample->i_pos;
	//alloc out

	//assume audio will lose samples rather than fragments
//
//    if(lls_sls_monitor_buffer_isobmff->last_fragment && audio_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number == lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number &&
//        audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number-1 != lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number) {
//
//        uint32_t from_mfu_sample_number = lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number + 1;
//
//        uint32_t to_mfu_sample_number = audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number - 1;
//
//        for(int i = from_mfu_sample_number; i < to_mfu_sample_number && i < moof_box_trun_sample_entry_vector->size; i++) {
//            trun_sample_entry_t* missing_mfu_sample_entry = moof_box_trun_sample_entry_vector->data[i-1];
//            __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: TRUNCATE SAMPLE: A: updating moof_box_trun_sample_entry vector to mark %u as to_remove", i);
//            missing_mfu_sample_entry->to_remove_sample_entry = true;
//        }
//    } else if(lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment &&
//            audio_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number != lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number &&
//              ((moof_box_trun_sample_entry_vector && moof_box_trun_sample_entry_vector->size > lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.sample_number) ||
//            audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number != 1)) {
//        //compute mpu fragment variances
//
////        uint32_t missing_packets = audio_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number - lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number - 1;
//        __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: cross MPU sample mpu: A: current mpu: %u, last mpu: %u, current sample: %u, last sample: %u, current packet_sequence_number: %u, last packet_sequence_number: %u",
//                audio_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
//                lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
//                audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number,
//                lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.sample_number,
//                audio_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number,
//                lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number);
//    }
//
//copy_packet:
//    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box) {
//        lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, sizeof(uint8_t));
//    }
//
//
//    lls_sls_monitor_buffer_isobmff->last_fragment = audio_data_unit;
//    lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment = audio_data_unit;
//    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, audio_data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
//}


//todo - refactor this out

/**
 * rebuild trun sample entries from the mmthsample header
 * note: some pcaps (e.g. some in-order) may not have a proper +8 offset
 *
 * offset − the offset of the media data contained in this MFU.
 * The offset base is the beginning of the containing “mdat” box.
 * MFU shall be placed at the position that offset indicates.
 *
 *
 */

int lls_sls_monitor_output_buffer_copy_and_recover_sample_fragment_block(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, mmtp_payload_fragments_union_t* data_unit) {

	block_t* mpu_data_unit_payload = data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload;
    trun_sample_entry_t* trun_sample_entry = NULL;
	uint32_t next_sample_offset_calculated = 0;
	
    //hacks
    //check if we need to add in +8 for mdat box length not captured in mmthsample offset
    //needed for DS-MMT IO 3point5mb 720p5994.pcap
    if(data_unit->mpu_data_unit_payload_fragments_timed.mmth_samplenumber == 1 && data_unit->mpu_data_unit_payload_fragments_timed.mmth_offset < 8) {
        __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("trun_sample_entry: mpu_sequence_number: %u, samplenumber: %u, mmth_offset is: %u, setting trun_mmthsample_missing_offset_mdat_box to true",
                                                    data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
                                                    data_unit->mpu_data_unit_payload_fragments_timed.mmth_offset,
                                                    data_unit->mpu_data_unit_payload_fragments_timed.mpu_sample_number);
        lls_sls_monitor_buffer_isobmff->trun_mmthsample_missing_offset_mdat_box = true;
        
    }
    
    for(int i=0; i < lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.count && !trun_sample_entry; i++) {
//        __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE("vector pos: %u, checking trun_sample_entry: %u against mpu_sample_number: %u",
//                i,
//                lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.data[i]->samplenumber,
//                data_unit->mpu_data_unit_payload_fragments_timed.mpu_sample_number);
//
        //hacks
        //needed for 2018-12-17-mmt-airwavz-recalc-trimmed-155s.pcap
        if(lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.data[i]->samplenumber == data_unit->mpu_data_unit_payload_fragments_timed.mpu_sample_number) {
			trun_sample_entry = lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.data[i];
            
            //hacks
            //needed for 2018-12-17-mmt-airwavz-recalc-trimmed-155s.pcap
            if(trun_sample_entry->mfu_mmth_last_offset <= 8 && trun_sample_entry->mfu_mmth_last_sample_size  &&
               ((trun_sample_entry->mfu_mmth_last_header_sample_size + trun_sample_entry->mfu_mmth_last_sample_size) == data_unit->mpu_data_unit_payload_fragments_timed.mpu_offset)) {
                
                __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("setting trun_mmthsample_offset_includes_header: true, mpu_seq_num: %u, mpu_sample_num: %u, mmth_last_offset: %u, last_header_sample_size: %u, last_sample_size: %u, mpu offset: %u",
                                                           data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
                                                           data_unit->mpu_data_unit_payload_fragments_timed.mpu_sample_number,
                                                           trun_sample_entry->mfu_mmth_last_offset,
                                                           trun_sample_entry->mfu_mmth_last_header_sample_size,
                                                           trun_sample_entry->mfu_mmth_last_sample_size,
                                                           data_unit->mpu_data_unit_payload_fragments_timed.mpu_offset);
                
                   trun_sample_entry->trun_mmthsample_offset_includes_header = true;
            }
		}

        next_sample_offset_calculated = lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.data[i]->sample_offset + lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.data[i]->sample_length;
	}
    
    if(!trun_sample_entry) {
        //hacks
        if(lls_sls_monitor_buffer_isobmff->trun_mmthsample_missing_offset_mdat_box) {
            data_unit->mpu_data_unit_payload_fragments_timed.mmth_offset += 8;
        }
        
		trun_sample_entry = trun_sample_entry_new();
		//use our mmthsample box information if present, otherwise pre-allocate based upon our sample and offset
		if(data_unit->mpu_data_unit_payload_fragments_timed.mmth_samplenumber) {
            
            //hack
            trun_sample_entry->mfu_mmth_last_header_sample_size = data_unit->mpu_data_unit_payload_fragments_timed.mfu_mmth_sample_header_size;
            trun_sample_entry->mfu_mmth_cum_header_sample_size += trun_sample_entry->mfu_mmth_last_header_sample_size;
            trun_sample_entry->mfu_mmth_last_offset = data_unit->mpu_data_unit_payload_fragments_timed.mpu_offset;
            trun_sample_entry->mfu_mmth_last_sample_size = data_unit->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_pos;
            
            trun_sample_entry->samplenumber = data_unit->mpu_data_unit_payload_fragments_timed.mmth_samplenumber;
			trun_sample_entry->sequence_number = data_unit->mpu_data_unit_payload_fragments_timed.mmth_sequence_number;
			trun_sample_entry->movie_fragment_sequence_number = data_unit->mpu_data_unit_payload_fragments_timed.mmth_movie_fragment_sequence_number;
			trun_sample_entry->sample_offset = data_unit->mpu_data_unit_payload_fragments_timed.mmth_offset;  //used for recompositing the full mdat box
			trun_sample_entry->sample_length = data_unit->mpu_data_unit_payload_fragments_timed.mmth_length;
            
			trun_sample_entry->sample = block_Alloc(trun_sample_entry->sample_length);
            __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("Present mmthsample box: sequence_number: %u, samplenumber: %u, sample_offset: %u, sample_length: %u, mfu_mmth_sample_header_size length: %u",
							trun_sample_entry->sequence_number,
							trun_sample_entry->samplenumber,
							trun_sample_entry->sample_offset,
							trun_sample_entry->sample_length,
                            data_unit->mpu_data_unit_payload_fragments_timed.mfu_mmth_sample_header_size);
		} else {
			trun_sample_entry->mmth_box_missing = true;
            trun_sample_entry->mfu_mmth_sample_header_size = 0; //unknown
			trun_sample_entry->samplenumber 	= data_unit->mpu_data_unit_payload_fragments_timed.mpu_sample_number;
            trun_sample_entry->sequence_number 	= 0; //unknown - data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number;
			trun_sample_entry->movie_fragment_sequence_number = data_unit->mpu_data_unit_payload_fragments_timed.mpu_movie_fragment_sequence_number;
			trun_sample_entry->sample_offset 	= next_sample_offset_calculated + 8; // we won't know until we rebuild the box
			trun_sample_entry->sample_length	= 0; // will be added in later
            
            //mpu_offset includes mfu_mmth_sample_header_size but we don't know our actual header size
			trun_sample_entry->sample = block_Alloc(data_unit->mpu_data_unit_payload_fragments_timed.mpu_offset + mpu_data_unit_payload->i_pos);

			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("Missing mmthsample box: sequence_number: %u, samplenumber: %u, setting sample_offset to: %u",
					trun_sample_entry->sequence_number,
					trun_sample_entry->samplenumber,
					trun_sample_entry->sample_offset);

		}
		lls_sls_monitor_buffer_isobmff_add_trun_sample_entry(lls_sls_monitor_buffer_isobmff, trun_sample_entry);
	}

    //hacks
	if(trun_sample_entry->mmth_box_missing) {
        //trun_sample_entry->sample_offset
		trun_sample_entry->sample_length += data_unit->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_pos;
	}
    
    //hacks
    if(trun_sample_entry->trun_mmthsample_offset_includes_header) {
        //mmth_offset is a global offset from the base of the mdat box
        if(data_unit->mpu_data_unit_payload_fragments_timed.mmth_offset > trun_sample_entry->mfu_mmth_cum_header_sample_size) {
            data_unit->mpu_data_unit_payload_fragments_timed.mmth_offset -= trun_sample_entry->mfu_mmth_cum_header_sample_size;
            trun_sample_entry->sample_offset -= trun_sample_entry->mfu_mmth_cum_header_sample_size;
        }
        //mpu offset is on a per sample basis
        if(data_unit->mpu_data_unit_payload_fragments_timed.mpu_offset > trun_sample_entry->mfu_mmth_last_header_sample_size) {
            data_unit->mpu_data_unit_payload_fragments_timed.mpu_offset -= trun_sample_entry->mfu_mmth_last_header_sample_size;
        }
    }

	uint32_t mpu_offset = data_unit->mpu_data_unit_payload_fragments_timed.mpu_offset; //0-based...
    
	if(trun_sample_entry->sample->p_size < mpu_offset + data_unit->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_pos) {
		//realloc us so we can seek to the proper next sample fragment position
		block_Resize(trun_sample_entry->sample, mpu_offset + data_unit->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_pos);
	}
    
    //offset by mfu_mmth_sample_header_size as mpu_offset includes this data payloads
	block_Seek(trun_sample_entry->sample, mpu_offset);
    __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("appending mpu_seq_num: %u, mpu_sample_number: %u, sample seq_num: %u, samplenum: %u, movie_fragment_sequence_num: %u, sample_offset: %u, original mpu_offset: %u, recalc mpu_offset: %u, sample_len: %u, du len: %u",
                                data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
                                data_unit->mpu_data_unit_payload_fragments_timed.mpu_sample_number,
                                trun_sample_entry->sequence_number,
								trun_sample_entry->samplenumber,
                                trun_sample_entry->movie_fragment_sequence_number,
                                trun_sample_entry->sample_offset,
                                data_unit->mpu_data_unit_payload_fragments_timed.mpu_offset,
								mpu_offset,
                                trun_sample_entry->sample_length,
								data_unit->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_pos);

	block_Append(trun_sample_entry->sample, data_unit->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload);

	//skip over any missing sample packets
	//block_Write(&trun_sample_entry->sample[mpu_offset], mpu_data_unit_payload->p_buffer, mpu_data_unit_payload->i_pos);

	return trun_sample_entry->sample->i_pos;



		//from hint box
		//	trun_sample_entry->sample_duration = audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_duration;

//	trun_sample_entry_vector_t* moof_box_trun_sample_entry_vector = lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_trun_sample_entry_vector;
//
//	//we need to rebuild our trun here
//	if(lls_sls_monitor_buffer_isobmff->moof_box_is_from_last_mpu) {
//		uint32_t trun_sample_index = video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number-1;
//		if(trun_sample_index >= moof_box_trun_sample_entry_vector->size) {
//			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("V: trun sample index %u is greater than size: %u", trun_sample_index, moof_box_trun_sample_entry_vector->size);
//			//TODO - append this in our trun box
//			return -1;
//		}
//		moof_box_trun_sample_entry_vector->data[trun_sample_index]->sample_size += video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_pos;
//		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("RECOVERY: REBUILD TRUN: V: sample index %u is now sample_size: %u", trun_sample_index, moof_box_trun_sample_entry_vector->data[trun_sample_index]->sample_size);
//
//		goto copy_packet;
//	}
//
//	if(!lls_sls_monitor_buffer_isobmff->last_fragment) {
//
//		//we should start with sample == 1, offset == 0, otherwise pad this out
//		//intra mfu packet loss
//		if(video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number == 1 && video_data_unit->mpu_data_unit_payload_fragments_timed.offset != 0) {
//			uint32_t lost_mfu_packets_count = 1;
//
//			if(lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment) {
//				lost_mfu_packets_count = (video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number -  lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number);
//			}
//
//			lls_sls_monitor_buffer_isobmff->last_fragment_lost_mfu_count += lost_mfu_packets_count;
//
//			__data_unit_recover_null_pad_offset(video_data_unit);
//			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: first MFU: V: lost %u mfu packets, sample_number: 0, offset: %u, building null payload for mdat, new fragment size: %u", lost_mfu_packets_count, video_data_unit->mpu_data_unit_payload_fragments_timed.offset, video_mpu_data_unit_payload->i_pos);
//		} else if(video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number >1 ) {
//			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: first SAMPLE: V: starting sample: %u, offset: %u",  video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number, video_data_unit->mpu_data_unit_payload_fragments_timed.offset);
//			//TODO - null this out of our mdat box and adjust offset accordingly
//		}
//	} else {
//		//compute intra sample variances
//		if(video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number == lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number &&
//				video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number == lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number &&
//				video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number - 1 != lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number) {
//			uint32_t missing_packets = video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number - lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number - 1;
//			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: INTRA sample: %u, current packet_sequence_number: %u, last packet_sequence_number: %u, missing: %u", video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number , video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number, lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number, missing_packets);
//
//			__data_unit_recover_null_pad_offset_range_same_sample_id(lls_sls_monitor_buffer_isobmff->last_fragment, video_data_unit);
//
//		} else if(video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number == lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number &&
//				video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number != lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number &&
//				lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_counter) {
//
//			//compute intra fragment variances, remember we should have an MFU inbetween..
//
//			//we can't null pad this out without interriogating the mdat
//
//			/**
//			 *
//			 * atsc3_lls_sls_monitor_output_buffer_utils.c:327:WARN :DETECT: cross SAMPLE LAST OPEN: current mpu: 6138, last frag counter: 1, last sample: 10, current sample: 11, current offset: 2864
//			 * .sample_number are 1 based while the trun_sample entry is 0 based
//			 *
//			 * atsc3_lls_sls_monitor_output_buffer_utils.c:222:WARN :RECOVER: null pad: adding 2864 head to 653, mpu_sequence_number: 6138, packet_sequence_number: 2917408
//			 *
//			 * null inject, should remove sample from moof/trun
//			 *
//			 */
//
//			uint32_t from_mfu_sample_number = lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number - 1;
//			uint32_t from_mfu_trun_sample_size = 0;
//			int32_t from_mfu_to_tail_pad_size = 0;
//			trun_sample_entry_t* from_missing_mfu_sample_entry = NULL;
//
//			//if we have this sample in-order moof box, then compute offset size...
//			if(from_mfu_sample_number <= moof_box_trun_sample_entry_vector->size) {
//				from_missing_mfu_sample_entry = moof_box_trun_sample_entry_vector->data[from_mfu_sample_number];
//				from_mfu_trun_sample_size = from_missing_mfu_sample_entry->sample_size;
//
//				//compute our last fragment size against the trun to pad out...
//				if(lls_sls_monitor_buffer_isobmff->last_fragment->mmtp_mpu_type_packet_header.data_unit_length) {
//					from_mfu_to_tail_pad_size = from_mfu_trun_sample_size - lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.offset - lls_sls_monitor_buffer_isobmff->last_fragment->mmtp_mpu_type_packet_header.data_unit_length;
//				} else {
//					from_mfu_to_tail_pad_size = from_mfu_trun_sample_size - lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.offset - lls_sls_monitor_buffer_isobmff->last_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;
//				}
//				if(from_mfu_to_tail_pad_size > 0) {
//					__data_unit_recover_null_pad_tail(lls_sls_monitor_buffer_isobmff->last_fragment, from_mfu_to_tail_pad_size);
//				} else {
//					//we should not get a negative size here unless our moof box is wrong (or missing)...
//					__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: ERROR: last fragment tail pad from mpu_sequence_number: %u, sample: %u is size: %u",
//							lls_sls_monitor_buffer_isobmff->last_fragment->mmtp_mpu_type_packet_header.mpu_sequence_number,
//							lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number,
//							from_mfu_to_tail_pad_size);
//				}
//			}
//
//            uint32_t to_mfu_sample_number = video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number - 1;
//
//			for(int i = from_mfu_sample_number + 1; i < to_mfu_sample_number && i < moof_box_trun_sample_entry_vector->size; i++) {
//				trun_sample_entry_t* missing_mfu_sample_entry = moof_box_trun_sample_entry_vector->data[i];
//				__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: (TODO FIXME: NOT) REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark %u as to_remove", i);
//				missing_mfu_sample_entry->to_remove_sample_entry = true;
//			}
//
//			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: cross SAMPLE LAST OPEN: current mpu: %u, last frag counter: %u, last sample: %u, current sample: %u, current offset: %u",
//					video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
//					lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_counter,
//					lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number,
//					video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number,
//					video_data_unit->mpu_data_unit_payload_fragments_timed.offset);
//
//			//we can null pad the offset size
//			uint32_t missing_current_bytes = video_data_unit->mpu_data_unit_payload_fragments_timed.offset;
//
//			if(missing_current_bytes) {
//				__data_unit_recover_null_pad_offset(video_data_unit);
//			}
//
//		} else if(lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment &&
//				video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number != lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number &&
//				video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number - 1 != lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number) {
//			//compute mpu fragment variances
//
//			uint32_t missing_packets = video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number - lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number - 1;
//			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: cross MPU sample mpu: V: current mpu: %u, last mpu: %u, current sample: %u, last sample: %u, current packet_sequence_number: %u, last packet_sequence_number: %u, missing: %u",
//					video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
//					lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
//					video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number,
//					lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number,
//					video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number,
//					lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number,
//					missing_packets);
//		}
//	}
//
//copy_packet:
//    if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box) {
//        lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, sizeof(uint8_t));
//    }
//
//    //TODO - fix me
//    if(moof_box_trun_sample_entry_vector && video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number <= moof_box_trun_sample_entry_vector->size) {
//       	moof_box_trun_sample_entry_vector->data[video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number-1]->has_matching_sample = true;
//       	moof_box_trun_sample_entry_vector->data[video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number-1]->sample_size += video_data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;
//    }
//
//    lls_sls_monitor_buffer_isobmff->last_fragment = video_data_unit;
//    lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment = video_data_unit;



  //  return block_Append((lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, video_mpu_data_unit_payload);
}


int lls_sls_monitor_buffer_isobmff_create_mdat_from_trun_sample_entries(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff_to_create_mdat) {

	if(!lls_sls_monitor_buffer_isobmff_to_create_mdat->trun_sample_entry_v.count) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_buffer_isobmff_create_mdat_from_trun_sample_entries count is 0: track_id: %u", lls_sls_monitor_buffer_isobmff_to_create_mdat->track_id);
		return 0;
	}

	uint32_t sample_length_cumulative = 0;
	uint32_t sample_offset_plus_last_length = 0;

	for(int i=0; i < lls_sls_monitor_buffer_isobmff_to_create_mdat->trun_sample_entry_v.count; i++) {
		trun_sample_entry_t* trun_sample_entry = lls_sls_monitor_buffer_isobmff_to_create_mdat->trun_sample_entry_v.data[i];
		sample_length_cumulative += trun_sample_entry->sample_length;
        
		uint32_t current_sample_offset_end = trun_sample_entry->sample_offset + trun_sample_entry->sample_length;
        //strip mdat box header from sample_offset as it's added below..
        if(current_sample_offset_end > 8) {
            current_sample_offset_end -= 8;
        }
		if(current_sample_offset_end > sample_offset_plus_last_length) {
			sample_offset_plus_last_length = current_sample_offset_end;
		}
	}
    
    __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("lls_sls_monitor_buffer_isobmff_create_mdat_from_trun_sample_entries: building mdat size of: sample_length_cumulative: %u, sample_offset_plus_last_length: %u",
                                               sample_length_cumulative,
                                               sample_offset_plus_last_length);
    


	uint32_t mdat_size = sample_offset_plus_last_length + 8;
	block_t* temp_mmt_mdat = block_Alloc(mdat_size);

	uint32_t mdat_size_nl = htonl(mdat_size);
	//write out our box size
	block_Write(temp_mmt_mdat, (uint8_t*)&mdat_size_nl, 4);
	const char* MDAT_ATOM = "mdat";
	block_Write(temp_mmt_mdat, (uint8_t*)MDAT_ATOM, 4);

	//copy all of our alloc'd fragments
    //mmth sample is already offset by 8
	uint32_t last_sample_offset = 0;
	for(int i=0; i < lls_sls_monitor_buffer_isobmff_to_create_mdat->trun_sample_entry_v.count; i++) {
		trun_sample_entry_t* trun_sample_entry = lls_sls_monitor_buffer_isobmff_to_create_mdat->trun_sample_entry_v.data[i];
		block_Seek(temp_mmt_mdat, trun_sample_entry->sample_offset);
        __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("lls_sls_monitor_buffer_isobmff_create_mdat_from_trun_sample_entries: appending sample %u at offset: %u, sample length: %u block_t length: %u",  trun_sample_entry->samplenumber,
                                            trun_sample_entry->sample_offset,
                                                   trun_sample_entry->sample_length,
                                                   trun_sample_entry->sample->i_pos);
        
		block_Append(temp_mmt_mdat, trun_sample_entry->sample);
		//last_sample_offset = trun_sample_entry->sample_offset + trun_sample_entry->sample_length;
	}

	lls_sls_monitor_buffer_isobmff_to_create_mdat->mmt_mdat_block = temp_mmt_mdat;
	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("lls_sls_monitor_buffer_isobmff_create_mdat_from_trun_sample_entries: setting new mdat size: %u", mdat_size);

	return mdat_size;
}

block_t* lls_sls_monitor_output_buffer_copy_alc_full_isobmff_box(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff) {
	if(!lls_sls_monitor_buffer_isobmff->init_block || !lls_sls_monitor_buffer_isobmff->init_block->i_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_output_buffer_copy_audio_alc_full_isobmff_box: init box: is: %p or len is 0", lls_sls_monitor_buffer_isobmff->init_block);
		return NULL;
	}
	if(!lls_sls_monitor_buffer_isobmff->alc_moof_mdat_block || ! lls_sls_monitor_buffer_isobmff->alc_moof_mdat_block->i_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_output_buffer_copy_audio_alc_full_isobmff_box: moof_mdat is: %p or len is 0", lls_sls_monitor_buffer_isobmff->alc_moof_mdat_block);
		return NULL;
	}

	uint32_t full_box_size = lls_sls_monitor_buffer_isobmff->init_block->i_pos + lls_sls_monitor_buffer_isobmff->alc_moof_mdat_block->i_pos;

	if(full_box_size <= 0) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_output_buffer_copy_alc_full_isobmff_box size <= 0, %u, returning NULL", full_box_size);
		return NULL;
	}
	block_t* isobmff_alc_full_block = block_Alloc(full_box_size);

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("lls_sls_monitor_output_buffer_copy_alc_full_isobmff_box: total size: %u, init size: %u, moof size: %u", full_box_size,
			lls_sls_monitor_buffer_isobmff->init_block->i_pos,
			lls_sls_monitor_buffer_isobmff->alc_moof_mdat_block->i_pos);

	block_Append(isobmff_alc_full_block, lls_sls_monitor_buffer_isobmff->init_block);
	block_Append(isobmff_alc_full_block, lls_sls_monitor_buffer_isobmff->alc_moof_mdat_block);

	return isobmff_alc_full_block;
}


block_t* lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff) {
	if(!lls_sls_monitor_buffer_isobmff->init_block || !lls_sls_monitor_buffer_isobmff->init_block->i_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_output_buffer_copy_mmt_full_isobmff_box: init box: is: %p or len is 0", lls_sls_monitor_buffer_isobmff->init_block);
		return NULL;
	}
	if(!lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow || ! lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->i_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box: mmt_moof_block_from_flow is: %p or len is 0", lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow);
		return NULL;
	}

	if(!lls_sls_monitor_buffer_isobmff->mmt_mdat_block || ! lls_sls_monitor_buffer_isobmff->mmt_mdat_block->i_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box: mmt_mdat_block is: %p or len is 0", lls_sls_monitor_buffer_isobmff->mmt_mdat_block);
		return NULL;
	}

	uint32_t full_box_size = lls_sls_monitor_buffer_isobmff->init_block->i_pos + lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->i_pos - 8 + lls_sls_monitor_buffer_isobmff->mmt_mdat_block->i_pos;

	if(full_box_size <= 0) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box size <= 0, %u, returning NULL", full_box_size);
		return NULL;
	}
	block_t* isobmff_mmt_full_flow_block = block_Alloc(full_box_size);

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("copy lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box: total size: %u, init size: %u, moof size: %u, mdat size: %u",
			full_box_size,
			lls_sls_monitor_buffer_isobmff->init_block->i_pos,
			lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->i_pos - 8,
			lls_sls_monitor_buffer_isobmff->mmt_mdat_block->i_pos);

	block_Append(isobmff_mmt_full_flow_block, lls_sls_monitor_buffer_isobmff->init_block);
	block_Append(isobmff_mmt_full_flow_block, lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow);
	block_Seek(isobmff_mmt_full_flow_block, isobmff_mmt_full_flow_block->i_pos - 8);

	//chopm off the old mdat box
	block_Append(isobmff_mmt_full_flow_block, lls_sls_monitor_buffer_isobmff->mmt_mdat_block);

	return isobmff_mmt_full_flow_block;
}


//moof blocks will have already been patched earlier...
block_t* lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box_no_patching_trailing_mdat(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff) {
	if(!lls_sls_monitor_buffer_isobmff || !lls_sls_monitor_buffer_isobmff->init_block || !lls_sls_monitor_buffer_isobmff->init_block->i_pos) {
        __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_output_buffer_copy_mmt_full_isobmff_box: lls_sls_monitor_buffer_isobmff: %p, init box: is: %p or len is 0", lls_sls_monitor_buffer_isobmff, lls_sls_monitor_buffer_isobmff ? lls_sls_monitor_buffer_isobmff->init_block ? lls_sls_monitor_buffer_isobmff->init_block : 0 : 0);
		return NULL;
	}

	block_t* target_moof_block = NULL;

	if(lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow && lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->i_pos) {
		target_moof_block = lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow;
	} else if(lls_sls_monitor_buffer_isobmff->mmt_moof_block_previous_mpu && lls_sls_monitor_buffer_isobmff->mmt_moof_block_previous_mpu->i_pos) {
		target_moof_block = lls_sls_monitor_buffer_isobmff->mmt_moof_block_previous_mpu;
	} else {
		//TODO: build a dummy moof block here..

		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box_no_patching_trailing_mdat: mmt_moof_block_from_flow is: %p or len is 0", lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow);
		return NULL;
	}

	if(!lls_sls_monitor_buffer_isobmff->mmt_mdat_block || ! lls_sls_monitor_buffer_isobmff->mmt_mdat_block->i_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box_no_patching_trailing_mdat: mmt_mdat_block is: %p or len is 0", lls_sls_monitor_buffer_isobmff->mmt_mdat_block);
		return NULL;
	}

	uint32_t full_box_size = lls_sls_monitor_buffer_isobmff->init_block->i_pos + target_moof_block->i_pos + lls_sls_monitor_buffer_isobmff->mmt_mdat_block->i_pos;

	if(full_box_size <= 0) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box_no_patching_trailing_mdat size <= 0, %u, returning NULL", full_box_size);
		return NULL;
	}
	block_t* isobmff_mmt_full_flow_block = block_Alloc(full_box_size);

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("copy lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box_no_patching_trailing_mdat: total size: %u, init size: %u, moof size: %u, mdat size: %u",
			full_box_size,
			lls_sls_monitor_buffer_isobmff->init_block->i_pos,
			target_moof_block->i_pos,
			lls_sls_monitor_buffer_isobmff->mmt_mdat_block->i_pos);

	block_Append(isobmff_mmt_full_flow_block, lls_sls_monitor_buffer_isobmff->init_block);
	block_Append(isobmff_mmt_full_flow_block, target_moof_block);
	block_Append(isobmff_mmt_full_flow_block, lls_sls_monitor_buffer_isobmff->mmt_mdat_block);

	return isobmff_mmt_full_flow_block;
}


void lls_slt_monitor_check_and_handle_pipe_ffplay_buffer_is_shutdown(lls_slt_monitor_t* lls_slt_monitor) {
	if(lls_slt_monitor->lls_sls_alc_monitor) {
		if(lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer) {
			bool is_shutdown_alc = pipe_buffer_reader_check_if_shutdown(&lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);
			if(is_shutdown_alc) {
				__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("lls_slt_monitor: ffplay is shutdown for ALC service_id: %u, setting ffplay_output_enabled = false", lls_slt_monitor->lls_sls_alc_monitor->service_id);
				lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled = false;
				lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer.should_flush_output_buffer = false;
			}
		}

	}

	if(lls_slt_monitor->lls_sls_mmt_monitor) {
		if(lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer) {
			bool is_shutdown_mmt = pipe_buffer_reader_check_if_shutdown(&lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);
			if(is_shutdown_mmt) {
				__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("lls_slt_monitor: ffplay is shutdown for MMT service_id: %u, setting ffplay_output_enabled = false",
						lls_slt_monitor->lls_sls_mmt_monitor->service_id);
				lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled = false;
			}
		}
	}
}

void lls_sls_monitor_output_buffer_alc_file_dump(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, const char* directory_path, uint32_t mpu_sequence_number_audio, uint32_t mpu_sequence_number_video) {

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("lls_sls_monitor_output_buffer_alc_file_dump: dumping to %s, audio sequence number: %u, video sequence number: %u", directory_path, mpu_sequence_number_audio, mpu_sequence_number_video);
	//just to be sure...
	mkdir(directory_path, 0777);

	//build our recon mpu
	uint32_t mpu_sequence_number_min = __MIN(mpu_sequence_number_audio, mpu_sequence_number_video);
    char* box_track_dump_recon_filename = (char*)calloc(128, sizeof(char));
    snprintf(box_track_dump_recon_filename, 127, "%s/%u.b", directory_path, mpu_sequence_number_min);

    FILE* box_track_dump_recon_fp = fopen(box_track_dump_recon_filename, "w");
    if(box_track_dump_recon_fp) {
    	fwrite(lls_sls_monitor_output_buffer->joined_isobmff_block->p_buffer, lls_sls_monitor_output_buffer->joined_isobmff_block->p_size, 1, box_track_dump_recon_fp);
    	fclose(box_track_dump_recon_fp);
    	free(box_track_dump_recon_filename);
    }

    //dump a track
    char* box_track_dump_a_filename = (char*)calloc(128, sizeof(char));
    snprintf(box_track_dump_a_filename, 127, "%s/%u.a", directory_path, mpu_sequence_number_audio);


    FILE* box_track_dump_a_fp = fopen(box_track_dump_a_filename, "w");
    if(box_track_dump_a_fp) {
    	fwrite(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block->p_buffer, 		  lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block->i_pos, 1, box_track_dump_a_fp);
    	fwrite(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.alc_moof_mdat_block->p_buffer , lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.alc_moof_mdat_block->i_pos, 1, box_track_dump_a_fp);
    	fclose(box_track_dump_a_fp);
    	free(box_track_dump_a_filename);
    }

    //dump v track
    char* box_track_dump_v_filename = (char*)calloc(128, sizeof(char));
    snprintf(box_track_dump_v_filename, 127, "%s/%u.v", directory_path, mpu_sequence_number_video);

    FILE* box_track_dump_v_fp = fopen(box_track_dump_v_filename, "w");
    if(box_track_dump_v_fp) {
    	fwrite(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block->p_buffer, 		  lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block->i_pos, 1, box_track_dump_v_fp);
    	fwrite(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.alc_moof_mdat_block->p_buffer , lls_sls_monitor_output_buffer->video_output_buffer_isobmff.alc_moof_mdat_block->i_pos, 1, box_track_dump_v_fp);

    	fclose(box_track_dump_v_fp);
    	free(box_track_dump_v_filename);
    }
}



void lls_sls_monitor_buffer_isobmff_intermediate_mmt_file_dump(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, const char* directory_path, uint32_t mpu_sequence_number, const char* prefix) {
    if(!lls_sls_monitor_buffer_isobmff || !lls_sls_monitor_buffer_isobmff->init_block || !lls_sls_monitor_buffer_isobmff->mmt_mdat_block) {
        __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("lls_sls_monitor_buffer_isobmff_mmt_file_dump: lls_sls_monitor_buffer_isobmff missing: %p", lls_sls_monitor_buffer_isobmff);

        return;
    }

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("lls_sls_monitor_buffer_isobmff_mmt_file_dump: dumping to %s, mpu_sequence_number: %u, prefix: %s", directory_path, mpu_sequence_number, prefix);
	//just to be sure...
	mkdir(directory_path, 0777);

	//build our recon mpu
    char* track_dump_file_name = (char*)calloc(128, sizeof(char));
    snprintf(track_dump_file_name, 127, "%s/%u.%s", directory_path, mpu_sequence_number, prefix);

    FILE* track_dump_recon_fp = fopen(track_dump_file_name, "w");
    if(track_dump_recon_fp) {
    	fwrite(lls_sls_monitor_buffer_isobmff->init_block->p_buffer, lls_sls_monitor_buffer_isobmff->init_block->p_size, 1, track_dump_recon_fp);
    	if(lls_sls_monitor_buffer_isobmff->mmt_moof_block) {
    		fwrite(lls_sls_monitor_buffer_isobmff->mmt_moof_block->p_buffer, lls_sls_monitor_buffer_isobmff->mmt_moof_block->p_size, 1, track_dump_recon_fp);
    	} else if(lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow) {
    		fwrite(lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->p_buffer, lls_sls_monitor_buffer_isobmff->mmt_moof_block_from_flow->p_size, 1, track_dump_recon_fp);

    	}
    	if(lls_sls_monitor_buffer_isobmff->mmt_mdat_block) {
    		fwrite(lls_sls_monitor_buffer_isobmff->mmt_mdat_block->p_buffer, lls_sls_monitor_buffer_isobmff->mmt_mdat_block->p_size, 1, track_dump_recon_fp);
    	}
    	fclose(track_dump_recon_fp);
    	free(track_dump_file_name);
    }

}


void ls_sls_monitor_buffer_isobmff_mmt_mpu_rebuilt_file_dump(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, const char* directory_path, uint32_t mpu_sequence_number, const char* prefix) {
    if(!lls_sls_monitor_buffer_isobmff || !lls_sls_monitor_buffer_isobmff->mmt_mpu_rebuilt) {
        __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("ls_sls_monitor_buffer_isobmff_mmt_mpu_rebuilt_file_dump: lls_sls_monitor_buffer_isobmff is: %p, returning", lls_sls_monitor_buffer_isobmff);
        
        return;
    }
	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("ls_sls_monitor_buffer_isobmff_mmt_mpu_rebuilt_file_dump: dumping to %s, mpu_sequence_number: %u, prefix: %s", directory_path, mpu_sequence_number, prefix);
	//just to be sure...
	mkdir(directory_path, 0777);

	//build our recon mpu
	char* track_dump_file_name = (char*)calloc(128, sizeof(char));
	snprintf(track_dump_file_name, 127, "%s/%u.%s", directory_path, mpu_sequence_number, prefix);

	FILE* track_dump_recon_fp = fopen(track_dump_file_name, "w");
	if(track_dump_recon_fp) {
		fwrite(lls_sls_monitor_buffer_isobmff->mmt_mpu_rebuilt->p_buffer, lls_sls_monitor_buffer_isobmff->mmt_mpu_rebuilt->p_size, 1, track_dump_recon_fp);
		fclose(track_dump_recon_fp);
		free(track_dump_file_name);
	}


}


void lls_sls_monitor_output_buffer_mmt_file_dump(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, const char* directory_path, uint32_t mpu_sequence_number_audio, uint32_t mpu_sequence_number_video) {

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("lls_sls_monitor_output_buffer_mmt_file_dump: dumping to %s, audio sequence number: %u, video sequence number: %u", directory_path, mpu_sequence_number_audio, mpu_sequence_number_video);
	//just to be sure...
	mkdir(directory_path, 0777);

	//build our recon mpu
	uint32_t mpu_sequence_number_min = __MIN(mpu_sequence_number_audio, mpu_sequence_number_video);
    char* box_track_dump_recon_filename = (char*)calloc(128, sizeof(char));
    snprintf(box_track_dump_recon_filename, 127, "%s/%u.b", directory_path, mpu_sequence_number_min);

    FILE* box_track_dump_recon_fp = fopen(box_track_dump_recon_filename, "w");
    if(box_track_dump_recon_fp) {
    	fwrite(lls_sls_monitor_output_buffer->joined_isobmff_block->p_buffer, lls_sls_monitor_output_buffer->joined_isobmff_block->p_size, 1, box_track_dump_recon_fp);
    	fclose(box_track_dump_recon_fp);
    	free(box_track_dump_recon_filename);
    }


	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_from_flow) {
		//dump a track with original moof
		char* box_track_dump_moof_flow_a_filename = (char*)calloc(128, sizeof(char));
		snprintf(box_track_dump_moof_flow_a_filename, 127, "%s/%u.moof_flow.a", directory_path, mpu_sequence_number_audio);


		FILE* box_track_dump_moof_flow_a_fp = fopen(box_track_dump_moof_flow_a_filename, "w");
		if(box_track_dump_moof_flow_a_fp) {
			fwrite(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block->p_buffer, 		  		lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block->i_pos, 1, box_track_dump_moof_flow_a_fp);

			//remove extra mdat box
			fwrite(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_from_flow->p_buffer, 	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_from_flow->i_pos - 8 , 1, box_track_dump_moof_flow_a_fp);
			fwrite(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_mdat_block->p_buffer, 			lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_mdat_block->i_pos, 1, box_track_dump_moof_flow_a_fp);

			fclose(box_track_dump_moof_flow_a_fp);
			free(box_track_dump_moof_flow_a_filename);
		}
	}

//
//	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_reconstituted) {
//		//dump a track with rebuilt moof
//		char* box_track_dump_moof_rebuilt_a_filename = (char*)calloc(128, sizeof(char));
//		snprintf(box_track_dump_moof_rebuilt_a_filename, 127, "%s/%u.moof_rebuilt.a", directory_path, mpu_sequence_number_audio);
//
//
//		FILE* box_track_dump_moof_rebuilt_a_fp = fopen(box_track_dump_moof_rebuilt_a_filename, "w");
//		if(box_track_dump_moof_rebuilt_a_fp) {
//			fwrite(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block->p_buffer, 		  			lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_block->i_pos, 1, box_track_dump_moof_rebuilt_a_fp);
//
//			fwrite(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_reconstituted->p_buffer, 	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_moof_block_reconstituted->i_pos, 1, box_track_dump_moof_rebuilt_a_fp);
//			fwrite(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_mdat_block->p_buffer, 				lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_mdat_block->i_pos, 1, box_track_dump_moof_rebuilt_a_fp);
//
//			fclose(box_track_dump_moof_rebuilt_a_fp);
//			free(box_track_dump_moof_rebuilt_a_filename);
//		}
//	}

    //dump v track
	if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_moof_block_from_flow) {
		char* box_track_dump_moof_flow_v_filename = (char*)calloc(128, sizeof(char));
		snprintf(box_track_dump_moof_flow_v_filename, 127, "%s/%u.moof_flow.v", directory_path, mpu_sequence_number_video);

		FILE* box_track_dump_moof_flow_v_fp = fopen(box_track_dump_moof_flow_v_filename, "w");
		if(box_track_dump_moof_flow_v_fp) {
			fwrite(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block->p_buffer, 				lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block->i_pos, 1, box_track_dump_moof_flow_v_fp);

			//remove extra mdat box
			fwrite(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_moof_block_from_flow->p_buffer, 	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_moof_block_from_flow->i_pos, 1, box_track_dump_moof_flow_v_fp);

			fwrite(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_mdat_block->p_buffer ,			lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_mdat_block->i_pos, 1, box_track_dump_moof_flow_v_fp);

			fclose(box_track_dump_moof_flow_v_fp);
			free(box_track_dump_moof_flow_v_filename);
		}
	}

//
//    if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_moof_block_reconstituted) {
//		//dump v track
//		char* box_track_dump_moof_rebuilt_v_filename = (char*)calloc(128, sizeof(char));
//		snprintf(box_track_dump_moof_rebuilt_v_filename, 127, "%s/%u.moof_flow.v", directory_path, mpu_sequence_number_video);
//
//		FILE* box_track_dump_moof_rebuilt_v_fp = fopen(box_track_dump_moof_rebuilt_v_filename, "w");
//		if(box_track_dump_moof_rebuilt_v_fp) {
//			fwrite(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block->p_buffer, 		 			 lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_block->i_pos, 1, box_track_dump_moof_rebuilt_v_fp);
//			fwrite(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_moof_block_reconstituted->p_buffer, 	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_moof_block_reconstituted->i_pos, 1, box_track_dump_moof_rebuilt_v_fp);
//			fwrite(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_mdat_block->p_buffer, 				lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_mdat_block->i_pos, 1, box_track_dump_moof_rebuilt_v_fp);
//
//			fclose(box_track_dump_moof_rebuilt_v_fp);
//			free(box_track_dump_moof_rebuilt_v_filename);
//		}
//    }

}


//sem_t
//mutex 	pthread_mutex_t pipe_buffer_reader_mutex_lock;

void lls_sls_monitor_reader_mutex_lock(pthread_mutex_t* pthread_mutex) {
	pthread_mutex_lock(pthread_mutex);
}

void lls_sls_monitor_reader_mutex_unlock(pthread_mutex_t* pthread_mutex) {
	pthread_mutex_unlock(pthread_mutex);
}


pthread_mutex_t* lls_sls_monitor_reader_mutext_create() {

//	char sem_name[31];
//	snprintf((char*)&sem_name, 29, "/atsc3_http_play_%ld", random());

  //  __PLAYER_FFPLAY_INFO("creating semaphore with path: %s", sem_name);
	//pthread_mutex_t* sem = sem_open(sem_name, O_CREAT, 0644, 0);
//	__PLAYER_FFPLAY_INFO("sem_init returned: %p", pipe_ffplay_buffer->pipe_buffer_semaphore);


	pthread_mutex_t* pthread_mutex = calloc(1, sizeof(pthread_mutex_t));
	if (pthread_mutex_init(pthread_mutex, NULL) != 0) {
		__PLAYER_FFPLAY_ERROR("pthread_mutex init failed");
		abort();
	}
    return pthread_mutex;
}
