/*
 * atsc3_stltp_depacketizer_context.h
 *
 *  Created on: Aug 18, 2020
 *      Author: jjustman
 */

#ifndef ATSC3_STLTP_DEPACKETIZER_CONTEXT_H_
#define ATSC3_STLTP_DEPACKETIZER_CONTEXT_H_

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"
#include "atsc3_listener_udp.h"
#include "atsc3_ip_udp_rtp_parser.h"
#include "atsc3_alp_types.h"
#include "atsc3_alp_parser.h"
#include "atsc3_stltp_types.h"


#if defined (__cplusplus)
extern "C" {
#endif

#define ATSC3_STLTP_DEPACKETIZER_ALL_PLPS_VALUE 255
#define ATSC3_STLTP_DEPACKETIZER_ALL_PLPS_INNER_RTP_PORT 0

//method callbacks for depacketization for baseband / alp packets, preamble, timing and management

typedef void(*atsc3_stltp_baseband_alp_packet_collection_callback_f)(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection);
typedef void(*atsc3_stltp_baseband_alp_packet_collection_callback_with_context_f)(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection, void* context);

typedef void(*atsc3_stltp_baseband_alp_packet_collection_callback_with_pcap_device_reference_f)(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection, pcap_t* atsc3_baseband_alp_output_pcap_device_reference); //used for re-injection


typedef void(*atsc3_stltp_preamble_packet_collection_callback_f)(atsc3_stltp_preamble_packet_tv* atsc3_stltp_preamble_packet_v);
typedef void(*atsc3_stltp_timing_management_packet_collection_callback_f)(atsc3_stltp_timing_management_packet_tv* atsc3_stltp_timing_management_packet_v);


//jjustman-2020-08-11- TODO: extend this to contain the relevant bootstrap reference emission time for re-modulation

typedef struct atsc3_stltp_depacketizer_context {
    bool                            context_configured;
	udp_flow_t						destination_flow_filter;	//restrict from dst_ip_addr and dst_port
	uint16_t						inner_rtp_port_filter;		//restrict to PLP0-63 (e.g. 30000+n)

	atsc3_stltp_tunnel_baseband_packet_pending_by_plp_t* 		atsc3_stltp_tunnel_baseband_packet_pending_by_plp_collection;

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


    atsc3_stltp_preamble_packet_collection_callback_f 									atsc3_stltp_preamble_packet_collection_callback;
    atsc3_stltp_timing_management_packet_collection_callback_f 							atsc3_stltp_timing_management_packet_collection_callback;


} atsc3_stltp_depacketizer_context_t;

atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context_new();
void atsc3_stltp_depacketizer_context_free(atsc3_stltp_depacketizer_context_t** atsc3_stltp_depacketizer_context_p);

void atsc3_stltp_depacketizer_context_set_all_plps(atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context);

//forward declare atsc3_stltp_depacketizer_context_t for now for header circular resolution
void atsc3_stltp_tunnel_packet_set_baseband_packet_pending_from_inner_rtp_for_plp(atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context, atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current);
void atsc3_stltp_tunnel_packet_set_baseband_packet_pending_from_plp(atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context, uint8_t from_plp_num, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current);

#define _ATSC3_STLTP_DEPACKETIZER_CONTEXT_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);


#if defined (__cplusplus)
}
#endif

#endif /* ATSC3_STLTP_DEPACKETIZER_CONTEXT_H_ */
