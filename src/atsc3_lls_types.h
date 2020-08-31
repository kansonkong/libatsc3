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

#ifndef ATSC3_LLS_TYPES_H_
#define ATSC3_LLS_TYPES_H_

#include "atsc3_logging_externs.h"
#include "atsc3_utils.h"
#include "xml.h"

#include "atsc3_aeat_types.h"
#include "atsc3_monitor_events_lls.h"
#include "atsc3_monitor_events_sls.h"
#include "atsc3_monitor_events_alc.h"

#include "atsc3_alc_session.h"
#include "atsc3_route_object.h"
#include "atsc3_sls_alc_flow.h"

#include "atsc3_listener_udp.h"


#include "atsc3_fdt.h"
#include "atsc3_sls_metadata_fragment_types.h"


//slight tight coupling...
#include "atsc3_lls_sls_monitor_output_buffer.h"
#include "atsc3_player_ffplay.h"

#if defined (__cplusplus)
extern "C" {
#endif



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

//jjustman-2020-03-10 - cleanup and renaming to atsc3_ prefix
typedef lls_table_t atsc3_lls_table_t;


/**
 TODO: jjustman-2019-09-18 - move to block_t
 **/
typedef struct lls_xml_payload {
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




  	slsProtocol				Meaning
 	---------- 				-------------
 	0 						ATSC Reserved
 	1						ROUTE
 	2						MMTP
 	other values			ATSC Reserved

 */


typedef struct atsc3_slt_capabilities {
	bool	__to_impl__;

} atsc3_slt_capabilities_t;


enum LLS_SLT_URL_TYPE {
	SLT_URL_TYPE_ATSC_RESERVED=0,
	SLT_URL_TYPE_SLS_SERVER=1,
	SLT_URL_TYPE_ESG_SERVER=2,
	SLT_URL_TYPE_SERVICE_USAGE_DGR_SERVER=3,
	SLT_URL_TYPE_DYNAMIC_EVENT_WEBSOCKET_SERVER=4,
	SLT_URL_TYPE_ATSC_RESERVED_OTHER=-1};

typedef struct atsc3_slt_ineturl {
	char*						url; 							//Base URL to acquire ESG or service layer signalling files available via broadband for Services in this SLT.
	uint8_t						url_type; 						//see: LLS_SLT_URL_TYPE
} atsc3_slt_ineturl_t;

enum LLS_SLT_SERVICE_PROTOCOL {
	SLS_PROTOCOL_ATSC_RESERVED=0,
	SLS_PROTOCOL_ROUTE=1,
	SLS_PROTOCOL_MMTP=2,
	SLS_PROTOCOL_ATSC_RESERVED_OTHER=-1};

typedef struct atsc3_slt_broadcast_svc_signalling {
	int 						sls_protocol;					//LLS_SLT_SERVICE_PROTOCOL

	uint8_t						sls_major_protocol_version;		//A/331:2019 - Default is 1
	uint8_t						sls_minor_protocol_version;		//A/331:2019 - Default is 0

	char*						sls_destination_ip_address;		//A string containing the dotted-IPv4 destination address of the packets carrying broadcast SLS data for this Service.
	char*						sls_destination_udp_port;		//Port number of the packets carrying broadcast SLS data for this Service.

	char*						sls_source_ip_address;			//0..1: A string containing the dotted-IPv4 source address of the packets carrying broadcast SLS data for this Service.

} atsc3_slt_broadcast_svc_signalling_t;


typedef struct atsc3_slt_simulcast_tsid {
	uint16_t					simulcast_tsid;					//Identifier of an ATSC 1.0 broadcast stream carrying the same programming content.
	uint16_t					simulcast_major_channel_no;		//Major channel number of the ATSC 1.0 Service carrying the same programming content.
	uint16_t					simulcast_minor_channel_no;		//Minor channel number of the ATSC 1.0 Service carrying the same programming content.
} atsc3_slt_simulcast_tsid_t;

typedef struct atsc3_slt_svc_capabilities {
	bool						__to_impl__;
} atsc3_slt_svc_capabilities_t;

typedef struct atsc3_slt_svc_inet_url {
	char* 						url;					//URL to access Internet signalling for this Service.
	uint8_t						url_type;				//Type of files available with this URL.
} atsc3_slt_svc_inet_url_t;

/**
 * OtherBsid – Each instance of this list of unsigned short integer values shall
 * indicate an identifier of another Broadcast Stream that delivers a duplicate
 * or a portion of this Service.
 *
 * The format of each instance of OtherBsid shall be identical to the format of @bsid.
 *
 * At least one OtherBsid element shall be present when the @essential attribute is present
 * for the parent Service element and is set to "true".
 *
 * No OtherBsid element shall be present when the @essential attribute is present for
 * the parent Service element and is set to "false".
 *
 * One or more OtherBsid elements with @type equal to "1" may be present when @essential
 * attribute is not present for the parent Service element.
 *
 * There is no default value when OtherBsid element is not present.
 *
 * @type – This unsigned byte integer value shall indicate whether the Broadcast Stream identified
 * by the OtherBsid delivers a duplicate or a portion of this Service according to Table 6.6.
 *
 * When the value of @type is set to "2", this indicates that this Service element represents a
 * portion of a Service which has components in the Broadcast Stream identified by the identifier
 * OtherBsid and whose Service identifier is given by the value of the @serviceId attribute of
 * the parent Service element.
 *
 * When more than one OtherBsid element are present under its parent Service element,
 * the OtherBsid@type attribute values of all these elements shall be equal.
 *
 * Table 6.6 Code Values for SLT.Service.OtherBsid@type
 *
  	type 		Meaning
	----		-------
	0			ATSC Reserved
	1			Duplicate
	2			Portion
	3-255		ATSC Reserved
 */


typedef struct atsc3_slt_other_bsid {
	uint16_t					other_bsid;				//Identifier(s) of other Broadcast Stream(s) that deliver duplicates or portions of this Service.
	uint8_t						type;					//Indicates whether the Broadcast Stream identified by the OtherBsid delivers a duplicate or a portion of this Service.
} atsc3_slt_other_bsid_t;


/*   A/331:2017 sample here:
 *
 *    <Service serviceId="1001" globalServiceID="urn:atsc:serviceid:ateme_mmt_1" majorChannelNo="10" minorChannelNo="1" serviceCategory="1" shortServiceName="ATEME MMT 1" sltSvcSeqNum="0">
 *
 */

/*
 *
 *
 * Service category, coded per Table 6.4.
 *
 * Code Values for SLT.Service@serviceCategory

	LLS_SLT_SERVICE_CATEGORY

	serviceCategory 		Meaning
	---------------			-------------
	0						ATSC Reserved
	1						Linear A/V service
	2						Linear audio only service
	3						App-based service
	4						ESG service (program guide)
	5						EAS service (emergency alert)
	6						DRM Data Service (DRM Data)
	Other values			ATSC Reserved
 *
 */

enum LLS_SLT_SERVICE_CATEGORY {
	SERVICE_CATEGORY_ATSC_RESERVED=0,
	SERVICE_CATEGORY_LINEAR_AV_SERVICE=1,
	SERVICE_CATEGORY_LINEAR_AUDIO_ONLY_SERVICE=2,
	SERVICE_CATEGORY_APP_BASED_SERVICE=3,
	SERVICE_CATEGORY_ESG_SERVICE=4,
	SERVICE_CATEGORY_EAS_SERVICE=5,
	SERVICE_CATEGORY_DRM_SERVICE=6,
	SERVICE_CATEGORY_ATSC_RESERVED_OTHER=-1	};


typedef struct atsc3_lls_slt_service {
	uint16_t					service_id;						//Integer number that identifies this Service within the scope of this Broadcast area.

	char*						global_service_id;				//A globally unique URI that identifies the ATSC 3.0 Service. This attribute is not present for the ESG, EAS and DRM Data Services.
	uint8_t						slt_svc_seq_num;				//Version of SLT Service info for this Service.
	bool						protected_flag;					//Indicates whether one or more components needed for meaningful presentation of this Service are protected (e.g. encrypted)

	uint16_t					major_channel_no;				//Major channel number of the Service.
	uint16_t 					minor_channel_no;				//Minor channel number of the Service.
	uint8_t						service_category;				//LLS_SLT_SERVICE_CATEGORY - Service category, coded per Table 6.4.

	char*						short_service_name;				//Short name of the Service.

	bool						hidden_flag;					//Indicates whether the Service is intended for testing or proprietary use, and is not to be selected by ordinary TV receivers.
	bool						broadband_access_required_flag;	//Indicates whether broadband access is required for a receiver to make a meaningful presentation of the Service.
	bool						essential_flag;					//Indicates if the essential portion of the Service is delivered via this Broadcast Stream

	char*						drm_system_id;					//0..1, For @serviceCategory=6 (DRM Data Service), specifies the DRM System ID of a specific DRM system delivered as part of this Service.

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_slt_simulcast_tsid);				//0..1, see atsc3_simulcast_tsid_t - atsc3_simulcast_tsid_t

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_slt_svc_capabilities);			//0..1, Required capabilities for decoding and meaningfully presenting content of this Service.

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_slt_broadcast_svc_signalling); 	//0..1, Location, protocol, address, id information for broadcast signaling.
																		//jjustman-2020-03-28 - yes, the cardinality is 0..1, but put it in a vector for struct commonality

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_slt_svc_inet_url); 				//0..N, URL to access Internet signalling for this Service.

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_slt_other_bsid); 					//0..N, Identifier(s) of other Broadcast Stream(s) that deliver duplicates or portions of this Service.

} atsc3_lls_slt_service_t;

