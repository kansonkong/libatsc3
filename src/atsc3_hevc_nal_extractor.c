/*
 * atsc3_hevc_nal_extractor.c
 *
 *  Created on: Oct 5, 2019
 *      Author: jjustman
 */

#include "atsc3_hevc_nal_extractor.h"

int _ATSC3_HEVC_NAL_EXTRACTOR_INFO_ENABLED = 0;
int _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG_ENABLED = 0;
int _ATSC3_HEVC_NAL_EXTRACTOR_TRACE_ENABLED = 0;

//default atsc3_vector_builder collection for hevc nal types

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_vps);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_sps);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_pps);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_prefix_sei);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_suffix_sei);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_sps);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_pps);

/*
 * create a new hevc_decoder_configuration_record
 * set our configuration_version to 1,
 * 	todo: set MSB bitmasks to 1 to match ISO 14496-15:2019 spec
 */
hevc_decoder_configuration_record_t *hevc_decoder_configuration_record_new() {
    hevc_decoder_configuration_record_t *hevc_decoder_configuration_record = calloc(1, sizeof(hevc_decoder_configuration_record_t));
    hevc_decoder_configuration_record->configuration_version = 1;

    return hevc_decoder_configuration_record;
}

/*
 * legacy (read: non ATSC/3.0) avcC SPS/PPS support
 */

avc1_decoder_configuration_record_t *avc1_decoder_configuration_record_new() {
    avc1_decoder_configuration_record_t *avc1_decoder_configuration_record = calloc(1, sizeof(avc1_decoder_configuration_record_t));
    return avc1_decoder_configuration_record;
}


/**
 * add default _free methods for each one of these to block_Destroy nal_unit, e.g.:
 *
 * void atsc3_nal_unit_vps_free(atsc3_nal_unit_vps_t** atsc3_nal_unit_vps_p) {
	if(atsc3_nal_unit_vps_p) {
		atsc3_nal_unit_vps_t* atsc3_nal_unit_vps = *atsc3_nal_unit_vps_p;
		if(atsc3_nal_unit_vps) {
			//todo: more impl's as needed
			block_Destroy(&atsc3_nal_unit_vps->nal_unit);

			free(atsc3_nal_unit_vps);
		}
		atsc3_nal_unit_vps = NULL;
		*atsc3_nal_unit_vps_p = NULL;
	}
}

s/nal_unit_vps/nal_unit_sps ...etc
*/

//atsc3_nal_unit_vps_free
void atsc3_nal_unit_vps_free(atsc3_nal_unit_vps_t **atsc3_nal_unit_vps_p) {
    if (atsc3_nal_unit_vps_p) {
        atsc3_nal_unit_vps_t *atsc3_nal_unit_vps = *atsc3_nal_unit_vps_p;
        if (atsc3_nal_unit_vps) {
//todo: more impl's as needed
            block_Destroy(&atsc3_nal_unit_vps->nal_unit);

            free(atsc3_nal_unit_vps);
        }
        atsc3_nal_unit_vps = NULL;
        *atsc3_nal_unit_vps_p = NULL;
    }
}

//atsc3_nal_unit_sps
void atsc3_nal_unit_sps_free(atsc3_nal_unit_sps_t **atsc3_nal_unit_sps_p) {
    if (atsc3_nal_unit_sps_p) {
        atsc3_nal_unit_sps_t *atsc3_nal_unit_sps = *atsc3_nal_unit_sps_p;
        if (atsc3_nal_unit_sps) {
//todo: more impl's as needed
            block_Destroy(&atsc3_nal_unit_sps->nal_unit);

            free(atsc3_nal_unit_sps);
        }
        atsc3_nal_unit_sps = NULL;
        *atsc3_nal_unit_sps_p = NULL;
    }
}

//atsc3_nal_unit_pps
void atsc3_nal_unit_pps_free(atsc3_nal_unit_pps_t **atsc3_nal_unit_pps_p) {
    if (atsc3_nal_unit_pps_p) {
        atsc3_nal_unit_pps_t *atsc3_nal_unit_pps = *atsc3_nal_unit_pps_p;
        if (atsc3_nal_unit_pps) {
//todo: more impl's as needed
            block_Destroy(&atsc3_nal_unit_pps->nal_unit);

            free(atsc3_nal_unit_pps);
        }
        atsc3_nal_unit_pps = NULL;
        *atsc3_nal_unit_pps_p = NULL;
    }
}

//atsc3_nal_unit_prefix_sei
void atsc3_nal_unit_prefix_sei_free(atsc3_nal_unit_prefix_sei_t **atsc3_nal_unit_prefix_sei_p) {
    if (atsc3_nal_unit_prefix_sei_p) {
        atsc3_nal_unit_prefix_sei_t *atsc3_nal_unit_prefix_sei = *atsc3_nal_unit_prefix_sei_p;
        if (atsc3_nal_unit_prefix_sei) {
//todo: more impl's as needed
            block_Destroy(&atsc3_nal_unit_prefix_sei->nal_unit);

            free(atsc3_nal_unit_prefix_sei);
        }
        atsc3_nal_unit_prefix_sei = NULL;
        *atsc3_nal_unit_prefix_sei_p = NULL;
    }
}

//atsc3_nal_unit_suffix_sei
void atsc3_nal_unit_suffix_sei_free(atsc3_nal_unit_suffix_sei_t **atsc3_nal_unit_suffix_sei_p) {
    if (atsc3_nal_unit_suffix_sei_p) {
        atsc3_nal_unit_suffix_sei_t *atsc3_nal_unit_suffix_sei = *atsc3_nal_unit_suffix_sei_p;
        if (atsc3_nal_unit_suffix_sei) {
//todo: more impl's as needed
            block_Destroy(&atsc3_nal_unit_suffix_sei->nal_unit);

            free(atsc3_nal_unit_suffix_sei);
        }
        atsc3_nal_unit_suffix_sei = NULL;
        *atsc3_nal_unit_suffix_sei_p = NULL;
    }
}


//atsc3_avc1_nal_unit_sps
void atsc3_avc1_nal_unit_sps_free(atsc3_avc1_nal_unit_sps_t **atsc3_avc1_nal_unit_sps_p) {
    if (atsc3_avc1_nal_unit_sps_p) {
        atsc3_avc1_nal_unit_sps_t *atsc3_avc1_nal_unit_sps = *atsc3_avc1_nal_unit_sps_p;
        if (atsc3_avc1_nal_unit_sps) {
//todo: more impl's as needed
            block_Destroy(&atsc3_avc1_nal_unit_sps->nal_unit);

            free(atsc3_avc1_nal_unit_sps);
        }
        atsc3_avc1_nal_unit_sps = NULL;
        *atsc3_avc1_nal_unit_sps_p = NULL;
    }
}

//atsc3_avc1_nal_unit_pps
void atsc3_avc1_nal_unit_pps_free(atsc3_avc1_nal_unit_pps_t **atsc3_avc1_nal_unit_pps_p) {
    if (atsc3_avc1_nal_unit_pps_p) {
        atsc3_avc1_nal_unit_pps_t *atsc3_avc1_nal_unit_pps = *atsc3_avc1_nal_unit_pps_p;
        if (atsc3_avc1_nal_unit_pps) {
//todo: more impl's as needed
            block_Destroy(&atsc3_avc1_nal_unit_pps->nal_unit);

            free(atsc3_avc1_nal_unit_pps);
        }
        atsc3_avc1_nal_unit_pps = NULL;
        *atsc3_avc1_nal_unit_pps_p = NULL;
    }
}

video_decoder_configuration_record_t *video_decoder_configuration_record_new() {
    video_decoder_configuration_record_t *video_decoder_configuration_record = calloc(1, sizeof(video_decoder_configuration_record_t));
    return video_decoder_configuration_record;
}

#define VPS_NAL_unit_type 32
#define SPS_NAL_unit_type 33
#define PPS_NAL_unit_type 34

/*
 * process either avcC or hvcC, use video_decoder_configuration_record (containing both avcC and hvcC decoder records)
   from track init box (moov)
*/

video_decoder_configuration_record_t* atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t(block_t *mpu_metadata_block) {
    atsc3_isobmff_mdhd_box_t* atsc3_isobmff_mdhd_box = NULL;

    if (!mpu_metadata_block || mpu_metadata_block->p_size < 8) {
        goto error;
    }

    block_Rewind(mpu_metadata_block);
//first, search for the tkhd
//then, search for isobmff box: hvcC
//todo: search for the avcC box for h264 use cases

    _ATSC3_HEVC_NAL_EXTRACTOR_TRACE("atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t: mpu_metadata_block_t: %p, p_buffer: %p, pos: %d, size: %d", mpu_metadata_block, mpu_metadata_block->p_buffer, mpu_metadata_block->i_pos, mpu_metadata_block->p_size);

    video_decoder_configuration_record_t* video_decoder_configuration_record = video_decoder_configuration_record_new();

    atsc3_isobmff_mdhd_box = atsc3_isobmff_box_parser_tools_parse_mdhd_timescale_from_block_t(mpu_metadata_block);
    if(atsc3_isobmff_mdhd_box) {
        if (atsc3_isobmff_mdhd_box->version == 1) {
            video_decoder_configuration_record->timebase = atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v1.timescale;
        } else if (atsc3_isobmff_mdhd_box->version == 0) {
            video_decoder_configuration_record->timebase = atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v0.timescale;
        }

        atsc3_isobmff_mdhd_box_free(&atsc3_isobmff_mdhd_box);
    }

    uint8_t *tkhd_ptr = block_Get(mpu_metadata_block);
    bool has_tkhd_match = false;
    int tkhd_match_index = 0;
    uint32_t init_buff_remaining = 0;

    for (int i = 0; !(has_tkhd_match) && (i < mpu_metadata_block->p_size - 4); i++) {

        _ATSC3_HEVC_NAL_EXTRACTOR_TRACE("atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t: searching for tkhd, position: %d, checking: 0x%02x (%c), 0x%02x (%c), 0x%02x (%c), 0x%02x (%c)",
                                        i, tkhd_ptr[i], tkhd_ptr[i], tkhd_ptr[i + 1], tkhd_ptr[i + 1], tkhd_ptr[i + 2], tkhd_ptr[i + 2], tkhd_ptr[i + 3], tkhd_ptr[i + 3]);

//look for our fourcc
        if (tkhd_ptr[i] == 't' && tkhd_ptr[i + 1] == 'k' && tkhd_ptr[i + 2] == 'h' && tkhd_ptr[i + 3] == 'd') {
            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: tkhd: found matching at position: %d", i);
            has_tkhd_match = true;
            tkhd_match_index = i + 4;
            init_buff_remaining = mpu_metadata_block->p_size - i;
            continue;
        }
    }

    if (has_tkhd_match && tkhd_match_index) {
        atsc3_init_parse_tkhd_for_width_height(video_decoder_configuration_record, &tkhd_ptr[tkhd_match_index], init_buff_remaining);

        if (video_decoder_configuration_record->width < 64) {
            _ATSC3_HEVC_NAL_EXTRACTOR_TRACE("atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t: atsc3_init_parse_tkhd_for_width_height return - video_decoder_configuration_record->width < 64 (%d), setting to 0!", video_decoder_configuration_record->width);
            video_decoder_configuration_record->width = 0;
        }

        if (video_decoder_configuration_record->height < 64) {
            _ATSC3_HEVC_NAL_EXTRACTOR_TRACE("atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t: atsc3_init_parse_tkhd_for_width_height - return video_decoder_configuration_record->height < 64 (%d), setting to 0!", video_decoder_configuration_record->height);
            video_decoder_configuration_record->height = 0;
        }
    }
    uint8_t *mpu_ptr = block_Get(mpu_metadata_block);

    bool has_hev1_match = false; //14496-15:2019 defines this as either hvc1 or hev1, should also be the same box size of avc1
    int hev1_match_index = 0;

    bool has_hvcC_match = false;
    int hvcC_match_index = 0;

    bool has_avcC_match = false;
    int avcC_match_index = 0;

    for (int i = 0; !(has_hvcC_match || has_avcC_match) && (i < mpu_metadata_block->p_size - 4); i++) {

        _ATSC3_HEVC_NAL_EXTRACTOR_TRACE("atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t: position: %d, checking: 0x%02x (%c), 0x%02x (%c), 0x%02x (%c), 0x%02x (%c)",
                                        i, mpu_ptr[i], mpu_ptr[i], mpu_ptr[i + 1], mpu_ptr[i + 1], mpu_ptr[i + 2], mpu_ptr[i + 2], mpu_ptr[i + 3], mpu_ptr[i + 3]);

//hev1 box extraction for w/height - HEVCConfigurationBox
        if (mpu_ptr[i] == 'h' && (mpu_ptr[i + 1] == 'v' || mpu_ptr[i + 1] == 'e') &&
            (mpu_ptr[i + 2] == 'c' || mpu_ptr[i + 2] == 'v') && mpu_ptr[i + 3] == '1') {
            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: HEVC: found matching hev1 (hvc1) at position: %d", i);
            has_hev1_match = true;
            hev1_match_index = i + 4;

            video_decoder_configuration_record->has_hev1_box = true;
            video_decoder_configuration_record->has_hvcC_box = true;
        }

            //look for our HEVC hvcC first, then fallback to avcC
        if (mpu_ptr[i] == 'h' && mpu_ptr[i + 1] == 'v' && mpu_ptr[i + 2] == 'c' &&
            mpu_ptr[i + 3] == 'C') {
            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: HEVC: found matching hvcC at position: %d", i);
            has_hvcC_match = true;
            hvcC_match_index = i + 4;
            video_decoder_configuration_record->has_hev1_box = true;

        } else if (mpu_ptr[i] == 'a' && mpu_ptr[i + 1] == 'v' && mpu_ptr[i + 2] == 'c' &&
                   mpu_ptr[i + 3] == 'C') {
            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: AVC: found matching avcC at position: %d", i);
            has_avcC_match = true;
            avcC_match_index = i + 4;

            video_decoder_configuration_record->has_avcC_box = true;
        }
    }

    if (has_hev1_match && hev1_match_index && !video_decoder_configuration_record->width && !video_decoder_configuration_record->height) {
        atsc3_init_parse_HEVCConfigurationBox_for_width_height(video_decoder_configuration_record, &mpu_ptr[hev1_match_index],
                                                               mpu_metadata_block->p_size -
                                                               hev1_match_index);
    }

    if (!has_hvcC_match && !has_avcC_match) {
        goto error;
    }

//jjustman-2019-11-14 - if we don't have a sane value for  width or height yet,
// either from trak/tkhd.width|height, or from hev1.width|height, set these to zero and warn...

    if (video_decoder_configuration_record->width < 64) {
        _ATSC3_HEVC_NAL_EXTRACTOR_WARN("atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t: video_decoder_configuration_record->width < 64 (%d), setting to 0!", video_decoder_configuration_record->width);
        video_decoder_configuration_record->width = 0;
    }

    if (video_decoder_configuration_record->height < 64) {
        _ATSC3_HEVC_NAL_EXTRACTOR_WARN("atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t: video_decoder_configuration_record->height < 64 (%d), setting to 0!", video_decoder_configuration_record->height);
        video_decoder_configuration_record->height = 0;
    }

//process as HEVC w/ hvcC match
    if (has_hvcC_match) {
        hevc_decoder_configuration_record_t *hevc_decoder_configuration_record = hevc_decoder_configuration_record_new();

//read our original box data by grabbing the previous uint32_t for box size (-4: hvcC, -4 size)
        uint32_t hvcc_box_size = 0;
        hvcc_box_size = ntohl(*((uint32_t *) (&mpu_ptr[hvcC_match_index - 8])));
        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: reading isobmff hvcc_box_size: %d", hvcc_box_size);

        if (hvcc_box_size > 8) {
            hevc_decoder_configuration_record->box_data_original = block_Duplicate_from_ptr(&mpu_ptr[hvcC_match_index],
                                                                                            hvcc_box_size -
                                                                                            8);
            block_Rewind(hevc_decoder_configuration_record->box_data_original);
        }

//todo: parse trailing 22 bytes after hvcC box name

//hack
        hvcC_match_index += 22;
//todo: parse out nals:
/*
		 * unsigned int(8) numOfArrays;
		 *
		 *  for (j=0; j < numOfArrays; j++) {

		 unsigned int(1) array_completeness;
		 bit(1) reserved = 0;
		 unsigned int(6) NAL_unit_type;

		 unsigned int(16) numNalus;
		 for (i=0; i< numNalus; i++) {
			 unsigned int(16) nalUnitLength;
			 bit(8*nalUnitLength) nalUnit;
			}
	 	 }
		 */

        hevc_decoder_configuration_record->num_of_arrays = mpu_ptr[hvcC_match_index++];
        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: num_of_arrays: %d", hevc_decoder_configuration_record->num_of_arrays);

        for (int i = 0; i < hevc_decoder_configuration_record->num_of_arrays; i++) {
            uint8_t temp_nal_header_byte = mpu_ptr[hvcC_match_index++];
            if ((temp_nal_header_byte & 0x40) != 0x00) {
                _ATSC3_HEVC_NAL_EXTRACTOR_WARN("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: HEVC: nal header bit 7 is not 0!");
            }

            uint8_t nal_unit_type = temp_nal_header_byte & 0x3F;

            uint16_t num_nauls = ntohs(*((uint16_t *) (&mpu_ptr[hvcC_match_index])));
            hvcC_match_index += 2;

            switch (nal_unit_type) {
                case VPS_NAL_unit_type:
                    for (int i = 0; i < num_nauls; i++) {
                        atsc3_nal_unit_vps_t *atsc3_nal_unit_vps = atsc3_nal_unit_vps_new();

                        atsc3_nal_unit_vps->nal_unit_length = ntohs(*((uint16_t *) (&mpu_ptr[hvcC_match_index])));
                        atsc3_nal_unit_vps->nal_unit = block_Alloc(atsc3_nal_unit_vps->nal_unit_length);

                        hvcC_match_index += 2;
                        for (int j = 0; j < atsc3_nal_unit_vps->nal_unit_length; j++) {
//hack
                            atsc3_nal_unit_vps->nal_unit->p_buffer[j] = mpu_ptr[hvcC_match_index++];
                        }
                        hevc_decoder_configuration_record_add_atsc3_nal_unit_vps(hevc_decoder_configuration_record, atsc3_nal_unit_vps);
                        _ATSC3_HEVC_NAL_EXTRACTOR_INFO("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: HEVC: adding VPS i: %d, nal_unit_length: %d, block_t size: %d", i, atsc3_nal_unit_vps->nal_unit_length, atsc3_nal_unit_vps->nal_unit->p_size);
                    }
                    break;

                case SPS_NAL_unit_type:
                    for (int i = 0; i < num_nauls; i++) {
                        atsc3_nal_unit_sps_t *atsc3_nal_unit_sps = atsc3_nal_unit_sps_new();

                        atsc3_nal_unit_sps->nal_unit_length = ntohs(*((uint16_t *) (&mpu_ptr[hvcC_match_index])));
                        atsc3_nal_unit_sps->nal_unit = block_Alloc(atsc3_nal_unit_sps->nal_unit_length);

                        hvcC_match_index += 2;
                        for (int j = 0; j < atsc3_nal_unit_sps->nal_unit_length; j++) {
//hack
                            atsc3_nal_unit_sps->nal_unit->p_buffer[j] = mpu_ptr[hvcC_match_index++];
                        }
                        hevc_decoder_configuration_record_add_atsc3_nal_unit_sps(hevc_decoder_configuration_record, atsc3_nal_unit_sps);
                        _ATSC3_HEVC_NAL_EXTRACTOR_INFO("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: HEVC: adding SPS i: %d, nal_unit_length: %d, block_t length: %d", i, atsc3_nal_unit_sps->nal_unit_length, atsc3_nal_unit_sps->nal_unit->p_size);

                    }
                    break;

                case PPS_NAL_unit_type:
                    for (int i = 0; i < num_nauls; i++) {
                        atsc3_nal_unit_pps_t *atsc3_nal_unit_pps = atsc3_nal_unit_pps_new();

                        atsc3_nal_unit_pps->nal_unit_length = ntohs(*((uint16_t *) (&mpu_ptr[hvcC_match_index])));
                        atsc3_nal_unit_pps->nal_unit = block_Alloc(atsc3_nal_unit_pps->nal_unit_length);

                        hvcC_match_index += 2;
                        for (int j = 0; j < atsc3_nal_unit_pps->nal_unit_length; j++) {
//hack
                            atsc3_nal_unit_pps->nal_unit->p_buffer[j] = mpu_ptr[hvcC_match_index++];
                        }
                        hevc_decoder_configuration_record_add_atsc3_nal_unit_pps(hevc_decoder_configuration_record, atsc3_nal_unit_pps);
                        _ATSC3_HEVC_NAL_EXTRACTOR_INFO("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: HEVC: adding PPS i: %d, nal_unit_length: %d, block_t length: %d", i, atsc3_nal_unit_pps->nal_unit_length, atsc3_nal_unit_pps->nal_unit->p_size);

                    }
                    break;

                default:
                    break;

            }
        }

        video_decoder_configuration_record->hevc_decoder_configuration_record = hevc_decoder_configuration_record;
        return video_decoder_configuration_record;
    }

//AVC1/avcC NAL processing
    if (has_avcC_match) {
/**
		 *  from https://github.com/aizvorski/h264bitstream/blob/master/h264_avcc.c
		 * 	avcc->configurationVersion = bs_read_u8(b);				//8
			  avcc->AVCProfileIndication = bs_read_u8(b);			//16
			  avcc->profile_compatibility = bs_read_u8(b);			//24
			  avcc->AVCLevelIndication = bs_read_u8(b);				//32

			  * int reserved = bs_read_u(b, 6); // '111111'b;		//40
			  avcc->lengthSizeMinusOne = bs_read_u(b, 2);

			  * int reserved = bs_read_u(b, 3); // '111'b;			//48 = 6 bytes
  	  	  	  avcc->numOfSequenceParameterSets = bs_read_u(b, 5);

		 */
        int avc_offset = avcC_match_index;

        avc1_decoder_configuration_record_t *avc1_decoder_configuration_record = avc1_decoder_configuration_record_new();
        avc1_decoder_configuration_record->configuration_version = mpu_ptr[avc_offset++];
        avc1_decoder_configuration_record->avc_profile_indication = mpu_ptr[avc_offset++];
        avc1_decoder_configuration_record->profile_compatibility = mpu_ptr[avc_offset++];
        avc1_decoder_configuration_record->avc_level_indication = mpu_ptr[avc_offset++];
        avc1_decoder_configuration_record->length_size_minus_one = mpu_ptr[avc_offset++];

        uint8_t num_of_sequence_parameter_sets_temp = mpu_ptr[avc_offset++];

        if ((num_of_sequence_parameter_sets_temp >> 5) != 0x7) { //3 MSBits set to '111' reserved
            _ATSC3_HEVC_NAL_EXTRACTOR_WARN("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: avc1: num_of_sequence_parameter_sets_temp 3 MSB != 111");
        }

//SPS count
        avc1_decoder_configuration_record->num_of_sequence_parameter_sets =
                num_of_sequence_parameter_sets_temp & 0x1F;
        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: avc1: num_of_sequence_parameter_sets: %u", avc1_decoder_configuration_record->num_of_sequence_parameter_sets);

//start parsing SPS here
        for (int i = 0;
                i < avc1_decoder_configuration_record->num_of_sequence_parameter_sets; i++) {
            atsc3_avc1_nal_unit_sps_t *atsc3_avc1_nal_unit_sps = atsc3_avc1_nal_unit_sps_new();
            atsc3_avc1_nal_unit_sps->nal_unit_length = ntohs(*((uint16_t *) (&mpu_ptr[avc_offset])));
            avc_offset += 2;

            atsc3_avc1_nal_unit_sps->nal_unit = block_Duplicate_from_ptr(&mpu_ptr[avc_offset], atsc3_avc1_nal_unit_sps->nal_unit_length);
            avc1_decoder_configuration_record_add_atsc3_avc1_nal_unit_sps(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_sps);
            avc_offset += atsc3_avc1_nal_unit_sps->nal_unit_length;

            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: avc1: adding SPS: %d, nal_length: %u, nal_unit->p_size: %d", i, atsc3_avc1_nal_unit_sps->nal_unit_length, atsc3_avc1_nal_unit_sps->nal_unit->p_size);
        }

//PPS count
        avc1_decoder_configuration_record->num_of_picture_parameter_sets = mpu_ptr[avc_offset++];
        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: avc1: num_of_picture_parameter_sets: %u", avc1_decoder_configuration_record->num_of_picture_parameter_sets);

//start parsing PPS here
        for (int i = 0;
                i < avc1_decoder_configuration_record->num_of_sequence_parameter_sets; i++) {
            atsc3_avc1_nal_unit_pps_t *atsc3_avc1_nal_unit_pps = atsc3_avc1_nal_unit_pps_new();
            atsc3_avc1_nal_unit_pps->nal_unit_length = ntohs(*((uint16_t *) (&mpu_ptr[avc_offset])));
            avc_offset += 2;

            atsc3_avc1_nal_unit_pps->nal_unit = block_Duplicate_from_ptr(&mpu_ptr[avc_offset], atsc3_avc1_nal_unit_pps->nal_unit_length);
            avc1_decoder_configuration_record_add_atsc3_avc1_nal_unit_pps(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_pps);
            avc_offset += atsc3_avc1_nal_unit_pps->nal_unit_length;

            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: avc1: adding pps: %d, nal_length: %u, nal_unit->p_size: %d", i, atsc3_avc1_nal_unit_pps->nal_unit_length, atsc3_avc1_nal_unit_pps->nal_unit->p_size);
        }

        video_decoder_configuration_record->avc1_decoder_configuration_record = avc1_decoder_configuration_record;
        return video_decoder_configuration_record;

    }


    _ATSC3_HEVC_NAL_EXTRACTOR_ERROR("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: error processing: ptr: %p, size: %d", mpu_metadata_block, mpu_metadata_block->p_size);

//hevc_decoder_configuration_record; //or avc1_decoder_configuration_record

    error:

_ATSC3_HEVC_NAL_EXTRACTOR_ERROR("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: error processing: ptr: %p, size: %d", mpu_metadata_block, mpu_metadata_block->p_size);

    if (video_decoder_configuration_record) {
        free(video_decoder_configuration_record);
        video_decoder_configuration_record = NULL;
    }

    return NULL;
}

void atsc3_avc1_decoder_configuration_record_dump(avc1_decoder_configuration_record_t *avc1_decoder_configuration_record) {
    _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_avc1_decoder_configuration_record_dump: avc1 config_record: %p", avc1_decoder_configuration_record);

}

void atsc3_hevc_decoder_configuration_record_dump(hevc_decoder_configuration_record_t *hevc_decoder_configuration_record) {
    _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_decoder_configuration_record_dump: hevc config_record: %p, nal_vps_count: %d, nal_sps_count: %d, nal_pps_count: %d", atsc3_hevc_decoder_configuration_record_dump, hevc_decoder_configuration_record->atsc3_nal_unit_vps_v.count, hevc_decoder_configuration_record->atsc3_nal_unit_sps_v.count, hevc_decoder_configuration_record->atsc3_nal_unit_pps_v.count);

    for (int i = 0; i < hevc_decoder_configuration_record->atsc3_nal_unit_vps_v.count; i++) {
        atsc3_nal_unit_vps_t *atsc3_nal_unit_vps = hevc_decoder_configuration_record->atsc3_nal_unit_vps_v.data[i];

//dump as hex, each byte of NAL will take 5 bytes for display + 1 byte null term
//0x00
//12345
        if (atsc3_nal_unit_vps->nal_unit_length) {
            uint32_t hex_dump_string_len = (atsc3_nal_unit_vps->nal_unit_length * 5) + 1;

            char *temp_nal_payload_string_base = calloc(hex_dump_string_len, sizeof(char));
            char *temp_nal_payload_string_append = temp_nal_payload_string_base;

            uint8_t *nal_ptr = block_Get(atsc3_nal_unit_vps->nal_unit);

            for (int j = 0; j < atsc3_nal_unit_vps->nal_unit_length; j++) {
                snprintf(temp_nal_payload_string_append, 6, "0x%02x ", nal_ptr[j]); //snprintf includes nul in the size
                temp_nal_payload_string_append += 5;
            }

            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("HEVC: hvcC: VPS %d, nal unit length is: %d, nal payload is:\n%s\n", i, atsc3_nal_unit_vps->nal_unit_length, temp_nal_payload_string_base);
            free(temp_nal_payload_string_base);
        } else {
            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("HEVC: hvcC: VPS %d, nal unit length is: 0!", i);
        }
    }

    for (int i = 0; i < hevc_decoder_configuration_record->atsc3_nal_unit_sps_v.count; i++) {
        atsc3_nal_unit_sps_t *atsc3_nal_unit_sps = hevc_decoder_configuration_record->atsc3_nal_unit_sps_v.data[i];

//dump as hex, each byte of NAL will take 5 bytes for display + 1 byte null term
//0x00
//12345
        if (atsc3_nal_unit_sps->nal_unit_length) {
            uint32_t hex_dump_string_len = (atsc3_nal_unit_sps->nal_unit_length * 5) + 1;

            char *temp_nal_payload_string_base = calloc(hex_dump_string_len, sizeof(char));
            char *temp_nal_payload_string_append = temp_nal_payload_string_base;

            uint8_t *nal_ptr = block_Get(atsc3_nal_unit_sps->nal_unit);

            for (int j = 0; j < atsc3_nal_unit_sps->nal_unit_length; j++) {
                snprintf(temp_nal_payload_string_append, 6, "0x%02x ", nal_ptr[j]); //snprintf includes nul in the size
                temp_nal_payload_string_append += 5;
            }

            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("HEVC: hvcC: SPS %d, nal unit length is: %d, nal payload is:\n%s\n", i, atsc3_nal_unit_sps->nal_unit_length, temp_nal_payload_string_base);
            free(temp_nal_payload_string_base);
        } else {
            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("HEVC: hvcC: SPS %d, nal unit length is: 0!", i);
        }
    }

    for (int i = 0; i < hevc_decoder_configuration_record->atsc3_nal_unit_pps_v.count; i++) {
        atsc3_nal_unit_pps_t *atsc3_nal_unit_pps = hevc_decoder_configuration_record->atsc3_nal_unit_pps_v.data[i];

//dump as hex, each byte of NAL will take 5 bytes for display + 1 byte null term
//0x00
//12345
        if (atsc3_nal_unit_pps->nal_unit_length) {
            uint32_t hex_dump_string_len = (atsc3_nal_unit_pps->nal_unit_length * 5) + 1;

            char *temp_nal_payload_string_base = calloc(hex_dump_string_len, sizeof(char));
            char *temp_nal_payload_string_append = temp_nal_payload_string_base;

            uint8_t *nal_ptr = block_Get(atsc3_nal_unit_pps->nal_unit);

            for (int j = 0; j < atsc3_nal_unit_pps->nal_unit_length; j++) {
                snprintf(temp_nal_payload_string_append, 6, "0x%02x ", nal_ptr[j]); //snprintf includes nul in the size
                temp_nal_payload_string_append += 5;
            }

            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("HEVC: hvcC: PPS %d, nal unit length is: %d, nal payload is:\n%s\n", i, atsc3_nal_unit_pps->nal_unit_length, temp_nal_payload_string_base);
            free(temp_nal_payload_string_base);
        } else {
            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("HEVC: hvcC: PPS %d, nal unit length is: 0!", i);
        }
    }
}

/*

 aligned(8) class TrackHeaderBox
    extends FullBox(‘tkhd’, version, flags){
    if (version==1) {
       unsigned int(64)  creation_time;
       unsigned int(64)  modification_time;
       unsigned int(32)  track_ID;
       const unsigned int(32)  reserved = 0;
       unsigned int(64)  duration;
    } else { // version==0
       unsigned int(32)  creation_time;
       unsigned int(32)  modification_time;
       unsigned int(32)  track_ID;
       const unsigned int(32)  reserved = 0;
       unsigned int(32)  duration;
    }
    const unsigned int(32)[2]  reserved = 0;
    template int(16) layer = 0;
    template int(16) alternate_group = 0;
    template int(16)  volume = {if track_is_audio 0x0100 else 0};
    const unsigned int(16)  reserved = 0;
    template int(32)[9]  matrix=
       { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 };
       // unity matrix
    unsigned int(32) width;
    unsigned int(32) height;
 }
 aligned(8) class FullBox(unsigned int(32) boxtype, unsigned int(8) v, bit(24) f) extends Box(boxtype) {
     unsigned int(8)   version = v;
     bit(24)           flags = f;
 }


 width and height fixed‐point 16.16 values are track‐dependent as follows:

 For text and subtitle tracks, they may, depending on the coding format, describe the suggested size of the rendering area. For such tracks, the value 0x0 may also be used to indicate that the data may be rendered at any size, that no preferred size has been indicated and that the actual size may be determined by the external context or by reusing the width and height of another track. For those tracks, the flag track_size_is_aspect_ratio may also be used.
 For non‐visual tracks (e.g. audio), they should be set to zero.
 For all other tracks, they specify the track's visual presentation size.

 These need not be the same as the pixel dimensions of the images,
 which is documented in the sample description(s);
 all images in the sequence are scaled to this size,
 before any overall transformation of the track represented by the matrix.

 The pixel dimensions of the images are the default values.

 */

void atsc3_init_parse_tkhd_for_width_height(video_decoder_configuration_record_t *video_decoder_configuration_record, uint8_t *tkhd_ptr_start, uint32_t init_buff_remaining) {
    uint8_t version = *tkhd_ptr_start++;
    tkhd_ptr_start += 3;  //complete fullbox
    if (version == 1) {
//walk past 64+64+32+32+64 = 256 bits -> 32 bytes
        tkhd_ptr_start += 32;
    } else {
//walk pask 32+32+32+32+32 = 160 bits -> 20 bytes
        tkhd_ptr_start += 20;
    }
    tkhd_ptr_start += 8; //past reserved
//16+16+16+16 + (32*9) = 64 + 288 = 352 -> 44
    tkhd_ptr_start += 44;

    uint32_t width = ntohs(*((uint16_t *) (tkhd_ptr_start)));
    tkhd_ptr_start += 4;
    uint32_t height = ntohs(*(uint16_t *) (tkhd_ptr_start));

    _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_init_parse_tkhd_for_width_height, got width: %d, height: %d", width, height);
    video_decoder_configuration_record->width = width;
    video_decoder_configuration_record->height = height;
}

//try and extract from hev1 box - HEVCConfigurationBox
/*
     * type Avc1Box struct {
            Version              uint16
            RevisionLevel        uint16
            Vendor               uint32
            TemporalQuality      uint32
            SpacialQuality       uint32
            ---------------		 ------  128 bits: 16 bytes
            Width                uint16
            Height               uint16
            HorizontalResolution uint32
            VerticalResolution   uint32
            EntryDataSize        uint32
            FramesPerSample      uint16
            CompressorName       [32]byte
            BitDepth             uint16
            ColorTableIndex      int16
            // contains filtered or unexported fields
        }
     */

void atsc3_init_parse_HEVCConfigurationBox_for_width_height(video_decoder_configuration_record_t *video_decoder_configuration_record, uint8_t *configurationBox_ptr_start, uint32_t init_buff_remaining) {
    configurationBox_ptr_start += 8 + 16; //fullbox + avc1 box struct internals

    uint16_t width = ntohs(*((uint16_t *) (configurationBox_ptr_start)));
    configurationBox_ptr_start += 2;
    uint16_t height = ntohs(*(uint16_t *) (configurationBox_ptr_start));

    if (width >= 64 && height >= 64) {
        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("c, got width: %d, height: %d", width, height);
        video_decoder_configuration_record->width = width;
        video_decoder_configuration_record->height = height;
    } else {
        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_init_parse_HEVCConfigurationBox_for_width_height, discarding invalid w/h: width: %d, height: %d", width, height);
    }
}

block_t *atsc3_hevc_decoder_configuration_record_get_nals_vps_combined_optional_start_code(hevc_decoder_configuration_record_t *hevc_decoder_configuration_record, bool include_nal_start_code) {
    _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_decoder_configuration_record_get_vps_nals_combined: hevc config_record: %p, nal_vps_count: %d", atsc3_hevc_decoder_configuration_record_dump, hevc_decoder_configuration_record->atsc3_nal_unit_vps_v.count);

    block_t *nals_vps_combined = NULL;

    for (int i = 0; i < hevc_decoder_configuration_record->atsc3_nal_unit_vps_v.count; i++) {
        atsc3_nal_unit_vps_t *atsc3_nal_unit_vps = hevc_decoder_configuration_record->atsc3_nal_unit_vps_v.data[i];

        if (atsc3_nal_unit_vps->nal_unit) {
            if (!nals_vps_combined) {
                nals_vps_combined = block_Duplicate(atsc3_nal_unit_vps->nal_unit);
            } else {
                block_Merge(nals_vps_combined, atsc3_nal_unit_vps->nal_unit);
            }
        }
    }

    if (nals_vps_combined) {
        if (include_nal_start_code) {
            uint8_t *out_p = NULL;
            int out_size = 0;

            int ret = atsc3_ffmpeg_h2645_ps_to_nalu(nals_vps_combined->p_buffer, nals_vps_combined->p_size, &out_p, &out_size);
            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_decoder_configuration_record_get_vps_nals_combined: ret: %d, old p: %p, size: %d, new p: %p, size: %d", ret, nals_vps_combined->p_buffer, nals_vps_combined->p_size, out_p, out_size);
            if (!ret) {
                block_Destroy(&nals_vps_combined);
                nals_vps_combined = block_Duplicate_from_ptr(out_p, out_size);
                _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("nals_pps_combined: %p", nals_vps_combined->p_buffer);
                free(out_p);
            }
        }

        block_Rewind(nals_vps_combined);
        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_decoder_configuration_record_get_vps_nals_combined: hevc config_record: %p, nal_vps_count: %d, nals_vps_combined: %p, nals_vps_combined->p_buffer: %p, nals_vps_combined->i_pos: %d, nals_vps_combined->p_size: %d", atsc3_hevc_decoder_configuration_record_dump, hevc_decoder_configuration_record->atsc3_nal_unit_vps_v.count, nals_vps_combined, nals_vps_combined->p_buffer, nals_vps_combined->i_pos, nals_vps_combined->p_size);
    } else {
        _ATSC3_HEVC_NAL_EXTRACTOR_WARN("atsc3_hevc_decoder_configuration_record_get_vps_nals_combined: hevc config_record: %p, nals_vps_combined is NULL!", hevc_decoder_configuration_record);
    }

    return nals_vps_combined;
}

block_t *atsc3_hevc_decoder_configuration_record_get_nals_sps_combined_optional_start_code(hevc_decoder_configuration_record_t *hevc_decoder_configuration_record, bool include_nal_start_code) {
    _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_decoder_configuration_record_get_sps_nals_combined: hevc config_record: %p, nal_sps_count: %d", atsc3_hevc_decoder_configuration_record_dump, hevc_decoder_configuration_record->atsc3_nal_unit_sps_v.count);

    block_t *nals_sps_combined = NULL;

    for (int i = 0; i < hevc_decoder_configuration_record->atsc3_nal_unit_sps_v.count; i++) {
        atsc3_nal_unit_sps_t *atsc3_nal_unit_vps = hevc_decoder_configuration_record->atsc3_nal_unit_sps_v.data[i];

        if (atsc3_nal_unit_vps->nal_unit) {
            if (!nals_sps_combined) {
                nals_sps_combined = block_Duplicate(atsc3_nal_unit_vps->nal_unit);
            } else {
                block_Merge(nals_sps_combined, atsc3_nal_unit_vps->nal_unit);
            }
        }
    }

    if (nals_sps_combined) {

        if (include_nal_start_code) {
            uint8_t *out_p = NULL;
            int out_size = 0;

            int ret = atsc3_ffmpeg_h2645_ps_to_nalu(nals_sps_combined->p_buffer, nals_sps_combined->p_size, &out_p, &out_size);
            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_decoder_configuration_record_get_sps_nals_combined: ret: %d, old p: %p, size: %d, new p: %p, size: %d", ret, nals_sps_combined->p_buffer, nals_sps_combined->p_size, out_p, out_size);
            if (!ret) {
                block_Destroy(&nals_sps_combined);
                nals_sps_combined = block_Duplicate_from_ptr(out_p, out_size);
                _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("nals_sps_combined: %p", nals_sps_combined->p_buffer);
                free(out_p);
            }
        }

        block_Rewind(nals_sps_combined);
        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_decoder_configuration_record_get_sps_nals_combined: hevc config_record: %p, nal_sps_count: %d, nals_sps_combined: %p, nals_sps_combined->p_buffer: %p, nals_sps_combined->i_pos: %d, nals_sps_combined->p_size: %d", atsc3_hevc_decoder_configuration_record_dump, hevc_decoder_configuration_record->atsc3_nal_unit_sps_v.count, nals_sps_combined, nals_sps_combined->p_buffer, nals_sps_combined->i_pos, nals_sps_combined->p_size);
    } else {
        _ATSC3_HEVC_NAL_EXTRACTOR_WARN("atsc3_hevc_decoder_configuration_record_get_sps_nals_combined: hevc config_record: %p, nals_sps_combined is NULL!", hevc_decoder_configuration_record);
    }

    return nals_sps_combined;
}

block_t *atsc3_hevc_decoder_configuration_record_get_nals_pps_combined_optional_start_code(hevc_decoder_configuration_record_t *hevc_decoder_configuration_record, bool include_nal_start_code) {
    _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_decoder_configuration_record_get_pps_nals_combined: hevc config_record: %p, nal_pps_count: %d", atsc3_hevc_decoder_configuration_record_dump, hevc_decoder_configuration_record->atsc3_nal_unit_pps_v.count);

    block_t *nals_pps_combined = NULL;

    for (int i = 0; i < hevc_decoder_configuration_record->atsc3_nal_unit_pps_v.count; i++) {
        atsc3_nal_unit_pps_t *atsc3_nal_unit_vps = hevc_decoder_configuration_record->atsc3_nal_unit_pps_v.data[i];
        if (atsc3_nal_unit_vps->nal_unit) {
            if (!nals_pps_combined) {
                nals_pps_combined = block_Duplicate(atsc3_nal_unit_vps->nal_unit);
            } else {
                block_Merge(nals_pps_combined, atsc3_nal_unit_vps->nal_unit);
            }
        }
    }

    if (nals_pps_combined) {
        if (include_nal_start_code) {
            uint8_t *out_p = NULL;
            int out_size = 0;

            int ret = atsc3_ffmpeg_h2645_ps_to_nalu(nals_pps_combined->p_buffer, nals_pps_combined->p_size, &out_p, &out_size);
            _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_decoder_configuration_record_get_pps_nals_combined: ret: %d, old p: %p, size: %d, new p: %p, size: %d", ret, nals_pps_combined->p_buffer, nals_pps_combined->p_size, out_p, out_size);
            if (!ret) {
                block_Destroy(&nals_pps_combined);
                nals_pps_combined = block_Duplicate_from_ptr(out_p, out_size);
                _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("nals_pps_combined: %p", nals_pps_combined->p_buffer);
                free(out_p);
            }
        }

        block_Rewind(nals_pps_combined);
        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_decoder_configuration_record_get_pps_nals_combined: hevc config_record: %p, nal_pps_count: %d, nals_pps_combined: %p, nals_pps_combined->p_buffer: %p, nals_pps_combined->i_pos: %d, nals_pps_combined->p_size: %d", atsc3_hevc_decoder_configuration_record_dump, hevc_decoder_configuration_record->atsc3_nal_unit_pps_v.count, nals_pps_combined, nals_pps_combined->p_buffer, nals_pps_combined->i_pos, nals_pps_combined->p_size);
    } else {
        _ATSC3_HEVC_NAL_EXTRACTOR_WARN("atsc3_hevc_decoder_configuration_record_get_pps_nals_combined: hevc config_record: %p, nals_pps_combined is NULL!", hevc_decoder_configuration_record);
    }

    return nals_pps_combined;
}


//by default, return start NAL code
block_t *atsc3_hevc_decoder_configuration_record_get_nals_vps_combined(hevc_decoder_configuration_record_t *hevc_decoder_configuration_record) {
    return atsc3_hevc_decoder_configuration_record_get_nals_vps_combined_optional_start_code(hevc_decoder_configuration_record, true);
}

block_t *atsc3_hevc_decoder_configuration_record_get_nals_sps_combined(hevc_decoder_configuration_record_t *hevc_decoder_configuration_record) {
    return atsc3_hevc_decoder_configuration_record_get_nals_sps_combined_optional_start_code(hevc_decoder_configuration_record, true);
}

block_t *atsc3_hevc_decoder_configuration_record_get_nals_pps_combined(hevc_decoder_configuration_record_t *hevc_decoder_configuration_record) {
    return atsc3_hevc_decoder_configuration_record_get_nals_pps_combined_optional_start_code(hevc_decoder_configuration_record, true);
}


int atsc3_ffmpeg_h2645_ps_to_nalu(const uint8_t *src, int src_size, uint8_t **out, int *out_size) {
    int i;
    int ret = 0;
    uint8_t *p = NULL;
    static const uint8_t nalu_header[] = {0x00, 0x00, 0x00, 0x01};

    if (!out || !out_size) {
        return -1;

    }

    p = calloc(sizeof(nalu_header) + src_size, sizeof(uint8_t));
    if (!p) {
        return -1;
    }

    *out = p;
    *out_size = sizeof(nalu_header) + src_size;

    memcpy(p, nalu_header, sizeof(nalu_header));
    memcpy(p + sizeof(nalu_header), src, src_size);

/* Escape 0x00, 0x00, 0x0{0-3} pattern */
    for (i = 4; i < *out_size; i++) {
        if (i < *out_size - 3 && p[i + 0] == 0 && p[i + 1] == 0 && p[i + 2] <= 3) {
            uint8_t *new;

            *out_size += 1;
            new = realloc(*out, *out_size);
            if (!new) {
                ret = -1;
                goto done;
            }
            *out = p = new;

//double appending end-code?
            i = i + 2;
            memmove(p + i + 1, p + i, *out_size - (i + 1));
            p[i] = 0x03;
        }
    }
    done:
    if (ret < 0) {
        free(out);
        *out_size = 0;
    }

    return ret;
}


void atsc3_hevc_nals_record_dump(const char *label, block_t *block) {
    _ATSC3_HEVC_NAL_EXTRACTOR_TRACE("atsc3_hevc_nals_record_dump: %s: ptr: %p, start: %d, len: %d", label, block->p_buffer, block->i_pos, block->p_size);
    uint32_t hex_dump_string_len = (block->p_size * 5) + 1;
    char *temp_nal_payload_string_base = calloc(hex_dump_string_len, sizeof(char));
    char *temp_nal_payload_string_append = temp_nal_payload_string_base;

    for (int i = 0; i < block->p_size; i++) {
        snprintf(temp_nal_payload_string_append, 6, "0x%02x ", block->p_buffer[i]); //snprintf includes nul in the size
        temp_nal_payload_string_append += 5;
    }

    if (block->p_size) {
        _ATSC3_HEVC_NAL_EXTRACTOR_DEBUG("atsc3_hevc_nals_record_dump: %s: ptr: %p, start: %d, len: %d, nals:\n\n%s\n", label, block->p_buffer, block->i_pos, block->p_size, temp_nal_payload_string_base);
        free(temp_nal_payload_string_base);
    }
}


/**
 jjustman-2019-10-12: large copy/paste/re-factor warning
 TODO: fix me by linking properly against libavcodec!



 */



//for libavcodec/ffmpeg SXS nals extraction
#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>

///end SXS


//////////


#define av_log(...)
#define ff_dlog(...)

#define av_restrict restrict

enum AVCodecID {
    AV_CODEC_ID_NONE,

/* video codecs */
    AV_CODEC_ID_MPEG1VIDEO,
    AV_CODEC_ID_MPEG2VIDEO, ///< preferred ID for MPEG-1/2 video decoding
    AV_CODEC_ID_H261,
    AV_CODEC_ID_H263,
    AV_CODEC_ID_RV10,
    AV_CODEC_ID_RV20,
    AV_CODEC_ID_MJPEG,
    AV_CODEC_ID_MJPEGB,
    AV_CODEC_ID_LJPEG,
    AV_CODEC_ID_SP5X,
    AV_CODEC_ID_JPEGLS,
    AV_CODEC_ID_MPEG4,
    AV_CODEC_ID_RAWVIDEO,
    AV_CODEC_ID_MSMPEG4V1,
    AV_CODEC_ID_MSMPEG4V2,
    AV_CODEC_ID_MSMPEG4V3,
    AV_CODEC_ID_WMV1,
    AV_CODEC_ID_WMV2,
    AV_CODEC_ID_H263P,
    AV_CODEC_ID_H263I,
    AV_CODEC_ID_FLV1,
    AV_CODEC_ID_SVQ1,
    AV_CODEC_ID_SVQ3,
    AV_CODEC_ID_DVVIDEO,
    AV_CODEC_ID_HUFFYUV,
    AV_CODEC_ID_CYUV,
    AV_CODEC_ID_H264,
    AV_CODEC_ID_INDEO3,
    AV_CODEC_ID_VP3,
    AV_CODEC_ID_THEORA,
    AV_CODEC_ID_ASV1,
    AV_CODEC_ID_ASV2,
    AV_CODEC_ID_FFV1,
    AV_CODEC_ID_4XM,
    AV_CODEC_ID_VCR1,
    AV_CODEC_ID_CLJR,
    AV_CODEC_ID_MDEC,
    AV_CODEC_ID_ROQ,
    AV_CODEC_ID_INTERPLAY_VIDEO,
    AV_CODEC_ID_XAN_WC3,
    AV_CODEC_ID_XAN_WC4,
    AV_CODEC_ID_RPZA,
    AV_CODEC_ID_CINEPAK,
    AV_CODEC_ID_WS_VQA,
    AV_CODEC_ID_MSRLE,
    AV_CODEC_ID_MSVIDEO1,
    AV_CODEC_ID_IDCIN,
    AV_CODEC_ID_8BPS,
    AV_CODEC_ID_SMC,
    AV_CODEC_ID_FLIC,
    AV_CODEC_ID_TRUEMOTION1,
    AV_CODEC_ID_VMDVIDEO,
    AV_CODEC_ID_MSZH,
    AV_CODEC_ID_ZLIB,
    AV_CODEC_ID_QTRLE,
    AV_CODEC_ID_TSCC,
    AV_CODEC_ID_ULTI,
    AV_CODEC_ID_QDRAW,
    AV_CODEC_ID_VIXL,
    AV_CODEC_ID_QPEG,
    AV_CODEC_ID_PNG,
    AV_CODEC_ID_PPM,
    AV_CODEC_ID_PBM,
    AV_CODEC_ID_PGM,
    AV_CODEC_ID_PGMYUV,
    AV_CODEC_ID_PAM,
    AV_CODEC_ID_FFVHUFF,
    AV_CODEC_ID_RV30,
    AV_CODEC_ID_RV40,
    AV_CODEC_ID_VC1,
    AV_CODEC_ID_WMV3,
    AV_CODEC_ID_LOCO,
    AV_CODEC_ID_WNV1,
    AV_CODEC_ID_AASC,
    AV_CODEC_ID_INDEO2,
    AV_CODEC_ID_FRAPS,
    AV_CODEC_ID_TRUEMOTION2,
    AV_CODEC_ID_BMP,
    AV_CODEC_ID_CSCD,
    AV_CODEC_ID_MMVIDEO,
    AV_CODEC_ID_ZMBV,
    AV_CODEC_ID_AVS,
    AV_CODEC_ID_SMACKVIDEO,
    AV_CODEC_ID_NUV,
    AV_CODEC_ID_KMVC,
    AV_CODEC_ID_FLASHSV,
    AV_CODEC_ID_CAVS,
    AV_CODEC_ID_JPEG2000,
    AV_CODEC_ID_VMNC,
    AV_CODEC_ID_VP5,
    AV_CODEC_ID_VP6,
    AV_CODEC_ID_VP6F,
    AV_CODEC_ID_TARGA,
    AV_CODEC_ID_DSICINVIDEO,
    AV_CODEC_ID_TIERTEXSEQVIDEO,
    AV_CODEC_ID_TIFF,
    AV_CODEC_ID_GIF,
    AV_CODEC_ID_DXA,
    AV_CODEC_ID_DNXHD,
    AV_CODEC_ID_THP,
    AV_CODEC_ID_SGI,
    AV_CODEC_ID_C93,
    AV_CODEC_ID_BETHSOFTVID,
    AV_CODEC_ID_PTX,
    AV_CODEC_ID_TXD,
    AV_CODEC_ID_VP6A,
    AV_CODEC_ID_AMV,
    AV_CODEC_ID_VB,
    AV_CODEC_ID_PCX,
    AV_CODEC_ID_SUNRAST,
    AV_CODEC_ID_INDEO4,
    AV_CODEC_ID_INDEO5,
    AV_CODEC_ID_MIMIC,
    AV_CODEC_ID_RL2,
    AV_CODEC_ID_ESCAPE124,
    AV_CODEC_ID_DIRAC,
    AV_CODEC_ID_BFI,
    AV_CODEC_ID_CMV,
    AV_CODEC_ID_MOTIONPIXELS,
    AV_CODEC_ID_TGV,
    AV_CODEC_ID_TGQ,
    AV_CODEC_ID_TQI,
    AV_CODEC_ID_AURA,
    AV_CODEC_ID_AURA2,
    AV_CODEC_ID_V210X,
    AV_CODEC_ID_TMV,
    AV_CODEC_ID_V210,
    AV_CODEC_ID_DPX,
    AV_CODEC_ID_MAD,
    AV_CODEC_ID_FRWU,
    AV_CODEC_ID_FLASHSV2,
    AV_CODEC_ID_CDGRAPHICS,
    AV_CODEC_ID_R210,
    AV_CODEC_ID_ANM,
    AV_CODEC_ID_BINKVIDEO,
    AV_CODEC_ID_IFF_ILBM,
#define AV_CODEC_ID_IFF_BYTERUN1 AV_CODEC_ID_IFF_ILBM
    AV_CODEC_ID_KGV1,
    AV_CODEC_ID_YOP,
    AV_CODEC_ID_VP8,
    AV_CODEC_ID_PICTOR,
    AV_CODEC_ID_ANSI,
    AV_CODEC_ID_A64_MULTI,
    AV_CODEC_ID_A64_MULTI5,
    AV_CODEC_ID_R10K,
    AV_CODEC_ID_MXPEG,
    AV_CODEC_ID_LAGARITH,
    AV_CODEC_ID_PRORES,
    AV_CODEC_ID_JV,
    AV_CODEC_ID_DFA,
    AV_CODEC_ID_WMV3IMAGE,
    AV_CODEC_ID_VC1IMAGE,
    AV_CODEC_ID_UTVIDEO,
    AV_CODEC_ID_BMV_VIDEO,
    AV_CODEC_ID_VBLE,
    AV_CODEC_ID_DXTORY,
    AV_CODEC_ID_V410,
    AV_CODEC_ID_XWD,
    AV_CODEC_ID_CDXL,
    AV_CODEC_ID_XBM,
    AV_CODEC_ID_ZEROCODEC,
    AV_CODEC_ID_MSS1,
    AV_CODEC_ID_MSA1,
    AV_CODEC_ID_TSCC2,
    AV_CODEC_ID_MTS2,
    AV_CODEC_ID_CLLC,
    AV_CODEC_ID_MSS2,
    AV_CODEC_ID_VP9,
    AV_CODEC_ID_AIC,
    AV_CODEC_ID_ESCAPE130,
    AV_CODEC_ID_G2M,
    AV_CODEC_ID_WEBP,
    AV_CODEC_ID_HNM4_VIDEO,
    AV_CODEC_ID_HEVC,
#define AV_CODEC_ID_H265 AV_CODEC_ID_HEVC
    AV_CODEC_ID_FIC,
    AV_CODEC_ID_ALIAS_PIX,
    AV_CODEC_ID_BRENDER_PIX,
    AV_CODEC_ID_PAF_VIDEO,
    AV_CODEC_ID_EXR,
    AV_CODEC_ID_VP7,
    AV_CODEC_ID_SANM,
    AV_CODEC_ID_SGIRLE,
    AV_CODEC_ID_MVC1,
    AV_CODEC_ID_MVC2,
    AV_CODEC_ID_HQX,
    AV_CODEC_ID_TDSC,
    AV_CODEC_ID_HQ_HQA,
    AV_CODEC_ID_HAP,
    AV_CODEC_ID_DDS,
    AV_CODEC_ID_DXV,
    AV_CODEC_ID_SCREENPRESSO,
    AV_CODEC_ID_RSCC,
    AV_CODEC_ID_AVS2,

    AV_CODEC_ID_Y41P = 0x8000,
    AV_CODEC_ID_AVRP,
    AV_CODEC_ID_012V,
    AV_CODEC_ID_AVUI,
    AV_CODEC_ID_AYUV,
    AV_CODEC_ID_TARGA_Y216,
    AV_CODEC_ID_V308,
    AV_CODEC_ID_V408,
    AV_CODEC_ID_YUV4,
    AV_CODEC_ID_AVRN,
    AV_CODEC_ID_CPIA,
    AV_CODEC_ID_XFACE,
    AV_CODEC_ID_SNOW,
    AV_CODEC_ID_SMVJPEG,
    AV_CODEC_ID_APNG,
    AV_CODEC_ID_DAALA,
    AV_CODEC_ID_CFHD,
    AV_CODEC_ID_TRUEMOTION2RT,
    AV_CODEC_ID_M101,
    AV_CODEC_ID_MAGICYUV,
    AV_CODEC_ID_SHEERVIDEO,
    AV_CODEC_ID_YLC,
    AV_CODEC_ID_PSD,
    AV_CODEC_ID_PIXLET,
    AV_CODEC_ID_SPEEDHQ,
    AV_CODEC_ID_FMVC,
    AV_CODEC_ID_SCPR,
    AV_CODEC_ID_CLEARVIDEO,
    AV_CODEC_ID_XPM,
    AV_CODEC_ID_AV1,
    AV_CODEC_ID_BITPACKED,
    AV_CODEC_ID_MSCC,
    AV_CODEC_ID_SRGC,
    AV_CODEC_ID_SVG,
    AV_CODEC_ID_GDV,
    AV_CODEC_ID_FITS,
    AV_CODEC_ID_IMM4,
    AV_CODEC_ID_PROSUMER,
    AV_CODEC_ID_MWSC,
    AV_CODEC_ID_WCMV,
    AV_CODEC_ID_RASC,
    AV_CODEC_ID_HYMT,
    AV_CODEC_ID_ARBC,
    AV_CODEC_ID_AGM,
    AV_CODEC_ID_LSCR,
    AV_CODEC_ID_VP4,
    AV_CODEC_ID_IMM5,

/* various PCM "codecs" */
    AV_CODEC_ID_FIRST_AUDIO = 0x10000,     ///< A dummy id pointing at the start of audio codecs
    AV_CODEC_ID_PCM_S16LE = 0x10000,
    AV_CODEC_ID_PCM_S16BE,
    AV_CODEC_ID_PCM_U16LE,
    AV_CODEC_ID_PCM_U16BE,
    AV_CODEC_ID_PCM_S8,
    AV_CODEC_ID_PCM_U8,
    AV_CODEC_ID_PCM_MULAW,
    AV_CODEC_ID_PCM_ALAW,
    AV_CODEC_ID_PCM_S32LE,
    AV_CODEC_ID_PCM_S32BE,
    AV_CODEC_ID_PCM_U32LE,
    AV_CODEC_ID_PCM_U32BE,
    AV_CODEC_ID_PCM_S24LE,
    AV_CODEC_ID_PCM_S24BE,
    AV_CODEC_ID_PCM_U24LE,
    AV_CODEC_ID_PCM_U24BE,
    AV_CODEC_ID_PCM_S24DAUD,
    AV_CODEC_ID_PCM_ZORK,
    AV_CODEC_ID_PCM_S16LE_PLANAR,
    AV_CODEC_ID_PCM_DVD,
    AV_CODEC_ID_PCM_F32BE,
    AV_CODEC_ID_PCM_F32LE,
    AV_CODEC_ID_PCM_F64BE,
    AV_CODEC_ID_PCM_F64LE,
    AV_CODEC_ID_PCM_BLURAY,
    AV_CODEC_ID_PCM_LXF,
    AV_CODEC_ID_S302M,
    AV_CODEC_ID_PCM_S8_PLANAR,
    AV_CODEC_ID_PCM_S24LE_PLANAR,
    AV_CODEC_ID_PCM_S32LE_PLANAR,
    AV_CODEC_ID_PCM_S16BE_PLANAR,

    AV_CODEC_ID_PCM_S64LE = 0x10800,
    AV_CODEC_ID_PCM_S64BE,
    AV_CODEC_ID_PCM_F16LE,
    AV_CODEC_ID_PCM_F24LE,
    AV_CODEC_ID_PCM_VIDC,

/* various ADPCM codecs */
    AV_CODEC_ID_ADPCM_IMA_QT = 0x11000,
    AV_CODEC_ID_ADPCM_IMA_WAV,
    AV_CODEC_ID_ADPCM_IMA_DK3,
    AV_CODEC_ID_ADPCM_IMA_DK4,
    AV_CODEC_ID_ADPCM_IMA_WS,
    AV_CODEC_ID_ADPCM_IMA_SMJPEG,
    AV_CODEC_ID_ADPCM_MS,
    AV_CODEC_ID_ADPCM_4XM,
    AV_CODEC_ID_ADPCM_XA,
    AV_CODEC_ID_ADPCM_ADX,
    AV_CODEC_ID_ADPCM_EA,
    AV_CODEC_ID_ADPCM_G726,
    AV_CODEC_ID_ADPCM_CT,
    AV_CODEC_ID_ADPCM_SWF,
    AV_CODEC_ID_ADPCM_YAMAHA,
    AV_CODEC_ID_ADPCM_SBPRO_4,
    AV_CODEC_ID_ADPCM_SBPRO_3,
    AV_CODEC_ID_ADPCM_SBPRO_2,
    AV_CODEC_ID_ADPCM_THP,
    AV_CODEC_ID_ADPCM_IMA_AMV,
    AV_CODEC_ID_ADPCM_EA_R1,
    AV_CODEC_ID_ADPCM_EA_R3,
    AV_CODEC_ID_ADPCM_EA_R2,
    AV_CODEC_ID_ADPCM_IMA_EA_SEAD,
    AV_CODEC_ID_ADPCM_IMA_EA_EACS,
    AV_CODEC_ID_ADPCM_EA_XAS,
    AV_CODEC_ID_ADPCM_EA_MAXIS_XA,
    AV_CODEC_ID_ADPCM_IMA_ISS,
    AV_CODEC_ID_ADPCM_G722,
    AV_CODEC_ID_ADPCM_IMA_APC,
    AV_CODEC_ID_ADPCM_VIMA,

    AV_CODEC_ID_ADPCM_AFC = 0x11800,
    AV_CODEC_ID_ADPCM_IMA_OKI,
    AV_CODEC_ID_ADPCM_DTK,
    AV_CODEC_ID_ADPCM_IMA_RAD,
    AV_CODEC_ID_ADPCM_G726LE,
    AV_CODEC_ID_ADPCM_THP_LE,
    AV_CODEC_ID_ADPCM_PSX,
    AV_CODEC_ID_ADPCM_AICA,
    AV_CODEC_ID_ADPCM_IMA_DAT4,
    AV_CODEC_ID_ADPCM_MTAF,
    AV_CODEC_ID_ADPCM_AGM,

/* AMR */
    AV_CODEC_ID_AMR_NB = 0x12000,
    AV_CODEC_ID_AMR_WB,

/* RealAudio codecs*/
    AV_CODEC_ID_RA_144 = 0x13000,
    AV_CODEC_ID_RA_288,

/* various DPCM codecs */
    AV_CODEC_ID_ROQ_DPCM = 0x14000,
    AV_CODEC_ID_INTERPLAY_DPCM,
    AV_CODEC_ID_XAN_DPCM,
    AV_CODEC_ID_SOL_DPCM,

    AV_CODEC_ID_SDX2_DPCM = 0x14800,
    AV_CODEC_ID_GREMLIN_DPCM,

/* audio codecs */
    AV_CODEC_ID_MP2 = 0x15000,
    AV_CODEC_ID_MP3, ///< preferred ID for decoding MPEG audio layer 1, 2 or 3
    AV_CODEC_ID_AAC,
    AV_CODEC_ID_AC3,
    AV_CODEC_ID_DTS,
    AV_CODEC_ID_VORBIS,
    AV_CODEC_ID_DVAUDIO,
    AV_CODEC_ID_WMAV1,
    AV_CODEC_ID_WMAV2,
    AV_CODEC_ID_MACE3,
    AV_CODEC_ID_MACE6,
    AV_CODEC_ID_VMDAUDIO,
    AV_CODEC_ID_FLAC,
    AV_CODEC_ID_MP3ADU,
    AV_CODEC_ID_MP3ON4,
    AV_CODEC_ID_SHORTEN,
    AV_CODEC_ID_ALAC,
    AV_CODEC_ID_WESTWOOD_SND1,
    AV_CODEC_ID_GSM, ///< as in Berlin toast format
    AV_CODEC_ID_QDM2,
    AV_CODEC_ID_COOK,
    AV_CODEC_ID_TRUESPEECH,
    AV_CODEC_ID_TTA,
    AV_CODEC_ID_SMACKAUDIO,
    AV_CODEC_ID_QCELP,
    AV_CODEC_ID_WAVPACK,
    AV_CODEC_ID_DSICINAUDIO,
    AV_CODEC_ID_IMC,
    AV_CODEC_ID_MUSEPACK7,
    AV_CODEC_ID_MLP,
    AV_CODEC_ID_GSM_MS, /* as found in WAV */
    AV_CODEC_ID_ATRAC3,
    AV_CODEC_ID_APE,
    AV_CODEC_ID_NELLYMOSER,
    AV_CODEC_ID_MUSEPACK8,
    AV_CODEC_ID_SPEEX,
    AV_CODEC_ID_WMAVOICE,
    AV_CODEC_ID_WMAPRO,
    AV_CODEC_ID_WMALOSSLESS,
    AV_CODEC_ID_ATRAC3P,
    AV_CODEC_ID_EAC3,
    AV_CODEC_ID_SIPR,
    AV_CODEC_ID_MP1,
    AV_CODEC_ID_TWINVQ,
    AV_CODEC_ID_TRUEHD,
    AV_CODEC_ID_MP4ALS,
    AV_CODEC_ID_ATRAC1,
    AV_CODEC_ID_BINKAUDIO_RDFT,
    AV_CODEC_ID_BINKAUDIO_DCT,
    AV_CODEC_ID_AAC_LATM,
    AV_CODEC_ID_QDMC,
    AV_CODEC_ID_CELT,
    AV_CODEC_ID_G723_1,
    AV_CODEC_ID_G729,
    AV_CODEC_ID_8SVX_EXP,
    AV_CODEC_ID_8SVX_FIB,
    AV_CODEC_ID_BMV_AUDIO,
    AV_CODEC_ID_RALF,
    AV_CODEC_ID_IAC,
    AV_CODEC_ID_ILBC,
    AV_CODEC_ID_OPUS,
    AV_CODEC_ID_COMFORT_NOISE,
    AV_CODEC_ID_TAK,
    AV_CODEC_ID_METASOUND,
    AV_CODEC_ID_PAF_AUDIO,
    AV_CODEC_ID_ON2AVC,
    AV_CODEC_ID_DSS_SP,
    AV_CODEC_ID_CODEC2,

    AV_CODEC_ID_FFWAVESYNTH = 0x15800,
    AV_CODEC_ID_SONIC,
    AV_CODEC_ID_SONIC_LS,
    AV_CODEC_ID_EVRC,
    AV_CODEC_ID_SMV,
    AV_CODEC_ID_DSD_LSBF,
    AV_CODEC_ID_DSD_MSBF,
    AV_CODEC_ID_DSD_LSBF_PLANAR,
    AV_CODEC_ID_DSD_MSBF_PLANAR,
    AV_CODEC_ID_4GV,
    AV_CODEC_ID_INTERPLAY_ACM,
    AV_CODEC_ID_XMA1,
    AV_CODEC_ID_XMA2,
    AV_CODEC_ID_DST,
    AV_CODEC_ID_ATRAC3AL,
    AV_CODEC_ID_ATRAC3PAL,
    AV_CODEC_ID_DOLBY_E,
    AV_CODEC_ID_APTX,
    AV_CODEC_ID_APTX_HD,
    AV_CODEC_ID_SBC,
    AV_CODEC_ID_ATRAC9,
    AV_CODEC_ID_HCOM,

/* subtitle codecs */
    AV_CODEC_ID_FIRST_SUBTITLE = 0x17000,          ///< A dummy ID pointing at the start of subtitle codecs.
    AV_CODEC_ID_DVD_SUBTITLE = 0x17000,
    AV_CODEC_ID_DVB_SUBTITLE,
    AV_CODEC_ID_TEXT,  ///< raw UTF-8 text
    AV_CODEC_ID_XSUB,
    AV_CODEC_ID_SSA,
    AV_CODEC_ID_MOV_TEXT,
    AV_CODEC_ID_HDMV_PGS_SUBTITLE,
    AV_CODEC_ID_DVB_TELETEXT,
    AV_CODEC_ID_SRT,

    AV_CODEC_ID_MICRODVD = 0x17800,
    AV_CODEC_ID_EIA_608,
    AV_CODEC_ID_JACOSUB,
    AV_CODEC_ID_SAMI,
    AV_CODEC_ID_REALTEXT,
    AV_CODEC_ID_STL,
    AV_CODEC_ID_SUBVIEWER1,
    AV_CODEC_ID_SUBVIEWER,
    AV_CODEC_ID_SUBRIP,
    AV_CODEC_ID_WEBVTT,
    AV_CODEC_ID_MPL2,
    AV_CODEC_ID_VPLAYER,
    AV_CODEC_ID_PJS,
    AV_CODEC_ID_ASS,
    AV_CODEC_ID_HDMV_TEXT_SUBTITLE,
    AV_CODEC_ID_TTML,
    AV_CODEC_ID_ARIB_CAPTION,

/* other specific kind of codecs (generally used for attachments) */
    AV_CODEC_ID_FIRST_UNKNOWN = 0x18000,           ///< A dummy ID pointing at the start of various fake codecs.
    AV_CODEC_ID_TTF = 0x18000,

    AV_CODEC_ID_SCTE_35, ///< Contain timestamp estimated through PCR of program stream.
    AV_CODEC_ID_BINTEXT = 0x18800,
    AV_CODEC_ID_XBIN,
    AV_CODEC_ID_IDF,
    AV_CODEC_ID_OTF,
    AV_CODEC_ID_SMPTE_KLV,
    AV_CODEC_ID_DVD_NAV,
    AV_CODEC_ID_TIMED_ID3,
    AV_CODEC_ID_BIN_DATA,


    AV_CODEC_ID_PROBE = 0x19000, ///< codec_id is not known (like AV_CODEC_ID_NONE) but lavf should attempt to identify it

    AV_CODEC_ID_MPEG2TS = 0x20000, /**< _FAKE_ codec to indicate a raw MPEG-2 TS
                                * stream (only used by libavformat) */
    AV_CODEC_ID_MPEG4SYSTEMS = 0x20001, /**< _FAKE_ codec to indicate a MPEG-4 Systems
                                * stream (only used by libavformat) */
    AV_CODEC_ID_FFMETADATA = 0x21000,   ///< Dummy codec for streams containing only metadata information.
    AV_CODEC_ID_WRAPPED_AVFRAME = 0x21001, ///< Passthrough codec, AVFrames wrapped in AVPacket
};


typedef enum {
    AV_CLASS_CATEGORY_NA = 0,
    AV_CLASS_CATEGORY_INPUT,
    AV_CLASS_CATEGORY_OUTPUT,
    AV_CLASS_CATEGORY_MUXER,
    AV_CLASS_CATEGORY_DEMUXER,
    AV_CLASS_CATEGORY_ENCODER,
    AV_CLASS_CATEGORY_DECODER,
    AV_CLASS_CATEGORY_FILTER,
    AV_CLASS_CATEGORY_BITSTREAM_FILTER,
    AV_CLASS_CATEGORY_SWSCALER,
    AV_CLASS_CATEGORY_SWRESAMPLER,
    AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT = 40,
    AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_INPUT,
    AV_CLASS_CATEGORY_NB  ///< not part of ABI/API
} AVClassCategory;


enum AVPictureType {
    AV_PICTURE_TYPE_NONE = 0, ///< Undefined
    AV_PICTURE_TYPE_I,     ///< Intra
    AV_PICTURE_TYPE_P,     ///< Predicted
    AV_PICTURE_TYPE_B,     ///< Bi-dir predicted
    AV_PICTURE_TYPE_S,     ///< S(GMC)-VOP MPEG-4
    AV_PICTURE_TYPE_SI,    ///< Switching Intra
    AV_PICTURE_TYPE_SP,    ///< Switching Predicted
    AV_PICTURE_TYPE_BI,    ///< BI type
};


enum AVMediaType {
    AVMEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as AVMEDIA_TYPE_DATA
    AVMEDIA_TYPE_VIDEO,
    AVMEDIA_TYPE_AUDIO,
    AVMEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
    AVMEDIA_TYPE_SUBTITLE,
    AVMEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
    AVMEDIA_TYPE_NB
};


#    define av_const __attribute__((const))
#    define av_always_inline __attribute__((always_inline)) inline

#if defined(__GNUC__) || defined(__clang__)
#    define av_unused __attribute__((unused))
#else
#    define av_unused
#endif



/**
 * assert() equivalent, that is always enabled.
 */
#define av_assert0(cond) do {                                           \
    if (!(cond)) {                                                      \
        av_log(NULL, AV_LOG_PANIC, "Assertion %s failed at %s:%d\n",    \
               AV_STRINGIFY(cond), __FILE__, __LINE__);                 \
        abort();                                                        \
    }                                                                   \
} while (0)


/**
 * assert() equivalent, that does not lie in speed critical code.
 * These asserts() thus can be enabled without fearing speed loss.
 */
#if defined(ASSERT_LEVEL) && ASSERT_LEVEL > 0
#define av_assert1(cond) av_assert0(cond)
#else
#define av_assert1(cond) ((void)0)
#endif


/**
 * assert() equivalent, that does lie in speed critical code.
 */
#if defined(ASSERT_LEVEL) && ASSERT_LEVEL > 1
                                                                                                                        #define av_assert2(cond) av_assert0(cond)
#define av_assert2_fpu() av_assert0_fpu()
#else
#define av_assert2(cond) ((void)0)
#define av_assert2_fpu() ((void)0)
#endif


#define AV_BSWAP16C(x) (((x) << 8 & 0xff00)  | ((x) >> 8 & 0x00ff))
#define AV_BSWAP32C(x) (AV_BSWAP16C(x) << 16 | AV_BSWAP16C((x) >> 16))
#define AV_BSWAP64C(x) (AV_BSWAP32C(x) << 32 | AV_BSWAP32C((x) >> 32))

#define AV_BSWAPC(s, x) AV_BSWAP##s##C(x)

#ifndef av_bswap16

static av_always_inline av_const uint16_t av_bswap16(uint16_t x) {
    x = (x >> 8) | (x << 8);
    return x;
}

#endif

#ifndef av_bswap32

static av_always_inline av_const uint32_t av_bswap32(uint32_t x) {
    return AV_BSWAP32C(x);
}

#endif

#ifndef av_bswap64

static inline uint64_t av_const av_bswap64(uint64_t x) {
    return (uint64_t) av_bswap32(x) << 32 | av_bswap32(x >> 32);
}

#endif

// be2ne ... big-endian to native-endian
// le2ne ... little-endian to native-endian

#if AV_HAVE_BIGENDIAN
                                                                                                                        #define av_be2ne16(x) (x)
#define av_be2ne32(x) (x)
#define av_be2ne64(x) (x)
#define av_le2ne16(x) av_bswap16(x)
#define av_le2ne32(x) av_bswap32(x)
#define av_le2ne64(x) av_bswap64(x)
#define AV_BE2NEC(s, x) (x)
#define AV_LE2NEC(s, x) AV_BSWAPC(s, x)
#else
#define av_be2ne16(x) av_bswap16(x)
#define av_be2ne32(x) av_bswap32(x)
#define av_be2ne64(x) av_bswap64(x)
#define av_le2ne16(x) (x)
#define av_le2ne32(x) (x)
#define av_le2ne64(x) (x)
#define AV_BE2NEC(s, x) AV_BSWAPC(s, x)
#define AV_LE2NEC(s, x) (x)
#endif

#define AV_BE2NE16C(x) AV_BE2NEC(16, x)
#define AV_BE2NE32C(x) AV_BE2NEC(32, x)
#define AV_BE2NE64C(x) AV_BE2NEC(64, x)
#define AV_LE2NE16C(x) AV_LE2NEC(16, x)
#define AV_LE2NE32C(x) AV_LE2NEC(32, x)
#define AV_LE2NE64C(x) AV_LE2NEC(64, x)


#   define av_alias __attribute__((may_alias))
//
//union unaligned_64 { uint64_t l; } __attribute__((packed)) av_alias;
//union unaligned_32 { uint32_t l; } __attribute__((packed)) av_alias;
//union unaligned_16 { uint16_t l; } __attribute__((packed)) av_alias;

#   define AV_RN(s, p) (((const union unaligned_##s *) (p))->l)
#   define AV_WN(s, p, v) ((((union unaligned_##s *) (p))->l) = (v))


#   define AV_RB(s, p)    av_bswap##s(AV_RN##s(p))
#   define AV_WB(s, p, v) AV_WN##s(p, av_bswap##s(v))
#   define AV_RL(s, p)    AV_RN##s(p)
#   define AV_WL(s, p, v) AV_WN##s(p, v)


#define AV_RB8(x)     (((const uint8_t*)(x))[0])
#define AV_WB8(p, d)  do { ((uint8_t*)(p))[0] = (d); } while(0)

#define AV_RL8(x)     AV_RB8(x)
#define AV_WL8(p, d)  AV_WB8(p, d)

#ifndef AV_RB16
#   define AV_RB16(p)    AV_RB(16, p)
#endif
#ifndef AV_WB16
#   define AV_WB16(p, v) AV_WB(16, p, v)
#endif

#ifndef AV_RL16
#   define AV_RL16(p)    AV_RL(16, p)
#endif
#ifndef AV_WL16
#   define AV_WL16(p, v) AV_WL(16, p, v)
#endif

#ifndef AV_RB32
#   define AV_RB32(p)    AV_RB(32, p)
#endif
#ifndef AV_WB32
#   define AV_WB32(p, v) AV_WB(32, p, v)
#endif

#ifndef AV_RL32
#   define AV_RL32(p)    AV_RL(32, p)
#endif
#ifndef AV_WL32
#   define AV_WL32(p, v) AV_WL(32, p, v)
#endif

#ifndef AV_RB64
#   define AV_RB64(p)    AV_RB(64, p)
#endif
#ifndef AV_WB64
#   define AV_WB64(p, v) AV_WB(64, p, v)
#endif

#ifndef AV_RL64
#   define AV_RL64(p)    AV_RL(64, p)
#endif
#ifndef AV_WL64
#   define AV_WL64(p, v) AV_WL(64, p, v)
#endif

#ifndef AV_RB24
#   define AV_RB24(x)                           \
    ((((const uint8_t*)(x))[0] << 16) |         \
     (((const uint8_t*)(x))[1] <<  8) |         \
      ((const uint8_t*)(x))[2])
#endif
#ifndef AV_WB24
#   define AV_WB24(p, d) do {                   \
        ((uint8_t*)(p))[2] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[0] = (d)>>16;           \
    } while(0)
#endif

#ifndef AV_RL24
#   define AV_RL24(x)                           \
    ((((const uint8_t*)(x))[2] << 16) |         \
     (((const uint8_t*)(x))[1] <<  8) |         \
      ((const uint8_t*)(x))[0])
#endif
#ifndef AV_WL24
#   define AV_WL24(p, d) do {                   \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
    } while(0)
#endif

#ifndef AV_RB48
#   define AV_RB48(x)                                     \
    (((uint64_t)((const uint8_t*)(x))[0] << 40) |         \
     ((uint64_t)((const uint8_t*)(x))[1] << 32) |         \
     ((uint64_t)((const uint8_t*)(x))[2] << 24) |         \
     ((uint64_t)((const uint8_t*)(x))[3] << 16) |         \
     ((uint64_t)((const uint8_t*)(x))[4] <<  8) |         \
      (uint64_t)((const uint8_t*)(x))[5])
#endif
#ifndef AV_WB48
#   define AV_WB48(p, darg) do {                \
        uint64_t d = (darg);                    \
        ((uint8_t*)(p))[5] = (d);               \
        ((uint8_t*)(p))[4] = (d)>>8;            \
        ((uint8_t*)(p))[3] = (d)>>16;           \
        ((uint8_t*)(p))[2] = (d)>>24;           \
        ((uint8_t*)(p))[1] = (d)>>32;           \
        ((uint8_t*)(p))[0] = (d)>>40;           \
    } while(0)
#endif

#ifndef AV_RL48
#   define AV_RL48(x)                                     \
    (((uint64_t)((const uint8_t*)(x))[5] << 40) |         \
     ((uint64_t)((const uint8_t*)(x))[4] << 32) |         \
     ((uint64_t)((const uint8_t*)(x))[3] << 24) |         \
     ((uint64_t)((const uint8_t*)(x))[2] << 16) |         \
     ((uint64_t)((const uint8_t*)(x))[1] <<  8) |         \
      (uint64_t)((const uint8_t*)(x))[0])
#endif
#ifndef AV_WL48
#   define AV_WL48(p, darg) do {                \
        uint64_t d = (darg);                    \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
        ((uint8_t*)(p))[3] = (d)>>24;           \
        ((uint8_t*)(p))[4] = (d)>>32;           \
        ((uint8_t*)(p))[5] = (d)>>40;           \
    } while(0)
#endif

/*
 * The AV_[RW]NA macros access naturally aligned data
 * in a type-safe way.
 */
union unaligned_64 {
    uint64_t l;
} __attribute__((packed)) av_alias;
union unaligned_32 {
    uint32_t l;
} __attribute__((packed)) av_alias;
union unaligned_16 {
    uint16_t l;
} __attribute__((packed)) av_alias;


#define AV_RNA(s, p)    (((const av_alias##s*)(p))->u##s)
#define AV_WNA(s, p, v) (((av_alias##s*)(p))->u##s = (v))


#ifndef AV_RN16
#   define AV_RN16(p) AV_RN(16, p)
#endif

#ifndef AV_RN32
#   define AV_RN32(p) AV_RN(32, p)
#endif

#ifndef AV_RN64
#   define AV_RN64(p) AV_RN(64, p)
#endif

#ifndef AV_WN16
#   define AV_WN16(p, v) AV_WN(16, p, v)
#endif

#ifndef AV_WN32
#   define AV_WN32(p, v) AV_WN(32, p, v)
#endif

#ifndef AV_WN64
#   define AV_WN64(p, v) AV_WN(64, p, v)
#endif


#ifndef AV_RN16A
#   define AV_RN16A(p) AV_RNA(16, p)
#endif

#ifndef AV_RN32
#   define AV_RN32(p) AV_RN(32, p)
#endif


#ifndef AV_RN32A
#   define AV_RN32A(p) AV_RNA(32, p)
#endif

#ifndef AV_RN64A
#   define AV_RN64A(p) AV_RNA(64, p)
#endif

#ifndef AV_WN16A
#   define AV_WN16A(p, v) AV_WNA(16, p, v)
#endif

#ifndef AV_WN32A
#   define AV_WN32A(p, v) AV_WNA(32, p, v)
#endif

#ifndef AV_WN64A
#   define AV_WN64A(p, v) AV_WNA(64, p, v)
#endif

#if AV_HAVE_BIGENDIAN
                                                                                                                        #   define AV_RLA(s, p)    av_bswap##s(AV_RN##s##A(p))
#   define AV_WLA(s, p, v) AV_WN##s##A(p, av_bswap##s(v))
#else
#   define AV_RLA(s, p)    AV_RN##s##A(p)
#   define AV_WLA(s, p, v) AV_WN##s##A(p, v)
#endif

#ifndef AV_RL64A
#   define AV_RL64A(p) AV_RLA(64, p)
#endif
#ifndef AV_WL64A
#   define AV_WL64A(p, v) AV_WLA(64, p, v)
#endif


# define UPDATE_CACHE_LE(name, gb) name ## _cache = \
      AV_RL32((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)

# define UPDATE_CACHE_BE(name, gb) name ## _cache = \
      AV_RB32((gb)->buffer + (name ## _index >> 3)) << (name ## _index & 7)


typedef struct GetBitContext {
    const uint8_t *buffer, *buffer_end;
#if CACHED_BITSTREAM_READER
                                                                                                                            uint64_t cache;
unsigned bits_left;
#endif
    int index;
    int size_in_bits;
    int size_in_bits_plus8;
} GetBitContext;


#if CACHED_BITSTREAM_READER
#   define MIN_CACHE_BITS 64
#elif defined LONG_BITSTREAM_READER
#   define MIN_CACHE_BITS 32
#else
#   define MIN_CACHE_BITS 25
#endif

#if !CACHED_BITSTREAM_READER

#define OPEN_READER_NOSIZE(name, gb)            \
    unsigned int name ## _index = (gb)->index;  \
    unsigned int av_unused name ## _cache

#if UNCHECKED_BITSTREAM_READER
                                                                                                                        #define OPEN_READER(name, gb) OPEN_READER_NOSIZE(name, gb)

#define BITS_AVAILABLE(name, gb) 1
#else
#define OPEN_READER(name, gb)                   \
    OPEN_READER_NOSIZE(name, gb);               \
    unsigned int name ## _size_plus8 = (gb)->size_in_bits_plus8

#define BITS_AVAILABLE(name, gb) name ## _index < name ## _size_plus8
#endif

#define CLOSE_READER(name, gb) (gb)->index = name ## _index

# ifdef LONG_BITSTREAM_READER

                                                                                                                        # define UPDATE_CACHE_LE(name, gb) name ## _cache = \
      AV_RL64((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)

# define UPDATE_CACHE_BE(name, gb) name ## _cache = \
      AV_RB64((gb)->buffer + (name ## _index >> 3)) >> (32 - (name ## _index & 7))

#else

# define UPDATE_CACHE_LE(name, gb) name ## _cache = \
      AV_RL32((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)

# define UPDATE_CACHE_BE(name, gb) name ## _cache = \
      AV_RB32((gb)->buffer + (name ## _index >> 3)) << (name ## _index & 7)

#endif


#if HAVE_I686
                                                                                                                        /* median of 3 */
#define mid_pred mid_pred
static inline av_const int mid_pred(int a, int b, int c)
{
int i=b;
__asm__ (
"cmp    %2, %1 \n\t"
"cmovg  %1, %0 \n\t"
"cmovg  %2, %1 \n\t"
"cmp    %3, %1 \n\t"
"cmovl  %3, %1 \n\t"
"cmp    %1, %0 \n\t"
"cmovg  %1, %0 \n\t"
:"+&r"(i), "+&r"(a)
:"r"(b), "r"(c)
);
return i;
}

#if HAVE_6REGS
#define COPY3_IF_LT(x, y, a, b, c, d)\
__asm__ volatile(\
    "cmpl  %0, %3       \n\t"\
    "cmovl %3, %0       \n\t"\
    "cmovl %4, %1       \n\t"\
    "cmovl %5, %2       \n\t"\
    : "+&r" (x), "+&r" (a), "+r" (c)\
    : "r" (y), "r" (b), "r" (d)\
);
#endif /* HAVE_6REGS */

#endif /* HAVE_I686 */

#define MASK_ABS(mask, level)                   \
    __asm__ ("cdq                    \n\t"      \
             "xorl %1, %0            \n\t"      \
             "subl %1, %0            \n\t"      \
             : "+a"(level), "=&d"(mask))

// avoid +32 for shift optimization (gcc should do that ...)
//#define NEG_SSR32 NEG_SSR32
//static inline  int32_t NEG_SSR32( int32_t a, int8_t s){
//    __asm__ ("sarl %1, %0\n\t"
//         : "+r" (a)
//         : "ic" ((uint8_t)(-s))
//    );
//    return a;
//}
//
//#define NEG_USR32 NEG_USR32
//static inline uint32_t NEG_USR32(uint32_t a, int8_t s){
//    __asm__ ("shrl %1, %0\n\t"
//         : "+r" (a)
//         : "ic" ((uint8_t)(-s))
//    );
//    return a;
//}
//


#ifndef NEG_SSR32
#   define NEG_SSR32(a, s) ((( int32_t)(a))>>(32-(s)))
#endif

#ifndef NEG_USR32
#   define NEG_USR32(a, s) (((uint32_t)(a))>>(32-(s)))
#endif


#ifdef BITSTREAM_READER_LE

                                                                                                                        # define UPDATE_CACHE(name, gb) UPDATE_CACHE_LE(name, gb)

# define SKIP_CACHE(name, gb, num) name ## _cache >>= (num)

#else

# define UPDATE_CACHE(name, gb) UPDATE_CACHE_BE(name, gb)

# define SKIP_CACHE(name, gb, num) name ## _cache <<= (num)

#endif

#if UNCHECKED_BITSTREAM_READER
#   define SKIP_COUNTER(name, gb, num) name ## _index += (num)
#else
#   define SKIP_COUNTER(name, gb, num) \
    name ## _index = FFMIN(name ## _size_plus8, name ## _index + (num))
#endif

#define BITS_LEFT(name, gb) ((int)((gb)->size_in_bits - name ## _index))

#define SKIP_BITS(name, gb, num)                \
    do {                                        \
        SKIP_CACHE(name, gb, num);              \
        SKIP_COUNTER(name, gb, num);            \
    } while (0)

#define LAST_SKIP_BITS(name, gb, num) SKIP_COUNTER(name, gb, num)

#define SHOW_UBITS_LE(name, gb, num) zero_extend(name ## _cache, num)
#define SHOW_SBITS_LE(name, gb, num) sign_extend(name ## _cache, num)

#define SHOW_UBITS_BE(name, gb, num) NEG_USR32(name ## _cache, num)
#define SHOW_SBITS_BE(name, gb, num) NEG_SSR32(name ## _cache, num)

#ifdef BITSTREAM_READER_LE
                                                                                                                        #   define SHOW_UBITS(name, gb, num) SHOW_UBITS_LE(name, gb, num)
#   define SHOW_SBITS(name, gb, num) SHOW_SBITS_LE(name, gb, num)
#else
#   define SHOW_UBITS(name, gb, num) SHOW_UBITS_BE(name, gb, num)
#   define SHOW_SBITS(name, gb, num) SHOW_SBITS_BE(name, gb, num)
#endif

#define GET_CACHE(name, gb) ((uint32_t) name ## _cache)

#endif

static inline int get_bits_count(const GetBitContext *s) {
#if CACHED_BITSTREAM_READER
    return s->index - s->bits_left;
#else
    return s->index;
#endif
}


static inline unsigned int get_bits(GetBitContext *s, int n);

static inline void skip_bits(GetBitContext *s, int n);

static inline unsigned int show_bits(GetBitContext *s, int n);


#define AV_INPUT_BUFFER_PADDING_SIZE 64


#ifndef av_ceil_log2
#   define av_ceil_log2     av_ceil_log2_c
#endif
#ifndef av_clip
#   define av_clip          av_clip_c
#endif
#ifndef av_clip64
#   define av_clip64        av_clip64_c
#endif
#ifndef av_clip_uint8
#   define av_clip_uint8    av_clip_uint8_c
#endif
#ifndef av_clip_int8
#   define av_clip_int8     av_clip_int8_c
#endif
#ifndef av_clip_uint16
#   define av_clip_uint16   av_clip_uint16_c
#endif
#ifndef av_clip_int16
#   define av_clip_int16    av_clip_int16_c
#endif
#ifndef av_clipl_int32
#   define av_clipl_int32   av_clipl_int32_c
#endif
#ifndef av_clip_intp2
#   define av_clip_intp2    av_clip_intp2_c
#endif
#ifndef av_clip_uintp2
#   define av_clip_uintp2   av_clip_uintp2_c
#endif
#ifndef av_mod_uintp2
#   define av_mod_uintp2    av_mod_uintp2_c
#endif
#ifndef av_sat_add32
#   define av_sat_add32     av_sat_add32_c
#endif
#ifndef av_sat_dadd32
#   define av_sat_dadd32    av_sat_dadd32_c
#endif
#ifndef av_sat_sub32
#   define av_sat_sub32     av_sat_sub32_c
#endif
#ifndef av_sat_dsub32
#   define av_sat_dsub32    av_sat_dsub32_c
#endif
#ifndef av_clipf
#   define av_clipf         av_clipf_c
#endif
#ifndef av_clipd
#   define av_clipd         av_clipd_c
#endif
#ifndef av_popcount
#   define av_popcount      av_popcount_c
#endif
#ifndef av_popcount64
#   define av_popcount64    av_popcount64_c
#endif
#ifndef av_parity
#   define av_parity        av_parity_c
#endif


#define AV_CODEC_FLAG2_IGNORE_CROP    (1 << 16)


#define AV_PIX_FMT_FLAG_RGB          (1 << 5)
#define AV_PIX_FMT_FLAG_ALPHA        (1 << 7)
#define AV_PIX_FMT_FLAG_PSEUDOPAL    (1 << 6)
#define FF_PSEUDOPAL AV_PIX_FMT_FLAG_PSEUDOPAL
#define AV_PIX_FMT_FLAG_BAYER        (1 << 8)
#define AV_PIX_FMT_FLAG_FLOAT        (1 << 9)






/* error handling */
#if EDOM > 0
#define AVERROR(e) (-(e))   ///< Returns a negative error code from a POSIX error code, to return from library functions.
#define AVUNERROR(e) (-(e)) ///< Returns a POSIX error code from a library function error return value.
#else
                                                                                                                        /* Some platforms have E* and errno already negated. */
#define AVERROR(e) (e)
#define AVUNERROR(e) (e)
#endif

#define FFERRTAG(a, b, c, d) (-(int)MKTAG(a, b, c, d))

#define AVERROR_BSF_NOT_FOUND      FFERRTAG(0xF8,'B','S','F') ///< Bitstream filter not found
#define AVERROR_BUG                FFERRTAG( 'B','U','G','!') ///< Internal bug, also see AVERROR_BUG2
#define AVERROR_BUFFER_TOO_SMALL   FFERRTAG( 'B','U','F','S') ///< Buffer too small
#define AVERROR_DECODER_NOT_FOUND  FFERRTAG(0xF8,'D','E','C') ///< Decoder not found
#define AVERROR_DEMUXER_NOT_FOUND  FFERRTAG(0xF8,'D','E','M') ///< Demuxer not found
#define AVERROR_ENCODER_NOT_FOUND  FFERRTAG(0xF8,'E','N','C') ///< Encoder not found
#define AVERROR_EOF                FFERRTAG( 'E','O','F',' ') ///< End of file
#define AVERROR_EXIT               FFERRTAG( 'E','X','I','T') ///< Immediate exit was requested; the called function should not be restarted
#define AVERROR_EXTERNAL           FFERRTAG( 'E','X','T',' ') ///< Generic error in an external library
#define AVERROR_FILTER_NOT_FOUND   FFERRTAG(0xF8,'F','I','L') ///< Filter not found
#define AVERROR_INVALIDDATA        FFERRTAG( 'I','N','D','A') ///< Invalid data found when processing input
#define AVERROR_MUXER_NOT_FOUND    FFERRTAG(0xF8,'M','U','X') ///< Muxer not found
#define AVERROR_OPTION_NOT_FOUND   FFERRTAG(0xF8,'O','P','T') ///< Option not found
#define AVERROR_PATCHWELCOME       FFERRTAG( 'P','A','W','E') ///< Not yet implemented in FFmpeg, patches welcome
#define AVERROR_PROTOCOL_NOT_FOUND FFERRTAG(0xF8,'P','R','O') ///< Protocol not found

#define AVERROR_STREAM_NOT_FOUND   FFERRTAG(0xF8,'S','T','R') ///< Stream not found
/**
 * This is semantically identical to AVERROR_BUG
 * it has been introduced in Libav after our AVERROR_BUG and with a modified value.
 */
#define AVERROR_BUG2               FFERRTAG( 'B','U','G',' ')
#define AVERROR_UNKNOWN            FFERRTAG( 'U','N','K','N') ///< Unknown error, typically from an external library
#define AVERROR_EXPERIMENTAL       (-0x2bb2afa8) ///< Requested feature is flagged experimental. Set strict_std_compliance if you really want to use it.
#define AVERROR_INPUT_CHANGED      (-0x636e6701) ///< Input changed between calls. Reconfiguration is required. (can be OR-ed with AVERROR_OUTPUT_CHANGED)
#define AVERROR_OUTPUT_CHANGED     (-0x636e6702) ///< Output changed between calls. Reconfiguration is required. (can be OR-ed with AVERROR_INPUT_CHANGED)
/* HTTP & RTSP errors */
#define AVERROR_HTTP_BAD_REQUEST   FFERRTAG(0xF8,'4','0','0')
#define AVERROR_HTTP_UNAUTHORIZED  FFERRTAG(0xF8,'4','0','1')
#define AVERROR_HTTP_FORBIDDEN     FFERRTAG(0xF8,'4','0','3')
#define AVERROR_HTTP_NOT_FOUND     FFERRTAG(0xF8,'4','0','4')
#define AVERROR_HTTP_OTHER_4XX     FFERRTAG(0xF8,'4','X','X')
#define AVERROR_HTTP_SERVER_ERROR  FFERRTAG(0xF8,'5','X','X')

#define AV_ERROR_MAX_STRING_SIZE 64


#define VLC_TYPE int16_t

typedef struct VLC {
    int bits;
    VLC_TYPE (*table)[2]; ///< code, bits
    int table_size, table_allocated;
} VLC;

typedef struct RL_VLC_ELEM {
    int16_t level;
    int8_t len;
    uint8_t run;
} RL_VLC_ELEM;

#define init_vlc(vlc, nb_bits, nb_codes, \
                 bits, bits_wrap, bits_size, \
                 codes, codes_wrap, codes_size, \
                 flags)                                 \
    ff_init_vlc_sparse(vlc, nb_bits, nb_codes,          \
                       bits, bits_wrap, bits_size,      \
                       codes, codes_wrap, codes_size,   \
                       NULL, 0, 0, flags)

int ff_init_vlc_sparse(VLC *vlc, int nb_bits, int nb_codes, const void *bits, int bits_wrap, int bits_size, const void *codes, int codes_wrap, int codes_size, const void *symbols, int symbols_wrap, int symbols_size, int flags);

void ff_free_vlc(VLC *vlc);

#define INIT_VLC_LE             2
#define INIT_VLC_USE_NEW_STATIC 4

#define INIT_VLC_SPARSE_STATIC(vlc, bits, a, b, c, d, e, f, g, h, i, j, static_size) \
    do {                                                                   \
        static VLC_TYPE table[static_size][2];                             \
        (vlc)->table           = table;                                    \
        (vlc)->table_allocated = static_size;                              \
        ff_init_vlc_sparse(vlc, bits, a, b, c, d, e, f, g, h, i, j,        \
            INIT_VLC_USE_NEW_STATIC);                                      \
    } while (0)

#define INIT_LE_VLC_SPARSE_STATIC(vlc, bits, a, b, c, d, e, f, g, h, i, j, static_size) \
    do {                                                                   \
        static VLC_TYPE table[static_size][2];                             \
        (vlc)->table           = table;                                    \
        (vlc)->table_allocated = static_size;                              \
        ff_init_vlc_sparse(vlc, bits, a, b, c, d, e, f, g, h, i, j,        \
            INIT_VLC_USE_NEW_STATIC | INIT_VLC_LE);                        \
    } while (0)

#define INIT_VLC_STATIC(vlc, bits, a, b, c, d, e, f, g, static_size)       \
    INIT_VLC_SPARSE_STATIC(vlc, bits, a, b, c, d, e, f, g, NULL, 0, 0, static_size)

#define INIT_LE_VLC_STATIC(vlc, bits, a, b, c, d, e, f, g, static_size) \
    INIT_LE_VLC_SPARSE_STATIC(vlc, bits, a, b, c, d, e, f, g, NULL, 0, 0, static_size)


#define INT_MAX INT32_MAX


#if defined(__cplusplus) && !defined(__STDC_CONSTANT_MACROS) && !defined(UINT64_C)
#error missing -D__STDC_CONSTANT_MACROS / #define __STDC_CONSTANT_MACROS
#endif

#if AV_HAVE_BIGENDIAN
#   define AV_NE(be, le) (be)
#else
#   define AV_NE(be, le) (le)
#endif

//rounded division & shift
#define RSHIFT(a, b) ((a) > 0 ? ((a) + ((1<<(b))>>1))>>(b) : ((a) + ((1<<(b))>>1)-1)>>(b))
/* assume b>0 */
#define ROUNDED_DIV(a, b) (((a)>0 ? (a) + ((b)>>1) : (a) - ((b)>>1))/(b))
/* Fast a/(1<<b) rounded toward +inf. Assume a>=0 and b>=0 */
#define AV_CEIL_RSHIFT(a, b) (!av_builtin_constant_p(b) ? -((-(a)) >> (b)) \
                                                       : ((a) + (1<<(b)) - 1) >> (b))
/* Backwards compat. */
#define FF_CEIL_RSHIFT AV_CEIL_RSHIFT

#define FFUDIV(a, b) (((a)>0 ?(a):(a)-(b)+1) / (b))
#define FFUMOD(a, b) ((a)-(b)*FFUDIV(a,b))

/**
 * Absolute value, Note, INT_MIN / INT64_MIN result in undefined behavior as they
 * are not representable as absolute values of their type. This is the same
 * as with *abs()
 * @see FFNABS()
 */
#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))
#define FFSIGN(a) ((a) > 0 ? 1 : -1)

/**
 * Negative Absolute value.
 * this works for all integers of all types.
 * As with many macros, this evaluates its argument twice, it thus must not have
 * a sideeffect, that is FFNABS(x++) has undefined behavior.
 */
#define FFNABS(a) ((a) <= 0 ? (a) : (-(a)))

/**
 * Comparator.
 * For two numerical expressions x and y, gives 1 if x > y, -1 if x < y, and 0
 * if x == y. This is useful for instance in a qsort comparator callback.
 * Furthermore, compilers are able to optimize this to branchless code, and
 * there is no risk of overflow with signed types.
 * As with many macros, this evaluates its argument multiple times, it thus
 * must not have a side-effect.
 */
#define FFDIFFSIGN(x, y) (((x)>(y)) - ((x)<(y)))

#define FFMAX(a, b) ((a) > (b) ? (a) : (b))
#define FFMAX3(a, b, c) FFMAX(FFMAX(a,b),c)
#define FFMIN(a, b) ((a) > (b) ? (b) : (a))
#define FFMIN3(a, b, c) FFMIN(FFMIN(a,b),c)

#define FFSWAP(type, a, b) do{type SWAP_tmp= b; b= a; a= SWAP_tmp;}while(0)
#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

/* misc math functions */

#ifndef av_log2

av_const int av_log2(unsigned v);

#endif

#ifndef av_log2_16bit

av_const int av_log2_16bit(unsigned v);

#endif

/**
 * Clip a signed integer value into the amin-amax range.
 * @param a value to clip
 * @param amin minimum value of the clip range
 * @param amax maximum value of the clip range
 * @return clipped value
 */
static av_always_inline av_const int av_clip_c(int a, int amin, int amax) {
#if defined(HAVE_AV_CONFIG_H) && defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
    if (amin > amax) abort();
#endif
    if (a < amin) {
        return amin;
    } else if (a > amax) {
        return amax;
    } else {
        return a;
    }
}

/**
 * Clip a signed 64bit integer value into the amin-amax range.
 * @param a value to clip
 * @param amin minimum value of the clip range
 * @param amax maximum value of the clip range
 * @return clipped value
 */
static av_always_inline av_const int64_t av_clip64_c(int64_t a, int64_t amin, int64_t amax) {
#if defined(HAVE_AV_CONFIG_H) && defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
    if (amin > amax) abort();
#endif
    if (a < amin) {
        return amin;
    } else if (a > amax) {
        return amax;
    } else {
        return a;
    }
}

/**
 * Clip a signed integer value into the 0-255 range.
 * @param a value to clip
 * @return clipped value
 */
static av_always_inline av_const uint8_t av_clip_uint8_c(int a) {
    if (a & (~0xFF)) {
        return (~a) >> 31;
    } else {
        return a;
    }
}

/**
 * Clip a signed integer value into the -128,127 range.
 * @param a value to clip
 * @return clipped value
 */
static av_always_inline av_const int8_t av_clip_int8_c(int a) {
    if ((a + 0x80U) & ~0xFF) {
        return (a >> 31) ^ 0x7F;
    } else {
        return a;
    }
}

/**
 * Clip a signed integer value into the 0-65535 range.
 * @param a value to clip
 * @return clipped value
 */
static av_always_inline av_const uint16_t av_clip_uint16_c(int a) {
    if (a & (~0xFFFF)) {
        return (~a) >> 31;
    } else {
        return a;
    }
}

/**
 * Clip a signed integer value into the -32768,32767 range.
 * @param a value to clip
 * @return clipped value
 */
static av_always_inline av_const int16_t av_clip_int16_c(int a) {
    if ((a + 0x8000U) & ~0xFFFF) {
        return (a >> 31) ^ 0x7FFF;
    } else {
        return a;
    }
}

/**
 * Clip a signed 64-bit integer value into the -2147483648,2147483647 range.
 * @param a value to clip
 * @return clipped value
 */
static av_always_inline av_const int32_t av_clipl_int32_c(int64_t a) {
    if ((a + 0x80000000u) & ~UINT64_C(0xFFFFFFFF)) {
        return (int32_t) ((a >> 63) ^ 0x7FFFFFFF);
    } else {
        return (int32_t) a;
    }
}

/**
 * Clip a signed integer into the -(2^p),(2^p-1) range.
 * @param  a value to clip
 * @param  p bit position to clip at
 * @return clipped value
 */
static av_always_inline av_const int av_clip_intp2_c(int a, int p) {
    if (((unsigned) a + (1 << p)) & ~((2 << p) - 1)) {
        return (a >> 31) ^ ((1 << p) - 1);
    } else {
        return a;
    }
}

/**
 * Clip a signed integer to an unsigned power of two range.
 * @param  a value to clip
 * @param  p bit position to clip at
 * @return clipped value
 */
static av_always_inline av_const unsigned av_clip_uintp2_c(int a, int p) {
    if (a & ~((1 << p) - 1)) {
        return (~a) >> 31 & ((1 << p) - 1);
    } else {
        return a;
    }
}

/**
 * Clear high bits from an unsigned integer starting with specific bit position
 * @param  a value to clip
 * @param  p bit position to clip at
 * @return clipped value
 */
static av_always_inline av_const unsigned av_mod_uintp2_c(unsigned a, unsigned p) {
    return a & ((1 << p) - 1);
}

/**
 * Add two signed 32-bit values with saturation.
 *
 * @param  a one value
 * @param  b another value
 * @return sum with signed saturation
 */
static av_always_inline int av_sat_add32_c(int a, int b) {
    return av_clipl_int32((int64_t) a + b);
}

/**
 * Add a doubled value to another value with saturation at both stages.
 *
 * @param  a first value
 * @param  b value doubled and added to a
 * @return sum sat(a + sat(2*b)) with signed saturation
 */
static av_always_inline int av_sat_dadd32_c(int a, int b) {
    return av_sat_add32(a, av_sat_add32(b, b));
}

/**
 * Subtract two signed 32-bit values with saturation.
 *
 * @param  a one value
 * @param  b another value
 * @return difference with signed saturation
 */
static av_always_inline int av_sat_sub32_c(int a, int b) {
    return av_clipl_int32((int64_t) a - b);
}

/**
 * Subtract a doubled value from another value with saturation at both stages.
 *
 * @param  a first value
 * @param  b value doubled and subtracted from a
 * @return difference sat(a - sat(2*b)) with signed saturation
 */
static av_always_inline int av_sat_dsub32_c(int a, int b) {
    return av_sat_sub32(a, av_sat_add32(b, b));
}

/**
 * Clip a float value into the amin-amax range.
 * @param a value to clip
 * @param amin minimum value of the clip range
 * @param amax maximum value of the clip range
 * @return clipped value
 */
static av_always_inline av_const float av_clipf_c(float a, float amin, float amax) {
#if defined(HAVE_AV_CONFIG_H) && defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
    if (amin > amax) abort();
#endif
    if (a < amin) {
        return amin;
    } else if (a > amax) {
        return amax;
    } else {
        return a;
    }
}

/**
 * Clip a double value into the amin-amax range.
 * @param a value to clip
 * @param amin minimum value of the clip range
 * @param amax maximum value of the clip range
 * @return clipped value
 */
static av_always_inline av_const double av_clipd_c(double a, double amin, double amax) {
#if defined(HAVE_AV_CONFIG_H) && defined(ASSERT_LEVEL) && ASSERT_LEVEL >= 2
    if (amin > amax) abort();
#endif
    if (a < amin) {
        return amin;
    } else if (a > amax) {
        return amax;
    } else {
        return a;
    }
}

/** Compute ceil(log2(x)).
 * @param x value used to compute ceil(log2(x))
 * @return computed ceiling of log2(x)
 */
static av_always_inline av_const int av_ceil_log2_c(int x) {
    return av_log2((x - 1) << 1);
}

/**
 * Count number of bits set to one in x
 * @param x value to count bits of
 * @return the number of bits set to one in x
 */
static av_always_inline av_const int av_popcount_c(uint32_t x) {
    x -= (x >> 1) & 0x55555555;
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x += x >> 8;
    return (x + (x >> 16)) & 0x3F;
}

/**
 * Count number of bits set to one in x
 * @param x value to count bits of
 * @return the number of bits set to one in x
 */
static av_always_inline av_const int av_popcount64_c(uint64_t x) {
    return av_popcount((uint32_t) x) + av_popcount((uint32_t) (x >> 32));
}

static av_always_inline av_const int av_parity_c(uint32_t v) {
    return av_popcount(v) & 1;
}

#define MKTAG(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define MKBETAG(a, b, c, d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))

/**
 * Convert a UTF-8 character (up to 4 bytes) to its 32-bit UCS-4 encoded form.
 *
 * @param val      Output value, must be an lvalue of type uint32_t.
 * @param GET_BYTE Expression reading one byte from the input.
 *                 Evaluated up to 7 times (4 for the currently
 *                 assigned Unicode range).  With a memory buffer
 *                 input, this could be *ptr++.
 * @param ERROR    Expression to be evaluated on invalid input,
 *                 typically a goto statement.
 *
 * @warning ERROR should not contain a loop control statement which
 * could interact with the internal while loop, and should force an
 * exit from the macro code (e.g. through a goto or a return) in order
 * to prevent undefined results.
 */
#define GET_UTF8(val, GET_BYTE, ERROR)\
    val= (GET_BYTE);\
    {\
        uint32_t top = (val & 128) >> 1;\
        if ((val & 0xc0) == 0x80 || val >= 0xFE)\
            ERROR\
        while (val & top) {\
            int tmp= (GET_BYTE) - 128;\
            if(tmp>>6)\
                ERROR\
            val= (val<<6) + tmp;\
            top <<= 5;\
        }\
        val &= (top << 1) - 1;\
    }

/**
 * Convert a UTF-16 character (2 or 4 bytes) to its 32-bit UCS-4 encoded form.
 *
 * @param val       Output value, must be an lvalue of type uint32_t.
 * @param GET_16BIT Expression returning two bytes of UTF-16 data converted
 *                  to native byte order.  Evaluated one or two times.
 * @param ERROR     Expression to be evaluated on invalid input,
 *                  typically a goto statement.
 */
#define GET_UTF16(val, GET_16BIT, ERROR)\
    val = GET_16BIT;\
    {\
        unsigned int hi = val - 0xD800;\
        if (hi < 0x800) {\
            val = GET_16BIT - 0xDC00;\
            if (val > 0x3FFU || hi > 0x3FFU)\
                ERROR\
            val += (hi<<10) + 0x10000;\
        }\
    }\

/**
 * @def PUT_UTF8(val, tmp, PUT_BYTE)
 * Convert a 32-bit Unicode character to its UTF-8 encoded form (up to 4 bytes long).
 * @param val is an input-only argument and should be of type uint32_t. It holds
 * a UCS-4 encoded Unicode character that is to be converted to UTF-8. If
 * val is given as a function it is executed only once.
 * @param tmp is a temporary variable and should be of type uint8_t. It
 * represents an intermediate value during conversion that is to be
 * output by PUT_BYTE.
 * @param PUT_BYTE writes the converted UTF-8 bytes to any proper destination.
 * It could be a function or a statement, and uses tmp as the input byte.
 * For example, PUT_BYTE could be "*output++ = tmp;" PUT_BYTE will be
 * executed up to 4 times for values in the valid UTF-8 range and up to
 * 7 times in the general case, depending on the length of the converted
 * Unicode character.
 */
#define PUT_UTF8(val, tmp, PUT_BYTE)\
    {\
        int bytes, shift;\
        uint32_t in = val;\
        if (in < 0x80) {\
            tmp = in;\
            PUT_BYTE\
        } else {\
            bytes = (av_log2(in) + 4) / 5;\
            shift = (bytes - 1) * 6;\
            tmp = (256 - (256 >> bytes)) | (in >> shift);\
            PUT_BYTE\
            while (shift >= 6) {\
                shift -= 6;\
                tmp = 0x80 | ((in >> shift) & 0x3f);\
                PUT_BYTE\
            }\
        }\
    }

/**
 * @def PUT_UTF16(val, tmp, PUT_16BIT)
 * Convert a 32-bit Unicode character to its UTF-16 encoded form (2 or 4 bytes).
 * @param val is an input-only argument and should be of type uint32_t. It holds
 * a UCS-4 encoded Unicode character that is to be converted to UTF-16. If
 * val is given as a function it is executed only once.
 * @param tmp is a temporary variable and should be of type uint16_t. It
 * represents an intermediate value during conversion that is to be
 * output by PUT_16BIT.
 * @param PUT_16BIT writes the converted UTF-16 data to any proper destination
 * in desired endianness. It could be a function or a statement, and uses tmp
 * as the input byte.  For example, PUT_BYTE could be "*output++ = tmp;"
 * PUT_BYTE will be executed 1 or 2 times depending on input character.
 */
#define PUT_UTF16(val, tmp, PUT_16BIT)\
    {\
        uint32_t in = val;\
        if (in < 0x10000) {\
            tmp = in;\
            PUT_16BIT\
        } else {\
            tmp = 0xD800 | ((in - 0x10000) >> 10);\
            PUT_16BIT\
            tmp = 0xDC00 | ((in - 0x10000) & 0x3FF);\
            PUT_16BIT\
        }\
    }\



static inline unsigned int get_bits(GetBitContext *s, int n);

static inline void skip_bits(GetBitContext *s, int n);

static inline unsigned int show_bits(GetBitContext *s, int n);


static inline av_const unsigned zero_extend(unsigned val, unsigned bits) {
    return (val << ((8 * sizeof(int)) - bits)) >> ((8 * sizeof(int)) - bits);
}


/* Bitstream reader API docs:
 * name
 *   arbitrary name which is used as prefix for the internal variables
 *
 * gb
 *   getbitcontext
 *
 * OPEN_READER(name, gb)
 *   load gb into local variables
 *
 * CLOSE_READER(name, gb)
 *   store local vars in gb
 *
 * UPDATE_CACHE(name, gb)
 *   Refill the internal cache from the bitstream.
 *   After this call at least MIN_CACHE_BITS will be available.
 *
 * GET_CACHE(name, gb)
 *   Will output the contents of the internal cache,
 *   next bit is MSB of 32 or 64 bits (FIXME 64 bits).
 *
 * SHOW_UBITS(name, gb, num)
 *   Will return the next num bits.
 *
 * SHOW_SBITS(name, gb, num)
 *   Will return the next num bits and do sign extension.
 *
 * SKIP_BITS(name, gb, num)
 *   Will skip over the next num bits.
 *   Note, this is equivalent to SKIP_CACHE; SKIP_COUNTER.
 *
 * SKIP_CACHE(name, gb, num)
 *   Will remove the next num bits from the cache (note SKIP_COUNTER
 *   MUST be called before UPDATE_CACHE / CLOSE_READER).
 *
 * SKIP_COUNTER(name, gb, num)
 *   Will increment the internal bit counter (see SKIP_CACHE & SKIP_BITS).
 *
 * LAST_SKIP_BITS(name, gb, num)
 *   Like SKIP_BITS, to be used if next call is UPDATE_CACHE or CLOSE_READER.
 *
 * BITS_LEFT(name, gb)
 *   Return the number of bits left
 *
 * For examples see get_bits, show_bits, skip_bits, get_vlc.
 */

#if CACHED_BITSTREAM_READER
#   define MIN_CACHE_BITS 64
#elif defined LONG_BITSTREAM_READER
#   define MIN_CACHE_BITS 32
#else
#   define MIN_CACHE_BITS 25
#endif

#if !CACHED_BITSTREAM_READER

#define OPEN_READER_NOSIZE(name, gb)            \
    unsigned int name ## _index = (gb)->index;  \
    unsigned int av_unused name ## _cache

#if UNCHECKED_BITSTREAM_READER
                                                                                                                        #define OPEN_READER(name, gb) OPEN_READER_NOSIZE(name, gb)

#define BITS_AVAILABLE(name, gb) 1
#else
#define OPEN_READER(name, gb)                   \
    OPEN_READER_NOSIZE(name, gb);               \
    unsigned int name ## _size_plus8 = (gb)->size_in_bits_plus8

#define BITS_AVAILABLE(name, gb) name ## _index < name ## _size_plus8
#endif

#define CLOSE_READER(name, gb) (gb)->index = name ## _index

# ifdef LONG_BITSTREAM_READER

                                                                                                                        # define UPDATE_CACHE_LE(name, gb) name ## _cache = \
      AV_RL64((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)

# define UPDATE_CACHE_BE(name, gb) name ## _cache = \
      AV_RB64((gb)->buffer + (name ## _index >> 3)) >> (32 - (name ## _index & 7))

#else

# define UPDATE_CACHE_LE(name, gb) name ## _cache = \
      AV_RL32((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)

# define UPDATE_CACHE_BE(name, gb) name ## _cache = \
      AV_RB32((gb)->buffer + (name ## _index >> 3)) << (name ## _index & 7)

#endif


#ifdef BITSTREAM_READER_LE

                                                                                                                        # define UPDATE_CACHE(name, gb) UPDATE_CACHE_LE(name, gb)

# define SKIP_CACHE(name, gb, num) name ## _cache >>= (num)

#else

# define UPDATE_CACHE(name, gb) UPDATE_CACHE_BE(name, gb)

# define SKIP_CACHE(name, gb, num) name ## _cache <<= (num)

#endif

#if UNCHECKED_BITSTREAM_READER
#   define SKIP_COUNTER(name, gb, num) name ## _index += (num)
#else
#   define SKIP_COUNTER(name, gb, num) \
    name ## _index = FFMIN(name ## _size_plus8, name ## _index + (num))
#endif

#define BITS_LEFT(name, gb) ((int)((gb)->size_in_bits - name ## _index))

#define SKIP_BITS(name, gb, num)                \
    do {                                        \
        SKIP_CACHE(name, gb, num);              \
        SKIP_COUNTER(name, gb, num);            \
    } while (0)

#define LAST_SKIP_BITS(name, gb, num) SKIP_COUNTER(name, gb, num)

#define SHOW_UBITS_LE(name, gb, num) zero_extend(name ## _cache, num)
#define SHOW_SBITS_LE(name, gb, num) sign_extend(name ## _cache, num)

#define SHOW_UBITS_BE(name, gb, num) NEG_USR32(name ## _cache, num)
#define SHOW_SBITS_BE(name, gb, num) NEG_SSR32(name ## _cache, num)

#ifdef BITSTREAM_READER_LE
                                                                                                                        #   define SHOW_UBITS(name, gb, num) SHOW_UBITS_LE(name, gb, num)
#   define SHOW_SBITS(name, gb, num) SHOW_SBITS_LE(name, gb, num)
#else
#   define SHOW_UBITS(name, gb, num) SHOW_UBITS_BE(name, gb, num)
#   define SHOW_SBITS(name, gb, num) SHOW_SBITS_BE(name, gb, num)
#endif

#define GET_CACHE(name, gb) ((uint32_t) name ## _cache)

#endif


#if CACHED_BITSTREAM_READER
                                                                                                                        static inline void refill_32(GetBitContext *s, int is_le)
{
#if !UNCHECKED_BITSTREAM_READER
if (s->index >> 3 >= s->buffer_end - s->buffer)
return;
#endif

if (is_le)
s->cache       = (uint64_t)AV_RL32(s->buffer + (s->index >> 3)) << s->bits_left | s->cache;
else
s->cache       = s->cache | (uint64_t)AV_RB32(s->buffer + (s->index >> 3)) << (32 - s->bits_left);
s->index     += 32;
s->bits_left += 32;
}

static inline void refill_64(GetBitContext *s, int is_le)
{
#if !UNCHECKED_BITSTREAM_READER
if (s->index >> 3 >= s->buffer_end - s->buffer)
return;
#endif

if (is_le)
s->cache = AV_RL64(s->buffer + (s->index >> 3));
else
s->cache = AV_RB64(s->buffer + (s->index >> 3));
s->index += 64;
s->bits_left = 64;
}

static inline uint64_t get_val(GetBitContext *s, unsigned n, int is_le)
{
uint64_t ret;
av_assert2(n>0 && n<=63);
if (is_le) {
ret = s->cache & ((UINT64_C(1) << n) - 1);
s->cache >>= n;
} else {
ret = s->cache >> (64 - n);
s->cache <<= n;
}
s->bits_left -= n;
return ret;
}

static inline unsigned show_val(const GetBitContext *s, unsigned n)
{
#ifdef BITSTREAM_READER_LE
return s->cache & ((UINT64_C(1) << n) - 1);
#else
return s->cache >> (64 - n);
#endif
}
#endif

/**
 * Skips the specified number of bits.
 * @param n the number of bits to skip,
 *          For the UNCHECKED_BITSTREAM_READER this must not cause the distance
 *          from the start to overflow int32_t. Staying within the bitstream + padding
 *          is sufficient, too.
 */
static inline void skip_bits_long(GetBitContext *s, int n) {
#if CACHED_BITSTREAM_READER
    skip_bits(s, n);
#else
#if UNCHECKED_BITSTREAM_READER
    s->index += n;
#else
    s->index += av_clip(n, -s->index, s->size_in_bits_plus8 - s->index);
#endif
#endif
}

#if CACHED_BITSTREAM_READER
                                                                                                                        static inline void skip_remaining(GetBitContext *s, unsigned n)
{
#ifdef BITSTREAM_READER_LE
s->cache >>= n;
#else
s->cache <<= n;
#endif
s->bits_left -= n;
}
#endif

/**
 * Read MPEG-1 dc-style VLC (sign bit + mantissa with no MSB).
 * if MSB not set it is negative
 * @param n length in bits
 */
static inline int get_xbits(GetBitContext *s, int n) {
#if CACHED_BITSTREAM_READER
                                                                                                                            int32_t cache = show_bits(s, 32);
int sign = ~cache >> 31;
skip_remaining(s, n);

return ((((uint32_t)(sign ^ cache)) >> (32 - n)) ^ sign) - sign;
#else
    register int sign;
    register int32_t cache;
    OPEN_READER(re, s);
    av_assert2(n > 0 && n <= 25);
    UPDATE_CACHE(re, s);
    cache = GET_CACHE(re, s);
    sign = ~cache >> 31;
    LAST_SKIP_BITS(re, s, n);
    CLOSE_READER(re, s);
    return (NEG_USR32(sign ^ cache, n) ^ sign) - sign;
#endif
}

#if !CACHED_BITSTREAM_READER

static inline int get_xbits_le(GetBitContext *s, int n) {
    register int sign;
    register int32_t cache;
    OPEN_READER(re, s);
    av_assert2(n > 0 && n <= 25);
    UPDATE_CACHE_LE(re, s);
    cache = GET_CACHE(re, s);
    sign = sign_extend(~cache, n) >> 31;
    LAST_SKIP_BITS(re, s, n);
    CLOSE_READER(re, s);
    return (zero_extend(sign ^ cache, n) ^ sign) - sign;
}

#endif
//
//static inline int get_sbits(GetBitContext *s, int n)
//{
//    register int tmp;
//#if CACHED_BITSTREAM_READER
//    av_assert2(n>0 && n<=25);
//    tmp = sign_extend(get_bits(s, n), n);
//#else
//    OPEN_READER(re, s);
//    av_assert2(n>0 && n<=25);
//    UPDATE_CACHE(re, s);
//    tmp = SHOW_SBITS(re, s, n);
//    LAST_SKIP_BITS(re, s, n);
//    CLOSE_READER(re, s);
//#endif
//    return tmp;
//}

/**
 * Read 1-25 bits.
 */
static inline unsigned int get_bits(GetBitContext *s, int n) {
    register unsigned int tmp;
#if CACHED_BITSTREAM_READER

                                                                                                                            av_assert2(n>0 && n<=32);
if (n > s->bits_left) {
#ifdef BITSTREAM_READER_LE
refill_32(s, 1);
#else
refill_32(s, 0);
#endif
if (s->bits_left < 32)
s->bits_left = n;
}

#ifdef BITSTREAM_READER_LE
tmp = get_val(s, n, 1);
#else
tmp = get_val(s, n, 0);
#endif
#else
    OPEN_READER(re, s);
    av_assert2(n > 0 && n <= 25);
    UPDATE_CACHE(re, s);
    tmp = SHOW_UBITS(re, s, n);
    LAST_SKIP_BITS(re, s, n);
    CLOSE_READER(re, s);
#endif
    av_assert2(tmp < UINT64_C(1) << n);
    return tmp;
}

/**
 * Read 0-25 bits.
 */
static av_always_inline int get_bitsz(GetBitContext *s, int n) {
    return n ? get_bits(s, n) : 0;
}

static inline unsigned int get_bits_le(GetBitContext *s, int n) {
#if CACHED_BITSTREAM_READER
                                                                                                                            av_assert2(n>0 && n<=32);
if (n > s->bits_left) {
refill_32(s, 1);
if (s->bits_left < 32)
s->bits_left = n;
}

return get_val(s, n, 1);
#else
    register int tmp;
    OPEN_READER(re, s);
    av_assert2(n > 0 && n <= 25);
    UPDATE_CACHE_LE(re, s);
    tmp = SHOW_UBITS_LE(re, s, n);
    LAST_SKIP_BITS(re, s, n);
    CLOSE_READER(re, s);
    return tmp;
#endif
}

/**
 * Show 1-25 bits.
 */
static inline unsigned int show_bits(GetBitContext *s, int n) {
    register unsigned int tmp;
#if CACHED_BITSTREAM_READER
                                                                                                                            if (n > s->bits_left)
#ifdef BITSTREAM_READER_LE
refill_32(s, 1);
#else
refill_32(s, 0);
#endif

tmp = show_val(s, n);
#else
    OPEN_READER_NOSIZE(re, s);
    av_assert2(n > 0 && n <= 25);
    UPDATE_CACHE(re, s);
    tmp = SHOW_UBITS(re, s, n);
#endif
    return tmp;
}

static inline void skip_bits(GetBitContext *s, int n) {
#if CACHED_BITSTREAM_READER
                                                                                                                            if (n < s->bits_left)
skip_remaining(s, n);
else {
n -= s->bits_left;
s->cache = 0;
s->bits_left = 0;

if (n >= 64) {
unsigned skip = (n / 8) * 8;

n -= skip;
s->index += skip;
}
#ifdef BITSTREAM_READER_LE
refill_64(s, 1);
#else
refill_64(s, 0);
#endif
if (n)
skip_remaining(s, n);
}
#else
    OPEN_READER(re, s);
    LAST_SKIP_BITS(re, s, n);
    CLOSE_READER(re, s);
#endif
}

static inline unsigned int get_bits1(GetBitContext *s) {
#if CACHED_BITSTREAM_READER
                                                                                                                            if (!s->bits_left)
#ifdef BITSTREAM_READER_LE
refill_64(s, 1);
#else
refill_64(s, 0);
#endif

#ifdef BITSTREAM_READER_LE
return get_val(s, 1, 1);
#else
return get_val(s, 1, 0);
#endif
#else
    unsigned int index = s->index;
    uint8_t result = s->buffer[index >> 3];
#ifdef BITSTREAM_READER_LE
                                                                                                                            result >>= index & 7;
result  &= 1;
#else
    result <<= index & 7;
    result >>= 8 - 1;
#endif
#if !UNCHECKED_BITSTREAM_READER
    if (s->index < s->size_in_bits_plus8)
#endif
    {
        index++;
    }
    s->index = index;

    return result;
#endif
}

static inline unsigned int show_bits1(GetBitContext *s) {
    return show_bits(s, 1);
}

static inline void skip_bits1(GetBitContext *s) {
    skip_bits(s, 1);
}

/**
 * Read 0-32 bits.
 */
static inline unsigned int get_bits_long(GetBitContext *s, int n) {
    av_assert2(n >= 0 && n <= 32);
    if (!n) {
        return 0;
#if CACHED_BITSTREAM_READER
                                                                                                                                }
return get_bits(s, n);
#else
    } else if (n <= MIN_CACHE_BITS) {
        return get_bits(s, n);
    } else {
#ifdef BITSTREAM_READER_LE
                                                                                                                                unsigned ret = get_bits(s, 16);
return ret | (get_bits(s, n - 16) << 16);
#else
        unsigned ret = get_bits(s, 16) << (n - 16);
        return ret | get_bits(s, n - 16);
#endif
    }
#endif
}

/**
 * Read 0-64 bits.
 */
static inline uint64_t get_bits64(GetBitContext *s, int n) {
    if (n <= 32) {
        return get_bits_long(s, n);
    } else {
#ifdef BITSTREAM_READER_LE
                                                                                                                                uint64_t ret = get_bits_long(s, 32);
return ret | (uint64_t) get_bits_long(s, n - 32) << 32;
#else
        uint64_t ret = (uint64_t) get_bits_long(s, n - 32) << 32;
        return ret | get_bits_long(s, 32);
#endif
    }
}

/**
 * Read 0-32 bits as a signed integer.
 */
static inline int get_sbits_long(GetBitContext *s, int n) {
// sign_extend(x, 0) is undefined
    if (!n) {
        return 0;
    }

    return sign_extend(get_bits_long(s, n), n);
}

/**
 * Show 0-32 bits.
 */
static inline unsigned int show_bits_long(GetBitContext *s, int n) {
    if (n <= MIN_CACHE_BITS) {
        return show_bits(s, n);
    } else {
        GetBitContext gb = *s;
        return get_bits_long(&gb, n);
    }
}

static inline int check_marker(void *logctx, GetBitContext *s, const char *msg) {
    int bit = get_bits1(s);
    if (!bit)
            av_log(logctx, AV_LOG_INFO, "Marker bit missing at %d of %d %s\n",
                   get_bits_count(s) - 1, s->size_in_bits, msg) { }

    return bit;
}

static inline int init_get_bits_xe(GetBitContext *s, const uint8_t *buffer, int bit_size, int is_le) {
    int buffer_size;
    int ret = 0;

    if (bit_size >= INT_MAX - FFMAX(7, AV_INPUT_BUFFER_PADDING_SIZE * 8) || bit_size < 0 ||
        !buffer) {
        bit_size = 0;
        buffer = NULL;
        ret = AVERROR_INVALIDDATA;
    }

    buffer_size = (bit_size + 7) >> 3;

    s->buffer = buffer;
    s->size_in_bits = bit_size;
    s->size_in_bits_plus8 = bit_size + 8;
    s->buffer_end = buffer + buffer_size;
    s->index = 0;

#if CACHED_BITSTREAM_READER
                                                                                                                            s->cache              = 0;
s->bits_left          = 0;
refill_64(s, is_le);
#endif

    return ret;
}

/**
 * Initialize GetBitContext.
 * @param buffer bitstream buffer, must be AV_INPUT_BUFFER_PADDING_SIZE bytes
 *        larger than the actual read bits because some optimized bitstream
 *        readers read 32 or 64 bit at once and could read over the end
 * @param bit_size the size of the buffer in bits
 * @return 0 on success, AVERROR_INVALIDDATA if the buffer_size would overflow.
 */
static inline int init_get_bits(GetBitContext *s, const uint8_t *buffer, int bit_size) {
#ifdef BITSTREAM_READER_LE
    return init_get_bits_xe(s, buffer, bit_size, 1);
#else
    return init_get_bits_xe(s, buffer, bit_size, 0);
#endif
}

/**
 * Initialize GetBitContext.
 * @param buffer bitstream buffer, must be AV_INPUT_BUFFER_PADDING_SIZE bytes
 *        larger than the actual read bits because some optimized bitstream
 *        readers read 32 or 64 bit at once and could read over the end
 * @param byte_size the size of the buffer in bytes
 * @return 0 on success, AVERROR_INVALIDDATA if the buffer_size would overflow.
 */
static inline int init_get_bits8(GetBitContext *s, const uint8_t *buffer, int byte_size) {
    if (byte_size > INT_MAX / 8 || byte_size < 0) {
        byte_size = -1;
    }
    return init_get_bits(s, buffer, byte_size * 8);
}

static inline int init_get_bits8_le(GetBitContext *s, const uint8_t *buffer, int byte_size) {
    if (byte_size > INT_MAX / 8 || byte_size < 0) {
        byte_size = -1;
    }
    return init_get_bits_xe(s, buffer, byte_size * 8, 1);
}

static inline const uint8_t *align_get_bits(GetBitContext *s) {
    int n = -get_bits_count(s) & 7;
    if (n) {
        skip_bits(s, n);
    }
    return s->buffer + (s->index >> 3);
}

/**
 * If the vlc code is invalid and max_depth=1, then no bits will be removed.
 * If the vlc code is invalid and max_depth>1, then the number of bits removed
 * is undefined.
 */
#define GET_VLC(code, name, gb, table, bits, max_depth)         \
    do {                                                        \
        int n, nb_bits;                                         \
        unsigned int index;                                     \
                                                                \
        index = SHOW_UBITS(name, gb, bits);                     \
        code  = table[index][0];                                \
        n     = table[index][1];                                \
                                                                \
        if (max_depth > 1 && n < 0) {                           \
            LAST_SKIP_BITS(name, gb, bits);                     \
            UPDATE_CACHE(name, gb);                             \
                                                                \
            nb_bits = -n;                                       \
                                                                \
            index = SHOW_UBITS(name, gb, nb_bits) + code;       \
            code  = table[index][0];                            \
            n     = table[index][1];                            \
            if (max_depth > 2 && n < 0) {                       \
                LAST_SKIP_BITS(name, gb, nb_bits);              \
                UPDATE_CACHE(name, gb);                         \
                                                                \
                nb_bits = -n;                                   \
                                                                \
                index = SHOW_UBITS(name, gb, nb_bits) + code;   \
                code  = table[index][0];                        \
                n     = table[index][1];                        \
            }                                                   \
        }                                                       \
        SKIP_BITS(name, gb, n);                                 \
    } while (0)

#define GET_RL_VLC(level, run, name, gb, table, bits, \
                   max_depth, need_update)                      \
    do {                                                        \
        int n, nb_bits;                                         \
        unsigned int index;                                     \
                                                                \
        index = SHOW_UBITS(name, gb, bits);                     \
        level = table[index].level;                             \
        n     = table[index].len;                               \
                                                                \
        if (max_depth > 1 && n < 0) {                           \
            SKIP_BITS(name, gb, bits);                          \
            if (need_update) {                                  \
                UPDATE_CACHE(name, gb);                         \
            }                                                   \
                                                                \
            nb_bits = -n;                                       \
                                                                \
            index = SHOW_UBITS(name, gb, nb_bits) + level;      \
            level = table[index].level;                         \
            n     = table[index].len;                           \
            if (max_depth > 2 && n < 0) {                       \
                LAST_SKIP_BITS(name, gb, nb_bits);              \
                if (need_update) {                              \
                    UPDATE_CACHE(name, gb);                     \
                }                                               \
                nb_bits = -n;                                   \
                                                                \
                index = SHOW_UBITS(name, gb, nb_bits) + level;  \
                level = table[index].level;                     \
                n     = table[index].len;                       \
            }                                                   \
        }                                                       \
        run = table[index].run;                                 \
        SKIP_BITS(name, gb, n);                                 \
    } while (0)

/* Return the LUT element for the given bitstream configuration. */
static inline int set_idx(GetBitContext *s, int code, int *n, int *nb_bits, VLC_TYPE (*table)[2]) {
    unsigned idx;

    *nb_bits = -*n;
    idx = show_bits(s, *nb_bits) + code;
    *n = table[idx][1];

    return table[idx][0];
}

/**
 * Parse a vlc code.
 * @param bits is the number of bits which will be read at once, must be
 *             identical to nb_bits in init_vlc()
 * @param max_depth is the number of times bits bits must be read to completely
 *                  read the longest vlc code
 *                  = (max_vlc_length + bits - 1) / bits
 * @returns the code parsed or -1 if no vlc matches
 */
static av_always_inline int get_vlc2(GetBitContext *s, VLC_TYPE (*table)[2], int bits, int max_depth) {
#if CACHED_BITSTREAM_READER
                                                                                                                            int nb_bits;
unsigned idx = show_bits(s, bits);
int code = table[idx][0];
int n    = table[idx][1];

if (max_depth > 1 && n < 0) {
skip_remaining(s, bits);
code = set_idx(s, code, &n, &nb_bits, table);
if (max_depth > 2 && n < 0) {
skip_remaining(s, nb_bits);
code = set_idx(s, code, &n, &nb_bits, table);
}
}
skip_remaining(s, n);

return code;
#else
    int code;

    OPEN_READER(re, s);
    UPDATE_CACHE(re, s);

    GET_VLC(code, re, s, table, bits, max_depth);

    CLOSE_READER(re, s);

    return code;
#endif
}

static inline int decode012(GetBitContext *gb) {
    int n;
    n = get_bits1(gb);
    if (n == 0) {
        return 0;
    } else {
        return get_bits1(gb) + 1;
    }
}

static inline int decode210(GetBitContext *gb) {
    if (get_bits1(gb)) {
        return 0;
    } else {
        return 2 - get_bits1(gb);
    }
}

static inline int get_bits_left(GetBitContext *gb) {
    return gb->size_in_bits - get_bits_count(gb);
}

static inline int skip_1stop_8data_bits(GetBitContext *gb) {
    if (get_bits_left(gb) <= 0) {
        return AVERROR_INVALIDDATA;
    }

    while (get_bits1(gb)) {
        skip_bits(gb, 8);
        if (get_bits_left(gb) <= 0) {
            return AVERROR_INVALIDDATA;
        }
    }

    return 0;
}


enum AVFrameSideDataType {
/**
     * The data is the AVPanScan struct defined in libavcodec.
     */
    AV_FRAME_DATA_PANSCAN, /**
     * ATSC A53 Part 4 Closed Captions.
     * A53 CC bitstream is stored as uint8_t in AVFrameSideData.data.
     * The number of bytes of CC data is AVFrameSideData.size.
     */
    AV_FRAME_DATA_A53_CC, /**
     * Stereoscopic 3d metadata.
     * The data is the AVStereo3D struct defined in libavutil/stereo3d.h.
     */
    AV_FRAME_DATA_STEREO3D, /**
     * The data is the AVMatrixEncoding enum defined in libavutil/channel_layout.h.
     */
    AV_FRAME_DATA_MATRIXENCODING, /**
     * Metadata relevant to a downmix procedure.
     * The data is the AVDownmixInfo struct defined in libavutil/downmix_info.h.
     */
    AV_FRAME_DATA_DOWNMIX_INFO, /**
     * ReplayGain information in the form of the AVReplayGain struct.
     */
    AV_FRAME_DATA_REPLAYGAIN, /**
     * This side data contains a 3x3 transformation matrix describing an affine
     * transformation that needs to be applied to the frame for correct
     * presentation.
     *
     * See libavutil/display.h for a detailed description of the data.
     */
    AV_FRAME_DATA_DISPLAYMATRIX, /**
     * Active Format Description data consisting of a single byte as specified
     * in ETSI TS 101 154 using AVActiveFormatDescription enum.
     */
    AV_FRAME_DATA_AFD, /**
     * Motion vectors exported by some codecs (on demand through the export_mvs
     * flag set in the libavcodec AVCodecContext flags2 option).
     * The data is the AVMotionVector struct defined in
     * libavutil/motion_vector.h.
     */
    AV_FRAME_DATA_MOTION_VECTORS, /**
     * Recommmends skipping the specified number of samples. This is exported
     * only if the "skip_manual" AVOption is set in libavcodec.
     * This has the same format as AV_PKT_DATA_SKIP_SAMPLES.
     * @code
     * u32le number of samples to skip from start of this packet
     * u32le number of samples to skip from end of this packet
     * u8    reason for start skip
     * u8    reason for end   skip (0=padding silence, 1=convergence)
     * @endcode
     */
    AV_FRAME_DATA_SKIP_SAMPLES, /**
     * This side data must be associated with an audio frame and corresponds to
     * enum AVAudioServiceType defined in avcodec.h.
     */
    AV_FRAME_DATA_AUDIO_SERVICE_TYPE, /**
     * Mastering display metadata associated with a video frame. The payload is
     * an AVMasteringDisplayMetadata type and contains information about the
     * mastering display color volume.
     */
    AV_FRAME_DATA_MASTERING_DISPLAY_METADATA, /**
     * The GOP timecode in 25 bit timecode format. Data format is 64-bit integer.
     * This is set on the first frame of a GOP that has a temporal reference of 0.
     */
    AV_FRAME_DATA_GOP_TIMECODE,

/**
     * The data represents the AVSphericalMapping structure defined in
     * libavutil/spherical.h.
     */
    AV_FRAME_DATA_SPHERICAL,

/**
     * Content light level (based on CTA-861.3). This payload contains data in
     * the form of the AVContentLightMetadata struct.
     */
    AV_FRAME_DATA_CONTENT_LIGHT_LEVEL,

/**
     * The data contains an ICC profile as an opaque octet buffer following the
     * format described by ISO 15076-1 with an optional name defined in the
     * metadata key entry "name".
     */
    AV_FRAME_DATA_ICC_PROFILE,

#if FF_API_FRAME_QP
                                                                                                                            /**
     * Implementation-specific description of the format of AV_FRAME_QP_TABLE_DATA.
     * The contents of this side data are undocumented and internal; use
     * av_frame_set_qp_table() and av_frame_get_qp_table() to access this in a
     * meaningful way instead.
     */
AV_FRAME_DATA_QP_TABLE_PROPERTIES,

/**
     * Raw QP table data. Its format is described by
     * AV_FRAME_DATA_QP_TABLE_PROPERTIES. Use av_frame_set_qp_table() and
     * av_frame_get_qp_table() to access this instead.
     */
AV_FRAME_DATA_QP_TABLE_DATA,
#endif

/**
     * Timecode which conforms to SMPTE ST 12-1. The data is an array of 4 uint32_t
     * where the first uint32_t describes how many (1-3) of the other timecodes are used.
     * The timecode format is described in the av_timecode_get_smpte_from_framenum()
     * function in libavutil/timecode.c.
     */
    AV_FRAME_DATA_S12M_TIMECODE,

/**
     * HDR dynamic metadata associated with a video frame. The payload is
     * an AVDynamicHDRPlus type and contains information for color
     * volume transform - application 4 of SMPTE 2094-40:2016 standard.
     */
    AV_FRAME_DATA_DYNAMIC_HDR_PLUS,

/**
     * Regions Of Interest, the data is an array of AVRegionOfInterest type, the number of
     * array element is implied by AVFrameSideData.size / AVRegionOfInterest.self_size.
     */
    AV_FRAME_DATA_REGIONS_OF_INTEREST,
};


enum AVFieldOrder {
    AV_FIELD_UNKNOWN,
    AV_FIELD_PROGRESSIVE,
    AV_FIELD_TT,          //< Top coded_first, top displayed first
    AV_FIELD_BB,          //< Bottom coded first, bottom displayed first
    AV_FIELD_TB,          //< Top coded first, bottom displayed first
    AV_FIELD_BT,          //< Bottom coded first, top displayed first
};


enum AVPictureStructure {
    AV_PICTURE_STRUCTURE_UNKNOWN,      //< unknown
    AV_PICTURE_STRUCTURE_TOP_FIELD,    //< coded as top field
    AV_PICTURE_STRUCTURE_BOTTOM_FIELD, //< coded as bottom field
    AV_PICTURE_STRUCTURE_FRAME,        //< coded as frame
};


enum AVSampleFormat {
    AV_SAMPLE_FMT_NONE = -1, AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
    AV_SAMPLE_FMT_S16,         ///< signed 16 bits
    AV_SAMPLE_FMT_S32,         ///< signed 32 bits
    AV_SAMPLE_FMT_FLT,         ///< float
    AV_SAMPLE_FMT_DBL,         ///< double

    AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
    AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
    AV_SAMPLE_FMT_FLTP,        ///< float, planar
    AV_SAMPLE_FMT_DBLP,        ///< double, planar
    AV_SAMPLE_FMT_S64,         ///< signed 64 bits
    AV_SAMPLE_FMT_S64P,        ///< signed 64 bits, planar

    AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};


enum AVDiscard {
/* We leave some space between them for extensions (drop some
     * keyframes for intra-only or drop just some bidir frames). */
    AVDISCARD_NONE = -16, ///< discard nothing
    AVDISCARD_DEFAULT = 0, ///< discard useless packets like 0 size packets in avi
    AVDISCARD_NONREF = 8, ///< discard all non reference
    AVDISCARD_BIDIR = 16, ///< discard all bidirectional frames
    AVDISCARD_NONINTRA = 24, ///< discard all non intra frames
    AVDISCARD_NONKEY = 32, ///< discard all frames except keyframes
    AVDISCARD_ALL = 48, ///< discard all
};

enum AVAudioServiceType {
    AV_AUDIO_SERVICE_TYPE_MAIN = 0,
    AV_AUDIO_SERVICE_TYPE_EFFECTS = 1,
    AV_AUDIO_SERVICE_TYPE_VISUALLY_IMPAIRED = 2,
    AV_AUDIO_SERVICE_TYPE_HEARING_IMPAIRED = 3,
    AV_AUDIO_SERVICE_TYPE_DIALOGUE = 4,
    AV_AUDIO_SERVICE_TYPE_COMMENTARY = 5,
    AV_AUDIO_SERVICE_TYPE_EMERGENCY = 6,
    AV_AUDIO_SERVICE_TYPE_VOICE_OVER = 7,
    AV_AUDIO_SERVICE_TYPE_KARAOKE = 8,
    AV_AUDIO_SERVICE_TYPE_NB, ///< Not part of ABI
};

#define attribute_deprecated


#define AV_BUFFER_FLAG_READONLY (1 << 0)


#define BUFFER_FLAG_READONLY      (1 << 0)
/**
 * The buffer was av_realloc()ed, so it is reallocatable.
 */
#define BUFFER_FLAG_REALLOCATABLE (1 << 1)


#define ATOMIC_FLAG_INIT 0

#define ATOMIC_VAR_INIT(value) (value)



//
//typedef intptr_t atomic_flag;
//typedef intptr_t atomic_bool;
//typedef intptr_t atomic_char;
//typedef intptr_t atomic_schar;
//typedef intptr_t atomic_uchar;
//typedef intptr_t atomic_short;
//typedef intptr_t atomic_ushort;
//typedef intptr_t atomic_int;
//typedef intptr_t atomic_uint;
//typedef intptr_t atomic_long;
//typedef intptr_t atomic_ulong;
//typedef intptr_t atomic_llong;
//typedef intptr_t atomic_ullong;
//typedef intptr_t atomic_wchar_t;
//typedef intptr_t atomic_int_least8_t;
//typedef intptr_t atomic_uint_least8_t;
//typedef intptr_t atomic_int_least16_t;
//typedef intptr_t atomic_uint_least16_t;
//typedef intptr_t atomic_int_least32_t;
//typedef intptr_t atomic_uint_least32_t;
//typedef intptr_t atomic_int_least64_t;
//typedef intptr_t atomic_uint_least64_t;
//typedef intptr_t atomic_int_fast8_t;
//typedef intptr_t atomic_uint_fast8_t;
//typedef intptr_t atomic_int_fast16_t;
//typedef intptr_t atomic_uint_fast16_t;
//typedef intptr_t atomic_int_fast32_t;
//typedef intptr_t atomic_uint_fast32_t;
//typedef intptr_t atomic_int_fast64_t;
//typedef intptr_t atomic_uint_fast64_t;
//typedef intptr_t atomic_intptr_t;
//typedef intptr_t atomic_uintptr_t;
//typedef intptr_t atomic_size_t;
//typedef intptr_t atomic_ptrdiff_t;
//typedef intptr_t atomic_intmax_t;
//typedef intptr_t atomic_uintmax_t;
//


struct AVBuffer {
    uint8_t *data; /**< data described by this buffer */
    int size; /**< size of data in bytes */

/**
     *  number of existing AVBufferRef instances referring to this buffer
     */
    atomic_uint refcount;

/**
     * a callback for freeing the data
     */
    void (*free)(void *opaque, uint8_t *data);

/**
     * an opaque pointer, to be used by the freeing callback
     */
    void *opaque;

/**
     * A combination of BUFFER_FLAG_*
     */
    int flags;
};


typedef struct AVBuffer AVBuffer;

/**
 * A reference to a data buffer.
 *
 * The size of this struct is not a part of the public ABI and it is not meant
 * to be allocated directly.
 */
typedef struct AVBufferRef {
    AVBuffer *buffer;

/**
     * The data buffer. It is considered writable if and only if
     * this is the only reference to the buffer, in which case
     * av_buffer_is_writable() returns 1.
     */
    uint8_t *data;
/**
     * Size of data in bytes.
     */
    int size;
} AVBufferRef;


typedef struct AVComponentDescriptor {
/**
     * Which of the 4 planes contains the component.
     */
    int plane;

/**
     * Number of elements between 2 horizontally consecutive pixels.
     * Elements are bits for bitstream formats, bytes otherwise.
     */
    int step;

/**
     * Number of elements before the component of the first pixel.
     * Elements are bits for bitstream formats, bytes otherwise.
     */
    int offset;

/**
     * Number of least significant bits that must be shifted away
     * to get the value.
     */
    int shift;

/**
     * Number of bits in the component.
     */
    int depth;

#if FF_API_PLUS1_MINUS1
                                                                                                                            /** deprecated, use step instead */
attribute_deprecated int step_minus1;

/** deprecated, use depth instead */
attribute_deprecated int depth_minus1;

/** deprecated, use offset instead */
attribute_deprecated int offset_plus1;
#endif
} AVComponentDescriptor;


enum AVPixelFormat {
    AV_PIX_FMT_NONE = -1,
    AV_PIX_FMT_YUV420P,   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    AV_PIX_FMT_YUYV422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    AV_PIX_FMT_RGB24,     ///< packed RGB 8:8:8, 24bpp, RGBRGB...
    AV_PIX_FMT_BGR24,     ///< packed RGB 8:8:8, 24bpp, BGRBGR...
    AV_PIX_FMT_YUV422P,   ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    AV_PIX_FMT_YUV444P,   ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    AV_PIX_FMT_YUV410P,   ///< planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
    AV_PIX_FMT_YUV411P,   ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
    AV_PIX_FMT_GRAY8,     ///<        Y        ,  8bpp
    AV_PIX_FMT_MONOWHITE, ///<        Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
    AV_PIX_FMT_MONOBLACK, ///<        Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
    AV_PIX_FMT_PAL8,      ///< 8 bits with AV_PIX_FMT_RGB32 palette
    AV_PIX_FMT_YUVJ420P,  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV420P and setting color_range
    AV_PIX_FMT_YUVJ422P,  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV422P and setting color_range
    AV_PIX_FMT_YUVJ444P,  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV444P and setting color_range
    AV_PIX_FMT_UYVY422,   ///< packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
    AV_PIX_FMT_UYYVYY411, ///< packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
    AV_PIX_FMT_BGR8,      ///< packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
    AV_PIX_FMT_BGR4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    AV_PIX_FMT_BGR4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
    AV_PIX_FMT_RGB8,      ///< packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
    AV_PIX_FMT_RGB4,      ///< packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    AV_PIX_FMT_RGB4_BYTE, ///< packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
    AV_PIX_FMT_NV12,      ///< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    AV_PIX_FMT_NV21,      ///< as above, but U and V bytes are swapped

    AV_PIX_FMT_ARGB,      ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    AV_PIX_FMT_RGBA,      ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    AV_PIX_FMT_ABGR,      ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
    AV_PIX_FMT_BGRA,      ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...

    AV_PIX_FMT_GRAY16BE,  ///<        Y        , 16bpp, big-endian
    AV_PIX_FMT_GRAY16LE,  ///<        Y        , 16bpp, little-endian
    AV_PIX_FMT_YUV440P,   ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
    AV_PIX_FMT_YUVJ440P,  ///< planar YUV 4:4:0 full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV440P and setting color_range
    AV_PIX_FMT_YUVA420P,  ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
    AV_PIX_FMT_RGB48BE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian
    AV_PIX_FMT_RGB48LE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian

    AV_PIX_FMT_RGB565BE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
    AV_PIX_FMT_RGB565LE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
    AV_PIX_FMT_RGB555BE,  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), big-endian   , X=unused/undefined
    AV_PIX_FMT_RGB555LE,  ///< packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), little-endian, X=unused/undefined

    AV_PIX_FMT_BGR565BE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
    AV_PIX_FMT_BGR565LE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
    AV_PIX_FMT_BGR555BE,  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), big-endian   , X=unused/undefined
    AV_PIX_FMT_BGR555LE,  ///< packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), little-endian, X=unused/undefined

#if FF_API_VAAPI
                                                                                                                            /** @name Deprecated pixel formats */
/**@{*/
AV_PIX_FMT_VAAPI_MOCO, ///< HW acceleration through VA API at motion compensation entry-point, Picture.data[3] contains a vaapi_render_state struct which contains macroblocks as well as various fields extracted from headers
AV_PIX_FMT_VAAPI_IDCT, ///< HW acceleration through VA API at IDCT entry-point, Picture.data[3] contains a vaapi_render_state struct which contains fields extracted from headers
AV_PIX_FMT_VAAPI_VLD,  ///< HW decoding through VA API, Picture.data[3] contains a VASurfaceID
/**@}*/
AV_PIX_FMT_VAAPI = AV_PIX_FMT_VAAPI_VLD,
#else
/**
     *  Hardware acceleration through VA-API, data[3] contains a
     *  VASurfaceID.
     */
    AV_PIX_FMT_VAAPI,
#endif

    AV_PIX_FMT_YUV420P16LE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV420P16BE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV422P16LE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV422P16BE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P16LE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P16BE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_DXVA2_VLD,    ///< HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer

    AV_PIX_FMT_RGB444LE,  ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), little-endian, X=unused/undefined
    AV_PIX_FMT_RGB444BE,  ///< packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), big-endian,    X=unused/undefined
    AV_PIX_FMT_BGR444LE,  ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), little-endian, X=unused/undefined
    AV_PIX_FMT_BGR444BE,  ///< packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), big-endian,    X=unused/undefined
    AV_PIX_FMT_YA8,       ///< 8 bits gray, 8 bits alpha

    AV_PIX_FMT_Y400A = AV_PIX_FMT_YA8, ///< alias for AV_PIX_FMT_YA8
    AV_PIX_FMT_GRAY8A = AV_PIX_FMT_YA8, ///< alias for AV_PIX_FMT_YA8

    AV_PIX_FMT_BGR48BE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
    AV_PIX_FMT_BGR48LE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian

/**
     * The following 12 formats have the disadvantage of needing 1 format for each bit depth.
     * Notice that each 9/10 bits sample is stored in 16 bits with extra padding.
     * If you want to support multiple bit depths, then using AV_PIX_FMT_YUV420P16* with the bpp stored separately is better.
     */
    AV_PIX_FMT_YUV420P9BE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P9LE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV420P10BE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P10LE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV422P10BE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P10LE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P9BE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P9LE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P10BE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P10LE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV422P9BE, ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P9LE, ///< planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_GBRP,      ///< planar GBR 4:4:4 24bpp
    AV_PIX_FMT_GBR24P = AV_PIX_FMT_GBRP, // alias for #AV_PIX_FMT_GBRP
    AV_PIX_FMT_GBRP9BE,   ///< planar GBR 4:4:4 27bpp, big-endian
    AV_PIX_FMT_GBRP9LE,   ///< planar GBR 4:4:4 27bpp, little-endian
    AV_PIX_FMT_GBRP10BE,  ///< planar GBR 4:4:4 30bpp, big-endian
    AV_PIX_FMT_GBRP10LE,  ///< planar GBR 4:4:4 30bpp, little-endian
    AV_PIX_FMT_GBRP16BE,  ///< planar GBR 4:4:4 48bpp, big-endian
    AV_PIX_FMT_GBRP16LE,  ///< planar GBR 4:4:4 48bpp, little-endian
    AV_PIX_FMT_YUVA422P,  ///< planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
    AV_PIX_FMT_YUVA444P,  ///< planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)
    AV_PIX_FMT_YUVA420P9BE,  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian
    AV_PIX_FMT_YUVA420P9LE,  ///< planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian
    AV_PIX_FMT_YUVA422P9BE,  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian
    AV_PIX_FMT_YUVA422P9LE,  ///< planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian
    AV_PIX_FMT_YUVA444P9BE,  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian
    AV_PIX_FMT_YUVA444P9LE,  ///< planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
    AV_PIX_FMT_YUVA420P10BE, ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA420P10LE, ///< planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA422P10BE, ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA422P10LE, ///< planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA444P10BE, ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA444P10LE, ///< planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA420P16BE, ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA420P16LE, ///< planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA422P16BE, ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA422P16LE, ///< planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    AV_PIX_FMT_YUVA444P16BE, ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    AV_PIX_FMT_YUVA444P16LE, ///< planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)

    AV_PIX_FMT_VDPAU,     ///< HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface

    AV_PIX_FMT_XYZ12LE,      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0
    AV_PIX_FMT_XYZ12BE,      ///< packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0
    AV_PIX_FMT_NV16,         ///< interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    AV_PIX_FMT_NV20LE,       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_NV20BE,       ///< interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian

    AV_PIX_FMT_RGBA64BE,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    AV_PIX_FMT_RGBA64LE,     ///< packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
    AV_PIX_FMT_BGRA64BE,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    AV_PIX_FMT_BGRA64LE,     ///< packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian

    AV_PIX_FMT_YVYU422,   ///< packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb

    AV_PIX_FMT_YA16BE,       ///< 16 bits gray, 16 bits alpha (big-endian)
    AV_PIX_FMT_YA16LE,       ///< 16 bits gray, 16 bits alpha (little-endian)

    AV_PIX_FMT_GBRAP,        ///< planar GBRA 4:4:4:4 32bpp
    AV_PIX_FMT_GBRAP16BE,    ///< planar GBRA 4:4:4:4 64bpp, big-endian
    AV_PIX_FMT_GBRAP16LE,    ///< planar GBRA 4:4:4:4 64bpp, little-endian
/**
     *  HW acceleration through QSV, data[3] contains a pointer to the
     *  mfxFrameSurface1 structure.
     */
    AV_PIX_FMT_QSV, /**
     * HW acceleration though MMAL, data[3] contains a pointer to the
     * MMAL_BUFFER_HEADER_T structure.
     */
    AV_PIX_FMT_MMAL,

    AV_PIX_FMT_D3D11VA_VLD,  ///< HW decoding through Direct3D11 via old API, Picture.data[3] contains a ID3D11VideoDecoderOutputView pointer

/**
     * HW acceleration through CUDA. data[i] contain CUdeviceptr pointers
     * exactly as for system memory frames.
     */
    AV_PIX_FMT_CUDA,

    AV_PIX_FMT_0RGB,        ///< packed RGB 8:8:8, 32bpp, XRGBXRGB...   X=unused/undefined
    AV_PIX_FMT_RGB0,        ///< packed RGB 8:8:8, 32bpp, RGBXRGBX...   X=unused/undefined
    AV_PIX_FMT_0BGR,        ///< packed BGR 8:8:8, 32bpp, XBGRXBGR...   X=unused/undefined
    AV_PIX_FMT_BGR0,        ///< packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined

    AV_PIX_FMT_YUV420P12BE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P12LE, ///< planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV420P14BE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    AV_PIX_FMT_YUV420P14LE, ///< planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    AV_PIX_FMT_YUV422P12BE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P12LE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV422P14BE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    AV_PIX_FMT_YUV422P14LE, ///< planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P12BE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P12LE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_YUV444P14BE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    AV_PIX_FMT_YUV444P14LE, ///< planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    AV_PIX_FMT_GBRP12BE,    ///< planar GBR 4:4:4 36bpp, big-endian
    AV_PIX_FMT_GBRP12LE,    ///< planar GBR 4:4:4 36bpp, little-endian
    AV_PIX_FMT_GBRP14BE,    ///< planar GBR 4:4:4 42bpp, big-endian
    AV_PIX_FMT_GBRP14LE,    ///< planar GBR 4:4:4 42bpp, little-endian
    AV_PIX_FMT_YUVJ411P,    ///< planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV411P and setting color_range

    AV_PIX_FMT_BAYER_BGGR8,    ///< bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples */
    AV_PIX_FMT_BAYER_RGGB8,    ///< bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples */
    AV_PIX_FMT_BAYER_GBRG8,    ///< bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples */
    AV_PIX_FMT_BAYER_GRBG8,    ///< bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples */
    AV_PIX_FMT_BAYER_BGGR16LE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, little-endian */
    AV_PIX_FMT_BAYER_BGGR16BE, ///< bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, big-endian */
    AV_PIX_FMT_BAYER_RGGB16LE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, little-endian */
    AV_PIX_FMT_BAYER_RGGB16BE, ///< bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, big-endian */
    AV_PIX_FMT_BAYER_GBRG16LE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, little-endian */
    AV_PIX_FMT_BAYER_GBRG16BE, ///< bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, big-endian */
    AV_PIX_FMT_BAYER_GRBG16LE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, little-endian */
    AV_PIX_FMT_BAYER_GRBG16BE, ///< bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, big-endian */

    AV_PIX_FMT_XVMC,///< XVideo Motion Acceleration via common packet passing

    AV_PIX_FMT_YUV440P10LE, ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
    AV_PIX_FMT_YUV440P10BE, ///< planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
    AV_PIX_FMT_YUV440P12LE, ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
    AV_PIX_FMT_YUV440P12BE, ///< planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
    AV_PIX_FMT_AYUV64LE,    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
    AV_PIX_FMT_AYUV64BE,    ///< packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), big-endian

    AV_PIX_FMT_VIDEOTOOLBOX, ///< hardware decoding through Videotoolbox

    AV_PIX_FMT_P010LE, ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, little-endian
    AV_PIX_FMT_P010BE, ///< like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, big-endian

    AV_PIX_FMT_GBRAP12BE,  ///< planar GBR 4:4:4:4 48bpp, big-endian
    AV_PIX_FMT_GBRAP12LE,  ///< planar GBR 4:4:4:4 48bpp, little-endian

    AV_PIX_FMT_GBRAP10BE,  ///< planar GBR 4:4:4:4 40bpp, big-endian
    AV_PIX_FMT_GBRAP10LE,  ///< planar GBR 4:4:4:4 40bpp, little-endian

    AV_PIX_FMT_MEDIACODEC, ///< hardware decoding through MediaCodec

    AV_PIX_FMT_GRAY12BE,   ///<        Y        , 12bpp, big-endian
    AV_PIX_FMT_GRAY12LE,   ///<        Y        , 12bpp, little-endian
    AV_PIX_FMT_GRAY10BE,   ///<        Y        , 10bpp, big-endian
    AV_PIX_FMT_GRAY10LE,   ///<        Y        , 10bpp, little-endian

    AV_PIX_FMT_P016LE, ///< like NV12, with 16bpp per component, little-endian
    AV_PIX_FMT_P016BE, ///< like NV12, with 16bpp per component, big-endian

/**
     * Hardware surfaces for Direct3D11.
     *
     * This is preferred over the legacy AV_PIX_FMT_D3D11VA_VLD. The new D3D11
     * hwaccel API and filtering support AV_PIX_FMT_D3D11 only.
     *
     * data[0] contains a ID3D11Texture2D pointer, and data[1] contains the
     * texture array index of the frame as intptr_t if the ID3D11Texture2D is
     * an array texture (or always 0 if it's a normal texture).
     */
    AV_PIX_FMT_D3D11,

    AV_PIX_FMT_GRAY9BE,   ///<        Y        , 9bpp, big-endian
    AV_PIX_FMT_GRAY9LE,   ///<        Y        , 9bpp, little-endian

    AV_PIX_FMT_GBRPF32BE,  ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, big-endian
    AV_PIX_FMT_GBRPF32LE,  ///< IEEE-754 single precision planar GBR 4:4:4,     96bpp, little-endian
    AV_PIX_FMT_GBRAPF32BE, ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, big-endian
    AV_PIX_FMT_GBRAPF32LE, ///< IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, little-endian

/**
     * DRM-managed buffers exposed through PRIME buffer sharing.
     *
     * data[0] points to an AVDRMFrameDescriptor.
     */
    AV_PIX_FMT_DRM_PRIME, /**
     * Hardware surfaces for OpenCL.
     *
     * data[i] contain 2D image objects (typed in C as cl_mem, used
     * in OpenCL as image2d_t) for each plane of the surface.
     */
    AV_PIX_FMT_OPENCL,

    AV_PIX_FMT_GRAY14BE,   ///<        Y        , 14bpp, big-endian
    AV_PIX_FMT_GRAY14LE,   ///<        Y        , 14bpp, little-endian

    AV_PIX_FMT_GRAYF32BE,  ///< IEEE-754 single precision Y, 32bpp, big-endian
    AV_PIX_FMT_GRAYF32LE,  ///< IEEE-754 single precision Y, 32bpp, little-endian

    AV_PIX_FMT_YUVA422P12BE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, big-endian
    AV_PIX_FMT_YUVA422P12LE, ///< planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, little-endian
    AV_PIX_FMT_YUVA444P12BE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, big-endian
    AV_PIX_FMT_YUVA444P12LE, ///< planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, little-endian

    AV_PIX_FMT_NV24,      ///< planar YUV 4:4:4, 24bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    AV_PIX_FMT_NV42,      ///< as above, but U and V bytes are swapped

    AV_PIX_FMT_NB         ///< number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of formats might differ between versions
};

#if AV_HAVE_BIGENDIAN
#   define AV_PIX_FMT_NE(be, le) AV_PIX_FMT_##be
#else
#   define AV_PIX_FMT_NE(be, le) AV_PIX_FMT_##le
#endif

#define AV_PIX_FMT_RGB32   AV_PIX_FMT_NE(ARGB, BGRA)
#define AV_PIX_FMT_RGB32_1 AV_PIX_FMT_NE(RGBA, ABGR)
#define AV_PIX_FMT_BGR32   AV_PIX_FMT_NE(ABGR, RGBA)
#define AV_PIX_FMT_BGR32_1 AV_PIX_FMT_NE(BGRA, ARGB)
#define AV_PIX_FMT_0RGB32  AV_PIX_FMT_NE(0RGB, BGR0)
#define AV_PIX_FMT_0BGR32  AV_PIX_FMT_NE(0BGR, RGB0)

#define AV_PIX_FMT_GRAY9  AV_PIX_FMT_NE(GRAY9BE,  GRAY9LE)
#define AV_PIX_FMT_GRAY10 AV_PIX_FMT_NE(GRAY10BE, GRAY10LE)
#define AV_PIX_FMT_GRAY12 AV_PIX_FMT_NE(GRAY12BE, GRAY12LE)
#define AV_PIX_FMT_GRAY14 AV_PIX_FMT_NE(GRAY14BE, GRAY14LE)
#define AV_PIX_FMT_GRAY16 AV_PIX_FMT_NE(GRAY16BE, GRAY16LE)
#define AV_PIX_FMT_YA16   AV_PIX_FMT_NE(YA16BE,   YA16LE)
#define AV_PIX_FMT_RGB48  AV_PIX_FMT_NE(RGB48BE,  RGB48LE)
#define AV_PIX_FMT_RGB565 AV_PIX_FMT_NE(RGB565BE, RGB565LE)
#define AV_PIX_FMT_RGB555 AV_PIX_FMT_NE(RGB555BE, RGB555LE)
#define AV_PIX_FMT_RGB444 AV_PIX_FMT_NE(RGB444BE, RGB444LE)
#define AV_PIX_FMT_RGBA64 AV_PIX_FMT_NE(RGBA64BE, RGBA64LE)
#define AV_PIX_FMT_BGR48  AV_PIX_FMT_NE(BGR48BE,  BGR48LE)
#define AV_PIX_FMT_BGR565 AV_PIX_FMT_NE(BGR565BE, BGR565LE)
#define AV_PIX_FMT_BGR555 AV_PIX_FMT_NE(BGR555BE, BGR555LE)
#define AV_PIX_FMT_BGR444 AV_PIX_FMT_NE(BGR444BE, BGR444LE)
#define AV_PIX_FMT_BGRA64 AV_PIX_FMT_NE(BGRA64BE, BGRA64LE)

#define AV_PIX_FMT_YUV420P9  AV_PIX_FMT_NE(YUV420P9BE , YUV420P9LE)
#define AV_PIX_FMT_YUV422P9  AV_PIX_FMT_NE(YUV422P9BE , YUV422P9LE)
#define AV_PIX_FMT_YUV444P9  AV_PIX_FMT_NE(YUV444P9BE , YUV444P9LE)
#define AV_PIX_FMT_YUV420P10 AV_PIX_FMT_NE(YUV420P10BE, YUV420P10LE)
#define AV_PIX_FMT_YUV422P10 AV_PIX_FMT_NE(YUV422P10BE, YUV422P10LE)
#define AV_PIX_FMT_YUV440P10 AV_PIX_FMT_NE(YUV440P10BE, YUV440P10LE)
#define AV_PIX_FMT_YUV444P10 AV_PIX_FMT_NE(YUV444P10BE, YUV444P10LE)
#define AV_PIX_FMT_YUV420P12 AV_PIX_FMT_NE(YUV420P12BE, YUV420P12LE)
#define AV_PIX_FMT_YUV422P12 AV_PIX_FMT_NE(YUV422P12BE, YUV422P12LE)
#define AV_PIX_FMT_YUV440P12 AV_PIX_FMT_NE(YUV440P12BE, YUV440P12LE)
#define AV_PIX_FMT_YUV444P12 AV_PIX_FMT_NE(YUV444P12BE, YUV444P12LE)
#define AV_PIX_FMT_YUV420P14 AV_PIX_FMT_NE(YUV420P14BE, YUV420P14LE)
#define AV_PIX_FMT_YUV422P14 AV_PIX_FMT_NE(YUV422P14BE, YUV422P14LE)
#define AV_PIX_FMT_YUV444P14 AV_PIX_FMT_NE(YUV444P14BE, YUV444P14LE)
#define AV_PIX_FMT_YUV420P16 AV_PIX_FMT_NE(YUV420P16BE, YUV420P16LE)
#define AV_PIX_FMT_YUV422P16 AV_PIX_FMT_NE(YUV422P16BE, YUV422P16LE)
#define AV_PIX_FMT_YUV444P16 AV_PIX_FMT_NE(YUV444P16BE, YUV444P16LE)

#define AV_PIX_FMT_GBRP9     AV_PIX_FMT_NE(GBRP9BE ,    GBRP9LE)
#define AV_PIX_FMT_GBRP10    AV_PIX_FMT_NE(GBRP10BE,    GBRP10LE)
#define AV_PIX_FMT_GBRP12    AV_PIX_FMT_NE(GBRP12BE,    GBRP12LE)
#define AV_PIX_FMT_GBRP14    AV_PIX_FMT_NE(GBRP14BE,    GBRP14LE)
#define AV_PIX_FMT_GBRP16    AV_PIX_FMT_NE(GBRP16BE,    GBRP16LE)
#define AV_PIX_FMT_GBRAP10   AV_PIX_FMT_NE(GBRAP10BE,   GBRAP10LE)
#define AV_PIX_FMT_GBRAP12   AV_PIX_FMT_NE(GBRAP12BE,   GBRAP12LE)
#define AV_PIX_FMT_GBRAP16   AV_PIX_FMT_NE(GBRAP16BE,   GBRAP16LE)

#define AV_PIX_FMT_BAYER_BGGR16 AV_PIX_FMT_NE(BAYER_BGGR16BE,    BAYER_BGGR16LE)
#define AV_PIX_FMT_BAYER_RGGB16 AV_PIX_FMT_NE(BAYER_RGGB16BE,    BAYER_RGGB16LE)
#define AV_PIX_FMT_BAYER_GBRG16 AV_PIX_FMT_NE(BAYER_GBRG16BE,    BAYER_GBRG16LE)
#define AV_PIX_FMT_BAYER_GRBG16 AV_PIX_FMT_NE(BAYER_GRBG16BE,    BAYER_GRBG16LE)

#define AV_PIX_FMT_GBRPF32    AV_PIX_FMT_NE(GBRPF32BE,  GBRPF32LE)
#define AV_PIX_FMT_GBRAPF32   AV_PIX_FMT_NE(GBRAPF32BE, GBRAPF32LE)

#define AV_PIX_FMT_GRAYF32    AV_PIX_FMT_NE(GRAYF32BE, GRAYF32LE)

#define AV_PIX_FMT_YUVA420P9  AV_PIX_FMT_NE(YUVA420P9BE , YUVA420P9LE)
#define AV_PIX_FMT_YUVA422P9  AV_PIX_FMT_NE(YUVA422P9BE , YUVA422P9LE)
#define AV_PIX_FMT_YUVA444P9  AV_PIX_FMT_NE(YUVA444P9BE , YUVA444P9LE)
#define AV_PIX_FMT_YUVA420P10 AV_PIX_FMT_NE(YUVA420P10BE, YUVA420P10LE)
#define AV_PIX_FMT_YUVA422P10 AV_PIX_FMT_NE(YUVA422P10BE, YUVA422P10LE)
#define AV_PIX_FMT_YUVA444P10 AV_PIX_FMT_NE(YUVA444P10BE, YUVA444P10LE)
#define AV_PIX_FMT_YUVA422P12 AV_PIX_FMT_NE(YUVA422P12BE, YUVA422P12LE)
#define AV_PIX_FMT_YUVA444P12 AV_PIX_FMT_NE(YUVA444P12BE, YUVA444P12LE)
#define AV_PIX_FMT_YUVA420P16 AV_PIX_FMT_NE(YUVA420P16BE, YUVA420P16LE)
#define AV_PIX_FMT_YUVA422P16 AV_PIX_FMT_NE(YUVA422P16BE, YUVA422P16LE)
#define AV_PIX_FMT_YUVA444P16 AV_PIX_FMT_NE(YUVA444P16BE, YUVA444P16LE)

#define AV_PIX_FMT_XYZ12      AV_PIX_FMT_NE(XYZ12BE, XYZ12LE)
#define AV_PIX_FMT_NV20       AV_PIX_FMT_NE(NV20BE,  NV20LE)
#define AV_PIX_FMT_AYUV64     AV_PIX_FMT_NE(AYUV64BE, AYUV64LE)
#define AV_PIX_FMT_P010       AV_PIX_FMT_NE(P010BE,  P010LE)
#define AV_PIX_FMT_P016       AV_PIX_FMT_NE(P016BE,  P016LE)

/**
  * Chromaticity coordinates of the source primaries.
  * These values match the ones defined by ISO/IEC 23001-8_2013 § 7.1.
  */
enum AVColorPrimaries {
    AVCOL_PRI_RESERVED0 = 0,
    AVCOL_PRI_BT709 = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP177 Annex B
    AVCOL_PRI_UNSPECIFIED = 2,
    AVCOL_PRI_RESERVED = 3,
    AVCOL_PRI_BT470M = 4,  ///< also FCC Title 47 Code of Federal Regulations 73.682 (a)(20)

    AVCOL_PRI_BT470BG = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM
    AVCOL_PRI_SMPTE170M = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    AVCOL_PRI_SMPTE240M = 7,  ///< functionally identical to above
    AVCOL_PRI_FILM = 8,  ///< colour filters using Illuminant C
    AVCOL_PRI_BT2020 = 9,  ///< ITU-R BT2020
    AVCOL_PRI_SMPTE428 = 10, ///< SMPTE ST 428-1 (CIE 1931 XYZ)
    AVCOL_PRI_SMPTEST428_1 = AVCOL_PRI_SMPTE428,
    AVCOL_PRI_SMPTE431 = 11, ///< SMPTE ST 431-2 (2011) / DCI P3
    AVCOL_PRI_SMPTE432 = 12, ///< SMPTE ST 432-1 (2010) / P3 D65 / Display P3
    AVCOL_PRI_EBU3213 = 22, ///< EBU Tech. 3213-E / JEDEC P22 phosphors
    AVCOL_PRI_JEDEC_P22 = AVCOL_PRI_EBU3213,
    AVCOL_PRI_NB                ///< Not part of ABI
};

/**
 * Color Transfer Characteristic.
 * These values match the ones defined by ISO/IEC 23001-8_2013 § 7.2.
 */
enum AVColorTransferCharacteristic {
    AVCOL_TRC_RESERVED0 = 0,
    AVCOL_TRC_BT709 = 1,  ///< also ITU-R BT1361
    AVCOL_TRC_UNSPECIFIED = 2,
    AVCOL_TRC_RESERVED = 3,
    AVCOL_TRC_GAMMA22 = 4,  ///< also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM
    AVCOL_TRC_GAMMA28 = 5,  ///< also ITU-R BT470BG
    AVCOL_TRC_SMPTE170M = 6,  ///< also ITU-R BT601-6 525 or 625 / ITU-R BT1358 525 or 625 / ITU-R BT1700 NTSC
    AVCOL_TRC_SMPTE240M = 7,
    AVCOL_TRC_LINEAR = 8,  ///< "Linear transfer characteristics"
    AVCOL_TRC_LOG = 9,  ///< "Logarithmic transfer characteristic (100:1 range)"
    AVCOL_TRC_LOG_SQRT = 10, ///< "Logarithmic transfer characteristic (100 * Sqrt(10) : 1 range)"
    AVCOL_TRC_IEC61966_2_4 = 11, ///< IEC 61966-2-4
    AVCOL_TRC_BT1361_ECG = 12, ///< ITU-R BT1361 Extended Colour Gamut
    AVCOL_TRC_IEC61966_2_1 = 13, ///< IEC 61966-2-1 (sRGB or sYCC)
    AVCOL_TRC_BT2020_10 = 14, ///< ITU-R BT2020 for 10-bit system
    AVCOL_TRC_BT2020_12 = 15, ///< ITU-R BT2020 for 12-bit system
    AVCOL_TRC_SMPTE2084 = 16, ///< SMPTE ST 2084 for 10-, 12-, 14- and 16-bit systems
    AVCOL_TRC_SMPTEST2084 = AVCOL_TRC_SMPTE2084,
    AVCOL_TRC_SMPTE428 = 17, ///< SMPTE ST 428-1
    AVCOL_TRC_SMPTEST428_1 = AVCOL_TRC_SMPTE428,
    AVCOL_TRC_ARIB_STD_B67 = 18, ///< ARIB STD-B67, known as "Hybrid log-gamma"
    AVCOL_TRC_NB                 ///< Not part of ABI
};

/**
 * YUV colorspace type.
 * These values match the ones defined by ISO/IEC 23001-8_2013 § 7.3.
 */
enum AVColorSpace {
    AVCOL_SPC_RGB = 0,  ///< order of coefficients is actually GBR, also IEC 61966-2-1 (sRGB)
    AVCOL_SPC_BT709 = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177 Annex B
    AVCOL_SPC_UNSPECIFIED = 2,
    AVCOL_SPC_RESERVED = 3,
    AVCOL_SPC_FCC = 4,  ///< FCC Title 47 Code of Federal Regulations 73.682 (a)(20)
    AVCOL_SPC_BT470BG = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
    AVCOL_SPC_SMPTE170M = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
    AVCOL_SPC_SMPTE240M = 7,  ///< functionally identical to above
    AVCOL_SPC_YCGCO = 8,  ///< Used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
    AVCOL_SPC_YCOCG = AVCOL_SPC_YCGCO,
    AVCOL_SPC_BT2020_NCL = 9,  ///< ITU-R BT2020 non-constant luminance system
    AVCOL_SPC_BT2020_CL = 10, ///< ITU-R BT2020 constant luminance system
    AVCOL_SPC_SMPTE2085 = 11, ///< SMPTE 2085, Y'D'zD'x
    AVCOL_SPC_CHROMA_DERIVED_NCL = 12, ///< Chromaticity-derived non-constant luminance system
    AVCOL_SPC_CHROMA_DERIVED_CL = 13, ///< Chromaticity-derived constant luminance system
    AVCOL_SPC_ICTCP = 14, ///< ITU-R BT.2100-0, ICtCp
    AVCOL_SPC_NB                ///< Not part of ABI
};

/**
 * MPEG vs JPEG YUV range.
 */
enum AVColorRange {
    AVCOL_RANGE_UNSPECIFIED = 0, AVCOL_RANGE_MPEG = 1, ///< the normal 219*2^(n-8) "MPEG" YUV ranges
    AVCOL_RANGE_JPEG = 2, ///< the normal     2^n-1   "JPEG" YUV ranges
    AVCOL_RANGE_NB               ///< Not part of ABI
};

/**
 * Location of chroma samples.
 *
 * Illustration showing the location of the first (top left) chroma sample of the
 * image, the left shows only luma, the right
 * shows the location of the chroma sample, the 2 could be imagined to overlay
 * each other but are drawn separately due to limitations of ASCII
 *
 *                1st 2nd       1st 2nd horizontal luma sample positions
 *                 v   v         v   v
 *                 ______        ______
 *1st luma line > |X   X ...    |3 4 X ...     X are luma samples,
 *                |             |1 2           1-6 are possible chroma positions
 *2nd luma line > |X   X ...    |5 6 X ...     0 is undefined/unknown position
 */
enum AVChromaLocation {
    AVCHROMA_LOC_UNSPECIFIED = 0,
    AVCHROMA_LOC_LEFT = 1, ///< MPEG-2/4 4:2:0, H.264 default for 4:2:0
    AVCHROMA_LOC_CENTER = 2, ///< MPEG-1 4:2:0, JPEG 4:2:0, H.263 4:2:0
    AVCHROMA_LOC_TOPLEFT = 3, ///< ITU-R 601, SMPTE 274M 296M S314M(DV 4:1:1), mpeg2 4:2:2
    AVCHROMA_LOC_TOP = 4,
    AVCHROMA_LOC_BOTTOMLEFT = 5,
    AVCHROMA_LOC_BOTTOM = 6,
    AVCHROMA_LOC_NB               ///< Not part of ABI
};


enum {
// 7.4.3.1: vps_max_layers_minus1 is in [0, 62].
    HEVC_MAX_LAYERS = 63, // 7.4.3.1: vps_max_sub_layers_minus1 is in [0, 6].
    HEVC_MAX_SUB_LAYERS = 7, // 7.4.3.1: vps_num_layer_sets_minus1 is in [0, 1023].
    HEVC_MAX_LAYER_SETS = 1024,

// 7.4.2.1: vps_video_parameter_set_id is u(4).
    HEVC_MAX_VPS_COUNT = 16, // 7.4.3.2.1: sps_seq_parameter_set_id is in [0, 15].
    HEVC_MAX_SPS_COUNT = 16, // 7.4.3.3.1: pps_pic_parameter_set_id is in [0, 63].
    HEVC_MAX_PPS_COUNT = 64,

// A.4.2: MaxDpbSize is bounded above by 16.
    HEVC_MAX_DPB_SIZE = 16, // 7.4.3.1: vps_max_dec_pic_buffering_minus1[i] is in [0, MaxDpbSize - 1].
    HEVC_MAX_REFS = HEVC_MAX_DPB_SIZE,

// 7.4.3.2.1: num_short_term_ref_pic_sets is in [0, 64].
    HEVC_MAX_SHORT_TERM_REF_PIC_SETS = 64, // 7.4.3.2.1: num_long_term_ref_pics_sps is in [0, 32].
    HEVC_MAX_LONG_TERM_REF_PICS = 32,

// A.3: all profiles require that CtbLog2SizeY is in [4, 6].
    HEVC_MIN_LOG2_CTB_SIZE = 4, HEVC_MAX_LOG2_CTB_SIZE = 6,

// E.3.2: cpb_cnt_minus1[i] is in [0, 31].
    HEVC_MAX_CPB_CNT = 32,

// A.4.1: in table A.6 the highest level allows a MaxLumaPs of 35 651 584.
    HEVC_MAX_LUMA_PS = 35651584, // A.4.1: pic_width_in_luma_samples and pic_height_in_luma_samples are
// constrained to be not greater than sqrt(MaxLumaPs * 8).  Hence height/
// width are bounded above by sqrt(8 * 35651584) = 16888.2 samples.
    HEVC_MAX_WIDTH = 16888, HEVC_MAX_HEIGHT = 16888,

// A.4.1: table A.6 allows at most 22 tile rows for any level.
    HEVC_MAX_TILE_ROWS = 22, // A.4.1: table A.6 allows at most 20 tile columns for any level.
    HEVC_MAX_TILE_COLUMNS = 20,

// A.4.2: table A.6 allows at most 600 slice segments for any level.
    HEVC_MAX_SLICE_SEGMENTS = 600,

// 7.4.7.1: in the worst case (tiles_enabled_flag and
// entropy_coding_sync_enabled_flag are both set), entry points can be
// placed at the beginning of every Ctb row in every tile, giving an
// upper bound of (num_tile_columns_minus1 + 1) * PicHeightInCtbsY - 1.
// Only a stream with very high resolution and perverse parameters could
// get near that, though, so set a lower limit here with the maximum
// possible value for 4K video (at most 135 16x16 Ctb rows).
    HEVC_MAX_ENTRY_POINT_OFFSETS = HEVC_MAX_TILE_COLUMNS * 135,
};


/**
 * SEI message types
 */
typedef enum {
    HEVC_SEI_TYPE_BUFFERING_PERIOD = 0,
    HEVC_SEI_TYPE_PICTURE_TIMING = 1,
    HEVC_SEI_TYPE_PAN_SCAN_RECT = 2,
    HEVC_SEI_TYPE_FILLER_PAYLOAD = 3,
    HEVC_SEI_TYPE_USER_DATA_REGISTERED_ITU_T_T35 = 4,
    HEVC_SEI_TYPE_USER_DATA_UNREGISTERED = 5,
    HEVC_SEI_TYPE_RECOVERY_POINT = 6,
    HEVC_SEI_TYPE_SCENE_INFO = 9,
    HEVC_SEI_TYPE_FULL_FRAME_SNAPSHOT = 15,
    HEVC_SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_START = 16,
    HEVC_SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_END = 17,
    HEVC_SEI_TYPE_FILM_GRAIN_CHARACTERISTICS = 19,
    HEVC_SEI_TYPE_POST_FILTER_HINT = 22,
    HEVC_SEI_TYPE_TONE_MAPPING_INFO = 23,
    HEVC_SEI_TYPE_FRAME_PACKING = 45,
    HEVC_SEI_TYPE_DISPLAY_ORIENTATION = 47,
    HEVC_SEI_TYPE_SOP_DESCRIPTION = 128,
    HEVC_SEI_TYPE_ACTIVE_PARAMETER_SETS = 129,
    HEVC_SEI_TYPE_DECODING_UNIT_INFO = 130,
    HEVC_SEI_TYPE_TEMPORAL_LEVEL0_INDEX = 131,
    HEVC_SEI_TYPE_DECODED_PICTURE_HASH = 132,
    HEVC_SEI_TYPE_SCALABLE_NESTING = 133,
    HEVC_SEI_TYPE_REGION_REFRESH_INFO = 134,
    HEVC_SEI_TYPE_TIME_CODE = 136,
    HEVC_SEI_TYPE_MASTERING_DISPLAY_INFO = 137,
    HEVC_SEI_TYPE_CONTENT_LIGHT_LEVEL_INFO = 144,
    HEVC_SEI_TYPE_ALTERNATIVE_TRANSFER_CHARACTERISTICS = 147,
    HEVC_SEI_TYPE_ALPHA_CHANNEL_INFO = 165,
} HEVC_SEI_Type;

typedef struct HEVCSEIPictureHash {
    uint8_t md5[3][16];
    uint8_t is_md5;
} HEVCSEIPictureHash;

typedef struct HEVCSEIFramePacking {
    int present;
    int arrangement_type;
    int content_interpretation_type;
    int quincunx_subsampling;
    int current_frame_is_frame0_flag;
} HEVCSEIFramePacking;

typedef struct HEVCSEIDisplayOrientation {
    int present;
    int anticlockwise_rotation;
    int hflip, vflip;
} HEVCSEIDisplayOrientation;

typedef struct HEVCSEIPictureTiming {
    int picture_struct;
} HEVCSEIPictureTiming;

typedef struct HEVCSEIA53Caption {
    int a53_caption_size;
    uint8_t *a53_caption;
} HEVCSEIA53Caption;

typedef struct HEVCSEIMasteringDisplay {
    int present;
    uint16_t display_primaries[3][2];
    uint16_t white_point[2];
    uint32_t max_luminance;
    uint32_t min_luminance;
} HEVCSEIMasteringDisplay;

typedef struct HEVCSEIContentLight {
    int present;
    uint16_t max_content_light_level;
    uint16_t max_pic_average_light_level;
} HEVCSEIContentLight;

typedef struct HEVCSEIAlternativeTransfer {
    int present;
    int preferred_transfer_characteristics;
} HEVCSEIAlternativeTransfer;

typedef struct HEVCSEI {
    HEVCSEIPictureHash picture_hash;
    HEVCSEIFramePacking frame_packing;
    HEVCSEIDisplayOrientation display_orientation;
    HEVCSEIPictureTiming picture_timing;
    HEVCSEIA53Caption a53_caption;
    HEVCSEIMasteringDisplay mastering_display;
    HEVCSEIContentLight content_light;
    int active_seq_parameter_set_id;
    HEVCSEIAlternativeTransfer alternative_transfer;
} HEVCSEI;

struct HEVCParamSets;


typedef struct RcOverride {
    int start_frame;
    int end_frame;
    int qscale; // If this is 0 then quality_factor will be used instead.
    float quality_factor;
} RcOverride;


enum AVPacketSideDataType {
/**
     * An AV_PKT_DATA_PALETTE side data packet contains exactly AVPALETTE_SIZE
     * bytes worth of palette. This side data signals that a new palette is
     * present.
     */
    AV_PKT_DATA_PALETTE,

/**
     * The AV_PKT_DATA_NEW_EXTRADATA is used to notify the codec or the format
     * that the extradata buffer was changed and the receiving side should
     * act upon it appropriately. The new extradata is embedded in the side
     * data buffer and should be immediately used for processing the current
     * frame or packet.
     */
    AV_PKT_DATA_NEW_EXTRADATA,

/**
     * An AV_PKT_DATA_PARAM_CHANGE side data packet is laid out as follows:
     * @code
     * u32le param_flags
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_COUNT)
     *     s32le channel_count
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_LAYOUT)
     *     u64le channel_layout
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_SAMPLE_RATE)
     *     s32le sample_rate
     * if (param_flags & AV_SIDE_DATA_PARAM_CHANGE_DIMENSIONS)
     *     s32le width
     *     s32le height
     * @endcode
     */
    AV_PKT_DATA_PARAM_CHANGE,

/**
     * An AV_PKT_DATA_H263_MB_INFO side data packet contains a number of
     * structures with info about macroblocks relevant to splitting the
     * packet into smaller packets on macroblock edges (e.g. as for RFC 2190).
     * That is, it does not necessarily contain info about all macroblocks,
     * as long as the distance between macroblocks in the info is smaller
     * than the target payload size.
     * Each MB info structure is 12 bytes, and is laid out as follows:
     * @code
     * u32le bit offset from the start of the packet
     * u8    current quantizer at the start of the macroblock
     * u8    GOB number
     * u16le macroblock address within the GOB
     * u8    horizontal MV predictor
     * u8    vertical MV predictor
     * u8    horizontal MV predictor for block number 3
     * u8    vertical MV predictor for block number 3
     * @endcode
     */
    AV_PKT_DATA_H263_MB_INFO,

/**
     * This side data should be associated with an audio stream and contains
     * ReplayGain information in form of the AVReplayGain struct.
     */
    AV_PKT_DATA_REPLAYGAIN,

/**
     * This side data contains a 3x3 transformation matrix describing an affine
     * transformation that needs to be applied to the decoded video frames for
     * correct presentation.
     *
     * See libavutil/display.h for a detailed description of the data.
     */
    AV_PKT_DATA_DISPLAYMATRIX,

/**
     * This side data should be associated with a video stream and contains
     * Stereoscopic 3D information in form of the AVStereo3D struct.
     */
    AV_PKT_DATA_STEREO3D,

/**
     * This side data should be associated with an audio stream and corresponds
     * to enum AVAudioServiceType.
     */
    AV_PKT_DATA_AUDIO_SERVICE_TYPE,

/**
     * This side data contains quality related information from the encoder.
     * @code
     * u32le quality factor of the compressed frame. Allowed range is between 1 (good) and FF_LAMBDA_MAX (bad).
     * u8    picture type
     * u8    error count
     * u16   reserved
     * u64le[error count] sum of squared differences between encoder in and output
     * @endcode
     */
    AV_PKT_DATA_QUALITY_STATS,

/**
     * This side data contains an integer value representing the stream index
     * of a "fallback" track.  A fallback track indicates an alternate
     * track to use when the current track can not be decoded for some reason.
     * e.g. no decoder available for codec.
     */
    AV_PKT_DATA_FALLBACK_TRACK,

/**
     * This side data corresponds to the AVCPBProperties struct.
     */
    AV_PKT_DATA_CPB_PROPERTIES,

/**
     * Recommmends skipping the specified number of samples
     * @code
     * u32le number of samples to skip from start of this packet
     * u32le number of samples to skip from end of this packet
     * u8    reason for start skip
     * u8    reason for end   skip (0=padding silence, 1=convergence)
     * @endcode
     */
    AV_PKT_DATA_SKIP_SAMPLES,

/**
     * An AV_PKT_DATA_JP_DUALMONO side data packet indicates that
     * the packet may contain "dual mono" audio specific to Japanese DTV
     * and if it is true, recommends only the selected channel to be used.
     * @code
     * u8    selected channels (0=mail/left, 1=sub/right, 2=both)
     * @endcode
     */
    AV_PKT_DATA_JP_DUALMONO,

/**
     * A list of zero terminated key/value strings. There is no end marker for
     * the list, so it is required to rely on the side data size to stop.
     */
    AV_PKT_DATA_STRINGS_METADATA,

/**
     * Subtitle event position
     * @code
     * u32le x1
     * u32le y1
     * u32le x2
     * u32le y2
     * @endcode
     */
    AV_PKT_DATA_SUBTITLE_POSITION,

/**
     * Data found in BlockAdditional element of matroska container. There is
     * no end marker for the data, so it is required to rely on the side data
     * size to recognize the end. 8 byte id (as found in BlockAddId) followed
     * by data.
     */
    AV_PKT_DATA_MATROSKA_BLOCKADDITIONAL,

/**
     * The optional first identifier line of a WebVTT cue.
     */
    AV_PKT_DATA_WEBVTT_IDENTIFIER,

/**
     * The optional settings (rendering instructions) that immediately
     * follow the timestamp specifier of a WebVTT cue.
     */
    AV_PKT_DATA_WEBVTT_SETTINGS,

/**
     * A list of zero terminated key/value strings. There is no end marker for
     * the list, so it is required to rely on the side data size to stop. This
     * side data includes updated metadata which appeared in the stream.
     */
    AV_PKT_DATA_METADATA_UPDATE,

/**
     * MPEGTS stream ID as uint8_t, this is required to pass the stream ID
     * information from the demuxer to the corresponding muxer.
     */
    AV_PKT_DATA_MPEGTS_STREAM_ID,

/**
     * Mastering display metadata (based on SMPTE-2086:2014). This metadata
     * should be associated with a video stream and contains data in the form
     * of the AVMasteringDisplayMetadata struct.
     */
    AV_PKT_DATA_MASTERING_DISPLAY_METADATA,

/**
     * This side data should be associated with a video stream and corresponds
     * to the AVSphericalMapping structure.
     */
    AV_PKT_DATA_SPHERICAL,

/**
     * Content light level (based on CTA-861.3). This metadata should be
     * associated with a video stream and contains data in the form of the
     * AVContentLightMetadata struct.
     */
    AV_PKT_DATA_CONTENT_LIGHT_LEVEL,

/**
     * ATSC A53 Part 4 Closed Captions. This metadata should be associated with
     * a video stream. A53 CC bitstream is stored as uint8_t in AVPacketSideData.data.
     * The number of bytes of CC data is AVPacketSideData.size.
     */
    AV_PKT_DATA_A53_CC,

/**
     * This side data is encryption initialization data.
     * The format is not part of ABI, use av_encryption_init_info_* methods to
     * access.
     */
    AV_PKT_DATA_ENCRYPTION_INIT_INFO,

/**
     * This side data contains encryption info for how to decrypt the packet.
     * The format is not part of ABI, use av_encryption_info_* methods to access.
     */
    AV_PKT_DATA_ENCRYPTION_INFO,

/**
     * Active Format Description data consisting of a single byte as specified
     * in ETSI TS 101 154 using AVActiveFormatDescription enum.
     */
    AV_PKT_DATA_AFD,

/**
     * The number of side data types.
     * This is not part of the public API/ABI in the sense that it may
     * change when new side data types are added.
     * This must stay the last enum value.
     * If its value becomes huge, some code using it
     * needs to be updated as it assumes it to be smaller than other limits.
     */
    AV_PKT_DATA_NB
};


typedef struct AVPacketSideData {
    uint8_t *data;
    int size;
    enum AVPacketSideDataType type;
} AVPacketSideData;


typedef struct AVPacket {
/**
     * A reference to the reference-counted buffer where the packet data is
     * stored.
     * May be NULL, then the packet data is not reference-counted.
     */
    AVBufferRef *buf;
/**
     * Presentation timestamp in AVStream->time_base units; the time at which
     * the decompressed packet will be presented to the user.
     * Can be AV_NOPTS_VALUE if it is not stored in the file.
     * pts MUST be larger or equal to dts as presentation cannot happen before
     * decompression, unless one wants to view hex dumps. Some formats misuse
     * the terms dts and pts/cts to mean something different. Such timestamps
     * must be converted to true pts/dts before they are stored in AVPacket.
     */
    int64_t pts;
/**
     * Decompression timestamp in AVStream->time_base units; the time at which
     * the packet is decompressed.
     * Can be AV_NOPTS_VALUE if it is not stored in the file.
     */
    int64_t dts;
    uint8_t *data;
    int size;
    int stream_index;
/**
     * A combination of AV_PKT_FLAG values
     */
    int flags;
/**
     * Additional packet data that can be provided by the container.
     * Packet can contain several types of side information.
     */
    AVPacketSideData *side_data;
    int side_data_elems;

/**
     * Duration of this packet in AVStream->time_base units, 0 if unknown.
     * Equals next_pts - this_pts in presentation order.
     */
    int64_t duration;

    int64_t pos;                            ///< byte position in stream, -1 if unknown

#if FF_API_CONVERGENCE_DURATION
                                                                                                                            /**
     * @deprecated Same as the duration field, but as int64_t. This was required
     * for Matroska subtitles, whose duration values could overflow when the
     * duration field was still an int.
     */
attribute_deprecated
int64_t convergence_duration;
#endif
} AVPacket;

typedef struct AVCodecDescriptor {
    enum AVCodecID id;
    enum AVMediaType type;
/**
     * Name of the codec described by this descriptor. It is non-empty and
     * unique for each codec descriptor. It should contain alphanumeric
     * characters and '_' only.
     */
    const char *name;
/**
     * A more descriptive name for this codec. May be NULL.
     */
    const char *long_name;
/**
     * Codec properties, a combination of AV_CODEC_PROP_* flags.
     */
    int props;
/**
     * MIME type(s) associated with the codec.
     * May be NULL; if not, a NULL-terminated array of MIME types.
     * The first item is always non-NULL and is the preferred MIME type.
     */
    const char *const *mime_types;
/**
     * If non-NULL, an array of profiles recognized for this codec.
     * Terminated with FF_PROFILE_UNKNOWN.
     */
    const struct AVProfile *profiles;
} AVCodecDescriptor;


enum HEVCNALUnitType {
    HEVC_NAL_TRAIL_N = 0,
    HEVC_NAL_TRAIL_R = 1,
    HEVC_NAL_TSA_N = 2,
    HEVC_NAL_TSA_R = 3,
    HEVC_NAL_STSA_N = 4,
    HEVC_NAL_STSA_R = 5,
    HEVC_NAL_RADL_N = 6,
    HEVC_NAL_RADL_R = 7,
    HEVC_NAL_RASL_N = 8,
    HEVC_NAL_RASL_R = 9,
    HEVC_NAL_VCL_N10 = 10,
    HEVC_NAL_VCL_R11 = 11,
    HEVC_NAL_VCL_N12 = 12,
    HEVC_NAL_VCL_R13 = 13,
    HEVC_NAL_VCL_N14 = 14,
    HEVC_NAL_VCL_R15 = 15,
    HEVC_NAL_BLA_W_LP = 16,
    HEVC_NAL_BLA_W_RADL = 17,
    HEVC_NAL_BLA_N_LP = 18,
    HEVC_NAL_IDR_W_RADL = 19,
    HEVC_NAL_IDR_N_LP = 20,
    HEVC_NAL_CRA_NUT = 21,
    HEVC_NAL_IRAP_VCL22 = 22,
    HEVC_NAL_IRAP_VCL23 = 23,
    HEVC_NAL_RSV_VCL24 = 24,
    HEVC_NAL_RSV_VCL25 = 25,
    HEVC_NAL_RSV_VCL26 = 26,
    HEVC_NAL_RSV_VCL27 = 27,
    HEVC_NAL_RSV_VCL28 = 28,
    HEVC_NAL_RSV_VCL29 = 29,
    HEVC_NAL_RSV_VCL30 = 30,
    HEVC_NAL_RSV_VCL31 = 31,
    HEVC_NAL_VPS = 32,
    HEVC_NAL_SPS = 33,
    HEVC_NAL_PPS = 34,
    HEVC_NAL_AUD = 35,
    HEVC_NAL_EOS_NUT = 36,
    HEVC_NAL_EOB_NUT = 37,
    HEVC_NAL_FD_NUT = 38,
    HEVC_NAL_SEI_PREFIX = 39,
    HEVC_NAL_SEI_SUFFIX = 40,
    HEVC_NAL_RSV_NVCL41 = 41,
    HEVC_NAL_RSV_NVCL42 = 42,
    HEVC_NAL_RSV_NVCL43 = 43,
    HEVC_NAL_RSV_NVCL44 = 44,
    HEVC_NAL_RSV_NVCL45 = 45,
    HEVC_NAL_RSV_NVCL46 = 46,
    HEVC_NAL_RSV_NVCL47 = 47,
    HEVC_NAL_UNSPEC48 = 48,
    HEVC_NAL_UNSPEC49 = 49,
    HEVC_NAL_UNSPEC50 = 50,
    HEVC_NAL_UNSPEC51 = 51,
    HEVC_NAL_UNSPEC52 = 52,
    HEVC_NAL_UNSPEC53 = 53,
    HEVC_NAL_UNSPEC54 = 54,
    HEVC_NAL_UNSPEC55 = 55,
    HEVC_NAL_UNSPEC56 = 56,
    HEVC_NAL_UNSPEC57 = 57,
    HEVC_NAL_UNSPEC58 = 58,
    HEVC_NAL_UNSPEC59 = 59,
    HEVC_NAL_UNSPEC60 = 60,
    HEVC_NAL_UNSPEC61 = 61,
    HEVC_NAL_UNSPEC62 = 62,
    HEVC_NAL_UNSPEC63 = 63,
};

enum HEVCSliceType {
    HEVC_SLICE_B = 0, HEVC_SLICE_P = 1, HEVC_SLICE_I = 2,
};


typedef struct AVRational {
    int num; ///< Numerator
    int den; ///< Denominator
} AVRational;


typedef struct ShortTermRPS {
    unsigned int num_negative_pics;
    int num_delta_pocs;
    int rps_idx_num_delta_pocs;
    int32_t delta_poc[32];
    uint8_t used[32];
} ShortTermRPS;

typedef struct LongTermRPS {
    int poc[32];
    uint8_t used[32];
    uint8_t nb_refs;
} LongTermRPS;

typedef struct SliceHeader {
    unsigned int pps_id;

///< address (in raster order) of the first block in the current slice segment
    unsigned int slice_segment_addr;
///< address (in raster order) of the first block in the current slice
    unsigned int slice_addr;

    enum HEVCSliceType slice_type;

    int pic_order_cnt_lsb;

    uint8_t first_slice_in_pic_flag;
    uint8_t dependent_slice_segment_flag;
    uint8_t pic_output_flag;
    uint8_t colour_plane_id;

///< RPS coded in the slice header itself is stored here
    int short_term_ref_pic_set_sps_flag;
    int short_term_ref_pic_set_size;
    ShortTermRPS slice_rps;
    const ShortTermRPS *short_term_rps;
    int long_term_ref_pic_set_size;
    LongTermRPS long_term_rps;
    unsigned int list_entry_lx[2][32];

    uint8_t rpl_modification_flag[2];
    uint8_t no_output_of_prior_pics_flag;
    uint8_t slice_temporal_mvp_enabled_flag;

    unsigned int nb_refs[2];

    uint8_t slice_sample_adaptive_offset_flag[3];
    uint8_t mvd_l1_zero_flag;

    uint8_t cabac_init_flag;
    uint8_t disable_deblocking_filter_flag; ///< slice_header_disable_deblocking_filter_flag
    uint8_t slice_loop_filter_across_slices_enabled_flag;
    uint8_t collocated_list;

    unsigned int collocated_ref_idx;

    int slice_qp_delta;
    int slice_cb_qp_offset;
    int slice_cr_qp_offset;

    uint8_t cu_chroma_qp_offset_enabled_flag;

    int beta_offset;    ///< beta_offset_div2 * 2
    int tc_offset;      ///< tc_offset_div2 * 2

    unsigned int max_num_merge_cand; ///< 5 - 5_minus_max_num_merge_cand

    unsigned *entry_point_offset;
    int *offset;
    int *size;
    int num_entry_point_offsets;

    int8_t slice_qp;

    uint8_t luma_log2_weight_denom;
    int16_t chroma_log2_weight_denom;

    int16_t luma_weight_l0[16];
    int16_t chroma_weight_l0[16][2];
    int16_t chroma_weight_l1[16][2];
    int16_t luma_weight_l1[16];

    int16_t luma_offset_l0[16];
    int16_t chroma_offset_l0[16][2];

    int16_t luma_offset_l1[16];
    int16_t chroma_offset_l1[16][2];

    int slice_ctb_addr_rs;
} SliceHeader;

typedef struct HEVCWindow {
    unsigned int left_offset;
    unsigned int right_offset;
    unsigned int top_offset;
    unsigned int bottom_offset;
} HEVCWindow;

typedef struct VUI {
    AVRational sar;

    int overscan_info_present_flag;
    int overscan_appropriate_flag;

    int video_signal_type_present_flag;
    int video_format;
    int video_full_range_flag;
    int colour_description_present_flag;
    uint8_t colour_primaries;
    uint8_t transfer_characteristic;
    uint8_t matrix_coeffs;

    int chroma_loc_info_present_flag;
    int chroma_sample_loc_type_top_field;
    int chroma_sample_loc_type_bottom_field;
    int neutra_chroma_indication_flag;

    int field_seq_flag;
    int frame_field_info_present_flag;

    int default_display_window_flag;
    HEVCWindow def_disp_win;

    int vui_timing_info_present_flag;
    uint32_t vui_num_units_in_tick;
    uint32_t vui_time_scale;
    int vui_poc_proportional_to_timing_flag;
    int vui_num_ticks_poc_diff_one_minus1;
    int vui_hrd_parameters_present_flag;

    int bitstream_restriction_flag;
    int tiles_fixed_structure_flag;
    int motion_vectors_over_pic_boundaries_flag;
    int restricted_ref_pic_lists_flag;
    int min_spatial_segmentation_idc;
    int max_bytes_per_pic_denom;
    int max_bits_per_min_cu_denom;
    int log2_max_mv_length_horizontal;
    int log2_max_mv_length_vertical;
} VUI;

typedef struct PTLCommon {
    uint8_t profile_space;
    uint8_t tier_flag;
    uint8_t profile_idc;
    uint8_t profile_compatibility_flag[32];
    uint8_t level_idc;
    uint8_t progressive_source_flag;
    uint8_t interlaced_source_flag;
    uint8_t non_packed_constraint_flag;
    uint8_t frame_only_constraint_flag;
} PTLCommon;

typedef struct PTL {
    PTLCommon general_ptl;
    PTLCommon sub_layer_ptl[HEVC_MAX_SUB_LAYERS];

    uint8_t sub_layer_profile_present_flag[HEVC_MAX_SUB_LAYERS];
    uint8_t sub_layer_level_present_flag[HEVC_MAX_SUB_LAYERS];
} PTL;

typedef struct HEVCVPS {
    uint8_t vps_temporal_id_nesting_flag;
    int vps_max_layers;
    int vps_max_sub_layers; ///< vps_max_temporal_layers_minus1 + 1

    PTL ptl;
    int vps_sub_layer_ordering_info_present_flag;
    unsigned int vps_max_dec_pic_buffering[HEVC_MAX_SUB_LAYERS];
    unsigned int vps_num_reorder_pics[HEVC_MAX_SUB_LAYERS];
    unsigned int vps_max_latency_increase[HEVC_MAX_SUB_LAYERS];
    int vps_max_layer_id;
    int vps_num_layer_sets; ///< vps_num_layer_sets_minus1 + 1
    uint8_t vps_timing_info_present_flag;
    uint32_t vps_num_units_in_tick;
    uint32_t vps_time_scale;
    uint8_t vps_poc_proportional_to_timing_flag;
    int vps_num_ticks_poc_diff_one; ///< vps_num_ticks_poc_diff_one_minus1 + 1
    int vps_num_hrd_parameters;

    uint8_t data[4096];
    int data_size;
} HEVCVPS;

typedef struct ScalingList {
/* This is a little wasteful, since sizeID 0 only needs 8 coeffs,
     * and size ID 3 only has 2 arrays, not 6. */
    uint8_t sl[4][6][64];
    uint8_t sl_dc[2][6];
} ScalingList;

typedef struct HEVCSPS {
    unsigned vps_id;
    int chroma_format_idc;
    uint8_t separate_colour_plane_flag;

    HEVCWindow output_window;

    HEVCWindow pic_conf_win;

    int bit_depth;
    int bit_depth_chroma;
    int pixel_shift;
    enum AVPixelFormat pix_fmt;

    unsigned int log2_max_poc_lsb;
    int pcm_enabled_flag;

    int max_sub_layers;
    struct {
        int max_dec_pic_buffering;
        int num_reorder_pics;
        int max_latency_increase;
    } temporal_layer[HEVC_MAX_SUB_LAYERS];
    uint8_t temporal_id_nesting_flag;

    VUI vui;
    PTL ptl;

    uint8_t scaling_list_enable_flag;
    ScalingList scaling_list;

    unsigned int nb_st_rps;
    ShortTermRPS st_rps[HEVC_MAX_SHORT_TERM_REF_PIC_SETS];

    uint8_t amp_enabled_flag;
    uint8_t sao_enabled;

    uint8_t long_term_ref_pics_present_flag;
    uint16_t lt_ref_pic_poc_lsb_sps[HEVC_MAX_LONG_TERM_REF_PICS];
    uint8_t used_by_curr_pic_lt_sps_flag[HEVC_MAX_LONG_TERM_REF_PICS];
    uint8_t num_long_term_ref_pics_sps;

    struct {
        uint8_t bit_depth;
        uint8_t bit_depth_chroma;
        unsigned int log2_min_pcm_cb_size;
        unsigned int log2_max_pcm_cb_size;
        uint8_t loop_filter_disable_flag;
    } pcm;
    uint8_t sps_temporal_mvp_enabled_flag;
    uint8_t sps_strong_intra_smoothing_enable_flag;

    unsigned int log2_min_cb_size;
    unsigned int log2_diff_max_min_coding_block_size;
    unsigned int log2_min_tb_size;
    unsigned int log2_max_trafo_size;
    unsigned int log2_ctb_size;
    unsigned int log2_min_pu_size;

    int max_transform_hierarchy_depth_inter;
    int max_transform_hierarchy_depth_intra;

    int sps_range_extension_flag;
    int transform_skip_rotation_enabled_flag;
    int transform_skip_context_enabled_flag;
    int implicit_rdpcm_enabled_flag;
    int explicit_rdpcm_enabled_flag;
    int extended_precision_processing_flag;
    int intra_smoothing_disabled_flag;
    int high_precision_offsets_enabled_flag;
    int persistent_rice_adaptation_enabled_flag;
    int cabac_bypass_alignment_enabled_flag;

///< coded frame dimension in various units
    int width;
    int height;
    int ctb_width;
    int ctb_height;
    int ctb_size;
    int min_cb_width;
    int min_cb_height;
    int min_tb_width;
    int min_tb_height;
    int min_pu_width;
    int min_pu_height;
    int tb_mask;

    int hshift[3];
    int vshift[3];

    int qp_bd_offset;

    uint8_t data[4096];
    int data_size;
} HEVCSPS;

typedef struct HEVCPPS {
    unsigned int sps_id; ///< seq_parameter_set_id

    uint8_t sign_data_hiding_flag;

    uint8_t cabac_init_present_flag;

    int num_ref_idx_l0_default_active; ///< num_ref_idx_l0_default_active_minus1 + 1
    int num_ref_idx_l1_default_active; ///< num_ref_idx_l1_default_active_minus1 + 1
    int pic_init_qp_minus26;

    uint8_t constrained_intra_pred_flag;
    uint8_t transform_skip_enabled_flag;

    uint8_t cu_qp_delta_enabled_flag;
    int diff_cu_qp_delta_depth;

    int cb_qp_offset;
    int cr_qp_offset;
    uint8_t pic_slice_level_chroma_qp_offsets_present_flag;
    uint8_t weighted_pred_flag;
    uint8_t weighted_bipred_flag;
    uint8_t output_flag_present_flag;
    uint8_t transquant_bypass_enable_flag;

    uint8_t dependent_slice_segments_enabled_flag;
    uint8_t tiles_enabled_flag;
    uint8_t entropy_coding_sync_enabled_flag;

    uint16_t num_tile_columns;   ///< num_tile_columns_minus1 + 1
    uint16_t num_tile_rows;      ///< num_tile_rows_minus1 + 1
    uint8_t uniform_spacing_flag;
    uint8_t loop_filter_across_tiles_enabled_flag;

    uint8_t seq_loop_filter_across_slices_enabled_flag;

    uint8_t deblocking_filter_control_present_flag;
    uint8_t deblocking_filter_override_enabled_flag;
    uint8_t disable_dbf;
    int beta_offset;    ///< beta_offset_div2 * 2
    int tc_offset;      ///< tc_offset_div2 * 2

    uint8_t scaling_list_data_present_flag;
    ScalingList scaling_list;

    uint8_t lists_modification_present_flag;
    int log2_parallel_merge_level; ///< log2_parallel_merge_level_minus2 + 2
    int num_extra_slice_header_bits;
    uint8_t slice_header_extension_present_flag;
    uint8_t log2_max_transform_skip_block_size;
    uint8_t pps_range_extensions_flag;
    uint8_t cross_component_prediction_enabled_flag;
    uint8_t chroma_qp_offset_list_enabled_flag;
    uint8_t diff_cu_chroma_qp_offset_depth;
    uint8_t chroma_qp_offset_list_len_minus1;
    int8_t cb_qp_offset_list[6];
    int8_t cr_qp_offset_list[6];
    uint8_t log2_sao_offset_scale_luma;
    uint8_t log2_sao_offset_scale_chroma;

// Inferred parameters
    unsigned int *column_width;  ///< ColumnWidth
    unsigned int *row_height;    ///< RowHeight
    unsigned int *col_bd;        ///< ColBd
    unsigned int *row_bd;        ///< RowBd
    int *col_idxX;

    int *ctb_addr_rs_to_ts; ///< CtbAddrRSToTS
    int *ctb_addr_ts_to_rs; ///< CtbAddrTSToRS
    int *tile_id;           ///< TileId
    int *tile_pos_rs;       ///< TilePosRS
    int *min_tb_addr_zs;    ///< MinTbAddrZS
    int *min_tb_addr_zs_tab;///< MinTbAddrZS

    uint8_t data[4096];
    int data_size;
} HEVCPPS;

typedef struct HEVCParamSets {
    AVBufferRef *vps_list[HEVC_MAX_VPS_COUNT];
    AVBufferRef *sps_list[HEVC_MAX_SPS_COUNT];
    AVBufferRef *pps_list[HEVC_MAX_PPS_COUNT];

/* currently active parameter sets */
    const HEVCVPS *vps;
    const HEVCSPS *sps;
    const HEVCPPS *pps;
} HEVCParamSets;


typedef struct H2645RBSP {
    uint8_t *rbsp_buffer;
    AVBufferRef *rbsp_buffer_ref;
    int rbsp_buffer_alloc_size;
    int rbsp_buffer_size;
} H2645RBSP;


//upstream ffmpeg headers

typedef struct ParseContext {
    uint8_t *buffer;
    int index;
    int last_index;
    unsigned int buffer_size;
    uint32_t state;             ///< contains the last few bytes in MSB order
    int frame_start_found;
    int overread;               ///< the number of bytes which where irreversibly read from the next frame
    int overread_index;         ///< the index into ParseContext.buffer of the overread bytes
    uint64_t state64;           ///< contains the last 8 bytes in MSB order
} ParseContext;


typedef struct H2645NAL {
    uint8_t *rbsp_buffer;

    int size;
    const uint8_t *data;

/**
     * Size, in bits, of just the data, excluding the stop bit and any trailing
     * padding. I.e. what HEVC calls SODB.
     */
    int size_bits;

    int raw_size;
    const uint8_t *raw_data;

    GetBitContext gb;

/**
     * NAL unit type
     */
    int type;

/**
     * HEVC only, nuh_temporal_id_plus_1 - 1
     */
    int temporal_id;

    int skipped_bytes;
    int skipped_bytes_pos_size;
    int *skipped_bytes_pos;
/**
     * H.264 only, nal_ref_idc
     */
    int ref_idc;
} H2645NAL;


/* an input packet split into unescaped NAL units */
typedef struct H2645Packet {
    H2645NAL *nals;
    H2645RBSP rbsp;
    int nb_nals;
    int nals_allocated;
} H2645Packet;


typedef struct AVCodecDefault AVCodecDefault;


///method decls
static int hevc_parse_nal_header(H2645NAL *nal, void *logctx);

static void alloc_rbsp_buffer(H2645RBSP *rbsp, unsigned int size, int use_ref);

static int find_next_start_code(const uint8_t *buf, const uint8_t *next_avc);

static int h264_parse_nal_header(H2645NAL *nal, void *logctx);

static inline int get_nalsize(int nal_length_size, const uint8_t *buf, int buf_size, int *buf_index, void *logctx);


int ff_h2645_extract_rbsp(const uint8_t *src, int length, H2645RBSP *rbsp, H2645NAL *nal, int small_padding);

int ff_hevc_decode_nal_sei(GetBitContext *gb, void *logctx, HEVCSEI *s, const HEVCParamSets *ps, int type);

int av_image_check_size2(unsigned int w, unsigned int h, int64_t max_pixels, enum AVPixelFormat pix_fmt, int log_offset, void *log_ctx);

int av_image_get_linesize(enum AVPixelFormat pix_fmt, int width, int plane);

/**
 * ----
 * SXS testing for csd-0
 *
 *
 * https://raw.githubusercontent.com/FFmpeg/FFmpeg/8ca55a2b9e95e79956ae0a9069f08e72c63fde16/libavcodec/hevc_parser.c
 *
 *
 */

/*
 * HEVC Annex B format parser
 *
 * Copyright (C) 2012 - 2013 Guillaume Martres
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#define START_CODE 0x000001 ///< start_code_prefix_one_3bytes

#define IS_IRAP_NAL(nal) (nal->type >= 16 && nal->type <= 23)
#define IS_IDR_NAL(nal) (nal->type == HEVC_NAL_IDR_W_RADL || nal->type == HEVC_NAL_IDR_N_LP)

typedef struct HEVCParserContext {
    ParseContext pc;

    H2645Packet pkt;
    HEVCParamSets ps;
    HEVCSEI sei;
    SliceHeader sh;

    int is_avc;
    int nal_length_size;
    int parsed_extradata;

    int poc;
    int pocTid0;
} HEVCParserContext;


#define MAX_MBPAIR_SIZE (256*1024) // a tighter bound could be calculated if someone cares about a few bytes


typedef struct AVClass {
/**
     * The name of the class; usually it is the same name as the
     * context structure type to which the AVClass is associated.
     */
    const char *class_name;

/**
     * A pointer to a function which returns the name of a context
     * instance ctx associated with the class.
     */
    const char *(*item_name)(void *ctx);

/**
     * a pointer to the first option specified in the class if any or NULL
     *
     * @see av_set_default_options()
     */
    const struct AVOption *option;

/**
     * LIBAVUTIL_VERSION with which this structure was created.
     * This is used to allow fields to be added without requiring major
     * version bumps everywhere.
     */

    int version;

/**
     * Offset in the structure where log_level_offset is stored.
     * 0 means there is no such variable
     */
    int log_level_offset_offset;

/**
     * Offset in the structure where a pointer to the parent context for
     * logging is stored. For example a decoder could pass its AVCodecContext
     * to eval as such a parent context, which an av_log() implementation
     * could then leverage to display the parent context.
     * The offset can be NULL.
     */
    int parent_log_context_offset;

/**
     * Return next AVOptions-enabled child or NULL
     */
    void *(*child_next)(void *obj, void *prev);

/**
     * Return an AVClass corresponding to the next potential
     * AVOptions-enabled child.
     *
     * The difference between child_next and this is that
     * child_next iterates over _already existing_ objects, while
     * child_class_next iterates over _all possible_ children.
     */
    const struct AVClass *(*child_class_next)(const struct AVClass *prev);

/**
     * Category used for visualization (like color)
     * This is only set if the category is equal for all objects using this class.
     * available since version (51 << 16 | 56 << 8 | 100)
     */
    AVClassCategory category;

/**
     * Callback to return the category.
     * available since version (51 << 16 | 59 << 8 | 100)
     */
    AVClassCategory (*get_category)(void *ctx);

/**
     * Callback to return the supported/allowed ranges.
     * available since version (52.12)
     */
    int (*query_ranges)(struct AVOptionRanges **, void *obj, const char *key, int flags);
} AVClass;


typedef struct AVProfile {
    int profile;
    const char *name; ///< short name for the profile
} AVProfile;


typedef struct AVDictionary AVDictionary;


typedef struct AVFrameSideData {
    enum AVFrameSideDataType type;
    uint8_t *data;
    int size;
    AVDictionary *metadata;
    AVBufferRef *buf;
} AVFrameSideData;


typedef struct AVFrame {
#define AV_NUM_DATA_POINTERS 8
/**
     * pointer to the picture/channel planes.
     * This might be different from the first allocated byte
     *
     * Some decoders access areas outside 0,0 - width,height, please
     * see avcodec_align_dimensions2(). Some filters and swscale can read
     * up to 16 bytes beyond the planes, if these filters are to be used,
     * then 16 extra bytes must be allocated.
     *
     * NOTE: Except for hwaccel formats, pointers not needed by the format
     * MUST be set to NULL.
     */
    uint8_t *data[AV_NUM_DATA_POINTERS];

/**
     * For video, size in bytes of each picture line.
     * For audio, size in bytes of each plane.
     *
     * For audio, only linesize[0] may be set. For planar audio, each channel
     * plane must be the same size.
     *
     * For video the linesizes should be multiples of the CPUs alignment
     * preference, this is 16 or 32 for modern desktop CPUs.
     * Some code requires such alignment other code can be slower without
     * correct alignment, for yet other it makes no difference.
     *
     * @note The linesize may be larger than the size of usable data -- there
     * may be extra padding present for performance reasons.
     */
    int linesize[AV_NUM_DATA_POINTERS];

/**
     * pointers to the data planes/channels.
     *
     * For video, this should simply point to data[].
     *
     * For planar audio, each channel has a separate data pointer, and
     * linesize[0] contains the size of each channel buffer.
     * For packed audio, there is just one data pointer, and linesize[0]
     * contains the total size of the buffer for all channels.
     *
     * Note: Both data and extended_data should always be set in a valid frame,
     * but for planar audio with more channels that can fit in data,
     * extended_data must be used in order to access all channels.
     */
    uint8_t **extended_data;

/**
     * @name Video dimensions
     * Video frames only. The coded dimensions (in pixels) of the video frame,
     * i.e. the size of the rectangle that contains some well-defined values.
     *
     * @note The part of the frame intended for display/presentation is further
     * restricted by the @ref cropping "Cropping rectangle".
     * @{
     */
    int width, height;
/**
     * @}
     */

/**
     * number of audio samples (per channel) described by this frame
     */
    int nb_samples;

/**
     * format of the frame, -1 if unknown or unset
     * Values correspond to enum AVPixelFormat for video frames,
     * enum AVSampleFormat for audio)
     */
    int format;

/**
     * 1 -> keyframe, 0-> not
     */
    int key_frame;

/**
     * Picture type of the frame.
     */
    enum AVPictureType pict_type;

/**
     * Sample aspect ratio for the video frame, 0/1 if unknown/unspecified.
     */
    AVRational sample_aspect_ratio;

/**
     * Presentation timestamp in time_base units (time when frame should be shown to user).
     */
    int64_t pts;

#if FF_API_PKT_PTS
                                                                                                                            /**
     * PTS copied from the AVPacket that was decoded to produce this frame.
     * @deprecated use the pts field instead
     */
attribute_deprecated
int64_t pkt_pts;
#endif

/**
     * DTS copied from the AVPacket that triggered returning this frame. (if frame threading isn't used)
     * This is also the Presentation time of this AVFrame calculated from
     * only AVPacket.dts values without pts values.
     */
    int64_t pkt_dts;

/**
     * picture number in bitstream order
     */
    int coded_picture_number;
/**
     * picture number in display order
     */
    int display_picture_number;

/**
     * quality (between 1 (good) and FF_LAMBDA_MAX (bad))
     */
    int quality;

/**
     * for some private data of the user
     */
    void *opaque;

#if FF_API_ERROR_FRAME
                                                                                                                            /**
     * @deprecated unused
     */
attribute_deprecated
uint64_t error[AV_NUM_DATA_POINTERS];
#endif

/**
     * When decoding, this signals how much the picture must be delayed.
     * extra_delay = repeat_pict / (2*fps)
     */
    int repeat_pict;

/**
     * The content of the picture is interlaced.
     */
    int interlaced_frame;

/**
     * If the content is interlaced, is top field displayed first.
     */
    int top_field_first;

/**
     * Tell user application that palette has changed from previous frame.
     */
    int palette_has_changed;

/**
     * reordered opaque 64 bits (generally an integer or a double precision float
     * PTS but can be anything).
     * The user sets AVCodecContext.reordered_opaque to represent the input at
     * that time,
     * the decoder reorders values as needed and sets AVFrame.reordered_opaque
     * to exactly one of the values provided by the user through AVCodecContext.reordered_opaque
     */
    int64_t reordered_opaque;

/**
     * Sample rate of the audio data.
     */
    int sample_rate;

/**
     * Channel layout of the audio data.
     */
    uint64_t channel_layout;

/**
     * AVBuffer references backing the data for this frame. If all elements of
     * this array are NULL, then this frame is not reference counted. This array
     * must be filled contiguously -- if buf[i] is non-NULL then buf[j] must
     * also be non-NULL for all j < i.
     *
     * There may be at most one AVBuffer per data plane, so for video this array
     * always contains all the references. For planar audio with more than
     * AV_NUM_DATA_POINTERS channels, there may be more buffers than can fit in
     * this array. Then the extra AVBufferRef pointers are stored in the
     * extended_buf array.
     */
    AVBufferRef *buf[AV_NUM_DATA_POINTERS];

/**
     * For planar audio which requires more than AV_NUM_DATA_POINTERS
     * AVBufferRef pointers, this array will hold all the references which
     * cannot fit into AVFrame.buf.
     *
     * Note that this is different from AVFrame.extended_data, which always
     * contains all the pointers. This array only contains the extra pointers,
     * which cannot fit into AVFrame.buf.
     *
     * This array is always allocated using av_malloc() by whoever constructs
     * the frame. It is freed in av_frame_unref().
     */
    AVBufferRef **extended_buf;
/**
     * Number of elements in extended_buf.
     */
    int nb_extended_buf;

    AVFrameSideData **side_data;
    int nb_side_data;

/**
 * @defgroup lavu_frame_flags AV_FRAME_FLAGS
 * @ingroup lavu_frame
 * Flags describing additional frame properties.
 *
 * @{
 */

/**
 * The frame data may be corrupted, e.g. due to decoding errors.
 */
#define AV_FRAME_FLAG_CORRUPT       (1 << 0)
/**
 * A flag to mark the frames which need to be decoded, but shouldn't be output.
 */
#define AV_FRAME_FLAG_DISCARD   (1 << 2)
/**
 * @}
 */

/**
     * Frame flags, a combination of @ref lavu_frame_flags
     */
    int flags;

/**
     * MPEG vs JPEG YUV range.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    enum AVColorRange color_range;

    enum AVColorPrimaries color_primaries;

    enum AVColorTransferCharacteristic color_trc;

/**
     * YUV colorspace type.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    enum AVColorSpace colorspace;

    enum AVChromaLocation chroma_location;

/**
     * frame timestamp estimated using various heuristics, in stream time base
     * - encoding: unused
     * - decoding: set by libavcodec, read by user.
     */
    int64_t best_effort_timestamp;

/**
     * reordered pos from the last AVPacket that has been input into the decoder
     * - encoding: unused
     * - decoding: Read by user.
     */
    int64_t pkt_pos;

/**
     * duration of the corresponding packet, expressed in
     * AVStream->time_base units, 0 if unknown.
     * - encoding: unused
     * - decoding: Read by user.
     */
    int64_t pkt_duration;

/**
     * metadata.
     * - encoding: Set by user.
     * - decoding: Set by libavcodec.
     */
    AVDictionary *metadata;

/**
     * decode error flags of the frame, set to a combination of
     * FF_DECODE_ERROR_xxx flags if the decoder produced a frame, but there
     * were errors during the decoding.
     * - encoding: unused
     * - decoding: set by libavcodec, read by user.
     */
    int decode_error_flags;
#define FF_DECODE_ERROR_INVALID_BITSTREAM   1
#define FF_DECODE_ERROR_MISSING_REFERENCE   2
#define FF_DECODE_ERROR_CONCEALMENT_ACTIVE  4
#define FF_DECODE_ERROR_DECODE_SLICES       8

/**
     * number of audio channels, only used for audio.
     * - encoding: unused
     * - decoding: Read by user.
     */
    int channels;

/**
     * size of the corresponding packet containing the compressed
     * frame.
     * It is set to a negative value if unknown.
     * - encoding: unused
     * - decoding: set by libavcodec, read by user.
     */
    int pkt_size;

#if FF_API_FRAME_QP
                                                                                                                            /**
     * QP table
     */
attribute_deprecated
int8_t *qscale_table;
/**
     * QP store stride
     */
attribute_deprecated
int qstride;

attribute_deprecated
int qscale_type;

attribute_deprecated
AVBufferRef *qp_table_buf;
#endif
/**
     * For hwaccel-format frames, this should be a reference to the
     * AVHWFramesContext describing the frame.
     */
    AVBufferRef *hw_frames_ctx;

/**
     * AVBufferRef for free use by the API user. FFmpeg will never check the
     * contents of the buffer ref. FFmpeg calls av_buffer_unref() on it when
     * the frame is unreferenced. av_frame_copy_props() calls create a new
     * reference with av_buffer_ref() for the target frame's opaque_ref field.
     *
     * This is unrelated to the opaque field, although it serves a similar
     * purpose.
     */
    AVBufferRef *opaque_ref;

/**
     * @anchor cropping
     * @name Cropping
     * Video frames only. The number of pixels to discard from the the
     * top/bottom/left/right border of the frame to obtain the sub-rectangle of
     * the frame intended for presentation.
     * @{
     */
    size_t crop_top;
    size_t crop_bottom;
    size_t crop_left;
    size_t crop_right;
/**
     * @}
     */

/**
     * AVBufferRef for internal use by a single libav* library.
     * Must not be used to transfer data between libraries.
     * Has to be NULL when ownership of the frame leaves the respective library.
     *
     * Code outside the FFmpeg libs should never check or change the contents of the buffer ref.
     *
     * FFmpeg calls av_buffer_unref() on it when the frame is unreferenced.
     * av_frame_copy_props() calls create a new reference with av_buffer_ref()
     * for the target frame's private_ref field.
     */
    AVBufferRef *private_ref;
} AVFrame;


typedef struct AVPixFmtDescriptor {
    const char *name;
    uint8_t nb_components;  ///< The number of components each pixel has, (1-4)

/**
     * Amount to shift the luma width right to find the chroma width.
     * For YV12 this is 1 for example.
     * chroma_width = AV_CEIL_RSHIFT(luma_width, log2_chroma_w)
     * The note above is needed to ensure rounding up.
     * This value only refers to the chroma components.
     */
    uint8_t log2_chroma_w;

/**
     * Amount to shift the luma height right to find the chroma height.
     * For YV12 this is 1 for example.
     * chroma_height= AV_CEIL_RSHIFT(luma_height, log2_chroma_h)
     * The note above is needed to ensure rounding up.
     * This value only refers to the chroma components.
     */
    uint8_t log2_chroma_h;

/**
     * Combination of AV_PIX_FMT_FLAG_... flags.
     */
    uint64_t flags;

/**
     * Parameters that describe how pixels are packed.
     * If the format has 1 or 2 components, then luma is 0.
     * If the format has 3 or 4 components:
     *   if the RGB flag is set then 0 is red, 1 is green and 2 is blue;
     *   otherwise 0 is luma, 1 is chroma-U and 2 is chroma-V.
     *
     * If present, the Alpha channel is always the last component.
     */
    AVComponentDescriptor comp[4];

/**
     * Alternative comma-separated names.
     */
    const char *alias;
} AVPixFmtDescriptor;


/**
 * Pixel format is big-endian.
*/
#define AV_PIX_FMT_FLAG_BE           (1 << 0)
/**
* Pixel format has a palette in data[1], values are indexes in this palette.
*/
#define AV_PIX_FMT_FLAG_PAL          (1 << 1)
/**
* All values of a component are bit-wise packed end to end.
*/
#define AV_PIX_FMT_FLAG_BITSTREAM    (1 << 2)
/**
* Pixel format is an HW accelerated format.
*/
#define AV_PIX_FMT_FLAG_HWACCEL      (1 << 3)
/**
* At least one pixel component is not in the first data plane.
*/
#define AV_PIX_FMT_FLAG_PLANAR       (1 << 4)
/**
* The pixel format contains RGB-like data (as opposed to YUV/grayscale).
*/
#define AV_PIX_FMT_FLAG_RGB          (1 << 5)

static inline int image_get_linesize(int width, int plane, int max_step, int max_step_comp, const AVPixFmtDescriptor *desc) {
    int s, shifted_w, linesize;

    if (!desc) {
        return AVERROR(EINVAL);
    }

    if (width < 0) {
        return AVERROR(EINVAL);
    }
    s = (max_step_comp == 1 || max_step_comp == 2) ? desc->log2_chroma_w : 0;
    shifted_w = ((width + (1 << s) - 1)) >> s;
    if (shifted_w && max_step > INT_MAX / shifted_w) {
        return AVERROR(EINVAL);
    }
    linesize = max_step * shifted_w;

    if (desc->flags & AV_PIX_FMT_FLAG_BITSTREAM) {
        linesize = (linesize + 7) >> 3;
    }
    return linesize;
}


static const AVPixFmtDescriptor av_pix_fmt_descriptors[AV_PIX_FMT_NB] = {[AV_PIX_FMT_YUV420P] = {.name = "yuv420p", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                                                         ,        /* Y */
        {                                                                                                                                                                                 1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                                                         ,        /* U */
        {                                                                                                                                                                                 2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                                                         ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUYV422] = {.name = "yuyv422", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 8, 1, 7, 1}
                                                                                                                                                      ,        /* Y */
        {                                                                                                                                              0, 4, 1, 0, 8, 3, 7, 2}
                                                                                                                                                      ,        /* U */
        {                                                                                                                                              0, 4, 3, 0, 8, 3, 7, 4}
                                                                                                                                                      ,        /* V */
},}, [AV_PIX_FMT_YVYU422] = {.name = "yvyu422", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 8, 1, 7, 1}
                                                                                                                     ,        /* Y */
        {                                                                                                             0, 4, 3, 0, 8, 3, 7, 4}
                                                                                                                     ,        /* U */
        {                                                                                                             0, 4, 1, 0, 8, 3, 7, 2}
                                                                                                                     ,        /* V */
},}, [AV_PIX_FMT_RGB24] = {.name = "rgb24", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 3, 0, 0, 8, 2, 7, 1}
                                                                                                                 ,        /* R */
        {                                                                                                         0, 3, 1, 0, 8, 2, 7, 2}
                                                                                                                 ,        /* G */
        {                                                                                                         0, 3, 2, 0, 8, 2, 7, 3}
                                                                                                                 ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_BGR24] = {.name = "bgr24", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 3, 2, 0, 8, 2, 7, 3}
                                                                                                                                               ,        /* R */
        {                                                                                                                                       0, 3, 1, 0, 8, 2, 7, 2}
                                                                                                                                               ,        /* G */
        {                                                                                                                                       0, 3, 0, 0, 8, 2, 7, 1}
                                                                                                                                               ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_YUV422P] = {.name = "yuv422p", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                   ,        /* Y */
        {                                                                                                                                           1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                   ,        /* U */
        {                                                                                                                                           2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                   ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV444P] = {.name = "yuv444p", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                      ,        /* Y */
        {                                                                                                                                              1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                      ,        /* U */
        {                                                                                                                                              2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                      ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV410P] = {.name = "yuv410p", .nb_components = 3, .log2_chroma_w = 2, .log2_chroma_h = 2, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                      ,        /* Y */
        {                                                                                                                                              1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                      ,        /* U */
        {                                                                                                                                              2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                      ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV411P] = {.name = "yuv411p", .nb_components = 3, .log2_chroma_w = 2, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                      ,        /* Y */
        {                                                                                                                                              1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                      ,        /* U */
        {                                                                                                                                              2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                      ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUVJ411P] = {.name = "yuvj411p", .nb_components = 3, .log2_chroma_w = 2, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* Y */
        {                                                                                                                                                1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* U */
        {                                                                                                                                                2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_GRAY8] = {.name = "gray", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                 ,        /* Y */
}, .flags = FF_PSEUDOPAL, .alias = "gray8,y8",}, [AV_PIX_FMT_MONOWHITE] = {.name = "monow", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 1, 0, 0, 1}
                                                                                                                                                                 ,        /* Y */
}, .flags = AV_PIX_FMT_FLAG_BITSTREAM,}, [AV_PIX_FMT_MONOBLACK] = {.name = "monob", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 7, 1, 0, 0, 1}
                                                                                                                                                         ,        /* Y */
}, .flags = AV_PIX_FMT_FLAG_BITSTREAM,}, [AV_PIX_FMT_PAL8] = {.name = "pal8", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                   ,}, .flags =
AV_PIX_FMT_FLAG_PAL |
AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVJ420P] = {.name = "yuvj420p", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                           ,        /* Y */
        {                                                                                                                                   1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                           ,        /* U */
        {                                                                                                                                   2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                           ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUVJ422P] = {.name = "yuvj422p", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* Y */
        {                                                                                                                                                1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* U */
        {                                                                                                                                                2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUVJ444P] = {.name = "yuvj444p", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* Y */
        {                                                                                                                                                1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* U */
        {                                                                                                                                                2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_XVMC] = {.name = "xvmc", .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_UYVY422] = {.name = "uyvy422", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 1, 0, 8, 1, 7, 2}
                                                                                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                                                                                       0, 4, 0, 0, 8, 3, 7, 1}
                                                                                                                                                                                                                               ,        /* U */
        {                                                                                                                                                                                                                       0, 4, 2, 0, 8, 3, 7, 3}
                                                                                                                                                                                                                               ,        /* V */
},}, [AV_PIX_FMT_UYYVYY411] = {.name = "uyyvyy411", .nb_components = 3, .log2_chroma_w = 2, .log2_chroma_h = 0, .comp = {{0, 4, 1, 0, 8, 3, 7, 2}
                                                                                                                         ,        /* Y */
        {                                                                                                                 0, 6, 0, 0, 8, 5, 7, 1}
                                                                                                                         ,        /* U */
        {                                                                                                                 0, 6, 3, 0, 8, 5, 7, 4}
                                                                                                                         ,        /* V */
},}, [AV_PIX_FMT_BGR8] = {.name = "bgr8", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 3, 0, 2, 1}
                                                                                                               ,        /* R */
        {                                                                                                       0, 1, 0, 3, 3, 0, 2, 1}
                                                                                                               ,        /* G */
        {                                                                                                       0, 1, 0, 6, 2, 0, 1, 1}
                                                                                                               ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB |
            FF_PSEUDOPAL,}, [AV_PIX_FMT_BGR4] = {.name = "bgr4", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 4, 3, 0, 1, 3, 0, 4}
                                                                                                                                      ,        /* R */
        {                                                                                                                              0, 4, 1, 0, 2, 3, 1, 2}
                                                                                                                                      ,        /* G */
        {                                                                                                                              0, 4, 0, 0, 1, 3, 0, 1}
                                                                                                                                      ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_BITSTREAM |
            AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_BGR4_BYTE] = {.name = "bgr4_byte", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 1, 0, 0, 1}
                                                                                                                                                       ,        /* R */
        {                                                                                                                                               0, 1, 0, 1, 2, 0, 1, 1}
                                                                                                                                                       ,        /* G */
        {                                                                                                                                               0, 1, 0, 3, 1, 0, 0, 1}
                                                                                                                                                       ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB |
            FF_PSEUDOPAL,}, [AV_PIX_FMT_RGB8] = {.name = "rgb8", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 6, 2, 0, 1, 1}
                                                                                                                                      ,        /* R */
        {                                                                                                                              0, 1, 0, 3, 3, 0, 2, 1}
                                                                                                                                      ,        /* G */
        {                                                                                                                              0, 1, 0, 0, 3, 0, 2, 1}
                                                                                                                                      ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB |
            FF_PSEUDOPAL,}, [AV_PIX_FMT_RGB4] = {.name = "rgb4", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 4, 0, 0, 1, 3, 0, 1}
                                                                                                                                      ,        /* R */
        {                                                                                                                              0, 4, 1, 0, 2, 3, 1, 2}
                                                                                                                                      ,        /* G */
        {                                                                                                                              0, 4, 3, 0, 1, 3, 0, 4}
                                                                                                                                      ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_BITSTREAM |
            AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_RGB4_BYTE] = {.name = "rgb4_byte", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 3, 1, 0, 0, 1}
                                                                                                                                                       ,        /* R */
        {                                                                                                                                               0, 1, 0, 1, 2, 0, 1, 1}
                                                                                                                                                       ,        /* G */
        {                                                                                                                                               0, 1, 0, 0, 1, 0, 0, 1}
                                                                                                                                                       ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB |
            FF_PSEUDOPAL,}, [AV_PIX_FMT_NV12] = {.name = "nv12", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                      ,        /* Y */
        {                                                                                                                              1, 2, 0, 0, 8, 1, 7, 1}
                                                                                                                                      ,        /* U */
        {                                                                                                                              1, 2, 1, 0, 8, 1, 7, 2}
                                                                                                                                      ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_NV21] = {.name = "nv21", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                ,        /* Y */
        {                                                                                                                                        1, 2, 1, 0, 8, 1, 7, 2}
                                                                                                                                                ,        /* U */
        {                                                                                                                                        1, 2, 0, 0, 8, 1, 7, 1}
                                                                                                                                                ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_ARGB] = {.name = "argb", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 4, 1, 0, 8, 3, 7, 2}
                                                                                                                                                ,        /* R */
        {                                                                                                                                        0, 4, 2, 0, 8, 3, 7, 3}
                                                                                                                                                ,        /* G */
        {                                                                                                                                        0, 4, 3, 0, 8, 3, 7, 4}
                                                                                                                                                ,        /* B */
        {                                                                                                                                        0, 4, 0, 0, 8, 3, 7, 1}
                                                                                                                                                ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_RGB |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_RGBA] = {.name = "rgba", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 4, 0, 0, 8, 3, 7, 1}
                                                                                                                                               ,        /* R */
        {                                                                                                                                       0, 4, 1, 0, 8, 3, 7, 2}
                                                                                                                                               ,        /* G */
        {                                                                                                                                       0, 4, 2, 0, 8, 3, 7, 3}
                                                                                                                                               ,        /* B */
        {                                                                                                                                       0, 4, 3, 0, 8, 3, 7, 4}
                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_RGB |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_ABGR] = {.name = "abgr", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 4, 3, 0, 8, 3, 7, 4}
                                                                                                                                               ,        /* R */
        {                                                                                                                                       0, 4, 2, 0, 8, 3, 7, 3}
                                                                                                                                               ,        /* G */
        {                                                                                                                                       0, 4, 1, 0, 8, 3, 7, 2}
                                                                                                                                               ,        /* B */
        {                                                                                                                                       0, 4, 0, 0, 8, 3, 7, 1}
                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_RGB |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_BGRA] = {.name = "bgra", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 4, 2, 0, 8, 3, 7, 3}
                                                                                                                                               ,        /* R */
        {                                                                                                                                       0, 4, 1, 0, 8, 3, 7, 2}
                                                                                                                                               ,        /* G */
        {                                                                                                                                       0, 4, 0, 0, 8, 3, 7, 1}
                                                                                                                                               ,        /* B */
        {                                                                                                                                       0, 4, 3, 0, 8, 3, 7, 4}
                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_RGB |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_0RGB] = {.name = "0rgb", .nb_components= 3, .log2_chroma_w= 0, .log2_chroma_h= 0, .comp = {{0, 4, 1, 0, 8, 3, 7, 2}
                                                                                                                                            ,        /* R */
        {                                                                                                                                    0, 4, 2, 0, 8, 3, 7, 3}
                                                                                                                                            ,        /* G */
        {                                                                                                                                    0, 4, 3, 0, 8, 3, 7, 4}
                                                                                                                                            ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_RGB0] = {.name = "rgb0", .nb_components= 3, .log2_chroma_w= 0, .log2_chroma_h= 0, .comp = {{0, 4, 0, 0, 8, 3, 7, 1}
                                                                                                                                          ,        /* R */
        {                                                                                                                                  0, 4, 1, 0, 8, 3, 7, 2}
                                                                                                                                          ,        /* G */
        {                                                                                                                                  0, 4, 2, 0, 8, 3, 7, 3}
                                                                                                                                          ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_0BGR] = {.name = "0bgr", .nb_components= 3, .log2_chroma_w= 0, .log2_chroma_h= 0, .comp = {{0, 4, 3, 0, 8, 3, 7, 4}
                                                                                                                                          ,        /* R */
        {                                                                                                                                  0, 4, 2, 0, 8, 3, 7, 3}
                                                                                                                                          ,        /* G */
        {                                                                                                                                  0, 4, 1, 0, 8, 3, 7, 2}
                                                                                                                                          ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_BGR0] = {.name = "bgr0", .nb_components= 3, .log2_chroma_w= 0, .log2_chroma_h= 0, .comp = {{0, 4, 2, 0, 8, 3, 7, 3}
                                                                                                                                          ,        /* R */
        {                                                                                                                                  0, 4, 1, 0, 8, 3, 7, 2}
                                                                                                                                          ,        /* G */
        {                                                                                                                                  0, 4, 0, 0, 8, 3, 7, 1}
                                                                                                                                          ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GRAY9BE] = {.name = "gray9be", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                   ,       /* Y */
}, .flags = AV_PIX_FMT_FLAG_BE, .alias = "y9be",}, [AV_PIX_FMT_GRAY9LE] = {.name = "gray9le", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                   ,       /* Y */
}, .alias = "y9le",}, [AV_PIX_FMT_GRAY10BE] = {.name = "gray10be", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                        ,       /* Y */
}, .flags = AV_PIX_FMT_FLAG_BE, .alias = "y10be",}, [AV_PIX_FMT_GRAY10LE] = {.name = "gray10le", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,       /* Y */
}, .alias = "y10le",}, [AV_PIX_FMT_GRAY12BE] = {.name = "gray12be", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                         ,       /* Y */
}, .flags = AV_PIX_FMT_FLAG_BE, .alias = "y12be",}, [AV_PIX_FMT_GRAY12LE] = {.name = "gray12le", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,       /* Y */
}, .alias = "y12le",}, [AV_PIX_FMT_GRAY14BE] = {.name = "gray14be", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                         ,       /* Y */
}, .flags = AV_PIX_FMT_FLAG_BE, .alias = "y14be",}, [AV_PIX_FMT_GRAY14LE] = {.name = "gray14le", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,       /* Y */
}, .alias = "y14le",}, [AV_PIX_FMT_GRAY16BE] = {.name = "gray16be", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                         ,       /* Y */
}, .flags = AV_PIX_FMT_FLAG_BE, .alias = "y16be",}, [AV_PIX_FMT_GRAY16LE] = {.name = "gray16le", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,       /* Y */
}, .alias = "y16le",}, [AV_PIX_FMT_YUV440P] = {.name = "yuv440p", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 1, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                       ,        /* Y */
        {                                                                                                                               1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                       ,        /* U */
        {                                                                                                                               2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                       ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUVJ440P] = {.name = "yuvj440p", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 1, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* Y */
        {                                                                                                                                                1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* U */
        {                                                                                                                                                2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV440P10LE] = {.name = "yuv440p10le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                              ,        /* Y */
        {                                                                                                                                                      1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                              ,        /* U */
        {                                                                                                                                                      2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                              ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV440P10BE] = {.name = "yuv440p10be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                              ,        /* Y */
        {                                                                                                                                                      1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                              ,        /* U */
        {                                                                                                                                                      2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                              ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_BE |
            AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV440P12LE] = {.name = "yuv440p12le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                              ,        /* Y */
        {                                                                                                                                                      1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                              ,        /* U */
        {                                                                                                                                                      2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                              ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV440P12BE] = {.name = "yuv440p12be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                              ,        /* Y */
        {                                                                                                                                                      1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                              ,        /* U */
        {                                                                                                                                                      2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                              ,        /* V */
}, .flags = AV_PIX_FMT_FLAG_BE |
            AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUVA420P] = {.name = "yuva420p", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* Y */
        {                                                                                                                                                1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* U */
        {                                                                                                                                                2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* V */
        {                                                                                                                                                3, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA422P] = {.name = "yuva422p", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* Y */
        {                                                                                                                                               1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* U */
        {                                                                                                                                               2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* V */
        {                                                                                                                                               3, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA444P] = {.name = "yuva444p", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* Y */
        {                                                                                                                                               1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* U */
        {                                                                                                                                               2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* V */
        {                                                                                                                                               3, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA420P9BE] = {.name = "yuva420p9be", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* Y */
        {                                                                                                                                                     1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* U */
        {                                                                                                                                                     2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* V */
        {                                                                                                                                                     3, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA420P9LE] = {.name = "yuva420p9le", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* Y */
        {                                                                                                                                                     1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* U */
        {                                                                                                                                                     2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* V */
        {                                                                                                                                                     3, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA422P9BE] = {.name = "yuva422p9be", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* Y */
        {                                                                                                                                                     1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* U */
        {                                                                                                                                                     2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* V */
        {                                                                                                                                                     3, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA422P9LE] = {.name = "yuva422p9le", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* Y */
        {                                                                                                                                                     1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* U */
        {                                                                                                                                                     2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* V */
        {                                                                                                                                                     3, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA444P9BE] = {.name = "yuva444p9be", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* Y */
        {                                                                                                                                                     1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* U */
        {                                                                                                                                                     2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* V */
        {                                                                                                                                                     3, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA444P9LE] = {.name = "yuva444p9le", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* Y */
        {                                                                                                                                                     1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* U */
        {                                                                                                                                                     2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* V */
        {                                                                                                                                                     3, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                             ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA420P10BE] = {.name = "yuva420p10be", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA420P10LE] = {.name = "yuva420p10le", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA422P10BE] = {.name = "yuva422p10be", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA422P10LE] = {.name = "yuva422p10le", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA444P10BE] = {.name = "yuva444p10be", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA444P10LE] = {.name = "yuva444p10le", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA420P16BE] = {.name = "yuva420p16be", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA420P16LE] = {.name = "yuva420p16le", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA422P16BE] = {.name = "yuva422p16be", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA422P16LE] = {.name = "yuva422p16le", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA444P16BE] = {.name = "yuva444p16be", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA444P16LE] = {.name = "yuva444p16le", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* Y */
        {                                                                                                                                                       1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* U */
        {                                                                                                                                                       2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* V */
        {                                                                                                                                                       3, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                               ,        /* A */
}, .flags = AV_PIX_FMT_FLAG_PLANAR |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_RGB48BE] = {.name = "rgb48be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 6, 0, 0, 16, 5, 15, 1}
                                                                                                                                                     ,       /* R */
        {                                                                                                                                             0, 6, 2, 0, 16, 5, 15, 3}
                                                                                                                                                     ,       /* G */
        {                                                                                                                                             0, 6, 4, 0, 16, 5, 15, 5}
                                                                                                                                                     ,       /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB |
            AV_PIX_FMT_FLAG_BE,}, [AV_PIX_FMT_RGB48LE] = {.name = "rgb48le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 6, 0, 0, 16, 5, 15, 1}
                                                                                                                                                  ,       /* R */
        {                                                                                                                                          0, 6, 2, 0, 16, 5, 15, 3}
                                                                                                                                                  ,       /* G */
        {                                                                                                                                          0, 6, 4, 0, 16, 5, 15, 5}
                                                                                                                                                  ,       /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_RGBA64BE] = {.name = "rgba64be", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 8, 0, 0, 16, 7, 15, 1}
                                                                                                                                                     ,       /* R */
        {                                                                                                                                             0, 8, 2, 0, 16, 7, 15, 3}
                                                                                                                                                     ,       /* G */
        {                                                                                                                                             0, 8, 4, 0, 16, 7, 15, 5}
                                                                                                                                                     ,       /* B */
        {                                                                                                                                             0, 8, 6, 0, 16, 7, 15, 7}
                                                                                                                                                     ,       /* A */
}, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_RGBA64LE] = {.name = "rgba64le", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 8, 0, 0, 16, 7, 15, 1}
                                                                                                                                                       ,       /* R */
        {                                                                                                                                               0, 8, 2, 0, 16, 7, 15, 3}
                                                                                                                                                       ,       /* G */
        {                                                                                                                                               0, 8, 4, 0, 16, 7, 15, 5}
                                                                                                                                                       ,       /* B */
        {                                                                                                                                               0, 8, 6, 0, 16, 7, 15, 7}
                                                                                                                                                       ,       /* A */
}, .flags = AV_PIX_FMT_FLAG_RGB |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_RGB565BE] = {.name = "rgb565be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, -1, 3, 5, 1, 4, 0}
                                                                                                                                                       ,        /* R */
        {                                                                                                                                               0, 2, 0 , 5, 6, 1, 5, 1}
                                                                                                                                                       ,        /* G */
        {                                                                                                                                               0, 2, 0 , 0, 5, 1, 4, 1}
                                                                                                                                                       ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_BE |
            AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_RGB565LE] = {.name = "rgb565le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 1, 3, 5, 1, 4, 2}
                                                                                                                                                     ,        /* R */
        {                                                                                                                                             0, 2, 0, 5, 6, 1, 5, 1}
                                                                                                                                                     ,        /* G */
        {                                                                                                                                             0, 2, 0, 0, 5, 1, 4, 1}
                                                                                                                                                     ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_RGB555BE] = {.name = "rgb555be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, -1, 2, 5, 1, 4, 0}
                                                                                                                                                     ,        /* R */
        {                                                                                                                                             0, 2, 0 , 5, 5, 1, 4, 1}
                                                                                                                                                     ,        /* G */
        {                                                                                                                                             0, 2, 0 , 0, 5, 1, 4, 1}
                                                                                                                                                     ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_BE |
            AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_RGB555LE] = {.name = "rgb555le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 1, 2, 5, 1, 4, 2}
                                                                                                                                                     ,        /* R */
        {                                                                                                                                             0, 2, 0, 5, 5, 1, 4, 1}
                                                                                                                                                     ,        /* G */
        {                                                                                                                                             0, 2, 0, 0, 5, 1, 4, 1}
                                                                                                                                                     ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_RGB444BE] = {.name = "rgb444be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, -1, 0, 4, 1, 3, 0}
                                                                                                                                                     ,        /* R */
        {                                                                                                                                             0, 2, 0 , 4, 4, 1, 3, 1}
                                                                                                                                                     ,        /* G */
        {                                                                                                                                             0, 2, 0 , 0, 4, 1, 3, 1}
                                                                                                                                                     ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_BE |
            AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_RGB444LE] = {.name = "rgb444le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 1, 0, 4, 1, 3, 2}
                                                                                                                                                     ,        /* R */
        {                                                                                                                                             0, 2, 0, 4, 4, 1, 3, 1}
                                                                                                                                                     ,        /* G */
        {                                                                                                                                             0, 2, 0, 0, 4, 1, 3, 1}
                                                                                                                                                     ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_BGR48BE] = {.name = "bgr48be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 6, 4, 0, 16, 5, 15, 5}
                                                                                                                                                   ,       /* R */
        {                                                                                                                                           0, 6, 2, 0, 16, 5, 15, 3}
                                                                                                                                                   ,       /* G */
        {                                                                                                                                           0, 6, 0, 0, 16, 5, 15, 1}
                                                                                                                                                   ,       /* B */
}, .flags = AV_PIX_FMT_FLAG_BE |
            AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_BGR48LE] = {.name = "bgr48le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 6, 4, 0, 16, 5, 15, 5}
                                                                                                                                                   ,       /* R */
        {                                                                                                                                           0, 6, 2, 0, 16, 5, 15, 3}
                                                                                                                                                   ,       /* G */
        {                                                                                                                                           0, 6, 0, 0, 16, 5, 15, 1}
                                                                                                                                                   ,       /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_BGRA64BE] = {.name = "bgra64be", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 8, 4, 0, 16, 7, 15, 5}
                                                                                                                                                     ,       /* R */
        {                                                                                                                                             0, 8, 2, 0, 16, 7, 15, 3}
                                                                                                                                                     ,       /* G */
        {                                                                                                                                             0, 8, 0, 0, 16, 7, 15, 1}
                                                                                                                                                     ,       /* B */
        {                                                                                                                                             0, 8, 6, 0, 16, 7, 15, 7}
                                                                                                                                                     ,       /* A */
}, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_BGRA64LE] = {.name = "bgra64le", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 8, 4, 0, 16, 7, 15, 5}
                                                                                                                                                       ,       /* R */
        {                                                                                                                                               0, 8, 2, 0, 16, 7, 15, 3}
                                                                                                                                                       ,       /* G */
        {                                                                                                                                               0, 8, 0, 0, 16, 7, 15, 1}
                                                                                                                                                       ,       /* B */
        {                                                                                                                                               0, 8, 6, 0, 16, 7, 15, 7}
                                                                                                                                                       ,       /* A */
}, .flags = AV_PIX_FMT_FLAG_RGB |
            AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_BGR565BE] = {.name = "bgr565be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0 , 0, 5, 1, 4, 1}
                                                                                                                                                       ,        /* R */
        {                                                                                                                                               0, 2, 0 , 5, 6, 1, 5, 1}
                                                                                                                                                       ,        /* G */
        {                                                                                                                                               0, 2, -1, 3, 5, 1, 4, 0}
                                                                                                                                                       ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_BE |
            AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_BGR565LE] = {.name = "bgr565le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 5, 1, 4, 1}
                                                                                                                                                     ,        /* R */
        {                                                                                                                                             0, 2, 0, 5, 6, 1, 5, 1}
                                                                                                                                                     ,        /* G */
        {                                                                                                                                             0, 2, 1, 3, 5, 1, 4, 2}
                                                                                                                                                     ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_BGR555BE] = {.name = "bgr555be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0 , 0, 5, 1, 4, 1}
                                                                                                                                                     ,       /* R */
        {                                                                                                                                             0, 2, 0 , 5, 5, 1, 4, 1}
                                                                                                                                                     ,       /* G */
        {                                                                                                                                             0, 2, -1, 2, 5, 1, 4, 0}
                                                                                                                                                     ,       /* B */
}, .flags = AV_PIX_FMT_FLAG_BE |
            AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_BGR555LE] = {.name = "bgr555le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 5, 1, 4, 1}
                                                                                                                                                     ,        /* R */
        {                                                                                                                                             0, 2, 0, 5, 5, 1, 4, 1}
                                                                                                                                                     ,        /* G */
        {                                                                                                                                             0, 2, 1, 2, 5, 1, 4, 2}
                                                                                                                                                     ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_BGR444BE] = {.name = "bgr444be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0 , 0, 4, 1, 3, 1}
                                                                                                                                                     ,       /* R */
        {                                                                                                                                             0, 2, 0 , 4, 4, 1, 3, 1}
                                                                                                                                                     ,       /* G */
        {                                                                                                                                             0, 2, -1, 0, 4, 1, 3, 0}
                                                                                                                                                     ,       /* B */
}, .flags = AV_PIX_FMT_FLAG_BE |
            AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_BGR444LE] = {.name = "bgr444le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 4, 1, 3, 1}
                                                                                                                                                     ,        /* R */
        {                                                                                                                                             0, 2, 0, 4, 4, 1, 3, 1}
                                                                                                                                                     ,        /* G */
        {                                                                                                                                             0, 2, 1, 0, 4, 1, 3, 2}
                                                                                                                                                     ,        /* B */
}, .flags = AV_PIX_FMT_FLAG_RGB,},
#if FF_API_VAAPI
        [AV_PIX_FMT_VAAPI_MOCO] = {
.name = "vaapi_moco",
.log2_chroma_w = 1,
.log2_chroma_h = 1,
.flags = AV_PIX_FMT_FLAG_HWACCEL,
},
[AV_PIX_FMT_VAAPI_IDCT] = {
.name = "vaapi_idct",
.log2_chroma_w = 1,
.log2_chroma_h = 1,
.flags = AV_PIX_FMT_FLAG_HWACCEL,
},
[AV_PIX_FMT_VAAPI_VLD] = {
.name = "vaapi_vld",
.log2_chroma_w = 1,
.log2_chroma_h = 1,
.flags = AV_PIX_FMT_FLAG_HWACCEL,
},
#else
        [AV_PIX_FMT_VAAPI] = {.name = "vaapi", .log2_chroma_w = 1, .log2_chroma_h = 1, .flags = AV_PIX_FMT_FLAG_HWACCEL,},
#endif
        [AV_PIX_FMT_YUV420P9LE] = {.name = "yuv420p9le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                              ,        /* Y */
                {                                                                                                              1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                              ,        /* U */
                {                                                                                                              2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                              ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV420P9BE] = {.name = "yuv420p9be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* Y */
                {                                                                                                                                                    1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* U */
                {                                                                                                                                                    2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV420P10LE] = {.name = "yuv420p10le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV420P10BE] = {.name = "yuv420p10be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV420P12LE] = {.name = "yuv420p12le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV420P12BE] = {.name = "yuv420p12be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV420P14LE] = {.name = "yuv420p14le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV420P14BE] = {.name = "yuv420p14be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV420P16LE] = {.name = "yuv420p16le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV420P16BE] = {.name = "yuv420p16be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV422P9LE] = {.name = "yuv422p9le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* Y */
                {                                                                                                                                                    1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* U */
                {                                                                                                                                                    2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV422P9BE] = {.name = "yuv422p9be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* Y */
                {                                                                                                                                                    1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* U */
                {                                                                                                                                                    2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV422P10LE] = {.name = "yuv422p10le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV422P10BE] = {.name = "yuv422p10be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV422P12LE] = {.name = "yuv422p12le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV422P12BE] = {.name = "yuv422p12be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV422P14LE] = {.name = "yuv422p14le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV422P14BE] = {.name = "yuv422p14be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV422P16LE] = {.name = "yuv422p16le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV422P16BE] = {.name = "yuv422p16be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV444P16LE] = {.name = "yuv444p16le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV444P16BE] = {.name = "yuv444p16be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV444P10LE] = {.name = "yuv444p10le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV444P10BE] = {.name = "yuv444p10be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV444P9LE] = {.name = "yuv444p9le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* Y */
                {                                                                                                                                                    1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* U */
                {                                                                                                                                                    2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV444P9BE] = {.name = "yuv444p9be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* Y */
                {                                                                                                                                                    1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* U */
                {                                                                                                                                                    2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                                    ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV444P12LE] = {.name = "yuv444p12le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV444P12BE] = {.name = "yuv444p12be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV444P14LE] = {.name = "yuv444p14le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_YUV444P14BE] = {.name = "yuv444p14be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* Y */
                {                                                                                                                                                      1, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* U */
                {                                                                                                                                                      2, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                                      ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_D3D11VA_VLD] = {.name = "d3d11va_vld", .log2_chroma_w = 1, .log2_chroma_h = 1, .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_DXVA2_VLD] = {.name = "dxva2_vld", .log2_chroma_w = 1, .log2_chroma_h = 1, .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_YA8] = {.name = "ya8", .nb_components = 2, .comp = {{0, 2, 0, 0, 8, 1, 7, 1}
                                                                                                                                                                                                                                                                                                                                                                        ,        /* Y */
                {                                                                                                                                                                                                                                                                                                                                                        0, 2, 1, 0, 8, 1, 7, 2}
                                                                                                                                                                                                                                                                                                                                                                        ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_ALPHA, .alias = "gray8a",}, [AV_PIX_FMT_YA16LE] = {.name = "ya16le", .nb_components = 2, .comp = {{0, 4, 0, 0, 16, 3, 15, 1}
                                                                                                                                      ,        /* Y */
                {                                                                                                                      0, 4, 2, 0, 16, 3, 15, 3}
                                                                                                                                      ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YA16BE] = {.name = "ya16be", .nb_components = 2, .comp = {{0, 4, 0, 0, 16, 3, 15, 1}
                                                                                                                   ,        /* Y */
                {                                                                                                   0, 4, 2, 0, 16, 3, 15, 3}
                                                                                                                   ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_VIDEOTOOLBOX] = {.name = "videotoolbox_vld", .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_GBRP] = {.name = "gbrp", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                                                                                                                    ,        /* R */
                {                                                                                                                                                                                                                                    0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                                                                                                                    ,        /* G */
                {                                                                                                                                                                                                                                    1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                                                                                                                    ,        /* B */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRP9LE] = {.name = "gbrp9le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                           ,        /* R */
                {                                                                                                                                           0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                           ,        /* G */
                {                                                                                                                                           1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                           ,        /* B */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRP9BE] = {.name = "gbrp9be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                           ,        /* R */
                {                                                                                                                                           0, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                           ,        /* G */
                {                                                                                                                                           1, 2, 0, 0, 9, 1, 8, 1}
                                                                                                                                                           ,        /* B */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRP10LE] = {.name = "gbrp10le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                             ,        /* R */
                {                                                                                                                                             0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                             ,        /* G */
                {                                                                                                                                             1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                             ,        /* B */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRP10BE] = {.name = "gbrp10be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                             ,        /* R */
                {                                                                                                                                             0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                             ,        /* G */
                {                                                                                                                                             1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                             ,        /* B */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRP12LE] = {.name = "gbrp12le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                             ,        /* R */
                {                                                                                                                                             0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                             ,        /* G */
                {                                                                                                                                             1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                             ,        /* B */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRP12BE] = {.name = "gbrp12be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                             ,        /* R */
                {                                                                                                                                             0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                             ,        /* G */
                {                                                                                                                                             1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                             ,        /* B */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRP14LE] = {.name = "gbrp14le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                             ,        /* R */
                {                                                                                                                                             0, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                             ,        /* G */
                {                                                                                                                                             1, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                             ,        /* B */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRP14BE] = {.name = "gbrp14be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                             ,        /* R */
                {                                                                                                                                             0, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                             ,        /* G */
                {                                                                                                                                             1, 2, 0, 0, 14, 1, 13, 1}
                                                                                                                                                             ,        /* B */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRP16LE] = {.name = "gbrp16le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                             ,       /* R */
                {                                                                                                                                             0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                             ,       /* G */
                {                                                                                                                                             1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                             ,       /* B */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRP16BE] = {.name = "gbrp16be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                             ,       /* R */
                {                                                                                                                                             0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                             ,       /* G */
                {                                                                                                                                             1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                             ,       /* B */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRAP] = {.name = "gbrap", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* R */
                {                                                                                                                                       0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* G */
                {                                                                                                                                       1, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* B */
                {                                                                                                                                       3, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_GBRAP16LE] = {.name = "gbrap16le", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                 ,       /* R */
                {                                                                                                                                                 0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                 ,       /* G */
                {                                                                                                                                                 1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                 ,       /* B */
                {                                                                                                                                                 3, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                 ,       /* A */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_GBRAP16BE] = {.name = "gbrap16be", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                 ,       /* R */
                {                                                                                                                                                 0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                 ,       /* G */
                {                                                                                                                                                 1, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                 ,       /* B */
                {                                                                                                                                                 3, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                                 ,       /* A */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_VDPAU] = {.name = "vdpau", .log2_chroma_w = 1, .log2_chroma_h = 1, .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_XYZ12LE] = {.name = "xyz12le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 6, 0, 4, 12, 5, 11, 1}
                                                                                                                                                                                                                                                                                ,       /* X */
                {                                                                                                                                                                                                                                                                0, 6, 2, 4, 12, 5, 11, 3}
                                                                                                                                                                                                                                                                                ,       /* Y */
                {                                                                                                                                                                                                                                                                0, 6, 4, 4, 12, 5, 11, 5}
                                                                                                                                                                                                                                                                                ,       /* Z */
        },
/*.flags = -- not used*/
        }, [AV_PIX_FMT_XYZ12BE] = {.name = "xyz12be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 6, 0, 4, 12, 5, 11, 1}
                                                                                                                           ,       /* X */
                {                                                                                                           0, 6, 2, 4, 12, 5, 11, 3}
                                                                                                                           ,       /* Y */
                {                                                                                                           0, 6, 4, 4, 12, 5, 11, 5}
                                                                                                                           ,       /* Z */
        }, .flags = AV_PIX_FMT_FLAG_BE,},
#define BAYER8_DESC_COMMON \
        .nb_components= 3, \
        .log2_chroma_w= 0, \
        .log2_chroma_h= 0, \
        .comp = {          \
            {0,1,0,0,2,0,1,1},\
            {0,1,0,0,4,0,3,1},\
            {0,1,0,0,2,0,1,1},\
        },                 \

#define BAYER16_DESC_COMMON \
        .nb_components= 3, \
        .log2_chroma_w= 0, \
        .log2_chroma_h= 0, \
        .comp = {          \
            {0,2,0,0,4,1,3,1},\
            {0,2,0,0,8,1,7,1},\
            {0,2,0,0,4,1,3,1},\
        },                 \

        [AV_PIX_FMT_BAYER_BGGR8] = {.name = "bayer_bggr8", BAYER8_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_BAYER_BGGR16LE] = {.name = "bayer_bggr16le", BAYER16_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_BAYER_BGGR16BE] = {.name = "bayer_bggr16be", BAYER16_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_BAYER_RGGB8] = {.name = "bayer_rggb8", BAYER8_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_BAYER_RGGB16LE] = {.name = "bayer_rggb16le", BAYER16_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_BAYER_RGGB16BE] = {.name = "bayer_rggb16be", BAYER16_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_BAYER_GBRG8] = {.name = "bayer_gbrg8", BAYER8_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_BAYER_GBRG16LE] = {.name = "bayer_gbrg16le", BAYER16_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_BAYER_GBRG16BE] = {.name = "bayer_gbrg16be", BAYER16_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_BAYER_GRBG8] = {.name = "bayer_grbg8", BAYER8_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_BAYER_GRBG16LE] = {.name = "bayer_grbg16le", BAYER16_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_BAYER_GRBG16BE] = {.name = "bayer_grbg16be", BAYER16_DESC_COMMON .flags =
        AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_RGB |
        AV_PIX_FMT_FLAG_BAYER,}, [AV_PIX_FMT_NV16] = {.name = "nv16", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                           ,        /* Y */
                {                                                                                                                           1, 2, 0, 0, 8, 1, 7, 1}
                                                                                                                                           ,        /* U */
                {                                                                                                                           1, 2, 1, 0, 8, 1, 7, 2}
                                                                                                                                           ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_NV20LE] = {.name = "nv20le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                            ,        /* Y */
                {                                                                                                                                            1, 4, 0, 0, 10, 3, 9, 1}
                                                                                                                                                            ,        /* U */
                {                                                                                                                                            1, 4, 2, 0, 10, 3, 9, 3}
                                                                                                                                                            ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_NV20BE] = {.name = "nv20be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                            ,        /* Y */
                {                                                                                                                                            1, 4, 0, 0, 10, 3, 9, 1}
                                                                                                                                                            ,        /* U */
                {                                                                                                                                            1, 4, 2, 0, 10, 3, 9, 3}
                                                                                                                                                            ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_BE,}, [AV_PIX_FMT_QSV] = {.name = "qsv", .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_MEDIACODEC] = {.name = "mediacodec", .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_MMAL] = {.name = "mmal", .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_CUDA] = {.name = "cuda", .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_AYUV64LE] = {.name = "ayuv64le", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 8, 2, 0, 16, 7, 15, 3}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ,        /* Y */
                {                                                                                                                                                                                                                                                                                                                                                                                                                                                          0, 8, 4, 0, 16, 7, 15, 5}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ,        /* U */
                {                                                                                                                                                                                                                                                                                                                                                                                                                                                          0, 8, 6, 0, 16, 7, 15, 7}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ,        /* V */
                {                                                                                                                                                                                                                                                                                                                                                                                                                                                          0, 8, 0, 0, 16, 7, 15, 1}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_AYUV64BE] = {.name = "ayuv64be", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 8, 2, 0, 16, 7, 15, 3}
                                                                                                                                                               ,        /* Y */
                {                                                                                                                                               0, 8, 4, 0, 16, 7, 15, 5}
                                                                                                                                                               ,        /* U */
                {                                                                                                                                               0, 8, 6, 0, 16, 7, 15, 7}
                                                                                                                                                               ,        /* V */
                {                                                                                                                                               0, 8, 0, 0, 16, 7, 15, 1}
                                                                                                                                                               ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_P010LE] = {.name = "p010le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 6, 10, 1, 9, 1}
                                                                                                                                                           ,        /* Y */
                {                                                                                                                                           1, 4, 0, 6, 10, 3, 9, 1}
                                                                                                                                                           ,        /* U */
                {                                                                                                                                           1, 4, 2, 6, 10, 3, 9, 3}
                                                                                                                                                           ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_P010BE] = {.name = "p010be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 6, 10, 1, 9, 1}
                                                                                                                                                            ,        /* Y */
                {                                                                                                                                            1, 4, 0, 6, 10, 3, 9, 1}
                                                                                                                                                            ,        /* U */
                {                                                                                                                                            1, 4, 2, 6, 10, 3, 9, 3}
                                                                                                                                                            ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_BE,}, [AV_PIX_FMT_P016LE] = {.name = "p016le", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                        ,       /* Y */
                {                                                                                                                                        1, 4, 0, 0, 16, 3, 15, 1}
                                                                                                                                                        ,       /* U */
                {                                                                                                                                        1, 4, 2, 0, 16, 3, 15, 3}
                                                                                                                                                        ,       /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_P016BE] = {.name = "p016be", .nb_components = 3, .log2_chroma_w = 1, .log2_chroma_h = 1, .comp = {{0, 2, 0, 0, 16, 1, 15, 1}
                                                                                                                                                            ,       /* Y */
                {                                                                                                                                            1, 4, 0, 0, 16, 3, 15, 1}
                                                                                                                                                            ,       /* U */
                {                                                                                                                                            1, 4, 2, 0, 16, 3, 15, 3}
                                                                                                                                                            ,       /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_BE,}, [AV_PIX_FMT_GBRAP12LE] = {.name = "gbrap12le", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                              ,       /* R */
                {                                                                                                                                              0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                              ,       /* G */
                {                                                                                                                                              1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                              ,       /* B */
                {                                                                                                                                              3, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                              ,       /* A */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_GBRAP12BE] = {.name = "gbrap12be", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                 ,       /* R */
                {                                                                                                                                                 0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                 ,       /* G */
                {                                                                                                                                                 1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                 ,       /* B */
                {                                                                                                                                                 3, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                 ,       /* A */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_GBRAP10LE] = {.name = "gbrap10le", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                 ,       /* R */
                {                                                                                                                                                 0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                 ,       /* G */
                {                                                                                                                                                 1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                 ,       /* B */
                {                                                                                                                                                 3, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                 ,       /* A */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_GBRAP10BE] = {.name = "gbrap10be", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                 ,       /* R */
                {                                                                                                                                                 0, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                 ,       /* G */
                {                                                                                                                                                 1, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                 ,       /* B */
                {                                                                                                                                                 3, 2, 0, 0, 10, 1, 9, 1}
                                                                                                                                                                 ,       /* A */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_D3D11] = {.name = "d3d11", .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_GBRPF32BE] = {.name = "gbrpf32be", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                                                                                            ,        /* R */
                {                                                                                                                                                                                                                            0, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                                                                                            ,        /* G */
                {                                                                                                                                                                                                                            1, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                                                                                            ,        /* B */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_RGB |
                    AV_PIX_FMT_FLAG_FLOAT,}, [AV_PIX_FMT_GBRPF32LE] = {.name = "gbrpf32le", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                 ,        /* R */
                {                                                                                                                                                 0, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                 ,        /* G */
                {                                                                                                                                                 1, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                 ,        /* B */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_FLOAT |
                    AV_PIX_FMT_FLAG_RGB,}, [AV_PIX_FMT_GBRAPF32BE] = {.name = "gbrapf32be", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                 ,        /* R */
                {                                                                                                                                                 0, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                 ,        /* G */
                {                                                                                                                                                 1, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                 ,        /* B */
                {                                                                                                                                                 3, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                 ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA |
                    AV_PIX_FMT_FLAG_RGB |
                    AV_PIX_FMT_FLAG_FLOAT,}, [AV_PIX_FMT_GBRAPF32LE] = {.name = "gbrapf32le", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{2, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                   ,        /* R */
                {                                                                                                                                                   0, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                   ,        /* G */
                {                                                                                                                                                   1, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                   ,        /* B */
                {                                                                                                                                                   3, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                   ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR | AV_PIX_FMT_FLAG_ALPHA | AV_PIX_FMT_FLAG_RGB |
                    AV_PIX_FMT_FLAG_FLOAT,}, [AV_PIX_FMT_DRM_PRIME] = {.name = "drm_prime", .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_OPENCL] = {.name  = "opencl", .flags = AV_PIX_FMT_FLAG_HWACCEL,}, [AV_PIX_FMT_GRAYF32BE] = {.name = "grayf32be", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                                                                                                                                                                                  ,       /* Y */
        }, .flags = AV_PIX_FMT_FLAG_BE |
                    AV_PIX_FMT_FLAG_FLOAT, .alias = "yf32be",}, [AV_PIX_FMT_GRAYF32LE] = {.name = "grayf32le", .nb_components = 1, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 4, 0, 0, 32, 3, 31, 1}
                                                                                                                                                                                    ,       /* Y */
        }, .flags = AV_PIX_FMT_FLAG_FLOAT, .alias = "yf32le",}, [AV_PIX_FMT_YUVA422P12BE] = {.name = "yuva422p12be", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                                          ,        /* Y */
                {                                                                                                                                                                          1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                                          ,        /* U */
                {                                                                                                                                                                          2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                                          ,        /* V */
                {                                                                                                                                                                          3, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                                          ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA422P12LE] = {.name = "yuva422p12le", .nb_components = 4, .log2_chroma_w = 1, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* Y */
                {                                                                                                                                                       1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* U */
                {                                                                                                                                                       2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* V */
                {                                                                                                                                                       3, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA444P12BE] = {.name = "yuva444p12be", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* Y */
                {                                                                                                                                                       1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* U */
                {                                                                                                                                                       2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* V */
                {                                                                                                                                                       3, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_BE | AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_YUVA444P12LE] = {.name = "yuva444p12le", .nb_components = 4, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* Y */
                {                                                                                                                                                       1, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* U */
                {                                                                                                                                                       2, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* V */
                {                                                                                                                                                       3, 2, 0, 0, 12, 1, 11, 1}
                                                                                                                                                                       ,        /* A */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR |
                    AV_PIX_FMT_FLAG_ALPHA,}, [AV_PIX_FMT_NV24] = {.name = "nv24", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                       ,        /* Y */
                {                                                                                                                                       1, 2, 0, 0, 8, 1, 7, 1}
                                                                                                                                                       ,        /* U */
                {                                                                                                                                       1, 2, 1, 0, 8, 1, 7, 2}
                                                                                                                                                       ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,}, [AV_PIX_FMT_NV42] = {.name = "nv42", .nb_components = 3, .log2_chroma_w = 0, .log2_chroma_h = 0, .comp = {{0, 1, 0, 0, 8, 0, 7, 1}
                                                                                                                                                        ,        /* Y */
                {                                                                                                                                        1, 2, 1, 0, 8, 1, 7, 2}
                                                                                                                                                        ,        /* U */
                {                                                                                                                                        1, 2, 0, 0, 8, 1, 7, 1}
                                                                                                                                                        ,        /* V */
        }, .flags = AV_PIX_FMT_FLAG_PLANAR,},};


typedef struct AVCodecContext {
/**
     * information on struct for av_log
     * - set by avcodec_alloc_context3
     */
    const AVClass *av_class;
    int log_level_offset;

    enum AVMediaType codec_type; /* see AVMEDIA_TYPE_xxx */
    const struct AVCodec *codec;
    enum AVCodecID codec_id; /* see AV_CODEC_ID_xxx */

/**
     * fourcc (LSB first, so "ABCD" -> ('D'<<24) + ('C'<<16) + ('B'<<8) + 'A').
     * This is used to work around some encoder bugs.
     * A demuxer should set this to what is stored in the field used to identify the codec.
     * If there are multiple such fields in a container then the demuxer should choose the one
     * which maximizes the information about the used codec.
     * If the codec tag field in a container is larger than 32 bits then the demuxer should
     * remap the longer ID to 32 bits with a table or other structure. Alternatively a new
     * extra_codec_tag + size could be added but for this a clear advantage must be demonstrated
     * first.
     * - encoding: Set by user, if not then the default based on codec_id will be used.
     * - decoding: Set by user, will be converted to uppercase by libavcodec during init.
     */
    unsigned int codec_tag;

    void *priv_data;

/**
     * Private context used for internal data.
     *
     * Unlike priv_data, this is not codec-specific. It is used in general
     * libavcodec functions.
     */
    struct AVCodecInternal *internal;

/**
     * Private data of the user, can be used to carry app specific stuff.
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    void *opaque;

/**
     * the average bitrate
     * - encoding: Set by user; unused for constant quantizer encoding.
     * - decoding: Set by user, may be overwritten by libavcodec
     *             if this info is available in the stream
     */
    int64_t bit_rate;

/**
     * number of bits the bitstream is allowed to diverge from the reference.
     *           the reference can be CBR (for CBR pass1) or VBR (for pass2)
     * - encoding: Set by user; unused for constant quantizer encoding.
     * - decoding: unused
     */
    int bit_rate_tolerance;

/**
     * Global quality for codecs which cannot change it per frame.
     * This should be proportional to MPEG-1/2/4 qscale.
     * - encoding: Set by user.
     * - decoding: unused
     */
    int global_quality;

/**
     * - encoding: Set by user.
     * - decoding: unused
     */
    int compression_level;
#define FF_COMPRESSION_DEFAULT -1

/**
     * AV_CODEC_FLAG_*.
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int flags;

/**
     * AV_CODEC_FLAG2_*
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int flags2;

/**
     * some codecs need / can use extradata like Huffman tables.
     * MJPEG: Huffman tables
     * rv10: additional flags
     * MPEG-4: global headers (they can be in the bitstream or here)
     * The allocated memory should be AV_INPUT_BUFFER_PADDING_SIZE bytes larger
     * than extradata_size to avoid problems if it is read with the bitstream reader.
     * The bytewise contents of extradata must not depend on the architecture or CPU endianness.
     * Must be allocated with the av_malloc() family of functions.
     * - encoding: Set/allocated/freed by libavcodec.
     * - decoding: Set/allocated/freed by user.
     */
    uint8_t *extradata;
    int extradata_size;

/**
     * This is the fundamental unit of time (in seconds) in terms
     * of which frame timestamps are represented. For fixed-fps content,
     * timebase should be 1/framerate and timestamp increments should be
     * identically 1.
     * This often, but not always is the inverse of the frame rate or field rate
     * for video. 1/time_base is not the average frame rate if the frame rate is not
     * constant.
     *
     * Like containers, elementary streams also can store timestamps, 1/time_base
     * is the unit in which these timestamps are specified.
     * As example of such codec time base see ISO/IEC 14496-2:2001(E)
     * vop_time_increment_resolution and fixed_vop_rate
     * (fixed_vop_rate == 0 implies that it is different from the framerate)
     *
     * - encoding: MUST be set by user.
     * - decoding: the use of this field for decoding is deprecated.
     *             Use framerate instead.
     */
    AVRational time_base;

/**
     * For some codecs, the time base is closer to the field rate than the frame rate.
     * Most notably, H.264 and MPEG-2 specify time_base as half of frame duration
     * if no telecine is used ...
     *
     * Set to time_base ticks per frame. Default 1, e.g., H.264/MPEG-2 set it to 2.
     */
    int ticks_per_frame;

/**
     * Codec delay.
     *
     * Encoding: Number of frames delay there will be from the encoder input to
     *           the decoder output. (we assume the decoder matches the spec)
     * Decoding: Number of frames delay in addition to what a standard decoder
     *           as specified in the spec would produce.
     *
     * Video:
     *   Number of frames the decoded output will be delayed relative to the
     *   encoded input.
     *
     * Audio:
     *   For encoding, this field is unused (see initial_padding).
     *
     *   For decoding, this is the number of samples the decoder needs to
     *   output before the decoder's output is valid. When seeking, you should
     *   start decoding this many samples prior to your desired seek point.
     *
     * - encoding: Set by libavcodec.
     * - decoding: Set by libavcodec.
     */
    int delay;


/* video only */
/**
     * picture width / height.
     *
     * @note Those fields may not match the values of the last
     * AVFrame output by avcodec_decode_video2 due frame
     * reordering.
     *
     * - encoding: MUST be set by user.
     * - decoding: May be set by the user before opening the decoder if known e.g.
     *             from the container. Some decoders will require the dimensions
     *             to be set by the caller. During decoding, the decoder may
     *             overwrite those values as required while parsing the data.
     */
    int width, height;

/**
     * Bitstream width / height, may be different from width/height e.g. when
     * the decoded frame is cropped before being output or lowres is enabled.
     *
     * @note Those field may not match the value of the last
     * AVFrame output by avcodec_receive_frame() due frame
     * reordering.
     *
     * - encoding: unused
     * - decoding: May be set by the user before opening the decoder if known
     *             e.g. from the container. During decoding, the decoder may
     *             overwrite those values as required while parsing the data.
     */
    int coded_width, coded_height;

/**
     * the number of pictures in a group of pictures, or 0 for intra_only
     * - encoding: Set by user.
     * - decoding: unused
     */
    int gop_size;

/**
     * Pixel format, see AV_PIX_FMT_xxx.
     * May be set by the demuxer if known from headers.
     * May be overridden by the decoder if it knows better.
     *
     * @note This field may not match the value of the last
     * AVFrame output by avcodec_receive_frame() due frame
     * reordering.
     *
     * - encoding: Set by user.
     * - decoding: Set by user if known, overridden by libavcodec while
     *             parsing the data.
     */
    enum AVPixelFormat pix_fmt;

/**
     * If non NULL, 'draw_horiz_band' is called by the libavcodec
     * decoder to draw a horizontal band. It improves cache usage. Not
     * all codecs can do that. You must check the codec capabilities
     * beforehand.
     * When multithreading is used, it may be called from multiple threads
     * at the same time; threads might draw different parts of the same AVFrame,
     * or multiple AVFrames, and there is no guarantee that slices will be drawn
     * in order.
     * The function is also used by hardware acceleration APIs.
     * It is called at least once during frame decoding to pass
     * the data needed for hardware render.
     * In that mode instead of pixel data, AVFrame points to
     * a structure specific to the acceleration API. The application
     * reads the structure and can change some fields to indicate progress
     * or mark state.
     * - encoding: unused
     * - decoding: Set by user.
     * @param height the height of the slice
     * @param y the y position of the slice
     * @param type 1->top field, 2->bottom field, 3->frame
     * @param offset offset into the AVFrame.data from which the slice should be read
     */
    void (*draw_horiz_band)(struct AVCodecContext *s, const AVFrame *src, int offset[AV_NUM_DATA_POINTERS], int y, int type, int height);

/**
     * callback to negotiate the pixelFormat
     * @param fmt is the list of formats which are supported by the codec,
     * it is terminated by -1 as 0 is a valid format, the formats are ordered by quality.
     * The first is always the native one.
     * @note The callback may be called again immediately if initialization for
     * the selected (hardware-accelerated) pixel format failed.
     * @warning Behavior is undefined if the callback returns a value not
     * in the fmt list of formats.
     * @return the chosen format
     * - encoding: unused
     * - decoding: Set by user, if not set the native format will be chosen.
     */
    enum AVPixelFormat (*get_format)(struct AVCodecContext *s, const enum AVPixelFormat *fmt);

/**
     * maximum number of B-frames between non-B-frames
     * Note: The output will be delayed by max_b_frames+1 relative to the input.
     * - encoding: Set by user.
     * - decoding: unused
     */
    int max_b_frames;

/**
     * qscale factor between IP and B-frames
     * If > 0 then the last P-frame quantizer will be used (q= lastp_q*factor+offset).
     * If < 0 then normal ratecontrol will be done (q= -normal_q*factor+offset).
     * - encoding: Set by user.
     * - decoding: unused
     */
    float b_quant_factor;

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int b_frame_strategy;
#endif

/**
     * qscale offset between IP and B-frames
     * - encoding: Set by user.
     * - decoding: unused
     */
    float b_quant_offset;

/**
     * Size of the frame reordering buffer in the decoder.
     * For MPEG-2 it is 1 IPB or 0 low delay IP.
     * - encoding: Set by libavcodec.
     * - decoding: Set by libavcodec.
     */
    int has_b_frames;

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int mpeg_quant;
#endif

/**
     * qscale factor between P- and I-frames
     * If > 0 then the last P-frame quantizer will be used (q = lastp_q * factor + offset).
     * If < 0 then normal ratecontrol will be done (q= -normal_q*factor+offset).
     * - encoding: Set by user.
     * - decoding: unused
     */
    float i_quant_factor;

/**
     * qscale offset between P and I-frames
     * - encoding: Set by user.
     * - decoding: unused
     */
    float i_quant_offset;

/**
     * luminance masking (0-> disabled)
     * - encoding: Set by user.
     * - decoding: unused
     */
    float lumi_masking;

/**
     * temporary complexity masking (0-> disabled)
     * - encoding: Set by user.
     * - decoding: unused
     */
    float temporal_cplx_masking;

/**
     * spatial complexity masking (0-> disabled)
     * - encoding: Set by user.
     * - decoding: unused
     */
    float spatial_cplx_masking;

/**
     * p block masking (0-> disabled)
     * - encoding: Set by user.
     * - decoding: unused
     */
    float p_masking;

/**
     * darkness masking (0-> disabled)
     * - encoding: Set by user.
     * - decoding: unused
     */
    float dark_masking;

/**
     * slice count
     * - encoding: Set by libavcodec.
     * - decoding: Set by user (or 0).
     */
    int slice_count;

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int prediction_method;
#define FF_PRED_LEFT   0
#define FF_PRED_PLANE  1
#define FF_PRED_MEDIAN 2
#endif

/**
     * slice offsets in the frame in bytes
     * - encoding: Set/allocated by libavcodec.
     * - decoding: Set/allocated by user (or NULL).
     */
    int *slice_offset;

/**
     * sample aspect ratio (0 if unknown)
     * That is the width of a pixel divided by the height of the pixel.
     * Numerator and denominator must be relatively prime and smaller than 256 for some video standards.
     * - encoding: Set by user.
     * - decoding: Set by libavcodec.
     */
    AVRational sample_aspect_ratio;

/**
     * motion estimation comparison function
     * - encoding: Set by user.
     * - decoding: unused
     */
    int me_cmp;
/**
     * subpixel motion estimation comparison function
     * - encoding: Set by user.
     * - decoding: unused
     */
    int me_sub_cmp;
/**
     * macroblock comparison function (not supported yet)
     * - encoding: Set by user.
     * - decoding: unused
     */
    int mb_cmp;
/**
     * interlaced DCT comparison function
     * - encoding: Set by user.
     * - decoding: unused
     */
    int ildct_cmp;
#define FF_CMP_SAD          0
#define FF_CMP_SSE          1
#define FF_CMP_SATD         2
#define FF_CMP_DCT          3
#define FF_CMP_PSNR         4
#define FF_CMP_BIT          5
#define FF_CMP_RD           6
#define FF_CMP_ZERO         7
#define FF_CMP_VSAD         8
#define FF_CMP_VSSE         9
#define FF_CMP_NSSE         10
#define FF_CMP_W53          11
#define FF_CMP_W97          12
#define FF_CMP_DCTMAX       13
#define FF_CMP_DCT264       14
#define FF_CMP_MEDIAN_SAD   15
#define FF_CMP_CHROMA       256

/**
     * ME diamond size & shape
     * - encoding: Set by user.
     * - decoding: unused
     */
    int dia_size;

/**
     * amount of previous MV predictors (2a+1 x 2a+1 square)
     * - encoding: Set by user.
     * - decoding: unused
     */
    int last_predictor_count;

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int pre_me;
#endif

/**
     * motion estimation prepass comparison function
     * - encoding: Set by user.
     * - decoding: unused
     */
    int me_pre_cmp;

/**
     * ME prepass diamond size & shape
     * - encoding: Set by user.
     * - decoding: unused
     */
    int pre_dia_size;

/**
     * subpel ME quality
     * - encoding: Set by user.
     * - decoding: unused
     */
    int me_subpel_quality;

/**
     * maximum motion estimation search range in subpel units
     * If 0 then no limit.
     *
     * - encoding: Set by user.
     * - decoding: unused
     */
    int me_range;

/**
     * slice flags
     * - encoding: unused
     * - decoding: Set by user.
     */
    int slice_flags;
#define SLICE_FLAG_CODED_ORDER    0x0001 ///< draw_horiz_band() is called in coded order instead of display
#define SLICE_FLAG_ALLOW_FIELD    0x0002 ///< allow draw_horiz_band() with field slices (MPEG-2 field pics)
#define SLICE_FLAG_ALLOW_PLANE    0x0004 ///< allow draw_horiz_band() with 1 component at a time (SVQ1)

/**
     * macroblock decision mode
     * - encoding: Set by user.
     * - decoding: unused
     */
    int mb_decision;
#define FF_MB_DECISION_SIMPLE 0        ///< uses mb_cmp
#define FF_MB_DECISION_BITS   1        ///< chooses the one which needs the fewest bits
#define FF_MB_DECISION_RD     2        ///< rate distortion

/**
     * custom intra quantization matrix
     * Must be allocated with the av_malloc() family of functions, and will be freed in
     * avcodec_free_context().
     * - encoding: Set/allocated by user, freed by libavcodec. Can be NULL.
     * - decoding: Set/allocated/freed by libavcodec.
     */
    uint16_t *intra_matrix;

/**
     * custom inter quantization matrix
     * Must be allocated with the av_malloc() family of functions, and will be freed in
     * avcodec_free_context().
     * - encoding: Set/allocated by user, freed by libavcodec. Can be NULL.
     * - decoding: Set/allocated/freed by libavcodec.
     */
    uint16_t *inter_matrix;

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int scenechange_threshold;

/** @deprecated use encoder private options instead */
attribute_deprecated
int noise_reduction;
#endif

/**
     * precision of the intra DC coefficient - 8
     * - encoding: Set by user.
     * - decoding: Set by libavcodec
     */
    int intra_dc_precision;

/**
     * Number of macroblock rows at the top which are skipped.
     * - encoding: unused
     * - decoding: Set by user.
     */
    int skip_top;

/**
     * Number of macroblock rows at the bottom which are skipped.
     * - encoding: unused
     * - decoding: Set by user.
     */
    int skip_bottom;

/**
     * minimum MB Lagrange multiplier
     * - encoding: Set by user.
     * - decoding: unused
     */
    int mb_lmin;

/**
     * maximum MB Lagrange multiplier
     * - encoding: Set by user.
     * - decoding: unused
     */
    int mb_lmax;

#if FF_API_PRIVATE_OPT
                                                                                                                            /**
     * @deprecated use encoder private options instead
     */
attribute_deprecated
int me_penalty_compensation;
#endif

/**
     * - encoding: Set by user.
     * - decoding: unused
     */
    int bidir_refine;

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int brd_scale;
#endif

/**
     * minimum GOP size
     * - encoding: Set by user.
     * - decoding: unused
     */
    int keyint_min;

/**
     * number of reference frames
     * - encoding: Set by user.
     * - decoding: Set by lavc.
     */
    int refs;

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int chromaoffset;
#endif

/**
     * Note: Value depends upon the compare function used for fullpel ME.
     * - encoding: Set by user.
     * - decoding: unused
     */
    int mv0_threshold;

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int b_sensitivity;
#endif

/**
     * Chromaticity coordinates of the source primaries.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    enum AVColorPrimaries color_primaries;

/**
     * Color Transfer Characteristic.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    enum AVColorTransferCharacteristic color_trc;

/**
     * YUV colorspace type.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    enum AVColorSpace colorspace;

/**
     * MPEG vs JPEG YUV range.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    enum AVColorRange color_range;

/**
     * This defines the location of chroma samples.
     * - encoding: Set by user
     * - decoding: Set by libavcodec
     */
    enum AVChromaLocation chroma_sample_location;

/**
     * Number of slices.
     * Indicates number of picture subdivisions. Used for parallelized
     * decoding.
     * - encoding: Set by user
     * - decoding: unused
     */
    int slices;

/** Field order
     * - encoding: set by libavcodec
     * - decoding: Set by user.
     */
    enum AVFieldOrder field_order;

/* audio only */
    int sample_rate; ///< samples per second
    int channels;    ///< number of audio channels

/**
     * audio sample format
     * - encoding: Set by user.
     * - decoding: Set by libavcodec.
     */
    enum AVSampleFormat sample_fmt;  ///< sample format

/* The following data should not be initialized. */
/**
     * Number of samples per channel in an audio frame.
     *
     * - encoding: set by libavcodec in avcodec_open2(). Each submitted frame
     *   except the last must contain exactly frame_size samples per channel.
     *   May be 0 when the codec has AV_CODEC_CAP_VARIABLE_FRAME_SIZE set, then the
     *   frame size is not restricted.
     * - decoding: may be set by some decoders to indicate constant frame size
     */
    int frame_size;

/**
     * Frame counter, set by libavcodec.
     *
     * - decoding: total number of frames returned from the decoder so far.
     * - encoding: total number of frames passed to the encoder so far.
     *
     *   @note the counter is not incremented if encoding/decoding resulted in
     *   an error.
     */
    int frame_number;

/**
     * number of bytes per packet if constant and known or 0
     * Used by some WAV based audio codecs.
     */
    int block_align;

/**
     * Audio cutoff bandwidth (0 means "automatic")
     * - encoding: Set by user.
     * - decoding: unused
     */
    int cutoff;

/**
     * Audio channel layout.
     * - encoding: set by user.
     * - decoding: set by user, may be overwritten by libavcodec.
     */
    uint64_t channel_layout;

/**
     * Request decoder to use this channel layout if it can (0 for default)
     * - encoding: unused
     * - decoding: Set by user.
     */
    uint64_t request_channel_layout;

/**
     * Type of service that the audio stream conveys.
     * - encoding: Set by user.
     * - decoding: Set by libavcodec.
     */
    enum AVAudioServiceType audio_service_type;

/**
     * desired sample format
     * - encoding: Not used.
     * - decoding: Set by user.
     * Decoder will decode to this format if it can.
     */
    enum AVSampleFormat request_sample_fmt;

/**
     * This callback is called at the beginning of each frame to get data
     * buffer(s) for it. There may be one contiguous buffer for all the data or
     * there may be a buffer per each data plane or anything in between. What
     * this means is, you may set however many entries in buf[] you feel necessary.
     * Each buffer must be reference-counted using the AVBuffer API (see description
     * of buf[] below).
     *
     * The following fields will be set in the frame before this callback is
     * called:
     * - format
     * - width, height (video only)
     * - sample_rate, channel_layout, nb_samples (audio only)
     * Their values may differ from the corresponding values in
     * AVCodecContext. This callback must use the frame values, not the codec
     * context values, to calculate the required buffer size.
     *
     * This callback must fill the following fields in the frame:
     * - data[]
     * - linesize[]
     * - extended_data:
     *   * if the data is planar audio with more than 8 channels, then this
     *     callback must allocate and fill extended_data to contain all pointers
     *     to all data planes. data[] must hold as many pointers as it can.
     *     extended_data must be allocated with av_malloc() and will be freed in
     *     av_frame_unref().
     *   * otherwise extended_data must point to data
     * - buf[] must contain one or more pointers to AVBufferRef structures. Each of
     *   the frame's data and extended_data pointers must be contained in these. That
     *   is, one AVBufferRef for each allocated chunk of memory, not necessarily one
     *   AVBufferRef per data[] entry. See: av_buffer_create(), av_buffer_alloc(),
     *   and av_buffer_ref().
     * - extended_buf and nb_extended_buf must be allocated with av_malloc() by
     *   this callback and filled with the extra buffers if there are more
     *   buffers than buf[] can hold. extended_buf will be freed in
     *   av_frame_unref().
     *
     * If AV_CODEC_CAP_DR1 is not set then get_buffer2() must call
     * avcodec_default_get_buffer2() instead of providing buffers allocated by
     * some other means.
     *
     * Each data plane must be aligned to the maximum required by the target
     * CPU.
     *
     * @see avcodec_default_get_buffer2()
     *
     * Video:
     *
     * If AV_GET_BUFFER_FLAG_REF is set in flags then the frame may be reused
     * (read and/or written to if it is writable) later by libavcodec.
     *
     * avcodec_align_dimensions2() should be used to find the required width and
     * height, as they normally need to be rounded up to the next multiple of 16.
     *
     * Some decoders do not support linesizes changing between frames.
     *
     * If frame multithreading is used and thread_safe_callbacks is set,
     * this callback may be called from a different thread, but not from more
     * than one at once. Does not need to be reentrant.
     *
     * @see avcodec_align_dimensions2()
     *
     * Audio:
     *
     * Decoders request a buffer of a particular size by setting
     * AVFrame.nb_samples prior to calling get_buffer2(). The decoder may,
     * however, utilize only part of the buffer by setting AVFrame.nb_samples
     * to a smaller value in the output frame.
     *
     * As a convenience, av_samples_get_buffer_size() and
     * av_samples_fill_arrays() in libavutil may be used by custom get_buffer2()
     * functions to find the required data size and to fill data pointers and
     * linesize. In AVFrame.linesize, only linesize[0] may be set for audio
     * since all planes must be the same size.
     *
     * @see av_samples_get_buffer_size(), av_samples_fill_arrays()
     *
     * - encoding: unused
     * - decoding: Set by libavcodec, user can override.
     */
    int (*get_buffer2)(struct AVCodecContext *s, AVFrame *frame, int flags);

/**
     * If non-zero, the decoded audio and video frames returned from
     * avcodec_decode_video2() and avcodec_decode_audio4() are reference-counted
     * and are valid indefinitely. The caller must free them with
     * av_frame_unref() when they are not needed anymore.
     * Otherwise, the decoded frames must not be freed by the caller and are
     * only valid until the next decode call.
     *
     * This is always automatically enabled if avcodec_receive_frame() is used.
     *
     * - encoding: unused
     * - decoding: set by the caller before avcodec_open2().
     */
    attribute_deprecated
    int refcounted_frames;

/* - encoding parameters */
    float qcompress;  ///< amount of qscale change between easy & hard scenes (0.0-1.0)
    float qblur;      ///< amount of qscale smoothing over time (0.0-1.0)

/**
     * minimum quantizer
     * - encoding: Set by user.
     * - decoding: unused
     */
    int qmin;

/**
     * maximum quantizer
     * - encoding: Set by user.
     * - decoding: unused
     */
    int qmax;

/**
     * maximum quantizer difference between frames
     * - encoding: Set by user.
     * - decoding: unused
     */
    int max_qdiff;

/**
     * decoder bitstream buffer size
     * - encoding: Set by user.
     * - decoding: unused
     */
    int rc_buffer_size;

/**
     * ratecontrol override, see RcOverride
     * - encoding: Allocated/set/freed by user.
     * - decoding: unused
     */
    int rc_override_count;
    RcOverride *rc_override;

/**
     * maximum bitrate
     * - encoding: Set by user.
     * - decoding: Set by user, may be overwritten by libavcodec.
     */
    int64_t rc_max_rate;

/**
     * minimum bitrate
     * - encoding: Set by user.
     * - decoding: unused
     */
    int64_t rc_min_rate;

/**
     * Ratecontrol attempt to use, at maximum, <value> of what can be used without an underflow.
     * - encoding: Set by user.
     * - decoding: unused.
     */
    float rc_max_available_vbv_use;

/**
     * Ratecontrol attempt to use, at least, <value> times the amount needed to prevent a vbv overflow.
     * - encoding: Set by user.
     * - decoding: unused.
     */
    float rc_min_vbv_overflow_use;

/**
     * Number of bits which should be loaded into the rc buffer before decoding starts.
     * - encoding: Set by user.
     * - decoding: unused
     */
    int rc_initial_buffer_occupancy;

#if FF_API_CODER_TYPE
                                                                                                                            #define FF_CODER_TYPE_VLC       0
#define FF_CODER_TYPE_AC        1
#define FF_CODER_TYPE_RAW       2
#define FF_CODER_TYPE_RLE       3
/**
     * @deprecated use encoder private options instead
     */
attribute_deprecated
int coder_type;
#endif /* FF_API_CODER_TYPE */

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int context_model;
#endif

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int frame_skip_threshold;

/** @deprecated use encoder private options instead */
attribute_deprecated
int frame_skip_factor;

/** @deprecated use encoder private options instead */
attribute_deprecated
int frame_skip_exp;

/** @deprecated use encoder private options instead */
attribute_deprecated
int frame_skip_cmp;
#endif /* FF_API_PRIVATE_OPT */

/**
     * trellis RD quantization
     * - encoding: Set by user.
     * - decoding: unused
     */
    int trellis;

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int min_prediction_order;

/** @deprecated use encoder private options instead */
attribute_deprecated
int max_prediction_order;

/** @deprecated use encoder private options instead */
attribute_deprecated
int64_t timecode_frame_start;
#endif

#if FF_API_RTP_CALLBACK
                                                                                                                            /**
     * @deprecated unused
     */
/* The RTP callback: This function is called    */
/* every time the encoder has a packet to send. */
/* It depends on the encoder if the data starts */
/* with a Start Code (it should). H.263 does.   */
/* mb_nb contains the number of macroblocks     */
/* encoded in the RTP payload.                  */
attribute_deprecated
void (*rtp_callback)(struct AVCodecContext *avctx, void *data, int size, int mb_nb);
#endif

#if FF_API_PRIVATE_OPT
                                                                                                                            /** @deprecated use encoder private options instead */
attribute_deprecated
int rtp_payload_size;   /* The size of the RTP payload: the coder will  */
/* do its best to deliver a chunk with size     */
/* below rtp_payload_size, the chunk will start */
/* with a start code on some codecs like H.263. */
/* This doesn't take account of any particular  */
/* headers inside the transmitted RTP payload.  */
#endif

#if FF_API_STAT_BITS
                                                                                                                            /* statistics, used for 2-pass encoding */
attribute_deprecated
int mv_bits;
attribute_deprecated
int header_bits;
attribute_deprecated
int i_tex_bits;
attribute_deprecated
int p_tex_bits;
attribute_deprecated
int i_count;
attribute_deprecated
int p_count;
attribute_deprecated
int skip_count;
attribute_deprecated
int misc_bits;

/** @deprecated this field is unused */
attribute_deprecated
int frame_bits;
#endif

/**
     * pass1 encoding statistics output buffer
     * - encoding: Set by libavcodec.
     * - decoding: unused
     */
    char *stats_out;

/**
     * pass2 encoding statistics input buffer
     * Concatenated stuff from stats_out of pass1 should be placed here.
     * - encoding: Allocated/set/freed by user.
     * - decoding: unused
     */
    char *stats_in;

/**
     * Work around bugs in encoders which sometimes cannot be detected automatically.
     * - encoding: Set by user
     * - decoding: Set by user
     */
    int workaround_bugs;
#define FF_BUG_AUTODETECT       1  ///< autodetection
#define FF_BUG_XVID_ILACE       4
#define FF_BUG_UMP4             8
#define FF_BUG_NO_PADDING       16
#define FF_BUG_AMV              32
#define FF_BUG_QPEL_CHROMA      64
#define FF_BUG_STD_QPEL         128
#define FF_BUG_QPEL_CHROMA2     256
#define FF_BUG_DIRECT_BLOCKSIZE 512
#define FF_BUG_EDGE             1024
#define FF_BUG_HPEL_CHROMA      2048
#define FF_BUG_DC_CLIP          4096
#define FF_BUG_MS               8192 ///< Work around various bugs in Microsoft's broken decoders.
#define FF_BUG_TRUNCATED       16384
#define FF_BUG_IEDGE           32768

/**
     * strictly follow the standard (MPEG-4, ...).
     * - encoding: Set by user.
     * - decoding: Set by user.
     * Setting this to STRICT or higher means the encoder and decoder will
     * generally do stupid things, whereas setting it to unofficial or lower
     * will mean the encoder might produce output that is not supported by all
     * spec-compliant decoders. Decoders don't differentiate between normal,
     * unofficial and experimental (that is, they always try to decode things
     * when they can) unless they are explicitly asked to behave stupidly
     * (=strictly conform to the specs)
     */
    int strict_std_compliance;
#define FF_COMPLIANCE_VERY_STRICT   2 ///< Strictly conform to an older more strict version of the spec or reference software.
#define FF_COMPLIANCE_STRICT        1 ///< Strictly conform to all the things in the spec no matter what consequences.
#define FF_COMPLIANCE_NORMAL        0
#define FF_COMPLIANCE_UNOFFICIAL   -1 ///< Allow unofficial extensions
#define FF_COMPLIANCE_EXPERIMENTAL -2 ///< Allow nonstandardized experimental things.

/**
     * error concealment flags
     * - encoding: unused
     * - decoding: Set by user.
     */
    int error_concealment;
#define FF_EC_GUESS_MVS   1
#define FF_EC_DEBLOCK     2
#define FF_EC_FAVOR_INTER 256

/**
     * debug
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int debug;
#define FF_DEBUG_PICT_INFO   1
#define FF_DEBUG_RC          2
#define FF_DEBUG_BITSTREAM   4
#define FF_DEBUG_MB_TYPE     8
#define FF_DEBUG_QP          16
#if FF_API_DEBUG_MV
                                                                                                                            /**
 * @deprecated this option does nothing
 */
#define FF_DEBUG_MV          32
#endif
#define FF_DEBUG_DCT_COEFF   0x00000040
#define FF_DEBUG_SKIP        0x00000080
#define FF_DEBUG_STARTCODE   0x00000100
#define FF_DEBUG_ER          0x00000400
#define FF_DEBUG_MMCO        0x00000800
#define FF_DEBUG_BUGS        0x00001000
#if FF_API_DEBUG_MV
                                                                                                                            #define FF_DEBUG_VIS_QP      0x00002000
#define FF_DEBUG_VIS_MB_TYPE 0x00004000
#endif
#define FF_DEBUG_BUFFERS     0x00008000
#define FF_DEBUG_THREADS     0x00010000
#define FF_DEBUG_GREEN_MD    0x00800000
#define FF_DEBUG_NOMC        0x01000000

#if FF_API_DEBUG_MV
                                                                                                                            /**
     * debug
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
int debug_mv;
#define FF_DEBUG_VIS_MV_P_FOR  0x00000001 // visualize forward predicted MVs of P-frames
#define FF_DEBUG_VIS_MV_B_FOR  0x00000002 // visualize forward predicted MVs of B-frames
#define FF_DEBUG_VIS_MV_B_BACK 0x00000004 // visualize backward predicted MVs of B-frames
#endif

/**
     * Error recognition; may misdetect some more or less valid parts as errors.
     * - encoding: unused
     * - decoding: Set by user.
     */
    int err_recognition;

/**
 * Verify checksums embedded in the bitstream (could be of either encoded or
 * decoded data, depending on the codec) and print an error message on mismatch.
 * If AV_EF_EXPLODE is also set, a mismatching checksum will result in the
 * decoder returning an error.
 */
#define AV_EF_CRCCHECK  (1<<0)
#define AV_EF_BITSTREAM (1<<1)          ///< detect bitstream specification deviations
#define AV_EF_BUFFER    (1<<2)          ///< detect improper bitstream length
#define AV_EF_EXPLODE   (1<<3)          ///< abort decoding on minor error detection

#define AV_EF_IGNORE_ERR (1<<15)        ///< ignore errors and continue
#define AV_EF_CAREFUL    (1<<16)        ///< consider things that violate the spec, are fast to calculate and have not been seen in the wild as errors
#define AV_EF_COMPLIANT  (1<<17)        ///< consider all spec non compliances as errors
#define AV_EF_AGGRESSIVE (1<<18)        ///< consider things that a sane encoder should not do as an error


/**
     * opaque 64-bit number (generally a PTS) that will be reordered and
     * output in AVFrame.reordered_opaque
     * - encoding: Set by libavcodec to the reordered_opaque of the input
     *             frame corresponding to the last returned packet. Only
     *             supported by encoders with the
     *             AV_CODEC_CAP_ENCODER_REORDERED_OPAQUE capability.
     * - decoding: Set by user.
     */
    int64_t reordered_opaque;

/**
     * Hardware accelerator in use
     * - encoding: unused.
     * - decoding: Set by libavcodec
     */
    const struct AVHWAccel *hwaccel;

/**
     * Hardware accelerator context.
     * For some hardware accelerators, a global context needs to be
     * provided by the user. In that case, this holds display-dependent
     * data FFmpeg cannot instantiate itself. Please refer to the
     * FFmpeg HW accelerator documentation to know how to fill this
     * is. e.g. for VA API, this is a struct vaapi_context.
     * - encoding: unused
     * - decoding: Set by user
     */
    void *hwaccel_context;

/**
     * error
     * - encoding: Set by libavcodec if flags & AV_CODEC_FLAG_PSNR.
     * - decoding: unused
     */
    uint64_t error[AV_NUM_DATA_POINTERS];

/**
     * DCT algorithm, see FF_DCT_* below
     * - encoding: Set by user.
     * - decoding: unused
     */
    int dct_algo;
#define FF_DCT_AUTO    0
#define FF_DCT_FASTINT 1
#define FF_DCT_INT     2
#define FF_DCT_MMX     3
#define FF_DCT_ALTIVEC 5
#define FF_DCT_FAAN    6

/**
     * IDCT algorithm, see FF_IDCT_* below.
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int idct_algo;
#define FF_IDCT_AUTO          0
#define FF_IDCT_INT           1
#define FF_IDCT_SIMPLE        2
#define FF_IDCT_SIMPLEMMX     3
#define FF_IDCT_ARM           7
#define FF_IDCT_ALTIVEC       8
#define FF_IDCT_SIMPLEARM     10
#define FF_IDCT_XVID          14
#define FF_IDCT_SIMPLEARMV5TE 16
#define FF_IDCT_SIMPLEARMV6   17
#define FF_IDCT_FAAN          20
#define FF_IDCT_SIMPLENEON    22
#define FF_IDCT_NONE          24 /* Used by XvMC to extract IDCT coefficients with FF_IDCT_PERM_NONE */
#define FF_IDCT_SIMPLEAUTO    128

/**
     * bits per sample/pixel from the demuxer (needed for huffyuv).
     * - encoding: Set by libavcodec.
     * - decoding: Set by user.
     */
    int bits_per_coded_sample;

/**
     * Bits per sample/pixel of internal libavcodec pixel/sample format.
     * - encoding: set by user.
     * - decoding: set by libavcodec.
     */
    int bits_per_raw_sample;

#if FF_API_LOWRES
                                                                                                                            /**
     * low resolution decoding, 1-> 1/2 size, 2->1/4 size
     * - encoding: unused
     * - decoding: Set by user.
     */
int lowres;
#endif

#if FF_API_CODED_FRAME
                                                                                                                            /**
     * the picture in the bitstream
     * - encoding: Set by libavcodec.
     * - decoding: unused
     *
     * @deprecated use the quality factor packet side data instead
     */
attribute_deprecated AVFrame *coded_frame;
#endif

/**
     * thread count
     * is used to decide how many independent tasks should be passed to execute()
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int thread_count;

/**
     * Which multithreading methods to use.
     * Use of FF_THREAD_FRAME will increase decoding delay by one frame per thread,
     * so clients which cannot provide future frames should not use it.
     *
     * - encoding: Set by user, otherwise the default is used.
     * - decoding: Set by user, otherwise the default is used.
     */
    int thread_type;
#define FF_THREAD_FRAME   1 ///< Decode more than one frame at once
#define FF_THREAD_SLICE   2 ///< Decode more than one part of a single frame at once

/**
     * Which multithreading methods are in use by the codec.
     * - encoding: Set by libavcodec.
     * - decoding: Set by libavcodec.
     */
    int active_thread_type;

/**
     * Set by the client if its custom get_buffer() callback can be called
     * synchronously from another thread, which allows faster multithreaded decoding.
     * draw_horiz_band() will be called from other threads regardless of this setting.
     * Ignored if the default get_buffer() is used.
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int thread_safe_callbacks;

/**
     * The codec may call this to execute several independent things.
     * It will return only after finishing all tasks.
     * The user may replace this with some multithreaded implementation,
     * the default implementation will execute the parts serially.
     * @param count the number of things to execute
     * - encoding: Set by libavcodec, user can override.
     * - decoding: Set by libavcodec, user can override.
     */
    int (*execute)(struct AVCodecContext *c, int (*func)(struct AVCodecContext *c2, void *arg), void *arg2, int *ret, int count, int size);

/**
     * The codec may call this to execute several independent things.
     * It will return only after finishing all tasks.
     * The user may replace this with some multithreaded implementation,
     * the default implementation will execute the parts serially.
     * Also see avcodec_thread_init and e.g. the --enable-pthread configure option.
     * @param c context passed also to func
     * @param count the number of things to execute
     * @param arg2 argument passed unchanged to func
     * @param ret return values of executed functions, must have space for "count" values. May be NULL.
     * @param func function that will be called count times, with jobnr from 0 to count-1.
     *             threadnr will be in the range 0 to c->thread_count-1 < MAX_THREADS and so that no
     *             two instances of func executing at the same time will have the same threadnr.
     * @return always 0 currently, but code should handle a future improvement where when any call to func
     *         returns < 0 no further calls to func may be done and < 0 is returned.
     * - encoding: Set by libavcodec, user can override.
     * - decoding: Set by libavcodec, user can override.
     */
    int (*execute2)(struct AVCodecContext *c, int (*func)(struct AVCodecContext *c2, void *arg, int jobnr, int threadnr), void *arg2, int *ret, int count);

/**
     * noise vs. sse weight for the nsse comparison function
     * - encoding: Set by user.
     * - decoding: unused
     */
    int nsse_weight;

/**
     * profile
     * - encoding: Set by user.
     * - decoding: Set by libavcodec.
     */
    int profile;
#define FF_PROFILE_UNKNOWN -99
#define FF_PROFILE_RESERVED -100

#define FF_PROFILE_AAC_MAIN 0
#define FF_PROFILE_AAC_LOW  1
#define FF_PROFILE_AAC_SSR  2
#define FF_PROFILE_AAC_LTP  3
#define FF_PROFILE_AAC_HE   4
#define FF_PROFILE_AAC_HE_V2 28
#define FF_PROFILE_AAC_LD   22
#define FF_PROFILE_AAC_ELD  38
#define FF_PROFILE_MPEG2_AAC_LOW 128
#define FF_PROFILE_MPEG2_AAC_HE  131

#define FF_PROFILE_DNXHD         0
#define FF_PROFILE_DNXHR_LB      1
#define FF_PROFILE_DNXHR_SQ      2
#define FF_PROFILE_DNXHR_HQ      3
#define FF_PROFILE_DNXHR_HQX     4
#define FF_PROFILE_DNXHR_444     5

#define FF_PROFILE_DTS         20
#define FF_PROFILE_DTS_ES      30
#define FF_PROFILE_DTS_96_24   40
#define FF_PROFILE_DTS_HD_HRA  50
#define FF_PROFILE_DTS_HD_MA   60
#define FF_PROFILE_DTS_EXPRESS 70

#define FF_PROFILE_MPEG2_422    0
#define FF_PROFILE_MPEG2_HIGH   1
#define FF_PROFILE_MPEG2_SS     2
#define FF_PROFILE_MPEG2_SNR_SCALABLE  3
#define FF_PROFILE_MPEG2_MAIN   4
#define FF_PROFILE_MPEG2_SIMPLE 5

#define FF_PROFILE_H264_CONSTRAINED  (1<<9)  // 8+1; constraint_set1_flag
#define FF_PROFILE_H264_INTRA        (1<<11) // 8+3; constraint_set3_flag

#define FF_PROFILE_H264_BASELINE             66
#define FF_PROFILE_H264_CONSTRAINED_BASELINE (66|FF_PROFILE_H264_CONSTRAINED)
#define FF_PROFILE_H264_MAIN                 77
#define FF_PROFILE_H264_EXTENDED             88
#define FF_PROFILE_H264_HIGH                 100
#define FF_PROFILE_H264_HIGH_10              110
#define FF_PROFILE_H264_HIGH_10_INTRA        (110|FF_PROFILE_H264_INTRA)
#define FF_PROFILE_H264_MULTIVIEW_HIGH       118
#define FF_PROFILE_H264_HIGH_422             122
#define FF_PROFILE_H264_HIGH_422_INTRA       (122|FF_PROFILE_H264_INTRA)
#define FF_PROFILE_H264_STEREO_HIGH          128
#define FF_PROFILE_H264_HIGH_444             144
#define FF_PROFILE_H264_HIGH_444_PREDICTIVE  244
#define FF_PROFILE_H264_HIGH_444_INTRA       (244|FF_PROFILE_H264_INTRA)
#define FF_PROFILE_H264_CAVLC_444            44

#define FF_PROFILE_VC1_SIMPLE   0
#define FF_PROFILE_VC1_MAIN     1
#define FF_PROFILE_VC1_COMPLEX  2
#define FF_PROFILE_VC1_ADVANCED 3

#define FF_PROFILE_MPEG4_SIMPLE                     0
#define FF_PROFILE_MPEG4_SIMPLE_SCALABLE            1
#define FF_PROFILE_MPEG4_CORE                       2
#define FF_PROFILE_MPEG4_MAIN                       3
#define FF_PROFILE_MPEG4_N_BIT                      4
#define FF_PROFILE_MPEG4_SCALABLE_TEXTURE           5
#define FF_PROFILE_MPEG4_SIMPLE_FACE_ANIMATION      6
#define FF_PROFILE_MPEG4_BASIC_ANIMATED_TEXTURE     7
#define FF_PROFILE_MPEG4_HYBRID                     8
#define FF_PROFILE_MPEG4_ADVANCED_REAL_TIME         9
#define FF_PROFILE_MPEG4_CORE_SCALABLE             10
#define FF_PROFILE_MPEG4_ADVANCED_CODING           11
#define FF_PROFILE_MPEG4_ADVANCED_CORE             12
#define FF_PROFILE_MPEG4_ADVANCED_SCALABLE_TEXTURE 13
#define FF_PROFILE_MPEG4_SIMPLE_STUDIO             14
#define FF_PROFILE_MPEG4_ADVANCED_SIMPLE           15

#define FF_PROFILE_JPEG2000_CSTREAM_RESTRICTION_0   1
#define FF_PROFILE_JPEG2000_CSTREAM_RESTRICTION_1   2
#define FF_PROFILE_JPEG2000_CSTREAM_NO_RESTRICTION  32768
#define FF_PROFILE_JPEG2000_DCINEMA_2K              3
#define FF_PROFILE_JPEG2000_DCINEMA_4K              4

#define FF_PROFILE_VP9_0                            0
#define FF_PROFILE_VP9_1                            1
#define FF_PROFILE_VP9_2                            2
#define FF_PROFILE_VP9_3                            3

#define FF_PROFILE_HEVC_MAIN                        1
#define FF_PROFILE_HEVC_MAIN_10                     2
#define FF_PROFILE_HEVC_MAIN_STILL_PICTURE          3
#define FF_PROFILE_HEVC_REXT                        4

#define FF_PROFILE_AV1_MAIN                         0
#define FF_PROFILE_AV1_HIGH                         1
#define FF_PROFILE_AV1_PROFESSIONAL                 2

#define FF_PROFILE_MJPEG_HUFFMAN_BASELINE_DCT            0xc0
#define FF_PROFILE_MJPEG_HUFFMAN_EXTENDED_SEQUENTIAL_DCT 0xc1
#define FF_PROFILE_MJPEG_HUFFMAN_PROGRESSIVE_DCT         0xc2
#define FF_PROFILE_MJPEG_HUFFMAN_LOSSLESS                0xc3
#define FF_PROFILE_MJPEG_JPEG_LS                         0xf7

#define FF_PROFILE_SBC_MSBC                         1

#define FF_PROFILE_PRORES_PROXY     0
#define FF_PROFILE_PRORES_LT        1
#define FF_PROFILE_PRORES_STANDARD  2
#define FF_PROFILE_PRORES_HQ        3
#define FF_PROFILE_PRORES_4444      4
#define FF_PROFILE_PRORES_XQ        5

#define FF_PROFILE_ARIB_PROFILE_A 0
#define FF_PROFILE_ARIB_PROFILE_C 1

/**
     * level
     * - encoding: Set by user.
     * - decoding: Set by libavcodec.
     */
    int level;
#define FF_LEVEL_UNKNOWN -99

/**
     * Skip loop filtering for selected frames.
     * - encoding: unused
     * - decoding: Set by user.
     */
    enum AVDiscard skip_loop_filter;

/**
     * Skip IDCT/dequantization for selected frames.
     * - encoding: unused
     * - decoding: Set by user.
     */
    enum AVDiscard skip_idct;

/**
     * Skip decoding for selected frames.
     * - encoding: unused
     * - decoding: Set by user.
     */
    enum AVDiscard skip_frame;

/**
     * Header containing style information for text subtitles.
     * For SUBTITLE_ASS subtitle type, it should contain the whole ASS
     * [Script Info] and [V4+ Styles] section, plus the [Events] line and
     * the Format line following. It shouldn't include any Dialogue line.
     * - encoding: Set/allocated/freed by user (before avcodec_open2())
     * - decoding: Set/allocated/freed by libavcodec (by avcodec_open2())
     */
    uint8_t *subtitle_header;
    int subtitle_header_size;

#if FF_API_VBV_DELAY
                                                                                                                            /**
     * VBV delay coded in the last frame (in periods of a 27 MHz clock).
     * Used for compliant TS muxing.
     * - encoding: Set by libavcodec.
     * - decoding: unused.
     * @deprecated this value is now exported as a part of
     * AV_PKT_DATA_CPB_PROPERTIES packet side data
     */
attribute_deprecated
uint64_t vbv_delay;
#endif

#if FF_API_SIDEDATA_ONLY_PKT
                                                                                                                            /**
     * Encoding only and set by default. Allow encoders to output packets
     * that do not contain any encoded data, only side data.
     *
     * Some encoders need to output such packets, e.g. to update some stream
     * parameters at the end of encoding.
     *
     * @deprecated this field disables the default behaviour and
     *             it is kept only for compatibility.
     */
attribute_deprecated
int side_data_only_packets;
#endif

/**
     * Audio only. The number of "priming" samples (padding) inserted by the
     * encoder at the beginning of the audio. I.e. this number of leading
     * decoded samples must be discarded by the caller to get the original audio
     * without leading padding.
     *
     * - decoding: unused
     * - encoding: Set by libavcodec. The timestamps on the output packets are
     *             adjusted by the encoder so that they always refer to the
     *             first sample of the data actually contained in the packet,
     *             including any added padding.  E.g. if the timebase is
     *             1/samplerate and the timestamp of the first input sample is
     *             0, the timestamp of the first output packet will be
     *             -initial_padding.
     */
    int initial_padding;

/**
     * - decoding: For codecs that store a framerate value in the compressed
     *             bitstream, the decoder may export it here. { 0, 1} when
     *             unknown.
     * - encoding: May be used to signal the framerate of CFR content to an
     *             encoder.
     */
    AVRational framerate;

/**
     * Nominal unaccelerated pixel format, see AV_PIX_FMT_xxx.
     * - encoding: unused.
     * - decoding: Set by libavcodec before calling get_format()
     */
    enum AVPixelFormat sw_pix_fmt;

/**
     * Timebase in which pkt_dts/pts and AVPacket.dts/pts are.
     * - encoding unused.
     * - decoding set by user.
     */
    AVRational pkt_timebase;

/**
     * AVCodecDescriptor
     * - encoding: unused.
     * - decoding: set by libavcodec.
     */
    const AVCodecDescriptor *codec_descriptor;

#if !FF_API_LOWRES
/**
     * low resolution decoding, 1-> 1/2 size, 2->1/4 size
     * - encoding: unused
     * - decoding: Set by user.
     */
    int lowres;
#endif

/**
     * Current statistics for PTS correction.
     * - decoding: maintained and used by libavcodec, not intended to be used by user apps
     * - encoding: unused
     */
    int64_t pts_correction_num_faulty_pts; /// Number of incorrect PTS values so far
    int64_t pts_correction_num_faulty_dts; /// Number of incorrect DTS values so far
    int64_t pts_correction_last_pts;       /// PTS of the last frame
    int64_t pts_correction_last_dts;       /// DTS of the last frame

/**
     * Character encoding of the input subtitles file.
     * - decoding: set by user
     * - encoding: unused
     */
    char *sub_charenc;

/**
     * Subtitles character encoding mode. Formats or codecs might be adjusting
     * this setting (if they are doing the conversion themselves for instance).
     * - decoding: set by libavcodec
     * - encoding: unused
     */
    int sub_charenc_mode;
#define FF_SUB_CHARENC_MODE_DO_NOTHING  -1  ///< do nothing (demuxer outputs a stream supposed to be already in UTF-8, or the codec is bitmap for instance)
#define FF_SUB_CHARENC_MODE_AUTOMATIC    0  ///< libavcodec will select the mode itself
#define FF_SUB_CHARENC_MODE_PRE_DECODER  1  ///< the AVPacket data needs to be recoded to UTF-8 before being fed to the decoder, requires iconv
#define FF_SUB_CHARENC_MODE_IGNORE       2  ///< neither convert the subtitles, nor check them for valid UTF-8

/**
     * Skip processing alpha if supported by codec.
     * Note that if the format uses pre-multiplied alpha (common with VP6,
     * and recommended due to better video quality/compression)
     * the image will look as if alpha-blended onto a black background.
     * However for formats that do not use pre-multiplied alpha
     * there might be serious artefacts (though e.g. libswscale currently
     * assumes pre-multiplied alpha anyway).
     *
     * - decoding: set by user
     * - encoding: unused
     */
    int skip_alpha;

/**
     * Number of samples to skip after a discontinuity
     * - decoding: unused
     * - encoding: set by libavcodec
     */
    int seek_preroll;

#if !FF_API_DEBUG_MV
/**
     * debug motion vectors
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int debug_mv;
#define FF_DEBUG_VIS_MV_P_FOR  0x00000001 //visualize forward predicted MVs of P frames
#define FF_DEBUG_VIS_MV_B_FOR  0x00000002 //visualize forward predicted MVs of B frames
#define FF_DEBUG_VIS_MV_B_BACK 0x00000004 //visualize backward predicted MVs of B frames
#endif

/**
     * custom intra quantization matrix
     * - encoding: Set by user, can be NULL.
     * - decoding: unused.
     */
    uint16_t *chroma_intra_matrix;

/**
     * dump format separator.
     * can be ", " or "\n      " or anything else
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    uint8_t *dump_separator;

/**
     * ',' separated list of allowed decoders.
     * If NULL then all are allowed
     * - encoding: unused
     * - decoding: set by user
     */
    char *codec_whitelist;

/**
     * Properties of the stream that gets decoded
     * - encoding: unused
     * - decoding: set by libavcodec
     */
    unsigned properties;
#define FF_CODEC_PROPERTY_LOSSLESS        0x00000001
#define FF_CODEC_PROPERTY_CLOSED_CAPTIONS 0x00000002

/**
     * Additional data associated with the entire coded stream.
     *
     * - decoding: unused
     * - encoding: may be set by libavcodec after avcodec_open2().
     */
    AVPacketSideData *coded_side_data;
    int nb_coded_side_data;

/**
     * A reference to the AVHWFramesContext describing the input (for encoding)
     * or output (decoding) frames. The reference is set by the caller and
     * afterwards owned (and freed) by libavcodec - it should never be read by
     * the caller after being set.
     *
     * - decoding: This field should be set by the caller from the get_format()
     *             callback. The previous reference (if any) will always be
     *             unreffed by libavcodec before the get_format() call.
     *
     *             If the default get_buffer2() is used with a hwaccel pixel
     *             format, then this AVHWFramesContext will be used for
     *             allocating the frame buffers.
     *
     * - encoding: For hardware encoders configured to use a hwaccel pixel
     *             format, this field should be set by the caller to a reference
     *             to the AVHWFramesContext describing input frames.
     *             AVHWFramesContext.format must be equal to
     *             AVCodecContext.pix_fmt.
     *
     *             This field should be set before avcodec_open2() is called.
     */
    AVBufferRef *hw_frames_ctx;

/**
     * Control the form of AVSubtitle.rects[N]->ass
     * - decoding: set by user
     * - encoding: unused
     */
    int sub_text_format;
#define FF_SUB_TEXT_FMT_ASS              0
#if FF_API_ASS_TIMING
#define FF_SUB_TEXT_FMT_ASS_WITH_TIMINGS 1
#endif

/**
     * Audio only. The amount of padding (in samples) appended by the encoder to
     * the end of the audio. I.e. this number of decoded samples must be
     * discarded by the caller from the end of the stream to get the original
     * audio without any trailing padding.
     *
     * - decoding: unused
     * - encoding: unused
     */
    int trailing_padding;

/**
     * The number of pixels per image to maximally accept.
     *
     * - decoding: set by user
     * - encoding: set by user
     */
    int64_t max_pixels;

/**
     * A reference to the AVHWDeviceContext describing the device which will
     * be used by a hardware encoder/decoder.  The reference is set by the
     * caller and afterwards owned (and freed) by libavcodec.
     *
     * This should be used if either the codec device does not require
     * hardware frames or any that are used are to be allocated internally by
     * libavcodec.  If the user wishes to supply any of the frames used as
     * encoder input or decoder output then hw_frames_ctx should be used
     * instead.  When hw_frames_ctx is set in get_format() for a decoder, this
     * field will be ignored while decoding the associated stream segment, but
     * may again be used on a following one after another get_format() call.
     *
     * For both encoders and decoders this field should be set before
     * avcodec_open2() is called and must not be written to thereafter.
     *
     * Note that some decoders may require this field to be set initially in
     * order to support hw_frames_ctx at all - in that case, all frames
     * contexts used must be created on the same device.
     */
    AVBufferRef *hw_device_ctx;

/**
     * Bit set of AV_HWACCEL_FLAG_* flags, which affect hardware accelerated
     * decoding (if active).
     * - encoding: unused
     * - decoding: Set by user (either before avcodec_open2(), or in the
     *             AVCodecContext.get_format callback)
     */
    int hwaccel_flags;

/**
     * Video decoding only. Certain video codecs support cropping, meaning that
     * only a sub-rectangle of the decoded frame is intended for display.  This
     * option controls how cropping is handled by libavcodec.
     *
     * When set to 1 (the default), libavcodec will apply cropping internally.
     * I.e. it will modify the output frame width/height fields and offset the
     * data pointers (only by as much as possible while preserving alignment, or
     * by the full amount if the AV_CODEC_FLAG_UNALIGNED flag is set) so that
     * the frames output by the decoder refer only to the cropped area. The
     * crop_* fields of the output frames will be zero.
     *
     * When set to 0, the width/height fields of the output frames will be set
     * to the coded dimensions and the crop_* fields will describe the cropping
     * rectangle. Applying the cropping is left to the caller.
     *
     * @warning When hardware acceleration with opaque output frames is used,
     * libavcodec is unable to apply cropping from the top/left border.
     *
     * @note when this option is set to zero, the width/height fields of the
     * AVCodecContext and output AVFrames have different meanings. The codec
     * context fields store display dimensions (with the coded dimensions in
     * coded_width/height), while the frame fields store the coded dimensions
     * (with the display dimensions being determined by the crop_* fields).
     */
    int apply_cropping;

/*
     * Video decoding only.  Sets the number of extra hardware frames which
     * the decoder will allocate for use by the caller.  This must be set
     * before avcodec_open2() is called.
     *
     * Some hardware decoders require all frames that they will use for
     * output to be defined in advance before decoding starts.  For such
     * decoders, the hardware frame pool must therefore be of a fixed size.
     * The extra frames set here are on top of any number that the decoder
     * needs internally in order to operate normally (for example, frames
     * used as reference pictures).
     */
    int extra_hw_frames;

/**
     * The percentage of damaged samples to discard a frame.
     *
     * - decoding: set by user
     * - encoding: unused
     */
    int discard_damaged_percentage;
} AVCodecContext;


static const char *hevc_nal_type_name[64] = {"TRAIL_N", // HEVC_NAL_TRAIL_N
        "TRAIL_R", // HEVC_NAL_TRAIL_R
        "TSA_N", // HEVC_NAL_TSA_N
        "TSA_R", // HEVC_NAL_TSA_R
        "STSA_N", // HEVC_NAL_STSA_N
        "STSA_R", // HEVC_NAL_STSA_R
        "RADL_N", // HEVC_NAL_RADL_N
        "RADL_R", // HEVC_NAL_RADL_R
        "RASL_N", // HEVC_NAL_RASL_N
        "RASL_R", // HEVC_NAL_RASL_R
        "RSV_VCL_N10", // HEVC_NAL_VCL_N10
        "RSV_VCL_R11", // HEVC_NAL_VCL_R11
        "RSV_VCL_N12", // HEVC_NAL_VCL_N12
        "RSV_VLC_R13", // HEVC_NAL_VCL_R13
        "RSV_VCL_N14", // HEVC_NAL_VCL_N14
        "RSV_VCL_R15", // HEVC_NAL_VCL_R15
        "BLA_W_LP", // HEVC_NAL_BLA_W_LP
        "BLA_W_RADL", // HEVC_NAL_BLA_W_RADL
        "BLA_N_LP", // HEVC_NAL_BLA_N_LP
        "IDR_W_RADL", // HEVC_NAL_IDR_W_RADL
        "IDR_N_LP", // HEVC_NAL_IDR_N_LP
        "CRA_NUT", // HEVC_NAL_CRA_NUT
        "IRAP_IRAP_VCL22", // HEVC_NAL_IRAP_VCL22
        "IRAP_IRAP_VCL23", // HEVC_NAL_IRAP_VCL23
        "RSV_VCL24", // HEVC_NAL_RSV_VCL24
        "RSV_VCL25", // HEVC_NAL_RSV_VCL25
        "RSV_VCL26", // HEVC_NAL_RSV_VCL26
        "RSV_VCL27", // HEVC_NAL_RSV_VCL27
        "RSV_VCL28", // HEVC_NAL_RSV_VCL28
        "RSV_VCL29", // HEVC_NAL_RSV_VCL29
        "RSV_VCL30", // HEVC_NAL_RSV_VCL30
        "RSV_VCL31", // HEVC_NAL_RSV_VCL31
        "VPS", // HEVC_NAL_VPS
        "SPS", // HEVC_NAL_SPS
        "PPS", // HEVC_NAL_PPS
        "AUD", // HEVC_NAL_AUD
        "EOS_NUT", // HEVC_NAL_EOS_NUT
        "EOB_NUT", // HEVC_NAL_EOB_NUT
        "FD_NUT", // HEVC_NAL_FD_NUT
        "SEI_PREFIX", // HEVC_NAL_SEI_PREFIX
        "SEI_SUFFIX", // HEVC_NAL_SEI_SUFFIX
        "RSV_NVCL41", // HEVC_NAL_RSV_NVCL41
        "RSV_NVCL42", // HEVC_NAL_RSV_NVCL42
        "RSV_NVCL43", // HEVC_NAL_RSV_NVCL43
        "RSV_NVCL44", // HEVC_NAL_RSV_NVCL44
        "RSV_NVCL45", // HEVC_NAL_RSV_NVCL45
        "RSV_NVCL46", // HEVC_NAL_RSV_NVCL46
        "RSV_NVCL47", // HEVC_NAL_RSV_NVCL47
        "UNSPEC48", // HEVC_NAL_UNSPEC48
        "UNSPEC49", // HEVC_NAL_UNSPEC49
        "UNSPEC50", // HEVC_NAL_UNSPEC50
        "UNSPEC51", // HEVC_NAL_UNSPEC51
        "UNSPEC52", // HEVC_NAL_UNSPEC52
        "UNSPEC53", // HEVC_NAL_UNSPEC53
        "UNSPEC54", // HEVC_NAL_UNSPEC54
        "UNSPEC55", // HEVC_NAL_UNSPEC55
        "UNSPEC56", // HEVC_NAL_UNSPEC56
        "UNSPEC57", // HEVC_NAL_UNSPEC57
        "UNSPEC58", // HEVC_NAL_UNSPEC58
        "UNSPEC59", // HEVC_NAL_UNSPEC59
        "UNSPEC60", // HEVC_NAL_UNSPEC60
        "UNSPEC61", // HEVC_NAL_UNSPEC61
        "UNSPEC62", // HEVC_NAL_UNSPEC62
        "UNSPEC63", // HEVC_NAL_UNSPEC63
};


static av_always_inline av_const int ff_ctz_c(int v) {
    static const uint8_t debruijn_ctz32[32] = {0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9};
    return debruijn_ctz32[(uint32_t) ((v & -v) * 0x077CB531U) >> 27];
}

#define ff_ctz ff_ctz_c


#define ALIGN (HAVE_AVX512 ? 64 : (HAVE_AVX ? 32 : 16))

/* NOTE: if you want to override these functions with your own
 * implementations (not recommended) you have to link libav* as
 * dynamic libraries and remove -Wl,-Bsymbolic from the linker flags.
 * Note that this will cost performance. */

static size_t max_alloc_size = INT_MAX;


void *av_realloc(void *ptr, size_t size) {
/* let's disallow possibly ambiguous cases */
    if (size > (max_alloc_size - 32)) {
        return NULL;
    }

#if HAVE_ALIGNED_MALLOC
    return _aligned_realloc(ptr, size + !size, ALIGN);
#else
    return realloc(ptr, size + !size);
#endif
}


void av_free(void *ptr) {
#if HAVE_ALIGNED_MALLOC
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}


static inline int av_size_mult(size_t a, size_t b, size_t *r) {
    size_t t = a * b;
/* Hack inspired from glibc: don't try the division if nelem and elsize
     * are both less than sqrt(SIZE_MAX). */
    if ((a | b) >= ((size_t) 1 << (sizeof(size_t) * 4)) && a && t / a != b) {
        return AVERROR(EINVAL);
    }
    *r = t;
    return 0;
}


void *av_realloc_f(void *ptr, size_t nelem, size_t elsize) {
    size_t size;
    void *r;

    if (av_size_mult(elsize, nelem, &size)) {
        av_free(ptr);
        return NULL;
    }
    r = av_realloc(ptr, size);
    if (!r) {
        av_free(ptr);
    }
    return r;
}


void av_max_alloc(size_t max) {
    max_alloc_size = max;
}

void *av_malloc(size_t size) {
    void *ptr = NULL;

/* let's disallow possibly ambiguous cases */
    if (size > (max_alloc_size - 32)) {
        return NULL;
    }

#if HAVE_POSIX_MEMALIGN
                                                                                                                            if (size) //OS X on SDK 10.6 has a broken posix_memalign implementation
if (posix_memalign(&ptr, ALIGN, size))
ptr = NULL;
#elif HAVE_ALIGNED_MALLOC
    ptr = _aligned_malloc(size, ALIGN);
#elif HAVE_MEMALIGN
                                                                                                                            #ifndef __DJGPP__
ptr = memalign(ALIGN, size);
#else
ptr = memalign(size, ALIGN);
#endif
/* Why 64?
     * Indeed, we should align it:
     *   on  4 for 386
     *   on 16 for 486
     *   on 32 for 586, PPro - K6-III
     *   on 64 for K7 (maybe for P3 too).
     * Because L1 and L2 caches are aligned on those values.
     * But I don't want to code such logic here!
     */
/* Why 32?
     * For AVX ASM. SSE / NEON needs only 16.
     * Why not larger? Because I did not see a difference in benchmarks ...
     */
/* benchmarks with P3
     * memalign(64) + 1          3071, 3051, 3032
     * memalign(64) + 2          3051, 3032, 3041
     * memalign(64) + 4          2911, 2896, 2915
     * memalign(64) + 8          2545, 2554, 2550
     * memalign(64) + 16         2543, 2572, 2563
     * memalign(64) + 32         2546, 2545, 2571
     * memalign(64) + 64         2570, 2533, 2558
     *
     * BTW, malloc seems to do 8-byte alignment by default here.
     */
#else
    ptr = malloc(size);
#endif
    if (!ptr && !size) {
        size = 1;
        ptr = av_malloc(1);
    }
#if CONFIG_MEMORY_POISONING
                                                                                                                            if (ptr)
memset(ptr, FF_MEMORY_POISON, size);
#endif
    return ptr;
}


void av_freep(void *arg) {
    void *val;

    memcpy(&val, arg, sizeof(val));
    memcpy(arg, &(void *) {NULL}, sizeof(val));
    av_free(val);
}


int av_reallocp(void *ptr, size_t size) {
    void *val;

    if (!size) {
        av_freep(ptr);
        return 0;
    }

    memcpy(&val, ptr, sizeof(val));
    val = av_realloc(val, size);

    if (!val) {
        av_freep(ptr);
        return AVERROR(ENOMEM);
    }

    memcpy(ptr, &val, sizeof(val));
    return 0;
}

void *av_mallocz(size_t size) {
    void *ptr = av_malloc(size);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

void *av_malloc_array(size_t nmemb, size_t size) {
    if (!size || nmemb >= INT_MAX / size) {
        return NULL;
    }
    return av_malloc(nmemb * size);
}

void *av_mallocz_array(size_t nmemb, size_t size) {
    if (!size || nmemb >= INT_MAX / size) {
        return NULL;
    }
    return av_mallocz(nmemb * size);
}

void *av_realloc_array(void *ptr, size_t nmemb, size_t size) {
    if (!size || nmemb >= INT_MAX / size) {
        return NULL;
    }
    return av_realloc(ptr, nmemb * size);
}

int av_reallocp_array(void *ptr, size_t nmemb, size_t size) {
    void *val;

    memcpy(&val, ptr, sizeof(val));
    val = av_realloc_f(val, nmemb, size);
    memcpy(ptr, &val, sizeof(val));
    if (!val && nmemb && size) {
        return AVERROR(ENOMEM);
    }

    return 0;
}


void *av_calloc(size_t nmemb, size_t size) {
    if (size <= 0 || nmemb >= INT_MAX / size) {
        return NULL;
    }
    return av_mallocz(nmemb * size);
}

char *av_strdup(const char *s) {
    char *ptr = NULL;
    if (s) {
        size_t len = strlen(s) + 1;
        ptr = av_realloc(NULL, len);
        if (ptr) {
            memcpy(ptr, s, len);
        }
    }
    return ptr;
}

char *av_strndup(const char *s, size_t len) {
    char *ret = NULL, *end;

    if (!s) {
        return NULL;
    }

    end = memchr(s, 0, len);
    if (end) {
        len = end - s;
    }

    ret = av_realloc(NULL, len + 1);
    if (!ret) {
        return NULL;
    }

    memcpy(ret, s, len);
    ret[len] = 0;
    return ret;
}


/**
 * AVCodec.
 */
typedef struct AVCodec {
/**
     * Name of the codec implementation.
     * The name is globally unique among encoders and among decoders (but an
     * encoder and a decoder can share the same name).
     * This is the primary way to find a codec from the user perspective.
     */
    const char *name;
/**
     * Descriptive name for the codec, meant to be more human readable than name.
     * You should use the NULL_IF_CONFIG_SMALL() macro to define it.
     */
    const char *long_name;
    enum AVMediaType type;
    enum AVCodecID id;
/**
     * Codec capabilities.
     * see AV_CODEC_CAP_*
     */
    int capabilities;
    const AVRational *supported_framerates; ///< array of supported framerates, or NULL if any, array is terminated by {0,0}
    const enum AVPixelFormat *pix_fmts;     ///< array of supported pixel formats, or NULL if unknown, array is terminated by -1
    const int *supported_samplerates;       ///< array of supported audio samplerates, or NULL if unknown, array is terminated by 0
    const enum AVSampleFormat *sample_fmts; ///< array of supported sample formats, or NULL if unknown, array is terminated by -1
    const uint64_t *channel_layouts;         ///< array of support channel layouts, or NULL if unknown. array is terminated by 0
    uint8_t max_lowres;                     ///< maximum value for lowres supported by the decoder
    const AVClass *priv_class;              ///< AVClass for the private context
    const AVProfile *profiles;              ///< array of recognized profiles, or NULL if unknown, array is terminated by {FF_PROFILE_UNKNOWN}

/**
     * Group name of the codec implementation.
     * This is a short symbolic name of the wrapper backing this codec. A
     * wrapper uses some kind of external implementation for the codec, such
     * as an external library, or a codec implementation provided by the OS or
     * the hardware.
     * If this field is NULL, this is a builtin, libavcodec native codec.
     * If non-NULL, this will be the suffix in AVCodec.name in most cases
     * (usually AVCodec.name will be of the form "<codec_name>_<wrapper_name>").
     */
    const char *wrapper_name;

/*****************************************************************
     * No fields below this line are part of the public API. They
     * may not be used outside of libavcodec and can be changed and
     * removed at will.
     * New public fields should be added right above.
     *****************************************************************
     */
    int priv_data_size;
    struct AVCodec *next;
/**
     * @name Frame-level threading support functions
     * @{
     */
/**
     * If defined, called on thread contexts when they are created.
     * If the codec allocates writable tables in init(), re-allocate them here.
     * priv_data will be set to a copy of the original.
     */
    int (*init_thread_copy)(AVCodecContext *);

/**
     * Copy necessary context variables from a previous thread context to the current one.
     * If not defined, the next thread will start automatically; otherwise, the codec
     * must call ff_thread_finish_setup().
     *
     * dst and src will (rarely) point to the same context, in which case memcpy should be skipped.
     */
    int (*update_thread_context)(AVCodecContext *dst, const AVCodecContext *src);
/** @} */

/**
     * Private codec-specific defaults.
     */
    const AVCodecDefault *defaults;

/**
     * Initialize codec static data, called from avcodec_register().
     *
     * This is not intended for time consuming operations as it is
     * run for every codec regardless of that codec being used.
     */
    void (*init_static_data)(struct AVCodec *codec);

    int (*init)(AVCodecContext *);

    int (*encode_sub)(AVCodecContext *, uint8_t *buf, int buf_size, const struct AVSubtitle *sub);

/**
     * Encode data to an AVPacket.
     *
     * @param      avctx          codec context
     * @param      avpkt          output AVPacket (may contain a user-provided buffer)
     * @param[in]  frame          AVFrame containing the raw data to be encoded
     * @param[out] got_packet_ptr encoder sets to 0 or 1 to indicate that a
     *                            non-empty packet was returned in avpkt.
     * @return 0 on success, negative error code on failure
     */
    int (*encode2)(AVCodecContext *avctx, AVPacket *avpkt, const AVFrame *frame, int *got_packet_ptr);

    int (*decode)(AVCodecContext *, void *outdata, int *outdata_size, AVPacket *avpkt);

    int (*close)(AVCodecContext *);

/**
     * Encode API with decoupled packet/frame dataflow. The API is the
     * same as the avcodec_ prefixed APIs (avcodec_send_frame() etc.), except
     * that:
     * - never called if the codec is closed or the wrong type,
     * - if AV_CODEC_CAP_DELAY is not set, drain frames are never sent,
     * - only one drain frame is ever passed down,
     */
    int (*send_frame)(AVCodecContext *avctx, const AVFrame *frame);

    int (*receive_packet)(AVCodecContext *avctx, AVPacket *avpkt);

/**
     * Decode API with decoupled packet/frame dataflow. This function is called
     * to get one output frame. It should call ff_decode_get_packet() to obtain
     * input data.
     */
    int (*receive_frame)(AVCodecContext *avctx, AVFrame *frame);

/**
     * Flush buffers.
     * Will be called when seeking
     */
    void (*flush)(AVCodecContext *);

/**
     * Internal codec capabilities.
     * See FF_CODEC_CAP_* in internal.h
     */
    int caps_internal;

/**
     * Decoding only, a comma-separated list of bitstream filters to apply to
     * packets before decoding.
     */
    const char *bsfs;

/**
     * Array of pointers to hardware configurations supported by the codec,
     * or NULL if no hardware supported.  The array is terminated by a NULL
     * pointer.
     *
     * The user can only access this field via avcodec_get_hw_config().
     */
    const struct AVCodecHWConfigInternal **hw_configs;
} AVCodec;

typedef struct AVCodecParserContext {
    void *priv_data;
    struct AVCodecParser *parser;
    int64_t frame_offset; /* offset of the current frame */
    int64_t cur_offset; /* current offset
                           (incremented by each av_parser_parse()) */
    int64_t next_frame_offset; /* offset of the next frame */
/* video info */
    int pict_type; /* XXX: Put it back in AVCodecContext. */
/**
     * This field is used for proper frame duration computation in lavf.
     * It signals, how much longer the frame duration of the current frame
     * is compared to normal frame duration.
     *
     * frame_duration = (1 + repeat_pict) * time_base
     *
     * It is used by codecs like H.264 to display telecined material.
     */
    int repeat_pict; /* XXX: Put it back in AVCodecContext. */
    int64_t pts;     /* pts of the current frame */
    int64_t dts;     /* dts of the current frame */

/* private data */
    int64_t last_pts;
    int64_t last_dts;
    int fetch_timestamp;

#define AV_PARSER_PTS_NB 4
    int cur_frame_start_index;
    int64_t cur_frame_offset[AV_PARSER_PTS_NB];
    int64_t cur_frame_pts[AV_PARSER_PTS_NB];
    int64_t cur_frame_dts[AV_PARSER_PTS_NB];

    int flags;
#define PARSER_FLAG_COMPLETE_FRAMES           0x0001
#define PARSER_FLAG_ONCE                      0x0002
/// Set if the parser has a valid file offset
#define PARSER_FLAG_FETCHED_OFFSET            0x0004
#define PARSER_FLAG_USE_CODEC_TS              0x1000

    int64_t offset;      ///< byte offset from starting packet start
    int64_t cur_frame_end[AV_PARSER_PTS_NB];

/**
     * Set by parser to 1 for key frames and 0 for non-key frames.
     * It is initialized to -1, so if the parser doesn't set this flag,
     * old-style fallback using AV_PICTURE_TYPE_I picture type as key frames
     * will be used.
     */
    int key_frame;

#if FF_API_CONVERGENCE_DURATION
                                                                                                                            /**
     * @deprecated unused
     */
attribute_deprecated
int64_t convergence_duration;
#endif

// Timestamp generation support:
/**
     * Synchronization point for start of timestamp generation.
     *
     * Set to >0 for sync point, 0 for no sync point and <0 for undefined
     * (default).
     *
     * For example, this corresponds to presence of H.264 buffering period
     * SEI message.
     */
    int dts_sync_point;

/**
     * Offset of the current timestamp against last timestamp sync point in
     * units of AVCodecContext.time_base.
     *
     * Set to INT_MIN when dts_sync_point unused. Otherwise, it must
     * contain a valid timestamp offset.
     *
     * Note that the timestamp of sync point has usually a nonzero
     * dts_ref_dts_delta, which refers to the previous sync point. Offset of
     * the next frame after timestamp sync point will be usually 1.
     *
     * For example, this corresponds to H.264 cpb_removal_delay.
     */
    int dts_ref_dts_delta;

/**
     * Presentation delay of current frame in units of AVCodecContext.time_base.
     *
     * Set to INT_MIN when dts_sync_point unused. Otherwise, it must
     * contain valid non-negative timestamp delta (presentation time of a frame
     * must not lie in the past).
     *
     * This delay represents the difference between decoding and presentation
     * time of the frame.
     *
     * For example, this corresponds to H.264 dpb_output_delay.
     */
    int pts_dts_delta;

/**
     * Position of the packet in file.
     *
     * Analogous to cur_frame_pts/dts
     */
    int64_t cur_frame_pos[AV_PARSER_PTS_NB];

/**
     * Byte position of currently parsed frame in stream.
     */
    int64_t pos;

/**
     * Previous frame byte position.
     */
    int64_t last_pos;

/**
     * Duration of the current frame.
     * For audio, this is in units of 1 / AVCodecContext.sample_rate.
     * For all other types, this is in units of AVCodecContext.time_base.
     */
    int duration;

    enum AVFieldOrder field_order;

/**
     * Indicate whether a picture is coded as a frame, top field or bottom field.
     *
     * For example, H.264 field_pic_flag equal to 0 corresponds to
     * AV_PICTURE_STRUCTURE_FRAME. An H.264 picture with field_pic_flag
     * equal to 1 and bottom_field_flag equal to 0 corresponds to
     * AV_PICTURE_STRUCTURE_TOP_FIELD.
     */
    enum AVPictureStructure picture_structure;

/**
     * Picture number incremented in presentation or output order.
     * This field may be reinitialized at the first picture of a new sequence.
     *
     * For example, this corresponds to H.264 PicOrderCnt.
     */
    int output_picture_number;

/**
     * Dimensions of the decoded video intended for presentation.
     */
    int width;
    int height;

/**
     * Dimensions of the coded video.
     */
    int coded_width;
    int coded_height;

/**
     * The format of the coded data, corresponds to enum AVPixelFormat for video
     * and for enum AVSampleFormat for audio.
     *
     * Note that a decoder can have considerable freedom in how exactly it
     * decodes the data, so the format reported here might be different from the
     * one returned by a decoder.
     */
    int format;
} AVCodecParserContext;

typedef struct AVCodecParser {
    int codec_ids[5]; /* several codec IDs are permitted */
    int priv_data_size;

    int (*parser_init)(AVCodecParserContext *s);

/* This callback never returns an error, a negative value means that
     * the frame start was in a previous packet. */
    int (*parser_parse)(AVCodecParserContext *s, AVCodecContext *avctx, const uint8_t **poutbuf, int *poutbuf_size, const uint8_t *buf, int buf_size);

    void (*parser_close)(AVCodecParserContext *s);

    int (*split)(AVCodecContext *avctx, const uint8_t *buf, int buf_size);

    struct AVCodecParser *next;
} AVCodecParser;


typedef struct PutBitContext {
    uint32_t bit_buf;
    int bit_left;
    uint8_t *buf, *buf_ptr, *buf_end;
    int size_in_bits;
} PutBitContext;


static inline void put_bits(PutBitContext *s, int n, unsigned int value) {
    unsigned int bit_buf;
    int bit_left;

    av_assert2(n <= 31 && value < (1U << n));

    bit_buf = s->bit_buf;
    bit_left = s->bit_left;

/* XXX: optimize */
#ifdef BITSTREAM_WRITER_LE
                                                                                                                            bit_buf |= value << (32 - bit_left);
if (n >= bit_left) {
if (3 < s->buf_end - s->buf_ptr) {
AV_WL32(s->buf_ptr, bit_buf);
s->buf_ptr += 4;
} else {
av_log(NULL, AV_LOG_ERROR, "Internal error, put_bits buffer too small\n");
av_assert2(0);
}
bit_buf     = value >> bit_left;
bit_left   += 32;
}
bit_left -= n;
#else
    if (n < bit_left) {
        bit_buf = (bit_buf << n) | value;
        bit_left -= n;
    } else {
        bit_buf <<= bit_left;
        bit_buf |= value >> (n - bit_left);
        if (3 < s->buf_end - s->buf_ptr) {
            AV_WB32(s->buf_ptr, bit_buf);
            s->buf_ptr += 4;
        } else {
            av_log(NULL, AV_LOG_ERROR, "Internal error, put_bits buffer too small\n");
            av_assert2(0);
        }
        bit_left += 32 - n;
        bit_buf = value;
    }
#endif

    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
}


static const char *h264_nal_type_name[32] = {"Unspecified 0", //H264_NAL_UNSPECIFIED
        "Coded slice of a non-IDR picture", // H264_NAL_SLICE
        "Coded slice data partition A", // H264_NAL_DPA
        "Coded slice data partition B", // H264_NAL_DPB
        "Coded slice data partition C", // H264_NAL_DPC
        "IDR", // H264_NAL_IDR_SLICE
        "SEI", // H264_NAL_SEI
        "SPS", // H264_NAL_SPS
        "PPS", // H264_NAL_PPS
        "AUD", // H264_NAL_AUD
        "End of sequence", // H264_NAL_END_SEQUENCE
        "End of stream", // H264_NAL_END_STREAM
        "Filler data", // H264_NAL_FILLER_DATA
        "SPS extension", // H264_NAL_SPS_EXT
        "Prefix", // H264_NAL_PREFIX
        "Subset SPS", // H264_NAL_SUB_SPS
        "Depth parameter set", // H264_NAL_DPS
        "Reserved 17", // H264_NAL_RESERVED17
        "Reserved 18", // H264_NAL_RESERVED18
        "Auxiliary coded picture without partitioning", // H264_NAL_AUXILIARY_SLICE
        "Slice extension", // H264_NAL_EXTEN_SLICE
        "Slice extension for a depth view or a 3D-AVC texture view", // H264_NAL_DEPTH_EXTEN_SLICE
        "Reserved 22", // H264_NAL_RESERVED22
        "Reserved 23", // H264_NAL_RESERVED23
        "Unspecified 24", // H264_NAL_UNSPECIFIED24
        "Unspecified 25", // H264_NAL_UNSPECIFIED25
        "Unspecified 26", // H264_NAL_UNSPECIFIED26
        "Unspecified 27", // H264_NAL_UNSPECIFIED27
        "Unspecified 28", // H264_NAL_UNSPECIFIED28
        "Unspecified 29", // H264_NAL_UNSPECIFIED29
        "Unspecified 30", // H264_NAL_UNSPECIFIED30
        "Unspecified 31", // H264_NAL_UNSPECIFIED31
};


const uint8_t ff_ue_golomb_len[256] = {1, 3, 3, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 17,};


const uint8_t ff_ue_golomb_vlc_code[512] = {32, 32, 32, 32, 32, 32, 32, 32, 31, 32, 32, 32, 32, 32, 32, 32, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const int8_t ff_se_golomb_vlc_code[512] = {17, 17, 17, 17, 17, 17, 17, 17, 16, 17, 17, 17, 17, 17, 17, 17, 8, -8, 9, -9, 10, -10, 11, -11, 12, -12, 13, -13, 14, -14, 15, -15, 4, 4, 4, 4, -4, -4, -4, -4, 5, 5, 5, 5, -5, -5, -5, -5, 6, 6, 6, 6, -6, -6, -6, -6, 7, 7, 7, 7, -7, -7, -7, -7, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};

const uint8_t ff_golomb_vlc_len[512] = {19, 17, 15, 15, 13, 13, 13, 13, 11, 11, 11, 11, 11, 11, 11, 11, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};


const uint8_t ff_log2_tab[256] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};

#define ff_log2 ff_log2_c

static av_always_inline av_const int ff_log2_c(unsigned int v) {
    int n = 0;
    if (v & 0xffff0000) {
        v >>= 16;
        n += 16;
    }
    if (v & 0xff00) {
        v >>= 8;
        n += 8;
    }
    n += ff_log2_tab[v];

    return n;
}

#define ff_log2_16bit ff_log2_16bit_c

static av_always_inline av_const int ff_log2_16bit_c(unsigned int v) {
    int n = 0;
    if (v & 0xff00) {
        v >>= 8;
        n += 8;
    }
    n += ff_log2_tab[v];

    return n;
}


int av_log2(unsigned v) {
    return ff_log2(v);
}

int av_log2_16bit(unsigned v) {
    return ff_log2_16bit(v);
}

static inline void set_ue_golomb(PutBitContext *pb, int i) {
    av_assert2(i >= 0);
    av_assert2(i <= 0xFFFE);

    if (i < 256) {
        put_bits(pb, ff_ue_golomb_len[i], i + 1);
    } else {
        int e = av_log2(i + 1);
        put_bits(pb, 2 * e + 1, i + 1);
    }
}

static inline void set_se_golomb(PutBitContext *pb, int i) {
    i = 2 * i - 1;
    if (i < 0) {
        i ^= -1;
    }    //FIXME check if gcc does the right thing
    set_ue_golomb(pb, i);
}

static inline int get_ue_golomb(GetBitContext *gb) {
    unsigned int buf;

#if CACHED_BITSTREAM_READER
                                                                                                                            buf = show_bits_long(gb, 32);

if (buf >= (1 << 27)) {
buf >>= 32 - 9;
skip_bits_long(gb, ff_golomb_vlc_len[buf]);

return ff_ue_golomb_vlc_code[buf];
} else {
int log = 2 * av_log2(buf) - 31;
buf >>= log;
buf--;
skip_bits_long(gb, 32 - log);

return buf;
}
#else
    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf = GET_CACHE(re, gb);

    if (buf >= (1 << 27)) {
        buf >>= 32 - 9;
        LAST_SKIP_BITS(re, gb, ff_golomb_vlc_len[buf]);
        CLOSE_READER(re, gb);

        return ff_ue_golomb_vlc_code[buf];
    } else {
        int log = 2 * av_log2(buf) - 31;
        LAST_SKIP_BITS(re, gb, 32 - log);
        CLOSE_READER(re, gb);
        if (log < 7) {
            av_log(NULL, AV_LOG_ERROR, "Invalid UE golomb code\n");
            return AVERROR_INVALIDDATA;
        }
        buf >>= log;
        buf--;

        return buf;
    }
#endif
}


static inline unsigned get_ue_golomb_long(GetBitContext *gb) {
    unsigned buf, log;

    buf = show_bits_long(gb, 32);
    log = 31 - av_log2(buf);
    skip_bits_long(gb, log);

    return get_bits_long(gb, log + 1) - 1;
}


static inline int get_se_golomb_long(GetBitContext *gb) {
    unsigned int buf = get_ue_golomb_long(gb);
    int sign = (buf & 1) - 1;
    return ((buf >> 1) ^ sign) + 1;
}


static inline int get_se_golomb(GetBitContext *gb) {
    unsigned int buf;

#if CACHED_BITSTREAM_READER
                                                                                                                            buf = show_bits_long(gb, 32);

if (buf >= (1 << 27)) {
buf >>= 32 - 9;
skip_bits_long(gb, ff_golomb_vlc_len[buf]);

return ff_se_golomb_vlc_code[buf];
} else {
int log = 2 * av_log2(buf) - 31;
buf >>= log;

skip_bits_long(gb, 32 - log);

if (buf & 1)
buf = -(buf >> 1);
else
buf = (buf >> 1);

return buf;
}
#else
    OPEN_READER(re, gb);
    UPDATE_CACHE(re, gb);
    buf = GET_CACHE(re, gb);

    if (buf >= (1 << 27)) {
        buf >>= 32 - 9;
        LAST_SKIP_BITS(re, gb, ff_golomb_vlc_len[buf]);
        CLOSE_READER(re, gb);

        return ff_se_golomb_vlc_code[buf];
    } else {
        int log = av_log2(buf), sign;
        LAST_SKIP_BITS(re, gb, 31 - log);
        UPDATE_CACHE(re, gb);
        buf = GET_CACHE(re, gb);

        buf >>= log;

        LAST_SKIP_BITS(re, gb, 32 - log);
        CLOSE_READER(re, gb);

        sign = -(buf & 1);
        buf = ((buf >> 1) ^ sign) - sign;

        return buf;
    }
#endif
}


typedef struct PutByteContext {
    uint8_t *buffer, *buffer_end, *buffer_start;
    int eof;
} PutByteContext;


typedef struct GetByteContext {
    const uint8_t *buffer, *buffer_end, *buffer_start;
} GetByteContext;

static av_always_inline void bytestream2_skip(GetByteContext *g, unsigned int size) {
    g->buffer += FFMIN(g->buffer_end - g->buffer, size);
}

static av_always_inline int bytestream2_tell(GetByteContext *g) {
    return (int) (g->buffer - g->buffer_start);
}


static av_always_inline unsigned int bytestream2_get_bytes_left(GetByteContext *g) {
    return g->buffer_end - g->buffer;
}

static av_always_inline unsigned int bytestream2_get_bytes_left_p(PutByteContext *p) {
    return p->buffer_end - p->buffer;
}


static av_always_inline void bytestream2_init(GetByteContext *g, const uint8_t *buf, int buf_size) {
    av_assert0(buf_size >= 0);
    g->buffer = buf;
    g->buffer_start = buf;
    g->buffer_end = buf + buf_size;
}


#define DEF(type, name, bytes, read, write)                                  \
static av_always_inline type bytestream_get_ ## name(const uint8_t **b)        \
{                                                                              \
    (*b) += bytes;                                                             \
    return read(*b - bytes);                                                   \
}                                                                              \
static av_always_inline void bytestream_put_ ## name(uint8_t **b,              \
                                                     const type value)         \
{                                                                              \
    write(*b, value);                                                          \
    (*b) += bytes;                                                             \
}                                                                              \
static av_always_inline void bytestream2_put_ ## name ## u(PutByteContext *p,  \
                                                           const type value)   \
{                                                                              \
    bytestream_put_ ## name(&p->buffer, value);                                \
}                                                                              \
static av_always_inline void bytestream2_put_ ## name(PutByteContext *p,       \
                                                      const type value)        \
{                                                                              \
    if (!p->eof && (p->buffer_end - p->buffer >= bytes)) {                     \
        write(p->buffer, value);                                               \
        p->buffer += bytes;                                                    \
    } else                                                                     \
        p->eof = 1;                                                            \
}                                                                              \
static av_always_inline type bytestream2_get_ ## name ## u(GetByteContext *g)  \
{                                                                              \
    return bytestream_get_ ## name(&g->buffer);                                \
}                                                                              \
static av_always_inline type bytestream2_get_ ## name(GetByteContext *g)       \
{                                                                              \
    if (g->buffer_end - g->buffer < bytes) {                                   \
        g->buffer = g->buffer_end;                                             \
        return 0;                                                              \
    }                                                                          \
    return bytestream2_get_ ## name ## u(g);                                   \
}                                                                              \
static av_always_inline type bytestream2_peek_ ## name(GetByteContext *g)      \
{                                                                              \
    if (g->buffer_end - g->buffer < bytes)                                     \
        return 0;                                                              \
    return read(g->buffer);                                                    \
}


DEF(uint64_t, le64, 8, AV_RL64, AV_WL64)

DEF(unsigned int, le32, 4, AV_RL32, AV_WL32)

DEF(unsigned int, le24, 3, AV_RL24, AV_WL24)

DEF(unsigned int, le16, 2, AV_RL16, AV_WL16)

DEF(uint64_t, be64, 8, AV_RB64, AV_WB64)

DEF(unsigned int, be32, 4, AV_RB32, AV_WB32)

DEF(unsigned int, be24, 3, AV_RB24, AV_WB24)

DEF(unsigned int, be16, 2, AV_RB16, AV_WB16)

DEF(unsigned int, byte, 1, AV_RB8, AV_WB8)


static int get_bit_length(H2645NAL *nal, int skip_trailing_zeros) {
    int size = nal->size;
    int v;

    while (skip_trailing_zeros && size > 0 && nal->data[size - 1] == 0) {
        size--;
    }

    if (!size) {
        return 0;
    }

    v = nal->data[size - 1];

    if (size > INT_MAX / 8) {
        return AVERROR(ERANGE);
    }
    size *= 8;

/* remove the stop bit and following trailing zeros,
     * or nothing for damaged bitstreams */
    if (v) {
        size -= ff_ctz(v) + 1;
    }

    return size;
}


#define ff_ctzll ff_ctzll_c

/* We use the De-Bruijn method outlined in:
 * http://supertech.csail.mit.edu/papers/debruijn.pdf. */
static av_always_inline av_const int ff_ctzll_c(long long v) {
    static const uint8_t debruijn_ctz64[64] = {0, 1, 2, 53, 3, 7, 54, 27, 4, 38, 41, 8, 34, 55, 48, 28, 62, 5, 39, 46, 44, 42, 22, 9, 24, 35, 59, 56, 49, 18, 29, 11, 63, 52, 6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10, 51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12};
    return debruijn_ctz64[(uint64_t) ((v & -v) * 0x022FDD63CC95386DU) >> 58];
}


/* Stein's binary GCD algorithm:
 * https://en.wikipedia.org/wiki/Binary_GCD_algorithm */
int64_t av_gcd(int64_t a, int64_t b) {
    int za, zb, k;
    int64_t u, v;
    if (a == 0) {
        return b;
    }
    if (b == 0) {
        return a;
    }
    za = ff_ctzll(a);
    zb = ff_ctzll(b);
    k = FFMIN(za, zb);
    u = llabs(a >> za);
    v = llabs(b >> zb);
    while (u != v) {
        if (u > v)
            FFSWAP(int64_t, v, u);
        v -= u;
        v >>= ff_ctzll(v);
    }
    return (uint64_t) u << k;
}


int av_reduce(int *dst_num, int *dst_den, int64_t num, int64_t den, int64_t max) {
    AVRational a0 = {0, 1}, a1 = {1, 0};
    int sign = (num < 0) ^(den < 0);
    int64_t gcd = av_gcd(FFABS(num), FFABS(den));

    if (gcd) {
        num = FFABS(num) / gcd;
        den = FFABS(den) / gcd;
    }
    if (num <= max && den <= max) {
        a1 = (AVRational) {num, den};
        den = 0;
    }

    while (den) {
        uint64_t x = num / den;
        int64_t next_den = num - den * x;
        int64_t a2n = x * a1.num + a0.num;
        int64_t a2d = x * a1.den + a0.den;

        if (a2n > max || a2d > max) {
            if (a1.num) {
                x = (max - a0.num) / a1.num;
            }
            if (a1.den) {
                x = FFMIN(x, (max - a0.den) / a1.den);
            }

            if (den * (2 * x * a1.den + a0.den) > num * a1.den) {
                a1 = (AVRational) {x * a1.num + a0.num, x * a1.den + a0.den};
            }
            break;
        }

        a0 = a1;
        a1 = (AVRational) {a2n, a2d};
        num = den;
        den = next_den;
    }
    av_assert2(av_gcd(a1.num, a1.den) <= 1U);
    av_assert2(a1.num <= max && a1.den <= max);

    *dst_num = sign ? -a1.num : a1.num;
    *dst_den = a1.den;

    return den == 0;
}


int ff_hevc_compute_poc(const HEVCSPS *sps, int pocTid0, int poc_lsb, int nal_unit_type) {
    int max_poc_lsb = 1 << sps->log2_max_poc_lsb;
    int prev_poc_lsb = pocTid0 % max_poc_lsb;
    int prev_poc_msb = pocTid0 - prev_poc_lsb;
    int poc_msb;

    if (poc_lsb < prev_poc_lsb && prev_poc_lsb - poc_lsb >= max_poc_lsb / 2) {
        poc_msb = prev_poc_msb + max_poc_lsb;
    } else if (poc_lsb > prev_poc_lsb && poc_lsb - prev_poc_lsb > max_poc_lsb / 2) {
        poc_msb = prev_poc_msb - max_poc_lsb;
    } else {
        poc_msb = prev_poc_msb;
    }

// For BLA picture types, POCmsb is set to 0.
    if (nal_unit_type == HEVC_NAL_BLA_W_LP || nal_unit_type == HEVC_NAL_BLA_W_RADL ||
        nal_unit_type == HEVC_NAL_BLA_N_LP) {
        poc_msb = 0;
    }

    return poc_msb + poc_lsb;
}


static int hevc_parse_slice_header(AVCodecParserContext *s, H2645NAL *nal, AVCodecContext *avctx) {
    HEVCParserContext *ctx = s->priv_data;
    HEVCParamSets *ps = &ctx->ps;
    HEVCSEI *sei = &ctx->sei;
    SliceHeader *sh = &ctx->sh;
    GetBitContext *gb = &nal->gb;
    const HEVCWindow *ow;
    int i, num = 0, den = 0;

    sh->first_slice_in_pic_flag = get_bits1(gb);
    s->picture_structure = sei->picture_timing.picture_struct;
    s->field_order = sei->picture_timing.picture_struct;

    if (IS_IRAP_NAL(nal)) {
        s->key_frame = 1;
        sh->no_output_of_prior_pics_flag = get_bits1(gb);
    }

    sh->pps_id = get_ue_golomb(gb);
    if (sh->pps_id >= HEVC_MAX_PPS_COUNT || !ps->pps_list[sh->pps_id]) {
        av_log(avctx, AV_LOG_ERROR, "PPS id out of range: %d\n", sh->pps_id);
        return AVERROR_INVALIDDATA;
    }
    ps->pps = (HEVCPPS *) ps->pps_list[sh->pps_id]->data;

    if (ps->pps->sps_id >= HEVC_MAX_SPS_COUNT || !ps->sps_list[ps->pps->sps_id]) {
        av_log(avctx, AV_LOG_ERROR, "SPS id out of range: %d\n", ps->pps->sps_id);
        return AVERROR_INVALIDDATA;
    }
    if (ps->sps != (HEVCSPS *) ps->sps_list[ps->pps->sps_id]->data) {
        ps->sps = (HEVCSPS *) ps->sps_list[ps->pps->sps_id]->data;
        ps->vps = (HEVCVPS *) ps->vps_list[ps->sps->vps_id]->data;
    }
    ow = &ps->sps->output_window;

    s->coded_width = ps->sps->width;
    s->coded_height = ps->sps->height;
    s->width = ps->sps->width - ow->left_offset - ow->right_offset;
    s->height = ps->sps->height - ow->top_offset - ow->bottom_offset;
    s->format = ps->sps->pix_fmt;
    avctx->profile = ps->sps->ptl.general_ptl.profile_idc;
    avctx->level = ps->sps->ptl.general_ptl.level_idc;

    if (ps->vps->vps_timing_info_present_flag) {
        num = ps->vps->vps_num_units_in_tick;
        den = ps->vps->vps_time_scale;
    } else if (ps->sps->vui.vui_timing_info_present_flag) {
        num = ps->sps->vui.vui_num_units_in_tick;
        den = ps->sps->vui.vui_time_scale;
    }

    if (num != 0 && den != 0) {
        av_reduce(&avctx->framerate.den, &avctx->framerate.num, num, den, 1 << 30);
    }

    if (!sh->first_slice_in_pic_flag) {
        int slice_address_length;

        if (ps->pps->dependent_slice_segments_enabled_flag) {
            sh->dependent_slice_segment_flag = get_bits1(gb);
        } else {
            sh->dependent_slice_segment_flag = 0;
        }

        slice_address_length = av_ceil_log2_c(ps->sps->ctb_width * ps->sps->ctb_height);
        sh->slice_segment_addr = get_bitsz(gb, slice_address_length);
        if (sh->slice_segment_addr >= ps->sps->ctb_width * ps->sps->ctb_height) {
            av_log(avctx, AV_LOG_ERROR, "Invalid slice segment address: %u.\n", sh->slice_segment_addr);
            return AVERROR_INVALIDDATA;
        }
    } else {
        sh->dependent_slice_segment_flag = 0;
    }

    if (sh->dependent_slice_segment_flag) {
        return 0;
    } /* break; */

    for (i = 0; i < ps->pps->num_extra_slice_header_bits; i++) {
        skip_bits(gb, 1);
    } // slice_reserved_undetermined_flag[]

    sh->slice_type = get_ue_golomb(gb);
    if (!(sh->slice_type == HEVC_SLICE_I || sh->slice_type == HEVC_SLICE_P ||
          sh->slice_type == HEVC_SLICE_B)) {
        av_log(avctx, AV_LOG_ERROR, "Unknown slice type: %d.\n", sh->slice_type);
        return AVERROR_INVALIDDATA;
    }
    s->pict_type = sh->slice_type == HEVC_SLICE_B ? AV_PICTURE_TYPE_B : sh->slice_type ==
                                                                        HEVC_SLICE_P ? AV_PICTURE_TYPE_P : AV_PICTURE_TYPE_I;

    if (ps->pps->output_flag_present_flag)
        sh->pic_output_flag = get_bits1(gb);

    if (ps->sps->separate_colour_plane_flag)
        sh->colour_plane_id = get_bits(gb, 2);

    if (!IS_IDR_NAL(nal)) {
        sh->pic_order_cnt_lsb = get_bits(gb, ps->sps->log2_max_poc_lsb);
        s->output_picture_number = ctx->poc = ff_hevc_compute_poc(ps->sps, ctx->pocTid0, sh->pic_order_cnt_lsb, nal->type);
    } else
        s->output_picture_number = ctx->poc = 0;

    if (nal->temporal_id == 0 && nal->type != HEVC_NAL_TRAIL_N && nal->type != HEVC_NAL_TSA_N &&
        nal->type != HEVC_NAL_STSA_N && nal->type != HEVC_NAL_RADL_N &&
        nal->type != HEVC_NAL_RASL_N && nal->type != HEVC_NAL_RADL_R &&
        nal->type != HEVC_NAL_RASL_R)
        ctx->pocTid0 = ctx->poc;

    return 1; /* no need to evaluate the rest */
}


void av_buffer_default_free(void *opaque, uint8_t *data) {
    av_free(data);
}


static void buffer_replace(AVBufferRef **dst, AVBufferRef **src) {
    AVBuffer *b;

    b = (*dst)->buffer;

    if (src) {
        **dst = **src;
        av_freep(src);
    } else {
        av_freep(dst);
    }

    if (atomic_fetch_add_explicit(&b->refcount, -1, memory_order_acq_rel) == 1) {
        b->free(b->opaque, b->data);
        av_freep(&b);
    }
}

void av_buffer_unref(AVBufferRef **buf) {
    if (!buf || !*buf) {
        return;
    }

    buffer_replace(buf, NULL);
}


AVBufferRef *av_buffer_create(uint8_t *data, int size, void (*free)(void *opaque, uint8_t *data), void *opaque, int flags) {
    AVBufferRef *ref = NULL;
    AVBuffer *buf = NULL;

    buf = av_mallocz(sizeof(*buf));
    if (!buf) {
        return NULL;
    }

    buf->data = data;
    buf->size = size;
    buf->free = free ? free : av_buffer_default_free;
    buf->opaque = opaque;

    atomic_init(&buf->refcount, 1);

    if (flags & AV_BUFFER_FLAG_READONLY) {
        buf->flags |= BUFFER_FLAG_READONLY;
    }

    ref = av_mallocz(sizeof(*ref));
    if (!ref) {
        av_freep(&buf);
        return NULL;
    }

    ref->buffer = buf;
    ref->data = data;
    ref->size = size;

    return ref;
}


static void hevc_pps_free(void *opaque, uint8_t *data) {
    HEVCPPS *pps = (HEVCPPS *) data;

    av_freep(&pps->column_width);
    av_freep(&pps->row_height);
    av_freep(&pps->col_bd);
    av_freep(&pps->row_bd);
    av_freep(&pps->col_idxX);
    av_freep(&pps->ctb_addr_rs_to_ts);
    av_freep(&pps->ctb_addr_ts_to_rs);
    av_freep(&pps->tile_pos_rs);
    av_freep(&pps->tile_id);
    av_freep(&pps->min_tb_addr_zs_tab);

    av_freep(&pps);
}


static void remove_pps(HEVCParamSets *s, int id) {
    if (s->pps_list[id] && s->pps == (const HEVCPPS *) s->pps_list[id]->data) {
        s->pps = NULL;
    }
    av_buffer_unref(&s->pps_list[id]);
}

static void remove_sps(HEVCParamSets *s, int id) {
    int i;
    if (s->sps_list[id]) {
        if (s->sps == (const HEVCSPS *) s->sps_list[id]->data) {
            s->sps = NULL;
        }

/* drop all PPS that depend on this SPS */
        for (i = 0; i < FF_ARRAY_ELEMS(s->pps_list); i++) {
            if (s->pps_list[i] && ((HEVCPPS *) s->pps_list[i]->data)->sps_id == id) {
                remove_pps(s, i);
            }
        }

        av_assert0(!(s->sps_list[id] && s->sps == (HEVCSPS *) s->sps_list[id]->data));
    }
    av_buffer_unref(&s->sps_list[id]);
}


static inline int setup_pps(AVCodecContext *avctx, GetBitContext *gb, HEVCPPS *pps, HEVCSPS *sps) {
    int log2_diff;
    int pic_area_in_ctbs;
    int i, j, x, y, ctb_addr_rs, tile_id;

// Inferred parameters
    pps->col_bd = av_malloc_array(pps->num_tile_columns + 1, sizeof(*pps->col_bd));
    pps->row_bd = av_malloc_array(pps->num_tile_rows + 1, sizeof(*pps->row_bd));
    pps->col_idxX = av_malloc_array(sps->ctb_width, sizeof(*pps->col_idxX));
    if (!pps->col_bd || !pps->row_bd || !pps->col_idxX) {
        return AVERROR(ENOMEM);
    }

    if (pps->uniform_spacing_flag) {
        if (!pps->column_width) {
            pps->column_width = av_malloc_array(pps->num_tile_columns, sizeof(*pps->column_width));
            pps->row_height = av_malloc_array(pps->num_tile_rows, sizeof(*pps->row_height));
        }
        if (!pps->column_width || !pps->row_height) {
            return AVERROR(ENOMEM);
        }

        for (i = 0; i < pps->num_tile_columns; i++) {
            pps->column_width[i] = ((i + 1) * sps->ctb_width) / pps->num_tile_columns -
                                   (i * sps->ctb_width) / pps->num_tile_columns;
        }

        for (i = 0; i < pps->num_tile_rows; i++) {
            pps->row_height[i] = ((i + 1) * sps->ctb_height) / pps->num_tile_rows -
                                 (i * sps->ctb_height) / pps->num_tile_rows;
        }
    }

    pps->col_bd[0] = 0;
    for (i = 0; i < pps->num_tile_columns; i++) {
        pps->col_bd[i + 1] = pps->col_bd[i] + pps->column_width[i];
    }

    pps->row_bd[0] = 0;
    for (i = 0; i < pps->num_tile_rows; i++) {
        pps->row_bd[i + 1] = pps->row_bd[i] + pps->row_height[i];
    }

    for (i = 0, j = 0; i < sps->ctb_width; i++) {
        if (i > pps->col_bd[j]) {
            j++;
        }
        pps->col_idxX[i] = j;
    }

/**
     * 6.5
     */
    pic_area_in_ctbs = sps->ctb_width * sps->ctb_height;

    pps->ctb_addr_rs_to_ts = av_malloc_array(pic_area_in_ctbs, sizeof(*pps->ctb_addr_rs_to_ts));
    pps->ctb_addr_ts_to_rs = av_malloc_array(pic_area_in_ctbs, sizeof(*pps->ctb_addr_ts_to_rs));
    pps->tile_id = av_malloc_array(pic_area_in_ctbs, sizeof(*pps->tile_id));
    pps->min_tb_addr_zs_tab = av_malloc_array(
            (sps->tb_mask + 2) * (sps->tb_mask + 2), sizeof(*pps->min_tb_addr_zs_tab));
    if (!pps->ctb_addr_rs_to_ts || !pps->ctb_addr_ts_to_rs || !pps->tile_id ||
        !pps->min_tb_addr_zs_tab) {
        return AVERROR(ENOMEM);
    }

    for (ctb_addr_rs = 0; ctb_addr_rs < pic_area_in_ctbs; ctb_addr_rs++) {
        int tb_x = ctb_addr_rs % sps->ctb_width;
        int tb_y = ctb_addr_rs / sps->ctb_width;
        int tile_x = 0;
        int tile_y = 0;
        int val = 0;

        for (i = 0; i < pps->num_tile_columns; i++) {
            if (tb_x < pps->col_bd[i + 1]) {
                tile_x = i;
                break;
            }
        }

        for (i = 0; i < pps->num_tile_rows; i++) {
            if (tb_y < pps->row_bd[i + 1]) {
                tile_y = i;
                break;
            }
        }

        for (i = 0; i < tile_x; i++) {
            val += pps->row_height[tile_y] * pps->column_width[i];
        }
        for (i = 0; i < tile_y; i++) {
            val += sps->ctb_width * pps->row_height[i];
        }

        val += (tb_y - pps->row_bd[tile_y]) * pps->column_width[tile_x] + tb_x -
               pps->col_bd[tile_x];

        pps->ctb_addr_rs_to_ts[ctb_addr_rs] = val;
        pps->ctb_addr_ts_to_rs[val] = ctb_addr_rs;
    }

    for (j = 0, tile_id = 0; j < pps->num_tile_rows; j++) {
        for (i = 0; i < pps->num_tile_columns; i++, tile_id++) {
            for (y = pps->row_bd[j]; y < pps->row_bd[j + 1]; y++) {
                for (x = pps->col_bd[i]; x < pps->col_bd[i + 1]; x++) {
                    pps->tile_id[pps->ctb_addr_rs_to_ts[y * sps->ctb_width + x]] = tile_id;
                }
            }
        }
    }

    pps->tile_pos_rs = av_malloc_array(tile_id, sizeof(*pps->tile_pos_rs));
    if (!pps->tile_pos_rs) {
        return AVERROR(ENOMEM);
    }

    for (j = 0; j < pps->num_tile_rows; j++) {
        for (i = 0; i < pps->num_tile_columns; i++) {
            pps->tile_pos_rs[j * pps->num_tile_columns + i] =
                    pps->row_bd[j] * sps->ctb_width + pps->col_bd[i];
        }
    }

    log2_diff = sps->log2_ctb_size - sps->log2_min_tb_size;
    pps->min_tb_addr_zs = &pps->min_tb_addr_zs_tab[1 * (sps->tb_mask + 2) + 1];
    for (y = 0; y < sps->tb_mask + 2; y++) {
        pps->min_tb_addr_zs_tab[y * (sps->tb_mask + 2)] = -1;
        pps->min_tb_addr_zs_tab[y] = -1;
    }
    for (y = 0; y < sps->tb_mask + 1; y++) {
        for (x = 0; x < sps->tb_mask + 1; x++) {
            int tb_x = x >> log2_diff;
            int tb_y = y >> log2_diff;
            int rs = sps->ctb_width * tb_y + tb_x;
            int val = pps->ctb_addr_rs_to_ts[rs] << (log2_diff * 2);
            for (i = 0; i < log2_diff; i++) {
                int m = 1 << i;
                val += (m & x ? m * m : 0) + (m & y ? 2 * m * m : 0);
            }
            pps->min_tb_addr_zs[y * (sps->tb_mask + 2) + x] = val;
        }
    }

    return 0;
}

static int pps_range_extensions(GetBitContext *gb, AVCodecContext *avctx, HEVCPPS *pps, HEVCSPS *sps) {
    int i;

    if (pps->transform_skip_enabled_flag) {
        pps->log2_max_transform_skip_block_size = get_ue_golomb_long(gb) + 2;
    }
    pps->cross_component_prediction_enabled_flag = get_bits1(gb);
    pps->chroma_qp_offset_list_enabled_flag = get_bits1(gb);
    if (pps->chroma_qp_offset_list_enabled_flag) {
        pps->diff_cu_chroma_qp_offset_depth = get_ue_golomb_long(gb);
        pps->chroma_qp_offset_list_len_minus1 = get_ue_golomb_long(gb);
        if (pps->chroma_qp_offset_list_len_minus1 > 5) {
            av_log(avctx, AV_LOG_ERROR, "chroma_qp_offset_list_len_minus1 shall be in the range [0, 5].\n");
            return AVERROR_INVALIDDATA;
        }
        for (i = 0; i <= pps->chroma_qp_offset_list_len_minus1; i++) {
            pps->cb_qp_offset_list[i] = get_se_golomb_long(gb);
            if (pps->cb_qp_offset_list[i]) {
                av_log(avctx, AV_LOG_WARNING, "cb_qp_offset_list not tested yet.\n");
            }
            pps->cr_qp_offset_list[i] = get_se_golomb_long(gb);
            if (pps->cr_qp_offset_list[i]) {
                av_log(avctx, AV_LOG_WARNING, "cb_qp_offset_list not tested yet.\n");
            }
        }
    }
    pps->log2_sao_offset_scale_luma = get_ue_golomb_long(gb);
    pps->log2_sao_offset_scale_chroma = get_ue_golomb_long(gb);

    if (pps->log2_sao_offset_scale_luma > FFMAX(sps->bit_depth - 10, 0) ||
        pps->log2_sao_offset_scale_chroma > FFMAX(sps->bit_depth_chroma - 10, 0)) {
        return AVERROR_INVALIDDATA;
    }

    return (0);
}

static int decode_nal_sei_decoded_picture_hash(HEVCSEIPictureHash *s, GetBitContext *gb) {
    int cIdx, i;
    uint8_t hash_type;
//uint16_t picture_crc;
//uint32_t picture_checksum;
    hash_type = get_bits(gb, 8);

    for (cIdx = 0; cIdx < 3/*((s->sps->chroma_format_idc == 0) ? 1 : 3)*/; cIdx++) {
        if (hash_type == 0) {
            s->is_md5 = 1;
            for (i = 0; i < 16; i++) {
                s->md5[cIdx][i] = get_bits(gb, 8);
            }
        } else if (hash_type == 1) {
// picture_crc = get_bits(gb, 16);
            skip_bits(gb, 16);
        } else if (hash_type == 2) {
// picture_checksum = get_bits_long(gb, 32);
            skip_bits(gb, 32);
        }
    }
    return 0;
}

static int decode_nal_sei_mastering_display_info(HEVCSEIMasteringDisplay *s, GetBitContext *gb) {
    int i;
// Mastering primaries
    for (i = 0; i < 3; i++) {
        s->display_primaries[i][0] = get_bits(gb, 16);
        s->display_primaries[i][1] = get_bits(gb, 16);
    }
// White point (x, y)
    s->white_point[0] = get_bits(gb, 16);
    s->white_point[1] = get_bits(gb, 16);

// Max and min luminance of mastering display
    s->max_luminance = get_bits_long(gb, 32);
    s->min_luminance = get_bits_long(gb, 32);

// As this SEI message comes before the first frame that references it,
// initialize the flag to 2 and decrement on IRAP access unit so it
// persists for the coded video sequence (e.g., between two IRAPs)
    s->present = 2;
    return 0;
}

static int decode_nal_sei_content_light_info(HEVCSEIContentLight *s, GetBitContext *gb) {
// Max and average light levels
    s->max_content_light_level = get_bits_long(gb, 16);
    s->max_pic_average_light_level = get_bits_long(gb, 16);
// As this SEI message comes before the first frame that references it,
// initialize the flag to 2 and decrement on IRAP access unit so it
// persists for the coded video sequence (e.g., between two IRAPs)
    s->present = 2;
    return 0;
}

static int decode_nal_sei_frame_packing_arrangement(HEVCSEIFramePacking *s, GetBitContext *gb) {
    get_ue_golomb_long(gb);             // frame_packing_arrangement_id
    s->present = !get_bits1(gb);

    if (s->present) {
        s->arrangement_type = get_bits(gb, 7);
        s->quincunx_subsampling = get_bits1(gb);
        s->content_interpretation_type = get_bits(gb, 6);

// spatial_flipping_flag, frame0_flipped_flag, field_views_flag
        skip_bits(gb, 3);
        s->current_frame_is_frame0_flag = get_bits1(gb);
// frame0_self_contained_flag, frame1_self_contained_flag
        skip_bits(gb, 2);

        if (!s->quincunx_subsampling && s->arrangement_type != 5) {
            skip_bits(gb, 16);
        }  // frame[01]_grid_position_[xy]
        skip_bits(gb, 8);       // frame_packing_arrangement_reserved_byte
        skip_bits1(gb);         // frame_packing_arrangement_persistence_flag
    }
    skip_bits1(gb);             // upsampled_aspect_ratio_flag
    return 0;
}

static int decode_nal_sei_display_orientation(HEVCSEIDisplayOrientation *s, GetBitContext *gb) {
    s->present = !get_bits1(gb);

    if (s->present) {
        s->hflip = get_bits1(gb);     // hor_flip
        s->vflip = get_bits1(gb);     // ver_flip

        s->anticlockwise_rotation = get_bits(gb, 16);
        skip_bits1(gb);     // display_orientation_persistence_flag
    }

    return 0;
}

static int decode_nal_sei_pic_timing(HEVCSEI *s, GetBitContext *gb, const HEVCParamSets *ps, void *logctx, int size) {
    HEVCSEIPictureTiming *h = &s->picture_timing;
    HEVCSPS *sps;

    if (!ps->sps_list[s->active_seq_parameter_set_id]) {
        return (AVERROR(ENOMEM));
    }
    sps = (HEVCSPS *) ps->sps_list[s->active_seq_parameter_set_id]->data;

    if (sps->vui.frame_field_info_present_flag) {
        int pic_struct = get_bits(gb, 4);
        h->picture_struct = AV_PICTURE_STRUCTURE_UNKNOWN;
        if (pic_struct == 2 || pic_struct == 10 || pic_struct == 12) {
            av_log(logctx, AV_LOG_DEBUG, "BOTTOM Field\n");
            h->picture_struct = AV_PICTURE_STRUCTURE_BOTTOM_FIELD;
        } else if (pic_struct == 1 || pic_struct == 9 || pic_struct == 11) {
            av_log(logctx, AV_LOG_DEBUG, "TOP Field\n");
            h->picture_struct = AV_PICTURE_STRUCTURE_TOP_FIELD;
        }
        get_bits(gb, 2);                   // source_scan_type
        get_bits(gb, 1);                   // duplicate_flag
        skip_bits1(gb);
        size--;
    }
    skip_bits_long(gb, 8 * size);

    return 0;
}

static int decode_registered_user_data_closed_caption(HEVCSEIA53Caption *s, GetBitContext *gb, int size) {
    int flag;
    int user_data_type_code;
    int cc_count;

    if (size < 3) {
        return AVERROR(EINVAL);
    }

    user_data_type_code = get_bits(gb, 8);
    if (user_data_type_code == 0x3) {
        skip_bits(gb, 1); // reserved

        flag = get_bits(gb, 1); // process_cc_data_flag
        if (flag) {
            skip_bits(gb, 1);
            cc_count = get_bits(gb, 5);
            skip_bits(gb, 8); // reserved
            size -= 2;

            if (cc_count && size >= cc_count * 3) {
                const uint64_t new_size = (s->a53_caption_size + cc_count * UINT64_C(3));
                int i, ret;

                if (new_size > INT_MAX) {
                    return AVERROR(EINVAL);
                }

/* Allow merging of the cc data from two fields. */
                ret = av_reallocp(&s->a53_caption, new_size);
                if (ret < 0) {
                    return ret;
                }

                for (i = 0; i < cc_count; i++) {
                    s->a53_caption[s->a53_caption_size++] = get_bits(gb, 8);
                    s->a53_caption[s->a53_caption_size++] = get_bits(gb, 8);
                    s->a53_caption[s->a53_caption_size++] = get_bits(gb, 8);
                }
                skip_bits(gb, 8); // marker_bits
            }
        }
    } else {
        int i;
        for (i = 0; i < size - 1; i++) {
            skip_bits(gb, 8);
        }
    }

    return 0;
}

static int decode_nal_sei_user_data_registered_itu_t_t35(HEVCSEI *s, GetBitContext *gb, int size) {
    uint32_t country_code;
    uint32_t user_identifier;

    if (size < 7) {
        return AVERROR(EINVAL);
    }
    size -= 7;

    country_code = get_bits(gb, 8);
    if (country_code == 0xFF) {
        skip_bits(gb, 8);
        size--;
    }

    skip_bits(gb, 8);
    skip_bits(gb, 8);

    user_identifier = get_bits_long(gb, 32);

    switch (user_identifier) {
        case MKBETAG('G', 'A', '9', '4'):
            return decode_registered_user_data_closed_caption(&s->a53_caption, gb, size);
        default:
            skip_bits_long(gb, size * 8);
            break;
    }
    return 0;
}

static int decode_nal_sei_active_parameter_sets(HEVCSEI *s, GetBitContext *gb, void *logctx) {
    int num_sps_ids_minus1;
    int i;
    unsigned active_seq_parameter_set_id;

    get_bits(gb, 4); // active_video_parameter_set_id
    get_bits(gb, 1); // self_contained_cvs_flag
    get_bits(gb, 1); // num_sps_ids_minus1
    num_sps_ids_minus1 = get_ue_golomb_long(gb); // num_sps_ids_minus1

    if (num_sps_ids_minus1 < 0 || num_sps_ids_minus1 > 15) {
        av_log(logctx, AV_LOG_ERROR, "num_sps_ids_minus1 %d invalid\n", num_sps_ids_minus1);
        return AVERROR_INVALIDDATA;
    }

    active_seq_parameter_set_id = get_ue_golomb_long(gb);
    if (active_seq_parameter_set_id >= HEVC_MAX_SPS_COUNT) {
        av_log(logctx, AV_LOG_ERROR, "active_parameter_set_id %d invalid\n", active_seq_parameter_set_id);
        return AVERROR_INVALIDDATA;
    }
    s->active_seq_parameter_set_id = active_seq_parameter_set_id;

    for (i = 1; i <= num_sps_ids_minus1; i++) {
        get_ue_golomb_long(gb);
    } // active_seq_parameter_set_id[i]

    return 0;
}

static int decode_nal_sei_alternative_transfer(HEVCSEIAlternativeTransfer *s, GetBitContext *gb) {
    s->present = 1;
    s->preferred_transfer_characteristics = get_bits(gb, 8);
    return 0;
}

static int decode_nal_sei_prefix(GetBitContext *gb, void *logctx, HEVCSEI *s, const HEVCParamSets *ps, int type, int size) {
    switch (type) {
        case 256:  // Mismatched value from HM 8.1
            return decode_nal_sei_decoded_picture_hash(&s->picture_hash, gb);
        case HEVC_SEI_TYPE_FRAME_PACKING:
            return decode_nal_sei_frame_packing_arrangement(&s->frame_packing, gb);
        case HEVC_SEI_TYPE_DISPLAY_ORIENTATION:
            return decode_nal_sei_display_orientation(&s->display_orientation, gb);
        case HEVC_SEI_TYPE_PICTURE_TIMING:
            return decode_nal_sei_pic_timing(s, gb, ps, logctx, size);
        case HEVC_SEI_TYPE_MASTERING_DISPLAY_INFO:
            return decode_nal_sei_mastering_display_info(&s->mastering_display, gb);
        case HEVC_SEI_TYPE_CONTENT_LIGHT_LEVEL_INFO:
            return decode_nal_sei_content_light_info(&s->content_light, gb);
        case HEVC_SEI_TYPE_ACTIVE_PARAMETER_SETS:
            return decode_nal_sei_active_parameter_sets(s, gb, logctx);
        case HEVC_SEI_TYPE_USER_DATA_REGISTERED_ITU_T_T35:
            return decode_nal_sei_user_data_registered_itu_t_t35(s, gb, size);
        case HEVC_SEI_TYPE_ALTERNATIVE_TRANSFER_CHARACTERISTICS:
            return decode_nal_sei_alternative_transfer(&s->alternative_transfer, gb);
        default:
            av_log(logctx, AV_LOG_DEBUG, "Skipped PREFIX SEI %d\n", type);
            skip_bits_long(gb, 8 * size);
            return 0;
    }
}

static int decode_nal_sei_suffix(GetBitContext *gb, void *logctx, HEVCSEI *s, int type, int size) {
    switch (type) {
        case HEVC_SEI_TYPE_DECODED_PICTURE_HASH:
            return decode_nal_sei_decoded_picture_hash(&s->picture_hash, gb);
        default:
            av_log(logctx, AV_LOG_DEBUG, "Skipped SUFFIX SEI %d\n", type);
            skip_bits_long(gb, 8 * size);
            return 0;
    }
}

static int decode_nal_sei_message(GetBitContext *gb, void *logctx, HEVCSEI *s, const HEVCParamSets *ps, int nal_unit_type) {
    int payload_type = 0;
    int payload_size = 0;
    int byte = 0xFF;
    av_log(logctx, AV_LOG_DEBUG, "Decoding SEI\n");

    while (byte == 0xFF) {
        if (get_bits_left(gb) < 16 || payload_type > INT_MAX - 255) {
            return AVERROR_INVALIDDATA;
        }
        byte = get_bits(gb, 8);
        payload_type += byte;
    }
    byte = 0xFF;
    while (byte == 0xFF) {
        if (get_bits_left(gb) < 8 + 8LL * payload_size) {
            return AVERROR_INVALIDDATA;
        }
        byte = get_bits(gb, 8);
        payload_size += byte;
    }
    if (nal_unit_type == HEVC_NAL_SEI_PREFIX) {
        return decode_nal_sei_prefix(gb, logctx, s, ps, payload_type, payload_size);
    } else { /* nal_unit_type == NAL_SEI_SUFFIX */
        return decode_nal_sei_suffix(gb, logctx, s, payload_type, payload_size);
    }
}

static int more_rbsp_data(GetBitContext *gb) {
    return get_bits_left(gb) > 0 && show_bits(gb, 8) != 0x80;
}

int ff_hevc_decode_nal_sei(GetBitContext *gb, void *logctx, HEVCSEI *s, const HEVCParamSets *ps, int type) {
    int ret;

    do {
        ret = decode_nal_sei_message(gb, logctx, s, ps, type);
        if (ret < 0) {
            return ret;
        }
    } while (more_rbsp_data(gb));
    return 1;
}


AVBufferRef *av_buffer_alloc(int size) {
    AVBufferRef *ret = NULL;
    uint8_t *data = NULL;

    data = av_malloc(size);
    if (!data) {
        return NULL;
    }

    ret = av_buffer_create(data, size, av_buffer_default_free, NULL, 0);
    if (!ret) {
        av_freep(&data);
    }

    return ret;
}

AVBufferRef *av_buffer_allocz(int size) {
    AVBufferRef *ret = av_buffer_alloc(size);
    if (!ret) {
        return NULL;
    }

    memset(ret->data, 0, size);
    return ret;
}


static const uint8_t hevc_sub_width_c[] = {1, 2, 2, 1};

static const uint8_t hevc_sub_height_c[] = {1, 2, 1, 1};


int av_image_check_size(unsigned int w, unsigned int h, int log_offset, void *log_ctx) {
    return av_image_check_size2(w, h, INT64_MAX, AV_PIX_FMT_NONE, log_offset, log_ctx);
}


int ff_hevc_decode_short_term_rps(GetBitContext *gb, AVCodecContext *avctx, ShortTermRPS *rps, const HEVCSPS *sps, int is_slice_header) {
    uint8_t rps_predict = 0;
    int delta_poc;
    int k0 = 0;
    int k1 = 0;
    int k = 0;
    int i;

    if (rps != sps->st_rps && sps->nb_st_rps) {
        rps_predict = get_bits1(gb);
    }

    if (rps_predict) {
        const ShortTermRPS *rps_ridx;
        int delta_rps;
        unsigned abs_delta_rps;
        uint8_t use_delta_flag = 0;
        uint8_t delta_rps_sign;

        if (is_slice_header) {
            unsigned int delta_idx = get_ue_golomb_long(gb) + 1;
            if (delta_idx > sps->nb_st_rps) {
                av_log(avctx, AV_LOG_ERROR, "Invalid value of delta_idx in slice header RPS: %d > %d.\n", delta_idx, sps->nb_st_rps);
                return AVERROR_INVALIDDATA;
            }
            rps_ridx = &sps->st_rps[sps->nb_st_rps - delta_idx];
            rps->rps_idx_num_delta_pocs = rps_ridx->num_delta_pocs;
        } else {
            rps_ridx = &sps->st_rps[rps - sps->st_rps - 1];
        }

        delta_rps_sign = get_bits1(gb);
        abs_delta_rps = get_ue_golomb_long(gb) + 1;
        if (abs_delta_rps < 1 || abs_delta_rps > 32768) {
            av_log(avctx, AV_LOG_ERROR, "Invalid value of abs_delta_rps: %d\n", abs_delta_rps);
            return AVERROR_INVALIDDATA;
        }
        delta_rps = (1 - (delta_rps_sign << 1)) * abs_delta_rps;
        for (i = 0; i <= rps_ridx->num_delta_pocs; i++) {
            int used = rps->used[k] = get_bits1(gb);

            if (!used) {
                use_delta_flag = get_bits1(gb);
            }

            if (used || use_delta_flag) {
                if (i < rps_ridx->num_delta_pocs) {
                    delta_poc = delta_rps + rps_ridx->delta_poc[i];
                } else {
                    delta_poc = delta_rps;
                }
                rps->delta_poc[k] = delta_poc;
                if (delta_poc < 0) {
                    k0++;
                } else {
                    k1++;
                }
                k++;
            }
        }

        if (k >= FF_ARRAY_ELEMS(rps->used)) {
            av_log(avctx, AV_LOG_ERROR, "Invalid num_delta_pocs: %d\n", k);
            return AVERROR_INVALIDDATA;
        }

        rps->num_delta_pocs = k;
        rps->num_negative_pics = k0;
// sort in increasing order (smallest first)
        if (rps->num_delta_pocs != 0) {
            int used, tmp;
            for (i = 1; i < rps->num_delta_pocs; i++) {
                delta_poc = rps->delta_poc[i];
                used = rps->used[i];
                for (k = i - 1; k >= 0; k--) {
                    tmp = rps->delta_poc[k];
                    if (delta_poc < tmp) {
                        rps->delta_poc[k + 1] = tmp;
                        rps->used[k + 1] = rps->used[k];
                        rps->delta_poc[k] = delta_poc;
                        rps->used[k] = used;
                    }
                }
            }
        }
        if ((rps->num_negative_pics >> 1) != 0) {
            int used;
            k = rps->num_negative_pics - 1;
// flip the negative values to largest first
            for (i = 0; i < rps->num_negative_pics >> 1; i++) {
                delta_poc = rps->delta_poc[i];
                used = rps->used[i];
                rps->delta_poc[i] = rps->delta_poc[k];
                rps->used[i] = rps->used[k];
                rps->delta_poc[k] = delta_poc;
                rps->used[k] = used;
                k--;
            }
        }
    } else {
        unsigned int prev, nb_positive_pics;
        rps->num_negative_pics = get_ue_golomb_long(gb);
        nb_positive_pics = get_ue_golomb_long(gb);

        if (rps->num_negative_pics >= HEVC_MAX_REFS || nb_positive_pics >= HEVC_MAX_REFS) {
            av_log(avctx, AV_LOG_ERROR, "Too many refs in a short term RPS.\n");
            return AVERROR_INVALIDDATA;
        }

        rps->num_delta_pocs = rps->num_negative_pics + nb_positive_pics;
        if (rps->num_delta_pocs) {
            prev = 0;
            for (i = 0; i < rps->num_negative_pics; i++) {
                delta_poc = get_ue_golomb_long(gb) + 1;
                if (delta_poc < 1 || delta_poc > 32768) {
                    av_log(avctx, AV_LOG_ERROR, "Invalid value of delta_poc: %d\n", delta_poc);
                    return AVERROR_INVALIDDATA;
                }
                prev -= delta_poc;
                rps->delta_poc[i] = prev;
                rps->used[i] = get_bits1(gb);
            }
            prev = 0;
            for (i = 0; i < nb_positive_pics; i++) {
                delta_poc = get_ue_golomb_long(gb) + 1;
                if (delta_poc < 1 || delta_poc > 32768) {
                    av_log(avctx, AV_LOG_ERROR, "Invalid value of delta_poc: %d\n", delta_poc);
                    return AVERROR_INVALIDDATA;
                }
                prev += delta_poc;
                rps->delta_poc[rps->num_negative_pics + i] = prev;
                rps->used[rps->num_negative_pics + i] = get_bits1(gb);
            }
        }
    }
    return 0;
}


#define AV_CODEC_FLAG2_IGNORE_CROP    (1 << 16)


static const char *const color_primaries_names[AVCOL_PRI_NB] = {[AVCOL_PRI_RESERVED0] = "reserved", [AVCOL_PRI_BT709] = "bt709", [AVCOL_PRI_UNSPECIFIED] = "unknown", [AVCOL_PRI_RESERVED] = "reserved", [AVCOL_PRI_BT470M] = "bt470m", [AVCOL_PRI_BT470BG] = "bt470bg", [AVCOL_PRI_SMPTE170M] = "smpte170m", [AVCOL_PRI_SMPTE240M] = "smpte240m", [AVCOL_PRI_FILM] = "film", [AVCOL_PRI_BT2020] = "bt2020", [AVCOL_PRI_SMPTE428] = "smpte428", [AVCOL_PRI_SMPTE431] = "smpte431", [AVCOL_PRI_SMPTE432] = "smpte432", [AVCOL_PRI_EBU3213] = "ebu3213",};


static const char *const color_space_names[] = {[AVCOL_SPC_RGB] = "gbr", [AVCOL_SPC_BT709] = "bt709", [AVCOL_SPC_UNSPECIFIED] = "unknown", [AVCOL_SPC_RESERVED] = "reserved", [AVCOL_SPC_FCC] = "fcc", [AVCOL_SPC_BT470BG] = "bt470bg", [AVCOL_SPC_SMPTE170M] = "smpte170m", [AVCOL_SPC_SMPTE240M] = "smpte240m", [AVCOL_SPC_YCGCO] = "ycgco", [AVCOL_SPC_BT2020_NCL] = "bt2020nc", [AVCOL_SPC_BT2020_CL] = "bt2020c", [AVCOL_SPC_SMPTE2085] = "smpte2085", [AVCOL_SPC_CHROMA_DERIVED_NCL] = "chroma-derived-nc", [AVCOL_SPC_CHROMA_DERIVED_CL] = "chroma-derived-c", [AVCOL_SPC_ICTCP] = "ictcp",};


static const char *const color_transfer_names[] = {[AVCOL_TRC_RESERVED0] = "reserved", [AVCOL_TRC_BT709] = "bt709", [AVCOL_TRC_UNSPECIFIED] = "unknown", [AVCOL_TRC_RESERVED] = "reserved", [AVCOL_TRC_GAMMA22] = "bt470m", [AVCOL_TRC_GAMMA28] = "bt470bg", [AVCOL_TRC_SMPTE170M] = "smpte170m", [AVCOL_TRC_SMPTE240M] = "smpte240m", [AVCOL_TRC_LINEAR] = "linear", [AVCOL_TRC_LOG] = "log100", [AVCOL_TRC_LOG_SQRT] = "log316", [AVCOL_TRC_IEC61966_2_4] = "iec61966-2-4", [AVCOL_TRC_BT1361_ECG] = "bt1361e", [AVCOL_TRC_IEC61966_2_1] = "iec61966-2-1", [AVCOL_TRC_BT2020_10] = "bt2020-10", [AVCOL_TRC_BT2020_12] = "bt2020-12", [AVCOL_TRC_SMPTE2084] = "smpte2084", [AVCOL_TRC_SMPTE428] = "smpte428", [AVCOL_TRC_ARIB_STD_B67] = "arib-std-b67",};


const char *av_color_primaries_name(enum AVColorPrimaries primaries) {
    return (unsigned) primaries < AVCOL_PRI_NB ? color_primaries_names[primaries] : NULL;
}


const char *av_color_space_name(enum AVColorSpace space) {
    return (unsigned) space < AVCOL_SPC_NB ? color_space_names[space] : NULL;
}


const char *av_color_transfer_name(enum AVColorTransferCharacteristic transfer) {
    return (unsigned) transfer < AVCOL_TRC_NB ? color_transfer_names[transfer] : NULL;
}


static const AVRational vui_sar[] = {{  0  , 1}
                                     , {1  , 1}
                                     , {12 , 11}
                                     , {10 , 11}
                                     , {16 , 11}
                                     , {40 , 33}
                                     , {24 , 11}
                                     , {20 , 11}
                                     , {32 , 11}
                                     , {80 , 33}
                                     , {18 , 11}
                                     , {15 , 11}
                                     , {64 , 33}
                                     , {160, 99}
                                     , {4  , 3}
                                     , {3  , 2}
                                     , {2  , 1}
                                     ,};


static void decode_sublayer_hrd(GetBitContext *gb, unsigned int nb_cpb, int subpic_params_present) {
    int i;

    for (i = 0; i < nb_cpb; i++) {
        get_ue_golomb_long(gb); // bit_rate_value_minus1
        get_ue_golomb_long(gb); // cpb_size_value_minus1

        if (subpic_params_present) {
            get_ue_golomb_long(gb); // cpb_size_du_value_minus1
            get_ue_golomb_long(gb); // bit_rate_du_value_minus1
        }
        skip_bits1(gb); // cbr_flag
    }
}


static int decode_hrd(GetBitContext *gb, int common_inf_present, int max_sublayers) {
    int nal_params_present = 0, vcl_params_present = 0;
    int subpic_params_present = 0;
    int i;

    if (common_inf_present) {
        nal_params_present = get_bits1(gb);
        vcl_params_present = get_bits1(gb);

        if (nal_params_present || vcl_params_present) {
            subpic_params_present = get_bits1(gb);

            if (subpic_params_present) {
                skip_bits(gb, 8); // tick_divisor_minus2
                skip_bits(gb, 5); // du_cpb_removal_delay_increment_length_minus1
                skip_bits(gb, 1); // sub_pic_cpb_params_in_pic_timing_sei_flag
                skip_bits(gb, 5); // dpb_output_delay_du_length_minus1
            }

            skip_bits(gb, 4); // bit_rate_scale
            skip_bits(gb, 4); // cpb_size_scale

            if (subpic_params_present) {
                skip_bits(gb, 4);
            }  // cpb_size_du_scale

            skip_bits(gb, 5); // initial_cpb_removal_delay_length_minus1
            skip_bits(gb, 5); // au_cpb_removal_delay_length_minus1
            skip_bits(gb, 5); // dpb_output_delay_length_minus1
        }
    }

    for (i = 0; i < max_sublayers; i++) {
        int low_delay = 0;
        unsigned int nb_cpb = 1;
        int fixed_rate = get_bits1(gb);

        if (!fixed_rate) {
            fixed_rate = get_bits1(gb);
        }

        if (fixed_rate) {
            get_ue_golomb_long(gb);  // elemental_duration_in_tc_minus1
        } else {
            low_delay = get_bits1(gb);
        }

        if (!low_delay) {
            nb_cpb = get_ue_golomb_long(gb) + 1;
            if (nb_cpb < 1 || nb_cpb > 32) {
                av_log(NULL, AV_LOG_ERROR, "nb_cpb %d invalid\n", nb_cpb);
                return AVERROR_INVALIDDATA;
            }
        }

        if (nal_params_present) {
            decode_sublayer_hrd(gb, nb_cpb, subpic_params_present);
        }
        if (vcl_params_present) {
            decode_sublayer_hrd(gb, nb_cpb, subpic_params_present);
        }
    }
    return 0;
}


static void decode_vui(GetBitContext *gb, AVCodecContext *avctx, int apply_defdispwin, HEVCSPS *sps) {
    VUI backup_vui, *vui = &sps->vui;
    GetBitContext backup;
    int sar_present, alt = 0;

    av_log(avctx, AV_LOG_DEBUG, "Decoding VUI\n");

    sar_present = get_bits1(gb);
    if (sar_present) {
        uint8_t sar_idx = get_bits(gb, 8);
        if (sar_idx < FF_ARRAY_ELEMS(vui_sar)) {
            vui->sar = vui_sar[sar_idx];
        } else if (sar_idx == 255) {
            vui->sar.num = get_bits(gb, 16);
            vui->sar.den = get_bits(gb, 16);
        } else
                av_log(avctx, AV_LOG_WARNING, "Unknown SAR index: %u.\n", sar_idx) { }
    }

    vui->overscan_info_present_flag = get_bits1(gb);
    if (vui->overscan_info_present_flag) {
        vui->overscan_appropriate_flag = get_bits1(gb);
    }

    vui->video_signal_type_present_flag = get_bits1(gb);
    if (vui->video_signal_type_present_flag) {
        vui->video_format = get_bits(gb, 3);
        vui->video_full_range_flag = get_bits1(gb);
        vui->colour_description_present_flag = get_bits1(gb);
        if (vui->video_full_range_flag && sps->pix_fmt == AV_PIX_FMT_YUV420P) {
            sps->pix_fmt = AV_PIX_FMT_YUVJ420P;
        }
        if (vui->colour_description_present_flag) {
            vui->colour_primaries = get_bits(gb, 8);
            vui->transfer_characteristic = get_bits(gb, 8);
            vui->matrix_coeffs = get_bits(gb, 8);

// Set invalid values to "unspecified"
            if (!av_color_primaries_name(vui->colour_primaries)) {
                vui->colour_primaries = AVCOL_PRI_UNSPECIFIED;
            }
            if (!av_color_transfer_name(vui->transfer_characteristic)) {
                vui->transfer_characteristic = AVCOL_TRC_UNSPECIFIED;
            }
            if (!av_color_space_name(vui->matrix_coeffs)) {
                vui->matrix_coeffs = AVCOL_SPC_UNSPECIFIED;
            }
            if (vui->matrix_coeffs == AVCOL_SPC_RGB) {
                switch (sps->pix_fmt) {
                    case AV_PIX_FMT_YUV444P:
                        sps->pix_fmt = AV_PIX_FMT_GBRP;
                        break;
                    case AV_PIX_FMT_YUV444P10:
                        sps->pix_fmt = AV_PIX_FMT_GBRP10;
                        break;
                    case AV_PIX_FMT_YUV444P12:
                        sps->pix_fmt = AV_PIX_FMT_GBRP12;
                        break;
                }
            }
        }
    }

    vui->chroma_loc_info_present_flag = get_bits1(gb);
    if (vui->chroma_loc_info_present_flag) {
        vui->chroma_sample_loc_type_top_field = get_ue_golomb_long(gb);
        vui->chroma_sample_loc_type_bottom_field = get_ue_golomb_long(gb);
    }

    vui->neutra_chroma_indication_flag = get_bits1(gb);
    vui->field_seq_flag = get_bits1(gb);
    vui->frame_field_info_present_flag = get_bits1(gb);

// Backup context in case an alternate header is detected
    memcpy(&backup, gb, sizeof(backup));
    memcpy(&backup_vui, vui, sizeof(backup_vui));
    if (get_bits_left(gb) >= 68 && show_bits_long(gb, 21) == 0x100000) {
        vui->default_display_window_flag = 0;
        av_log(avctx, AV_LOG_WARNING, "Invalid default display window\n");
    } else {
        vui->default_display_window_flag = get_bits1(gb);
    }

    if (vui->default_display_window_flag) {
        int vert_mult = hevc_sub_height_c[sps->chroma_format_idc];
        int horiz_mult = hevc_sub_width_c[sps->chroma_format_idc];
        vui->def_disp_win.left_offset = get_ue_golomb_long(gb) * horiz_mult;
        vui->def_disp_win.right_offset = get_ue_golomb_long(gb) * horiz_mult;
        vui->def_disp_win.top_offset = get_ue_golomb_long(gb) * vert_mult;
        vui->def_disp_win.bottom_offset = get_ue_golomb_long(gb) * vert_mult;

        if (apply_defdispwin && avctx->flags2 & AV_CODEC_FLAG2_IGNORE_CROP) {
            av_log(avctx, AV_LOG_DEBUG, "discarding vui default display window, "
                                        "original values are l:%u r:%u t:%u b:%u\n", vui->def_disp_win.left_offset, vui->def_disp_win.right_offset, vui->def_disp_win.top_offset, vui->def_disp_win.bottom_offset);

            vui->def_disp_win.left_offset = vui->def_disp_win.right_offset = vui->def_disp_win.top_offset = vui->def_disp_win.bottom_offset = 0;
        }
    }

    timing_info:
    vui->vui_timing_info_present_flag = get_bits1(gb);

    if (vui->vui_timing_info_present_flag) {
        if (get_bits_left(gb) < 66 && !alt) {
// The alternate syntax seem to have timing info located
// at where def_disp_win is normally located
            av_log(avctx, AV_LOG_WARNING, "Strange VUI timing information, retrying...\n");
            memcpy(vui, &backup_vui, sizeof(backup_vui));
            memcpy(gb, &backup, sizeof(backup));
            alt = 1;
            goto timing_info;
        }
        vui->vui_num_units_in_tick = get_bits_long(gb, 32);
        vui->vui_time_scale = get_bits_long(gb, 32);
        if (alt) {
            av_log(avctx, AV_LOG_INFO, "Retry got %"PRIu32"/%"PRIu32" fps\n", vui->vui_time_scale, vui->vui_num_units_in_tick);
        }
        vui->vui_poc_proportional_to_timing_flag = get_bits1(gb);
        if (vui->vui_poc_proportional_to_timing_flag) {
            vui->vui_num_ticks_poc_diff_one_minus1 = get_ue_golomb_long(gb);
        }
        vui->vui_hrd_parameters_present_flag = get_bits1(gb);
        if (vui->vui_hrd_parameters_present_flag) {
            decode_hrd(gb, 1, sps->max_sub_layers);
        }
    }

    vui->bitstream_restriction_flag = get_bits1(gb);
    if (vui->bitstream_restriction_flag) {
        if (get_bits_left(gb) < 8 && !alt) {
            av_log(avctx, AV_LOG_WARNING, "Strange VUI bitstream restriction information, retrying"
                                          " from timing information...\n");
            memcpy(vui, &backup_vui, sizeof(backup_vui));
            memcpy(gb, &backup, sizeof(backup));
            alt = 1;
            goto timing_info;
        }
        vui->tiles_fixed_structure_flag = get_bits1(gb);
        vui->motion_vectors_over_pic_boundaries_flag = get_bits1(gb);
        vui->restricted_ref_pic_lists_flag = get_bits1(gb);
        vui->min_spatial_segmentation_idc = get_ue_golomb_long(gb);
        vui->max_bytes_per_pic_denom = get_ue_golomb_long(gb);
        vui->max_bits_per_min_cu_denom = get_ue_golomb_long(gb);
        vui->log2_max_mv_length_horizontal = get_ue_golomb_long(gb);
        vui->log2_max_mv_length_vertical = get_ue_golomb_long(gb);
    }

    if (get_bits_left(gb) < 1 && !alt) {
// XXX: Alternate syntax when sps_range_extension_flag != 0?
        av_log(avctx, AV_LOG_WARNING, "Overread in VUI, retrying from timing information...\n");
        memcpy(vui, &backup_vui, sizeof(backup_vui));
        memcpy(gb, &backup, sizeof(backup));
        alt = 1;
        goto timing_info;
    }
}

typedef struct ImgUtils {
    const AVClass *class;
    int log_offset;
    void *log_ctx;
} ImgUtils;

int av_image_check_size2(unsigned int w, unsigned int h, int64_t max_pixels, enum AVPixelFormat pix_fmt, int log_offset, void *log_ctx) {
    ImgUtils imgutils = {
//.class      = &imgutils_class,
            .log_offset = log_offset, .log_ctx    = log_ctx,};
    int64_t stride = av_image_get_linesize(pix_fmt, w, 0);
    if (stride <= 0) {
        stride = 8LL * w;
    }
    stride += 128 * 8;

    if ((int) w <= 0 || (int) h <= 0 || stride >= INT_MAX ||
        stride * (uint64_t) (h + 128) >= INT_MAX) {
        av_log(&imgutils, AV_LOG_ERROR, "Picture size %ux%u is invalid\n", w, h);
        return AVERROR(EINVAL);
    }

    if (max_pixels < INT64_MAX) {
        if (w * (int64_t) h > max_pixels) {
            av_log(&imgutils, AV_LOG_ERROR, "Picture size %ux%u exceeds specified max pixel count %"PRId64", see the documentation if you wish to increase it\n", w, h, max_pixels);
            return AVERROR(EINVAL);
        }
    }

    return 0;
}


static int decode_profile_tier_level(GetBitContext *gb, AVCodecContext *avctx, PTLCommon *ptl) {
    int i;

    if (get_bits_left(gb) < 2 + 1 + 5 + 32 + 4 + 16 + 16 + 12) {
        return -1;
    }

    ptl->profile_space = get_bits(gb, 2);
    ptl->tier_flag = get_bits1(gb);
    ptl->profile_idc = get_bits(gb, 5);
    if (ptl->profile_idc == FF_PROFILE_HEVC_MAIN)
            av_log(avctx, AV_LOG_DEBUG, "Main profile bitstream\n") { }
    else if (ptl->profile_idc == FF_PROFILE_HEVC_MAIN_10)
            av_log(avctx, AV_LOG_DEBUG, "Main 10 profile bitstream\n") { }
    else if (ptl->profile_idc == FF_PROFILE_HEVC_MAIN_STILL_PICTURE)
            av_log(avctx, AV_LOG_DEBUG, "Main Still Picture profile bitstream\n") { }
    else if (ptl->profile_idc == FF_PROFILE_HEVC_REXT)
            av_log(avctx, AV_LOG_DEBUG, "Range Extension profile bitstream\n") { }
    else
            av_log(avctx, AV_LOG_WARNING, "Unknown HEVC profile: %d\n", ptl->profile_idc) { }

    for (i = 0; i < 32; i++) {
        ptl->profile_compatibility_flag[i] = get_bits1(gb);

        if (ptl->profile_idc == 0 && i > 0 && ptl->profile_compatibility_flag[i]) {
            ptl->profile_idc = i;
        }
    }
    ptl->progressive_source_flag = get_bits1(gb);
    ptl->interlaced_source_flag = get_bits1(gb);
    ptl->non_packed_constraint_flag = get_bits1(gb);
    ptl->frame_only_constraint_flag = get_bits1(gb);

    skip_bits(gb, 16); // XXX_reserved_zero_44bits[0..15]
    skip_bits(gb, 16); // XXX_reserved_zero_44bits[16..31]
    skip_bits(gb, 12); // XXX_reserved_zero_44bits[32..43]

    return 0;
}

static int parse_ptl(GetBitContext *gb, AVCodecContext *avctx, PTL *ptl, int max_num_sub_layers) {
    int i;
    if (decode_profile_tier_level(gb, avctx, &ptl->general_ptl) < 0 ||
        get_bits_left(gb) < 8 + (8 * 2 * (max_num_sub_layers - 1 > 0))) {
        av_log(avctx, AV_LOG_ERROR, "PTL information too short\n");
        return -1;
    }

    ptl->general_ptl.level_idc = get_bits(gb, 8);

    for (i = 0; i < max_num_sub_layers - 1; i++) {
        ptl->sub_layer_profile_present_flag[i] = get_bits1(gb);
        ptl->sub_layer_level_present_flag[i] = get_bits1(gb);
    }

    if (max_num_sub_layers - 1 > 0) {
        for (i = max_num_sub_layers - 1; i < 8; i++) {
            skip_bits(gb, 2);
        }
    } // reserved_zero_2bits[i]
    for (i = 0; i < max_num_sub_layers - 1; i++) {
        if (ptl->sub_layer_profile_present_flag[i] &&
            decode_profile_tier_level(gb, avctx, &ptl->sub_layer_ptl[i]) < 0) {
            av_log(avctx, AV_LOG_ERROR, "PTL information for sublayer %i too short\n", i);
            return -1;
        }
        if (ptl->sub_layer_level_present_flag[i]) {
            if (get_bits_left(gb) < 8) {
                av_log(avctx, AV_LOG_ERROR, "Not enough data for sublayer %i level_idc\n", i);
                return -1;
            } else {
                ptl->sub_layer_ptl[i].level_idc = get_bits(gb, 8);
            }
        }
    }

    return 0;
}


const AVPixFmtDescriptor *av_pix_fmt_desc_get(enum AVPixelFormat pix_fmt) {
    if (pix_fmt < 0 || pix_fmt >= AV_PIX_FMT_NB) {
        return NULL;
    }
    return &av_pix_fmt_descriptors[pix_fmt];
}


static int map_pixel_format(AVCodecContext *avctx, HEVCSPS *sps) {
    const AVPixFmtDescriptor *desc;
    switch (sps->bit_depth) {
        case 8:
            if (sps->chroma_format_idc == 0) {
                sps->pix_fmt = AV_PIX_FMT_GRAY8;
            }
            if (sps->chroma_format_idc == 1) {
                sps->pix_fmt = AV_PIX_FMT_YUV420P;
            }
            if (sps->chroma_format_idc == 2) {
                sps->pix_fmt = AV_PIX_FMT_YUV422P;
            }
            if (sps->chroma_format_idc == 3) {
                sps->pix_fmt = AV_PIX_FMT_YUV444P;
            }
            break;
        case 9:
            if (sps->chroma_format_idc == 0) {
                sps->pix_fmt = AV_PIX_FMT_GRAY9;
            }
            if (sps->chroma_format_idc == 1) {
                sps->pix_fmt = AV_PIX_FMT_YUV420P9;
            }
            if (sps->chroma_format_idc == 2) {
                sps->pix_fmt = AV_PIX_FMT_YUV422P9;
            }
            if (sps->chroma_format_idc == 3) {
                sps->pix_fmt = AV_PIX_FMT_YUV444P9;
            }
            break;
        case 10:
            if (sps->chroma_format_idc == 0) {
                sps->pix_fmt = AV_PIX_FMT_GRAY10;
            }
            if (sps->chroma_format_idc == 1) {
                sps->pix_fmt = AV_PIX_FMT_YUV420P10;
            }
            if (sps->chroma_format_idc == 2) {
                sps->pix_fmt = AV_PIX_FMT_YUV422P10;
            }
            if (sps->chroma_format_idc == 3) {
                sps->pix_fmt = AV_PIX_FMT_YUV444P10;
            }
            break;
        case 12:
            if (sps->chroma_format_idc == 0) {
                sps->pix_fmt = AV_PIX_FMT_GRAY12;
            }
            if (sps->chroma_format_idc == 1) {
                sps->pix_fmt = AV_PIX_FMT_YUV420P12;
            }
            if (sps->chroma_format_idc == 2) {
                sps->pix_fmt = AV_PIX_FMT_YUV422P12;
            }
            if (sps->chroma_format_idc == 3) {
                sps->pix_fmt = AV_PIX_FMT_YUV444P12;
            }
            break;
        default:
            av_log(avctx, AV_LOG_ERROR, "The following bit-depths are currently specified: 8, 9, 10 and 12 bits, "
                                        "chroma_format_idc is %d, depth is %d\n", sps->chroma_format_idc, sps->bit_depth);
            return AVERROR_INVALIDDATA;
    }

    desc = av_pix_fmt_desc_get(sps->pix_fmt);
    if (!desc) {
        return AVERROR(EINVAL);
    }

    sps->hshift[0] = sps->vshift[0] = 0;
    sps->hshift[2] = sps->hshift[1] = desc->log2_chroma_w;
    sps->vshift[2] = sps->vshift[1] = desc->log2_chroma_h;

    sps->pixel_shift = sps->bit_depth > 8;

    return 0;
}


static const uint8_t default_scaling_list_intra[] = {16, 16, 16, 16, 17, 18, 21, 24, 16, 16, 16, 16, 17, 19, 22, 25, 16, 16, 17, 18, 20, 22, 25, 29, 16, 16, 18, 21, 24, 27, 31, 36, 17, 17, 20, 24, 30, 35, 41, 47, 18, 19, 22, 27, 35, 44, 54, 65, 21, 22, 25, 31, 41, 54, 70, 88, 24, 25, 29, 36, 47, 65, 88, 115};

static const uint8_t default_scaling_list_inter[] = {16, 16, 16, 16, 17, 18, 20, 24, 16, 16, 16, 17, 18, 20, 24, 25, 16, 16, 17, 18, 20, 24, 25, 28, 16, 17, 18, 20, 24, 25, 28, 33, 17, 18, 20, 24, 25, 28, 33, 41, 18, 20, 24, 25, 28, 33, 41, 54, 20, 24, 25, 28, 33, 41, 54, 71, 24, 25, 28, 33, 41, 54, 71, 91};


static void set_default_scaling_list_data(ScalingList *sl) {
    int matrixId;

    for (matrixId = 0; matrixId < 6; matrixId++) {
// 4x4 default is 16
        memset(sl->sl[0][matrixId], 16, 16);
        sl->sl_dc[0][matrixId] = 16; // default for 16x16
        sl->sl_dc[1][matrixId] = 16; // default for 32x32
    }
    memcpy(sl->sl[1][0], default_scaling_list_intra, 64);
    memcpy(sl->sl[1][1], default_scaling_list_intra, 64);
    memcpy(sl->sl[1][2], default_scaling_list_intra, 64);
    memcpy(sl->sl[1][3], default_scaling_list_inter, 64);
    memcpy(sl->sl[1][4], default_scaling_list_inter, 64);
    memcpy(sl->sl[1][5], default_scaling_list_inter, 64);
    memcpy(sl->sl[2][0], default_scaling_list_intra, 64);
    memcpy(sl->sl[2][1], default_scaling_list_intra, 64);
    memcpy(sl->sl[2][2], default_scaling_list_intra, 64);
    memcpy(sl->sl[2][3], default_scaling_list_inter, 64);
    memcpy(sl->sl[2][4], default_scaling_list_inter, 64);
    memcpy(sl->sl[2][5], default_scaling_list_inter, 64);
    memcpy(sl->sl[3][0], default_scaling_list_intra, 64);
    memcpy(sl->sl[3][1], default_scaling_list_intra, 64);
    memcpy(sl->sl[3][2], default_scaling_list_intra, 64);
    memcpy(sl->sl[3][3], default_scaling_list_inter, 64);
    memcpy(sl->sl[3][4], default_scaling_list_inter, 64);
    memcpy(sl->sl[3][5], default_scaling_list_inter, 64);
}


const uint8_t ff_hevc_diag_scan4x4_x[16] = {0, 0, 1, 0, 1, 2, 0, 1, 2, 3, 1, 2, 3, 2, 3, 3,};

const uint8_t ff_hevc_diag_scan4x4_y[16] = {0, 1, 0, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 3, 2, 3,};

const uint8_t ff_hevc_diag_scan8x8_x[64] = {0, 0, 1, 0, 1, 2, 0, 1, 2, 3, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 7, 3, 4, 5, 6, 7, 4, 5, 6, 7, 5, 6, 7, 6, 7, 7,};


const uint8_t ff_hevc_diag_scan8x8_y[64] = {0, 1, 0, 2, 1, 0, 3, 2, 1, 0, 4, 3, 2, 1, 0, 5, 4, 3, 2, 1, 0, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 7, 6, 5, 4, 3, 2, 7, 6, 5, 4, 3, 7, 6, 5, 4, 7, 6, 5, 7, 6, 7,};

static int scaling_list_data(GetBitContext *gb, AVCodecContext *avctx, ScalingList *sl, HEVCSPS *sps) {
    uint8_t scaling_list_pred_mode_flag;
    int32_t scaling_list_dc_coef[2][6];
    int size_id, matrix_id, pos;
    int i;

    for (size_id = 0; size_id < 4; size_id++) {
        for (matrix_id = 0; matrix_id < 6; matrix_id += ((size_id == 3) ? 3 : 1)) {
            scaling_list_pred_mode_flag = get_bits1(gb);
            if (!scaling_list_pred_mode_flag) {
                unsigned int delta = get_ue_golomb_long(gb);
/* Only need to handle non-zero delta. Zero means default,
                 * which should already be in the arrays. */
                if (delta) {
// Copy from previous array.
                    delta *= (size_id == 3) ? 3 : 1;
                    if (matrix_id < delta) {
                        av_log(avctx, AV_LOG_ERROR, "Invalid delta in scaling list data: %d.\n", delta);
                        return AVERROR_INVALIDDATA;
                    }

                    memcpy(sl->sl[size_id][matrix_id], sl->sl[size_id][matrix_id - delta],
                           size_id > 0 ? 64 : 16);
                    if (size_id > 1) {
                        sl->sl_dc[size_id - 2][matrix_id] = sl->sl_dc[size_id - 2][matrix_id -
                                                                                   delta];
                    }
                }
            } else {
                int next_coef, coef_num;
                int32_t scaling_list_delta_coef;

                next_coef = 8;
                coef_num = FFMIN(64, 1 << (4 + (size_id << 1)));
                if (size_id > 1) {
                    scaling_list_dc_coef[size_id - 2][matrix_id] = get_se_golomb(gb) + 8;
                    next_coef = scaling_list_dc_coef[size_id - 2][matrix_id];
                    sl->sl_dc[size_id - 2][matrix_id] = next_coef;
                }
                for (i = 0; i < coef_num; i++) {
                    if (size_id == 0) {
                        pos = 4 * ff_hevc_diag_scan4x4_y[i] + ff_hevc_diag_scan4x4_x[i];
                    } else {
                        pos = 8 * ff_hevc_diag_scan8x8_y[i] + ff_hevc_diag_scan8x8_x[i];
                    }

                    scaling_list_delta_coef = get_se_golomb(gb);
                    next_coef = (next_coef + 256U + scaling_list_delta_coef) % 256;
                    sl->sl[size_id][matrix_id][pos] = next_coef;
                }
            }
        }
    }

    if (sps->chroma_format_idc == 3) {
        for (i = 0; i < 64; i++) {
            sl->sl[3][1][i] = sl->sl[2][1][i];
            sl->sl[3][2][i] = sl->sl[2][2][i];
            sl->sl[3][4][i] = sl->sl[2][4][i];
            sl->sl[3][5][i] = sl->sl[2][5][i];
        }
        sl->sl_dc[1][1] = sl->sl_dc[0][1];
        sl->sl_dc[1][2] = sl->sl_dc[0][2];
        sl->sl_dc[1][4] = sl->sl_dc[0][4];
        sl->sl_dc[1][5] = sl->sl_dc[0][5];
    }


    return 0;
}

static void missing_feature_sample(int sample, void *avc, const char *msg, va_list argument_list) {
//    av_vlog(avc, AV_LOG_WARNING, msg, argument_list);
//    av_log(avc, AV_LOG_WARNING, " is not implemented. Update your FFmpeg "
//           "version to the newest one from Git. If the problem still "
//           "occurs, it means that your file has a feature which has not "
//           "been implemented.\n");
//    if (sample)
//        av_log(avc, AV_LOG_WARNING, "If you want to help, upload a sample "
//               "of this file to ftp://upload.ffmpeg.org/incoming/ "
//               "and contact the ffmpeg-devel mailing list. (ffmpeg-devel@ffmpeg.org)\n");
}

void avpriv_request_sample(void *avc, const char *msg, ...) {
    va_list argument_list;

    va_start(argument_list, msg);
    missing_feature_sample(1, avc, msg, argument_list);
    va_end(argument_list);
}


int ff_hevc_parse_sps(HEVCSPS *sps, GetBitContext *gb, unsigned int *sps_id, int apply_defdispwin, AVBufferRef **vps_list, AVCodecContext *avctx) {
    HEVCWindow *ow;
    int ret = 0;
    int log2_diff_max_min_transform_block_size;
    int bit_depth_chroma, start, vui_present, sublayer_ordering_info;
    int i;

// Coded parameters

    sps->vps_id = get_bits(gb, 4);
    if (sps->vps_id >= HEVC_MAX_VPS_COUNT) {
        av_log(avctx, AV_LOG_ERROR, "VPS id out of range: %d\n", sps->vps_id);
        return AVERROR_INVALIDDATA;
    }

    if (vps_list && !vps_list[sps->vps_id]) {
        av_log(avctx, AV_LOG_ERROR, "VPS %d does not exist\n", sps->vps_id);
        return AVERROR_INVALIDDATA;
    }

    sps->max_sub_layers = get_bits(gb, 3) + 1;
    if (sps->max_sub_layers > HEVC_MAX_SUB_LAYERS) {
        av_log(avctx, AV_LOG_ERROR, "sps_max_sub_layers out of range: %d\n", sps->max_sub_layers);
        return AVERROR_INVALIDDATA;
    }

    sps->temporal_id_nesting_flag = get_bits(gb, 1);

    if ((ret = parse_ptl(gb, avctx, &sps->ptl, sps->max_sub_layers)) < 0) {
        return ret;
    }

    *sps_id = get_ue_golomb_long(gb);
    if (*sps_id >= HEVC_MAX_SPS_COUNT) {
        av_log(avctx, AV_LOG_ERROR, "SPS id out of range: %d\n", *sps_id);
        return AVERROR_INVALIDDATA;
    }

    sps->chroma_format_idc = get_ue_golomb_long(gb);
    if (sps->chroma_format_idc > 3U) {
        av_log(avctx, AV_LOG_ERROR, "chroma_format_idc %d is invalid\n", sps->chroma_format_idc);
        return AVERROR_INVALIDDATA;
    }

    if (sps->chroma_format_idc == 3) {
        sps->separate_colour_plane_flag = get_bits1(gb);
    }

    if (sps->separate_colour_plane_flag) {
        sps->chroma_format_idc = 0;
    }

    sps->width = get_ue_golomb_long(gb);
    sps->height = get_ue_golomb_long(gb);
    if ((ret = av_image_check_size(sps->width, sps->height, 0, avctx)) < 0) {
        return ret;
    }

    if (get_bits1(gb)) { // pic_conformance_flag
        int vert_mult = hevc_sub_height_c[sps->chroma_format_idc];
        int horiz_mult = hevc_sub_width_c[sps->chroma_format_idc];
        sps->pic_conf_win.left_offset = get_ue_golomb_long(gb) * horiz_mult;
        sps->pic_conf_win.right_offset = get_ue_golomb_long(gb) * horiz_mult;
        sps->pic_conf_win.top_offset = get_ue_golomb_long(gb) * vert_mult;
        sps->pic_conf_win.bottom_offset = get_ue_golomb_long(gb) * vert_mult;

        if (avctx->flags2 & AV_CODEC_FLAG2_IGNORE_CROP) {
            av_log(avctx, AV_LOG_DEBUG, "discarding sps conformance window, "
                                        "original values are l:%u r:%u t:%u b:%u\n", sps->pic_conf_win.left_offset, sps->pic_conf_win.right_offset, sps->pic_conf_win.top_offset, sps->pic_conf_win.bottom_offset);

            sps->pic_conf_win.left_offset = sps->pic_conf_win.right_offset = sps->pic_conf_win.top_offset = sps->pic_conf_win.bottom_offset = 0;
        }
        sps->output_window = sps->pic_conf_win;
    }

    sps->bit_depth = get_ue_golomb_long(gb) + 8;
    bit_depth_chroma = get_ue_golomb_long(gb) + 8;
    if (sps->chroma_format_idc && bit_depth_chroma != sps->bit_depth) {
        av_log(avctx, AV_LOG_ERROR, "Luma bit depth (%d) is different from chroma bit depth (%d), "
                                    "this is unsupported.\n", sps->bit_depth, bit_depth_chroma);
        return AVERROR_INVALIDDATA;
    }
    sps->bit_depth_chroma = bit_depth_chroma;

    ret = map_pixel_format(avctx, sps);
    if (ret < 0) {
        return ret;
    }

    sps->log2_max_poc_lsb = get_ue_golomb_long(gb) + 4;
    if (sps->log2_max_poc_lsb > 16) {
        av_log(avctx, AV_LOG_ERROR, "log2_max_pic_order_cnt_lsb_minus4 out range: %d\n",
               sps->log2_max_poc_lsb - 4);
        return AVERROR_INVALIDDATA;
    }

    sublayer_ordering_info = get_bits1(gb);
    start = sublayer_ordering_info ? 0 : sps->max_sub_layers - 1;
    for (i = start; i < sps->max_sub_layers; i++) {
        sps->temporal_layer[i].max_dec_pic_buffering = get_ue_golomb_long(gb) + 1;
        sps->temporal_layer[i].num_reorder_pics = get_ue_golomb_long(gb);
        sps->temporal_layer[i].max_latency_increase = get_ue_golomb_long(gb) - 1;
        if (sps->temporal_layer[i].max_dec_pic_buffering > (unsigned) HEVC_MAX_DPB_SIZE) {
            av_log(avctx, AV_LOG_ERROR, "sps_max_dec_pic_buffering_minus1 out of range: %d\n",
                   sps->temporal_layer[i].max_dec_pic_buffering - 1U);
            return AVERROR_INVALIDDATA;
        }
        if (sps->temporal_layer[i].num_reorder_pics >
            sps->temporal_layer[i].max_dec_pic_buffering - 1) {
            av_log(avctx, AV_LOG_WARNING, "sps_max_num_reorder_pics out of range: %d\n", sps->temporal_layer[i].num_reorder_pics);
            if (avctx->err_recognition & AV_EF_EXPLODE ||
                sps->temporal_layer[i].num_reorder_pics > HEVC_MAX_DPB_SIZE - 1) {
                return AVERROR_INVALIDDATA;
            }
            sps->temporal_layer[i].max_dec_pic_buffering =
                    sps->temporal_layer[i].num_reorder_pics + 1;
        }
    }

    if (!sublayer_ordering_info) {
        for (i = 0; i < start; i++) {
            sps->temporal_layer[i].max_dec_pic_buffering = sps->temporal_layer[start].max_dec_pic_buffering;
            sps->temporal_layer[i].num_reorder_pics = sps->temporal_layer[start].num_reorder_pics;
            sps->temporal_layer[i].max_latency_increase = sps->temporal_layer[start].max_latency_increase;
        }
    }

    sps->log2_min_cb_size = get_ue_golomb_long(gb) + 3;
    sps->log2_diff_max_min_coding_block_size = get_ue_golomb_long(gb);
    sps->log2_min_tb_size = get_ue_golomb_long(gb) + 2;
    log2_diff_max_min_transform_block_size = get_ue_golomb_long(gb);
    sps->log2_max_trafo_size = log2_diff_max_min_transform_block_size + sps->log2_min_tb_size;

    if (sps->log2_min_cb_size < 3 || sps->log2_min_cb_size > 30) {
        av_log(avctx, AV_LOG_ERROR, "Invalid value %d for log2_min_cb_size", sps->log2_min_cb_size);
        return AVERROR_INVALIDDATA;
    }

    if (sps->log2_diff_max_min_coding_block_size > 30) {
        av_log(avctx, AV_LOG_ERROR, "Invalid value %d for log2_diff_max_min_coding_block_size", sps->log2_diff_max_min_coding_block_size);
        return AVERROR_INVALIDDATA;
    }

    if (sps->log2_min_tb_size >= sps->log2_min_cb_size || sps->log2_min_tb_size < 2) {
        av_log(avctx, AV_LOG_ERROR, "Invalid value for log2_min_tb_size");
        return AVERROR_INVALIDDATA;
    }

    if (log2_diff_max_min_transform_block_size < 0 || log2_diff_max_min_transform_block_size > 30) {
        av_log(avctx, AV_LOG_ERROR, "Invalid value %d for log2_diff_max_min_transform_block_size", log2_diff_max_min_transform_block_size);
        return AVERROR_INVALIDDATA;
    }

    sps->max_transform_hierarchy_depth_inter = get_ue_golomb_long(gb);
    sps->max_transform_hierarchy_depth_intra = get_ue_golomb_long(gb);

    sps->scaling_list_enable_flag = get_bits1(gb);
    if (sps->scaling_list_enable_flag) {
        set_default_scaling_list_data(&sps->scaling_list);

        if (get_bits1(gb)) {
            ret = scaling_list_data(gb, avctx, &sps->scaling_list, sps);
            if (ret < 0) {
                return ret;
            }
        }
    }

    sps->amp_enabled_flag = get_bits1(gb);
    sps->sao_enabled = get_bits1(gb);

    sps->pcm_enabled_flag = get_bits1(gb);
    if (sps->pcm_enabled_flag) {
        sps->pcm.bit_depth = get_bits(gb, 4) + 1;
        sps->pcm.bit_depth_chroma = get_bits(gb, 4) + 1;
        sps->pcm.log2_min_pcm_cb_size = get_ue_golomb_long(gb) + 3;
        sps->pcm.log2_max_pcm_cb_size = sps->pcm.log2_min_pcm_cb_size + get_ue_golomb_long(gb);
        if (FFMAX(sps->pcm.bit_depth, sps->pcm.bit_depth_chroma) > sps->bit_depth) {
            av_log(avctx, AV_LOG_ERROR, "PCM bit depth (%d, %d) is greater than normal bit depth (%d)\n", sps->pcm.bit_depth, sps->pcm.bit_depth_chroma, sps->bit_depth);
            return AVERROR_INVALIDDATA;
        }

        sps->pcm.loop_filter_disable_flag = get_bits1(gb);
    }

    sps->nb_st_rps = get_ue_golomb_long(gb);
    if (sps->nb_st_rps > HEVC_MAX_SHORT_TERM_REF_PIC_SETS) {
        av_log(avctx, AV_LOG_ERROR, "Too many short term RPS: %d.\n", sps->nb_st_rps);
        return AVERROR_INVALIDDATA;
    }
    for (i = 0; i < sps->nb_st_rps; i++) {
        if ((ret = ff_hevc_decode_short_term_rps(gb, avctx, &sps->st_rps[i], sps, 0)) < 0) {
            return ret;
        }
    }

    sps->long_term_ref_pics_present_flag = get_bits1(gb);
    if (sps->long_term_ref_pics_present_flag) {
        sps->num_long_term_ref_pics_sps = get_ue_golomb_long(gb);
        if (sps->num_long_term_ref_pics_sps > HEVC_MAX_LONG_TERM_REF_PICS) {
            av_log(avctx, AV_LOG_ERROR, "Too many long term ref pics: %d.\n", sps->num_long_term_ref_pics_sps);
            return AVERROR_INVALIDDATA;
        }
        for (i = 0; i < sps->num_long_term_ref_pics_sps; i++) {
            sps->lt_ref_pic_poc_lsb_sps[i] = get_bits(gb, sps->log2_max_poc_lsb);
            sps->used_by_curr_pic_lt_sps_flag[i] = get_bits1(gb);
        }
    }

    sps->sps_temporal_mvp_enabled_flag = get_bits1(gb);
    sps->sps_strong_intra_smoothing_enable_flag = get_bits1(gb);
    sps->vui.sar = (AVRational) {0, 1};
    vui_present = get_bits1(gb);
    if (vui_present) {
        decode_vui(gb, avctx, apply_defdispwin, sps);
    }

    if (get_bits1(gb)) { // sps_extension_flag
        sps->sps_range_extension_flag = get_bits1(gb);
        skip_bits(gb, 7); //sps_extension_7bits = get_bits(gb, 7);
        if (sps->sps_range_extension_flag) {
            sps->transform_skip_rotation_enabled_flag = get_bits1(gb);
            sps->transform_skip_context_enabled_flag = get_bits1(gb);
            sps->implicit_rdpcm_enabled_flag = get_bits1(gb);

            sps->explicit_rdpcm_enabled_flag = get_bits1(gb);

            sps->extended_precision_processing_flag = get_bits1(gb);
            if (sps->extended_precision_processing_flag)
                    av_log(avctx, AV_LOG_WARNING, "extended_precision_processing_flag not yet implemented\n") { }

            sps->intra_smoothing_disabled_flag = get_bits1(gb);
            sps->high_precision_offsets_enabled_flag = get_bits1(gb);
            if (sps->high_precision_offsets_enabled_flag)
                    av_log(avctx, AV_LOG_WARNING, "high_precision_offsets_enabled_flag not yet implemented\n") { }

            sps->persistent_rice_adaptation_enabled_flag = get_bits1(gb);

            sps->cabac_bypass_alignment_enabled_flag = get_bits1(gb);
            if (sps->cabac_bypass_alignment_enabled_flag)
                    av_log(avctx, AV_LOG_WARNING, "cabac_bypass_alignment_enabled_flag not yet implemented\n") { }
        }
    }
    if (apply_defdispwin) {
        sps->output_window.left_offset += sps->vui.def_disp_win.left_offset;
        sps->output_window.right_offset += sps->vui.def_disp_win.right_offset;
        sps->output_window.top_offset += sps->vui.def_disp_win.top_offset;
        sps->output_window.bottom_offset += sps->vui.def_disp_win.bottom_offset;
    }

    ow = &sps->output_window;
    if (ow->left_offset >= INT_MAX - ow->right_offset ||
        ow->top_offset >= INT_MAX - ow->bottom_offset ||
        ow->left_offset + ow->right_offset >= sps->width ||
        ow->top_offset + ow->bottom_offset >= sps->height) {
        av_log(avctx, AV_LOG_WARNING, "Invalid cropping offsets: %u/%u/%u/%u\n", ow->left_offset, ow->right_offset, ow->top_offset, ow->bottom_offset);
        if (avctx->err_recognition & AV_EF_EXPLODE) {
            return AVERROR_INVALIDDATA;
        }
        av_log(avctx, AV_LOG_WARNING, "Displaying the whole video surface.\n");
        memset(ow, 0, sizeof(*ow));
        memset(&sps->pic_conf_win, 0, sizeof(sps->pic_conf_win));
    }

// Inferred parameters
    sps->log2_ctb_size = sps->log2_min_cb_size + sps->log2_diff_max_min_coding_block_size;
    sps->log2_min_pu_size = sps->log2_min_cb_size - 1;

    if (sps->log2_ctb_size > HEVC_MAX_LOG2_CTB_SIZE) {
        av_log(avctx, AV_LOG_ERROR, "CTB size out of range: 2^%d\n", sps->log2_ctb_size);
        return AVERROR_INVALIDDATA;
    }
    if (sps->log2_ctb_size < 4) {
        av_log(avctx, AV_LOG_ERROR, "log2_ctb_size %d differs from the bounds of any known profile\n", sps->log2_ctb_size);
        avpriv_request_sample(avctx, "log2_ctb_size %d", sps->log2_ctb_size);
        return AVERROR_INVALIDDATA;
    }

    sps->ctb_width = (sps->width + (1 << sps->log2_ctb_size) - 1) >> sps->log2_ctb_size;
    sps->ctb_height = (sps->height + (1 << sps->log2_ctb_size) - 1) >> sps->log2_ctb_size;
    sps->ctb_size = sps->ctb_width * sps->ctb_height;

    sps->min_cb_width = sps->width >> sps->log2_min_cb_size;
    sps->min_cb_height = sps->height >> sps->log2_min_cb_size;
    sps->min_tb_width = sps->width >> sps->log2_min_tb_size;
    sps->min_tb_height = sps->height >> sps->log2_min_tb_size;
    sps->min_pu_width = sps->width >> sps->log2_min_pu_size;
    sps->min_pu_height = sps->height >> sps->log2_min_pu_size;
    sps->tb_mask = (1 << (sps->log2_ctb_size - sps->log2_min_tb_size)) - 1;

    sps->qp_bd_offset = 6 * (sps->bit_depth - 8);

    if (av_mod_uintp2(sps->width, sps->log2_min_cb_size) ||
        av_mod_uintp2(sps->height, sps->log2_min_cb_size)) {
        av_log(avctx, AV_LOG_ERROR, "Invalid coded frame dimensions.\n");
        return AVERROR_INVALIDDATA;
    }

    if (sps->max_transform_hierarchy_depth_inter > sps->log2_ctb_size - sps->log2_min_tb_size) {
        av_log(avctx, AV_LOG_ERROR, "max_transform_hierarchy_depth_inter out of range: %d\n", sps->max_transform_hierarchy_depth_inter);
        return AVERROR_INVALIDDATA;
    }
    if (sps->max_transform_hierarchy_depth_intra > sps->log2_ctb_size - sps->log2_min_tb_size) {
        av_log(avctx, AV_LOG_ERROR, "max_transform_hierarchy_depth_intra out of range: %d\n", sps->max_transform_hierarchy_depth_intra);
        return AVERROR_INVALIDDATA;
    }
    if (sps->log2_max_trafo_size > FFMIN(sps->log2_ctb_size, 5)) {
        av_log(avctx, AV_LOG_ERROR, "max transform block size out of range: %d\n", sps->log2_max_trafo_size);
        return AVERROR_INVALIDDATA;
    }

    if (get_bits_left(gb) < 0) {
        av_log(avctx, AV_LOG_ERROR, "Overread SPS by %d bits\n", -get_bits_left(gb));
        return AVERROR_INVALIDDATA;
    }

    return 0;
}


int ff_hevc_decode_nal_sps(GetBitContext *gb, AVCodecContext *avctx, HEVCParamSets *ps, int apply_defdispwin) {
    HEVCSPS *sps;
    AVBufferRef *sps_buf = av_buffer_allocz(sizeof(*sps));
    unsigned int sps_id;
    int ret;
    ptrdiff_t nal_size;

    if (!sps_buf) {
        return AVERROR(ENOMEM);
    }
    sps = (HEVCSPS *) sps_buf->data;

    av_log(avctx, AV_LOG_DEBUG, "Decoding SPS\n");

    nal_size = gb->buffer_end - gb->buffer;
    if (nal_size > sizeof(sps->data)) {
        av_log(avctx, AV_LOG_WARNING, "Truncating likely oversized SPS "
                                      "(%"PTRDIFF_SPECIFIER" > %"SIZE_SPECIFIER")\n", nal_size, sizeof(sps->data));
        sps->data_size = sizeof(sps->data);
    } else {
        sps->data_size = nal_size;
    }
    memcpy(sps->data, gb->buffer, sps->data_size);

    ret = ff_hevc_parse_sps(sps, gb, &sps_id, apply_defdispwin, ps->vps_list, avctx);
    if (ret < 0) {
        av_buffer_unref(&sps_buf);
        return ret;
    }

    if (avctx->debug & FF_DEBUG_BITSTREAM) {
        av_log(avctx, AV_LOG_DEBUG, "Parsed SPS: id %d; coded wxh: %dx%d; "
                                    "cropped wxh: %dx%d; pix_fmt: %s.\n", sps_id, sps->width, sps->height,
               sps->width - (sps->output_window.left_offset + sps->output_window.right_offset),
               sps->height - (sps->output_window.top_offset +
                              sps->output_window.bottom_offset), av_get_pix_fmt_name(sps->pix_fmt));
    }

/* check if this is a repeat of an already parsed SPS, then keep the
     * original one.
     * otherwise drop all PPSes that depend on it */
    if (ps->sps_list[sps_id] && !memcmp(ps->sps_list[sps_id]->data, sps_buf->data, sps_buf->size)) {
        av_buffer_unref(&sps_buf);
    } else {
        remove_sps(ps, sps_id);
        ps->sps_list[sps_id] = sps_buf;
    }

    return 0;
}


void av_image_fill_max_pixsteps(int max_pixsteps[4], int max_pixstep_comps[4], const AVPixFmtDescriptor *pixdesc) {
    int i;
    memset(max_pixsteps, 0, 4 * sizeof(max_pixsteps[0]));
    if (max_pixstep_comps) {
        memset(max_pixstep_comps, 0, 4 * sizeof(max_pixstep_comps[0]));
    }

    for (i = 0; i < 4; i++) {
        const AVComponentDescriptor *comp = &(pixdesc->comp[i]);
        if (comp->step > max_pixsteps[comp->plane]) {
            max_pixsteps[comp->plane] = comp->step;
            if (max_pixstep_comps) {
                max_pixstep_comps[comp->plane] = i;
            }
        }
    }
}


int av_image_get_linesize(enum AVPixelFormat pix_fmt, int width, int plane) {
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmt);
    int max_step[4];       /* max pixel step for each plane */
    int max_step_comp[4];       /* the component for each plane which has the max pixel step */

    if (!desc || desc->flags & AV_PIX_FMT_FLAG_HWACCEL) {
        return AVERROR(EINVAL);
    }

    av_image_fill_max_pixsteps(max_step, max_step_comp, desc);
    return image_get_linesize(width, plane, max_step[plane], max_step_comp[plane], desc);
}


static void remove_vps(HEVCParamSets *s, int id) {
    int i;
    if (s->vps_list[id]) {
        if (s->vps == (const HEVCVPS *) s->vps_list[id]->data) {
            s->vps = NULL;
        }

        for (i = 0; i < FF_ARRAY_ELEMS(s->sps_list); i++) {
            if (s->sps_list[i] && ((HEVCSPS *) s->sps_list[i]->data)->vps_id == id) {
                remove_sps(s, i);
            }
        }
    }
    av_buffer_unref(&s->vps_list[id]);
}


int ff_hevc_decode_nal_vps(GetBitContext *gb, AVCodecContext *avctx, HEVCParamSets *ps) {
    int i, j;
    int vps_id = 0;
    ptrdiff_t nal_size;
    HEVCVPS *vps;
    AVBufferRef *vps_buf = av_buffer_allocz(sizeof(*vps));

    if (!vps_buf) {
        return AVERROR(ENOMEM);
    }
    vps = (HEVCVPS *) vps_buf->data;

    av_log(avctx, AV_LOG_DEBUG, "Decoding VPS\n");

    nal_size = gb->buffer_end - gb->buffer;
    if (nal_size > sizeof(vps->data)) {
        av_log(avctx, AV_LOG_WARNING, "Truncating likely oversized VPS "
                                      "(%"PTRDIFF_SPECIFIER" > %"SIZE_SPECIFIER")\n", nal_size, sizeof(vps->data));
        vps->data_size = sizeof(vps->data);
    } else {
        vps->data_size = nal_size;
    }
    memcpy(vps->data, gb->buffer, vps->data_size);

    vps_id = get_bits(gb, 4);
    if (vps_id >= HEVC_MAX_VPS_COUNT) {
        av_log(avctx, AV_LOG_ERROR, "VPS id out of range: %d\n", vps_id);
        goto err;
    }

    if (get_bits(gb, 2) != 3) { // vps_reserved_three_2bits
        av_log(avctx, AV_LOG_ERROR, "vps_reserved_three_2bits is not three\n");
        goto err;
    }

    vps->vps_max_layers = get_bits(gb, 6) + 1;
    vps->vps_max_sub_layers = get_bits(gb, 3) + 1;
    vps->vps_temporal_id_nesting_flag = get_bits1(gb);

    if (get_bits(gb, 16) != 0xffff) { // vps_reserved_ffff_16bits
        av_log(avctx, AV_LOG_ERROR, "vps_reserved_ffff_16bits is not 0xffff\n");
        goto err;
    }

    if (vps->vps_max_sub_layers > HEVC_MAX_SUB_LAYERS) {
        av_log(avctx, AV_LOG_ERROR, "vps_max_sub_layers out of range: %d\n", vps->vps_max_sub_layers);
        goto err;
    }

    if (parse_ptl(gb, avctx, &vps->ptl, vps->vps_max_sub_layers) < 0) {
        goto err;
    }

    vps->vps_sub_layer_ordering_info_present_flag = get_bits1(gb);

    i = vps->vps_sub_layer_ordering_info_present_flag ? 0 : vps->vps_max_sub_layers - 1;
    for (; i < vps->vps_max_sub_layers; i++) {
        vps->vps_max_dec_pic_buffering[i] = get_ue_golomb_long(gb) + 1;
        vps->vps_num_reorder_pics[i] = get_ue_golomb_long(gb);
        vps->vps_max_latency_increase[i] = get_ue_golomb_long(gb) - 1;

        if (vps->vps_max_dec_pic_buffering[i] > HEVC_MAX_DPB_SIZE ||
            !vps->vps_max_dec_pic_buffering[i]) {
            av_log(avctx, AV_LOG_ERROR, "vps_max_dec_pic_buffering_minus1 out of range: %d\n",
                   vps->vps_max_dec_pic_buffering[i] - 1);
            goto err;
        }
        if (vps->vps_num_reorder_pics[i] > vps->vps_max_dec_pic_buffering[i] - 1) {
            av_log(avctx, AV_LOG_WARNING, "vps_max_num_reorder_pics out of range: %d\n", vps->vps_num_reorder_pics[i]);
            if (avctx->err_recognition & AV_EF_EXPLODE) {
                goto err;
            }
        }
    }

    vps->vps_max_layer_id = get_bits(gb, 6);
    vps->vps_num_layer_sets = get_ue_golomb_long(gb) + 1;
    if (vps->vps_num_layer_sets < 1 || vps->vps_num_layer_sets > 1024 ||
        (vps->vps_num_layer_sets - 1LL) * (vps->vps_max_layer_id + 1LL) > get_bits_left(gb)) {
        av_log(avctx, AV_LOG_ERROR, "too many layer_id_included_flags\n");
        goto err;
    }

    for (i = 1; i < vps->vps_num_layer_sets; i++) {
        for (j = 0; j <= vps->vps_max_layer_id; j++) {
            skip_bits(gb, 1);
        }
    }  // layer_id_included_flag[i][j]

    vps->vps_timing_info_present_flag = get_bits1(gb);
    if (vps->vps_timing_info_present_flag) {
        vps->vps_num_units_in_tick = get_bits_long(gb, 32);
        vps->vps_time_scale = get_bits_long(gb, 32);
        vps->vps_poc_proportional_to_timing_flag = get_bits1(gb);
        if (vps->vps_poc_proportional_to_timing_flag) {
            vps->vps_num_ticks_poc_diff_one = get_ue_golomb_long(gb) + 1;
        }
        vps->vps_num_hrd_parameters = get_ue_golomb_long(gb);
        if (vps->vps_num_hrd_parameters > (unsigned) vps->vps_num_layer_sets) {
            av_log(avctx, AV_LOG_ERROR, "vps_num_hrd_parameters %d is invalid\n", vps->vps_num_hrd_parameters);
            goto err;
        }
        for (i = 0; i < vps->vps_num_hrd_parameters; i++) {
            int common_inf_present = 1;

            get_ue_golomb_long(gb); // hrd_layer_set_idx
            if (i) {
                common_inf_present = get_bits1(gb);
            }
            decode_hrd(gb, common_inf_present, vps->vps_max_sub_layers);
        }
    }
    get_bits1(gb); /* vps_extension_flag */

    if (get_bits_left(gb) < 0) {
        av_log(avctx, AV_LOG_ERROR, "Overread VPS by %d bits\n", -get_bits_left(gb));
        if (ps->vps_list[vps_id]) {
            goto err;
        }
    }

    if (ps->vps_list[vps_id] && !memcmp(ps->vps_list[vps_id]->data, vps_buf->data, vps_buf->size)) {
        av_buffer_unref(&vps_buf);
    } else {
        remove_vps(ps, vps_id);
        ps->vps_list[vps_id] = vps_buf;
    }

    return 0;

    err:
    av_buffer_unref(&vps_buf);
    return AVERROR_INVALIDDATA;
}


int ff_hevc_decode_nal_pps(GetBitContext *gb, AVCodecContext *avctx, HEVCParamSets *ps) {
    HEVCSPS *sps = NULL;
    int i, ret = 0;
    unsigned int pps_id = 0;
    ptrdiff_t nal_size;
    unsigned log2_parallel_merge_level_minus2;

    AVBufferRef *pps_buf;
    HEVCPPS *pps = av_mallocz(sizeof(*pps));

    if (!pps) {
        return AVERROR(ENOMEM);
    }

    pps_buf = av_buffer_create((uint8_t *) pps, sizeof(*pps), hevc_pps_free, NULL, 0);
    if (!pps_buf) {
        av_freep(&pps);
        return AVERROR(ENOMEM);
    }

    av_log(avctx, AV_LOG_DEBUG, "Decoding PPS\n");

    nal_size = gb->buffer_end - gb->buffer;
    if (nal_size > sizeof(pps->data)) {
        av_log(avctx, AV_LOG_WARNING, "Truncating likely oversized PPS "
                                      "(%"PTRDIFF_SPECIFIER" > %"SIZE_SPECIFIER")\n", nal_size, sizeof(pps->data));
        pps->data_size = sizeof(pps->data);
    } else {
        pps->data_size = nal_size;
    }
    memcpy(pps->data, gb->buffer, pps->data_size);

// Default values
    pps->loop_filter_across_tiles_enabled_flag = 1;
    pps->num_tile_columns = 1;
    pps->num_tile_rows = 1;
    pps->uniform_spacing_flag = 1;
    pps->disable_dbf = 0;
    pps->beta_offset = 0;
    pps->tc_offset = 0;
    pps->log2_max_transform_skip_block_size = 2;

// Coded parameters
    pps_id = get_ue_golomb_long(gb);
    if (pps_id >= HEVC_MAX_PPS_COUNT) {
        av_log(avctx, AV_LOG_ERROR, "PPS id out of range: %d\n", pps_id);
        ret = AVERROR_INVALIDDATA;
        goto err;
    }
    pps->sps_id = get_ue_golomb_long(gb);
    if (pps->sps_id >= HEVC_MAX_SPS_COUNT) {
        av_log(avctx, AV_LOG_ERROR, "SPS id out of range: %d\n", pps->sps_id);
        ret = AVERROR_INVALIDDATA;
        goto err;
    }
    if (!ps->sps_list[pps->sps_id]) {
        av_log(avctx, AV_LOG_ERROR, "SPS %u does not exist.\n", pps->sps_id);
        ret = AVERROR_INVALIDDATA;
        goto err;
    }
    sps = (HEVCSPS *) ps->sps_list[pps->sps_id]->data;

    pps->dependent_slice_segments_enabled_flag = get_bits1(gb);
    pps->output_flag_present_flag = get_bits1(gb);
    pps->num_extra_slice_header_bits = get_bits(gb, 3);

    pps->sign_data_hiding_flag = get_bits1(gb);

    pps->cabac_init_present_flag = get_bits1(gb);

    pps->num_ref_idx_l0_default_active = get_ue_golomb_long(gb) + 1;
    pps->num_ref_idx_l1_default_active = get_ue_golomb_long(gb) + 1;

    pps->pic_init_qp_minus26 = get_se_golomb(gb);

    pps->constrained_intra_pred_flag = get_bits1(gb);
    pps->transform_skip_enabled_flag = get_bits1(gb);

    pps->cu_qp_delta_enabled_flag = get_bits1(gb);
    pps->diff_cu_qp_delta_depth = 0;
    if (pps->cu_qp_delta_enabled_flag) {
        pps->diff_cu_qp_delta_depth = get_ue_golomb_long(gb);
    }

    if (pps->diff_cu_qp_delta_depth < 0 ||
        pps->diff_cu_qp_delta_depth > sps->log2_diff_max_min_coding_block_size) {
        av_log(avctx, AV_LOG_ERROR, "diff_cu_qp_delta_depth %d is invalid\n", pps->diff_cu_qp_delta_depth);
        ret = AVERROR_INVALIDDATA;
        goto err;
    }

    pps->cb_qp_offset = get_se_golomb(gb);
    if (pps->cb_qp_offset < -12 || pps->cb_qp_offset > 12) {
        av_log(avctx, AV_LOG_ERROR, "pps_cb_qp_offset out of range: %d\n", pps->cb_qp_offset);
        ret = AVERROR_INVALIDDATA;
        goto err;
    }
    pps->cr_qp_offset = get_se_golomb(gb);
    if (pps->cr_qp_offset < -12 || pps->cr_qp_offset > 12) {
        av_log(avctx, AV_LOG_ERROR, "pps_cr_qp_offset out of range: %d\n", pps->cr_qp_offset);
        ret = AVERROR_INVALIDDATA;
        goto err;
    }
    pps->pic_slice_level_chroma_qp_offsets_present_flag = get_bits1(gb);

    pps->weighted_pred_flag = get_bits1(gb);
    pps->weighted_bipred_flag = get_bits1(gb);

    pps->transquant_bypass_enable_flag = get_bits1(gb);
    pps->tiles_enabled_flag = get_bits1(gb);
    pps->entropy_coding_sync_enabled_flag = get_bits1(gb);

    if (pps->tiles_enabled_flag) {
        int num_tile_columns_minus1 = get_ue_golomb(gb);
        int num_tile_rows_minus1 = get_ue_golomb(gb);

        if (num_tile_columns_minus1 < 0 || num_tile_columns_minus1 >= sps->ctb_width) {
            av_log(avctx, AV_LOG_ERROR, "num_tile_columns_minus1 out of range: %d\n", num_tile_columns_minus1);
            ret = num_tile_columns_minus1 < 0 ? num_tile_columns_minus1 : AVERROR_INVALIDDATA;
            goto err;
        }
        if (num_tile_rows_minus1 < 0 || num_tile_rows_minus1 >= sps->ctb_height) {
            av_log(avctx, AV_LOG_ERROR, "num_tile_rows_minus1 out of range: %d\n", num_tile_rows_minus1);
            ret = num_tile_rows_minus1 < 0 ? num_tile_rows_minus1 : AVERROR_INVALIDDATA;
            goto err;
        }
        pps->num_tile_columns = num_tile_columns_minus1 + 1;
        pps->num_tile_rows = num_tile_rows_minus1 + 1;

        pps->column_width = av_malloc_array(pps->num_tile_columns, sizeof(*pps->column_width));
        pps->row_height = av_malloc_array(pps->num_tile_rows, sizeof(*pps->row_height));
        if (!pps->column_width || !pps->row_height) {
            ret = AVERROR(ENOMEM);
            goto err;
        }

        pps->uniform_spacing_flag = get_bits1(gb);
        if (!pps->uniform_spacing_flag) {
            uint64_t sum = 0;
            for (i = 0; i < pps->num_tile_columns - 1; i++) {
                pps->column_width[i] = get_ue_golomb_long(gb) + 1;
                sum += pps->column_width[i];
            }
            if (sum >= sps->ctb_width) {
                av_log(avctx, AV_LOG_ERROR, "Invalid tile widths.\n");
                ret = AVERROR_INVALIDDATA;
                goto err;
            }
            pps->column_width[pps->num_tile_columns - 1] = sps->ctb_width - sum;

            sum = 0;
            for (i = 0; i < pps->num_tile_rows - 1; i++) {
                pps->row_height[i] = get_ue_golomb_long(gb) + 1;
                sum += pps->row_height[i];
            }
            if (sum >= sps->ctb_height) {
                av_log(avctx, AV_LOG_ERROR, "Invalid tile heights.\n");
                ret = AVERROR_INVALIDDATA;
                goto err;
            }
            pps->row_height[pps->num_tile_rows - 1] = sps->ctb_height - sum;
        }
        pps->loop_filter_across_tiles_enabled_flag = get_bits1(gb);
    }

    pps->seq_loop_filter_across_slices_enabled_flag = get_bits1(gb);

    pps->deblocking_filter_control_present_flag = get_bits1(gb);
    if (pps->deblocking_filter_control_present_flag) {
        pps->deblocking_filter_override_enabled_flag = get_bits1(gb);
        pps->disable_dbf = get_bits1(gb);
        if (!pps->disable_dbf) {
            int beta_offset_div2 = get_se_golomb(gb);
            int tc_offset_div2 = get_se_golomb(gb);
            if (beta_offset_div2 < -6 || beta_offset_div2 > 6) {
                av_log(avctx, AV_LOG_ERROR, "pps_beta_offset_div2 out of range: %d\n", beta_offset_div2);
                ret = AVERROR_INVALIDDATA;
                goto err;
            }
            if (tc_offset_div2 < -6 || tc_offset_div2 > 6) {
                av_log(avctx, AV_LOG_ERROR, "pps_tc_offset_div2 out of range: %d\n", tc_offset_div2);
                ret = AVERROR_INVALIDDATA;
                goto err;
            }
            pps->beta_offset = 2 * beta_offset_div2;
            pps->tc_offset = 2 * tc_offset_div2;
        }
    }

    pps->scaling_list_data_present_flag = get_bits1(gb);
    if (pps->scaling_list_data_present_flag) {
        set_default_scaling_list_data(&pps->scaling_list);
        ret = scaling_list_data(gb, avctx, &pps->scaling_list, sps);
        if (ret < 0) {
            goto err;
        }
    }
    pps->lists_modification_present_flag = get_bits1(gb);
    log2_parallel_merge_level_minus2 = get_ue_golomb_long(gb);
    if (log2_parallel_merge_level_minus2 > sps->log2_ctb_size) {
        av_log(avctx, AV_LOG_ERROR, "log2_parallel_merge_level_minus2 out of range: %d\n", log2_parallel_merge_level_minus2);
        ret = AVERROR_INVALIDDATA;
        goto err;
    }
    pps->log2_parallel_merge_level = log2_parallel_merge_level_minus2 + 2;

    pps->slice_header_extension_present_flag = get_bits1(gb);

    if (get_bits1(gb)) { // pps_extension_present_flag
        pps->pps_range_extensions_flag = get_bits1(gb);
        skip_bits(gb, 7); // pps_extension_7bits
        if (sps->ptl.general_ptl.profile_idc == FF_PROFILE_HEVC_REXT &&
            pps->pps_range_extensions_flag) {
            if ((ret = pps_range_extensions(gb, avctx, pps, sps)) < 0) {
                goto err;
            }
        }
    }

    ret = setup_pps(avctx, gb, pps, sps);
    if (ret < 0) {
        goto err;
    }

    if (get_bits_left(gb) < 0) {
        av_log(avctx, AV_LOG_ERROR, "Overread PPS by %d bits\n", -get_bits_left(gb));
        goto err;
    }

    remove_pps(ps, pps_id);
    ps->pps_list[pps_id] = pps_buf;

    return 0;

    err:
    av_buffer_unref(&pps_buf);
    return ret;
}


int ff_h2645_packet_split(H2645Packet *pkt, const uint8_t *buf, int length, void *logctx, int is_nalff, int nal_length_size, enum AVCodecID codec_id, int small_padding, int use_ref) {
    GetByteContext bc;
    int consumed, ret = 0;
    int next_avc = is_nalff ? 0 : length;
    int64_t padding = small_padding ? 0 : MAX_MBPAIR_SIZE;

    bytestream2_init(&bc, buf, length);
    alloc_rbsp_buffer(&pkt->rbsp, length + padding, use_ref);

    if (!pkt->rbsp.rbsp_buffer) {
        return AVERROR(ENOMEM);
    }

    pkt->rbsp.rbsp_buffer_size = 0;
    pkt->nb_nals = 0;
    while (bytestream2_get_bytes_left(&bc) >= 4) {
        H2645NAL *nal;
        int extract_length = 0;
        int skip_trailing_zeros = 1;

        if (bytestream2_tell(&bc) == next_avc) {
            int i = 0;
            extract_length = get_nalsize(nal_length_size, bc.buffer, bytestream2_get_bytes_left(&bc), &i, logctx);
            if (extract_length < 0) {
                return extract_length;
            }

            bytestream2_skip(&bc, nal_length_size);

            next_avc = bytestream2_tell(&bc) + extract_length;
        } else {
            int buf_index;

            if (bytestream2_tell(&bc) > next_avc)
                    av_log(logctx, AV_LOG_WARNING, "Exceeded next NALFF position, re-syncing.\n") { }

/* search start code */
            buf_index = find_next_start_code(bc.buffer, buf + next_avc);

            bytestream2_skip(&bc, buf_index);

            if (!bytestream2_get_bytes_left(&bc)) {
                if (pkt->nb_nals > 0) {
// No more start codes: we discarded some irrelevant
// bytes at the end of the packet.
                    return 0;
                } else {
                    av_log(logctx, AV_LOG_ERROR, "No start code is found.\n");
                    return AVERROR_INVALIDDATA;
                }
            }

            extract_length = FFMIN(bytestream2_get_bytes_left(&bc),
                                   next_avc - bytestream2_tell(&bc));

            if (bytestream2_tell(&bc) >= next_avc) {
/* skip to the start of the next NAL */
                bytestream2_skip(&bc, next_avc - bytestream2_tell(&bc));
                continue;
            }
        }

        if (pkt->nals_allocated < pkt->nb_nals + 1) {
            int new_size = pkt->nals_allocated + 1;
            void *tmp = av_realloc_array(pkt->nals, new_size, sizeof(*pkt->nals));

            if (!tmp) {
                return AVERROR(ENOMEM);
            }

            pkt->nals = tmp;
            memset(pkt->nals + pkt->nals_allocated, 0,
                   (new_size - pkt->nals_allocated) * sizeof(*pkt->nals));

            nal = &pkt->nals[pkt->nb_nals];
            nal->skipped_bytes_pos_size = 1024; // initial buffer size
            nal->skipped_bytes_pos = av_malloc_array(nal->skipped_bytes_pos_size, sizeof(*nal->skipped_bytes_pos));
            if (!nal->skipped_bytes_pos) {
                return AVERROR(ENOMEM);
            }

            pkt->nals_allocated = new_size;
        }
        nal = &pkt->nals[pkt->nb_nals];

        consumed = ff_h2645_extract_rbsp(bc.buffer, extract_length, &pkt->rbsp, nal, small_padding);
        if (consumed < 0) {
            return consumed;
        }

        if (is_nalff && (extract_length != consumed) && extract_length)
                av_log(logctx, AV_LOG_DEBUG, "NALFF: Consumed only %d bytes instead of %d\n", consumed, extract_length) { }

        pkt->nb_nals++;

        bytestream2_skip(&bc, consumed);

/* see commit 3566042a0 */
        if (bytestream2_get_bytes_left(&bc) >= 4 && bytestream2_peek_be32(&bc) == 0x000001E0) {
            skip_trailing_zeros = 0;
        }

        nal->size_bits = get_bit_length(nal, skip_trailing_zeros);

        ret = init_get_bits(&nal->gb, nal->data, nal->size_bits);
        if (ret < 0) {
            return ret;
        }

        if (codec_id == AV_CODEC_ID_HEVC) {
            ret = hevc_parse_nal_header(nal, logctx);
        } else {
            ret = h264_parse_nal_header(nal, logctx);
        }
        if (ret <= 0 || nal->size <= 0 || nal->size_bits <= 0) {
            if (ret < 0) {
                av_log(logctx, AV_LOG_WARNING, "Invalid NAL unit %d, skipping.\n", nal->type);
            }
            pkt->nb_nals--;
        }
    }

    return 0;
}


void ff_hevc_reset_sei(HEVCSEI *s) {
    s->a53_caption.a53_caption_size = 0;
    av_freep(&s->a53_caption.a53_caption);
}

/**
 * Parse NAL units of found picture and decode some basic information.
 *
 * @param s parser context.
 * @param avctx codec context.
 * @param buf buffer with field/frame data.
 * @param buf_size size of the buffer.
 */
static int parse_nal_units(AVCodecParserContext *s, const uint8_t *buf, int buf_size, AVCodecContext *avctx) {
    HEVCParserContext *ctx = s->priv_data;
    HEVCParamSets *ps = &ctx->ps;
    HEVCSEI *sei = &ctx->sei;
    int ret, i;

/* set some sane default values */
    s->pict_type = AV_PICTURE_TYPE_I;
    s->key_frame = 0;
    s->picture_structure = AV_PICTURE_STRUCTURE_UNKNOWN;

    ff_hevc_reset_sei(sei);

    ret = ff_h2645_packet_split(&ctx->pkt, buf, buf_size, avctx, ctx->is_avc, ctx->nal_length_size, AV_CODEC_ID_HEVC, 1, 0);
    if (ret < 0) {
        return ret;
    }

    for (i = 0; i < ctx->pkt.nb_nals; i++) {
        H2645NAL *nal = &ctx->pkt.nals[i];
        GetBitContext *gb = &nal->gb;

        switch (nal->type) {
            case HEVC_NAL_VPS:
                ff_hevc_decode_nal_vps(gb, avctx, ps);
                break;
            case HEVC_NAL_SPS:
                ff_hevc_decode_nal_sps(gb, avctx, ps, 1);
                break;
            case HEVC_NAL_PPS:
                ff_hevc_decode_nal_pps(gb, avctx, ps);
                break;
            case HEVC_NAL_SEI_PREFIX:
            case HEVC_NAL_SEI_SUFFIX:
                ff_hevc_decode_nal_sei(gb, avctx, sei, ps, nal->type);
                break;
            case HEVC_NAL_TRAIL_N:
            case HEVC_NAL_TRAIL_R:
            case HEVC_NAL_TSA_N:
            case HEVC_NAL_TSA_R:
            case HEVC_NAL_STSA_N:
            case HEVC_NAL_STSA_R:
            case HEVC_NAL_BLA_W_LP:
            case HEVC_NAL_BLA_W_RADL:
            case HEVC_NAL_BLA_N_LP:
            case HEVC_NAL_IDR_W_RADL:
            case HEVC_NAL_IDR_N_LP:
            case HEVC_NAL_CRA_NUT:
            case HEVC_NAL_RADL_N:
            case HEVC_NAL_RADL_R:
            case HEVC_NAL_RASL_N:
            case HEVC_NAL_RASL_R:
                ret = hevc_parse_slice_header(s, nal, avctx);
                if (ret) {
                    return ret;
                }
                break;
        }
    }
/* didn't find a picture! */
    av_log(avctx, AV_LOG_ERROR, "missing picture in access unit with size %d\n", buf_size);
    return -1;
}

#define END_NOT_FOUND (-100)

/**
 * Find the end of the current frame in the bitstream.
 * @return the position of the first byte of the next frame, or END_NOT_FOUND
 */
static int hevc_find_frame_end(AVCodecParserContext *s, const uint8_t *buf, int buf_size) {
    HEVCParserContext *ctx = s->priv_data;
    ParseContext *pc = &ctx->pc;
    int i;

    for (i = 0; i < buf_size; i++) {
        int nut;

        pc->state64 = (pc->state64 << 8) | buf[i];

        if (((pc->state64 >> 3 * 8) & 0xFFFFFF) != START_CODE) {
            continue;
        }

        nut = (pc->state64 >> 2 * 8 + 1) & 0x3F;
// Beginning of access unit
        if ((nut >= HEVC_NAL_VPS && nut <= HEVC_NAL_EOB_NUT) || nut == HEVC_NAL_SEI_PREFIX ||
            (nut >= 41 && nut <= 44) || (nut >= 48 && nut <= 55)) {
            if (pc->frame_start_found) {
                pc->frame_start_found = 0;
                return i - 5;
            }
        } else if (nut <= HEVC_NAL_RASL_R ||
                   (nut >= HEVC_NAL_BLA_W_LP && nut <= HEVC_NAL_CRA_NUT)) {
            int first_slice_segment_in_pic_flag = buf[i] >> 7;
            if (first_slice_segment_in_pic_flag) {
                if (!pc->frame_start_found) {
                    pc->frame_start_found = 1;
                } else { // First slice of next frame found
                    pc->frame_start_found = 0;
                    return i - 5;
                }
            }
        }
    }

    return END_NOT_FOUND;
}


void ff_h2645_packet_uninit(H2645Packet *pkt) {
    int i;
    for (i = 0; i < pkt->nals_allocated; i++) {
        av_freep(&pkt->nals[i].skipped_bytes_pos);
    }
    av_freep(&pkt->nals);
    pkt->nals_allocated = 0;
    if (pkt->rbsp.rbsp_buffer_ref) {
        av_buffer_unref(&pkt->rbsp.rbsp_buffer_ref);
        pkt->rbsp.rbsp_buffer = NULL;
    } else {
        av_freep(&pkt->rbsp.rbsp_buffer);
    }
    pkt->rbsp.rbsp_buffer_alloc_size = pkt->rbsp.rbsp_buffer_size = 0;
}


static const char *h264_nal_unit_name(int nal_type) {
    av_assert0(nal_type >= 0 && nal_type < 32);
    return h264_nal_type_name[nal_type];
}

static int h264_parse_nal_header(H2645NAL *nal, void *logctx) {
    GetBitContext *gb = &nal->gb;

    if (get_bits1(gb) != 0) {
        return AVERROR_INVALIDDATA;
    }

    nal->ref_idc = get_bits(gb, 2);
    nal->type = get_bits(gb, 5);

    av_log(logctx, AV_LOG_DEBUG, "nal_unit_type: %d(%s), nal_ref_idc: %d\n", nal->type, h264_nal_unit_name(nal->type), nal->ref_idc);

    return 1;
}

static int find_next_start_code(const uint8_t *buf, const uint8_t *next_avc) {
    int i = 0;

    if (buf + 3 >= next_avc) {
        return next_avc - buf;
    }

    while (buf + i + 3 < next_avc) {
        if (buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1) {
            break;
        }
        i++;
    }
    return i + 3;
}


int av_buffer_is_writable(const AVBufferRef *buf) {
    if (buf->buffer->flags & AV_BUFFER_FLAG_READONLY) {
        return 0;
    }

    return atomic_load(&buf->buffer->refcount) == 1;
}

static void alloc_rbsp_buffer(H2645RBSP *rbsp, unsigned int size, int use_ref) {
    int min_size = size;

    if (size > INT_MAX - AV_INPUT_BUFFER_PADDING_SIZE) {
        goto fail;
    }
    size += AV_INPUT_BUFFER_PADDING_SIZE;

    if (rbsp->rbsp_buffer_alloc_size >= size &&
        (!rbsp->rbsp_buffer_ref || av_buffer_is_writable(rbsp->rbsp_buffer_ref))) {
        av_assert0(rbsp->rbsp_buffer);
        memset(rbsp->rbsp_buffer + min_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
        return;
    }

    size = FFMIN(size + size / 16 + 32, INT_MAX);

    if (rbsp->rbsp_buffer_ref) {
        av_buffer_unref(&rbsp->rbsp_buffer_ref);
    } else {
        av_free(rbsp->rbsp_buffer);
    }

    rbsp->rbsp_buffer = av_mallocz(size);
    if (!rbsp->rbsp_buffer) {
        goto fail;
    }
    rbsp->rbsp_buffer_alloc_size = size;

    if (use_ref) {
        rbsp->rbsp_buffer_ref = av_buffer_create(rbsp->rbsp_buffer, size, NULL, NULL, 0);
        if (!rbsp->rbsp_buffer_ref) {
            goto fail;
        }
    }

    return;

    fail:
    rbsp->rbsp_buffer_alloc_size = 0;
    if (rbsp->rbsp_buffer_ref) {
        av_buffer_unref(&rbsp->rbsp_buffer_ref);
        rbsp->rbsp_buffer = NULL;
    } else {
        av_freep(&rbsp->rbsp_buffer);
    }

    return;
}


static inline int get_nalsize(int nal_length_size, const uint8_t *buf, int buf_size, int *buf_index, void *logctx) {
    int i, nalsize = 0;

    if (*buf_index >= buf_size - nal_length_size) {
// the end of the buffer is reached, refill it
        return AVERROR(EAGAIN);
    }

    for (i = 0; i < nal_length_size; i++) {
        nalsize = ((unsigned) nalsize << 8) | buf[(*buf_index)++];
    }
    if (nalsize <= 0 || nalsize > buf_size - *buf_index) {
        av_log(logctx, AV_LOG_ERROR, "Invalid NAL unit size (%d > %d).\n", nalsize,
               buf_size - *buf_index);
        return AVERROR_INVALIDDATA;
    }
    return nalsize;
}


/**
 * @return AVERROR_INVALIDDATA if the packet is not a valid NAL unit,
 * 0 if the unit should be skipped, 1 otherwise
 */
static int hevc_parse_nal_header(H2645NAL *nal, void *logctx) {
    GetBitContext *gb = &nal->gb;
    int nuh_layer_id;

    if (get_bits1(gb) != 0) {
        return AVERROR_INVALIDDATA;
    }

    nal->type = get_bits(gb, 6);

    nuh_layer_id = get_bits(gb, 6);
    nal->temporal_id = get_bits(gb, 3) - 1;
    if (nal->temporal_id < 0) {
        return AVERROR_INVALIDDATA;
    }

    av_log(logctx, AV_LOG_DEBUG, "nal_unit_type: %d(%s), nuh_layer_id: %d, temporal_id: %d\n", nal->type, hevc_nal_unit_name(nal->type), nuh_layer_id, nal->temporal_id);

    return nuh_layer_id == 0;
}


static int hevc_decode_nal_units(const uint8_t *buf, int buf_size, HEVCParamSets *ps, HEVCSEI *sei, int is_nalff, int nal_length_size, int err_recognition, int apply_defdispwin, void *logctx) {
    int i;
    int ret = 0;
    H2645Packet pkt = {0};

    ret = ff_h2645_packet_split(&pkt, buf, buf_size, logctx, is_nalff, nal_length_size, AV_CODEC_ID_HEVC, 1, 0);
    if (ret < 0) {
        goto done;
    }

    for (i = 0; i < pkt.nb_nals; i++) {
        H2645NAL *nal = &pkt.nals[i];

/* ignore everything except parameter sets and VCL NALUs */
        switch (nal->type) {
            case HEVC_NAL_VPS:
                ret = ff_hevc_decode_nal_vps(&nal->gb, logctx, ps);
                if (ret < 0) {
                    goto done;
                }
                break;
            case HEVC_NAL_SPS:
                ret = ff_hevc_decode_nal_sps(&nal->gb, logctx, ps, apply_defdispwin);
                if (ret < 0) {
                    goto done;
                }
                break;
            case HEVC_NAL_PPS:
                ret = ff_hevc_decode_nal_pps(&nal->gb, logctx, ps);
                if (ret < 0) {
                    goto done;
                }
                break;
            case HEVC_NAL_SEI_PREFIX:
            case HEVC_NAL_SEI_SUFFIX:
                ret = ff_hevc_decode_nal_sei(&nal->gb, logctx, sei, ps, nal->type);
                if (ret < 0) {
                    goto done;
                }
                break;
            default:
                av_log(logctx, AV_LOG_VERBOSE, "Ignoring NAL type %d in extradata\n", nal->type);
                break;
        }
    }

    done:
    ff_h2645_packet_uninit(&pkt);
    if (err_recognition & AV_EF_EXPLODE) {
        return ret;
    }

    return 0;
}


int ff_hevc_decode_extradata(const uint8_t *data, int size, HEVCParamSets *ps, HEVCSEI *sei, int *is_nalff, int *nal_length_size, int err_recognition, int apply_defdispwin, void *logctx) {
    int ret = 0;
    GetByteContext gb;

    bytestream2_init(&gb, data, size);

    if (size > 3 && (data[0] || data[1] || data[2] > 1)) {
/* It seems the extradata is encoded as hvcC format.
         * Temporarily, we support configurationVersion==0 until 14496-15 3rd
         * is finalized. When finalized, configurationVersion will be 1 and we
         * can recognize hvcC by checking if avctx->extradata[0]==1 or not. */
        int i, j, num_arrays, nal_len_size;

        *is_nalff = 1;

        bytestream2_skip(&gb, 21);
        nal_len_size = (bytestream2_get_byte(&gb) & 3) + 1;
        num_arrays = bytestream2_get_byte(&gb);

/* nal units in the hvcC always have length coded with 2 bytes,
         * so put a fake nal_length_size = 2 while parsing them */
        *nal_length_size = 2;

/* Decode nal units from hvcC. */
        for (i = 0; i < num_arrays; i++) {
            int type = bytestream2_get_byte(&gb) & 0x3f;
            int cnt = bytestream2_get_be16(&gb);

            for (j = 0; j < cnt; j++) {
// +2 for the nal size field
                int nalsize = bytestream2_peek_be16(&gb) + 2;
                if (bytestream2_get_bytes_left(&gb) < nalsize) {
                    av_log(logctx, AV_LOG_ERROR, "Invalid NAL unit size in extradata.\n");
                    return AVERROR_INVALIDDATA;
                }

                ret = hevc_decode_nal_units(gb.buffer, nalsize, ps, sei, *is_nalff, *nal_length_size, err_recognition, apply_defdispwin, logctx);
                if (ret < 0) {
                    av_log(logctx, AV_LOG_ERROR, "Decoding nal unit %d %d from hvcC failed\n", type, i);
                    return ret;
                }
                bytestream2_skip(&gb, nalsize);
            }
        }

/* Now store right nal length size, that will be used to parse
         * all other nals */
        *nal_length_size = nal_len_size;
    } else {
        *is_nalff = 0;
        ret = hevc_decode_nal_units(data, size, ps, sei, *is_nalff, *nal_length_size, err_recognition, apply_defdispwin, logctx);
        if (ret < 0) {
            return ret;
        }
    }

    return ret;
}


void *av_fast_realloc(void *ptr, unsigned int *size, size_t min_size) {
    if (min_size <= *size) {
        return ptr;
    }

    if (min_size > max_alloc_size - 32) {
        *size = 0;
        return NULL;
    }

    min_size = FFMIN(max_alloc_size - 32, FFMAX(min_size + min_size / 16 + 32, min_size));

    ptr = av_realloc(ptr, min_size);
/* we could set this to the unmodified min_size but this is safer
     * if the user lost the ptr and uses NULL now
     */
    if (!ptr) {
        min_size = 0;
    }

    *size = min_size;

    return ptr;
}


int ff_combine_frame(ParseContext *pc, int next, const uint8_t **buf, int *buf_size) {
    if (pc->overread) {
        ff_dlog(NULL, "overread %d, state:%"PRIX32" next:%d index:%d o_index:%d\n", pc->overread, pc->state, next, pc->index, pc->overread_index);
        ff_dlog(NULL, "%X %X %X %X\n", (*buf)[0], (*buf)[1], (*buf)[2], (*buf)[3]);
    }

/* Copy overread bytes from last frame into buffer. */
    for (; pc->overread > 0; pc->overread--) {
        pc->buffer[pc->index++] = pc->buffer[pc->overread_index++];
    }

    if (next > *buf_size) {
        return AVERROR(EINVAL);
    }

/* flush remaining if EOF */
    if (!*buf_size && next == END_NOT_FOUND) {
        next = 0;
    }

    pc->last_index = pc->index;

/* copy into buffer end return */
    if (next == END_NOT_FOUND) {
        void *new_buffer = av_fast_realloc(pc->buffer, &pc->buffer_size,
                                           *buf_size + pc->index + AV_INPUT_BUFFER_PADDING_SIZE);

        if (!new_buffer) {
            av_log(NULL, AV_LOG_ERROR, "Failed to reallocate parser buffer to %d\n",
                   *buf_size + pc->index + AV_INPUT_BUFFER_PADDING_SIZE);
            pc->index = 0;
            return AVERROR(ENOMEM);
        }
        pc->buffer = new_buffer;
        memcpy(&pc->buffer[pc->index], *buf, *buf_size);
        pc->index += *buf_size;
        return -1;
    }

    av_assert0(next >= 0 || pc->buffer);

    *buf_size = pc->overread_index = pc->index + next;

/* append to buffer */
    if (pc->index) {
        void *new_buffer = av_fast_realloc(pc->buffer, &pc->buffer_size,
                                           next + pc->index + AV_INPUT_BUFFER_PADDING_SIZE);
        if (!new_buffer) {
            av_log(NULL, AV_LOG_ERROR, "Failed to reallocate parser buffer to %d\n",
                   next + pc->index + AV_INPUT_BUFFER_PADDING_SIZE);
            pc->overread_index = pc->index = 0;
            return AVERROR(ENOMEM);
        }
        pc->buffer = new_buffer;
        if (next > -AV_INPUT_BUFFER_PADDING_SIZE) {
            memcpy(&pc->buffer[pc->index], *buf, next + AV_INPUT_BUFFER_PADDING_SIZE);
        }
        pc->index = 0;
        *buf = pc->buffer;
    }

    if (next < -8) {
        pc->overread += -8 - next;
        next = -8;
    }
/* store overread bytes */
    for (; next < 0; next++) {
        pc->state = pc->state << 8 | pc->buffer[pc->last_index + next];
        pc->state64 = pc->state64 << 8 | pc->buffer[pc->last_index + next];
        pc->overread++;
    }

    if (pc->overread) {
        ff_dlog(NULL, "overread %d, state:%"PRIX32" next:%d index:%d o_index:%d\n", pc->overread, pc->state, next, pc->index, pc->overread_index);
        ff_dlog(NULL, "%X %X %X %X\n", (*buf)[0], (*buf)[1], (*buf)[2], (*buf)[3]);
    }

    return 0;
}


static int hevc_parse(AVCodecParserContext *s, AVCodecContext *avctx, const uint8_t **poutbuf, int *poutbuf_size, const uint8_t *buf, int buf_size) {
    int next;
    HEVCParserContext *ctx = s->priv_data;
    ParseContext *pc = &ctx->pc;
    int is_dummy_buf = !buf_size;
    const uint8_t *dummy_buf = buf;

    if (avctx->extradata && !ctx->parsed_extradata) {
        ff_hevc_decode_extradata(avctx->extradata, avctx->extradata_size, &ctx->ps, &ctx->sei, &ctx->is_avc, &ctx->nal_length_size, avctx->err_recognition, 1, avctx);
        ctx->parsed_extradata = 1;
    }

    if (s->flags & PARSER_FLAG_COMPLETE_FRAMES) {
        next = buf_size;
    } else {
        next = hevc_find_frame_end(s, buf, buf_size);
        if (ff_combine_frame(pc, next, &buf, &buf_size) < 0) {
            *poutbuf = NULL;
            *poutbuf_size = 0;
            return buf_size;
        }
    }

    is_dummy_buf &= (dummy_buf == buf);

    if (!is_dummy_buf) {
        parse_nal_units(s, buf, buf_size, avctx);
    }

    *poutbuf = buf;
    *poutbuf_size = buf_size;
    return next;
}


const uint8_t *avpriv_find_start_code(const uint8_t *av_restrict p, const uint8_t *end, uint32_t *av_restrict state) {
    int i;

    av_assert0(p <= end);
    if (p >= end) {
        return end;
    }

    for (i = 0; i < 3; i++) {
        uint32_t tmp = *state << 8;
        *state = tmp + *(p++);
        if (tmp == 0x100 || p == end) {
            return p;
        }
    }

    while (p < end) {
        if (p[-1] > 1) {
            p += 3;
        } else if (p[-2]) {
            p += 2;
        } else if (p[-3] | (p[-1] - 1)) {
            p++;
        } else {
            p++;
            break;
        }
    }

    p = FFMIN(p, end) - 4;
    *state = AV_RB32(p);

    return p + 4;
}


// Split after the parameter sets at the beginning of the stream if they exist.
static int hevc_split(AVCodecContext *avctx, const uint8_t *buf, int buf_size) {
    const uint8_t *ptr = buf, *end = buf + buf_size;
    uint32_t state = -1;
    int has_vps = 0;
    int has_sps = 0;
    int has_pps = 0;
    int nut;

    while (ptr < end) {
        ptr = avpriv_find_start_code(ptr, end, &state);
        if ((state >> 8) != START_CODE) {
            break;
        }
        nut = (state >> 1) & 0x3F;
        if (nut == HEVC_NAL_VPS) {
            has_vps = 1;
        } else if (nut == HEVC_NAL_SPS) {
            has_sps = 1;
        } else if (nut == HEVC_NAL_PPS) {
            has_pps = 1;
        } else if ((nut != HEVC_NAL_SEI_PREFIX || has_pps) && nut != HEVC_NAL_AUD) {
            if (has_vps && has_sps) {
                while (ptr - 4 > buf && ptr[-5] == 0) {
                    ptr--;
                }
                return ptr - 4 - buf;
            }
        }
    }
    return 0;
}
//
//static void hevc_parser_close(AVCodecParserContext *s)
//{
//    HEVCParserContext *ctx = s->priv_data;
//
//    ff_hevc_ps_uninit(&ctx->ps);
//    ff_h2645_packet_uninit(&ctx->pkt);
//    ff_hevc_reset_sei(&ctx->sei);
//
//    av_freep(&ctx->pc.buffer);
//}
//
//AVCodecParser ff_hevc_parser = {
//    .codec_ids      = { AV_CODEC_ID_HEVC },
//    .priv_data_size = sizeof(HEVCParserContext),
//    .parser_parse   = hevc_parse,
//    .parser_close   = hevc_parser_close,
//    .split          = hevc_split,
//};

void ff_h2645_packet_uninit(H2645Packet *pkt);


int h2645_ps_to_nalu(const uint8_t *src, int src_size, uint8_t **out, int *out_size) {
    int i;
    int ret = 0;
    uint8_t *p = NULL;
    static const uint8_t nalu_header[] = {0x00, 0x00, 0x00, 0x01};

    if (!out || !out_size) {
        return AVERROR(EINVAL);
    }

    p = av_malloc(sizeof(nalu_header) + src_size);
    if (!p) {
        return AVERROR(ENOMEM);
    }

    *out = p;
    *out_size = sizeof(nalu_header) + src_size;

    memcpy(p, nalu_header, sizeof(nalu_header));
    memcpy(p + sizeof(nalu_header), src, src_size);

/* Escape 0x00, 0x00, 0x0{0-3} pattern */
    for (i = 4; i < *out_size; i++) {
        if (i < *out_size - 3 && p[i + 0] == 0 && p[i + 1] == 0 && p[i + 2] <= 3) {
            uint8_t *new;

            *out_size += 1;
            new = av_realloc(*out, *out_size);
            if (!new) {
                ret = AVERROR(ENOMEM);
                goto done;
            }
            *out = p = new;

            i = i + 2;
            memmove(p + i + 1, p + i, *out_size - (i + 1));
            p[i] = 0x03;
        }
    }
    done:
    if (ret < 0) {
        av_freep(out);
        *out_size = 0;
    }

    return ret;
}

struct FFAMediaFormat;
typedef struct FFAMediaFormat FFAMediaFormat;


static int hevc_set_extradata(AVCodecContext *avctx, FFAMediaFormat *format) {
    int i;
    int ret;

    HEVCParamSets ps;
    HEVCSEI sei;

    const HEVCVPS *vps = NULL;
    const HEVCPPS *pps = NULL;
    const HEVCSPS *sps = NULL;
    int is_nalff = 0;
    int nal_length_size = 0;

    uint8_t *vps_data = NULL;
    uint8_t *sps_data = NULL;
    uint8_t *pps_data = NULL;
    int vps_data_size = 0;
    int sps_data_size = 0;
    int pps_data_size = 0;

    memset(&ps, 0, sizeof(ps));
    memset(&sei, 0, sizeof(sei));

    ret = ff_hevc_decode_extradata(avctx->extradata, avctx->extradata_size, &ps, &sei, &is_nalff, &nal_length_size, 0, 1, avctx);
    if (ret < 0) {
        goto done;
    }

    for (i = 0; i < HEVC_MAX_VPS_COUNT; i++) {
        if (ps.vps_list[i]) {
            vps = (const HEVCVPS *) ps.vps_list[i]->data;
            break;
        }
    }

    for (i = 0; i < HEVC_MAX_PPS_COUNT; i++) {
        if (ps.pps_list[i]) {
            pps = (const HEVCPPS *) ps.pps_list[i]->data;
            break;
        }
    }

    if (pps) {
        if (ps.sps_list[pps->sps_id]) {
            sps = (const HEVCSPS *) ps.sps_list[pps->sps_id]->data;
        }
    }

    if (vps && pps && sps) {
        uint8_t *data;
        int data_size;

        if ((ret = h2645_ps_to_nalu(vps->data, vps->data_size, &vps_data, &vps_data_size)) < 0 ||
            (ret = h2645_ps_to_nalu(sps->data, sps->data_size, &sps_data, &sps_data_size)) < 0 ||
            (ret = h2645_ps_to_nalu(pps->data, pps->data_size, &pps_data, &pps_data_size)) < 0) {
            goto done;
        }

        data_size = vps_data_size + sps_data_size + pps_data_size;
        data = av_mallocz(data_size);
        if (!data) {
            ret = AVERROR(ENOMEM);
            goto done;
        }

        memcpy(data, vps_data, vps_data_size);
        memcpy(data + vps_data_size, sps_data, sps_data_size);
        memcpy(data + vps_data_size + sps_data_size, pps_data, pps_data_size);

// ff_AMediaFormat_setBuffer(format, "csd-0", data, data_size);

        av_freep(&data);
    } else {
        av_log(avctx, AV_LOG_ERROR, "Could not extract VPS/PPS/SPS from extradata");
        ret = AVERROR_INVALIDDATA;
    }

    done:
//ff_hevc_ps_uninit(&ps);

    av_freep(&vps_data);
    av_freep(&sps_data);
    av_freep(&pps_data);

    return ret;
}


//https://raw.githubusercontent.com/FFmpeg/FFmpeg/master/libavcodec/hevc_parse.c

//
/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
//
//#include "bytestream.h"
//#include "h2645_parse.h"
//#include "hevc.h"
//#include "hevc_parse.h"








//https://raw.githubusercontent.com/FFmpeg/FFmpeg/master/libavcodec/h2645_parse.c

/*
 * H.264/HEVC common parsing code
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */



int ff_h2645_extract_rbsp(const uint8_t *src, int length, H2645RBSP *rbsp, H2645NAL *nal, int small_padding) {
    int i, si, di;
    uint8_t *dst;

    nal->skipped_bytes = 0;
#define STARTCODE_TEST                                                  \
        if (i + 2 < length && src[i + 1] == 0 && src[i + 2] <= 3) {     \
            if (src[i + 2] != 3 && src[i + 2] != 0) {                   \
                /* startcode, so we must be past the end */             \
                length = i;                                             \
            }                                                           \
            break;                                                      \
        }
#if HAVE_FAST_UNALIGNED
                                                                                                                            #define FIND_FIRST_ZERO                                                 \
        if (i > 0 && !src[i])                                           \
            i--;                                                        \
        while (src[i])                                                  \
            i++
#if HAVE_FAST_64BIT
for (i = 0; i + 1 < length; i += 9) {
if (!((~AV_RN64(src + i) &
(AV_RN64(src + i) - 0x0100010001000101ULL)) &
0x8000800080008080ULL))
continue;
FIND_FIRST_ZERO;
STARTCODE_TEST;
i -= 7;
}
#else
for (i = 0; i + 1 < length; i += 5) {
if (!((~AV_RN32(src + i) &
(AV_RN32(src + i) - 0x01000101U)) &
0x80008080U))
continue;
FIND_FIRST_ZERO;
STARTCODE_TEST;
i -= 3;
}
#endif /* HAVE_FAST_64BIT */
#else
    for (i = 0; i + 1 < length; i += 2) {
        if (src[i]) {
            continue;
        }
        if (i > 0 && src[i - 1] == 0) {
            i--;
        }
        STARTCODE_TEST;
    }
#endif /* HAVE_FAST_UNALIGNED */

    if (i >= length - 1 && small_padding) { // no escaped 0
        nal->data = nal->raw_data = src;
        nal->size = nal->raw_size = length;
        return length;
    } else if (i > length) {
        i = length;
    }

    nal->rbsp_buffer = &rbsp->rbsp_buffer[rbsp->rbsp_buffer_size];
    dst = nal->rbsp_buffer;

    memcpy(dst, src, i);
    si = di = i;
    while (si + 2 < length) {
// remove escapes (very rare 1:2^22)
        if (src[si + 2] > 3) {
            dst[di++] = src[si++];
            dst[di++] = src[si++];
        } else if (src[si] == 0 && src[si + 1] == 0 && src[si + 2] != 0) {
            if (src[si + 2] == 3) { // escape
                dst[di++] = 0;
                dst[di++] = 0;
                si += 3;

                if (nal->skipped_bytes_pos) {
                    nal->skipped_bytes++;
                    if (nal->skipped_bytes_pos_size < nal->skipped_bytes) {
                        nal->skipped_bytes_pos_size *= 2;
                        av_assert0(nal->skipped_bytes_pos_size >= nal->skipped_bytes);
                        av_reallocp_array(&nal->skipped_bytes_pos, nal->skipped_bytes_pos_size, sizeof(*nal->skipped_bytes_pos));
                        if (!nal->skipped_bytes_pos) {
                            nal->skipped_bytes_pos_size = 0;
                            return AVERROR(ENOMEM);
                        }
                    }
                    if (nal->skipped_bytes_pos) {
                        nal->skipped_bytes_pos[nal->skipped_bytes - 1] = di - 1;
                    }
                }
                continue;
            } else { // next start code
                goto nsc;
            }
        }

        dst[di++] = src[si++];
    }
    while (si < length) {
        dst[di++] = src[si++];
    }

    nsc:
    memset(dst + di, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    nal->data = dst;
    nal->size = di;
    nal->raw_data = src;
    nal->raw_size = si;
    rbsp->rbsp_buffer_size += si;

    return si;
}

static const char *hevc_nal_unit_name(int nal_type) {
    av_assert0(nal_type >= 0 && nal_type < 64);
    return hevc_nal_type_name[nal_type];
}


static pthread_mutex_t atomic_lock = PTHREAD_MUTEX_INITIALIZER;

void avpriv_atomic_lock(void) {
    pthread_mutex_lock(&atomic_lock);
}

void avpriv_atomic_unlock(void) {
    pthread_mutex_unlock(&atomic_lock);
}
//
//static inline intptr_t atomic_load(intptr_t *object)
//{
//    intptr_t ret;
//    avpriv_atomic_lock();
//    ret = *object;
//    avpriv_atomic_unlock();
//    return ret;
//}










///end h2645_parse.c

/***
 *
 *
 * hevc_parse
 *
 *    .parser_parse   = hevc_parse,
t hevc_parse(AVCodecParserContext *s, AVCodecContext *avctx,
                      const uint8_t **poutbuf, int *poutbuf_size,
                      const uint8_t *buf, int buf_size)
 *
 */

/***


 END of manual munging of libavcodec/ffmpeg
 */

/*
 jjustman-2019-10-12

atsc3_hevc_extract_extradata_nals_combined_ffmpegImpl
 borrowed from ffmpeg..thanks!

input:
 payload of hvcC ISOBMFF box

output:
 returns HEVC RBSP or NULL on error
 */


void ff_hevc_ps_uninit(HEVCParamSets *ps) {
    int i;

    for (i = 0; i < FF_ARRAY_ELEMS(ps->vps_list); i++) {
        av_buffer_unref(&ps->vps_list[i]);
    }
    for (i = 0; i < FF_ARRAY_ELEMS(ps->sps_list); i++) {
        av_buffer_unref(&ps->sps_list[i]);
    }
    for (i = 0; i < FF_ARRAY_ELEMS(ps->pps_list); i++) {
        av_buffer_unref(&ps->pps_list[i]);
    }

    ps->sps = NULL;
    ps->pps = NULL;
    ps->vps = NULL;
}

block_t *atsc3_hevc_extract_extradata_nals_combined_ffmpegImpl(block_t *hvcc_box) {
    block_t *nals_combined = NULL;

//mapping for libavcodec ad-hoc context
    AVCodecContext *avctx = (AVCodecContext *) calloc(1, sizeof(AVCodecContext));
    avctx->extradata = block_Get(hvcc_box);
    avctx->extradata_size = block_Len(hvcc_box);

    int i;
    int ret;

    HEVCParamSets ps;
    HEVCSEI sei;

    const HEVCVPS *vps = NULL;
    const HEVCPPS *pps = NULL;
    const HEVCSPS *sps = NULL;
    int is_nalff = 0;
    int nal_length_size = 0;

    uint8_t *vps_data = NULL;
    uint8_t *sps_data = NULL;
    uint8_t *pps_data = NULL;
    int vps_data_size = 0;
    int sps_data_size = 0;
    int pps_data_size = 0;

    memset(&ps, 0, sizeof(ps));
    memset(&sei, 0, sizeof(sei));

    ret = ff_hevc_decode_extradata(block_Get(hvcc_box), block_Len(hvcc_box), &ps, &sei, &is_nalff, &nal_length_size, 0, 1, avctx);
    if (ret < 0) {
        goto error;

    }


    for (i = 0; i < HEVC_MAX_VPS_COUNT; i++) {
        if (ps.vps_list[i]) {
            vps = (const HEVCVPS *) ps.vps_list[i]->data;
            break;
        }
    }

    for (i = 0; i < HEVC_MAX_PPS_COUNT; i++) {
        if (ps.pps_list[i]) {
            pps = (const HEVCPPS *) ps.pps_list[i]->data;
            break;
        }
    }

    if (pps) {
        if (ps.sps_list[pps->sps_id]) {
            sps = (const HEVCSPS *) ps.sps_list[pps->sps_id]->data;
        }
    }

    if (vps && pps && sps) {
        uint8_t *data;
        int data_size;

        if ((ret = h2645_ps_to_nalu(vps->data, vps->data_size, &vps_data, &vps_data_size)) < 0 ||
            (ret = h2645_ps_to_nalu(sps->data, sps->data_size, &sps_data, &sps_data_size)) < 0 ||
            (ret = h2645_ps_to_nalu(pps->data, pps->data_size, &pps_data, &pps_data_size)) < 0) {
            goto done;
        }

        data_size = vps_data_size + sps_data_size + pps_data_size;
        nals_combined = block_Alloc(data_size);

        block_Write(nals_combined, vps_data, vps_data_size);
        block_Write(nals_combined, sps_data, sps_data_size);
        block_Write(nals_combined, pps_data, pps_data_size);
        block_Rewind(nals_combined);

        _ATSC3_HEVC_NAL_EXTRACTOR_INFO("avctx: width: %d, height: %d", avctx->width, avctx->height);

    } else {
        _ATSC3_HEVC_NAL_EXTRACTOR_ERROR("Could not extract VPS/PPS/SPS from extradata");
    }
//will return NULL on error
    error:
    done:
    ff_hevc_ps_uninit(&ps);

    av_freep(&vps_data);
    av_freep(&sps_data);
    av_freep(&pps_data);

    av_freep(&avctx);

    return nals_combined;
}


/**
 *
 * process samples from mp4 block payload into annexb NAL rbsp
 */


block_t *atsc3_hevc_extract_mp4toannexb_filter_ffmpegImpl(block_t *sample, block_t *last_extradata_NAL_parsed) {
    block_t *sample_processed = block_Alloc(0);
    uint8_t start_code[] = {0x00, 0x00, 0x00, 0x01};
//assumed: TODO-fixme: jjustman-2019-10-12
    int nal_len = 4;

//HEVCBSFContext *s = ctx->priv_data;
    GetByteContext gb;

    int got_irap = 0;
    int i, ret = 0;

    block_Rewind(sample);

    bytestream2_init(&gb, block_Get(sample), block_Len(sample));

    while (bytestream2_get_bytes_left(&gb)) {
        uint32_t nalu_size = 0;
        int nalu_type;
        int is_irap, add_extradata, extra_size, prev_size;

        for (i = 0; i < nal_len; i++) {
            nalu_size = (nalu_size << 8) | bytestream2_get_byte(&gb);
        }

        nalu_type = (bytestream2_peek_byte(&gb) >> 1) & 0x3f;

/* prepend extradata to IRAP frames */
        is_irap = nalu_type >= 16 && nalu_type <= 23;
        add_extradata = is_irap && !got_irap;
        extra_size = add_extradata * block_Len(last_extradata_NAL_parsed);
        got_irap |= is_irap;

//do not allow us to over-run our gb.buffer when reading the NAL size...
        if (SIZE_MAX - nalu_size < 4 || SIZE_MAX - 4 - nalu_size < extra_size ||
            nalu_size > (gb.buffer_end - gb.buffer)) {
//WARN: probable buffer-overrun
            goto fail;
        }

        prev_size = block_Len(sample_processed);
//		not needed with block, just append the relevant start code (e.g. AV_WB32 1 hack)
//        ret = av_grow_packet(out, 4 + nalu_size + extra_size);
//        if (ret < 0)
//            goto fail;

        if (add_extradata) {
            block_Rewind(last_extradata_NAL_parsed);
            block_MergeNoRewind(sample_processed, last_extradata_NAL_parsed);
        }
        block_AppendFromBuf(sample_processed, start_code, 4); //        AV_WB32(out->data + prev_size + extra_size, 1);

        block_AppendFromBuf(sample_processed, gb.buffer, nalu_size); //block_write i->pos keeps track of this for us -   //bytestream2_get_buffer(&gb, out->data + prev_size + 4 + extra_size, nalu_size);
        gb.buffer += nalu_size;
    }

    block_Rewind(sample_processed);
    goto done;

    fail:

//TODO - try and recover as much as the sample as possible without overrunning the frame

    if (gb.buffer_end > gb.buffer) {
//recover N bytes..
        block_AppendFromBuf(sample_processed, start_code, 4);
        uint32_t len = gb.buffer_end - gb.buffer;

        block_AppendFromBuf(sample_processed, gb.buffer, len);
    } else {
//return an empty sample with poision bit
        uint8_t nal_poision[] = {0x80, 0x00, 0x00, 0x00};
        block_AppendFromBuf(sample_processed, nal_poision, 4);
    }

    block_Rewind(sample_processed);

    done:

    return sample_processed;
}


