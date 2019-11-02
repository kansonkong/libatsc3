/*
 * atsc3_sls_metadata_fragment_types_parser.h
 *
 *  Created on: Apr 2, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_SLS_METADATA_FRAGMENT_TYPES_PARSER_H_
#define ATSC3_SLS_METADATA_FRAGMENT_TYPES_PARSER_H_

#include "atsc3_sls_metadata_fragment_types.h"
#include "atsc3_mime_multipart_related_parser.h"

#define ATSC3_ROUTE_MBMS_ENVELOPE_TYPE	"application/mbms-envelope+xml"
#define	ATSC3_ROUTE_USD_TYPE	 		"application/route-usd+xml"
#define ATSC3_ROUTE_S_TSID_TYPE			"application/route-s-tsid+xml"
#define ATSC3_ROUTE_MPD_TYPE			"application/dash+xml"
#define ATSC3_SLS_HELD_FRAGMENT_TYPE	"application/atsc-held+xml"

atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragment_types_parse_from_mime_multipart_related_instance(atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance);

#define __SLS_METADATA_FRAGMENT_PARSER_PRINTLN(...) printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __SLS_METADATA_FRAGMENT_PARSER_ERROR(...)  	printf("%s:%d:ERROR:",__FILE__,__LINE__);__SLS_METADATA_FRAGMENT_PARSER_PRINTLN(__VA_ARGS__);
#define __SLS_METADATA_FRAGMENT_PARSER_WARN(...)   	printf("%s:%d:WARN:",__FILE__,__LINE__);__SLS_METADATA_FRAGMENT_PARSER_PRINTLN(__VA_ARGS__);
#define __SLS_METADATA_FRAGMENT_PARSER_INFO(...)   	if(_SLS_METADATA_FRAGMENT_PARSER_INFO_ENABLED)  { printf("%s:%d:INFO:",__FILE__,__LINE__);__SLS_METADATA_FRAGMENT_PARSER_PRINTLN(__VA_ARGS__); }

#define __SLS_METADATA_FRAGMENT_PARSER_DEBUG(...)   if(_SLS_METADATA_FRAGMENT_PARSER_DEBUG_ENABLED) { printf("%s:%d:DEBUG:",__FILE__,__LINE__);__SLS_METADATA_FRAGMENT_PARSER_PRINTLN(__VA_ARGS__); }
#define __SLS_METADATA_FRAGMENT_PARSER_TRACE(...)  	if(_SLS_METADATA_FRAGMENT_PARSER_TRACE_ENABLED) { printf("%s:%d:TRACE:",__FILE__,__LINE__);__SLS_METADATA_FRAGMENT_PARSER_PRINTLN(__VA_ARGS__); }

#endif /* ATSC3_SLS_METADATA_FRAGMENT_TYPES_PARSER_H_ */
