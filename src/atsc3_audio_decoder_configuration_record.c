//
// Created by Jason Justman on 12/1/20.
//

#include "atsc3_audio_decoder_configuration_record.h"

int _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_INFO_ENABLED = 1;
int _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_DEBUG_ENABLED = 1;
int _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_TRACE_ENABLED = 0;

atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record_new() {
    atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record = calloc(1, sizeof(atsc3_audio_decoder_configuration_record_t));

    return atsc3_audio_decoder_configuration_record;
}

void atsc3_audio_decoder_configuration_record_free(atsc3_audio_decoder_configuration_record_t** atsc3_audio_decoder_configuration_record_p) {
    if(atsc3_audio_decoder_configuration_record_p) {
        atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record = *atsc3_audio_decoder_configuration_record_p;
        if(atsc3_audio_decoder_configuration_record) {

            free(atsc3_audio_decoder_configuration_record);
            atsc3_audio_decoder_configuration_record = NULL;
        }

        *atsc3_audio_decoder_configuration_record_p = NULL;
    }
}

atsc3_audio_ac4_sample_entry_box_t* atsc3_audio_ac4_sample_entry_box_new() {
    atsc3_audio_ac4_sample_entry_box_t* atsc3_audio_ac4_sample_entry_box = calloc(1, sizeof(atsc3_audio_ac4_sample_entry_box_t));

    return atsc3_audio_ac4_sample_entry_box;
}

atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record_parse_from_block_t(block_t* mmt_mpu_metadata_block) {
    atsc3_isobmff_mdhd_box_t* atsc3_isobmff_mdhd_box = NULL;

    atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record = atsc3_audio_decoder_configuration_record_new();


    atsc3_isobmff_mdhd_box = atsc3_isobmff_box_parser_tools_parse_mdhd_from_block_t(mmt_mpu_metadata_block);
    if(atsc3_isobmff_mdhd_box) {
        if (atsc3_isobmff_mdhd_box->version == 1) {
            atsc3_audio_decoder_configuration_record->timebase = atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v1.timescale;
        } else if (atsc3_isobmff_mdhd_box->version == 0) {
            atsc3_audio_decoder_configuration_record->timebase = atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v0.timescale;
        }

        atsc3_isobmff_mdhd_box_free(&atsc3_isobmff_mdhd_box);
    }

    atsc3_audio_decoder_configuration_parse_codec_type_and_sample_rate_from_block_t(atsc3_audio_decoder_configuration_record, mmt_mpu_metadata_block);

    return atsc3_audio_decoder_configuration_record;
}

/*
 *

        //jjustman-2020-11-30 - hack


         *
         *   [stbl] size=8+157
          [stsd] size=12+77
            entry-count = 1
            [ac-4] size=8+65
              data_reference_index = 1
              channel_count = 2
              sample_size = 16
              sample_rate = 48000
              [dac4] size=8+29
                ac4_dsi_version = 1
                bitstream_version = 2
                fs_index = 1
                fs = 48000
                frame_rate_index = 3
                short_program_id = 0
                program_uuid = [00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00]
                bit_rate_mode = 2
                bit_rate = 0
                bit_rate_precision = 4294967295
                [00].presentation_version = 1
                [00].presentation_config_v1 = 31
                [00].mdcompat = 0
                [00].presentation_group_index = 0
                [00].dsi_frame_rate_multiply_info = 0
                [00].dsi_frame_rate_fraction_info = 0
                [00].presentation_emdf_version = 0
                [00].presentation_key_id = 0
                [00].b_presentation_channel_coded = 1
                [00].dsi_presentation_ch_mode = 1
                [00].pres_b_4_back_channels_present = 0
                [00].pres_top_channel_pairs = 0
                [00].presentation_channel_mask_v1 = 1


         * E.3 AC4SampleEntry Box
         *  https://www.etsi.org/deliver/etsi_ts/103100_103199/103190/01.01.01_60/ts_103190v010101p.pdf
         *
         *  AC4SampleEntry()
                {
                 BoxHeader.Size;        32 Note 3
                 BoxHeader.Type;        32 'ac-4'
                 Reserved[6];           8 0
                 DataReferenceIndex;    16 Note 3
                 Reserved[2];           32 0
                 ChannelCount; 16 Note 1, 2
                 SampleSize; 16 16
                 Reserved; 32 0
                 SamplingFrequency; 16 Note 2
                 Reserved; 16 0
                 Ac4SpecificBox();
                }

                ac4_dsi()
                {
                    ac4_dsi_version 3
                    bitstream_version 7
                    fs_index 1
                     frame_rate_index 4
                     n_presentations 9

                     fs_index: 0 -> 44,1000
                               1 -> 48,000


 *
 *  [mp4a] size=8+67
      data_reference_index = 1
      channel_count = 2
      sample_size = 16
      sample_rate = 48000
      [esds] size=12+27
        [ESDescriptor] size=2+25
          es_id = 0
          stream_priority = 0
          [DecoderConfig] size=2+17
            stream_type = 5
            object_type = 64
            up_stream = 0
            buffer_size = 8192
            max_bitrate = 128000
            avg_bitrate = 128000
            DecoderSpecificInfo = 11 90
          [Descriptor:06] size=2+1

          https://wiki.multimedia.cx/index.php/Understanding_AAC
          https://developer.android.com/reference/android/media/MediaCodecs

     jjustman-2020-12-02: TODO: add in additional fourcc parsing types in addition to ac-4
 */

