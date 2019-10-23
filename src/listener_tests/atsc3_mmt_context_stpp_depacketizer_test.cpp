/*
 * atsc3_mmt_context_stpp_depacketizer_test.c
 *
 *  Created on: Oct 19, 2019
 *      Author: jjustman
 *
 * sample listener for MMT stpp (IMSC-1) caption data
 */

int PACKET_COUNTER=0;

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <ncurses.h>
#include <limits.h>

#include "../atsc3_listener_udp.h"
#include "../atsc3_utils.h"

#include "../atsc3_lls.h"

#include "../atsc3_lls_slt_parser.h"
#include "../atsc3_lls_sls_monitor_output_buffer_utils.h"

#include "../atsc3_mmtp_packet_types.h"
#include "../atsc3_mmtp_parser.h"
#include "../atsc3_mmtp_ntp32_to_pts.h"
#include "../atsc3_mmt_mpu_utils.h"

#include "../atsc3_logging_externs.h"

#include "../atsc3_mmt_context_mfu_depacketizer.h"

//commandline stream filtering per flow

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;
uint16_t* dst_packet_id_filter = NULL;


//jjustman-2019-09-18: refactored MMTP flow collection management
mmtp_flow_t* mmtp_flow;

//todo: jjustman-2019-09-18 refactor me out for mpu recon persitance
udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container;

lls_slt_monitor_t* lls_slt_monitor;

lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;

//jjustman-2019-10-03 - context event callbacks...
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context;

//stpp packet_id under test
uint16_t atsc3_mmt_context_stpp_packet_id_for_testing = 19;

void atsc3_mmt_signalling_information_on_stpp_essence_packet_id_dump(uint16_t stpp_packet_id) {
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_stpp_essence_packet_id_dump: stpp mp_table packet_id: %u, stpp_packet_id_under_test: %u",
			stpp_packet_id,
			atsc3_mmt_context_stpp_packet_id_for_testing);
}

void atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_dump(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_dump: stpp mp_table packet_id: %u, stpp_packet_id_under_test: %u, mpu_sequence_number: %d, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
				stpp_packet_id,
				atsc3_mmt_context_stpp_packet_id_for_testing,
				mpu_sequence_number,
				mpu_presentation_time_ntp64,
				mpu_presentation_time_seconds,
				mpu_presentation_time_microseconds);
}



void atsc3_mmt_mpu_mfu_on_sample_complete_dump(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_rebuilt) {
	if(packet_id != atsc3_mmt_context_stpp_packet_id_for_testing) {
		return;
	}

	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_complete_dump: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d",
			packet_id,
            mpu_sequence_number,
            sample_number,
			mmt_mfu_sample,
			mmt_mfu_sample->p_size);

    char myFileName[128];
    snprintf(myFileName, 128, "mpu/%d.%d.%d.mfu.complete", packet_id, mpu_sequence_number, sample_number);
    FILE* fp = fopen(myFileName, "w");
    if(fp) {
    	block_Rewind(mmt_mfu_sample);
    	fwrite(block_Get(mmt_mfu_sample), mmt_mfu_sample->p_size, 1, fp);
    	fclose(fp);
    } else {
    	  __MMT_CONTEXT_MPU_DEBUG("ERROR writing sample to complete filename: %s, atsc3_mmt_mpu_mfu_on_sample_complete_dump: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d",
    			    myFileName,
				  	packet_id,
    	            mpu_sequence_number,
    	            sample_number,
    				mmt_mfu_sample,
    				mmt_mfu_sample->p_size);

    }
}

void atsc3_mmt_mpu_mfu_on_sample_corrupt_dump(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
	if(packet_id != atsc3_mmt_context_stpp_packet_id_for_testing) {
		return;
	}
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_corrupt_dump: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d",
				packet_id,
	            mpu_sequence_number,
	            sample_number,
				mmt_mfu_sample,
				mmt_mfu_sample->p_size);

	char myFileName[128];
	snprintf(myFileName,  128, "mpu/%d.%d.%d.mfu.corrupt", packet_id, mpu_sequence_number, sample_number);
	FILE* fp = fopen(myFileName, "w");
	if(fp) {
		block_Rewind(mmt_mfu_sample);
		fwrite(block_Get(mmt_mfu_sample), mmt_mfu_sample->p_size, 1, fp);
		fclose(fp);
	} else {
		  __MMT_CONTEXT_MPU_DEBUG("ERROR writing sample to corrupt filename: %s, atsc3_mmt_mpu_mfu_on_sample_complete_dump: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d",
					myFileName,
					packet_id,
					mpu_sequence_number,
					sample_number,
					mmt_mfu_sample,
					mmt_mfu_sample->p_size);

	}
}



