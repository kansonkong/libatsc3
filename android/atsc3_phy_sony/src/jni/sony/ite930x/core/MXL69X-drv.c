#include "it930x-core.h"
#include "mxl_eagle_apis.h"
#include "mxl_eagle_fw.h"

#define MXL69X_DEBUG 0

#if MXL69X_DEBUG 
	#define DRV_MXL69X_DEBUG(args...)   printk(args)
#else
	#define DRV_MXL69X_DEBUG(args...)
#endif

#define RF_POWER_MAX_BOUNDARY -1100
#define RF_POWER_MIN_BOUNDARY -8350
#define SNR_MAX_BOUNDARY 342
#define SNR_MIN_BOUNDARY 202

MXL_EAGLE_MPEGOUT_PARAMS_T 	mpegOutParamStruct;

Dword DRV_MXL69X_Initialize(Device_Context *DC, Byte br_idx, Byte tsSrcIdx, Byte ChipType)
{
	Dword dwError = Error_NO_ERROR;
	
	MXL_STATUS_E mxl_error_code;
	MXL_EAGLE_DEVICE_E			deviceType;
	MXL_EAGLE_DEV_XTAL_T 		devXtalStruct;

	MXL_EAGLE_DEV_VER_T 		DevVerStruct;
	MXL_EAGLE_DEV_STATUS_T 		StatusStruct;
	GPIO_STRUCT 				GpioStruct[4] = { {0xD8CB, 0xD8CC, 0xD8CD},{0xD8D7, 0xD8D8, 0xD8D9},{0xD8D3, 0xD8D4, 0xD8D5},{0xD8DF, 0xD8E0, 0xD8E1} };
	
	if (ChipType == EEPROM_MXL691 || ChipType == EEPROM_MXL691_DUALP)
		DC->MxL_EAGLE_Devices[tsSrcIdx].deviceType = MXL_EAGLE_DEVICE_691;
	else if (ChipType == EEPROM_MXL692)
		DC->MxL_EAGLE_Devices[tsSrcIdx].deviceType = MXL_EAGLE_DEVICE_692;
	else if (ChipType == EEPROM_MXL248)
		DC->MxL_EAGLE_Devices[tsSrcIdx].deviceType = MXL_EAGLE_DEVICE_248;
	
	// Reset MXL69X
	dwError = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, GpioStruct[tsSrcIdx].GpioOut, 0);
	if (dwError) 
		goto exit;
	dwError = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, GpioStruct[tsSrcIdx].GpioEn, 1);
	if (dwError) 
		goto exit;
	dwError = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, GpioStruct[tsSrcIdx].GpioOn, 1);
	if (dwError)
		goto exit;
	BrUser_delay(NULL, 200);
	dwError = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, GpioStruct[tsSrcIdx].GpioOut, 1);
	if (dwError) 
		goto exit;
	BrUser_delay(NULL, 200);
	
	mxl_error_code = MxLWare_EAGLE_API_CfgDrvInit(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &(DC->it9300), DC->MxL_EAGLE_Devices[tsSrcIdx].deviceType, DC->it9300.tsSource[br_idx][tsSrcIdx].i2cBus, DC->it9300.tsSource[br_idx][tsSrcIdx].i2cAddr);
	if (mxl_error_code) {
		deb_data("MxLWare_EAGLE_API_CfgDrvInit fail! 0x%04x", mxl_error_code);
		goto exit;
	}
	
	mxl_error_code = MxLWare_EAGLE_API_CfgDevReset(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]));
	if (mxl_error_code) {
		deb_data("MxLWare_EAGLE_API_CfgDevReset fail! 0x%04x", mxl_error_code);
		goto exit;
	}
	
	devXtalStruct.xtalCap = 26;
	devXtalStruct.clkOutEnable = MXL_E_DISABLE;
	devXtalStruct.clkOutDivEnable = MXL_E_DISABLE;
	devXtalStruct.xtalSharingEnable = MXL_E_DISABLE;
	devXtalStruct.xtalCalibrationEnable = MXL_E_DISABLE;
	
	mxl_error_code = MxLWare_EAGLE_API_CfgDevPowerRegulators(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), MXL_EAGLE_POWER_SUPPLY_SOURCE_SINGLE);
	if (mxl_error_code) 
		goto exit;
	mxl_error_code = MxLWare_EAGLE_API_CfgDevFwDownload(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), mxl_eagle_firmware_rawData, mxl_eagle_firmware_length, NULL);
	if (mxl_error_code) 
		goto exit;
	mxl_error_code = MxLWare_EAGLE_API_CfgDevXtal(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &devXtalStruct);
	if (mxl_error_code) 
		goto exit;
	
	DRV_MXL69X_DEBUG("MxLWare_EAGLE_API_CfgDevXtal done\n");

	BrUser_delay(NULL, 1500);

	mxl_error_code = MxLWare_EAGLE_API_ReqDevVersionInfo(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &DevVerStruct);
	if (mxl_error_code) 
		goto exit;
	
	DRV_MXL69X_DEBUG("MxLWare_EAGLE_API_ReqDevVersionInfo done\n");
	
	if(DC->MxL_EAGLE_Devices[tsSrcIdx].deviceType != MXL_EAGLE_DEVICE_248) { // MXL_EAGLE_DEVICE_248 do not support ATSC
		mxl_error_code = MxLWare_EAGLE_API_CfgDevDemodType(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), MXL_EAGLE_DEMOD_TYPE_ATSC);
		if (mxl_error_code) 
			goto exit;
	} else {
		mxl_error_code = MxLWare_EAGLE_API_CfgDevDemodType(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), MXL_EAGLE_DEMOD_TYPE_QAM);
		if (mxl_error_code) 
			goto exit;
	}
	mxl_error_code = MxLWare_EAGLE_API_CfgDevPowerMode(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), MXL_EAGLE_POWER_MODE_ACTIVE);
	if (mxl_error_code) 
		goto exit;
		
	BrUser_delay(NULL, 1500);

	mxl_error_code = MxLWare_EAGLE_API_ReqDevStatus(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &StatusStruct);
	if (mxl_error_code) 
		goto exit;
		
	DRV_MXL69X_DEBUG("MxLWare_EAGLE_API_ReqDevStatus done\n");

	mpegOutParamStruct.mpegIsParallel = MXL_E_DISABLE;
	mpegOutParamStruct.lsbOrMsbFirst = MXL_EAGLE_DATA_SERIAL_MSB_1ST;
	mpegOutParamStruct.mpegSyncPulseWidth = MXL_EAGLE_DATA_SYNC_WIDTH_BYTE;
	mpegOutParamStruct.mpegValidPol = MXL_EAGLE_CLOCK_POSITIVE;
	mpegOutParamStruct.mpegSyncPol = MXL_EAGLE_CLOCK_POSITIVE;
	mpegOutParamStruct.mpegClkPol = MXL_EAGLE_CLOCK_NEGATIVE;
	mpegOutParamStruct.mpeg3WireModeEnable = MXL_E_DISABLE;
	mpegOutParamStruct.mpegClkFreq = MXL_EAGLE_MPEG_CLOCK_54MHz;
	mpegOutParamStruct.mpegPadDrv.padDrvMpegSyn = MXL_EAGLE_IO_MUX_DRIVE_MODE_1X;
	mpegOutParamStruct.mpegPadDrv.padDrvMpegDat = MXL_EAGLE_IO_MUX_DRIVE_MODE_1X;
	mpegOutParamStruct.mpegPadDrv.padDrvMpegVal = MXL_EAGLE_IO_MUX_DRIVE_MODE_1X;
	mpegOutParamStruct.mpegPadDrv.padDrvMpegClk = MXL_EAGLE_IO_MUX_DRIVE_MODE_1X;
	mxl_error_code = MxLWare_EAGLE_API_CfgDevMpegOutParams(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &mpegOutParamStruct);
	if (mxl_error_code) {
		deb_data("MxLWare_EAGLE_API_CfgDevMpegOutParams fail! 0x%04x", mxl_error_code);
		goto exit;
	}

	DRV_MXL69X_DEBUG("[MXL69X] init success\n");
	return 0;

