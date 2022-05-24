//
// Created by Jason Justman on 5/24/22.
//

#include "TolkaPHYAndroid.h"


//jjustman-2022-05-24 - todo: use jni ref resolution for this ptr
TolkaPHYAndroid* tolkaPHYAndroid = nullptr;
CircularBuffer TolkaPHYAndroid::cb = nullptr;
libusb_device_handle* TolkaPHYAndroid::Libusb_device_handle = nullptr;

Endeavour TolkaPHYAndroid::Endeavour_s;
mutex TolkaPHYAndroid::CircularBufferMutex;

mutex TolkaPHYAndroid::CS_global_mutex;
atomic_bool TolkaPHYAndroid::cb_should_discard;

int _TOLKA_PHY_ANDROID_DEBUG_ENABLED = 1;
int _TOLKA_PHY_ANDROID_TRACE_ENABLED = 0;


//tolka methods

TolkaPHYAndroid::TolkaPHYAndroid(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = this->env->NewGlobalRef(jni_instance);
    this->setRxUdpPacketProcessCallback(atsc3_core_service_bridge_process_packet_from_plp_and_block);
    this->setRxLinkMappingTableProcessCallback(atsc3_phy_jni_bridge_notify_link_mapping_table);

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_set_callback(&TolkaPHYAndroid::NotifyPlpSelectionChangeCallback, this);
    }

    _TOLKA_PHY_ANDROID_INFO("TolkaPHYAndroid::TolkaPHYAndroid - created with this: %p", this);
    TolkaPHYAndroid::cb_should_discard = false;

    memset(&Endeavour_s, 0, sizeof(Endeavour_s));
    memset(&SL3000_R855_driver, 0, sizeof(SL3000_R855_instance_t) * 4);
}

TolkaPHYAndroid::~TolkaPHYAndroid() {

    _TOLKA_PHY_ANDROID_INFO("TolkaPHYAndroid::~TolkaPHYAndroid - enter: deleting with this: %p", this);

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

    _TOLKA_PHY_ANDROID_INFO("TolkaPHYAndroid::~TolkaPHYAndroid - exit: deleting with this: %p", this);
}

//todo: finish actual impls


int  TolkaPHYAndroid::init() {
    //jj: todo
    return -1;
}
int  TolkaPHYAndroid::run() {
    //jj: todo
    return -1;
};
bool TolkaPHYAndroid::is_running() {
    //jj: todo
    return false;
};

int TolkaPHYAndroid::stop() {
    //jj: todo
    return -1;
};

int TolkaPHYAndroid::deinit() {
    //jj: todo
    return -1;
};

string TolkaPHYAndroid::get_sdk_version()  {
    //jj: todo
    return "jj-tolka-sdk_version-0.00001";
};
string TolkaPHYAndroid::get_firmware_version() {
    //jj: todo
    return "jj-tolka-todo-firmware-version";
};

int TolkaPHYAndroid::download_bootloader_firmware(int fd, int device_type, string devicePath) {
    //jj: todo
    return -1;
};

