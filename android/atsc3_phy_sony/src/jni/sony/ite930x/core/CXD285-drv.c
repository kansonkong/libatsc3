#include "it930x-core.h"

#include "sony_cxd_apis.h"

void sony_cxd_dvbt_t2OutputChannelInformation(sony_cxd_integ_t * pInteg, uint32_t frequencyKHz, sony_cxd_dtv_system_t system, uint8_t dataPLPID, sony_cxd_dvbt2_profile_t t2Profile)
{
	sony_cxd_result_t	result;
	int32_t				snr = 0;
	uint8_t				sqi = 0;
	uint8_t				ssi = 0;
	int32_t				rfLevel;
	uint32_t			ber;


	// Callback contains channel information, so output channel information to table. 
	if (system == sony_cxd_DTV_SYSTEM_DVBT)
	{
		printk("DVB-T :\n");

		//Lock
		deb_data("start to wait ts lock\n");
		result = sony_cxd_integ_dvbt_WaitTSLock(pInteg);
		deb_data("end of wait ts lock\n");
		if (result == sony_cxd_RESULT_OK)
			printk("TS Lock\n");
		else
			printk("TS UnLock!!!\n");

		// Allow the monitors time to settle 
		//Sleep(1000);
		sony_cxd_SLEEP(1000);//msleep
		
		//SNR
		result = sony_cxd_demod_dvbt_monitor_SNR(pInteg->pDemod, &snr);
		if (result != sony_cxd_RESULT_OK)
		{
			snr = 0;
		}
		printk("SNR = %2u.%03u\n", snr / 1000, snr % 1000);

		//SQI
		result = sony_cxd_demod_dvbt_monitor_Quality(pInteg->pDemod, &sqi);
		if (result != sony_cxd_RESULT_OK)
		{
			sqi = 0;
		}
		printk("SQ = %3u\n", sqi);

		//SSI
		result = sony_cxd_integ_dvbt_monitor_SSI(pInteg, &ssi);
		if (result != sony_cxd_RESULT_OK)
		{
			ssi = 0;
		}
		printk("SS = %3u\n", ssi);
		result = sony_cxd_integ_dvbt_t2_monitor_RFLevel(pInteg, &rfLevel);
		if (result != sony_cxd_RESULT_OK)
		{
			printk("Error\n");
		}
		else
		{
			printk("RF Level = %d dBm\n", rfLevel / 1000);
		}

		//BER
		result = sony_cxd_demod_dvbt_monitor_PreRSBER(pInteg->pDemod, &ber);
		if (result != sony_cxd_RESULT_OK)
		{
			printk("Error\n");

		}
		else
		{
			printk("PreRSBER = %u x 10^-7\n", ber);
		}
	}
	else if (system == sony_cxd_DTV_SYSTEM_DVBT2)
	{
		printk("DVB-T2 :\n");

		//Lock
		result = sony_cxd_integ_dvbt2_WaitTSLock(pInteg, t2Profile);
		if (result == sony_cxd_RESULT_OK)
			printk("TS Lock\n");
		else
			printk("TS UnLock!!!\n");

		// Allow the monitors time to settle 
		//Sleep(1000);
		sony_cxd_SLEEP(1000);//msleep

		//SNR
		result = sony_cxd_demod_dvbt2_monitor_SNR(pInteg->pDemod, &snr);
		if (result != sony_cxd_RESULT_OK)
		{
			snr = 0;
		}
		printk("SNR = %2u.%03u\n", snr / 1000, snr % 1000);

		//SQI
		result = sony_cxd_demod_dvbt2_monitor_Quality(pInteg->pDemod, &sqi);
		if (result != sony_cxd_RESULT_OK)
		{
			sqi = 0;
		}
		printk("SQ = %3u\n", sqi);

		//SSI
		result = sony_cxd_integ_dvbt2_monitor_SSI(pInteg, &ssi);
		if (result != sony_cxd_RESULT_OK)
		{
			ssi = 0;
		}
		printk("SS = %3u\n", ssi);
		result = sony_cxd_integ_dvbt_t2_monitor_RFLevel(pInteg, &rfLevel);
		if (result != sony_cxd_RESULT_OK)
		{
			printk("Error\n");
		}
		else
		{
			printk("RF Level = %d dBm\n", rfLevel / 1000);
		}

		//BER
		result = sony_cxd_demod_dvbt2_monitor_PreBCHBER(pInteg->pDemod, &ber);
		if (result != sony_cxd_RESULT_OK)
		{
			printk("Error\n");

		}
		else
		{
			printk("PreBCHBER = %u x 10^-9\n", ber);
		}


		printk("PLPID = %3u, ", dataPLPID);
		if (t2Profile == sony_cxd_DVBT2_PROFILE_LITE)
			printk("Lite\n");
		else
			printk("Base\n");
	}
	else
	{
		// Never occured 
		printk("Unknown system.\n");
	}
}

