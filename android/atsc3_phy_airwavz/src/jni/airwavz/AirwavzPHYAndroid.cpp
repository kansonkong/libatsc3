//
// Created by Jason Justman on 09/23/20
//

#include "AirwavzPHYAndroid.h"
AirwavzPHYAndroid* airwavzPHYAndroid = nullptr;

mutex AirwavzPHYAndroid::CS_global_mutex;

AirwavzPHYAndroid::AirwavzPHYAndroid(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = this->env->NewGlobalRef(jni_instance);
    this->setRxUdpPacketProcessCallback(atsc3_core_service_bridge_process_packet_from_plp_and_block);
    this->setRxLinkMappingTableProcessCallback(atsc3_phy_jni_bridge_notify_link_mapping_table);

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_set_callback(&AirwavzPHYAndroid::NotifyPlpSelectionChangeCallback, this);
    }

    _AIRWAVZ_PHY_ANDROID_INFO("AirwavzPHYAndroid::AirwavzPHYAndroid - created with this: %p", this);
}

AirwavzPHYAndroid::~AirwavzPHYAndroid() {

    _AIRWAVZ_PHY_ANDROID_INFO("AirwavzPHYAndroid::~AirwavzPHYAndroid - enter: deleting with this: %p", this);

    this->stop();

    if(hRedZoneCapture_open) {
        RedZoneCaptureClose(hRedZoneCapture);
    }
    hRedZoneCapture_open = false;

    if(hBasebandParser_open) {
        RZRBasebandParserClose(hBasebandParser);
    }
    hBasebandParser_open = false;

    _AIRWAVZ_PHY_ANDROID_INFO("AirwavzPHYAndroid::~AirwavzPHYAndroid - exit: deleting with this: %p", this);
}

void AirwavzPHYAndroid::pinConsumerThreadAsNeeded() {
    _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::pinConsumerThreadAsNeeded: mJavaVM: %p, atsc3_ndk_application_bridge instance: %p", atsc3_ndk_phy_airwavz_static_loader_get_javaVM(), atsc3_ndk_application_bridge_get_instance());

    consumerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_airwavz_static_loader_get_javaVM(), "AirwavzPHYAndroid::consumerThread");
    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->pinConsumerThreadAsNeeded();
    }
}

void AirwavzPHYAndroid::releasePinnedConsumerThreadAsNeeded() {
    if(consumerJniEnv) {
        delete consumerJniEnv;
        consumerJniEnv = nullptr;
    }

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->releasePinnedConsumerThreadAsNeeded();
    }
}

void AirwavzPHYAndroid::pinStatusThreadAsNeeded() {
    statusJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_airwavz_static_loader_get_javaVM(), "AirwavzPHYAndroid::statusThread");

    if(atsc3_ndk_phy_bridge_get_instance()) {
        atsc3_ndk_phy_bridge_get_instance()->pinStatusThreadAsNeeded();
    }
}

void AirwavzPHYAndroid::releasePinnedStatusThreadAsNeeded() {
    if(statusJniEnv) {
        delete statusJniEnv;
        statusJniEnv = nullptr;
    }

    if(atsc3_ndk_phy_bridge_get_instance()) {
        atsc3_ndk_phy_bridge_get_instance()->releasePinnedStatusThreadAsNeeded();
    }
}

int AirwavzPHYAndroid::init()
{
    _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::init with this: %p", this);

    //LogMsgF("at3drv ver %s, id %s, branch %s\n", vinfo.ver, vinfo.rev_id, vinfo.rev_branch);

    init_completed = true;
    return 0;
}

int AirwavzPHYAndroid::run()
{
    return 0;
}

bool AirwavzPHYAndroid::is_running() {

    return processThreadIsRunning && statusThreadIsRunning;
}

