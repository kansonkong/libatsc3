/*
 * atsc3_alp_types.h
 *
 *  Created on: May 1, 2019
 *      Author: jjustman
 */

#include <string.h>

#ifdef __DISABLE_LIBPCAP__
typedef struct pcap pcap_t;
#endif

#ifndef ATSC3_ALP_TYPES_H_
#define ATSC3_ALP_TYPES_H_

#include "atsc3_vector_builder.h"
#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"
#include "atsc3_ip_udp_rtp_types.h"
#include "atsc3_stltp_types.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef struct alp_single_packet_header_sub_stream_identification {
	uint8_t SID;
} alp_single_packet_header_sub_stream_identification_t;

typedef struct alp_single_packet_header_header_extension {
	uint8_t 	extension_type;
	uint8_t 	extension_length_minus1;
	uint8_t*	extension_byte;
} alp_single_packet_header_header_extension_t;


/**
 * <A330-2016 Table 5.3 - Payload Configuration Field Value and Total Header Length>
 * 									Next Field
 * PC Field		Meaning				Name	Value	Additional Header Size	Additional Header Field		Total Header Length
 * --------		---------------		-------------	----------------------	-----------------------		-------------------
 * 0			Single Packet		HM		0		-						-							2 bytes
 * 									HM		1		1 byte					length_MSB					3 bytes
 * 1			Segmentation or		S/C		0		1 byte					seg_SN, LSI					3 bytes
 * 				 Continuation		S/C		1		1 byte					length_MSB, count			3 + ceil( (count+1) * 1.5)) bytes
 */

typedef struct alp_single_packet_hdr {
	uint8_t		length_MSB:5;
	uint8_t		reserved:1;
	uint8_t		SIF:1;
	uint8_t		HEF:1;
	alp_single_packet_header_sub_stream_identification_t 	alp_single_packet_header_sub_stream_identification;
	alp_single_packet_header_header_extension_t				alp_single_packet_header_header_extension;
} alp_single_packet_hdr_t;

//alp_packet_header:1
typedef struct alp_packet_header_mode {
	uint8_t	 header_mode:1;
	uint16_t length:11;
	alp_single_packet_hdr_t alp_single_packet_header;

} alp_packet_header_mode_t;

typedef struct alp_segmentation_header {
	uint8_t	segment_sequence_number:5;
	uint8_t	last_segment_indicator:1;
	uint8_t	SIF:1;
	uint8_t	HEF:1;
	alp_single_packet_header_sub_stream_identification_t 	alp_single_packet_header_sub_stream_identification;
	alp_single_packet_header_header_extension_t				alp_single_packet_header_header_extension;
} alp_segmentation_header_t;

typedef struct alp_concatentation_header_component_length {
	uint16_t length:12;
} alp_concatentation_header_component_length_t;

typedef struct alp_concatentation_header {
	uint8_t	length_MSB:4;
	uint8_t	count:3;
	uint8_t	SIF:1;
	uint8_t	HEF:1;
	//upper_bound on component_length
	alp_concatentation_header_component_length_t			alp_concatenation_header_component_length[7];
	uint8_t stuffing_bits:4;
	alp_single_packet_header_sub_stream_identification_t 	alp_single_packet_header_sub_stream_identification;
} alp_concatentation_header_t;


typedef struct alp_packet_segmentation_concatenation {
	uint8_t 	segmentation_concatenation:1;
	uint16_t 	length:11;
	union {
		alp_segmentation_header_t 	alp_segmentation_header; //segmentation_concatenation == 0
		alp_concatentation_header_t alp_concatentation_header; //segmentation_concatenation ==1
	};

} alp_packet_segmentation_concatenation_t;

/*
 * 	signaling_type		meaning
 * 	--------------		----------
 * 	0x00				reserved
 * 	0x01				link mapping table
 * 	0x02				ROHC-U Description Table
 * 	0x03-0xEF			Reserved
 * 	0xF0-0xFF			user private
 *
 * 	signaling_format	meaning
 * 	----------------	--------
 * 	00					Binary
 * 	01					XML
 * 	10					JSON
 * 	11					Reserved
 *
 * 	signaling_encoding	meaning
 * 	------------------	-------
 * 	00					No compression
 * 	01					DEFLATE
 * 	10					reserved
 * 	11					reserved
 */
typedef struct alp_additional_header_for_signaling_information {
	uint8_t		signaling_type;
	uint16_t	signaling_type_extension;
	uint8_t		signaling_version;
	uint8_t		signaling_format:2;
	uint8_t		signaling_encoding:2;
	uint8_t		reserved:4; //1111

} alp_additional_header_for_signaling_information_t;

/* 0x00-0xFF	reserved
 *
 */
