/*
 * atsc3_lls_sls_parser.h
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 *
 *     Service Announcement Standard A/332 [5].
7. SERVICE LAYER SIGNALING
For ROUTE/DASH, the SLS for each service describes characteristics of the service,
such as a list of its components and where to acquire them, and the receiver capabilities
required to make a meaningful presentation of the service, and the availability and
 associated rules regarding access to file repair services by receivers.

 In the ROUTE/DASH system, the SLS includes:
		User Service Bundle Description (USBD),
		the S-TSID,
		Associated Procedure Description (APD),
		the DASH Media Presentation Description (MPD),
		the HTML Entry pages Location Description (HELD) (see A/337 [7]), and
		Distribution Window Description (DWD) (see A/337 [7]).

The USBD and APD are based on the identically-named
(i.e. User Service Bundle Description and Associated Procedure Description)
service description metadata fragments as defined in MBMS [14], with extensions
that support ATSC 3.0 requirements.

Table 7.1 shows the elements and attributes of the ROUTE/DASH USBD that would
be used in practice for ATSC 3.0 service delivery.


For MMTP, the SLS for each service describes characteristics of the service,
such as a list of its components and where to acquire them, and the receiver
capabilities required to make a meaningful presentation of the service.

In the MMTP system, the SLS includes the
 	 USBD fragment,
 	 the MMT Package (MP) table,
 	 the HTML Entry pages Location Description (HELD) (see A/337 [7]), and the
 	 Distribution Window Description (DWD) (see A/337 [7]).

 	 For hybrid delivery, the MMTP-specific SLS can further include the MPD for
 	 broadband components.


Table 7.4 shows the elements and attributes of the MMTP USBD that would be used
in practice for ATSC 3.0 service delivery.
*/

#include "atsc3_lls_types.h"
#include "xml.h"

#ifndef ATSC3_LLS_SLS_UTILS_H_
#define ATSC3_LLS_SLS_UTILS_H_

extern int _LLS_SLT_UTILS_DEBUG_ENABLED;




//int lls_sls_table_sls_alc_session_process_update(lls_table_t* lls_table, lls_sls_alc_session_vector_t* lls_session);



#define _LLS_SLT_UTILS_PRINTLN(...) printf(__VA_ARGS__);printf("\n")
#define _LLS_SLT_UTILS_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_LLS_SLT_UTILS_PRINTLN(__VA_ARGS__);
#define _LLS_SLT_UTILS_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_LLS_SLT_UTILS_PRINTLN(__VA_ARGS__);
#define _LLS_SLT_UTILS_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_LLS_SLT_UTILS_PRINTLN(__VA_ARGS__);

#define _LLS_SLT_UTILS_DEBUG(...)   if(_LLS_SLT_UTILS_DEBUG_ENABLED) { printf("%s:%d:DEBUG:",__FILE__,__LINE__);_LLS_SLT_UTILS_PRINTLN(__VA_ARGS__); }

#endif /* ATSC3_LLS_SLS_UTILS_H_ */
