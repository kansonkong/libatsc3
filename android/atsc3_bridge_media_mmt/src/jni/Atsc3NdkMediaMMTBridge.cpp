#include "Atsc3NdkMediaMMTBridge.h"

pthread_once_t Atsc3NdkMediaMMTBridge::JniPtrOnce = PTHREAD_ONCE_INIT;
pthread_key_t Atsc3NdkMediaMMTBridge::JniPtr = 0;
map<jobject, Atsc3NdkMediaMMTBridge*> Atsc3NdkMediaMMTBridge::MediaBridgePtrMap;
int _NDK_MEDIA_MMT_BRIDGE_atsc3_onMfuPacket_counter = 0;

Atsc3NdkMediaMMTBridge::Atsc3NdkMediaMMTBridge(JNIEnv* env, jobject jni_instance, jobject fragment_buffer, jint max_fragment_count) {
    this->jni_instance_globalRef = env->NewGlobalRef(jni_instance);

    this->mmtExtractor = new MMTExtractor();

    uint8_t* fragmentBufferPtr = reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(fragment_buffer));
    uint32_t fragmentBufferSize = (uint32_t)(env->GetDirectBufferCapacity(fragment_buffer));
    this->fragmentBuffer = new Atsc3RingBuffer(fragmentBufferPtr, fragmentBufferSize, fragmentBufferSize / max_fragment_count);


    /* notes:
      keep a single block_t instance for udp processing:
          block_Alloc will set _a_size with original malloc'd size, set to MAX_ATSC3_PHY_IP_DATAGRAM_SIZE (65535)

     when processing a udp packet, e.g. atsc3_process_mmtp_udp_packet:
          call block_resize() (performs soft resize by updating ->p_size)
          call block_write() w/ directBufferAddress(byte_buffer) to copy payload

          _do not_ block_destroy() after we are done to avoid calloc/free memory trashing
    */
    this->preAllocInFlightUdpPacket = block_Alloc(MAX_ATSC3_PHY_IP_DATAGRAM_SIZE);
}

//jni management
int Atsc3NdkMediaMMTBridge::pinConsumerThreadAsNeeded() {
    Atsc3JniEnv* localAtsc3NdkMediaMMTBridgeJniEnv = nullptr;

    localAtsc3NdkMediaMMTBridgeJniEnv = Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv();

    if(localAtsc3NdkMediaMMTBridgeJniEnv) {
        //_NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkMediaMMTBridge::pinConsumerThreadAsNeeded: mJavaVM: %p, localAtsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() already pinned: %p", mJavaVM, localAtsc3NdkMediaMMTBridgeJniEnv);
    } else {
        localAtsc3NdkMediaMMTBridgeJniEnv = new Atsc3JniEnv(mJavaVM);
        pthread_setspecific(Atsc3NdkMediaMMTBridge::JniPtr, localAtsc3NdkMediaMMTBridgeJniEnv);
        //_NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkMediaMMTBridge::pinConsumerThreadAsNeeded: mJavaVM: %p, creating localAtsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv(): %p", mJavaVM, localAtsc3NdkMediaMMTBridgeJniEnv);
    }
    return 0;
}

//special hack so we don't try to double-AttachCurrentThread, but keep a reference of our first (i.e. Atsc3NdkApplicationBridge bridgeConsumerJniEnv) attached JniEnv
int Atsc3NdkMediaMMTBridge::referenceConsumerJniEnvAsNeeded(IAtsc3JniEnv* consumerAtsc3JniEnv) {
    Atsc3JniEnv* localAtsc3NdkMediaMMTBridgeJniEnv = nullptr;

    localAtsc3NdkMediaMMTBridgeJniEnv = Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv();

    if(localAtsc3NdkMediaMMTBridgeJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_TRACE("Atsc3NdkMediaMMTBridge::referenceConsumerJniEnvAsNeeded: localAtsc3NdkMediaMMTBridge is  already persisted to this thread: %p", localAtsc3NdkMediaMMTBridgeJniEnv);
    } else {
        pthread_setspecific(Atsc3NdkMediaMMTBridge::JniPtr, consumerAtsc3JniEnv);
        //_NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkMediaMMTBridge::referenceConsumerJniEnvAsNeeded: this: %p, assigning consumerAtsc3JniEnv: %p, jniEnv: %p",  this, consumerAtsc3JniEnv, consumerAtsc3JniEnv->Get());
    }
    return 0;

}
int Atsc3NdkMediaMMTBridge::releasePinnedConsumerThreadAsNeeded() {
    Atsc3JniEnv* localAtsc3NdkMediaMMTBridge = nullptr;

    localAtsc3NdkMediaMMTBridge = Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv();

    if(localAtsc3NdkMediaMMTBridge) {
        _NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkMediaMMTBridge::releasePinnedConsumerThreadAsNeeded: localAtsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() is: %p:", localAtsc3NdkMediaMMTBridge);
        delete localAtsc3NdkMediaMMTBridge;
        localAtsc3NdkMediaMMTBridge = nullptr;
        pthread_setspecific(Atsc3NdkMediaMMTBridge::JniPtr, nullptr);

    } else {
        _NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkMediaMMTBridge::releasePinnedConsumerThreadAsNeeded: mJavaVM: %p, no localAtsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() found!", mJavaVM);
    }

    return 0;
}

