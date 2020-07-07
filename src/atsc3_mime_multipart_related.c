/*
 * atsc3_mime_multipart_related.c
 *
 *  Created on: Mar 25, 2019
 *      Author: jjustman
 */

#include "atsc3_mime_multipart_related.h"

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_mime_multipart_related_instance, atsc3_mime_multipart_related_payload)
//ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_mime_multipart_related_payload);
void atsc3_mime_multipart_related_payload_free(atsc3_mime_multipart_related_payload_t** atsc3_mime_multipart_related_payload_p) {
    if(atsc3_mime_multipart_related_payload_p) {
        atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = *atsc3_mime_multipart_related_payload_p;
        if(atsc3_mime_multipart_related_payload) {
            freeclean((void**)&atsc3_mime_multipart_related_payload->content_location);
            freeclean((void**)&atsc3_mime_multipart_related_payload->content_type);
            if(atsc3_mime_multipart_related_payload->payload) {
            	block_Destroy(&atsc3_mime_multipart_related_payload->payload);
            }
          
            free(atsc3_mime_multipart_related_payload);
            atsc3_mime_multipart_related_payload = NULL;
        }
        *atsc3_mime_multipart_related_payload_p = NULL;
    }
}

