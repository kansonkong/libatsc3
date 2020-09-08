//
// Created by Jason Justman on 8/19/20.
//

#include "SaankhyaPHYAndroid.h"
SaankhyaPHYAndroid* saankhyaPHYAndroid = nullptr;

CircularBuffer SaankhyaPHYAndroid::cb = nullptr;
mutex SaankhyaPHYAndroid::CircularBufferMutex;

mutex SaankhyaPHYAndroid::CS_global_mutex;


SaankhyaPHYAndroid::SaankhyaPHYAndroid(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = this->env->NewGlobalRef(jni_instance);
    this->setRxUdpPacketProcessCallback(atsc3_core_service_bridge_process_packet_from_plp_and_block);
    this->setRxLinkMappingTableProcessCallback(atsc3_phy_jni_bridge_notify_link_mapping_table);

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_set_callback(&SaankhyaPHYAndroid::NotifyPlpSelectionChangeCallback, this);
    }

    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::SaankhyaPHYAndroid - created with this: %p", this);
}

SaankhyaPHYAndroid::~SaankhyaPHYAndroid() {

    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::~SaankhyaPHYAndroid - enter: deleting with this: %p", this);
    this->stop();

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_clear_callback();
    }

    //jjustman-2020-08-24 - do not attempt to delete producer/consumer/statusJniEnvironment here, as you will
    //most likely get a JNI threadlocal exception

    if(this->atsc3_sl_tlv_block) {
        block_Destroy(&this->atsc3_sl_tlv_block);
    }

    if(atsc3_sl_tlv_payload) {
        atsc3_sl_tlv_payload_free(&atsc3_sl_tlv_payload);
    }

    if(cb) {
        CircularBufferFree(cb);
    }

    if(false) {
        /***
         *
         *  jjustman-2020-08-23 - TODO: fix this issue with deleting global ref?

            08-24 07:30:12.812 12165 12165 F DEBUG   : *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
            08-24 07:30:12.812 12165 12165 F DEBUG   : Build fingerprint: 'samsung/beyond2qlteue/beyond2q:10/QP1A.190711.020/G975U1UES4DTG1:user/release-keys'
            08-24 07:30:12.812 12165 12165 F DEBUG   : Revision: '17'
            08-24 07:30:12.812 12165 12165 F DEBUG   : ABI: 'arm64'
            08-24 07:30:12.812  1320  1399 D PkgPredictorService: pkg:org.ngbp.libatsc3 activity:org.ngbp.libatsc3.sampleapp.MainActivity thisTime:-1
            08-24 07:30:12.812 12165 12165 F DEBUG   : Timestamp: 2020-08-24 07:30:12+0900
            08-24 07:30:12.812 12165 12165 F DEBUG   : pid: 12018, tid: 12159, name: Thread-9  >>> org.ngbp.libatsc3 <<<
            08-24 07:30:12.812 12165 12165 F DEBUG   : uid: 10292
            08-24 07:30:12.812 12165 12165 F DEBUG   : signal 6 (SIGABRT), code -1 (SI_QUEUE), fault addr --------
            08-24 07:30:12.812 12165 12165 F DEBUG   : Abort message: 'JNI DETECTED ERROR IN APPLICATION: thread Thread[6,tid=12159,Native,Thread*=0x7933322800,peer=0x12f2b0e0,"Thread-9"] using JNIEnv* from thread Thread[1,tid=12018,Runnable,Thread*=0x7933326000,peer=0x72d72ef0,"main"]
            08-24 07:30:12.812 12165 12165 F DEBUG   :     in call to DeleteGlobalRef
            08-24 07:30:12.812 12165 12165 F DEBUG   :     from int org.ngbp.libatsc3.middleware.android.phy.SaankhyaPHYAndroid.init()'
            08-24 07:30:12.812 12165 12165 F DEBUG   :     x0  0000000000000000  x1  0000000000002f7f  x2  0000000000000006  x3  000000789c2feec0
            08-24 07:30:12.812 12165 12165 F DEBUG   :     x4  fefeff783099cf97  x5  fefeff783099cf97  x6  fefeff783099cf97  x7  7f7f7f7f7fffffff
            08-24 07:30:12.812 12165 12165 F DEBUG   :     x8  00000000000000f0  x9  dbc96eeb4ea8c79c  x10 0000000000000001  x11 0000000000000000
            08-24 07:30:12.812 12165 12165 F DEBUG   :     x12 fffffff0fffffbdf  x13 ffffffffffffffff  x14 0000000000000000  x15 ffffffffffffffff
            08-24 07:30:12.812 12165 12165 F DEBUG   :     x16 0000007930c1d8c0  x17 0000007930bf9fe0  x18 000000783a0be000  x19 0000000000002ef2
            08-24 07:30:12.812 12165 12165 F DEBUG   :     x20 0000000000002f7f  x21 00000000ffffffff  x22 000000783db74600  x23 00000078aceadcc5
            08-24 07:30:12.812 12165 12165 F DEBUG   :     x24 00000078acecf8ce  x25 0000000000000001  x26 0000007932efc258  x27 00000079333a6150
            08-24 07:30:12.812 12165 12165 F DEBUG   :     x28 00000078ad3d9338  x29 000000789c2fef60
            08-24 07:30:12.812 12165 12165 F DEBUG   :     sp  000000789c2feea0  lr  0000007930bab27c  pc  0000007930bab2a8
            08-24 07:30:12.891 12165 12165 F DEBUG   : backtrace:
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #00 pc 00000000000832a8  /apex/com.android.runtime/lib64/bionic/libc.so (abort+160) (BuildId: b0750023d0cf44584c064da02400c159)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #01 pc 00000000004b99bc  /apex/com.android.runtime/lib64/libart.so (art::Runtime::Abort(char const*)+2388) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #02 pc 000000000000b458  /system/lib64/libbase.so (android::base::LogMessage::~LogMessage()+580) (BuildId: 36cd125456a5320dd3dcb8cfbd889a1a)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #03 pc 0000000000377fa0  /apex/com.android.runtime/lib64/libart.so (art::JavaVMExt::JniAbort(char const*, char const*)+1584) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #04 pc 00000000003781c4  /apex/com.android.runtime/lib64/libart.so (art::JavaVMExt::JniAbortV(char const*, char const*, std::__va_list)+108) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #05 pc 000000000036a5ec  /apex/com.android.runtime/lib64/libart.so (art::(anonymous namespace)::ScopedCheck::AbortF(char const*, ...)+136) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #06 pc 0000000000368de8  /apex/com.android.runtime/lib64/libart.so (art::(anonymous namespace)::ScopedCheck::CheckPossibleHeapValue(art::ScopedObjectAccess&, char, art::(anonymous namespace)::JniValueType)+416) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #07 pc 00000000003684a8  /apex/com.android.runtime/lib64/libart.so (art::(anonymous namespace)::ScopedCheck::Check(art::ScopedObjectAccess&, bool, char const*, art::(anonymous namespace)::JniValueType*)+652) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #08 pc 000000000036b28c  /apex/com.android.runtime/lib64/libart.so (art::(anonymous namespace)::CheckJNI::DeleteRef(char const*, _JNIEnv*, _jobject*, art::IndirectRefKind)+672) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #09 pc 0000000000017290  /data/app/org.ngbp.libatsc3-nohwIryClhnu1vbwH0TMFg==/base.apk!libatsc3_phy_saankhya.so (offset 0x16d000) (_JNIEnv::DeleteGlobalRef(_jobject*)+40) (BuildId: c718f4141b1baee2331289b1564d0d0db23ad6b7)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #10 pc 0000000000017118  /data/app/org.ngbp.libatsc3-nohwIryClhnu1vbwH0TMFg==/base.apk!libatsc3_phy_saankhya.so (offset 0x16d000) (SaankhyaPHYAndroid::~SaankhyaPHYAndroid()+1128) (BuildId: c718f4141b1baee2331289b1564d0d0db23ad6b7)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #11 pc 000000000001730c  /data/app/org.ngbp.libatsc3-nohwIryClhnu1vbwH0TMFg==/base.apk!libatsc3_phy_saankhya.so (offset 0x16d000) (SaankhyaPHYAndroid::~SaankhyaPHYAndroid()+36) (BuildId: c718f4141b1baee2331289b1564d0d0db23ad6b7)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #12 pc 0000000000017c34  /data/app/org.ngbp.libatsc3-nohwIryClhnu1vbwH0TMFg==/base.apk!libatsc3_phy_saankhya.so (offset 0x16d000) (SaankhyaPHYAndroid::deinit()+84) (BuildId: c718f4141b1baee2331289b1564d0d0db23ad6b7)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #13 pc 000000000001cc64  /data/app/org.ngbp.libatsc3-nohwIryClhnu1vbwH0TMFg==/base.apk!libatsc3_phy_saankhya.so (offset 0x16d000) (Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_init+1152) (BuildId: c718f4141b1baee2331289b1564d0d0db23ad6b7)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #14 pc 0000000000140350  /apex/com.android.runtime/lib64/libart.so (art_quick_generic_jni_trampoline+144) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #15 pc 0000000000137334  /apex/com.android.runtime/lib64/libart.so (art_quick_invoke_stub+548) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #16 pc 0000000000145fec  /apex/com.android.runtime/lib64/libart.so (art::ArtMethod::Invoke(art::Thread*, unsigned int*, unsigned int, art::JValue*, char const*)+244) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #17 pc 00000000002e3624  /apex/com.android.runtime/lib64/libart.so (art::interpreter::ArtInterpreterToCompiledCodeBridge(art::Thread*, art::ArtMethod*, art::ShadowFrame*, unsigned short, art::JValue*)+384) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #18 pc 00000000002de884  /apex/com.android.runtime/lib64/libart.so (bool art::interpreter::DoCall<false, false>(art::ArtMethod*, art::Thread*, art::ShadowFrame&, art::Instruction const*, unsigned short, art::JValue*)+892) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #19 pc 00000000005a14a8  /apex/com.android.runtime/lib64/libart.so (MterpInvokeVirtual+648) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #20 pc 0000000000131814  /apex/com.android.runtime/lib64/libart.so (mterp_op_invoke_virtual+20) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #21 pc 000000000001fb54  [anon:dalvik-classes2.dex extracted in memory from /data/app/org.ngbp.libatsc3-nohwIryClhnu1vbwH0TMFg==/base.apk!classes2.dex] (org.ngbp.libatsc3.sampleapp.MainActivity.usbPHYLayerDeviceTryToInstantiateFromRegisteredPHYNDKs+208)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #22 pc 00000000005a1768  /apex/com.android.runtime/lib64/libart.so (MterpInvokeVirtual+1352) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #23 pc 0000000000131814  /apex/com.android.runtime/lib64/libart.so (mterp_op_invoke_virtual+20) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #24 pc 0000000000021a24  [anon:dalvik-classes2.dex extracted in memory from /data/app/org.ngbp.libatsc3-nohwIryClhnu1vbwH0TMFg==/base.apk!classes2.dex] (org.ngbp.libatsc3.sampleapp.MainActivity.usbPHYLayerDeviceInstantiateAndUpdateAtsc3NdkPHYClientInstance+16)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #25 pc 00000000005a3a74  /apex/com.android.runtime/lib64/libart.so (MterpInvokeDirect+1100) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #26 pc 0000000000131914  /apex/com.android.runtime/lib64/libart.so (mterp_op_invoke_direct+20) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #27 pc 000000000001fd9c  [anon:dalvik-classes2.dex extracted in memory from /data/app/org.ngbp.libatsc3-nohwIryClhnu1vbwH0TMFg==/base.apk!classes2.dex] (org.ngbp.libatsc3.sampleapp.MainActivity.access$2000)
            08-24 07:30:12.891 12165 12165 F DEBUG   :       #28 pc 00000000005a4218  /apex/com.android.runtime/lib64/libart.so (MterpInvokeStatic+1040) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #29 pc 0000000000131994  /apex/com.android.runtime/lib64/libart.so (mterp_op_invoke_static+20) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #30 pc 000000000001def0  [anon:dalvik-classes2.dex extracted in memory from /data/app/org.ngbp.libatsc3-nohwIryClhnu1vbwH0TMFg==/base.apk!classes2.dex] (org.ngbp.libatsc3.sampleapp.MainActivity$13$1.run+48)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #31 pc 00000000005a2f88  /apex/com.android.runtime/lib64/libart.so (MterpInvokeInterface+1788) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #32 pc 0000000000131a14  /apex/com.android.runtime/lib64/libart.so (mterp_op_invoke_interface+20) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #33 pc 00000000000eaa54  /apex/com.android.runtime/javalib/core-oj.jar (java.lang.Thread.run+8)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #34 pc 00000000002b4938  /apex/com.android.runtime/lib64/libart.so (_ZN3art11interpreterL7ExecuteEPNS_6ThreadERKNS_20CodeItemDataAccessorERNS_11ShadowFrameENS_6JValueEbb.llvm.3584781260104004149+240) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #35 pc 0000000000592a10  /apex/com.android.runtime/lib64/libart.so (artQuickToInterpreterBridge+1032) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #36 pc 0000000000140468  /apex/com.android.runtime/lib64/libart.so (art_quick_to_interpreter_bridge+88) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #37 pc 0000000000137334  /apex/com.android.runtime/lib64/libart.so (art_quick_invoke_stub+548) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #38 pc 0000000000145fec  /apex/com.android.runtime/lib64/libart.so (art::ArtMethod::Invoke(art::Thread*, unsigned int*, unsigned int, art::JValue*, char const*)+244) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #39 pc 00000000004b1144  /apex/com.android.runtime/lib64/libart.so (art::(anonymous namespace)::InvokeWithArgArray(art::ScopedObjectAccessAlreadyRunnable const&, art::ArtMethod*, art::(anonymous namespace)::ArgArray*, art::JValue*, char const*)+104) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #40 pc 00000000004b2258  /apex/com.android.runtime/lib64/libart.so (art::InvokeVirtualOrInterfaceWithJValues(art::ScopedObjectAccessAlreadyRunnable const&, _jobject*, _jmethodID*, jvalue const*)+416) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #41 pc 00000000004f31c0  /apex/com.android.runtime/lib64/libart.so (art::Thread::CreateCallback(void*)+1176) (BuildId: 9f61584f79f2db8d8a1869001bfb944e)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #42 pc 00000000000e6f10  /apex/com.android.runtime/lib64/bionic/libc.so (__pthread_start(void*)+36) (BuildId: b0750023d0cf44584c064da02400c159)
            08-24 07:30:12.892 12165 12165 F DEBUG   :       #43 pc 00000000000850c8  /apex/com.android.runtime/lib64/bionic/libc.so (__start_thread+64) (BuildId: b0750023d0cf44584c064da02400c159)


        if (this->env && this->jni_instance_globalRef) {
            this->env->DeleteGlobalRef(this->jni_instance_globalRef);
            this->jni_instance_globalRef = nullptr;
        }
       */
    }

    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::~SaankhyaPHYAndroid - exit: deleting with this: %p", this);
}

