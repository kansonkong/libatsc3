#include "SRTRxSTLTPVirtualPHY.h"

std::hash<std::thread::id> __SRTRxSTLTPVirtualPHY_thread_hasher__;

SRTRxSTLTPVirtualPHY::SRTRxSTLTPVirtualPHY(string srtConnectionSource) {

	atsc3_stltp_depacketizer_context = atsc3_stltp_depacketizer_context_new();

	atsc3_srt_live_receiver_context = atsc3_srt_live_receiver_context_new(srtConnectionSource.c_str());
	atsc3_srt_live_receiver_context_set_rx_udp_packet_process_callback_with_context(atsc3_srt_live_receiver_context, SRTRxSTLTPVirtualPHY::Atsc3_srt_live_rx_udp_packet_process_callback_with_context, (void*) this);

	atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_with_context = &SRTRxSTLTPVirtualPHY::Atsc3_stltp_baseband_alp_packet_collection_callback_with_context;
	atsc3_stltp_depacketizer_context->atsc3_stltp_baseband_alp_packet_collection_callback_context = (void*)this;



}
//
//
//void SRTRxSTLTPVirtualPHY::atsc3_srt_stltp_listen_ip_port_plp(string ip, string port, uint8_t plp) {
//
//	const char* filter_dst_ip = ip.c_str();
//	const char* filter_dst_port = port.c_str();
//
//	//parse ip
//	uint32_t dst_ip_addr_filter = 0;
//	uint16_t dst_port_filter_int = 0;
//
//	char* pch = strtok ((char*)filter_dst_ip,".");
//	int offset = 24;
//	while (pch != NULL && offset>=0) {
//		uint8_t octet = atoi(pch);
//		dst_ip_addr_filter |= octet << offset;
//		offset-=8;
//		pch = strtok (NULL, ".");
//	}
//
//	//parse port
//	dst_port_filter_int = 0xFFFF & atoi(filter_dst_port);
//
//	atsc3_stltp_depacketizer_context->destination_flow_filter.dst_ip_addr = dst_ip_addr_filter;
//	atsc3_stltp_depacketizer_context->destination_flow_filter.dst_port = dst_port_filter_int;
//
//	//uint8_t stltp_plp_id = atoi();
//	if(plp >=0 && plp <= 63) {
//		atsc3_stltp_depacketizer_context->inner_rtp_port_filter = plp + 30000;
//	} else {
//		atsc3_stltp_depacketizer_context->inner_rtp_port_filter = 30000;
//	}
//}

//jjustman-2020-08-10: todo - mutex guard this
int SRTRxSTLTPVirtualPHY::atsc3_srt_thread_run() {
	atsc3_srt_live_receiver_context->should_run = false;
    SRTRXSTLTP_VIRTUAL_PHY_INFO("atsc3_srt_thread_run: checking for previous srt_thread: producerShutdown: %d, consumerShutdown: %d", srtProducerShutdown, srtConsumerShutdown);

    //e.g. must contain at least srt://
    if(atsc3_srt_live_receiver_context->source_connection_string == NULL || strlen(atsc3_srt_live_receiver_context->source_connection_string) < 7) {
        SRTRXSTLTP_VIRTUAL_PHY_ERROR("srtConnectionSource is empty or too short!");
        return -1;
    }

    while(!srtProducerShutdown || !srtConsumerShutdown) {
    	usleep(100000);
        SRTRXSTLTP_VIRTUAL_PHY_INFO("atsc3_srt_thread_run: waiting for shutdown for previous srt_thread: producerShutdown: %d, consumerShutdown: %d", srtProducerShutdown, srtConsumerShutdown);
    }

    if(srtProducerThreadPtr.joinable()) {
		srtProducerThreadPtr.join();
	}
	if(srtConsumerThreadPtr.joinable()) {
		srtConsumerThreadPtr.join();
	}

	atsc3_srt_live_receiver_context->should_run = true;
    SRTRXSTLTP_VIRTUAL_PHY_INFO("atsc3_srt_thread_run: setting atsc3_srt_live_receiver_context->should_run: %d", atsc3_srt_live_receiver_context->should_run);

    srtProducerThreadPtr = std::thread([this](){
		srtProducerShutdown = false;
    	pinProducerThreadAsNeeded();

        SRTRXSTLTP_VIRTUAL_PHY_INFO("SRTRxSTLTPVirtualPHY::atsc3_srt_producer_thread_run with this: %p", this);
        this->srtProducerThreadRun();
        releasePinProducerThreadAsNeeded();
    });

    srtConsumerThreadPtr = std::thread([this](){
    	srtConsumerShutdown = false;
		pinConsumerThreadAsNeeded();
        SRTRXSTLTP_VIRTUAL_PHY_INFO("SRTRxSTLTPVirtualPHY::atsc3_srt_consumer_thread_run with this: %p", this);

        this->srtConsumerThreadRun();
        releaseConsumerThreadAsNeeded();
    });

    SRTRXSTLTP_VIRTUAL_PHY_INFO("atsc3_srt_thread_run: threads created, srtProducerThreadPtr id: %lu, srtConsumerThreadPtr id: %lu",
                                  __SRTRxSTLTPVirtualPHY_thread_hasher__(srtProducerThreadPtr.get_id()),
                                  __SRTRxSTLTPVirtualPHY_thread_hasher__(srtConsumerThreadPtr.get_id()));

    return 0;
}