bool Atsc3NdkMediaMMTBridge::isConsumerThreadPinned() {
    return pthread_getspecific(Atsc3NdkMediaMMTBridge::JniPtr) != NULL;
}


void Atsc3NdkMediaMMTBridge::LogMsg(const char *msg)
{
    this->pinConsumerThreadAsNeeded(); //jjustman-2020-12-17 - hack

    if (!mJavaVM) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("LogMsg: mJavaVM is NULL!");
        return;
    }

    Atsc3JniEnv* localJniEnv = Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv();

    if (!localJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("LogMsg: error creating env pin!");
        return;
    }
    jstring js = localJniEnv->Get()->NewStringUTF(msg);
    localJniEnv->Get()->CallIntMethod(jni_instance_globalRef, mOnLogMsgId, js);
    localJniEnv->Get()->DeleteLocalRef(js);
}

void Atsc3NdkMediaMMTBridge::LogMsg(const std::string &str)
{
    LogMsg(str.c_str());
}

void Atsc3NdkMediaMMTBridge::LogMsgF(const char *fmt, ...)
{
    va_list v;
    char msg[1024];
    va_start(v, fmt);
    vsnprintf(msg, sizeof(msg)-1, fmt, v);
    msg[sizeof(msg)-1] = 0;
    va_end(v);

    LogMsg(msg);
}

//From JNI method handler for UDP packet processing

//jjustman-2020-12-16 - keep our block_t instead of re-alloc/destroying
int Atsc3NdkMediaMMTBridge::acceptNdkByteBufferUdpPacket(jobject byte_buffer, jint byte_buffer_length) {
    int ret = 0;

    uint8_t* udp_packet_buffer_ptr = nullptr;
    jlong udp_packet_buffer_ptr_length = 0;

    Atsc3JniEnv* localJniEnv = Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv();
    if(!localJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::acceptNdkByteBufferUdpPacket: localJniEnv is NULL!");
        return -1;
    }

    udp_packet_buffer_ptr = reinterpret_cast<uint8_t*>(localJniEnv->Get()->GetDirectBufferAddress(byte_buffer));
    udp_packet_buffer_ptr_length = localJniEnv->Get()->GetDirectBufferCapacity(byte_buffer);

    if(!udp_packet_buffer_ptr) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::acceptNdkByteBufferUdpPacket: udp_packet_buffer_ptr is NULL!");
        return -1;
    }

    if(!byte_buffer_length) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::acceptNdkByteBufferUdpPacket: byte_buffer_length is 0!");
        return -1;
    }

    if(!udp_packet_buffer_ptr_length) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::acceptNdkByteBufferUdpPacket: udp_packet_buffer_ptr_length is 0!");
        return -1;
    }

//    if(udp_packet_buffer_ptr_length != byte_buffer_length)  {
//        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::acceptNdkByteBufferUdpPacket: udp_packet_buffer_ptr_length != byte_buffer_length, %ld != %d", udp_packet_buffer_ptr_length, byte_buffer_length);
//        return -1;
//    }

    if(!this->preAllocInFlightUdpPacket) {
        _NDK_MEDIA_MMT_BRIDGE_WARN("Atsc3NdkMediaMMTBridge::acceptNdkByteBufferUdpPacket: this->preAllocInFlightUdpPacket went away, reallocating");
        this->preAllocInFlightUdpPacket = block_Alloc(MAX_ATSC3_PHY_IP_DATAGRAM_SIZE);
    }

    //jjustman-2020-12-16 - TODO: support block_t w/ transprent directBufferAddress and only alloc/memcpy if p_buffer is modified?
    block_Rewind(this->preAllocInFlightUdpPacket);
    block_Resize(this->preAllocInFlightUdpPacket, byte_buffer_length);

    block_Write(this->preAllocInFlightUdpPacket, udp_packet_buffer_ptr, byte_buffer_length);
    block_Rewind(this->preAllocInFlightUdpPacket);

    extractUdpPacket(this->preAllocInFlightUdpPacket);
    block_Rewind(this->preAllocInFlightUdpPacket);

    return ret;
}

void Atsc3NdkMediaMMTBridge::extractUdpPacket(block_t* udpPacket) {
    if (!this->mmtExtractor) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::extractUdpPacket: this->mmtExtractor is NULL!");
        return;
    }

    mmtExtractor->atsc3_core_service_bridge_process_mmt_packet(udpPacket);
}

uint32_t Atsc3NdkMediaMMTBridge::getFragmentBufferCurrentPosition() {
    if (fragmentBuffer) {
        return fragmentBuffer->getCurrentPosition();
    }

    return 0;
}

uint32_t Atsc3NdkMediaMMTBridge::getFragmentBufferCurrentPageNumber() {
    if (fragmentBuffer) {
        return fragmentBuffer->getCurrentPageNumber();
    }

    return 0;
}


//wired up and invoked from atsc3_mmt_mfu_context_callbacks_default_jni.cpp

