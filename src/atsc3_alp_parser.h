/*
 * atsc3_alp_parser.h
 *
 *  Created on: May 1, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_ALP_PARSER_H_
#define ATSC3_ALP_PARSER_H_

#include <pcap.h>
#include <string.h>

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"
#include "atsc3_alp_types.h"
#include "atsc3_stltp_types.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef struct atsc3_baseband_packet_collection {
    pcap_t* descrInject; //optional descriptor for alp injection

    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_baseband_packet_header);
    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_baseband_packet_refragmented_complete);
    
} atsc3_baseband_packet_collection_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_baseband_packet_collection, atsc3_baseband_packet_header);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_baseband_packet_collection, atsc3_baseband_packet_refragmented_complete);


void atsc3_alp_parse_stltp_baseband_packet(atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet, atsc3_baseband_packet_collection_t* atsc3_baseband_packet_collection);
    
    
void atsc3_alp_reflect_baseband_packet_collection_completed(atsc3_baseband_packet_collection_t* atsc3_baseband_packet_collection);

alp_packet_t* atsc3_alp_packet_parse(block_t* baseband_packet_payload);



#if defined (__cplusplus)
}
#endif

#define __ALP_PARSER_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __ALP_PARSER_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __ALP_PARSER_INFO(...)  if(_ALP_PARSER_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __ALP_PARSER_DEBUG(...) if(_ALP_PARSER_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }

#endif /* ATSC3_ALP_PARSER_H_ */
