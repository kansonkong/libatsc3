//
// Created by Jason Justman on 8/24/20.
//

#include "atsc3_lowasis_phy_android_rxdata.h"

int _ATSC3_LOWASIS_PHY_RXDATA_ANDROID_INFO_ENABLED = 0;
int _ATSC3_LOWASIS_PHY_RXDATA_ANDROID_DEBUG_ENABLED = 0;
int _ATSC3_LOWASIS_PHY_RXDATA_ANDROID_TRACE_ENABLED = 0;


atsc3_lowasis_phy_android_rxdata_t* atsc3_lowasis_phy_android_rxdata_duplicate_from_s_rx_data(S_RX_DATA* s_rx_data) {
    atsc3_lowasis_phy_android_rxdata_t* atsc3_lowasis_phy_android_rxdata = (atsc3_lowasis_phy_android_rxdata_t*)calloc(1, sizeof(atsc3_lowasis_phy_android_rxdata_t));

    atsc3_lowasis_phy_android_rxdata->eType = s_rx_data->eType;
    atsc3_lowasis_phy_android_rxdata->payload = block_Duplicate_from_ptr(s_rx_data->ptr, s_rx_data->nLength);

    _LOWASIS_PHY_RXDATA_ANDROID_TRACE("atsc3_lowasis_phy_android_rxdata_duplicate_from_s_rx_data: atsc3_lowasis_phy_android_rxdata->eType: %d, s_rx_data->ptr: %p, s_rx_data->nLength: %d\n",
                                      atsc3_lowasis_phy_android_rxdata->eType,
                                      s_rx_data->ptr, s_rx_data->nLength);

    block_Rewind(atsc3_lowasis_phy_android_rxdata->payload);

    if (s_rx_data->eType == eAT3_RXDTYPE_IP) {
        atsc3_lowasis_phy_android_rxdata->pInfo = (S_AT3DRV_RXDINFO_IP*) calloc(1, sizeof(S_AT3DRV_RXDINFO_IP));
        memcpy(atsc3_lowasis_phy_android_rxdata->pInfo, s_rx_data->pInfo, sizeof(S_AT3DRV_RXDINFO_IP));
    } else if(s_rx_data->eType == eAT3_RXDTYPE_IP_LMT) {
        S_AT3DRV_RXDINFO_LMT* s_rx_data_lmt_info = (S_AT3DRV_RXDINFO_LMT*)s_rx_data->pInfo;

        atsc3_lowasis_phy_android_rxdata->pInfo = (S_AT3DRV_RXDINFO_LMT*) calloc(1, sizeof(S_AT3DRV_RXDINFO_LMT));
        memcpy(atsc3_lowasis_phy_android_rxdata->pInfo, s_rx_data->pInfo, sizeof(S_AT3DRV_RXDINFO_LMT));

        S_AT3DRV_RXDINFO_LMT* to_update_lmt_info = (S_AT3DRV_RXDINFO_LMT*)atsc3_lowasis_phy_android_rxdata->pInfo;
        to_update_lmt_info->lmt = (S_AT3_LMT*)calloc(1, sizeof(S_AT3_LMT));
        /*
         * jjustman-2020-09-30: testing with 8k fft+ (16qam and 64qam) plps, and 16k fft + 256qam results in occassional in which s_rx_data_lmt_info->lmt may be null
         *
            2020-09-30 15:35:54.035 9541-9575/org.ngbp.libatsc3 E/cutl: _CheckOverrun: [31;1m!! bitBuffer Overrun! required bits 16, bitOffset 111:0, buf 111, 0x9200006ce6e06007[0m
            2020-09-30 15:35:54.036 9541-9575/org.ngbp.libatsc3 E/alp_: Parse: !! bitbuf overrun, at loop <plp 2/3: mc 3/116>, overrun bits 40
            2020-09-30 15:35:54.036 9541-9575/org.ngbp.libatsc3 E/dev_: OnRxLmt: !! parselmt err
         */
        if(s_rx_data_lmt_info->lmt) {
            to_update_lmt_info->lmt->num_mc = s_rx_data_lmt_info->lmt->num_mc;
            to_update_lmt_info->lmt->mc = (S_AT3_LMT::S_LMT_MC*) calloc(to_update_lmt_info->lmt->num_mc, sizeof(S_AT3_LMT::S_LMT_MC));
            for (int i = 0; i < s_rx_data_lmt_info->lmt->num_mc; i++) {
                memcpy(&to_update_lmt_info->lmt->mc[i], &s_rx_data_lmt_info->lmt->mc[i], sizeof(S_AT3_LMT::S_LMT_MC));
            }
        } else {
            _LOWASIS_PHY_RXDATA_ANDROID_ERROR("atsc3_lowasis_phy_android_rxdata_duplicate_from_s_rx_data: s_rx_data_lmt_info->lmt is NULL, discarding return data!");
            atsc3_lowasis_phy_android_rxdata_free(&atsc3_lowasis_phy_android_rxdata);
            goto error;
        }

    } else if(s_rx_data->eType == eAT3_RXDTYPE_ALP) {
        atsc3_lowasis_phy_android_rxdata->pInfo = (S_AT3DRV_RXDINFO_ALP*) calloc(1, sizeof(S_AT3DRV_RXDINFO_ALP));
        memcpy(atsc3_lowasis_phy_android_rxdata->pInfo, s_rx_data->pInfo, sizeof(S_AT3DRV_RXDINFO_ALP));
    } else if(s_rx_data->eType == eAT3_RXDTYPE_BBPCTR) {
        atsc3_lowasis_phy_android_rxdata->pInfo = (S_AT3DRV_RXDINFO_BBPCTR*) calloc(1, sizeof(S_AT3DRV_RXDINFO_BBPCTR));
        memcpy(atsc3_lowasis_phy_android_rxdata->pInfo, s_rx_data->pInfo, sizeof(S_AT3DRV_RXDINFO_BBPCTR));
    }

    atsc3_lowasis_phy_android_rxdata->ulTick = AT3_GetMsTime();

    return atsc3_lowasis_phy_android_rxdata;

error:
    return NULL;
}

