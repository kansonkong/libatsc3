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
atomic_bool SaankhyaPHYAndroid::cb_should_discard;

//jjustman-2021-02-04 - global error flag if i2c txn fails, usually due to demod crash
//      TODO: reset SL demod and re-initalize automatically if this error is SL_ERR_CMD_IF_FAILURE

SL_Result_t     SaankhyaPHYAndroid::global_sl_result_error_flag = SL_OK;
SL_I2cResult_t  SaankhyaPHYAndroid::global_sl_i2c_result_error_flag = SL_I2C_OK;

SaankhyaPHYAndroid::SaankhyaPHYAndroid(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = this->env->NewGlobalRef(jni_instance);
    this->setRxUdpPacketProcessCallback(atsc3_core_service_bridge_process_packet_from_plp_and_block);
    this->setRxLinkMappingTableProcessCallback(atsc3_phy_jni_bridge_notify_link_mapping_table);

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_set_callback(&SaankhyaPHYAndroid::NotifyPlpSelectionChangeCallback, this);
    }

    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::SaankhyaPHYAndroid - created with this: %p", this);
    SaankhyaPHYAndroid::cb_should_discard = false;
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
    statusMetricsResetFromTuneChange();
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
    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::stop: enter with this: %p, slUnit: %d, tUnit: %d, captureThreadIsRunning: %d, statusThreadIsRunning: %d, processThreadIsRunning: %d",
                              this,
                              this->slUnit,
                              this->tUnit,
                              this->captureThreadIsRunning,
                              this->statusThreadIsRunning,
                              this->processThreadIsRunning);

    SaankhyaPHYAndroid::cb_should_discard = true;
    statusThreadShouldRun = false;
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

    if(captureThreadIsRunning) {
        _SAANKHYA_PHY_ANDROID_DEBUG("SaankhyaPHYAndroid::stop: setting captureThreadShouldRun: false");
        SL_RxDataStop();
    }
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

SL_ConfigResult_t SaankhyaPHYAndroid::configPlatformParams_autodetect(int fd, string device_path) {

    SL_ConfigResult_t res = SL_CONFIG_OK;
    _SAANKHYA_PHY_ANDROID_DEBUG("configPlatformParams_autodetect:: open with fd: %d, device_path: %s", fd, device_path.c_str());

    if(fd == SL_HOSTINTERFACE_TYPE_MARKONE_FD && device_path.c_str() && !strcasecmp(SL_HOSTINTERFACE_TYPE_MARKONE_PATH, device_path.c_str())) {
        //configure as aa_MarkONE
        res = configPlatformParams_aa_markone();
    } else {
        //configure as (aa/bb) FX3
        res = configPlatformParams_aa_fx3();
        //or
        //configPlatformParams_bb_fx3()
    }

    _SAANKHYA_PHY_ANDROID_DEBUG("configPlatformParams_autodetect::retun res: %d", res);

    return res;
}
int SaankhyaPHYAndroid::open(int fd, string device_path)
{
    _SAANKHYA_PHY_ANDROID_DEBUG("open: with fd: %d, device_path: %s", fd, device_path.c_str());

    SL_I2cResult_t i2cres;

    SL_Result_t slres;
    SL_ConfigResult_t cres;
    SL_TunerResult_t tres;
    SL_UtilsResult_t utilsres;

    SL_ConfigResult_t sl_configResult = SL_CONFIG_OK;
    sl_configResult = configPlatformParams_autodetect(fd, device_path);

    if(sl_configResult != SL_CONFIG_OK) {
        _SAANKHYA_PHY_ANDROID_DEBUG("open: configPlatformParams_autodetect failed, with fd: %d, device_path: %s, configResult failed, res: %d", fd, device_path.c_str(), sl_configResult);
        return -1;
    }

    SL_SetUsbFd(fd);

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
            if(i2cres == SL_I2C_DEV_NODE_NOT_FOUND) {
                //this is most likely markone auto-probing on a non markone handset
                _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::open() - unable to find /dev/i2c markone handle - (e.g /dev/i2c-3)");
            } else {
                global_sl_i2c_result_error_flag = i2cres;

                if(atsc3_ndk_phy_bridge_get_instance()) {
                    atsc3_ndk_phy_bridge_get_instance()->atsc3_notify_phy_error("SaankhyaPHYAndroid::open() - ERROR: SL_I2cInit failed: code: %d", global_sl_i2c_result_error_flag);
                }

                _SAANKHYA_PHY_ANDROID_ERROR("SaankhyaPHYAndroid::open() - ERROR: Error:SL_I2cInit failed:");
                printToConsoleI2cError(i2cres);
            }

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

    //jjustman-2021-01-13 - set our demod as not started
    demodStartStatus = 0;

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

    regionInfo = SL_ATSC3P0_REGION_US;

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
        _SAANKHYA_PHY_ANDROID_DEBUG("SL_DemodInit: SUCCESS, slUnit: %d, slres: %d", slUnit, slres);
    }

    do
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("before SL_DemodGetStatus: slUnit: %d, slres is: %d", slUnit, slres);
        slres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_BOOT, (SL_DemodBootStatus_t*)&bootStatus);
        _SAANKHYA_PHY_ANDROID_DEBUG("SL_DemodGetStatus: slUnit: %d, slres is: %d", slUnit, slres);
        if (slres != SL_OK)
        {
            _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_Demod Get Boot Status :");
            printToConsoleDemodError(slres);
        }
        SL_SleepMS(250);
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

    slres = SL_DemodSetAtsc3p0Region(slUnit, regionInfo);
    if (slres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("\n Error:SL_DemodSetAtsc3p0Region :");
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

    //tell any RXDataCallback or process event that we should discard
    SaankhyaPHYAndroid::cb_should_discard = true;

    //acquire our CB mutex so we don't push stale TLV packets
    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);

    //acquire our lock for setting tuning parameters (including re-tuning)
    unique_lock<mutex> SL_I2C_command_mutex_tuner_tune(SL_I2C_command_mutex);
    unique_lock<mutex> SL_PlpConfigParams_mutex_update_plps(SL_PlpConfigParams_mutex, std::defer_lock);

    atsc3_core_service_application_bridge_reset_context();

    printf("SaankhyaPHYAndroid::tune: Frequency: %d, PLP: %d", freqKHz, plpid);

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

    if (!atsc3_sl_tlv_block) {
        allocate_atsc3_sl_tlv_block();
    }

    //setup shared memory allocs
    if(!cb) {
        cb = CircularBufferCreate(TLV_CIRCULAR_BUFFER_SIZE);
    } else {
        //jjustman-2021-01-19 - clear out our current cb on re-tune
        CircularBufferReset(cb);
        //just in case any last pending SDIO transactions arent completed yet...
        SL_SleepMS(100);
    }

    //jjustman-2021-01-19 - allow for cb to start acumulating TLV frames
    SaankhyaPHYAndroid::cb_should_discard = false;
    CircularBufferMutex_local.unlock();

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

    if(!demodStartStatus) {
        while (SL_IsRxDataStarted() != 1) {
            SL_SleepMS(100);

            if (((isRxDataStartedSpinCount++) % 100) == 0) {
                _SAANKHYA_PHY_ANDROID_WARN("::Open() - waiting for SL_IsRxDataStarted, spinCount: %d", isRxDataStartedSpinCount);
                //jjustman-2020-10-21 - todo: reset demod?
            }
        }
        _SAANKHYA_PHY_ANDROID_DEBUG("Starting SLDemod: ");

        slres = SL_DemodStart(slUnit);

        if (!(slres == SL_OK || slres == SL_ERR_ALREADY_STARTED)) {
            _SAANKHYA_PHY_ANDROID_DEBUG("Saankhya Demod Start Failed");
            demodStartStatus = 0;
            goto ERROR;
        } else {
            demodStartStatus = 1;
            _SAANKHYA_PHY_ANDROID_DEBUG("SUCCESS");
            //_SAANKHYA_PHY_ANDROID_DEBUG("SL Demod Output Capture: STARTED : sl-tlv.bin");
        }
        SL_SleepMS(500); // Delay to accomdate set configurations at SL to take effect for SL_DemodStart()

    } else {
        _SAANKHYA_PHY_ANDROID_DEBUG("SLDemod: already running");
    }

    SL_PlpConfigParams_mutex_update_plps.lock();

    plpInfo.plp0 = plpid;
    plpInfo.plp1 = 0xFF;
    plpInfo.plp2 = 0xFF;
    plpInfo.plp3 = 0xFF;

    slres = SL_DemodConfigPlps(slUnit, &plpInfo);

    SL_PlpConfigParams_mutex_update_plps.unlock();

    if (slres != 0) {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodConfigPlps :");
        printToConsoleDemodError(slres);
        goto ERROR;
    }

    statusMetricsResetFromTuneChange();

    ret = 0;
    goto UNLOCK;

