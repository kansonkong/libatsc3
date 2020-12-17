/*
 * IAtsc3NdkMediaMMTBridge.h
 *
 *  Created on: Dec 2, 2020
 *      Author: jjustman
 */
#include <string>
#include <vector>

#ifndef SRC_APPLICATION_IATSC3NDKMEDIAMMTBRIDGE_H_
#define SRC_APPLICATION_IATSC3NDKMEDIAMMTBRIDGE_H_

using namespace std;

class IAtsc3NdkMediaMMTBridge {

    public:
        //jni management
        virtual int pinConsumerThreadAsNeeded() = 0;
        virtual int releasePinnedConsumerThreadAsNeeded() = 0;

        //logging
        virtual void LogMsg(const char *msg) = 0;
        virtual void LogMsg(const std::string &msg) = 0;
        virtual void LogMsgF(const char *fmt, ...) = 0;

        /* Media MMT Bridge callbacks for ExoPlayer JNI handoff */
        virtual void atsc3_onInitHEVC_NAL_Extracted(uint16_t packet_id, uint32_t mpu_sequence_number, uint8_t* buffer, uint32_t bufferLen) = 0;
        virtual void atsc3_onInitAudioDecoderConfigurationRecord(uint16_t packet_id, uint32_t mpu_sequence_number, atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record) = 0;

        //Signalling callbacks
        virtual void atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) = 0;
        virtual void atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microsecond) = 0;
        virtual void atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) = 0;

        //Fragment Metadata callbacks
        virtual void atsc3_onExtractedSampleDuration(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t extracted_sample_duration_us) = 0;
        virtual void atsc3_setVideoWidthHeightFromTrak(uint16_t packet_id, uint32_t width, uint32_t height) = 0;

        //MFU callbacks
        virtual void atsc3_onMfuPacket(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected) = 0;
        virtual void atsc3_onMfuPacketCorrupt(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) = 0;
        virtual void atsc3_onMfuPacketCorruptMmthSampleHeader(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) = 0;

        virtual void atsc3_onMfuSampleMissing(uint16_t i, uint32_t i1, uint32_t i2) = 0;
};


#endif /* SRC_APPLICATION_IATSC3NDKMEDIAMMTBRIDGE_H_ */
