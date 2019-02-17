/*
 * atsc3_lls_listener_test.c
 *
 *  Created on: Jan 19, 2019
 *      Author: jjustman
 *
 *
 * borrowed from https://stackoverflow.com/questions/26275019/how-to-read-and-send-udp-packets-on-mac-os-x
 * uses libpacp for udp mulicast packet listening
 *
 * opt flags:
  export LDFLAGS="-L/usr/local/opt/libpcap/lib"
  export CPPFLAGS="-I/usr/local/opt/libpcap/include"

  to invoke test driver, run ala:

  ./atsc3_lls_listener_test vnic1 | grep 224.0.23.60

  output should look like


atsc3_lls_listener_test.c:100:DEBUG:Destination Address		239.255.10.4

atsc3_lls_listener_test.c:99:DEBUG:Source Address			192.168.0.4

atsc3_lls_listener_test.c:153:DEBUG:Dst. Address : 224.0.23.60 (3758102332)	Dst. Port    : 4937  	Data length: 193
 */

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
#include <pthread.h>

extern "C" {


#include "atsc3_listener_udp.h"
#include "atsc3_utils.h"

#include "atsc3_lls.h"
#include "atsc3_mmtp_types.h"
#include "atsc3_mmtp_parser.h"
#include "atsc3_mmt_mpu_parser.h"
#include "atsc3_mmt_mpu_utils.h"

#include "atsc3_player_ffplay.h"

}

#include "bento4/MPUtoISOBMFFProcessor.h"

extern int _PLAYER_FFPLAY_DEBUG_ENABLED;
extern int _MPU_DEBUG_ENABLED;
extern int _MMT_MPU_DEBUG_ENABLED;

extern int _MMTP_DEBUG_ENABLED;
extern int _LLS_DEBUG_ENABLED;

#define MAX_PCAP_LEN 1514

//#define _ENABLE_TRACE 1
//#define _SHOW_PACKET_FLOW 1
int PACKET_COUNTER=0;

#define _ENABLE_DEBUG true

#define __ERROR(...)   printf("%s:%d:ERROR:%.4f: ",__FILE__,__LINE__, gt());printf(__VA_ARGS__);printf("\n");
#define __WARN(...)    printf("%s:%d:WARN:%.4f: ",__FILE__,__LINE__,gt());printf(__VA_ARGS__);printf("\n");
#define __INFO(...)    printf("%s:%d:INFO:%.4f: ",__FILE__,__LINE__,gt());printf(__VA_ARGS__);printf("\n");

#ifdef _ENABLE_DEBUG
#define __DEBUG(...)   printf("%s:%d:DEBUG:%.4f: ",__FILE__,__LINE__,gt());__PRINTLN(__VA_ARGS__);
#define __DEBUGF(...)  printf("%s:%d:DEBUG:%.4f: ",__FILE__,__LINE__,gt());__PRINTF(__VA_ARGS__);
#define __DEBUGA(...) 	__PRINTF(__VA_ARGS__);
#define __DEBUGN(...)  __PRINTLN(__VA_ARGS__);
#else
#define __DEBUG(...)
#define __DEBUGF(...)
#define __DEBUGA(...)
#define __DEBUGN(...)
#endif

#ifdef _ENABLE_TRACE
#define __TRACE(...)   printf("%s:%d:TRACE:",__FILE__,__LINE__);__PRINTLN(__VA_ARGS__)

#else
#define __TRACE(...)
#endif

// extern "C" void lls_table_free(lls_table*);

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;

//make sure to invoke     mmtp_sub_flow_vector_init(&p_sys->mmtp_sub_flow_vector);
mmtp_sub_flow_vector_t* mmtp_sub_flow_vector;

//todo - keep track of these by flow and packet_id to detect mpu_sequence_number increment
mmtp_payload_fragments_union_t* mmtp_payload_previous_for_reassembly = NULL;
uint32_t __SEQUENCE_NUMBER_COUNT=0;

