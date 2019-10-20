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

//todo:
//atsc3_lls_slt_service_t* atsc3_phy_mmt_player_bridge_add_monitor_a331_service_id(int service_id);
//atsc3_lls_slt_service_t* atsc3_phy_mmt_player_bridge_remove_monitor_a331_service_id(int service_id);

#endif //AT3DRV_ANDROID_2_26_190826_ATSC3_PHY_MMT_PLAYER_BRIDGE_H
