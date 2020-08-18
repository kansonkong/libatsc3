//
// Created by Jason Justman on 2019-09-27.
//

#ifndef LIBATSC3_SRTSTLTPVIRTUALPHY_H
#define LIBATSC3_SRTSTLTPVIRTUALPHY_H

#include <iostream>
#include <queue>
#include <thread>



using namespace std;

#define DEBUG 1

#include <phy/IAtsc3NdkPHYClient.h>

// libatsc3 type imports here
#include <atsc3_utils.h>
#include <atsc3_pcap_type.h>
#include <atsc3_stltp_parser.h>
#include <atsc3_alp_types.h>
#include <atsc3_alp_parser.h>
#include <atsc3_stltp_depacketizer.h>
#include <phy/virtual/srt/atsc3_srt_live_receiver.h>

/*
 * : public libatsc3_Iphy_mockable
 *  * defer: #include <atsc3_core_service_player_bridge.h>
 *   void atsc3_core_service_bridge_process_packet_phy(block_t* packet);
 *
 */

class SRTRxSTLTPVirtualPHY : public IAtsc3NdkPHYClient
{
public:
	SRTRxSTLTPVirtualPHY(string srtConnectionSource);

    int Init();
    int Prepare(const char *strDevListInfo, int delim1, int delim2);
    int Open(int fd, int bus, int addr);
    int Tune(int freqKHz, int plpId);
    int Stop();
    int Close();
    int Reset();
    int Uninit();

    /*
     * pcap methods
     */

    //configure one-shot listener for single PLP flow from STLTP
    void atsc3_pcap_stltp_listen_ip_port_plp(string ip, string port, uint8_t plp);

    int atsc3_srt_thread_run();
    int atsc3_srt_thread_stop(); 							//will invoke cleanup of context

    bool is_srt_running();

    //special "friend" callback from srt_live_receiver context
    static void Atsc3_srt_live_rx_udp_packet_process_callback_with_context(block_t* block, void* context);
    void atsc3_srt_live_rx_udp_packet_received(block_t* block);

    //special "friend" callback from stltp_depacketizer context
    static void Atsc3_stltp_baseband_alp_packet_collection_callback_with_context(uint8_t plp, atsc3_alp_packet_collection_t* atsc3_alp_packet_collection, void* context);
    void atsc3_stltp_baseband_alp_packet_collection_received(uint8_t plp, atsc3_alp_packet_collection_t* atsc3_alp_packet_collection);


    ~SRTRxSTLTPVirtualPHY() {
    	atsc3_srt_thread_stop(); //cleanup just to be sure..
    	atsc3_stltp_depacketizer_context_free(&atsc3_stltp_depacketizer_context);
    	atsc3_srt_live_receiver_context_free(&atsc3_srt_live_receiver_context);
    }
protected:

    //pcap replay context and locals
    int srtProducerThreadRun();
    int srtConsumerThreadRun();
    int srtLocalCleanup();

    //overloadable callbacks for Android to pin mJavaVM as needed
    virtual void pinProducerThreadAsNeeded() { };
    virtual void releasePinProducerThreadAsNeeded() { };

    virtual void pinConsumerThreadAsNeeded() { };
    virtual void releaseConsumerThreadAsNeeded() { };


    //local member variables for srt management

    //use atsc3_srt_live_receiver_context->should_run
    //bool                            srtThreadShouldRun = true;

    std::thread                     srtProducerThreadPtr;
    bool                            srtProducerShutdown = true;

    std::thread                     srtConsumerThreadPtr;
    bool                            srtConsumerShutdown = true;

    queue<block_t*>                 srt_rx_buffer_queue;
    mutex                           srt_rx_buffer_queue_mutex;
    condition_variable              srt_rx_condition;

    atsc3_srt_live_receiver_context_t* 		atsc3_srt_live_receiver_context;

    //STLTP depacketizer context
    //build map of PLP to context's

    atsc3_stltp_depacketizer_context_t* 	atsc3_stltp_depacketizer_context;

    /*
     * depacketizer will need to dispatch via
     *             if(this->atsc3_rx_udp_packet_process_callback) {
            	this->atsc3_rx_udp_packet_process_callback(phy_payload_to_process);
            }
     *
     */

//SRTRxSTLTPVirtualPHY

};

#define SRTRXSTLTP_VIRTUAL_PHY_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define SRTRXSTLTP_VIRTUAL_PHY_WARN(...)  	 	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define SRTRXSTLTP_VIRTUAL_PHY_INFO(...)   		__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define SRTRXSTLTP_VIRTUAL_PHY_DEBUG(...)   	__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);



#endif //LIBATSC3_SRTSTLTPVIRTUALPHY_H
