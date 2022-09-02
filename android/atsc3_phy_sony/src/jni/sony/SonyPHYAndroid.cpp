//
// Created by Jason Justman on 08/11/22
//

#include "SonyPHYAndroid.h"
//jjustman-2022-05-24 - todo: use jni ref resolution for this ptr
SonyPHYAndroid* sonyPHYAndroid = nullptr;
CircularBuffer SonyPHYAndroid::cb_tlv = nullptr;

libusb_device_handle* SonyPHYAndroid::Libusb_device_handle = nullptr;
long SonyPHYAndroid::ITE_93x_OPEN_CLEANUP_FROM_ERROR_LAST_REBOOT_TIMESTAMP = 0;

int SonyPHYAndroid::SONY_USB_ENDPOINT_RX = -1;
int SonyPHYAndroid::SONY_USB_ENDPOINT_TX = -1;
int SonyPHYAndroid::SONY_USB_ENDPOINT_RX_TS = -1;

volatile bool SonyPHYAndroid::captureThreadShouldRun = false;

mutex SonyPHYAndroid::CircularBufferMutex;

mutex SonyPHYAndroid::CS_global_mutex;
atomic_bool SonyPHYAndroid::cb_should_discard;

int SonyPHYAndroid::Last_tune_freq = -1;

int _SONY_PHY_ANDROID_INFO_ENABLED = 1;
int _SONY_PHY_ANDROID_DEBUG_ENABLED = 0;
int _SONY_PHY_ANDROID_TRACE_ENABLED = 0;

//sony methods

SonyPHYAndroid::SonyPHYAndroid(JNIEnv* env, jobject jni_instance) {
    this->env = env;
    this->jni_instance_globalRef = this->env->NewGlobalRef(jni_instance);
    this->setRxUdpPacketProcessCallback(atsc3_core_service_bridge_process_packet_from_plp_and_block);
    this->setRxLinkMappingTableProcessCallback(atsc3_phy_jni_bridge_notify_link_mapping_table);

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->atsc3_phy_notify_plp_selection_change_set_callback(&SonyPHYAndroid::NotifyPlpSelectionChangeCallback, this);
    }

    _SONY_PHY_ANDROID_INFO("SonyPHYAndroid::SonyPHYAndroid - created with this: %p", this);
    SonyPHYAndroid::cb_should_discard = false;

    dev = (IT930x_Device*)calloc(1, sizeof(IT930x_Device));
    dev->DC = (Device_Context*)calloc(1, sizeof(Device_Context));

    DC = dev->DC;

    for (int i = 0; i < 16; i++)
        dev->minor[i] = 0;

    dev->DC = DC;
//    dev->interface = intf;
    dev->DealWith_ReturnChannelPacket = 0;
    //mutex_init(&dev->DC->dev_mutex);
  //  DC->usb_dev = interface_to_usbdev(intf);
    DC->it9300.userData = DC;
    //atomic_set(&dev->DC->filter_count, 0);
    DC->it9300.ctrlBus = BUS_USB;	///for read eeprom
    DC->it9300.maxBusTxSize = 63;	///for read eeprom
    DC->it9300.usbbus_timeout = 500;
    DC->rx_number = 0;
    DC->map_enalbe = false;
    DC->hihg_byte = 0;
    DC->low_byte = 0;
    DC->if_degug = 0;

    BrUser_createCriticalSection();
}

SonyPHYAndroid::~SonyPHYAndroid() {

    _SONY_PHY_ANDROID_INFO("SonyPHYAndroid::~SonyPHYAndroid - enter: deleting with this: %p", this);

    this->stop();

    BrUser_deleteCriticalSection();
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

    if(cb_tlv) {
        CircularBufferFree(cb_tlv);
    }
    cb_tlv = nullptr;

    CircularBufferMutex_local.unlock();

    if(DC) {
        freeclean((void**)&DC);
    }

    if(dev) {
        freeclean((void**)&dev);
    }
    if(Libusb_device_handle) {
        libusb_reset_device(Libusb_device_handle);
        libusb_close(Libusb_device_handle);
        Libusb_device_handle = nullptr;
        libusb_exit(NULL);
    }

    _SONY_PHY_ANDROID_INFO("SonyPHYAndroid::~SonyPHYAndroid - exit: deleting with this: %p", this);
}

int  SonyPHYAndroid::init() {
    //jj: todo
    return -1;
}
int  SonyPHYAndroid::run() {
    //jj: todo
    return -1;
};
bool SonyPHYAndroid::is_running() {
    //jj: todo
    return false;
};

int SonyPHYAndroid::stop() {

    _SONY_PHY_ANDROID_INFO("SonyPHYAndroid::stop: enter with this: %p, captureThreadIsRunning: %d, processThreadIsRunning: %d, processAlpThreadIsRunning:%d, statusThreadIsRunning: %d",
                               this,
                               this->captureThreadIsRunning,
                               this->processThreadIsRunning,
                               this->processAlpThreadIsRunning,
                               this->statusThreadIsRunning);

    SonyPHYAndroid::cb_should_discard = true;

    statusThreadShouldRun = false;
    captureThreadShouldRun = false;
    processThreadShouldRun = false;
    processAlpThreadShouldRun = false;

    //tear down status thread first, as its the most 'problematic' with the Sony i2c i/f processing
    while(this->statusThreadIsRunning) {
        usleep(1000);
        _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: this->statusThreadIsRunning: %d", this->statusThreadIsRunning);
    }

    _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: before join for statusThreadHandle");
    if(statusThreadHandle.joinable()) {
        statusThreadHandle.join();
    }
    _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: after join for statusThreadHandle");

    while(this->captureThreadIsRunning) {
        usleep(1000);
        _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: this->captureThreadIsRunning: %d", this->captureThreadIsRunning);
    }

    _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: before join for captureThreadHandle");
    if(captureThreadHandle.joinable()) {
        captureThreadHandle.join();
    }
    _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: after join for captureThreadHandle");

    if(processThreadIsRunning) {
        _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: setting processThreadShouldRun: false");
        while(this->processThreadIsRunning) {
            usleep(1000);
            _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: this->processThreadIsRunning: %d", this->processThreadIsRunning);
        }
    }

    _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: before join for processThreadHandle");
    if(processThreadHandle.joinable()) {
        processThreadHandle.join();
    }

    //process ALP thread

    if(processAlpThreadIsRunning) {
        tlv_buffer_queue_for_alp_extraction_notification.notify_one();
        _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: spin on processAlpThreadIsRunning, sending notify_one on tlv_buffer_queue_for_alp_extraction_notification");
        while(this->processAlpThreadIsRunning) {
            usleep(1000);
            _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: this->processAlpThreadIsRunning: %d", this->processAlpThreadIsRunning);
        }
    }

    _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::stop: before join for processThreadHandle");
    if(processAlpThreadHandle.joinable()) {
        processAlpThreadHandle.join();
    }

    _SONY_PHY_ANDROID_INFO("SonyPHYAndroid::stop:exit");

    return 0;
};

int SonyPHYAndroid::deinit() {
    //jj: todo
    return -1;
};

string SonyPHYAndroid::get_sdk_version()  {
    //jj: todo
    return "jj-sony-sdk_version-0.00001";
};
string SonyPHYAndroid::get_firmware_version() {
    //jj: todo
    return "jj-sony-todo-firmware-version";
};

int SonyPHYAndroid::download_bootloader_firmware(int fd, int device_type, string devicePath) {
    //jj: noop
    return -1;
};

