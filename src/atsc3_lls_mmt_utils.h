/*
 * atsc3_lls_mmt_tools.h
 *
 *  Created on: Mar 2, 2019
 *      Author: jjustman
 */


#ifndef ATSC3_LLS_MMT_TOOLS_H_
#define ATSC3_LLS_MMT_TOOLS_H_

#include <assert.h>
#include <stdbool.h>

#include "atsc3_lls.h"
#include "atsc3_lls_types.h"
#include "atsc3_logging_externs.h"

#include "atsc3_mmt_context_mfu_depacketizer.h"

#if defined (__cplusplus)
extern "C" {
#endif

lls_sls_mmt_monitor_t* lls_sls_mmt_monitor_create(void);
void lls_slt_mmt_session_and_monitor_mark_all_atsc3_lls_slt_service_as_transient_stale(lls_slt_monitor_t* lls_slt_monitor);
void lls_slt_mmt_session_and_monitor_remove_all_atsc3_lls_slt_service_with_matching_transient_stale(lls_slt_monitor_t* lls_slt_monitor);


lls_sls_mmt_session_t* lls_slt_mmt_session_create(atsc3_lls_slt_service_t* atsc3_lls_slt_service);
lls_sls_mmt_session_t* lls_slt_mmt_session_find_or_create(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service);

lls_sls_mmt_session_t* lls_slt_mmt_session_find(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service);

lls_sls_mmt_session_t* lls_slt_mmt_session_find_from_udp_packet(lls_slt_monitor_t* lls_slt_monitor, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port);
lls_sls_mmt_session_t* lls_slt_mmt_session_find_from_service_id(lls_slt_monitor_t* lls_slt_monitor, uint16_t service_id);

lls_sls_mmt_monitor_t* lls_sls_mmt_monitor_find_from_service_id(lls_slt_monitor_t* lls_slt_monitor, uint16_t service_id);

void lls_slt_mmt_session_remove(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service);


lls_sls_mmt_monitor_t* lls_monitor_sls_mmt_session_create(atsc3_lls_slt_service_t* atsc3_lls_slt_service);

void lls_sls_mmt_session_free(lls_sls_mmt_session_t** lls_session_ptr);


#define _ATSC3_LLS_MMT_UTILS_ERROR(...)  __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_LLS_MMT_UTILS_WARN(...)   __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_LLS_MMT_UTILS_INFO(...)   if(_LLS_MMT_UTILS_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);  }
#define _ATSC3_LLS_MMT_UTILS_DEBUG(...)  if(_LLS_MMT_UTILS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_LLS_MMT_UTILS_TRACE(...)  if(_LLS_MMT_UTILS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#if defined (__cplusplus)
}
#endif


#endif /* ATSC3_LLS_MMT_TOOLS_H_ */
