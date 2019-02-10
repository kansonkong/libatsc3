/*
 * atsc3_lls_alc_utils.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#include "atsc3_lls_alc_utils.h"

lls_session_t* lls_session_create() {
	lls_session_t* lls_session = calloc(1, sizeof(*lls_session));
	assert(lls_session);

	//do not instantiate any other lls_table_xxx types, as they will need to be assigned
	//do not instanitate any lls_slt_alc_sessions, they will be created later

	lls_session->lls_slt_alc_sessions = NULL;
	lls_session->lls_slt_alc_sessions_n = 0;

	return lls_session;
}

void lls_session_free(lls_session_t** lls_session_ptr) {
	lls_session_t* lls_session = *lls_session_ptr;
	if(lls_session) {

		free(lls_session);
	}
	*lls_session_ptr = NULL;
}

lls_slt_alc_session_t* lls_slt_alc_session_create(lls_service_t* lls_service) {
	lls_slt_alc_session_t* lls_slt_alc_session = calloc(1, sizeof(lls_slt_alc_session_t));

	lls_slt_alc_session->service_id = lls_service->service_id;

	lls_slt_alc_session->alc_arguments = calloc(1, sizeof(alc_arguments_t));
	lls_slt_alc_session->sls_source_ip_address = parseIpAddressIntoIntval(lls_service->broadcast_svc_signaling.sls_source_ip_address);
	lls_slt_alc_session->sls_destination_ip_address = parseIpAddressIntoIntval(lls_service->broadcast_svc_signaling.sls_destination_ip_address);
	lls_slt_alc_session->sls_destination_udp_port = parsePortIntoIntval(lls_service->broadcast_svc_signaling.sls_destination_udp_port);

	__LLSU_TRACE("adding sls_source ip: %s as: %u.%u.%u.%u| dest: %s:%s as: %u.%u.%u.%u:%u (%u:%u)",
			lls_service->broadcast_svc_signaling.sls_source_ip_address,
			__toipnonstruct(lls_slt_alc_session->sls_source_ip_address),
			lls_service->broadcast_svc_signaling.sls_destination_ip_address,
			lls_service->broadcast_svc_signaling.sls_destination_udp_port,
			__toipandportnonstruct(lls_slt_alc_session->sls_destination_ip_address, lls_slt_alc_session->sls_destination_udp_port),
			lls_slt_alc_session->sls_destination_ip_address, lls_slt_alc_session->sls_destination_udp_port);

	lls_slt_alc_session->alc_session = open_alc_session(lls_slt_alc_session->alc_arguments);

	return lls_slt_alc_session;
}

void lls_slt_alc_session_remove(lls_service_t* lls_service, lls_slt_alc_session_t* lls_slt_alc_session) {
	//noop for now
}


lls_slt_alc_session_t* lls_slt_alc_session_find(lls_session_t* lls_session, lls_service_t* lls_service) {

	uint32_t sls_source_ip_address = parseIpAddressIntoIntval(lls_service->broadcast_svc_signaling.sls_source_ip_address);
	uint32_t sls_destination_ip_address = parseIpAddressIntoIntval(lls_service->broadcast_svc_signaling.sls_destination_ip_address);
	uint16_t sls_destination_udp_port = parsePortIntoIntval(lls_service->broadcast_svc_signaling.sls_destination_udp_port);

	for(int i=0; i < lls_session->lls_slt_alc_sessions_n; i++ ) {
		lls_slt_alc_session_t* lls_slt_alc_session = lls_session->lls_slt_alc_sessions[i];

		__LLSU_TRACE("lls_slt_alc_session_find lls_service: service_id: %hu, src: %s (%u), dest: %s:%s (%u:%u), checking against %hu, dest: %u.%u.%u.%u:%u (%u:%u)",
				lls_service->service_id,
				lls_service->broadcast_svc_signaling.sls_source_ip_address,
				sls_source_ip_address,
				lls_service->broadcast_svc_signaling.sls_destination_ip_address,
				lls_service->broadcast_svc_signaling.sls_destination_udp_port,
				sls_destination_ip_address,
				sls_destination_udp_port,
				lls_slt_alc_session->service_id,
				__toipandportnonstruct(lls_slt_alc_session->sls_destination_ip_address, lls_slt_alc_session->sls_destination_udp_port),
				lls_slt_alc_session->sls_destination_ip_address,
				lls_slt_alc_session->sls_destination_udp_port);

		if(lls_slt_alc_session->service_id == lls_service->service_id &&
		   lls_slt_alc_session->sls_source_ip_address == sls_source_ip_address &&
		   lls_slt_alc_session->sls_destination_ip_address == sls_destination_ip_address &&
		   lls_slt_alc_session->sls_destination_udp_port == sls_destination_udp_port) {
			__LLSU_TRACE("matching, returning with %p", lls_slt_alc_session);
				return lls_slt_alc_session;
			}
		}
	return NULL;
}


lls_slt_alc_session_t* lls_slt_alc_session_find_from_udp_packet(lls_session_t* lls_session, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port) {

	for(int i=0; i < lls_session->lls_slt_alc_sessions_n; i++ ) {
		lls_slt_alc_session_t* lls_slt_alc_session = lls_session->lls_slt_alc_sessions[i];

		if((lls_slt_alc_session->sls_relax_source_ip_check || (!lls_slt_alc_session->sls_relax_source_ip_check && lls_slt_alc_session->sls_source_ip_address == src_ip_addr)) &&
			lls_slt_alc_session->sls_destination_ip_address == dst_ip_addr &&
			lls_slt_alc_session->sls_destination_udp_port == dst_port) {
			__LLSU_TRACE("matching, returning with %p", lls_slt_alc_session);
			return lls_slt_alc_session;
		}
	}

	return NULL;
}


int comparator_lls_slt_alc_session_t(const void *a, const void *b) {
	__LLSU_TRACE("comparator_lls_slt_alc_session_t with %u from %u", ((lls_slt_alc_session_t *)a)->service_id, ((lls_slt_alc_session_t *)b)->service_id);

	if ( ((lls_slt_alc_session_t*)a)->service_id <  ((lls_slt_alc_session_t*)b)->service_id ) return -1;
	if ( ((lls_slt_alc_session_t*)a)->service_id == ((lls_slt_alc_session_t*)b)->service_id ) return  0;
	if ( ((lls_slt_alc_session_t*)a)->service_id >  ((lls_slt_alc_session_t*)b)->service_id ) return  1;

	return 0;
}

lls_slt_alc_session_t* lls_slt_alc_session_find_or_create(lls_session_t* lls_session, lls_service_t* lls_service) {
	lls_slt_alc_session_t* lls_slt_alc_session = lls_slt_alc_session_find(lls_session, lls_service);
	if(!lls_slt_alc_session) {
		if(lls_session->lls_slt_alc_sessions_n && lls_session->lls_slt_alc_sessions) {
			__LLSU_TRACE("*before realloc to %p, %i, adding %u", lls_session->lls_slt_alc_sessions, lls_session->lls_slt_alc_sessions_n, lls_service->service_id);

			lls_session->lls_slt_alc_sessions = realloc(lls_session->lls_slt_alc_sessions, (lls_session->lls_slt_alc_sessions_n + 1) * sizeof(lls_slt_alc_session_t*));
			if(!lls_session->lls_slt_alc_sessions) {
				abort();
			}

			lls_slt_alc_session = lls_session->lls_slt_alc_sessions[lls_session->lls_slt_alc_sessions_n++] = lls_slt_alc_session_create(lls_service);
			if(!lls_slt_alc_session) {
				abort();
			}

			//sort after realloc
			qsort((void**)lls_session->lls_slt_alc_sessions, lls_session->lls_slt_alc_sessions_n, sizeof(lls_slt_alc_session_t**), comparator_lls_slt_alc_session_t);

			__LLSU_TRACE(" *after realloc to %p, %i, adding %u", lls_slt_alc_session, lls_session->lls_slt_alc_sessions_n, lls_service->service_id);

		} else {
			lls_session->lls_slt_alc_sessions = calloc(1, sizeof(lls_slt_alc_session_t*));
			assert(lls_session->lls_slt_alc_sessions);
			lls_session->lls_slt_alc_sessions_n = 1;

			lls_session->lls_slt_alc_sessions[0] = lls_slt_alc_session_create(lls_service);
			assert(lls_session->lls_slt_alc_sessions[0]);

			lls_slt_alc_session = lls_session->lls_slt_alc_sessions[0];
			__LLSU_TRACE("*calloc %p for %u", lls_slt_alc_session, lls_service->service_id);
		}
	}

	lls_slt_alc_session->sls_relax_source_ip_check = __LLS_SESSION_RELAX_SOURCE_IP_CHECK__;

	return lls_slt_alc_session;
}



