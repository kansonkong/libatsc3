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

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window, atsc3_mmt_mfu_mpu_timestamp_descriptor);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_mmt_mfu_mpu_timestamp_descriptor);

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



void atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_noop(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample,  uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_noop: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, mfu_fragment_count_expected: %u, mfu_fragment_count_rebuilt: %u",
    packet_id,
    mpu_sequence_number,
    sample_number,
    mfu_fragment_count_expected,
    mfu_fragment_count_rebuilt);
}

//MPU - init box (mpu_metadata) present
void atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop(uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_movie_fragment_metadata) {
    //as a last resort if mmt_atsc3_message isn't present with v/a descriptor timing, persist at least one trun frame duration...
    uint32_t sample_duration = atsc3_mmt_movie_fragment_extract_sample_duration(mmt_movie_fragment_metadata);

    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop: packet_id: %u, mpu_sequence_number: %u, mmt_movie_fragment_metadata: %p, size: %d, extracted sample_duration: %d",
                            packet_id,
                            mpu_sequence_number,
                            mmt_movie_fragment_metadata,
                            mmt_movie_fragment_metadata->p_size,
                            sample_duration);
}

void __internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
	while(atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count > 10) {
        atsc3_mmt_mfu_mpu_timestamp_descriptor_t *atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window_pop_atsc3_mmt_mfu_mpu_timestamp_descriptor(&atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window);

        __MMT_CONTEXT_MPU_TRACE("__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor, removing: packet_id: %d, mpu_sequence_number: %d", atsc3_mmt_mfu_mpu_timestamp_descriptor->packet_id, atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_sequence_number);

        atsc3_mmt_mfu_mpu_timestamp_descriptor_free(&atsc3_mmt_mfu_mpu_timestamp_descriptor);
    }

    __MMT_CONTEXT_MPU_TRACE("__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor, window count: %d, adding: packet_id: %d, mpu_sequence_number: %d", atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count, packet_id, mpu_sequence_number);

    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* matching_atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context, packet_id, mpu_sequence_number);

	if(!matching_atsc3_mmt_mfu_mpu_timestamp_descriptor) {
        atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_mpu_timestamp_descriptor_new();
        atsc3_mmt_mfu_mpu_timestamp_descriptor->packet_id = packet_id;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_sequence_number = mpu_sequence_number;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_microseconds = mpu_presentation_time_microseconds;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_ntp64 = mpu_presentation_time_ntp64;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_seconds = mpu_presentation_time_seconds;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_microseconds = mpu_presentation_time_microseconds;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value = mpu_presentation_time_seconds * 1000000 + mpu_presentation_time_microseconds;

        atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window_add_atsc3_mmt_mfu_mpu_timestamp_descriptor(&atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window, atsc3_mmt_mfu_mpu_timestamp_descriptor);
	}
}


atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number) {
//    __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number: entries: %d, looking for packet_id: %d, mpu_sequence_number: %d",
//                            atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count,
//                            packet_id,
//                            mpu_sequence_number);

    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor_max = NULL;

    for(int i=0; i < atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count; i++) {
        atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.data[i];
        if(atsc3_mmt_mfu_mpu_timestamp_descriptor->packet_id == packet_id && atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_sequence_number == mpu_sequence_number) {
//                __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number: retuning matching: %lu, looking for packet_id: %d, mpu_sequence_number: %d",
//                                        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value,
//                                        packet_id,
//                                        mpu_sequence_number);
                return atsc3_mmt_mfu_mpu_timestamp_descriptor;
        }
    }

    return NULL;
}


atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_last_failsafe(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number) {
    __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_last_failsafe: entries: %d, looking for packet_id: %d, mpu_sequence_number: %d",
						   atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count,
						   packet_id,
						   mpu_sequence_number);

    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor_max = NULL;

	for(int i=0; i < atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count; i++) {
		atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.data[i];
		if(atsc3_mmt_mfu_mpu_timestamp_descriptor->packet_id == packet_id) {
            atsc3_mmt_mfu_mpu_timestamp_descriptor_max = atsc3_mmt_mfu_mpu_timestamp_descriptor;
		    if(atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_sequence_number == mpu_sequence_number) {
                __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_last_failsafe: retuning matching: %" PRIu64 " , looking for packet_id: %d, mpu_sequence_number: %d",
                                        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value,
                                        packet_id,
                                        mpu_sequence_number);
                return atsc3_mmt_mfu_mpu_timestamp_descriptor;
            }
		}
	}

	if(atsc3_mmt_mfu_mpu_timestamp_descriptor_max != NULL) {
//	    //TODO: hack because we lost the MPT clock reference...
        //atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_presentation_time_as_us_value;// += 1000000;
        __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number: retuning fallback max: %" PRIu64 ", looking for packet_id: %d, mpu_sequence_number: %d",
                                atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_presentation_time_as_us_value,
                                packet_id,
                                mpu_sequence_number);
        return atsc3_mmt_mfu_mpu_timestamp_descriptor_max;

    }
//	}

	return NULL;
}



atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_new() {
	atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = calloc(1, sizeof(atsc3_mmt_mfu_context_t));

	//internal related callbacks
	atsc3_mmt_mfu_context->__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor = &__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor;

	//helper methods
	atsc3_mmt_mfu_context->get_mpu_timestamp_from_packet_id_mpu_sequence_number         = atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number; //&atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_last_failsafe;

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
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header = &atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_noop;

    //movie fragment related callbacks, as a last resort
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present = &atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop;

	return atsc3_mmt_mfu_context;
}

void atsc3_mmt_mfu_context_free(atsc3_mmt_mfu_context_t** atsc3_mmt_mfu_context_p) {
    if(atsc3_mmt_mfu_context_p) {
        atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = *atsc3_mmt_mfu_context_p;
        if(atsc3_mmt_mfu_context) {
            if(atsc3_mmt_mfu_context->udp_flow) {
                free(atsc3_mmt_mfu_context->udp_flow);
                atsc3_mmt_mfu_context->udp_flow = NULL;
            }

            if(atsc3_mmt_mfu_context->mmtp_flow) {
                mmtp_flow_free_mmtp_asset_flow(atsc3_mmt_mfu_context->mmtp_flow);
                free(atsc3_mmt_mfu_context->mmtp_flow);
                atsc3_mmt_mfu_context->mmtp_flow = NULL;
            }

            if(atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container) {
                udp_flow_latest_mpu_sequence_number_container_t_release(atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container);
                atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container = NULL;
            }

            //jjustman-2020-08-31 - todo: check to confirm these aren't shared pointers...
            if(atsc3_mmt_mfu_context->lls_slt_monitor) {
                atsc3_lls_slt_monitor_free(&atsc3_mmt_mfu_context->lls_slt_monitor);
            }

            if(atsc3_mmt_mfu_context->matching_lls_sls_mmt_session) {
                lls_sls_mmt_session_flows_free(&atsc3_mmt_mfu_context->matching_lls_sls_mmt_session);
            }

            if(atsc3_mmt_mfu_context->mp_table_last) {
                //jjustman-2020-08-31: todo - free inner impl
                free(atsc3_mmt_mfu_context->mp_table_last);
                atsc3_mmt_mfu_context->mp_table_last = NULL;
            }

            free(atsc3_mmt_mfu_context);
            atsc3_mmt_mfu_context = NULL;
        }
        *atsc3_mmt_mfu_context_p = NULL;
    }
}


