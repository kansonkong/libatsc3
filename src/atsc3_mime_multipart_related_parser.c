/*
 * atsc3_mime_multipart_related_parser.c
 *
 *  Created on: Mar 25, 2019
 *      Author: jjustman
 */

#include <string.h>
#include "strnstr.h"
#include "atsc3_mime_multipart_related_parser.h"

int _MIME_PARSER_INFO_ENABLED  = 0;
int _MIME_PARSER_DEBUG_ENABLED = 0;
int _MIME_PARSER_TRACE_ENABLED = 0;


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
#define ATSC3_MIME_MULTIPART_RELATED_HEADER_MULTIPART_RELATED "multipart/related;"

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
	int line_count = 0;
	bool has_read_content_type = false;
	bool has_completed_header = false;

	//begin parsing header, starting with
	//Content-Type: Multipart/Related; boundary=boundary-content;

	while(!feof(fp) && !has_completed_header) {
		fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);
		line_count++;

		if(!has_read_content_type) {
			token_len = __MIN(strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_TYPE), strlen(line_buffer));
			ret = strncasecmp(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_TYPE, line_buffer, token_len);

			if(ret) {
				__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: line: %u (should be first) isn't Content-Type, ret: %u val:\n%s", line_count, ret, line_buffer);
				goto error;
			}
			line_buffer += token_len;
			line_buffer = _ltrim(line_buffer);
			token_len = __MIN(strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_MULTIPART_RELATED), strlen(line_buffer));
			ret = strncasecmp(ATSC3_MIME_MULTIPART_RELATED_HEADER_MULTIPART_RELATED, line_buffer, token_len);

			if(ret) {
				__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: line: %u (should be first) isn't multipart/related, ret: %u val:\n%s", line_count, ret, line_buffer);
				goto error;
			}
			line_buffer += token_len;
			has_read_content_type = true;
			//continue reading any other markers here
		}

		char* trim_line_buffer = _ltrim(line_buffer);
		if(trim_line_buffer == line_buffer) {
			__MIME_PARSER_INFO("atsc3_mime_multipart_related_parser: line: %u, header doesn't have space", line_count);

		}
		__MIME_PARSER_INFO("atsc3_mime_multipart_related_parser: line: %u, parsing trimmed: |%s|, original line: %s", line_count, trim_line_buffer, line_buffer);

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
					__MIME_PARSER_INFO("atsc3_mime_multipart_related_parser: line: %u, missing closing semicolon on type, using len: %u", line_count, remaining_trim_line_buffer_len);
					semicolon_pos = trim_line_buffer + remaining_trim_line_buffer_len;
				} else {
					__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: line: %u, missing closing semicolon on type, no strlen remaining", line_count);
					goto error;
				}
			}

			if(in_quotes) {
				semicolon_pos--;
			}

			int len = semicolon_pos - trim_line_buffer;
			if(!len) {
				__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: line: %u, missing type length", line_count);
				goto error;
			}

			if(trim_line_buffer < semicolon_pos && len) {
				//don't forget null pad
				atsc3_mime_multipart_related_instance->type = calloc(len+1, sizeof(char));
				strncpy(atsc3_mime_multipart_related_instance->type, trim_line_buffer, len);
				__MIME_PARSER_INFO("atsc3_mime_multipart_related_parser: line: %u, type is: %s", line_count, atsc3_mime_multipart_related_instance->type);
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
					__MIME_PARSER_WARN("atsc3_mime_multipart_related_parser: line: %u, missing closing semicolon on boundary, using len: %u", line_count, remaining_trim_line_buffer_len);
					semicolon_pos = trim_line_buffer + remaining_trim_line_buffer_len;
				} else {
					__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: line: %u, missing closing semicolon on boundary, no strlen remaining", line_count);
					goto error;
				}
			}

			if(in_quotes) {
				semicolon_pos--;
			}

			int len = semicolon_pos - trim_line_buffer;
			if(!len) {
				__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: line: %u, missing boundary length", line_count);
				goto error;
			}

			if(trim_line_buffer < semicolon_pos && len) {
				//don't forget null pad

				atsc3_mime_multipart_related_instance->boundary = calloc(len+1, sizeof(char));
				strncpy(atsc3_mime_multipart_related_instance->boundary, trim_line_buffer, len);
				__MIME_PARSER_INFO("atsc3_mime_multipart_related_parser: line: %u, boundary is: %s", line_count, atsc3_mime_multipart_related_instance->boundary);
			}
		}

		if(atsc3_mime_multipart_related_instance->type && atsc3_mime_multipart_related_instance->boundary) {
			has_completed_header = true;
		}
	}

	if(!has_completed_header) {
		__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: line: %u, header is incomplete", line_count);
		goto error;
	}

	bool has_consumed_up_to_first_dashdash = false;

	//the first line after the header should be blank, keep going until we get -- indiciating the start of a block
	while(!has_consumed_up_to_first_dashdash) {
		fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);
		line_count++;
		line_buffer = __trim(line_buffer);

		if(strlen(line_buffer)>1) {
			if(line_buffer[0] == '-' && line_buffer[1] == '-') {
				has_consumed_up_to_first_dashdash = true;
			} else {
				__MIME_PARSER_WARN("atsc3_mime_multipart_related_parser: line: %u, garbage payload: %s", line_count, line_buffer);
				//try to continue parsing, otherwise goto error;
			}
		}
	}

	//first 2 chars should be --, this should never happen unless we hit the end of file?
	if(strncmp("--", line_buffer, 2)) {
		__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: line: %u, payload boundary open is not --, instead: %s", line_count, line_buffer);
		goto error;
	}

	line_buffer+=2;
	line_buffer = __trim(line_buffer);
	if(strncmp(atsc3_mime_multipart_related_instance->boundary, line_buffer, strlen(atsc3_mime_multipart_related_instance->boundary))) {
		__MIME_PARSER_ERROR("atsc3_mime_multipart_related_parser: line: %u, payload boundary open is not boundary |%s|, instead: |%s|", line_count, atsc3_mime_multipart_related_instance->boundary, line_buffer);
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

			//this should actually be block_t as its easer to work with...
		*/

		while(!feof(fp) && !payload_header_complete) {
			fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);
			line_count++;

			if(!strncmp(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_TYPE, line_buffer, strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_TYPE))) {
				line_buffer += strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_TYPE);
				line_buffer = __trim(line_buffer);
				atsc3_mime_multipart_related_payload->content_type = calloc(strlen(line_buffer)+1, sizeof(char));
				memcpy(atsc3_mime_multipart_related_payload->content_type, line_buffer, strlen(line_buffer));

			} else if(!strncmp(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_LOCATION, line_buffer, strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_LOCATION))) {
				line_buffer += strlen(ATSC3_MIME_MULTIPART_RELATED_HEADER_CONTENT_LOCATION);
				line_buffer = __trim(line_buffer);
				atsc3_mime_multipart_related_payload->content_location = calloc(strlen(line_buffer)+1, sizeof(char));
				memcpy(atsc3_mime_multipart_related_payload->content_location, line_buffer, strlen(line_buffer));
			} else {
				line_buffer = __trim(line_buffer);
				if(!strlen(line_buffer)) {
					payload_header_complete = true;
				} else {
					__MIME_PARSER_INFO("atsc3_mime_multipart_related_parser: line: %u, ignoring payload header entry: %s", line_count, line_buffer);
				}
			}
		}


		while(!feof(fp) && !payload_entry_complete) {
			fgets(line_buffer, ATSC3_MIME_MULTIPART_RELATED_LINE_BUFFER, fp);
			line_count++;
			//check to see if we should close out boundary

			//first 2 chars should be --
			if(!strncmp("--", line_buffer, 2)) {
				__MIME_PARSER_TRACE("atsc3_mime_multipart_related_parser: line: %u, checking start of boundary closed %s", line_count, line_buffer);

				line_buffer += 2;
				if(!strncmp(atsc3_mime_multipart_related_instance->boundary, line_buffer, strlen(atsc3_mime_multipart_related_instance->boundary))) {
					payload_entry_complete = true;

					//todo: refactor
					atsc3_mime_multipart_related_payload->payload = calloc(multipart_buffer_pos+1, sizeof(char));
					memcpy(atsc3_mime_multipart_related_payload->payload, multipart_buffer, multipart_buffer_pos);
					atsc3_mime_multipart_related_payload->payload_length = multipart_buffer_pos;
					atsc3_mime_multipart_related_instance_add_atsc3_mime_multipart_related_payload(atsc3_mime_multipart_related_instance, atsc3_mime_multipart_related_payload);
					__MIME_PARSER_TRACE("atsc3_mime_multipart_related_parser: line: %u, payload boundary pushing new entry: %s", line_count, atsc3_mime_multipart_related_payload->payload);
					//if remaining string length is --, then we are done..
				} else {
					line_buffer -= 2;
				}
			}

			if(!payload_entry_complete) {
				uint32_t line_buffer_len = strlen(line_buffer);

				__MIME_PARSER_TRACE("atsc3_mime_multipart_related_parser: line: %u, pushing to buffer at pos: %u, len: %u, line buffer: %s", line_count, multipart_buffer_pos, line_buffer_len, line_buffer);

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


atsc3_sls_metadata_fragments_t* atsc3_mbms_envelope_to_sls_metadata_fragments_parse_from_fdt_fp(FILE* atsc3_fdt_instance_fp) {

	atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = NULL;

	if(!atsc3_fdt_instance_fp) {
		_ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_ERROR("atsc3_fdt_instance_fp is null!");
		return NULL;
	}

	atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance = atsc3_mime_multipart_related_parser(atsc3_fdt_instance_fp);
	if(atsc3_mime_multipart_related_instance) {
		atsc3_mime_multipart_related_instance_dump(atsc3_mime_multipart_related_instance);

		//build out the atsc3_sls_metadata_fragment_types
		atsc3_sls_metadata_fragments = atsc3_sls_metadata_fragment_types_parse_from_mime_multipart_related_instance(atsc3_mime_multipart_related_instance);

	} else {
		_ATSC3_ROUTE_MBMS_ENVELOPE_PARSER_ERROR("atsc3_mime_multipart_related_instance is null!");
		return NULL;
	}

	return atsc3_sls_metadata_fragments;
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