pipe_ffplay_buffer_t* pipe_ffplay_buffer = NULL;
uint32_t last_mpu_sequence_number = 0;
uint32_t fragment_count = 0;
void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {

  int i = 0;
  int k = 0;
  u_char ethernet_packet[14];
  u_char ip_header[24];
  u_char udp_header[8];
  int udp_header_start = 34;
  udp_packet_t* udp_packet = NULL;

//dump full packet if needed
#ifdef _ENABLE_TRACE
    for (i = 0; i < pkthdr->len; i++) {
        if ((i % 16) == 0) {
            __TRACE("%03x0\t", k);
            k++;
        }
        __TRACE("%02x ", packet[i]);
    }
#endif
    __TRACE("*******************************************************");

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
	udp_packet = (udp_packet_t*) calloc(1, sizeof(udp_packet_t));
	udp_packet->src_ip_addr = ((ip_header[12] & 0xFF) << 24) | ((ip_header[13]  & 0xFF) << 16) | ((ip_header[14]  & 0xFF) << 8) | (ip_header[15] & 0xFF);
	udp_packet->dst_ip_addr = ((ip_header[16] & 0xFF) << 24) | ((ip_header[17]  & 0xFF) << 16) | ((ip_header[18]  & 0xFF) << 8) | (ip_header[19] & 0xFF);

	for (i = 0; i < 8; i++) {
		udp_header[i] = packet[udp_header_start + i];
	}

	udp_packet->src_port = (udp_header[0] << 8) + udp_header[1];
	udp_packet->dst_port = (udp_header[2] << 8) + udp_header[3];

	udp_packet->data_length = pkthdr->len - (udp_header_start + 8);
	if(udp_packet->data_length <=0 || udp_packet->data_length > 1514) {
		__ERROR("invalid data length of udp packet: %d", udp_packet->data_length);
		return;
	}
	udp_packet->data = (u_char*) malloc(udp_packet->data_length * sizeof(udp_packet->data));
	memcpy(udp_packet->data, &packet[udp_header_start + 8], udp_packet->data_length);

	//inefficient as hell for 1 byte at a time, but oh well...
	#ifdef __ENABLE_TRACE
		for (i = 0; i < udp_packet->data_length; i++) {
			__TRACE("%02x ", packet[udp_header_start + 8 + i]);
		}
	#endif

	#ifdef _SHOW_PACKET_FLOW
		__INFO("--- Packet size : %-10d | Counter: %-8d", udp_packet->data_length, PACKET_COUNTER++);
		__INFO("    Src. Addr   : %d.%d.%d.%d\t(%-10u)\t", ip_header[12], ip_header[13], ip_header[14], ip_header[15], udp_packet->src_ip_addr);
		__INFO("    Src. Port   : %-5hu ", (uint16_t)((udp_header[0] << 8) + udp_header[1]));
		__INFO("    Dst. Addr   : %d.%d.%d.%d\t(%-10u)\t", ip_header[16], ip_header[17], ip_header[18], ip_header[19], udp_packet->dst_ip_addr);
		__INFO("    Dst. Port   : %-5hu \t", (uint16_t)((udp_header[2] << 8) + udp_header[3]));
	#endif

	//dispatch for LLS extraction and dump
	if(udp_packet->dst_ip_addr == LLS_DST_ADDR && udp_packet->dst_port == LLS_DST_PORT) {
		//process as lls
		lls_table_t* lls = lls_table_create(udp_packet->data, udp_packet->data_length);
		if(lls) {
			lls_dump_instance_table(lls);
			lls_table_free(lls);
		} else {
			__ERROR("unable to parse LLS table");
		}
	}


	if(udp_packet->dst_ip_addr <= MIN_ATSC3_MULTICAST_BLOCK || udp_packet->dst_ip_addr >= MAX_ATSC3_MULTICAST_BLOCK) {
		//out of range, so drop

		goto cleanup;
	}

	if((dst_ip_addr_filter == NULL && dst_ip_port_filter == NULL) || (udp_packet->dst_ip_addr == *dst_ip_addr_filter && udp_packet->dst_port == *dst_ip_port_filter)) {

		mmtp_payload_fragments_union_t* mmtp_payload = mmtp_packet_parse(mmtp_sub_flow_vector, udp_packet->data, udp_packet->data_length);

		if(!mmtp_payload) {
			__ERROR("mmtp_packet_parse: raw packet ptr is null, parsing failed for flow: %d.%d.%d.%d:(%-10u):%-5hu \t ->  %d.%d.%d.%d\t(%-10u)\t:%-5hu",
					ip_header[12], ip_header[13], ip_header[14], ip_header[15], udp_packet->src_ip_addr,
					(uint16_t)((udp_header[0] << 8) + udp_header[1]),
					ip_header[16], ip_header[17], ip_header[18], ip_header[19], udp_packet->dst_ip_addr,
					(uint16_t)((udp_header[2] << 8) + udp_header[3])
					);
			goto cleanup;
		}

		//for filtering MMT flows by a specific packet_id
		if(dst_packet_id_filter && *dst_packet_id_filter != mmtp_payload->mmtp_packet_header.mmtp_packet_id) {
			__TRACE("dropping packet id: %d", mmtp_payload->mmtp_packet_header.mmtp_packet_id);
			goto cleanup;
		}
		//dump header, then dump applicable packet type
		//mmtp_packet_header_dump(mmtp_payload);

		if(mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x0) {

			if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_timed_flag == 1) {

				if(mmtp_payload_previous_for_reassembly && mmtp_payload_previous_for_reassembly->mmtp_mpu_type_packet_header.mpu_sequence_number != mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number) {
					__INFO("Starting re-fragmenting because mpu_sequence number changed from %u to %u", mmtp_payload_previous_for_reassembly->mmtp_mpu_type_packet_header.mpu_sequence_number, mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number);

					//reassemble previous segment
					mmtp_sub_flow_t* mmtp_sub_flow = mmtp_sub_flow_vector_get_or_set_packet_id(mmtp_sub_flow_vector, mmtp_payload_previous_for_reassembly->mmtp_packet_header.mmtp_packet_id);

					//hack...mmtp_payload_previous_for_reassembly->mmtp_mpu_type_packet_header.mpu_sequence_number);
					//mpu_data_unit_payload_fragments_t* mpu_metadata_fragments =		mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector, 	mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number);
					mpu_data_unit_payload_fragments_t* mpu_metadata_fragments =		mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data[0];

					mpu_data_unit_payload_fragments_t* movie_metadata_fragments = 	mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->movie_fragment_metadata_vector, mmtp_payload_previous_for_reassembly->mmtp_mpu_type_packet_header.mpu_sequence_number);

					mpu_data_unit_payload_fragments_t* data_unit_payload_types =	mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->media_fragment_unit_vector,		mmtp_payload_previous_for_reassembly->mmtp_mpu_type_packet_header.mpu_sequence_number);

					mmtp_payload_fragments_union_t* mpu_metadata = NULL;
					//at mpu_sequence_number: %u, mpu_metadata packet_id: %d", mmtp_payload_previous_for_reassembly->mmtp_mpu_type_packet_header.mpu_sequence_number, mpu_metadata->mmtp_packet_header.mmtp_packet_id);


//					mpu_metadata_fragments->
					if(mpu_metadata_fragments && mpu_metadata_fragments->timed_fragments_vector.size) {
						__INFO("MPU Metadata: fragments: %p, size: %lu", mpu_metadata_fragments, mpu_metadata_fragments->timed_fragments_vector.size);

						mpu_metadata = mpu_metadata_fragments->timed_fragments_vector.data[0];
						__INFO("MPU Metadata: Found at mpu_sequence_number: %u, mpu_metadata packet_id: %d", mmtp_payload_previous_for_reassembly->mmtp_mpu_type_packet_header.mpu_sequence_number, mpu_metadata->mmtp_packet_header.mmtp_packet_id);

						pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);

						__MMT_MPU_INFO("MPU Metadata: Pushing to Bento4")

						//swap out our p_buffer with bento isobmff processing
						AP4_DataBuffer* cleaned_mpu_metadata = mpuToISOBMFFProcessBoxes(mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer, -1);

						block_Release(&mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
						mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload = block_Alloc(cleaned_mpu_metadata->GetDataSize());
						memcpy(mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, cleaned_mpu_metadata->GetData(), cleaned_mpu_metadata->GetDataSize());
						__MMT_MPU_INFO("MPU Metadata: Got back %d bytes from mpuToISOBMFFProcessBoxes", cleaned_mpu_metadata->GetDataSize());

						mpu_push_to_output_buffer_no_locking(pipe_ffplay_buffer, mpu_metadata);
						delete cleaned_mpu_metadata;

						pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);

					} else {
						__WARN("MPU Metadata is NULL!");
					}

					mmtp_payload_fragments_union_t* fragment_metadata = NULL;
					if(movie_metadata_fragments && movie_metadata_fragments->timed_fragments_vector.size) {
						fragment_metadata = movie_metadata_fragments->timed_fragments_vector.data[0];
						__INFO("Movie Fragment Metadata: Found for mpu_sequence_number: %u, fragment_metadata packet_id: %d", mmtp_payload_previous_for_reassembly->mmtp_mpu_type_packet_header.mpu_sequence_number, fragment_metadata->mmtp_packet_header.mmtp_packet_id);

					} else {
						__WARN("Movie Fragment Metadata is NULL!");
						goto update_previous_mmtp_payload; //for now
					}

					if(!data_unit_payload_types) {
						__WARN("data_unit_payload_types is null!");
						goto update_previous_mmtp_payload;
					}
					mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments = &data_unit_payload_types->timed_fragments_vector;

					if(!data_unit_payload_fragments) {
						__WARN("data_unit_payload_fragments is null!");
						goto update_previous_mmtp_payload;
					}


					int total_fragments = data_unit_payload_fragments->size;

					if(fragment_metadata) {
						pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);

						uint32_t total_mdat_body_size = 0;

						//todo - keep an array of our offsets here if we have sequence gaps...
						//data_unit payload is only fragment_type = 0x2
						for(int i=0; i < total_fragments; i++) {
							mmtp_payload_fragments_union_t* packet = data_unit_payload_fragments->data[i];
							__TRACE("i: %d, p: %p, frag indicator is: %d, mpu_sequence_number: %u, size: %u, packet_counter: %u, data_unit payload: %p, block_t: %p", i, packet, packet->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_indicator, packet->mpu_data_unit_payload_fragments_timed.mpu_sequence_number, packet->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_buffer, packet->mpu_data_unit_payload_fragments_timed.packet_counter, packet->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload, packet->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->p_buffer);

							total_mdat_body_size += packet->mpu_data_unit_payload_fragments_timed.mpu_data_unit_payload->i_buffer;
						}

						AP4_DataBuffer* cleaned_fragment_metadata = mpuToISOBMFFProcessBoxes(fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer, total_mdat_body_size);

						block_Release(&fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
						fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload = block_Alloc(cleaned_fragment_metadata->GetDataSize());
						//AP4_Result tell
						memcpy(fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, cleaned_fragment_metadata->GetData(), cleaned_fragment_metadata->GetDataSize());

						__MMT_MPU_INFO("Fragment Metadata: Got back %d bytes from mpuToISOBMFFProcessBoxes, total mdat box size is: %d", cleaned_fragment_metadata->GetDataSize(), total_mdat_body_size);

						mpu_push_to_output_buffer_no_locking(pipe_ffplay_buffer, fragment_metadata);
						delete cleaned_fragment_metadata;
						pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
					} else {
						//TODO - rebuild media fragment box here
					}

					//push to mpu_push_output_buffer
					pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);
					for(int i=0; i < total_fragments; i++) {
						mmtp_payload_fragments_union_t* packet = data_unit_payload_fragments->data[i];

						mpu_push_to_output_buffer_no_locking(pipe_ffplay_buffer, packet);
					}

					//only trigger a signal if we have our init box and fragment metadata for this cycle
					if(pipe_ffplay_buffer->has_written_init_box && fragment_metadata) {
						__INFO("Calling pipe_buffer_condition_signal");
						pipe_buffer_condition_signal(pipe_ffplay_buffer);
					}
					pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);


					//remove our packets from the subflow and free block allocs, as mpu_push_to_output_buffer_no_locking will copy the p_buffer to a slab
					for(int i=0; i < total_fragments; i++) {
						mmtp_payload_fragments_union_t* packet = data_unit_payload_fragments->data[i];

						__TRACE("freeing payload: %p, packet_counter: %u, packet_sequence_number: %u", mmtp_payload, mmtp_payload->mmtp_mpu_type_packet_header.packet_counter, mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number);
					//???	mmtp_payload_fragment_remove_from_subflow_and_free_packet(mmtp_sub_flow_vector, packet);
					}
				}


			} else {
				//non-timed
			}

