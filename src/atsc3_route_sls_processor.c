/*
 * atsc3_route_sls_processor.c
 *
 *  Created on: Apr 5, 2019
 *      Author: jjustman
 */


#include "atsc3_route_sls_processor.h"
#include "atsc3_lls_alc_utils.h"
#include "strnstr.h"


int _ROUTE_SLS_PROCESSOR_INFO_ENABLED = 1;
int _ROUTE_SLS_PROCESSOR_DEBUG_ENABLED = 0;

/*
 jjustman-2020-06-02 TODO: make sure we properly clear out lls_sls_alc_monitor on MBMS TSI=0, TOI change

 TODO: only process the full SLS if TSI=0, TOI= has changed, indiciating a new SLS payload...

 */

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

	    file_name = alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow, 0, 0);

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
				  lls_sls_alc_update_s_tsid_RS_dIpAddr_dPort_if_missing(udp_flow, lls_sls_alc_monitor, atsc3_sls_metadata_fragments->atsc3_route_s_tsid);

					//update our audio and video tsi and init
					lls_sls_alc_update_tsi_toi_from_route_s_tsid(lls_sls_alc_monitor, atsc3_sls_metadata_fragments->atsc3_route_s_tsid);
				}
				if(lls_sls_alc_monitor->atsc3_sls_metadata_fragments) {
				  //invoke any chained destructors as needed
				  atsc3_sls_metadata_fragments_free(&lls_sls_alc_monitor->atsc3_sls_metadata_fragments);
				}
				lls_sls_alc_monitor->atsc3_sls_metadata_fragments = atsc3_sls_metadata_fragments;

				// #1569
				// jjustman-2020-07-20 - TODO - add SLS TOI value here - *mbms_toi
				if(lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_sls_held_fragment &&
						lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_sls_held_fragment->raw_xml_fragment &&
						lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_sls_held_fragment->raw_xml_fragment->p_size) {
					if(lls_sls_alc_monitor->atsc3_sls_on_held_trigger_received_callback) {
						lls_sls_alc_monitor->atsc3_sls_on_held_trigger_received_callback(lls_sls_alc_monitor->atsc3_lls_slt_service->service_id, lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_sls_held_fragment->raw_xml_fragment);
					}
				}

				//jjustman-2020-07-07 -  TODO: dispatch any additional atsc3_monitor_events_sls here

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
				  if(!atsc3_mime_multipart_related_payload->content_type) {
				    _ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_process_from_alc_packet_and_file: content_type is null for tsi/toi:%u/%u", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);
				  } else {
				    
				    //jjustman-2019-11-05 - patch MPD type="dynamic" with availabilityStartTime to NOW and startNumber to the most recent A/V flows for TOI delivery
				    if(strncmp(atsc3_mime_multipart_related_payload->content_type, ATSC3_ROUTE_MPD_TYPE, __MIN(strlen(atsc3_mime_multipart_related_payload->content_type), strlen(ATSC3_ROUTE_MPD_TYPE))) == 0) {
				      atsc3_route_sls_patch_mpd_availability_start_time_and_start_number(atsc3_mime_multipart_related_payload, lls_sls_alc_monitor);
				    }
				  }
				  
				  //jjustman-2020-07-14 - mpd patching may result in an empty payload return (e.g. unable to properly patch due to error, so check that we don't flush out a possibly corrupt SLS object
				  //QQ: are 0 byte ROUTE objects acceptable?

				  if(atsc3_mime_multipart_related_payload->payload && atsc3_mime_multipart_related_payload->payload->p_buffer && atsc3_mime_multipart_related_payload->payload->p_size) {

					  char mbms_filename[1025] = { 0 };
					  snprintf(mbms_filename, 1024, "route/%d", lls_sls_alc_monitor->atsc3_lls_slt_service->service_id);
					  mkdir(mbms_filename, 0777);
					  snprintf(mbms_filename, 1024, "route/%d/%s", lls_sls_alc_monitor->atsc3_lls_slt_service->service_id, atsc3_mime_multipart_related_payload->sanitizied_content_location);
					  FILE* fp_mbms_file = fopen(mbms_filename, "w");
					  if(fp_mbms_file) {
						/* lldb: set set target.max-string-summary-length 10000 */
						_ATSC3_ROUTE_SLS_PROCESSOR_DEBUG("writing MBMS object to: %s, payload: %s", mbms_filename, atsc3_mime_multipart_related_payload->payload->p_buffer);

						size_t fwrite_size = fwrite(atsc3_mime_multipart_related_payload->payload->p_buffer, atsc3_mime_multipart_related_payload->payload->p_size, 1, fp_mbms_file);
						if(!fwrite_size) {
						  _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("fwrite for atsc3_mime_multipart_related_payload, file: %s, is: %d", mbms_filename, fwrite_size);
						}
						fclose(fp_mbms_file);
						fp_mbms_file = NULL;
					  } else {
						  _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("sls mbms fragment dump, original content_location: %s, unable to write to local path: %s", atsc3_mime_multipart_related_payload->sanitizied_content_location, mbms_filename);
					  }
				  } else {
					  _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("sls mbms fragment paylaod is null or p_size is zero, skipping write of original content_location: %s", atsc3_mime_multipart_related_payload->sanitizied_content_location);
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

    TODO: jjustman-2020-06-02: update to only patch values for  S-TSID.MediaInfo@repId == MPD.representation@id

    	S-TSID:
    		<MediaInfo contentType="video" repId="Video1_1">

    	MPD:

    	    <Representation bandwidth="6200000" codecs="hev1.2.4.L120.40" frameRate="30000/1001" height="1080" id="Video1_1" sar="1:1" width="1920">
            	<SegmentTemplate duration="2002000" initialization="video-init.mp4v" media="video-$Number$.mp4v" startNumber="0" timescale="1000000"/>

	-or-
		S-TSID:
           <MediaInfo contentType="audio" repId="a02_2"/>

		MPD:

          <Representation audioSamplingRate="48000" bandwidth="96000" codecs="ac-4.02.00.00" id="a02_2">
            <SegmentTemplate duration="2002000" initialization="a0-$RepresentationID$-init.mp4" media="a0-$RepresentationID$-$Number$.m4s" startNumber="0" timescale="1000000"/>

//TODO: confirm fragment is complete...

 */
void atsc3_route_sls_patch_mpd_availability_start_time_and_start_number(atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {

    if(lls_sls_alc_monitor->last_mpd_payload && (lls_sls_alc_monitor->last_mpd_payload_patched && !lls_sls_alc_monitor->has_discontiguous_toi_flow)) {
        //compare if our original vs. new payload has changed, and patch accordingly, otherwise swap out to our old payload
        if(strncmp(lls_sls_alc_monitor->last_mpd_payload->p_buffer, atsc3_mime_multipart_related_payload->payload->p_buffer, __MIN(strlen(lls_sls_alc_monitor->last_mpd_payload->p_buffer), atsc3_mime_multipart_related_payload->payload->p_buffer)) == 0) {
        	if(atsc3_mime_multipart_related_payload->payload) {
        		block_Destroy(&atsc3_mime_multipart_related_payload->payload);
        		atsc3_mime_multipart_related_payload->payload = block_Duplicate(lls_sls_alc_monitor->last_mpd_payload_patched);
			}
            return;
        }
    }

    if(lls_sls_alc_monitor->has_discontiguous_toi_flow) {
        _ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_patch_mpd_availability_start_time_and_start_number, has_discontiguous_toi_flow is true, rebuilding MPD!");
    }
    
    char* temp_lower_mpd = calloc(atsc3_mime_multipart_related_payload->payload->p_size, sizeof(char));
    for(int i=0; i < strlen(atsc3_mime_multipart_related_payload->payload->p_buffer); i++) {
        temp_lower_mpd[i] = tolower(atsc3_mime_multipart_related_payload->payload->p_buffer[i]);
    }
    
    if(strstr(temp_lower_mpd, "type=\"dynamic\"") != NULL) {
        if(lls_sls_alc_monitor->last_mpd_payload) {
            block_Destroy(&lls_sls_alc_monitor->last_mpd_payload);
        }
        lls_sls_alc_monitor->last_mpd_payload = block_Duplicate(atsc3_mime_multipart_related_payload->payload);
        
        //update our availabilityStartTime
        char* ast_char = strstr(temp_lower_mpd, _MPD_availability_start_time_VALUE_);
        if(ast_char) {
            time_t now;
            time(&now);

#ifdef __EXOPLAYER_ROUTE_DASH_SHIFT_AVAILABILITY_START_TIME__
            //move us N seconds in the future so exoplayer will fast-start (offset minBufferTime and timeShiftBufferDepthMs)
            now += __EXOPLAYER_ROUTE_DASH_SHIFT_AVAILABILITY_START_TIME__;
#endif

            int ast_char_pos_end = (ast_char + strlen(_MPD_availability_start_time_VALUE_)) - temp_lower_mpd;
            //replace 2019-10-09T19:03:50Z with now()...
            //        123456789012345678901
            //                 1         2

            //jjustman-2020-05-06 - hack, linux strftime will truncate 1 character short, ignore since we are null padded
            char iso_now_timestamp[_ISO8601_DATE_TIME_LENGTH_ + 2] = { 0 };
            strftime((char*)&iso_now_timestamp, _ISO8601_DATE_TIME_LENGTH_+1, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

            char* to_start_ptr = atsc3_mime_multipart_related_payload->payload->p_buffer + ast_char_pos_end + 1;
            /*
            _ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_patch_mpd_availability_start_time_and_start_number: patching mpd availabilityStartTime: from %.20s to %s, v: last_video_toi: %d, last_closed_video_toi: %d, a: last_audio_toi: %d, last_closed_audio_toi: %d, stpp: last_text_toi: %d, last_closed_text_toi: %d",
                                            to_start_ptr, (char*)iso_now_timestamp,
                                            lls_sls_alc_monitor->last_video_toi,
                                            lls_sls_alc_monitor->last_closed_video_toi,
                                            lls_sls_alc_monitor->last_audio_toi,
                                            lls_sls_alc_monitor->last_closed_audio_toi,
                                            lls_sls_alc_monitor->last_text_toi,
                                            lls_sls_alc_monitor->last_closed_text_toi);
                                        */
            
            for(int i=0; i < _ISO8601_DATE_TIME_LENGTH_; i++) {
                to_start_ptr[i] = iso_now_timestamp[i];
            }
            
            //jjustman-2020-07-14

        	atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context = atsc3_pcre2_regex_context_new(ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_PATTERN);

        	block_t* block_mpd = block_Duplicate(atsc3_mime_multipart_related_payload->payload);
        	block_Rewind(block_mpd);

        	atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector = atsc3_pcre2_regex_match(atsc3_pcre2_regex_context, block_mpd);
        	if(atsc3_pcre2_regex_match_capture_vector) {

				atsc3_pcre2_regex_match_capture_vector_dump(atsc3_pcre2_regex_match_capture_vector);

				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t* match_vector = atsc3_route_dash_find_matching_s_tsid_representations_from_mpd_pcre2_regex_matches(atsc3_pcre2_regex_match_capture_vector, &lls_sls_alc_monitor->atsc3_sls_alc_all_mediainfo_flow_v);
				if(!match_vector || !match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count) {
					_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("find_matching_s_tsid_representations - match vector is null or match_v.cound is 0!");
					goto error;
				}

				_ATSC3_ROUTE_SLS_PROCESSOR_DEBUG("got back %d match tuples <capture, media_info, alc_flow>", match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count);

				for(int i=0; i < match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count; i++) {
					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_t* atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match = match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.data[i];

					_ATSC3_ROUTE_SLS_PROCESSOR_DEBUG("s-tsid repId: %s, contentType: %s, startNumber replace start: %d, end: %d, toi value: %d",
							atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->rep_id,
							atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->content_type,
							atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_start,
							atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_end,
							atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_sls_alc_flow->last_closed_toi);

					//todo - hand this off to patch the mpd payload

				}

				block_t* patched_mpd = atsc3_route_dash_patch_mpd_manifest_from_matching_matching_s_tsid_representation_media_info_alc_flow_match_vector(match_vector , block_mpd);
				block_Rewind(patched_mpd);


				block_Destroy(&atsc3_mime_multipart_related_payload->payload);
				atsc3_mime_multipart_related_payload->payload = block_Duplicate(patched_mpd);
				lls_sls_alc_monitor->last_mpd_payload_patched = block_Duplicate(patched_mpd);

				block_Destroy(&patched_mpd);

				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_free(&match_vector);

				atsc3_pcre2_regex_match_capture_vector_free(&atsc3_pcre2_regex_match_capture_vector);


				//send a forced callback that our ROUTE/DASH flow is discontigous and needs to be reloaded
				//jjustman-2020-07-22 - atsc3_route_sls_process_from_alc_packet_and_file - TODO - fix this moving after we flush to disk
				if(lls_sls_alc_monitor->has_discontiguous_toi_flow && lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched) {
					lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched(lls_sls_alc_monitor->atsc3_lls_slt_service->service_id);
				}

        	} else {
        		_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("atsc3_pcre2_regex_match returned NULL - with block_mpd: %s", block_mpd->p_buffer);
           		block_Destroy(&atsc3_mime_multipart_related_payload->payload);

           		if(lls_sls_alc_monitor->last_mpd_payload_patched) {
           			atsc3_mime_multipart_related_payload->payload = block_Duplicate(lls_sls_alc_monitor->last_mpd_payload_patched);
            		_ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_pcre2_regex_match returned NULL - returning last patched payload! %s", lls_sls_alc_monitor->last_mpd_payload_patched);

           		}

        	}

        	atsc3_pcre2_regex_context_free(&atsc3_pcre2_regex_context);

        	block_Destroy(&block_mpd);

        	//end jjustman 2020-07-14

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
