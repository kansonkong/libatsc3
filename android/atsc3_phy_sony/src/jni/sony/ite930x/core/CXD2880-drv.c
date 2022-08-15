#include "it930x-core.h"
#include "sony_common.h"
#include "sony_devio_i2c.h"
#include "sony_integ_dvbt.h"
#include "sony_integ_dvbt2.h"
#include "sony_integ_dvbt_t2.h"
#include "sony_integ_isdbt.h"
#include "sony_tunerdemod_monitor.h"
#include "sony_tunerdemod_dvbt_monitor.h"
#include "sony_tunerdemod_dvbt2_monitor.h"
#include "sony_tunerdemod_isdbt_monitor.h"
#include "drvi2c_ite.h"
#include "sony_tunermodule.h"

#define CXD2880_DVBT_RF_POWER_MAX_BOUNDARY -23
#define CXD2880_DVBT_RF_POWER_MIN_BOUNDARY -86
#define CXD2880_DVBT2_RF_POWER_MAX_BOUNDARY -34
#define CXD2880_DVBT2_RF_POWER_MIN_BOUNDARY -84
#define CXD2880_ISDBT_RF_POWER_MAX_BOUNDARY -23
#define CXD2880_ISDBT_RF_POWER_MIN_BOUNDARY -89
#define CXD2880_DVBT_SNR_MAX_BOUNDARY 40000
#define CXD2880_DVBT_SNR_MIN_BOUNDARY 18800
#define CXD2880_DVBT2_SNR_MAX_BOUNDARY 40000
#define CXD2880_DVBT2_SNR_MIN_BOUNDARY 16200
#define CXD2880_ISDBT_SNR_MAX_BOUNDARY 28612
#define CXD2880_ISDBT_SNR_MIN_BOUNDARY 16369

#define CXD2880_DEBUG 1

#if CXD2880_DEBUG 
	#define DRV_CXD2880_DEBUG(args...)   printk(args)
#else
	#define DRV_CXD2880_DEBUG(args...)
#endif

typedef struct
{
	sony_tunerdemod_t       tunerDemod;
	sony_regio_t            regio;
	sony_i2c_t              i2c;
	drvi2c_ite_t			iteI2c;
} sony_driver_instance_t;

Byte cxd2880_lock_mode = 0;
ChannelStatistic cxd2880_channelStatistic = {0,0,0,0,0,0,0};
sony_driver_instance_t 		ew100_driver;
sony_dvbt2_ofdm_t 			ofdmInfo;
sony_dvbt2_profile_t 		profileFound;
Byte CXD2880_ActivePLPID = 0; //default

Dword DRV_CXD2880_initialize(Device_Context *DC, int br_idx, int tsSrcIdx, Byte ChipType)
{
	sony_result_t					ew100_ret = SONY_RESULT_OK;
	uint8_t							ew100_i2cAddressDemod = 0;
	sony_tunerdemod_create_param_t	ew100_createParam;				
	Byte							sony_chip = 0;
	sony_tunerdemod_pid_filter_config_t ew100_pPIDFilterConfig;
	Byte i2cBus = 0;
	int i = 0;
	Endeavour *it9300 = (Endeavour*)&DC->it9300;
	
	ew100_i2cAddressDemod = it9300->tsSource[br_idx][tsSrcIdx].i2cAddr;
	i2cBus = it9300->tsSource[br_idx][tsSrcIdx].i2cBus;
	sony_chip = ChipType;// SONY_CHIP_EW100;

	//Driver Init & Create
	drvi2c_ite_Initialize(&ew100_driver.iteI2c, (Endeavour*)&DC->it9300, i2cBus);
	drvi2c_ite_CreateI2c(&ew100_driver.i2c, &ew100_driver.iteI2c);

	//Create
	ew100_ret = sony_regio_i2c_Create(&ew100_driver.regio, &ew100_driver.i2c, ew100_i2cAddressDemod);
	if (ew100_ret != SONY_RESULT_OK) {
		printk("Error : ew100_ret = %d\n", ew100_ret);
		goto exit;
	}
	
	ew100_createParam.tsOutputIF = SONY_TUNERDEMOD_TSOUT_IF_TS;
	ew100_createParam.enableInternalLDO = 1;
	ew100_createParam.xtalShareType = SONY_TUNERDEMOD_XTAL_SHARE_NONE;
	ew100_createParam.xosc_i = 0x08;
	ew100_createParam.isCXD2881GG = 0;
	if (sony_chip == EEPROM_EW100)
		ew100_createParam.xosc_cap = 0x08;
	else if (sony_chip == EEPROM_CXD2880_33)
		ew100_createParam.xosc_cap = 0x21;        
	else if (sony_chip == EEPROM_CXD2880)
		ew100_createParam.xosc_cap = 0x12;

	ew100_ret = sony_tunerdemod_Create(&ew100_driver.tunerDemod, &ew100_driver.regio, &ew100_createParam);
	if (ew100_ret != SONY_RESULT_OK) {
		goto exit;
	}

	if (sony_chip == EEPROM_EW100) {
		ew100_ret = sony_tunermodule_Create(&ew100_driver.tunerDemod);
		if (ew100_ret != SONY_RESULT_OK) {
			goto exit;
		}
	}

	//Initialize
	ew100_ret = sony_integ_Initialize(&ew100_driver.tunerDemod);
	if (ew100_ret != SONY_RESULT_OK) {
		goto exit;
	}

	ew100_ret = sony_tunerdemod_SetConfig(&ew100_driver.tunerDemod, SONY_TUNERDEMOD_CONFIG_TSCLK_FREQ, 0);//82.28MHz
	if (ew100_ret != SONY_RESULT_OK) {
		goto exit;
	}
	
	for (i = 0; i < 32; i++) {
		ew100_pPIDFilterConfig.pidConfig[i].pid = 0;
		ew100_pPIDFilterConfig.pidConfig[i].isEnable = 0;
	}
	
	printk("CXD2880 initialize chip succeeded.\n");
	
	return 0;
exit:
	printk("something wrong!!!!!!!!!\n");
	return 1;
}