int SonyPHYAndroid::open(int fd, int device_type, string devicePath)   {
    int r = -1;

    //check to make sure we aren't re-enumerating from cleanup_from_error ite9300 reboot flow (e.g. to reset for possible other usbphyatsc3 sources that may have same VID/PID for IT93x chip host i/f
    if(SonyPHYAndroid::ITE_93x_OPEN_CLEANUP_FROM_ERROR_LAST_REBOOT_TIMESTAMP) {
        long current_ts = gtl();
        long last_open_delta = current_ts - SonyPHYAndroid::ITE_93x_OPEN_CLEANUP_FROM_ERROR_LAST_REBOOT_TIMESTAMP;

        _SONY_PHY_ANDROID_WARN("open - ts delta is: %ld", last_open_delta);
        if(last_open_delta < 2000) {
            //less than 2s, ignore this re-enumeration
            return -31337;
        }
    }

    libusb_fd = fd;

    r = libusb_init(NULL);
    if(r) {
        _SONY_PHY_ANDROID_ERROR("open - libusb_init returned: %d", r);
        return -1;
    }

    int ret = libusb_wrap_sys_device(NULL, libusb_fd, &Libusb_device_handle);
    if(ret || !Libusb_device_handle) {
        _SONY_PHY_ANDROID_ERROR("open: libusb_wrap_sys_device: failed device from fd: %d, ret is: %d, device_handle: %p", fd, ret, Libusb_device_handle);
        return -1;
    }

    //confirm vid/pid
    struct libusb_device_descriptor d;

    libusb_device *tdev = libusb_get_device(Libusb_device_handle);
    ret = libusb_get_device_descriptor(tdev, &d);
    if(ret) {
        _SONY_PHY_ANDROID_ERROR("open: libusb_get_device_descriptor: failed device from fd: %d, ret is: %d, device_handle: %p", fd, ret, Libusb_device_handle);
        return -1;
    }
    _SONY_PHY_ANDROID_INFO("open: libusb_get_device, handle: %d, vendorId: %d, productId: %d", Libusb_device_handle, d.idVendor, d.idProduct);

    libusb_open(tdev, &Libusb_device_handle);
    if(ret) {
        _SONY_PHY_ANDROID_ERROR("open: libusb_open: failed device from fd: %d, ret is: %d, device_handle: %p", fd, ret, Libusb_device_handle);
        return -1;
    }

    libusb_config_descriptor *configDesc = NULL;
    //claim rx/tx/ts_bulk endpoints
    ret = libusb_get_config_descriptor(tdev, 0, &configDesc);
    if (ret != 0)
    {
        _SONY_PHY_ANDROID_ERROR("open: cyusb_get_config_descriptor failed, ret: %d", ret);
        return -1;
    }
    _SONY_PHY_ANDROID_INFO("total interfaces: %d", configDesc->bNumInterfaces);

    for(int i=0; i < configDesc->bNumInterfaces; i++) {
        _SONY_PHY_ANDROID_INFO("endpoint: %d, claiming", i);
        ret = libusb_claim_interface(Libusb_device_handle, i);
        if(ret) {
            _SONY_PHY_ANDROID_ERROR("open: libusb_claim_interface: failed to claim ep: %d", i);
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
                if (k == 0) { //hack?endpointDesc->bEndpointAddress == SONY_USB_ENDPOINT_RX)
                 //   ret = libusb_set_interface_alt_setting(Libusb_device_handle, i, j);
                    SONY_USB_ENDPOINT_RX = endpointDesc->bEndpointAddress;
                    _SONY_PHY_ANDROID_INFO("libusb_set_interface_alt_setting, SONY_USB_ENDPOINT_RX, ret: %d, interface: %d, endpoint: %d (0x%02x)", ret, i, SONY_USB_ENDPOINT_RX, SONY_USB_ENDPOINT_RX);
                } else if (k == 1) { //hack endpointDesc->bEndpointAddress == SONY_USB_ENDPOINT_TX) {
                //    ret = libusb_set_interface_alt_setting(Libusb_device_handle, i, j);
                    SONY_USB_ENDPOINT_TX = endpointDesc->bEndpointAddress;

                    _SONY_PHY_ANDROID_INFO("libusb_set_interface_alt_setting, SONY_USB_ENDPOINT_TX, ret: %d, interface: %d, endpoint: %d (0x%02x)", ret, i, SONY_USB_ENDPOINT_TX, SONY_USB_ENDPOINT_TX);
                } else if (k == 2) { //hack ->bEndpointAddress == SONY_USB_ENDPOINT_RX_TS) {
                    SONY_USB_ENDPOINT_RX_TS = endpointDesc->bEndpointAddress;
                //    ret = libusb_set_interface_alt_setting(Libusb_device_handle, i, j);
                    _SONY_PHY_ANDROID_INFO("libusb_set_interface_alt_setting, SONY_USB_ENDPOINT_RX_TS, ret: %d, interface: %d, endpoint: %d (0x%02x)", ret, i, SONY_USB_ENDPOINT_RX_TS, SONY_USB_ENDPOINT_RX_TS);
                } else {
                    _SONY_PHY_ANDROID_INFO("unknown interface: %d, endpoint: %d (0x%02x)", i, endpointDesc->bEndpointAddress, endpointDesc->bEndpointAddress);
                }
            }
        }
    }

    /*
     *
     * ep addrs should be:
     *
epRead:   2022-05-24 05:40:25.897 27647-27647/com.example.endeavour_SL3000_R855.debug V/&DC->it9300.Bus: EP(0),
        addr = 0x81, attr = 2, dir = 128, num = 1, intval = 0, maxSize =512
epWrite:  2022-05-24 05:41:07.680 27647-27647/com.example.endeavour_SL3000_R855.debug V/&DC->it9300.Bus: EP(1),
        addr = 0x2, attr = 2, dir = 0, num = 2, intval = 0, maxSize =512
epReadTs: 2022-05-24 05:41:44.177 27647-27647/com.example.endeavour_SL3000_R855.debug V/&DC->it9300.Bus: EP(2),
        addr = 0x84, attr = 2, dir = 128, num = 4, intval = 0, maxSize =512
     */

    _SONY_PHY_ANDROID_INFO("discovered endpoints are: SONY_USB_ENDPOINT_RX: %d (0x%02x), SONY_USB_ENDPOINT_TX: %d (0x%02x), SONY_USB_ENDPOINT_RX_TS: %d (0x%02x)",
                            SONY_USB_ENDPOINT_RX,
                            SONY_USB_ENDPOINT_RX,
                            SONY_USB_ENDPOINT_TX,
                            SONY_USB_ENDPOINT_TX,
                            SONY_USB_ENDPOINT_RX_TS,
                            SONY_USB_ENDPOINT_RX_TS);


    /***
     *
     *
    //Init &DC->it9300
    &DC->it9300.ctrlBus = BUS_USB;
    &DC->it9300.maxBusTxSize = 63;
    &DC->it9300.bypassScript = True;
    &DC->it9300.bypassBoot = False;
    &DC->it9300.chipCount = 1;
    &DC->it9300.gator[0].existed = True;
    &DC->it9300.gator[0].outDataType = OUT_DATA_USB_DATAGRAM;
    &DC->it9300.gator[0].outTsPktLen = PKT_LEN_188;

    //jjustman-2022-05-19: reg_ts0_tag_len - 0 -> 188
    &DC->it9300.tsSource[0][0].existed = True;
    &DC->it9300.tsSource[0][0].tsType  = TS_SERIAL;
    &DC->it9300.tsSource[0][0].i2cAddr = 0x38;
    &DC->it9300.tsSource[0][0].i2cBus  = 3;
    &DC->it9300.tsSource[0][0].tsPort  = TS_PORT_1;
    &DC->it9300.tsSource[0][0].tsPktLen = PKT_LEN_188;

    &DC->it9300.gator[0].booted = False;
    &DC->it9300.gator[0].initialized = False;

    int retVal = 0;

     */

    Byte chip_index = 0, br_idx = 0, ts_idx = 0, valtemp = 0;

    GPIO_STRUCT GpioStruct[4] = { {0xD8CB, 0xD8CC, 0xD8CD},
                                  {0xD8D7, 0xD8D8, 0xD8D9},
                                  {0xD8D3, 0xD8D4, 0xD8D5},
                                  {0xD8DF, 0xD8E0, 0xD8E1} };
    Byte reg_value = 0;


    DC->it9300.usbbus_timeout = 3000;
    //DL_device_communication_test
    internal_device_communication_test(DC);

    //DL_GetEEPROMConfig(DC)
    internal_getEEPROMConfig(DC);

    int error = 0;

    int URB_BUFSIZE = 188 * 816; //32 * 4; //188 * 816

    DC->it9300.it9300user_usb20_frame_size_dw = URB_BUFSIZE / 4;
    _SONY_PHY_ANDROID_INFO("--- URB_BUFSIZE:%u --- \n", URB_BUFSIZE);

    error = IT9300_getFirmwareVersion (&DC->it9300, 0);
    if (error) {
        _SONY_PHY_ANDROID_INFO("IT9300_getFirmwareVersion fail! \n");
        goto exit;
    }

    if (DC->it9300.firmwareVersion != 0) {
        //Switch to TS mode for cleaning PSB buffer
        _SONY_PHY_ANDROID_INFO("--- Clean PSB buffer --- \n");
        error = IT9300_writeRegister ((Endeavour*) &DC->it9300, 0, p_br_mp2if_mpeg_ser_mode, 1);
        if (error)
            goto exit;
        // Reset Rx and Read USB for no data issue
        _SONY_PHY_ANDROID_INFO("--- RESET RX --- \n");
        error = IT9300_writeRegister((Endeavour*) &DC->it9300, 0, p_br_reg_top_gpioh2_en, 0x01);
        if (error)
            goto exit;
        error = IT9300_writeRegister((Endeavour*) &DC->it9300, 0, p_br_reg_top_gpioh2_on, 0x01);
        if (error)
            goto exit;
        error = IT9300_writeRegister((Endeavour*) &DC->it9300, 0, p_br_reg_top_gpioh2_o, 0x0);
        if (error)
            goto exit;

        if (DC->chip_Type[0][0] == EEPROM_MXL691 ||
            DC->chip_Type[0][0] == EEPROM_MXL691_DUALP ||
            DC->chip_Type[0][0] == EEPROM_MXL692 ||
            DC->chip_Type[0][0] == EEPROM_MXL248) {
            /*#define    p_br_reg_top_gpioh9_o	0xD8CB
            #define    p_br_reg_top_gpioh9_en	0xD8CC
            #define    p_br_reg_top_gpioh9_on	0xD8CD

            #define    p_br_reg_top_gpioh10_o	0xD8D7
            #define    p_br_reg_top_gpioh10_en	0xD8D8
            #define    p_br_reg_top_gpioh10_on	0xD8D9

            #define    p_br_reg_top_gpioh11_o	0xD8D3
            #define    p_br_reg_top_gpioh11_en	0xD8D4
            #define    p_br_reg_top_gpioh11_on	0xD8D5

            #define    p_br_reg_top_gpioh12_o	0xD8DF
            #define    p_br_reg_top_gpioh12_en	0xD8E0
            #define    p_br_reg_top_gpioh12_on	0xD8E1*/
            _SONY_PHY_ANDROID_INFO("--- RESET MXL69X RX --- \n");

            for (int i = 0; i < 4; i++) {
                // Reset MXL69X
                // default reset 4 devices
                error = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, GpioStruct[i].GpioEn, 1);
                if (error)
                    goto exit;
                error = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, GpioStruct[i].GpioOn, 1);
                if (error)
                    goto exit;
                error = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, GpioStruct[i].GpioOut, 0);
                if (error)
                    goto exit;
            }

        }
        msleep(50);
    }

    ///DL_IT930x_device_init(DC);



    //Device Init - from DRV_IT930x_device_init

    //with single it930x host i/f, call DRV_IT930x_Initialize - ish...
    error = internal_it930x_initialize(DC, 0);
    if(error) {
        _SONY_PHY_ANDROID_ERROR("internal_it930x_initialize failed!")
        return -1;
    }

    //configure demod silicon


    for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
        for (ts_idx = 0; ts_idx < DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {

            _SONY_PHY_ANDROID_ERROR("calling init for 0x%02x", DC->chip_Type[br_idx][ts_idx]);

            switch (DC->chip_Type[br_idx][ts_idx]) {
                case EEPROM_IT913X: //IT913x
                    error = DRV_IT913x_Initialize(DC, br_idx, ts_idx);
                    break;
                case EEPROM_Si2168B: //Si2168B
                    error = DRV_Si2168B_Initialize(DC, br_idx, ts_idx);
                    break;
                case EEPROM_Dibcom9090: //DIB9090
                    error = DRV_Dib9090_Initialize(DC, EEPROM_Dibcom9090);
                    break;
                case EEPROM_Dibcom9090_4chan: //DIB9090_4ch
                    error = DRV_Dib9090_Initialize(DC, EEPROM_Dibcom9090_4chan);
                    break;
                case EEPROM_MXL691: //MXL69x ATSC, Sean
                    error = DRV_MXL69X_Initialize(DC, br_idx, ts_idx, EEPROM_MXL691);
                    break;
                case EEPROM_MXL691_DUALP:
                    error = DRV_MXL69X_Initialize(DC, br_idx, ts_idx, EEPROM_MXL691_DUALP);
                    break;
                case EEPROM_MXL692:
                    error = DRV_MXL69X_Initialize(DC, br_idx, ts_idx, EEPROM_MXL692);
                    break;
                case EEPROM_MXL248:
                    error = DRV_MXL69X_Initialize(DC, br_idx, ts_idx, EEPROM_MXL248);
                    break;
                case EEPROM_CXD285X: //Sony CXD285X
                    error = DRV_CXD285X_Initialize(DC, br_idx, ts_idx, EEPROM_CXD285X);
                    break;
                case EEPROM_CXD6801_NUVYYO: //Sony CXD6801
                    error = DRV_CXD6801_initialize(DC, br_idx, ts_idx, EEPROM_CXD6801_NUVYYO);
                    break;
                case EEPROM_CXD6801_EVB: //Sony CXD6801
                    error = DRV_CXD6801_initialize(DC, br_idx, ts_idx, EEPROM_CXD6801_EVB);
                    break;
                case EEPROM_CXD6801_TS7: //Sony CXD6801
                    error = DRV_CXD6801_initialize(DC, br_idx, ts_idx, EEPROM_CXD6801_TS7);
                    break;
                case EEPROM_EW100: //Sony CXD2880
                    error = DRV_CXD2880_initialize(DC, br_idx, ts_idx, EEPROM_EW100);
                    break;
                case EEPROM_CXD2880: //Sony CXD2880
                    error = DRV_CXD2880_initialize(DC, br_idx, ts_idx, EEPROM_CXD2880);
                    break;
                case EEPROM_CXD2880_33: //Sony CXD2880
                    error = DRV_CXD2880_initialize(DC, br_idx, ts_idx, EEPROM_CXD2880_33);
                    break;
                case EEPROM_MXL541C:
                    error = DRV_MXL541C_Initialize(DC, br_idx, ts_idx, EEPROM_MXL541C);
                    break;
                default:
                    _SONY_PHY_ANDROID_INFO("Unknown chip type [%d]\n", DC->chip_Type[br_idx][ts_idx]);
                    break;
            }

            if (error) {
                _SONY_PHY_ANDROID_INFO("Tuner Initialize Fail [0x%08lx]\n", error);
                DC->is_rx_init[br_idx][ts_idx] = 0;
                if (DC->chip_Type[br_idx][ts_idx] != EEPROM_IT913X)
					goto cleanup_from_error;
            } else
                DC->is_rx_init[br_idx][ts_idx] = 1;
        }
    }

    ///====== Set In SyncByte & TsType ======

    for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
        for (ts_idx = 0; ts_idx < DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
            ///Set SyncByte (0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77, 0x87)
            //DC->it9300.tsSource[br_idx][ts_idx].syncByte = 0x17 + ((br_idx * 4) + ts_idx) * 16;
            ///Set SyncByte (0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47)

            //jj - hack to get back to 0x47?
//            DC->it9300.tsSource[br_idx][ts_idx].syncByte = 0x40 + ((br_idx * 4) + ts_idx);
            DC->it9300.tsSource[br_idx][ts_idx].syncByte = 0x47; // + ((br_idx * 4) + ts_idx);

//#ifdef PROBE_DEBUG_MESSAGE
            _SONY_PHY_ANDROID_INFO("syncByte = 0x%x \n", DC->it9300.tsSource[br_idx][ts_idx].syncByte);
//#endif
            error = IT9300_setInTsType((Endeavour*) &DC->it9300, br_idx, ts_idx);
            if (error) {
                _SONY_PHY_ANDROID_INFO("%d, %d Ts Source Set in TsType error!\n", br_idx, ts_idx);
                return (error);
            } else {
//#ifdef PROBE_DEBUG_MESSAGE
                _SONY_PHY_ANDROID_INFO("%d, %d Ts Source Set in TsType OK!\n", br_idx, ts_idx);
//#endif
            }
//            _SONY_PHY_ANDROID_INFO
//            error = IT9300_setSyncByteMode(&DC->it9300, br_idx, ts_idx);
//            if (error)  {
//                _SONY_PHY_ANDROID_INFO("%d it9300, %d Ts Source set syncByte error!\n", br_idx, ts_idx);
//                return (error);
//            } else {
//#ifdef PROBE_DEBUG_MESSAGE
//                _SONY_PHY_ANDROID_INFO("%d it9300, %d Ts Source set syncByte OK!\n", br_idx, ts_idx);
//#endif
//            }


            //jjustman-2022-08-12 -
            //ts0_tei_modify?
            error = IT9300_writeRegister (&DC->it9300, 0, p_br_mp2if_ts0_tei_modify, 0);//0:tag 1:sync  2:remap
            if(error) {
                _SONY_PHY_ANDROID_ERROR("IT9300 set ignore fail pin failed, error = 0x%lx", error);
                return -1;
            }

//            //Ignore TS fail pin
//            //0x1F
            error = IT9300_writeRegister(&DC->it9300, 0, p_br_reg_ts_fail_ignore, 0x1F); //necessary
            if(error) {
                _SONY_PHY_ANDROID_ERROR("IT9300 set ignore fail pin failed, error = 0x%lx", error);
                return -1;
            }

//
            //jjustman-2022-08-12 - testing
            //set aggre
            error = IT9300_writeRegister (&DC->it9300, 0, p_br_reg_ts0_aggre_mode, 0);//0:tag 1:sync  2:remap
            if (error) goto exit;
//

//            return (IT9300_writeRegister (&DC->it9300, 0, p_br_reg_null_mode, 1));

        }
    }

