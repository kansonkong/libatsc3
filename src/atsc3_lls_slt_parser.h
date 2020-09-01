/*
 * atsc3_lls_slt_parser.h
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 *
 *      6.3 Service List Table (SLT)
The Service List Table (SLT) is one of the instance types of LLS information.
The function of the SLT is similar to that of the Program Association Table (PAT)
in MPEG-2 Systems [33], and the Fast Information Channel (FIC) found in ATSC A/153, Part 3 [44].

For a receiver first encountering the broadcast emission, this is the place to start.
It supports a rapid channel scan which allows a receiver to build a list of all the services
it can receive, with their channel name, channel number, etc., and it provides bootstrap information
that allows a receiver to discover the SLS for each service.

For ROUTE/DASH-delivered services,
the bootstrap information includes the source IP address, the destination IP address and the
destination port of the LCT channel that carries the ROUTE-specific SLS.

For MMTP/MPU-delivered services, the bootstrap information includes the destination IP
address and destination port of the MMTP session carrying the MMTP- specific SLS.
 *
 */

#include "atsc3_lls.h"
#include "atsc3_lls_slt_utils.h"
#include "atsc3_lls_mmt_utils.h"

#include "atsc3_lls_alc_utils.h"
#include "atsc3_lls_sls_parser.h"


#ifndef ATSC3_LLS_SLT_PARSER_H_
#define ATSC3_LLS_SLT_PARSER_H_


#if defined (__cplusplus)
extern "C" {
#endif


lls_slt_monitor_t* lls_slt_monitor_create(void);
void atsc3_lls_slt_monitor_free(lls_slt_monitor_t** lls_slt_monitor_p);


int lls_slt_table_check_process_update(lls_table_t* lls_table, lls_slt_monitor_t* lls_slt_monitor);
int lls_slt_table_perform_update(lls_table_t* lls_table, lls_slt_monitor_t* lls_slt_monitor);


//etst methods
char* lls_get_service_category_value(uint service_category);
char* lls_get_sls_protocol_value(uint protocol);


int lls_slt_table_build(lls_table_t* lls_table, xml_node_t *xml_root);

int SLT_BROADCAST_SVC_SIGNALING_build_table(atsc3_lls_slt_service_t* atsc3_lls_slt_service, xml_node_t *service_row_node, kvp_collection_t* kvp_collection);


#if defined (__cplusplus)
}
#endif

#define __LLS_SLT_PARSER_PRINTLN(...) 	 printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __LLS_SLT_PARSER_ERROR(...)  	 printf("%s:%d:ERROR:",__FILE__,__LINE__);__LLS_SLT_PARSER_PRINTLN(__VA_ARGS__);
#define __LLS_SLT_PARSER_WARN(...)   	 printf("%s:%d:WARN:",__FILE__,__LINE__);__LLS_SLT_PARSER_PRINTLN(__VA_ARGS__);
#define __LLS_SLT_PARSER_INFO(...)   	 if(_LLS_SLT_PARSER_INFO_ENABLED) 		{ printf("%s:%d:INFO:",__FILE__,__LINE__);__LLS_SLT_PARSER_PRINTLN(__VA_ARGS__); }
#define __LLS_SLT_PARSER_INFO_MMT(...)   if(_LLS_SLT_PARSER_INFO_MMT_ENABLED) 	{ printf("%s:%d:INFO:",__FILE__,__LINE__);__LLS_SLT_PARSER_PRINTLN(__VA_ARGS__); }
#define __LLS_SLT_PARSER_INFO_ROUTE(...) if(_LLS_SLT_PARSER_INFO_ROUTE_ENABLED) { printf("%s:%d:INFO:",__FILE__,__LINE__);__LLS_SLT_PARSER_PRINTLN(__VA_ARGS__); }

#define __LLS_SLT_PARSER_DEBUG(...)   	if(_LLS_SLT_PARSER_DEBUG_ENABLED) { printf("%s:%d:DEBUG:",__FILE__,__LINE__);__LLS_SLT_PARSER_PRINTLN(__VA_ARGS__); }
#define __LLS_SLT_PARSER_TRACE(...)  	 if(_LLS_SLT_PARSER_TRACE_ENABLED) { printf("%s:%d:TRACE:",__FILE__,__LINE__);__LLS_SLT_PARSER_PRINTLN(__VA_ARGS__); }

#endif /* ATSC3_LLS_SLT_PARSER_H_ */
