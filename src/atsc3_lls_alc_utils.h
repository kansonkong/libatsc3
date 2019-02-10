/*
 * atsc3_lls_tools.h
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */
#include <assert.h>
#include <stdbool.h>

#include "atsc3_lls.h"

#include "mad.h"
#include "alc_session.h"


#ifndef __LLS_SESSION_RELAX_SOURCE_IP_CHECK__
#define __LLS_SESSION_RELAX_SOURCE_IP_CHECK__ true
#endif

#ifndef ATSC3_LLS_ALC_TOOLS_H_
#define ATSC3_LLS_ALC_TOOLS_H_

#define _LLS_PRINTLN(...) printf(__VA_ARGS__);printf("\n")
#define _LLS_PRINTF(...)  printf(__VA_ARGS__);

#define __LLSU_TRACE(...) _LLS_PRINTLN(__VA_ARGS__);

//alc - assume single session for now

typedef struct lls_alc_session {
	uint16_t service_id;

	bool sls_relax_source_ip_check;
	uint32_t sls_source_ip_address;

	uint32_t sls_destination_ip_address;
	uint16_t sls_destination_udp_port;

	alc_arguments_t* alc_arguments;
	alc_session_t* alc_session;

} lls_slt_alc_session_t;

typedef struct lls_session {
	lls_table_t* lls_table_slt;

	int lls_slt_alc_sessions_n;
	lls_slt_alc_session_t** lls_slt_alc_sessions;

} lls_session_t;

lls_session_t* lls_session_create();


lls_slt_alc_session_t* lls_slt_alc_session_create(lls_service_t* lls_service);
lls_slt_alc_session_t* lls_slt_alc_session_find_or_create(lls_session_t* lls_session, lls_service_t* lls_service);

lls_slt_alc_session_t* lls_slt_alc_session_find(lls_session_t* lls_session, lls_service_t* lls_service);
lls_slt_alc_session_t* lls_slt_alc_session_find_from_udp_packet(lls_session_t* lls_session, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port);

void lls_slt_alc_session_remove(lls_service_t* lls_service, lls_slt_alc_session_t* lls_slt_alc_session);

void lls_session_free(lls_session_t** lls_session_ptr);



#endif /* ATSC3_LLS_ALC_TOOLS_H_ */
