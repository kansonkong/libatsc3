/*
 * atsc3_lls_slt_utils.h
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */

#include "atsc3_lls.h"
#include "atsc3_lls_alc_utils.h"

#ifndef ATSC3_LLS_SLT_UTILS_H_
#define ATSC3_LLS_SLT_UTILS_H_

extern int _LLS_SLT_UTILS_DEBUG_ENABLED;

int lls_slt_table_process_update(lls_session_t* lls_session, lls_table_t* lls_table);



#define _LLS_SLT_UTILS_PRINTLN(...) printf(__VA_ARGS__);printf("\n")
#define _LLS_SLT_UTILS_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_LLS_SLT_UTILS_PRINTLN(__VA_ARGS__);
#define _LLS_SLT_UTILS_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_LLS_SLT_UTILS_PRINTLN(__VA_ARGS__);
#define _LLS_SLT_UTILS_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_LLS_SLT_UTILS_PRINTLN(__VA_ARGS__);

#define _LLS_SLT_UTILS_DEBUG(...)   if(_LLS_SLT_UTILS_DEBUG_ENABLED) { printf("%s:%d:DEBUG:",__FILE__,__LINE__);_LLS_SLT_UTILS_PRINTLN(__VA_ARGS__); }

#endif /* ATSC3_LLS_SLT_UTILS_H_ */
