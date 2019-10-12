/*
 * atsc3_hevc_nal_extractor_test.c
 *
 *	test harness for extraction of NAL (sps/pps/vps) from ISOBMFF MPU metadata fragments
 *	for use with HEVC decoder initialization
 *
 *	Specification Reference: ISO 14496-15:2019 - https://www.iso.org/standard/74429.html
 *
 *	Consumer Implementation Use Case: Android MediaCodec: Codec Specific Data (HEVC) - https://developer.android.com/reference/android/media/MediaCodec
 *
 *  Created on: Oct 5, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "../atsc3_utils.h"
#include "../atsc3_logging_externs.h"

#include "../atsc3_hevc_nal_extractor.h"

#define _ATSC3_HEVC_NAL_EXTRACTOR_TEST_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_HEVC_NAL_EXTRACTOR_TEST_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _ATSC3_HEVC_NAL_EXTRACTOR_TEST_DEBUG(...)  	__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);

#define __ERROR_FILE_NOT_FOUND -1
#define __ERROR_PARSE_FROM_BLOCK_T -2

int parse_hevc_nal_test(const char* filename, bool expect_avcC_fallback) {
	_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("-> starting test of: %s", filename);

	int ret = 0;

	struct stat st;
	FILE *fp = NULL;

	stat(filename, &st);
	off_t mpu_metadata_payload_size = st.st_size;

	if(mpu_metadata_payload_size > 0) {
		fp = fopen(filename, "r");
	}

	if(!fp) {
		ret = __ERROR_FILE_NOT_FOUND;
	} else {

		uint8_t* mpu_metadata_payload = (uint8_t*) calloc(mpu_metadata_payload_size, sizeof(uint8_t));
		fread(mpu_metadata_payload, mpu_metadata_payload_size, 1, fp);

		block_t* hevc_mpu_metadata_block = block_Duplicate_from_ptr(mpu_metadata_payload, mpu_metadata_payload_size);

		video_decoder_configuration_record_t* video_decoder_configuration_record = atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t(hevc_mpu_metadata_block);

		block_Destroy(&hevc_mpu_metadata_block);

		if(!video_decoder_configuration_record) {
			ret = __ERROR_PARSE_FROM_BLOCK_T;
		} else {
			//avc1: check  sps, pps
			if(expect_avcC_fallback && video_decoder_configuration_record->avc1_decoder_configuration_record) {
				_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("...-> got back avc1_decoder: %p, sps: %d, pps: %d",
						video_decoder_configuration_record->avc1_decoder_configuration_record,
						video_decoder_configuration_record->avc1_decoder_configuration_record->atsc3_avc1_nal_unit_sps_v.count,
						video_decoder_configuration_record->avc1_decoder_configuration_record->atsc3_avc1_nal_unit_pps_v.count);
                
                atsc3_avc1_decoder_configuration_record_dump(video_decoder_configuration_record->avc1_decoder_configuration_record);

                ret = video_decoder_configuration_record->avc1_decoder_configuration_record->atsc3_avc1_nal_unit_sps_v.count
                        + video_decoder_configuration_record->avc1_decoder_configuration_record->atsc3_avc1_nal_unit_pps_v.count;

			} else {
				//HEVC: vps, sps, pps
				_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("...-> got back hevcC_decoder: %p, vps: %d, sps: %d, pps: %d",
										video_decoder_configuration_record->hevc_decoder_configuration_record,
										video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_vps_v.count,
										video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_sps_v.count,
										video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_pps_v.count);
                
                atsc3_hevc_decoder_configuration_record_dump(video_decoder_configuration_record->hevc_decoder_configuration_record);

                block_t* nals_vps_combined = atsc3_hevc_decoder_configuration_record_get_nals_vps_combined(video_decoder_configuration_record->hevc_decoder_configuration_record);
                _ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("VPS_combined: nals_vps_combined: %p, p_buffer: %p, i_pos: %d, p_size: %d",
                		nals_vps_combined, nals_vps_combined->p_buffer, nals_vps_combined->i_pos, nals_vps_combined->p_size);

                block_t* nals_sps_combined = atsc3_hevc_decoder_configuration_record_get_nals_sps_combined(video_decoder_configuration_record->hevc_decoder_configuration_record);
                _ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("SPS_combined: nals_vps_combined: %p, p_buffer: %p, i_pos: %d, p_size: %d",
                 		nals_sps_combined, nals_sps_combined->p_buffer, nals_sps_combined->i_pos, nals_sps_combined->p_size);

                block_t* nals_pps_combined = atsc3_hevc_decoder_configuration_record_get_nals_pps_combined(video_decoder_configuration_record->hevc_decoder_configuration_record);
                _ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("PPS_combined: nals_vps_combined: %p, p_buffer: %p, i_pos: %d, p_size: %d",
                		nals_pps_combined, nals_pps_combined->p_buffer, nals_pps_combined->i_pos, nals_pps_combined->p_size);

                ret = video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_vps_v.count
                                     + video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_sps_v.count
                                     + video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_pps_v.count;

                atsc3_hevc_nals_record_dump("vps", nals_vps_combined);
                atsc3_hevc_nals_record_dump("sps", nals_sps_combined);
                atsc3_hevc_nals_record_dump("pps", nals_pps_combined);
			}
		}

		if(fp) {
			fclose(fp);
		}
	}

	_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("-> completed test of: %s with result: %d", filename, ret);

	return ret;
}

/*
 * test from atsc3_phy_mmt_player_bridge.c
 *
void atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_mpu_metadata) {
    atsc3_hevc_nals_record_dump("mmt_mpu_metadata", mmt_mpu_metadata);

    if (global_video_packet_id && global_video_packet_id == packet_id) {
        //manually extract our NALs here
        video_decoder_configuration_record_t *video_decoder_configuration_record = atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t(
                mmt_mpu_metadata);

        //we will get either avc1 (avcC) NAL or hevc (hvcC) nals back
        if (video_decoder_configuration_record) {
            if (video_decoder_configuration_record->hevc_decoder_configuration_record) {
                block_t* hevc_nals_combined = block_Alloc(0);
//
//                block_t *nals_vps_combined = atsc3_hevc_decoder_configuration_record_get_nals_vps_combined(
//                        video_decoder_configuration_record->hevc_decoder_configuration_record);
//                block_t *nals_sps_combined = atsc3_hevc_decoder_configuration_record_get_nals_sps_combined(
//                        video_decoder_configuration_record->hevc_decoder_configuration_record);
//                block_t *nals_pps_combined = atsc3_hevc_decoder_configuration_record_get_nals_pps_combined(
//                        video_decoder_configuration_record->hevc_decoder_configuration_record);

                block_t* nals_vps_combined = atsc3_hevc_decoder_configuration_record_get_nals_vps_combined_optional_start_code(
                        video_decoder_configuration_record->hevc_decoder_configuration_record, true);
                block_t* nals_sps_combined = atsc3_hevc_decoder_configuration_record_get_nals_sps_combined_optional_start_code(
                        video_decoder_configuration_record->hevc_decoder_configuration_record, true);
                block_t* nals_pps_combined = atsc3_hevc_decoder_configuration_record_get_nals_pps_combined_optional_start_code(
                        video_decoder_configuration_record->hevc_decoder_configuration_record, true);

                if (nals_vps_combined) {
                    block_Merge(hevc_nals_combined, nals_vps_combined);
                }

                if (nals_sps_combined) {
                    block_Merge(hevc_nals_combined, nals_sps_combined);
                }

                if (nals_pps_combined) {
                    block_Merge(hevc_nals_combined, nals_pps_combined);
                }

                if(hevc_nals_combined->p_size) {
                    block_Rewind(hevc_nals_combined);
                    at3DrvIntf_ptr->onInitHEVC_NAL_Extracted(packet_id == global_video_packet_id, mpu_sequence_number,  block_Get(hevc_nals_combined), hevc_nals_combined->p_size);
                }
            }
        }
    }
}
 */