exit:
	deb_data("[MXL69X] init fail\n");
	return dwError;
}

Dword DRV_MXL69X_acquireChannel(Device_Context *DC, AcquireChannelRequest *request, int br_idx, int tsSrcIdx)
{
	MXL_EAGLE_TUNER_CHANNEL_PARAMS_T 	channelTuneStruct;
	MXL_EAGLE_ATSC_DEMOD_STATUS_T 		AtscStatStruct;
	MXL_EAGLE_QAM_DEMOD_PARAMS_T		QamParamsStruct;
	MXL_STATUS_E mxl_error_code;
	
	bool bLock = false;
	
	DC->MxL_EAGLE_Devices[tsSrcIdx].mode = request->mode;

	if (request->mode == TV_STANDARD_ATSC) {
		mxl_error_code = MxLWare_EAGLE_API_CfgDevDemodType(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), MXL_EAGLE_DEMOD_TYPE_ATSC);
		if (mxl_error_code) 
			goto exit;

		mxl_error_code = MxLWare_EAGLE_API_CfgDevMpegOutParams(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &mpegOutParamStruct);
		if (mxl_error_code) 
			goto exit;
		
		channelTuneStruct.freqInHz = request->frequency * 1000;
		channelTuneStruct.tuneMode = MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_VIEW; // MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_SCAN
		channelTuneStruct.bandWidth = MXL_EAGLE_TUNER_BW_8MHz; // MXL_EAGLE_TUNER_BW_6MHz, MXL_EAGLE_TUNER_BW_7MHz, MXL_EAGLE_TUNER_BW_8MHz
		mxl_error_code = MxLWare_EAGLE_API_CfgTunerChannelTune(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &channelTuneStruct);
		if (mxl_error_code) {
			deb_data("MxLWare_EAGLE_API_CfgTunerChannelTune fail!\n");
			goto exit;
		}

		BrUser_delay(NULL, 1000);

		mxl_error_code = MxLWare_EAGLE_API_CfgAtscDemodInit(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]));
		if (mxl_error_code) {
			deb_data("MxLWare_EAGLE_API_CfgAtscDemodInit fail!\n");
			goto exit;
		}
		
		mxl_error_code = MxLWare_EAGLE_API_CfgAtscDemodAcquireCarrier(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]));
		if (mxl_error_code) {
			deb_data("MxLWare_EAGLE_API_CfgAtscDemodAcquireCarrier fail!\n");
			goto exit;
		}

		BrUser_delay(NULL, 1000);
		
		mxl_error_code = MxLWare_EAGLE_API_ReqAtscDemodStatus(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &AtscStatStruct);
		if (mxl_error_code) {
			deb_data("MxLWare_EAGLE_API_ReqAtscDemodStatus wrong!!!!!!!!! (%d)\n", mxl_error_code);
			//goto exit;
		} else {
			if (AtscStatStruct.isFrameLock)
				bLock = true;
			else
				bLock = false;
		}
	} else if (request->mode == TV_STANDARD_QAMB || request->mode == TV_STANDARD_DVBC) {
		mxl_error_code = MxLWare_EAGLE_API_CfgDevDemodType(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), MXL_EAGLE_DEMOD_TYPE_QAM);
		if (mxl_error_code) goto exit;

		mxl_error_code = MxLWare_EAGLE_API_CfgDevMpegOutParams(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &mpegOutParamStruct);
		if (mxl_error_code) goto exit;
		
		channelTuneStruct.freqInHz = request->frequency * 1000;
		channelTuneStruct.tuneMode = MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_VIEW; // MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_SCAN
		channelTuneStruct.bandWidth = MXL_EAGLE_TUNER_BW_8MHz; // MXL_EAGLE_TUNER_BW_6MHz, MXL_EAGLE_TUNER_BW_7MHz, MXL_EAGLE_TUNER_BW_8MHz
		mxl_error_code = MxLWare_EAGLE_API_CfgTunerChannelTune(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &channelTuneStruct);
		if (mxl_error_code) {
			deb_data("MxLWare_EAGLE_API_CfgTunerChannelTune fail!\n");
			goto exit;
		}

		BrUser_delay(NULL, 1000);
		
		memset((UINT8 *)&QamParamsStruct, 0, sizeof(QamParamsStruct));
		if (request->mode == TV_STANDARD_QAMB)
			QamParamsStruct.annexType = MXL_EAGLE_QAM_DEMOD_ANNEX_B;	//J.83B 
		else if (request->mode == TV_STANDARD_DVBC)
			QamParamsStruct.annexType = MXL_EAGLE_QAM_DEMOD_ANNEX_A;	//DVB-C
		QamParamsStruct.qamType = MXL_EAGLE_QAM_DEMOD_AUTO; // MXL_EAGLE_QAM_DEMOD_QAM64, MXL_EAGLE_QAM_DEMOD_QAM256, MXL_EAGLE_QAM_DEMOD_QPSK
		QamParamsStruct.iqFlip = MXL_EAGLE_DEMOD_IQ_AUTO; // MXL_EAGLE_DEMOD_IQ_NORMAL, MXL_EAGLE_DEMOD_IQ_FLIPPED
		QamParamsStruct.searchRangeIdx = 6; //!< Equalizer frequency search range. Accepted range is 0..15. The search range depends on the current symbol rate, see documentation
		QamParamsStruct.spurCancellerEnable = MXL_E_ENABLE;
		QamParamsStruct.symbolRateHz = request->symbolrate*1000; //5309734; // 5056941; //!< For any QAM type in Annex-A mode, For QAM64 in Annex-B mode. Range = [2.0MHz?.2MHz]
		QamParamsStruct.symbolRate256QamHz = QamParamsStruct.symbolRateHz; //!< Symbol rate for QAM256 in Annex-B mode. In Annex-A, this should be the same as symbRateInHz. Range as above.		
		mxl_error_code = MxLWare_EAGLE_API_CfgQamDemodParams(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &QamParamsStruct);
		if (mxl_error_code) {
			deb_data("MxLWare_EAGLE_API_CfgQamDemodParams fail!\n");
			goto exit;
		}
		mxl_error_code = MxLWare_EAGLE_API_CfgQamDemodRestart(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]));
		if (mxl_error_code) {
			deb_data("MxLWare_EAGLE_API_CfgQamDemodRestart fail!\n");
			goto exit;
		}
		BrUser_delay(NULL, 140);
	}
