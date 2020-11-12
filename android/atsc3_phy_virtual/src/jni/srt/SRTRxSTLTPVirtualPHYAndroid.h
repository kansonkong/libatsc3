//
// Created by Jason Justman on 8/18/20.
//

#include <string.h>
#include <jni.h>
#include <thread>
#include <mutex>
#include <semaphore.h>
using namespace std;

#include <Atsc3LoggingUtils.h>
#include <Atsc3JniEnv.h>
#include <atsc3_utils.h>
#include <SRTRxSTLTPVirtualPHY.h>

#include <atsc3_core_service_player_bridge.h>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SRTRXSTLTPVIRTUALPHYANDROID_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SRTRXSTLTPVIRTUALPHYANDROID_H

#include <Atsc3NdkPHYVirtualStaticJniLoader.h>

class SRTRxSTLTPVirtualPHYAndroid : public SRTRxSTLTPVirtualPHY {

public:
    static mutex CS_global_mutex;

    SRTRxSTLTPVirtualPHYAndroid(JNIEnv* env, jobject jni_instance);

    ~SRTRxSTLTPVirtualPHYAndroid();

protected:
    JNIEnv* env = nullptr;
    jobject jni_instance_globalRef = nullptr;

    void pinProducerThreadAsNeeded() override;
    void releasePinnedProducerThreadAsNeeded() override;
    Atsc3JniEnv* producerJniEnv = nullptr;

    void pinConsumerThreadAsNeeded() override;
    void releasePinnedConsumerThreadAsNeeded() override;
    Atsc3JniEnv* consumerJniEnv = nullptr;
};


#define _SRTRXSTLTP_VIRTUAL_PHY_ANDROID_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _SRTRXSTLTP_VIRTUAL_PHY_ANDROID_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _SRTRXSTLTP_VIRTUAL_PHY_ANDROID_INFO(...)    	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _SRTRXSTLTP_VIRTUAL_PHY_ANDROID_DEBUG(...)      __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define _SRTRXSTLTP_VIRTUAL_PHY_ANDROID_TRACE(...)      __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);

#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SRTRXSTLTPVIRTUALPHYANDROID_H
