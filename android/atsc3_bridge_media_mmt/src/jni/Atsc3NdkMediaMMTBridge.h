// Atsc3NdkMediaMMTBridge
// Created by Jason Justman on 2019-09-27.
//

#include <jni.h>
#include <string.h>
#include <thread>
#include <map>
#include <queue>
#include <vector>
#include <mutex>
#include <semaphore.h>
#include <list>
#include <sys/types.h>

using namespace std;

#ifndef LIBATSC3_ATSC3_NDK_MEDIA_MMT_BRIDGE_H
#define LIBATSC3_ATSC3_NDK_MEDIA_MMT_BRIDGE_H

#define MODULE_NAME "Atsc3NdkApplicationBridge"

#include "Atsc3LoggingUtils.h"
#include "Atsc3JniEnv.h"

// libatsc3 type imports here
#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>
#include <atsc3_lls_types.h>
#include <atsc3_alc_rx.h>
#include <atsc3_pcap_type.h>
#include <atsc3_monitor_events_alc.h>
#include <atsc3_route_package_utils.h>
#include <application/IAtsc3NdkMediaMMTBridge.h>

#include "Atsc3NdkMediaMMTBridgeStaticJniLoader.h"

#include <atsc3_mmt_mfu_context_callbacks_default_jni.h>

class Atsc3NdkMediaMMTBridge : public IAtsc3NdkMediaMMTBridge
{
public:
    Atsc3NdkMediaMMTBridge(JNIEnv* env, jobject jni_instance);

    //logging
    void LogMsg(const char *msg);
    void LogMsg(const std::string &msg);
    void LogMsgF(const char *fmt, ...);

    /* Media MMT Bridge callbacks for ExoPlayer JNI handoff */
    void atsc3_onInitHEVC_NAL_Extracted(uint16_t packet_id, uint32_t mpu_sequence_number, uint8_t* buffer, uint32_t bufferLen);

    void atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
    void atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microsecond);
    void atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);

    void atsc3_onExtractedSampleDuration(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t extracted_sample_duration_us);
    void atsc3_setVideoWidthHeightFromTrak(uint32_t width, uint32_t height);

    void atsc3_onMfuPacket(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected);
    void atsc3_onMfuPacketCorrupt(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);
    void atsc3_onMfuPacketCorruptMmthSampleHeader(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);

    void atsc3_onMfuSampleMissing(uint16_t i, uint32_t i1, uint32_t i2);

private:
    JNIEnv* env = nullptr;
    jobject jni_instance_globalRef = nullptr;
    jclass jni_class_globalRef = nullptr;

    std::thread mhRxThread;

    //global env.Get()->NewGlobalRef(jobjectByteBuffer); for c alloc'd MFU's and NAL's
    std::vector<jobject> global_jobject_mfu_refs;
    std::vector<jobject> global_jobject_nal_refs;

public:
    JavaVM* mJavaVM = nullptr;    // Java VM, if we don't have a pinned thread context for dispatch

    void setJniClassReference(string jclass_name) {
        if(env) {
            jclass jclass_local = env->FindClass(jclass_name.c_str());
            jni_class_globalRef = reinterpret_cast<jclass>(env->NewGlobalRef(jclass_local));
        }
    }

    jclass getJniClassReference() {
        return jni_class_globalRef;
    }

    int pinConsumerThreadAsNeeded();
    int releasePinnedConsumerThreadAsNeeded();

    jmethodID mOnLogMsgId = nullptr;

    jmethodID mOnInitHEVC_NAL_Extracted = nullptr; //java method for pushing to a/v codec buffers

    jmethodID atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID = nullptr;  // java class method id
    jmethodID atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID = nullptr;  // java class method id
    jmethodID atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID = nullptr;  // java class method id

    jmethodID atsc3_onExtractedSampleDurationID = nullptr;
    jmethodID atsc3_setVideoWidthHeightFromTrakID = nullptr;

    jmethodID atsc3_onMfuPacketID = nullptr; //java method for pushing to a/v codec buffers
    jmethodID atsc3_onMfuPacketCorruptID = nullptr; //java method for pushing to a/v codec buffers
    jmethodID atsc3_onMfuPacketCorruptMmthSampleHeaderID = nullptr; //java method for pushing to a/v codec buffers

    jmethodID atsc3_onMfuSampleMissingID = nullptr;


    //todo: refactor this out - ala https://gist.github.com/qiao-tw/6e43fb2311ee3c31752e11a4415deeb1
    jclass      jni_java_util_ArrayList = nullptr;
    jmethodID   jni_java_util_ArrayList_cctor = nullptr;
    jmethodID   jni_java_util_ArrayList_add = nullptr;


protected:
    Atsc3JniEnv* bridgeConsumerJniEnv = nullptr;
};

#define _NDK_MEDIA_MMT_BRIDGE_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _NDK_MEDIA_MMT_BRIDGE_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _NDK_MEDIA_MMT_BRIDGE_INFO(...)   	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _NDK_MEDIA_MMT_BRIDGE_DEBUG(...)   	__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define _NDK_MEDIA_MMT_BRIDGE_TRACE(...)   	__LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);

#endif //LIBATSC3_ATSC3_NDK_MEDIA_MMT_BRIDGE_H
