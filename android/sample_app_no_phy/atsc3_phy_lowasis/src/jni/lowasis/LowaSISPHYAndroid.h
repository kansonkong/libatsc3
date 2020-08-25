//
// Created by Jason Justman on 8/19/20.
//

#include <string.h>
#include <jni.h>
#include <Atsc3LoggingUtils.h>
#include <Atsc3JniEnv.h>
#include <atsc3_utils.h>
#include <atsc3_sl_tlv_demod_type.h>
#include <atsc3_alp_parser.h>

#include <mutex>
#include <semaphore.h>
#include <pthread.h>

#include <atsc3_core_service_player_bridge.h>

#ifndef LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_LOWASISPHYANDROID_H
#define LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_LOWASISPHYANDROID_H

#include <Atsc3NdkPHYLowaSISStaticJniLoader.h>

typedef void * (*THREADFUNCPTR)(void *);

class LowaSISPHYAndroid : public IAtsc3NdkPHYClient {

public:
    static mutex Cctor_muxtex;

    LowaSISPHYAndroid(JNIEnv* env, jobject jni_instance);

    virtual int  init()       override;
    virtual int  run()        override;
    virtual bool is_running() override;
    virtual int  stop()       override;
    virtual int  deinit()     override;

    virtual int  download_bootloader_firmware(int fd) override;
    virtual int  open(int fd, int bus, int addr)   override;
    virtual int  tune(int freqKhz, int single_plp) override;
    virtual int  listen_plps(vector<uint8_t> plps) override;

    virtual ~LowaSISPHYAndroid();

    static void RxDataCallback(unsigned char *data, long len);

    static int         usbFD;

    int RxThread();

    static void NotifyPlpSelectionChangeCallback(vector<uint8_t> plps, void* context);

    //jjustman-2020-08-23 - moving to public for now..
    uint64_t alp_completed_packets_parsed;
    uint64_t alp_total_bytes;
    uint64_t alp_total_LMTs_recv;

    void dump_plp_list();

protected:
    void pinProducerThreadAsNeeded() override;
    void releasePinnedProducerThreadAsNeeded() override;
    Atsc3JniEnv* producerJniEnv = nullptr;

    void pinConsumerThreadAsNeeded() override;
    void releasePinnedConsumerThreadAsNeeded() override;
    Atsc3JniEnv* consumerJniEnv = nullptr;

    void pinStatusThreadAsNeeded() override;
    void releasePinnedStatusThreadAsNeeded() override;
    Atsc3JniEnv* statusJniEnv = nullptr;

    JNIEnv* env = nullptr;
    jobject jni_instance_globalRef = nullptr;

private:

    //thread handling methods
    static void* CaptureThread(void* context);
    static void* ProcessThread(void* context);
    static void* TunerStatusThread(void* context); //TODO: jjustman-2019-11-30: merge with

    block_t* atsc3_sl_tlv_block = NULL;
    mutex    atsc3_sl_tlv_block_mutex;
    void allocate_atsc3_sl_tlv_block();

    atsc3_sl_tlv_payload_t* atsc3_sl_tlv_payload = NULL;


};

#define _LOWASIS_PHY_ANDROID_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _LOWASIS_PHY_ANDROID_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _LOWASIS_PHY_ANDROID_INFO(...)    	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _LOWASIS_PHY_ANDROID_DEBUG(...)     __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define _LOWASIS_PHY_ANDROID_TRACE(...)     __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);


#endif //LIBATSC3_ANDROID_SAMPLE_APP_W_PHY_LOWASISPHYANDROID_H
