/*
 * atsc3_a332_oma_bcast_types.h
 *
 *  Created on: Sep 24, 2019
 *      Author: jjustman
 *
 *
 *	baseline parsing support for the following OMA BCAST types:
 *
 *		Content-Type="application/vnd.oma.bcast.sgdd+xml"
 *		Content-Type="application/vnd.oma.bcast.sgdu"
 *
 *	including
 *
 *	Content-Encoding="gzip"/>
 *
 *
 *	Reference(s):
 *
 *	A/332:2017 - ATSC Standard: Service Announcement
 *		https://www.atsc.org/wp-content/uploads/2017/03/A332-2017a-Service-Announcement-2.pdf
 *
 *		Schemas: https://www.atsc.org/wp-content/uploads/2017/03/SA-1.0-20170921.zip
 *
 *	OMA: “Service Guide for Mobile Broadcast Services,” Version 1.0.1, document OMA- TS-BCAST_Service_Guide-V1_0_1-20130109-A, Open Mobile Alliance, 9 January 2013.
		http://www.openmobilealliance.org/release/BCAST/V1_0_1-20130109-A/OMA-TS-BCAST_Service_Guide-V1_0_1-20130109-A.pdf

 *	OMA: “Service Guide for Mobile Broadcast Services,” Version 1.1, document OMA-TS- BCAST_Service_Guide-V1_1-20131029-A, Open Mobile Alliance, 29 October 2013.
 	 	 http://www.openmobilealliance.org/release/BCAST/V1_1-20131029-A/OMA-TS-BCAST_Service_Guide-V1_1-20131029-A.pdf

 *	FLUTE (v1): https://tools.ietf.org/html/rfc3926
 *
 *
 */

#include <atsc3_utils.h>
#include <atsc3_vector_builder.h>

#ifndef ATSC3_A332_OMA_BCAST_TYPES_H_
#define ATSC3_A332_OMA_BCAST_TYPES_H_


