/*
 * atsc3_lls_mmt_utils.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 *
 * TODO:
 * 	normalize between atsc3_lls_alc_utils.c and this impl
 */

#include "atsc3_lls_mmt_utils.h"

int _LLS_MMT_UTILS_INFO_ENABLED = 0;
int _LLS_MMT_UTILS_DEBUG_ENABLED = 0;
int _LLS_MMT_UTILS_TRACE_ENABLED = 0;


lls_sls_mmt_monitor_t* lls_sls_mmt_monitor_create() {
	lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = lls_sls_mmt_monitor_new();
	return lls_sls_mmt_monitor;
}

//jjustman-2021-04-16 - TODO - fix method name to be lls_sls_mmt_session_create NOT lls_slt_mmt_session_create

lls_sls_mmt_session_t* lls_slt_mmt_session_create(atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	lls_sls_mmt_session_t* lls_slt_mmt_session = lls_sls_mmt_session_new();
	//hack
	lls_slt_mmt_session->sls_relax_source_ip_check = 1;

	//jjustman-2021-04-16 - TODO: move this to transients
	lls_slt_mmt_session->atsc3_lls_slt_service = atsc3_lls_slt_service;
	lls_slt_mmt_session->service_id = atsc3_lls_slt_service->service_id;

	lls_slt_mmt_session->mmt_arguments = (mmt_arguments_t*)calloc(1, sizeof(mmt_arguments_t));

	if(atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count) {
		atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0];

	    if(atsc3_slt_broadcast_svc_signalling->sls_source_ip_address) {
	    	lls_slt_mmt_session->sls_source_ip_address = parseIpAddressIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_source_ip_address);
	    }

	    lls_slt_mmt_session->sls_destination_ip_address = parseIpAddressIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address);
		lls_slt_mmt_session->sls_destination_udp_port = parsePortIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port);

		_ATSC3_LLS_MMT_UTILS_TRACE("adding MMT  sls_source ip: %s as: %u.%u.%u.%u| dest: %s:%s as: %u.%u.%u.%u:%u (%u:%u)",
				atsc3_slt_broadcast_svc_signalling->sls_source_ip_address,
				__toipnonstruct(lls_slt_mmt_session->sls_source_ip_address),
				atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address,
				atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port,
				__toipandportnonstruct(lls_slt_mmt_session->sls_destination_ip_address, lls_slt_mmt_session->sls_destination_udp_port),
				lls_slt_mmt_session->sls_destination_ip_address, lls_slt_mmt_session->sls_destination_udp_port);

	} else {
		_ATSC3_LLS_MMT_UTILS_ERROR("lls_slt_mmt_session_create: SLT parsing of broadcast_svc_signalling for service_id: %u missing!", atsc3_lls_slt_service->service_id);
	}
	//lls_slt_mmt_session->mmt_session = open_mmt_session(lls_slt_mmt_session->mmt_arguments);

	return lls_slt_mmt_session;
}

void lls_slt_mmt_session_remove(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	//noop for now
}


