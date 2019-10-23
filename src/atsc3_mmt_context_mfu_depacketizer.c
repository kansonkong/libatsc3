/*
 * atsc3_mmt_reconstitution_from_media_sample.c
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 *
 *
 *
 *      TODO:
 *

A/331: 8.1.2.3 Synchronization:
The synchronization of MPUs shall be done by using timestamps referencing UTC
delivered by ATSC 3.0 PHY layer.

The MPU_timestamp_descriptor as defined in subclause 10.5.2 of ISO/IEC 23008-1 [37]
shall be used to represent the presentation time of the first media sample in
presentation order in each MPU.

The presentation time of each media sample of an MPU shall be calculated by
adding the presentation time of the first media sample of an MPU to the value
of the composition time of each sample in the MPU.

The rule to calculate the value of the composition time of media samples in an MPU
shall be be calculated by using the rule in the ISO BMFF specification [34].


23008-1 reference:

CRI table:

CRI descriptor: 0x0000

MPU_timestamp_descriptor: 0x0001

 */

#include "atsc3_mmt_context_mfu_depacketizer.h"
#include "atsc3_mmtp_packet_types.h"

int _MMT_CONTEXT_MPU_SIGNAL_INFO_ENABLED = 0;
int _MMT_CONTEXT_MPU_DEBUG_ENABLED = 1;
int _MMT_CONTEXT_MPU_TRACE_ENABLED = 0;

//TODO: jjustman-2019-10-03 - refactor these out to proper impl's:

//MPU
void atsc3_mmt_mpu_on_sequence_number_change_noop(uint16_t packet_id, uint32_t mpu_sequence_number_old, uint32_t mpu_sequence_number_new) {
	//noop
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_on_sequence_number_change_noop: packet_id: %u, from %d, to %d", packet_id, mpu_sequence_number_old,  mpu_sequence_number_new);
}

//MPU - init box (mpu_metadata) present
void atsc3_mmt_mpu_on_sequence_mpu_metadata_present_noop(uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_mpu_metadata) {
    //noop
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_on_sequence_mpu_metadata_present_noop: packet_id: %u, mpu_sequence_number: %u, mmt_mpu_metadata: %p, size: %d",
        packet_id,
        mpu_sequence_number,
        mmt_mpu_metadata,
        mmt_mpu_metadata->p_size);

    //todo: extract NALs for example here...
}

//MP_table
void atsc3_mmt_signalling_information_on_mp_table_subset_noop(mp_table_t* mp_table) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_mp_table_subset_noop: mp_table: %p", mp_table);
}

//MP_table
void atsc3_mmt_signalling_information_on_mp_table_complete_noop(mp_table_t* mp_table) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_mp_table_complete_noop: mp_table: %p", mp_table);
}

//audio essence packet id extraction
void atsc3_mmt_signalling_information_on_audio_essence_packet_id_noop(uint16_t audio_packet_id) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_audio_packet_id_noop: audio_packet_id: %u", audio_packet_id);
}

//video essence packet_id extraction
void atsc3_mmt_signalling_information_on_video_essence_packet_id_noop(uint16_t video_packet_id) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_video_packet_id_noop: video_packet_id: %u", video_packet_id);
}

//stpp essence packet_id extraction
void atsc3_mmt_signalling_information_on_stpp_essence_packet_id_noop(uint16_t stpp_packet_id) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_stpp_packet_id_noop: stpp_packet_id: %u", stpp_packet_id);
}


//audio packet id extraction with mpu_sequence and mpu_presentation_time
void atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_noop(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_noop: audio_packet_id: %u, mpu_sequence_number: %u, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
			audio_packet_id,
			mpu_sequence_number,
			mpu_presentation_time_ntp64,
			mpu_presentation_time_seconds,
			mpu_presentation_time_microseconds);
}

//video packet_id extraction with mpu_sequence and mpu_presentation_time
void atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_noop(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_noop: video_packet_id: %u, mpu_sequence_number: %u, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
			video_packet_id,
			mpu_sequence_number,
			mpu_presentation_time_ntp64,
			mpu_presentation_time_seconds,
			mpu_presentation_time_microseconds);
}

//stpp packet_id extraction with mpu_sequence and mpu_presentation_time
void atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_noop(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_noop: stpp_packet_id: %u, mpu_sequence_number: %u, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
			stpp_packet_id,
			mpu_sequence_number,
			mpu_presentation_time_ntp64,
			mpu_presentation_time_seconds,
			mpu_presentation_time_microseconds);
}


void atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_noop(uint16_t packet_id, uint32_t mpu_sequence_number, mmt_signalling_message_mpu_timestamp_descriptor_t* mmt_signalling_message_mpu_timestamp_descriptor) {
	//noop;
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_noop: packet_id: %u, mpu_sequence_number: %d, mmt_signalling_message_mpu_timestamp_descriptor: %p",
				packet_id,
				mpu_sequence_number,
				mmt_signalling_message_mpu_timestamp_descriptor);
}

//MFU callbacks

void atsc3_mmt_mpu_mfu_on_sample_complete_noop(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_rebuilt) {
	//noop;
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_complete_noop: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d, mfu_fragment count rebuilt: %d",
			packet_id,
            mpu_sequence_number,
            sample_number,
			mmt_mfu_sample,
			mmt_mfu_sample->p_size,
            mfu_fragment_count_rebuilt);
}

