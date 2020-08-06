#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <atsc3_lls_types.h>
#include <atsc3_phy_mmt_player_bridge.h>
#include <atsc3_pcap_type.h>
#include <atsc3_monitor_events_alc.h>
#include "atsc3NdkClient.h"
#include "atsc3NdkClientNoPhyImpl.h"


#if DEBUG
	#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, MODULE_NAME, __VA_ARGS__)
	#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , MODULE_NAME, __VA_ARGS__)
	#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , MODULE_NAME, __VA_ARGS__)
	#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , MODULE_NAME, __VA_ARGS__)
	#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , MODULE_NAME, __VA_ARGS__)
#else
#define LOGV(...)
	#define LOGD(...)
	#define LOGI(...)
	#define LOGW(...)
	#define LOGE(...)
#endif

#define printf LOGD
#define eprintf LOGE

#define ASSERT(cond,s) do { \
        if (!(cond)) { eprintf("%s: !! %s assert fail, line %d\n", __func__, s, __LINE__); \
            return -1; } \
        } while(0)

#define CHK_AR(ar,s) do { \
		if (ar) { eprintf("%s: !! %s, err %d, line %d\n", __func__, s, ar, __LINE__); \
			return -1; } \
		} while(0)
#define SHOW_AR(ar,s) do { \
		if (ar) { printf("%s: !! %s, err %d, line %d\n", __func__, s, ar, __LINE__); } \
		} while(0)


using namespace std;
vector<string> Split(const char *str, char delimiter = ' ') {
    vector<string> vs;
    if (!str) return vs;
    do {
        const char *begin = str;
        while(*str != delimiter && *str)
            str++;
        vs.push_back(string(begin, str));
    } while (0 != *str++);
    return vs;
}

atsc3NdkClient api;
atsc3NdkClientNoPhyImpl apiImpl;

int atsc3NdkClient::Init()
{
    printf("%s:%s:TODO", __FILE__, __func__);

    mbInit = true;
    return 0;
}

int atsc3NdkClient::Prepare(const char *strDevListInfo, int delim1, int delim2)
{
    // format example:  delim1 is colon, delim2 is comma
    // "/dev/bus/usb/001/001:21,/dev/bus/usb/001/002:22"


    return 0;
}
/*
 * Open ... dongle device
 * note: target device need to be populated before calling this api
 *
 * https://github.com/libusb/libusb/pull/242
 */

/** jjustman-2019-11-08 - todo: fix for double app launch */
int atsc3NdkClient::Open(int fd, int bus, int addr)
{
    apiImpl.Init(this);
    apiImpl.Open(fd, bus, addr);
    return 0;
}

int atsc3NdkClient::atsc3_rx_callback_f(void* pData, uint64_t ullUser)
{
//    atsc3NdkClient *me = (atsc3NdkClient *)ullUser; // same as &api
//    return me->RxCallbackJJ(pData);
    return 0;
}


//used for inclusion of pcap's via android assetManager

int atsc3NdkClient::atsc3_pcap_replay_open_file_from_assetManager(const char *filename,
                                                              AAssetManager *mgr) {

    if(!filename) {
        return -1;
    }

    if(pcapThreadShouldRun) {
        //shutdown and restart
        atsc3_pcap_thread_stop();
    }

    LogMsgF("atsc3NdkClient::atsc3_pcap_open_for_replay_from_assetManager: filename: %s, aasetManager: %p", filename, mgr);
    pcap_replay_filename = (char*)calloc(strlen(filename)+1, sizeof(char));
    strncpy(pcap_replay_filename, filename, strlen(filename));

    pcap_replay_asset_ref_ptr = AAssetManager_open(mgr, (const char *) pcap_replay_filename, AASSET_MODE_UNKNOWN);
    if (NULL == pcap_replay_asset_ref_ptr) {
        __android_log_print(ANDROID_LOG_ERROR, "atsc3_pcap_open_for_replay_from_assetManager", "_ASSET_NOT_FOUND_");
        return JNI_FALSE;
    }

    off_t pcap_start = 0, pcap_length = 0;

    int pcap_fd = AAsset_openFileDescriptor(pcap_replay_asset_ref_ptr, &pcap_start, &pcap_length);

    //set this.atsc3_pcap_replay_context

    atsc3_pcap_replay_context = atsc3_pcap_replay_open_from_fd(pcap_replay_filename, pcap_fd, pcap_start, pcap_length);
    if(!atsc3_pcap_replay_context) {
        return -1;
    }

    return 0;
}

int atsc3NdkClient::atsc3_pcap_replay_open_file(const char *filename) {

    atsc3_pcap_replay_context = atsc3_pcap_replay_open_filename(filename);
    LogMsgF("atsc3NdkClient::atsc3_pcap_replay_open_file: file: %s, replay context: %p", filename, atsc3_pcap_replay_context);
    if(!atsc3_pcap_replay_context) {
        return -1;
    }
    return 0;
}


/**
 * TODO:  jjustman-2019-10-10: implement pcap replay in new superclass
 *         -D__MOCK_PCAP_REPLAY__ in the interim
 *
 * @return 0
 *
 *
 * borrowed from libatsc3/test/atsc3_pcap_replay_test.c
 */

int atsc3NdkClient::PcapProducerThreadParserRun() {

    int packet_push_count = 0;

    LogMsgF("atsc3NdkClient::PcapProducerThreadParserRun with this: %p", this);

    if(!atsc3_pcap_replay_context) {
        LogMsgF("atsc3NdkClient::PcapProducerThreadParserRun - ERROR - no atsc3_pcap_replay_context!");
        pcapThreadShouldRun = false;
        return -1;
    }

    atsc3_pcap_replay_context_t* atsc3_pcap_replay_local_context = atsc3_pcap_replay_context;
    while (pcapThreadShouldRun) {
        queue<block_t *> to_dispatch_queue; //perform a shallow copy so we can exit critical section asap

        //_ATSC3_PCAP_REPLAY_TEST_DEBUG("Opening pcap: %s, context is: %p", PCAP_REPLAY_TEST_FILENAME, atsc3_pcap_replay_local_context);
        if(atsc3_pcap_replay_local_context) {
            while(pcapThreadShouldRun && (atsc3_pcap_replay_local_context = atsc3_pcap_replay_iterate_packet(atsc3_pcap_replay_local_context))) {
                atsc3_pcap_replay_usleep_packet(atsc3_pcap_replay_local_context);
                //push block_t as packet buffer to consumer queue

                block_Seek(atsc3_pcap_replay_local_context->atsc3_pcap_packet_instance.current_pcap_packet, ATSC3_PCAP_ETH_HEADER_LENGTH);

                block_t* phy_payload = block_Duplicate_from_position(atsc3_pcap_replay_local_context->atsc3_pcap_packet_instance.current_pcap_packet);
                block_Rewind(atsc3_pcap_replay_local_context->atsc3_pcap_packet_instance.current_pcap_packet);
                if(phy_payload->p_size && (packet_push_count++ % 10000) == 0) {
                    LogMsgF("atsc3NdkClient::RunPcapThreadParser - pushing to atsc3_phy_mmt_player_bridge_process_packet_phy: count: %d, len was: %d, new payload: %p (0x%02x 0x%02x), len: %d",
                            packet_push_count,
                            atsc3_pcap_replay_local_context->atsc3_pcap_packet_instance.current_pcap_packet->p_size,
                            phy_payload,
                            phy_payload->p_buffer[0], phy_payload->p_buffer[1],
                            phy_payload->p_size);
                }

                if(phy_payload->p_size) {
                    to_dispatch_queue.push(phy_payload);
                }

                if(!atsc3_pcap_replay_local_context->delay_delta_behind_rt_replay || to_dispatch_queue.size() > 10) { //pcap_replay_buffer_queue.size() doesn't seem to be accurate...
                    int pushed_count = to_dispatch_queue.size();
                    lock_guard<mutex> pcap_replay_buffer_queue_guard(pcap_replay_buffer_queue_mutex);
                    while(to_dispatch_queue.size()) {
                        pcap_replay_buffer_queue.push(to_dispatch_queue.front());
                        to_dispatch_queue.pop();
                    }
                    pcap_replay_condition.notify_one();  //todo: jjustman-2019-11-06 - only signal if we aren't behind packet processing or we have a growing queue
                    //printf("atsc3NdkClient::PcapProducerThreadRun - signalling notify_one at count: %d", pushed_count);
                }
                //cleanup happens on the consumer thread for dispatching
            }
        }
    }

    //unlock our consumer thread
    if(!pcapThreadShouldRun) {
        lock_guard<mutex> pcap_replay_buffer_queue_guard(pcap_replay_buffer_queue_mutex);
        pcap_replay_condition.notify_one();
    }

    if(!atsc3_pcap_replay_local_context) {
        LogMsgF("atsc3NdkClient::RunPcapThreadParser - unwinding thread, end of file!");
    } else {
        LogMsgF("atsc3NdkClient::RunPcapThreadParser - unwinding thread, pcapThreadShouldRun is false");
    }

    pcapProducerShutdown = true;

    //thread unwound here
    return 0;
}



