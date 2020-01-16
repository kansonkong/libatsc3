/*
 * atsc3_route_s_tsid.c
 *
 *  Created on: Apr 2, 2019
 *      Author: jjustman
 */

#include "atsc3_route_s_tsid.h"
#include "atsc3_fdt_parser.h"

int _ROUTE_S_TSID_PARSER_INFO_ENABLED = 0;
int _ROUTE_S_TSID_PARSER_DEBUG_ENABLED = 0;

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_route_s_tsid_RS, atsc3_route_s_tsid_RS_LS)

void atsc3_route_s_tsid_RS_LS_free(atsc3_route_s_tsid_RS_LS_t** atsc3_route_s_tsid_RS_LS_p) {
    if(atsc3_route_s_tsid_RS_LS_p) {
        atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS = *atsc3_route_s_tsid_RS_LS_p;
        if(atsc3_route_s_tsid_RS_LS) {
            freeclean((void**)&atsc3_route_s_tsid_RS_LS->start_time);
            freeclean((void**)&atsc3_route_s_tsid_RS_LS->end_time);
            
            if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow) {
                atsc3_fdt_instance_free(&atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance);
                
                if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo) {
                    if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo) {
                        freeclean((void**)&atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->content_type);
                        freeclean((void**)&atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->rep_id);
                    }
                    freeclean((void**)&atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo);
                    
                    if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia) {
                        freeclean((void**)&atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia->aea_id);
                    }
                    freeclean((void**)&atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia);
                }
                
                if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload) {
                    freeclean((void**)&atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->fec_parms);
                }
                freeclean((void**)&atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload);

                freeclean((void**)&atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow);
            }
            
            if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_RepairFlow) {
                //todo
                freeclean((void**)&atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_RepairFlow);
            }
            
            free(atsc3_route_s_tsid_RS_LS);
            atsc3_route_s_tsid_RS_LS = NULL;
        }
        *atsc3_route_s_tsid_RS_LS_p = NULL;
    }
}
void atsc3_route_s_tsid_RS_free(atsc3_route_s_tsid_RS_t** atsc3_route_s_tsid_RS_p) {
    if(atsc3_route_s_tsid_RS_p) {
        atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_RS = *atsc3_route_s_tsid_RS_p;
        if(atsc3_route_s_tsid_RS) {
            atsc3_route_s_tsid_RS_free_atsc3_route_s_tsid_RS_LS(atsc3_route_s_tsid_RS);
            free(atsc3_route_s_tsid_RS);
            atsc3_route_s_tsid_RS = NULL;
        }
        *atsc3_route_s_tsid_RS_p = NULL;
    }
}
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

		uint8_t* root_child_name_string = xml_string_clone(root_child_name);
		_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("checking root_child tag at: %i, val: %s", i, root_child_name_string);
		freeclean_uint8_t(&root_child_name_string);

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
    //jjustman-2019-11-02: todo: figure out who is freeing interior strings...
    xml_document_free(xml_document, false);
    block_Destroy(&s_tsid_fragment_block);

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
		atsc3_route_s_tsid_RS->dest_ip_addr = parseIpAddressIntoIntval(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "dPort"))) {
		atsc3_route_s_tsid_RS->dest_port = parsePortIntoIntval(matching_attribute);
        free(matching_attribute);
	}
    

	if((matching_attribute = kvp_collection_get(kvp_collection,  "sIpAddr"))) {
		atsc3_route_s_tsid_RS->src_ip_addr = parseIpAddressIntoIntval(matching_attribute);
        free(matching_attribute);
	}

	//parse child nodes
	size_t num_s_tsid_rs_entry_row_children = xml_node_children(xml_rs_node);
	for(int j=0; j < num_s_tsid_rs_entry_row_children; j++) {
		xml_node_t* s_tsid_rs_entry_row_children = xml_node_child(xml_rs_node, j);
		if(xml_node_equals_ignore_case(s_tsid_rs_entry_row_children, "LS")) {
			atsc3_route_s_tsid_parse_RS_LS(s_tsid_rs_entry_row_children, atsc3_route_s_tsid_RS);
		}
	}
    free(xml_attributes);
    kvp_collection_free(kvp_collection);
	return atsc3_route_s_tsid;
}



atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_parse_RS_LS(xml_node_t* xml_rs_ls_node, atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_RS) {
	atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS = atsc3_route_s_tsid_RS_LS_new();
	atsc3_route_s_tsid_RS_add_atsc3_route_s_tsid_RS_LS(atsc3_route_s_tsid_RS, atsc3_route_s_tsid_RS_LS);

	//parse our inner attributes, tsi, bw, startTime, endtime
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_rs_ls_node);
	_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("LS.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "tsi"))) {
		atsc3_route_s_tsid_RS_LS->tsi = atoi(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "bw"))) {
		atsc3_route_s_tsid_RS_LS->bw = atoi(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "startTime"))) {
		atsc3_route_s_tsid_RS_LS->start_time = matching_attribute;
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "endTime"))) {
		atsc3_route_s_tsid_RS_LS->end_time = matching_attribute;
	}

	//parse child nodes
	size_t num_s_tsid_rs_ls_entry_row_children = xml_node_children(xml_rs_ls_node);
	for(int j=0; j < num_s_tsid_rs_ls_entry_row_children; j++) {
		xml_node_t* s_tsid_rs_ls_entry_row_children = xml_node_child(xml_rs_ls_node, j);
		if(xml_node_equals_ignore_case(s_tsid_rs_ls_entry_row_children, "SrcFlow")) {
			atsc3_route_s_tsid_parse_RS_LS_SrcFlow(s_tsid_rs_ls_entry_row_children, atsc3_route_s_tsid_RS_LS);
		} else if(xml_node_equals_ignore_case(s_tsid_rs_ls_entry_row_children, "RepairFlow")) {
			atsc3_route_s_tsid_parse_RS_LS_RepairFlow(s_tsid_rs_ls_entry_row_children, atsc3_route_s_tsid_RS_LS);
		}
	}
    free(xml_attributes);
    kvp_collection_free(kvp_collection);
    
	return atsc3_route_s_tsid_RS;
}


atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_parse_RS_LS_SrcFlow(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS) {


	atsc3_route_s_tsid_RS_LS_SrcFlow_t* atsc3_route_s_tsid_RS_LS_SrcFlow = calloc(1, sizeof(atsc3_route_s_tsid_RS_LS_SrcFlow_t));
	atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow = atsc3_route_s_tsid_RS_LS_SrcFlow;

	//parse our inner attributes, dIpAddr, dPort, dsIpAddr
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_node);
	_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("SrcFlow.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "rt"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow->rt = strncasecmp("true", matching_attribute, 4);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "minBuffSize"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow->min_buff_size = atoi(matching_attribute);
        free(matching_attribute);
	}

	//parse child nodes
	size_t num_s_tsid_rs_ls_SrcFlow_entry_row_children = xml_node_children(xml_node);
	for(int j=0; j < num_s_tsid_rs_ls_SrcFlow_entry_row_children; j++) {
		xml_node_t* s_tsid_rs_ls_SrcFlow_entry_row_children = xml_node_child(xml_node, j);
		if(xml_node_equals_ignore_case(s_tsid_rs_ls_SrcFlow_entry_row_children, "EFDT")) {
			atsc3_route_s_tsid_parse_RS_LS_SrcFlow_EFDT(s_tsid_rs_ls_SrcFlow_entry_row_children, atsc3_route_s_tsid_RS_LS_SrcFlow);
		} else if(xml_node_equals_ignore_case(s_tsid_rs_ls_SrcFlow_entry_row_children, "ContentInfo")) {
			atsc3_route_s_tsid_parse_RS_LS_SrcFlow_ContentInfo(s_tsid_rs_ls_SrcFlow_entry_row_children, atsc3_route_s_tsid_RS_LS_SrcFlow);
		} else if(xml_node_equals_ignore_case(s_tsid_rs_ls_SrcFlow_entry_row_children, "Payload")) {
			atsc3_route_s_tsid_parse_RS_LS_SrcFlow_Payload(s_tsid_rs_ls_SrcFlow_entry_row_children, atsc3_route_s_tsid_RS_LS_SrcFlow);
		}
	}
    free(xml_attributes);
    kvp_collection_free(kvp_collection);
	return atsc3_route_s_tsid_RS_LS;
}

