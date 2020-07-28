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

int _HELD_PARSER_INFO_ENABLED = 0;
int _HELD_PARSER_DEBUG_ENABLED = 0;
int _HELD_PARSER_TRACE_ENABLED = 0;

ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_sls_held_fragment);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_sls_held_fragment, atsc3_sls_html_entry_package);

//jjustman-2020-07-27: TODO: change this from char* payload to block_t*
atsc3_sls_held_fragment_t* atsc3_sls_held_fragment_parse_from_payload(char* payload, char* content_locationt) {

	if(!strlen(payload)) {
		_ATSC3_HELD_PARSER_ERROR("HELD fragment is empty, payload ptr: %p", payload);
		return NULL;
	}

    atsc3_sls_held_fragment_t* atsc3_sls_held_fragment = atsc3_sls_held_fragment_new();

    //jjustman-2020-07-07 - if we can't parse this HELD fragment, at least pass back raw_xml_fragment
    atsc3_sls_held_fragment->raw_xml_fragment = block_Promote(payload);

	block_t* held_fragment_block = block_Promote(payload);
	xml_document_t* xml_document = xml_parse_document(held_fragment_block->p_buffer, held_fragment_block->i_pos);

	if(!xml_document) {
        goto error;
	}

	xml_node_t* xml_document_root_node = xml_document_root(xml_document);
	xml_string_t* xml_document_root_node_name = xml_node_name(xml_document_root_node);

	//opening header should be xml
	dump_xml_string(xml_document_root_node_name);
	if(!xml_string_equals_ignore_case(xml_document_root_node_name, "xml")) {
		_ATSC3_HELD_PARSER_ERROR("atsc3_sls_held_fragment_parse_from_payload: opening tag missing xml preamble");
        goto error;
	}

	//we should have at least one HELD and then one HTMLEntityPackage
	//atsc3_sls_held_fragment

	size_t num_root_children = xml_node_children(xml_document_root_node);
	for(int i=0; i < num_root_children; i++) {
		xml_node_t* root_child = xml_node_child(xml_document_root_node, i);
		if(xml_node_equals_ignore_case(root_child, "HELD")) {

			//HTMLEntryPackage
			size_t num_html_entry_packages_children = xml_node_children(root_child);
			for(int j=0; j < num_html_entry_packages_children; j++) {
				xml_node_t* html_entry_packages_node = xml_node_child(root_child, j);
				if(xml_node_equals_ignore_case(html_entry_packages_node, "HTMLEntryPackage")) {

					atsc3_sls_html_entry_package_t* atsc3_sls_html_entry_package = atsc3_sls_html_entry_package_new();

					uint8_t* xml_attributes = xml_attributes_clone_node(html_entry_packages_node);
                    _ATSC3_HELD_PARSER_DEBUG("atsc3_sls_held_fragment_parse_from_payload: raw xml attributes: %s", xml_attributes);
					kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);

					char* matching_attribute = NULL;

                    //strings are duplicated via kvp_collection_get without risk of doublefree
					if((matching_attribute = kvp_collection_get(kvp_collection,  "appContextId"))) {
						atsc3_sls_html_entry_package->app_context_id = matching_attribute;
					}

					if((matching_attribute = kvp_collection_get(kvp_collection,  "appRendering"))) {
						atsc3_sls_html_entry_package->app_rendering = strncmp("true", matching_attribute, 4) == 0;
					}

                    //TODO: jjustman-2019-09-18 - parse this into struct tm
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

                    //TODO: jjustman-2019-09-18 - parse this into struct tm
					if((matching_attribute = kvp_collection_get(kvp_collection,  "validFrom"))) {
						atsc3_sls_html_entry_package->valid_from_s = matching_attribute;
					}
                    
                    //TODO: jjustman-2019-09-18 - parse this into struct tm
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
                    
                    if(kvp_collection) {
                        kvp_collection_free(kvp_collection);
                    }
				}
			}

		}
	}
    goto cleanup;

    
error:
	_ATSC3_HELD_PARSER_WARN("Unable to parse HELD fragment - returning raw_xml_fragment for debugging: %s", atsc3_sls_held_fragment->raw_xml_fragment->p_buffer);
    
cleanup:
    if(held_fragment_block) {
        block_Destroy(&held_fragment_block);
    }
    if(xml_document) {
        xml_document_free(xml_document, false);
        xml_document = NULL;
    }
    
	return atsc3_sls_held_fragment;
    
}

