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

	uint32_t mmtp_msg_length_long;
	buf = extract(buf, (uint8_t*)&mmtp_msg_length_long, 4);
	mmt_signalling_message_header_and_payload->message_header.length = ntohl(mmtp_msg_length_long);

	if(mmt_signalling_message_header->message_id == PA_message) {
		buf = pa_message_parse(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id >= MPI_message_start && mmt_signalling_message_header->message_id < MPI_message_end) {
		buf = mpi_message_parse(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id >= MPT_message_start && mmt_signalling_message_header->message_id < MPT_message_end) {
		buf = mpt_message_parse(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == CRI_message) {
		//0x200
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == DCI_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == SSWR_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == AL_FEC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == HRBM_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == MC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == AC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == AF_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == RQF_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == ADC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == HRB_removal_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == LS_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == LR_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == NAMF_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == LDC_message) {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else if(mmt_signalling_message_header->message_id == MMT_ATSC3_MESSAGE_ID) {
		buf = mmt_atsc3_message_payload_parse(mmt_signalling_message_header_and_payload, buf, buf_size);
	} else {
		buf = si_message_not_supported(mmt_signalling_message_header_and_payload, buf, buf_size);
	}

	return buf;

}
////parse the mmtp payload header for signaling message mode
//uint8_t	mmtp_payload_header[2];
//buf = extract(buf, mmtp_payload_header, 2);



uint8_t* pa_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, uint8_t* udp_raw_buf, uint32_t udp_raw_buf_size) {
	_MMSM_WARN("signalling information message id not supported: 0x%04x", mmt_signalling_message_header_and_payload->message_header.message_id);

	return udp_raw_buf;
}
uint8_t* mpi_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, uint8_t* udp_raw_buf, uint32_t buf_size) {
	_MMSM_WARN("signalling information message id not supported: 0x%04x", mmt_signalling_message_header_and_payload->message_header.message_id);

	return udp_raw_buf;
}
uint8_t* mpt_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, uint8_t* buf, uint32_t buf_size) {

	mpt_message_t* mpt_message = calloc(1, sizeof(mpt_message_t));

	uint8_t scratch[2];
	buf = extract(buf, scratch, 2);
	mpt_message->message_id = (scratch[0] << 8) | (scratch[1]);
	//if message_id==20 - full message, otherwise subset n-1

	uint8_t scratch_single;
	buf = extract(buf, &scratch_single, 1);
	mpt_message->version = scratch_single;

	buf = extract(buf, scratch, 2);
	mpt_message->length = (scratch[0] << 8) | (scratch[1]);

	//si_message->mmtp_signalling_message_fragments.payload = (void*) mpt_message;


	buf = extract(buf, &scratch_single, 1);
	mpt_message->mp_table.table_id = scratch_single;

	buf = extract(buf, &scratch_single, 1);
	mpt_message->mp_table.version = scratch_single;

	buf = extract(buf, scratch, 2);
	mpt_message->mp_table.length = (scratch[0] << 8) | scratch[1];

	buf = extract(buf, &scratch_single, 1);
	if((scratch_single >> 2) != 0x3F) {
	//	_MMSM_WARN("mp_table reserved 6 bits are not set - message_id: 0x%04x, table_id: 0x%02x, packet_counter: %u", mmt_signalling_message_header->message_id, mpt_message->mp_table.table_id, si_message->mmtp_mpu_type_packet_header.packet_counter);

		goto cleanup;
	}

	//set MP_table_mode
	mpt_message->mp_table.mp_table_mode = scratch_single & 0x2;

	if(mpt_message->mp_table.table_id == 0x20 || mpt_message->mp_table.table_id == 0x11) {
		//process packages & descriptors
		_MMSM_WARN("mp_table processing for mmt_package_id not supported yet!");

		//read mmt_package_id here

	}


	buf = extract(buf, &scratch_single, 1);
	scratch_single = (scratch_single > 255) ? 255 : (scratch_single > 0) ? scratch_single : 0;
	mpt_message->mp_table.number_of_assets = scratch_single;
	mpt_message->mp_table.mp_table_asset_row = calloc(scratch_single, sizeof(mp_table_asset_row_t));
	for(int i=0; i < mpt_message->mp_table.number_of_assets; i++ ) {
		mp_table_asset_row_t* row = &mpt_message->mp_table.mp_table_asset_row[i];

		//grab our identifer mapping
		buf = extract(buf, &scratch_single, 1);
		row->identifier_mapping.identifier_type = scratch_single;
		if(row->identifier_mapping.identifier_type == 0x00) {
			uint8_t asset_id_array[4];

			buf = extract(buf, asset_id_array, 4);
			row->identifier_mapping.asset_id.asset_id_scheme = (asset_id_array[0] << 24) | (asset_id_array[1] << 16) | (asset_id_array[2] << 8) | asset_id_array[3];

			buf = extract(buf, asset_id_array, 4);
			row->identifier_mapping.asset_id.asset_id_length = (asset_id_array[0] << 24) | (asset_id_array[1] << 16) | (asset_id_array[2] << 8) | asset_id_array[3];

			//implicit vuln here:
			row->identifier_mapping.asset_id.asset_id_bytes = calloc((row->identifier_mapping.asset_id.asset_id_length), sizeof(uint8_t));

			for(int i=0; i < row->identifier_mapping.asset_id.asset_id_length; i++) {
				//not the most performant...
				buf = extract(buf, &scratch_single, 1);
				row->identifier_mapping.asset_id.asset_id_bytes[i] = scratch_single;

			}

		} else if(row->identifier_mapping.identifier_type == 0x01) {
			//build url

		}
		uint8_t asset_arr[4];
		buf = extract(buf, asset_arr, 4);
		row->asset_type = (asset_arr[0] << 24) & (asset_arr[1] << 16) & (asset_arr[2] << 8) & asset_arr[3];

	}


cleanup:

	return NULL;
}

