/*
* atsc3_sl_tlv_demod_type.h
*
*  Created on: Nov 22, 2019
*      Author: jjustman
*
*   process SL TLV demod format payload
*/

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>


#ifndef ATSC3_SL_TLV_DEMOD_TYPE_H_
#define ATSC3_SL_TLV_DEMOD_TYPE_H_


#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"

#if defined (__cplusplus)
extern "C" {
#endif


/**
 
 Ref: SL SDK API v0.5
 
 2 Demodulator Output Data Format
 --------------------------------
 When the selected standard is ATSC 3.0, the data coming out of selected output interface is ALP data.
 There is a metadata packet that is transferred before the ALP packet.
 The structure of the packet is described below.
 
 The fields of this metadata structure contain both information and diagnostic data.

 Byte       Size        Value               Description
 -------    ------      --------            ------------------
 B0 – B3    4 Bytes     0x24681357          Magic number indicating the start of the Meta data of the TS packet.
 B4 – B7    4 Bytes     Positive value      Size of the ALP Packet in bytes
 B8         1 Byte      0 – 63              PLP number
 B9         1 Byte      188                 TS transfer size in bytes
 B10        1 Byte      24                  TLV header size in bytes
 B11        1 Byte      Positive number     Padding at the end of the ALP packet in bytes
 B12 – B15  4 Bytes     Reserved            Reserved for future use
 B16        1 Byte      Reserved            Reserved for future use
 B17 – B19  3 Bytes     Reserved            Reserved for future use
 B20 – B23  4 Bytes     Reserved            Reserved for future use
 
For other broadcast standards (ATSC1, DVB-T2, ISMB-T etc), the data is in standard MPEG2 Transport stream format.
 */

typedef struct atsc3_sl_tlv_payload {
    uint32_t    magic_number;               //0x24681357
    uint32_t    alp_packet_size;
    uint8_t     plp_number;
    uint8_t     ts_size;                    //jjustman: 2019-11-22: default should be 188
    uint8_t     tlv_size;                   //jjustman: 2019-11-22: default should be 24
    uint8_t     alp_trailing_padding_size;  //padding at the end of the ALP packet in bytes
    uint32_t    reserved_b12_b15;           //reserved b12-b15
    uint8_t     reserved_b16;               //reserved b16
    uint32_t    reserved_b17_b19;           //reserved b17-b19
    uint32_t    reserved_b20_b23;           //reserved b20-b23
    block_t*    alp_payload;                //extracted ALP payload
} atsc3_sl_tlv_payload_t;

atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload_parse_from_block_t(block_t* atsc3_sl_tlv_payload_unparsed_block_t);
void atsc3_sl_tlv_payload_free(atsc3_sl_tlv_payload_t** atsc3_sl_tlv_payload_p);
void atsc3_sl_tlv_payload_dump(atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload);


#define __SL_TLV_DEMOD_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __SL_TLV_DEMOD_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __SL_TLV_DEMOD_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define __SL_TLV_DEMOD_DEBUG(...)   if(_SL_TLV_DEMOD_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __SL_TLV_DEMOD_TRACE(...)   if(_SL_TLV_DEMOD_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#if defined (__cplusplus)
}
#endif

#endif /* ATSC3_SL_TLV_DEMOD_TYPE_H_ */