int AirwavzPHYAndroid::stop()
{
    int res = 0;

    //tear down status thread first, as its the most 'problematic'
    if(statusThreadIsRunning) {
        //give AT3DRV_WaitRxData some time to shutdown, may take up to 1.5s
        _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::stop: statusThreadIsRunning waiting for thread to to wind-down, setting statusThreadShoulddRun: false");
        statusThreadShouldRun = false;

        usleep(15 * 100000);

        while(this->statusThreadIsRunning) {
            usleep(100000);
            _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::stop: this->statusThreadIsRunning: %d", this->statusThreadIsRunning);
        }
        if(statusThreadHandle.joinable()) {
            statusThreadHandle.join();
        }
        _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::stop: after join for statusThreadHandle");
    }

    if(processThreadIsRunning) {
        processThreadShouldRun = false;
        //unlock our producer thread, RAII scoped block to notify
        {
            lock_guard<mutex> airwavz_phy_rx_data_buffer_queue_guard(airwavz_phy_rx_data_buffer_queue_mutex);
            airwavz_phy_rx_data_buffer_condition.notify_one();
        }
        _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::stop: setting processThreadShouldRun: false");
        while(this->processThreadIsRunning) {
            usleep(100000);
            _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::stop: this->processThreadIsRunning: %d", this->processThreadIsRunning);
        }
        if(processThreadHandle.joinable()) {
            processThreadHandle.join();
        }
        _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::stop: after join for processThreadHandle");
    }

    _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::stop: before RedZoneCaptureStop, hRedZoneCapture_started: %d", hRedZoneCapture_started);
     if(hRedZoneCapture_started) {
        res = RedZoneCaptureStop(hRedZoneCapture);
         _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::stop: after RedZoneCaptureStop, res: %d", res);
     }

    hRedZoneCapture_started = false;

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_clear_callback();
    }

    _AIRWAVZ_PHY_ANDROID_INFO("AirwavzPHYAndroid::stop: return with this: %p", this);
    return res;
}

/*
 * jjustman-2020-08-23: NOTE - do NOT call delete AirwavzPHYAndroid* from anywhere else,
 *      only call deinit() otherwise you will get fortify crashes, ala:
 *  08-24 08:29:32.717 18991 18991 F libc    : FORTIFY: pthread_mutex_destroy called on a destroyed mutex (0x783b5c87b8)
 */

int AirwavzPHYAndroid::deinit()
{
    _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::deinit: enter with this: %p", this);
    this->stop();
    delete this;
    _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::deinit: return after delete this, with this: %p", this);

    return 0;
}

int AirwavzPHYAndroid::open(int fd, string device_path)
{
    int     ret = 0;
    int     drv_verbosity = 6;      // suggest a value of 2 (0 - 9 legal) to set debug output level;

    _AIRWAVZ_PHY_ANDROID_DEBUG("AirwavzPHYAndroid::open, this: %p,  with fd: %d, device_path: %s", this, fd, device_path.c_str());
    ret = RedZoneCaptureOpen(&hRedZoneCapture);
    if (ret) {
        _AIRWAVZ_PHY_ANDROID_ERROR("RedZoneCaptureOpen: Failed to initialize RedZoneCapture status = %d", ret);
        return -1;
    }

    hRedZoneCapture_open = true;

    ret = RedZoneCaptureSetProp(hRedZoneCapture, RedZoneLoggingVerboseMode, &drv_verbosity, sizeof(drv_verbosity));
    _AIRWAVZ_PHY_ANDROID_DEBUG("RedZoneCaptureSetProp: RedZoneLoggingVerboseMode returned %d\n", ret);

    ret = RedZoneCaptureInitSysDevice(hRedZoneCapture, fd );
    if (ret != RZR_SUCCESS)
    {
        _AIRWAVZ_PHY_ANDROID_ERROR("RedZoneCaptureInitSysDevice: Failed to initialize RedZoneCapture %d\nThis could be a result on a firmware download and re-ennumeration - try again\n", ret);
        return -2;
    }

    RedZoneOperatingMode opmode = OperatingModeATSC3;
    atsc3RedZoneParserCallbackData.device_mode = opmode;
    atsc3RedZoneParserCallbackData.airwavzPHYAndroid_instance = this;

    ret = RedZoneCaptureSetProp(hRedZoneCapture, RedZoneOperatingModeProp, &opmode, sizeof(opmode));
    _AIRWAVZ_PHY_ANDROID_DEBUG("RedZoneCaptureSetProp: RedZoneOperatingModePropMode returned %d\n", ret);

    ret = RZRBasebandParserOpen(&hBasebandParser);
    if(ret) {
        _AIRWAVZ_PHY_ANDROID_ERROR("Failed to open hBasebandParser, ret: %d", ret);
        return -4;
    }
    hBasebandParser_open = true;

    //jjustman-2020-09-23 - this method no longer exists?
    //RZRBasebandParserSetVerbosity(hBasebandParser, bbp_verbosity);

    ret = RZRBasebandParserRegisterCallbacks(hBasebandParser, BasebandParserALPCallbackGlobal, &atsc3RedZoneParserCallbackData);
    if(ret) {
        _AIRWAVZ_PHY_ANDROID_ERROR("Failed to register vendor packet parser callbacks, ret: %d", ret);
        return -6;
    }

    if (RedZoneCaptureRegisterCallbacks(hRedZoneCapture, RedZoneCaptureBasebandPacketCallbackGlobal, &atsc3RedZoneParserCallbackData))
    {
        _AIRWAVZ_PHY_ANDROID_ERROR("Failed to register callbacks with RedZoneMemoryConsumer");
        return -7;
    }

    ret = RedZoneCaptureStart(hRedZoneCapture);
    if (ret) {
        _AIRWAVZ_PHY_ANDROID_ERROR("Failed to start RedZoneCapture, ret: %d", ret);
        return -31337;
    }
    hRedZoneCapture_started = true;


    if(processThreadHandle.joinable()) {
        processThreadShouldRun = false;
        _AIRWAVZ_PHY_ANDROID_INFO("::Open() - setting processThreadShouldRun to false, Waiting for processThreadHandle to join()");
        processThreadHandle.join();
    }

    if(statusThreadHandle.joinable()) {
        statusThreadShouldRun = false;
        _AIRWAVZ_PHY_ANDROID_INFO("::Open() - setting statusThreadShouldRun to false, Waiting for statusThreadHandle to join()");
        statusThreadHandle.join();
    }

    processThreadShouldRun = true;
    processThreadHandle = std::thread([this](){
        this->processThread();
    });

    statusThreadShouldRun = true;
    statusThreadHandle = std::thread([this]() {
        this->statusThread();
    });

    _AIRWAVZ_PHY_ANDROID_DEBUG( "atsc3NdkClientAirwavzRZR:open() fd: %d, success", fd);

    return ret;
}

