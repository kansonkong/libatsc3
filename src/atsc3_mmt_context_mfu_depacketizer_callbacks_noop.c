//
// Created by Jason Justman on 12/1/20.
//

#include "atsc3_mmt_context_mfu_depacketizer_callbacks_noop.h"


//MPU
void atsc3_mmt_mpu_on_sequence_number_change_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number_old, uint32_t mpu_sequence_number_new) {
    //noop
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_mpu_on_sequence_number_change_noop: packet_id: %u, from %d, to %d", packet_id, mpu_sequence_number_old,  mpu_sequence_number_new);
}

//MPU - init box (mpu_metadata) present
void atsc3_mmt_mpu_on_sequence_mpu_metadata_present_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_mpu_metadata) {
    //noop
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_mpu_on_sequence_mpu_metadata_present_noop: packet_id: %u, mpu_sequence_number: %u, mmt_mpu_metadata: %p, size: %d",
                            packet_id,
                            mpu_sequence_number,
                            mmt_mpu_metadata,
                            mmt_mpu_metadata->p_size);

    //todo: extract NALs for example here...
}

//MP_table
void atsc3_mmt_signalling_information_on_mp_table_subset_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mp_table_t* mp_table) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_signalling_information_on_mp_table_subset_noop: mp_table: %p", mp_table);
}

//MP_table
void atsc3_mmt_signalling_information_on_mp_table_complete_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mp_table_t* mp_table) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_signalling_information_on_mp_table_complete_noop: mp_table: %p", mp_table);
}

//audio essence packet id extraction
void atsc3_mmt_signalling_information_on_audio_essence_packet_id_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t audio_packet_id, mp_table_asset_row_t* mp_table_asset_row) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_signalling_information_on_audio_packet_id_noop: audio_packet_id: %u", audio_packet_id);
}

//video essence packet_id extraction
void atsc3_mmt_signalling_information_on_video_essence_packet_id_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t video_packet_id, mp_table_asset_row_t* mp_table_asset_row) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_signalling_information_on_video_packet_id_noop: video_packet_id: %u", video_packet_id);
}

//stpp essence packet_id extraction
void atsc3_mmt_signalling_information_on_stpp_essence_packet_id_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t stpp_packet_id, mp_table_asset_row_t* mp_table_asset_row) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_signalling_information_on_stpp_packet_id_noop: stpp_packet_id: %u", stpp_packet_id);
}


//audio packet id extraction with mpu_sequence and mpu_presentation_time
void atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_noop: audio_packet_id: %u, mpu_sequence_number: %u, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
                            audio_packet_id,
                            mpu_sequence_number,
                            mpu_presentation_time_ntp64,
                            mpu_presentation_time_seconds,
                            mpu_presentation_time_microseconds);
}

//video packet_id extraction with mpu_sequence and mpu_presentation_time
void atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_noop: video_packet_id: %u, mpu_sequence_number: %u, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
                            video_packet_id,
                            mpu_sequence_number,
                            mpu_presentation_time_ntp64,
                            mpu_presentation_time_seconds,
                            mpu_presentation_time_microseconds);
}

//stpp packet_id extraction with mpu_sequence and mpu_presentation_time
void atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_noop: stpp_packet_id: %u, mpu_sequence_number: %u, mpu_presentation_time_ntp64: %llu, mpu_presentation_time_seconds: %u, mpu_presentation_time_microseconds: %u",
                            stpp_packet_id,
                            mpu_sequence_number,
                            mpu_presentation_time_ntp64,
                            mpu_presentation_time_seconds,
                            mpu_presentation_time_microseconds);
}


void atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, mmt_signalling_message_mpu_timestamp_descriptor_t* mmt_signalling_message_mpu_timestamp_descriptor) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_signalling_information_on_mpu_timestamp_descriptor_noop: packet_id: %u, mpu_sequence_number: %d, mmt_signalling_message_mpu_timestamp_descriptor: %p",
                            packet_id,
                            mpu_sequence_number,
                            mmt_signalling_message_mpu_timestamp_descriptor);
}

//MFU callbacks

void atsc3_mmt_mpu_mfu_on_sample_complete_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_rebuilt) {
    //noop;
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_mpu_mfu_on_sample_complete_noop: packet_id: %u, mmtp_timestamp: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d, mfu_fragment count rebuilt: %d",
                            packet_id,
                            mmtp_timestamp,
                            mpu_sequence_number,
                            sample_number,
                            mmt_mfu_sample,
                            mmt_mfu_sample->p_size,
                            mfu_fragment_count_rebuilt);
}