void SaankhyaPHYAndroid::pinProducerThreadAsNeeded() {
    producerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_saankhya_static_loader_get_javaVM(), "SaankhyaPHYAndroid::producerThread");
}

void SaankhyaPHYAndroid::releasePinnedProducerThreadAsNeeded() {
    if(producerJniEnv) {
        delete producerJniEnv;
        producerJniEnv = nullptr;
    }
}

void SaankhyaPHYAndroid::pinConsumerThreadAsNeeded() {
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::pinConsumerThreadAsNeeded: mJavaVM: %p, atsc3_ndk_application_bridge instance: %p", atsc3_ndk_phy_saankhya_static_loader_get_javaVM(), atsc3_ndk_application_bridge_get_instance());

    consumerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_saankhya_static_loader_get_javaVM(), "SaankhyaPHYAndroid::consumerThread");
    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->pinConsumerThreadAsNeeded();
    }
}

void SaankhyaPHYAndroid::releasePinnedConsumerThreadAsNeeded() {
    if(consumerJniEnv) {
        delete consumerJniEnv;
        consumerJniEnv = nullptr;
    }

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->releasePinnedConsumerThreadAsNeeded();
    }
}

void SaankhyaPHYAndroid::pinStatusThreadAsNeeded() {
    statusJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_saankhya_static_loader_get_javaVM(), "SaankhyaPHYAndroid::statusThread");

    if(atsc3_ndk_phy_bridge_get_instance()) {
        atsc3_ndk_phy_bridge_get_instance()->pinStatusThreadAsNeeded();
    }
}

void SaankhyaPHYAndroid::releasePinnedStatusThreadAsNeeded() {
    if(statusJniEnv) {
        delete statusJniEnv;
        statusJniEnv = nullptr;
    }

    if(atsc3_ndk_phy_bridge_get_instance()) {
        atsc3_ndk_phy_bridge_get_instance()->releasePinnedStatusThreadAsNeeded();
    }
}


void SaankhyaPHYAndroid::resetProcessThreadStatistics() {
    alp_completed_packets_parsed = 0;
    alp_total_bytes = 0;
    alp_total_LMTs_recv = 0;
}


int SaankhyaPHYAndroid::init()
{
    SaankhyaPHYAndroid::configPlatformParams();
    return 0;
}

int SaankhyaPHYAndroid::run()
{
    return 0;
}

bool SaankhyaPHYAndroid::is_running() {

    return (captureThreadIsRunning && processThreadIsRunning && statusThreadIsRunning);
}

int SaankhyaPHYAndroid::stop()
{
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: enter with this: %p", this);
    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_clear_callback();
    }
    //tear down status thread first, as its the most 'problematic'
    if(statusThreadIsRunning) {
        statusThreadShouldRun = false;
        _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: setting statusThreadShouldRun: false");
        while(this->statusThreadIsRunning) {
            SL_SleepMS(100);
            _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: this->statusThreadIsRunning: %d", this->statusThreadIsRunning);
        }
        pthread_join(sThreadID, NULL);
        _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: after pthread_join for sThreadID");
    }

    if(captureThreadIsRunning) {
        captureThreadShouldRun = false;
        _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: setting captureThreadShouldRun: false");

        SL_RxDataStop();
        while(this->captureThreadIsRunning) {
            SL_SleepMS(100);
            _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: this->captureThreadIsRunning: %d", this->captureThreadIsRunning);
        }
        pthread_join(cThreadID, NULL);
        _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: after pthread_join for cThreadID");
    }

    if(processThreadIsRunning) {
        processThreadShouldRun = false;
        _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: setting processThreadShouldRun: false");
        while(this->processThreadIsRunning) {
            SL_SleepMS(100);
            _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: this->processThreadIsRunning: %d", this->processThreadIsRunning);
        }
        pthread_join(pThreadID, NULL);
        _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: after pthread_join for pThreadID");
    }


    SL_I2cUnInit();
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: return with this: %p", this);
    return 0;
}

/*
 * jjustman-2020-08-23: NOTE - do NOT call delete slApi*, only call deinit() otherwise you will get fortify crashes, ala:
 *  08-24 08:29:32.717 18991 18991 F libc    : FORTIFY: pthread_mutex_destroy called on a destroyed mutex (0x783b5c87b8)
 */

