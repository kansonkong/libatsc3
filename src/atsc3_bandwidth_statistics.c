/*
 * atsc3_bandwidth_statistics.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#include "atsc3_output_statistics_ncurses.h"
#include "atsc3_bandwidth_statistics.h"

void doBandwidthStatusUpdate();

void *printBandwidthStatistics(void *vargp)
{
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
}

void doBandwidthStatusUpdate() {
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

	__BW_STATS_RUNTIME("Bandwidth Stats over: %.2fs ", deltaTS);
	__BW_STATS_LIFETIME("Runtime: %.2fs", runtimeTS);

	//subtract current from last over deltaT, update last snapshot value, compute Bytes/second without losing precision

	__BW_TRACE("grand: %'10d, global_bandwidth_statistics->interval_total_current_rx: %'d, global_bandwidth_statistics->interval_total_last_rx: '%d, delta: %'d",
			global_bandwidth_statistics->grand_total_rx,
			global_bandwidth_statistics->interval_total_current_rx,
			global_bandwidth_statistics->interval_total_last_rx,
			(global_bandwidth_statistics->interval_total_current_rx - global_bandwidth_statistics->interval_total_last_rx));

	uint32_t interval_total_delta = (global_bandwidth_statistics->interval_total_current_rx - global_bandwidth_statistics->interval_total_last_rx);
	uint32_t interval_total_rx_s = (interval_total_delta * 1000000) / deltaTuS;
	global_bandwidth_statistics->interval_total_last_rx = global_bandwidth_statistics->interval_total_current_rx;
	global_bandwidth_statistics->grand_total_rx += interval_total_delta;

	uint32_t interval_lls_delta = (global_bandwidth_statistics->interval_lls_current_rx - global_bandwidth_statistics->interval_lls_last_rx);
	uint32_t interval_lls_rx_s = (interval_lls_delta * 1000000) / deltaTuS;
	global_bandwidth_statistics->interval_lls_last_rx = global_bandwidth_statistics->interval_lls_current_rx;
	global_bandwidth_statistics->grand_lls_rx += interval_lls_delta;

	uint32_t interval_mmt_delta = (global_bandwidth_statistics->interval_mmt_current_rx - global_bandwidth_statistics->interval_mmt_last_rx);
	uint32_t interval_mmt_rx_s = (interval_mmt_delta * 1000000) / deltaTuS;
	global_bandwidth_statistics->interval_mmt_last_rx = global_bandwidth_statistics->interval_mmt_current_rx;
	global_bandwidth_statistics->grand_mmt_rx += interval_mmt_delta;

	uint32_t interval_alc_delta = (global_bandwidth_statistics->interval_alc_current_rx - global_bandwidth_statistics->interval_alc_last_rx);
	uint32_t interval_alc_rx_s = (interval_alc_delta * 1000000) / deltaTuS;
	global_bandwidth_statistics->interval_alc_last_rx = global_bandwidth_statistics->interval_alc_current_rx;
	global_bandwidth_statistics->grand_alc_rx += interval_alc_delta;

	uint32_t interval_filtered_delta = (global_bandwidth_statistics->interval_filtered_current_rx - global_bandwidth_statistics->interval_filtered_last_rx);
	uint32_t interval_filtered_rx_s = (interval_filtered_delta * 1000000) / deltaTuS;
	global_bandwidth_statistics->interval_filtered_last_rx = global_bandwidth_statistics->interval_filtered_current_rx;
	global_bandwidth_statistics->grand_filtered_rx += interval_filtered_delta;

	__BW_STATS_RUNTIME("LLS     : %'8d  b/s, %'13d b",   interval_lls_delta, 						interval_lls_rx_s);
	__BW_STATS_RUNTIME("MMT     : %'8d Kb/s, %'13d Kb",  (interval_mmt_delta * 8) / 1024.0, 		(interval_mmt_rx_s * 8) / 1024.0);
	__BW_STATS_RUNTIME("ALC     : %'8d Kb/s, %'13d Kb",  (interval_alc_delta * 8) / 1024.0, 		(interval_alc_rx_s* 8) / 1024.0);
	__BW_STATS_RUNTIME("Filtered: %'8d Kb/s, %'13d KB",  (interval_filtered_delta * 8) / 1024.0,	(interval_filtered_rx_s * 8) / 1024.0);
	__BW_STATS_RUNTIME("Total   : %'8d Kb/s, %'13d KB",  (interval_total_delta * 8) / 1024.0, 		(interval_total_rx_s * 8) / 1024.0);

	__BW_STATS_LIFETIME("LLS     : %'13d B", 			 global_bandwidth_statistics->grand_lls_rx);
	__BW_STATS_LIFETIME("MMT     : %'13d B", 			 global_bandwidth_statistics->grand_mmt_rx);
	__BW_STATS_LIFETIME("ALC     : %'13d B", 			 global_bandwidth_statistics->grand_alc_rx);
	__BW_STATS_LIFETIME("Filtered: %'13d B",			 global_bandwidth_statistics->grand_filtered_rx);
	__BW_STATS_LIFETIME("Total   : %'13d B", 			 global_bandwidth_statistics->grand_total_rx);

}




