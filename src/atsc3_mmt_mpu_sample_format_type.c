/*
 * atsc3_mmt_mpu_sample_format_type.c
 *
 *  Created on: Aug 10, 2019
 *      Author: jjustman
 */

#include "atsc3_mmt_mpu_sample_format_type.h"


int _MMTP_MPU_SAMPLE_FORMAT_DEBUG_ENABLED = 0;
int _MMTP_MPU_SAMPLE_FORMAT_TRACE_ENABLED = 0;



//mfu's have time and un-timed additional DU headers, so recalc to_read_packet_len after doing (uint8_t*)extract
//we use the du_header field
//parse data unit header here based upon mpu timed flag

/**
* MFU mpu_fragmentation_indicator==1's are prefixed by the following box, need to remove
*
aligned(8) class MMTHSample {
   unsigned int(32) sequence_number;
   if (is_timed) {

    //interior block is 152 bits, or 19 bytes
      signed int(8) trackrefindex;
      unsigned int(32) movie_fragment_sequence_number
      unsigned int(32) samplenumber;
      unsigned int(8)  priority;
      unsigned int(8)  dependency_counter;
      unsigned int(32) offset;
      unsigned int(32) length;
    //end interior block

      multiLayerInfo();
} else {
        //additional 2 bytes to chomp for non timed delivery
      unsigned int(16) item_ID;
   }
}

aligned(8) class multiLayerInfo extends Box("muli") {
   bit(1) multilayer_flag;
   bit(7) reserved0;
   if (multilayer_flag==1) {
       //32 bits
      bit(3) dependency_id;
      bit(1) depth_flag;
      bit(4) reserved1;
      bit(3) temporal_id;
      bit(1) reserved2;
      bit(4) quality_id;
      bit(6) priority_id;
   }  bit(10) view_id;
   else{
       //16bits
      bit(6) layer_id;
      bit(3) temporal_id;
      bit(7) reserved3;
} }
*/

