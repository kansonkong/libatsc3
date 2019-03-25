/*
 * atsc3_mime_multipart_related_parser.c
 *
 *  Created on: Mar 25, 2019
 *      Author: jjustman
 */

#include <string.h>

#include "atsc3_mime_multipart_related_parser.h"

int _MIME_PARSER_INFO_ENABLED = 1;
int _MIME_PARSER_DEBUG_ENABLED = 1;
int _MIME_PARSER_TRACE_ENABLED = 1;

/**
 * destructive trim
 *
//    if (totrim > 0) {
//        size_t len = strlen(str);
//        if (totrim == len) {
//            str[0] = '\0';
//        }
//        else {
//            memmove(str, str + totrim, len + 1 - totrim);
//        }
//    }
 */
static char *_ltrim(char *str)
{
    size_t totrim;
    char* seps = "\t\n\v\f\r ";

    totrim = strspn(str, seps);
    str += totrim;
    return str;
}

/**
 *
Example payload:

Content-Type: multipart/related;
  type="application/mbms-envelope+xml";
  boundary="--boundary_at_1550614650679"


----boundary_at_1550614650679
Content-Type: application/mbms-envelope+xml
Content-Location: envelope.xml

<?xml version="1.0" encoding="UTF-8"?>
<metadataEnvelope xmlns="urn:3gpp:metadata:2005:MBMS:envelope">
    <item contentType="application/route-usd+xml" metadataURI="usbd.xml" version="1"/>
    <item contentType="application/route-s-tsid+xml" metadataURI="stsid.xml" version="1"/>
    <item contentType="application/dash+xml" metadataURI="mpd.xml" version="82"/>
    <item contentType="application/atsc-held+xml" metadataURI="held.xml" version="1"/>
</metadataEnvelope>
----boundary_at_1550614650679
Content-Type: application/route-usd+xml
Content-Location: usbd.xml

 *
 *
 */
#define ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_TYPE "Content-Type:"
#define ATSC3_MIME_MULTIPART_RELATED_HEADER_MULTIPART_RELATED "multipart/related"

atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_parser(FILE* fp) {
	if(!fp) {
		__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: fp is null!");
		return NULL;
	}

	atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance = calloc(1, sizeof(atsc3_mime_multipart_related_instance_t));

	char* line_buffer = calloc(ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, sizeof(char));
	char* multipart_buffer = calloc(ATSC3_MIME_MULTIPART_RELATED_PAYLOAD_BUFFER, sizeof(char));

	int ret;
	int token_len;
	bool has_completed_header = false;

	//parse out Content-Type: multipart/related;
	fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);
	token_len = __MIN(strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_TYPE), strlen(line_buffer));
	ret = strncasecmp(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_TYPE, line_buffer, token_len);

	if(ret) {
		__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: first line isn't Content-Type, ret: %u val:\n%s", ret, line_buffer);
		goto error;
	}
	line_buffer += token_len;
	line_buffer = _ltrim(line_buffer);
	token_len = __MIN(strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_MULTIPART_RELATED), strlen(line_buffer));
	ret = strncasecmp(ATSC3_MIME_MULTIPART_RELATED_HEADER_MULTIPART_RELATED, line_buffer, token_len);

	if(ret) {
		__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: first line isn't multipart/related, ret: %u val:\n%s", ret, line_buffer);
		goto error;
	}

	//find our type block
	//atsc3_mime_multipart_related_parser.c:100:INFO:atsc3_mime_multipart_related_parser: parsing line:   type="application/mbms-envelope+xml";


	while(!feof(fp) && !has_completed_header) {
		fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);

		char* trim_line_buffer = _ltrim(line_buffer);
		if(trim_line_buffer == line_buffer) {
			__MIME_PARSER_INFO("atsc3_mime_multipart_related_parser: header doesn't have space");

		}
		__MIME_PARSER_INFO("atsc3_mime_multipart_related_parser: parsing line: %s", line_buffer);

		if(!strncmp("type=", trim_line_buffer, 5)) {
			trim_line_buffer += 5;
			bool in_quotes = false;
			if(*trim_line_buffer == '"' || *trim_line_buffer == '\'') {
				trim_line_buffer++;
				in_quotes = true;
			}

			//extract out to first semicolon
			char* semicolon_pos = strnstr(trim_line_buffer, ";", strlen(trim_line_buffer));
			if(in_quotes) {
				semicolon_pos--;
			}

			if(!semicolon_pos) {
				__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: missing closing semicolon on type");
				goto error;
			}

			int len = semicolon_pos - trim_line_buffer;
			if(!len) {
				__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: missing type length");
				goto error;
			}

			if(trim_line_buffer < semicolon_pos && len) {
				atsc3_mime_multipart_related_instance->type = calloc(len, sizeof(char));
				strncpy(atsc3_mime_multipart_related_instance->type, trim_line_buffer, len);
				__MIME_PARSER_INFO("atsc3_mime_multipart_related_parser: type is: %s", atsc3_mime_multipart_related_instance->type);
			}

		}

		//strtok()
	}

	//get each payload
	while(!feof(fp) && !has_completed_header) {
		fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);

	}

	return atsc3_mime_multipart_related_instance;


error:
	if(atsc3_mime_multipart_related_instance) {
		free(atsc3_mime_multipart_related_instance);
		atsc3_mime_multipart_related_instance = NULL;
	}
	return NULL;
}
