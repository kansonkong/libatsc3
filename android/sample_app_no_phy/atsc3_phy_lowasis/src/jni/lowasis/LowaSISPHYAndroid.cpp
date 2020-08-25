//
// Created by Jason Justman on 8/19/20.
//

#include "LowaSISPHYAndroid.h"
LowaSISPHYAndroid* lowaSISPHYAndroid = nullptr;

mutex LowaSISPHYAndroid::Cctor_muxtex;

LowaSISPHYAndroid::LowaSISPHYAndroid(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = this->env->NewGlobalRef(jni_instance);
    this->setRxUdpPacketProcessCallback(atsc3_core_service_bridge_process_packet_from_plp_and_block);
    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_set_callback(&LowaSISPHYAndroid::NotifyPlpSelectionChangeCallback, this);
    }

    _LOWASIS_PHY_ANDROID_INFO("LowaSISPHYAndroid::LowaSISPHYAndroid - created with this: %p", this);
}

LowaSISPHYAndroid::~LowaSISPHYAndroid() {

    _LOWASIS_PHY_ANDROID_INFO("LowaSISPHYAndroid::~LowaSISPHYAndroid - enter: deleting with this: %p", this);
    this->stop();

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_clear_callback();
    }

    if(this->producerJniEnv) {
        delete this->producerJniEnv;
    }

    if(this->consumerJniEnv) {
        delete this->producerJniEnv;
    }

    if(this->atsc3_sl_tlv_block) {
        block_Destroy(&this->atsc3_sl_tlv_block);
    }

    if(atsc3_sl_tlv_payload) {
        atsc3_sl_tlv_payload_free(&atsc3_sl_tlv_payload);
    }


    if(false) {
        /***
         *
         *  jjustman-2020-08-23 - TODO: fix this issue with deleting global ref?

        if (this->env && this->jni_instance_globalRef) {
            this->env->DeleteGlobalRef(this->jni_instance_globalRef);
            this->jni_instance_globalRef = nullptr;
        }
       */
    }

    _LOWASIS_PHY_ANDROID_INFO("LowaSISPHYAndroid::~LowaSISPHYAndroid - exit: deleting with this: %p", this);
}

void LowaSISPHYAndroid::pinProducerThreadAsNeeded() {
    producerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_lowasis_static_loader_get_javaVM(), "LowaSISPHYAndroid::producerThread");
}

void LowaSISPHYAndroid::releasePinnedProducerThreadAsNeeded() {
    if(producerJniEnv) {
        delete producerJniEnv;
        producerJniEnv = nullptr;
    }
}

void LowaSISPHYAndroid::pinConsumerThreadAsNeeded() {
    _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::pinConsumerThreadAsNeeded: mJavaVM: %p, atsc3_ndk_application_bridge instance: %p", atsc3_ndk_phy_lowasis_static_loader_get_javaVM(), atsc3_ndk_application_bridge_get_instance());

    consumerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_lowasis_static_loader_get_javaVM(), "LowaSISPHYAndroid::consumerThread");
    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->pinConsumerThreadAsNeeded();
    }
}

void LowaSISPHYAndroid::releasePinnedConsumerThreadAsNeeded() {
    if(consumerJniEnv) {
        delete consumerJniEnv;
        consumerJniEnv = nullptr;
    }

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->releasePinnedConsumerThreadAsNeeded();
    }
}

void LowaSISPHYAndroid::pinStatusThreadAsNeeded() {
    statusJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_lowasis_static_loader_get_javaVM(), "LowaSISPHYAndroid::statusThread");

    if(atsc3_ndk_phy_bridge_get_instance()) {
        atsc3_ndk_phy_bridge_get_instance()->pinStatusThreadAsNeeded();
    }
}

void LowaSISPHYAndroid::releasePinnedStatusThreadAsNeeded() {
    if(statusJniEnv) {
        delete statusJniEnv;
        statusJniEnv = nullptr;
    }

    if(atsc3_ndk_phy_bridge_get_instance()) {
        atsc3_ndk_phy_bridge_get_instance()->releasePinnedStatusThreadAsNeeded();
    }
}

