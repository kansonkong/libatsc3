/*
 * atsc3_lls_listener_test.c
 *
 *  Created on: Jan 19, 2019
 *      Author: jjustman
 *
 *
 * borrowed from https://stackoverflow.com/questions/26275019/how-to-read-and-send-udp-packets-on-mac-os-x
 * uses libpacp for udp mulicast packet listening
 *
 * opt flags:
  export LDFLAGS="-L/usr/local/opt/libpcap/lib"
  export CPPFLAGS="-I/usr/local/opt/libpcap/include"

  to invoke test driver, run ala:

  ./atsc3_lls_listener_test vnic1 | grep 224.0.23.60

  output should look like


atsc3_lls_listener_test.c:100:DEBUG:Destination Address		239.255.10.4

atsc3_lls_listener_test.c:99:DEBUG:Source Address			192.168.0.4

atsc3_lls_listener_test.c:153:DEBUG:Dst. Address : 224.0.23.60 (3758102332)	Dst. Port    : 4937  	Data length: 193
 */
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

#include <atsc3_listener_udp.h>
#include <atsc3_pcap_type.h>

#include <atsc3_lls.h>
#include <atsc3_lls_types.h>
#include <atsc3_lls_slt_parser.h>

#include <atsc3_logging_externs.h>

#include <phy/virtual/PcapSTLTPVirtualPHY.h>


#define _ATSC3_LLS_LISTENER_TEST_FROM_STLTP_PCAP_TEST_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_LLS_LISTENER_TEST_FROM_STLTP_PCAP_TEST_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__); _ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_LLS_LISTENER_TEST_FROM_STLTP_PCAP_TEST_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__); _ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_LLS_LISTENER_TEST_FROM_STLTP_PCAP_TEST_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_LLS_LISTENER_TEST_FROM_STLTP_PCAP_TEST_TRACE(...)   printf("%s:%d:TRACE:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);


lls_slt_monitor_t* lls_slt_monitor;


void process_packet(block_t* ip_blockt) {
    udp_packet_t* udp_packet = udp_packet_process_from_block_t(ip_blockt);

	if(!udp_packet) {
		return;
	}

	//dispatch for LLS extraction and dump
	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {

		atsc3_lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data);
        
		if(lls_table) {
			printf("---\n");
			lls_dump_instance_table(lls_table);
		} else {
			__WARN("Error parsing LLS payload, data len: %u", block_Remaining_size(udp_packet->data));
		}
	}

	if(udp_packet->data) {
		block_Destroy(&udp_packet->data);
		udp_packet->data = NULL;
	}

	if(udp_packet) {
		free(udp_packet);
		udp_packet = NULL;
	}
}


uint64_t rx_udp_invocation_count = 0;

void atsc3_stltp_baseband_alp_packet_collection_callback(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection) {

    for(int i=0; i < atsc3_alp_packet_collection->atsc3_baseband_packet_v.count; i++) {
        atsc3_baseband_packet_t* atsc3_baseband_packet = atsc3_alp_packet_collection->atsc3_baseband_packet_v.data[i];

        //atsc3_baseband_packet_dump(atsc3_baseband_packet);

    }

    for(int i=0; i < atsc3_alp_packet_collection->atsc3_alp_packet_v.count; i++) {
        atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_collection->atsc3_alp_packet_v.data[i];
        block_Rewind(atsc3_alp_packet->alp_payload);
        atsc3_alp_packet_dump(atsc3_alp_packet);
    }
}

void atsc3_stltp_timing_management_packet_collection_callback(atsc3_stltp_timing_management_packet_tv* atsc3_stltp_timing_management_packet_v) {
    for(int i=0; i < atsc3_stltp_timing_management_packet_v->count; i++) {

        atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet = atsc3_stltp_timing_management_packet_v->data[i];
        atsc3_timing_management_packet_t* atsc3_timing_management_packet = atsc3_stltp_timing_management_packet->timing_management_packet;

        //atsc3_timing_management_packet_dump(atsc3_timing_management_packet);
    }

}

void atsc3_stltp_preamble_packet_collection_callback(atsc3_stltp_preamble_packet_tv* atsc3_stltp_preamble_packet_v) {
    for(int i=0; i < atsc3_stltp_preamble_packet_v->count; i++) {

        atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet = atsc3_stltp_preamble_packet_v->data[i];
        atsc3_preamble_packet_t* atsc3_preamble_packet = atsc3_stltp_preamble_packet->preamble_packet;

        //atsc3_preamble_packet_dump(atsc3_preamble_packet);
    }
}

