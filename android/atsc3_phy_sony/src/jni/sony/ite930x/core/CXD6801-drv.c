#include "it930x-core.h"
#include "sony_cxd6801_apis.h"

//#define DEBUG_CXD6801	

#define ATSC1_RF_POWER_MAX_BOUNDARY -27
#define ATSC1_RF_POWER_MIN_BOUNDARY -83
#define ATSC3_RF_POWER_MAX_BOUNDARY -25
#define ATSC3_RF_POWER_MIN_BOUNDARY -87
#define ATSC1_SNR_MAX_BOUNDARY 34718
#define ATSC1_SNR_MIN_BOUNDARY 14918
#define ATSC3_SNR_MAX_BOUNDARY 40000
#define ATSC3_SNR_MIN_BOUNDARY 8900

#define CXD6801_DEBUG 0

#if CXD6801_DEBUG 
	#define DRV_CXD6801_DEBUG(args...)   printk(args)
#else
	#define DRV_CXD6801_DEBUG(args...)
#endif

sony_cxd6801_driver_instance_t	CXD6801_driver[4];
sony_cxd6801_result_t	result;
uint32_t error;
sony_cxd6801_demod_create_param_t CXD6801_createParam[4];
uint8_t	i2c_bus, demod_i2cAdd, tuner_i2cAdd;
Byte lock_mode = 0;
ChannelStatistic cxd6801_channelStatistic = {0,0,0,0,0,0,0};

Dword DRV_CXD6801_initialize(Device_Context *DC, int br_idx, int tsSrcIdx, Byte ChipType)
{
	
	uint32_t command;
	uint8_t	id;
	uint32_t frequencyKHz;
	Dword TSDATA_OUT = 0;

	//DRV_CXD6801_DEBUG("========== DRV_CXD6801_initialize chip %x ===========\n",tsSrcIdx);

	error = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, p_br_reg_ts_fail_ignore, 0x1F);
	if (error) {
		printk("IT9300_writeRegister fail\n");
		goto exit;
	}
	id = tsSrcIdx;

	if (id == 0) {
		i2c_bus = 3;
		demod_i2cAdd = 0xD8;
		tuner_i2cAdd = 0xC0;
	} else if (id == 1) {
		i2c_bus = 3;
		demod_i2cAdd = 0xDA;
		if (ChipType == EEPROM_CXD6801_EVB) 
			tuner_i2cAdd = 0xC0;
		else 
			tuner_i2cAdd = 0xC2;
	} else if (id == 2) {
		i2c_bus = 2;
		demod_i2cAdd = 0xD8;
		tuner_i2cAdd = 0xC0;
	} else {
		i2c_bus = 2;
		demod_i2cAdd = 0xDA;
		if (ChipType == EEPROM_CXD6801_EVB) 
			tuner_i2cAdd = 0xC0;
		else 
			tuner_i2cAdd = 0xC2;
	}
	
	if (ChipType == EEPROM_CXD6801_TS7)
		TSDATA_OUT = 1;
	else
		TSDATA_OUT = 0;

#ifdef DEBUG_CXD6801
	//For Debug
	i2c_bus = 3;
	demod_i2cAdd = 0xD8;
	tuner_i2cAdd = 0xC0;