/**
 * A/332
 *
 *
 * 5.5 SG Delivery
5.5.1 SG delivery over Broadcast
When SG data are delivered via broadcast, the SGDUs and SGDDs shall be delivered as specified in Section 5.4.2 of the OMA BCAST SG specification [11], except that a single LCT component of a ROUTE [3] session (called the Service Guide Announcement Channel) shall be used for delivery of the SGDDs. SGDUs are referenced from within SGDDs by means of the ServiceGuideDeliveryDescriptor.DescriptorEntry.Transport element. If AL-FEC coding is not employed, SGDUs shall be transported using the modified Compact No-Code FEC scheme defined in A/331 [3] where the FEC Payload ID is formatted as a 32-bit start_offset. If AL- FEC coding is employed, SGDUs shall be transported using the RaptorQ scheme [17]. EFDTs shall be transmitted in the transport session delivering the Service Guide fragments.
 *
 *
 *
 * OMA TS BCAST
 *
 *
 * 5.4.1.5.1 Transport dependencies
Similarly as in the case of the network using more than one SGDU frame for delivering the Service Guide fragments, the network can also use multiple SGDDs for declaring the exhaustive list of the fragments in the Service Guide. In such a case it is easy to see that in order for the terminals to be aware of all the Service Guide fragments, the terminals need to be aware of all the SGDDs the network uses. For the broadcast delivery of the SGDDs, the network SHALL therefore place all the SGDDs representing a Service Guide into one and only one delivery session. This session is called the Service Guide Announcement Channel. The network SHALL also make sure that the SGDDs declare all fragments that are delivered over the broadcast channel.
As mentioned before for interactive delivery of the service guide, the main role of the SGDD is to declare all fragments that describe one or more services. The information about division of the fragments into SGDUs in this case is not essential, since all fragments are retrieved interactively and individually for each terminal, and thus a fixed division into SGDUs does not exist. However, the grouping in the service layer can be used to provide information about fragments belonging to the same service. The SGDD MAY declare fragments that are delivered over the interaction channel, but it SHALL at least declare a set of fragments that allow interactive retrieval of the complete SG. For example, the SGDD could declare only ‘Service’ fragments. The terminal could then interactively retrieve fragments related to specific selected services, using the request mechanism described in section 5.4.3.
 *
 *
 *
 *
 * 5.4.1.3 Service Guide fragment encapsulation
In order to deliver the fragments from the network to the terminals the network needs to be able to place the fragments into
the underlying transport frames. The network is provided with means of delivering more than one fragment as a atomic unit
at one time but on the other hand the network is not restricted to deliver all the fragments at one go either. For the terminals
to correctly receive and process any collection of fragments as one delivery unit the network SHALL comply with the
following:
 The Service Guide Delivery Unit structure as defined in Table 1 SHALL be used for encapsulating Service Guide
fragments for transport.
 The field ‘fragmentTransportID’ SHALL be assigned with the ‘transportID’ values as defined in section 5.4.1.1 to
identify each of the fragments carried in the Service Guide Delivery Unit.
 When encapsulating the fragments into the Service Guide Delivery Unit, the mapping defined in section 5.4.1.1
SHALL be used.
 In case the SGDUs are listed in any FDT Instances the corresponding ‘Content-Type’ attributes SHALL be set to
“application/vnd.oma.bcast.sgdu” to describe that the transport object contains an SGDU.
Using the ‘fragmentTransportID’ and ‘fragmentVersion’ fields the terminal can quickly infer whether the associated
fragment in the SGDU has changed.
Data Field Name Data Type
Service_Guide_Delivery_Unit {
 Unit_Header {
 extension_offset uimsbf32
 reserved 16 bits
 n_o_service_guide_fragments uimsbf24
 for(i=0; i< n_o_service_guide_fragments; i++) {
 fragmentTransportID[i] uimsbf32
 fragmentVersion[i] uimsbf32
 offset[i] uimsbf32
 }
 }
 Unit_Payload {
 for(i=0; i< n_o_service_guide_fragments; i++) {
 fragmentEncoding[i] uimsbf8
 if(fragmentEncoding[i]=0) {
OMA-TS-BCAST_Service_Guide-V1_1-20131029-A Page 149 (299)
 2013 Open Mobile Alliance Ltd. All Rights Reserved.
Used with the permission of the Open Mobile Alliance Ltd. under the terms as stated in this document. [OMA-Template-Spec-20130101-I]
 fragmentType uimsbf8
 XMLFragment bytestring
 }
 else if(fragmentEncoding[i]=1) {
 validFrom uimsbf32
 validTo uimsbf32
 fragmentID bytestring
 SDPfragment bytestring
 }
 else if(fragmentEncoding[i]=2) {
 validFrom uimsbf32
 validTo uimsbf32
 fragmentID bytestring
 USBDfragment bytestring
 }
 else if(fragmentEncoding[i]=3) {
 validFrom uimsbf32
 validTo uimsbf32
 fragmentID bytestring
 ADPfragment bytestring
 }
 }
 }
 if(extension_offset>0) {
 extension_type uimsbf8
 next_extension_offset uimsbf32
 extension_data bitstring
 }
}
Table 1: Service Guide Delivery Unit structure
uimsbfN Unsigned Nbit Integer, most significant bit first
bytestring Array of bytes, each occupying eight bits
bitstring Array of bits, length is multiple of eight
Table 2: Mnemonics used in Table 1
extension_offset Offset in bytes from the start of the Unit_Payload to the start of the
first extension. Set to 0 if there is no extension Present.
reserved A bitfield reserved for future extensions of BCAST. All bits in this
field SHALL be set to 0 in an SGDU conforming to this
specification. Terminals MAY choose to ignore this field.
n_o_service_guide_fragments Number of Service Guide fragments encapsulated in this specific
Delivery Unit.
offset[i] Offset in bytes from the start of the Unit_Payload to the start of the
fragment_encoding field of the i:th Service Guide fragment. The
OMA-TS-BCAST_Service_Guide-V1_1-20131029-A Page 150 (299)
 2013 Open Mobile Alliance Ltd. All Rights Reserved.
Used with the permission of the Open Mobile Alliance Ltd. under the terms as stated in this document. [OMA-Template-Spec-20130101-I]
offset list is sorted in ascending order.
fragmentTransportID[i] Signals the identifier of the i:th Service Guide fragment which is
defined for transport (see 5.4.1.5)
fragmentVersion[i] Signals the version of the i:th Service Guide fragment.
Note: The scope of the fragmentVersion is limited to this transport
session. The value of fragmentVersion can turn over from 2^32-1 to
0.
fragmentEncoding[i] Signals the encoding of the i:th Service Guide fragment, with the
following values:
0 – XML encoded OMA BCAST Service Guide fragment
1 – SDP fragment
2 – MBMS User Service Bundle Description (USBD) as specified
in [3GPP TS 26.346] (see 5.1.2.4 ‘SessionDescription’ element)
3 – XML encoded Associated Delivery Procedure as specified in
[BCAST11-Distribution] section 5.3.4.
4-127 – reserved for future BCAST extensions
128-255 – available for proprietary extensions
fragmentType[i] This field signals the type of an XML encoded BCAST Service
Guide fragment, with the following values:
0 – unspecified
1 – ‘Service’ Fragment
2 – ‘Content’ fragment
3 – ‘Schedule’ Fragment
4 – ‘Access’ Fragment
5 – ‘PurchaseItem’ Fragment
6 – ‘PurchaseData’ Fragment
7– ‘PurchaseChannel’ Fragment
8 – ‘PreviewData’ Fragment
9 – ‘InteractivityData’ Fragment
10-127 – reserved for BCAST extensions
128-255 – available for proprietary extensions
fragmentID Null-terminated string containing the fragment ID of an SDP or
MBMS USBD or Associated Delivery Procedure fragment as
referenced from an ‘Access’ fragment via
SessionDescriptionReference.
Note: for an XML encoded OMA BCAST Service Guide fragment,
this information is contained in the fragment itself.
validFrom 32 bit word representation of the validFrom value of an SDP or
MBMS USBD or Associated Delivery Procedure fragment. This
field is expressed as the first 32bits integer part of NTP time stamp.
When set to “0” the interpretation is that “validFrom” is undefined.
Note: for an XML encoded OMA BCAST Service Guide fragment,
this information is contained in the fragment itself.
validTo 32 bit word representation of the validTo value of an SDP or
MBMS USBD or Associated Delivery Procedure Description
fragment. This field is expressed as the first 32bits integer part of
NTP time stamp. When set to “0” the interpretation is that
“validTo” is undefined.
Note: for an XML encoded OMA BCAST Service Guide fragment,
this information is contained in the fragment itself.
XMLfragment String containing the actual XML data of the encapsulated Service
OMA-TS-BCAST_Service_Guide-V1_1-20131029-A Page 151 (299)
 2013 Open Mobile Alliance Ltd. All Rights Reserved.
Used with the permission of the Open Mobile Alliance Ltd. under the terms as stated in this document. [OMA-Template-Spec-20130101-I]
Guide fragment without the termination character.
SDPfragment String containing the actual SDP data, without termination
character.
USBDfragment String containing the actual MBMS USBD data, without
termination character.
ADPfragment String containing the actual XML data of the encapsulated
Associated Delivery Procedure fragment, without termination
character.
extension_type Signals the type of the extension.
0-127 – reserved for BCAST extensions
128-255 – available for proprietary extensions
Terminals MAY discard unknown extensions. In any case, they
SHALL NOT get into an error state when they encounter unknown
extensions.
next_extension_offset Offset in bytes from the start of the current extension to the start of
the next extension. Set to 0 if there is no next extension. The start
of an extension is assumed to be the position of the extension type.
extension_data Content of the extension.
Table 3: Semantics for Table 1
5.4.1.4 Compression of Service Guide Delivery Units and Service Guide Delivery
Descriptors
The network is provided with means of reducing the size of the SGDUs and SGDDs being delivered to terminals by using
GZIP compression. For the algorithms and their respective signalling there are the following rules and constraints:
When FLUTE is used for transmission of the SDGUs
 the network MAY compress the SGDUs with the GZIP algorithm,
 terminals SHALL support both plain SGDUs and GZIP compressed SGDUs,
 When GZIP compression is used and the SGDUs are listed in any FDT Instances the corresponding ‘ContentEncoding’ attributes SHALL be set to “gzip”.
Additionally, when FLUTE is used for transmission of the SGDD, the network MAY compress the SGDDs with the GZIP
algorithm. In this case the Content-Encoding attribute in the corresponding description of the FDT SHALL be set to “gzip”.
When ALC/LCT is used for transmission of the SGDUs
 the network MAY compress the SGDUs with the GZIP algorithm,
 terminals SHALL support both plain SGDUs and GZIP compressed SGDUs,
 When GZIP compression is used, the network SHALL signal GZIP compression of SGDUs by including the
EXT_CENC header in the ALC packet of the SGDU.
When HTTP is used for service guide delivery, the network MAY compress the HTTP response body with the GZIP
algorithm. In this case the Content-Encoding attribute in the corresponding description of the HTTP response SHALL be set
to “gzip”.





 */