Dword DRV_CXD2880_blindtune(Device_Context *DC, AcquireChannelRequest *request, Byte br_idx, Byte tsSrcIdx)
{	
	sony_result_t	ew100_ret = SONY_RESULT_OK;
	sony_dvbt_tune_param_t	ew100_tunerParam_DVBT;
	sony_dtv_system_t systemFound = SONY_DTV_SYSTEM_UNKNOWN;
	uint32_t frequencyKHz = 0;
	uint32_t bandwidth = 0;
	uint8_t numPlps = 0;
	uint8_t plpIds[255] = {0};
	int i = 0;
	sony_integ_dvbt_t2_scan_result_t scanResult;
	sony_integ_dvbt_t2_scan_param_t pScanParam;
	
	frequencyKHz = request->frequency;
	bandwidth = request->bandwidth;
	profileFound = SONY_DVBT2_PROFILE_ANY;
	
	DRV_CXD2880_DEBUG("DRV_CXD2880_blindtune DVB_T/T2\n");
	DRV_CXD2880_DEBUG("Freq_kHz : %d\n",frequencyKHz);
	DRV_CXD2880_DEBUG("Bandwidth : %d\n",bandwidth);

	ew100_tunerParam_DVBT.centerFreqKHz = frequencyKHz;
	if (bandwidth == 5000)
		ew100_tunerParam_DVBT.bandwidth = SONY_DTV_BW_5_MHZ;
	else if (bandwidth == 6000)
		ew100_tunerParam_DVBT.bandwidth = SONY_DTV_BW_6_MHZ;
	else if (bandwidth == 7000)
		ew100_tunerParam_DVBT.bandwidth = SONY_DTV_BW_7_MHZ;
	else
		ew100_tunerParam_DVBT.bandwidth = SONY_DTV_BW_8_MHZ;
		
	ew100_tunerParam_DVBT.profile = SONY_DVBT_PROFILE_HP;

	scanResult.tuneResult = sony_integ_dvbt_t2_BlindTune(&ew100_driver.tunerDemod, frequencyKHz, ew100_tunerParam_DVBT.bandwidth, SONY_DTV_SYSTEM_ANY, SONY_DVBT2_PROFILE_ANY, &systemFound, &profileFound);				
	if (ew100_ret != SONY_RESULT_OK) {
		printk("sony_integ_dvbt_t2_BlindTune Error : ew100_ret = %d\n", ew100_ret);
		goto exit;
	}
	
	if (systemFound != 0) {
		switch (systemFound) {
		case 1 :					
			DRV_CXD2880_DEBUG("SONY_DTV_SYSTEM_DVBT \n");
			break;
		case 2 :					
			DRV_CXD2880_DEBUG("SONY_DTV_SYSTEM_DVBT2 \n");
			ew100_ret = sony_tunerdemod_dvbt2_monitor_OFDM(&ew100_driver.tunerDemod, &ofdmInfo);
			if (ew100_ret != SONY_RESULT_OK) {
				printk("sony_tunerdemod_dvbt2_monitor_OFDM Error : ew100_ret = %d\n", ew100_ret);
				goto exit;
			}

			ew100_ret = sony_tunerdemod_dvbt2_monitor_DataPLPs(&ew100_driver.tunerDemod, plpIds, &numPlps);
			if (ew100_ret != SONY_RESULT_OK) {
				printk("sony_tunerdemod_dvbt2_monitor_DataPLPs Error : ew100_ret = %d\n", ew100_ret);
				goto exit;
			} else {
				DRV_CXD2880_DEBUG("numPlps = %d\n", numPlps);
				for (i = 0; i < numPlps; i++)
					DRV_CXD2880_DEBUG("plpIds[%d] = %d\n", i, plpIds[i]);							
			}
			DRV_CXD2880_DEBUG("BlindTune select first ID of PLP ID list automatically.\n");
			break;
		}
	}
	msleep(1000);
	
	if (systemFound == 2) 
		return TV_STANDARD_DVBT_T2;

	return !TV_STANDARD_DVBT_T2;
	
exit:
	printk("something wrong!!!!!!!!!\n");
	return 0xFF;
}

