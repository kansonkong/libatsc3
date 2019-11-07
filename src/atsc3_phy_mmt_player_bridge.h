//
// Created by Jason Justman on 2019-09-27.
//

#ifndef AT3DRV_ANDROID_2_26_190826_ATSC3_PHY_MMT_PLAYER_BRIDGE_H
#define AT3DRV_ANDROID_2_26_190826_ATSC3_PHY_MMT_PLAYER_BRIDGE_H

#include "../jni/At3DrvIntf.h"
#include "atsc3_utils.h"
#include "atsc3_lls_types.h"


void atsc3_phy_mmt_player_bridge_init(At3DrvIntf* At3DrvIntf_ptr);

void atsc3_phy_mmt_player_bridge_process_packet_phy(block_t* packet);

//change SLT service and wire up a single montior
atsc3_lls_slt_service_t* atsc3_phy_mmt_player_bridge_set_single_monitor_a331_service_id(int service_id);

//add additional alc monitor service_id's for supplimentary MMT or ROUTE flows
atsc3_lls_slt_service_t* atsc3_phy_mmt_player_bridge_add_monitor_a331_service_id(int service_id);
atsc3_lls_slt_service_t* atsc3_phy_mmt_player_bridge_remove_monitor_a331_service_id(int service_id);
//TODO: wire up ROUTE/ALC and MBMS/FDT event callback hooks for close_object emission (including delivery metrics w.r.t ALC DU loss)

lls_sls_alc_monitor_t* atsc3_lls_sls_alc_monitor_get_from_service_id(int service_id);

//ALC/ROUTE: use case: parse out the atsc3_mbms_metadata_envelope to get MPD from metadataURI
atsc3_sls_metadata_fragments_t* atsc3_slt_alc_get_sls_metadata_fragments_from_monitor_service_id(int service_id);

//ALC/ROUTE: use case: get the app-based/esg service corresponding efdt atsc3_fdt_file content_location value
atsc3_route_s_tsid_t* atsc3_slt_alc_get_sls_route_s_tsid_from_monitor_service_id(int service_id);

string atsc3_ndk_cache_temp_folder_path_get();
string atsc3_route_service_context_temp_folder_name(int service_id);

#endif //AT3DRV_ANDROID_2_26_190826_ATSC3_PHY_MMT_PLAYER_BRIDGE_H
