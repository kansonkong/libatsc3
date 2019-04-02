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
#include "atsc3_vector_builder.h"

typedef struct atsc3_route_session_source_flow {


} atsc3_route_session_source_flow_t;

typedef struct atsc3_route_session_repair_flow {


} atsc3_route_session_repair_flow_t;

typedef struct atsc3_route_session_layered_coding_transport {
	uint32_t 	tsi;
	uint32_t	bw;

	//todo - correct these mappings
	char*		start_time;
	char*		end_time;
	atsc3_route_session_source_flow_t atsc3_route_session_source_flow;
	atsc3_route_session_repair_flow_t atsc3_route_session_repairflow;

} atsc3_route_session_layered_coding_transport_t;

typedef struct atsc3_route_session {
	uint32_t	dest_ip_addr;
	uint16_t	dest_port;
	uint32_t	src_ip_addr;
	atsc3_route_session_layered_coding_transport_t atsc3_route_session_layered_coding_transport;

} atsc3_route_session_t;

typedef struct atsc3_route_s_tsid {

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_route_session)
} atsc3_route_s_tsid_t;


ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_route_s_tsid, atsc3_route_session)

#endif /* ATSC3_ROUTE_S_TSID_H_ */
