/*
 * atsc3_mmt_signalling_message.h
 *
 *  Created on: Jan 21, 2019
 *      Author: jjustman
 */

#include "atsc3_mmt_signalling_message.h"

int _MMT_SIGNALLING_MESSAGE_ERROR_23008_1_ENABLED = 0;
int _MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED = 0;
int _MMT_SIGNALLING_MESSAGE_TRACE_ENABLED = 0;


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

//release our packet_header once we have a concrete object type
mmtp_signalling_packet_t* mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(mmtp_packet_header_t** mmtp_packet_header_p, block_t* udp_packet) {
	mmtp_packet_header_t* mmtp_packet_header = *mmtp_packet_header_p;
	mmtp_signalling_packet_t* mmtp_signalling_packet = NULL;

	if(mmtp_packet_header) {
		mmtp_signalling_packet = mmtp_signalling_packet_parse_from_block_t(mmtp_packet_header, udp_packet);
		mmtp_packet_header_free(mmtp_packet_header_p);

		if(mmtp_signalling_packet->si_fragmentation_indicator != 0x0) {
            mmtp_signalling_packet->udp_packet_inner_msg_payload = block_Duplicate_from_position(udp_packet);
		}
	}

	return mmtp_signalling_packet;
}

mmtp_signalling_packet_t* mmtp_signalling_packet_parse_from_block_t(mmtp_packet_header_t* mmtp_packet_header, block_t* udp_packet) {
    uint8_t  mmtp_si_fi_reserved_ha_byte = 0;
    uint8_t  mmtp_si_frag_counter = 0;

	if(mmtp_packet_header->mmtp_payload_type != 0x02) {
		__MMSM_ERROR("signalling_message_parse_payload_header: mmtp_payload_type 0x02 != 0x%x", mmtp_packet_header->mmtp_payload_type);
		return NULL;
	}

	mmtp_signalling_packet_t* mmtp_signalling_packet = mmtp_signalling_packet_new();

	//hack-ish, probably not endian safe...
	memcpy(mmtp_signalling_packet, mmtp_packet_header, sizeof(mmtp_packet_header_t));

    mmtp_si_fi_reserved_ha_byte = block_Read_uint8(udp_packet);
    mmtp_si_frag_counter = block_Read_uint8(udp_packet);

    /*
	 * f_i: bits 0-1 fragmentation indicator:

         0x0 (00) = payload contains one or more complete signalling messages
         0x1 (01) = payload contains the first fragment of a signalling message
         0x2 (10) = payload contains a fragment of a signalling message that is neither first/last
         0x3 (11) = payload contains the last fragment of a signalling message
	 */

	mmtp_signalling_packet->si_fragmentation_indicator = (mmtp_si_fi_reserved_ha_byte >> 6) & 0x03;
    __MMSM_DEBUG("mmtp_signalling_packet: mmtp_si_fi_reserved_ha_byte is: 0x%02x, mmtp_signalling_packet->si_fragmentation_indicator: 0x%02x, udp_len: %d",
                 mmtp_si_fi_reserved_ha_byte,
                 mmtp_signalling_packet->si_fragmentation_indicator,
                 udp_packet->p_size
            );

	//next 4 bits are 0x0000 reserved, output error message if we are validating against 23008-1:2017
	if((mmtp_si_fi_reserved_ha_byte >> 2) & 0xF) {
		__MMSM_ERROR_23008_1("mmt_signalling_message_parse_packet_header: signalling message mmtp header bits 2-5 are not reserved '0'");
	}

	//bit 6 is additional Header
	mmtp_signalling_packet->si_additional_length_header = ((mmtp_si_fi_reserved_ha_byte >> 1) & 0x1);

	//bit 7 is Aggregation
	mmtp_signalling_packet->si_aggregation_flag = (mmtp_si_fi_reserved_ha_byte & 0x1);

	//count of for how many fragments follow this message, e.g si_fragmentation_indicator != 0
	//note, packets are not allowed to be both aggregated and fragmented

	mmtp_signalling_packet->si_fragmentation_counter = mmtp_si_frag_counter;

	return mmtp_signalling_packet;
}

/**
 * TODO - move block_t pointer
 * return -1 for error extracting mmt_signaling_message payloads
 */


int8_t mmt_signalling_message_parse_packet(mmtp_signalling_packet_t* mmtp_signalling_packet, block_t* udp_packet) {
	int8_t processed_messages_count = -1;

	if(mmtp_signalling_packet->mmtp_payload_type != 0x02) {
		__MMSM_ERROR("signalling_message_parse_payload_header: mmtp_payload_type 0x02 != 0x%x", mmtp_signalling_packet->mmtp_payload_type);
		return processed_messages_count;
	}

	if(mmtp_signalling_packet->si_aggregation_flag) {
		uint32_t mmtp_aggregation_msg_length = 0;

		while(block_Remaining_size(udp_packet)) {
			if(mmtp_signalling_packet->si_additional_length_header) {
				//read the full 32 bits for MSG_length
                mmtp_aggregation_msg_length = block_Read_uint32_ntohl(udp_packet);

			} else {
				//only read 16 bits for MSG_length
                mmtp_aggregation_msg_length = block_Read_uint16_ntohs(udp_packet);
            }

			if(mmtp_aggregation_msg_length > block_Remaining_size(udp_packet)) {
                __MMSM_ERROR("mmt_signalling_message_parse_packet: aggregation_flag=1, and mmtp_aggregation_msg_length: %d is greater than block remaining size for udp_packet: %p, len: %d!", mmtp_aggregation_msg_length, udp_packet, block_Remaining_size(udp_packet));
			} else {
                //build a msg from buf to buf+mmtp_aggregation_msg_length
                __MMSM_DEBUG("mmt_signalling_message_parse_packet: aggregation_flag=1 is UNTESTED - mmtp_aggregation_msg_length is: %d, udp_packet: %p", mmtp_aggregation_msg_length, udp_packet);
                processed_messages_count += mmt_signalling_message_parse_id_type(mmtp_signalling_packet, udp_packet);
            }
		}
	} else if(block_Remaining_size(udp_packet)) {
		//parse a single message (from a re-constituted udp_packet msg_payload
		processed_messages_count = mmt_signalling_message_parse_id_type(mmtp_signalling_packet, udp_packet);
	}
	return processed_messages_count;
}

