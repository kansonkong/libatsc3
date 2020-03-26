/*
 * atsc3_dstp_types.h
 *
 *  Created on: Sep 10, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_DSTP_TYPES_H_
#define ATSC3_DSTP_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#ifdef __ANDROID__
#include <bits/in_addr.h>
#endif
#include <arpa/inet.h>

#include "atsc3_utils.h"
#include "atsc3_vector_builder.h"
#include "atsc3_logging_externs.h"
#include "atsc3_listener_udp.h"
#include "atsc3_ip_udp_rtp_types.h"
#include "atsc3_ip_udp_rtp_parser.h"
#include "atsc3_baseband_packet_types.h"

#if defined (__cplusplus)
extern "C" {
#endif

/*
 * A/324:2018 5 Jan 2018
 *

Table 6.1 RTP Header Field Definitions for Data Source Transport Protocol

Syntax					No. of Bits		Format
---------------------	-----------		------
RTP_Fixed_Header() {
	version (V) 				  2		'10' 		bit(0)
	padding (P) 				  1		bslbf
	extension (X) 				  1		bslbf
	CSRC_count (CC) 			  4		'0000'

	marker (M) 				 	  1		'0'			bit(1)
	payload_type (PT) 			  7		uimsbf

	sequence_number 			 16		uimsbf  	bits(2-3)

	timestamp_min() 			 32	  	Table 7.3	bits(4-7)

	timestamp_max()				 32	  	Table 7.3	bits(8-11)
}


Please refer to RFC 3550 [6] for base definitions of the fields described in Table 6.1.

	version – Indicates the version number of the RTP protocol. ‘10’ is currently defined by [6].

	padding – Indicates, when set to ‘1’, that padding is included in the payload. No padding is included when the value is ‘0’. Padding shall be supported as specified in [6].

	extension – Indicates, when set to ‘1’, that an RTP extension follows the header. When set to ‘0’,
	no header extension is included. No extensions are required by this specification; however, use of the standard extension mechanism shall be allowed. In other words, processors of DSTP must not preclude use of extensions.

	CSRC_count – Indicates the number of additional contributing source identifiers (CSRCs). No additional CSRCs are involved in this application; therefore, this field shall be set to ‘0000’.

	marker – A bit indicating a payload start. This bit shall be set to ‘0’ since packet fragmentation is not performed at this layer.

	payload_type –The payload_type encoding defined here spans payload_type values from 80 (0x50) through 95 (0x5f), which all are in the undefined range documented in [7]. The value of the payload_type for DSTP shall be as defined in Table 6.2.

Table 6.2 DSTP RTP Packet payload_type Encoding

Syntax 					No. of Bits 	Format
----------------------	-----------		------
payload_type() {
	prefix						  3		'101'
	reserved 					  2		'11'
	wakeup_control()			  2		Section 6.4
}

	wakeup_control() – A 2-bit field that communicates information needed to control the emission wakeup field. See Section 6.4 for a definition of this field.
	Note that the wakeup_control() field shall be ignored for packets with addresses and ports other than 224.0.23.60 and 4937,
	respectively.

	sequence_number – As per [6], the sequence_number shall increment by one, modulo 216 or 65536, for each packet with the same IP address and port Tuple. The initial sequence_number should be randomized, although this is not important in this setting since the stream will not be restarted frequently nor is it expected to be on a public network. Since UDP/IP does not guarantee ordered delivery and may even duplicate packets, the sequence_number can be used to reconstitute any stream as produced and to detect loss, allowing a backup stream to be selected.

	timestamp_min – Defined by Table 6.3, this value shall specify the earliest time at which the start of the payload may be delivered. A value of ‘0’ shall indicate that the packet may be delivered
	with best effort.

	timestamp_max – Defined in Table 6.3, this value shall indicate the latest time at which the start of
	the payload may be delivered. A value of ‘0’ shall denote that the packet may be delivered with best effort. Note that this field replaces the SSRC_ID field specified in RFC 3550 [6].

Table 6.3 Timestamp Field Definitions for Data Source Transport

Syntax			    No. of Bits		Format
------------------	----------		------
timestamp () {
	seconds			16				uimsbf
	fraction		16				uimsbf
}

	timestamp fields shall be formatted according to the short-form of NTP specified in RFC 5905 [15].

	seconds shall carry a value equal to the 16 least significant bits (LSBs) of the seconds portion of
	the UTC time value of the targeted Bootstrap Reference Emission Time.
	fraction shall carry a 16-bit fractional seconds value of the UTC time of the targeted Bootstrap

	Reference Emission Time—allowing a resolution of approximately 15 microseconds.

	The timestamp fields within the RTP header allow a Data Source system to specify schedule constraints on delivery of packets.
	Presumably, this scheduling information would be communicated somehow from upstream systems.
	Please note that NTP is based on UTC and thus is adjusted for leap seconds. There is no adjustment for leap seconds in
	emission timing, which is based purely on TAI seconds and fractional seconds.
	See Section 8.3.2 for more information on Bootstrap emission timing.


6.4 Emergency Alert Wakeup RTP Controls

	Emergency Alert Wakeup RTP fields in DSTP provide signaling information that indicates to the Scheduler how to manage the
	ea_wake_up_1 and ea_wake_up_2 bits in the Bootstrap. In the text that follows, these two bits are treated together as the
	“Emission Wakeup Field”. There are two flags provided in the wakeup_control() field.

	The first, wakeup_active, indicates that a currently-active Advanced Emergency Alert (AEA) message has the @wakeup
	attribute set to “true”. The flag shall reflect this condition on the DSTP RTP headers of all LLS packets generated
	by a given source, not just on packets containing an Advanced Emergency Alert Table (AEAT).

	In contrast, the AEAT_wakeup_alert
	flag shall reflect specific values within the current packet and apply only to packets containing an AEAT.

Table 6.4 RTP payload_type Wakeup Control Field Definition

Syntax			    	No. of Bits		Format
-----------------------	----------		------
wakeup_control() {
	wakeup_active		1				bslbf
	AEAT_wakeup_alert	1				bslbf
}


	wakeup_active – A 1-bit flag that, when set to ‘1’, shall indicate that the source providing the LLS data is
	requesting that the Emission Wakeup Field be non-zero. When set to ‘0’, this flag shall indicate that the
	source providing the LLS data is requesting that the Emission Wakeup Field be set to a value of zero if no
	other source is requesting a non-zero value.

	AEAT_wakeup_alert – A 1-bit flag that, when set to ‘1’, shall indicate that the packet contains a new or updated
	AEAT that carries a new Wakeup Alert. When set to ‘0’, it shall indicate that the packet does not contain an
	AEAT with a new Wakeup Alert.

	This bit shall only be ‘1’ when the LLS_table_version of the AEAT LLS_table() has been incremented and
	the AEAT contains an AEA element with the @wakeup attribute newly set to ‘true’.
	In other words, if a new AEA is added with @wakeup set to ‘true’ or a previous AEA has been updated with
	@wakeup set to ‘true’, then the LLS_table_version will be incremented and the AEAT_wakeup_alert shall be set to ‘1’.

	The wakeup_active field controls whether the Emission Wakeup Field is ‘off’ (zero) or ‘on’ (non-zero).
	AEAT_wakeup_alert controls when the Emission Wakeup Field toggles through the ‘on’ (non-zero) states, i.e., 1, 2, and 3.
	(See Table 6.5 for an example of wakeup_active and AEAT_wakeup_alert usage.)

	It is expected that LLS tables, such as the SLT or SystemTime tables, will be updated relatively frequently,
	allowing timely control. There is no restriction, however, on providing extra LLS signaling to accommodate updating
	the Emission Wakeup Field. For example, the Data Source could emit an SLT specifically to return the Emission Wakeup Field
	value to zero.

Table 6.5 Example Wakeup Bit Controls


Time	LLS Table					Wakeup 	wakeup_active	AEAT_wakeup_alert	Emission Wakeup
									State	 									Field
----	---------					------	-------------	-----------------	---------------
t0		SLT	       					OFF		0				0					00
t1      SystemTime 		 			OFF		0				0					00
t2      New AEA @wakeup=true		ON		1				1					01
t3		SLT							ON		1				0					01
t4		Same AEA					ON		1				0					01
t5		SystemTime					ON		1				0					01
t6		Updated AEA @wakeup=true	ON		1				1					10
t7		SLT							ON		1				0					10
t8		SLT							ON*		1*				0*					10*
t9    	New AEA @wakeup=false		OFF		0				0					00


* jjustman-2019-09-10 - editorial correction

 Note that this example is for a single source of LLS.
 If there is more than one source of LLS, the Scheduler is responsible for combining those controls
 and setting the Emission Wakeup Field appropriately.

 *
 */

