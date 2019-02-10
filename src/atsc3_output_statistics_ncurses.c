/*
 * atsc3_output_statistics_ncurses.c
 *
 *  Created on: Feb 7, 2019
 *      Author: jjustman
 */

#include "atsc3_output_statistics_ncurses.h"

void ncurses_init() {
	/** hacks
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
	*/
	ncurses_mutext_init();

	//wire up resize handler
	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = handle_winch;
	sigaction(SIGWINCH, &sa, NULL);


	//remap as our printf is redirected to stderr
	my_screen = newterm(NULL, stdout, stdin);
	create_or_update_window_sizes(false);

	raw();
	keypad(my_window, TRUE);		/* We get F1, F2 etc..		*/
	noecho();						/* Don't echo() while we do getch */

}

void* ncurses_input_run_thread() {
	int ch;

	while(1) {

		ch = wgetch(my_window);
		if(ch == CTRL('c')) {
			//end and clear screen back to terminal
			endwin();
			exit(1);
		}
		if(ch == KEY_F(1))		/* Without keypad enabled this will */
			printw("F1 Key pressed");/*  not get to us either	*/
						/* Without noecho() some ugly escape
						 * charachters might have been printed
						 * on screen			*/
		else
		{	printw("The pressed key is ");
			attron(A_BOLD);
			printw("%c", ch);
			attroff(A_BOLD);
		}
	}
}

void ncurses_mutext_init() {
	if (pthread_mutex_init(&ncurses_writer_lock, NULL) != 0) {
		printf("ncurses_mutex_init failed");
		abort();
	}
}
void ncurses_writer_lock_mutex_acquire() {
    pthread_mutex_lock(&ncurses_writer_lock);
}
void ncurses_writer_lock_mutex_release() {
    pthread_mutex_unlock(&ncurses_writer_lock);
}
void ncurses_writer_lock_mutex_destroy() {
    pthread_mutex_destroy(&ncurses_writer_lock);
}

void create_or_update_window_sizes(bool should_reload_term_size) {
	int rows, cols;

	if(should_reload_term_size) {
		struct winsize size;
		if (ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) {
			__NCURSES_INFO("rows: %d, cols:%d\n",  size.ws_row, size.ws_col);
			//delete sub wins
			delwin(pkt_global_loss_window);
			delwin(bottom_window_outline);
			delwin(pkt_flow_stats_window);
			delwin(right_window_outline);
			delwin(bw_window_lifetime);
			delwin(bw_window_runtime);
			delwin(bw_window_outline);
			delwin(signaling_global_stats_window);
			delwin(pkt_global_stats_window);
			delwin(left_window_outline);
			delwin(my_window);
		    clear();
		    endwin();
			resizeterm(size.ws_row, size.ws_col);
		}
	}
	my_window = newwin(0, 0, 0, 0);

	getmaxyx(my_window, rows, cols);              /* get the number of rows and columns */

	int pct_split_top = 85;

	int left_window_h = (rows * pct_split_top ) / 100;
	int left_window_w = cols/2;
	int left_window_y = 0;
	int left_window_x = 0;
	int right_window_h = (rows * pct_split_top) / 100;
	int right_window_w = cols/2 -1;
	int right_window_y = 0;
	int right_window_x = cols/2 +1;

	int bottom_window_h = rows - left_window_h - 1;
	int bottom_window_w = cols - 1;
	int bottom_window_y = left_window_h + 1;
	int bottom_window_x = 0;

	//WINDOW 					*newwin(int nlines, int ncols, int begin_y, int begin_x);
	left_window_outline = 	newwin(left_window_h, left_window_w, left_window_y, left_window_x);
	right_window_outline =	newwin(right_window_h, right_window_w, right_window_y, right_window_x);
	bottom_window_outline = newwin(bottom_window_h, bottom_window_w, bottom_window_y, bottom_window_x);

	//draw our anchors
	box(left_window_outline, 0, 0);
	box(right_window_outline, 0, 0);
	box(bottom_window_outline, 0, 0);

	char msg_global[] = "Global ATSC 3.0 Statistics";
	mvwprintw(left_window_outline, 0, (left_window_w - strlen(msg_global))/2,"%s", msg_global);

	char msg_flows[] = "Flow ATSC 3.0 Statistics";
	mvwprintw(right_window_outline, 0, right_window_w/2 - strlen(msg_flows)/2, "%s", msg_flows);

	char msg_global_lossl[] = "MMT Loss";
	mvwprintw(bottom_window_outline, 0, cols/2 - strlen(msg_global)/2,"%s", msg_global_lossl);


	//
	//WINDOW *derwin(WINDOW *orig, 							int nlines, 	int ncols, 			int begin_y, 		int begin_x);
	//left
	pkt_global_stats_window = derwin(left_window_outline, 		left_window_h-12, 44,				 1, 	1);

	//left signaling
	signaling_global_stats_window = derwin(left_window_outline, left_window_h-11, left_window_w-50, 1, 46 );

	//left
	//bandwidth window
	bw_window_outline = 		derwin(left_window_outline, 	8, 			left_window_w-2,  left_window_h-8, 	1);
	whline(bw_window_outline, ACS_HLINE, left_window_w-2);

	char msg_bandwidth[] = "RX Bandwidth Statistics";
	mvwprintw(bw_window_outline, 0, cols/4 - strlen(msg_bandwidth)/2,"%s", msg_bandwidth);

	bw_window_runtime = 		derwin(bw_window_outline, 6, (left_window_w-2)/2, 1, 1);
	bw_window_lifetime = 		derwin(bw_window_outline, 6, (left_window_w-3)/2, 1, left_window_w/2-1);

	//pkt_global_loss_window_outline = 	derwin(left_window_outline, pkt_window_height-25, half_cols-4, 22, 1);

	//RIGHT
	pkt_flow_stats_window =	derwin(right_window_outline, right_window_h-2, right_window_w-3, 1, 1);

	//bottom
	pkt_global_loss_window = 	derwin(bottom_window_outline, bottom_window_h-2, bottom_window_w-2, 1, 1);

	wrefresh(my_window);
	wrefresh(left_window_outline);
	wrefresh(right_window_outline);
	wrefresh(bottom_window_outline);

}