int atsc3NdkClient::PcapConsumerThreadRun() {


    while (pcapThreadShouldRun) {
        queue<block_t *> to_dispatch_queue; //perform a shallow copy so we can exit critical section asap
        {
            //critical section
            unique_lock<mutex> condition_lock(pcap_replay_buffer_queue_mutex);
            pcap_replay_condition.wait(condition_lock);

            while (pcap_replay_buffer_queue.size()) {
                to_dispatch_queue.push(pcap_replay_buffer_queue.front());
                pcap_replay_buffer_queue.pop();
            }
            condition_lock.unlock();
            pcap_replay_condition.notify_one();
        }

        //printf("atsc3NdkClient::PcapConsumerThreadRun - pushing %d packets", to_dispatch_queue.size());
        while(to_dispatch_queue.size()) {
            block_t *phy_payload_to_process = to_dispatch_queue.front();
            //jjustman-2019-11-06 moved  to semaphore producer/consumer thread for processing pcap replay in time-sensitive phy simulation
            atsc3_phy_mmt_player_bridge_process_packet_phy(phy_payload_to_process);

            to_dispatch_queue.pop();
            block_Destroy(&phy_payload_to_process);
        }
    }
    pcapConsumerShutdown = true;

    return 0;
}

int atsc3NdkClient::RxThread()
{

    return 0;


}

int atsc3NdkClient::Tune(int freqKHz, int plpid)
{
    apiImpl.Tune(freqKHz, plpid);

    return 0;
}

int atsc3NdkClient::Stop()
{

    return 0;
}

int atsc3NdkClient::Reset()
{

    return 0;
}

int atsc3NdkClient::Close()
{

    return 0;
}

int atsc3NdkClient::Uninit()
{

    return 0;
}



void atsc3NdkClient::atsc3_onMfuPacket(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_rebuilt)
{
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //printf("atsc3NdkClient::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = Atsc3_Jni_Processing_Thread_Env->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_rebuilt);
    //printf("atsc3NdkClient::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}


void atsc3NdkClient::atsc3_onMfuPacketCorrupt(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt)
{
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //printf("atsc3NdkClient::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = Atsc3_Jni_Processing_Thread_Env->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketCorruptID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    //printf("atsc3NdkClient::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}


void atsc3NdkClient::atsc3_onMfuPacketCorruptMmthSampleHeader(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, uint8_t* buffer, uint32_t bufferLen, uint64_t presentationUs, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt)
{
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__
    //jobject NewGlobalRef(JNIEnv *env, jobject obj);
    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_mfu_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketID, mpu_sequence_number, is_video, sample_number, jobjectGlobalByteBuffer, bufferLen, presentationUs);
    //printf("atsc3NdkClient::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   // env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectLocalByteBuffer = Atsc3_Jni_Processing_Thread_Env->Get()->NewDirectByteBuffer(buffer, bufferLen);

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuPacketCorruptMmthSampleHeaderID, packet_id, mpu_sequence_number, sample_number, jobjectLocalByteBuffer, bufferLen, presentationUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);
    //printf("atsc3NdkClient::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(jobjectLocalByteBuffer);
#endif
}

//push extracted HEVC nal's to MediaCodec for init
void atsc3NdkClient::atsc3_onInitHEVC_NAL_Extracted(uint16_t packet_id, uint32_t mpu_sequence_number, uint8_t* buffer, uint32_t bufferLen) {
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

#ifdef __JOBJECT_BYTE_BUFFER_GLOBAL_REF__

    jobject jobjectByteBuffer = env.Get()->NewDirectByteBuffer(buffer, bufferLen);
    jobject jobjectGlobalByteBuffer = env.Get()->NewGlobalRef(jobjectByteBuffer);
    global_jobject_nal_refs.push_back(jobjectGlobalByteBuffer);

    int r = env.Get()->CallIntMethod(mClsDrvIntf, mOnInitHEVC_NAL_Extracted, is_video, mpu_sequence_number, jobjectGlobalByteBuffer, bufferLen);
    //printf("atsc3NdkClient::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    //env.Get()->DeleteLocalRef(jobjectByteBuffer);
#else
    jobject jobjectByteBuffer = Atsc3_Jni_Processing_Thread_Env->Get()->NewDirectByteBuffer(buffer, bufferLen);
    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, mOnInitHEVC_NAL_Extracted, packet_id, mpu_sequence_number, jobjectByteBuffer, bufferLen);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(jobjectByteBuffer);
#endif
}


/**
 *  void atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
    void atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microsecond);
    void atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
 * @param msg
 */


//todo: jjustman-2019-10-20: fixme env should be thread-bound already...
void atsc3NdkClient::atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID, video_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
    //printf("atsc3NdkClient::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
   //env.Get()->DeleteLocalRef(jobjectLocalByteBuffer);
}


void atsc3NdkClient::atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID, audio_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
    //printf("atsc3NdkClient::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    //env.Get()->DeleteLocalRef(jobjectLocalByteBuffer);
}



void atsc3NdkClient::atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds) {
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    int r =  Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID, stpp_packet_id, mpu_sequence_number, mpu_presentation_time_ntp64, mpu_presentation_time_seconds, mpu_presentation_time_microseconds);
    //printf("atsc3NdkClient::onMfuPacket, ret: %d, bufferLen: %u", r, bufferLen);
    //env.Get()->DeleteLocalRef(jobjectLocalByteBuffer);
}


void atsc3NdkClient::atsc3_onMfuSampleMissing(uint16_t pcaket_id, uint32_t mpu_sequence_number, uint32_t sample_number) {
    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onMfuSampleMissingID, pcaket_id, mpu_sequence_number, sample_number);

}


