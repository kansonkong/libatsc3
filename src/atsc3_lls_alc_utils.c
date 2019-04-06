/*
 * atsc3_lls_alc_utils.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#include "atsc3_lls_alc_utils.h"


int _LLS_ALC_UTILS_INFO_ENABLED = 0;
int _LLS_ALC_UTILS_DEBUG_ENABLED = 0;

lls_sls_alc_monitor_t* lls_sls_alc_monitor_create() {
	lls_sls_alc_monitor_t* lls_sls_alc_monitor = (lls_sls_alc_monitor_t*)calloc(1, sizeof(lls_sls_alc_monitor_t));

	return lls_sls_alc_monitor;
}


lls_sls_alc_session_vector_t* lls_sls_alc_session_vector_create() {
	lls_sls_alc_session_vector_t* lls_sls_alc_session_vector = (lls_sls_alc_session_vector_t*)calloc(1, sizeof(lls_sls_alc_session_vector_t));
	assert(lls_sls_alc_session_vector);

	//do not instantiate any other lls_table_xxx types, as they will need to be assigned
	//do not instanitate any lls_slt_alc_sessions, they will be created later

	lls_sls_alc_session_vector->lls_slt_alc_sessions = NULL;
	lls_sls_alc_session_vector->lls_slt_alc_sessions_n = 0;

	return lls_sls_alc_session_vector;
}

lls_sls_alc_session_t* lls_slt_alc_session_create(lls_service_t* lls_service) {
	lls_sls_alc_session_t* lls_slt_alc_session = (lls_sls_alc_session_t*)calloc(1, sizeof(lls_sls_alc_session_t));

	lls_slt_alc_session->service_id = lls_service->service_id;

	lls_slt_alc_session->alc_arguments = (alc_arguments_t*)calloc(1, sizeof(alc_arguments_t));
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

void lls_slt_alc_session_remove(lls_sls_alc_session_vector_t* lls_slt_alc_session, lls_service_t* lls_service) {
	//noop for now
}


lls_sls_alc_session_t* lls_slt_alc_session_find(lls_sls_alc_session_vector_t* lls_sls_alc_session_vector, lls_service_t* lls_service) {

	uint32_t sls_source_ip_address = parseIpAddressIntoIntval(lls_service->broadcast_svc_signaling.sls_source_ip_address);
	uint32_t sls_destination_ip_address = parseIpAddressIntoIntval(lls_service->broadcast_svc_signaling.sls_destination_ip_address);
	uint16_t sls_destination_udp_port = parsePortIntoIntval(lls_service->broadcast_svc_signaling.sls_destination_udp_port);

	for(int i=0; i < lls_sls_alc_session_vector->lls_slt_alc_sessions_n; i++ ) {
		lls_sls_alc_session_t* lls_slt_alc_session = lls_sls_alc_session_vector->lls_slt_alc_sessions[i];

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


lls_sls_alc_session_t* lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor_t* lls_slt_monitor, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port) {

	lls_sls_alc_session_vector_t* lls_sls_alc_session_vector = lls_slt_monitor->lls_sls_alc_session_vector;

	for(int i=0; i < lls_sls_alc_session_vector->lls_slt_alc_sessions_n; i++ ) {
		lls_sls_alc_session_t* lls_slt_alc_session = lls_sls_alc_session_vector->lls_slt_alc_sessions[i];

		if((lls_slt_alc_session->sls_relax_source_ip_check || (!lls_slt_alc_session->sls_relax_source_ip_check && lls_slt_alc_session->sls_source_ip_address == src_ip_addr)) &&
			lls_slt_alc_session->sls_destination_ip_address == dst_ip_addr &&
			lls_slt_alc_session->sls_destination_udp_port == dst_port) {
			__LLSU_TRACE("matching, returning with %p", lls_slt_alc_session);
			return lls_slt_alc_session;
		}
	}

	return NULL;
}



lls_sls_alc_session_t* lls_slt_alc_session_find_from_service_id(lls_slt_monitor_t* lls_slt_monitor, uint16_t service_id) {

	lls_sls_alc_session_vector_t* lls_sls_alc_session_vector = lls_slt_monitor->lls_sls_alc_session_vector;

	for(int i=0; i < lls_sls_alc_session_vector->lls_slt_alc_sessions_n; i++ ) {
		lls_sls_alc_session_t* lls_slt_alc_session = lls_sls_alc_session_vector->lls_slt_alc_sessions[i];

		if(lls_slt_alc_session->service_id == service_id) {
			__LLSU_TRACE("matching service_id: %u, returning with %p", lls_slt_alc_session->service_id, lls_slt_alc_session);
			return lls_slt_alc_session;
		}
	}

	return NULL;
}


int comparator_lls_slt_alc_session_t(const void *a, const void *b) {
	__LLSU_TRACE("comparator_lls_slt_alc_session_t with %u from %u", ((lls_sls_alc_session_t *)a)->service_id, ((lls_sls_alc_session_t *)b)->service_id);

	if ( ((lls_sls_alc_session_t*)a)->service_id <  ((lls_sls_alc_session_t*)b)->service_id ) return -1;
	if ( ((lls_sls_alc_session_t*)a)->service_id == ((lls_sls_alc_session_t*)b)->service_id ) return  0;
	if ( ((lls_sls_alc_session_t*)a)->service_id >  ((lls_sls_alc_session_t*)b)->service_id ) return  1;

	return 0;
}

lls_sls_alc_session_t* lls_slt_alc_session_find_or_create(lls_sls_alc_session_vector_t* lls_sls_alc_session_vector, lls_service_t* lls_service) {
	lls_sls_alc_session_t* lls_slt_alc_session = lls_slt_alc_session_find(lls_sls_alc_session_vector, lls_service);
	if(!lls_slt_alc_session) {
		if(lls_sls_alc_session_vector->lls_slt_alc_sessions_n && lls_sls_alc_session_vector->lls_slt_alc_sessions) {
			__LLSU_TRACE("*before realloc to %p, %i, adding %u", lls_sls_alc_session_vector->lls_slt_alc_sessions, lls_sls_alc_session_vector->lls_slt_alc_sessions_n, lls_service->service_id);

			lls_sls_alc_session_vector->lls_slt_alc_sessions = (lls_sls_alc_session_t**)realloc(lls_sls_alc_session_vector->lls_slt_alc_sessions, (lls_sls_alc_session_vector->lls_slt_alc_sessions_n + 1) * sizeof(lls_sls_alc_session_t*));
			if(!lls_sls_alc_session_vector->lls_slt_alc_sessions) {
				abort();
			}

			lls_slt_alc_session = lls_sls_alc_session_vector->lls_slt_alc_sessions[lls_sls_alc_session_vector->lls_slt_alc_sessions_n++] = lls_slt_alc_session_create(lls_service);
			if(!lls_slt_alc_session) {
				abort();
			}

			//sort after realloc
			qsort((void**)lls_sls_alc_session_vector->lls_slt_alc_sessions, lls_sls_alc_session_vector->lls_slt_alc_sessions_n, sizeof(lls_sls_alc_session_t**), comparator_lls_slt_alc_session_t);

			__LLSU_TRACE(" *after realloc to %p, %i, adding %u", lls_slt_alc_session, lls_sls_alc_session_vector->lls_slt_alc_sessions_n, lls_service->service_id);

		} else {
            //lls_sls_alc_session_vector->lls_slt_alc_sessions = (lls_sls_alc_session_t**)calloc(1, sizeof(lls_sls_alc_session_t*));

            lls_sls_alc_session_vector->lls_slt_alc_sessions = (lls_sls_alc_session_t**)calloc(1, sizeof(lls_sls_alc_session_t**));
			assert(lls_sls_alc_session_vector->lls_slt_alc_sessions);
			lls_sls_alc_session_vector->lls_slt_alc_sessions_n = 1;

			lls_sls_alc_session_vector->lls_slt_alc_sessions[0] = lls_slt_alc_session_create(lls_service);
			assert(lls_sls_alc_session_vector->lls_slt_alc_sessions[0]);

			lls_slt_alc_session = lls_sls_alc_session_vector->lls_slt_alc_sessions[0];
			__LLSU_TRACE("*calloc %p for %u", lls_slt_alc_session, lls_service->service_id);
		}
	}

	lls_slt_alc_session->sls_relax_source_ip_check = __LLS_SESSION_RELAX_SOURCE_IP_CHECK__;

	return lls_slt_alc_session;
}


void lls_sls_alc_session_free(lls_sls_alc_session_t** lls_sls_alc_session_ptr) {
    lls_sls_alc_session_t* lls_sls_alc_session = *lls_sls_alc_session_ptr;
    if(lls_sls_alc_session) {
        
        free(lls_sls_alc_session);
    }
    *lls_sls_alc_session_ptr = NULL;
}

/**
 * looks like
 *
atsc3_route_s_tsid.c:267:DEBUG:SrcFlow.Payload.attributes: codePoint="128" formatId="1" frag="0" order="true"
atsc3_route_s_tsid.c:313:DEBUG:--Dumping S-TSID
atsc3_route_s_tsid.c:317:DEBUG:S-TSID.RS: RS: dIpAddr: 4026470657, dPort: 8000, sIpAddr: 179044658
atsc3_route_s_tsid.c:320:DEBUG: S-TSID.RS.LS: bw: 10000000, tsi: 1, start_time: (null), end_time: (null)
atsc3_route_s_tsid.c:328:DEBUG:   S-TSID.RS.LS.SrcFlow.ContentInfo: contentType: video, repId: 0, startup: 0
atsc3_route_s_tsid.c:336:DEBUG:     S-TSID.RS.LS.source_flow.fdt-instance: version: 0, expires: 4000000000, content_type: (null), file_template: test-0-$TOI$.mp4v
atsc3_route_s_tsid.c:350:DEBUG:     S-TSID.RS.LS.source_flow.fdt-instance.file: content-location: test-0-init.mp4v, toi: 2100000000, content_length: 0, transfer_length: 0, content_type; (null), content_encoding: (null)
atsc3_route_s_tsid.c:320:DEBUG: S-TSID.RS.LS: bw: 500000, tsi: 2, start_time: (null), end_time: (null)
atsc3_route_s_tsid.c:328:DEBUG:   S-TSID.RS.LS.SrcFlow.ContentInfo: contentType: audio, repId: 1, startup: 0
atsc3_route_s_tsid.c:336:DEBUG:     S-TSID.RS.LS.source_flow.fdt-instance: version: 0, expires: 4000000000, content_type: (null), file_template: test-1-$TOI$.mp4a
atsc3_route_s_tsid.c:350:DEBUG:     S-TSID.RS.LS.source_flow.fdt-instance.file: content-location: test-1-init.mp4a, toi: 2100000000, content_length: 0, transfer_length: 0, content_type; (null), content_encoding: (null)
 *
 */