mmt_signalling_message_header_and_payload_t* __mmt_signalling_message_parse_length_long(block_t* udp_packet, mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {
	uint32_t mmtp_msg_length_long = 0;

	if(block_Remaining_size(udp_packet) < 4) {
        __MMSM_WARN("__mmt_signalling_message_parse_length_long: needed 4 bytes for msg_length, but only %d bytes remaining in udp_packet",
                    block_Remaining_size(udp_packet))
		return NULL;
	}

    mmtp_msg_length_long = block_Read_uint32_ntohl(udp_packet);
	if(mmtp_msg_length_long <= block_Remaining_size(udp_packet)) {
        mmt_signalling_message_header_and_payload->message_header.length = mmtp_msg_length_long;
        return mmt_signalling_message_header_and_payload;
	} else {
        __MMSM_WARN("__mmt_signalling_message_parse_length_long: truncated mmtp signalling message, mmtp_msg_length_short: %d bytes, but only %d bytes remaining in udp_packet",
                    mmtp_msg_length_long, block_Remaining_size(udp_packet));
	}
	return NULL;
}

mmt_signalling_message_header_and_payload_t* __mmt_signalling_message_parse_length_short(block_t* udp_packet, mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {
	uint16_t mmtp_msg_length_short = 0;

	if(block_Remaining_size(udp_packet) < 2) {
        __MMSM_WARN("__mmt_signalling_message_parse_length_short: needed 2 bytes for msg_length, but only %d bytes remaining in udp_packet",
                    block_Remaining_size(udp_packet));
        return NULL;
    }

	mmtp_msg_length_short = block_Read_uint16_ntohs(udp_packet);
    if(mmtp_msg_length_short <= block_Remaining_size(udp_packet)) {
        mmt_signalling_message_header_and_payload->message_header.length = mmtp_msg_length_short;
        return mmt_signalling_message_header_and_payload;
    } else {
        __MMSM_WARN("__mmt_signalling_message_parse_length_short: truncated mmtp signalling message, mmtp_msg_length_short: %d bytes, but only %d bytes remaining in udp_packet",
                mmtp_msg_length_short, block_Remaining_size(udp_packet));
    }

	return NULL;
}

uint8_t mmt_signalling_message_parse_id_type(mmtp_signalling_packet_t* mmtp_signalling_packet, block_t* udp_packet) {

    uint8_t* buf = NULL;

    uint16_t message_id = 0;
    uint8_t  version = 0;

    //create general signalling message format
	message_id = block_Read_uint16_ntohs(udp_packet);
    version = block_Read_uint8(udp_packet);

	mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmt_signalling_message_header_and_payload_create(message_id, version);
	mmtp_signalling_packet_add_mmt_signalling_message_header_and_payload(mmtp_signalling_packet, mmt_signalling_message_header_and_payload);

	mmt_signalling_message_header_t* mmt_signalling_message_header = &mmt_signalling_message_header_and_payload->message_header;
	mmt_signalling_message_payload_u* mmt_signalling_message_payload = &mmt_signalling_message_header_and_payload->message_payload;

	/** each message parser is required to call either
	 * PA: 				__mmt_signalling_message_parse_length_long
	 * MPI: 			__mmt_signalling_message_parse_length_long
	 * mmt_atsc3_msg: 	__mmt_signalling_message_parse_length_long
	 *
	 * -all others-:
	 *	__mmt_signalling_message_parse_length_short
	 *
	 * length – this field indicates the length of the signalling message.
	 * This field for all signalling messages except PA messages and MPI message is 2 bytes long.
	 * The length of PA messages and MPI messages is 4 bytes long because it is expected that occasionally
	 * an MPI table whose length cannot be expressed by a 2 bytes length fields. Also, note that a PA message
	 * includes at least one MPI table.
	 */

	//jjustman-2020-11-12 - todo: clean this up for *_message_parse to use block_Read_....() and avoid pointer calculation for relative seek offset


	if(mmt_signalling_message_header->message_id == PA_message) {
		buf = pa_message_parse(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = PA_message;

	} else if(mmt_signalling_message_header->message_id >= MPI_message_start && mmt_signalling_message_header->message_id <= MPI_message_end) {
		buf = mpi_message_parse(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = MPI_message;

	} else if(mmt_signalling_message_header->message_id >= MPT_message_start && mmt_signalling_message_header->message_id <= MPT_message_end) {
		buf = mpt_message_parse(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = MPT_message;

	} else if(mmt_signalling_message_header->message_id == CRI_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = CRI_message;

	} else if(mmt_signalling_message_header->message_id == DCI_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = DCI_message;

	} else if(mmt_signalling_message_header->message_id == SSWR_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = SSWR_message;

	} else if(mmt_signalling_message_header->message_id == AL_FEC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = AL_FEC_message;

	} else if(mmt_signalling_message_header->message_id == HRBM_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = HRBM_message;

	} else if(mmt_signalling_message_header->message_id == MC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = MC_message;

	} else if(mmt_signalling_message_header->message_id == AC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = AC_message;

	} else if(mmt_signalling_message_header->message_id == AF_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = AF_message;

	} else if(mmt_signalling_message_header->message_id == RQF_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = RQF_message;

	} else if(mmt_signalling_message_header->message_id == ADC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = ADC_message;

	} else if(mmt_signalling_message_header->message_id == HRB_removal_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = HRB_removal_message;

	} else if(mmt_signalling_message_header->message_id == LS_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = LS_message;

	} else if(mmt_signalling_message_header->message_id == LR_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = LR_message;

	} else if(mmt_signalling_message_header->message_id == NAMF_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = NAMF_message;

	} else if(mmt_signalling_message_header->message_id == LDC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = LDC_message;

	} else if(mmt_signalling_message_header->message_id == MMT_ATSC3_MESSAGE_ID) {
		buf = mmt_atsc3_message_payload_parse(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = MMT_ATSC3_MESSAGE_ID;

		//jjustman-2020-12-03: TODO: perform any additional processing based upon the internal atsc3_message_content_type
        mmt_atsc3_message_payload_dump(mmt_signalling_message_header_and_payload);

	} else if(mmt_signalling_message_header->message_id == MMT_SCTE35_Signal_Message) {
		buf = mmt_scte35_message_payload_parse(mmt_signalling_message_header_and_payload, udp_packet);
		mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type = MMT_SCTE35_Signal_Message;

	} else {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, udp_packet);
	}

	//jjustman-2020-11-12 - super-hack - TODO: fix this logic
    uint8_t* buf_start = block_Get(udp_packet);

    //jjustman-2020-11-11 - keep our udp_packet position up to date (note: buf may be null if message type processing failed
	if(buf) {
        block_Seek_Relative(udp_packet, __MAX(0, buf - buf_start));
    }

	return (buf && buf != buf_start);

}


mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload_create(uint16_t message_id, uint8_t version) {
	mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = calloc(1, sizeof(mmt_signalling_message_header_and_payload_t));
	mmt_signalling_message_header_and_payload->message_header.message_id = message_id;
	mmt_signalling_message_header_and_payload->message_header.version = version;

	return mmt_signalling_message_header_and_payload;
}

uint8_t* pa_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet) {
	__mmt_signalling_message_parse_length_long(udp_packet, mmt_signalling_message_header_and_payload);

	__MMSM_WARN("signalling information message id not supported: 0x%04x", mmt_signalling_message_header_and_payload->message_header.message_id);

	return block_Get(udp_packet);
}
uint8_t* mpi_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet) {
	__mmt_signalling_message_parse_length_long(udp_packet, mmt_signalling_message_header_and_payload);

	__MMSM_WARN("signalling information message id not supported: 0x%04x", mmt_signalling_message_header_and_payload->message_header.message_id);

	return block_Get(udp_packet);
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

/*
 * __read_mmt_general_location_info: max read length clamped to 4+4+2+2+2 = 14 bytes
 */

uint8_t* __read_mmt_general_location_info(uint8_t* buf, uint32_t remaining_len, mmt_general_location_info_t* mmt_general_location_info) {

	if(!remaining_len) {
		return NULL;
	}
	buf = extract(buf, (uint8_t*)&mmt_general_location_info->location_type, 1);

	if(mmt_general_location_info->location_type == MMT_GENERAL_LOCATION_INFO_LOCATION_TYPE_MMTP_PACKET_FLOW_SAME_AS_SI) {
		if(remaining_len < 2) {
			return NULL;
		}
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->packet_id, 2);
		mmt_general_location_info->packet_id = ntohs(mmt_general_location_info->packet_id);
	} else if(mmt_general_location_info->location_type == MMT_GENERAL_LOCATION_INFO_LOCATION_TYPE_MMTP_PACKET_FLOW_UDP_IP_V4) {
		if(remaining_len < 12) {
			return NULL;
		}
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->ipv4_src_addr, 4);
		mmt_general_location_info->ipv4_src_addr = ntohl(mmt_general_location_info->ipv4_src_addr);
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->ipv4_dst_addr, 4);
		mmt_general_location_info->ipv4_dst_addr = ntohl(mmt_general_location_info->ipv4_dst_addr);

		buf = extract(buf, (uint8_t*)&mmt_general_location_info->ipv4_dst_port, 2);
		mmt_general_location_info->ipv4_dst_port = ntohs(mmt_general_location_info->ipv4_dst_port);

		buf = extract(buf, (uint8_t*)&mmt_general_location_info->packet_id, 2);
		mmt_general_location_info->packet_id = ntohs(mmt_general_location_info->packet_id);

	} else if(mmt_general_location_info->location_type == 0x02) {
		//noop
	} else if(mmt_general_location_info->location_type == 0x0A) {
		if(remaining_len < 14) {
			return NULL;
		}
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->ipv4_src_addr, 4);
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->ipv4_dst_addr, 4);
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->ipv4_dst_port, 2);
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->packet_id, 2);
		buf = extract(buf, (uint8_t*)&mmt_general_location_info->message_id, 2);
	} else {
		__MMSM_WARN("MMT_SI: descriptor MMT_general_location_info for location_type: 0x%02x is not supported!", mmt_general_location_info->location_type);
	}

	return buf;
}

/*
 *
 * jjustman-2020-11-12 - TODO: fix this for proper block_t reading rather than raw-buf management
 */

