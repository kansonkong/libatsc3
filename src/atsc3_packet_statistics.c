/*
 * atsc3_mmt_packet_statistics.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#include "atsc3_listener_udp.h"
#include "atsc3_packet_statistics.h"

int global_mmt_loss_count;
bool __LOSS_DISPLAY_ENABLED = true;

extern atsc3_global_statistics_t* atsc3_global_statistics;

void *print_global_statistics_thread(void *vargp)
{
#ifndef __DISABLE_NCURSES__

    __PS_TRACE("Starting printGlobalStatistics");
	setlocale(LC_ALL,"");
	while(true) {
		sleep(1);
		ncurses_writer_lock_mutex_acquire();
		__PS_STATS_NOUPDATE();
		atsc3_packet_statistics_dump_global_stats();
		__DOUPDATE();
		ncurses_writer_lock_mutex_release();
	}
#endif

	return NULL;
}


void *print_mfu_statistics_thread(void *vargp)
{
#ifndef __DISABLE_NCURSES__

    __PS_TRACE("Starting print_mfu_statistics_thread");
	__LOSS_DISPLAY_ENABLED = false;
	setlocale(LC_ALL,"");
	while(true) {
		usleep(50000);
		ncurses_writer_lock_mutex_acquire();
		__PS_STATS_NOUPDATE();
		atsc3_packet_statistics_dump_mfu_stats();
		__DOUPDATE_MFU();
		ncurses_writer_lock_mutex_release();
	}
#endif
	return NULL;
}

int comparator_packet_id_mmt_stats_t(const void *a, const void *b)
{
	__PS_TRACE("comparator_packet_id_mmt_stats_t with %u from %u", ((packet_id_mmt_stats_t *)a)->packet_id, ((packet_id_mmt_stats_t *)b)->packet_id);

	if ( ((packet_id_mmt_stats_t*)a)->packet_id <  ((packet_id_mmt_stats_t*)b)->packet_id ) return -1;
	if ( ((packet_id_mmt_stats_t*)a)->packet_id == ((packet_id_mmt_stats_t*)b)->packet_id ) return  0;
	if ( ((packet_id_mmt_stats_t*)a)->packet_id >  ((packet_id_mmt_stats_t*)b)->packet_id ) return  1;

	return 0;
}

packet_flow_t* find_packet_flow(uint32_t ip, uint16_t port) {
	for(int i=0; i < atsc3_global_statistics->packet_flow_n; i++ ) {
		packet_flow_t* packet_flow = atsc3_global_statistics->packet_flow_vector[i];
		__PS_TRACE("  find_packet_flow with ip: %u, port: %u", ip, port);

		if(packet_flow->ip == ip && packet_flow->port == port) {
			__PS_TRACE("  find_packet_flow returning with %p", packet_flow);

			return packet_flow;
		}
	}
	return NULL;
}

packet_id_mmt_stats_t* find_packet_id(uint32_t ip, uint16_t port, uint32_t packet_id) {
	for(int i=0; i < atsc3_global_statistics->packet_id_n; i++ ) {
		packet_id_mmt_stats_t* packet_mmt_stats = atsc3_global_statistics->packet_id_vector[i];
		__PS_TRACE("  find_packet_id with ip: %u, port: %u, %u", ip, port, packet_id, packet_mmt_stats->packet_id);

		if(packet_mmt_stats->ip == ip && packet_mmt_stats->port == port && packet_mmt_stats->packet_id == packet_id) {
			__PS_TRACE("  find_packet_id returning with %p", packet_mmt_stats);

			return packet_mmt_stats;
		}
	}

	return NULL;
}
/**
 * todo - refactor out the reference to global_stats
 *
 */

