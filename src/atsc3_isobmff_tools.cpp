/*
 * atsc3_isobmff_tools.c
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "atsc3_isobmff_tools.h"
#include "bento4/ISOBMFFTrackJoiner.h"

int _ISOBMFF_TOOLS_DEBUG_ENABLED = 0;
/**
 * query the corresponding packet id's to resolve our mpu_metadata and
 * rebuild our combined boxes and tracks
 */

//temporary buffers here between isobmff muxer

#define __MAX_RECON_BUFFER 4096000
//super hack

uint8_t* __VIDEO_RECON_FRAGMENT = NULL;
uint32_t __VIDEO_RECON_FRAGMENT_SIZE = 0;

uint8_t* __AUDIO_RECON_FRAGMENT = NULL;
uint32_t __AUDIO_RECON_FRAGMENT_SIZE = 0;


int __AUDIO_SEGMENT_RECON_COUNTER = 0;
int __VIDEO_SEGMENT_RECON_COUNTER = 0;
int __BOX_SEGMENT_RECON_COUNTER = 0;


void resetBufferPos() {
	__VIDEO_RECON_FRAGMENT_SIZE = 0;
	__AUDIO_RECON_FRAGMENT_SIZE = 0;
}
void __copy_video_block_t(block_t* video_isobmff_header) {

	if(!__VIDEO_RECON_FRAGMENT) {
		__VIDEO_RECON_FRAGMENT = (uint8_t*) calloc(__MAX_RECON_BUFFER, sizeof(uint8_t*));
	}

    memcpy(&__VIDEO_RECON_FRAGMENT[__VIDEO_RECON_FRAGMENT_SIZE], video_isobmff_header->p_buffer, video_isobmff_header->i_buffer);
    __VIDEO_RECON_FRAGMENT_SIZE += video_isobmff_header->i_buffer;
}


void __copy_audio_block_t(block_t* audio_isobmff_header) {

	if(!__AUDIO_RECON_FRAGMENT) {
		__AUDIO_RECON_FRAGMENT = (uint8_t*)calloc(__MAX_RECON_BUFFER, sizeof(uint8_t*));
	}
    memcpy(&__AUDIO_RECON_FRAGMENT[__AUDIO_RECON_FRAGMENT_SIZE], audio_isobmff_header->p_buffer, audio_isobmff_header->i_buffer);
    __AUDIO_RECON_FRAGMENT_SIZE += audio_isobmff_header->i_buffer;
}


ISOBMFFTrackJoinerFileResouces_t* _l_loadFileResources() {

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

    char* video_track_dump_filename = (char*)calloc(128, sizeof(char));
    snprintf(video_track_dump_filename, 127, "mpu/%u.v", __VIDEO_SEGMENT_RECON_COUNTER++);

    FILE* video_track_dump_fp = fopen(video_track_dump_filename, "w");
    fwrite(__VIDEO_RECON_FRAGMENT, __VIDEO_RECON_FRAGMENT_SIZE, 1, video_track_dump_fp);
    fclose(video_track_dump_fp);
    free(video_track_dump_filename);

	const char* file2 = "36";
	isoBMFFTrackJoinerResources->file2_name = (char*)calloc(strlen(file2)+1, sizeof(char));
	strncpy(isoBMFFTrackJoinerResources->file2_name, file2, strlen(file2));

	isoBMFFTrackJoinerResources->file2_payload = __AUDIO_RECON_FRAGMENT;
	isoBMFFTrackJoinerResources->file2_size = __AUDIO_RECON_FRAGMENT_SIZE;
    
    char* audio_track_dump_filename = (char*)calloc(128, sizeof(char));
    snprintf(audio_track_dump_filename, 127, "mpu/%u.a", __AUDIO_SEGMENT_RECON_COUNTER++);
    
    FILE* audio_track_dump_fp = fopen(audio_track_dump_filename, "w");
    fwrite(__AUDIO_RECON_FRAGMENT, __AUDIO_RECON_FRAGMENT_SIZE, 1, audio_track_dump_fp);
    fclose(audio_track_dump_fp);
    free(audio_track_dump_filename);

	return isoBMFFTrackJoinerResources;
}

