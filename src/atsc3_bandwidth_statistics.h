/*
 * atsc3_bandwidth_statistics.h
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <unistd.h>
#include <locale.h>

#include "atsc3_output_statistics_ncurses.h"
#include "atsc3_utils.h"


#ifndef ATSC3_BANDWIDTH_STATISTICS_H_
#define ATSC3_BANDWIDTH_STATISTICS_H_

#ifndef __BW_STATS_NCURSES
#define __BW_STATS(...)   printf("%s:%d: ","bw_stats",__LINE__);__PRINTLN(__VA_ARGS__);
#define __BW_STATS_RUNTIME(...)  printf("%s:%d: ","bw_stats",__LINE__);
#define __BW_STATS_L(...)  __PRINTLN(__VA_ARGS__);

#define __BW_STATS_BORDER(...) __BW_STATS(__VA_ARGS__)
#define __BW_STATS_REFRESH()
#define __BW_CLEAR()
#endif

#define __BW_TRACE(...)   //printf("%s:%d:TRACE: ",__FILE__,__LINE__);__PRINTLN(__VA_ARGS__);

typedef struct bandwith_statistics {
	//using sig_atomic_t so parser thread doesn't need to be synchronized for locks for updating the current value
	//only the collation thread should touch grand_total_rx

	//TODO - move all of this and the computation logic to macros...
	sig_atomic_t	interval_lls_current_bytes_rx;
	sig_atomic_t	interval_lls_current_packets_rx;
	uint32_t		interval_lls_last_bytes_rx;
	uint32_t		interval_lls_last_packets_rx;
	uint32_t		grand_lls_bytes_rx;
	uint32_t		grand_lls_packets_rx;

	sig_atomic_t	interval_mmt_current_bytes_rx;
	sig_atomic_t	interval_mmt_current_packets_rx;
	uint32_t		interval_mmt_last_bytes_rx;
	uint32_t		interval_mmt_last_packets_rx;
	uint32_t		grand_mmt_bytes_rx;
	uint32_t		grand_mmt_packets_rx;

	sig_atomic_t	interval_alc_current_bytes_rx;
	sig_atomic_t	interval_alc_current_packets_rx;
	uint32_t		interval_alc_last_bytes_rx;
	uint32_t		interval_alc_last_packets_rx;
	uint32_t		grand_alc_bytes_rx;
	uint32_t		grand_alc_packets_rx;

	sig_atomic_t	interval_filtered_current_bytes_rx;
	sig_atomic_t	interval_filtered_current_packets_rx;
	uint32_t		interval_filtered_last_bytes_rx;
	uint32_t		interval_filtered_last_packets_rx;
	uint32_t		grand_filtered_bytes_rx;
	uint32_t		grand_filtered_packets_rx;

	sig_atomic_t	interval_total_current_bytes_rx;
	sig_atomic_t	interval_total_current_packets_rx;
	uint32_t		interval_total_last_bytes_rx;
	uint32_t		interval_total_last_packets_rx;
	uint32_t		grand_total_bytes_rx;
	uint32_t		grand_total_packets_rx;

	struct timeval 	snapshot_timeval_start;
	struct timeval 	program_timeval_start;
} bandwidth_statistics_t;


#if defined (__cplusplus)
extern "C" {
#endif

extern bandwidth_statistics_t *global_bandwidth_statistics;
void *print_bandwidth_statistics_thread(void *vargp);

#if defined (__cplusplus)
}
#endif

#endif /* ATSC3_BANDWIDTH_STATISTICS_H_ */