int parse_hevc_nal_test_atsc3_phy_mmt_player_bridge(const char* filename, bool expect_avcC_fallback) {
	_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("-> starting test of: %s", filename);

	int ret = 0;

	struct stat st;
	FILE *fp = NULL;

	stat(filename, &st);
	off_t mpu_metadata_payload_size = st.st_size;

	if(mpu_metadata_payload_size > 0) {
		fp = fopen(filename, "r");
	}

	if(!fp) {
		ret = __ERROR_FILE_NOT_FOUND;
	} else {

		uint8_t* mpu_metadata_payload = (uint8_t*) calloc(mpu_metadata_payload_size, sizeof(uint8_t));
		fread(mpu_metadata_payload, mpu_metadata_payload_size, 1, fp);

		block_t* hevc_mpu_metadata_block = block_Duplicate_from_ptr(mpu_metadata_payload, mpu_metadata_payload_size);

		video_decoder_configuration_record_t* video_decoder_configuration_record = atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t(hevc_mpu_metadata_block);

		block_Destroy(&hevc_mpu_metadata_block);

		if(!video_decoder_configuration_record) {
			ret = __ERROR_PARSE_FROM_BLOCK_T;
		} else {
			//avc1: check  sps, pps
			if(expect_avcC_fallback && video_decoder_configuration_record->avc1_decoder_configuration_record) {
				_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("...-> got back avc1_decoder: %p, sps: %d, pps: %d",
						video_decoder_configuration_record->avc1_decoder_configuration_record,
						video_decoder_configuration_record->avc1_decoder_configuration_record->atsc3_avc1_nal_unit_sps_v.count,
						video_decoder_configuration_record->avc1_decoder_configuration_record->atsc3_avc1_nal_unit_pps_v.count);

                atsc3_avc1_decoder_configuration_record_dump(video_decoder_configuration_record->avc1_decoder_configuration_record);

                ret = video_decoder_configuration_record->avc1_decoder_configuration_record->atsc3_avc1_nal_unit_sps_v.count
                        + video_decoder_configuration_record->avc1_decoder_configuration_record->atsc3_avc1_nal_unit_pps_v.count;

			} else {
				//HEVC: vps, sps, pps
				_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("...-> got back hevcC_decoder: %p, vps: %d, sps: %d, pps: %d",
										video_decoder_configuration_record->hevc_decoder_configuration_record,
										video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_vps_v.count,
										video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_sps_v.count,
										video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_pps_v.count);

                atsc3_hevc_decoder_configuration_record_dump(video_decoder_configuration_record->hevc_decoder_configuration_record);

				 block_t* hevc_nals_combined = block_Alloc(0);


				 block_t* nals_vps_combined = atsc3_hevc_decoder_configuration_record_get_nals_vps_combined_optional_start_code(
						 video_decoder_configuration_record->hevc_decoder_configuration_record, true);
				 block_t* nals_sps_combined = atsc3_hevc_decoder_configuration_record_get_nals_sps_combined_optional_start_code(
						 video_decoder_configuration_record->hevc_decoder_configuration_record, true);
				 block_t* nals_pps_combined = atsc3_hevc_decoder_configuration_record_get_nals_pps_combined_optional_start_code(
						 video_decoder_configuration_record->hevc_decoder_configuration_record, true);


				_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("VPS_combined: nals_vps_combined: %p, p_buffer: %p, i_pos: %d, p_size: %d",
						nals_vps_combined, nals_vps_combined->p_buffer, nals_vps_combined->i_pos, nals_vps_combined->p_size);
	             atsc3_hevc_nals_record_dump("nal_VPS_start_code", nals_vps_combined);

				_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("SPS_combined: nal_sps_combined: %p, p_buffer: %p, i_pos: %d, p_size: %d",
						nals_sps_combined, nals_sps_combined->p_buffer, nals_sps_combined->i_pos, nals_sps_combined->p_size);
	             atsc3_hevc_nals_record_dump("nal_SPS_start_code", nals_sps_combined);

				_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("PPS_combined: nals_pps_combined: %p, p_buffer: %p, i_pos: %d, p_size: %d",
						nals_pps_combined, nals_pps_combined->p_buffer, nals_pps_combined->i_pos, nals_pps_combined->p_size);
	             atsc3_hevc_nals_record_dump("nal_PPS_start_code", nals_pps_combined);

				 if (nals_vps_combined) {
					 block_Merge(hevc_nals_combined, nals_vps_combined);
				 }

				 if (nals_sps_combined) {
					 block_Merge(hevc_nals_combined, nals_sps_combined);
				 }

				 if (nals_pps_combined) {
					 block_Merge(hevc_nals_combined, nals_pps_combined);
				 }

				 if(hevc_nals_combined->p_size) {
					 block_Rewind(hevc_nals_combined);
					 //at3DrvIntf_ptr->onInitHEVC_NAL_Extracted(packet_id == global_video_packet_id, mpu_sequence_number,  block_Get(hevc_nals_combined), hevc_nals_combined->p_size);
		             atsc3_hevc_nals_record_dump("final NAL cds-0 data", hevc_nals_combined);
				 }


                ret = video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_vps_v.count
                                     + video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_sps_v.count
                                     + video_decoder_configuration_record->hevc_decoder_configuration_record->atsc3_nal_unit_pps_v.count;

			}
		}

		if(fp) {
			fclose(fp);
		}
	}

	_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("-> completed test of: %s with result: %d", filename, ret);

	return ret;
}


