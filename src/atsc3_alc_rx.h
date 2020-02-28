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


typedef struct alc_packet {
	atsc3_def_lct_hdr_t* def_lct_hdr;
	uint8_t fec_encoding_id;
    
    //for fec_encoding_id == 128, raptor fec
    bool use_sbn_esi;
    uint8_t sbn;	//sbn: source block number for fec recovery
	uint32_t esi; 	//esi: encoding symbol id, our 24bit offset
    
    //for all other fec values
    bool use_start_offset;
    uint32_t start_offset;
    
    uint8_t close_object_flag;
    uint8_t close_session_flag;

    bool 	 ext_route_presentation_ntp_timestamp_set;
    uint64_t ext_route_presentation_ntp_timestamp;
    
    unsigned int alc_len;
    unsigned long long transfer_len;

	uint8_t* alc_payload;

} alc_packet_t;


int alc_rx_analyze_packet_a331_compliant(char *data, int len, alc_packet_t** alc_packet_ptr);

void alc_packet_free(alc_packet_t** alc_packet_ptr);


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

