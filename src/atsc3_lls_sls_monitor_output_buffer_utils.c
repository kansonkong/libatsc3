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


TODO: atsc3_isobmff_tools.cpp:363:WARN : 1551746974.9445: Movie Fragment Metadata is NULL for packet_id: 36

 */

#include "atsc3_lls_sls_monitor_output_buffer_utils.h"


int _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG_ENABLED = 0;
int _LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_TRACE_ENABLED = 0;


int lls_sls_monitor_output_buffer_recover_from_last_video_moof_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_moof_box_pos) {
		lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos = lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_moof_box_pos;
		lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_is_from_last_mpu = true;
		lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_is_from_last_mpu_processed = false;

		//reparse our trun sample table
		block_t* moof_box = block_Alloc(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos);
		block_Write(moof_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos);
		trun_sample_entry_vector_t* trun_sample_entry_vector = parseMoofBoxForTrunSampleEntries(moof_box);

		//clear out our sample size to be rebuilt later
		for(int i=0; i < trun_sample_entry_vector->size; i++) {
			trun_sample_entry_vector->data[i]->sample_size = 0;
			trun_sample_entry_vector->data[i]->has_matching_sample = false;
		}

		lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_trun_sample_entry_vector = trun_sample_entry_vector;
	}
	return 0;
}


int lls_sls_monitor_output_buffer_recover_from_last_audio_moof_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_moof_box_pos) {
		lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_moof_box_pos;
		lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_is_from_last_mpu = true;
		lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_is_from_last_mpu_processed = false;

		//reparse our trun sample table
		block_t* moof_box = block_Alloc(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos);
		block_Write(moof_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos);
		trun_sample_entry_vector_t* trun_sample_entry_vector = parseMoofBoxForTrunSampleEntries(moof_box);

		//clear out our sample size to be rebuilt later
		for(int i=0; i < trun_sample_entry_vector->size; i++) {
			trun_sample_entry_vector->data[i]->sample_size = 0;
			trun_sample_entry_vector->data[i]->has_matching_sample = false;
		}

		lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_trun_sample_entry_vector = trun_sample_entry_vector;
	}
	return 0;
}


void lls_sls_monitor_output_buffer_reset_moof_and_fragment_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_is_from_last_mpu = false;
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_is_from_last_mpu_processed = false;

	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_trun_sample_entry_vector) {
		free(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_trun_sample_entry_vector);
		lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_trun_sample_entry_vector = NULL;
	}
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_moof_box_pos = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos;

	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos = 0;
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos = 0;
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_fragment = NULL;
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_fragment_lost_mfu_count = 0;

	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_is_from_last_mpu = false;
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_is_from_last_mpu_processed = false;

	if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_trun_sample_entry_vector) {
		free(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_trun_sample_entry_vector);
		lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_trun_sample_entry_vector = NULL;
	}
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_moof_box_pos = lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos;
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos = 0;
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos = 0;
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_fragment = NULL;
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_fragment_lost_mfu_count = 0;

    lls_sls_monitor_output_buffer->should_flush_output_buffer = false;
}

void lls_sls_monitor_output_buffer_reset_all_position(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos = 0;
	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.last_mpu_sequence_number_last_fragment = NULL;

	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos = 0;
	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.last_mpu_sequence_number_last_fragment = NULL;

	lls_sls_monitor_output_buffer_reset_moof_and_fragment_position(lls_sls_monitor_output_buffer);
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

int lls_sls_monitor_output_buffer_copy_audio_init_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_header) {

    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box) {
        lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_INIT_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_INIT_BUFFER, audio_isobmff_header);
}


int lls_sls_monitor_output_buffer_copy_audio_moof_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_moof) {

    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box) {
        lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_MOOF_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_MOOF_BUFFER, audio_isobmff_moof);
}


int lls_sls_monitor_output_buffer_copy_audio_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_isobmff_fragment) {
    
    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box) {
        lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, audio_isobmff_fragment);
}


int lls_sls_monitor_output_buffer_copy_video_init_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_header) {

    if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box) {
        lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_INIT_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_INIT_BUFFER, video_isobmff_header);
}


