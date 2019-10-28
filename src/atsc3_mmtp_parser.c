/*
 * mmtp_types.h
 *
 *  Created on: Jan 3, 2019
 *      Author: jjustman
 *
 *
 * parses the header of the MMTP packet, and invokes specific methods for MPU and signaling messages
 */
#include <assert.h>
#include <limits.h>

#include "atsc3_mmtp_parser.h"
#include "atsc3_mmt_mpu_parser.h"
#include "atsc3_mmt_mpu_utils.h"
#include "atsc3_mmt_signalling_message.h"
#include "atsc3_mmtp_packet_types.h"

int _MMTP_DEBUG_ENABLED = 0;
int _MMTP_TRACE_ENABLED = 0;

/*
 * Open items:
 *
 *  The value of the timestamp field of an MMTP packet shall represent the UTC time when the first byte of the MMTP packet is passed to the UDP layer and shall be formatted in the “short format” as specified in Clause 6 of RFC 5905, NTP version 4 [23].
 *  The value of the RAP_flag of MMTP packets shall be set to 0 when the value of the FT field is equal to 0.
 */

mmtp_packet_header_t* mmtp_packet_header_parse_from_block_t(block_t* udp_packet) {

	if(block_Remaining_size(udp_packet) < 20) {
		//bail, the min header is at least 20 bytes
		__MMTP_PARSER_ERROR("mmtp_packet_header_parse_from_block_t: udp_raw_buf size is: %d, need at least 20 bytes, udp_packet ptr: %p", block_Remaining_size(udp_packet), udp_packet);
		return NULL;
	}

	mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_new();

	int mmtp_payload_length = block_Remaining_size(udp_packet);
	uint8_t *raw_buf = block_Get(udp_packet);
	uint8_t *buf = raw_buf;

	__MMTP_PARSER_DEBUG("mmtp_packet_header_parse_from_block_t: udp_packet->i_pos: %d, udp_packet->p_size: %d, udp_packet->p_buffer: %p, mmtp_packet_header: %p",
			udp_packet->i_pos,
			udp_packet->p_size,
			raw_buf,
			mmtp_packet_header);


	uint8_t mmtp_packet_preamble[20];

	buf = extract(buf, mmtp_packet_preamble, 20);

	//A/331 Section 8.1.2.1.3 Constraints on MMTP
	// The value of the version field of MMTP packets shall be '01'.
#if !_ISO23008_1_MMTP_VERSION_0x00_SUPPORT_
	mmtp_packet_header->mmtp_packet_version = (mmtp_packet_preamble[0] & 0xC0) >> 6;
	if(mmtp_packet_header->mmtp_packet_version != 0x1) {
		__MMTP_PARSER_ERROR("mmtp_packet_header_parse_from_block_t: MMTP version field != 0x1, value is 0x%x, bailing!", mmtp_packet_header->mmtp_packet_version);
		goto error;
	}
#endif

	mmtp_packet_header->packet_counter_flag = (mmtp_packet_preamble[0] & 0x20) >> 5;
	mmtp_packet_header->fec_type = (mmtp_packet_preamble[0] & 0x18) >> 3;

#if _ISO23008_1_MMTP_VERSION_0x00_SUPPORT_

	if(mmtp_packet_header->mmtp_packet_version == 0x00) {
		//after fec_type, with v=0, next bitmask is 0x4 >>2
		//0000 0010
		//V0CF E-XR
		mmtp_packet_header->mmtp_header_extension_flag = (mmtp_packet_preamble[0] & 0x2) >> 1;
		mmtp_packet_header->mmtp_rap_flag = mmtp_packet_preamble[0] & 0x1;

		//6 bits right aligned
		mmtp_packet_header->mmtp_payload_type = mmtp_packet_preamble[1] & 0x3f;
		if(mmtp_packet_header->mmtp_header_extension_flag & 0x1) {
			mmtp_packet_header->mmtp_header_extension_type = (mmtp_packet_preamble[16]) << 8 | mmtp_packet_preamble[17];
			mmtp_packet_header->mmtp_header_extension_length = (mmtp_packet_preamble[18]) << 8 | mmtp_packet_preamble[19];
		} else {
			//walk back by 4 bytes
			buf-=4;
		}
	} else

#endif

	if(mmtp_packet_header->mmtp_packet_version == 0x01) {
		//bitmask is 0000 00
		//0000 0100
		//V1CF EXRQ
		mmtp_packet_header->mmtp_header_extension_flag = (mmtp_packet_preamble[0] & 0x4) >> 2;    //X
		mmtp_packet_header->mmtp_rap_flag = (mmtp_packet_preamble[0] & 0x2) >> 1;			    //RAP
        mmtp_packet_header->mmtp_qos_flag = mmtp_packet_preamble[0] & 0x1;					    //Q: QOS
		//0000 0000
		//FEBI TYPE
		//4 bits for preamble right aligned

		mmtp_packet_header->mmtp_flow_identifer_flag = ((mmtp_packet_preamble[1]) & 0x80) >> 7;			//F
		mmtp_packet_header->mmtp_flow_extension_flag = ((mmtp_packet_preamble[1]) & 0x40) >> 6;			//E
		mmtp_packet_header->mmtp_header_compression = ((mmtp_packet_preamble[1]) &  0x20) >> 5; 		//B
		mmtp_packet_header->mmtp_indicator_ref_header_flag = ((mmtp_packet_preamble[1]) & 0x10) >> 4;	//I

		mmtp_packet_header->mmtp_payload_type = mmtp_packet_preamble[1] & 0xF;
        
        if(!((mmtp_packet_preamble[16] >> 7) & 0x1)) {
        	__MMTP_PARSER_DEBUG("mmtp_demuxer: ISO23008-1: mmtp_packet_preamble byte[16] 'r' bit is not 1!");
        }
		//TB 2 bits
		mmtp_packet_header->mmtp_type_of_bitrate = ((mmtp_packet_preamble[16] & 0x40) >> 6) | ((mmtp_packet_preamble[16] & 0x20) >> 5);

		//DS 3 bits
        mmtp_packet_header->mmtp_delay_sensitivity = ((mmtp_packet_preamble[16] >> 2) & 0x7);
           
		//TP 3 bits
		mmtp_packet_header->mmtp_transmission_priority = ((mmtp_packet_preamble[16] & 0x03) << 1) | ((mmtp_packet_preamble[17] >> 7) & 0x1);

		mmtp_packet_header->flow_label = mmtp_packet_preamble[17] & 0x7f;

		//header extension is offset by 2 bytes in v=1, so an additional block chain read is needed to get extension length
		if(mmtp_packet_header->mmtp_header_extension_flag & 0x1) {
			mmtp_packet_header->mmtp_header_extension_type = (mmtp_packet_preamble[18] << 8) | mmtp_packet_preamble[19];

			__MMTP_PARSER_TRACE("mmtp_packet_header_parse_from_block_t: mmtp_demuxer - doing mmtp_header_extension_length_bytes: %d",  mmtp_packet_header->mmtp_header_extension_type);

			uint8_t mmtp_header_extension_length_bytes[2];
			buf = extract(buf, mmtp_header_extension_length_bytes, 2);

			mmtp_packet_header->mmtp_header_extension_length = mmtp_header_extension_length_bytes[0] << 8 | mmtp_header_extension_length_bytes[1];
		} else {
			//walk us back for mmtp payload type header parsing
			buf-=2;
		}
	} else {
		__MMTP_PARSER_ERROR("mmtp_demuxer - unknown packet version of 0x%X", mmtp_packet_header->mmtp_packet_version);
		goto error;
	}

	mmtp_packet_header->mmtp_packet_id			= mmtp_packet_preamble[2]  << 8  | mmtp_packet_preamble[3];

#if _ATSC3_MMT_PACKET_ID_MPEGTS_COMPATIBILITY_
	//exception for MMT signaling, See A/331 7.2.3.

	if(!(mmtp_packet_header->mmtp_packet_id == 0x0000 && mmtp_packet_header->mmtp_payload_type == 0x2)) {
		if(!(mmtp_packet_header->mmtp_packet_id >= 0x0010 && mmtp_packet_header->mmtp_packet_id <= 0x1FFE)) {
			__MMTP_PARSER_ERROR("mmtp_packet_header_parse_from_block_t: MMTP packet_id is not compliant with A/331 8.1.2.1.3 - MPEG2 conversion compatibility, packet_id: %-10hu (0x%04x)", mmtp_packet_header->mmtp_packet_id, mmtp_packet_header->mmtp_packet_id);
			goto error;
		}
	}
#endif
	mmtp_packet_header->mmtp_timestamp = mmtp_packet_preamble[4]  << 24 | mmtp_packet_preamble[5]  << 16 | mmtp_packet_preamble[6]   << 8 | mmtp_packet_preamble[7];
	compute_ntp32_to_seconds_microseconds(mmtp_packet_header->mmtp_timestamp, &mmtp_packet_header->mmtp_timestamp_s, &mmtp_packet_header->mmtp_timestamp_us);

	mmtp_packet_header->packet_sequence_number	= mmtp_packet_preamble[8]  << 24 | mmtp_packet_preamble[9]  << 16 | mmtp_packet_preamble[10]  << 8 | mmtp_packet_preamble[11];
   
    if(mmtp_packet_header->packet_counter_flag) {
        mmtp_packet_header->packet_counter = mmtp_packet_preamble[12] << 24 | mmtp_packet_preamble[13] << 16 | mmtp_packet_preamble[14]  << 8 | mmtp_packet_preamble[15];
    } else {
        //walk back our buff by 4 bytes, korean MMT may not set this.
        buf-=4;
    }

    if(mmtp_packet_header->mmtp_header_extension_flag & 0x1) {
    	//clamp mmtp_header_extension_length to max length of our mmtp packet
		mmtp_packet_header->mmtp_header_extension_length = MIN(mmtp_packet_header->mmtp_header_extension_length, mmtp_payload_length - (buf - raw_buf));

		__MMT_MPU_PARSER_DEBUG("mmtp_mpu_packet_parse_from_block_t: mmtp_header_extension_flag, header extension size: %d, packet version: %d, payload_type: 0x%X, packet_id 0x%hu, timestamp: 0x%X, packet_sequence_number: 0x%X, packet_counter: 0x%X",
				mmtp_packet_header->mmtp_packet_version,
				mmtp_packet_header->mmtp_header_extension_length,
				mmtp_packet_header->mmtp_payload_type,
				mmtp_packet_header->mmtp_packet_id,
				mmtp_packet_header->mmtp_timestamp,
				mmtp_packet_header->packet_sequence_number,
				mmtp_packet_header->packet_counter);

		mmtp_packet_header->mmtp_header_extension = block_Alloc(mmtp_packet_header->mmtp_header_extension_length);
		block_Write(mmtp_packet_header->mmtp_header_extension, buf, mmtp_packet_header->mmtp_header_extension_length);
		buf += mmtp_packet_header->mmtp_header_extension_length;
		int32_t mmtp_payload_remaining_length = mmtp_payload_length - (buf - raw_buf);
		if(mmtp_payload_remaining_length < 1) {
			__MMT_MPU_PARSER_ERROR("mmtp_packet_header_parse_from_block_t: reading mmtp_header_extension_length, remaining size too small: %d", mmtp_payload_remaining_length);
			goto error;
		}
	}

    int32_t bytes_processed = (buf - raw_buf);
    __MMTP_PARSER_DEBUG("mmtp_packet_header_parse_from_block_t: completed header parse, consumed %d bytes, mmtp_packet_header is: %p",
    		bytes_processed,
			mmtp_packet_header);

    block_Seek_Relative(udp_packet, bytes_processed);

	return mmtp_packet_header;

error:
	mmtp_packet_header_free(&mmtp_packet_header);

	return NULL;
}