/**
 *
 *
 *
 *5.4.2 Delivery over the Broadcast Channel
Over the Broadcast Channel, interface SG-5, the Service Guide is delivered using broadcast file delivery sessions. The network places the fragments of the Service Guide into one or more SGDUs and constructs one or more SGDDs to represent the contents of the Service Guide as well as the division of the fragments into the SGDUs. The SGDD(s) and the SGDU(s) are placed into file delivery session(s) to be transported as transport objects, TOs. While the SGDUs can be transported using one or more file delivery sessions, the SGDDs are provided using only one session, namely the Service Guide Announcement Channel as defined in section 5.4.1.5.1.
In the Service Guide Announcement Channel, the network SHALL use FLUTE [RFC 3926] as the broadcast delivery protocol and FDT Instances SHALL therefore be provided. In the Service Guide Delivery Channel, the network SHALL either use FLUTE (in which case FDT instances SHALL be provided) or ALC (in which case FDT Instances SHALL NOT be provided).
 2013 Open Mobile Alliance Ltd. All Rights Reserved.
Used with the permission of the Open Mobile Alliance Ltd. under the terms as stated in this document. [OMA-Template-Spec-20130101-I]

OMA-TS-BCAST_Service_Guide-V1_1-20131029-A Page 187 (299)
 The following enhancements apply for the case when the file information is conveyed in the Service Guide or in a file delivery table:
 SG-D in BSD/A MAY apply the "Compact No-Code FEC scheme" [RFC 3695] (FEC Encoding ID 0, also known as "Null-FEC").
 SG-D in BSD/A MAY utilize the split-TOI scheme as specified in section 5.4.2.1.3 in conjunction with FLUTE, for signalling the identifier and version of any transported object (e.g. the Service Guide Delivery Unit or Service Guide Delivery Descriptor).
 SG-D in BSD/A MAY utilize the scheme as specified in section 5.4.2.1.3 in conjunction with FLUTE, for signalling the identifier and version of the Service Guide Delivery Unit.
In order for the terminals to distuinguish the SGDDs and SGDUs from other transport objects the network SHALL set the ‘Content-Type’ attribute of the ‘File’ element in the FDT Instances
 to “application/vnd.oma.bcast.sgdd+xml” for SGDDs and
 to “application/vnd.oma.bcast.sgdu” for SGDUs.
As there is no signalling whether the network uses FDT Instances in the Service Guide delivery sessions other than the Service Guide Announcement Channel, the terminal
 SHALL assume that the Transport Object Identifier, TOI, zero is reserved for the FDT Instances.
 And the network SHALL not use the TOI zero for any types of files other than FDT Instance.
The network SHALL signal the Forward Error Correction, FEC, parameters for the transport objects in the Service Guide delivery sessions using one of the mechanisms defined in FLUTE [RFC 3926], and the terminal SHALL support all these mechanisms. When ALC/LCT is used for SGDU delivery, the FEC-encoding-ID SHALL be signaled in the LCT header codepoint field, and the EXT_FTI extension header SHALL be used to signal other coding parameters.
5.4.2.1 Signaling Changes in the Service Guide over Broadcast Channel
In the following, the way of signaling changes in Service Guide fragments is specified. The changes in the Service Guide are signalled through the change in the transmitted SGDUs which consequently cause a change in the transmitted SGDDs. Observing these changes, the terminal SHALL be able to determine the change. However, this specification does not specify the normative terminal behavior for this. Informative examples for four cases of localizing changes and achieving their discovery are outlined in section 5.5.1.1.


 */

