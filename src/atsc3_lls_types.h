/*
 * atsc3_lls_types.h
 *
 *  Created on: Feb 23, 2019
 *      Author: jjustman
 */

#include <assert.h>

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <sys/types.h>

#include "atsc3_lls_sls_monitor_output_buffer.h"
#include "alc_session.h"
#include "atsc3_listener_udp.h"

//slight tight coupling...
#include "atsc3_player_ffplay.h"
#include "xml.h"



#ifndef ATSC3_LLS_TYPES_H_
#define ATSC3_LLS_TYPES_H_

#include "atsc3_fdt.h"
#include "atsc3_sls_metadata_fragment_types.h"


/***
 * From < A/331 2017 - Signaling Delivery Sync > https://www.atsc.org/wp-content/uploads/2017/12/A331-2017-Signaling-Deivery-Sync-FEC-3.pdf
 * LLS shall be transported in IP packets with address:
 * 224.0.23.60 and destination port 4937/udp
 *
 *
 *UDP/IP packets delivering LLS data shall be formatted per the bit stream syntax given in Table 6.1 below.
 *UDP/IP The first byte of every UDP/IP packet carrying LLS data shall be the start of an LLS_table().
 *UDP/IP  The maximum length of any LLS table is limited by the largest IP packet that can be delivered from the PHY layer, 65,507 bytes5.
 *UDP/IP
 *      Syntax
 *

Syntax							Bits			Format
------							----			------
LLS_table() {

	LLS_table_id 				8
	LLS_group_id 				8
	group_count_minus1 			8
	LLS_table_version 			8
	switch (LLS_table_id) {
		case 0x01:
			SLT					var
			break;
		case 0x02:
			RRT					var
			break;
		case 0x03:
			SystemTime			var
			break;
		case 0x04:
			AEAT 				var
			break;
		case 0x05:
			OnscreenMessageNotification	var
			break;
		default:
			reserved			var
	}
}

No. of Bits
8 8 8 8
var var var var var var
Format
uimsbf uimsbf uimsbf uimsbf
Sec. 6.3
See Annex F Sec. 6.4 Sec. 6.5 Sec. 6.6
     }
 *
 */



/*
 *
 * To create the proper LLS table type instance, invoke
 *

 	lls_table_t* lls = lls_create_table(binary_payload, binary_payload_size);
	if(lls) {
		lls_dump_instance_table(lls);
	}

 */

typedef struct llt_xml_payload {
	uint8_t 	*xml_payload_compressed;
	uint32_t 	xml_payload_compressed_size;
	uint8_t 	*xml_payload;
	uint32_t 	xml_payload_size;

} lls_xml_payload_t;

