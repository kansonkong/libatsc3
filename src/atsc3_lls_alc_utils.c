/*
 * atsc3_lls_alc_utils.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#include "atsc3_lls_alc_utils.h"

int _LLS_ALC_UTILS_INFO_ENABLED = 0;
int _LLS_ALC_UTILS_DEBUG_ENABLED = 0;
int _LLS_ALC_UTILS_TRACE_ENABLED = 0;

lls_sls_alc_monitor_t* lls_sls_alc_monitor_create() {
	lls_sls_alc_monitor_t* lls_sls_alc_monitor = lls_sls_alc_monitor_new();
	return lls_sls_alc_monitor;
}

lls_sls_alc_session_t* lls_slt_alc_session_create(atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	lls_sls_alc_session_t* lls_slt_alc_session = lls_sls_alc_session_new();

	lls_slt_alc_session->atsc3_lls_slt_service = atsc3_lls_slt_service;
	lls_slt_alc_session->service_id = atsc3_lls_slt_service->service_id;

	lls_slt_alc_session->alc_arguments = (atsc3_alc_arguments_t*)calloc(1, sizeof(atsc3_alc_arguments_t));

	if(atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count) {
		atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0];

	    if(atsc3_slt_broadcast_svc_signalling->sls_source_ip_address) {
	    	lls_slt_alc_session->sls_source_ip_address = parseIpAddressIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_source_ip_address);
	    }
		lls_slt_alc_session->sls_destination_ip_address = parseIpAddressIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address);
		lls_slt_alc_session->sls_destination_udp_port = parsePortIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port);

		_ATSC3_LLS_ALC_UTILS_TRACE("adding ALC sls_source ip: %s as: %u.%u.%u.%u| dest: %s:%s as: %u.%u.%u.%u:%u (%u:%u)",
				atsc3_slt_broadcast_svc_signalling->sls_source_ip_address,
				__toipnonstruct(lls_slt_alc_session->sls_source_ip_address),
				atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address,
				atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port,
				__toipandportnonstruct(lls_slt_alc_session->sls_destination_ip_address, lls_slt_alc_session->sls_destination_udp_port),
				lls_slt_alc_session->sls_destination_ip_address,
				lls_slt_alc_session->sls_destination_udp_port);
	} else {
		_ATSC3_LLS_ALC_UTILS_ERROR("lls_slt_alc_session_create: SLT parsing of broadcast_svc_signalling for service_id: %u missing!", atsc3_lls_slt_service->service_id);
	}
	lls_slt_alc_session->alc_session = atsc3_open_alc_session(lls_slt_alc_session->alc_arguments);

	return lls_slt_alc_session;
}

void lls_slt_alc_session_remove(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	//noop for now
}


lls_sls_alc_session_t* lls_slt_alc_session_find(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service) {

	if(!atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count) {
		_ATSC3_LLS_ALC_UTILS_ERROR("lls_slt_alc_session_find: SLT parsing of broadcast_svc_signalling for service_id: %u missing!", atsc3_lls_slt_service->service_id);
		return NULL;
	}

	atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0];

	uint32_t sls_source_ip_address = 0;
	if(atsc3_slt_broadcast_svc_signalling->sls_source_ip_address) {
		uint32_t sls_source_ip_address = parseIpAddressIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_source_ip_address);
	}

	uint32_t sls_destination_ip_address = parseIpAddressIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address);
	uint16_t sls_destination_udp_port = parsePortIntoIntval(atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port);

	for(int i=0; i < lls_slt_monitor->lls_sls_alc_session_flows_v.count; i++) {
		lls_sls_alc_session_flows_t* lls_sls_alc_session_flows = lls_slt_monitor->lls_sls_alc_session_flows_v.data[i];

		for(int j=0; j < lls_sls_alc_session_flows->lls_sls_alc_session_v.count; j++ ) {
			lls_sls_alc_session_t* lls_sls_alc_session = lls_sls_alc_session_flows->lls_sls_alc_session_v.data[j];

			_ATSC3_LLS_ALC_UTILS_TRACE("lls_slt_alc_session_find lls_service: service_id: %hu, src: %s (%u), dest: %s:%s (%u:%u), checking against %hu, dest: %u.%u.%u.%u:%u (%u:%u)",
					atsc3_lls_slt_service->service_id,
					atsc3_slt_broadcast_svc_signalling->sls_source_ip_address,
					sls_source_ip_address,
					atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address,
					atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port,
					sls_destination_ip_address,
					sls_destination_udp_port,
					lls_sls_alc_session->service_id,
					__toipandportnonstruct(lls_sls_alc_session->sls_destination_ip_address, lls_sls_alc_session->sls_destination_udp_port),
					lls_sls_alc_session->sls_destination_ip_address,
					lls_sls_alc_session->sls_destination_udp_port);

			if(lls_sls_alc_session->service_id == atsc3_lls_slt_service->service_id &&
			   (!sls_source_ip_address || (lls_sls_alc_session->sls_source_ip_address == sls_source_ip_address)) &&
			   lls_sls_alc_session->sls_destination_ip_address == sls_destination_ip_address &&
			   lls_sls_alc_session->sls_destination_udp_port == sls_destination_udp_port) {
				_ATSC3_LLS_ALC_UTILS_TRACE("matching, returning with %p", lls_sls_alc_session);
					return lls_sls_alc_session;
			}
		}
	}
	return NULL;
}

/*
 
 jjustman-2020-03-25 - workaround warning: set our lls_slt_monitor->lls_sls_alc_monitor if we have a matching session
 TODO: deprecate this method and instead use atsc3_lls_sls_alc_monitor_find_from_udp_packet(0
 
 lls_sls_alc_monitor
 
 */

