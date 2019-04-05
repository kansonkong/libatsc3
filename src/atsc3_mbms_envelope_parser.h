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
#include "atsc3_mime_multipart_related_parser.h"
#include "atsc3_sls_metadata_fragment_types_parser.h"

uint32_t* atsc3_mbms_envelope_find_toi_from_fdt(atsc3_fdt_instance_t* atsc3_fdt_instance);

//atsc3_mbms_metadata_envelope_t* atsc3_mbms_envelope_parse_from_fdt(atsc3_fdt_instance_t* atsc3_fdt_instance);
atsc3_sls_metadata_fragments_t* atsc3_mbms_envelope_to_sls_metadata_fragments_parse_from_fdt_fp(FILE* atsc3_fdt_instance_fp);





#define _ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);



#endif /* ATSC3_MBMS_ENVELOPE_PARSER_H_ */