/**
 *  |SLT|, attributes len: 70, val: xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/SLT/1.0/" bsid="50"
children: 569:dump_xml_string::xml_string: len: 7, is_self_closing: 0, val: |Service|, attributes len: 172, val: serviceId="1001" globalServiceID="urn:atsc:serviceid:ateme_mmt_1" majorChannelNo="10" minorChannelNo="1" serviceCategory="1" shortServiceName="ATEME MMT 1" sltSvcSeqNum="0"
69:dump_xml_string::xml_string: len: 21, is_self_closing: 1, val: |BroadcastSvcSignaling|, attributes len: 118, val: slsProtocol="2" slsDestinationIpAddress="239.255.10.1" slsDestinationUdpPort="51001" slsSourceIpAddress="172.16.200.1"
69:dump_xml_string::xml_string: len: 7, is_self_closing: 0, val: |Service|, attributes len: 172, val: serviceId="1002" globalServiceID="urn:atsc:serviceid:ateme_mmt_2" majorChannelNo="10" minorChannelNo="2" serviceCategory="1" shortServiceName="ATEME MMT 2" sltSvcSeqNum="0"
69:dump_xml_string::xml_string: len: 21, is_self_closing: 1, val: |BroadcastSvcSignaling|, attributes len: 118, val: slsProtocol="2" slsDestinationIpAddress="239.255.10.2" slsDestinationUdpPort="51002" slsSourceIpAddress="172.16.200.1"
69:dump_xml_string::xml_string: len: 7, is_self_closing: 0, val: |Service|, attributes len: 172, val: serviceId="1003" globalServiceID="urn:atsc:serviceid:ateme_mmt_3" majorChannelNo="10" minorChannelNo="3" serviceCategory="1" shortServiceName="ATEME MMT 3" sltSvcSeqNum="0"
69:dump_xml_string::xml_string: len: 21, is_self_closing: 1, val: |BroadcastSvcSignaling|, attributes len: 118, val: slsProtocol="2" slsDestinationIpAddress="239.255.10.3" slsDestinationUdpPort="51003" slsSourceIpAddress="172.16.200.1"
69:dump_xml_string::xml_string: len: 7, is_self_closing: 0, val: |Service|, attributes len: 172, val: serviceId="1004" globalServiceID="urn:atsc:serviceid:ateme_mmt_4" majorChannelNo="10" minorChannelNo="4" serviceCategory="1" shortServiceName="ATEME MMT 4" sltSvcSeqNum="0"
69:dump_xml_string::xml_string: len: 21, is_self_closing: 1, val: |BroadcastSvcSignaling|, attributes len: 118, val: slsProtocol="2" slsDestinationIpAddress="239.255.10.4" slsDestinationUdpPort="51004" slsSourceIpAddress="172.16.200.1"
69:dump_xml_string::xml_string: len: 7, is_self_closing: 0, val: |Service|, attributes len: 117, val: serviceId="5009" globalServiceID="urn:atsc:serviceid:esg" serviceCategory="4" shortServiceName="ESG" sltSvcSeqNum="0"
69:dump_xml_string::xml_string: len: 21, is_self_closing: 1, val: |BroadcastSvcSignaling|, attributes len: 118, val: slsProtocol="1" slsDestinationIpAddress="239.255.20.9" slsDestinationUdpPort="52009" slsSourceIpAddress="172.16.200.1"
 */

/*
 * <SLT xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/SLT/1.0/" bsid="50">
 *
 *
 */
typedef struct slt_entry {
	uint bsid; //broadcast stream id

} slt_entry_t;



/*
 *
 * A/331 Section 6.3 Service List Table XML

<?xml version="1.0" encoding="UTF-8"?>
<SLT xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/SLT/1.0/" bsid="50">
   <Service serviceId="1001" globalServiceID="urn:atsc:serviceid:ateme_mmt_1" majorChannelNo="10" minorChannelNo="1" serviceCategory="1" shortServiceName="ATEME MMT 1" sltSvcSeqNum="0">
      <BroadcastSvcSignaling slsProtocol="2" slsDestinationIpAddress="239.255.10.1" slsDestinationUdpPort="51001" slsSourceIpAddress="172.16.200.1" />
   </Service>
   <Service serviceId="1002" globalServiceID="urn:atsc:serviceid:ateme_mmt_2" majorChannelNo="10" minorChannelNo="2" serviceCategory="1" shortServiceName="ATEME MMT 2" sltSvcSeqNum="0">
      <BroadcastSvcSignaling slsProtocol="2" slsDestinationIpAddress="239.255.10.2" slsDestinationUdpPort="51002" slsSourceIpAddress="172.16.200.1" />
   </Service>
   <Service serviceId="1003" globalServiceID="urn:atsc:serviceid:ateme_mmt_3" majorChannelNo="10" minorChannelNo="3" serviceCategory="1" shortServiceName="ATEME MMT 3" sltSvcSeqNum="0">
      <BroadcastSvcSignaling slsProtocol="2" slsDestinationIpAddress="239.255.10.3" slsDestinationUdpPort="51003" slsSourceIpAddress="172.16.200.1" />
   </Service>
   <Service serviceId="1004" globalServiceID="urn:atsc:serviceid:ateme_mmt_4" majorChannelNo="10" minorChannelNo="4" serviceCategory="1" shortServiceName="ATEME MMT 4" sltSvcSeqNum="0">
      <BroadcastSvcSignaling slsProtocol="2" slsDestinationIpAddress="239.255.10.4" slsDestinationUdpPort="51004" slsSourceIpAddress="172.16.200.1" />
   </Service>
   <Service serviceId="5009" globalServiceID="urn:atsc:serviceid:esg" serviceCategory="4" shortServiceName="ESG" sltSvcSeqNum="0">
      <BroadcastSvcSignaling slsProtocol="1" slsDestinationIpAddress="239.255.20.9" slsDestinationUdpPort="52009" slsSourceIpAddress="172.16.200.1" />
   </Service>
</SLT>


 Table 6.4 Code Values for SLT.Service@serviceCategory


	serviceCategory 		Meaning
	---------------			-------------
	0						ATSC Reserved
	1						Linear A/V service
	2						Linear audio only service
	3						App-based service
	4						ESG service (program guide)
	5						EAS service (emergency alert)
	Other values			ATSC Reserved

  	slsProtocol				Meaning
 	---------- 				-------------
 	0 						ATSC Reserved
 	1						ROUTE
 	2						MMTP
 	other values			ATSC Reserved

 */