Dword DRV_CXD285X_Initialize(Device_Context *DC, int br_idx, int tsSrcIdx, Byte ChipType)
{
	uint8_t	i2c_bus = 0, demod_i2cAdd = 0, tuner_i2cAdd = 0;
	sony_cxd_result_t						CXD285x_result = sony_cxd_RESULT_OK;
	sony_cxd_demod_create_param_t			CXD285x_createParam[4];
	Dword 									dwError = Error_NO_ERROR;
	GPIO_STRUCT 							GpioStruct[4] = { {0xD8CB, 0xD8CC, 0xD8CD},
															{0xD8D7, 0xD8D8, 0xD8D9},
															{0xD8D3, 0xD8D4, 0xD8D5},
															{0xD8DF, 0xD8E0, 0xD8E1} };
	
	deb_data("br_idx = %d, tsSrcIdx = %d, ChipType = %d\n", br_idx, tsSrcIdx, ChipType);
	
	//reset CXD285
	dwError = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, GpioStruct[tsSrcIdx].GpioOut, 0);
	if (dwError) goto exit;
	dwError = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, GpioStruct[tsSrcIdx].GpioEn, 1);
	if (dwError) goto exit;
	dwError = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, GpioStruct[tsSrcIdx].GpioOn, 1);
	if (dwError) goto exit;
	BrUser_delay(NULL, 200);
	dwError = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, GpioStruct[tsSrcIdx].GpioOut, 1);
	if (dwError) goto exit;
	BrUser_delay(NULL, 200);
	
	deb_data("reset CXD285 success\n");
	
	if (tsSrcIdx == 0)
	{
		i2c_bus = 2;
		demod_i2cAdd = 0xC8;
		tuner_i2cAdd = 0xC0;
	}
	else if (tsSrcIdx == 1)
	{
		i2c_bus = 2;
		demod_i2cAdd = 0xCA;
		tuner_i2cAdd = 0xC2;
	}
	else if (tsSrcIdx == 2)
	{
		i2c_bus = 3;
		demod_i2cAdd = 0xD8;
		tuner_i2cAdd = 0xC4;
	}
	else if (tsSrcIdx == 3)
	{
		i2c_bus = 3;
		demod_i2cAdd = 0xDA;
		tuner_i2cAdd = 0xC6;
	}


	drvi2c_cxd_ite_Initialize(&(DC->CXD285x_driver[tsSrcIdx].iteI2c), (Endeavour*)&DC->it9300, i2c_bus);
	drvi2c_cxd_ite_CreateI2c(&(DC->CXD285x_driver[tsSrcIdx].i2c), &(DC->CXD285x_driver[tsSrcIdx].iteI2c));
	CXD285x_createParam[tsSrcIdx].xtalFreq = sony_cxd_DEMOD_XTAL_24000KHz;
	CXD285x_createParam[tsSrcIdx].i2cAddressSLVT = demod_i2cAdd;
	CXD285x_createParam[tsSrcIdx].tunerI2cConfig = sony_cxd_DEMOD_TUNER_I2C_CONFIG_REPEATER;
	CXD285x_result = sony_cxd_tuner_helene_Create(&(DC->CXD285x_driver[tsSrcIdx].tunerTerrCable), &(DC->CXD285x_driver[tsSrcIdx].tunerSat), sony_cxd_HELENE_XTAL_16000KHz, tuner_i2cAdd, &(DC->CXD285x_driver[tsSrcIdx].i2c), 0, &(DC->CXD285x_driver[tsSrcIdx].helene));
	if (CXD285x_result != sony_cxd_RESULT_OK)
	{
		deb_data("sony_cxd_tuner_helene_Create failed.\n");
		goto exit;
	}
	CXD285x_result = sony_cxd_integ_Create(&(DC->CXD285x_driver[tsSrcIdx].integ), &(DC->CXD285x_driver[tsSrcIdx].demod), &CXD285x_createParam[tsSrcIdx], &(DC->CXD285x_driver[tsSrcIdx].i2c), &(DC->CXD285x_driver[tsSrcIdx].tunerTerrCable));
	if (CXD285x_result != sony_cxd_RESULT_OK)
	{
		deb_data("sony_cxd_integ_Create failed.\n");
		goto exit;
	}
	CXD285x_result = sony_cxd_integ_Initialize(&(DC->CXD285x_driver[tsSrcIdx].integ));
	if (CXD285x_result != sony_cxd_RESULT_OK)
	{
		deb_data("sony_cxd_integ_IDVB-T : Initialize failed.\n");
		goto exit;
	}

	CXD285x_result = sony_cxd_demod_SetConfig(&(DC->CXD285x_driver[tsSrcIdx].demod), sony_cxd_DEMOD_CONFIG_PARALLEL_SEL, 0);
	if (CXD285x_result != sony_cxd_RESULT_OK)
	{
		deb_data("sony_cxd_demod_SetConfig (sony_cxd_DEMOD_CONFIG_SAT_IFAGCNEG) failed.\n");
		goto exit;
	}
	CXD285x_result = sony_cxd_demod_SetConfig(&(DC->CXD285x_driver[tsSrcIdx].demod), sony_cxd_DEMOD_CONFIG_SER_DATA_ON_MSB, 0);
	if (CXD285x_result != sony_cxd_RESULT_OK)
	{
		deb_data("sony_cxd_demod_SetConfig (sony_cxd_DEMOD_CONFIG_SAT_IFAGCNEG) failed.\n");
		goto exit;
	}
	CXD285x_result = sony_cxd_demod_SetConfig(&(DC->CXD285x_driver[tsSrcIdx].demod), sony_cxd_DEMOD_CONFIG_TERR_CABLE_TS_SERIAL_CLK_FREQ, 3); //64MHz
	if (CXD285x_result != sony_cxd_RESULT_OK)
	{
		deb_data("sony_cxd_demod_SetConfig (sony_cxd_DEMOD_CONFIG_SAT_IFAGCNEG) failed.\n");
		goto exit;
	}

	deb_data("CXD285x chip %d init ok\n", tsSrcIdx);
	
	return 0;
