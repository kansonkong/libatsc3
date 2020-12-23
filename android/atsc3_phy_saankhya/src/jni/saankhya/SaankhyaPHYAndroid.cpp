//
// Created by Jason Justman on 8/19/20.
//

//jjustman-2020-10-30 - workaround for SL_TUNER deinit not decrementing ref count
#define __SL_TUNER_DEINIT_DISABLED__ true


/*  MarkONE "workarounds" for /dev handle permissions


ADB_IP_ADDRESS="192.168.4.57:5555"
adb connect $ADB_IP_ADDRESS
adb root
adb shell

adb remount
adb push jjlibatsc3 /

--->jjlibatsc3 contents:

setenforce 0
cd /dev
chmod 777 ion
chmod 777 i2c-3
chmod 777 saankhya_dev
while :
do
chmod 777 input/event7
chmod 777 saankhya_sdio_drv
sleep 1
done

 */

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

    if(this->atsc3_sl_tlv_block) {
        block_Destroy(&this->atsc3_sl_tlv_block);
    }

    if(atsc3_sl_tlv_payload) {
        atsc3_sl_tlv_payload_free(&atsc3_sl_tlv_payload);
    }

    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);

    if(cb) {
        CircularBufferFree(cb);
    }
    cb = nullptr;
    CircularBufferMutex_local.unlock();


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
    SL_I2cResult_t sl_res_uninit = SL_I2C_OK;
    SL_Result_t sl_result = SL_OK;
    SL_TunerResult_t sl_tuner_result = SL_TUNER_OK;

    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: enter with this: %p", this);
    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::stop: enter with this: %p, slUnit: %d, tUnit: %d, captureThreadIsRunning: %d, statusThreadIsRunning: %d, processThreadIsRunning: %d, sleeping for %d ms",
                              this,
                              this->slUnit,
                              this->tUnit,
                              this->captureThreadIsRunning,
                              this->statusThreadIsRunning,
                              this->processThreadIsRunning);



    statusThreadShouldRun = false;
    if(captureThreadIsRunning) {
        _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: setting captureThreadShouldRun: false");
        SL_RxDataStop();
    }
    captureThreadShouldRun = false;
    processThreadShouldRun = false;

    //tear down status thread first, as its the most 'problematic' with the saankhya i2c i/f processing
    while(this->statusThreadIsRunning) {
        SL_SleepMS(100);
        _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: this->statusThreadIsRunning: %d", this->statusThreadIsRunning);
    }

    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: before join for statusThreadHandle");
    if(statusThreadHandle.joinable()) {
        statusThreadHandle.join();
    }
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: after join for statusThreadHandle");

    while(this->captureThreadIsRunning) {
        SL_SleepMS(100);
        _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: this->captureThreadIsRunning: %d", this->captureThreadIsRunning);
    }

    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: before join for captureThreadHandle");
    if(captureThreadHandle.joinable()) {
        captureThreadHandle.join();
    }
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: after join for captureThreadHandle");

    if(processThreadIsRunning) {
        _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: setting processThreadShouldRun: false");
        while(this->processThreadIsRunning) {
            SL_SleepMS(100);
            _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: this->processThreadIsRunning: %d", this->processThreadIsRunning);
        }
    }

    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: before join for processThreadHandle");
    if(processThreadHandle.joinable()) {
        processThreadHandle.join();
    }
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: after join for processThreadHandle");

    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: before SL_I2cUnInit");

    sl_res_uninit = SL_I2cUnInit();

    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: after SL_I2cUnInit, sl_res_uninit is: %d", sl_res_uninit);

    //jjustman-2020-10-30 - decrement our sl instance count
    sl_result = SL_DemodUnInit(slUnit);
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: after SL_DemodUnInit, slUnit now: %d, sl_result: %d", slUnit, sl_result);
    slUnit = -1;

#ifndef __SL_TUNER_DEINIT_DISABLED__
    //jjustman-2020-10-30 - TODO: SL_TunerInit and SL_TunerUnInit is just a refcount, not an instance handle
    sl_tuner_result = SL_TunerUnInit(tUnit);
    tUnit = __MIN(0, tUnit - 1);
