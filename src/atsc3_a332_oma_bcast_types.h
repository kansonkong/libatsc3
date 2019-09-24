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

 *
 *
 *
 */

#ifndef ATSC3_A332_OMA_BCAST_TYPES_H_
#define ATSC3_A332_OMA_BCAST_TYPES_H_


/**
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
 */




#endif /* ATSC3_A332_OMA_BCAST_TYPES_H_ */