atsc3_route_s_tsid_RS_LS_SrcFlow_t* atsc3_route_s_tsid_parse_RS_LS_SrcFlow_EFDT(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_SrcFlow_t* atsc3_route_s_tsid_RS_LS_SrcFlow) {

	atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_efdt_instance_parse_from_xml_node(xml_node);
	atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance = atsc3_fdt_instance;
	return atsc3_route_s_tsid_RS_LS_SrcFlow;
}

atsc3_route_s_tsid_RS_LS_SrcFlow_t* atsc3_route_s_tsid_parse_RS_LS_SrcFlow_ContentInfo(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_SrcFlow_t* atsc3_route_s_tsid_RS_LS_SrcFlow) {
	//parse out contentInfo/mediaInfo

	xml_string_t* xml_node_name_contentInfo = xml_node_name(xml_node);

	//opening header should be xml
	dump_xml_string(xml_node_name_contentInfo);
	if(!xml_string_equals_ignore_case(xml_node_name_contentInfo, "ContentInfo")) {
		_ATSC3_ROUTE_S_TSID_PARSER_ERROR("atsc3_route_s_tsid_parse_RS_LS_SrcFlow_ContentInfo: opening node name is not ContentInfo, actual: %s", xml_string_clone(xml_node_name_contentInfo));
		return NULL;
	}

	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_t* atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo = calloc(1, sizeof(atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_t));
	atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo;

	size_t num_children = xml_node_children(xml_node);
	for(int j=0; j < num_children; j++) {
		xml_node_t* node_child = xml_node_child(xml_node, j);
		if(xml_node_equals_ignore_case(node_child, "MediaInfo")) {
			atsc3_route_s_tsid_parse_RS_LS_SrcFlow_ContentInfo_MediaInfo(node_child, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo);
		} else if(xml_node_equals_ignore_case(node_child, "AEAMedia")) {
			atsc3_route_s_tsid_parse_RS_LS_SrcFlow_ContentInfo_AEAMedia(node_child, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo);
		}
	}
	return atsc3_route_s_tsid_RS_LS_SrcFlow;
}

atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* atsc3_route_s_tsid_parse_RS_LS_SrcFlow_ContentInfo_MediaInfo(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_t* atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo) {
	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo = calloc(1, sizeof(atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t));
	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo;

	//assign any of our attributes here
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_node);
	_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("MediaInfo.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "contentType"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->content_type = matching_attribute;
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "repId"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->rep_id = matching_attribute;
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "startup"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->startup = strncasecmp("true", matching_attribute, 4);
        free(matching_attribute);
	}

    free(xml_attributes);
    kvp_collection_free(kvp_collection);
	return atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo;
}

atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia_t*  atsc3_route_s_tsid_parse_RS_LS_SrcFlow_ContentInfo_AEAMedia(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_t* atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo) {
	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia_t* atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia = calloc(1, sizeof(atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia_t));
	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia;

	//assign any of our attributes here
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_node);
	_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("AEAMediaInfo.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "AEAId"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia->aea_id = matching_attribute;
	}

    free(xml_attributes);
    kvp_collection_free(kvp_collection);
	return atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia;
}


