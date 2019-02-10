/*
 * atsc3_mmt_mpu_utils.h
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */
#include "atsc3_mmtp_types.h"

#ifndef ATSC3_MMT_MPU_UTILS_H_
#define ATSC3_MMT_MPU_UTILS_H_

extern int _MMT_MPU_DEBUG_ENABLED;

void dump_mpu(mmtp_payload_fragments_union_t* mmtp_payload);
void mpu_dump_reconstitued(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload);



#define __MMT_MPU_ERROR(...)   printf("%s:%d:ERROR :","listener",__LINE__);printf(__VA_ARGS__);printf("\n");
#define __MMT_MPU_WARN(...)    printf("%s:%d:WARN: ","listener",__LINE__);printf(__VA_ARGS__);printf("\n");
#define __MMT_MPU_INFO(...)    printf("%s:%d: ","listener",__LINE__);printf(__VA_ARGS__);printf("\n");

#define __MMT_MPU_DEBUG(...)   if(_MMT_MPU_DEBUG_ENABLED) {printf("%s:%d:DEBUG: ","listener",__LINE__);printf(__VA_ARGS__);printf("\n"); }

#endif /* ATSC3_MMT_MPU_UTILS_H_ */
