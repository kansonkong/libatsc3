/*
 * atsc3_lls_alc_tools.h
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */


#ifndef __LLS_SESSION_RELAX_SOURCE_IP_CHECK__
#define __LLS_SESSION_RELAX_SOURCE_IP_CHECK__ true
#endif

#ifndef ATSC3_LLS_ALC_TOOLS_H_
#define ATSC3_LLS_ALC_TOOLS_H_
#include <assert.h>
#include <stdbool.h>

#include "atsc3_lls.h"
#include "atsc3_lls_types.h"
#include "mad.h"
#include "alc_session.h"
#include "atsc3_lls_slt_parser.h"
#include "atsc3_lls_sls_parser.h"


#define _LLS_PRINTLN(...) printf(__VA_ARGS__);printf("\r\n")
#define _LLS_PRINTF(...)  printf(__VA_ARGS__);

#define __LLSU_TRACE(...)
//#define __LLSU_TRACE(...) _LLS_PRINTLN(__VA_ARGS__);


#if defined (__cplusplus)
extern "C" {
#endif


lls_sls_alc_monitor_t* lls_sls_alc_monitor_create(void);

lls_sls_alc_session_vector_t* lls_sls_alc_session_vector_create(void);


lls_sls_alc_session_t* lls_slt_alc_session_create(lls_service_t* lls_service);
lls_sls_alc_session_t* lls_slt_alc_session_find_or_create(lls_sls_alc_session_vector_t* lls_session, lls_service_t* lls_service);

lls_sls_alc_session_t* lls_slt_alc_session_find(lls_sls_alc_session_vector_t* lls_session, lls_service_t* lls_service);

lls_sls_alc_session_t* lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor_t* lls_slt_monitor, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port);
lls_sls_alc_session_t* lls_slt_alc_session_find_from_service_id(lls_slt_monitor_t* lls_slt_monitor, uint16_t service_id);

void lls_slt_alc_session_remove(lls_sls_alc_session_vector_t* lls_slt_alc_session, lls_service_t* lls_service);


lls_sls_alc_monitor_t* lls_monitor_sls_alc_session_create(lls_service_t* lls_service);


void lls_sls_alc_session_free(lls_sls_alc_session_t** lls_session_ptr);


#if defined (__cplusplus)
}
#endif


#endif /* ATSC3_LLS_ALC_TOOLS_H_ */