packet_id_mmt_stats_t* find_or_create_packet_id(uint32_t ip, uint16_t port, uint32_t packet_id) {
	packet_id_mmt_stats_t* packet_mmt_stats = find_packet_id(ip, port, packet_id);
	if(!packet_mmt_stats) {
		if(atsc3_global_statistics->packet_id_n && atsc3_global_statistics->packet_id_vector) {

			__PS_TRACE("*before realloc to %p, %i, adding %u", atsc3_global_statistics->packet_id_vector, atsc3_global_statistics->packet_id_n, packet_id);

			atsc3_global_statistics->packet_id_vector = (packet_id_mmt_stats_t**)realloc(atsc3_global_statistics->packet_id_vector, (atsc3_global_statistics->packet_id_n + 1) * sizeof(packet_id_mmt_stats_t*));
			if(!atsc3_global_statistics->packet_id_vector) {
				abort();
			}

			packet_mmt_stats = atsc3_global_statistics->packet_id_vector[atsc3_global_statistics->packet_id_n++] = (packet_id_mmt_stats_t*)calloc(1, sizeof(packet_id_mmt_stats_t));
			if(!packet_mmt_stats) {
				abort();
			}

			//sort after realloc
		    qsort((void**)atsc3_global_statistics->packet_id_vector, atsc3_global_statistics->packet_id_n, sizeof(packet_id_mmt_stats_t**), comparator_packet_id_mmt_stats_t);

		    __PS_TRACE(" *after realloc to %p, %i, adding %u", packet_mmt_stats, atsc3_global_statistics->packet_id_n, packet_id);

		} else {
			atsc3_global_statistics->packet_id_n = 1;
			atsc3_global_statistics->packet_id_vector = (packet_id_mmt_stats_t**)calloc(1, sizeof(packet_id_mmt_stats_t*));
			atsc3_global_statistics->packet_id_vector[0] = (packet_id_mmt_stats_t*)calloc(1, sizeof(packet_id_mmt_stats_t));

			if(!atsc3_global_statistics->packet_id_vector) {
				abort();
			}

			packet_mmt_stats = atsc3_global_statistics->packet_id_vector[0];
			__PS_TRACE("*calloc %p for %u", packet_mmt_stats, packet_id);
		}
		packet_mmt_stats->ip = ip;
		packet_mmt_stats->port = port;
		packet_mmt_stats->packet_id = packet_id;

		packet_mmt_stats->mpu_stats_timed_sample_interval = 	(packet_id_mmt_timed_mpu_stats_t*)calloc(1, sizeof(packet_id_mmt_timed_mpu_stats_t));
		packet_mmt_stats->mpu_stats_nontimed_sample_interval = (packet_id_mmt_nontimed_mpu_stats_t*)calloc(1, sizeof(packet_id_mmt_nontimed_mpu_stats_t));
		packet_mmt_stats->signalling_stats_sample_interval = 	(packet_id_signalling_stats_t*)calloc(1, sizeof(packet_id_signalling_stats_t));

		packet_mmt_stats->mpu_stats_timed_lifetime = 	(packet_id_mmt_timed_mpu_stats_t*)calloc(1, sizeof(packet_id_mmt_timed_mpu_stats_t));
		packet_mmt_stats->mpu_stats_nontimed_lifetime = (packet_id_mmt_nontimed_mpu_stats_t*)calloc(1, sizeof(packet_id_mmt_nontimed_mpu_stats_t));
		packet_mmt_stats->signalling_stats_lifetime = 	(packet_id_signalling_stats_t*)calloc(1, sizeof(packet_id_signalling_stats_t));
	}

	return packet_mmt_stats;
}

void atsc3_packet_statistics_mmt_timed_mpu_stats_populate(mmtp_mpu_packet_t* mmtp_mpu_packet, packet_id_mmt_stats_t* packet_mmt_stats) {
	packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_timed_total++;

	if(packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_sequence_number) {
		if(packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_sequence_number != mmtp_mpu_packet->mpu_sequence_number) {

			packet_mmt_stats->mpu_stats_timed_sample_interval->previous_mpu_sequence_number = packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_sequence_number_last;
			packet_mmt_stats->mpu_stats_timed_sample_interval->previous_timestamp_mpu_logged = false;

			packet_mmt_stats->mpu_stats_timed_sample_interval->previous_timestamp_mpu_metadata = packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_mpu_metadata;
			packet_mmt_stats->mpu_stats_timed_sample_interval->previous_timestamp_first_mfu = packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_first_mfu; //0x02
			packet_mmt_stats->mpu_stats_timed_sample_interval->previous_timestamp_last_mfu = packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_last_mfu; //0x02
			packet_mmt_stats->mpu_stats_timed_sample_interval->previous_timestamp_movie_fragment_metadata = packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_movie_fragment_metadata; //0x01

			packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_mpu_metadata = 0; // 0x00
			packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_first_mfu = 0; //0x02
			packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_last_mfu = 0; //0x02
			packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_movie_fragment_metadata = 0; //0x01

		}
		packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_sequence_number_last = packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_sequence_number;
	} else {
		packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_sequence_number_last = 0;

	}

	packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_sequence_number = mmtp_mpu_packet->mpu_sequence_number;

	if(packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_fragementation_counter) {
		packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_fragementation_counter_last = packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_fragementation_counter;
	} else {
		packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_fragementation_counter_last = 0;
	}
	packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_fragementation_counter = mmtp_mpu_packet->mpu_fragment_counter;

	//keep track of only our current stats for mfu timing from our mpu sequence number
	if(mmtp_mpu_packet->mpu_fragment_type == 0x00) {
		packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_mpu_metadata = mmtp_mpu_packet->mmtp_timestamp;
	} else if(mmtp_mpu_packet->mpu_fragment_type == 0x2) {
		if(!packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_first_mfu) {
			packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_first_mfu = mmtp_mpu_packet->mmtp_timestamp;
		}
		packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_last_mfu = mmtp_mpu_packet->mmtp_timestamp;

	} else if(mmtp_mpu_packet->mpu_fragment_type == 0x1) {
		packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_movie_fragment_metadata = mmtp_mpu_packet->mmtp_timestamp;
	}

}

