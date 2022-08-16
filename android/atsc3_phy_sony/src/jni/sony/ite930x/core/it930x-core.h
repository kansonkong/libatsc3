#ifndef _IT930x_H_
#define _IT930x_H_

#define _GNU_SOURCE

#define deb_data
#define printk
#define mutex_lock //
#define mutex_unlock //

//	DC->Si2168B_FE[tsSrcIdx] = kzalloc(sizeof(Si2168B_L2_Context), GFP_KERNEL);
#define kzalloc(...) 0

#include <unistd.h>		// usleep
#define msleep usleep
#define mdelay usleep
#define GFP_KERNEL

#define do_div(...) 1
//ugh...

#include <stdint.h>
#include <stdbool.h>




#if defined (__cplusplus)
extern "C" {
#endif

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;

typedef bool atomic_t;
typedef void* spinlock_t;
typedef void* wait_queue_head_t;
//typedef void* kref;

// #include <linux/types.h>
//
//#include <linux/kernel.h>
//#include <linux/errno.h>
//#include <linux/init.h>
//#include <linux/slab.h>
//#include <linux/module.h>
//#include <linux/kref.h>
//#include <linux/usb.h>
//#include <linux/version.h>
//#include <linux/mutex.h>
//#include <linux/stddef.h>
//#include <linux/spinlock.h>
//#include <linux/gfp.h>
//#include <linux/gfp.h>
//#include <linux/fs.h>
//#include <linux/reboot.h>
//#include <linux/notifier.h>
//#include <asm/uaccess.h>
//#include <linux/uaccess.h>
#include "iocontrol.h"
#include "IT9300.h"
#include "IT9133.h"
#include "userdef.h"
#include "type.h"
#include "brType.h"
#include "firmware.h"
#include "firmware_V2.h"
#include "firmware_V2I.h"
#include "version.h"
#include "brfirmware.h"
#include "brRegister.h"
#include "usb2impl.h"
#include <dibx09x_common.h>
#include <mxl_eagle_apis.h>
#include <sony_cxd_apis.h>
//#include <linux/stddef.h>
//#include <linux/delay.h>
//#include <linux/types.h>
//#include <linux/stddef.h>
//#include <linux/uio.h>

#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#endif

//#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 39)
//	#include <linux/smp_lock.h>
//#endif

/* Get a minor range for your devices from the usb maintainer */
#ifdef CONFIG_USB_DYNAMIC_MINORS
	#define USB_it930X_MINOR_BASE	0
#else
	#define USB_it930X_MINOR_BASE	192
#endif

#define DRIVER_RELEASE_VERSION	"v20.03.30.1"

/********************* User Define ***************/
#define RX_CHIP_NUMBER 1
#define RX_BOARD_ID 0x50
#define RX_CHIP_TYPE 0x1b

/*********************Rx_ID_Length***************/
#define RX_ID_LENGTH 5
#define RX_DROP_URB_SIZE 3

/***************** customization *****************/

#define URB_COUNT   4
#define BR_URB_COUNT   4
#define BLOCK_BUFFER_SIZE 188*816
//
//#define DEBUG 1
//#if DEBUG
//	#define deb_data(args...)   printk(args)
//#else
//	#define deb_data(args...)
//#endif

#define BCAS_ENABLE 0//For bcas enable/disable : 1/0

#define WORK_QUEUE_INIT		1
//#define PATCH_FOR_NX		1

/***************** from compat.h *****************/
/*

#if LINUX_VERSION_CODE <=  KERNEL_VERSION(2,6,18)
typedef int bool;
#define true	1
#define false	0
#endif
*/

/***************** from device.h *****************/
#define SLAVE_DEMOD_2WIREADDR		0x3A
#define TS_PACKET_SIZE				188
#define TS_PACKET_COUNT_FU			21

/***************** from driver.h *****************/
#define TS_FRAMES_HI 128
#define TS_FRAMES_FU 128
#define MAX_USB20_IRP_NUM  5
#define MAX_USB11_IRP_NUM  2

/***************** from afdrv.h *****************/
//#define GANY_ONLY 0x42F5
#define EEPROM_FLB_OFS  8
#define EEPROM_IRMODE		(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x10)	//00:disabled, 01:HID
#define EEPROM_SELSUSPEND	(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x28)	//Selective Suspend Mode
#define EEPROM_TSMODE		(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x28 + 1)	//0:one ts, 1:dual ts
#define EEPROM_2WIREADDR		(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x28 + 2)	//MPEG2 2WireAddr
#define EEPROM_SUSPEND		(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x28 + 3)	//Suspend Mode
#define EEPROM_IRTYPE		(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x28 + 4)	//0:NEC, 1:RC6
#define EEPROM_SAWBW1		(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x28 + 5)
#define EEPROM_XTAL1			(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x28 + 6)	//0:28800, 1:20480
#define EEPROM_SPECINV1		(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x28 + 7)
#define EEPROM_TUNERID		(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x30 + 4)
#define EEPROM_IFFREQL		(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x30) 
#define EEPROM_IFFREQH		(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x30 + 1)   
#define EEPROM_IF1L			(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x30 + 2)   
#define EEPROM_IF1H			(OVA_EEPROM_CFG + EEPROM_FLB_OFS + 0x30 + 3)
#define EEPROM_SHIFT			(0x10)	//EEPROM Addr Shift for slave front end
#define EEPROM_EVBNUM		(OVA_EEPROM_CFG + 0x38)	//0x4994 + 0x38
#define EEPROM_CHIPTYPE_1stNLC_1	(OVA_EEPROM_CFG + 0x31)
#define EEPROM_CHIPTYPE_1stNLC_2	(OVA_EEPROM_CFG + 0x32)
#define EEPROM_CHIPTYPE_1stNLC_3	(OVA_EEPROM_CFG + 0x35)
#define EEPROM_CHIPTYPE_1stNLC_4	(OVA_EEPROM_CFG + 0x36)
#define EEPROM_CHIPTYPE_otherNLC	(OVA_EEPROM_CFG + 0x37)
#define EEPROM_BOARDID	(OVA_EEPROM_CFG + 0x54)
#define to_afa_dev(d) container_of(d, IT930x_Device it930x_dev, kref)

//belowd enum is for recognizing different receiver from EEPROM
/**********************************************************************
* 
*             Chip Type                   |    enum name 
* ----------------------------------------|--------------------
*  chip_Type[][] = 1 : IT913X             |    EEPROM_IT913X
*                = 5 : Si2168B            |    EPROM_Si2168B
*                = 6 : MxL608 + Atbm782x  |    EEPROM_MxL608_Atbm782x
*                = 7 : Dibcom9090         |    EEPROM_Dibcom9090
*                = a : Dibcom9090_4chan   |    EEPROM_Dibcom9090_4chan
*
************************************************************************/
typedef enum{
	EEPROM_IT913X	= 1,
	EEPROM_Si2168B = 5,
	EEPROM_MxL608_Atbm782x,
	EEPROM_Dibcom9090,
	EEPROM_Dibcom9090_4chan = 10,
	//EEPROM_MXL69X = 15,
	EEPROM_EW100 = 13,
	EEPROM_MXL691 = 15,
	EEPROM_CXD2880_33 = 16,
	EEPROM_CXD2880 = 19,
	EEPROM_MXL691_DUALP = 20,//691
	EEPROM_MXL692 = 21,
	EEPROM_MXL248 = 22,
	EEPROM_CXD285X = 23,
	EEPROM_CXD6801_NUVYYO = 27,
	EEPROM_MXL541C = 28,
	EEPROM_CXD6801_EVB = 29,
	EEPROM_CXD6801_TS7 = 30
}EEPROM_RECEIVER_TYPE;

/***************** debug message *****************/
//define PROBE_DEBUG_MESSAGE

extern struct usb_device *udevs;

/* AirHD no use, RC, after kernel 38 support */
struct it930x_config {
	u8  dual_mode:1;
	u16 mt2060_if1[2];
	u16 firmware_size;
	u16 firmware_checksum;
	u32 eeprom_sum;
};

struct it930x_ofdm_channel {
	u32 RF_kHz;
	u8  Bw;
	s16 nfft;
	s16 guard;
	s16 nqam;
	s16 vit_hrch;
	s16 vit_select_hp;
	s16 vit_alpha;
	s16 vit_code_rate_hp;
	s16 vit_code_rate_lp;
	u8  intlv_native;
};

/* AirHD no use, RC, after kernel 38 support */
struct tuner_priv {
	struct tuner_config *cfg;
	struct i2c_adapter *i2c;
	u32 frequency;
	u32 bandwidth;
	u16 if1_freq;
	u8  fmfreq;
};

typedef struct _TUNER_INFO {
    Bool bTunerInited;
    Bool bSettingFreq;
    Byte TunerId;
    Bool bTunerOK;
	Bool bTunerLock;//AirHD
} TUNER_INFO, *PTUNER_INFO;

typedef struct _FILTER_CONTEXT_HW {
    Dword ulCurrentFrequency;
    Word  ucCurrentBandWidth;  
    Dword ulDesiredFrequency;
    Word  ucDesiredBandWidth;     
    Bool bTimerOn;
    Byte GraphBuilt;
    TUNER_INFO tunerinfo; 
    int  bEnPID;
    Bool bApOn;
    int bResetTs;
    Byte OvrFlwChk;
} FILTER_CONTEXT_HW, *PFILTER_CONTEXT_HW;

struct DIB9090_DEVICE {
	struct dibDataBusHost *i2c;
	struct dibFrontend frontend[4]; //dibFrontend
	struct dibDemodMonitor monitor[4]; //dibDemodMonitor
	Dword frequency;
	Word bandwidth;
};

typedef struct _GPIO_STRUCT {
    Dword GpioOut;
    Dword GpioEn;
    Dword GpioOn;
} GPIO_STRUCT, *PGPIO_STRUCT;

typedef struct Endeavour_DEVICE_CONTEXT{
    FILTER_CONTEXT_HW fc[2];
    struct usb_device *usb_dev;
    Byte DeviceNo;
    Bool bBootCode;
    Bool bEP12Error;
    Bool bEP45Error;
    Bool bDualTs;
    Bool bIrTblDownload;    
    Byte BulkOutData[256];
    u32 WriteLength;
    Bool bSurpriseRemoval;
    Bool bDevNotResp;
    Bool bEnterSuspend;
    Bool bSupportSuspend;
    Bool bSupportSelSuspend;
    u16 regIdx; 
    Byte eepromIdx;
    u16 UsbMode;
    u16 MaxPacketSize;
    u32 MaxIrpSize;
    u32 TsFrames;
    u32 TsFrameSize;
    u32 TsFrameSizeDw;
    u32 TsPacketCount;
    Bool bSelectiveSuspend;
    u32 ulActiveFilter;
    Architecture architecture;
    StreamType StreamType;
    Bool bDCAPIP;
    Bool bSwapFilter;
    Bool bTunerPowerOff;
    Byte UsbCtrlTimeOut;
	//Endeavour endeavour;
	Byte it9300_Number;
	Byte rx_number;
	Endeavour it9300;
	DefaultDemodulator it9133[4][5];
	Byte Si2168B_standardName[4][5];
	Dword *Si2168B_FE[5];
	Byte is_rx_init[4][5];
	Byte chip_Type[4][5];
	MXL_EAGLE_DEV_CONTEXT_T	MxL_EAGLE_Devices[5];
	sony_cxd_CXD285x_driver_instance_t CXD285x_driver[5];
	Word rx_device_id[8];
    Bool ForceWrite;
    Byte chip_version;
    Bool bProprietaryIr;
    Byte bIrType;
	Byte *temp_read_buffer;
	Byte *temp_write_buffer;
	Byte *temp_urb_buffer;
	bool power_state[2];
	Dword parsebufflen;
	Dword device_id;
	Dword board_id;
	bool disconnect;
	char filter_name[16][40];
	bool map_enalbe;
	Byte hihg_byte;
	Byte low_byte;	
	atomic_t filter_count;
	bool auto_regist;
	void* dev_mutex;
	int if_degug;
	struct DIB9090_DEVICE dib9090;
} Device_Context;

/* Structure for urb context */
typedef struct _RX_URB_CONTEXT{
	//struct file *file;
	void *br_chip;
	Byte index;
} Rx_Urb_Context;

/* Structure to hold all of our device specific stuff */
typedef struct _CHIP{
	Byte chip_index;
	
	bool urb_streaming;
	struct urb *urbs[BR_URB_COUNT];	
	Rx_Urb_Context urb_context[BR_URB_COUNT];	
	Byte urbstatus[BR_URB_COUNT];
	Byte *pRingBuffer;
	Byte *br_pRingBuffer[BR_URB_COUNT];
	Dword dwTolBufferSize;
	Byte urb_count;
	Dword CurrBuffPointAddr;
	Dword ReadBuffPointAddr;
	Dword dwReadRemaingBufferSize;			
	
	spinlock_t read_buf_lock;
	void *dev;
	
	///for WriteRingBuffer	
	Dword TxWriteBuffPointAddr;
	Dword dwTxRemaingBufferSize;
	
	int handle_startcapture[4];		//chip open flag
	bool if_chip_start_capture;		//chip open flag	
	Byte chip_open_count;
	Byte acq_transfer_count;	//acq_transfer_count
	bool if_search_syncbyte;
	wait_queue_head_t read_wait_queue;
	bool g_if_wait_read;
	int read_wait_len;
} Chip;

#define usb_interface libusb_device_handle
typedef struct _IT930x_DEVICE{
	struct usb_interface *interface;
//	struct kref kref;
	struct file *file[16];
	Device_Context *DC;
//    struct notifier_block reboot_notifier;  /* default mode before reboot */
	Chip *chip[16];
	Chip *br_chip;
	Byte minor[16];
	bool Tuner_inited[4];
	Byte syncByte;
	bool SetNULLPacket;
	///for returnchannel
	unsigned long DealWith_ReturnChannelPacket;
	// Device Init work queue thread
//	struct work_struct init_work;
	//struct sk_buff_head init_qhead;
//	spinlock_t init_lock;
	unsigned long init_state;
	wait_queue_head_t init_wait;
//	struct workqueue_struct *init_wq;
	atomic_t clean_urb_flag;
} IT930x_Device;

typedef struct _FILE_INFORMATION{
	IT930x_Device *dev;
	Byte chip_index;
	bool if_file_start_capture; //ap open flag
} File_Information;

typedef Dword it930x_ioctl_compat_t(void* dev, unsigned long cmd, unsigned long arg, unsigned char number);
			       
extern int it930x_device_count;
extern int dvb_usb_it930x_debug;
extern Dword IsLock(Device_Context *DC, Bool* locked, int br_idx, int ts_idx);
extern Dword DL_GetEEPROMConfig(Device_Context *DC);
extern Dword DL_device_communication_test(Device_Context *DC);
extern Dword DL_IT930x_device_init(Device_Context *DC);
extern Dword DL_IT930x_device_deinit(Device_Context *DC);
extern Dword DL_DemodIOCTLFun(void* demodulator, Dword IOCTLCode, unsigned long pIOBuffer, Byte chip_index);
extern Dword DL_Demodulator_acquireChannel(Device_Context *DC, AcquireChannelRequest* request, int br_idx, int ts_idx);
extern Dword DL_Demodulator_isLocked (Device_Context *DC, Bool* locked, int br_idx, int ts_idx);
extern Dword DL_Demodulator_getStatistic(Device_Context *DC, GetStatisticRequest* statistic, int br_idx, int ts_idx);
extern Dword DL_Demodulator_getChannelStatistic (Device_Context *DC, Byte chip, ChannelStatistic* channelStatistic, int br_idx, int ts_idx);
extern Dword DL_Demodulator_writeRegisters(Device_Context *DC, Processor processor, Byte option , Dword registerAddress, Byte bufferLength, Byte* buffer, int br_idx, int ts_idx);
extern Dword DL_Demodulator_readRegisters(Device_Context *DC, Processor processor, Byte option , Dword registerAddress, Byte bufferLength, Byte* buffer, int br_idx, int ts_idx);
extern Dword DL_Demodulator_resetPidFilter(Device_Context *DC, int br_idx, int ts_idx);
extern Dword DL_Demodulator_controlPidFilter(Device_Context *DC, Byte control, int br_idx, int ts_idx);
extern Dword DL_Demodulator_addPidToFilter(Device_Context *DC, Byte index, Word value, int br_idx, int ts_idx);
extern Dword DL_Demodulator_removePidFromFilter(Device_Context *DC, Byte index, Word value, int br_idx, int ts_idx);
extern Dword DL_Demodulator_getBoardInputPower(Device_Context *DC, Byte* boardInputPower, int br_idx, int ts_idx);
extern Dword DL_Demodulator_getMPLPID(Device_Context *DC, MPLPData *pMplpdata, int br_idx, int ts_idx);
extern Dword DL_Demodulator_setPLPID(Device_Context *DC, MPLPData *pMplpdata, int br_idx, int ts_idx);
extern Dword DRV_IT913x_Initialize(Device_Context *DC, int br_idx, int tsSrcIdx);
extern Dword DRV_Si2168B_Initialize(Device_Context *DC, int br_idx, int tsSrcIdx);
extern Dword DRV_Si2168B_acquireChannel(Device_Context *DC, Word bandwidth, Dword frequency, int br_idx, int ts_idx);
extern Dword DRV_Si2168B_getStatistic(Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int ts_idx);
extern Dword DRV_Si2168B_getChannelStatistic (Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int ts_idx);
extern Dword DRV_Si2168B_isLocked(Device_Context *DC, Bool *locked, int br_idx, int ts_idx);
extern Dword DL_ResetRxPort(Device_Context *DC, int is_open);
extern Dword DL_Dib9090_getDiversityDetailData(Device_Context *DC, DibcomDiversityData* data, Byte byChipType);
extern Dword DRV_Dib9090_Initialize(Device_Context *DC, Byte byChipType);
extern Dword DRV_Dib9090_acquireChannel(Device_Context *DC, Word bandwidth, Dword frequency, Byte byChipType);
extern Dword DRV_Dib9090_getStatistic(Device_Context *DC, GetStatisticRequest *getstatisticrequest, Byte byChipType, Bool bIsRescan);
extern Dword DRV_Dib9090_getChannelStatistic (Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int ts_idx);
extern Dword DRV_Dib9090_isLocked(Device_Context *DC, Bool *locked, Byte byChipType);
extern Dword DRV_Dib9090_DeInitialize(Device_Context *DC, Byte byChipType);
extern Dword DRV_Dib9090_getDiversityDetailData(Device_Context *DC, DibcomDiversityData* data, Byte byChipType);
extern Dword DRV_ResetRxPort(Device_Context *DC, int is_open);
extern Dword DRV_MXL69X_Initialize(Device_Context *DC, Byte br_idx, Byte tsSrcIdx, Byte ChipType);
extern Dword DRV_MXL69X_acquireChannel(Device_Context *DC, AcquireChannelRequest *request, int br_idx, int tsSrcIdx);
extern Dword DRV_MXL69X_getStatistic(Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int tsSrcIdx);
extern Dword DRV_MXL69X_getChannelStatistic (Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int tsSrcIdx);
extern Dword DRV_MXL69X_isLocked(Device_Context *DC, Bool *locked, int br_idx, int tsSrcIdx);
extern Dword DRV_MXL69X_DeInitialize(Device_Context *DC, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD285X_Initialize(Device_Context *DC, int br_idx, int tsSrcIdx, Byte ChipType);
extern Dword DRV_CXD285X_acquireChannel(Device_Context *DC, AcquireChannelRequest *request, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD285X_getStatistic(Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD285X_isLocked(Device_Context *DC, Bool *locked, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD6801_initialize(Device_Context *DC, int br_idx, int tsSrcIdx, Byte ChipType);
extern Dword DRV_CXD6801_acquireChannel(Device_Context *DC, AcquireChannelRequest *request, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD6801_isLocked(Device_Context *DC, Bool *locked, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD6801_getStatistic(Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD6801_getChannelStatistic(Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD6801_readRegister(Device_Context *DC, Processor processor, Byte option , Dword registerAddress, Byte bufferLength, Byte* buffer, int br_idx, int ts_idx);
extern Dword DRV_CXD6801_writeRegister(Device_Context *DC, Processor processor, Byte option ,Dword registerAddress, Byte bufferLength, Byte* buffer, int br_idx, int ts_idx);
extern Dword DRV_MXL541C_Initialize(Device_Context *DC, int br_idx, int tsSrcIdx, Byte ChipType);
extern Dword DRV_MXL541C_DeInitialize(Device_Context *DC, int br_idx, int tsSrcIdx);
extern Dword DRV_MXL541C_acquireChannel(Device_Context *DC, AcquireChannelRequest *request, int br_idx, int tsSrcIdx);
extern Dword DRV_MXL541C_getStatistic(Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int tsSrcIdx);
extern Dword DRV_MXL541C_getChannelStatistic(Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int ts_idx);
extern Dword DRV_MXL541C_isLocked(Device_Context *DC, Bool *locked, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD2880_initialize(Device_Context *DC, int br_idx, int tsSrcIdx, Byte ChipType);
extern Dword DRV_CXD2880_acquireChannel(Device_Context *DC, AcquireChannelRequest *request, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD2880_isLocked(Device_Context *DC, Bool *locked, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD2880_getStatistic(Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD2880_getChannelStatistic(Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int tsSrcIdx);
extern Dword DRV_CXD2880_getMPLPID(Device_Context *DC, MPLPData *pMplpdata, int br_idx, int ts_idx);
extern Dword DRV_CXD2880_setPLPID(Device_Context *DC, MPLPData *pMplpdata, int br_idx, int ts_idx);

//jjustman-2022-08-11
extern Dword DRV_CXD6801_setPlpConfig(Device_Context *DC, MPLPData *pMplpdata, int br_idx, int tsSrcIdx);




#if defined (__cplusplus)
}
#endif


#endif