//enable ts port (src/it930x-core.c


///========================== Enable TsPort ==========================
    /* enable tsport_1~4 */
    for (br_idx = 0; br_idx < dev->DC->it9300_Number; br_idx++) {
        for (ts_idx = 0; ts_idx < dev->DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {

            ret = IT9300_enableTsPort(&dev->DC->it9300, br_idx, ts_idx);
            if (ret) {
                _SONY_PHY_ANDROID_INFO("PROBE - Enable %d it9300 Tsport_%d fail\n", br_idx, ts_idx+1);
            }

            reg_value = 1;
            //FDI : omega ofdm 0xa5 close.
            //Demodulator_writeRegisters((Demodulator *) dev->DC->it9300.tsSource[br_idx][ts_idx].htsDev, 8, 0xa5, 1, &reg_value);
            //DL_Demodulator_writeRegisters((Demodulator *) dev->DC->it9300.tsSource[br_idx][ts_idx].htsDev, 8, 0xa5, 1, &reg_value);
            internal_Demodulator_writeRegisters(dev->DC, Processor_OFDM,0, 0xa5, 1, &reg_value, br_idx, ts_idx); //Processor_OFDM = 8
        }

        /////IT9300_setIgnoreFail(g_it9300, br_idx, true);	//open error packet
    }

    _SONY_PHY_ANDROID_INFO("open success...");
    //success?!

    return 0;

cleanup_from_error:
	_SONY_PHY_ANDROID_WARN("open - error: %d, dc: %p, calling IT9300_reboot", -error, DC);
	if(DC) {
		int reboot_error = IT9300_reboot(&DC->it9300, br_idx);
        SonyPHYAndroid::ITE_93x_OPEN_CLEANUP_FROM_ERROR_LAST_REBOOT_TIMESTAMP = gtl();

        _SONY_PHY_ANDROID_WARN("open - error: after IT9300_reboot: reboot error: %d, last reboot timestamp: %ld", reboot_error, SonyPHYAndroid::ITE_93x_OPEN_CLEANUP_FROM_ERROR_LAST_REBOOT_TIMESTAMP);
	}

	return -error;
exit:
    return -1;
};

int SonyPHYAndroid::IT9300_setIgnoreFail (IN  Endeavour*      endeavour, IN  Byte            chip,IN  bool            bvalue) {
if(bvalue)
return (IT9300_writeRegister (endeavour, (Byte)chip, p_br_reg_ts_fail_ignore, 0x1F));
else
return (IT9300_writeRegister (endeavour, (Byte)chip, p_br_reg_ts_fail_ignore, 0));
}
int SonyPHYAndroid::internal_device_communication_test(Device_Context* DC) {
//    device_communication_test (Device_Context *DC) {
        Dword error = Error_NO_ERROR;
        Dword version = 0;

        error = IT9300_getFirmwareVersion (&DC->it9300, 0);

        if (error) {
            _SONY_PHY_ANDROID_INFO("Device test Error\n");
        } else {
                _SONY_PHY_ANDROID_INFO("Device test ok\n");
        }
        return error;
}
Word g_def_device_id = 0xffff;

int SonyPHYAndroid::internal_getEEPROMConfig(Device_Context* DC) {
        Dword	result = Error_NO_ERROR;
        Byte	chip_version = 0;
        Dword	chip_Type = 0;
        Byte	var[2];
        Byte	btmp = 0;
        Byte	eeprom_exit = 0;
        int i = 0, j = 0, br_idx = 0, ts_idx = 0;
        Word rxid_reg_addr = 0x49EC, Serial_No = -1;

        DC->it9300_Number = 1;
        for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
            DC->it9300.receiver_chip_number[br_idx] = RX_CHIP_NUMBER;
        }
        DC->board_id = RX_BOARD_ID;
        for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++)
            for(ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++)
                DC->chip_Type[br_idx][ts_idx] = RX_CHIP_TYPE;

        DC->device_id = g_def_device_id;
        DC->it9300.chipCount = 1;
        DC->it9300.bypassScript = True;
        DC->it9300.bypassBoot = False;
        DC->it9300.maxBusTxSize = 63;

        for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
            DC->it9300.gator[br_idx].existed = True;
            DC->it9300.gator[br_idx].outTsPktLen = PKT_LEN_188;
            DC->it9300.gator[br_idx].booted = False;
            DC->it9300.gator[br_idx].initialized = False;

            if (br_idx == 0)
                DC->it9300.gator[br_idx].outDataType = OUT_DATA_USB_DATAGRAM;
            else
                DC->it9300.gator[br_idx].outDataType = OUT_DATA_TS_PARALLEL;

            for (ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {

                DC->it9300.tsSource[br_idx][ts_idx].existed = True;
                DC->it9300.tsSource[br_idx][ts_idx].tsType = TS_SERIAL;
                DC->it9300.tsSource[br_idx][ts_idx].tsPktLen = 188;	///Unknow
                DC->it9300.tsSource[br_idx][ts_idx].initialized = False; ///Unknow
                DC->it9300.tsSource[br_idx][ts_idx].htsDev = &DC->it9133[br_idx][ts_idx];
                DC->it9133[br_idx][ts_idx].userData = &DC->it9300;
                DC->it9133[br_idx][ts_idx].pendeavour = &DC->it9300;
                DC->it9133[br_idx][ts_idx].br_idx = br_idx;
                DC->it9133[br_idx][ts_idx].tsSrcIdx = ts_idx;

                if (ts_idx >= 2)
                    DC->it9300.tsSource[br_idx][ts_idx].i2cBus = 2;
                else
                    DC->it9300.tsSource[br_idx][ts_idx].i2cBus = 3;

                if ((ts_idx == 0)||(ts_idx == 2))
                    DC->it9300.tsSource[br_idx][ts_idx].i2cAddr  = 0x38;
                else
                    DC->it9300.tsSource[br_idx][ts_idx].i2cAddr = 0x3C;

                DC->it9300.tsSource[br_idx][ts_idx].tsPort = static_cast<TsPort>(ts_idx + 1);

                DC->rx_device_id[((br_idx * 4) + ts_idx)] = DC->device_id;
                //_SONY_PHY_ANDROID_INFO("EEPROM - %d, %d, RxDevice_ID = 0x%lx \n", br_idx, ts_idx, DC->device_id);
                DC->device_id--;
            }
        }

        ///check eeprom valid or not
        result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, OVA_EEPROM_CFG_VALID, 1, &eeprom_exit);
        if (result) {
            _SONY_PHY_ANDROID_INFO("EEPROM - 0x461D eeprom valid bit read fail!");
            goto exit;
        }

        //_SONY_PHY_ANDROID_INFO("EEPROM - Valid bit = 0x%02X \n", eeprom_exit);
        //printk("eeprom_exit = %d\n",eeprom_exit);
        if (eeprom_exit != 0) {
            //_SONY_PHY_ANDROID_INFO("EEPROM - ========== Can read eeprom \n");
            ///Read Board ID

            result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_BOARDID, 1, &btmp);
            if (result) {
                _SONY_PHY_ANDROID_INFO("EEPROM - read Board ID fail \n");
                goto exit;
            }

            DC->board_id = btmp;

            //_SONY_PHY_ANDROID_INFO("EEPROM - Board ID: %ld \n", DC->board_id);
            ///read IT930x NUM

            result	= IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_EVBNUM, 1, &btmp);
            if (result) {
                _SONY_PHY_ANDROID_INFO("EEPROM - read IT9300 NUM fail \n");
                goto exit;
            }

            //_SONY_PHY_ANDROID_INFO("EEPROM - Number of IT9300 is %d \n", btmp);

            DC->it9300_Number = btmp;

            ///read chip type
            /*patch for read eeprom valid bit*/

            result = IT9300_readRegister ((Endeavour*) &DC->it9300, 0, chip_version_7_0, &chip_version); //chip =0
            result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, chip_version_7_0+1, 2, var);
            if (result) {
                _SONY_PHY_ANDROID_INFO("EEPROM - patch fail \n");
                goto exit;
            }

            chip_Type = var[1]<<8 | var[0];

            //_SONY_PHY_ANDROID_INFO("EEPROM - BR Chip Version is %d, BR Chip Type is 0x%lx \n", chip_version , chip_Type);

            for (i = 0; i < 4; i++)
                DC->it9300.gator[i].i2cAddr = 0x68 + (Byte)(i<<1);

            DC->it9300.gator[1].i2cAddr = 0x6a;//0x6a;

            ///read NLC(Receiver) NUM
            for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
                DC->it9300.gator[br_idx].existed = True;

                DC->it9300.gator[br_idx].i2cAddr = 0x68 + (Byte)((i+1)<<1);

                if (br_idx == 0)
                    DC->it9300.gator[0].outDataType = OUT_DATA_USB_DATAGRAM;
                else
                    DC->it9300.gator[br_idx].outDataType = OUT_DATA_TS_PARALLEL;

                DC->it9300.gator[br_idx].outTsPktLen = PKT_LEN_188;
                DC->it9300.gator[br_idx].initialized = False;
                DC->it9300.gator[br_idx].booted = False;

                result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_EVBNUM + 1 + br_idx, 1, &btmp);
                if (result) {
                    _SONY_PHY_ANDROID_INFO("EEPROM - read NLC NUM fail \n");
                    goto exit;
                }

                //_SONY_PHY_ANDROID_INFO("EEPROM - %d NLC(Receiver) NUM: %d \n", br_idx, btmp);

                DC->it9300.receiver_chip_number[br_idx] = btmp;

                for (ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
                    //Receiver chip type
                    if (br_idx == 0) {
                        if (ts_idx == 0)
                            result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_CHIPTYPE_1stNLC_1, 1, &btmp);
                        else if (ts_idx == 1)
                            result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_CHIPTYPE_1stNLC_2, 1, &btmp);
                        else if (ts_idx == 2)
                            result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_CHIPTYPE_1stNLC_3, 1, &btmp);
                        else if (ts_idx == 3)
                            result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_CHIPTYPE_1stNLC_4, 1, &btmp);

                        //_SONY_PHY_ANDROID_INFO("EEPROM - %d Receiver Chip Type is 0x%x \n", ts_idx , btmp);

                        DC->chip_Type[br_idx][ts_idx] = btmp;
                    } else {
                        result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_CHIPTYPE_otherNLC, 1, &btmp);
                        DC->chip_Type[br_idx][ts_idx] = btmp;
                    }

                    if (result) {
                        _SONY_PHY_ANDROID_INFO("EEPROM - read NLC Chip Type: fail...!!\n");
                        goto exit;
                    }

                    DC->it9300.tsSource[br_idx][ts_idx].tsPktLen = 188;	///Unknow
                    DC->it9300.tsSource[br_idx][ts_idx].initialized = False; ///Unknow
                    DC->it9300.tsSource[br_idx][ts_idx].existed = True;
                    DC->it9300.tsSource[br_idx][ts_idx].htsDev = &DC->it9133[br_idx][ts_idx];
                    DC->it9133[br_idx][ts_idx].userData = &DC->it9300;
                    DC->it9133[br_idx][ts_idx].pendeavour = &DC->it9300;
                    DC->it9133[br_idx][ts_idx].br_idx = br_idx;
                    DC->it9133[br_idx][ts_idx].tsSrcIdx = ts_idx;
                }
            }