/**
 
 todo: move this into lls_slt_monitor and lls_sls_monitor_output_buffer.c
 **/
block_t* atsc3_isobmff_build_mpu_metadata_ftyp_moof_mdat_box(udp_flow_t* udp_flow, udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, mmtp_sub_flow_vector_t* mmtp_sub_flow_vector, lls_sls_mmt_monitor_t* lls_sls_mmt_monitor) {

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

 		mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

		if(mmtp_sub_flow && mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.size) {
            int all_mpu_metadata_fragment_walk_count = mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.size-1;
 
            //mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data ?
            while(all_mpu_metadata_fragment_walk_count>1) {
                mpu_metadata_fragments =  mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector.data[all_mpu_metadata_fragment_walk_count--];

                if(mpu_metadata_fragments) {
                    int fragment_walk_count = 0;
                    bool found_mpu_metadata_fragment = false;
 
                        //&& mpu_metadata_fragments->timed_fragments_vector.
                        //while(fragment_walk_count < mpu_metadata_fragments->timed_fragments_vector.size) {
                    while(fragment_walk_count < 1 && mpu_metadata_fragments->timed_fragments_vector.size) {

                        mpu_metadata_fragment = mpu_metadata_fragments->timed_fragments_vector.data[fragment_walk_count++];
                        if(mpu_metadata_fragment && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload && mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer ) {
                            found_mpu_metadata_fragment = true;
                            break;
                        }
                        mpu_metadata_fragment = NULL;
                    }
                }
 
				if(!mpu_metadata_fragment) {
					__ISOBMFF_TOOLS_WARN("Unable to find mpu_metadata from mmt_flows");
					return NULL;
				}

				__ISOBMFF_TOOLS_DEBUG("Found an mpu_metadata packet_id: %d, mpu_sequence_number: %u", mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id, mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_sequence_number);

				//TODO - fix me to use the proper mbms packet id's for video and audio packet_id's
				if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->video_packet_id) {
					video_isobmff_header = mpu_metadata_fragment->mmtp_mpu_type_packet_header.mpu_data_unit_payload;
				} else if(mpu_metadata_fragment->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->audio_packet_id) {
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

    if(!lls_sls_mmt_monitor->lls_sls_monitor_output_buffer.has_written_init_box) {
		__copy_video_block_t(video_isobmff_header);
		__copy_audio_block_t(audio_isobmff_header);

    }
	//just do this statically here for now, packet_id=35 is video, packet_id=36 is audio
    
	for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
        
		udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];

        //remember, subflows are built in case of DU fragmentation - korean MMT samples have this edge case
		mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);
        if(!mmtp_sub_flow) {
            __ISOBMFF_TOOLS_WARN("mmtp_sub_flow for packet_id: %u is null", udp_flow_packet_id_mpu_sequence_tuple->packet_id);
            //goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
            return NULL;
        }
        //hack
		movie_metadata_fragments = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->movie_fragment_metadata_vector, udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number - 1);

		mmtp_payload_fragments_union_t* fragment_metadata = NULL;
		if(movie_metadata_fragments && movie_metadata_fragments->timed_fragments_vector.size) {
            for(int i=0; i < movie_metadata_fragments->timed_fragments_vector.size; i++) {
                fragment_metadata = movie_metadata_fragments->timed_fragments_vector.data[i];
                __ISOBMFF_TOOLS_INFO("Movie Fragment Metadata: Found for fragment_metadata packet_id: %d, mpu_sequence_number: %u, fragmentation_indicator: %u, fragmentation_counter: %u", fragment_metadata->mmtp_packet_header.mmtp_packet_id, udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number, fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_indicator, fragment_metadata->mmtp_mpu_type_packet_header.mpu_fragmentation_counter);

                if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->video_packet_id) {

                    __copy_video_block_t(fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                } else if(udp_flow_packet_id_mpu_sequence_tuple->packet_id == lls_sls_mmt_monitor->audio_packet_id) {
                    __copy_audio_block_t(fragment_metadata->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                }
            }
        } else {
            __ISOBMFF_TOOLS_WARN("Movie Fragment Metadata is NULL!");
            //goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
            return NULL;
        }
    }
   
    //double hack to patch the mdat box size
    uint32_t video_du_size = 0;
    uint32_t audio_du_size = 0;

    for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
        udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];
        
        mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

        data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->media_fragment_unit_vector, udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number - 1 );

        mmtp_payload_fragments_union_t* mpu_data_unit_payload_fragments_timed = NULL;

        if(data_unit_payload_types && data_unit_payload_types->timed_fragments_vector.size) {
            total_fragments = data_unit_payload_types->timed_fragments_vector.size;
            //mpu_data_unit_payload_fragments_timed = data_unit_payload_types->timed_fragments_vector;
            //push to mpu_push_output_buffer
            for(int i=0; i < total_fragments; i++) {
                //mmtp_payload_fragments_union_t* packet = data_unit->data[i];
                mmtp_payload_fragments_union_t* data_unit = data_unit_payload_types->timed_fragments_vector.data[i];

                if(data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->video_packet_id && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) {
                    video_du_size += data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer;
                } else if(data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->audio_packet_id && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id){
                    audio_du_size += data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer;
                }
            }

        } else {
            __ISOBMFF_TOOLS_WARN("data unit size is null");
            //goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
            return NULL;
        }
    }

    if(!__AUDIO_RECON_FRAGMENT_SIZE || !__VIDEO_RECON_FRAGMENT_SIZE || !video_du_size || !audio_du_size) {
        __ISOBMFF_TOOLS_WARN("Returning - data unit size is null: audio_recon_fragment_size: %u, video_recon_fragment_size: %u, video_du_size: %u, audio_du_size: %u", __AUDIO_RECON_FRAGMENT_SIZE, __VIDEO_RECON_FRAGMENT_SIZE, video_du_size, audio_du_size);

        return NULL;
    }
    uint8_t mdat_box[8];
