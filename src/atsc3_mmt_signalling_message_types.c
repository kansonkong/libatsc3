/*
 * atsc3_mmt_signalling_messages_types.c
 *
 *  Created on: Aug 10, 2019
 *      Author: jjustman
 */

#include "atsc3_mmt_signalling_message_types.h"

void mmt_signalling_message_header_and_payload_free(mmt_signalling_message_header_and_payload_t** mmt_signalling_message_header_and_payload_p) {
    mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = *mmt_signalling_message_header_and_payload_p;
    __MMTP_DEBUG("mmt_signalling_message_header_and_payload_free: %p: message_id: %d, message type: %d",
            mmt_signalling_message_header_and_payload, mmt_signalling_message_header_and_payload->message_header.message_id, mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type);
    if(mmt_signalling_message_header_and_payload) {
        __MMTP_DEBUG("mmt_signalling_message_header_and_payload_free: clearing MMT_ATSC3_MESSAGE_ID");
    
		//determine if we have any internal mallocs to clear
		if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MMT_ATSC3_MESSAGE_ID) {
			//free structs from mmt_atsc3_message_payload_t
            __MMTP_DEBUG("mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload ptr: %p", mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload);
            if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload) {
				free(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload);
				mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload = NULL;
			}
            
            __MMTP_DEBUG("mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content ptr: %p", mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content);
            if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content) {
				free(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content);
				mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content = NULL;
			}

            __MMTP_DEBUG("mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content_compressed ptr: %p", mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content_compressed);
            if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content_compressed) {
				free(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content_compressed);
				mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content_compressed = NULL;
			}
            
            __MMTP_DEBUG("mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload ptr: %p", mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload);
            if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload) {
                free(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload);
                mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload = NULL;
            }
            if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.mmt_atsc3_held_message) {
                mmt_atsc3_held_message_free(&mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.mmt_atsc3_held_message);
            }
            if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.mmt_atsc3_route_component) {
                if(!mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.mmt_atsc3_route_component->__is_pinned_to_context) {
                    mmt_atsc3_route_component_free(&mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.mmt_atsc3_route_component);
                }
            }

            
		} else if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MPT_message) {
			//clear out mp_table
            __MMTP_DEBUG("mmt_signalling_message_header_and_payload_free: clearing mp_table: assets: %d", mmt_signalling_message_header_and_payload->message_payload.mp_table.number_of_assets);
           
			for(int i=0; i < mmt_signalling_message_header_and_payload->message_payload.mp_table.number_of_assets; i++) {
				mp_table_asset_row_t* mp_table_asset_row = &mmt_signalling_message_header_and_payload->message_payload.mp_table.mp_table_asset_row[i];

                __MMTP_DEBUG("mmt_signalling_message_header_and_payload_free_inner: i: %d, ptr: %p", i, mp_table_asset_row);
                atsc3_mmt_mp_table_asset_row_free_inner(mp_table_asset_row);
    		}

			//since mp_table_asset_row is allocated as calloc(numAssets, sizeof(mp_table_asset_row_t)), we can only do one free()
			free(mmt_signalling_message_header_and_payload->message_payload.mp_table.mp_table_asset_row);
			mmt_signalling_message_header_and_payload->message_payload.mp_table.mp_table_asset_row = NULL;

            __MMTP_DEBUG("mmt_signalling_message_header_and_payload_free: mmt_signalling_message_header_and_payload->message_payload.mp_table.mmt_package_id.mmt_package_id: %p", mmt_signalling_message_header_and_payload->message_payload.mp_table.mmt_package_id.mmt_package_id);

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
        } else {
            __MMTP_DEBUG("mmt_signalling_message_header_and_payload_free: clearing mp_table: assets: %d", mmt_signalling_message_header_and_payload->message_payload.mp_table.number_of_assets);

        }

		free(mmt_signalling_message_header_and_payload);
		*mmt_signalling_message_header_and_payload_p = NULL;
	}
}