void atsc3_mmt_mpu_mfu_on_sample_corrupt_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_mpu_mfu_on_sample_corrupt_noop: packet_id: %u, mmtp_timestamp: %u, mpu_sequence_number: %u, sample_number: %u, mmt_mfu_sample: %p, len: %d, mfu_fragment count expected: %d, rebuilt %d",
                            packet_id,
                            mmtp_timestamp,
                            mpu_sequence_number,
                            sample_number,
                            mmt_mfu_sample,
                            mmt_mfu_sample->p_size,
                            mfu_fragment_count_expected,
                            mfu_fragment_count_rebuilt);
}



void atsc3_mmt_mpu_mfu_on_sample_missing_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_mpu_mfu_on_sample_missing_noop: packet_id: %u, mpu_sequence_number: %u, sample_number: %u",
                            packet_id,
                            mpu_sequence_number,
                            sample_number);
}



void atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample,  uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_noop: packet_id: %u, mmtp_timestamp: %d, mpu_sequence_number: %u, sample_number: %u, mfu_fragment_count_expected: %u, mfu_fragment_count_rebuilt: %u",
                            packet_id,
                            mmtp_timestamp,
                            mpu_sequence_number,
                            sample_number,
                            mfu_fragment_count_expected,
                            mfu_fragment_count_rebuilt);
}

//MPU - init box (mpu_metadata) present
void atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_movie_fragment_metadata) {
	if(!atsc3_mmt_mfu_context) {
	 __MMT_CONTEXT_MPU_WARN("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop: atsc3_mmt_mfu_context is NULL for packet_id: %d, mpu_sequence_number: %d, mmt_movie_fragment_metadata: %p",
							packet_id, mpu_sequence_number, mmt_movie_fragment_metadata);
		return;
	}
	
	if(!atsc3_mmt_mfu_context->mmtp_packet_id_packets_container) {
	 __MMT_CONTEXT_MPU_WARN("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop: atsc3_mmt_mfu_context: %p, but mmtp_packet_id_packets_container is NULL for packet_id: %d, mpu_sequence_number: %d, mmt_movie_fragment_metadata: %p",
							atsc3_mmt_mfu_context, packet_id, mpu_sequence_number, mmt_movie_fragment_metadata);
		return;
	}
	
	if(!mmt_movie_fragment_metadata || !mmt_movie_fragment_metadata->p_size) {
		__MMT_CONTEXT_MPU_WARN("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop: packet_id: %d, mpu_sequence_number: %d, mmt_movie_fragment_metadata: %p: returned null or no length!",
							   packet_id, mpu_sequence_number, mmt_movie_fragment_metadata);
		return;
	}
	
	uint32_t decoder_configuration_timebase = 1000000; //set as default to uS
    uint32_t extracted_sample_duration_us = 0;

    if(!mmt_movie_fragment_metadata || !mmt_movie_fragment_metadata->p_size) {
        __MMT_CONTEXT_MPU_WARN("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop: packet_id: %d, mpu_sequence_number: %d, mmt_movie_fragment_metadata: %p: returned null or no length!",
                               packet_id, mpu_sequence_number, mmt_movie_fragment_metadata);
        return;
    }
	
	if(!atsc3_mmt_mfu_context->mmtp_packet_id_packets_container) {
		__MMT_CONTEXT_MPU_WARN("atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop: atsc3_mmt_mfu_context: %p, mmtp_packet_id_packets_container is NULL for: packet_id: %d, mpu_sequence_number: %d, mmt_movie_fragment_metadata: %p!",
							   atsc3_mmt_mfu_context, packet_id, mpu_sequence_number, mmt_movie_fragment_metadata);
		return;
	}

    if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record && atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->timebase) {
        decoder_configuration_timebase = atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_video_decoder_configuration_record->timebase;
    } else if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record && atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record->timebase) {
        decoder_configuration_timebase = atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_audio_decoder_configuration_record->timebase;
    } else if(atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record && atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record->timebase) {
        decoder_configuration_timebase = atsc3_mmt_mfu_context->mmtp_packet_id_packets_container->atsc3_stpp_decoder_configuration_record->timebase;
    }

    extracted_sample_duration_us = atsc3_mmt_movie_fragment_extract_sample_duration_us(mmt_movie_fragment_metadata, decoder_configuration_timebase);

    __MMT_CONTEXT_MPU_DEBUG("callback_noop: atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_noop: packet_id: %u, mpu_sequence_number: %u, mmt_movie_fragment_metadata: %p, size: %d, extracted extracted_sample_duration_us: %d",
                            packet_id,
                            mpu_sequence_number,
                            mmt_movie_fragment_metadata,
                            mmt_movie_fragment_metadata->p_size,
                            extracted_sample_duration_us);
}

