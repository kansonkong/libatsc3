/*
 * atsc3_alc_listener_mde_writer_from_stltp_pcap.cpp
 *
 *  Created on: Aug 31rd, 2020
 *      Author: jjustman
 *
 *
*/

//#define _ENABLE_TRACE 1
#define _SHOW_PACKET_FLOW 1
int PACKET_COUNTER=0;

#ifdef __MALLOC_DEBUGGING
#include <mcheck.h>
#endif


#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/stat.h>

#include <atsc3_utils.h>
#include <atsc3_lls.h>
#include <atsc3_lls_alc_utils.h>
#include <atsc3_alc_rx.h>
#include <atsc3_alc_utils.h>
#include <atsc3_listener_udp.h>
#include <atsc3_logging_externs.h>

#include <atsc3_udp.h>
#include <phy/virtual/PcapSTLTPVirtualPHY.h>


#define _PCAP_STLTP_VIRTUAL_TEST_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _PCAP_STLTP_VIRTUAL_TEST_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _PCAP_STLTP_VIRTUAL_TEST_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _PCAP_STLTP_VIRTUAL_TEST_DEBUG(...)   __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);

uint64_t rx_udp_invocation_count = 0;

lls_slt_monitor_t* lls_slt_monitor;
lls_sls_alc_monitor_t* lls_sls_alc_monitor;

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_service_id_filter = NULL;

atsc3_alc_arguments_t* alc_arguments;
atsc3_alc_session_t* atsc3_alc_session;

uint32_t alc_packet_received_count = 0;