//
//void LowaSISPHYAndroid::resetProcessThreadStatistics() {
//    alp_completed_packets_parsed = 0;
//    alp_total_bytes = 0;
//    alp_total_LMTs_recv = 0;
//}


int LowaSISPHYAndroid::init()
{
    return 0;
}

int LowaSISPHYAndroid::run()
{
    return 0;
}

bool LowaSISPHYAndroid::is_running() {

    return false; //(captureThreadIsRunning && processThreadIsRunning && statusThreadIsRunning);
}

int LowaSISPHYAndroid::stop()
{
    _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::stop: enter with this: %p", this);

//    //tear down status thread first, as its the most 'problematic'
//    if(statusThreadIsRunning) {
//        statusThreadShouldRun = false;
//        _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::stop: setting statusThreadShouldRun: false");
//        while(this->statusThreadIsRunning) {
//            SL_SleepMS(100);
//            _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::stop: this->statusThreadIsRunning: %d", this->statusThreadIsRunning);
//        }
//        pthread_join(sThreadID, NULL);
//        _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::stop: after pthread_join for sThreadID");
//    }
//
//    if(captureThreadIsRunning) {
//        captureThreadShouldRun = false;
//        _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::stop: setting captureThreadShouldRun: false");
//
//        SL_RxDataStop();
//        while(this->captureThreadIsRunning) {
//            SL_SleepMS(100);
//            _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::stop: this->captureThreadIsRunning: %d", this->captureThreadIsRunning);
//        }
//        pthread_join(cThreadID, NULL);
//        _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::stop: after pthread_join for cThreadID");
//    }
//
//    if(processThreadIsRunning) {
//        processThreadShouldRun = false;
//        _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::stop: setting processThreadShouldRun: false");
//        while(this->processThreadIsRunning) {
//            SL_SleepMS(100);
//            _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::stop: this->processThreadIsRunning: %d", this->processThreadIsRunning);
//        }
//        pthread_join(pThreadID, NULL);
//        _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::stop: after pthread_join for pThreadID");
//    }
//

    _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::stop: return with this: %p", this);
    return 0;
}

/*
 * jjustman-2020-08-23: NOTE - do NOT call delete lowaSISPHYAndroid* from anywhere else,
 *      only call deinit() otherwise you will get fortify crashes, ala:
 *  08-24 08:29:32.717 18991 18991 F libc    : FORTIFY: pthread_mutex_destroy called on a destroyed mutex (0x783b5c87b8)
 */

int LowaSISPHYAndroid::deinit()
{
    _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::deinit: enter with this: %p", this);

    this->stop();
    delete this;
    _LOWASIS_PHY_ANDROID_DEBUG("LowaSISPHYAndroid::deinit: return after delete this, with this: %p", this);

    return 0;
}

int LowaSISPHYAndroid::open(int fd, int bus, int addr)
{

    printf("OPEN COMPLETE!");
    return 0;

ERROR:

    return -1;
}

int LowaSISPHYAndroid::tune(int freqKHz, int plpid)
{
   int ret = 0;

//
//    if(!processThreadIsRunning) {
//        processThreadShouldRun = true;
//        pThread = pthread_create(&pThreadID, NULL, (THREADFUNCPTR)&LowaSISPHYAndroid::ProcessThread, (void*)this);
//        if (pThread != 0) {
//            //processFlag = 0;
//            printf("\n Process Thread failed to launch");
//            goto ERROR;
//        } else  {
//            //processFlag = 1;
//        }
//    }
//
//    if(!captureThreadIsRunning) {
//        captureThreadShouldRun = true;
//        printf("creating capture thread with cb buffer size: %d, tlv_block_size: %d",
//               CB_SIZE, BUFFER_SIZE);
//        cThread = pthread_create(&cThreadID, NULL, (THREADFUNCPTR)&LowaSISPHYAndroid::CaptureThread, (void*)this);
//        if (cThread != 0) {
//            printf("\n Capture Thread failed to launch");
//            goto ERROR;
//        }
//    }
//
//    if(!statusThreadIsRunning) {
//        statusThreadShouldRun = true;
//        sThread = pthread_create(&sThreadID, NULL, (THREADFUNCPTR) &LowaSISPHYAndroid::TunerStatusThread, (void*)this);
//        if (sThread != 0) {
//            printf("\n Capture Thread launched unsuccessfully");
//            goto ERROR;
//        }
//    }

    ret = 0;
    goto UNLOCK;

 ERROR:
    ret = -1;

    //unlock our i2c mutext
UNLOCK:
    return ret;

}