ERROR:
    ret = -1;

    //unlock our i2c mutex
UNLOCK:
    SL_I2C_command_mutex_tuner_tune.unlock();
    return ret;

}

void SaankhyaPHYAndroid::statusMetricsResetFromTuneChange() {
    _SAANKHYA_PHY_ANDROID_INFO("statusMetricsResetFromContextChange - resetting statusThreadFirstLoopAfterTuneComplete");

    statusThreadFirstLoopAfterTuneComplete = true; //will dump DemodGetconfiguration from statusThread

    statusMetricsResetFromPLPListenChange(); //clear our diag flags and metrics types also...
}

void SaankhyaPHYAndroid::statusMetricsResetFromPLPListenChange() {
    _SAANKHYA_PHY_ANDROID_INFO("statusMetricsResetFromPLPListenChange - resetting statusThreadFirstLoop_*Lock flags and clearing TunerSignalInfo/_Diag's");

    statusThreadFirstLoopAfterTuneComplete_HasBootstrapLock_for_BSR_Diag = false;
    statusThreadFirstLoopAfterTuneComplete_HasL1B_DemodLock_for_L1B_Diag = false;
    statusThreadFirstLoopAfterTuneComplete_HasL1D_DemodLock_for_L1D_Diag = false;

    demodLockStatus = 0;
    cpuStatus = 0;

    //hack for re-initializing our status structs/diag after a tune()
    memset(&tunerInfo,  0, sizeof(SL_TunerSignalInfo_t));
    memset(&perfDiag,   0, sizeof(SL_Atsc3p0Perf_Diag_t));
    memset(&bsrDiag,    0, sizeof(SL_Atsc3p0Bsr_Diag_t));
    memset(&l1bDiag,    0, sizeof(SL_Atsc3p0L1B_Diag_t));
    memset(&l1dDiag,    0, sizeof(SL_Atsc3p0L1D_Diag_t));
}

int SaankhyaPHYAndroid::listen_plps(vector<uint8_t> plps_original_list)
{
    vector<uint8_t> plps;
    for(int i=0; i < plps_original_list.size(); i++) {
        if(plps_original_list.at(i) == 0) {
            //skip, duplicate plp0 will cause demod to fail
        } else {
            bool duplicate = false;
            for(int j=0; j < plps.size(); j++) {
                 if(plps.at(j) == plps_original_list.at(i)) {
                     duplicate = true;
                 }
            }
            if(!duplicate) {
                plps.push_back(plps_original_list.at(i));
            }
        }
    }

    unique_lock<mutex> SL_PlpConfigParams_mutex_update_plps(SL_PlpConfigParams_mutex);

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

    unique_lock<mutex> SL_I2C_command_mutex_demod_configure_plps(SL_I2C_command_mutex);

    slres = SL_DemodConfigPlps(slUnit, &plpInfo);
    SL_I2C_command_mutex_demod_configure_plps.unlock();
    SL_PlpConfigParams_mutex_update_plps.unlock();

    if (slres != 0)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error: SL_DemodConfigPLP: %d", slres);
        printToConsoleDemodError(slres);
    }

    statusMetricsResetFromPLPListenChange();

    return slres;
}

