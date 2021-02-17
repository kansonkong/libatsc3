/*
 * atsc3_mime_multpart_related_parser.h
 *
 *  Created on: Mar 25, 2019
 *      Author: jjustman
 *
 *
 *      [17] IETF: RFC 2387, “The MIME Multipart/Related Content-type”,
 *      Internet Engineering Task Force, Reston, VA, August 1998.
 *
 *      http://tools.ietf.org/html/rfc2387
 */


#ifndef ATSC3_MIME_MULTPART_RELATED_PARSER_H_
#define ATSC3_MIME_MULTPART_RELATED_PARSER_H_

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


#include "atsc3_utils.h"
#include "atsc3_mime_multipart_related.h"
#include "atsc3_sls_metadata_fragment_types.h"
#include "atsc3_sls_metadata_fragment_types_parser.h"
#include "atsc3_logging_externs.h"

//jjustman - 2019-08-20 RFC 2045 says:
//   "7bit data" refers to data that is all represented as relatively
//   short lines with 998 octets or less between CRLF line separation
//   but has been observed to be far greater in some defective signaler applications

#define ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER 8192
#define ATSC3_MIME_MULTIPART_RELATED_PAYLOAD_BLOCK_T_DEFAULT_ALLOC 8192

//this is a bit of a misnomer, its actually the multipart mbms and child envelope parers..
atsc3_sls_metadata_fragments_t* atsc3_mbms_envelope_to_sls_metadata_fragments_parse_from_fdt_fp(FILE* atsc3_fdt_instance_fp);

atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_parser(FILE* fp);

void atsc3_mime_multipart_related_instance_dump(atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance);



#ifdef __cplusplus
}
#endif


#define __MIME_PARSER_PRINTLN(...)  printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __MIME_PARSER_ERROR(...)  	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__)
#define __MIME_PARSER_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__)
#define __MIME_PARSER_INFO(...)   	if(_MIME_PARSER_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__) }

#define __MIME_PARSER_DEBUG(...)   	if(_MIME_PARSER_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__) }
#define __MIME_PARSER_TRACE(...)  	if(_MIME_PARSER_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__) }


#endif /* ATSC3_MIME_MULTPART_RELATED_PARSER_H_ */
