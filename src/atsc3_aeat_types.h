/*
 * atsc3_aeat_types.h
 *
 *  Created on: Sep 18, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_AEAT_TYPES_H_
#define ATSC3_AEAT_TYPES_H_

#include <stdbool.h>
#include "atsc3_utils.h"
#include "xml.h"
#include "atsc3_vector_builder.h"
#include "atsc3_logging_externs.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef struct atsc3_aea_header_event_code {
	char* type; //@type
	char* value;
} atsc3_aea_header_event_code_t;

typedef struct atsc3_aea_header_event_desc {
	char* lang;	//@lang
	char* value;
} atsc3_aea_header_event_desc_t;

typedef struct atsc3_aea_header_location {
	char* type; //@type
	char* value;
} atsc3_aea_header_location_t;

typedef struct atsc3_aea_header {
	char*		effective_s;
	struct tm 	effective;

	char*		expires_s;
	struct tm 	expires;

	atsc3_aea_header_event_code_t* event_code;
	atsc3_aea_header_event_desc_t* event_desc;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_aea_header_location);

} atsc3_aea_header_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_aea_header, atsc3_aea_header_location);

typedef struct atsc3_aea_text {
	char* lang;	//@lang
	char* value;

} atsc3_aea_text_t;

typedef struct atsc3_aea_service_name {
	char* lang;	//@lang
	char* value;

} atsc3_aea_service_name_t;

typedef struct atsc3_aea_live_media {
	char* 		bsid; 		//@bsid
	uint16_t	service_id; //@serviceId;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_aea_service_name);

} atsc3_aea_live_media_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_aea_live_media, atsc3_aea_service_name);


typedef struct atsc3_aea_media {
	char*		lang; 			//@lang
	char*		media_desc;		//@mediaDesc
	char*		media_type;		//@mediaType	- A/331:2019 - default, no value, optional: EventDescAudio, AEAtextAudio, EventSymbol
	char*		url; 			//@url[required=true]
	char*		alternate_url;	//@alternateUrl
	char*		content_type;	//@contentType
	char*		content_length;	//@contentLength
	char*		media_assoc;	//@mediaAssoc

	char*		value;
} atsc3_aea_media_t;

typedef struct atsc3_aea_table {
	char*	aea_id;
	char*	issuer;
	char*	audience; 	//A/331:2019 - Mapped to "public", "restricted", "private", or (reserved)
	char*	aea_type;	//A/331:2019 - Enum of "alert", "update", "cancel", (reserved)

	char*	ref_aea_id; //optional

	uint8_t	priority;	//4=max, 3=high, 2=moderate, 1=low, 0=mmonitor
	bool	wakeup;

	atsc3_aea_header_t*	atsc3_aea_header;

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_aea_text);
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_aea_live_media);
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_aea_media);

} atsc3_aea_table_t;

/**
 *
 * AEAT table
 * 	<AEAT xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/AEAT/1.0/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/AEAT/1.0/ AEAT-1.0-20190122.xsd">
 *
 */
typedef struct atsc3_aeat_table {
    char*   aeat_xml_fragment_latest;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_aea_table);
} atsc3_aeat_table_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_aeat_table, atsc3_aea_table);

atsc3_aeat_table_t* atsc3_aeat_table_new();
void atsc3_aeat_table_free(atsc3_aeat_table_t** atsc3_aeat_table_p);


#if defined (__cplusplus)
}
#endif

#define __AEAT_TYPES_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __AEAT_TYPES_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __AEAT_TYPES_INFO(...)  if(_AEAT_TYPES_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __AEAT_TYPES_DEBUG(...) if(_AEAT_TYPES_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __AEAT_TYPES_TRACE(...) if(_AEAT_TYPES_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }

#endif /* ATSC3_AEAT_TYPES_H_ */