//MMT Initialization callbacks for Video and Audio format(s)
//push extracted HEVC nal's to MediaCodec for init
void Atsc3NdkMediaMMTBridge::atsc3_onInitHEVC_NAL_Extracted(uint16_t service_id, uint16_t packet_id, uint32_t mpu_sequence_number, uint8_t* buffer, uint32_t bufferLen) {
//    this->pinConsumerThreadAsNeeded(); //jjustman-2020-12-17 - hack
//
//    if (!Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()) {
//        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onInitHEVC_NAL_Extracted: Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() is NULL!");
//        return;
//    }
//
//    _NDK_MEDIA_MMT_BRIDGE_DEBUG("Atsc3NdkMediaMMTBridge::atsc3_onInitHEVC_NAL_Extracted: with service_id: %d, packet_id: %d, mpu_sequence_number: %d, bufferLen: %d",
//                               service_id, packet_id, mpu_sequence_number, bufferLen);
//
//    jobject jobjectByteBuffer = Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()->Get()->NewDirectByteBuffer(buffer, bufferLen);
//     Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()->Get()->CallIntMethod(jni_instance_globalRef, atsc3_OnInitHEVC_NAL_Extracted, packet_id, (int64_t)mpu_sequence_number, jobjectByteBuffer, bufferLen);
//    Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()->Get()->DeleteLocalRef(jobjectByteBuffer);

    //writeToRingBuffer(RING_BUFFER_PAGE_INIT, packet_id, 0, 0, buffer, bufferLen);
    fragmentBuffer->write(Atsc3RingBuffer::RING_BUFFER_PAGE_INIT, packet_id, 0, 0, buffer, bufferLen);
}