void atsc3_mmt_mpu_sample_format_parse(block_t* raw_packet) {
    
//    uint8_t mmthsample_len;
//    uint8_t mmthsample_sequence_number[4];
//
//    if(mmtp_mpu_type_packet_header.mpu_timed_flag) {
//
//    //    uint16_t seconds;
//    //    uint16_t microseconds;
//        //on first init, p_sys->first_pts will always be 0 from calloc
////                    uint64_t pts = compute_relative_ntp32_pts(p_sys->first_pts, mpu_data_unit_payload_fragments_timed.mmtp_timestamp_s, mpu_data_unit_payload_fragments_timed.mmtp_timestamp_us);
////                    if(!p_sys->has_set_first_pts) {
////                        p_sys->first_pts = pts;
////                        p_sys->has_set_first_pts = 1;
////                    }
//
//        //build our PTS
//        //mpu_data_unit_payload_fragments_timed.pts = pts;
//
//        //112 bits in aggregate, 14 bytes
//        uint8_t timed_mfu_block[14];
//        buf = (uint8_t*)extract(buf, timed_mfu_block, 14);
//
//        mpu_data_unit_payload_fragments_timed.mpu_movie_fragment_sequence_number     = (timed_mfu_block[0] << 24) | (timed_mfu_block[1] << 16) | (timed_mfu_block[2]  << 8) | (timed_mfu_block[3]);
//        mpu_data_unit_payload_fragments_timed.mpu_sample_number                           = (timed_mfu_block[4] << 24) | (timed_mfu_block[5] << 16) | (timed_mfu_block[6]  << 8) | (timed_mfu_block[7]);
//        mpu_data_unit_payload_fragments_timed.mpu_offset                               = (timed_mfu_block[8] << 24) | (timed_mfu_block[9] << 16) | (timed_mfu_block[10] << 8) | (timed_mfu_block[11]);
//        mpu_data_unit_payload_fragments_timed.mpu_priority                             = timed_mfu_block[12];
//        mpu_data_unit_payload_fragments_timed.mpu_dep_counter                        = timed_mfu_block[13];
//        uint8_t* rewind_buf = buf;
//
//        //see if bento4 will handle this?
//        //parse out mmthsample block if this is our first fragment or we are a complete fragment,
//        if(mpu_data_unit_payload_fragments_timed.mpu_fragment_type == 2 &&
//            (mpu_data_unit_payload_fragments_timed.mpu_fragmentation_indicator == 0 ||
//                        mpu_data_unit_payload_fragments_timed.mpu_fragmentation_indicator == 1)) {
//
//            //MMTHSample does not subclass box...
//            //buf = (uint8_t*)extract(buf, &mmthsample_len, 1);
//            buf = (uint8_t*)extract(buf, mmthsample_sequence_number, 4);
//            mpu_data_unit_payload_fragments_timed.mmth_sequence_number = ntohl(*(uint32_t*)(mmthsample_sequence_number));
//
//            uint8_t mmthsample_timed_block[19];
//            buf = (uint8_t*)extract(buf, mmthsample_timed_block, 19);
//            int mmth_position=0;
//            mpu_data_unit_payload_fragments_timed.mmth_trackrefindex = mmthsample_timed_block[mmth_position++];
//            mpu_data_unit_payload_fragments_timed.mmth_movie_fragment_sequence_number = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));
//            mmth_position+=4;
//            mpu_data_unit_payload_fragments_timed.mmth_samplenumber = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));
//            mmth_position+=4;
//            mpu_data_unit_payload_fragments_timed.mmth_priority = mmthsample_timed_block[mmth_position++];
//            mpu_data_unit_payload_fragments_timed.mmth_dependency_counter = mmthsample_timed_block[mmth_position++];
//            //offset is from base of the containing mdat box (e.g. samplenumber 1 should have an offset of 8
//            mpu_data_unit_payload_fragments_timed.mmth_offset = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));
//            mmth_position+=4;
//            mpu_data_unit_payload_fragments_timed.mmth_length = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));
//
//                //hi skt!
//            if(mmthsample_sequence_number[0] == 'S' && mmthsample_sequence_number[1] == 'K' && mmthsample_sequence_number[2] == 'T') {
//                mpu_data_unit_payload_fragments_timed.mmth_sequence_number = mpu_data_unit_payload_fragments_timed.mpu_sequence_number;
//                mpu_data_unit_payload_fragments_timed.mmth_samplenumber = mpu_data_unit_payload_fragments_timed.mpu_sample_number;
//                mpu_data_unit_payload_fragments_timed.mmth_movie_fragment_sequence_number = mpu_data_unit_payload_fragments_timed.mpu_movie_fragment_sequence_number;
//                mpu_data_unit_payload_fragments_timed.mmth_offset = mpu_data_unit_payload_fragments_timed.mpu_offset + 8;
//            }
//
//            //read multilayerinfo
//            uint8_t multilayerinfo_box_length_block[4];
//            uint32_t multilayerinfo_box_length = 0;
//            uint32_t multilayerinfo_box_name;
//            uint8_t multilayer_flag;
//
//            uint32_t box_parsed_position = 0;
//
//            buf = (uint8_t*)extract(buf, multilayerinfo_box_length_block, 4);
//            multilayerinfo_box_length = ntohl(*(uint32_t*)(&multilayerinfo_box_length_block));
//            box_parsed_position+=4;
//
//
//            buf = (uint8_t*)extract(buf, (uint8_t*)&multilayerinfo_box_name, 4);
//            multilayerinfo_box_name = ntohl(*(uint32_t*)(&multilayerinfo_box_name));
//
//            box_parsed_position+=4;
//
//            //make sure multilayerinfo_box_name == muli
//            assert(multilayerinfo_box_name == _BOX_MFU_MULI);
//
//            buf = (uint8_t*)extract(buf, &multilayer_flag, 1);
//            box_parsed_position++;
//
//            int is_multilayer = (multilayer_flag >> 7) & 0x01;
//            //if MSB is 1, then read multilevel struct, otherwise just pull layer info...
//            if(is_multilayer) {
//                uint8_t multilayer_data_block[4];
//                buf = (uint8_t*)extract(buf, multilayer_data_block, 4);
//                box_parsed_position+=4;
//
//            } else {
//                uint8_t multilayer_layer_id_temporal_id[2];
//                buf = (uint8_t*)extract(buf, multilayer_layer_id_temporal_id, 2);
//                box_parsed_position+=2;
//            }
//
//            //we need at least 8 bytes for a proper isobmff box child
//            while(box_parsed_position < multilayerinfo_box_length - 8) {
//
//                //try and parse out known 'private' isobmff boxes prepended to this sample
//                uint8_t private_box_length_block[4];
//                uint32_t private_box_length;
//
//                uint32_t private_box_name;
//
//                buf = (uint8_t*)extract(buf, private_box_length_block, 4);
//                box_parsed_position+=4;
//
//                private_box_length = ntohl(*(uint32_t*)(&private_box_length_block));
//                buf = (uint8_t*)extract(buf, (uint8_t*)&private_box_name, 4);
//                private_box_name = ntohl(*(uint32_t*)(&private_box_name));
//
//                box_parsed_position+=4;
//
//                _MPU_TRACE("mpu mode (0x02), packet_id: %u, packet_seq_num: %u, timed mfu has child box size: %u, name: %c%c%c%c", mmtp_mpu_type_packet_header.mmtp_packet_id,
//                    mmtp_mpu_type_packet_header.packet_sequence_number,
//                    private_box_length,
//                    ((private_box_name >> 24) & 0xFF), ((private_box_name >> 16) & 0xFF), ((private_box_name >> 8) & 0xFF), (private_box_name & 0xFF));
//
//                if(private_box_name == _BOX_MFU_MJSD) {
//                    //parse out timing information and child boxes
//                    uint8_t sample_presentation_time_block[8];
//                    uint64_t sample_presentation_time;
//
//                    uint8_t sample_decode_time_block[8];
//                    uint64_t sample_decode_time;
//
//                    buf = (uint8_t*)extract(buf, sample_presentation_time_block, 8);
//                    sample_presentation_time = ntohll(*(uint64_t*)(&sample_presentation_time_block));
//
//                    box_parsed_position+=8;
//
//                    buf = (uint8_t*)extract(buf, sample_decode_time_block, 8);
//                    sample_decode_time = ntohll(*(uint64_t*)(&sample_decode_time_block));
//
//                    box_parsed_position+=8;
//
//                    _MPU_TRACE("mpu mode (0x02), MJSD, remaining child box size is: %u",  (multilayerinfo_box_length - box_parsed_position));
//
//        if((multilayerinfo_box_length - box_parsed_position) > 8) {
//
//          buf = (uint8_t*)extract(buf, private_box_length_block, 4);
//          box_parsed_position+=4;
//
//          private_box_length = ntohl(*(uint32_t*)(&private_box_length_block));
//          buf = (uint8_t*)extract(buf, (uint8_t*)&private_box_name, 4);
//          private_box_name = ntohl(*(uint32_t*)(&private_box_name));
//
//          box_parsed_position+=4;
//_MPU_INFO("!!!mpu mode (0x02), packet_id: %u, packet_seq_num: %u, MJSD  child box size: %u, name: %c%c%c%c", mmtp_mpu_type_packet_header.mmtp_packet_id,
//                    mmtp_mpu_type_packet_header.packet_sequence_number,
//                    private_box_length,
//                    ((private_box_name >> 24) & 0xFF), ((private_box_name >> 16) & 0xFF), ((private_box_name >> 8) & 0xFF), (private_box_name & 0xFF));
//
//
//        }
//                }
//            }
//
//            _MPU_TRACE("mpu mode (0x02), timed mfu has remaining payload: %u", (multilayerinfo_box_length - box_parsed_position));
//
//            //for any remaining muli box size, ignore as possibly corrupt
//            for(int i = box_parsed_position; i < multilayerinfo_box_length; i++) {
//                uint8_t muli_box_incomplete_byte;
//                buf = (uint8_t*)extract(buf, &muli_box_incomplete_byte, 1);
//            }
//
//            mpu_data_unit_payload_fragments_timed.mfu_mmth_sample_header_size = 4 + 19 + multilayerinfo_box_length;
//                _MPU_DEBUG("mpu mode (0x02), timed MFU, mfu_mmth_sample_header_size: %u, mpu_fragmentation_indicator: %d, movie_fragment_seq_num: %u, sample_num: %u, offset: %u, pri: %d, dep_counter: %d, multilayer: %d, mpu_sequence_number: %u",
//                mpu_data_unit_payload_fragments_timed.mfu_mmth_sample_header_size,
//                mpu_data_unit_payload_fragments_timed.mpu_fragmentation_indicator,
//                mpu_data_unit_payload_fragments_timed.mpu_movie_fragment_sequence_number,
//                mpu_data_unit_payload_fragments_timed.mpu_sample_number,
//                mpu_data_unit_payload_fragments_timed.mpu_offset,
//                mpu_data_unit_payload_fragments_timed.mpu_priority,
//                mpu_data_unit_payload_fragments_timed.mpu_dep_counter,
//                is_multilayer,
//                mpu_data_unit_payload_fragments_timed.mpu_sequence_number);
//        } else {
//            //jdj-2019-06-13 -- HACK -- mpu offset is incorrectly set at 34 bytes for fixed header, but we need to honor muli box size here
//            //mpu_data_unit_payload_fragments_timed.mpu_offset -= 24;
//            //jdj-2019-06-13 -- end HACK --
//
//            _MPU_DEBUG("mpu mode (0x02), timed MFU, mpu_fragmentation_indicator: %d, movie_fragment_seq_num: %u, sample_num: %u, offset: %u, pri: %d, dep_counter: %d, mpu_sequence_number: %u",
//                mpu_data_unit_payload_fragments_timed.mpu_fragmentation_indicator,
//                mpu_data_unit_payload_fragments_timed.mpu_movie_fragment_sequence_number,
//                mpu_data_unit_payload_fragments_timed.mpu_sample_number,
//                mpu_data_unit_payload_fragments_timed.mpu_offset,
//                mpu_data_unit_payload_fragments_timed.mpu_priority,
//                mpu_data_unit_payload_fragments_timed.mpu_dep_counter,
//                mpu_data_unit_payload_fragments_timed.mpu_sequence_number);
//        }
//        //end mfu box read
//
//        to_read_packet_length = udp_raw_buf_size - (buf - raw_buf);
//    } else {
//        uint8_t non_timed_mfu_block[4];
//        uint32_t non_timed_mfu_item_id;
//        //only 32 bits
//        buf = (uint8_t*)extract(buf, non_timed_mfu_block, 4);
//        mpu_data_unit_payload_fragments_nontimed.non_timed_mfu_item_id = (non_timed_mfu_block[0] << 24) | (non_timed_mfu_block[1] << 16) | (non_timed_mfu_block[2] << 8) | non_timed_mfu_block[3];
//
//        if(mpu_data_unit_payload_fragments_nontimed.mpu_fragmentation_indicator == 1) {
//            //MMTHSample does not subclass box...
//            //buf = (uint8_t*)extract(buf, &mmthsample_len, 1);
//
//            buf = (uint8_t*)(uint8_t*)extract(buf, mmthsample_sequence_number, 4);
//
//            uint8_t mmthsample_item_id[2];
//            buf = (uint8_t*)extract(buf, mmthsample_sequence_number, 2);
//            //end reading of mmthsample box
//        }
//
//        _MPU_DEBUG("mpu mode (0x02), non-timed MFU, item_id is: %u", mpu_data_unit_payload_fragments_nontimed.non_timed_mfu_item_id);
//        to_read_packet_length = udp_raw_buf_size - (buf - raw_buf);
//    }
//
//    __LOG_TRACE( p_demux, "%d:before reading fragment packet: reading length: %d (mmtp_raw_packet_size: %d, buf: %p, raw_buf:%p)",
//            __LINE__,
//            to_read_packet_length,
//            mmtp_raw_packet_size,
//            buf,
//            raw_buf);
//
//    block_t *tmp_mpu_fragment = block_Alloc(to_read_packet_length);
//    _MPU_TRACE("creating tmp_mpu_fragment, setting block_t->i_buffer to: %d", to_read_packet_length);
//
//    buf = (uint8_t*)extract(buf, tmp_mpu_fragment->p_buffer, to_read_packet_length);
//    tmp_mpu_fragment->i_pos = to_read_packet_length;
//
//
//    mmtp_mpu_type_packet_header.mpu_data_unit_payload = tmp_mpu_fragment;
//
//    //send off only the CLEAN mdat payload from our MFU
//    remainingPacketLen = udp_raw_buf_size - (buf - raw_buf);
//    _MPU_TRACE( "after reading fragment packet: remainingPacketLen: %d", remainingPacketLen);

}