int AirwavzPHYAndroid::tune(int freqKHz, int plpId)
{
    int ret = 0;

    _AIRWAVZ_PHY_ANDROID_DEBUG("%s (%d KHz, plp %d)", __func__, freqKHz, plpId);

    if (plpId < 0 || plpId >= 64) {
        _AIRWAVZ_PHY_ANDROID_ERROR("AirwavzPHYAndroid::tune - invalid plp! value: %d", plpId);
        return -1;
    }

    //clear out our atsc3_core_service_player_bridge context
    atsc3_core_service_application_bridge_reset_context();

    ret = RedZoneCaptureSetProp(hRedZoneCapture, RedZoneFrequencyKHzProp, &freqKHz, sizeof(freqKHz));
    if(ret) {
        _AIRWAVZ_PHY_ANDROID_ERROR("AirwavzPHYAndroid::tune - RedZoneCaptureSetProp RedZoneFrequencyKHzProp returned: %d for freq: %d", ret, freqKHz);
        return ret;
    }
    _AIRWAVZ_PHY_ANDROID_DEBUG("tuned to freq %d MHz", freqKHz/1000);

    vector<uint8_t> myPlps;
    myPlps.push_back(plpId);

    ret = listen_plps(myPlps);

    if(ret) {
        _AIRWAVZ_PHY_ANDROID_ERROR("AirwavzPHYAndroid::tune - listen_plps returned: %d", ret);
        return ret;
    }

    is_tuned = true;

    _AIRWAVZ_PHY_ANDROID_DEBUG("tuned to freq %d MHz and listen plp %d", freqKHz/1000, plpId);

    return ret;
}

/*
 * jjustman-2020-08-24 -  always listen to plp0, so listen_plps may be a maximum of 3 values (4 if 0 is included)
 */
