/*
 * atsc3_route_sls_processor.c
 *
 *  Created on: Apr 5, 2019
 *      Author: jjustman
 */


#include "atsc3_route_sls_processor.h"
#include "atsc3_lls_alc_utils.h"


void atsc3_route_sls_process_from_alc_packet_and_file(udp_flow_t* udp_flow, alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
    char* file_name = NULL;
    char* mbms_toi_filename = NULL;
    
    FILE *fp = NULL;
    FILE *fp_mbms = NULL;
    
    xml_document_t* fdt_xml = NULL;
    atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = NULL;
    
	//check if our toi == 0, if so, reprocess our sls fdt in preperation for an upcoming actual mbms emission
	_ATSC3_ROUTE_SLS_PROCESSOR_INFO("alc_packet tsi/toi:%u/%u", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);

	if(alc_packet->def_lct_hdr->toi == 0) {

	    file_name = alc_packet_dump_to_object_get_temporary_filename(udp_flow, alc_packet);

		fp = fopen(file_name, "r");
		if(!fp) {
			_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("alc_packet tsi/toi:0/0 filename: %s is null!", file_name);
			goto cleanup;
		}

		fdt_xml = xml_open_document(fp);
		if(fdt_xml) {
			atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_fdt_instance_parse_from_xml_document(fdt_xml);
			if(!atsc3_fdt_instance) {
				_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("alc_packet tsi/toi:0/0 instance is null!");
				goto cleanup;
			}
			atsc3_fdt_instance_dump(atsc3_fdt_instance);
			if(lls_sls_alc_monitor) {
                if(lls_sls_alc_monitor->atsc3_fdt_instance) {
                    atsc3_fdt_instance_free(&lls_sls_alc_monitor->atsc3_fdt_instance);
                }
				lls_sls_alc_monitor->atsc3_fdt_instance = atsc3_fdt_instance;
			}
		}
	} else {
		//keep a reference for our mbms toi

		if(lls_sls_alc_monitor && lls_sls_alc_monitor->atsc3_fdt_instance) {
			uint32_t* mbms_toi = atsc3_mbms_envelope_find_toi_from_fdt(lls_sls_alc_monitor->atsc3_fdt_instance);
			if(!mbms_toi) {
				_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("Unable to find MBMS TOI");
				goto cleanup;
			}

            mbms_toi_filename = alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow, 0, *mbms_toi);

			fp_mbms = fopen(mbms_toi_filename, "r");
			if(!fp_mbms) {
				_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("alc_packet tsi/toi:0/%u filename: %s is null!", *mbms_toi, mbms_toi_filename);
				goto cleanup;
			}

			atsc3_sls_metadata_fragments = atsc3_mbms_envelope_to_sls_metadata_fragments_parse_from_fdt_fp(fp_mbms);

			if(atsc3_sls_metadata_fragments) {
				if(atsc3_sls_metadata_fragments->atsc3_route_s_tsid) {
					//update our audio and video tsi and init
					lls_sls_alc_update_tsi_toi_from_route_s_tsid(lls_sls_alc_monitor, atsc3_sls_metadata_fragments->atsc3_route_s_tsid);
				}
                if(lls_sls_alc_monitor->atsc3_sls_metadata_fragments) {
                    //invoke any chained destructors as needed
                    atsc3_sls_metadata_fragments_free(&lls_sls_alc_monitor->atsc3_sls_metadata_fragments);
                }
				lls_sls_alc_monitor->atsc3_sls_metadata_fragments = atsc3_sls_metadata_fragments;
                
                /* https://github.com/google/shaka-player/issues/237
                 
                 For MPDs with type dynamic, it is important to look at the combo "availabilityStartTime + period@start" (AST + PST) and "startNumber"
                 and the current time.

                 startNumber refers to the segment that is available one segmentDuration after the period start
                 (at the period start, only the init segments are available),

                 For dynamic MPDs, you shall "never" start to play with startNumber, but the latest available segment is
                 LSN = floor( (now - (availabilityStartTime+PST))/segmentDuration + startNumber- 1).

                 It is also important to align the mediaTimeLine (based on baseMediaDecodeTime) so that it starts at 0 at the beginning of the period.
                 
                 content_type    char *    "application/dash+xml"    0x00000001064c8fe0
                 */
                //TODO: jjustman-2019-11-02: write out our multipart mbms payload to our route/svc_id, e.g. to get the mpd
                for(int i=0; i < lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count; i++) {
                    atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i];
                    //jjustman-2019-11-05 - patch MPD type="dynamic" with availabilityStartTime to NOW and startNumber to the most recent A/V flows for TOI delivery
                    if(strncmp(atsc3_mime_multipart_related_payload->content_type, ATSC3_ROUTE_MPD_TYPE, __MIN(strlen(atsc3_mime_multipart_related_payload->content_type), strlen(ATSC3_ROUTE_MPD_TYPE))) == 0) {
                        atsc3_route_sls_patch_mpd_availability_start_time_and_start_number(atsc3_mime_multipart_related_payload, lls_sls_alc_monitor);
                    }
                    
                    char mbms_filename[1025] = { 0 };
                    snprintf(mbms_filename, 1024, "route/%d", lls_sls_alc_monitor->atsc3_lls_slt_service->service_id);
                    mkdir(mbms_filename, 0777);
                    snprintf(mbms_filename, 1024, "route/%d/%s", lls_sls_alc_monitor->atsc3_lls_slt_service->service_id, atsc3_mime_multipart_related_payload->content_location);
                    FILE* fp = fopen(mbms_filename, "w");
                    if(fp) {
                        fwrite(atsc3_mime_multipart_related_payload->payload, atsc3_mime_multipart_related_payload->payload_length, 1, fp);
                        fclose(fp);
                    } else {
                        _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("sls mbms fragment dump, original content_location: %s, unable to write to local path: %s", atsc3_mime_multipart_related_payload->content_location, mbms_filename);

                    }
                }
			}
		} else {
			_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("No pending atsc3_fdt_instance to process in TSI:0");
            goto cleanup;

		}
	}
    
