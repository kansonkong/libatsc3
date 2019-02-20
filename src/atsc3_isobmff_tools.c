/*
 * atsc3_isobmff_tools.c
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 */


/**
 * query the corresponding packet id's to resolve our mpu_metadata and
 * rebuild our combined boxes and tracks
 */

block_t* atsc3_isobmff_build_mpu_metadata_ftyp_box(udp_packet_t* udp_packet, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector) {

	block_t* video_isobmff_header = NULL;
	block_t* audio_isobmff_header = NULL;

	block_t* mpu_metadata_output_block_t = NULL;

	mmtp_sub_flow_t* mmtp_sub_flow = NULL;
	mmtp_payload_fragments_union_t* mpu_metadata = NULL;
	mmtp_payload_fragments_union_t* mpu_metadata_fragments = NULL;  //mpu_data_unit_payload_fragments_t* -- contains timed/nontimed but not the full flowdrilldown..

	udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container = udp_flow_find_matching_flows(udp_flow_latest_mpu_sequence_number_container, udp_packet);

	if(!udp_flow_latest_mpu_sequence_number_container || !udp_flow_latest_mpu_sequence_number_container->udp_flows_n) {
		__ISOBMFF_TOOLS_ERROR("atsc3_isobmff_build_mpu_metadata_ftyp_box: Unable to find flows for MPU metadata creation from %u:%u", udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port);
	}

	for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
		udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];

		__ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_mpu_metadata_ftyp_box: Searching for MPU Metadata with %u:%u and packet_id: %u", udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

		mmtp_sub_flow = mmtp_sub_flow_vector_get_or_set_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

		if(mmtp_sub_flow) {
			mpu_metadata_fragments = mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data ? mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data[0] : NULL;

			if(mpu_metadata_fragments) {
				int fragment_walk_count = 0;

				//&& mpu_metadata_fragments->timed_fragments_vector.
				while(fragment_walk_count < mpu_metadata_fragments->timed_fragments_vector.size && !mpu_metadata_fragments->mmtp_mpu_type_packet_header.mpu_data_unit_payload ) {
					mpu_metadata_fragments = mpu_metadata_fragments->timed_fragments_vector.data[fragment_walk_count++];
				}

				mpu_metadata = mpu_metadata_fragments->timed_fragments_vector.data[0];

				if(fragment_walk_count == mpu_metadata_fragments->timed_fragments_vector.size || !mpu_metadata == mpu_metadata_fragments->timed_fragments_vector.data[0])
					__ISOBMFF_TOOLS_WARNING("Unable to find mpu_metadata from mmt_flows");
					return NULL;
				} else {

					__ISOBMFF_TOOLS_DEBUG("Found at mpu_metadata packet_id: %d, mpu_sequence_number: %u", udp_flow_last_packet_id_mpu_sequence_id->packet_id, mpu_metadata->mmtp_mpu_type_packet_header.mpu_sequence_number);
					__INFO("MPU Metadata: Found at mpu_metadata packet_id: %d, mpu_sequence_number: %u", udp_flow_last_packet_id_mpu_sequence_id->packet_id, mpu_metadata->mmtp_mpu_type_packet_header.mpu_sequence_number);

					//TODO - fix me to use the proper mbms packet id's for video and audio packet_id's
					if(mpu_metadata->mmtp_mpu_type_packet_header.mmtp_packet_id == 35) {
						video_isobmff_header = mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload;
					} else if(mpu_metadata->mmtp_mpu_type_packet_header.mmtp_packet_id == 35) {
						audio_isobmff_header = mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload;

					}
				}
			}
		}
	}

	AP4_DataBuffer* cleaned_mpu_metadata = mpuToDumpISOBMFFBoxes(mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer, -1);

	__MMT_MPU_INFO("MPU Metadata: Pushing to Bento4")

	 /**

	 //swap out our p_buffer with bento isobmff processing
	 AP4_DataBuffer* cleaned_mpu_metadata = mpuToISOBMFFProcessBoxes(mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer, -1);
	 **/
	block_Release(&mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
	mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload = block_Alloc(cleaned_mpu_metadata->GetDataSize());
	memcpy(mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, cleaned_mpu_metadata->GetData(), cleaned_mpu_metadata->GetDataSize());
	__MMT_MPU_INFO("MPU Metadata: Got back %d bytes from mpuToISOBMFFProcessBoxes", cleaned_mpu_metadata->GetDataSize());

	delete cleaned_mpu_metadata;


	return mpu_metadata_block_t;
}
