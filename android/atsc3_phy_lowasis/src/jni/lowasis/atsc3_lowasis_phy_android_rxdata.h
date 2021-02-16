//
// Created by Jason Justman on 8/24/20.
//
#include <stdlib.h>
#include <string.h>

#include <at3drv_api.h>
#include <at3_atscstd.h>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3_LOWASIS_PHY_ANDROID_RXDATA_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3_LOWASIS_PHY_ANDROID_RXDATA_H

#include <Atsc3LoggingUtils.h>
#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>

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


#define _LOWASIS_PHY_RXDATA_ANDROID_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _LOWASIS_PHY_RXDATA_ANDROID_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _LOWASIS_PHY_RXDATA_ANDROID_INFO(...)   	if(_ATSC3_LOWASIS_PHY_RXDATA_ANDROID_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _LOWASIS_PHY_RXDATA_ANDROID_DEBUG(...)   	if(_ATSC3_LOWASIS_PHY_RXDATA_ANDROID_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _LOWASIS_PHY_RXDATA_ANDROID_TRACE(...)   	if(_ATSC3_LOWASIS_PHY_RXDATA_ANDROID_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_ATSC3_LOWASIS_PHY_ANDROID_RXDATA_H