lls_sls_alc_session_t* lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor_t* lls_slt_monitor, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port) {

	for(int i=0; i < lls_slt_monitor->lls_sls_alc_session_flows_v.count; i++) {
		lls_sls_alc_session_flows_t* lls_sls_alc_session_flows = lls_slt_monitor->lls_sls_alc_session_flows_v.data[i];

		for(int j=0; j < lls_sls_alc_session_flows->lls_sls_alc_session_v.count; j++ ) {
			lls_sls_alc_session_t* lls_slt_alc_session = lls_sls_alc_session_flows->lls_sls_alc_session_v.data[j];

			if((lls_slt_alc_session->sls_relax_source_ip_check || (!lls_slt_alc_session->sls_relax_source_ip_check && lls_slt_alc_session->sls_source_ip_address == src_ip_addr)) &&
				lls_slt_alc_session->sls_destination_ip_address == dst_ip_addr && lls_slt_alc_session->sls_destination_udp_port == dst_port) {
				_ATSC3_LLS_ALC_UTILS_TRACE("lls_slt_alc_session_find_from_udp_packet: checking lls_slt_monitor->lls_sls_alc_monitor");
			
				for(int k=0; k < lls_slt_monitor->lls_sls_alc_monitor_v.count; k++) {
					lls_sls_alc_monitor_t* lls_sls_alc_monitor = lls_slt_monitor->lls_sls_alc_monitor_v.data[k];
					_ATSC3_LLS_ALC_UTILS_TRACE("lls_slt_alc_session_find_from_udp_packet: checking lls_slt_monitor->lls_sls_alc_monitor_v.data[%d] from: %p (service_id: %d) with: %p (service_id: %d)",
											  k,
											  lls_slt_monitor->lls_sls_alc_monitor,
											  lls_slt_monitor->lls_sls_alc_monitor->atsc3_lls_slt_service->service_id,
											  lls_sls_alc_monitor,
											  lls_sls_alc_monitor->atsc3_lls_slt_service->service_id);

					if(lls_sls_alc_monitor->atsc3_lls_slt_service && lls_sls_alc_monitor->atsc3_lls_slt_service->service_id == lls_slt_alc_session->service_id) {
						_ATSC3_LLS_ALC_UTILS_TRACE("lls_slt_alc_session_find_from_udp_packet: updating lls_slt_monitor->lls_sls_alc_monitor from: %p (service_id: %d) to: %p (service_id: %d)",
												  lls_slt_monitor->lls_sls_alc_monitor, lls_slt_monitor->lls_sls_alc_monitor->atsc3_lls_slt_service->service_id,
												  lls_sls_alc_monitor, lls_sls_alc_monitor->atsc3_lls_slt_service->service_id);
						lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor;
					}
				}
				_ATSC3_LLS_ALC_UTILS_TRACE("lls_slt_alc_session_find_from_udp_packet: matching, returning with %p", lls_slt_alc_session);

				return lls_slt_alc_session;
			}
		}
	}
	return NULL;
}


