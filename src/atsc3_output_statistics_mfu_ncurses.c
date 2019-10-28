/*
 * atsc3_output_statistics_mfu_ncurses.c
 *
 *  Created on: Mar 25, 2019
 *      Author: jjustman
 */

#ifndef __DISABLE_NCURSES__
#include <ncurses.h>
#endif

#include "atsc3_output_statistics_mfu_ncurses.h"
#include "atsc3_player_ffplay.h"
#include "atsc3_alc_utils.h"
#include "atsc3_output_statistics_ncurses_windows.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"

pthread_mutex_t ncurses_writer_lock;
int initfunc(WINDOW* ripoff_win, int cols) {
	//printf("got my_window: %p", ripoff_win);
	my_window = ripoff_win;
	return 0;
}
extern void trace (const unsigned int);

#define BRIGHT_WHITE  15

void ncurses_init() {
#ifndef __DISABLE_NCURSES__

	ncurses_mutext_init();
	def_prog_mode();
	//wire up resize handler
	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = handle_winch;
	sigaction(SIGWINCH, &sa, NULL);

	//rip off top line
	ripoffline(1,initfunc);
	
	
	//remap as our printf is redirected to stderr
	char* my_term = (char *)"xterm";
	my_screen = newterm(my_term, stdout, stdin);

	set_term(my_screen);
	raw();

	noecho();
	
	curs_set(0);
	start_color();
	use_default_colors();

	init_pair(2, COLOR_RED, COLOR_WHITE);
	init_pair(3, COLOR_YELLOW, COLOR_WHITE);
	init_pair(4, COLOR_BLACK, COLOR_GREEN);


	create_or_update_window_sizes(false);
	//clearok(curscr, false);
	//scrollok(curscr, false);
#endif
}

int play_mode = 0;
uint32_t my_service_id = 0;
uint32_t my_route_tsi = 0;
uint32_t my_route_toi_init_fragment = 0;

void mtl_clear() {
#ifndef __DISABLE_NCURSES__

	wmove(my_window, 0, 1);
	wclrtoeol(my_window);
	wmove(my_window, 0, 1);
#endif
}