int TolkaPHYAndroid::open(int fd, int device_type, string devicePath)   {
    int r = -1;

    libusb_fd = fd;

    r = libusb_init(NULL);
    if(r) {
        _TOLKA_PHY_ANDROID_ERROR("open - libusb_init returned: %d", r);
        return -1;
    }

    int ret = libusb_wrap_sys_device(NULL, libusb_fd, &Libusb_device_handle);
    if(ret || !Libusb_device_handle) {
        _TOLKA_PHY_ANDROID_ERROR("open: libusb_wrap_sys_device: failed device from fd: %d, ret is: %d, device_handle: %p", fd, ret, Libusb_device_handle);
        return -1;
    }

    //confirm vid/pid
    struct libusb_device_descriptor d;

    libusb_device *tdev = libusb_get_device(Libusb_device_handle);
    ret = libusb_get_device_descriptor(tdev, &d);
    if(ret) {
        _TOLKA_PHY_ANDROID_ERROR("open: libusb_get_device_descriptor: failed device from fd: %d, ret is: %d, device_handle: %p", fd, ret, Libusb_device_handle);
        return -1;
    }
    _TOLKA_PHY_ANDROID_INFO("open: libusb_get_device, handle: %d, vendorId: %d, productId: %d", Libusb_device_handle, d.idVendor, d.idProduct);

    libusb_open(tdev, &Libusb_device_handle);
    if(ret) {
        _TOLKA_PHY_ANDROID_ERROR("open: libusb_open: failed device from fd: %d, ret is: %d, device_handle: %p", fd, ret, Libusb_device_handle);
        return -1;
    }

    libusb_config_descriptor *configDesc = NULL;
    //claim rx/tx/ts_bulk endpoints
    ret = libusb_get_config_descriptor(tdev, 0, &configDesc);
    if (ret != 0)
    {
        _TOLKA_PHY_ANDROID_ERROR("open: cyusb_get_config_descriptor failed, ret: %d", ret);
        return -1;
    }
    _TOLKA_PHY_ANDROID_INFO("total interfaces: %d", configDesc->bNumInterfaces);

    for(int i=0; i < configDesc->bNumInterfaces; i++) {
        _TOLKA_PHY_ANDROID_INFO("endpoint: %d, claiming", i);
        ret = libusb_claim_interface(Libusb_device_handle, i);
        if(ret) {
            _TOLKA_PHY_ANDROID_ERROR("open: libusb_claim_interface: failed to claim ep: %d", i);
            return -1;
        }

        //claim endpoints
        int if_numsettings = configDesc->interface[i].num_altsetting;
        for (int j = 0; j < if_numsettings; j++) {
            libusb_interface_descriptor* interfaceDesc = (libusb_interface_descriptor *)&(configDesc->interface[i].altsetting[j]);

            // Step 4.b.1: Check if the desired endpoint is present.
            for (int k = 0; k < interfaceDesc->bNumEndpoints; k++)
            {
                libusb_endpoint_descriptor* endpointDesc = (libusb_endpoint_descriptor *)&(interfaceDesc->endpoint[k]);
                if (k == 0) { //hack?endpointDesc->bEndpointAddress == TOLKA_USB_ENDPOINT_RX)
                 //   ret = libusb_set_interface_alt_setting(Libusb_device_handle, i, j);
                    TOLKA_USB_ENDPOINT_RX = endpointDesc->bEndpointAddress;
                    _TOLKA_PHY_ANDROID_INFO("libusb_set_interface_alt_setting, TOLKA_USB_ENDPOINT_RX, ret: %d, interface: %d, endpoint: %d (0x%02x)", ret, i, TOLKA_USB_ENDPOINT_RX, TOLKA_USB_ENDPOINT_RX);
                } else if (k == 1) { //hack endpointDesc->bEndpointAddress == TOLKA_USB_ENDPOINT_TX) {
                //    ret = libusb_set_interface_alt_setting(Libusb_device_handle, i, j);
                    TOLKA_USB_ENDPOINT_TX = endpointDesc->bEndpointAddress;

                    _TOLKA_PHY_ANDROID_INFO("libusb_set_interface_alt_setting, TOLKA_USB_ENDPOINT_TX, ret: %d, interface: %d, endpoint: %d (0x%02x)", ret, i, TOLKA_USB_ENDPOINT_TX, TOLKA_USB_ENDPOINT_TX);
                } else if (k == 2) { //hack ->bEndpointAddress == TOLKA_USB_ENDPOINT_RX_TS) {
                    TOLKA_USB_ENDPOINT_RX_TS = endpointDesc->bEndpointAddress;
                //    ret = libusb_set_interface_alt_setting(Libusb_device_handle, i, j);
                    _TOLKA_PHY_ANDROID_INFO("libusb_set_interface_alt_setting, TOLKA_USB_ENDPOINT_RX_TS, ret: %d, interface: %d, endpoint: %d (0x%02x)", ret, i, TOLKA_USB_ENDPOINT_RX_TS, TOLKA_USB_ENDPOINT_RX_TS);
                } else {
                    _TOLKA_PHY_ANDROID_INFO("unknown interface: %d, endpoint: %d (0x%02x)", i, endpointDesc->bEndpointAddress, endpointDesc->bEndpointAddress);
                }
            }
        }
    }

    /*
     *
     * ep addrs should be:
     *
epRead:   2022-05-24 05:40:25.897 27647-27647/com.example.endeavour_SL3000_R855.debug V/Endeavour_s.Bus: EP(0),
        addr = 0x81, attr = 2, dir = 128, num = 1, intval = 0, maxSize =512
epWrite:  2022-05-24 05:41:07.680 27647-27647/com.example.endeavour_SL3000_R855.debug V/Endeavour_s.Bus: EP(1),
        addr = 0x2, attr = 2, dir = 0, num = 2, intval = 0, maxSize =512
epReadTs: 2022-05-24 05:41:44.177 27647-27647/com.example.endeavour_SL3000_R855.debug V/Endeavour_s.Bus: EP(2),
        addr = 0x84, attr = 2, dir = 128, num = 4, intval = 0, maxSize =512
     */

    _TOLKA_PHY_ANDROID_INFO("discovered endpoints are: TOLKA_USB_ENDPOINT_RX: %d (0x%02x), TOLKA_USB_ENDPOINT_TX: %d (0x%02x), TOLKA_USB_ENDPOINT_RX_TS: %d (0x%02x)",
                            TOLKA_USB_ENDPOINT_RX,
                            TOLKA_USB_ENDPOINT_RX,
                            TOLKA_USB_ENDPOINT_TX,
                            TOLKA_USB_ENDPOINT_TX,
                            TOLKA_USB_ENDPOINT_RX_TS,
                            TOLKA_USB_ENDPOINT_RX_TS);


    BrUser_createCriticalSection();

    //Init Endeavour_s
    Endeavour_s.ctrlBus = BUS_USB;
    Endeavour_s.maxBusTxSize = 63;
    Endeavour_s.bypassScript = True;
    Endeavour_s.bypassBoot = False;
    Endeavour_s.chipCount = 1;
    Endeavour_s.gator[0].existed = True;
    Endeavour_s.gator[0].outDataType = OUT_DATA_USB_DATAGRAM;
    Endeavour_s.gator[0].outTsPktLen = PKT_LEN_188;

    //jjustman-2022-05-19: reg_ts0_tag_len - 0 -> 188
    Endeavour_s.tsSource[0][0].existed = True;
    Endeavour_s.tsSource[0][0].tsType  = TS_SERIAL;
    Endeavour_s.tsSource[0][0].tsPort  = TS_PORT_0;
    Endeavour_s.tsSource[0][0].tsPktLen = PKT_LEN_188;
    Endeavour_s.tsSource[0][0].syncByte = 0x47;

    Endeavour_s.gator[0].booted = False;
    Endeavour_s.gator[0].initialized = False;

    long error = 0;
    uint8_t tmp0[1];
    uint8_t tmp1[1];
    long data_cnt;

    IT9300_readRegister(&Endeavour_s, 0, 0xDA98 , tmp0);
    IT9300_readRegister(&Endeavour_s, 0, 0xDA99 , tmp1);
    data_cnt = 4*((tmp1[0] * 256) + tmp0[0]);

    error = IT9300_getFirmwareVersion (&Endeavour_s, 0);
    if (error) {
        _TOLKA_PHY_ANDROID_ERROR("open: IT9300_getFirmwareVersion fail! \n");
        return -1;
    }

    _TOLKA_PHY_ANDROID_INFO("open: IT9300_getFirmwareVersion is: %d", Endeavour_s.firmwareVersion);



    if (Endeavour_s.firmwareVersion != 0) {
        //Switch to TS mode for cleaning PSB buffer
        printf("\n--- Clean PSB buffer --- \n");
        error = IT9300_writeRegister(&Endeavour_s, 0, p_br_mp2if_mpeg_ser_mode, 1);
        if (error)
            return -1;

        // Reset Rx and Read USB for no data issue
        printf("--- RESET IT9300 --- \n");
        error = IT9300_writeRegister(&Endeavour_s, 0, p_br_reg_top_gpioh2_en, 0x01);
        if (error)
            return -1;
        error = IT9300_writeRegister(&Endeavour_s, 0, p_br_reg_top_gpioh2_on, 0x01);
        if (error)
            return -1;
        error = IT9300_writeRegister(&Endeavour_s, 0, p_br_reg_top_gpioh2_o, 0x0);
        if (error)
            return -1;

        usleep(50 * 1000);
    }

    error = IT9300_initialize(&Endeavour_s, 0);
    if(error)
    {
        _TOLKA_PHY_ANDROID_ERROR("IT9300 initialize failed, error = 0x%lx", error);
        return -1;
    }

    int num_of_tuner = 1;
    for (int i = 0; i < num_of_tuner; i++) {
        if (Endeavour_s.tsSource[0][i].existed == True) {
            error = IT9300_enableTsPort(&Endeavour_s, 0, i);
            if (error) {
                _TOLKA_PHY_ANDROID_ERROR("IT9300 IT9300_enableTsPort, port: %d, error = 0x%lx", i, error);
            }
        }

        //jjustman-2022-05-19
        //set this register by hand, as the impl assigns the value of 1 - error = IT9300_setSyncByteMode(&Endeavour_s, 0, 0);
        //instead of using a struct member for the proper value extraction in the default impl

        error = IT9300_writeRegister (&Endeavour_s, 0, p_br_reg_ts0_aggre_mode + i, 0);//0:tag 1:sync  2:remap
        if(error) {
            _TOLKA_PHY_ANDROID_ERROR("IT9300 set ignore fail pin failed, error = 0x%lx", error);
            return -1;
        }

        //ts0_tei_modify?
        error = IT9300_writeRegister (&Endeavour_s, 0, p_br_mp2if_ts0_tei_modify + i, 0);//0:tag 1:sync  2:remap
        if(error) {
            _TOLKA_PHY_ANDROID_ERROR("IT9300 set ignore fail pin failed, error = 0x%lx", error);
            return -1;
        }

        //                (Endeavour_s->tsSource[chip][tsSrcIdx]).tsPktLen);
        error = IT9300_setInTsPktLen(&Endeavour_s, 0, i);
        if(error) {
            _TOLKA_PHY_ANDROID_ERROR("IT9300 set IT9300_setInTsPktLen failed, error = 0x%lx", error);
            return -1;
        }

//how to handle ts 0x47 sync byte wrangling...?
        //IT9300_setSyncByteMode
//        error = IT9300_setSyncByteMode(&Endeavour_s, 0, i);
//        if(error)
//        {
//            printf("IT9300 set IT9300_setSyncByteMode failed, error = 0x%lx\n", error);
//            goto exit;
//        }

#if 0
        Endeavour_s.tsSource[0][0].syncByte = 0x40;
            Endeavour_s.tsSource[0][1].syncByte = 0x41;
            Endeavour_s.tsSource[0][2].syncByte = 0x42;
            Endeavour_s.tsSource[0][3].syncByte = 0x43;
            error = IT9300_setSyncByteMode(&Endeavour_s, 0, 0);
            if (error) goto exit;
            error = IT9300_setSyncByteMode(&Endeavour_s, 0, 1);
            if (error) goto exit;
            error = IT9300_setSyncByteMode(&Endeavour_s, 0, 2);
            if (error) goto exit;
            error = IT9300_setSyncByteMode(&Endeavour_s, 0, 3);
            if (error) goto exit;
            error = IT9300_enableTsPort(&Endeavour_s, 0, 0);
            if (error) goto exit;
            error = IT9300_enableTsPort(&Endeavour_s, 0, 1);
            if (error) goto exit;
            error = IT9300_enableTsPort(&Endeavour_s, 0, 2);
            if (error) goto exit;
            error = IT9300_enableTsPort(&Endeavour_s, 0, 3);
            if (error) goto exit;
#endif

        //Ignore TS fail pin
        error = IT9300_writeRegister(&Endeavour_s, 0, p_br_reg_ts_fail_ignore, 0x1F); //necessary
        if(error) {
            _TOLKA_PHY_ANDROID_ERROR("IT9300 set ignore fail pin failed, error = 0x%lx", error);
            return -1;
        }

        error = IT9300_getFirmwareVersion (&Endeavour_s, 0);
        if (error) {
            _TOLKA_PHY_ANDROID_ERROR("open: IT9300_getFirmwareVersion fail! \n");
            return -1;
        }

        _TOLKA_PHY_ANDROID_INFO("open(): IT9300 initialize ok: firmware post IT9300_initialize is: %d", Endeavour_s.firmwareVersion);
    }



    R855_ErrCode R855_result = R855_Success;
    SL_Result_t sl3000_result = SL_OK;

    // Init R855
    printf("R855 initialize. \n");
    SL3000_R855_driver[0].R855_Info.R855_Standard = R855_ATSC_IF_5M;
    R855_result = Init_R855(&Endeavour_s, &SL3000_R855_driver[0].R855_Info);
    if(R855_result != R855_Success) {
        printf("R855 Init error = %d\n", R855_result);
    } else {
        printf("R855 Init succeeful = %d\n", R855_result);
    }
    error = R855_result;

    // Init SL3000
    printf("SL3000 initialize. \n");
    if(true) {
        printf("Init TVStandard = %d\n", SL_TUNERSTD_ATSC3_0);
        SL3000_R855_driver[0].tunerCfg.std = SL_TUNERSTD_ATSC3_0;
        sl3000_result = SL3000_atsc3_init(&Endeavour_s, &SL3000_R855_driver[0].tunerCfg, &SL3000_R855_driver[0].PlfConfig, SL_DEMODSTD_ATSC3_0);
    }
        //    } else if(tvStandard == ATSC1) {
//        printf("Init TVStandard = %d\n", tvStandard);
//        SL3000_R855_driver[0].tunerCfg.std = SL_TUNERSTD_ATSC1_0;
//        sl3000_result = SL3000_atsc1_init(&Endeavour_s, &SL3000_R855_driver[0].tunerCfg, &SL3000_R855_driver[0].PlfConfig, SL_DEMODSTD_ATSC1_0);
//    } else {
//        printf("SL3000 Unknown tvStandard = %d\n", tvStandard);
//        error = SL_ERR_INVALID_ARGUMENTS;
//        goto exit;
//    }

    if(sl3000_result != SL_OK)
        printf("SL3000 Init error = %d\n", sl3000_result);
    else
        printf("SL3000 Init succeeful = %d\n", sl3000_result);
    error = sl3000_result;

    exit:
    if(error)
        printf("IT9300 error = 0x%lx\n", error);


    return 0;
};
//helper functions for brUser.cpp for buxTx/Rx
/*
 *
	public static final long Error_NO_ERROR 					=	0x00000000;
	public static final long Error_USB_DEVICE_NOT_FOUND			=	0x07000001;
	public static final long Error_USB_PID_VID_WRONG			=	0x07000002;
	public static final long Error_USB_WRITE_FAIL				=	0x07000003;
	public static final long Error_USB_READ_FAIL				=	0x07000004;
	public static final long Error_USB_OPEN_FAIL				=	0x07000005;
	public static final long Error_USB_OPEN_UNKNOWN_MODE		=	0x07000006;
 */
