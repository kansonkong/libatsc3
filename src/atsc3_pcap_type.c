/*
 * atsc3_pcap_type.c
 *
 *  Created on: Oct 9, 2019
 *      Author: jjustman
 */


#include "atsc3_pcap_type.h"

int _ATSC3_PCAP_TYPE_DEBUG_ENABLED = 1;
int _ATSC3_PCAP_TYPE_TRACE_ENABLED = 0;

/**
 *
 * either:
 * fseek(f, 0, SEEK_END); // seek to end of file
size = ftell(f); // get current file pointer
fseek(f, 0, SEEK_SET); // seek back to beginning of file
// proceed with allocating memory and reading the file
 *
 * or
 *
 * #include <sys/stat.h>
struct stat st;
stat(filename, &st);
size = st.st_size;


 */

atsc3_pcap_replay_context_t* atsc3_pcap_replay_open_filename(const char* pcap_filename) {
	atsc3_pcap_replay_context_t* atsc3_pcap_replay_context = calloc(1, sizeof(atsc3_pcap_replay_context_t));
	atsc3_pcap_replay_context->pcap_file_name = calloc(sizeof(pcap_filename+1), sizeof(char));
	strncpy(atsc3_pcap_replay_context->pcap_file_name, pcap_filename, strlen(pcap_filename));

	struct stat st;
	stat(atsc3_pcap_replay_context->pcap_file_name, &st);
	if(!st.st_size) {
		return NULL;
	}

	atsc3_pcap_replay_context->pcap_file_len = st.st_size;
	atsc3_pcap_replay_context->pcap_file_pos = 0;

	atsc3_pcap_replay_context->pcap_fp = fopen(atsc3_pcap_replay_context->pcap_file_name, "r");
	if(!atsc3_pcap_replay_context->pcap_fp) {
		return NULL;
	}

	return atsc3_pcap_replay_context;
}

atsc3_pcap_replay_context_t* atsc3_pcap_replay_iterate_packet(atsc3_pcap_replay_context_t* atsc3_pcap_replay_context_to_iterate) {
	if(atsc3_pcap_replay_context_to_iterate->pcap_file_pos + ATSC3_PCAP_MIN_GLOBAL_AND_PACKET_AND_ETH_HEADER_LENGTH > atsc3_pcap_replay_context_to_iterate->pcap_file_len) {
		return NULL;
	}

	if(!atsc3_pcap_replay_context_to_iterate->pcap_fp) {
		return NULL;
	}

	if(atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.current_pcap_packet) {
		_ATSC3_PCAP_TYPE_DEBUG("block_Destroy on packet: %p", atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.current_pcap_packet);
		block_Destroy(&atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.current_pcap_packet);
	}

	memset(&atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance, 0, sizeof(atsc3_pcap_packet_instance_t));

	//read our global header first
	if(!atsc3_pcap_replay_context_to_iterate->has_read_atsc3_pcap_global_header) {
		fread((void*)&atsc3_pcap_replay_context_to_iterate->atsc3_pcap_global_header, sizeof(atsc3_pcap_global_header_t), 1, atsc3_pcap_replay_context_to_iterate->pcap_fp);
		atsc3_pcap_replay_context_to_iterate->has_read_atsc3_pcap_global_header = true;
	}
	fread((void*)&atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.atsc3_pcap_packet_header, sizeof(atsc3_pcap_packet_header_t), 1, atsc3_pcap_replay_context_to_iterate->pcap_fp);

	atsc3_pcap_replay_context_to_iterate->last_packet_ts_sec = atsc3_pcap_replay_context_to_iterate->current_packet_ts_sec;
	atsc3_pcap_replay_context_to_iterate->last_packet_ts_usec = atsc3_pcap_replay_context_to_iterate->current_packet_ts_usec;

	//remap to host byte order?
	atsc3_pcap_replay_context_to_iterate->current_packet_ts_sec = atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.atsc3_pcap_packet_header.ts_sec;
	atsc3_pcap_replay_context_to_iterate->current_packet_ts_usec = atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.atsc3_pcap_packet_header.ts_usec;

	atsc3_pcap_replay_context_to_iterate->pcap_file_pos += sizeof(atsc3_pcap_global_header_t) + sizeof(atsc3_pcap_packet_header_t);

	atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.current_pcap_packet = block_Alloc(atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.atsc3_pcap_packet_header.incl_len);

	_ATSC3_PCAP_TYPE_DEBUG("PEEK: Reading packet: %d, size: %d, fpos is: %ld", atsc3_pcap_replay_context_to_iterate->pcap_read_packet_count, atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.atsc3_pcap_packet_header.incl_len, ftell(atsc3_pcap_replay_context_to_iterate->pcap_fp));
	fread((void*)atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.current_pcap_packet->p_buffer, atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.atsc3_pcap_packet_header.incl_len, 1, atsc3_pcap_replay_context_to_iterate->pcap_fp);

	atsc3_pcap_replay_context_to_iterate->pcap_file_pos += atsc3_pcap_replay_context_to_iterate->atsc3_pcap_packet_instance.atsc3_pcap_packet_header.incl_len;
	atsc3_pcap_replay_context_to_iterate->pcap_read_packet_count++;

	return atsc3_pcap_replay_context_to_iterate;
}


atsc3_pcap_replay_context_t* atsc3_pcap_replay_usleep_packet(atsc3_pcap_replay_context_t* atsc3_pcap_replay_context_to_iterate) {
	atsc3_pcap_replay_context_to_iterate->last_wallclock_timeval = atsc3_pcap_replay_context_to_iterate->current_wallclock_timeval;

	if(atsc3_pcap_replay_context_to_iterate->last_packet_ts_sec || atsc3_pcap_replay_context_to_iterate->last_packet_ts_usec) {

		struct timeval skew_wallclock_timeval;
		gettimeofday(&skew_wallclock_timeval, NULL);

		//compute our internal wall-clock differential for skew
		//		gettimeofday(&time_now, NULL);
		//todo: jjustman-2019-10-10
		long long delta_and_skew_wallClockUs = timediff(skew_wallclock_timeval, atsc3_pcap_replay_context_to_iterate->last_wallclock_timeval);

		struct timeval last_packet_timeval;
		last_packet_timeval.tv_sec = atsc3_pcap_replay_context_to_iterate->last_packet_ts_sec;
		last_packet_timeval.tv_usec = atsc3_pcap_replay_context_to_iterate->last_packet_ts_usec;

		struct timeval current_packet_timeval;
		current_packet_timeval.tv_sec = atsc3_pcap_replay_context_to_iterate->current_packet_ts_sec;
		current_packet_timeval.tv_usec = atsc3_pcap_replay_context_to_iterate->current_packet_ts_usec;


		long long packet_capture_ts_differentialUS = timediff(current_packet_timeval, last_packet_timeval);

		usleep(packet_capture_ts_differentialUS);

	}

	gettimeofday(&atsc3_pcap_replay_context_to_iterate->current_wallclock_timeval, NULL);
	return atsc3_pcap_replay_context_to_iterate;
}
