/*
 * atsc3_mmtp_packet_types.c
 *
 *  Created on: Aug 10, 2019
 *      Author: jjustman
 */

#include "atsc3_mmtp_packet_types.h"

//cleanup macro impl's
void mmtp_packet_header_free(mmtp_packet_header_t** mmtp_packet_header_p) {
	if(mmtp_packet_header_p) {
		mmtp_packet_header_t* mmtp_packet_header = *mmtp_packet_header_p;

		if(mmtp_packet_header) {
			block_Destroy(&mmtp_packet_header->raw_packet);
			block_Destroy(&mmtp_packet_header->mmtp_header_extension);

			freesafe(mmtp_packet_header);
			mmtp_packet_header = NULL;
		}
		*mmtp_packet_header_p = NULL;
	}
}



void mpu_sequence_number_mmtp_mpu_packet_collection_free(mpu_sequence_number_mmtp_mpu_packet_collection_t** mpu_sequence_number_mmtp_mpu_packet_collection_p) {
	if(mpu_sequence_number_mmtp_mpu_packet_collection_p) {
		mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = *mpu_sequence_number_mmtp_mpu_packet_collection_p;

		if(mpu_sequence_number_mmtp_mpu_packet_collection) {
			mpu_sequence_number_mmtp_mpu_packet_collection_free_mmtp_mpu_packet(mpu_sequence_number_mmtp_mpu_packet_collection);
			freesafe(mpu_sequence_number_mmtp_mpu_packet_collection);
			mpu_sequence_number_mmtp_mpu_packet_collection = NULL;
		}
		*mpu_sequence_number_mmtp_mpu_packet_collection_p = NULL;
	}
}

//chained from mmtp_packet_id_packets_container_free_mmtp_signalling_packet
void mmtp_signalling_packet_free(mmtp_signalling_packet_t** mmtp_signalling_packet_p) {
	if(mmtp_signalling_packet_p) {
		mmtp_signalling_packet_t* mmtp_signalling_packet = *mmtp_signalling_packet_p;

		if(mmtp_signalling_packet) {
			block_Destroy(&mmtp_signalling_packet->raw_packet);
			block_Destroy(&mmtp_signalling_packet->mmtp_header_extension);
            __MMTP_INFO("mmtp_signalling_packet_free, packet_id: %d", mmtp_signalling_packet->mmtp_packet_id);

			mmtp_signalling_packet_free_mmt_signalling_message_header_and_payload(mmtp_signalling_packet);
			freesafe(mmtp_signalling_packet);
			mmtp_signalling_packet = NULL;
		}
		*mmtp_signalling_packet_p = NULL;
	}
}

void mmtp_mpu_packet_free(mmtp_mpu_packet_t** mmtp_mpu_packet_p) {
	if(mmtp_mpu_packet_p) {
		mmtp_mpu_packet_t* mmtp_mpu_packet = *mmtp_mpu_packet_p;

		if(mmtp_mpu_packet) {
			block_Destroy(&mmtp_mpu_packet->raw_packet);
			block_Destroy(&mmtp_mpu_packet->mmtp_header_extension);
			block_Destroy(&mmtp_mpu_packet->du_mpu_metadata_block);
			block_Destroy(&mmtp_mpu_packet->du_movie_fragment_block);
			block_Destroy(&mmtp_mpu_packet->du_mfu_block);
            if(mmtp_mpu_packet->mmthsample_header) {
                free(mmtp_mpu_packet->mmthsample_header);
                mmtp_mpu_packet->mmthsample_header = NULL;
            }

			freesafe(mmtp_mpu_packet);
			mmtp_mpu_packet = NULL;
		}
		*mmtp_mpu_packet_p = NULL;
	}
}

void mmtp_packet_id_packets_container_free(mmtp_packet_id_packets_container_t** mmtp_packet_id_packets_container_p) {
	if(mmtp_packet_id_packets_container_p) {
		mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container = *mmtp_packet_id_packets_container_p;

		if(mmtp_packet_id_packets_container) {
			mmtp_packet_id_packets_container_free_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container);
			mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);
			mmtp_packet_id_packets_container_free_mmtp_mpu_nontimed_packet(mmtp_packet_id_packets_container);
			mmtp_packet_id_packets_container_free_mmtp_generic_object_packet(mmtp_packet_id_packets_container);
			mmtp_packet_id_packets_container_free_mmtp_repair_symbol_packet(mmtp_packet_id_packets_container);

			freesafe(mmtp_packet_id_packets_container);
			mmtp_packet_id_packets_container = NULL;
		}
		*mmtp_packet_id_packets_container_p = NULL;
	}
}
void mmtp_asset_free(mmtp_asset_t** mmtp_asset_p) {
	if(mmtp_asset_p) {
		mmtp_asset_t* mmtp_asset = *mmtp_asset_p;

		if(mmtp_asset) {
			mmtp_asset->parent_mmtp_asset_flow = NULL; //do NOT free this, as its just a soft reference to our parent
			//chain inner payload free
			mmtp_asset_free_mmtp_packet_id_packets_container(mmtp_asset);
			freesafe(mmtp_asset);
			mmtp_asset = NULL;
		}
		*mmtp_asset_p = NULL;
	}
}

//for mpu_sequence_number_mmtp_mpu_packet_t
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mpu_sequence_number_mmtp_mpu_packet_collection, mmtp_mpu_packet);

//for mmtp_signalling_packet_t
//invokes mmt_signalling_message_header_and_payload_free as a chained destructor
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_signalling_packet, mmt_signalling_message_header_and_payload);

//mpu_sequence_number_mmtp_mpu_packet_new
//for mmtp_packet_id_packets_container_t
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
//invokes mmt_signalling_message_free as chained destructor
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_packet_id_packets_container, mmtp_signalling_packet);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_packet_id_packets_container, mmtp_mpu_nontimed_packet);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(mmtp_mpu_nontimed_packet);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_packet_id_packets_container, mmtp_generic_object_packet);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(mmtp_generic_object_packet);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_packet_id_packets_container, mmtp_repair_symbol_packet);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(mmtp_repair_symbol_packet);

//for mmtp_asset_t
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_asset, mmtp_packet_id_packets_container);

//for mmtp_asset_flow
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_asset_flow, mmtp_asset);

//for mmtp_flow
ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(mmtp_flow);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(mmtp_flow, mmtp_asset_flow);

//interim free
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(mmtp_asset_flow);


