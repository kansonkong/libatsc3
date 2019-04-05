/*
 * atsc3_sls_held_fragment.c
 *
 *  Created on: Apr 2, 2019
 *      Author: jjustman
 */


#include "atsc3_sls_held_fragment.h"

#ifdef __cplusplus
extern "C" {
#endif


ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_sls_held_fragment);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_sls_held_fragment, atsc3_sls_html_entry_package);

atsc3_sls_held_fragment_t* atsc3_sls_held_fragment_parse_from_payload(char* payload, char* content_locationt) {

	atsc3_sls_held_fragment_t* atsc3_sls_held_fragment;

	block_t* held_fragment_block = block_Promote(payload);
	xml_document_t* xml_document = xml_parse_document(held_fragment_block->p_buffer, held_fragment_block->i_pos);

	if(!xml_document) {
		return NULL;
	}

	xml_node_t* xml_document_root_node = xml_document_root(xml_document);
	xml_string_t* xml_document_root_node_name = xml_node_name(xml_document_root_node);

	//opening header should be xml
	dump_xml_string(xml_document_root_node_name);
	if(!xml_string_equals_ignore_case(xml_document_root_node_name, "xml")) {
		_ATSC3_HELD_PARSER_ERROR("atsc3_sls_held_fragment_parse_from_payload: opening tag missing xml preamble");
		return NULL;
	}

	//we should have at least one HELD adn then one HTMLEntityPackage
	//atsc3_sls_held_fragment

	size_t num_root_children = xml_node_children(xml_document_root_node);
	for(int i=0; i < num_root_children; i++) {
		xml_node_t* root_child = xml_node_child(xml_document_root_node, i);
		if(xml_node_equals_ignore_case(root_child, "HELD")) {
			atsc3_sls_held_fragment = atsc3_sls_held_fragment_new();

			//HTMLEntryPackage
			size_t num_html_entry_packages_children = xml_node_children(root_child);
			for(int j=0; j < num_html_entry_packages_children; j++) {
				xml_node_t* html_entry_packages_node = xml_node_child(root_child, j);
				if(xml_node_equals_ignore_case(html_entry_packages_node, "HTMLEntryPackage")) {

					atsc3_sls_html_entry_package_t* atsc3_sls_html_entry_package = atsc3_sls_html_entry_package_new();

					uint8_t* xml_attributes = xml_attributes_clone_node(html_entry_packages_node);
					//_ATSC3_HELD_PARSER_DEBUG("attributes: %s", xml_attributes);
					kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);

					char* matching_attribute = NULL;

					if((matching_attribute = kvp_collection_get(kvp_collection,  "appContextId"))) {
						atsc3_sls_html_entry_package->app_context_id = matching_attribute;
					}

					if((matching_attribute = kvp_collection_get(kvp_collection,  "appRendering"))) {
						atsc3_sls_html_entry_package->app_rendering = strncmp("true", matching_attribute, 4) == 0;
					}

					if((matching_attribute = kvp_collection_get(kvp_collection,  "clearAppContextCacheDate"))) {
						atsc3_sls_html_entry_package->clear_app_context_cache_date_s = matching_attribute;
					}

					if((matching_attribute = kvp_collection_get(kvp_collection,  "bcastEntryPackageUrl"))) {
						atsc3_sls_html_entry_package->bcast_entry_package_url = matching_attribute;
					}
					if((matching_attribute = kvp_collection_get(kvp_collection,  "bcastEntryPageUrl"))) {
						atsc3_sls_html_entry_package->bcast_entry_page_url = matching_attribute;
					}
					if((matching_attribute = kvp_collection_get(kvp_collection,  "bbandEntryPageUrl"))) {
						atsc3_sls_html_entry_package->bband_entry_page_url = matching_attribute;
					}
					if((matching_attribute = kvp_collection_get(kvp_collection,  "validFrom"))) {
						atsc3_sls_html_entry_package->valid_from_s = matching_attribute;
					}
					if((matching_attribute = kvp_collection_get(kvp_collection,  "validUntil"))) {
						atsc3_sls_html_entry_package->valid_until_s = matching_attribute;
					}
					if((matching_attribute = kvp_collection_get(kvp_collection,  "coupledServices"))) {
						atsc3_sls_html_entry_package->coupled_services_s = matching_attribute;
					}
					if((matching_attribute = kvp_collection_get(kvp_collection,  "lctTSIRef"))) {
						atsc3_sls_html_entry_package->lct_tsi_ref = matching_attribute;
					}

					if(atsc3_sls_html_entry_package) {
						atsc3_sls_held_fragment_add_atsc3_sls_html_entry_package(atsc3_sls_held_fragment, atsc3_sls_html_entry_package);

					}
				}
			}

		}
	}

	return atsc3_sls_held_fragment;
}

void atsc3_sls_held_fragment_dump(atsc3_sls_held_fragment_t* atsc3_sls_held_fragment) {
	_ATSC3_HELD_PARSER_INFO("---dumping held");
	for(int i=0; i < atsc3_sls_held_fragment->atsc3_sls_html_entry_package_v.count; i++) {
		atsc3_sls_html_entry_package_t* atsc3_sls_html_entry_package = atsc3_sls_held_fragment->atsc3_sls_html_entry_package_v.data[i];
		_ATSC3_HELD_PARSER_INFO("Dumping HELD: %i, appContextId: %s, bbandEntryPageUrl: %s",
				i, atsc3_sls_html_entry_package->app_context_id, atsc3_sls_html_entry_package->bband_entry_page_url);
	}

}


#ifdef __cplusplus
}; //extern "C"
#endif