void atsc3_lowasis_phy_android_rxdata_free(atsc3_lowasis_phy_android_rxdata_t** atsc3_lowasis_phy_android_rxdata_p) {
    if(atsc3_lowasis_phy_android_rxdata_p) {
        atsc3_lowasis_phy_android_rxdata_t* atsc3_lowasis_phy_android_rxdata = *atsc3_lowasis_phy_android_rxdata_p;
        if(atsc3_lowasis_phy_android_rxdata) {

            if(atsc3_lowasis_phy_android_rxdata->payload) {
                block_Destroy(&atsc3_lowasis_phy_android_rxdata->payload);
            }

            if(atsc3_lowasis_phy_android_rxdata->pInfo) {
                if(atsc3_lowasis_phy_android_rxdata->eType == eAT3_RXDTYPE_IP_LMT) {
                    S_AT3DRV_RXDINFO_LMT* to_update_lmt_info = (S_AT3DRV_RXDINFO_LMT*)atsc3_lowasis_phy_android_rxdata->pInfo;
                    if(to_update_lmt_info->lmt) {
                        if(to_update_lmt_info->lmt->mc) {
                            free(to_update_lmt_info->lmt->mc);
                            to_update_lmt_info->lmt->mc = nullptr;
                            to_update_lmt_info->lmt->num_mc = 0;
                        }

                        free(to_update_lmt_info->lmt);
                        to_update_lmt_info->lmt = nullptr;
                    }
                }
                free(atsc3_lowasis_phy_android_rxdata->pInfo);
                atsc3_lowasis_phy_android_rxdata->pInfo = nullptr;
            }

            free((void*)atsc3_lowasis_phy_android_rxdata);

            atsc3_lowasis_phy_android_rxdata = nullptr;
        }
        *atsc3_lowasis_phy_android_rxdata_p = nullptr;
    }
}