#endif

	DRV_CXD6801_DEBUG("i2c_bus = %d\n",i2c_bus);
	DRV_CXD6801_DEBUG("demod_i2cAdd = %X\n",demod_i2cAdd);
	DRV_CXD6801_DEBUG("tuner_i2cAdd = %X\n",tuner_i2cAdd);

	drvi2c_cxd6801_ite_Initialize(&CXD6801_driver[id].iteI2c, (Endeavour*)&DC->it9300, i2c_bus);
	drvi2c_cxd6801_ite_CreateI2c(&CXD6801_driver[id].i2c, &CXD6801_driver[id].iteI2c);
	CXD6801_createParam[id].xtalFreq = SONY_CXD6801_DEMOD_XTAL_24000KHz;
	CXD6801_createParam[id].i2cAddressSLVT = demod_i2cAdd;
	CXD6801_createParam[id].tunerI2cConfig = SONY_CXD6801_DEMOD_TUNER_I2C_CONFIG_REPEATER;
	CXD6801_createParam[id].atscCoreDisable = 0; /* ATSC 1.0 core enable */

	if (id == 0) {
		//For CXD6800, 6801, 6802 SiP setting example
		//24MHz Xtal, I2C slave address is 0xC0, IF/AGC 2 is used, REFOUT enable
		result = sony_cxd6801_tuner_ascot3_Create(
			&CXD6801_driver[id].tuner,
			SONY_CXD6801_ASCOT3_XTAL_24000KHz,
			tuner_i2cAdd,
			&CXD6801_driver[id].i2c,
			SONY_CXD6801_ASCOT3_CONFIG_IFAGCSEL_ALL2 | SONY_CXD6801_ASCOT3_CONFIG_REFOUT_600mVpp,
			&CXD6801_driver[id].ascot3);
				
		if (result != SONY_CXD6801_RESULT_OK) {
			printk("sony_cxd6801_tuner_ascot3_Create  chip %x failed. (%d)\n",id, result);
			goto exit;
		}
			CXD6801_driver[id].ascot3.xosc_cap_set = 0x28; // For optimization 

	} else {
		//For CXD6800, 6801, 6802 SiP setting example
		//24MHz Xtal, I2C slave address is 0xC0, IF/AGC 2 is used, REFOUT enable
		result = sony_cxd6801_tuner_ascot3_Create(
		&CXD6801_driver[id].tuner,
		SONY_CXD6801_ASCOT3_XTAL_24000KHz,
		tuner_i2cAdd,
		&CXD6801_driver[id].i2c,
		SONY_CXD6801_ASCOT3_CONFIG_IFAGCSEL_ALL2 | SONY_CXD6801_ASCOT3_CONFIG_REFOUT_600mVpp | SONY_CXD6801_ASCOT3_CONFIG_EXT_REF,
		&CXD6801_driver[id].ascot3);
			
		if (result != SONY_CXD6801_RESULT_OK) {
			printk("sony_cxd6801_tuner_ascot3_Create  chip %x failed. (%d)\n",id, result);
			goto exit;
		}
			CXD6801_driver[id].ascot3.xosc_cap_set = 0x30; 
	}

	/* Construct sony_cxd6801_integ_t and sony_cxd6801_demod_t instances */
	result = sony_cxd6801_integ_Create(
		&CXD6801_driver[id].integ,
		&CXD6801_driver[id].demod,
		&CXD6801_createParam[id],
		&CXD6801_driver[id].i2c,
		&CXD6801_driver[id].tuner
		);
	if (result != SONY_CXD6801_RESULT_OK) {
		printk("sony_cxd6801_integ_Create  chip %x  failed. (%d)\n",id, result);
		goto exit;
	}

	result = sony_cxd6801_integ_Initialize(&CXD6801_driver[id].integ);
	if (result != SONY_CXD6801_RESULT_OK) {
		printk("sony_cxd6801_integ_Initialize  chip %x failed. (%d)\n",id, result);
		goto exit;
	}

	result = sony_cxd6801_demod_SetConfig(&CXD6801_driver[id].demod, SONY_CXD6801_DEMOD_CONFIG_OUTPUT_ATSC3, 1);
	if (result != SONY_CXD6801_RESULT_OK) {
		printk("sony_cxd6801_demod_SetConfig (SONY_CXD6801_DEMOD_CONFIG_OUTPUT_ATSC3)  chip %x failed. (%d)\n",id, result);
		goto exit;		
	}

	//for Debug
#ifdef DEBUG_CXD6801
	//DRV_CXD6801_DEBUG("P mode\n");
	error = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, p_br_reg_ts1_in_src, 1);
	if (error) {
		printk("IT9300_writeRegister chip %x fail\n",id);
		goto exit;			
	}
#else
	//DRV_CXD6801_DEBUG("S mode\n");
	result = sony_cxd6801_demod_SetConfig(&CXD6801_driver[id].demod, SONY_CXD6801_DEMOD_CONFIG_PARALLEL_SEL, 0);		// 0: serial, 1: Parallel(Default)
	if (result != SONY_CXD6801_RESULT_OK) {
		printk("sony_cxd6801_demod_SetConfig (SONY_CXD6801_DEMOD_CONFIG_PARALLEL_SEL) chip %x failed. (%d)\n",id, result);
		goto exit;
	}
	result = sony_cxd6801_demod_SetConfig(&CXD6801_driver[id].demod, SONY_CXD6801_DEMOD_CONFIG_SER_DATA_ON_MSB, TSDATA_OUT);		//0: TSDATA0, 1: TSDATA7(Default).
	if (result != SONY_CXD6801_RESULT_OK) {
		printk("sony_cxd6801_demod_SetConfig (SONY_CXD6801_DEMOD_CONFIG_SER_DATA_ON_MSB) chip %x failed. (%d)\n",id, result);
		goto exit;
	}
	result = sony_cxd6801_demod_SetConfig(&CXD6801_driver[id].demod, SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TS_SERIAL_CLK_FREQ, 1);	//3:64MHz
	if (result != SONY_CXD6801_RESULT_OK) {
		printk("sony_cxd6801_demod_SetConfig (SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TS_SERIAL_CLK_FREQ) chip %x failed. (%d)\n",id, result);
		goto exit;
	}

#endif
	printk("CXD6801 initialize chip %x succeeded.\n",id);
	
	return 0;
exit:
	DRV_CXD6801_DEBUG("something wrong!!!!!!!!!\n");
	return 1;
}

