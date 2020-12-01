/*
 * atsc3_hevc_nal_extractor.h
 *
 *  Created on: Oct 5, 2019
 *      Author: jjustman
 *
 *  for extraction of NAL (sps/pps/vps) from ISOBMFF MPU metadata fragments
 *	for use with HEVC decoder initialization
 *
 *	Specification Reference: ISO 14496-15:2019 - https://www.iso.org/standard/74429.html
 *
 *	Consumer Implementation Use Case: Android MediaCodec: Codec Specific Data (HEVC) - https://developer.android.com/reference/android/media/MediaCodec
 *
 *
 *  It is recommended that the arrays be in the order VPS, SPS, PPS, prefix SEI, suffix SEI.
 *
 *
 *  TODO:  add hev1 descriptor data for w/h
 *
 *   [hev1] size=8+339
              data_reference_index = 1
              width = 3840
              height = 2160
              compressor =
              [hvcC] size=8+253
                Configuration Version = 1
                Profile Space = 0
                Profile = Main 10
                Tier = 0
                Profile Compatibility = 20000000
                Constraint = b00000000000
                Level = 153
                Min Spatial Segmentation = 0
                Parallelism Type = 0
                Chroma Format = 1
                Chroma Depth = 10
                Luma Depth = 10
                Average Frame Rate = 0
                Constant Frame Rate = 0
                Number Of Temporal Layers = 3
                Temporal Id Nested = 0
                NALU Length Size = 4
 */

#include <stddef.h>

#ifndef ATSC3_HEVC_NAL_EXTRACTOR_H_
#define ATSC3_HEVC_NAL_EXTRACTOR_H_

#include "atsc3_logging_externs.h"
#include "atsc3_video_decoder_configuration_record.h"
#include "atsc3_isobmff_box_parser_tools.h"

#ifdef __cplusplus
extern "C" {
#endif

atsc3_video_decoder_configuration_record_t* atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t(block_t*);

block_t* atsc3_hevc_extract_extradata_nals_combined_ffmpegImpl(block_t* hvcc_box);
block_t* atsc3_hevc_extract_mp4toannexb_filter_ffmpegImpl(block_t* sample, block_t* last_extradata_NAL_parsed);

void atsc3_hevc_decoder_configuration_record_dump(hevc_decoder_configuration_record_t* hevc_decoder_configuration_record);
void atsc3_hevc_nals_record_dump(const char* label, block_t* block);

void atsc3_avc1_decoder_configuration_record_dump(avc1_decoder_configuration_record_t* avc1_decoder_configuration_record);

void atsc3_init_parse_tkhd_for_width_height(atsc3_video_decoder_configuration_record_t* atsc3_video_decoder_configuration_record, uint8_t* tkhd_ptr_start, uint32_t init_buff_remaining);
void atsc3_init_parse_HEVCConfigurationBox_for_width_height(atsc3_video_decoder_configuration_record_t* atsc3_video_decoder_configuration_record, uint8_t* configurationBox_ptr_start, uint32_t init_buff_remaining);


/**
 
 jjustman-2019-10-12: do not use these
    vps/sps/pps parsing by hand is incredibly complex,
    use ffmpegImpl or wait until we are linked with libavcodec for NAL parsing
 */

block_t* atsc3_hevc_decoder_configuration_record_get_nals_vps_combined(hevc_decoder_configuration_record_t* hevc_decoder_configuration_record);
block_t* atsc3_hevc_decoder_configuration_record_get_nals_sps_combined(hevc_decoder_configuration_record_t* hevc_decoder_configuration_record);
block_t* atsc3_hevc_decoder_configuration_record_get_nals_pps_combined(hevc_decoder_configuration_record_t* hevc_decoder_configuration_record);

block_t* atsc3_hevc_decoder_configuration_record_get_nals_vps_combined_optional_start_code(hevc_decoder_configuration_record_t* hevc_decoder_configuration_record, bool include_nal_start_code);
block_t* atsc3_hevc_decoder_configuration_record_get_nals_sps_combined_optional_start_code(hevc_decoder_configuration_record_t* hevc_decoder_configuration_record, bool include_nal_start_code);
block_t* atsc3_hevc_decoder_configuration_record_get_nals_pps_combined_optional_start_code(hevc_decoder_configuration_record_t* hevc_decoder_configuration_record, bool include_nal_start_code);

//from - https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/mediacodecdec.c
int atsc3_ffmpeg_h2645_ps_to_nalu(const uint8_t *src, int src_size, uint8_t **out, int *out_size);


#define _ATSC3_HEVC_NAL_EXTRACTOR_ERROR(...)  	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_HEVC_NAL_EXTRACTOR_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_HEVC_NAL_EXTRACTOR_INFO(...)   	if(_ATSC3_HEVC_NAL_EXTRACTOR_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);  }
#define _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG(...)  	if(_ATSC3_HEVC_NAL_EXTRACTOR_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_HEVC_NAL_EXTRACTOR_TRACE(...)	if(_ATSC3_HEVC_NAL_EXTRACTOR_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#ifdef __cplusplus
}
#endif

#endif /* ATSC3_HEVC_NAL_EXTRACTOR_H_ */
