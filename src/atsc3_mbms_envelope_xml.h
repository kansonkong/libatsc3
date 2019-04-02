/*
 * atsc3_mbms_envelope_xml.h
 *
 *  Created on: Mar 25, 2019
 *      Author: jjustman
 *
 *
 *    e.g. <fdt:File Content-Length="9016" Content-Location="sls" Content-Type="application/mbms-envelope+xml" TOI="4653138"/>
 *
 *
<?xml version="1.0" encoding="UTF-8"?>
<metadataEnvelope xmlns="urn:3gpp:metadata:2005:MBMS:envelope">
    <item contentType="application/route-usd+xml" metadataURI="usbd.xml" version="1"/>
    <item contentType="application/route-s-tsid+xml" metadataURI="stsid.xml" version="1"/>
    <item contentType="application/dash+xml" metadataURI="mpd.xml" version="82"/>
    <item contentType="application/atsc-held+xml" metadataURI="held.xml" version="1"/>
</metadataEnvelope>

or

	<item contentType="application/sdp" metadataURI="http://usd.ex.com/fragments/sdp-1.sdp"
		validFrom="2012-03-01T18:53:10Z" validUntil="2012-03-07T18:53:10Z" version="1"
		meta:nextURL="http://mynexturl.com"/>
 */

#ifndef ATSC3_MBMS_ENVELOPE_XML_H_
#define ATSC3_MBMS_ENVELOPE_XML_H_

#define ATSC3_MBMS_ENVELOPE_CONTENT_TYPE application/mbms-envelope+xml

typedef struct atsc3_mbms_metadata_item {
	char* content_type;
	char* metadata_uri;
	char* valid_from_string;
	char* valid_until_string;
	uint32_t version;
	char* next_url;
} atsc3_mbms_metadata_item_t;

typedef struct atsc3_mbms_metadata_envelope {

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_mbms_metadata_item);

} atsc3_mbms_metadata_envelope_t;



#endif /* ATSC3_MBMS_ENVELOPE_XML_H_ */