/*
 example s-tsid for ESG service:
 
	<?xml version="1.0" encoding="UTF-8"?>
	<S-TSID xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/S-TSID/1.0/" xmlns:afdt="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ATSC-FDT/1.0/" xmlns:fdt="urn:ietf:params:xml:ns:fdt">
	   <RS sIpAddr="172.16.200.1" dIpAddr="239.255.50.6" dPort="5006">
		  <LS tsi="50">
			 <SrcFlow>
				<EFDT>
				   <FDT-Instance Expires="4294967295" afdt:efdtVersion="0">
					  <fdt:File TOI="1220" Content-Location="sgdd_1220" Content-Length="45677" Transfer-Length="3931" Content-Type="application/vnd.oma.bcast.sgdd+xml" Content-Encoding="gzip" />
				   </FDT-Instance>
				</EFDT>
				<Payload codePoint="1" formatId="1" frag="0" order="true" />
			 </SrcFlow>
		  </LS>
		  <LS tsi="60">
			 <SrcFlow>
				<EFDT>
				   <FDT-Instance Expires="4294967295" afdt:efdtVersion="0">
					  <fdt:File TOI="3303" Content-Location="sgdu_short_3303" Content-Length="102900" Transfer-Length="11191" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
					  <fdt:File TOI="4439" Content-Location="sgdu_service_schedule_4439" Content-Length="19322" Transfer-Length="2173" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
				   </FDT-Instance>
				</EFDT>
				<Payload codePoint="1" formatId="1" frag="0" order="true" />
			 </SrcFlow>
		  </LS>
		  <LS tsi="70">
			 <SrcFlow>
				<EFDT>
				   <FDT-Instance Expires="4294967295" afdt:efdtVersion="0">
					  <fdt:File TOI="2299" Content-Location="sgdu_long_2299" Content-Length="106689" Transfer-Length="12876" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
					  <fdt:File TOI="2300" Content-Location="sgdu_long_2300" Content-Length="2819" Transfer-Length="999" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
					  <fdt:File TOI="2301" Content-Location="sgdu_long_2301" Content-Length="101356" Transfer-Length="11652" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
					  <fdt:File TOI="2302" Content-Location="sgdu_long_2302" Content-Length="1425" Transfer-Length="709" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
					  <fdt:File TOI="2304" Content-Location="sgdu_long_2304" Content-Length="80136" Transfer-Length="9452" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
					  <fdt:File TOI="4440" Content-Location="sgdu_service_schedule_4440" Content-Length="52972" Transfer-Length="4934" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
				   </FDT-Instance>
				</EFDT>
				<Payload codePoint="1" formatId="1" frag="0" order="true" />
			 </SrcFlow>
		  </LS>
		  <LS tsi="80">
			 <SrcFlow>
				<EFDT>
				   <FDT-Instance Expires="4294967295" afdt:efdtVersion="0">
					  <fdt:File TOI="5873" Content-Location="s53098_ll_h3_ab.png" Content-Length="9034" Transfer-Length="8727" Content-Encoding="gzip" />
					  <fdt:File TOI="5874" Content-Location="s28717_h3_aa.png" Content-Length="56251" Transfer-Length="56173" Content-Encoding="gzip" />
					  <fdt:File TOI="5875" Content-Location="s10269_ll_h3_ab.png" Content-Length="16121" Transfer-Length="16069" Content-Encoding="gzip" />
					  <fdt:File TOI="5876" Content-Location="s11118_ll_h3_ab.png" Content-Length="17470" Transfer-Length="17008" Content-Encoding="gzip" />
				   </FDT-Instance>
				</EFDT>
				<Payload codePoint="1" formatId="1" frag="0" order="true" />
			 </SrcFlow>
		  </LS>
	   </RS>
	</S-TSID>
 
 
 
 -> file instances and types
 
 <fdt:File TOI="1220" Content-Location="sgdd_1220" Content-Length="45677" Transfer-Length="3931" Content-Type="application/vnd.oma.bcast.sgdd+xml" Content-Encoding="gzip" />

 <fdt:File TOI="3303" Content-Location="sgdu_short_3303" Content-Length="102900" Transfer-Length="11191" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
 <fdt:File TOI="4439" Content-Location="sgdu_service_schedule_4439" Content-Length="19322" Transfer-Length="2173" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
 
 <!-- extracted payload has preamble of atsc3_service_guide_delivery_unit_t -->
 
 <fdt:File TOI="2299" Content-Location="sgdu_long_2299" Content-Length="106689" Transfer-Length="12876" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
 <fdt:File TOI="2300" Content-Location="sgdu_long_2300" Content-Length="2819" Transfer-Length="999" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
 <fdt:File TOI="2301" Content-Location="sgdu_long_2301" Content-Length="101356" Transfer-Length="11652" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
 <fdt:File TOI="2302" Content-Location="sgdu_long_2302" Content-Length="1425" Transfer-Length="709" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
 <fdt:File TOI="2304" Content-Location="sgdu_long_2304" Content-Length="80136" Transfer-Length="9452" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
 <fdt:File TOI="4440" Content-Location="sgdu_service_schedule_4440" Content-Length="52972" Transfer-Length="4934" Content-Type="application/vnd.oma.bcast.sgdu" Content-Encoding="gzip" />
 
 
 <fdt:File TOI="5873" Content-Location="s53098_ll_h3_ab.png" Content-Length="9034" Transfer-Length="8727" Content-Encoding="gzip" />
 <fdt:File TOI="5874" Content-Location="s28717_h3_aa.png" Content-Length="56251" Transfer-Length="56173" Content-Encoding="gzip" />
 <fdt:File TOI="5875" Content-Location="s10269_ll_h3_ab.png" Content-Length="16121" Transfer-Length="16069" Content-Encoding="gzip" />
 <fdt:File TOI="5876" Content-Location="s11118_ll_h3_ab.png" Content-Length="17470" Transfer-Length="17008" Content-Encoding="gzip" />


 
 */
