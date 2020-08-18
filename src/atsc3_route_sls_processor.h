/*
 * atsc3_route_sls_processor.h
 *
 *  Created on: Apr 5, 2019
 *      Author: jjustman
 *
 *      process our tsi: 0 and releant toi sls and mbms signaling
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifndef ATSC3_ROUTE_SLS_PROCESSOR_H_
#define ATSC3_ROUTE_SLS_PROCESSOR_H_

#include "atsc3_logging_externs.h"
#include "atsc3_alc_rx.h"
#include "atsc3_lls_types.h"
#include "atsc3_alc_utils.h"
#include "atsc3_fdt_parser.h"
#include "atsc3_mbms_envelope_parser.h"
#include "atsc3_mime_multipart_related_parser.h"
#include "atsc3_route_dash_utils.h"
#include "atsc3_pcre2_regex_utils.h"


void atsc3_route_sls_process_from_alc_packet_and_file(udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor);

int32_t atsc3_sls_write_mime_multipart_related_payload_to_file(atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload, lls_sls_alc_monitor_t* lls_sls_alc_monitor);
char* atsc3_sls_generate_filename_from_atsc3_mime_multipart_related_payload(atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload, lls_sls_alc_monitor_t* lls_sls_alc_monitor);

bool atsc3_route_sls_process_from_sls_metadata_fragments_patch_mpd_availability_start_time_and_start_number(atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments, lls_sls_alc_monitor_t* lls_sls_alc_monitor);

bool atsc3_route_sls_patch_mpd_availability_start_time_and_start_number(atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload, lls_sls_alc_monitor_t* lls_sls_alc_monitor);

#define _ATSC3_ROUTE_SLS_PROCESSOR_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_ROUTE_SLS_PROCESSOR_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_ROUTE_SLS_PROCESSOR_INFO(...)   if(_ROUTE_SLS_PROCESSOR_INFO_ENABLED) { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_ROUTE_SLS_PROCESSOR_DEBUG(...)  if(_ROUTE_SLS_PROCESSOR_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_ROUTE_SLS_PROCESSOR_TRACE(...)  if(_ROUTE_SLS_PROCESSOR_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }



#endif /* ATSC3_ROUTE_SLS_PROCESSOR_H_ */
