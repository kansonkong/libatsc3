/*
 * atsc3_fdt.h
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */
#include <stdio.h>
#include <string.h>
#include "atsc3_vector_builder.h"
#include "xml.h"


#ifndef ATSC3_FDT_H_
#define ATSC3_FDT_H_
/**
 *
 * https://www.ietf.org/rfc/rfc6726.txt
 *

   <?xml version="1.0" encoding="UTF-8"?>
   <xs:schema xmlns="urn:ietf:params:xml:ns:fdt"
              xmlns:xs="http://www.w3.org/2001/XMLSchema"
              targetNamespace="urn:ietf:params:xml:ns:fdt"
              elementFormDefault="qualified">
     <xs:element name="FDT-Instance" type="FDT-InstanceType"/>
     <xs:complexType name="FDT-InstanceType">
       <xs:sequence>
         <xs:element name="File" type="FileType" maxOccurs="unbounded"/>
         <xs:any namespace="##other" processContents="skip"
                 minOccurs="0" maxOccurs="unbounded"/>
       </xs:sequence>
       <xs:attribute name="Expires"
                     type="xs:string"
                     use="required"/>
       <xs:attribute name="Complete"
                     type="xs:boolean"
                     use="optional"/>
       <xs:attribute name="Content-Type"
                     type="xs:string"
                     use="optional"/>
       <xs:attribute name="Content-Encoding"
                     type="xs:string"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-FEC-Encoding-ID"
                     type="xs:unsignedByte"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-FEC-Instance-ID"
                     type="xs:unsignedLong"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-Maximum-Source-Block-Length"
                     type="xs:unsignedLong"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-Encoding-Symbol-Length"
                     type="xs:unsignedLong"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-Max-Number-of-Encoding-Symbols"
                     type="xs:unsignedLong"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-Scheme-Specific-Info"
                     type="xs:base64Binary"
                     use="optional"/>
       <xs:anyAttribute processContents="skip"/>
     </xs:complexType>
     <xs:complexType name="FileType">
       <xs:sequence>
         <xs:any namespace="##other" processContents="skip"
                 minOccurs="0" maxOccurs="unbounded"/>
       </xs:sequence>
       <xs:attribute name="Content-Location"
                     type="xs:anyURI"
                     use="required"/>
       <xs:attribute name="TOI"
                     type="xs:positiveInteger"
                     use="required"/>
       <xs:attribute name="Content-Length"
                     type="xs:unsignedLong"
                     use="optional"/>
       <xs:attribute name="Transfer-Length"
                     type="xs:unsignedLong"
                     use="optional"/>
       <xs:attribute name="Content-Type"
                     type="xs:string"
                     use="optional"/>
       <xs:attribute name="Content-Encoding"
                     type="xs:string"
                     use="optional"/>
       <xs:attribute name="Content-MD5"
                     type="xs:base64Binary"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-FEC-Encoding-ID"
                     type="xs:unsignedByte"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-FEC-Instance-ID"
                     type="xs:unsignedLong"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-Maximum-Source-Block-Length"
                     type="xs:unsignedLong"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-Encoding-Symbol-Length"
                     type="xs:unsignedLong"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-Max-Number-of-Encoding-Symbols"
                     type="xs:unsignedLong"
                     use="optional"/>
       <xs:attribute name="FEC-OTI-Scheme-Specific-Info"
                     type="xs:base64Binary"
                     use="optional"/>
       <xs:anyAttribute processContents="skip"/>
     </xs:complexType>
   </xs:schema>
   END


A.3.3.2.3 ATSC Extensions to the FDT-Instance Element

 @efdtVersion – An 8-bit unsigned integer value that shall represent the version of this EFDT element.
   The version shall be increased by one modulo 256 each time the EFDT element is updated.
   @version is an ATSC-defined extension of the FLUTE FDT as specified in RFC 6726 [30].



   Sample XML payload:

<?xml version="1.0" encoding="UTF-8"?>
<FDT-Instance xmlns="urn:ietf:params:xml:ns:fdt"
              xmlns:afdt="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ATSC-FDT/1.0/"
              Expires="4294967295"
              afdt:efdtVersion="47">
   <File Content-Location="sls"
         TOI="196655"
         Content-Length="2902"
         Content-Type="multipart/related"/>
</FDT-Instance>

 
 Alternate EFDT representation
 
 <?xml version="1.0" encoding="UTF-8"?>
 <EFDT xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/S-TSID/1.0/" xmlns:fdt="urn:ietf:params:xml:ns:fdt" xmlns:afdt="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ATSC-FDT/1.0/">
    <FDT-Instance afdt:efdtVersion="82" Expires="4074963450">
        <fdt:File Content-Length="9016" Content-Location="sls" Content-Type="application/mbms-envelope+xml" TOI="4653138"/>
    </FDT-Instance>
 </EFDT>


 */


typedef struct atsc3_fdt_fec_attributes {

	uint8_t 	fec_oti_fec_encoding_id;
	uint32_t 	fec_oti_fec_instance_id;
	uint32_t 	fec_oti_maximum_source_block_length;
	uint32_t 	fec_oti_encoding_symbol_length;
	uint32_t 	fec_oti_max_number_of_encoding_symbols;
	char*	 	fec_oti_sceheme_specific_info;
} atsc3_fdt_fec_attributes_t;

