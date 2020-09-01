/*
 * atsc3_lls_slt_parser.c
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 *
 *      6.3 Service List Table (SLT)
The Service List Table (SLT) is one of the instance types of LLS information.
The function of the SLT is similar to that of the Program Association Table (PAT)
in MPEG-2 Systems [33], and the Fast Information Channel (FIC) found in ATSC A/153, Part 3 [44].

For a receiver first encountering the broadcast emission, this is the place to start.
It supports a rapid channel scan which allows a receiver to build a list of all the services
it can receive, with their channel name, channel number, etc., and it provides bootstrap information
that allows a receiver to discover the SLS for each service.

For ROUTE/DASH-delivered services,
the bootstrap information includes the source IP address, the destination IP address and the
destination port of the LCT channel that carries the ROUTE-specific SLS.

For MMTP/MPU-delivered services, the bootstrap information includes the destination IP
address and destination port of the MMTP session carrying the MMTP- specific SLS.
 *
 */

#include "atsc3_lls_slt_parser.h"
#include "atsc3_lls_sls_parser.h"

int _LLS_SLT_PARSER_INFO_ENABLED = 1;
int _LLS_SLT_PARSER_INFO_MMT_ENABLED = 0;
int _LLS_SLT_PARSER_INFO_ROUTE_ENABLED = 0;

int _LLS_SLT_PARSER_DEBUG_ENABLED = 0;
int _LLS_SLT_PARSER_TRACE_ENABLED = 0;

#define LLS_SLT_SIMULCAST_TSID 				"SimulcastTSID"
#define LLS_SLT_SVC_CAPABILITIES			"SvcCapabilities"
#define LLS_SLT_BROADCAST_SVC_SIGNALING 	"BroadcastSvcSignaling"
#define LLS_SLT_SVC_INET_URL				"SvcInetUrl"
#define LLS_SLT_OTHER_BSID					"OtherBsid"


lls_slt_monitor_t* lls_slt_monitor_create() {
	lls_slt_monitor_t* lls_slt_monitor = lls_slt_monitor_new();
	return lls_slt_monitor;
}

//jjustman-2020-08-31 - TODO: clean up all interior objects
void atsc3_lls_slt_monitor_free(lls_slt_monitor_t** lls_slt_monitor_p) {
    if(lls_slt_monitor_p) {
        lls_slt_monitor_t* lls_slt_monitor = *lls_slt_monitor_p;
        if(lls_slt_monitor) {
            lls_slt_monitor_free_lls_slt_service_id(lls_slt_monitor);
            lls_slt_monitor_free_lls_sls_mmt_monitor(lls_slt_monitor);
            lls_slt_monitor_free_lls_sls_mmt_session_flows(lls_slt_monitor);
            lls_slt_monitor_free_lls_sls_alc_monitor(lls_slt_monitor);
            lls_slt_monitor_free_lls_sls_alc_session_flows(lls_slt_monitor);
            lls_slt_monitor_free_lls_slt_service_id_group_id_cache(lls_slt_monitor);
            lls_table_free(&lls_slt_monitor->lls_latest_slt_table);
            lls_table_free(&lls_slt_monitor->lls_latest_aeat_table);
            lls_table_free(&lls_slt_monitor->lls_latest_on_screen_message_notification_table);

            free(lls_slt_monitor);
            lls_slt_monitor = NULL;
        }
        *lls_slt_monitor_p = NULL;
    }
}