//TODO: purge
////think of this as castable to the base fields as they are the same size layouts
//mmtp_payload_fragments_union_t* mmtp_packet_create(block_t * raw_packet,
//												uint8_t mmtp_packet_version,
//												uint8_t mmtp_payload_type,
//												uint16_t mmtp_packet_id,
//												uint32_t packet_sequence_number,
//												uint32_t packet_counter,
//												uint32_t mmtp_timestamp) {
//	mmtp_payload_fragments_union_t *entry = NULL;
//
//	//pick the larger of the timed vs. non-timed fragment struct sizes
//	entry = calloc(1, sizeof(mmtp_payload_fragments_union_t));
//
//	if(!entry) {
//		abort();
//	}
//
//	entry->mmtp_packet_header->raw_packet = raw_packet;
//	entry->mmtp_packet_header->mmtp_packet_version = mmtp_packet_version;
//	entry->mmtp_packet_header->mmtp_payload_type = mmtp_payload_type;
//	entry->mmtp_packet_header->mmtp_packet_id = mmtp_packet_id;
//	entry->mmtp_packet_header->packet_sequence_number = packet_sequence_number;
//	entry->mmtp_packet_header->packet_counter = packet_counter;
//	entry->mmtp_packet_header->mmtp_timestamp = mmtp_timestamp;
//
//	return entry;
//}