void mmtp_mfu_process_from_payload_with_context(udp_packet_t *udp_packet,
                                                mmtp_mpu_packet_t* mmtp_mpu_packet,
                                                atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context) {


    if(mmtp_mpu_packet->mmtp_payload_type != 0x0) {
        __MMT_CONTEXT_MPU_WARN("mmtp_mfu_process_from_payload_with_context: got incorrect payload type of: %d for flow: %d:%d, packet_id: %d, psn: %d", mmtp_mpu_packet->mmtp_payload_type, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port, mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->packet_sequence_number);
        atsc3_global_statistics->packet_counter_mmt_unknown++;
        return;
    }
    
    if(mmtp_mpu_packet->mpu_timed_flag != 0x1) {
        __MMT_CONTEXT_MPU_WARN("mmtp_mfu_process_from_payload_with_context: got incorrect mpu_timed_flag type of: %d for flow: %d:%d, packet_id: %d, mpu_sequence_number: %d, psn: %d,", mmtp_mpu_packet->mpu_timed_flag, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port, mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->packet_sequence_number);
        atsc3_global_statistics->packet_counter_mmt_nontimed_mpu++;
        return;
    }
    
    atsc3_global_statistics->packet_counter_mmt_mpu++;
    atsc3_global_statistics->packet_counter_mmt_timed_mpu++;
    
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

        return;
    }
    
    if(!(matching_lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id == matching_lls_sls_mmt_session->service_id &&
           matching_lls_sls_mmt_session->sls_destination_ip_address == udp_packet->udp_flow.dst_ip_addr &&
           matching_lls_sls_mmt_session->sls_destination_udp_port == udp_packet->udp_flow.dst_port)) {
               __MMT_CONTEXT_MPU_TRACE("mmtp_mfu_process_from_payload_with_context: sls monitor flow: %d:%d, service_id: %d not matching for flow: %d:%d, service_id: %d, packet_id: %d, mpu_sequence_number: %d, psn: %d",
                               matching_lls_sls_mmt_session->sls_destination_ip_address, matching_lls_sls_mmt_session->sls_destination_udp_port, matching_lls_sls_mmt_monitor->atsc3_lls_slt_service->service_id,
                               udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port, matching_lls_sls_mmt_session->service_id, mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->packet_sequence_number);
         return;
    }
    
    udp_flow_packet_id_mpu_sequence_tuple_t* last_flow_reference = udp_flow_latest_mpu_sequence_number_add_or_replace_and_check_for_rollover(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_mpu_packet, lls_slt_monitor, matching_lls_sls_mmt_session, mmtp_flow);

    //assign our mmtp_mpu_packet to asset/packet_id/mpu_sequence_number flow
    mmtp_asset_flow = mmtp_flow_find_or_create_from_udp_packet(mmtp_flow, udp_packet);
    mmtp_asset = mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow, matching_lls_sls_mmt_session);
    mmtp_packet_id_packets_container = mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset, mmtp_mpu_packet);
    mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_or_create_mpu_sequence_number_mmtp_mpu_packet_collection_from_mmt_mpu_packet(mmtp_packet_id_packets_container, mmtp_mpu_packet);

    //persist our mmtp_mpu_packet for mpu reconstitution as per original libatsc3 design
    mpu_sequence_number_mmtp_mpu_packet_collection_add_mmtp_mpu_packet(mpu_sequence_number_mmtp_mpu_packet_collection, mmtp_mpu_packet);

    //check for rollover for any remaining emissions from our last mpu_sequence tuple, and then current mpu_sequence rebuild
    if(matching_lls_sls_mmt_monitor->video_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
        if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
            mpu_sequence_number_mmtp_mpu_packet_collection_t* last_mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number);
            //rebuild any straggler DU's
            mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context, mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number, 0, true);
            //notify context of the sequence number channge
            atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change(mmtp_mpu_packet->mmtp_packet_id, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number, mmtp_mpu_packet->mpu_sequence_number);
            //remove last mpu_sequence_number reference from our packets conntainenr
            mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection_non_vector_builder(mmtp_packet_id_packets_container, last_mpu_sequence_number_mmtp_mpu_packet_collection);
        }
        udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video, last_flow_reference);

    } else if(matching_lls_sls_mmt_monitor->audio_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
        if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
            mpu_sequence_number_mmtp_mpu_packet_collection_t* last_mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number);
            mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context, mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number, 0, true);
            atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change(mmtp_mpu_packet->mmtp_packet_id, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number, mmtp_mpu_packet->mpu_sequence_number);
            mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection_non_vector_builder(mmtp_packet_id_packets_container, last_mpu_sequence_number_mmtp_mpu_packet_collection);
        }
        udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio, last_flow_reference);
    } else if(matching_lls_sls_mmt_monitor->stpp_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
        if(matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_stpp && matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_stpp->mpu_sequence_number < mmtp_mpu_packet->mpu_sequence_number) {
            mpu_sequence_number_mmtp_mpu_packet_collection_t* last_mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_stpp->mpu_sequence_number);
            mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context, mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_stpp->mpu_sequence_number, 0, true);
            atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change(mmtp_mpu_packet->mmtp_packet_id, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_stpp->mpu_sequence_number, mmtp_mpu_packet->mpu_sequence_number);
            mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection_non_vector_builder(mmtp_packet_id_packets_container, last_mpu_sequence_number_mmtp_mpu_packet_collection);
        }
        udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_stpp, last_flow_reference);
    }

    mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context, mmtp_packet_id_packets_container, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->sample_number, false);

    return;
}