int lls_sls_monitor_output_buffer_copy_video_moof_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_moof) {

    if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box) {
        lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_MOOF_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_MOOF_BUFFER, video_isobmff_moof);
}


int lls_sls_monitor_output_buffer_copy_and_parse_audio_moof_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, mmtp_payload_fragments_union_t* audio_isobmff_moof_fragment) {

    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box) {
        lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_MOOF_BUFFER, sizeof(uint8_t));
    }
    int size = __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_MOOF_BUFFER, audio_isobmff_moof_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
    if(size && (audio_isobmff_moof_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 0x0 || audio_isobmff_moof_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 0x3)) {
    	//parse this as a mdat
    	block_t* moof_box = block_Alloc(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos);
    	block_Write(moof_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos);
    	trun_sample_entry_vector_t* trun_sample_entry_vector = parseMoofBoxForTrunSampleEntries(moof_box);
    	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_trun_sample_entry_vector = trun_sample_entry_vector;
    }
    return size;
}

int lls_sls_monitor_output_buffer_copy_and_parse_video_moof_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, mmtp_payload_fragments_union_t* video_isobmff_moof_fragment) {

    if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box) {
        lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_MOOF_BUFFER, sizeof(uint8_t));
    }
    int size = __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_MOOF_BUFFER, video_isobmff_moof_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
    if(size && (video_isobmff_moof_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 0x0 || video_isobmff_moof_fragment->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator == 0x3)) {
    	//parse this as a mdat
    	block_t* moof_box = block_Alloc(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos);
    	block_Write(moof_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos);
    	trun_sample_entry_vector_t* trun_sample_entry_vector = parseMoofBoxForTrunSampleEntries(moof_box);
    	lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_trun_sample_entry_vector = trun_sample_entry_vector;
    }
    return size;
}


int lls_sls_monitor_output_buffer_copy_video_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* video_isobmff_fragment) {

    if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box) {
        lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, sizeof(uint8_t));
    }
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, video_isobmff_fragment);
}



void __data_unit_recover_null_pad_offset(mmtp_payload_fragments_union_t* data_unit) {
	uint32_t to_null_size =	data_unit->mpu_data_unit_payload_fragments_timed.offset;
	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: null pad: adding %u head to %u, sample: %u, fragment: %u, mpu_sequence_number: %u, packet_sequence_number: %u",
			to_null_size,
			data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos,
			data_unit->mpu_data_unit_payload_fragments_timed.sample_number,
			data_unit->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_counter,
			data_unit->mmtp_mpu_type_packet_header.mpu_sequence_number,
			data_unit->mmtp_mpu_type_packet_header.packet_sequence_number);

	block_t* temp_mpu_data_unit_payload = block_Alloc(to_null_size + data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos);
	temp_mpu_data_unit_payload->i_pos = to_null_size;
	block_Write(temp_mpu_data_unit_payload, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos);
	//hack - keep track of our old du size
	data_unit->mmtp_mpu_type_packet_header.data_unit_length = data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;

	block_Release(&data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
	data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload = temp_mpu_data_unit_payload;
}


void __data_unit_recover_null_pad_tail(mmtp_payload_fragments_union_t* data_unit, uint32_t tail_pad_size) {
	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: tail pad: adding %u tail to %u, sample: %u, fragment: %u, mpu_sequence_number: %u, packet_sequence_number: %u",
			tail_pad_size,
			data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos,
			data_unit->mpu_data_unit_payload_fragments_timed.sample_number,
			data_unit->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_counter,
			data_unit->mmtp_mpu_type_packet_header.mpu_sequence_number,
			data_unit->mmtp_mpu_type_packet_header.packet_sequence_number);

	uint32_t new_tail_pos = tail_pad_size + data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;
	block_t* temp_mpu_data_unit_payload = block_Alloc(new_tail_pos);
	block_Write(temp_mpu_data_unit_payload, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos);
	temp_mpu_data_unit_payload->i_pos = new_tail_pos;

    //hack - keep track of our old du size
	data_unit->mmtp_mpu_type_packet_header.data_unit_length = data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;

	block_Release(&data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
	data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload = temp_mpu_data_unit_payload;
}


