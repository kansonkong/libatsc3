/*
 * srtRxSTLTPVirtualPHYTest.cpp
 *
 *  Created on: Aug 10, 2020
 *      Author: jjustman
 */


#include <stdio.h>
#include <string.h>

#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>

#include <phy/virtual/SRTRxSTLTPVirtualPHY.h>

#define _SRT_STLTP_VIRTUAL_TEST_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_TEST_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_TEST_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _SRT_STLTP_VIRTUAL_TEST_DEBUG(...)   __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);

uint64_t rx_udp_invocation_count = 0;

void phy_rx_udp_packet_process_callback(uint8_t plp_num, block_t* packet) {
	//if((rx_udp_invocation_count % 1000) == 0) {
		_SRT_STLTP_VIRTUAL_TEST_DEBUG("PLP: %d, packet number: %llu, packet: %p, len: %d",
				plp_num, rx_udp_invocation_count, packet, packet->p_size);

	//}
	rx_udp_invocation_count++;

}

int test_srt_stltp_with_bna_rx() {
	const char* SRT_HOST_CONNECTION_STRING = "srt://bna.srt.atsc3.com:31347?passphrase=88731837-0EB5-4951-83AA-F515B3BEBC20";

	SRTRxSTLTPVirtualPHY* srtRxSTLTPVirtualPHY = new SRTRxSTLTPVirtualPHY(SRT_HOST_CONNECTION_STRING);

	//atsc3_core_service_bridge_process_packet_phy(phy_payload_to_process);

	srtRxSTLTPVirtualPHY->setRxUdpPacketProcessCallback(phy_rx_udp_packet_process_callback);

	srtRxSTLTPVirtualPHY->run();


	double srt_thread_run_start_time = gt();

	int loop_count = 0;
	bool should_break = false;
	sleep(1);
	while((gt() - srt_thread_run_start_time) < 60 &&  !should_break) {
		usleep(1000000);
		_SRT_STLTP_VIRTUAL_TEST_INFO("srt_is_running: %d", srtRxSTLTPVirtualPHY->is_running());
		if(loop_count++ > 10) {
			should_break = !srtRxSTLTPVirtualPHY->is_running();
		}
	}
	double srt_thread_run_end_time = gt();

//	double first_packet_ts_s = (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_sec + (atsc3_pcap_replay_context_volitale->first_wallclock_timeval.tv_usec / 1000000.0));
//	double last_packet_ts_s = (atsc3_pcap_replay_context_volitale->last_packet_ts_sec + (atsc3_pcap_replay_context_volitale->last_packet_ts_usec / 1000000.0));
//	, first_packet_ts_sec: %0.3f, last_packet_ts_sec: %0.3f, delta: %0.3f",

	_SRT_STLTP_VIRTUAL_TEST_INFO("...completed, start time: %0.3f, end time: %0.3f, duration: %0.3f",
			srt_thread_run_start_time,
			srt_thread_run_end_time,
			srt_thread_run_end_time - srt_thread_run_start_time);

	srtRxSTLTPVirtualPHY->stop();

	delete srtRxSTLTPVirtualPHY;

	return 0;
}

int main(int argc, char* argv[] ) {

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

	_SRT_STLTP_VIRTUAL_TEST_INFO("starting test_srt_stltp_with_bna_rx test");

	test_srt_stltp_with_bna_rx();


    return 0;
}