int SRTRxSTLTPVirtualPHY::srtLocalCleanup() {
    int spinlock_count = 0;
    while(spinlock_count++ < 10 && (!srtProducerShutdown || !srtConsumerShutdown)) {
        SRTRXSTLTP_VIRTUAL_PHY_INFO("SRTRxSTLTPVirtualPHY::srtLocalCleanup: waiting for srtProducerShutdown: %d, srtConsumerShutdown: %d, atsc3_srt_live_receiver_context->should_run: %d",
                srtProducerShutdown, srtConsumerShutdown, atsc3_srt_live_receiver_context->should_run);
        usleep(100000);
    }
    //release any local resources held in our context
	atsc3_stltp_depacketizer_context_free(&atsc3_stltp_depacketizer_context);
    atsc3_srt_live_receiver_context_free(&atsc3_srt_live_receiver_context);

    //release any remaining block_t* payloads in srt_replay_buffer_queue
    while(srt_rx_buffer_queue.size()) {
        block_t* to_free = srt_rx_buffer_queue.front();
        srt_rx_buffer_queue.pop();
        block_Destroy(&to_free);
    }

    return 0;
}

bool SRTRxSTLTPVirtualPHY::is_srt_running() {
	return !atsc3_srt_live_receiver_context->is_shutdown && !srtProducerShutdown && !srtConsumerShutdown;
}


int SRTRxSTLTPVirtualPHY::atsc3_srt_thread_stop() {

	if(atsc3_srt_live_receiver_context && !atsc3_srt_live_receiver_context->is_shutdown) {
		atsc3_srt_live_receiver_notify_shutdown(atsc3_srt_live_receiver_context);
	}

    SRTRXSTLTP_VIRTUAL_PHY_INFO("SRTRxSTLTPVirtualPHY::atsc3_srt_thread_stop with this: %p", &srtProducerThreadPtr);
    if(srtProducerThreadPtr.joinable()) {
        srtProducerThreadPtr.join();
    }

    if(srtConsumerThreadPtr.joinable()) {
        srtConsumerThreadPtr.join();
    }
    SRTRXSTLTP_VIRTUAL_PHY_INFO("SRTRxSTLTPVirtualPHY::atsc3_srt_thread_stop: stopped with this: %p", &srtProducerThreadPtr);

    srtLocalCleanup();
    return 0;
}


/**
 * TODO:  jjustman-2019-10-10: implement srt replay in new superclass
 *         -D__MOCK_srt_REPLAY__ in the interim
 *
 * @return 0
 *
 *
 * borrowed from libatsc3/test/atsc3_srt_replay_test.c
 */

