/*
 * atsc3_route_usbd.c
 *
 *  Created on: April 6, 2019
 *      Author: jjustman
 *
 */

#include "atsc3_route_usbd.h"

int _ROUTE_USBD_PARSER_INFO_ENABLED = 0;
int _ROUTE_USBD_PARSER_DEBUG_ENABLED = 0;

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_user_service_description, atsc3_user_service_delivery_method)
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_user_service_delivery_method);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_user_service_delivery_method, atsc3_user_service_broadcast_app_service)
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_user_service_broadcast_app_service);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_user_service_delivery_method, atsc3_user_service_unicast_app_service)
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_user_service_unicast_app_service);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_user_service_unicast_app_service, atsc3_user_service_broadcast_app_service_base_pattern)
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_user_service_broadcast_app_service_base_pattern);


//base parser implementation..


//jjustman-2020-07-27 - todo: change this char* payload to block_t*
atsc3_route_user_service_bundle_description_t* atsc3_route_user_service_bundle_description_parse_from_payload(char* payload, char* content_location) {
	block_t* usbd_fragment_block = block_Promote(payload);
	xml_document_t* xml_document = xml_parse_document(usbd_fragment_block->p_buffer, usbd_fragment_block->i_pos);
	if(!xml_document) {
		return NULL;
	}
	xml_node_t* xml_document_root_node = xml_document_root(xml_document);
	xml_string_t* xml_document_root_node_name = xml_node_name(xml_document_root_node);

	//opening header should be xml
	dump_xml_string(xml_document_root_node_name);
	if(!xml_string_equals_ignore_case(xml_document_root_node_name, "xml")) {
		_ATSC3_ROUTE_USBD_PARSER_ERROR("atsc3_route_user_service_bundle_description_parse_from_payload: opening tag missing xml preamble");
		return NULL;
	}

	atsc3_route_user_service_bundle_description_t* atsc3_route_user_service_bundle_description = calloc(1, sizeof(atsc3_route_user_service_bundle_description_t));

	size_t num_root_children = xml_node_children(xml_document_root_node);
	for(int i=0; i < num_root_children; i++) {
		xml_node_t* root_child = xml_node_child(xml_document_root_node, i);
		xml_string_t* root_child_name = xml_node_name(root_child);
		uint8_t* root_child_name_string = xml_string_clone(root_child_name);
		_ATSC3_ROUTE_USBD_PARSER_INFO("checking root_child tag at: %i, val: %s", i, root_child_name_string);
		freeclean_uint8_t(&root_child_name_string);

		if(xml_node_equals_ignore_case(root_child, "BundleDescriptionROUTE")) {
			size_t bundle_children = xml_node_children(root_child);

			//todo: fix me
			if(bundle_children) {
				xml_node_t* usbd_child = xml_node_child(root_child, 0);
				atsc3_route_usbd_parse_from_usd(usbd_child, atsc3_route_user_service_bundle_description);
			}
		}
	}
    xml_document_free(xml_document, false);
    block_Destroy(&usbd_fragment_block);
	return atsc3_route_user_service_bundle_description;
}


atsc3_user_service_description_t* atsc3_route_usbd_parse_from_usd(xml_node_t* xml_node, atsc3_route_user_service_bundle_description_t* atsc3_route_user_service_bundle_description) {

	atsc3_user_service_description_t* atsc3_user_service_description = calloc(1, sizeof(atsc3_user_service_description_t));
	atsc3_route_user_service_bundle_description->atsc3_user_service_description = atsc3_user_service_description;

	//assign any of our attributes here
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_node);
	_ATSC3_ROUTE_USBD_PARSER_DEBUG("UserServiceDescription.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "serviceId"))) {
		atsc3_user_service_description->service_id = atoi(matching_attribute);
        free(matching_attribute);
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "serviceId"))) {
		atsc3_user_service_description->service_status = strncmp("true", matching_attribute, 4) == 0 ;
        free(matching_attribute);
	}

	//parse children...


	size_t num_children = xml_node_children(xml_node);
	for(int j=0; j < num_children; j++) {
		xml_node_t* usd_child = xml_node_child(xml_node, j);
		if(xml_node_equals_ignore_case(usd_child, "Name")) {
			//todo: refactor this quick and dirry
			//get lang attribute...
			atsc3_user_service_description->name.name = (char*)xml_easy_content(usd_child);
		} else if(xml_node_equals_ignore_case(usd_child, "ServiceLanguage")) {
			//todo: refactor this quick and dirry
			//get lang attribute...
			atsc3_user_service_description->service_language = (char*)xml_easy_content(usd_child);
		}
		//todo - delivery method
		//broadcast app svc
	}
    free(xml_attributes);
    kvp_collection_free(kvp_collection);
	return atsc3_user_service_description;
}

void atsc3_route_usb_dump(atsc3_route_user_service_bundle_description_t* atsc3_route_user_service_bundle_description) {
	if(!atsc3_route_user_service_bundle_description) {
		_ATSC3_ROUTE_USBD_PARSER_ERROR("atsc3_route_user_service_bundle_description is null!");
		return;
	}
	if(atsc3_route_user_service_bundle_description->atsc3_user_service_description) {
		_ATSC3_ROUTE_USBD_PARSER_DEBUG("usb: service_id: %u, name: %s, lang: %s",
				atsc3_route_user_service_bundle_description->atsc3_user_service_description->service_id,
				atsc3_route_user_service_bundle_description->atsc3_user_service_description->name.name,
				atsc3_route_user_service_bundle_description->atsc3_user_service_description->service_language);

	}

	_ATSC3_ROUTE_USBD_PARSER_DEBUG("--atsc3_route_usb_dump");
}