lls_sls_alc_monitor_t* atsc3_lls_sls_alc_monitor_find_from_udp_packet(lls_slt_monitor_t* lls_slt_monitor, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port) {
	for(int i=0; i < lls_slt_monitor->lls_sls_alc_session_flows_v.count; i++) {
		lls_sls_alc_session_flows_t* lls_sls_alc_session_flows = lls_slt_monitor->lls_sls_alc_session_flows_v.data[i];

		for(int j=0; j < lls_sls_alc_session_flows->lls_sls_alc_session_v.count; j++ ) {
			lls_sls_alc_session_t* lls_slt_alc_session = lls_sls_alc_session_flows->lls_sls_alc_session_v.data[j];

			if((lls_slt_alc_session->sls_relax_source_ip_check || (!lls_slt_alc_session->sls_relax_source_ip_check && lls_slt_alc_session->sls_source_ip_address == src_ip_addr)) &&
				lls_slt_alc_session->sls_destination_ip_address == dst_ip_addr && lls_slt_alc_session->sls_destination_udp_port == dst_port) {
				_ATSC3_LLS_ALC_UTILS_TRACE("lls_slt_alc_session_find_from_udp_packet: matching, returning with %p", lls_slt_alc_session);

				for(int k=0; k < lls_slt_monitor->lls_sls_alc_monitor_v.count; k++) {
					lls_sls_alc_monitor_t* lls_sls_alc_monitor = lls_slt_monitor->lls_sls_alc_monitor_v.data[k];
					if(lls_sls_alc_monitor->atsc3_lls_slt_service && lls_sls_alc_monitor->atsc3_lls_slt_service->service_id == lls_slt_alc_session->service_id) {
						return lls_sls_alc_monitor;
					}
				}
			}
		}
	}
	return NULL;
}

lls_sls_alc_session_t* lls_slt_alc_session_find_from_service_id(lls_slt_monitor_t* lls_slt_monitor, uint16_t service_id) {
	if(!lls_slt_monitor) {
		_ATSC3_LLS_ALC_UTILS_ERROR("lls_slt_alc_session_find_from_service_id: lls_slt_monitor is NULL, service_id: %d", service_id);
		return NULL;
	}

	for(int i=0; i < lls_slt_monitor->lls_sls_alc_session_flows_v.count; i++) {
		lls_sls_alc_session_flows_t* lls_sls_alc_session_flows = lls_slt_monitor->lls_sls_alc_session_flows_v.data[i];

		for(int j=0; j < lls_sls_alc_session_flows->lls_sls_alc_session_v.count; j++ ) {
			lls_sls_alc_session_t* lls_sls_alc_session = lls_sls_alc_session_flows->lls_sls_alc_session_v.data[j];

			if(lls_sls_alc_session->service_id == service_id) {
				_ATSC3_LLS_ALC_UTILS_TRACE("lls_slt_alc_session_find_from_service_id: matching service_id: %u, returning with %p",
						lls_sls_alc_session->service_id, lls_sls_alc_session);
				return lls_sls_alc_session;
			}
		}
	}
	return NULL;
}


int comparator_lls_slt_alc_session_t(const void *a, const void *b) {
	_ATSC3_LLS_ALC_UTILS_TRACE("comparator_lls_slt_alc_session_t with %u from %u", ((lls_sls_alc_session_t *)a)->service_id, ((lls_sls_alc_session_t *)b)->service_id);
	if ( ((lls_sls_alc_session_t*)a)->service_id <  ((lls_sls_alc_session_t*)b)->service_id ) return -1;
	if ( ((lls_sls_alc_session_t*)a)->service_id == ((lls_sls_alc_session_t*)b)->service_id ) return  0;
	if ( ((lls_sls_alc_session_t*)a)->service_id >  ((lls_sls_alc_session_t*)b)->service_id ) return  1;
	return 0;
}