/**
 //jjustman-2019-10-23: do not optimisticly emit mmthsample_header du payload, otehrwise we may lose partial MFU emission flush below...
    if(false && mmtp_mpu_packet->mmthsample_header) {
        __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_process_from_payload_with_context: new MFU.MMTHSample: packet_id: %u, mpu_sequence_number: %u, mmthsample.samplenumber: %u, mmthsample.movie_fragment_sequence_number: %u, mmthsample.length: %u, mfu packet size: %u, fragmentation_indicator: %u",
            mmtp_mpu_packet->mmtp_packet_id,
            mmtp_mpu_packet->mpu_sequence_number,
            mmtp_mpu_packet->mmthsample_header->samplenumber,
            mmtp_mpu_packet->mmthsample_header->movie_fragment_sequence_number,
            mmtp_mpu_packet->mmthsample_header->length,
            mmtp_mpu_packet->du_mfu_block ? mmtp_mpu_packet->du_mfu_block->p_size : 0,
            mmtp_mpu_packet->mpu_fragmentation_indicator);

        //in the case of audio (or video P frame) packets, our du mfu packet size should be equal to the mmthsample_header->length value,
        if(mmtp_mpu_packet->du_mfu_block->p_size == mmtp_mpu_packet->mmthsample_header->length) {
            block_t* du_mfu_block_duplicated_for_context_callback_invocation = block_Duplicate(mmtp_mpu_packet->du_mfu_block);
            atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->mmthsample_header->samplenumber, du_mfu_block_duplicated_for_context_callback_invocation, 1);
            block_Destroy(&du_mfu_block_duplicated_for_context_callback_invocation);
        } else if(mmtp_mpu_packet->mpu_fragmentation_indicator == 0x00) {
            //otherwise, check mpu_fragmentation_indicator...only process if we are marked as fi=0x00 (complete data unit)
            //let DU rebuild handle any other packets
            __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_process_from_payload_with_context: mmthsample mismatch with du_mfu_block and frag_indicator==0x00, mpu_fragmentation_indicator == 0x00, but du_mfu_block.size (%u) != mmthsample_header->length (%u), packet_id: %u, mpu_sequence_number: %u, mmthsample.samplenumber: %u, mmthsample.movie_fragment_sequence_number: %u, mmthsample.length: %u, mfu packet size: %u, fragmentation_indicator: %u",
                mmtp_mpu_packet->du_mfu_block->p_size,
                mmtp_mpu_packet->mmthsample_header->length,
                mmtp_mpu_packet->mmtp_packet_id,
                mmtp_mpu_packet->mpu_sequence_number,
                mmtp_mpu_packet->mmthsample_header->samplenumber,
                mmtp_mpu_packet->mmthsample_header->movie_fragment_sequence_number,
                mmtp_mpu_packet->mmthsample_header->length,
                mmtp_mpu_packet->du_mfu_block ? mmtp_mpu_packet->du_mfu_block->p_size : 0,
                mmtp_mpu_packet->mpu_fragmentation_indicator);
            
            block_t* du_mfu_block_duplicated_for_context_callback_invocation = block_Duplicate(mmtp_mpu_packet->du_mfu_block);
            atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->mmthsample_header->samplenumber, du_mfu_block_duplicated_for_context_callback_invocation, 1);
            block_Destroy(&du_mfu_block_duplicated_for_context_callback_invocation);
        }

    } else {
 */