int SRTRxSTLTPVirtualPHY::srtProducerThreadRun() {
	int res = 0;

    int packet_push_count = 0;

    SRTRXSTLTP_VIRTUAL_PHY_INFO("SRTRxSTLTPVirtualPHY::srtProducerThreadRun with this: %p", this);

    if(!atsc3_stltp_depacketizer_context) {
        SRTRXSTLTP_VIRTUAL_PHY_INFO("SRTRxSTLTPVirtualPHY::srtProducerThreadRun - ERROR - no atsc3_stltp_depacketizer_context!");
        atsc3_srt_live_receiver_context->should_run = false;
        return -1;
    }

    atsc3_srt_live_receiver_context->should_run = true;
    res = atsc3_srt_live_receiver_start_in_proc(this->atsc3_srt_live_receiver_context);

//    atsc3_srt_replay_context_t* atsc3_srt_replay_local_context = atsc3_srt_replay_context;
//    while (atsc3_srt_live_receiver_context->should_run) {
//        queue<block_t *> to_dispatch_queue; //perform a shallow copy so we can exit critical section asap
//
//        //_ATSC3_srt_REPLAY_TEST_DEBUG("Opening srt: %s, context is: %p", srt_REPLAY_TEST_FILENAME, atsc3_srt_replay_local_context);
//        if(atsc3_srt_replay_local_context) {
//            while(atsc3_srt_live_receiver_context->should_run && (atsc3_srt_replay_local_context = atsc3_srt_replay_iterate_packet(atsc3_srt_replay_local_context))) {
//                atsc3_srt_replay_usleep_packet(atsc3_srt_replay_local_context);
//                //push block_t as packet buffer to consumer queue
//
//                block_Seek(atsc3_srt_replay_local_context->atsc3_srt_packet_instance.current_srt_packet, ATSC3_srt_ETH_HEADER_LENGTH);
//
//                block_t* phy_payload = block_Duplicate_from_position(atsc3_srt_replay_local_context->atsc3_srt_packet_instance.current_srt_packet);
//                block_Rewind(atsc3_srt_replay_local_context->atsc3_srt_packet_instance.current_srt_packet);
//                if(phy_payload->p_size && (packet_push_count++ % 10000) == 0) {
//                    SRTRXSTLTP_VIRTUAL_PHY_INFO("SRTRxSTLTPVirtualPHY::RunsrtThreadParser - pushing to atsc3_core_service_bridge_process_packet_phy: count: %d, len was: %d, new payload: %p (0x%02x 0x%02x), len: %d",
//                            packet_push_count,
//                            atsc3_srt_replay_local_context->atsc3_srt_packet_instance.current_srt_packet->p_size,
//                            phy_payload,
//                            phy_payload->p_buffer[0], phy_payload->p_buffer[1],
//                            phy_payload->p_size);
//                }
//
//                if(phy_payload->p_size) {
//                    to_dispatch_queue.push(phy_payload);
//                }
//
//                if(!atsc3_srt_replay_local_context->delay_delta_behind_rt_replay || to_dispatch_queue.size() > 10) { //srt_replay_buffer_queue.size() doesn't seem to be accurate...
//                    int pushed_count = to_dispatch_queue.size();
//                    lock_guard<mutex> srt_replay_buffer_queue_guard(srt_rx_buffer_queue_mutex);
//                    while(to_dispatch_queue.size()) {
//                        srt_rx_buffer_queue.push(to_dispatch_queue.front());
//                        to_dispatch_queue.pop();
//                    }
//                    srt_rx_condition.notify_one();  //todo: jjustman-2019-11-06 - only signal if we aren't behind packet processing or we have a growing queue
//                    //printf("SRTRxSTLTPVirtualPHY::srtProducerThreadRun - signalling notify_one at count: %d", pushed_count);
//                }
//                //cleanup happens on the consumer thread for dispatching
//                atsc3_srt_live_receiver_context->should_run = !atsc3_srt_replay_check_file_pos_is_eof(atsc3_srt_replay_local_context);
//            }
//        }
//    }
//

    atsc3_srt_live_receiver_context->should_run = false;
    SRTRXSTLTP_VIRTUAL_PHY_INFO("SRTRxSTLTPVirtualPHY::RunsrtThreadParser - unwinding thread, atsc3_srt_live_receiver_context->should_run is false, res: %d", res);

    //unlock our consumer thread
    lock_guard<mutex> srt_replay_buffer_queue_guard(srt_rx_buffer_queue_mutex);
    srt_rx_condition.notify_one();

    srtProducerShutdown = true;

    //thread unwound here
    return res;
}