/*
 * Service Guide Delivery Descriptor
 * Ref: 5.4.1.5.2 - https://www.openmobilealliance.org/release/BCAST/V1_1-20131029-A/OMA-TS-BCAST_Service_Guide-V1_1-20131029-A.pdf
 *
 * Processing: gzip extraction of payload
 */



/*
 * Service Guide Delivery Unit
 * Ref: 5.4.1.3 - https://www.openmobilealliance.org/release/BCAST/V1_1-20131029-A/OMA-TS-BCAST_Service_Guide-V1_1-20131029-A.pdf
 *
 * Processing: gzip extraction of payload
 * 				extract interior payload
 */

typedef struct atsc3_service_guide_fragment_header {
	uint32_t	fragment_transport_id;
	uint32_t	fragment_version;
	uint32_t	offset;
} atsc3_service_guide_fragment_header_t;

typedef struct atsc3_service_guide_fragment_payload {
	uint8_t 	fragment_encoding;

	//fragmentEncoding == 0;
	uint8_t		fragment_type;
	block_t*	byte_string;

	//fragmentEncoding == 1 || 2 || 3
    uint32_t	valid_from;
    uint32_t	valid_to;
    block_t*	fragment_id;

    //fragmentEncoding == 1
	block_t*	sdp_fragment;

	//fragmentEncoding == 2
	block_t*	usbd_fragment;

	//fragmentEncoding == 3
	block_t*	adp_fragment;


} atsc3_service_guide_fragment_payload_t;

typedef struct atsc3_service_guide_delivery_unit {
	uint32_t 	extension_offset;
	uint16_t	reserved;
	uint32_t	n_o_service_guide_fragments:24;

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_service_guide_fragment_header);
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_service_guide_fragment_payload);

	//extension_offset > 0
	uint8_t		extension_type;
	uint32_t	next_extension_offset;
	block_t*	extension_data;


} atsc3_service_guide_delivery_unit_t;


ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_service_guide_delivery_unit, atsc3_service_guide_fragment_header);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_service_guide_delivery_unit, atsc3_service_guide_fragment_payload);

#endif /* ATSC3_A332_OMA_BCAST_TYPES_H_ */
