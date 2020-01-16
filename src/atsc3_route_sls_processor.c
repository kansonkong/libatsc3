/*
 * atsc3_route_sls_processor.c
 *
 *  Created on: Apr 5, 2019
 *      Author: jjustman
 */


#include "atsc3_route_sls_processor.h"
#include "atsc3_lls_alc_utils.h"
#include "strnstr.h"


int _ROUTE_SLS_PROCESSOR_INFO_ENABLED = 0;
int _ROUTE_SLS_PROCESSOR_DEBUG_ENABLED = 0;

void atsc3_route_sls_process_from_alc_packet_and_file(udp_flow_t* udp_flow, alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
    char* file_name = NULL;
    char* mbms_toi_filename = NULL;
    
    FILE *fp = NULL;
    FILE *fp_mbms = NULL;
    
    xml_document_t* fdt_xml = NULL;
    atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = NULL;
    
	//check if our toi == 0, if so, reprocess our sls fdt in preperation for an upcoming actual mbms emission
	_ATSC3_ROUTE_SLS_PROCESSOR_DEBUG("alc_packet tsi/toi:%u/%u", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);

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

			//dump our full payload for debugging
			if(false) {

                fseek(fp_mbms, 0L, SEEK_END);
                int sz = ftell(fp_mbms);

                fseek(fp_mbms, 0L, SEEK_SET);
                char* mbms_temp_buffer = calloc(sz+1, sizeof(char));

                fread(mbms_temp_buffer, sz, 1, fp_mbms);
                _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("alc_packet tsi/toi:0/%u - filename: %s, mbms payload file size: %d, is:\n%s", *mbms_toi, mbms_toi_filename, sz, mbms_temp_buffer);

                free(mbms_temp_buffer);
                fseek(fp_mbms, 0L, SEEK_SET);



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
                        /* lldb: set set target.max-string-summary-length 10000 */
                        _ATSC3_ROUTE_SLS_PROCESSOR_INFO("writing MBMS object to: %s, payload: %s", mbms_filename, atsc3_mime_multipart_related_payload->payload);

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

/*
 
 special start number patching for pcap/rfcap replays with exoplayer2...


 */
void atsc3_route_sls_patch_mpd_availability_start_time_and_start_number(atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {

    if(lls_sls_alc_monitor->last_mpd_payload && (lls_sls_alc_monitor->last_mpd_payload_patched && !lls_sls_alc_monitor->has_discontiguous_toi_flow)) {
        //compare if our original vs. new payload has changed, and patch accordingly, otherwise swap out to our old payload
        if(strncmp(lls_sls_alc_monitor->last_mpd_payload->p_buffer, atsc3_mime_multipart_related_payload->payload, __MIN(strlen(lls_sls_alc_monitor->last_mpd_payload->p_buffer), atsc3_mime_multipart_related_payload->payload_length)) == 0) {
            free(atsc3_mime_multipart_related_payload->payload);
            atsc3_mime_multipart_related_payload->payload = strdup(lls_sls_alc_monitor->last_mpd_payload_patched->p_buffer);
            atsc3_mime_multipart_related_payload->payload_length = strlen(lls_sls_alc_monitor->last_mpd_payload_patched->p_buffer);
            return;
        }
    }

    if(lls_sls_alc_monitor->has_discontiguous_toi_flow) {
        _ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_patch_mpd_availability_start_time_and_start_number, has_discontiguous_toi_flow is true, rebuilding MPD!");
    }
    
    char* temp_lower_mpd = calloc(strlen(atsc3_mime_multipart_related_payload->payload)+1, sizeof(char));
    for(int i=0; i < strlen(atsc3_mime_multipart_related_payload->payload); i++) {
        temp_lower_mpd[i] = tolower(atsc3_mime_multipart_related_payload->payload[i]);
    }
    
    if(strstr(temp_lower_mpd, "type=\"dynamic\"") != NULL) {
        if(lls_sls_alc_monitor->last_mpd_payload) {
            block_Destroy(&lls_sls_alc_monitor->last_mpd_payload);
        }
        lls_sls_alc_monitor->last_mpd_payload = block_Alloc(strlen(atsc3_mime_multipart_related_payload->payload)+1);
        block_Write(lls_sls_alc_monitor->last_mpd_payload, (const uint8_t*)atsc3_mime_multipart_related_payload->payload, strlen(atsc3_mime_multipart_related_payload->payload));
        
        //update our availabilityStartTime
        char* ast_char = strstr(temp_lower_mpd, _MPD_availability_start_time_VALUE_);
        if(ast_char) {
            time_t now;
            time(&now);

            //jjustman-2019-12-29 - add in ~4s to "now" so mpd will have at least a 1s forward buffer (rounding down)...
            //TODO: fix me to be closer to horizon without first startup glitch from exoplayer
            now += 4;

            int ast_char_pos_end = (ast_char + strlen(_MPD_availability_start_time_VALUE_)) - temp_lower_mpd;
            //replace 2019-10-09T19:03:50Z with now()...
            //        123456789012345678901
            //                 1         2
            char iso_now_timestamp[_ISO8601_DATE_TIME_LENGTH_ + 1] = { 0 };
            strftime((char*)&iso_now_timestamp, _ISO8601_DATE_TIME_LENGTH_, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

            char* to_start_ptr = atsc3_mime_multipart_related_payload->payload + ast_char_pos_end + 1;
            _ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_patch_mpd_availability_start_time_and_start_number: patching mpd availabilityStartTime: from %.20s to %s, v: last_video_toi: %d, last_closed_video_toi: %d, a: last_audio_toi: %d, last_closed_audio_toi: %d",
                                            to_start_ptr, (char*)iso_now_timestamp,
                                            lls_sls_alc_monitor->last_video_toi,
                                            lls_sls_alc_monitor->last_closed_video_toi,
                                            lls_sls_alc_monitor->last_audio_toi,
                                            lls_sls_alc_monitor->last_closed_audio_toi);
            
            for(int i=0; i < _ISO8601_DATE_TIME_LENGTH_; i++) {
                to_start_ptr[i] = iso_now_timestamp[i];
            }
            
            //only patch startNumber values if we have lls_sls_alc_monitor->last_closed_video_toi and lls_sls_alc_monitor->last_closed_audio_toi
            //todo: jjustman-2019-11-05: make this more robust...

            if(lls_sls_alc_monitor->last_closed_video_toi && lls_sls_alc_monitor->last_closed_audio_toi) {

                _ATSC3_ROUTE_SLS_PROCESSOR_DEBUG("In-flight MPD is: \n%s", atsc3_mime_multipart_related_payload->payload);

                char* vcodec_representation_start_pos = strnstr(temp_lower_mpd, "video/mp4", strlen(temp_lower_mpd));
                if(!vcodec_representation_start_pos) {
                        _ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_patch_mpd_availability_start_time_and_start_number: vcodec_representation_start_pos is null for payload: %s",
                                                        atsc3_mime_multipart_related_payload->payload);
                    goto error;
                }
                char* acodec_representation_start_pos = strnstr(temp_lower_mpd, "audio/mp4", strlen(temp_lower_mpd));
                if(!acodec_representation_start_pos) {
                    _ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_patch_mpd_availability_start_time_and_start_number: acodec_representation_start_pos is null for payload: %s",
                                                    atsc3_mime_multipart_related_payload->payload);
                    goto error;
                }
                char* first_start_number_start = strstr(temp_lower_mpd, "startnumber=\"");
                if(!first_start_number_start) {
                    _ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_patch_mpd_availability_start_time_and_start_number: first_start_number_start is null for payload: %s",
                                                    atsc3_mime_multipart_related_payload->payload);
                    goto error;
                }
                char* second_start_number_start = strstr(first_start_number_start + 12, "startnumber=\"");
                if(!second_start_number_start) {
                    _ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_patch_mpd_availability_start_time_and_start_number: second_start_number_start is null for payload: %s",
                                                    atsc3_mime_multipart_related_payload->payload);
                    goto error;
                }
                char* video_start_number_start = NULL;
                char* audio_start_number_start = NULL;

                //hack for video/audio start_number sequencing
                if(vcodec_representation_start_pos > acodec_representation_start_pos) {
                    video_start_number_start = first_start_number_start;
                    audio_start_number_start = second_start_number_start;
                } else {
                    video_start_number_start = first_start_number_start;
                    audio_start_number_start = second_start_number_start;
                }

                int mpd_new_payload_max_len = strlen(atsc3_mime_multipart_related_payload->payload) + 32;
                char* new_mpd_payload = calloc(mpd_new_payload_max_len + 1, sizeof(char));

                char* video_start_number_end = strstr(video_start_number_start, "\"");
                if(!video_start_number_end) goto error;
            
                char* audio_start_number_end = strstr(audio_start_number_start, "\"");
                if(!audio_start_number_end) goto error;

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

                //if !lls_sls_alc_monitor->has_discontiguous_toi_flow,  use the last_closed video/audio toi, otherwise use the 'current' video/audio toi
                if(vcodec_representation_start_pos < acodec_representation_start_pos) {
                    snprintf(new_mpd_payload, mpd_new_payload_max_len, "%s%d%s%d%s",
                             mpd_patched_start_ptr,
                             (!lls_sls_alc_monitor->has_discontiguous_toi_flow ? lls_sls_alc_monitor->last_closed_video_toi : lls_sls_alc_monitor->last_video_toi),
                             mpd_patched_video_start_end_ptr,
                             (!lls_sls_alc_monitor->has_discontiguous_toi_flow ? lls_sls_alc_monitor->last_closed_audio_toi : lls_sls_alc_monitor->last_audio_toi),
                             audio_start_number_end_ptr);
                } else {
                    snprintf(new_mpd_payload, mpd_new_payload_max_len, "%s%d%s%d%s",
                             mpd_patched_start_ptr,
                             (!lls_sls_alc_monitor->has_discontiguous_toi_flow ? lls_sls_alc_monitor->last_closed_audio_toi : lls_sls_alc_monitor->last_audio_toi),
                             mpd_patched_video_start_end_ptr,
                             (!lls_sls_alc_monitor->has_discontiguous_toi_flow ? lls_sls_alc_monitor->last_closed_video_toi : lls_sls_alc_monitor->last_video_toi),
                             audio_start_number_end_ptr);
                }

                free(atsc3_mime_multipart_related_payload->payload);
                atsc3_mime_multipart_related_payload->payload = new_mpd_payload;
                atsc3_mime_multipart_related_payload->payload_length = strlen(new_mpd_payload);

                if(lls_sls_alc_monitor->last_mpd_payload_patched) {
                    block_Destroy(&lls_sls_alc_monitor->last_mpd_payload_patched);

                }

                lls_sls_alc_monitor->last_mpd_payload_patched = block_Alloc(atsc3_mime_multipart_related_payload->payload_length + 1);
                block_Write(lls_sls_alc_monitor->last_mpd_payload_patched, (const uint8_t*)atsc3_mime_multipart_related_payload->payload, atsc3_mime_multipart_related_payload->payload_length);

                //send a forced callback that our ROUTE/DASH flow is discontigous and needs to be reloaded
                if(lls_sls_alc_monitor->has_discontiguous_toi_flow && lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched) {
                    lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched(lls_sls_alc_monitor->atsc3_lls_slt_service->service_id);
                }

                _ATSC3_ROUTE_SLS_PROCESSOR_DEBUG("Final MPD is: \n%s", atsc3_mime_multipart_related_payload->payload);

            } else {
                _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("unable to patch startNumber values: no closed video/audio toi, v: %d, a: %d", lls_sls_alc_monitor->last_closed_video_toi, lls_sls_alc_monitor->last_closed_audio_toi);
            }
        } else {
            _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("unable to patch startNumber values: "_MPD_availability_start_time_VALUE_" present");
        }
    } else {
        _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("unable to patch startNumber values: MPD is missing type=dynamic");
    }

    lls_sls_alc_monitor->has_discontiguous_toi_flow = false;

    return;
    
error:
    _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("unable to patch startNumber values!");
   
}
