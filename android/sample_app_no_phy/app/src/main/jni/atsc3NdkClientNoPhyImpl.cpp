//
// Created by Jason Justman on 2019-11-23.
//

#include <atsc3_utils.h>
#include <atsc3_phy_mmt_player_bridge.h>
#include "atsc3NdkClient.h"

#include "atsc3NdkClientNoPhyImpl.h"

extern atsc3NdkClientNoPhyImpl apiImpl;

bool atsc3NdkClientNoPhyImpl::captureThreadShouldRun = false;
bool atsc3NdkClientNoPhyImpl::processThreadShouldRun = false;
bool atsc3NdkClientNoPhyImpl::tunerStatusThreadShouldRun = false;
bool atsc3NdkClientNoPhyImpl::tunerStatusThreadShouldPollTunerStatus = false;

atsc3NdkClient* atsc3NdkClientNoPhyImpl::atsc3NdkClient_ref = NULL;

//NDK JNI main dispatcher reference
void atsc3NdkClientNoPhyImpl::Init(atsc3NdkClient* ref_) {
    atsc3NdkClientNoPhyImpl::atsc3NdkClient_ref = ref_;

}

int atsc3NdkClientNoPhyImpl::Open(int fd_, int bus_, int addr_) {
    fd = fd_;
    bus = bus_;
    addr = addr_;

    return 0;
}

int atsc3NdkClientNoPhyImpl::Tune(int freqKhz, int plpId) {

    vector<int> myPlps;
    myPlps.push_back(plpId);

    return TuneMultiplePLP(freqKhz, myPlps);
}

/**
 * jjustman-2019-12-17: TODO - refactor this to InitTunerAndDemodTransfer
 * @param freqKhz
 * @param plpIds
 * @return
 */
int atsc3NdkClientNoPhyImpl::TuneMultiplePLP(int freqKhz, vector<int> plpIds) {

    int cThread_ret = 0;
    int pThread_ret = 0;
    int sThread_ret = 0;


    //1. config plp's
    //2. set tuner frequency
    //3. start processing threads


    //start our Capture thread which will invoke libusb_submit_transfer and libusb_handle_events
    if (!cThreadID) {

//        if (!atsc3_sl_tlv_block) {
//            atsc3_sl_tlv_block = block_Alloc(BUFFER_SIZE);
//        }
//
//        printf("creating capture thread, cb buffer size: %d, tlv_block_size: %d",
//               CB_SIZE, BUFFER_SIZE);
        atsc3NdkClientNoPhyImpl::captureThreadShouldRun = true;


        cThread_ret = pthread_create(&cThreadID, NULL, (THREADFUNCPTR) &atsc3NdkClientNoPhyImpl::CaptureThread, NULL);
        printf("created CaptureThread, cThreadID is: %d", cThreadID);
        if (cThread_ret != 0) {
            atsc3NdkClientNoPhyImpl::captureThreadShouldRun = false;
            printf("Capture Thread launched unsuccessfully, cThread_ret: %d", cThread_ret);
            return 0;
        }
    } else {
        printf("using existing CaptureThread");

    }


    //create our ProcessThread - handles enqueued TLV payload from CaptureThread
    if (!pThreadID) {

        atsc3NdkClientNoPhyImpl::processThreadShouldRun = true;

        pThread_ret = pthread_create(&pThreadID, NULL, (THREADFUNCPTR) &atsc3NdkClientNoPhyImpl::ProcessThread, NULL);
        if (pThread_ret != 0) {
            atsc3NdkClientNoPhyImpl::processThreadShouldRun = 0;
            printf("Process Thread launched unsuccessfully, ret: %d", pThread_ret);
            return -1;
        }
    } else {
        printf("using existing ProcessThread, pThread is: %d", pThreadID);
    }

    if (!tsThreadID) {

        atsc3NdkClientNoPhyImpl::tunerStatusThreadShouldRun = true;

        sThread_ret = pthread_create(&tsThreadID, NULL, (THREADFUNCPTR) &atsc3NdkClientNoPhyImpl::TunerStatusThread, NULL);
        if (sThread_ret != 0) {
            atsc3NdkClientNoPhyImpl::tunerStatusThreadShouldRun = false;
            printf("Status Thread launched unsuccessfully, ret: %d", sThread_ret);
            return 0;
        }
    } else {
        printf("using existing TunerStatusThread, sThread is: %d", tsThreadID);
    }

    atsc3NdkClientNoPhyImpl::atsc3NdkClient_ref->LogMsgF("TuneMultiplePLP: completed: capture: %ul, process: %ul, tunerStatus: %ul", cThreadID, pThreadID, tsThreadID);

    return 0;
}