//jjustman-2021-09-15
void atsc3_mmt_signalling_information_on_userservicedescription_present_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_signalling_information_usbd_component_t* mmt_atsc3_usbd_message) {
	__ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_GUARD_AND_LOG_ATSC3_MMT_MFU_CONTEXT("mmt_atsc3_usbd_message", mmt_atsc3_usbd_message);
	block_Rewind(mmt_atsc3_usbd_message->usbd_payload);
	__MMT_CONTEXT_MPU_DEBUG("USBD table is:\n%s", block_Get(mmt_atsc3_usbd_message->usbd_payload));
}

bool atsc3_mmt_signalling_information_on_routecomponent_message_present_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_route_component_t* mmt_atsc3_route_component) {
	__ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_GUARD_AND_LOG_ATSC3_MMT_MFU_CONTEXT_RETURN_FALSE("mmt_atsc3_route_component", mmt_atsc3_route_component);
	__MMT_CONTEXT_MPU_DEBUG("mmt_atsc3_route_component ptr: %p, value:\n", mmt_atsc3_route_component);
	mmt_atsc3_route_component_dump(mmt_atsc3_route_component);
	return false;
}

void atsc3_mmt_signalling_information_on_held_message_present_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_held_message_t* mmt_atsc3_held_message) {
	__ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_GUARD_AND_LOG_ATSC3_MMT_MFU_CONTEXT("mmt_atsc3_held_message", mmt_atsc3_held_message);
	block_Rewind(mmt_atsc3_held_message->held_message);
	__MMT_CONTEXT_MPU_DEBUG("mmt_atsc3_held_message ptr: %p, value:\n%s", mmt_atsc3_held_message, block_Get(mmt_atsc3_held_message->held_message));
}

void mmt_atsc3_message_content_type_video_stream_properties_descriptor_present_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_video_stream_properties_descriptor_t* mmt_atsc3_video_stream_properties_descriptor_message){
	__ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_GUARD_AND_LOG_ATSC3_MMT_MFU_CONTEXT("mmt_atsc3_video_stream_properties_descriptor_message", mmt_atsc3_video_stream_properties_descriptor_message);
	
	//jjustman-2021-09-15 - video stream properties descriptor is challenging...
	__MMT_CONTEXT_MPU_DEBUG("mmt_atsc3_video_stream_properties_descriptor_message: %p, number_of_assets: %d", mmt_atsc3_video_stream_properties_descriptor_message, mmt_atsc3_video_stream_properties_descriptor_message->descriptor_header.number_of_assets);
	for(int i=0; i < mmt_atsc3_video_stream_properties_descriptor_message->descriptor_header.number_of_assets; i++) {
		mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset_t* mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset = mmt_atsc3_video_stream_properties_descriptor_message->mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset_v.data[i];
		
		__MMT_CONTEXT_MPU_DEBUG(" asset: %d: asset_id_length: %d, asset_id: %s, codec_code: %s",
								i,
								mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset->asset_header.asset_id_length,
								mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset->asset_header.asset_id,
								mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset->codec_code);
	}
}

void atsc3_mmt_signalling_information_on_caption_asset_descriptor_present_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_caption_asset_descriptor_t* mmt_atsc3_caption_asset_descriptor_message){
	__ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_GUARD_AND_LOG_ATSC3_MMT_MFU_CONTEXT("mmt_atsc3_caption_asset_descriptor_message", mmt_atsc3_caption_asset_descriptor_message);
	
	__MMT_CONTEXT_MPU_DEBUG("mmt_atsc3_caption_asset_descriptor_message: %p, number_of_assets: %d", mmt_atsc3_caption_asset_descriptor_message, mmt_atsc3_caption_asset_descriptor_message->descriptor_header.number_of_assets);
	for(int i=0; i < mmt_atsc3_caption_asset_descriptor_message->descriptor_header.number_of_assets; i++) {
		mmt_atsc3_message_content_type_caption_asset_descriptor_asset_t* mmt_atsc3_message_content_type_caption_asset_descriptor_asset = mmt_atsc3_caption_asset_descriptor_message->mmt_atsc3_message_content_type_caption_asset_descriptor_asset_v.data[i];
		
		__MMT_CONTEXT_MPU_DEBUG(" asset: %d: asset_id_length: %d, asset_id: %s, language_length: %d, language: %s",
								i,
								mmt_atsc3_message_content_type_caption_asset_descriptor_asset->asset_header.asset_id_length,
								mmt_atsc3_message_content_type_caption_asset_descriptor_asset->asset_header.asset_id,
								mmt_atsc3_message_content_type_caption_asset_descriptor_asset->language_header.language_length,
								mmt_atsc3_message_content_type_caption_asset_descriptor_asset->language_header.language);
	}
}