void __data_unit_recover_null_pad_offset_range_same_sample_id(mmtp_payload_fragments_union_t* data_unit_from, mmtp_payload_fragments_union_t* data_unit_to) {
	if(data_unit_from->mpu_data_unit_payload_fragments_timed.sample_number != data_unit_to->mpu_data_unit_payload_fragments_timed.sample_number ||
		data_unit_from->mmtp_mpu_type_packet_header.mpu_sequence_number != data_unit_to->mmtp_mpu_type_packet_header.mpu_sequence_number) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: NULL PAD: sample/mpu numbers do not match: %u, %u", data_unit_from->mpu_data_unit_payload_fragments_timed.sample_number, data_unit_to->mpu_data_unit_payload_fragments_timed.sample_number);
		return;
	}


    uint32_t to_null_size;
    //todo - fix me
    /**
     
     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 6 as to_remove
     atsc3_lls_sls_monitor_output_buffer_utils.c:546:WARN :DETECT: cross SAMPLE LAST OPEN: current mpu: 811, last frag counter: 18, last sample: 1, current sample: 8, current offset: 0
     atsc3_lls_sls_monitor_output_buffer_utils.c:319:WARN :RECOVER: tail pad: adding 4844 tail to 1398, sample: 8, fragment: 1, mpu_sequence_number: 811, packet_sequence_number: 226474
     atsc3_lls_sls_monitor_output_buffer_utils.c:546:WARN :DETECT: cross SAMPLE LAST OPEN: current mpu: 811, last frag counter: 1, last sample: 8, current sample: 1, current offset: 4296
     atsc3_lls_sls_monitor_output_buffer_utils.c:299:WARN :RECOVER: null pad: adding 4296 head to 1432, sample: 1, fragment: 19, mpu_sequence_number: 811, packet_sequence_number: 225568
     atsc3_lls_sls_monitor_output_buffer_utils.c:319:WARN :RECOVER: tail pad: adding 313152 tail to 5728, sample: 1, fragment: 19, mpu_sequence_number: 811, packet_sequence_number: 225568
     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 1 as to_remove
     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 2 as to_remove
     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 3 as to_remove
     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 4 as to_remove
     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 5 as to_remove
     atsc3_lls_sls_monitor_output_buffer_utils.c:537:WARN :RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark 6 as to_remove
     atsc3_lls_sls_monitor_output_buffer_utils.c:546:WARN :DETECT: cross SAMPLE LAST OPEN: current mpu: 811, last frag counter: 19, last sample: 1, current sample: 8, current offset: 1432
     atsc3_lls_sls_monitor_output_buffer_utils.c:299:WARN :RECOVER: null pad: adding 1432 head to 257, sample: 8, fragment: 0, mpu_sequence_number: 811, packet_sequence_number: 226475
     atsc3_lls_sls_monitor_output_buffer_utils.c:319:WARN :RECOVER: tail pad: adding 4262 tail to 1432, sample: 38, fragment: 5, mpu_sequence_number: 811, packet_sequence_number: 223417
     atsc3_lls_sls_monitor_output_buffer_utils.c:546:WARN :DETECT: cross SAMPLE LAST OPEN: current mpu: 811, last frag counter: 5, last sample: 38, current sample: 1, current offset: 21480
     atsc3_lls_sls_monitor_output_buffer_utils.c:299:WARN :RECOVER: null pad: adding 21480 head to 1432, sample: 1, fragment: 17, mpu_sequence_number: 811, packet_sequence_number: 226947
     atsc3_lls_sls_monitor_output_buffer_utils.c:491:WARN :DETECT: INTRA sample: 1, current packet_sequence_number: 225569, last packet_sequence_number: 226947, missing: 4294965917
     atsc3_lls_sls_monitor_output_buffer_utils.c:357:WARN :RECOVER: null pad RANGE: from: sample number: 1, offset: 21480, to: sample_number: 1, offset: 5728, adding 4294950112 head to 1432, to packet_sequence_number: 225569, from packet_sequence_number: 226947, mpu_sequence_number: 811
     atsc3_listener_metrics_ncurses(33521,0x700005b2b000) malloc: *** mach_vm_map(size=18446744073709539328) failed (error code=3)
     *** error: can't allocate region
     **/
    if(data_unit_from->mmtp_mpu_type_packet_header.data_unit_length) {
        //check for - wraparound
        int32_t to_null_size_i = data_unit_to->mpu_data_unit_payload_fragments_timed.offset - (data_unit_from->mpu_data_unit_payload_fragments_timed.offset + data_unit_from->mpu_data_unit_payload_fragments_timed.data_unit_length);
        if(to_null_size_i < 0) {
            to_null_size = data_unit_to->mpu_data_unit_payload_fragments_timed.offset;
        } else {
            to_null_size = to_null_size_i;
        }
    }  else {
        to_null_size = data_unit_to->mpu_data_unit_payload_fragments_timed.offset - (data_unit_from->mpu_data_unit_payload_fragments_timed.offset + data_unit_from->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_pos);
    }
    __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: null pad RANGE: from: sample number: %u, offset: %u, to: sample_number: %u, offset: %u, adding %u head to %u, to packet_sequence_number: %u, from packet_sequence_number: %u, mpu_sequence_number: %u",
            data_unit_from->mpu_data_unit_payload_fragments_timed.sample_number,
            data_unit_from->mpu_data_unit_payload_fragments_timed.offset,
            data_unit_to->mpu_data_unit_payload_fragments_timed.sample_number,
            data_unit_to->mpu_data_unit_payload_fragments_timed.offset,
           to_null_size,
           data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos,
			data_unit_to->mmtp_mpu_type_packet_header.packet_sequence_number,
			data_unit_from->mmtp_mpu_type_packet_header.packet_sequence_number,
			data_unit_to->mmtp_mpu_type_packet_header.mpu_sequence_number);

    
	block_t* temp_mpu_data_unit_payload = block_Alloc(to_null_size + data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos);
	temp_mpu_data_unit_payload->i_pos = to_null_size;
	block_Write(temp_mpu_data_unit_payload, data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos);
    
    //hack - keep track of our old du size
    data_unit_to->mmtp_mpu_type_packet_header.data_unit_length = data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;
    
	block_Release(&data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
	data_unit_to->mmtp_mpu_type_packet_header.mpu_data_unit_payload = temp_mpu_data_unit_payload;
}