#if 1 //4 btye version
///Rx DEVICE ID
            result = internal_get_rx_id(DC);

            if (DC->device_id == g_def_device_id) {
                for (br_idx = 0; br_idx <  DC->it9300_Number; br_idx++) {
                    for (ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
                        //DC->it9133[br_idx][ts_idx].returnchannel_rx_dev_id = (DC->device_id - ts_idx - (br_idx * 4));
                        DC->rx_device_id[((br_idx * 4) + ts_idx)] = (DC->device_id - ts_idx - (br_idx * 4));
                        //_SONY_PHY_ANDROID_INFO("EEPROM - %d, %d, RxDevice_ID = %ld \n", br_idx, ts_idx, (DC->device_id - ts_idx - (br_idx * 4)));
                        g_def_device_id--;
                    }
                }
            } else {
                for (br_idx = 0; br_idx <  DC->it9300_Number; br_idx++) {
                    for (ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
                        //DC->it9133[br_idx][ts_idx].returnchannel_rx_dev_id = (DC->device_id + ts_idx + (br_idx * 4));
                        DC->rx_device_id[((br_idx * 4) + ts_idx)] = (DC->device_id + ts_idx + (br_idx * 4));
                        //_SONY_PHY_ANDROID_INFO("EEPROM - %d, %d, RxDevice_ID = %ld \n", br_idx, ts_idx, (DC->device_id + ts_idx + (br_idx * 4)));
                    }
                }
            }
#endif
        }

        if ((DC->board_id == 0x50) || (DC->board_id == 0x54)) { /* 4/8/12/16 */
            ///4
            if (DC->it9300_Number == 1) {
            }
            ///8
            if (DC->it9300_Number == 2) {
                /* 1st */
                DC->it9300.tsSource[0][4].tsType   = TS_PARALLEL;	/* 1st Port_0 */
                DC->it9300.tsSource[0][4].tsPort   = TS_PORT_0;		/* 1st Port_0 */

                /* 2st */
                DC->it9300.tsSource[1][0].i2cAddr = 0x38;
                DC->it9300.tsSource[1][1].i2cAddr = 0x3C;
                DC->it9300.tsSource[1][2].i2cAddr = 0x38;
                DC->it9300.tsSource[1][3].i2cAddr = 0x3C;
                DC->it9300.tsSource[1][0].i2cBus = 3;
                DC->it9300.tsSource[1][1].i2cBus = 3;
                DC->it9300.tsSource[1][2].i2cBus = 2;
                DC->it9300.tsSource[1][3].i2cBus = 2;
                DC->it9300.tsSource[1][0].tsType = TS_SERIAL;
                DC->it9300.tsSource[1][1].tsType = TS_SERIAL;
                DC->it9300.tsSource[1][2].tsType = TS_SERIAL;
                DC->it9300.tsSource[1][3].tsType = TS_SERIAL;
                DC->it9300.tsSource[1][0].tsPort = TS_PORT_1;
                DC->it9300.tsSource[1][1].tsPort = TS_PORT_2;
                DC->it9300.tsSource[1][2].tsPort = TS_PORT_3;
                DC->it9300.tsSource[1][3].tsPort = TS_PORT_4;
            }

            ///12
            if (DC->it9300_Number == 3) {
                /* Temp CASE */
            }

            ///16
            if (DC->it9300_Number == 4)	 {
                /* Temp CASE */
            }

        } else if (DC->board_id == 0x51) {	///5's
            /* Temp CASE */
        } else if (DC->board_id == 0x52) { // MXL69X ATSC device
            //MAX_NUMBER_OF_RX_FILTER
            DC->it9300.tsSource[0][0].i2cAddr = 0xC6;
            DC->it9300.tsSource[0][1].i2cAddr = 0xC0;
            DC->it9300.tsSource[0][2].i2cAddr = 0xC6;
            DC->it9300.tsSource[0][3].i2cAddr = 0xC0;
            DC->it9300.tsSource[0][0].i2cBus = 2;
            DC->it9300.tsSource[0][1].i2cBus = 2;
            DC->it9300.tsSource[0][2].i2cBus = 3;
            DC->it9300.tsSource[0][3].i2cBus = 3;
            DC->it9300.tsSource[0][4].i2cBus = 3;
            DC->it9300.tsSource[0][4].i2cAddr = 0x3E;
            DC->it9300.tsSource[0][4].tsType = TS_SERIAL;
            DC->it9300.tsSource[0][4].tsPort = TS_PORT_0;
        } else if (DC->board_id == 0x30) { ///2's
            if (DC->chip_Type[0][0] == EEPROM_MXL692) {
                DC->it9300.tsSource[0][0].i2cAddr = 0xC6;
                DC->it9300.tsSource[0][1].i2cAddr = 0xC0;
            } else {
                DC->it9300.tsSource[0][0].i2cAddr = 0x38;
                DC->it9300.tsSource[0][1].i2cAddr = 0x38;
            }

            if (DC->it9300.receiver_chip_number[0] == 2 && (DC->chip_Type[0][0]) == 0x00) { //check diversity
                DC->it9300.tsSource[0][0].i2cBus = 2;
                DC->it9300.tsSource[0][1].i2cBus = 3;
                DC->it9300.tsSource[0][0].tsPort = TS_PORT_4;
                DC->it9300.tsSource[0][1].tsPort = TS_PORT_0;
                DC->chip_Type[0][0] =  DC->chip_Type[0][1];
                DC->chip_Type[0][1] = 0x00;
                DC->it9300.receiver_chip_number[0] = 1;
            } else {
                DC->it9300.tsSource[0][0].i2cBus = 3;
                DC->it9300.tsSource[0][1].i2cBus = 2;
                DC->it9300.tsSource[0][0].tsPort = TS_PORT_0;
                DC->it9300.tsSource[0][1].tsPort = TS_PORT_4;
            }

            DC->it9300.tsSource[0][0].tag[0] = 0x00 + ((1)<<4);
            DC->it9300.tsSource[0][0].tag[1] = 0x01 + ((1)<<4);
            DC->it9300.tsSource[0][0].tag[2] = 0x02 + ((1)<<4);
            DC->it9300.tsSource[0][0].tag[3] = 0x03 + ((1)<<4);
            DC->it9300.tsSource[0][1].tag[0] = 0x00 + ((2)<<4);
            DC->it9300.tsSource[0][1].tag[1] = 0x01 + ((2)<<4);
            DC->it9300.tsSource[0][1].tag[2] = 0x02 + ((2)<<4);
            DC->it9300.tsSource[0][1].tag[3] = 0x03 + ((2)<<4);
        } else if (DC->board_id == 0x33) {
            DC->it9300.tsSource[0][0].i2cAddr = 0xC8;
            DC->it9300.tsSource[0][0].i2cBus = 2;
            DC->it9300.tsSource[0][0].tsType = TS_SERIAL;
            DC->it9300.tsSource[0][0].tsPort = TS_PORT_0;
        }

        if (eeprom_exit != 0) {
            _SONY_PHY_ANDROID_INFO("============ Set Data From EEPROM Value ============\n");
        } else {
                _SONY_PHY_ANDROID_INFO("============ Set Data From Default Value ============\n");
        }
        _SONY_PHY_ANDROID_INFO("SETTING INFO - Number of IT9300 is %d \n", DC->it9300_Number);

        for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++)
            _SONY_PHY_ANDROID_INFO("SETTING INFO - %d NLC(Receiver) NUM: %d \n", br_idx, DC->it9300.receiver_chip_number[br_idx]);

        _SONY_PHY_ANDROID_INFO("SETTING INFO - Board ID: %ld \n", DC->board_id);
        _SONY_PHY_ANDROID_INFO("SETTING INFO - Device ID: %ld \n", DC->device_id);

        if (eeprom_exit != 0)
            _SONY_PHY_ANDROID_INFO("SETTING INFO - BR Chip Version is %d, BR Chip Type is 0x%lx \n", chip_version , chip_Type);

        for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
            for(ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
                _SONY_PHY_ANDROID_INFO("SETTING INFO - %d, %d Receiver Chip Type is 0x%x \n", br_idx, ts_idx , DC->chip_Type[br_idx][ts_idx]);
                _SONY_PHY_ANDROID_INFO("SETTING INFO - %d, %d Receiver i2cAddr is 0x%x \n", br_idx, ts_idx , DC->it9300.tsSource[br_idx][ts_idx].i2cAddr);
                _SONY_PHY_ANDROID_INFO("SETTING INFO - %d, %d Receiver i2cBus is 0x%x \n", br_idx, ts_idx , DC->it9300.tsSource[br_idx][ts_idx].i2cBus);
                _SONY_PHY_ANDROID_INFO("SETTING INFO - %d, %d Receiver tsType is 0x%x \n", br_idx, ts_idx , DC->it9300.tsSource[br_idx][ts_idx].tsType);
                _SONY_PHY_ANDROID_INFO("SETTING INFO - %d, %d Receiver tsPort is 0x%x \n", br_idx, ts_idx , DC->it9300.tsSource[br_idx][ts_idx].tsPort);
                _SONY_PHY_ANDROID_INFO("SETTING INFO - %d, %d RxDevice_ID = %d \n", br_idx, ts_idx, DC->rx_device_id[((br_idx * 4) + ts_idx)]);
            }
        }

        _SONY_PHY_ANDROID_INFO("=====================================================\n");
        _SONY_PHY_ANDROID_INFO("- %s success -\n",  __func__);
        return result;

        exit:
        _SONY_PHY_ANDROID_INFO("- %s fail -\n",  __func__);
        return result;

}

int SonyPHYAndroid::internal_it930x_initialize(Device_Context *DC, Byte br_idx) {

    Dword error = BR_ERR_NO_ERROR;
    Dword fileVersion = 0xFFFFFFFF, cmdVersion = 0xFFFFFFFF, tmpVersion = 0;
    Byte usb_dma_reg = 0, chip_index = 0, var1 = 0, var2 = 0, var3 = 0;

    error = IT9300_readRegisters(&DC->it9300, 0, 0xF53A, 1, &var1);
    error = IT9300_readRegisters(&DC->it9300, 0, 0xDA98, 1, &var2);
    error = IT9300_readRegisters(&DC->it9300, 0, 0xDA99, 1, &var3);

    _SONY_PHY_ANDROID_INFO("0xF53A = %d,  0xDA98 = 0x%X,  0xDA99 = 0x%X\n", var1, var2, var3);

    error = IT9300_getFirmwareVersion (&DC->it9300, br_idx);
    if (error) {
        _SONY_PHY_ANDROID_INFO("IT9300_getFirmwareVersion fail! \n");
        goto exit;
    }

    if (DC->it9300.firmwareVersion != 0) {
        DC->it9300.gator[br_idx].booted = True;
    } else
        DC->it9300.gator[br_idx].booted = False;

    if (DC->it9300.gator[br_idx].booted) {
        /* getFirmwareVersion() */
        error = internal_getFirmwareVersionFromFile(DC, &tmpVersion);
        if (error) {
            _SONY_PHY_ANDROID_INFO("\t getFirmwareVersionFromFile fail [0x%08lx]\n", error);
            goto exit;
        }

        fileVersion = (Dword)(tmpVersion&0xFFFFFFFF);

        /* Use "Command_QUERYINFO" to get firmware version */
        error = IT9300_getFirmwareVersion(&DC->it9300, br_idx);
        if (error) {
            _SONY_PHY_ANDROID_INFO("\t getFirmwareVersion fail [0x%08lx]\n", error);
            goto exit;
        }
        cmdVersion = DC->it9300.firmwareVersion;

        _SONY_PHY_ANDROID_INFO("\t cmdVersion = 0x%lX\n", cmdVersion);
        _SONY_PHY_ANDROID_INFO("\t fileVersion = 0x%lX\n", fileVersion);

        if (cmdVersion != fileVersion) {
            _SONY_PHY_ANDROID_INFO("\t Outside_Fw = 0x%lX, Inside_Fw = 0x%lX, Reboot\n", fileVersion, cmdVersion);

            error = IT9300_reboot(&DC->it9300, br_idx);
            DC->bBootCode = True;
            if (error) {
                _SONY_PHY_ANDROID_INFO("\t Reboot fail [0x%08lx]\n", error);
                goto exit;
            } else
                return Error_NOT_READY;
        } else
                _SONY_PHY_ANDROID_INFO("\t FW version is the same\n");
    }

    error = IT9300_initialize (&DC->it9300, br_idx);
    if (error) {
        _SONY_PHY_ANDROID_INFO("IT9300_initialize fail \n");
        goto exit;
    }

    if ((DC->chip_Type[0][0] == EEPROM_EW100) || (DC->chip_Type[0][0] == EEPROM_CXD2880) || (DC->chip_Type[0][0] == EEPROM_CXD2880_33)) {
        error = IT9300_writeRegister(&DC->it9300, 0, p_br_reg_ts_fail_ignore, 0x1F);
        if (error)
            goto exit;
        error = IT9300_writeRegister(&DC->it9300, 0, p_br_reg_ts0_clk_sel, 0x01);
        if (error)
            goto exit;
    }

    _SONY_PHY_ANDROID_INFO("IT9300_initialize ok \n");



#if BCAS_ENABLE
    error = IT9300_bcasInit(&DC->it9300, br_idx);
    if (error) {
		_SONY_PHY_ANDROID_INFO("IT9300_bcasInit fail \n");
		goto exit;
	}
	_SONY_PHY_ANDROID_INFO("IT9300_bcasInit OK \n");
#endif
    exit:
    return error;
}


