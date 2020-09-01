/*
 * atsc3_stltp_types.h
 *
 *  Created on: Mar 16, 2019
 *      Author: jjustman
 */

#include <stdint.h>
#include <stdbool.h>

#ifndef ATSC3_STLTP_TYPES_H_
#define ATSC3_STLTP_TYPES_H_

#include "atsc3_vector_builder.h"
#include "atsc3_logging_externs.h"
#include "atsc3_listener_udp.h"
#include "atsc3_ip_udp_rtp_types.h"
#include "atsc3_ip_udp_rtp_parser.h"
#include "atsc3_baseband_packet_types.h"

#if defined (__cplusplus)
extern "C" {
#endif

/*
 see ATSC A/324:2018 - for overview of STL and CTP
 
 Structure for data payload/frame encapsulation is as follows:
 
     STLTP: OUTER (ip/udp/rtp)
        STLTP: INNER (ip/udp/rtp)
            BASEBAND: (base/optional/extension/payload size from FEC Kpayload, Ninner, and Outer BCH/CRC/NONE)
                ALP: (ip/udp/rtp)
 
 All are segmentable/concatenateable/extensible/etc, so expect a lot of compexlity in managing 4 sliding payload windows
 
 */

#define ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL 					        0x61
#define ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL_STRING                      "tunnel"

// A/324:2018 TMP, Preamble, Baseband  inner RTP IP: 239.0.51.48 -> 0xEF003330
#define ATSC3_STLTP_PAYLOAD_TYPE_INNER_IP_ADDRESS					0xEF003330

#define ATSC3_STLTP_PAYLOAD_TYPE_TIMING_MANAGEMENT_PACKET       	0x4C
#define ATSC3_STLTP_PAYLOAD_TYPE_TIMING_MANAGEMENT_PACKET_STRING    "timing_management"
#define ATSC3_STLTP_PAYLOAD_TYPE_TIMING_MANAGEMENT_PORT		       	30065

#define ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET                    0x4D
#define ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET_STRING		        "preamble"
#define ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET_PORT				30064

#define ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET 		            0x4E
#define ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET_STRING             "baseband"
#define ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET_PORT_MIN			30000
#define ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET_PORT_MAX			30063



enum ATSC3_CTP_STL_PAYLOAD_TYPES {
    STLTP_PAYLOAD_TYPE_TUNNEL            = ATSC3_STLTP_PAYLOAD_TYPE_TUNNEL,
    STLTP_PAYLOAD_TYPE_TIMING_MANAGEMENT = ATSC3_STLTP_PAYLOAD_TYPE_TIMING_MANAGEMENT_PACKET,
    STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET   = ATSC3_STLTP_PAYLOAD_TYPE_PREAMBLE_PACKET,
    STLTP_PAYLOAD_TYPE_BASEBAND_PACKET   = ATSC3_STLTP_PAYLOAD_TYPE_BASEBAND_PACKET
};

extern const char *ATSC3_CTP_STL_PAYLOAD_TYPE_TO_STRING(int code);



/* see A322: PLP for more information regarding baseband packet encapsulation (
https://www.atsc.org/wp-content/uploads/2016/10/A322-2018-Physical-Layer-Protocol.pdf
*/

typedef struct atsc3_stltp_baseband_packet {
    atsc3_ip_udp_rtp_packet_t*     ip_udp_rtp_packet_outer;
    
    atsc3_ip_udp_rtp_packet_t*     ip_udp_rtp_packet_inner;

    uint32_t                       fragment_count;
    bool                           is_complete;

    //TODO: refactor this to block_t for payload/offset/length
    uint8_t* 	        	       payload;
    uint32_t 	        	       payload_offset;
	uint32_t 	        	       payload_length;

    //for reference between stltp inner -> baseband -> alp
    atsc3_baseband_packet_t        atsc3_baseband_packet;

    uint8_t 					   plp_num;

} atsc3_stltp_baseband_packet_t;


/**
 * ATSC A/324:2018 Section 8.2.1
 *
 *
 *The Preamble data shall be delivered in an RTP/UDP/IP multicast Stream conforming to RFC
3550 [6] with the constraints defined below. The maximum Preamble data structure size can
exceed the typical 1500-byte MTU, so a mechanism is defined herein to allow segmentation of the
Preamble data across multiple RTP/UDP/IP packets. Note that such segmentation is only required
to conform with typical MTU sizes of 1500 bytes. If the local Network allows larger multicast
packets, this segmentation may not be needed.
 
The payload data for each Preamble Stream RTP/UDP/IP packet shall be a fragment of the
Preamble Payload data structure described in Table 8.1. To provide validation that the
L1_Basic_signaling and L1_Detail_signaling structures are delivered correctly over the STL, a
16-bit cyclic redundancy check is provided. The CRC is applied to the combined length field,
L1_Basic_signaling and L1_Detail_signaling, and appended as the last 16 bits of the payload data.
The resultant Stream of Preamble Payload packets shall have destination address 239.0.51.48 and
destination port 30064.


Table 8.1 Preamble Payload
Syntax 							No. of Bits		Format
	Preamble_Payload () {
		length 					16			 	uimsbf
		L1_Basic_signaling() 	200 			Table 9.2 of [3]  //atsc a/322
		L1_Detail_signaling() 	var 			Table 9.12 of [3] //atsc a/322
		crc16 					16 				uimsbf
}

 *
 * Preamble Payload data structure as described in Table 8.1. The Data Consumer can accumulate
RTP packets until it has received all of the bytes defined by the length field in the first packet. If a
packet is missed, as determined by a missing sequence number, or if a packet with the marker (M)
bit set to ‘1’ is received prematurely, indicating the start of the next Preamble Payload Packet Set,
then one or more packets have been lost and the entire Preamble Payload data set has been lost.
Any accumulated data shall be discarded.

The RTP header fields shall follow the syntax defined in RFC 3550 [6], with the following
additional constraints:

The Padding (P) bit shall conform to the RFC 3550 [6] specification.

The Extension (X) bit shall be set to zero ‘0’, indicating the header contains no extension.

The CSRC Count (CC) shall be set to zero ‘0’, as no CSRC fields are necessary.

The marker (M) bit shall be set to one ‘1’ to indicate that the first byte of the payload is the start of
the Preamble Payload data. A zero ‘0’ value shall indicate that the payload is a continuation of the
Preamble Payload data from the previous packet.

The Payload Type (PT) shall be set to 77 (0x4d), indicating the Preamble Payload type.

The Sequence Number shall conform to the RFC 3550 [6] specification.
The Timestamp shall be defined as in Table 8.2. The timestamp shall be set to the same value for
all of the Preamble Payload Packet Set.

The Synchronization Source (SSRC) Identifier shall be set to zero ‘0’. There shall be no other sources
of Preamble Payload data carried within an STLTP Stream.

Any redundant sources can be
managed using IGMP Source-Specific Multicast (SSM) mechanisms.

**/

/*
 
 l1_basic_signalling: 200 bits ->
 */

typedef struct L1_basic_signaling {
    uint8_t     L1B_version:3;
    uint8_t     L1B_mimo_scattered_pilot_encoding:1;
    uint8_t     L1B_lls_flag:1;
    uint8_t     L1B_time_info_flag:2;
    uint8_t     L1B_return_channel_flag:1;
    
    uint8_t     L1B_papr_reduction:2;
    uint8_t     L1B_frame_length_mode:1;
    
    //if L1B_frame_length_mode==0
    uint16_t    L1B_frame_length:10;
    uint16_t    L1B_excess_samples_per_symbol:13;
    //else
    uint16_t    L1B_time_offset:16;
    uint8_t     L1B_additional_samples:7;
    //end else
    
    uint8_t     L1B_num_subframes:8;
    uint8_t     L1B_preamble_num_symbols:3;
    uint8_t     L1B_preamble_reduced_carriers:3;
    uint8_t     L1B_L1_Detail_content_tag:2;
    uint16_t    L1B_L1_Detail_size_bytes:13;
    
    uint8_t     L1B_L1_Detail_fec_type:3;
    uint8_t     L1B_L1_Detail_additional_parity_mode:2;
    
    uint32_t    L1B_L1_Detail_total_cells:19;
    
    uint8_t     L1B_first_sub_mimo:1;
    uint8_t     L1B_first_sub_miso:2;
    uint8_t     L1B_first_sub_fft_size:2;
    uint8_t     L1B_first_sub_reduced_carriers:3;
    uint8_t     L1B_first_sub_guard_interval:4;
    uint16_t    L1B_first_sub_num_ofdm_symbols:11;
    
    uint8_t     L1B_first_sub_scattered_pilot_pattern:5;
    uint8_t     L1B_first_sub_scattered_pilot_boost:3;
    uint8_t     L1B_first_sub_sbs_first:1;
    uint8_t     L1B_first_sub_sbs_last:1;
    
    uint64_t    L1B_reserved:48;
    uint32_t    L1B_crc;

} L1_basic_signaling_t;

/*
 
 A/322:2018 - Physical Layer Protocol - Table 9.8 - L1-Detail Signaling Fields and Syntax
 
*/
    
typedef struct L1D_bonded_bsid_block {
    uint16_t    L1D_bonded_bsid;
    uint8_t     reserved:3;
} L1D_bonded_bsid_block_t;

//if(L1B_time_info_flag != 00)
typedef struct L1D_time_sec_block {
    uint32_t    L1D_time_sec;
    uint16_t    L1D_time_msec:10;
    //if(L1B_time_info_flag != 01)
    uint16_t    L1D_time_usec:10;
    //if(L1B_time_info_flag != 10)
    uint16_t    L1D_time_nsec:10;
} L1D_time_sec_block_t;
    
    
typedef struct L1D_plp_bonded_rf_id {
    uint8_t     L1D_plp_bonded_rf_id:3;
} L1D_plp_bonded_rf_id_t;

    
typedef struct L1D_plp_HTI_num_fec_blocks {
    uint16_t    L1D_plp_HTI_num_fec_blocks:12;
} L1D_plp_HTI_num_fec_blocks_t;
    
typedef struct L1D_PLP_parameters {
//{
    uint8_t     L1D_plp_id:6;
    uint8_t     L1D_plp_lls_flag:1;
    uint8_t     L1D_plp_layer:2;
    uint32_t    L1D_plp_start:24;
    uint32_t    L1D_plp_size:24;
	
    uint8_t     L1D_plp_scrambler_type:2;
    uint8_t     L1D_plp_fec_type:4;
    
    //if (L1D_plp_fec_type∈{0,1,2,3,4,5}) {
		uint8_t     L1D_plp_mod:4;
		uint8_t     L1D_plp_cod:4;
    //}
    
    uint8_t     L1D_plp_TI_mode:2;
    
    //if (L1D_plp_TI_mode=00) {
		uint16_t    L1D_plp_fec_block_start:15;
    //} else if (L1D_plp_TI_mode=01) {
		uint32_t    L1D_plp_CTI_fec_block_start:22;
    //}
    
    //if (L1D_num_rf>0) {
    uint8_t     L1D_plp_num_channel_bonded:3;
	
		//if (L1D_plp_num_channel_bonded>0) {
		uint8_t     L1D_plp_channel_bonding_format:2;
		
			//for (k=0..L1D_plp_num_channel_bonded) {
			ATSC3_VECTOR_BUILDER_STRUCT(L1D_plp_bonded_rf_id);
	
			//}
        //}
    //}
    
    //if (i=0 && L1B_first_sub_mimo=1) || (i >0 && L1D_mimo=1) {
    uint8_t     L1D_plp_mimo_stream_combining:1;
    uint8_t     L1D_plp_mimo_IQ_interleaving:1;
    uint8_t     L1D_plp_mimo_PH:1;
    //}
    
    //if (L1D_plp_layer=0) {
		uint8_t     L1D_plp_type:1;
        //if (L1D_plp_type=1) {
			uint16_t    L1D_plp_num_subslices:14;
			uint32_t    L1D_plp_subslice_interval:24;
        //}
        
		//if (((L1D_plp_TI_mode=01) || (L1D_plp_TI_mode=10))&&(L1D_plp_mod=0000)) {
			uint8_t     L1D_plp_TI_extended_interleaving:1;
        //}
	
        //if (L1D_plp_TI_mode=01) {
			uint8_t     L1D_plp_CTI_depth:3;
			uint16_t    L1D_plp_CTI_start_row:11;
		//}else if (L1D_plp_TI_mode=10) {
			uint8_t     L1D_plp_HTI_inter_subframe:1;
			uint8_t     L1D_plp_HTI_num_ti_blocks:4;
			uint16_t    L1D_plp_HTI_num_fec_blocks_max:12;
			//if (L1D_plp_HTI_inter_subframe=0) {
				uint16_t    L1D_plp_HTI_num_fec_blocks:12;
            //} else {
                //for (k=0..L1D_plp_HTI_num_ti_blocks) {
					ATSC3_VECTOR_BUILDER_STRUCT(L1D_plp_HTI_num_fec_blocks);
                //}
            //}
    uint8_t     L1D_plp_HTI_cell_interleaver:1;
        //}
    //} else {
    uint8_t     L1D_plp_ldm_injection_level:5;
    //}
//}
    
} L1D_PLP_parameters_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(L1D_PLP_parameters, L1D_plp_bonded_rf_id);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(L1D_PLP_parameters, L1D_plp_HTI_num_fec_blocks);


//for (i=0 .. L1B_num_subframes)
typedef struct L1D_subframe_parameters {
    //if(i>0)
		uint8_t     L1D_mimo:1;
		uint8_t     L1D_miso:2;
		uint8_t     L1D_fft_size:2;
		uint8_t     L1D_reduced_carriers:3;
		uint8_t     L1D_guard_interval:4;
		uint16_t    L1D_num_ofdm_symbols:11;
		uint8_t     L1D_scattered_pilot_pattern:5;
		uint8_t     L1D_scattered_pilot_boost:3;
		uint8_t     L1D_sbs_first:1;
		uint8_t     L1D_sbs_last:1;
    //}
    
    //if(L1B_num_subframes>0)
		uint8_t     L1D_subframe_multiplex:1;
    //}
    
    uint8_t     L1D_frequency_interleaver:1;
    
    /*
     if (((i=0)&&(L1B_first_sub_sbs_first || L1B_first_sub_sbs_last)) || ((i>0)&&(L1D_sbs_first | L1D_sbs_last))) {
     */
		uint16_t    L1D_sbs_null_cells:13;
    //}
    
    uint8_t     L1D_num_plp;
    
    //for (j=0 .. L1D_num_plp) {
    ATSC3_VECTOR_BUILDER_STRUCT(L1D_PLP_parameters);
    //}
} L1D_subframe_parameters_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(L1D_subframe_parameters, L1D_PLP_parameters);

    
typedef struct L1_detail_signaling {
    uint8_t                 L1D_version:4;
    uint8_t                 L1D_num_rf:3;
    ATSC3_VECTOR_BUILDER_STRUCT(L1D_bonded_bsid_block);
    
    //if(L1B_time_info_flag != 00)
    L1D_time_sec_block_t    L1D_time_sec_block;

    //for (i=0 .. L1B_num_subframes)
    ATSC3_VECTOR_BUILDER_STRUCT(L1D_subframe_parameters);
    
    uint16_t        L1D_bsid;
    uint8_t*        L1D_reserved; //for future payload use cases in L1D payload
    uint32_t        L1D_crc;
} L1_detail_signaling_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(L1_detail_signaling, L1D_bonded_bsid_block);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(L1_detail_signaling, L1D_subframe_parameters);

typedef struct atsc3_preamble_packet {
	atsc3_rtp_header_timestamp_t	bootstrap_timing_data_timestamp_short_reference;

    uint16_t                    length;
    L1_basic_signaling_t        L1_basic_signaling;
    L1_detail_signaling_t       L1_detail_signaling;
    uint16_t                    crc16;
    
} atsc3_preamble_packet_t;

typedef struct atsc3_stltp_preamble_packet {
    
    atsc3_ip_udp_rtp_packet_t*      ip_udp_rtp_packet_outer;
    
    atsc3_ip_udp_rtp_packet_t*      ip_udp_rtp_packet_inner;

    uint32_t                        fragment_count;
    bool                            is_complete;
    
    //TODO: refactor this to block_t for payload/offset/length
    uint8_t*                         payload;
    uint16_t                         payload_offset;
    uint16_t                         payload_length;
   
    atsc3_preamble_packet_t*          preamble_packet;
} atsc3_stltp_preamble_packet_t;

    
//ATSC A/324:2018 - STL
//Table 8.3 - Timing and Management Stream Packet Payload

#define ATSC3_A324_A_MILLISECOND_PERIOD 1.048576

typedef struct bootstrap_timing_data {
    uint32_t    seconds;
    uint32_t    nanoseconds;

    atsc3_rtp_header_timestamp_t bootstrap_timing_data_timestamp_short_reference;
} atsc3_bootstrap_timing_data_t;
    
typedef struct per_transmitter_data {
    uint16_t    xmtr_id;
    uint16_t    tx_time_offset;
    uint8_t     txid_injection_lvl;
    uint8_t     miso_filt_code_index;
    uint32_t    _reserved:29; //1
} atsc3_per_transmitter_data_t;

typedef struct packet_release_time {
    uint8_t     pkt_rls_seconds;
    uint16_t    pkt_rls_a_milliseconds;
    uint32_t    pkt_rls_computed_milliseconds;
    uint8_t     _reserved:2;
} packet_release_time_t;
    
typedef struct error_check_data {
    uint16_t    crc16;
} error_check_data_t;

//Table 8.3 - Timing and Management Stream Packet Payload
    
typedef struct timing_management_packet {
	atsc3_rtp_header_timestamp_t	bootstrap_timing_data_timestamp_short_reference;

    //Structure_Data() {
    uint16_t    length;
	uint8_t		version_major:4;
	uint8_t		version_minor:4;
	uint8_t 	maj_log_rep_cnt_pre:4;
	uint8_t 	maj_log_rep_cnt_tim:4;
	uint8_t 	bootstrap_major:4;
	uint8_t 	bootstrap_minor:4;
	uint8_t 	min_time_to_next:5;
	uint8_t 	system_bandwidth:2;
	uint8_t 	bsr_coefficient:7;
	uint8_t 	preamble_structure:8;
	uint8_t 	ea_wakeup:2;
	uint8_t 	num_emission_tim:6;
	uint8_t 	num_xmtrs_in_group:6;
	uint8_t 	xmtr_group_num:7;
	uint8_t 	maj_log_override:3;
	uint8_t		num_miso_filt_codes:2;
	uint8_t 	tx_carrier_offset:2;
	uint8_t 	_reserved:6; 		//for (i=0; i<6; i++) ‘1’
    //}

    //Bootstrap_Timing_Data() {
        //for (i=0; i<=num_emission_tim; i++) {
    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_bootstrap_timing_data);
        //}
    //}
    
    //Per_Transmitter_Data () {
        //for (i=0; i<=num_xmtrs_in_group; i++) {
    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_per_transmitter_data);
        //}
    //}
    
    //Packet_Release_Time() {
    packet_release_time_t   packet_release_time;
    //}
    
    //Error_Check_Data () {
    error_check_data_t error_check_data;
    //}
    
} atsc3_timing_management_packet_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_timing_management_packet, atsc3_bootstrap_timing_data);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_timing_management_packet, atsc3_per_transmitter_data);

