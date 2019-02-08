/*
 * atsc3_listener_metrics_test.c
 *
 *  Created on: Jan 19, 2019
 *      Author: jjustman
 *
 * global listener driver for LLS, MMT and ROUTE / DASH (coming soon)
 *
 *
 * borrowed from https://stackoverflow.com/questions/26275019/how-to-read-and-send-udp-packets-on-mac-os-x
 * uses libpacp for udp mulicast packet listening
 *
 * opt flags:
  export LDFLAGS="-L/usr/local/opt/libpcap/lib"
  export CPPFLAGS="-I/usr/local/opt/libpcap/include"

  to invoke test driver, run ala:

  ./atsc3_listener_metrics_test vnic1


  TODO: A/331 - Section 8.1.2.1.3 - Constraints on MMTP
  	  PacketId



  TODO: A/331 - Section 6.1  IP Address Assignment
  	  Implement a more robust ip filtering functionality for flow selection

  	  6.1 IP Address Assignment


LLS shall be transported in IP packets with address 224.0.23.60 and
destination port 4937/udp.1 All IP packets other than LLS IP packets
shall carry a Destination IP address either

	(a) allocated and reserved by a mechanism guaranteeing that the
	 destination addresses in use are unique in a geographic region2,or

	(b) in the range of 239.255.0.0 to 239.255.255.2553, where the
	bits in the third octet shall correspond to a value of
	SLT.Service@majorChannelNo registered to the broadcaster for use
	in the Service Area4 of the broadcast transmission, with the
	following caveats:

	• If a broadcast entity operates transmissions carrying different Services
	on multiple RF frequencies with all or a part of their service area in common,
	each IP address/port combination shall be unique across all such broadcast emissions;

	•In the case that multiple LLS streams (hence, multiple SLTs) are present in a
	given broadcast emission, each IP address/port combination in use for non-LLS streams
	shall be unique across all Services in the aggregate broadcast emission;



	for testing:
	  fprintf(stdout, "left window h: rows: %d, max: %d, %lu, %d,  %d, %d", rows, INT_MAX, sizeof(int), left_window_h, rows*100, (rows*100)/66);

  abort();

*/


//#define _ENABLE_TRACE 1
//#define _SHOW_PACKET_FLOW 1

int PACKET_COUNTER=0;

#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <ncurses.h>
#include <limits.h>
/* ncurses.h includes stdio.h */
#include "atsc3_output_statistics_ncurses.h"

#define _ENABLE_DEBUG true

#define __ERROR(...)   printf("%s:%d:ERROR :","listener",__LINE__);printf(__VA_ARGS__);
#define __WARN(...)    printf("%s:%d:WARN: ","listener",__LINE__);printf(__VA_ARGS__);
#define __INFO(...)    printf("%s:%d: ","listener",__LINE__);printf(__VA_ARGS__);

#ifdef _ENABLE_DEBUG
#define __DEBUG(...)   printf("%s:%d:DEBUG: ","listener",__LINE__);printf(__VA_ARGS__);
#define __DEBUGF(...)  printf("%s:%d:DEBUG: ","listener",__LINE__);printf(__VA_ARGS__);
#define __DEBUGA(...) 	__PRINTF(__VA_ARGS__);
#define __DEBUGN(...)  __PRINTLN(__VA_ARGS__);
#else
#define __DEBUG(...)
#define __DEBUGF(...)
#define __DEBUGA(...)
#define __DEBUGN(...)
#endif


#ifndef _TEST_RUN_VALGRIND_OSX_

int printf(const char *format, ...)  {
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
    va_end(argptr);

	return 0;
}
#endif



