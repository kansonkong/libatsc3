/*
 * atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer.cpp
 *
 *  Created on: Aug 18, 2020
 *      Author: jjustman
 *
 *      notes: for running under lldb w/ asan build:
 *      	env ASAN_OPTIONS=detect_container_overflow=0
 *
 */


#include <stdio.h>
#include <string.h>

#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>

#include <atsc3_listener_udp.h>
#include <atsc3_lls.h>
#include <atsc3_lls_alc_utils.h>
#include <atsc3_alc_rx.h>
#include <atsc3_alc_utils.h>

#include <phy/virtual/SRTRxSTLTPVirtualPHY.h>

#define _SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_DEBUG(...)   __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);

uint64_t rx_udp_invocation_count = 0;
SRTRxSTLTPVirtualPHY* srtRxSTLTPVirtualPHY = NULL;
double srt_thread_run_start_time = 0;


lls_slt_monitor_t* lls_slt_monitor;
lls_sls_alc_monitor_t* lls_sls_alc_monitor;
atsc3_alc_arguments_t* alc_arguments;
atsc3_alc_session_t* atsc3_alc_session;

uint32_t alc_packet_received_count = 0;

void alc_process_from_udp_packet(udp_packet_t* udp_packet) {
	atsc3_alc_packet_t* alc_packet = NULL;

    //dispatch for LLS extraction and dump
    if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
        lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data);

        //auto-assign our first ROUTE service id here
        if(lls_table && lls_table->lls_table_id == SLT) {
            _SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("LLS table is:\n%s", lls_table->raw_xml.xml_payload);

            for(int i=0; i < lls_table->slt_table.atsc3_lls_slt_service_v.count; i++) {
                atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_table->slt_table.atsc3_lls_slt_service_v.data[i];
                if(atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count &&
                   atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol == SLS_PROTOCOL_ROUTE) {
					lls_sls_alc_monitor_t* lls_sls_alc_monitor_local = lls_sls_alc_monitor_create();

					lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);
					lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

					lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
					if(!lls_sls_alc_session) {
						_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("lls_slt_alc_session_find_from_service_id: lls_sls_alc_session is NULL!");
					}
					lls_sls_alc_monitor_local->lls_alc_session = lls_sls_alc_session;
					lls_sls_alc_monitor_local->atsc3_lls_slt_service = atsc3_lls_slt_service;
					lls_sls_alc_monitor_local->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;


					_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("process_packet: adding lls_sls_alc_monitor: %p to lls_slt_monitor: %p, service_id: %d",
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


    lls_sls_alc_monitor_t* matching_lls_sls_alc_monitor = atsc3_lls_sls_alc_monitor_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
	//only used for service id filtering.
    lls_sls_alc_session_t* matching_lls_slt_alc_session = lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);

    if(matching_lls_sls_alc_monitor && matching_lls_slt_alc_session) {

		//process ALC streams
		int retval = alc_rx_analyze_packet_a331_compliant((char*)block_Get(udp_packet->data), block_Remaining_size(udp_packet->data), &alc_packet);

		if(!retval) {
			//check our alc_packet for a wrap-around TOI value, if it is a monitored TSI, and re-patch the MBMS MPD for updated availabilityStartTime and startNumber with last closed TOI values
			atsc3_alc_packet_check_monitor_flow_for_toi_wraparound_discontinuity(alc_packet, matching_lls_sls_alc_monitor);

			//keep track of our EXT_FTI and update last_toi as needed for TOI length and manual set of the close_object flag
			atsc3_route_object_t* atsc3_route_object = atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows(alc_packet, matching_lls_sls_alc_monitor);

			//persist to disk, process sls mbms and/or emit ROUTE media_delivery_event complete to the application tier if
			//the full packet has been recovered (e.g. no missing data units in the forward transmission)
			if(atsc3_route_object) {
				atsc3_alc_packet_persist_to_toi_resource_process_sls_mbms_and_emit_callback(&udp_packet->udp_flow, alc_packet, matching_lls_sls_alc_monitor, atsc3_route_object);
				alc_packet_received_count++;

			} else {
				_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_ERROR("Error in ALC persist, atsc3_route_object is NULL!");

			}

		} else {
			_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_ERROR("Error in ALC decode: %d", retval);
		}
	} else {
#ifdef __PENDANTIC__
		_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("Discarding packet: lls_sls_alc_monitor: %p, matching_lls_sls_alc_monitor: %p, matching_lls_slt_alc_session: %p, packet: %u.%u.%u.%u:%u, size: %d",
				lls_sls_alc_monitor, matching_lls_sls_alc_monitor, matching_lls_slt_alc_session,
				__toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port),
				udp_packet->data->p_size);
#endif
	}

udp_packet_free:
	alc_packet_free(&alc_packet);
	alc_packet = NULL;

    return udp_packet_free(&udp_packet);
}

void phy_rx_udp_packet_process_callback(uint8_t plp_num, block_t* packet) {

    udp_packet_t* udp_packet = udp_packet_process_from_ptr(block_Get(packet), packet->p_size);

    if((rx_udp_invocation_count % 1000) == 0) {
		_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_DEBUG("PLP: %d, packet number: %llu, packet: %p, len: %d, udp_packet: %p",
					plp_num, rx_udp_invocation_count++, packet, packet->p_size, udp_packet);
    }

	if(!udp_packet) {
		return;
	}

	alc_process_from_udp_packet(udp_packet);
}

