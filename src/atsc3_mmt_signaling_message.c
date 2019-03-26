/*
 * atsc3_mmt_signaling_message.h
 *
 *  Created on: Jan 21, 2019
 *      Author: jjustman
 */

#include "atsc3_mmtp_types.h"
#include "atsc3_mmtp_parser.h"
#include "atsc3_mmt_signaling_message.h"


int _MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED = 0;
int _MMT_SIGNALLING_MESSAGE_TRACE_ENABLED = 0;


mmt_signalling_message_vector_t* mmt_signalling_message_vector_create() {
	mmt_signalling_message_vector_t* mmt_signalling_message_vector = calloc(1, sizeof(mmt_signalling_message_vector_t*));
	assert(mmt_signalling_message_vector);

	return mmt_signalling_message_vector;
}

mmt_signalling_message_vector_t* mmt_signalling_message_vector_add(mmt_signalling_message_vector_t* mmt_signalling_message_vector, mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {
	mmt_signalling_message_vector->messages = realloc(mmt_signalling_message_vector->messages, sizeof(mmt_signalling_message_header_and_payload_t*));
	assert(mmt_signalling_message_vector->messages);

	mmt_signalling_message_vector->messages[mmt_signalling_message_vector->messages_n++] = mmt_signalling_message_header_and_payload;

	return mmt_signalling_message_vector;
}


mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload_create(uint16_t message_id, uint8_t version) {
	mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = calloc(1, sizeof(mmt_signalling_message_header_and_payload_t));
	mmt_signalling_message_header_and_payload->message_header.message_id = message_id;
	mmt_signalling_message_header_and_payload->message_header.version = version;

	return mmt_signalling_message_header_and_payload;
}

//todo, we will probably need to iterate over each one of these entries
void mmt_signalling_message_vector_free(mmt_signalling_message_vector_t** mmt_signalling_message_vector_p) {
	mmt_signalling_message_vector_t* mmt_signalling_message_vector = *mmt_signalling_message_vector_p;
	if(mmt_signalling_message_vector) {
		for(int i=0; i < mmt_signalling_message_vector->messages_n; mmt_signalling_message_vector++) {
			mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmt_signalling_message_vector->messages[i];
			mmt_signalling_message_header_and_payload_free(&mmt_signalling_message_header_and_payload);
		}

		free(mmt_signalling_message_vector);
		mmt_signalling_message_vector = NULL;
		*mmt_signalling_message_vector_p = NULL;
	}
}

void mmt_signalling_message_header_and_payload_free(mmt_signalling_message_header_and_payload_t** mmt_signalling_message_header_and_payload_p) {
	mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = *mmt_signalling_message_header_and_payload_p;
	if(mmt_signalling_message_header_and_payload) {

		//determine if we have any internal mallocs to clear
		if(mmt_signalling_message_header_and_payload->message_header.message_id == MMT_ATSC3_MESSAGE_ID) {
			if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload) {
				free(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload);
				mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload = NULL;
			}
			if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content) {
				free(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content);
				mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content = NULL;
			}
		}

finally:
	free(mmt_signalling_message_header_and_payload);
	*mmt_signalling_message_header_and_payload_p = NULL;

	}
}


/**
 *
 * MPU_timestamp_descriptor message example
 *
0000   62 02 00 23 af b9 00 00 00 2b 4f 2f 00 35 10 58   b..#¯¹...+O/.5.X
0010   a4 00 00 00 00 12 ce 00 3f 12 ce 00 3b 04 01 00   ¤.....Î.?.Î.;...
0020   00 00 00 00 00 00 00 10 11 11 11 11 11 11 11 11   ................
0030   11 11 11 11 11 11 11 11 68 65 76 31 fd 00 ff 00   ........hev1ý.ÿ.
0040   01 5f 90 01 00 00 23 00 0f 00 01 0c 00 00 16 ce   ._....#........Î
0050   df c2 af b8 d6 45 9f ff                           ßÂ¯¸ÖE.ÿ

raw base64 payload:

62020023afb90000002b4f2f00351058a40000000012ce003f12ce003b04010000000000000000101111111111111111111111111111111168657631fd00ff00015f9001000023000f00010c000016cedfc2afb8d6459fff
 *
 */


uint8_t* mmt_signaling_message_parse_packet_header(mmtp_payload_fragments_union_t* mmtp_packet, uint8_t* udp_raw_buf, uint32_t udp_raw_buf_size) {

	if(mmtp_packet->mmtp_packet_header.mmtp_payload_type != 0x02) {
		_MMSM_ERROR("signaling_message_parse_payload_header: mmtp_payload_type 0x02 != 0x%x", mmtp_packet->mmtp_packet_header.mmtp_payload_type);
		return NULL;
	}

	uint8_t* raw_buf = udp_raw_buf;
	uint8_t* buf = udp_raw_buf;

	//parse the mmtp payload header for signaling message mode
	uint8_t	mmtp_payload_header[2];
	buf = extract(buf, mmtp_payload_header, 2);

	/* TODO:
	 * f_i: bits 0-1 fragmentation indicator:
	 * 0x00 = payload contains one or more complete signaling messages
	 * 0x01 = payload contains the first fragment of a signaling message
	 * 0x10 = payload contains a fragment of a signaling message that is neither first/last
	 * 0x11 = payload contains the last fragment of a signaling message
	 */

	mmtp_packet->mmtp_signalling_message_fragments.si_fragmentation_indiciator = (mmtp_payload_header[0] >> 6) & 0x03;
	//next 4 bits are 0x0000 reserved
	if((mmtp_payload_header[0] >> 2) & 0xF) {
		_MMTP_ERROR("signaling message mmtp header bits 2-5 are not reserved 0");
	}

	//bit 6 is additional Header
	mmtp_packet->mmtp_signalling_message_fragments.si_additional_length_header = ((mmtp_payload_header[0] >> 1) & 0x1);

	//bit 7 is Aggregation
	mmtp_packet->mmtp_signalling_message_fragments.si_aggregation_flag = (mmtp_payload_header[0] & 0x1);

	//count of for how many fragments follow this message, e.g si_fragmentation_indiciator != 0
	//note, packets are not allowed to be both aggregated and fragmented

	mmtp_packet->mmtp_signalling_message_fragments.si_fragmentation_counter = mmtp_payload_header[1];

	return buf;
}