//mmtp_payload_fragments_union_t* video_last_data_unit = NULL;
//mmtp_payload_fragments_union_t* audio_last_data_unit = NULL;


int lls_sls_monitor_output_buffer_copy_and_recover_audio_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, mmtp_payload_fragments_union_t* audio_data_unit) {

	lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff = &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff;
	block_t* audio_mpu_data_unit_payload = audio_data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload;

	trun_sample_entry_vector_t* moof_box_trun_sample_entry_vector = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_trun_sample_entry_vector;

	//we need to rebuild our trun here
	if(lls_sls_monitor_buffer_isobmff->moof_box_is_from_last_mpu) {
		uint32_t trun_sample_index = audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number-1;
		if(trun_sample_index >= moof_box_trun_sample_entry_vector->size) {
			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("trun sample index %u is greater than size: %u", trun_sample_index, moof_box_trun_sample_entry_vector->size);
			//TODO - append this in our trun box
			return -1;
		}

		moof_box_trun_sample_entry_vector->data[trun_sample_index]->sample_size += audio_data_unit->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_pos;
    	moof_box_trun_sample_entry_vector->data[trun_sample_index]->has_matching_sample = true;

		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("RECOVERY: REBUILD TRUN: A: sample index %u is now sample_size: %u", trun_sample_index, moof_box_trun_sample_entry_vector->data[trun_sample_index]->sample_size);

		goto copy_packet;
	}

	//assume audio will lose samples rather than fragments

    if(lls_sls_monitor_buffer_isobmff->last_fragment && audio_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number == lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number &&
        audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number-1 != lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number) {

        uint32_t from_mfu_sample_number = lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number + 1;

        uint32_t to_mfu_sample_number = audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number - 1;

        for(int i = from_mfu_sample_number; i < to_mfu_sample_number && i < moof_box_trun_sample_entry_vector->size; i++) {
            trun_sample_entry_t* missing_mfu_sample_entry = moof_box_trun_sample_entry_vector->data[i-1];
            __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: TRUNCATE SAMPLE: A: updating moof_box_trun_sample_entry vector to mark %u as to_remove", i);
            missing_mfu_sample_entry->to_remove_sample_entry = true;
        }
    } else if(lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment &&
            audio_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number != lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number &&
              ((moof_box_trun_sample_entry_vector && moof_box_trun_sample_entry_vector->size > lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.sample_number) ||
            audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number != 1)) {
        //compute mpu fragment variances

//        uint32_t missing_packets = audio_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number - lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number - 1;
        __LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: cross MPU sample mpu: A: current mpu: %u, last mpu: %u, current sample: %u, last sample: %u, current packet_sequence_number: %u, last packet_sequence_number: %u",
                audio_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
                lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
                audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number,
                lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.sample_number,
                audio_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number,
                lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number);
    }

