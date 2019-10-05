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

#include "../atsc3_utils.h"
#include "../atsc3_logging_externs.h"

#include "../atsc3_hevc_nal_extractor.h"

#define _ATSC3_HEVC_NAL_EXTRACTOR_TEST_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_HEVC_NAL_EXTRACTOR_TEST_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_HEVC_NAL_EXTRACTOR_TEST_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _ATSC3_HEVC_NAL_EXTRACTOR_TEST_DEBUG(...)  	__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);

#define __ERROR_FILE_NOT_FOUND -1
#define __ERROR_PARSE_FROM_BLOCK_T -2


int parse_hevc_nal_test(const char* filename) {
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
		fread(mpu_metadata_payload, size, 1, fp);

		block_t* hevc_mpu_metadata_block = block_Duplicate_from_ptr(mpu_metadata_payload, mpu_metadata_payload_size);

		hevc_decoder_configuration_record_t*  hevc_decoder_configuration_record = atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t(hevc_mpu_metadata_block);

		block_Destroy(&hevc_mpu_metadata_block);

		if(!hevc_decoder_configuration_record) {
			ret = __ERROR_PARSE_FROM_BLOCK_T;
		} else {
			//check vps, sps, pps

		}

		if(fp) {
			fclose(fp);
		}
	}

	return ret;
}

int main(int argc, char* argv[] ) {

	const char* test_sample_hevc_v_1_filename = "testdata/hevc/nal/14496-15.2019-nal-extraction-samples/2019-06-18-digi-mmt-5004.35.init.mp4";
	int result = parse_hevc_nal_test(test_sample_hevc_v_1_filename);
	if(!result) {
		_ATSC3_HEVC_NAL_EXTRACTOR_TEST_ERROR("Sample: %c failed with error: %d", test_sample_hevc_v_1_filename, result);
	}

	return 0;
}

