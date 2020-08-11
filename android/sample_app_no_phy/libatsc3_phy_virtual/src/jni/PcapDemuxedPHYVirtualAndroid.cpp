#include "PcapDemuxedPHYVirtualAndroid.h"

PcapDemuxedVirtualPHYAndroid* api;

int PcapDemuxedVirtualPHYAndroid::Init()
{
    printf("%s:%s", __FILE__, __func__);
    return 0;
}

int PcapDemuxedVirtualPHYAndroid::Prepare(const char *strDevListInfo, int delim1, int delim2)
{
    return 0;
}

int PcapDemuxedVirtualPHYAndroid::Open(int fd, int bus, int addr)
{
    return 0;
}

//used for inclusion of pcap's via android assetManager
int PcapDemuxedVirtualPHYAndroid::atsc3_pcap_replay_open_file_from_assetManager(const char *filename,
                                                                                           AAssetManager *mgr) {

    if(!filename) {
        return -1;
    }

    if(pcapThreadShouldRun) {
        //shutdown and restart
        atsc3_pcap_thread_stop();
    }

    NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::atsc3_pcap_open_for_replay_from_assetManager: filename: %s, aasetManager: %p", filename, mgr);
    pcap_replay_filename = (char*)calloc(strlen(filename)+1, sizeof(char));
    strncpy(pcap_replay_filename, filename, strlen(filename));

    pcap_replay_asset_ref_ptr = AAssetManager_open(mgr, (const char *) pcap_replay_filename, AASSET_MODE_UNKNOWN);
    if (NULL == pcap_replay_asset_ref_ptr) {
        NDK_PCAP_DEMUXED_VIRTUAL_PHY_ERROR("atsc3_pcap_open_for_replay_from_assetManager, asset not found!");
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

int PcapDemuxedVirtualPHYAndroid::atsc3_pcap_replay_open_file(const char *filename) {

    atsc3_pcap_replay_context = atsc3_pcap_replay_open_filename(filename);
    NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::atsc3_pcap_replay_open_file: file: %s, replay context: %p", filename, atsc3_pcap_replay_context);
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

int PcapDemuxedVirtualPHYAndroid::PcapProducerThreadParserRun() {

    int packet_push_count = 0;

    NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::PcapProducerThreadParserRun with this: %p", this);

    if(!atsc3_pcap_replay_context) {
        NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::PcapProducerThreadParserRun - ERROR - no atsc3_pcap_replay_context!");
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
                    NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::RunPcapThreadParser - pushing to atsc3_core_service_bridge_process_packet_phy: count: %d, len was: %d, new payload: %p (0x%02x 0x%02x), len: %d",
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
                    //printf("PcapDemuxedVirtualPHYAndroid::PcapProducerThreadRun - signalling notify_one at count: %d", pushed_count);
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
        NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::RunPcapThreadParser - unwinding thread, end of file!");
    } else {
        NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::RunPcapThreadParser - unwinding thread, pcapThreadShouldRun is false");
    }

    pcapProducerShutdown = true;

    //thread unwound here
    return 0;
}



int PcapDemuxedVirtualPHYAndroid::PcapConsumerThreadRun() {


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

        //printf("PcapDemuxedVirtualPHYAndroid::PcapConsumerThreadRun - pushing %d packets", to_dispatch_queue.size());
        while(to_dispatch_queue.size()) {
            block_t *phy_payload_to_process = to_dispatch_queue.front();
            //jjustman-2019-11-06 moved  to semaphore producer/consumer thread for processing pcap replay in time-sensitive phy simulation
       //     atsc3_core_service_bridge_process_packet_phy(phy_payload_to_process);

            to_dispatch_queue.pop();
            block_Destroy(&phy_payload_to_process);
        }
    }
    pcapConsumerShutdown = true;

    return 0;
}


int PcapDemuxedVirtualPHYAndroid::Tune(int freqKHz, int plpid)
{
    return 0;
}

int PcapDemuxedVirtualPHYAndroid::Stop()
{
    api->atsc3_pcap_thread_stop();
    return 0;
}

int PcapDemuxedVirtualPHYAndroid::Reset()
{
    return 0;
}

int PcapDemuxedVirtualPHYAndroid::Close()
{
    return 0;
}

int PcapDemuxedVirtualPHYAndroid::Uninit()
{
    return 0;
}


int PcapDemuxedVirtualPHYAndroid::atsc3_pcap_thread_run() {
    pcapThreadShouldRun = false;
    NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("atsc3_pcap_thread_run: checking for previous pcap_thread: producerShutdown: %d, consumerShutdown: %d", pcapProducerShutdown, pcapConsumerShutdown);

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
        atsc3_jni_pcap_producer_thread_env = new Atsc3JniEnv(mJavaVM);

        NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::atsc3_pcap_producer_thread_run with this: %p", this);

        this->PcapProducerThreadParserRun();
        delete atsc3_jni_pcap_producer_thread_env;
    });

    pcapConsumerThreadPtr = std::thread([this](){
        atsc3_jni_pcap_consumer_thread_env = new Atsc3JniEnv(mJavaVM);
        Atsc3_Jni_Processing_Thread_Env = atsc3_jni_pcap_consumer_thread_env; //hack
        NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::atsc3_pcap_consumer_thread_run with this: %p", this);

        this->PcapConsumerThreadRun();
        Atsc3_Jni_Processing_Thread_Env = NULL;
        delete atsc3_jni_pcap_consumer_thread_env;
    });


    return 0;
}