void process_packet(atsc3_udp_packet_t* udp_packet) {

  if(!udp_packet) {
	return;
  }

  atsc3_alc_packet_t* alc_packet = NULL;
    
    
    //dispatch for LLS extraction and dump
    if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
        lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data);

        //auto-assign our first ROUTE service id here
        if(lls_table && lls_table->lls_table_id == SLT) {
            for(int i=0; i < lls_table->slt_table.atsc3_lls_slt_service_v.count; i++) {
                atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_table->slt_table.atsc3_lls_slt_service_v.data[i];
                if(atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count &&
                   atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol == SLS_PROTOCOL_ROUTE) {
					lls_sls_alc_monitor_t* lls_sls_alc_monitor_local = lls_sls_alc_monitor_create();
								
					lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);
					lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

					lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
					if(!lls_sls_alc_session) {
						__WARN("lls_slt_alc_session_find_from_service_id: lls_sls_alc_session is NULL!");
					}
					lls_sls_alc_monitor_local->lls_alc_session = lls_sls_alc_session;
					lls_sls_alc_monitor_local->atsc3_lls_slt_service = atsc3_lls_slt_service;
					lls_sls_alc_monitor_local->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;


					__WARN("process_packet: adding lls_sls_alc_monitor: %p to lls_slt_monitor: %p, service_id: %d",
						   lls_sls_alc_monitor_local, lls_slt_monitor, lls_sls_alc_session->service_id);

					lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor_local);

					if(!lls_sls_alc_monitor) {
						lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor_local;
						lls_sls_alc_monitor =  lls_sls_alc_monitor_local;

					} else {
						//only swap out this lls_sls_alc_monitor if this alc flow is "retired"
					}
                }
            }
        }
        

        return udp_packet_free(&udp_packet);
    }
    
    /*
	 jjustman-2020-03-25 - alternatively, filter out by ServiceID:
	lls_sls_alc_monitor->atsc3_lls_slt_service &&
	lls_sls_alc_monitor->atsc3_lls_slt_service->service_id == matching_lls_slt_alc_session->atsc3_lls_slt_service->service_id
	 
	clang optimized out matching_lls_slt_alc_session in the first conditional, so its added in the 3rd filter test for service_id
    */
	
    lls_sls_alc_monitor_t* matching_lls_sls_alc_monitor = atsc3_lls_sls_alc_monitor_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
	//only used for service id filtering.
    lls_sls_alc_session_t* matching_lls_slt_alc_session = lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);

    if(matching_lls_sls_alc_monitor) {

		if(matching_lls_slt_alc_session && (
		   (dst_service_id_filter == NULL && dst_ip_addr_filter == NULL && dst_ip_port_filter == NULL) ||
		   ((dst_service_id_filter != NULL && matching_lls_slt_alc_session && matching_lls_slt_alc_session->service_id == *dst_service_id_filter ) ||
			(dst_ip_addr_filter != NULL && dst_ip_port_filter == NULL && udp_packet->udp_flow.dst_ip_addr == *dst_ip_addr_filter) ||
			(dst_ip_addr_filter != NULL && dst_ip_port_filter != NULL && udp_packet->udp_flow.dst_ip_addr == *dst_ip_addr_filter && udp_packet->udp_flow.dst_port == *dst_ip_port_filter)))) {

			//process ALC streams
			int retval = alc_rx_analyze_packet_a331_compliant((char*)block_Get(udp_packet->data), block_Remaining_size(udp_packet->data), &alc_packet);

	#ifdef __MALLOC_DEBUGGING
			mcheck(0);
	#endif

			if(!retval) {
				//check our alc_packet for a wrap-around TOI value, if it is a monitored TSI, and re-patch the MBMS MPD for updated availabilityStartTime and startNumber with last closed TOI values
				atsc3_alc_packet_check_monitor_flow_for_toi_wraparound_discontinuity(alc_packet, matching_lls_sls_alc_monitor);
	#ifdef __MALLOC_DEBUGGING
	mcheck(0);
	#endif

				//keep track of our EXT_FTI and update last_toi as needed for TOI length and manual set of the close_object flag
				atsc3_route_object_t* atsc3_route_object = atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows(alc_packet, matching_lls_sls_alc_monitor);
	#ifdef __MALLOC_DEBUGGING
	mcheck(0);
	#endif

				//persist to disk, process sls mbms and/or emit ROUTE media_delivery_event complete to the application tier if
				//the full packet has been recovered (e.g. no missing data units in the forward transmission)
				if(atsc3_route_object) {
					atsc3_alc_packet_persist_to_toi_resource_process_sls_mbms_and_emit_callback(&udp_packet->udp_flow, alc_packet, matching_lls_sls_alc_monitor, atsc3_route_object);
					alc_packet_received_count++;

	//				if(alc_packet_received_count > 10000) {
	//					exit(0);
	//				}

				} else {
					__ERROR("Error in ALC persist, atsc3_route_object is NULL!");

				}
		#ifdef __MALLOC_DEBUGGING
		mcheck(0);
		#endif

			} else {
				__ERROR("Error in ALC decode: %d", retval);
			}
		} else {
			__INFO("Discarding packet: lls_sls_alc_monitor: %p, matching_lls_sls_alc_monitor: %p, matching_lls_slt_alc_session: %p, ", lls_sls_alc_monitor, matching_lls_sls_alc_monitor, matching_lls_slt_alc_session);
		}
	} else {
		__INFO("Discarding packet: lls_sls_alc_monitor: %p, matching_lls_sls_alc_monitor: %p, matching_lls_slt_alc_session: %p", lls_sls_alc_monitor, matching_lls_sls_alc_monitor, matching_lls_slt_alc_session);
	}

udp_packet_free:
	alc_packet_free(&alc_packet);
	alc_packet = NULL;
#ifdef __MALLOC_DEBUGGING

	mcheck(0);
#endif
	
    return udp_packet_free(&udp_packet);
}


void phy_rx_udp_packet_process_callback(uint8_t plp_num, block_t* packet) {

	if((rx_udp_invocation_count++ % 1000) == 0) {
		_PCAP_STLTP_VIRTUAL_TEST_DEBUG("PLP: %d, packet number: %llu, packet: %p, len: %d",
				plp_num, rx_udp_invocation_count, packet, packet->p_size);
	}

	atsc3_udp_packet_t* atsc3_udp_packet = atsc3_udp_packet_from_block_t(packet);
	if(atsc3_udp_packet) {
		process_packet(atsc3_udp_packet);
	}
}



