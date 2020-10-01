//
// Created by Jason Justman on 09/23/20.
//

#include <string.h>
#include <jni.h>
#include <thread>
#include <map>
#include <utility>
#include <queue>
#include <mutex>
#include <semaphore.h>
using namespace std;

#include <Atsc3LoggingUtils.h>
#include <Atsc3JniEnv.h>
#include <atsc3_utils.h>
#include <atsc3_sl_tlv_demod_type.h>
#include <atsc3_alp_parser.h>

#include <atsc3_core_service_player_bridge.h>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_AIRWAVZPHYANDROID_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_AIRWAVZPHYANDROID_H

#include <Atsc3NdkPHYAirwavzStaticJniLoader.h>

#include "props_api.h"
#include "redzone_c_api.h"
#include "basebandparser.h"

typedef void * (*THREADFUNCPTR)(void *);

typedef struct atsc3RedZoneParserCallbackData
{
    enum RedZoneOperatingMode device_mode;
    void* airwavzPHYAndroid_instance;
} atsc3RedZoneParserCallbackData_t;

class AirwavzPHYAndroid : public IAtsc3NdkPHYClient {

public:
    static mutex CS_global_mutex;

    AirwavzPHYAndroid(JNIEnv* env, jobject jni_instance);

    virtual int  init()       override;
    virtual int  run()        override;
    virtual bool is_running() override;
    virtual int  stop()       override;
    virtual int  deinit()     override;

    virtual int  open(int fd, string device_path)   override;
    virtual int  tune(int freqKhz, int single_plp) override;
    virtual int  listen_plps(vector<uint8_t> plps) override;

    virtual ~AirwavzPHYAndroid();

    static void NotifyPlpSelectionChangeCallback(vector<uint8_t> plps, void* context);

    static void BasebandParserALPCallbackGlobal(uint32_t plpId, const uint8_t *pPacket, int32_t sPacket, void *pUserData);
    void basebandParserALPCallbackPushQueue(uint8_t plpId, const uint8_t *pPacket, int32_t sPacket);

    static void RedZoneCaptureBasebandPacketCallbackGlobal(RedZoneCaptureBasebandPacket *pPacket, void *pUserData);

    RZRBasebandParserHandle_t getHBasebandParser() { return  hBasebandParser; }

protected:

    void pinConsumerThreadAsNeeded() override;
    void releasePinnedConsumerThreadAsNeeded() override;
    Atsc3JniEnv* consumerJniEnv = nullptr;

    void pinStatusThreadAsNeeded() override;
    void releasePinnedStatusThreadAsNeeded() override;
    Atsc3JniEnv* statusJniEnv = nullptr;

    JNIEnv* env = nullptr;
    jobject jni_instance_globalRef = nullptr;

private:

    bool init_completed = false;
    bool is_tuned = false;

    //Producer threading is handled inside of Airwavz Redzone SDK,
    //we push to our queue and let our processingThread do nonblocking work...

    //uses      pinConsumerThreadAsNeeded
    int         processThread();
    std::thread processThreadHandle;
    bool        processThreadShouldRun = false;
    bool        processThreadIsRunning = false;

    //uses      pinStatusThreadAsNeeded
    int         statusThread();
    std::thread statusThreadHandle;
    bool        statusThreadShouldRun = false;
    bool        statusThreadIsRunning = false;

    queue<pair<uint8_t, block_t*>>                airwavz_phy_rx_data_buffer_queue;
    mutex                                         airwavz_phy_rx_data_buffer_queue_mutex;
    condition_variable                            airwavz_phy_rx_data_buffer_condition;

    void resetStatstics();
    uint64_t alp_completed_packets_parsed = 0;
    uint64_t alp_total_bytes = 0;
    uint64_t alp_total_LMTs_recv = 0;

    //Airwavz api specific methods

    RedZoneCaptureHandle_t              hRedZoneCapture;
    bool                                hRedZoneCapture_open = false;
    bool                                hRedZoneCapture_started = false;

    RZRBasebandParserHandle_t           hBasebandParser;
    bool                                hBasebandParser_open = false;

    atsc3RedZoneParserCallbackData_t    atsc3RedZoneParserCallbackData;
};

#define _AIRWAVZ_PHY_ANDROID_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _AIRWAVZ_PHY_ANDROID_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _AIRWAVZ_PHY_ANDROID_INFO(...)    	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _AIRWAVZ_PHY_ANDROID_DEBUG(...)     __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define _AIRWAVZ_PHY_ANDROID_TRACE(...)     __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);


#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_AIRWAVZPHYANDROID_H
