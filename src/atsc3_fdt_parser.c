/*
 * atsc3_fdt_parser.c
 *
 *  Created on: Mar 17, 2019
 *      Author: jjustman
 
 
 
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


#include "atsc3_fdt_parser.h"

//TODO: EFDT is a superclass of FDT-instance..

atsc3_fdt_instance_t* atsc3_efdt_instance_parse_from_xml_node(xml_node_t* xml_efdt_node) {
	atsc3_fdt_instance_t* atsc3_fdt_instance = calloc(1, sizeof(atsc3_fdt_instance_t));
	assert(atsc3_fdt_instance);

	//we should only be expecting either an EFDT or FDT-Instance node here
	size_t num_root_children = xml_node_children(xml_efdt_node);
	for (int i = 0; i < num_root_children; i++) {
		xml_node_t* root_child = xml_node_child(xml_efdt_node, i);
		if (xml_node_equals_ignore_case(root_child, "EFDT")) {
			//replace root_child with this child
			if (!xml_node_children(root_child)) {
				_ATSC3_FDT_PARSER_ERROR("atsc3_efdt_xml_node_parse: EDFT contains no children!");
			} else {
				root_child = xml_node_child(root_child, 0);
			}
		}

		if (xml_node_equals_ignore_case(root_child, "FDT-Instance")) {
			atsc3_fdt_parse_from_xml_fdt_instance(atsc3_fdt_instance, root_child);
			size_t num_fdt_children = xml_node_children(root_child);
			for (int j = 0; j < num_fdt_children; j++) {
				xml_node_t* fdt_child = xml_node_child(root_child, j);
				if (xml_node_equals_ignore_case(fdt_child, "File")) {
					atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_file_parse_from_xml_fdt_instance(fdt_child);
					if (atsc3_fdt_file) {
						_ATSC3_FDT_PARSER_DEBUG("atsc3_fdt_parse_from_xml_fdt_instance: adding file: %u, toi: %u, location: %s, length: %u, type: %s",
								i, atsc3_fdt_file->toi,
								atsc3_fdt_file->content_location,
								atsc3_fdt_file->content_length,
								atsc3_fdt_file->content_type);
						//add to atsc3_fdt_instance
						atsc3_fdt_instance_add_atsc3_fdt_file(atsc3_fdt_instance, atsc3_fdt_file);
					}
				}
			}
		}
	}

	return atsc3_fdt_instance;
}

atsc3_fdt_instance_t* atsc3_fdt_instance_parse_from_xml_document(xml_document_t* xml_document) {

	if(!xml_document) {
		return NULL;
	}

	xml_node_t* xml_document_root_node = xml_document_root(xml_document);
	xml_string_t* xml_document_root_node_name = xml_node_name(xml_document_root_node);
    
    //opening header should be xml
	dump_xml_string(xml_document_root_node_name);
    if(!xml_string_equals_ignore_case(xml_document_root_node_name, "xml")) {
        _ATSC3_FDT_PARSER_ERROR("atsc3_fdt_instance_parse_from_xml_document: opening tag missing xml preamble");
        return NULL;
    }

    //we should only be expecting either an EFDT or FDT-Instance node here
    atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_efdt_instance_parse_from_xml_node(xml_document_root_node);

	return atsc3_fdt_instance;
}

atsc3_fdt_instance_t* atsc3_fdt_parse_from_xml_fdt_instance(atsc3_fdt_instance_t* atsc3_fdt_instance, xml_node_t* node) {
    _ATSC3_FDT_PARSER_DEBUG("atsc3_fdt_parse_from_xml_fdt_instance: enter");
    
    uint8_t* xml_attributes = xml_attributes_clone_node(node);
    _ATSC3_FDT_PARSER_DEBUG("attributes: %s", xml_attributes);
    kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
    
    char* matching_attribute = NULL;
    
    if((matching_attribute = kvp_collection_get(kvp_collection, "expires"))) {
        atsc3_fdt_instance->expires = atoi(matching_attribute);
        free(matching_attribute);
    }
    
    if((matching_attribute = kvp_collection_get(kvp_collection, "complete"))) {
        atsc3_fdt_instance->complete = strncmp(matching_attribute, "t", 1) == 0;
        free(matching_attribute);
    }
    
    if((matching_attribute = kvp_collection_get(kvp_collection,  "content-type"))) {
        atsc3_fdt_instance->content_type = matching_attribute;
    }

    if((matching_attribute = kvp_collection_get(kvp_collection,  "content-encoding"))) {
        atsc3_fdt_instance->content_encoding = matching_attribute;
    }
    
    //TODO: remainder of elements are FEC related

	/*atsc-fdt/1.0 attributes here*/
    //TODO: fix with proper namespace mapping...e.g. resolve against  xmlns:afdt="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ATSC-FDT/1.0/"

    if((matching_attribute = kvp_collection_get(kvp_collection,  "afdt:efdt_vesion"))) {
    	atsc3_fdt_instance->content_encoding = matching_attribute;
    }
    if((matching_attribute = kvp_collection_get(kvp_collection,  "afdt:maxExpiresDelta"))) {
    	atsc3_fdt_instance->max_expires_delta = atoi(matching_attribute);
    }
    if((matching_attribute = kvp_collection_get(kvp_collection,  "afdt:maxTransportSize"))) {
    	atsc3_fdt_instance->max_transport_size = atoi(matching_attribute);
    }
    if((matching_attribute = kvp_collection_get(kvp_collection,  "afdt:fileTemplate"))) {
    	atsc3_fdt_instance->file_template = matching_attribute;
    }
    if((matching_attribute = kvp_collection_get(kvp_collection,  "afdt:appContextIdList"))) {
    	atsc3_fdt_instance->app_context_id_list = matching_attribute;
    }
    if((matching_attribute = kvp_collection_get(kvp_collection,  "afdt:filterCodes"))) {
    	atsc3_fdt_instance->filter_codes = matching_attribute;
    }




    if((matching_attribute = kvp_collection_get(kvp_collection,  "content-encoding"))) {
          atsc3_fdt_instance->content_encoding = matching_attribute;
      }  if((matching_attribute = kvp_collection_get(kvp_collection,  "content-encoding"))) {
          atsc3_fdt_instance->content_encoding = matching_attribute;
      }  if((matching_attribute = kvp_collection_get(kvp_collection,  "content-encoding"))) {
          atsc3_fdt_instance->content_encoding = matching_attribute;
      }  if((matching_attribute = kvp_collection_get(kvp_collection,  "content-encoding"))) {
          atsc3_fdt_instance->content_encoding = matching_attribute;
      }



    free(xml_attributes);    
    return atsc3_fdt_instance;
}