uint8_t* mpt_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet) {
	if(!__mmt_signalling_message_parse_length_short(udp_packet, mmt_signalling_message_header_and_payload)) {
        __MMSM_WARN("mpt_message_parse: __mmt_signalling_message_parse_length_short failed to parse, udp_packet bytes remaining: %d",
                    block_Remaining_size(udp_packet));
        return NULL;
	}


	//we have already consumed the mpt_message, now we are processing the mp_table
	uint8_t *raw_buf = block_Get(udp_packet);
	uint16_t remaining_len = block_Remaining_size(udp_packet);
	uint8_t *buf = raw_buf;

	mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;

	//jjustman-2020-10-01 - we need at least 5 bytes to get started
	if(remaining_len < 6) {
		__MMSM_WARN("mpt_message_parse: short read for header: need 6 bytes but remaining size is: %d", remaining_len);
		goto cleanup;
	}

	//jjustman-2019-08-12 - mp_table.id: 8 bit
	uint8_t table_id;
	buf = extract(buf, &table_id, 1);
	remaining_len--;

	mp_table->table_id = table_id;

	//if message_id==20 - full message, otherwise subset n-1

	uint8_t version;
	buf = extract(buf, &version, 1);
	remaining_len--;

	mp_table->version = version;

	uint16_t length;
	buf = extract(buf,(uint8_t*)&length, 2);
	remaining_len -= 2;

	mp_table->length = ntohs(length);

	uint8_t reserved_mp_table_mode;
	buf = extract(buf, &reserved_mp_table_mode, 1);
	remaining_len--;
	if((reserved_mp_table_mode >> 2) != 0x3F) {
		__MMSM_ERROR_23008_1("mp_table RESERVED 6 bits are not set '111111' - message_id: 0x%04x, table_id: 0x%02x", mmt_signalling_message_header_and_payload->message_header.message_id, mp_table->table_id);
		//goto cleanup;
	}
	//set MP_table_mode
	mp_table->mp_table_mode = reserved_mp_table_mode & 0x3;

	if(mp_table->table_id == 0x20 || mp_table->table_id == 0x11) {
		//process packages & descriptors
		//read mmt_package_id here
		uint8_t mmt_package_id_length;
		buf = extract(buf, &mmt_package_id_length, 1);
		remaining_len--;

		//jjustman-2020-10-01 - we need at least 5 bytes to get started
		if(remaining_len < mmt_package_id_length) {
			__MMSM_WARN("mpt_message_parse: short read for mmt_package_id_length: need %d bytes but remaining size is: %d", mmt_package_id_length, remaining_len);
			goto cleanup;
		}

		mp_table->mmt_package_id.mmt_package_id_length = mmt_package_id_length;

		buf = __read_uint8_len_to_string(buf, mmt_package_id_length, &mp_table->mmt_package_id.mmt_package_id);
		remaining_len -= mmt_package_id_length;

		//jjustman-2020-10-01 - we need at least 5 bytes to get started
		if(remaining_len < 2) {
			__MMSM_WARN("mpt_message_parse: short read for table_descriptors_length: need %d bytes but remaining size is: %d", 2, remaining_len);
			goto cleanup;
		}

		uint16_t table_descriptors_length;
		buf = extract(buf, (uint8_t*)&table_descriptors_length, 2);
		remaining_len -= 2;

		mp_table->mp_table_descriptors.mp_table_descriptors_length = ntohs(table_descriptors_length);
		if(mp_table->mp_table_descriptors.mp_table_descriptors_length > 0) {
			//jjustman-2020-10-01 - we need at least 5 bytes to get started
			if(remaining_len < mp_table->mp_table_descriptors.mp_table_descriptors_length ) {
				__MMSM_WARN("mpt_message_parse: short read for mp_table->mp_table_descriptors.mp_table_descriptors_length : need %d bytes but remaining size is: %d", mp_table->mp_table_descriptors.mp_table_descriptors_length , remaining_len);
				goto cleanup;
			}

			__MMSM_DEBUG("mpt_message_parse: eading mp_table_descriptors size: %u", mp_table->mp_table_descriptors.mp_table_descriptors_length);
			mp_table->mp_table_descriptors.mp_table_descriptors_byte = calloc(mp_table->mp_table_descriptors.mp_table_descriptors_length, sizeof(uint8_t));
			buf = extract(buf, (uint8_t*)&mp_table->mp_table_descriptors.mp_table_descriptors_byte, mp_table->mp_table_descriptors.mp_table_descriptors_length);
			remaining_len -= mp_table->mp_table_descriptors.mp_table_descriptors_length;
		}
	}

	if(remaining_len <  1) {
		__MMSM_WARN("mpt_message_parse: short read for number_of_assets : need %d bytes but remaining size is: %d", 1, remaining_len);
		goto cleanup;
	}
	uint8_t number_of_assets;
	buf = extract(buf, &number_of_assets, 1);
	remaining_len--;
	number_of_assets = __CLIP(number_of_assets, 0, 255);
	mp_table->number_of_assets = number_of_assets;

	mp_table->mp_table_asset_row = calloc(number_of_assets, sizeof(mp_table_asset_row_t));
	for(int i=0; i < mp_table->number_of_assets; i++ ) {
		mp_table_asset_row_t* row = &mp_table->mp_table_asset_row[i];

		if(remaining_len <  9) {
			__MMSM_WARN("mpt_message_parse: short read for number_of_assets loop (%d assets) : need %d bytes but remaining size is: %d", mp_table->number_of_assets, 9, remaining_len);
			goto cleanup;
		}

		//grab our identifer mapping
		uint8_t identifier_type;
		buf = extract(buf, &identifier_type, 1);
		remaining_len--;

		row->identifier_mapping.identifier_type = identifier_type;
		if(row->identifier_mapping.identifier_type == 0x00) {
			uint32_t asset_id_scheme;

			buf = extract(buf, (uint8_t*)&asset_id_scheme, 4);
			row->identifier_mapping.asset_id.asset_id_scheme = ntohl(asset_id_scheme);
			remaining_len -= 4;

			uint32_t asset_id_length;
			buf = extract(buf, (uint8_t*)&asset_id_length, 4);
			remaining_len -= 4;
			row->identifier_mapping.asset_id.asset_id_length = ntohl(asset_id_length);
			if(remaining_len < row->identifier_mapping.asset_id.asset_id_length) {
				__MMSM_WARN("mpt_message_parse: short read for asset: %d, row->identifier_mapping.asset_id.asset_id_length: need %d bytes but remaining size is: %d", i,  row->identifier_mapping.asset_id.asset_id_length, remaining_len);
				goto cleanup;
			}
			buf = __read_uint32_len_to_string(buf, row->identifier_mapping.asset_id.asset_id_length, &row->identifier_mapping.asset_id.asset_id);
			remaining_len -= row->identifier_mapping.asset_id.asset_id_length;

		} else if(row->identifier_mapping.identifier_type == 0x01) {
			//build url

		}

		if(remaining_len < 5) {
			__MMSM_WARN("mpt_message_parse: short read for asset: %d, row->asset_type and default_asset_flag: need %d bytes but remaining size is: %d", i, 5, remaining_len);
			goto cleanup;
		}

		buf = extract(buf, (uint8_t*)&row->asset_type, 4);
		remaining_len -= 4;

		uint8_t reserved_default_asset_flag;
		buf = extract(buf, (uint8_t*)&reserved_default_asset_flag, 1);
		remaining_len--;

		row->default_asset_flag = (reserved_default_asset_flag >> 1) & 0x1;
		row->asset_clock_relation_flag = reserved_default_asset_flag & 0x1;
		if(row->asset_clock_relation_flag) {
			if(remaining_len < 6) {
				__MMSM_WARN("mpt_message_parse: short read for asset: %d, asset_clock_relation_flag: need %d bytes but remaining size is: %d", i, 6, remaining_len);
				goto cleanup;
			}
			buf = extract(buf, (uint8_t*)&row->asset_clock_relation_id, 1);
			remaining_len--;

			uint8_t reserved_asset_timescale_flag;
			buf = extract(buf, (uint8_t*)&reserved_asset_timescale_flag, 1);
			remaining_len--;

			row->asset_timescale_flag = reserved_asset_timescale_flag & 0x1;
			if(row->asset_timescale_flag) {
				buf = extract(buf, (uint8_t*)&row->asset_timescale, 4);
				row->asset_timescale = ntohl(row->asset_timescale);
				remaining_len -= 4;
			}
		}

		if(remaining_len < 1) {
			__MMSM_WARN("mpt_message_parse: short read for asset: %d, location_count: need %d bytes but remaining size is: %d", i, 1, remaining_len);
			goto cleanup;
		}
		buf = extract(buf, (uint8_t*)&row->location_count, 1);
		remaining_len--;


		//build out mmt_general_location_info N times.....

		//reset our remaining_leng
		uint8_t* old_buf = buf;
		buf = __read_mmt_general_location_info(old_buf, remaining_len, &row->mmt_general_location_info);

		if(buf == NULL) {
			__MMSM_WARN("mpt_message_parse: incomplete table: __read_mmt_general_location_info: asset: %d, returned NULL, had %d bytes remaining len", i, remaining_len);
			goto cleanup;
		}

		remaining_len = (remaining_len - (buf - old_buf));

		if(remaining_len < 2) {
			__MMSM_WARN("mpt_message_parse: short read for asset: %d, asset_descriptors_length: need %d bytes but remaining size is: %d", i, 2, remaining_len);
			goto cleanup;
		}


		//asset_descriptors
		uint16_t asset_descriptors_length;
		buf = extract(buf, (uint8_t*)&asset_descriptors_length, 2);
		remaining_len -= 2;

		row->asset_descriptors_length = ntohs(asset_descriptors_length);

		if(remaining_len < row->asset_descriptors_length) {
			__MMSM_WARN("mpt_message_parse: short read for asset: %d, row->asset_descriptors_length: need %d bytes but remaining size is: %d", i, row->asset_descriptors_length, remaining_len);
			goto cleanup;
		}
		buf = __read_uint16_len_to_string(buf, row->asset_descriptors_length, &row->asset_descriptors_payload);
		remaining_len -= row->asset_descriptors_length;

		//peek at
		if(row->asset_descriptors_length) {
			if(row->asset_descriptors_payload[0] == 0x00 && row->asset_descriptors_payload[1] == 0x01) {
				row->mmt_signalling_message_mpu_timestamp_descriptor = calloc(1, sizeof(mmt_signalling_message_mpu_timestamp_descriptor_t));
				row->mmt_signalling_message_mpu_timestamp_descriptor->descriptor_tag = 0x0001;
				row->mmt_signalling_message_mpu_timestamp_descriptor->descriptor_length = row->asset_descriptors_payload[2];
				row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n = row->mmt_signalling_message_mpu_timestamp_descriptor->descriptor_length / 12;

				if(row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n) {
					row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple = calloc(row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n, sizeof(mmt_signalling_message_mpu_tuple_t));
					for(int i=0; i < row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n; i++) {
						uint32_t mpu_sequence_number;
						uint64_t mpu_presentation_time;
						memcpy(&mpu_sequence_number, &row->asset_descriptors_payload[3 + (i*12)], 4);
						memcpy(&mpu_presentation_time, &row->asset_descriptors_payload[7 + (i*12)], 8);

						row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[i].mpu_sequence_number = ntohl(mpu_sequence_number);
						row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[i].mpu_presentation_time = ntohll(mpu_presentation_time);

					}
				}
			}
		}
	}

	return buf;
//failure/error/etc
cleanup:

	return NULL;
}

/*
 * jjustman-2020-09-17 - TODO: re-implement with block_t readers that are length aware for guards
 */

#define MMT_ATSC3_MESSAGE_ROUTECOMPONENT "<ROUTEComponent"
#define MMT_ATSC3_MESSAGE_STSID_URI "sTSIDUri=\""
#define MMT_ATSC3_MESSAGE_STSID_DESTINATION_IP_ADDRESS "sTSIDDestinationIpAddress=\""
#define MMT_ATSC3_MESSAGE_STSID_DESTINATION_UDP_PORT "sTSIDDestinationUdpPort=\""
#define MMT_ATSC3_MESSAGE_STSID_SOURCE_IP_ADDRESS "sTSIDSourceIpAddress=\""