void atsc3NdkClient::LogMsg(const char *msg)
{
    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady() || !mOnLogMsgId)
        return;
    CJniEnv env(mJavaVM);
    if (!env) {
        eprintf("!! err on get jni env\n");
        return;
    }
    jstring js = env.Get()->NewStringUTF(msg);
    int r = env.Get()->CallIntMethod(mClsDrvIntf, mOnLogMsgId, js);
    env.Get()->DeleteLocalRef(js);
}

void atsc3NdkClient::LogMsg(const std::string &str)
{
    LogMsg(str.c_str());
}

void atsc3NdkClient::LogMsgF(const char *fmt, ...)
{
    va_list v;
    char msg[1024];
    va_start(v, fmt);
    vsnprintf(msg, sizeof(msg)-1, fmt, v);
    msg[sizeof(msg)-1] = 0;
    va_end(v);
    LogMsg(msg);
}

//alcCompleteObjectMsg
void atsc3NdkClient::atsc3_onAlcObjectStatusMessage(const char *fmt, ...)
{
    va_list v;
    char msg[1024];
    va_start(v, fmt);
    vsnprintf(msg, sizeof(msg)-1, fmt, v);
    msg[sizeof(msg)-1] = 0;
    va_end(v);


    if (!JReady())
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    jstring js = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(msg);

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_on_alc_object_status_message_ID, js);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(js);

}


int atsc3NdkClient::atsc3_pcap_thread_run() {
    pcapThreadShouldRun = false;
    LogMsgF("atsc3_pcap_thread_run: checking for previous pcap_thread: producerShutdown: %d, consumerShutdown: %d", pcapProducerShutdown, pcapConsumerShutdown);

    if(pcapProducerThreadPtr.joinable()) {
        pcapProducerThreadPtr.join();
    }
    if(pcapConsumerThreadPtr.joinable()) {
        pcapConsumerThreadPtr.join();
    }
    usleep(500000);

    //jjustman-2019-11-05 - TODO: make sure mhRxThread is terminated before we instantiate a new
    pcapThreadShouldRun = true;

    pcapProducerThreadPtr = std::thread([this](){
        JavaVMAttachArgs args;
        args.version = JNI_VERSION_1_4;
        args.name = "libatsc3_pcapProducerThreadParser";
        args.group = NULL;

        atsc3_jni_pcap_producer_thread_env = new CJniEnv(mJavaVM, args);

        LogMsgF("atsc3NdkClient::atsc3_pcap_producer_thread_run with this: %p", this);

        this->PcapProducerThreadParserRun();
        delete atsc3_jni_pcap_producer_thread_env;
    });

    pcapConsumerThreadPtr = std::thread([this](){
        JavaVMAttachArgs args;
        args.version = JNI_VERSION_1_4;
        args.name = "libatsc3_pcapConsumerThreadParser";
        args.group = NULL;

        atsc3_jni_pcap_consumer_thread_env = new CJniEnv(mJavaVM, args);
        Atsc3_Jni_Processing_Thread_Env = atsc3_jni_pcap_consumer_thread_env; //hack
        LogMsgF("atsc3NdkClient::atsc3_pcap_consumer_thread_run with this: %p", this);

        this->PcapConsumerThreadRun();
        Atsc3_Jni_Processing_Thread_Env = NULL;
        delete atsc3_jni_pcap_consumer_thread_env;
    });


    return 0;
}

int atsc3NdkClient::pinFromRxCaptureThread() {
    printf("atsc3NdkClient::Atsc3_Jni_Processing_Thread_Env: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Processing_Thread_Env = new CJniEnv(mJavaVM);
    return 0;
};

int atsc3NdkClient::pinFromRxProcessingThread() {
    printf("atsc3NdkClient::pinFromRxProcessingThread: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Processing_Thread_Env = new CJniEnv(mJavaVM);
    return 0;
}


int atsc3NdkClient::pinFromRxStatusThread() {
    printf("atsc3NdkClient::pinFromRxStatusThread: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Status_Thread_Env = new CJniEnv(mJavaVM);
    return 0;
}

int atsc3NdkClient::PcapLocalCleanup() {
    int spinlock_count = 0;
    while(spinlock_count++ < 10 && (!pcapProducerShutdown || !pcapConsumerShutdown)) {
        LogMsgF("atsc3NdkClient::PcapLocalCleanup: waiting for pcapProducerShutdown: %d, pcapConsumerShutdown: %d, pcapThreadShouldRun: %d",
                pcapProducerShutdown, pcapConsumerShutdown, pcapThreadShouldRun);
        sleep(1);
    }
    //release any local resources held in our context
    atsc3_pcap_replay_free(&atsc3_pcap_replay_context);

    //release any remaining block_t* payloads in pcap_replay_buffer_queue
    while(pcap_replay_buffer_queue.size()) {
        block_t* to_free = pcap_replay_buffer_queue.front();
        pcap_replay_buffer_queue.pop();
        block_Destroy(&to_free);
    }

    if(pcap_replay_asset_ref_ptr) {
        AAsset_close(pcap_replay_asset_ref_ptr);
        pcap_replay_asset_ref_ptr = NULL;
    }

    //we can close the asset reference, but don't close the AAssetManager GlobalReference
    if(global_pcap_asset_manager_ref) {
        CJniEnv env(mJavaVM);
        if (!env) {
            eprintf("!! err on get jni env\n");
        } else {
            env.Get()->DeleteGlobalRef(global_pcap_asset_manager_ref);
        }
        global_pcap_asset_manager_ref = NULL;
    }

    if(pcap_replay_filename) {
        free(pcap_replay_filename);
        pcap_replay_filename = NULL;
    }

    return 0;
}

int atsc3NdkClient::atsc3_pcap_thread_stop() {

    pcapThreadShouldRun = false;
    LogMsgF("atsc3NdkClient::atsc3_pcap_thread_stop with this: %p", &pcapProducerThreadPtr);
    if(pcapProducerThreadPtr.joinable()) {
        pcapProducerThreadPtr.join();
    }

    if(pcapConsumerThreadPtr.joinable()) {
        pcapConsumerThreadPtr.join();
    }
    LogMsgF("atsc3NdkClient::atsc3_pcap_thread_stop: stopped with this: %p", &pcapProducerThreadPtr);

    PcapLocalCleanup();
    return 0;
}

int atsc3NdkClient::atsc3_slt_selectService(int service_id) {
    int ret = -1;
    atsc3_lls_slt_service_t* atsc3_lls_slt_service = atsc3_phy_mmt_player_bridge_set_single_monitor_a331_service_id(service_id);
    if(atsc3_lls_slt_service && atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.count) {
        ret = atsc3_lls_slt_service->atsc3_slt_broadcast_svc_signalling_v.data[0]->sls_protocol;
    }

    return ret;
}

//E_AT3_FESTAT
void atsc3NdkClient::RxStatusThread() {

}

void atsc3NdkClient::atsc3_update_rf_stats(int32_t tuner_lock,
                                        int32_t rssi,
                                        uint8_t modcod_valid,
                                        uint8_t plp_fec_type,
                                        uint8_t plp_mod,
                                        uint8_t plp_cod,
                                        int32_t nRfLevel1000,
                                        int32_t nSnr1000,
                                        uint32_t ber_pre_ldpc_e7,
                                        uint32_t ber_pre_bch_e9,
                                        uint32_t fer_post_bch_e6,
                                        uint8_t demod_lock_status,
                                        uint8_t cpu_status,
                                        uint8_t plp_any,
                                        uint8_t plp_all) {

    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady() || !mOnLogMsgId)
        return;

    if (!Atsc3_Jni_Status_Thread_Env) {
        eprintf("atsc3NdkClient:atsc3_update_rf_stats: err on get jni env: Atsc3_Jni_Status_Thread_Env\n");
        return;
    }
    int r = Atsc3_Jni_Status_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_rf_phy_status_callback_ID,
                                                              tuner_lock,
                                                              rssi,
                                                              modcod_valid,
                                                              plp_fec_type,
                                                              plp_mod,
                                                              plp_cod,
                                                              nRfLevel1000,
                                                              nSnr1000,
                                                              ber_pre_ldpc_e7,
                                                              ber_pre_bch_e9,
                                                              fer_post_bch_e6,
                                                              demod_lock_status,
                                                              cpu_status,
                                                              plp_any,
                                                              plp_all);

}


void atsc3NdkClient::atsc3_onExtractedSampleDuration(uint16_t packet_id, uint32_t mpu_sequence_number,
                                                 uint32_t extracted_sample_duration_us) {

    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady() || !mOnLogMsgId)
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }
    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onExtractedSampleDurationID,
                                                                  packet_id,
                                                                  mpu_sequence_number,
                                                                  extracted_sample_duration_us);
}



