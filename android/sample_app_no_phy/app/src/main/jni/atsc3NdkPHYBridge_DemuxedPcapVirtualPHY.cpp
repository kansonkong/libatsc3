#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <atsc3_lls_types.h>
#include <atsc3_phy_mmt_player_bridge.h>
#include <atsc3_pcap_type.h>
#include <atsc3_monitor_events_alc.h>
#include "atsc3NdkPHYBridge_DemuxedPcapVirtualPHY.h"
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

atsc3NdkPHYBridge_DemuxedPcapVirtualPHY api;
atsc3NdkClientNoPhyImpl apiImpl;

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::Init()
{
    printf("%s:%s:TODO", __FILE__, __func__);

    mbInit = true;
    return 0;
}

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::Prepare(const char *strDevListInfo, int delim1, int delim2)
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
int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::Open(int fd, int bus, int addr)
{
    apiImpl.Init(this);
    apiImpl.Open(fd, bus, addr);
    return 0;
}

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_rx_callback_f(void* pData, uint64_t ullUser)
{
//    atsc3NdkPHYBridge_DemuxedPcapVirtualPHY *me = (atsc3NdkPHYBridge_DemuxedPcapVirtualPHY *)ullUser; // same as &api
//    return me->RxCallbackJJ(pData);
    return 0;
}


