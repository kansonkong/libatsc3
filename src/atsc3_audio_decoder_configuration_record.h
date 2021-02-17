//
// Created by Jason Justman on 12/1/20.
//
#include <stddef.h>
#include <stdlib.h>

#ifndef ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_H_
#define ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_H_

#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"
#include "atsc3_isobmff_box_parser_tools.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct atsc3_audio_ac4_specific_box {
    uint32_t                        box_size;   //preceeding isobmff box size
    uint32_t                        type;       //dac4 fourcc code

    uint8_t                         ac4_dsi_version:3;
    uint8_t                         bitstream_version:7;
    uint8_t                         fs_index:1;         //This field shall contain the sampling frequency index as described in clause 4.3.3.2.5. Its value shall be Its value shall be the same as read from the ac4_toc, 0: 44100, 1: 48000
    uint8_t                         frame_rate_index:4;
    uint16_t                        n_presentations:9;
    //additional parsing as needed...

} atsc3_audio_ac4_specific_box_t;

/* atsc3_audio_ac4_sample_entry_box:
 *  min. size: at least 45 bytes
 */
typedef struct atsc3_audio_ac4_sample_entry_box {
    uint32_t                        box_size;               //preceeding isobmff box size
    uint32_t                        type;                   //ac-4 fourcc code
    uint8_t                         reserved_48[6];         //6 bytes reserved
    uint16_t                        data_reference_index;
    uint32_t                        reserved_64[2];         //8 bytes reserved
    uint16_t                        channel_count;          //The ChannelCount field should be set to the total number of audio output channels of the default presentation of that track, if not defined differently by an application standard
    uint16_t                        sample_size;            //value 16
    uint32_t                        reserved_32;            //4 bytes reserved
    uint16_t                        sampling_frequency;
    uint16_t                        reserved_16;            //2 bytes reserved

    //assumed required that if we are ac4_sample, then we must have an ac4_specific box
    atsc3_audio_ac4_specific_box_t  atsc3_audio_ac4_specific_box;
} atsc3_audio_ac4_sample_entry_box_t;

typedef struct atsc3_audio_decoder_configuration_record {

    //extracted from moov/init segment
    uint16_t channel_count;
    uint16_t sample_depth;
    uint32_t sample_rate;
    uint32_t timebase;  //14496-12:2015 - timescale is uint32_t

    //extracted from moof/fragment metadata
    uint32_t sample_duration;   //14496-12:2015 - duration is uint32_t

    atsc3_audio_ac4_sample_entry_box_t* atsc3_audio_ac4_sample_entry_box;

    //todo: jjustman-2020-12-01 - add in stsd child fourcc code and isobmff box in block_t format

} atsc3_audio_decoder_configuration_record_t;

atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record_new();
void atsc3_audio_decoder_configuration_record_free(atsc3_audio_decoder_configuration_record_t** atsc3_audio_decoder_configuration_record_p);

atsc3_audio_ac4_sample_entry_box_t* atsc3_audio_ac4_sample_entry_box_new();
atsc3_audio_ac4_sample_entry_box_t* atsc3_audio_decoder_ac4_parse_init_box_from_block_t(block_t* mmt_mpu_metadata_block);
void atsc3_audio_ac4_sample_entry_box_dump(atsc3_audio_ac4_sample_entry_box_t* atsc3_audio_ac4_sample_entry_box);

atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record_parse_from_block_t(block_t* mmt_mpu_metadata_block);
bool atsc3_audio_decoder_configuration_parse_codec_type_and_sample_rate_from_block_t(atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record, block_t* mmt_mpu_metadata_block);



#define _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_INFO(...)  if(_ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_DEBUG(...) if(_ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_TRACE(...) if(_ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#ifdef __cplusplus
}
#endif

#endif //ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_H_