/**
 *
 */
uint8_t* mmt_signaling_message_parse_packet(mmtp_payload_fragments_union_t *mmtp_signalling_packet, uint8_t* udp_raw_buf, uint32_t udp_raw_buf_size) {

	if(mmtp_signalling_packet->mmtp_packet_header.mmtp_payload_type != 0x02) {
		_MMSM_ERROR("signaling_message_parse_payload_header: mmtp_payload_type 0x02 != 0x%x", mmtp_signalling_packet->mmtp_packet_header.mmtp_payload_type);
		return NULL;
	}

	uint8_t *raw_buf = udp_raw_buf;
	uint8_t *buf = udp_raw_buf;
	uint32_t buf_size = udp_raw_buf_size;

	if(mmtp_signalling_packet->mmtp_signalling_message_fragments.si_aggregation_flag) {
		uint32_t mmtp_aggregation_msg_length;
		_MMSM_ERROR("AGGREGATED SI NOT SUPPORTED");

		return NULL;
		while(buf_size) {
			if(mmtp_signalling_packet->mmtp_signalling_message_fragments.si_additional_length_header) {
				//read the full 32 bits for MSG_length
				buf = extract(buf, (uint8_t*)&mmtp_aggregation_msg_length, 4);
				mmtp_aggregation_msg_length = ntohl(mmtp_aggregation_msg_length);

			} else {
				//only read 16 bits for MSG_length
				uint16_t aggregation_msg_length_short;
				buf = extract(buf, (uint8_t*)&aggregation_msg_length_short, 2);
				mmtp_aggregation_msg_length = ntohs(aggregation_msg_length_short);
			}

			//build a msg from buf to buf+mmtp_aggregation_msg_length
			_MMSM_ERROR("AGGREGATED SI NOT SUPPORTED");
			buf = mmt_signaling_message_parse_id_type(mmtp_signalling_packet, buf, buf_size);
			buf_size = udp_raw_buf_size - (buf - udp_raw_buf);
		}
	} else {

		//parse a single message
		buf = mmt_signaling_message_parse_id_type(mmtp_signalling_packet, buf, buf_size);
	}
	return buf;
}