typedef struct atsc3_stltp_timing_management_packet {
    atsc3_ip_udp_rtp_packet_t*      ip_udp_rtp_packet_outer;
    //atsc3_rtp_header_t*             rtp_header_outer; //pointer from ip_udp_rtp_packet_outer->rtp_header
    
    atsc3_ip_udp_rtp_packet_t*      ip_udp_rtp_packet_inner;
    //atsc3_rtp_header_t*             rtp_header_inner; //pointer from ip_udp_rtp_packet_outer->rtp_header

    uint32_t                        fragment_count;
    bool                            is_complete;

    //TODO: refactor this to block_t for payload/offset/length
	uint8_t* 					    payload;
	uint16_t 					    payload_offset;
	uint16_t 					    payload_length;

	atsc3_timing_management_packet_t*	timing_management_packet;
    
} atsc3_stltp_timing_management_packet_t;


/*
 jjustman-2019-07-23: note: when parsing the stltp tunnel outer/inner, only seek against packet_outer,
                            use inner when parsing out baseband/preamble/timing packet handoffs only

                            //incomplete packet for fragmentation

atsc3_stltp_tunnel_packet_set_baseband_packet_pending_from_inner_rtp_for_plp(atsc3_stltp_depacketizer_context, atsc3_stltp_tunnel_packet_current->ip_udp_rtp_packet_inner, atsc3_stltp_tunnel_packet_current);

 */

