/*
 * atsc3_stltp_parser.h
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_STLTP_PARSER_H_
#define ATSC3_STLTP_PARSER_H_

#include <stdlib.h>
#include "atsc3_stltp_types.h"
#include "atsc3_logging_externs.h"


atsc3_rtp_fixed_header_t* atsc3_stltp_parse_rtp_fixed_header(uint8_t* data, uint32_t size);



//#define __STLTP_PARSER_DEBUG(...)  		if(_STLTP_PARSER_DEBUG_ENABLED) { printf("%s:%d:DEBUG:%.4f: ",__FILE__,__LINE__, gt()); printf(__VA_ARGS__); printf("\n"); }
#define __STLTP_PARSER_DEBUG(...)  		if(_STLTP_PARSER_DEBUG_ENABLED) { printf("%s:%d:DEBUG: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n"); }

#endif /* ATSC3_STLTP_PARSER_H_ */
