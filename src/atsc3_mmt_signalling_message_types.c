/*
 * atsc3_mmt_signalling_messages_types.c
 *
 *  Created on: Aug 10, 2019
 *      Author: jjustman
 */

#include "atsc3_mmt_signalling_message_types.h"

void mmt_signalling_message_header_and_payload_free(mmt_signalling_message_header_and_payload_t** mmt_signalling_message_header_and_payload_p) {
	mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = *mmt_signalling_message_header_and_payload_p;
    if(mmt_signalling_message_header_and_payload) {
		//determine if we have any internal mallocs to clear
		if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MMT_ATSC3_MESSAGE_ID) {
			//free structs from mmt_atsc3_message_payload_t
			if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload) {
				free(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload);
				mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload = NULL;
			}
			if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content) {
				free(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content);
				mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content = NULL;
			}
			if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content_compressed) {
				free(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content_compressed);
				mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content_compressed = NULL;
			}
		} else if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MPT_message) {
			//clear out mp_table
			for(int i=0; i < mmt_signalling_message_header_and_payload->message_payload.mp_table.number_of_assets; i++) {
				mp_table_asset_row_t* mp_table_asset_row = &mmt_signalling_message_header_and_payload->message_payload.mp_table.mp_table_asset_row[i];
				if(mp_table_asset_row) {
					if(mp_table_asset_row->identifier_mapping.identifier_type == 0x00) {
						if(mp_table_asset_row->identifier_mapping.asset_id.asset_id) { //mp_table_asset_row->identifier_mapping.asset_id.asset_id_length
							mp_table_asset_row->identifier_mapping.asset_id.asset_id_length = 0 ;
							free(mp_table_asset_row->identifier_mapping.asset_id.asset_id);
							mp_table_asset_row->identifier_mapping.asset_id.asset_id = NULL;
						}
					}
					if(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor) {
						if(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple) {
							free(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple);
							mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple = NULL;
						}
						free(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor);
						mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor = NULL;
					}

					if(mp_table_asset_row->asset_descriptors_payload) { //mp_table_asset_row->asset_descriptors_length?
						mp_table_asset_row->asset_descriptors_length = 0;
						free(mp_table_asset_row->asset_descriptors_payload);
						mp_table_asset_row->asset_descriptors_payload = NULL;
					}
				}
			}

			free(mmt_signalling_message_header_and_payload->message_payload.mp_table.mp_table_asset_row);
			mmt_signalling_message_header_and_payload->message_payload.mp_table.mp_table_asset_row = NULL;

			if(mmt_signalling_message_header_and_payload->message_payload.mp_table.mmt_package_id.mmt_package_id) {
				free(mmt_signalling_message_header_and_payload->message_payload.mp_table.mmt_package_id.mmt_package_id);
				mmt_signalling_message_header_and_payload->message_payload.mp_table.mmt_package_id.mmt_package_id = NULL;
				mmt_signalling_message_header_and_payload->message_payload.mp_table.mmt_package_id.mmt_package_id_length = 0;
			}

			if(mmt_signalling_message_header_and_payload->message_payload.mp_table.mp_table_descriptors.mp_table_descriptors_byte) {
				free(mmt_signalling_message_header_and_payload->message_payload.mp_table.mp_table_descriptors.mp_table_descriptors_byte);
				mmt_signalling_message_header_and_payload->message_payload.mp_table.mp_table_descriptors.mp_table_descriptors_byte = NULL;
				mmt_signalling_message_header_and_payload->message_payload.mp_table.mp_table_descriptors.mp_table_descriptors_length = 0;
			}


		}

		free(mmt_signalling_message_header_and_payload);
		*mmt_signalling_message_header_and_payload_p = NULL;
	}
}