#define TOLKA_USB_NO_ERROR      0x00000000
#define TOLKA_USB_WRITE_FAIL    0x07000003
#define TOLKA_USB_READ_FAIL     0x07000004

//static in c binding..
jlong busTx(Dword bufferLength, Byte* buffer) {
    int retVal = TOLKA_USB_NO_ERROR;
    int transferred = -1;

    if (TolkaPHYAndroid::Libusb_device_handle != NULL)
    {
        retVal = libusb_bulk_transfer(TolkaPHYAndroid::Libusb_device_handle, TOLKA_USB_ENDPOINT_TX, buffer, bufferLength, &transferred, 500);
        if (retVal == 0)
        {
            return TOLKA_USB_NO_ERROR;
        }
        else
        {
            _TOLKA_PHY_ANDROID_ERROR("busTx write failed, ep: %d, retVal: %d, transferred: %d", TOLKA_USB_ENDPOINT_TX, retVal, transferred);
            return TOLKA_USB_WRITE_FAIL;
        }
    }
}
//static
jlong busRx(Dword bufferLength, Byte* buffer) {
    int retVal = TOLKA_USB_NO_ERROR;
    int transferred = -1;

    if (TolkaPHYAndroid::Libusb_device_handle != NULL)
    {
        retVal = libusb_bulk_transfer(TolkaPHYAndroid::Libusb_device_handle, TOLKA_USB_ENDPOINT_RX, buffer, bufferLength, &transferred, 500);
        if (retVal == 0)
        {
            return TOLKA_USB_NO_ERROR;
        }
        else
        {
            _TOLKA_PHY_ANDROID_ERROR("buxRx read failed, ep: %d, retVal: %d, transferred: %d", TOLKA_USB_ENDPOINT_TX, retVal, transferred);
            return TOLKA_USB_READ_FAIL;
        }
    }
}

