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
#include "atsc3_stltp_parser.h"
#include "atsc3_alp_types.h"
#include "atsc3_alp_parser.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef void(*atsc3_stltp_baseband_alp_packet_collection_callback_f)(uint8_t plp, atsc3_alp_packet_collection_t* atsc3_alp_packet_collection);
typedef void(*atsc3_stltp_baseband_alp_packet_collection_callback_with_context_f)(uint8_t plp, atsc3_alp_packet_collection_t* atsc3_alp_packet_collection, void* context);

typedef void(*atsc3_stltp_baseband_alp_packet_collection_callback_with_pcap_device_reference_f)(uint8_t plp, atsc3_alp_packet_collection_t* atsc3_alp_packet_collection, pcap_t* atsc3_baseband_alp_output_pcap_device_reference); //used for re-injection

//jjustman-2020-08-11- TODO: extend this to contain the relevant bootstrap reference emission time for re-modulation

typedef struct atsc3_stltp_depacketizer_context {
	udp_flow_t						destination_flow_filter;	//restrict from dst_ip_addr and dst_port
	uint16_t						inner_rtp_port_filter;		//restrict to PLP0-63 (e.g. 30000+n)

	atsc3_stltp_tunnel_packet_t* 	atsc3_stltp_tunnel_packet_processed;
	atsc3_alp_packet_collection_t* 	atsc3_alp_packet_collection;


	//callback methods for our extracted ALP packet - e.g. for a/331 processing or ethernet i/f reflection - void atsc3_reflect_alp_packet_collection(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection) {

	//no context
	atsc3_stltp_baseband_alp_packet_collection_callback_f								atsc3_stltp_baseband_alp_packet_collection_callback; //callback method for our alp packet collection for a/331 processing


	//generic context for class instance re-scoping
	atsc3_stltp_baseband_alp_packet_collection_callback_with_context_f					atsc3_stltp_baseband_alp_packet_collection_callback_with_context; //callback method for our alp packet collection for a/331 processing
	void*																				atsc3_stltp_baseband_alp_packet_collection_callback_context; //if needed for instance casting...

	//explicit pcap_t context for output baseband alp packet  reflection
	atsc3_stltp_baseband_alp_packet_collection_callback_with_pcap_device_reference_f	atsc3_stltp_baseband_alp_packet_collection_callback_with_pcap_device_reference; //callback method for re-injection from a pcap_t handle
    pcap_t*                         													atsc3_baseband_alp_output_pcap_device_reference;	//required for 2nd callback method


    /*
     * 	example:
          	pcap_lookupnet(devInject, &netpInject, &maskpInject, errbufInject);
    		pcap_t* descrInject = pcap_open_live(devInject, MAX_PCAP_LEN, 1, 1, errbufInject);
     *
     */

} atsc3_stltp_depacketizer_context_t;

atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context_new();
void atsc3_stltp_depacketizer_context_free(atsc3_stltp_depacketizer_context_t** atsc3_stltp_depacketizer_context_p);


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
