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

#include <sys/types.h>

using namespace std;

#define DEBUG 1

#include "../IAtsc3NdkPHYClient.h"

// libatsc3 type imports here
#include <atsc3_utils.h>
#include <atsc3_pcap_type.h>
#include <atsc3_route_package_utils.h>
#include <atsc3_core_service_player_bridge.h>

/*
 * : public libatsc3_Iphy_mockable
 */

class PcapDemuxedVirtualPHY : public IAtsc3NdkPHYClient
{
public:
    //PcapDemuxedVirtualPHY();

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

    int atsc3_pcap_replay_open_file(const char *filename);
    int atsc3_pcap_thread_run();
    int atsc3_pcap_thread_stop();

protected:

    std::thread mhRxThread;

    // statistics
    uint32_t s_ulLastTickPrint;
    uint64_t s_ullTotalBytes = 0;
    uint64_t s_ullTotalPkts;
    unsigned s_uTotalLmts = 0;
    std::map<std::string, unsigned> s_mapIpPort;

    //pcap replay context and locals
    int PcapProducerThreadParserRun();
    int PcapConsumerThreadRun();
    int PcapLocalCleanup();

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

    //alc service monitoring
    vector<int>                     atsc3_slt_alc_additional_services_monitored;

    std::thread atsc3_rxStatusThread;
    void RxStatusThread();
    bool rxStatusThreadShouldRun;
};

#define PCAP_DEMUXED_VIRTUAL_PHY_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define PCAP_DEMUXED_VIRTUAL_PHY_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define PCAP_DEMUXED_VIRTUAL_PHY_INFO(...)   	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define PCAP_DEMUXED_VIRTUAL_PHY_DEBUG(...)   	__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);



#endif //LIBATSC3_PCAPDEMUXEDVIRTUALPHY_H