typedef struct atsc3_alp_packet atsc3_alp_packet_t;

typedef struct atsc3_stltp_tunnel_baseband_packet_pending_by_plp {

    atsc3_stltp_baseband_packet_t*          atsc3_stltp_baseband_packet_pending;
    block_t*                                atsc3_baseband_packet_short_fragment;

    atsc3_alp_packet_t*             		atsc3_alp_packet_pending;

} atsc3_stltp_tunnel_baseband_packet_pending_by_plp_t;


ATSC3_VECTOR_BUILDER_DISTINCT_TYPEDEF_STRUCT_DEFINITION(atsc3_stltp_preamble_packet);
ATSC3_VECTOR_BUILDER_DISTINCT_TYPEDEF_STRUCT_DEFINITION(atsc3_stltp_timing_management_packet);

typedef struct atsc3_stltp_tunnel_packet {

    //outer RTP packet pointer, make sure to seek 40 bytes for IP/UDP/RTP header offset
	atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_outer;
    
    //floating inner packet, may be delinated either by outer->rtp_header->marker OR
    //by reading inner packet length until remaining_bytes==0 for next inner packet
    atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_inner;

    //only set this when we have a short packet read (e.g. less than 40 bytes) at the end of a payload,
    //we need to re-fragement it when more outer data is available
    atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_pending_refragmentation_outer;
    
    //only set this when we have a pending baseband/preamble/timing_managent packet and always duplicate and free
    atsc3_ip_udp_rtp_packet_t* ip_udp_rtp_packet_pending_concatenation_inner;

	//atsc3_stltp_baseband_packet_t* 		atsc3_stltp_baseband_packet;
    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_stltp_baseband_packet);
    
    atsc3_stltp_tunnel_baseband_packet_pending_by_plp_t* atsc3_stltp_tunnel_baseband_packet_pending_by_plp;

	//atsc3_stltp_preamble_packet_t* 		atsc3_stltp_preamble_packet;
    ATSC3_VECTOR_BUILDER_DISTINCT_TYPEDEF_STRUCT_INSTANCE(atsc3_stltp_preamble_packet);
    atsc3_stltp_preamble_packet_t*          atsc3_stltp_preamble_packet_pending;
    //todo: add short fragment re-assembly for preamble (if needed)
    
    //atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet;
    ATSC3_VECTOR_BUILDER_DISTINCT_TYPEDEF_STRUCT_INSTANCE(atsc3_stltp_timing_management_packet);
    atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet_pending;
    //todo: add short fragment re-assembly for TMP (timing&management)


} atsc3_stltp_tunnel_packet_t;