int SonyPHYAndroid::internal_getFirmwareVersionFromFile(Device_Context *DC, Dword* version) {
    Byte chip_version = 0;
    Dword chip_Type;
    Byte var[2];
    Dword result = Error_NO_ERROR;

    Dword OFDM_VER1, OFDM_VER2, OFDM_VER3, OFDM_VER4;
    Dword LINK_VER1, LINK_VER2, LINK_VER3, LINK_VER4;

    _SONY_PHY_ANDROID_INFO("- Enter %s Function line %d\n", __func__,__LINE__);

    //result = Demodulator_readRegister((Demodulator*) &DC->Demodulator, 0, Processor_LINK, chip_version_7_0, &chip_version);
    result = IT9300_readRegister(&DC->it9300, 0, chip_version_7_0, &chip_version);
    if (result) {
        _SONY_PHY_ANDROID_INFO("\t Error: IT9300_readRegister [chip_version_7_0] is failed\n");
        goto exit;
    }

    //result = Demodulator_readRegisters((Demodulator*) &DC->Demodulator, 0, Processor_LINK, (chip_version_7_0 + 1), 2, var);
    result = IT9300_readRegisters(&DC->it9300, 0, (chip_version_7_0 + 1), 2, var);
    if (result) {
        _SONY_PHY_ANDROID_INFO("\t Error: IT9300_readRegisters [chip_version_7_0 + 1] is failed\n");
        goto exit;
    }

    chip_Type = var[1]<<8 | var[0];

    //if(processor == Processor_OFDM)
    //	*version = (Dword)((OFDM_VER1 << 24) + (OFDM_VER2 << 16) + (OFDM_VER3 << 8) + OFDM_VER4);
    //else //LINK
    *version = (Dword)((DVB_LL_VERSION1 << 24) + (DVB_LL_VERSION2 << 16) + (DVB_LL_VERSION3 << 8) + DVB_LL_VERSION4);

    _SONY_PHY_ANDROID_INFO("- %s : success -\n", __func__);
    return result;

    exit:
    _SONY_PHY_ANDROID_INFO("- %s : fail -\n", __func__);
    return result;
}


Dword Demodulator_writeRegisters (
        IN  Demodulator*    demodulator,
        IN  Processor       processor,
        IN  Dword           registerAddress,
        IN  Byte            bufferLength,
        IN  Byte*           buffer
) {
    return (Standard_writeRegisters (demodulator, processor, registerAddress, bufferLength, buffer));
}

int SonyPHYAndroid::internal_Demodulator_writeRegisters (Device_Context *DC, Processor processor, Byte option ,Dword registerAddress, Byte bufferLength, Byte* buffer, int br_idx, int ts_idx) {
    int error = Error_NO_ERROR;

    mutex_lock(&DC->dev_mutex);

    switch (processor) {
        case Processor_DEMOD:
            switch (DC->chip_Type[br_idx][ts_idx]){
                case EEPROM_IT913X:
                    error = Demodulator_writeRegisters((Demodulator*) DC->it9300.tsSource[br_idx][ts_idx].htsDev, Processor_LINK, registerAddress, bufferLength, buffer);
                    break;
                case EEPROM_CXD6801_NUVYYO:
                case EEPROM_CXD6801_EVB:
                case EEPROM_CXD6801_TS7:
                    error = DRV_CXD6801_writeRegister(DC, processor, option, registerAddress, bufferLength, buffer, br_idx, ts_idx);
                    break;
                default:
                    _SONY_PHY_ANDROID_INFO("DL_Demodulator_writeRegisters Unknown chip type [%d]\n", DC->chip_Type[br_idx][ts_idx]);
                    error = BR_ERR_INVALID_DEV_TYPE;
                    break;
            }
            break;
        case Processor_TUNER:
            switch (DC->chip_Type[br_idx][ts_idx]){
                case EEPROM_IT913X:
                    error = Demodulator_writeRegisters((Demodulator*) DC->it9300.tsSource[br_idx][ts_idx].htsDev, Processor_OFDM, registerAddress, bufferLength, buffer);
                    break;
                case EEPROM_CXD6801_NUVYYO:
                case EEPROM_CXD6801_EVB:
                case EEPROM_CXD6801_TS7:
                    error = DRV_CXD6801_writeRegister(DC, processor, option, registerAddress, bufferLength, buffer, br_idx, ts_idx);
                    break;
                default:
                    _SONY_PHY_ANDROID_INFO("DL_Demodulator_writeRegisters Unknown chip type [%d]\n", DC->chip_Type[br_idx][ts_idx]);
                    error = BR_ERR_INVALID_DEV_TYPE;
                    break;
            }
            break;
        case Processor_OFDM:
            error = Demodulator_writeRegisters((Demodulator*) DC->it9300.tsSource[br_idx][ts_idx].htsDev, processor, registerAddress, bufferLength, buffer);
            break;
        default:
            _SONY_PHY_ANDROID_INFO("DL_Demodulator_writeRegisters Unknown request processor [%d]\n", processor);
            error = BR_ERR_INVALID_DEV_TYPE;
            break;
    }

    mutex_unlock(&DC->dev_mutex);

    return error;

}

#define DRV_CXD6801_DEBUG  _SONY_PHY_ANDROID_INFO


int SonyPHYAndroid::internal_get_rx_id(Device_Context* DC) {
    Dword result = Error_NO_ERROR;
    Byte read_register = 0;
    Word map_enalbe_reg_addr = (OVA_EEPROM_CFG + 0x50);
    Word rxid_h_reg_addr = (OVA_EEPROM_CFG + 0x51);
    Word rxid_l_reg_addr = (OVA_EEPROM_CFG + 0x52);
    Dword temp = 0, temp2 = 0, i = 0;

    result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, map_enalbe_reg_addr, 1, &read_register);
    if (result) {
        _SONY_PHY_ANDROID_INFO("\t Error: Read EEPROM fail [0x%08lx]\n", result);
        goto exit;
    }

    if (read_register) {
        DC->map_enalbe = true;

        result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, rxid_h_reg_addr, 1, &read_register);
        temp = read_register;
        //_SONY_PHY_ANDROID_INFO("EEPROM - high value = %d \n", read_register);

        result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, rxid_l_reg_addr, 1, &read_register);
        temp2 = read_register;
        //_SONY_PHY_ANDROID_INFO("EEPROM - low value = %d \n", read_register);

        DC->device_id = (temp << 8) + temp2;
    } else {
        //_SONY_PHY_ANDROID_INFO("EEPROM - Mapp disalbe, Rx device ID using default\n");
        DC->device_id = g_def_device_id;
    }

    //_SONY_PHY_ANDROID_INFO("EEPROM - Device_ID = %ld \n", DC->device_id);

    //_SONY_PHY_ANDROID_INFO("- %s success -\n",  __func__);
    return result;

    exit:
    _SONY_PHY_ANDROID_INFO("EEPROM - Rx device ID using default\n");
    DC->device_id = g_def_device_id;

    _SONY_PHY_ANDROID_INFO("- %s fail -\n",  __func__);
    return result;
}



void SonyPHYAndroid::resetProcessThreadStatistics() {
    ip_completed_packets_parsed = 0;
    alp_total_bytes = 0;
    alp_total_LMTs_recv = 0;
}

void SonyPHYAndroid::statusMetricsResetFromTuneChange() {
    _SONY_PHY_ANDROID_INFO("statusMetricsResetFromContextChange - resetting statusThreadFirstLoopAfterTuneComplete");

    statusThreadFirstLoopAfterTuneComplete = true; //will dump DemodGetconfiguration from statusThread

    statusMetricsResetFromPLPListenChange(); //clear our diag flags and metrics types also...
}

void SonyPHYAndroid::statusMetricsResetFromPLPListenChange() {
    _SONY_PHY_ANDROID_INFO("statusMetricsResetFromPLPListenChange - resetting statusThreadFirstLoop_*Lock flags and clearing TunerSignalInfo/_Diag's");

    statusThreadFirstLoopAfterTuneComplete_HasBootstrapLock_for_BSR_Diag = false;
    statusThreadFirstLoopAfterTuneComplete_HasL1B_DemodLock_for_L1B_Diag = false;
    statusThreadFirstLoopAfterTuneComplete_HasL1D_DemodLock_for_L1D_Diag = false;

//    demodLockStatus = 0;
    cpuStatus = 0;

}

void SonyPHYAndroid::allocate_atsc3_sl_tlv_block() {
    //protect for de-alloc with using recursive lock here
    //atsc3_sl_tlv_block_mutex.lock();

    if(!atsc3_sl_tlv_block) {
        atsc3_sl_tlv_block = block_Alloc(TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE);
    }
    //atsc3_sl_tlv_block_mutex.unlock();
}

int SonyPHYAndroid::tune(int freqKhz, int single_plp) {
    int ret = 0;


    //tell any RXDataCallback or process event that we should discard
    cb_should_discard = true;

    //acquire our CB mutex so we don't push stale TLV packets
    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);

    //jjustman-2021-03-10 - also acquire our atsc3_sl_tlv_block_mutex so we can safely discard any pending TLV frames
    if(!cb_tlv) {
        cb_tlv = CircularBufferCreate(TLV_CIRCULAR_BUFFER_SIZE);
    } else {
        //jjustman-2021-01-19 - clear out our current cb on re-tune

        //forcibly flush any in-flight TLV packets in cb here by calling, need (cb_should_discard == true),
        // as our type is atomic_bool and we can't printf its value here due to:
        //                  call to implicitly-deleted copy constructor of 'std::__ndk1::atomic_bool' (aka 'atomic<bool>')
        _SONY_PHY_ANDROID_INFO("SonyPHYAndroid::tune - cb_should_discard: %u, cb_GetDataSize: %zu, calling CircularBufferReset(), cb: %p, early in tune() call",
                                   (cb_should_discard == true), CircularBufferGetDataSize(this->cb_tlv), cb_tlv);
        CircularBufferReset(cb_tlv);
    }

    atsc3_core_service_application_bridge_reset_context();

    AcquireChannelRequest acquireChannelRequest = { 0, 6000, (u32)freqKhz, 0, 5};
	_SONY_PHY_ANDROID_INFO("SonyPHYAndroid::tune - acquireChannelRequest: freq: %d, mode: %d",
						   acquireChannelRequest.frequency, acquireChannelRequest.mode);

    acquireChannelRequest.error = DL_Demodulator_acquireChannel(DC, &acquireChannelRequest, 0, 0);
    if (!acquireChannelRequest.error) {
        _SONY_PHY_ANDROID_INFO("set Mode =%d, Freq = %d, BW = %d, success\n",
                               acquireChannelRequest.mode, acquireChannelRequest.frequency,
                               acquireChannelRequest.bandwidth);
    } else {
        _SONY_PHY_ANDROID_INFO("set Mode =%d, Freq = %d, BW = %d, fail = 0x%x\n",
                               acquireChannelRequest.mode, acquireChannelRequest.frequency,
                               acquireChannelRequest.bandwidth, acquireChannelRequest.error);
    }

    /*
     * Byte		chip;
    Statistic	statistic;

     Bool signalPresented;       /** Signal is presented.                                                                         */
  //  Bool signalLocked;          /** Signal is locked.                                                                            */
 //   Byte signalQuality;         /** Signal quality, from 0 (poor) to 100 (good).                                                 */
 //   Byte signalStrength;        /** Signal strength from 0 (weak) to 100 (strong).

  //  u32			error;
  //  int  		snr;        /** Signal snr from 0 (weak) to 100 (strong).                                                    */