typedef atsc3_lls_slt_service_t atsc3_lls_slt_service_cache_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_lls_slt_service, atsc3_slt_simulcast_tsid);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_lls_slt_service, atsc3_slt_svc_capabilities);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_lls_slt_service, atsc3_slt_broadcast_svc_signalling);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_lls_slt_service, atsc3_slt_svc_inet_url);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_lls_slt_service, atsc3_slt_other_bsid);


/*
 * A/331:2019 - Section 6.3.1 SLT Syntax Description
 *
 */
typedef struct atsc3_lls_slt_table {
	int*						bsid;			//list  - TODO: fix me?
	int							bsid_n;

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_slt_capabilities);	//TODO: jjustman-2019-10-03 - change to sa: CapabilitiesType - Required capabilities for decoding and meaningfully presenting the content for all the Services in this SLT instance.

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_slt_ineturl); 		//TODO: Base URL to acquire ESG or service layer signalling files available via broadband for Services in this SLT

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_lls_slt_service);		 //jjustman-2019-10-03 - refactored from
															//lls_service_t**		service_entry; 	//list
} atsc3_lls_slt_table_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_lls_slt_table, atsc3_slt_capabilities);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_lls_slt_table, atsc3_slt_ineturl);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_lls_slt_table, atsc3_lls_slt_service);

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

