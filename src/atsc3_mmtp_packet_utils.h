/*
 * atsc3_mmtp_packet_utils.h
 *
 *  Created on: Aug 31, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMTP_PACKET_UTILS_H_
#define ATSC3_MMTP_PACKET_UTILS_H_

#if defined (__cplusplus)
extern "C" {
#endif

#include "atsc3_mmtp_packet_types.h"
#include "atsc3_lls_types.h"



/**
    collection management methods here
**/

mmtp_asset_flow_t*                                                mmtp_flow_find_or_create_from_udp_packet(mmtp_flow_t* mmtp_flow, udp_packet_t* udp_packet);
mmtp_asset_t*                                                     mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow_t* mmtp_asset_flow, lls_sls_mmt_session_t* lls_sls_mmt_session);
mmtp_packet_id_packets_container_t*                               mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset_t* mmtp_asset, mmtp_mpu_packet_t* mmtp_mpu_packet);

mpu_sequence_number_mmtp_mpu_packet_collection_t*                 mmtp_packet_id_packets_container_find_or_create_mpu_sequence_number_mmtp_mpu_packet_collection_from_mmt_mpu_packet(mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container, mmtp_mpu_packet_t* mmtp_mpu_packet);
mpu_sequence_number_mmtp_mpu_packet_collection_t*                 mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container, uint32_t mpu_sequence_number);
   
//mmtp_asset_t*       mmtp_asset_flow_find_or_create_asset_from_mmt_mpu_packet(mmtp_asset_flow_t* mmtp_asset_flow, mmtp_mpu_packet_t* mmtp_mpu_packet);

void mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection_to_remove);

void mmtp_asset_flow_set_flow_from_udp_packet(mmtp_asset_flow_t* mmtp_asset_flow, udp_packet_t* udp_packet);


#define __MMTP_UTILS_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __MMTP_UTILS_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMTP_UTILS_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define __MMTP_UTILS_DEBUG(...)   if(_MMTP_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __MMTP_UTILS_TRACE(...)   if(_MMTP_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#ifdef __cplusplus
}
#endif
#endif /* ATSC3_MMTP_PACKET_UTILS_H_ */