//    int  		rf_power;   /** Signal rf_power from -80dbm (weak) to -20dbm (strong).                                       */
 //   Byte		reserved[8];
  //   */

    //setup shared memory for cb callback (or reset if already allocated)



    //atsc3_sl_tlv_block_mutex.unlock();
    //CircularBufferMutex_local.unlock();

    //jjustman-2021-01-19 - allow for cb to start acumulating TLV frames

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
            _SONY_PHY_ANDROID_INFO("::Open() - starting captureThread took %d spins, final state: %d",
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
            _SONY_PHY_ANDROID_WARN("::Open() - starting processThreadIsRunning took %d spins, final state: %d",
                                    threadStartupSpinlockCount,
                                    this->processThreadIsRunning);
        }
    }

    if(!this->processAlpThreadIsRunning) {
        processAlpThreadShouldRun = true;
        processAlpThreadHandle = std::thread([this]() {
            this->processAlpFromCircularBufferThread();
        });

        //micro spinlock
        int threadStartupSpinlockCount = 0;
        while (!this->processAlpThreadIsRunning && threadStartupSpinlockCount++ < 100) {
            usleep(10000);
        }

        if (threadStartupSpinlockCount > 50) {
            _SONY_PHY_ANDROID_WARN("::Open() - starting processAlpThreadIsRunning took %d spins, final state: %d",
                                   threadStartupSpinlockCount,
                                   this->processAlpThreadIsRunning);
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
            _SONY_PHY_ANDROID_WARN("::Open() - starting statusThread took %d spins, final state: %d",
                                    threadStartupSpinlockCount,
                                    this->statusThreadIsRunning);
        }
    }

    //start off listening on PLP0
    MPLPData mPlpData = {  };

    mPlpData.PLPIDArray[0]  = 0;
    mPlpData.PLPIDArray[1]  = 1;
    mPlpData.PLPIDArray[2]  = 2;
    mPlpData.PLPIDArray[3]  = 0;
    mPlpData.TotalPLPnum    = 1;

    DL_Demodulator_setPLPID(DC, &mPlpData, 0, 0);
    plp_configuration_data = mPlpData;

    usleep(1000000);
    cb_should_discard = false;
    return ret;
}


int SonyPHYAndroid::captureThread()
{
    _SONY_PHY_ANDROID_INFO("SonyPHYAndroid::captureThread: starting with this: %p", this);
    this->pinProducerThreadAsNeeded();
    this->captureThreadIsRunning = true;

    readFromUsbDemodEndpointRxTs();

    this->releasePinnedProducerThreadAsNeeded();
    this->captureThreadIsRunning = false;

    _SONY_PHY_ANDROID_INFO("SonyPHYAndroid::CaptureThread complete");

    return 0;
}


int SonyPHYAndroid::listen_plps(vector<uint8_t> plps) {
    _SONY_PHY_ANDROID_INFO("SonyPHYAndroid::listen_plps, size: %d", plps.size());

    unique_lock<mutex> plpConfigParams_mutex_update_plps(plpConfigParams_mutex);

    MPLPData mPlpData = { 0xff, 0xff };

    int numPlps             = plps.size();

    mPlpData.PLPIDArray[0]  = (numPlps > 0 ? plps.at(0) : 0);
    mPlpData.PLPIDArray[1]  = (numPlps > 1 ? plps.at(1) : 0);
    mPlpData.PLPIDArray[2]  = (numPlps > 2 ? plps.at(2) : 0);
    mPlpData.PLPIDArray[3]  = (numPlps > 3 ? plps.at(3) : 0);
    mPlpData.TotalPLPnum    = numPlps;

    _SONY_PHY_ANDROID_INFO("setting PLPs to: size: %d, plp[0]: 0x%02x, plp[1]: 0x%02x, plp[2]: 0x%02x, plp[3]: 0x%02x",
                           mPlpData.TotalPLPnum,
                           mPlpData.PLPIDArray[0],
                           mPlpData.PLPIDArray[1],
                           mPlpData.PLPIDArray[2],
                           mPlpData.PLPIDArray[3]);

    DL_Demodulator_setPLPID(DC, &mPlpData, 0, 0);
    plp_configuration_data = mPlpData;

    statusMetricsResetFromPLPListenChange();

    return 0;
};

void SonyPHYAndroid::NotifyPlpSelectionChangeCallback(vector<uint8_t> plps, void *context) {
    ((SonyPHYAndroid *) context)->listen_plps(plps);
}

//jni pinning

void SonyPHYAndroid::pinProducerThreadAsNeeded() {
    producerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_sony_static_loader_get_javaVM(), "SonyPHYAndroid::producerThread");
}

void SonyPHYAndroid::releasePinnedProducerThreadAsNeeded() {
    if(producerJniEnv) {
        delete producerJniEnv;
        producerJniEnv = nullptr;
    }
}

void SonyPHYAndroid::pinConsumerThreadAsNeeded() {
    _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::pinConsumerThreadAsNeeded: mJavaVM: %p, atsc3_ndk_application_bridge instance: %p", atsc3_ndk_phy_sony_static_loader_get_javaVM(), atsc3_ndk_application_bridge_get_instance());

    consumerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_sony_static_loader_get_javaVM(), "SonyPHYAndroid::consumerThread");
    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->pinConsumerThreadAsNeeded();
    }
}

void SonyPHYAndroid::releasePinnedConsumerThreadAsNeeded() {
    if(consumerJniEnv) {
        delete consumerJniEnv;
        consumerJniEnv = nullptr;
    }

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->releasePinnedConsumerThreadAsNeeded();
    }
}

void SonyPHYAndroid::pinStatusThreadAsNeeded() {
    statusJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_sony_static_loader_get_javaVM(), "SonyPHYAndroid::statusThread");

    if(atsc3_ndk_phy_bridge_get_instance()) {
        atsc3_ndk_phy_bridge_get_instance()->pinStatusThreadAsNeeded();
    }
}