lls_sls_mmt_session_t* lls_slt_mmt_session_find(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service) {

	if(!atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count) {
		_ATSC3_LLS_MMT_UTILS_ERROR("lls_slt_mmt_session_find: SLT parsing of broadcast_svc_signalling for service_id: %u missing!", atsc3_lls_slt_service->service_id);
		return NULL;
	}

	atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0];

	uint32_t sls_source_ip_address = 0;
	if(atsc3_slt_broadcast_svc_signalling->sls_source_ip_address) {
		uint32_t sls_source_ip_address = parseIpAddressIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_source_ip_address);
	}

	uint32_t sls_destination_ip_address = parseIpAddressIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address);
	uint16_t sls_destination_udp_port = parsePortIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port);

	for(int i=0; i < lls_slt_monitor->lls_sls_mmt_session_flows_v.count; i++) {
		lls_sls_mmt_session_flows_t* lls_sls_mmt_session_flows = lls_slt_monitor->lls_sls_mmt_session_flows_v.data[i];

		for(int j=0; j < lls_sls_mmt_session_flows->lls_sls_mmt_session_v.count; j++ ) {
			lls_sls_mmt_session_t* lls_sls_mmt_session = lls_sls_mmt_session_flows->lls_sls_mmt_session_v.data[j];

			_ATSC3_LLS_MMT_UTILS_TRACE("lls_slt_mmt_session_find lls_service: service_id: %hu, src: %s (%u), dest: %s:%s (%u:%u), checking against %hu, dest: %u.%u.%u.%u:%u (%u:%u)",
				atsc3_lls_slt_service->service_id,
				(atsc3_slt_broadcast_svc_signalling->sls_source_ip_address ? atsc3_slt_broadcast_svc_signalling->sls_source_ip_address : ""),
				sls_source_ip_address,
				atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address,
				atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port,
				sls_destination_ip_address,
				sls_destination_udp_port,
				lls_sls_mmt_session->service_id,
				__toipandportnonstruct(lls_sls_mmt_session->sls_destination_ip_address, lls_sls_mmt_session->sls_destination_udp_port),
				lls_sls_mmt_session->sls_destination_ip_address,
				lls_sls_mmt_session->sls_destination_udp_port);

		if(lls_sls_mmt_session->service_id == atsc3_lls_slt_service->service_id &&
			(!sls_source_ip_address || (lls_sls_mmt_session->sls_source_ip_address == sls_source_ip_address)) &&
			lls_sls_mmt_session->sls_destination_ip_address == sls_destination_ip_address &&
			lls_sls_mmt_session->sls_destination_udp_port == sls_destination_udp_port) {
			_ATSC3_LLS_MMT_UTILS_TRACE("matching, returning with %p", lls_sls_mmt_session);
				return lls_sls_mmt_session;
			}
		}
	}
	return NULL;
}

//
//lls_sls_mmt_session_t* lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor_t* lls_slt_monitor, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port) {
//
//	if(!lls_slt_monitor || !lls_slt_monitor->lls_sls_mmt_session_vector) {
//		__LLSU_MMT_TRACE("%s: error, monitor or session vector is NULL: %p, %p", __FUNCTION__, lls_slt_monitor, lls_slt_monitor->lls_sls_mmt_session_vector);
//		return NULL;
//	}
//	lls_sls_mmt_session_vector_t* lls_sls_mmt_session_vector = lls_slt_monitor->lls_sls_mmt_session_vector;
//
//	for(int i=0; i < lls_sls_mmt_session_vector->lls_slt_mmt_sessions_n; i++ ) {
//		lls_sls_mmt_session_t* lls_slt_mmt_session = lls_sls_mmt_session_vector->lls_slt_mmt_sessions[i];
//
//		if(lls_slt_mmt_session->sls_destination_ip_address == dst_ip_addr &&
//			lls_slt_mmt_session->sls_destination_udp_port == dst_port) {
//			__LLSU_MMT_TRACE("matching, returning with %p, dst_ip: %u, dst_port: %u", lls_slt_mmt_session, dst_ip_addr, dst_port);
//			return lls_slt_mmt_session;
//		}
//	}
//
//	return NULL;
//}


