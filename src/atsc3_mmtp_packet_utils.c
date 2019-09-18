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
        mmtp_flow_add_mmtp_asset_flow(mmtp_flow, mmtp_asset_flow);
    }

    return mmtp_asset_flow;
}

mmtp_asset_t* mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow_t* mmtp_asset_flow, lls_sls_mmt_session_t* lls_sls_mmt_session) {
    mmtp_asset_t* mmtp_asset = NULL;

    for(int i=0; i < mmtp_asset_flow->mmtp_asset_v.count; i++) {
        mmtp_asset = mmtp_asset_flow->mmtp_asset_v.data[i];
        if(mmtp_asset->atsc3_service_id == lls_sls_mmt_session->service_id) {
            break;
        } else {
            mmtp_asset = NULL;
        }
    }

    if(!mmtp_asset) {
        mmtp_asset = mmtp_asset_new();
        //todo - map this into helper method
        mmtp_asset->parent_mmtp_asset_flow = mmtp_asset_flow;
        mmtp_asset->atsc3_service_id = lls_sls_mmt_session->service_id;
        
        mmtp_asset_flow_add_mmtp_asset(mmtp_asset_flow, mmtp_asset);
    }

    return mmtp_asset;
}

mmtp_packet_id_packets_container_t* mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset_t* mmtp_asset, mmtp_mpu_packet_t* mmtp_mpu_packet) {
    mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container = NULL;

    for(int i=0; i < mmtp_asset->mmtp_packet_id_packets_container_v.count; i++) {
        mmtp_packet_id_packets_container = mmtp_asset->mmtp_packet_id_packets_container_v.data[i];
        if(mmtp_packet_id_packets_container->packet_id == mmtp_mpu_packet->mmtp_packet_id) {
            break;
        } else {
            mmtp_packet_id_packets_container = NULL;
        }
    }

    if(!mmtp_packet_id_packets_container) {
        mmtp_packet_id_packets_container = mmtp_packet_id_packets_container_new();
        mmtp_packet_id_packets_container->packet_id = mmtp_mpu_packet->mmtp_packet_id;
        
        mmtp_asset_add_mmtp_packet_id_packets_container(mmtp_asset, mmtp_packet_id_packets_container);
    }

    return mmtp_packet_id_packets_container;
}


mpu_sequence_number_mmtp_mpu_packet_collection_t* mmtp_packet_id_packets_container_find_or_create_mpu_sequence_number_mmtp_mpu_packet_collection_from_mmt_mpu_packet(mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container, mmtp_mpu_packet_t* mmtp_mpu_packet) {
    mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = NULL;
    
    for(int i=0; i < mmtp_packet_id_packets_container->mpu_sequence_number_mmtp_mpu_packet_collection_v.count; i++) {
        mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container->mpu_sequence_number_mmtp_mpu_packet_collection_v.data[i];
        if(mpu_sequence_number_mmtp_mpu_packet_collection->mpu_sequence_number == mmtp_mpu_packet->mpu_sequence_number) {
            break;
        } else {
            mpu_sequence_number_mmtp_mpu_packet_collection = NULL;
        }
    }
    
    if(!mpu_sequence_number_mmtp_mpu_packet_collection) {
        mpu_sequence_number_mmtp_mpu_packet_collection = mpu_sequence_number_mmtp_mpu_packet_collection_new();
        mpu_sequence_number_mmtp_mpu_packet_collection->mpu_sequence_number = mmtp_mpu_packet->mpu_sequence_number;
        mpu_sequence_number_mmtp_mpu_packet_collection->packet_id = mmtp_mpu_packet->mmtp_packet_id;
        
        mmtp_packet_id_packets_container_add_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
    }
    
    return mpu_sequence_number_mmtp_mpu_packet_collection;
}

mpu_sequence_number_mmtp_mpu_packet_collection_t* mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container, uint32_t mpu_sequence_number) {
    mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = NULL;
    
    for(int i=0; i < mmtp_packet_id_packets_container->mpu_sequence_number_mmtp_mpu_packet_collection_v.count; i++) {
        mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container->mpu_sequence_number_mmtp_mpu_packet_collection_v.data[i];
        if(mpu_sequence_number_mmtp_mpu_packet_collection->mpu_sequence_number == mpu_sequence_number) {
            break;
        } else {
            mpu_sequence_number_mmtp_mpu_packet_collection = NULL;
        }
    }
    
    return mpu_sequence_number_mmtp_mpu_packet_collection;
}


void mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection_to_remove) {
    mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = NULL;
    bool removed_mmtp_mpu_packet_collection_entry = false;
    int i=0;
    
    for(; i < mmtp_packet_id_packets_container->mpu_sequence_number_mmtp_mpu_packet_collection_v.count; i++) {
        mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container->mpu_sequence_number_mmtp_mpu_packet_collection_v.data[i];
        
        if(mpu_sequence_number_mmtp_mpu_packet_collection == mpu_sequence_number_mmtp_mpu_packet_collection_to_remove) {
            mpu_sequence_number_mmtp_mpu_packet_collection_free(&mpu_sequence_number_mmtp_mpu_packet_collection);
            removed_mmtp_mpu_packet_collection_entry = true;
            break;
        }
    }
    
    if(removed_mmtp_mpu_packet_collection_entry) {
        mmtp_packet_id_packets_container->mpu_sequence_number_mmtp_mpu_packet_collection_v.count--;
        for(; i < mmtp_packet_id_packets_container->mpu_sequence_number_mmtp_mpu_packet_collection_v.count; i++) {
            mmtp_packet_id_packets_container->mpu_sequence_number_mmtp_mpu_packet_collection_v.data[i] = mmtp_packet_id_packets_container->mpu_sequence_number_mmtp_mpu_packet_collection_v.data[i+1];
        }
        mmtp_packet_id_packets_container->mpu_sequence_number_mmtp_mpu_packet_collection_v.data[i] = NULL;
    }
}




//utility methods

mmtp_packet_header_t* mmtp_packet_header_new() {
    return calloc(1, sizeof(mmtp_packet_header_t));
}

void mmtp_asset_flow_set_flow_from_udp_packet(mmtp_asset_flow_t* mmtp_asset_flow, udp_packet_t* udp_packet) {
    mmtp_asset_flow->dst_ip_addr = udp_packet->udp_flow.dst_ip_addr;
    mmtp_asset_flow->dst_port = udp_packet->udp_flow.dst_port;
}