int AirwavzPHYAndroid::listen_plps(vector<uint8_t> plpIds)
{
    int ret = 0;

    RedZonePLPSet plpset = {255, 255, 255, 255};

    int nplp = plpIds.size();

    if (nplp > 4) {
        _AIRWAVZ_PHY_ANDROID_WARN("Requested %d PLPs - only the first 4 will be tuned\n", nplp);
        nplp = 4;
    }

    switch (nplp) {
        case 0:
            _AIRWAVZ_PHY_ANDROID_WARN("No PLPs specified\n");
            break;
        case 4:
            plpset.plp3_id = plpIds[3];
            RZRBasebandParserEnablePLP(hBasebandParser, plpset.plp3_id);
            /* fall through */
        case 3:
            plpset.plp2_id = plpIds[2];
            RZRBasebandParserEnablePLP(hBasebandParser, plpset.plp2_id);
            /* fall through */
        case 2:
            plpset.plp1_id = plpIds[1];
            RZRBasebandParserEnablePLP(hBasebandParser, plpset.plp1_id);
            /* fall through */
        case 1:
            plpset.plp0_id = plpIds[0];
            RZRBasebandParserEnablePLP(hBasebandParser, plpset.plp0_id);
            /* fall through */
            break;
    }

    if (nplp >0) {
        ret = RedZoneCaptureSetProp(hRedZoneCapture, RedZonePLPSelectionProp, &plpset, sizeof(plpset));
        if(ret) {
            _AIRWAVZ_PHY_ANDROID_ERROR("RedZonePLPSelectionProp plpSet returned: %d", ret);
            return ret;
        }

        _AIRWAVZ_PHY_ANDROID_DEBUG("Set PLP Selection returned %d\n", ret);
    }

    return ret;
}

void AirwavzPHYAndroid::RedZoneCaptureBasebandPacketCallbackGlobal(RedZoneCaptureBasebandPacket *pPacket, void *pUserData)
{
    atsc3RedZoneParserCallbackData_t *pParserCallbackData = (atsc3RedZoneParserCallbackData_t *)pUserData;
    AirwavzPHYAndroid* myInstance = (AirwavzPHYAndroid*) pParserCallbackData->airwavzPHYAndroid_instance;

    if (pParserCallbackData->device_mode == RedZoneOperatingModeATSC3)
    {
        RZRBasebandParserParsePacket(myInstance->getHBasebandParser(), pPacket);
    }
}

//jjustman-2020-09-23 - push to queue as block_t, processThread will dequeue and depacketize
// and handoff to libatsc3 alp parser here on callback invocation, and re-scope for (AirwavzPHYAndroid*) for single consumer context
void AirwavzPHYAndroid::BasebandParserALPCallbackGlobal(uint32_t plpId, const uint8_t *pPacket, int32_t sPacket, void *pUserData) {
    atsc3RedZoneParserCallbackData_t *pParserCallbackData = (atsc3RedZoneParserCallbackData_t *) pUserData;
    AirwavzPHYAndroid* myInstance = (AirwavzPHYAndroid *) pParserCallbackData->airwavzPHYAndroid_instance;

    //jjustman-2020-09-23 - push to queue
    myInstance->basebandParserALPCallbackPushQueue(plpId, pPacket, sPacket);
}


void AirwavzPHYAndroid::basebandParserALPCallbackPushQueue(uint8_t plpId, const uint8_t *pPacket, int32_t sPacket) {

    block_t* packet = block_Duplicate_from_ptr((uint8_t*)pPacket, sPacket);

    lock_guard<mutex> airwavz_phy_rx_data_buffer_queue_guard(airwavz_phy_rx_data_buffer_queue_mutex);
    airwavz_phy_rx_data_buffer_queue.push(make_pair(plpId, packet));
    airwavz_phy_rx_data_buffer_condition.notify_one();
}