exit:
	return 0;
}

Byte DRV_MXL69X_normalization_rf_power(int32_t rfLeveldB)
{
	if (rfLeveldB < RF_POWER_MIN_BOUNDARY) 
		return 0;
	if (rfLeveldB > RF_POWER_MAX_BOUNDARY) 
		return 100;
		
	return (Byte)(((rfLeveldB-(RF_POWER_MIN_BOUNDARY))*100)/(RF_POWER_MAX_BOUNDARY - RF_POWER_MIN_BOUNDARY));
}

Byte DRV_MXL69X_normalization_snr(int32_t snr)
{
	if (snr < SNR_MIN_BOUNDARY) 
		return 0;
	if (snr > SNR_MAX_BOUNDARY) 
		return 100;
	
	return (Byte)(((snr-(SNR_MIN_BOUNDARY))*100)/(SNR_MAX_BOUNDARY - SNR_MIN_BOUNDARY));
}

Dword DRV_MXL69X_getStatistic(Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int tsSrcIdx)
{
	MXL_STATUS_E mxl_error_code = 0;
	MXL_EAGLE_ATSC_DEMOD_STATUS_T	AtscStatStruct;
	MXL_EAGLE_QAM_DEMOD_STATUS_T	QamStatusStruct;
	MXL_EAGLE_TUNER_AGC_STATUS_T	TunerAgcStatusStruct;
	Byte mode = 0; 
	
	getstatisticrequest->statistic.signalLocked = 0;
	getstatisticrequest->statistic.signalPresented = 0;
	getstatisticrequest->statistic.signalQuality = 0;
	getstatisticrequest->statistic.signalStrength = 0;
	getstatisticrequest->snr = 0;
	getstatisticrequest->rf_power = 0;
		
	mode = DC->MxL_EAGLE_Devices[tsSrcIdx].mode;
	
	if (mode == TV_STANDARD_ATSC) {
		mxl_error_code = MxLWare_EAGLE_API_ReqAtscDemodStatus(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &AtscStatStruct);
		if (mxl_error_code) {
			deb_data("MxLWare_EAGLE_API_ReqAtscDemodStatus wrong!!!!!!!!! (%d)\n", mxl_error_code);
			goto exit;
		} else {
			DRV_MXL69X_DEBUG("SNR [dB]: %d \n", (AtscStatStruct.snrDbTenths / 10));
			DRV_MXL69X_DEBUG("Carrier offset [Hz]: %i \n", AtscStatStruct.carrierOffsetHz);
			DRV_MXL69X_DEBUG("Frame Lock: %d \n", AtscStatStruct.isFrameLock);
			DRV_MXL69X_DEBUG("ATSC Lock: %d \n", AtscStatStruct.isAtscLock);
			DRV_MXL69X_DEBUG("FEC Lock: %d \n", AtscStatStruct.isFecLock);
		}

		BrUser_delay(NULL, 100);

		mxl_error_code = MxLWare_EAGLE_API_ReqTunerAgcStatus(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &TunerAgcStatusStruct);
		if (mxl_error_code) {
			deb_data("MxLWare_EAGLE_API_ReqTunerAgcStatus wrong!!!!!!!!! (%d)\n", mxl_error_code);
			goto exit;
		} else {
			DRV_MXL69X_DEBUG("AGC Lock: %d \n", TunerAgcStatusStruct.isLocked);
			DRV_MXL69X_DEBUG("AGC Raw Gain: %d \n", TunerAgcStatusStruct.rawAgcGain);
			DRV_MXL69X_DEBUG("Rx Power: %d \n", TunerAgcStatusStruct.rxPowerDbHundredths);
		}

		BrUser_delay(NULL, 100);
		
		getstatisticrequest->statistic.signalPresented = (AtscStatStruct.isFrameLock)? True:False;
		getstatisticrequest->statistic.signalLocked = (AtscStatStruct.isAtscLock)? True:False;
		getstatisticrequest->statistic.signalQuality = DRV_MXL69X_normalization_snr(AtscStatStruct.snrDbTenths);
		getstatisticrequest->statistic.signalStrength = DRV_MXL69X_normalization_rf_power(TunerAgcStatusStruct.rxPowerDbHundredths);
		getstatisticrequest->snr = (AtscStatStruct.snrDbTenths / 10);
		getstatisticrequest->rf_power = TunerAgcStatusStruct.rxPowerDbHundredths/100;
	} else if(mode == TV_STANDARD_QAMB || mode == TV_STANDARD_DVBC) {
		mxl_error_code = MxLWare_EAGLE_API_ReqQamDemodStatus(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &QamStatusStruct);
		if (mxl_error_code) {
			deb_data("MxLWare_EAGLE_API_ReqQamDemodStatus wrong!!!!!!!!! (%d)\n", mxl_error_code);
			goto exit;
		} else {
			DRV_MXL69X_DEBUG("SNR [dB]: %d \n", (QamStatusStruct.snrDbTenth / 10));
			DRV_MXL69X_DEBUG("Carrier offset [Hz]: %i \n", QamStatusStruct.carrierOffsetHz);
			DRV_MXL69X_DEBUG("Mpeg Lock: %d \n", QamStatusStruct.isMpegLocked);
			DRV_MXL69X_DEBUG("QAM Lock: %d \n", QamStatusStruct.isQamLocked);
			DRV_MXL69X_DEBUG("FEC Lock: %d \n", QamStatusStruct.isFecLocked);
		}
		
		BrUser_delay(NULL, 100);

		mxl_error_code = MxLWare_EAGLE_API_ReqTunerAgcStatus(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &TunerAgcStatusStruct);
		if (mxl_error_code) {
			deb_data("MxLWare_EAGLE_API_ReqTunerAgcStatus wrong!!!!!!!!! (%d)\n", mxl_error_code);
			goto exit;
		} else {
			DRV_MXL69X_DEBUG("AGC Lock: %d \n", TunerAgcStatusStruct.isLocked);
			DRV_MXL69X_DEBUG("AGC Raw Gain: %d \n", TunerAgcStatusStruct.rawAgcGain);
			DRV_MXL69X_DEBUG("Rx Power: %d \n", TunerAgcStatusStruct.rxPowerDbHundredths);
		}
		
		BrUser_delay(NULL, 100);
		
		getstatisticrequest->statistic.signalPresented = (AtscStatStruct.isFrameLock)? True:False;
		getstatisticrequest->statistic.signalLocked = (AtscStatStruct.isAtscLock)? True:False;
		getstatisticrequest->statistic.signalQuality = DRV_MXL69X_normalization_snr(AtscStatStruct.snrDbTenths);
		getstatisticrequest->statistic.signalStrength = DRV_MXL69X_normalization_rf_power(TunerAgcStatusStruct.rxPowerDbHundredths);
		getstatisticrequest->snr = (QamStatusStruct.snrDbTenth / 10);
		getstatisticrequest->rf_power = TunerAgcStatusStruct.rxPowerDbHundredths/100;
	}
exit:
	return mxl_error_code;
}

