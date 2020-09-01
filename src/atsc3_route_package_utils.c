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

int _ROUTE_PACKAGE_UTILS_DEBUG_ENABLED=0;
int _ROUTE_PACKAGE_UTILS_TRACE_ENABLED=0;


ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_NO_CCTOR(atsc3_route_package_extracted_envelope_metadata_and_payload, atsc3_mime_multipart_related_payload);
ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_route_package_extracted_envelope_metadata_and_payload);

void atsc3_route_package_extracted_envelope_metadata_and_payload_free(atsc3_route_package_extracted_envelope_metadata_and_payload_t** atsc3_route_package_extracted_envelope_metadata_and_payload_p) {
		if(atsc3_route_package_extracted_envelope_metadata_and_payload_p) {
			atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload = *atsc3_route_package_extracted_envelope_metadata_and_payload_p;
			if(atsc3_route_package_extracted_envelope_metadata_and_payload) {
				freeclean((void**)&atsc3_route_package_extracted_envelope_metadata_and_payload->package_name);
				freeclean((void**)&atsc3_route_package_extracted_envelope_metadata_and_payload->app_context_id_list);
				freeclean((void**)&atsc3_route_package_extracted_envelope_metadata_and_payload->filter_codes);
				freeclean((void**)&atsc3_route_package_extracted_envelope_metadata_and_payload->filter_codes);
				freeclean((void**)&atsc3_route_package_extracted_envelope_metadata_and_payload->package_extract_path);
				block_Destroy(&atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml);
				atsc3_mbms_metadata_envelope_free(&atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope);

				//atsc3_route_package_extracted_envelope_metadata_and_payload_free_atsc3_mime_multipart_related_payload(atsc3_route_package_extracted_envelope_metadata_and_payload);

				atsc3_mime_multipart_related_instance_free(&atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_instance);

				atsc3_route_package_extracted_envelope_metadata_and_payload_dealloc_atsc3_mime_multipart_related_payload(atsc3_route_package_extracted_envelope_metadata_and_payload);

				freesafe((void*)atsc3_route_package_extracted_envelope_metadata_and_payload);
				atsc3_route_package_extracted_envelope_metadata_and_payload = NULL;
			}
			*atsc3_route_package_extracted_envelope_metadata_and_payload_p = NULL;
		}
	}

//
//void atsc3_route_package_extract_payload_multipart_item_free(atsc3_route_package_extract_payload_multipart_item_t** atsc3_route_package_extract_payload_multipart_item_p) {
//    if(atsc3_route_package_extract_payload_multipart_item_p) {
//    	atsc3_route_package_extract_payload_multipart_item_t* atsc3_route_package_extract_payload_multipart_item = *atsc3_route_package_extract_payload_multipart_item_p;
//        if(atsc3_route_package_extract_payload_multipart_item) {
//            freeclean((void**)&atsc3_route_package_extract_payload_multipart_item->content_location);
//            atsc3_route_package_extract_payload_multipart_item = NULL;
//        }
//        *atsc3_route_package_extract_payload_multipart_item_p = NULL;
//    }
//}

/*
 *
 * NOTE: caller is responsable for free()ing a non-null returned value
 */
#define __ATSC3_ROUTE_PACKAGE_DEFAULT_PACKAGE_NAME__ "undefined"
char* atsc3_route_package_generate_path_from_appContextIdList(atsc3_fdt_file_t* atsc3_fdt_file) {

	SHA256_CTX ctx;
	BYTE buf[SHA256_BLOCK_SIZE];
	sha256_init(&ctx);

	bool sha256_set_from_fdt = false;

	char* package_extract_path = calloc(1 + SHA256_BLOCK_SIZE*2, sizeof(char)); //sha256 -> 32 bytes + 1

	if(atsc3_fdt_file) {
		if(atsc3_fdt_file->app_context_id_list) {
			sha256_update(&ctx, (const BYTE *)atsc3_fdt_file->app_context_id_list, strlen(atsc3_fdt_file->app_context_id_list));
			sha256_final(&ctx, (BYTE *)buf);
			sha256_set_from_fdt = true;
		} else if(atsc3_fdt_file->content_location) {
			sha256_update(&ctx, (const BYTE *)atsc3_fdt_file->content_location, strlen(atsc3_fdt_file->content_location));
			sha256_final(&ctx, (BYTE *)buf);
			sha256_set_from_fdt = true;
		}
	}

	if(!sha256_set_from_fdt) {
		sha256_update(&ctx, (const BYTE *)__ATSC3_ROUTE_PACKAGE_DEFAULT_PACKAGE_NAME__, strlen(__ATSC3_ROUTE_PACKAGE_DEFAULT_PACKAGE_NAME__));
		sha256_final(&ctx, (BYTE *)buf);
	}

	for(int i=0; i < SHA256_BLOCK_SIZE; i++) {
		sprintf(&package_extract_path[i*2], "%02x", buf[i]);
	}

	return package_extract_path;
}

