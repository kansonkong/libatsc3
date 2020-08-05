/*
 * atsc3_monitor_events_alc.h
 *
 *  Created on: Jul 7, 2020
 *      Author: jjustman
 *
 *  Purpose:
 *
 *  	monitor event dispatcher callback signatures for relevant ALC (Asynchronous Layered Coding) events
 *
 */

#ifndef ATSC3_MONITOR_EVENTS_ALC_H
#define ATSC3_MONITOR_EVENTS_ALC_H

#include "atsc3_utils.h"
#include "atsc3_route_package_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*atsc3_alc_on_object_close_flag_s_tsid_content_location_f)(uint32_t tsi, uint32_t toi, char* s_tsid_content_location);
typedef void (*atsc3_alc_on_route_mpd_patched_f)(uint16_t service_id);

typedef void (*atsc3_alc_on_package_extract_completed_f)(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload_t);


//jjustman-2020-07-01 #1569: dispatch async event notification that package extraction has completed
//codePoint==3 || codePoint == 4
// filesystem path that .pkg was extracted to bcastEntryPackageUrl/
// appContextIdList (scope)
// list<string> objects ~

//A/344 - ICR - when you receieve a packageExtraction complete with appContextIdList, and then within ~5s receive a HELD emission
//                  then you can launch the <bcastEntryPackageUrl, bcastEntryPageUrl>

typedef void (*atsc3_alc_on_package_extracted_f)(uint32_t tsi, uint32_t toi, uint8_t lct_codepoint, char* stsid_content_location, char* filesystem_content_location, char** payload);


#ifdef __cplusplus
}
#endif
    

#endif /* ATSC3_MONITOR_EVENTS_ALC_H */