exit:
	deb_data("something wrong!!!!!!!!!\n");	
	return CXD285x_result;
}

Dword DRV_CXD285X_acquireChannel(Device_Context *DC, AcquireChannelRequest *request, int br_idx, int tsSrcIdx)
{
	//for DVBT
	sony_cxd_dtv_bandwidth_t	bandwidth;
	sony_cxd_dtv_system_t		system;
	sony_cxd_dvbt2_profile_t	t2Profile;
	sony_cxd_dtv_system_t		systemTuned;
	sony_cxd_dvbt2_profile_t	t2ProfileTuned;
	uint8_t						tryAnotherT2Profile;
	sony_cxd_dvbt2_profile_t	anotherT2Profile;
	uint8_t						numPlps;
	uint8_t						plpIds[255];
	sony_cxd_dvbt2_ofdm_t		ofdmInfo;
	
	//for ISDBT
	sony_cxd_isdbt_tune_param_t	tuneParam;
	
	//
	uint32_t					tmp;
	uint8_t						i;
	sony_cxd_result_t			CXD285x_result = sony_cxd_RESULT_OK;
	
	if(request->mode == 3)//DVBT
	{
		system = sony_cxd_DTV_SYSTEM_ANY;
		t2Profile = sony_cxd_DVBT2_PROFILE_ANY;
		systemTuned = sony_cxd_DTV_SYSTEM_UNKNOWN;
		t2ProfileTuned = sony_cxd_DVBT2_PROFILE_ANY;
		tryAnotherT2Profile = 0;
		anotherT2Profile = sony_cxd_DVBT2_PROFILE_BASE;
		
		if (request->bandwidth == 5000)
		{
			bandwidth = sony_cxd_DTV_BW_5_MHZ;
		}
		else if (request->bandwidth ==  6000)
		{
			bandwidth = sony_cxd_DTV_BW_6_MHZ;
		}
		else if (request->bandwidth ==  7000)
		{
			bandwidth = sony_cxd_DTV_BW_7_MHZ;
		}
		else
		{
			bandwidth = sony_cxd_DTV_BW_8_MHZ;
		}
		
		
		deb_data("dvbt_t2_BlindTune!!!\n");
		CXD285x_result = sony_cxd_integ_dvbt_t2_BlindTune(&(DC->CXD285x_driver[tsSrcIdx].integ), request->frequency, bandwidth, system, t2Profile, &systemTuned, &t2ProfileTuned);
		deb_data("dvbt_t2_BlindTune!!!\n");
		
		if (CXD285x_result == sony_cxd_RESULT_OK)
		{
			if (systemTuned == sony_cxd_DTV_SYSTEM_DVBT)
			{
				// DVB-T signal found! 
				deb_data("DVB-T signal found!!!\n");
				sony_cxd_dvbt_t2OutputChannelInformation(&(DC->CXD285x_driver[tsSrcIdx].integ), request->frequency, sony_cxd_DTV_SYSTEM_DVBT, 0, sony_cxd_DVBT2_PROFILE_BASE);
			}
			else if (systemTuned == sony_cxd_DTV_SYSTEM_DVBT2)
			{
				// DVB-T2 signal found! 
				deb_data("DVB-T2 signal found!!!\n");
				numPlps = 0;
				CXD285x_result = sony_cxd_demod_dvbt2_monitor_OFDM(&(DC->CXD285x_driver[tsSrcIdx].demod), &ofdmInfo);
				if (CXD285x_result != sony_cxd_RESULT_OK)
				{
					// Failed to get OFDM info... 
					deb_data("No Signal !!!\n");
				}
				CXD285x_result = sony_cxd_demod_dvbt2_monitor_DataPLPs(&(DC->CXD285x_driver[tsSrcIdx].demod), plpIds, &numPlps);
				if (CXD285x_result != sony_cxd_RESULT_OK)
				{
					// Failed to get data PLP ID list... 
					deb_data("No Signal !!!\n");
				}
				// Output information for first PLP ID. (BlindTune select first ID of PLP ID list automatically.) 
				sony_cxd_dvbt_t2OutputChannelInformation(&(DC->CXD285x_driver[tsSrcIdx].integ), request->frequency, sony_cxd_DTV_SYSTEM_DVBT2, plpIds[0], t2ProfileTuned);
				// Try to switch other PLPs 
				for (i = 1; i < numPlps; i++)
				{
					CXD285x_result = sony_cxd_integ_dvbt2_Scan_SwitchDataPLP(&(DC->CXD285x_driver[tsSrcIdx].integ), ofdmInfo.mixed, plpIds[i], t2ProfileTuned);
					if (CXD285x_result == sony_cxd_RESULT_OK)
					{
						//Output information for second or later PLP IDs. 
						sony_cxd_dvbt_t2OutputChannelInformation(&(DC->CXD285x_driver[tsSrcIdx].integ), request->frequency, sony_cxd_DTV_SYSTEM_DVBT2, plpIds[i], t2ProfileTuned);
					}
					else
					{
						break;
					}
				}

				if (CXD285x_result != sony_cxd_RESULT_OK)
				{
					deb_data("Error !!!\n");
				}

				if (ofdmInfo.mixed)
				{
					// FEF is included. Another T2 profile may be exist. 
					tryAnotherT2Profile = 1;
					anotherT2Profile = (t2ProfileTuned == sony_cxd_DVBT2_PROFILE_BASE) ? sony_cxd_DVBT2_PROFILE_LITE : sony_cxd_DVBT2_PROFILE_BASE;
				}
			}
			else
			{
				deb_data("Other Error !!!\n");
			}
		}
		else if ((CXD285x_result == sony_cxd_RESULT_ERROR_TIMEOUT) || (CXD285x_result == sony_cxd_RESULT_ERROR_UNLOCK))
		{
			deb_data("UnLock !!!\n");
		}
		else
		{
			deb_data("Other Error !!!\n");
		}
		
		if (tryAnotherT2Profile)
		{
			// Another T2 profile may exist. 
			CXD285x_result = sony_cxd_integ_dvbt2_BlindTune(&(DC->CXD285x_driver[tsSrcIdx].integ), request->frequency, bandwidth, anotherT2Profile, &t2ProfileTuned);
			if (CXD285x_result == sony_cxd_RESULT_OK)
			{
				numPlps = 0;
				CXD285x_result = sony_cxd_demod_dvbt2_monitor_OFDM(&(DC->CXD285x_driver[tsSrcIdx].demod), &ofdmInfo);
				if (CXD285x_result != sony_cxd_RESULT_OK)
				{
					deb_data("Error !!!\n");
				}
				CXD285x_result = sony_cxd_demod_dvbt2_monitor_DataPLPs(&(DC->CXD285x_driver[tsSrcIdx].demod), plpIds, &numPlps);
				if (CXD285x_result != sony_cxd_RESULT_OK)
				{
					deb_data("Error !!!\n");
				}
				// Output information for first PLP ID. (BlindTune select first ID of PLP ID list automatically.) 
				sony_cxd_dvbt_t2OutputChannelInformation(&(DC->CXD285x_driver[tsSrcIdx].integ), request->frequency, sony_cxd_DTV_SYSTEM_DVBT2, plpIds[0], t2ProfileTuned);
				// Try to switch other PLPs 
				for (i = 1; i < numPlps; i++)
				{
					CXD285x_result = sony_cxd_integ_dvbt2_Scan_SwitchDataPLP(&(DC->CXD285x_driver[tsSrcIdx].integ), ofdmInfo.mixed, plpIds[i], t2ProfileTuned);
					if (CXD285x_result == sony_cxd_RESULT_OK)
					{
						// Output information for second or later PLP IDs. 
						sony_cxd_dvbt_t2OutputChannelInformation(&(DC->CXD285x_driver[tsSrcIdx].integ), request->frequency, sony_cxd_DTV_SYSTEM_DVBT2, plpIds[i], t2ProfileTuned);
					}
					else
					{
						break;
					}
				}

				if (CXD285x_result != sony_cxd_RESULT_OK)
				{
					deb_data("Other Error !!!\n");
				}
			}
			else if ((CXD285x_result == sony_cxd_RESULT_ERROR_TIMEOUT) || (CXD285x_result == sony_cxd_RESULT_ERROR_UNLOCK))
			{
				deb_data("UnLock !!!\n");
			}
			else
			{
				deb_data("Other Error !!!\n");
			}
		}
	}
	else if(request->mode == 4)//ISDBT
	{
		tuneParam.centerFreqKHz = request->frequency;			// Channel centre frequency in KHz 
		tuneParam.bandwidth = sony_cxd_DTV_BW_6_MHZ;	// Channel bandwidth 
		CXD285x_result = sony_cxd_integ_isdbt_Tune(&(DC->CXD285x_driver[tsSrcIdx].integ), &tuneParam);
		if (CXD285x_result != sony_cxd_RESULT_OK)
		{
			deb_data("sony_cxd_integ_isdbt_Tune failed!!!\n");			
		}
	}	
	return CXD285x_result;
}

