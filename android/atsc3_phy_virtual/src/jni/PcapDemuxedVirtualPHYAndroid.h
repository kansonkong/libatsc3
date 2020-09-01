//
// Created by Jason Justman on 8/18/20.
//

#include <string.h>
#include <jni.h>
#include <Atsc3LoggingUtils.h>
#include <Atsc3JniEnv.h>
#include <atsc3_utils.h>
#include <atsc3_core_service_player_bridge.h>
#include <PcapDemuxedVirtualPHY.h>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SRTRXSTLTPVIRTUALPHYANDROID_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SRTRXSTLTPVIRTUALPHYANDROID_H

#include <Atsc3NdkPHYVirtualStaticJniLoader.h>

class PcapDemuxedVirtualPHYAndroid : public PcapDemuxedVirtualPHY {

public:
    PcapDemuxedVirtualPHYAndroid(JNIEnv* env, jobject jni_instance);

    ~PcapDemuxedVirtualPHYAndroid();

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

#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_SRTRXSTLTPVIRTUALPHYANDROID_H