int LowaSISPHYAndroid::listen_plps(vector<uint8_t> plps_orignal_list)
{
    int ret = 0;

    vector<uint8_t> plps;
    for(int i=0; i < plps_orignal_list.size(); i++) {
        if(plps_orignal_list.at(i) == 0) {
            //skip, duplicate plp0 will cause demod to fail
        } else {
            plps.push_back(plps_orignal_list.at(i));
        }
    }


    return ret;
}

int LowaSISPHYAndroid::download_bootloader_firmware(int fd) {
//    SL_SetUsbFd(fd);
//
//    SL_I2cResult_t i2cres;
//
//    printf("SL_I2cPreInit - Before");
//    i2cres = SL_I2cPreInit();
//    printf("SL_I2cPreInit returned: %d", i2cres);
//
//    if (i2cres != SL_I2C_OK)
//    {
//        if(i2cres == SL_I2C_AWAITING_REENUMERATION) {
//            printf("\n INFO:SL_I2cPreInit SL_FX3S_I2C_AWAITING_REENUMERATION");
//            //sleep for 2s
//            sleep(2);
//            return 0;
//        } else {
//            printf("\n Error:SL_I2cPreInit failed: %d", i2cres);
//            printToConsoleI2cError(i2cres);
//        }
//    }
//    return -1;

    return -1;
}


void* LowaSISPHYAndroid::ProcessThread(void* context)
{
    printf("LowaSISPHYAndroid::ProcessThread: with context: %p", context);

    LowaSISPHYAndroid* apiImpl = (LowaSISPHYAndroid*) context;
//    apiImpl->processThreadIsRunning = true;
//
//    apiImpl->pinConsumerThreadAsNeeded();
//
//    apiImpl->resetProcessThreadStatistics();
//
//    while (apiImpl->processThreadShouldRun)
//    {
//        //printf("atsc3NdkClientSlImpl::ProcessThread: getDataSize is: %d", CircularBufferGetDataSize(cb));
//
//        while(CircularBufferGetDataSize(apiImpl->cb) >= BUFFER_SIZE) {
//            apiImpl->processTLVFromCallback();
//        }
//        usleep(10000);
//    }
//
//    apiImpl->releasePinnedConsumerThreadAsNeeded();
//
//    apiImpl->processThreadIsRunning = false;
    _LOWASIS_PHY_ANDROID_INFO("LowaSISPHYAndroid::ProcessThread complete");

    return 0;
}

void* LowaSISPHYAndroid::CaptureThread(void* context)
{
    LowaSISPHYAndroid* apiImpl = (LowaSISPHYAndroid*) context;
//    apiImpl->captureThreadIsRunning = true;
//
//    apiImpl->pinProducerThreadAsNeeded();
//
//    SL_RxDataStart((RxDataCB) & LowaSISPHYAndroid::RxDataCallback);
//
//    apiImpl->releasePinnedProducerThreadAsNeeded();
//
//    apiImpl->captureThreadIsRunning = false;

    _LOWASIS_PHY_ANDROID_INFO("LowaSISPHYAndroid::CaptureThread complete");

    return 0;
}