void Atsc3NdkMediaMMTBridge::atsc3_onInitAudioDecoderConfigurationRecord(uint16_t service_id, uint16_t packet_id, uint32_t mpu_sequence_number, atsc3_audio_decoder_configuration_record_t* atsc3_audio_decoder_configuration_record) {
    this->pinConsumerThreadAsNeeded(); //jjustman-2020-12-17 - hack

    Atsc3JniEnv* jniEnv = Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv();

    if (!jniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onInitAudioDecoderConfigurationRecord: Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() is NULL!");
        return;
    }

    if(!atsc3_audio_decoder_configuration_record) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onInitAudioDecoderConfigurationRecord: atsc3_audio_decoder_configuration_record is NULL!");
        return;
    }
    if(!atsc3_OnInitAudioDecoderConfigurationRecord) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onInitAudioDecoderConfigurationRecord: atsc3_OnInitAudioDecoderConfigurationRecord jmethodID is NULL!");
        return;
    }

    jclass jcls = mmtAudioDecoderConfigurationRecord_jclass_global_ref;
    jobject jobj = jniEnv->Get()->AllocObject(jcls);

    if(!jobj) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onInitAudioDecoderConfigurationRecord: unable to instantiate mmtAudioDecoderConfigurationRecord_jclass_global_ref, jobj is NULL!");
        return;
    }

    jclass j_ac4_se_box_cls = mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_jclass_global_ref;
    jobject j_ac4_se_box_obj = nullptr;

    jclass j_ac4_se_specific_box_cls = mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_AC4SpecificBox_jclass_global_ref;
    jobject j_ac4_se_specific_box_obj = nullptr;

    jfieldID packet_id_valId = jniEnv->Get()->GetFieldID(jcls, "packet_id", "I");
    jniEnv->Get()->SetIntField(jobj, packet_id_valId, packet_id);

    //marshall our uint32_t packet_id to int64_t -> long in java
    jfieldID mpu_sequence_number_valId = jniEnv->Get()->GetFieldID(jcls, "mpu_sequence_number", "J");
    jniEnv->Get()->SetLongField(jobj, mpu_sequence_number_valId, (int64_t)mpu_sequence_number);

    jfieldID channel_count_valId = jniEnv->Get()->GetFieldID(jcls, "channel_count", "I");
    jniEnv->Get()->SetIntField(jobj, channel_count_valId, atsc3_audio_decoder_configuration_record->channel_count);

    jfieldID sample_depth_valId = jniEnv->Get()->GetFieldID(jcls, "sample_depth", "I");
    jniEnv->Get()->SetIntField(jobj, sample_depth_valId, atsc3_audio_decoder_configuration_record->sample_depth);

    //unsigned 2^31 should be fine...
    jfieldID sample_rate_valId = jniEnv->Get()->GetFieldID(jcls, "sample_rate", "I");
    jniEnv->Get()->SetIntField(jobj, sample_rate_valId, (int32_t)atsc3_audio_decoder_configuration_record->sample_rate);

    jfieldID timebase_valId = jniEnv->Get()->GetFieldID(jcls, "timebase", "J");
    jniEnv->Get()->SetLongField(jobj, timebase_valId, (int64_t)atsc3_audio_decoder_configuration_record->timebase);

    jfieldID sample_duration_valId = jniEnv->Get()->GetFieldID(jcls, "sample_duration", "J");
    jniEnv->Get()->SetLongField(jobj, sample_duration_valId, (int64_t)atsc3_audio_decoder_configuration_record->sample_duration);

    if(atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box) {
        //build our ac4_sample_entry box
        j_ac4_se_box_obj = jniEnv->Get()->AllocObject(j_ac4_se_box_cls);

        if(!j_ac4_se_box_obj) {
            _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onInitAudioDecoderConfigurationRecord: unable to instantiate mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_jclass_global_ref, j_ac4_se_box_obj is NULL!");
            return; //todo: goto cleanup
        }

        jfieldID ac4_se_box_size_valId = jniEnv->Get()->GetFieldID(j_ac4_se_box_cls, "box_size", "J");
        jniEnv->Get()->SetLongField(j_ac4_se_box_obj, ac4_se_box_size_valId, (int64_t)atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->box_size);

        //type - int32 is fine as we are really a 4cc
        jfieldID ac4_se_box_type_valId = jniEnv->Get()->GetFieldID(j_ac4_se_box_cls, "type", "I");
        jniEnv->Get()->SetIntField(j_ac4_se_box_obj, ac4_se_box_type_valId, (int32_t)atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->type);

        jfieldID ac4_se_box_channel_count_valId = jniEnv->Get()->GetFieldID(j_ac4_se_box_cls, "channel_count", "I");
        jniEnv->Get()->SetIntField(j_ac4_se_box_obj, ac4_se_box_channel_count_valId, (int32_t)atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->channel_count);

        jfieldID ac4_se_box_sample_size_valId = jniEnv->Get()->GetFieldID(j_ac4_se_box_cls, "sample_size", "I");
        jniEnv->Get()->SetIntField(j_ac4_se_box_obj, ac4_se_box_sample_size_valId, (int32_t)atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->sample_size);

        jfieldID ac4_se_box_sampling_frequency_valId = jniEnv->Get()->GetFieldID(j_ac4_se_box_cls, "sampling_frequency", "I");
        jniEnv->Get()->SetIntField(j_ac4_se_box_obj, ac4_se_box_sampling_frequency_valId, (int32_t)atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->sampling_frequency);

        //assumed required that if we are ac4_sample, then we must have an ac4_specific box
        j_ac4_se_specific_box_obj = jniEnv->Get()->AllocObject(j_ac4_se_specific_box_cls);

        if(!j_ac4_se_specific_box_obj) {
            _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onInitAudioDecoderConfigurationRecord: unable to instantiate mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_AC4SpecificBox_jclass_global_ref, j_ac4_se_specific_box_obj is NULL!");
            return; //todo: goto cleanup
        }

        jfieldID ac4_se_specific_box_size_valId = jniEnv->Get()->GetFieldID(j_ac4_se_specific_box_cls, "box_size", "J");
        jniEnv->Get()->SetLongField(j_ac4_se_specific_box_obj, ac4_se_specific_box_size_valId, (int64_t)atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.box_size);

        //type - int32 is fine as we are really a 4cc
        jfieldID ac4_se_specific_box_type_valId = jniEnv->Get()->GetFieldID(j_ac4_se_specific_box_cls, "type", "I");
        jniEnv->Get()->SetIntField(j_ac4_se_specific_box_obj, ac4_se_specific_box_type_valId, (int32_t)atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.type);

        jfieldID ac4_se_box_specific_ac4_dsi_version_valId = jniEnv->Get()->GetFieldID(j_ac4_se_specific_box_cls, "ac4_dsi_version", "I");
        jniEnv->Get()->SetIntField(j_ac4_se_specific_box_obj, ac4_se_box_specific_ac4_dsi_version_valId, atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.ac4_dsi_version);

        jfieldID ac4_se_box_specific_ac4_bitstream_version_valId = jniEnv->Get()->GetFieldID(j_ac4_se_specific_box_cls, "bitstream_version", "I");
        jniEnv->Get()->SetIntField(j_ac4_se_specific_box_obj, ac4_se_box_specific_ac4_bitstream_version_valId, atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.bitstream_version);

        jfieldID ac4_se_box_specific_fs_index_valId = jniEnv->Get()->GetFieldID(j_ac4_se_specific_box_cls, "fs_index", "I");
        jniEnv->Get()->SetIntField(j_ac4_se_specific_box_obj, ac4_se_box_specific_fs_index_valId, atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.fs_index);

        jfieldID ac4_se_box_specific_frame_rate_index_valId = jniEnv->Get()->GetFieldID(j_ac4_se_specific_box_cls, "frame_rate_index", "I");
        jniEnv->Get()->SetIntField(j_ac4_se_specific_box_obj, ac4_se_box_specific_frame_rate_index_valId, atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.frame_rate_index);

        jfieldID ac4_se_box_specific_n_presentations_valId = jniEnv->Get()->GetFieldID(j_ac4_se_specific_box_cls, "n_presentations", "I");
        jniEnv->Get()->SetIntField(j_ac4_se_specific_box_obj, ac4_se_box_specific_n_presentations_valId, atsc3_audio_decoder_configuration_record->atsc3_audio_ac4_sample_entry_box->atsc3_audio_ac4_specific_box.n_presentations);

        //lastly, set our audioAC4SampleEntryBox
        jfieldID audioAC4SampleEntryBox_valId = jniEnv->Get()->GetFieldID(jcls, "audioAC4SampleEntryBox", "Lorg/ngbp/libatsc3/middleware/android/mmt/models/MMTAudioDecoderConfigurationRecord$AudioAC4SampleEntryBox;");
        jniEnv->Get()->SetObjectField(jobj, audioAC4SampleEntryBox_valId, j_ac4_se_box_obj);
    }


    _NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkMediaMMTBridge::atsc3_onInitAudioDecoderConfigurationRecord: with service_id: %d, packet_id: %d, mpu_sequence_number: %d, atsc3_audio_decoder_configuration_record: %p, channel_count: %d",
                               service_id, packet_id, mpu_sequence_number, atsc3_audio_decoder_configuration_record, atsc3_audio_decoder_configuration_record->channel_count);

    jniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_OnInitAudioDecoderConfigurationRecord, packet_id, (int64_t)mpu_sequence_number, jobj);
    if(j_ac4_se_box_obj) {
        jniEnv->Get()->DeleteLocalRef(j_ac4_se_box_obj);
    }

    if(j_ac4_se_specific_box_obj) {
        jniEnv->Get()->DeleteLocalRef(j_ac4_se_specific_box_obj);
    }

    jniEnv->Get()->DeleteLocalRef(jobj);

    return;
}