mp_table_asset_row_t* atsc3_mmt_mp_table_asset_row_duplicate(const mp_table_asset_row_t* mp_table_asset_row_src) {
    mp_table_asset_row_t* mp_table_asset_row_dst = calloc(1, sizeof(mp_table_asset_row_t));

    //hack to get our non-ptr fields duplicated
    memcpy(mp_table_asset_row_dst, mp_table_asset_row_src, sizeof(mp_table_asset_row_t));

    if (mp_table_asset_row_src->identifier_mapping.identifier_type == 0x00) { //asset_id type
        if(mp_table_asset_row_src->identifier_mapping.asset_id.asset_id_length && mp_table_asset_row_src->identifier_mapping.asset_id.asset_id) {
            mp_table_asset_row_dst->identifier_mapping.asset_id.asset_id = calloc(mp_table_asset_row_src->identifier_mapping.asset_id.asset_id_length, sizeof(uint8_t));
            memcpy(mp_table_asset_row_dst->identifier_mapping.asset_id.asset_id, mp_table_asset_row_src->identifier_mapping.asset_id.asset_id, mp_table_asset_row_src->identifier_mapping.asset_id.asset_id_length);
        } else {
            mp_table_asset_row_dst->identifier_mapping.asset_id.asset_id_length = 0;
            mp_table_asset_row_dst->identifier_mapping.asset_id.asset_id = NULL;
        }
    } else if(mp_table_asset_row_src->identifier_mapping.identifier_type == 0x01) { //url

        if(mp_table_asset_row_src->identifier_mapping.url_count && mp_table_asset_row_src->identifier_mapping.url_length_list) {
            mp_table_asset_row_dst->identifier_mapping.url_length_list = calloc(mp_table_asset_row_src->identifier_mapping.url_count, sizeof(url_length_t));
            for(int i=0; i < mp_table_asset_row_src->identifier_mapping.url_count; i++) {

                url_length_t* url_length_old = &mp_table_asset_row_src->identifier_mapping.url_length_list[i];
                url_length_t* url_length_new = &mp_table_asset_row_dst->identifier_mapping.url_length_list[i];

                url_length_new->length = url_length_old->length;
                url_length_new->byte = calloc(url_length_new->length, sizeof(uint8_t));
                memcpy(url_length_new->byte, url_length_old->byte, url_length_new->length);
            }
        } else {
            mp_table_asset_row_dst->identifier_mapping.url_count = 0;
            mp_table_asset_row_dst->identifier_mapping.url_length_list = NULL;
        }
    } else if(mp_table_asset_row_src->identifier_mapping.identifier_type == 0x02) { //regex

        if(mp_table_asset_row_src->identifier_mapping.regex_length && mp_table_asset_row_src->identifier_mapping.regex_byte) {
            mp_table_asset_row_dst->identifier_mapping.regex_byte = calloc(mp_table_asset_row_src->identifier_mapping.regex_length, sizeof(uint8_t));
            memcpy(mp_table_asset_row_dst->identifier_mapping.regex_byte, mp_table_asset_row_src->identifier_mapping.regex_byte, mp_table_asset_row_src->identifier_mapping.regex_length);
        }
    } else if(mp_table_asset_row_src->identifier_mapping.identifier_type == 0x03) { //representation_id

        if(mp_table_asset_row_src->identifier_mapping.representation_id_length && mp_table_asset_row_src->identifier_mapping.representation_id_byte) {
            mp_table_asset_row_dst->identifier_mapping.representation_id_byte = calloc(mp_table_asset_row_src->identifier_mapping.representation_id_length, sizeof(uint8_t));
            memcpy(mp_table_asset_row_dst->identifier_mapping.representation_id_byte, mp_table_asset_row_src->identifier_mapping.representation_id_byte, mp_table_asset_row_src->identifier_mapping.representation_id_length);
        }
    } else { //any other, private

        if (mp_table_asset_row_src->identifier_mapping.private_length && mp_table_asset_row_src->identifier_mapping.private_byte) {
            mp_table_asset_row_dst->identifier_mapping.private_byte = calloc(mp_table_asset_row_src->identifier_mapping.private_length, sizeof(uint8_t));
            memcpy(mp_table_asset_row_dst->identifier_mapping.private_byte, mp_table_asset_row_src->identifier_mapping.private_byte, mp_table_asset_row_src->identifier_mapping.private_length);
        }
    }

    if(mp_table_asset_row_src->asset_descriptors_length && mp_table_asset_row_src->asset_descriptors_payload) {
        mp_table_asset_row_dst->asset_descriptors_payload = calloc(mp_table_asset_row_src->asset_descriptors_length, sizeof(uint8_t));
        memcpy(mp_table_asset_row_dst->asset_descriptors_payload, mp_table_asset_row_src->asset_descriptors_payload, mp_table_asset_row_src->asset_descriptors_length);
    } else {
        mp_table_asset_row_dst->asset_descriptors_length = 0;
        mp_table_asset_row_dst->asset_descriptors_payload = NULL;
    }

    if(mp_table_asset_row_src->mmt_signalling_message_mpu_timestamp_descriptor) {
        mp_table_asset_row_dst->mmt_signalling_message_mpu_timestamp_descriptor = calloc(1, sizeof(mmt_signalling_message_mpu_timestamp_descriptor_t));

        mp_table_asset_row_dst->mmt_signalling_message_mpu_timestamp_descriptor->descriptor_tag = mp_table_asset_row_src->mmt_signalling_message_mpu_timestamp_descriptor->descriptor_tag;
        mp_table_asset_row_dst->mmt_signalling_message_mpu_timestamp_descriptor->descriptor_length = mp_table_asset_row_src->mmt_signalling_message_mpu_timestamp_descriptor->descriptor_length;
        mp_table_asset_row_dst->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n = mp_table_asset_row_src->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n;

        if(mp_table_asset_row_src->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n && mp_table_asset_row_src->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple) {
            mp_table_asset_row_dst->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple = calloc(mp_table_asset_row_src->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n, sizeof(mmt_signalling_message_mpu_tuple_t));
            for(int i=0; i < mp_table_asset_row_src->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n; i++) {

                mmt_signalling_message_mpu_tuple_t* mmt_signalling_message_mpu_tuple_src = &mp_table_asset_row_src->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[i];
                mmt_signalling_message_mpu_tuple_t* mmt_signalling_message_mpu_tuple_dst = &mp_table_asset_row_dst->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[i];

                mmt_signalling_message_mpu_tuple_dst->mpu_sequence_number   = mmt_signalling_message_mpu_tuple_src->mpu_sequence_number;
                mmt_signalling_message_mpu_tuple_dst->mpu_presentation_time = mmt_signalling_message_mpu_tuple_src->mpu_presentation_time;
            }

        } else {
            mp_table_asset_row_dst->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n = 0;
            mp_table_asset_row_dst->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple = NULL;
        }
    }

    return mp_table_asset_row_dst;
}