void mmtp_packet_header_dump(mmtp_packet_header_t* mmtp_packet_header) {
	__MMTP_PARSER_DEBUG("------------------");
	__MMTP_PARSER_DEBUG("MMTP Packet Header (%p)", mmtp_packet_header);
	__MMTP_PARSER_DEBUG("------------------");
	__MMTP_PARSER_DEBUG(" packet version         : %-10d (0x%d%d)", 	mmtp_packet_header->mmtp_packet_version, ((mmtp_packet_header->mmtp_packet_version >> 1) & 0x1), mmtp_packet_header->mmtp_packet_version & 0x1);
	__MMTP_PARSER_DEBUG(" payload_type           : %-10d (0x%d%d)", 	mmtp_packet_header->mmtp_payload_type, ((mmtp_packet_header->mmtp_payload_type >> 1) & 0x1), mmtp_packet_header->mmtp_payload_type & 0x1);
	__MMTP_PARSER_DEBUG(" packet_id              : %-10hu (0x%04x)", mmtp_packet_header->mmtp_packet_id, mmtp_packet_header->mmtp_packet_id);
	__MMTP_PARSER_DEBUG(" timestamp              : %-10u (0x%08x)", 	mmtp_packet_header->mmtp_timestamp, mmtp_packet_header->mmtp_timestamp);
	__MMTP_PARSER_DEBUG(" packet_sequence_number : %-10u (0x%08x)",	mmtp_packet_header->packet_sequence_number,mmtp_packet_header->packet_sequence_number);
	__MMTP_PARSER_DEBUG(" packet counter         : %-10u (0x%04x)", 	mmtp_packet_header->packet_counter, mmtp_packet_header->packet_counter);
	__MMTP_PARSER_DEBUG("------------------");
}