#ifndef __DISABLE_NCURSES__
void* ncurses_input_run_thread(void* lls_slt_monitor_ptr) {

	int ch;
    ncurses_init();
    lls_slt_monitor_t* lls_slt_monitor = (lls_slt_monitor_t*)lls_slt_monitor_ptr;
    lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;
    lls_sls_alc_monitor* lls_sls_alc_monitor = NULL;


    while(1) {

      ch = wgetch(curscr);
		if(ch == CTRL('c') || ch == 'q') {
			//end and clear screen back to terminal
			goto endwin;
		}
        
        if(ch == 'm') {
            if(lls_slt_monitor && lls_slt_monitor->lls_sls_alc_monitor) {
                lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled = false;
            }

            mtl_clear();
            wprintw(my_window, "Switching to MMT Capture Mode, press 's' to select Service ID, 'p' to play, 'x' to return to normal flow monitoring");
            play_mode = 1;

            while(1) {
                char mmt_input_str[16];
                
                ch = wgetch(my_window);
                //fallthru to play down below
                if(ch == 'p') {
                    break;
                }
                
                if(ch == CTRL('c') || ch == 'q') {
                    goto endwin;
                } else if (ch == 'x') {
                    play_mode = 0;
                    mtl_clear();
                    wprintw(my_window, "Exiting MMT");                
                }
                if(ch == 'v') {
                	if(lls_sls_mmt_monitor) {
						mtl_clear();
						wprintw(my_window, "Please enter video packet_id: ");
						echo();
						//wgetstr(my_window, str);
						mvwgetnstr(my_window, 0, 32, mmt_input_str, 10);
						noecho();
						mtl_clear();

						long video_packet_id_l = strtol(mmt_input_str, NULL, 0);
						lls_sls_mmt_monitor->video_packet_id = (uint32_t) video_packet_id_l;
						mtl_clear();
						wprintw(my_window, "Monitoring video packet_id: %u",  lls_sls_mmt_monitor->video_packet_id);
                	} else {
						mtl_clear();
						wprintw(my_window, "Please start with service id selection 's' ");
						echo();
					}
                }
                if(ch == 'a') {
					if(lls_sls_mmt_monitor) {
						mtl_clear();
						wprintw(my_window, "Please enter audio packet_id: ");
						echo();
						//wgetstr(my_window, str);
						mvwgetnstr(my_window, 0, 32, mmt_input_str, 10);
						noecho();
						mtl_clear();

						long audio_packet_id_l = strtol(mmt_input_str, NULL, 0);
						lls_sls_mmt_monitor->audio_packet_id = (uint32_t) audio_packet_id_l;
						mtl_clear();
						wprintw(my_window, "Monitoring audio packet_id: %u",  lls_sls_mmt_monitor->audio_packet_id);
					} else {
						mtl_clear();
						wprintw(my_window, "Please start with service id selection 's' ");
						echo();
					}
				}
                if(ch == 's') {
                    mtl_clear();
                    wprintw(my_window, "Please enter Service ID: ");
                    echo();
                    //wgetstr(my_window, str);
                    mvwgetnstr(my_window, 0, 32, mmt_input_str, 10);
                    noecho();
                    mtl_clear();
                    
                    long my_tsi_long = strtol(mmt_input_str, NULL, 0);
                    my_service_id = (uint32_t) my_tsi_long;
                    mtl_clear();
                    
                    //find our matching lls_sls and create a monitor entry
                    
                    lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, my_service_id);
                    if(lls_sls_mmt_session) {
                        //TODO - free and teardown if we already have an active montiro

                        //build our alc_session map
                        
                        lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();
                        lls_sls_mmt_monitor->lls_mmt_session = lls_sls_mmt_session;
                        lls_sls_mmt_monitor->atsc3_lls_slt_service = lls_sls_mmt_session->atsc3_lls_slt_service;

                        lls_sls_mmt_monitor->video_packet_id = lls_sls_mmt_session->video_packet_id;
                        lls_sls_mmt_monitor->audio_packet_id = lls_sls_mmt_session->audio_packet_id;

                        lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.has_written_init_box = false;
                        lls_slt_monitor->lls_sls_mmt_monitor = lls_sls_mmt_monitor;
                        
                        wprintw(my_window, "Monitoring Service ID: %u, video packet_id: %u, audio packet_id: %u",  my_service_id, lls_sls_mmt_monitor->video_packet_id, lls_sls_mmt_monitor->audio_packet_id);
                    }
                }
            }
        }



        //cheat on fallthru on 'p'
        if(ch == 'p') {
            //play stream...
            if(play_mode == 1) {
                mtl_clear();
                if(lls_slt_monitor->lls_sls_mmt_monitor) {
                    wprintw(my_window, "MMT: Starting playback for service_id: %u, video packet_id: %u, audio packet_id: %u",
                    		lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id,
							lls_slt_monitor->lls_sls_mmt_monitor->video_packet_id,
							lls_slt_monitor->lls_sls_mmt_monitor->audio_packet_id);
                    
                    lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer = pipe_create_ffplay_resolve_fps(&lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff);
                    
                    lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled = true;
                    
                    lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer = (http_output_buffer_t*)calloc(1, sizeof(http_output_buffer_t));
                    lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_buffer->http_payload_buffer_mutex = lls_sls_monitor_reader_mutext_create();
                    lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.http_output_enabled = true;

                } else if(lls_slt_monitor->lls_sls_alc_monitor) {
                    wprintw(my_window, "ROUTE/DASH: Starting playback for service_id: %u, video_tsi: %u, audio_tsi: %u",
                    		lls_slt_monitor->lls_sls_alc_monitor->atsc3_lls_slt_service->service_id,
							lls_slt_monitor->lls_sls_alc_monitor->video_tsi, lls_slt_monitor->lls_sls_alc_monitor->audio_tsi);
                    lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.pipe_ffplay_buffer = pipe_create_ffplay_resolve_fps(&lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff);
                    
                    lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.ffplay_output_enabled = true;
                }
                wrefresh(my_window);
            } else {
                mtl_clear();
                wprintw(my_window, "No monitored MMT or ROUTE Service ID");
                
                __NCURSES_WARN("not playing - play mode is: %d", play_mode);
            }
        }
        
        if(ch == 'd') {
            //set mode to dump
            if(lls_slt_monitor->lls_sls_mmt_monitor) {
                if(!lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
                    lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;
                    
                      wprintw(my_window, "MMT: Starting dump for service_id: %u, video packet_id: %u, audio packet_id: %u",
                    		  lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id, lls_slt_monitor->lls_sls_mmt_monitor->video_packet_id, lls_slt_monitor->lls_sls_mmt_monitor->audio_packet_id);
                    
                } else {
                    lls_slt_monitor->lls_sls_mmt_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled = false;
                      wprintw(my_window, "MMT: Ending dump for service_id: %u, video packet_id: %u, audio packet_id: %u",
                    		  lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id, lls_slt_monitor->lls_sls_mmt_monitor->video_packet_id, lls_slt_monitor->lls_sls_mmt_monitor->audio_packet_id);
                }
              
            } else if(lls_slt_monitor->lls_sls_alc_monitor) {
                if(!lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
                    lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;
                     wprintw(my_window, "ROUTE/DASH: Starting dump for service_id: %u, video_tsi: %u, audio_tsi: %u",
                    		 lls_slt_monitor->lls_sls_alc_monitor->atsc3_lls_slt_service->service_id, lls_slt_monitor->lls_sls_alc_monitor->video_tsi, lls_slt_monitor->lls_sls_alc_monitor->audio_tsi);
                } else {
                    lls_slt_monitor->lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled = true;
                     wprintw(my_window, "ROUTE/DASH: Ending dump for service_id: %u, video_tsi: %u, audio_tsi: %u",
                    		 lls_slt_monitor->lls_sls_alc_monitor->atsc3_lls_slt_service->service_id, lls_slt_monitor->lls_sls_alc_monitor->video_tsi, lls_slt_monitor->lls_sls_alc_monitor->audio_tsi);

                }
            }
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


endwin:
	endwin();
	exit(1);

	return NULL;
}
#endif


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

WINDOW* signaling_global_stats_window_outline;

void create_or_update_window_sizes(bool should_reload_term_size) {
#ifndef __DISABLE_NCURSES__

	int rows, cols;

	if(should_reload_term_size) {
		struct winsize size;
		if (ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) {
			__NCURSES_INFO("rows: %d, cols:%d\n",  size.ws_row, size.ws_col);
			//delete sub wins
			delwin(pkt_global_loss_window);
			delwin(bottom_window_outline);
			delwin(pkt_flow_stats_mmt_window);
			delwin(right_window_outline);
			delwin(bw_window_lifetime);
			delwin(bw_window_runtime);
			delwin(bw_window_outline);
			delwin(signaling_global_stats_window);
			delwin(pkt_global_stats_window);
			delwin(left_window_outline);
			//			delwin(my_window);
			clear();
			//endwin();
			resizeterm(size.ws_row, size.ws_col);
		}
	}
	//my_window = newwin(0, 0, 0, 0);
	getmaxyx(curscr, rows, cols);              /* get the number of rows and columns */
	rows -= 1; //move up as my_window is a tear-off for input

	int pct_split_top = 100;

	int left_window_h = (rows * pct_split_top ) / 100 - 1;
	int left_window_w = cols/2;
	int left_window_y = 0;
	int left_window_x = 0;
	int right_window_h = (rows * pct_split_top) / 100 - 1 ;
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

	//draw our anchors
	box(left_window_outline, 0, 0);
	box(right_window_outline, 0, 0);

	char msg_flows[] = "MMT MFU Flow Statistics";
	char msg_global[] = "Global ATSC 3.0 Statistics";
	mvwprintw(right_window_outline, 0, right_window_w/2 - strlen(msg_flows)/2, "%s", msg_global);
	mvwprintw(left_window_outline, 0, (left_window_w - strlen(msg_global))/2,"%s", msg_flows);

	//
	//WINDOW *derwin(WINDOW *orig, 							int nlines, 	int ncols, 			int begin_y, 		int begin_x);
	//left
	pkt_flow_stats_mmt_window = derwin(left_window_outline, 		left_window_h-12, left_window_w-3,				 1, 	1);
	scrollok(pkt_flow_stats_mmt_window, false);

	pkt_flow_stats_mmt_log_outline_window = derwin(left_window_outline, 			11, left_window_w-3,				 left_window_h-11, 	1);
	whline(pkt_flow_stats_mmt_log_outline_window, ACS_HLINE, left_window_w-2);

	char msg_mpu_log[] = "MPU Log";
	mvwprintw(pkt_flow_stats_mmt_log_outline_window, 0, cols/4 - strlen(msg_mpu_log)/2,"%s", msg_mpu_log);

	pkt_flow_stats_mmt_log_window = derwin(pkt_flow_stats_mmt_log_outline_window, 		9, left_window_w-10,				 1, 	1);
	scrollok(pkt_flow_stats_mmt_log_window, true);

	//pkt_global_loss_window_outline = 	derwin(left_window_outline, pkt_window_height-25, half_cols-4, 22, 1);

	//RIGHT
	int packet_stats_window_height = 18;

	pkt_global_stats_window =	derwin(right_window_outline, 	packet_stats_window_height, 					right_window_w-3, 							1, 1);
	scrollok(pkt_global_stats_window, false);


	//RIGHT signaling
	signaling_global_stats_window_outline = derwin(right_window_outline, 		 left_window_h-packet_stats_window_height-10, right_window_w-3, packet_stats_window_height+1, 1 );
	whline(signaling_global_stats_window_outline, ACS_HLINE, left_window_w-2);
	signaling_global_stats_window = derwin(signaling_global_stats_window_outline, left_window_h-packet_stats_window_height-11, right_window_w-3, 1, 0);
	scrollok(signaling_global_stats_window, false);


	//bandwidth window
	bw_window_outline = 		derwin(right_window_outline, 	8, 			left_window_w-2,  left_window_h-8, 	1);
	whline(bw_window_outline, ACS_HLINE, left_window_w-2);

	char msg_bandwidth[] = "RX Bandwidth Statistics";
	mvwprintw(bw_window_outline, 0, cols/4 - strlen(msg_bandwidth)/2,"%s", msg_bandwidth);

	bw_window_runtime = 		derwin(bw_window_outline, 6, (left_window_w-2)/2, 1, 1);
	bw_window_lifetime = 		derwin(bw_window_outline, 6, (left_window_w-3)/2, 1, left_window_w/2-1);
	scrollok(bw_window_runtime, false);
	scrollok(bw_window_lifetime, false);

	//wnoutrefresh(curscr);
    wnoutrefresh(left_window_outline);
	wnoutrefresh(right_window_outline);
	wnoutrefresh(bottom_window_outline);
	//immedok(curscr, false);
	immedok(left_window_outline, false);
	immedok(right_window_outline, false);
	immedok(bottom_window_outline, false);
	immedok(left_window_outline, false);
	immedok(right_window_outline, false);
	immedok(bottom_window_outline, false);
	//doupdate();

#endif
}

void handle_winch(int sig)
{
	ncurses_writer_lock_mutex_acquire();

    // Needs to be called after an endwin() so ncurses will initialize
    // itself with the new terminal dimensions.
    create_or_update_window_sizes(true);
    ncurses_writer_lock_mutex_release();

}


void handle_sighup(int sig)
{
    //noop?
    __NCURSES_INFO("got sighup at: %u", sig);
    
}

void* print_lls_instance_table_thread(void* lls_slt_monitor_ptr) {

	lls_slt_monitor_t* lls_slt_monitor = (lls_slt_monitor_t*)lls_slt_monitor_ptr;

	while(1) {
		sleep(1);
		if(lls_slt_monitor->lls_latest_slt_table) {
			ncurses_writer_lock_mutex_acquire();

			//clear our window so we aren't appending, otherwise it will look as if we are leaking slt
			__LLS_DUMP_CLEAR();
			lls_dump_instance_table_mmt_only_ncurses(lls_slt_monitor->lls_latest_slt_table);
			__DOUPDATE();
			__LLS_REFRESH();
	       
			ncurses_writer_lock_mutex_release();
		}
	}

}

void lls_dump_instance_table_ncurses(lls_table_t* base_table) {

	__LLS_DUMP("LLS Base Table:");
	__LLS_DUMP("");
	__LLS_DUMP("lls_table_id       : %-3d (0x%-3x)     group_id      : %-3d (0x%-3x)", base_table->lls_table_id,	base_table->lls_table_id, base_table->lls_group_id, base_table->lls_group_id);
	__LLS_DUMP("group_count_minus1 : %-3d (0x%-3x)     table_version : %-3d (0x%-3x)", base_table->group_count_minus1, base_table->group_count_minus1, base_table->lls_table_version, base_table->lls_table_version);
	__LLS_DUMP("");

	if(base_table->lls_table_id == SLT) {

		__LLS_DUMP("SLT: Service contains %d entries:", base_table->slt_table.atsc3_lls_slt_service_v.count);

		for(int i=0; i < base_table->slt_table.atsc3_lls_slt_service_v.count; i++) {
			atsc3_lls_slt_service_t* service = base_table->slt_table.atsc3_lls_slt_service_v.data[i];
			__LLS_DUMP("service_id         : %-5d           global_service_id : %s", service->service_id, service->global_service_id);
			__LLS_DUMP("major_channel_no   : %-5d           minor_channel_no  : %d", service->major_channel_no, service->minor_channel_no);
			__LLS_DUMP("service_category   : %1d, %-8s    slt_svc_seq_num   : %d", service->service_category, lls_get_service_category_value(service->service_category), service->slt_svc_seq_num);
			__LLS_DUMP("short_service_name : %s", service->short_service_name);
			if(service->atsc3_slt_broadcast_svc_signalling_v.count) {
				__LLS_DUMP(" broadcast_svc_signaling");
				__LLS_DUMP("  sls_protocol               : %d, %s", service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol, lls_get_sls_protocol_value(service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol));
				__LLS_DUMP("  sls_destination_ip_address : %s:%s", service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_destination_ip_address, service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_destination_udp_port);
				__LLS_DUMP("  sls_source_ip_address      : %s", service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_source_ip_address);
			}
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



void lls_dump_instance_table_mmt_only_ncurses(lls_table_t* base_table) {

	__LLS_DUMP("LLS Base Table:");
	if(base_table->lls_table_id == SLT) {

		__LLS_DUMP("SLT: Service contains %d entries: (only showing MMT)", base_table->slt_table.atsc3_lls_slt_service_v.count);

		for(int i=0l; i < base_table->slt_table.atsc3_lls_slt_service_v.count; i++) {
			atsc3_lls_slt_service_t* service = base_table->slt_table.atsc3_lls_slt_service_v.data[i];

			if(service->atsc3_slt_broadcast_svc_signalling_v.count && service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol == 0x2) {
				__LLS_DUMP("service_id         : %-5d           global_service_id : %s", service->service_id, service->global_service_id);
				__LLS_DUMP("major_channel_no   : %-5d           minor_channel_no  : %d", service->major_channel_no, service->minor_channel_no);
				__LLS_DUMP("service_category   : %1d, %-8s    slt_svc_seq_num   : %d", service->service_category, lls_get_service_category_value(service->service_category), service->slt_svc_seq_num);
				__LLS_DUMP("short_service_name : %s", service->short_service_name);

				__LLS_DUMP(" broadcast_svc_signaling");
				__LLS_DUMP("  sls_protocol               : %d, %s", service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol,
																		lls_get_sls_protocol_value(service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol));
				__LLS_DUMP("  sls_destination_ip_address : %s:%s", service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_destination_ip_address, service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_destination_udp_port);
				__LLS_DUMP("  sls_source_ip_address      : %s", service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_source_ip_address);
				__LLS_DUMP("");
			}
		}
	}

}