void atsc3_mmt_signalling_information_on_audio_stream_properties_descriptor_present_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_audio_stream_properties_descriptor_t* mmt_atsc3_audio_stream_properties_descriptor_message){
	__ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_GUARD_AND_LOG_ATSC3_MMT_MFU_CONTEXT("mmt_atsc3_audio_stream_properties_descriptor_message", mmt_atsc3_audio_stream_properties_descriptor_message);
	
	__MMT_CONTEXT_MPU_DEBUG("mmt_atsc3_audio_stream_properties_descriptor_message: %p, number_of_assets: %d", mmt_atsc3_audio_stream_properties_descriptor_message, mmt_atsc3_audio_stream_properties_descriptor_message->descriptor_header.number_of_assets);
	for(int i=0; i < mmt_atsc3_audio_stream_properties_descriptor_message->descriptor_header.number_of_assets; i++) {
		mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_t* mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset = mmt_atsc3_audio_stream_properties_descriptor_message->mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_v.data[i];
		
		__MMT_CONTEXT_MPU_DEBUG(" asset: %d: asset_id_length: %d, asset_id: %s",
								i,
								mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset->asset_header.asset_id_length,
								mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset->asset_header.asset_id);
		
		for(int j=0; j < mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset->num_presentations; j++) {
			mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_t* mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation = mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset->mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_v.data[j];
			
			if(mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->language_present) {
				//remember, we are _minus1
				__MMT_CONTEXT_MPU_DEBUG("\tlanguage present, num_languages: %d", mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->num_languages_minus1 + 1);
				
				for(int k=0; k < mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->num_languages_minus1 + 1; k++) {
					mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_language_t* mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_language = mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_language_v.data[k];
					
					__MMT_CONTEXT_MPU_DEBUG("\t\tlanguage length: %d, language: %s",
											mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_language->language_length, mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation_language->language);
				}
			}
			
			if(mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->label_present) {
				__MMT_CONTEXT_MPU_DEBUG("\tlabel present, length: %d, value: %s",
										mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->label_length, mmt_atsc3_message_content_type_audio_stream_properties_descriptor_asset_presentation->label_data_byte);
				
			}
		}
	}
}

void atsc3_mmt_signalling_information_on_security_properties_descriptor_LAURL_present_noop(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_t* mmt_atsc3_security_properties_descriptor_LAURL_message){
	__ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_GUARD_AND_LOG_ATSC3_MMT_MFU_CONTEXT("mmt_atsc3_security_properties_descriptor_LAURL_message", mmt_atsc3_security_properties_descriptor_LAURL_message);

	for(int i=0; i < mmt_atsc3_security_properties_descriptor_LAURL_message->descriptor_header.number_of_assets; i++) {
		mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_t* mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset = mmt_atsc3_security_properties_descriptor_LAURL_message->mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_v.data[i];
		
		__MMT_CONTEXT_MPU_DEBUG(" asset: %d: asset_id_length: %d, asset_id: %s",
								i,
								mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->asset_header.asset_id_length,
								mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset->asset_header.asset_id);
	}
}


atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_callbacks_noop_new() {
    atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_internal_flows_new();

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

	//jjustman-2021-09-15 - adding additional mmt_atsc3 asset descriptor callbacks
		
	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_UserServiceDescription: mmt_atsc3_message_content_type == 0x0001 -> mmt_atsc3_signalling_information_usbd_component_t
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_userservicedescription_present = &atsc3_mmt_signalling_information_on_userservicedescription_present_noop;
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_routecomponent_message_present = &atsc3_mmt_signalling_information_on_routecomponent_message_present_noop;

	//jjustman-2021-09-14 - TODO: add in callbacks for AEI -> 0x0004

	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_HELD
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_held_message_present = &atsc3_mmt_signalling_information_on_held_message_present_noop;

	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_VIDEO_STREAM_PROPERTIES_DESCRIPTOR
	atsc3_mmt_mfu_context->mmt_atsc3_message_content_type_video_stream_properties_descriptor_present = &mmt_atsc3_message_content_type_video_stream_properties_descriptor_present_noop;

	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_caption_asset_descriptor_present = &atsc3_mmt_signalling_information_on_caption_asset_descriptor_present_noop;

	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_AUDIO_STREAM_PROPERTIES_DESCRIPTOR
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_stream_properties_descriptor_present = &atsc3_mmt_signalling_information_on_audio_stream_properties_descriptor_present_noop;

	//MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL
	atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_security_properties_descriptor_LAURL_present = &atsc3_mmt_signalling_information_on_security_properties_descriptor_LAURL_present_noop;

    return atsc3_mmt_mfu_context;
}