int SaankhyaPHYAndroid::deinit()
{
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::deinit: enter with this: %p", this);

    this->stop();
    delete this;
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::deinit: return after delete this, with this: %p", this);

    return 0;
}

int SaankhyaPHYAndroid::open(int fd, string device_path)
{
    SL_SetUsbFd(fd);

    _SAANKHYA_PHY_ANDROID_DEBUG("open with fd: %d, device_path: %s", fd, device_path.c_str());

    SL_I2cResult_t i2cres;

    SL_Result_t slres;
    SL_ConfigResult_t cres;
    SL_TunerResult_t tres;
    SL_UtilsResult_t utilsres;
    SL_TunerConfig_t tunerCfg;
    SL_TunerConfig_t tunerGetCfg;
    SL_TunerSignalInfo_t tunerInfo;
    int swMajorNo, swMinorNo;
    unsigned int cFrequency = 0;
    SL_AfeIfConfigParams_t afeInfo;
    SL_OutIfConfigParams_t outPutInfo;
    SL_IQOffsetCorrectionParams_t iqOffSetCorrection;
    SL_DemodBootStatus_t bootStatus;

    cres = SL_ConfigGetPlatform(&getPlfConfig);
    if (cres == SL_CONFIG_OK)
    {
        printToConsolePlfConfiguration(getPlfConfig);
    }
    else
    {
        SL_Printf("\n ERROR : SL_ConfigGetPlatform Failed ");
        goto ERROR;
    }

    cres = SL_ConfigSetBbCapture(BB_CAPTURE_DISABLE);
    if (cres != SL_CONFIG_OK)
    {
        SL_Printf("\n ERROR : SL_ConfigSetBbCapture Failed ");
        goto ERROR;
    }

    if (getPlfConfig.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
    {
        i2cres = SL_I2cInit();
        if (i2cres != SL_I2C_OK)
        {
            _SAANKHYA_PHY_ANDROID_ERROR("ERROR : Error:SL_I2cInit failed Failed");

            SL_Printf("\n Error:SL_I2cInit failed :");
            printToConsoleI2cError(i2cres);
            goto ERROR;
        }
        else
        {
            cmdIf = SL_CMD_CONTROL_IF_I2C;
            printf("atsc3NdkClientSlImpl: setting cmdIf: %d", cmdIf);
        }
    }
    else if (getPlfConfig.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SDIO)
    {
        SL_Printf("\n Error:SL_SdioInit failed :Not Supported");
        goto ERROR;
    }
    else if (getPlfConfig.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SPI)
    {
        SL_Printf("\n Error:SL_SpiInit failed :Not Supported");
        goto ERROR;
    }

    /* Demod Config */
    switch (getPlfConfig.boardType)
    {
        case SL_EVB_3000:
            if (getPlfConfig.tunerType == TUNER_NXP)
            {
                afeInfo.spectrum = SL_SPECTRUM_INVERTED;
                afeInfo.iftype = SL_IFTYPE_LIF;
                afeInfo.ifreq = 4.4 + IF_OFFSET;
            }
            else if (getPlfConfig.tunerType == TUNER_SI)
            {
                afeInfo.spectrum = SL_SPECTRUM_NORMAL;
                afeInfo.iftype = SL_IFTYPE_ZIF;
                afeInfo.ifreq = 0.0;
            }
            else
            {
                SL_Printf("\n Invalid Tuner Selection");
            }

            if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
            {
                outPutInfo.oif = SL_OUTPUTIF_TSSERIAL_LSB_FIRST;
                /* CPLD Reset */
                SL_GpioSetPin(getPlfConfig.cpldResetGpioPin, 0x00);          // Low
                SL_SleepMS(100); // 100ms delay for Toggle
                SL_GpioSetPin(getPlfConfig.cpldResetGpioPin, 0x01);          // High
            }
            else if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
            {
                outPutInfo.oif = SL_OUTPUTIF_SDIO;
            }
            else
            {
                SL_Printf("\n Invalid OutPut Interface Selection");
            }

            afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
            afeInfo.qswap = SL_QPOL_SWAP_DISABLE;
            iqOffSetCorrection.iCoeff1 = 1.0;
            iqOffSetCorrection.qCoeff1 = 1.0;
            iqOffSetCorrection.iCoeff2 = 0.0;
            iqOffSetCorrection.qCoeff2 = 0.0;
            break;

        case SL_EVB_3010:
            if (getPlfConfig.tunerType == TUNER_NXP)
            {
                afeInfo.spectrum = SL_SPECTRUM_INVERTED;
                afeInfo.iftype = SL_IFTYPE_LIF;
                afeInfo.ifreq = 4.4 + IF_OFFSET;
            }
            else if (getPlfConfig.tunerType == TUNER_SI)
            {
                printf("using TUNER_SI, ifreq: 0");
                afeInfo.spectrum = SL_SPECTRUM_NORMAL;
                afeInfo.iftype = SL_IFTYPE_ZIF;
                afeInfo.ifreq = 0.0;
            }
            else
            {
                SL_Printf("\n Invalid Tuner Selection");
            }

            if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
            {
                outPutInfo.oif = SL_OUTPUTIF_TSPARALLEL_LSB_FIRST;
            }
            else if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
            {
                outPutInfo.oif = SL_OUTPUTIF_SDIO;
            }
            else
            {
                SL_Printf("\n Invalid Output Interface Selection");
            }

            afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
            afeInfo.qswap = SL_QPOL_SWAP_DISABLE;
            iqOffSetCorrection.iCoeff1 = 1.0;
            iqOffSetCorrection.qCoeff1 = 1.0;
            iqOffSetCorrection.iCoeff2 = 0.0;
            iqOffSetCorrection.qCoeff2 = 0.0;
            break;

        case SL_EVB_4000:
            if (getPlfConfig.tunerType == TUNER_SI)
            {
                afeInfo.spectrum = SL_SPECTRUM_NORMAL;
                afeInfo.iftype = SL_IFTYPE_ZIF;
                afeInfo.ifreq = 0.0;
            }
            else
            {
                SL_Printf("\n Invalid Tuner Selection");
            }

            if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
            {
                outPutInfo.oif = SL_OUTPUTIF_TSSERIAL_LSB_FIRST;
                /* CPLD Reset */
                SL_GpioSetPin(getPlfConfig.cpldResetGpioPin, 0x00); // Low
                SL_SleepMS(100); // 100ms delay for Toggle
                SL_GpioSetPin(getPlfConfig.cpldResetGpioPin, 0x01); // High
            }
            else if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
            {
                outPutInfo.oif = SL_OUTPUTIF_SDIO;
            }
            else
            {
                SL_Printf("\n Invalid Output Interface Selection");
            }

            afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
            afeInfo.qswap = SL_QPOL_SWAP_ENABLE;
            iqOffSetCorrection.iCoeff1 = 1;
            iqOffSetCorrection.qCoeff1 = 1;
            iqOffSetCorrection.iCoeff2 = 0;
            iqOffSetCorrection.qCoeff2 = 0;

            break;

        case SL_KAILASH_DONGLE:
            if (getPlfConfig.tunerType == TUNER_SI)
            {
                printf("using SL_KAILASH with SPECTRUM_NORMAL and ZIF");
                afeInfo.spectrum = SL_SPECTRUM_NORMAL;
                afeInfo.iftype = SL_IFTYPE_ZIF;
                afeInfo.ifreq = 0.0;
            }
            else
            {
                SL_Printf("\n Invalid Tuner Type selected ");
            }

            if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
            {
                outPutInfo.oif = SL_OUTPUTIF_TSPARALLEL_LSB_FIRST;
            }
            else
            {
                SL_Printf("\n Invalid OutPut Interface Selection");
            }

            afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
            afeInfo.qswap = SL_QPOL_SWAP_ENABLE;
            iqOffSetCorrection.iCoeff1 = (float)1.00724023045574;
            iqOffSetCorrection.qCoeff1 = (float)0.998403791546105;
            iqOffSetCorrection.iCoeff2 = (float)0.0432678874719328;
            iqOffSetCorrection.qCoeff2 = (float)0.0436508327768608;
            break;

        default:
            SL_Printf("\n Invalid Board Type Selected ");
            break;
    }
    afeInfo.iqswap = SL_IQSWAP_DISABLE;
    afeInfo.agcRefValue = 125; //afcRefValue in mV
    outPutInfo.TsoClockInvEnable = SL_TSO_CLK_INV_ON;

    cres = SL_ConfigGetBbCapture(&getbbValue);
    if (cres != SL_CONFIG_OK)
    {
        _SAANKHYA_PHY_ANDROID_ERROR("ERROR : SL_ConfigGetPlatform Failed");

        SL_Printf("\n ERROR : SL_ConfigGetPlatform Failed ");
        goto ERROR;
    }

    if (getbbValue)
    {
        plpInfo.plp0 = 0x00;
    }
    else
    {
        plpInfo.plp0 = 0x00;
    }
    plpInfo.plp1 = 0xFF;
    plpInfo.plp2 = 0xFF;
    plpInfo.plp3 = 0xFF;

    printf("SL_DemodCreateInstance: before");
    slres = SL_DemodCreateInstance(&slUnit);
    if (slres != SL_OK)
    {
        printf("\n Error:SL_DemodCreateInstance: slres: %d", slres);
        SL_Printf("\n Error:SL_DemodCreateInstance :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    //jjustman-2020-07-20 - create thread for libusb_handle_events for context callbacks
    //jjustman-2020-07-29 - disable
    //pthread_create(&pThreadID, NULL, (THREADFUNCPTR)&atsc3NdkClientSlImpl::LibUSB_Handle_Events_Callback, (void*)this);

    SL_Printf("\n Initializing SL Demod..: ");
    printf("SL_DemodInit: before, slUnit: %d, cmdIf: %d", slUnit, cmdIf);
    slres = SL_DemodInit(slUnit, cmdIf, SL_DEMODSTD_ATSC3_0);
    if (slres != SL_OK)
    {
        printf("SL_DemodInit: failed, slres: %d", slres);
        SL_Printf("FAILED");
        SL_Printf("\n Error:SL_DemodInit :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }
    else
    {
        printf("SL_DemodInit: SUCCESS, slres: %d", slres);
        SL_Printf("SUCCESS");
    }

    do
    {
        printf("before SL_DemodGetStatus: slres is: %d", slres);
        slres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_BOOT, (SL_DemodBootStatus_t*)&bootStatus);
        printf("SL_DemodGetStatus: slres is: %d", slres);
        if (slres != SL_OK)
        {
            SL_Printf("\n Error:SL_Demod Get Boot Status :");
            printToConsoleDemodError(slres);
        }
        SL_SleepMS(1000);
    } while (bootStatus != SL_DEMOD_BOOT_STATUS_COMPLETE);

    SL_Printf("\n Demod Boot Status      : ");
    printf("\n Demod Boot Status      : ");
    if (bootStatus == SL_DEMOD_BOOT_STATUS_INPROGRESS)
    {
        SL_Printf("%s", "INPROGRESS");
        printf("inprogress");
    }
    else if (bootStatus == SL_DEMOD_BOOT_STATUS_COMPLETE)
    {
        SL_Printf("%s", "COMPLETED");
        printf("COMPLETED");
    }
    else if (bootStatus == SL_DEMOD_BOOT_STATUS_ERROR)
    {
        SL_Printf("%s", "ERROR");
        printf("ERROR");
        goto ERROR;
    }

    slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_AFEIF, &afeInfo);
    if (slres != 0)
    {
        SL_Printf("\n Error:SL_DemodConfigure :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    slres = SL_DemodConfigure(slUnit, SL_CONFIG_TYPE_IQ_OFFSET_CORRECTION, &iqOffSetCorrection);
    if (slres != 0)
    {
        SL_Printf("\n Error:SL_DemodConfigure :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_OUTPUTIF, &outPutInfo);
    if (slres != 0)
    {
        SL_Printf("\n Error:SL_DemodConfigure :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    slres = SL_DemodConfigPlps(slUnit, &plpInfo);
    if (slres != 0)
    {
        SL_Printf("\n Error:SL_DemodConfigPlps :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    slres = SL_DemodGetSoftwareVersion(slUnit, &swMajorNo, &swMinorNo);
    if (slres == SL_OK)
    {
        SL_Printf("\n Demod SW Version       : %d.%d", swMajorNo, swMinorNo);
        printf("\n Demod SW Version       : %d.%d", swMajorNo, swMinorNo);
    }

    /* Tuner Config */
    tunerCfg.bandwidth = SL_TUNER_BW_6MHZ;
    tunerCfg.std = SL_TUNERSTD_ATSC3_0;

    tres = SL_TunerCreateInstance(&tUnit);
    if (tres != 0)
    {
        SL_Printf("\n Error:SL_TunerCreateInstance :");
        printToConsoleTunerError(tres);
        goto ERROR;
    }

    tres = SL_TunerInit(tUnit);
    if (tres != 0)
    {
        SL_Printf("\n Error:SL_TunerInit :");
        printToConsoleTunerError(tres);
        goto ERROR;
    }

    tres = SL_TunerConfigure(tUnit, &tunerCfg);
    if (tres != 0)
    {
        SL_Printf("\n Error:SL_TunerConfigure :");
        printToConsoleTunerError(tres);
        goto ERROR;
    }

    if (getPlfConfig.boardType == SL_EVB_4000)
    {
        /*
         * Apply tuner IQ offset. Relevant to SITUNE Tuner
         */
        tunerIQDcOffSet.iOffSet = 15;
        tunerIQDcOffSet.qOffSet = 14;

        tres = SL_TunerExSetDcOffSet(tUnit, &tunerIQDcOffSet);
        if (tres != 0)
        {
            SL_Printf("\n Error:SL_TunerExSetDcOffSet :");
            printToConsoleTunerError(tres);
            if (getPlfConfig.tunerType == TUNER_SI)
            {
                goto ERROR;
            }
        }
    }

    printf("OPEN COMPLETE!");
    return 0;

ERROR:

    return -1;
}

int SaankhyaPHYAndroid::tune(int freqKHz, int plpid)
{
    unsigned int cFrequency = 0;
    int ret = 0;


    //acquire our lock for setting tuning parameters (including re-tuning)
    unique_lock<mutex> SL_I2C_command_mutex_tuner_tune(SL_I2C_command_mutex);
    atsc3_core_service_application_bridge_reset_context();

    tres = SL_TunerSetFrequency(tUnit, freqKHz*1000);
    if (tres != 0)
    {
        SL_Printf("\n Error:SL_TunerSetFrequency :");
        printToConsoleTunerError(tres);
        goto ERROR;
    }

    tres = SL_TunerGetConfiguration(tUnit, &tunerGetCfg);
    if (tres != 0)
    {
        SL_Printf("\n Error:SL_TunerGetConfiguration :");
        printToConsoleTunerError(tres);
        goto ERROR;
    } else {
        if (tunerGetCfg.std == SL_TUNERSTD_ATSC3_0)
        {
            SL_Printf("\n Tuner Config Std       : ATSC3.0");
        }
        else
        {
            SL_Printf("\n Tuner Config Std       : NA");
        }
        switch (tunerGetCfg.bandwidth)
        {
            case SL_TUNER_BW_6MHZ:
                SL_Printf("\n Tuner Config Bandwidth : 6MHz");
                break;

            case SL_TUNER_BW_7MHZ:
                SL_Printf("\n Tuner Config Bandwidth : 7MHz");
                break;

            case SL_TUNER_BW_8MHZ:
                SL_Printf("\n Tuner Config Bandwidth : 8MHz");
                break;

            default:
                SL_Printf("\n Tuner Config Bandwidth : NA");
        }
    }

    tres = SL_TunerGetFrequency(tUnit, &cFrequency);
    if (tres != 0)
    {
        SL_Printf("\n Error:SL_TunerGetFrequency :");
        printToConsoleTunerError(tres);
        goto ERROR;
    }
    else
    {
        SL_Printf("\n Tuner Locked Frequency : %dHz", cFrequency);

    }

    tres = SL_TunerGetStatus(tUnit, &tunerInfo);
    if (tres != 0)
    {
        SL_Printf("\n Error:SL_TunerGetStatus :");
        printToConsoleTunerError(tres);
        goto ERROR;
    }
    else
    {
        SL_Printf("\n Tuner Lock status      : ");
        SL_Printf((tunerInfo.status == 1) ? "LOCKED" : "NOT LOCKED");
        SL_Printf("\n Tuner RSSI             : %3.2f dBm", tunerInfo.signalStrength);

        printf("\n Tuner Lock status      : ");
        printf((tunerInfo.status == 1) ? "LOCKED" : "NOT LOCKED");
        printf("\n Tuner RSSI             : %3.2f dBm", tunerInfo.signalStrength);

        printf("tuner frequency: %d", cFrequency);
    }

    //setup shared memory allocs
    if(!cb) {
        cb = CircularBufferCreate(CB_SIZE);
    }

    if (!atsc3_sl_tlv_block) {
        allocate_atsc3_sl_tlv_block();
    }


    if(!processThreadIsRunning) {
        processThreadShouldRun = true;
        pThread = pthread_create(&pThreadID, NULL, (THREADFUNCPTR)&SaankhyaPHYAndroid::ProcessThread, (void*)this);
        if (pThread != 0) {
            //processFlag = 0;
            printf("\n Process Thread failed to launch");
            goto ERROR;
        } else  {
            //processFlag = 1;
        }
    }

    if(!captureThreadIsRunning) {
        captureThreadShouldRun = true;
        printf("creating capture thread with cb buffer size: %d, tlv_block_size: %d",
               CB_SIZE, BUFFER_SIZE);
        cThread = pthread_create(&cThreadID, NULL, (THREADFUNCPTR)&SaankhyaPHYAndroid::CaptureThread, (void*)this);
        if (cThread != 0) {
            printf("\n Capture Thread failed to launch");
            goto ERROR;
        }
    }

    if(!statusThreadIsRunning) {
        statusThreadShouldRun = true;
        sThread = pthread_create(&sThreadID, NULL, (THREADFUNCPTR) &SaankhyaPHYAndroid::TunerStatusThread, (void*)this);
        if (sThread != 0) {
            printf("\n Capture Thread launched unsuccessfully");
            goto ERROR;
        }
    }

    while (SL_IsRxDataStarted() != 1)
    {
        SL_SleepMS(100);
    }
    SL_Printf("\n Starting SLDemod: ");

    slres = SL_DemodStart(slUnit);

    if (slres != 0)
    {
        SL_Printf("\n Saankhya Demod Start Failed");
        goto ERROR;
    }
    else
    {
        demodStartStatus = 1;
        SL_Printf("SUCCESS");
        //SL_Printf("\n SL Demod Output Capture: STARTED : sl-tlv.bin");
    }
    SL_SleepMS(1000); // Delay to accomdate set configurations at SL to take effect


    plpInfo.plp0 = plpid;
    plpInfo.plp1 = 0xFF;
    plpInfo.plp2 = 0xFF;
    plpInfo.plp3 = 0xFF;

    slres = SL_DemodConfigPlps(slUnit, &plpInfo);
    if (slres != 0)
    {
        SL_Printf("\n Error:SL_DemodConfigPlps :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    slres = SL_DemodGetConfiguration(slUnit, &cfgInfo);
    if (slres != SL_OK)
    {
        SL_Printf("\n Error:SL_DemodGetConfiguration :");
        printToConsoleDemodError(slres);
        if (slres == SL_ERR_CMD_IF_FAILURE)
        {
            handleCmdIfFailure();
            goto ERROR;
        }
    }
    else
    {
        printToConsoleDemodConfiguration(cfgInfo);
    }

    ret = 0;
    goto UNLOCK;

 ERROR:
    ret = -1;

    //unlock our i2c mutext
UNLOCK:
    SL_I2C_command_mutex_tuner_tune.unlock();
    return ret;

}

int SaankhyaPHYAndroid::listen_plps(vector<uint8_t> plps_orignal_list)
{
    vector<uint8_t> plps;
    for(int i=0; i < plps_orignal_list.size(); i++) {
        if(plps_orignal_list.at(i) == 0) {
            //skip, duplicate plp0 will cause demod to fail
        } else {
            plps.push_back(plps_orignal_list.at(i));
        }
    }

    if(plps.size() == 0) {
        //we always need to listen to plp0...kinda
        plpInfo.plp0 = 0;
        plpInfo.plp1 = 0xFF;
        plpInfo.plp2 = 0xFF;
        plpInfo.plp3 = 0xFF;
    } else if(plps.size() == 1) {
        plpInfo.plp0 = 0;
        plpInfo.plp1 = plps.at(0);
        plpInfo.plp2 = 0xFF;
        plpInfo.plp3 = 0xFF;
    } else if(plps.size() == 2) {
        plpInfo.plp0 = 0;
        plpInfo.plp1 = plps.at(0);
        plpInfo.plp2 = plps.at(1);
        plpInfo.plp3 = 0xFF;
    } else if(plps.size() == 3) {
        plpInfo.plp0 = 0;
        plpInfo.plp1 = plps.at(0);
        plpInfo.plp2 = plps.at(1);
        plpInfo.plp3 = plps.at(2);
    } else if(plps.size() == 4) {
        plpInfo.plp0 = plps.at(0);
        plpInfo.plp1 = plps.at(1);
        plpInfo.plp2 = plps.at(2);
        plpInfo.plp3 = plps.at(3);
    }

    printf("calling SL_DemodConfigPLPS with 0: %02x, 1: %02x, 2: %02x, 3: %02x",
            plpInfo.plp0,
            plpInfo.plp1,
            plpInfo.plp2,
            plpInfo.plp3);

    unique_lock<mutex> SL_I2C_command_mutex_config_plps(SL_I2C_command_mutex);
    slres = SL_DemodConfigPlps(slUnit, &plpInfo);
    if (slres != 0)
    {
        printf("Error: SL_DemodConfigPLP: %d", slres);
        SL_Printf("\n Error:SL_DemodConfigPlps :");
        printToConsoleDemodError(slres);
    }

    SL_I2C_command_mutex_config_plps.unlock();

    return slres;
}

void SaankhyaPHYAndroid::dump_plp_list() {
    slres = SL_DemodGetLlsPlpList(slUnit, &llsPlpInfo);
    if (slres != SL_OK)
    {
        SL_Printf("\n Error:SL_DemodGetLlsPlpList :");
        printToConsoleDemodError(slres);
        if (slres == SL_ERR_CMD_IF_FAILURE)
        {
            handleCmdIfFailure();
            return;
        }
    }

    plpllscount = 0;
    for (int plpIndx = 0; (plpIndx < 64) && (plpllscount < 4); plpIndx++)
    {
        plpInfoVal = ((llsPlpInfo & (llsPlpMask << plpIndx)) == pow(2, plpIndx)) ? 0x01 : 0xFF;

        printf("PLP: %d, plpInfoVal: %d", plpIndx, plpInfoVal);

        if (plpInfoVal == 0x01)
        {
            plpllscount++;
            if (plpllscount == 1)
            {
                plpInfo.plp0 = plpIndx;
            }
            else if (plpllscount == 2)
            {
                plpInfo.plp1 = plpIndx;
            }
            else if (plpllscount == 3)
            {
                plpInfo.plp2 = plpIndx;
            }
            else if (plpllscount == 4)
            {
                plpInfo.plp3 = plpIndx;
            }
            else
            {
                plpllscount++;
            }
        }
    }

    if (plpInfo.plp0 == -1)
    {
        plpInfo.plp0 = 0x00;
    }

}

int SaankhyaPHYAndroid::download_bootloader_firmware(int fd, string device_path) {
    SL_SetUsbFd(fd);

    SL_I2cResult_t i2cres;

    printf("SL_I2cPreInit - Before, path: %s, fd: %d", device_path.c_str(), fd);
    i2cres = SL_I2cPreInit();
    printf("SL_I2cPreInit returned: %d", i2cres);

    if (i2cres != SL_I2C_OK)
    {
        if(i2cres == SL_I2C_AWAITING_REENUMERATION) {
            printf("\n INFO:SL_I2cPreInit SL_FX3S_I2C_AWAITING_REENUMERATION");
            //sleep for 2s
            sleep(2);
            return 0;
        } else {
            printf("\n Error:SL_I2cPreInit failed: %d", i2cres);
            printToConsoleI2cError(i2cres);
        }
    }
    return -1;
}

SL_ConfigResult_t SaankhyaPHYAndroid::configPlatformParams() {

    SL_ConfigResult_t res;

#define SL_DEMOD_OUTPUT_SDIO 1
    /*
     * Assign Platform Configuration Parameters. For other ref platforms, replace settings from
     * comments above
     */
    sPlfConfig.chipType = SL_CHIP_4000;
    sPlfConfig.chipRev = SL_CHIP_REV_AA;
    sPlfConfig.boardType = SL_BORQS_EVT;
    sPlfConfig.tunerType = TUNER_SI;
    sPlfConfig.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
    sPlfConfig.demodOutputIf = SL_DEMOD_OUTPUTIF_SDIO;
    sPlfConfig.demodI2cAddr = 0x30; /* SLDemod 7-bit Physical I2C Address */

#ifdef SL_FX3S
    sPlfConfig.demodResetGpioPin = 47;   /* FX3S GPIO 47 connected to Demod Reset Pin */
    sPlfConfig.cpldResetGpioPin = 43;   /* FX3S GPIO 43 connected to CPLD Reset Pin and used only for serial TS Interface  */
    sPlfConfig.demodI2cAddr3GpioPin = 37;   /* FX3S GPIO 37 connected to Demod I2C Address3 Pin and used only for SDIO Interface */
#endif

    /*
     * Relative Path to SLSDK from working directory
     * Example: D:\UNAME\PROJECTS\slsdk
     * User can just specifying "..", which will point to this directory or can specify full directory path explicitly
     */
    sPlfConfig.slsdkPath = ".";

    /* Set Configuration Parameters */
    res = SL_ConfigSetPlatform(sPlfConfig);

    printf("configPlatformParams: with boardType: %d", sPlfConfig.boardType);

    return res;

}



void SaankhyaPHYAndroid::handleCmdIfFailure(void)
{
    SL_Printf("\n SL CMD IF FAILURE: Cannot continue!");
    SL_DemodUnInit(slUnit);
    SL_TunerUnInit(tUnit);
    //processFlag = 0;
    //diagFlag = 0;
}

void SaankhyaPHYAndroid::printToConsoleI2cError(SL_I2cResult_t err)
{
    switch (err)
    {
        case SL_I2C_ERR_TRANSFER_FAILED:
            SL_Printf(" Sl I2C Transfer Failed");
            break;
        case SL_I2C_ERR_NOT_INITIALIZED:
            SL_Printf(" Sl I2C Not Initialized");
            break;

        case SL_I2C_ERR_BUS_TIMEOUT:
            SL_Printf(" Sl I2C Bus Timeout");
            break;

        case SL_I2C_ERR_LOST_ARBITRATION:
            SL_Printf(" Sl I2C Lost Arbitration");
            break;

        default:
            break;
    }
}

void SaankhyaPHYAndroid::printToConsoleTunerError(SL_TunerResult_t err)
{
    switch (err)
    {
        case SL_TUNER_ERR_OPERATION_FAILED:
            SL_Printf(" Sl Tuner Operation Failed");
            break;

        case SL_TUNER_ERR_INVALID_ARGS:
            SL_Printf(" Sl Tuner Invalid Argument");
            break;

        case SL_TUNER_ERR_NOT_SUPPORTED:
            SL_Printf(" Sl Tuner Not Supported");
            break;

        case SL_TUNER_ERR_MAX_INSTANCES_REACHED:
            SL_Printf(" Sl Tuner Maximum Instance Reached");
            break;
        default:
            break;
    }
}

void SaankhyaPHYAndroid::printToConsolePlfConfiguration(SL_PlatFormConfigParams_t cfgInfo)
{
    SL_Printf("\n\n SL Platform Configuration");
    switch (cfgInfo.boardType)
    {
        case SL_EVB_3000:
            SL_Printf("\n Board Type             : SL_EVB_3000");
            break;

        case SL_EVB_3010:
            SL_Printf("\n Board Type             : SL_EVB_3010");
            break;

        case SL_EVB_4000:
            SL_Printf("\n Board Type             : SL_EVB_4000");
            break;

        case SL_KAILASH_DONGLE:
            SL_Printf("\n Board Type             : SL_KAILASH_DONGLE");
            break;

        case SL_BORQS_EVT:
            SL_Printf("\n Board Type             : SL_BORQS_EVT");
            break;

        default:
            SL_Printf("\n Board Type             : NA");
    }

    switch (cfgInfo.chipType)
    {
        case SL_CHIP_3000:
            SL_Printf("\n Chip Type              : SL_CHIP_3000");
            break;

        case SL_CHIP_3010:
            SL_Printf("\n Chip Type              : SL_CHIP_3010");
            break;

        case SL_CHIP_4000:
            SL_Printf("\n Chip Type              : SL_CHIP_4000");
            break;

        default:
            SL_Printf("\n Chip Type              : NA");
    }

    if (cfgInfo.chipRev == SL_CHIP_REV_AA)
    {
        SL_Printf("\n Chip Revision          : SL_CHIP_REV_AA");
    }
    else if (cfgInfo.chipRev == SL_CHIP_REV_BB)
    {
        SL_Printf("\n Chip Revision          : SL_CHIP_REV_BB");
    }
    else
    {
        SL_Printf("\n Chip Revision          : NA");
    }

    if (cfgInfo.tunerType == TUNER_NXP)
    {
        SL_Printf("\n Tuner Type             : TUNER_NXP");
    }
    else if (cfgInfo.tunerType == TUNER_SI)
    {
        SL_Printf("\n Tuner Type             : TUNER_SI");
    }
    else
    {
        SL_Printf("\n Tuner Type             : NA");
    }

    switch (cfgInfo.demodControlIf)
    {
        case SL_DEMOD_CMD_CONTROL_IF_I2C:
            SL_Printf("\n Command Interface      : SL_DEMOD_CMD_CONTROL_IF_I2C");
            break;

        case SL_DEMOD_CMD_CONTROL_IF_SDIO:
            SL_Printf("\n Command Interface      : SL_DEMOD_CMD_CONTROL_IF_SDIO");
            break;

        case SL_DEMOD_CMD_CONTROL_IF_SPI:
            SL_Printf("\n Command Interface      : SL_DEMOD_CMD_CONTROL_IF_SPI");
            break;

        default:
            SL_Printf("\n Command Interface      : NA");
    }

    switch (cfgInfo.demodOutputIf)
    {
        case SL_DEMOD_OUTPUTIF_TS:
            SL_Printf("\n Output Interface       : SL_DEMOD_OUTPUTIF_TS");
            break;

        case SL_DEMOD_OUTPUTIF_SDIO:
            SL_Printf("\n Output Interface       : SL_DEMOD_OUTPUTIF_SDIO");
            break;

        case SL_DEMOD_OUTPUTIF_SPI:
            SL_Printf("\n Output Interface       : SL_DEMOD_OUTPUTIF_SPI");
            break;

        default:
            SL_Printf("\n Output Interface       : NA");
    }

    SL_Printf("\n Demod I2C Address      : 0x%x\n", cfgInfo.demodI2cAddr);
}

void SaankhyaPHYAndroid::printToConsoleDemodConfiguration(SL_DemodConfigInfo_t cfgInfo)
{
    SL_Printf("\n\n SL Demod Configuration");
    switch (cfgInfo.std)
    {
        case SL_DEMODSTD_ATSC3_0:
            SL_Printf("\n Standard               : ATSC3_0");
            break;

        case SL_DEMODSTD_ATSC1_0:
            SL_Printf("\n Demod Standard         : ATSC1_0");
            break;

        default:
            SL_Printf("\n Demod Standard         : NA");
    }

    SL_Printf("\n PLP Configuration");
    SL_Printf("\n   PLP0                 : %d", (signed char)cfgInfo.plpInfo.plp0);
    SL_Printf("\n   PLP1                 : %d", (signed char)cfgInfo.plpInfo.plp1);
    SL_Printf("\n   PLP2                 : %d", (signed char)cfgInfo.plpInfo.plp2);
    SL_Printf("\n   PLP3                 : %d", (signed char)cfgInfo.plpInfo.plp3);

    SL_Printf("\n Input Configuration");
    switch (cfgInfo.afeIfInfo.iftype)
    {
        case SL_IFTYPE_ZIF:
            SL_Printf("\n   IF Type              : ZIF");
            break;

        case SL_IFTYPE_LIF:
            SL_Printf("\n   IF Type              : LIF");
            break;

        default:
            SL_Printf("\n   IF Type              : NA");
    }

    switch (cfgInfo.afeIfInfo.iqswap)
    {
        case SL_IQSWAP_DISABLE:
            SL_Printf("\n   IQSWAP               : DISABLE");
            break;

        case SL_IQSWAP_ENABLE:
            SL_Printf("\n   IQSWAP               : ENABLE");
            break;

        default:
            SL_Printf("\n   IQSWAP               : NA");
    }

    switch (cfgInfo.afeIfInfo.iswap)
    {
        case SL_IPOL_SWAP_DISABLE:
            SL_Printf("\n   ISWAP                : DISABLE");
            break;

        case SL_IPOL_SWAP_ENABLE:
            SL_Printf("\n   ISWAP                : ENABLE");
            break;

        default:
            SL_Printf("\n   ISWAP                : NA");
    }

    switch (cfgInfo.afeIfInfo.qswap)
    {
        case SL_QPOL_SWAP_DISABLE:
            SL_Printf("\n   QSWAP                : DISABLE");
            break;

        case SL_QPOL_SWAP_ENABLE:
            SL_Printf("\n   QSWAP                : ENABLE");
            break;

        default:
            SL_Printf("\n   QSWAP                : NA");
    }

    SL_Printf("\n   ICoeff1              : %3.4f", cfgInfo.iqOffCorInfo.iCoeff1);
    SL_Printf("\n   QCoeff1              : %3.4f", cfgInfo.iqOffCorInfo.qCoeff1);
    SL_Printf("\n   ICoeff2              : %3.4f", cfgInfo.iqOffCorInfo.iCoeff2);
    SL_Printf("\n   QCoeff2              : %3.4f", cfgInfo.iqOffCorInfo.qCoeff2);

    SL_Printf("\n   AGCReference         : %d mv", cfgInfo.afeIfInfo.agcRefValue);
    SL_Printf("\n   Tuner IF Frequency   : %3.2f MHz", cfgInfo.afeIfInfo.ifreq);

    SL_Printf("\n Output Configuration");
    switch (cfgInfo.oifInfo.oif)
    {
        case SL_OUTPUTIF_TSPARALLEL_LSB_FIRST:
            SL_Printf("\n   OutputInteface       : TS PARALLEL LSB FIRST");
            break;

        case SL_OUTPUTIF_TSPARALLEL_MSB_FIRST:
            SL_Printf("\n   OutputInteface       : TS PARALLEL MSB FIRST");
            break;

        case SL_OUTPUTIF_TSSERIAL_LSB_FIRST:
            SL_Printf("\n   OutputInteface       : TS SERAIL LSB FIRST");
            break;

        case SL_OUTPUTIF_TSSERIAL_MSB_FIRST:
            SL_Printf("\n   OutputInteface       : TS SERIAL MSB FIRST");
            break;

        case SL_OUTPUTIF_SDIO:
            SL_Printf("\n   OutputInteface       : SDIO");
            break;

        case SL_OUTPUTIF_SPI:
            SL_Printf("\n   OutputInteface       : SPI");
            break;

        default:
            SL_Printf("\n   OutputInteface       : NA");
    }

    switch (cfgInfo.oifInfo.TsoClockInvEnable)
    {
        case SL_TSO_CLK_INV_OFF:
            SL_Printf("\n   TS Out Clock Inv     : DISABLED");
            break;

        case SL_TSO_CLK_INV_ON:
            SL_Printf("\n   TS Out Clock Inv     : ENABLED");
            break;

        default:
            SL_Printf("\n    TS Out Clock Inv    : NA");
    }
}

void SaankhyaPHYAndroid::printToConsoleDemodError(SL_Result_t err)
{
    switch (err)
    {
        case SL_ERR_OPERATION_FAILED:
            SL_Printf(" Sl Operation Failed");
            break;

        case SL_ERR_TOO_MANY_INSTANCES:
            SL_Printf(" Sl Too Many Instance");
            break;

        case SL_ERR_CODE_DWNLD:
            SL_Printf(" Sl Code download Failed");
            break;

        case SL_ERR_INVALID_ARGUMENTS:
            SL_Printf(" Sl Invalid Argument");
            break;

        case SL_ERR_CMD_IF_FAILURE:
            SL_Printf(" Sl Command Interface Failure");
            break;

        case SL_ERR_NOT_SUPPORTED:
            SL_Printf(" Sl Not Supported");
            break;
        default:
            break;
    }
}

void* SaankhyaPHYAndroid::ProcessThread(void* context)
{
    printf("SaankhyaPHYAndroid::ProcessThread: with context: %p", context);

    SaankhyaPHYAndroid* apiImpl = (SaankhyaPHYAndroid*) context;
    apiImpl->processThreadIsRunning = true;

    apiImpl->pinConsumerThreadAsNeeded();

    apiImpl->resetProcessThreadStatistics();

    while (apiImpl->processThreadShouldRun)
    {
        //printf("atsc3NdkClientSlImpl::ProcessThread: getDataSize is: %d", CircularBufferGetDataSize(cb));

        while(CircularBufferGetDataSize(apiImpl->cb) >= BUFFER_SIZE) {
            apiImpl->processTLVFromCallback();
        }
        usleep(10000);
    }

    apiImpl->releasePinnedConsumerThreadAsNeeded();

    apiImpl->processThreadIsRunning = false;
    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::ProcessThread complete");

    return 0;
}

//SL_Fx3s_RxDataStop
void* SaankhyaPHYAndroid::CaptureThread(void* context)
{
    SaankhyaPHYAndroid* apiImpl = (SaankhyaPHYAndroid*) context;
    apiImpl->captureThreadIsRunning = true;

    apiImpl->pinProducerThreadAsNeeded();

    SL_RxDataStart((RxDataCB)&SaankhyaPHYAndroid::RxDataCallback);

    apiImpl->releasePinnedProducerThreadAsNeeded();

    apiImpl->captureThreadIsRunning = false;

    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::CaptureThread complete");

    return 0;
}

void* SaankhyaPHYAndroid::TunerStatusThread(void* context)
{

    SaankhyaPHYAndroid* apiImpl = (SaankhyaPHYAndroid*) context;
    apiImpl->statusThreadIsRunning = true;

    apiImpl->pinStatusThreadAsNeeded();

    SL_Result_t sl_res;
    SL_TunerResult_t tres;
    SL_TunerSignalInfo_t tunerInfo;
    SL_DemodLockStatus_t demodLockStatus;
    uint cpuStatus = 0;
    uint lastCpuStatus = 0;
    SL_Result_t dres;

    unsigned long long llsPlpInfo;

    SL_Atsc3p0Perf_Diag_t perfDiag;
    SL_Atsc3p0Bsr_Diag_t  bsrDiag;
    SL_Atsc3p0L1B_Diag_t  l1bDiag;
    SL_Atsc3p0L1D_Diag_t  l1dDiag;

    double snr;
    double ber_l1b;
    double ber_l1d;
    double ber_plp0;
    unique_lock<mutex> SL_I2C_command_mutex_tuner_status_io(apiImpl->SL_I2C_command_mutex, std::defer_lock);
    bool first_run = true;

    while(apiImpl->statusThreadShouldRun) {

        //only actively poll the tuner status if the RF status window is visible
//        if(!atsc3NdkClientSlImpl::tunerStatusThreadShouldPollTunerStatus) {
//            usleep(1000000);
//            continue;
//        }

        if(!first_run) {
            SL_I2C_command_mutex_tuner_status_io.unlock();
        }
        first_run = false;

        if(lastCpuStatus == 0xFFFFFFFF) {
            usleep(1000000); //jjustman: target: sleep for 500ms
            //TODO: jjustman-2019-12-05: investigate FX3 firmware and i2c single threaded interrupt handling instead of dma xfer
        } else {
            usleep(2500000);
        }
        lastCpuStatus = 0;

        /*jjustman-2020-01-06: For the SL3000/SiTune, we will have 3 status attributes with the following possible values:

                tunerInfo.status:   SL_TUNER_STATUS_NOT_LOCKED (0)
                                    SL_TUNER_STATUS_LOCKED (1)

                demodLockStatus:    SL_DEMOD_STATUS_NOT_LOCK (0)
                                    SL_DEMOD_STATUS_LOCK (1)

                cpuStatus:          (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED",
         */

        SL_I2C_command_mutex_tuner_status_io.lock();

        SL_DemodConfigInfo_t demodInfo;
        sl_res = SL_DemodGetConfiguration(apiImpl->slUnit, &demodInfo);
        if(sl_res != SL_OK) {
            printf("Error calling SL_DemodGetConfiguration");
            continue;
        }

        printf("DemodGetConfiguration, plp's: 0: %02x, 1: %02x, 2: %02x, 3: %02x",
                demodInfo.plpInfo.plp0,
                demodInfo.plpInfo.plp1,
                demodInfo.plpInfo.plp2,
                demodInfo.plpInfo.plp3);


        tres = SL_TunerGetStatus(apiImpl->tUnit, &tunerInfo);
        if (tres != SL_TUNER_OK) {
            //atsc3NdkClientSlImpl::atsc3NdkClientSLRef->LogMsgF("Error:SL_TunerGetStatus: deviceHandle: %p, res: %d", __deviceHandle_FIXME, tres);
            printf("\n Error:SL_TunerGetStatus: tres: %d", tres);
            //printToConsoleTunerError(tres);
            continue;
        }

        dres = SL_DemodGetStatus(apiImpl->slUnit, SL_DEMOD_STATUS_TYPE_LOCK, (SL_DemodLockStatus_t*)&demodLockStatus);
        if (dres != SL_OK) {
            printf("\n Error:SL_Demod Get Lock Status                :");
            // printToConsoleDemodError(dres);
            continue;
        }

        dres = SL_DemodGetStatus(apiImpl->slUnit, SL_DEMOD_STATUS_TYPE_CPU, (int*)&cpuStatus);
        if (dres != SL_OK) {
            printf("\n Error:SL_Demod Get CPU Status                 :");
            //printToConsoleDemodError(dres);
            continue;
        }

        lastCpuStatus = cpuStatus;

        dres = SL_DemodGetAtsc3p0Diagnostics(apiImpl->slUnit, SL_DEMOD_DIAG_TYPE_PERF, (SL_Atsc3p0Perf_Diag_t*)&perfDiag);
        if (dres != SL_OK) {
            printf("\n Error getting ATSC3.0 Performance Diagnostics :");
            // printToConsoleDemodError(dres);
            continue;
        }

        dres = SL_DemodGetAtsc3p0Diagnostics(apiImpl->slUnit, SL_DEMOD_DIAG_TYPE_BSR, (SL_Atsc3p0Bsr_Diag_t*)&bsrDiag);
        if (dres != SL_OK) {
            printf("\n Error getting ATSC3.0 Boot Strap Diagnostics  :");
            //  printToConsoleDemodError(dres);
            continue;
        }

        dres = SL_DemodGetAtsc3p0Diagnostics(apiImpl->slUnit, SL_DEMOD_DIAG_TYPE_L1B, (SL_Atsc3p0L1B_Diag_t*)&l1bDiag);
        if (dres != SL_OK) {
            printf("\n Error getting ATSC3.0 L1B Diagnostics         :");
            // printToConsoleDemodError(dres);
            continue;
        }

        dres = SL_DemodGetAtsc3p0Diagnostics(apiImpl->slUnit, SL_DEMOD_DIAG_TYPE_L1D, (SL_Atsc3p0L1D_Diag_t*)&l1dDiag);
        if (dres != SL_OK) {
            printf("\n Error getting ATSC3.0 L1D Diagnostics         :");
            //    printToConsoleDemodError(dres);
            continue;
        }

        int slres = SL_DemodGetLlsPlpList(apiImpl->slUnit, &llsPlpInfo);
        if (slres != SL_OK) {
            printf("\n Error:SL_DemodGetLlsPlpList :");
            continue;
        }


        snr = (float)perfDiag.GlobalSnrLinearScale / 16384;
        snr = 10000.0 * log10(snr); //10

        ber_l1b = (float)perfDiag.NumBitErrL1b / perfDiag.NumFecBitsL1b; // //aBerPreLdpcE7,
        ber_l1d = (float)perfDiag.NumBitErrL1d / perfDiag.NumFecBitsL1d;//aBerPreBchE9,
        ber_plp0 = (float)perfDiag.NumBitErrPlp0 / perfDiag.NumFecBitsPlp0; //aFerPostBchE6,

        printf("atsc3NdkClientSlImpl::TunerStatusThread: tunerInfo.status: %d, tunerInfo.signalStrength: %f, cpuStatus: %s, demodLockStatus: %d, globalSnr: %f",
               tunerInfo.status,
               tunerInfo.signalStrength,
               (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED",
               demodLockStatus,
               perfDiag.GlobalSnrLinearScale);

        if(atsc3_ndk_phy_bridge_get_instance()) {

            atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_stats(tunerInfo.status == 1,
                tunerInfo.signalStrength,
                saankhyaPHYAndroid->plpInfo.plp0 == l1dDiag.sfParams[0].PlpParams[0].L1dSfPlpId,
                l1dDiag.sfParams[0].PlpParams[0].L1dSfPlpFecType,
                l1dDiag.sfParams[0].PlpParams[0].L1dSfPlpModType,
                l1dDiag.sfParams[0].PlpParams[0].L1dSfPlpCoderate,
                tunerInfo.signalStrength/1000,
                snr,
                ber_l1b,
                ber_l1d,
                ber_plp0,
                demodLockStatus,
                cpuStatus == 0xFFFFFFFF,
                llsPlpInfo & 0x01 == 0x01,
                0);

            atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_bw_stats(saankhyaPHYAndroid->alp_completed_packets_parsed,
                                                                          saankhyaPHYAndroid->alp_total_bytes,
                                                                          saankhyaPHYAndroid->alp_total_LMTs_recv);
            }
    }

    apiImpl->releasePinnedStatusThreadAsNeeded();
    apiImpl->statusThreadIsRunning = false;
    return 0;
}

//jjustman-2020-08-23 - TODO: wire up these callbacks in SaankhyaPHYAndroid::cctor rather than direct
//coupling to atsc3_core_service_bridge

void SaankhyaPHYAndroid::processTLVFromCallback()
{
    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);
    int bytesRead = CircularBufferPop(cb, BUFFER_SIZE, (char*)&processDataCircularBufferForCallback);
    CircularBufferMutex_local.unlock();

    unique_lock<mutex> atsc3_sl_tlv_block_mutex_local(atsc3_sl_tlv_block_mutex);

    if(!atsc3_sl_tlv_block) {
        _SAANKHYA_PHY_ANDROID_WARN("ERROR: atsc3NdkClientSlImpl::processTLVFromCallback - atsc3_sl_tlv_block is NULL!");
        allocate_atsc3_sl_tlv_block();
    }

    if(bytesRead) {

        block_Write(atsc3_sl_tlv_block, (uint8_t*)&processDataCircularBufferForCallback, bytesRead);
        block_Rewind(atsc3_sl_tlv_block);

        bool atsc3_sl_tlv_payload_complete = false;

        do {
            atsc3_sl_tlv_payload = atsc3_sl_tlv_payload_parse_from_block_t(atsc3_sl_tlv_block);

            if(atsc3_sl_tlv_payload) {
                atsc3_sl_tlv_payload_dump(atsc3_sl_tlv_payload);
                if(atsc3_sl_tlv_payload->alp_payload_complete) {
                    atsc3_sl_tlv_payload_complete = true;

                    block_Rewind(atsc3_sl_tlv_payload->alp_payload);
                    atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_parse(atsc3_sl_tlv_payload->plp_number, atsc3_sl_tlv_payload->alp_payload);
                    if(atsc3_alp_packet) {
                        alp_completed_packets_parsed++;

                        alp_total_bytes += atsc3_alp_packet->alp_payload->p_size;

                        if(atsc3_alp_packet->alp_packet_header.packet_type == 0x00) {

                            block_Rewind(atsc3_alp_packet->alp_payload);
                            if(atsc3_phy_rx_udp_packet_process_callback) {
                                atsc3_phy_rx_udp_packet_process_callback(atsc3_sl_tlv_payload->plp_number, atsc3_alp_packet->alp_payload);
                            }

                        } else if(atsc3_alp_packet->alp_packet_header.packet_type == 0x4) {
                            alp_total_LMTs_recv++;
                            atsc3_link_mapping_table_t* atsc3_link_mapping_table_pending = atsc3_alp_packet_extract_lmt(atsc3_alp_packet);

                            if(atsc3_phy_rx_link_mapping_table_process_callback) {
                                atsc3_link_mapping_table_t *atsc3_link_mapping_table_to_free = atsc3_phy_rx_link_mapping_table_process_callback(atsc3_link_mapping_table_pending);

                                if (atsc3_link_mapping_table_to_free) {
                                    atsc3_link_mapping_table_free(&atsc3_link_mapping_table_to_free);
                                }
                            }
                        }

                        atsc3_alp_packet_free(&atsc3_alp_packet);
                    }
                    //free our atsc3_sl_tlv_payload
                    atsc3_sl_tlv_payload_free(&atsc3_sl_tlv_payload);

                } else {
                    atsc3_sl_tlv_payload_complete = false;
                    //jjustman-2019-12-29 - noisy, TODO: wrap in __DEBUG macro check
                    //printf("alp_payload->alp_payload_complete == FALSE at pos: %d, size: %d", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);
                }
            } else {
                atsc3_sl_tlv_payload_complete = false;
                //jjustman-2019-12-29 - noisy, TODO: wrap in __DEBUG macro check
                //printf("ERROR: alp_payload IS NULL, short TLV read?  at atsc3_sl_tlv_block: i_pos: %d, p_size: %d", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);
            }

        } while(atsc3_sl_tlv_payload_complete);


        if(atsc3_sl_tlv_payload && !atsc3_sl_tlv_payload->alp_payload_complete && atsc3_sl_tlv_block->i_pos > atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size) {
            //accumulate from our last starting point and continue iterating during next bbp
            atsc3_sl_tlv_block->i_pos -= atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size;
            //free our atsc3_sl_tlv_payload
            atsc3_sl_tlv_payload_free(&atsc3_sl_tlv_payload);
        }

        if(atsc3_sl_tlv_block->p_size > atsc3_sl_tlv_block->i_pos) {
            //truncate our current block_t starting at i_pos, and then read next i/o block
            block_t* temp_sl_tlv_block = block_Duplicate_from_position(atsc3_sl_tlv_block);
            block_Destroy(&atsc3_sl_tlv_block);
            atsc3_sl_tlv_block = temp_sl_tlv_block;
            block_Seek(atsc3_sl_tlv_block, atsc3_sl_tlv_block->p_size);
        } else if(atsc3_sl_tlv_block->p_size == atsc3_sl_tlv_block->i_pos) {
            //clear out our tlv block as we are the "exact" size of our last alp packet

            block_Destroy(&atsc3_sl_tlv_block);
            atsc3_sl_tlv_block = block_Alloc(BUFFER_SIZE);
        } else {
            printf("atsc3_sl_tlv_block: positioning mismatch: i_pos: %d, p_size: %d - rewinding and seeking for magic packet?", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);

            //jjustman: 2019-11-23: rewind in order to try seek for our magic packet in the next loop here
            block_Rewind(atsc3_sl_tlv_block);
        }
    }

    atsc3_sl_tlv_block_mutex_local.unlock();

}

void SaankhyaPHYAndroid::RxDataCallback(unsigned char *data, long len)
{
    //printf("atsc3NdkClientSlImpl::RxDataCallback: pushing data: %p, len: %d", data, len);
    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);

    CircularBufferPush(SaankhyaPHYAndroid::cb, (char *)data, len);
    CircularBufferMutex_local.unlock();
}

void SaankhyaPHYAndroid::NotifyPlpSelectionChangeCallback(vector<uint8_t> plps, void *context) {
    ((SaankhyaPHYAndroid *) context)->listen_plps(plps);
}

void SaankhyaPHYAndroid::allocate_atsc3_sl_tlv_block() {
    unique_lock<mutex> atsc3_sl_tlv_block_mutex_local(atsc3_sl_tlv_block_mutex);
    if(!atsc3_sl_tlv_block) {
        atsc3_sl_tlv_block = block_Alloc(BUFFER_SIZE);
    }
    atsc3_sl_tlv_block_mutex_local.unlock();
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_init(JNIEnv *env, jobject instance) {
    unique_lock<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    _SAANKHYA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_init: start init, env: %p", env);
    if(saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_init: start init, saankhyaPHYAndroid is present: %p, calling deinit/delete", saankhyaPHYAndroid);
        saankhyaPHYAndroid->deinit();
        saankhyaPHYAndroid = nullptr;
    }

    saankhyaPHYAndroid = new SaankhyaPHYAndroid(env, instance);
    saankhyaPHYAndroid->init();

    _SAANKHYA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_init: return, instance: %p", saankhyaPHYAndroid);
    saankhy_phy_android_cctor_mutex_local.unlock();
    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_run(JNIEnv *env, jobject thiz) {
    unique_lock<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_run: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        res = saankhyaPHYAndroid->run();
        _SAANKHYA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_run: returning res: %d", res);
    }
    saankhy_phy_android_cctor_mutex_local.unlock();

    return res;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_is_1running(JNIEnv* env, jobject instance)
{
    unique_lock<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    jboolean res = false;

    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_is_1running: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = false;
    } else {
        res = saankhyaPHYAndroid->is_running();
    }
    saankhy_phy_android_cctor_mutex_local.unlock();

    return res;
}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_stop(JNIEnv *env, jobject thiz) {
    unique_lock<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_stop: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        res = saankhyaPHYAndroid->stop();
        _SAANKHYA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_stop: returning res: %d", res);
    }
    saankhy_phy_android_cctor_mutex_local.unlock();

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_deinit(JNIEnv *env, jobject thiz) {
    unique_lock<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_deinit: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {

        saankhyaPHYAndroid->deinit();
        saankhyaPHYAndroid = nullptr;
    }

    saankhy_phy_android_cctor_mutex_local.unlock();
    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_download_1bootloader_1firmware(JNIEnv *env, jobject thiz, jint fd, jstring device_path_jstring) {
    unique_lock<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    _SAANKHYA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_download_1bootloader_1firmware: fd: %d", fd);
    int res = 0;

    if(!saankhyaPHYAndroid)  {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_download_1bootloader_1firmware: saankhyaPHYAndroid is NULL!");
        res = -1;
    } else {
        const char* device_path_weak = env->GetStringUTFChars(device_path_jstring, 0);
        string device_path(device_path_weak);

        res = saankhyaPHYAndroid->download_bootloader_firmware(fd, device_path); //calls pre_init
        env->ReleaseStringUTFChars( device_path_jstring, device_path_weak );

        //jjustman-2020-08-23 - hack, clear out our in-flight reference since we should re-enumerate
        delete saankhyaPHYAndroid;
        saankhyaPHYAndroid = nullptr;
    }

    saankhy_phy_android_cctor_mutex_local.unlock();

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_open(JNIEnv *env, jobject thiz, jint fd, jstring device_path_jstring) {
    unique_lock<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    _SAANKHYA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_open: fd: %d", fd);

    int res = 0;
    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_open: saankhyaPHYAndroid is NULL!");
        res = -1;
    } else {
        const char* device_path_weak = env->GetStringUTFChars(device_path_jstring, 0);
        string device_path(device_path_weak);

        res = saankhyaPHYAndroid->open(fd, device_path);
        env->ReleaseStringUTFChars( device_path_jstring, device_path_weak );
    }
    _SAANKHYA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_open: fd: %d, return: %d", fd, res);

    saankhy_phy_android_cctor_mutex_local.unlock();

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_tune(JNIEnv *env, jobject thiz,
                                                                      jint freq_khz,
                                                                      jint single_plp) {

    unique_lock<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_tune: saankhyaPHYAndroid is NULL!");
        res = -1;
    } else {
        res = saankhyaPHYAndroid->tune(freq_khz, single_plp);
    }

    saankhy_phy_android_cctor_mutex_local.unlock();

    return res;
}
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_listen_1plps(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jobject plps) {
    unique_lock<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_listen_1plps: saankhyaPHYAndroid is NULL!");
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

        res = saankhyaPHYAndroid->listen_plps(listen_plps);
    }
    saankhy_phy_android_cctor_mutex_local.unlock();

    return res;
}