void atsc3_mmt_mpu_mfu_on_sample_missing_dump(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
	if(packet_id != atsc3_mmt_context_stpp_packet_id_for_testing) {
		return;
	}
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_complete_dump: packet_id: %u, mpu_sequence_number: %u, sample_number: %u",
				packet_id,
	            mpu_sequence_number,
	            sample_number);

	char myFileName[128];
	snprintf(myFileName,  128, "mpu/%d.%d.%d.mfu.missing", packet_id, mpu_sequence_number, sample_number);
	FILE* fp = fopen(myFileName, "w");
	if(fp) {

		fwrite("x", 1, 1, fp);
		fclose(fp);
	} else {
		  __MMT_CONTEXT_MPU_DEBUG("ERROR writing sample to missing filename: %s, atsc3_mmt_mpu_mfu_on_sample_complete_dump: packet_id: %u, mpu_sequence_number: %u, sample_number: %u",
					myFileName,
					packet_id,
					mpu_sequence_number,
					sample_number);

	}
}

mmtp_packet_header_t*  mmtp_parse_header_from_udp_packet(udp_packet_t* udp_packet) {

	mmtp_packet_header_t* mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);

    if(!mmtp_packet_header) {
        __ERROR("mmtp_parse_header_from_udp_packet: mmtp_packet_header_parse_from_block_t: raw packet ptr is null, parsing failed for flow: %d.%d.%d.%d:(%-10u):%-5u \t ->  %d.%d.%d.%d:(%-10u):%-5u ",
                __toipandportnonstruct(udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.src_port),
                udp_packet->udp_flow.src_ip_addr,
                __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port),
                udp_packet->udp_flow.dst_ip_addr);
        return NULL;
    }

    return mmtp_packet_header;
}

