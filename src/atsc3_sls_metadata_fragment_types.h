/*
 * atsc3_sls_metadata_fragment_types.h
 *
 *  Created on: Apr 2, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_SLS_METADATA_FRAGMENT_TYPES_H_
#define ATSC3_SLS_METADATA_FRAGMENT_TYPES_H_

#include "atsc3_sls_held_fragment.h"
#include "atsc3_mbms_envelope_xml.h"
#include "atsc3_route_usbd.h"
#include "atsc3_route_s_tsid.h"
#include "atsc3_route_mpd.h"
#include "atsc3_mime_multipart_related.h"
#include "atsc3_mbms_envelope_parser.h"

/***
 *
 * Table 6.16 Metadata Object Types

Name 		Values
----		-------
ALL			All metadata objects for requested service(s)
RD			All applicable ROUTE SLS metadata fragments for the requested service(s), which shall include the USBD and S-TSID, and may include the APD and MPD fragments
APD			APD for requested service(s)
MMT			All MMT metadata objects for requested service(s)
USBD		USBD for requested service(s)
STSID		S-TSID for requested service(s)
MPD			DASH MPD for requested service(s)
PAT			MMT Package Access Table for requested service(s)
MPT			MMT Package Table for requested service(s)
MPIT		MMT Media Presentation Information Table for requested service(s)
CRIT		MMT Clock Relation Information Table for requested service(s)
DCIT		MMT Device Capabilities Information Table for requested service(s)
HELD		HTML Entry pages Location Description for requested service(s) (A/337 [7])
DWD			Distribution Window Description for requested service(s) (A/337 [7])
AEI			MMT	Application Event Information for requested service(s)
EMSG		ROUTE/DASH Application Dynamic Event for requested service(s)
EVTI		MMT Application Dynamic Event for requested service(s)
 */


typedef struct atsc3_sls_metadata_fragments {
	atsc3_mime_multipart_related_instance_t* 		atsc3_mime_multipart_related_instance;
	atsc3_mbms_metadata_envelope_t* 				atsc3_mbms_metadata_envelope;
	atsc3_route_user_service_bundle_description_t*	atsc3_route_user_service_bundle_description;
	atsc3_route_s_tsid_t*							atsc3_route_s_tsid;
	atsc3_route_mpd_t*								atsc3_route_mpd;
	atsc3_sls_held_fragment_t*						atsc3_sls_held_fragment;
} atsc3_sls_metadata_fragments_t;




#endif /* ATSC3_SLS_METADATA_FRAGMENT_TYPES_H_ */
