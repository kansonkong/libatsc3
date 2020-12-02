//
// Created by Jason Justman on 12/1/20.
//

#include "atsc3_isobmff_box_parser_tools.h"

int _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_INFO_ENABLED = 0;
int _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_DEBUG_ENABLED = 0;
int _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_TRACE_ENABLED = 0;


atsc3_isobmff_mdhd_box_t* atsc3_isobmff_mdhd_box_new() {
    atsc3_isobmff_mdhd_box_t* atsc3_isobmff_mdhd_box = calloc(1, sizeof(atsc3_isobmff_mdhd_box_t));

    return atsc3_isobmff_mdhd_box;
}

void atsc3_isobmff_mdhd_box_free(atsc3_isobmff_mdhd_box_t** atsc3_isobmff_mdhd_box_p) {
    if(atsc3_isobmff_mdhd_box_p) {
        atsc3_isobmff_mdhd_box_t* atsc3_isobmff_mdhd_box = *atsc3_isobmff_mdhd_box_p;
        if(atsc3_isobmff_mdhd_box) {

            free(atsc3_isobmff_mdhd_box);
            atsc3_isobmff_mdhd_box = NULL;
        }
        *atsc3_isobmff_mdhd_box_p = NULL;
    }
}

atsc3_isobmff_mdhd_box_t* atsc3_isobmff_box_parser_tools_parse_mdhd_from_block_t(block_t* isobmff_fragment_block_t) {

    atsc3_isobmff_mdhd_box_t* atsc3_isobmff_mdhd_box = NULL;

    uint8_t* mdhd_ptr = block_Get(isobmff_fragment_block_t);

    for (int i = 0; !(atsc3_isobmff_mdhd_box) && (i < isobmff_fragment_block_t->p_size - 4); i++) {
        block_Seek(isobmff_fragment_block_t, i);

        _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_TRACE("atsc3_isobmff_box_parser_tools_parse_mdhd_from_block_t: searching for mdhd, position: %d, checking: 0x%02x (%c), 0x%02x (%c), 0x%02x (%c), 0x%02x (%c)",
                                                 i, mdhd_ptr[i], mdhd_ptr[i], mdhd_ptr[i + 1], mdhd_ptr[i + 1], mdhd_ptr[i + 2], mdhd_ptr[i + 2], mdhd_ptr[i + 3], mdhd_ptr[i + 3]);

        //look for our fourcc
        if (mdhd_ptr[i] == 'm' && mdhd_ptr[i + 1] == 'd' && mdhd_ptr[i + 2] == 'h' && mdhd_ptr[i + 3] == 'd') {
            _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_DEBUG("atsc3_isobmff_box_parser_tools_parse_mdhd_from_block_t: mdhd: found matching at position: %d", i);
            block_Seek(isobmff_fragment_block_t, i - 4);
            atsc3_isobmff_mdhd_box = atsc3_isobmff_mdhd_box_new();

            atsc3_isobmff_mdhd_box->box_size = block_Read_uint32_ntohl(isobmff_fragment_block_t);
            atsc3_isobmff_mdhd_box->type = block_Read_uint32_ntohl(isobmff_fragment_block_t);
            atsc3_isobmff_mdhd_box->version = block_Read_uint8(isobmff_fragment_block_t);
            atsc3_isobmff_mdhd_box->flags = block_Read_uint32_bitlen(isobmff_fragment_block_t, 24);

            if(atsc3_isobmff_mdhd_box->version == 1) {
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v1.creation_time = block_Read_uint64_ntohll(isobmff_fragment_block_t);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v1.modification_time = block_Read_uint64_ntohll(isobmff_fragment_block_t);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v1.timescale = block_Read_uint32_ntohl(isobmff_fragment_block_t);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v1.duration = block_Read_uint64_ntohll(isobmff_fragment_block_t);
            } else if(atsc3_isobmff_mdhd_box->version == 0) {
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v0.creation_time = block_Read_uint32_ntohl(isobmff_fragment_block_t);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v0.modification_time = block_Read_uint32_ntohl(isobmff_fragment_block_t);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v0.timescale = block_Read_uint32_ntohl(isobmff_fragment_block_t);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v0.duration = block_Read_uint32_ntohl(isobmff_fragment_block_t);
            }
            atsc3_isobmff_mdhd_box->pad = block_Read_uint8_bitlen(isobmff_fragment_block_t, 1);

            atsc3_isobmff_mdhd_box->language[0] = block_Read_uint8_bitlen(isobmff_fragment_block_t, 5) + 0x60;
            atsc3_isobmff_mdhd_box->language[1] = block_Read_uint8_bitlen(isobmff_fragment_block_t, 5) + 0x60;
            atsc3_isobmff_mdhd_box->language[2] = block_Read_uint8_bitlen(isobmff_fragment_block_t, 5) + 0x60;

            atsc3_isobmff_mdhd_box->pre_defined = block_Read_uint16_ntohs(isobmff_fragment_block_t);

            break;
        }
    }

    return atsc3_isobmff_mdhd_box;
}