typedef struct atsc3_rtp_dstp_wakeup_control {
	uint8_t		wakeup_active:1;
	uint8_t		aeat_wakeup_alert:1;
} atsc3_rtp_dstp_wakeup_control_t;

typedef struct atsc3_rtp_dstp_payload_type {
	  uint8_t							prefix:3;	 //101
	  uint8_t							reserved:2; //11
	  atsc3_rtp_dstp_wakeup_control_t	wakeup_control;
} atsc3_rtp_dstp_payload_type_t;

typedef struct atsc3_rtp_dstp_timestamp_type {
	uint16_t	seconds;
	uint16_t	fraction;
} atsc3_rtp_dstp_timestamp_type_t;

typedef struct atsc3_rtp_dstp_header {
    uint8_t								version:2;			//10
    uint8_t 							padding:1;			//0
    uint8_t 							extension:1;		//0
    uint8_t 							csrc_count:4;		//0

    uint8_t 							marker:1;			//0
    atsc3_rtp_dstp_payload_type_t 		payload_type;

    uint16_t							sequence_number;

    atsc3_rtp_dstp_timestamp_type_t		timestamp_min;

    atsc3_rtp_dstp_timestamp_type_t 	timestamp_max;
} atsc3_rtp_dstp_header_t;

typedef struct atsc3_ip_udp_rtp_dstp_packet {
    udp_flow_t             		udp_flow;
    atsc3_rtp_dstp_header_t*    rtp_header;
    block_t*               		data;
} atsc3_ip_udp_rtp_dstp_packet_t;

