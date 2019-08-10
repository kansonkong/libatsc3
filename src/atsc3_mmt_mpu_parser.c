/*
 * atsc3_mmt_mpu_parser.c
 *
 *  Created on: Jan 26, 2019
 *      Author: jjustman
 *
 *
 *      do the heavy lifting...
 */


#include "atsc3_mmtp_parser.h"
#include "atsc3_mmt_mpu_parser.h"
#include "atsc3_mmtp_packet_types.h"

int _MPU_DEBUG_ENABLED = 0;
int _MPU_TRACE_ENABLED = 0;


uint8_t* mmt_mpu_parse_payload(mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, mmtp_payload_fragments_union_t* mmtp_packet_header, udp_flow_t* udp_flow, uint8_t* udp_raw_buf, int udp_raw_buf_size) {

	mmtp_sub_flow_t *mmtp_sub_flow = NULL;
	block_t *mmtp_raw_packet_block;

	//resync our buf positions
	uint8_t *raw_buf = udp_raw_buf;
	uint8_t *buf = udp_raw_buf;

	//create a sub_flow with this packet_id
	_MPU_DEBUG( "mmtp_demuxer, after mmtp_packet_header_parse_from_raw_packet, mmtp_packet_id is: %d, mmtp_payload_type: 0x%x, packet_counter: %d, remaining len: %lu, mmtp_raw_packet_size: %d, buf: %p, raw_buf:%p",
			mmtp_packet_header->mmtp_packet_header->mmtp_packet_id,
			mmtp_packet_header->mmtp_packet_header->mmtp_payload_type,
			mmtp_packet_header->mmtp_packet_header->packet_counter,
			(udp_raw_buf_size - (buf - raw_buf)),
			udp_raw_buf_size,
			buf,
			raw_buf);

	mmtp_sub_flow = mmtp_sub_flow_vector_get_or_set_packet_id(mmtp_sub_flow_vector, udp_flow, mmtp_packet_header->mmtp_packet_header->mmtp_packet_id);
	_MPU_DEBUG("mmtp_demuxer - mmtp_sub_flow is: %p, mmtp_sub_flow->mpu_fragments: %p", mmtp_sub_flow, mmtp_sub_flow->mpu_fragments);

	//push this to
	//if our header extension length is set, then block (uint8_t*)extract the header extension length, adn we should be at our payload data
	uint8_t *mmtp_header_extension_value = NULL;

	if(mmtp_packet_header->mmtp_packet_header->mmtp_header_extension_flag & 0x1) {
		//clamp mmtp_header_extension_length
		mmtp_packet_header->mmtp_packet_header->mmtp_header_extension_length = MIN(mmtp_packet_header->mmtp_packet_header->mmtp_header_extension_length, 2^16);

		_MPU_DEBUG( "mmtp_header_extension_flag, header extension size: %d, packet version: %d, payload_type: 0x%X, packet_id 0x%hu, timestamp: 0x%X, packet_sequence_number: 0x%X, packet_counter: 0x%X",
				mmtp_packet_header->mmtp_packet_header->mmtp_packet_version,
				mmtp_packet_header->mmtp_packet_header->mmtp_header_extension_length,
				mmtp_packet_header->mmtp_packet_header->mmtp_payload_type,
				mmtp_packet_header->mmtp_packet_header->mmtp_packet_id,
				mmtp_packet_header->mmtp_packet_header->mmtp_timestamp,
				mmtp_packet_header->mmtp_packet_header->packet_sequence_number,
				mmtp_packet_header->mmtp_packet_header->packet_counter);

		mmtp_packet_header->mmtp_packet_header->mmtp_header_extension_value = (uint8_t*)malloc(mmtp_packet_header->mmtp_packet_header->mmtp_header_extension_length);
		//read the header extension value up to the extension length field 2^16
		buf = (uint8_t*)extract(buf, (uint8_t*)&mmtp_packet_header->mmtp_packet_header->mmtp_header_extension_value, mmtp_packet_header->mmtp_packet_header->mmtp_header_extension_length);
	}

	if(mmtp_packet_header->mmtp_packet_header->mmtp_payload_type == 0x0) {
		//VECTOR:  TODO - refactor this into helper method

		//pull the mpu and frag iformation

		uint8_t mpu_payload_length_block[2];
		uint16_t mpu_payload_length = 0;

		//msg_Warn( p_demux, "buf pos before mpu_payload_length (uint8_t*)extract is: %p", (void *)buf);
		buf = (uint8_t*)extract(buf, (uint8_t*)&mpu_payload_length_block, 2);
		mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_payload_length = (mpu_payload_length_block[0] << 8) | mpu_payload_length_block[1];
		//_MPU_DEBUG( p_demux, "mmtp_demuxer - doing mpu_payload_length: %hu (0x%X 0x%X)",  mpu_payload_length, mpu_payload_length_block[0], mpu_payload_length_block[1]);

		uint8_t mpu_fragmentation_info;
		//msg_Warn( p_demux, "buf pos before (uint8_t*)extract is: %p", (void *)buf);
		buf = (uint8_t*)extract(buf, &mpu_fragmentation_info, 1);

		mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_fragment_type = (mpu_fragmentation_info & 0xF0) >> 4;
		mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_timed_flag = (mpu_fragmentation_info & 0x8) >> 3;
		mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator = (mpu_fragmentation_info & 0x6) >> 1;
		mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_aggregation_flag = (mpu_fragmentation_info & 0x1);


		_MPU_DEBUG("Mpu fragment type: %d :mmtp_demuxer - mmtp packet: mpu_fragmentation_info is: 0x%x, mpu_fragment_type: 0x%x, mpu_timed_flag: 0x%x, mpu_fragmentation_indicator: 0x%x, mpu_aggregation_flag: 0x%x",
			mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_fragment_type,
			mpu_fragmentation_info,
			mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_fragment_type,
			mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_timed_flag,
			mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator,
			mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_aggregation_flag);



		uint8_t mpu_fragmentation_counter;
		//msg_Warn( p_demux, "buf pos before (uint8_t*)extract is: %p", (void *)buf);
		buf = (uint8_t*)extract(buf, &mpu_fragmentation_counter, 1);
		mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_fragmentation_counter = mpu_fragmentation_counter;

		//re-fanagle
		uint8_t mpu_sequence_number_block[4];

		buf = (uint8_t*)extract(buf, (uint8_t*)&mpu_sequence_number_block, 4);
		mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_sequence_number = (mpu_sequence_number_block[0] << 24)  | (mpu_sequence_number_block[1] <<16) | (mpu_sequence_number_block[2] << 8) | (mpu_sequence_number_block[3]);
		_MPU_DEBUG("mmtp_demuxer - mmtp packet: mpu_payload_length: %hu (0x%X 0x%X), mpu_fragmentation_counter: %d, mpu_sequence_number: %d",
				mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_payload_length,
				mpu_payload_length_block[0],
				mpu_payload_length_block[1],
				mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_fragmentation_counter,
				mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_sequence_number);


		mpu_fragments_assign_to_payload_vector(mmtp_sub_flow, mmtp_packet_header);

		//VECTOR: assign data unit payload once parsed, eventually replacing processMpuPacket

		int remainingPacketLen = -1;

		//todo - if FEC_type != 0, parse out source_FEC_payload_ID trailing bits...
		do {
			//pull out aggregate packets data unit length
			int to_read_packet_length = -1;
			//mpu_fragment_type

			//only read DU length if mpu_aggregation_flag=1
			if(mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_aggregation_flag) {
				uint8_t data_unit_length_block[2];
				buf = (uint8_t*)extract(buf, (uint8_t*)&data_unit_length_block, 2);
				mmtp_packet_header->mmtp_mpu_type_packet_header.data_unit_length = (data_unit_length_block[0] << 8) | (data_unit_length_block[1]);
				to_read_packet_length = mmtp_packet_header->mmtp_mpu_type_packet_header.data_unit_length;
				_MPU_DEBUG( "mpu data unit size: %d, mpu_aggregation_flag:1, to_read_packet_length: %d",
						mmtp_packet_header->mmtp_mpu_type_packet_header.data_unit_length, to_read_packet_length);

			} else {
				to_read_packet_length = udp_raw_buf_size - (buf-raw_buf);
				_MPU_DEBUG("using data_unit_size from packet length: mpu_aggregation_flag:0, raw packet size: %d, buf: %p, raw_buf: %p, to_read_packet_length: %d",
						udp_raw_buf_size, buf, raw_buf, to_read_packet_length);
			}

			//if we are MPU metadata or movie fragment metadatas
			if(mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_fragment_type != 0x2) {
				//read our packet length just as a mpu metadata fragment or movie fragment metadata
				//read our packet length without any mfu
				block_t *tmp_mpu_fragment = block_Alloc(to_read_packet_length);
				_MPU_DEBUG("creating tmp_mpu_fragment, setting block_t->i_buffer to: %d", to_read_packet_length);

				buf = (uint8_t*)extract(buf, tmp_mpu_fragment->p_buffer, to_read_packet_length);
				tmp_mpu_fragment->i_pos = to_read_packet_length;

				mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_data_unit_payload = tmp_mpu_fragment;

				remainingPacketLen = udp_raw_buf_size - (buf - raw_buf);
				//this should only be non-zero if mpu_aggregration_flag=1
				//__LOG_INFO(p_demux, "%d::mpu_fragment_type: %hu, remainingPacketLen: %d", __LINE__, mpu_fragment_type, remainingPacketLen);

			} else {

				/**
				 *
				 * jdj-2019-03-30 - TODO: fix me to read proper box sizes.....
				 *
				 *
				 */

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

				uint8_t mmthsample_len;
				uint8_t mmthsample_sequence_number[4];

				if(mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_timed_flag) {

				//	uint16_t seconds;
				//	uint16_t microseconds;
					compute_ntp32_to_seconds_microseconds(mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmtp_timestamp, &mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmtp_timestamp_s, &mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmtp_timestamp_us);
					_MPU_DEBUG("converting mmtp_timestamp: %u to seconds: %hu, microseconds: %hu", mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmtp_timestamp, mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmtp_timestamp_s, mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmtp_timestamp_us);
					//on first init, p_sys->first_pts will always be 0 from calloc
//					uint64_t pts = compute_relative_ntp32_pts(p_sys->first_pts, mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmtp_timestamp_s, mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmtp_timestamp_us);
//					if(!p_sys->has_set_first_pts) {
//						p_sys->first_pts = pts;
//						p_sys->has_set_first_pts = 1;
//					}

					//build our PTS
					//mmtp_packet_header->mpu_data_unit_payload_fragments_timed.pts = pts;

					//112 bits in aggregate, 14 bytes
					uint8_t timed_mfu_block[14];
					buf = (uint8_t*)extract(buf, timed_mfu_block, 14);

					mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_movie_fragment_sequence_number 	= (timed_mfu_block[0] << 24) | (timed_mfu_block[1] << 16) | (timed_mfu_block[2]  << 8) | (timed_mfu_block[3]);
					mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_sample_number				 	  	= (timed_mfu_block[4] << 24) | (timed_mfu_block[5] << 16) | (timed_mfu_block[6]  << 8) | (timed_mfu_block[7]);
					mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_offset     					  	= (timed_mfu_block[8] << 24) | (timed_mfu_block[9] << 16) | (timed_mfu_block[10] << 8) | (timed_mfu_block[11]);
					mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_priority 							= timed_mfu_block[12];
					mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_dep_counter						= timed_mfu_block[13];
					uint8_t* rewind_buf = buf;

                    //see if bento4 will handle this?
					//parse out mmthsample block if this is our first fragment or we are a complete fragment,
					if(mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_fragment_type == 2 &&
                        (mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_indicator == 0 ||
									mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_indicator == 1)) {

						//MMTHSample does not subclass box...
						//buf = (uint8_t*)extract(buf, &mmthsample_len, 1);
						buf = (uint8_t*)extract(buf, mmthsample_sequence_number, 4);
						mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_sequence_number = ntohl(*(uint32_t*)(mmthsample_sequence_number));

						uint8_t mmthsample_timed_block[19];
						buf = (uint8_t*)extract(buf, mmthsample_timed_block, 19);
						int mmth_position=0;
						mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_trackrefindex = mmthsample_timed_block[mmth_position++];
						mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_movie_fragment_sequence_number = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));
						mmth_position+=4;
						mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_samplenumber = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));
						mmth_position+=4;
						mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_priority = mmthsample_timed_block[mmth_position++];
						mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_dependency_counter = mmthsample_timed_block[mmth_position++];
						//offset is from base of the containing mdat box (e.g. samplenumber 1 should have an offset of 8
                        mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_offset = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));
						mmth_position+=4;
						mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_length = ntohl(*(uint32_t*)(&mmthsample_timed_block[mmth_position]));

  						//hi skt!
                        if(mmthsample_sequence_number[0] == 'S' && mmthsample_sequence_number[1] == 'K' && mmthsample_sequence_number[2] == 'T') {
                            mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_sequence_number = mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_sequence_number;
                            mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_samplenumber = mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_sample_number;
                            mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_movie_fragment_sequence_number = mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_movie_fragment_sequence_number;
                            mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mmth_offset = mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_offset + 8;
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

                            _MPU_TRACE("mpu mode (0x02), packet_id: %u, packet_seq_num: %u, timed mfu has child box size: %u, name: %c%c%c%c", mmtp_packet_header->mmtp_mpu_type_packet_header.mmtp_packet_id,
                                mmtp_packet_header->mmtp_mpu_type_packet_header.packet_sequence_number,
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

		                        _MPU_TRACE("mpu mode (0x02), MJSD, remaining child box size is: %u",  (multilayerinfo_box_length - box_parsed_position));

					if((multilayerinfo_box_length - box_parsed_position) > 8) {

					  buf = (uint8_t*)extract(buf, private_box_length_block, 4);
					  box_parsed_position+=4;

					  private_box_length = ntohl(*(uint32_t*)(&private_box_length_block));
					  buf = (uint8_t*)extract(buf, (uint8_t*)&private_box_name, 4);
					  private_box_name = ntohl(*(uint32_t*)(&private_box_name));

					  box_parsed_position+=4;
_MPU_INFO("!!!mpu mode (0x02), packet_id: %u, packet_seq_num: %u, MJSD  child box size: %u, name: %c%c%c%c", mmtp_packet_header->mmtp_mpu_type_packet_header.mmtp_packet_id,
                                mmtp_packet_header->mmtp_mpu_type_packet_header.packet_sequence_number,
                                private_box_length,
                                ((private_box_name >> 24) & 0xFF), ((private_box_name >> 16) & 0xFF), ((private_box_name >> 8) & 0xFF), (private_box_name & 0xFF));
				

					}
	                        }
						}

                        _MPU_TRACE("mpu mode (0x02), timed mfu has remaining payload: %u", (multilayerinfo_box_length - box_parsed_position));

						//for any remaining muli box size, ignore as possibly corrupt
						for(int i = box_parsed_position; i < multilayerinfo_box_length; i++) {
							uint8_t muli_box_incomplete_byte;
							buf = (uint8_t*)extract(buf, &muli_box_incomplete_byte, 1);
						}

                        mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mfu_mmth_sample_header_size = 4 + 19 + multilayerinfo_box_length;
                            _MPU_DEBUG("mpu mode (0x02), timed MFU, mfu_mmth_sample_header_size: %u, mpu_fragmentation_indicator: %d, movie_fragment_seq_num: %u, sample_num: %u, offset: %u, pri: %d, dep_counter: %d, multilayer: %d, mpu_sequence_number: %u",
                            mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mfu_mmth_sample_header_size,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_indicator,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_movie_fragment_sequence_number,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_sample_number,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_offset,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_priority,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_dep_counter,
							is_multilayer,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_sequence_number);
					} else {
                        //jdj-2019-06-13 -- HACK -- mpu offset is incorrectly set at 34 bytes for fixed header, but we need to honor muli box size here
						//mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_offset -= 24;
						//jdj-2019-06-13 -- end HACK --

						_MPU_DEBUG("mpu mode (0x02), timed MFU, mpu_fragmentation_indicator: %d, movie_fragment_seq_num: %u, sample_num: %u, offset: %u, pri: %d, dep_counter: %d, mpu_sequence_number: %u",
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_fragmentation_indicator,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_movie_fragment_sequence_number,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_sample_number,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_offset,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_priority,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_dep_counter,
							mmtp_packet_header->mpu_data_unit_payload_fragments_timed.mpu_sequence_number);
					}
					//end mfu box read

					to_read_packet_length = udp_raw_buf_size - (buf - raw_buf);
				} else {
					uint8_t non_timed_mfu_block[4];
					uint32_t non_timed_mfu_item_id;
					//only 32 bits
					buf = (uint8_t*)extract(buf, non_timed_mfu_block, 4);
					mmtp_packet_header->mpu_data_unit_payload_fragments_nontimed.non_timed_mfu_item_id = (non_timed_mfu_block[0] << 24) | (non_timed_mfu_block[1] << 16) | (non_timed_mfu_block[2] << 8) | non_timed_mfu_block[3];

					if(mmtp_packet_header->mpu_data_unit_payload_fragments_nontimed.mpu_fragmentation_indicator == 1) {
						//MMTHSample does not subclass box...
						//buf = (uint8_t*)extract(buf, &mmthsample_len, 1);

						buf = (uint8_t*)(uint8_t*)extract(buf, mmthsample_sequence_number, 4);

						uint8_t mmthsample_item_id[2];
						buf = (uint8_t*)extract(buf, mmthsample_sequence_number, 2);
						//end reading of mmthsample box
					}

					_MPU_DEBUG("mpu mode (0x02), non-timed MFU, item_id is: %u", mmtp_packet_header->mpu_data_unit_payload_fragments_nontimed.non_timed_mfu_item_id);
					to_read_packet_length = udp_raw_buf_size - (buf - raw_buf);
				}

				__LOG_TRACE( p_demux, "%d:before reading fragment packet: reading length: %d (mmtp_raw_packet_size: %d, buf: %p, raw_buf:%p)",
						__LINE__,
						to_read_packet_length,
						mmtp_raw_packet_size,
						buf,
						raw_buf);

				block_t *tmp_mpu_fragment = block_Alloc(to_read_packet_length);
				_MPU_TRACE("creating tmp_mpu_fragment, setting block_t->i_buffer to: %d", to_read_packet_length);

				buf = (uint8_t*)extract(buf, tmp_mpu_fragment->p_buffer, to_read_packet_length);
				tmp_mpu_fragment->i_pos = to_read_packet_length;


				mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_data_unit_payload = tmp_mpu_fragment;

				//send off only the CLEAN mdat payload from our MFU
				remainingPacketLen = udp_raw_buf_size - (buf - raw_buf);
				_MPU_TRACE( "after reading fragment packet: remainingPacketLen: %d", remainingPacketLen);

			}

		} while(mmtp_packet_header->mmtp_mpu_type_packet_header.mpu_aggregation_flag && remainingPacketLen>0);
	}

	__LOG_TRACE(p_demux, "%d:demux - return", __LINE__);

	//in case we were a fragmented packet we could run this loop again?
	return buf;
}