typedef struct on_screen_message_notification {
    char*      on_screen_message_notification_xml_fragment_latest;
	void* to_implement;
} on_screen_message_notification_t;


/*
 * defined in A/360:2019
 *
 * Table 5.1
 *

    Element or AttributeName        Use     DataType                ShortDescription
    ------------------------------  ----    --------------          -----------------------
     CertificationData                                              Root element of the CertificationData table.
      ToBeSignedData                1
        @OCSPRefresh                1       xs:dayTimeDuration      The duration for which an OCSPResponse is considered valid from its producedAt time.
        Certificates                1..N    Base64 String           A list of certificates that are used to authenticate a broadcaster signature.
                                                                    This must include end-entity certificates authenticating the CurrentCert and the CMSSignedData
                                                                    signing certificate and any intermediate CA certificates used to validate these certificates.
                                                                    The Root CA certificate is not included in the list.
        CurrentCert                 1       Base64 String           SubjectKeyIdentifier for the certificate currently used to sign signaling messages.
        CertReplacement             0..1
         @NextCertFrom              1       DateTime                Earliest time at which NextCert can be validly used.
         @CurrentCertUntil          1       DateTime                Latest time at which CurrentCert can be validly used.
         NextCert                   1       Base64 String           SubjectKeyIdentifier for the certificate next used to sign signaling messages.
      CMSSignedData                 1       Base64 String           A CMS Signed Data structure authenticating the ToBeSignedData contained in this table.
      OCSPResponse                  1..N    Base64 String           A set of OCSP Responses that provide status information for each of the Certificates.


  See payload sample in test_data/2019-phx-nab-interop-signed-lls/lls_table_type_id_6.xml

 */

