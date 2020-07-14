/*
 * atsc3_route_s_tsid.h
 *
 *  Created on: Apr 2, 2019
 *      Author: jjustman
 *
 *
atsc3_mime_multipart_related_parser.c:318:DEBUG:type     : application/route-s-tsid+xml
atsc3_mime_multipart_related_parser.c:319:DEBUG:location : stsid.xml
atsc3_mime_multipart_related_parser.c:320:DEBUG:payload  :

<?xml version="1.0" encoding="UTF-8"?>
<S-TSID xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/S-TSID/1.0/" xmlns:fdt="urn:ietf:params:xml:ns:fdt" xmlns:afdt="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ATSC-FDT/1.0/">
    <RS dIpAddr="239.255.17.1" dPort="8000" sIpAddr="10.172.1.50">
        <LS bw="10000000" tsi="1">
            <SrcFlow rt="true">
                <EFDT>
                    <FDT-Instance afdt:efdtVersion="1" Expires="4000000000" afdt:fileTemplate="test-0-$TOI$.mp4v" afdt:maxTransportSize="1926926">
                        <fdt:File Content-Location="test-0-init.mp4v" TOI="2100000000"/>
                    </FDT-Instance>
                </EFDT>
                <ContentInfo>
                    <MediaInfo contentType="video" repId="0"/>
                </ContentInfo>
                <Payload codePoint="128" formatId="1" frag="0" order="true"/>
            </SrcFlow>
        </LS>
        <LS bw="500000" tsi="2">
            <SrcFlow rt="true">
                <EFDT>
                    <FDT-Instance afdt:efdtVersion="1" Expires="4000000000" afdt:fileTemplate="test-1-$TOI$.mp4a" afdt:maxTransportSize="106531">
                        <fdt:File Content-Location="test-1-init.mp4a" TOI="2100000000"/>
                    </FDT-Instance>
                </EFDT>
                <ContentInfo>
                    <MediaInfo contentType="audio" lang="eng" repId="1"/>
                </ContentInfo>
                <Payload codePoint="128" formatId="1" frag="0" order="true"/>
            </SrcFlow>
        </LS>
    </RS>
</S-TSID>

@startTime – This attribute shall represent the start time of this LCT channel, defined in accordance to the time description, i.e. “t=” line in RFC 4566 [20], and representing the “t=<start-time>” value, The only difference from RFC 4566 [20] is in the format of the session start time – i.e., instead of decimal representation of the NTP (Network Time Protocol) time value, in this specification, the session start time shall be represented by the “dateTime” XML datatype as defined in XSD Datatypes [43]. Absence of this attribute shall be an indication that the start time of this LCT channel occurred at some time in the past.
@endTime – This attribute shall represent the end time of this LCT channel, defined in accordance to the time description, i.e. “t=” line in RFC 4566 [20], and representing the “t=<end-time>” value, The only difference from RFC 4566 [20] is in the format of the session end time – i.e., instead of decimal representation of the NTP (Network Time Protocol) time value, in this specification, the session end time shall be represented by the dateTime XML datatype as defined in XSD Datatypes [43]. Absence of this attribute shall be an indication that the end time of this LCT channel will occur at an undefined time in the future.
 */

#ifndef ATSC3_ROUTE_S_TSID_H_
#define ATSC3_ROUTE_S_TSID_H_

#include "atsc3_utils.h"
#include "xml.h"
#include "atsc3_vector_builder.h"
#include "atsc3_fdt.h"
#include "atsc3_logging_externs.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 *
	<xs:complexType name="EFDTType">
		<xs:sequence>
			<xs:element name="FDT-Instance" type="fdt:FDT-InstanceType" minOccurs="0"/>
			<xs:any namespace="##other" processContents="strict" minOccurs="0" maxOccurs="unbounded"/>
		</xs:sequence>
		<xs:anyAttribute namespace="##other" processContents="strict"/>
	</xs:complexType>
	**/

//use from atsc3_fdt?


