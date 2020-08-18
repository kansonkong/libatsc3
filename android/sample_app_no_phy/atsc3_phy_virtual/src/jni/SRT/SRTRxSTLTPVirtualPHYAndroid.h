//
// Created by Jason Justman on 8/18/20.
//

#include <string.h>
#include <jni.h>
#include <atsc3_core_service_player_bridge.h>
#include <Atsc3LoggingUtils.h>
#include <Atsc3JniEnv.h>
#include <atsc3_utils.h>
#include <SRTRxSTLTPVirtualPHY.h>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SRTRXSTLTPVIRTUALPHYANDROID_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SRTRXSTLTPVIRTUALPHYANDROID_H

#include <Atsc3NdkPHYVirtualStaticJniLoader.h>

class SRTRxSTLTPVirtualPHYAndroid : public SRTRxSTLTPVirtualPHY {

public:
    SRTRxSTLTPVirtualPHYAndroid(JNIEnv* env, jobject jni_instance);

    ~SRTRxSTLTPVirtualPHYAndroid();

protected:
    void pinProducerThreadAsNeeded() override;
    void releaseProducerThreadAsNeeded() override;
    Atsc3JniEnv* producerJniEnv = nullptr;

    void pinConsumerThreadAsNeeded() override;
    void releaseConsumerThreadAsNeeded() override;
    Atsc3JniEnv* consumerJniEnv = nullptr;

    JNIEnv* env = nullptr;
    jobject jni_instance_globalRef = nullptr;

};



#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SRTRXSTLTPVIRTUALPHYANDROID_H