update_previous_mmtp_payload:

			mmtp_payload_previous_for_reassembly = mmtp_payload;

			} else if(mmtp_payload->mmtp_packet_header.mmtp_payload_type == 0x2) {

			signaling_message_dump(mmtp_payload);

		} else {
			_MMTP_WARN("mmtp_packet_parse: unknown payload type of 0x%x", mmtp_payload->mmtp_packet_header.mmtp_payload_type);
			goto cleanup;
		}
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


void* pcap_loop_run_thread(void* dev_pointer) {
	char* dev = (char*) dev_pointer;

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* descr;
	struct bpf_program fp;
	bpf_u_int32 maskp;
	bpf_u_int32 netp;

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

#define MAX_PCAP_LEN 1514
/**
 *
 * atsc3_mmt_listener_test interface (dst_ip) (dst_port)
 *
 * arguments:
 */
int main(int argc,char **argv) {

    char *dev;

    char *filter_dst_ip = "";
    char *filter_dst_port = "";
    char *filter_packet_id = "";

    int dst_port_filter_int;
    int dst_ip_port_filter_int;
    int dst_packet_id_filter_int;

    _MMT_MPU_DEBUG_ENABLED = 1;
    _PLAYER_FFPLAY_DEBUG_ENABLED = 1;
    _MMTP_DEBUG_ENABLED = 0;
    _MPU_DEBUG_ENABLED = 0;
    _LLS_DEBUG_ENABLED = 0;

    //listen to all flows
    if(argc == 2) {
    	dev = argv[1];
    	__INFO("listening on dev: %s", dev);
    } else if(argc>=4) {

    	//listen to a selected flow
		dev = argv[1];
		filter_dst_ip = argv[2];

		//skip ip address filter if our params are * or -
		if(!(strncmp("*", filter_dst_ip, 1) == 0 || strncmp("-", filter_dst_ip, 1) == 0)) {
			dst_ip_addr_filter = (uint32_t*) calloc(1, sizeof(uint32_t));
			char* pch = strtok (filter_dst_ip,".");
			int offset = 24;
			while (pch != NULL && offset>=0) {
				uint8_t octet = atoi(pch);
				*dst_ip_addr_filter |= octet << offset;
				offset-=8;
				pch = strtok (NULL, ".");
			}
		}

		if(argc>=4) {
			filter_dst_port = argv[3];
			if(!(strncmp("*", filter_dst_port, 1) == 0 || strncmp("-", filter_dst_port, 1) == 0)) {
				__INFO("832:");

				dst_port_filter_int = atoi(filter_dst_port);
				dst_ip_port_filter = (uint16_t*)  calloc(1, sizeof(uint16_t));
				*dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;
			}
		}

		if(argc>=5) {
			filter_packet_id = argv[4];
			if(!(strncmp("*", filter_packet_id, 1) == 0 || strncmp("-", filter_packet_id, 1) == 0)) {
				dst_packet_id_filter_int = atoi(filter_packet_id);
				dst_packet_id_filter = (uint16_t*) calloc(1, sizeof(uint16_t));
				*dst_packet_id_filter |= dst_packet_id_filter_int & 0xFFFF;
			}
		}

		__INFO("listening on dev: %s, dst_ip: %s (%p), dst_port: %s (%p), dst_packet_id: %s (%p)", dev, filter_dst_ip, dst_ip_addr_filter, filter_dst_port, dst_ip_port_filter, filter_packet_id, dst_packet_id_filter);

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
    mmtp_sub_flow_vector = (mmtp_sub_flow_vector_t*)calloc(1, sizeof(mmtp_sub_flow_vector_t));
    mmtp_sub_flow_vector_init(mmtp_sub_flow_vector);

    mkdir("mpu", 0777);

    pipe_ffplay_buffer = pipe_create_ffplay();

#ifndef _TEST_RUN_VALGRIND_OSX_

	pthread_t global_pcap_thread_id;
	int pcap_ret = pthread_create(&global_pcap_thread_id, NULL, pcap_loop_run_thread, (void*)dev);
	assert(!pcap_ret);

	pthread_join(global_pcap_thread_id, NULL);

#else
    pcap_loop_run(dev);
#endif


    return 0;
}

/**
 *
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
 */



