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



 */

typedef struct atsc3_fdt_file {

} atsc3_fdt_file_t;

typedef struct atsc3_fdt_instance {

	ATSC3_VECTOR_BUILDER(atsc3_fdt_file)

} atsc3_fdt_instance;

#endif /* ATSC3_FDT_H_ */
