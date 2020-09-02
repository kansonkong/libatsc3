/*
 * atsc3_mime_multipart_related.h
 *
 *  Created on: Mar 25, 2019
 *      Author: jjustman
    */
#include <stdio.h>
#include <string.h>

#ifndef ATSC3_MIME_MULTIPART_RELATED_H_
#define ATSC3_MIME_MULTIPART_RELATED_H_

#include "atsc3_vector_builder.h"
#include "xml.h"


/**
 * Content-Type: multipart/related;
  type="application/mbms-envelope+xml";
  boundary="--boundary_at_1550614650679"


----boundary_at_1550614650679
Content-Type: application/mbms-envelope+xml
Content-Location: envelope.xml
 *
 */

typedef struct atsc3_mime_multipart_related_payload {

	char*						unsafe_content_location;				//original path as supplied in the multipart/related object
	char* 						sanitizied_content_location;			//sanitized path (e.g. no ../ or ~
	char*						content_transfer_encoding;

	//from atsc3_mbms_metadata_item_v - mapped in atsc3_route_package_extract_unsigned_payload
	char* 						content_type;							//back-patch from envelope.xml

	char* 						valid_from_string;
	char* 						valid_until_string;
	uint32_t					version;								//back-patch from envelope.xml
	char* 						next_url_string;
	char*						avail_at_string;
	//end from atsc3_mbms_metadata_item_v

	uint32_t					extracted_size;

	block_t*					payload;

} atsc3_mime_multipart_related_payload_t;

typedef struct atsc3_mime_multipart_related_instance {
	char*	type;
	char*	boundary;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_mime_multipart_related_payload);
} atsc3_mime_multipart_related_instance_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_mime_multipart_related_instance, atsc3_mime_multipart_related_payload)



#endif /* ATSC3_MIME_MULTIPART_RELATED_H_ */