void SaankhyaPHYAndroid::dump_plp_list() {
    unique_lock<mutex> SL_PlpConfigParams_mutex_update_plps(SL_PlpConfigParams_mutex);
    unique_lock<mutex> SL_I2C_command_mutex_demod_configure_plps(SL_I2C_command_mutex);

    slres = SL_DemodGetLlsPlpList(slUnit, &llsPlpInfo);
    if (slres != SL_OK)
    {
        _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_DemodGetLlsPlpList :");
        printToConsoleDemodError(slres);
        if (slres == SL_ERR_CMD_IF_FAILURE)
        {
            handleCmdIfFailure();
            goto ERROR;
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

ERROR:
    _SAANKHYA_PHY_ANDROID_ERROR("Error:dump_plp_list failed!");

UNLOCK:
    SL_I2C_command_mutex_demod_configure_plps.unlock();
    SL_PlpConfigParams_mutex_update_plps.unlock();
}

int SaankhyaPHYAndroid::download_bootloader_firmware(int fd, string device_path) {
    _SAANKHYA_PHY_ANDROID_DEBUG("download_bootloader_firmware, path: %s, fd: %d", device_path.c_str(), fd);

    SL_ConfigResult_t sl_configResult = SL_CONFIG_OK;
    sl_configResult = configPlatformParams_autodetect(fd, device_path);

    if(sl_configResult != SL_CONFIG_OK) {
        _SAANKHYA_PHY_ANDROID_DEBUG("download_bootloader_firmware: configPlatformParams_autodetect failed - fd: %d, device_path: %s, configResult failed, res: %d", fd, device_path.c_str(), sl_configResult);
        return -1;
    }

    SL_SetUsbFd(fd);
    SL_I2cResult_t i2cres;

    i2cres = SL_I2cPreInit();
    _SAANKHYA_PHY_ANDROID_DEBUG("download_bootloader_firmware: SL_I2cPreInit returned: %d", i2cres);

    if (i2cres != SL_I2C_OK)
    {
        if(i2cres == SL_I2C_AWAITING_REENUMERATION) {
            _SAANKHYA_PHY_ANDROID_DEBUG("download_bootloader_firmware: INFO:SL_I2cPreInit SL_FX3S_I2C_AWAITING_REENUMERATION");
            //sleep for 3s?
            //sleep(3);
            return 0;
        } else {
            _SAANKHYA_PHY_ANDROID_DEBUG("Error:SL_I2cPreInit failed: %d", i2cres);
            printToConsoleI2cError(i2cres);
        }
    }
    return -1;
}

//jjustman-2020-09-09 KAILASH dongle specific configuration
SL_ConfigResult_t SaankhyaPHYAndroid::configPlatformParams_aa_fx3() {

    SL_ConfigResult_t res = SL_CONFIG_OK;

    sPlfConfig.chipType = SL_CHIP_3010;
    sPlfConfig.chipRev = SL_CHIP_REV_AA;
    sPlfConfig.boardType = SL_KAILASH_DONGLE;
    sPlfConfig.tunerType = TUNER_SI;
    sPlfConfig.hostInterfaceType = SL_HostInterfaceType_FX3;

    sPlfConfig.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
    sPlfConfig.demodOutputIf = SL_DEMOD_OUTPUTIF_TS;
    sPlfConfig.demodI2cAddr = 0x30; /* SLDemod 7-bit Physical I2C Address */

    sPlfConfig.demodResetGpioPin = 47;      /* FX3S GPIO 47 connected to Demod Reset Pin */
    sPlfConfig.cpldResetGpioPin = 43;       /* FX3S GPIO 43 connected to CPLD Reset Pin and used only for serial TS Interface  */
    sPlfConfig.tunerResetGpioPin = 23;    /* FX3S GPIO 23 connected to Tuner Reset Pin */

    sPlfConfig.slsdkPath = "."; //jjustman-2020-09-09 use extern object linkages for fx3/hex firmware

    /* Set Configuration Parameters */
    res = SL_ConfigSetPlatform(sPlfConfig);

    _SAANKHYA_PHY_ANDROID_DEBUG("configPlatformParams_aa_fx3: with chipType: %d, chipRev: %d, boardType: %d, tunerType: %d, hostInterfaceType: %d, ",
                                sPlfConfig.chipType,
                                sPlfConfig.chipRev,
                                sPlfConfig.boardType,
                                sPlfConfig.tunerType,
                                sPlfConfig.hostInterfaceType);


    //reconfigure method callbacks for cust_markone
    SL_ConfigureGpio_slref();
    SL_ConfigureI2c_slref();
    SL_ConfigureTs_slref();

    return res;
}

SL_ConfigResult_t SaankhyaPHYAndroid::configPlatformParams_aa_markone() {

    SL_ConfigResult_t res = SL_CONFIG_OK;

    sPlfConfig.chipType = SL_CHIP_4000;
    sPlfConfig.chipRev = SL_CHIP_REV_AA;
    sPlfConfig.boardType = SL_EVB_4000; //from venky 2020-09-07 - SL_BORQS_EVT;
    sPlfConfig.tunerType = TUNER_SI;
    sPlfConfig.hostInterfaceType = SL_HostInterfaceType_MarkONE;

    sPlfConfig.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
    sPlfConfig.demodOutputIf = SL_DEMOD_OUTPUTIF_SDIO;
    sPlfConfig.demodI2cAddr = 0x30; /* SLDemod 7-bit Physical I2C Address */

    sPlfConfig.slsdkPath = "."; //jjustman-2020-09-09 use extern object linkages for fx3/hex firmware

    sPlfConfig.demodResetGpioPin = 12;   /* 09-10 03:25:56.498     0     0 E SAANKHYA: Reset low GPIO: 12 */
    sPlfConfig.demodI2cAddr3GpioPin = 0;   /* not used on markone AA ? */

    //from: sdm660-qrd-kf.dtsi
    sPlfConfig.tunerResetGpioPin = 13;

    /* Set Configuration Parameters */
    res = SL_ConfigSetPlatform(sPlfConfig);

    _SAANKHYA_PHY_ANDROID_DEBUG("configPlatformParams_aa_markone: with chipType: %d, chipRev: %d, boardType: %d, tunerType: %d, hostInterfaceType: %d, ",
            sPlfConfig.chipType,
            sPlfConfig.chipRev,
            sPlfConfig.boardType,
            sPlfConfig.tunerType,
            sPlfConfig.hostInterfaceType);

    //reconfigure method callbacks for cust_markone
    SL_ConfigureGpio_markone();
    SL_ConfigureI2c_markone();
    SL_ConfigureTs_markone();

    return res;
}


//jjustman-2020-09-09 KAILASH dongle specific configuration
SL_ConfigResult_t SaankhyaPHYAndroid::configPlatformParams_bb_fx3() {

    SL_ConfigResult_t res = SL_CONFIG_ERR_NOT_SUPPORTED;
//
//    sPlfConfig.chipType = SL_CHIP_3010;
//    sPlfConfig.chipRev = SL_CHIP_REV_BB;
//    sPlfConfig.boardType = SL_KAILASH_DONGLE_2;
//    sPlfConfig.tunerType = TUNER_SI;
//    sPlfConfig.hostInterfaceType = SL_HostInterfaceType_FX3;
//
//    sPlfConfig.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
//    sPlfConfig.demodOutputIf = SL_DEMOD_OUTPUTIF_TS;
//    sPlfConfig.demodI2cAddr = 0x30; /* SLDemod 7-bit Physical I2C Address */
//
//    sPlfConfig.demodResetGpioPin = 47;      /* FX3S GPIO 47 connected to Demod Reset Pin */
//    sPlfConfig.cpldResetGpioPin = 43;       /* FX3S GPIO 43 connected to CPLD Reset Pin and used only for serial TS Interface  */
//    sPlfConfig.tunerResetGpioPin = 23;    /* FX3S GPIO 23 connected to Tuner Reset Pin */
//
//    sPlfConfig.slsdkPath = "."; //jjustman-2020-09-09 use extern object linkages for fx3/hex firmware
//
//    /* Set Configuration Parameters */
//    res = SL_ConfigSetPlatform(sPlfConfig);
//
//    _SAANKHYA_PHY_ANDROID_DEBUG("configPlatformParams_bb_fx3: with chipType: %d, chipRev: %d, boardType: %d, tunerType: %d, hostInterfaceType: %d, ",
//                                sPlfConfig.chipType,
//                                sPlfConfig.chipRev,
//                                sPlfConfig.boardType,
//                                sPlfConfig.tunerType,
//                                sPlfConfig.hostInterfaceType);
//   //reconfigure method callbacks for cust_markone
//    SL_ConfigureGpio_slref();
//    SL_ConfigureI2c_slref();
//    SL_ConfigureTS_slref();

    return res;
}



void SaankhyaPHYAndroid::handleCmdIfFailure(void)
{
    if(slCmdIfFailureCount < 30) {

        _SAANKHYA_PHY_ANDROID_DEBUG("SL CMD IF FAILURE: cmdIfFailureCount: %d, Cannot continue - leaving demod init for now...", ++slCmdIfFailureCount);
    } else {
        _SAANKHYA_PHY_ANDROID_DEBUG("SL CMD IF FAILURE: cmdIfFailureCount: %d, TODO: reboot demod", ++slCmdIfFailureCount);
//jjustman-2021-02-24 - TODO - reboot demod
//        SL_DemodUnInit(slUnit);
//        slUnit = -1;
    }

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
            _SAANKHYA_PHY_ANDROID_WARN(" Sl I2C Transfer Failed");
            break;
        case SL_I2C_ERR_NOT_INITIALIZED:
            _SAANKHYA_PHY_ANDROID_WARN(" Sl I2C Not Initialized");
            break;

        case SL_I2C_ERR_BUS_TIMEOUT:
            _SAANKHYA_PHY_ANDROID_WARN(" Sl I2C Bus Timeout");
            break;

        case SL_I2C_ERR_LOST_ARBITRATION:
            _SAANKHYA_PHY_ANDROID_WARN(" Sl I2C Lost Arbitration");
            break;

        default:
            _SAANKHYA_PHY_ANDROID_WARN(" Sl I2C other error: %d", err);

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
            processTLVFromCallbackInvocationCount++;
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

    unique_lock<mutex> SL_I2C_command_mutex_tuner_status_io(this->SL_I2C_command_mutex, std::defer_lock);
    unique_lock<mutex> SL_PlpConfigParams_mutex_get_plps(SL_PlpConfigParams_mutex, std::defer_lock);

    SL_Result_t dres;
    SL_Result_t sl_res;
    SL_TunerResult_t tres;

    uint lastCpuStatus = 0;

    SL_PlpConfigParams_t  loop_plpInfo = { 0 };
    unsigned long long    llsPlpInfo;

    double snr;
    double ber_l1b;
    double ber_l1d;
    double ber_plp0;

    SL_Atsc3p0L1DPlp_Diag_t myPlps[4];

    atsc3_ndk_phy_client_rf_metrics_t atsc3_ndk_phy_client_rf_metrics = { '0' };

    //wait for demod to come up before polling status
    while(!this->demodStartStatus && this->statusThreadShouldRun) {
        usleep(1000000);
    }

    while(this->statusThreadShouldRun) {

        //running
        if(lastCpuStatus == 0xFFFFFFFF) {
            usleep(2000000);
            //jjustman: target: sleep for 500ms
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

        //PLP info we will use for this stats iteration
        SL_PlpConfigParams_mutex_get_plps.lock();
        loop_plpInfo.plp0 = plpInfo.plp0;
        loop_plpInfo.plp1 = plpInfo.plp1;
        loop_plpInfo.plp2 = plpInfo.plp2;
        loop_plpInfo.plp3 = plpInfo.plp3;
        SL_PlpConfigParams_mutex_get_plps.unlock();

        //if this is our first loop after a Tune() command has completed, dump SL_DemodGetConfiguration
        if(statusThreadFirstLoopAfterTuneComplete) {
            SL_SleepMS(250); // Delay to accomdate set configurations at SL to take effect
            statusThreadFirstLoopAfterTuneComplete = false;

            slres = SL_DemodGetConfiguration(slUnit, &cfgInfo);
            if (slres != SL_OK)
            {
                _SAANKHYA_PHY_ANDROID_ERROR("Error:SL_TunerGetStatus: tres: %d", tres);
                printToConsoleDemodError(slres);
                if (slres == SL_ERR_CMD_IF_FAILURE)
                {
                    handleCmdIfFailure();
                    goto sl_i2c_tuner_mutex_unlock;
                }
            }
            else
            {
                printToConsoleDemodConfiguration(cfgInfo);
            }
        }

        /*jjustman-2020-01-06: For the SL3000/SiTune, we will have 3 status attributes with the following possible values:
                cpuStatus:          (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED",

                tunerInfo.status:   SL_TUNER_STATUS_NOT_LOCKED (0)
                                    SL_TUNER_STATUS_LOCKED (1)

                demodLockStatus:   Updated as of SLAPI-0.14:

                        This data type represents the signal lock status of the SL demodulator.

                        Bit Number  Value   Description         Details
                        ----------  -----   -----------         ---------------
                        0           0       RF UnLocked         -
                                    1       RF Locked           RF LOCKED: Bootstrap Information decoded and available

                        1           0       L1B UnLocked        -
                                    1       L1B Locked          L1B LOCKED: L1B information available

                        2           0       L1D UnLocked        -
                                    1       L1D Locked          L1D LOCKED: L1D related information available

                        3           Reserved

                        4           0       BB PLP0 Not Locked  -
                                    1       BB PLP0 Locked      BB PLP0 Locked: PLP0 ALP Data coming out of SLDemod

                        5           0       BB PLP1 Not Locked  -
                                    1       BB PLP1 Locked      BB PLP1 Locked: PLP1 ALP Data coming out of SLDemod

                        6           0       BB PLP2 Not Locked  -
                                    1       BB PLP2 Locked      BB PLP2 Locked: PLP2 ALP Data coming out of SLDemod

                        7           0       BB PLP3 Not Locked  -
                                    1       BB PLP3 Locked      BB PLP3 Locked: PLP3 ALP Data coming out of SLDemod

                        8-31        Reserved

        */

        dres = SL_DemodGetStatus(this->slUnit, SL_DEMOD_STATUS_TYPE_CPU, (int*)&cpuStatus);
        if (dres != SL_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error:SL_Demod Get CPU Status: dres: %d", dres);
            continue;
        }
        lastCpuStatus = cpuStatus;

        //jjustman-2020-10-14 - not really worth it on AA as we don't get rssi here
        tres = SL_TunerGetStatus(this->tUnit, &tunerInfo);
        if (tres != SL_TUNER_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error:SL_TunerGetStatus: tres: %d", tres);
            goto sl_i2c_tuner_mutex_unlock;
        }

        atsc3_ndk_phy_client_rf_metrics.tuner_lock = (tunerInfo.status == 1);

        //important, we should only query BSR, L1B, and L1D Diag data after each relevant lock has been acquired to prevent i2c bus txns from crashing the demod...
        dres = SL_DemodGetStatus(this->slUnit, SL_DEMOD_STATUS_TYPE_LOCK, (SL_DemodLockStatus_t*)&demodLockStatus);
        if (dres != SL_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error:SL_Demod Get Lock Status  : dres: %d", dres);
            goto sl_i2c_tuner_mutex_unlock;
        }

        atsc3_ndk_phy_client_rf_metrics.demod_lock = demodLockStatus;

        atsc3_ndk_phy_client_rf_metrics.plp_lock_any = (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP0_LOCK) ||
                                                        (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP1_LOCK) ||
                                                        (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP2_LOCK) ||
                                                        (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP3_LOCK);


        atsc3_ndk_phy_client_rf_metrics.plp_lock_all = (loop_plpInfo.plp0 != 0xFF && (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP0_LOCK)) &&
                                                        (loop_plpInfo.plp1 != 0xFF && (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP1_LOCK)) &&
                                                        (loop_plpInfo.plp2 != 0xFF && (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP2_LOCK)) &&
                                                        (loop_plpInfo.plp3 != 0xFF && (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP3_LOCK));

        //we have RF / Bootstrap lock
        if(demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_RF_LOCK) {
            if(!statusThreadFirstLoopAfterTuneComplete_HasBootstrapLock_for_BSR_Diag) {
                statusThreadFirstLoopAfterTuneComplete_HasBootstrapLock_for_BSR_Diag = true;

                dres = SL_DemodGetAtsc3p0Diagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_BSR, (SL_Atsc3p0Bsr_Diag_t*)&bsrDiag);
                if (dres != SL_OK) {
                    _SAANKHYA_PHY_ANDROID_ERROR("Error: SL_DemodGetAtsc3p0Diagnostics with SL_DEMOD_DIAG_TYPE_BSR failed, res: %d", dres);
                    goto sl_i2c_tuner_mutex_unlock;
                }

                printAtsc3BsrDiagnostics(bsrDiag, 0);
            }
        }

        //we have L1B_Lock
        if(demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_L1B_LOCK) {

            if(!statusThreadFirstLoopAfterTuneComplete_HasL1B_DemodLock_for_L1B_Diag) {
                statusThreadFirstLoopAfterTuneComplete_HasL1B_DemodLock_for_L1B_Diag = true;

                dres = SL_DemodGetAtsc3p0Diagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_L1B, (SL_Atsc3p0L1B_Diag_t*)&l1bDiag);
                if (dres != SL_OK) {
                    _SAANKHYA_PHY_ANDROID_ERROR("Error: SL_DemodGetAtsc3p0Diagnostics with SL_DEMOD_DIAG_TYPE_L1B failed, res: %d", dres);
                    goto sl_i2c_tuner_mutex_unlock;
                }

                printAtsc3L1bDiagnostics(l1bDiag, 0);
            }
        }

        //we have L1D_Lock
        if(demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_L1D_LOCK) {
            if(!statusThreadFirstLoopAfterTuneComplete_HasL1D_DemodLock_for_L1D_Diag) {
                statusThreadFirstLoopAfterTuneComplete_HasL1D_DemodLock_for_L1D_Diag = true;

                dres = SL_DemodGetAtsc3p0Diagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_L1D, (SL_Atsc3p0L1D_Diag_t*)&l1dDiag);
                if (dres != SL_OK) {
                    _SAANKHYA_PHY_ANDROID_ERROR("Error: SL_DemodGetAtsc3p0Diagnostics with SL_DEMOD_DIAG_TYPE_L1D failed, res: %d", dres);
                    goto sl_i2c_tuner_mutex_unlock;
                }

                printAtsc3L1dDiagnostics(l1bDiag.L1bNoOfSubframes, l1dDiag, 0);
                printAtsc3SignalDetails(l1bDiag.L1bNoOfSubframes, l1dDiag, 0);
            }
        }

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

        sl_res = SL_DemodGetLlsPlpList(this->slUnit, &llsPlpInfo);
        if (sl_res != SL_OK) {
            _SAANKHYA_PHY_ANDROID_ERROR("Error:SL_DemodGetLlsPlpList : sl_res: %d", sl_res);
            goto sl_i2c_tuner_mutex_unlock;
        }


        SL_I2C_command_mutex_tuner_status_io.unlock();

        snr = (float)perfDiag.GlobalSnrLinearScale / 16384;
        snr = 10000.0 * log10(snr); //10

        //hacks...
        atsc3_ndk_phy_client_rf_metrics.rssi = tunerInfo.signalStrength;
        atsc3_ndk_phy_client_rf_metrics.rfLevel1000 = tunerInfo.signalStrength/1000;
        atsc3_ndk_phy_client_rf_metrics.snr1000 = snr;
        ///

        ber_l1b = (float)perfDiag.NumBitErrL1b / perfDiag.NumFecBitsL1b; // //aBerPreLdpcE7,
        ber_l1d = (float)perfDiag.NumBitErrL1d / perfDiag.NumFecBitsL1d;//aBerPreBchE9,
        ber_plp0 = (float)perfDiag.NumBitErrPlp0 / perfDiag.NumFecBitsPlp0; //aFerPostBchE6,

        //build our listen plp details

        memset(&myPlps[0], 0, sizeof(SL_Atsc3p0L1DPlp_Diag_t));
        memset(&myPlps[1], 0, sizeof(SL_Atsc3p0L1DPlp_Diag_t));
        memset(&myPlps[2], 0, sizeof(SL_Atsc3p0L1DPlp_Diag_t));
        memset(&myPlps[3], 0, sizeof(SL_Atsc3p0L1DPlp_Diag_t));

//L1dSfNumPlp2Decode
        for(int subframeIdx = 0; subframeIdx <= l1bDiag.L1bNoOfSubframes; subframeIdx++) {
            for(int plpIdx = 0; plpIdx < (0xFF & l1dDiag.sfParams[subframeIdx].L1dSfNumPlp2Decode); plpIdx++) {

                if(loop_plpInfo.plp0 == l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx].L1dSfPlpId) {
                    myPlps[0] = l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx];
                } else if(loop_plpInfo.plp1 == l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx].L1dSfPlpId) {
                    myPlps[1] = l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx];
                } else if(loop_plpInfo.plp2 == l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx].L1dSfPlpId) {
                    myPlps[2] = l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx];
                } else if(loop_plpInfo.plp3 == l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx].L1dSfPlpId) {
                    myPlps[3] = l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx];
                }
            }
        }

        //plp[0]
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].plp_id         = loop_plpInfo.plp0;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].modcod_valid   = (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP0_LOCK) != 0;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].plp_fec_type   = myPlps[0].L1dSfPlpFecType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].plp_mod        = myPlps[0].L1dSfPlpModType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].plp_cod        = myPlps[0].L1dSfPlpCoderate;

        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].ber_pre_ldpc   = perfDiag.LdpcItrnsPlp0; // over ???//BER x1e7
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].ber_pre_bch    = (perfDiag.NumBitErrPlp0 * 1000000000) / (perfDiag.Plp0StreamByteCount * 8); //s_fe_detail.aBerPreBchE9[i]; //BER 1xe9
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].fer_post_bch   = (perfDiag.NumFrameErrPlp0 * 1000000) / perfDiag.NumFecFramePlp0;  //FER 1xe6

        //plp[1]
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].plp_id         = loop_plpInfo.plp1;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].modcod_valid   = (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP1_LOCK) != 0;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].plp_fec_type   = myPlps[1].L1dSfPlpFecType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].plp_mod        = myPlps[1].L1dSfPlpModType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].plp_cod        = myPlps[1].L1dSfPlpCoderate;

        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].ber_pre_ldpc   = perfDiag.LdpcItrnsPlp1; // over ???//BER x1e7
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].ber_pre_bch    = (perfDiag.NumBitErrPlp1 * 1000000000) / (perfDiag.Plp1StreamByteCount * 8); //s_fe_detail.aBerPreBchE9[i]; //BER 1xe9
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].fer_post_bch   = (perfDiag.NumFrameErrPlp1 * 1000000) / perfDiag.NumFecFramePlp1;  //FER 1xe6

        //plp[2]
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].plp_id         = loop_plpInfo.plp2;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].modcod_valid   = (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP2_LOCK) != 0;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].plp_fec_type   = myPlps[2].L1dSfPlpFecType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].plp_mod        = myPlps[2].L1dSfPlpModType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].plp_cod        = myPlps[2].L1dSfPlpCoderate;

        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].ber_pre_ldpc   = perfDiag.LdpcItrnsPlp2; // over ???//BER x1e7
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].ber_pre_bch    = (perfDiag.NumBitErrPlp2 * 1000000000) / (perfDiag.Plp2StreamByteCount * 8); //s_fe_detail.aBerPreBchE9[i]; //BER 1xe9
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].fer_post_bch   = (perfDiag.NumFrameErrPlp2 * 1000000) / perfDiag.NumFecFramePlp2;  //FER 1xe6

        //plp[3]
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].plp_id         = loop_plpInfo.plp3;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].modcod_valid   = (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLP3_LOCK) != 0;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].plp_fec_type   = myPlps[3].L1dSfPlpFecType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].plp_mod        = myPlps[3].L1dSfPlpModType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].plp_cod        = myPlps[3].L1dSfPlpCoderate;

        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].ber_pre_ldpc   = perfDiag.LdpcItrnsPlp3; // over ???//BER x1e7
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].ber_pre_bch    = (perfDiag.NumBitErrPlp3 * 1000000000) / (perfDiag.Plp3StreamByteCount * 8); //s_fe_detail.aBerPreBchE9[i]; //BER 1xe9
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].fer_post_bch   = (perfDiag.NumFrameErrPlp3 * 1000000) / perfDiag.NumFecFramePlp3;  //FER 1xe6


        _SAANKHYA_PHY_ANDROID_DEBUG("atsc3NdkClientSlImpl::StatusThread: SNR: %f, tunerInfo.status: %d, tunerInfo.signalStrength: %f, cpuStatus: %s, demodLockStatus: %d,  ber_l1b: %d, ber_l1d: %d, ber_plp0: %d, plps: 0x%02x (fec: %d, mod: %d, cr: %d), 0x%02x (fec: %d, mod: %d, cr: %d), 0x%02x (fec: %d, mod: %d, cr: %d), 0x%02x (fec: %d, mod: %d, cr: %d)",
                snr / 1000.0,
               tunerInfo.status,
               tunerInfo.signalStrength / 1000,
               (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED",
               demodLockStatus,
               ber_l1b,
               ber_l1d,
               ber_plp0,
               loop_plpInfo.plp0,
                myPlps[0].L1dSfPlpFecType,
                myPlps[0].L1dSfPlpModType,
                myPlps[0].L1dSfPlpCoderate,
               loop_plpInfo.plp1,
                myPlps[1].L1dSfPlpFecType,
                myPlps[1].L1dSfPlpModType,
                myPlps[1].L1dSfPlpCoderate,
               loop_plpInfo.plp2,
                myPlps[2].L1dSfPlpFecType,
                myPlps[2].L1dSfPlpModType,
                myPlps[2].L1dSfPlpCoderate,
                loop_plpInfo.plp3,
                myPlps[3].L1dSfPlpFecType,
                myPlps[3].L1dSfPlpModType,
                myPlps[3].L1dSfPlpCoderate);

        if(atsc3_ndk_phy_bridge_get_instance()) {
            atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_stats_from_atsc3_ndk_phy_client_rf_metrics_t(&atsc3_ndk_phy_client_rf_metrics);
            atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_bw_stats(saankhyaPHYAndroid->alp_completed_packets_parsed,
                                                                          saankhyaPHYAndroid->alp_total_bytes,
                                                                          saankhyaPHYAndroid->alp_total_LMTs_recv);
         }

        //we've already unlocked, so don't fall thru
        continue;

sl_i2c_tuner_mutex_unlock:
        SL_I2C_command_mutex_tuner_status_io.unlock();

        if(global_sl_result_error_flag != SL_OK || global_sl_i2c_result_error_flag != SL_I2C_OK) {
            if(atsc3_ndk_phy_bridge_get_instance()) {
                atsc3_ndk_phy_bridge_get_instance()->atsc3_notify_phy_error("SaankhyaPHYAndroid::tunerStatusThread() - ERROR: command failed, error code: sl_res error code: %d, sl_i2c_res: %d", global_sl_result_error_flag, global_sl_i2c_result_error_flag);
            }
        }

    }

    this->releasePinnedStatusThreadAsNeeded();
    this->statusThreadIsRunning = false;
    _SAANKHYA_PHY_ANDROID_INFO("SaankhyaPHYAndroid::statusThread complete");

    return 0;
}

