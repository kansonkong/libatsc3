/*
 * atsc3_route_sls_processor.h
 *
 *  Created on: Apr 5, 2019
 *      Author: jjustman
 *
 *      process our tsi: 0 and releant toi sls and mbms signaling
 */

#ifndef ATSC3_ROUTE_SLS_PROCESSOR_H_
#define ATSC3_ROUTE_SLS_PROCESSOR_H_

#include "atsc3_alc_rx.h"
#include "atsc3_lls_types.h"
#include "atsc3_alc_utils.h"
#include "atsc3_fdt_parser.h"
#include "atsc3_mbms_envelope_parser.h"



void atsc3_route_sls_process_from_alc_packet_and_file(alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor);


#define _ATSC3_ROUTE_SLS_PROCESSOR_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_SLS_PROCESSOR_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_SLS_PROCESSOR_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_ROUTE_SLS_PROCESSOR_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);



#endif /* ATSC3_ROUTE_SLS_PROCESSOR_H_ */
