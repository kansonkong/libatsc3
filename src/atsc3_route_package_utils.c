/*
 * atsc3_route_package_utils.c
 *
 *  Created on: Jul 7, 2020
 *      Author: jjustman
 *
 *
 *     TODO: handle package path management for appContextIdList extraction
 */


#include "atsc3_route_package_utils.h"

int _ROUTE_PACKAGE_UTILS_DEBUG_ENABLED=1;
int _ROUTE_PACKAGE_UTILS_TRACE_ENABLED=1;

#define __ROUTE_PACKAGE_PATH_TODO_FIXME__ "package_test"
int atsc3_route_package_extract_unsigned_payload(const char* filename) {
	int ret = 0;
	FILE *fp = fopen(filename, "r");

	if(fp) {
		//validate our struct
		atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance = atsc3_mime_multipart_related_parser(fp);
		if(atsc3_mime_multipart_related_instance) {
			atsc3_mime_multipart_related_instance_dump(atsc3_mime_multipart_related_instance);

			mkdir(__ROUTE_PACKAGE_PATH_TODO_FIXME__, 0777);

			for(int i=0; i < atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count; i++) {
				atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i];

				//hack for directory creation:
				//jjustman-2020-07-07: TODO: sanatize this logic

				char* original_filename = atsc3_mime_multipart_related_payload->content_location;
				char sandboxed_filename[1025] = {0};
				snprintf(sandboxed_filename, 1024, "%s/%s", __ROUTE_PACKAGE_PATH_TODO_FIXME__, original_filename);


				char* last_slash_position = NULL;
				for(int i=0; i < strlen(sandboxed_filename); i++) {
					if(sandboxed_filename[i] == '/') {
						last_slash_position = &sandboxed_filename[i];
					}
				}

				if(last_slash_position) {
					//hack - x2
					*last_slash_position = '\0';
					__ROUTE_PACKAGE_UTILS_INFO("making directory: %s", sandboxed_filename);
					mkpath(sandboxed_filename, 0777);
					*last_slash_position = '/';
				}

				__ROUTE_PACKAGE_UTILS_INFO("writing payload to file: %s, size: %d", sandboxed_filename, atsc3_mime_multipart_related_payload->payload->p_size);

				FILE* fp_payload = fopen(sandboxed_filename, "w");
				if(fp_payload) {

					fwrite(atsc3_mime_multipart_related_payload->payload->p_buffer, atsc3_mime_multipart_related_payload->payload->p_size, 1, fp_payload);
					fclose(fp_payload);
					fp_payload = NULL;
				} else {
					__ROUTE_PACKAGE_UTILS_ERROR("unable to open payload file: %s", sandboxed_filename);
				}
			}
		} else {
			__ROUTE_PACKAGE_UTILS_ERROR("atsc3_mime_multipart_related_instance is null!");
			ret = -1;
		}
	}

	return ret;
}