/**
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
 
 **/
atsc3_fdt_file_t* atsc3_fdt_file_parse_from_xml_fdt_instance(xml_node_t* node) {
    _ATSC3_FDT_PARSER_DEBUG("atsc3_fdt_file_parse_from_xml_fdt_instance: enter");
    atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_file_new();
    
    uint8_t* xml_attributes = xml_attributes_clone_node(node);
    _ATSC3_FDT_PARSER_DEBUG("attributes: %s", xml_attributes);
    kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
    
    char* matching_attribute = NULL;
    
    
    if((matching_attribute = kvp_collection_get(kvp_collection, "Content-Location"))) {
        atsc3_fdt_file->content_location = matching_attribute;
    }
    
    if((matching_attribute = kvp_collection_get(kvp_collection, "toi"))) {
        atsc3_fdt_file->toi = atoi(matching_attribute);
        free(matching_attribute);
    }
    
    if((matching_attribute = kvp_collection_get(kvp_collection, "Content-Length"))) {
        atsc3_fdt_file->content_length = atoi(matching_attribute);
        free(matching_attribute);
    }
    
    if((matching_attribute = kvp_collection_get(kvp_collection, "Transfer-Length"))) {
        atsc3_fdt_file->transfer_length = atoi(matching_attribute);
        free(matching_attribute);
    }
    
    if((matching_attribute = kvp_collection_get(kvp_collection,  "Content-Type"))) {
        atsc3_fdt_file->content_type = matching_attribute;
    }
    
    if((matching_attribute = kvp_collection_get(kvp_collection,  "Content-Encoding"))) {
        atsc3_fdt_file->content_encoding = matching_attribute;
    }
       
    if((matching_attribute = kvp_collection_get(kvp_collection,  "Content-MD5"))) {
        atsc3_fdt_file->content_md5 = matching_attribute;
    }
    //TODO: remainder of elements are FEC related
    
    free(xml_attributes);
    return atsc3_fdt_file;
}

void atsc3_fdt_instance_dump(atsc3_fdt_instance_t* atsc3_fdt_instance) {
	_ATSC3_FDT_PARSER_DEBUG("---atsc3_fdt_instance: %p, start---",atsc3_fdt_instance);
	_ATSC3_FDT_PARSER_DEBUG("   expires: %u, file count is: %u", atsc3_fdt_instance->expires, atsc3_fdt_instance->atsc3_fdt_file_v.count);
	for(int i=0; i < atsc3_fdt_instance->atsc3_fdt_file_v.count; i++) {
		atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[i];
		_ATSC3_FDT_PARSER_DEBUG("     atsc3_fdt_file[%u]: toi: %u, location: %s, length: %u, type: %s",i, atsc3_fdt_file->toi, atsc3_fdt_file->content_location, atsc3_fdt_file->content_length, atsc3_fdt_file->content_type);

	}
	_ATSC3_FDT_PARSER_DEBUG("---atsc3_fdt_instance: %p, end---",atsc3_fdt_instance);

}