void* LowaSISPHYAndroid::TunerStatusThread(void* context)
{
    LowaSISPHYAndroid* apiImpl = (LowaSISPHYAndroid*) context;
//    apiImpl->statusThreadIsRunning = true;
//    apiImpl->pinStatusThreadAsNeeded();

//    while(apiImpl->statusThreadShouldRun) {

//
//        if(atsc3_ndk_phy_bridge_get_instance()) {
//
//            atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_stats(tunerInfo.status == 1,
//                tunerInfo.signalStrength,
//                LowaSISPHYAndroid->plpInfo.plp0 == l1dDiag.sfParams[0].PlpParams[0].L1dSfPlpId,
//                l1dDiag.sfParams[0].PlpParams[0].L1dSfPlpFecType,
//                l1dDiag.sfParams[0].PlpParams[0].L1dSfPlpModType,
//                l1dDiag.sfParams[0].PlpParams[0].L1dSfPlpCoderate,
//                tunerInfo.signalStrength/1000,
//                snr,
//                ber_l1b,
//                ber_l1d,
//                ber_plp0,
//                demodLockStatus,
//                cpuStatus == 0xFFFFFFFF,
//                llsPlpInfo & 0x01 == 0x01,
//                0);
//
//            atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_bw_stats(LowaSISPHYAndroid->alp_completed_packets_parsed,
//                                                                          LowaSISPHYAndroid->alp_total_bytes,
//                                                                          LowaSISPHYAndroid->alp_total_LMTs_recv);
//            }
//    }

//    apiImpl->releasePinnedStatusThreadAsNeeded();
//    apiImpl->statusThreadIsRunning = false;
    return 0;
}

