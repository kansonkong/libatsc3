/*
 * atsc3_mbms_envelope_xml.c
 *
 *  Created on: Aug 10, 2019
 *      Author: jjustman
 */


#include "atsc3_mbms_envelope_xml.h"


ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_mbms_metadata_envelope)
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_mbms_metadata_envelope, atsc3_mbms_metadata_item);

//ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_mbms_metadata_item);
void atsc3_mbms_metadata_item_free(atsc3_mbms_metadata_item_t** atsc3_mbms_metadata_item_p) {
    if(atsc3_mbms_metadata_item_p) {
        atsc3_mbms_metadata_item_t* atsc3_mbms_metadata_item = *atsc3_mbms_metadata_item_p;
        if(atsc3_mbms_metadata_item) {
            
            freeclean((void**)&atsc3_mbms_metadata_item->content_type);
            freeclean((void**)&atsc3_mbms_metadata_item->metadata_uri);
            freeclean((void**)&atsc3_mbms_metadata_item->valid_from_string);
            freeclean((void**)&atsc3_mbms_metadata_item->valid_until_string);
            freeclean((void**)&atsc3_mbms_metadata_item->next_url_string);
            freeclean((void**)&atsc3_mbms_metadata_item->avail_at_string);
            
            free(atsc3_mbms_metadata_item);
            atsc3_mbms_metadata_item = NULL;
        }
        *atsc3_mbms_metadata_item_p = NULL;
    }
}