int global_loss_count;
int __INVOKE_ATSC3_PACKET_STATISTICS_MMT_STATS_POPULATE_COUNT = 0;

void atsc3_packet_statistics_mmt_stats_populate(udp_packet_t* udp_packet, mmtp_mpu_packet_t* mmtp_mpu_packet) {
#ifndef __DISABLE_NCURSES__

	packet_id_mmt_stats_t* packet_mmt_stats = find_or_create_packet_id(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port, mmtp_mpu_packet->mmtp_packet_id);

	packet_mmt_stats->packet_sequence_number_sample_interval_processed++;
	packet_mmt_stats->packet_sequence_number_lifetime_processed++;

#if defined _DUMP_ALL_MPU_FLOWS_ && _DUMP_ALL_MPU_FLOWS_ == true
	//push this to our missing packet flow for investigation
					__PS_STATS_L("packets present:\t%u.%u.%u.%u\t%u\tpacket_counter:\t%u\ttimestamp:\t%u\tpacket_id:\t%u\tpacket_sequence_number:\t%u",
							__toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port),
							mmtp_mpu_packet->packet_counter,
							mmtp_mpu_packet->mmtp_timestamp,
							mmtp_mpu_packet->mmtp_packet_id,
							mmtp_mpu_packet->packet_sequence_number
							);

