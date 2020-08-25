//
// Created by Jason Justman on 8/24/20.
//

#include "atsc3_lowasis_phy_android_rxdata.h"

atsc3_lowasis_phy_android_rxdata_t* atsc3_lowasis_phy_android_rxdata_duplicate_from_s_rx_data(S_RX_DATA* s_rx_data) {
    atsc3_lowasis_phy_android_rxdata_t* atsc3_lowasis_phy_android_rxdata = (atsc3_lowasis_phy_android_rxdata_t*)calloc(1, sizeof(atsc3_lowasis_phy_android_rxdata_t));

    atsc3_lowasis_phy_android_rxdata->eType = s_rx_data->eType;
    atsc3_lowasis_phy_android_rxdata->payload = block_Duplicate_from_ptr(s_rx_data->ptr, s_rx_data->nLength);
    block_Rewind(atsc3_lowasis_phy_android_rxdata->payload);

    if (s_rx_data->eType == eAT3_RXDTYPE_IP) {
        atsc3_lowasis_phy_android_rxdata->pInfo = (S_AT3DRV_RXDINFO_IP*) calloc(1, sizeof(S_AT3DRV_RXDINFO_IP));
        memcpy(atsc3_lowasis_phy_android_rxdata->pInfo, s_rx_data->pInfo, sizeof(S_AT3DRV_RXDINFO_IP));
    } else if(s_rx_data->eType == eAT3_RXDTYPE_IP_LMT) {
        atsc3_lowasis_phy_android_rxdata->pInfo = (S_AT3DRV_RXDINFO_LMT*) calloc(1, sizeof(S_AT3DRV_RXDINFO_LMT));
        memcpy(atsc3_lowasis_phy_android_rxdata->pInfo, s_rx_data->pInfo, sizeof(S_AT3DRV_RXDINFO_LMT));
    } else if(s_rx_data->eType == eAT3_RXDTYPE_ALP) {
        atsc3_lowasis_phy_android_rxdata->pInfo = (S_AT3DRV_RXDINFO_ALP*) calloc(1, sizeof(S_AT3DRV_RXDINFO_ALP));
        memcpy(atsc3_lowasis_phy_android_rxdata->pInfo, s_rx_data->pInfo, sizeof(S_AT3DRV_RXDINFO_ALP));
    } else if(s_rx_data->eType == eAT3_RXDTYPE_BBPCTR) {
        atsc3_lowasis_phy_android_rxdata->pInfo = (S_AT3DRV_RXDINFO_BBPCTR*) calloc(1, sizeof(S_AT3DRV_RXDINFO_BBPCTR));
        memcpy(atsc3_lowasis_phy_android_rxdata->pInfo, s_rx_data->pInfo, sizeof(S_AT3DRV_RXDINFO_BBPCTR));
    }

    atsc3_lowasis_phy_android_rxdata->ulTick = AT3_GetMsTime();

    return atsc3_lowasis_phy_android_rxdata;
}

void atsc3_lowasis_phy_android_rxdata_free(atsc3_lowasis_phy_android_rxdata_t** atsc3_lowasis_phy_android_rxdata_p) {
    if(atsc3_lowasis_phy_android_rxdata_p) {
        atsc3_lowasis_phy_android_rxdata_t* atsc3_lowasis_phy_android_rxdata = *atsc3_lowasis_phy_android_rxdata_p;
        if(atsc3_lowasis_phy_android_rxdata) {

            if(atsc3_lowasis_phy_android_rxdata->payload) {
                block_Destroy(&atsc3_lowasis_phy_android_rxdata->payload);
            }

            if(atsc3_lowasis_phy_android_rxdata->pInfo) {
                free(atsc3_lowasis_phy_android_rxdata->pInfo);
                atsc3_lowasis_phy_android_rxdata->pInfo = nullptr;
            }

            free((void*)atsc3_lowasis_phy_android_rxdata);

            atsc3_lowasis_phy_android_rxdata = nullptr;
        }
        *atsc3_lowasis_phy_android_rxdata_p = nullptr;
    }
}


