/*
* atsc3_sony_tlv_demod_type.h
*
*  Created on: Aug 10, 2022
*      Author: jjustman
*
*   process Sony TLV/ALP demod format payload
*/

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <inttypes.h>

#ifndef ATSC3_SONY_TLV_DEMOD_TYPE_H_
#define ATSC3_SONY_TLV_DEMOD_TYPE_H_

#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>
#include <atsc3_alp_parser.h>

#if defined (__cplusplus)
extern "C" {
#endif

extern int __ATSC3_SL_TLV_USE_INLINE_ALP_PARSER_CALL__;
extern int __ATSC3_SL_TLV_EXTRACT_L1D_TIME_INFO__;

/*

 sony tlv header looks like:
     
         Syntax                                     Bits        Format
         ------                                     -----       ------
         sony_tlv_header() {
            sync_byte                               8           uimsbf, value == 0x47 per mpeg-2 ts sync byte
            transport_error_indicator               1           uimsbf, value == 0 -> no error packet,
                                                                        value == 0x1 -> error packet
            package_start_indicator                 1           uimsbf, value == 0 -> no start pointer, 3 byte header     (185 byte payload)
                                                                        value == 1 -> start pointer in 4th byte of packet (184 byte payload)
            reserved_0                              1           uimsbf, value SHOULD == 0
            pid                                     13          uimsbf, TLV/ALP PID value SHOULD == 0x002D
            
            if(package_start_indicator == 1) {
                start_pointer                       8           uimsbf, number of bytes - 1 until next TLV packet start position
            }
         }
 */

typedef struct atsc3_sony_tlv_payload {
    uint8_t     sync_byte;                      //must be 0x47
    uint8_t     transport_error_indicator:1;
    uint8_t     package_start_indicator:1;
    uint8_t     reserved_0:1;                   //should be 0
    uint16_t    pid:13;                         //should be 0x002D
    
    //if(package_start_indicator == 1) {
    uint8_t     start_pointer;                  //number of bytes - 1 until next TLV packet start position
    //
    
    block_t*    unparsed_alp_payload;
} atsc3_sony_tlv_payload_t;

/*
 sony alp has 2 special formats, one for PTP, and one for PLP information using ALP "Header Extension" flags.
 
 see atsc3_alp_types for base atsc3_alp_packet implementation reference
 
         Syntax                                     Bits        Format
         ------                                     -----       ------
         atsc3_alp_packet () {
            packet_type                             3           uimsbf,
            payload_configuration                   1           uimsbf,

            if(payload_configuration == 0) {
                header_mode                         1           uimsbf, for sony extensions, value == 1
                length                              11
                
                if(header_mode == 1) {
                    single_packet_hdr()             var         note, at least one additional byte to parse format
                    
                    if(header_extension_flag == 1) {
                        header_extension()          var
                    }
                }
            }
        }
     
        single_packet_hdr() {
            length_msb                              5           value for sony extension expected to be 0x0
            reserved_1                              1           value == 1
            substream_identifier_flag               1           value SHOULD be 0
            header_extension_flag                   1           value SHOULD be 1
        }
        
 
    Resultant ALP bitstream start marker will look something like:
 
    First 3 bytes   bitfields
    -------------   ----------------------------------
    0x080005        0000 1000 0000 0000 0000 0101
                         H                    R H
                         M                    s E
                                              v F
                                              d
                                              1
    
 
 
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
    uint32_t    reserved_b24_b27;           //reserved b24-b27

    uint32_t    l1d_time_sec;               //jjustman-2021-10-24 - new time info fields
    uint32_t    l1d_time_msec;
    uint32_t    l1d_time_usec;
    uint32_t    l1d_time_nsec;

	
    bool		alp_payload_complete;		//flag if alp_payload block_t may be incomplete, e.g. BBP under-run
    block_t*    alp_payload;                //extracted ALP payload buffer data
	
	//only set if __ATSC3_SL_TLV_USE_INLINE_ALP_PARSER_CALL__ is true
	//jjustman-2022-07-23 - don't keep a ref to our transient inline alp parser call
	// atsc3_alp_packet_t* atsc3_alp_packet; 	//extract the atsc3_alp_packet in the TLV_payload_parser
	
} atsc3_sl_tlv_payload_t;


typedef struct atsc3_sl_tlv_payload_metrics {
	uint64_t	total_tlv_bytes_read;
	uint32_t	total_tlv_packets_parsed_count;

	uint32_t	total_tlv_packets_with_matching_magic_count;
	uint32_t	total_tlv_header_alp_size_bytes_read;
	uint32_t	total_alp_packets_actual_size_bytes;
	uint32_t	total_tlv_header_alp_trailing_padding_size_bytes;
	
	uint32_t	total_tlv_packets_without_matching_magic_count;
	uint32_t	total_tlv_packets_without_matching_magic_is_null_value_count;

	uint64_t	total_tlv_bytes_discarded_due_to_magic_mismatch_count;

	uint32_t	total_tlv_packets_without_matching_magic_recovered_in_block_count;
	uint32_t	total_tlv_bytes_discarded_without_matching_magic_recovered_in_block_count;

    uint32_t    total_tlv_packets_discarded_for_sdio_padding; //PLP == 0xFF && IF=SDIO, see SL_DEMOD_API.pdf v0.15 pp 29

    //sane mismatch heuristics from TLV header restrictions
	uint32_t	total_tlv_packets_with_invalid_PLP_value_count; //e.g. PLP > 63
	uint32_t	total_tlv_packets_with_invalid_TS_transfer_size_count; //SLAPI-0.7 defines this as 188
	uint32_t	total_tlv_packets_with_invalid_TLV_header_size_value; //SLAPI-0.7 defines this as 24
	
	uint32_t	total_tlv_packets_with_successfully_extracted_alp_count;
	uint32_t	total_tlv_packets_with_failed_extracted_alp_count;
	
	uint32_t	total_tlv_packets_with_TLV_header_ALP_size_greater_than_max_IP_UDP_datagram_size_count;
	uint32_t	total_tlv_packets_with_TLV_header_ALP_size_mismatch_from_parsed_ALP_header_count;
	
	//TODO: uint32_t	total_tlv_packets_without_ALP_starting_at_TS_transfer_size_header_length_count;
	uint32_t	total_tlv_packets_without_magic_number_after_alp_size_data_bytes_consumed_count;
	
	/*
	 for packets that do not have any of the above heuristic errors, capture ALP header metrics (A/330:2019)
	 
	 Table 5.2 Code Values for packet_type
	 
		packet_type Value	Meaning
		-----------------	--------
		000					IPv4 packet					//0x0
		001					Reserved						//0x1
		010					Compressed IP packet		//0x2
		011					Reserved						//0x3
		100					Link layer signaling packet	//0x4
		101					Reserved						//0x5
		110					Packet Type Extension		//0x6
		111					MPEG-2 Transport Stream		//0x7

		*NOTE*: packet_types other than 000 (0x0) or 100 (0x4) are SUSPECT, and may indiciate corruption in the BBP FEC frame or BBP de-encapsulation.
	 */
	//0x0
	uint32_t	total_alp_packet_type_ip_packets_count;
	uint64_t	total_alp_packet_type_ip_packets_bytes_read;
	
	//0x4
	uint32_t	total_alp_packet_type_link_layer_signalling_packets_count;
	uint64_t	total_alp_packet_type_link_layer_signalling_bytes_read;
	
	//USUALLY not present in most ATSC 3.0 use-cases
	//0x2
	uint32_t	total_alp_packet_type_packet_compressed_ip_packet_count;
	uint64_t	total_alp_packet_type_packet_compressed_ip_packet_bytes_read;

	//0x6
	uint32_t	total_alp_packet_type_packet_type_extension_count;
	uint64_t	total_alp_packet_type_packet_type_extension_bytes_read;
	
	//0x7
	uint32_t	total_alp_packet_type_packet_mpeg2_transport_stream_count;
	uint64_t	total_alp_packet_type_packet_mpeg2_transport_stream_bytes_read;
	
	//SHOULD NEVER be present according to A330:2019
	//0x1, 0x3, 0x5
	uint32_t	total_alp_packet_type_reserved_count;
	uint64_t	total_alp_packet_type_reserved_bytes_read;
	
} atsc3_sl_tlv_payload_metrics_t;

atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload_parse_from_block_t(block_t* atsc3_sl_tlv_payload_unparsed_block_t);
atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload_parse_from_block_t_with_metrics(block_t* atsc3_sl_tlv_payload_unparsed_block_t, atsc3_sl_tlv_payload_metrics_t* atsc3_sl_tlv_payload_metrics);

void atsc3_sl_tlv_payload_free(atsc3_sl_tlv_payload_t** atsc3_sl_tlv_payload_p);
void atsc3_sl_tlv_payload_dump(atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload);

void atsc3_sl_tlv_payload_metrics_dump(atsc3_sl_tlv_payload_metrics_t* atsc3_sl_tlv_payload_metrics);

#define __SONY_TLV_DEMOD_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __SONY_TLV_DEMOD_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __SONY_TLV_DEMOD_INFO(...)    if(_SONY_TLV_DEMOD_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }

#define __SONY_TLV_DEMOD_DEBUG(...)   if(_SONY_TLV_DEMOD_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __SONY_TLV_DEMOD_TRACE(...)   if(_SONY_TLV_DEMOD_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#if defined (__cplusplus)
}
#endif

#endif /* ATSC3_SONY_TLV_DEMOD_TYPE_H_ */
