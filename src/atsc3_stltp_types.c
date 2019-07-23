/*
 * atsc3_stltp_types.c
 *
 *  Created on: Jul 23, 2019
 *      Author: jjustman
 */


#include "atsc3_stltp_types.h"

ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_stltp_tunnel_packet)

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_stltp_tunnel_packet, atsc3_stltp_baseband_packet);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_stltp_tunnel_packet, atsc3_stltp_preamble_packet);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_stltp_tunnel_packet, atsc3_stltp_timing_management_packet);

