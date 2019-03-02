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

int __LLS_SLT_PARSER_DEBUG_ENABLED=0;
int __LLS_SLT_PARSER_TRACE_ENABLED=0;


#define LLS_SLT_SIMULCAST_TSID 				"SimulcastTSID"
#define LLS_SLT_SVC_CAPABILITIES			"SvcCapabilities"
#define LLS_SLT_BROADCAST_SVC_SIGNALING 	"BroadcastSvcSignaling"
#define LLS_SLT_SVC_INET_URL				"SvcInetUrl"
#define LLS_SLT_OTHER_BSID					"OtherBsid"


//defer creating our sls_monitor instances until they are needed
//create our sls_monitor_ref
//lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor_create();
//lls_slt_monitor->lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();

lls_slt_monitor_t* lls_slt_monitor_create() {
	lls_slt_monitor_t* lls_slt_monitor = (lls_slt_monitor_t*)calloc(1, sizeof(lls_slt_monitor_t));

    //create our vector references
    lls_slt_monitor->lls_sls_mmt_session_vector = lls_sls_mmt_session_vector_create();
	lls_slt_monitor->lls_sls_alc_session_vector = lls_sls_alc_session_vector_create();

	return lls_slt_monitor;
}

int lls_slt_table_build(lls_table_t *lls_table, xml_node_t *xml_root) {
	/** bsid **/

	xml_string_t* root_node_name = xml_node_name(xml_root); //root
	dump_xml_string(root_node_name);

	uint8_t* slt_attributes = xml_attributes_clone(root_node_name);
	__LLS_SLT_PARSER_DEBUG("build_SLT_table, attributes are: %s", (const char*)slt_attributes);

	kvp_collection_t* slt_attributes_collecton = kvp_collection_parse(slt_attributes);
	char* bsid_char = kvp_collection_get(slt_attributes_collecton, "bsid");
	//if there is a space, split and callocif(strnstr(bsid, "", ))

	//TODO: fix me
	if(bsid_char) {
		int bsid_i;
		bsid_i = atoi(bsid_char);
		freesafe(bsid_char);

		lls_table->slt_table.bsid_n = 1;
		lls_table->slt_table.bsid =  (int*)calloc(lls_table->slt_table.bsid_n , sizeof(int));
		lls_table->slt_table.bsid[0] = bsid_i;
	}

	__LLS_SLT_PARSER_TRACE("build_SLT_table, attributes are: %s\n", slt_attributes);

	int svc_size = xml_node_children(xml_root);

	//build our service rows
	for(int i=0; i < svc_size; i++) {
		xml_node_t* service_row_node = xml_node_child(xml_root, i);
		xml_string_t* service_row_node_xml_string = xml_node_name(service_row_node);

		/** push service row **/
		lls_table->slt_table.service_entry_n++;
		//TODO - grow this dynamically to N?
		if(!lls_table->slt_table.service_entry) {
			lls_table->slt_table.service_entry = (lls_service_t**)calloc(32, sizeof(lls_service_t**));
		}

		//service_row_node_xml_string
		uint8_t* child_row_node_attributes_s = xml_attributes_clone(service_row_node_xml_string);
		kvp_collection_t* service_attributes_collecton = kvp_collection_parse(child_row_node_attributes_s);

		lls_table->slt_table.service_entry[lls_table->slt_table.service_entry_n-1] = (lls_service_t*)calloc(1, sizeof(lls_service_t));
		lls_service_t* service_entry = lls_table->slt_table.service_entry[lls_table->slt_table.service_entry_n-1];
		//map in other attributes, e.g


		int scratch_i = 0;
		char* serviceId = kvp_collection_get(service_attributes_collecton, "serviceId");

		if(!serviceId) {
			__LLS_SLT_PARSER_ERROR("missing required element - serviceId!");
			return -1;
		}

		scratch_i = atoi(serviceId);
		freesafe(serviceId);
		service_entry->service_id = scratch_i & 0xFFFF;
		__LLS_SLT_PARSER_TRACE("service id is: %s, int is: %d, uint_16: %u", serviceId, scratch_i, (scratch_i & 0xFFFF));

		//copy our char* elements
		service_entry->global_service_id  = kvp_collection_get(service_attributes_collecton, "globalServiceID");
		service_entry->short_service_name = kvp_collection_get(service_attributes_collecton, "shortServiceName");

		char* majorChannelNo  = kvp_collection_get(service_attributes_collecton, "majorChannelNo");
		char* minorChannelNo  = kvp_collection_get(service_attributes_collecton, "minorChannelNo");
		char* serviceCategory = kvp_collection_get(service_attributes_collecton, "serviceCategory");
		char* sltSvcSeqNum    = kvp_collection_get(service_attributes_collecton, "sltSvcSeqNum");

		//optional parameters here
		if(majorChannelNo) {
			scratch_i = atoi(majorChannelNo);
			service_entry->major_channel_no = scratch_i & 0xFFFF;
			freesafe(majorChannelNo);
		}

		if(minorChannelNo) {
			scratch_i = atoi(minorChannelNo);
			service_entry->minor_channel_no = scratch_i & 0xFFFF;
			freesafe(minorChannelNo);
		}

		if(serviceCategory) {
			scratch_i = atoi(serviceCategory);
			service_entry->service_category = scratch_i & 0xFFFF;
			freesafe(serviceCategory);
		}

		if(sltSvcSeqNum) {
			scratch_i = atoi(sltSvcSeqNum);
			service_entry->slt_svc_seq_num = scratch_i & 0xFFFF;
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
				SLT_BROADCAST_SVC_SIGNALING_build_table(service_entry, service_row_node, kvp_child_attributes);

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
	}

	if(slt_attributes) {
		free(slt_attributes);
	}
	if(slt_attributes_collecton) {
		kvp_collection_free(slt_attributes_collecton);
	}

	return 0;
}

int SLT_BROADCAST_SVC_SIGNALING_build_table(lls_service_t* service_table, xml_node_t *service_row_node, kvp_collection_t* kvp_collection) {
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
	service_table->broadcast_svc_signaling.sls_protocol = atoi(slsProtocol);
	freesafe(slsProtocol);

	service_table->broadcast_svc_signaling.sls_destination_ip_address = kvp_collection_get(kvp_collection, "slsDestinationIpAddress");
	service_table->broadcast_svc_signaling.sls_destination_udp_port = kvp_collection_get(kvp_collection, "slsDestinationUdpPort");
	service_table->broadcast_svc_signaling.sls_source_ip_address = kvp_collection_get(kvp_collection, "slsSourceIpAddress");


	//kvp_find_key(kvp_collection, "slsProtocol";

cleanup:
	//cleanup
	if(svc_attributes) {
		free(svc_attributes);
	}

	return ret;
}

int lls_slt_table_check_process_update(lls_table_t* lls_table, lls_slt_monitor_t* lls_slt_monitor) {
	int retval = 0;

	//if we have a lls_slt table, and the group is the same but its a new version, reprocess
	if(!lls_slt_monitor->lls_table_slt ||
		(lls_slt_monitor->lls_table_slt && lls_slt_monitor->lls_table_slt->lls_group_id == lls_table->lls_group_id &&
				lls_slt_monitor->lls_table_slt->lls_table_version != lls_table->lls_table_version)) {

		int update_retval = 0;
		__LLS_SLT_PARSER_DEBUG("Beginning processing of SLT from lls_table_slt_update");

		update_retval = lls_slt_table_process_update(lls_table, lls_slt_monitor );

		if(!update_retval) {
			__LLS_SLT_PARSER_DEBUG("lls_table_slt_update -- complete");
		} else {
			__LLS_SLT_PARSER_ERROR("unable to parse LLS table");
			lls_table_free(lls_table);

			retval = -1;
		}
	}

	return retval;
}




int lls_slt_table_process_update(lls_table_t* lls_table, lls_slt_monitor_t* lls_slt_monitor) {

	if(lls_slt_monitor->lls_table_slt) {
		lls_table_free(lls_slt_monitor->lls_table_slt);
		lls_slt_monitor->lls_table_slt = NULL;
	}
	lls_slt_monitor->lls_table_slt = lls_table;

	for(int i=0; i < lls_table->slt_table.service_entry_n; i++) {
		lls_service_t* lls_service = lls_table->slt_table.service_entry[i];
		__LLS_SLT_PARSER_DEBUG("checking service: %d", lls_service->service_id);

		if(lls_service->broadcast_svc_signaling.sls_protocol == SLS_PROTOCOL_ROUTE) {
            __LLS_SLT_PARSER_INFO("ROUTE: adding service: %u", lls_service->service_id);

			lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_or_create(lls_slt_monitor->lls_sls_alc_session_vector, lls_service);

			//TODO - we probably need to clear out any missing ALC sessions?
			if(!lls_sls_alc_session->alc_session) {
				lls_slt_alc_session_remove(lls_slt_monitor->lls_sls_alc_session_vector, lls_service);
                __LLS_SLT_PARSER_ERROR("ROUTE: Unable to instantiate alc session for service_id: %d via SLS_PROTOCOL_ROUTE", lls_service->service_id);
				goto cleanup;
		  	}
		}
        
        if(lls_service->broadcast_svc_signaling.sls_protocol == SLS_PROTOCOL_MMTP) {
            __LLS_SLT_PARSER_INFO("MMT: adding service: %u", lls_service->service_id);
            
            lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_or_create(lls_slt_monitor->lls_sls_mmt_session_vector, lls_service);
            
            //TODO - clear out any dropped mmt sessions?
            if(!lls_sls_mmt_session) {
                lls_slt_alc_session_remove(lls_slt_monitor->lls_sls_mmt_session_vector, lls_service);
                __LLS_SLT_PARSER_ERROR("MMT: Unable to instantiate session for service_id: %d via SLS_PROTOCOL_MMTP", lls_service->service_id);
                goto cleanup;
            }
        }
	}

	return 0;

cleanup:
	return -1;
}