uint8_t* __mmt_signaling_message_parse_length_long(uint8_t* buf, mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {
	uint32_t mmtp_msg_length_long;
	buf = extract(buf, (uint8_t*)&mmtp_msg_length_long, 4);
	mmt_signalling_message_header_and_payload->message_header.length = ntohl(mmtp_msg_length_long);
	return buf;
}

uint8_t* __mmt_signaling_message_parse_length_short(uint8_t* buf, mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {
	uint16_t mmtp_msg_length_short;
	buf = extract(buf, (uint8_t*)&mmtp_msg_length_short, 2);
	mmt_signalling_message_header_and_payload->message_header.length = ntohs(mmtp_msg_length_short);
	return buf;
}

uint8_t* mmt_signaling_message_parse_id_type(mmtp_payload_fragments_union_t *mmtp_signalling_packet, uint8_t* udp_raw_buf, uint32_t buf_size) {
	uint8_t *raw_buf = udp_raw_buf;
	uint8_t *buf = udp_raw_buf;

	//create general signaling message format
	uint16_t  message_id;
	buf = extract(buf, (uint8_t*)&message_id, 2);
	message_id = ntohs(message_id);

	uint8_t version;
	buf = extract(buf, &version, 1);

	mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmt_signalling_message_header_and_payload_create(message_id, version);
	mmt_signalling_message_vector_add(&mmtp_signalling_packet->mmtp_signalling_message_fragments.mmt_signalling_message_vector, mmt_signalling_message_header_and_payload);
	mmt_signalling_message_header_t* mmt_signalling_message_header = &mmt_signalling_message_header_and_payload->message_header;
	mmt_signalling_message_payload_u* mmt_signalling_message_payload = &mmt_signalling_message_header_and_payload->message_payload;

	/** each message parser is required to call either
	 * PA: 				__mmt_signaling_message_parse_length_long
	 * MPI: 			__mmt_signaling_message_parse_length_long
	 * mmt_atsc3_msg: 	__mmt_signaling_message_parse_length_long
	 *
	 * -all others-:
	 *	__mmt_signaling_message_parse_length_short
	 *
	 * length – this field indicates the length of the signalling message.
	 * This field for all signalling messages except PA messages and MPI message is 2 bytes long.
	 * The length of PA messages and MPI messages is 4 bytes long because it is expected that occasionally
	 * an MPI table whose length cannot be expressed by a 2 bytes length fields. Also, note that a PA message
	 * includes at least one MPI table.
	 */

	if(mmt_signalling_message_header->message_id == PA_message) {
		buf = pa_message_parse(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = PA_message;

	} else if(mmt_signalling_message_header->message_id >= MPI_message_start && mmt_signalling_message_header->message_id < MPI_message_end) {
		buf = mpi_message_parse(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = MPI_message;

	} else if(mmt_signalling_message_header->message_id >= MPT_message_start && mmt_signalling_message_header->message_id <= MPT_message_end) {
		buf = mpt_message_parse(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = MPT_message;

	} else if(mmt_signalling_message_header->message_id == CRI_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = CRI_message;

	} else if(mmt_signalling_message_header->message_id == DCI_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = DCI_message;

	} else if(mmt_signalling_message_header->message_id == SSWR_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = SSWR_message;

	} else if(mmt_signalling_message_header->message_id == AL_FEC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = AL_FEC_message;
	} else if(mmt_signalling_message_header->message_id == HRBM_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = HRBM_message;
	} else if(mmt_signalling_message_header->message_id == MC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = MC_message;

	} else if(mmt_signalling_message_header->message_id == AC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = AC_message;

	} else if(mmt_signalling_message_header->message_id == AF_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = AF_message;

	} else if(mmt_signalling_message_header->message_id == RQF_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = RQF_message;

	} else if(mmt_signalling_message_header->message_id == ADC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = ADC_message;

	} else if(mmt_signalling_message_header->message_id == HRB_removal_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = HRB_removal_message;

	} else if(mmt_signalling_message_header->message_id == LS_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = LS_message;

	} else if(mmt_signalling_message_header->message_id == LR_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = LR_message;

	} else if(mmt_signalling_message_header->message_id == NAMF_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = NAMF_message;

	} else if(mmt_signalling_message_header->message_id == LDC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = LDC_message;

	} else if(mmt_signalling_message_header->message_id == MMT_ATSC3_MESSAGE_ID) {
		buf = mmt_atsc3_message_payload_parse(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = MMT_ATSC3_MESSAGE_ID;

	} else if(mmt_signalling_message_header->message_id == MMT_SCTE35_Signal_Message) {
		buf = mmt_scte35_message_payload_parse(mmt_signalling_message_header_and_payload, buf, buf_size);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = MMT_SCTE35_Signal_Message;

	} else {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	}

	return buf;

}
////parse the mmtp payload header for signaling message mode
//uint8_t	mmtp_payload_header[2];
//buf = extract(buf, mmtp_payload_header, 2);



uint8_t* pa_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, uint8_t* udp_raw_buf, uint32_t udp_raw_buf_size) {
	udp_raw_buf = __mmt_signaling_message_parse_length_long(udp_raw_buf, mmt_signalling_message_header_and_payload);

	uint8_t *raw_buf = udp_raw_buf;
	uint8_t *buf = udp_raw_buf;

	_MMSM_WARN("signalling information message id not supported: 0x%04x", mmt_signalling_message_header_and_payload->message_header.message_id);

	return udp_raw_buf;
}
uint8_t* mpi_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, uint8_t* udp_raw_buf, uint32_t buf_size) {

	udp_raw_buf = __mmt_signaling_message_parse_length_long(udp_raw_buf, mmt_signalling_message_header_and_payload);
	uint8_t *raw_buf = udp_raw_buf;
	uint8_t *buf = udp_raw_buf;

	_MMSM_WARN("signalling information message id not supported: 0x%04x", mmt_signalling_message_header_and_payload->message_header.message_id);

	return udp_raw_buf;
}

uint8_t* __read_uint8_len_to_string(uint8_t* buf, uint8_t len, uint8_t** dest_p) {

	if (len) {
		uint8_t* temp_str = calloc(len + 1, sizeof(char));
		//not efficent, but oh well
		for (int i = 0; i < len; i++) {
			buf = extract(buf, (uint8_t*) &temp_str[i], 1);
		}
		*dest_p = temp_str;
	}
	return buf;
}


uint8_t* __read_uint16_len_to_string(uint8_t* buf, uint16_t len, uint8_t** dest_p) {

	if (len) {
		uint8_t* temp_str = calloc(len + 1, sizeof(char));
		//not efficent, but oh well
		for (int i = 0; i < len; i++) {
			buf = extract(buf, (uint8_t*) &temp_str[i], 1);
		}
		*dest_p = temp_str;
	}
	return buf;
}

uint8_t* __read_uint32_len_to_string(uint8_t* buf, uint32_t len, uint8_t** dest_p) {

	if (len) {
		uint8_t* temp_str = calloc(len + 1, sizeof(char));
		//not efficent, but oh well
		for (int i = 0; i < len; i++) {
			buf = extract(buf, (uint8_t*) &temp_str[i], 1);
		}
		*dest_p = temp_str;
	}
	return buf;
}

uint8_t* __read_mmt_general_location_info(uint8_t* buf, mmt_general_location_info_t* mmt_general_location_info) {
	buf = extract(buf, (uint8_t*)&mmt_general_location_info->location_type, 1);

	if(mmt_general_location_info->location_type == 0x00) {
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->packet_id, 2);
		mmt_general_location_info->packet_id = ntohs(mmt_general_location_info->packet_id);
	} else if(mmt_general_location_info->location_type == 0x01) {
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->ipv4_src_addr, 4);
		mmt_general_location_info->ipv4_src_addr = ntohl(mmt_general_location_info->ipv4_src_addr);
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->ipv4_dst_addr, 4);
		mmt_general_location_info->ipv4_dst_addr = ntohl(mmt_general_location_info->ipv4_dst_addr);

		buf = extract(buf, (uint8_t*)&mmt_general_location_info->dst_port, 2);
		mmt_general_location_info->packet_id = ntohs(mmt_general_location_info->dst_port);

		buf = extract(buf, (uint8_t*)&mmt_general_location_info->packet_id, 2);
		mmt_general_location_info->packet_id = ntohs(mmt_general_location_info->packet_id);

	} else if(mmt_general_location_info->location_type == 0x02) {
		//noop
	} else if(mmt_general_location_info->location_type == 0x0A) {
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->ipv4_src_addr, 4);
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->ipv4_dst_addr, 4);
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->dst_port, 2);
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->packet_id, 2);
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->message_id, 2);

	}

	return buf;
}