typedef struct atsc3_certification_data{
    void* to_implement;

} atsc3_certification_data_t;

/* defined in A/331:2020
 *
 * Section 6.7 SignedMultiTable

 Syntax                                             No. of Bits     Format
 ------------------------------------------------   -----------     -----------
 SignedMultiTable() {
    LLS_payload_count                               8               uimsbf
    for (i=0; i<LLS_payload_count; i++) {
        LLS_payload_id                              8               uimsbf
        LLS_payload_version                         8               uimsbf
        LLS_payload_length                          16              uimsbf
        LLS_payload()                               var
    }
    signature_length                                16              uimsbf
    signature()                                     var             uimsbf


 Description:

    LLS_payload_count – An 8-bit unsigned integer that shall list the number of LLS tables in the loop defined in Table 6.15 above.
    LLS_payload_id – An 8-bit unsigned integer that shall indicate the payload type (values of LLS_table_id) of the LLS_payload() field.
                     Valid values are from the values defined for LLS_table_id.
                     *The values 0x00 and 0xFE shall not be used.*

    LLS_payload_version – An 8-bit unsigned integer that shall be defined as LLS_table_version for this payload.
    LLS_payload_length – A 16-bit unsigned integer that shall indicate the length, in bytes, of the LLS_payload() field.
    LLS_payload() – The payload as signaled by the LLS_payload_id value (e.g., SLT, RRT). See LLS_table_id.

    signature_length – A 16-bit unsigned integer that shall indicate the length, in bytes, of the signature() field.
    signature() – This field shall be CMS Signed Data as per A/360 [10], Section 5.2.2.3.
                  The signature shall be computed over the LLS_payload_count field up to but not including the signature_length field.


    Cross-reference from A/360:2019:

    5.2.2.3 Signatures for Low Level Signaling (LLS) Tables

    A signature that is applied to a LLS message is carried in a CMS Signed Data (RFC 5652 [13]) element with the following characteristics:

        1) The characteristics shall be as specified in Section 5.2.2.1 above.
        2) The SignerIdentifier shall match either the CurrentCert or, if present, the NextCert.


        https://tools.ietf.org/html/rfc5652

        The CMS values are generated using ASN.1 [X.208-88], using BER-
        encoding (Basic Encoding Rules) [X.209-88].  Values are typically
        represented as octet strings.  While many systems are capable of
        transmitting arbitrary octet strings reliably, it is well known that
        many electronic mail systems are not.  This document does not address
        mechanisms for encoding octet strings for reliable transmission in
        such environments.


 */
typedef struct atsc3_signed_multi_table_lls_payload {
    uint8_t             lls_payload_id;
    uint8_t             lls_payload_version;
    uint16_t            lls_payload_length;

    //hack
    block_t*            lls_payload;

    //super hack
    atsc3_lls_table_t*  lls_table;

} atsc3_signed_multi_table_lls_payload_t;

