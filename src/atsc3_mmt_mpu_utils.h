/*
 * atsc3_mmt_mpu_utils.h
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */
#include "atsc3_mmtp_types.h"
#include "atsc3_mmtp_parser.h"
#include "atsc3_player_ffplay.h"

#ifndef ATSC3_MMT_MPU_UTILS_H_
#define ATSC3_MMT_MPU_UTILS_H_

extern int _MMT_MPU_DEBUG_ENABLED;

void mpu_dump_header(mmtp_payload_fragments_union_t* mmtp_payload);
void mpu_dump_flow(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload);
void mpu_dump_reconstitued(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload);
void mpu_push_to_output_buffer(pipe_ffplay_buffer_t* ffplay_buffer, mmtp_payload_fragments_union_t* mmtp_payload);

void mpu_push_to_output_buffer_no_locking(pipe_ffplay_buffer_t* ffplay_buffer, mmtp_payload_fragments_union_t* mmtp_payload);

#define __MMT_MPU_ERROR(...)   printf("%s:%d:ERROR :",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\n");
#define __MMT_MPU_WARN(...)    printf("%s:%d:WARN: ",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\n");
#define __MMT_MPU_INFO(...)    printf("%s:%d: ",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\n");

#define __MMT_MPU_DEBUG(...)   if(_MMT_MPU_DEBUG_ENABLED) {printf("%s:%d:DEBUG: ",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\n"); }

#endif /* ATSC3_MMT_MPU_UTILS_H_ */
