/*
	at3drv_api.h

	for Project Atlas

	Main AT3DRV driver API.

	Digital STREAM Labs, Inc. 2017
	Copyright © 2017, 2018, 2019 LowaSIS, Inc.
*/

#ifndef __AT3DRV_API_H__
#define __AT3DRV_API_H__

//============================================================================

#include "at3drv_common.h"


#ifdef __cplusplus
extern "C" {
#endif

//============================================================================
// configs



//============================================================================
// typedef

/*
	
*/
typedef void *AT3_DEVICE;


/*
	
*/
typedef void *AT3_OPTION;


/*	demodulation type

	NOTE! each enum value should be same as tDHL_Demod values.
*/
typedef enum 
{
	eAT3_DEMOD_8VSB = 0,
	eAT3_DEMOD_64QAM,
	eAT3_DEMOD_256QAM,
	eAT3_DEMOD_ATSC30,

	eAT3_DEMOD_MAX,

} E_AT3_DEMOD;


typedef enum
{
	eAT3_FEVENDOR_Unknown = 0,
	eAT3_FEVENDOR_LGDT3307_R850,
	eAT3_FEVENDOR_CXD2878_ASCOT3,

} E_AT3_FEVENDOR;


typedef struct
{
	E_AT3_FEVENDOR vendor;
	uint32_t hwrev;  // hw (silicon) revision. 0 in case of no info.
	char fwrev[16];  // firmware revision. empty string in case of no info.

} S_AT3_FE_INFO;



/*	FE Status
*/
typedef enum
{
	eAT3_FESTAT_LOCK = 0,
		// type of pValue: int *
		// return signal lock state. 1 if locked, 0 is unlocked.

	eAT3_FESTAT_SNR,
		// this is for test. do not use it if you don't know what it is.
	
	eAT3_FESTAT_BER,
		// not implemented
	eAT3_FESTAT_MER,
		// not implemented
	eAT3_FESTAT_STRENGTH,
		// type of pValue: int *
		// it returns rssi value as unit of dBm x 1000.

	eAT3_FESTAT_PLP,
		// type of pValue: uint64_t [2]
		// value[0] is full plp id bitmap (64 bit, LSB bit[0] is for plp 0)
		// value[1] is lls plp id bitmap (64 bit, LSB bit[0] is for plp 0)

	eAT3_FESTAT_LGDEMOD_V1,
		// type of pValue: S_LGDEMOD_L2_SIG_STATUS *
	eAT3_FESTAT_LGD_SIGSTAT_V1 = eAT3_FESTAT_LGDEMOD_V1,

	eAT3_FESTAT_LOCK2,
		// type of pValue: S_FE_LOCK *
		// return detail signal lock status.

	eAT3_FESTAT_LGD_PLP_V1,
		// type of pValue: S_LGD_L2_PLPINFO *
		// caller should fill S_LGD_L2_PLPINFO::index before calling API.
		//
		// note that S_LGD_L2_PLPINFO is subset of LX_DEMOD_L2_ATSC3_PLP_INFO_T.

	eAT3_FESTAT_ALP_STD,
		// currently detected alp standard.
		// this value is valid only when alp-auto mode is used (user does not fix specific standard).
		// type of pValue: E_AT3_ALP_STD * (eAlpStd_Korea or eAlpStd_Usa)
		//
		// note that alp auto detect mode may not be supported in some fe vendor.

	eAT3_FESTAT_RF_DETAIL,
		// type of pValue: S_FE_DETAIL *

	eAT3_FESTAT_VENDOR,
		// type of pValue: E_AT3_FEVENDOR *

	eAT3_FESTAT_HW_INFO,
		// type of pValue: S_AT3_FE_INFO *

	eAT3_FESTAT_L1BASIC,
		// type of pValue: S_AT3_PHY_L1PREAMBLE **
		// note that only l1basic part of *pValue will be filled.
		// application can use AT3_L1PreambleString() for string representation.
		// application should call S_AT3_PHY_L1PREAMBLE::free() after use.

	eAT3_FESTAT_L1PREAMBLE,
		// type of pValue: S_AT3_PHY_L1PREAMBLE **
		// application can use AT3_L1PreambleString() for string representation.
		// application should call S_AT3_PHY_L1PREAMBLE::free() after use.

	eAT3_FESTAT_BOOTSTRAP_PARAM,
		// type of pValue: S_AT3_PHY_BSP *
		// caller should provide this structure memory. 

	eAT3_FESTAT_MAX,

} E_AT3_FESTAT;

typedef E_AT3_FESTAT E_AT3_RFSTAT; // for compatibility with old API

#define eAT3_RFSTAT_LOCK           eAT3_FESTAT_LOCK
#define eAT3_RFSTAT_STRENGTH       eAT3_FESTAT_STRENGTH
#define eAT3_RFSTAT_PLP            eAT3_FESTAT_PLP
#define eAT3_RFSTAT_LGDEMOD_V1     eAT3_FESTAT_LGDEMOD_V1
#define eAT3_RFSTAT_LGD_SIGSTAT_V1 eAT3_FESTAT_LGDEMOD_V1
#define eAT3_RFSTAT_LGD_PLP_V1     eAT3_FESTAT_LGD_PLP_V1
#define eAT3_RFSTAT_MAX            eAT3_FESTAT_MAX


/*
	RXDTYPE: Received Data Type
	type of data that is received from device.
*/
typedef enum 
{
	// Media data
	eAT3_RXDTYPE_BBPCTR = 0, // bbp container (proprietary)
		// info: S_AT3DRV_RXDINFO_BBPCTR

	eAT3_RXDTYPE_BBP, // bbp packet
		// info: S_AT3DRV_RXDINFO_BBP

	eAT3_RXDTYPE_ALP, // alp packet
		// entire alp packet (header + payload) is returned.
		// info: S_AT3DRV_RXDINFO_ALP

	eAT3_RXDTYPE_TS_RAW = 0x10, // not supported yet
		// Raw MPEG2-TS data from device, which may not be frame synchronized.

	eAT3_RXDTYPE_TS,
		// valid TS packet data which are frame synchronized. always multiple of 188.
		// info: S_AT3DRV_RXDINFO_TS

	eAT3_RXDTYPE_IP = 0x20, // ip packet
		// entire ip packet (header + payload) is returned.
		// info: S_AT3DRV_RXDINFO_IP

	eAT3_RXDTYPE_IP_LMT, // not supported yet
		// unparsed lmt.
		// info: S_AT3DRV_RXDINFO_LMT


	// Control data
	// todo

} E_AT3DRV_RXDTYPE;


/*
	E_AT3_RESET_TYPE: various type of reset
*/
typedef enum 
{
	eAT3_RESET_TYPE_TUNER,
	eAT3_RESET_TYPE_DEMOD,
	eAT3_RESET_TYPE_DEVICE,

} E_AT3_RESET_TYPE;


/*	detail info of data type eAT3_RXDTYPE_BBPCTR.
*/
typedef struct
{
	int bDiscontinuity;
		// non-zero if it is not continuation of previous data.
		// it means there is some dropped data just before this packet.

	uint32_t ulTick;
		// system tick when parser start to process this packet (measured using AT3_GetMsTime()).

} S_AT3DRV_RXDINFO_BBPCTR;


/*	detail info of data type eAT3_RXDTYPE_BBP.
*/
typedef struct
{
	int bDiscontinuity;
		// non-zero if it is not continuation of previous data.
		// it means there is some dropped data just before this packet.

	uint32_t ulTick;
		// system tick when parser start to process this packet (measured using AT3_GetMsTime()).

	uint8_t plp_id;  // plp id. 0 ~ 63
	int     length;  // byte size of bbp. same as S_RX_DATA::nLength
	uint8_t counter; // continuity counter. 0 ~ 255

	// detail info
	E_L1d_PlpFecType fec_type;
	E_L1d_PlpCod cod;
	S_AT3_L1TIME l1time;

} S_AT3DRV_RXDINFO_BBP;


/*	detail info of data type eAT3_RXDTYPE_ALP.
		ptr : point to ALP packet header start.
		nLength : byte size of entire ALP packet (ALP header + payload)
*/
typedef struct
{
	// lower-layer info
	uint8_t plp_id;	

	// alp header info
	uint8_t packet_type; // 0: IPv4, ...
	uint16_t header_len;

	// l1d time info
	S_AT3_L1TIME l1time;

	int b_discon;  // non-zero if there is data loss just before this packet.

} S_AT3DRV_RXDINFO_ALP;

/*	detail info of data type eAT3_RXDTYPE_IP.
		ptr : point to IP packet header start.
		nLength : byte size of entire IP packet (IP header + payload)
*/
typedef struct
{
	// lower-layer info
	uint8_t plp_id;

	// l1d time info
	S_AT3_L1TIME l1time;

	int b_discon;  // non-zero if there is data loss just before this packet.

} S_AT3DRV_RXDINFO_IP;

typedef struct
{
	uint8_t plp_id; // plp from which this lmt data is received.

	uint8_t  lmt_ver;

	// more infos
	// uint8_t  lmt_format;   // 0:Binary, 1:XML, 2:JSON. it should be 0.
	// uint8_t  lmt_encoding; // 2bits, 0:Plain, 1:Deflate. it should be 0.

	S_AT3_LMT *lmt;  // parsed lmt
		// this is pointer to parsed lmt table. 
		// it is only valid when related drv option is enabled.
		// 
		// it is valid inside of (*AT3DRV_DATA_RX_CB)() callback.
		// application should not access this pointer once callback is returned.
		// if app want to keep this value beyond callback, it should do deep-copy 
		// of this structure.

} S_AT3DRV_RXDINFO_LMT;

/*	detail info of data type eAT3_RXDTYPE_TS.
*/
typedef struct
{
	int nNumPkts;
		// number of packets in this buffer. it should be nLength/188

	int bDiscontinuity;
		// non-zero if it is not continuation of previous data.
		// it means there is some dropped data before this data.
	
	// uint64_t ullNumTotalPkt;
		// accumulated total number of packets since FE_Start.

} S_AT3DRV_RXDINFO_TS;


/*

	pInfo: type specific data.
*/
typedef struct 
{
	E_AT3DRV_RXDTYPE eType;
	uint8_t *ptr; // buffer pointer
	int nLength;  // actual data length
	void *pInfo;  // additional info. type depends on eType.

} S_RX_DATA;





/*

*/
typedef struct
{
	int32_t snr;
	int32_t offsetFrq_Hz;
	int32_t sam_offsetFrq_Hz;
	int16_t if_agc;
	uint32_t mse_info;
	uint32_t constPwr;
	uint32_t ber;
	uint32_t packetError;
	uint32_t sym_rate;
	uint8_t signalQuality;
	uint8_t operatingMode;
	uint8_t constellation;
	uint8_t spInv;
	uint8_t bandwidth;

	// lock info
	uint8_t demodLock, lock_l1b, lock_l1d;

	uint8_t guardInterval; // not for VSB
	uint8_t codeRate, fftMode; // not for VSB

	uint8_t num_PLP;

	int32_t demod_snr; // for debugging
	uint32_t mse_info_demod; // for debugging

	// uint32_t plp_pre_bch_ber; // for debugging
	// uint16_t ldpc_err; // for debugging
	// uint16_t plp_outer_err;
	// uint8_t plp_ldpc_iter, l1b_ldpc_iter, l1d_ldpc_iter; // for debugging

	uint8_t frame_length;

} S_LGDEMOD_L2_SIG_STATUS;


typedef struct
{
	uint8_t  index;   // [in] index of PLP to query this info.

	// all below fields are output.
	uint8_t  l1b_num_subframes; // L1-basic number of sub-frame
	uint8_t  l1b_pre_num_symbols; // L1-basic preamble N-symbol
	uint16_t l1b_num_ofdm_symbols; // L1-basic OFDM N-symbol
	uint8_t  l1b_sub_fft_size; // L1-basic 1st sub-frame FFT size
		// 0 = 8K, 1 = 16K, 2 = 32K, 3 = reserved
	uint8_t  l1b_sub_gi; // L1-basic 1st sub-frame guard interval
		// 0=reserved, 1=1/192, 2=2/384, 3=3/512, 4=4/768, 
		// 5=5/1024, 6=6/1536, 7=7/2048, 8=8/2432, 
		// 9=9/3072, 10=10/3648, 11=11/4096, 12=12/4864
	uint8_t  l1b_reduce_carr; // L1-basic 1st sub-frame reduced carrier
	uint8_t  l1b_sub_scatt_pp; // L1-basic 1st sub-frame scattered pilot mode
	uint8_t  l1b_sub_scatt_pb; // L1-basic 1st sub-frame scattered boosting mode
	uint8_t  l1b_sub_sbs_first; // L1-basic 1st sub-frame SBS first
		// 0 = Not boundary symbol, 1 = Boundary symbol
	uint8_t  l1b_sub_sbs_last; // L1-basic 1st sub-frame SBS last
		// 0 = Not boundary symbol, 1 = Boundary symbol
	uint8_t  num_plp; // L1-detail number of PLPs
	uint8_t  plp_ti_mode; // L1-detail PLP TI mode
		// 0 = No time interleaving, 1 = CTI mode, 2 = HTI mode
	uint8_t  plp_fec_type; // L1-detail PLP FEC type
		// 0 = BCH + 16K LDPC, 1 = BCH + 64K LDPC, 
		// 2 = CRC + 16K LDPC, 3 = CRC + 64K LDPC,
		// 4 = 16K LDPC only, 5 = 64K LDPC only
	uint8_t  plp_mod; // L1-detail PLP modulation
	uint8_t  plp_cr;  // L1-detail PLP code rate
		// 0=2/15, 1=3/15, 2=4/15, 3=5/15, 4=6/15, 5=7/15, 
		// 6=8/15, 7=9/15, 8=10/15, 9=11/15, 10=12/15, 11=13/15
	uint8_t  l1d_plp_cti_depth; // L1-detail PLP CTI depth
	uint16_t l1d_plp_cti_start_row; // L1-detail PLP CTI start row
	uint8_t  l1d_plp_hti_inter_subframe; // L1-detail HTI inter sub-frame
	uint8_t  l1d_plp_hti_num_ti_blocks; //L1-detail PLP HTI number of TI blocks
	uint16_t l1d_plp_hti_num_fec_blocks; // L1-detail PLP HTI number of FEC blocks

	uint8_t  l1d_miso; // L1-detail MISO options
		// 0 = No MISO, 1 = MISO with 64 coefficients
		// 2 = MISO with 256 coefficients, 3 = reserved
	uint8_t  l1d_mimo; // L1-detail MIMO
		// 0 = MIMO not used, 1 = MIMO used
	uint16_t l1d_plp_hti_num_fec_blocks_max; // L1-detail PLP HTI number of FEC blocks max

	uint8_t  l1d_plp_id; // L1-detail PLP ID
	uint8_t  l1d_slt_flag; // L1-detail SLT flag

	uint8_t  easinfo; // ??

	// below is undocumented! comments may not be correct!
	uint8_t  l1d_plp_type;  // 0 for non-dispersed (subslicing not used), 1 for dispersed.
	uint8_t  plpn_layer;    // LDM: 0 for core, 1 for enhanced.

} S_LGD_L2_PLPINFO;
// note that S_LGD_L2_PLPINFO is subset of LX_DEMOD_L2_ATSC3_PLP_INFO_T.


typedef struct
{
    uint8_t bDemodLock; // true if demod is locked, false if state unknown.

    uint8_t bNoSignal;  // true if driver decide that there is no signal.

    uint8_t bPlpLockAny, bPlpLockAll;
    	// true if any(all) of selected plp is locked.
    uint8_t bmPlpLock;
    	// each bit represents plp index that user requested using AT3DRV_FE_SetPLP().
    	//   0x1: first plp, 0x2: second plp, 0x4: 3rd plp, 0x8: 4th plp.
    	// note! actual requested plp ids may be modifed inside AT3DRV_FE_SetPLP().
    	// see comments of AT3DRV_FE_SetPLP api.

} S_FE_LOCK;

typedef struct
{
	uint32_t flagRequest;
		// <input> bit mask of FE_SIG element to be queried.
		// user set this by ORing below flags of field option.
	uint32_t flagReturned;
		// <output> bit mask of FE_SIG element to return.
		// driver set this flag if corresponding fields has valid data.
		// this may not be same as flagRequest because of some reason
		// - some data is not supported by chip/driver.
		// - some data is aquired regardless of user's request.

	uint32_t flagSupported;
		// <output>.
		// driver always set this flag regardless of flagRequest.

	//---------------------
	// lock status
	#define FE_SIG_MASK_SimpleLock (1UL << 0)
		uint8_t bLock;   // 0 or 1

	#define FE_SIG_MASK_Lock (1UL << 1)
		S_FE_LOCK lock;

	// plp error
	#define FE_SIG_MASK_PlpErr (1UL << 4)
	    uint8_t bPlpErr;  // true if some of user-set plp has error (not found, etc..)

	// user-set plp
	#define FE_SIG_MASK_UserPlp (1UL << 5)
		uint8_t idPlps[4]; // plp id that user sets. 0x40 if not set.

	// full plp info (from L1 signaling)
	#define FE_SIG_MASK_PlpList (1UL << 6)
		uint8_t numPlps;
		uint64_t bmPlpsExist, bmPlpsLls, bmPlpsLayer1, bmPlpsChbond;
			// bitmap of total plp list. each bit represents plp

	// rf level estimation
	#define FE_SIG_MASK_RfLevel (1UL << 8)
		int32_t nRfLevel1000;  // RF Level in dB * 1000

	// carrier offset
	#define FE_SIG_MASK_CarrierOffset (1UL << 9)
		int32_t nCarrierOffset; // in Hz

	// if agc
	#define FE_SIG_MASK_IfAgc (1UL << 10)
		uint32_t uIfAgcOut;  // 0 ~ 0xfff

	// spectrun inversion
	#define FE_SIG_MASK_SpectrumInv (1UL << 11)
		uint8_t bSpectrumInv;  // 0 if normal, 1 if inverted

	// estimated snr
	#define FE_SIG_MASK_SNR (1UL << 12)
		int32_t nSnr1000;  // snr (dB) x 1000

	// ber
	#define FE_SIG_MASK_BER (1UL << 16)
		uint32_t aBerPreLdpcE7[4];   // return BER x 1e7. (uint32_t)-1 if invalid.
		uint32_t aBerPreBchE9[4];    // return BER x 1e9. (uint32_t)-1 if invalid.
		uint32_t aFerPostBchE6[4];   // return FER x 1e6. (uint32_t)-1 if invalid.

	// sampling freq offset
	#define FE_SIG_MASK_SamplingOffset (1UL << 17)
		int32_t nSamplingFreqOffsetPpm;  // +/-220ppm

	// fec mod/cod value
	#define FE_SIG_MASK_FecModCod (1UL << 18)
		struct {
			uint8_t valid;
			E_L1d_PlpFecType fecType;
			E_L1d_PlpMod mod;
			E_L1d_PlpCod cod;
		} aFecModCod[4];

	// bbp err count
	#define FE_SIG_MASK_BbpErr (1UL << 19)
		uint32_t aErrCntBbp1Sec[4];  // number of bbp error during last 1 sec.
	
	// bootstrap info
	#define FE_SIG_MASK_Bootstrap (1UL << 24)
		struct {
			uint8_t systemBw; // 0:6MHz, 1:7MHz, 2:8MHz, 3:>8MHz
			uint8_t eaWakeUp; // 0 ~ 3. ea_wake_up_1 is lsb, ea_wake_up_2 is msb.
		} sBootstrap;

	// ...

} S_FE_DETAIL;


/*
	fuction type used for callback.

*/
typedef AT3RESULT (*AT3DRV_L1D_TIME_INFO_CB) (uint32_t sec, uint16_t msec,
                                             uint16_t usec, uint16_t nsec,
                                             uint64_t ullUser);


/*	AT3DRV_DATA_RX_CB

	callback function should return 'success' for driver to continue working.
	AT3OK(return value) should be true.
	if callback returns any kinds of error (AT3FAIL(r)), AT3DRV_HandleRxData
	will also returns error.

	usually, they (callback function) returns AT3RES_OK.
	however, sometimes AT3RES_OK_BUSY is usefull when caller want to keep 
	their buffer as small as possible.
	if callback returns AT3RES_OK_BUSY, driver calls callback as minimum 
	as possible, then AT3DRV_HandleRxData returns more quickly.
	application can process rxdata and call AT3DRV_HandleRxData again later,
	without any data loss.
*/
typedef AT3RESULT (*AT3DRV_DATA_RX_CB) (S_RX_DATA *pData, uint64_t ullUser);



//-------------------------------------
// device enum

/*
	Device key info is used to distinguish each physical device.
	This key value is guaranteed to be unique in current system
	even though same-vid-pid devices are present.

	This type is used in return value of LDR find/search APIs.
	you can use special key when needed.
*/
typedef uint64_t AT3_DEV_KEY;

#define cDEV_KEY_NULL 0      // invalid key
#define cDEV_KEY_ANY  0xff   // special meaning: any device


typedef enum
{
	eDT_CypressFx2 = 0,
	eDT_CypressFx3,
	eDT_AtlasFx2,
	eDT_AtlasFx3,
	eDT_Unknown,
	// eDT_MAX,

} E_AT3_DEV_TYPE;


#ifdef __AT3DRV_DEFINE_NAME_ARRARY // used drv internally
	// warning! order should be matched to enum values.
	const char *g_DevTypeStringArray[] = { "cyfx2", "cyfx3", "atfx2", "atfx3", "?", };
#else
	extern const char *g_DevTypeStringArray[];
#endif
#define AT3DRV_DevTypeString(e) g_DevTypeStringArray[(int)(e)]



typedef enum {
	eDFF_CypressFx2 = 0x1, // == (1<<eDT_CypressFx2)
	eDFF_CypressFx3 = 0x2,
	eDFF_AtlasFx2 = 0x4,
	eDFF_AtlasFx3 = 0x8,

	eDFF_CypressAny = 0x3,
	eDFF_AtlasAny = 0xc,
	eDFF_AnyFx3 = 0xa,
	eDFF_AnyFx2 = 0x5,

	eDFF_Any = 0xf,

	eDFF_Everything = 0x10000,

} E_AT3_DEV_FIND_FILTER;

// filter for single device type only
#define AT3_MK_DF_FILTER(eDevType) \
	( (E_AT3_DEV_FIND_FILTER) (1 << (int)(eDevType)) )


/*
	version info of at3drv driver library.
*/
typedef struct {
	uint8_t major, minor, patch;
	const char *ver;
	const char *rev_id;
	const char *rev_branch;

} S_AT3DRV_VER_INFO;




/*
	it is used for AT3DRV_LDR_PolulateUsbDevice(s) apis, where user can add
	newly found usb devices into device database managed by at3drv driver.

	ETA means 'Enumeration thru Android'.

	this is for android only. 
	for other os (windows, linux), at3drv can enumerate devices by himself,
	so no need to get support from application.

	for more details, refer dev guide document.
*/
typedef struct
{
	const char *devfs;  // device file path. ex: /dev/bus/usb/xxx/yyy
	int fd;             // device file descriptor

} S_ETA_DEVICE;



//============================================================================
// Init/Open API

/*
	this api should be called before any other api call.

	do not call this Init() api at the global constructor.

	return
		BAD_PARAM: version mismatch
		API_LOCK: if api is locked
*/
AT3DRVAPI AT3RESULT 
	AT3DRV_Init(uint32_t uVersion);


AT3DRVAPI AT3RESULT
	AT3DRV_Uninit(void);


/*
	Open AT3 device and return its handle.

	pDevOut [out]: opened device handle
	hDevKey: which device to open
		if cDEV_KEY_IGNORE is used, then any proper device will be opened.
	hOpt: handle option used for this device creation.
*/
AT3DRVAPI AT3RESULT 
	AT3DRV_OpenDevice(AT3_DEVICE *phDevOut, AT3_DEV_KEY hDevKey, AT3_OPTION hOpt);

/*
	Close AT3 device and free all related memory and resources.

	Warning!
		At the time of this api call, there should not be ANT pending 
		AT3DRV_WaitRxData() call in other thread.
*/
AT3DRVAPI AT3RESULT 
	AT3DRV_CloseDevice(AT3_DEVICE hDev);


//============================================================================
// FE control/query API

/*
	
	Note:
		if the device is already in FE_Start state, driver will do FE_Stop
		first and do FE_Start with new parameter.
		However, it is recommended call FE_Stop before do another FE_Start.
*/
AT3DRVAPI AT3RESULT 
	AT3DRV_FE_Start(AT3_DEVICE hDev, int nFreqKHz, E_AT3_DEMOD eDemod, uint8_t ucPlpId);


/*

*/
AT3DRVAPI AT3RESULT 
	AT3DRV_FE_Stop(AT3_DEVICE hDev);

/*
	plp id 0x40 means unused.

	Note:
		invalid pid and duplicated pid will be skipped.
		so, the order of plp ids may be changed internally.
	ex:
		user specifies:
			uint8_t plps[4] = { 0, 0x40, 2, 2 };
			AT3DRV_FE_SetPLP(dev, plps, 4);

		modified plpid array => { 0, 2, 0x40, 0x40 }, num plp = 2.
*/
AT3DRVAPI AT3RESULT 
	AT3DRV_FE_SetPLP(AT3_DEVICE hDev, uint8_t aPlpIds[], int nPlp);

/*

*/
AT3DRVAPI AT3RESULT 
	AT3DRV_FE_GetStatus(AT3_DEVICE hDev, E_AT3_FESTAT nStatType, void *pValue);

/*

*/
AT3DRVAPI AT3RESULT 
	AT3DRV_FE_Control(AT3_DEVICE hDev, const char *pMsg, void *pData);

/*

*/
AT3DRVAPI AT3RESULT
	AT3DRV_RegisterL1DTimeInfoCallback(AT3_DEVICE hDev, AT3DRV_L1D_TIME_INFO_CB fnCallback,
	                                   uint64_t ullUser);

//============================================================================
// Media data Get API

/*
	
		
*/
AT3DRVAPI AT3RESULT 
	AT3DRV_WaitRxData(AT3_DEVICE hDev, int nTimeoutMs);

/*

*/
AT3DRVAPI AT3RESULT 
	AT3DRV_HandleRxData(AT3_DEVICE hDev, AT3DRV_DATA_RX_CB fnCallback, uint64_t ullUser);

/*
	
	
*/
AT3DRVAPI AT3RESULT
	AT3DRV_CancelWait(AT3_DEVICE hDev);


//============================================================================
// Device control API

/*

*/
AT3DRVAPI AT3RESULT 
	AT3DRV_ResetDevice(AT3_DEVICE hDev, E_AT3_RESET_TYPE eResetType);


//============================================================================
// Option API


AT3DRVAPI AT3RESULT
	AT3DRV_Option_Create(AT3_OPTION *pHandle);

AT3DRVAPI AT3RESULT 
	AT3DRV_Option_SetString(AT3_OPTION handle, const char *sOptionName, const char *sOptionValue);
AT3DRVAPI AT3RESULT 
	AT3DRV_Option_SetInt(AT3_OPTION handle, const char *sOptionName, int nOptionValue);

AT3DRVAPI AT3RESULT
	AT3DRV_Option_Release(AT3_OPTION handle);


//============================================================================
// Loader API

/*

*/

AT3DRVAPI AT3RESULT
	AT3DRV_LDR_FindDeviceByType(E_AT3_DEV_TYPE eDevType, AT3_DEV_KEY *pahDevKeys, int nMaxKey, int *pnNumKey);
AT3DRVAPI AT3RESULT 
	AT3DRV_LDR_SearchDevicesByType(E_AT3_DEV_FIND_FILTER eFilter, AT3_DEV_KEY *pahDevKeys, int nMaxKey, int *pnNumKey);



/*

*/
AT3DRVAPI AT3RESULT 
	AT3DRV_LDR_LoadFirmware(AT3_DEV_KEY hKey);

AT3DRVAPI AT3RESULT 
	AT3DRV_LDR_LoadFirmwareEx(AT3_DEV_KEY hKey, const char *sVersion);


/*

*/
AT3DRVAPI AT3RESULT 
	AT3DRV_LDR_CheckDeviceExist(AT3_DEV_KEY hKey, E_AT3_DEV_TYPE *peDevType);


/*

*/
AT3DRVAPI AT3RESULT
	AT3DRV_LDR_PolulateUsbDevices(const S_ETA_DEVICE *etaDevs, int nNumDevs, int *pnNumAdded);

AT3DRVAPI AT3RESULT
	AT3DRV_LDR_PolulateUsbDevice(const char *devfs, int fd);


//============================================================================
// version info

/*

*/
AT3DRVAPI AT3RESULT
	AT3DRV_GetVersionInfo(S_AT3DRV_VER_INFO *pVerInfo);


//============================================================================
// debug API

// below apis are all for debug purposes.
// note that it is not thread safe.

AT3DRVAPI FILE *AT3UTL_GetDbgLogFp(void);
AT3DRVAPI void AT3UTL_SetDbgLogFp(FILE *fp);
AT3DRVAPI void AT3UTL_ListModules(void);
AT3DRVAPI void AT3UTL_SetModuleDbgLevel(const char *name, int level);
AT3DRVAPI void AT3UTL_SetDbgLevelParams(const char *pParamList);
AT3DRVAPI void AT3UTL_SetAllDbgLevel(int level);
AT3DRVAPI void AT3UTL_ControlGlobalDbgLevel(int bEnable, int level);


//============================================================================
// utility API

// return system tick count (unit: ms)
AT3DRVAPI uint32_t AT3_GetMsTime(void);

// wait system delay (unit: ms)
AT3DRVAPI void AT3_DelayMs(uint32_t n);

#ifdef __cplusplus
};
#endif


#endif // __AT3DRV_API_H__

