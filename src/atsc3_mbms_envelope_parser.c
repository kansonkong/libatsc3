/*
 * atsc3_mbms_envelope_parser.c
 *
 *  Created on: Apr 5, 2019
 *      Author: jjustman
 */


#include "atsc3_mbms_envelope_parser.h"

int _ROUTE_MBMS_ENVELOPE_PARSER_INFO_ENABLED = 0;
int _ROUTE_MBMS_ENVELOPE_PARSER_DEBUG_ENABLED = 0;

atsc3_fdt_file_t* atsc3_mbms_envelope_find_multipart_fdt_file_from_fdt_instance(atsc3_fdt_instance_t* atsc3_fdt_instance) {

	for(int i=0; i < atsc3_fdt_instance->atsc3_fdt_file_v.count; i++) {
		atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[i];
		if(!strncasecmp(ATSC3_MBMS_ENVELOPE_CONTENT_TYPE, atsc3_fdt_file->content_type, strlen(ATSC3_MBMS_ENVELOPE_CONTENT_TYPE))) {
			_ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_DEBUG("returning MBMS matching type: %s for toi: %u",  ATSC3_MBMS_ENVELOPE_CONTENT_TYPE, atsc3_fdt_file->toi);

			return atsc3_fdt_file;
        } else if(!strncasecmp(ATSC3_FDT_MULTIPART_RELATED, atsc3_fdt_file->content_type, strlen(ATSC3_FDT_MULTIPART_RELATED))) {
            _ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_DEBUG("returning multipart/related matching type: %s for toi: %u",  ATSC3_FDT_MULTIPART_RELATED, atsc3_fdt_file->toi);
            
            return atsc3_fdt_file;
        } else if(!strncasecmp(ATSC3_FDT_MULTIPART_SIGNED, atsc3_fdt_file->content_type, strlen(ATSC3_FDT_MULTIPART_SIGNED))) {
            _ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_INFO("returning multipart/signed matching type: %s for toi: %u",  ATSC3_FDT_MULTIPART_SIGNED, atsc3_fdt_file->toi);
            
            return atsc3_fdt_file;
        } else {
			_ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_DEBUG("ignoring fdt_file: toi: %u, type: %s, name: %s", atsc3_fdt_file->toi, atsc3_fdt_file->content_type, atsc3_fdt_file->content_location);
		}
	}

	return NULL;
}

bool atsc3_fdt_file_is_multipart_signed(atsc3_fdt_file_t* atsc3_fdt_file) {
	return !strncasecmp(ATSC3_FDT_MULTIPART_SIGNED, atsc3_fdt_file->content_type, strlen(ATSC3_FDT_MULTIPART_SIGNED));
}


/**
 *
 * <?xml version="1.0" encoding="UTF-8"?>
	<metadataEnvelope xmlns="urn:3gpp:metadata:2005:MBMS:envelope">
		<item contentType="application/route-usd+xml" metadataURI="usbd.xml" version="1"/>
		<item contentType="application/route-s-tsid+xml" metadataURI="stsid.xml" version="1"/>
		<item contentType="application/dash+xml" metadataURI="mpd.xml" version="82"/>
		<item contentType="application/atsc-held+xml" metadataURI="held.xml" version="1"/>
	</metadataEnvelope>

 *
 *
 */