uint8_t* mpt_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, uint8_t* udp_raw_buf, uint32_t buf_size) {
	udp_raw_buf = __mmt_signaling_message_parse_length_short(udp_raw_buf, mmt_signalling_message_header_and_payload);

	//we have already consumed the mpt_message, now we are processing the mp_table
	uint8_t *raw_buf = udp_raw_buf;
	uint8_t *buf = udp_raw_buf;
	mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;

	uint8_t table_id;
	buf = extract(buf, &table_id, 1);
	mp_table->table_id = table_id;

	//if message_id==20 - full message, otherwise subset n-1

	uint8_t version;
	buf = extract(buf, &version, 1);
	mp_table->version = version;

	uint16_t length;
	buf = extract(buf,(uint8_t*)&length, 2);
	mp_table->length = ntohs(length);


	uint8_t reserved_mp_table_mode;
	buf = extract(buf, &reserved_mp_table_mode, 1);
	if((reserved_mp_table_mode >> 2) != 0x3F) {
		_MMSM_WARN("mp_table reserved 6 bits are not set - message_id: 0x%04x, table_id: 0x%02x", mmt_signalling_message_header_and_payload->message_header.message_id, mp_table->table_id);
		goto cleanup;
	}
	//set MP_table_mode
	mp_table->mp_table_mode = reserved_mp_table_mode & 0x3;

	if(mp_table->table_id == 0x20 || mp_table->table_id == 0x11) {
		//process packages & descriptors
		//read mmt_package_id here
		uint8_t mmt_package_id_length;
		buf = extract(buf, &mmt_package_id_length, 1);
		mp_table->mmt_package_id.mmt_package_id_length = mmt_package_id_length;

		buf = __read_uint8_len_to_string(buf, mmt_package_id_length, &mp_table->mmt_package_id.mmt_package_id);
	
        uint16_t table_descriptors_length;
        buf = extract(buf, (uint8_t*)&table_descriptors_length, 2);
        
        mp_table->mp_table_descriptors.mp_table_descriptors_length = ntohs(table_descriptors_length);
        if(mp_table->mp_table_descriptors.mp_table_descriptors_length > 0) {
            //TODO: bounds check this untrusted read..
            _MMSM_DEBUG("reading mp_table_descriptors size: %u", mp_table->mp_table_descriptors.mp_table_descriptors_length);
            mp_table->mp_table_descriptors.mp_table_descriptors_byte = calloc(mp_table->mp_table_descriptors.mp_table_descriptors_length, sizeof(uint8_t));
            buf = extract(buf, (uint8_t*)&mp_table->mp_table_descriptors.mp_table_descriptors_byte, mp_table->mp_table_descriptors.mp_table_descriptors_length);
        }
    }

	uint8_t number_of_assets;
	buf = extract(buf, &number_of_assets, 1);
	number_of_assets = __CLIP(number_of_assets, 0, 255);
	mp_table->number_of_assets = number_of_assets;

	mp_table->mp_table_asset_row = calloc(number_of_assets, sizeof(mp_table_asset_row_t));
	for(int i=0; i < mp_table->number_of_assets; i++ ) {
		mp_table_asset_row_t* row = &mp_table->mp_table_asset_row[i];

		//grab our identifer mapping
		uint8_t identifier_type;
		buf = extract(buf, &identifier_type, 1);
		row->identifier_mapping.identifier_type = identifier_type;
		if(row->identifier_mapping.identifier_type == 0x00) {
			uint32_t asset_id_scheme;

			buf = extract(buf, (uint8_t*)&asset_id_scheme, 4);
			row->identifier_mapping.asset_id.asset_id_scheme = ntohl(asset_id_scheme);

			uint32_t asset_id_length;
			buf = extract(buf, (uint8_t*)&asset_id_length, 4);
			row->identifier_mapping.asset_id.asset_id_length = ntohl(asset_id_length);
			buf = __read_uint32_len_to_string(buf, row->identifier_mapping.asset_id.asset_id_length, &row->identifier_mapping.asset_id.asset_id);


		} else if(row->identifier_mapping.identifier_type == 0x01) {
			//build url

		}
		uint32_t asset_type;
		buf = extract(buf, (uint8_t*)&asset_type, 4);
		row->asset_type = ntohl(asset_type);

		uint8_t reserved_default_asset_flag;
		buf = extract(buf, (uint8_t*)&reserved_default_asset_flag, 1);
		row->default_asset_flag = (reserved_default_asset_flag >> 1) & 0x1;
		row->asset_clock_relation_flag = reserved_default_asset_flag & 0x1;
		if(row->asset_clock_relation_flag) {
			buf = extract(buf, (uint8_t*)&row->asset_clock_relation_id, 1);
			uint8_t reserved_asset_timescale_flag;
			buf = extract(buf, (uint8_t*)&reserved_asset_timescale_flag, 1);
			row->asset_timescale_flag = reserved_asset_timescale_flag & 0x1;
			if(row->asset_timescale_flag) {
				buf = extract(buf, (uint8_t*)&row->asset_timescale, 4);
				row->asset_timescale = ntohl(row->asset_timescale);

			}
		}
		buf = extract(buf, (uint8_t*)&row->location_count, 1);
		//build out mmt_general_location_info N times.....
		buf = __read_mmt_general_location_info(buf, &row->mmt_general_location_info);

        //asset_descriptors
        uint16_t asset_descriptors_length;
		buf = extract(buf, (uint8_t*)&asset_descriptors_length, 2);    
        row->asset_descriptors_length = ntohs(asset_descriptors_length);

		buf = __read_uint16_len_to_string(buf, row->asset_descriptors_length, &row->asset_descriptors_payload);

        //peek at
        if(row->asset_descriptors_length) {
            if(row->asset_descriptors_payload[0] == 0x00 && row->asset_descriptors_payload[1] == 0x01) {
                row->mmt_signaling_message_mpu_timestamp_descriptor = calloc(1, sizeof(mmt_signaling_message_mpu_timestamp_descriptor_t));
                row->mmt_signaling_message_mpu_timestamp_descriptor->descriptor_tag = 0x0001;
                row->mmt_signaling_message_mpu_timestamp_descriptor->descriptor_length = row->asset_descriptors_payload[2];
                row->mmt_signaling_message_mpu_timestamp_descriptor->mpu_tuple_n = row->mmt_signaling_message_mpu_timestamp_descriptor->descriptor_length / 12;
                
                if(row->mmt_signaling_message_mpu_timestamp_descriptor->mpu_tuple_n) {
                    row->mmt_signaling_message_mpu_timestamp_descriptor->mpu_tuple = calloc(row->mmt_signaling_message_mpu_timestamp_descriptor->mpu_tuple_n, sizeof(mmt_signaling_message_mpu_tuple_t));
                    for(int i=0; i < row->mmt_signaling_message_mpu_timestamp_descriptor->mpu_tuple_n; i++) {
                        uint32_t mpu_sequence_number;
                        uint64_t mpu_presentation_time;
                        memcpy(&mpu_sequence_number, &row->asset_descriptors_payload[3 + (i*12)], 4);
                        memcpy(&mpu_presentation_time, &row->asset_descriptors_payload[7 + (i*12)], 8);

                        row->mmt_signaling_message_mpu_timestamp_descriptor->mpu_tuple[i].mpu_sequence_number = ntohl(mpu_sequence_number);
                        row->mmt_signaling_message_mpu_timestamp_descriptor->mpu_tuple[i].mpu_presentation_time = ntohll(mpu_presentation_time);

                    }
                }
            }
        }
	}


cleanup:

	return NULL;
}

