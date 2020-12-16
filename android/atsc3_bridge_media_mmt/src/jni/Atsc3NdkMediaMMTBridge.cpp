#include "Atsc3NdkMediaMMTBridge.h"

Atsc3NdkMediaMMTBridge* mediaMMTBridge;

Atsc3NdkMediaMMTBridge::Atsc3NdkMediaMMTBridge(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = env->NewGlobalRef(jni_instance);

    mmtExtractor = new MMTExtractor();
}

//jni management
int Atsc3NdkMediaMMTBridge::pinConsumerThreadAsNeeded() {
    _NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkMediaMMTBridge::pinConsumerThreadAsNeeded: mJavaVM: %p", mJavaVM);
    bridgeConsumerJniEnv = new Atsc3JniEnv(mJavaVM);
    return 0;
}

int Atsc3NdkMediaMMTBridge::releasePinnedConsumerThreadAsNeeded() {
    _NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkMediaMMTBridge::releasePinnedConsumerThreadAsNeeded: bridgeConsumerJniEnv is: %p:", bridgeConsumerJniEnv);
    if(bridgeConsumerJniEnv) {
        delete bridgeConsumerJniEnv;
    }
    return 0;
}

bool Atsc3NdkMediaMMTBridge::isConsumerThreadPinned() {
    return bridgeConsumerJniEnv != NULL;
}

void Atsc3NdkMediaMMTBridge::LogMsg(const char *msg)
{
    if (!mJavaVM) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("LogMsg: mJavaVM is NULL!");
        return;
    }

    Atsc3JniEnv env(mJavaVM);
    if (!env) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("LogMsg: error creating env pin!");
        return;
    }
    jstring js = env.Get()->NewStringUTF(msg);
    int r = env.Get()->CallIntMethod(jni_instance_globalRef, mOnLogMsgId, js);
    env.Get()->DeleteLocalRef(js);
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

//push extracted HEVC nal's to MediaCodec for init
void Atsc3NdkMediaMMTBridge::atsc3_onInitHEVC_NAL_Extracted(uint16_t packet_id, uint32_t mpu_sequence_number, uint8_t* buffer, uint32_t bufferLen) {
    if (!bridgeConsumerJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__

    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_nal_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(jni_instance_globalRef, mOnInitHEVC_NAL_Extracted, is_video, mpu_sequence_number, jobjectGlobalByteBuffer, bufferLen);
    //_NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    //env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectByteBuffer = bridgeConsumerJniEnv->Get()->NewDirectByteBuffer(buffer, bufferLen);
    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, mOnInitHEVC_NAL_Extracted, packet_id, mpu_sequence_number, jobjectByteBuffer, bufferLen);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(jobjectByteBuffer);
#endif
}

//MMT Signalling callbacks
void Atsc3NdkMediaMMTBridge::atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    if (!bridgeConsumerJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID, video_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

void Atsc3NdkMediaMMTBridge::atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!bridgeConsumerJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

    //jjustman-2020-11-19 - HACK for testing
    if(audio_packet_id != 200) {
        return;
    }

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID, audio_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

void Atsc3NdkMediaMMTBridge::atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    if (!bridgeConsumerJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

    int r =  bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID, stpp_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
}

//MFU metadata for sample duration
void Atsc3NdkMediaMMTBridge::atsc3_onExtractedSampleDuration(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t extracted_sample_duration_us) {

    if (!bridgeConsumerJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }
    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onExtractedSampleDurationID,
                                                       packet_id,
                                                       mpu_sequence_number,
                                                       extracted_sample_duration_us);
}


//video w/h for rendering
void Atsc3NdkMediaMMTBridge::atsc3_setVideoWidthHeightFromTrak(uint32_t width, uint32_t height) {

    if (!bridgeConsumerJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }
    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_setVideoWidthHeightFromTrakID, width, height);
}

