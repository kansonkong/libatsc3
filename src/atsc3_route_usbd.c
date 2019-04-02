/*
 * atsc3_route_usbd.c
 *
 *  Created on: April 6, 2019
 *      Author: jjustman
 *
 */

#include "atsc3_route_usbd.h"

//ATSC3_VECTOR_PARENT_BUILDER_METHODS_IMPLEMENTATION(atsc3_user_service_delivery_method);


ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_user_service_description, atsc3_user_service_delivery_method)
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_user_service_delivery_method, atsc3_user_service_broadcast_app_service)
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_user_service_delivery_method, atsc3_user_service_unicast_app_service)
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_user_service_unicast_app_service, atsc3_user_service_broadcast_app_service_base_pattern)