Dword DRV_CXD6801_setPlpConfig(Device_Context *DC, MPLPData *pMplpdata, int br_idx, int tsSrcIdx) {
	uint8_t	id = tsSrcIdx;

	uint8_t plpIDNum = pMplpdata->TotalPLPnum;
	uint8_t plpID[4] = { 0 };
	for(int i = 0; i < pMplpdata->TotalPLPnum && i < 4; i++) {
		plpID[i] = pMplpdata->PLPIDArray[i];
	}

	sony_cxd6801_demod_atsc3_SetPLPConfig(&CXD6801_driver[id].demod, plpIDNum, plpID);

	return 0;
}

Dword DRV_CXD6801_acquireChannel(Device_Context *DC, AcquireChannelRequest *request, int br_idx, int tsSrcIdx)
{
	uint8_t	id;
	uint32_t tmp;
	uint32_t frequencyKHz;
	sony_cxd6801_atsc3_tune_param_t	tuneParam3;
	sony_cxd6801_dtv_bandwidth_t	bandwidth;
	sony_cxd6801_atsc_tune_param_t	tuneParam1;

	id = tsSrcIdx;
	frequencyKHz = request->frequency;
	lock_mode = request->mode ;
	
	DRV_CXD6801_DEBUG("Select ID : %x\n",id);
	DRV_CXD6801_DEBUG("Freq_kHz : %d\n",frequencyKHz);
	DRV_CXD6801_DEBUG("request->mode : %x\n",request->mode);
	
	if (request->mode == TV_STANDARD_ATSC) {
		if (frequencyKHz != 0) {
			DRV_CXD6801_DEBUG("========== ATSC1.0 chip %x DRV_CXD6801_acquireChannel ===========\n",tsSrcIdx);
	
			tuneParam1.centerFreqKHz = frequencyKHz;
			result = sony_cxd6801_integ_atsc_Tune(&CXD6801_driver[id].integ, &tuneParam1);
			if (result == SONY_CXD6801_RESULT_ERROR_UNLOCK || result == SONY_CXD6801_RESULT_ERROR_TIMEOUT) {
				DRV_CXD6801_DEBUG("Unlock\n");
				printk("Error : sony_integ_atsc_Tune chip %x failed. (result = %d)\n",id, result);
				return (Dword)result;
			} else if (result != SONY_CXD6801_RESULT_OK) {
				printk("Error : sony_cxd6801_integ_atsc_Tune chip %x failed. (result = %d)\n",id, result);
				return (Dword)result;
			}
		} 
	}else if (request->mode == TV_STANDARD_ATSC3) {
		
		if (frequencyKHz != 0) {
			DRV_CXD6801_DEBUG("========== ATSC3.0 chip %x DRV_CXD6801_acquireChannel ===========\n",tsSrcIdx);

			//DRV_CXD6801_DEBUG("Bandwidth_kHz (6000, 7000, 8000) : ");
			//scanf_s("%d", &tmp);
			tmp = request->bandwidth;
			DRV_CXD6801_DEBUG("Bandwidth_kHz : %d\n",tmp);

			if (tmp == 6000) {
				bandwidth = SONY_CXD6801_DTV_BW_6_MHZ;
			} else if (tmp == 7000) {
				bandwidth = SONY_CXD6801_DTV_BW_7_MHZ;
			} else {
				bandwidth = SONY_CXD6801_DTV_BW_8_MHZ;
			}

			tuneParam3.centerFreqKHz = frequencyKHz;
			tuneParam3.bandwidth = bandwidth;			    // Channel bandwidth 
			tuneParam3.plpIDNum = 1;						// Number of valid PLP ID 
			tuneParam3.plpID[0] = 0;						// PLP ID 0 
			tuneParam3.plpID[1] = 1;						// PLP ID 1 
			tuneParam3.plpID[2] = 2;						// PLP ID 2 
			tuneParam3.plpID[3] = 3;						// PLP ID 3 

			result = sony_cxd6801_integ_atsc3_Tune(&CXD6801_driver[id].integ, &tuneParam3);
			if (result == SONY_CXD6801_RESULT_ERROR_UNLOCK || result == SONY_CXD6801_RESULT_ERROR_TIMEOUT) {
				DRV_CXD6801_DEBUG("Unlock\n");
				printk("Error : sony_integ_atsc3_Tune chip %x failed. (result = %d)\n",id, result);
				return (Dword)result;
			} else if (result != SONY_CXD6801_RESULT_OK) {
				printk("Error : sony_integ_atsc3_Tune chip %x failed. (result = %d)\n",id, result);
				return (Dword)result;
			}
		}
	} else {
		printk("CXD6801 mode select wrong !!!\n");
		return -1;
	}
	return 0;
}