mpu_data_unit_payload_fragments_t* mpu_data_unit_payload_fragments_find_mpu_sequence_number(mpu_data_unit_payload_fragments_vector_t *vec, uint32_t mpu_sequence_number) {
	for (size_t i = 0; i < vec->size; ++i) {
		mpu_data_unit_payload_fragments_t *mpu_fragments = vec->data[i];

		if (mpu_fragments->mpu_sequence_number == mpu_sequence_number) {
			return vec->data[i];
		}
	}
	return NULL;
}


mpu_data_unit_payload_fragments_t* mpu_data_unit_payload_fragments_get_or_set_mpu_sequence_number_from_packet(mpu_data_unit_payload_fragments_vector_t *vec, mmtp_payload_fragments_union_t *mpu_type_packet) {

	mpu_data_unit_payload_fragments_t *entry = mpu_data_unit_payload_fragments_find_mpu_sequence_number(vec, mpu_type_packet->mmtp_mpu_type_packet_header.mpu_sequence_number);
	if(!entry) {
		entry = (mpu_data_unit_payload_fragments_t*)calloc(1, sizeof(mpu_data_unit_payload_fragments_t));

		entry->mpu_sequence_number = mpu_type_packet->mmtp_mpu_type_packet_header.mpu_sequence_number;
		atsc3_vector_init(&entry->timed_fragments_vector);
		atsc3_vector_init(&entry->nontimed_fragments_vector);
		atsc3_vector_push(vec, entry);
	}

	return entry;
}


