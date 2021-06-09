//
// Created by Jason Justman on 12/24/20.
//
#include <string>
#include <vector>
using namespace std;

#ifndef LIBATSC3_IATSC3NDKPHYCLIENTRFMETRICS_H
#define LIBATSC3_IATSC3NDKPHYCLIENTRFMETRICS_H

#include <atsc3_utils.h>

typedef struct atsc3_ndk_phy_client_rf_plp_metrics {
    uint8_t     plp_id;

    uint32_t    modcod_valid; //i.e. is PLP[n] ALP data able to be demod?  e.g. SL_DEMOD_LOCK_STATUS_MASK_BB_PLP0_LOCK

    uint8_t     plp_fec_type;
    uint8_t     plp_mod;
    uint8_t     plp_cod;

    uint32_t    ber_pre_ldpc; //BER x 1e7
    uint32_t    ber_pre_bch; //BER x 1e9
    uint32_t    fer_post_bch; //FER x 1e6

    int32_t     snr1000;

} atsc3_ndk_phy_client_rf_plp_metrics_t;


typedef struct atsc3_ndk_phy_client_rf_metrics {
    int32_t     tuner_lock;
    int32_t     demod_lock;

    uint8_t     plp_lock_any;
    uint8_t     plp_lock_all;
    uint8_t     plp_lock_by_setplp_index;

    int32_t     cpu_status;

    int32_t     rssi;
    int32_t     rfLevel1000;

    int32_t     snr1000_global;
    int32_t     snr1000_l1b;
    int32_t     snr1000_l1d;

    uint8_t     bootstrap_system_bw;
    uint8_t     bootstrap_ea_wakeup;

    atsc3_ndk_phy_client_rf_plp_metrics_t  phy_client_rf_plp_metrics[4];

} atsc3_ndk_phy_client_rf_metrics_t;

//jjustman-2020-12-24 - shortcut hack...

#endif //LIBATSC3_IATSC3NDKPHYCLIENTRFMETRICS_H