void atsc3_sls_held_fragment_dump(atsc3_sls_held_fragment_t* atsc3_sls_held_fragment) {

	_ATSC3_HELD_PARSER_INFO("HELD fragment dump, raw xml payload is:\n%s", atsc3_sls_held_fragment->raw_xml_fragment->p_buffer);

	for(int i=0; i < atsc3_sls_held_fragment->atsc3_sls_html_entry_package_v.count; i++) {
		atsc3_sls_html_entry_package_t* atsc3_sls_html_entry_package = atsc3_sls_held_fragment->atsc3_sls_html_entry_package_v.data[i];
        //todo: add clear_app_context_cache_date (struct tm)
        _ATSC3_HELD_PARSER_INFO("\tHELD entry: %i, "
                                "appContextId: %s, "
                                "appRendering: %d, "
                                "clear_app_context_cache_date_s: %s, "
                                "bcastEntryPackageUrl: %s, "
                                "bcastEntryPageUrl: %s, "
                                "bbandEntryPageUrl: %s, "
                                "valid_from_s: %s, "
                                "valid_until_s: %s, "
                                "coupled_services_s: %s, "
                                "lct_tsi_ref: %s",
                                i,
                                atsc3_sls_html_entry_package->app_context_id,
                                atsc3_sls_html_entry_package->app_rendering,
                                atsc3_sls_html_entry_package->clear_app_context_cache_date_s,
                                
                                atsc3_sls_html_entry_package->bcast_entry_package_url,
                                atsc3_sls_html_entry_package->bcast_entry_page_url,
                                atsc3_sls_html_entry_package->bband_entry_page_url,
                                
                                atsc3_sls_html_entry_package->valid_from_s,
                                atsc3_sls_html_entry_package->valid_until_s,
                                atsc3_sls_html_entry_package->coupled_services_s,
                                atsc3_sls_html_entry_package->lct_tsi_ref);
	}

}
    
void atsc3_sls_html_entry_package_free(atsc3_sls_html_entry_package_t** atsc3_sls_html_entry_package_p) {
    if(atsc3_sls_html_entry_package_p) {
        atsc3_sls_html_entry_package_t* atsc3_sls_html_entry_package = *atsc3_sls_html_entry_package_p;
        if(atsc3_sls_html_entry_package) {
            if(atsc3_sls_html_entry_package->app_context_id) {
                free(atsc3_sls_html_entry_package->app_context_id);
                atsc3_sls_html_entry_package->app_context_id = NULL;
            }
            
            if(atsc3_sls_html_entry_package->atsc3_sls_html_entry_package_required_capabilities) {
                free(atsc3_sls_html_entry_package->atsc3_sls_html_entry_package_required_capabilities);
                atsc3_sls_html_entry_package->atsc3_sls_html_entry_package_required_capabilities = NULL;
            }
            
            //clear_app_context_cache_date_s
            if(atsc3_sls_html_entry_package->clear_app_context_cache_date_s) {
                free(atsc3_sls_html_entry_package->clear_app_context_cache_date_s);
                atsc3_sls_html_entry_package->clear_app_context_cache_date_s = NULL;
            }
            
            //bcast_entry_package_url
            if(atsc3_sls_html_entry_package->bcast_entry_package_url) {
                free(atsc3_sls_html_entry_package->bcast_entry_package_url);
                atsc3_sls_html_entry_package->bcast_entry_package_url = NULL;
            }
            
            //bcast_entry_page_url
            if(atsc3_sls_html_entry_package->bcast_entry_page_url) {
                free(atsc3_sls_html_entry_package->bcast_entry_page_url);
                atsc3_sls_html_entry_package->bcast_entry_page_url = NULL;
            }
            
            //bband_entry_page_url
            if(atsc3_sls_html_entry_package->bband_entry_page_url) {
                free(atsc3_sls_html_entry_package->bband_entry_page_url);
                atsc3_sls_html_entry_package->bband_entry_page_url = NULL;
            }
            
            //valid_from_s
            if(atsc3_sls_html_entry_package->valid_from_s) {
                free(atsc3_sls_html_entry_package->valid_from_s);
                atsc3_sls_html_entry_package->valid_from_s = NULL;
            }
            
            //valid_until_s
            if(atsc3_sls_html_entry_package->valid_until_s) {
                free(atsc3_sls_html_entry_package->valid_until_s);
                atsc3_sls_html_entry_package->valid_until_s = NULL;
            }
            
            //coupled_services_s
            if(atsc3_sls_html_entry_package->coupled_services_s) {
                free(atsc3_sls_html_entry_package->coupled_services_s);
                atsc3_sls_html_entry_package->coupled_services_s = NULL;
            }
            
            //lct_tsi_ref
            if(atsc3_sls_html_entry_package->lct_tsi_ref) {
                free(atsc3_sls_html_entry_package->lct_tsi_ref);
                atsc3_sls_html_entry_package->lct_tsi_ref = NULL;
            }
            
            //other interior members here
            freesafe(atsc3_sls_html_entry_package);
            atsc3_sls_html_entry_package = NULL;
        }
        *atsc3_sls_html_entry_package_p = NULL;
    }
}


#ifdef __cplusplus
}; //extern "C"
#endif
