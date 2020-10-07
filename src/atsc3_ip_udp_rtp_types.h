//
//  atsc3_ip_udp_rtp_parser_types.h
//  libatsc3
//
//  Created by Jason Justman on 7/14/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

#include "atsc3_udp.h"

#ifndef ATSC3_IP_UDP_RTP_PARSER_TYPES_H
#define ATSC3_IP_UDP_RTP_PARSER_TYPES_H


/*
 * A/324 Table 8.2
 *
  	seconds_pre shall carry a value equal to the 22 least significant bits (LSBs) of the seconds field of the Bootstrap_Timing_Data (), described in Table 8.3.

	a-milliseconds_pre shall carry a 10-bit value identical to the value contained in the 3rd through 12 MSBs of the nanoseconds field of the Bootstrap_Timing_Data (), described in Table 8.3.

	Note that the a-millisecond_pre value is used in the RTP Header Timestamp only as an identifier of the Reference Emission Time of the Frame in which its contents belong; consequently, the
	somewhat longer Period of an a-millisecond_pre relative to precisely one millisecond is immaterial for this use.
 */

typedef struct atsc3_rtp_ctp_header_timestamp {
	uint32_t	seconds_pre:22;
	uint16_t	a_milliseconds_pre:10;
} atsc3_rtp_ctp_header_timestamp_t;

/*
 * jjustman-2020-08-31 - do NOT change the size of this struct or it will break STLTP de-packetization
 *
 * A324/2020 CS266r61 CTP field requirements
 * version: 		 '10'
 * CSRC_count: 	 	 '0000'
 * protocol_version: '10'
 *
 * payload_type:	one of:
 * 						 DSTP  (81: 101 0001)
 * 						 ALP   (82: 101 0010)
 * 						 STLTP (97: 110 0001)
 */

enum ATSC3_RTP_CTP_PAYLOAD_TYPE {
	ATSC3_RTP_CTP_PAYLOAD_TYPE_DSTP    						  = 81, //'101 0001'
	ATSC3_RTP_CTP_PAYLOAD_TYPE_ALPTP   					      = 82, //'101 0010'
	ATSC3_RTP_CTP_PAYLOAD_TYPE_STLTP  				 		  = 97,	//'110 0001'

	ATSC3_RTP_CTP_PAYLOAD_TYPE_STLTP_INNER_TIMING_MANAGEMENT  = 76,	//'100 1100'
	ATSC3_RTP_CTP_PAYLOAD_TYPE_STLTP_INNER_PREAMBLE  		  = 77,	//'100 1101'
	ATSC3_RTP_CTP_PAYLOAD_TYPE_STLTP_INNER_BASEBAND_PACKET 		  = 78,	//'100 1110'

	ATSC3_RTP_CTP_PAYLOAD_TYPE_UNKNOWN = 127  //'111 1111'
};


typedef struct atsc3_rtp_ctp_header {
    uint8_t 	version:2;		//'10'
    uint8_t 	padding:1;
    uint8_t 	extension:1;
    uint8_t 	csrc_count:4;	//'0000'
    uint8_t 	marker:1;
    uint8_t		payload_type:7;

    uint16_t 	sequence_number:16;

    uint32_t 							timestamp_min;	//DSTP OR ALPTP: timestamp_min: Table 6.3,
    uint32_t 							timestamp;		//STLTP: timestamp() table 9.2, otherwise reserved
    atsc3_rtp_ctp_header_timestamp_t	timestamp_bootstrap_timing_data_timestamp_short_reference; //only for STLTP inner types

    uint8_t		protocol_version:2; //'10' as per A/324:2020

    /* STLTP only */
    uint8_t		redundancy:2;
    uint8_t		number_of_channels:2;
    /* DSTP or ALPTP: 14 bits, STLTP: 10 bits */
    uint16_t	reserved:14;

    uint16_t 	packet_offset:16;
} atsc3_rtp_ctp_header_t;

typedef struct atsc3_ip_udp_rtp_ctp_packet {
    udp_flow_t             		udp_flow;
    atsc3_rtp_ctp_header_t*     rtp_ctp_header;
    bool                   		is_in_marker;

    //note - data will be the payload after the following packet headers removed:
    //[ethernet, ip, udp, rtp]
    block_t*                	data;
    
} atsc3_ip_udp_rtp_ctp_packet_t;

#endif /* ATSC3_IP_UDP_RTP_PARSER_TYPES_H */