//jjustman-2022-05-24 - hack
#define TUNER_R855 31337

SL_Result_t TolkaPHYAndroid::SL3000_atsc3_init(Endeavour  *Endeavour_s, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *sPlfConfig, SL_DemodStd_t std)
{
    SL_I2cResult_t i2cres;
    SL_Result_t slres;
    SL_ConfigResult_t cres;
    SL_TunerResult_t tres;
    SL_UtilsResult_t utilsres;
    SL_TunerConfig_t tunerCfg = *pTunerCfg;
    SL_TunerConfig_t tunerGetCfg;
    SL_TunerSignalInfo_t tunerInfo;
    int swMajorNo, swMinorNo;
    unsigned int cFrequency = 0;
    SL_AfeIfConfigParams_t afeInfo;
    SL_OutIfConfigParams_t outPutInfo;
    SL_ExtLnaConfigParams_t       lnaInfo;
    SL_IQOffsetCorrectionParams_t iqOffSetCorrection;
    SL_DemodBootStatus_t bootStatus;


    uint8_t i2c_bus = 3;

    sPlfConfig->chipType = SL_CHIP_3000;
    sPlfConfig->chipRev = SL_CHIP_REV_BB;
    sPlfConfig->boardType = SL_EVB_3000;//SL_KAILASH_DONGLE_3;// SL_EVB_3000;
    sPlfConfig->tunerType = (SL_TunerType_t)TUNER_R855;//TUNER_NXP; hack
    sPlfConfig->demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
    sPlfConfig->demodOutputIf = SL_DEMOD_OUTPUTIF_TS;
    sPlfConfig->demodResetGpioPin = 137;         //jjustman-2022-05-24 - hack for sl_demodgpioreset impl

    sPlfConfig->demodI2cAddr = SL_DEMOD_I2C_ADDR; /* SLDemod 7-bit Physical I2C Address */

    //sPlfConfig.slsdkPath = "..";
    sPlfConfig->slsdkPath = "";

    //int						slUnit;

    /* Tuner Config */
    pTunerCfg->bandwidth = SL_TUNER_BW_6MHZ;

    sPlfConfig->dispConf = SL_DispatcherConfig_tolka;

    /* Set Configuration Parameters */
    SL_ConfigSetPlatform(*sPlfConfig);

    cres = SL_ConfigGetPlatform(sPlfConfig);
    if (cres == SL_CONFIG_OK)
    {
        printToConsolePlfConfiguration(*sPlfConfig);
    }
    else
    {
        SL_Printf("\n ERROR : SL_ConfigGetPlatform Failed ");
    }

    //jjustman-2022-05-24 - noop hack instead of using sl_i2c dispatcher cust impl wireup
//    i2cres = SL_I2cInit(Endeavour_s, i2c_bus);
//    if (i2cres != SL_I2C_OK)
//    {
//        SL_Printf("\n Error:SL_I2cInit failed :");
//        //printToConsoleI2cError(i2cres);
//        //goto TEST_ERROR;
//    }
//    else
//    {
//
//   }
    /* Demod Config */
    cmdIf = SL_CMD_CONTROL_IF_I2C;

    afeInfo.spectrum = SL_SPECTRUM_INVERTED;
    afeInfo.iftype = SL_IFTYPE_LIF;
    afeInfo.ifreq = 5 + TOLKA_R855_ATSC3_IF_OFFSET;
    outPutInfo.oif = SL_OUTPUTIF_TSSERIAL_MSB_FIRST;

    afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
    afeInfo.qswap = SL_QPOL_SWAP_DISABLE;
    iqOffSetCorrection.iCoeff1 = 1.0;
    iqOffSetCorrection.qCoeff1 = 1.0;
    iqOffSetCorrection.iCoeff2 = 0.0;
    iqOffSetCorrection.qCoeff2 = 0.0;
    lnaInfo.lnaMode = SL_EXT_LNA_CFG_MODE_NOT_PRESENT;

    afeInfo.iqswap = SL_IQSWAP_DISABLE;
    afeInfo.agcRefValue = 125; //afcRefValue in mV
    outPutInfo.TsoClockInvEnable = SL_TSO_CLK_INV_ON;



    atsc3ConfigInfo.plpConfig.plp0 = 0x0;
    atsc3ConfigInfo.plpConfig.plp1 = 0xFF;
    atsc3ConfigInfo.plpConfig.plp2 = 0xFF;
    atsc3ConfigInfo.plpConfig.plp3 = 0xFF;
    atsc3ConfigInfo.region = SL_ATSC3P0_REGION_US;
    SL_Printf("\n atsc3ConfigInfo.plpConfig.plp0=%d\n", atsc3ConfigInfo.plpConfig.plp0);

    slres = SL_DemodCreateInstance(&slUnit);
    if (slres != SL_OK)
    {
        _TOLKA_PHY_ANDROID_ERROR("Error:SL_DemodCreateInstance: res: %d", slres);
    }

    SL_Printf("\n Initializing SL Demod..: ");



    slres = SL_DemodInit(slUnit, cmdIf, SL_DEMODSTD_ATSC3_0);
    if (slres != SL_OK)
    {
        SL_Printf("FAILED");
        _TOLKA_PHY_ANDROID_ERROR("\n Error:SL_DemodInit: res: %d", slres);
    } else {
        SL_Printf("SUCCESS");
    }

    do
    {
        slres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_BOOT, (SL_DemodBootStatus_t*)&bootStatus);
        if (slres != SL_OK)
        {
            _TOLKA_PHY_ANDROID_ERROR("Error:SL_Demod Get Boot Status: res: %d", slres);
        }
    } while (bootStatus != SL_DEMOD_BOOT_STATUS_COMPLETE);

    SL_Printf("\n Demod Boot Status 	 : ");
    if (bootStatus == SL_DEMOD_BOOT_STATUS_INPROGRESS)
    {
        SL_Printf("%s", "INPROGRESS");
    }
    else if (bootStatus == SL_DEMOD_BOOT_STATUS_COMPLETE)
    {
        SL_Printf("%s", "COMPLETED");
    }
    else if (bootStatus == SL_DEMOD_BOOT_STATUS_ERROR)
    {
        SL_Printf("%s", "ERROR");
        //goto TEST_ERROR;
    }

    slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_AFEIF, &afeInfo);
    if (slres != 0) {
        _TOLKA_PHY_ANDROID_ERROR("Error:SL_DemodConfigure: %d", slres);
    }

    slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_IQ_OFFSET_CORRECTION, &iqOffSetCorrection);
    if (slres != 0) {
        _TOLKA_PHY_ANDROID_ERROR("Error:SL_DemodConfigure: res: %d", slres);
    }

    slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_OUTPUTIF, &outPutInfo);
    if (slres != 0)
    {
        _TOLKA_PHY_ANDROID_ERROR("\n Error:SL_DemodConfigure: %d", slres);

    }
    slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_EXT_LNA, (unsigned int *)&lnaInfo.lnaMode);
    if (slres != 0)
    {
        _TOLKA_PHY_ANDROID_ERROR("\n Error:SL_DemodConfigure: %d", slres);
    }

    slres = SL_DemodConfigureEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
    if (slres != 0)
    {
        _TOLKA_PHY_ANDROID_ERROR("\n Error:SL_DemodConfigureEx: %d", slres);

    }

    slres = SL_DemodGetSoftwareVersion(slUnit, &swMajorNo, &swMinorNo);
    if (slres == SL_OK)
    {
        _TOLKA_PHY_ANDROID_INFO("\n Demod SW Version		 : %d.%d", swMajorNo, swMinorNo);
    }

    /* Tuner Config */
    tunerCfg.bandwidth = SL_TUNER_BW_6MHZ;
    tunerCfg.std = SL_TUNERSTD_ATSC3_0;

    cres = SL_ConfigSetBbCapture(BB_CAPTURE_DISABLE);
    if (cres != SL_CONFIG_OK)
    {
        _TOLKA_PHY_ANDROID_ERROR("ERROR : SL_ConfigSetBbCapture Failed ");
        //return -1;
    }
    slres = SL_DemodStart(slUnit);

    if (slres != 0)
    {
        _TOLKA_PHY_ANDROID_ERROR("\n Saankhya Demod Start Failed");
        //goto TEST_ERROR;
    }
    else
    {
        demodStartStatus = 1;
        _TOLKA_PHY_ANDROID_INFO("SL_DemodStart: Success");
        SL_Printf("SUCCESS");
    }
    SL_SleepMS(1000); // Delay to accomdate set configurations at SL to take effect
    slres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_PLP_LLS_INFO, &llsPlpInfo);
    if (slres != SL_OK)
    {
        _TOLKA_PHY_ANDROID_ERROR("\n Error: ATSC3 Get LlsPlp List: %d", slres);
        if (slres == SL_ERR_CMD_IF_FAILURE)
        {
            //jjustman-2022-05-24 - todo
            //handleCmdIfFailure();

            //goto TEST_ERROR;
        }
    }

    plpllscount = 0;
    plpInfoVal = 1;
    for (int plpIndx = 0; (plpIndx < 64) && (plpllscount < 4); plpIndx++)
    {
        //plpInfoVal = ((llsPlpInfo & (llsPlpMask << plpIndx)) == pow(2.0, plpIndx)) ? 0x01 : 0xFF;
        SL_Printf("\n [plpIndx(%d)]plpllscount=%d  plpInfoVal=%d\n", plpIndx, plpllscount, plpInfoVal);
        if (plpInfoVal == 0x01)
        {
            plpllscount++;
            if (plpllscount == 1)
            {
                atsc3ConfigInfo.plpConfig.plp0 = (char)plpIndx;
            }
            else if (plpllscount == 2)
            {
                atsc3ConfigInfo.plpConfig.plp1 = (char)plpIndx;
            }
            else if (plpllscount == 3)
            {
                atsc3ConfigInfo.plpConfig.plp2 = (char)plpIndx;
            }
            else if (plpllscount == 4)
            {
                atsc3ConfigInfo.plpConfig.plp3 = (char)plpIndx;
            }
            else
            {
                plpllscount++;
            }
        }
    }

    if (atsc3ConfigInfo.plpConfig.plp0 ==  (char)0xFF)
    {
        atsc3ConfigInfo.plpConfig.plp0 = (char)0x00;
    }

    return slres;
}


