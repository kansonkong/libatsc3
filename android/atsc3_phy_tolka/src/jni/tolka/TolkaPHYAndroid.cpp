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

SL_Result_t     TolkaPHYAndroid::global_sl_result_error_flag = SL_OK;
SL_I2cResult_t  TolkaPHYAndroid::global_sl_i2c_result_error_flag = SL_I2C_OK;

int TolkaPHYAndroid::Last_tune_freq = -1;

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
//    SL_SleepMS(1000); // Delay to accomdate set configurations at SL to take effect
//    slres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_PLP_LLS_INFO, &llsPlpInfo);
//    if (slres != SL_OK)
//    {
//        _TOLKA_PHY_ANDROID_ERROR("\n Error: ATSC3 Get LlsPlp List: %d", slres);
//        if (slres == SL_ERR_CMD_IF_FAILURE)
//        {
//            //jjustman-2022-05-24 - todo
//            //handleCmdIfFailure();
//
//            //goto TEST_ERROR;
//        }
//    }
//
//    plpllscount = 0;
//    plpInfoVal = 1;
//    for (int plpIndx = 0; (plpIndx < 64) && (plpllscount < 4); plpIndx++)
//    {
//        //plpInfoVal = ((llsPlpInfo & (llsPlpMask << plpIndx)) == pow(2.0, plpIndx)) ? 0x01 : 0xFF;
//        SL_Printf("\n [plpIndx(%d)]plpllscount=%d  plpInfoVal=%d\n", plpIndx, plpllscount, plpInfoVal);
//        if (plpInfoVal == 0x01)
//        {
//            plpllscount++;
//            if (plpllscount == 1)
//            {
//                atsc3ConfigInfo.plpConfig.plp0 = (char)plpIndx;
//            }
//            else if (plpllscount == 2)
//            {
//                atsc3ConfigInfo.plpConfig.plp1 = (char)plpIndx;
//            }
//            else if (plpllscount == 3)
//            {
//                atsc3ConfigInfo.plpConfig.plp2 = (char)plpIndx;
//            }
//            else if (plpllscount == 4)
//            {
//                atsc3ConfigInfo.plpConfig.plp3 = (char)plpIndx;
//            }
//            else
//            {
//                plpllscount++;
//            }
//        }
//    }
//
//    if (atsc3ConfigInfo.plpConfig.plp0 ==  (char)0xFF)
//    {
//        atsc3ConfigInfo.plpConfig.plp0 = (char)0x00;
//    }

    return slres;
}

//jjustman-2022-05-24 - stub


void TolkaPHYAndroid::resetProcessThreadStatistics() {
    alp_completed_packets_parsed = 0;
    alp_total_bytes = 0;
    alp_total_LMTs_recv = 0;
}



void TolkaPHYAndroid::statusMetricsResetFromTuneChange() {
    _TOLKA_PHY_ANDROID_INFO("statusMetricsResetFromContextChange - resetting statusThreadFirstLoopAfterTuneComplete");

    statusThreadFirstLoopAfterTuneComplete = true; //will dump DemodGetconfiguration from statusThread

    statusMetricsResetFromPLPListenChange(); //clear our diag flags and metrics types also...
}