int lls_slt_table_build(lls_table_t *lls_table, xml_node_t *xml_root) {
	/** bsid **/

	xml_string_t* root_node_name = xml_node_name(xml_root); //root
	dump_xml_string(root_node_name);

	uint8_t* slt_attributes = xml_attributes_clone(root_node_name);
	__LLS_SLT_PARSER_DEBUG("build_SLT_table, attributes are: %s", (const char*)slt_attributes);

	kvp_collection_t* slt_attributes_collecton = kvp_collection_parse(slt_attributes);
	char* bsid_char = kvp_collection_get(slt_attributes_collecton, "bsid");
	//if there is a space, split and callo cif(strnstr(bsid, "", ))

	//TODO: fix me
	if(bsid_char) {
		int bsid_i;
		bsid_i = atoi(bsid_char);
		freesafe(bsid_char);

		lls_table->slt_table.bsid_n = 1;
		lls_table->slt_table.bsid =  (int*)calloc(lls_table->slt_table.bsid_n , sizeof(int));
		(*lls_table->slt_table.bsid) = bsid_i;
	}


	int svc_size = xml_node_children(xml_root);
	__LLS_SLT_PARSER_TRACE("build_SLT_table, lls_table->slt_table.bsid_n: %u, svc_size: %u, attributes are: %s\n", lls_table->slt_table.bsid_n, svc_size, slt_attributes);

	//lls_table->slt_table.service_entry = (lls_service_t**)calloc(svc_size, sizeof(lls_service_t*));

	atsc3_lls_slt_table_prealloc_atsc3_lls_slt_service(&lls_table->slt_table, svc_size);

	//build our service rows
	for(int i=0; i < svc_size; i++) {
		atsc3_lls_slt_service_t* atsc3_lls_slt_service = atsc3_lls_slt_service_new();

		xml_node_t* service_row_node = xml_node_child(xml_root, i);
		xml_string_t* service_row_node_xml_string = xml_node_name(service_row_node);

		if(xml_string_equals_ignore_case(service_row_node_xml_string, "Service")) {
			dump_xml_string(service_row_node_xml_string);

			//service_row_node_xml_string
			uint8_t* child_row_node_attributes_s = xml_attributes_clone(service_row_node_xml_string);

			kvp_collection_t* service_attributes_collecton = kvp_collection_parse(child_row_node_attributes_s);


			int scratch_i = 0;
			char* serviceId = kvp_collection_get(service_attributes_collecton, "serviceId");

			if(!serviceId) {
				//2019-03-26 - dump lls table for diagnostics
				__LLS_SLT_PARSER_ERROR("missing required element - serviceId! raw xml payload is: \n%s", lls_table->raw_xml.xml_payload);
				return -1;
			}

			scratch_i = atoi(serviceId);
			freesafe(serviceId);
			atsc3_lls_slt_service->service_id = scratch_i & 0xFFFF;
			__LLS_SLT_PARSER_TRACE("service id is: %s, int is: %d, uint_16: %u", serviceId, scratch_i, (scratch_i & 0xFFFF));

			//copy our char* elements
			atsc3_lls_slt_service->global_service_id  = kvp_collection_get(service_attributes_collecton, "globalServiceID");
			atsc3_lls_slt_service->short_service_name = kvp_collection_get(service_attributes_collecton, "shortServiceName");

			char* majorChannelNo  = kvp_collection_get(service_attributes_collecton, "majorChannelNo");
			char* minorChannelNo  = kvp_collection_get(service_attributes_collecton, "minorChannelNo");
			char* serviceCategory = kvp_collection_get(service_attributes_collecton, "serviceCategory");
			char* sltSvcSeqNum    = kvp_collection_get(service_attributes_collecton, "sltSvcSeqNum");

			//optional parameters here
			if(majorChannelNo) {
				scratch_i = atoi(majorChannelNo);
				atsc3_lls_slt_service->major_channel_no = scratch_i & 0xFFFF;
				freesafe(majorChannelNo);
			}

			if(minorChannelNo) {
				scratch_i = atoi(minorChannelNo);
				atsc3_lls_slt_service->minor_channel_no = scratch_i & 0xFFFF;
				freesafe(minorChannelNo);
			}

			if(serviceCategory) {
				scratch_i = atoi(serviceCategory);
				atsc3_lls_slt_service->service_category = scratch_i & 0xFFFF;
				freesafe(serviceCategory);
			}

			if(sltSvcSeqNum) {
				scratch_i = atoi(sltSvcSeqNum);
				atsc3_lls_slt_service->slt_svc_seq_num = scratch_i & 0xFFFF;
				freesafe(sltSvcSeqNum);
			}

			int svc_child_size = xml_node_children(service_row_node);

			dump_xml_string(service_row_node_xml_string);

			for(int j=0; j < svc_child_size; j++) {

				xml_node_t* child_row_node = xml_node_child(service_row_node, j);
				xml_string_t* child_row_node_xml_string = xml_node_name(child_row_node);

				//this is a malloc
				uint8_t* child_row_node_attributes_s = xml_attributes_clone(child_row_node_xml_string);
				kvp_collection_t* kvp_child_attributes = kvp_collection_parse(child_row_node_attributes_s);

				dump_xml_string(child_row_node_xml_string);

				if(xml_string_equals_ignore_case(child_row_node_xml_string, LLS_SLT_SIMULCAST_TSID)) {
					_LLS_ERROR("build_SLT_table - not supported: LLS_SLT_SIMULCAST_TSID");
				} else if(xml_string_equals_ignore_case(child_row_node_xml_string, LLS_SLT_SVC_CAPABILITIES)) {
					_LLS_ERROR("build_SLT_table - not supported: LLS_SLT_SVC_CAPABILITIES");
				} else if(xml_string_equals_ignore_case(child_row_node_xml_string, LLS_SLT_BROADCAST_SVC_SIGNALING)) {
					SLT_BROADCAST_SVC_SIGNALING_build_table(atsc3_lls_slt_service, service_row_node, kvp_child_attributes);

				} else if(xml_string_equals_ignore_case(child_row_node_xml_string, LLS_SLT_SVC_INET_URL)) {
					_LLS_ERROR("build_SLT_table - not supported: LLS_SLT_SVC_INET_URL");
				} else if(xml_string_equals_ignore_case(child_row_node_xml_string, LLS_SLT_OTHER_BSID)) {
					_LLS_ERROR("build_SLT_table - not supported: LLS_SLT_OTHER_BSID");
				} else {
					_LLS_ERROR("build_SLT_table - unknown type: %s\n", xml_string_clone(child_row_node_xml_string));
				}

				//cleanup
				free(child_row_node_attributes_s);
				kvp_collection_free(kvp_child_attributes);
			}

			//cleanup
			if(service_attributes_collecton) {
				kvp_collection_free(service_attributes_collecton);
			}
			if(child_row_node_attributes_s) {
				free(child_row_node_attributes_s);
			}

			//push entry
			atsc3_lls_slt_table_add_atsc3_lls_slt_service(&lls_table->slt_table, atsc3_lls_slt_service);
		}
	}

	if(slt_attributes) {
		free(slt_attributes);
	}
	if(slt_attributes_collecton) {
		kvp_collection_free(slt_attributes_collecton);
	}

	return 0;
}