int  TolkaPHYAndroid::tune(int freqKhz, int single_plp) {
    //jj: todo
    return -1;
};
int  TolkaPHYAndroid::listen_plps(vector<uint8_t> plps) {
    //jj: todo
    return -1;
};

void TolkaPHYAndroid::NotifyPlpSelectionChangeCallback(vector<uint8_t> plps, void *context) {
    ((TolkaPHYAndroid *) context)->listen_plps(plps);
}

//jni pinning

void TolkaPHYAndroid::pinProducerThreadAsNeeded() {
    producerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_tolka_static_loader_get_javaVM(), "TolkaPHYAndroid::producerThread");
}

void TolkaPHYAndroid::releasePinnedProducerThreadAsNeeded() {
    if(producerJniEnv) {
        delete producerJniEnv;
        producerJniEnv = nullptr;
    }
}

void TolkaPHYAndroid::pinConsumerThreadAsNeeded() {
    _TOLKA_PHY_ANDROID_DEBUG("TolkaPHYAndroid::pinConsumerThreadAsNeeded: mJavaVM: %p, atsc3_ndk_application_bridge instance: %p", atsc3_ndk_phy_tolka_static_loader_get_javaVM(), atsc3_ndk_application_bridge_get_instance());

    consumerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_tolka_static_loader_get_javaVM(), "TolkaPHYAndroid::consumerThread");
    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->pinConsumerThreadAsNeeded();
    }
}