lls_sls_alc_session_t* lls_slt_alc_session_find_or_create(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service) {
	lls_sls_alc_session_t* lls_slt_alc_session = lls_slt_alc_session_find(lls_slt_monitor, atsc3_lls_slt_service);
	if(!lls_slt_alc_session) {
		lls_slt_alc_session = lls_slt_alc_session_create(atsc3_lls_slt_service);

		lls_sls_alc_session_flows_t* lls_sls_alc_session_flows = lls_sls_alc_session_flows_new();
		lls_sls_alc_session_flows_add_lls_sls_alc_session(lls_sls_alc_session_flows, lls_slt_alc_session);

		lls_slt_monitor_add_lls_sls_alc_session_flows(lls_slt_monitor, lls_sls_alc_session_flows);
		lls_slt_alc_session->sls_relax_source_ip_check = __LLS_SESSION_RELAX_SOURCE_IP_CHECK__;
	}
	return lls_slt_alc_session;
}


void lls_sls_alc_session_free(lls_sls_alc_session_t** lls_sls_alc_session_ptr) {
	_ATSC3_LLS_ALC_UTILS_ERROR("TODO: lls_sls_alc_session_free - should use ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE");
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

/*
 * jjustman-2020-03-10 - patch around RS missing dIpAddr and dPort if missing
 *
 * fixes spec gaps in A/331:2020 with regards to non-SLS carrying flows of media essense observed in the wild...
 *
 * <S-TSID xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/S-TSID/1.0/" xmlns:afdt="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ATSC-FDT/1.0/" xmlns:fdt="urn:ietf:param
s:xml:ns:fdt">
    <RS sIpAddr="172.16.200.1">

    @dIpAddr    0..1 stsid:IPv4addressType      Destination IP address of this ROUTE session; mandatory for ROUTE session other than session carrying SLS (session signaled in SLT); defaults to session carrying SLS.
    @dPort      0..1 unsignedShort              Destination port of this ROUTE session; mandatory for ROUTE session other than session carrying SLS (session signaled in SLT); defaults to session carrying SLS.
 */

void lls_sls_alc_update_s_tsid_RS_dIpAddr_dPort_if_missing(udp_flow_t* udp_flow, lls_sls_alc_monitor_t* lls_sls_alc_monitor, atsc3_route_s_tsid_t* atsc3_route_s_tsid) {
    if(!lls_sls_alc_monitor || !atsc3_route_s_tsid) {
        _ATSC3_LLS_ALC_UTILS_ERROR("lls_sls_alc_session: %p, atsc3_route_s_tsid:%p returning!", lls_sls_alc_monitor, atsc3_route_s_tsid);
        return;
    }

    char* content_type = NULL;

    for(int i=0; i < atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.count; i++) {
        atsc3_route_s_tsid_RS_t *atsc3_route_s_tsid_RS = atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.data[i];
        if(!atsc3_route_s_tsid_RS->dest_ip_addr || !atsc3_route_s_tsid_RS->dest_port) {
            _ATSC3_LLS_ALC_UTILS_WARN( "lls_sls_alc_update_s_tsid_RS_dIpAddr_dPort_if_missing: patching S-TSID.RS: RS: dIpAddr: %u, dPort: %u, sIpAddr: %u with new dIpAddr: %u, dPort: %u",
                    atsc3_route_s_tsid_RS->dest_ip_addr,
                    atsc3_route_s_tsid_RS->dest_port,
                    atsc3_route_s_tsid_RS->src_ip_addr,
                    udp_flow->dst_ip_addr,
                    udp_flow->dst_port);
            if(!atsc3_route_s_tsid_RS->dest_ip_addr) {
                atsc3_route_s_tsid_RS->dest_ip_addr = udp_flow->dst_ip_addr;
            }

            if(!atsc3_route_s_tsid_RS->dest_port) {
                atsc3_route_s_tsid_RS->dest_port = udp_flow->dst_port;
            }
        }
    }
}

/*
 
 jjustman-2020-06-02 - TODO: add in support for more than one tsi per content type flow
 
 E.G:
	 

	 <?xml version="1.0" encoding="UTF-8"?>
	 <S-TSID @xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/S-TSID/1.0/" xmlns:afdt="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ATSC-FDT/1.0/" xmlns:fdt="urn:ietf:params:xml:ns:fdt">
		 <RS sIpAddr="172.16.200.1">
			 <LS tsi="100">
				 <SrcFlow rt="true">
					 <EFDT>
						 <FDT-Instance Expires="4294967295" afdt:efdtVersion="1" afdt:maxTransportSize="6200000" afdt:fileTemplate="video-$TOI$.mp4v">
							 <fdt:File TOI="1" Content-Location="video-init.mp4v"/>
						 </FDT-Instance>
					 </EFDT>
					 <ContentInfo>
						 <MediaInfo contentType="video" repId="Video1_1">
							 <ContentRating value="1,'TV-14 D-L',{0 'TV-14'}{1 'D'}{2 'L'}"/>
						 </MediaInfo>
					 </ContentInfo>
					 <Payload codePoint="8" formatId="1" frag="0" order="true"/>
				 </SrcFlow>
			 </LS>
			 <LS tsi="200">
				 <SrcFlow rt="true">
					 <EFDT>
						 <FDT-Instance Expires="4294967295" afdt:efdtVersion="1" afdt:maxTransportSize="96000" afdt:fileTemplate="a0-a02_2-$TOI$.m4s">
							 <fdt:File TOI="1" Content-Location="a0-a02_2-init.mp4"/>
						 </FDT-Instance>
					 </EFDT>
					 <ContentInfo>
						 <MediaInfo contentType="audio" repId="a2_2"/>
					   </ContentInfo>
					 <Payload codePoint="8" formatId="1" frag="0" order="true"/>
				 </SrcFlow>
			 </LS>
			 <LS tsi="201">
				 <SrcFlow rt="true">
					 <EFDT>
						 <FDT-Instance Expires="4294967295" afdt:efdtVersion="1" afdt:maxTransportSize="32000" afdt:fileTemplate="a1-a13_3-$TOI$.m4s">
							 <fdt:File TOI="1" Content-Location="a1-a13_3-init.mp4"/>
						 </FDT-Instance>
					 </EFDT>
					 <ContentInfo>
						 <MediaInfo contentType="audio" repId="a13_3"/>
					 </ContentInfo>
					 <Payload codePoint="8" formatId="1" frag="0" order="true"/>
				 </SrcFlow>
			 </LS>
			 <LS tsi="300">
				 <SrcFlow rt="true">
					 <EFDT>
						 <FDT-Instance Expires="4294967295" afdt:efdtVersion="1" afdt:maxTransportSize="140000" afdt:fileTemplate="d4_4-$TOI$.m4s">
							 <fdt:File TOI="1" Content-Location="d4_4-init.mp4"/>
						 </FDT-Instance>
					 </EFDT>
					 <ContentInfo>
						 <MediaInfo contentType="subtitles" repId="d3_3"/>
					 </ContentInfo>
					 <Payload codePoint="8" formatId="1" frag="0" order="true"/>
				 </SrcFlow>
			 </LS>
		 </RS>
	 </S-TSID>
	 
jjustman-2020-06-02: TODO: make sure we properly clear out lls_sls_alc_monitor on MBMS TSI=0, TOI change

A/331:2020 Section 7:

   The S-TSID fragment provides component acquisition information associated with one Service.
   It also provides mapping between DASH Representations found in the MPD and in the TSI corresponding
   to the component of the Service.

   The S-TSID can provide component acquisition information in the
   form of a TSI, and for a streaming Service, also the associated DASH Representation identifier
   carrying DASH Segments associated with the DASH Representation.

   By the TSI value, the receiver collects the audio/video components from the Service and
   begins buffering DASH Segments then applies the appropriate decoding processes.


Example:

	<ContentInfo>
    	<MediaInfo contentType="audio" repId="a2_2"/>
    </ContentInfo>

 
 jjustman-2020-07-27: TODO - keep track of our SLS TOI and s-tsid version here if we have to rebuild
 */

void lls_sls_alc_update_all_mediainfo_flow_v_from_route_s_tsid(lls_sls_alc_monitor_t* lls_sls_alc_monitor, atsc3_route_s_tsid_t* atsc3_route_s_tsid) {

	if(!lls_sls_alc_monitor || !atsc3_route_s_tsid) {
		_ATSC3_LLS_ALC_UTILS_ERROR("lls_sls_alc_session: %p, atsc3_route_s_tsid: %p returning!", lls_sls_alc_monitor, atsc3_route_s_tsid);
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
				atsc3_route_s_tsid_RS_LS_SrcFlow_Payload_t* atsc3_route_s_tsid_RS_LS_SrcFlow_Payload = atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload;

				atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance;
				atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info = NULL; //jjustman-2020-07-14 - keep track of this mediaInfo/repId and tsi/toi for mpd startNumber patching


                if(!atsc3_fdt_instance) {
                   _ATSC3_LLS_ALC_UTILS_WARN("    S-TSID.RS.LS.source_flow present but no fdt-instance element!");
                   return;
                }

				if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo && atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo) {
					media_info = atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo;

					atsc3_sls_alc_flow_t* atsc3_sls_alc_all_mediainfo_flow = atsc3_sls_alc_flow_add_entry_unique_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, atsc3_route_s_tsid_RS_LS->tsi, media_info);

					//TODO: jjustman-2020-07-14 - fixme to a qualfied type
					src_flow_content_info_content_type = atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->content_type;

					for(int k=0; k < atsc3_fdt_instance->atsc3_fdt_file_v.count; k++) {
						atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[k];
						if(src_flow_content_info_content_type && atsc3_fdt_file->toi && atsc3_route_s_tsid_RS_LS->tsi) {
							atsc3_sls_alc_all_mediainfo_flow->toi_init = atsc3_fdt_file->toi;

						}
					}

					_ATSC3_LLS_ALC_UTILS_DEBUG("   S-TSID.RS.LS.SrcFlow.ContentInfo: contentType: %s, repId: %s, startup: %u",
							atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->content_type,
							atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->content_type,
							atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo->atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo->startup);

				}

				//jjustman-2020-07-27 - todo - purge this
//				//continue on...
//				if(!media_info) {
//					_ATSC3_LLS_ALC_UTILS_INFO("    atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo missing for tsi: %d", atsc3_route_s_tsid_RS_LS->tsi);
//				}
//
//
//				_ATSC3_LLS_ALC_UTILS_DEBUG("     S-TSID.RS.LS.source_flow.fdt-instance: version: %u, expires: %u, content_type: %s, file_template: %s",
//						atsc3_fdt_instance->efdt_version,
//						atsc3_fdt_instance->expires,
//						atsc3_fdt_instance->content_type,
//						atsc3_fdt_instance->file_template);
//
//				for(int k=0; k < atsc3_fdt_instance->atsc3_fdt_file_v.count; k++) {
//					atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[k];
//
//
//					if(src_flow_content_info_content_type && atsc3_fdt_file->toi && atsc3_route_s_tsid_RS_LS->tsi) {
//
//						if(strncasecmp("audio", src_flow_content_info_content_type, 5) == 0 && !lls_sls_alc_monitor->audio_tsi_manual_override) {
//                            if(!lls_sls_alc_monitor->audio_tsi_manual_override)  {
//                                //update our audio tsi and toi accordingly
//                            	atsc3_sls_alc_flow_t* atsc3_sls_alc_flow = atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init(&lls_sls_alc_monitor->atsc3_sls_alc_audio_flow_v, atsc3_route_s_tsid_RS_LS->tsi, atsc3_fdt_file->toi, media_info);
//
//                            } else {
//                                _ATSC3_LLS_ALC_UTILS_DEBUG("lls_sls_alc_update_tsi_toi_from_route_s_tsid: not replacing audio tsi/toi_init, as manual override set: %d, %d",
//                                                           lls_sls_alc_monitor->audio_tsi_manual_override->tsi,
//                                                           lls_sls_alc_monitor->audio_tsi_manual_override->toi_init);
//                            }
//
//						} else if(strncasecmp("video", src_flow_content_info_content_type, 5) == 0) {
//                            if (!lls_sls_alc_monitor->video_tsi_manual_override) {
//                            	atsc3_sls_alc_flow_t* atsc3_sls_alc_flow = atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init(&lls_sls_alc_monitor->atsc3_sls_alc_video_flow_v, atsc3_route_s_tsid_RS_LS->tsi, atsc3_fdt_file->toi, media_info);
//
//                            } else {
//                                _ATSC3_LLS_ALC_UTILS_DEBUG(
//                                        "lls_sls_alc_update_tsi_toi_from_route_s_tsid: not replacing video tsi/toi_init, as manual override set: %d, %d",
//                                        lls_sls_alc_monitor->video_tsi_manual_override->tsi,
//                                        lls_sls_alc_monitor->video_tsi_manual_override->toi_init);
//                            }
//                        } else if(strncasecmp("text", src_flow_content_info_content_type, 4) == 0 || strncasecmp("subtitles", src_flow_content_info_content_type, 4) == 0) {
//                        	//jjustman-2020-07-14 - todo: fix me for "subtitles" content type strlen
//                            if (!lls_sls_alc_monitor->text_tsi_manual_override) {
//                                _ATSC3_LLS_ALC_UTILS_INFO("lls_sls_alc_update_tsi_toi_from_route_s_tsid: setting subtitle tsi/toi_init information to: %d, %d", atsc3_route_s_tsid_RS_LS->tsi, atsc3_fdt_file->toi);
//                                atsc3_sls_alc_flow_t* atsc3_sls_alc_flow = atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init(&lls_sls_alc_monitor->atsc3_sls_alc_subtitles_flow_v, atsc3_route_s_tsid_RS_LS->tsi, atsc3_fdt_file->toi, media_info);
//
//                            } else {
//                                _ATSC3_LLS_ALC_UTILS_DEBUG(
//                                        "lls_sls_alc_update_tsi_toi_from_route_s_tsid: not replacing text tsi/toi_init, as manual override set: %d, %d",
//                                        lls_sls_alc_monitor->text_tsi_manual_override->tsi,
//                                        lls_sls_alc_monitor->text_tsi_manual_override->toi_init);
//                            }
//
//                        } else {
//                            _ATSC3_LLS_ALC_UTILS_INFO("lls_sls_alc_update_tsi_toi_from_route_s_tsid: unknown src_flow_content_info_content_type: %s, using atsc3_fdt_file->content_type: %s, setting data tsi/toi information to: %d, %d", src_flow_content_info_content_type, atsc3_fdt_file->content_type, atsc3_fdt_file->toi, atsc3_route_s_tsid_RS_LS->tsi);
//                            atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_nrt = atsc3_sls_alc_flow_add_entry_unique_tsi_toi_nrt(&lls_sls_alc_monitor->atsc3_sls_alc_data_flow_v, atsc3_route_s_tsid_RS_LS->tsi, atsc3_fdt_file->toi);
//                        	atsc3_sls_alc_flow_nrt_set_fdt_file_content_type_if_null(atsc3_sls_alc_flow_nrt, atsc3_fdt_file->content_type);
//						}
//
//					} else {
//						if(!atsc3_route_s_tsid_RS_LS_SrcFlow_Payload || (atsc3_route_s_tsid_RS_LS_SrcFlow_Payload && !(atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->codepoint == 3 || atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->codepoint == 4))) {
//							_ATSC3_LLS_ALC_UTILS_WARN("missing contentInfo/MediaInfo for flow tsi/toi, src_flow_content_info_content_type: %s, tsi: %u, toi: %u for content-location: %s",
//									src_flow_content_info_content_type,
//									atsc3_fdt_file->toi,
//									atsc3_route_s_tsid_RS_LS->tsi,
//									atsc3_fdt_file->content_location);
//						} else {
//							_ATSC3_LLS_ALC_UTILS_DEBUG("skipping contentInfo/MediaInfo check for codePoint: %d, tsi: %u, toi: %u for content-location: %s",
//									atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->codepoint,
//									atsc3_fdt_file->toi,
//									atsc3_route_s_tsid_RS_LS->tsi,
//									atsc3_fdt_file->content_location);
//
//						}
//					}
//
//					_ATSC3_LLS_ALC_UTILS_DEBUG("     S-TSID.RS.LS.source_flow.fdt-instance.file: content-location: %s, toi: %u, content_length: %u, transfer_length: %u, content_type; %s, content_encoding: %s",
//							atsc3_fdt_file->content_location,
//							atsc3_fdt_file->toi,
//							atsc3_fdt_file->content_length,
//							atsc3_fdt_file->transfer_length,
//							atsc3_fdt_file->content_type,
//							atsc3_fdt_file->content_encoding);
//				}
			}
		}
	}

}