//push this to mpu_fragments_vector->all_fragments_vector first,
// 	then re-assign once fragment_type and fragmentation info are parsed
//mpu_sequence_number *SHOULD* only be resolved from the interior all_fragments_vector for tuple lookup
mpu_fragments_t* mpu_fragments_get_or_set_packet_id(mmtp_sub_flow_t* mmtp_sub_flow, uint16_t mmtp_packet_id) {

	mpu_fragments_t *entry = mmtp_sub_flow->mpu_fragments;
	if(!entry) {
		__PRINTF_DEBUG("*** %d:mpu_fragments_get_or_set_packet_id - adding vector: %p, all_fragments_vector is: %p\n",
				__LINE__, entry, &entry->all_mpu_fragments_vector);

		mmtp_sub_flow_mpu_fragments_allocate(mmtp_sub_flow);
		entry = mmtp_sub_flow->mpu_fragments;
		entry->mmtp_packet_id = mmtp_packet_id;
	}

	return entry;
}

//check for duplicate entries before pushing into the corresponding table (based upon packet_id,  packet_sequence_number, mpu_sequence_number and fragmentation counter)
void mpu_fragments_assign_to_payload_vector(mmtp_sub_flow_t* mmtp_sub_flow, mmtp_payload_fragments_union_t* mpu_type_packet) {
	//use mmtp_sub_flow ref, find packet_id, map into mpu/mfu vector
//	mmtp_sub_flow_t mmtp_sub_flow = mpu_type_packet->mpu_

	mpu_fragments_t *mpu_fragments = mmtp_sub_flow->mpu_fragments;

	mpu_data_unit_payload_fragments_t *to_assign_payload_vector = NULL;

	if(mpu_type_packet->mmtp_mpu_type_packet_header.mpu_fragment_type == 0x00) {
		//push to mpu_metadata fragments vector

		__PRINTF_TRACE("%d:mpu_fragments_assign_to_payload_vector - MPU fragment type == %x, packet_counter: %u, packet_id: %d, sequence_number: %d, fragment type: %d, mpu_fragments is: %p, all_mpu_frags_vector.size: %zu\n", __LINE__, mpu_type_packet->mmtp_mpu_type_packet_header.mpu_fragment_type, mpu_type_packet->mmtp_packet_header->packet_counter, mpu_type_packet->mmtp_mpu_type_packet_header.mmtp_packet_id,  mpu_type_packet->mmtp_mpu_type_packet_header.mpu_sequence_number, mpu_type_packet->mmtp_mpu_type_packet_header.mpu_fragment_type, mpu_fragments, mpu_fragments->all_mpu_fragments_vector.size);
		to_assign_payload_vector = mpu_data_unit_payload_fragments_get_or_set_mpu_sequence_number_from_packet(&mpu_fragments->mpu_metadata_fragments_vector, mpu_type_packet);

	} else if(mpu_type_packet->mmtp_mpu_type_packet_header.mpu_fragment_type == 0x01) {
		//push to mpu_movie_fragment
		to_assign_payload_vector = mpu_data_unit_payload_fragments_get_or_set_mpu_sequence_number_from_packet(&mpu_fragments->movie_fragment_metadata_vector, mpu_type_packet);
	} else if(mpu_type_packet->mmtp_mpu_type_packet_header.mpu_fragment_type == 0x02) {
		//push to media_fragment
		to_assign_payload_vector = mpu_data_unit_payload_fragments_get_or_set_mpu_sequence_number_from_packet(&mpu_fragments->media_fragment_unit_vector, mpu_type_packet);

		_MPU_DEBUG("%d:mpu_fragments_assign_to_payload_vector - data unit.mpu_sequence_number, %d, fragment type == %x, packet_counter: %u, packet_id: %d, sequence_number: %d, fragment type: %d, mpu_fragments is: %p, all_mpu_frags_vector.size: %zu\n",
				__LINE__,
				to_assign_payload_vector->mpu_sequence_number,
				mpu_type_packet->mmtp_mpu_type_packet_header.mpu_fragment_type,
				mpu_type_packet->mmtp_packet_header->packet_counter, mpu_type_packet->mmtp_mpu_type_packet_header.mmtp_packet_id,
				mpu_type_packet->mmtp_mpu_type_packet_header.mpu_sequence_number,
				mpu_type_packet->mmtp_mpu_type_packet_header.mpu_fragment_type, mpu_fragments, mpu_fragments->all_mpu_fragments_vector.size);

	}


	if(to_assign_payload_vector) {
		_MPU_DEBUG("%d:mpu_fragments_assign_to_payload_vector: %p - packet_counter: %u, packet_id: %d, sequence_number: %d, fragment type: %d, mpu_fragments is: %p, all_mpu_frags_vector.size: %zu\n", __LINE__, to_assign_payload_vector, mpu_type_packet->mmtp_packet_header->packet_counter, mpu_type_packet->mmtp_mpu_type_packet_header.mmtp_packet_id,  mpu_type_packet->mmtp_mpu_type_packet_header.mpu_sequence_number, mpu_type_packet->mmtp_mpu_type_packet_header.mpu_fragment_type, mpu_fragments, mpu_fragments->all_mpu_fragments_vector.size);

		//__PRINTF_TRACE("%d: to_assign_payload_vector, sequence_number: %d, size is: %zu\n", __LINE__, mpu_type_packet->mmtp_mpu_type_packet_header.mpu_sequence_number, to_assign_payload_vector->timed_fragments_vector.size);
		if(mpu_type_packet->mmtp_mpu_type_packet_header.mpu_timed_flag) {
			//__PRINTF_TRACE("%d:mpu_data_unit_payload_fragments_get_or_set_mpu_sequence_number_from_packet, sequence_number: %d, pushing to timed_fragments_vector: %p", __LINE__, to_assign_payload_vector->mpu_sequence_number, &to_assign_payload_vector->timed_fragments_vector);
			atsc3_vector_push(&to_assign_payload_vector->timed_fragments_vector, mpu_type_packet);
			_MPU_DEBUG("%d:mpu_fragments_assign_to_payload_vector: mpu_fragments_assign_to_payload_vector, vector size: %zu for MPU fragment type == %x, packet_counter: %u, packet_id: %d, sequence_number: %d, fragment type: %d, mpu_fragments is: %p, all_mpu_frags_vector.size: %zu\n", __LINE__,
					to_assign_payload_vector->timed_fragments_vector.size,
					mpu_type_packet->mmtp_mpu_type_packet_header.mpu_fragment_type,
					mpu_type_packet->mmtp_packet_header->packet_counter,
					mpu_type_packet->mmtp_mpu_type_packet_header.mmtp_packet_id,
					mpu_type_packet->mmtp_mpu_type_packet_header.mpu_sequence_number,
					mpu_type_packet->mmtp_mpu_type_packet_header.mpu_fragment_type,
					mpu_fragments,
					mpu_fragments->all_mpu_fragments_vector.size);

		} else {
			atsc3_vector_push(&to_assign_payload_vector->nontimed_fragments_vector, mpu_type_packet);
		}

	}
}
//deprecated 2019-05-06
//mpu_fragments_t* mpu_fragments_find_packet_id(mmtp_sub_flow_vector_t *vec, uint16_t mmtp_packet_id) {
//	mmtp_sub_flow_t *entry = mmtp_sub_flow_vector_find_packet_id(vec, mmtp_packet_id);
//	if(entry) {
//		return entry->mpu_fragments;
//	}
//
//	return NULL;
//}

