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
#include "atsc3_logging_externs.h"

#include "atsc3_ip_udp_rtp_parser.h"
#include "atsc3_stltp_depacketizer_context.h"

#include "atsc3_stltp_types.h"

#if defined (__cplusplus)
extern "C" {
#endif

//outer/inner ip_udp_rtp packet parsers
atsc3_ip_udp_rtp_packet_t* atsc3_stltp_udp_packet_outer_fragment_check_marker(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet);
    
//inner packet parsers
atsc3_ip_udp_rtp_packet_t* atsc3_stltp_udp_packet_inner_prepend_fragment(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet);
atsc3_ip_udp_rtp_packet_t* atsc3_stltp_tunnel_packet_inner_parse_ip_udp_header(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet);
atsc3_stltp_tunnel_packet_t* atsc3_stltp_raw_packet_extract_inner_from_outer_packet(atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context, atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_fragment);

//helper method for extraction of inner packet concrete types
bool atsc3_stltp_tunnel_packet_extract_fragment_encapsulated_payload(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current);
    
//extraction of our inner packet concrete types
atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current);
atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current);
atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet_extract(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current);

//parse to concrete types
atsc3_preamble_packet_t* atsc3_stltp_parse_preamble_packet(atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet);
void atsc3_preamble_packet_set_bootstrap_timing_ref_from_baseband_packet(atsc3_preamble_packet_t* atsc3_preamble_packet, atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet);

atsc3_timing_management_packet_t* atsc3_stltp_parse_timing_management_packet(atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet);

//utils
void atsc3_baseband_packet_dump(atsc3_baseband_packet_t* atsc3_baseband_packet);
void atsc3_preamble_packet_dump(atsc3_preamble_packet_t* atsc3_preamble_packet);
void atsc3_timing_management_packet_dump(atsc3_timing_management_packet_t* atsc3_timing_management_packet);

#define __STLTP_PARSER_ERROR(...)       __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __STLTP_PARSER_WARN(...)        __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __STLTP_PARSER_INFO(...)        if(_STLTP_PARSER_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);  }
#define __STLTP_PARSER_DUMP(...)        if(_STLTP_PARSER_DUMP_ENABLED)  { __LIBATSC3_TIMESTAMP_DUMP(__VA_ARGS__);  }
#define __STLTP_PARSER_DEBUG(...)       if(_STLTP_PARSER_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __STLTP_PARSER_TRACE(...)       if(_STLTP_PARSER_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#if defined (__cplusplus)
}
#endif

#endif /* ATSC3_STLTP_PARSER_H_ */