//cctor's

atsc3_rtp_dstp_header_t* atsc3_rtp_dstp_header_new();
atsc3_ip_udp_rtp_dstp_packet_t* atsc3_ip_udp_rtp_dstp_packet_new();
atsc3_ip_udp_rtp_dstp_packet_t* atsc3_ip_udp_rtp_dstp_packet_new_from_flow(udp_flow_t* udp_flow);

//writer methods

block_t* atsc3_rtp_dstp_header_write_to_block_t(atsc3_rtp_dstp_header_t* atsc3_rtp_dstp_header);
block_t* atsc3_ip_udp_rtp_dstp_write_to_eth_phy_packet_block_t(atsc3_ip_udp_rtp_dstp_packet_t* atsc3_ip_udp_rtp_dstp_packet);

//destructors

void atsc3_rtp_dstp_header_free(atsc3_rtp_dstp_header_t** atsc3_rtp_dstp_header_p);
void atsc3_ip_udp_rtp_dstp_packet_free(atsc3_ip_udp_rtp_dstp_packet_t** atsc3_ip_udp_rtp_dstp_packet_p);


#if defined (__cplusplus)
}
#endif

#define __DSTP_TYPES_ERROR(...)    __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS_);
#define __DSTP_TYPES_WARN(...)     __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __DSTP_TYPES_INFO(...)     __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define __DSTP_TYPES_DEBUG(...)    if(_DSTP_TYPES_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __DSTP_TYPES_TRACE(...)    if(_DSTP_TYPES_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#endif
