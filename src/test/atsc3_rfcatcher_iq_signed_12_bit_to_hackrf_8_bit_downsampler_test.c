/*
 * atsc3_rfcatcher_iq_signed_12_bit_to_hackrf_8_bit_downsampler_test.c
 *
 *  Created on: 2022-07-25
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>

#include "../atsc3_utils.h"

#define _ATSC3_RFCATCHER_TO_HACKRF_DOWNSAMPLER_TEST_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_RFCATCHER_TO_HACKRF_DOWNSAMPLER_TEST_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_RFCATCHER_TO_HACKRF_DOWNSAMPLER_TEST_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_RFCATCHER_TO_HACKRF_DOWNSAMPLER_TEST_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);

#define _ATSC3_RFCATCHER_TO_HACKRF_DOWNSAMPLER_TEST_TRACE(...)   printf("%s:%d:TRACE:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);

#define LOG_STATUS_PROGRESS_EVERY_N_BYTES 10000000
int main(int argc, char* argv[] ) {
    bool needs_endian_flip = false;
    if(is_bigendian()) {
        needs_endian_flip = true;
        _ATSC3_RFCATCHER_TO_HACKRF_DOWNSAMPLER_TEST_INFO("needs endian flip!");
    }
    const char* RFCATCHER_TO_HACKRF_DOWNSAMPLER_INPUT_FILENAME = "testdata/LDM2_rfcatcher.iq";
    const char* RFCATCHER_TO_HACKRF_DOWNSAMPLER_OUTPUT_FILENAME = "testdata/LDM2_hackrf_out_8bit.iq";

    FILE* fp_in = NULL;
    long fpos_in = 0;
    
    FILE* fp_out = NULL;
    long fpos_out = 0;
    
    struct stat st;
    stat(RFCATCHER_TO_HACKRF_DOWNSAMPLER_INPUT_FILENAME, &st);
    if(!st.st_size) {
        _ATSC3_RFCATCHER_TO_HACKRF_DOWNSAMPLER_TEST_ERROR("atsc3_rfcatcher_iq_signed_12_bit_to_hackrf_8_bit_downsampler_test: %s, ERROR: st.st_size is 0!", RFCATCHER_TO_HACKRF_DOWNSAMPLER_INPUT_FILENAME);
        return -1;
    }

    _ATSC3_RFCATCHER_TO_HACKRF_DOWNSAMPLER_TEST_DEBUG("atsc3_rfcatcher_iq_signed_12_bit_to_hackrf_8_bit_downsampler_test: file: %s, size: %zu", RFCATCHER_TO_HACKRF_DOWNSAMPLER_INPUT_FILENAME, st.st_size);

    fp_in = fopen(RFCATCHER_TO_HACKRF_DOWNSAMPLER_INPUT_FILENAME, "r");
    
    if(!fp_in) {
        _ATSC3_RFCATCHER_TO_HACKRF_DOWNSAMPLER_TEST_ERROR("atsc3_rfcatcher_iq_signed_12_bit_to_hackrf_8_bit_downsampler_test: ERROR: unable to open input file, %s", RFCATCHER_TO_HACKRF_DOWNSAMPLER_INPUT_FILENAME);
        return -2;
    }
    
    fp_out = fopen(RFCATCHER_TO_HACKRF_DOWNSAMPLER_OUTPUT_FILENAME, "w");
    
    if(!fp_out) {
        _ATSC3_RFCATCHER_TO_HACKRF_DOWNSAMPLER_TEST_ERROR("atsc3_rfcatcher_iq_signed_12_bit_to_hackrf_8_bit_downsampler_test: ERROR: unable to open output file, %s", RFCATCHER_TO_HACKRF_DOWNSAMPLER_OUTPUT_FILENAME);
        fclose(fp_in);
        return -3;
    }

    /*
     converting from 12bit to 8 bit,
        read in each short,
        div by 16 (e.g. 4096/256 = 16)
        take lower 8 bits
        write out downsample
     */
    
    int16_t sample_in_12bit;
    int8_t  sample_out_8bit;
    
    while(!feof(fp_in)) {
        fread(&sample_in_12bit, 2, 1, fp_in);
        if(needs_endian_flip) {
            sample_in_12bit = ntohs(sample_in_12bit);
        }
        
        fpos_in += 2;
        
        sample_out_8bit = (int8_t) (( sample_in_12bit / 16) & 0xFF);
        
        fwrite(&sample_out_8bit, 1, 1, fp_out);
        fpos_out += 1;
        
        if((fpos_in % LOG_STATUS_PROGRESS_EVERY_N_BYTES == 0)) {
            _ATSC3_RFCATCHER_TO_HACKRF_DOWNSAMPLER_TEST_TRACE("Read in sample value 0x%04x (%d), converted to 0x%02x (%d), fpos_in: %ld, fpos_out: %ld, remaining:  %zu",
                                                          sample_in_12bit,
                                                          sample_in_12bit,
                                                          sample_out_8bit,
                                                          sample_out_8bit,
                                                          fpos_in,
                                                          fpos_out, (st.st_size - fpos_in));
        }
    }
    
    fclose(fp_out);

    return 0;
}