uint8_t* mmt_atsc3_message_payload_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, uint8_t* udp_raw_buf, uint32_t udp_raw_buf_size) {
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
		//cheat and over-alloc+1 for a null byte
		uint8_t* URI_payload = calloc(URI_length + 1, sizeof(char));
		//not efficent, but oh well
		for(int i=0; i < mmt_atsc3_message_payload->URI_length; i++) {
			buf = extract(buf, (uint8_t*)&URI_payload[i], 1);
		}
		mmt_atsc3_message_payload->URI_payload = URI_payload;
	}

	uint32_t temp_atsc3_message_content_length;
	buf = extract(buf, (uint8_t*)&temp_atsc3_message_content_length, 4);
	temp_atsc3_message_content_length = ntohl(temp_atsc3_message_content_length);

	if(temp_atsc3_message_content_length) {
		//cheat and over-alloc+1 for a null byte

		uint8_t* temp_atsc3_message_content = calloc(temp_atsc3_message_content_length + 1, sizeof(char));
		//not efficent, but oh well
		for(int i=0; i < temp_atsc3_message_content_length; i++) {
			buf = extract(buf, (uint8_t*)&temp_atsc3_message_content[i], 1);
		}


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

void mmt_atsc3_message_payload_dump(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload) {

	mmt_atsc3_message_payload_t* mmt_atsc3_message_payload = &mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload;

	_MMSM_INFO("-----------------");
	_MMSM_INFO("mmt_atsc3_message");
	_MMSM_INFO("-----------------");
	_MMSM_INFO("service_id:                        %u", mmt_atsc3_message_payload->service_id);
	_MMSM_INFO("atsc3_message_content_type:        %u", mmt_atsc3_message_payload->atsc3_message_content_type);
	_MMSM_INFO("atsc3_message_content_version:     %u", mmt_atsc3_message_payload->atsc3_message_content_version);
	_MMSM_INFO("atsc3_message_content_compression: %u", mmt_atsc3_message_payload->atsc3_message_content_compression);
	_MMSM_INFO("URI_length:                        %u", mmt_atsc3_message_payload->URI_length);
	_MMSM_INFO("URI_payload:                       %s", mmt_atsc3_message_payload->URI_payload);
	if(mmt_atsc3_message_payload->atsc3_message_content_compression == 0x02) {
		_MMSM_INFO("atsc3_message_content_length_compressed:      %u", mmt_atsc3_message_payload->atsc3_message_content_length_compressed);
	}
	_MMSM_INFO("atsc3_message_content_length:      %u", mmt_atsc3_message_payload->atsc3_message_content_length);
	_MMSM_INFO("atsc3_message_content:             %s", mmt_atsc3_message_payload->atsc3_message_content);

}


uint8_t* si_message_not_supported(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, uint8_t* udp_raw_buf, uint32_t udp_raw_buf_size) {
//	_MMSM_WARN("signalling information message id not supported: 0x%04x", mmt_signalling_message_header->message_id);

	return NULL;
}



void signaling_message_mmtp_packet_header_dump(mmtp_payload_fragments_union_t* mmtp_payload_fragments) {
	_MMSM_INFO("------------------");
	_MMSM_INFO("MMTP Packet Header");
	_MMSM_INFO("------------------");
	_MMSM_INFO(" packet version         : %-10d (0x%d%d)", mmtp_payload_fragments->mmtp_packet_header.mmtp_packet_version, ((mmtp_payload_fragments->mmtp_packet_header.mmtp_packet_version >> 1) & 0x1), mmtp_payload_fragments->mmtp_packet_header.mmtp_packet_version & 0x1);
	_MMSM_INFO(" payload_type           : %-10d (0x%d%d)", mmtp_payload_fragments->mmtp_packet_header.mmtp_payload_type, ((mmtp_payload_fragments->mmtp_packet_header.mmtp_payload_type >> 1) & 0x1), mmtp_payload_fragments->mmtp_packet_header.mmtp_payload_type & 0x1);
	_MMSM_INFO(" packet_id              : %-10hu (0x%04x)", mmtp_payload_fragments->mmtp_packet_header.mmtp_packet_id, mmtp_payload_fragments->mmtp_packet_header.mmtp_packet_id);
	_MMSM_INFO(" timestamp              : %-10u (0x%08x)", mmtp_payload_fragments->mmtp_packet_header.mmtp_timestamp, mmtp_payload_fragments->mmtp_packet_header.mmtp_timestamp);
	_MMSM_INFO(" packet_sequence_number : %-10u (0x%08x)", mmtp_payload_fragments->mmtp_packet_header.packet_sequence_number,mmtp_payload_fragments->mmtp_packet_header.packet_sequence_number);
	_MMSM_INFO(" packet counter         : %-10u (0x%04x)", mmtp_payload_fragments->mmtp_packet_header.packet_counter, mmtp_payload_fragments->mmtp_packet_header.packet_counter);
	_MMSM_INFO("------------------");
}
void signaling_message_dump(mmtp_payload_fragments_union_t* mmtp_payload_fragments) {
	if(mmtp_payload_fragments->mmtp_packet_header.mmtp_payload_type != 0x02) {
		_MMSM_ERROR("signaling_message_dump, payload_type 0x%x != 0x02", mmtp_payload_fragments->mmtp_packet_header.mmtp_payload_type);
		return;
	}

	//dump mmtp packet header
	signaling_message_mmtp_packet_header_dump(mmtp_payload_fragments);

	_MMSM_INFO("-----------------");
	_MMSM_INFO("Signaling Message");
	_MMSM_INFO("-----------------");
	/**
	 * dump si payload header fields
	 * 	uint8_t		si_fragmentation_indiciator; //2 bits,
		uint8_t		si_additional_length_header; //1 bit
		uint8_t		si_aggregation_flag; 		 //1 bit
		uint8_t		si_fragmentation_counter;    //8 bits
		uint16_t	si_aggregation_message_length;
	 */
	_MMSM_INFO(" fragmentation_indiciator   : %d", 	mmtp_payload_fragments->mmtp_signalling_message_fragments.si_fragmentation_indiciator);
	_MMSM_INFO(" additional_length_header   : %d", 	mmtp_payload_fragments->mmtp_signalling_message_fragments.si_additional_length_header);
	_MMSM_INFO(" aggregation_flag           : %d",	mmtp_payload_fragments->mmtp_signalling_message_fragments.si_aggregation_flag);
	_MMSM_INFO(" fragmentation_counter      : %d",	mmtp_payload_fragments->mmtp_signalling_message_fragments.si_fragmentation_counter);
	_MMSM_INFO(" aggregation_message_length : %hu",	mmtp_payload_fragments->mmtp_signalling_message_fragments.si_aggregation_message_length);

	for(int i=0; i < mmtp_payload_fragments->mmtp_signalling_message_fragments.mmt_signalling_message_vector.messages_n; i++) {
		mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmtp_payload_fragments->mmtp_signalling_message_fragments.mmt_signalling_message_vector.messages[i];

		_MMSM_INFO("-----------------");
		_MMSM_INFO(" Message ID      : %hu (0x%04x)", 	mmt_signalling_message_header_and_payload->message_header.message_id, mmt_signalling_message_header_and_payload->message_header.message_id);
		_MMSM_INFO(" Version         : %d", 			mmt_signalling_message_header_and_payload->message_header.version);
		_MMSM_INFO(" Length          : %u", 			mmt_signalling_message_header_and_payload->message_header.length);
		_MMSM_INFO("------------------");
		_MMSM_INFO(" Payload         : %p", 			&mmt_signalling_message_header_and_payload->message_payload);
		_MMSM_INFO("------------------");
		_MMSM_INFO("");

		//_MMSM_INFO("--------------------------------------");

		if(mmt_signalling_message_header_and_payload->message_header.message_id == PA_message) {
			pa_message_dump(mmtp_payload_fragments);
		} else if(mmt_signalling_message_header_and_payload->message_header.message_id >= MPI_message_start && mmt_signalling_message_header_and_payload->message_header.message_id < MPI_message_end) {
			mpi_message_dump(mmtp_payload_fragments);
		} else if(mmt_signalling_message_header_and_payload->message_header.message_id >= MPT_message_start && mmt_signalling_message_header_and_payload->message_header.message_id < MPT_message_end) {
			mpt_message_dump(mmtp_payload_fragments);
		} else if(mmt_signalling_message_header_and_payload->message_header.message_id == MMT_ATSC3_MESSAGE_ID) {
			mmt_atsc3_message_payload_dump(mmt_signalling_message_header_and_payload);
		}
	}
}

void pa_message_dump(mmtp_payload_fragments_union_t* mmtp_payload_fragments) {

}

void mpi_message_dump(mmtp_payload_fragments_union_t* mmtp_payload_fragments) {

}

void mpt_message_dump(mmtp_payload_fragments_union_t* mmtp_payload_fragments) {

}