uint8_t* mmt_atsc3_message_payload_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, uint8_t* udp_raw_buf, uint32_t udp_raw_buf_size) {
	udp_raw_buf = __mmt_signaling_message_parse_length_long(udp_raw_buf, mmt_signalling_message_header_and_payload);

	uint8_t *raw_buf = udp_raw_buf;
	uint8_t *buf = udp_raw_buf;

	mmt_atsc3_message_payload_t* mmt_atsc3_message_payload = &mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload;

	uint16_t service_id;
	buf = extract(buf, (uint8_t*)&service_id, 2);
	mmt_atsc3_message_payload->service_id = ntohs(service_id);

	uint16_t atsc3_message_content_type;
	buf = extract(buf, (uint8_t*)&atsc3_message_content_type, 2);
	mmt_atsc3_message_payload->atsc3_message_content_type = ntohs(atsc3_message_content_type);

	uint8_t atsc3_message_content_version;
	buf = extract(buf, (uint8_t*)&atsc3_message_content_version, 1);
	mmt_atsc3_message_payload->atsc3_message_content_version = atsc3_message_content_version;

	uint8_t atsc3_message_content_compression;
	buf = extract(buf, (uint8_t*)&atsc3_message_content_compression, 1);
	mmt_atsc3_message_payload->atsc3_message_content_compression = atsc3_message_content_compression;

	uint8_t 	URI_length;
	buf = extract(buf, (uint8_t*)&URI_length, 1);
	mmt_atsc3_message_payload->URI_length = __MAX(0, __MIN(255, URI_length));

	if(URI_length) {
		buf = __read_uint8_len_to_string(buf, URI_length, &mmt_atsc3_message_payload->URI_payload);
	}

	uint32_t temp_atsc3_message_content_length;
	buf = extract(buf, (uint8_t*)&temp_atsc3_message_content_length, 4);
	temp_atsc3_message_content_length = ntohl(temp_atsc3_message_content_length);

	if(temp_atsc3_message_content_length) {
		//cheat and over-alloc+1 for a null byte

		uint8_t *temp_atsc3_message_content = NULL;
		buf = __read_uint32_len_to_string(buf, temp_atsc3_message_content_length, &temp_atsc3_message_content);

		if(mmt_atsc3_message_payload->atsc3_message_content_compression == 0x02) {
			mmt_atsc3_message_payload->atsc3_message_content_length_compressed = temp_atsc3_message_content_length;
			mmt_atsc3_message_payload->atsc3_message_content_compressed = temp_atsc3_message_content;

			//ungzip
			uint8_t *decompressed_payload;
			int32_t ret = atsc3_unzip_gzip_payload(mmt_atsc3_message_payload->atsc3_message_content_compressed, mmt_atsc3_message_payload->atsc3_message_content_length_compressed, &decompressed_payload);

			if(ret > 0) {
				mmt_atsc3_message_payload->atsc3_message_content_length = ret;
				mmt_atsc3_message_payload->atsc3_message_content = decompressed_payload;
			} else {
				_MMSM_ERROR("atsc3_message_content_compressed, unable to decompress: error is: %u", ret);
			}

		} else {
			//treat this as uncompressed for now..
			mmt_atsc3_message_payload->atsc3_message_content_length = temp_atsc3_message_content_length;
			mmt_atsc3_message_payload->atsc3_message_content = temp_atsc3_message_content;
		}
	}

	return buf;
}

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmt_scte35_message_payload, mmt_scte35_signal_descriptor)