//on fully recovered MFU packet
void Atsc3NdkMediaMMTBridge::atsc3_onMfuPacket(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_rebuilt)
{
    if (!bridgeConsumerJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //_NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = bridgeConsumerJniEnv->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_rebuilt);
    //_NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}

//on partially corrupt MFU packet data
void Atsc3NdkMediaMMTBridge::atsc3_onMfuPacketCorrupt(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt)
{
    if (!bridgeConsumerJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //_NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = bridgeConsumerJniEnv->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketCorruptID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    //_NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}

//on partially corrupt MFU missing MMTHSample header

void Atsc3NdkMediaMMTBridge::atsc3_onMfuPacketCorruptMmthSampleHeader(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt)
{
    if (!bridgeConsumerJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //_NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = bridgeConsumerJniEnv->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuPacketCorruptMmthSampleHeaderID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    //_NDK_MEDIA_MMT_BRIDGE_INFO("Atsc3NdkPHYBridge::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    bridgeConsumerJniEnv->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}

void Atsc3NdkMediaMMTBridge::atsc3_onMfuSampleMissing(uint16_t pcaket_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
    if (!bridgeConsumerJniEnv) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("ats3_onMfuPacket: bridgeConsumerJniEnv is NULL!");
        return;
    }

    int r = bridgeConsumerJniEnv->Get()->CallIntMethod(jni_instance_globalRef, atsc3_onMfuSampleMissingID, pcaket_id, mpu_sequence_number, sample_number);
}

void Atsc3NdkMediaMMTBridge::atsc3_extractUdpPacket(block_t* packet) {
    if (!mmtExtractor) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("atsc3_extractUdpPacket: mmtExtractor is NULL!");
        return;
    }

    mmtExtractor->atsc3_core_service_bridge_process_mmt_packet(packet);
}

//--------------------------------------------------------------------------

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_init(JNIEnv *env, jobject instance)
{
    mediaMMTBridge = new Atsc3NdkMediaMMTBridge(env, instance);
    _NDK_MEDIA_MMT_BRIDGE_INFO("Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_init:  env: %p", env);
    mediaMMTBridge->setJniClassReference("org/ngbp/libatsc3/middleware/Atsc3NdkMediaMMTBridge");
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

    //java.nio.ByteBuffer
    mediaMMTBridge->mOnInitHEVC_NAL_Extracted = env->GetMethodID(jniClassReference, "atsc3_onInitHEVC_NAL_Packet", "(IILjava/nio/ByteBuffer;I)I");
    if (mediaMMTBridge->mOnInitHEVC_NAL_Extracted == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_onInitHEVC_NAL_Packet' method id");
        return -1;
    }

    /*
     *  public int atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(int video_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds);
     *  public int atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds);
     *  public int atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds) {
    */

    mediaMMTBridge->atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jniClassReference, "atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor", "(IIJJI)I");
    if (mediaMMTBridge->atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID' method id");
        return -1;
    }
    mediaMMTBridge->atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jniClassReference, "atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor", "(IIJJI)I");
    if (mediaMMTBridge->atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID' method id");
        return -1;
    }
    mediaMMTBridge->atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jniClassReference, "atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor", "(IIJJI)I");
    if (mediaMMTBridge->atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID' method id");
        return -1;
    }

    //atsc3_onExtractedSampleDurationID
    mediaMMTBridge->atsc3_onExtractedSampleDurationID = env->GetMethodID(jniClassReference, "atsc3_onExtractedSampleDuration", "(III)I");
    if (mediaMMTBridge->atsc3_onExtractedSampleDurationID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_onExtractedSampleDurationID' method id");
        return -1;
    }

    //atsc3_setVideoWidthHeightFromTrakID
    mediaMMTBridge->atsc3_setVideoWidthHeightFromTrakID = env->GetMethodID(jniClassReference, "atsc3_setVideoWidthHeightFromTrak", "(II)I");
    if (mediaMMTBridge->atsc3_setVideoWidthHeightFromTrakID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_setVideoWidthHeightFromTrakID' method id");
        return -1;
    }

    //java.nio.ByteBuffer, L: fully qualified class, J: long
    mediaMMTBridge->atsc3_onMfuPacketID = env->GetMethodID(jniClassReference, "atsc3_onMfuPacket", "(IIILjava/nio/ByteBuffer;IJI)I");
    if (mediaMMTBridge->atsc3_onMfuPacketID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_onMfuPacket' method id");
        return -1;
    }
    //java.nio.ByteBuffer, L: fully qualified class, J: long
    mediaMMTBridge->atsc3_onMfuPacketCorruptID = env->GetMethodID(jniClassReference, "atsc3_onMfuPacketCorrupt", "(IIILjava/nio/ByteBuffer;IJII)I");
    if (mediaMMTBridge->atsc3_onMfuPacketCorruptID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("AppBridge_init - Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_onMfuPacketCorrupt' method id");
        return -1;
    }

    //java.nio.ByteBuffer, L: fully qualified class, J: long
    mediaMMTBridge->atsc3_onMfuPacketCorruptMmthSampleHeaderID = env->GetMethodID(jniClassReference, "atsc3_onMfuPacketCorruptMmthSampleHeader", "(IIILjava/nio/ByteBuffer;IJII)I");
    if (mediaMMTBridge->atsc3_onMfuPacketCorruptMmthSampleHeaderID == NULL) {
        _NDK_MEDIA_MMT_BRIDGE_ERROR("Atsc3NdkMediaMMTBridge_init: cannot find 'atsc3_onMfuPacketCorruptMmthSampleHeaderID' method id");
        return -1;
    }

    //java.nio.ByteBuffer, L: fully qualified class, J: long
    mediaMMTBridge->atsc3_onMfuSampleMissingID = env->GetMethodID(jniClassReference, "atsc3_onMfuSampleMissing", "(III)I");
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
Java_org_ngbp_libatsc3_middleware_Atsc3NdkMediaMMTBridge_atsc3_1process_1mmtp_1udp_1packet(
        JNIEnv *env, jobject thiz, jobject byte_buffer, jint length) {

    //TODO: this should not be here
    if (!mediaMMTBridge->isConsumerThreadPinned()) {
        mediaMMTBridge->pinConsumerThreadAsNeeded();
    }

    uint8_t* buffer = reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(byte_buffer));
    jlong size = env->GetDirectBufferCapacity(byte_buffer);
    if (buffer) {
        block_t* packet = block_Alloc(length);
        block_Write(packet, buffer, length);
        block_Rewind(packet);
        mediaMMTBridge->atsc3_extractUdpPacket(packet);
        block_Destroy(&packet);
        return 0;
    }

    return -1;
}