Dword DRV_CXD6801_isLocked(Device_Context *DC, Bool* locked, int br_idx, int tsSrcIdx)
{
	uint8_t	id;
	uint8_t	vqLockState = 0;
	uint8_t	agcLockState = 0;
	uint8_t	tsLockState = 0;
	uint8_t	unlockDetected = 0;
	uint8_t	syncState = 0;
	uint8_t	alpLockState[4] = { 0 };
	uint8_t	alpLockAll = 0;
	uint8_t	alpUnlockDetected = 0;
	int i;
	
	id = tsSrcIdx;
	DRV_CXD6801_DEBUG("Select ID : %x\n",id);
	
	if (lock_mode == TV_STANDARD_ATSC) {
		
		DRV_CXD6801_DEBUG("========== ATSC1.0 chip %x DRV_CXD3801_isLocked ===========\n",tsSrcIdx);
		result = sony_cxd6801_demod_atsc_monitor_SyncStat(&CXD6801_driver[id].demod, &vqLockState, &agcLockState, &tsLockState, &unlockDetected);
		if (result == SONY_CXD6801_RESULT_OK) {
			if (tsLockState == 1) 
				DRV_CXD6801_DEBUG("TS Lock\n");
			else 
				DRV_CXD6801_DEBUG("TS UnLock\n");
			
			if (unlockDetected == 1)
				DRV_CXD6801_DEBUG("Early Unlock Detected !!!\n");
			
		} else {
			printk("Error : sony_cxd6801_demod_atsc_monitor_SyncStat failed. (result = %d)\n", result);
			*locked = False;
			return (Dword)result;
		}
	} else if (lock_mode == TV_STANDARD_ATSC3) {
		
		DRV_CXD6801_DEBUG("========== ATSC3.0 chip %x DRV_CXD6801_isLocked ===========\n",tsSrcIdx);
		result = sony_cxd6801_demod_atsc3_monitor_SyncStat(&CXD6801_driver[id].demod, &syncState, &alpLockState[0], &alpLockAll, &alpUnlockDetected);
		if (result == SONY_CXD6801_RESULT_OK) {
			for (i = 0; i < 4; i++) {
				if (alpLockState[i] == 1)
					DRV_CXD6801_DEBUG("ALP[%d] Lock\n", i);
				else
					DRV_CXD6801_DEBUG("ALP[%d] UnLock\n", i);
				
			}

			if (alpLockAll == 1)
				DRV_CXD6801_DEBUG("All PLPs Lock\n");
			else
				DRV_CXD6801_DEBUG("Not All PLPs Lock\n");
			
			if (alpUnlockDetected == 1)
				DRV_CXD6801_DEBUG("Early Unlock Detected !!!\n");
		} else {
			printk("Error : sony_cxd6801_demod_atsc3_monitor_SyncStat chip %x failed. (result = %d)\n",id, result);
			*locked = False;
			return (Dword)result;
		}
	} else {
		printk("CXD6801 mode select wrong !!!\n");
		*locked = False;
		return -1;
	}
	*locked = true;
	return 0;
}

Byte DRV_CXD6801_normalization_rf_power(int32_t	rfLeveldB, Byte lock_mode)
{
	if (lock_mode == TV_STANDARD_ATSC) {
		if(rfLeveldB < ATSC1_RF_POWER_MIN_BOUNDARY) 
			return 0;
		if(rfLeveldB > ATSC1_RF_POWER_MAX_BOUNDARY) 
			return 100;
		
		return (Byte)(((rfLeveldB-(ATSC1_RF_POWER_MIN_BOUNDARY))*100)/(ATSC1_RF_POWER_MAX_BOUNDARY - ATSC1_RF_POWER_MIN_BOUNDARY));
	} else {
		if(rfLeveldB < ATSC3_RF_POWER_MIN_BOUNDARY) 
			return 0;
		if(rfLeveldB > ATSC3_RF_POWER_MAX_BOUNDARY) 
			return 100;
		
		return (Byte)(((rfLeveldB-(ATSC3_RF_POWER_MIN_BOUNDARY))*100)/(ATSC3_RF_POWER_MAX_BOUNDARY - ATSC3_RF_POWER_MIN_BOUNDARY));
	}
}

Byte DRV_CXD6801_normalization_snr(int32_t snr, Byte lock_mode)
{
	if (lock_mode == TV_STANDARD_ATSC) {
		if(snr < ATSC1_SNR_MIN_BOUNDARY) 
			return 0;
		if(snr > ATSC1_SNR_MAX_BOUNDARY) 
			return 100;

		return (Byte)(((snr-(ATSC1_SNR_MIN_BOUNDARY))*100)/(ATSC1_SNR_MAX_BOUNDARY - ATSC1_SNR_MIN_BOUNDARY));
	} else {
		
		if(snr < ATSC3_SNR_MIN_BOUNDARY) 
			return 0;
		if(snr > ATSC3_SNR_MAX_BOUNDARY) 
			return 100;
	
		return (Byte)(((snr-(ATSC3_SNR_MIN_BOUNDARY))*100)/(ATSC3_SNR_MAX_BOUNDARY - ATSC3_SNR_MIN_BOUNDARY));
	}
}