void mmtp_process_sls_from_payload(udp_packet_t *udp_packet, mmtp_signalling_packet_t* mmtp_signalling_packet, lls_sls_mmt_session_t* matching_lls_slt_mmt_session) {

	__INFO("mmtp_process_sls_from_payload: processing mmt flow: %d.%d.%d.%d:(%u) packet_id: %d, signalling message: %p",
			__toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port),
			mmtp_signalling_packet->mmtp_packet_id,
			mmtp_signalling_packet);

	mmt_signalling_message_dump(mmtp_signalling_packet);
}

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	mmtp_packet_header_t* mmtp_packet_header = NULL;

	udp_packet_t* udp_packet = process_packet_from_pcap(user, pkthdr, packet);
	if(!udp_packet) {
		return;
	}

	//drop mdNS
	if(udp_packet->udp_flow.dst_ip_addr == UDP_FILTER_MDNS_IP_ADDRESS && udp_packet->udp_flow.dst_port == UDP_FILTER_MDNS_PORT) {
		return udp_packet_free(&udp_packet);
	}

	if(udp_packet->udp_flow.dst_ip_addr == LLS_DST_ADDR && udp_packet->udp_flow.dst_port == LLS_DST_PORT) {
		//auto-monitor code here for MMT
		//process as lls.sst, dont free as we keep track of our object in the lls_slt_monitor
		lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor, udp_packet->data);
		if(lls_table) {
			if(lls_table->lls_table_id == SLT) {
				int retval = lls_slt_table_perform_update(lls_table, lls_slt_monitor);

				if(!retval) {
					lls_dump_instance_table(lls_table);
					for(int i=0; i < lls_table->slt_table.atsc3_lls_slt_service_v.count; i++) {
						atsc3_lls_slt_service_t* atsc3_lls_slt_service = lls_table->slt_table.atsc3_lls_slt_service_v.data[i];
						if(atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count && atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol == SLS_PROTOCOL_MMTP) {
							if(lls_sls_mmt_monitor) {
								//re-configure
							} else {
								//TODO:  make sure
								//lls_service->broadcast_svc_signaling.sls_destination_ip_address && lls_service->broadcast_svc_signaling.sls_destination_udp_port
								//match our dst_ip_addr_filter && udp_packet->udp_flow.dst_ip_addr != *dst_ip_addr_filter and port filter
								__INFO("Adding service: %d", atsc3_lls_slt_service->service_id);

								lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();
								lls_sls_mmt_monitor->atsc3_lls_slt_service = atsc3_lls_slt_service; //HACK!
								lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);

								lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

								//we may not be initialized yet, so re-check again later
								//this should _never_happen...
								lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);
								if(!lls_sls_mmt_session) {
									__WARN("lls_slt_mmt_session_find_from_service_id: lls_sls_mmt_session is NULL!");
								}
								lls_sls_mmt_monitor->lls_mmt_session = lls_sls_mmt_session;
								lls_slt_monitor->lls_sls_mmt_monitor = lls_sls_mmt_monitor;

								lls_slt_monitor_add_lls_sls_mmt_monitor(lls_slt_monitor, lls_sls_mmt_monitor);
							}
						}
					}
				}
			}
		}

		__TRACE("Checking lls_sls_mmt_monitor: %p,", lls_sls_mmt_monitor);

		if(lls_sls_mmt_monitor && lls_sls_mmt_monitor->lls_mmt_session) {
			__TRACE("Checking lls_sls_mmt_monitor->lls_mmt_session: %p,", lls_sls_mmt_monitor->lls_mmt_session);
		}

		//recheck video_packet_id/audio_packet_id
		if(lls_sls_mmt_monitor && lls_sls_mmt_monitor->lls_mmt_session) {
			if(!lls_sls_mmt_monitor->video_packet_id) {
				lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, lls_sls_mmt_monitor->lls_mmt_session->service_id);
				lls_sls_mmt_monitor->video_packet_id = lls_sls_mmt_session->video_packet_id;
				lls_sls_mmt_monitor->audio_packet_id = lls_sls_mmt_session->audio_packet_id;
				lls_sls_mmt_monitor->stpp_packet_id  = lls_sls_mmt_session->stpp_packet_id;
				__INFO("service: %d, setting audio_packet_id/video_packet_id/stpp: %u, %u, %u",
						lls_sls_mmt_session->atsc3_lls_slt_service->service_id,
						lls_sls_mmt_monitor->audio_packet_id,
						lls_sls_mmt_monitor->video_packet_id,
						lls_sls_mmt_monitor->stpp_packet_id);
			}
		}
		return udp_packet_free(&udp_packet);
	}

    if((dst_ip_addr_filter && udp_packet->udp_flow.dst_ip_addr != *dst_ip_addr_filter)) {
        return udp_packet_free(&udp_packet);
    }


    //TODO: jjustman-2019-10-03 - packet header parsing to dispatcher mapping
    lls_sls_mmt_session_t* matching_lls_sls_mmt_session = lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor, udp_packet->udp_flow.src_ip_addr, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
	__TRACE("Checking matching_lls_sls_mmt_session: %p,", matching_lls_sls_mmt_session);

	if(matching_lls_sls_mmt_session && lls_slt_monitor && lls_slt_monitor->lls_sls_mmt_monitor && matching_lls_sls_mmt_session->atsc3_lls_slt_service->service_id == lls_slt_monitor->lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id) {

		mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);
        
		if(!mmtp_packet_header) {
			return udp_packet_free(&udp_packet);
		}

		//for filtering MMT flows by a specific packet_id
		if(dst_packet_id_filter && *dst_packet_id_filter != mmtp_packet_header->mmtp_packet_id) {
			goto cleanup;
		}

		mmtp_packet_header_dump(mmtp_packet_header);

		//dump header, then dump applicable packet type
		if(mmtp_packet_header->mmtp_payload_type == 0x0) {
			//bail if we are not the MFU packet_id under test, but allow SLS/signalling messages to passthru

			if(mmtp_packet_header->mmtp_packet_id != atsc3_mmt_context_stpp_packet_id_for_testing) {
				//discard
				goto cleanup;
			}

            mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
            if(!mmtp_mpu_packet) {
                goto error;
            }
            
			if(mmtp_mpu_packet->mpu_timed_flag == 1) {
				mmtp_mpu_dump_header(mmtp_mpu_packet);

				//TODO: jjustman-2019-10-03 - handle context parameters better
				// mmtp_flow, lls_slt_monitor, , udp_flow_latest_mpu_sequence_number_container, matching_lls_sls_mmt_session);

				atsc3_mmt_mfu_context->mmtp_flow = mmtp_flow;
				atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container;
				atsc3_mmt_mfu_context->lls_slt_monitor = lls_slt_monitor;
				atsc3_mmt_mfu_context->matching_lls_sls_mmt_session = matching_lls_sls_mmt_session;

				__TRACE("process_packet: mmtp_mfu_process_from_payload_with_context with udp_packet: %p, mmtp_mpu_packet: %p, atsc3_mmt_mfu_context: %p,",
						udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);

				mmtp_mfu_process_from_payload_with_context(udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);

			} else {
				//non-timed
				__ATSC3_WARN("process_packet: mmtp_packet_header_parse_from_block_t - non-timed payload: packet_id: %u", mmtp_packet_header->mmtp_packet_id);
			}
		} else if(mmtp_packet_header->mmtp_payload_type == 0x2) {

			mmtp_signalling_packet_t* mmtp_signalling_packet = mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
			uint8_t parsed_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, udp_packet->data);
			if(parsed_count) {
				mmt_signalling_message_dump(mmtp_signalling_packet);

				__TRACE("process_packet: calling mmt_signalling_message_process_with_context with udp_packet: %p, mmtp_signalling_packet: %p, atsc3_mmt_mfu_context: %p,",
						udp_packet,
						mmtp_signalling_packet,
						atsc3_mmt_mfu_context);

				mmt_signalling_message_process_with_context(udp_packet, mmtp_signalling_packet, atsc3_mmt_mfu_context);


				//internal auto-selection service_id/packet_id triggering hacks below

				//TODO: jjustman-2019-10-03 - if signalling_packet == MP_table, set atsc3_mmt_mfu_context->mp_table_last;
				mmtp_asset_flow_t* mmtp_asset_flow = mmtp_flow_find_or_create_from_udp_packet(mmtp_flow, udp_packet);
				mmtp_asset_t* mmtp_asset = mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow, matching_lls_sls_mmt_session);

				//TODO: FIX ME!!! HACK - jjustman-2019-09-05
				mmtp_mpu_packet_t* mmtp_mpu_packet = mmtp_mpu_packet_new();
				mmtp_mpu_packet->mmtp_packet_id = mmtp_signalling_packet->mmtp_packet_id;

				mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container = mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset, mmtp_mpu_packet);
				mmtp_packet_id_packets_container_add_mmtp_signalling_packet(mmtp_packet_id_packets_container, mmtp_signalling_packet);

				//TODO: FIX ME!!! HACK - jjustman-2019-09-05
				mmtp_mpu_packet_free(&mmtp_mpu_packet);

				//update our sls_mmt_session info
				mmt_signalling_message_update_lls_sls_mmt_session(mmtp_signalling_packet, matching_lls_sls_mmt_session);

				//TODO - remap this
				//add in flows 				lls_sls_mmt_session_t* lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, lls_sls_mmt_monitor->lls_mmt_session->service_id);

				if(lls_sls_mmt_monitor && lls_sls_mmt_monitor->lls_mmt_session && matching_lls_sls_mmt_session) {
					__INFO("mmt_signalling_information: from atsc3 service_id: %u, patching: seting audio_packet_id/video_packet_id/stpp_packet_id: %u, %u, %u",
							matching_lls_sls_mmt_session->atsc3_lls_slt_service->service_id,
							matching_lls_sls_mmt_session->audio_packet_id,
							matching_lls_sls_mmt_session->video_packet_id,
							matching_lls_sls_mmt_session->stpp_packet_id);

					if(matching_lls_sls_mmt_session->audio_packet_id) {
						lls_sls_mmt_monitor->audio_packet_id = matching_lls_sls_mmt_session->audio_packet_id;
					}

					if(matching_lls_sls_mmt_session->video_packet_id) {
						lls_sls_mmt_monitor->video_packet_id = matching_lls_sls_mmt_session->video_packet_id;
					}

					if(matching_lls_sls_mmt_session->stpp_packet_id) {
						lls_sls_mmt_monitor->stpp_packet_id = matching_lls_sls_mmt_session->stpp_packet_id;
					}
				}
			}

		} else {
			__ATSC3_WARN("process_packet: mmtp_packet_header_parse_from_block_t - unknown payload type of 0x%x", mmtp_packet_header->mmtp_payload_type);
			goto cleanup;
		}
	}

