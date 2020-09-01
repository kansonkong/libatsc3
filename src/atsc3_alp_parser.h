/*
 * atsc3_alp_parser.h
 *
 *  Created on: May 1, 2019
 *      Author: jjustman
 */
#ifndef __DISABLE_LIBPCAP__
#include <pcap.h>
#else
//jjustman-2019-09-27 - hacks...
#define pcap_sendpacket(...) (0)
#define pcap_geterr(...) ("")
#endif

#include <string.h>

#ifndef ATSC3_ALP_PARSER_H_
#define ATSC3_ALP_PARSER_H_

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"
#include "atsc3_alp_types.h"
#include "atsc3_stltp_types.h"

#if defined (__cplusplus)
extern "C" {
#endif

atsc3_baseband_packet_t* atsc3_stltp_parse_baseband_packet(atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet);
atsc3_alp_packet_t* atsc3_alp_packet_parse(uint8_t plp_num, block_t* baseband_packet_payload);
void atsc3_alp_packet_packet_set_bootstrap_timing_ref_from_baseband_packet(atsc3_alp_packet_t* atsc3_alp_packet, atsc3_baseband_packet_t* atsc3_baseband_packet);


void atsc3_alp_packet_collection_extract_lmt(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection);
atsc3_link_mapping_table_t* atsc3_alp_packet_extract_lmt(atsc3_alp_packet_t* atsc3_alp_packet);

void atsc3_reflect_alp_packet_collection(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection, pcap_t* atsc3_baseband_alp_output_pcap_device_reference);

#if defined (__cplusplus)
}
#endif

#define __ALP_PARSER_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __ALP_PARSER_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __ALP_PARSER_INFO(...)  if(_ALP_PARSER_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __ALP_PARSER_DEBUG(...) if(_ALP_PARSER_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }

#endif /* ATSC3_ALP_PARSER_H_ */
