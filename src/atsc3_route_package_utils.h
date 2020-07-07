/*
 * atsc3_route_package_utils.h
 *
 *  Created on: Jul 7, 2020
 *      Author: jjustman
 */



#include <stdio.h>
#include <string.h>

#ifndef ATSC3_ROUTE_PACKAGE_UTILS_H_
#define ATSC3_ROUTE_PACKAGE_UTILS_H_


#include "atsc3_logging_externs.h"
#include "atsc3_utils.h"
#include "atsc3_mime_multipart_related_parser.h"
#include "atsc3_sls_metadata_fragment_types_parser.h"


#if defined (__cplusplus)
extern "C" {
#endif

int atsc3_route_package_extract_unsigned_payload(const char* filename);




#define __ROUTE_PACKAGE_UTILS_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __ROUTE_PACKAGE_UTILS_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __ROUTE_PACKAGE_UTILS_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define __ROUTE_PACKAGE_UTILS_DEBUG(...)   if(_ROUTE_PACKAGE_UTILS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __ROUTE_PACKAGE_UTILS_TRACE(...)   if(_ROUTE_PACKAGE_UTILS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }



#if defined (__cplusplus)
}
#endif


#endif /* ATSC3_ROUTE_PACKAGE_UTILS_H_ */
