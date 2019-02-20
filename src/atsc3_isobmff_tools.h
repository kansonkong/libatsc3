/*
 * atsc3_isobmff_tools.h
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 */

#include "atsc3_utils.h"
#include "atsc3_listener_udp.h"
#include "atsc3_mmt_mpu_utils.h"


#ifndef ATSC3_ISOBMFF_TOOLS_H_
#define ATSC3_ISOBMFF_TOOLS_H_

extern int _ISOBMFF_TOOLS_DEBUG_ENABLED;



block_t* atsc3_isobmff_build_mpu_metadata_ftyp_box(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector);






#define __ISOBMFF_TOOLS_ERROR(...)   printf("%s:%d:ERROR :",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\n");
#define __ISOBMFF_TOOLS_WARN(...)    printf("%s:%d:WARN: ",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\n");
#define __ISOBMFF_TOOLS_INFO(...)    printf("%s:%d: ",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\n");
#define __ISOBMFF_TOOLS_DEBUG(...)   if(_ISOBMFF_TOOLS_DEBUG_ENABLED) {printf("%s:%d:DEBUG: ",__FILE__,__LINE__);printf(__VA_ARGS__);printf("\n"); }


#endif /* ATSC3_ISOBMFF_TOOLS_H_ */
