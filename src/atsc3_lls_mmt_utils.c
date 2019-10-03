/*
 * atsc3_lls_mmt_utils.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 *
 * TODO:
 * 	normaliE between atsc3_lls_alc_utils.c and this impl
 */

#include "atsc3_lls_mmt_utils.h"

int _LLS_MMT_UTILS_INFO_ENABLED = 0;
int _LLS_MMT_UTILS_DEBUG_ENABLED = 0;
int _LLS_MMT_UTILS_TRACE_ENABLED = 0;


lls_sls_mmt_monitor_t* lls_sls_mmt_monitor_create() {
	lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = lls_sls_mmt_monitor_new();
	return lls_sls_mmt_monitor;
}


lls_sls_mmt_session_t* lls_slt_mmt_session_create(atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	lls_sls_mmt_session_t* lls_slt_mmt_session = lls_sls_mmt_session_new();

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
	if(!lls_slt_monitor) {
		_ATSC3_LLS_MMT_UTILS_ERROR("lls_slt_mmt_session_find_from_service_id, lls_slt_monitor is NULL, service_id: %d", service_id);
		return NULL;
	}

	for(int i=0; i < lls_slt_monitor->lls_sls_mmt_session_flows_v.count; i++) {
		lls_sls_mmt_session_flows_t* lls_sls_mmt_session_flows = lls_slt_monitor->lls_sls_mmt_session_flows_v.data[i];

		for(int j=0; j < lls_sls_mmt_session_flows->lls_sls_mmt_session_v.count; j++ ) {
			lls_sls_mmt_session_t* lls_sls_mmt_session = lls_sls_mmt_session_flows->lls_sls_mmt_session_v.data[j];

			if(lls_sls_mmt_session->service_id == service_id) {
				_ATSC3_LLS_ALC_UTILS_TRACE("lls_slt_mmt_session_find_from_service_id: matching service_id: %u, returning with %p",
						lls_sls_mmt_session->service_id, lls_sls_mmt_session);
				return lls_sls_mmt_session;
			}
		}
	}
	return NULL;
}



lls_sls_mmt_monitor_t* lls_sls_mmt_monitor_find_from_service_id(lls_slt_monitor_t* lls_slt_monitor, uint16_t service_id) {
	if(!lls_slt_monitor) {
		_ATSC3_LLS_MMT_UTILS_ERROR("lls_sls_mmt_monitor_find_from_service_id, lls_slt_monitor is NULL, service_id: %d", service_id);
		return NULL;
	}

	for(int i=0; i < lls_slt_monitor->lls_sls_mmt_monitor_v.count; i++) {
		lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = lls_slt_monitor->lls_sls_mmt_monitor_v.data[i];
		if(lls_sls_mmt_monitor->atsc3_lls_slt_service && lls_sls_mmt_monitor->atsc3_lls_slt_service->lls_sls_mmt_monitor->atsc3_lls_slt_service == service_id) {
			if(!lls_sls_mmt_monitor->lls_mmt_session) {
				_ATSC3_LLS_MMT_UTILS_WARN("lls_sls_mmt_monitor_find_from_service_id: %p, service_id: %d, lls_mmt_session is NULL", lls_sls_mmt_monitor, service_id);
			}
			if(!lls_sls_mmt_monitor->audio_packet_id) {
				_ATSC3_LLS_MMT_UTILS_WARN("lls_sls_mmt_monitor_find_from_service_id: %p, service_id: %d, audio_packet_id is NULL", lls_sls_mmt_monitor, service_id);
			}
			if(!lls_sls_mmt_monitor->video_packet_id) {
				_ATSC3_LLS_MMT_UTILS_WARN("lls_sls_mmt_monitor_find_from_service_id: %p, service_id: %d, video_packet_id is NULL", lls_sls_mmt_monitor, service_id);
			}

			return lls_sls_mmt_monitor;
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


lls_sls_mmt_session_t* lls_slt_mmt_session_find_or_create(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	lls_sls_mmt_session_t* lls_slt_mmt_session = lls_slt_mmt_session_find(lls_slt_monitor, lls_service);
	if(!lls_slt_mmt_session) {
		lls_slt_mmt_session = lls_slt_mmt_session_create(atsc3_lls_slt_service);

		lls_sls_mmt_session_flows_t* lls_sls_mmt_session_flows = lls_sls_mmt_session_flows_new();
		lls_sls_mmtsession_flows_add_lls_sls_mmt_session(lls_sls_mmt_session_flows, lls_slt_mmt_session);

		lls_slt_monitor_add_lls_sls_mmt_session_flows(lls_slt_monitor, lls_sls_mmt_session_flows);
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