/*
 * <xs:complexType name="MediaInfoType">
		<xs:sequence>
			<xs:element name="ContentRating" type="stsid:ContentRatingType"/>
			<xs:any namespace="##other" processContents="strict" minOccurs="0" maxOccurs="unbounded"/>
		</xs:sequence>
		<xs:attribute name="startup" type="xs:boolean"/>
		<xs:attribute ref="xml:lang"/>
		<xs:attribute name="contentType" type="stsid:contentTypeType"/>
		<xs:attribute name="repId" type="stsid:StringNoWhitespaceType" use="required"/>
		<xs:anyAttribute namespace="##other" processContents="strict"/>
	</xs:complexType>
 */
#define ATSC3_ROUTE_S_TSID_RS_LS_SRCFLOW_CONTENTINFO_MEDIAINFO_CHAR_MAX_LEN 128

typedef struct atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo {
	char* 	content_type;
	char* 	lang;
	char*	rep_id;
	bool 	startup;

	//TODO: jjustman-2020-06-02: content_rating object, e.g.
	//<ContentRating value="1,'TV-14 D-L',{0 'TV-14'}{1 'D'}{2 'L'}"/>


} atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t;

//jjustman-2020-07-14 - TODO - create ATSC3_ALLOC define to support: _new, _clone, _free

atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_new();
atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_clone(atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo);
void atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_free(atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t** atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_p);

/*
 * <!-- AEA Media Content -->
	<xs:complexType name="AEAMediaType">
		<xs:sequence>
			<xs:element name="AEAId" type="xs:string" minOccurs="0" maxOccurs="unbounded"/>
			<xs:any namespace="##other" processContents="strict" minOccurs="0" maxOccurs="unbounded"/>
		</xs:sequence>
		<xs:anyAttribute namespace="##other" processContents="strict"/>
	</xs:complexType>
 */
typedef struct atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia {
	char* aea_id;
} atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia_t;

/*
	<!-- ContentInfo Type -->
	<xs:complexType name="ContentInfoType">
		<xs:choice>
			<xs:element name="MediaInfo" type="stsid:MediaInfoType"/>
			<xs:element name="AEAMedia" type="stsid:AEAMediaType"/>
			<xs:any namespace="##other" processContents="strict" minOccurs="0" maxOccurs="unbounded"/>
		</xs:choice>
		<xs:anyAttribute namespace="##other" processContents="strict"/>
	</xs:complexType>

 */
typedef struct atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo {
	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t*	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo;
	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia_t*	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia;
} atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_t;


/*
 * <xs:complexType name="PayloadType">
		<xs:attribute name="codePoint" type="xs:unsignedByte" default="0"/>
		<xs:attribute name="formatId" type="stsid:formatIdType" use="required"/>
		<xs:attribute name="frag" type="stsid:fragType" default="0"/>
		<xs:attribute name="order" type="xs:boolean" default="0"/>
		<xs:attribute name="srcFecPayloadId" type="stsid:srcFecPayloadIdType" use="required"/>
		<xs:attribute name="fecParams" type="stsid:fecOTIType"/>
		<xs:anyAttribute processContents="strict"/>
	</xs:complexType>
 */
typedef struct atsc3_route_s_tsid_RS_LS_SrcFlow_Payload {
	uint8_t codepoint;
	uint8_t format_id;
	uint8_t frag;
	bool	order;
	uint8_t	src_fec_payload_id;
	/**
	 * todo
	 * <xs:simpleType name="fecOTIType">
		<xs:restriction base="xs:hexBinary">
			<xs:length value="12"/>
		</xs:restriction>
	</xs:simpleType>
	 */
	char* fec_parms;

} atsc3_route_s_tsid_RS_LS_SrcFlow_Payload_t;

/*
 *
 * 		<xs:attribute name="rt" type="xs:boolean" default="false"/>
		<xs:attribute name="minBuffSize" type="xs:unsignedInt"/>
		..
		<xs:element name="EFDT" type="stsid:EFDTType" minOccurs="0"/>
		<xs:element name="ContentInfo" type="stsid:ContentInfoType" minOccurs="0"/>
		<xs:element name="Payload" type="stsid:PayloadType" maxOccurs="unbounded"/>
		<xs:any namespace="##other" processContents="strict" minOccurs="0" maxOccurs="unbounded"/>
 *
 */
