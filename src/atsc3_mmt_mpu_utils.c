/*
 * atsc3_mmt_mpu_utils.c
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */

#include "atsc3_mmt_mpu_utils.h"

int _MMT_MPU_DEBUG_ENABLED = 0;

void mpu_dump_header(mmtp_payload_fragments_union_t* mmtp_payload) {

	__MMT_MPU_DEBUG("------------------");

	if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_timed_flag) {
		__MMT_MPU_DEBUG("MFU Packet (Timed)");
		__MMT_MPU_DEBUG("-----------------");
		__MMT_MPU_DEBUG(" mpu_fragmentation_indicator: %d", mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_fragment_type);
		__MMT_MPU_DEBUG(" movie_fragment_seq_num: %u", mmtp_payload->mpu_data_unit_payload_fragments_timed.movie_fragment_sequence_number);
		__MMT_MPU_DEBUG(" sample_num: %u", mmtp_payload->mpu_data_unit_payload_fragments_timed.sample_number);
		__MMT_MPU_DEBUG(" offset: %u", mmtp_payload->mpu_data_unit_payload_fragments_timed.offset);
		__MMT_MPU_DEBUG(" pri: %d", mmtp_payload->mpu_data_unit_payload_fragments_timed.priority);
		__MMT_MPU_DEBUG(" mpu_sequence_number: %u",mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number);

	} else {
		__MMT_MPU_DEBUG("MFU Packet (Non-timed)");
		__MMT_MPU_DEBUG("---------------------");
		__MMT_MPU_DEBUG(" mpu_fragmentation_indicator: %d", mmtp_payload->mpu_data_unit_payload_fragments_nontimed.mpu_fragment_type);
		__MMT_MPU_DEBUG(" non_timed_mfu_item_id: %u", mmtp_payload->mpu_data_unit_payload_fragments_nontimed.non_timed_mfu_item_id);

	}

	__MMT_MPU_DEBUG("-----------------");
}

void mpu_dump_flow(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload) {
	//sub_flow_vector is a global
	mpu_dump_header(mmtp_payload);

	__MMT_MPU_DEBUG("::dumpMfu ******* file dump file: %d.%d.%d.%d:%d-p:%d.s:%d.ft:%d",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,

			mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type);

	char *myFilePathName = calloc(64, sizeof(char*));
	snprintf(myFilePathName, 64, "mpu/%d.%d.%d.%d,%d-p.%d.s,%d.ft,%d",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,

			mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type);


	__MMT_MPU_DEBUG("::dumpMfu ******* file dump file: %s", myFilePathName);

	FILE *f = fopen(myFilePathName, "a");
	if(!f) {
		__MMT_MPU_ERROR("::dumpMpu ******* UNABLE TO OPEN FILE %s", myFilePathName);
			return;
	}

	int blocks_written = fwrite(mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer, 1, f);
	if(blocks_written != 1) {
		__MMT_MPU_WARN("Incomplete block written for %s", myFilePathName);
	}


	fclose(f);
}

//assumes in-order delivery
void mpu_dump_reconstitued(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload) {
	//sub_flow_vector is a global
	mpu_dump_header(mmtp_payload);

	__MMT_MPU_DEBUG("::dump_mpu_reconstitued ******* file dump file: %d.%d.%d.%d:%d-p:%d.s:%d.ft:%d",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,

			mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type);

	char *myFilePathName = calloc(64, sizeof(char*));
	snprintf(myFilePathName, 64, "mpu/%d.%d.%d.%d,%d-p.%d.s,%d.ft",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number);


	__MMT_MPU_DEBUG("::dumpMfu ******* file dump file: %s", myFilePathName);

		FILE *f = fopen(myFilePathName, "a");
	if(!f) {
		__MMT_MPU_ERROR("::dumpMpu ******* UNABLE TO OPEN FILE %s", myFilePathName);
			return;
	}

	int blocks_written = fwrite(mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer, 1, f);
	if(blocks_written != 1) {
		__MMT_MPU_WARN("Incomplete block written for %s", myFilePathName);
	}

	fclose(f);
}

/*
 *
 * rules:
 *
 * mpu_sequnece number change ->
 * packet_seqeunce number gap
 *
 * for each sample_number increment from 1...60 map
 * 	when fragmentation_indication==1, fragmentation_counter = N fragments to process (e.g. 70, 69....)
 *
 *
 */

void mpu_play_object(pipe_ffplay_buffer_t* pipe_ffplay_buffer, mmtp_payload_fragments_union_t* mmtp_payload) {

	bool should_signal = false;
	if(mmtp_payload->mmtp_mpu_type_packet_header.mmtp_payload_type != 0x0) {
		__MMT_MPU_WARN("Incorrect payload type: 0x%x", mmtp_payload->mmtp_mpu_type_packet_header.mmtp_payload_type);
		goto cleanup;

	}

	if(!pipe_ffplay_buffer->has_written_init_box) {
		if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type != 0x0) {
			goto cleanup;
		}

		pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);
		pipe_buffer_push_block(pipe_ffplay_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer);
		pipe_ffplay_buffer->last_mpu_sequence_number = mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number;
		pipe_ffplay_buffer->has_written_init_box = true;
		pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);

		__MMT_MPU_DEBUG("pushing init fragment for %d fragment_type: 0", mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number);

		goto cleanup;

	} else {
		//ignore our init box after we've sent it the first time
		if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type == 0x0) {
			goto cleanup;
		}

		pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);
		pipe_buffer_push_block(pipe_ffplay_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer);

		if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number != pipe_ffplay_buffer->last_mpu_sequence_number) {
			//signal here, we will have the first fragment of the next slice in the payload, but its simpler for now...
			__MMT_MPU_DEBUG("triggering signal because mpu_sequence changed from %u to %u",  pipe_ffplay_buffer->last_mpu_sequence_number, mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number );
			pipe_buffer_condition_signal(pipe_ffplay_buffer);
		}
		pipe_ffplay_buffer->last_mpu_sequence_number = mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number;

		pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);

	}


cleanup:
	mmtp_payload_fragments_union_free(&mmtp_payload);

}
