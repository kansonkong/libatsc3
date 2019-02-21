/*
 * atsc3_isobmff_tools.c
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 */

#include "atsc3_isobmff_tools.h"
#include "atsc3_player_ffplay.h"
#include <stdlib.h>
#include "bento4/ISOBMFFTrackJoiner.h"

int _ISOBMFF_TOOLS_DEBUG_ENABLED = 0;
/**
 * query the corresponding packet id's to resolve our mpu_metadata and
 * rebuild our combined boxes and tracks
 */

//temporary buffers here between isobmff muxer

#define __MAX_RECON_BUFFER 4096000
//super hack
bool has_sent_init_box = false;

uint8_t* __VIDEO_RECON_FRAGMENT = NULL;
uint32_t __VIDEO_RECON_FRAGMENT_SIZE = 0;

uint8_t* __AUDIO_RECON_FRAGMENT = NULL;
uint32_t __AUDIO_RECON_FRAGMENT_SIZE = 0;

void __copy_video_block_t(block_t* video_isobmff_header) {

    memcpy(&__VIDEO_RECON_FRAGMENT[__VIDEO_RECON_FRAGMENT_SIZE], video_isobmff_header->p_buffer, video_isobmff_header->i_buffer);
    __VIDEO_RECON_FRAGMENT_SIZE += video_isobmff_header->i_buffer;
}


void __copy_audio_block_t(block_t* audio_isobmff_header) {

    memcpy(&__AUDIO_RECON_FRAGMENT[__AUDIO_RECON_FRAGMENT_SIZE], audio_isobmff_header->p_buffer, audio_isobmff_header->i_buffer);
    __AUDIO_RECON_FRAGMENT_SIZE += audio_isobmff_header->i_buffer;
}


ISOBMFFTrackJoinerFileResouces_t* loadFileResources() {

	ISOBMFFTrackJoinerFileResouces_t* isoBMFFTrackJoinerResources = (ISOBMFFTrackJoinerFileResouces_t*)calloc(1, sizeof(ISOBMFFTrackJoinerFileResouces_t));

	const char* file1 = "35";

	isoBMFFTrackJoinerResources->file1_name = (char*)calloc(strlen(file1)+1, sizeof(char));
	strncpy(isoBMFFTrackJoinerResources->file1_name, file1, strlen(file1));
	printf("**** first 8 bytes are %x %x %x %x %x %x %x %x",
			__VIDEO_RECON_FRAGMENT[0],
			__VIDEO_RECON_FRAGMENT[1],
			__VIDEO_RECON_FRAGMENT[2],
			__VIDEO_RECON_FRAGMENT[3],
			__VIDEO_RECON_FRAGMENT[4],
			__VIDEO_RECON_FRAGMENT[5],
			__VIDEO_RECON_FRAGMENT[6],
			__VIDEO_RECON_FRAGMENT[7]);

	isoBMFFTrackJoinerResources->file1_payload = __VIDEO_RECON_FRAGMENT;
	isoBMFFTrackJoinerResources->file1_size = __VIDEO_RECON_FRAGMENT_SIZE;


	const char* file2 = "36";
	isoBMFFTrackJoinerResources->file2_name = (char*)calloc(strlen(file2)+1, sizeof(char));
	strncpy(isoBMFFTrackJoinerResources->file2_name, file2, strlen(file2));

	isoBMFFTrackJoinerResources->file2_payload = __AUDIO_RECON_FRAGMENT;
	isoBMFFTrackJoinerResources->file2_size = __AUDIO_RECON_FRAGMENT_SIZE;;

	return isoBMFFTrackJoinerResources;
}