typedef struct atsc3_route_s_tsid_RS_LS_SrcFlow {
	bool 											rt;
	uint32_t 										min_buff_size;
	atsc3_fdt_instance_t*							atsc3_fdt_instance; //EFDT
	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_t*	atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo;
	atsc3_route_s_tsid_RS_LS_SrcFlow_Payload_t*		atsc3_route_s_tsid_RS_LS_SrcFlow_Payload;

} atsc3_route_s_tsid_RS_LS_SrcFlow_t;

typedef struct atsc3_route_s_tsid_RS_LS_RepairFlow {
	bool	__to_impl__;

} atsc3_route_s_tsid_RS_LS_RepairFlow_t;

/**
 * LCT channel
 	 	<xs:attribute name="tsi" type="xs:unsignedInt" use="required"/>
		<xs:attribute name="bw" type="xs:unsignedInt"/>
		<xs:attribute name="startTime" type="xs:dateTime"/>
		<xs:attribute name="endTime" type="xs:dateTime"/>
 */
typedef struct atsc3_route_s_tsid_RS_LS {
	uint32_t 	tsi;
	uint32_t	bw;

	//todo - correct these mappings
	char*		start_time;
	char*		end_time;
	atsc3_route_s_tsid_RS_LS_SrcFlow_t*		atsc3_route_s_tsid_RS_LS_SrcFlow;
	atsc3_route_s_tsid_RS_LS_RepairFlow_t*	atsc3_route_s_tsid_RS_LS_RepairFlow;

} atsc3_route_s_tsid_RS_LS_t;

/*
 * RS: Route sessions
 */
typedef struct atsc3_route_s_tsid_RS {
	uint32_t	dest_ip_addr;
	uint16_t	dest_port;
	uint32_t	src_ip_addr;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_route_s_tsid_RS_LS);

} atsc3_route_s_tsid_RS_t;
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_route_s_tsid_RS, atsc3_route_s_tsid_RS_LS)

typedef struct atsc3_route_s_tsid {

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_route_s_tsid_RS)
} atsc3_route_s_tsid_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_route_s_tsid, atsc3_route_s_tsid_RS)


atsc3_route_s_tsid_t* atsc3_route_s_tsid_parse_from_payload(char* payload, char* content_location);
atsc3_route_s_tsid_t* atsc3_route_s_tsid_parse_RS(xml_node_t* xml_node, atsc3_route_s_tsid_t* atsc3_route_s_tsid);
atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_parse_RS_LS(xml_node_t* xml_node, atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_RS);
atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_parse_RS_LS_SrcFlow(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS);
atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_parse_RS_LS_RepairFlow(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS);
atsc3_route_s_tsid_RS_LS_SrcFlow_t* atsc3_route_s_tsid_parse_RS_LS_SrcFlow_EFDT(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_SrcFlow_t* atsc3_route_s_tsid_RS_LS_SrcFlow);

atsc3_route_s_tsid_RS_LS_SrcFlow_t* atsc3_route_s_tsid_parse_RS_LS_SrcFlow_ContentInfo(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_SrcFlow_t* atsc3_route_s_tsid_RS_LS_SrcFlow);
atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* atsc3_route_s_tsid_parse_RS_LS_SrcFlow_ContentInfo_MediaInfo(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_t* atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo);
atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_AEAMedia_t*  atsc3_route_s_tsid_parse_RS_LS_SrcFlow_ContentInfo_AEAMedia(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_t* atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo);

atsc3_route_s_tsid_RS_LS_SrcFlow_Payload_t* atsc3_route_s_tsid_parse_RS_LS_SrcFlow_Payload(xml_node_t* xml_node, atsc3_route_s_tsid_RS_LS_SrcFlow_t* atsc3_route_s_tsid_RS_LS_SrcFlow);

void atsc3_route_s_tsid_dump(atsc3_route_s_tsid_t* atsc3_route_s_tsid);


#define _ATSC3_ROUTE_S_TSID_PARSER_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_ROUTE_S_TSID_PARSER_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);;
#define _ATSC3_ROUTE_S_TSID_PARSER_INFO(...)    if(_ROUTE_S_TSID_PARSER_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_ROUTE_S_TSID_PARSER_DEBUG(...)   if(_ROUTE_S_TSID_PARSER_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }


#ifdef __cplusplus
}
#endif

#endif /* ATSC3_ROUTE_S_TSID_H_ */
