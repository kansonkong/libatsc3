//
// Created by Jason Justman on 2019-09-27.
//

#ifndef LIBATSC3_PCAPDEMUXEDVIRTUALPHY_H
#define LIBATSC3_PCAPDEMUXEDVIRTUALPHY_H

#include <string>
#include <thread>
#include <map>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <list>
#include <condition_variable>

#include <sys/types.h>

using namespace std;

#define DEBUG 1

#include "../IAtsc3NdkPHYClient.h"

// libatsc3 type imports here
#include <atsc3_utils.h>
#include <atsc3_pcap_type.h>

/*
 * : public libatsc3_Iphy_mockable
 * defer: #include <atsc3_core_service_player_bridge.h>
 *void atsc3_core_service_bridge_process_packet_phy(block_t* packet);
 *
 */

class PcapDemuxedVirtualPHY : public IAtsc3NdkPHYClient
{
public:
    //PcapDemuxedVirtualPHY();

	virtual int  init();
	virtual int  run();
	virtual bool is_running();
	virtual int  stop();
	virtual int  deinit();

    /*
     * pcap methods
     */

    int atsc3_pcap_replay_open_file(const char *filename);
    int atsc3_pcap_thread_run();
    int atsc3_pcap_thread_stop(); 							//will invoke cleanup of context

    bool is_pcap_replay_running();
    atsc3_pcap_replay_context_t* get_pcap_replay_context_status_volatile(); //treat this as const*

    ~PcapDemuxedVirtualPHY() {
    	atsc3_pcap_thread_stop(); //cleanup just to be sure..
    }
protected:

    //pcap replay context and locals
    int PcapProducerThreadParserRun();
    int PcapConsumerThreadRun();
    int PcapLocalCleanup();


    //local member variables for pcap replay
    char*                           pcap_replay_filename = NULL;
    bool                            pcapThreadShouldRun;

    std::thread                     pcapProducerThreadPtr;
    bool                            pcapProducerShutdown = true;

    std::thread                     pcapConsumerThreadPtr;
    bool                            pcapConsumerShutdown = true;

    atsc3_pcap_replay_context_t*    atsc3_pcap_replay_context = NULL;

    queue<block_t*>                 pcap_replay_buffer_queue;
    mutex                           pcap_replay_buffer_queue_mutex;
    condition_variable              pcap_replay_condition;



};

#define _PCAP_DEMUXED_VIRTUAL_PHY_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _PCAP_DEMUXED_VIRTUAL_PHY_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _PCAP_DEMUXED_VIRTUAL_PHY_INFO(...)   	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _PCAP_DEMUXED_VIRTUAL_PHY_DEBUG(...)   	__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);



#endif //LIBATSC3_PCAPDEMUXEDVIRTUALPHY_H