void atsc3NdkClientNoPhyImpl::processTLVFromCallback()
{
//    int bytesRead = CircularBufferPop(atsc3NdkClientNoPhyImpl::cb, BUFFER_SIZE, (char*)&processDataCircularBufferForCallback);
//    if(!atsc3_sl_tlv_block) {
//        printf("ERROR: atsc3NdkClientSlImpl::processTLVFromCallback - atsc3_sl_tlv_block is NULL!");
//        return;
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
//                    atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_parse(atsc3_sl_tlv_payload->alp_payload);
//                    if(atsc3_alp_packet) {
//                        alp_completed_packets_parsed++;
//
//                        alp_total_bytes += atsc3_alp_packet->alp_payload->p_size;
//
//                        if(atsc3_alp_packet->alp_packet_header.packet_type == 0x00) {
//
//                            block_Rewind(atsc3_alp_packet->alp_payload);
//                            atsc3_phy_mmt_player_bridge_process_packet_phy(atsc3_alp_packet->alp_payload);
//
//                        } else if(atsc3_alp_packet->alp_packet_header.packet_type == 0x4) {
//                            alp_total_LMTs_recv++;
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
}


void atsc3NdkClientNoPhyImpl::RxDataCallback(unsigned char *data, long len)
{
    //printf("atsc3NdkClientNoPhyImpl::RxDataCallback: pushing data: %p, len: %d", data, len);
    //CircularBufferPush(atsc3NdkClientSlImpl::cb, (char *)data, len);
}

void* atsc3NdkClientNoPhyImpl::CaptureThread(void*)
{
    atsc3NdkClientNoPhyImpl::atsc3NdkClient_ref->pinFromRxCaptureThread();

    //...((RxDataCB)&atsc3NdkClientNoPhyImpl::RxDataCallback);
    return 0;
}


//Process Thread Impl
void* atsc3NdkClientNoPhyImpl::ProcessThread(void*)
{
    apiImpl.resetProcessThreadStatistics();

    atsc3NdkClientNoPhyImpl::atsc3NdkClient_ref->pinFromRxProcessingThread();

    while (atsc3NdkClientNoPhyImpl::processThreadShouldRun)
    {
        //printf("atsc3NdkClientSlImpl::ProcessThread: getDataSize is: %d", CircularBufferGetDataSize(cb));

//        if (CircularBufferGetDataSize(cb) >= BUFFER_SIZE) {
//            apiImpl.processTLVFromCallback();
//        } else {
//            usleep(10000);
//        }
    }

    return 0;
}

void* atsc3NdkClientNoPhyImpl::TunerStatusThread(void*)
{

    atsc3NdkClientNoPhyImpl::atsc3NdkClient_ref->pinFromRxStatusThread();


    while(atsc3NdkClientNoPhyImpl::tunerStatusThreadShouldRun) {
//
//        //only actively poll the tuner status if the RF status window is visible
//        if(!atsc3NdkClientNoPhyImpl::tunerStatusThreadShouldPollTunerStatus) {
//            usleep(1000000);
//            continue;
//        }
//
//
//
//        /*jjustman-2020-01-06: For the SL3000/SiTune, we will have 3 status attributes with the following possible values:
//
//                tunerInfo.status:   SL_TUNER_STATUS_NOT_LOCKED (0)
//                                    SL_TUNER_STATUS_LOCKED (1)
//
//                demodLockStatus:    SL_DEMOD_STATUS_NOT_LOCK (0)
//                                    SL_DEMOD_STATUS_LOCK (1)
//
//                cpuStatus:          (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED",
//         */
//
//
//
//        snr = (float)perfDiag.GlobalSnrLinearScale / 16384;
//        snr = 10000.0 * log10(snr); //10
//
//        ber_l1b = (float)perfDiag.NumBitErrL1b / perfDiag.NumFecBitsL1b; // //aBerPreLdpcE7,
//        ber_l1d = (float)perfDiag.NumBitErrL1d / perfDiag.NumFecBitsL1d;//aBerPreBchE9,
//        ber_plp0 = (float)perfDiag.NumBitErrPlp0 / perfDiag.NumFecBitsPlp0; //aFerPostBchE6,
//
//        printf("atsc3NdkClientNoPhyImpl::TunerStatusThread: tunerInfo.status: %d, tunerInfo.signalStrength: %f, cpuStatus: %s, demodLockStatus: %d, globalSnr: %f",
//                tunerInfo.status,
//                tunerInfo.signalStrength,
//                (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED",
//                demodLockStatus,
//                perfDiag.GlobalSnrLinearScale);
//
//
//        atsc3NdkClientSLRef->atsc3_update_rf_stats(tunerInfo.status == 1,
//                tunerInfo.signalStrength,
//                apiImpl.plpInfo.plp0 == l1dDiag.sf0ParamsStr.Plp0paramsstr.L1dSfPlpId,
//                l1dDiag.sf0ParamsStr.Plp0paramsstr.L1dSfPlpFecType,
//                l1dDiag.sf0ParamsStr.Plp0paramsstr.L1dSfPlpModType,
//                l1dDiag.sf0ParamsStr.Plp0paramsstr.L1dSfPlpCoderate,
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
//        atsc3NdkClientNoPhyImpl->atsc3_update_rf_bw_stats(apiImpl.alp_completed_packets_parsed, apiImpl.alp_total_bytes, apiImpl.alp_total_LMTs_recv);

    }
    return 0;
}


void atsc3NdkClientNoPhyImpl::resetProcessThreadStatistics() {
    alp_completed_packets_parsed = 0;
    alp_total_bytes = 0;
    alp_total_LMTs_recv = 0;
}