/*
 mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number
 
    mfu_sample_number_from_current_du may be 0 if:
    flush_all_fragments: set to true when mpu_sequence_number rolls over, otherwise set to false
 

     we only care about:
        mfu's that start with a sample header (and fi=0): mmtp_mpu_packet->mmthsample_header
     
        otherwise, check and recon if necessary:
            mmtp_mpu_packet->mpu_fragment_type=0x0 - mpu_metadata
            mmtp_mpu_packet->mpu_fragment_type=0x2 - mfu
 
 //todo: jjustman-2019-10-03 - how to handle missing first NAL or partial DU's?

*/
void mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container, uint32_t mpu_sequence_number, uint32_t mfu_sample_number_from_current_du, bool flush_all_fragments) {

    block_t* du_mpu_metadata_block_rebuilt = NULL;
    
    int du_mfu_block_rebuild_index_start = -1;
    int32_t mfu_fragment_counter_mmthsample_header_start = 0;
    int32_t mfu_fragment_counter_missing_mmthsample_header_start = 0;
    int32_t mfu_fragment_counter_position = 0;
    int32_t mfu_fragment_count_rebuilt = 0;

    mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, mpu_sequence_number);
    for(int i=0; i < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count; i++) {
        mmtp_mpu_packet_t *mmtp_mpu_init_packet_to_rebuild = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[i];

        if (mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed) {
            continue;
        }

        //process mpu_metadata, don't send incomplete payloads for init box (e.g. isnt FI==0x00 or endns in 0x03)
        if (mmtp_mpu_init_packet_to_rebuild->mpu_fragment_type == 0x0) {
            //mark this DU as completed for purging at the end of this method

            if (!mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block) {
                __MMT_CONTEXT_MPU_WARN(
                        "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number MPU Metadata, missing du_mpu_metadata_block i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                        i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                        atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                        atsc3_mmt_mfu_context->udp_flow->dst_port,
                        mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                        mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                        mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                continue;
            }

            __MMT_CONTEXT_MPU_DEBUG(
                    "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: MPU Metadata: init box: packet_id: %u, mpu_sequence_number: %u, mmtp_mpu_packet.samplenumber: %u, mmtp_mpu_packet.mpu_fragment_counter: %u, mmtp_mpu_packet.offset: %u, du_mpu_metadata_block packet size: %u, fragmentation_indicator: %u",
                    mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                    mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                    mmtp_mpu_init_packet_to_rebuild->sample_number,
                    mmtp_mpu_init_packet_to_rebuild->mpu_fragment_counter,
                    mmtp_mpu_init_packet_to_rebuild->offset,
                    mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block ? mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block->p_size : 0,
                    mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);

            //one (or more) DU
            if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x00) {
                mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed = true;

                block_t *du_mpu_metadata_block_duplicated_for_context_callback_invocation = block_Duplicate(
                        mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block);
                atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present(
                        mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                        mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                        du_mpu_metadata_block_duplicated_for_context_callback_invocation);
                block_Destroy(&du_mpu_metadata_block_duplicated_for_context_callback_invocation);
            } else if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x03) {
                if (du_mpu_metadata_block_rebuilt != NULL &&
                    mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block) {
                    mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed = true;

                    __MMT_CONTEXT_MPU_DEBUG(
                            "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: Appending MPU Metadata, i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                            i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                            atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                            atsc3_mmt_mfu_context->udp_flow->dst_port,
                            mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                            mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                            mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);

                    block_Merge(du_mpu_metadata_block_rebuilt,
                                mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block);
                    block_Rewind(du_mpu_metadata_block_rebuilt);
                    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present(
                            mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                            mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                            du_mpu_metadata_block_rebuilt);
                    block_Destroy(&du_mpu_metadata_block_rebuilt);
                } else {
                    __MMT_CONTEXT_MPU_WARN(
                            "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number Missing proceeding MPU Metadata i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                            i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                            atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                            atsc3_mmt_mfu_context->udp_flow->dst_port,
                            mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                            mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                            mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                }
            } else {
                if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x01) {
                    if (du_mpu_metadata_block_rebuilt) {
                        block_Destroy(&du_mpu_metadata_block_rebuilt);
                    }
                    du_mpu_metadata_block_rebuilt = block_Duplicate(
                            mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block);
                } else if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x02) {
                    if (du_mpu_metadata_block_rebuilt) {
                        block_Merge(du_mpu_metadata_block_rebuilt,
                                    mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block);
                    } else {
                        __MMT_CONTEXT_MPU_WARN(
                                "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number Missing initial MPU Metadata i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                                atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                atsc3_mmt_mfu_context->udp_flow->dst_port,
                                mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                                mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                                mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                    }
                }
            }
        }
    }


    //make sure our DU for this is set to i_pos == p_size, so we can opportunisticly alloc and make sure block_AppendFull works properly
    //jjustman-2019-10-24: todo - fix me! mmtp_mpu_packet_to_rebuild->du_mfu_block->i_pos = mmtp_mpu_packet_to_rebuild->du_mfu_block->p_size;

    //regardless of mpu_fragmentation_indicator== as it may be lost in emission... and compute relative DU offset for rebuilding MFU
    //first DU in MFU should contain MMTHSample, but may not if its a lost DU
    //last DU in MFU should contain mpu_fragment_counter == 0, , but may not if its a lost DU


    for(int i=0; i < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count; i++) {
        mmtp_mpu_packet_t *mmtp_mpu_packet_to_rebuild = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[i];

        if (mmtp_mpu_packet_to_rebuild->mfu_reassembly_performed) {
            continue;
        }

        //only rebuild MPU DU's here
        if(mmtp_mpu_packet_to_rebuild->mpu_fragment_type == 0x2) {

            //MFU rebuild any pending packets less than our sample_number,
            if (!mmtp_mpu_packet_to_rebuild->du_mfu_block) {
                __MMT_CONTEXT_MPU_WARN("MFU: missing du_mfu_block! creating dummy 0 byte block: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                                                i, mmtp_mpu_packet_to_rebuild->packet_sequence_number,
                                                                atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                                                atsc3_mmt_mfu_context->udp_flow->dst_port,
                                                                mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                                                mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                                                mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);
                mmtp_mpu_packet_to_rebuild->du_mfu_block = block_Alloc(0);
            }


            if(mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator == 0x00) {
                //otherwise, check mpu_fragmentation_indicator...only process if we are marked as fi=0x00 (complete data unit)
                //let DU rebuild handle any other packets

//                __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: mpu_fragmentation_indicator==0x00,  du_mfu_block.size: %u, mmthsample_header->length: %u, packet_id: %u, mpu_sequence_number: %u, mmthsample.samplenumber: %u, mmthsample.movie_fragment_sequence_number: %u, mmthsample.length: %u, fragmentation_indicator: %u",
//                                        mmtp_mpu_packet_to_rebuild->du_mfu_block ? mmtp_mpu_packet_to_rebuild->du_mfu_block->p_size : 0,
//                                        mmtp_mpu_packet_to_rebuild->mmthsample_header ? mmtp_mpu_packet_to_rebuild->mmthsample_header->length : 0,
//                                        mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
//                                        mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
//                                        mmtp_mpu_packet_to_rebuild->mmthsample_header->samplenumber,
//                                        mmtp_mpu_packet_to_rebuild->mmthsample_header->movie_fragment_sequence_number,
//                                        mmtp_mpu_packet_to_rebuild->mmthsample_header->length,
//                                        mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);

                block_Rewind(mmtp_mpu_packet_to_rebuild->du_mfu_block);
                block_t* du_mfu_block_duplicated_for_context_callback_invocation = block_Duplicate(mmtp_mpu_packet_to_rebuild->du_mfu_block);
                atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, du_mfu_block_duplicated_for_context_callback_invocation, 1);
                mmtp_mpu_packet_to_rebuild->mfu_reassembly_performed = true;
                block_Destroy(&du_mfu_block_duplicated_for_context_callback_invocation);

                continue;
            }

            int mmtp_mpu_starting_index = i;
            int mmtp_mpu_ending_index = -1;

            //todo? int block_alloc_size = 0;

            //otherwise, walk thru a consecutive set of mfu samples to rebuild
            for(int j=mmtp_mpu_starting_index; j < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count && mmtp_mpu_ending_index == -1; j++) {
                mmtp_mpu_packet_t *mmtp_mpu_packet_to_rebuild_next = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[j];
                if (mmtp_mpu_packet_to_rebuild->sample_number != mmtp_mpu_packet_to_rebuild_next->sample_number) {
                    mmtp_mpu_ending_index = j;
                }
            }
            //handle force flush, <-1
            if(mmtp_mpu_ending_index == -1 && flush_all_fragments) {
                mmtp_mpu_ending_index = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count;
            } else if(mmtp_mpu_ending_index == -1) {
                continue;
            }
            //todo - compute gap here between sample_numbers for tracking our MFU loss


            if(mmtp_mpu_ending_index == -1) {
                //exit from loop
                __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: exiting loop from mmtp_mpu_starting_index: %d, mmtp_mpu_ending_index: %d, count: %d, flush_all_fragments: %d, packet_id: %u, mpu_sequence_number: %u",
                                        mmtp_mpu_starting_index, mmtp_mpu_ending_index,
                                        mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count,
                                        mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                        mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                        flush_all_fragments);
                i = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count;
                continue;
            } else {
                __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: iterating from mmtp_mpu_starting_index: %d, mmtp_mpu_ending_index: %d, count: %d, flush_all_fragments: %d, packet_id: %u, mpu_sequence_number: %u",
                                        mmtp_mpu_starting_index, mmtp_mpu_ending_index,
                                        mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count,
                                        mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                        mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                        flush_all_fragments);
            }

            //rebuild here as we have rolled over to the next sample number
            block_t* du_mfu_block_to_rebuild = NULL;
            mfu_fragment_counter_mmthsample_header_start = 0;
            mfu_fragment_counter_missing_mmthsample_header_start = 0;
            mfu_fragment_count_rebuilt = 0;
            uint32_t mfu_mmth_sample_header_size_to_shift_offset = 0;

            for(int j=mmtp_mpu_starting_index; j < mmtp_mpu_ending_index; j++) {
                mmtp_mpu_packet_t *mmtp_mpu_packet_to_rebuild_from_du = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[j];
                mmtp_mpu_packet_to_rebuild_from_du->mfu_reassembly_performed = true;
                mfu_fragment_count_rebuilt++;

//                __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number:\trebuilding from packet_id: %u, mpu_sequence_number: %u, sample_number: %d, fragment: %d, du_mfu_size: %d, offset: %d",
//                        mmtp_mpu_packet_to_rebuild_from_du->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_from_du->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_from_du->sample_number, mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block->p_size, mmtp_mpu_packet_to_rebuild_from_du->offset);

                if(mmtp_mpu_packet_to_rebuild_from_du->mpu_fragmentation_indicator == 0x00 || mmtp_mpu_packet_to_rebuild_from_du->mpu_fragmentation_indicator == 0x01) {
                    mfu_fragment_counter_position = mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter;

                    if(mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header && mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header->length) {
                        mfu_fragment_counter_mmthsample_header_start = mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter;

                        mfu_mmth_sample_header_size_to_shift_offset = mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header->mfu_mmth_sample_header_size;

                        //use the original mmthsample_length, only shift the offset of the non-prefixed fragments
                        du_mfu_block_to_rebuild = block_Alloc(mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header->length); //- mfu_mmth_sample_header_size_to_shift_offset);
//                        __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number:\tblock_alloc\tto\tsize:\t%d\t(mmthsample_header->length),\tmfu_mmth_sample_header_size,\tfrom\tpacket_id:\t%u,\tmpu_sequence_number:\t%u,\tsample_number:\t%d,\tfragment:\t%d,\tdu_mfu_size:\t%d",
//                                mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header->length, mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header->mfu_mmth_sample_header_size, mmtp_mpu_packet_to_rebuild_from_du->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_from_du->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_from_du->sample_number, mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block->p_size);

                        block_AppendFull(du_mfu_block_to_rebuild, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block);
                    } else {
                        du_mfu_block_to_rebuild = block_Duplicate(mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block);
                        mfu_fragment_counter_missing_mmthsample_header_start = mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter;
                    }
                } else {
                    if(!du_mfu_block_to_rebuild) {
                        du_mfu_block_to_rebuild = block_Duplicate(mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block);
                        mfu_fragment_counter_missing_mmthsample_header_start = mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter;

                    } else {

                        //jjustman-2019-10-25 - we can't just append with a pre-allocated block from our mmthsample_header, we will need to offset our proper positions when rebuilding
                        //or develop a more robust missing DU strategy for error concealment
                        if(block_IsAlloc(du_mfu_block_to_rebuild)) {
//                            __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number:\tblock_AppendFull fragment with IsAlloc, offset: %u, mmth_sample_to_shift: %u, packet_id: %u, mpu_sequence_number: %u, sample_number: %d, block_AppendFull with du_mfu_block_to_rebuild: %p, pos: %d, size: %d, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block: %p, du_mfu_pos: %d, du_mfu_size: %d, fragment: %d",
//                                                    mmtp_mpu_packet_to_rebuild_from_du->offset, mfu_mmth_sample_header_size_to_shift_offset, mmtp_mpu_packet_to_rebuild_from_du->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_from_du->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_from_du->sample_number, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild->i_pos, du_mfu_block_to_rebuild->p_size, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block,  mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block->i_pos, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block->p_size, mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter);

                            //testing, hevc doesn't like large null blocks when its expecting nals...
                            //block_Seek(du_mfu_block_to_rebuild, mmtp_mpu_packet_to_rebuild_from_du->offset - mfu_mmth_sample_header_size_to_shift_offset);
                            //block_Write(du_mfu_block_to_rebuild, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block->p_buffer, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block->p_size);
                            //so, append as usual, and then trim the tail of the allocation
                            block_AppendFull(du_mfu_block_to_rebuild, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block);
                        } else {
//                            __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number:\tblock_AppendFull fragment, packet_id: %u, mpu_sequence_number: %u, sample_number: %d, block_AppendFull with du_mfu_block_to_rebuild: %p, pos: %d, size: %d, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block: %p, du_mfu_pos: %d, du_mfu_size: %d, fragment: %d",
//                                                    mmtp_mpu_packet_to_rebuild_from_du->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_from_du->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_from_du->sample_number, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild->i_pos, du_mfu_block_to_rebuild->p_size, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block,  mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block->i_pos, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block->p_size, mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter);

                            block_AppendFull(du_mfu_block_to_rebuild, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block);
                        }

//                        __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number:\tafter block_appendFull: packet_id: %u, mpu_sequence_number: %u, sample_number: %d, offset: %d, du_mfu_block_to_rebuild: %p, pos: %d, size: %d",
//                                                mmtp_mpu_packet_to_rebuild_from_du->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_from_du->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_from_du->sample_number, mmtp_mpu_packet_to_rebuild_from_du->offset, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild->i_pos, du_mfu_block_to_rebuild->p_size);

                    }
                }
            }

            __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number before callbacks for: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, fragment_counter: %u, psn: %u, du_mfu_block_to_rebuild: %p, pos: %d, p_size: %u",
                    mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->i_pos : -1, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->p_size : -1);

            //todo: impl meta cases for corrupt/missing samples/packets, etc

            if(du_mfu_block_to_rebuild) {
                //if we were from alloc, trim off the null tail from i_ptr
                if(block_IsAlloc(du_mfu_block_to_rebuild)) {
                    if(du_mfu_block_to_rebuild->i_pos > du_mfu_block_to_rebuild->p_size) {
                        //this is bad
                        assert(false);
                    } else {
                        du_mfu_block_to_rebuild->p_size = du_mfu_block_to_rebuild->i_pos;
                    }
                }
                if (mfu_fragment_counter_mmthsample_header_start) {
                    if (mfu_fragment_counter_mmthsample_header_start - (mfu_fragment_count_rebuilt - 1) == 0) {
                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, du_mfu_block_to_rebuild, mfu_fragment_count_rebuilt);
//                        __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number MFU DU with MMTHSample, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, du_mfu_block_to_rebuild: %p, ->p_size: %u, mfu_fragment_counter_mmthsample_header_start: %d, mfu_fragment_count_rebuilt: %d",
//                                mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->p_size : 0, mfu_fragment_counter_mmthsample_header_start, mfu_fragment_count_rebuilt);
                    } else {
                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt(mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, du_mfu_block_to_rebuild, mfu_fragment_counter_mmthsample_header_start, mfu_fragment_count_rebuilt);
                        __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number MFU DU with corrupt payload, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u,  du_mfu_block_to_rebuild: %p, du_mfu_block_to_rebuild->p_size: %u, mfu_fragment_counter_mmthsample_header_start: %d, mfu_fragment_count_rebuilt: %d", mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->p_size : 0, mfu_fragment_counter_mmthsample_header_start, mfu_fragment_count_rebuilt);
                    }
                } else {
                    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header( mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, du_mfu_block_to_rebuild, mfu_fragment_counter_missing_mmthsample_header_start, mfu_fragment_count_rebuilt);
                    __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number MFU DU with missing mmthsample header, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, du_mfu_block_to_rebuild: %p, du_mfu_block_to_rebuild->p_size: %u", mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->p_size : 0);
                }