lls_sls_mmt_session_t* lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor_t* lls_slt_monitor, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port) {

	_ATSC3_LLS_MMT_UTILS_TRACE("lls_slt_mmt_session_find_from_udp_packet: checking lls_sls_mmt_session_flows_v, count: %d", lls_slt_monitor->lls_sls_mmt_session_flows_v.count);

	for(int i=0; i < lls_slt_monitor->lls_sls_mmt_session_flows_v.count; i++) {
		lls_sls_mmt_session_flows_t* lls_sls_mmt_session_flows = lls_slt_monitor->lls_sls_mmt_session_flows_v.data[i];

		_ATSC3_LLS_MMT_UTILS_TRACE("lls_slt_mmt_session_find_from_udp_packet: checking lls_sls_mmt_session_flows->lls_sls_mmt_session_v.count, count: %d", lls_sls_mmt_session_flows->lls_sls_mmt_session_v.count);

		for(int j=0; j < lls_sls_mmt_session_flows->lls_sls_mmt_session_v.count; j++ ) {
			lls_sls_mmt_session_t* lls_slt_mmt_session = lls_sls_mmt_session_flows->lls_sls_mmt_session_v.data[j];

			if((lls_slt_mmt_session->sls_relax_source_ip_check || (!lls_slt_mmt_session->sls_relax_source_ip_check && lls_slt_mmt_session->sls_source_ip_address == src_ip_addr)) &&
				lls_slt_mmt_session->sls_destination_ip_address == dst_ip_addr &&
				lls_slt_mmt_session->sls_destination_udp_port == dst_port) {
				_ATSC3_LLS_MMT_UTILS_TRACE("lls_slt_mmt_session_find_from_udp_packet: matching, returning with %p", lls_slt_mmt_session);
				return lls_slt_mmt_session;
			} else {
				_ATSC3_LLS_MMT_UTILS_TRACE("lls_slt_mmt_session_find_from_udp_packet: not matching sip: %u to%u, dip: %u to %u, port: %u to %u",
						lls_slt_mmt_session->sls_source_ip_address, src_ip_addr,
						lls_slt_mmt_session->sls_destination_ip_address, dst_ip_addr,
						lls_slt_mmt_session->sls_destination_udp_port, dst_port
						);

			}
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

			//hack
			if(lls_sls_mmt_session->service_id == service_id || (lls_sls_mmt_session->atsc3_lls_slt_service && lls_sls_mmt_session->atsc3_lls_slt_service->service_id == service_id) ) {
				_ATSC3_LLS_MMT_UTILS_TRACE("lls_slt_mmt_session_find_from_service_id: matching service_id: %u, returning with %p",
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
		if(lls_sls_mmt_monitor->transients.atsc3_lls_slt_service && lls_sls_mmt_monitor->transients.atsc3_lls_slt_service->service_id == service_id) {
			if(!lls_sls_mmt_monitor->transients.lls_mmt_session) {
				//jjustman-2019-10-19: TODO - fix me, everyone should have a lls_mmt_session if they are in monitor
				_ATSC3_LLS_MMT_UTILS_ERROR("lls_sls_mmt_monitor_find_from_service_id: %p, service_id: %d, lls_mmt_session is NULL", lls_sls_mmt_monitor, service_id);
			}

			return lls_sls_mmt_monitor;
		}
	}

	//if we aren't monitoring this session, this is OK..
	_ATSC3_LLS_MMT_UTILS_TRACE("lls_sls_mmt_monitor_find_from_service_id: returning lls_sls_mmt_monitor_t NULL for lls_slt_monitor: %p, service_id: %u", lls_slt_monitor, service_id);

	return NULL;
}


int comparator_lls_slt_mmt_session_t(const void *a, const void *b) {
	_ATSC3_LLS_MMT_UTILS_TRACE("comparator_lls_slt_mmt_session_t with %u from %u", ((lls_sls_mmt_session_t *)a)->service_id, ((lls_sls_mmt_session_t *)b)->service_id);

	if ( ((lls_sls_mmt_session_t*)a)->service_id <  ((lls_sls_mmt_session_t*)b)->service_id ) return -1;
	if ( ((lls_sls_mmt_session_t*)a)->service_id == ((lls_sls_mmt_session_t*)b)->service_id ) return  0;
	if ( ((lls_sls_mmt_session_t*)a)->service_id >  ((lls_sls_mmt_session_t*)b)->service_id ) return  1;

	return 0;
}

/*
 SLS flows for MMT are instantiated in atsc3_lls_slt_parser: int lls_slt_table_perform_update(lls_table_t* lls_table, lls_slt_monitor_t* lls_slt_monitor)
 */
lls_sls_mmt_session_t* lls_slt_mmt_session_find_or_create(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	lls_sls_mmt_session_t* lls_slt_mmt_session = lls_slt_mmt_session_find(lls_slt_monitor, atsc3_lls_slt_service);
	if(!lls_slt_mmt_session) {
		lls_slt_mmt_session = lls_slt_mmt_session_create(atsc3_lls_slt_service);

		lls_sls_mmt_session_flows_t* lls_sls_mmt_session_flows = lls_sls_mmt_session_flows_new();
		lls_sls_mmt_session_flows_add_lls_sls_mmt_session(lls_sls_mmt_session_flows, lls_slt_mmt_session);

		lls_slt_monitor_add_lls_sls_mmt_session_flows(lls_slt_monitor, lls_sls_mmt_session_flows);
	} else {

        for(int k=0; k < lls_slt_monitor->lls_sls_mmt_monitor_v.count; k++) {
            lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = lls_slt_monitor->lls_sls_mmt_monitor_v.data[k];

            if(lls_sls_mmt_monitor && lls_sls_mmt_monitor->transients.atsc3_lls_slt_service && lls_sls_mmt_monitor->transients.atsc3_lls_slt_service->service_id == atsc3_lls_slt_service->service_id) {
                lls_sls_mmt_monitor->transients.atsc3_lls_slt_service = atsc3_lls_slt_service;
                lls_sls_mmt_monitor->transients.atsc3_lls_slt_service_stale = NULL;
            }
        }

        lls_slt_mmt_session->atsc3_lls_slt_service = atsc3_lls_slt_service;
        lls_slt_mmt_session->transients.atsc3_lls_slt_service_stale = NULL;
	}

	return lls_slt_mmt_session;
}

void lls_slt_mmt_session_and_monitor_mark_all_atsc3_lls_slt_service_as_transient_stale(lls_slt_monitor_t* lls_slt_monitor) {
    for(int i=0; i < lls_slt_monitor->lls_sls_mmt_session_flows_v.count; i++) {
        lls_sls_mmt_session_flows_t* lls_sls_mmt_session_flows = lls_slt_monitor->lls_sls_mmt_session_flows_v.data[i];

        for(int j=0; j < lls_sls_mmt_session_flows->lls_sls_mmt_session_v.count; j++ ) {
            lls_sls_mmt_session_t* lls_slt_mmt_session = lls_sls_mmt_session_flows->lls_sls_mmt_session_v.data[j];
            lls_slt_mmt_session->transients.atsc3_lls_slt_service_stale = lls_slt_mmt_session->atsc3_lls_slt_service;

            for(int k=0; k < lls_slt_monitor->lls_sls_mmt_monitor_v.count; k++) {
                lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = lls_slt_monitor->lls_sls_mmt_monitor_v.data[k];
                lls_sls_mmt_monitor->transients.atsc3_lls_slt_service_stale = lls_sls_mmt_monitor->transients.atsc3_lls_slt_service;
            }
        }
    }
}


void lls_slt_mmt_session_and_monitor_remove_all_atsc3_lls_slt_service_with_matching_transient_stale(lls_slt_monitor_t* lls_slt_monitor) {
    for(int i=0; i < lls_slt_monitor->lls_sls_mmt_session_flows_v.count; i++) {
        lls_sls_mmt_session_flows_t* lls_sls_mmt_session_flows = lls_slt_monitor->lls_sls_mmt_session_flows_v.data[i];

        for(int j=0; j < lls_sls_mmt_session_flows->lls_sls_mmt_session_v.count; j++ ) {
            lls_sls_mmt_session_t* lls_sls_mmt_session = lls_sls_mmt_session_flows->lls_sls_mmt_session_v.data[j];

            for(int k=0; k < lls_slt_monitor->lls_sls_mmt_monitor_v.count; k++) {
                lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = lls_slt_monitor->lls_sls_mmt_monitor_v.data[k];

                if(lls_sls_mmt_monitor->transients.atsc3_lls_slt_service && lls_sls_mmt_monitor->transients.atsc3_lls_slt_service_stale && lls_sls_mmt_monitor->transients.atsc3_lls_slt_service == lls_sls_mmt_monitor->transients.atsc3_lls_slt_service_stale) {
                    //remove this monitor

                    //jjustman-2021-03-10 - hack workaround here
                    lls_sls_mmt_monitor->transients.atsc3_lls_slt_service = NULL;
                    lls_sls_mmt_monitor->transients.atsc3_lls_slt_service_stale = NULL;
                }
            }

            //lls_sls_alc_session: we didn't update our atsc3_lls_slt_service and the pointer is stale
            if(lls_sls_mmt_session->atsc3_lls_slt_service && lls_sls_mmt_session->transients.atsc3_lls_slt_service_stale && lls_sls_mmt_session->atsc3_lls_slt_service == lls_sls_mmt_session->transients.atsc3_lls_slt_service_stale) {
                //remove immediately

                //jjustman-2021-03-10 - hack workaround here
                lls_sls_mmt_session->atsc3_lls_slt_service = NULL;
                lls_sls_mmt_session->transients.atsc3_lls_slt_service_stale = NULL;
            }
        }
    }
}
