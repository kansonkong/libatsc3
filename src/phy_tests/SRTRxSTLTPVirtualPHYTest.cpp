/*
 * srtRxSTLTPVirtualPHYTest.cpp
 *
 *  Created on: Aug 10, 2020
 *      Author: jjustman
 */


#include <stdio.h>
#include <string.h>

#include <atsc3_utils.h>

#include <phy/virtual/SRTRxSTLTPVirtualPHY.h>

#define _SRT_STLTP_VIRTUAL_TEST_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_TEST_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_TEST_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_TEST_DEBUG(...)   __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);

uint64_t rx_udp_invocation_count = 0;

void phy_rx_udp_packet_process_callback(uint8_t plp_num, block_t* packet) {
//	if((rx_udp_invocation_count++ % 10) == 0) {
		_SRT_STLTP_VIRTUAL_TEST_DEBUG("PLP: %d, packet number: %llu, packet: %p, len: %d",
				plp_num, rx_udp_invocation_count, packet, packet->p_size);
//	}
}

int test_srt_stltp_with_bna_rx() {
	const char* SRT_HOST_CONNECTION_STRING = "srt://bna.srt.atsc3.com:31347?passphrase=88731837-0EB5-4951-83AA-F515B3BEBC20";

	SRTRxSTLTPVirtualPHY* srtRxSTLTPVirtualPHY = new SRTRxSTLTPVirtualPHY(SRT_HOST_CONNECTION_STRING);

	//atsc3_core_service_bridge_process_packet_phy(phy_payload_to_process);

	srtRxSTLTPVirtualPHY->SetRxUdpPacketProcessCallback(phy_rx_udp_packet_process_callback);

	srtRxSTLTPVirtualPHY->atsc3_srt_thread_run();


	double srt_thread_run_start_time = gt();

	sleep(1);
	while((gt() - srt_thread_run_start_time) < 10 ) {
		usleep(1000000);
		_SRT_STLTP_VIRTUAL_TEST_INFO("srt_is_running: %d", srtRxSTLTPVirtualPHY->is_srt_running());

//		atsc3_pcap_replay_context_volitale = srtRxSTLTPVirtualPHY->get_pcap_replay_context_status_volatile();
//		//not mutexed, but shouldn't be disposed until we invoke atsc3_pcap_thread_stop
//		if(atsc3_pcap_replay_context_volitale) {
//			_SRT_STLTP_VIRTUAL_TEST_DEBUG("pcap_file_pos: %d, pcap_file_len: %d",
//					atsc3_pcap_replay_context_volitale->pcap_file_pos,
//					atsc3_pcap_replay_context_volitale->pcap_file_len);
//		}
	}
	double srt_thread_run_end_time = gt();

//	double first_packet_ts_s = (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_sec + (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_usec / 1000000.0));
//	double last_packet_ts_s = (atsc3_pcap_replay_context_volitale->last_packet_ts_sec + (atsc3_pcap_replay_context_volitale->last_packet_ts_usec / 1000000.0));
//	, first_packet_ts_sec: %0.3f, last_packet_ts_sec: %0.3f, delta: %0.3f",

	_SRT_STLTP_VIRTUAL_TEST_INFO("...completed, start time: %0.3f, end time: %0.3f, duration: %0.3f",
			srt_thread_run_start_time,
			srt_thread_run_end_time,
			srt_thread_run_end_time - srt_thread_run_start_time);

	srtRxSTLTPVirtualPHY->atsc3_srt_thread_stop();

	delete srtRxSTLTPVirtualPHY;

	return 0;
}

int main(int argc, char* argv[] ) {

	_SRT_STLTP_VIRTUAL_TEST_INFO("starting test_srt_stltp_with_bna_rx test");

	test_srt_stltp_with_bna_rx();


    return 0;
}



