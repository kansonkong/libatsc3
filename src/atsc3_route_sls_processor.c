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
int _ROUTE_SLS_PROCESSOR_TRACE_ENABLED = 0;


/*
	method: atsc3_route_sls_process_from_alc_packet_and_file
 
	jjustman-2020-06-02 TODO: make sure we properly clear out lls_sls_alc_monitor on MBMS TSI=0, TOI change

	jjustman-2020-07-28 - todo: use atsc3_route_object for fp handle reference
 */

void atsc3_route_sls_process_from_alc_packet_and_file(udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
    char* file_name = NULL;
    char* mbms_toi_filename = NULL;
    
    FILE *fp = NULL;
    FILE *fp_mbms = NULL;
    
    xml_document_t* fdt_xml = NULL;
    atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments_pending = NULL;
    
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
			atsc3_fdt_instance_t* atsc3_fdt_instance_new = atsc3_fdt_instance_parse_from_xml_document(fdt_xml);
			if(!atsc3_fdt_instance_new) {
				_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("alc_packet tsi/toi:0/0 instance is null!");
				goto cleanup;
			}
			atsc3_fdt_instance_dump(atsc3_fdt_instance_new);
			
			if(lls_sls_alc_monitor) {
				//jjustman-2020-08-04 - SLS version change, watchout,
				//set this new fdt as a pending reference to check if we are ready for switchover
                if(lls_sls_alc_monitor->atsc3_fdt_instance) {
					bool should_persist_new_as_pending_edft = false;
					atsc3_fdt_instance_t* atsc3_fdt_instance_current = lls_sls_alc_monitor->atsc3_fdt_instance;

					if(atsc3_fdt_instance_current->efdt_version != atsc3_fdt_instance_new->efdt_version) {
						should_persist_new_as_pending_edft = true;
					} else if(atsc3_fdt_instance_current->atsc3_fdt_file_v.count && atsc3_fdt_instance_new->atsc3_fdt_file_v.count) {
						atsc3_fdt_file_t* atsc3_fdt_file_current = atsc3_fdt_instance_current->atsc3_fdt_file_v.data[0];
						atsc3_fdt_file_t* atsc3_fdt_file_new = atsc3_fdt_instance_new->atsc3_fdt_file_v.data[0];
						
						if(atsc3_fdt_file_current->toi != atsc3_fdt_file_new->toi) {
							should_persist_new_as_pending_edft = true;
						}
					}
					
					if(should_persist_new_as_pending_edft) {
						if(lls_sls_alc_monitor->atsc3_fdt_instance_pending) {
							_ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_process_from_alc_packet_and_file: ABANDONING: atsc3_fdt_instance_pending: %p, SLS fdt tsi/toi: 0/0 file: %s, version change from %d to %d",
															lls_sls_alc_monitor->atsc3_fdt_instance_pending,
															file_name,
															atsc3_fdt_instance_current->efdt_version,
															atsc3_fdt_instance_new->efdt_version
							);
							atsc3_fdt_instance_free(&lls_sls_alc_monitor->atsc3_fdt_instance_pending);
						} else {
							_ATSC3_ROUTE_SLS_PROCESSOR_INFO("atsc3_route_sls_process_from_alc_packet_and_file: SLS fdt tsi/toi: 0/0 file: %s, version change from %d to %d",
															file_name,
															atsc3_fdt_instance_current->efdt_version,
															atsc3_fdt_instance_new->efdt_version
							);
						lls_sls_alc_monitor->atsc3_fdt_instance_pending = atsc3_fdt_instance_new;
						}
					} else {
						//otherwise free this non-changed fdt_instance
						atsc3_fdt_instance_free(&atsc3_fdt_instance_new);
					}
				} else {
					if(lls_sls_alc_monitor->atsc3_fdt_instance_pending) {
						_ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_process_from_alc_packet_and_file: no current atsc3_fdt_instance and ABANDONING: atsc3_fdt_instance_pending: %p, SLS fdt tsi/toi: 0/0 file: %s",
													lls_sls_alc_monitor->atsc3_fdt_instance_pending,
													file_name);
						atsc3_fdt_instance_free(&lls_sls_alc_monitor->atsc3_fdt_instance_pending);

					}
					lls_sls_alc_monitor->atsc3_fdt_instance_pending = atsc3_fdt_instance_new;
				}
			}
		}
	} else {
		//check to see if we have a pending atsc3_fdt_instance and if we need to swap/roll forward any flows
		if(lls_sls_alc_monitor && lls_sls_alc_monitor->atsc3_fdt_instance_pending) {
			atsc3_fdt_instance_t* atsc3_fdt_instance_pending = lls_sls_alc_monitor->atsc3_fdt_instance_pending;
			
			uint32_t* mbms_toi = atsc3_mbms_envelope_find_toi_from_fdt(atsc3_fdt_instance_pending);
			if(!mbms_toi) {
				_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("Unable to find MBMS TOI for atsc3_fdt_instance_pending: %p", atsc3_fdt_instance_pending);
				goto cleanup;
			}

			mbms_toi_filename = alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow, 0, *mbms_toi);

			fp_mbms = fopen(mbms_toi_filename, "r");
			if(!fp_mbms) {
				_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("alc_packet tsi/toi:0/%u filename: %s is null for pending: %p!", *mbms_toi, mbms_toi_filename, atsc3_fdt_instance_pending);
				goto cleanup;
			}
			
			atsc3_sls_metadata_fragments_pending = atsc3_mbms_envelope_to_sls_metadata_fragments_parse_from_fdt_fp(fp_mbms);
			if(!atsc3_sls_metadata_fragments_pending) {
				_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("atsc3_sls_metadata_fragments_pending is NULL, tsi/toi:0/%u filename: %s for pending: %p!", *mbms_toi, mbms_toi_filename, atsc3_fdt_instance_pending);
				goto cleanup;
			}
			
			if(atsc3_sls_metadata_fragments_pending->atsc3_route_s_tsid) {
			  lls_sls_alc_update_s_tsid_RS_dIpAddr_dPort_if_missing(udp_flow, lls_sls_alc_monitor, atsc3_sls_metadata_fragments_pending->atsc3_route_s_tsid);

				//update our mediainfo audio and video tsi and init
				lls_sls_alc_update_all_mediainfo_flow_v_from_route_s_tsid(lls_sls_alc_monitor, atsc3_sls_metadata_fragments_pending->atsc3_route_s_tsid);
			}
			
			// #1569
			// jjustman-2020-07-20 - TODO - add SLS TOI value here - *mbms_toi
			bool held_payload_has_changed = atsc3_lls_sls_alc_monitor_sls_metadata_fragements_has_held_changed(lls_sls_alc_monitor, atsc3_sls_metadata_fragments_pending);
			
			if(held_payload_has_changed) {
				if(lls_sls_alc_monitor->atsc3_sls_on_held_trigger_received_callback || lls_sls_alc_monitor->atsc3_sls_on_held_trigger_received_with_version_callback) {
					block_t* atsc3_held_fragment_block_t = atsc3_sls_metadata_fragements_get_sls_held_fragment_duplicate_raw_xml_or_empty(atsc3_sls_metadata_fragments_pending);
					if(lls_sls_alc_monitor->atsc3_sls_on_held_trigger_received_callback) {
						lls_sls_alc_monitor->atsc3_sls_on_held_trigger_received_callback(lls_sls_alc_monitor->atsc3_lls_slt_service->service_id, atsc3_held_fragment_block_t);
					}
					if(lls_sls_alc_monitor->atsc3_sls_on_held_trigger_received_with_version_callback) {
						lls_sls_alc_monitor->atsc3_sls_on_held_trigger_received_with_version_callback(lls_sls_alc_monitor->atsc3_lls_slt_service->service_id, atsc3_fdt_instance_pending->efdt_version, atsc3_held_fragment_block_t);
					}
					block_Destroy(&atsc3_held_fragment_block_t);
				}
			}

			//jjustman-2020-07-07 -  TODO: dispatch any additional atsc3_monitor_events_sls here
			//jjustman-2020-08-05 - TODO: clear out any "removed" sls fragments from our lls_sls_alc_monitor->atsc3_sls_metadata_fragments from on disk...
			
			for(int i=0; i < atsc3_sls_metadata_fragments_pending->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count; i++) {
				bool should_write_sls_fragment_to_file = true;
				
				atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = atsc3_sls_metadata_fragments_pending->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i];
				if(!atsc3_mime_multipart_related_payload->content_type) {
					_ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_process_from_alc_packet_and_file: content_type is null for tsi/toi:%u/%u", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);
				} else {
					//jjustman-2020-08-05 - skip patching our MPD here, it will happen in our parent call flow via atsc3_route_sls_process_from_sls_metadata_fragments_patch_mpd_availability_start_time_and_start_number
					if(strncmp(atsc3_mime_multipart_related_payload->content_type, ATSC3_ROUTE_MPD_TYPE, __MIN(strlen(atsc3_mime_multipart_related_payload->content_type), strlen(ATSC3_ROUTE_MPD_TYPE))) == 0) {
						should_write_sls_fragment_to_file = false;
					}
				}
			
				if(should_write_sls_fragment_to_file) {
					int32_t bytes_written = atsc3_sls_write_mime_multipart_related_payload_to_file(atsc3_mime_multipart_related_payload, lls_sls_alc_monitor);
				}
			}
			
			//perform our swap of SLS and fragments
			
			//clear out our old SLS, and invoke any chained destructors as needed for our prior sls
			if(lls_sls_alc_monitor->atsc3_fdt_instance) {
				//jjustman-2020-08-04 TODO: free any "dead" alc flows here...
				//tsc3_sls_alc_flow_v 	atsc3_sls_alc_all_s_tsid_flow_v;
				atsc3_fdt_instance_free(&lls_sls_alc_monitor->atsc3_fdt_instance);
			}
			lls_sls_alc_monitor->atsc3_fdt_instance = atsc3_fdt_instance_pending;
			lls_sls_alc_monitor->atsc3_fdt_instance_pending = NULL;
			
			//clear out our old metadata fragements, and replace with pending
			if(lls_sls_alc_monitor->atsc3_sls_metadata_fragments) {
			  atsc3_sls_metadata_fragments_free(&lls_sls_alc_monitor->atsc3_sls_metadata_fragments);
			}
			lls_sls_alc_monitor->atsc3_sls_metadata_fragments = atsc3_sls_metadata_fragments_pending;
			lls_sls_alc_monitor->atsc3_sls_metadata_fragments_pending = NULL;
			
		} else if (lls_sls_alc_monitor && lls_sls_alc_monitor->atsc3_sls_metadata_fragments) {
			//perform a sanity check to make sure our sls fragments are present on disk, this should not normally happen with a monitored service and atsc3_route_object, which will avoid eviction for SLS fragments

			for(int i=0; i < lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count; i++) {
				bool should_check_sls_fragment_on_disk_present = true;

				atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i];
				if(!atsc3_mime_multipart_related_payload->content_type) {
					_ATSC3_ROUTE_SLS_PROCESSOR_TRACE("atsc3_route_sls_process_from_alc_packet_and_file: content_type is null for tsi/toi:%u/%u", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);
				} else {
					//jjustman-2020-08-05 - we can re-flush our mpd only if we have patched it (reference block_t assignment in lls_sls_alc_monitor->last_mpd_payload_patched)
					if(strncmp(atsc3_mime_multipart_related_payload->content_type, ATSC3_ROUTE_MPD_TYPE, __MIN(strlen(atsc3_mime_multipart_related_payload->content_type), strlen(ATSC3_ROUTE_MPD_TYPE))) == 0) {
						should_check_sls_fragment_on_disk_present = (lls_sls_alc_monitor->last_mpd_payload_patched && lls_sls_alc_monitor->last_mpd_payload_patched->p_size);
					}
				}

				if(should_check_sls_fragment_on_disk_present) {
					char* mbms_filename = atsc3_sls_generate_filename_from_atsc3_mime_multipart_related_payload(atsc3_mime_multipart_related_payload, lls_sls_alc_monitor);
					struct stat st = {0};
					if(stat(mbms_filename, &st) != 0) {
						int32_t bytes_written = atsc3_sls_write_mime_multipart_related_payload_to_file(atsc3_mime_multipart_related_payload, lls_sls_alc_monitor);
						_ATSC3_ROUTE_SLS_PROCESSOR_INFO("atsc3_route_sls_process_from_alc_packet_and_file: lls_sls_alc_monitor fragment check for on-disk object: %s missing, re-persisting with size: %d", mbms_filename, bytes_written);
					}
					freesafe((void*)mbms_filename);
				}
			}


		} else {
			_ATSC3_ROUTE_SLS_PROCESSOR_TRACE("No lls_sls_alc_monitor and no pending atsc3_fdt_instance to process in TSI:0!");
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
        xml_document_free(fdt_xml, true);
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
//jjustman-2020-08-05 -
bool atsc3_route_sls_process_from_sls_metadata_fragments_patch_mpd_availability_start_time_and_start_number(atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
	bool fragment_route_sls_patch_mpd = false;
	if(!lls_sls_alc_monitor) {
		_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("atsc3_route_sls_process_from_sls_metadata_fragments_patch_mpd_availability_start_time_and_start_number, lls_sls_alc_monitor is NULL!");
		return false;
	}
	if(!lls_sls_alc_monitor->atsc3_sls_metadata_fragments) {
		_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("atsc3_route_sls_process_from_sls_metadata_fragments_patch_mpd_availability_start_time_and_start_number, lls_sls_alc_monitor->atsc3_sls_metadata_fragments is NULL! lls_sls_alc_monitor: %p", lls_sls_alc_monitor);
		return false;
	}
	
	if(lls_sls_alc_monitor->last_mpd_payload_patched) {
		_ATSC3_ROUTE_SLS_PROCESSOR_DEBUG("atsc3_route_sls_process_from_sls_metadata_fragments_patch_mpd_availability_start_time_and_start_number, lls_sls_alc_monitor->last_mpd_payload_patched is: %p, skipping patch and returning false!", lls_sls_alc_monitor->last_mpd_payload_patched);
		return false;
	}
	
	for(int i=0; i < atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count; i++) {
		atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i];
		if(atsc3_mime_multipart_related_payload->content_type) {
			//jjustman-2019-11-05 - patch MPD type="dynamic" with availabilityStartTime to NOW and startNumber to the most recent A/V flows for TOI delivery
			if(strncmp(atsc3_mime_multipart_related_payload->content_type, ATSC3_ROUTE_MPD_TYPE, __MIN(strlen(atsc3_mime_multipart_related_payload->content_type), strlen(ATSC3_ROUTE_MPD_TYPE))) == 0) {
				fragment_route_sls_patch_mpd = atsc3_route_sls_patch_mpd_availability_start_time_and_start_number(atsc3_mime_multipart_related_payload, lls_sls_alc_monitor);
				
				if(fragment_route_sls_patch_mpd) {
					atsc3_sls_write_mime_multipart_related_payload_to_file(atsc3_mime_multipart_related_payload, lls_sls_alc_monitor);
					//jjustman-2020-07-27 - send a forced callback that our ROUTE/DASH flow is discontigous and needs to be reloaded once we are written to disk
					if(lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched_callback) {
						lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched_callback(lls_sls_alc_monitor->atsc3_lls_slt_service->service_id);
					}
					
					if(lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched_with_filename_callback) {
						char* mpd_filename = atsc3_sls_generate_filename_from_atsc3_mime_multipart_related_payload(atsc3_mime_multipart_related_payload, lls_sls_alc_monitor);

						lls_sls_alc_monitor->atsc3_lls_sls_alc_on_route_mpd_patched_with_filename_callback(lls_sls_alc_monitor->atsc3_lls_slt_service->service_id, mpd_filename);
						freeclean((void**)&mpd_filename);
					}
				}
			}
		}
	}
	return fragment_route_sls_patch_mpd;
}