cleanup:
    if(file_name) {
        free(file_name);
        file_name = NULL;
    }
    
    if(mbms_toi_filename) {
        free(mbms_toi_filename);
        mbms_toi_filename = NULL;
    }
    
    if(fdt_xml) {
        xml_document_free(fdt_xml, false);
        fdt_xml = NULL;
    }
    
    if(fp) {
        fclose(fp);
        fp = NULL;
    }
    if(fp_mbms) {
        fclose(fp_mbms);
        fp_mbms = NULL;
    }
    

    //mbms_toi is a pointer to atsc3_fdt_intstance alloc, so don't try and free it
    
}

#define _ISO8601_DATE_TIME_LENGTH_ 20
#define _MPD_availability_start_time_VALUE_ "availabilitystarttime="

void atsc3_route_sls_patch_mpd_availability_start_time_and_start_number(atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
    char* temp_lower_mpd = calloc(strlen(atsc3_mime_multipart_related_payload->payload)+1, sizeof(char));
    for(int i=0; i < strlen(atsc3_mime_multipart_related_payload->payload); i++) {
        temp_lower_mpd[i] = tolower(atsc3_mime_multipart_related_payload->payload[i]);
    }
    
    if(strstr(temp_lower_mpd, "type=\"dynamic\"") != NULL) {
        //update our availabilityStartTime
        char* ast_char = strstr(temp_lower_mpd, _MPD_availability_start_time_VALUE_);
        if(ast_char) {
            time_t now;
            time(&now);

            int ast_char_pos_end = (ast_char + strlen(_MPD_availability_start_time_VALUE_)) - temp_lower_mpd;
            //replace 2019-10-09T19:03:50Z with now()...
            //        123456789012345678901
            //                 1         2
            char iso_now_timestamp[_ISO8601_DATE_TIME_LENGTH_ + 1] = { 0 };
            strftime(&iso_now_timestamp, _ISO8601_DATE_TIME_LENGTH_, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

            char* to_start_ptr = atsc3_mime_multipart_related_payload->payload + ast_char_pos_end + 1;
            _ATSC3_ROUTE_SLS_PROCESSOR_INFO("patching mpd availabilityStartTime: from %.20s to %s, v: startNumber: %d, a: startNumber: %d",
                                            to_start_ptr, (char*)iso_now_timestamp,
                                            lls_sls_alc_monitor->last_closed_video_toi,
                                            lls_sls_alc_monitor->last_closed_audio_toi);
            
            for(int i=0; i < _ISO8601_DATE_TIME_LENGTH_; i++) {
                to_start_ptr[i] = iso_now_timestamp[i];
            }
            int mpd_new_payload_max_len = strlen(atsc3_mime_multipart_related_payload->payload) + 32;
            char* new_mpd_payload = calloc(mpd_new_payload_max_len + 1, sizeof(char));
            
            //todo: jjustman-2019-11-05: make this more robust...
            char* video_start = strstr(temp_lower_mpd, "contenttype=\"video\"");
            if(!video_start) goto fail;
            char* video_start_number_start = strstr(video_start, "startnumber=\"");
                                                                //1234567890123
            if(!video_start_number_start) goto fail;

            char* video_start_number_end = strstr(video_start_number_start, "\"");
            if(!video_start_number_end) goto fail;

            char* audio_start = strstr(temp_lower_mpd, "contenttype=\"audio\"");
            if(!audio_start) goto fail;

            char* audio_start_number_start = strstr(audio_start, "startnumber=\"");
            if(!audio_start_number_start) goto fail;

            char* audio_start_number_end = strstr(audio_start_number_start, "\"");
            if(!audio_start_number_end) goto fail;

            video_start_number_start[13] = '\0';
            audio_start_number_start[13] = '\0';

            char* mpd_patched_start_ptr = atsc3_mime_multipart_related_payload->payload;
            int video_start_number_start_pos = (video_start_number_start + 13) - temp_lower_mpd;
            mpd_patched_start_ptr[video_start_number_start_pos] = '\0';
            int video_start_number_end_pos = video_start_number_end  - temp_lower_mpd;
            
            char* mpd_patched_video_start_end_ptr = mpd_patched_start_ptr + video_start_number_end_pos + 2;
            
            int audio_start_number_start_pos = (audio_start_number_start + 13) - temp_lower_mpd;
            mpd_patched_start_ptr[audio_start_number_start_pos] = '\0';
            int audio_start_number_end_pos = audio_start_number_end  - temp_lower_mpd + 2;

            char* audio_start_number_end_ptr = mpd_patched_start_ptr + audio_start_number_end_pos;
            
            snprintf(new_mpd_payload, mpd_new_payload_max_len, "%s%d%s%d%s",
                     mpd_patched_start_ptr,
                     lls_sls_alc_monitor->last_closed_video_toi,
                     mpd_patched_video_start_end_ptr,
                     lls_sls_alc_monitor->last_closed_audio_toi,
                     audio_start_number_end_ptr);

            free(atsc3_mime_multipart_related_payload->payload);
            atsc3_mime_multipart_related_payload->payload = new_mpd_payload;
            atsc3_mime_multipart_related_payload->payload_length = strlen(new_mpd_payload);
                    
        }
    }
    
fail:
    _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("unable to patch startNumber values!");
   
}
