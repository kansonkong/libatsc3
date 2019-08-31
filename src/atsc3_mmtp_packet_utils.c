/*
 * atsc3_mmtp_packet_utils.c
 *
 *  Created on: Aug 31, 2019
 *      Author: jjustman
 */

#include "atsc3_mmtp_packet_utils.h"


mmtp_asset_flow_t* mmtp_flow_find_or_create_from_udp_packet(mmtp_flow_t* mmtp_flow, udp_packet_t* udp_packet) {
    mmtp_asset_flow_t* mmtp_asset_flow = NULL;
    for(int i=0; i < mmtp_flow->mmtp_asset_flow_v.count; i++) {
        mmtp_asset_flow = mmtp_flow->mmtp_asset_flow_v.data[i];
        if(mmtp_asset_flow->dst_ip_addr == udp_packet->udp_flow.dst_ip_addr && mmtp_asset_flow->dst_port == udp_packet->udp_flow.dst_port) {
            break;
        }
    }

    if(!mmtp_asset_flow) {
        mmtp_asset_flow = mmtp_asset_flow_new();
        mmtp_asset_flow_set_flow_from_udp_packet(mmtp_asset_flow, udp_packet);
    }

    return mmtp_asset_flow;
}

mmtp_asset_t* mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow_t* mmtp_asset_flow, lls_sls_mmt_session_t* lls_sls_mmt_session) {
    mmtp_asset_t* mmtp_asset = NULL;

    for(int i=0; i < mmtp_asset_flow->mmtp_asset_v.count; i++) {
        mmtp_asset = mmtp_asset_flow->mmtp_asset_v.data[i];
        if(mmtp_asset->atsc3_service_id == lls_sls_mmt_session->service_id) {
            break;
        }
    }

    if(!mmtp_asset) {
        mmtp_asset = mmtp_asset_new();
        //todo - map this into helper method
        mmtp_asset->parent_mmtp_asset_flow = mmtp_asset_flow;
        mmtp_asset->atsc3_service_id = lls_sls_mmt_session->service_id;
    }

    return mmtp_asset;
}
//
//mmtp_asset_t* mmtp_asset_flow_find_or_create_asset_from_mmt_mpu_packet(mmtp_asset_flow_t* mmtp_asset_flow, mmtp_mpu_packet_t* mmtp_mpu_packet) {
//    mmtp_asset_t* mmtp_asset = NULL;
//
//    for(int i=0; i < mmtp_asset_flow->mmtp_asset_v.count; i++) {
//        mmtp_asset = mmtp_asset_flow->mmtp_asset_v.data[i];
//        if(mmtp_asset->mmtp_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
//            break;
//        }
//    }
//
//    if(!mmtp_asset) {
//        mmtp_asset = mmtp_asset_new();
//        //todo - map this into helper method
//        mmtp_asset->parent_mmtp_asset_flow = mmtp_asset_flow;
//        mmtp_asset->mmtp_packet_id = mmtp_mpu_packet->mmtp_packet_id;
//    }
//
//    return mmtp_asset;
//}

void mmtp_asset_add_mmtp_mpu_packet(mmtp_asset_t* mmtp_asset, mmtp_mpu_packet_t* mmtp_mpu_packet) {
//    mmtp_asset_
}


//utility methods

mmtp_packet_header_t* mmtp_packet_header_new() {
    return calloc(1, sizeof(mmtp_packet_header_t));
}

void mmtp_asset_flow_set_flow_from_udp_packet(mmtp_asset_flow_t* mmtp_asset_flow, udp_packet_t* udp_packet) {
    mmtp_asset_flow->dst_ip_addr = udp_packet->udp_flow.dst_ip_addr;
    mmtp_asset_flow->dst_port = udp_packet->udp_flow.dst_port;
}