typedef struct atsc3_signed_multi_table {
    block_t*    raw_signed_multi_table_for_signature;

    uint8_t     lls_payload_count;

    ATSC3_VECTOR_BUILDER_STRUCT(atsc3_signed_multi_table_lls_payload);

    uint16_t    signature_length;
    block_t*    signature;

} atsc3_signed_multi_table_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_signed_multi_table, atsc3_signed_multi_table_lls_payload);


typedef struct lls_reserved_table {
	void* to_implement;
} lls_reserved_table_t;

typedef enum {
	SLT = 0x01,
	RRT = 0x02,
	SystemTime = 0x03,
	AEAT = 0x04,
	OnscreenMessageNotification = 0x05,
	CertificationData = 0x06,
	SignedMultiTable = 0xFE,
	UserDefined = 0xFF,
    RESERVED = 0x00,             //anything else...
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
		atsc3_lls_slt_table_t				slt_table;
		rrt_table_t							rrt_table;
		system_time_table_t					system_time_table;
		atsc3_aeat_table_t					aeat_table;
		on_screen_message_notification_t	on_screen_message_notification;

		//jjustman-2020-03-09 - new in A/331:2020 and A/360:2019
		atsc3_certification_data_t          certification_data; //0x06 - A/360:2019
		atsc3_signed_multi_table_t          signed_multi_table; //0xFE - A/331:2020
		//jjustman-2020-03-09 - NOTE: lls_table_id types of 0x00 and 0xFE are not allowed in the signed_multi_table

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

//jjustman-2019-08-30 - DEPRECATED
typedef struct udp_flow_latest_mpu_sequence_number_container {
    uint32_t udp_flows_n;
    udp_flow_packet_id_mpu_sequence_tuple_t** udp_flows;
    
} udp_flow_latest_mpu_sequence_number_container_t;
//jjustman-2019-08-30 - DEPRECATED


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
    atsc3_lls_slt_service_t* atsc3_lls_slt_service;
    
    uint32_t sls_source_ip_address;
	bool	 sls_relax_source_ip_check;

    uint32_t sls_destination_ip_address;
    uint16_t sls_destination_udp_port;
    
    uint16_t video_packet_id;
    uint16_t audio_packet_id;
    uint16_t stpp_packet_id;

    udp_flow_packet_id_mpu_sequence_tuple_t* last_udp_flow_packet_id_mpu_sequence_tuple_audio;
    bool last_udp_flow_packet_id_mpu_sequence_tuple_audio_processed;
    udp_flow_packet_id_mpu_sequence_tuple_t* to_process_udp_flow_packet_id_mpu_sequence_tuple_audio;
    
    udp_flow_packet_id_mpu_sequence_tuple_t* last_udp_flow_packet_id_mpu_sequence_tuple_video;
    bool last_udp_flow_packet_id_mpu_sequence_tuple_video_processed;
    udp_flow_packet_id_mpu_sequence_tuple_t* to_process_udp_flow_packet_id_mpu_sequence_tuple_video;

	udp_flow_packet_id_mpu_sequence_tuple_t* last_udp_flow_packet_id_mpu_sequence_tuple_stpp;


	mmt_arguments_t* mmt_arguments;
    mmt_session_t* mmt_session;
    
} lls_sls_mmt_session_t;


/**
 * used to store all mmt active sessions for this flow
 */