Dword DRV_CXD285X_getStatistic (Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int tsSrcIdx)
{
	sony_cxd_result_t				CXD285x_result;
	sony_cxd_demod_lock_result_t	isLock;
	int32_t							snr = 0;
	int32_t 							rflevel = 0;
	uint8_t							sqi = 0;
	uint8_t							ssi = 0;
	uint8_t							dmdLock = 0;
	uint8_t							syncState = 0;
	uint8_t							tsLock = 0;
	uint8_t							earlyUnlock = 0;
	
	getstatisticrequest->statistic.signalLocked = 0;
	getstatisticrequest->statistic.signalPresented = 0;
	getstatisticrequest->statistic.signalQuality = 0;
	getstatisticrequest->statistic.signalStrength = 0;
	getstatisticrequest->snr = 0;
	getstatisticrequest->rf_power = 0;
	
	if (DC->CXD285x_driver[tsSrcIdx].demod.system == sony_cxd_DTV_SYSTEM_DVBT)
	{
		//Demod Lock
		CXD285x_result = sony_cxd_demod_dvbt_CheckDemodLock(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &isLock);
		if (CXD285x_result == sony_cxd_RESULT_OK && isLock == sony_cxd_DEMOD_LOCK_RESULT_LOCKED)
		{
			deb_data("DVB-T Demod Lock\n");
			getstatisticrequest->statistic.signalLocked = True;
		}			
		else
			deb_data("DVB-T Demod UnLock!!!\n");
		
		//TS Lock
		CXD285x_result = sony_cxd_demod_dvbt_CheckTSLock(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &isLock);
		if (CXD285x_result == sony_cxd_RESULT_OK && isLock == sony_cxd_DEMOD_LOCK_RESULT_LOCKED)
		{
			deb_data("DVB-T TS Lock\n");
			getstatisticrequest->statistic.signalPresented = True;
		}			
		else
			deb_data("TS UnLock!!!\n");
		
		//SQI
		CXD285x_result = sony_cxd_demod_dvbt_monitor_Quality(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &sqi);
		if (CXD285x_result != sony_cxd_RESULT_OK)
		{
			sqi = 0;
		}
		deb_data("DVB-T SQ = %3u\n", sqi);
		getstatisticrequest->statistic.signalQuality = (Dword)sqi;
		
		//SS
		CXD285x_result = sony_cxd_integ_dvbt_monitor_SSI(&(DC->CXD285x_driver[tsSrcIdx].integ), &ssi);
		if (CXD285x_result != sony_cxd_RESULT_OK)
		{
			ssi = 0;
		}
		deb_data("DVB-T SS = %3u\n", ssi);
		getstatisticrequest->statistic.signalStrength = (Dword)ssi;
	}	
	else if (DC->CXD285x_driver[tsSrcIdx].demod.system == sony_cxd_DTV_SYSTEM_DVBT2)
	{
		//Demod Lock
		CXD285x_result = sony_cxd_demod_dvbt2_CheckDemodLock(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &isLock);
		if (CXD285x_result == sony_cxd_RESULT_OK && isLock == sony_cxd_DEMOD_LOCK_RESULT_LOCKED)
		{
			deb_data("DVB-T2 Demod Lock\n");
			getstatisticrequest->statistic.signalLocked = True;
		}			
		else
			deb_data("DVB-T2 Demod UnLock!!!\n");
		//TS Lock
		CXD285x_result = sony_cxd_demod_dvbt2_CheckTSLock(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &isLock);
		if (CXD285x_result == sony_cxd_RESULT_OK && isLock == sony_cxd_DEMOD_LOCK_RESULT_LOCKED)
		{
			deb_data("DVB-T2 TS Lock\n");
			getstatisticrequest->statistic.signalPresented = True;
		}	
		else
			deb_data("DVB-T2 TS UnLock!!!\n");
			
		//SQI
		CXD285x_result = sony_cxd_demod_dvbt2_monitor_Quality(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &sqi);
		if (CXD285x_result != sony_cxd_RESULT_OK)
		{
			sqi = 0;
		}
		deb_data("DVB-T2 SQ = %3u\n", sqi);
		getstatisticrequest->statistic.signalQuality = (Dword)sqi;
		
		//SS
		CXD285x_result = sony_cxd_integ_dvbt2_monitor_SSI(&(DC->CXD285x_driver[tsSrcIdx].integ), &ssi);
		if (CXD285x_result != sony_cxd_RESULT_OK)
		{
			ssi = 0;
		}
		deb_data("DVB-T SS = %3u\n", ssi);
		getstatisticrequest->statistic.signalStrength = (Dword)ssi;
	}
	else if (DC->CXD285x_driver[tsSrcIdx].demod.system == sony_cxd_DTV_SYSTEM_ISDBT)
	{
		//Demod and TS Lock
		CXD285x_result = sony_cxd_demod_isdbt_monitor_SyncStat(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &dmdLock, &tsLock, &earlyUnlock);
		if (CXD285x_result == sony_cxd_RESULT_OK && dmdLock == 1)
		{
			deb_data("ISDBT Demod Lock\n");
			getstatisticrequest->statistic.signalLocked = True;
		}
		else
			deb_data("ISDBT Demod UnLock!!!\n");
		if (CXD285x_result == sony_cxd_RESULT_OK && tsLock == 1)
		{
			deb_data("ISDBT TS Lock\n");
			getstatisticrequest->statistic.signalPresented = True;
		}
		else
			deb_data("ISDBT TS UnLock!!!\n");
			
		//SNR
		CXD285x_result = sony_cxd_demod_isdbt_monitor_SNR(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &snr);
		if (CXD285x_result != sony_cxd_RESULT_OK)
		{
			snr = 0;
		}
		deb_data("ISDBT SQ(SNR) = %2u.%3u\n", snr / 1000, snr % 1000);
		snr = snr /1000;
		getstatisticrequest->statistic.signalQuality = (Byte)snr;
		getstatisticrequest->snr = (int)snr;
		deb_data("ISDBT SQ(SNR) = %u\n", getstatisticrequest->statistic.signalQuality);
		
		
		//SS
		CXD285x_result = sony_cxd_integ_isdbt_monitor_RFLevel(&(DC->CXD285x_driver[tsSrcIdx].integ), &rflevel);
		if (CXD285x_result != sony_cxd_RESULT_OK)
		{
			ssi = 0;
		}
		deb_data("ISDBT RF Level = %d dbm\n", rflevel / 1000);
		getstatisticrequest->statistic.signalStrength = (Byte)(rflevel / 1000);	
		getstatisticrequest->statistic.signalStrength = getstatisticrequest->statistic.signalStrength * 100 / 256;
		deb_data("ISDBT RF Level = %d dbm\n", getstatisticrequest->statistic.signalStrength);
			
	}
	return 0;	
}