void lls_sls_alc_update_tsi_toi_from_route_s_tsid(lls_sls_alc_monitor_t* lls_sls_alc_monitor, atsc3_route_s_tsid_t* atsc3_route_s_tsid) {

	if(!lls_sls_alc_monitor || !atsc3_route_s_tsid) {
		_ATSC3_LLS_ALC_UTILS_ERROR("lls_sls_alc_session: %p, atsc3_route_s_tsid:%p returning!", lls_sls_alc_monitor, atsc3_route_s_tsid);
		return;
	}

	char* content_type = NULL;

	for(int i=0; i < atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.count; i++) {
		atsc3_route_s_tsid_RS_t*  atsc3_route_s_tsid_RS = atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.data[i];
		_ATSC3_LLS_ALC_UTILS_DEBUG("S-TSID.RS: RS: dIpAddr: %u, dPort: %u, sIpAddr: %u", atsc3_route_s_tsid_RS->dest_ip_addr, atsc3_route_s_tsid_RS->dest_port, atsc3_route_s_tsid_RS->src_ip_addr);
		for(int j=0; j < atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.count; j++) {
			char* src_flow_content_info_content_type = NULL;

			atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS = atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.data[j];
			_ATSC3_LLS_ALC_UTILS_DEBUG(" S-TSID.RS.LS: bw: %u, tsi: %u, start_time: %s, end_time: %s", 	atsc3_route_s_tsid_RS_LS->bw,
																										atsc3_route_s_tsid_RS_LS->tsi,
																										atsc3_route_s_tsid_RS_LS->start_time,
																										atsc3_route_s_tsid_RS_LS->end_time);

			if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow) {

				if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo && atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo) {
					src_flow_content_info_content_type = atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->content_type;

					_ATSC3_LLS_ALC_UTILS_DEBUG("   S-TSID.RS.LS.SrcFlow.ContentInfo: contentType: %s, repId: %s, startup: %u",
							atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->content_type,
							atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->content_type,
							atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->startup);

				}
				atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance;
				_ATSC3_LLS_ALC_UTILS_DEBUG("     S-TSID.RS.LS.source_flow.fdt-instance: version: %u, expires: %u, content_type: %s, file_template: %s",
						atsc3_fdt_instance->efdt_vesion,
						atsc3_fdt_instance->expires,
						atsc3_fdt_instance->content_type,
						atsc3_fdt_instance->file_template);

				for(int k=0; k < atsc3_fdt_instance->atsc3_fdt_file_v.count; k++) {
					atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[k];


					if(src_flow_content_info_content_type && atsc3_fdt_file->toi && atsc3_route_s_tsid_RS_LS->tsi) {

						if(strncasecmp("audio", src_flow_content_info_content_type, 5) == 0) {
							//update our audio tsi and toi accordingly
							lls_sls_alc_monitor->audio_toi_init = atsc3_fdt_file->toi;
							lls_sls_alc_monitor->audio_tsi = atsc3_route_s_tsid_RS_LS->tsi;

						} else if(strncasecmp("video", src_flow_content_info_content_type, 5) == 0) {
							lls_sls_alc_monitor->video_toi_init = atsc3_fdt_file->toi;
							lls_sls_alc_monitor->video_tsi = atsc3_route_s_tsid_RS_LS->tsi;


						} else {
							_ATSC3_LLS_ALC_UTILS_ERROR("unknown src_flow_content_info_content_type: %s", src_flow_content_info_content_type);
						}

					} else {
						_ATSC3_LLS_ALC_UTILS_ERROR("missing required flow tsi/toi, src_flow_content_info_content_type: %s, tsi: %u, toi: %u for content-location: %s",
								src_flow_content_info_content_type,
								atsc3_fdt_file->toi,
								atsc3_route_s_tsid_RS_LS->tsi,
								atsc3_fdt_file->content_location);
					}

					_ATSC3_LLS_ALC_UTILS_DEBUG("     S-TSID.RS.LS.source_flow.fdt-instance.file: content-location: %s, toi: %u, content_length: %u, transfer_length: %u, content_type; %s, content_encoding: %s",
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