typedef struct lls_sls_mmt_session_flows {
    lls_table_t* lls_table_slt;

    ATSC3_VECTOR_BUILDER_STRUCT(lls_sls_mmt_session);
    
    //jjustman-2019-10-03 - refactorted to ATSC3_VECTOR_BUILDER_STRUCT
    //    int lls_slt_mmt_sessions_n;
    //    lls_sls_mmt_session_t** lls_slt_mmt_sessions;

} lls_sls_mmt_session_flows_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(lls_sls_mmt_session_flows, lls_sls_mmt_session);

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
	//jjustman-2019-10-03 - hack-ish
	atsc3_lls_slt_service_t* atsc3_lls_slt_service;

	bool sls_relax_source_ip_check;
	uint32_t sls_source_ip_address;

	uint32_t sls_destination_ip_address;
	uint16_t sls_destination_udp_port;

	atsc3_alc_arguments_t* alc_arguments;
	atsc3_alc_session_t* alc_session;

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
    
	atsc3_lls_slt_service_t* 				atsc3_lls_slt_service;

    lls_sls_mmt_session_t* 					lls_mmt_session;

    uint16_t 								video_packet_id;
    uint16_t 								audio_packet_id;
    uint16_t								stpp_packet_id;

    lls_sls_monitor_output_buffer_t 		lls_sls_monitor_output_buffer;
    lls_sls_monitor_output_buffer_mode_t 	lls_sls_monitor_output_buffer_mode;

} lls_sls_mmt_monitor_t;




/**
 * used to store all alc active sessions for this flow
 */
typedef struct lls_sls_alc_session_flows {
	lls_table_t* 			lls_table_slt;

	ATSC3_VECTOR_BUILDER_STRUCT(lls_sls_alc_session);

	//TODO - jjustman-2019-10-03 - swap this to ATSC3_VECTOR_BUILDER_STRUCT
	//int 					lls_slt_alc_sessions_n;
	//lls_sls_alc_session_t** lls_slt_alc_sessions;

} lls_sls_alc_session_flows_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(lls_sls_alc_session_flows, lls_sls_alc_session);


 
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
	atsc3_lls_slt_service_t* 				atsc3_lls_slt_service;
    
	lls_sls_alc_session_t* 					lls_alc_session;
	
	atsc3_fdt_instance_t* 					atsc3_fdt_instance;
	atsc3_fdt_instance_t* 					atsc3_fdt_instance_pending; 			//used for a new pending fdt_instance for our SLS, invoked in atsc3_route_sls_process_from_alc_packet_and_file

    atsc3_sls_metadata_fragments_t* 		atsc3_sls_metadata_fragments;
    atsc3_sls_metadata_fragments_t* 		atsc3_sls_metadata_fragments_pending;	//used for a new pending set of SLS fragments, which need to be checked for changes before re-dispatching events

    uint32_t 	usbd_tsi;
	uint32_t 	stsid_tsi;
	uint32_t 	apd_tsi;
	uint32_t 	mpd_tsi;
	uint32_t 	held_tsi;
	uint32_t 	dwd_tsi;
	
    bool		has_discontiguous_toi_flow;
	block_t* 	last_mpd_payload;
    block_t* 	last_mpd_payload_patched;

    atsc3_sls_alc_flow_v 	atsc3_sls_alc_all_s_tsid_flow_v;

    //atsc3_sls_alc_flow_v 	atsc3_sls_alc_all_mediainfo_flow_v;
	
	//method callback handlers
    atsc3_alc_on_object_close_flag_s_tsid_content_location_f	atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_callback;
	
	atsc3_alc_on_route_mpd_patched_f    						atsc3_lls_sls_alc_on_route_mpd_patched_callback;                             //dispatched in atsc3_route_sls_processor.c
	atsc3_alc_on_route_mpd_patched_with_filename_f		 		atsc3_lls_sls_alc_on_route_mpd_patched_with_filename_callback;				//dispatched in atsc3_route_sls_processor.c
	
	atsc3_alc_on_package_extract_completed_f					atsc3_lls_sls_alc_on_package_extract_completed_callback;

	//this should be in the sls_monitor...
	atsc3_sls_on_held_trigger_received_f						atsc3_sls_on_held_trigger_received_callback;
	atsc3_sls_on_held_trigger_received_with_version_f			atsc3_sls_on_held_trigger_received_with_version_callback;


	uint64_t													lct_packets_received_count;


    //jjustman-2020-07-01 #WI - todo: dispatch HELD block_t* payload to application callback

	//<?xml version="1.0" encoding="UTF-8"?>
    //<HELD xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/AppSignaling/HELD/1.0/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
    //<HTMLEntryPackage appContextId="tag:sinclairplatform.com,2020:KSNV:2089" appRendering="false" bcastEntryPackageUrl="App.pkg" bcastEntryPageUrl="index.html" coupledServices="5004"/>
    //</HELD>

    //jjustman-2020-07-01 1569: dispatch async event notification that package extraction has completed
    //codePoint==3 || codePoint == 4
    // filesystem path that .pkg was extracted to bcastEntryPackageUrl/
    // appContextIdList (scope)
    // list<string> objects ~

    //A/344 - ICR - when you receieve a packageExtraction complete with appContextIdList, and then within ~5s receive a HELD emission
    //                  then you can launch the <bcastEntryPackageUrl, bcastEntryPageUrl>


    //only used in special debugging cases
	atsc3_sls_alc_flow_t* audio_tsi_manual_override;
	atsc3_sls_alc_flow_t* video_tsi_manual_override;
	atsc3_sls_alc_flow_t* text_tsi_manual_override;
	atsc3_sls_alc_flow_t* data_tsi_manual_override;
	
	//only used for ffplay re-constituion for alc flows
    uint32_t last_pending_flushed_audio_toi;
    uint32_t last_pending_flushed_video_toi;
    uint32_t last_pending_flushed_text_toi;

    uint32_t last_completed_flushed_audio_toi;
    uint32_t last_completed_flushed_video_toi;
    uint32_t last_completed_flushed_text_toi;
	
    lls_sls_monitor_output_buffer_t 		lls_sls_monitor_output_buffer;
    lls_sls_monitor_output_buffer_mode_t 	lls_sls_monitor_output_buffer_mode;
} lls_sls_alc_monitor_t;