block_t* atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector) {

	__VIDEO_RECON_FRAGMENT_SIZE = 0;
	__AUDIO_RECON_FRAGMENT_SIZE = 0;
	mpu_data_unit_payload_fragments_t* data_unit_payload_types = NULL;
	mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments = NULL; //technically this is mpu_fragments->media_fragment_unit_vector
	mpu_data_unit_payload_fragments_t* mpu_metadata_fragments =	NULL;
	mpu_data_unit_payload_fragments_t* movie_metadata_fragments  = NULL;
	mmtp_sub_flow_t* mmtp_sub_flow = NULL;
	int total_fragments = 0;


	block_t* video_isobmff_header = NULL;
	block_t* audio_isobmff_header = NULL;

	block_t* mpu_metadata_output_block_t = NULL;

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


    /** build 2 separate reconsituted flows here based upon
     *
     * 	video_isobmff_header
     *  audio_isobmff_header
     */

    if(!__VIDEO_RECON_FRAGMENT) {
    	__VIDEO_RECON_FRAGMENT = (uint8_t*) calloc(__MAX_RECON_BUFFER, sizeof(uint8_t*));
    }
    if(!__AUDIO_RECON_FRAGMENT) {
    	__AUDIO_RECON_FRAGMENT = (uint8_t*)calloc(__MAX_RECON_BUFFER, sizeof(uint8_t*));
    }

    if(!has_sent_init_box) {
		__copy_video_block_t(video_isobmff_header);
		__copy_audio_block_t(audio_isobmff_header);

    }
	//just do this statically here for now, packet_id=35 is video, packet_id=36 is audio
	for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
		udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];


		mmtp_sub_flow = mmtp_sub_flow_vector_get_or_set_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

		movie_metadata_fragments = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->movie_fragment_metadata_vector, udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number);

		mmtp_payload_fragments_union_t* fragment_metadata = NULL;
		if(movie_metadata_fragments && movie_metadata_fragments->timed_fragments_vector.size) {
			fragment_metadata = movie_metadata_fragments->timed_fragments_vector.data[0];
			__ISOBMFF_TOOLS_INFO("Movie Fragment Metadata: Found for fragment_metadata packet_id: %d, mpu_sequence_number: %u", fragment_metadata->mmtp_packet_header.mmtp_packet_id, udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number);

			if(!has_sent_init_box) {
				if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == 35) {

					__copy_video_block_t(fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
				} else {
					__copy_audio_block_t(fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
				}
			}
		} else {
			__ISOBMFF_TOOLS_WARN("Movie Fragment Metadata is NULL!");
			//goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
			return NULL;
		}

		data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->media_fragment_unit_vector, udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number);

		mmtp_payload_fragments_union_t* mpu_data_unit_payload_fragments_timed = NULL;

		if(data_unit_payload_types && data_unit_payload_types->timed_fragments_vector.size) {
			total_fragments = data_unit_payload_types->timed_fragments_vector.size;
			//mpu_data_unit_payload_fragments_timed = data_unit_payload_types->timed_fragments_vector;
			//push to mpu_push_output_buffer
			for(int i=0; i < total_fragments; i++) {
				//mmtp_payload_fragments_union_t* packet = data_unit->data[i];
				mmtp_payload_fragments_union_t* data_unit = data_unit_payload_types->timed_fragments_vector.data[0];

				if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) {
					if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == 35) {
						__copy_video_block_t(data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
					} else {
						__copy_audio_block_t(data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);

					}
				}
			}

		} else {
			__ISOBMFF_TOOLS_WARN("data unit size is null");
			//goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
			return NULL;
		}
	}




	__ISOBMFF_TOOLS_WARN("video size is: %d, audio size is: %d", __VIDEO_RECON_FRAGMENT_SIZE, __AUDIO_RECON_FRAGMENT_SIZE);

	if(!__VIDEO_RECON_FRAGMENT_SIZE || !__AUDIO_RECON_FRAGMENT_SIZE) {

		return NULL;
	}


	AP4_DataBuffer* dataBuffer = new AP4_DataBuffer(1024000);
	AP4_MemoryByteStream* memoryOutputByteStream = new AP4_MemoryByteStream(dataBuffer);

	ISOBMFFTrackJoinerFileResouces_t* fileResources = loadFileResources();

	parsrseAndBuildJoinedBoxes(fileResources, memoryOutputByteStream);


	__ISOBMFF_TOOLS_WARN("building reutrn alloc of %u", dataBuffer->GetDataSize());
	mpu_metadata_output_block_t = block_Alloc(dataBuffer->GetDataSize());
	memcpy(mpu_metadata_output_block_t->p_buffer, dataBuffer->GetData(), dataBuffer->GetDataSize());

	//trying to free this causes segfaults 100% of the time

	free (dataBuffer);
	has_sent_init_box = true;

	return mpu_metadata_output_block_t;
}