Dword DRV_CXD6801_getStatistic(Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int tsSrcIdx)
{
	uint8_t	id;
	uint8_t	vqLockState = 0;
	uint8_t	agcLockState = 0;
	uint8_t	tsLockState = 0;
	uint8_t	unlockDetected = 0;
	uint8_t	syncState = 0;
	uint8_t	alpLockState[4] = { 0 };
	uint8_t	alpLockAll = 0;
	uint8_t	alpUnlockDetected = 0;
	int32_t	snr;
	uint32_t ber1;
	uint32_t ber3[4];
	int32_t	rfLeveldB;
	int i;
	
	getstatisticrequest->statistic.signalPresented = false;       
    getstatisticrequest->statistic.signalLocked = false;   
    getstatisticrequest->statistic.signalQuality = 0;                                                        
    getstatisticrequest->statistic.signalStrength = 0;
    getstatisticrequest->snr = 0;  
    getstatisticrequest->rf_power = 0;            
	
	id = tsSrcIdx;
	DRV_CXD6801_DEBUG("Select ID : %x\n",id);
	
	if (lock_mode == TV_STANDARD_ATSC) {
		
		DRV_CXD6801_DEBUG("========== ATSC1.0 chip %x DRV_CXD6801_getStatistic ===========\n",tsSrcIdx);
		
		result = sony_cxd6801_demod_atsc_monitor_SyncStat(&CXD6801_driver[id].demod, &vqLockState, &agcLockState, &tsLockState, &unlockDetected);

		if (result == SONY_CXD6801_RESULT_OK) {
			if (tsLockState == 1) {
				DRV_CXD6801_DEBUG("TS Lock\n");
				getstatisticrequest->statistic.signalLocked = True;
			} else {
				DRV_CXD6801_DEBUG("TS UnLock\n");
				getstatisticrequest->statistic.signalLocked = false;
			}

			if (unlockDetected == 1) {
				getstatisticrequest->statistic.signalPresented = false;
				DRV_CXD6801_DEBUG("Early Unlock Detected !!!\n");
				
			} else {
				getstatisticrequest->statistic.signalPresented = True;
			}
		} else {
			printk("Error : sony_cxd6801_demod_atsc_monitor_SyncStat failed. (result = %d)\n", result);
			//return (Dword)result;
		}
		
		//SNR			
		result = sony_cxd6801_demod_atsc_monitor_SNR(&CXD6801_driver[id].demod, &snr);
		if (result == SONY_CXD6801_RESULT_OK) {
			getstatisticrequest->statistic.signalQuality = DRV_CXD6801_normalization_snr(snr,TV_STANDARD_ATSC);
			getstatisticrequest->snr = (int32_t)(snr/1000);  
			DRV_CXD6801_DEBUG("signalQuality = %d\n",getstatisticrequest->statistic.signalQuality);	
			DRV_CXD6801_DEBUG("chip %x SNR = %d x 10^-3 dB\n",id, snr);
		} else {
			printk("chip %x sony_cxd6801_demod_atsc_monitor_SNR Error = %d\n",id, result);
			//return (Dword)result;
		}

		//BER
		cxd6801_channelStatistic.postVitBitCount = 10000000;			
		result = sony_cxd6801_demod_atsc_monitor_PreRSBER(&CXD6801_driver[id].demod, &ber1);
		if (result == SONY_CXD6801_RESULT_OK)
		{
			DRV_CXD6801_DEBUG("chip %x PreRSBER = %u x 10^-7\n",id, ber1);
			cxd6801_channelStatistic.postVitErrorCount = ber1;
		} else {
			printk("chip %x sony_cxd6801_demod_atsc_monitor_PreRSBER Error = %d\n",id, result);
			//return (Dword)result;
		}
		
		//RF Power
		result = sony_cxd6801_integ_atsc_monitor_RFLevel(&CXD6801_driver[id].integ, &rfLeveldB);
		if (result == SONY_CXD6801_RESULT_OK)
		{
			DRV_CXD6801_DEBUG("rfLeveldB = %d\n",rfLeveldB);
			DRV_CXD6801_DEBUG("chip %x RF Power = %d dB\n",id, rfLeveldB / 1000);
			getstatisticrequest->statistic.signalStrength = DRV_CXD6801_normalization_rf_power(rfLeveldB / 1000,TV_STANDARD_ATSC);
			getstatisticrequest->rf_power = rfLeveldB / 1000;  
			DRV_CXD6801_DEBUG("signalStrength = %d\n",getstatisticrequest->statistic.signalStrength);	
		} else {
			printk("chip %x sony_cxd6801_integ_atsc_monitor_RFLevel Error = %d\n",id, result);
			//return (Dword)result;
		}
	} else if (lock_mode == TV_STANDARD_ATSC3) {
		
		DRV_CXD6801_DEBUG("========== ATSC3.0 chip %x DRV_CXD6801_getStatistic ===========\n",tsSrcIdx);
		
		result = sony_cxd6801_demod_atsc3_monitor_SyncStat(&CXD6801_driver[id].demod, &syncState, &alpLockState[0], &alpLockAll, &alpUnlockDetected);

		if (result == SONY_CXD6801_RESULT_OK) {
			for (i = 0; i < 4; i++) {
				if (alpLockState[i] == 1)
					DRV_CXD6801_DEBUG("ALP[%d] Lock\n", i);
				else
					DRV_CXD6801_DEBUG("ALP[%d] UnLock\n", i);
			}
			if (alpLockAll == 1) {
				DRV_CXD6801_DEBUG("All PLPs Lock\n");				
				getstatisticrequest->statistic.signalLocked = True;
			} else {
				DRV_CXD6801_DEBUG("Not All PLPs Lock\n");
				getstatisticrequest->statistic.signalLocked = false;
			}
			if (alpUnlockDetected == 1){
				getstatisticrequest->statistic.signalPresented = false;
				printk("Early Unlock Detected !!!\n");
			} else {
				getstatisticrequest->statistic.signalPresented = True;
			}
				
			if (alpLockAll == 1) {
				//SNR			
				result = sony_cxd6801_demod_atsc3_monitor_SNR(&CXD6801_driver[id].demod, &snr);
				if (result == SONY_CXD6801_RESULT_OK) {
					getstatisticrequest->statistic.signalQuality = DRV_CXD6801_normalization_snr(snr,TV_STANDARD_ATSC3);
					getstatisticrequest->snr = (int32_t)(snr/1000);  
					DRV_CXD6801_DEBUG("signalQuality = %d\n",getstatisticrequest->statistic.signalQuality);
					DRV_CXD6801_DEBUG("chip %x SNR = %d x 10^-3 dB\n",id, snr);
				} else {
					printk("chip %x sony_cxd6801_demod_atsc3_monitor_SNR Error = %d\n",id, result);
					//return (Dword)result;
				}

				//BER
				cxd6801_channelStatistic.postVitBitCount = 1000000000;			
				result = sony_cxd6801_demod_atsc3_monitor_PreBCHBER(&CXD6801_driver[id].demod, &ber3[0]);
				if (result == SONY_CXD6801_RESULT_OK) {
					for (i = 0; i < 4; i++) {
						if (ber3[i] == SONY_CXD6801_DEMOD_ATSC3_MONITOR_PRELDPCBER_INVALID)
							DRV_CXD6801_DEBUG("chip %x Pre-BCH BER[%d] : Unused\n",id, i);
						else {
							DRV_CXD6801_DEBUG("chip %x Pre-BCH BER[%d] = %u x 10^-9\n",id, i, ber3[i]);
							cxd6801_channelStatistic.postVitErrorCount = ber3[0];
						}
					}
				} else {
					printk("chip %x sony_cxd6801_demod_atsc3_monitor_PreBCHBER Error = %d\n",id, result);
					//return (Dword)result;
				}
			}
		
			//RF Power
			result = sony_cxd6801_integ_atsc3_monitor_RFLevel(&CXD6801_driver[id].integ, &rfLeveldB);	
			if (result == SONY_CXD6801_RESULT_OK) {
				DRV_CXD6801_DEBUG("rfLeveldB = %d\n",rfLeveldB);
				DRV_CXD6801_DEBUG("chip %x RF Power = %d dB\n",id, rfLeveldB / 1000);
				getstatisticrequest->statistic.signalStrength = DRV_CXD6801_normalization_rf_power(rfLeveldB / 1000,TV_STANDARD_ATSC3);
				getstatisticrequest->rf_power = rfLeveldB / 1000;  
				DRV_CXD6801_DEBUG("signalStrength = %d\n",getstatisticrequest->statistic.signalStrength);
			} else {
				printk("chip %x sony_cxd6801_integ_atsc3_monitor_RFLevel Error = %d\n",id, result);
				//return (Dword)result;
			}
		} else {
			printk("Error : sony_cxd6801_demod_atsc3_monitor_SyncStat chip %x failed. (result = %d)\n",id, result);
			//return (Dword)result;
		}
	} else {
		printk("CXD6801 mode select wrong !!!\n");
		//return -1;
	}
	return 0;
}