//MMT Signalling callbacks
void Atsc3NdkMediaMMTBridge::atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    this->pinConsumerThreadAsNeeded(); //jjustman-2020-12-17 - hack
    if (!Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() is NULL!");
        return;
    }

     Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()->Get()->CallIntMethod(jni_instance_globalRef, atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID, video_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

void Atsc3NdkMediaMMTBridge::atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    this->pinConsumerThreadAsNeeded(); //jjustman-2020-12-17 - hack
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() is NULL!");
        return;
    }

     Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()->Get()->CallIntMethod(jni_instance_globalRef, atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID, audio_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

void Atsc3NdkMediaMMTBridge::atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    this->pinConsumerThreadAsNeeded(); //jjustman-2020-12-17 - hack
     if (!Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() is NULL!");
        return;
    }

     Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()->Get()->CallIntMethod(jni_instance_globalRef, atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID, stpp_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

//MFU metadata for sample duration
void Atsc3NdkMediaMMTBridge::atsc3_onExtractedSampleDuration(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t extracted_sample_duration_us) {
    this->pinConsumerThreadAsNeeded(); //jjustman-2020-12-17 - hack

    Atsc3JniEnv* localJniEnv = Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv();

    if (!localJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: Atsc3NdkMediaMMTBridge::atsc3_onExtractedSampleDuration: localJniEnv is NULL!");
        return;
    }

    localJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onExtractedSampleDurationID, (int32_t)packet_id, (int64_t)mpu_sequence_number, (int64_t)extracted_sample_duration_us);
}


//video w/h for rendering
void Atsc3NdkMediaMMTBridge::atsc3_setVideoWidthHeightFromTrak(uint16_t packet_id, uint32_t width, uint32_t height) {
    this->pinConsumerThreadAsNeeded(); //jjustman-2020-12-17 - hack
    if (!Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() is NULL!");
        return;
    }
     Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()->Get()->CallIntMethod(jni_instance_globalRef, atsc3_setVideoWidthHeightFromTrakID, packet_id, width, height);
}

//on fully recovered MFU packet
void Atsc3NdkMediaMMTBridge::atsc3_onMfuPacket(uint16_t service_id, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_rebuilt) {
//    Atsc3JniEnv* localJniEnv = Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv();
//
//    if (!localJniEnv) {
//        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() is NULL!");
//        return;
//    }
//
//    //LogMsg("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacket: before localJniEnv->Get() NewDirectByteBuffer");
//
//    //_NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacket: this: %p, localJniEnv: %p, localJniEnv->get(): %p, packet_id: %d, mpu_sequence_number: %d, buffer: %p, length: %d", this, localJniEnv, localJniEnv->Get(), packet_id, mpu_sequence_number, buffer, bufferLen);
//    jobject jobjectLocalByteBuffer = localJniEnv->Get()->NewDirectByteBuffer(buffer, bufferLen);
//    //LogMsg("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacket: after localJniEnv->Get() NewDirectByteBuffer");
//
//    //_NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacket: before CallIntMethod, jobjectLocalByteBuffer is: %d, jni_instance_globalRef: %p, method: %d", jobjectLocalByteBuffer, jni_instance_globalRef, atsc3_onMfuPacketID);
//
//    //LogMsg("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacket: before localJniEnv->Get()->CallIntMethod");
//    localJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketID, (int32_t) packet_id, (int64_t)mpu_sequence_number, (int32_t)sample_number, jobjectLocalByteBuffer, (int32_t)bufferLen, (int64_t)presentationUs, (int32_t)mfu_fragment_count_rebuilt);
//    //LogMsg("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacket: after localJniEnv->Get()->CallIntMethod");
//
//    //_NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
//    //_NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacket: before delete");
//    //jjustman-2020-12-17 - TODO - confirm -
//    localJniEnv->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
//    //_NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacket: return");
//
//    if((_NDK_MEDIA_MMT_BRIDGE_atsc3_onMfuPacket_counter++ < 10) || ((_NDK_MEDIA_MMT_BRIDGE_atsc3_onMfuPacket_counter % 1000) == 0)) {
//        _NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacket: Atsc3RingBuffer::RING_BUFFER_PAGE_FRAGMENT, this: %p, service_id: %d, packet_id: %d, mpu_sequence_number: %d, buffer: %p, length: %d, onMfuPacket_counter: %d", this, service_id, packet_id, mpu_sequence_number, buffer, bufferLen, _NDK_MEDIA_MMT_BRIDGE_atsc3_onMfuPacket_counter);
//    }

    //writeToRingBuffer(RING_BUFFER_PAGE_FRAGMENT, packet_id, sample_number, presentationUs, buffer, bufferLen);
    fragmentBuffer->write(Atsc3RingBuffer::RING_BUFFER_PAGE_FRAGMENT, packet_id, sample_number, presentationUs, buffer, bufferLen);
}

//on partially corrupt MFU packet data
void Atsc3NdkMediaMMTBridge::atsc3_onMfuPacketCorrupt(uint16_t service_id, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
//    Atsc3JniEnv* localJniEnv = Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv();
//
//    if (!localJniEnv) {
//        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() is NULL!");
//        return;
//    }

    //writeToRingBuffer(RING_BUFFER_PAGE_FRAGMENT, packet_id, sample_number, presentationUs, buffer, bufferLen);
    fragmentBuffer->write(Atsc3RingBuffer::RING_BUFFER_PAGE_FRAGMENT, packet_id, sample_number, presentationUs, buffer, bufferLen);
}

//on partially corrupt MFU missing MMTHSample header

void Atsc3NdkMediaMMTBridge::atsc3_onMfuPacketCorruptMmthSampleHeader(uint16_t service_id, uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt) {
//    this->pinConsumerThreadAsNeeded(); //jjustman-2020-12-17 - hack
//    Atsc3JniEnv* localJniEnv = Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv();
//
//    if (!localJniEnv) {
//        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() is NULL!");
//        return;
//    }
//
//    //LogMsg("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacketCorruptMmthSampleHeader: before NewDirectByteBuffer");
//    //_NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacketCorruptMmthSampleHeader: packet_id: %d, mpu_sequence_number: %d, buffer: %p, bufferLen: %d", packet_id, mpu_sequence_number, buffer, bufferLen);
//    jobject jobjectLocalByteBuffer = localJniEnv->Get()->NewDirectByteBuffer(buffer, bufferLen);
//    //LogMsg("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacketCorruptMmthSampleHeader: after NewDirectByteBuffer");
//
//    //_NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge::atsc3_onMfuPacketCorruptMmthSampleHeader: jobjectLocalByteBuffer is: %d, jni_instance_globalRef: %p, method: %d, ", jobjectLocalByteBuffer, jni_instance_globalRef, atsc3_onMfuPacketCorruptMmthSampleHeaderID);

    //writeToRingBuffer(RING_BUFFER_PAGE_FRAGMENT, packet_id, sample_number, presentationUs, buffer, bufferLen);
    fragmentBuffer->write(Atsc3RingBuffer::RING_BUFFER_PAGE_FRAGMENT, packet_id, sample_number, presentationUs, buffer, bufferLen);
}

void Atsc3NdkMediaMMTBridge::atsc3_onMfuSampleMissing(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
    this->pinConsumerThreadAsNeeded(); //jjustman-2020-12-17 - hack
    if (!Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() is NULL!");
        return;
    }

     Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv()->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuSampleMissingID, (int32_t)packet_id, (int64_t)mpu_sequence_number, (int32_t)sample_number);
}