/**
 *
 *  FFAMediaFormat *format = NULL;
    MediaCodecH264DecContext *s = avctx->priv_data;

    format = ff_AMediaFormat_new();
    if (!format) {
        av_log(avctx, AV_LOG_ERROR, "Failed to create media format\n");
        ret = AVERROR_EXTERNAL;
        goto done;
    }

    switch (avctx->codec_id) {
#endif
#if CONFIG_HEVC_MEDIACODEC_DECODER
    case AV_CODEC_ID_HEVC:
        codec_mime = "video/hevc";

        ret = hevc_set_extradata(avctx, format);
 *
 *
 *
 *        Incorrect NAL reassembly:
 *            0x03  0x03 is not properly handled in the hevc ps to annex unmapping
 *
 *
 *
 *
 atsc3_hevc_nal_extractor: 776:DEBUG:1570869284.0665:atsc3_hevc_nals_record_dump: nal_VPS_start_code: nals:
 0x00 0x00 0x00 0x01 0x40 0x01 0x0c 0x04 0xff 0xff 0x02 0x20 0x00 0x00 0x03 0x03 0x00 0xb0 0x00 0x00 0x03 0x03 0x00 0x00 0x03 0x03 0x00 0x99 0x00 0x00 0x19 0xc0 0xc0 0x00 0x00 0xfa 0x40 0x00 0x3a 0x98 0x20 0x0f 0xa5 0x70 0x05 0xef 0x79 0x20 0x00 0x0c 0xd8 0xd0 0x00 0x00 0xcd 0x8d 0x00 0x00 0x06 0x6c 0x68 0x00 0x00 0x66 0xc6 0x85 0x40 0x00 0x19 0xb1 0xa0 0x00 0x01 0x9b 0x1a 0x00 0x00 0x0c 0xd8 0xd0 0x00 0x00 0xcd 0x8d 0x0e 0x00 0x00 0xcd 0x8d 0x00 0x00 0x0c 0xd8 0xd1 0x00 0x00 0x66 0xc6 0x80 0x00 0x06 0x6c 0x68 0x20

 test/atsc3_hevc_nal_extr: 250:INFO :1570869284.0665:SPS_combined: nal_sps_combined: 0x10062e670, p_buffer: 0x10062e690, i_pos: 0, p_size: 118
 atsc3_hevc_nal_extractor: 765:INFO :1570869284.0665:atsc3_hevc_nals_record_dump: nal_SPS_start_code: ptr: 0x10062e690, start: 0, len: 118
 atsc3_hevc_nal_extractor: 776:DEBUG:1570869284.0666:atsc3_hevc_nals_record_dump: nal_SPS_start_code: nals:
 0x00 0x00 0x00 0x01 0x42 0x01 0x04 0x02 0x20 0x00 0x00 0x03 0x03 0x00 0xb0 0x00 0x00 0x03 0x03 0x00 0x00 0x03 0x03 0x00 0x99 0x00 0x00 0xa0 0x01 0xe0 0x20 0x02 0x1c 0x4d 0x94 0x67 0x92 0x42 0x96 0x11 0x80 0xb5 0x09 0x10 0x09 0xb6 0x50 0x00 0x00 0x3e 0x90 0x00 0x0e 0xa6 0x08 0x03 0xe9 0xe0 0x0b 0xde 0xf2 0x40 0x00 0x19 0xb1 0xa0 0x00 0x01 0x9b 0x1a 0x00 0x00 0x0c 0xd8 0xd0 0x00 0x00 0xcd 0x8d 0x0a 0x80 0x00 0x33 0x63 0x40 0x00 0x03 0x36 0x34 0x00 0x00 0x19 0xb1 0xa0 0x00 0x01 0x9b 0x1a 0x1c 0x00 0x01 0x9b 0x1a 0x00 0x00 0x19 0xb1 0xa2 0x00 0x00 0xcd 0x8d 0x00 0x00 0x0c 0xd8 0xd0 0x20

 test/atsc3_hevc_nal_extr: 254:INFO :1570869284.0666:PPS_combined: nals_pps_combined: 0x10062e790, p_buffer: 0x10062e710, i_pos: 0, p_size: 11
 atsc3_hevc_nal_extractor: 765:INFO :1570869284.0666:atsc3_hevc_nals_record_dump: nal_PPS_start_code: ptr: 0x10062e710, start: 0, len: 11
 atsc3_hevc_nal_extractor: 776:DEBUG:1570869284.0666:atsc3_hevc_nals_record_dump: nal_PPS_start_code: nals:
 0x00 0x00 0x00 0x01 0x44 0x01 0xc0 0x72 0xf0 0x06 0x40

 atsc3_hevc_nal_extractor: 765:INFO :1570869284.0666:atsc3_hevc_nals_record_dump: final NAL cds-0 data: ptr: 0x10062e7b0, start: 0, len: 233
 atsc3_hevc_nal_extractor: 776:DEBUG:1570869284.0668:atsc3_hevc_nals_record_dump: final NAL cds-0 data: nals:
 0x00 0x00 0x00 0x01 0x40 0x01 0x0c 0x04 0xff 0xff 0x02 0x20 0x00 0x00 0x03 0x03 0x00 0xb0 0x00 0x00 0x03 0x03 0x00 0x00 0x03 0x03 0x00 0x99 0x00 0x00 0x19 0xc0 0xc0 0x00 0x00 0xfa 0x40 0x00 0x3a 0x98 0x20 0x0f 0xa5 0x70 0x05 0xef 0x79 0x20 0x00 0x0c 0xd8 0xd0 0x00 0x00 0xcd 0x8d 0x00 0x00 0x06 0x6c 0x68 0x00 0x00 0x66 0xc6 0x85 0x40 0x00 0x19 0xb1 0xa0 0x00 0x01 0x9b 0x1a 0x00 0x00 0x0c 0xd8 0xd0 0x00 0x00 0xcd 0x8d 0x0e 0x00 0x00 0xcd 0x8d 0x00 0x00 0x0c 0xd8 0xd1 0x00 0x00 0x66 0xc6 0x80 0x00 0x06 0x6c 0x68 0x20 0x00 0x00 0x00 0x01 0x42 0x01 0x04 0x02 0x20 0x00 0x00 0x03 0x03 0x00 0xb0 0x00 0x00 0x03 0x03 0x00 0x00 0x03 0x03 0x00 0x99 0x00 0x00 0xa0 0x01 0xe0 0x20 0x02 0x1c 0x4d 0x94 0x67 0x92 0x42 0x96 0x11 0x80 0xb5 0x09 0x10 0x09 0xb6 0x50 0x00 0x00 0x3e 0x90 0x00 0x0e 0xa6 0x08 0x03 0xe9 0xe0 0x0b 0xde 0xf2 0x40 0x00 0x19 0xb1 0xa0 0x00 0x01 0x9b 0x1a 0x00 0x00 0x0c 0xd8 0xd0 0x00 0x00 0xcd 0x8d 0x0a 0x80 0x00 0x33 0x63 0x40 0x00 0x03 0x36 0x34 0x00 0x00 0x19 0xb1 0xa0 0x00 0x01 0x9b 0x1a 0x1c 0x00 0x01 0x9b 0x1a 0x00 0x00 0x19 0xb1 0xa2 0x00 0x00 0xcd 0x8d 0x00 0x00 0x0c 0xd8 0xd0 0x20 0x00 0x00 0x00 0x01 0x44 0x01 0xc0 0x72 0xf0 0x06 0x40

 
 Correct:
 *
 *
 */