Dword DRV_CXD2880_acquireChannel(Device_Context *DC, AcquireChannelRequest *request, int br_idx, int tsSrcIdx)
{
	sony_result_t					ew100_ret = SONY_RESULT_OK;
	sony_dvbt_tune_param_t			ew100_tunerParam_DVBT;
	sony_dvbt2_tune_param_t			ew100_tunerParam_DVBT2;
	sony_isdbt_tune_param_t			ew100_tunerParam_ISDBT;
	sony_dvbt2_plp_t				ew100_plpInfo;
	uint32_t frequencyKHz = 0;
	uint32_t bandwidth = 0;
	
	frequencyKHz = request->frequency;
	bandwidth = request->bandwidth;
	cxd2880_lock_mode = request->mode;
	
	DRV_CXD2880_DEBUG("Freq_kHz : %d\n",frequencyKHz);
	DRV_CXD2880_DEBUG("Bandwidth : %d\n",bandwidth);
	DRV_CXD2880_DEBUG("request->mode : %x\n",cxd2880_lock_mode);
	
	if (cxd2880_lock_mode == TV_STANDARD_ISDBT) {
		DRV_CXD2880_DEBUG("DRV_CXD2880_acquireChannel ISDBT\n");
		if (frequencyKHz != 0) {
			ew100_tunerParam_ISDBT.centerFreqKHz = frequencyKHz;
			if (bandwidth == 5000)
				ew100_tunerParam_ISDBT.bandwidth = SONY_DTV_BW_5_MHZ;
			else if (bandwidth == 6000)
				ew100_tunerParam_ISDBT.bandwidth = SONY_DTV_BW_6_MHZ;
			else if (bandwidth == 7000)
				ew100_tunerParam_ISDBT.bandwidth = SONY_DTV_BW_7_MHZ;
			else if (bandwidth == 8000)
				ew100_tunerParam_ISDBT.bandwidth = SONY_DTV_BW_8_MHZ;
			else
				ew100_tunerParam_ISDBT.bandwidth = SONY_DTV_BW_1_7_MHZ;
			ew100_ret = sony_integ_isdbt_Tune(&ew100_driver.tunerDemod, &ew100_tunerParam_ISDBT);
			if (ew100_ret != SONY_RESULT_OK) {
				printk("ISDBT Error : ew100_ret = %d\n", ew100_ret);
				goto exit;
			}
			msleep(1000);
		}
	} else if (cxd2880_lock_mode == TV_STANDARD_DVBT_T2) {		

		cxd2880_lock_mode = DRV_CXD2880_blindtune(DC, request, br_idx, tsSrcIdx);
		
		if (cxd2880_lock_mode == 0xFF)
			goto exit;
		
		if (cxd2880_lock_mode == TV_STANDARD_DVBT_T2) {		
			DRV_CXD2880_DEBUG("DRV_CXD2880_acquireChannel DVB-T2\n");
			if (frequencyKHz != 0) {
				ew100_tunerParam_DVBT2.centerFreqKHz = frequencyKHz;
				if (bandwidth == 5000)
					ew100_tunerParam_DVBT2.bandwidth = SONY_DTV_BW_5_MHZ;
				else if (bandwidth == 6000)
					ew100_tunerParam_DVBT2.bandwidth = SONY_DTV_BW_6_MHZ;
				else if (bandwidth == 7000)
					ew100_tunerParam_DVBT2.bandwidth = SONY_DTV_BW_7_MHZ;
				else if (bandwidth == 8000)
					ew100_tunerParam_DVBT2.bandwidth = SONY_DTV_BW_8_MHZ;
				else
					ew100_tunerParam_DVBT2.bandwidth = SONY_DTV_BW_1_7_MHZ;
					
				CXD2880_ActivePLPID = ew100_tunerParam_DVBT2.dataPLPID;
				ew100_tunerParam_DVBT2.dataPLPID = 0;
				ew100_tunerParam_DVBT2.profile = SONY_DVBT2_PROFILE_BASE;
				ew100_tunerParam_DVBT2.tuneInfo = SONY_TUNERDEMOD_DVBT2_TUNE_INFO_OK;
				ew100_ret = sony_integ_dvbt2_Tune(&ew100_driver.tunerDemod, &ew100_tunerParam_DVBT2);
				if (ew100_ret != SONY_RESULT_OK && ew100_ret != SONY_RESULT_OK_CONFIRM) {
					printk("DVB-T2 Error : ew100_ret = %d\n", ew100_ret);
					goto exit;
				}
				//Confirm PLP ID
				if ((ew100_ret == SONY_RESULT_OK_CONFIRM) && (ew100_tunerParam_DVBT2.tuneInfo == SONY_TUNERDEMOD_DVBT2_TUNE_INFO_INVALID_PLP_ID)) {
					printk("DVB-T2 PLP ID error in acquisition:\n");
					ew100_ret = sony_tunerdemod_dvbt2_monitor_ActivePLP(&ew100_driver.tunerDemod, SONY_DVBT2_PLP_DATA, &ew100_plpInfo);
					if (ew100_ret != SONY_RESULT_OK) {
						printk(" DVB-T2 Error : sony_tunerdemod_dvbt2_monitor_ActivePLP failed. (result = %d)\n",ew100_ret);
						goto exit;
					} else {
						printk(" - PLP Requested : %u\n", ew100_tunerParam_DVBT2.dataPLPID);
						printk(" - PLP Acquired  : %u\n\n", ew100_plpInfo.id);
					}
				}
				msleep(1000);
			}
		} else {
			DRV_CXD2880_DEBUG("DRV_CXD2880_acquireChannel DVB-T\n");
			if (frequencyKHz != 0) {
				ew100_tunerParam_DVBT.centerFreqKHz = frequencyKHz;
				if (bandwidth == 5000)
					ew100_tunerParam_DVBT.bandwidth = SONY_DTV_BW_5_MHZ;
				else if (bandwidth == 6000)
					ew100_tunerParam_DVBT.bandwidth = SONY_DTV_BW_6_MHZ;
				else if (bandwidth == 7000)
					ew100_tunerParam_DVBT.bandwidth = SONY_DTV_BW_7_MHZ;
				else
					ew100_tunerParam_DVBT.bandwidth = SONY_DTV_BW_8_MHZ;
				ew100_tunerParam_DVBT.profile = SONY_DVBT_PROFILE_HP;
				ew100_ret = sony_integ_dvbt_Tune(&ew100_driver.tunerDemod, &ew100_tunerParam_DVBT);
				if (ew100_ret != SONY_RESULT_OK) {
					printk("DVB-T Error : ew100_ret = %d\n", ew100_ret);
					goto exit;
				}
				msleep(1000);
			}	
		}
	} 
	
	return 0;
exit:
	printk("something wrong!!!!!!!!!\n");
	return 1;
}

