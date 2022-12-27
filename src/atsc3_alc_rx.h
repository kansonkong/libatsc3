/*
 * atsc3_alc_rx.h
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */


#include <stdbool.h>
#include <inttypes.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <assert.h>
#include <stdint.h>

#include "atsc3_utils.h"
#include "atsc3_lct_hdr.h"
#include "atsc3_alc_session.h"




#ifndef _ALC_RX_H_
#define _ALC_RX_H_

#ifdef __cplusplus
extern "C" {
#endif

	/*
	 *
	 * Table A.3.6 Defined Values of Codepoint Field of LCT Header
Codepoint value (CP)
0 1 2 3
4
5
6
7
8
9
10 – 127 128 – 255
Semantics
ATSC Reserved (not used) NRT- File Mode
NRT – Entity Mode
NRT – Unsigned Package Mode
NRT – Signed Package Mode
New IS, timeline changed New IS, timeline continued Redundant IS
Media Segment, File Mode Media Segment, Entity Mode ATSC Reserved
Attributes of this type of packet are signalled by attributes given in the SrcFlow.Payload element associated with the CodePoint vlaue
@formatId
1 (File Mode)
2 (Entity Mode)
3 (Unsigned Package Mode)
4 (Signed Package Mode)
1 (File Mode) 1
1
1
2 (Entity Mode)
Per Payload element
@frag
0 (arbitrary) 0
0
0
0
0
0
1 (sample) 1
Per Payload element
@order
true true true
true
true true true true true
Per Payload element
	 */

typedef struct atsc3_alc_lct_codepoint_mapping {
		uint8_t 	codepoint;
		uint8_t 	format_id;
		uint8_t		frag;
		bool		order;
} atsc3_alc_lct_codepoint_mapping_t;

#define ATSC3_ALC_LCT_CODEPOINT_MAPPING_TABLE_MAX_ENTRIES 10
extern atsc3_alc_lct_codepoint_mapping_t atsc3_alc_lct_codepoint_mappping_table[];

typedef struct atsc3_alc_packet {
	atsc3_def_lct_hdr_t* 	def_lct_hdr;
	uint8_t 				fec_encoding_id;
    
    //jjustman-2021-02-03 - for fec_encoding_id=6 - raptorQ
    //  all others, see A/331:2020

    bool 					use_sbn_esi;
    uint8_t 				sbn;	//sbn: source block number for fec recovery
	uint32_t 				esi; 	//esi: encoding symbol id, our 24bit offset
    
    //for all other fec values
    bool 					use_start_offset;
    uint32_t 				start_offset;
    
    uint8_t 				close_object_flag;
    uint8_t 				close_session_flag;

    bool 	 				ext_route_presentation_ntp_timestamp_set;
    uint64_t				ext_route_presentation_ntp_timestamp;
    
    unsigned int 			alc_len;
    unsigned long long 		transfer_len;

	uint8_t* 				alc_payload; //todo - use block_t

} atsc3_alc_packet_t;


int alc_rx_analyze_packet_a331_compliant(char *data, int len, atsc3_alc_packet_t** alc_packet_ptr);

void alc_packet_free(atsc3_alc_packet_t** alc_packet_ptr);


#define ALC_RX_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define ALC_RX_WARN(...)   		__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define ALC_RX_INFO(...)   		__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define ALC_RX_DEBUG(...)   	if(_ALC_RX_DEBUG_ENABLED) 		{ __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__)}

//todo: jjustman-2019-09-24 - replace va_args[0] (" ", ",", ":", with \t)
#define ALC_RX_TRACE_TAB(...)   if(_ALC_RX_TRACE_TAB_ENABLED) 	{ __LIBATSC3_TIMESTAMP_TRACE_TAB(__VA_ARGS__)}

#define ALC_RX_DEBUGF(...)  	if(_ALC_RX_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__) }
#define ALC_RX_DEBUGA(...)  	if(_ALC_RX_DEBUG_ENABLED) { __PRINTF(__VA_ARGS__); }
#define ALC_RX_DEBUGN(...)  	if(_ALC_RX_DEBUG_ENABLED) { __PRINTLN(__VA_ARGS__); }

#define ALC_RX_TRACE(...)   	if(_ALC_RX_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__)}


#ifdef __cplusplus
}; //extern "C"
#endif

#endif