void phy_rx_udp_packet_process_callback(uint8_t plp_num, block_t* packet) {
    if((rx_udp_invocation_count++ % 10000) == 0) {
        _ATSC3_LLS_LISTENER_TEST_FROM_STLTP_PCAP_TEST_DEBUG("PLP: %d, packet number: %" PRIu64 ", packet: %p, len: %d",
                plp_num, rx_udp_invocation_count, packet, packet->p_size);
    }
    
    process_packet(packet);
}



int process_stltp_pcap_file(char* filename, char* stltp_dst_ip, char* stltp_dst_port) {

    atsc3_pcap_replay_context_t* atsc3_pcap_replay_context_volitale = NULL;
    atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = NULL;

    PcapSTLTPVirtualPHY* pcapSTLTPVirtualPHY = new PcapSTLTPVirtualPHY();

    pcapSTLTPVirtualPHY->setRxUdpPacketProcessCallback(phy_rx_udp_packet_process_callback);
    atsc3_stltp_depacketizer_context = pcapSTLTPVirtualPHY->get_atsc3_stltp_depacketizer_context();

    //wire up specific callbacks for premable and t&m packets here
    //atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback = atsc3_stltp_baseband_alp_packet_collection_callback;
    //atsc3_stltp_depacketizer_context->atsc3_stltp_timing_management_packet_collection_callback = atsc3_stltp_timing_management_packet_collection_callback;
    //atsc3_stltp_depacketizer_context->atsc3_stltp_preamble_packet_collection_callback = atsc3_stltp_preamble_packet_collection_callback;

    pcapSTLTPVirtualPHY->atsc3_pcap_stltp_listen_ip_port_plp(stltp_dst_ip, stltp_dst_port, ATSC3_STLTP_DEPACKETIZER_ALL_PLPS_VALUE);

    pcapSTLTPVirtualPHY->atsc3_pcap_replay_open_file(filename);

    pcapSTLTPVirtualPHY->atsc3_pcap_thread_run();

    double pcap_thread_run_start_time = gt();
    sleep(1);
    while(pcapSTLTPVirtualPHY->is_pcap_replay_running()) {
        usleep(1000000);
        atsc3_pcap_replay_context_volitale = pcapSTLTPVirtualPHY->get_pcap_replay_context_status_volatile();
        //not mutexed, but shouldn't be disposed until we invoke atsc3_pcap_thread_stop
        if(atsc3_pcap_replay_context_volitale) {
            _ATSC3_LLS_LISTENER_TEST_FROM_STLTP_PCAP_TEST_DEBUG("pcap_file_pos: %d, pcap_file_len: %d",
                    atsc3_pcap_replay_context_volitale->pcap_file_pos,
                    atsc3_pcap_replay_context_volitale->pcap_file_len);
        }
    }
    double pcap_thread_run_end_time = gt();
    atsc3_pcap_replay_context_volitale = pcapSTLTPVirtualPHY->get_pcap_replay_context_status_volatile();

    double first_packet_ts_s = (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_sec + (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_usec / 1000000.0));
    double last_packet_ts_s = (atsc3_pcap_replay_context_volitale->last_wallclock_timeval.tv_sec + (atsc3_pcap_replay_context_volitale->last_wallclock_timeval.tv_usec / 1000000.0));

    _ATSC3_LLS_LISTENER_TEST_FROM_STLTP_PCAP_TEST_INFO("...completed, start time: %0.3f, end time: %0.3f, duration: %0.3f, first_packet_ts_sec: %0.3f, last_packet_ts_sec: %0.3f, delta: %0.3f",
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



int main(int argc,char **argv) {

	_LLS_INFO_ENABLED = 1;

#ifdef __LOTS_OF_DEBUGGING__


#endif

	_LLS_DEBUG_ENABLED = 1;
	_LLS_TRACE_ENABLED = 1;

    
    char* PCAP_FILENAME = "";
    
    char* stltp_dst_ip = "239.239.239.239";
    char* stltp_dst_port = "30000";
    
    struct bpf_program fp;
    bpf_u_int32 maskp;
    bpf_u_int32 netp;

    if(argc == 1) {
    	println("%s - a udp mulitcast listener test harness for atsc3 LLS messages, defaults to: 224.0.23.60:4937:", argv[0]);
		println("---");
    	println("args: dev");
    	println(" dev: device to listen for udp multicast, default listen to 0.0.0.0:0");

		exit(1);
    }
    
    
    //listen to all flows
    if(argc >= 2) {
        PCAP_FILENAME = argv[1];
    }
    
    if(argc==4) {
        stltp_dst_ip = argv[2];
        stltp_dst_port = argv[3];
    }
    
    lls_slt_monitor = lls_slt_monitor_create();

    process_stltp_pcap_file(PCAP_FILENAME, stltp_dst_ip, stltp_dst_port);

    return 0;
}

