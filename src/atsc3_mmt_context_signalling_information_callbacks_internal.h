//
// Created by Jason Justman on 12/1/20.
//

#include <stddef.h>
#include <stdlib.h>

#ifndef ATSC3_MMT_SIGNALLING_INFORMATION_CONTEXT_CALLBACKS_INTERNAL_H_
#define ATSC3_MMT_SIGNALLING_INFORMATION_CONTEXT_CALLBACKS_INTERNAL_H_

#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"
#include "atsc3_mmt_context_mfu_depacketizer.h"

#ifdef __cplusplus
extern "C" {
#endif

void atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_callback_internal(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);

void atsc3_mmt_signalling_information_on_audio_essence_packet_id_callback_internal(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t audio_packet_id, mp_table_asset_row_t* mp_table_asset_row);
void atsc3_mmt_signalling_information_on_video_essence_packet_id_callback_internal(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t video_packet_id, mp_table_asset_row_t* mp_table_asset_row);
void atsc3_mmt_signalling_information_on_stpp_essence_packet_id_callback_internal( atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t stpp_packet_id,  mp_table_asset_row_t* mp_table_asset_row);

#ifdef __cplusplus
}
#endif


#endif //ATSC3_MMT_SIGNALLING_INFORMATION_CONTEXT_CALLBACKS_INTERNAL_H_