Dword DRV_CXD2880_isLocked(Device_Context *DC, Bool* locked, int br_idx, int tsSrcIdx)
{
	sony_result_t					ew100_ret = SONY_RESULT_OK;
	uint8_t							ew100_syncStat = 0;
	uint8_t							ew100_tsLockStat = 0;
	uint8_t							ew100_unlockDetect = 0;

	*locked = true;

	if (cxd2880_lock_mode == TV_STANDARD_ISDBT) {
		DRV_CXD2880_DEBUG("DRV_CXD2880_isLocked ISDBT\n");
		//Demod Lock
		ew100_ret = sony_tunerdemod_isdbt_monitor_SyncStat(&ew100_driver.tunerDemod, &ew100_syncStat, &ew100_tsLockStat, &ew100_unlockDetect);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("ISDBT Error : ew100_ret = %d\n", ew100_ret);
			goto exit;
		} else {
			if (ew100_syncStat == 6) {
				DRV_CXD2880_DEBUG("ISDBT Demod Lock\n");
			} else if (ew100_syncStat >= 1 && ew100_syncStat <= 5) 
				DRV_CXD2880_DEBUG("ISDBT Demod Unlock\n");
			else
				DRV_CXD2880_DEBUG("ISDBT Demod ??\n");

			if (ew100_tsLockStat == 1) 
				DRV_CXD2880_DEBUG("ISDBT TS Lock\n");
			else if (ew100_tsLockStat == 0) {
				DRV_CXD2880_DEBUG("ISDBT Ts Unlock\n");
				*locked = False;
				return SONY_RESULT_ERROR_UNLOCK;
			} else
				DRV_CXD2880_DEBUG("ISDBT Ts ??\n");

			if (ew100_unlockDetect == 1) {
				DRV_CXD2880_DEBUG("ISDBT Early Unlock Detected\n");
				*locked = False;
				return SONY_RESULT_ERROR_UNLOCK;
			}
		}
	} else if (cxd2880_lock_mode == TV_STANDARD_DVBT_T2)  {
		DRV_CXD2880_DEBUG("DRV_CXD2880_isLocked DVB-T2\n");
		//Demod Lock
		ew100_ret = sony_tunerdemod_dvbt2_monitor_SyncStat(&ew100_driver.tunerDemod, &ew100_syncStat, &ew100_tsLockStat, &ew100_unlockDetect);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T2 Error : ew100_ret = %d\n", ew100_ret);
			goto exit;
		} else {
			if (ew100_syncStat == 6) 
				DRV_CXD2880_DEBUG("DVB-T2 Demod Lock\n");
			else if (ew100_syncStat >= 1 && ew100_syncStat <= 5) {
				DRV_CXD2880_DEBUG("DVB-T2 Demod Unlock\n");
				*locked = False;
				return SONY_RESULT_ERROR_UNLOCK;
			} else
				DRV_CXD2880_DEBUG("DVB-T2 Demod ??\n");

			if (ew100_tsLockStat == 1) 
				DRV_CXD2880_DEBUG("DVB-T2 TS Lock\n");
			else if (ew100_tsLockStat == 0) {
				DRV_CXD2880_DEBUG("DVB-T2 Ts Unlock\n");
				*locked = False;
				return SONY_RESULT_ERROR_UNLOCK;
			} else
				DRV_CXD2880_DEBUG("DVB-T2 Ts ??\n");

			if (ew100_unlockDetect == 1) {
				DRV_CXD2880_DEBUG("DVB-T2 Early Unlock Detected\n");
				*locked = False;
				return SONY_RESULT_ERROR_UNLOCK;
			}
		}	
	} else {
		DRV_CXD2880_DEBUG("DRV_CXD2880_isLocked DVB-T\n");
		//Demod Lock
		ew100_ret = sony_tunerdemod_dvbt_monitor_SyncStat(&ew100_driver.tunerDemod, &ew100_syncStat, &ew100_tsLockStat, &ew100_unlockDetect);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T Error : ew100_ret = %d\n", ew100_ret);
			goto exit;
		} else {
			if (ew100_syncStat == 6) 
				DRV_CXD2880_DEBUG("DVB-T Demod Lock\n");
			else if (ew100_syncStat >= 1 && ew100_syncStat <= 5) {
				DRV_CXD2880_DEBUG("DVB-T Demod Unlock\n");
				*locked = False;
				return SONY_RESULT_ERROR_UNLOCK;
			} else
				DRV_CXD2880_DEBUG("DVB-T Demod ??\n");

			if (ew100_tsLockStat == 1)
				DRV_CXD2880_DEBUG("DVB-T TS Lock\n");
			else if (ew100_tsLockStat == 0) {
				DRV_CXD2880_DEBUG("DVB-T Ts Unlock\n");
				*locked = False;
				return SONY_RESULT_ERROR_UNLOCK;
			} else
				DRV_CXD2880_DEBUG("DVB-T Ts ??\n");

			if (ew100_unlockDetect == 1) {
				DRV_CXD2880_DEBUG("DVB-T Early Unlock Detected\n");
				*locked = False;
				return SONY_RESULT_ERROR_UNLOCK;
			}
		}
	} 
	
	return 0;
exit:
	printk("something wrong!!!!!!!!!\n");
	return 1;
}