//jjustman-2020-07-27: TODO - fix this type from char* payload to block_t*
atsc3_mbms_metadata_envelope_t* atsc3_mbms_envelope_parse_from_payload(char* payload, char* content_location) {
	atsc3_mbms_metadata_envelope_t* atsc3_mbms_metadata_envelope = NULL;
    
	block_t* metadata_envelope_fragment_block = block_Promote(payload);
	xml_document_t* xml_document = xml_parse_document(metadata_envelope_fragment_block->p_buffer, metadata_envelope_fragment_block->i_pos);
	if(!xml_document) {
		return NULL;
	}
	xml_node_t* xml_document_root_node = xml_document_root(xml_document);
	xml_string_t* xml_document_root_node_name = xml_node_name(xml_document_root_node);

	//opening header should be xml
	if(!xml_string_equals_ignore_case(xml_document_root_node_name, "xml")) {
		_ATSC3_ROUTE_S_TSID_PARSER_ERROR("atsc3_mbms_envelope_parse_from_payload: opening tag missing xml preamble");
		return NULL;
	}

	size_t num_root_children = xml_node_children(xml_document_root_node);
	for(int i=0; i < num_root_children; i++) {
		xml_node_t* root_child = xml_node_child(xml_document_root_node, i);
		xml_string_t* root_child_name = xml_node_name(root_child);

		uint8_t* root_child_name_string = xml_string_clone(root_child_name);
		_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("checking root_child tag at: %i, val: %s", i, root_child_name_string);
		freeclean_uint8_t(&root_child_name_string);

		if(xml_node_equals_ignore_case(root_child, "metadataEnvelope")) {
			atsc3_mbms_metadata_envelope = atsc3_mbms_metadata_envelope_new();

			size_t num_envelope_children = xml_node_children(root_child);
			for(int i=0; i < num_envelope_children; i++) {
				xml_node_t* envelope_child = xml_node_child(root_child, i);

				if(xml_node_equals_ignore_case(envelope_child, "item")) {
					atsc3_mbms_metadata_item_t* atsc3_mbms_metadata_item = atsc3_mbms_metadata_item_new();
                    
					atsc3_mbms_metadata_envelope_add_atsc3_mbms_metadata_item(atsc3_mbms_metadata_envelope, atsc3_mbms_metadata_item);

					//assign any of our attributes here
					uint8_t* xml_attributes = xml_attributes_clone_node(envelope_child);
					_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("metadataEnvelope.item.attributes: %s", xml_attributes);

					kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
					char* matching_attribute = NULL;

					if((matching_attribute = kvp_collection_get(kvp_collection,  "contentType"))) {
						atsc3_mbms_metadata_item->content_type = matching_attribute;
					}
					if((matching_attribute = kvp_collection_get(kvp_collection,  "metadataURI"))) {
						atsc3_mbms_metadata_item->metadata_uri = matching_attribute;
					}
					if((matching_attribute = kvp_collection_get(kvp_collection,  "validFrom"))) {
						atsc3_mbms_metadata_item->valid_from_string = matching_attribute;
					}
					if((matching_attribute = kvp_collection_get(kvp_collection,  "validUntil"))) {
						atsc3_mbms_metadata_item->valid_until_string = matching_attribute;
					}
					if((matching_attribute = kvp_collection_get(kvp_collection,  "version"))) {
						atsc3_mbms_metadata_item->version = atoi(matching_attribute);
                        free(matching_attribute);
					}
                    
					//todo - fix me with proper namespacing
					if((matching_attribute = kvp_collection_get(kvp_collection,  "nextURL"))) {
						atsc3_mbms_metadata_item->next_url_string = matching_attribute;
					}
					if((matching_attribute = kvp_collection_get(kvp_collection,  "availAt"))) {
						atsc3_mbms_metadata_item->avail_at_string = matching_attribute;
					}
                    free(xml_attributes);
                    kvp_collection_free(kvp_collection);
				}
			}
		}
	}
    xml_document_free(xml_document, false);
    block_Destroy(&metadata_envelope_fragment_block);
    
	return atsc3_mbms_metadata_envelope;
}

void atsc3_mbms_metadata_envelope_dump(atsc3_mbms_metadata_envelope_t* atsc3_mbms_metadata_envelope) {
	if(!atsc3_mbms_metadata_envelope) {
		_ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_ERROR("atsc3_mbms_metadata_envelope is null!");
		return;
	}

	for(int i=0; i < atsc3_mbms_metadata_envelope->atsc3_mbms_metadata_item_v.count; i++) {
		atsc3_mbms_metadata_item_t* atsc3_mbms_metadata_item = atsc3_mbms_metadata_envelope->atsc3_mbms_metadata_item_v.data[i];
		_ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_DEBUG("item: content_type: %s, metadata_uri: %s, valid_from: %s, valid_until: %s, version: %u, next_url: %s, avail_at: %s",
		atsc3_mbms_metadata_item->content_type,
		atsc3_mbms_metadata_item->metadata_uri,
		atsc3_mbms_metadata_item->valid_from_string,
		atsc3_mbms_metadata_item->valid_until_string,
		atsc3_mbms_metadata_item->version,
		atsc3_mbms_metadata_item->next_url_string,
		atsc3_mbms_metadata_item->avail_at_string);
	}

	_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("--atsc3_mbms_metadata_envelope_t");

}