#endif

    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: after SL_TunerUnInit, tUnit now: %d, sl_tuner_result: %d", tUnit, sl_tuner_result);

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_clear_callback();
    }
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
        _SAANKHYA_PHY_ANDROID_DEBUG("ERROR : SL_ConfigGetPlatform Failed ");
        goto ERROR;
    }

    cres = SL_ConfigSetBbCapture(BB_CAPTURE_DISABLE);
    if (cres != SL_CONFIG_OK)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("ERROR : SL_ConfigSetBbCapture Failed ");
        goto ERROR;
    }

    _SAANKHYA_PHY_ANDROID_DEBUG("%s:%d - before SL_I2cInit()", __FILE__, __LINE__);

    if (getPlfConfig.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
    {
        i2cres = SL_I2cInit();
        if (i2cres != SL_I2C_OK)
        {
            _SAANKHYA_PHY_ANDROID_ERROR("ERROR : Error:SL_I2cInit failed Failed");

            _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_I2cInit failed :");
            printToConsoleI2cError(i2cres);
            goto ERROR;
        }
        else
        {
            cmdIf = SL_CMD_CONTROL_IF_I2C;
            _SAANKHYA_PHY_ANDROID_DEBUG("atsc3NdkClientSlImpl: setting cmdIf: %d", cmdIf);
        }
    }
    else if (getPlfConfig.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SDIO)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_SdioInit failed :Not Supported");
        goto ERROR;
    }
    else if (getPlfConfig.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SPI)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_SpiInit failed :Not Supported");
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
                _SAANKHYA_PHY_ANDROID_DEBUG("Invalid Tuner Selection");
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
                _SAANKHYA_PHY_ANDROID_DEBUG("Invalid OutPut Interface Selection");
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
                _SAANKHYA_PHY_ANDROID_DEBUG("using TUNER_SI, ifreq: 0");
                afeInfo.spectrum = SL_SPECTRUM_NORMAL;
                afeInfo.iftype = SL_IFTYPE_ZIF;
                afeInfo.ifreq = 0.0;
            }
            else
            {
                _SAANKHYA_PHY_ANDROID_DEBUG("Invalid Tuner Selection");
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
                _SAANKHYA_PHY_ANDROID_DEBUG("Invalid Output Interface Selection");
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
                _SAANKHYA_PHY_ANDROID_DEBUG("Invalid Tuner Selection");
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
                _SAANKHYA_PHY_ANDROID_DEBUG("%s:%d - SL4000 using SL_DEMOD_OUTPUTIF_SDIO", __FILE__, __LINE__);

                outPutInfo.oif = SL_OUTPUTIF_SDIO;
            }
            else
            {
                _SAANKHYA_PHY_ANDROID_DEBUG("Invalid Output Interface Selection");
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
                _SAANKHYA_PHY_ANDROID_DEBUG("using SL_KAILASH with SPECTRUM_NORMAL and ZIF");
                afeInfo.spectrum = SL_SPECTRUM_NORMAL;
                afeInfo.iftype = SL_IFTYPE_ZIF;
                afeInfo.ifreq = 0.0;
            }
            else
            {
                _SAANKHYA_PHY_ANDROID_DEBUG("Invalid Tuner Type selected ");
            }

            if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
            {
                outPutInfo.oif = SL_OUTPUTIF_TSPARALLEL_LSB_FIRST;
            }
            else
            {
                _SAANKHYA_PHY_ANDROID_DEBUG("Invalid OutPut Interface Selection");
            }

            afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
            afeInfo.qswap = SL_QPOL_SWAP_ENABLE;
            iqOffSetCorrection.iCoeff1 = (float)1.00724023045574;
            iqOffSetCorrection.qCoeff1 = (float)0.998403791546105;
            iqOffSetCorrection.iCoeff2 = (float)0.0432678874719328;
            iqOffSetCorrection.qCoeff2 = (float)0.0436508327768608;
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("Invalid Board Type Selected ");
            break;
    }
    afeInfo.iqswap = SL_IQSWAP_DISABLE;
    afeInfo.agcRefValue = 125; //afcRefValue in mV
    outPutInfo.TsoClockInvEnable = SL_TSO_CLK_INV_ON;

    _SAANKHYA_PHY_ANDROID_DEBUG("%s:%d - before SL_ConfigGetBbCapture", __FILE__, __LINE__);

    cres = SL_ConfigGetBbCapture(&getbbValue);
    if (cres != SL_CONFIG_OK)
    {
        _SAANKHYA_PHY_ANDROID_ERROR("ERROR : SL_ConfigGetPlatform Failed");
        _SAANKHYA_PHY_ANDROID_DEBUG("%s:%d - ERROR : SL_ConfigGetPlatform Failed", __FILE__, __LINE__);
        _SAANKHYA_PHY_ANDROID_DEBUG("ERROR : SL_ConfigGetPlatform Failed ");
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

    _SAANKHYA_PHY_ANDROID_DEBUG("SL_DemodCreateInstance: before invocation, slUnit: %d",slUnit);
    slres = SL_DemodCreateInstance(&slUnit);
    if (slres != SL_OK)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodCreateInstance: slres: %d", slres);
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodCreateInstance :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    //jjustman-2020-07-20 - create thread for libusb_handle_events for context callbacks
    //jjustman-2020-07-29 - disable
    //pthread_create(&pThreadID, NULL, (THREADFUNCPTR)&atsc3NdkClientSlImpl::LibUSB_Handle_Events_Callback, (void*)this);

    _SAANKHYA_PHY_ANDROID_DEBUG("Initializing SL Demod..: ");
    _SAANKHYA_PHY_ANDROID_DEBUG("SL_DemodInit: before, slUnit: %d, cmdIf: %d", slUnit, cmdIf);
    slres = SL_DemodInit(slUnit, cmdIf, SL_DEMODSTD_ATSC3_0);
    if (slres != SL_OK)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("SL_DemodInit: failed, slres: %d", slres);
        _SAANKHYA_PHY_ANDROID_DEBUG("FAILED");
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodInit :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }
    else
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("SL_DemodInit: SUCCESS, slres: %d", slres);
    }

    do
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("before SL_DemodGetStatus: slres is: %d", slres);
        slres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_BOOT, (SL_DemodBootStatus_t*)&bootStatus);
        _SAANKHYA_PHY_ANDROID_DEBUG("SL_DemodGetStatus: slres is: %d", slres);
        if (slres != SL_OK)
        {
            _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_Demod Get Boot Status :");
            printToConsoleDemodError(slres);
        }
        SL_SleepMS(1000);
    } while (bootStatus != SL_DEMOD_BOOT_STATUS_COMPLETE);

    _SAANKHYA_PHY_ANDROID_DEBUG("Demod Boot Status  : ");
    if (bootStatus == SL_DEMOD_BOOT_STATUS_INPROGRESS)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("%s", "INPROGRESS");
    }
    else if (bootStatus == SL_DEMOD_BOOT_STATUS_COMPLETE)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("%s", "COMPLETED");
    }
    else if (bootStatus == SL_DEMOD_BOOT_STATUS_ERROR)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("%s", "ERROR");
        goto ERROR;
    }

    slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_AFEIF, &afeInfo);
    if (slres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodConfigure :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    slres = SL_DemodConfigure(slUnit, SL_CONFIG_TYPE_IQ_OFFSET_CORRECTION, &iqOffSetCorrection);
    if (slres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodConfigure :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_OUTPUTIF, &outPutInfo);
    if (slres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodConfigure :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    slres = SL_DemodConfigPlps(slUnit, &plpInfo);
    if (slres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodConfigPlps :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    slres = SL_DemodGetSoftwareVersion(slUnit, &swMajorNo, &swMinorNo);
    if (slres == SL_OK)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Demod SW Version: %d.%d", swMajorNo, swMinorNo);
    }

    /* Tuner Config */
    tunerCfg.bandwidth = SL_TUNER_BW_6MHZ;
    tunerCfg.std = SL_TUNERSTD_ATSC3_0;

    if(tUnit == -1) {
        tres = SL_TunerCreateInstance(&tUnit);
    } else {
#ifndef __SL_TUNER_DEINIT_DISABLED__
        //create a new tuner instance
        tres = SL_TunerCreateInstance(&tUnit);
#else
        tres = SL_TUNER_OK;
#endif
        //otherwise, don't create a new instance as we will leak due to SL_TunerUnInit not decrementing refcount
    }

    if (tres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_TunerCreateInstance :");
        printToConsoleTunerError(tres);
        goto ERROR;
    }

    tres = SL_TunerInit(tUnit);
    if (tres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_TunerInit :");
        printToConsoleTunerError(tres);
        goto ERROR;
    }

    tres = SL_TunerConfigure(tUnit, &tunerCfg);
    if (tres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_TunerConfigure :");
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
            _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_TunerExSetDcOffSet :");
            printToConsoleTunerError(tres);
            if (getPlfConfig.tunerType == TUNER_SI)
            {
                goto ERROR;
            }
        }
    }

    _SAANKHYA_PHY_ANDROID_DEBUG("OPEN COMPLETE!");
    return 0;

ERROR:

    return -1;
}

