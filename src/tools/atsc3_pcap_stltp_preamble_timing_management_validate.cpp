/*
 * pcapSTLTPVirtualPHYTest.cpp
 *
 *  Created on: Aug 10, 2020
 *      Author: jjustman
 */


#include <stdio.h>
#include <string.h>

#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>
#include <atsc3_stltp_types.h>

#include <phy/virtual/PcapSTLTPVirtualPHY.h>

#define _PCAP_STLTP_VIRTUAL_TEST_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _PCAP_STLTP_VIRTUAL_TEST_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _PCAP_STLTP_VIRTUAL_TEST_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _PCAP_STLTP_VIRTUAL_TEST_DEBUG(...)   __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);

uint64_t rx_udp_invocation_count = 0;

void atsc3_stltp_baseband_alp_packet_collection_callback(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection) {

	for(int i=0; i < atsc3_alp_packet_collection->atsc3_baseband_packet_v.count; i++) {
		atsc3_baseband_packet_t* atsc3_baseband_packet = atsc3_alp_packet_collection->atsc3_baseband_packet_v.data[i];

		atsc3_baseband_packet_dump(atsc3_baseband_packet);

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

		atsc3_timing_management_packet_dump(atsc3_timing_management_packet);
	}

}

void atsc3_stltp_preamble_packet_collection_callback(atsc3_stltp_preamble_packet_tv* atsc3_stltp_preamble_packet_v) {
	for(int i=0; i < atsc3_stltp_preamble_packet_v->count; i++) {

		atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet = atsc3_stltp_preamble_packet_v->data[i];
		atsc3_preamble_packet_t* atsc3_preamble_packet = atsc3_stltp_preamble_packet->preamble_packet;

		atsc3_preamble_packet_dump(atsc3_preamble_packet);
	}
}

void phy_rx_udp_packet_process_callback(uint8_t plp_num, block_t* packet) {
	if((rx_udp_invocation_count++ % 1000) == 0) {
		_PCAP_STLTP_VIRTUAL_TEST_DEBUG("PLP: %d, packet number: %llu, packet: %p, len: %d",
				plp_num, rx_udp_invocation_count, packet, packet->p_size);
	}
}

int process_stltp_pcap_file(char* filename, char* stltp_dst_ip, char* stltp_dst_port) {

	atsc3_pcap_replay_context_t* atsc3_pcap_replay_context_volitale = NULL;
	atsc3_stltp_depacketizer_context_t* atsc3_stltp_depacketizer_context = NULL;

	PcapSTLTPVirtualPHY* pcapSTLTPVirtualPHY = new PcapSTLTPVirtualPHY();

	pcapSTLTPVirtualPHY->setRxUdpPacketProcessCallback(phy_rx_udp_packet_process_callback);
	atsc3_stltp_depacketizer_context = pcapSTLTPVirtualPHY->get_atsc3_stltp_depacketizer_context();

	//wire up specific callbacks for premable and t&m packets here
	atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback = atsc3_stltp_baseband_alp_packet_collection_callback;
	atsc3_stltp_depacketizer_context->atsc3_stltp_timing_management_packet_collection_callback = atsc3_stltp_timing_management_packet_collection_callback;
	atsc3_stltp_depacketizer_context->atsc3_stltp_preamble_packet_collection_callback = atsc3_stltp_preamble_packet_collection_callback;


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

int main(int argc, char* argv[] ) {

	char* filename = "";
    char* stltp_dst_ip = "239.239.239.239";
    char* stltp_dst_port = "30000";

    _STLTP_PARSER_INFO_ENABLED = 1;
    _STLTP_PARSER_DUMP_ENABLED = 1;
    _ATSC3_ALP_TYPES_DUMP_ENABLED = 1;

	if(argc == 2) {
		filename = argv[1];
	}  else if(argc==4) {

    	//listen to a selected flow
		filename = argv[1];
		stltp_dst_ip = argv[2];
		stltp_dst_port = argv[3];
	} else {
		_PCAP_STLTP_VIRTUAL_TEST_INFO("%s - stltp pcap preamble & timing and management validation tool", argv[0]);
		_PCAP_STLTP_VIRTUAL_TEST_INFO("---");
		_PCAP_STLTP_VIRTUAL_TEST_INFO("args: file (stltp_dst_ip stltp_dst_port"); // (service_id)|
		_PCAP_STLTP_VIRTUAL_TEST_INFO(" file: file to process for stltp extraction - defaults to %s:%s", stltp_dst_ip, stltp_dst_port);
		_PCAP_STLTP_VIRTUAL_TEST_INFO(" --- optional ---");
		_PCAP_STLTP_VIRTUAL_TEST_INFO("   stltp_dst_ip: stltp dest ip address");
    	_PCAP_STLTP_VIRTUAL_TEST_INFO("   stltp_dst_port: stltp dest port");

		return -1;
	}

	_PCAP_STLTP_VIRTUAL_TEST_INFO("starting replay with filename: %s, stltp_dst_ip: %s, stltp_dst_port: %s", filename, stltp_dst_ip, stltp_dst_port);

	process_stltp_pcap_file(filename, stltp_dst_ip, stltp_dst_port);

    return 0;
}



