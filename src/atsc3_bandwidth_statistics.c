/*
 * atsc3_bandwidth_statistics.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#include "atsc3_output_statistics_ncurses.h"
#include "atsc3_bandwidth_statistics.h"

bandwidth_statistics_t *global_bandwidth_statistics;

void doBandwidthStatusUpdate();

void *print_bandwidth_statistics_thread(void *vargp)
{
#ifndef __DISABLE_NCURSES__

	__BW_TRACE("Starting printBandwidthStatistics");
    setlocale(LC_ALL,"");

	while(true) {
		gettimeofday(&global_bandwidth_statistics->snapshot_timeval_start, NULL);
		sleep(1);

		ncurses_writer_lock_mutex_acquire();
		__BW_STATS_NOUPDATE();
		doBandwidthStatusUpdate();
		__DOUPDATE();
		ncurses_writer_lock_mutex_release();
	}
#endif
	return NULL;
}

void doBandwidthStatusUpdate() {

#ifndef __DISABLE_NCURSES__

	struct timeval time_now;

	//acquire our ncurses_writer mutex

	gettimeofday(&time_now, NULL);
	__BW_TRACE("using snapshot_timeval_start: %lu, time_now as global_bandwidth_statistics->snapshot_timeval_start to: %lu", global_bandwidth_statistics->snapshot_timeval_start.tv_sec, time_now.tv_sec);

	//delta is in 5001715 / 1000000.0 -> 5001715
	long long deltaTuS = timediff(time_now, global_bandwidth_statistics->snapshot_timeval_start);
	double deltaTS = deltaTuS/1000000.0;

	long long runtimeTuS = timediff(time_now, global_bandwidth_statistics->program_timeval_start);
	double runtimeTS = runtimeTuS/1000000.0;
	__BW_CLEAR(); //needed?

	__BW_STATS_RUNTIME("Bandwidth Calculation Interval: %.2fs ", deltaTS);
	__BW_STATS_LIFETIME("Runtime Duration: %.2fs", runtimeTS);

	//subtract current from last over deltaT, update last snapshot value, compute Bytes/second without losing precision

	__BW_TRACE("grand: %'10d, global_bandwidth_statistics->interval_total_current_rx: %'d, global_bandwidth_statistics->interval_total_last_rx: '%d, delta: %'d",
			global_bandwidth_statistics->grand_total_bytes_rx,
			global_bandwidth_statistics->interval_total_current_bytes_rx,
			global_bandwidth_statistics->interval_total_last_bytes_rx,
			(global_bandwidth_statistics->interval_total_current_bytes_rx - global_bandwidth_statistics->interval_total_last_bytes_rx));

	uint32_t interval_total_bytes_delta =    (global_bandwidth_statistics->interval_total_current_bytes_rx -  global_bandwidth_statistics->interval_total_last_bytes_rx);
	uint32_t interval_total_packtets_delta = (global_bandwidth_statistics->interval_total_current_packets_rx - global_bandwidth_statistics->interval_total_last_packets_rx);
	global_bandwidth_statistics->interval_total_last_bytes_rx = global_bandwidth_statistics->interval_total_current_bytes_rx;
	global_bandwidth_statistics->interval_total_last_packets_rx = global_bandwidth_statistics->interval_total_current_packets_rx;
	uint32_t interval_total_bits_rx_s = (8 * interval_total_bytes_delta) / deltaTS;

	uint32_t interval_total_packets_rx_s = (interval_total_packtets_delta) / deltaTS;


	uint32_t interval_lls_bytes_delta =   (global_bandwidth_statistics->interval_lls_current_bytes_rx -  global_bandwidth_statistics->interval_lls_last_bytes_rx);
	uint32_t interval_lls_packets_delta = (global_bandwidth_statistics->interval_lls_current_packets_rx - global_bandwidth_statistics->interval_lls_last_packets_rx);
	uint32_t interval_lls_bits_rx_s =    (8 * interval_lls_bytes_delta ) / deltaTS;
	uint32_t interval_lls_packets_rx_s =  (interval_lls_packets_delta ) / deltaTS;
	global_bandwidth_statistics->interval_lls_last_bytes_rx =   global_bandwidth_statistics->interval_lls_current_bytes_rx;
	global_bandwidth_statistics->interval_lls_last_packets_rx = global_bandwidth_statistics->interval_lls_current_packets_rx;
	global_bandwidth_statistics->grand_lls_bytes_rx += interval_lls_bytes_delta;
	global_bandwidth_statistics->grand_lls_packets_rx += interval_lls_packets_delta;


	uint32_t interval_mmt_bytes_delta =   (global_bandwidth_statistics->interval_mmt_current_bytes_rx   - global_bandwidth_statistics->interval_mmt_last_bytes_rx);
	uint32_t interval_mmt_packets_delta = (global_bandwidth_statistics->interval_mmt_current_packets_rx - global_bandwidth_statistics->interval_mmt_last_packets_rx);
	uint32_t interval_mmt_bits_rx_s =    (8 * interval_mmt_bytes_delta ) / deltaTS;
	uint32_t interval_mmt_packets_rx_s =  (interval_mmt_packets_delta ) / deltaTS;
	global_bandwidth_statistics->interval_mmt_last_bytes_rx =   global_bandwidth_statistics->interval_mmt_current_bytes_rx;
	global_bandwidth_statistics->interval_mmt_last_packets_rx = global_bandwidth_statistics->interval_mmt_current_packets_rx;
	global_bandwidth_statistics->grand_mmt_bytes_rx +=   interval_mmt_bytes_delta;
	global_bandwidth_statistics->grand_mmt_packets_rx += interval_mmt_packets_delta;

	uint32_t interval_alc_bytes_delta =   (global_bandwidth_statistics->interval_alc_current_bytes_rx -   global_bandwidth_statistics->interval_alc_last_bytes_rx);
	uint32_t interval_alc_packets_delta = (global_bandwidth_statistics->interval_alc_current_packets_rx - global_bandwidth_statistics->interval_alc_last_packets_rx);
	uint32_t interval_alc_bits_rx_s =    (8 * interval_alc_bytes_delta ) / deltaTS;
	uint32_t interval_alc_packets_rx_s =  (interval_alc_packets_delta ) / deltaTS;
	global_bandwidth_statistics->interval_alc_last_bytes_rx = global_bandwidth_statistics->interval_alc_current_bytes_rx;
	global_bandwidth_statistics->interval_alc_last_packets_rx = global_bandwidth_statistics->interval_alc_current_packets_rx;
	global_bandwidth_statistics->grand_alc_bytes_rx += interval_alc_bytes_delta;
	global_bandwidth_statistics->grand_alc_packets_rx += interval_alc_packets_delta;

//	printf("interval_filtered_current_rx: %d", global_bandwidth_statistics->interval_filtered_current_rx);

	uint32_t interval_filtered_bytes_delta =   (global_bandwidth_statistics->interval_filtered_current_bytes_rx -  global_bandwidth_statistics->interval_filtered_last_bytes_rx);
	uint32_t interval_filtered_packets_delta = (global_bandwidth_statistics->interval_filtered_current_packets_rx - global_bandwidth_statistics->interval_filtered_last_packets_rx);
	uint32_t interval_filtered_bits_rx_s =     (8 * interval_filtered_bytes_delta ) / deltaTS;
	uint32_t interval_filtered_packets_rx_s =  (interval_filtered_packets_delta) / deltaTS;
	global_bandwidth_statistics->interval_filtered_last_bytes_rx = global_bandwidth_statistics->interval_filtered_current_bytes_rx;
	global_bandwidth_statistics->interval_filtered_last_packets_rx = global_bandwidth_statistics->interval_filtered_current_packets_rx;
	global_bandwidth_statistics->grand_filtered_bytes_rx += interval_filtered_bytes_delta;
	global_bandwidth_statistics->grand_filtered_packets_rx += interval_filtered_packets_delta;

	__BW_STATS_RUNTIME("LLS     : %'13d Kb/s, %'13d pps",	interval_lls_bits_rx_s 		 / 1024, 							interval_lls_packets_rx_s);
	__BW_STATS_RUNTIME("MMT     : %'13d Kb/s, %'13d pps",	interval_mmt_bits_rx_s      / 1024, 		interval_mmt_packets_rx_s);
	__BW_STATS_RUNTIME("ALC     : %'13d Kb/s, %'13d pps",	interval_alc_bits_rx_s      / 1024, 		interval_alc_packets_rx_s);
	__BW_STATS_RUNTIME("Filtered: %'13d Kb/s, %'13d pps",	interval_filtered_bits_rx_s / 1024,		interval_filtered_packets_rx_s);
	__BW_STATS_RUNTIME("Total   : %'13d Kb/s, %'13d pps",	interval_total_bits_rx_s    / 1024, 		interval_total_packets_rx_s);

	__BW_STATS_LIFETIME("LLS     : %'13d B, %'13d pkts",	global_bandwidth_statistics->grand_lls_bytes_rx, 		global_bandwidth_statistics->grand_lls_packets_rx);
	__BW_STATS_LIFETIME("MMT     : %'13d B, %'13d pkts",	global_bandwidth_statistics->grand_mmt_bytes_rx, 		global_bandwidth_statistics->grand_mmt_packets_rx);
	__BW_STATS_LIFETIME("ALC     : %'13d B, %'13d pkts",	global_bandwidth_statistics->grand_alc_bytes_rx, 		global_bandwidth_statistics->grand_alc_packets_rx);
	__BW_STATS_LIFETIME("Filtered: %'13d B, %'13d pkts",	global_bandwidth_statistics->grand_filtered_bytes_rx, 	global_bandwidth_statistics->grand_filtered_packets_rx);
	__BW_STATS_LIFETIME("Total   : %'13d B, %'13d pkts",	global_bandwidth_statistics->grand_total_bytes_rx,		global_bandwidth_statistics->grand_total_packets_rx);

#endif
}