typedef struct lls_slt_service_id {
	uint16_t					service_id;
	struct timeval				time_added;
	struct timeval				time_last_slt_update;
} lls_slt_service_id_t;

typedef struct lls_slt_service_id_group_id_cache {
	uint8_t 	lls_group_id;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_lls_slt_service_cache);
} lls_slt_service_id_group_id_cache_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(lls_slt_service_id_group_id_cache, atsc3_lls_slt_service_cache);


typedef struct lls_slt_monitor {

	//LLS SLT service_id's we are monitoring
	ATSC3_VECTOR_BUILDER_STRUCT(lls_slt_service_id);

	//jjustman-2019-10-03 - TODO: refactor out this usage...
	lls_sls_mmt_monitor_t* lls_sls_mmt_monitor;
	//representative mmt SLS - global monitor is what we are building mfu or mpu's
    ATSC3_VECTOR_BUILDER_STRUCT(lls_sls_mmt_monitor);

	//representative mmt SLS - session_flows is the MMTP flow udp/port/packet_id tuples
    ATSC3_VECTOR_BUILDER_STRUCT(lls_sls_mmt_session_flows);

	//jjustman-2019-10-03 - TODO: refactor out this usage...
	lls_sls_alc_monitor_t* lls_sls_alc_monitor;
	//representative alc SLS - monitor is what we are building our TOI's from our listener
    ATSC3_VECTOR_BUILDER_STRUCT(lls_sls_alc_monitor);

	//representative alc SLS - session_flows are all the ALC flow udp/port/tsi tuples
    ATSC3_VECTOR_BUILDER_STRUCT(lls_sls_alc_session_flows);

	//jjustman-2019-10-19: todo: keep track of lls_slt tables by group_id
	ATSC3_VECTOR_BUILDER_STRUCT(lls_slt_service_id_group_id_cache);

    //LATEST:	last successfully processed SLT table
    lls_table_t* lls_latest_slt_table;

    //LATEST: 	last successfully processed AEAT table
	//			use this against aeat_table_latest.atsc3_aeat_table_t
    lls_table_t* lls_latest_aeat_table;
    
    //LATEST: 	last successfully processed on screen message notification table
    //use this against on_screen_message_notification
    lls_table_t* lls_latest_on_screen_message_notification_table;

    //jjustman-2019-10-12 - adding lls event callback hooks

    //defined in atsc3_monitor_events_lls.h
	atsc3_lls_on_sls_table_present_f								atsc3_lls_on_sls_table_present_callback;
	atsc3_lls_on_rrt_table_present_f								atsc3_lls_on_rrt_table_present_callback;
	atsc3_lls_on_systemtime_table_present_f							atsc3_lls_on_systemtime_table_present_callback;
	atsc3_lls_on_aeat_table_present_f								atsc3_lls_on_aeat_table_present_callback;
	atsc3_lls_on_onscreenmessagenotification_table_present_f		atsc3_lls_on_onscreenmessagenotification_table_present_callback;
	atsc3_lls_on_certificationdata_table_present_f					atsc3_lls_on_certificationdata_table_present_callback;
	atsc3_lls_on_signedmultitable_table_present_f					atsc3_lls_on_signedmultitable_table_present_callback;
	atsc3_lls_on_userdefined_table_present_f						atsc3_lls_on_userdefined_table_present_callback;

} lls_slt_monitor_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(lls_slt_monitor, lls_slt_service_id);

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(lls_slt_monitor, lls_sls_mmt_monitor);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(lls_slt_monitor, lls_sls_mmt_session_flows);

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(lls_slt_monitor, lls_sls_alc_monitor);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(lls_slt_monitor, lls_sls_alc_session_flows);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(lls_slt_monitor, lls_slt_service_id_group_id_cache);


