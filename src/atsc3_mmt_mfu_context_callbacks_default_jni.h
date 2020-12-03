//
// Created by Jason Justman on 12/1/20.
//
#include <stddef.h>
#include <stdlib.h>

#ifndef ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_H_
#define ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_H_

#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"
#include "atsc3_mmt_context_mfu_depacketizer.h"
#include "atsc3_hevc_nal_extractor.h"

#include "application/IAtsc3NdkMediaMMTBridge.h"

#ifdef __cplusplus
extern "C" {
#endif

//wire up default JNI callbacks
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_callbacks_default_jni_new();

//NDK/JNI mediaMMTBridge reference pin

void atsc3_ndk_media_mmt_bridge_init(IAtsc3NdkMediaMMTBridge* atsc3NdkMediaMMTBridge);
IAtsc3NdkMediaMMTBridge* atsc3_ndk_media_mmt_bridge_get_instance();

//mmt mfu context management
void atsc3_ndk_media_mmt_bridge_reset_context();

void atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_mpu_metadata);

void atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
void atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
void atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);


void atsc3_mmt_mpu_mfu_on_sample_complete_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_rebuilt);
void atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);
void atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);
void atsc3_mmt_mpu_mfu_on_sample_missing_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number);

void atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_movie_fragment_metadata);



#define _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_INFO(...)  if(_ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG(...) if(_ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_TRACE(...) if(_ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#ifdef __cplusplus
}
#endif

#endif //ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_H_
