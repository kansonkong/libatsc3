/*
 * atsc3_aeat_parser.c
 *
 *  Created on: Sep 18, 2019
 *      Author: jjustman
 */

#include "atsc3_aeat_parser.h"
#include "xml.h"


int _AEAT_PARSER_INFO_ENABLED = 1;
int _AEAT_PARSER_DEBUG_ENABLED = 0;
int _AEAT_PARSER_TRACE_ENABLED = 0;



int atsc3_aeat_table_populate_from_xml(lls_table_t* lls_table, xml_node_t* xml_root) {
    int ret = 0;


	xml_string_t* root_node_name = xml_node_name(xml_root); //root
	dump_xml_string(root_node_name);

	uint8_t* aeat_attributes = xml_attributes_clone(root_node_name);

    __AEAT_PARSER_DEBUG("atsc3_aeat_table_populate_from_xml: parsing aeat_attributes:\n%s", aeat_attributes);

	kvp_collection_t* aeat_attributes_collecton = kvp_collection_parse(aeat_attributes);
	char* bsid_char = kvp_collection_get(aeat_attributes_collecton, "bsid");

    return ret;
}