//--------------------------------------------------------------------------

void Atsc3NdkMediaMMTBridge::PthreadDestructor(void* prevJniPtr) {
    printf("Atsc3NdkMediaMMTBridge::PthreadDestructor: ptr: %p", prevJniPtr);
}
void Atsc3NdkMediaMMTBridge::CreateJniAndMediaBridgePtrKey() {
    //JniPtr with destructor
    printf("Atsc3NdkMediaMMTBridge::CreateJniAndMediaBridgePtrKey()");

    pthread_key_create(&Atsc3NdkMediaMMTBridge::JniPtr, &Atsc3NdkMediaMMTBridge::PthreadDestructor);
}

// _NDK_MEDIA_MMT_BRIDGE_TRACE("Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_atsc3_1process_1mmtp_1udp_1packet: std::this_thread::get_id(): %zu", std::hash<std::thread::id>{}(std::this_thread::get_id()));
Atsc3NdkMediaMMTBridge* Atsc3NdkMediaMMTBridge::GetMediaBridgePtr(JNIEnv* env, jobject instance) {
    Atsc3NdkMediaMMTBridge* myAtsc3NdkMediaMMTBridge = nullptr;
    map<jobject, Atsc3NdkMediaMMTBridge*>::iterator it;

    for(it = Atsc3NdkMediaMMTBridge::MediaBridgePtrMap.begin(); it != Atsc3NdkMediaMMTBridge::MediaBridgePtrMap.end() && !myAtsc3NdkMediaMMTBridge; it++) {
        if(env->IsSameObject(it->first, instance)) {
            myAtsc3NdkMediaMMTBridge = it->second;
            break;
        }
    }

    return myAtsc3NdkMediaMMTBridge;
}