int parse_hevc_nal_test_ffmpeg(const char* filename, bool expect_avcC_fallback) {
    _ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("-> starting test of: %s", filename);

    int ret = 0;

    struct stat st;
    FILE *fp = NULL;

    stat(filename, &st);
    off_t mpu_metadata_payload_size = st.st_size;

    if(mpu_metadata_payload_size > 0) {
        fp = fopen(filename, "r");
    }

    if(!fp) {
        ret = __ERROR_FILE_NOT_FOUND;
    } else {

        uint8_t* mpu_metadata_payload = (uint8_t*) calloc(mpu_metadata_payload_size, sizeof(uint8_t));
        fread(mpu_metadata_payload, mpu_metadata_payload_size, 1, fp);

        block_t* hevc_mpu_metadata_block = block_Duplicate_from_ptr(mpu_metadata_payload, mpu_metadata_payload_size);

        video_decoder_configuration_record_t* video_decoder_configuration_record = atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t(hevc_mpu_metadata_block);

//        AVCodecContext* avctx = (AVCodecContext*)calloc(1, sizeof(AVCodecContext));
//        avctx->extradata = block_Get(video_decoder_configuration_record->hevc_decoder_configuration_record->box_data_original);
//        avctx->extradata_size = video_decoder_configuration_record->hevc_decoder_configuration_record->box_data_original->p_size;
//
//        ret = hevc_set_extradata(avctx, format);
        
        block_t* hevc_nals_combined = atsc3_hevc_extract_extradata_nals_combined_ffmpegImpl(video_decoder_configuration_record->hevc_decoder_configuration_record->box_data_original);

        atsc3_hevc_nals_record_dump("nals_combined_start_code", hevc_nals_combined);
        
        uint8_t* nals = block_Get(hevc_nals_combined);
        uint8_t  nals_len = block_Len(hevc_nals_combined);
        
        //jjustman:2019-10-12: todo - match nals_len and start code/ 0x03 0x03 removal in test

        _ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("...-> hevc_set_extradata, res is: %d", ret);
        if(fp) {
            fclose(fp);
        }
    }

    _ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("-> completed test of: %s with result: %d", filename, ret);

    return ret;
}




