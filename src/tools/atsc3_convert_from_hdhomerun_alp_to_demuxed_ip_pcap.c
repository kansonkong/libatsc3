/*
 * atsc3_convert_from_hdhomerun_alp_to_demuxed_ip_pcap.c
 *
 *  Created on: Oct 10, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>

#include <atsc3_utils.h>
#include <atsc3_pcap_type.h>

#define _ATSC3_CONVERT_PCAP_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_CONVERT_PCAP_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_CONVERT_PCAP_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_CONVERT_PCAP_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);


int main(int argc, char* argv[] ) {
    if(argc != 2) {
        _ATSC3_CONVERT_PCAP_INFO("%s: convert hdhomerun alp pcap to demuxed ip pcap with repeating LLS emission", argv[0]);
        _ATSC3_CONVERT_PCAP_INFO("");
        _ATSC3_CONVERT_PCAP_INFO("args: hdhomerun_input_file.pcap");
        _ATSC3_CONVERT_PCAP_INFO("");
        _ATSC3_CONVERT_PCAP_INFO("output file will be named hdhomerun_input_file.demuxed.pcap");
        exit(1);
    }
    
    char* PCAP_ALPCAP_INPUT_FILENAME = argv[1];
    size_t pcap_alpcap_input_filename_length = strlen(PCAP_ALPCAP_INPUT_FILENAME);
    
    //demuxed.
    //12345678
    const char* pcap_demuxed_output_filename_suffix = ".demuxed.pcap";
    
    char* PCAP_DEMUXED_OUTPUT_FILENAME = calloc(pcap_alpcap_input_filename_length + 9, sizeof(char));
    memcpy(PCAP_DEMUXED_OUTPUT_FILENAME, PCAP_ALPCAP_INPUT_FILENAME, pcap_alpcap_input_filename_length);
    memcpy(PCAP_DEMUXED_OUTPUT_FILENAME + pcap_alpcap_input_filename_length - 5, pcap_demuxed_output_filename_suffix, strlen(pcap_demuxed_output_filename_suffix));
    
    atsc3_pcap_replay_context_t* atsc3_pcap_reader_context = NULL;
    atsc3_pcap_writer_context_t* atsc3_pcap_writer_context = NULL;

    atsc3_pcap_reader_context = atsc3_pcap_replay_open_filename(PCAP_ALPCAP_INPUT_FILENAME);
    _ATSC3_CONVERT_PCAP_INFO("Opening hdhomerun alp input pcap: %s, context is: %p", PCAP_ALPCAP_INPUT_FILENAME, atsc3_pcap_reader_context);
    
    atsc3_pcap_writer_context = atsc3_pcap_writer_open_filename(PCAP_DEMUXED_OUTPUT_FILENAME);
    if(!atsc3_pcap_writer_context) {
        _ATSC3_CONVERT_PCAP_ERROR("Unable to open demuxed output pcap filename: %s", PCAP_DEMUXED_OUTPUT_FILENAME);
        exit(2);
    }
    
    _ATSC3_CONVERT_PCAP_INFO("Opening demuxed output pcap as  : %s, context is: %p", PCAP_DEMUXED_OUTPUT_FILENAME, atsc3_pcap_writer_context);

    if(atsc3_pcap_reader_context) {
        while((atsc3_pcap_reader_context = atsc3_pcap_replay_iterate_packet(atsc3_pcap_reader_context))) {

            struct timeval current_packet_wallclock_timeval = { .tv_sec = atsc3_pcap_reader_context->current_packet_ts_sec, .tv_usec = atsc3_pcap_reader_context->current_packet_ts_usec};
            
            atsc3_pcap_writer_set_context_wallclock_timeval(atsc3_pcap_writer_context, current_packet_wallclock_timeval);
            
            _ATSC3_CONVERT_PCAP_DEBUG("Writing: pos: %ld, Got packet len: %d, ts_sec: %u, ts_usec: %u",
                    ftell(atsc3_pcap_reader_context->pcap_fp),
                    atsc3_pcap_reader_context->atsc3_pcap_packet_instance.current_pcap_packet->p_size,
                    atsc3_pcap_reader_context->atsc3_pcap_packet_instance.atsc3_pcap_packet_header.ts_sec,
                    atsc3_pcap_reader_context->atsc3_pcap_packet_instance.atsc3_pcap_packet_header.ts_usec);
            
            //jjustman-2022-12-19 - todo - fix me to properly prepend network link type for this packet...
            block_Rewind(atsc3_pcap_reader_context->atsc3_pcap_packet_instance.current_pcap_packet);
            atsc3_pcap_writer_iterate_packet(atsc3_pcap_writer_context, atsc3_pcap_reader_context->atsc3_pcap_packet_instance.current_pcap_packet);

        }
    }
    
    if(atsc3_pcap_reader_context) {
        atsc3_pcap_replay_free(&atsc3_pcap_reader_context);        
    }
    if(atsc3_pcap_writer_context) {
        atsc3_pcap_writer_context_close(atsc3_pcap_writer_context);
        atsc3_pcap_writer_context_free(&atsc3_pcap_writer_context);
    }

    return 0;
}