enum LLS_SLT_SERVICE_CATEGORY {
	SERVICE_CATEGORY_ATSC_RESERVED=0,
	SERVICE_CATEGORY_LINEAR_AV_SERVICE=1,
	SERVICE_CATEGORY_LINEAR_AUDIO_ONLY_SERVICE=2,
	SERVICE_CATEGORY_APP_BASED_SERVICE=3,
	SERVICE_CATEGORY_ESG_SERVICE=4,
	SERVICE_CATEGORY_EAS_SERVICE=5,
	SERVICE_CATEGORY_ATSC_RESERVED_OTHER=-1	};


enum LLS_SLT_SERVICE_PROTOCOL {
	SLS_PROTOCOL_ATSC_RESERVED=0,
	SLS_PROTOCOL_ROUTE=1,
	SLS_PROTOCOL_MMTP=2,
	SLS_PROTOCOL_ATSC_RESERVED_OTHER=-1};

typedef struct broadcast_svc_signaling {
	int 	sls_protocol;
	char*	sls_destination_ip_address;
	char*	sls_destination_udp_port;
	char*	sls_source_ip_address;

} broadcast_svc_signaling_t;
/*
 *    <Service serviceId="1001" globalServiceID="urn:atsc:serviceid:ateme_mmt_1" majorChannelNo="10" minorChannelNo="1" serviceCategory="1" shortServiceName="ATEME MMT 1" sltSvcSeqNum="0">
 *
 */
typedef struct service {
	uint16_t	service_id;
	char*		global_service_id;
	uint		major_channel_no;
	uint 		minor_channel_no;
	uint		service_category;
	char*		short_service_name;
	uint8_t 	slt_svc_seq_num;  //Version of SLT service info for this service.
	broadcast_svc_signaling_t broadcast_svc_signaling;
} lls_service_t;


typedef struct slt_table {
	int*				bsid;			//list
	int					bsid_n;
	char*			 	slt_capabilities;
	int					service_entry_n;
	lls_service_t**		service_entry; 	//list

} slt_table_t;




/** from atsc a/331 section 6.4
 *

6.4 System Time Fragment

System time is delivered in the ATSC PHY layer as a 32-bit count of the number of seconds, a 10-
bit fraction of a second (in units of milliseconds), and optionally 10-bit microsecond and
nanosecond components, since January 1, 1970 00:00:00, International Atomic Time (TAI), which
is the Precision Time Protocol (PTP) epoch as defined in IEEE 1588 [47]. Further time-related
information is signaled in the XML SystemTime element delivered in LLS.

 */

typedef struct system_time_table {
	int16_t 	current_utc_offset;	//required
	uint16_t 	ptp_prepend; 		//opt
	bool		leap59;				//opt
	bool		leap61;				//opt
	char*		utc_local_offset;	//required
	bool		ds_status;			//opt
	uint8_t		ds_day_of_month;	//opt
	uint8_t		ds_hour;			//opt

} system_time_table_t;

typedef struct aeat_table {
	void* to_implement;
} aeat_table_t;
typedef struct on_screen_message_notification {
	void* to_implement;
} on_screen_message_notification_t;
typedef struct lls_reserved_table {
	void* to_implement;
} lls_reserved_table_t;

typedef enum {
	SLT = 1,
	RRT,
	SystemTime,
	AEAT,
	OnscreenMessageNotification,
	RESERVED
} lls_table_type_t;