int main(int argc, char* argv[] ) {
	_ATSC3_HEVC_NAL_EXTRACTOR_INFO_ENABLED = 1;
	_ATSC3_HEVC_NAL_EXTRACTOR_DEBUG_ENABLED = 1;
	_ATSC3_HEVC_NAL_EXTRACTOR_TRACE_ENABLED = 1;

	_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("->starting test cases for atsc3_hevc_nal_extractor");

	const char* test_sample_hevc_v_1_filename = "testdata/hevc/nal/14496-15.2019-nal-extraction-samples/2019-06-18-digi-mmt-5004.35.init.mp4";
	int result = parse_hevc_nal_test(test_sample_hevc_v_1_filename, true);
	if(result < 0) {
		_ATSC3_HEVC_NAL_EXTRACTOR_TEST_ERROR("Sample: %s failed with error: %d", test_sample_hevc_v_1_filename, result);
	}

	const char* test_sample_hevc_v_2_filename = "testdata/hevc/nal/14496-15.2019-nal-extraction-samples/2019-09-18-cleveland-mmt-35.10.init.mp4";
	int result2 = parse_hevc_nal_test(test_sample_hevc_v_2_filename, true);
	if(result2 < 0) {
		_ATSC3_HEVC_NAL_EXTRACTOR_TEST_ERROR("Sample: %s failed with error: %d", test_sample_hevc_v_2_filename, result2);
	}

	///testdata/hevc/nal/10.1.dallas.v.mp4

	const char* test_sample_hevc_v_3_filename = "testdata/hevc/nal/10.1.dallas.v.mp4";
	int result3 = parse_hevc_nal_test(test_sample_hevc_v_3_filename, true);
	if(result3 < 0) {
		_ATSC3_HEVC_NAL_EXTRACTOR_TEST_ERROR("Sample: %s failed with error: %d", test_sample_hevc_v_3_filename, result3);
	}


	//parse_hevc_nal_test_atsc3_phy_mmt_player_bridge
	//testdata
	const char* test_sample_hevc_v_4_filename = "testdata/hevc/nal/cleveland-35.10.init.mp4";
	int result4 = parse_hevc_nal_test_atsc3_phy_mmt_player_bridge(test_sample_hevc_v_4_filename, true);
	if(result4 < 0) {
		_ATSC3_HEVC_NAL_EXTRACTOR_TEST_ERROR("Sample: %s failed with error: %d", test_sample_hevc_v_4_filename, result4);
	}

	_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("->completed run of test cases for atsc3_hevc_nal_extractor");

	//SXS - test from ffmpeg
	//testdata
	const char* test_sample_hevc_v_5_filename = "testdata/hevc/nal/cleveland-35.10.init.mp4";
	int result5 = parse_hevc_nal_test_ffmpeg(test_sample_hevc_v_5_filename, true);
	if(result5 < 0) {
		_ATSC3_HEVC_NAL_EXTRACTOR_TEST_ERROR("Sample: %s failed with error: %d", test_sample_hevc_v_5_filename, result4);
	}

	_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("->completed run of test cases for atsc3_hevc_nal_extractor");

	return 0;
}

