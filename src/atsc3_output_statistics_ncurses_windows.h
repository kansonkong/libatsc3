/*
 * atsc3_output_statistics_ncurses_windows.h
 *
 *  Created on: Feb 25, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_OUTPUT_STATISTICS_NCURSES_WINDOWS_H_
#define ATSC3_OUTPUT_STATISTICS_NCURSES_WINDOWS_H_


SCREEN* my_screen;
//wmove(bw_window, 0, 0 __VA_ARGS__); //vwprintf(bw_window, __VA_ARGS__);
WINDOW* my_window;

WINDOW* left_window_outline;
	WINDOW* pkt_global_stats_window;
	WINDOW* signaling_global_stats_window;

	WINDOW* bw_window_outline;
		WINDOW* bw_window_runtime;
		WINDOW* bw_window_lifetime;


WINDOW* right_window_outline;
	WINDOW* pkt_flow_stats_mmt_window;
    WINDOW* pkt_flow_stats_route_window;



WINDOW* bottom_window_outline;
	WINDOW* pkt_global_loss_window;

//used in output_statistics_mfu

WINDOW* pkt_flow_stats_mmt_log_outline_window;
WINDOW* pkt_flow_stats_mmt_log_window;


#endif /* ATSC3_OUTPUT_STATISTICS_NCURSES_WINDOWS_H_ */