#endif

					//top level flow check from our new mmtp payload packet and our "current" reference packet

	if(packet_mmt_stats->has_packet_sequence_number &&
		mmtp_mpu_packet->packet_sequence_number != packet_mmt_stats->packet_sequence_number + 1) {

		//compute our intra packet gap, remember to add 1 because we have the anchor packets
		packet_mmt_stats->packet_sequence_number_last_gap = mmtp_mpu_packet->packet_sequence_number - packet_mmt_stats->packet_sequence_number - 1;

		//compute our sample interval gap
		packet_mmt_stats->packet_sequence_number_sample_interval_gap += packet_mmt_stats->packet_sequence_number_last_gap;

		if(packet_mmt_stats->packet_sequence_number_last_gap > packet_mmt_stats->packet_sequence_number_max_gap) {
			packet_mmt_stats->packet_sequence_number_max_gap = packet_mmt_stats->packet_sequence_number_last_gap;
		}

		//add this gap into the total count of mmt packets missing
		packet_mmt_stats->packet_sequence_number_sample_interval_missing += packet_mmt_stats->packet_sequence_number_last_gap;
		packet_mmt_stats->packet_sequence_number_lifetime_missing += packet_mmt_stats->packet_sequence_number_last_gap;
		atsc3_global_statistics->packet_counter_mmtp_packets_missing += packet_mmt_stats->packet_sequence_number_last_gap;


		if(packet_mmt_stats->packet_id && __LOSS_DISPLAY_ENABLED) {
			//todo clean this up
			if(__INVOKE_ATSC3_PACKET_STATISTICS_MMT_STATS_POPULATE_COUNT++%2) {
				ncurses_writer_lock_mutex_acquire();
				int row, col, h, w;
				getbegyx(pkt_global_loss_window, row, col);
				getmaxyx(pkt_global_loss_window, h, w);

				if(global_mmt_loss_count > row-3) {
					wmove(pkt_global_loss_window, 1, 1);
					wdeleteln(pkt_global_loss_window);
					wmove(pkt_global_loss_window, h-1, 0);

					//wrefresh(pkt_global_loss_window);

					global_mmt_loss_count--;
				}


				//todo - refactor this into struct for display scrolling and searching
//				__PS_STATS_GLOBAL_LOSS("Flow: %u.%u.%u.%u:%u, Packet_id: %u, Packet Counter: %u to %u, TS: %u-t%u, PSN: %u-%u, missing: %u",
//								__toip(packet_mmt_stats),
//								packet_mmt_stats->packet_id,
//								packet_mmt_stats->packet_counter_value,
//								mmtp_mpu_packet->packet_counter,
//								packet_mmt_stats->timestamp,
//								mmtp_mpu_packet->mmtp_timestamp,
//								packet_mmt_stats->packet_sequence_number,
//								mmtp_mpu_packet->packet_sequence_number,
//								packet_mmt_stats->packet_sequence_number_last_gap);
				global_mmt_loss_count++;
				ncurses_writer_lock_mutex_release();

				__PS_REFRESH_LOSS();

			}
		}

		//push this to our missing packet flow for investigation
		//__PS_STATS_STDOUT("packets missing:\t%u.%u.%u.%u\t%u\tpacket_counter_from:\t%u\tpacket_counter_to:\t%u\ttimestamp_from:\t%u\tfrom_s:\t%u\tfrom_us:\t%u\ttimestamp_to:\t%u\tto_s:\t%u\tto_us:\t%u\tpacket_id:\t%u\tPSN_from:\t%u\tPSN_to:\t%u\tTotal_missing:\t%u",
//				__toip(packet_mmt_stats),
//				packet_mmt_stats->packet_counter_value,
//				mmtp_mpu_packet->packet_counter,
//				packet_mmt_stats->timestamp,
//				packet_mmt_stats->timestamp_s,
//				packet_mmt_stats->timestamp_us,
//				mmtp_mpu_packet->mmtp_timestamp,
//				mmtp_mpu_packet->mmtp_timestamp_s,
//				mmtp_mpu_packet->mmtp_timestamp_us,
//				packet_mmt_stats->packet_id,
//				packet_mmt_stats->packet_sequence_number,
//				mmtp_mpu_packet->packet_sequence_number,
//				packet_mmt_stats->packet_sequence_number_last_gap);
			//	__PS_REFRESH_LOSS();
	}
	//remember, a lot of these values can roll over...
	packet_mmt_stats->packet_counter_value = mmtp_mpu_packet->packet_counter;

	//if we have a "current" packet sequence number, set it to our last value
	if(packet_mmt_stats->has_packet_sequence_number) {
		packet_mmt_stats->packet_sequence_number_last_value = packet_mmt_stats->packet_sequence_number;
		packet_mmt_stats->has_packet_sequence_number_last_value = true;
	} else {
		packet_mmt_stats->packet_sequence_number_last_value = 0;
	}

	//if we should reset our sample interval packet sequence number, i.e. NOT !packet_mmt_stats->has_packet_sequence_number_sample_interval_start
	if(!packet_mmt_stats->has_packet_sequence_number_sample_interval_start) {
		packet_mmt_stats->packet_sequence_number_sample_interval_start = mmtp_mpu_packet->packet_sequence_number;
		packet_mmt_stats->has_packet_sequence_number_sample_interval_start = true;
	}

	//if we haven't set our lifetime packet sequence number
	if(!packet_mmt_stats->has_packet_sequence_number_lifetime_start) {
		packet_mmt_stats->packet_sequence_number_lifetime_start = mmtp_mpu_packet->packet_sequence_number;
		packet_mmt_stats->has_packet_sequence_number_lifetime_start = true;
	}

	//update our "current" packet sequence number
	packet_mmt_stats->has_packet_sequence_number = true;
	packet_mmt_stats->packet_sequence_number = mmtp_mpu_packet->packet_sequence_number;

	if(packet_mmt_stats->has_timestamp) {
		packet_mmt_stats->has_timestamp_last = true;
		packet_mmt_stats->timestamp_last = packet_mmt_stats->timestamp;
	} else {
		packet_mmt_stats->timestamp_last = 0;
	}

	//set our timestamp
	packet_mmt_stats->timestamp = mmtp_mpu_packet->mmtp_timestamp;
	packet_mmt_stats->timestamp_s = mmtp_mpu_packet->mmtp_timestamp_s;
	packet_mmt_stats->timestamp_us = mmtp_mpu_packet->mmtp_timestamp_us;

	packet_mmt_stats->has_timestamp = true;

	//keep track of our starting timestamp sample interval for this flow - has_timestamp_sample_interval_start
	if(!packet_mmt_stats->has_timestamp_sample_interval_start) {
		packet_mmt_stats->timestamp_sample_interval_start = mmtp_mpu_packet->mmtp_timestamp;
		if(packet_mmt_stats->timestamp_sample_interval_start) {
			compute_ntp32_to_seconds_microseconds(packet_mmt_stats->timestamp_sample_interval_start, &packet_mmt_stats->timestamp_sample_interval_start_s, &packet_mmt_stats->timestamp_sample_interval_start_us);
			packet_mmt_stats->has_timestamp_sample_interval_start = true;
		} else {
		//	__PS_STATS_STDOUT("Missing sample start timestamp!");
			packet_mmt_stats->timestamp_sample_interval_start_s = 0;
			packet_mmt_stats->timestamp_sample_interval_start_us = 0;
		}
	}

	//keep track of our starting timestamp lifetime for this flow - has_timestamp_lifetime_start
	if(!packet_mmt_stats->has_timestamp_lifetime_start) {
		packet_mmt_stats->timestamp_lifetime_start = mmtp_mpu_packet->mmtp_timestamp;
		if(packet_mmt_stats->timestamp_lifetime_start) {
			compute_ntp32_to_seconds_microseconds(packet_mmt_stats->timestamp_lifetime_start, &packet_mmt_stats->timestamp_lifetime_start_s, &packet_mmt_stats->timestamp_lifetime_start_us);
			packet_mmt_stats->has_timestamp_lifetime_start = true;
		} else {
			//__PS_STATS_STDOUT("Missing sample start timestamp!");
			packet_mmt_stats->timestamp_lifetime_start_s = 0;
			packet_mmt_stats->timestamp_lifetime_start_us = 0;
		}
	}

	//mpu metadata
	if(mmtp_mpu_packet->mmtp_payload_type == 0x0) {

		//assign our timed mpu stats
		if(mmtp_mpu_packet->mpu_timed_flag == 1) {
			atsc3_packet_statistics_mmt_timed_mpu_stats_populate(mmtp_mpu_packet, packet_mmt_stats);

		} else {
			//assign our non-timed stats here
			packet_mmt_stats->mpu_stats_nontimed_sample_interval->mpu_nontimed_total++;
		}
	} else if(mmtp_mpu_packet->mmtp_payload_type == 0x2) {
		//assign our signalling stats here
		packet_mmt_stats->signalling_stats_sample_interval->signalling_messages_total++;
	}

	atsc3_global_statistics->packet_id_delta = packet_mmt_stats;