//tfhd

atsc3_isobmff_tfhd_box_t* atsc3_isobmff_tfhd_box_new() {
    atsc3_isobmff_tfhd_box_t* atsc3_isobmff_tfhd_box = calloc(1, sizeof(atsc3_isobmff_tfhd_box_t));

    return atsc3_isobmff_tfhd_box;
}

void atsc3_isobmff_tfhd_box_free(atsc3_isobmff_tfhd_box_t** atsc3_isobmff_tfhd_box_p) {
    if(atsc3_isobmff_tfhd_box_p) {
        atsc3_isobmff_tfhd_box_t* atsc3_isobmff_tfhd_box = *atsc3_isobmff_tfhd_box_p;
        if(atsc3_isobmff_tfhd_box) {

            free(atsc3_isobmff_tfhd_box);
            atsc3_isobmff_tfhd_box = NULL;
        }
        *atsc3_isobmff_tfhd_box_p = NULL;
    }
}

atsc3_isobmff_tfhd_box_t* atsc3_isobmff_box_parser_tools_parse_tfhd_from_block_t(block_t* isobmff_movie_fragment_metadata_block_t) {

    atsc3_isobmff_tfhd_box_t* atsc3_isobmff_tfhd_box = NULL;

    uint8_t* tfhd_ptr = block_Get(isobmff_movie_fragment_metadata_block_t);

    for (int i = 0; !(atsc3_isobmff_tfhd_box) && (i < isobmff_movie_fragment_metadata_block_t->p_size - 4); i++) {
        block_Seek(isobmff_movie_fragment_metadata_block_t, i);

        _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_TRACE("atsc3_isobmff_box_parser_tools_parse_tfhd_from_block_t: searching for tfhd, position: %d, checking: 0x%02x (%c), 0x%02x (%c), 0x%02x (%c), 0x%02x (%c)",
                                              i, tfhd_ptr[i], tfhd_ptr[i], tfhd_ptr[i + 1], tfhd_ptr[i + 1], tfhd_ptr[i + 2], tfhd_ptr[i + 2], tfhd_ptr[i + 3], tfhd_ptr[i + 3]);

        //look for our fourcc
        if (tfhd_ptr[i] == 't' && tfhd_ptr[i + 1] == 'f' && tfhd_ptr[i + 2] == 'h' && tfhd_ptr[i + 3] == 'd') {
            _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_DEBUG("atsc3_isobmff_box_parser_tools_parse_tfhd_from_block_t: tfhd: found matching at position: %d", i);
            block_Seek(isobmff_movie_fragment_metadata_block_t, i - 4);
            atsc3_isobmff_tfhd_box = atsc3_isobmff_tfhd_box_new();

            atsc3_isobmff_tfhd_box->box_size = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);
            atsc3_isobmff_tfhd_box->type = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);
            atsc3_isobmff_tfhd_box->version = block_Read_uint8(isobmff_movie_fragment_metadata_block_t);
            atsc3_isobmff_tfhd_box->flags = block_Read_uint32_bitlen(isobmff_movie_fragment_metadata_block_t, 24);

            atsc3_isobmff_tfhd_box->track_id = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);

            if(atsc3_isobmff_tfhd_box->flags & 0x000001) {
                atsc3_isobmff_tfhd_box->base_data_offset = block_Read_uint64_ntohll(isobmff_movie_fragment_metadata_block_t);
            }

            if(atsc3_isobmff_tfhd_box->flags & 0x000002) {
                atsc3_isobmff_tfhd_box->sample_description_index = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);
            }

            if(atsc3_isobmff_tfhd_box->flags & 0x000008) {
                atsc3_isobmff_tfhd_box->default_sample_duration = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);
                atsc3_isobmff_tfhd_box->flag_default_sample_duration = true;
            }

            if(atsc3_isobmff_tfhd_box->flags & 0x000010) {
                atsc3_isobmff_tfhd_box->default_sample_size = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);
            }

            if(atsc3_isobmff_tfhd_box->flags & 0x000020) {
                atsc3_isobmff_tfhd_box->default_sample_flags = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);
            }

            if(atsc3_isobmff_tfhd_box->flags & 0x010000) {
                atsc3_isobmff_tfhd_box->flag_duration_is_empty = true;
            }

            break;
        }
    }

    return atsc3_isobmff_tfhd_box;
}