void SonyPHYAndroid::releasePinnedStatusThreadAsNeeded() {
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
Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_init(JNIEnv *env, jobject instance) {
    lock_guard<mutex> sony_phy_android_cctor_mutex_local(SonyPHYAndroid::CS_global_mutex);

    _SONY_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_init: start init, env: %p", env);
    if(sonyPHYAndroid) {
        _SONY_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_init: start init, SonyPHYAndroid is present: %p, calling deinit/delete", sonyPHYAndroid);
        sonyPHYAndroid->deinit();
        sonyPHYAndroid = nullptr;
    }

    sonyPHYAndroid = new SonyPHYAndroid(env, instance);
    sonyPHYAndroid->init();

    _SONY_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_init: return, instance: %p", sonyPHYAndroid);

    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_run(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> sony_phy_android_cctor_mutex_local(SonyPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!sonyPHYAndroid) {
        _SONY_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_run: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        res = sonyPHYAndroid->run();
        _SONY_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_run: returning res: %d", res);
    }

    return res;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_is_1running(JNIEnv* env, jobject instance)
{
    lock_guard<mutex> sony_phy_android_cctor_mutex_local(SonyPHYAndroid::CS_global_mutex);

    jboolean res = false;

    if(!sonyPHYAndroid) {
        _SONY_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_is_1running: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = false;
    } else {
        res = sonyPHYAndroid->is_running();
    }

    return res;
}


extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_stop(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> sony_phy_android_cctor_mutex_local(SonyPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!sonyPHYAndroid) {
        _SONY_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_stop: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {
        res = sonyPHYAndroid->stop();
        _SONY_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_stop: returning res: %d", res);
    }

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_deinit(JNIEnv *env, jobject thiz) {
    lock_guard<mutex> sony_phy_android_cctor_mutex_local(SonyPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!sonyPHYAndroid) {
        _SONY_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_deinit: error, srtRxSTLTPVirtualPHYAndroid is NULL!");
        res = -1;
    } else {

        sonyPHYAndroid->deinit();
        sonyPHYAndroid = nullptr;
    }

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_download_1bootloader_1firmware(JNIEnv *env, jobject thiz, jint fd, jint device_type, jstring device_path_jstring) {
    lock_guard<mutex> sony_phy_android_cctor_mutex_local(SonyPHYAndroid::CS_global_mutex);

    _SONY_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_download_1bootloader_1firmware: fd: %d", fd);
    int res = 0;

    if(!sonyPHYAndroid)  {
        _SONY_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_download_1bootloader_1firmware: SonyPHYAndroid is NULL!");
        res = -1;
    } else {
        const char* device_path_weak = env->GetStringUTFChars(device_path_jstring, 0);
        string device_path(device_path_weak);

        res = sonyPHYAndroid->download_bootloader_firmware(fd, device_type, device_path); //calls pre_init
        env->ReleaseStringUTFChars( device_path_jstring, device_path_weak );

        //jjustman-2020-08-23 - hack, clear out our in-flight reference since we should re-enumerate
        delete sonyPHYAndroid;
        sonyPHYAndroid = nullptr;
    }

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_open(JNIEnv *env, jobject thiz, jint fd, jint device_type, jstring device_path_jstring) {
    lock_guard<mutex> sony_phy_android_cctor_mutex_local(SonyPHYAndroid::CS_global_mutex);

    _SONY_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_open: fd: %d", fd);

    int res = 0;
    if(!sonyPHYAndroid) {
        _SONY_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_open: SonyPHYAndroid is NULL!");
        res = -1;
    } else {
        const char* device_path_weak = env->GetStringUTFChars(device_path_jstring, 0);
        string device_path(device_path_weak);

        res = sonyPHYAndroid->open(fd, device_type, device_path);
        env->ReleaseStringUTFChars( device_path_jstring, device_path_weak );

        if(res < 0) {
            delete sonyPHYAndroid;
            sonyPHYAndroid = nullptr;
        }
    }
    _SONY_PHY_ANDROID_DEBUG("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_open: fd: %d, return: %d", fd, res);

    return res;
}

extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_tune(JNIEnv *env, jobject thiz,
                                                                      jint freq_khz,
                                                                      jint single_plp) {

    lock_guard<mutex> sony_phy_android_cctor_mutex_local(SonyPHYAndroid::CS_global_mutex);


    int res = 0;
    if(!sonyPHYAndroid) {
        _SONY_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_tune: SonyPHYAndroid is NULL!");
        res = -1;
    } else {
        res = sonyPHYAndroid->tune(freq_khz, single_plp);
    }

    return res;
}
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_listen_1plps(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jobject plps) {
    lock_guard<mutex> sony_phy_android_cctor_mutex_local(SonyPHYAndroid::CS_global_mutex);

    int res = 0;
    if(!sonyPHYAndroid) {
        _SONY_PHY_ANDROID_ERROR("Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_listen_1plps: SonyPHYAndroid is NULL!");
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

        res = sonyPHYAndroid->listen_plps(listen_plps);
    }

    return res;
}

extern "C" JNIEXPORT jstring JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_get_1sdk_1version(JNIEnv *env, jobject thiz) {
    string sdk_version = sonyPHYAndroid->get_sdk_version();
    return env->NewStringUTF(sdk_version.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_SonyPHYAndroid_get_1firmware_1version(JNIEnv *env, jobject thiz) {
    string firmware_version = sonyPHYAndroid->get_firmware_version();
    return env->NewStringUTF(firmware_version.c_str());
}

//jjustman-2022-05-24 - hacks

//URB 153408 -> ~ 300 -> /4 -.
//jjustman-2022-08-16 - was 128 x 4
static volatile int     isCaptStarted = 0;
static unsigned int     reqsize = 64;  // Request size in number of packets
static unsigned int     queuedepth = 8;   // Number of requests to queue
static unsigned int     pktsize = 512;     // Maximum packet size for the endpoint
static unsigned int     success_count = 0;  // Number of successful transfers
static unsigned int     failure_count = 0;  // Number of failed transfers
static volatile int     rqts_in_flight = 0; // Number of transfers that are in progress

static void xfer_callback(struct libusb_transfer *transfer)
{
    // Reduce the number of requests in flight.
    rqts_in_flight--;

    // Prepare and re-submit the read request.
    if (SonyPHYAndroid::captureThreadShouldRun)
    {
        switch (transfer->status)
        {
            case LIBUSB_TRANSFER_COMPLETED:
                if ((transfer->actual_length <= transfer->length) && (transfer->actual_length > 0)) {
                    //_SONY_PHY_ANDROID_WARN("received %p, len: %d", transfer->buffer, transfer->actual_length);

                    SonyPHYAndroid::RxDataCallback(transfer->buffer, transfer->actual_length);
                    success_count++;

                } else {
                    _SONY_PHY_ANDROID_WARN("\n xfer_callback() : Invalid Buffer, not processed ");
                }
                break;
            case LIBUSB_TRANSFER_ERROR:     // Fall through
            case LIBUSB_TRANSFER_TIMED_OUT: // Fall through
            case LIBUSB_TRANSFER_CANCELLED: // Fall through
            case LIBUSB_TRANSFER_STALL:     // Fall through
            case LIBUSB_TRANSFER_NO_DEVICE: // Fall through
            case LIBUSB_TRANSFER_OVERFLOW:  // Fall through
                _SONY_PHY_ANDROID_WARN("\n xfer_callback() : ERROR: %d", transfer->status);
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

static void readFromUsbDemodEndpointRxTs() {

    int  rStatus;

    struct libusb_transfer **transfers = NULL;      // List of transfer structures.
    unsigned char **databuffers = NULL;         // List of data buffers.

    // Allocate buffers and transfer structures
    bool allocfail = false;

    databuffers = (unsigned char **)calloc(queuedepth, sizeof(unsigned char *));
    transfers = (struct libusb_transfer **)calloc(queuedepth, sizeof(struct libusb_transfer *));

    if ((databuffers != NULL) && (transfers != NULL))
    {
        for (unsigned int i = 0; i < queuedepth; i++)
        {
            databuffers[i] = (unsigned char *)malloc(reqsize * pktsize);
            //databuffers[i] = libusb_dev_mem_alloc(SonyPHYAndroid::Libusb_device_handle, reqsize * pktsize);

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
        _SONY_PHY_ANDROID_WARN("Failed to allocate buffers and transfer structures");
        free_transfer_buffers(databuffers, transfers);
        return;
    }


    //  // Launch all the transfers till queue depth is complete
    //ResetDataEp();
    for (unsigned int i = 0; i < queuedepth; i++)
    {
        libusb_fill_bulk_transfer(transfers[i], SonyPHYAndroid::Libusb_device_handle, SonyPHYAndroid::SONY_USB_ENDPOINT_RX_TS,
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

    while (SonyPHYAndroid::captureThreadShouldRun)
    {
        libusb_handle_events_timeout_completed(NULL, &tv_thread_running_events, &libusb_running_events_completed);
    }
    // Set the stop_transfers flag and wait until all transfers are complete.

    _SONY_PHY_ANDROID_WARN("Stopping transfers: \n");
    _SONY_PHY_ANDROID_WARN("%s:%d: stopping transfers with %d inflight", __FILE__, __LINE__, rqts_in_flight);

    //jjustman-2020-08-23 - workaround for blocking libusb_handle_events which has a 60s timeout by default
    int libusb_event_spin_count = 0;
    int libusb_events_completed = 0;

    struct timeval tv_shutdown;
    tv_shutdown.tv_sec = 0;
    tv_shutdown.tv_usec = 100000;

    while (rqts_in_flight != 0 && (libusb_event_spin_count++ < 100))
    {
        _SONY_PHY_ANDROID_WARN("\n %d requests are pending", rqts_in_flight);
        _SONY_PHY_ANDROID_WARN("%s:%d: stopping transfers with %d inflight, spin count: %d", __FILE__, __LINE__, rqts_in_flight, libusb_event_spin_count);

        libusb_handle_events_timeout_completed(NULL, &tv_shutdown, &libusb_events_completed);
    }

    if(rqts_in_flight > 0) {
        _SONY_PHY_ANDROID_WARN("%s:%d: ignoring remaining transfer with %d inflight - WARNING: this may leak transfer buffers, databuffers: %p, transfers: %p, and may crash with destroyedMutex exception", __FILE__, __LINE__, rqts_in_flight, databuffers, transfers);
    } else {
        // All transfers are complete. We can now free up all structures.
        _SONY_PHY_ANDROID_WARN("\n Transfers completed, freeing databuffers: %p, transfers: %p", databuffers, transfers);
        free_transfer_buffers(databuffers, transfers);
    }


    isCaptStarted = 0;
    _SONY_PHY_ANDROID_WARN("SonyPHYAndroid: thread complete and returning");
    return;
}


int SonyPHYAndroid::processThread()
{
    _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::processThread: starting with this: %p", this);
    this->pinConsumerThreadAsNeeded();
    this->processThreadIsRunning = true;

    this->resetProcessThreadStatistics();

    while (this->processThreadShouldRun)
    {
        //_Sony_PHY_ANDROID_DEBUG("SonyPHYAndroid::ProcessThread: getDataSize is: %d", CircularBufferGetDataSize(cb));

        while(CircularBufferGetDataSize(this->cb_tlv) >= TLV_CIRCULAR_BUFFER_MIN_PROCESS_SIZE) {
            processTLVFromCallbackInvocationCount++;
            this->processTLVFromCallback();
        }

        //jjustman - try increasing to 50ms? shortest atsc3 subframe?
        usleep(33000); //jjustman-2022-02-16 - peg us at 16.67ms/2 ~ 8ms
        // pegs us at ~ 30 spinlocks/sec if no data
    }

    this->releasePinnedConsumerThreadAsNeeded();
    this->processThreadIsRunning = false;

    _SONY_PHY_ANDROID_INFO("SonyPHYAndroid::ProcessThread complete");

    return 0;
}




int SonyPHYAndroid::statusThread() {
    _SONY_PHY_ANDROID_DEBUG("SonyPHYAndroid::statusThread: starting with this: %p", this);

    this->pinStatusThreadAsNeeded();
    this->statusThreadIsRunning = true;

    while(this->statusThreadShouldRun) {
        atsc3_ndk_phy_client_rf_metrics_t atsc3_ndk_phy_client_rf_metrics = { '0' };

        GetStatisticRequest Request;
        int br_idx = 0, ts_idx = 0;

        Request.error = DL_Demodulator_getStatistic (DC, &Request, br_idx, ts_idx);
        if (Request.error) {
            _SONY_PHY_ANDROID_ERROR("DL_Demodulator_getStatistic: error: %d", Request.error);
        } else {
            _SONY_PHY_ANDROID_INFO("DL_Demodulator_getStatistic: signalPresented: %d, signalLocked: %d, signalQuality: %d, signalStrength: %d, snr: %d, rf_power: %d",
                                   Request.statistic.signalPresented,
                                   Request.statistic.signalLocked,
                                   Request.statistic.signalQuality,
                                   Request.statistic.signalStrength,
                                   Request.snr,
                                   Request.rf_power);

        }

        atsc3_ndk_phy_client_rf_metrics.demod_lock = Request.statistic.signalLocked;

        atsc3_ndk_phy_client_rf_metrics.snr1000_global = Request.snr * 1000;
        atsc3_ndk_phy_client_rf_metrics.rssi_1000 = Request.rf_power * 1000;

        for(int i=0; i < 4; i++) {
            if(i < plp_configuration_data.TotalPLPnum) {
                atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[i].plp_id = plp_configuration_data.PLPIDArray[i];
            } else {
                atsc3_ndk_phy_client_rf_metrics.phy_client_rf_plp_metrics[i].plp_id = 0xFF;
            }
        }

        if(atsc3_ndk_phy_bridge_get_instance()) {
            atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_stats_from_atsc3_ndk_phy_client_rf_metrics_t(&atsc3_ndk_phy_client_rf_metrics);
            atsc3_ndk_phy_bridge_get_instance()->atsc3_update_rf_bw_stats(sonyPHYAndroid->ip_completed_packets_parsed,
                                                                          sonyPHYAndroid->alp_total_bytes,
                                                                          sonyPHYAndroid->alp_total_LMTs_recv);
        }

        usleep(1000000);
    }
	this->statusThreadIsRunning = false;
    return 0;
}

block_t* current_reassembeled_baseband_alp_frame = nullptr;

void SonyPHYAndroid::processTLVFromCallback() {
    int innerLoopCount = 0;
    int innerLoggingLoopCount = 0;
    int tlv_bytesRead = 0;

    queue<block_t*> local_assembly_tlv_buffer_queue_for_alp_extraction;

    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex, std::defer_lock);
    //promoted from unique_lock, std::defer_lock to recursive_mutex: atsc3_sl_tlv_block_mutex

    //jjustman-2020-12-07 - loop while we have data in our cb, or break if cb_should_discard is true
    while(true && !SonyPHYAndroid::cb_should_discard) {
        CircularBufferMutex_local.lock();
        tlv_bytesRead = CircularBufferPop(cb_tlv, TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE, (char*)tlv_processDataBufferForCallback);
        CircularBufferMutex_local.unlock();

        //jjustman-2021-01-19 - if we don't get any data back, or the cb_should_discard flag is set, bail
        if(!tlv_bytesRead || SonyPHYAndroid::cb_should_discard) {
            _SONY_PHY_ANDROID_TRACE("atsc3NdkClientSlImpl::processTLVFromCallback() - empty read from CircularBufferPop, got %d bytes, but expected: %d", tlv_bytesRead, TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE);
            break;
        }

        if (!atsc3_sl_tlv_block) {
            _SONY_PHY_ANDROID_DEBUG("ERROR: atsc3NdkClientSlImpl::processTLVFromCallback - atsc3_sl_tlv_block is NULL!");
            allocate_atsc3_sl_tlv_block();
        }

        block_Write(atsc3_sl_tlv_block, (uint8_t *) tlv_processDataBufferForCallback, tlv_bytesRead);
        block_Rewind(atsc3_sl_tlv_block);

        if (block_Remaining_size(atsc3_sl_tlv_block) < TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE) {
            _SONY_PHY_ANDROID_DEBUG("atsc3NdkClientSlImpl::processTLVFromCallback() - short read from CircularBufferPop, got %d bytes, but expected: %d", tlv_bytesRead, TLV_CIRCULAR_BUFFER_PROCESS_BLOCK_SIZE);
            //release our recursive lock early as we are bailing this method
//            atsc3_sl_tlv_block_mutex.unlock();
            //break;
        }

        _SONY_PHY_ANDROID_TRACE("processTLVFromCallback - got block size of: %d bytes", block_Remaining_size(atsc3_sl_tlv_block));

        uint8_t to_check_ts_header_bytes[3] = { 0 };

        while(block_Remaining_size(atsc3_sl_tlv_block) > 3000) {

            uint8_t ts_payload_alp_start_byte_pos   = 0;
            bool 	ts_payload_is_sync_start_flag = false;
            bool 	ts_payload_is_continuation = false;
            bool    ts_payload_tei_flag = false;

            for(int i=0; i < 3; i++) {
                to_check_ts_header_bytes[i] = block_Read_uint8(atsc3_sl_tlv_block);
            }

            //package_start_indicator and optional transport error indicator
            if(to_check_ts_header_bytes[0] == 0x47 && (to_check_ts_header_bytes[1] == 0x40 || to_check_ts_header_bytes[1] == 0xC0) && to_check_ts_header_bytes[2] == 0x2D) {
                ts_payload_tei_flag = (to_check_ts_header_bytes[1] >> 7) & 0x1;

                ts_payload_is_sync_start_flag = true;
                ts_payload_alp_start_byte_pos = block_Read_uint8(atsc3_sl_tlv_block) ; //- 1; //because we've already consumed this byte..

                if(innerLoggingLoopCount++ % 10000 == 0) {
                    _SONY_PHY_ANDROID_DEBUG("ts_payload_is_sync_start_flag\ttei flag: %d\tpos\t%d, bytes: 0x%02x 0x%02x 0x%02x 0x%02x, alp offset in tlv: %d",
                                           ts_payload_tei_flag, atsc3_sl_tlv_block->i_pos,
                                           to_check_ts_header_bytes[0], to_check_ts_header_bytes[1], to_check_ts_header_bytes[2], ts_payload_alp_start_byte_pos,
                                            ts_payload_alp_start_byte_pos);
                }

                //carry over and push to our local assembly queue
                if(current_reassembeled_baseband_alp_frame) {

                   if(ts_payload_alp_start_byte_pos) {
                       block_AppendFromSrciPosToSizeAndMoveIptrs(current_reassembeled_baseband_alp_frame, atsc3_sl_tlv_block, ts_payload_alp_start_byte_pos);
                   }

                    _SONY_PHY_ANDROID_TRACE("pushing frame size: %d", current_reassembeled_baseband_alp_frame->p_size);
                    local_assembly_tlv_buffer_queue_for_alp_extraction.push(current_reassembeled_baseband_alp_frame);
                    current_reassembeled_baseband_alp_frame = NULL; //transfer ownership
                } else if(ts_payload_alp_start_byte_pos) {
                    //we need to move our ptr forward to the start of the first frame
                    block_Seek_Relative(atsc3_sl_tlv_block, ts_payload_alp_start_byte_pos);
                }

                uint8_t starting_bb_alp_length = 184 - ts_payload_alp_start_byte_pos;
                if(current_reassembeled_baseband_alp_frame) {
                    _SONY_PHY_ANDROID_WARN("discarding non-enqueued current_reassembeled_baseband_alp_frame: %p, p_size: %d", current_reassembeled_baseband_alp_frame, current_reassembeled_baseband_alp_frame->p_size);
                    block_Destroy(&current_reassembeled_baseband_alp_frame);
                }
                current_reassembeled_baseband_alp_frame = block_Duplicate_from_position_and_sizeAndMoveIptrs(atsc3_sl_tlv_block, starting_bb_alp_length);

                if(innerLoggingLoopCount++ % 10000 == 0) {
                    _SONY_PHY_ANDROID_DEBUG(
                            "ts_payload_is_sync_start_flag\ttei flag: %d\tfirst 4 bytes: 0x%02x 0x%02x 0x%02x 0x%02x",
                            ts_payload_tei_flag,
                            current_reassembeled_baseband_alp_frame->p_buffer[0],
                            current_reassembeled_baseband_alp_frame->p_buffer[1],
                            current_reassembeled_baseband_alp_frame->p_buffer[2],
                            current_reassembeled_baseband_alp_frame->p_buffer[3]);
                }

            } else if(to_check_ts_header_bytes[0] == 0x47 && (to_check_ts_header_bytes[1] == 0x00 || to_check_ts_header_bytes[1] == 0x80) && to_check_ts_header_bytes[2] == 0x2D) {
                ts_payload_tei_flag = (to_check_ts_header_bytes[1] >> 7) & 0x1;

                ts_payload_is_continuation = true;

                if(innerLoggingLoopCount++ % 10000 == 0) {
                    _SONY_PHY_ANDROID_DEBUG("ts_payload_is_continuation\ttei flag: %d\tpos\t%d, bytes: 0x%02x 0x%02x 0x%02x", ts_payload_tei_flag, atsc3_sl_tlv_block->i_pos,
                                        to_check_ts_header_bytes[0], to_check_ts_header_bytes[1], to_check_ts_header_bytes[2]);
                }


                if(current_reassembeled_baseband_alp_frame) {
                    block_AppendFromSrciPosToSizeAndMoveIptrs(current_reassembeled_baseband_alp_frame, atsc3_sl_tlv_block, 185);
                } else {
                    //discard but move our atsc3_sl_tlv_block forward
                    block_Seek_Relative(atsc3_sl_tlv_block, 185);
                }
            } else {
                //walk thru +1
                _SONY_PHY_ANDROID_TRACE("seeking thru ts missing sync, i_pos: %d, current_ts_header_bytes: 0x%02x 0x%02x 0x%02x 0x%02x", atsc3_sl_tlv_block->i_pos,
                                       to_check_ts_header_bytes[0],
                                       to_check_ts_header_bytes[1],
                                       to_check_ts_header_bytes[2]
                                       );

                block_Seek_Relative(atsc3_sl_tlv_block, -2); //remember, we consumed 4 bytes already, so walk back 3 to find the next startr
                continue;
            }
        }

        //truncate and loop
        //todo - use a stable back-buffer for looping
        uint32_t remaining_bytes = block_Remaining_size(atsc3_sl_tlv_block);
        block_t* temp_block = block_Duplicate_from_position(atsc3_sl_tlv_block);
        block_Seek(temp_block, remaining_bytes); //so we can append the next tlv buffer read

        block_Destroy(&atsc3_sl_tlv_block);
        atsc3_sl_tlv_block = temp_block;
    }

    //push our frames for alp processing
    if(local_assembly_tlv_buffer_queue_for_alp_extraction.size()) {
        lock_guard<mutex> tlv_buffer_queue_for_alp_extraction_guard(tlv_buffer_queue_for_alp_extraction_mutex);
        while(local_assembly_tlv_buffer_queue_for_alp_extraction.size()) {
            tlv_buffer_queue_for_alp_extraction.push(local_assembly_tlv_buffer_queue_for_alp_extraction.front());
            local_assembly_tlv_buffer_queue_for_alp_extraction.pop();
        }
        tlv_buffer_queue_for_alp_extraction_notification.notify_one();
    }
}

int SonyPHYAndroid::processAlpFromCircularBufferThread() {
    int alpProcessedLoopCounter = 0;

    //jjustamn-2022-08-12 - hack-ish

    alpConsumerJniEnv = new Atsc3JniEnv(atsc3_ndk_phy_sony_static_loader_get_javaVM(), "SonyPHYAndroid::processAlpFromCircularBufferThread");

    if(atsc3_ndk_application_bridge_get_instance()) {
        atsc3_ndk_application_bridge_get_instance()->pinConsumerThreadAsNeeded();
    }

    queue<block_t*> local_extraction_tlv_buffer_queue_for_alp_frames;

    int alp_bytesReadFromCB = 0;
    uint8_t my_plp = 0xFF;
    uint64_t l1dTimeNs_value_last = 0;

    processAlpThreadIsRunning = true;

    //jjustman-2020-12-07 - loop while we have data in our cb, or break if cb_should_discard is true
    while(processAlpThreadShouldRun) {

        {
            unique_lock<mutex> condition_lock(tlv_buffer_queue_for_alp_extraction_mutex);
            tlv_buffer_queue_for_alp_extraction_notification.wait(condition_lock);

            while (tlv_buffer_queue_for_alp_extraction.size()) {
                local_extraction_tlv_buffer_queue_for_alp_frames.push(tlv_buffer_queue_for_alp_extraction.front());
                tlv_buffer_queue_for_alp_extraction.pop();
            }
            condition_lock.unlock();
        }

        while(processAlpThreadShouldRun && local_extraction_tlv_buffer_queue_for_alp_frames.size()) {

            block_t* atsc3_baseband_alp_payload = local_extraction_tlv_buffer_queue_for_alp_frames.front();
            local_extraction_tlv_buffer_queue_for_alp_frames.pop();
            block_Rewind(atsc3_baseband_alp_payload);

            alp_total_bytes += block_Remaining_size(atsc3_baseband_alp_payload);

            //_SONY_PHY_ANDROID_DEBUG("processALPFromCircularBufferThread - loop, atsc3_baseband_alp_payload is: %p, i_pos: %d, p_size: %d", atsc3_baseband_alp_payload, atsc3_baseband_alp_payload->i_pos, atsc3_baseband_alp_payload->p_size);

            while(block_Remaining_size(atsc3_baseband_alp_payload) > 5) {

                atsc3_alp_packet_t* atsc3_alp_packet = atsc3_alp_packet_parse(my_plp, atsc3_baseband_alp_payload);


                if(!atsc3_alp_packet) {
                    _SONY_PHY_ANDROID_WARN("parse returned NULL alp packet, pre-parse i_ptr: %d, pending_alp_packet->i_pos: %d, p_size: %d", pre_parse_alp_packet_rollback_ptr_i_pos, atsc3_baseband_alp_payload->i_pos, atsc3_baseband_alp_payload->p_size);
					continue;
                }

//                _SONY_PHY_ANDROID_TRACE("after atsc3_alp_packet_parse\t%p\t%p", atsc3_alp_packet, atsc3_alp_packet->alp_payload);

//                if(!atsc3_alp_packet->is_alp_payload_complete) {
//                    _SONY_PHY_ANDROID_DEBUG("alp payload NOT complete! pre-parse i_ptr: %d, pending_alp_packet->i_pos: %d, p_size: %d, alp_length: %d",
//                                            pre_parse_alp_packet_rollback_ptr_i_pos,
//                                            atsc3_baseband_alp_payload->i_pos, atsc3_baseband_alp_payload->p_size, atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length);
//                    //block_Destroy(&atsc3_baseband_alp_payload);
//                    continue;
//                }

                //_SONY_PHY_ANDROID_DEBUG("atsc3_alp_packet->alp_packet_header.packet_type is: 0x%02x, len: %d", atsc3_alp_packet->alp_packet_header.packet_type, atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length);

                //do our link mapping table first

                if (atsc3_alp_packet->alp_packet_header.packet_type == 0x4) {
                    //_SONY_PHY_ANDROID_DEBUG("alp: LMT: total count: %d", alp_total_LMTs_recv);

                    alp_total_LMTs_recv++;
                    atsc3_link_mapping_table_t *atsc3_link_mapping_table_pending = atsc3_alp_packet_extract_lmt(atsc3_alp_packet);

                    if (atsc3_phy_rx_link_mapping_table_process_callback && atsc3_link_mapping_table_pending) {
                        atsc3_link_mapping_table_t *atsc3_link_mapping_table_to_free = atsc3_phy_rx_link_mapping_table_process_callback(atsc3_link_mapping_table_pending);
                        if (atsc3_link_mapping_table_to_free) {
                            atsc3_link_mapping_table_free(&atsc3_link_mapping_table_to_free);
                        }
                    }
                }

                if (atsc3_alp_packet->alp_packet_header.packet_type == 0x00) {

                    //dispatch our IP datagram, otherwise check for header extension mode(s) for plp and ptp
                    if(atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.length) {

                        if((alpProcessedLoopCounter++ % 10000) == 0) {
                            _SONY_PHY_ANDROID_DEBUG("alp: IP: payload len: %d, first two bytes: 0x%02x 0x%02x", atsc3_alp_packet->alp_payload->p_size, atsc3_alp_packet->alp_payload->p_buffer[0], atsc3_alp_packet->alp_payload->p_buffer[1]);
                        }

                        block_Rewind(atsc3_alp_packet->alp_payload);
                        if (atsc3_phy_rx_udp_packet_process_callback) {
                            atsc3_phy_rx_udp_packet_process_callback(my_plp, atsc3_alp_packet->alp_payload);
                            ip_completed_packets_parsed++;
                        }

                    } else if(atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.HEF) {

                        // _SONY_PHY_ANDROID_DEBUG("header_extension: HEF extension type is: 0x%02x", atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_type);
                        if(atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_type == 0xF1) {

                            //check for mis-varlen reads
                            if(atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_byte) {
                                my_plp = (atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_byte[0] >> 2) & 0x3F;
                                //_SONY_PHY_ANDROID_DEBUG("header_extension: myplp is now: 0x%02x", my_plp);
                            }

                        } else if(atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_type == 0xF0) {

                            if(atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_byte) {
                                block_t* l1d_timeinfo_blockt = block_Duplicate_from_ptr(atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_byte, atsc3_alp_packet->alp_packet_header.alp_packet_header_mode.alp_single_packet_header.header_extension.extension_length_minus1 + 1);
                                block_Rewind(l1d_timeinfo_blockt);

                                uint8_t  time_info_flag = block_Read_uint8_bitlen(l1d_timeinfo_blockt, 2);
                                uint64_t time_info_sec  = block_Read_uint64_bitlen(l1d_timeinfo_blockt, 32);
                                uint64_t time_info_msec = block_Read_uint64_bitlen(l1d_timeinfo_blockt, 10);
                                uint64_t time_info_usec = block_Read_uint64_bitlen(l1d_timeinfo_blockt, 10);
                                uint64_t time_info_nsec = block_Read_uint64_bitlen(l1d_timeinfo_blockt, 10);

                                uint64_t l1dTimeNs_value = (time_info_sec * 1000000000) + (time_info_msec * 1000000) + (time_info_usec * 1000) + time_info_nsec;
                                if(l1dTimeNs_value != l1dTimeNs_value_last) {
                                    _SONY_PHY_ANDROID_DEBUG("header_extension: l1d_time_info_flag: 0x%01x, time_sec: %0.3d, time_msec: %0.3d, time_usec: %0.3d, time_nsec: %0.3d, l1d_time_val_ns: %" PRIu64 "",
                                            time_info_flag,
                                            time_info_sec,
                                            time_info_msec,
                                            time_info_usec,
                                            time_info_nsec,
                                            l1dTimeNs_value);

                                    if (atsc3_ndk_phy_bridge_get_instance()) {
                                        atsc3_ndk_phy_bridge_get_instance()->atsc3_update_l1d_time_information(time_info_flag,time_info_sec, time_info_msec, time_info_usec, time_info_nsec);
                                    }
                                    l1dTimeNs_value_last = l1dTimeNs_value;
                                }
                            }
                        }
                    }
				}

//				_SONY_PHY_ANDROID_TRACE("before atsc3_alp_packet_free\t%p\t%p", atsc3_alp_packet, atsc3_alp_packet->alp_payload);
				atsc3_alp_packet_free(&atsc3_alp_packet);
            }

            block_Destroy(&atsc3_baseband_alp_payload);
        }
        //thr.yield
        usleep(1000);
    }

    processAlpThreadIsRunning = false;

    return 0;
}

void SonyPHYAndroid::RxDataCallback(unsigned char *data, long len) {
    if(SonyPHYAndroid::cb_should_discard) {
        return;
    }

    _SONY_PHY_ANDROID_DEBUG("atsc3NdkClientSlImpl::RxDataCallback: pushing data: %p, len: %d", data, len);
    unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);

    if(SonyPHYAndroid::cb_tlv) {
        CircularBufferPush(SonyPHYAndroid::cb_tlv, (char *) data , len);
    }
    CircularBufferMutex_local.unlock();
}