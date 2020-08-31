/*
 * PcapDemuxedVirtualPHYTest.cpp
 *
 *  Created on: Aug 10, 2020
 *      Author: jjustman
 */


#include <stdio.h>
#include <string.h>

#include <atsc3_utils.h>

#include <phy/virtual/PcapDemuxedVirtualPHY.h>

#define _PCAP_STLTP_VIRTUAL_TEST_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _PCAP_DEMUXED_VIRTUAL_TEST_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _PCAP_DEMUXED_VIRTUAL_TEST_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _PCAP_DEMUXED_VIRTUAL_TEST_DEBUG(...)   __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);

uint64_t rx_udp_invocation_count = 0;

void phy_rx_udp_packet_process_callback(uint8_t plp, block_t* packet) {
	if((rx_udp_invocation_count++ % 1000) == 0) {
		_PCAP_DEMUXED_VIRTUAL_TEST_DEBUG("packet number: %llu, packet: %p, len: %d",
				rx_udp_invocation_count, packet, packet->p_size);
	}
}

int test_demux_1m_100000_packet_replay() {
	const char* PCAP_REPLAY_TEST_FILENAME = "testdata/demux/20200729.1058.demux.100000.pcap";
	PcapDemuxedVirtualPHY* pcapDemuxedVirtualPHY = new PcapDemuxedVirtualPHY();
	//            atsc3_core_service_bridge_process_packet_phy(phy_payload_to_process);

	pcapDemuxedVirtualPHY->setRxUdpPacketProcessCallback(phy_rx_udp_packet_process_callback);

	pcapDemuxedVirtualPHY->atsc3_pcap_replay_open_file(PCAP_REPLAY_TEST_FILENAME);

	pcapDemuxedVirtualPHY->atsc3_pcap_thread_run();
	atsc3_pcap_replay_context_t* atsc3_pcap_replay_context_volitale = NULL;

	double pcap_thread_run_start_time = gt();
	sleep(1);
	while(pcapDemuxedVirtualPHY->is_pcap_replay_running()) {
		usleep(1000000);
		atsc3_pcap_replay_context_volitale = pcapDemuxedVirtualPHY->get_pcap_replay_context_status_volatile();
		//not mutexed, but shouldn't be disposed until we invoke atsc3_pcap_thread_stop
		if(atsc3_pcap_replay_context_volitale) {
			_PCAP_DEMUXED_VIRTUAL_TEST_DEBUG("pcap_file_pos: %d, pcap_file_len: %d",
					atsc3_pcap_replay_context_volitale->pcap_file_pos,
					atsc3_pcap_replay_context_volitale->pcap_file_len);
		}
	}
	double pcap_thread_run_end_time = gt();
	atsc3_pcap_replay_context_volitale = pcapDemuxedVirtualPHY->get_pcap_replay_context_status_volatile();

	double first_packet_ts_s = (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_sec + (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_usec / 1000000.0));
	double last_packet_ts_s = (atsc3_pcap_replay_context_volitale->last_packet_ts_sec + (atsc3_pcap_replay_context_volitale->last_packet_ts_usec / 1000000.0));

	_PCAP_DEMUXED_VIRTUAL_TEST_INFO("...completed, start time: %0.3f, end time: %0.3f, duration: %0.3f, first_packet_ts_sec: %0.3f, last_packet_ts_sec: %0.3f, delta: %0.3f",
			pcap_thread_run_start_time,
			pcap_thread_run_end_time,
			pcap_thread_run_end_time - pcap_thread_run_start_time,
			first_packet_ts_s,
			last_packet_ts_s,
			(last_packet_ts_s - first_packet_ts_s)
			);

	pcapDemuxedVirtualPHY->atsc3_pcap_thread_stop();

	delete pcapDemuxedVirtualPHY;

	return 0;
}

int main(int argc, char* argv[] ) {

	_PCAP_DEMUXED_VIRTUAL_TEST_INFO("starting test_demux_1m_100000_packet_replay test");

	test_demux_1m_100000_packet_replay();


    return 0;
}



