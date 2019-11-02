/*
 * ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H.h
 *
 *  Created on: Oct 3, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H
#define ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H

#include "atsc3_utils.h"
#include "atsc3_packet_statistics.h"
#include "atsc3_mmtp_parser.h"

#include "atsc3_mmt_mpu_utils.h"
#include "atsc3_lls_sls_monitor_output_buffer.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"
#include "atsc3_isobmff_tools.h"
#include "atsc3_mmtp_packet_types.h"
#include "atsc3_mmtp_packet_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*atsc3_mmt_mpu_on_sequence_number_change_f)(uint16_t packet_id, uint32_t mpu_sequence_number_old, uint32_t mpu_sequence_number_new);

typedef void (*atsc3_mmt_mpu_on_sequence_mpu_metadata_present_f)(uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_mpu_metadata);

typedef void (*atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_f)(uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_movie_fragment_metadata);

#ifdef __cplusplus
}
#endif
    

#endif /* ATSC3_MMT_CONTEXT_MPU_DEPACKETIZER_H */