void create_or_update_window_sizes(bool should_reload_term_size) {
	int rows, cols;

	if(should_reload_term_size) {
		struct winsize size;
		if (ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) {
			__INFO("rows: %d, cols:%d\n",  size.ws_row, size.ws_col);
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

	int pct_split_top = 66;

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
	signaling_global_stats_window = derwin(left_window_outline, left_window_h-6, left_window_w/2-2, 1, 45 );
	wvline(signaling_global_stats_window, ACS_VLINE, left_window_h-8);

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
	pkt_flow_stats_window =	derwin(right_window_outline, right_window_h-6, right_window_w-3, 1, 1);

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




#include "atsc3_listener_udp.h"
#include "atsc3_utils.h"

#include "atsc3_lls.h"
#include "atsc3_lls_alc_tools.h"

#include "atsc3_mmtp_types.h"
#include "atsc3_mmtp_parser.h"
#include "atsc3_mmtp_ntp32_to_pts.h"

#include "alc_channel.h"
#include "alc_rx.h"
#include "atsc3_alc_utils.h"

#include "atsc3_bandwidth_statistics.h"
#include "atsc3_packet_statistics.h"

extern int _MPU_DEBUG_ENABLED;
extern int _MMTP_DEBUG_ENABLED;
extern int _LLS_DEBUG_ENABLED;



#ifdef _ENABLE_TRACE
#define __TRACE(...)   printf("%s:%d:TRACE:",__FILE__,__LINE__);__PRINTLN(__VA_ARGS__);

void __trace_dump_ip_header_info(u_char* ip_header) {
    __TRACE("Version\t\t\t\t\t%d", (ip_header[0] >> 4));
    __TRACE("IHL\t\t\t\t\t\t%d", (ip_header[0] & 0x0F));
    __TRACE("Type of Service\t\t\t%d", ip_header[1]);
    __TRACE("Total Length\t\t\t%d", ip_header[2]);
    __TRACE("Identification\t\t\t0x%02x 0x%02x", ip_header[3], ip_header[4]);
    __TRACE("Flags\t\t\t\t\t%d", ip_header[5] >> 5);
    __TRACE("Fragment Offset\t\t\t%d", (((ip_header[5] & 0x1F) << 8) + ip_header[6]));
    __TRACE("Time To Live\t\t\t%d", ip_header[7]);
    __TRACE("Header Checksum\t\t\t0x%02x 0x%02x", ip_header[10], ip_header[11]);
}

#else
#define __TRACE(...)
#endif

//commandline stream filtering

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;

// lls and alc glue for slt, contains lls_table_slt and lls_slt_alc_session

lls_session_t* lls_session;

int process_lls_table_slt_update(lls_table_t* lls) {

	if(lls_session->lls_table_slt) {
		lls_table_free(lls_session->lls_table_slt);
		lls_session->lls_table_slt = NULL;
	}
	lls_session->lls_table_slt = lls;

	ncurses_writer_lock_mutex_acquire();

	__LLS_DUMP_NOUPDATE();
	__LLS_DUMP_CLEAR();
	for(int i=0; i < lls->slt_table.service_entry_n; i++) {
			service_t* service = lls->slt_table.service_entry[i];
			lls_dump_instance_table_ncurses(lls_session->lls_table_slt);
	}
	__DOUPDATE();
	__LLS_REFRESH();
	ncurses_writer_lock_mutex_release();


	for(int i=0; i < lls->slt_table.service_entry_n; i++) {
		service_t* service = lls->slt_table.service_entry[i];
		__INFO("checking service: %d", service->service_id);

		if(service->broadcast_svc_signaling.sls_protocol == SLS_PROTOCOL_ROUTE) {
			//TODO - we probably need to clear out the ALC session?
			if(!lls_session->lls_slt_alc_session->alc_session) {

				lls_session->lls_slt_alc_session->lls_slt_service_id_alc = service->service_id;
				lls_session->lls_slt_alc_session->alc_arguments = calloc(1, sizeof(alc_arguments_t));

				lls_session->lls_slt_alc_session->sls_source_ip_address = parseIpAddressIntoIntval(service->broadcast_svc_signaling.sls_source_ip_address);

				lls_session->lls_slt_alc_session->sls_destination_ip_address = parseIpAddressIntoIntval(service->broadcast_svc_signaling.sls_destination_ip_address);
				lls_session->lls_slt_alc_session->sls_destination_udp_port = parsePortIntoIntval(service->broadcast_svc_signaling.sls_destination_udp_port);

				__INFO("adding sls_source ip: %s as: %u.%u.%u.%u| dest: %s:%s as: %u.%u.%u.%u:%u (%u:%u)",
						service->broadcast_svc_signaling.sls_source_ip_address,
						__toipnonstruct(lls_session->lls_slt_alc_session->sls_source_ip_address),
						service->broadcast_svc_signaling.sls_destination_ip_address,
						service->broadcast_svc_signaling.sls_destination_udp_port,
						__toipandportnonstruct(lls_session->lls_slt_alc_session->sls_destination_ip_address, lls_session->lls_slt_alc_session->sls_destination_udp_port),
						lls_session->lls_slt_alc_session->sls_destination_ip_address, lls_session->lls_slt_alc_session->sls_destination_udp_port);

				lls_session->lls_slt_alc_session->alc_session = open_alc_session(lls_session->lls_slt_alc_session->alc_arguments);

				if(!lls_session->lls_slt_alc_session->alc_session) {
				  __ERROR("Unable to instantiate alc session for service_id: %d via SLS_PROTOCOL_ROUTE", service->service_id);
					goto cleanup;
				}

		  	}
		}
	}
	global_stats->packet_counter_lls_slt_update_processed++;
	return 0;

cleanup:
	if(lls_session->lls_slt_alc_session->alc_arguments) {
		free(lls_session->lls_slt_alc_session->alc_arguments);
		lls_session->lls_slt_alc_session->alc_arguments = NULL;
	}

	if(lls_session->lls_slt_alc_session->alc_session) {
		free(lls_session->lls_slt_alc_session->alc_session);
		lls_session->lls_slt_alc_session->alc_session = NULL;
	}
	return -1;
}


//make sure to invoke     mmtp_sub_flow_vector_init(&p_sys->mmtp_sub_flow_vector);
mmtp_sub_flow_vector_t* mmtp_sub_flow_vector;
void dump_mpu(mmtp_payload_fragments_union_t* mmtp_payload) {

	__DEBUG("------------------");

	if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_timed_flag) {
		__DEBUG("MFU Packet (Timed)");
		__DEBUG("-----------------");
		__DEBUG(" mpu_fragmentation_indicator: %d", mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_fragment_type);
		__DEBUG(" movie_fragment_seq_num: %u", mmtp_payload->mpu_data_unit_payload_fragments_timed.movie_fragment_sequence_number);
		__DEBUG(" sample_num: %u", mmtp_payload->mpu_data_unit_payload_fragments_timed.sample_number);
		__DEBUG(" offset: %u", mmtp_payload->mpu_data_unit_payload_fragments_timed.offset);
		__DEBUG(" pri: %d", mmtp_payload->mpu_data_unit_payload_fragments_timed.priority);
		__DEBUG(" mpu_sequence_number: %u",mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number);

	} else {
		__DEBUG("MFU Packet (Non-timed)");
		__DEBUG("---------------------");
		__DEBUG(" mpu_fragmentation_indicator: %d", mmtp_payload->mpu_data_unit_payload_fragments_nontimed.mpu_fragment_type);
		__DEBUG(" non_timed_mfu_item_id: %u", mmtp_payload->mpu_data_unit_payload_fragments_nontimed.non_timed_mfu_item_id);

	}

	__DEBUG("-----------------");
}

void mpu_dump_flow(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload) {
	//sub_flow_vector is a global
	dump_mpu(mmtp_payload);

	__DEBUG("::dumpMfu ******* file dump file: %d.%d.%d.%d:%d-p:%d.s:%d.ft:%d",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,

			mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type);

	char *myFilePathName = calloc(64, sizeof(char*));
	snprintf(myFilePathName, 64, "mpu/%d.%d.%d.%d,%d-p.%d.s,%d.ft,%d",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,

			mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type);


	__DEBUG("::dumpMfu ******* file dump file: %s", myFilePathName);

	FILE *f = fopen(myFilePathName, "a");
	if(!f) {
		__INFO("::dumpMpu ******* UNABLE TO OPEN FILE %s", myFilePathName);
			return;
	}


	for(int i=0; i <  mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer; i++) {
		fputc(mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer[i], f);
	}
	fclose(f);
}

//assumes in-order delivery
void mpu_dump_reconstitued(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload) {
	//sub_flow_vector is a global
	dump_mpu(mmtp_payload);

	__DEBUG("::dump_mpu_reconstitued ******* file dump file: %d.%d.%d.%d:%d-p:%d.s:%d.ft:%d",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,

			mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type);

	char *myFilePathName = calloc(64, sizeof(char*));
	snprintf(myFilePathName, 64, "mpu/%d.%d.%d.%d,%d-p.%d.s,%d.ft",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number);


	__DEBUG("::dumpMfu ******* file dump file: %s", myFilePathName);

		FILE *f = fopen(myFilePathName, "a");
	if(!f) {
		__ERROR("::dumpMpu ******* UNABLE TO OPEN FILE %s", myFilePathName);
			return;
	}


	for(int i=0; i <  mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer; i++) {
		fputc(mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer[i], f);
	}
	fclose(f);
}


void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {

  int i = 0;
  int k = 0;
  u_char ethernet_packet[14];
  u_char ip_header[24];
  u_char udp_header[8];
  int udp_header_start = 34;
  udp_packet_t* udp_packet = NULL;

    for (i = 0; i < 14; i++) {
        ethernet_packet[i] = packet[0 + i];
    }
    if (!(ethernet_packet[12] == 0x08 && ethernet_packet[13] == 0x00)) {
        __TRACE("Source MAC Address\t\t\t%02X:%02X:%02X:%02X:%02X:%02X", ethernet_packet[6], ethernet_packet[7], ethernet_packet[8], ethernet_packet[9], ethernet_packet[10], ethernet_packet[11]);
        __TRACE("Destination MAC Address\t\t%02X:%02X:%02X:%02X:%02X:%02X", ethernet_packet[0], ethernet_packet[1], ethernet_packet[2], ethernet_packet[3], ethernet_packet[4], ethernet_packet[5]);
    	__TRACE("Discarding packet with Ethertype unknown");
    	return;
    }

    for (i = 0; i < 20; i++) {
		ip_header[i] = packet[14 + i];
	}

	//check if we are a UDP packet, otherwise bail
	if (ip_header[9] != 0x11) {
		__TRACE("Protocol not UDP, dropping");
		return;
	}

	#ifdef _ENABLE_TRACE
        __trace_dump_ip_header_info(ip_header);
	#endif

	if ((ip_header[0] & 0x0F) > 5) {
		udp_header_start = 48;
		__TRACE("Options\t\t\t\t\t0x%02x 0x%02x 0x%02x 0x%02x", ip_header[20], ip_header[21], ip_header[22], ip_header[23]);
	}

	//malloc our udp_packet_header:
	udp_packet = calloc(1, sizeof(udp_packet_t));
	udp_packet->src_ip_addr = ((ip_header[12] & 0xFF) << 24) | ((ip_header[13]  & 0xFF) << 16) | ((ip_header[14]  & 0xFF) << 8) | (ip_header[15] & 0xFF);
	udp_packet->dst_ip_addr = ((ip_header[16] & 0xFF) << 24) | ((ip_header[17]  & 0xFF) << 16) | ((ip_header[18]  & 0xFF) << 8) | (ip_header[19] & 0xFF);

	for (i = 0; i < 8; i++) {
		udp_header[i] = packet[udp_header_start + i];
	}

	udp_packet->src_port = (udp_header[0] << 8) + udp_header[1];
	udp_packet->dst_port = (udp_header[2] << 8) + udp_header[3];

	udp_packet->total_packet_length = pkthdr->len;
	udp_packet->data_length = pkthdr->len - (udp_header_start + 8);



	if(udp_packet->data_length <=0 || udp_packet->data_length > 1514) {
		__ERROR("invalid data length of udp packet: %d", udp_packet->data_length);
		return;
	}
	__TRACE("Data length: %d", udp_packet->data_length);
	udp_packet->data = malloc(udp_packet->data_length * sizeof(udp_packet->data));
	memcpy(udp_packet->data, &packet[udp_header_start + 8], udp_packet->data_length);

	//inefficient as hell for 1 byte at a time, but oh well...
	#ifdef __ENABLE_TRACE
		for (i = 0; i < udp_packet->data_length; i++) {
			__TRACE("%02x ", packet[udp_header_start + 8 + i]);
		}
	#endif

	//collect global
	global_bandwidth_statistics->interval_total_current_bytes_rx += udp_packet->total_packet_length;
	global_bandwidth_statistics->interval_total_current_packets_rx++;
	global_bandwidth_statistics->grand_total_bytes_rx += udp_packet->total_packet_length;
	global_bandwidth_statistics->grand_total_packets_rx++;
	global_stats->packets_total_received++;

	//drop mdNS
	if(udp_packet->dst_ip_addr == UDP_FILTER_MDNS_IP_ADDRESS && udp_packet->dst_port == UDP_FILTER_MDNS_PORT) {
		global_stats->packet_counter_filtered_ipv4++;
		//printf("setting dns current_bytes_rx: %d, packets_rx: %d", global_bandwidth_statistics->interval_filtered_current_bytes_rx, global_bandwidth_statistics->interval_filtered_current_packets_rx);
		global_bandwidth_statistics->interval_filtered_current_bytes_rx += udp_packet->data_length;
		global_bandwidth_statistics->interval_filtered_current_packets_rx++;

		goto cleanup;
	}

	if(udp_packet->dst_ip_addr == LLS_DST_ADDR && udp_packet->dst_port == LLS_DST_PORT) {
		global_bandwidth_statistics->interval_lls_current_bytes_rx += udp_packet->data_length;
		global_bandwidth_statistics->interval_lls_current_packets_rx++;

		global_stats->packet_counter_lls_packets_received++;

		//process as lls
		lls_table_t* lls = lls_table_create(udp_packet->data, udp_packet->data_length);
		if(lls) {
			global_stats->packet_counter_lls_packets_parsed++;
			bool should_free_lls = true; //do not free the lls table if we keep a reference in process_lls_table_slt_update
			if(lls->lls_table_id == SLT) {
				global_stats->packet_counter_lls_slt_packets_parsed++;
				//if we have a lls_slt table, and the group is the same but its a new version, reprocess
				if(!lls_session->lls_table_slt ||
					(lls_session->lls_table_slt && lls_session->lls_table_slt->lls_group_id == lls->lls_group_id &&
					lls_session->lls_table_slt->lls_table_version != lls->lls_table_version)) {

					int retval = 0;
					__DEBUG("Beginning processing of SLT from lls_table_slt_update");

					retval = process_lls_table_slt_update(lls);
					should_free_lls = false;

					if(!retval) {
						__DEBUG("lls_table_slt_update -- complete");
					} else {
						global_stats->packet_counter_lls_packets_parsed_error++;
						__ERROR("unable to parse LLS table");
						lls_table_free(lls);

						goto cleanup;
					}
				}
			}
			if(should_free_lls) {
				lls_table_free(lls);
			}
		}

		goto cleanup;
	}


	//ATSC3/331 Section 6.1 - drop non mulitcast ip ranges - e.g not in  239.255.0.0 to 239.255.255.255

	if(udp_packet->dst_ip_addr <= MIN_ATSC3_MULTICAST_BLOCK || udp_packet->dst_ip_addr >= MAX_ATSC3_MULTICAST_BLOCK) {
		//out of range, so drop
		global_stats->packet_counter_filtered_ipv4++;
		global_bandwidth_statistics->interval_filtered_current_bytes_rx += udp_packet->data_length;
		global_bandwidth_statistics->interval_filtered_current_packets_rx++;

		goto cleanup;
	}

	//ALC (ROUTE) - If this flow is registered from the SLT, process it as ALC, otherwise run the flow thru MMT
	if(lls_session->lls_slt_alc_session->alc_session &&	(lls_session->lls_slt_alc_session->sls_relax_source_ip_check || lls_session->lls_slt_alc_session->sls_source_ip_address == udp_packet->src_ip_addr) &&
			lls_session->lls_slt_alc_session->sls_destination_ip_address == udp_packet->dst_ip_addr && lls_session->lls_slt_alc_session->sls_destination_udp_port == udp_packet->dst_port) {

		global_bandwidth_statistics->interval_alc_current_bytes_rx += udp_packet->data_length;
		global_bandwidth_statistics->interval_alc_current_packets_rx++;
		global_stats->packet_counter_alc_recv++;

		if(lls_session->lls_slt_alc_session->alc_session) {
			//re-inject our alc session
			alc_packet_t* alc_packet = NULL;
			alc_channel_t ch;
			ch.s = lls_session->lls_slt_alc_session->alc_session;

			//process ALC streams
			int retval = alc_rx_analyze_packet((char*)udp_packet->data, udp_packet->data_length, &ch, &alc_packet);
			if(!retval) {
				global_stats->packet_counter_alc_packets_parsed++;
				dumpAlcPacketToObect(alc_packet);
				goto cleanup;
			} else {
				__ERROR("Error in ALC decode: %d", retval);
				global_stats->packet_counter_alc_packets_parsed_error++;
				goto cleanup;
			}
		} else {
			__WARN("Have matching ALC session information but ALC client is not active!");
			goto cleanup;
		}
	}

	//Process flow as MMT, we should only have MMT packets left at this point..
	if((dst_ip_addr_filter == NULL && dst_ip_port_filter == NULL) || (udp_packet->dst_ip_addr == *dst_ip_addr_filter && udp_packet->dst_port == *dst_ip_port_filter)) {

		global_bandwidth_statistics->interval_mmt_current_bytes_rx += udp_packet->data_length;
		global_bandwidth_statistics->interval_mmt_current_packets_rx++;
		global_stats->packet_counter_mmtp_packets_received++;

		__TRACE("data len: %d", udp_packet->data_length)
		mmtp_payload_fragments_union_t* mmtp_payload = mmtp_packet_parse(mmtp_sub_flow_vector, udp_packet->data, udp_packet->data_length);

		if(!mmtp_payload) {
			global_stats->packet_counter_mmtp_packets_parsed_error++;
			__ERROR("mmtp_packet_parse: raw packet ptr is null, parsing failed for flow: %d.%d.%d.%d:(%-10u):%-5hu \t ->  %d.%d.%d.%d\t(%-10u)\t:%-5hu",
					ip_header[12], ip_header[13], ip_header[14], ip_header[15], udp_packet->src_ip_addr,
					(uint16_t)((udp_header[0] << 8) + udp_header[1]),
					ip_header[16], ip_header[17], ip_header[18], ip_header[19], udp_packet->dst_ip_addr,
					(uint16_t)((udp_header[2] << 8) + udp_header[3])
					);
			goto cleanup;
		}
		atsc3_packet_statistics_mmt_stats_populate(udp_packet, mmtp_payload);

		//dump header, then dump applicable packet type
		//mmtp_packet_header_dump(mmtp_payload);

		if(mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x0) {
			global_stats->packet_counter_mmt_mpu++;

			if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_timed_flag == 1) {
				global_stats->packet_counter_mmt_timed_mpu++;

				//timed
				//mpu_dump_flow(udp_packet->dst_ip_addr, udp_packet->dst_port, mmtp_payload);
				//mpu_dump_reconstitued(udp_packet->dst_ip_addr, udp_packet->dst_port, mmtp_payload);

			} else {
				//non-timed
				global_stats->packet_counter_mmt_nontimed_mpu++;
			}


		} else if(mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x2) {

			signaling_message_dump(mmtp_payload);
			global_stats->packet_counter_mmt_signaling++;

		} else {
			_MMTP_WARN("mmtp_packet_parse: unknown payload type of 0x%x", mmtp_payload->mmtp_packet_header.mmtp_payload_type);
			global_stats->packet_counter_mmt_unknown++;
			goto cleanup;
		}

		mmtp_payload_fragments_union_free(&mmtp_payload);
	}

cleanup:

	if(udp_packet->data) {
		free(udp_packet->data);
		udp_packet->data = NULL;
	}

	if(udp_packet) {
		free(udp_packet);
		udp_packet = NULL;
	}
}


#define MAX_PCAP_LEN 1514
/**
 *
 * atsc3_mmt_listener_test interface (dst_ip) (dst_port)
 *
 * arguments:
 */
int main(int argc,char **argv) {

	_MPU_DEBUG_ENABLED = 0;
	_MMTP_DEBUG_ENABLED = 0;
	_LLS_DEBUG_ENABLED = 0;

    char *dev;

    char *dst_ip = NULL;
    char *dst_port = NULL;
    int dst_port_filter_int;

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* descr;
    struct bpf_program fp;
    bpf_u_int32 maskp;
    bpf_u_int32 netp;

    //listen to all flows
    if(argc == 2) {
    	dev = argv[1];
    	__INFO("listening on dev: %s", dev);
    } else if(argc==4) {
    	//listen to a selected flow
    	dev = argv[1];
    	dst_ip = argv[2];
    	dst_port = argv[3];

    	dst_ip_addr_filter = calloc(1, sizeof(uint32_t));
    	char* pch = strtok (dst_ip,".");
    	int offset = 24;
    	while (pch != NULL && offset>=0) {
    		uint8_t octet = atoi(pch);
    		*dst_ip_addr_filter |= octet << offset;
    		offset-=8;
    	    pch = strtok (NULL, " ,.-");
    	  }

    	dst_port_filter_int = atoi(dst_port);
    	dst_ip_port_filter = calloc(1, sizeof(uint16_t));
    	*dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;

    	__INFO("listening on dev: %s, dst_ip: %s, dst_port: %s", dev, dst_ip, dst_port);

    } else {
    	println("%s - a udp mulitcast listener test harness for atsc3 mmt messages", argv[0]);
    	println("---");
    	println("args: dev (dst_ip) (dst_port)");
    	println(" dev: device to listen for udp multicast, default listen to 0.0.0.0:0");
    	println(" (dst_ip): optional, filter to specific ip address");
    	println(" (dst_port): optional, filter to specific port");
    	println("");
    	exit(1);
    }
    // mkdir("mpu", 0777);

    /** setup global structs **/

    mmtp_sub_flow_vector = calloc(1, sizeof(*mmtp_sub_flow_vector));
    mmtp_sub_flow_vector_init(mmtp_sub_flow_vector);
    lls_session = lls_session_create();

    global_stats = calloc(1, sizeof(*global_stats));
    gettimeofday(&global_stats->program_timeval_start, 0);

    global_bandwidth_statistics = calloc(1, sizeof(*global_bandwidth_statistics));
	gettimeofday(&global_bandwidth_statistics->program_timeval_start, NULL);

    //create our background thread for bandwidth calculation



    /** ncurses support **/

#ifndef _TEST_RUN_VALGRIND_OSX_

	ncurses_mutext_init();


	//wire up resize handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = handle_winch;
    sigaction(SIGWINCH, &sa, NULL);

    //remap as our printf is redirected to stderr
    my_screen = newterm(NULL, stdout, stdin);
    create_or_update_window_sizes(false);

	pthread_t global_bandwidth_thread_id;
	pthread_create(&global_bandwidth_thread_id, NULL, printBandwidthStatistics, NULL);

	pthread_t global_stats_thread_id;
	pthread_create(&global_stats_thread_id, NULL, printGlobalStatistics, NULL);


#endif



    pcap_lookupnet(dev, &netp, &maskp, errbuf);
    descr = pcap_open_live(dev, MAX_PCAP_LEN, 1, 0, errbuf);

    if(descr == NULL) {
        printf("pcap_open_live(): %s",errbuf);
        exit(1);
    }

    char filter[] = "udp";
    if(pcap_compile(descr,&fp, filter,0,netp) == -1) {
        fprintf(stderr,"Error calling pcap_compile");
        exit(1);
    }

    if(pcap_setfilter(descr,&fp) == -1) {
        fprintf(stderr,"Error setting filter");
        exit(1);
    }

    pcap_loop(descr,-1,process_packet,NULL);

    return 0;
}

/***
 *
 *
 *
	//dispatch for LLS extraction and dump
	#ifdef _SHOW_PACKET_FLOW
		__INFO("--- Packet size : %-10d | Counter: %-8d", udp_packet->data_length, PACKET_COUNTER++);
		__INFO("    Src. Addr   : %d.%d.%d.%d\t(%-10u)\t", ip_header[12], ip_header[13], ip_header[14], ip_header[15], udp_packet->src_ip_addr);
		__INFO("    Src. Port   : %-5hu ", (uint16_t)((udp_header[0] << 8) + udp_header[1]));
		__INFO("    Dst. Addr   : %d.%d.%d.%d\t(%-10u)\t", ip_header[16], ip_header[17], ip_header[18], ip_header[19], udp_packet->dst_ip_addr);
		__INFO("    Dst. Port   : %-5hu \t", (uint16_t)((udp_header[2] << 8) + udp_header[3]));
	#endif
 *
//dump full packet if needed
#ifdef _ENABLE_TRACE
    for (i = 0; i < pkthdr->len; i++) {
        if ((i % 16) == 0) {
            __TRACE("%03x0\t", k);
            k++;
        }
        __TRACE("%02x ", packet[i]);
    }
    __TRACE("*******************************************************");
#endif
 *
	//4294967295
	//1234567890
	__TRACE("Src. Addr  : %d.%d.%d.%d\t(%-10u)\t", ip_header[12], ip_header[13], ip_header[14], ip_header[15], udp_packet->src_ip_addr);
	__TRACE("Src. Port  : %-5hu ", (udp_header[0] << 8) + udp_header[1]);
	__TRACE("Dst. Addr  : %d.%d.%d.%d\t(%-10u)\t", ip_header[16], ip_header[17], ip_header[18], ip_header[19], udp_packet->dst_ip_addr);
	__TRACE("Dst. Port  : %-5hu \t", (udp_header[2] << 8) + udp_header[3]);

	__TRACE("Length\t\t\t\t\t%d", (udp_header[4] << 8) + udp_header[5]);
	__TRACE("Checksum\t\t\t\t0x%02x 0x%02x", udp_header[6], udp_header[7]);
 *
 *
 *
 */