//                __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number atsc3 block_destroy MFU DU with MMTHSample, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, du_mfu_block_to_rebuild: %p, du_mfu_block_to_rebuild->p_size: %u", mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->p_size : 0);
                block_Destroy(&du_mfu_block_to_rebuild);
            }

            //iterate our parent loop forward
            i = mmtp_mpu_ending_index;
        }
    }

    //jjustman-2019-10-29 - in the spirit of OOO MFU, process the movie fragment metadata as a last resort to extract the sample duration until mmt_atsc3_message support is functional
    block_t* du_movie_fragment_block_rebuilt;

    for(int i=0; i < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count; i++) {
        mmtp_mpu_packet_t *mmtp_mpu_init_packet_to_rebuild = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[i];

        if (mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed) {
            continue;
        }

        //process movie fragment metadata, don't send incomplete payloads for moof box (e.g. isnt FI==0x00 or endns in 0x03)
        if (mmtp_mpu_init_packet_to_rebuild->mpu_fragment_type == 0x1) {
            //mark this DU as completed for purging at the end of this method

            if (!mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block) {
                __MMT_CONTEXT_MPU_WARN(
                        "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number Movie fragment metadata Metadata, missing du_movie_fragment_block i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                        i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                        atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                        atsc3_mmt_mfu_context->udp_flow->dst_port,
                        mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                        mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                        mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                continue;
            }

            __MMT_CONTEXT_MPU_DEBUG(
                    "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: Movie fragment metadata: moof: packet_id: %u, mpu_sequence_number: %u, mmtp_mpu_packet.samplenumber: %u, mmtp_mpu_packet.mpu_fragment_counter: %u, mmtp_mpu_packet.offset: %u, du_mpu_metadata_block packet size: %u, fragmentation_indicator: %u",
                    mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                    mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                    mmtp_mpu_init_packet_to_rebuild->sample_number,
                    mmtp_mpu_init_packet_to_rebuild->mpu_fragment_counter,
                    mmtp_mpu_init_packet_to_rebuild->offset,
                    mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block ? mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block->p_size : 0,
                    mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);

            //one (or more) DU
            if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x00) {
                mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed = true;

                block_t *du_movie_fragment_block_duplicated_for_context_callback_invocation = block_Duplicate(
                        mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block);
                atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present(
                        mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                        mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                        du_movie_fragment_block_duplicated_for_context_callback_invocation);
                block_Destroy(&du_movie_fragment_block_duplicated_for_context_callback_invocation);
            } else if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x03) {
                if (du_movie_fragment_block_rebuilt != NULL &&
                    mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block) {
                    mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed = true;

                    __MMT_CONTEXT_MPU_DEBUG(
                            "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: Appending movie fragment metadata, i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                            i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                            atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                            atsc3_mmt_mfu_context->udp_flow->dst_port,
                            mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                            mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                            mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);

                    block_Merge(du_movie_fragment_block_rebuilt,
                                mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block);
                    block_Rewind(du_movie_fragment_block_rebuilt);
                    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present(
                            mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                            mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                            du_movie_fragment_block_rebuilt);
                    block_Destroy(&du_movie_fragment_block_rebuilt);
                } else {
                    __MMT_CONTEXT_MPU_WARN(
                            "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number Missing proceeding movie fragment metadata i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                            i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                            atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                            atsc3_mmt_mfu_context->udp_flow->dst_port,
                            mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                            mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                            mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                }
            } else {
                if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x01) {
                    if (du_movie_fragment_block_rebuilt) {
                        block_Destroy(&du_movie_fragment_block_rebuilt);
                    }
                    du_movie_fragment_block_rebuilt = block_Duplicate(
                            mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block);
                } else if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x02) {
                    if (du_movie_fragment_block_rebuilt) {
                        block_Merge(du_movie_fragment_block_rebuilt,
                                    mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block);
                    } else {
                        __MMT_CONTEXT_MPU_WARN(
                                "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number Missing initial movie fragment m]etadata i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                                atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                atsc3_mmt_mfu_context->udp_flow->dst_port,
                                mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                                mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                                mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                    }
                }
            }
        }
    }
}
//
//            //notify any pending mmtp_mpu_packet_t's that haven't been emitted yet (e.g. might be missing 0x03 FI)
//           if (mmtp_mpu_packet_to_rebuild_last && mmtp_mpu_packet_to_rebuild_last->mfu_reassembly_performed == false && mmtp_mpu_packet_to_rebuild_last->sample_number != mmtp_mpu_packet_to_rebuild->sample_number) {
//               //mark packets as rebuilt, should remove from mpu_sequence_number_mmtp_mpu_packet_collection instead...
//               if (du_mfu_block_rebuilt && du_mfu_block_rebuilt->p_size) {
//                   block_Rewind(du_mfu_block_rebuilt);
//
//                   if(du_mfu_block_rebuild_index_start >= 0) {
//                       for(int j=du_mfu_block_rebuild_index_start; j < i; j++) {
//                           mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[j]->mfu_reassembly_performed = true;
//
//                           //hack, handle this context/state better
//                           block_Destroy(&mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[j]->du_mfu_block);
//                       }
//                   }
//
//                   __MMT_CONTEXT_MPU_DEBUG(
//                           "MFU packet_id: %u, mpu_sequence_number: %u, emission: sample_number: %u, fragment_indicator: %u, mfu_fragment_counter_mmthsample_header_start: %d, mpu_fragment_counter_position: %d, mpu_fragment_count_rebuilt: %d",
//                           mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id,
//                           mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number,
//                           mfu_sample_number_from_current_du,
//                           mmtp_mpu_packet_to_rebuild_last->mpu_fragmentation_indicator,
//                           mfu_fragment_counter_mmthsample_header_start,
//                           mfu_fragment_counter_position,
//                           mfu_fragment_count_rebuilt);
//
//
//
//               } else {
//                   __MMT_CONTEXT_MPU_ERROR("ERROR: du is zero! psn: %u, MPU Metadata: du_mpu_metadata_block_rebuilt is null or size == 0! with %u:%u and packet_id: %u, mpu_sequence_number: %u, sample_number: %u, fragment_indicator: %u",
//                           mmtp_mpu_packet_to_rebuild->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number,                            mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);
//
//                   //jjustman-2019-10-23: TODO: compute last->current mfu_sample_number gap...
//                   atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_missing(mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_last->sample_number);
//               }
//
//               mfu_fragment_counter_mmthsample_header_start = 0;
//               mfu_fragment_counter_missing_mmthsample_header_start = 0;
//               mfu_fragment_counter_position = 0;
//               mfu_fragment_count_rebuilt = 0;
//               du_mfu_block_rebuild_index_start = -1;
//               mmtp_mpu_packet_to_rebuild_last = NULL;
//           }
//
//            if (flush_all_fragments || mmtp_mpu_packet_to_rebuild->sample_number < mfu_sample_number_from_current_du) {
//                if(du_mfu_block_rebuild_index_start == -1) {
//                    du_mfu_block_rebuild_index_start = i;
//                }
//
//                if(mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator == 0x01) {
//                    if(du_mfu_block_rebuilt != NULL) {
//                        block_Destroy(&du_mfu_block_rebuilt);
//                        __MMT_CONTEXT_MPU_WARN("MFU fragment_type == 0x01 but du_mfu_block_rebuilt was not null!, clearing for i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
//                                                   i, mmtp_mpu_packet_to_rebuild->packet_sequence_number,
//                                                   atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
//                                                   atsc3_mmt_mfu_context->udp_flow->dst_port,
//                                                   mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
//                                                   mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
//                                                   mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);
//                    }
//
//                    if (mmtp_mpu_packet_to_rebuild->mmthsample_header) {
//                        mfu_fragment_counter_mmthsample_header_start = mmtp_mpu_packet_to_rebuild->mpu_fragment_counter;
//                        mfu_fragment_counter_position = mfu_fragment_counter_mmthsample_header_start;
//
//                        /*
//                         * jjustman-2019-10-12: TODO: use mmthsample.length, pre alloc block_t for MFU rebuild,
//                         * and then make sure to only append the DU at the proper offset + length
//                         */
//                        __MMT_CONTEXT_MPU_DEBUG(
//                                "Found MFU DU mpu_fragment_type=0x01 with MMTHSample, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, mmthsample.length: %u, du_mfu_block->p_sie: %u",
//                                mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
//                                mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
//                                mmtp_mpu_packet_to_rebuild->sample_number,
//                                mmtp_mpu_packet_to_rebuild->mpu_fragment_counter,
//                                mmtp_mpu_packet_to_rebuild->packet_sequence_number,
//                                atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
//                                atsc3_mmt_mfu_context->udp_flow->dst_port,
//                                mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator,
//                                mmtp_mpu_packet_to_rebuild->mmthsample_header->length,
//                                mmtp_mpu_packet_to_rebuild->du_mfu_block->p_size);
//                        //TODO: jjustman-2019-10-23 - opportunisic MFU sizing
//                        //if(mmtp_mpu_packet_to_rebuild->mmthsample_header->length > block_Len(du_mfu_block_rebuilt)) {
//                        //block_Resize(du_mfu_block_rebuilt,  mmtp_mpu_packet_to_rebuild->mmthsample_header->length);
//                        du_mfu_block_rebuilt = block_Duplicate(mmtp_mpu_packet_to_rebuild->du_mfu_block);
//
//                        //du_mfu_block_rebuilt = block_Alloc(mmtp_mpu_packet_to_rebuild->du_mfu_block->p_size);
//                        //block_Rewind(du_mfu_block_rebuilt);
//                        //block_Append(du_mfu_block_rebuilt, mmtp_mpu_packet_to_rebuild->du_mfu_block);
//                    } else {
//                        __MMT_CONTEXT_MPU_WARN(
//                                "i: %u, psn: %u, Found MFU DU mpu_fragment_indicator=0x01 but missing MMTHSample header start?! with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u, du_mfu_block->p_size: %u",
//                                mmtp_mpu_packet_to_rebuild->mpu_fragment_counter,
//                                mmtp_mpu_packet_to_rebuild->packet_sequence_number,
//                                atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
//                                atsc3_mmt_mfu_context->udp_flow->dst_port,
//                                mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
//                                mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
//                                mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator,
//                                mmtp_mpu_packet_to_rebuild->du_mfu_block->p_size
//                                );
//                        //block alloc should be about ~MTU * mpu_fragment_counter bytes
//                        // du_mfu_block_rebuilt = block_Duplicate(mmtp_mpu_packet_to_rebuild->du_mfu_block);
//                        mfu_fragment_counter_missing_mmthsample_header_start = mmtp_mpu_packet_to_rebuild->mpu_fragment_counter;
//                        mfu_fragment_counter_position = mmtp_mpu_packet_to_rebuild->mpu_fragment_counter;
//                        //alloc our mpu offset with null? du_mfu_block_rebuilt
//                        du_mfu_block_rebuilt = block_Duplicate(mmtp_mpu_packet_to_rebuild->du_mfu_block);
//                    }
//                    mfu_fragment_counter_position--;
//                    mfu_fragment_count_rebuilt++;
//
//                } else {
//                    if(du_mfu_block_rebuilt) {
//                        __MMT_CONTEXT_MPU_DEBUG("Appending MFU DU, packet_id: %u, mpu_sequence_number: %u, sample_number: %u, fragment_counter: %u, psn: %u, with %u:%u, mpu_fragment_indicator: %u", mmtp_mpu_packet_to_rebuild->mmtp_packet_id,mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);
//                        mfu_fragment_count_rebuilt++;
//
//                        mfu_fragment_counter_position--;
//                        //block_Append(du_mfu_block_rebuilt, mmtp_mpu_packet_to_rebuild->du_mfu_block);
//                        block_Merge(du_mfu_block_rebuilt, mmtp_mpu_packet_to_rebuild->du_mfu_block);
//                    } else {
//                        __MMT_CONTEXT_MPU_WARN("MFU DU rebuild, missing du_mfu_block_rebuilt, packet_id: %u, mpu_sequence_number: %u, sample_number: %u, fragment_counter: %u, psn: %u, with %u:%u, mpu_fragment_indicator: %u", mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);
//                        du_mfu_block_rebuilt = block_Duplicate(mmtp_mpu_packet_to_rebuild->du_mfu_block);
//                        mfu_fragment_count_rebuilt++;
//                        mfu_fragment_counter_position = mmtp_mpu_packet_to_rebuild->mpu_fragment_counter;
//                    }
//                }
//            }
//
//            mmtp_mpu_packet_to_rebuild_last = mmtp_mpu_packet_to_rebuild;
//        }
//    }
//
//    if(flush_all_fragments && du_mfu_block_rebuilt) {
//        //copy and paste from above...grr
//
//        if (mfu_fragment_counter_mmthsample_header_start) {
//            if (mfu_fragment_counter_mmthsample_header_start - (mfu_fragment_count_rebuilt - 1) == 0) {
//                atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_last->sample_number, du_mfu_block_rebuilt, mfu_fragment_count_rebuilt);
//                __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_complete MFU DU with MMTHSample, flush: packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, du_mfu_block_rebuilt: %p, ->p_size: %u", mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_last->sample_number, mmtp_mpu_packet_to_rebuild_last->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild_last->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild_last->mpu_fragmentation_indicator,  du_mfu_block_rebuilt, du_mfu_block_rebuilt ? du_mfu_block_rebuilt->p_size : 0);
//            } else {
//                atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt(mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_last->sample_number, du_mfu_block_rebuilt, mfu_fragment_counter_mmthsample_header_start, mfu_fragment_count_rebuilt);
//                    __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_corrupt MFU DU with MMTHSample, flush: packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u,  du_mfu_block_rebuilt: %p, du_mfu_block_rebuilt->p_size: %u", mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_last->sample_number, mmtp_mpu_packet_to_rebuild_last->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild_last->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild_last->mpu_fragmentation_indicator, du_mfu_block_rebuilt, du_mfu_block_rebuilt ? du_mfu_block_rebuilt->p_size : 0 );
//            }
//        } else {
//            atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header( mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_last->sample_number, du_mfu_block_rebuilt, mfu_fragment_counter_missing_mmthsample_header_start, mfu_fragment_count_rebuilt);
//            __MMT_CONTEXT_MPU_DEBUG("atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header MFU DU with MMTHSample, flush: packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, du_mfu_block_rebuilt: %p, du_mfu_block_rebuilt->p_size: %u", mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_last->sample_number, mmtp_mpu_packet_to_rebuild_last->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild_last->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild_last->mpu_fragmentation_indicator, du_mfu_block_rebuilt, du_mfu_block_rebuilt ? du_mfu_block_rebuilt->p_size : 0);
//        }
//        __MMT_CONTEXT_MPU_DEBUG("atsc3 block_destroy MFU DU with MMTHSample, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, du_mfu_block_rebuilt: %p, sdu_mfu_block_rebuilt->p_size: %u", mmtp_mpu_packet_to_rebuild_last->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_last->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_last->sample_number, mmtp_mpu_packet_to_rebuild_last->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild_last->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild_last->mpu_fragmentation_indicator, du_mfu_block_rebuilt, du_mfu_block_rebuilt ? du_mfu_block_rebuilt->p_size : 0);
//        block_Destroy(&du_mfu_block_rebuilt);
//    }
//}


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
					__MMSM_DEBUG("atsc3_mmt_context_mfu_depacketizer: MPT message: checking packet_id: %u, asset_type: %s, default: %u, identifier: %s", mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					//mp_table_asset_row->asset_type == HEVC
					if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID, mp_table_asset_row->asset_type, 4) == 0) {
						atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_essence_packet_id(mp_table_asset_row->mmt_general_location_info.packet_id);
						if(mpu_sequence_number_p && mpu_presentation_time_ntp64_p) {
							atsc3_mmt_mfu_context->__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor(atsc3_mmt_mfu_context,
																																		   mp_table_asset_row->mmt_general_location_info.packet_id,
																																		   *mpu_sequence_number_p,
																																		   *mpu_presentation_time_ntp64_p,
																																		   mpu_presentation_time_seconds,
																																		   mpu_presentation_time_microseconds);

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

					} else if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MP4A_ID, mp_table_asset_row->asset_type, 4) == 0 ||
					            strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_AC_4_ID, mp_table_asset_row->asset_type, 4) == 0 ||
                                strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MHM1_ID, mp_table_asset_row->asset_type, 4) == 0 ||
                                strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MHM2_ID, mp_table_asset_row->asset_type, 4) == 0) {
						//mp_table_asset_row->asset_type ==  MP4A || AC-4
						atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_essence_packet_id(mp_table_asset_row->mmt_general_location_info.packet_id);
						if(mpu_sequence_number_p && mpu_presentation_time_ntp64_p) {
							atsc3_mmt_mfu_context->__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor(atsc3_mmt_mfu_context,
																																		   mp_table_asset_row->mmt_general_location_info.packet_id,
																																		   *mpu_sequence_number_p,
																																		   *mpu_presentation_time_ntp64_p,
																																		   mpu_presentation_time_seconds,
																																		   mpu_presentation_time_microseconds);


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
							atsc3_mmt_mfu_context->__internal__atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor(atsc3_mmt_mfu_context,
																																				 mp_table_asset_row->mmt_general_location_info.packet_id,
																																				 *mpu_sequence_number_p,
																																				 *mpu_presentation_time_ntp64_p,
																																				 mpu_presentation_time_seconds,
																																				 mpu_presentation_time_microseconds);

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
			__MMSM_TRACE("mmt_signalling_message_update_lls_sls_mmt_session: Ignoring signal: 0x%x", mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type);
		}
	}
}

