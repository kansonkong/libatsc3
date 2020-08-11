#include "PcapDemuxedVirtualPHY.h"

int PcapDemuxedVirtualPHY::Init()
{
    printf("%s:%s:TODO", __FILE__, __func__);

  //  mbInit = true;
    return 0;
}

int PcapDemuxedVirtualPHY::Prepare(const char *strDevListInfo, int delim1, int delim2)
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
int PcapDemuxedVirtualPHY::Open(int fd, int bus, int addr)
{
    return 0;
}

int PcapDemuxedVirtualPHY::atsc3_pcap_replay_open_file(const char *filename) {

    atsc3_pcap_replay_context = atsc3_pcap_replay_open_filename(filename);
    PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHY::atsc3_pcap_replay_open_file: file: %s, replay context: %p", filename, atsc3_pcap_replay_context);
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

int PcapDemuxedVirtualPHY::PcapProducerThreadParserRun() {

    int packet_push_count = 0;

    PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHY::PcapProducerThreadParserRun with this: %p", this);

    if(!atsc3_pcap_replay_context) {
        PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHY::PcapProducerThreadParserRun - ERROR - no atsc3_pcap_replay_context!");
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
                    PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHY::RunPcapThreadParser - pushing to atsc3_core_service_bridge_process_packet_phy: count: %d, len was: %d, new payload: %p (0x%02x 0x%02x), len: %d",
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
                    //printf("PcapDemuxedVirtualPHY::PcapProducerThreadRun - signalling notify_one at count: %d", pushed_count);
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
        PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHY::RunPcapThreadParser - unwinding thread, end of file!");
    } else {
        PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHY::RunPcapThreadParser - unwinding thread, pcapThreadShouldRun is false");
    }

    pcapProducerShutdown = true;

    //thread unwound here
    return 0;
}



int PcapDemuxedVirtualPHY::PcapConsumerThreadRun() {


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

        //printf("PcapDemuxedVirtualPHY::PcapConsumerThreadRun - pushing %d packets", to_dispatch_queue.size());
        while(to_dispatch_queue.size()) {
            block_t *phy_payload_to_process = to_dispatch_queue.front();
            //jjustman-2019-11-06 moved  to semaphore producer/consumer thread for processing pcap replay in time-sensitive phy simulation
            atsc3_core_service_bridge_process_packet_phy(phy_payload_to_process);

            to_dispatch_queue.pop();
            block_Destroy(&phy_payload_to_process);
        }
    }
    pcapConsumerShutdown = true;

    return 0;
}

int PcapDemuxedVirtualPHY::Tune(int freqKHz, int plpid)
{
    return 0;
}

int PcapDemuxedVirtualPHY::Stop()
{
    return 0;
}

int PcapDemuxedVirtualPHY::Reset()
{
    return 0;
}

int PcapDemuxedVirtualPHY::Close()
{
    return 0;
}

int PcapDemuxedVirtualPHY::Uninit()
{
    return 0;
}


int PcapDemuxedVirtualPHY::atsc3_pcap_thread_run() {
    pcapThreadShouldRun = false;
    PCAP_DEMUXED_VIRTUAL_PHY_INFO("atsc3_pcap_thread_run: checking for previous pcap_thread: producerShutdown: %d, consumerShutdown: %d", pcapProducerShutdown, pcapConsumerShutdown);

    if(pcapProducerThreadPtr.joinable()) {
        pcapProducerThreadPtr.join();
    }
    if(pcapConsumerThreadPtr.joinable()) {
        pcapConsumerThreadPtr.join();
    }
    usleep(500000);

    //jjustman-2019-11-05 - TODO: make sure mhRxThread is terminated before we instantiate a new
    pcapThreadShouldRun = true;

//    pcapProducerThreadPtr = std::thread([this](){
//        atsc3_jni_pcap_producer_thread_env = new Atsc3JniEnv(mJavaVM);
//
//        PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHY::atsc3_pcap_producer_thread_run with this: %p", this);
//
//        this->PcapProducerThreadParserRun();
//        delete atsc3_jni_pcap_producer_thread_env;
//    });
//
//    pcapConsumerThreadPtr = std::thread([this](){
//        atsc3_jni_pcap_consumer_thread_env = new Atsc3JniEnv(mJavaVM);
//        Atsc3_Jni_Processing_Thread_Env = atsc3_jni_pcap_consumer_thread_env; //hack
//        PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHY::atsc3_pcap_consumer_thread_run with this: %p", this);
//
//        this->PcapConsumerThreadRun();
//        Atsc3_Jni_Processing_Thread_Env = NULL;
//        delete atsc3_jni_pcap_consumer_thread_env;
//    });


    return 0;
}
//
//int PcapDemuxedVirtualPHY::pinFromRxCaptureThread() {
////    printf("PcapDemuxedVirtualPHY::Atsc3_Jni_Processing_Thread_Env: mJavaVM: %p", mJavaVM);
////    Atsc3_Jni_Processing_Thread_Env = new Atsc3JniEnv(mJavaVM);
//    return 0;
//};
//
//int PcapDemuxedVirtualPHY::pinFromRxProcessingThread() {
////    printf("PcapDemuxedVirtualPHY::pinFromRxProcessingThread: mJavaVM: %p", mJavaVM);
////    Atsc3_Jni_Processing_Thread_Env = new Atsc3JniEnv(mJavaVM);
//    return 0;
//}
//
//
//int PcapDemuxedVirtualPHY::pinFromRxStatusThread() {
////    printf("PcapDemuxedVirtualPHY::pinFromRxStatusThread: mJavaVM: %p", mJavaVM);
////    Atsc3_Jni_Status_Thread_Env = new Atsc3JniEnv(mJavaVM);
//    return 0;
//}

int PcapDemuxedVirtualPHY::PcapLocalCleanup() {
    int spinlock_count = 0;
    while(spinlock_count++ < 10 && (!pcapProducerShutdown || !pcapConsumerShutdown)) {
        PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHY::PcapLocalCleanup: waiting for pcapProducerShutdown: %d, pcapConsumerShutdown: %d, pcapThreadShouldRun: %d",
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

//    if(pcap_replay_asset_ref_ptr) {
//        AAsset_close(pcap_replay_asset_ref_ptr);
//        pcap_replay_asset_ref_ptr = NULL;
//    }
//
//    //we can close the asset reference, but don't close the AAssetManager GlobalReference
//    if(global_pcap_asset_manager_ref) {
//        Atsc3JniEnv env(mJavaVM);
//        if (!env) {
//            PCAP_DEMUXED_VIRTUAL_PHY_ERROR("!! err on get jni env");
//        } else {
//            env.Get()->DeleteGlobalRef(global_pcap_asset_manager_ref);
//        }
//        global_pcap_asset_manager_ref = NULL;
//    }

    if(pcap_replay_filename) {
        free(pcap_replay_filename);
        pcap_replay_filename = NULL;
    }

    return 0;
}

int PcapDemuxedVirtualPHY::atsc3_pcap_thread_stop() {

    pcapThreadShouldRun = false;
    PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHY::atsc3_pcap_thread_stop with this: %p", &pcapProducerThreadPtr);
    if(pcapProducerThreadPtr.joinable()) {
        pcapProducerThreadPtr.join();
    }

    if(pcapConsumerThreadPtr.joinable()) {
        pcapConsumerThreadPtr.join();
    }
    PCAP_DEMUXED_VIRTUAL_PHY_INFO("PcapDemuxedVirtualPHY::atsc3_pcap_thread_stop: stopped with this: %p", &pcapProducerThreadPtr);

    PcapLocalCleanup();
    return 0;
}
