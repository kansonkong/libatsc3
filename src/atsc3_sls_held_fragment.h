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
#include "atsc3_vector_builder.h"

typedef struct atsc3_sls_html_entry_package_required_capabilities {
	//todo

} atsc3_sls_html_entry_package_required_capabilities_t;

typedef struct atsc3_sls_html_entry_package {
	char* app_context_id;
	atsc3_sls_html_entry_package_required_capabilities_t* atsc3_sls_html_entry_package_required_capabilities;
	bool app_rendering;
	struct tm clear_app_context_cache_date;
	char* bcast_entry_package_url;
	char* bcast_entry_page_url;
	char* bband_entry_page_url;
	struct tm valid_from;
	struct tm valid_util;
	char* coupled_services_s; //with spaces inbetween uint16_t;
	char*	lct_tsi_ref;	//with spaces of uint32_t;


} atsc3_sls_html_entry_package_t;

typedef struct atsc3_sls_held_fragment {
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_sls_html_entry_package);

} atsc3_sls_held_fragment_t;


ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_sls_held_fragment, atsc3_sls_html_entry_package);

#endif /* ATSC3_SLS_HELD_FRAGMENT_H_ */