int AirwavzPHYAndroid::processThread() {

    _AIRWAVZ_PHY_ANDROID_INFO("AirwavzPHYAndroid::ProcessThread: with this: %p", this);

    this->pinConsumerThreadAsNeeded();
    this->processThreadIsRunning = true;

    queue<pair<uint8_t, block_t*>> to_process_queue; //perform a shallow copy so we can exit critical section asap
    pair<uint8_t, block_t*> plpPacket;

    while (this->processThreadShouldRun) {
        //enter critical section with condition wait
        {
            unique_lock<mutex> condition_lock(airwavz_phy_rx_data_buffer_queue_mutex);
            airwavz_phy_rx_data_buffer_condition.wait(condition_lock);

            while (airwavz_phy_rx_data_buffer_queue.size()) {
                to_process_queue.push(airwavz_phy_rx_data_buffer_queue.front());
                airwavz_phy_rx_data_buffer_queue.pop();
            }
            condition_lock.unlock();
        }
        //exit critical section, now we can process our to_process_queue

        while (to_process_queue.size()) {
            plpPacket = to_process_queue.front();
            uint8_t plpId = plpPacket.first;
            block_t* alpPayload = plpPacket.second;

            to_process_queue.pop();

            block_Rewind(alpPayload);

            atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_parse(plpId, alpPayload);
            if(atsc3_alp_packet) {
                alp_completed_packets_parsed++;
                alp_total_bytes += atsc3_alp_packet->alp_payload->p_size;

                if(atsc3_alp_packet->alp_packet_header.packet_type == 0x00) {

                    block_Rewind(atsc3_alp_packet->alp_payload);
                    if(atsc3_phy_rx_udp_packet_process_callback) {
                        atsc3_phy_rx_udp_packet_process_callback(plpId, atsc3_alp_packet->alp_payload);
                    }

                } else if(atsc3_alp_packet->alp_packet_header.packet_type == 0x4) {
                    alp_total_LMTs_recv++;
                    atsc3_link_mapping_table_t *atsc3_link_mapping_table_pending = atsc3_alp_packet_extract_lmt(atsc3_alp_packet);

                    if (atsc3_phy_rx_link_mapping_table_process_callback && atsc3_link_mapping_table_pending) {
                        atsc3_link_mapping_table_t *atsc3_link_mapping_table_to_free = atsc3_phy_rx_link_mapping_table_process_callback(atsc3_link_mapping_table_pending);

                        if (atsc3_link_mapping_table_to_free) {
                            atsc3_link_mapping_table_free(&atsc3_link_mapping_table_to_free);
                        }
                    }
                }
                atsc3_alp_packet_free(&atsc3_alp_packet);
            }
            block_Destroy(&alpPayload);
        }
    }

    this->processThreadIsRunning = false;
    this->releasePinnedConsumerThreadAsNeeded();

    _AIRWAVZ_PHY_ANDROID_INFO("AirwavzPHYAndroid::processThread complete");

    return 0;
}


int AirwavzPHYAndroid::statusThread()
{
    int ret = 0;
    _AIRWAVZ_PHY_ANDROID_INFO("AirwavzPHYAndroid::statusThread started, this: %p", this);

    this->pinStatusThreadAsNeeded();
    this->statusThreadIsRunning = true;

    //jjustman-2020-09-23 - if we have a high bitrate (e.g. 20Mbps), frequent polling of tuning/demod status will cause stream glitches
    while(this->statusThreadShouldRun) {
        usleep(1000000);

        if(this->is_tuned) {
            if (atsc3_ndk_phy_bridge_get_instance()) {

                unsigned char lock;
                int SNR, RSSi;

                ret = RedZoneCaptureGetProp(hRedZoneCapture, RedZoneMasterLockProp, &lock, sizeof(lock));
                usleep(250000);

                ret = RedZoneCaptureGetProp(hRedZoneCapture, RedZoneSNRProp, &SNR, sizeof(SNR));
                usleep(250000);

                ret = RedZoneCaptureGetProp(hRedZoneCapture, RedZoneRSSIProp, &RSSi, sizeof(RSSi));
                usleep(250000);

                //RedZoneMasterLockProp
                uint8_t master_lock_prop;
                ret = RedZoneCaptureGetProp(hRedZoneCapture, RedZoneMasterLockProp, &master_lock_prop, sizeof(master_lock_prop));
                usleep(250000);

                //RedZonePLPSelectionProp
                RedZonePLPSet plpSet;
                ret = RedZoneCaptureGetProp(hRedZoneCapture, RedZonePLPSelectionProp, &plpSet, sizeof(plpSet));
                usleep(250000);

                //RedZoneL1BasicInfoProp
                ret = RedZoneCaptureGetProp(hRedZoneCapture, RedZoneL1BasicInfoProp, &RSSi, sizeof(RSSi));

                RedZonePLPSpecificInfo plpSpecificInfo;
                ret = RedZoneCaptureGetProp(hRedZoneCapture, RedZonePLPSpecificInfoProp, &plpSpecificInfo, sizeof(plpSpecificInfo));
                usleep(250000);

                RedZonePLPStatusInfo plpStatusInfo;
                ret = RedZoneCaptureGetProp(hRedZoneCapture, RedZonePLPStatusInfoProp, &plpStatusInfo, sizeof(plpStatusInfo));
                usleep(250000);

                //RedZoneLLSValidBitmaskProp
                uint64_t llsValid;
                ret = RedZoneCaptureGetProp(hRedZoneCapture, RedZoneLLSValidBitmaskProp, &llsValid, sizeof(llsValid));
                usleep(250000);

                atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_stats(
                        master_lock_prop,                  // tunerInfo.status == 1,
                        RSSi * -1,               // tunerInfo.signalStrength,
                        plpStatusInfo.Quality[0],                  // modcod_valid
                        0,                  // plp fec type
                        plpSpecificInfo.mod,                  // plp mod
                        plpSpecificInfo.cod,                  // plp cod
                        RSSi * 10,          // RFLevel1000
                        SNR * 10,           // SNR1000
                        plpStatusInfo.Pre_LDPC_BER[0],                  // ber pre ldpc
                        plpStatusInfo.Pre_BCH_BER[0],                  // ber pre bch
                        plpStatusInfo.Post_BCH_FER[0],             // ber pre bch
                        plpStatusInfo.Lock[0],               // demod lock
                        1,                  // cpu status
                        llsValid & 0x1,                  // plp lls?
                        llsValid & 0xFF);

                atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_bw_stats(alp_completed_packets_parsed, alp_total_bytes, alp_total_LMTs_recv);
            }
        }
    }

    this->statusThreadIsRunning = false;
    this->releasePinnedStatusThreadAsNeeded();

    return 0;
}