void atsc3NdkClient::atsc3_setVideoWidthHeightFromTrak(uint32_t width, uint32_t height) {

    // this method can be called in native thread. we don't safely use pre-assigned mJniEnv.
    if (!JReady() || !mOnLogMsgId)
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }
    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_setVideoWidthHeightFromTrakID, width, height);

}


void atsc3NdkClient::atsc3_update_rf_bw_stats(uint64_t total_pkts, uint64_t total_bytes,
                                          unsigned int total_lmts) {
    if (!JReady() || !mOnLogMsgId)
        return;
    if (!Atsc3_Jni_Status_Thread_Env) {
        eprintf("atsc3NdkClient:atsc3_update_rf_bw_stats: err on get jni env: Atsc3_Jni_Status_Thread_Env\n");
        return;
    }
    int r = Atsc3_Jni_Status_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_update_rf_bw_stats_ID, total_pkts, total_bytes, total_lmts);
}

//Java to native methods



void atsc3NdkClient::set_plp_settings(jint *a_plp_ids, jsize a_plp_size) {

    uint8_t* u_plp_ids = (uint8_t*)calloc(a_plp_size, sizeof(uint8_t));
    for(int i=0; i < a_plp_size; i++) {
        u_plp_ids[i] = (uint8_t)a_plp_ids[i];
    }

    //AT3DRV_FE_SetPLP
   // AT3DRV_FE_SetPLP(mhDevice, u_plp_ids, a_plp_size);

}

std::string atsc3NdkClient::get_android_temp_folder() {
    CJniEnv env(mJavaVM);

    jclass clazz = env.Get()->FindClass("org/ngbp/libatsc3/sampleapp/atsc3NdkClient");

    jmethodID getCacheDir = env.Get()->GetMethodID( clazz, "getCacheDir", "()Ljava/io/File;" );
    jobject cache_dir = env.Get()->CallObjectMethod(mClsDrvIntf, getCacheDir );

    jclass fileClass = env.Get()->FindClass( "java/io/File" );
    jmethodID getPath = env.Get()->GetMethodID( fileClass, "getPath", "()Ljava/lang/String;" );
    jstring path_string = (jstring)env.Get()->CallObjectMethod( cache_dir, getPath );

    const char *path_chars = env.Get()->GetStringUTFChars( path_string, NULL );
    std::string temp_folder( path_chars );

    env.Get()->ReleaseStringUTFChars( path_string, path_chars );
    //app->activity->vm->DetachCurrentThread();
    return temp_folder;
}

//return -1 on service_id not found
//return -2 on duplicate additional service_id request
int atsc3NdkClient::atsc3_slt_alc_select_additional_service(int service_id) {
    //keep track of internally here which "additional service_id's" we have on monitor;

    bool is_monitoring_duplicate = false;
    for(int i=0; i < atsc3_slt_alc_additional_services_monitored.size() && !is_monitoring_duplicate; i++) {
        if(atsc3_slt_alc_additional_services_monitored.at(i) == service_id) {
            //duplicate request
            is_monitoring_duplicate = true;
            continue;
        }
    }

    if(is_monitoring_duplicate) {
        return -2;
    }
    atsc3_lls_slt_service_t* atsc3_lls_slt_service = atsc3_phy_mmt_player_bridge_add_monitor_a331_service_id(service_id);
    if(!atsc3_lls_slt_service) {
        return -1;
    }

    atsc3_slt_alc_additional_services_monitored.push_back(service_id);

    return 0;
}

//TODO: jjustman-2019-11-07 - add mutex here around additional_services_monitored collection
int atsc3NdkClient::atsc3_slt_alc_clear_additional_service_selections() {

    for(int i=0; i < atsc3_slt_alc_additional_services_monitored.size(); i++) {
        int to_remove_monitor_service_id = atsc3_slt_alc_additional_services_monitored.at(i);
        atsc3_phy_mmt_player_bridge_remove_monitor_a331_service_id(to_remove_monitor_service_id);
    }

    atsc3_slt_alc_additional_services_monitored.clear();

    return 0;
}

//use 2nd param to_match_content_type to filter down to MPD via const char* to_match_content_type
//application/dash+xml

/*
 * jjustman-2019-11-08 - note mbms_envelope might not have the proper content_type set, so check
 */
vector<string>
atsc3NdkClient::atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id(int service_id, const char* to_match_content_type) {

    vector<string> my_mbms_metadata_uri_values;
    atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = atsc3_slt_alc_get_sls_metadata_fragments_from_monitor_service_id(service_id);

    if(atsc3_sls_metadata_fragments && atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope && atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope->atsc3_mbms_metadata_item_v.count) {
        for(int i=0; i < atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope->atsc3_mbms_metadata_item_v.count; i++) {
            atsc3_mbms_metadata_item_t* atsc3_mbms_metadata_item = atsc3_sls_metadata_fragments->atsc3_mbms_metadata_envelope->atsc3_mbms_metadata_item_v.data[i];
            if(atsc3_mbms_metadata_item->metadata_uri) {
                //if to_match_content_type is supplied, filter by match
                if(to_match_content_type && atsc3_mbms_metadata_item->content_type &&
                   strncasecmp(to_match_content_type, atsc3_mbms_metadata_item->content_type, strlen(to_match_content_type)) == 0 ) {
                    string my_metadata_route_service_temp_folder_name = atsc3_route_service_context_temp_folder_name(service_id) + atsc3_mbms_metadata_item->metadata_uri;
                    my_mbms_metadata_uri_values.push_back(my_metadata_route_service_temp_folder_name);
                }
            }
        }

    }

    //also walk thru

    if(atsc3_sls_metadata_fragments && atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance && atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count) {
        for(int i=0; i < atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.count; i++) {
            atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = atsc3_sls_metadata_fragments->atsc3_mime_multipart_related_instance->atsc3_mime_multipart_related_payload_v.data[i];
            if(atsc3_mime_multipart_related_payload->sanitizied_content_location) {
                //if to_match_content_type is supplied, filter by match
                if(to_match_content_type && atsc3_mime_multipart_related_payload->content_type &&
                   strncasecmp(to_match_content_type, atsc3_mime_multipart_related_payload->content_type, strlen(to_match_content_type)) == 0 ) {
                    string my_metadata_route_service_temp_folder_name = atsc3_route_service_context_temp_folder_name(service_id) + atsc3_mime_multipart_related_payload->sanitizied_content_location;
                    my_mbms_metadata_uri_values.push_back(my_metadata_route_service_temp_folder_name);
                }
            }
        }

    }

    return my_mbms_metadata_uri_values;
}