typedef struct rrt_table {
	void* to_implement;

} rrt_table_t;




typedef struct lls_table {
	uint8_t								lls_table_id; //map via lls_table_id_type;
	uint8_t								lls_group_id;
	uint8_t 							group_count_minus1;
	uint8_t								lls_table_version;
	lls_xml_payload_t					raw_xml;

	union {

		slt_table_t							slt_table;
		rrt_table_t							rrt_table;
		system_time_table_t					system_time_table;
		aeat_table_t						aeat_table;
		on_screen_message_notification_t	on_screen_message_notification;
		lls_reserved_table_t				lls_reserved_table;
	};
	xml_document_t* xml_document;
} lls_table_t;



typedef struct udp_flow_packet_id_mpu_sequence_tuple {
    udp_flow_t  udp_flow;
    uint16_t    packet_id;
    uint32_t    mpu_sequence_number;
    uint32_t    mpu_sequence_number_last_refragmentation_flush;
    uint32_t    mpu_sequence_number_evict_range_start;
    
    uint32_t    mpu_sequence_number_negative_discontinuity;
    uint32_t    mpu_sequence_number_negative_discontinuity_received_fragments;
    bool   		has_sent_init_box;
    
} udp_flow_packet_id_mpu_sequence_tuple_t;

//we'll just keep a linear search of these, it should pretty straightforard to iterate thru for now..
typedef struct udp_flow_latest_mpu_sequence_number_container {
    uint32_t udp_flows_n;
    udp_flow_packet_id_mpu_sequence_tuple_t** udp_flows;
    
} udp_flow_latest_mpu_sequence_number_container_t;


//just to match the alc pattern...
typedef struct mmt_arguments {
    void* not_implemented;
} mmt_arguments_t;

//just to match the alc pattern...
typedef struct mmt_session {
    void* not_implemented;
} mmt_session_t;

/**
 global_service_id : (null)                                                │
 major_channel_no   : 2               minor_channel_no  : 2
 service_category   : 1, linear av    slt_svc_seq_num   : 0
 short_service_name : MM1
 broadcast_svc_signaling
 sls_protocol               : 2, MMTP
 sls_destination_ip_address : 239.255.1.1:49153
 sls_source_ip_address      : (null)
**/


typedef struct lls_sls_mmt_session {
    uint16_t service_id;
    
    uint32_t sls_source_ip_address;
    
    uint32_t sls_destination_ip_address;
    uint16_t sls_destination_udp_port;
    
    uint16_t video_packet_id;
    uint16_t audio_packet_id;

    udp_flow_packet_id_mpu_sequence_tuple_t* last_udp_flow_packet_id_mpu_sequence_tuple_audio;
    bool last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed;
    udp_flow_packet_id_mpu_sequence_tuple_t* to_process_udp_flow_packet_id_mpu_sequence_tuple_audio;
    
    udp_flow_packet_id_mpu_sequence_tuple_t* last_udp_flow_packet_id_mpu_sequence_tuple_video;
    bool last_udp_flow_packet_id_mpu_sequence_tuple_video_processed;
    udp_flow_packet_id_mpu_sequence_tuple_t* to_process_udp_flow_packet_id_mpu_sequence_tuple_video;

    mmt_arguments_t* mmt_arguments;
    mmt_session_t* mmt_session;
//    alc_arguments_t* alc_arguments;
//    alc_session_t* alc_session;
    
} lls_sls_mmt_session_t;


/**
 * used to store all mmt active sessions for this flow
 */
typedef struct lls_sls_mmt_session_vector {
    lls_table_t* lls_table_slt;
    
    int lls_slt_mmt_sessions_n;
    lls_sls_mmt_session_t** lls_slt_mmt_sessions;
    
} lls_sls_mmt_session_vector_t;



/**
 global_service_id : (null)
 major_channel_no   : 1               minor_channel_no  : 1
 service_category   : 1, linear av    slt_svc_seq_num   : 0
 short_service_name : RT1
 broadcast_svc_signaling
 sls_protocol               : 1, ROUTE
 sls_destination_ip_address : 239.255.1.1:49152
 sls_source_ip_address      : 10.1.62.40
 **/

//alc - assume single session for now