/*
 * jjustman-2020-12-02 - since mp_table_asset_row is allocated as calloc(numAssets, sizeof(mp_table_asset_row_t)), we can only do one free()
 */
void atsc3_mmt_mp_table_asset_row_free_inner(mp_table_asset_row_t* mp_table_asset_row) {

    if (mp_table_asset_row) {
        if (mp_table_asset_row->identifier_mapping.identifier_type == 0x00) {
            if (mp_table_asset_row->identifier_mapping.asset_id.asset_id) { //mp_table_asset_row->identifier_mapping.asset_id.asset_id_length
                mp_table_asset_row->identifier_mapping.asset_id.asset_id_length = 0;
                free(mp_table_asset_row->identifier_mapping.asset_id.asset_id);
                mp_table_asset_row->identifier_mapping.asset_id.asset_id = NULL;
            }
        } else {
            __MMTP_WARN("atsc3_mmt_mp_table_asset_row_free: TODO: cleanup mp_table_asset_row->identifier_mapping.identifier_type: 0x%02x instance", mp_table_asset_row->identifier_mapping.identifier_type);
        }

        __MMTP_DEBUG("mmt_signalling_message_header_and_payload_free: mmt_signalling_message_mpu_timestamp_descriptor: %p", mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor);

        if (mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor) {
            if (mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple) {
                free(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple);
                mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple = NULL;
            }
            free(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor);
            mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor = NULL;
        }

        if (mp_table_asset_row->asset_descriptors_payload) { //mp_table_asset_row->asset_descriptors_length?
            mp_table_asset_row->asset_descriptors_length = 0;
            free(mp_table_asset_row->asset_descriptors_payload);
            mp_table_asset_row->asset_descriptors_payload = NULL;
        }
    }
}


void mmt_atsc3_route_component_free(mmt_atsc3_route_component_t** mmt_atsc3_route_component_p) {
    if(mmt_atsc3_route_component_p) {
        mmt_atsc3_route_component_t* mmt_atsc3_route_component = *mmt_atsc3_route_component_p;
        if(mmt_atsc3_route_component) {
            if(mmt_atsc3_route_component->stsid_uri_s) {
                freeclean((void**)&mmt_atsc3_route_component->stsid_uri_s);
            }
            if(mmt_atsc3_route_component->stsid_destination_ip_address_s) {
                freeclean((void**)&mmt_atsc3_route_component->stsid_destination_ip_address_s);
            }
            if(mmt_atsc3_route_component->stsid_source_ip_address_s) {
                freeclean((void**)&mmt_atsc3_route_component->stsid_source_ip_address_s);
            }

            free(mmt_atsc3_route_component);
            mmt_atsc3_route_component = NULL;
        }
        *mmt_atsc3_route_component_p = NULL;
    }
}

void mmt_atsc3_held_message_free(mmt_atsc3_held_message_t** mmt_atsc3_held_message_p) {
    if(mmt_atsc3_held_message_p) {
        mmt_atsc3_held_message_t* mmt_atsc3_held_message = *mmt_atsc3_held_message_p;
        if(mmt_atsc3_held_message) {
            if(mmt_atsc3_held_message->held_message) {
                block_Destroy(&mmt_atsc3_held_message->held_message);
            }

            free(mmt_atsc3_held_message);
            mmt_atsc3_held_message = NULL;
        }
        *mmt_atsc3_held_message_p = NULL;
    }
}
