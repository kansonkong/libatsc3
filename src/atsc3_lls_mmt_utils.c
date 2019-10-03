/*
 * atsc3_lls_mmt_utils.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#include "atsc3_lls_mmt_utils.h"


int _LLSU_MMT_TRACE_ENABLED = 0;
int _LLSU_TRACE_ENABLED = 0;

lls_sls_mmt_monitor_t* lls_sls_mmt_monitor_create() {
	lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = (lls_sls_mmt_monitor_t*)calloc(1, sizeof(lls_sls_mmt_monitor_t));

	return lls_sls_mmt_monitor;
}


lls_sls_mmt_session_vector_t* lls_sls_mmt_session_vector_create() {
	lls_sls_mmt_session_vector_t* lls_sls_mmt_session_vector = (lls_sls_mmt_session_vector_t*)calloc(1, sizeof(lls_sls_mmt_session_vector_t));
	assert(lls_sls_mmt_session_vector);

	//do not instantiate any other lls_table_xxx types, as they will need to be assigned
	//do not instanitate any lls_slt_mmt_sessions, they will be created later

	lls_sls_mmt_session_vector->lls_slt_mmt_sessions = NULL;
	lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n = 0;

	return lls_sls_mmt_session_vector;
}


lls_sls_mmt_session_t* lls_slt_mmt_session_create(atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	lls_sls_mmt_session_t* lls_slt_mmt_session = (lls_sls_mmt_session_t*)calloc(1, sizeof(lls_sls_mmt_session_t));

	lls_slt_mmt_session->service_id = atsc3_lls_slt_service->service_id;

	lls_slt_mmt_session->mmt_arguments = (mmt_arguments_t*)calloc(1, sizeof(mmt_arguments_t));
    if(lls_service->broadcast_svc_signaling.sls_source_ip_address) {
        lls_slt_mmt_session->sls_source_ip_address = parseIpAddressIntoIntval(lls_service->broadcast_svc_signaling.sls_source_ip_address);
    }
	lls_slt_mmt_session->sls_destination_ip_address = parseIpAddressIntoIntval(lls_service->broadcast_svc_signaling.sls_destination_ip_address);
	lls_slt_mmt_session->sls_destination_udp_port = parsePortIntoIntval(lls_service->broadcast_svc_signaling.sls_destination_udp_port);

	__LLSU_MMT_TRACE("adding sls_source ip: %s as: %u.%u.%u.%u| dest: %s:%s as: %u.%u.%u.%u:%u (%u:%u)",
			lls_service->broadcast_svc_signaling.sls_source_ip_address,
			__toipnonstruct(lls_slt_mmt_session->sls_source_ip_address),
			lls_service->broadcast_svc_signaling.sls_destination_ip_address,
			lls_service->broadcast_svc_signaling.sls_destination_udp_port,
			__toipandportnonstruct(lls_slt_mmt_session->sls_destination_ip_address, lls_slt_mmt_session->sls_destination_udp_port),
			lls_slt_mmt_session->sls_destination_ip_address, lls_slt_mmt_session->sls_destination_udp_port);
    //noop
	//lls_slt_mmt_session->mmt_session = open_mmt_session(lls_slt_mmt_session->mmt_arguments);

	return lls_slt_mmt_session;
}

void lls_slt_mmt_session_remove(lls_sls_mmt_session_vector_t* lls_slt_mmt_session, atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	//noop for now
}


lls_sls_mmt_session_t* lls_slt_mmt_session_find(lls_sls_mmt_session_vector_t* lls_sls_mmt_session_vector, atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
    uint32_t sls_source_ip_address = 0;
    if(lls_service->broadcast_svc_signaling.sls_source_ip_address) {
       sls_source_ip_address = parseIpAddressIntoIntval(lls_service->broadcast_svc_signaling.sls_source_ip_address);
    }
	uint32_t sls_destination_ip_address = parseIpAddressIntoIntval(lls_service->broadcast_svc_signaling.sls_destination_ip_address);
	uint16_t sls_destination_udp_port = parsePortIntoIntval(lls_service->broadcast_svc_signaling.sls_destination_udp_port);

	for(int i=0; i < lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n; i++ ) {
		lls_sls_mmt_session_t* lls_slt_mmt_session = lls_sls_mmt_session_vector->lls_slt_mmt_sessions[i];

		__LLSU_MMT_TRACE("lls_slt_mmt_session_find lls_service: service_id: %hu, src: %s (%u), dest: %s:%s (%u:%u), checking against %hu, dest: %u.%u.%u.%u:%u (%u:%u)",
				lls_service->service_id,
				(lls_service->broadcast_svc_signaling.sls_source_ip_address ? lls_service->broadcast_svc_signaling.sls_source_ip_address : ""),
				sls_source_ip_address,
				lls_service->broadcast_svc_signaling.sls_destination_ip_address,
				lls_service->broadcast_svc_signaling.sls_destination_udp_port,
				sls_destination_ip_address,
				sls_destination_udp_port,
				lls_slt_mmt_session->service_id,
				__toipandportnonstruct(lls_slt_mmt_session->sls_destination_ip_address, lls_slt_mmt_session->sls_destination_udp_port),
				lls_slt_mmt_session->sls_destination_ip_address,
				lls_slt_mmt_session->sls_destination_udp_port);

        // lls_slt_mmt_session->sls_source_ip_address == sls_source_ip_address &&
		if(lls_slt_mmt_session->service_id == lls_service->service_id &&
           lls_slt_mmt_session->sls_destination_ip_address == sls_destination_ip_address &&
		   lls_slt_mmt_session->sls_destination_udp_port == sls_destination_udp_port) {
			__LLSU_MMT_TRACE("matching, returning with %p", lls_slt_mmt_session);
				return lls_slt_mmt_session;
			}
		}
	return NULL;
}


lls_sls_mmt_session_t* lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor_t* lls_slt_monitor, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port) {

	if(!lls_slt_monitor || !lls_slt_monitor->lls_sls_mmt_session_vector) {
		__LLSU_MMT_TRACE("%s: error, monitor or session vector is NULL: %p, %p", __FUNCTION__, lls_slt_monitor, lls_slt_monitor->lls_sls_mmt_session_vector);
		return NULL;
	}
	lls_sls_mmt_session_vector_t* lls_sls_mmt_session_vector = lls_slt_monitor->lls_sls_mmt_session_vector;

	for(int i=0; i < lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n; i++ ) {
		lls_sls_mmt_session_t* lls_slt_mmt_session = lls_sls_mmt_session_vector->lls_slt_mmt_sessions[i];

		if(lls_slt_mmt_session->sls_destination_ip_address == dst_ip_addr &&
			lls_slt_mmt_session->sls_destination_udp_port == dst_port) {
			__LLSU_MMT_TRACE("matching, returning with %p, dst_ip: %u, dst_port: %u", lls_slt_mmt_session, dst_ip_addr, dst_port);
			return lls_slt_mmt_session;
		}
	}

	return NULL;
}



lls_sls_mmt_session_t* lls_slt_mmt_session_find_from_service_id(lls_slt_monitor_t* lls_slt_monitor, uint16_t service_id) {

	lls_sls_mmt_session_vector_t* lls_sls_mmt_session_vector = lls_slt_monitor->lls_sls_mmt_session_vector;

	for(int i=0; i < lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n; i++ ) {
		lls_sls_mmt_session_t* lls_slt_mmt_session = lls_sls_mmt_session_vector->lls_slt_mmt_sessions[i];

		if(lls_slt_mmt_session->service_id == service_id) {
			__LLSU_MMT_TRACE("matching service_id: %u, returning with %p", lls_slt_mmt_session->service_id, lls_slt_mmt_session);
			return lls_slt_mmt_session;
		}
	}

	return NULL;
}


int comparator_lls_slt_mmt_session_t(const void *a, const void *b) {
	__LLSU_MMT_TRACE("comparator_lls_slt_mmt_session_t with %u from %u", ((lls_sls_mmt_session_t *)a)->service_id, ((lls_sls_mmt_session_t *)b)->service_id);

	if ( ((lls_sls_mmt_session_t*)a)->service_id <  ((lls_sls_mmt_session_t*)b)->service_id ) return -1;
	if ( ((lls_sls_mmt_session_t*)a)->service_id == ((lls_sls_mmt_session_t*)b)->service_id ) return  0;
	if ( ((lls_sls_mmt_session_t*)a)->service_id >  ((lls_sls_mmt_session_t*)b)->service_id ) return  1;

	return 0;
}

lls_sls_mmt_session_t* lls_slt_mmt_session_find_or_create(lls_sls_mmt_session_vector_t* lls_sls_mmt_session_vector, atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	lls_sls_mmt_session_t* lls_slt_mmt_session = lls_slt_mmt_session_find(lls_sls_mmt_session_vector, lls_service);
	if(!lls_slt_mmt_session) {
		if(lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n && lls_sls_mmt_session_vector->lls_slt_mmt_sessions) {
			__LLSU_MMT_TRACE("*before realloc to %p, %i, adding %u", lls_sls_mmt_session_vector->lls_slt_mmt_sessions, lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n, lls_service->service_id);

			lls_sls_mmt_session_vector->lls_slt_mmt_sessions = (lls_sls_mmt_session_t**)realloc(lls_sls_mmt_session_vector->lls_slt_mmt_sessions, (lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n + 1) * sizeof(lls_sls_mmt_session_t*));
			if(!lls_sls_mmt_session_vector->lls_slt_mmt_sessions) {
				abort();
			}

			lls_slt_mmt_session = lls_sls_mmt_session_vector->lls_slt_mmt_sessions[lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n++] = lls_slt_mmt_session_create(lls_service);
			if(!lls_slt_mmt_session) {
				abort();
			}

			//sort after realloc
			qsort((void**)lls_sls_mmt_session_vector->lls_slt_mmt_sessions, lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n, sizeof(lls_sls_mmt_session_t**), comparator_lls_slt_mmt_session_t);

			__LLSU_MMT_TRACE(" *after realloc to %p, %i, adding %u", lls_slt_mmt_session, lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n, lls_service->service_id);

		} else {
			lls_sls_mmt_session_vector->lls_slt_mmt_sessions = (lls_sls_mmt_session_t**)calloc(1, sizeof(lls_sls_mmt_session_t**));
			assert(lls_sls_mmt_session_vector->lls_slt_mmt_sessions);
			lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n = 1;

			lls_sls_mmt_session_vector->lls_slt_mmt_sessions[0] = lls_slt_mmt_session_create(lls_service);
			assert(lls_sls_mmt_session_vector->lls_slt_mmt_sessions[0]);

			lls_slt_mmt_session = lls_sls_mmt_session_vector->lls_slt_mmt_sessions[0];
			__LLSU_MMT_TRACE("*calloc %p for %u", lls_slt_mmt_session, lls_service->service_id);
		}
	}


	return lls_slt_mmt_session;
}


void lls_sls_mmt_session_free(lls_sls_mmt_session_t** lls_sls_mmt_session_ptr) {
    lls_sls_mmt_session_t* lls_sls_mmt_session = *lls_sls_mmt_session_ptr;
    if(lls_sls_mmt_session) {
        
        free(lls_sls_mmt_session);
    }
    *lls_sls_mmt_session_ptr = NULL;
}


