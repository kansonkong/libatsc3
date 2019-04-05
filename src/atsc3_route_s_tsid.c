/*
 * atsc3_route_s_tsid.c
 *
 *  Created on: Apr 2, 2019
 *      Author: jjustman
 */

#include "atsc3_route_s_tsid.h"

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_route_s_tsid_RS, atsc3_route_s_tsid_RS_LS)

ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_route_s_tsid)
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_route_s_tsid, atsc3_route_s_tsid_RS)


atsc3_route_s_tsid_t* atsc3_route_s_tsid_parse_from_payload(char* payload, char* content_location) {

	atsc3_route_s_tsid_t* atsc3_route_s_tsid;

	block_t* s_tsid_fragment_block = block_Promote(payload);
	xml_document_t* xml_document = xml_parse_document(s_tsid_fragment_block->p_buffer, s_tsid_fragment_block->i_pos);
	if(!xml_document) {
		return NULL;

	}
	xml_node_t* xml_document_root_node = xml_document_root(xml_document);
	xml_string_t* xml_document_root_node_name = xml_node_name(xml_document_root_node);

	//opening header should be xml
	dump_xml_string(xml_document_root_node_name);
	if(!xml_string_equals_ignore_case(xml_document_root_node_name, "xml")) {
		_ATSC3_ROUTE_S_TSID_PARSER_ERROR("atsc3_route_s_tsid_parse_from_payload: opening tag missing xml preamble");
		return NULL;
	}

	size_t num_root_children = xml_node_children(xml_document_root_node);
	for(int i=0; i < num_root_children; i++) {
		xml_node_t* root_child = xml_node_child(xml_document_root_node, i);
		xml_string_t* root_child_name = xml_node_name(root_child);

		_ATSC3_ROUTE_S_TSID_PARSER_INFO("checking root_child tag at: %i, valr", i);
		dump_xml_string(root_child_name);

		if(xml_node_equals_ignore_case(root_child, "S-TSID")) {
			atsc3_route_s_tsid = atsc3_route_s_tsid_new();

			//attributes observed so far are only namespaces, so skip...
//			uint8_t* xml_attributes = xml_attributes_clone_node(root_child);
//			_ATSC3_ROUTE_MPD_PARSER_DEBUG("attributes: %s", xml_attributes);
//			kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
//
//			char* matching_attribute = NULL;
//
//			if((matching_attribute = kvp_collection_get(kvp_collection,  "availabilityStartTime"))) {
//				atsc3_route_mpd->availability_start_time = matching_attribute;
//			}

			//process all child nodes
			size_t num_s_tsid_entry_row_children = xml_node_children(root_child);
			for(int j=0; j < num_s_tsid_entry_row_children; j++) {
				xml_node_t* s_tsid_entry_row_children = xml_node_child(root_child, j);
				if(xml_node_equals_ignore_case(s_tsid_entry_row_children, "RS")) {
					atsc3_route_s_tsid_parse_RS(s_tsid_entry_row_children, atsc3_route_s_tsid);
				}
			}
		}
	}

	return atsc3_route_s_tsid;
}

atsc3_route_s_tsid_t* atsc3_route_s_tsid_parse_RS(xml_node_t* xml_rs_node, atsc3_route_s_tsid_t* atsc3_route_s_tsid) {
	atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_RS = atsc3_route_s_tsid_RS_new();
	atsc3_route_s_tsid_add_atsc3_route_s_tsid_RS(atsc3_route_s_tsid, atsc3_route_s_tsid_RS);

	//parse our inner attributes, dIpAddr, dPort, dsIpAddr
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_rs_node);
	_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("RS.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "dIpAddr"))) {
		atsc3_route_s_tsid_RS->dest_ip_addr = 0; //todo - convert string dIpAddr to uint32_t
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "dPort"))) {
		atsc3_route_s_tsid_RS->dest_port = 0; //todo - convert string dPort to uint16_t
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "sIpAddr"))) {
		atsc3_route_s_tsid_RS->src_ip_addr = 0; //todo - convert string sIpAddr to uint32_t
	}

	//parse child nodes
	size_t num_s_tsid_rs_entry_row_children = xml_node_children(xml_rs_node);
	for(int j=0; j < num_s_tsid_rs_entry_row_children; j++) {
		xml_node_t* s_tsid_rs_entry_row_children = xml_node_child(xml_rs_node, j);
		if(xml_node_equals_ignore_case(s_tsid_rs_entry_row_children, "LS")) {
			atsc3_route_s_tsid_parse_RS_LS(s_tsid_rs_entry_row_children, atsc3_route_s_tsid_RS);
		}
	}

	return atsc3_route_s_tsid;
}



atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_parse_RS_LS(xml_node_t* xml_rs_node, atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_RS) {
	atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS = atsc3_route_s_tsid_RS_LS_new();
	atsc3_route_s_tsid_RS_add_atsc3_route_s_tsid_RS_LS(atsc3_route_s_tsid_RS, atsc3_route_s_tsid_RS_LS);

	//parse our inner attributes, dIpAddr, dPort, dsIpAddr
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_rs_node);
	_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("RS.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "dIpAddr"))) {
		atsc3_route_s_tsid_RS->dest_ip_addr = 0; //todo - convert string dIpAddr to uint32_t
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "dPort"))) {
		atsc3_route_s_tsid_RS->dest_port = 0; //todo - convert string dPort to uint16_t
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "sIpAddr"))) {
		atsc3_route_s_tsid_RS->src_ip_addr = 0; //todo - convert string sIpAddr to uint32_t
	}

	//parse child nodes
	size_t num_s_tsid_rs_entry_row_children = xml_node_children(xml_rs_node);
	for(int j=0; j < num_s_tsid_rs_entry_row_children; j++) {
		xml_node_t* s_tsid_rs_entry_row_children = xml_node_child(xml_rs_node, j);
		if(xml_node_equals_ignore_case(s_tsid_rs_entry_row_children, "LS")) {
			atsc3_route_s_tsid_parse_RS_LS(s_tsid_rs_entry_row_children, atsc3_route_s_tsid_RS);
		}
	}
	return atsc3_route_s_tsid_RS;
}


void atsc3_route_s_tsid_dump(atsc3_route_s_tsid_t* atsc3_route_s_tsid) {


}



