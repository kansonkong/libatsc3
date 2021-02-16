
#include <stdio.h>

using namespace std;

#ifndef LIBATSC3_MMTEXTRACTOR_H
#define LIBATSC3_MMTEXTRACTOR_H

#include "atsc3_utils.h"
#include "atsc3_udp.h"
#include "atsc3_mmtp_packet_types.h"
#include "atsc3_mmtp_parser.h"

//TODO: remove this
#include "atsc3_lls_types.h"

class MMTExtractor {
    public:
    MMTExtractor();

    void atsc3_core_service_bridge_process_mmt_packet(block_t* packet);
    int8_t atsc3_core_service_bridge_process_mmt_udp_packet(udp_packet_t* udp_packet, mmtp_asset_t* mmtp_asset, lls_sls_mmt_session_t* lls_sls_mmt_session);

    private:
    atsc3_lls_slt_service_t* atsc3_lls_slt_service;

    //jjustman-2019-10-03 - context event callbacks...
    lls_slt_monitor_t* lls_slt_monitor = NULL;

    //mmtp/sls flow management
    lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;
    atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = NULL;
};

#endif //LIBATSC3_MMTEXTRACTOR_H