int SLT_BROADCAST_SVC_SIGNALING_build_table(atsc3_lls_slt_service_t* atsc3_lls_slt_service, xml_node_t *service_row_node, kvp_collection_t* kvp_collection) {
	int ret = 0;
	xml_string_t* service_row_node_xml_string = xml_node_name(service_row_node);
	uint8_t *svc_attributes = xml_attributes_clone(service_row_node_xml_string);
	_LLS_TRACE("build_SLT_BROADCAST_SVC_SIGNALING_table - attributes are: %s", svc_attributes);

	char* slsProtocol = kvp_collection_get(kvp_collection, "slsProtocol");
	if(!slsProtocol) {
		_LLS_ERROR("build_SLT_BROADCAST_SVC_SIGNALING_table: missing slsProtocol value");
		ret = -1;
		return -1;
	}

	int scratch_i=0;

	atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = atsc3_slt_broadcast_svc_signalling_new();
	atsc3_slt_broadcast_svc_signalling->sls_protocol = atoi(slsProtocol);
	freesafe(slsProtocol);

	atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address = kvp_collection_get(kvp_collection, "slsDestinationIpAddress");
	atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port = kvp_collection_get(kvp_collection, "slsDestinationUdpPort");
	atsc3_slt_broadcast_svc_signalling->sls_source_ip_address = kvp_collection_get(kvp_collection, "slsSourceIpAddress");

	//jjustman-2019-10-03, if we have at least dest_ip and dest_port, then add
	if(atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address && atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port) {
		atsc3_lls_slt_service_add_atsc3_slt_broadcast_svc_signalling(atsc3_lls_slt_service, atsc3_slt_broadcast_svc_signalling);
	}

	//kvp_find_key(kvp_collection, "slsProtocol";

cleanup:
	//cleanup
	if(svc_attributes) {
		free(svc_attributes);
	}

	return ret;
}