#endif
}

int DUMP_COUNTER=0;
int DUMP_COUNTER_2=0;

void atsc3_packet_statistics_dump_global_stats(){
#ifndef __DISABLE_NCURSES__

	bool has_output = false;

	__PS_CLEAR();
	struct timeval tNow;
	gettimeofday(&tNow, NULL);
	long long elapsedDurationUs = timediff(tNow, atsc3_global_statistics->program_timeval_start);
	__PS_STATS_GLOBAL("Elapsed Duration            : %-.2fs", elapsedDurationUs / 1000000.0);
	__PS_STATS_GLOBAL("");
	__PS_STATS_GLOBAL("LLS total packets received  : %'-u", atsc3_global_statistics->packet_counter_lls_packets_received);
	__PS_STATS_GLOBAL("> parsed good               : %'-u", atsc3_global_statistics->packet_counter_lls_packets_parsed);
	__PS_STATS_GLOBAL("> parsed error              : %'-u", atsc3_global_statistics->packet_counter_lls_packets_parsed_error);
	__PS_STATS_GLOBAL("- SLT packets decoded       : %'-u", atsc3_global_statistics->packet_counter_lls_slt_packets_parsed);
	__PS_STATS_GLOBAL("  - SLT updates processed   : %'-u", atsc3_global_statistics->packet_counter_lls_slt_update_processed);
	__PS_STATS_GLOBAL("");
	__PS_STATS_GLOBAL("MMTP total packets received : %'-u", atsc3_global_statistics->packet_counter_mmtp_packets_received);
	__PS_STATS_GLOBAL("- type=0x0 MPU              : %'-u", atsc3_global_statistics->packet_counter_mmt_mpu);
	__PS_STATS_GLOBAL("  - timed                   : %'-u", atsc3_global_statistics->packet_counter_mmt_timed_mpu);
	__PS_STATS_GLOBAL("  - non-timed               : %'-u", atsc3_global_statistics->packet_counter_mmt_nontimed_mpu);
	__PS_STATS_GLOBAL("- type=0x1 Signaling        : %'-u",	atsc3_global_statistics->packet_counter_mmt_signaling);
	__PS_STATS_GLOBAL("- type=0x? Other            : %'-u",	atsc3_global_statistics->packet_counter_mmt_unknown);
	__PS_STATS_GLOBAL("> parsed errors             : %'-u",	atsc3_global_statistics->packet_counter_mmtp_packets_parsed_error);
	__PS_STATS_GLOBAL("> missing packets           : %'-u",	atsc3_global_statistics->packet_counter_mmtp_packets_missing);
	__PS_STATS_GLOBAL("");
	__PS_STATS_GLOBAL("ALC total packets received  : %'-u",	atsc3_global_statistics->packet_counter_alc_recv);
	__PS_STATS_GLOBAL("> parsed good               : %'-u",	atsc3_global_statistics->packet_counter_alc_packets_parsed);
	__PS_STATS_GLOBAL("> parsed errors             : %'-u",	atsc3_global_statistics->packet_counter_alc_packets_parsed_error);
	__PS_STATS_GLOBAL("")
	__PS_STATS_GLOBAL("Non ATSC3 Packets           : %'-u", atsc3_global_statistics->packet_counter_filtered_ipv4);
	__PS_STATS_GLOBAL("");
	__PS_STATS_GLOBAL("");
	__PS_STATS_GLOBAL("Total Mulicast Packets RX   : %'-u", atsc3_global_statistics->packets_total_received);

	//dump flow status
	for(int i=0; i < atsc3_global_statistics->packet_id_n; i++ ) {
		packet_id_mmt_stats_t* packet_mmt_stats = atsc3_global_statistics->packet_id_vector[i];
        //sanity check
        if(!packet_mmt_stats) {
            __ERROR("index: %u, packet_mmt_stats is NULL", i);
            continue;
        }

		double computed_flow_packet_loss = 0;
		if(packet_mmt_stats->packet_sequence_number_lifetime_processed && packet_mmt_stats->packet_sequence_number_lifetime_missing) {
			computed_flow_packet_loss = 100.0 * (packet_mmt_stats->packet_sequence_number_lifetime_missing / packet_mmt_stats->packet_sequence_number_lifetime_processed);
		}
		uint16_t seconds;
		uint16_t microseconds;
		compute_ntp32_to_seconds_microseconds(packet_mmt_stats->timestamp, &seconds, &microseconds);
		__PS_STATS_FLOW("Interval Flow           : %u.%u.%u.%u:%u, packet_id: %u, NTP range: %u.%03u to %u.%03u (%-u - %-u)", __toip(packet_mmt_stats),
						packet_mmt_stats->packet_id,
						packet_mmt_stats->timestamp_sample_interval_start_s,
						packet_mmt_stats->timestamp_sample_interval_start_us/100,

						seconds,
						microseconds/100,
						packet_mmt_stats->timestamp_sample_interval_start,
						packet_mmt_stats->timestamp);


		__PS_STATS_FLOW("packet_sequence_numbers : %-10u to %-10u (0x%08x to 0x%08x)",		packet_mmt_stats->packet_sequence_number_sample_interval_start, 	packet_mmt_stats->packet_sequence_number, packet_mmt_stats->packet_sequence_number_sample_interval_start,	packet_mmt_stats->packet_sequence_number);
		__PS_STATS_FLOW("packet RX count         : %-6d     mpu_sequence_number: %-10u  ",	packet_mmt_stats->packet_sequence_number_sample_interval_processed,
																										packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_sequence_number_last);

		__PS_STATS_FLOW("missing                 : %-6d     mpu timed_total: %-6d", 	packet_mmt_stats->packet_sequence_number_sample_interval_missing,
																								packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_timed_total);
		__PS_STATS_FLOW("pkt_seq num gap         : %-9d  signalling_messages total: %u", packet_mmt_stats->packet_sequence_number_sample_interval_gap,
				packet_mmt_stats->signalling_stats_sample_interval->signalling_messages_total);

		__PS_STATS_FLOW("Lifetime NTP            : %u.%03u  to %u.%03u  (%-10u to %-10u)    Loss Pct: %f",packet_mmt_stats->timestamp_lifetime_start_s, packet_mmt_stats->timestamp_lifetime_start_us/100, seconds, microseconds/100, packet_mmt_stats->timestamp_lifetime_start, packet_mmt_stats->timestamp, computed_flow_packet_loss);
		__PS_STATS_FLOW("packet_seq_numbers      : %-10u to %-10u (0x%08x to 0x%08x)    max sequence gap: %-6d ",	packet_mmt_stats->packet_sequence_number_lifetime_start,  packet_mmt_stats->packet_sequence_number, packet_mmt_stats->packet_sequence_number_lifetime_start, packet_mmt_stats->packet_sequence_number, packet_mmt_stats->packet_sequence_number_max_gap);
		__PS_STATS_FLOW("Total packets RX        : %-6u     missing: %-6u",	packet_mmt_stats->packet_sequence_number_lifetime_processed, packet_mmt_stats->packet_sequence_number_lifetime_missing);

		int row, col;
		getyx(pkt_flow_stats_mmt_window, row, col);
	//	printf("----row: %d, col: %d\n", row, col);
		wmove(pkt_flow_stats_mmt_window, row, col+2);
		//wrefresh(pkt_flow_stats_mmt_window);
		whline(pkt_flow_stats_mmt_window, ACS_HLINE, 8);
		//wrefresh(pkt_flow_stats_mmt_window);

		__PS_STATS_HR();

		//clear out any sample interval attributes
		packet_mmt_stats->has_timestamp_sample_interval_start = false;
		packet_mmt_stats->packet_sequence_number_sample_interval_gap = 0;
		packet_mmt_stats->packet_sequence_number_sample_interval_start = 0;
		packet_mmt_stats->has_packet_sequence_number_sample_interval_start = false;
	}

	__PS_REFRESH();


	//check for any intra status update flow derivations from deltas
//	if(global_stats->packet_id_delta) {
//		packet_id_mmt_stats_t* packet_mmt_stats = global_stats->packet_id_delta;
//		if(packet_mmt_stats->mpu_stats_timed->mpu_sequence_number_last &&
//				(packet_mmt_stats->mpu_stats_timed->mpu_sequence_number_last != packet_mmt_stats->mpu_stats_timed->mpu_sequence_number && packet_mmt_stats->mpu_stats_timed->mpu_fragementation_counter_last != 0)) {
//
//			__PS_WARN(" **mpu sequence gap, packet_id: %u, FROM mpu_sequence:%u, packet_seq_num_last:%u, mpu_frag_counter_last: %d TO mpu_sequence:%u, packet_seq_num:%u, mpu_frag_counter: %u",
//					packet_mmt_stats->packet_id,
//					packet_mmt_stats->mpu_stats_timed->mpu_sequence_number_last,
//					packet_mmt_stats->packet_sequence_number_last_value,
//					packet_mmt_stats->mpu_stats_timed->mpu_fragementation_counter_last,
//					packet_mmt_stats->mpu_stats_timed->mpu_sequence_number,
//					packet_mmt_stats->packet_sequence_number,
//					packet_mmt_stats->mpu_stats_timed->mpu_fragementation_counter);
//
//			has_output=true;
//		}
//	}


	//process any gaps or deltas

//	global_stats->packet_id_delta = NULL;

#endif
}