copy_packet:
    if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box) {
        lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, sizeof(uint8_t));
    }

    if(audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number <= moof_box_trun_sample_entry_vector->size) {
    	moof_box_trun_sample_entry_vector->data[audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number-1]->has_matching_sample = true;
       	moof_box_trun_sample_entry_vector->data[audio_data_unit->mpu_data_unit_payload_fragments_timed.sample_number-1]->sample_size += audio_data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;

    }

    lls_sls_monitor_buffer_isobmff->last_fragment = audio_data_unit;
    lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment = audio_data_unit;
    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, audio_data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
}


int lls_sls_monitor_output_buffer_copy_and_recover_video_fragment_block(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, mmtp_payload_fragments_union_t* video_data_unit) {

	lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff = &lls_sls_monitor_output_buffer->video_output_buffer_isobmff;
	block_t* video_mpu_data_unit_payload = video_data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload;

	trun_sample_entry_vector_t* moof_box_trun_sample_entry_vector = lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_trun_sample_entry_vector;

	//we need to rebuild our trun here
	if(lls_sls_monitor_buffer_isobmff->moof_box_is_from_last_mpu) {
		uint32_t trun_sample_index = video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number-1;
		if(trun_sample_index >= moof_box_trun_sample_entry_vector->size) {
			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("V: trun sample index %u is greater than size: %u", trun_sample_index, moof_box_trun_sample_entry_vector->size);
			//TODO - append this in our trun box
			return -1;
		}
		moof_box_trun_sample_entry_vector->data[trun_sample_index]->sample_size += video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_pos;
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("RECOVERY: REBUILD TRUN: V: sample index %u is now sample_size: %u", trun_sample_index, moof_box_trun_sample_entry_vector->data[trun_sample_index]->sample_size);

		goto copy_packet;
	}

	if(!lls_sls_monitor_buffer_isobmff->last_fragment) {

		//we should start with sample == 1, offset == 0, otherwise pad this out
		//intra mfu packet loss
		if(video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number == 1 && video_data_unit->mpu_data_unit_payload_fragments_timed.offset != 0) {
			uint32_t lost_mfu_packets_count = 1;

			if(lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment) {
				lost_mfu_packets_count = (video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number -  lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number);
			}

			lls_sls_monitor_buffer_isobmff->last_fragment_lost_mfu_count += lost_mfu_packets_count;

			__data_unit_recover_null_pad_offset(video_data_unit);
			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: first MFU: V: lost %u mfu packets, sample_number: 0, offset: %u, building null payload for mdat, new fragment size: %u", lost_mfu_packets_count, video_data_unit->mpu_data_unit_payload_fragments_timed.offset, video_mpu_data_unit_payload->i_pos);
		} else if(video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number >1 ) {
			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: first SAMPLE: V: starting sample: %u, offset: %u",  video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number, video_data_unit->mpu_data_unit_payload_fragments_timed.offset);
			//TODO - null this out of our mdat box and adjust offset accordingly
		}
	} else {
		//compute intra sample variances
		if(video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number == lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number &&
				video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number == lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number &&
				video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number - 1 != lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number) {
			uint32_t missing_packets = video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number - lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number - 1;
			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: INTRA sample: %u, current packet_sequence_number: %u, last packet_sequence_number: %u, missing: %u", video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number , video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number, lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number, missing_packets);

			__data_unit_recover_null_pad_offset_range_same_sample_id(lls_sls_monitor_buffer_isobmff->last_fragment, video_data_unit);

		} else if(video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number == lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number &&
				video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number != lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number &&
				lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_counter) {

			//compute intra fragment variances, remember we should have an MFU inbetween..

			//we can't null pad this out without interriogating the mdat

			/**
			 *
			 * atsc3_lls_sls_monitor_output_buffer_utils.c:327:WARN :DETECT: cross SAMPLE LAST OPEN: current mpu: 6138, last frag counter: 1, last sample: 10, current sample: 11, current offset: 2864
			 * .sample_number are 1 based while the trun_sample entry is 0 based
			 *
			 * atsc3_lls_sls_monitor_output_buffer_utils.c:222:WARN :RECOVER: null pad: adding 2864 head to 653, mpu_sequence_number: 6138, packet_sequence_number: 2917408
			 *
			 * null inject, should remove sample from moof/trun
			 *
			 */

			uint32_t from_mfu_sample_number = lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number - 1;
			uint32_t from_mfu_trun_sample_size = 0;
			uint32_t from_mfu_to_tail_pad_size = 0;
			trun_sample_entry_t* from_missing_mfu_sample_entry = NULL;

			if(from_mfu_sample_number <= moof_box_trun_sample_entry_vector->size) {
				from_missing_mfu_sample_entry = moof_box_trun_sample_entry_vector->data[from_mfu_sample_number];
				from_mfu_trun_sample_size = from_missing_mfu_sample_entry->sample_size;

				//compute our last fragment size against the trun
				if(lls_sls_monitor_buffer_isobmff->last_fragment->mmtp_mpu_type_packet_header.data_unit_length) {
					from_mfu_to_tail_pad_size = from_mfu_trun_sample_size - lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.offset - lls_sls_monitor_buffer_isobmff->last_fragment->mmtp_mpu_type_packet_header.data_unit_length;
				} else {
					from_mfu_to_tail_pad_size = from_mfu_trun_sample_size - lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.offset - lls_sls_monitor_buffer_isobmff->last_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;
				}

				__data_unit_recover_null_pad_tail(lls_sls_monitor_buffer_isobmff->last_fragment, from_mfu_to_tail_pad_size);
			}

            uint32_t to_mfu_sample_number = video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number - 1;

			for(int i = from_mfu_sample_number + 1; i < to_mfu_sample_number && i < moof_box_trun_sample_entry_vector->size; i++) {
				trun_sample_entry_t* missing_mfu_sample_entry = moof_box_trun_sample_entry_vector->data[i];
				__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("RECOVER: REMOVE SAMPLE: V: updating moof_box_trun_sample_entry vector to mark %u as to_remove", i);
				missing_mfu_sample_entry->to_remove_sample_entry = true;
			}

			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: cross SAMPLE LAST OPEN: current mpu: %u, last frag counter: %u, last sample: %u, current sample: %u, current offset: %u",
					video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
					lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_counter,
					lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number,
					video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number,
					video_data_unit->mpu_data_unit_payload_fragments_timed.offset);

			//we can null pad the offset size
			uint32_t missing_current_bytes = video_data_unit->mpu_data_unit_payload_fragments_timed.offset;

			if(missing_current_bytes) {
				__data_unit_recover_null_pad_offset(video_data_unit);
			}

		} else if(lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment &&
				video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number != lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number &&
				video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number - 1 != lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number) {
			//compute mpu fragment variances

			uint32_t missing_packets = video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number - lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number - 1;
			__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_WARN("DETECT: cross MPU sample mpu: V: current mpu: %u, last mpu: %u, current sample: %u, last sample: %u, current packet_sequence_number: %u, last packet_sequence_number: %u, missing: %u",
					video_data_unit->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
					lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,
					video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number,
					lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.sample_number,
					video_data_unit->mpu_data_unit_payload_fragments_timed.packet_sequence_number,
					lls_sls_monitor_buffer_isobmff->last_fragment->mpu_data_unit_payload_fragments_timed.packet_sequence_number,
					missing_packets);
		}
	}