Byte DRV_CXD2880_normalization_rf_power(int32_t rfLeveldB,Byte lock_mode)
{
	if (lock_mode == TV_STANDARD_ISDBT) {
		DRV_CXD2880_DEBUG("DRV_CXD2880_normalization_rf_power ISDBT\n");
		if(rfLeveldB < CXD2880_ISDBT_RF_POWER_MIN_BOUNDARY) 
			return 0;
		if(rfLeveldB > CXD2880_ISDBT_RF_POWER_MAX_BOUNDARY) 
			return 100;
		
		return (Byte)(((rfLeveldB-(CXD2880_ISDBT_RF_POWER_MIN_BOUNDARY))*100)/(CXD2880_ISDBT_RF_POWER_MAX_BOUNDARY - CXD2880_ISDBT_RF_POWER_MIN_BOUNDARY));

	} else if (lock_mode == TV_STANDARD_DVBT_T2) {
		DRV_CXD2880_DEBUG("DRV_CXD2880_normalization_rf_power DVB-T2\n");
		if(rfLeveldB < CXD2880_DVBT2_RF_POWER_MIN_BOUNDARY) 
			return 0;
		if(rfLeveldB > CXD2880_DVBT2_RF_POWER_MAX_BOUNDARY) 
			return 100;
		
		return (Byte)(((rfLeveldB-(CXD2880_DVBT2_RF_POWER_MIN_BOUNDARY))*100)/(CXD2880_DVBT2_RF_POWER_MAX_BOUNDARY - CXD2880_DVBT2_RF_POWER_MIN_BOUNDARY));
	} else {
		DRV_CXD2880_DEBUG("DRV_CXD2880_normalization_rf_power DVB-T\n");
		if(rfLeveldB < CXD2880_DVBT_RF_POWER_MIN_BOUNDARY) 
			return 0;
		if(rfLeveldB > CXD2880_DVBT_RF_POWER_MAX_BOUNDARY) 
			return 100;
		
		return (Byte)(((rfLeveldB-(CXD2880_DVBT_RF_POWER_MIN_BOUNDARY))*100)/(CXD2880_DVBT_RF_POWER_MAX_BOUNDARY - CXD2880_DVBT_RF_POWER_MIN_BOUNDARY));
	}
}

Byte DRV_CXD2880_normalization_snr(int32_t snr,Byte lock_mode)
{
	if (lock_mode == TV_STANDARD_ISDBT) {
		DRV_CXD2880_DEBUG("DRV_CXD2880_normalization_snr ISDBT\n");
		if(snr < CXD2880_ISDBT_SNR_MIN_BOUNDARY) 
			return 0;
		if(snr > CXD2880_ISDBT_SNR_MAX_BOUNDARY) 
			return 100;

		return (Byte)(((snr-(CXD2880_ISDBT_SNR_MIN_BOUNDARY))*100)/(CXD2880_ISDBT_SNR_MAX_BOUNDARY - CXD2880_ISDBT_SNR_MIN_BOUNDARY));
	} else if (lock_mode == TV_STANDARD_DVBT_T2) {
		DRV_CXD2880_DEBUG("DRV_CXD2880_normalization_snr DVB-T2\n");
		if(snr < CXD2880_DVBT2_SNR_MIN_BOUNDARY) 
			return 0;
		if(snr > CXD2880_DVBT2_SNR_MAX_BOUNDARY) 
			return 100;
	
		return (Byte)(((snr-(CXD2880_DVBT2_SNR_MIN_BOUNDARY))*100)/(CXD2880_DVBT2_SNR_MAX_BOUNDARY - CXD2880_DVBT2_SNR_MIN_BOUNDARY));
	} else {
		DRV_CXD2880_DEBUG("DRV_CXD2880_normalization_snr DVB-T\n");
		if(snr < CXD2880_DVBT_SNR_MIN_BOUNDARY) 
			return 0;
		if(snr > CXD2880_DVBT_SNR_MAX_BOUNDARY) 
			return 100;

		return (Byte)(((snr-(CXD2880_DVBT_SNR_MIN_BOUNDARY))*100)/(CXD2880_DVBT_SNR_MAX_BOUNDARY - CXD2880_DVBT_SNR_MIN_BOUNDARY));
	}
}