int main(int argc,char **argv) {

#ifdef __MALLOC_DEBUGGING
          mcheck(0);
	  mtrace ();
#endif
	_LLS_SLT_PARSER_INFO_ROUTE_ENABLED = 1;
	_LLS_ALC_UTILS_INFO_ENABLED = 1;

    _ALC_UTILS_DEBUG_ENABLED = 0;
	_ALC_RX_DEBUG_ENABLED = 0;
    _ALC_UTILS_DEBUG_ENABLED = 0;

    _ROUTE_OBJECT_INFO_ENABLED = 1;
    _ROUTE_OBJECT_DEBUG_ENABLED = 0;
    _ROUTE_OBJECT_TRACE_ENABLED = 0;

    _SLS_ALC_FLOW_INFO_ENABLED = 1;
    _SLS_ALC_FLOW_DEBUG_ENABLED = 0;
    _SLS_ALC_FLOW_TRACE_ENABLED = 0;

    _ROUTE_SLS_PROCESSOR_DEBUG_ENABLED = 0;

	
#ifdef __LOTS_OF_DEBUGGING__
	_LLS_INFO_ENABLED = 1;
	_LLS_DEBUG_ENABLED = 1;

	_LLS_SLT_PARSER_INFO_ROUTE_ENABLED = 1;
    _ALC_UTILS_DEBUG_ENABLED = 1;
    _ALC_UTILS_TRACE_ENABLED = 1;

	_ALC_UTILS_IOTRACE_ENABLED=1;

	_ALC_RX_DEBUG_ENABLED = 1;
	_ALC_RX_TRACE_ENABLED = 1;
    _ROUTE_OBJECT_DEBUG_ENABLED = 1;
    _ROUTE_SLS_PROCESSOR_DEBUG_ENABLED = 1;
    _SLS_ALC_FLOW_DEBUG_ENABLED = 1;


#endif


	char *filename;

    char *stltp_dst_ip = "239.239.239.239";
    char *stltp_dst_port = "30000";

    char *dst_ip = NULL;
    char *dst_port = NULL;
	char *dst_service_id = NULL;
	
	uint16_t dst_service_id_int;
	uint32_t dst_ip_int = 4025479151; //239.239.239.239 default
	uint16_t dst_port_int = 30000;

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* descr;
    struct bpf_program fp;
    bpf_u_int32 maskp;
    bpf_u_int32 netp;

	//wire up our required lls and sls structs here, if needed for "ad-hoc" IP based ROUTE flow selection without LLS/SLS
	
    mkdir("route", 0777);

    lls_slt_monitor = lls_slt_monitor_create();
	alc_arguments = (atsc3_alc_arguments_t*)calloc(1, sizeof(atsc3_alc_arguments_t));
    
    atsc3_alc_session = atsc3_open_alc_session(alc_arguments);
	

    //listen to all flows
    if(argc==2) {

    	filename = argv[1];
        __INFO("opening STLTP pcap for replay: %s", filename);

    } else if(argc==4) {

    	//listen to a selected flow

    	filename = argv[1];
		stltp_dst_ip = argv[2];
		stltp_dst_port = argv[3];
		
		__INFO("opening STLTP pcap for replay: %s, stltp dst_ip: %s, stltp dst_port: %s", filename, stltp_dst_ip, stltp_dst_port);
    } else if (argc==6) {
		stltp_dst_ip = argv[2];
		stltp_dst_port = argv[3];

		dst_ip = argv[4];
		dst_port = argv[5];

		dst_ip_int = parseIpAddressIntoIntval(dst_ip);
		dst_ip_addr_filter = &dst_ip_int;

		dst_port_int = parsePortIntoIntval(dst_port);
		dst_ip_port_filter = &dst_port_int;

		__INFO("opening STLTP pcap for replay: %s, stltp dst_ip: %s, stltp dst_port: %s, dst_ip: %s, dst_port: %s", filename, stltp_dst_ip, stltp_dst_port, dst_ip, dst_port);

		/* jjustman-2020-03-28 - create a dummy lls_sls_alc_monitor_t for ad-hoc SLS management (e.g. LLS is not present in this pcap flow - may be on a non-listening PLP */

		atsc3_lls_slt_service_t* atsc3_lls_slt_service = atsc3_lls_slt_service_new();
		atsc3_lls_slt_service->service_id=31337; //hack

		atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = atsc3_slt_broadcast_svc_signalling_new();
		atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address = dst_ip;
		atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port =  dst_port;
		atsc3_slt_broadcast_svc_signalling->sls_protocol = SLS_PROTOCOL_ROUTE;
		atsc3_lls_slt_service_add_atsc3_slt_broadcast_svc_signalling(atsc3_lls_slt_service, atsc3_slt_broadcast_svc_signalling);

		lls_slt_alc_session_find_or_create(lls_slt_monitor, atsc3_lls_slt_service);

		lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);
		lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

		lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
		if(!lls_sls_alc_session) {
			__WARN("lls_slt_alc_session_find_from_service_id: lls_sls_alc_session is NULL!");
		}

		lls_sls_alc_monitor_t* lls_sls_alc_monitor_local = lls_sls_alc_monitor_create();
		lls_sls_alc_monitor_local->lls_alc_session = lls_sls_alc_session;
		lls_sls_alc_monitor_local->atsc3_lls_slt_service = atsc3_lls_slt_service;
		lls_sls_alc_monitor_local->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;


		__WARN("process_packet: adding lls_sls_alc_monitor: %p to lls_slt_monitor: %p, service_id: %d",
			   lls_sls_alc_monitor_local, lls_slt_monitor, lls_sls_alc_session->service_id);

		lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor_local);

		if(!lls_sls_alc_monitor) {
			lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor_local;
			lls_sls_alc_monitor =  lls_sls_alc_monitor_local;
		}
    } else {
    	println("%s - a STLTP de-packetization and ROUTE/ALC extraction tool", argv[0]);
    	println("---");
    	println("args: file (stltp_dst_ip stltp_dst_port"); // (service_id)|

    	println(" file: file to process for stltp extraction  - defaults to %s:%s", stltp_dst_ip, stltp_dst_port);
    	println(" --- optional ---");
    	println("   stltp_dst_ip: stltp dest ip address");
    	println("   stltp_dst_port: stltp dest port");
    	println("");
    	exit(1);
    }



	PcapSTLTPVirtualPHY* pcapSTLTPVirtualPHY = new PcapSTLTPVirtualPHY();

	//atsc3_core_service_bridge_process_packet_phy(phy_payload_to_process);

	pcapSTLTPVirtualPHY->setRxUdpPacketProcessCallback(phy_rx_udp_packet_process_callback);
	pcapSTLTPVirtualPHY->atsc3_pcap_stltp_listen_ip_port_plp(stltp_dst_ip, stltp_dst_port, ATSC3_STLTP_DEPACKETIZER_ALL_PLPS_VALUE);
	pcapSTLTPVirtualPHY->atsc3_pcap_replay_open_file(filename);

	pcapSTLTPVirtualPHY->atsc3_pcap_thread_run();
	atsc3_pcap_replay_context_t* atsc3_pcap_replay_context_volitale = NULL;

	double pcap_thread_run_start_time = gt();
	sleep(1);
	while(pcapSTLTPVirtualPHY->is_pcap_replay_running()) {
		usleep(1000000);
		atsc3_pcap_replay_context_volitale = pcapSTLTPVirtualPHY->get_pcap_replay_context_status_volatile();
		//not mutexed, but shouldn't be disposed until we invoke atsc3_pcap_thread_stop
		if(atsc3_pcap_replay_context_volitale) {
			_PCAP_STLTP_VIRTUAL_TEST_DEBUG("pcap_file_pos: %d, pcap_file_len: %d",
					atsc3_pcap_replay_context_volitale->pcap_file_pos,
					atsc3_pcap_replay_context_volitale->pcap_file_len);
		}
	}
	double pcap_thread_run_end_time = gt();
	atsc3_pcap_replay_context_volitale = pcapSTLTPVirtualPHY->get_pcap_replay_context_status_volatile();

	double first_packet_ts_s = (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_sec + (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_usec / 1000000.0));
	double last_packet_ts_s = (atsc3_pcap_replay_context_volitale->last_wallclock_timeval.tv_sec + (atsc3_pcap_replay_context_volitale->last_wallclock_timeval.tv_usec / 1000000.0));

	_PCAP_STLTP_VIRTUAL_TEST_INFO("...completed, start time: %0.3f, end time: %0.3f, duration: %0.3f, first_packet_ts_sec: %0.3f, last_packet_ts_sec: %0.3f, delta: %0.3f",
			pcap_thread_run_start_time,
			pcap_thread_run_end_time,
			pcap_thread_run_end_time - pcap_thread_run_start_time,
			first_packet_ts_s,
			last_packet_ts_s,
			(last_packet_ts_s - first_packet_ts_s)
			);

	pcapSTLTPVirtualPHY->atsc3_pcap_thread_stop();

	delete pcapSTLTPVirtualPHY;


    return 0;
}