void TolkaPHYAndroid::statusMetricsResetFromPLPListenChange() {
    _TOLKA_PHY_ANDROID_INFO("statusMetricsResetFromPLPListenChange - resetting statusThreadFirstLoop_*Lock flags and clearing TunerSignalInfo/_Diag's");

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

void TolkaPHYAndroid::allocate_atsc3_sl_tlv_block() {
    //protect for de-alloc with using recursive lock here
    atsc3_sl_tlv_block_mutex.lock();

    if(!atsc3_sl_tlv_block) {
        atsc3_sl_tlv_block = block_Alloc(TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE);
    }
    atsc3_sl_tlv_block_mutex.unlock();
}

SL_Result_t TolkaPHYAndroid::SL3000_atsc3_tune(SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *sPlfConfig)
{
    SL_I2cResult_t i2cres;
    SL_Result_t slres;
    SL_AfeIfConfigParams_t afeInfo;
    SL_OutIfConfigParams_t outPutInfo;
    SL_IQOffsetCorrectionParams_t iqOffSetCorrection;
    SL_DemodBootStatus_t bootStatus;
    SL_TunerSignalInfo_t tunerInfo;
    SL_TunerResult_t tres;

    SL_DemodStd_t std = SL_DEMODSTD_ATSC3_0;

    slres = SL_DemodConfigureEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
    if (slres != 0)
    {
        _TOLKA_PHY_ANDROID_ERROR("Error:SL_Demod Get ATSC3 Configuration: res: %d", slres);
        return slres;
    }
//
//    slres = SL_DemodGetConfiguration(slUnit, &cfgInfo);
//    if (slres != SL_OK)
//    {
//        SL_Printf("\n Error:SL_Demod Get Configuration :");
//        printToConsoleDemodError(slres);
//        if (slres == SL_ERR_CMD_IF_FAILURE)
//        {
//            handleCmdIfFailure();
//            //goto TEST_ERROR;
//        }
//    }
//    else
//    {
//        SL_Result_t slres = SL_DemodGetConfigurationEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
//        if (slres != SL_OK)
//        {
//            SL_Printf("\n Error:SL_Demod Get ConfigurationEX :");
//            printToConsoleDemodError(slres);
//            if (slres == SL_ERR_CMD_IF_FAILURE)
//            {
//                handleCmdIfFailure();
//
//            }
//        }
//        printToConsoleDemodConfiguration(cfgInfo);
//        printToConsoleDemodAtsc3Configuration(atsc3ConfigInfo);
//    }

    //SL_Printf("\n\n Show SL Diagnostics    : [y/n] :");

    return slres;
}

//#if 0
////plpIDNum Number of valid PLP ID in plpID[]. (1 - 4)
//SL_Result_t SL3000_atsc3_setPLP(char plpIDNum, char* plpID)
//{
//	SL_Result_t slres = SL_OK;
//#if 0
//	for(int i=0;i<4;i++)
//		SL_Printf("\n plpID[%d]=%d\n",i,plpID[i]);
//#endif
//	switch (plpIDNum)
//	{
//	case 1:
//		atsc3ConfigInfo.plpConfig.plp0 = plpID[0] & 0xFF;
//		atsc3ConfigInfo.plpConfig.plp1 = 0xFF;
//		atsc3ConfigInfo.plpConfig.plp2 = 0xFF;
//		atsc3ConfigInfo.plpConfig.plp3 = 0xFF;
//		break;
//	case 2:
//		atsc3ConfigInfo.plpConfig.plp0 = plpID[0] & 0xFF;
//		atsc3ConfigInfo.plpConfig.plp1 = plpID[1] & 0xFF;
//		atsc3ConfigInfo.plpConfig.plp2 = 0xFF;
//		atsc3ConfigInfo.plpConfig.plp3 = 0xFF;
//		break;
//	case 3:
//		atsc3ConfigInfo.plpConfig.plp0 = plpID[0] & 0xFF;
//		atsc3ConfigInfo.plpConfig.plp1 = plpID[1] & 0xFF;
//		atsc3ConfigInfo.plpConfig.plp2 = plpID[2] & 0xFF;
//		atsc3ConfigInfo.plpConfig.plp3 = 0xFF;
//		break;
//	case 4:
//		atsc3ConfigInfo.plpConfig.plp0 = plpID[0] & 0xFF;
//		atsc3ConfigInfo.plpConfig.plp1 = plpID[1] & 0xFF;
//		atsc3ConfigInfo.plpConfig.plp2 = plpID[2] & 0xFF;
//		atsc3ConfigInfo.plpConfig.plp3 = plpID[3] & 0xFF;
//		break;
//	default:
//		slres = SL_ERR_INVALID_ARGUMENTS;
//		break;
//	}
//
//
//	slres = SL_DemodConfigureEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
//	if (slres != 0)
//	{
//		SL_Printf("\n Error:SL_Demod Get ATSC3 Configuration :");
//		printToConsoleDemodError(slres);
//		//goto TEST_ERROR;
//	}
//
//	return slres;
//}
//#endif
//SL_Result_t SL3000_atsc3_setPLP(char plpMask, char plpID[4])
//{
//    SL_Result_t sRes = SL_OK;
//#if 0
//    for(int i=0;i<4;i++)
//	 SL_Printf("\n plpID[%d]=%d\n",i,plpID[i]);
//#endif
//    //sRes= (SL_Result_t)manualplpConfig(plpMask, &atsc3ConfigInfo);
//    int Res=manualplpConfig(plpMask, &atsc3ConfigInfo,plpID);
//
//    sRes = SL_DemodConfigureEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
//    if (sRes != 0)
//    {
//        SL_Printf("\n Error:SL_Demod Get ATSC3 Configuration :");
//        printToConsoleDemodError(sRes);
//        //goto TEST_ERROR;
//    }
//
//    return sRes;
//
//}

int TolkaPHYAndroid::tune(int freqKhz, int single_plp) {
    int ret = 0;

    R855_ErrCode R855_result = R855_Success;
    SL_Result_t sl3000_result = SL_OK;
    SignalInfo_t sigInfo;
    jlong result = 0;
    int monitor_timer = 7; // monitor locked signal 7 time

    int id = 0;
    int bandwidthKHz = 6000;
    _TOLKA_PHY_ANDROID_INFO("tune: id: %d, freq: %d, bandwidhtKhz: %d", freqKhz, bandwidthKHz);

    TolkaPHYAndroid::cb_should_discard = true;

    //acquire our CB mutex so we don't push stale TLV packets
    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);

    //jjustman-2021-03-10 - also acquire our atsc3_sl_tlv_block_mutex so we can safely discard any pending TLV frames
    atsc3_sl_tlv_block_mutex.lock();

    if(cb) {
        //forcibly flush any in-flight TLV packets in cb here by calling, need (cb_should_discard == true),
        // as our type is atomic_bool and we can't printf its value here due to:
        //                  call to implicitly-deleted copy constructor of 'std::__ndk1::atomic_bool' (aka 'atomic<bool>')
        _TOLKA_PHY_ANDROID_INFO("TolkaPHYAndroid::tune - cb_should_discard: %u, cb_GetDataSize: %zu, calling CircularBufferReset(), cb: %p, early in tune() call",
                                   (cb_should_discard == true), CircularBufferGetDataSize(this->cb), cb);
        CircularBufferReset(cb);
    }

    //jjustman-2021-03-10 - destroy (and let processTLVFromCallback) recreate our atsc3_sl_tlv_block and atsc3_sl_tlv_payload to avoid race condition between cb mutex and sl_tlv block processing
    if(atsc3_sl_tlv_block) {
        block_Destroy(&atsc3_sl_tlv_block);
    }

    if(atsc3_sl_tlv_payload) {
        atsc3_sl_tlv_payload_free(&atsc3_sl_tlv_payload);
    }

    //acquire our lock for setting tuning parameters (including re-tuning)
    unique_lock<mutex> SL_I2C_command_mutex_tuner_tune(SL_I2C_command_mutex);
    unique_lock<mutex> SL_PlpConfigParams_mutex_update_plps(SL_PlpConfigParams_mutex, std::defer_lock);

    atsc3_core_service_application_bridge_reset_context();

    if (freqKhz != 0) {
        switch (bandwidthKHz) {
            case 6000:
                SL3000_R855_driver[0].tunerCfg.bandwidth = SL_TUNER_BW_6MHZ;
                break;
            case 7000:
                SL3000_R855_driver[0].tunerCfg.bandwidth = SL_TUNER_BW_7MHZ;
                break;
            case 8000:
                SL3000_R855_driver[0].tunerCfg.bandwidth = SL_TUNER_BW_8MHZ;
                break;
            default:
                SL3000_R855_driver[0].tunerCfg.bandwidth = SL_TUNER_BW_6MHZ;
                break;
        }
        SL3000_R855_driver[0].R855_Info.RF_KHz = (UINT32) freqKhz; // unit: kHz
        SL_Result_t slres = SL_OK;

        //IF AGC select
        SL3000_R855_driver[0].R855_Info.R855_IfAgc_Select = R855_IF_AGC1;
        R855_result = Tune_R855(&SL3000_R855_driver[0].R855_Info);
        if(R855_result != R855_Success) {
            printf("R855_tune() error = %d\n", R855_result);
            return -1;      // R855_Fail
        } else {
            printf("R855_tune() OK = %d\n", R855_result);
        }

        sl3000_result = SL3000_atsc3_tune(&SL3000_R855_driver[0].tunerCfg,
                                  &SL3000_R855_driver[0].PlfConfig);
        if(sl3000_result != SL_OK) {
            printf("SL3000_atsc3_tune() error = %d\n", sl3000_result);
            return sl3000_result;
        } else {
            printf("SL3000_atsc3_tune() OK = %d\n", sl3000_result);
        }

//        while(monitor_timer) {
//            Monitor_SL3000_ATSC3_Signal(&sigInfo, SL3000_R855_driver[0].R855_Info.RF_KHz, SL3000_R855_driver[0].R855_Info.R855_Standard);
//            if (sigInfo.locked == LOCKED) {
//                printf("Monitor_SL3000_ATSC3_Signal() Locked, RSSI: %d db\n", sigInfo.rssi);
//                result = 0;
//                return result;
//            } else {
//                printf("Monitor_SL3000_ATSC3_Signal() Unlock, RSSI: %d db\n", sigInfo.rssi);
//                result = -1;
//            }
//            monitor_timer--;
//            usleep(2000000);
//        }

        //setup shared memory for cb callback (or reset if already allocated)
        if(!cb) {
            cb = CircularBufferCreate(TLV_CIRCULAR_BUFFER_SIZE);
        } else {
            //jjustman-2021-01-19 - clear out our current cb on re-tune
            CircularBufferReset(cb);
            //just in case any last pending SDIO transactions arent completed yet...
            SL_SleepMS(100);
        }

        atsc3_sl_tlv_block_mutex.unlock();
        CircularBufferMutex_local.unlock();

        //jjustman-2021-01-19 - allow for cb to start acumulating TLV frames
        TolkaPHYAndroid::cb_should_discard = false;

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
                _TOLKA_PHY_ANDROID_WARN("::Open() - starting captureThread took %d spins, final state: %d",
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
                _TOLKA_PHY_ANDROID_WARN("::Open() - starting processThreadIsRunning took %d spins, final state: %d",
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
                _TOLKA_PHY_ANDROID_WARN("::Open() - starting statusThread took %d spins, final state: %d",
                                           threadStartupSpinlockCount,
                                           this->statusThreadIsRunning);
            }
        }

//        if(!demodStartStatus) {
//            while (SL_IsRxDataStarted() != 1) {
//                SL_SleepMS(100);
//
//                if (((isRxDataStartedSpinCount++) % 100) == 0) {
//                    _SAANKHYA_PHY_ANDROID_WARN("::Open() - waiting for SL_IsRxDataStarted, spinCount: %d", isRxDataStartedSpinCount);
//                    //jjustman-2020-10-21 - todo: reset demod?
//                }
//            }
//
//            /*
//
//             jjustman-2021-05-04 - testing for (seemingly) random YOGA cpu crashes:
//
//                2021-05-04 19:17:38.042 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SaankhyaPHYAndroid.cpp          : 627:DEBUG:1620170258.0428:SL_DemodInit: SUCCESS, slUnit: 0, slres: 0
//                2021-05-04 19:17:38.043 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SaankhyaPHYAndroid.cpp          : 632:DEBUG:1620170258.0432:before SL_DemodGetStatus: slUnit: 0, slres is: 0
//                2021-05-04 19:17:38.045 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SaankhyaPHYAndroid.cpp          : 634:DEBUG:1620170258.0454:SL_DemodGetStatus: slUnit: 0, slres is: 0
//                2021-05-04 19:17:38.295 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SaankhyaPHYAndroid.cpp          : 643:DEBUG:1620170258.2957:Demod Boot Status  :
//                2021-05-04 19:17:38.296 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SaankhyaPHYAndroid.cpp          : 650:DEBUG:1620170258.2960:COMPLETED
//                2021-05-04 19:17:38.316 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SL_DemodConfigPlps, instance: 0, plp [0]: 0x00, [1]: 0xff, [2]: 0xff, [3]: 0xff
//                2021-05-04 19:17:38.325 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SaankhyaPHYAndroid.cpp          : 701:DEBUG:1620170258.3259:Demod SW Version: 3.24
//                2021-05-04 19:17:38.669 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SaankhyaPHYAndroid.cpp          : 763:DEBUG:1620170258.6698:OPEN COMPLETE!
//                2021-05-04 19:17:38.670 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SaankhyaPHYAndroid.cpp          :2431:DEBUG:1620170258.6701:Java_org_ngbp_libatsc3_middleware_android_phy_SaankhyaPHYAndroid_open: fd: 145, return: 0
//                2021-05-04 19:17:38.670 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/UsbAtsc3Source: prepareDevices: open with org.ngbp.libatsc3.middleware.android.phy.SaankhyaPHYAndroid@d66354a for path: /dev/bus/usb/002/002, fd: 145, success
//                2021-05-04 19:17:38.710 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: atsc3_core_service_player_bridge: 122:WARN :1620170258.7103:atsc3_core_service_application_bridge_reset_context!
//                2021-05-04 19:17:38.710 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SaankhyaPHYAndroid::tune: Frequency: 593000, PLP: 0
//                2021-05-04 19:17:38.740 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SaankhyaPHYAndroid.cpp          : 815:DEBUG:1620170258.7400:Error:SL_TunerSetFrequency :
//                2021-05-04 19:17:38.740 4722-4865/com.nextgenbroadcast.mobile.middleware.sample D/NDK: SaankhyaPHYAndroid.cpp          :1412:DEBUG:1620170258.7401: Sl Tuner Operation Failed
//
//
//            if (getPlfConfig.boardType == SL_KAILASH_DONGLE_3) {
//                _SAANKHYA_PHY_ANDROID_DEBUG("::tune - before SL_DemodStart - sleeping for 2000 seconds to avoid double SL_DemodConfigPlps call(s)");
//                usleep(2000000);
//
//            }
//             */
//
//
//            _SAANKHYA_PHY_ANDROID_DEBUG("Starting SLDemod: ");
//
//            slres = SL_DemodStart(slUnit);
//
//            if (!(slres == SL_OK || slres == SL_ERR_ALREADY_STARTED)) {
//                _SAANKHYA_PHY_ANDROID_DEBUG("Saankhya Demod Start Failed");
//                demodStartStatus = 0;
//                goto ERROR;
//            } else {
//                demodStartStatus = 1;
//                _SAANKHYA_PHY_ANDROID_DEBUG("SUCCESS");
//                //_SAANKHYA_PHY_ANDROID_DEBUG("SL Demod Output Capture: STARTED : sl-tlv.bin");
//            }
//            SL_SleepMS(500); // Delay to accomdate set configurations at SL to take effect for SL_DemodStart()
//
//        } else {
//            _SAANKHYA_PHY_ANDROID_DEBUG("SLDemod: already running");
//        }

        SL_PlpConfigParams_mutex_update_plps.lock();

        atsc3ConfigInfo.plpConfig.plp0 = single_plp;
        atsc3ConfigInfo.plpConfig.plp1 = 0xFF;
        atsc3ConfigInfo.plpConfig.plp2 = 0xFF;
        atsc3ConfigInfo.plpConfig.plp3 = 0xFF;

        slres = SL_DemodConfigureEx(slUnit, demodStandard, &atsc3ConfigInfo);

        SL_PlpConfigParams_mutex_update_plps.unlock();

        if (slres != 0) {
            _TOLKA_PHY_ANDROID_ERROR("SL_DemodConfigPlps res: %d", slres);
            goto ERROR;
        }

        statusMetricsResetFromTuneChange();

        ret = 0;
        Last_tune_freq = freqKhz;

        goto UNLOCK;

        ERROR:
        ret = -1;
        Last_tune_freq = -1;

        //unlock our i2c mutex
        UNLOCK:
        SL_I2C_command_mutex_tuner_tune.unlock();
        return ret;


    }

    return result;
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

//jjustman-2022-05-24 - hacks

static volatile bool stopRx = false;
static volatile int  isCaptStarted = 0;
static unsigned int reqsize = 16;  // Request size in number of packets
static unsigned int queuedepth = 16;   // Number of requests to queue
static unsigned int pktsize = 1024;     // Maximum packet size for the endpoint
static unsigned int        success_count = 0;  // Number of successful transfers
static unsigned int        failure_count = 0;  // Number of failed transfers
static volatile int        rqts_in_flight = 0; // Number of transfers that are in progress
static RxDataCB USBCB = NULL;

static void xfer_callback(struct libusb_transfer *transfer)
{
    // Reduce the number of requests in flight.
    rqts_in_flight--;

    // Prepare and re-submit the read request.
    if (!stopRx)
    {
        switch (transfer->status)
        {
            case LIBUSB_TRANSFER_COMPLETED:
                if ((transfer->actual_length <= transfer->length) && (transfer->actual_length > 0))
                {
                    if (USBCB != NULL)
                    {
                        (*USBCB)(transfer->buffer, transfer->actual_length);
                    }
                    else
                    {
                        //Rx Data Callback not set
                    }
                    success_count++;

                }
                else
                {
                    _TOLKA_PHY_ANDROID_WARN("\n xfer_callback() : Invalid Buffer, not processed ");
                }
                break;
            case LIBUSB_TRANSFER_ERROR:     // Fall through
            case LIBUSB_TRANSFER_TIMED_OUT: // Fall through
            case LIBUSB_TRANSFER_CANCELLED: // Fall through
            case LIBUSB_TRANSFER_STALL:     // Fall through
            case LIBUSB_TRANSFER_NO_DEVICE: // Fall through
            case LIBUSB_TRANSFER_OVERFLOW:  // Fall through
                //SL_Log(SL_LOGTYPE_ERROR, (char*)"\n xfer Callback Failure:");
                failure_count++;
                break;
        }

        transfer->actual_length = 0;
        libusb_submit_transfer(transfer);
        rqts_in_flight++;

    }
}

// Function to free data buffers and transfer structures
static void free_transfer_buffers(unsigned char **databuffers, struct libusb_transfer **transfers)
{
    // Free up any allocated data buffers
    if (databuffers != NULL)
    {
        for (unsigned int i = 0; i < queuedepth; i++)
        {
            if (databuffers[i] != NULL)
            {
                free(databuffers[i]);
            }
            databuffers[i] = NULL;
        }
        free(databuffers);
    }

    // Free up any allocated transfer structures
    if (transfers != NULL) {
        for (unsigned int i = 0; i < queuedepth; i++) {
            if (transfers[i] != NULL) {
                /* jjustman-2021-12-10: TODO: check and confirm Disabled due to memory corruption */
                libusb_free_transfer(transfers[i]);
            }
            transfers[i] = NULL;
        }
        free(transfers);
    }
}

/*Sl Ref Fx3s RxData API */
static void SL_RxDataStart_tolka_hack(RxDataCB dataSecCb)
{
    stopRx = false; // reset our exit flag state

    int  rStatus;

    struct libusb_transfer **transfers = NULL;      // List of transfer structures.
    unsigned char **databuffers = NULL;         // List of data buffers.
    USBCB = dataSecCb; // Assign RxdataCB to USBCB function

    // Allocate buffers and transfer structures
    bool allocfail = false;

    databuffers = (unsigned char **)calloc(queuedepth, sizeof(unsigned char *));
    transfers = (struct libusb_transfer **)calloc(queuedepth, sizeof(struct libusb_transfer *));

    if ((databuffers != NULL) && (transfers != NULL))
    {
        for (unsigned int i = 0; i < queuedepth; i++)
        {
            databuffers[i] = (unsigned char *)malloc(reqsize * pktsize);
            transfers[i] = libusb_alloc_transfer(0);

            if ((databuffers[i] == NULL) || (transfers[i] == NULL))
            {
                allocfail = true;
                break;
            }
        }
    }
    else
    {
        allocfail = true;
    }

    // Check if all memory allocations have succeeded
    if (allocfail)
    {
        _TOLKA_PHY_ANDROID_WARN("Failed to allocate buffers and transfer structures");
        free_transfer_buffers(databuffers, transfers);
        return;
    }


    //  // Launch all the transfers till queue depth is complete
    //ResetDataEp();
    for (unsigned int i = 0; i < queuedepth; i++)
    {
        libusb_fill_bulk_transfer(transfers[i], TolkaPHYAndroid::Libusb_device_handle, TOLKA_USB_ENDPOINT_RX_TS,
                                  databuffers[i], reqsize * pktsize, xfer_callback, NULL, 5000); //5000
        rStatus = libusb_submit_transfer(transfers[i]);
        if (rStatus == 0)
            rqts_in_flight++;
    }
    isCaptStarted = 1;

    //jjustman-2020-08-23 - workaround for blocking libusb_handle_events which has a 60s timeout by default
    //probelmatic when we are in a thread_join waiting for this to teardown and blocked...

    int libusb_running_events_completed = 0;
    struct timeval tv_thread_running_events;
    tv_thread_running_events.tv_sec = 1;
    tv_thread_running_events.tv_usec = 0;

    while (!stopRx)
    {
        libusb_handle_events_timeout_completed(NULL, &tv_thread_running_events, &libusb_running_events_completed);
    }
    // Set the stop_transfers flag and wait until all transfers are complete.

    _TOLKA_PHY_ANDROID_WARN("Stopping transfers: \n");
    _TOLKA_PHY_ANDROID_WARN("%s:%d: stopping transfers with %d inflight", __FILE__, __LINE__, rqts_in_flight);

    //jjustman-2020-08-23 - workaround for blocking libusb_handle_events which has a 60s timeout by default
    int libusb_event_spin_count = 0;
    int libusb_events_completed = 0;

    struct timeval tv_shutdown;
    tv_shutdown.tv_sec = 0;
    tv_shutdown.tv_usec = 100000;

    while (rqts_in_flight != 0 && (libusb_event_spin_count++ < 100))
    {
        _TOLKA_PHY_ANDROID_WARN("\n %d requests are pending", rqts_in_flight);
        _TOLKA_PHY_ANDROID_WARN("%s:%d: stopping transfers with %d inflight, spin count: %d", __FILE__, __LINE__, rqts_in_flight, libusb_event_spin_count);

        libusb_handle_events_timeout_completed(NULL, &tv_shutdown, &libusb_events_completed);
    }

    if(rqts_in_flight > 0) {
        _TOLKA_PHY_ANDROID_WARN("%s:%d: ignoring remaining transfer with %d inflight - WARNING: this may leak transfer buffers, databuffers: %p, transfers: %p, and may crash with destroyedMutex exception", __FILE__, __LINE__, rqts_in_flight, databuffers, transfers);
    } else {
        // All transfers are complete. We can now free up all structures.
        _TOLKA_PHY_ANDROID_WARN("\n Transfers completed, freeing databuffers: %p, transfers: %p", databuffers, transfers);
        free_transfer_buffers(databuffers, transfers);
    }


    isCaptStarted = 0;
    _TOLKA_PHY_ANDROID_WARN("SL_Fx3s_RxDataStart: thread complete and returning");
    return;
}

static int SL_IsRxDataStarted_tolka(void)
{
    return isCaptStarted;
}

static void SL_RxDataStop_tolka_hack(void) {
    stopRx = true;
}

//missing sl_demod.c - SL_DemodReset / SL_DemodGpioReset overload...
void SL_DispatcherConfig_tolka()
{
   //jjustman-2022-05-24 - noop...
    SL_ConfigureI2c_tolka();
    slGpioDispatcherMethods.SL_GpioSetPin = SL_GpioSetPin_tolka_hack;
    slRxDataDispatcherMethods.SL_RxDataStart = SL_RxDataStart_tolka_hack;
    slRxDataDispatcherMethods.SL_RxDataStop = SL_RxDataStop_tolka_hack;

    slRxDataDispatcherMethods.SL_IsRxDataStarted = SL_IsRxDataStarted_tolka;

}


int TolkaPHYAndroid::processThread()
{
    _TOLKA_PHY_ANDROID_DEBUG("TolkaPHYAndroid::processThread: starting with this: %p", this);
    this->pinConsumerThreadAsNeeded();
    this->processThreadIsRunning = true;

    this->resetProcessThreadStatistics();

    while (this->processThreadShouldRun)
    {
        //_SAANKHYA_PHY_ANDROID_DEBUG("TolkaPHYAndroid::ProcessThread: getDataSize is: %d", CircularBufferGetDataSize(cb));

        //unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);

        while(CircularBufferGetDataSize(this->cb) >= TLV_CIRCULAR_BUFFER_MIN_PROCESS_SIZE) {
            processTLVFromCallbackInvocationCount++;
            this->processTLVFromCallback();
        }
        //CircularBufferMutex_local.unlock();

        //jjustman - try increasing to 50ms? shortest atsc3 subframe?
        usleep(33000); //jjustman-2022-02-16 - peg us at 16.67ms/2 ~ 8ms
        // pegs us at ~ 30 spinlocks/sec if no data
    }

    this->releasePinnedConsumerThreadAsNeeded();
    this->processThreadIsRunning = false;

    _TOLKA_PHY_ANDROID_INFO("TolkaPHYAndroid::ProcessThread complete");

    return 0;
}



int TolkaPHYAndroid::captureThread()
{
    _TOLKA_PHY_ANDROID_DEBUG("TolkaPHYAndroid::captureThread: starting with this: %p", this);
    this->pinProducerThreadAsNeeded();
    this->captureThreadIsRunning = true;

    SL_RxDataStart((RxDataCB)&TolkaPHYAndroid::RxDataCallback);

    this->releasePinnedProducerThreadAsNeeded();
    this->captureThreadIsRunning = false;

    _TOLKA_PHY_ANDROID_INFO("TolkaPHYAndroid::CaptureThread complete");

    return 0;
}

int TolkaPHYAndroid::statusThread()
{
    _TOLKA_PHY_ANDROID_DEBUG("TolkaPHYAndroid::statusThread: starting with this: %p", this);

    this->pinStatusThreadAsNeeded();
    this->statusThreadIsRunning = true;

    unique_lock<mutex> SL_I2C_command_mutex_tuner_status_io(this->SL_I2C_command_mutex, std::defer_lock);
    unique_lock<mutex> SL_PlpConfigParams_mutex_get_plps(SL_PlpConfigParams_mutex, std::defer_lock);

    SL_Result_t dres = SL_OK;
    SL_Result_t sl_res = SL_OK;
    SL_TunerResult_t tres = SL_TUNER_OK;

    uint lastCpuStatus = 0;

    SL_Atsc3p0PlpConfigParams_t     loop_atsc3ConfigParams = { 0 };
    unsigned long long              llsPlpInfo;

    /* jjustman-2021-06-07 - #11798
     *  int L1bSnrLinearScale;
        int L1dSnrLinearScale;
        int Plp0SnrLinearScale;
        int Plp1SnrLinearScale;
        int Plp2SnrLinearScale;
        int Plp3SnrLinearScale;
        int GlobalSnrLinearScale;
     */
    double snr_global;
    double snr_l1b;
    double snr_l1d;
    double snr_plp[4];

    int ber_l1b;
    int ber_l1d;
    int ber_plp0;

    SL_Atsc3p0L1DPlp_Diag_t myPlps[4];

    atsc3_ndk_phy_client_rf_metrics_t atsc3_ndk_phy_client_rf_metrics = { '0' };

    //wait for demod to come up before polling status
    while(!this->demodStartStatus && this->statusThreadShouldRun) {
        usleep(1000000);
    }

//#define
    while(this->statusThreadShouldRun) {

        //running
        if(lastCpuStatus == 0xFFFFFFFF) {

#if defined(__JJ_DEBUG_BSR_EA_WAKEUP) || defined(__JJ_DEBUG_L1D_TIMEINFO)

            int bsrLoopCount = 0;
            int l1dLoopCount = 0;

            while(true) {

#ifdef __JJ_DEBUG_BSR_EA_WAKEUP
                dres = SL_DemodGetAtsc3p0Diagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_BSR, (SL_Atsc3p0Bsr_Diag_t*)&bsrDiag);
                if (dres != SL_OK) {
                    _SAANKHYA_PHY_ANDROID_ERROR("TolkaPHYAndroid::StatusThread: Error: SL_DemodGetAtsc3p0Diagnostics with SL_DEMOD_DIAG_TYPE_BSR failed, res: %d", dres);
                } else {
                    _TOLKA_PHY_ANDROID_DEBUG("TolkaPHYAndroid::StatusThread: BSR: Bsr1EAWakeup1: %d, Bsr1EAWakeup2: %d", bsrDiag.Bsr1EAWakeup1, bsrDiag.Bsr1EAWakeup2);
                    //jjustman-2021-09-01 - push to phy bridge
                }

                usleep(__JJ_DEBUG_BSR_EA_WAKEUP_USLEEP);

                if(bsrLoopCount++ > __JJ_DEBUG_BSR_EA_WAKEUP_ITERATIONS) {
                    break;
                }
#endif

#ifdef __JJ_DEBUG_L1D_TIMEINFO
                //optional gate check:
                //demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_L1D_LOCK
                dres = SL_DemodGetAtsc3p0Diagnostics(slUnit, SL_DEMOD_DIAG_TYPE_L1B, (SL_Atsc3p0L1B_Diag_t*)&l1bDiag);
                dres = SL_DemodGetAtsc3p0Diagnostics(slUnit, SL_DEMOD_DIAG_TYPE_L1D, (SL_Atsc3p0L1D_Diag_t*)&l1dDiag);
                printToConsoleAtsc3L1dDiagnostics(l1dDiag);

                _TOLKA_PHY_ANDROID_DEBUG("TolkaPHYAndroid::StatusThread: L1time: flag: %d, s: %d, ms: %d, us: %d, ns: %d",
                                            l1bDiag.L1bTimeInfoFlag, l1dDiag.l1dGlobalParamsStr.L1dTimeSec, l1dDiag.l1dGlobalParamsStr.L1dTimeMsec, l1dDiag.l1dGlobalParamsStr.L1dTimeUsec, l1dDiag.l1dGlobalParamsStr.L1dTimeNsec);

                if(atsc3_ndk_phy_bridge_get_instance()) {
                    atsc3_ndk_phy_bridge_get_instance()->atsc3_update_l1d_time_information( l1bDiag.L1bTimeInfoFlag, l1dDiag.l1dGlobalParamsStr.L1dTimeSec, l1dDiag.l1dGlobalParamsStr.L1dTimeMsec, l1dDiag.l1dGlobalParamsStr.L1dTimeUsec, l1dDiag.l1dGlobalParamsStr.L1dTimeNsec);
                }

                usleep(__JJ_DEBUG_L1D_TIMEINFO_USLEEP);

                if(l1dLoopCount++ > __JJ_DEBUG_L1D_TIMEINFO_DEMOD_GET_ITERATIONS) {
                    break;
                }

#endif

#ifdef __JJ_DEBUG_L1D_TIMEINFO_CYCLE_STOP_START_DEMOD
    slres = SL_DemodStop(slUnit);
    _TOLKA_PHY_ANDROID_DEBUG("after SL_DemodStop, count: %d, slRes: %d", l1dLoopCount);
    usleep(__JJ_DEBUG_L1D_TIMEINFO_USLEEP);
    slres = SL_DemodStart(slUnit);
    _TOLKA_PHY_ANDROID_DEBUG("after SL_DemodStart, count: %d, slRes: %d", l1dLoopCount);
    usleep(__JJ_DEBUG_L1D_TIMEINFO_USLEEP);
#endif
            }
#else

            //2022-03-30 - updated to 10s...hack testing for 256QAM 11/15
            usleep(2000000);
            //jjustman: target: sleep for 500ms
            //TODO: jjustman-2019-12-05: investigate FX3 firmware and i2c single threaded interrupt handling instead of dma xfer
#endif

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
        /*
         * jjustman-2021-03-16 - to restart loop, be sure to use:
         *          goto sl_i2c_tuner_mutex_unlock;
         * rather than continue; as we need to release this mutex for other threads to (possibly) access i2c
         */

        SL_I2C_command_mutex_tuner_status_io.lock();

        //PLP info we will use for this stats iteration
        SL_PlpConfigParams_mutex_get_plps.lock();
        loop_atsc3ConfigParams.plp0 = atsc3ConfigInfo.plpConfig.plp0;
        loop_atsc3ConfigParams.plp1 = atsc3ConfigInfo.plpConfig.plp1;
        loop_atsc3ConfigParams.plp2 = atsc3ConfigInfo.plpConfig.plp2;
        loop_atsc3ConfigParams.plp3 = atsc3ConfigInfo.plpConfig.plp3;
        SL_PlpConfigParams_mutex_get_plps.unlock();

        //if this is our first loop after a Tune() command has completed, dump SL_DemodGetConfiguration
        if(statusThreadFirstLoopAfterTuneComplete) {
            SL_SleepMS(250); // Delay to accomdate set configurations at SL to take effect
            statusThreadFirstLoopAfterTuneComplete = false;

            slres = SL_DemodGetConfiguration(slUnit, &cfgInfo);
            if (slres != SL_OK)
            {
                _TOLKA_PHY_ANDROID_ERROR("SL_TunerGetStatus error:  %d", slres);
                //printToConsoleDemodError("SL_TunerGetStatus", slres);
                if (slres == SL_ERR_CMD_IF_FAILURE)
                {
                    //handleCmdIfFailure();
                    goto sl_i2c_tuner_mutex_unlock;
                }
            }
            else
            {
                //printToConsoleDemodConfiguration(cfgInfo);
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
            _TOLKA_PHY_ANDROID_ERROR("Error:SL_Demod Get CPU Status: dres: %d", dres);
            goto sl_i2c_tuner_mutex_unlock;
        }
        lastCpuStatus = cpuStatus;
        //jjustman-2021-05-11 - give 256qam 11/15 fec bitrates a chance to flush ALP buffer without oveflowing and lose bootstrap/l1b/l1d lock

#ifdef _JJ_I2C_TUNER_STATUS_THREAD_SLEEP_MS_ENABLED_
        SL_SleepMS(10);
#endif


        //jjustman-2020-10-14 - not really worth it on AA as we don't get rssi here
//        tres = SL_TunerGetStatus(this->tUnit, &tunerInfo);
//        if (tres != SL_TUNER_OK) {
//            _SAANKHYA_PHY_ANDROID_ERROR("Error:SL_TunerGetStatus: tres: %d", tres);
//            goto sl_i2c_tuner_mutex_unlock;
//        }

#ifdef _JJ_I2C_TUNER_STATUS_THREAD_SLEEP_MS_ENABLED_
        SL_SleepMS(10);
#endif

    //    atsc3_ndk_phy_client_rf_metrics.tuner_lock = (tunerInfo.status == 1);

        //jjustman-2022-03-30 - this call may cause the demod to hang with 256QAM 11/15
        //important, we should only query BSR, L1B, and L1D Diag data after each relevant lock has been acquired to prevent i2c bus txns from crashing the demod...
        dres = SL_DemodGetStatus(this->slUnit, SL_DEMOD_STATUS_TYPE_LOCK, (SL_DemodLockStatus_t*)&demodLockStatus);
        if (dres != SL_OK) {
            _TOLKA_PHY_ANDROID_ERROR("Error:SL_Demod Get Lock Status  : dres: %d", dres);
            goto sl_i2c_tuner_mutex_unlock;
        }

#ifdef _JJ_I2C_TUNER_STATUS_THREAD_SLEEP_MS_ENABLED_
        SL_SleepMS(10);
#endif

        atsc3_ndk_phy_client_rf_metrics.demod_lock = demodLockStatus;

        atsc3_ndk_phy_client_rf_metrics.plp_lock_any =  (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP0_LOCK) ||
                                                        (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP1_LOCK) ||
                                                        (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP2_LOCK) ||
                                                        (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP3_LOCK);


        atsc3_ndk_phy_client_rf_metrics.plp_lock_all =  (loop_atsc3ConfigParams.plp0 != 0xFF && (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP0_LOCK)) &&
                                                        (loop_atsc3ConfigParams.plp1 != 0xFF && (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP1_LOCK)) &&
                                                        (loop_atsc3ConfigParams.plp2 != 0xFF && (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP2_LOCK)) &&
                                                        (loop_atsc3ConfigParams.plp3 != 0xFF && (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP3_LOCK));

        //we have RF / Bootstrap lock
        if(demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_RF_LOCK) {
            if(!statusThreadFirstLoopAfterTuneComplete_HasBootstrapLock_for_BSR_Diag) {
                statusThreadFirstLoopAfterTuneComplete_HasBootstrapLock_for_BSR_Diag = true;

                dres = SL_DemodGetDiagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_BSR, (SL_Atsc3p0Bsr_Diag_t*)&bsrDiag);
                if (dres != SL_OK) {
                    _TOLKA_PHY_ANDROID_ERROR("Error: SL_DemodGetDiagnostics with SL_DEMOD_DIAG_TYPE_BSR failed, res: %d", dres);
                    goto sl_i2c_tuner_mutex_unlock;
                }

                //printAtsc3BsrDiagnostics(bsrDiag, 0);
            }
        }

        //we have L1B_Lock
        if(demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1B_LOCK) {

            if(!statusThreadFirstLoopAfterTuneComplete_HasL1B_DemodLock_for_L1B_Diag) {
                statusThreadFirstLoopAfterTuneComplete_HasL1B_DemodLock_for_L1B_Diag = true;

                dres = SL_DemodGetDiagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_L1B, (SL_Atsc3p0L1B_Diag_t*)&l1bDiag);
                if (dres != SL_OK) {
                    _TOLKA_PHY_ANDROID_ERROR("Error: SL_DemodGetDiagnostics with SL_DEMOD_DIAG_TYPE_L1B failed, res: %d", dres);
                    goto sl_i2c_tuner_mutex_unlock;
                }

                //printAtsc3L1bDiagnostics(l1bDiag, 0);

                //jjustman-2021-10-24 - keep track of our L1bTimeInfoFlag
                last_l1bTimeInfoFlag = l1bDiag.L1bTimeInfoFlag;
            }
        }

        //we have L1D_Lock
        if(demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1D_LOCK) {
            if(!statusThreadFirstLoopAfterTuneComplete_HasL1D_DemodLock_for_L1D_Diag) {
                statusThreadFirstLoopAfterTuneComplete_HasL1D_DemodLock_for_L1D_Diag = true;

                dres = SL_DemodGetDiagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_L1D, (SL_Atsc3p0L1D_Diag_t*)&l1dDiag);
                if (dres != SL_OK) {
                    _TOLKA_PHY_ANDROID_ERROR("Error: SL_DemodGetAtsc3p0Diagnostics with SL_DEMOD_DIAG_TYPE_L1D failed, res: %d", dres);
                    goto sl_i2c_tuner_mutex_unlock;
                }

                //printAtsc3L1dDiagnostics(l1bDiag.L1bNoOfSubframes, l1dDiag, 0);
                //printAtsc3SignalDetails(l1bDiag.L1bNoOfSubframes, l1dDiag, 0);
            }
        }

//#define _JJ_DISABLE_PLP_SNR
        //we need this for SNR
#ifndef _JJ_DISABLE_PLP_SNR
        dres = SL_DemodGetDiagnostics(this->slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_PERF, (SL_Atsc3p0Perf_Diag_t*)&perfDiag);
        if (dres != SL_OK) {
            _TOLKA_PHY_ANDROID_ERROR("Error getting ATSC3.0 Performance Diagnostics : dres: %d", dres);
            goto sl_i2c_tuner_mutex_unlock;
        }
#endif
        //jjustman-2021-05-11 - TODO: only run this at PLP selection lock, e.g.:
        //  for each PLPne.g. demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_BB_PLPn_LOCK) != 0;
        //
        //        sl_res = SL_DemodGetLlsPlpList(this->slUnit, &llsPlpInfo);
        //        if (sl_res != SL_OK) {
        //            _SAANKHYA_PHY_ANDROID_ERROR("Error:SL_DemodGetLlsPlpList : sl_res: %d", sl_res);
        //            goto sl_i2c_tuner_mutex_unlock;
        //        }

        //jjustman-2021-03-16 - exit our i2c critical section while we build and push our PHY statistics, we can use "continue" for next loop iteration after this point
        SL_I2C_command_mutex_tuner_status_io.unlock();

        //jjustman-2021-06-08 - for debugging purposes only
#ifdef _JJ_TUNER_STATUS_THREAD_PRINT_PERF_DIAGNOSTICS_ENABLED_
        printAtsc3PerfDiagnostics(perfDiag, 0);
#endif


        atsc3_ndk_phy_client_rf_metrics.cpu_status = (cpuStatus == 0xFFFFFFFF); //0xFFFFFFFF -> running -> 1 to jni layer
        snr_global = compute_snr(perfDiag.GlobalSnrLinearScale);
        atsc3_ndk_phy_client_rf_metrics.snr1000_global = snr_global;

        snr_l1b = compute_snr(perfDiag.L1bSnrLinearScale);
        atsc3_ndk_phy_client_rf_metrics.snr1000_l1b = snr_l1b;

        snr_l1d = compute_snr(perfDiag.L1dSnrLinearScale);
        atsc3_ndk_phy_client_rf_metrics.snr1000_l1d = snr_l1d;

        snr_plp[0] = compute_snr(perfDiag.Plp0SnrLinearScale);
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].snr1000 = snr_plp[0];

        snr_plp[1] = compute_snr(perfDiag.Plp1SnrLinearScale);
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].snr1000 = snr_plp[1];

        snr_plp[2] = compute_snr(perfDiag.Plp2SnrLinearScale);
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].snr1000 = snr_plp[2];

        snr_plp[3] = compute_snr(perfDiag.Plp3SnrLinearScale);
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].snr1000 = snr_plp[3];