void atsc3_mmt_mpu_mfu_on_sample_corrupt_noop(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_corrupt_noop: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d, mfu_fragment count expected: %d, rebuilt %d",
        packet_id,
        mpu_sequence_number,
        sample_number,
        mmt_mfu_sample,
        mmt_mfu_sample->p_size,
        mfu_fragment_count_expected,
        mfu_fragment_count_rebuilt);
}
    


void atsc3_mmt_mpu_mfu_on_sample_missing_noop(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
	__MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_missing_noop: packet_id: %u, mpu_sequence_number: %u, sample_number: %u",
        packet_id,
        mpu_sequence_number,
        sample_number);
}


atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_new() {
	atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = calloc(1, sizeof(atsc3_mmt_mfu_context_t));

	//MPU related callbacks
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change 						= &atsc3_mmt_mpu_on_sequence_number_change_noop;
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present               = &atsc3_mmt_mpu_on_sequence_mpu_metadata_present_noop;

	//signalling information callbacks
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_subset			= &atsc3_mmt_signalling_information_on_mp_table_subset_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_complete 		= &atsc3_mmt_signalling_information_on_mp_table_complete_noop;

	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_essence_packet_id 	= &atsc3_mmt_signalling_information_on_audio_essence_packet_id_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_essence_packet_id 	= &atsc3_mmt_signalling_information_on_video_essence_packet_id_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_essence_packet_id 	= &atsc3_mmt_signalling_information_on_stpp_essence_packet_id_noop;

	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor  = &atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_noop;

	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_noop;

	//MFU related callbacks
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete = &atsc3_mmt_mpu_mfu_on_sample_complete_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt 	= &atsc3_mmt_mpu_mfu_on_sample_corrupt_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_missing 	= &atsc3_mmt_mpu_mfu_on_sample_missing_noop;

	return atsc3_mmt_mfu_context;
}

