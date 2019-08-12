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

#define ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER 1024
#define ATSC3_MIME_MULTIPART_RELATED_PAYLOAD_BUFFER 65535

//this is a bit of a misnomer, its actually the multipart mbms and child envelope parers..
atsc3_sls_metadata_fragments_t* atsc3_mbms_envelope_to_sls_metadata_fragments_parse_from_fdt_fp(FILE* atsc3_fdt_instance_fp);

atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_parser(FILE* fp);

void atsc3_mime_multipart_related_instance_dump(atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance);



#ifdef __cplusplus
}
#endif


#define __MIME_PARSER_PRINTLN(...)  printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __MIME_PARSER_ERROR(...)  	printf("%s:%d:ERROR:",__FILE__,__LINE__);__MIME_PARSER_PRINTLN(__VA_ARGS__);
#define __MIME_PARSER_WARN(...)   	printf("%s:%d:WARN:",__FILE__,__LINE__);__MIME_PARSER_PRINTLN(__VA_ARGS__);
#define __MIME_PARSER_INFO(...)   	if(_MIME_PARSER_INFO_ENABLED)  { printf("%s:%d:INFO:",__FILE__,__LINE__);__MIME_PARSER_PRINTLN(__VA_ARGS__); }

#define __MIME_PARSER_DEBUG(...)   	if(_MIME_PARSER_DEBUG_ENABLED) { printf("%s:%d:DEBUG:",__FILE__,__LINE__);__MIME_PARSER_PRINTLN(__VA_ARGS__); }
#define __MIME_PARSER_TRACE(...)  	if(_MIME_PARSER_TRACE_ENABLED) { printf("%s:%d:TRACE:",__FILE__,__LINE__);__MIME_PARSER_PRINTLN(__VA_ARGS__); }


#endif /* ATSC3_MIME_MULTPART_RELATED_PARSER_H_ */