//internal use only
void mmt_mpu_free_payload(mmtp_payload_fragments_union_t* mmtp_payload_fragments) {

	//free raw data block allocs
	if(mmtp_payload_fragments && mmtp_packet_header->mmtp_payload_type == 0x0) {
		if(mmtp_mpu_type_packet_header.raw_packet) {
			_MMTP_TRACE("mmt_mpu_free_payload: raw_packet: calling block_Release with: %p", &mmtp_mpu_type_packet_header.raw_packet);

			block_Release(&mmtp_mpu_type_packet_header.raw_packet);
			mmtp_mpu_type_packet_header.raw_packet = NULL;
		}

		if(mmtp_mpu_type_packet_header.mpu_data_unit_payload) {
			_MMTP_TRACE("mmtp_mpu_type_packet_header.mpu_data_unit_payload BEFORE : %p", mmtp_mpu_type_packet_header.mpu_data_unit_payload);
            
			_MMTP_TRACE("mmt_mpu_free_payload: mpu_data_unit_payload: calling block_Release with: %p", &mmtp_mpu_type_packet_header.mpu_data_unit_payload);

			block_Release(&mmtp_mpu_type_packet_header.mpu_data_unit_payload);
			mmtp_mpu_type_packet_header.mpu_data_unit_payload = NULL;
		}
	}

}