void mmtp_mfu_process_from_payload_with_context(udp_packet_t *udp_packet,
											mmtp_mpu_packet_t* mmtp_mpu_packet,
											atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context) {

	atsc3_mmt_mfu_context->udp_flow = &udp_packet->udp_flow;

	//borrow from our context
	mmtp_flow_t *mmtp_flow = atsc3_mmt_mfu_context->mmtp_flow;
	lls_slt_monitor_t* lls_slt_monitor = atsc3_mmt_mfu_context->lls_slt_monitor;
	lls_sls_mmt_session_t* matching_lls_sls_mmt_session = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session;
	udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container = atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container;

    //forward declare so our goto will compile
    mmtp_asset_flow_t* mmtp_asset_flow = NULL;
    mmtp_asset_t* mmtp_asset = NULL;
    mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container = NULL;
    mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = NULL;

    lls_sls_mmt_monitor_t* matching_lls_sls_mmt_monitor = lls_sls_mmt_monitor_find_from_service_id(lls_slt_monitor, matching_lls_sls_mmt_session->service_id);

    if(!matching_lls_sls_mmt_monitor) {
    	//we may not be monitoring this atsc3 service_id, so discard

        __MMT_CONTEXT_MPU_TRACE("mmtp_mfu_process_from_payload_with_context: service_id: %u, packet_id: %u, lls_slt_monitor size: %u, matching_lls_sls_mmt_monitor is NULL!",
                matching_lls_sls_mmt_session->service_id,
                mmtp_mpu_packet->mmtp_packet_id,
                lls_slt_monitor->lls_sls_mmt_monitor_v.count);

        goto packet_cleanup;
    }

    //assign our mmtp_mpu_packet to asset/packet_id/mpu_sequence_number flow
    mmtp_asset_flow = mmtp_flow_find_or_create_from_udp_packet(mmtp_flow, udp_packet);
    mmtp_asset = mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow, matching_lls_sls_mmt_session);
    mmtp_packet_id_packets_container = mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset, mmtp_mpu_packet);
    mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_or_create_mpu_sequence_number_mmtp_mpu_packet_collection_from_mmt_mpu_packet(mmtp_packet_id_packets_container, mmtp_mpu_packet);

    //persist our mmtp_mpu_packet for mpu reconstitution as per original libatsc3 design
    mpu_sequence_number_mmtp_mpu_packet_collection_add_mmtp_mpu_packet(mpu_sequence_number_mmtp_mpu_packet_collection, mmtp_mpu_packet);

    //TODO - this should never happen with this strong type
    
    if(mmtp_mpu_packet->mmtp_payload_type == 0x0) {
        atsc3_global_statistics->packet_counter_mmt_mpu++;

        if(mmtp_mpu_packet->mpu_timed_flag == 1) {
            atsc3_global_statistics->packet_counter_mmt_timed_mpu++;

            if(matching_lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id == matching_lls_sls_mmt_session->service_id &&
            		matching_lls_sls_mmt_session->sls_destination_ip_address == udp_packet->udp_flow.dst_ip_addr &&
					matching_lls_sls_mmt_session->sls_destination_udp_port == udp_packet->udp_flow.dst_port) {

                /**
                 
                 we only care about:
                    mfu's that start with a sample header (and fi=0): mmtp_mpu_packet->mmthsample_header
                 
                    otherwise, check and recon if necessary:
                        mmtp_mpu_packet->mpu_fragment_type=0x0 - mpu_metadata
                        mmtp_mpu_packet->mpu_fragment_type=0x2 - mfu
                 */
            	udp_flow_packet_id_mpu_sequence_tuple_t* last_flow_reference = udp_flow_latest_mpu_sequence_number_add_or_replace_and_check_for_rollover(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_mpu_packet, lls_slt_monitor, matching_lls_sls_mmt_session, mmtp_flow);

            	char* essence_type = (matching_lls_sls_mmt_monitor->audio_packet_id == mmtp_mpu_packet->mmtp_packet_id) ? "a" : "v";
				//see if we are at the start of a new mfu sample
                //implies mmtp_mpu_packet->mpu_fragment_type==0x02

                //jjustman-2019-10-23: do not optimisticly emit mmthsample_header du payload, otehrwise we may lose partial MFU emission flush below...
				if(false && mmtp_mpu_packet->mmthsample_header) {
					__MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_process_from_payload_with_context: new MFU.MMTHSample: packet_id: %u (%c), mpu_sequence_number: %u, mmthsample.samplenumber: %u, mmthsample.movie_fragment_sequence_number: %u, mmthsample.length: %u, mfu packet size: %u, fragmentation_indicator: %u",
						mmtp_mpu_packet->mmtp_packet_id,
						*essence_type,
						mmtp_mpu_packet->mpu_sequence_number,
						mmtp_mpu_packet->mmthsample_header->samplenumber,
						mmtp_mpu_packet->mmthsample_header->movie_fragment_sequence_number,
						mmtp_mpu_packet->mmthsample_header->length,
                        mmtp_mpu_packet->du_mfu_block ? mmtp_mpu_packet->du_mfu_block->p_size : 0,
                        mmtp_mpu_packet->mpu_fragmentation_indicator);

                    //in the case of audio (or video P frame) packets, our du mfu packet size should be equal to the mmthsample_header->length value,
                    if(mmtp_mpu_packet->du_mfu_block->p_size == mmtp_mpu_packet->mmthsample_header->length) {
                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->mmthsample_header->samplenumber, block_Duplicate(mmtp_mpu_packet->du_mfu_block), 1);
                    } else if(mmtp_mpu_packet->mpu_fragmentation_indicator == 0x00) {
                    	//otherwise, check mpu_fragmentation_indicator...only process if we are marked as fi=0x00 (complete data unit)
                    	//let DU rebuild handle any other packets
                        __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_process_from_payload_with_context: mmthsample mismatch with du_mfu_block and frag_indicator==0x00, mpu_fragmentation_indicator == 0x00, but du_mfu_block.size (%u) != mmthsample_header->length (%u), packet_id: %u (%c), mpu_sequence_number: %u, mmthsample.samplenumber: %u, mmthsample.movie_fragment_sequence_number: %u, mmthsample.length: %u, mfu packet size: %u, fragmentation_indicator: %u",
                            mmtp_mpu_packet->du_mfu_block->p_size,
                            mmtp_mpu_packet->mmthsample_header->length,
                            mmtp_mpu_packet->mmtp_packet_id,
                            *essence_type,
                            mmtp_mpu_packet->mpu_sequence_number,
                            mmtp_mpu_packet->mmthsample_header->samplenumber,
                            mmtp_mpu_packet->mmthsample_header->movie_fragment_sequence_number,
                            mmtp_mpu_packet->mmthsample_header->length,
                            mmtp_mpu_packet->du_mfu_block ? mmtp_mpu_packet->du_mfu_block->p_size : 0,
                            mmtp_mpu_packet->mpu_fragmentation_indicator);

                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->mmthsample_header->samplenumber, block_Duplicate(mmtp_mpu_packet->du_mfu_block), 1);
                    }

				} else {

                    //process mpu_metadata
                    if(mmtp_mpu_packet->mpu_fragment_type == 0x0) {
                        __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_process_from_payload_with_context: MPU Metadata: init box: packet_id: %u (%c), mpu_sequence_number: %u, mmtp_mpu_packet.samplenumber: %u, mmtp_mpu_packet.mpu_fragment_counter: %u, mmtp_mpu_packet.offset: %u, du_mpu_metadata_block packet size: %u, fragmentation_indicator: %u",
                        mmtp_mpu_packet->mmtp_packet_id,
                        *essence_type,
                        mmtp_mpu_packet->mpu_sequence_number,
                        mmtp_mpu_packet->sample_number,
                        mmtp_mpu_packet->mpu_fragment_counter,
                        mmtp_mpu_packet->offset,
                        mmtp_mpu_packet->du_mpu_metadata_block ? mmtp_mpu_packet->du_mpu_metadata_block->p_size : 0,
                        mmtp_mpu_packet->mpu_fragmentation_indicator);

                        //one (or more) DU
                        if(mmtp_mpu_packet->mpu_fragmentation_indicator == 0x00) {
                            atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present(mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, block_Duplicate(mmtp_mpu_packet->du_mpu_metadata_block));
                        } else if(mmtp_mpu_packet->mpu_fragmentation_indicator == 0x03) {

                            //rebuild mmtp_mpu_packet if we have our closing fragment...
                            block_t* du_mpu_metadata_block_rebuilt = NULL;
                            mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, mmtp_mpu_packet->mpu_sequence_number);
                            for(int i=0; i < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count; i++) {
                                mmtp_mpu_packet_t* mmtp_mpu_packet_to_rebuild = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[i];

                                //assume no loss/re-sequencing for now..
                                if(mmtp_mpu_packet_to_rebuild->du_mpu_metadata_block) {
                                    if(!du_mpu_metadata_block_rebuilt) {
                                        __MMT_CONTEXT_MPU_DEBUG("i: %u, psn: %u, Found MPU Metadata with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                                              i,
                                                              mmtp_mpu_packet_to_rebuild->packet_sequence_number,
                                                              atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                                              atsc3_mmt_mfu_context->udp_flow->dst_port,
                                                              mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                                              mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                                              mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);

                                        du_mpu_metadata_block_rebuilt = block_Duplicate(mmtp_mpu_packet_to_rebuild->du_mpu_metadata_block);
                                    } else {
                                        __MMT_CONTEXT_MPU_DEBUG("i: %u, psn: %u, Appending MPU Metadata with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                                              i,
                                                              mmtp_mpu_packet_to_rebuild->packet_sequence_number,
                                                              atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                                              atsc3_mmt_mfu_context->udp_flow->dst_port,
                                                              mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                                              mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                                              mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);

                                        block_Merge(du_mpu_metadata_block_rebuilt, mmtp_mpu_packet_to_rebuild->du_mpu_metadata_block);
                                    }
                                }
                            }

                            if(du_mpu_metadata_block_rebuilt && du_mpu_metadata_block_rebuilt->p_size) {
                                atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present(mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, du_mpu_metadata_block_rebuilt);
                            } else {
                                __MMT_CONTEXT_MPU_ERROR("psn: %u, MPU Metadata: du_mpu_metadata_block_rebuilt is null or size == 0! with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                                        mmtp_mpu_packet->packet_sequence_number,
                                                        atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                                        atsc3_mmt_mfu_context->udp_flow->dst_port,
                                                        mmtp_mpu_packet->mmtp_packet_id,
                                                        mmtp_mpu_packet->mpu_sequence_number,
                                                        mmtp_mpu_packet->mpu_fragmentation_indicator);
                            }
                        }
                    } else if(mmtp_mpu_packet->mpu_fragment_type == 0x2) {

                        //MFU rebuild any pending packets less than our sample_number,
                        //regardless of mpu_fragmentation_indicator== as it may be lost in emission...
                        //TODO: check previous mpu_sequence_number for any remainder packets to flush...
                        mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, mmtp_mpu_packet->mpu_sequence_number);

                        if(mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count) {

							//rebuild mmtp_mpu_packet if we have our closing fragment...
							//todo: jjustman-2019-10-03 - how to handle missing first NAL or partial DU's?
							//and compute relative DU offset for rebuilding MFU
							//first DU in MFU should contain MMTHSample,
							//last DU in MFU should contain mpu_fragment_counter == 0;

							uint32_t mfu_sample_number_from_du = mmtp_mpu_packet->sample_number;
							mmtp_mpu_packet_t *mmtp_mpu_packet_to_rebuild_last = NULL;

							block_t *du_mfu_block_rebuilt = NULL;
							int du_mfu_block_rebuild_index_start = -1;

							int32_t mfu_fragment_counter_mmthsample_header_start = 0;
							int32_t mfu_fragment_counter_missing_mmthsample_header_start = 0;
							int32_t mfu_fragment_counter_position = 0;
							int32_t mfu_fragment_count_rebuilt = 0;

							for (int i = 0; i <mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count; i++) {
								mmtp_mpu_packet_t *mmtp_mpu_packet_to_rebuild = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[i];

								if (mmtp_mpu_packet_to_rebuild->mfu_reassembly_performed) {
                                    continue;
								}

								if(mmtp_mpu_packet_to_rebuild->mpu_fragment_type != 0x02)    //skip over this packet, but should eventually remove it frmom the mpu_sequence_number_mmtp_mpu_packet_collection...
									continue;




								//todo: jjustman-2019-10-23 - refactor me out
								//notify any pending mmtp_mpu_packet_t's that haven't been emitted yet (e.g. might be missing 0x03 FI)
								if (mmtp_mpu_packet_to_rebuild_last && mmtp_mpu_packet_to_rebuild_last->mfu_reassembly_performed == false && mmtp_mpu_packet_to_rebuild_last->sample_number != mmtp_mpu_packet_to_rebuild->sample_number) {
									//mark packets as rebuilt, should remove from mpu_sequence_number_mmtp_mpu_packet_collection instead...
									if (du_mfu_block_rebuilt && du_mfu_block_rebuilt->p_size) {
									    if(du_mfu_block_rebuild_index_start >= 0) {
                                            for(int j=du_mfu_block_rebuild_index_start; j < i; j++) {
                                                mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[j]->mfu_reassembly_performed = true;

                                                //hack, handle this context/state better
                                                block_Destroy(&mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[j]->du_mfu_block);

                                            }
                                        }

										__MMT_CONTEXT_MPU_DEBUG(
												"MFU packet_id: %u, mpu_sequence_number: %u, emission: sample_number: %u, fragment_indicator: %u, mfu_fragment_counter_mmthsample_header_start: %d, mpu_fragment_counter_position: %d, mpu_fragment_count_rebuilt: %d",
												mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id,
												mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number,
												mfu_sample_number_from_du,
												mmtp_mpu_packet_to_rebuild_last->mpu_fragmentation_indicator,
												mfu_fragment_counter_mmthsample_header_start,
												mfu_fragment_counter_position,
												mfu_fragment_count_rebuilt);

										if (mfu_fragment_counter_mmthsample_header_start) {
											if (mfu_fragment_counter_mmthsample_header_start - (mfu_fragment_count_rebuilt - 1) == 0) {
												atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(
														mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id,
														mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number,
														mmtp_mpu_packet_to_rebuild_last->sample_number,
														du_mfu_block_rebuilt,
														mfu_fragment_count_rebuilt);
                                                __MMT_CONTEXT_MPU_DEBUG(
                                                        "atsc3_mmt_mpu_mfu_on_sample_complete MFU DU with MMTHSample, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, mmthsample.length: %u, du_mfu_block_rebuilt: %p, ->p_size: %u",
                                                        mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                                        mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                                        mmtp_mpu_packet_to_rebuild->sample_number,
                                                        mmtp_mpu_packet_to_rebuild->mpu_fragment_counter,
                                                        mmtp_mpu_packet_to_rebuild->packet_sequence_number,
                                                        atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                                        atsc3_mmt_mfu_context->udp_flow->dst_port,
                                                        mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator,
                                                        mmtp_mpu_packet_to_rebuild->mmthsample_header->length,
														du_mfu_block_rebuilt,
														du_mfu_block_rebuilt ? du_mfu_block_rebuilt->p_size : 0);
											} else {
												atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt(
														mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id,
														mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number,
														mmtp_mpu_packet_to_rebuild_last->sample_number,
														du_mfu_block_rebuilt,
														mfu_fragment_counter_mmthsample_header_start,
														mfu_fragment_count_rebuilt);
                                                __MMT_CONTEXT_MPU_DEBUG(
                                                        "atsc3_mmt_mpu_mfu_on_sample_corrupt MFU DU with MMTHSample, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, mmthsample.length: %u, du_mfu_block_rebuilt: %p, du_mfu_block_rebuilt->p_size: %u",
                                                        mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                                        mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                                        mmtp_mpu_packet_to_rebuild->sample_number,
                                                        mmtp_mpu_packet_to_rebuild->mpu_fragment_counter,
                                                        mmtp_mpu_packet_to_rebuild->packet_sequence_number,
                                                        atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                                        atsc3_mmt_mfu_context->udp_flow->dst_port,
                                                        mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator,
                                                        mmtp_mpu_packet_to_rebuild->mmthsample_header->length,
														du_mfu_block_rebuilt,
														du_mfu_block_rebuilt ? du_mfu_block_rebuilt->p_size : 0 );
											}
										} else {
											atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header(
													mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id,
													mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number,
													mmtp_mpu_packet_to_rebuild_last->sample_number,
													du_mfu_block_rebuilt,
													mfu_fragment_counter_missing_mmthsample_header_start,
													mfu_fragment_count_rebuilt);

                                            __MMT_CONTEXT_MPU_DEBUG(
                                                    "atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header MFU DU with MMTHSample, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, mmthsample.length: %u, du_mfu_block_rebuilt: %p, du_mfu_block_rebuilt->p_size: %u",
                                                    mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                                    mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                                    mmtp_mpu_packet_to_rebuild->sample_number,
                                                    mmtp_mpu_packet_to_rebuild->mpu_fragment_counter,
                                                    mmtp_mpu_packet_to_rebuild->packet_sequence_number,
                                                    atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                                    atsc3_mmt_mfu_context->udp_flow->dst_port,
                                                    mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator,
                                                    mmtp_mpu_packet_to_rebuild->mmthsample_header->length,
													du_mfu_block_rebuilt,
													du_mfu_block_rebuilt ? du_mfu_block_rebuilt->p_size : 0
                                                    );

                                        }
                                        __MMT_CONTEXT_MPU_DEBUG(
                                                "atsc3 block_destroy MFU DU with MMTHSample, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, mmthsample.length: %u, du_mfu_block_rebuilt: %p, sdu_mfu_block_rebuilt->p_size: %u",
                                                mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                                mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                                mmtp_mpu_packet_to_rebuild->sample_number,
                                                mmtp_mpu_packet_to_rebuild->mpu_fragment_counter,
                                                mmtp_mpu_packet_to_rebuild->packet_sequence_number,
                                                atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                                atsc3_mmt_mfu_context->udp_flow->dst_port,
                                                mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator,
                                                mmtp_mpu_packet_to_rebuild->mmthsample_header->length,
												du_mfu_block_rebuilt,
												du_mfu_block_rebuilt ? du_mfu_block_rebuilt->p_size : 0
                                        );

                                        block_Destroy(&du_mfu_block_rebuilt);

                                    } else {
										__MMT_CONTEXT_MPU_ERROR(
												"ERROR: du is zero! psn: %u, MPU Metadata: du_mpu_metadata_block_rebuilt is null or size == 0! with %u:%u and packet_id: %u, mpu_sequence_number: %u, sample_number: %u, fragment_indicator: %u",
												mmtp_mpu_packet_to_rebuild->packet_sequence_number,
												atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
												atsc3_mmt_mfu_context->udp_flow->dst_port,
												mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
												mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
												mmtp_mpu_packet_to_rebuild->sample_number,
												mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);

										//jjustman-2019-10-23: TODO: compute last->current mfu_sample_number gap...
										atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_missing(
												mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id,
												mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number,
												mmtp_mpu_packet_to_rebuild_last->sample_number);
									}

									mfu_fragment_counter_mmthsample_header_start = 0;
									mfu_fragment_counter_missing_mmthsample_header_start = 0;
									mfu_fragment_counter_position = 0;
									mfu_fragment_count_rebuilt = 0;
									du_mfu_block_rebuild_index_start = -1;
                                    mmtp_mpu_packet_to_rebuild_last = NULL;
								}

                                if(mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator == 0x00) {
                                    //otherwise, check mpu_fragmentation_indicator...only process if we are marked as fi=0x00 (complete data unit)
                                    //let DU rebuild handle any other packets
                                    __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_process_from_payload_with_context: mpu_fragmentation_indicator==0x00,  du_mfu_block.size (%u) != mmthsample_header->length (%u), packet_id: %u (%c), mpu_sequence_number: %u, mmthsample.samplenumber: %u, mmthsample.movie_fragment_sequence_number: %u, mmthsample.length: %u, mfu packet size: %u, fragmentation_indicator: %u",
															mmtp_mpu_packet_to_rebuild->du_mfu_block ? mmtp_mpu_packet_to_rebuild->du_mfu_block->p_size : 0,

															mmtp_mpu_packet_to_rebuild->mmthsample_header ? mmtp_mpu_packet_to_rebuild->mmthsample_header->length : 0,
                                                            mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                                            *essence_type,
                                                            mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                                            mmtp_mpu_packet_to_rebuild->mmthsample_header->samplenumber,
                                                            mmtp_mpu_packet_to_rebuild->mmthsample_header->movie_fragment_sequence_number,
                                                            mmtp_mpu_packet_to_rebuild->mmthsample_header->length,
                                                            mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);

                                    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, block_Duplicate(mmtp_mpu_packet_to_rebuild->du_mfu_block), 1);
                                    mmtp_mpu_packet_to_rebuild->mfu_reassembly_performed = true;
                                    continue;
                                }


								if (mmtp_mpu_packet_to_rebuild->sample_number < mfu_sample_number_from_du) {
									if(du_mfu_block_rebuild_index_start == -1) {
										du_mfu_block_rebuild_index_start = i;
									}
									if (mmtp_mpu_packet_to_rebuild->du_mfu_block) {
										if (!du_mfu_block_rebuilt) {
											//make sure our first mfu DU for this is set to i_pos == p_size, so we can opportunisticly alloc and make sure block_AppendFull works properly
											mmtp_mpu_packet_to_rebuild->du_mfu_block->i_pos = mmtp_mpu_packet_to_rebuild->du_mfu_block->p_size;
											if (mmtp_mpu_packet_to_rebuild->mmthsample_header) {
												mfu_fragment_counter_mmthsample_header_start = mmtp_mpu_packet_to_rebuild->mpu_fragment_counter;
												mfu_fragment_counter_position = mfu_fragment_counter_mmthsample_header_start;

												/*
                                                 * jjustman-2019-10-12: TODO: use mmthsample.length, pre alloc block_t for MFU rebuild,
                                                 * and then make sure to only append the DU at the proper offset + length
                                                 */
												__MMT_CONTEXT_MPU_DEBUG(
														"Found MFU DU with MMTHSample, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, mmthsample.length: %u, du_mfu_block->p_sie: %u",
                                                        mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                                        mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                                        mmtp_mpu_packet_to_rebuild->sample_number,
                                                        mmtp_mpu_packet_to_rebuild->mpu_fragment_counter,
														mmtp_mpu_packet_to_rebuild->packet_sequence_number,
														atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
														atsc3_mmt_mfu_context->udp_flow->dst_port,
														mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator,
														mmtp_mpu_packet_to_rebuild->mmthsample_header->length,
														mmtp_mpu_packet_to_rebuild->du_mfu_block->p_size);

												//du_mfu_block_rebuilt = block_Duplicate(mmtp_mpu_packet_to_rebuild->du_mfu_block);

												//opportunisic MFU sizing
												//if(mmtp_mpu_packet_to_rebuild->mmthsample_header->length > block_Len(du_mfu_block_rebuilt)) {
												//block_Resize(du_mfu_block_rebuilt,  mmtp_mpu_packet_to_rebuild->mmthsample_header->length);                                                			}
											} else {
												__MMT_CONTEXT_MPU_DEBUG(
														"i: %u, psn: %u, Found MFU DU but missing MMTHSample header start,with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u, du_mfu_block->p_size: %u",
														mmtp_mpu_packet_to_rebuild->mpu_fragment_counter,
														mmtp_mpu_packet_to_rebuild->packet_sequence_number,
														atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
														atsc3_mmt_mfu_context->udp_flow->dst_port,
														mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
														mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
														mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator,
                                                        mmtp_mpu_packet_to_rebuild->du_mfu_block->p_size
														);
												// du_mfu_block_rebuilt = block_Duplicate(mmtp_mpu_packet_to_rebuild->du_mfu_block);
												mfu_fragment_counter_missing_mmthsample_header_start = mmtp_mpu_packet_to_rebuild->mpu_fragment_counter;

											}
											mfu_fragment_counter_position--;
											mfu_fragment_count_rebuilt++;
											du_mfu_block_rebuilt = block_Duplicate(mmtp_mpu_packet_to_rebuild->du_mfu_block);


										} else {
											__MMT_CONTEXT_MPU_DEBUG(
													"Appending MFU DU, packet_id: %u, mpu_sequence_number: %u, sample_number: %u, fragment_counter: %u, psn: %u, with %u:%u,  fragment_indicator: %u",
                                                    mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                                    mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                                    mmtp_mpu_packet_to_rebuild->sample_number,
                                                    mmtp_mpu_packet_to_rebuild->mpu_fragment_counter,
													mmtp_mpu_packet_to_rebuild->packet_sequence_number,
													atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
													atsc3_mmt_mfu_context->udp_flow->dst_port,
													mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);

											mfu_fragment_counter_position--;
											mfu_fragment_count_rebuilt++;
											// block_AppendFull(du_mfu_block_rebuilt, mmtp_mpu_packet_to_rebuild->du_mfu_block);
											block_Merge(du_mfu_block_rebuilt,mmtp_mpu_packet_to_rebuild->du_mfu_block);
										}

									}
								} else {
									//todo: jjustman - 2019-10-23 - mmtp_mpu_packet_to_rebuild shoudl be
//									__MMT_CONTEXT_MPU_INFO(
//											"skipping DU in MFU: packet_id: %u, mpu_sequence_number: %u, sample: %u (packet DU sample: %u), fragment_counter: %u, psn: %u, MFU DU with %u:%u, fragment_indicator: %u",
//											mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
//											mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
//											mmtp_mpu_packet_to_rebuild->sample_number,
//											mfu_sample_number_from_du,
//											mmtp_mpu_packet_to_rebuild->mpu_fragment_counter,
//											mmtp_mpu_packet_to_rebuild->packet_sequence_number,
//											atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
//											atsc3_mmt_mfu_context->udp_flow->dst_port,
//											mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);
								}

								mmtp_mpu_packet_to_rebuild_last = mmtp_mpu_packet_to_rebuild;

								//end of for loop
							}
						}
