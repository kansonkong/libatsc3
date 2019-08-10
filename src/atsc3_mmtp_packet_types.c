/*
 * atsc3_mmtp_packet_types.c
 *
 *  Created on: Aug 10, 2019
 *      Author: jjustman
 */

#include "atsc3_mmtp_packet_types.h"

mmtp_packet_header_t* mmtp_packet_header_new() {
	return calloc(1, sizeof(mmtp_packet_header_t));
}

//for mpu_sequence_number_mmtp_mpu_packet_t
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mpu_sequence_number_mmtp_mpu_packet, mmtp_mpu_packet);

//for mmtp_signalling_packet_t
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_signalling_packet, mmt_signalling_message_header_and_payload);

//mpu_sequence_number_mmtp_mpu_packet_new
//for mmtp_packet_id_packets_container_t
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_packet_id_packets_container, mmtp_signalling_packet);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_packet_id_packets_container, mmtp_mpu_nontimed_packet);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_packet_id_packets_container, mmtp_generic_object_packet);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_packet_id_packets_container, mmtp_repair_symbol_packet);

//for mmtp_asset_t
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_asset, mmtp_packet_id_packets_container);

//for mmtp_asset_flow
ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(mmtp_asset_flow);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_asset_flow, mmtp_asset);


