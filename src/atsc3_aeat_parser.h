/*
 * atsc3_aeat_parser.h
 *
 *  Created on: Sep 18, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_AEAT_PARSER_H_
#define ATSC3_AEAT_PARSER_H_


#include <stdbool.h>
#include "atsc3_utils.h"
#include "xml.h"
#include "atsc3_vector_builder.h"
#include "atsc3_logging_externs.h"
#include "atsc3_lls.h"

#if defined (__cplusplus)
extern "C" {
#endif

int atsc3_aeat_table_populate_from_xml(lls_table_t* lls_table, xml_node_t* xml_root);

#define __AEAT_PARSER_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __AEAT_PARSER_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __AEAT_PARSER_INFO(...)  if(_AEAT_PARSER_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __AEAT_PARSER_DEBUG(...) if(_AEAT_PARSER_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __AEAT_PARSER_TRACE(...) if(_AEAT_PARSER_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }

#if defined (__cplusplus)
}
#endif
#endif /* ATSC3_AEAT_PARSER_H_ */
