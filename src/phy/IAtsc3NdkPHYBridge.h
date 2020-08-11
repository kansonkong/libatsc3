/*
 * IAtsc3NdkApplicationBridge.h
 *
 *  Created on: Aug 10, 2020
 *      Author: jjustman
 */
#include <string>
#include "IAtsc3NdkPHYClient.h"

using namespace std;

#ifndef LIBATSC3_IATSC3NDKPHYBRIDGE_H
#define LIBATSC3_IATSC3NDKPHYBRIDGE_H

class IAtsc3NdkPHYBridge {
public:

    /* jjustman-2020-08-10: TODO - for saankhya slapi:
     *     int atsc3_rx_callback_f(void*, uint64_t ullUser);
     *
     *
    virtual void ListenPLP1(uint8_t PLP1) = 0;
    virtual void ListenPLPs(uint8_t PLP0, uint8_t PLP1, uint8_t PLP2, uint8_t PLP3) = 0;

*/

    virtual void LogMsg(const char *msg) = 0;
    virtual void LogMsg(const std::string &msg) = 0;
    virtual void LogMsgF(const char *fmt, ...) = 0;

    //moving to "friend" scope
    virtual void atsc3_update_rf_stats(int32_t tuner_lock,    //1
                                  int32_t rssi,
                                  uint8_t modcod_valid,
                                  uint8_t plp_fec_type,
                                  uint8_t plp_mod,
                                  uint8_t plp_cod,
                                  int32_t nRfLevel1000,
                                  int32_t nSnr1000,
                                  uint32_t ber_pre_ldpc_e7,
                                  uint32_t ber_pre_bch_e9,
                                  uint32_t fer_post_bch_e6,
                                  uint8_t demod_lock,
                                  uint8_t signal,
                                  uint8_t plp_any,
                                  uint8_t plp_all) =0; //15

    virtual void atsc3_update_rf_bw_stats(uint64_t total_pkts, uint64_t total_bytes, unsigned int total_lmts) = 0;

    //application callbacks - mapped to native method for Android
    virtual void setRfPhyStatisticsViewVisible(bool isRfPhyStatisticsVisible) = 0;
//
//protected:
//    IAtsc3NdkPHYClient* iAtsc3NDKPhyClient = nullptr; //jjustman-2020-08-10 - TODO: handle this for mapping plp management without having to traverse the JNI boundary

};

#endif //LIBATSC3_IATSC3NDKPHYBRIDGE_H