//                        } else {
//                            __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_process_from_payload_with_context: carry over MFU : packet_id: %u (%c), mpu_sequence_number: %u, mmtp_mpu_packet.samplenumber: %u, mmtp_mpu_packet.mpu_fragment_counter: %u, mmtp_mpu_packet.offset: %u, mfu packet size: %u, fragmentation_indicator: %u",
//                                mmtp_mpu_packet->mmtp_packet_id,
//                                *essence_type,
//                                mmtp_mpu_packet->mpu_sequence_number,
//                                mmtp_mpu_packet->sample_number,
//                                mmtp_mpu_packet->mpu_fragment_counter,
//                                mmtp_mpu_packet->offset,
//                                mmtp_mpu_packet->du_mfu_block ? mmtp_mpu_packet->du_mfu_block->p_size : 0,
//                                mmtp_mpu_packet->mpu_fragmentation_indicator);
//                        }
                    }
				}

                if(matching_lls_sls_mmt_monitor->video_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
                    if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change(mmtp_mpu_packet->mmtp_packet_id, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number, mmtp_mpu_packet->mpu_sequence_number);
                        mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number);
                        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
                        //keep track of our last mpu_sequence_number...
                    }
                    udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video, last_flow_reference);

                } else if(matching_lls_sls_mmt_monitor->audio_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
                    if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change(mmtp_mpu_packet->mmtp_packet_id, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number, mmtp_mpu_packet->mpu_sequence_number);
                        mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number);
                        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
                        //keep track of our last mpu_sequence_number...
                    }
                    udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio, last_flow_reference);
                } else if(matching_lls_sls_mmt_monitor->stpp_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
                    if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_stpp && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_stpp->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change(mmtp_mpu_packet->mmtp_packet_id, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_stpp->mpu_sequence_number, mmtp_mpu_packet->mpu_sequence_number);
                        mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_stpp->mpu_sequence_number);
                        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
                        //keep track of our last mpu_sequence_number...
                    }
                    udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_stpp, last_flow_reference);
                }
            }
        } else {
            //non-timed
            atsc3_global_statistics->packet_counter_mmt_nontimed_mpu++;
        }

        goto ret;

    } else if(mmtp_mpu_packet->mmtp_payload_type == 0x2) {

		atsc3_global_statistics->packet_counter_mmt_signaling++;
		__MMT_CONTEXT_MPU_SIGNAL_INFO("mmtp_mfu_process_from_payload_with_context: processing mmt flow: %d.%d.%d.%d:(%u) packet_id: 0, signalling message", __toipandportnonstruct(udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port));

    } else {
    	__MMT_CONTEXT_MPU_WARN("mmtp_mfu_process_from_payload_with_context: unknown payload type of 0x%x", mmtp_mpu_packet->mmtp_payload_type);
		atsc3_global_statistics->packet_counter_mmt_unknown++;
		goto packet_cleanup;
    }