//jjustman-2020-08-23 - TODO: wire up these callbacks in LowaSISPHYAndroid::cctor rather than direct
//coupling to atsc3_core_service_bridge
//
//void LowaSISPHYAndroid::processTLVFromCallback()
//{
//    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);
//    int bytesRead = CircularBufferPop(cb, BUFFER_SIZE, (char*)&processDataCircularBufferForCallback);
//    CircularBufferMutex_local.unlock();
//
//    unique_lock<mutex> atsc3_sl_tlv_block_mutex_local(atsc3_sl_tlv_block_mutex);
//
//    if(!atsc3_sl_tlv_block) {
//        _LOWASIS_PHY_ANDROID_WARN("ERROR: atsc3NdkClientSlImpl::processTLVFromCallback - atsc3_sl_tlv_block is NULL!");
//        allocate_atsc3_sl_tlv_block();
//    }
//
//    if(bytesRead) {
//
//        block_Write(atsc3_sl_tlv_block, (uint8_t*)&processDataCircularBufferForCallback, bytesRead);
//        block_Rewind(atsc3_sl_tlv_block);
//
//        bool atsc3_sl_tlv_payload_complete = false;
//
//        do {
//            atsc3_sl_tlv_payload = atsc3_sl_tlv_payload_parse_from_block_t(atsc3_sl_tlv_block);
//
//            if(atsc3_sl_tlv_payload) {
//                atsc3_sl_tlv_payload_dump(atsc3_sl_tlv_payload);
//                if(atsc3_sl_tlv_payload->alp_payload_complete) {
//                    atsc3_sl_tlv_payload_complete = true;
//
//                    block_Rewind(atsc3_sl_tlv_payload->alp_payload);
//                    atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_parse(atsc3_sl_tlv_payload->plp_number, atsc3_sl_tlv_payload->alp_payload);
//                    if(atsc3_alp_packet) {
//                        alp_completed_packets_parsed++;
//
//                        alp_total_bytes += atsc3_alp_packet->alp_payload->p_size;
//
//                        if(atsc3_alp_packet->alp_packet_header.packet_type == 0x00) {
//
//                            block_Rewind(atsc3_alp_packet->alp_payload);
//                            //atsc3_phy_mmt_player_bridge_process_packet_phy(atsc3_alp_packet->alp_payload);
//                            atsc3_core_service_bridge_process_packet_phy(atsc3_alp_packet->alp_payload);
//                        } else if(atsc3_alp_packet->alp_packet_header.packet_type == 0x4) {
//                            alp_total_LMTs_recv++;
//                            atsc3_link_mapping_table_t* atsc3_link_mapping_table_pending = atsc3_alp_packet_extract_lmt(atsc3_alp_packet);
//                            atsc3_phy_jni_bridge_notify_link_mapping_table(atsc3_link_mapping_table_pending);
//                        }
//
//                        atsc3_alp_packet_free(&atsc3_alp_packet);
//                    }
//                    //free our atsc3_sl_tlv_payload
//                    atsc3_sl_tlv_payload_free(&atsc3_sl_tlv_payload);
//
//                } else {
//                    atsc3_sl_tlv_payload_complete = false;
//                    //jjustman-2019-12-29 - noisy, TODO: wrap in __DEBUG macro check
//                    //printf("alp_payload->alp_payload_complete == FALSE at pos: %d, size: %d", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);
//                }
//            } else {
//                atsc3_sl_tlv_payload_complete = false;
//                //jjustman-2019-12-29 - noisy, TODO: wrap in __DEBUG macro check
//                //printf("ERROR: alp_payload IS NULL, short TLV read?  at atsc3_sl_tlv_block: i_pos: %d, p_size: %d", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);
//            }
//
//        } while(atsc3_sl_tlv_payload_complete);
//
//
//        if(atsc3_sl_tlv_payload && !atsc3_sl_tlv_payload->alp_payload_complete && atsc3_sl_tlv_block->i_pos > atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size) {
//            //accumulate from our last starting point and continue iterating during next bbp
//            atsc3_sl_tlv_block->i_pos -= atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size;
//            //free our atsc3_sl_tlv_payload
//            atsc3_sl_tlv_payload_free(&atsc3_sl_tlv_payload);
//        }
//
//        if(atsc3_sl_tlv_block->p_size > atsc3_sl_tlv_block->i_pos) {
//            //truncate our current block_t starting at i_pos, and then read next i/o block
//            block_t* temp_sl_tlv_block = block_Duplicate_from_position(atsc3_sl_tlv_block);
//            block_Destroy(&atsc3_sl_tlv_block);
//            atsc3_sl_tlv_block = temp_sl_tlv_block;
//            block_Seek(atsc3_sl_tlv_block, atsc3_sl_tlv_block->p_size);
//        } else if(atsc3_sl_tlv_block->p_size == atsc3_sl_tlv_block->i_pos) {
//            //clear out our tlv block as we are the "exact" size of our last alp packet
//
//            block_Destroy(&atsc3_sl_tlv_block);
//            atsc3_sl_tlv_block = block_Alloc(BUFFER_SIZE);
//        } else {
//            printf("atsc3_sl_tlv_block: positioning mismatch: i_pos: %d, p_size: %d - rewinding and seeking for magic packet?", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);
//
//            //jjustman: 2019-11-23: rewind in order to try seek for our magic packet in the next loop here
//            block_Rewind(atsc3_sl_tlv_block);
//        }
//    }
//
//    atsc3_sl_tlv_block_mutex_local.unlock();
//
//}
//
//void LowaSISPHYAndroid::RxDataCallback(unsigned char *data, long len)
//{
//    //printf("atsc3NdkClientSlImpl::RxDataCallback: pushing data: %p, len: %d", data, len);
//    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);
//
//    CircularBufferPush(LowaSISPHYAndroid::cb, (char *)data, len);
//    CircularBufferMutex_local.unlock();
//}
//
void LowaSISPHYAndroid::NotifyPlpSelectionChangeCallback(vector<uint8_t> plps, void *context) {
    ((LowaSISPHYAndroid *) context)->listen_plps(plps);
}
//
//void LowaSISPHYAndroid::allocate_atsc3_sl_tlv_block() {
//    unique_lock<mutex> atsc3_sl_tlv_block_mutex_local(atsc3_sl_tlv_block_mutex);
//    if(!atsc3_sl_tlv_block) {
//        atsc3_sl_tlv_block = block_Alloc(BUFFER_SIZE);
//    }
//    atsc3_sl_tlv_block_mutex_local.unlock();
//}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_init(JNIEnv *env, jobject instance) {
    unique_lock<mutex> saankhy_phy_android_cctor_mutex_local(LowaSISPHYAndroid::Cctor_muxtex);

    _LOWASIS_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_init: start init, env: %p", env);
    if(lowaSISPHYAndroid) {
        _LOWASIS_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_init: start init, LowaSISPHYAndroid is present: %p, calling deinit/delete", lowaSISPHYAndroid);
        lowaSISPHYAndroid->deinit();
        lowaSISPHYAndroid = nullptr;
    }

    lowaSISPHYAndroid = new LowaSISPHYAndroid(env, instance);
    lowaSISPHYAndroid->init();

    _LOWASIS_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_init: return, instance: %p", lowaSISPHYAndroid);
    saankhy_phy_android_cctor_mutex_local.unlock();
    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_run(JNIEnv *env, jobject thiz) {
    int res = 0;
    if(!lowaSISPHYAndroid) {
        _LOWASIS_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_run: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        res = lowaSISPHYAndroid->run();
        _LOWASIS_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_run: returning res: %d", res);
    }
    return res;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_is_1running(JNIEnv* env, jobject instance)
{
    jboolean res = false;

    if(!lowaSISPHYAndroid) {
        _LOWASIS_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_is_1running: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = false;
    } else {
        res = lowaSISPHYAndroid->is_running();
    }
    return res;
}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_stop(JNIEnv *env, jobject thiz) {
    int res = 0;
    if(!lowaSISPHYAndroid) {
        _LOWASIS_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_stop: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        res = lowaSISPHYAndroid->stop();
        _LOWASIS_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_stop: returning res: %d", res);
    }
    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_deinit(JNIEnv *env, jobject thiz) {
    unique_lock<mutex> saankhy_phy_android_cctor_mutex_local(LowaSISPHYAndroid::Cctor_muxtex);

    int res = 0;
    if(!lowaSISPHYAndroid) {
        _LOWASIS_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_deinit: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        lowaSISPHYAndroid->deinit();
        lowaSISPHYAndroid = nullptr;
    }

    saankhy_phy_android_cctor_mutex_local.unlock();
    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_download_1bootloader_1firmware(JNIEnv *env, jobject thiz, jint fd) {
    _LOWASIS_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_download_1bootloader_1firmware: fd: %d", fd);
    int res = 0;

    if(!lowaSISPHYAndroid)  {
        _LOWASIS_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_download_1bootloader_1firmware: LowaSISPHYAndroid is NULL!");
        res = -1;
    } else {
        res = lowaSISPHYAndroid->download_bootloader_firmware(fd); //calls pre_init
        //jjustman-2020-08-23 - hack, clear out our in-flight reference since we should re-enumerate
        delete lowaSISPHYAndroid;
        lowaSISPHYAndroid = nullptr;
    }
    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_open(JNIEnv *env, jobject thiz, jint fd) {
    _LOWASIS_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_open: fd: %d", fd);

    int res = 0;
    if(!lowaSISPHYAndroid) {
        _LOWASIS_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_open: LowaSISPHYAndroid is NULL!");
        res = -1;
    } else {
        res = lowaSISPHYAndroid->open(fd, 0, 0);
    }
    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_tune(JNIEnv *env, jobject thiz,
                                                                      jint freq_khz,
                                                                      jint single_plp) {
    int res = 0;
    if(!lowaSISPHYAndroid) {
        _LOWASIS_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_tune: LowaSISPHYAndroid is NULL!");
        res = -1;
    } else {
        res = lowaSISPHYAndroid->tune(freq_khz, single_plp);
    }
    return res;
}
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_listen_1plps(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jobject plps) {
    int res = 0;
    if(!lowaSISPHYAndroid) {
        _LOWASIS_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_LowaSISPHYAndroid_listen_1plps: LowaSISPHYAndroid is NULL!");
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

        res = lowaSISPHYAndroid->listen_plps(listen_plps);
    }
    return res;
}