vector<string> atsc3NdkClient::atsc3_slt_alc_get_sls_route_s_tsid_fdt_file_content_locations_from_monitor_service_id(int service_id) {
    vector<string> my_fdt_file_content_location_values;
    atsc3_route_s_tsid_t* atsc3_route_s_tsid = atsc3_slt_alc_get_sls_route_s_tsid_from_monitor_service_id(service_id);

    if(atsc3_route_s_tsid) {
        for(int i=0; i < atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.count; i++) {
            atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_RS = atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.data[i];

            for(int j=0; j < atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.count; j++) {
                atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS = atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.data[j];

                if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow) {
                    atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance;

                    if(atsc3_fdt_instance) {
                        for(int k=0; k < atsc3_fdt_instance->atsc3_fdt_file_v.count; k++) {
                            atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[k];
                            if(atsc3_fdt_file && atsc3_fdt_file->content_location ) {

                                string my_fdt_file_content_location_local_context_path = atsc3_route_service_context_temp_folder_name(service_id) + atsc3_fdt_file->content_location;
                                my_fdt_file_content_location_values.push_back(my_fdt_file_content_location_local_context_path);
                                //TODO: jjustman-2019-11-07 - re-factor to use atsc3_fdt_file_t struct
                            }
                        }
                    }
                }
            }
        }
    }

    return my_fdt_file_content_location_values;
}

void atsc3NdkClient::atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_jni(uint32_t tsi, uint32_t toi, char *content_location) {
    //jjustman-2020-01-07: add in alc flow debugging
   // printf("atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_jni: tsi: %d, toi: %d, content_location: %s", tsi, toi, content_location);

    //len: nnn, lost DU: nnn
    atsc3_onAlcObjectStatusMessage("C: tsi: %d, toi: %d, content_location: %s", tsi, toi, content_location);
}

void atsc3NdkClient::atsc3_lls_sls_alc_on_route_mpd_patched_jni(uint16_t service_id) {
    if (!JReady() || !atsc3_lls_sls_alc_on_route_mpd_patched_ID)
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }
    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_lls_sls_alc_on_route_mpd_patched_ID, service_id);

}

// https://stackoverflow.com/questions/6343459/get-strings-used-in-java-from-jni
void atsc3NdkClient::atsc3_lls_sls_alc_on_package_extract_completed_callback_jni(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload) {
    if (!JReady() || !atsc3_lls_sls_alc_on_package_extract_completed_ID)
        return;
    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err on get jni env: Atsc3_Jni_Processing_Thread_Env\n");
        return;
    }

    if(!atsc3_route_package_extracted_envelope_metadata_and_payload) {
        eprintf("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err atsc3_route_package_extracted_envelope_metadata_and_payload is NULL\n");
        return;
    }

    if(!atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml || !atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml->p_buffer) {
        eprintf("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml (or p_buffer) is NULL\n");
        return;
    }

    if(!atsc3_route_package_extracted_envelope_metadata_and_payload->package_name) {
    	eprintf("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err atsc3_route_package_extracted_envelope_metadata_and_payload->package_name is NULL\n");
        return;
    }

    std::list<jstring> to_clean_jstrings;
    std::list<jobject> to_clean_jobject;

    //org.ngbp.libatsc3.android.PackageExtractEnvelopeMetadataAndPayload
    jclass jcls = api.packageExtractEnvelopeMetadataAndPayload_jclass_global_ref;
    jobject jobj = Atsc3_Jni_Processing_Thread_Env->Get()->AllocObject(jcls);

    if(!jobj) {
        eprintf("atsc3_lls_sls_alc_on_package_extract_completed_callback_jni::err unable to allocate packageExtractEnvelopeMetadataAndPayload_jclass_global_ref instance jobj!");
        return;
    }


    jfieldID packageName_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "packageName", "Ljava/lang/String;");
    jstring packageName_payload = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_route_package_extracted_envelope_metadata_and_payload->package_name);
    Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj, packageName_valId, packageName_payload);
    to_clean_jstrings.push_back(packageName_payload);

    jfieldID tsi_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "tsi", "I");
    Atsc3_Jni_Processing_Thread_Env->Get()->SetIntField(jobj, tsi_valId, atsc3_route_package_extracted_envelope_metadata_and_payload->tsi);

    jfieldID toi_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "toi", "I");
    Atsc3_Jni_Processing_Thread_Env->Get()->SetIntField(jobj, toi_valId, atsc3_route_package_extracted_envelope_metadata_and_payload->toi);

    jfieldID appContextIdList_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "appContextIdList", "Ljava/lang/String;");
    jstring appContextIdList_payload = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_route_package_extracted_envelope_metadata_and_payload->app_context_id_list);
    Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj, appContextIdList_valId, appContextIdList_payload);
    to_clean_jstrings.push_back(appContextIdList_payload);


    jfieldID packageExtractPath_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "packageExtractPath", "Ljava/lang/String;");
    jstring packageExtractPath_payload = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_route_package_extracted_envelope_metadata_and_payload->package_extract_path);
    Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj, packageExtractPath_valId, packageExtractPath_payload);
    to_clean_jstrings.push_back(packageExtractPath_payload);


    jfieldID mbmsEnvelopeRawXml_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "mbmsEnvelopeRawXml", "Ljava/lang/String;");
    jstring mbmsEnvelopeRawXml_payload = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF((char*)atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mbms_metadata_envelope_raw_xml->p_buffer);
    Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj, mbmsEnvelopeRawXml_valId, mbmsEnvelopeRawXml_payload);
    to_clean_jstrings.push_back(mbmsEnvelopeRawXml_payload);

    if(atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.count > 0) {

        jobject multipartRelatedPayloadList_jobject = Atsc3_Jni_Processing_Thread_Env->Get()->NewObject(api.jni_java_util_ArrayList, api.jni_java_util_ArrayList_cctor, atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.count);

        for(int i=0; i < atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.count; i++) {
            atsc3_mime_multipart_related_payload_t* atsc3_mime_multipart_related_payload = atsc3_route_package_extracted_envelope_metadata_and_payload->atsc3_mime_multipart_related_payload_v.data[i];
            jobject jobj_multipart_related_payload_jobject = Atsc3_Jni_Processing_Thread_Env->Get()->AllocObject(api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref);

            to_clean_jobject.push_back(jobj_multipart_related_payload_jobject);

            jfieldID contentLocation_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "contentLocation", "Ljava/lang/String;");
            jstring contentLocation_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->sanitizied_content_location);
            Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, contentLocation_valId, contentLocation_jstring);
            to_clean_jstrings.push_back(contentLocation_jstring);

            if(atsc3_mime_multipart_related_payload->content_type) {
                jfieldID contentType_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "contentType", "Ljava/lang/String;");
                jstring contentType_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->content_type);
                Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, contentType_valId, contentType_jstring);
                to_clean_jstrings.push_back(contentType_jstring);
            }

            if(atsc3_mime_multipart_related_payload->valid_from_string) {
                jfieldID validFrom_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "validFrom", "Ljava/lang/String;");
                jstring validFrom_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->valid_from_string);
                Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, validFrom_valId, validFrom_jstring);
                to_clean_jstrings.push_back(validFrom_jstring);
            }

            if(atsc3_mime_multipart_related_payload->valid_until_string) {
                jfieldID validUntil_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "validUntil", "Ljava/lang/String;");
                jstring validUntil_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->valid_until_string);
                Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, validUntil_valId, validUntil_jstring);
                to_clean_jstrings.push_back(validUntil_jstring);
            }

            //version

            jfieldID version_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "version", "I");
            Atsc3_Jni_Processing_Thread_Env->Get()->SetIntField(jobj_multipart_related_payload_jobject, version_valId, atsc3_mime_multipart_related_payload->version);


            if(atsc3_mime_multipart_related_payload->next_url_string) {
                jfieldID nextUrl_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "nextUrl", "Ljava/lang/String;");
                jstring nextUrl_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->next_url_string);
                Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, nextUrl_valId, nextUrl_jstring);
                to_clean_jstrings.push_back(nextUrl_jstring);
            }

            if(atsc3_mime_multipart_related_payload->avail_at_string) {
                jfieldID availAt_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "availAt", "Ljava/lang/String;");
                jstring availAt_jstring = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(atsc3_mime_multipart_related_payload->avail_at_string);
                Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj_multipart_related_payload_jobject, availAt_valId, availAt_jstring);
                to_clean_jstrings.push_back(availAt_jstring);
            }

            //extractedSize
            jfieldID extractedSize_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref, "extractedSize", "I");
            Atsc3_Jni_Processing_Thread_Env->Get()->SetIntField(jobj_multipart_related_payload_jobject, extractedSize_valId, atsc3_mime_multipart_related_payload->extracted_size);

            Atsc3_Jni_Processing_Thread_Env->Get()->CallBooleanMethod(multipartRelatedPayloadList_jobject, api.jni_java_util_ArrayList_add, jobj_multipart_related_payload_jobject);
        }

        jfieldID multipartRelatedPayloadList_valId = Atsc3_Jni_Processing_Thread_Env->Get()->GetFieldID(jcls, "multipartRelatedPayloadList", "Ljava/util/List;");
        Atsc3_Jni_Processing_Thread_Env->Get()->SetObjectField(jobj, multipartRelatedPayloadList_valId, multipartRelatedPayloadList_jobject);
    }

    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_lls_sls_alc_on_package_extract_completed_ID, jobj);

    for (std::list<jstring>::iterator it=to_clean_jstrings.begin(); it != to_clean_jstrings.end(); ++it) {
        Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(*it);
    }
    to_clean_jstrings.clear();

    for (std::list<jobject>::iterator it=to_clean_jobject.begin(); it != to_clean_jobject.end(); ++it) {
        Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(*it);
    }
    to_clean_jobject.clear();

    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(jobj);
}

