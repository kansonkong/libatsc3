/*
 * atsc3_a332_oma_bcast_types.c
 *
 *  Created on: Nov 17, 2020
 *      Author: jjustman
 */


#include "atsc3_oma_bcast_types.h"


ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_service_guide_delivery_unit, atsc3_service_guide_fragment_header);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_service_guide_delivery_unit, atsc3_service_guide_fragment_payload);

ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_service_guide_fragment_header);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_service_guide_fragment_payload);