uint8_t* mmt_scte35_message_payload_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, uint8_t* udp_raw_buf, uint32_t udp_raw_buf_size) {

	udp_raw_buf = __mmt_signaling_message_parse_length_long(udp_raw_buf, mmt_signalling_message_header_and_payload);

	uint8_t *raw_buf = udp_raw_buf;
	uint8_t *buf = udp_raw_buf;

	mmt_scte35_message_payload_t* mmt_scte35_message_payload = &mmt_signalling_message_header_and_payload->message_payload.mmt_scte35_message_payload;

	//walk thru each signal descriptor
	uint8_t scte35_signal_descriptor_n;
	buf = extract(buf, (uint8_t*)&scte35_signal_descriptor_n, 1);
	for(int i=0; i < scte35_signal_descriptor_n && (udp_raw_buf_size > (buf-raw_buf)); i++) {
		//make sure we have at least 19 bytes available (16+16+64+7+33+16)
		if(19 < udp_raw_buf_size - (buf-raw_buf)) {
			_MMSM_WARN("mmt_scte35_message_payload_parse: short read for descriptor: %u, need 19 but remaining is: %ld", i, (udp_raw_buf_size - (buf-raw_buf)));
			goto parse_incomplete;
		}

		//parse out each descriptor
		mmt_scte35_signal_descriptor_t* mmt_scte35_signal_descriptor = mmt_scte35_signal_descriptor_new();
		buf = extract(buf, (uint8_t*)&mmt_scte35_signal_descriptor->descriptor_tag, 2);
		buf = extract(buf, (uint8_t*)&mmt_scte35_signal_descriptor->descriptor_length, 2);
		buf = extract(buf, (uint8_t*)&mmt_scte35_signal_descriptor->ntp_timestamp, 8);

		//pts_timestamp is 1+32
		uint8_t pts_timestamp_block[5];
		buf = extract(buf, (uint8_t*)&pts_timestamp_block, 5);
		mmt_scte35_signal_descriptor->pts_timestamp |= ((pts_timestamp_block[0] & 0x1UL) << 33);
		mmt_scte35_signal_descriptor->pts_timestamp |= ntohl(*(uint32_t*)(&pts_timestamp_block[1]));

		buf = extract(buf, (uint8_t*)&mmt_scte35_signal_descriptor->signal_length, 2);

		if(mmt_scte35_signal_descriptor->signal_length > udp_raw_buf_size - (buf-raw_buf)) {
			_MMSM_WARN("mmt_scte35_message_payload_parse: signal length for descriptor: %u, need %u but remaining is: %ld", i, mmt_scte35_signal_descriptor->signal_length, (udp_raw_buf_size - (buf-raw_buf)));
			goto parse_incomplete;
		}

		buf = extract(buf, (uint8_t*)&mmt_scte35_signal_descriptor->signal_byte, mmt_scte35_signal_descriptor->signal_length);
		mmt_scte35_message_payload_add_mmt_scte35_signal_descriptor(&mmt_signalling_message_header_and_payload->message_payload.mmt_scte35_message_payload, mmt_scte35_signal_descriptor);
		_MMSM_INFO("mmt_scte35_message_payload_parse: adding signal at NTP_timestamp: %llu, PTS: %llu", mmt_scte35_signal_descriptor->ntp_timestamp, mmt_scte35_signal_descriptor->pts_timestamp);
	}

parse_incomplete:

	return buf;
}