void handle_winch(int sig)
{
	ncurses_writer_lock_mutex_acquire();

    // Needs to be called after an endwin() so ncurses will initialize
    // itself with the new terminal dimensions.
    create_or_update_window_sizes(true);
    ncurses_writer_lock_mutex_release();

}

void* print_lls_instance_table_thread(void* lls_session_ptr) {

	lls_session_t* lls_session = (lls_session_t*)lls_session_ptr;
	while(1) {
		sleep(1);
		if(lls_session->lls_table_slt) {
			ncurses_writer_lock_mutex_acquire();
			__LLS_DUMP_CLEAR();
			__LLS_REFRESH();
			lls_dump_instance_table_ncurses(lls_session->lls_table_slt);
			__DOUPDATE();
			__LLS_REFRESH();
			ncurses_writer_lock_mutex_release();
		}
	}

}

void lls_dump_instance_table_ncurses(lls_table_t* base_table) {

	__LLS_DUMP("LLS Base Table:");
	__LLS_DUMP("");
	__LLS_DUMP("lls_table_id       : %-3d (0x%-3x)  group_id      : %-3d (0x%-3x)", base_table->lls_table_id,	base_table->lls_table_id, base_table->lls_group_id, base_table->lls_group_id);
	__LLS_DUMP("group_count_minus1 : %-3d (0x%-3x)  table_version : %-3d (0x%-3x)", base_table->group_count_minus1, base_table->group_count_minus1, base_table->lls_table_version, base_table->lls_table_version);
	__LLS_DUMP("");

	if(base_table->lls_table_id == SLT) {

		__LLS_DUMP("SLT: Service contains %d entries:", base_table->slt_table.service_entry_n);

		for(int i=0l; i < base_table->slt_table.service_entry_n; i++) {
			lls_service_t* service = base_table->slt_table.service_entry[i];
			__LLS_DUMP("  service_id                  : %d", service->service_id);
			__LLS_DUMP("  global_service_id           : %s", service->global_service_id);
			__LLS_DUMP("  major_channel_no            : %-5d     minor_channel_no        : %d", service->major_channel_no, service->minor_channel_no);
			__LLS_DUMP("  service_category            : %-5d     slt_svc_seq_num         : %d", service->service_category, service->slt_svc_seq_num);
			__LLS_DUMP("  short_service_name          : %s", service->short_service_name);
			__LLS_DUMP("  broadcast_svc_signaling");
			__LLS_DUMP("    sls_protocol              : %d", service->broadcast_svc_signaling.sls_protocol);
			__LLS_DUMP("    sls_destination_ip_address: %s:%s", service->broadcast_svc_signaling.sls_destination_ip_address, service->broadcast_svc_signaling.sls_destination_udp_port);
			__LLS_DUMP("    sls_source_ip_address     : %s", service->broadcast_svc_signaling.sls_source_ip_address);
			__LLS_DUMP("");
		}
	}

	//decorate with instance types: hd = int16_t, hu = uint_16t, hhu = uint8_t
	if(base_table->lls_table_id == SystemTime) {
	__LLS_DUMP(" SystemTime:");
	__LLS_DUMP("  current_utc_offset       : %hd", base_table->system_time_table.current_utc_offset);
	__LLS_DUMP("  ptp_prepend              : %hu", base_table->system_time_table.ptp_prepend);
	__LLS_DUMP("  leap59                   : %d",  base_table->system_time_table.leap59);
	__LLS_DUMP("  leap61                   : %d",  base_table->system_time_table.leap61);
	__LLS_DUMP("  utc_local_offset         : %s",  base_table->system_time_table.utc_local_offset);

	__LLS_DUMP("  ds_status                : %d",  base_table->system_time_table.ds_status);
	__LLS_DUMP("  ds_day_of_month          : %hhu", base_table->system_time_table.ds_day_of_month);
	__LLS_DUMP("  ds_hour                  : %hhu", base_table->system_time_table.ds_hour);

	}

}