lls_slt_service_id_t* lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service_t* atsc3_lls_slt_service);

lls_slt_service_id_group_id_cache_t* lls_slt_monitor_find_lls_slt_service_id_group_id_cache_from_lls_group_id(lls_slt_monitor_t* lls_slt_monitor, uint8_t lls_group_id);
lls_slt_service_id_group_id_cache_t* lls_slt_monitor_find_or_create_lls_slt_service_id_group_id_cache_from_lls_group_id(lls_slt_monitor_t* lls_slt_monitor, uint8_t lls_group_id);
atsc3_lls_slt_service_t* lls_slt_monitor_add_or_update_lls_slt_service_id_group_id_cache_entry(lls_slt_monitor_t* lls_slt_monitor, uint16_t lls_group_id, atsc3_lls_slt_service_t* atsc3_lls_slt_service);
atsc3_lls_slt_service_t* lls_slt_monitor_find_lls_slt_service_id_group_id_cache_entry(lls_slt_monitor_t* lls_slt_monitor, uint16_t service_id);

void atsc3_lls_sls_alc_monitor_increment_lct_packet_received_count(lls_sls_alc_monitor_t* lls_sls_alc_monitor);
void atsc3_lls_sls_alc_monitor_check_all_s_tsid_flows_has_given_up_route_objects(lls_sls_alc_monitor_t* lls_sls_alc_monitor);

bool atsc3_lls_sls_alc_monitor_sls_metadata_fragements_has_held_changed(lls_sls_alc_monitor_t* atsc3_lls_sls_alc_monitor, atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments_pending);
block_t* atsc3_sls_metadata_fragements_get_sls_held_fragment_duplicate_raw_xml_or_empty(atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments_pending);


#define _ATSC3_LLS_TYPES_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_LLS_TYPES_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);;
#define _ATSC3_LLS_TYPES_INFO(...)    if(_LLS_TYPES_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_LLS_TYPES_DEBUG(...)   if(_LLS_TYPES_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_LLS_TYPES_TRACE(...)   if(_LLS_TYPES_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }



#if defined (__cplusplus)
}
#endif




#endif /* ATSC3_LLS_TYPES_H_ */