int SaankhyaPHYAndroid::tune(int freqKHz, int plpid)
{
    int ret = 0;
    unsigned int cFrequency = 0;
    int isRxDataStartedSpinCount = 0;

    //create our CB mutex, but don't acquire it yet
    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex, defer_lock);

    //acquire our lock for setting tuning parameters (including re-tuning)
    unique_lock<mutex> SL_I2C_command_mutex_tuner_tune(SL_I2C_command_mutex);
    atsc3_core_service_application_bridge_reset_context();


    tres = SL_TunerSetFrequency(tUnit, freqKHz*1000);
    if (tres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_TunerSetFrequency :");
        printToConsoleTunerError(tres);
        goto ERROR;
    }

    tres = SL_TunerGetConfiguration(tUnit, &tunerGetCfg);
    if (tres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_TunerGetConfiguration :");
        printToConsoleTunerError(tres);
        goto ERROR;
    } else {
        if (tunerGetCfg.std == SL_TUNERSTD_ATSC3_0)
        {
            _SAANKHYA_PHY_ANDROID_DEBUG("Tuner Config Std: ATSC3.0");
        }
        else
        {
            _SAANKHYA_PHY_ANDROID_DEBUG("Tuner Config Std: NA");
        }
        switch (tunerGetCfg.bandwidth)
        {
            case SL_TUNER_BW_6MHZ:
                _SAANKHYA_PHY_ANDROID_DEBUG("Tuner Config Bandwidth : 6MHz");
                break;

            case SL_TUNER_BW_7MHZ:
                _SAANKHYA_PHY_ANDROID_DEBUG("Tuner Config Bandwidth : 7MHz");
                break;

            case SL_TUNER_BW_8MHZ:
                _SAANKHYA_PHY_ANDROID_DEBUG("Tuner Config Bandwidth : 8MHz");
                break;

            default:
                _SAANKHYA_PHY_ANDROID_DEBUG("Tuner Config Bandwidth : NA");
        }
    }

    tres = SL_TunerGetFrequency(tUnit, &cFrequency);
    if (tres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_TunerGetFrequency :");
        printToConsoleTunerError(tres);
        goto ERROR;
    }
    else
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Tuner Locked Frequency : %dHz", cFrequency);

    }

    tres = SL_TunerGetStatus(tUnit, &tunerInfo);
    if (tres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_TunerGetStatus :");
        printToConsoleTunerError(tres);
        goto ERROR;
    }
    else
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Tuner Lock status  : ");
        _SAANKHYA_PHY_ANDROID_DEBUG((tunerInfo.status == 1) ? "LOCKED" : "NOT LOCKED");
        _SAANKHYA_PHY_ANDROID_DEBUG("Tuner RSSI: %3.2f dBm", tunerInfo.signalStrength);

        _SAANKHYA_PHY_ANDROID_DEBUG("tuner frequency: %d", cFrequency);
    }

    CircularBufferMutex_local.lock();
    //setup shared memory allocs
    if(!cb) {
        cb = CircularBufferCreate(TLV_CIRCULAR_BUFFER_SIZE);
    }
    CircularBufferMutex_local.unlock();

    if (!atsc3_sl_tlv_block) {
        allocate_atsc3_sl_tlv_block();
    }

    //check if we were re-initalized and might have an open threads to wind-down
#ifdef __RESPWAN_THREAD_WORKERS
    if(captureThreadHandle.joinable()) {
        captureThreadShouldRun = false;
        _SAANKHYA_PHY_ANDROID_INFO("::Open() - setting captureThreadShouldRun to false, Waiting for captureThreadHandle to join()");
        captureThreadHandle.join();
    }

    if(processThreadHandle.joinable()) {
        processThreadShouldRun = false;
        _SAANKHYA_PHY_ANDROID_INFO("::Open() - setting processThreadShouldRun to false, Waiting for processThreadHandle to join()");
        processThreadHandle.join();
    }

    if(statusThreadHandle.joinable()) {
        statusThreadShouldRun = false;
        _SAANKHYA_PHY_ANDROID_INFO("::Open() - setting statusThreadShouldRun to false, Waiting for statusThreadHandle to join()");
        statusThreadHandle.join();
    }
#endif

    if(!this->captureThreadIsRunning) {
        captureThreadShouldRun = true;
        captureThreadHandle = std::thread([this]() {
            this->captureThread();
        });

        //micro spinlock
        int threadStartupSpinlockCount = 0;
        while(!this->captureThreadIsRunning && threadStartupSpinlockCount++ < 100) {
            usleep(10000);
        }

        if(threadStartupSpinlockCount > 50) {
            _SAANKHYA_PHY_ANDROID_WARN("::Open() - starting captureThread took %d spins, final state: %d",
                    threadStartupSpinlockCount,
                    this->captureThreadIsRunning);
        }
    }

    if(!this->processThreadIsRunning) {
        processThreadShouldRun = true;
        processThreadHandle = std::thread([this]() {
            this->processThread();
        });

        //micro spinlock
        int threadStartupSpinlockCount = 0;
        while (!this->processThreadIsRunning && threadStartupSpinlockCount++ < 100) {
            usleep(10000);
        }

        if (threadStartupSpinlockCount > 50) {
            _SAANKHYA_PHY_ANDROID_WARN("::Open() - starting processThreadIsRunning took %d spins, final state: %d",
                                       threadStartupSpinlockCount,
                                       this->processThreadIsRunning);
        }
    }

    if(!this->statusThreadIsRunning) {
        statusThreadShouldRun = true;
        statusThreadHandle = std::thread([this]() {
            this->statusThread();
        });

        //micro spinlock
        int threadStartupSpinlockCount = 0;
        while (!this->statusThreadIsRunning && threadStartupSpinlockCount++ < 100) {
            usleep(10000);
        }

        if (threadStartupSpinlockCount > 50) {
            _SAANKHYA_PHY_ANDROID_WARN("::Open() - starting statusThread took %d spins, final state: %d",
                                       threadStartupSpinlockCount,
                                       this->statusThreadIsRunning);
        }
    }


    while (SL_IsRxDataStarted() != 1)
    {
        SL_SleepMS(100);

        if(((isRxDataStartedSpinCount++) % 100) == 0) {
            _SAANKHYA_PHY_ANDROID_WARN("::Open() - waiting for SL_IsRxDataStarted, spinCount: %d", isRxDataStartedSpinCount);
            //jjustman-2020-10-21 - todo: reset demod?
        }
    }
    _SAANKHYA_PHY_ANDROID_DEBUG("Starting SLDemod: ");

    slres = SL_DemodStart(slUnit);

    if (slres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Saankhya Demod Start Failed");
        goto ERROR;
    }
    else
    {
        demodStartStatus = 1;
        _SAANKHYA_PHY_ANDROID_DEBUG("SUCCESS");
        //_SAANKHYA_PHY_ANDROID_DEBUG("SL Demod Output Capture: STARTED : sl-tlv.bin");
    }
    SL_SleepMS(1000); // Delay to accomdate set configurations at SL to take effect

    plpInfo.plp0 = plpid;
    plpInfo.plp1 = 0xFF;
    plpInfo.plp2 = 0xFF;
    plpInfo.plp3 = 0xFF;

    slres = SL_DemodConfigPlps(slUnit, &plpInfo);
    if (slres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodConfigPlps :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    slres = SL_DemodGetConfiguration(slUnit, &cfgInfo);
    if (slres != SL_OK)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodGetConfiguration :");
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

    _SAANKHYA_PHY_ANDROID_DEBUG("calling SL_DemodConfigPLPS with 0: %02x, 1: %02x, 2: %02x, 3: %02x",
            plpInfo.plp0,
            plpInfo.plp1,
            plpInfo.plp2,
            plpInfo.plp3);

    unique_lock<mutex> SL_I2C_command_mutex_config_plps(SL_I2C_command_mutex);
    slres = SL_DemodConfigPlps(slUnit, &plpInfo);
    if (slres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error: SL_DemodConfigPLP: %d", slres);
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodConfigPlps :");
        printToConsoleDemodError(slres);
    }

    SL_I2C_command_mutex_config_plps.unlock();

    return slres;
}

