#include "PcapSTLTPVirtualPHY.h"


int _ATSC3_PCAP_STLTP_VIRTUAL_PHY_INFO_ENABLED  = 0;
int _ATSC3_PCAP_STLTP_VIRTUAL_PHY_DEBUG_ENABLED = 0;
int _ATSC3_PCAP_STLTP_VIRTUAL_PHY_TRACE_ENABLED = 0;

std::hash<std::thread::id> __pcapSTLTPVirtualPHY_thread_hasher__;


int PcapSTLTPVirtualPHY::init() {

	return 0;
}

int PcapSTLTPVirtualPHY::run() {

	return this->atsc3_pcap_thread_run();
}

bool PcapSTLTPVirtualPHY::is_running() {

	return !pcapProducerShutdown && !pcapConsumerShutdown;
}

int PcapSTLTPVirtualPHY::stop() {
	return this->atsc3_pcap_thread_stop();
}

int PcapSTLTPVirtualPHY::deinit() {
	return 0;
}

PcapSTLTPVirtualPHY::PcapSTLTPVirtualPHY() {
	atsc3_stltp_depacketizer_context = atsc3_stltp_depacketizer_context_new();

	atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_with_context = &PcapSTLTPVirtualPHY::Atsc3_stltp_baseband_alp_packet_collection_callback_with_context;
	atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_context = (void*)this;
}


void PcapSTLTPVirtualPHY::atsc3_pcap_stltp_listen_ip_port_plp(string ip, string port, uint8_t plp) {

	const char* filter_dst_ip = ip.c_str();
	const char* filter_dst_port = port.c_str();

	//parse ip
	uint32_t dst_ip_addr_filter = 0;
	uint16_t dst_port_filter_int = 0;

	dst_ip_addr_filter = parseIpAddressIntoIntval(filter_dst_ip);

	//parse port
	dst_port_filter_int = parsePortIntoIntval(filter_dst_port);

	atsc3_stltp_depacketizer_context->destination_flow_filter.dst_ip_addr = dst_ip_addr_filter;
	atsc3_stltp_depacketizer_context->destination_flow_filter.dst_port = dst_port_filter_int;

	//uint8_t stltp_plp_id = atoi();
	if(plp >=0 && plp <= 63) {
		atsc3_stltp_depacketizer_context->inner_rtp_port_filter = plp + 30000;
	} else if(plp == 255) {
		atsc3_stltp_depacketizer_context->inner_rtp_port_filter = ATSC3_STLTP_DEPACKETIZER_ALL_PLPS_INNER_RTP_PORT;
	} else {
		atsc3_stltp_depacketizer_context->inner_rtp_port_filter = 30000;
	}
	atsc3_stltp_depacketizer_context->context_configured = true;
}

int PcapSTLTPVirtualPHY::atsc3_pcap_replay_open_file(const char *filename) {

    atsc3_pcap_replay_context = atsc3_pcap_replay_open_filename(filename);
    _PCAP_STLTP_VIRTUAL_PHY_INFO("PcapSTLTPVirtualPHY::atsc3_pcap_replay_open_file: file: %s, replay context: %p", filename, atsc3_pcap_replay_context);
    if(!atsc3_pcap_replay_context) {
        return -1;
    }
    return 0;
}


//jjustman-2020-08-10: todo - mutex guard this
int PcapSTLTPVirtualPHY::atsc3_pcap_thread_run() {
    pcapThreadShouldRun = false;
    _PCAP_STLTP_VIRTUAL_PHY_INFO("atsc3_pcap_thread_run: checking for previous pcap_thread: producerShutdown: %d, consumerShutdown: %d", pcapProducerShutdown, pcapConsumerShutdown);


    while(!pcapProducerShutdown || !pcapConsumerShutdown) {
    	usleep(100000);
        _PCAP_STLTP_VIRTUAL_PHY_INFO("atsc3_pcap_thread_run: waiting for shutdown for previous pcap_thread: producerShutdown: %d, consumerShutdown: %d", pcapProducerShutdown, pcapConsumerShutdown);
    }

    if(pcapProducerThreadPtr.joinable()) {
		pcapProducerThreadPtr.join();
	}
	if(pcapConsumerThreadPtr.joinable()) {
		pcapConsumerThreadPtr.join();
	}

    pcapThreadShouldRun = true;
    _PCAP_STLTP_VIRTUAL_PHY_INFO("atsc3_pcap_thread_run: setting pcapthreadShouldRun: %d", pcapThreadShouldRun);

    pcapProducerThreadPtr = std::thread([this](){
		pcapProducerShutdown = false;
    	pinProducerThreadAsNeeded();

        _PCAP_STLTP_VIRTUAL_PHY_INFO("PcapSTLTPVirtualPHY::atsc3_pcap_producer_thread_run with this: %p", this);
        this->PcapProducerThreadParserRun();
        releasePinnedProducerThreadAsNeeded();
    });

    pcapConsumerThreadPtr = std::thread([this](){
    	pcapConsumerShutdown = false;
		pinConsumerThreadAsNeeded();
        _PCAP_STLTP_VIRTUAL_PHY_INFO("PcapSTLTPVirtualPHY::atsc3_pcap_consumer_thread_run with this: %p", this);

        this->PcapConsumerThreadRun();
        releasePinnedConsumerThreadAsNeeded();
    });

    _PCAP_STLTP_VIRTUAL_PHY_INFO("atsc3_pcap_thread_run: threads created, pcapProducerThreadPtr id: %lu, pcapConsumerThreadPtr id: %lu",
                                  __pcapSTLTPVirtualPHY_thread_hasher__(pcapProducerThreadPtr.get_id()),
                                  __pcapSTLTPVirtualPHY_thread_hasher__(pcapConsumerThreadPtr.get_id()));

    return 0;
}

