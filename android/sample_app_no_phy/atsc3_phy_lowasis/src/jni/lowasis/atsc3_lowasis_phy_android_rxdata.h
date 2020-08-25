//
// Created by Jason Justman on 8/24/20.
//
#include <stdlib.h>
#include <string.h>

#include <at3drv_api.h>
#include <at3_atscstd.h>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3_LOWASIS_PHY_ANDROID_RXDATA_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3_LOWASIS_PHY_ANDROID_RXDATA_H

#include <atsc3_utils.h>

//mirror of S_RX_DATA but for internal refcounting
typedef struct lowasis_phy_android_rxdata
{
    uint32_t            ulTick;
    E_AT3DRV_RXDTYPE    eType;
    block_t*            payload;
    void*               pInfo;  // additional info. type depends on eType.
} atsc3_lowasis_phy_android_rxdata_t;


atsc3_lowasis_phy_android_rxdata_t* atsc3_lowasis_phy_android_rxdata_duplicate_from_s_rx_data(S_RX_DATA* s_rx_data);
void atsc3_lowasis_phy_android_rxdata_free(atsc3_lowasis_phy_android_rxdata_t** atsc3_lowasis_phy_android_rxdata_p);

#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3_LOWASIS_PHY_ANDROID_RXDATA_H