void SaankhyaPHYAndroid::dump_plp_list() {
    slres = SL_DemodGetLlsPlpList(slUnit, &llsPlpInfo);
    if (slres != SL_OK)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodGetLlsPlpList :");
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

        _SAANKHYA_PHY_ANDROID_DEBUG("PLP: %d, plpInfoVal: %d", plpIndx, plpInfoVal);

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

    _SAANKHYA_PHY_ANDROID_DEBUG("SL_I2cPreInit - Before, path: %s, fd: %d", device_path.c_str(), fd);
    i2cres = SL_I2cPreInit();
    _SAANKHYA_PHY_ANDROID_DEBUG("SL_I2cPreInit returned: %d", i2cres);

    if (i2cres != SL_I2C_OK)
    {
        if(i2cres == SL_I2C_AWAITING_REENUMERATION) {
            _SAANKHYA_PHY_ANDROID_DEBUG("INFO:SL_I2cPreInit SL_FX3S_I2C_AWAITING_REENUMERATION");
            //sleep for 2s
            sleep(2);
            return 0;
        } else {
            _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_I2cPreInit failed: %d", i2cres);
            printToConsoleI2cError(i2cres);
        }
    }
    return -1;
}

SL_ConfigResult_t SaankhyaPHYAndroid::configPlatformParams() {

    SL_ConfigResult_t res;
    /*
      * Assign Platform Configuration Parameters. For other ref platforms, replace settings from
      * comments above
      */

//jjustman-2020-09-09 MarkONE specific configuration
#ifdef SL_MARKONE

    sPlfConfig.chipType = SL_CHIP_4000;
    sPlfConfig.chipRev = SL_CHIP_REV_AA;
    sPlfConfig.boardType = SL_EVB_4000; //from venky 2020-09-07 - SL_BORQS_EVT;
    sPlfConfig.tunerType = TUNER_SI;
    sPlfConfig.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
    sPlfConfig.demodOutputIf = SL_DEMOD_OUTPUTIF_SDIO;
    sPlfConfig.demodI2cAddr = 0x30; /* SLDemod 7-bit Physical I2C Address */
     /*
     * Relative Path to SLSDK from working directory
     * Example: D:\UNAME\PROJECTS\slsdk
     * User can just specifying "..", which will point to this directory or can specify full directory path explicitly
     */
    sPlfConfig.slsdkPath = "/data/out"; //from venky 2020-09-07

    sPlfConfig.demodResetGpioPin = 12;   /* 09-10 03:25:56.498     0     0 E SAANKHYA: Reset low GPIO: 12 */
    sPlfConfig.demodI2cAddr3GpioPin = 37;   /* FX3S GPIO 37 connected to Demod I2C Address3 Pin and used only for SDIO Interface */

#endif

//jjustman-2020-09-09 KAILASH dongle specific configuration
#ifdef SL_KAILASH

    #define SL_FX3S 1
    sPlfConfig.chipType = SL_CHIP_3010;
    sPlfConfig.chipRev = SL_CHIP_REV_AA;
    sPlfConfig.boardType = SL_KAILASH_DONGLE;
    sPlfConfig.tunerType = TUNER_SI;
    sPlfConfig.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
    sPlfConfig.demodOutputIf = SL_DEMOD_OUTPUTIF_TS;
    sPlfConfig.demodI2cAddr = 0x30; /* SLDemod 7-bit Physical I2C Address */

    sPlfConfig.demodResetGpioPin = 47;   /* FX3S GPIO 47 connected to Demod Reset Pin */
    sPlfConfig.cpldResetGpioPin = 43;   /* FX3S GPIO 43 connected to CPLD Reset Pin and used only for serial TS Interface  */
    sPlfConfig.demodI2cAddr3GpioPin = 37;   /* FX3S GPIO 37 connected to Demod I2C Address3 Pin and used only for SDIO Interface */
    sPlfConfig.slsdkPath = "."; //jjustman-2020-09-09 use extern object linkages for fx3/hex firmware

#endif



    /* Set Configuration Parameters */
    res = SL_ConfigSetPlatform(sPlfConfig);

    _SAANKHYA_PHY_ANDROID_DEBUG("configPlatformParams: with boardType: %d", sPlfConfig.boardType);

    return res;

}



void SaankhyaPHYAndroid::handleCmdIfFailure(void)
{
    _SAANKHYA_PHY_ANDROID_DEBUG("SL CMD IF FAILURE: Cannot continue!");
    SL_DemodUnInit(slUnit);
    slUnit = -1;

#ifndef __SL_TUNER_DEINIT_DISABLED__
    SL_TunerUnInit(tUnit);
    tUnit = -1;
#endif
}

