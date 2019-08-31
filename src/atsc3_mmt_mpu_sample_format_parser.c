/*
 * atsc3_mmt_mpu_sample_format_parser.c
 *
 *  Created on: Aug 10, 2019
 *      Author: jjustman
 */

#include "atsc3_mmt_mpu_sample_format_parser.h"


int _MMTP_MPU_SAMPLE_FORMAT_DEBUG_ENABLED = 0;
int _MMTP_MPU_SAMPLE_FORMAT_TRACE_ENABLED = 0;


//mfu's have time and un-timed additional DU headers, so recalc to_read_packet_len after doing (uint8_t*)extract
//we use the du_header field
//parse data unit header here based upon mpu timed flag

/**
* MFU mpu_fragmentation_indicator==1's are prefixed by the following box, need to remove and process
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

//    mmthsample_header_t* mmthsample_header;

void atsc3_mmt_mpu_sample_format_parse(mmtp_mpu_packet_t* mmtp_mpu_packet, block_t* raw_packet) {
    
    int mmtp_mpu_payload_length = block_Remaining_size(raw_packet);
    uint8_t* udp_raw_buf = block_Get(raw_packet);
    uint8_t* buf = udp_raw_buf;

    if(mmtp_mpu_packet->mpu_timed_flag) {
        
        //112 bits in aggregate, 14 bytes
        uint8_t timed_mfu_block[14];
        buf = (uint8_t*)extract(buf, timed_mfu_block, 14);

        mmtp_mpu_packet->movie_fragment_sequence_number     = (timed_mfu_block[0] << 24) | (timed_mfu_block[1] << 16) | (timed_mfu_block[2]  << 8) | (timed_mfu_block[3]);
        mmtp_mpu_packet->sample_number = (timed_mfu_block[4] << 24) | (timed_mfu_block[5] << 16) | (timed_mfu_block[6]  << 8) | (timed_mfu_block[7]);
        mmtp_mpu_packet->offset = (timed_mfu_block[8] << 24) | (timed_mfu_block[9] << 16) | (timed_mfu_block[10] << 8) | (timed_mfu_block[11]);
        mmtp_mpu_packet->priority  = timed_mfu_block[12];
        mmtp_mpu_packet->dep_counter = timed_mfu_block[13];
        uint8_t* rewind_buf = buf;

        //parse out mmthsample block if this is our first fragment or we are a complete fragment,
        if(mmtp_mpu_packet->mpu_fragment_type == 2 &&
            (mmtp_mpu_packet->mpu_fragmentation_indicator == 0 || mmtp_mpu_packet->mpu_fragmentation_indicator  == 1)) {

            //MMTHSample does not subclass box...
            //buf = (uint8_t*)extract(buf, &mmthsample_len, 1);
            mmtp_mpu_packet->mmthsample_header = calloc(1, sizeof(mmthsample_header_t));
            
            uint8_t mmthsample_len;
            uint8_t mmthsample_sequence_number[4];
            
            buf = (uint8_t*)extract(buf, mmthsample_sequence_number, 4);
            mmtp_mpu_packet->mmthsample_header->sequence_number = ntohl(*(uint32_t*)(mmthsample_sequence_number));

            uint8_t mmthsample_timed_block[19];
            buf = (uint8_t*)extract(buf, mmthsample_timed_block, 19);
            int mmth_position=0;
            
            mmtp_mpu_packet->mmthsample_header->trackrefindex = mmthsample_timed_block[mmth_position++];
            mmtp_mpu_packet->mmthsample_header->movie_fragment_sequence_number = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));
            mmth_position+=4;
            mmtp_mpu_packet->mmthsample_header->samplenumber = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));
            mmth_position+=4;
            mmtp_mpu_packet->mmthsample_header->priority = mmthsample_timed_block[mmth_position++];
            mmtp_mpu_packet->mmthsample_header->dependency_counter = mmthsample_timed_block[mmth_position++];
            //offset is from base of the containing mdat box (e.g. samplenumber 1 should have an offset of 8
            mmtp_mpu_packet->mmthsample_header->offset = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));
            mmth_position+=4;
            mmtp_mpu_packet->mmthsample_header->length = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));

                //hi skt!
            if(mmthsample_sequence_number[0] == 'S' && mmthsample_sequence_number[1] == 'K' && mmthsample_sequence_number[2] == 'T') {
                mmtp_mpu_packet->mmthsample_header->sequence_number  = mmtp_mpu_packet->mpu_sequence_number;
                mmtp_mpu_packet->mmthsample_header->samplenumber = mmtp_mpu_packet->sample_number;
                mmtp_mpu_packet->mmthsample_header->movie_fragment_sequence_number = mmtp_mpu_packet->movie_fragment_sequence_number;
                mmtp_mpu_packet->mmthsample_header->offset = mmtp_mpu_packet->offset + 8;
            }

            //read multilayerinfo
            uint8_t multilayerinfo_box_length_block[4];
            uint32_t multilayerinfo_box_length = 0;
            uint32_t multilayerinfo_box_name;
            uint8_t multilayer_flag;

            uint32_t box_parsed_position = 0;

            buf = (uint8_t*)extract(buf, multilayerinfo_box_length_block, 4);
            multilayerinfo_box_length = ntohl(*(uint32_t*)(&multilayerinfo_box_length_block));
            box_parsed_position+=4;

            buf = (uint8_t*)extract(buf, (uint8_t*)&multilayerinfo_box_name, 4);
            multilayerinfo_box_name = ntohl(*(uint32_t*)(&multilayerinfo_box_name));

            box_parsed_position+=4;

            //make sure multilayerinfo_box_name == muli
            assert(multilayerinfo_box_name == _BOX_MFU_MULI);

            buf = (uint8_t*)extract(buf, &multilayer_flag, 1);
            box_parsed_position++;

            int is_multilayer = (multilayer_flag >> 7) & 0x01;
            //if MSB is 1, then read multilevel struct, otherwise just pull layer info...
            if(is_multilayer) {
                uint8_t multilayer_data_block[4];
                buf = (uint8_t*)extract(buf, multilayer_data_block, 4);
                box_parsed_position+=4;

            } else {
                uint8_t multilayer_layer_id_temporal_id[2];
                buf = (uint8_t*)extract(buf, multilayer_layer_id_temporal_id, 2);
                box_parsed_position+=2;
            }

            //we need at least 8 bytes for a proper isobmff box child
            while(box_parsed_position < multilayerinfo_box_length - 8) {

                //try and parse out known 'private' isobmff boxes prepended to this sample
                uint8_t private_box_length_block[4];
                uint32_t private_box_length;

                uint32_t private_box_name;

                buf = (uint8_t*)extract(buf, private_box_length_block, 4);
                box_parsed_position+=4;

                private_box_length = ntohl(*(uint32_t*)(&private_box_length_block));
                buf = (uint8_t*)extract(buf, (uint8_t*)&private_box_name, 4);
                private_box_name = ntohl(*(uint32_t*)(&private_box_name));

                box_parsed_position+=4;

                __MMTP_MPU_SAMPLE_FORMAT_TRACE("mpu mode (0x02), packet_id: %u, packet_seq_num: %u, timed mfu has child box size: %u, name: %c%c%c%c",
                	mmtp_mpu_packet->mmtp_packet_id,
                    mmtp_mpu_packet->packet_sequence_number,
                    private_box_length,
                    ((private_box_name >> 24) & 0xFF), ((private_box_name >> 16) & 0xFF), ((private_box_name >> 8) & 0xFF), (private_box_name & 0xFF));

                if(private_box_name == _BOX_MFU_MJSD) {
                    //parse out timing information and child boxes
                    uint8_t sample_presentation_time_block[8];
                    uint64_t sample_presentation_time;

                    uint8_t sample_decode_time_block[8];
                    uint64_t sample_decode_time;

                    buf = (uint8_t*)extract(buf, sample_presentation_time_block, 8);
                    sample_presentation_time = ntohll(*(uint64_t*)(&sample_presentation_time_block));

                    box_parsed_position+=8;

                    buf = (uint8_t*)extract(buf, sample_decode_time_block, 8);
                    sample_decode_time = ntohll(*(uint64_t*)(&sample_decode_time_block));

                    box_parsed_position+=8;

                    __MMTP_MPU_SAMPLE_FORMAT_TRACE("mpu mode (0x02), MJSD, remaining child box size is: %u",  (multilayerinfo_box_length - box_parsed_position));

                    if((multilayerinfo_box_length - box_parsed_position) > 8) {

                        buf = (uint8_t*)extract(buf, private_box_length_block, 4);
                        box_parsed_position+=4;

                        private_box_length = ntohl(*(uint32_t*)(&private_box_length_block));
                        buf = (uint8_t*)extract(buf, (uint8_t*)&private_box_name, 4);
                        private_box_name = ntohl(*(uint32_t*)(&private_box_name));

                        box_parsed_position+=4;
                        __MMTP_MPU_SAMPLE_FORMAT_INFO("mpu mode (0x02), packet_id: %u, packet_seq_num: %u, MJSD  child box size: %u, name: %c%c%c%c",
                        		mmtp_mpu_packet->mmtp_packet_id,
                                mmtp_mpu_packet->packet_sequence_number,
                                private_box_length,
                                ((private_box_name >> 24) & 0xFF), ((private_box_name >> 16) & 0xFF), ((private_box_name >> 8) & 0xFF), (private_box_name & 0xFF));


                    }
                }
            }

            __MMTP_MPU_SAMPLE_FORMAT_TRACE("mpu mode (0x02), timed mfu has remaining payload: %u", (multilayerinfo_box_length - box_parsed_position));

            //for any remaining muli box size, ignore as possibly corrupt
            for(int i = box_parsed_position; i < multilayerinfo_box_length; i++) {
                uint8_t muli_box_incomplete_byte;
                buf = (uint8_t*)extract(buf, &muli_box_incomplete_byte, 1);
            }

            mmtp_mpu_packet->mmthsample_header->mfu_mmth_sample_header_size = 4 + 19 + multilayerinfo_box_length;
            	__MMTP_MPU_SAMPLE_FORMAT_DEBUG("mpu mode (0x02), timed MFU, mfu_mmth_sample_header_size: %u, mpu_fragmentation_indicator: %d, movie_fragment_seq_num: %u, sample_num: %u, offset: %u, pri: %d, dep_counter: %d, multilayer: %d, mpu_sequence_number: %u",
                mmtp_mpu_packet->mmthsample_header->mfu_mmth_sample_header_size,
                mmtp_mpu_packet->mpu_fragmentation_indicator,
                mmtp_mpu_packet->movie_fragment_sequence_number,
                mmtp_mpu_packet->sample_number,
                mmtp_mpu_packet->offset,
                mmtp_mpu_packet->priority,
                mmtp_mpu_packet->dep_counter,
                is_multilayer,
                mmtp_mpu_packet->mpu_sequence_number);
        } else {
            //jdj-2019-06-13 -- HACK -- mpu offset is incorrectly set at 34 bytes for fixed header, but we need to honor muli box size here
            //mpu_data_unit_payload_fragments_timed.mpu_offset -= 24;
            //jdj-2019-06-13 -- end HACK --

        	__MMTP_MPU_SAMPLE_FORMAT_DEBUG("mpu mode (0x02), timed MFU, mpu_fragmentation_indicator: %d, movie_fragment_seq_num: %u, sample_num: %u, offset: %u, pri: %d, dep_counter: %d, mpu_sequence_number: %u",
                mmtp_mpu_packet->mpu_fragmentation_indicator,
                mmtp_mpu_packet->movie_fragment_sequence_number,
                mmtp_mpu_packet->sample_number,
                mmtp_mpu_packet->offset,
                mmtp_mpu_packet->priority,
                mmtp_mpu_packet->dep_counter,
                mmtp_mpu_packet->mpu_sequence_number);
        }
        //end mfu box read

    } else {
//        uint8_t non_timed_mfu_block[4];
//        uint32_t non_timed_mfu_item_id;
//        //only 32 bits
//        buf = (uint8_t*)extract(buf, non_timed_mfu_block, 4);
//       // mmtp_mpu_packet->non_timed_mfu_item_id = (non_timed_mfu_block[0] << 24) | (non_timed_mfu_block[1] << 16) | (non_timed_mfu_block[2] << 8) | non_timed_mfu_block[3];
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
    }
    
    block_Seek(raw_packet, (buf - udp_raw_buf));
}
