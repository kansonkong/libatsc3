//
// Created by Jason Justman on 2019-11-23.
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>


using namespace std;


#ifndef LIBATSC3_ATSC3NDKCLIENTNOPHYIMPL_H
#define LIBATSC3_ATSC3NDKCLIENTNOPHYIMPL_H

#include <atsc3_utils.h>

#include <atsc3_sl_tlv_demod_type.h>
#include <atsc3_alp_parser.h>

#include "atsc3NdkClient.h"

typedef void * (*THREADFUNCPTR)(void *);

class atsc3NdkClientNoPhyImpl {
    public:
        void Init(atsc3NdkClient* ref_);

        int Open(int fd, int bus, int addr);
        int Tune(int freqKhz, int plpId);
        int TuneMultiplePLP(int freqKhz, vector<int> plpIds);
        int GetTunerStatus();

        void processTLVFromCallback();

        //thread state management flags, TODO: remove from static to instance binding
        static bool        captureThreadShouldRun;
        static bool        processThreadShouldRun;
        static bool        tunerStatusThreadShouldRun;
        static bool        tunerStatusThreadShouldPollTunerStatus;

        //tuner/sl unit global values, TODO: move to instance values
        static int         global_tUnit;  //tUnint, move to instance
        static int         global_slUnit; //jjustman-2019-12-18 remove me

    static void RxDataCallback(unsigned char *data, long len);

    //data metrics
    uint64_t alp_completed_packets_parsed;
    uint64_t alp_total_bytes;
    uint64_t alp_total_LMTs_recv;


private:

    static atsc3NdkClient* atsc3NdkClient_ref;   //reference to our base JNI NDI handler

    //thread handler references for Capture, Processing and Tuner Status threads

    pthread_t   cThreadID = 0;
    pthread_t   pThreadID = 0;
    pthread_t   tsThreadID = 0;

    //thread handling methods
    static void* CaptureThread(void*);
    static void* ProcessThread(void*);
    static void* TunerStatusThread(void*);

    //libusb instance device values
    int fd;
    int bus;
    int addr;
    //libusb_device_handle*  runningDeviceHandleOpened = NULL;

    block_t* atsc3_sl_tlv_block = NULL;
    atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload = NULL;


    void resetProcessThreadStatistics();
};

#endif //LIBATSC3_ATSC3NDKCLIENTNOPHYIMPL_H