//QQ: are 0 byte ROUTE objects acceptable?
int32_t atsc3_sls_write_mime_multipart_related_payload_to_file(atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
	int32_t bytes_written = -1;
	char* mbms_filename = NULL;
	
	if(!lls_sls_alc_monitor || !lls_sls_alc_monitor->atsc3_lls_slt_service) {
		_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("atsc3_sls_write_mime_multipart_related_payload_to_file: lls_sls_alc_monitor is: %p or no ->atsc3_lls_slt_service assigned", lls_sls_alc_monitor);
		return -2;
	}
	if(!atsc3_mime_multipart_related_payload) {
		_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("atsc3_sls_write_mime_multipart_related_payload_to_file: atsc3_mime_multipart_related_payload is NULL!");
		return -2;
	}

	if(atsc3_mime_multipart_related_payload->payload && atsc3_mime_multipart_related_payload->payload->p_buffer && atsc3_mime_multipart_related_payload->payload->p_size) {

		mbms_filename = atsc3_sls_generate_filename_from_atsc3_mime_multipart_related_payload(atsc3_mime_multipart_related_payload, lls_sls_alc_monitor);
		FILE* fp_mbms_file = fopen(mbms_filename, "w");
		if(fp_mbms_file) {
		  /* lldb: set set target.max-string-summary-length 10000 */
		  _ATSC3_ROUTE_SLS_PROCESSOR_DEBUG("atsc3_sls_write_mime_multipart_related_payload_to_file: writing MBMS object to: %s, payload: %s", mbms_filename, atsc3_mime_multipart_related_payload->payload->p_buffer);

		  size_t fwrite_size = fwrite(atsc3_mime_multipart_related_payload->payload->p_buffer, atsc3_mime_multipart_related_payload->payload->p_size, 1, fp_mbms_file);
		  if(!fwrite_size) {
			_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("atsc3_sls_write_mime_multipart_related_payload_to_file: fwrite for atsc3_mime_multipart_related_payload, file: %s, is: %zu", mbms_filename, fwrite_size);
		  } else {
			  bytes_written = atsc3_mime_multipart_related_payload->payload->p_size;
		  }
		  fclose(fp_mbms_file);
		  fp_mbms_file = NULL;
		} else {
			_ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_sls_write_mime_multipart_related_payload_to_file: sls mbms fragment dump, original content_location: %s, unable to write to local path: %s", atsc3_mime_multipart_related_payload->sanitizied_content_location, mbms_filename);
		}
		freeclean((void**)&mbms_filename);
	} else {
		_ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_sls_write_mime_multipart_related_payload_to_file: sls mbms fragment dump, 0 byte payload? original content_location: %s,local path: %s, atsc3_mime_multipart_related_payload->payload: %p",atsc3_mime_multipart_related_payload->sanitizied_content_location, mbms_filename, atsc3_mime_multipart_related_payload->payload );
	}
	
	return bytes_written;
}

