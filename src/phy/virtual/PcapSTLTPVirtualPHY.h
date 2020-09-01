//
// Created by Jason Justman on 2019-09-27.
//

#ifndef LIBATSC3_PCAPSTLTPVIRTUALPHY_H
#define LIBATSC3_PCAPSTLTPVIRTUALPHY_H

#include <string>
#include <thread>
#include <map>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <list>

#include <sys/types.h>

#include <string.h>
#include <stdlib.h>
#include <condition_variable>


using namespace std;

#define DEBUG 1

#include "../IAtsc3NdkPHYClient.h"

// libatsc3 type imports here
#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>
#include <atsc3_pcap_type.h>
#include <atsc3_stltp_parser.h>
#include <atsc3_alp_parser.h>
#include <atsc3_stltp_depacketizer.h>

/*
 * : public libatsc3_Iphy_mockable
 *  * defer: #include <atsc3_core_service_player_bridge.h>
 *   void atsc3_core_service_bridge_process_packet_phy(block_t* packet);
 *
 */

class PcapSTLTPVirtualPHY : public IAtsc3NdkPHYClient
{
public:
    PcapSTLTPVirtualPHY();

	virtual int  init();
	virtual int  run();
	virtual bool is_running();
	virtual int  stop();
	virtual int  deinit();

    /*
     * pcap methods
     */

    //configure one-shot listener for single PLP flow from STLTP
    void atsc3_pcap_stltp_listen_ip_port_plp(string ip, string port, uint8_t plp);

    int atsc3_pcap_replay_open_file(const char *filename);

    int atsc3_pcap_thread_run();
    int atsc3_pcap_thread_stop(); 							//will invoke cleanup of context

    bool is_pcap_replay_running();
    atsc3_pcap_replay_context_t* get_pcap_replay_context_status_volatile(); 	//treat this as const*
    atsc3_stltp_depacketizer_context_t* get_atsc3_stltp_depacketizer_context() { return this->atsc3_stltp_depacketizer_context; }

    //special "friend" callback from stltp_depacketizer context
    static void Atsc3_stltp_baseband_alp_packet_collection_callback_with_context(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection, void* context);
    void atsc3_stltp_baseband_alp_packet_collection_received(atsc3_alp_packet_collection_t* atsc3_alp_packet_collection);

    ~PcapSTLTPVirtualPHY() {
    	atsc3_pcap_thread_stop(); //cleanup just to be sure..
    	atsc3_stltp_depacketizer_context_free(&atsc3_stltp_depacketizer_context);
    }
protected:

    //pcap replay context and locals
    int PcapProducerThreadParserRun();
    int PcapConsumerThreadRun();
    int PcapLocalCleanup();

    //local member variables for pcap replay
    char*                           pcap_replay_filename = NULL;
    bool                            pcapThreadShouldRun = false;

    std::thread                     pcapProducerThreadPtr;
    bool                            pcapProducerShutdown = true;

    std::thread                     pcapConsumerThreadPtr;
    bool                            pcapConsumerShutdown = true;

    atsc3_pcap_replay_context_t*    atsc3_pcap_replay_context = NULL;

    queue<block_t*>                 pcap_replay_buffer_queue;
    mutex                           pcap_replay_buffer_queue_mutex;
    condition_variable              pcap_replay_condition;

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



};

#define _PCAP_STLTP_VIRTUAL_PHY_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _PCAP_STLTP_VIRTUAL_PHY_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _PCAP_STLTP_VIRTUAL_PHY_INFO(...)   	if(_ATSC3_PCAP_STLTP_VIRTUAL_PHY_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _PCAP_STLTP_VIRTUAL_PHY_DEBUG(...)   	if(_ATSC3_PCAP_STLTP_VIRTUAL_PHY_DEBUG_ENABLED) {__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _PCAP_STLTP_VIRTUAL_PHY_TRACE(...)   	if(_ATSC3_PCAP_STLTP_VIRTUAL_PHY_TRACE_ENABLED) {__LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#endif //LIBATSC3_PCAPSTLTPVIRTUALPHY_H