int start_srt_rx_stltp_virtual_phy(string srt_connection_string) {
	int res = -1;
	srtRxSTLTPVirtualPHY = new SRTRxSTLTPVirtualPHY(srt_connection_string);

	srtRxSTLTPVirtualPHY->setRxUdpPacketProcessCallback(phy_rx_udp_packet_process_callback);

	res = srtRxSTLTPVirtualPHY->run();

	srt_thread_run_start_time = gt();

	return res;
}

int stop_srt_rx_stltp_virtual_phy() {
	int res = -1;
	if(srtRxSTLTPVirtualPHY) {
		res = srtRxSTLTPVirtualPHY->stop();

		delete srtRxSTLTPVirtualPHY;
	}
	return res;
}

void configure_lls_sls_monitor() {

    lls_slt_monitor = lls_slt_monitor_create();
	alc_arguments = (atsc3_alc_arguments_t*)calloc(1, sizeof(atsc3_alc_arguments_t));
    atsc3_alc_session = atsc3_open_alc_session(alc_arguments);

    lls_sls_alc_monitor = lls_sls_alc_monitor_create();
    lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor);

//
//
//
//	atsc3_lls_slt_service_t* atsc3_lls_slt_service = atsc3_lls_slt_service_new();
//	atsc3_lls_slt_service->service_id=31337; //hack
//
//	atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = atsc3_slt_broadcast_svc_signalling_new();
//	atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address = dst_ip;
//	atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port =  dst_port;
//	atsc3_slt_broadcast_svc_signalling->sls_protocol = SLS_PROTOCOL_ROUTE;
//	atsc3_lls_slt_service_add_atsc3_slt_broadcast_svc_signalling(atsc3_lls_slt_service, atsc3_slt_broadcast_svc_signalling);
//
//	lls_slt_alc_session_find_or_create(lls_slt_monitor, atsc3_lls_slt_service);
//
//	lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);
//	lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);
//
//	lls_sls_alc_session_t* lls_sls_alc_session = lls_slt_alc_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
//	if(!lls_sls_alc_session) {
//		__WARN("lls_slt_alc_session_find_from_service_id: lls_sls_alc_session is NULL!");
//	}
//
//	lls_sls_alc_monitor_local->lls_alc_session = lls_sls_alc_session;
//	lls_sls_alc_monitor_local->atsc3_lls_slt_service = atsc3_lls_slt_service;
//	lls_sls_alc_monitor_local->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;
//
//
//	__WARN("process_packet: adding lls_sls_alc_monitor: %p to lls_slt_monitor: %p, service_id: %d",
//		   lls_sls_alc_monitor_local, lls_slt_monitor, lls_sls_alc_session->service_id);
//
//	lls_slt_monitor_add_lls_sls_alc_monitor(lls_slt_monitor, lls_sls_alc_monitor_local);
//
//	if(!lls_sls_alc_monitor) {
//		lls_slt_monitor->lls_sls_alc_monitor = lls_sls_alc_monitor_local;
//		lls_sls_alc_monitor =  lls_sls_alc_monitor_local;
//	}

}

int main(int argc, char* argv[] ) {

#ifdef __PENDANTIC__
	_ALP_PARSER_INFO_ENABLED = 1;
	_ALP_PARSER_DEBUG_ENABLED = 1;

	_ATSC3_STLTP_DEPACKETIZER_INFO_ENABLED = 1;
	_ATSC3_STLTP_DEPACKETIZER_DEBUG_ENABLED = 1;
	_ATSC3_STLTP_DEPACKETIZER_TRACE_ENABLED = 1;

	_STLTP_PARSER_INFO_ENABLED = 1;
	_STLTP_PARSER_DEBUG_ENABLED = 1;
	_STLTP_PARSER_TRACE_ENABLED = 1;

	_STLTP_TYPES_DEBUG_ENABLED = 1;
	_STLTP_TYPES_TRACE_ENABLED = 1;
#endif
	string srt_connection_string = "srt://las.srt.atsc3.com:31351?passphrase=6E35F28D-21B8-46A4-8081-F3232D150728&packetfilter=fec";

	if(argc > 1) {
		srt_connection_string = string(argv[1]);
	}

	configure_lls_sls_monitor();

	_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("starting atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer with connection: %s", srt_connection_string.c_str());
	start_srt_rx_stltp_virtual_phy(srt_connection_string);


	int loop_count = 0;
	bool should_break = false;
	sleep(1);
	while(!should_break) {
		usleep(10000000);
		_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_INFO("srt_is_running: %d", srtRxSTLTPVirtualPHY->is_running());
		if(loop_count++ > 10) {
			should_break = !srtRxSTLTPVirtualPHY->is_running();
		}
	}

	double srt_thread_run_end_time = gt();

	_SRT_STLTP_VIRTUAL_PHY_ALC_WRITER_WARN("srtRxSTLTPVirtualPHY !running, start time: %0.3f, end time: %0.3f, duration: %0.3f",
			srt_thread_run_start_time,
			srt_thread_run_end_time,
			srt_thread_run_end_time - srt_thread_run_start_time);

	stop_srt_rx_stltp_virtual_phy();


    return 0;
}