Dword DRV_CXD285X_isLocked(Device_Context *DC, Bool* locked, int br_idx, int tsSrcIdx)
{
	sony_cxd_result_t				CXD285x_result = sony_cxd_RESULT_OK;
	sony_cxd_demod_lock_result_t	isDemodLock;
	sony_cxd_demod_lock_result_t	isTSLock;
	
	uint8_t		dmdLock = 0;
	uint8_t		syncState = 0;
	uint8_t		tsLock = 0;
	uint8_t		earlyUnlock = 0;
	
	*locked = False;
	
	if (DC->CXD285x_driver[tsSrcIdx].demod.system == sony_cxd_DTV_SYSTEM_DVBT)
	{
		//Demod Lock
		CXD285x_result = sony_cxd_demod_dvbt_CheckDemodLock(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &isDemodLock);
		if (CXD285x_result != sony_cxd_RESULT_OK || isDemodLock != sony_cxd_DEMOD_LOCK_RESULT_LOCKED)
		{
			deb_data("DVB-T Demod UnLock\n");
			goto exit;
		}			
		//TS Lock
		CXD285x_result = sony_cxd_demod_dvbt_CheckTSLock(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &isTSLock);
		if (CXD285x_result == sony_cxd_RESULT_OK && isTSLock == sony_cxd_DEMOD_LOCK_RESULT_LOCKED)
		{
			deb_data("DVB-T Demod and TS is Locked\n");
			*locked = True;
		}			
		else
		{
			deb_data("DVB-T TS UnLocked!!!\n");
			goto exit;
		}
	}
	else if (DC->CXD285x_driver[tsSrcIdx].demod.system == sony_cxd_DTV_SYSTEM_DVBT2)
	{
		//Demod Lock
		CXD285x_result = sony_cxd_demod_dvbt2_CheckDemodLock(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &isDemodLock);
		if (CXD285x_result != sony_cxd_RESULT_OK || isDemodLock != sony_cxd_DEMOD_LOCK_RESULT_LOCKED)
		{
			deb_data("DVB-T2 Demod UnLocked\n");
			goto exit;
		}			
		//TS Lock
		CXD285x_result = sony_cxd_demod_dvbt2_CheckTSLock(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &isTSLock);
		if (CXD285x_result == sony_cxd_RESULT_OK && isTSLock == sony_cxd_DEMOD_LOCK_RESULT_LOCKED)
		{
			deb_data("DVB-T2 Demod and TS is Locked\n");
			*locked = True;
		}			
		else
		{
			deb_data("DVB-T2 TS UnLocked!!!\n");
			goto exit;
		}
	}
	else if (DC->CXD285x_driver[tsSrcIdx].demod.system == sony_cxd_DTV_SYSTEM_ISDBT)
	{
		CXD285x_result = sony_cxd_demod_isdbt_monitor_SyncStat(DC->CXD285x_driver[tsSrcIdx].integ.pDemod, &dmdLock, &tsLock, &earlyUnlock);
		if (CXD285x_result == sony_cxd_RESULT_OK && dmdLock == 1 && tsLock == 1)
		{
			deb_data("TS Lock\n");
			*locked = True;
		}
		else
			deb_data("TS UnLock!!!\n");
	}
exit:
	return CXD285x_result;
}