atsc3_route_s_tsid_RS_LS_SrcFlow_Payload_t* atsc3_route_s_tsid_parse_RS_LS_SrcFlow_Payload(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_SrcFlow_t* atsc3_route_s_tsid_RS_LS_SrcFlow) {
	atsc3_route_s_tsid_RS_LS_SrcFlow_Payload_t* atsc3_route_s_tsid_RS_LS_SrcFlow_Payload = calloc(1, sizeof(atsc3_route_s_tsid_RS_LS_SrcFlow_Payload_t));
	atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload = atsc3_route_s_tsid_RS_LS_SrcFlow_Payload;

	//assign any of our attributes here
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_node);
	_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("SrcFlow.Payload.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "codePoint"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->code_point = atoi(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "formatId"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->format_id = atoi(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "frag"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->frag = atoi(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "order"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->order = matching_attribute;
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "srcFecPayloadId"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->src_fec_payload_id = atoi(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "fecParams"))) {
		atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->fec_parms = matching_attribute;
	}

    free(xml_attributes);
    kvp_collection_free(kvp_collection);
	return atsc3_route_s_tsid_RS_LS_SrcFlow_Payload;
}

/*
 * TODO: jjustman-2019-04-05
 */
atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_parse_RS_LS_RepairFlow(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS) {

	return atsc3_route_s_tsid_RS_LS;
}

void atsc3_route_s_tsid_dump(atsc3_route_s_tsid_t* atsc3_route_s_tsid) {

	if(!atsc3_route_s_tsid) {
		_ATSC3_ROUTE_S_TSID_PARSER_ERROR("atsc3_route_s_tsid is null!");
		return;
	}

	_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("--Dumping S-TSID");

	for(int i=0; i < atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.count; i++) {
		atsc3_route_s_tsid_RS_t*  atsc3_route_s_tsid_RS = atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.data[i];
		_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("S-TSID.RS: RS: dIpAddr: %u, dPort: %u, sIpAddr: %u", atsc3_route_s_tsid_RS->dest_ip_addr, atsc3_route_s_tsid_RS->dest_port, atsc3_route_s_tsid_RS->src_ip_addr);

		for(int j=0; j < atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.count; j++) {
			atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS_t = atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.data[j];
			_ATSC3_ROUTE_S_TSID_PARSER_DEBUG(" S-TSID.RS.LS: bw: %u, tsi: %u, start_time: %s, end_time: %s", atsc3_route_s_tsid_RS_LS_t->bw, atsc3_route_s_tsid_RS_LS_t->tsi, atsc3_route_s_tsid_RS_LS_t->start_time, atsc3_route_s_tsid_RS_LS_t->end_time);

			if(atsc3_route_s_tsid_RS_LS_t->atsc3_route_s_tsid_RS_LS_SrcFlow) {

				if(atsc3_route_s_tsid_RS_LS_t->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo && atsc3_route_s_tsid_RS_LS_t->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo) {

					_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("   S-TSID.RS.LS.SrcFlow.ContentInfo: contentType: %s, repId: %s, startup: %u",
							atsc3_route_s_tsid_RS_LS_t->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->content_type,
							atsc3_route_s_tsid_RS_LS_t->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->rep_id,
							atsc3_route_s_tsid_RS_LS_t->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->startup);

				}
				atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_route_s_tsid_RS_LS_t->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance;
                if(!atsc3_fdt_instance) {
                    _ATSC3_ROUTE_S_TSID_PARSER_WARN("    S-TSID.RS.LS.source_flow present but no fdt-instance element!");
                    return;
                }
				_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("     S-TSID.RS.LS.source_flow.fdt-instance: version: %u, expires: %u, content_type: %s, file_template: %s",
						atsc3_fdt_instance->efdt_vesion,
						atsc3_fdt_instance->expires,
						atsc3_fdt_instance->content_type,
						atsc3_fdt_instance->file_template);

				for(int k=0; k < atsc3_fdt_instance->atsc3_fdt_file_v.count; k++) {
					atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[k];

					_ATSC3_ROUTE_S_TSID_PARSER_DEBUG("     S-TSID.RS.LS.source_flow.fdt-instance.file: content-location: %s, toi: %u, content_length: %u, transfer_length: %u, content_type; %s, content_encoding: %s",
							atsc3_fdt_file->content_location,
							atsc3_fdt_file->toi,
							atsc3_fdt_file->content_length,
							atsc3_fdt_file->transfer_length,
							atsc3_fdt_file->content_type,
							atsc3_fdt_file->content_encoding);
				}
			}
		}
	}
}