void mmt_atsc3_message_payload_dump(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {

	mmt_atsc3_message_payload_t* mmt_atsc3_message_payload = &mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload;

	_MMSM_DEBUG("-----------------");
	_MMSM_DEBUG("mmt_atsc3_message");
	_MMSM_DEBUG("-----------------");
	_MMSM_DEBUG("service_id:                        %u", mmt_atsc3_message_payload->service_id);
	_MMSM_DEBUG("atsc3_message_content_type:        %u", mmt_atsc3_message_payload->atsc3_message_content_type);
	_MMSM_DEBUG("atsc3_message_content_version:     %u", mmt_atsc3_message_payload->atsc3_message_content_version);
	_MMSM_DEBUG("atsc3_message_content_compression: %u", mmt_atsc3_message_payload->atsc3_message_content_compression);
	_MMSM_DEBUG("URI_length:                        %u", mmt_atsc3_message_payload->URI_length);
	_MMSM_DEBUG("URI_payload:                       %s", mmt_atsc3_message_payload->URI_payload);
	if(mmt_atsc3_message_payload->atsc3_message_content_compression == 0x02) {
		_MMSM_DEBUG("atsc3_message_content_length_compressed:      %u", mmt_atsc3_message_payload->atsc3_message_content_length_compressed);
	}
	_MMSM_DEBUG("atsc3_message_content_length:      %u", mmt_atsc3_message_payload->atsc3_message_content_length);
	_MMSM_DEBUG("atsc3_message_content:             %s", mmt_atsc3_message_payload->atsc3_message_content);

}


uint8_t* si_message_not_supported(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, uint8_t* udp_raw_buf, uint32_t udp_raw_buf_size) {
	_MMSM_WARN("signalling information message id not supported: 0x%04x", mmt_signalling_message_header_and_payload->message_header.message_id);

	return NULL;
}



void signaling_message_mmtp_packet_header_dump(mmtp_payload_fragments_union_t* mmtp_payload_fragments) {
	_MMSM_DEBUG("------------------");
	_MMSM_DEBUG("MMTP Packet Header");
	_MMSM_DEBUG("------------------");
	_MMSM_DEBUG(" packet version         : %-10d (0x%d%d)", mmtp_payload_fragments->mmtp_packet_header.mmtp_packet_version, ((mmtp_payload_fragments->mmtp_packet_header.mmtp_packet_version >> 1) & 0x1), mmtp_payload_fragments->mmtp_packet_header.mmtp_packet_version & 0x1);
	_MMSM_DEBUG(" payload_type           : %-10d (0x%d%d)", mmtp_payload_fragments->mmtp_packet_header.mmtp_payload_type, ((mmtp_payload_fragments->mmtp_packet_header.mmtp_payload_type >> 1) & 0x1), mmtp_payload_fragments->mmtp_packet_header.mmtp_payload_type & 0x1);
	_MMSM_DEBUG(" packet_id              : %-10hu (0x%04x)", mmtp_payload_fragments->mmtp_packet_header.mmtp_packet_id, mmtp_payload_fragments->mmtp_packet_header.mmtp_packet_id);
	_MMSM_DEBUG(" timestamp              : %-10u (0x%08x)", mmtp_payload_fragments->mmtp_packet_header.mmtp_timestamp, mmtp_payload_fragments->mmtp_packet_header.mmtp_timestamp);
	_MMSM_DEBUG(" packet_sequence_number : %-10u (0x%08x)", mmtp_payload_fragments->mmtp_packet_header.packet_sequence_number,mmtp_payload_fragments->mmtp_packet_header.packet_sequence_number);
	_MMSM_DEBUG(" packet counter         : %-10u (0x%04x)", mmtp_payload_fragments->mmtp_packet_header.packet_counter, mmtp_payload_fragments->mmtp_packet_header.packet_counter);
	_MMSM_DEBUG("------------------");
}

void signaling_message_dump(mmtp_payload_fragments_union_t* mmtp_payload_fragments) {
	if(mmtp_payload_fragments->mmtp_packet_header.mmtp_payload_type != 0x02) {
		_MMSM_ERROR("signaling_message_dump, payload_type 0x%x != 0x02", mmtp_payload_fragments->mmtp_packet_header.mmtp_payload_type);
		return;
	}

	//dump mmtp packet header
	signaling_message_mmtp_packet_header_dump(mmtp_payload_fragments);

	_MMSM_DEBUG("-----------------");
	_MMSM_DEBUG("Signaling Message");
	_MMSM_DEBUG("-----------------");
	/**
	 * dump si payload header fields
	 * 	uint8_t		si_fragmentation_indiciator; //2 bits,
		uint8_t		si_additional_length_header; //1 bit
		uint8_t		si_aggregation_flag; 		 //1 bit
		uint8_t		si_fragmentation_counter;    //8 bits
		uint16_t	si_aggregation_message_length;
	 */
	_MMSM_DEBUG(" fragmentation_indiciator   : %d", 	mmtp_payload_fragments->mmtp_signalling_message_fragments.si_fragmentation_indiciator);
	_MMSM_DEBUG(" additional_length_header   : %d", 	mmtp_payload_fragments->mmtp_signalling_message_fragments.si_additional_length_header);
	_MMSM_DEBUG(" aggregation_flag           : %d",	mmtp_payload_fragments->mmtp_signalling_message_fragments.si_aggregation_flag);
	_MMSM_DEBUG(" fragmentation_counter      : %d",	mmtp_payload_fragments->mmtp_signalling_message_fragments.si_fragmentation_counter);
	_MMSM_DEBUG(" aggregation_message_length : %hu",	mmtp_payload_fragments->mmtp_signalling_message_fragments.si_aggregation_message_length);

	for(int i=0; i < mmtp_payload_fragments->mmtp_signalling_message_fragments.mmt_signalling_message_vector.messages_n; i++) {
		mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmtp_payload_fragments->mmtp_signalling_message_fragments.mmt_signalling_message_vector.messages[i];

		_MMSM_DEBUG("-----------------");
		_MMSM_DEBUG(" Message ID      : %hu (0x%04x)", 	mmt_signalling_message_header_and_payload->message_header.message_id, mmt_signalling_message_header_and_payload->message_header.message_id);
		_MMSM_DEBUG(" Version         : %d", 			mmt_signalling_message_header_and_payload->message_header.version);
		_MMSM_DEBUG(" Length          : %u", 			mmt_signalling_message_header_and_payload->message_header.length);
		_MMSM_DEBUG("------------------");
		_MMSM_DEBUG(" Payload         : %p", 			&mmt_signalling_message_header_and_payload->message_payload);
		_MMSM_DEBUG("------------------");


		if(mmt_signalling_message_header_and_payload->message_header.message_id == PA_message) {
			pa_message_dump(mmtp_payload_fragments);
		} else if(mmt_signalling_message_header_and_payload->message_header.message_id >= MPI_message_start && mmt_signalling_message_header_and_payload->message_header.message_id < MPI_message_end) {
			mpi_message_dump(mmtp_payload_fragments);
		} else if(mmt_signalling_message_header_and_payload->message_header.message_id >= MPT_message_start && mmt_signalling_message_header_and_payload->message_header.message_id < MPT_message_end) {
			mpt_message_dump(mmt_signalling_message_header_and_payload);
		} else if(mmt_signalling_message_header_and_payload->message_header.message_id == MMT_ATSC3_MESSAGE_ID) {
			mmt_atsc3_message_payload_dump(mmt_signalling_message_header_and_payload);
		}
	}
}

void pa_message_dump(mmtp_payload_fragments_union_t* mmtp_payload_fragments) {
	_MMSM_DEBUG(" pa_message");
	_MMSM_DEBUG("-----------------");

}

void mpi_message_dump(mmtp_payload_fragments_union_t* mmtp_payload_fragments) {
	_MMSM_DEBUG(" mpi_message");
	_MMSM_DEBUG("-----------------");

}

void mpt_message_dump(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {

	_MMSM_DEBUG(" mpt_message");
	_MMSM_DEBUG("-----------------");

	mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;

	_MMSM_DEBUG(" table_id                    : %u", mp_table->table_id);
	_MMSM_DEBUG(" version                     : %u", mp_table->version);
	_MMSM_DEBUG(" length                      : %u", mp_table->length);
	_MMSM_DEBUG(" mp_table_mode               : %u", mp_table->mp_table_mode);
	_MMSM_DEBUG(" mmt_package_id.length:      : %u", mp_table->mmt_package_id.mmt_package_id_length);
	if(mp_table->mmt_package_id.mmt_package_id_length) {
		_MMSM_DEBUG(" mmt_package_id.val:       : %s", mp_table->mmt_package_id.mmt_package_id);
	}
	_MMSM_DEBUG(" mp_table_descriptors.length : %u", mp_table->mp_table_descriptors.mp_table_descriptors_length);
	if(mp_table->mp_table_descriptors.mp_table_descriptors_length) {
		_MMSM_DEBUG(" mp_table_descriptors.val    : %s", mp_table->mp_table_descriptors.mp_table_descriptors_byte);
	}
	_MMSM_DEBUG(" number_of_assets            : %u", mp_table->number_of_assets);

	for(int i=0; i < mp_table->number_of_assets; i++) {
		mp_table_asset_row_t* mp_table_asset_row = &mp_table->mp_table_asset_row[i];
		_MMSM_DEBUG(" asset identifier type       : %u", mp_table_asset_row->identifier_mapping.identifier_type);
		if(mp_table_asset_row->identifier_mapping.identifier_type == 0x00) {
			_MMSM_DEBUG(" asset id                    : %s", mp_table_asset_row->identifier_mapping.asset_id.asset_id);

		}
		_MMSM_DEBUG(" asset type                  : %u", mp_table_asset_row->asset_type);
		_MMSM_DEBUG(" asset_clock_relation_flag   : %u", mp_table_asset_row->asset_clock_relation_flag);
		_MMSM_DEBUG(" asset_clock_relation_id     : %u", mp_table_asset_row->asset_clock_relation_id);
		_MMSM_DEBUG(" asset_timescale_flag        : %u", mp_table_asset_row->asset_timescale_flag);
		_MMSM_DEBUG(" asset_timescale             : %u", mp_table_asset_row->asset_timescale);
		_MMSM_DEBUG(" location_count              : %u", mp_table_asset_row->location_count);
//		for(int j=0; j < mp_table_asset_row->location_count; j++) {
//
//		}
		_MMSM_DEBUG(" mmt_general_location_info location_type  : %u", mp_table_asset_row->mmt_general_location_info.location_type);
		_MMSM_DEBUG(" mmt_general_location_info pkt_id         : %u", mp_table_asset_row->mmt_general_location_info.packet_id);
		_MMSM_DEBUG(" mmt_general_location_info ipv4 src addr  : %u", mp_table_asset_row->mmt_general_location_info.ipv4_src_addr);
		_MMSM_DEBUG(" mmt_general_location_info ipv4 dest addr : %u", mp_table_asset_row->mmt_general_location_info.ipv4_dst_addr);
		_MMSM_DEBUG(" mmt_general_location_info ipv4 dest port : %u", mp_table_asset_row->mmt_general_location_info.dst_port);
		_MMSM_DEBUG(" mmt_general_location_info message id     : %u", mp_table_asset_row->mmt_general_location_info.message_id);

		//first entry
		_MMSM_DEBUG(" asset_descriptors_length                 : %u", mp_table_asset_row->asset_descriptors_length);
		if(mp_table_asset_row->asset_descriptors_length) {
            if(mp_table_asset_row->mmt_signaling_message_mpu_timestamp_descriptor) {
                for(int i=0; i < mp_table_asset_row->mmt_signaling_message_mpu_timestamp_descriptor->mpu_tuple_n; i++) {
                    _MMSM_DEBUG("   mpu_timestamp_descriptor %u, mpu_sequence_number: %u", i, mp_table_asset_row->mmt_signaling_message_mpu_timestamp_descriptor->mpu_tuple[i].mpu_sequence_number);
                    _MMSM_DEBUG("   mpu_timestamp_descriptor %u, mpu_presentation_time: %llu", i, mp_table_asset_row->mmt_signaling_message_mpu_timestamp_descriptor->mpu_tuple[i].mpu_presentation_time);
                }
            }
		}
	}

}