packet_cleanup:
	//jjustman-2019-10-19: todo -- impl

ret:
    return;
}



/*
 * jjustman-2019-10-03: todo: filter by
 * 	ignore atsc3_mmt_mfu_context->lls_slt_monitor (or mmtp_flow, matching_lls_sls_mmt_session)
 *
 * 	NOTE: if adding a new fourcc asset_type for context callbacks, make sure it is also added
 * 		   in atsc3_mmt_signalling_message.c/mmt_signalling_message_update_lls_sls_mmt_session
 *
 *
 */

void mmt_signalling_message_process_with_context(udp_packet_t *udp_packet,
												 mmtp_signalling_packet_t* mmtp_signalling_packet,
												 atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context) {
	atsc3_mmt_mfu_context->udp_flow = &udp_packet->udp_flow;

	for(int i=0; i < mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.count; i++) {
		mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.data[i];

		//MPT_message, mp_table, dispatch either complete or subset of MPT_message table
		if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MPT_message) {
			mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;

			//dispatched when message_id >= 0x11 (17) && message_id <= 0x19 (31)
			if(mmt_signalling_message_header_and_payload->message_header.message_id >= MPT_message_start && mmt_signalling_message_header_and_payload->message_header.message_id < MPT_message_end) {
				__MMSM_TRACE("mmt_signalling_message_process_with_context: partial mp_table, message_id: 0x%02x", mmt_signalling_message_header_and_payload->message_header.message_id);
				atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_subset(mp_table);

			} else if(mmt_signalling_message_header_and_payload->message_header.message_id == MPT_message_end) {
				__MMSM_TRACE("mmt_signalling_message_process_with_context: complete mp_table, message_id: 0x%02x", mmt_signalling_message_header_and_payload->message_header.message_id);
				atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_complete(mp_table);

			} else {
				__MMSM_ERROR("mmt_signalling_message_process_with_context: MESSAGE_id_type == MPT_message but message_id not in bounds: 0x%02x", mmt_signalling_message_header_and_payload->message_header.message_id);
			}

			if(mp_table->number_of_assets) {
				for(int i=0; i < mp_table->number_of_assets; i++) {
					//slight hack, check the asset types and default_asset = 1
					mp_table_asset_row_t* mp_table_asset_row = &mp_table->mp_table_asset_row[i];

					uint32_t* mpu_sequence_number_p 			 = NULL;
					uint64_t* mpu_presentation_time_ntp64_p 	 = NULL;
					uint32_t  mpu_presentation_time_seconds 	 = 0;
					uint32_t  mpu_presentation_time_microseconds = 0;

					//try and extract mpu_presentation_timestamp first
					if(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor && mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n) {
						__MMSM_DEBUG("MPT message: checking packet_id: %u, mmt_signalling_message_mpu_timestamp_descriptor count is: %u",
								mp_table_asset_row->mmt_general_location_info.packet_id,
								mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n);

						for(int l=0; l < mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n; l++) {
							mmt_signalling_message_mpu_tuple_t* mmt_signaling_message_mpu_tuple = &mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[l];

							mpu_sequence_number_p = &mmt_signaling_message_mpu_tuple->mpu_sequence_number;
							mpu_presentation_time_ntp64_p = &mmt_signaling_message_mpu_tuple->mpu_presentation_time;

							compute_ntp64_to_seconds_microseconds(mmt_signaling_message_mpu_tuple->mpu_presentation_time, &mpu_presentation_time_seconds, &mpu_presentation_time_microseconds);

							__MMSM_DEBUG("packet_id: %u, mpu_sequence_number: %u to mpu_presentation_time: %llu, seconds: %u, ms: %u",
									mp_table_asset_row->mmt_general_location_info.packet_id,
									mmt_signaling_message_mpu_tuple->mpu_sequence_number,
									mmt_signaling_message_mpu_tuple->mpu_presentation_time,
									mpu_presentation_time_seconds,
									mpu_presentation_time_microseconds);
						}
					}

					//resolve packet_id's to matching essence types
					__MMSM_DEBUG("MPT message: checking packet_id: %u, asset_type: %s, default: %u, identifier: %s", mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					//mp_table_asset_row->asset_type == HEVC
					if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID, mp_table_asset_row->asset_type, 4) == 0) {
						atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_essence_packet_id(mp_table_asset_row->mmt_general_location_info.packet_id);
						if(mpu_sequence_number_p && mpu_presentation_time_ntp64_p) {
							atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor(mp_table_asset_row->mmt_general_location_info.packet_id,
									*mpu_sequence_number_p,
									*mpu_presentation_time_ntp64_p,
									mpu_presentation_time_seconds,
									mpu_presentation_time_microseconds);
						}
						__MMSM_DEBUG("MPT message: mmtp_flow: %p, setting video_packet_id: packet_id: %u, asset_type: %s, default: %u, identifier: %s",
								atsc3_mmt_mfu_context->mmtp_flow,
								mp_table_asset_row->mmt_general_location_info.packet_id,
								mp_table_asset_row->asset_type,
								mp_table_asset_row->default_asset_flag,
								mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					} else if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MP4A_ID, mp_table_asset_row->asset_type, 4) == 0 || strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_AC_4_ID, mp_table_asset_row->asset_type, 4) == 0) {
						//mp_table_asset_row->asset_type ==  MP4A || AC-4
						atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_essence_packet_id(mp_table_asset_row->mmt_general_location_info.packet_id);
						if(mpu_sequence_number_p && mpu_presentation_time_ntp64_p) {
							atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor(mp_table_asset_row->mmt_general_location_info.packet_id,
									*mpu_sequence_number_p,
									*mpu_presentation_time_ntp64_p,
									mpu_presentation_time_seconds,
									mpu_presentation_time_microseconds);
						}
						//matching_lls_sls_mmt_session->audio_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
						__MMSM_DEBUG("MPT message: mmtp_flow: %p, setting audio_packet_id: packet_id: %u, asset_type: %s, default: %u, identifier: %s",
								atsc3_mmt_mfu_context->mmtp_flow,
								mp_table_asset_row->mmt_general_location_info.packet_id,
								mp_table_asset_row->asset_type,
								mp_table_asset_row->default_asset_flag,
								mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					} else if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_IMSC1_ID, mp_table_asset_row->asset_type, 4) == 0) {
						atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_essence_packet_id(mp_table_asset_row->mmt_general_location_info.packet_id);
						if(mpu_sequence_number_p && mpu_presentation_time_ntp64_p) {
							atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor(mp_table_asset_row->mmt_general_location_info.packet_id,
									*mpu_sequence_number_p,
									*mpu_presentation_time_ntp64_p,
									mpu_presentation_time_seconds,
									mpu_presentation_time_microseconds);
						}
						//matching_lls_sls_mmt_session->audio_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
						__MMSM_DEBUG("MPT message: mmtp_flow: %p, setting stpp_packet_id: packet_id: %u, asset_type: %s, default: %u, identifier: %s",
								atsc3_mmt_mfu_context->mmtp_flow,
								mp_table_asset_row->mmt_general_location_info.packet_id,
								mp_table_asset_row->asset_type,
								mp_table_asset_row->default_asset_flag,
								mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					}
				}
			}
		} else {
			__MMSM_INFO("mmt_signalling_message_update_lls_sls_mmt_session: Ignoring signal: 0x%x", mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type);
		}
	}

}


