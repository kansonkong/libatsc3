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

atsc3_isobmff_mdhd_box_t* atsc3_isobmff_box_parser_tools_parse_mdhd_timescale_from_block_t(block_t* isobmff_fragment_block) {

    atsc3_isobmff_mdhd_box_t* atsc3_isobmff_mdhd_box = NULL;

    block_Rewind(isobmff_fragment_block);
    uint8_t* mdhd_ptr = block_Get(isobmff_fragment_block);

    for (int i = 0; !(atsc3_isobmff_mdhd_box) && (i < isobmff_fragment_block->p_size - 4); i++) {
        block_Seek(isobmff_fragment_block, i);

        _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_TRACE("atsc3_avc1_hevc_nal_extractor_parse_from_mpu_metadata_block_t: searching for mdhd, position: %d, checking: 0x%02x (%c), 0x%02x (%c), 0x%02x (%c), 0x%02x (%c)",
                                                 i, mdhd_ptr[i], mdhd_ptr[i], mdhd_ptr[i + 1], mdhd_ptr[i + 1], mdhd_ptr[i + 2], mdhd_ptr[i + 2], mdhd_ptr[i + 3], mdhd_ptr[i + 3]);

        //look for our fourcc
        if (mdhd_ptr[i] == 'm' && mdhd_ptr[i + 1] == 'd' && mdhd_ptr[i + 2] == 'h' && mdhd_ptr[i + 3] == 'd') {
            _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_DEBUG("atsc3_hevc_nal_extractor_parse_from_mpu_metadata_block_t: mdhd: found matching at position: %d", i);
            block_Seek(isobmff_fragment_block, i-4);
            atsc3_isobmff_mdhd_box = atsc3_isobmff_mdhd_box_new();

            atsc3_isobmff_mdhd_box->box_size = block_Read_uint32_ntohl(isobmff_fragment_block);
            atsc3_isobmff_mdhd_box->type = block_Read_uint32_ntohl(isobmff_fragment_block);
            atsc3_isobmff_mdhd_box->version = block_Read_uint8(isobmff_fragment_block);
            atsc3_isobmff_mdhd_box->flags = block_Read_uint32_bitlen(isobmff_fragment_block, 24);

            if(atsc3_isobmff_mdhd_box->version == 1) {
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v1.creation_time = block_Read_uint64_ntohll(isobmff_fragment_block);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v1.modification_time = block_Read_uint64_ntohll(isobmff_fragment_block);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v1.timescale = block_Read_uint32_ntohl(isobmff_fragment_block);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v1.duration = block_Read_uint64_ntohll(isobmff_fragment_block);
            } else if(atsc3_isobmff_mdhd_box->version == 0) {
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v0.creation_time = block_Read_uint32_ntohl(isobmff_fragment_block);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v0.modification_time = block_Read_uint32_ntohl(isobmff_fragment_block);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v0.timescale = block_Read_uint32_ntohl(isobmff_fragment_block);
                atsc3_isobmff_mdhd_box->atsc3_isobmff_mdhd_box_v0.duration = block_Read_uint32_ntohl(isobmff_fragment_block);
            }
            atsc3_isobmff_mdhd_box->pad = block_Read_uint8_bitlen(isobmff_fragment_block, 1);

            atsc3_isobmff_mdhd_box->language[0] = block_Read_uint8_bitlen(isobmff_fragment_block, 5) + 0x60;
            atsc3_isobmff_mdhd_box->language[1] = block_Read_uint8_bitlen(isobmff_fragment_block, 5) + 0x60;
            atsc3_isobmff_mdhd_box->language[2] = block_Read_uint8_bitlen(isobmff_fragment_block, 5) + 0x60;

            atsc3_isobmff_mdhd_box->pre_defined = block_Read_uint16_ntohs(isobmff_fragment_block);

            break;
        }
    }

    return atsc3_isobmff_mdhd_box;
}