copy_packet:
    if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box) {
        lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box = (uint8_t*)calloc(_LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, sizeof(uint8_t));
    }

    if(video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number <= moof_box_trun_sample_entry_vector->size) {
       	moof_box_trun_sample_entry_vector->data[video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number-1]->has_matching_sample = true;
       	moof_box_trun_sample_entry_vector->data[video_data_unit->mpu_data_unit_payload_fragments_timed.sample_number-1]->sample_size += video_data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_pos;
    }

    lls_sls_monitor_buffer_isobmff->last_fragment = video_data_unit;
    lls_sls_monitor_buffer_isobmff->last_mpu_sequence_number_last_fragment = video_data_unit;

    return __lls_sls_monitor_output_buffer_check_and_copy(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos, _LLS_SLS_MONITOR_OUTPUT_MAX_FRAGMENT_BUFFER, video_mpu_data_unit_payload);
}


int lls_sls_monitor_buffer_isobmff_moof_patch_mdat_box(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff_to_patch) {
	uint8_t mdat_box[8];
	uint32_t mdat_box_old_size;
	uint32_t total_mdat_body_size = lls_sls_monitor_buffer_isobmff_to_patch->fragment_pos + 8;

	memcpy(&mdat_box, &lls_sls_monitor_buffer_isobmff_to_patch->moof_box[lls_sls_monitor_buffer_isobmff_to_patch->moof_box_pos-8], 8);
	mdat_box_old_size = ntohl((uint32_t)mdat_box);

	if(mdat_box[4] == 'm' && mdat_box[5] == 'd' && mdat_box[6] == 'a' && mdat_box[7] == 't') {
		mdat_box[0] = (total_mdat_body_size >> 24) & 0xFF;
		mdat_box[1] = (total_mdat_body_size >> 16) & 0xFF;
		mdat_box[2] = (total_mdat_body_size >> 8) & 0xFF;
		mdat_box[3] = (total_mdat_body_size) & 0xFF;


		memcpy(&lls_sls_monitor_buffer_isobmff_to_patch->moof_box[lls_sls_monitor_buffer_isobmff_to_patch->moof_box_pos-8], &mdat_box, 4);
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("patch mdat box:  metadata packet: %p, size was: %u, now: %u, last 8 bytes of metadata fragment updated to: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
				lls_sls_monitor_buffer_isobmff_to_patch, mdat_box_old_size, total_mdat_body_size,
				mdat_box[0], mdat_box[1], mdat_box[2], mdat_box[3], mdat_box[4], mdat_box[5], mdat_box[6], mdat_box[7]);

		return total_mdat_body_size;

	} else {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("patch modat box: metadata packet: %p, cant find trailing mdat!", lls_sls_monitor_buffer_isobmff_to_patch->moof_box);
		return 0;
	}

}
block_t* lls_sls_monitor_output_buffer_copy_audio_full_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos || !lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy audio full box: error: init_box_pos: %u, fragment_pos: %u, returning NULL", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);
		return NULL;
	}

	uint32_t full_box_size = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos + lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos + lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos;
	if(full_box_size <= 0) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy audio full box size <= 0, %u, returning NULL", full_box_size);
		return NULL;
	}
	block_t* isobmff_full_block = block_Alloc(full_box_size);

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("copy audio full box: total size: %u, init size: %u, moof size: %u, fragment size: %u", full_box_size, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);
	block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos);

	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos) {
		block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos);

	}

	block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);

	return isobmff_full_block;
}

