/*
* atsc3_sl_tlv_demod_type.h
*
*  Created on: Nov 22, 2019
*      Author: jjustman
*
*   process SL TLV demod format payload
*/

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>


#ifndef ATSC3_SL_TLV_DEMOD_TYPE_H_
#define ATSC3_SL_TLV_DEMOD_TYPE_H_


#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"

#if defined (__cplusplus)
extern "C" {
#endif


/**
 
 Ref: SL SDK API v0.5
 
 2 Demodulator Output Data Format
 --------------------------------
 When the selected standard is ATSC 3.0, the data coming out of selected output interface is ALP data.
 There is a metadata packet that is transferred before the ALP packet.
 The structure of the packet is described below.
 
 The fields of this metadata structure contain both information and diagnostic data.

 Byte       Size        Value               Description
 -------    ------      --------            ------------------
 B0 – B3    4 Bytes     0x24681357          Magic number indicating the start of the Meta data of the TS packet.
 B4 – B7    4 Bytes     Positive value      Size of the ALP Packet in bytes
 B8         1 Byte      0 – 63              PLP number
 B9         1 Byte      188                 TS transfer size in bytes
 B10        1 Byte      24                  TLV header size in bytes
 B11        1 Byte      Positive number     Padding at the end of the ALP packet in bytes
 B12 – B15  4 Bytes     Reserved            Reserved for future use
 B16        1 Byte      Reserved            Reserved for future use
 B17 – B19  3 Bytes     Reserved            Reserved for future use
 B20 – B23  4 Bytes     Reserved            Reserved for future use
 
For other broadcast standards (ATSC1, DVB-T2, ISMB-T etc), the data is in standard MPEG2 Transport stream format.
 */

typedef struct atsc3_sl_tlv_payload {
	uint32_t	sl_tlv_total_parsed_payload_size;	//computed total sl_tlv payload size form parsed block_t

    uint32_t    magic_number;               //0x24681357
    uint32_t    alp_packet_size;
    uint8_t     plp_number;
    uint8_t     ts_size;                    //jjustman: 2019-11-22: default should be 188
    uint8_t     tlv_size;                   //jjustman: 2019-11-22: default should be 24
    uint8_t     alp_trailing_padding_size;  //padding at the end of the ALP packet in bytes
    uint32_t    reserved_b12_b15;           //reserved b12-b15
    uint8_t     reserved_b16;               //reserved b16
    uint32_t    reserved_b17_b19;           //reserved b17-b19
    uint32_t    reserved_b20_b23;           //reserved b20-b23
    bool		alp_payload_complete;		//flag if alp_payload block_t may be incomplete, e.g. BBP under-run
    block_t*    alp_payload;                //extracted ALP payload
} atsc3_sl_tlv_payload_t;


typedef struct atsc3_sl_tlv_payload_metrics {
	uint64_t	total_tlv_bytes_read;
	uint32_t	total_tlv_packets_parsed_count;

	uint32_t	total_tlv_packets_with_matching_magic_count;

	uint32_t	total_tlv_packets_without_matching_magic_count;
	uint32_t	total_tlv_packets_without_matching_magic_is_null_value_count;

	uint64_t	total_tlv_bytes_discarded_due_to_magic_mismatch_count;
	
	//sane mismatch heuristics from TLV header restrictions
	/*
		A331/2020 Section 6.2 defines this value as 65,507, from the following calculation in footnote 5:
	 
		The maximum size of the IP datagram is 65,535 bytes. The maximum UDP data payload is 65,535 minus 20 bytes for the IP header minus 8 bytes for the UDP header.
	*/
	uint32_t	total_tlv_packets_with_TLV_header_ALP_size_greater_than_a331_max_phy_length_count;
	uint32_t	total_tlv_packets_with_TLV_header_ALP_size_mismatch_from_parsed_ALP_header_count;
	uint32_t	total_tlv_packets_without_ALP_starting_at_TS_transfer_size_header_length_count;
	uint32_t	total_tlv_packets_without_magic_number_after_alp_size_data_bytes_consumed_count;

	uint32_t	total_tlv_packets_with_invalid_PLP_value_count; //e.g. PLP > 63
	uint32_t	total_tlv_packets_with_invalid_TS_transfer_size_count; //SLAPI-0.7 defines this as 188
	uint32_t	total_tlv_packets_with_invalid_TLV_header_size_value; //SLAPI-0.7 defines this as 24
	
	//TODO: check for "paddding at end of ALP bytes"
	
	/*
	 for packets that do not have any of the above heuristic errors, capture ALP header metrics (A/330:2019)
	 
	 Table 5.2 Code Values for packet_type
	 
		packet_type Value	Meaning
		-----------------	--------
		000					IPv4 packet
		001					Reserved
		010					Compressed IP packet
		011					Reserved
		100					Link layer signaling packet
		101					Reserved
		110					Packet Type Extension
		111					MPEG-2 Transport Stream

		*NOTE*: packet_types other than 000 (0x0) or 100 (0x4) are SUSPECT, and may indiciate corruption in the BBP FEC frame or BBP de-encapsulation.
	 */
	uint32_t	total_alp_packet_type_ip_packets_count;
	uint64_t	total_alp_packet_type_ip_packets_bytes_read;
	
	uint32_t	total_alp_packet_type_link_layer_signalling_packets_count;
	uint64_t	total_alp_packet_type_link_layer_signalling_bytes_read;
	
	//USUALLY not present in most ATSC 3.0 use-cases
	uint32_t	total_alp_packet_type_packet_compressed_ip_packet_count;
	uint64_t	total_alp_packet_type_packet_compressed_ip_packet_bytes_read;

	uint32_t	total_alp_packet_type_packet_type_extension_count;
	uint64_t	total_alp_packet_type_packet_type_extension_bytes_read;
	
	uint32_t	total_alp_packet_type_packet_mpeg2_transport_stream_count;
	uint64_t	total_alp_packet_type_packet_mpeg2_transport_stream_bytes_read;
	
	//SHOULD NEVER be present according to A330:2019
	uint32_t	total_alp_packet_type_reserved_count;
	uint64_t	total_alp_packet_type_reserved_bytes_read;


} atsc3_sl_tlv_payload_metrics_t;

atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload_parse_from_block_t(block_t* atsc3_sl_tlv_payload_unparsed_block_t);
void atsc3_sl_tlv_payload_free(atsc3_sl_tlv_payload_t** atsc3_sl_tlv_payload_p);
void atsc3_sl_tlv_payload_dump(atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload);


#define __SL_TLV_DEMOD_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __SL_TLV_DEMOD_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __SL_TLV_DEMOD_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define __SL_TLV_DEMOD_DEBUG(...)   if(_SL_TLV_DEMOD_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __SL_TLV_DEMOD_TRACE(...)   if(_SL_TLV_DEMOD_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#if defined (__cplusplus)
}
#endif

#endif /* ATSC3_SL_TLV_DEMOD_TYPE_H_ */
