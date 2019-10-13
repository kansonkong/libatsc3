//
// Created by Jason Justman on 2019-09-27.
//

#ifndef AT3DRV_ANDROID_2_26_190826_ATSC3_PHY_MMT_PLAYER_BRIDGE_H
#define AT3DRV_ANDROID_2_26_190826_ATSC3_PHY_MMT_PLAYER_BRIDGE_H

#include "../jni/At3DrvIntf.h"
#include "atsc3_utils.h"


void atsc3_phy_mmt_player_bridge_init(At3DrvIntf* At3DrvIntf_ptr);

void atsc3_phy_mmt_player_bridge_process_packet_phy(block_t* packet);

#endif //AT3DRV_ANDROID_2_26_190826_ATSC3_PHY_MMT_PLAYER_BRIDGE_H