Dword DRV_MXL69X_getChannelStatistic (Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int tsSrcIdx)
{
	Dword error = Error_NO_ERROR;
	
	channelStatistic->abortCount = 0;
	channelStatistic->postVitBitCount = 0;
	channelStatistic->postVitErrorCount = 0;
	channelStatistic->softBitCount = 0;
	channelStatistic->softErrorCount = 0;
	channelStatistic->preVitBitCount = 0;
	channelStatistic->preVitErrorCount = 0;
	
	return error;
}

Dword DRV_MXL69X_isLocked(Device_Context *DC, Bool* locked, int br_idx, int tsSrcIdx)
{
	MXL_STATUS_E mxl_error_code = 0;
	MXL_EAGLE_ATSC_DEMOD_STATUS_T AtscStatStruct;
	MXL_EAGLE_QAM_DEMOD_STATUS_T QamStatusStruct;
	int i = 0;
	Byte mode = 0;
	
	*locked = False;
	mode = DC->MxL_EAGLE_Devices[tsSrcIdx].mode;
	
	if (mode == TV_STANDARD_ATSC) {
		for (i = 0; i < 5; i++) {
			mxl_error_code = MxLWare_EAGLE_API_ReqAtscDemodStatus(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &AtscStatStruct);
			if (mxl_error_code) {
				deb_data("MxLWare_EAGLE_API_ReqAtscDemodStatus wrong!!!!!!!!! (%d)\n", mxl_error_code);
			} else {
				DRV_MXL69X_DEBUG("SNR [dB]: %d \n", (AtscStatStruct.snrDbTenths / 10));
				DRV_MXL69X_DEBUG("Carrier offset [Hz]: %i \n", AtscStatStruct.carrierOffsetHz);
				DRV_MXL69X_DEBUG("Frame Lock: %d \n", AtscStatStruct.isFrameLock);
				DRV_MXL69X_DEBUG("ATSC Lock: %d \n", AtscStatStruct.isAtscLock);
				DRV_MXL69X_DEBUG("FEC Lock: %d \n", AtscStatStruct.isFecLock);
				
				*locked = AtscStatStruct.isAtscLock;
				if (*locked)
					break;
			}
			BrUser_delay(NULL, 1000);
		}
	} else if (mode == TV_STANDARD_QAMB || mode == TV_STANDARD_DVBC) {
		for (i = 0; i < 5; i++) {
			mxl_error_code = MxLWare_EAGLE_API_ReqQamDemodStatus(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]), &QamStatusStruct);
			if (mxl_error_code) {
				deb_data("MxLWare_EAGLE_API_ReqQamDemodStatus wrong!!!!!!!!! (%d)\n", mxl_error_code);
			} else {
				DRV_MXL69X_DEBUG("SNR [dB]: %d \n", (QamStatusStruct.snrDbTenth / 10));
				DRV_MXL69X_DEBUG("Carrier offset [Hz]: %i \n", QamStatusStruct.carrierOffsetHz);
				DRV_MXL69X_DEBUG("Frame Lock: %d \n", QamStatusStruct.isMpegLocked);
				DRV_MXL69X_DEBUG("QAM Lock: %d \n", QamStatusStruct.isQamLocked);
				DRV_MXL69X_DEBUG("FEC Lock: %d \n", QamStatusStruct.isFecLocked);
				
				if (QamStatusStruct.isQamLocked == 1 && QamStatusStruct.isFecLocked == 1 && QamStatusStruct.isMpegLocked == 1) {
					*locked = True;
					break;	
				}				
			}
			BrUser_delay(NULL, 1000);
		}
	}	
	return mxl_error_code;
}

Dword DRV_MXL69X_DeInitialize(Device_Context *DC, int br_idx, int tsSrcIdx)
{
	Dword dwError = Error_NO_ERROR;
	
	DRV_MXL69X_DEBUG("DRV_MXL69X_DeInitialize - %d\n", tsSrcIdx);
	MxLWare_EAGLE_API_CfgDrvUnInit(tsSrcIdx, &(DC->MxL_EAGLE_Devices[tsSrcIdx]));

	return dwError;
}
