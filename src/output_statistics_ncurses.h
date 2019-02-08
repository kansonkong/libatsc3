/*
 * output_statistics_ncurses.h
 *
 *  Created on: Feb 7, 2019
 *      Author: jjustman
 *
 *      hacks:
 *
 *
  FILE *f = fopen("/dev/tty", "r+");
  SCREEN *screen = newterm(NULL, f, f);
  set_term(screen);

  //this goes to stdout
  fprintf(stdout, "hello\n");
  //this goes to the console
  fprintf(stderr, "some error\n");
  //this goes to display
  mvprintw(0, 0, "hello ncurses");
  refresh();
  getch();
  endwin();

  return 0;

  #define _DUMP_ALL_MPU_FLOWS_ true

 */

#ifndef OUTPUT_STATISTICS_NCURSES_H_
#define OUTPUT_STATISTICS_NCURSES_H_


//#if defined OUTPUT_STATISTICS && OUTPUT_STATISTICS == NCURSES
#define __BW_STATS_NCURSES true
#define __PKT_STATS_NCURSES true

#include <stdarg.h>
#include <ncurses.h>                    /* ncurses.h includes stdio.h */

SCREEN* my_screen;
//wmove(bw_window, 0, 0 __VA_ARGS__); //vwprintf(bw_window, __VA_ARGS__);
WINDOW* my_window;

WINDOW* left_window_outline;
	WINDOW* pkt_global_stats_window;

	WINDOW* bw_window_outline;
		WINDOW* bw_window_runtime;
		WINDOW* bw_window_lifetime;


WINDOW* right_window_outline;
	WINDOW* pkt_flow_stats_window;

WINDOW* bottom_window_outline;
	WINDOW* pkt_global_loss_window;




#define __BW_STATS_RUNTIME(...) 	wprintw(bw_window_runtime, __VA_ARGS__); \
									wprintw(bw_window_runtime,"\n");

#define __BW_STATS_LIFETIME(...)	wprintw(bw_window_lifetime, __VA_ARGS__); \
									wprintw(bw_window_lifetime,"\n");

#define __BW_STATS_REFRESH(...)		touchwin(bw_window_outline); \
									wrefresh(bw_window_runtime); \
									wrefresh(bw_window_lifetime);

#define __BW_CLEAR() 				werase(bw_window_runtime); \
									werase(bw_window_lifetime);

#define __PS_STATS_GLOBAL(...) 		wprintw(pkt_global_stats_window, __VA_ARGS__); \
									wprintw(pkt_global_stats_window,"\n");

#define __PS_STATS_FLOW(...) 		wprintw(pkt_flow_stats_window, __VA_ARGS__); \
									wprintw(pkt_flow_stats_window,"\n");

#define __PS_STATS_LOSS(...) 		wprintw(pkt_global_loss_window, __VA_ARGS__); \
									wprintw(pkt_global_loss_window,"\n");

#define __PS_REFRESH_LOSS() 		wrefresh(pkt_global_loss_window);


#define __PS_REFRESH() 				wrefresh(pkt_global_stats_window); \
									wrefresh(pkt_flow_stats_window);

#define __PS_CLEAR() 				werase(pkt_global_stats_window); \
									werase(pkt_flow_stats_window);

#define __PS_STATS_GLOBAL_LOSS(...) wprintw(pkt_global_loss_window, __VA_ARGS__); \
									wprintw(pkt_global_loss_window,"\n");

#define __PS_STATS_STDOUT(...)		printf(__VA_ARGS__); \
									printf("\n");




#endif /* OUTPUT_STATISTICS_NCURSES_H_ */