//        //sanjay
//        if (kailash_3_rssi == true) {
//            atsc3_ndk_phy_client_rf_metrics.rssi_1000 = ((tunerInfo.signalStrength * 1000) -
//                                                         (256000));
//        } else {
//            atsc3_ndk_phy_client_rf_metrics.rssi_1000 = tunerInfo.signalStrength * 1000;
//        }

        //jjustman-2021-05-11 - fixme to just be perfDiag values
        ber_l1b = perfDiag.NumBitErrL1b; //(float)perfDiag.NumBitErrL1b / perfDiag.NumFecBitsL1b; // //aBerPreLdpcE7,
        ber_l1d = perfDiag.NumBitErrL1b; //(float) perfDiag.NumBitErrL1d / perfDiag.NumFecBitsL1d;//aBerPreBchE9,
        ber_plp0 = perfDiag.NumBitErrPlp0;// (float)perfDiag.NumBitErrPlp0 / perfDiag.NumFecBitsPlp0; //aFerPostBchE6,

        //build our listen plp details

        memset(&myPlps[0], 0, sizeof(SL_Atsc3p0L1DPlp_Diag_t));
        memset(&myPlps[1], 0, sizeof(SL_Atsc3p0L1DPlp_Diag_t));
        memset(&myPlps[2], 0, sizeof(SL_Atsc3p0L1DPlp_Diag_t));
        memset(&myPlps[3], 0, sizeof(SL_Atsc3p0L1DPlp_Diag_t));