void SaankhyaPHYAndroid::printToConsoleI2cError(SL_I2cResult_t err)
{
    switch (err)
    {
        case SL_I2C_ERR_TRANSFER_FAILED:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl I2C Transfer Failed");
            break;
        case SL_I2C_ERR_NOT_INITIALIZED:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl I2C Not Initialized");
            break;

        case SL_I2C_ERR_BUS_TIMEOUT:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl I2C Bus Timeout");
            break;

        case SL_I2C_ERR_LOST_ARBITRATION:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl I2C Lost Arbitration");
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
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl Tuner Operation Failed");
            break;

        case SL_TUNER_ERR_INVALID_ARGS:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl Tuner Invalid Argument");
            break;

        case SL_TUNER_ERR_NOT_SUPPORTED:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl Tuner Not Supported");
            break;

        case SL_TUNER_ERR_MAX_INSTANCES_REACHED:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl Tuner Maximum Instance Reached");
            break;
        default:
            break;
    }
}

void SaankhyaPHYAndroid::printToConsolePlfConfiguration(SL_PlatFormConfigParams_t cfgInfo)
{
    _SAANKHYA_PHY_ANDROID_DEBUG("SL Platform Configuration");
    switch (cfgInfo.boardType)
    {
        case SL_EVB_3000:
            _SAANKHYA_PHY_ANDROID_DEBUG("Board Type  : SL_EVB_3000");
            break;

        case SL_EVB_3010:
            _SAANKHYA_PHY_ANDROID_DEBUG("Board Type  : SL_EVB_3010");
            break;

        case SL_EVB_4000:
            _SAANKHYA_PHY_ANDROID_DEBUG("Board Type  : SL_EVB_4000");
            break;

        case SL_KAILASH_DONGLE:
            _SAANKHYA_PHY_ANDROID_DEBUG("Board Type  : SL_KAILASH_DONGLE");
            break;

        case SL_BORQS_EVT:
            _SAANKHYA_PHY_ANDROID_DEBUG("Board Type  : SL_BORQS_EVT");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("Board Type  : NA");
    }

    switch (cfgInfo.chipType)
    {
        case SL_CHIP_3000:
            _SAANKHYA_PHY_ANDROID_DEBUG("Chip Type: SL_CHIP_3000");
            break;

        case SL_CHIP_3010:
            _SAANKHYA_PHY_ANDROID_DEBUG("Chip Type: SL_CHIP_3010");
            break;

        case SL_CHIP_4000:
            _SAANKHYA_PHY_ANDROID_DEBUG("Chip Type: SL_CHIP_4000");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("Chip Type : NA");
    }

    if (cfgInfo.chipRev == SL_CHIP_REV_AA)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Chip Revision: SL_CHIP_REV_AA");
    }
    else if (cfgInfo.chipRev == SL_CHIP_REV_BB)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Chip Revision: SL_CHIP_REV_BB");
    }
    else
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Chip Revision: NA");
    }

    if (cfgInfo.tunerType == TUNER_NXP)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Tuner Type: TUNER_NXP");
    }
    else if (cfgInfo.tunerType == TUNER_SI)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Tuner Type: TUNER_SI");
    }
    else
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Tuner Type: NA");
    }

    switch (cfgInfo.demodControlIf)
    {
        case SL_DEMOD_CMD_CONTROL_IF_I2C:
            _SAANKHYA_PHY_ANDROID_DEBUG("Command Interface: SL_DEMOD_CMD_CONTROL_IF_I2C");
            break;

        case SL_DEMOD_CMD_CONTROL_IF_SDIO:
            _SAANKHYA_PHY_ANDROID_DEBUG("Command Interface: SL_DEMOD_CMD_CONTROL_IF_SDIO");
            break;

        case SL_DEMOD_CMD_CONTROL_IF_SPI:
            _SAANKHYA_PHY_ANDROID_DEBUG("Command Interface: SL_DEMOD_CMD_CONTROL_IF_SPI");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("Command Interface: NA");
    }

    switch (cfgInfo.demodOutputIf)
    {
        case SL_DEMOD_OUTPUTIF_TS:
            _SAANKHYA_PHY_ANDROID_DEBUG("Output Interface: SL_DEMOD_OUTPUTIF_TS");
            break;

        case SL_DEMOD_OUTPUTIF_SDIO:
            _SAANKHYA_PHY_ANDROID_DEBUG("Output Interface: SL_DEMOD_OUTPUTIF_SDIO");
            break;

        case SL_DEMOD_OUTPUTIF_SPI:
            _SAANKHYA_PHY_ANDROID_DEBUG("Output Interface: SL_DEMOD_OUTPUTIF_SPI");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("Output Interface: NA");
    }

    _SAANKHYA_PHY_ANDROID_DEBUG("Demod I2C Address: 0x%x", cfgInfo.demodI2cAddr);
}