Dword DRV_CXD2880_getStatistic(Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int tsSrcIdx)
{
	sony_result_t					ew100_ret = SONY_RESULT_OK;
	uint8_t							ew100_syncStat = 0;
	uint8_t							ew100_tsLockStat = 0;
	uint8_t							ew100_unlockDetect = 0;
	int								ew100_rfLeveldB = 0;
	int								ew100_snr = 0;
	unsigned int					ew100_ber = 0, ew100_berA = 0, ew100_berB = 0, ew100_berC = 0;
	unsigned int					ew100_pen = 0, ew100_penA = 0, ew100_penB = 0, ew100_penC = 0;
	unsigned int					ew100_per = 0;
	uint8_t							ew100_ssi = 0;
	uint8_t							ew100_quality = 0 ;					
	const char *constellation_str[] = { "QPSK", "16-QAM", "64-QAM" };
	const char *hierarchy_str[] = { "Non", "A1", "A2", "A4" };
	const char *coderate_str[] = { "1/2", "2/3", "3/4", "5/6", "7/8" };
	const char *gi_str[] = { "1/32", "1/16", "1/8", "1/4" };
	const char *mode_str[] = { "2K", "8K" };
	sony_dvbt_tpsinfo_t tpsInfo;
	
	getstatisticrequest->statistic.signalPresented = false;       
    getstatisticrequest->statistic.signalLocked = false;   
    getstatisticrequest->statistic.signalQuality = 0;                                                        
    getstatisticrequest->statistic.signalStrength = 0;
    getstatisticrequest->snr = 0;  
    getstatisticrequest->rf_power = 0;  

	if (cxd2880_lock_mode == TV_STANDARD_ISDBT) {
		DRV_CXD2880_DEBUG("DRV_CXD2880_getStatistic ISDBT\n");
		//RF Level			
		ew100_ret = sony_tunerdemod_monitor_RFLevel(&ew100_driver.tunerDemod, &ew100_rfLeveldB);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("ISDBT Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			DRV_CXD2880_DEBUG("ISDBT RF Level : %d\n", (int)(ew100_rfLeveldB / 1000));
			getstatisticrequest->statistic.signalStrength = DRV_CXD2880_normalization_rf_power(ew100_rfLeveldB / 1000,TV_STANDARD_ISDBT);
			getstatisticrequest->rf_power = (int)(ew100_rfLeveldB / 1000);  
		}

		//Demod Lock
		ew100_ret = sony_tunerdemod_isdbt_monitor_SyncStat(&ew100_driver.tunerDemod, &ew100_syncStat, &ew100_tsLockStat, &ew100_unlockDetect);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("ISDBT Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			if (ew100_syncStat == 6)
				DRV_CXD2880_DEBUG("ISDBT Demod Lock\n");
			else if (ew100_syncStat >= 1 && ew100_syncStat <= 5) 
				DRV_CXD2880_DEBUG("ISDBT Demod Unlock\n");
			else
				DRV_CXD2880_DEBUG("ISDBT Demod ??\n");

			if (ew100_tsLockStat == 1) {
				DRV_CXD2880_DEBUG("ISDBT TS Lock\n");
				getstatisticrequest->statistic.signalLocked = True;
			} else if (ew100_tsLockStat == 0) {
				DRV_CXD2880_DEBUG("ISDBT Ts Unlock\n");
				getstatisticrequest->statistic.signalLocked = false;
			} else
				DRV_CXD2880_DEBUG("ISDBT Ts ??\n");

			if (ew100_unlockDetect == 1) {
				DRV_CXD2880_DEBUG("ISDBT Early Unlock Detected\n");
				getstatisticrequest->statistic.signalPresented = false;
			} else {
				getstatisticrequest->statistic.signalPresented = True;
			}
		}

		//SNR
		ew100_ret = sony_tunerdemod_isdbt_monitor_SNR(&ew100_driver.tunerDemod, &ew100_snr);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("ISDBT Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			DRV_CXD2880_DEBUG("ISDBT SNR : %d\n", (int)(ew100_snr / 1000));
			getstatisticrequest->statistic.signalQuality = DRV_CXD2880_normalization_snr(ew100_snr,TV_STANDARD_ISDBT);
			getstatisticrequest->snr = (int)(ew100_snr / 1000);  
		}
			
		//BER
		cxd2880_channelStatistic.postVitBitCount = 10000000;	
		ew100_ret = sony_tunerdemod_isdbt_monitor_PreRSBER(&ew100_driver.tunerDemod, &ew100_berA, &ew100_berB, &ew100_berC);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("ISDBT Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			DRV_CXD2880_DEBUG("ISDBT Layer A BER : %d * 10^-7\n", ew100_berA);
			DRV_CXD2880_DEBUG("ISDBT Layer B BER : %d * 10^-7\n", ew100_berB);
			DRV_CXD2880_DEBUG("ISDBT Layer C BER : %d * 10^-7\n", ew100_berC);
			cxd2880_channelStatistic.postVitErrorCount = ew100_berA;
		}

		//Packet Error
		ew100_ret = sony_tunerdemod_isdbt_monitor_PacketErrorNumber(&ew100_driver.tunerDemod, &ew100_penA, &ew100_penB, &ew100_penC);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("ISDBT Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			DRV_CXD2880_DEBUG("ISDBT Layer A Packet Error (1 sec) : %d\n", ew100_penA);
			DRV_CXD2880_DEBUG("ISDBT Layer A Packet Error (1 sec) : %d\n", ew100_penB);
			DRV_CXD2880_DEBUG("ISDBT Layer A Packet Error (1 sec) : %d\n", ew100_penC);
		}

	} else if (cxd2880_lock_mode == TV_STANDARD_DVBT_T2) {	
		DRV_CXD2880_DEBUG("DRV_CXD2880_getStatistic DVB-T2\n");
		//RF Level
		ew100_ret = sony_tunerdemod_monitor_RFLevel(&ew100_driver.tunerDemod, &ew100_rfLeveldB);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T2 Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			DRV_CXD2880_DEBUG("DVB-T2 RF Level : %d\n", (int)(ew100_rfLeveldB / 1000));
			getstatisticrequest->statistic.signalStrength = DRV_CXD2880_normalization_rf_power(ew100_rfLeveldB / 1000,TV_STANDARD_DVBT_T2);
			getstatisticrequest->rf_power = (int)(ew100_rfLeveldB / 1000); 
		}

		//Demod Lock
		ew100_ret = sony_tunerdemod_dvbt2_monitor_SyncStat(&ew100_driver.tunerDemod, &ew100_syncStat, &ew100_tsLockStat, &ew100_unlockDetect);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T2 Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			if (ew100_syncStat == 6) 
				DRV_CXD2880_DEBUG("DVB-T2 Demod Lock\n");
			else if (ew100_syncStat >= 1 && ew100_syncStat <= 5) 
				DRV_CXD2880_DEBUG("DVB-T2 Demod Unlock\n");
			else
				DRV_CXD2880_DEBUG("DVB-T2 Demod ??\n");

			if (ew100_tsLockStat == 1) {
				DRV_CXD2880_DEBUG("DVB-T2 TS Lock\n");
				getstatisticrequest->statistic.signalLocked = True;
			} else if (ew100_tsLockStat == 0) {
				DRV_CXD2880_DEBUG("DVB-T2 Ts Unlock\n");
				getstatisticrequest->statistic.signalLocked = false;
			} else
				DRV_CXD2880_DEBUG("DVB-T2 Ts ??\n");

			if (ew100_unlockDetect == 1) {
				DRV_CXD2880_DEBUG("DVB-T2 Early Unlock Detected\n");
				getstatisticrequest->statistic.signalPresented = false;
			} else {
				getstatisticrequest->statistic.signalPresented = True;
			}
		}

		//SNR
		ew100_ret = sony_tunerdemod_dvbt2_monitor_SNR(&ew100_driver.tunerDemod, &ew100_snr);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T2 Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			DRV_CXD2880_DEBUG("DVB-T2 SNR : %d\n", (int)(ew100_snr / 1000));
			getstatisticrequest->statistic.signalQuality = DRV_CXD2880_normalization_snr(ew100_snr,TV_STANDARD_DVBT_T2);
			getstatisticrequest->snr = (int)(ew100_snr / 1000);  
		}

		//BER
		cxd2880_channelStatistic.postVitBitCount = 1000000000;	
		ew100_ret = sony_tunerdemod_dvbt2_monitor_PreBCHBER(&ew100_driver.tunerDemod, &ew100_ber);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T2 Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			DRV_CXD2880_DEBUG("DVB-T2 BER : %d * 10^-9\n", ew100_ber);
			cxd2880_channelStatistic.postVitErrorCount = ew100_ber;
		}

		//PER
		ew100_ret = sony_tunerdemod_dvbt2_monitor_PER(&ew100_driver.tunerDemod, &ew100_per);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T2 Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else
			DRV_CXD2880_DEBUG("DVB-T2 PER : %d * 10^-9\n", ew100_per);

		//Packet Error
		ew100_ret = sony_tunerdemod_dvbt2_monitor_PacketErrorNumber(&ew100_driver.tunerDemod, &ew100_pen);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T2 Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else
			DRV_CXD2880_DEBUG("DVB-T2 Packet Error (1 sec) : %u\n", ew100_pen);

		//SSI
		ew100_ret = sony_tunerdemod_dvbt2_monitor_SSI(&ew100_driver.tunerDemod, &ew100_ssi);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T2 Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else
			DRV_CXD2880_DEBUG("DVB-T2 SSI : %d\n", ew100_ssi);

		//SQI
		ew100_ret = sony_tunerdemod_dvbt2_monitor_Quality(&ew100_driver.tunerDemod, &ew100_quality);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T2 Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else
			DRV_CXD2880_DEBUG("DVB-T2 SQI : %d\n", ew100_quality);
			
	} else {
		DRV_CXD2880_DEBUG("DRV_CXD2880_getStatistic DVB-T\n");
		//RF Level			
		ew100_ret = sony_tunerdemod_monitor_RFLevel(&ew100_driver.tunerDemod, &ew100_rfLeveldB);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			DRV_CXD2880_DEBUG("DVB-T RF Level : %d\n", (int)(ew100_rfLeveldB / 1000));
			getstatisticrequest->statistic.signalStrength = DRV_CXD2880_normalization_rf_power(ew100_rfLeveldB / 1000,!TV_STANDARD_DVBT_T2);
			getstatisticrequest->rf_power = (int)(ew100_rfLeveldB / 1000); 
		}

		//Demod Lock
		ew100_ret = sony_tunerdemod_dvbt_monitor_SyncStat(&ew100_driver.tunerDemod, &ew100_syncStat, &ew100_tsLockStat, &ew100_unlockDetect);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			if (ew100_syncStat == 6)
				DRV_CXD2880_DEBUG("DVB-T Demod Lock\n");
			else if (ew100_syncStat >= 1 && ew100_syncStat <= 5)
				DRV_CXD2880_DEBUG("DVB-T Demod Unlock\n");
			else
				DRV_CXD2880_DEBUG("DVB-T Demod ??\n");

			if (ew100_tsLockStat == 1) {
				DRV_CXD2880_DEBUG("DVB-T TS Lock\n");
				getstatisticrequest->statistic.signalLocked = True;
			} else if (ew100_tsLockStat == 0) {
				DRV_CXD2880_DEBUG("DVB-T Ts Unlock\n");
				getstatisticrequest->statistic.signalLocked = false;
			} else
				DRV_CXD2880_DEBUG("DVB-T Ts ??\n");

			if (ew100_unlockDetect == 1) {
				DRV_CXD2880_DEBUG("DVB-T Early Unlock Detected\n");
				getstatisticrequest->statistic.signalPresented = false;
			} else {
				getstatisticrequest->statistic.signalPresented = True;
			}
		}

		//SNR
		ew100_ret = sony_tunerdemod_dvbt_monitor_SNR(&ew100_driver.tunerDemod, &ew100_snr);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			DRV_CXD2880_DEBUG("DVB-T SNR : %d\n", (int)(ew100_snr / 1000));
			getstatisticrequest->statistic.signalQuality = DRV_CXD2880_normalization_snr(ew100_snr,!TV_STANDARD_DVBT_T2);
			getstatisticrequest->snr = (int)(ew100_snr / 1000);  
		}

		//BER
		cxd2880_channelStatistic.postVitBitCount = 10000000;	
		ew100_ret = sony_tunerdemod_dvbt_monitor_PreRSBER(&ew100_driver.tunerDemod, &ew100_ber);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else {
			DRV_CXD2880_DEBUG("DVB-T BER : %d * 10^-7\n", ew100_ber);
			cxd2880_channelStatistic.postVitErrorCount = ew100_ber;
		}

		//BER
		ew100_ret = sony_tunerdemod_dvbt_monitor_PER(&ew100_driver.tunerDemod, &ew100_per);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else
			DRV_CXD2880_DEBUG("DVB-T BER : %d * 10^-6\n", ew100_per);

		//Packet Error
		ew100_ret = sony_tunerdemod_dvbt_monitor_PacketErrorNumber(&ew100_driver.tunerDemod, &ew100_pen);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else
			DRV_CXD2880_DEBUG("DVB-T Packet Error (1 sec) : %u\n", ew100_pen);

		//SSI
		ew100_ret = sony_tunerdemod_dvbt_monitor_SSI(&ew100_driver.tunerDemod, &ew100_ssi);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else
			DRV_CXD2880_DEBUG("DVB-T SSI : %d\n", ew100_ssi);

		//SQI
		ew100_ret = sony_tunerdemod_dvbt_monitor_Quality(&ew100_driver.tunerDemod, &ew100_quality);
		if (ew100_ret != SONY_RESULT_OK) {
			printk("DVB-T Error : ew100_ret = %d\n", ew100_ret);
			//goto exit;
		} else
			DRV_CXD2880_DEBUG("DVB-T SQI : %d\n", ew100_quality);

		ew100_ret = sony_tunerdemod_dvbt_monitor_TPSInfo(&ew100_driver.tunerDemod, &tpsInfo);
		if (ew100_ret == SONY_RESULT_OK) {
			DRV_CXD2880_DEBUG("TPS Information          | Constellation   | %s\n", constellation_str[tpsInfo.constellation]);
			DRV_CXD2880_DEBUG("                         | Hierachy        | %s\n", hierarchy_str[tpsInfo.hierarchy]);
			DRV_CXD2880_DEBUG("                         | Code Rate HP    | %s\n", coderate_str[tpsInfo.rateHP]);
			DRV_CXD2880_DEBUG("                         | Code Rate LP    | %s\n", coderate_str[tpsInfo.rateLP]);
			DRV_CXD2880_DEBUG("                         | Guard Interval  | %s\n", gi_str[tpsInfo.guard]);
			DRV_CXD2880_DEBUG("                         | Mode            | %s\n", mode_str[tpsInfo.mode]);
		}
		DRV_CXD2880_DEBUG("-------------------------|-----------------|----------------- \n");
		
	} 
	
	return 0;
}

