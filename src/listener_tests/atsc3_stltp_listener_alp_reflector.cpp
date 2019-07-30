/*
 * atsc3_stltp_listener_alp_reflector.c
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 *
 * stltp listener for atsc a/324
 *
 * */
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

#include "../atsc3_listener_udp.h"
#include "../atsc3_stltp_parser.h"
#include "../atsc3_alp_parser.h"
#include "../atsc3_logging_externs.h"


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

atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_processed = NULL;
atsc3_alp_packet_collection_t* atsc3_alp_packet_collection = NULL;

void process_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    //extract our outer ip/udp/rtp packet
    atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet = atsc3_ip_udp_rtp_process_packet_from_pcap(user, pkthdr, packet);
    if(!ip_udp_rtp_packet) {
        return;
    }
    //TODO - add SMPTE-2022.1 FEC decoding (see fork of prompeg-decoder - https://github.com/jjustman/prompeg-decoder)
    
    //dispatch for STLTP decoding and reflection
    if(ip_udp_rtp_packet->udp_flow.dst_ip_addr == *dst_ip_addr_filter && ip_udp_rtp_packet->udp_flow.dst_port == *dst_ip_port_filter) {
        atsc3_stltp_tunnel_packet_processed = atsc3_stltp_raw_packet_extract_inner_from_outer_packet(ip_udp_rtp_packet, atsc3_stltp_tunnel_packet_processed);
        
        if(atsc3_stltp_tunnel_packet_processed) {
            if(atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.count) {
                __INFO(">>>stltp atsc3_stltp_baseband_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.count);
                
                for(int i=0; i < atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.count; i++) {
                    atsc3_alp_packet_t* atsc3_alp_packet = NULL;
                    atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet = atsc3_stltp_tunnel_packet_processed->atsc3_stltp_baseband_packet_v.data[i];
                    __INFO("atsc3_baseband_packet: port: %d", atsc3_stltp_baseband_packet->ip_udp_rtp_packet->udp_flow.dst_port);

                    //make sure we get a packet back, base field pointer (13b) : 0x1FFF (8191 bytes) will return NULL
                    atsc3_baseband_packet_t* atsc3_baseband_packet = atsc3_stltp_parse_baseband_packet(atsc3_stltp_baseband_packet);
                    if(atsc3_baseband_packet) {
                        
                        //if we have a pending packet and a block before our payload pointer
                        if(atsc3_alp_packet_collection->atsc3_alp_packet_pending && atsc3_baseband_packet->alp_payload_pre_pointer) {
                            //merge block_t pre_pointer by computing the difference...
                            uint32_t remaining_packet_pending_bytes = block_Remaining_size(atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_payload);
                            uint32_t remaining_baseband_frame_bytes = block_Remaining_size(atsc3_baseband_packet->alp_payload_pre_pointer);
                            __INFO("atsc3_baseband_packet: alp_packet_pending: size: %d, fragment remaining bytes: %d, bb pre_pointer frame bytes remaining: %d, bb pre_pointer size: %d",
                                   atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_payload->p_size,
                                   remaining_packet_pending_bytes,
                                   remaining_baseband_frame_bytes,
                                   atsc3_baseband_packet->alp_payload_pre_pointer->p_size);

                            //we may overrun this clamping...
                            block_Seek(atsc3_baseband_packet->alp_payload_pre_pointer, __MIN(remaining_packet_pending_bytes, remaining_baseband_frame_bytes));
                            
                            atsc3_alp_packet = atsc3_alp_packet_collection->atsc3_alp_packet_pending;
                            block_Append(atsc3_alp_packet->alp_payload, atsc3_baseband_packet->alp_payload_pre_pointer);
                            uint32_t final_alp_packet_short_bytes_remaining = block_Remaining_size(atsc3_alp_packet_collection->atsc3_alp_packet_pending->alp_payload);

                            if(final_alp_packet_short_bytes_remaining) {
                                if(atsc3_baseband_packet->alp_payload_post_pointer) {
                                    //ignore and discard, as our base pointer should have contained this overrun
                                    __WARN("atsc3_baseband_packet: discarding pending packet: short and no post pointer: %d bytes still remaining", final_alp_packet_short_bytes_remaining);
                                    atsc3_alp_packet_collection->atsc3_alp_packet_pending = NULL;
                                    atsc3_alp_packet = NULL;

                                } else {
                                    __INFO("atsc3_baseband_packet: carry over pending packet: short:  %d bytes still remaining", final_alp_packet_short_bytes_remaining);
                                    atsc3_alp_packet_collection->atsc3_alp_packet_pending = atsc3_alp_packet;
                                    atsc3_alp_packet->is_alp_payload_complete  = false;
                                }
                            } else {
                                //packet is complete after refragmenting
                                atsc3_alp_packet->is_alp_payload_complete = true;
                                atsc3_alp_packet_collection_add_atsc3_alp_packet(atsc3_alp_packet_collection, atsc3_alp_packet);
                                atsc3_alp_packet_collection->atsc3_alp_packet_pending = NULL;
                                atsc3_alp_packet = NULL;
                            }
                        }
                        
                        //hack to carry over 1 byte payload that is too small
                        if(atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment) {
                            
                            block_Merge(atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment, atsc3_baseband_packet->alp_payload_pre_pointer);
                            
                            block_Release(&atsc3_baseband_packet->alp_payload_pre_pointer);
                            atsc3_baseband_packet->alp_payload_pre_pointer = atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment;
                            
                            while((atsc3_alp_packet = atsc3_alp_packet_parse(atsc3_baseband_packet->alp_payload_pre_pointer))) {
                                __INFO("  atsc3_baseband_packet: carry over:  parse alp_payload_pre_pointer: pos: %d, size: %d",
                                       atsc3_baseband_packet->alp_payload_pre_pointer->i_pos,
                                       atsc3_baseband_packet->alp_payload_pre_pointer->p_size);
                                
                                if(atsc3_alp_packet->is_alp_payload_complete) {
                                    atsc3_alp_packet_collection_add_atsc3_alp_packet(atsc3_alp_packet_collection, atsc3_alp_packet);
                                } else {
                                    __WARN("  incomplete fragment for alp_payload_pre_pointer");
                                    
                                    atsc3_alp_packet_collection->atsc3_alp_packet_pending = atsc3_alp_packet;
                                    atsc3_alp_packet = NULL;
                                    break;
                                }
                            }
                            block_Release(&atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment);
                            
                        }
                        
                        if(atsc3_baseband_packet->alp_payload_post_pointer) {
                            
                            __INFO("atsc3_baseband_packet: starting alp_payload_post_pointer: pos: %d, size: %d",
                                   atsc3_baseband_packet->alp_payload_post_pointer->i_pos,
                                   atsc3_baseband_packet->alp_payload_post_pointer->p_size);
                            

                            while((atsc3_alp_packet = atsc3_alp_packet_parse(atsc3_baseband_packet->alp_payload_post_pointer))) {
                                __INFO("  atsc3_baseband_packet: after parse alp_payload_post_pointer: pos: %d, size: %d",
                                       atsc3_baseband_packet->alp_payload_post_pointer->i_pos,
                                       atsc3_baseband_packet->alp_payload_post_pointer->p_size);
                                

                                if(atsc3_alp_packet->is_alp_payload_complete) {
                                    atsc3_alp_packet_collection_add_atsc3_alp_packet(atsc3_alp_packet_collection, atsc3_alp_packet);
                                } else {
                                    atsc3_alp_packet_collection->atsc3_alp_packet_pending = atsc3_alp_packet;
                                    atsc3_alp_packet = NULL;
                                    break;
                                }
                            }
                            //anything remaining in post_pointer needs to be carried over for next alp_packet processing
                            uint32_t remaining_size = block_Remaining_size(atsc3_baseband_packet->alp_payload_post_pointer);
                            if(remaining_size) {
                                __INFO(" !!!TODO: Carrying over one byte in alp_payload_post_pointer: %d", remaining_size);
                                atsc3_stltp_tunnel_packet_processed->atsc3_baseband_packet_short_fragment = atsc3_baseband_packet->alp_payload_post_pointer;
                            }
                            
                        } else {
                            __INFO("atsc3_baseband_packet: no alp_payload_post_pointer - ignoring as padding");
                     
                        }
                    }
                }
                
                atsc3_reflect_alp_packet_collection(atsc3_alp_packet_collection);
                atsc3_alp_packet_collection_clear_atsc3_alp_packet(atsc3_alp_packet_collection);
            }
            
            if(atsc3_stltp_tunnel_packet_processed->atsc3_stltp_preamble_packet_v.count) {
                __INFO(">>>stltp atsc3_stltp_preamble_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_preamble_packet_v.count);
                for(int i=0; i < atsc3_stltp_tunnel_packet_processed->atsc3_stltp_preamble_packet_v.count; i++) {
                    atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet = atsc3_stltp_tunnel_packet_processed->atsc3_stltp_preamble_packet_v.data[i];
                    //atsc3_alp_parse_stltp_preamble_packet(atsc3_stltp_preamble_packet);
                }
            }
            
            if(atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.count) {
                __INFO(">>>stltp atsc3_stltp_timing_management_packet packet complete: count: %u",  atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.count);
                for(int i=0; i < atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.count; i++) {
                    atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet = atsc3_stltp_tunnel_packet_processed->atsc3_stltp_timing_management_packet_v.data[i];
                    //atsc3_alp_parse_stltp_baseband_packet(atsc3_stltp_baseband_packet);
                }
            }
            
            atsc3_stltp_tunnel_packet_clear_completed_inner_packets(atsc3_stltp_tunnel_packet_processed);
            
        } else {
            __ERROR("error processing packet: %p, size: %u",  ip_udp_rtp_packet, ip_udp_rtp_packet->data->p_size);
        }
    }
    
    atsc3_ip_udp_rtp_packet_destroy(&ip_udp_rtp_packet);
}

int main(int argc,char **argv) {
    _IP_UDP_RTP_PARSER_DEBUG_ENABLED = 1;
    _ATSC3_UTILS_TRACE_ENABLED = 0;
    
    char *dev;
    char *devInject;
    char *filter_dst_ip = NULL;
    char *filter_dst_port = NULL;
    char *filter_packet_id = NULL;
    
    int dst_port_filter_int;
    int dst_ip_port_filter_int;
    int dst_packet_id_filter_int;
    
    char errbuf[PCAP_ERRBUF_SIZE];
    char errbufInject[PCAP_ERRBUF_SIZE];

    pcap_t* descr;
    struct bpf_program fp;
    bpf_u_int32 maskp;
    bpf_u_int32 netp;
    
    struct bpf_program fpInject;
    bpf_u_int32 maskpInject;
    bpf_u_int32 netpInject;
    
    atsc3_alp_packet_collection = atsc3_alp_packet_collection_new();

    if(argc != 5) {
        println("%s - an atsc3 stltp udp mulitcast reflector ", argv[0]);
        println("---");
        println("args: devListen ip port devInject");
        println(" devListen : device to listen for stltp udp multicast");
        println(" ip        : ip address for stltp");
        println(" port      : port for single stltp");
        println(" devInject : device to inject for ALP IP reflection");
        
        exit(1);
    } else {
        dev = argv[1];
        filter_dst_ip = argv[2];
        filter_dst_port = argv[3];
        devInject = argv[4];
        
        //parse ip
        dst_ip_addr_filter = (uint32_t*)calloc(1, sizeof(uint32_t));
        char* pch = strtok (filter_dst_ip,".");
        int offset = 24;
        while (pch != NULL && offset>=0) {
            uint8_t octet = atoi(pch);
            *dst_ip_addr_filter |= octet << offset;
            offset-=8;
            pch = strtok (NULL, ".");
        }
        
        //parse port
        
        dst_port_filter_int = atoi(filter_dst_port);
        dst_ip_port_filter = (uint16_t*)calloc(1, sizeof(uint16_t));
        *dst_ip_port_filter |= dst_port_filter_int & 0xFFFF;
    }
    
    
    println("%s -an atsc3 stltp udp mulitcast reflector , listening on dev: %s, refleting: %s", argv[0], dev, devInject);
    
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
   
    //inject
    pcap_lookupnet(devInject, &netpInject, &maskpInject, errbufInject);
    pcap_t* descrInject = pcap_open_live(devInject, MAX_PCAP_LEN, 1, 0, errbufInject);
    atsc3_alp_packet_collection->descrInject = descrInject;
    
    pcap_loop(descr, -1, process_packet, NULL);
    
    return 0;
}

