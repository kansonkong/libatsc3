/*
 * atsc3_fdt_parser.h
 *
 *  Created on: Mar 17, 2019
 *      Author: jjustman
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include <xlocale.h>

#ifndef ATSC3_FDT_PARSER_H_
#define ATSC3_FDT_PARSER_H_


#include "xml.h"
#include "atsc3_fdt.h"
#include "atsc3_logging_externs.h"

atsc3_fdt_instance_t* 	atsc3_fdt_instance_parse_from_xml_document(xml_document_t* xml_document);

//internal builders
atsc3_fdt_instance_t* 	atsc3_efdt_instance_parse_from_xml_node(xml_node_t* xml_efdt_node);

atsc3_fdt_instance_t* 	atsc3_fdt_parse_from_xml_fdt_instance(atsc3_fdt_instance_t* atsc3_fdt_instance, xml_node_t* node);
atsc3_fdt_file_t* 		atsc3_fdt_file_parse_from_xml_fdt_instance(xml_node_t* node);

void atsc3_fdt_instance_dump(atsc3_fdt_instance_t* atsc3_fdt_instance);

void atsc3_fdt_instance_free(atsc3_fdt_instance_t** atsc3_fdt_instance_p);

#define _ATSC3_FDT_PARSER_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_PARSER_WARN(...)    printf("%s:%d:WARN :",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_PARSER_INFO(...)    printf("%s:%d:INFO :",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_FDT_PARSER_DEBUG(...)   if(_FDT_PARSER_DEBUG_ENABLED) { printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__); }


#endif /* ATSC3_FDT_PARSER_H_ */
