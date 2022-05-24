//
// Created by Jason Justman on 5/24/22.
//

#include "TolkaPHYAndroid.h"


//jjustman-2022-05-24 - todo: use jni ref resolution for this ptr
TolkaPHYAndroid* tolkaPHYAndroid = nullptr;
CircularBuffer TolkaPHYAndroid::cb = nullptr;
libusb_device_handle* TolkaPHYAndroid::Libusb_device_handle = nullptr;

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

    memset(&endeavour, 0, sizeof(Endeavour));
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
                    _TOLKA_PHY_ANDROID_INFO("libusb_set_interface_alt_setting, TOLKA_USB_ENDPOINT_RX, ret: %d, interface: %d, endpoint: %d (0x%02x)", ret, i, TOLKA_USB_ENDPOINT_TX);
                } else if (k == 1) { //hack endpointDesc->bEndpointAddress == TOLKA_USB_ENDPOINT_TX) {
                //    ret = libusb_set_interface_alt_setting(Libusb_device_handle, i, j);
                    TOLKA_USB_ENDPOINT_TX = endpointDesc->bEndpointAddress;

                    _TOLKA_PHY_ANDROID_INFO("libusb_set_interface_alt_setting, TOLKA_USB_ENDPOINT_TX, ret: %d, interface: %d, endpoint: %d (0x%02x)", ret, i, TOLKA_USB_ENDPOINT_RX);
                } else if (k == 2) { //hack ->bEndpointAddress == TOLKA_USB_ENDPOINT_RX_TS) {
                    TOLKA_USB_ENDPOINT_RX_TS = endpointDesc->bEndpointAddress;
                //    ret = libusb_set_interface_alt_setting(Libusb_device_handle, i, j);
                    _TOLKA_PHY_ANDROID_INFO("libusb_set_interface_alt_setting, TOLKA_USB_ENDPOINT_RX_TS, ret: %d, interface: %d, endpoint: %d (0x%02x)", ret, i, TOLKA_USB_ENDPOINT_RX_TS);
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
epRead:   2022-05-24 05:40:25.897 27647-27647/com.example.endeavour_SL3000_R855.debug V/Endeavour.Bus: EP(0),
        addr = 0x81, attr = 2, dir = 128, num = 1, intval = 0, maxSize =512
epWrite:  2022-05-24 05:41:07.680 27647-27647/com.example.endeavour_SL3000_R855.debug V/Endeavour.Bus: EP(1),
        addr = 0x2, attr = 2, dir = 0, num = 2, intval = 0, maxSize =512
epReadTs: 2022-05-24 05:41:44.177 27647-27647/com.example.endeavour_SL3000_R855.debug V/Endeavour.Bus: EP(2),
        addr = 0x84, attr = 2, dir = 128, num = 4, intval = 0, maxSize =512
     */

    _TOLKA_PHY_ANDROID_INFO("discovered endpoints are: TOLKA_USB_ENDPOINT_RX: %d (0x%02x), TOLKA_USB_ENDPOINT_TX: %d (0x%02x), TOLKA_USB_ENDPOINT_RX_TS: %d (0x%02x)",
                            TOLKA_USB_ENDPOINT_RX,
                            TOLKA_USB_ENDPOINT_TX,
                            TOLKA_USB_ENDPOINT_RX_TS);


    BrUser_createCriticalSection();

    //Init Endeavour
    endeavour.ctrlBus = BUS_USB;
    endeavour.maxBusTxSize = 63;
    endeavour.bypassScript = True;
    endeavour.bypassBoot = False;
    endeavour.chipCount = 1;
    endeavour.gator[0].existed = True;
    endeavour.gator[0].outDataType = OUT_DATA_USB_DATAGRAM;
    endeavour.gator[0].outTsPktLen = PKT_LEN_188;

    //jjustman-2022-05-19: reg_ts0_tag_len - 0 -> 188
    endeavour.tsSource[0][0].existed = True;
    endeavour.tsSource[0][0].tsType  = TS_SERIAL;
    endeavour.tsSource[0][0].tsPort  = TS_PORT_0;
    endeavour.tsSource[0][0].tsPktLen = PKT_LEN_188;
    endeavour.tsSource[0][0].syncByte = 0x47;

    endeavour.gator[0].booted = False;
    endeavour.gator[0].initialized = False;

    long error = 0;
    uint8_t tmp0[1];
    uint8_t tmp1[1];
    long data_cnt;

    IT9300_readRegister(&endeavour, 0, 0xDA98 , tmp0);
    IT9300_readRegister(&endeavour, 0, 0xDA99 , tmp1);
    data_cnt = 4*((tmp1[0] * 256) + tmp0[0]);

    error = IT9300_getFirmwareVersion (&endeavour, 0);
    if (error) {
        _TOLKA_PHY_ANDROID_ERROR("open: IT9300_getFirmwareVersion fail! \n");
        return -1;
    }

    _TOLKA_PHY_ANDROID_INFO("open: IT9300_getFirmwareVersion is: %d", endeavour.firmwareVersion);
    
    error = IT9300_initialize(&endeavour, 0);
    if(error)
    {
        _TOLKA_PHY_ANDROID_ERROR("IT9300 initialize failed, error = 0x%lx", error);
        return -1;
    }

    int num_of_tuner = 1;
    for (int i = 0; i < num_of_tuner; i++) {
        if (endeavour.tsSource[0][i].existed == True) {
            error = IT9300_enableTsPort(&endeavour, 0, i);
            if (error) {
                _TOLKA_PHY_ANDROID_ERROR("IT9300 IT9300_enableTsPort, port: %d, error = 0x%lx", i, error);
            }
        }

        //jjustman-2022-05-19
        //set this register by hand, as the impl assigns the value of 1 - error = IT9300_setSyncByteMode(&endeavour, 0, 0);
        //instead of using a struct member for the proper value extraction in the default impl

        error = IT9300_writeRegister (&endeavour, 0, p_br_reg_ts0_aggre_mode + i, 0);//0:tag 1:sync  2:remap
        if(error) {
            _TOLKA_PHY_ANDROID_ERROR("IT9300 set ignore fail pin failed, error = 0x%lx", error);
            return -1;
        }

        //ts0_tei_modify?
        error = IT9300_writeRegister (&endeavour, 0, p_br_mp2if_ts0_tei_modify + i, 0);//0:tag 1:sync  2:remap
        if(error) {
            _TOLKA_PHY_ANDROID_ERROR("IT9300 set ignore fail pin failed, error = 0x%lx", error);
            return -1;
        }

        //                (endeavour->tsSource[chip][tsSrcIdx]).tsPktLen);
        error = IT9300_setInTsPktLen(&endeavour, 0, i);
        if(error) {
            _TOLKA_PHY_ANDROID_ERROR("IT9300 set IT9300_setInTsPktLen failed, error = 0x%lx", error);
            return -1;
        }

//how to handle ts 0x47 sync byte wrangling...?
        //IT9300_setSyncByteMode
//        error = IT9300_setSyncByteMode(&endeavour, 0, i);
//        if(error)
//        {
//            printf("IT9300 set IT9300_setSyncByteMode failed, error = 0x%lx\n", error);
//            goto exit;
//        }

#if 0
        endeavour.tsSource[0][0].syncByte = 0x40;
            endeavour.tsSource[0][1].syncByte = 0x41;
            endeavour.tsSource[0][2].syncByte = 0x42;
            endeavour.tsSource[0][3].syncByte = 0x43;
            error = IT9300_setSyncByteMode(&endeavour, 0, 0);
            if (error) goto exit;
            error = IT9300_setSyncByteMode(&endeavour, 0, 1);
            if (error) goto exit;
            error = IT9300_setSyncByteMode(&endeavour, 0, 2);
            if (error) goto exit;
            error = IT9300_setSyncByteMode(&endeavour, 0, 3);
            if (error) goto exit;
            error = IT9300_enableTsPort(&endeavour, 0, 0);
            if (error) goto exit;
            error = IT9300_enableTsPort(&endeavour, 0, 1);
            if (error) goto exit;
            error = IT9300_enableTsPort(&endeavour, 0, 2);
            if (error) goto exit;
            error = IT9300_enableTsPort(&endeavour, 0, 3);
            if (error) goto exit;
#endif

        //Ignore TS fail pin
        error = IT9300_writeRegister(&endeavour, 0, p_br_reg_ts_fail_ignore, 0x1F); //necessary
        if(error) {
            _TOLKA_PHY_ANDROID_ERROR("IT9300 set ignore fail pin failed, error = 0x%lx", error);
            return -1;
        }

        _TOLKA_PHY_ANDROID_INFO("open(): IT9300 initialize ok");
    }


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