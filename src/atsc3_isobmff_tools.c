/*
 * atsc3_isobmff_tools.c
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 */

#include "atsc3_isobmff_tools.h"
int _ISOBMFF_TOOLS_DEBUG_ENABLED = 0;
/**
 * query the corresponding packet id's to resolve our mpu_metadata and
 * rebuild our combined boxes and tracks
 */

block_t* atsc3_isobmff_build_mpu_metadata_ftyp_box(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector) {

	block_t* video_isobmff_header = NULL;
	block_t* audio_isobmff_header = NULL;

	block_t* mpu_metadata_output_block_t = NULL;

	mmtp_sub_flow_t* mmtp_sub_flow = NULL;
	mpu_data_unit_payload_fragments_t* mpu_metadata_fragments = NULL;  //* -- contains timed/nontimed but not the full flowdrilldown..
	mmtp_payload_fragments_union_t* mpu_metadata_fragment = NULL;

	udp_flow_latest_mpu_sequence_number_container_t* udp_flow_matching_flows = udp_flow_find_matching_flows(udp_flow_latest_mpu_sequence_number_container, udp_flow);

	if(!udp_flow_latest_mpu_sequence_number_container || !udp_flow_latest_mpu_sequence_number_container->udp_flows_n) {
		__ISOBMFF_TOOLS_ERROR("atsc3_isobmff_build_mpu_metadata_ftyp_box: Unable to find flows for MPU metadata creation from %u:%u", udp_flow->dst_ip_addr, udp_flow->dst_port);
	}

	for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
		udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];

		__ISOBMFF_TOOLS_DEBUG("atsc3_isobmff_build_mpu_metadata_ftyp_box: Searching for MPU Metadata with %u:%u and packet_id: %u", udp_flow->dst_ip_addr, udp_flow->dst_port, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

 		mmtp_sub_flow = mmtp_sub_flow_vector_get_or_set_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

		if(mmtp_sub_flow) {
			mpu_metadata_fragments = mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data ? mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data[0] : NULL;

			if(mpu_metadata_fragments) {
				int fragment_walk_count = 0;

				//&& mpu_metadata_fragments->timed_fragments_vector.
				while(fragment_walk_count < mpu_metadata_fragments->timed_fragments_vector.size) {
					mpu_metadata_fragment = mpu_metadata_fragments->timed_fragments_vector.data[fragment_walk_count++];
					if(mpu_metadata_fragment && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer ) {
						break;
					}
					mpu_metadata_fragment = NULL;
				}
				if(!mpu_metadata_fragment) {
					__ISOBMFF_TOOLS_WARN("Unable to find mpu_metadata from mmt_flows");
					return NULL;
				}

				__ISOBMFF_TOOLS_DEBUG("Found an mpu_metadata packet_id: %d, mpu_sequence_number: %u", mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_sequence_number);

				//TODO - fix me to use the proper mbms packet id's for video and audio packet_id's
				if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == 35) {
					video_isobmff_header = mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload;
				} else if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == 36) {
					audio_isobmff_header = mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload;
				}
			}
		}
	}
    if(!video_isobmff_header || !audio_isobmff_header) {
        __ISOBMFF_TOOLS_WARN("Unable to find video and audio isobmff headers, v: %p, a: %p", video_isobmff_header, audio_isobmff_header);
        return NULL;
    }
    
    __ISOBMFF_TOOLS_DEBUG("MPU Metadata: Pushing to Bento4");
//	AP4_DataBuffer* cleaned_mpu_metadata = mpuToDumpISOBMFFBoxes(mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer, -1);
//swap out our p_buffer with bento isobmff processing
//	AP4_DataBuffer* cleaned_mpu_metadata = mpuToISOBMFFProcessBoxes(mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer, -1);

//	block_Release(&mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
//	mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload = block_Alloc(cleaned_mpu_metadata->GetDataSize());
//	emcpy(mpu_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, cleaned_mpu_metadata->GetData(), cleaned_mpu_metadata->GetDataSize());
//	ISOBMFF_TOOLS_DEBUG("MPU Metadata: Got back %d bytes from mpuToISOBMFFProcessBoxes", cleaned_mpu_metadata->GetDataSize());
//
//	delete cleaned_mpu_metadata;


	return mpu_metadata_output_block_t;
}