/**
 * mmt stats:
 *
 * only print out the timestamp ordering for:
 * 0x0: timestamp mpu_metadata
 * 0x2: timestamp first mfu
 * 0x1: timestamp movie fragment metadata
 */


void atsc3_packet_statistics_dump_mfu_stats(){
#ifndef __DISABLE_NCURSES__

	bool has_output = false;

	__PS_CLEAR();
	struct timeval tNow;
	gettimeofday(&tNow, NULL);
	long long elapsedDurationUs = timediff(tNow, atsc3_global_statistics->program_timeval_start);
	__PS_STATS_GLOBAL("Elapsed Duration            : %-.2fs", elapsedDurationUs / 1000000.0);
	__PS_STATS_GLOBAL("");
	__PS_STATS_GLOBAL("LLS total packets received  : %'-u", atsc3_global_statistics->packet_counter_lls_packets_received);
	__PS_STATS_GLOBAL("> parsed good               : %'-u", atsc3_global_statistics->packet_counter_lls_packets_parsed);
	__PS_STATS_GLOBAL("> parsed error              : %'-u", atsc3_global_statistics->packet_counter_lls_packets_parsed_error);
	__PS_STATS_GLOBAL("- SLT packets decoded       : %'-u", atsc3_global_statistics->packet_counter_lls_slt_packets_parsed);
	__PS_STATS_GLOBAL("  - SLT updates processed   : %'-u", atsc3_global_statistics->packet_counter_lls_slt_update_processed);
	__PS_STATS_GLOBAL("");
	__PS_STATS_GLOBAL("MMTP total packets received : %'-u", atsc3_global_statistics->packet_counter_mmtp_packets_received);
	__PS_STATS_GLOBAL("- type=0x0 MPU              : %'-u", atsc3_global_statistics->packet_counter_mmt_mpu);
	__PS_STATS_GLOBAL("  - timed                   : %'-u", atsc3_global_statistics->packet_counter_mmt_timed_mpu);
	__PS_STATS_GLOBAL("  - non-timed               : %'-u", atsc3_global_statistics->packet_counter_mmt_nontimed_mpu);
	__PS_STATS_GLOBAL("- type=0x1 Signaling        : %'-u",	atsc3_global_statistics->packet_counter_mmt_signaling);
	__PS_STATS_GLOBAL("- type=0x? Other            : %'-u",	atsc3_global_statistics->packet_counter_mmt_unknown);
	__PS_STATS_GLOBAL("> parsed errors             : %'-u",	atsc3_global_statistics->packet_counter_mmtp_packets_parsed_error);
	__PS_STATS_GLOBAL("> missing packets           : %'-u",	atsc3_global_statistics->packet_counter_mmtp_packets_missing);
	__PS_STATS_GLOBAL("");
	__PS_STATS_GLOBAL("Total Mulicast Packets RX   : %'-u", atsc3_global_statistics->packets_total_received);

	//dump flow status
	for(int i=0; i < atsc3_global_statistics->packet_id_n; i++ ) {
		packet_id_mmt_stats_t* packet_mmt_stats = atsc3_global_statistics->packet_id_vector[i];
		if(!packet_mmt_stats->packet_id)
			continue;


		uint16_t seconds;
		uint16_t microseconds;

		uint16_t seconds_mfu_first;
		uint16_t microseconds_mfu_first;

		uint16_t seconds_mfu_last;
		uint16_t microseconds_mfu_last;

		uint16_t seconds_movie_fragment;
		uint16_t microseconds_movie_fragment;

		compute_ntp32_to_seconds_microseconds(packet_mmt_stats->timestamp, &seconds, &microseconds);
		__PS_STATS_FLOW("Interval Flow                 : %u.%u.%u.%u:%u, Packet ID: %u", __toip(packet_mmt_stats),
						packet_mmt_stats->packet_id);

		__PS_STATS_FLOW("NTP range                     : %u.%03u to %u.%03u", packet_mmt_stats->timestamp_sample_interval_start_s,
				packet_mmt_stats->timestamp_sample_interval_start_us/100,
				seconds,
				microseconds/100);
		__PS_STATS_FLOW("");

		__PS_STATS_FLOW(" current mpu_sequence_number  : %u", packet_mmt_stats->mpu_stats_timed_sample_interval->mpu_sequence_number);
		compute_ntp32_to_seconds_microseconds(packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_mpu_metadata, &seconds, &microseconds);
		__PS_STATS_FLOW("  0x0: timestamp mpu_metadata : %u.%03u", seconds, microseconds/100);

		//without mutex locks or de-jittering this may be dirty data...
		compute_ntp32_to_seconds_microseconds(packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_first_mfu, &seconds_mfu_first, &microseconds_mfu_first);
		compute_ntp32_to_seconds_microseconds(packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_last_mfu, &seconds_mfu_last, &microseconds_mfu_last);

		__PS_STATS_FLOW("  0x2: timestamp first mfu    : %u.%03u", seconds_mfu_first, microseconds_mfu_first/100);
		__PS_STATS_FLOW("  0x2: timestamp last mfu     : %u.%03u", seconds_mfu_last, microseconds_mfu_last/100);

		compute_ntp32_to_seconds_microseconds(packet_mmt_stats->mpu_stats_timed_sample_interval->timestamp_movie_fragment_metadata, &seconds_movie_fragment, &microseconds_movie_fragment);
		__PS_STATS_FLOW("  0x1: movie fragment metadata: %u.%03u ", seconds_movie_fragment, microseconds_movie_fragment/100);

		__PS_STATS_FLOW("");
		__PS_STATS_FLOW(" previous mpu_sequence stats  : %u", packet_mmt_stats->mpu_stats_timed_sample_interval->previous_mpu_sequence_number);

		compute_ntp32_to_seconds_microseconds(packet_mmt_stats->mpu_stats_timed_sample_interval->previous_timestamp_mpu_metadata, &seconds, &microseconds);
		__PS_STATS_FLOW("  0x0: timestamp mpu_metadata : %u.%03u", seconds, microseconds/100);

		compute_ntp32_to_seconds_microseconds(packet_mmt_stats->mpu_stats_timed_sample_interval->previous_timestamp_first_mfu, &seconds_mfu_first, &microseconds_mfu_first);
		compute_ntp32_to_seconds_microseconds(packet_mmt_stats->mpu_stats_timed_sample_interval->previous_timestamp_last_mfu,  &seconds_mfu_last, &microseconds_mfu_last);

		__PS_STATS_FLOW("  0x2: timestamp first mfu    : %u.%03u", seconds_mfu_first, microseconds_mfu_first/100);
		__PS_STATS_FLOW("  0x2: timestamp last mfu     : %u.%03u", seconds_mfu_last, microseconds_mfu_last/100);

		compute_ntp32_to_seconds_microseconds(packet_mmt_stats->mpu_stats_timed_sample_interval->previous_timestamp_movie_fragment_metadata, &seconds_movie_fragment, &microseconds_movie_fragment);
		//if our moof came before the first frame, then this is most likely a MPU delivery
		if(seconds_movie_fragment < seconds_mfu_first || (seconds_movie_fragment == seconds_mfu_first && microseconds_movie_fragment/100 < microseconds_mfu_first/100)) {
		//		if(seconds_movie_fragment < seconds_mfu_last || (seconds_movie_fragment == seconds_mfu_last && microseconds_movie_fragment/100 < microseconds_mfu_last/100)) {
			__PS_STATS_FLOW_W("  0x1: movie fragment metadata: %u.%03u ", seconds_movie_fragment, microseconds_movie_fragment/100);
			wattron(pkt_flow_stats_mmt_window, COLOR_PAIR(2));
			__PS_STATS_FLOW("(MPU)");
			wattroff(pkt_flow_stats_mmt_window, COLOR_PAIR(2));

			if(!packet_mmt_stats->mpu_stats_timed_sample_interval->previous_timestamp_mpu_logged) {
				__PS_STATS_FLOW_LOG("Flow: %u.%u.%u.%u:%u, PktID: %u, MPU_seq_num: %u, 0x2: %u.%03u-%u.%03u, 0x1: %u.%03u", __toip(packet_mmt_stats),
							packet_mmt_stats->packet_id,
							packet_mmt_stats->mpu_stats_timed_sample_interval->previous_mpu_sequence_number,
							seconds_mfu_first, microseconds_mfu_first/100,
							seconds_mfu_last, microseconds_mfu_last/100,
							seconds_movie_fragment, microseconds_movie_fragment/100);
				__PS_STATS_FLOW_LOG_REFRESH();
				packet_mmt_stats->mpu_stats_timed_sample_interval->previous_timestamp_mpu_logged = true;
			}

		} else {
			__PS_STATS_FLOW_W("  0x1: movie fragment metadata: %u.%03u ", seconds_movie_fragment, microseconds_movie_fragment/100);
			wattron(pkt_flow_stats_mmt_window, COLOR_PAIR(3) | COLOR_PAIR(0xff) );

			__PS_STATS_FLOW("(MFU)");
			wattroff(pkt_flow_stats_mmt_window, COLOR_PAIR(3) | COLOR_PAIR(0xff));
		}

		__PS_STATS_FLOW("");

		//clear out any sample interval attributes
		packet_mmt_stats->has_timestamp_sample_interval_start = false;
		packet_mmt_stats->packet_sequence_number_sample_interval_gap = 0;
		packet_mmt_stats->packet_sequence_number_sample_interval_start = 0;
		packet_mmt_stats->has_packet_sequence_number_sample_interval_start = false;
	}

	__PS_REFRESH();

#endif

}