int PcapSTLTPVirtualPHY::PcapLocalCleanup() {
    int spinlock_count = 0;
    while(spinlock_count++ < 10 && (!pcapProducerShutdown || !pcapConsumerShutdown)) {
        _PCAP_STLTP_VIRTUAL_PHY_INFO("PcapSTLTPVirtualPHY::PcapLocalCleanup: waiting for pcapProducerShutdown: %d, pcapConsumerShutdown: %d, pcapThreadShouldRun: %d",
                pcapProducerShutdown, pcapConsumerShutdown, pcapThreadShouldRun);
        usleep(100000);
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

bool PcapSTLTPVirtualPHY::is_pcap_replay_running() {
	return !pcapProducerShutdown && !pcapConsumerShutdown;
}

atsc3_pcap_replay_context_t* PcapSTLTPVirtualPHY::get_pcap_replay_context_status_volatile() {
	return atsc3_pcap_replay_context;
}


int PcapSTLTPVirtualPHY::atsc3_pcap_thread_stop() {

    pcapThreadShouldRun = false;
    _PCAP_STLTP_VIRTUAL_PHY_INFO("PcapSTLTPVirtualPHY::atsc3_pcap_thread_stop with this: %p", &pcapProducerThreadPtr);
    if(pcapProducerThreadPtr.joinable()) {
        pcapProducerThreadPtr.join();
    }

    if(pcapConsumerThreadPtr.joinable()) {
        pcapConsumerThreadPtr.join();
    }
    _PCAP_STLTP_VIRTUAL_PHY_INFO("PcapSTLTPVirtualPHY::atsc3_pcap_thread_stop: stopped with this: %p", &pcapProducerThreadPtr);

    PcapLocalCleanup();
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

int PcapSTLTPVirtualPHY::PcapProducerThreadParserRun() {

    int packet_push_count = 0;
    bool pcapThreadShouldRun_local = true;

    _PCAP_STLTP_VIRTUAL_PHY_INFO("PcapSTLTPVirtualPHY::PcapProducerThreadParserRun with this: %p", this);

    if(!atsc3_pcap_replay_context) {
        _PCAP_STLTP_VIRTUAL_PHY_INFO("PcapSTLTPVirtualPHY::PcapProducerThreadParserRun - ERROR - no atsc3_pcap_replay_context!");
        pcapThreadShouldRun = false;
        return -1;
    }

    atsc3_pcap_replay_context_t* atsc3_pcap_replay_local_context = atsc3_pcap_replay_context;
    while (pcapThreadShouldRun && pcapThreadShouldRun_local) {
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
                    _PCAP_STLTP_VIRTUAL_PHY_DEBUG("PcapSTLTPVirtualPHY::RunPcapThreadParser - pushing to to_dispatch_queue: count: %d, queue current size: %lu, len was: %d, new payload: %p (0x%02x 0x%02x), len: %d",
                            packet_push_count,
							to_dispatch_queue.size(),
                            atsc3_pcap_replay_local_context->atsc3_pcap_packet_instance.current_pcap_packet->p_size,
                            phy_payload,
                            phy_payload->p_buffer[0], phy_payload->p_buffer[1],
                            phy_payload->p_size);
                }

                if(phy_payload->p_size) {
                    to_dispatch_queue.push(phy_payload);
                }

                if(!atsc3_pcap_replay_local_context->delay_delta_behind_rt_replay || to_dispatch_queue.size() > 25) {
                    int pushed_count = to_dispatch_queue.size();
                    lock_guard<mutex> pcap_replay_buffer_queue_guard(pcap_replay_buffer_queue_mutex);
                    while(to_dispatch_queue.size()) {
                        pcap_replay_buffer_queue.push(to_dispatch_queue.front());
                        to_dispatch_queue.pop();
                    }
                    pcap_replay_condition.notify_one();  //todo: jjustman-2019-11-06 - only signal if we aren't behind packet processing or we have a growing queue
                }
                //cleanup happens on the consumer thread for dispatching
                pcapThreadShouldRun_local = !atsc3_pcap_replay_check_file_pos_is_eof(atsc3_pcap_replay_local_context);
            }
        }
    }

    //unlock our consumer thread
    if(!pcapThreadShouldRun) {
        lock_guard<mutex> pcap_replay_buffer_queue_guard(pcap_replay_buffer_queue_mutex);
        pcap_replay_condition.notify_one();
    }

    if(!atsc3_pcap_replay_local_context) {
        _PCAP_STLTP_VIRTUAL_PHY_INFO("PcapSTLTPVirtualPHY::RunPcapThreadParser - unwinding thread, end of file!");
    } else {
        _PCAP_STLTP_VIRTUAL_PHY_INFO("PcapSTLTPVirtualPHY::RunPcapThreadParser - unwinding thread, pcapThreadShouldRun is false");
    }

    pcapProducerShutdown = true;

    //thread unwound here
    return 0;
}

