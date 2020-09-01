	/*
 * atsc3_stltp_depacketizer.h
 *
 *  Created on: Aug 11, 2020
 *      Author: jjustman
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#ifndef ATSC3_STLTP_DEPACKETIZER_H_
#define ATSC3_STLTP_DEPACKETIZER_H_

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"
#include "atsc3_listener_udp.h"
#include "atsc3_ip_udp_rtp_parser.h"
#include "atsc3_stltp_depacketizer_context.h"

#include "atsc3_stltp_parser.h"
#include "atsc3_alp_types.h"
#include "atsc3_alp_parser.h"


#if defined (__cplusplus)
extern "C" {
#endif

#define ATSC3_STLTP_MULTICAST_RANGE_MIN 4009754624
#define ATSC3_STLTP_MULTICAST_RANGE_MAX 4026531839

void atsc3_stltp_depacketizer_from_ip_udp_rtp_packet(atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet, atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context);
bool atsc3_stltp_depacketizer_from_blockt(block_t** packet_p, atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context);
void atsc3_stltp_depacketizer_from_pcap_frame(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet, atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context);

uint8_t atsc3_stltp_depacketizer_context_get_plp_from_context(atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context);


#if defined (__cplusplus)
}
#endif


#define _ATSC3_STLTP_DEPACKETIZER_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_STLTP_DEPACKETIZER_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_STLTP_DEPACKETIZER_INFO(...)    if(_ATSC3_STLTP_DEPACKETIZER_INFO_ENABLED)  {__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);   }
#define _ATSC3_STLTP_DEPACKETIZER_DEBUG(...)   if(_ATSC3_STLTP_DEPACKETIZER_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_STLTP_DEPACKETIZER_TRACE(...)   if(_ATSC3_STLTP_DEPACKETIZER_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#endif /* ATSC3_STLTP_DEPACKETIZER_H_ */