void SaankhyaPHYAndroid::printToConsoleDemodConfiguration(SL_DemodConfigInfo_t cfgInfo)
{
    _SAANKHYA_PHY_ANDROID_DEBUG(" SL Demod Configuration");
    switch (cfgInfo.std)
    {
        case SL_DEMODSTD_ATSC3_0:
            _SAANKHYA_PHY_ANDROID_DEBUG("Standard: ATSC3_0");
            break;

        case SL_DEMODSTD_ATSC1_0:
            _SAANKHYA_PHY_ANDROID_DEBUG("Demod Standard  : ATSC1_0");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("Demod Standard  : NA");
    }

    _SAANKHYA_PHY_ANDROID_DEBUG("PLP Configuration");
    _SAANKHYA_PHY_ANDROID_DEBUG("  PLP0: %d", (signed char)cfgInfo.plpInfo.plp0);
    _SAANKHYA_PHY_ANDROID_DEBUG("  PLP1: %d", (signed char)cfgInfo.plpInfo.plp1);
    _SAANKHYA_PHY_ANDROID_DEBUG("  PLP2: %d", (signed char)cfgInfo.plpInfo.plp2);
    _SAANKHYA_PHY_ANDROID_DEBUG("  PLP3: %d", (signed char)cfgInfo.plpInfo.plp3);

    _SAANKHYA_PHY_ANDROID_DEBUG("Input Configuration");
    switch (cfgInfo.afeIfInfo.iftype)
    {
        case SL_IFTYPE_ZIF:
            _SAANKHYA_PHY_ANDROID_DEBUG("  IF Type: ZIF");
            break;

        case SL_IFTYPE_LIF:
            _SAANKHYA_PHY_ANDROID_DEBUG("  IF Type: LIF");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("  IF Type: NA");
    }

    switch (cfgInfo.afeIfInfo.iqswap)
    {
        case SL_IQSWAP_DISABLE:
            _SAANKHYA_PHY_ANDROID_DEBUG("  IQSWAP : DISABLE");
            break;

        case SL_IQSWAP_ENABLE:
            _SAANKHYA_PHY_ANDROID_DEBUG("  IQSWAP : ENABLE");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("  IQSWAP : NA");
    }

    switch (cfgInfo.afeIfInfo.iswap)
    {
        case SL_IPOL_SWAP_DISABLE:
            _SAANKHYA_PHY_ANDROID_DEBUG("  ISWAP  : DISABLE");
            break;

        case SL_IPOL_SWAP_ENABLE:
            _SAANKHYA_PHY_ANDROID_DEBUG("  ISWAP  : ENABLE");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("  ISWAP  : NA");
    }

    switch (cfgInfo.afeIfInfo.qswap)
    {
        case SL_QPOL_SWAP_DISABLE:
            _SAANKHYA_PHY_ANDROID_DEBUG("  QSWAP  : DISABLE");
            break;

        case SL_QPOL_SWAP_ENABLE:
            _SAANKHYA_PHY_ANDROID_DEBUG("  QSWAP  : ENABLE");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("  QSWAP  : NA");
    }

    _SAANKHYA_PHY_ANDROID_DEBUG("  ICoeff1: %3.4f", cfgInfo.iqOffCorInfo.iCoeff1);
    _SAANKHYA_PHY_ANDROID_DEBUG("  QCoeff1: %3.4f", cfgInfo.iqOffCorInfo.qCoeff1);
    _SAANKHYA_PHY_ANDROID_DEBUG("  ICoeff2: %3.4f", cfgInfo.iqOffCorInfo.iCoeff2);
    _SAANKHYA_PHY_ANDROID_DEBUG("  QCoeff2: %3.4f", cfgInfo.iqOffCorInfo.qCoeff2);

    _SAANKHYA_PHY_ANDROID_DEBUG("  AGCReference  : %d mv", cfgInfo.afeIfInfo.agcRefValue);
    _SAANKHYA_PHY_ANDROID_DEBUG("  Tuner IF Frequency: %3.2f MHz", cfgInfo.afeIfInfo.ifreq);

    _SAANKHYA_PHY_ANDROID_DEBUG("Output Configuration");
    switch (cfgInfo.oifInfo.oif)
    {
        case SL_OUTPUTIF_TSPARALLEL_LSB_FIRST:
            _SAANKHYA_PHY_ANDROID_DEBUG("  OutputInteface: TS PARALLEL LSB FIRST");
            break;

        case SL_OUTPUTIF_TSPARALLEL_MSB_FIRST:
            _SAANKHYA_PHY_ANDROID_DEBUG("  OutputInteface: TS PARALLEL MSB FIRST");
            break;

        case SL_OUTPUTIF_TSSERIAL_LSB_FIRST:
            _SAANKHYA_PHY_ANDROID_DEBUG("  OutputInteface: TS SERAIL LSB FIRST");
            break;

        case SL_OUTPUTIF_TSSERIAL_MSB_FIRST:
            _SAANKHYA_PHY_ANDROID_DEBUG("  OutputInteface: TS SERIAL MSB FIRST");
            break;

        case SL_OUTPUTIF_SDIO:
            _SAANKHYA_PHY_ANDROID_DEBUG("  OutputInteface: SDIO");
            break;

        case SL_OUTPUTIF_SPI:
            _SAANKHYA_PHY_ANDROID_DEBUG("  OutputInteface: SPI");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("  OutputInteface: NA");
    }

    switch (cfgInfo.oifInfo.TsoClockInvEnable)
    {
        case SL_TSO_CLK_INV_OFF:
            _SAANKHYA_PHY_ANDROID_DEBUG("  TS Out Clock Inv: DISABLED");
            break;

        case SL_TSO_CLK_INV_ON:
            _SAANKHYA_PHY_ANDROID_DEBUG("  TS Out Clock Inv: ENABLED");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_DEBUG("   TS Out Clock Inv: NA");
    }
}

void SaankhyaPHYAndroid::printToConsoleDemodError(SL_Result_t err)
{
    switch (err)
    {
        case SL_ERR_OPERATION_FAILED:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl Operation Failed");
            break;

        case SL_ERR_TOO_MANY_INSTANCES:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl Too Many Instance");
            break;

        case SL_ERR_CODE_DWNLD:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl Code download Failed");
            break;

        case SL_ERR_INVALID_ARGUMENTS:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl Invalid Argument");
            break;

        case SL_ERR_CMD_IF_FAILURE:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl Command Interface Failure");
            break;

        case SL_ERR_NOT_SUPPORTED:
            _SAANKHYA_PHY_ANDROID_DEBUG(" Sl Not Supported");
            break;
        default:
            break;
    }
}

int SaankhyaPHYAndroid::processThread()
{
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::processThread: starting with this: %p", this);
    this->pinConsumerThreadAsNeeded();
    this->processThreadIsRunning = true;

    this->resetProcessThreadStatistics();

    while (this->processThreadShouldRun)
    {
        //_SAANKHYA_PHY_ANDROID_DEBUG("atsc3NdkClientSlImpl::ProcessThread: getDataSize is: %d", CircularBufferGetDataSize(cb));

        //unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);

        while(CircularBufferGetDataSize(this->cb) >= TLV_CIRCULAR_BUFFER_MIN_PROCESS_SIZE) {
            this->processTLVFromCallback();
        }
       // CircularBufferMutex_local.unlock();

        usleep(33000); //pegs us at ~ 30 spinlocks/sec if no data
    }

    this->releasePinnedConsumerThreadAsNeeded();
    this->processThreadIsRunning = false;

    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::ProcessThread complete");

    return 0;
}

int SaankhyaPHYAndroid::captureThread()
{
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::captureThread: starting with this: %p", this);
    this->pinProducerThreadAsNeeded();
    this->captureThreadIsRunning = true;

    SL_RxDataStart((RxDataCB)&SaankhyaPHYAndroid::RxDataCallback);

    this->releasePinnedProducerThreadAsNeeded();
    this->captureThreadIsRunning = false;

    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::CaptureThread complete");

    return 0;
}

