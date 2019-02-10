/*
 * atsc3_lls_slt_utils.c
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */

#include "atsc3_lls_slt_utils.h"

int _LLS_SLT_UTILS_DEBUG_ENABLED=0;

int lls_slt_table_process_update(lls_session_t* lls_session, lls_table_t* lls_table) {

	if(lls_session->lls_table_slt) {
		lls_table_free(lls_session->lls_table_slt);
		lls_session->lls_table_slt = NULL;
	}
	lls_session->lls_table_slt = lls_table;

	for(int i=0; i < lls_table->slt_table.service_entry_n; i++) {
		lls_service_t* lls_service = lls_table->slt_table.service_entry[i];
		_LLS_SLT_UTILS_DEBUG("checking service: %d", lls_service->service_id);

		if(lls_service->broadcast_svc_signaling.sls_protocol == SLS_PROTOCOL_ROUTE) {
			lls_slt_alc_session_t* lls_slt_alc_session = lls_slt_alc_session_find_or_create(lls_session, lls_service);

			//TODO - we probably need to clear out any missing ALC sessions?
			if(!lls_slt_alc_session->alc_session) {
				lls_slt_alc_session_remove(lls_service, lls_slt_alc_session);
				_LLS_SLT_UTILS_ERROR("Unable to instantiate alc session for service_id: %d via SLS_PROTOCOL_ROUTE", lls_service->service_id);
				goto cleanup;
		  	}
		}
	}

	return 0;

cleanup:
	return -1;
}

