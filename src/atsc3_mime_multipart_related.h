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
	char* 						content_type;
	char* 						content_location;

	block_t*					payload;

} atsc3_mime_multipart_related_payload_t;

typedef struct atsc3_mime_multipart_related_instance {
	char*	type;
	char*	boundary;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_mime_multipart_related_payload)
} atsc3_mime_multipart_related_instance_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_mime_multipart_related_instance, atsc3_mime_multipart_related_payload)



#endif /* ATSC3_MIME_MULTIPART_RELATED_H_ */