Dword DRV_CXD2880_getChannelStatistic(Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int ts_idx)
{	
	channelStatistic->abortCount = 0;
	channelStatistic->postVitBitCount = 0;
	channelStatistic->postVitErrorCount = 0;
	channelStatistic->softBitCount = 0;
	channelStatistic->softErrorCount = 0;
	channelStatistic->preVitBitCount = 0;
	channelStatistic->preVitErrorCount = 0;

	channelStatistic->postVitBitCount = cxd2880_channelStatistic.postVitBitCount;
	channelStatistic->postVitErrorCount = cxd2880_channelStatistic.postVitErrorCount;
	
	return 0;
}

Dword DRV_CXD2880_getMPLPID(Device_Context *DC, MPLPData *pMplpdata, int br_idx, int ts_idx)
{	
	sony_result_t	ew100_ret = SONY_RESULT_OK;
	uint8_t numPlps = 0;
	uint8_t plpIds[255] = {0};
	int i = 0;

	ew100_ret = sony_tunerdemod_dvbt2_monitor_DataPLPs(&ew100_driver.tunerDemod, plpIds, &numPlps);
	if (ew100_ret != SONY_RESULT_OK) {
		printk("DRV_CXD2880_getMPLPID Error : ew100_ret = %d\n", ew100_ret);
		goto exit;
	} else {
		DRV_CXD2880_DEBUG("numPlps = %d\n", numPlps);
		pMplpdata->TotalPLPnum = numPlps;
		for (i = 0; i < numPlps; i++) {
			DRV_CXD2880_DEBUG("plpIds[%d] = %d\n", i, plpIds[i]);	
			pMplpdata->PLPIDArray[i] = plpIds[i];
		}
		CXD2880_ActivePLPID = plpIds[0];			
		pMplpdata->ActivePLPID = CXD2880_ActivePLPID;	
		pMplpdata->FECLock = 0;		
	}
	
	return 0;
exit:
	printk("something wrong!!!!!!!!!\n");
	return 1;
}

Dword DRV_CXD2880_setPLPID(Device_Context *DC, MPLPData *pMplpdata, int br_idx, int ts_idx)
{	
	sony_result_t	ew100_ret = SONY_RESULT_OK;
	uint8_t select_plpId;

	select_plpId = pMplpdata->SetPLPID;
	ew100_ret = sony_integ_dvbt2_Scan_SwitchDataPLP(&ew100_driver.tunerDemod, ofdmInfo.mixed, select_plpId, profileFound);
	if (ew100_ret != SONY_RESULT_OK) {
		printk("DRV_CXD2880_setPLPID Error : ew100_ret = %d\n", ew100_ret);
		goto exit;
	} else 
		CXD2880_ActivePLPID = select_plpId;

	return 0;
exit:
	printk("something wrong!!!!!!!!!\n");
	return 1;
}