//	__MMT_MPU_INFO("total_mdat_body_size: %u, total box size: %u", total_mdat_body_size, total_mdat_body_size+8);
    uint32_t total_mdat_body_size = video_du_size + 8;

    memcpy(&mdat_box, &__VIDEO_RECON_FRAGMENT[__VIDEO_RECON_FRAGMENT_SIZE-8], 8);

    if(mdat_box[4] == 'm' && mdat_box[5] == 'd' && mdat_box[6] == 'a' && mdat_box[7] == 't') {
        mdat_box[0] = (total_mdat_body_size >> 24) & 0xFF;
        mdat_box[1] = (total_mdat_body_size >> 16) & 0xFF;
        mdat_box[2] = (total_mdat_body_size >> 8) & 0xFF;
        mdat_box[3] = (total_mdat_body_size) & 0xFF;


        memcpy(&__VIDEO_RECON_FRAGMENT[__VIDEO_RECON_FRAGMENT_SIZE-8], &mdat_box, 4);
//			__MMT_MPU_INFO("last 8 bytes of metadata fragment updated to: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
//											mdat_box[0], mdat_box[1], mdat_box[2], mdat_box[3], mdat_box[4], mdat_box[5], mdat_box[6], mdat_box[7]);

    } else {
        __MMT_MPU_ERROR("fragment metadata packet, cant find trailing mdat!");
    }

    //	__MMT_MPU_INFO("total_mdat_body_size: %u, total box size: %u", total_mdat_body_size, total_mdat_body_size+8);
    total_mdat_body_size = audio_du_size + 8;

    memcpy(&mdat_box, &__AUDIO_RECON_FRAGMENT[__AUDIO_RECON_FRAGMENT_SIZE-8], 8);

    if(mdat_box[4] == 'm' && mdat_box[5] == 'd' && mdat_box[6] == 'a' && mdat_box[7] == 't') {
        mdat_box[0] = (total_mdat_body_size >> 24) & 0xFF;
        mdat_box[1] = (total_mdat_body_size >> 16) & 0xFF;
        mdat_box[2] = (total_mdat_body_size >> 8) & 0xFF;
        mdat_box[3] = (total_mdat_body_size) & 0xFF;


        memcpy(&__AUDIO_RECON_FRAGMENT[__AUDIO_RECON_FRAGMENT_SIZE-8], &mdat_box, 4);
//			__MMT_MPU_INFO("last 8 bytes of metadata fragment updated to: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
//											mdat_box[0], mdat_box[1], mdat_box[2], mdat_box[3], mdat_box[4], mdat_box[5], mdat_box[6], mdat_box[7]);

    } else {
        __MMT_MPU_ERROR("fragment metadata packet, cant find trailing mdat!");
    }

    for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
        udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];
        
        mmtp_sub_flow = mmtp_sub_flow_vector_find_packet_id(mmtp_sub_flow_vector, udp_flow_packet_id_mpu_sequence_tuple->packet_id);

        //hack for the previous sequence number which should be complete
        data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mmtp_sub_flow->mpu_fragments->media_fragment_unit_vector, udp_flow_packet_id_mpu_sequence_tuple->mpu_sequence_number - 1);

       mmtp_payload_fragments_union_t* mpu_data_unit_payload_fragments_timed = NULL;

        if(data_unit_payload_types && data_unit_payload_types->timed_fragments_vector.size) {
            total_fragments = data_unit_payload_types->timed_fragments_vector.size;
            //push to mpu_push_output_buffer
            for(int i=0; i < total_fragments; i++) {
                mmtp_payload_fragments_union_t* data_unit = data_unit_payload_types->timed_fragments_vector.data[i];

                //only push to our correct mpu flow....
                if(data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->video_packet_id && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id) {
                    __copy_video_block_t(data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                } else if(data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == lls_sls_mmt_monitor->audio_packet_id && data_unit->mmtp_mpu_type_packet_header.mmtp_packet_id == udp_flow_packet_id_mpu_sequence_tuple->packet_id){
                    __copy_audio_block_t(data_unit->mmtp_mpu_type_packet_header.mpu_data_unit_payload);
                }
            }

        } else {
            __ISOBMFF_TOOLS_WARN("data unit size is null");
            //goto purge_pending_mfu_and_update_previous_mmtp_payload; //for now
            return NULL;
        }
    }


	if(!__VIDEO_RECON_FRAGMENT_SIZE || !__AUDIO_RECON_FRAGMENT_SIZE) {
        __ISOBMFF_TOOLS_WARN("returning null - video size is: %d, audio size is: %d", __VIDEO_RECON_FRAGMENT_SIZE, __AUDIO_RECON_FRAGMENT_SIZE);

		return NULL;
	}
    __ISOBMFF_TOOLS_INFO("video size is: %d, audio size is: %d", __VIDEO_RECON_FRAGMENT_SIZE, __AUDIO_RECON_FRAGMENT_SIZE);


	AP4_DataBuffer* dataBuffer = new AP4_DataBuffer(4096000);
	AP4_MemoryByteStream* memoryOutputByteStream = new AP4_MemoryByteStream(dataBuffer);

    //TODO - fix me
	ISOBMFFTrackJoinerFileResouces_t* fileResources = _l_loadFileResources();
    parseAndBuildJoinedBoxes(fileResources, memoryOutputByteStream);

    __ISOBMFF_TOOLS_WARN("building reutrn alloc of %u", dataBuffer->GetDataSize());
	mpu_metadata_output_block_t = block_Alloc(dataBuffer->GetDataSize());
	memcpy(mpu_metadata_output_block_t->p_buffer, dataBuffer->GetData(), dataBuffer->GetDataSize());

	//trying to free this causes segfaults 100% of the time

	free (dataBuffer);
    
    char* box_track_dump_filename = (char*)calloc(128, sizeof(char));
    snprintf(box_track_dump_filename, 127, "mpu/%u.b", __BOX_SEGMENT_RECON_COUNTER++);
    
    FILE* box_track_dump_fp = fopen(box_track_dump_filename, "w");
    fwrite(mpu_metadata_output_block_t->p_buffer, mpu_metadata_output_block_t->i_buffer, 1, box_track_dump_fp);
    fclose(box_track_dump_fp);
    free(box_track_dump_filename);
    
    
	return mpu_metadata_output_block_t;
}


