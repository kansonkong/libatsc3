/*
 * atsc3_stltp_parser.h
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_STLTP_PARSER_H_
#define ATSC3_STLTP_PARSER_H_

#include <assert.h>
#include <stdlib.h>
#include "atsc3_utils.h"
#include "atsc3_stltp_types.h"
#include "atsc3_logging_externs.h"


#if defined (__cplusplus)
extern "C" {
#endif

atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_extract_fragment(uint8_t* raw_packet_data, uint32_t raw_packet_length, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_fragment);
atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_fragment);

atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_tunnel);
atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet, atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header_tunnel);


atsc3_rtp_fixed_header_t* atsc3_stltp_parse_rtp_fixed_header(uint8_t* raw_packet_data, uint32_t raw_packet_length);

void atsc3_rtp_fixed_header_dump(atsc3_rtp_fixed_header_t* atsc3_rtp_fixed_header);


#if defined (__cplusplus)
}
#endif

#define __STLTP_PARSER_ERROR(...)  		printf("%s:%d:ERROR: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n");
#define __STLTP_PARSER_WARN(...)  		printf("%s:%d:WARN : ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n");

//#define __STLTP_PARSER_DEBUG(...)  		if(_STLTP_PARSER_DEBUG_ENABLED) { printf("%s:%d:DEBUG:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n"); }
#define __STLTP_PARSER_DEBUG(...)  		if(_STLTP_PARSER_DEBUG_ENABLED) { printf("%s:%d:DEBUG: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n"); }

#endif /* ATSC3_STLTP_PARSER_H_ */
