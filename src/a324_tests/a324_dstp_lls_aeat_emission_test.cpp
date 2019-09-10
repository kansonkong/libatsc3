/*
 * a324_dstp_lls_aeat_emission_test.cpp
 *
 *  Created on: Sept 09, 2019
 *      Author: jjustman
 *
 * a324 dstp lls aeat emission test harness
 * ATSC A/324:2018 Scheduler / Studio to Transmitter Link 5 January 2018
 *
 * build a LLS AEAT payload message via DSTP with wakeup bit(s) assigned
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


#include "../atsc3_listener_udp.h"
#include "../atsc3_lls.h"
#include "../atsc3_stltp_parser.h"
#include "../atsc3_alp_parser.h"
#include "../atsc3_logging_externs.h"

#include "../atsc3_dstp_types.h"


FILE* __DEBUG_LOG_FILE = NULL;
bool  __DEBUG_LOG_AVAILABLE = true;

//overload printf to write to stderr
int printf(const char *format, ...)  {
    
    if(__DEBUG_LOG_AVAILABLE && !__DEBUG_LOG_FILE) {
        __DEBUG_LOG_FILE = fopen("debug.log", "w");
        if(!__DEBUG_LOG_FILE) {
            __DEBUG_LOG_AVAILABLE = false;
            __DEBUG_LOG_FILE = stderr;
        }
    }
    
    va_list argptr;
    va_start(argptr, format);
    vfprintf(__DEBUG_LOG_FILE, format, argptr);
    va_end(argptr);
    fflush(__DEBUG_LOG_FILE);
    return 0;
}


int PACKET_COUNTER = 0;

uint32_t* dst_ip_addr_filter = NULL;
uint16_t* dst_ip_port_filter = NULL;

extern pcap_t* descrInject;


int main(int argc,char **argv) {
    char *devInject;
    char *xmlFilename;

	char errbuf[PCAP_ERRBUF_SIZE];
	char errbufInject[PCAP_ERRBUF_SIZE];

	pcap_t* descr;
	struct bpf_program fp;
	bpf_u_int32 maskp;
	bpf_u_int32 netp;

	struct bpf_program fpInject;
	bpf_u_int32 maskpInject;
	bpf_u_int32 netpInject;

    if(argc != 3) {
        println("%s - an atsc3 a324 LLS AEAT DSTP emitter", argv[0]);
        println("---");
        println("args: devInject aeat.xml");
        println(" devInject : device to inject for ALP IP reflection");
        println(" aeat.xml  : aeat to inject xml");

        exit(1);
    } else {
        devInject = argv[1];
        xmlFilename = argv[2];
    }

    //inject
	pcap_lookupnet(devInject, &netpInject, &maskpInject, errbufInject);
	pcap_t* descrInject = pcap_open_live(devInject, MAX_PCAP_LEN, 1, 0, errbufInject);

	struct stat st;
	stat(xmlFilename, &st);
	off_t size = st.st_size;

	if(size > 0) {
		FILE* fp = fopen(xmlFilename, "r");
		if(fp) {
			udp_flow_t flow;
			flow.src_ip_addr = (192 << 24) | (168 << 16) | (254 << 8) | 100;
			flow.src_port = 31337;

			flow.dst_ip_addr = LLS_DST_ADDR;
			flow.dst_port = LLS_DST_PORT;

			atsc3_ip_udp_rtp_dstp_packet_t* atsc3_ip_udp_rtp_dstp_packet = atsc3_ip_udp_rtp_dstp_packet_new_from_flow(&flow);

			uint8_t* block = (uint8_t*) calloc(size, sizeof(uint8_t));
			fread(block, size, 1, fp);
			//build our ad-hoc LLS table here
			uint8_t lls_table_header[4];
			lls_table_header[0] = 0x04;
			lls_table_header[1] = 0;
			lls_table_header[2] = 0;
			lls_table_header[3] = 1;

			block_Write(atsc3_ip_udp_rtp_dstp_packet->data, &lls_table_header[0], 4);
			block_Write(atsc3_ip_udp_rtp_dstp_packet->data, block, size);
			free(block);

			atsc3_ip_udp_rtp_dstp_packet->rtp_header->payload_type.wakeup_control.aeat_wakeup_alert = 1;
			atsc3_ip_udp_rtp_dstp_packet->rtp_header->payload_type.wakeup_control.wakeup_active = 1;

			block_t* final_eth_payload = atsc3_ip_udp_rtp_dstp_write_to_eth_phy_packet_block_t(atsc3_ip_udp_rtp_dstp_packet);
			block_Rewind(final_eth_payload);

		    if (pcap_sendpacket(descrInject, block_Get(final_eth_payload), final_eth_payload->p_size) != 0) {
		    	__ERROR("error sending the packet: %s", pcap_geterr(descrInject));
		    } else {
		    	__INFO("sent packet %p, len: %d", block_Get(final_eth_payload), final_eth_payload->p_size);
		    }

		    free(final_eth_payload);

			fclose(fp);
		}


	} else {
		__ERROR("Unable to open file: %s", xmlFilename);
	}



	return 0;

}

