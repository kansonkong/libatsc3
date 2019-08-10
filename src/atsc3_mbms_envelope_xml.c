/*
 * atsc3_mbms_envelope_xml.c
 *
 *  Created on: Aug 10, 2019
 *      Author: jjustman
 */


#include "atsc3_mbms_envelope_xml.h"


ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_mbms_metadata_envelope)
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_mbms_metadata_envelope, atsc3_mbms_metadata_item);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_mbms_metadata_item);