//trun

atsc3_isobmff_trun_box_t* atsc3_isobmff_trun_box_new() {
    atsc3_isobmff_trun_box_t* atsc3_isobmff_trun_box = calloc(1, sizeof(atsc3_isobmff_trun_box_t));

    return atsc3_isobmff_trun_box;
}

void atsc3_isobmff_trun_box_free(atsc3_isobmff_trun_box_t** atsc3_isobmff_trun_box_p) {
    if(atsc3_isobmff_trun_box_p) {
        atsc3_isobmff_trun_box_t* atsc3_isobmff_trun_box = *atsc3_isobmff_trun_box_p;
        if(atsc3_isobmff_trun_box) {

            free(atsc3_isobmff_trun_box);
            atsc3_isobmff_trun_box = NULL;
        }
        *atsc3_isobmff_trun_box_p = NULL;
    }
}

atsc3_isobmff_trun_box_t* atsc3_isobmff_box_parser_tools_parse_trun_from_block_t(block_t* isobmff_movie_fragment_metadata_block_t) {

    atsc3_isobmff_trun_box_t* atsc3_isobmff_trun_box = NULL;

    uint8_t* trun_ptr = block_Get(isobmff_movie_fragment_metadata_block_t);

    for (int i = 0; !(atsc3_isobmff_trun_box) && (i < isobmff_movie_fragment_metadata_block_t->p_size - 4); i++) {
        block_Seek(isobmff_movie_fragment_metadata_block_t, i);

        _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_TRACE("atsc3_isobmff_box_parser_tools_parse_trun_from_block_t: searching for trun, position: %d, checking: 0x%02x (%c), 0x%02x (%c), 0x%02x (%c), 0x%02x (%c)",
                                              i, trun_ptr[i], trun_ptr[i], trun_ptr[i + 1], trun_ptr[i + 1], trun_ptr[i + 2], trun_ptr[i + 2], trun_ptr[i + 3], trun_ptr[i + 3]);

        //look for our fourcc
        if (trun_ptr[i] == 't' && trun_ptr[i + 1] == 'r' && trun_ptr[i + 2] == 'u' && trun_ptr[i + 3] == 'n') {
            _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_DEBUG("atsc3_isobmff_box_parser_tools_parse_trun_from_block_t: trun: found matching at position: %d", i);
            block_Seek(isobmff_movie_fragment_metadata_block_t, i - 4);
            atsc3_isobmff_trun_box = atsc3_isobmff_trun_box_new();

            atsc3_isobmff_trun_box->box_size = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);
            atsc3_isobmff_trun_box->type = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);
            atsc3_isobmff_trun_box->version = block_Read_uint8(isobmff_movie_fragment_metadata_block_t);
            atsc3_isobmff_trun_box->flags = block_Read_uint32_bitlen(isobmff_movie_fragment_metadata_block_t, 24);


            atsc3_isobmff_trun_box->sample_count = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);

            if(atsc3_isobmff_trun_box->flags & 0x000001) {
                atsc3_isobmff_trun_box->data_offset = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);
                atsc3_isobmff_trun_box->flag_data_offset_present = true;
            }

            if(atsc3_isobmff_trun_box->flags & 0x000004) {
                atsc3_isobmff_trun_box->first_sample_flags = block_Read_uint32_ntohl(isobmff_movie_fragment_metadata_block_t);
                atsc3_isobmff_trun_box->flag_first_sample_flags_present = true;
            }

            if(atsc3_isobmff_trun_box->flags & 0x000100) {
                atsc3_isobmff_trun_box->flag_sample_duration_present = true;
            }
            if(atsc3_isobmff_trun_box->flags & 0x000200) {
                atsc3_isobmff_trun_box->flag_sample_size_present = true;
            }
            if(atsc3_isobmff_trun_box->flags & 0x000400) {
                atsc3_isobmff_trun_box->flag_sample_flags_present = true;
            }
            if(atsc3_isobmff_trun_box->flags & 0x000800) {
                atsc3_isobmff_trun_box->flag_sample_composition_time_offset_present = true;
            }

            if(atsc3_isobmff_trun_box->sample_count) {
                //read first entry here

            }



            break;
        }
    }

    return atsc3_isobmff_trun_box;
}