/*
 * NOTE: jjustman-2021-01-19 - moved critical section mutex from outer wrapper to only CircularBufferPop critical section
 *          moved CS from processTLVFromCallback which was acquired for all of the TLV processing, which is not necessary
 *
 */

void SaankhyaPHYAndroid::processTLVFromCallback()
{
    int innerLoopCount = 0;
    int bytesRead = 0;

    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex, std::defer_lock);
    unique_lock<mutex> atsc3_sl_tlv_block_mutex_local(atsc3_sl_tlv_block_mutex, std::defer_lock);

    //jjustman-2020-12-07 - loop while we have data in our cb, mutex lock to
    while(true) {
        CircularBufferMutex_local.lock();
        bytesRead = CircularBufferPop(cb, TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE, (char*)&processDataCircularBufferForCallback);
        CircularBufferMutex_local.unlock();

        //jjustman-2021-01-19 - if we don't get any data back, or the cb_should_discard flag is set, bail
        if(!bytesRead || SaankhyaPHYAndroid::cb_should_discard) {
            return;
        }

        atsc3_sl_tlv_block_mutex_local.lock();
        if (!atsc3_sl_tlv_block) {
            _SAANKHYA_PHY_ANDROID_WARN("ERROR: atsc3NdkClientSlImpl::processTLVFromCallback - atsc3_sl_tlv_block is NULL!");
            allocate_atsc3_sl_tlv_block();
        }

        if (bytesRead) {
            block_Write(atsc3_sl_tlv_block, (uint8_t *) &processDataCircularBufferForCallback, bytesRead);
            if (bytesRead < TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE) {
                //_SAANKHYA_PHY_ANDROID_TRACE("atsc3NdkClientSlImpl::processTLVFromCallback() - short read from CircularBufferPop, got %d bytes, but expected: %d", bytesRead, TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE);
                break;
            }

            block_Rewind(atsc3_sl_tlv_block);

            //_SAANKHYA_PHY_ANDROID_DEBUG("atsc3NdkClientSlImpl::processTLVFromCallback() - processTLVFromCallbackInvocationCount: %d, inner loop count: %d, atsc3_sl_tlv_block.p_size: %d, atsc3_sl_tlv_block.i_pos: %d", processTLVFromCallbackInvocationCount, ++innerLoopCount, atsc3_sl_tlv_block->p_size, atsc3_sl_tlv_block->i_pos);

            bool atsc3_sl_tlv_payload_complete = false;

            do {
                atsc3_sl_tlv_payload = atsc3_sl_tlv_payload_parse_from_block_t(atsc3_sl_tlv_block);

                if (atsc3_sl_tlv_payload) {
                    atsc3_sl_tlv_payload_dump(atsc3_sl_tlv_payload);
                    if (atsc3_sl_tlv_payload->alp_payload_complete) {
                        atsc3_sl_tlv_payload_complete = true;

                        block_Rewind(atsc3_sl_tlv_payload->alp_payload);
                        atsc3_alp_packet_t *atsc3_alp_packet = atsc3_alp_packet_parse(atsc3_sl_tlv_payload->plp_number, atsc3_sl_tlv_payload->alp_payload);
                        if (atsc3_alp_packet) {
                            alp_completed_packets_parsed++;

                            alp_total_bytes += atsc3_alp_packet->alp_payload->p_size;

                            if (atsc3_alp_packet->alp_packet_header.packet_type == 0x00) {

                                block_Rewind(atsc3_alp_packet->alp_payload);
                                if (atsc3_phy_rx_udp_packet_process_callback) {
                                    atsc3_phy_rx_udp_packet_process_callback(atsc3_sl_tlv_payload->plp_number, atsc3_alp_packet->alp_payload);
                                }

                            } else if (atsc3_alp_packet->alp_packet_header.packet_type == 0x4) {
                                alp_total_LMTs_recv++;
                                atsc3_link_mapping_table_t *atsc3_link_mapping_table_pending = atsc3_alp_packet_extract_lmt(atsc3_alp_packet);

                                if (atsc3_phy_rx_link_mapping_table_process_callback && atsc3_link_mapping_table_pending) {
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

            } while (atsc3_sl_tlv_payload_complete);

            if (atsc3_sl_tlv_payload && !atsc3_sl_tlv_payload->alp_payload_complete && atsc3_sl_tlv_block->i_pos > atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size) {
                //accumulate from our last starting point and continue iterating during next bbp
                atsc3_sl_tlv_block->i_pos -= atsc3_sl_tlv_payload->sl_tlv_total_parsed_payload_size;
                //free our atsc3_sl_tlv_payload
                atsc3_sl_tlv_payload_free(&atsc3_sl_tlv_payload);
            }

            if (atsc3_sl_tlv_block->p_size > atsc3_sl_tlv_block->i_pos) {
                //truncate our current block_t starting at i_pos, and then read next i/o block
                block_t *temp_sl_tlv_block = block_Duplicate_from_position(atsc3_sl_tlv_block);
                block_Destroy(&atsc3_sl_tlv_block);
                atsc3_sl_tlv_block = temp_sl_tlv_block;
                block_Seek(atsc3_sl_tlv_block, atsc3_sl_tlv_block->p_size);
            } else if (atsc3_sl_tlv_block->p_size == atsc3_sl_tlv_block->i_pos) {
                //clear out our tlv block as we are the "exact" size of our last alp packet

                //jjustman-2020-10-30 - TODO: this can be optimized out without a re-alloc
                block_Destroy(&atsc3_sl_tlv_block);
                atsc3_sl_tlv_block = block_Alloc(TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE);
            } else {
                _SAANKHYA_PHY_ANDROID_WARN("atsc3_sl_tlv_block: positioning mismatch: i_pos: %d, p_size: %d - rewinding and seeking for magic packet?", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);

                //jjustman: 2019-11-23: rewind in order to try seek for our magic packet in the next loop here
                block_Rewind(atsc3_sl_tlv_block);
            }
        }

        atsc3_sl_tlv_block_mutex_local.unlock();
    }
}

void SaankhyaPHYAndroid::RxDataCallback(unsigned char *data, long len)
{
    if(SaankhyaPHYAndroid::cb_should_discard) {
        return;
    }

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