void atsc3_route_package_extracted_envelope_metadata_and_payload_set_alc_tsi_toi_from_alc_packet(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload, atsc3_alc_packet_t* alc_packet) {
	atsc3_route_package_extracted_envelope_metadata_and_payload->tsi = alc_packet->def_lct_hdr->tsi;
	atsc3_route_package_extracted_envelope_metadata_and_payload->toi = alc_packet->def_lct_hdr->toi;
}

void atsc3_route_package_extracted_envelope_metadata_and_payload_set_fdt_attributes(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload, atsc3_fdt_file_t* atsc3_fdt_file) {
	if(atsc3_fdt_file) {
		if(atsc3_fdt_file->content_location) {
			atsc3_route_package_extracted_envelope_metadata_and_payload->package_name = strdup(atsc3_fdt_file->content_location);
		}
		if(atsc3_fdt_file->app_context_id_list) {
			atsc3_route_package_extracted_envelope_metadata_and_payload->app_context_id_list = strdup(atsc3_fdt_file->app_context_id_list);
		}
		if(atsc3_fdt_file->filter_codes) {
			atsc3_route_package_extracted_envelope_metadata_and_payload->filter_codes = strdup(atsc3_fdt_file->filter_codes);
		}
	}

}
atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extract_unsigned_payload(const char* filename, const char* package_extract_path) {
	int ret = 0;
	if(!package_extract_path) {
		__ROUTE_PACKAGE_UTILS_ERROR("package: %s, package_extract_path is NULL!", filename);
		return NULL;
	}

	atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload = atsc3_route_package_extracted_envelope_metadata_and_payload_new();

	FILE *fp = fopen(filename, "r");

	if(fp) {
		//validate our struct
		atsc3_mime_multipart_related_instance_t* atsc3_mime_multipart_related_instance = atsc3_mime_multipart_related_parser(fp);

		if(atsc3_mime_multipart_related_instance) {
			//keep a reference for our atsc3_mime_multipart_related_instance in our atsc3_route_package_extracted_envelope_metadata_and_payload so we can free it properly
			atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_instance = atsc3_mime_multipart_related_instance;

			atsc3_mime_multipart_related_instance_dump(atsc3_mime_multipart_related_instance);

			mkdir(package_extract_path, 0777);
			atsc3_route_package_extracted_envelope_metadata_and_payload->package_extract_path = strdup(package_extract_path);

			for(int i=0; i < atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count; i++) {
				atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i];


				/*
				 * from atsc3_sls_metadata_fragment_types_parser.c
				 * ROUTE MBMS envelope fragment creation
				 * TODO: jjustman-2020-07-22 - determine if we should write the envelope.xml to disk or not,
				 *	since we will have the raw_xml for the http server virtual pathing and object metadata
				 *
				 * TODO: make sure that
				 * 	atsc3_route_package_extract_payload_metadata->atsc3_mbms_metadata_envelope_raw_xml and
				 * 	atsc3_route_package_extract_payload_metadata->atsc3_mbms_metadata_envelope are empty before assigning them...
				 */
				if(atsc3_mime_multipart_related_payload->content_type && !strncmp(ATSC3_ROUTE_MBMS_ENVELOPE_TYPE, atsc3_mime_multipart_related_payload->content_type, strlen(ATSC3_ROUTE_MBMS_ENVELOPE_TYPE))) {
					atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml = block_Duplicate(atsc3_mime_multipart_related_payload->payload);
					atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope = atsc3_mbms_envelope_parse_from_payload((char*)atsc3_mime_multipart_related_payload->payload->p_buffer, atsc3_mime_multipart_related_payload->sanitizied_content_location);
					atsc3_mbms_metadata_envelope_dump(atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope);
					continue;
				}

				if(atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope && i > 0) {
					//envelope position should be at i-1
					if(i <= atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope->atsc3_mbms_metadata_item_v.count) {
						atsc3_mbms_metadata_item_t* atsc3_mbms_metadata_item = atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope->atsc3_mbms_metadata_item_v.data[i-1];

						__ROUTE_PACKAGE_UTILS_DEBUG("comparing atsc3_mbms_metadata_item->metadata_uri: %s, atsc3_mime_multipart_related_payload->unsafe_content_location: %s",
								atsc3_mbms_metadata_item->metadata_uri, atsc3_mime_multipart_related_payload->unsafe_content_location);

						if(!strcasecmp(atsc3_mbms_metadata_item->metadata_uri, atsc3_mime_multipart_related_payload->unsafe_content_location)) {
							atsc3_mime_multipart_related_payload->version = atsc3_mbms_metadata_item->version;
							if(atsc3_mbms_metadata_item->content_type) {
									atsc3_mime_multipart_related_payload->content_type = strdup(atsc3_mbms_metadata_item->content_type);
							}
							if(atsc3_mbms_metadata_item->valid_from_string) {
									atsc3_mime_multipart_related_payload->valid_from_string = strdup(atsc3_mbms_metadata_item->valid_from_string);
							}
							if(atsc3_mbms_metadata_item->valid_until_string) {
									atsc3_mime_multipart_related_payload->valid_until_string = strdup(atsc3_mbms_metadata_item->valid_until_string);
							}
							if(atsc3_mbms_metadata_item->next_url_string) {
									atsc3_mime_multipart_related_payload->next_url_string = strdup(atsc3_mbms_metadata_item->next_url_string);
							}
							if(atsc3_mbms_metadata_item->avail_at_string) {
									atsc3_mime_multipart_related_payload->avail_at_string = strdup(atsc3_mbms_metadata_item->avail_at_string);
							}
						}
					}
				}

				atsc3_route_package_extracted_envelope_metadata_and_payload_add_atsc3_mime_multipart_related_payload(atsc3_route_package_extracted_envelope_metadata_and_payload, atsc3_mime_multipart_related_payload);

				//hack for directory creation:
				//jjustman-2020-07-07: TODO: sanatize this logic

				char* original_filename = atsc3_mime_multipart_related_payload->sanitizied_content_location;
				char sandboxed_filename[1025] = {0};
				snprintf(sandboxed_filename, 1024, "%s/%s", package_extract_path, original_filename);


				char* last_slash_position = NULL;
				for(int i=0; i < strlen(sandboxed_filename); i++) {
					if(sandboxed_filename[i] == '/') {
						last_slash_position = &sandboxed_filename[i];
					}
				}

				if(last_slash_position) {
					//hack - x2
					*last_slash_position = '\0';
					__ROUTE_PACKAGE_UTILS_DEBUG("making directory: %s", sandboxed_filename);
					mkpath(sandboxed_filename, 0777);
					*last_slash_position = '/';
				}

				__ROUTE_PACKAGE_UTILS_DEBUG("writing payload to file: %s, size: %d", sandboxed_filename, atsc3_mime_multipart_related_payload->payload->p_size);

				FILE* fp_payload = fopen(sandboxed_filename, "w");
				if(fp_payload) {

					fwrite(atsc3_mime_multipart_related_payload->payload->p_buffer, atsc3_mime_multipart_related_payload->payload->p_size, 1, fp_payload);
					atsc3_mime_multipart_related_payload->extracted_size = atsc3_mime_multipart_related_payload->payload->p_size;
					block_Destroy(&atsc3_mime_multipart_related_payload->payload);
					fclose(fp_payload);
					fp_payload = NULL;
				} else {
					__ROUTE_PACKAGE_UTILS_ERROR("unable to open payload file: %s", sandboxed_filename);
				}
			}

		} else {
			__ROUTE_PACKAGE_UTILS_ERROR("atsc3_mime_multipart_related_instance is null! filename: %s", filename);
			ret = -1;
		}

		fclose(fp);
	}

	return atsc3_route_package_extracted_envelope_metadata_and_payload;
}