Atsc3JniEnv* Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv() {
    Atsc3JniEnv* myAtsc3JniEnv = reinterpret_cast<Atsc3JniEnv*>(pthread_getspecific(Atsc3NdkMediaMMTBridge::JniPtr));
    if(!myAtsc3JniEnv) {
        //printf("Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv - myAtsc3JniEnv is NULL!");
    } else {
        //printf("Atsc3NdkMediaMMTBridge::GetBridgeConsumerJniEnv - using myAtsc3JniEnv: %p, JNIEnv: %p", myAtsc3JniEnv, myAtsc3JniEnv->Get());
    }
    return myAtsc3JniEnv;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_init(JNIEnv *env, jobject instance, jobject fragment_buffer, jint max_fragment_count)
{
    //jjustman-2020-12-17 - hack

    Atsc3NdkMediaMMTBridge* mediaMMTBridge = nullptr;

    pthread_once(&Atsc3NdkMediaMMTBridge::JniPtrOnce, &Atsc3NdkMediaMMTBridge::CreateJniAndMediaBridgePtrKey);

    mediaMMTBridge = new Atsc3NdkMediaMMTBridge(env, instance, fragment_buffer, max_fragment_count);
    Atsc3NdkMediaMMTBridge::MediaBridgePtrMap.insert(pair<jobject, Atsc3NdkMediaMMTBridge*>(mediaMMTBridge->jni_instance_globalRef, mediaMMTBridge));

    _NDK_MEDIA_MMT_BRIDGE_INFO("Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_init:  env: %p", env);
    mediaMMTBridge->setJniClassReference(env, "org/ngbp/libatsc3/middleware/Atsc3NdkMediaMMTBridge");
    mediaMMTBridge->mJavaVM = atsc3_ndk_media_mmt_bridge_static_loader_get_javaVM();

    if(mediaMMTBridge->mJavaVM == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: mediaMMTBridge->mJavaVM is NULL!");
        return -1;
    }

    jclass jniClassReference = mediaMMTBridge->getJniClassReference();

    if (jniClassReference == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find Atsc3NdkMediaMMTBridge java class reference!");
        return -2;
    }

    mediaMMTBridge->mOnLogMsgId = env->GetMethodID(jniClassReference, "onLogMsg", "(Ljava/lang/String;)I");
    if (mediaMMTBridge->mOnLogMsgId == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("AppBridge_init - cannot find 'onLogMsg' method id");
        return -1;
    }

    //MMT Initialization callbacks for Video and Audio format(s)

    mediaMMTBridge->atsc3_OnInitHEVC_NAL_Extracted = env->GetMethodID(jniClassReference, "atsc3_onInitHEVC_NAL_Packet", "(IJLjava/nio/ByteBuffer;I)I");
    if (mediaMMTBridge->atsc3_OnInitHEVC_NAL_Extracted == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_onInitHEVC_NAL_Packet' method id");
        return -1;
    }

    mediaMMTBridge->atsc3_OnInitAudioDecoderConfigurationRecord = env->GetMethodID(jniClassReference, "atsc3_OnInitAudioDecoderConfigurationRecord", "(IJLorg/ngbp/libatsc3/middleware/android/mmt/models/MMTAudioDecoderConfigurationRecord;)I");
    if (mediaMMTBridge->atsc3_OnInitAudioDecoderConfigurationRecord == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_OnInitAudioDecoderConfigurationRecord' method id");
        return -1;
    }

    //capture our MMTAudioDecoderConfigurationRecord for marshalling from _t to java class
    jclass mmtAudioDecoderConfigurationRecord_jclass_init_env = env->FindClass("org/ngbp/libatsc3/middleware/android/mmt/models/MMTAudioDecoderConfigurationRecord");
    if(!mmtAudioDecoderConfigurationRecord_jclass_init_env) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'mmtAudioDecoderConfigurationRecord_jclass' class reference");
        return -1;
    } else {
        mediaMMTBridge->mmtAudioDecoderConfigurationRecord_jclass_global_ref = (jclass)(env->NewGlobalRef(mmtAudioDecoderConfigurationRecord_jclass_init_env));
    }

    //MMTAudioDecoderConfigurationRecord$AudioAC4SampleEntryBox
    jclass mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_jclass_init_env = env->FindClass("org/ngbp/libatsc3/middleware/android/mmt/models/MMTAudioDecoderConfigurationRecord$AudioAC4SampleEntryBox");
    if(!mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_jclass_init_env) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_jclass' class reference");
        return -1;
    } else {
        mediaMMTBridge->mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_jclass_global_ref = (jclass)(env->NewGlobalRef(mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_jclass_init_env));
    }

    //MMTAudioDecoderConfigurationRecord$AudioAC4SampleEntryBox$AC4SpecificBox
    jclass mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_AC4SpecificBox_jclass_init_env = env->FindClass("org/ngbp/libatsc3/middleware/android/mmt/models/MMTAudioDecoderConfigurationRecord$AudioAC4SampleEntryBox$AC4SpecificBox");
    if(!mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_AC4SpecificBox_jclass_init_env) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_AC4SpecificBox_jclass' class reference");
        return -1;
    } else {
        mediaMMTBridge->mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_AC4SpecificBox_jclass_global_ref = (jclass)(env->NewGlobalRef(mmtAudioDecoderConfigurationRecord_AudioAC4SampleEntryBox_AC4SpecificBox_jclass_init_env));
    }


    //Signalling callbacks

    /*
     *  public int atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(int video_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds);
     *  public int atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds);
     *  public int atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds) {
    */

    mediaMMTBridge->atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jniClassReference, "atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor", "(IJJJI)I");
    if (mediaMMTBridge->atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID' method id");
        return -1;
    }
    mediaMMTBridge->atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jniClassReference, "atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor", "(IJJJI)I");
    if (mediaMMTBridge->atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID' method id");
        return -1;
    }
    mediaMMTBridge->atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jniClassReference, "atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor", "(IJJJI)I");
    if (mediaMMTBridge->atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID' method id");
        return -1;
    }

    //Fragment Metadata callbacks
    //atsc3_onExtractedSampleDurationID
    mediaMMTBridge->atsc3_onExtractedSampleDurationID = env->GetMethodID(jniClassReference, "atsc3_onExtractedSampleDuration", "(IJJ)I");
    if (mediaMMTBridge->atsc3_onExtractedSampleDurationID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_onExtractedSampleDurationID' method id");
        return -1;
    }

    //atsc3_setVideoWidthHeightFromTrakID
    mediaMMTBridge->atsc3_setVideoWidthHeightFromTrakID = env->GetMethodID(jniClassReference, "atsc3_setVideoWidthHeightFromTrak", "(III)I");
    if (mediaMMTBridge->atsc3_setVideoWidthHeightFromTrakID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_setVideoWidthHeightFromTrakID' method id");
        return -1;
    }

    //MFU callbacks
    //java.nio.ByteBuffer, L: fully qualified class, J: long
    mediaMMTBridge->atsc3_onMfuPacketID = env->GetMethodID(jniClassReference, "atsc3_onMfuPacket", "(IJILjava/nio/ByteBuffer;IJI)I");
    if (mediaMMTBridge->atsc3_onMfuPacketID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_onMfuPacket' method id");
        return -1;
    }
    //java.nio.ByteBuffer, L: fully qualified class, J: long
    mediaMMTBridge->atsc3_onMfuPacketCorruptID = env->GetMethodID(jniClassReference, "atsc3_onMfuPacketCorrupt", "(IJILjava/nio/ByteBuffer;IJII)I");
    if (mediaMMTBridge->atsc3_onMfuPacketCorruptID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("AppBridge_init - Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_onMfuPacketCorrupt' method id");
        return -1;
    }

    //java.nio.ByteBuffer, L: fully qualified class, J: long
    mediaMMTBridge->atsc3_onMfuPacketCorruptMmthSampleHeaderID = env->GetMethodID(jniClassReference, "atsc3_onMfuPacketCorruptMmthSampleHeader", "(IJILjava/nio/ByteBuffer;IJII)I");
    if (mediaMMTBridge->atsc3_onMfuPacketCorruptMmthSampleHeaderID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_onMfuPacketCorruptMmthSampleHeaderID' method id");
        return -1;
    }

    //java.nio.ByteBuffer, L: fully qualified class, J: long
    mediaMMTBridge->atsc3_onMfuSampleMissingID = env->GetMethodID(jniClassReference, "atsc3_onMfuSampleMissing", "(IJI)I");
    if (mediaMMTBridge->atsc3_onMfuSampleMissingID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_onMfuSampleMissingID' method id");
        return -1;
    }

    atsc3_ndk_media_mmt_bridge_init(mediaMMTBridge);
    _NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkMediaMMTBridge_init: done, with mediaMMTBridge: %p", mediaMMTBridge);

    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_atsc3_1process_1mmtp_1udp_1packet(JNIEnv *env, jobject instance, jobject byte_buffer, jint length) {
    int ret = -1;
    _NDK_MEDIA_MMT_BRIDGE_ERROR("Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_atsc3_1process_1mmtp_1udp_1packet: ENTER jnienv: %p", env);

    Atsc3NdkMediaMMTBridge* mediaMMTBridge = Atsc3NdkMediaMMTBridge::GetMediaBridgePtr(env, instance);

    if(!mediaMMTBridge) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_atsc3_1process_1mmtp_1udp_1packet: mediaMMTBridge is NULL, discarding udp packet!");
        return -2;
    }

    mediaMMTBridge->pinConsumerThreadAsNeeded();
    ret = mediaMMTBridge->acceptNdkByteBufferUdpPacket(byte_buffer, length);

    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_release(JNIEnv *env, jobject instance) {

    Atsc3NdkMediaMMTBridge* mediaMMTBridge = Atsc3NdkMediaMMTBridge::GetMediaBridgePtr(env, instance);

    _NDK_MEDIA_MMT_BRIDGE_ERROR("Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_release: ENTER jnienv: %p, mediaMMTBridge: %p", env, mediaMMTBridge);
    if(!mediaMMTBridge) {
        printf("Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_release: mediaMMTBridge is NULL!");
        return;
    }

    if(mediaMMTBridge) {
        mediaMMTBridge->releasePinnedConsumerThreadAsNeeded();
    }

    //jjustman-2020-12-16 - TODO: delete mediaMMTBridge; (and block_destory(preAllocInFlightUdpPacket))
    return;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_getFragmentBufferCurrentPosition(JNIEnv *env, jobject thiz) {
    Atsc3NdkMediaMMTBridge* mediaMMTBridge = Atsc3NdkMediaMMTBridge::GetMediaBridgePtr(env, thiz);

    return mediaMMTBridge->getFragmentBufferCurrentPosition();
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_getFragmentBufferCurrentPageNumber(JNIEnv *env, jobject thiz) {
    Atsc3NdkMediaMMTBridge* mediaMMTBridge = Atsc3NdkMediaMMTBridge::GetMediaBridgePtr(env, thiz);

    return mediaMMTBridge->getFragmentBufferCurrentPageNumber();
}