typedef struct alp_additional_header_for_type_extension {
	uint16_t	extended_type;
} alp_additional_header_for_type_extension_t;


/**
 * alp_packet_header.packet_type:
 *
 * 	packet_type Value		Meaning
 * 	-----------------		-------------
 * 	000						IPv4 packet
 * 	001						Reserved
 * 	010						Compressed IP packet
 * 	011						Reserved
 * 	100						Link layer signalling packet
 * 	101						Reserved
 * 	110						Packet Type Extension
 * 	111						MPEG-2 Transport Stream
 *
 *	<From A330-2016 Table 5.2 Code Values for packet_type>
 */

typedef struct alp_packet_header  {
	uint8_t packet_type:3;
	uint8_t payload_configuration:1;
	union {
		alp_packet_header_mode_t 				alp_packet_header_mode; //payload_configuration == 0
		alp_packet_segmentation_concatenation_t	alp_packet_segmentation_concatenation; //payload_configuration == 1
	};
    
    //for payload_type == 4 (bits: 100)
    alp_additional_header_for_signaling_information_t alp_additional_header_for_signaling_information;
    
} alp_packet_header_t;

typedef struct atsc3_alp_packet {
	atsc3_rtp_header_timestamp_t	bootstrap_timing_data_timestamp_short_reference;
	uint8_t				plp_num;

    alp_packet_header_t alp_packet_header;
    block_t*            alp_payload;
    bool                is_alp_payload_complete;
} atsc3_alp_packet_t;

typedef struct atsc3_link_mapping_table_multicast {
	uint32_t		src_ip_add;
	uint32_t		dst_ip_add;
	uint16_t		src_udp_port;
	uint16_t		dst_udp_port;
	uint8_t 		sid_flag:1;
	uint8_t 		compressed_flag:1;
	uint8_t			reserved:6;
	
	/* optional char if sid_flag==1
			unsigned char	SID;
	   optional char if compressed_flag==1
			unsigned char	context_id;
	*/
	uint8_t			sid;
	uint8_t			context;
	
} atsc3_link_mapping_table_multicast_t;

typedef struct atsc3_link_mapping_table_plp {
	uint8_t PLP_ID;			/**jdj-2019-01-07  -    Protocol-Specific Indication (PSI): 2 bits **/
	uint8_t	reserved;
	uint8_t num_multicasts;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_link_mapping_table_multicast);
} atsc3_link_mapping_table_plp_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_link_mapping_table_plp, atsc3_link_mapping_table_multicast);

//TODO: include: alp_packet_header->alp_additional_header_for_signaling_information.signaling_version
typedef struct atsc3_link_mapping_table {
    uint8_t alp_additional_header_for_signaling_information_signaling_version;
	uint8_t num_PLPs_minus1:6;
	uint8_t reserved:2;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_link_mapping_table_plp);
} atsc3_link_mapping_table_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_link_mapping_table, atsc3_link_mapping_table_plp);

typedef struct atsc3_alp_packet_collection {
    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_baseband_packet); //re-fragmented baseband packets for alp de-encapsulation
    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_alp_packet);      //completed ALP output packets for emission
    
} atsc3_alp_packet_collection_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_alp_packet_collection, atsc3_baseband_packet);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_alp_packet_collection, atsc3_alp_packet);

//TODO: jjustman-2019-08-08 - fixme: clone is a shallow clone (memcpy) and MAY leave dangling pointer references
atsc3_alp_packet_t* atsc3_alp_packet_clone(atsc3_alp_packet_t* atsc3_alp_packet);

void atsc3_alp_packet_dump(atsc3_alp_packet_t* atsc3_alp_packet);

//always free if created from atsc3_alp_packet_clone when no longer needed
void atsc3_alp_packet_free(atsc3_alp_packet_t** atsc3_alp_packet_p);
void atsc3_alp_packet_free_alp_payload(atsc3_alp_packet_t* atsc3_alp_packet);
void atsc3_link_mapping_table_free(atsc3_link_mapping_table_t** atsc3_link_mapping_table_p);


#define _ATSC3_ALP_TYPES_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_ALP_TYPES_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_ALP_TYPES_INFO(...)    if(_ATSC3_ALP_TYPES_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);  }
#define _ATSC3_ALP_TYPES_DUMP(...)    if(_ATSC3_ALP_TYPES_DUMP_ENABLED)  { __LIBATSC3_TIMESTAMP_DUMP(__VA_ARGS__);   }
#define _ATSC3_ALP_TYPES_DEBUG(...)   if(_ATSC3_ALP_TYPES_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_ALP_TYPES_TRACE(...)   if(_ATSC3_ALP_TYPES_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#if defined (__cplusplus)
}
#endif



#endif /* ATSC3_ALP_TYPES_H_ */