Dword DRV_CXD6801_getChannelStatistic(Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int ts_idx)
{
	Dword error = Error_NO_ERROR;
	
	channelStatistic->abortCount = 0;
	channelStatistic->postVitBitCount = 0;
	channelStatistic->postVitErrorCount = 0;
	channelStatistic->softBitCount = 0;
	channelStatistic->softErrorCount = 0;
	channelStatistic->preVitBitCount = 0;
	channelStatistic->preVitErrorCount = 0;

	channelStatistic->postVitBitCount = cxd6801_channelStatistic.postVitBitCount;
	channelStatistic->postVitErrorCount = cxd6801_channelStatistic.postVitErrorCount;
	
	return error;
}

Dword DRV_CXD6801_readRegister(Device_Context *DC, Processor processor, Byte option , Dword registerAddress, Byte bufferLength, Byte* buffer, int br_idx, int ts_idx)
{
	uint8_t data = 0;
	uint8_t	id;
	Dword error = SONY_CXD6801_RESULT_OK;
	id = ts_idx;
	
	DRV_CXD6801_DEBUG("====== DRV_CXD6801_readRegister ======\n");
	DRV_CXD6801_DEBUG("Select ID : %x\n",id);
	DRV_CXD6801_DEBUG("processor = %d\n",processor);
	DRV_CXD6801_DEBUG("option = %d\n",option);
	DRV_CXD6801_DEBUG("registerAddress = 0x%lx\n",registerAddress);
	DRV_CXD6801_DEBUG("bufferLength = %d\n",bufferLength);
	DRV_CXD6801_DEBUG("data = 0x%x\n",data);
	DRV_CXD6801_DEBUG("CXD6801_driver[%x].demod.i2cAddressSLVX = %x\n",id,CXD6801_driver[id].demod.i2cAddressSLVX);
	DRV_CXD6801_DEBUG("CXD6801_driver[%x].demod.i2cAddressSLVR = %x\n",id,CXD6801_driver[id].demod.i2cAddressSLVR);
	DRV_CXD6801_DEBUG("CXD6801_driver[%x].demod.i2cAddressSLVM = %x\n",id,CXD6801_driver[id].demod.i2cAddressSLVM);
	DRV_CXD6801_DEBUG("CXD6801_driver[%x].demod.i2cAddressSLVT = %x\n",id,CXD6801_driver[id].demod.i2cAddressSLVT);
	DRV_CXD6801_DEBUG("CXD6801_driver[%x].tuner.i2cAddress = %x\n",id,CXD6801_driver[id].tuner.i2cAddress);
	 
	if(processor == Processor_DEMOD) {
		switch (option) {
		case DEMOD_SLVX:
			if (CXD6801_driver[id].demod.pI2c->ReadRegister(CXD6801_driver[id].demod.pI2c, 
															CXD6801_driver[id].demod.i2cAddressSLVX, 
															(Byte)registerAddress, &data, 1) != SONY_CXD6801_RESULT_OK) 
				printk("chip [%d] Demod ReadRegister SLVX bank fail!\n",id);
			break;
		case DEMOD_SLVR:
			if (CXD6801_driver[id].demod.pI2c->ReadRegister(CXD6801_driver[id].demod.pI2c, 
															CXD6801_driver[id].demod.i2cAddressSLVR, 
															(Byte)registerAddress, &data, 1) != SONY_CXD6801_RESULT_OK) 
				printk("chip [%d] Demod ReadRegister SLVR bank fail!\n",id);
			break;
		case DEMOD_SLVM:
			if (CXD6801_driver[id].demod.pI2c->ReadRegister(CXD6801_driver[id].demod.pI2c, 
															CXD6801_driver[id].demod.i2cAddressSLVM, 
															(Byte)registerAddress, &data, 1) != SONY_CXD6801_RESULT_OK) 
				printk("chip [%d] Demod ReadRegister SLVM bank fail!\n",id);	
			break;
		case DEMOD_SLVT:
			if (CXD6801_driver[id].demod.pI2c->ReadRegister(CXD6801_driver[id].demod.pI2c, 
															CXD6801_driver[id].demod.i2cAddressSLVT, 
															(Byte)registerAddress, &data, 1) != SONY_CXD6801_RESULT_OK) 
				printk("chip [%d] Demod ReadRegister SLVT bank fail!\n",id);
			break;
		default:
			printk("chip [%d] Demod ReadRegister UNKNOWN bank %x!\n",id,option);
			error = SONY_CXD6801_RESULT_ERROR_ARG;
			goto exit;
			break;
		}
		buffer[0] = data;
	} else if (processor == Processor_TUNER) {
		
		result = sony_cxd6801_demod_I2cRepeaterEnable (&CXD6801_driver[id].demod, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
			printk("chip [%d] TUNER ReadRegister sony_cxd6801_demod_I2cRepeaterEnable function fail!\n",id);
			goto exit;
        }

		if (CXD6801_driver[id].tuner.pI2c->ReadRegister(CXD6801_driver[id].tuner.pI2c, 
														CXD6801_driver[id].tuner.i2cAddress, 
														(Byte)registerAddress, &data, 1) != SONY_CXD6801_RESULT_OK) {
			printk("chip [%d] TUNER ReadRegister fail!\n",id);
			goto exit;
		}
		buffer[0] = data;
		
        result = sony_cxd6801_demod_I2cRepeaterEnable (&CXD6801_driver[id].demod, 0x00);
		if (result != SONY_CXD6801_RESULT_OK) {
			printk("chip [%d] TUNER ReadRegister sony_cxd6801_demod_I2cRepeaterEnable function fail!\n",id);
			goto exit;
        }
		
	} else {
		DRV_CXD6801_DEBUG("chip [%x] DRV_CXD6801_readRegister unknown processor %d!\n",id,processor);
		error = SONY_CXD6801_RESULT_ERROR_ARG;
	}
	return error;
exit:
	return error;
}

