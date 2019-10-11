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

                //prefix vps/sps/pps wih h2645_ps_to_nalu
                uint8_t* out_p;
                int out_size;
                int rett = h2645_ps_to_nalu(nals_vps_combined->p_buffer, nals_vps_combined->p_size, &out_p, &out_size);
                
                if(out_size) {
                    uint32_t hex_dump_string_len = (out_size * 5) + 1;

                    char* temp_nal_payload_string_base = calloc(hex_dump_string_len, sizeof(char));
                    char* temp_nal_payload_string_append = temp_nal_payload_string_base;

                    uint8_t* nal_ptr = out_p;

                    for(int j=0; j < out_size; j++) {
                        snprintf(temp_nal_payload_string_append, 6, "0x%02x ", nal_ptr[j]); //snprintf includes nul in the size
                        temp_nal_payload_string_append += 5;
                    }

                    _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("HEVC: wrapped NAL hvcC: VPS, nal unit length is: %d, nal payload is:\n%s\n",
                                                    out_size, temp_nal_payload_string_base);
                    free(temp_nal_payload_string_base);
                } else {
                    _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("HEVC: hvcC: VPS %d, nal unit length is: 0!");
                }
			}
		}

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

	_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("->completed run of test cases for atsc3_hevc_nal_extractor");

	///testdata/hevc/nal/10.1.dallas.v.mp4

	const char* test_sample_hevc_v_3_filename = "testdata/hevc/nal/10.1.dallas.v.mp4";
	int result3 = parse_hevc_nal_test(test_sample_hevc_v_3_filename, true);
	if(result3 < 0) {
		_ATSC3_HEVC_NAL_EXTRACTOR_TEST_ERROR("Sample: %s failed with error: %d", test_sample_hevc_v_3_filename, result3);
	}

	_ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO("->completed run of test cases for atsc3_hevc_nal_extractor");


	return 0;
}