//L1dSfNumPlp2Decode
        for(int subframeIdx = 0; subframeIdx <= l1bDiag.L1bNoOfSubframes; subframeIdx++) {
            for(int plpIdx = 0; plpIdx < (0xFF & l1dDiag.sfParams[subframeIdx].L1dSfNumPlp2Decode); plpIdx++) {

                if(loop_atsc3ConfigParams.plp0 == l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx].L1dSfPlpId) {
                    myPlps[0] = l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx];
                } else if(loop_atsc3ConfigParams.plp1 == l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx].L1dSfPlpId) {
                    myPlps[1] = l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx];
                } else if(loop_atsc3ConfigParams.plp2 == l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx].L1dSfPlpId) {
                    myPlps[2] = l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx];
                } else if(loop_atsc3ConfigParams.plp3 == l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx].L1dSfPlpId) {
                    myPlps[3] = l1dDiag.sfParams[subframeIdx].PlpParams[plpIdx];
                }
            }
        }

        //plp[0]
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].plp_id          = loop_atsc3ConfigParams.plp0;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].modcod_valid    = (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP0_LOCK) != 0;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].plp_fec_type    = myPlps[0].L1dSfPlpFecType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].plp_mod         = myPlps[0].L1dSfPlpModType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].plp_cod         = myPlps[0].L1dSfPlpCoderate;

        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].ber_pre_ldpc    = perfDiag.LdpcItrnsPlp0; // over ???//BER x1e7
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].ber_pre_bch     = perfDiag.NumBitErrPlp0; //(perfDiag.NumBitErrPlp0 * 1000000000) / (perfDiag.Plp0StreamByteCount * 8); //s_fe_detail.aBerPreBchE9[i]; //BER 1xe9
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].fer_post_bch    = perfDiag.NumFecBitsPlp0; //(perfDiag.NumFrameErrPlp0 * 1000000) / perfDiag.NumFecFramePlp0;  //FER 1xe6
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].total_fec       = perfDiag.NumFecFramePlp0;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].total_error_fec = perfDiag.NumFrameErrPlp0;

        //plp[1]
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].plp_id          = loop_atsc3ConfigParams.plp1;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].modcod_valid    = (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP1_LOCK) != 0;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].plp_fec_type    = myPlps[1].L1dSfPlpFecType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].plp_mod         = myPlps[1].L1dSfPlpModType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].plp_cod         = myPlps[1].L1dSfPlpCoderate;

        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].ber_pre_ldpc    = perfDiag.LdpcItrnsPlp1; // over ???//BER x1e7
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].ber_pre_bch     = perfDiag.NumBitErrPlp1; //(perfDiag.NumBitErrPlp1 * 1000000000) / (perfDiag.Plp1StreamByteCount * 8); //s_fe_detail.aBerPreBchE9[i]; //BER 1xe9
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].fer_post_bch    = perfDiag.NumFecBitsPlp1; //(perfDiag.NumFrameErrPlp1 * 1000000) / perfDiag.NumFecFramePlp1;  //FER 1xe6
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].total_fec       = perfDiag.NumFecFramePlp1;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].total_error_fec = perfDiag.NumFrameErrPlp1;
        //plp[2]
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].plp_id          = loop_atsc3ConfigParams.plp2;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].modcod_valid    = (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP2_LOCK) != 0;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].plp_fec_type    = myPlps[2].L1dSfPlpFecType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].plp_mod         = myPlps[2].L1dSfPlpModType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].plp_cod         = myPlps[2].L1dSfPlpCoderate;

        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].ber_pre_ldpc    = perfDiag.LdpcItrnsPlp2; // over ???//BER x1e7
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].ber_pre_bch     = perfDiag.NumBitErrPlp2; //(perfDiag.NumBitErrPlp2 * 1000000000) / (perfDiag.Plp2StreamByteCount * 8); //s_fe_detail.aBerPreBchE9[i]; //BER 1xe9
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].fer_post_bch    = perfDiag.NumFecBitsPlp2; //(perfDiag.NumFrameErrPlp2 * 1000000) / perfDiag.NumFecFramePlp2;  //FER 1xe6
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].total_fec       = perfDiag.NumFecFramePlp2;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].total_error_fec = perfDiag.NumFrameErrPlp2;

        //plp[3]
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].plp_id          = loop_atsc3ConfigParams.plp3;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].modcod_valid    = (demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP3_LOCK) != 0;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].plp_fec_type    = myPlps[3].L1dSfPlpFecType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].plp_mod         = myPlps[3].L1dSfPlpModType;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].plp_cod         = myPlps[3].L1dSfPlpCoderate;

        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].ber_pre_ldpc    = perfDiag.LdpcItrnsPlp3; // over ???//BER x1e7
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].ber_pre_bch     = perfDiag.NumBitErrPlp3; //(perfDiag.NumBitErrPlp3 * 1000000000) / (perfDiag.Plp3StreamByteCount * 8); //s_fe_detail.aBerPreBchE9[i]; //BER 1xe9
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].fer_post_bch    = perfDiag.NumFecBitsPlp3; //(perfDiag.NumFrameErrPlp3 * 1000000) / perfDiag.NumFecFramePlp3;  //FER 1xe6
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].total_fec       = perfDiag.NumFecFramePlp3;
        atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].total_error_fec = perfDiag.NumFrameErrPlp3;


        _TOLKA_PHY_ANDROID_DEBUG("atsc3NdkClientSlImpl::StatusThread: global_SNR: %f, l1b_SNR: %f, l1d_SNR: %f tunerInfo.status: %d, tunerInfo.signalStrength: %f, cpuStatus: %s, demodLockStatus: %d (RF: %d, L1B: %d, L1D: %d),  ber_l1b: %d, ber_l1d: %d, ber_plp0: %d, plps: 0x%02x (fec: %d, mod: %d, cr: %d, snr: %f), 0x%02x (fec: %d, mod: %d, cr: %d, snr: %f), 0x%02x (fec: %d, mod: %d, cr: %d, snr: %f), 0x%02x (fec: %d, mod: %d, cr: %d, snr: %f)",
                                    snr_global / 1000.0,
                                    snr_l1b / 1000.0,
                                    snr_l1d / 1000.0,

                                    tunerInfo.status,
                                    tunerInfo.signalStrength / 1000,
                                    (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED",
                                    demodLockStatus,
                                    demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_RF_LOCK,
                                    demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1B_LOCK,
                                    demodLockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1D_LOCK,

                                    ber_l1b,
                                    ber_l1d,
                                    ber_plp0,
                                    loop_atsc3ConfigParams.plp0,
                                    myPlps[0].L1dSfPlpFecType,
                                    myPlps[0].L1dSfPlpModType,
                                    myPlps[0].L1dSfPlpCoderate,
                                    atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[0].snr1000 / 1000.0,
                                    loop_atsc3ConfigParams.plp1,
                                    myPlps[1].L1dSfPlpFecType,
                                    myPlps[1].L1dSfPlpModType,
                                    myPlps[1].L1dSfPlpCoderate,
                                    atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[1].snr1000 / 1000.0,
                                    loop_atsc3ConfigParams.plp2,
                                    myPlps[2].L1dSfPlpFecType,
                                    myPlps[2].L1dSfPlpModType,
                                    myPlps[2].L1dSfPlpCoderate,
                                    atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[2].snr1000 / 1000.0,
                                    loop_atsc3ConfigParams.plp3,
                                    myPlps[3].L1dSfPlpFecType,
                                    myPlps[3].L1dSfPlpModType,
                                    myPlps[3].L1dSfPlpCoderate,
                                    atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[3].snr1000 / 1000.0
        );

        if(atsc3_ndk_phy_bridge_get_instance()) {
            atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_stats_from_atsc3_ndk_phy_client_rf_metrics_t(&atsc3_ndk_phy_client_rf_metrics);
            atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_bw_stats(tolkaPHYAndroid->alp_completed_packets_parsed,
                                                                          tolkaPHYAndroid->alp_total_bytes,
                                                                          tolkaPHYAndroid->alp_total_LMTs_recv);
        }

        //we've already unlocked, so don't fall thru
        continue;

        sl_i2c_tuner_mutex_unlock:
        SL_I2C_command_mutex_tuner_status_io.unlock();

        if(global_sl_result_error_flag != SL_OK || global_sl_i2c_result_error_flag != SL_I2C_OK || dres != SL_OK || sl_res != SL_OK || tres != SL_TUNER_OK) {
            if(atsc3_ndk_phy_bridge_get_instance()) {
                atsc3_ndk_phy_bridge_get_instance()->atsc3_notify_phy_error("TolkaPHYAndroid::tunerStatusThread() - ERROR: command failed: global_sl_res: %d, global_sl_i2c_res: %d, dres: %d, sl_res, tres: %d",
                                                                            global_sl_result_error_flag,
                                                                            global_sl_i2c_result_error_flag,
                                                                            dres, sl_res, tres);
            }
        }

    }

    this->releasePinnedStatusThreadAsNeeded();
    this->statusThreadIsRunning = false;
    _TOLKA_PHY_ANDROID_INFO("TolkaPHYAndroid::statusThread complete");

    return 0;
}




