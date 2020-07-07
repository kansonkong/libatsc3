/*
 * atsc3_sls_held_fragment.h
 *
 *  Created on: Apr 2, 2019
 *      Author: jjustman
 */

#include <stdbool.h>

#ifndef ATSC3_SLS_HELD_FRAGMENT_H_
#define ATSC3_SLS_HELD_FRAGMENT_H_

#include "atsc3_utils.h"
#include "xml.h"
#include "atsc3_vector_builder.h"

typedef struct atsc3_sls_html_entry_package_required_capabilities {
    //jjustman-2019-09-18 - TODO: impl, also add in free for members in atsc3_sls_held_fragment.c
	bool		__to_impl__;

} atsc3_sls_html_entry_package_required_capabilities_t;

typedef struct atsc3_sls_html_entry_package {
	char* 		app_context_id;
	atsc3_sls_html_entry_package_required_capabilities_t* atsc3_sls_html_entry_package_required_capabilities;
	bool 		app_rendering;

	char* 		clear_app_context_cache_date_s;
	struct tm 	clear_app_context_cache_date;

	char* 		bcast_entry_package_url;
	char* 		bcast_entry_page_url;
	char* 		bband_entry_page_url;

	char*		valid_from_s;
	struct tm 	valid_from;

	char*		valid_until_s;
	struct tm 	valid_util;

	char* 		coupled_services_s; //with spaces inbetween uint16_t;
	char*		lct_tsi_ref;	//with spaces of uint32_t;


} atsc3_sls_html_entry_package_t;

typedef struct atsc3_sls_held_fragment {
	block_t* raw_xml_fragment;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_sls_html_entry_package);
} atsc3_sls_held_fragment_t;


ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_sls_held_fragment, atsc3_sls_html_entry_package);

atsc3_sls_held_fragment_t* atsc3_sls_held_fragment_parse_from_payload(char* payload, char* content_location);
void atsc3_sls_held_fragment_dump(atsc3_sls_held_fragment_t* atsc3_sls_held_fragment);

#define _ATSC3_HELD_PARSER_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_HELD_PARSER_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_HELD_PARSER_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_HELD_PARSER_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);



#endif /* ATSC3_SLS_HELD_FRAGMENT_H_ */