void atsc3NdkClient::atsc3_sls_on_held_trigger_received_callback_jni(uint16_t service_id, const char *held_payload) {
	if (!JReady() || !atsc3_lls_sls_alc_on_route_mpd_patched_ID)
		return;

	if (!Atsc3_Jni_Processing_Thread_Env) {
		eprintf("!! err on get jni env\n");
		return;
	}

	atsc3_onAlcObjectStatusMessage("HELD: service_id: %d, xml:\n%s", service_id, held_payload);
}

void atsc3NdkClient::atsc3_onSlsTablePresent(const char *sls_payload_xml) {
    if (!JReady() || !atsc3_onSlsTablePresent_ID) {
        eprintf("err: JReady: %d, atsc3_onSlsTablePresent_ID: %d",  JReady(), atsc3_onSlsTablePresent_ID);

        return;
    }

    if (!Atsc3_Jni_Processing_Thread_Env) {
        eprintf("!! err on get jni env\n");
        return;
    }

    jstring xml_payload = Atsc3_Jni_Processing_Thread_Env->Get()->NewStringUTF(sls_payload_xml);
    int r = Atsc3_Jni_Processing_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_onSlsTablePresent_ID, xml_payload);
    Atsc3_Jni_Processing_Thread_Env->Get()->DeleteLocalRef(xml_payload);
}