bool atsc3_stltp_tunnel_packet_is_rtp_packet_outer_sequence_number_contiguous(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_last, atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet_current);

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_stltp_tunnel_packet, atsc3_stltp_baseband_packet);
atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet_new_and_init(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet);
void atsc3_baseband_packet_set_plp_from_stltp_baseband_packet(atsc3_baseband_packet_t* atsc3_baseband_packet, atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet);
void atsc3_baseband_packet_set_bootstrap_timing_ref_from_stltp_baseband_packet(atsc3_baseband_packet_t* atsc3_baseband_packet, atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet);
void atsc3_preamble_packet_set_bootstrap_timing_ref_from_stltp_preamble_packet(atsc3_preamble_packet_t* atsc3_preamble_packet, atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet);
void atsc3_timing_management_packet_set_bootstrap_timing_ref_from_stltp_preamble_packet(atsc3_timing_management_packet_t* atsc3_timing_management_packet, atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet);

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_stltp_tunnel_packet, atsc3_stltp_preamble_packet);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_stltp_tunnel_packet, atsc3_stltp_timing_management_packet);
void atsc3_stltp_tunnel_packet_clear_completed_inner_packets(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet);

void atsc3_stltp_tunnel_packet_free(atsc3_stltp_tunnel_packet_t** atsc3_stltp_tunnel_packet);
void atsc3_stltp_tunnel_packet_destroy(atsc3_stltp_tunnel_packet_t** atsc3_stltp_tunnel_packet);
void atsc3_stltp_tunnel_packet_outer_destroy(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet);
void atsc3_stltp_tunnel_packet_inner_destroy(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet);
void atsc3_stltp_tunnel_packet_outer_inner_destroy(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet);