void TolkaPHYAndroid::releasePinnedConsumerThreadAsNeeded() {
    if(consumerJniEnv) {
        delete consumerJniEnv;
        consumerJniEnv = nullptr;
    }

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->releasePinnedConsumerThreadAsNeeded();
    }
}

void TolkaPHYAndroid::pinStatusThreadAsNeeded() {
    statusJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_tolka_static_loader_get_javaVM(), "TolkaPHYAndroid::statusThread");

    if(atsc3_ndk_phy_bridge_get_instance()) {
        atsc3_ndk_phy_bridge_get_instance()->pinStatusThreadAsNeeded();
    }
}

void TolkaPHYAndroid::releasePinnedStatusThreadAsNeeded() {
    if(statusJniEnv) {
        delete statusJniEnv;
        statusJniEnv = nullptr;
    }

    if(atsc3_ndk_phy_bridge_get_instance()) {
        atsc3_ndk_phy_bridge_get_instance()->releasePinnedStatusThreadAsNeeded();
    }
}

//dummy jni impls

extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_init(JNIEnv *env, jobject instance) {
    lock_guard<mutex> tolka_phy_android_cctor_mutex_local(TolkaPHYAndroid::CS_global_mutex);

    _TOLKA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_init: start init, env: %p", env);
    if(tolkaPHYAndroid) {
        _TOLKA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_init: start init, TolkaPHYAndroid is present: %p, calling deinit/delete", tolkaPHYAndroid);
        tolkaPHYAndroid->deinit();
        tolkaPHYAndroid = nullptr;
    }

    tolkaPHYAndroid = new TolkaPHYAndroid(env, instance);
    tolkaPHYAndroid->init();

    _TOLKA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_init: return, instance: %p", tolkaPHYAndroid);

    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_run(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> tolka_phy_android_cctor_mutex_local(TolkaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!tolkaPHYAndroid) {
        _TOLKA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_run: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        res = tolkaPHYAndroid->run();
        _TOLKA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_run: returning res: %d", res);
    }

    return res;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_is_1running(JNIEnv* env, jobject instance)
{
    lock_guard<mutex> tolka_phy_android_cctor_mutex_local(TolkaPHYAndroid::CS_global_mutex);

    jboolean res = false;

    if(!tolkaPHYAndroid) {
        _TOLKA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_is_1running: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = false;
    } else {
        res = tolkaPHYAndroid->is_running();
    }

    return res;
}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_stop(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> tolka_phy_android_cctor_mutex_local(TolkaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!tolkaPHYAndroid) {
        _TOLKA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_stop: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        res = tolkaPHYAndroid->stop();
        _TOLKA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_stop: returning res: %d", res);
    }

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_deinit(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> tolka_phy_android_cctor_mutex_local(TolkaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!tolkaPHYAndroid) {
        _TOLKA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_deinit: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {

        tolkaPHYAndroid->deinit();
        tolkaPHYAndroid = nullptr;
    }

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_download_1bootloader_1firmware(JNIEnv *env, jobject thiz, jint fd, jint device_type, jstring device_path_jstring) {
    lock_guard<mutex> tolka_phy_android_cctor_mutex_local(TolkaPHYAndroid::CS_global_mutex);

    _TOLKA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_download_1bootloader_1firmware: fd: %d", fd);
    int res = 0;

    if(!tolkaPHYAndroid)  {
        _TOLKA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_download_1bootloader_1firmware: TolkaPHYAndroid is NULL!");
        res = -1;
    } else {
        const char* device_path_weak = env->GetStringUTFChars(device_path_jstring, 0);
        string device_path(device_path_weak);

        res = tolkaPHYAndroid->download_bootloader_firmware(fd, device_type, device_path); //calls pre_init
        env->ReleaseStringUTFChars( device_path_jstring, device_path_weak );

        //jjustman-2020-08-23 - hack, clear out our in-flight reference since we should re-enumerate
        delete tolkaPHYAndroid;
        tolkaPHYAndroid = nullptr;
    }

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_open(JNIEnv *env, jobject thiz, jint fd, jint device_type, jstring device_path_jstring) {
    lock_guard<mutex> tolka_phy_android_cctor_mutex_local(TolkaPHYAndroid::CS_global_mutex);

    _TOLKA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_open: fd: %d", fd);

    int res = 0;
    if(!tolkaPHYAndroid) {
        _TOLKA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_open: TolkaPHYAndroid is NULL!");
        res = -1;
    } else {
        const char* device_path_weak = env->GetStringUTFChars(device_path_jstring, 0);
        string device_path(device_path_weak);

        res = tolkaPHYAndroid->open(fd, device_type, device_path);
        env->ReleaseStringUTFChars( device_path_jstring, device_path_weak );
    }
    _TOLKA_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_open: fd: %d, return: %d", fd, res);

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_tune(JNIEnv *env, jobject thiz,
                                                                      jint freq_khz,
                                                                      jint single_plp) {

    lock_guard<mutex> tolka_phy_android_cctor_mutex_local(TolkaPHYAndroid::CS_global_mutex);


    int res = 0;
    if(!tolkaPHYAndroid) {
        _TOLKA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_tune: TolkaPHYAndroid is NULL!");
        res = -1;
    } else {
        res = tolkaPHYAndroid->tune(freq_khz, single_plp);
    }

    return res;
}
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_listen_1plps(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jobject plps) {
    lock_guard<mutex> tolka_phy_android_cctor_mutex_local(TolkaPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!tolkaPHYAndroid) {
        _TOLKA_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_listen_1plps: TolkaPHYAndroid is NULL!");
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

        res = tolkaPHYAndroid->listen_plps(listen_plps);
    }

    return res;
}

extern "C" JNIEXPORT jstring JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_get_1sdk_1version(JNIEnv *env, jobject thiz) {
    string sdk_version = tolkaPHYAndroid->get_sdk_version();
    return env->NewStringUTF(sdk_version.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_TolkaPHYAndroid_get_1firmware_1version(JNIEnv *env, jobject thiz) {
    string firmware_version = tolkaPHYAndroid->get_firmware_version();
    return env->NewStringUTF(firmware_version.c_str());
}

//carry-over methods from atsc3.cpp


static void printToConsolePlfConfiguration(SL_PlatFormConfigParams_t cfgInfo)
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

        case SL_KAILASH_DONGLE_2:
            SL_Printf("\n Board Type             : SL_KAILASH_DONGLE_2");
            break;

        case SL_SILISA_DONGLE:
            SL_Printf("\n Board Type             : SL_SILISA_DONGLE");
            break;

        case SL_YOGA_DONGLE:
            SL_Printf("\n Board Type             : SL_YOGA_DONGLE");
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
    else if (cfgInfo.tunerType == TUNER_SI_P)
    {
        SL_Printf("\n Tuner Type             : TUNER_SI_P");
    }
    else if (cfgInfo.tunerType == TUNER_SILABS)
    {
        SL_Printf("\n Tuner Type             : TUNER_SILABS");
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

static void logPlfConfiguration(SL_PlatFormConfigParams_t cfgInfo)
{
    SL_Printf("\n---------SL Platform Configuration----------------------");
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

        case SL_KAILASH_DONGLE_2:
            SL_Printf("\n Board Type             : SL_KAILASH_DONGLE_2");
            break;

        case SL_SILISA_DONGLE:
            SL_Printf("\n Board Type             : SL_KAILASH_DONGLE_3");
            break;
#if 0
            case SL_BORQS_EVT:
		SL_Printf("\n Board Type             : SL_BORQS_EVT");
		break;
#endif
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
    else if (cfgInfo.tunerType == TUNER_SI_P)
    {
        SL_Printf("\n Tuner Type             : TUNER_SI_P");
    }
    else if (cfgInfo.tunerType == TUNER_SILABS)
    {
        SL_Printf("\n Tuner Type             : TUNER_SILABS");
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

    printf("\n Demod I2C Address      : 0x%x\n", cfgInfo.demodI2cAddr);
}


SL_I2cResult_t SL_I2cWrite_tolka(unsigned char i2cAddr, unsigned int wLen, unsigned char *data)
{
    SL_I2cResult_t retVal = SL_I2C_OK;
    Dword error = BR_ERR_NO_ERROR;
    if (wLen > 254)
    {
        printf("[SL_I2cWrite]I2C write length > 254 bytes!!!!\n");
        return SL_I2C_ERR_TRANSFER_FAILED;
    }
    //retVal = (SL_I2cResult_t)IT9300_ExtI2C_write(endeavour_sl3000, 0, endeavour_sl3000_i2cBus, (Byte)(i2cAddr<<1), (Byte)wLen, data, False);
    usleep(10000); //jjustman-2022-05-24 - super hack???

    error = IT9300_ExtI2C_write(&TolkaPHYAndroid::Endeavour_s, 0, TOLKA_USB_sl3000_i2cBus, (Byte)(i2cAddr << 1), (Byte)wLen, data, False);
    if (!error)
        retVal = SL_I2C_OK;
    else
    {
        printf("[SL_I2cWrite]error=0x%x\n", error);
        retVal = SL_I2C_ERR_TRANSFER_FAILED;
    }
    return retVal;
}

SL_I2cResult_t SL_I2cRead_tolka(unsigned char i2cAddr, unsigned int rLen, unsigned char*data)
{
    SL_I2cResult_t retVal = SL_I2C_OK;
    Dword error = BR_ERR_NO_ERROR;

    Byte		buf[255];
    int 		len=0;
    if (rLen > 255)
    {
        printf("[SL_I2cRead]I2C write length > 254 bytes!!!!\n");
        return SL_I2C_ERR_TRANSFER_FAILED;
    }

#if 0
    for(int i=0;i<rLen;i++)
		SL_Printf("[SL_I2cRead]data[%d] =0x%x \n",i,data[i]);
#endif


    usleep(10000); //jjustman-2022-05-24 - super hack???

    error = IT9300_ExtI2C_read(&TolkaPHYAndroid::Endeavour_s, 0, TOLKA_USB_sl3000_i2cBus, (Byte)((i2cAddr<<1)+1), rLen, data);
    if (!error)
        retVal = SL_I2C_OK;
    else
    {
        printf("[SL_I2cRead]error=0x%x\n", error);
        retVal = SL_I2C_ERR_TRANSFER_FAILED;
    }


    return retVal;
}


void SL_ConfigureI2c_tolka() {
    slI2cDispatcherMethods.SL_I2cPreInit     = nullptr;
    slI2cDispatcherMethods.SL_I2cInit        = nullptr;
    slI2cDispatcherMethods.SL_I2cUnInit      = nullptr;
    slI2cDispatcherMethods.SL_I2cWrite       = SL_I2cWrite_tolka;
    slI2cDispatcherMethods.SL_I2cRead        = SL_I2cRead_tolka;
    slI2cDispatcherMethods.SL_I2cWriteNoStop = nullptr;
    slI2cDispatcherMethods.SL_I2cReadNoStop  = nullptr;
}

//            error = IT9300_writeRegister(&endeavour, 0, p_br_reg_top_gpioh2_on, 0x01);
SL_GpioResult_t SL_GpioSetPin_tolka_hack(unsigned char gpio, unsigned char value) {
    if(gpio == 137) {
        //jjustman-2022-05-24 - hack for sl_demodgpioreset impl
        _TOLKA_PHY_ANDROID_WARN("SL_GpioSetPin_tolka_hack: invoking IT9300_writeRegister for p_br_reg_top_gpioh2_on: %d, val: %d", gpio, value);

        IT9300_writeRegister(&TolkaPHYAndroid::Endeavour_s, 0, p_br_reg_top_gpioh2_on, value);
        usleep(250000);
    } else {
        _TOLKA_PHY_ANDROID_WARN("SL_GpioSetPin_tolka_hack: returning SL_GPIO_OK for UNKNOWN gpio: %d, val: %d", gpio, value);
    }
    return SL_GPIO_OK;
}


//missing sl_demod.c - SL_DemodReset / SL_DemodGpioReset overload...
void SL_DispatcherConfig_tolka()
{
   //jjustman-2022-05-24 - noop...
    SL_ConfigureI2c_tolka();
    slGpioDispatcherMethods.SL_GpioSetPin = SL_GpioSetPin_tolka_hack;

}