/**
 * https://www.etsi.org/deliver/etsi_ts/126300_126399/126346/13.06.00_60/ts_126346v130600p.pdf
 *
 *<xs:complexType name="FileType">
 <xs:sequence>
 <xs:element ref="mbms2007:Cache-Control" minOccurs="0"/>
 <xs:element ref="sv:delimiter"/>
 <xs:element ref="mbms2012:Alternate-Content-Location-1" minOccurs="0"
maxOccurs="unbounded"/>
 <xs:element ref="mbms2012:Alternate-Content-Location-2" minOccurs="0"
maxOccurs="unbounded"/>
 <xs:element ref="sv:delimiter"/>
 <xs:any namespace="##other" processContents="skip" minOccurs="0" maxOccurs="unbounded"/>
 <xs:element name="Group" type="mbms2005:groupIdType" minOccurs="0"
maxOccurs="unbounded"/>
 <xs:element name="MBMS-Session-Identity" type="mbms2005:MBMS-Session-Identity-Type"
minOccurs="0" maxOccurs="unbounded"/>
 </xs:sequence>

 <xs:attribute name="Content-Location" type="xs:anyURI" use="required"/>
 <xs:attribute name="TOI" type="xs:positiveInteger" use="required"/>
 <xs:attribute name="Content-Length" type="xs:unsignedLong" use="optional"/>
 <xs:attribute name="Transfer-Length" type="xs:unsignedLong" use="optional"/>
 <xs:attribute name="Content-Type" type="xs:string" use="optional"/>
 <xs:attribute name="Content-Encoding" type="xs:string" use="optional"/>
 <xs:attribute name="Content-MD5" type="xs:base64Binary" use="optional"/>
 <xs:attribute name="FEC-OTI-FEC-Encoding-ID" type="xs:unsignedLong" use="optional"/>
 <xs:attribute name="FEC-OTI-FEC-Instance-ID" type="xs:unsignedLong" use="optional"/>
 <xs:attribute name="FEC-OTI-Maximum-Source-Block-Length" type="xs:unsignedLong"
use="optional"/>
 <xs:attribute name="FEC-OTI-Encoding-Symbol-Length" type="xs:unsignedLong" use="optional"/>
 <xs:attribute name="FEC-OTI-Max-Number-of-Encoding-Symbols" type="xs:unsignedLong"
use="optional"/>
 <xs:attribute name="FEC-OTI-Scheme-Specific-Info" type="xs:base64Binary" use="optional"/>
 <xs:attribute ref="mbms2009:Decryption-KEY-URI" use="optional"/>
 <xs:attribute ref="mbms2012:FEC-Redundancy-Level" use="optional"/>
 <xs:attribute ref="mbms2012:File-ETag" use="optional"/>
 <xs:attribute ref="mbms2015:IndependentUnitPositions" use="optional"/>
 <xs:anyAttribute processContents="skip"/>
 </xs:complexType>


jjustman-2020-07-07: A/331:2020

    Table A.3.4 ATSC-Defined Extensions to FDT-Instance.File Element

        appContextIdList
        filterCodes


 */

typedef struct atsc3_fdt_file {
	char* 						content_location;
	uint32_t 					toi;
	uint32_t 					content_length;
	uint32_t 					transfer_length;
	char*						content_type;
	char*						content_encoding;
	char*						content_md5;

	char*						app_context_id_list;
	//todo: impl appContextIdList as vector(string)

	char*						filter_codes;
	//todo: impl appContextIdList as vector(uint32_t)

	atsc3_fdt_fec_attributes_t	atsc3_fdt_fec_attributes;
} atsc3_fdt_file_t;

/*
 *
 xmlns:afdt="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ATSC-FDT/1.0/"

    <xs:attribute name="efdtVersion" type="xs:unsignedByte"/>
	<xs:attribute name="maxExpiresDelta" type="xs:unsignedInt"/>
	<xs:attribute name="maxTransportSize" type="xs:unsignedInt"/>
	<xs:attribute name="fileTemplate" type="afdt:fileTemplateType"/>
	<xs:attribute name="appContextIdList" type="afdt:uriListType"/>
	<xs:attribute name="filterCodes" type="afdt:listOfUnsignedIntType"/>


	<xs:simpleType name="uriListType">
		<xs:list itemType="xs:anyURI"/>
	</xs:simpleType>
	<xs:simpleType name="fileTemplateType">
		<xs:restriction base="xs:string"/>
	</xs:simpleType>
	<xs:simpleType name="listOfUnsignedIntType">
		<xs:list itemType="xs:unsignedInt"/>
	</xs:simpleType>
 *
 *
 *
 jjustman-2020-07-07 - A/331:2020 - Additional elements as defined in A.3.3.2.3 - ATSC Extensions to the FDT-Instance Element

	edftVersion
    maxExpiresDelta
    maxTransportSize
    appContextIdList
    fileTemplate
    filterCodes


 */

typedef struct atsc3_fdt_instance {
	uint32_t 					expires;
	bool 						complete;
	char* 						content_type;
	char* 						content_encoding;
	atsc3_fdt_fec_attributes_t 	atsc3_fdt_fec_attributes;

	/*atsc-fdt/1.0*/
	uint8_t						efdt_version;
	uint32_t					max_expires_delta;
	uint32_t					max_transport_size;
	char*						app_context_id_list;
	char*						file_template;
	char*						filter_codes;

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_fdt_file)

} atsc3_fdt_instance_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_fdt_instance, atsc3_fdt_file)


void atsc3_fdt_instance_free(atsc3_fdt_instance_t** atsc3_fdt_instance_p);



#endif /* ATSC3_FDT_H_ */