void AirwavzPHYAndroid::resetStatstics() {
    alp_completed_packets_parsed = 0;
    alp_total_bytes = 0;
    alp_total_LMTs_recv = 0;
}

void AirwavzPHYAndroid::NotifyPlpSelectionChangeCallback(vector<uint8_t> plps, void *context) {
    ((AirwavzPHYAndroid *) context)->listen_plps(plps);
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_init(JNIEnv *env, jobject instance) {
    lock_guard<mutex> airwavz_phy_android_cctor_mutex_local(AirwavzPHYAndroid::CS_global_mutex);

    _AIRWAVZ_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_init: start init, env: %p", env);
    if(airwavzPHYAndroid) {
        _AIRWAVZ_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_init: start init, AirwavzPHYAndroid is present: %p, calling deinit/delete", airwavzPHYAndroid);
        airwavzPHYAndroid->deinit();
        airwavzPHYAndroid = nullptr;
    }

    airwavzPHYAndroid = new AirwavzPHYAndroid(env, instance);
    airwavzPHYAndroid->init();

    _AIRWAVZ_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_init: return, instance: %p", airwavzPHYAndroid);
    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_run(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> airwavz_phy_android_cctor_mutex_local(AirwavzPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!airwavzPHYAndroid) {
        _AIRWAVZ_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_run: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        res = airwavzPHYAndroid->run();
        _AIRWAVZ_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_run: returning res: %d", res);
    }
    return res;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_is_1running(JNIEnv* env, jobject instance)
{
    lock_guard<mutex> airwavz_phy_android_cctor_mutex_local(AirwavzPHYAndroid::CS_global_mutex);
    jboolean res = false;

    if(!airwavzPHYAndroid) {
        _AIRWAVZ_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_is_1running: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = false;
    } else {
        res = airwavzPHYAndroid->is_running();

    }
    return res;
}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_stop(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> airwavz_phy_android_cctor_mutex_local(AirwavzPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!airwavzPHYAndroid) {
        _AIRWAVZ_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_stop: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        _AIRWAVZ_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_stop: enter with airwavzPHYAndroid: %p", airwavzPHYAndroid);

        res = airwavzPHYAndroid->stop();
        _AIRWAVZ_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_stop: returning res: %d", res);
    }

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_deinit(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> airwavz_phy_android_cctor_mutex_local(AirwavzPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!airwavzPHYAndroid) {
        _AIRWAVZ_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_deinit: error, airwavzPHYAndroid is NULL!");
        res = -1;
    } else {
        _AIRWAVZ_PHY_ANDROID_INFO("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_deinit: enter with airwavzPHYAndroid: %p", airwavzPHYAndroid);

        airwavzPHYAndroid->deinit();
        airwavzPHYAndroid = nullptr;
        _AIRWAVZ_PHY_ANDROID_INFO("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_deinit: exit with airwavzPHYAndroid: %p", airwavzPHYAndroid);
    }

    return res;
}
//
//extern "C" JNIEXPORT jint JNICALL
//Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_download_1bootloader_1firmware(JNIEnv *env, jobject thiz, jint fd, jstring device_path_jstring) {
//    lock_guard<mutex> airwavz_phy_android_cctor_mutex_local(AirwavzPHYAndroid::CS_global_mutex);
//
//    _AIRWAVZ_PHY_ANDROID_INFO("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_download_1bootloader_1firmware: fd: %d", fd);
//    int res = 0;
//
//    if(!airwavzPHYAndroid)  {
//        _AIRWAVZ_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_download_1bootloader_1firmware: AirwavzPHYAndroid is NULL!");
//        res = -1;
//    } else {
//        const char* device_path_weak = env->GetStringUTFChars(device_path_jstring, 0);
//        string device_path(device_path_weak);
//        res = airwavzPHYAndroid->download_bootloader_firmware(fd, device_path); //calls pre_init
//        env->ReleaseStringUTFChars( device_path_jstring, device_path_weak );
//
//        //jjustman-2020-08-23 - hack, clear out our in-flight reference since we should re-enumerate
//        delete airwavzPHYAndroid;
//        airwavzPHYAndroid = nullptr;
//    }
//
//    return res;
//}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_open(JNIEnv *env, jobject thiz, jint fd, jstring device_path_jstring) {
    lock_guard<mutex> airwavz_phy_android_cctor_mutex_local(AirwavzPHYAndroid::CS_global_mutex);

    _AIRWAVZ_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_open: fd: %d", fd);

    int res = 0;
    if(!airwavzPHYAndroid) {
        _AIRWAVZ_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_open: AirwavzPHYAndroid is NULL!");
        res = -1;
    } else {
        const char* device_path_weak = env->GetStringUTFChars(device_path_jstring, 0);
        string device_path(device_path_weak);
        res = airwavzPHYAndroid->open(fd, device_path);
        env->ReleaseStringUTFChars( device_path_jstring, device_path_weak );
        if(res) {
            _AIRWAVZ_PHY_ANDROID_WARN("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_open: returned: %d, calling deinit()!", res);
            airwavzPHYAndroid->deinit();
            airwavzPHYAndroid = nullptr;
        }
    }
    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_tune(JNIEnv *env, jobject thiz,
                                                                      jint freq_khz,
                                                                      jint single_plp) {
    lock_guard<mutex> airwavz_phy_android_cctor_mutex_local(AirwavzPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!airwavzPHYAndroid) {
        _AIRWAVZ_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_tune: AirwavzPHYAndroid is NULL!");
        res = -1;
    } else {
        res = airwavzPHYAndroid->tune(freq_khz, single_plp);
    }

    return res;
}
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_listen_1plps(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jobject plps) {
    lock_guard<mutex> airwavz_phy_android_cctor_mutex_local(AirwavzPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!airwavzPHYAndroid) {
        _AIRWAVZ_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_AirwavzPHYAndroid_listen_1plps: AirwavzPHYAndroid is NULL!");
        res = -1;
    } else {
        vector<uint8_t> listen_plps;

        jobject jIterator = env->CallObjectMethod(plps, env->GetMethodID(env->GetObjectClass(plps), "iterator", "()Ljava/util/Iterator;"));
        jmethodID nextMid = env->GetMethodID(env->GetObjectClass(jIterator), "next", "()Ljava/lang/Object;");
        jmethodID hasNextMid = env->GetMethodID(env->GetObjectClass(jIterator), "hasNext", "()Z");

        while (env->CallBooleanMethod(jIterator, hasNextMid)) {
            jobject jItem = env->CallObjectMethod(jIterator, nextMid);
            jbyte jByte = env->CallByteMethod(jItem, env->GetMethodID(env->GetObjectClass(jItem), "byteValue", "()B"));
            listen_plps.push_back(jByte);
        }

        res = airwavzPHYAndroid->listen_plps(listen_plps);
    }

    return res;
}