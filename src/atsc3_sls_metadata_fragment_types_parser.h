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

atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragment_types_parse_from_mime_multipart_related_instance(atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance);

#endif /* ATSC3_SLS_METADATA_FRAGMENT_TYPES_PARSER_H_ */