int SRTRxSTLTPVirtualPHY::srtConsumerThreadRun() {

    queue<block_t *> to_dispatch_queue; //perform a shallow copy so we can exit critical section asap
    queue<block_t *> to_purge_queue; //perform a shallow copy so we can exit critical section asap
    while (atsc3_srt_live_receiver_context->should_run) {
        {
            //critical section
            unique_lock<mutex> condition_lock(srt_rx_buffer_queue_mutex);
            srt_rx_condition.wait(condition_lock);

            while (srt_rx_buffer_queue.size()) {
                to_dispatch_queue.push(srt_rx_buffer_queue.front());
                srt_rx_buffer_queue.pop();
            }
            condition_lock.unlock();
            srt_rx_condition.notify_one();
        }

        //printf("SRTRxSTLTPVirtualPHY::srtConsumerThreadRun - pushing %d packets", to_dispatch_queue.size());
        while(to_dispatch_queue.size()) {
            block_t* phy_payload_to_process = to_dispatch_queue.front();

            //jjustman-2020-08-11 - dispatch this for processing against our stltp_depacketizer context
            if(!atsc3_stltp_depacketizer_from_blockt(&phy_payload_to_process, atsc3_stltp_depacketizer_context)) {
            	to_purge_queue.push(phy_payload_to_process); //we were unable to process this block, so purge
            }
            to_dispatch_queue.pop();
        }

        while(to_purge_queue.size()) {
            block_t *phy_payload_to_purge = to_purge_queue.front();
            to_purge_queue.pop();
            block_Destroy(&phy_payload_to_purge);
        }

    }
    srtConsumerShutdown = true;

    return 0;
}



void SRTRxSTLTPVirtualPHY::Atsc3_srt_live_rx_udp_packet_process_callback_with_context(block_t* block, void* context) {
	SRTRxSTLTPVirtualPHY* srtRxSTLTPVirtualPHY = (SRTRxSTLTPVirtualPHY*)context;
	srtRxSTLTPVirtualPHY->atsc3_srt_live_rx_udp_packet_received(block);
}

//hand this SRT datagram off to our STLTP listener queue
//jjustman-2020-08-17 - TODO: SRT flows are only a single dip:dport, so we will need to configure the STLTP context accordingly with the first packet from our received flow...
//jjustman-2020-08-17 - TODO: buffer this as needed with an internal queue and then push to srt_rx_buffer_queue
void SRTRxSTLTPVirtualPHY::atsc3_srt_live_rx_udp_packet_received(block_t* block) {
	lock_guard<mutex> srt_replay_buffer_queue_guard(srt_rx_buffer_queue_mutex);
	srt_rx_buffer_queue.push(block_Duplicate(block));
	srt_rx_condition.notify_one();
}


void SRTRxSTLTPVirtualPHY::Atsc3_stltp_baseband_alp_packet_collection_callback_with_context(uint8_t plp, atsc3_alp_packet_collection_t* atsc3_alp_packet_collection, void* context) {
	SRTRxSTLTPVirtualPHY* srtRxSTLTPVirtualPHY = (SRTRxSTLTPVirtualPHY*)context;
	srtRxSTLTPVirtualPHY->atsc3_stltp_baseband_alp_packet_collection_received(plp, atsc3_alp_packet_collection);
}

/*
 * jjustman-2020-08-11: NOTE - we will only process ALP packets here with packet_type = 0x0
 *
 */
void SRTRxSTLTPVirtualPHY::atsc3_stltp_baseband_alp_packet_collection_received(uint8_t plp, atsc3_alp_packet_collection_t* atsc3_alp_packet_collection) {

	for(int i=0; i < atsc3_alp_packet_collection->atsc3_alp_packet_v.count; i++) {
		atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_collection->atsc3_alp_packet_v.data[i];
        block_Rewind(atsc3_alp_packet->alp_payload);

		//if we are an IP packet, push this via our IAtsc3NdkPHYClient callback
		if(atsc3_phy_rx_udp_packet_process_callback && atsc3_alp_packet && atsc3_alp_packet->alp_packet_header.packet_type == 0x0) {
			atsc3_phy_rx_udp_packet_process_callback(plp, atsc3_alp_packet->alp_payload);
		}
	}
}



/*
 * default IPHY impl's here
 */

int SRTRxSTLTPVirtualPHY::Init()
{
    return 0;
}

int SRTRxSTLTPVirtualPHY::Prepare(const char *strDevListInfo, int delim1, int delim2)
{
    return 0;
}

int SRTRxSTLTPVirtualPHY::Open(int fd, int bus, int addr)
{
    return 0;
}

int SRTRxSTLTPVirtualPHY::Tune(int freqKHz, int plpid)
{
    return 0;
}

int SRTRxSTLTPVirtualPHY::Stop()
{
    return 0;
}

int SRTRxSTLTPVirtualPHY::Reset()
{
    return 0;
}

int SRTRxSTLTPVirtualPHY::Close()
{
    return 0;
}

int SRTRxSTLTPVirtualPHY::Uninit()
{
    return 0;
}