bool atsc3_audio_decoder_configuration_parse_codec_type_and_sample_rate_from_block_t(atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record, block_t* mmt_mpu_metadata_block) {

    atsc3_audio_ac4_sample_entry_box_t* atsc3_audio_ac4_sample_entry_box = NULL;

    block_Rewind(mmt_mpu_metadata_block);
    uint8_t* audio_ptr = block_Get(mmt_mpu_metadata_block);

    for (int i = 0;  (i < mmt_mpu_metadata_block->p_size - 4); i++) {
        block_Seek(mmt_mpu_metadata_block, i);
        _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_TRACE("atsc3_audio_decoder_configuration_parse_codec_type_and_sample_rate_from_block_t: searching for either ac-4 or mp4a, position: %d, checking: 0x%02x (%c), 0x%02x (%c), 0x%02x (%c), 0x%02x (%c)",
                                              i, audio_ptr[i], audio_ptr[i], audio_ptr[i + 1], audio_ptr[i + 1], audio_ptr[i + 2], audio_ptr[i + 2], audio_ptr[i + 3], audio_ptr[i + 3]);


        //look for ac-4 fourcc
        if (audio_ptr[i] == 'a' && audio_ptr[i + 1] == 'c' && audio_ptr[i + 2] == '-' && audio_ptr[i + 3] == '4') {
            _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_DEBUG("atsc3_audio_decoder_configuration_parse_codec_type_and_sample_rate_from_block_t: ac-4: found matching at position: %d, seeking back 4 bytes to: %d", i, i-4);
            block_Seek(mmt_mpu_metadata_block, i-4);
            atsc3_audio_ac4_sample_entry_box = atsc3_audio_decoder_ac4_parse_init_box_from_block_t(mmt_mpu_metadata_block);

            if(atsc3_audio_ac4_sample_entry_box) {
                atsc3_audio_ac4_sample_entry_box_dump(atsc3_audio_ac4_sample_entry_box);
                atsc3_audio_decoder_configuration_record->channel_count = atsc3_audio_ac4_sample_entry_box->channel_count;
                atsc3_audio_decoder_configuration_record->sample_depth = atsc3_audio_ac4_sample_entry_box->sample_size;
                atsc3_audio_decoder_configuration_record->sample_rate = atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.fs_index == 0 ? 44100 : 48000;
            }
            break;
        }
    }

    return (atsc3_audio_ac4_sample_entry_box != NULL);
}


