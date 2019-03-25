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
#define ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_LOCATION "Content-Location:"
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

		// type="application/mbms-envelope+xml";

		if(!strncmp("type=", trim_line_buffer, 5)) {
			trim_line_buffer += 5;
			bool in_quotes = false;
			if(*trim_line_buffer == '"' || *trim_line_buffer == '\'') {
				trim_line_buffer++;
				in_quotes = true;
			}

			//extract out to first semicolon
			char* semicolon_pos = strnstr(trim_line_buffer, ";", strlen(trim_line_buffer));

			if(!semicolon_pos) {
				trim_line_buffer = _rtrim(trim_line_buffer);
				int remaining_trim_line_buffer_len = strlen(trim_line_buffer);
				if(remaining_trim_line_buffer_len) {
					__MIME_PARSER_WARN("atsc3_mime_multipart_related_parser: missing closing semicolon on type, using len: %u", remaining_trim_line_buffer_len);
					semicolon_pos = trim_line_buffer + remaining_trim_line_buffer_len;
				} else {
					__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: missing closing semicolon on type, no strlen remaining");
					goto error;
				}
			}

			if(in_quotes) {
				semicolon_pos--;
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

		// boundary="--boundary_at_1550614650679"
		if(!strncmp("boundary=", trim_line_buffer, 9)) {
			trim_line_buffer += 9;
			bool in_quotes = false;
			if(*trim_line_buffer == '"' || *trim_line_buffer == '\'') {
				trim_line_buffer++;
				in_quotes = true;
			}

			//extract out to first semicolon
			char* semicolon_pos = strnstr(trim_line_buffer, ";", strlen(trim_line_buffer));

			if(!semicolon_pos) {
				trim_line_buffer = _rtrim(trim_line_buffer);

				int remaining_trim_line_buffer_len = strlen(trim_line_buffer);
				if(remaining_trim_line_buffer_len) {
					__MIME_PARSER_WARN("atsc3_mime_multipart_related_parser: missing closing semicolon on boundary, using len: %u", remaining_trim_line_buffer_len);
					semicolon_pos = trim_line_buffer + remaining_trim_line_buffer_len;
				} else {
					__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: missing closing semicolon on boundary, no strlen remaining");
					goto error;
				}
			}

			if(in_quotes) {
				semicolon_pos--;
			}

			int len = semicolon_pos - trim_line_buffer;
			if(!len) {
				__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: missing boundary length");
				goto error;
			}

			if(trim_line_buffer < semicolon_pos && len) {
				atsc3_mime_multipart_related_instance->boundary = calloc(len, sizeof(char));
				strncpy(atsc3_mime_multipart_related_instance->boundary, trim_line_buffer, len);
				__MIME_PARSER_INFO("atsc3_mime_multipart_related_parser: boundary is: %s", atsc3_mime_multipart_related_instance->boundary);
			}
		}

		if(atsc3_mime_multipart_related_instance->type && atsc3_mime_multipart_related_instance->boundary) {
			has_completed_header = true;
		}
	}

	if(!has_completed_header) {
		__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: header is incomplete");
		goto error;
	}

	//chomp open header
	fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);
	line_buffer = __trim(line_buffer);

	if(strlen(line_buffer)) {
		__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: payload line 1 is not \\r\\n, val: %s", line_buffer);
		goto error;
	}
	fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);
	line_buffer = __trim(line_buffer);

	if(strlen(line_buffer)) {
		__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: payload line 2 is not \\r\\n, val: %s", line_buffer);
		goto error;
	}

	//open block
	fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);
	//first 2 chars should be --
	if(strncmp("--", line_buffer, 2)) {
		__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: payload boundary open is not --, instead: %s", line_buffer);
		goto error;
	}
	line_buffer+=2;
	line_buffer = __trim(line_buffer);
	if(strncmp(atsc3_mime_multipart_related_instance->boundary, line_buffer, strlen(atsc3_mime_multipart_related_instance->boundary))) {
		__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: payload boundary open is not boundary |%s|, instead: |%s|", atsc3_mime_multipart_related_instance->boundary, line_buffer);
		goto error;
	}


	//get each payload
	while(!feof(fp)) {
		//for each payload entry
		bool payload_header_complete = false;
		bool payload_entry_complete = false;

		memset(multipart_buffer, 0, ATSC3_MIME_MULTIPART_RELATED_PAYLOAD_BUFFER);
		uint32_t multipart_buffer_pos = 0;

		atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = atsc3_mime_multipart_related_payload_new();

		/**
		 * try and parse out header attributes first, e.g.:
			Content-Type: application/atsc-held+xml
			Content-Location: held.xml
		*/

		while(!feof(fp) && !payload_header_complete) {
			fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);

			if(!strncmp(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_TYPE, line_buffer, strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_TYPE))) {
				line_buffer += strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_TYPE);
				line_buffer = __trim(line_buffer);
				atsc3_mime_multipart_related_payload->content_type = calloc(strlen(line_buffer), sizeof(char));
				memcpy(atsc3_mime_multipart_related_payload->content_type, line_buffer, strlen(line_buffer));

			} else if(!strncmp(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_LOCATION, line_buffer, strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_LOCATION))) {
				line_buffer += strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_LOCATION);
				line_buffer = __trim(line_buffer);
				atsc3_mime_multipart_related_payload->content_location = calloc(strlen(line_buffer), sizeof(char));
				memcpy(atsc3_mime_multipart_related_payload->content_location, line_buffer, strlen(line_buffer));
			} else {
				line_buffer = __trim(line_buffer);
				if(!strlen(line_buffer)) {
					payload_header_complete = true;
				} else {
					__MIME_PARSER_INFO("atsc3_mime_multipart_related_parser: ignoring payload header entry: %s", line_buffer);
				}
			}
		}


		while(!feof(fp) && !payload_entry_complete) {
			fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);
			//check to see if we should close out boundary

			//first 2 chars should be --
			if(!strncmp("--", line_buffer, 2)) {
				__MIME_PARSER_TRACE("atsc3_mime_multipart_related_parser: checking start of boundary closed %s", line_buffer);

				line_buffer += 2;
				if(!strncmp(atsc3_mime_multipart_related_instance->boundary, line_buffer, strlen(atsc3_mime_multipart_related_instance->boundary))) {
					payload_entry_complete = true;

					//todo: refactor
					atsc3_mime_multipart_related_payload->payload = calloc(multipart_buffer_pos, sizeof(char));
					memcpy(atsc3_mime_multipart_related_payload->payload, multipart_buffer, multipart_buffer_pos);
					atsc3_mime_multipart_related_payload->payload_length = multipart_buffer_pos;
					atsc3_mime_multipart_related_instance_add_atsc3_mime_multipart_related_payload(atsc3_mime_multipart_related_instance, atsc3_mime_multipart_related_payload);
					__MIME_PARSER_TRACE("atsc3_mime_multipart_related_parser: payload boundary pushing new entry: %s",  atsc3_mime_multipart_related_payload->payload);
					//if remaining string length is --, then we are done..
				} else {
					line_buffer -= 2;
				}
			}

			if(!payload_entry_complete) {
				uint32_t line_buffer_len = strlen(line_buffer);

				__MIME_PARSER_TRACE("atsc3_mime_multipart_related_parser: pushing to buffer at pos: %u, len: %u, line buffer: %s", multipart_buffer_pos, line_buffer_len, line_buffer);

				//push to buffer
				memcpy(&multipart_buffer[multipart_buffer_pos], line_buffer, line_buffer_len);
				multipart_buffer_pos += line_buffer_len;
			}
		}
	}

	return atsc3_mime_multipart_related_instance;


error:
	if(atsc3_mime_multipart_related_instance) {
		free(atsc3_mime_multipart_related_instance);
		atsc3_mime_multipart_related_instance = NULL;
	}
	return NULL;
}

void atsc3_mime_multipart_related_instance_dump(atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance) {
	__MIME_PARSER_DEBUG("------------------");
	__MIME_PARSER_DEBUG("Multipart dump is:");
	__MIME_PARSER_DEBUG("------------------");
	__MIME_PARSER_DEBUG("");
	__MIME_PARSER_DEBUG("count: %u", atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count);
	for(int i=0; i < atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count; i++) {
		__MIME_PARSER_DEBUG("type     : %s", atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i]->content_type);
		__MIME_PARSER_DEBUG("location : %s", atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i]->content_location);
		__MIME_PARSER_DEBUG("payload  :\n%s", atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i]->payload);
	}

}