cleanup:
	if(mmtp_packet_header) {
		mmtp_packet_header_free(&mmtp_packet_header);
	}
    return;

error:
    __ATSC3_WARN("process_packet: error, bailing loop!");
    return;
}


void* pcap_loop_run_thread(void* dev_pointer) {
	char* dev = (char*) dev_pointer;

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* descr;
	struct bpf_program fp;
	bpf_u_int32 maskp;
	bpf_u_int32 netp;

	pcap_lookupnet(dev, &netp, &maskp, errbuf);
    descr = pcap_open_live(dev, MAX_PCAP_LEN, 1, 1, errbuf);

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


/**
 *
 * atsc3_mmt_listener_test interface (dst_ip) (dst_port)
 *
 * arguments:
 */
int main(int argc,char **argv) {

    _MMT_CONTEXT_MPU_DEBUG_ENABLED = 1;

#ifdef __lots_of_logging_
    _LLS_DEBUG_ENABLED = 1;

    _MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED = 1;
    _MMT_SIGNALLING_MESSAGE_TRACE_ENABLED = 1;

    _LLS_SLT_PARSER_INFO_MMT_ENABLED = 1;
    _LLS_MMT_UTILS_TRACE_ENABLED = 1;
    
    _MMTP_DEBUG_ENABLED = 1;
    _MMT_MPU_PARSER_DEBUG_ENABLED = 1;

    _MMT_CONTEXT_MPU_DEBUG_ENABLED = 1;
    _MMT_CONTEXT_MPU_SIGNAL_INFO_ENABLED = 1;

#endif
    
    
    char *dev;

    char *filter_dst_ip = NULL;
    char *filter_dst_port = NULL;
    char *filter_packet_id = NULL;

    int dst_port_filter_int;
    int dst_ip_port_filter_int;
    int dst_packet_id_filter_int;

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
			dst_ip_addr_filter = (uint32_t*)calloc(1, sizeof(uint32_t));
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

				dst_port_filter_int = atoi(filter_dst_port);
				dst_ip_port_filter = (uint16_t*)calloc(1, sizeof(uint16_t));
				*dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;
        	}
    	}

    	if(argc>=5) {
    		filter_packet_id = argv[4];
        	if(!(strncmp("*", filter_packet_id, 1) == 0 || strncmp("-", filter_packet_id, 1) == 0)) {
				dst_packet_id_filter_int = atoi(filter_packet_id);
				dst_packet_id_filter = (uint16_t*)calloc(1, sizeof(uint16_t));
				*dst_packet_id_filter |= dst_packet_id_filter_int & 0xFFFF;
        	}
    	}

    	__INFO("listening on dev: %s, dst_ip: %s (%p), dst_port: %s (%p), dst_packet_id: %s (%p)", dev, filter_dst_ip, dst_ip_addr_filter, filter_dst_port, dst_ip_port_filter, filter_packet_id, dst_packet_id_filter);


    } else {
    	println("%s - a udp mulitcast listener test harness for atsc3 mmt stpp emissions", argv[0]);
    	println("---");
    	println("args: dev (dst_ip) (dst_port) (packet_id)");
    	println(" dev: device to listen for udp multicast, default listen to 0.0.0.0:0");
    	println(" (dst_ip): optional, filter to specific ip address");
    	println(" (dst_port): optional, filter to specific port");

    	println("");
    	exit(1);
    }

    /** setup global structs **/

    lls_slt_monitor = lls_slt_monitor_create();
    mmtp_flow = mmtp_flow_new();
    udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container_t_init();

    //callback contexts
    atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_new();

    //stpp SLS related callbacks from mp_table
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_essence_packet_id = &atsc3_mmt_signalling_information_on_stpp_essence_packet_id_dump;
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_dump;


	//MFU related callbacks
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete = &atsc3_mmt_mpu_mfu_on_sample_complete_dump;
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt 	= &atsc3_mmt_mpu_mfu_on_sample_corrupt_dump;
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_missing 	= &atsc3_mmt_mpu_mfu_on_sample_missing_dump;




#ifndef _TEST_RUN_VALGRIND_OSX_

	pthread_t global_pcap_thread_id;
	int pcap_ret = pthread_create(&global_pcap_thread_id, NULL, pcap_loop_run_thread, (void*)dev);
	assert(!pcap_ret);


	pthread_join(global_pcap_thread_id, NULL);

#else
	pcap_loop_run_thread(dev);
#endif


    return 0;
}