//--------------------------------------------------------------------------

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiInit(JNIEnv *env, jobject instance, jobject drvIntf)
{
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiInit: start init, env: %p\n", env);

    api.mJniEnv = env;

    env->GetJavaVM(&api.mJavaVM);
    if(api.mJavaVM == NULL) {
        eprintf("!! no java vm\n");
        return -1;
    }

    jclass jClazz = env->FindClass("org/ngbp/libatsc3/sampleapp/atsc3NdkClient");
    if (jClazz == NULL) {
        eprintf("!! Cannot find atsc3NdkClient java class\n");
        return -1;
    }
    api.mOnLogMsgId = env->GetMethodID(jClazz, "onLogMsg", "(Ljava/lang/String;)I");
    if (api.mOnLogMsgId == NULL) {
        eprintf("!! Cannot find 'onLogMsg' method id\n");
        return -1;
    }

    //atsc3_onSlsTablePresent_ID
    api.atsc3_onSlsTablePresent_ID = env->GetMethodID(jClazz, "atsc3_onSlsTablePresent", "(Ljava/lang/String;)I");
    if (api.atsc3_onSlsTablePresent_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_onSlsTablePresent_ID' method id\n");
        return -1;
    }

    //java.nio.ByteBuffer, L: fully qualified class, J: long
    api.atsc3_onMfuPacketID = env->GetMethodID(jClazz, "atsc3_onMfuPacket", "(IIILjava/nio/ByteBuffer;IJI)I");
    if (api.atsc3_onMfuPacketID == NULL) {
        eprintf("!! Cannot find 'atsc3_onMfuPacket' method id\n");
        return -1;
    }
    //java.nio.ByteBuffer, L: fully qualified class, J: long
    api.atsc3_onMfuPacketCorruptID = env->GetMethodID(jClazz, "atsc3_onMfuPacketCorrupt", "(IIILjava/nio/ByteBuffer;IJII)I");
    if (api.atsc3_onMfuPacketCorruptID == NULL) {
        eprintf("!! Cannot find 'atsc3_onMfuPacketCorrupt' method id\n");
        return -1;
    }

    //java.nio.ByteBuffer, L: fully qualified class, J: long
    api.atsc3_onMfuPacketCorruptMmthSampleHeaderID = env->GetMethodID(jClazz, "atsc3_onMfuPacketCorruptMmthSampleHeader", "(IIILjava/nio/ByteBuffer;IJII)I");
    if (api.atsc3_onMfuPacketCorruptMmthSampleHeaderID == NULL) {
        eprintf("!! Cannot find 'atsc3_onMfuPacketCorruptMmthSampleHeaderID' method id\n");
        return -1;
    }


    //java.nio.ByteBuffer, L: fully qualified class, J: long
    api.atsc3_onMfuSampleMissingID = env->GetMethodID(jClazz, "atsc3_onMfuSampleMissing", "(III)I");
    if (api.atsc3_onMfuSampleMissingID == NULL) {
        eprintf("!! Cannot find 'atsc3_onMfuSampleMissingID' method id\n");
        return -1;
    }

    //java.nio.ByteBuffer
    api.mOnInitHEVC_NAL_Extracted = env->GetMethodID(jClazz, "atsc3_onInitHEVC_NAL_Packet", "(IILjava/nio/ByteBuffer;I)I");
    if (api.mOnInitHEVC_NAL_Extracted == NULL) {
        eprintf("!! Cannot find 'atsc3_onInitHEVC_NAL_Packet' method id\n");
        return -1;
    }

    /*
     *  public int atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(int video_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds);
     *  public int atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds);
     *  public int atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds) {
    */

    api.atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jClazz, "atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor", "(IIJII)I");
    if (api.atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor_ID' method id\n");
        return -1;
    }
    api.atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jClazz, "atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor", "(IIJII)I");
    if (api.atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor_ID' method id\n");
        return -1;
    }
    api.atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID = env->GetMethodID(jClazz, "atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor", "(IIJII)I");
    if (api.atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor_ID' method id\n");
        return -1;
    }

    api.atsc3_rf_phy_status_callback_ID = env->GetMethodID(jClazz, "atsc3_rf_phy_status_callback", "(IIIIIIIIIIIIIII)I");
    if (api.atsc3_rf_phy_status_callback_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_rf_phy_status_callback' method id\n");
        return -1;
    }

    //atsc3_onExtractedSampleDurationID
    api.atsc3_onExtractedSampleDurationID = env->GetMethodID(jClazz, "atsc3_onExtractedSampleDuration", "(III)I");
    if (api.atsc3_onExtractedSampleDurationID == NULL) {
        eprintf("!! Cannot find 'atsc3_onExtractedSampleDurationID' method id\n");
        return -1;
    }

    //atsc3_setVideoWidthHeightFromTrakID
    api.atsc3_setVideoWidthHeightFromTrakID = env->GetMethodID(jClazz, "atsc3_setVideoWidthHeightFromTrak", "(II)I");
    if (api.atsc3_setVideoWidthHeightFromTrakID == NULL) {
        eprintf("!! Cannot find 'atsc3_setVideoWidthHeightFromTrakID' method id\n");
        return -1;
    }

    //atsc3_update_rf_bw_stats_ID
    api.atsc3_update_rf_bw_stats_ID = env->GetMethodID(jClazz, "atsc3_updateRfBwStats", "(JJI)I");
    if (api.atsc3_update_rf_bw_stats_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_update_rf_bw_stats_ID' method id\n");
        return -1;
    }

    //atsc3_lls_sls_alc_on_route_mpd_patched_ID
    api.atsc3_lls_sls_alc_on_route_mpd_patched_ID = env->GetMethodID(jClazz, "atsc3_lls_sls_alc_on_route_mpd_patched", "(I)I");
    if (api.atsc3_lls_sls_alc_on_route_mpd_patched_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_lls_sls_alc_on_route_mpd_patched_ID' method id\n");
        return -1;
    }

    //atsc3_on_alc_object_status_message_ID
    api.atsc3_on_alc_object_status_message_ID = env->GetMethodID(jClazz, "atsc3_on_alc_object_status_message", "(Ljava/lang/String;)I");
    if (api.atsc3_on_alc_object_status_message_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_on_alc_object_status_message_ID' method id\n");
        return -1;
    }

    //jjustman-2020-07-27 - atsc3_lls_sls_alc_on_package_extract_completed_ID
    //org.ngbp.libatsc3.android.PackageExtractEnvelopeMetadataAndPayload
	api.atsc3_lls_sls_alc_on_package_extract_completed_ID = env->GetMethodID(jClazz, "atsc3_lls_sls_alc_on_package_extract_completed", "(Lorg/ngbp/libatsc3/android/PackageExtractEnvelopeMetadataAndPayload;)I");
	if (api.atsc3_lls_sls_alc_on_package_extract_completed_ID == NULL) {
	   eprintf("!! Cannot find 'atsc3_lls_sls_alc_on_package_extract_completed_ID' method id\n");
	   return -1;
	}

	api.packageExtractEnvelopeMetadataAndPayload_jclass_init_env = env->FindClass("org/ngbp/libatsc3/android/PackageExtractEnvelopeMetadataAndPayload");

    if (api.packageExtractEnvelopeMetadataAndPayload_jclass_init_env == NULL) {
        eprintf("Cannot find 'packageExtractEnvelopeMetadataAndPayload_jclass' class reference\n");
        return -1;
    } else {
        api.packageExtractEnvelopeMetadataAndPayload_jclass_global_ref = (jclass)(env->NewGlobalRef(api.packageExtractEnvelopeMetadataAndPayload_jclass_init_env));
    }

    api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_init_env = env->FindClass("org/ngbp/libatsc3/android/PackageExtractEnvelopeMetadataAndPayload$MultipartRelatedPayload");
    if (api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_init_env == NULL) {
        eprintf("Cannot find 'packageExtractEnvelopeMetadataAndPayload$MultipartRelatedPayload_jclass_init_env' class reference\n");
        return -1;
    } else {
       api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref = (jclass)(env->NewGlobalRef(api.packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_init_env));
    }

    api.jni_java_util_ArrayList = (jclass) env->NewGlobalRef(env->FindClass("java/util/ArrayList"));
    eprintf("creating api.jni_java_util_ArrayList");

    api.jni_java_util_ArrayList_cctor = env->GetMethodID(api.jni_java_util_ArrayList, "<init>", "(I)V");
    eprintf("creating api.jni_java_util_ArrayList_cctor");
    api.jni_java_util_ArrayList_add  = env->GetMethodID(api.jni_java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");
    eprintf("creating api.jni_java_util_ArrayList_add");

    api.mClsDrvIntf = (jclass)(api.mJniEnv->NewGlobalRef(drvIntf));

    int r = api.Init();
    if (r)
        return r;

    api.LogMsg("Api init ok");

    //wire up atsc3_phy_mmt_player_bridge
    //atsc3NdkClient* at3DrvIntf_ptr
    atsc3_phy_mmt_player_bridge_init(&api);

    printf("**** jni init OK\n");
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiPrepare(JNIEnv *env, jobject instance, jstring devlist_, jint d1, jint d2)
{
    printf("jni prepare\n");

    const char *devlist = env->GetStringUTFChars(devlist_, 0);
    int r = api.Prepare(devlist, (int)d1, (int)d2);
    env->ReleaseStringUTFChars(devlist_, devlist);
    return r;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiFwLoad(JNIEnv *env, jobject instance, jlong key)
{
    printf("jni fwload\n");

//    int r = api.FwLoad((AT3_DEV_KEY) key);
//    return r;

    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiOpen(JNIEnv *env, jobject instance, jint fd, jlong key)
{
    int bus = (key >> 8) & 0xFF;
    int addr = key & 0xFF;

    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiOpen: invoking open with fd: %d, key: %d, bus: %d, addr: %d",
            fd, key, bus, addr);

    api.Open(fd, bus, addr);

    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiTune(JNIEnv *env, jobject instance, jint freqKHz, jint plpid)
{
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiTune::tune\n");

    return api.Tune(freqKHz, plpid);
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiStop(JNIEnv *env, jobject instance)
{
    printf("jni stop\n");
    return api.Stop();
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiClose(JNIEnv *env, jobject instance)
{
    printf("ApiClose:\n");
    return api.Close();
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiUninit(JNIEnv *env, jobject instance)
{
    printf("ApiUninit:\n");
    int r = api.Uninit();

    if (api.mClsDrvIntf) {
        api.mJniEnv->DeleteGlobalRef(api.mClsDrvIntf);
        api.mClsDrvIntf = nullptr;
    }

    return r;
}
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiReset(JNIEnv *env, jobject instance)
{
    printf("jni reset:\n");
    return api.Reset();
}

extern "C" JNIEXPORT jlongArray JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiFindDeviceKey(JNIEnv *env, jobject instance, jboolean bPreBootDevice)
{
//    printf("jni find %s devices\n", bPreBootDevice ? "preboot" : "atlas");
//    std::vector<AT3_DEV_KEY> vKeys = api.FindKeys(bPreBootDevice);

//    if (vKeys.empty())
//        return NULL;
    jlongArray arr;
//    arr = env->NewLongArray(vKeys.size());
//
//    std::vector<jlong> vTmp;
//    for (int i=0; i<vKeys.size(); i++)
//        vTmp.push_back(vKeys[i]);
//
//    env->SetLongArrayRegion(arr, 0, vKeys.size(), &vTmp[0]);
    return NULL;
}

//hacks

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1pcap_1open_1for_1replay_1from_1assetManager(JNIEnv *env, jobject thiz, jstring filename_,
                                                        jobject asset_manager_weak) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1pcap_1open_1for_1replay_1from_1assetManager\n");

    const char* filename_weak = env->GetStringUTFChars(filename_, 0);

    //global ref and AAsetManager ptr null'd in PcapLocalCleanup
    api.global_pcap_asset_manager_ref = env->NewGlobalRef(asset_manager_weak);

    AAssetManager* aasetManager = AAssetManager_fromJava(env, asset_manager_weak);
    int r = api.atsc3_pcap_replay_open_file_from_assetManager(filename_weak, aasetManager);
    env->ReleaseStringUTFChars(filename_, filename_weak);

    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1pcap_1open_1for_1replay_1from_1assetManager - return: %d\n", r);
    return r;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1pcap_1thread_1run(JNIEnv *env, jobject thiz) {

    printf("::Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApPcapThreadRun\n");

    int r = api.atsc3_pcap_thread_run();
    return r;

}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1pcap_1thread_1stop(JNIEnv *env, jobject thiz) {

    printf("::Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1pcap_1thread_1stop\n");

    int r = api.atsc3_pcap_thread_stop();
    return r;

}
extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1slt_1selectService(JNIEnv *env, jobject thiz,
                                                         jint service_id) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1slt_1selectService, service_id: %d\n", (int)service_id);
    int ret = api.atsc3_slt_selectService((int)service_id);

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_ApiSetPLP(JNIEnv *env, jobject thiz, jintArray a_plp_ids) {
    // TODO: implement ApiSetPLP()
//
//    jsize len = *env->GetArrayLength(a_plp_ids);
//    jint *a_body = *env->GetIntArrayElements(a_plp_ids, 0);
////    for (int i=0; i<len; i++) {
////        sum += body[i];
////    }
//    api.set_plp_settings(a_body, len);

    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1pcap_1open_1for_1replay(JNIEnv *env, jobject thiz,
                                                                  jstring filename_) {

    const char* filename_weak = env->GetStringUTFChars(filename_, 0);

    int ret = api.atsc3_pcap_replay_open_file(filename_weak);

    env->ReleaseStringUTFChars( filename_, filename_weak );

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1slt_1alc_1select_1additional_1service(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jint service_id) {

    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1slt_1alc_1select_1additional_1service, additional service_id: %d\n", (int)service_id);
    int ret = api.atsc3_slt_alc_select_additional_service((int)service_id);

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1slt_1alc_1clear_1additional_1service_1selections(
        JNIEnv *env, jobject thiz) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1slt_1alc_1clear_1additional_1service_1selections\n");
    int ret = api.atsc3_slt_alc_clear_additional_service_selections();
    return ret;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1slt_1alc_1get_1sls_1metadata_1fragments_1content_1locations_1from_1monitor_1service_1id(JNIEnv *env, jobject thiz, jint service_id, jstring to_match_content_type_string) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1slt_1alc_1get_1sls_1metadata_1fragments_1content_1locations_1from_1monitor_1service_1id, service_id: %d\n", (int)service_id);
    const char* to_match_content_type_weak = env->GetStringUTFChars(to_match_content_type_string, 0);

    vector<string> slt_alc_sls_metadata_fragment_content_locations = api.atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id((int)service_id, to_match_content_type_weak);

    jstring str;
    int i;

    jobjectArray slt_alc_sls_metadata_fragment_content_locations_jni = env->NewObjectArray(slt_alc_sls_metadata_fragment_content_locations.size(), env->FindClass("java/lang/String"),0);

    for(i=0; i < slt_alc_sls_metadata_fragment_content_locations.size(); i++) {
        str = env->NewStringUTF(slt_alc_sls_metadata_fragment_content_locations.at(i).c_str());
        env->SetObjectArrayElement(slt_alc_sls_metadata_fragment_content_locations_jni, i, str);
    }
    env->ReleaseStringUTFChars( to_match_content_type_string, to_match_content_type_weak );

    return slt_alc_sls_metadata_fragment_content_locations_jni;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1slt_1alc_1get_1sls_1route_1s_1tsid_1fdt_1file_1content_1locations_1from_1monitor_1service_1id(
        JNIEnv *env, jobject thiz, jint service_id) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_atsc3_1slt_1alc_1get_1sls_1route_1s_1tsid_1fdt_1file_1content_1locations_1from_1monitor_1service_1id, service_id: %d\n", (int)service_id);

    vector<string> slt_alc_sls_route_s_tsid_fdt_file_content_locations = api.atsc3_slt_alc_get_sls_route_s_tsid_fdt_file_content_locations_from_monitor_service_id((int)service_id);

    jstring str;
    int i;

    jobjectArray slt_alc_sls_route_s_tsid_fdt_file_content_locations_jni = env->NewObjectArray(slt_alc_sls_route_s_tsid_fdt_file_content_locations.size(), env->FindClass("java/lang/String"),0);

    for(i=0; i < slt_alc_sls_route_s_tsid_fdt_file_content_locations.size(); i++) {
        str = env->NewStringUTF(slt_alc_sls_route_s_tsid_fdt_file_content_locations.at(i).c_str());
        env->SetObjectArrayElement(slt_alc_sls_route_s_tsid_fdt_file_content_locations_jni, i, str);
    }

    return slt_alc_sls_route_s_tsid_fdt_file_content_locations_jni;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkClient_setRfPhyStatisticsViewVisible(JNIEnv *env, jobject thiz, jboolean is_rf_phy_statistics_visible) {
    if(is_rf_phy_statistics_visible) {
        atsc3NdkClientNoPhyImpl::tunerStatusThreadShouldPollTunerStatus = true;
    } else {
        atsc3NdkClientNoPhyImpl::tunerStatusThreadShouldPollTunerStatus = false;
    }

    return 0;
}



int atsc3NdkClient::ListenPLP1(int plp1) {
    //apiImpl.ListenPLP1(plp1);
    return 0;
}