int SaankhyaPHYAndroid::statusThread()
{
    _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::statusThread: starting with this: %p", this);
    this->pinStatusThreadAsNeeded();
    this->statusThreadIsRunning = true;

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
    unique_lock<mutex> SL_I2C_command_mutex_tuner_status_io(this->SL_I2C_command_mutex, std::defer_lock);

    while(this->statusThreadShouldRun) {

        //running
        if(lastCpuStatus == 0xFFFFFFFF) {
            usleep(2000000); //jjustman: target: sleep for 500ms
            //TODO: jjustman-2019-12-05: investigate FX3 firmware and i2c single threaded interrupt handling instead of dma xfer
        } else {
            //halted
            usleep(5000000);
        }
        lastCpuStatus = 0;

        //bail early if we should shutdown
        if(!this->statusThreadShouldRun) {
            break;
        }

        //jjustman-2020-10-14 - try to make this loop as small as possible to not upset the SDIO I/F ALP buffer window

        SL_I2C_command_mutex_tuner_status_io.lock();

        /*jjustman-2020-01-06: For the SL3000/SiTune, we will have 3 status attributes with the following possible values:

                tunerInfo.status:   SL_TUNER_STATUS_NOT_LOCKED (0)
                                    SL_TUNER_STATUS_LOCKED (1)

                demodLockStatus:    SL_DEMOD_STATUS_NOT_LOCK (0)
                                    SL_DEMOD_STATUS_LOCK (1)

                cpuStatus:          (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED",
         */

        //used for PLP demod info
        SL_DemodConfigInfo_t demodInfo;
        sl_res = SL_DemodGetConfiguration(this->slUnit, &demodInfo);
        if(sl_res != SL_OK) {
            _SAANKHYA_PHY_ANDROID_DEBUG("Error calling SL_DemodGetConfiguration, releasing lock and continue, sl_res: %d", sl_res);
            goto sl_i2c_tuner_mutex_unlock;
        }

        //jjustman-2020-10-14 - not really worth it on AA as we don't get rssi here
        tres = SL_TunerGetStatus(this->tUnit, &tunerInfo);
        if (tres != SL_TUNER_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error:SL_TunerGetStatus: tres: %d", tres);
            goto sl_i2c_tuner_mutex_unlock;
        }

        //important
        dres = SL_DemodGetStatus(this->slUnit, SL_DEMOD_STATUS_TYPE_LOCK, (SL_DemodLockStatus_t*)&demodLockStatus);
        if (dres != SL_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error:SL_Demod Get Lock Status  : dres: %d", dres);
            goto sl_i2c_tuner_mutex_unlock;
        }

        //important
        dres = SL_DemodGetStatus(this->slUnit, SL_DEMOD_STATUS_TYPE_CPU, (int*)&cpuStatus);
        if (dres != SL_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error:SL_Demod Get CPU Status: dres: %d", dres);
            continue;
        }

        lastCpuStatus = cpuStatus;

        //we need this for SNR
        dres = SL_DemodGetAtsc3p0Diagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_PERF, (SL_Atsc3p0Perf_Diag_t*)&perfDiag);
        if (dres != SL_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error getting ATSC3.0 Performance Diagnostics : dres: %d", dres);
            goto sl_i2c_tuner_mutex_unlock;
        }

        dres = SL_DemodGetAtsc3p0Diagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_BSR, (SL_Atsc3p0Bsr_Diag_t*)&bsrDiag);
        if (dres != SL_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error getting ATSC3.0 Boot Strap Diagnostics  : dres: %d", dres);
            goto sl_i2c_tuner_mutex_unlock;
        }

        /*
         * jjustman-2020-10-14 - omitting

        dres = SL_DemodGetAtsc3p0Diagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_L1B, (SL_Atsc3p0L1B_Diag_t*)&l1bDiag);
        if (dres != SL_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error getting ATSC3.0 L1B Diagnostics  : dres: %d", dres);
            goto sl_i2c_tuner_mutex_unlock;
        }
        */

        //important for PLP FEC type, and Mod/Cod
        dres = SL_DemodGetAtsc3p0Diagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_L1D, (SL_Atsc3p0L1D_Diag_t*)&l1dDiag);
        if (dres != SL_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error getting ATSC3.0 L1D Diagnostics  : dres: %d", dres);
            goto sl_i2c_tuner_mutex_unlock;
        }

        sl_res = SL_DemodGetLlsPlpList(this->slUnit, &llsPlpInfo);
        if (sl_res != SL_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error:SL_DemodGetLlsPlpList : sl_res: %d", sl_res);
            goto sl_i2c_tuner_mutex_unlock;
        }

        SL_I2C_command_mutex_tuner_status_io.unlock();

        snr = (float)perfDiag.GlobalSnrLinearScale / 16384;
        snr = 10000.0 * log10(snr); //10

        ber_l1b = (float)perfDiag.NumBitErrL1b / perfDiag.NumFecBitsL1b; // //aBerPreLdpcE7,
        ber_l1d = (float)perfDiag.NumBitErrL1d / perfDiag.NumFecBitsL1d;//aBerPreBchE9,
        ber_plp0 = (float)perfDiag.NumBitErrPlp0 / perfDiag.NumFecBitsPlp0; //aFerPostBchE6,

        _SAANKHYA_PHY_ANDROID_DEBUG("atsc3NdkClientSlImpl::StatusThread: SNR: %f, tunerInfo.status: %d, tunerInfo.signalStrength: %f, cpuStatus: %s, demodLockStatus: %d,  ber_l1b: %d, ber_l1d: %d, ber_plp0: %d, plps: 0x%02 (fec: %d, mod: %d, cr: %d), 0x%02 (fec: %d, mod: %d, cr: %d), 0x%02 (fec: %d, mod: %d, cr: %d), 0x%02 (fec: %d, mod: %d, cr: %d)",
                snr / 1000.0,
               tunerInfo.status,
               tunerInfo.signalStrength / 1000,
               (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED",
               demodLockStatus,
               ber_l1b,
               ber_l1d,
               ber_plp0,
               demodInfo.plpInfo.plp0,
               l1dDiag.sfParams[0].PlpParams[0].L1dSfPlpFecType,
               l1dDiag.sfParams[0].PlpParams[0].L1dSfPlpModType,
               l1dDiag.sfParams[0].PlpParams[0].L1dSfPlpCoderate,
               demodInfo.plpInfo.plp1,
               l1dDiag.sfParams[1].PlpParams[1].L1dSfPlpFecType,
               l1dDiag.sfParams[1].PlpParams[1].L1dSfPlpModType,
               l1dDiag.sfParams[1].PlpParams[1].L1dSfPlpCoderate,
               demodInfo.plpInfo.plp2,
               l1dDiag.sfParams[2].PlpParams[2].L1dSfPlpFecType,
               l1dDiag.sfParams[2].PlpParams[2].L1dSfPlpModType,
               l1dDiag.sfParams[2].PlpParams[2].L1dSfPlpCoderate,
               demodInfo.plpInfo.plp3,
               l1dDiag.sfParams[3].PlpParams[3].L1dSfPlpFecType,
               l1dDiag.sfParams[3].PlpParams[3].L1dSfPlpModType,
               l1dDiag.sfParams[3].PlpParams[3].L1dSfPlpCoderate);

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
        //we've already unlocked, so don't fall thru
        continue;

sl_i2c_tuner_mutex_unlock:
        SL_I2C_command_mutex_tuner_status_io.unlock();

    }

    this->releasePinnedStatusThreadAsNeeded();
    this->statusThreadIsRunning = false;
    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::statusThread complete");

    return 0;
}