double TolkaPHYAndroid::compute_snr(int snr_linear_scale) {
    double snr = (float)snr_linear_scale / 16384;
    snr = 10000.0 * log10(snr); //10

    return snr;
}

void TolkaPHYAndroid::processTLVFromCallback()
{
    int innerLoopCount = 0;
    int bytesRead = 0;

    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex, std::defer_lock);
    //promoted from unique_lock, std::defer_lock to recursive_mutex: atsc3_sl_tlv_block_mutex

    //jjustman-2020-12-07 - loop while we have data in our cb, or break if cb_should_discard is true
    while(true && !TolkaPHYAndroid::cb_should_discard) {
        CircularBufferMutex_local.lock();
        bytesRead = CircularBufferPop(cb, TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE, (char*)&processDataCircularBufferForCallback);
        CircularBufferMutex_local.unlock();

        //jjustman-2021-01-19 - if we don't get any data back, or the cb_should_discard flag is set, bail
        if(!bytesRead || TolkaPHYAndroid::cb_should_discard) {
            break; //we need to issue a matching sl_tlv_block_mutex ublock...
        }

        atsc3_sl_tlv_block_mutex.lock();
        if (!atsc3_sl_tlv_block) {
            _TOLKA_PHY_ANDROID_WARN("ERROR: atsc3NdkClientSlImpl::processTLVFromCallback - atsc3_sl_tlv_block is NULL!");
            allocate_atsc3_sl_tlv_block();
        }

        block_Write(atsc3_sl_tlv_block, (uint8_t *) &processDataCircularBufferForCallback, bytesRead);
        if (bytesRead < TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE) {
            //_SAANKHYA_PHY_ANDROID_TRACE("atsc3NdkClientSlImpl::processTLVFromCallback() - short read from CircularBufferPop, got %d bytes, but expected: %d", bytesRead, TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE);
            //release our recursive lock early as we are bailing this method
            atsc3_sl_tlv_block_mutex.unlock();
            break;
        }

        block_Rewind(atsc3_sl_tlv_block);

        //_SAANKHYA_PHY_ANDROID_DEBUG("atsc3NdkClientSlImpl::processTLVFromCallback() - processTLVFromCallbackInvocationCount: %d, inner loop count: %d, atsc3_sl_tlv_block.p_size: %d, atsc3_sl_tlv_block.i_pos: %d", processTLVFromCallbackInvocationCount, ++innerLoopCount, atsc3_sl_tlv_block->p_size, atsc3_sl_tlv_block->i_pos);

        bool atsc3_sl_tlv_payload_complete = false;

        do {
            atsc3_sl_tlv_payload = atsc3_sl_tlv_payload_parse_from_block_t(atsc3_sl_tlv_block);
            _TOLKA_PHY_ANDROID_TRACE("atsc3NdkClientSlImpl::processTLVFromCallback() - processTLVFromCallbackInvocationCount: %d, inner loop count: %d, atsc3_sl_tlv_block.p_size: %d, atsc3_sl_tlv_block.i_pos: %d, atsc3_sl_tlv_payload: %p",
                                        processTLVFromCallbackInvocationCount, ++innerLoopCount, atsc3_sl_tlv_block->p_size, atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_payload);

            if (atsc3_sl_tlv_payload) {
                atsc3_sl_tlv_payload_dump(atsc3_sl_tlv_payload);

                uint64_t l1dTimeNs_value = atsc3_sl_tlv_payload->l1d_time_sec + (atsc3_sl_tlv_payload->l1d_time_msec * 1000) + (atsc3_sl_tlv_payload->l1d_time_usec * 1000000) + (atsc3_sl_tlv_payload->l1d_time_nsec * 1000000000) ;

                //jjustman-2021-10-24 - hack-ish to push our l1d time info - still needed?
                //true ||
                if(last_l1dTimeNs_value != l1dTimeNs_value) {
                    _TOLKA_PHY_ANDROID_TRACE("atsc3NdkClientSlImpl::processTLVFromCallback() L1DTimeInfo is: L1time: flag: %d, s: %d, ms: %d, us: %d, ns: %d, current l1dTimeNs: %d, last_l1dTimeNs_value: %d, frame duration: %",
                                                last_l1bTimeInfoFlag,
                                                atsc3_sl_tlv_payload->l1d_time_sec, atsc3_sl_tlv_payload->l1d_time_msec, atsc3_sl_tlv_payload->l1d_time_usec, atsc3_sl_tlv_payload->l1d_time_nsec,
                                                l1dTimeNs_value,
                                                last_l1dTimeNs_value
                    );

                    if (atsc3_ndk_phy_bridge_get_instance()) {
                        atsc3_ndk_phy_bridge_get_instance()->atsc3_update_l1d_time_information(last_l1bTimeInfoFlag, atsc3_sl_tlv_payload->l1d_time_sec, atsc3_sl_tlv_payload->l1d_time_msec, atsc3_sl_tlv_payload->l1d_time_usec, atsc3_sl_tlv_payload->l1d_time_nsec);
                    }
                    last_l1dTimeNs_value = l1dTimeNs_value;
                }


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
                    continue;

                } else {

                    atsc3_sl_tlv_payload_free(&atsc3_sl_tlv_payload);
                    atsc3_sl_tlv_payload_complete = false;
                    break; //jjustman-2021-05-04 - gross, i know...
                }
            }

            if(atsc3_sl_tlv_block->i_pos > (atsc3_sl_tlv_block->p_size - 188)) {
                atsc3_sl_tlv_payload_complete = false;
            } else {
                atsc3_sl_tlv_payload_complete = true;
            }
            //jjustman-2019-12-29 - noisy, TODO: wrap in __DEBUG macro check
            //_SAANKHYA_PHY_ANDROID_DEBUG("ERROR: alp_payload IS NULL, short TLV read?  at atsc3_sl_tlv_block: i_pos: %d, p_size: %d", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);

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
            _TOLKA_PHY_ANDROID_WARN("atsc3_sl_tlv_block: positioning mismatch: i_pos: %d, p_size: %d - rewinding and seeking for magic packet?", atsc3_sl_tlv_block->i_pos, atsc3_sl_tlv_block->p_size);

            //jjustman: 2019-11-23: rewind in order to try seek for our magic packet in the next loop here
            block_Rewind(atsc3_sl_tlv_block);
        }
        //unlock for our inner loop
        atsc3_sl_tlv_block_mutex.unlock();
    }
    //atsc3_sl_tlv_block_mutex is now unlocked from either bytesRead < TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE _or_ 2 lines above...
}


void TolkaPHYAndroid::RxDataCallback(unsigned char *data, long len) {
    if(TolkaPHYAndroid::cb_should_discard) {
        return;
    }

    //_SAANKHYA_PHY_ANDROID_DEBUG("atsc3NdkClientSlImpl::RxDataCallback: pushing data: %p, len: %d", data, len);
    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);
    if(TolkaPHYAndroid::cb) {
        CircularBufferPush(TolkaPHYAndroid::cb, (char *) data, len);
    }
    CircularBufferMutex_local.unlock();
}