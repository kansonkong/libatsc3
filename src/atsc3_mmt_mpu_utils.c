/*
 * atsc3_mmt_mpu_utils.c
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */

#include "atsc3_mmt_mpu_utils.h"

int _MMT_MPU_DEBUG_ENABLED = 0;

void dump_mpu(mmtp_payload_fragments_union_t* mmtp_payload) {

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
	dump_mpu(mmtp_payload);

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


	for(int i=0; i <  mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer; i++) {
		fputc(mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer[i], f);
	}
	fclose(f);
}

//assumes in-order delivery
void mpu_dump_reconstitued(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload) {
	//sub_flow_vector is a global
	dump_mpu(mmtp_payload);

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


	for(int i=0; i <  mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer; i++) {
		fputc(mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer[i], f);
	}
	fclose(f);
}