/*
 * NOTE: invoking spinlock thread MUST have acquired cb lock before invoking processTLVFromCallback
 *     e.g.
 *      unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);
 *      processTLVFromCallback()
 *      CircularBufferMutex_local.unlock();
 */

void SaankhyaPHYAndroid::processTLVFromCallback()
{
    int bytesRead = CircularBufferPop(cb, TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE, (char*)&processDataCircularBufferForCallback);

    unique_lock<mutex> atsc3_sl_tlv_block_mutex_local(atsc3_sl_tlv_block_mutex);

    if(!atsc3_sl_tlv_block) {
        _SAANKHYA_PHY_ANDROID_WARN("ERROR: atsc3NdkClientSlImpl::processTLVFromCallback - atsc3_sl_tlv_block is NULL!");
        allocate_atsc3_sl_tlv_block();
    }

    if(bytesRead) {
        if(bytesRead < TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE) {
            _SAANKHYA_PHY_ANDROID_WARN("ERROR: atsc3NdkClientSlImpl::processTLVFromCallback - short read from CircularBufferPop, got %d bytes, but expected: %d",
                    bytesRead,
                    TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE);
        }
        block_Write(atsc3_sl_tlv_block, (uint8_t*)&processDataCircularBufferForCallback, bytesRead);
        //block_Tail_Truncate(atsc3_sl_tlv_block, block_Remaining_size(atsc3_sl_tlv_block));
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

                            if(atsc3_phy_rx_link_mapping_table_process_callback && atsc3_link_mapping_table_pending) {
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
                    //_SAANKHYA_PHY_ANDROID_DEBUG("alp_payload->alp_payload_complete == FALSE at pos: %d, size: %d", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);
                }
            } else {
                atsc3_sl_tlv_payload_complete = false;
                //jjustman-2019-12-29 - noisy, TODO: wrap in __DEBUG macro check
                //_SAANKHYA_PHY_ANDROID_DEBUG("ERROR: alp_payload IS NULL, short TLV read?  at atsc3_sl_tlv_block: i_pos: %d, p_size: %d", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);
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

            //jjustman-2020-10-30 - TODO: this can be optimized out without a re-alloc
            block_Destroy(&atsc3_sl_tlv_block);
            atsc3_sl_tlv_block = block_Alloc(TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE);
        } else {
            _SAANKHYA_PHY_ANDROID_DEBUG("atsc3_sl_tlv_block: positioning mismatch: i_pos: %d, p_size: %d - rewinding and seeking for magic packet?", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);

            //jjustman: 2019-11-23: rewind in order to try seek for our magic packet in the next loop here
            block_Rewind(atsc3_sl_tlv_block);
        }
    }

    atsc3_sl_tlv_block_mutex_local.unlock();

}

void SaankhyaPHYAndroid::RxDataCallback(unsigned char *data, long len)
{
    //_SAANKHYA_PHY_ANDROID_DEBUG("atsc3NdkClientSlImpl::RxDataCallback: pushing data: %p, len: %d", data, len);
    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);
    if(SaankhyaPHYAndroid::cb) {
        CircularBufferPush(SaankhyaPHYAndroid::cb, (char *) data, len);
    }
    CircularBufferMutex_local.unlock();
}

void SaankhyaPHYAndroid::NotifyPlpSelectionChangeCallback(vector<uint8_t> plps, void *context) {
    ((SaankhyaPHYAndroid *) context)->listen_plps(plps);
}

void SaankhyaPHYAndroid::allocate_atsc3_sl_tlv_block() {
    unique_lock<mutex> atsc3_sl_tlv_block_mutex_local(atsc3_sl_tlv_block_mutex);
    if(!atsc3_sl_tlv_block) {
        atsc3_sl_tlv_block = block_Alloc(TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE);
    }
    atsc3_sl_tlv_block_mutex_local.unlock();
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_init(JNIEnv *env, jobject instance) {
    lock_guard<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    _SAANKHYA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_init: start init, env: %p", env);
    if(saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_init: start init, saankhyaPHYAndroid is present: %p, calling deinit/delete", saankhyaPHYAndroid);
        saankhyaPHYAndroid->deinit();
        saankhyaPHYAndroid = nullptr;
    }

    saankhyaPHYAndroid = new SaankhyaPHYAndroid(env, instance);
    saankhyaPHYAndroid->init();

    _SAANKHYA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_init: return, instance: %p", saankhyaPHYAndroid);

    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_run(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_run: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        res = saankhyaPHYAndroid->run();
        _SAANKHYA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_run: returning res: %d", res);
    }

    return res;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_is_1running(JNIEnv* env, jobject instance)
{
    lock_guard<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    jboolean res = false;

    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_is_1running: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = false;
    } else {
        res = saankhyaPHYAndroid->is_running();
    }

    return res;
}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_stop(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_stop: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        res = saankhyaPHYAndroid->stop();
        _SAANKHYA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_stop: returning res: %d", res);
    }

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_deinit(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_deinit: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {

        saankhyaPHYAndroid->deinit();
        saankhyaPHYAndroid = nullptr;
    }

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_download_1bootloader_1firmware(JNIEnv *env, jobject thiz, jint fd, jstring device_path_jstring) {
    lock_guard<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

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

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_open(JNIEnv *env, jobject thiz, jint fd, jstring device_path_jstring) {
    lock_guard<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

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

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_tune(JNIEnv *env, jobject thiz,
                                                                      jint freq_khz,
                                                                      jint single_plp) {

    lock_guard<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);


    int res = 0;
    if(!saankhyaPHYAndroid) {
        _SAANKHYA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_tune: saankhyaPHYAndroid is NULL!");
        res = -1;
    } else {
        res = saankhyaPHYAndroid->tune(freq_khz, single_plp);
    }

    return res;
}
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_listen_1plps(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jobject plps) {
    lock_guard<mutex> saankhy_phy_android_cctor_mutex_local(SaankhyaPHYAndroid::CS_global_mutex);

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

    return res;
}