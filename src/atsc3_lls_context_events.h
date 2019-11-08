/*
 * atsc3_lls_context_events.h
 *
 *  Created on: Oct 3, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_LLS_CONTEXT_EVENTS_H
#define ATSC3_LLS_CONTEXT_EVENTS_H

#include "atsc3_utils.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct lls_table lls_table_t; //forward declare until we can refactor lls_monitor out properly

typedef void (*atsc3_lls_on_sls_table_present_f)(lls_table_t* lls_table);
typedef void (*atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_f)(uint32_t tsi, uint32_t toi, char* s_tsid_content_location);

#ifdef __cplusplus
}
#endif
    

#endif /* ATSC3_LLS_CONTEXT_EVENTS_H */
