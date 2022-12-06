/*
 * atsc3_stltp_alp_listener_reflector_test_all_plps_from_stltp_pcap.cpp
 *
 *  Created on: 2022-10-20

 *      Author: jjustman
 *
 * stltp pcap reflector for atsc a/324
 *
 *
 *
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


#include <atsc3_utils.h>
#include <atsc3_listener_udp.h>
#include <atsc3_pcap_type.h>

#include <atsc3_ntp_utils.h>
#include <atsc3_logging_externs.h>

#include <atsc3_stltp_types.h>

#include <phy/virtual/PcapSTLTPVirtualPHY.h>



#define _ATSC3_STLTP_ALP_LISTENER_REFLECTOR_TEST_ALL_PLPS_FROM_STLTP_PCAP_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_STLTP_ALP_LISTENER_REFLECTOR_TEST_ALL_PLPS_FROM_STLTP_PCAP_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__); _ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_STLTP_ALP_LISTENER_REFLECTOR_TEST_ALL_PLPS_FROM_STLTP_PCAP_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__); _ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_STLTP_ALP_LISTENER_REFLECTOR_TEST_ALL_PLPS_FROM_STLTP_PCAP_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_STLTP_ALP_LISTENER_REFLECTOR_TEST_ALL_PLPS_FROM_STLTP_PCAP_TRACE(...)   printf("%s:%d:TRACE:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);


int PACKET_COUNTER = 0;

atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context;

extern pcap_t* descrInject;

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	atsc3_stltp_depacketizer_from_pcap_frame(user, pkthdr, packet, atsc3_stltp_depacketizer_context);
}


int process_stltp_pcap_file_optional_dst_ip_dst_port(char* filename, pcap_t* descrInject, char* stltp_dst_ip, char* stltp_dst_port) {

    atsc3_pcap_replay_context_t* atsc3_pcap_replay_context_volitale = NULL;
    atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = NULL;

    PcapSTLTPVirtualPHY* pcapSTLTPVirtualPHY = new PcapSTLTPVirtualPHY();

    //pcapSTLTPVirtualPHY->setRxUdpPacketProcessCallback(phy_rx_udp_packet_process_callback);
    atsc3_stltp_depacketizer_context = pcapSTLTPVirtualPHY->get_atsc3_stltp_depacketizer_context();

    atsc3_stltp_depacketizer_context_set_all_plps(atsc3_stltp_depacketizer_context);
    atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_with_pcap_device_reference = atsc3_reflect_alp_packet_collection;
    atsc3_stltp_depacketizer_context->atsc3_baseband_alp_output_pcap_device_reference = descrInject;

    if(stltp_dst_ip && stltp_dst_port) {
        pcapSTLTPVirtualPHY->atsc3_pcap_stltp_listen_ip_port_plp(stltp_dst_ip, stltp_dst_port, ATSC3_STLTP_DEPACKETIZER_ALL_PLPS_VALUE);
    }
    
    pcapSTLTPVirtualPHY->atsc3_pcap_replay_open_file(filename);

    pcapSTLTPVirtualPHY->atsc3_pcap_thread_run();

    double pcap_thread_run_start_time = gt();
    sleep(1);
    while(pcapSTLTPVirtualPHY->is_pcap_replay_running()) {
        usleep(1000000);
        atsc3_pcap_replay_context_volitale = pcapSTLTPVirtualPHY->get_pcap_replay_context_status_volatile();
        //not mutexed, but shouldn't be disposed until we invoke atsc3_pcap_thread_stop
        if(atsc3_pcap_replay_context_volitale) {
            _ATSC3_STLTP_ALP_LISTENER_REFLECTOR_TEST_ALL_PLPS_FROM_STLTP_PCAP_DEBUG("pcap_file_pos: %d, pcap_file_len: %d",
                    atsc3_pcap_replay_context_volitale->pcap_file_pos,
                    atsc3_pcap_replay_context_volitale->pcap_file_len);
        }
    }
    double pcap_thread_run_end_time = gt();
    atsc3_pcap_replay_context_volitale = pcapSTLTPVirtualPHY->get_pcap_replay_context_status_volatile();

    double first_packet_ts_s = (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_sec + (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_usec / 1000000.0));
    double last_packet_ts_s = (atsc3_pcap_replay_context_volitale->last_wallclock_timeval.tv_sec + (atsc3_pcap_replay_context_volitale->last_wallclock_timeval.tv_usec / 1000000.0));

    _ATSC3_STLTP_ALP_LISTENER_REFLECTOR_TEST_ALL_PLPS_FROM_STLTP_PCAP_INFO("...completed, start time: %0.3f, end time: %0.3f, duration: %0.3f, first_packet_ts_sec: %0.3f, last_packet_ts_sec: %0.3f, delta: %0.3f",
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
    _IP_UDP_RTP_PARSER_DEBUG_ENABLED = 0;
    _IP_UDP_RTP_PARSER_TRACE_ENABLED = 0;

    _ATSC3_UTILS_TRACE_ENABLED = 0;
	
	//jjustman-2020-12-31 - extra debugging
	_STLTP_PARSER_INFO_ENABLED  = 1;
	_STLTP_PARSER_DUMP_ENABLED  = 0;
	_STLTP_PARSER_DEBUG_ENABLED = 0;
	_STLTP_PARSER_TRACE_ENABLED = 0;

    _STLTP_TYPES_DEBUG_ENABLED = 0;
    _STLTP_TYPES_TRACE_ENABLED = 0;
    

    char* PCAP_FILENAME = "";
    
    char* stltp_dst_ip = NULL;
    char* stltp_dst_port = NULL;


    char *devInject;
	
    char errbuf[PCAP_ERRBUF_SIZE];
    char errbufInject[PCAP_ERRBUF_SIZE];

    pcap_t* descr;
    struct bpf_program fp;
    bpf_u_int32 maskp;
    bpf_u_int32 netp;
    
    struct bpf_program fpInject;
    bpf_u_int32 maskpInject;
    bpf_u_int32 netpInject;
    

    uint32_t* dst_ip_addr_filter = NULL;
    uint16_t* dst_ip_port_filter = NULL;

    atsc3_stltp_depacketizer_context = atsc3_stltp_depacketizer_context_new();

    if(argc < 3) {
        println("%s - an atsc3 stltp udp mulitcast reflector ", argv[0]);
        println("---");
        println("args: stltp_pcap_file_name devInject (stltp_ip_address) (stltp_port)");
        println(" stltp_pcap_file_name : STLTP pcap file to replay");
        println(" devInject            : eth device name for stltp demuxed emission");
        println("");
        println(" (stltp_ip_address)    : optional, override auto-detect for stltp ip address from pcap file");
        println(" (stltp_port)          : optional, override auto-detect for stltp port address from pcap file");

        exit(1);
    }
    
    //listen to all flows
    if(argc >= 3) {
        PCAP_FILENAME = argv[1];
        devInject = argv[2];

    }
    
    if(argc >= 4) {
        stltp_dst_ip = argv[3];
        stltp_dst_port = argv[4];
    }
    
    
    println("%s -an atsc3 stltp udp mulitcast reflector, pcap file: %s, devInject: %s, stltp_dst_ip: %s, stltp_dst_port: %s",
            argv[0], PCAP_FILENAME, devInject, (stltp_dst_ip ? stltp_dst_ip : "auto"), (stltp_dst_port ? stltp_dst_port : "auto"));
    
    //inject dev i/f
    pcap_lookupnet(devInject, &netpInject, &maskpInject, errbufInject);
    pcap_t* descrInject = pcap_open_live(devInject, MAX_PCAP_LEN, 1, 1, errbufInject);
    
    process_stltp_pcap_file_optional_dst_ip_dst_port(PCAP_FILENAME, descrInject, stltp_dst_ip, stltp_dst_port);
        
    return 0;
}