void atsc3_route_package_extract_payload_metadata_dump(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload) {

	__ROUTE_PACKAGE_UTILS_DEBUG("---");
	__ROUTE_PACKAGE_UTILS_DEBUG("tsi: %d, toi: %d", atsc3_route_package_extracted_envelope_metadata_and_payload->tsi, atsc3_route_package_extracted_envelope_metadata_and_payload->toi);

	if(atsc3_route_package_extracted_envelope_metadata_and_payload->package_name) {
		__ROUTE_PACKAGE_UTILS_DEBUG("package_name: %s", atsc3_route_package_extracted_envelope_metadata_and_payload->package_name);

	} else {
		__ROUTE_PACKAGE_UTILS_ERROR("package_name is NULL!");

	}


	if(atsc3_route_package_extracted_envelope_metadata_and_payload->app_context_id_list) {
		__ROUTE_PACKAGE_UTILS_DEBUG("app_context_id_list: %s", atsc3_route_package_extracted_envelope_metadata_and_payload->app_context_id_list);
	} else {
		__ROUTE_PACKAGE_UTILS_ERROR("app_context_id_list is NULL!");
	}


	if(atsc3_route_package_extracted_envelope_metadata_and_payload->package_extract_path) {
		__ROUTE_PACKAGE_UTILS_DEBUG("package extract path: %s", atsc3_route_package_extracted_envelope_metadata_and_payload->package_extract_path);
	} else {
		__ROUTE_PACKAGE_UTILS_ERROR("package extract path is NULL!");
	}




	__ROUTE_PACKAGE_UTILS_DEBUG("raw XML envelope:\n%s", atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml->p_buffer);
	__ROUTE_PACKAGE_UTILS_DEBUG("---");
	for(int i=0; i < atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.count; i++) {
		atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.data[i];
		__ROUTE_PACKAGE_UTILS_DEBUG("item: %d, path: %s, size: %d, content_type: %s, valid_from: %s, valid_until: %s, version: %u, next_url: %s, avail_at: %s",
				i,
				atsc3_mime_multipart_related_payload->sanitizied_content_location,
				atsc3_mime_multipart_related_payload->extracted_size,
				atsc3_mime_multipart_related_payload->content_type,
				atsc3_mime_multipart_related_payload->valid_from_string,
				atsc3_mime_multipart_related_payload->valid_until_string,
				atsc3_mime_multipart_related_payload->version,
				atsc3_mime_multipart_related_payload->next_url_string,
				atsc3_mime_multipart_related_payload->avail_at_string
				);

	}

}