Dword DRV_CXD6801_writeRegister(Device_Context *DC, Processor processor, Byte option , Dword registerAddress, Byte bufferLength, Byte* buffer, int br_idx, int ts_idx)
{
	uint8_t data = 0;
	uint8_t	id;
	Dword error = SONY_CXD6801_RESULT_OK;
	
	id = ts_idx;
	data = buffer[bufferLength-1];
	DRV_CXD6801_DEBUG("====== DRV_CXD6801_writeRegister ======\n");
	DRV_CXD6801_DEBUG("Select ID : %x\n",id);
	DRV_CXD6801_DEBUG("processor = %d\n",processor);
	DRV_CXD6801_DEBUG("option = %d\n",option);
	DRV_CXD6801_DEBUG("registerAddress = 0x%lx\n",registerAddress);
	DRV_CXD6801_DEBUG("bufferLength = %d\n",bufferLength);
	DRV_CXD6801_DEBUG("data = 0x%x\n",data);
	DRV_CXD6801_DEBUG("CXD6801_driver[%x].demod.i2cAddressSLVX = %x\n",id,CXD6801_driver[id].demod.i2cAddressSLVX);
	DRV_CXD6801_DEBUG("CXD6801_driver[%x].demod.i2cAddressSLVR = %x\n",id,CXD6801_driver[id].demod.i2cAddressSLVR);
	DRV_CXD6801_DEBUG("CXD6801_driver[%x].demod.i2cAddressSLVM = %x\n",id,CXD6801_driver[id].demod.i2cAddressSLVM);
	DRV_CXD6801_DEBUG("CXD6801_driver[%x].demod.i2cAddressSLVT = %x\n",id,CXD6801_driver[id].demod.i2cAddressSLVT);
	DRV_CXD6801_DEBUG("CXD6801_driver[%x].tuner.i2cAddress = %x\n",id,CXD6801_driver[id].tuner.i2cAddress);
	
	if(processor == Processor_DEMOD) {
		switch (option) {
		case DEMOD_SLVX:
			if (CXD6801_driver[id].demod.pI2c->WriteOneRegister(CXD6801_driver[id].demod.pI2c, 
																CXD6801_driver[id].demod.i2cAddressSLVX, 
																(Byte)registerAddress, data) != SONY_CXD6801_RESULT_OK) 
				printk("chip [%d] WriteOneRegister SLVX bank fail!\n",id);
			break;
		case DEMOD_SLVR:
			if (CXD6801_driver[id].demod.pI2c->WriteOneRegister(CXD6801_driver[id].demod.pI2c, 
																CXD6801_driver[id].demod.i2cAddressSLVR, 
																(Byte)registerAddress, data) != SONY_CXD6801_RESULT_OK) 
				printk("chip [%d] WriteOneRegister SLVR bank fail!\n",id);
			break;
		case DEMOD_SLVM:
			if (CXD6801_driver[id].demod.pI2c->WriteOneRegister(CXD6801_driver[id].demod.pI2c, 
																CXD6801_driver[id].demod.i2cAddressSLVM, 
																(Byte)registerAddress, data) != SONY_CXD6801_RESULT_OK) 
				printk("chip [%d] WriteOneRegister SLVM bank fail!\n",id);
			break;
		case DEMOD_SLVT:
			if (CXD6801_driver[id].demod.pI2c->WriteOneRegister(CXD6801_driver[id].demod.pI2c, 
																CXD6801_driver[id].demod.i2cAddressSLVT, 
																(Byte)registerAddress, data) != SONY_CXD6801_RESULT_OK) 
				printk("chip [%d] WriteOneRegister SLVT bank fail!\n",id);
			break;
		default:
			printk("chip [%d] WriteOneRegister UNKNOWN bank %x!\n",id,option);
			error = SONY_CXD6801_RESULT_ERROR_ARG;
			goto exit;
		}
	
	} else if (processor == Processor_TUNER) {
		
		result = sony_cxd6801_demod_I2cRepeaterEnable (&CXD6801_driver[id].demod, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
			printk("chip [%d] TUNER WriteOneRegister sony_cxd6801_demod_I2cRepeaterEnable function fail!\n",id);
			goto exit;
        }

		if (CXD6801_driver[id].tuner.pI2c->WriteOneRegister(CXD6801_driver[id].tuner.pI2c, 
															CXD6801_driver[id].tuner.i2cAddress, 
															(Byte)registerAddress, data) != SONY_CXD6801_RESULT_OK) {
			printk("chip [%d] TUNER WriteOneRegister fail!\n",id);
			goto exit;
		}
		
        result = sony_cxd6801_demod_I2cRepeaterEnable (&CXD6801_driver[id].demod, 0x00);
		if (result != SONY_CXD6801_RESULT_OK) {
			printk("chip [%d] TUNER WriteOneRegister sony_cxd6801_demod_I2cRepeaterEnable function fail!\n",id);
			goto exit;
        }
        
	} else {
		printk("chip [%x] DRV_CXD6801_writeRegister unknown processor %d!\n",id,processor);
		error = SONY_CXD6801_RESULT_ERROR_ARG;
		goto exit;
	}
	return error;
exit:
	return error;
}