block_t* lls_sls_monitor_output_buffer_copy_audio_moof_fragment_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	if(!lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy audio moof_fragment: error: fragment_pos: %u, returning NULL", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);
		return NULL;
	}

	uint32_t moof_fragment_size = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos + lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos;
	if(moof_fragment_size <= 0) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy audio moof_fragment size <= 0, %u, returning NULL", moof_fragment_size);
		return NULL;
	}
	block_t* isobmff_moof_fragment_block = block_Alloc(moof_fragment_size);

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("copy audio moof_fragment: total size: %u,  moof size: %u, fragment size: %u", moof_fragment_size,  lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);

	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos) {
		block_Write(isobmff_moof_fragment_block, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos);
	}

	block_Write(isobmff_moof_fragment_block, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos);

	return isobmff_moof_fragment_block;
}


block_t* lls_sls_monitor_output_buffer_copy_video_full_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos || !lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy video full box: error: init_box_pos: %u, fragment_pos: %u, returning NULL", lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);
		return NULL;
	}

	uint32_t full_box_size = lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos + lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos + lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos;
	if(full_box_size <= 0) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy video full box size <= 0, %u, returning NULL", full_box_size);
		return NULL;
	}
	block_t* isobmff_full_block = block_Alloc(full_box_size);

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("copy video full box: total size: %u, init size: %u, moof size: %u, fragment size: %u", full_box_size, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);
	block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos);

	if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos) {
		block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos);

	}

	block_Write(isobmff_full_block, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);

	return isobmff_full_block;
}

