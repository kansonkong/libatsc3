/*
 * atsc3_monitor_events_lls.h
 *
 *  Created on: Jul 7, 2020
 *      Author: jjustman
 *
 *  Purpose:
 *
 *  	monitor event dispatcher callback signatures for relevant LLS (low level signalling) emissions
 *
 */

#ifndef ATSC3_MONITOR_EVENTS_LLS_H
#define ATSC3_MONITOR_EVENTS_LLS_H

#include "atsc3_utils.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct lls_table lls_table_t; //forward declare until we can refactor lls_monitor out properly



//LLS_table_id = 0x01
typedef void (*atsc3_lls_on_sls_table_present_f)(lls_table_t* lls_table);

//LLS_table_id = 0x02
typedef void (*atsc3_lls_on_rrt_table_present_f)(lls_table_t* lls_table);

//LLS_table_id = 0x03
typedef void (*atsc3_lls_on_systemtime_table_present_f)(lls_table_t* lls_table);

//LLS_table_id = 0x04
typedef void (*atsc3_lls_on_aeat_table_present_f)(lls_table_t* lls_table);

//LLS_table_id = 0x05
typedef void (*atsc3_lls_on_onscreenmessagenotification_table_present_f)(lls_table_t* lls_table);

//LLS_table_id = 0x06
//A/360:2019 - See: 5.2.2.2 Certificate and OCSP Response LLS Table
typedef void (*atsc3_lls_on_certificationdata_table_present_f)(lls_table_t* lls_table);

//LLS_table_id = 0xFE
typedef void (*atsc3_lls_on_signedmultitable_table_present_f)(lls_table_t* lls_table);

//LLS_table_id = 0xFF
typedef void (*atsc3_lls_on_userdefined_table_present_f)(lls_table_t* lls_table);

#ifdef __cplusplus
}
#endif
    

#endif /* ATSC3_MONITOR_EVENTS_LLS_H */
