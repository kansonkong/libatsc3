/*
 * atsc3_mbms_envelope_parser.h
 *
 *  Created on: Apr 5, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MBMS_ENVELOPE_PARSER_H_
#define ATSC3_MBMS_ENVELOPE_PARSER_H_

#include "atsc3_utils.h"
#include "atsc3_mbms_envelope_xml.h"
#include "atsc3_fdt.h"
#include "atsc3_sls_metadata_fragment_types.h"
#include "atsc3_logging_externs.h"

#define ATSC3_FDT_MULTIPART_RELATED "multipart/related"

uint32_t* atsc3_mbms_envelope_find_toi_from_fdt(atsc3_fdt_instance_t* atsc3_fdt_instance);

//this is the actual atsc3_mbms_metadata_envelope parser
atsc3_mbms_metadata_envelope_t* atsc3_mbms_envelope_parse_from_payload(char* payload, char* content_location);
void atsc3_mbms_metadata_envelope_dump(atsc3_mbms_metadata_envelope_t* atsc3_mbms_metadata_envelope);

#define _ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_INFO(...)    if(_ROUTE_MBMS_ENVELOPE_PARSER_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_DEBUG(...)   if(_ROUTE_MBMS_ENVELOPE_PARSER_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }



#endif /* ATSC3_MBMS_ENVELOPE_PARSER_H_ */