char* atsc3_sls_generate_filename_from_atsc3_mime_multipart_related_payload(atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
	
	char stack_filename[1025] = { 0 };
	snprintf(stack_filename, 1024, "route/%d", lls_sls_alc_monitor->atsc3_lls_slt_service->service_id);
	mkdir(stack_filename, 0777);
	snprintf(stack_filename, 1024, "route/%d/%s", lls_sls_alc_monitor->atsc3_lls_slt_service->service_id, atsc3_mime_multipart_related_payload->sanitizied_content_location);
	
	return strdup(stack_filename);
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

 
 https://github.com/google/shaka-player/issues/237
	
	For MPDs with type dynamic, it is important to look at the combo "availabilityStartTime + period@start" (AST + PST) and "startNumber"
	and the current time.
	
	startNumber refers to the segment that is available one segmentDuration after the period start
	(at the period start, only the init segments are available),
	
	For dynamic MPDs, you shall "never" start to play with startNumber, but the latest available segment is
	LSN = floor( (now - (availabilityStartTime+PST))/segmentDuration + startNumber- 1).
	
	It is also important to align the mediaTimeLine (based on baseMediaDecodeTime) so that it starts at 0 at the beginning of the period.
	
	content_type    char *    "application/dash+xml"    0x00000001064c8fe0
 */
 
bool atsc3_route_sls_patch_mpd_availability_start_time_and_start_number(atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
	bool successfully_patched = false;
	atsc3_pcre2_regex_context_t* atsc3_pcre2_regex_context = NULL;

	//compare if our original vs. new payload has changed, and patch accordingly, otherwise swap out to our old payload
    if(lls_sls_alc_monitor->last_mpd_payload && (lls_sls_alc_monitor->last_mpd_payload_patched && !lls_sls_alc_monitor->has_discontiguous_toi_flow)) {
        if(strncmp((const char*)lls_sls_alc_monitor->last_mpd_payload->p_buffer, (const char*)atsc3_mime_multipart_related_payload->payload->p_buffer, __MIN(strlen((const char*)lls_sls_alc_monitor->last_mpd_payload->p_buffer), strlen((const char*)atsc3_mime_multipart_related_payload->payload->p_buffer))) == 0) {
//	jjustman-2020-08-05 - do not replace our payload with our patched unless we actually performed a patch update
//        	if(atsc3_mime_multipart_related_payload->payload) {
//        		block_Destroy(&atsc3_mime_multipart_related_payload->payload);
//        		atsc3_mime_multipart_related_payload->payload = block_Duplicate(lls_sls_alc_monitor->last_mpd_payload_patched);
//			}
            return false;
        }
    }

	block_t* in_flight_last_mpd_payload = block_Duplicate(atsc3_mime_multipart_related_payload->payload);

    if(lls_sls_alc_monitor->has_discontiguous_toi_flow || !lls_sls_alc_monitor->last_mpd_payload) {
        _ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_route_sls_patch_mpd_availability_start_time_and_start_number, lls_sls_alc_monitor: %p, serviceId: %d, has_discontiguous_toi_flow: %d, last_mpd_payload: %p, rebuilding MPD!",
        		lls_sls_alc_monitor,
        		(lls_sls_alc_monitor->atsc3_lls_slt_service ? lls_sls_alc_monitor->atsc3_lls_slt_service->service_id : 0),
				lls_sls_alc_monitor->has_discontiguous_toi_flow,
        		lls_sls_alc_monitor->last_mpd_payload);
    }
    
    char* temp_lower_mpd = calloc(atsc3_mime_multipart_related_payload->payload->p_size+1, sizeof(char));
    for(int i=0; i < strlen((const char*)atsc3_mime_multipart_related_payload->payload->p_buffer); i++) {
        temp_lower_mpd[i] = tolower(atsc3_mime_multipart_related_payload->payload->p_buffer[i]);
    }
    
    if(strstr(temp_lower_mpd, "type=\"dynamic\"") != NULL) {

        //update our availabilityStartTime
        char* ast_char = strstr(temp_lower_mpd, _MPD_availability_start_time_VALUE_);
        if(ast_char) {
            time_t now;
            time(&now);

#ifdef __EXOPLAYER_ROUTE_DASH_SHIFT_AVAILABILITY_START_TIME__
            //move us N seconds in the future so exoplayer will fast-start (offset minBufferTime and timeShiftBufferDepthMs)
            now += __EXOPLAYER_ROUTE_DASH_SHIFT_AVAILABILITY_START_TIME__;
#else
            now += 2; //shift us forward 2 seconds so we can try to stay behind the horizon
#endif

            int ast_char_pos_end = (ast_char + strlen(_MPD_availability_start_time_VALUE_)) - temp_lower_mpd;
            //replace 2019-10-09T19:03:50Z with now()...
            //        123456789012345678901
            //                 1         2

	    //jjustman-2020-05-06 - hack, linux strftime will truncate 1 character short, ignore since we are null padded
            char iso_now_timestamp[_ISO8601_DATE_TIME_LENGTH_ + 2] = { 0 };
            strftime((char*)&iso_now_timestamp, _ISO8601_DATE_TIME_LENGTH_+1, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

            char* to_start_ptr = (char*) atsc3_mime_multipart_related_payload->payload->p_buffer + ast_char_pos_end + 1;
 
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

            atsc3_pcre2_regex_context = atsc3_pcre2_regex_context_new(ATSC3_ROUTE_DASH_MPD_REPRESENTATION_ID_SEGMENT_TEMPLATE_START_NUMBER_REGEX_PATTERN);

        	block_t* block_mpd = block_Duplicate(atsc3_mime_multipart_related_payload->payload);
        	block_Rewind(block_mpd);

        	atsc3_pcre2_regex_match_capture_vector_t* atsc3_pcre2_regex_match_capture_vector = atsc3_pcre2_regex_match(atsc3_pcre2_regex_context, block_mpd);
        	if(atsc3_pcre2_regex_match_capture_vector) {

				atsc3_pcre2_regex_match_capture_vector_dump(atsc3_pcre2_regex_match_capture_vector);

				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_t* match_vector = atsc3_route_dash_find_matching_s_tsid_representations_from_mpd_pcre2_regex_matches(atsc3_pcre2_regex_match_capture_vector, &lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v);
				if(!match_vector || !match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count) {
		        	block_Destroy(&block_mpd);
		        	atsc3_pcre2_regex_match_capture_vector_free(&atsc3_pcre2_regex_match_capture_vector);

					_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("find_matching_s_tsid_representations - match vector is null or match_v.cound is 0!");
					goto error;
				}

				_ATSC3_ROUTE_SLS_PROCESSOR_DEBUG("got back %d match tuples <capture, media_info, alc_flow>", match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count);

				for(int i=0; i < match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.count; i++) {
					atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_t* atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match = match_vector->atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_v.data[i];

					_ATSC3_ROUTE_SLS_PROCESSOR_DEBUG("s-tsid repId: %s, contentType: %s, startNumber replace start: %zu, end: %zu, toi value: %d",
							atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->rep_id,
							atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_route_s_content_info_media_info->content_type,
							atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_start,
							atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_preg2_regex_match_capture_start_number->match_end,
							atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match->atsc3_sls_alc_flow->last_closed_toi);
				}

				block_t* patched_mpd = atsc3_route_dash_patch_mpd_manifest_from_matching_matching_s_tsid_representation_media_info_alc_flow_match_vector(match_vector , block_mpd);
				block_Rewind(patched_mpd);

				block_Destroy(&atsc3_mime_multipart_related_payload->payload);
				atsc3_mime_multipart_related_payload->payload = block_Duplicate(patched_mpd);
				
				block_Destroy(&lls_sls_alc_monitor->last_mpd_payload_patched);
				lls_sls_alc_monitor->last_mpd_payload_patched = block_Duplicate(patched_mpd);

				block_Destroy(&patched_mpd);

				atsc3_route_dash_matching_s_tsid_representation_media_info_alc_flow_match_vector_free(&match_vector);
				atsc3_pcre2_regex_match_capture_vector_free(&atsc3_pcre2_regex_match_capture_vector);

				block_Destroy(&lls_sls_alc_monitor->last_mpd_payload);
				lls_sls_alc_monitor->last_mpd_payload = block_Duplicate(in_flight_last_mpd_payload);
				
				successfully_patched = true;
				lls_sls_alc_monitor->has_discontiguous_toi_flow = false;

        	} else {
        		_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("atsc3_pcre2_regex_match returned NULL - with block_mpd: %s", block_mpd->p_buffer);
//do not do this...
//           		if(lls_sls_alc_monitor->last_mpd_payload_patched) {
//					block_Destroy(&atsc3_mime_multipart_related_payload->payload);
//
//           			atsc3_mime_multipart_related_payload->payload = block_Duplicate(lls_sls_alc_monitor->last_mpd_payload_patched);
//            		_ATSC3_ROUTE_SLS_PROCESSOR_WARN("atsc3_pcre2_regex_match returned NULL - returning last patched payload! %s", lls_sls_alc_monitor->last_mpd_payload_patched->p_buffer);
//					successfully_patched = false;
//				}
        	}

        	atsc3_pcre2_regex_context_free(&atsc3_pcre2_regex_context);

        	block_Destroy(&block_mpd);
        	block_Destroy(&in_flight_last_mpd_payload);

        } else {
            _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("unable to patch startNumber values: "_MPD_availability_start_time_VALUE_" present");
            goto error;
        }
    } else {
        _ATSC3_ROUTE_SLS_PROCESSOR_ERROR("unable to patch startNumber values: MPD is missing type=dynamic");
        goto error;

    }

    freesafe(temp_lower_mpd);

    return successfully_patched;
    
error:
	if(atsc3_pcre2_regex_context) {
		atsc3_pcre2_regex_context_free(&atsc3_pcre2_regex_context);
	}

	block_Destroy(&in_flight_last_mpd_payload);

    freesafe(temp_lower_mpd);

    return false;
}