typedef struct lls_sls_alc_session {
	uint16_t service_id;

	bool sls_relax_source_ip_check;
	uint32_t sls_source_ip_address;

	uint32_t sls_destination_ip_address;
	uint16_t sls_destination_udp_port;

	alc_arguments_t* alc_arguments;
	alc_session_t* alc_session;

	//jdj-2019-05-29 - hack for resolving monitor tsi/toi

} lls_sls_alc_session_t;

/**
 
 A/331 - Section 7:
 ...
 
 In the MMTP system, the SLS includes the
    USBD fragment,
    the MMT Package (MP) table,
    the HTML Entry pages Location Description (HELD) (see A/337 [7]), and the
    Distribution Window Description (DWD) (see A/337 [7]).
 
    For hybrid delivery, the MMTP-specific SLS can further include the MPD for broadband components.
    Table 7.4 shows the elements and attributes of the MMTP USBD that would be used in practice for ATSC 3.0 service delivery.
 
**/
typedef struct lls_sls_mmt_monitor {
    
    lls_service_t* lls_service;
    uint16_t service_id;
    lls_sls_mmt_session_t* lls_mmt_session;

    uint16_t video_packet_id;
    uint16_t audio_packet_id;
    
    
    lls_sls_monitor_output_buffer_t lls_sls_monitor_output_buffer;
    lls_sls_monitor_output_buffer_mode_t lls_sls_monitor_output_buffer_mode;

} lls_sls_mmt_monitor_t;




/**
 * used to store all alc active sessions for this flow
 */
typedef struct lls_sls_alc_session_vector {
	lls_table_t* lls_table_slt;

	int lls_slt_alc_sessions_n;
	lls_sls_alc_session_t** lls_slt_alc_sessions;

} lls_sls_alc_session_vector_t;



/**
 * used to store monitor references of current flows

A/331 - Section 7:
 ...
 
    In the ROUTE/DASH system, the SLS includes:
		User Service Bundle Description (USBD),
		the S-TSID,
		Associated Procedure Description (APD),
		the DASH Media Presentation Description (MPD),
		the HTML Entry pages Location Description (HELD) (see A/337 [7]), and
		Distribution Window Description (DWD) (see A/337 [7]).
 */

typedef struct lls_sls_alc_monitor {
	lls_service_t* lls_service;
	uint16_t service_id;

	lls_sls_alc_session_t* lls_alc_session;
	uint32_t video_tsi;
	uint32_t audio_tsi;

	/**
	* jdj-2019-05-29: TODO - use a sparse array lookup (https://github.com/ned14/nedtries) for resolution to proper transfer_object_length to back-patch close flag
	*/
	uint32_t last_video_toi;
	uint32_t last_video_toi_length;
	uint32_t last_audio_toi;
	uint32_t last_audio_toi_length;

	uint32_t last_closed_video_toi;
	uint32_t last_closed_audio_toi;

    uint32_t last_pending_flushed_audio_toi;
    uint32_t last_pending_flushed_video_toi;

    uint32_t last_completed_flushed_audio_toi;
    uint32_t last_completed_flushed_video_toi;

	uint32_t video_toi_init;
	uint32_t audio_toi_init;

	uint32_t usbd_tsi;
	uint32_t stsid_tsi;
	uint32_t apd_tsi;
	uint32_t mpd_tsi;
	uint32_t held_tsi;
	uint32_t dwd_tsi;
    
    lls_sls_monitor_output_buffer_t lls_sls_monitor_output_buffer;
    lls_sls_monitor_output_buffer_mode_t lls_sls_monitor_output_buffer_mode;

    atsc3_fdt_instance_t* atsc3_fdt_instance;
    atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments;

} lls_sls_alc_monitor_t;


typedef struct lls_slt_monitor {
    lls_sls_mmt_monitor_t* lls_sls_mmt_monitor;
	lls_sls_alc_monitor_t* lls_sls_alc_monitor;

    lls_sls_mmt_session_vector_t* lls_sls_mmt_session_vector;
    lls_sls_alc_session_vector_t* lls_sls_alc_session_vector;
	lls_service_t* lls_service;

	lls_table_t* lls_table_slt;

} lls_slt_monitor_t;




#endif /* ATSC3_LLS_TYPES_H_ */