atsc3_audio_ac4_sample_entry_box_t* atsc3_audio_decoder_ac4_parse_init_box_from_block_t(block_t* mmt_mpu_metadata_block) {
    atsc3_audio_ac4_sample_entry_box_t* atsc3_audio_ac4_sample_entry_box = NULL;

    if(!mmt_mpu_metadata_block || block_Remaining_size(mmt_mpu_metadata_block) < 45) {
        _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_ERROR("atsc3_audio_decoder_ac4_parse_init_box_from_block_t: unable to parse ac4_sample_entry box, block_t too small, size: %d", (!mmt_mpu_metadata_block ? 0 : block_Remaining_size(mmt_mpu_metadata_block)));
        return NULL;
    }

    atsc3_audio_ac4_sample_entry_box = atsc3_audio_ac4_sample_entry_box_new();

    atsc3_audio_ac4_sample_entry_box->box_size = block_Read_uint32_ntohl(mmt_mpu_metadata_block);
    atsc3_audio_ac4_sample_entry_box->type = block_Read_uint32_ntohl(mmt_mpu_metadata_block);
    //skip 6 bytes
    block_Seek_Relative(mmt_mpu_metadata_block, 6);
    atsc3_audio_ac4_sample_entry_box->data_reference_index = block_Read_uint16_ntohs(mmt_mpu_metadata_block);
    block_Seek_Relative(mmt_mpu_metadata_block, 8);
    atsc3_audio_ac4_sample_entry_box->channel_count = block_Read_uint16_ntohs(mmt_mpu_metadata_block);
    atsc3_audio_ac4_sample_entry_box->sample_size = block_Read_uint16_ntohs(mmt_mpu_metadata_block);
    block_Seek_Relative(mmt_mpu_metadata_block, 4);
    atsc3_audio_ac4_sample_entry_box->sampling_frequency = block_Read_uint16_ntohs(mmt_mpu_metadata_block);
    block_Seek_Relative(mmt_mpu_metadata_block, 2);

    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.box_size = block_Read_uint32_ntohl(mmt_mpu_metadata_block);;
    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.type = block_Read_uint32_ntohl(mmt_mpu_metadata_block);;

    //bit slicing here
    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.ac4_dsi_version = block_Read_uint8_bitlen(mmt_mpu_metadata_block, 3);
    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.bitstream_version = block_Read_uint8_bitlen(mmt_mpu_metadata_block, 7);
    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.fs_index = block_Read_uint8_bitlen(mmt_mpu_metadata_block, 1);
    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.frame_rate_index = block_Read_uint8_bitlen(mmt_mpu_metadata_block, 4);
    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.n_presentations = block_Read_uint16_bitlen(mmt_mpu_metadata_block, 9);

    return atsc3_audio_ac4_sample_entry_box;
}

void atsc3_audio_ac4_sample_entry_box_dump(atsc3_audio_ac4_sample_entry_box_t* atsc3_audio_ac4_sample_entry_box) {
    if(!atsc3_audio_ac4_sample_entry_box) {
        _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_ERROR("atsc3_audio_ac4_sample_entry_box_dump: atsc3_audio_ac4_sample_entry_box is NULL!");
        return;
    }

    _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_DEBUG("atsc3_audio_ac4_sample_entry_box_dump: [%c%c%c%c] size=%d, data_reference_index: %d (0x%04x), channel_count: %d (0x%04x), sample_size: %d (0x%04x), sampling_frequency: %d (0x%04x)",
                                                    (atsc3_audio_ac4_sample_entry_box->type >> 24) & 0xFF,
                                                    (atsc3_audio_ac4_sample_entry_box->type >> 16) & 0xFF,
                                                    (atsc3_audio_ac4_sample_entry_box->type >> 8) & 0xFF,
                                                    (atsc3_audio_ac4_sample_entry_box->type >> 0) & 0xFF,
                                                    atsc3_audio_ac4_sample_entry_box->box_size,
                                                    atsc3_audio_ac4_sample_entry_box->data_reference_index,
                                                    atsc3_audio_ac4_sample_entry_box->data_reference_index,
                                                    atsc3_audio_ac4_sample_entry_box->channel_count,
                                                    atsc3_audio_ac4_sample_entry_box->channel_count,
                                                    atsc3_audio_ac4_sample_entry_box->sample_size,
                                                    atsc3_audio_ac4_sample_entry_box->sample_size,
                                                    atsc3_audio_ac4_sample_entry_box->sampling_frequency,
                                                    atsc3_audio_ac4_sample_entry_box->sampling_frequency);

    _ATSC3_AUDIO_DECODER_CONFIGURATION_RECORD_DEBUG("atsc3_audio_ac4_sample_entry_box_dump: atsc3_audio_ac4_specific_box: [%c%c%c%c] size=%d, ac4_dsi_version: 0x%01x, bitstream_version: 0x%02x, fs_index: 0x%01x, frame_rate_index: 0x%01x, n_presentations: 0x%02x",
                                                    (atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.type >> 24) & 0xFF,
                                                    (atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.type >> 16) & 0xFF,
                                                    (atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.type >> 8) & 0xFF,
                                                    (atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.type >> 0) & 0xFF,
                                                    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.box_size,
                                                    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.ac4_dsi_version,
                                                    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.bitstream_version,
                                                    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.fs_index,
                                                    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.frame_rate_index,
                                                    atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.n_presentations);


}