//used for inclusion of pcap's via android assetManager

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_pcap_replay_open_file_from_assetManager(const char *filename,
                                                                     AAssetManager *mgr) {

    if(!filename) {
        return -1;
    }

    if(pcapThreadShouldRun) {
        //shutdown and restart
        atsc3_pcap_thread_stop();
    }

    LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_pcap_open_for_replay_from_assetManager: filename: %s, aasetManager: %p", filename, mgr);
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

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_pcap_replay_open_file(const char *filename) {

    atsc3_pcap_replay_context = atsc3_pcap_replay_open_filename(filename);
    LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_pcap_replay_open_file: file: %s, replay context: %p", filename, atsc3_pcap_replay_context);
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

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::PcapProducerThreadParserRun() {

    int packet_push_count = 0;

    LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::PcapProducerThreadParserRun with this: %p", this);

    if(!atsc3_pcap_replay_context) {
        LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::PcapProducerThreadParserRun - ERROR - no atsc3_pcap_replay_context!");
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
                    LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::RunPcapThreadParser - pushing to atsc3_phy_mmt_player_bridge_process_packet_phy: count: %d, len was: %d, new payload: %p (0x%02x 0x%02x), len: %d",
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
                    //printf("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::PcapProducerThreadRun - signalling notify_one at count: %d", pushed_count);
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
        LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::RunPcapThreadParser - unwinding thread, end of file!");
    } else {
        LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::RunPcapThreadParser - unwinding thread, pcapThreadShouldRun is false");
    }

    pcapProducerShutdown = true;

    //thread unwound here
    return 0;
}



int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::PcapConsumerThreadRun() {


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

        //printf("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::PcapConsumerThreadRun - pushing %d packets", to_dispatch_queue.size());
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

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::RxThread()
{

    return 0;


}

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::Tune(int freqKHz, int plpid)
{
    apiImpl.Tune(freqKHz, plpid);

    return 0;
}

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::Stop()
{

    return 0;
}

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::Reset()
{

    return 0;
}

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::Close()
{

    return 0;
}

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::Uninit()
{

    return 0;
}





void atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::LogMsg(const char *msg)
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

void atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::LogMsg(const std::string &str)
{
    LogMsg(str.c_str());
}

void atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::LogMsgF(const char *fmt, ...)
{
    va_list v;
    char msg[1024];
    va_start(v, fmt);
    vsnprintf(msg, sizeof(msg)-1, fmt, v);
    msg[sizeof(msg)-1] = 0;
    va_end(v);
    LogMsg(msg);
}

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_pcap_thread_run() {
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
        atsc3_jni_pcap_producer_thread_env = new CJniEnv(mJavaVM);

        LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_pcap_producer_thread_run with this: %p", this);

        this->PcapProducerThreadParserRun();
        delete atsc3_jni_pcap_producer_thread_env;
    });

    pcapConsumerThreadPtr = std::thread([this](){
        atsc3_jni_pcap_consumer_thread_env = new CJniEnv(mJavaVM);
        Atsc3_Jni_Processing_Thread_Env = atsc3_jni_pcap_consumer_thread_env; //hack
        LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_pcap_consumer_thread_run with this: %p", this);

        this->PcapConsumerThreadRun();
        Atsc3_Jni_Processing_Thread_Env = NULL;
        delete atsc3_jni_pcap_consumer_thread_env;
    });


    return 0;
}

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::pinFromRxCaptureThread() {
    printf("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::Atsc3_Jni_Processing_Thread_Env: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Processing_Thread_Env = new CJniEnv(mJavaVM);
    return 0;
};

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::pinFromRxProcessingThread() {
    printf("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::pinFromRxProcessingThread: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Processing_Thread_Env = new CJniEnv(mJavaVM);
    return 0;
}


int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::pinFromRxStatusThread() {
    printf("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::pinFromRxStatusThread: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Status_Thread_Env = new CJniEnv(mJavaVM);
    return 0;
}

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::PcapLocalCleanup() {
    int spinlock_count = 0;
    while(spinlock_count++ < 10 && (!pcapProducerShutdown || !pcapConsumerShutdown)) {
        LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::PcapLocalCleanup: waiting for pcapProducerShutdown: %d, pcapConsumerShutdown: %d, pcapThreadShouldRun: %d",
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

int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_pcap_thread_stop() {

    pcapThreadShouldRun = false;
    LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_pcap_thread_stop with this: %p", &pcapProducerThreadPtr);
    if(pcapProducerThreadPtr.joinable()) {
        pcapProducerThreadPtr.join();
    }

    if(pcapConsumerThreadPtr.joinable()) {
        pcapConsumerThreadPtr.join();
    }
    LogMsgF("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_pcap_thread_stop: stopped with this: %p", &pcapProducerThreadPtr);

    PcapLocalCleanup();
    return 0;
}

//E_AT3_FESTAT
void atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::RxStatusThread() {

}

void atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_update_rf_stats(int32_t tuner_lock,
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
        eprintf("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY:atsc3_update_rf_stats: err on get jni env: Atsc3_Jni_Status_Thread_Env\n");
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


void atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::atsc3_update_rf_bw_stats(uint64_t total_pkts, uint64_t total_bytes,
                                                 unsigned int total_lmts) {
    if (!JReady() || !mOnLogMsgId)
        return;
    if (!Atsc3_Jni_Status_Thread_Env) {
        eprintf("atsc3NdkPHYBridge_DemuxedPcapVirtualPHY:atsc3_update_rf_bw_stats: err on get jni env: Atsc3_Jni_Status_Thread_Env\n");
        return;
    }
    int r = Atsc3_Jni_Status_Thread_Env->Get()->CallIntMethod(mClsDrvIntf, atsc3_update_rf_bw_stats_ID, total_pkts, total_bytes, total_lmts);
}

//Java to native methods



void atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::set_plp_settings(jint *a_plp_ids, jsize a_plp_size) {

    uint8_t* u_plp_ids = (uint8_t*)calloc(a_plp_size, sizeof(uint8_t));
    for(int i=0; i < a_plp_size; i++) {
        u_plp_ids[i] = (uint8_t)a_plp_ids[i];
    }

    //AT3DRV_FE_SetPLP
   // AT3DRV_FE_SetPLP(mhDevice, u_plp_ids, a_plp_size);

}

std::string atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::get_android_temp_folder() {
    CJniEnv env(mJavaVM);

    jclass clazz = env.Get()->FindClass("org/ngbp/libatsc3/sampleapp/atsc3NdkPHYBridge_DemuxedPcapVirtualPHY");

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

//--------------------------------------------------------------------------

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiInit(JNIEnv *env, jobject instance, jobject drvIntf)
{
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiInit: start init, env: %p\n", env);

    api.mJniEnv = env;

    env->GetJavaVM(&api.mJavaVM);
    if(api.mJavaVM == NULL) {
        eprintf("!! no java vm\n");
        return -1;
    }

    jclass jClazz = env->FindClass("org/ngbp/libatsc3/middleware/phy/virtual/DemuxedPcapVirtualPHY");
    if (jClazz == NULL) {
        eprintf("!! Cannot find org/ngbp/libatsc3/middleware/phy/virtual/DemuxedPcapVirtualPHY java class\n");
        return -1;
    }
    api.mOnLogMsgId = env->GetMethodID(jClazz, "onLogMsg", "(Ljava/lang/String;)I");
    if (api.mOnLogMsgId == NULL) {
        eprintf("!! Cannot find 'onLogMsg' method id\n");
        return -1;
    }

    api.atsc3_rf_phy_status_callback_ID = env->GetMethodID(jClazz, "atsc3_rf_phy_status_callback", "(IIIIIIIIIIIIIII)I");
    if (api.atsc3_rf_phy_status_callback_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_rf_phy_status_callback' method id\n");
        return -1;
    }

    //atsc3_update_rf_bw_stats_ID
    api.atsc3_update_rf_bw_stats_ID = env->GetMethodID(jClazz, "atsc3_updateRfBwStats", "(JJI)I");
    if (api.atsc3_update_rf_bw_stats_ID == NULL) {
        eprintf("!! Cannot find 'atsc3_update_rf_bw_stats_ID' method id\n");
        return -1;
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
    //atsc3NdkPHYBridge_DemuxedPcapVirtualPHY* at3DrvIntf_ptr

    atsc3_phy_player_bridge_init(&api);

    printf("**** jni init OK\n");
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiPrepare(JNIEnv *env, jobject instance, jstring devlist_, jint d1, jint d2)
{
    printf("jni prepare\n");

    const char *devlist = env->GetStringUTFChars(devlist_, 0);
    int r = api.Prepare(devlist, (int)d1, (int)d2);
    env->ReleaseStringUTFChars(devlist_, devlist);
    return r;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiFwLoad(JNIEnv *env, jobject instance, jlong key)
{
    printf("jni fwload\n");

//    int r = api.FwLoad((AT3_DEV_KEY) key);
//    return r;

    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiOpen(JNIEnv *env, jobject instance, jint fd, jlong key)
{
    int bus = (key >> 8) & 0xFF;
    int addr = key & 0xFF;

    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiOpen: invoking open with fd: %d, key: %d, bus: %d, addr: %d",
            fd, key, bus, addr);

    api.Open(fd, bus, addr);

    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiTune(JNIEnv *env, jobject instance, jint freqKHz, jint plpid)
{
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiTune::tune\n");

    return api.Tune(freqKHz, plpid);
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiStop(JNIEnv *env, jobject instance)
{
    printf("jni stop\n");
    return api.Stop();
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiClose(JNIEnv *env, jobject instance)
{
    printf("ApiClose:\n");
    return api.Close();
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiUninit(JNIEnv *env, jobject instance)
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
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiReset(JNIEnv *env, jobject instance)
{
    printf("jni reset:\n");
    return api.Reset();
}

extern "C" JNIEXPORT jlongArray JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiFindDeviceKey(JNIEnv *env, jobject instance, jboolean bPreBootDevice)
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
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1pcap_1open_1for_1replay_1from_1assetManager(JNIEnv *env, jobject thiz, jstring filename_,
                                                        jobject asset_manager_weak) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1pcap_1open_1for_1replay_1from_1assetManager\n");

    const char* filename_weak = env->GetStringUTFChars(filename_, 0);

    //global ref and AAsetManager ptr null'd in PcapLocalCleanup
    api.global_pcap_asset_manager_ref = env->NewGlobalRef(asset_manager_weak);

    AAssetManager* aasetManager = AAssetManager_fromJava(env, asset_manager_weak);
    int r = api.atsc3_pcap_replay_open_file_from_assetManager(filename_weak, aasetManager);
    env->ReleaseStringUTFChars(filename_, filename_weak);

    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1pcap_1open_1for_1replay_1from_1assetManager - return: %d\n", r);
    return r;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1pcap_1thread_1run(JNIEnv *env, jobject thiz) {

    printf("::Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApPcapThreadRun\n");

    int r = api.atsc3_pcap_thread_run();
    return r;

}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1pcap_1thread_1stop(JNIEnv *env, jobject thiz) {

    printf("::Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1pcap_1thread_1stop\n");

    int r = api.atsc3_pcap_thread_stop();
    return r;

}
extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1slt_1selectService(JNIEnv *env, jobject thiz,
                                                         jint service_id) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1slt_1selectService, service_id: %d\n", (int)service_id);
    int ret = api.atsc3_slt_selectService((int)service_id);

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_ApiSetPLP(JNIEnv *env, jobject thiz, jintArray a_plp_ids) {
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
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1pcap_1open_1for_1replay(JNIEnv *env, jobject thiz,
                                                                  jstring filename_) {

    const char* filename_weak = env->GetStringUTFChars(filename_, 0);

    int ret = api.atsc3_pcap_replay_open_file(filename_weak);

    env->ReleaseStringUTFChars( filename_, filename_weak );

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1slt_1alc_1select_1additional_1service(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jint service_id) {

    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1slt_1alc_1select_1additional_1service, additional service_id: %d\n", (int)service_id);
    int ret = api.atsc3_slt_alc_select_additional_service((int)service_id);

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1slt_1alc_1clear_1additional_1service_1selections(
        JNIEnv *env, jobject thiz) {
    printf("Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_atsc3_1slt_1alc_1clear_1additional_1service_1selections\n");
    int ret = api.atsc3_slt_alc_clear_additional_service_selections();
    return ret;
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_atsc3NdkPHYBridge_DemuxedPcapVirtualPHY_setRfPhyStatisticsViewVisible(JNIEnv *env, jobject thiz, jboolean is_rf_phy_statistics_visible) {
    if(is_rf_phy_statistics_visible) {
        atsc3NdkClientNoPhyImpl::tunerStatusThreadShouldPollTunerStatus = true;
    } else {
        atsc3NdkClientNoPhyImpl::tunerStatusThreadShouldPollTunerStatus = false;
    }

    return 0;
}



int atsc3NdkPHYBridge_DemuxedPcapVirtualPHY::ListenPLP1(int plp1) {
    //apiImpl.ListenPLP1(plp1);
    return 0;
}