int PcapSTLTPVirtualPHY::PcapConsumerThreadRun() {

    queue<block_t *> to_dispatch_queue; //perform a shallow copy so we can exit critical section asap
    queue<block_t *> to_purge_queue; //perform a shallow copy so we can exit critical section asap
    while (pcapThreadShouldRun) {
        {
            //critical section
            unique_lock<mutex> condition_lock(pcap_replay_buffer_queue_mutex);
            pcap_replay_condition.wait(condition_lock);

            while (pcap_replay_buffer_queue.size()) {
                to_dispatch_queue.push(pcap_replay_buffer_queue.front());
                pcap_replay_buffer_queue.pop();
            }
            condition_lock.unlock();
        }

        //printf("PcapSTLTPVirtualPHY::PcapConsumerThreadRun - pushing %d packets", to_dispatch_queue.size());
        while(to_dispatch_queue.size()) {
            block_t* phy_payload_to_process = to_dispatch_queue.front();

            //jjustman-2020-08-11 - dispatch this for processing against our stltp_depacketizer context
            if(!atsc3_stltp_depacketizer_from_blockt(&phy_payload_to_process, atsc3_stltp_depacketizer_context)) {
            	to_purge_queue.push(phy_payload_to_process); //we were unable to process this block, so purge it ourselves as cleanup
            }
            to_dispatch_queue.pop();
        }

        while(to_purge_queue.size()) {
            block_t *phy_payload_to_purge = to_purge_queue.front();
            to_purge_queue.pop();
            block_Destroy(&phy_payload_to_purge);
        }

    }
    pcapConsumerShutdown = true;

    return 0;
}


void PcapSTLTPVirtualPHY::Atsc3_stltp_baseband_alp_packet_collection_callback_with_context(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection, void* context) {
	PcapSTLTPVirtualPHY* pcapSTLTPVirtualPHY = (PcapSTLTPVirtualPHY*)context;

	pcapSTLTPVirtualPHY->atsc3_stltp_baseband_alp_packet_collection_received(atsc3_alp_packet_collection);
}

/*
 * jjustman-2020-08-11: NOTE - we will only process ALP packets here with packet_type = 0x0
 *
 */
void PcapSTLTPVirtualPHY::atsc3_stltp_baseband_alp_packet_collection_received(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection) {

	for(int i=0; i < atsc3_alp_packet_collection->atsc3_alp_packet_v.count; i++) {
		atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_collection->atsc3_alp_packet_v.data[i];
        block_Rewind(atsc3_alp_packet->alp_payload);

		//if we are an IP packet, push this via our IAtsc3NdkPHYClient callback
		if(atsc3_phy_rx_udp_packet_process_callback && atsc3_alp_packet && atsc3_alp_packet->alp_packet_header.packet_type == 0x0) {
			atsc3_phy_rx_udp_packet_process_callback(atsc3_alp_packet->plp_num, atsc3_alp_packet->alp_payload);
		}
	}
}