block_t* lls_sls_monitor_output_buffer_copy_video_moof_fragment_isobmff_box(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer) {
	if(!lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy video moof_fragment: error: fragment_pos: %u, returning NULL", lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);
		return NULL;
	}

	uint32_t moof_fragment_size = lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos + lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos;
	if(moof_fragment_size <= 0) {
		__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_ERROR("copy video moof_fragment size <= 0, %u, returning NULL", moof_fragment_size);
		return NULL;
	}
	block_t* isobmff_moof_fragment_block = block_Alloc(moof_fragment_size);

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_DEBUG("copy video moof_fragment: total size: %u,  moof size: %u, fragment size: %u", moof_fragment_size,  lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);

	if(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos) {
		block_Write(isobmff_moof_fragment_block, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos);
	}

	block_Write(isobmff_moof_fragment_block, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos);

	return isobmff_moof_fragment_block;
}


void lls_slt_monitor_check_and_handle_pipe_ffplay_buffer_is_shutdown(lls_slt_monitor_t* lls_slt_monitor) {
	if(lls_slt_monitor->lls_sls_alc_monitor) {
		if(lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer) {
			bool is_shutdown_alc = pipe_buffer_reader_check_if_shutdown(&lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer);
			if(is_shutdown_alc) {
				__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("lls_slt_monitor: ffplay is shutdown for ALC service_id: %u, setting ffplay_output_enabled = false", lls_slt_monitor->lls_sls_alc_monitor->service_id);
				lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled = false;
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

void lls_sls_monitor_output_buffer_file_dump(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, const char* directory_path, uint32_t mpu_sequence_number_audio, uint32_t mpu_sequence_number_video) {

	__LLS_SLS_MONITOR_OUTPUT_BUFFER_UTILS_INFO("lls_sls_monitor_output_buffer_file_dump: dumping to %s, audio sequence number: %u, video sequence number: %u", directory_path, mpu_sequence_number_audio, mpu_sequence_number_video);
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
    	fwrite(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box, 	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.init_box_pos, 1, box_track_dump_a_fp);
    	fwrite(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box, 	lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.moof_box_pos, 1, box_track_dump_a_fp);
    	fwrite(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_box, lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.fragment_pos, 1, box_track_dump_a_fp);
    	fclose(box_track_dump_a_fp);
    	free(box_track_dump_a_filename);
    }

    //dump v track
    char* box_track_dump_v_filename = (char*)calloc(128, sizeof(char));
    snprintf(box_track_dump_v_filename, 127, "%s/%u.v", directory_path, mpu_sequence_number_video);

    FILE* box_track_dump_v_fp = fopen(box_track_dump_v_filename, "w");
    if(box_track_dump_v_fp) {
    	fwrite(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.init_box_pos, 1, box_track_dump_v_fp);
    	fwrite(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.moof_box_pos, 1, box_track_dump_v_fp);
    	fwrite(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_box, lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fragment_pos, 1, box_track_dump_v_fp);

    	fclose(box_track_dump_v_fp);
    	free(box_track_dump_v_filename);
    }
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
