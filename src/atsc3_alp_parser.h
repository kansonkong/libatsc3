/*
 * atsc3_alp_parser.h
 *
 *  Created on: May 1, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_ALP_PARSER_H_
#define ATSC3_ALP_PARSER_H_

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"
#include "atsc3_alp_types.h"
#include "atsc3_stltp_types.h"

#if defined (__cplusplus)
extern "C" {
#endif

void atsc3_alp_parse_stltp_baseband_packet(atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet);



#if defined (__cplusplus)
}
#endif

#define __ALP_PARSER_ERROR(...)  		printf("%s:%d:ERROR: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("%s%s","\r","\n")
#define __ALP_PARSER_WARN(...)  		printf("%s:%d:WARN : ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("%s%s","\r","\n")

#define __ALP_PARSER_INFO(...)  		if(_ALP_PARSER_INFO_ENABLED) { printf("%s:%d:INFO:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("%s%s","\r","\n"); }
#define __ALP_PARSER_DEBUG(...)  		if(_ALP_PARSER_DEBUG_ENABLED) { printf("%s:%d:DEBUG: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("%s%s","\r","\n"); }

#endif /* ATSC3_ALP_PARSER_H_ */