int lls_slt_table_perform_update(lls_table_t* lls_table, lls_slt_monitor_t* lls_slt_monitor) {

    for(int i=0; i < lls_table->slt_table.atsc3_lls_slt_service_v.count; i++) {

		atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_table->slt_table.atsc3_lls_slt_service_v.data[i];

		lls_slt_monitor_add_or_update_lls_slt_service_id_group_id_cache_entry(lls_slt_monitor, lls_table->lls_group_id, atsc3_lls_slt_service);

		__LLS_SLT_PARSER_DEBUG("checking service: %d", atsc3_lls_slt_service->service_id);

		if(atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count) {

			if(atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol == SLS_PROTOCOL_ROUTE) {
				__LLS_SLT_PARSER_INFO_ROUTE("ROUTE: adding service: %u, flow: %s:%s",
						atsc3_lls_slt_service->service_id,
						atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_destination_ip_address,
						atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_destination_udp_port);

				//->atsc3_slt_broadcast_svc_signalling_v.data[0]
				lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_or_create(lls_slt_monitor, atsc3_lls_slt_service);

				//TODO - we probably need to clear out any missing ALC sessions?
				if(lls_sls_alc_session && !lls_sls_alc_session->alc_session) {
					lls_slt_alc_session_remove(lls_slt_monitor, atsc3_lls_slt_service);
					__LLS_SLT_PARSER_ERROR("ROUTE: Unable to instantiate alc session for service_id: %d via SLS_PROTOCOL_ROUTE", atsc3_lls_slt_service->service_id);
					goto cleanup;
				}
			} else if(atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol == SLS_PROTOCOL_MMTP) {
				__LLS_SLT_PARSER_INFO_MMT("MMT: adding service: %u, flow: %s:%s", atsc3_lls_slt_service->service_id,
						atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_destination_ip_address,
						atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_destination_udp_port);

				lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_or_create(lls_slt_monitor, atsc3_lls_slt_service);

				//TODO - clear out any dropped mmt sessions?
				if(!lls_sls_mmt_session) {
					lls_slt_mmt_session_remove(lls_slt_monitor, atsc3_lls_slt_service);
					__LLS_SLT_PARSER_ERROR("MMT: Unable to instantiate session for service_id: %d via SLS_PROTOCOL_MMTP", atsc3_lls_slt_service->service_id);
					goto cleanup;
				}
			} else {
				__LLS_SLT_PARSER_ERROR("SLT: atsc3_lls_slt_service id: %u, unable to process not implemented for sls_protocol: %d", atsc3_lls_slt_service->service_id, atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol);
			}
		} else {
			__LLS_SLT_PARSER_ERROR("SLT: atsc3_lls_slt_service id: %u, does not contain broadcast_svc_signalling", atsc3_lls_slt_service->service_id);
		}
	}

	return 0;

cleanup:
	return -1;
}