int PcapDemuxedVirtualPHYAndroid::pinFromRxCaptureThread() {
    printf("PcapDemuxedVirtualPHYAndroid::Atsc3_Jni_Processing_Thread_Env: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Processing_Thread_Env = new Atsc3JniEnv(mJavaVM);
    return 0;
};

int PcapDemuxedVirtualPHYAndroid::pinFromRxProcessingThread() {
    printf("PcapDemuxedVirtualPHYAndroid::pinFromRxProcessingThread: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Processing_Thread_Env = new Atsc3JniEnv(mJavaVM);
    return 0;
}


int PcapDemuxedVirtualPHYAndroid::pinFromRxStatusThread() {
    printf("PcapDemuxedVirtualPHYAndroid::pinFromRxStatusThread: mJavaVM: %p", mJavaVM);
    Atsc3_Jni_Status_Thread_Env = new Atsc3JniEnv(mJavaVM);
    return 0;
}

int PcapDemuxedVirtualPHYAndroid::PcapLocalCleanup() {
    int spinlock_count = 0;
    while(spinlock_count++ < 10 && (!pcapProducerShutdown || !pcapConsumerShutdown)) {
        NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::PcapLocalCleanup: waiting for pcapProducerShutdown: %d, pcapConsumerShutdown: %d, pcapThreadShouldRun: %d",
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
        Atsc3JniEnv env(mJavaVM);
        if (!env) {
            NDK_PCAP_DEMUXED_VIRTUAL_PHY_ERROR("!! err on get jni env");
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

int PcapDemuxedVirtualPHYAndroid::atsc3_pcap_thread_stop() {

    pcapThreadShouldRun = false;
    NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::atsc3_pcap_thread_stop with this: %p", &pcapProducerThreadPtr);
    if(pcapProducerThreadPtr.joinable()) {
        pcapProducerThreadPtr.join();
    }

    if(pcapConsumerThreadPtr.joinable()) {
        pcapConsumerThreadPtr.join();
    }
    NDK_PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHYAndroid::atsc3_pcap_thread_stop: stopped with this: %p", &pcapProducerThreadPtr);

    PcapLocalCleanup();
    return 0;
}

//--------------------------------------------------------------------------

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiInit(JNIEnv *env, jobject instance, jobject drvIntf)
{
    printf("Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiInit: start init, env: %p\n", env);
    api = new PcapDemuxedVirtualPHYAndroid();

    api->mJniEnv = env;

    env->GetJavaVM(&api->mJavaVM);
    if(api->mJavaVM == NULL) {
        NDK_PCAP_DEMUXED_VIRTUAL_PHY_ERROR("!! no java vm");
        return -1;
    }

    jclass jClazz = env->FindClass("org/ngbp/libatsc3/middleware/phy/virtual/DemuxedPcapVirtualPHY");
    if (jClazz == NULL) {
        NDK_PCAP_DEMUXED_VIRTUAL_PHY_ERROR("!! Cannot find org/ngbp/libatsc3/middleware/phy/virtual/DemuxedPcapVirtualPHY java class");
        return -1;
    }

    api->mOnLogMsgId = env->GetMethodID(jClazz, "onLogMsg", "(Ljava/lang/String;)I");
    if (api->mOnLogMsgId == NULL) {
        NDK_PCAP_DEMUXED_VIRTUAL_PHY_ERROR("!! Cannot find 'onLogMsg' method id");
        return -1;
    }

    api->jni_java_util_ArrayList = (jclass) env->NewGlobalRef(env->FindClass("java/util/ArrayList"));
    NDK_PCAP_DEMUXED_VIRTUAL_PHY_ERROR("creating api->jni_java_util_ArrayList");

    api->jni_java_util_ArrayList_cctor = env->GetMethodID(api->jni_java_util_ArrayList, "<init>", "(I)V");
    NDK_PCAP_DEMUXED_VIRTUAL_PHY_ERROR("creating api->jni_java_util_ArrayList_cctor");
    api->jni_java_util_ArrayList_add  = env->GetMethodID(api->jni_java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");
    NDK_PCAP_DEMUXED_VIRTUAL_PHY_ERROR("creating api->jni_java_util_ArrayList_add");

    api->mClsDrvIntf = (jclass)(api->mJniEnv->NewGlobalRef(drvIntf));

    int r = api->Init();
    if (r)
        return r;

    printf("**** jni init OK");
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiPrepare(JNIEnv *env, jobject instance, jstring devlist_, jint d1, jint d2)
{
    printf("jni prepare");

    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiFwLoad(JNIEnv *env, jobject instance, jlong key)
{
    printf("jni fwload");
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiOpen(JNIEnv *env, jobject instance, jint fd, jlong key)
{
    return api->Open(0, 0, 0);
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiTune(JNIEnv *env, jobject instance, jint freqKHz, jint plpid)
{
    printf("Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiTune::tune");
    return api->Tune(0, 0);
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiStop(JNIEnv *env, jobject instance)
{
    return api->Stop();
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiClose(JNIEnv *env, jobject instance)
{
    printf("ApiClose:");
    return api->Close();
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiUninit(JNIEnv *env, jobject instance)
{
    return api->Uninit();
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiReset(JNIEnv *env, jobject instance)
{
    printf("jni reset:");
    return api->Reset();
}

extern "C" JNIEXPORT jlongArray JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiFindDeviceKey(JNIEnv *env, jobject instance, jboolean bPreBootDevice)
{
    return NULL;
}

//hacks

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_atsc3_1pcap_1open_1for_1replay_1from_1assetManager(JNIEnv *env, jobject thiz, jstring filename_,
                                                        jobject asset_manager_weak) {
    printf("Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_atsc3_1pcap_1open_1for_1replay_1from_1assetManager");

    const char* filename_weak = env->GetStringUTFChars(filename_, 0);

    //global ref and AAsetManager ptr null'd in PcapLocalCleanup
    api->global_pcap_asset_manager_ref = env->NewGlobalRef(asset_manager_weak);

    AAssetManager* aasetManager = AAssetManager_fromJava(env, asset_manager_weak);
    int r = api->atsc3_pcap_replay_open_file_from_assetManager(filename_weak, aasetManager);
    env->ReleaseStringUTFChars(filename_, filename_weak);

    printf("Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_atsc3_1pcap_1open_1for_1replay_1from_1assetManager - return: %d\n", r);
    return r;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_atsc3_1pcap_1thread_1run(JNIEnv *env, jobject thiz) {

    printf("::Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApPcapThreadRun");

    int r = api->atsc3_pcap_thread_run();
    return r;

}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_atsc3_1pcap_1thread_1stop(JNIEnv *env, jobject thiz) {

    printf("::Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_atsc3_1pcap_1thread_1stop");

    int r = api->atsc3_pcap_thread_stop();
    return r;

}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_ApiSetPLP(JNIEnv *env, jobject thiz, jintArray a_plp_ids) {

    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_sampleapp_PcapDemuxedVirtualPHYAndroid_atsc3_1pcap_1open_1for_1replay(JNIEnv *env, jobject thiz,
                                                                  jstring filename_) {

    const char* filename_weak = env->GetStringUTFChars(filename_, 0);

    int ret = api->atsc3_pcap_replay_open_file(filename_weak);

    env->ReleaseStringUTFChars( filename_, filename_weak );

    return ret;
}