uint8_t* mmt_atsc3_message_payload_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet) {
	if(!__mmt_signalling_message_parse_length_long(udp_packet, mmt_signalling_message_header_and_payload)) {
        __MMSM_WARN("mpt_message_parse: __mmt_signalling_message_parse_length_long failed to parse, udp_packet bytes remaining: %d",
                    block_Remaining_size(udp_packet));
        return NULL;
	}

	uint8_t *raw_buf = block_Get(udp_packet);
	uint32_t udp_size = block_Remaining_size(udp_packet);
	uint8_t *buf = raw_buf;

	mmt_atsc3_message_payload_t* mmt_atsc3_message_payload = &mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload;

	if((udp_size - (buf - raw_buf)) < 2) {
		return NULL;
	}
	uint16_t service_id;
	buf = extract(buf, (uint8_t*)&service_id, 2);
	mmt_atsc3_message_payload->service_id = ntohs(service_id);

	if((udp_size - (buf - raw_buf)) < 2) {
		return NULL;
	}
	uint16_t atsc3_message_content_type;
	buf = extract(buf, (uint8_t*)&atsc3_message_content_type, 2);
	mmt_atsc3_message_payload->atsc3_message_content_type = ntohs(atsc3_message_content_type);

	if((udp_size - (buf - raw_buf)) < 1) {
		return NULL;
	}
	uint8_t atsc3_message_content_version;
	buf = extract(buf, (uint8_t*)&atsc3_message_content_version, 1);
	mmt_atsc3_message_payload->atsc3_message_content_version = atsc3_message_content_version;

	if((udp_size - (buf - raw_buf)) < 1) {
		return NULL;
	}
	uint8_t atsc3_message_content_compression;
	buf = extract(buf, (uint8_t*)&atsc3_message_content_compression, 1);
	mmt_atsc3_message_payload->atsc3_message_content_compression = atsc3_message_content_compression;

	uint8_t 	URI_length;
	buf = extract(buf, (uint8_t*)&URI_length, 1);
	mmt_atsc3_message_payload->URI_length = __MAX(0, __MIN(255, URI_length));

	if(URI_length) {
		if((udp_size - (buf - raw_buf)) < URI_length) {
			return NULL;
		}
		buf = __read_uint8_len_to_string(buf, URI_length, &mmt_atsc3_message_payload->URI_payload);
	}

	if((udp_size - (buf - raw_buf)) < 4) {
		return NULL;
	}
	uint32_t temp_atsc3_message_content_length;
	buf = extract(buf, (uint8_t*)&temp_atsc3_message_content_length, 4);
	temp_atsc3_message_content_length = ntohl(temp_atsc3_message_content_length);

	if(temp_atsc3_message_content_length) {
		//cheat and over-alloc+1 for a null byte

		uint8_t *temp_atsc3_message_content = NULL;
		if((udp_size - (buf - raw_buf)) < temp_atsc3_message_content_length) {
			return NULL;
		}
		buf = __read_uint32_len_to_string(buf, temp_atsc3_message_content_length, &temp_atsc3_message_content);

		if(mmt_atsc3_message_payload->atsc3_message_content_compression == 0x02) {
			mmt_atsc3_message_payload->atsc3_message_content_length_compressed = temp_atsc3_message_content_length;
			mmt_atsc3_message_payload->atsc3_message_content_compressed = temp_atsc3_message_content;

			//ungzip
			uint8_t *decompressed_payload;
			int32_t ret = atsc3_unzip_gzip_payload(mmt_atsc3_message_payload->atsc3_message_content_compressed, mmt_atsc3_message_payload->atsc3_message_content_length_compressed, &decompressed_payload);

			if(ret > 0) {
				mmt_atsc3_message_payload->atsc3_message_content_length = ret;
				mmt_atsc3_message_payload->atsc3_message_content = calloc(ret + 1, sizeof(char));
				memcpy(mmt_atsc3_message_payload->atsc3_message_content, decompressed_payload, ret);
				free(decompressed_payload);
				decompressed_payload = NULL;
			} else {
				__MMSM_ERROR("atsc3_message_content_compressed, unable to decompress: error is: %u", ret);
			}

		} else {
			//treat this as uncompressed for now..
			mmt_atsc3_message_payload->atsc3_message_content_length = temp_atsc3_message_content_length;
			mmt_atsc3_message_payload->atsc3_message_content = temp_atsc3_message_content;
		}
	}


	mmt_atsc3_message_payload->atsc3_message_content_blockt = block_Duplicate_from_ptr(mmt_atsc3_message_payload->atsc3_message_content, mmt_atsc3_message_payload->atsc3_message_content_length);
	block_Rewind(mmt_atsc3_message_payload->atsc3_message_content_blockt);

    //jjustman-2021-04-27 - TODO: process thse message_content_types as callback interface against impl dispatch table
	if(mmt_atsc3_message_payload->atsc3_message_content) {
	    //parse internal mmt_atsc3 message


	    switch(mmt_atsc3_message_payload->atsc3_message_content_type) {

	        // atsc3_message_content_type: 0x0001
	        case MMT_ATSC3_MESSAGE_CONTENT_TYPE_UserServiceDescription:
	            {

                    bool has_open_routecomponent_tag = false;
                    uint8_t *stsid_uri = NULL;
                    uint8_t *stsid_destination_ip_address = NULL;
                    uint16_t stsid_destination_udp_port = 0;
                    uint8_t *stsid_source_ip_address = NULL;

                    //extract out <ROUTEComponent sTSIDUri="stsid.sls" sTSIDDestinationIpAddress="239.255.70.1" sTSIDDestinationUdpPort="5009" sTSIDSourceIpAddress="172.16.200.1"></ROUTEComponent>
                    //jjustman-2020-12-08 - TODO: use preg2 to handle this...
                    //25 is the longest match for stsidDestinationIpAddress with all octets
                    for (int i = 0; i < mmt_atsc3_message_payload->atsc3_message_content_length - 44; i++) {
                        uint8_t *atsc3_message = &mmt_atsc3_message_payload->atsc3_message_content[i];

                        if (!strncasecmp(MMT_ATSC3_MESSAGE_ROUTECOMPONENT, (const char *) atsc3_message, strlen(MMT_ATSC3_MESSAGE_ROUTECOMPONENT))) {
                            //we have our opening ROUTE tag, now process interior elements
                            has_open_routecomponent_tag = true;
                        } else if (has_open_routecomponent_tag) {
                            if (!strncasecmp(MMT_ATSC3_MESSAGE_STSID_URI, (const char *) atsc3_message, strlen(MMT_ATSC3_MESSAGE_STSID_URI))) {
                                atsc3_message += strlen(MMT_ATSC3_MESSAGE_STSID_URI);
                                char *end = strstr((const char *) atsc3_message, "\"");
                                if (end) {
                                    int len = end - (char *) atsc3_message;
                                    stsid_uri = calloc(len + 1, sizeof(uint8_t));
                                    memcpy(stsid_uri, atsc3_message, len);
                                }
                            } else if (!strncasecmp(MMT_ATSC3_MESSAGE_STSID_DESTINATION_IP_ADDRESS, (const char *) atsc3_message, strlen(MMT_ATSC3_MESSAGE_STSID_DESTINATION_IP_ADDRESS))) {
                                atsc3_message += strlen(MMT_ATSC3_MESSAGE_STSID_DESTINATION_IP_ADDRESS);
                                char *end = strstr((const char *) atsc3_message, "\"");
                                if (end) {
                                    int len = end - (char *) atsc3_message;
                                    stsid_destination_ip_address = calloc(len + 1, sizeof(uint8_t));
                                    memcpy(stsid_destination_ip_address, atsc3_message, len);
                                }
                            }
                            if (!strncasecmp(MMT_ATSC3_MESSAGE_STSID_DESTINATION_UDP_PORT, (const char *) atsc3_message, strlen(MMT_ATSC3_MESSAGE_STSID_DESTINATION_UDP_PORT))) {
                                atsc3_message += strlen(MMT_ATSC3_MESSAGE_STSID_DESTINATION_UDP_PORT);
                                char *end = strstr((const char *) atsc3_message, "\"");
                                if (end) {
                                    int len = end - (char *) atsc3_message;
                                    uint8_t *stsid_port_s = calloc(len + 1, sizeof(uint8_t));
                                    memcpy(stsid_port_s, atsc3_message, len);
                                    stsid_destination_udp_port = atoi((const char *) stsid_port_s);
                                    free(stsid_port_s);
                                }
                            }
                            if (!strncasecmp(MMT_ATSC3_MESSAGE_STSID_SOURCE_IP_ADDRESS, (const char *) atsc3_message, strlen(MMT_ATSC3_MESSAGE_STSID_SOURCE_IP_ADDRESS))) {
                                atsc3_message += strlen(MMT_ATSC3_MESSAGE_STSID_SOURCE_IP_ADDRESS);
                                char *end = strstr((const char *) atsc3_message, "\"");
                                if (end) {
                                    int len = end - (char *) atsc3_message;

                                    stsid_source_ip_address = calloc(len + 1, sizeof(uint8_t));
                                    memcpy(stsid_source_ip_address, atsc3_message, len);
                                }
                            }
                        }
                    }

                    if (stsid_uri && stsid_destination_ip_address && stsid_destination_udp_port) {
                        mmt_atsc3_route_component_t *mmt_atsc3_route_component = mmt_atsc3_message_payload_add_mmt_atsc3_route_component(mmt_atsc3_message_payload);
                        if (mmt_atsc3_route_component) {
                            mmt_atsc3_route_component->stsid_uri_s = stsid_uri;
                            mmt_atsc3_route_component->stsid_destination_ip_address_s = stsid_destination_ip_address;
                            mmt_atsc3_route_component->stsid_destination_ip_address = parseIpAddressIntoIntval((const char *) stsid_destination_ip_address);
                            mmt_atsc3_route_component->stsid_destination_udp_port = stsid_destination_udp_port;
                            if (stsid_source_ip_address) {
                                mmt_atsc3_route_component->stsid_source_ip_address_s = stsid_source_ip_address;
                                mmt_atsc3_route_component->stsid_source_ip_address = parseIpAddressIntoIntval((const char *) stsid_source_ip_address);
                            }
                        }
                    } else {
                        if (stsid_uri) {
                            freeclean((void **) &stsid_uri);
                        }
                        if (stsid_destination_ip_address) {
                            freeclean((void **) &stsid_destination_ip_address);
                        }
                        if (stsid_source_ip_address) {
                            freeclean((void **) &stsid_source_ip_address);
                        }
                    }
	            }
                break;

	        // atsc3_message_content_type: 0x0002
            case MMT_ATSC3_MESSAGE_CONTENT_TYPE_MPD_FROM_DASHIF:
                {
                    __MMSM_INFO("mmt_atsc3_message_payload_parse, ignornig mmt_atsc3 message type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_MPD_FROM_DASHIF (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);
                }
                break;

            // atsc3_message_content_type: 0x0003
	        case MMT_ATSC3_MESSAGE_CONTENT_TYPE_HELD:
	            {
                    __MMSM_DEBUG("mmt_atsc3_message_payload_parse, processing mmt_atsc3_message_type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_HELD (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);

                    mmt_atsc3_held_message_t* mmt_atsc3_held_message = mmt_atsc3_message_payload_add_mmt_atsc3_held_message(mmt_atsc3_message_payload);
                    if (mmt_atsc3_held_message) {
                        mmt_atsc3_held_message->held_message = block_Duplicate_from_ptr(mmt_atsc3_message_payload->atsc3_message_content, mmt_atsc3_message_payload->atsc3_message_content_length);
                    }
                }
                break;

            // atsc3_message_content_type: 0x0004
            case MMT_ATSC3_MESSAGE_CONTENT_TYPE_APPLICATION_EVENT_INFORMATION_A337:
                {
                    __MMSM_INFO("mmt_atsc3_message_payload_parse, ignornig mmt_atsc3 message type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_APPLICATION_EVENT_INFORMATION_A337 (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);
                }
                break;

            // atsc3_message_content_type: 0x0005
            case MMT_ATSC3_MESSAGE_CONTENT_TYPE_VIDEO_STREAM_PROPERTIES_DESCRIPTOR:
                {
                    __MMSM_INFO("mmt_atsc3_message_payload_parse, ignornig mmt_atsc3 message type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_VIDEO_STREAM_PROPERTIES_DESCRIPTOR (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);
                }
                break;

            // atsc3_message_content_type: 0x0006
            case MMT_ATSC3_MESSAGE_CONTENT_TYPE_ATSC_STAGGERCAST_DESCRIPTOR:
                {
                    __MMSM_INFO("mmt_atsc3_message_payload_parse, ignornig mmt_atsc3 message type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_ATSC_STAGGERCAST_DESCRIPTOR (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);
                }
                break;

            // atsc3_message_content_type: 0x0007
            case MMT_ATSC3_MESSAGE_CONTENT_TYPE_INBAND_EVENT_DESCRIPTOR_A337:
                {
                    __MMSM_INFO("mmt_atsc3_message_payload_parse, ignornig mmt_atsc3 message type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_INBAND_EVENT_DESCRIPTOR_A337 (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);
                }
                break;

            // atsc3_message_content_type: 0x0008
            case MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR:
                {
					block_t* src = mmt_atsc3_message_payload->atsc3_message_content_blockt;
                    __MMSM_INFO("mmt_atsc3_message_payload_parse, parsing mmt_atsc3 message type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);
					
					if(_MMT_SIGNALLING_MESSAGE_TRACE_ENABLED) {
						for(int i=0; i < src->p_size; i++) {
							printf("0x%02x ", src->p_buffer[i]);
						}
					}
					
					mmt_atsc3_message_content_type_caption_asset_descriptor_t* mmt_atsc3_message_content_type_caption_asset_descriptor = mmt_atsc3_message_content_type_caption_asset_descriptor_new();
										
					mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.descriptor_tag = block_Read_uint16_ntohs(src);
					mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.descriptor_length = block_Read_uint16_ntohs(src);
					mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.number_of_assets = block_Read_uint8(src);

					//for sanity check
					if(mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.descriptor_tag != MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR) {
						__MMSM_ERROR("MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR: descriptor_tag mismatch: mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.descriptor_tag (0x%04x) != MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR (0x%04x)",  mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.descriptor_tag, MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR);
					} else if(!mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.number_of_assets) {
						__MMSM_ERROR("MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR: number_of_assets is 0!");
					} else {
						
						for(int i = 0; i < mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.number_of_assets; i++) {
							mmt_atsc3_message_content_type_caption_asset_descriptor_asset_t* mmt_atsc3_message_content_type_caption_asset_descriptor_asset = mmt_atsc3_message_content_type_caption_asset_descriptor_asset_new();
							__MMSM_TRACE("MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR: starting to parse asset: %d, %p", i, mmt_atsc3_message_content_type_caption_asset_descriptor_asset);
							mmt_atsc3_message_content_type_caption_asset_descriptor_asset->asset_header.asset_id_length = block_Read_uint32_ntohl(src);
							mmt_atsc3_message_content_type_caption_asset_descriptor_asset->asset_header.asset_id = block_Read_uint8_varlen(src, mmt_atsc3_message_content_type_caption_asset_descriptor_asset->asset_header.asset_id_length);

							mmt_atsc3_message_content_type_caption_asset_descriptor_asset->language_header.language_length = block_Read_uint8(src);
							mmt_atsc3_message_content_type_caption_asset_descriptor_asset->language_header.language = block_Read_uint8_varlen(src, mmt_atsc3_message_content_type_caption_asset_descriptor_asset->language_header.language_length);

							mmt_atsc3_message_content_type_caption_asset_descriptor_asset->role = block_Read_uint8_varlen(src, 4);
							mmt_atsc3_message_content_type_caption_asset_descriptor_asset->aspect_ratio = block_Read_uint8_varlen(src, 4);
							mmt_atsc3_message_content_type_caption_asset_descriptor_asset->easy_reader = block_Read_uint8_varlen(src, 1);
							mmt_atsc3_message_content_type_caption_asset_descriptor_asset->profile = block_Read_uint8_varlen(src, 2);
							mmt_atsc3_message_content_type_caption_asset_descriptor_asset->flag_3d_support = block_Read_uint8_varlen(src, 1);
							
							mmt_atsc3_message_content_type_caption_asset_descriptor_asset->reserved = block_Read_uint8_varlen(src, 4);
							
							mmt_atsc3_message_content_type_caption_asset_descriptor_add_mmt_atsc3_message_content_type_caption_asset_descriptor_asset(mmt_atsc3_message_content_type_caption_asset_descriptor, mmt_atsc3_message_content_type_caption_asset_descriptor_asset);
						}
						
						//jjustman-2021-07-07 - A/331:2021 - 19 Jan 21 is ambigious about the following reserved padding, assuming N == number_of_assets
						for(int i=0; i < mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.number_of_assets; i++) {
							//read 8 bits and discard
							block_Read_uint8(src);
						}
					}
					   
					mmt_atsc3_message_payload->mmt_atsc3_message_content_type_caption_asset_descriptor = mmt_atsc3_message_content_type_caption_asset_descriptor;
                }

                break;

            // atsc3_message_content_type: 0x0009
            case MMT_ATSC3_MESSAGE_CONTENT_TYPE_AUDIO_STREAM_PROPERTIES_DESCRIPTOR:
                {
                    __MMSM_INFO("mmt_atsc3_message_payload_parse, ignornig mmt_atsc3 message type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_AUDIO_STREAM_PROPERTIES_DESCRIPTOR (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);
                }

                break;

            // atsc3_message_content_type: 0x000A
            case MMT_ATSC3_MESSAGE_CONTENT_TYPE_DWD:
                {
                    __MMSM_INFO("mmt_atsc3_message_payload_parse, ignornig mmt_atsc3 message type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_DWD (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);
                }

                break;

            // atsc3_message_content_type: 0x000B
            case MMT_ATSC3_MESSAGE_CONTENT_TYPE_RSAT_A200:
                {
                    __MMSM_INFO("mmt_atsc3_message_payload_parse, ignornig mmt_atsc3 message type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_RSAT_A200 (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);
                }

                break;
            // atsc3_message_content_type: 0x000C
            case MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR:
                {
                    __MMSM_INFO("mmt_atsc3_message_payload_parse, ignornig mmt_atsc3 message type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);
                }
                break;

			// atsc3_message_content_type: 0x000D
			case MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL:
				{
					block_t* src = mmt_atsc3_message_payload->atsc3_message_content_blockt;
					__MMSM_INFO("mmt_atsc3_message_payload_parse, parsing mmt_atsc3 message type: MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL (0x%04x)", mmt_atsc3_message_payload->atsc3_message_content_type);
					if(_MMT_SIGNALLING_MESSAGE_TRACE_ENABLED) {
					    for(int i=0; i < src->p_size; i++) {
					        printf("0x%02x ", src->p_buffer[i]);
					    }
					}

					mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_t* mmt_atsc3_message_content_type_security_properties_descriptor_LAURL = mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_new();

					mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.descriptor_tag = block_Read_uint16_ntohs(src);
					mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.descriptor_length = block_Read_uint16_ntohs(src);
					mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.number_of_assets = block_Read_uint8(src);

					//for sanity check
					if(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.descriptor_tag != MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL) {
						__MMSM_ERROR("MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL: descriptor_tag mismatch: mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.descriptor_tag (0x%04x) != MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL (0x%04x)",  mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.descriptor_tag, MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL);
					} else if(!mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.number_of_assets) {
						__MMSM_ERROR("MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL: number_of_assets is 0!");
					} else {
						//start parsing
						for(int i = 0; i < mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.number_of_assets; i++) {
							mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_t* mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset = mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_new();
							__MMSM_TRACE("MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL: starting to parse asset: %d, %p", i, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset);
							mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->asset_header.asset_id_length = block_Read_uint32_ntohl(src);
							mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->asset_header.asset_id = block_Read_uint8_varlen(src, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->asset_header.asset_id_length);

							mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->scheme_code_present = block_Read_uint8_bitlen(src, 1);
							mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->default_KID_present = block_Read_uint8_bitlen(src, 1);
							mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->license_info_present = block_Read_uint8_bitlen(src, 1);
							mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->reserved_5_0 = block_Read_uint8_bitlen(src, 5);

							if(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->scheme_code_present) {
								for(int j=0; j < 4; j++) {
									mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->scheme_code[i] = block_Read_uint8(src);
								}
							}

							if(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->default_KID_present) {
								mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->default_KID_length = block_Read_uint8(src);
								mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->default_KID = block_Read_uint8_varlen(src, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->default_KID_length);

							}

							if(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->license_info_present) {
								mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->number_of_license_info = block_Read_uint8(src);
								for(int j=0; j < mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->number_of_license_info; j++) {
									mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info_t* mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info = mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info_new();
									mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info->license_type = block_Read_uint8(src);
									mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info->LA_URL_length = block_Read_uint8(src);
									mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info->LA_URL = block_Read_uint8_varlen(src, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info->LA_URL_length);

									mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_add_mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info);
								}
							}


							//parse mmt_si_security_properties_descriptor_t
							//jjustman-2021-06-08 - TODO: refactor me out to a first class mmt_si_security_properties_descriptor by ref as below...
							mmt_si_security_properties_descriptor_t* mmt_si_security_properties_descriptor = &mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->mmt_si_security_properties_descriptor;
							mmt_si_security_properties_descriptor->descriptor_header.descriptor_tag = block_Read_uint16_ntohs(src);
							mmt_si_security_properties_descriptor->descriptor_header.descriptor_length = block_Read_uint16_ntohs(src);
							mmt_si_security_properties_descriptor->security_system_count = block_Read_uint8(src);
							mmt_si_security_properties_descriptor->reserved_7_0 = block_Read_uint8_bitlen(src, 7);
							mmt_si_security_properties_descriptor->system_provider_url_flag = block_Read_uint8_bitlen(src, 1);

							if(mmt_si_security_properties_descriptor->system_provider_url_flag) {
								mmt_si_security_properties_descriptor->system_provider_url_length = block_Read_uint8(src);
								mmt_si_security_properties_descriptor->system_provider_url = block_Read_uint8_varlen(src, mmt_si_security_properties_descriptor->system_provider_url_length);

							}

							__MMSM_TRACE("mmt_si_security_properties_descriptor->security_system_count: %d", mmt_si_security_properties_descriptor->security_system_count);

							for(int N1=0; N1 < mmt_si_security_properties_descriptor->security_system_count; N1++) {
								mmt_si_security_properties_descriptor_system_t* mmt_si_security_properties_descriptor_system = mmt_si_security_properties_descriptor_system_new();
								__MMSM_TRACE("mmt_si_security_properties_descriptor_t: starting to parse mmt_si_security_properties_descriptor_system: %d, ptr: %p", N1, mmt_si_security_properties_descriptor_system);
								//meh
								for(int j=0; j < 16; j++) {
									mmt_si_security_properties_descriptor_system->system_id[j] = block_Read_uint8(src);
								}

								mmt_si_security_properties_descriptor_system->kid_count = block_Read_uint16_ntohs(src);

								for(int N3=0; N3 < mmt_si_security_properties_descriptor_system->kid_count; N3++) {
									mmt_si_security_properties_descriptor_kid_t* mmt_si_security_properties_descriptor_kid = mmt_si_security_properties_descriptor_kid_new();
									for(int j=0; j < 16; j++) {
										mmt_si_security_properties_descriptor_kid->kid[j] = block_Read_uint8(src);
									}
									mmt_si_security_properties_descriptor_system_add_mmt_si_security_properties_descriptor_kid(mmt_si_security_properties_descriptor_system, mmt_si_security_properties_descriptor_kid);
								}

								mmt_si_security_properties_descriptor_system->data_size = block_Read_uint32_ntohl(src);
								mmt_si_security_properties_descriptor_system->data = block_Read_uint8_varlen(src, mmt_si_security_properties_descriptor_system->data_size);

								mmt_si_security_properties_descriptor_add_mmt_si_security_properties_descriptor_system(mmt_si_security_properties_descriptor, mmt_si_security_properties_descriptor_system);
							}

							mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_add_mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset);
						}

						mmt_atsc3_message_payload->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL = mmt_atsc3_message_content_type_security_properties_descriptor_LAURL;
					}


				} //end of MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL
				break;

            default:
                {
                    bool is_reserved_mmt_atsc3_message_type = (mmt_atsc3_message_payload->atsc3_message_content_type >= MMT_ATSC3_MESSAGE_CONTENT_TYPE_RESERVED) ? true : false;

                    __MMSM_INFO("mmt_atsc3_message_payload_parse, ignornig mmt_atsc3 message type: 0x%04x (is reserved type: %d)", mmt_atsc3_message_payload->atsc3_message_content_type, is_reserved_mmt_atsc3_message_type);
                }
        }
	}

	return buf;
}

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmt_scte35_message_payload, mmt_scte35_signal_descriptor)
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(mmt_scte35_signal_descriptor);

uint8_t* mmt_scte35_message_payload_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet) {

	__mmt_signalling_message_parse_length_long(udp_packet, mmt_signalling_message_header_and_payload);

	uint32_t udp_raw_buf_size = block_Remaining_size(udp_packet);
	uint8_t *raw_buf = block_Get(udp_packet);
	uint8_t *buf = raw_buf;

	mmt_scte35_message_payload_t* mmt_scte35_message_payload = &mmt_signalling_message_header_and_payload->message_payload.mmt_scte35_message_payload;

	//walk thru each signal descriptor
	uint8_t scte35_signal_descriptor_n;
	buf = extract(buf, (uint8_t*)&scte35_signal_descriptor_n, 1);
	for(int i=0; i < scte35_signal_descriptor_n && (udp_raw_buf_size > (buf-raw_buf)); i++) {
		//make sure we have at least 19 bytes available (16+16+64+7+33+16)
		if(19 < udp_raw_buf_size - (buf-raw_buf)) {
			//llvm printf can't decide if this is %llu or %lu based upon armv7/armv8, so just force it to %u
			__MMSM_WARN("mmt_scte35_message_payload_parse: short read for descriptor: %u, need 19 but remaining is: %u", i, (uint32_t)((udp_raw_buf_size - (buf-raw_buf))));
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
		mmt_scte35_signal_descriptor->pts_timestamp |= (((uint64_t)pts_timestamp_block[0] & 0x1UL) << 33);
		mmt_scte35_signal_descriptor->pts_timestamp |= ntohl(*(uint32_t*)(&pts_timestamp_block[1]));

		buf = extract(buf, (uint8_t*)&mmt_scte35_signal_descriptor->signal_length, 2);

		if(mmt_scte35_signal_descriptor->signal_length > udp_raw_buf_size - (buf-raw_buf)) {
			//llvm printf can't decide if this is %llu or %lu based upon armv7/armv8, so just force it to %u
			__MMSM_WARN("mmt_scte35_message_payload_parse: signal length for descriptor: %u, need %u but remaining is: %u", i, mmt_scte35_signal_descriptor->signal_length, (uint32_t)((udp_raw_buf_size - (buf-raw_buf))));
			goto parse_incomplete;
		}

		buf = extract(buf, (uint8_t*)&mmt_scte35_signal_descriptor->signal_byte, mmt_scte35_signal_descriptor->signal_length);
		mmt_scte35_message_payload_add_mmt_scte35_signal_descriptor(&mmt_signalling_message_header_and_payload->message_payload.mmt_scte35_message_payload, mmt_scte35_signal_descriptor);
		__MMSM_INFO("mmt_scte35_message_payload_parse: adding signal at NTP_timestamp: %" PRIu64 ", PTS: %" PRIu64, mmt_scte35_signal_descriptor->ntp_timestamp, mmt_scte35_signal_descriptor->pts_timestamp);
	}

	parse_incomplete:

	return buf;
}


void mmt_atsc3_message_payload_dump(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {

	mmt_atsc3_message_payload_t* mmt_atsc3_message_payload = &mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload;

	__MMSM_DEBUG("-----------------");
	__MMSM_DEBUG("mmt_atsc3_message");
	__MMSM_DEBUG("-----------------");
	__MMSM_DEBUG("service_id:                        %u", mmt_atsc3_message_payload->service_id);
	__MMSM_DEBUG("atsc3_message_content_type:        %u", mmt_atsc3_message_payload->atsc3_message_content_type);
	__MMSM_DEBUG("atsc3_message_content_version:     %u", mmt_atsc3_message_payload->atsc3_message_content_version);
	__MMSM_DEBUG("atsc3_message_content_compression: %u", mmt_atsc3_message_payload->atsc3_message_content_compression);
	__MMSM_DEBUG("URI_length:                        %u", mmt_atsc3_message_payload->URI_length);
	__MMSM_DEBUG("URI_payload:                       %s", mmt_atsc3_message_payload->URI_payload);
	if(mmt_atsc3_message_payload->atsc3_message_content_compression == 0x02) {
		__MMSM_DEBUG("atsc3_message_content_length_compressed:      %u", mmt_atsc3_message_payload->atsc3_message_content_length_compressed);
	}
	__MMSM_DEBUG("atsc3_message_content_length:      %u", mmt_atsc3_message_payload->atsc3_message_content_length);
	__MMSM_DEBUG("atsc3_message_content:             %s", mmt_atsc3_message_payload->atsc3_message_content);

	if(mmt_atsc3_message_payload->mmt_atsc3_route_component) {
        __MMSM_DEBUG("mmt_atsc3_route_component: stsid url: %s, stsid_destination_ip_addr_s: %s (%d), stsid_destination_udp_port: %d, stsid_source_ip_address: %s (%d)",
                     mmt_atsc3_message_payload->mmt_atsc3_route_component->stsid_uri_s,
                     mmt_atsc3_message_payload->mmt_atsc3_route_component->stsid_destination_ip_address_s,
                     mmt_atsc3_message_payload->mmt_atsc3_route_component->stsid_destination_ip_address,
                     mmt_atsc3_message_payload->mmt_atsc3_route_component->stsid_destination_udp_port,
                     mmt_atsc3_message_payload->mmt_atsc3_route_component->stsid_source_ip_address_s,
                     mmt_atsc3_message_payload->mmt_atsc3_route_component->stsid_source_ip_address);
    }
						
    //MMT_ATSC3_MESSAGE_CONTENT_TYPE_HELD
	if(mmt_atsc3_message_payload->mmt_atsc3_held_message) {
        __MMSM_DEBUG("mmt_atsc3_held_message: HELD message:\n%s", mmt_atsc3_message_payload->mmt_atsc3_held_message->held_message->p_buffer);
	}
						
	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR
	if(mmt_atsc3_message_payload->mmt_atsc3_message_content_type_caption_asset_descriptor) {
	 __MMSM_DEBUG("mmt_atsc3_message_payload: mmt_atsc3_message_content_type_caption_asset_descriptor: %p, header: tag: 0x%04x, len: %d, number_of_assets: %d",
			 mmt_atsc3_message_payload->mmt_atsc3_message_content_type_caption_asset_descriptor,
			 mmt_atsc3_message_payload->mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.descriptor_tag,
			 mmt_atsc3_message_payload->mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.descriptor_length,
			 mmt_atsc3_message_payload->mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.number_of_assets);

		 for(int i=0; i < mmt_atsc3_message_payload->mmt_atsc3_message_content_type_caption_asset_descriptor->descriptor_header.number_of_assets; i++) {
			 mmt_atsc3_message_content_type_caption_asset_descriptor_asset_t* mmt_atsc3_message_content_type_caption_asset_descriptor_asset = mmt_atsc3_message_payload->mmt_atsc3_message_content_type_caption_asset_descriptor->mmt_atsc3_message_content_type_caption_asset_descriptor_asset_v.data[i];
			 __MMSM_DEBUG(" asset idx: %d, asset len: %d, assset: %s", i, mmt_atsc3_message_content_type_caption_asset_descriptor_asset->asset_header.asset_id_length,
						  mmt_atsc3_message_content_type_caption_asset_descriptor_asset->asset_header.asset_id_length > 0 ? (char*)mmt_atsc3_message_content_type_caption_asset_descriptor_asset->asset_header.asset_id : "(null)");

			 __MMSM_DEBUG(" asset idx: %d, language_length: %d, language: %s", i, mmt_atsc3_message_content_type_caption_asset_descriptor_asset->language_header.language_length,
						  mmt_atsc3_message_content_type_caption_asset_descriptor_asset->language_header.language_length > 0 ? (char*)mmt_atsc3_message_content_type_caption_asset_descriptor_asset->language_header.language : "(null)");

			 
			 __MMSM_DEBUG(" 	role: %d, aspect_ratio: %d, easy_reader: %d, profile: %d, flag_3d_support: %d",
						  mmt_atsc3_message_content_type_caption_asset_descriptor_asset->role,
						  mmt_atsc3_message_content_type_caption_asset_descriptor_asset->aspect_ratio,
						  mmt_atsc3_message_content_type_caption_asset_descriptor_asset->easy_reader,
						  mmt_atsc3_message_content_type_caption_asset_descriptor_asset->profile,
						  mmt_atsc3_message_content_type_caption_asset_descriptor_asset->flag_3d_support);
		 }
	}

	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL
	if(mmt_atsc3_message_payload->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL) {
		__MMSM_DEBUG("mmt_atsc3_message_payload: mmt_atsc3_message_content_type_security_properties_descriptor_LAURL: %p, header: tag: 0x%04x, len: %d, number_of_assets: %d",
				mmt_atsc3_message_payload->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL,
				mmt_atsc3_message_payload->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.descriptor_tag,
				mmt_atsc3_message_payload->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.descriptor_length,
				mmt_atsc3_message_payload->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->descriptor_header.number_of_assets);

		for(int i=0; i < mmt_atsc3_message_payload->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_v.count; i++) {
			mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_t* mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset = mmt_atsc3_message_payload->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_v.data[i];
			__MMSM_DEBUG(" asset idx: %d, asset len: %d, assset: %s", i, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->asset_header.asset_id_length,
					mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->asset_header.asset_id_length > 0 ? (char*)mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->asset_header.asset_id : "(null)");

			__MMSM_DEBUG(" 	scheme_code_present: %d, default_KID_present: %d, license_info_present: %d",
					mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->scheme_code_present,
					mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->default_KID_present,
					mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->license_info_present);

			if(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->scheme_code_present) {
				__MMSM_DEBUG(" 	scheme_code: %s", mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->scheme_code);
			}

			if(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->default_KID_present) {
				__MMSM_DEBUG(" 	default_KID_length: %d, default_KID:",
						mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->default_KID_length);
				if(_MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED) {
					printf(" ");
					for(int hex=0; hex < mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->default_KID_length ; hex++) {
						printf("%02x", mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->default_KID[hex]);
					}
					printf("\n");
				}

			}

			if(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->license_info_present) {
				__MMSM_DEBUG(" 	num_of_license_info: %d", mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->number_of_license_info);

				for(int j=0; j < mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->number_of_license_info; j++) {
					mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info_t* mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info = mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info_v.data[j];
					__MMSM_DEBUG("    idx: %d, ptr: %p, license_type: %d, LA_URL_length: %d, LA_URL:\n%s",
							j, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info,
							mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info->license_type,
							mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info->LA_URL_length,
							mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info->LA_URL_length > 0 ? (char*)mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info->LA_URL : "(null)");


				}
			}

			mmt_si_security_properties_descriptor_t* mmt_si_security_properties_descriptor = &mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->mmt_si_security_properties_descriptor;
			__MMSM_DEBUG("    mm_si_security_properties_descriptor header: tag: 0x%04x, len: %d, security_system_count: %d, system_provider_url_flag: %d",
					mmt_si_security_properties_descriptor->descriptor_header.descriptor_tag,
					mmt_si_security_properties_descriptor->descriptor_header.descriptor_length,
					mmt_si_security_properties_descriptor->security_system_count,
					mmt_si_security_properties_descriptor->system_provider_url_flag);

			if(mmt_si_security_properties_descriptor->system_provider_url_flag) {
				__MMSM_DEBUG(" 	system_provider_url_flag_length: %d, system_provider_url:\n%s",
						mmt_si_security_properties_descriptor->system_provider_url_flag,
						mmt_si_security_properties_descriptor->system_provider_url_flag > 0 ? (char*)mmt_si_security_properties_descriptor->system_provider_url : "(null)");

			}

			for(int j=0; j < mmt_si_security_properties_descriptor->security_system_count; j++) {
				mmt_si_security_properties_descriptor_system_t* mmt_si_security_properties_descriptor_system = mmt_si_security_properties_descriptor->mmt_si_security_properties_descriptor_system_v.data[j];
				__MMSM_DEBUG("    mmt_si_security_properties_descriptor_system: idx: %d, ptr: %p, kid_count: %d, system_id:",
						j, mmt_si_security_properties_descriptor_system,
						mmt_si_security_properties_descriptor_system->kid_count);

                if(_MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED) {
                    printf(" ");
                    for (int hex = 0; hex < 16; hex++) {
                        printf("%02x", mmt_si_security_properties_descriptor_system->system_id[hex]);
                    }

                    printf("\n");
                }

				for(int k=0; k < mmt_si_security_properties_descriptor_system->kid_count; k++) {

					mmt_si_security_properties_descriptor_kid_t* mmt_si_security_properties_descriptor_kid = mmt_si_security_properties_descriptor_system->mmt_si_security_properties_descriptor_kid_v.data[k];
					__MMSM_DEBUG("         kid: idx: %d, ptr: %p, key:", k, mmt_si_security_properties_descriptor_kid);

					if(_MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED) {
						printf(" ");
						for(int hex=0; hex < 16; hex++) {
							printf("%02x", mmt_si_security_properties_descriptor_kid->kid[hex]);
						}
						printf("\n");
					}

				}



				__MMSM_DEBUG("         data_size: %d, data:", mmt_si_security_properties_descriptor_system->data_size);

				if(_MMT_SIGNALLING_MESSAGE_TRACE_ENABLED) {
					printf(" ");
					for(int hex=0; hex < mmt_si_security_properties_descriptor_system->data_size; hex++) {
						printf("%02x", mmt_si_security_properties_descriptor_system->data[hex]);
					}
					printf("\n");
				}

			}


		}

	}

}


uint8_t* si_message_not_supported(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet) {
	if(mmt_signalling_message_header_and_payload->message_header.message_id == 0x0204 || mmt_signalling_message_header_and_payload->message_header.message_id == 0x020A) {
		//hrmb messages
		__MMSM_TRACE("signalling information message id not supported (hrbm): 0x%04x", mmt_signalling_message_header_and_payload->message_header.message_id);

	} else {
		__MMSM_WARN("signalling information message id not supported (other SI): 0x%04x", mmt_signalling_message_header_and_payload->message_header.message_id);
	}
	return NULL;
}

/*
 * jjustman-2020-12-01 - TODO - fix me to remove single *_packet_id references
 */

bool mmt_signalling_message_update_lls_sls_mmt_session(mmtp_signalling_packet_t* mmtp_signalling_packet, lls_sls_mmt_session_t* matching_lls_sls_mmt_session) {
	bool has_atsc3_mmt_sls_mpt_location_info_updated = false;

	for(int i=0; i < mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.count; i++) {

		mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.data[i];
		if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MPT_message) {
			mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;

			//update our lls_sls_mmt_session
			if(matching_lls_sls_mmt_session && mp_table->number_of_assets) {

				for(int j=0; j < mp_table->number_of_assets; j++) {
					mp_table_asset_row_t* mp_table_asset_row = &mp_table->mp_table_asset_row[j];

					if(!mp_table_asset_row) {
						__MMSM_WARN("mp_table_asset_row index: %d, ptr: %p is missing mp_table_asset_row!", j, mp_table_asset_row);
						continue;
					} else if(mp_table_asset_row->mmt_general_location_info.location_type != MMT_GENERAL_LOCATION_INFO_LOCATION_TYPE_MMTP_PACKET_FLOW_UDP_IP_V4) {
                        __MMSM_DEBUG("mp_table_asset_row index: %d, ptr: %p, mp_table_asset_row->mmt_general_location_info.location_type is: 0x%02x", j, mp_table_asset_row, mp_table_asset_row->mmt_general_location_info.location_type);
                        continue;
                    } else {
				        //slight hack, check the asset types and default_asset = 1
                        atsc3_mmt_sls_mpt_location_info_t* atsc3_mmt_sls_mpt_location_info = NULL;
                        bool loop_atsc3_mmt_sls_mpt_location_info_updated = false;

                        for(int k=0; !atsc3_mmt_sls_mpt_location_info && k < matching_lls_sls_mmt_session->atsc3_mmt_sls_mpt_location_info_v.count; k++) {
                            atsc3_mmt_sls_mpt_location_info_t* atsc3_mmt_sls_mpt_location_info_to_check = matching_lls_sls_mmt_session->atsc3_mmt_sls_mpt_location_info_v.data[k];
                            if(mp_table_asset_row->mmt_general_location_info.packet_id == atsc3_mmt_sls_mpt_location_info_to_check->packet_id) {
                                atsc3_mmt_sls_mpt_location_info = atsc3_mmt_sls_mpt_location_info_to_check;
                            }
                        }

                        if(!atsc3_mmt_sls_mpt_location_info) {
                            //jjustman-2021-05-11 - todo: refactor me into a custom __cctor for setting ipv4 relax source ip check
                            atsc3_mmt_sls_mpt_location_info = atsc3_mmt_sls_mpt_location_info_new();
                            atsc3_mmt_sls_mpt_location_info->ipv4_relax_source_ip_check = 1;
                            atsc3_mmt_sls_mpt_location_info->location_type = mp_table_asset_row->mmt_general_location_info.location_type;

                            lls_sls_mmt_session_add_atsc3_mmt_sls_mpt_location_info(matching_lls_sls_mmt_session, atsc3_mmt_sls_mpt_location_info);
                            loop_atsc3_mmt_sls_mpt_location_info_updated = true;

                            atsc3_mmt_sls_mpt_location_info->packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
                            if(mp_table_asset_row->identifier_mapping.asset_id.asset_id) {
                                atsc3_mmt_sls_mpt_location_info->asset_id = (uint8_t*)strndup((const char *)mp_table_asset_row->identifier_mapping.asset_id.asset_id, mp_table_asset_row->identifier_mapping.asset_id.asset_id_length);
                                atsc3_mmt_sls_mpt_location_info->asset_id_length = mp_table_asset_row->identifier_mapping.asset_id.asset_id_length;
                                atsc3_mmt_sls_mpt_location_info->asset_id_scheme = mp_table_asset_row->identifier_mapping.asset_id.asset_id_scheme;
                            }
                        }

                        memcpy(atsc3_mmt_sls_mpt_location_info->asset_type, mp_table_asset_row->asset_type, MP_TABLE_ASSET_ROW_ASSET_TYPE_LENGTH);

                        //jjustman-2021-05-11 - just for MMT IPv4 PLP selection for now...
                        if(mp_table_asset_row->mmt_general_location_info.location_type == MMT_GENERAL_LOCATION_INFO_LOCATION_TYPE_MMTP_PACKET_FLOW_UDP_IP_V4) {
                            atsc3_mmt_sls_mpt_location_info->ipv4_src_addr = mp_table_asset_row->mmt_general_location_info.ipv4_src_addr;
                            atsc3_mmt_sls_mpt_location_info->ipv4_dst_addr = mp_table_asset_row->mmt_general_location_info.ipv4_dst_addr;
                            atsc3_mmt_sls_mpt_location_info->ipv4_dst_port = mp_table_asset_row->mmt_general_location_info.ipv4_dst_port;
                            __MMSM_DEBUG("MPT message: added atsc3_mmt_sls_mpt_location_info for packet_id: %u, asset_type: %s, ipv4_dst_addr: %d, ipv4_dst_port: %d",
                                         atsc3_mmt_sls_mpt_location_info->packet_id, atsc3_mmt_sls_mpt_location_info->asset_type, atsc3_mmt_sls_mpt_location_info->ipv4_dst_addr, atsc3_mmt_sls_mpt_location_info->ipv4_dst_port);

                        }
                        //jjustman-2021-05-11: TODO - handle additional MMT_general_location_info::location_type's

                        if(loop_atsc3_mmt_sls_mpt_location_info_updated) {
                            has_atsc3_mmt_sls_mpt_location_info_updated = true;
                        }

                        //jjustman-2021-05-11 - legacy video/audio/captions packet_id monitoring for matching_lls_sls_mmt_session for transmux, e.g. HLS output
                        __MMSM_TRACE("MPT message: checking packet_id: %u, asset_type: %s, default: %u, identifier: %s", mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");
                        if(ATSC3_MP_TABLE_IS_VIDEO_ASSET_TYPE_ANY(mp_table_asset_row->asset_type)) {
                            matching_lls_sls_mmt_session->video_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
                            __MMSM_TRACE("MPT message: matching_lls_sls_mmt_session: %p, setting video_packet_id: packet_id: %u, asset_type: %s, default: %u, identifier: %s",
                                        matching_lls_sls_mmt_session,
                                        mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

                        } else if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_AC_4_ID, mp_table_asset_row->asset_type, 4) == 0 ||  strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MHM1_ID, mp_table_asset_row->asset_type, 4) == 0 ||
                                    strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MHM2_ID, mp_table_asset_row->asset_type, 4) == 0 || strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MP4A_ID, mp_table_asset_row->asset_type, 4) == 0) {
                            matching_lls_sls_mmt_session->audio_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
                            __MMSM_TRACE("MPT message: matching_lls_sls_mmt_session: %p, setting audio_packet_id: packet_id: %u, asset_type: %s, default: %u, identifier: %s",
                                        matching_lls_sls_mmt_session, mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

                        } else if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_IMSC1_ID, mp_table_asset_row->asset_type, 4) == 0) {
                            matching_lls_sls_mmt_session->stpp_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
                            __MMSM_TRACE("MPT message: matching_lls_sls_mmt_session: %p, setting stpp_packet_id: packet_id: %u, asset_type: %s, default: %u, identifier: %s",
                                        matching_lls_sls_mmt_session,
                                        mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");
					    }
					}
				}
			}
		} else {
			__MMSM_DEBUG("mmt_signalling_message_update_lls_sls_mmt_session: Ignoring signal: 0x%x", mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type);
		}
	}
	
	return has_atsc3_mmt_sls_mpt_location_info_updated;
}

mmt_atsc3_route_component_t* mmt_atsc3_message_payload_add_mmt_atsc3_route_component(mmt_atsc3_message_payload_t* mmt_atsc3_message_payload) {
    if(!mmt_atsc3_message_payload) {
        return NULL;
    }
    mmt_atsc3_route_component_t* mmt_atsc3_route_component = calloc(1, sizeof(mmt_atsc3_route_component_t));
    mmt_atsc3_message_payload->mmt_atsc3_route_component = mmt_atsc3_route_component;
    return mmt_atsc3_route_component;
}

mmt_atsc3_held_message_t* mmt_atsc3_message_payload_add_mmt_atsc3_held_message(mmt_atsc3_message_payload_t* mmt_atsc3_message_payload) {
    if(!mmt_atsc3_message_payload) {
        return NULL;
    }
    mmt_atsc3_held_message_t* mmt_atsc3_held_message = calloc(1, sizeof(mmt_atsc3_held_message_t));
    mmt_atsc3_message_payload->mmt_atsc3_held_message = mmt_atsc3_held_message;
    return mmt_atsc3_held_message;
}


void signalling_message_mmtp_packet_header_dump(mmtp_packet_header_t* mmtp_packet_header) {
	__MMSM_DEBUG("------------------");
	__MMSM_DEBUG("MMTP Packet Header: Signalling Message: ptr: %p", mmtp_packet_header);
	__MMSM_DEBUG("------------------");
	__MMSM_DEBUG(" packet version         : %-10d (0x%d%d)", 	mmtp_packet_header->mmtp_packet_version, ((mmtp_packet_header->mmtp_packet_version >> 1) & 0x1), mmtp_packet_header->mmtp_packet_version & 0x1);
	__MMSM_DEBUG(" payload_type           : %-10d (0x%d%d)", 	mmtp_packet_header->mmtp_payload_type, ((mmtp_packet_header->mmtp_payload_type >> 1) & 0x1), mmtp_packet_header->mmtp_payload_type & 0x1);
	__MMSM_DEBUG(" packet_id              : %-10hu (0x%04x)", 	mmtp_packet_header->mmtp_packet_id, mmtp_packet_header->mmtp_packet_id);
	__MMSM_DEBUG(" timestamp              : %-10u (0x%08x)",	mmtp_packet_header->mmtp_timestamp, mmtp_packet_header->mmtp_timestamp);
	__MMSM_DEBUG(" packet_sequence_number : %-10u (0x%08x)", 	mmtp_packet_header->packet_sequence_number,mmtp_packet_header->packet_sequence_number);
	__MMSM_DEBUG(" packet counter         : %-10u (0x%04x)", 	mmtp_packet_header->packet_counter, mmtp_packet_header->packet_counter);
	__MMSM_DEBUG("------------------");
}

void mmt_signalling_message_dump(mmtp_signalling_packet_t* mmtp_signalling_packet) {
	if(mmtp_signalling_packet->mmtp_payload_type != 0x02) {
		__MMSM_ERROR("signalling_message_dump, payload_type 0x%x != 0x02", mmtp_signalling_packet->mmtp_payload_type);
		return;
	}

	//dump mmtp packet header
	signalling_message_mmtp_packet_header_dump((mmtp_packet_header_t*)mmtp_signalling_packet);

	__MMSM_DEBUG("------------------");
	__MMSM_DEBUG("Signalling Message");
	__MMSM_DEBUG("------------------");
	/**
	 * dump si payload header fields
	 * 	uint8_t		si_fragmentation_indicator; //2 bits,
		uint8_t		si_additional_length_header; //1 bit
		uint8_t		si_aggregation_flag; 		 //1 bit
		uint8_t		si_fragmentation_counter;    //8 bits
	 */
	__MMSM_DEBUG(" si_fragmentation_indicator   : %d", 	mmtp_signalling_packet->si_fragmentation_indicator);
	__MMSM_DEBUG(" additional_length_header   : %d", 	mmtp_signalling_packet->si_additional_length_header);
	__MMSM_DEBUG(" aggregation_flag           : %d",	mmtp_signalling_packet->si_aggregation_flag);
	__MMSM_DEBUG(" fragmentation_counter      : %d",	mmtp_signalling_packet->si_fragmentation_counter);

	for(int i=0; i < mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.count; i++) {
		mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.data[i];

		__MMSM_DEBUG("-----------------");
		__MMSM_DEBUG(" Message ID: %hu (0x%04x)", 	mmt_signalling_message_header_and_payload->message_header.message_id, mmt_signalling_message_header_and_payload->message_header.message_id);
		__MMSM_DEBUG(" Version   : %d", 			mmt_signalling_message_header_and_payload->message_header.version);
		__MMSM_DEBUG(" Length    : %u", 			mmt_signalling_message_header_and_payload->message_header.length);
		__MMSM_DEBUG("-----------");
		__MMSM_DEBUG(" Payload   : %p", 			&mmt_signalling_message_header_and_payload->message_payload);
		__MMSM_DEBUG("------------------");


		if(mmt_signalling_message_header_and_payload->message_header.message_id == PA_message) {
			pa_message_dump(mmt_signalling_message_header_and_payload);
		} else if(mmt_signalling_message_header_and_payload->message_header.message_id >= MPI_message_start && mmt_signalling_message_header_and_payload->message_header.message_id <= MPI_message_end) {
			mpi_message_dump(mmt_signalling_message_header_and_payload);
		} else if(mmt_signalling_message_header_and_payload->message_header.message_id >= MPT_message_start && mmt_signalling_message_header_and_payload->message_header.message_id <= MPT_message_end) {
			mpt_message_dump(mmt_signalling_message_header_and_payload);
		} else if(mmt_signalling_message_header_and_payload->message_header.message_id == MMT_ATSC3_MESSAGE_ID) {
			mmt_atsc3_message_payload_dump(mmt_signalling_message_header_and_payload);
		}
	}
}

void pa_message_dump(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {
	__MMSM_DEBUG(" pa_message");
	__MMSM_DEBUG("-----------------");

}

void mpi_message_dump(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {
	__MMSM_DEBUG(" mpi_message");
	__MMSM_DEBUG("-----------------");

}

void mpt_message_dump(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {

	__MMSM_DEBUG(" mpt_message");
	__MMSM_DEBUG("-----------------");

	mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;

	__MMSM_DEBUG(" table_id                    : %u", mp_table->table_id);
	__MMSM_DEBUG(" version                     : %u", mp_table->version);
	__MMSM_DEBUG(" length                      : %u", mp_table->length);
	__MMSM_DEBUG(" mp_table_mode               : %u", mp_table->mp_table_mode);
	__MMSM_DEBUG(" mmt_package_id.length:      : %u", mp_table->mmt_package_id.mmt_package_id_length);
	if(mp_table->mmt_package_id.mmt_package_id_length) {
		__MMSM_DEBUG(" mmt_package_id.val:       : %s", mp_table->mmt_package_id.mmt_package_id);
	}
	__MMSM_DEBUG(" mp_table_descriptors.length : %u", mp_table->mp_table_descriptors.mp_table_descriptors_length);
	if(mp_table->mp_table_descriptors.mp_table_descriptors_length) {
		__MMSM_DEBUG(" mp_table_descriptors.val    : %s", mp_table->mp_table_descriptors.mp_table_descriptors_byte);
	}
	__MMSM_DEBUG(" number_of_assets            : %u", mp_table->number_of_assets);

	for(int i=0; i < mp_table->number_of_assets; i++) {
		mp_table_asset_row_t* mp_table_asset_row = &mp_table->mp_table_asset_row[i];
		__MMSM_DEBUG(" asset identifier type       : %u", mp_table_asset_row->identifier_mapping.identifier_type);
		if(mp_table_asset_row->identifier_mapping.identifier_type == 0x00) {
			__MMSM_DEBUG(" asset id                    : %s", mp_table_asset_row->identifier_mapping.asset_id.asset_id);

		}
		__MMSM_DEBUG(" asset type                  : %s", mp_table_asset_row->asset_type);
		__MMSM_DEBUG(" asset_clock_relation_flag   : %u", mp_table_asset_row->asset_clock_relation_flag);
		__MMSM_DEBUG(" asset_clock_relation_id     : %u", mp_table_asset_row->asset_clock_relation_id);
		__MMSM_DEBUG(" asset_timescale_flag        : %u", mp_table_asset_row->asset_timescale_flag);
		__MMSM_DEBUG(" asset_timescale             : %u", mp_table_asset_row->asset_timescale);
		__MMSM_DEBUG(" location_count              : %u", mp_table_asset_row->location_count);
//		for(int j=0; j < mp_table_asset_row->location_count; j++) {
//
//		}
		__MMSM_DEBUG(" mmt_general_location_info location_type  : %u", mp_table_asset_row->mmt_general_location_info.location_type);
		__MMSM_DEBUG(" mmt_general_location_info pkt_id         : %u", mp_table_asset_row->mmt_general_location_info.packet_id);
		__MMSM_DEBUG(" mmt_general_location_info ipv4 src addr  : %u", mp_table_asset_row->mmt_general_location_info.ipv4_src_addr);
		__MMSM_DEBUG(" mmt_general_location_info ipv4 dest addr : %u", mp_table_asset_row->mmt_general_location_info.ipv4_dst_addr);
		__MMSM_DEBUG(" mmt_general_location_info ipv4 dest port : %u", mp_table_asset_row->mmt_general_location_info.ipv4_dst_port);
		__MMSM_DEBUG(" mmt_general_location_info message id     : %u", mp_table_asset_row->mmt_general_location_info.message_id);

		//first entry
		__MMSM_DEBUG(" asset_descriptors_length                 : %u", mp_table_asset_row->asset_descriptors_length);
		if(mp_table_asset_row->asset_descriptors_length) {
			if(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor) {
				for(int i=0; i < mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n; i++) {
					__MMSM_DEBUG("   mpu_timestamp_descriptor %u, mpu_sequence_number: %u", i, mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[i].mpu_sequence_number);
					__MMSM_DEBUG("   mpu_timestamp_descriptor %u, mpu_presentation_time: %" PRIu64, i, mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[i].mpu_presentation_time);
				}
			}
		}
	}

}