/*
 *
 * from iso 14496-12
 *
 aligned(8) class TrackRunBox extends FullBox(‘trun’, version, tr_flags) {
    unsigned int(32)  sample_count;
    // the following are optional fields
    signed int(32) data_offset;
    unsigned int(32)  first_sample_flags;
    // all fields in the following array are optional
    {
      unsigned int(32)  sample_duration;
      unsigned int(32)  sample_size;
      unsigned int(32)  sample_flags
      if (version == 0)
         { unsigned int(32) sample_composition_time_offset }
      else
         { signed int(32) sample_composition_time_offset }
    }[ sample_count ]
}

aligned(8) class FullBox(unsigned int(32) boxtype, unsigned int(8) v, bit(24) f) extends Box(boxtype) {
    unsigned int(8)   version = v;
    bit(24)           flags = f;
}
*/

uint32_t atsc3_mmt_movie_fragment_extract_sample_duration(block_t* mmt_movie_fragment_metadata) {
    __MMSM_TRACE("atsc3_mmt_movie_fragment_extract_sample_duration: extracting from %p, length: %d", mmt_movie_fragment_metadata, mmt_movie_fragment_metadata->p_size);

    uint32_t box_size = 0;
    uint32_t sample_count = 0;
    uint32_t sample_duration_us = 0;

    uint8_t version = 0;
    uint32_t tr_flags = 0;
    uint32_t data_offset = 0;
    uint32_t first_sample_flags = 0;

    block_Rewind(mmt_movie_fragment_metadata);

    uint8_t* ptr = block_Get(mmt_movie_fragment_metadata);
    ptr += 4;
    for(int i=4; i < mmt_movie_fragment_metadata->p_size-8 && (sample_duration_us == 0); i++) {

        if(ptr[0] == 't' && ptr[1] == 'r' && ptr[2] == 'u' && ptr[3] == 'n') {
            //read our box length from ptr-4
            box_size = ntohl(*(uint32_t*)(ptr-4));
            ptr += 4; //iterate past our box name
            version = *ptr++;
            //next 3 bytes for fullbox flags, 0x000001: data_offset present, 0x000004: first_sample_flags_present

            tr_flags = (*ptr++ << 16) |  (*ptr++ << 8) |  (*ptr++);

            sample_count = ntohl(*(uint32_t*)(ptr));
            ptr += 4;
            if(tr_flags & 0x1) {
                data_offset = ntohl(*(uint32_t*)(ptr));
                ptr += 4;
            }
            if(tr_flags & 0x4) {
                first_sample_flags = ntohl(*(uint32_t*)(ptr));
                ptr += 4;
            }

            if(sample_count > 0) {
                //iterate internal samples is not needed with MFU mode, so bail
                if(tr_flags & 0x000100) {
                    sample_duration_us = ntohl(*(uint32_t *) (ptr));
                    continue;
                } else {
                    //use "default" duration
                    //iso14496-12:2015 - 0x000100 sample‐duration‐present: indicates that each sample has its own duration, otherwise the default is used.
                }
            }
        }

        ptr++;
    }

    __MMSM_TRACE("atsc3_mmt_movie_fragment_extract_sample_duration: returning sample_duration_us as %d", sample_duration_us);

    return sample_duration_us;
}