//free outer/inner data payloads
void atsc3_stltp_baseband_packet_free_outer_inner_data(atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet);    
void atsc3_stltp_preamble_packet_free_outer_inner_data(atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet);
void atsc3_stltp_timing_management_packet_free_outer_inner_data(atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet);

//free inner member attributes when used in vector_t
void atsc3_stltp_baseband_packet_free_v(atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet);
void atsc3_stltp_preamble_packet_free_v(atsc3_stltp_preamble_packet_t* atsc3_stltp_preamble_packet);
void atsc3_stltp_timing_management_packet_free_v(atsc3_stltp_timing_management_packet_t* atsc3_stltp_timing_management_packet);

//free inner and ptr*this
void atsc3_stltp_baseband_packet_free(atsc3_stltp_baseband_packet_t** atsc3_stltp_baseband_packet_p);
void atsc3_stltp_preamble_packet_free(atsc3_stltp_preamble_packet_t** atsc3_stltp_preamble_packet_p);
void atsc3_stltp_timing_management_packet_free(atsc3_stltp_timing_management_packet_t** atsc3_stltp_timing_management_packet_p);
    
//utility methods for dumping outer/inner/rtp header payloads
void atsc3_rtp_header_dump_outer(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet);
void atsc3_rtp_header_dump_inner(atsc3_stltp_tunnel_packet_t* atsc3_stltp_tunnel_packet);
void atsc3_rtp_header_dump(atsc3_rtp_header_t* atsc3_rtp_header, int spaces);

    

#if defined (__cplusplus)
}
#endif

#define __STLTP_TYPES_ERROR(...)    __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS_);
#define __STLTP_TYPES_WARN(...)     __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __STLTP_TYPES_INFO(...)     __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define __STLTP_TYPES_DEBUG(...)    if(_STLTP_TYPES_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __STLTP_TYPES_TRACE(...)    if(_STLTP_TYPES_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#endif /* ATSC3_STLTP_TYPES_H_ */
