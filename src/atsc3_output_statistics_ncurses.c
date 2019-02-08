/*
 * atsc3_output_statistics_ncurses.c
 *
 *  Created on: Feb 7, 2019
 *      Author: jjustman
 */

#include "atsc3_output_statistics_ncurses.h"

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
