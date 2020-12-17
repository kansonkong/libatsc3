/*
 * atsc3_mmt_mpu_sample_format_parser.c
 *
 *  Created on: Aug 10, 2019
 *      Author: jjustman
 *
 *  updated: 2020-12-16 - refactored to use block_t and block_Read_uxxx to prevent buffer over-read for real-world MMT depacketization...
 */

#include "atsc3_mmt_mpu_sample_format_parser.h"

int _MMTP_MPU_SAMPLE_FORMAT_DEBUG_ENABLED = 0;
int _MMTP_MPU_SAMPLE_FORMAT_TRACE_ENABLED = 0;

//mfu's have time and un-timed additional DU headers, so recalc to_read_packet_len after doing (uint8_t*)extract
//we use the du_header field
//parse data unit header here based upon mpu timed flag

/*
 * atsc3_mmt_mpu_sample_format_parse: parse out the MMTP payload for MPU (mfu) mode
 *
 * See ISO23008-1:2017 9.3.2.2 MMTP payload header (i.e. DU_header)
 */
mmtp_mpu_packet_t* atsc3_mmt_mpu_sample_format_parse(mmtp_mpu_packet_t* mmtp_mpu_packet, block_t* raw_packet) {

    if(mmtp_mpu_packet->mpu_timed_flag) {

        mmtp_mpu_packet->movie_fragment_sequence_number = block_Read_uint32_ntohl(raw_packet);
        mmtp_mpu_packet->sample_number = block_Read_uint32_ntohl(raw_packet);
        mmtp_mpu_packet->offset = block_Read_uint32_ntohl(raw_packet);

        mmtp_mpu_packet->priority = block_Read_uint8(raw_packet);
        mmtp_mpu_packet->dep_counter = block_Read_uint8(raw_packet);
    } else {
        mmtp_mpu_packet->item_id = block_Read_uint32_ntohl(raw_packet);
        //jjustman-2020-12-16 - not supported in ATSC 3.0
        __MMTP_MPU_SAMPLE_FORMAT_WARN("atsc3_mmt_mpu_sample_format_parse: mpu mode (0x02), but NOT TIMED MFU!, packet_id: %d, mpu_sequence_number: %d, frag_counter: %d, parsed item id: 0x%08x",
                                       mmtp_mpu_packet->mmtp_packet_id,
                                       mmtp_mpu_packet->mpu_sequence_number,
                                       mmtp_mpu_packet->mpu_fragment_counter,
                                       mmtp_mpu_packet->item_id);
    }

    //parse out mmthsample block if this is our first fragment or we are a complete fragment,
    if(mmtp_mpu_packet->mpu_fragment_type == 0x2 && (mmtp_mpu_packet->mpu_fragmentation_indicator == 0x0 || mmtp_mpu_packet->mpu_fragmentation_indicator == 0x1)) {

        //MMTHSample does not subclass box...but its multiLayerInfo() does!
        //See ISO23008-1:2017 - 8.3 Sample Format: MMTHSample

        mmtp_mpu_packet->mmthsample_header = calloc(1, sizeof(mmthsample_header_t));

        mmtp_mpu_packet->mmthsample_header->sequence_number = block_Read_uint32_ntohl(raw_packet);

        mmtp_mpu_packet->mmthsample_header->trackrefindex = (int8_t) block_Read_uint8(raw_packet);
        mmtp_mpu_packet->mmthsample_header->movie_fragment_sequence_number = block_Read_uint32_ntohl(raw_packet);
        mmtp_mpu_packet->mmthsample_header->samplenumber = block_Read_uint32_ntohl(raw_packet);
        mmtp_mpu_packet->mmthsample_header->priority = block_Read_uint8(raw_packet);
        mmtp_mpu_packet->mmthsample_header->dependency_counter = block_Read_uint8(raw_packet);

        //offset is from base of the containing mdat box (e.g. samplenumber 1 should have an offset of 8
        mmtp_mpu_packet->mmthsample_header->offset = block_Read_uint32_ntohl(raw_packet);
        mmtp_mpu_packet->mmthsample_header->length = block_Read_uint32_ntohl(raw_packet);

//             //hi skt!
//            if(mmthsample_sequence_number[0] == 'S' && mmthsample_sequence_number[1] == 'K' && mmthsample_sequence_number[2] == 'T') {
//                mmtp_mpu_packet->mmthsample_header->sequence_number  = mmtp_mpu_packet->mpu_sequence_number;
//                mmtp_mpu_packet->mmthsample_header->samplenumber = mmtp_mpu_packet->sample_number;
//                mmtp_mpu_packet->mmthsample_header->movie_fragment_sequence_number = mmtp_mpu_packet->movie_fragment_sequence_number;
//                mmtp_mpu_packet->mmthsample_header->offset = mmtp_mpu_packet->offset + 8;
//            }

        //read multilayerinfo - see atsc3_mmt_multiLayerInfoBox for ISO 14496 "Box" defns.
        uint32_t muliBoxStartPosition = raw_packet->i_pos;

        mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.box_size = block_Read_uint32_ntohl(raw_packet);
        mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.box_type = block_Read_uint32_ntohl(raw_packet);

        //iso14496 - 4.2 weirdness when size == 1
        if(mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.box_size == 1) {
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.box_size_large = block_Read_uint64_ntohll(raw_packet);
        }

        if(mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.box_type != _BOX_MFU_MULI) {
            __MMTP_MPU_SAMPLE_FORMAT_ERROR("atsc3_mmt_mpu_sample_format_parse: multilayerinfo_box_name (0x%08x) != _BOX_MFU_MULI (0x%08x), returning NULL!", mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.box_type, _BOX_MFU_MULI);
            return NULL;
        }

        mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.multilayer_flag = block_Read_uint8_bitlen(raw_packet, 1);
        mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.reserved0       = block_Read_uint8_bitlen(raw_packet, 7);

        //if multilayer_flag is 1, then read multilevel struct, otherwise just pull layer info...
        if(mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.multilayer_flag) {
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.dependency_id            = block_Read_uint8_bitlen(raw_packet, 3);
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.depth_flag               = block_Read_uint8_bitlen(raw_packet, 1);
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.reserved1                = block_Read_uint8_bitlen(raw_packet, 4);
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.multilayer_temporal_id   = block_Read_uint8_bitlen(raw_packet, 3);
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.reserved2                = block_Read_uint8_bitlen(raw_packet, 1);
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.quality_id               = block_Read_uint8_bitlen(raw_packet, 4);
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.priority_id              = block_Read_uint8_bitlen(raw_packet, 6);
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.view_id                  = block_Read_uint16_bitlen(raw_packet, 10);
        } else {
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.layer_id                 = block_Read_uint8_bitlen(raw_packet, 6);
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.temporal_id              = block_Read_uint8_bitlen(raw_packet, 3);
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.reserved3                = block_Read_uint8_bitlen(raw_packet, 7);
        }

        //we need at least 8 bytes for a proper isobmff box child
        while(block_Remaining_size(raw_packet) && (mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.box_size - (raw_packet->i_pos - muliBoxStartPosition)) > 8) {
            //try and parse out known 'private' isobmff boxes prepended to this sample
            //jjustman-2020-12-16 - TODO: construct these as first class models for MJSD and MJGP
            uint32_t privateBoxStartPosition = raw_packet->i_pos;
            uint32_t private_box_size   = block_Read_uint32_ntohl(raw_packet);
            uint32_t private_box_name   = block_Read_uint32_ntohl(raw_packet);

            __MMTP_MPU_SAMPLE_FORMAT_TRACE("mpu mode (0x02), packet_id: %u, packet_seq_num: %u, timed mfu has child box size: %u, name: %c%c%c%c",
                mmtp_mpu_packet->mmtp_packet_id,
                mmtp_mpu_packet->packet_sequence_number,
                private_box_size,
                ((private_box_name >> 24) & 0xFF), ((private_box_name >> 16) & 0xFF), ((private_box_name >> 8) & 0xFF), (private_box_name & 0xFF));

            if(private_box_name == _BOX_MFU_MJSD) {
                //parse out timing information and child boxes
                uint64_t sample_presentation_time = block_Read_uint64_ntohll(raw_packet);
                uint64_t sample_decode_time       = block_Read_uint64_ntohll(raw_packet);

                int32_t box_mfu_mjsd_remaining_length = (private_box_size - (raw_packet->i_pos - privateBoxStartPosition));
                __MMTP_MPU_SAMPLE_FORMAT_TRACE("mpu mode (0x02), MJSD, remaining child box size is: %d", box_mfu_mjsd_remaining_length);

                if(box_mfu_mjsd_remaining_length > 8) {
                    uint32_t privateBoxStartPosition_mjgp = raw_packet->i_pos;
                    uint32_t private_box_size_mjgp = block_Read_uint32_ntohl(raw_packet);
                    uint32_t private_box_name_mjgp   = block_Read_uint32_ntohl(raw_packet);

                    //skip interior remaining size here
                    int32_t box_mfu_mjgp_remaining_length = (private_box_size_mjgp - (raw_packet->i_pos - privateBoxStartPosition_mjgp));
                    block_Seek_Relative(raw_packet, box_mfu_mjgp_remaining_length);

                    __MMTP_MPU_SAMPLE_FORMAT_INFO("atsc3_mmt_mpu_sample_format_parse: mpu mode (0x02), packet_id: %u, packet_seq_num: %u, MJSD child box size: %u, name: %c%c%c%c (MJGP), consuming inner bytes: %d",
                                                  mmtp_mpu_packet->mmtp_packet_id,
                                                  mmtp_mpu_packet->packet_sequence_number,
                                                  private_box_size_mjgp,
                                                  ((private_box_name_mjgp >> 24) & 0xFF), ((private_box_name_mjgp >> 16) & 0xFF), ((private_box_name_mjgp >> 8) & 0xFF), (private_box_name_mjgp & 0xFF),
                                                  box_mfu_mjgp_remaining_length);

                }
            }
        }

        int32_t muli_box_bytes_remaining = (mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.box_size - (raw_packet->i_pos - muliBoxStartPosition));

        __MMTP_MPU_SAMPLE_FORMAT_TRACE("atsc3_mmt_mpu_sample_format_parse: mpu mode (0x02), timed mfu has remaining payload: %d", muli_box_bytes_remaining);
        //skip over any last remaining bytes that may be in children of the muli box...
        if(muli_box_bytes_remaining > 0) {
            block_Seek_Relative(raw_packet, muli_box_bytes_remaining);
        }

        //yuck...
        mmtp_mpu_packet->mmthsample_header->mfu_mmth_sample_header_size = 4 + 19 + mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.box_size;

        __MMTP_MPU_SAMPLE_FORMAT_DEBUG("atsc3_mmt_mpu_sample_format_parse: mpu mode (0x02), timed MFU, mfu_mmth_sample_header_size: %u, mpu_fragmentation_indicator: %d, movie_fragment_seq_num: %u, sample_num: %u, offset: %u, pri: %d, dep_counter: %d, multilayer: %d, mpu_sequence_number: %u",
            mmtp_mpu_packet->mmthsample_header->mfu_mmth_sample_header_size,
            mmtp_mpu_packet->mpu_fragmentation_indicator,
            mmtp_mpu_packet->movie_fragment_sequence_number,
            mmtp_mpu_packet->sample_number,
            mmtp_mpu_packet->offset,
            mmtp_mpu_packet->priority,
            mmtp_mpu_packet->dep_counter,
            mmtp_mpu_packet->mmthsample_header->atsc3_mmt_multiLayerInfoBox.multilayer_flag,
            mmtp_mpu_packet->mpu_sequence_number);
    }
    //end MMTHSample and muli box box read for MFU processing

    return mmtp_mpu_packet;
}
