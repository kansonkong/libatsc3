/*****************************************************************************/
/*  Saankhya Confidential                                                    */
/*  COPYRIGHT (C) 2020 Saankhya Labs Pvt. Ltd - All Rights Reserved          */
/*                                                                           */
/* Saankhya Labs makes no warranty express or implied including but not      */
/* limited to, any warranty of merchantability or fitness for a particular   */
/* purpose and/or requirements, for a particular purpose in relation to the  */
/* LICENSED MATERIALS, which is provided "AS IS", WITH ALL FAULTS.           */
/*                                                                           */
/* Saankhya Labs does not represent or warrant that the LICENSED MATERIALS   */
/* provided here under is free of infringement of any third party patents,   */
/* copyrights, trade secrets or other intellectual property rights.          */
/* ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE EXCLUDED TO  */
/* THE FULLEST EXTENT PERMITTED BY LAW                                       */
/* NOTICE: All information contained herein is, and remains the property of  */
/* Saankhya Labs Pvt. Ltd and its suppliers, if any. The intellectual and    */
/* technical concepts contained herein are proprietary to Saankhya Labs & its*/
/* suppliers and may be covered by U.S. and Foreign Patents, patents in      */
/* process, and are protected by trade secret or copyright law. Dissemination*/
/* of this information or reproduction of this material is strictly forbidden*/
/* unless prior written permission is obtained from Saankhya Labs Pvt Ltd.   */
/*                                                                           */
/*  File Name   :   atsc1.c                                                  */
/*  version     :   0.4                                                      */
/*  Date        :   25/03/2022        \                                      */
/*  Description :   Atsc1 test code Implementation                           */
/*                                                                           */
/*****************************************************************************/

#include "atsc1.h"
//#include "CircularBuffer.h"
//#include "atsc1_diag.h"

/* Defines */
#define CB_SIZE              (16*1024*12*16)   // Global  circular buffer size
#define MAX_TS_BUFFER_SIZE   (1024*16)           // Max buffer size for TS transfer
#define IF_OFFSET            (0.000)             // User can Update as needed
#define BLOCK_SIZE           (10000)             // User can Update as needed
#define TUNER_FREQUENCY      (473000000)         // Frequency in HERTZ
#define TS_SYNC_BYTE         (0x47)
#define SDIO_TRANSFER_SIZE   (8192)
#define TS_PACKET_SIZE       (188)
#define PAD_START_BYTE       (0x00)
#define PAD_BYTES            (108)

/* Global Variables */
//CircularBuffer              cb;
unsigned long int           pThread, cThread, dThread;
void                        *fp;
int                         tUnit;
int                         slUnit;
unsigned char               buffer[MAX_TS_BUFFER_SIZE];
int                         processFlag;
int                         diagFlag;
unsigned int                tunerFrequency;
SL_DemodConfigInfo_t        cfgInfo;
SL_Atsc1p0ConfigParams_t    atsc1ConfigInfo;
SL_CmdControlIf_t           cmdIf;
SL_PlatFormConfigParams_t   getPlfConfig;
SL_TunerDcOffSet_t          tunerIQDcOffSet;
SL_Atsc1p0Perf_Diag_t       atsc1PerfDiag;
static volatile int         isCaptureThreadRunning = 0, isProcessThreadRunning = 0, isDiagThreadRunning = 0;
static int                  demodLockStatus = 0;
static bool                 tsSyncByteCheck = false;
static unsigned int         validBufferLength = 0;
static int                  resideuBufFlag = 0;
static char                 residueBuffer[SDIO_TRANSFER_SIZE];
//volatile int isDiagThreadRunning1 = 0; // Diagnostic Thread Stopped

/* Static function prototypes */
//static void   processData(unsigned int size);
//static void   RxDataCallback(unsigned char *data, long len);
//static void   printToConsoleDemodConfiguration(SL_DemodConfigInfo_t cfgInfo);
static void   printToConsoleDemodConfiguration(SL_DemodConfigInfo_t cfgInfo);
static void   printToConsoleDemodAtsc1Configuration(SL_Atsc1p0ConfigParams_t atsc1Info);
static void   logDemodConfiguration(SL_DemodConfigInfo_t cfgInfo);
static void   logDemodAtsc1Configuration(SL_Atsc1p0ConfigParams_t atsc1Info);
static void   printToConsoleTunerError(SL_TunerResult_t err);
static void   printToConsoleDemodError(SL_Result_t err);
//static void   handleCmdIfFailure(void);
static void   printToConsolePlfConfiguration(SL_PlatFormConfigParams_t cfgInfo);
static void   logPlfConfiguration(SL_PlatFormConfigParams_t cfgInfo);
static void printAtsc1Diagnostics(SL_Atsc1p0Perf_Diag_t perfDiag);

//static int    CaptureThread();
//static int    ProcessThread();
//static int    DiagnosticThread();

SL_Result_t SL_ATSC1_WriteBytes(unsigned int address, unsigned int lenBytes, void *buf)
{
	//SL_Result_t retVal = SL_OK;
	if (lenBytes >= 4)
		return SL_WriteBytes(slUnit, address, lenBytes, (unsigned int*)buf);
	else
		return SL_ERR_INVALID_ARGUMENTS;


}
SL_Result_t SL_ATSC1_ReadBytes(unsigned int address, unsigned int lenBytes, void *buf)
{
	if (lenBytes >= 4)
		return SL_ReadBytes(slUnit, address, lenBytes, (unsigned int*)buf);
	else
		return SL_ERR_INVALID_ARGUMENTS;

}


SL_Result_t SL3000_atsc1_init(Endeavour  *endeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *sPlfConfig, SL_DemodStd_t std)
{
	SL_I2cResult_t i2cres;
	SL_Result_t slres;
	SL_AfeIfConfigParams_t afeInfo;
	SL_OutIfConfigParams_t outPutInfo;
	SL_IQOffsetCorrectionParams_t iqOffSetCorrection;
	int swMajorNo, swMinorNo;
	unsigned int cFrequency = 0;
	SL_DemodBootStatus_t bootStatus;
    SL_ExtLnaConfigParams_t       lnaInfo;

	uint8_t i2c_bus = 3;

	sPlfConfig->chipType = SL_CHIP_3000;
	sPlfConfig->chipRev = SL_CHIP_REV_BB;
	sPlfConfig->boardType = SL_EVB_3000;//SL_KAILASH_DONGLE_3;// SL_EVB_3000;
	sPlfConfig->tunerType = TUNER_R855;//TUNER_NXP;
	sPlfConfig->demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
	sPlfConfig->demodOutputIf = SL_DEMOD_OUTPUTIF_TS;

	sPlfConfig->demodI2cAddr = SL_DEMOD_I2C_ADDR; /* SLDemod 7-bit Physical I2C Address */

	//sPlfConfig.slsdkPath = "..";
	sPlfConfig->slsdkPath = "";


	//int                       slUnit;

	//SL_Printf("\n SL3000_atsc1_tune Start!!");
	/* Tuner Config */
	pTunerCfg->bandwidth = SL_TUNER_BW_6MHZ;

	/* Set Configuration Parameters */
	SL_ConfigSetPlatform(*sPlfConfig);

	i2cres = SL_I2cInit(endeavour, i2c_bus);
	if (i2cres != SL_I2C_OK)
	{
		SL_Printf("\n Error:SL_I2cInit failed :");
		//printToConsoleI2cError(i2cres);
		//goto TEST_ERROR;
	}
	else
	{
		cmdIf = SL_CMD_CONTROL_IF_I2C;
	}
	/* Demod Config */
	switch (sPlfConfig->boardType)
	{
	case SL_EVB_3000:
		if (sPlfConfig->tunerType == TUNER_R855)
		{
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 5 + IF_OFFSET;

			//afeInfo.ifreq = 3.25 + IF_OFFSET;			
		}
		else if (sPlfConfig->tunerType == TUNER_NXP)
		{
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 3.25 + IF_OFFSET;

			//afeInfo.ifreq = 3.25 + IF_OFFSET;			
		}
		else if (sPlfConfig->tunerType == TUNER_SI || sPlfConfig->tunerType == TUNER_SI_P)
		{
			afeInfo.spectrum = SL_SPECTRUM_NORMAL;
			afeInfo.iftype = SL_IFTYPE_ZIF;
			afeInfo.ifreq = 0.0;
		}
		else if (sPlfConfig->tunerType == TUNER_SILABS)
		{
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 3.25;
		}
		else
		{
			SL_Printf("\n Invalid Tuner Selection");
		}

		if (sPlfConfig->demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
		{
			outPutInfo.oif = SL_OUTPUTIF_TSSERIAL_MSB_FIRST;

		}
		else if (sPlfConfig->demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
		{
			outPutInfo.oif = SL_OUTPUTIF_SDIO;
		}
		else
		{
			SL_Printf("\n Invalid OutPut Interface Selection");
		}

		afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
		afeInfo.qswap = SL_QPOL_SWAP_DISABLE;
		iqOffSetCorrection.iCoeff1 = 1.0;
		iqOffSetCorrection.qCoeff1 = 1.0;
		iqOffSetCorrection.iCoeff2 = 0.0;
		iqOffSetCorrection.qCoeff2 = 0.0;
		break;
	case SL_SILISA_DONGLE:
		if (sPlfConfig->tunerType == TUNER_SILABS)
		{
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 3.25;
		}
		else
		{
			SL_Printf("\n Invalid Tuner Selection");
		}

		if (sPlfConfig->demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
		{
			outPutInfo.oif = SL_OUTPUTIF_SDIO;
		}
		else
		{
			SL_Printf("\n Invalid OutPut Interface Selection");
		}

		afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
		afeInfo.qswap = SL_QPOL_SWAP_DISABLE;
		iqOffSetCorrection.iCoeff1 = 1.0;
		iqOffSetCorrection.qCoeff1 = 1.0;
		iqOffSetCorrection.iCoeff2 = 0.0;
		iqOffSetCorrection.qCoeff2 = 0.0;
		break;
	default:
		SL_Printf("\n Invalid Board Type Selected ");
		break;
	}
	afeInfo.iqswap = SL_IQSWAP_DISABLE;
	afeInfo.agcRefValue = 100; //afcRefValue in mV
	outPutInfo.TsoClockInvEnable = SL_TSO_CLK_INV_ON;

	slres = SL_DemodCreateInstance(&slUnit);
	if (slres != SL_OK)
	{
		SL_Printf("\n Error:SL_DemodCreateInstance :");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}

	SL_Printf("\n Initializing SL Demod..: ");

	slres = SL_DemodInit(slUnit, cmdIf, std);
	if (slres != SL_OK)
	{
		SL_Printf("FAILED");
		SL_Printf("\n Error:SL_DemodInit :");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}
	else
	{
		SL_Printf("\n SL_DemodInit SUCCESS\n");
	}

	do
	{
		slres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_BOOT, (SL_DemodBootStatus_t*)&bootStatus);
		if (slres != SL_OK)
		{
			SL_Printf("\n Error:SL_Demod Get Boot Status :");
			printToConsoleDemodError(slres);
		}
	} while (bootStatus != SL_DEMOD_BOOT_STATUS_COMPLETE);
	SL_Printf("\n Demod Boot Status      : ");
	if (bootStatus == SL_DEMOD_BOOT_STATUS_INPROGRESS)
	{
		SL_Printf(" %s", "INPROGRESS");
	}
	else if (bootStatus == SL_DEMOD_BOOT_STATUS_COMPLETE)
	{
		SL_Printf(" %s", "COMPLETED");
	}
	else if (bootStatus == SL_DEMOD_BOOT_STATUS_ERROR)
	{
		SL_Printf(" %s", "ERROR");
		//goto TEST_ERROR;
	}

	slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_AFEIF, &afeInfo);
	if (slres != 0)
	{
		SL_Printf("\n Error:SL_DemodConfigure :");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}

	slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_IQ_OFFSET_CORRECTION, &iqOffSetCorrection);
	if (slres != 0)
	{
		SL_Printf("\n Error:SL_DemodConfigure :");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}

	slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_OUTPUTIF, &outPutInfo);
	if (slres != 0)
	{
		SL_Printf("\n Error:SL_DemodConfigure :");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}
	    slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_EXT_LNA, (unsigned int *)&lnaInfo.lnaMode);
	    if (slres != 0)
	    {
	        SL_Printf("\n Error:SL_DemodConfigure :");
	        printToConsoleDemodError(slres);
	        //goto TEST_ERROR;
	    }

	slres = SL_DemodGetSoftwareVersion(slUnit, &swMajorNo, &swMinorNo);
	if (slres == SL_OK)
	{
		SL_Printf("\n Demod SW Version       : %d.%d", swMajorNo, swMinorNo);
	}


	atsc1ConfigInfo.bw = SL_BW_6MHZ;
	atsc1ConfigInfo.blockSize = BLOCK_SIZE;

	slres = SL_DemodConfigureEx(slUnit, std, &atsc1ConfigInfo);
	if (slres != 0)
	{
		SL_Printf("\n Error:SL_DemodConfigureEx :");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}
	slres = SL_DemodStart(slUnit);

	if (slres != 0)
	{
		SL_Printf("\n Saankhya Demod Start Failed");
		//goto TEST_ERROR;
	}
	else
	{
		SL_Printf("SUCCESS");
	}
	SL_SleepMS(1000); // Delay to accomdate set configurations at SL to take effect
	return slres;
}
SL_Result_t SL3000_atsc1_tune(Endeavour  *endeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *sPlfConfig)
{
	SL_I2cResult_t i2cres;
	SL_Result_t slres;
	SL_AfeIfConfigParams_t afeInfo;
	SL_OutIfConfigParams_t outPutInfo;
	SL_IQOffsetCorrectionParams_t iqOffSetCorrection;
	SL_DemodBootStatus_t bootStatus;
	SL_TunerSignalInfo_t tunerInfo;
	SL_TunerResult_t tres;

	SL_DemodStd_t std = SL_DEMODSTD_ATSC1_0;

	slres = SL_DemodGetConfiguration(slUnit, &cfgInfo);

	if (slres == SL_OK)
	{
		slres = SL_DemodGetConfigurationEx(slUnit, std, &atsc1ConfigInfo);
	}

	if (slres != SL_OK)
	{
		SL_Printf("\n Error:SL_Demod Get ATSC1 Configuration :");
		printToConsoleDemodError(slres);
		if (slres == SL_ERR_CMD_IF_FAILURE)
		{
			SL_Printf("\n Error:SL_ERR_CMD_IF_FAILURE");
			//handleCmdIfFailure();
			//goto TEST_ERROR;
		}
	}
	else
	{
		printToConsoleDemodConfiguration(cfgInfo);
		printToConsoleDemodAtsc1Configuration(atsc1ConfigInfo);
	}

	//SL_SleepMS(500);
	return slres;
}
void Monitor_SL3000_ATSC1_Signal(SignalInfo_t *pSigInfo, int freq, R855_Standard_Type RT_Standard)
{
	SL_TunerResult_t     tres;
	//SL_TunerSignalInfo_t tunerInfo;
	SL_DemodLockStatus_t lockStatus;
	int                  cpuStatus = 0;
	SL_Result_t          dres;
		int                  lockStatusFlag = 0;


	dres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_LOCK, (SL_DemodLockStatus_t*)&lockStatus);
	if (dres != SL_OK)
	{
		SL_Printf("\n Error:SL_Demod Get Lock Status                :");
		printToConsoleDemodError(dres);
	}
	else
	{
		if ((lockStatus & SL_DEMOD_LOCK_STATUS_MASK_DVBT_RF_LOCK))
		{
			pSigInfo->locked = LOCKED;
		}
		else
		{
			pSigInfo->locked = UNLOCKED;
			SL_Printf("\n Demod Lock Status : Unlocked");
		}
	}

	R855_GetTotalRssi(freq, RT_Standard, &pSigInfo->rssi);
//	SL_Printf("\n Tuner RSSI		: %d dBm", pSigInfo->rssi);


	dres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_CPU, (int*)&cpuStatus);

	if (dres != SL_OK)
	{
//        SL_Printf("\n Error:SL_Demod Get CPU Status                 :");
		printToConsoleDemodError(dres);
	}
//    else
//    {
//		SL_Printf("\n Demod CPU Status  : %s", (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED");
//    }

	if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC1p0_RF_LOCK)
	{
		dres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC1P0_PERF, (SL_Atsc1p0Perf_Diag_t*)&atsc1PerfDiag);
		if (dres != SL_OK)
		{
//            SL_Printf("\n Error getting ATSC1 Performance Diagnostics :");
			printToConsoleDemodError(dres);
		}
		else
		{
			//printAtsc1Diagnostics(atsc1PerfDiag);
			/* SNR */
			double snr = (double)52428800.0 / (double)atsc1PerfDiag.PostEquSNR;
			pSigInfo->snr = 10.0 * log10(snr);
//			SL_Printf("\n SNR               : %3.2f dB", pSigInfo->snr);

			/* BER */
			pSigInfo->ber = (double)atsc1PerfDiag.BitErrorCnt / ((double)atsc1PerfDiag.TotalTxBitCnt * 188.0 * 8.0);
//			SL_Printf("\n BER               : %1.2e", pSigInfo->ber);

			/* PER */
			pSigInfo->per = (double)atsc1PerfDiag.BlockErrorCnt / (double)atsc1PerfDiag.TotalTxBlockCnt;
//			SL_Printf("\n PER               : %1.2e", pSigInfo->per);

			/* Confidence */
			pSigInfo->confidence = (atsc1PerfDiag.BlockErrorCnt == 0 ? 100 : (double)(100.0 - (100.0 * (double)atsc1PerfDiag.BlockErrorCnt / (double)atsc1PerfDiag.TotalTxBlockCnt)));
//			SL_Printf("\n CONF              : %.2f %%\n\n", pSigInfo->confidence);
		}

		if (dres == SL_ERR_CMD_IF_FAILURE)
		{
			//handleCmdIfFailure();
			SL_Printf("\n SL_ERR_CMD_IF_FAILURE\n");
		}
	}
}

void SL3000_ATSC1_Initialize(Endeavour	*pEndeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *pPlfConfig, R855_Set_Info *pR855_Info)
{
	Init_R855(pEndeavour, pR855_Info);
	pTunerCfg->std = SL_TUNERSTD_ATSC1_0;//SL_TUNERSTD_ATSC3_0;
	SL3000_atsc1_init(pEndeavour, pTunerCfg, pPlfConfig, SL_DEMODSTD_ATSC1_0);
	//Sleep(1000);
}
void SL3000_ATSC1_BindTune(Endeavour  *pEndeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *pPlfConfig, R855_Set_Info *pR855_Info)
{
	//IF AGC select
	pR855_Info->R855_IfAgc_Select = R855_IF_AGC1;
	Tune_R855(pR855_Info);
	SL3000_atsc1_tune(pEndeavour, pTunerCfg, pPlfConfig);
	//Sleep(1000);
}

#if 0	// by ITE
/*
 *  ATSC1 Main Test function
 */
void atsc1_test(Endeavour			*endeavour, SL_TunerConfig_t *pTunerCfg, unsigned int              tunerFrequency)
{
	SL_I2cResult_t i2cres;
	SL_Result_t slres;
	SL_ConfigResult_t cres;
	SL_TunerResult_t tres;
	SL_UtilsResult_t utilsres;
	SL_TunerConfig_t tunerCfg;
	SL_TunerConfig_t tunerGetCfg;
	SL_TunerSignalInfo_t tunerInfo;
	int swMajorNo, swMinorNo;
	unsigned int cFrequency = 0;
	SL_AfeIfConfigParams_t afeInfo;
	SL_OutIfConfigParams_t outPutInfo;
    SL_ExtLnaConfigParams_t       lnaInfo;
	SL_IQOffsetCorrectionParams_t iqOffSetCorrection;
	SL_DemodBootStatus_t bootStatus;

	getPlfConfig.chipType = SL_CHIP_3000;
	getPlfConfig.chipRev = SL_CHIP_REV_BB;
	getPlfConfig.boardType = SL_EVB_3000;//SL_KAILASH_DONGLE_3;// SL_EVB_3000;
	getPlfConfig.tunerType = TUNER_NXP;//TUNER_NXP;
	getPlfConfig.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
	getPlfConfig.demodOutputIf = SL_DEMOD_OUTPUTIF_TS;

	getPlfConfig.demodI2cAddr = SL_DEMOD_I2C_ADDR; /* SLDemod 7-bit Physical I2C Address */

	//sPlfConfig.slsdkPath = "..";
	getPlfConfig.slsdkPath = "";



	// tunerFrequency = TUNER_FREQUENCY;
	/* Tuner Config */
	tunerCfg.bandwidth = SL_TUNER_BW_6MHZ;

	SL_Printf("\n Running ATSC1.0 Test...");
	/* Set Configuration Parameters */
	SL_ConfigSetPlatform(getPlfConfig);

	cres = SL_ConfigGetPlatform(&getPlfConfig);
	if (cres == SL_CONFIG_OK)
	{
		printToConsolePlfConfiguration(getPlfConfig);
	}
	else
	{
		SL_Printf("\n ERROR : SL_ConfigGetPlatform Failed ");
		goto TEST_ERROR;
	}

	if (getPlfConfig.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
	{
		uint8_t i2c_bus = 3;
		i2cres = SL_I2cInit(endeavour, i2c_bus);
		if (i2cres != SL_I2C_OK)
		{
			SL_Printf("\n Error:SL_I2cInit failed :");
			//printToConsoleI2cError(i2cres);
			//goto TEST_ERROR;
		}
		else
		{
			cmdIf = SL_CMD_CONTROL_IF_I2C;
		}
	}
	else if (getPlfConfig.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SDIO)
	{
		SL_Printf("\n Error:SL_SdioInit failed :Not Supported");
		goto TEST_ERROR;
	}
	else if (getPlfConfig.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SPI)
	{
		SL_Printf("\n Error:SL_SpiInit failed :Not Supported");
		goto TEST_ERROR;
	}

	/* Demod Config */
	switch (getPlfConfig.boardType)
	{
	case SL_EVB_3000:
		if (getPlfConfig.tunerType == TUNER_NXP)
		{
#if 0        
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 3.25 + IF_OFFSET;
#else
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 5 + IF_OFFSET;

#endif
			//afeInfo.ifreq = 4.4 + IF_OFFSET;			
		}
		else if (getPlfConfig.tunerType == TUNER_SI || getPlfConfig.tunerType == TUNER_SI_P)
		{
			afeInfo.spectrum = SL_SPECTRUM_NORMAL;
			afeInfo.iftype = SL_IFTYPE_ZIF;
			afeInfo.ifreq = 0.0;
		}
		else if (getPlfConfig.tunerType == TUNER_SILABS)
		{
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 3.25;
		}
		else
		{
			SL_Printf("\n Invalid Tuner Selection");
		}

		if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
		{
			outPutInfo.oif = SL_OUTPUTIF_TSSERIAL_MSB_FIRST;
			/* CPLD Reset */
			//SL_GpioSetPin(getPlfConfig.cpldResetGpioPin, 0x00);          // Low 
			SL_SleepMS(100); // 100ms delay for Toggle 
			//SL_GpioSetPin(getPlfConfig.cpldResetGpioPin, 0x01);          // High 
		}
		else if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
		{
			outPutInfo.oif = SL_OUTPUTIF_SDIO;
		}
		else
		{
			SL_Printf("\n Invalid OutPut Interface Selection");
		}

		afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
		afeInfo.qswap = SL_QPOL_SWAP_DISABLE;
		iqOffSetCorrection.iCoeff1 = 1.0;
		iqOffSetCorrection.qCoeff1 = 1.0;
		iqOffSetCorrection.iCoeff2 = 0.0;
		iqOffSetCorrection.qCoeff2 = 0.0;

        lnaInfo.lnaMode = SL_EXT_LNA_CFG_MODE_NOT_PRESENT;
		break;

	case SL_EVB_3010:
		if (getPlfConfig.tunerType == TUNER_NXP)
		{
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 3.25 + IF_OFFSET;
		}
		else if (getPlfConfig.tunerType == TUNER_SI || getPlfConfig.tunerType == TUNER_SI_P)
		{
			afeInfo.spectrum = SL_SPECTRUM_NORMAL;
			afeInfo.iftype = SL_IFTYPE_ZIF;
			afeInfo.ifreq = 0.0;
		}
		else if (getPlfConfig.tunerType == TUNER_SILABS)
		{
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 3.25;
		}
		else
		{
			SL_Printf("\n Invalid Tuner Selection");
		}

		if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
		{
			outPutInfo.oif = SL_OUTPUTIF_TSPARALLEL_LSB_FIRST;
		}
		else if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
		{
			outPutInfo.oif = SL_OUTPUTIF_SDIO;
		}
		else
		{
			SL_Printf("\n Invalid Output Interface Selection");
		}

		afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
		afeInfo.qswap = SL_QPOL_SWAP_DISABLE;
		iqOffSetCorrection.iCoeff1 = 1.0;
		iqOffSetCorrection.qCoeff1 = 1.0;
		iqOffSetCorrection.iCoeff2 = 0.0;
		iqOffSetCorrection.qCoeff2 = 0.0;

        lnaInfo.lnaMode = SL_EXT_LNA_CFG_MODE_NOT_PRESENT;
		break;

	case SL_EVB_4000:
		if (getPlfConfig.tunerType == TUNER_SI || getPlfConfig.tunerType == TUNER_SI_P)
		{
			afeInfo.spectrum = SL_SPECTRUM_NORMAL;
			afeInfo.iftype = SL_IFTYPE_ZIF;
			afeInfo.ifreq = 0.0;
		}
		else
		{
			SL_Printf("\n Invalid Tuner Selection");
		}

		if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
		{
			outPutInfo.oif = SL_OUTPUTIF_TSSERIAL_LSB_FIRST;
			/* CPLD Reset */
			//SL_GpioSetPin(getPlfConfig.cpldResetGpioPin, 0x00); // Low 
			SL_SleepMS(100); // 100ms delay for Toggle 
			//SL_GpioSetPin(getPlfConfig.cpldResetGpioPin, 0x01); // High 
		}
		else if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
		{
			outPutInfo.oif = SL_OUTPUTIF_SDIO;
		}
		else
		{
			SL_Printf("\n Invalid Output Interface Selection");
		}

		afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
		afeInfo.qswap = SL_QPOL_SWAP_ENABLE;
		iqOffSetCorrection.iCoeff1 = 1;
		iqOffSetCorrection.qCoeff1 = 1;
		iqOffSetCorrection.iCoeff2 = 0;
		iqOffSetCorrection.qCoeff2 = 0;

        lnaInfo.lnaMode = SL_EXT_LNA_CFG_MODE_NOT_PRESENT;
		break;

	case SL_KAILASH_DONGLE:
		if (getPlfConfig.tunerType == TUNER_SI)
		{
			afeInfo.spectrum = SL_SPECTRUM_NORMAL;
			afeInfo.iftype = SL_IFTYPE_ZIF;
			afeInfo.ifreq = 0.0;
		}
		else
		{
			SL_Printf("\n Invalid Tuner Type selected ");
		}

		if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
		{
			outPutInfo.oif = SL_OUTPUTIF_TSPARALLEL_LSB_FIRST;
		}
		else
		{
			SL_Printf("\n Invalid OutPut Interface Selection");
		}

        lnaInfo.lnaMode = SL_EXT_LNA_CFG_MODE_NOT_PRESENT;
		break;

	case SL_KAILASH_DONGLE_2:
		if (getPlfConfig.tunerType == TUNER_SI_P)
		{
			afeInfo.spectrum = SL_SPECTRUM_NORMAL;
			afeInfo.iftype = SL_IFTYPE_ZIF;
			afeInfo.ifreq = 0.0;
		}
		else
		{
			SL_Printf("\n Invalid Tuner Type selected ");
		}

		if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
		{
			outPutInfo.oif = SL_OUTPUTIF_TSPARALLEL_LSB_FIRST;
		}
		else
		{
			SL_Printf("\n Invalid OutPut Interface Selection");
		}

		afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
		afeInfo.qswap = SL_QPOL_SWAP_ENABLE;
		iqOffSetCorrection.iCoeff1 = (float)1.00724023045574;
		iqOffSetCorrection.qCoeff1 = (float)0.998403791546105;
		iqOffSetCorrection.iCoeff2 = (float)0.0432678874719328;
		iqOffSetCorrection.qCoeff2 = (float)0.0436508327768608;

        lnaInfo.lnaMode = SL_EXT_LNA_CFG_MODE_NOT_PRESENT;
		break;

    case SL_SILISA_DONGLE:
		if (getPlfConfig.tunerType == TUNER_SILABS)
		{
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 3.25;
		}
		else
		{
			SL_Printf("\n Invalid Tuner Selection");
		}

		if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
		{
			outPutInfo.oif = SL_OUTPUTIF_SDIO;
		}
		else
		{
			SL_Printf("\n Invalid OutPut Interface Selection");
		}

		afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
		afeInfo.qswap = SL_QPOL_SWAP_DISABLE;
		iqOffSetCorrection.iCoeff1 = 1.0;
		iqOffSetCorrection.qCoeff1 = 1.0;
		iqOffSetCorrection.iCoeff2 = 0.0;
		iqOffSetCorrection.qCoeff2 = 0.0;

        lnaInfo.lnaMode = SL_EXT_LNA_CFG_MODE_NOT_PRESENT;
		break;

    case SL_YOGA_DONGLE:
        if (getPlfConfig.tunerType == TUNER_SI_P)
		{
            afeInfo.spectrum = SL_SPECTRUM_NORMAL;
            afeInfo.iftype = SL_IFTYPE_ZIF;
            afeInfo.ifreq = 0.0;
		}
		else
		{
			SL_Printf("\n Invalid Tuner Selection");
		}

		if (getPlfConfig.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
		{
			outPutInfo.oif = SL_OUTPUTIF_SDIO;
		}
		else
		{
            SL_Printf("\n Invalid Output Interface Selection");
		}

		afeInfo.iswap = SL_IPOL_SWAP_DISABLE;
        afeInfo.qswap = SL_QPOL_SWAP_ENABLE;
        iqOffSetCorrection.iCoeff1 = 1;
        iqOffSetCorrection.qCoeff1 = 1;
        iqOffSetCorrection.iCoeff2 = 0;
        iqOffSetCorrection.qCoeff2 = 0;

        lnaInfo.lnaMode = SL_EXT_LNA_CFG_MODE_AUTO;
        lnaInfo.lnaGpioNum = 12;
        if (lnaInfo.lnaMode != SL_EXT_LNA_CFG_MODE_NOT_PRESENT)
        {
            /*
             * GPIO12 is used for LNA Bypass/Enable in Yoga Dongle.
             * It may be different for other boards. Use bits 8 to 15 to specify the same
             */
			//lnaInfo.lnaMode = (SL_ExtLnaModeConfig_t)(lnaInfo.lnaMode | (lnaInfo.lnaGpioNum << 8));
        }
		break;

	default:
		SL_Printf("\n Invalid Board Type Selected ");
		break;
	}
	afeInfo.iqswap = SL_IQSWAP_DISABLE;
	afeInfo.agcRefValue = 100; //afcRefValue in mV
	outPutInfo.TsoClockInvEnable = SL_TSO_CLK_INV_ON;

	slres = SL_DemodCreateInstance(&slUnit);
	if (slres != SL_OK)
	{
		SL_Printf("\n Error:SL_DemodCreateInstance :");
		printToConsoleDemodError(slres);
		goto TEST_ERROR;
	}

	SL_Printf("\n Initializing SL Demod..: ");

	slres = SL_DemodInit(slUnit, cmdIf, SL_DEMODSTD_ATSC1_0);
	if (slres != SL_OK)
	{
		SL_Printf("FAILED");
		SL_Printf("\n Error:SL_DemodInit :");
		printToConsoleDemodError(slres);
		goto TEST_ERROR;
	}
	else
	{
		SL_Printf("SL_DemodInit SUCCESS\n");
	}


	do
	{
		slres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_BOOT, (SL_DemodBootStatus_t*)&bootStatus);
		if (slres != SL_OK)
		{
			SL_Printf("\n Error:SL_Demod Get Boot Status :");
			printToConsoleDemodError(slres);
		}
	} while (bootStatus != SL_DEMOD_BOOT_STATUS_COMPLETE);
	SL_Printf("\n Demod Boot Status      : ");
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
		goto TEST_ERROR;
	}

	slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_AFEIF, &afeInfo);
	if (slres != 0)
	{
		SL_Printf("\n Error:SL_DemodConfigure :");
		printToConsoleDemodError(slres);
		goto TEST_ERROR;
	}

    slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_IQ_OFFSET_CORRECTION, &iqOffSetCorrection);
	if (slres != 0)
	{
		SL_Printf("\n Error:SL_DemodConfigure :");
		printToConsoleDemodError(slres);
		goto TEST_ERROR;
	}

	slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_OUTPUTIF, &outPutInfo);
	if (slres != 0)
	{
		SL_Printf("\n Error:SL_DemodConfigure :");
		printToConsoleDemodError(slres);
		goto TEST_ERROR;
	}

    slres = SL_DemodConfigure(slUnit, SL_CONFIGTYPE_EXT_LNA, (unsigned int *)&lnaInfo.lnaMode);
    if (slres != 0)
    {
        SL_Printf("\n Error:SL_DemodConfigure :");
        printToConsoleDemodError(slres);
        goto TEST_ERROR;
    }

	slres = SL_DemodGetSoftwareVersion(slUnit, &swMajorNo, &swMinorNo);
	if (slres == SL_OK)
	{
		SL_Printf("\n Demod SW Version       : %d.%d", swMajorNo, swMinorNo);
	}


	/* ATSC Standard Configuration */
	switch (tunerCfg.bandwidth)
	{
	case SL_TUNER_BW_6MHZ:
		atsc1ConfigInfo.bw = SL_BW_6MHZ;
		break;

	case SL_TUNER_BW_7MHZ:
		atsc1ConfigInfo.bw = SL_BW_7MHZ;
		break;

	case SL_TUNER_BW_8MHZ:
		atsc1ConfigInfo.bw = SL_BW_8MHZ;
		break;
	}
	atsc1ConfigInfo.blockSize = BLOCK_SIZE;

	slres = SL_DemodConfigureEx(slUnit, SL_DEMODSTD_ATSC1_0, &atsc1ConfigInfo);
	if (slres != 0)
	{
		SL_Printf("\n Error:SL_DemodConfigureEx :");
		printToConsoleDemodError(slres);
		goto TEST_ERROR;
	}

	slres = SL_DemodStart(slUnit);

	if (slres != 0)
	{
		SL_Printf("\n Saankhya Demod Start Failed");
		goto TEST_ERROR;
	}
	else
	{
		SL_Printf("SUCCESS");
		SL_Printf("\n SL Demod Output Capture: STARTED : sl-ts.bin");
	}
	SL_SleepMS(1000); // Delay to accomdate set configurations at SL to take effect

	slres = SL_DemodGetConfiguration(slUnit, &cfgInfo);

	if (slres == SL_OK)
	{
		slres = SL_DemodGetConfigurationEx(slUnit, SL_DEMODSTD_ATSC1_0, &atsc1ConfigInfo);
	}

	if (slres != SL_OK)
	{
		SL_Printf("\n Error:SL_Demod Get ATSC1 Configuration :");
		printToConsoleDemodError(slres);
		if (slres == SL_ERR_CMD_IF_FAILURE)
		{
			//handleCmdIfFailure();
			goto TEST_ERROR;
		}
	}
	else
	{
		printToConsoleDemodConfiguration(cfgInfo);
		printToConsoleDemodAtsc1Configuration(atsc1ConfigInfo);
	}
	SL_DemodLockStatus_t lockStatus;
	static unsigned int  loopCounter = 0;
	int                  lockStatusFlag = 0;
	int                  cpuStatus = 0;


	slres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_LOCK, (SL_DemodLockStatus_t*)&lockStatus);
	SL_Printf("\n lockStatus=%d\n:", lockStatus);
	if (slres != SL_OK)
	{
		SL_Printf("\n Error:SL_Demod Get Lock Status                :");
		printToConsoleDemodError(slres);
	}
	else
	{
		if ((lockStatus & SL_DEMOD_LOCK_STATUS_MASK_DVBT_RF_LOCK))
		{
			if (!lockStatusFlag)
			{
				loopCounter = 0;
				lockStatusFlag = 1;
			}
			SL_Printf("\n Demod Lock Status : Locked");
			if (loopCounter == 0)
				SL_Printf("\n\n Demod Lock Status      : Locked");
		}
		else
		{
			if (lockStatusFlag)
			{
				loopCounter = 0;
				lockStatusFlag = 0;
			}
			SL_Printf("\n Demod Lock Status : Unlocked");
			if (loopCounter == 0)
				SL_Printf("\n\n Demod Lock Status      : Unlocked");
		}
	}

#if 0
	tres = SL_TunerGetStatus(tUnit, &tunerInfo);
	if (tres != SL_TUNER_OK)
	{
		SL_Printf("\n Error:SL_TunerGetStatus                       :");
		printToConsoleTunerError(tres);
	}
	else
	{
		SL_Printf("\n Tuner RSSI        : %3.2f dBm", tunerInfo.signalStrength);
		if (loopCounter == 0)
			SL_Printf("\n Tuner RSSI             : %3.2f dBm", tunerInfo.signalStrength);
	}
#endif
#if 0	

	slres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_CPU, (int*)&cpuStatus);

	if (slres != SL_OK)
	{
		SL_Printf("\n Error:SL_Demod Get CPU Status                 :");
		printToConsoleDemodError(slres);
	}
	else
	{
		SL_Printf("\n Demod CPU Status  : %s", (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED");
		if (loopCounter == 0)
			SL_Printf("\n Demod CPU Status       : %s", (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED");
	}		

	SL_Printf("\n Press any key to exit..");

	getchar();
	getchar();

	diagFlag = 0;
	SL_Printf("\n Releasing resources...");

	SL_SleepMS(100); // wait for diagnostic Thread Exit
	SL_DemodStop(slUnit);
	/*Wait Diagostic Read Complete and Demod Stop*/
	SL_DemodUnInit(slUnit);
	//SL_TunerUnInit(tUnit);
	//SL_I2cUnInit();
	SL_DemodDeleteInstance(slUnit);

	SL_SleepMS(100);

	SL_RxDataStop();

	SL_SleepMS(100);
#endif
	SL_Printf("\n Exiting ATSC1 test");
	SL_SleepMS(1000);
	return;

TEST_ERROR:
	SL_DemodUnInit(slUnit);
	SL_TunerUnInit(tUnit);
	SL_DemodDeleteInstance(slUnit);
	SL_TunerDeleteInstance(tUnit);

	SL_Printf("\n Press any key to exit TEST_ERROR..");
	getchar();
	SL_I2cUnInit();
	SL_Printf("\n Exiting ATSC1 test \n");
	SL_SleepMS(1000);
	return;
}
#endif
#if 0
int CaptureThread()
{
	isCaptureThreadRunning = 1;
	SL_RxDataStart((RxDataCB)RxDataCallback);
	isCaptureThreadRunning = 0;
	SL_DeleteThread(&cThread);
	return 0;
}

int ProcessThread()
{
	SL_Printf("\n [ProcessThread]\n");
	isProcessThreadRunning = 1;
	fp = SL_FileOpen("sl-ts.bin", "wb");
	if (fp == NULL)
	{
		SL_Printf("\n Error: sl-ts.bin failed to open");
	}
	SL_FileClose(fp);

	cb = CircularBufferCreate(CB_SIZE);
	while (processFlag)
	{

		if (CircularBufferGetDataSize(cb) >= MAX_TS_BUFFER_SIZE)
		{
			processData(MAX_TS_BUFFER_SIZE);
		}
		else
		{
			//SL_SleepMS(5);
		}
	}
	CircularBufferFree(cb); // delete Circular buffer
	cb = NULL;
	isProcessThreadRunning = 0;
	SL_DeleteThread(&pThread);
	return 0;
}

int DiagnosticThread()
{
	isDiagThreadRunning = 1; // Diagnostic Thread Started

	SL_TunerResult_t     tres;
	SL_TunerSignalInfo_t tunerInfo;
	SL_DemodLockStatus_t lockStatus;
	int                  cpuStatus = 0;
	SL_Result_t          dres;
	static unsigned int  loopCounter = 0;
	int                  lockStatusFlag = 0;
	/*
	atsc1DiagFp = SL_FileOpen("logAtsc1Diag.txt", "w");
	if (atsc1DiagFp == NULL)
	{
	SL_Printf("\n Failed to open logAtsc1Diag.txt");
	}
	else
	{
	logPlfConfiguration(getPlfConfig);
	logDemodConfiguration(cfgInfo);
	logDemodAtsc1Configuration(atsc1ConfigInfo);
	SL_FileClose(atsc1DiagFp);
	}

	while (diagFlag)
	{
	atsc1DiagFp = SL_FileOpen("logAtsc1Diag.txt", "a");
	if (atsc1DiagFp == NULL)
	{
	SL_Printf("\n Failed to open logAtsc1Diag.txt");
	}
	SL_ClearScreen();

	dres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_LOCK, (SL_DemodLockStatus_t*)&lockStatus);
	if (dres != SL_OK)
	{
	SL_Printf("\n Error:SL_Demod Get Lock Status                :");
	printToConsoleDemodError(dres);
	}
	else
	{
	if ((lockStatus & SL_DEMOD_LOCK_STATUS_MASK_DVBT_RF_LOCK))
	{
	if (!lockStatusFlag)
	{
	loopCounter = 0;
	lockStatusFlag = 1;
	}
	SL_Printf("\n Demod Lock Status : Locked");
	if (loopCounter == 0)
	SL_FilePrintf(atsc1DiagFp, "\n\n Demod Lock Status      : Locked");
	}
	else
	{
	if (lockStatusFlag)
	{
	loopCounter = 0;
	lockStatusFlag = 0;
	}
	SL_Printf("\n Demod Lock Status : Unlocked");
	if (loopCounter == 0)
	SL_FilePrintf(atsc1DiagFp, "\n\n Demod Lock Status      : Unlocked");
	}
	}

	tres = SL_TunerGetStatus(tUnit, &tunerInfo);
	if (tres != SL_TUNER_OK)
	{
	SL_Printf("\n Error:SL_TunerGetStatus                       :");
	printToConsoleTunerError(tres);
	}
	else
	{
	SL_Printf("\n Tuner RSSI        : %3.2f dBm", tunerInfo.signalStrength);
	if (loopCounter == 0)
	SL_FilePrintf(atsc1DiagFp, "\n Tuner RSSI             : %3.2f dBm", tunerInfo.signalStrength);
	}

	dres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_CPU, (int*)&cpuStatus);

	if (dres != SL_OK)
	{
	SL_Printf("\n Error:SL_Demod Get CPU Status                 :");
	printToConsoleDemodError(dres);
	}
	else
	{
	SL_Printf("\n Demod CPU Status  : %s", (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED");
	if (loopCounter == 0)
	SL_FilePrintf(atsc1DiagFp, "\n Demod CPU Status       : %s", (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED");
	}
        if (demodLockStatus == 1)
        {
            dres = SL_DemodGetConfiguration(slUnit, &cfgInfo);
            if (dres != SL_OK)
            {
                SL_Printf("\n Error:SL_Demod Get Configuration :");
                printToConsoleDemodError(dres);
                if (dres == SL_ERR_CMD_IF_FAILURE)
                {
                    handleCmdIfFailure();
                }
            }

            if (cfgInfo.extLnaMode != SL_EXT_LNA_CFG_MODE_NOT_PRESENT)
            {
                switch ((cfgInfo.extLnaMode >> 16) & 0x00000001)
                {
                case 0:
                    SL_Printf("\n Current LNA status: External LNA Bypassed ");
                    break;

                case 1:
                    SL_Printf("\n Current LNA status: External LNA Enabled");
                    break;

                default:
                    SL_Printf("\n Current LNA status: Invalid");
                    break;
                }
            }
        }
	if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC1p0_RF_LOCK)
	{
	dres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC1P0_PERF, (SL_Atsc1p0Perf_Diag_t*)&atsc1PerfDiag);
	if (dres != SL_OK)
	{
	SL_Printf("\n Error getting ATSC1 Performance Diagnostics :");
	printToConsoleDemodError(dres);
	}
	else
	{
	printAtsc1Diagnostics(atsc1PerfDiag);
	if (loopCounter == 0)
	logAtsc1Diagnostics(atsc1PerfDiag);
	}

	if (dres == SL_ERR_CMD_IF_FAILURE)
	{
	handleCmdIfFailure();
	}
	}

	loopCounter = (loopCounter + 1) % 60;
	SL_FileClose(atsc1DiagFp);
	SL_Printf("\n Press any key to exit..");
	SL_SleepMS(1000); //Read Diagnostics every second
	}
	isDiagThreadRunning = 0; // Diagnostic Thread Stopped
	SL_DeleteThread(&dThread);
	*/
	return 0;
}

static void handleCmdIfFailure(void)
{
	SL_Printf("\n SL CMD IF FAILURE: Cannot continue!");
	SL_DemodUnInit(slUnit);
	SL_TunerUnInit(tUnit);
	SL_DemodDeleteInstance(slUnit);
	SL_TunerDeleteInstance(tUnit);
	processFlag = 0;
	diagFlag = 0;
}

static void RxDataCallback(unsigned char *data, long len)
{
	if (cb != NULL)
		CircularBufferPush(cb, (char *)data, len);
}

static void processData(unsigned int size)
{
	fp = SL_FileOpen("sl-ts.bin", "ab");
	if (fp == NULL)
	{
		SL_Printf("\n Error: sl-ts.bin failed to open");
	}
	else
	{
		CircularBufferPop(cb, size, (char*)buffer);
		SL_FileWrite(buffer, 1, size, fp);
		SL_FileClose(fp);
	}
}

#endif

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
	else if (cfgInfo.tunerType == TUNER_R855)
	{
		SL_Printf("\n Tuner Type             : TUNER_R855");
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

#if 0
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

	case SL_KAILASH_DONGLE_3:
		SL_Printf("\n Board Type             : SL_KAILASH_DONGLE_3");
		break;

	case SL_SILISA_DONGLE:
		SL_Printf("\n Board Type             : SL_SILISA_DONGLE");
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
#endif
static void printToConsoleDemodConfiguration(SL_DemodConfigInfo_t cfgInfo)
{
	//SL_Printf("\n\n SL Demod Configuration");
	switch (cfgInfo.std)
	{
	case SL_DEMODSTD_ATSC3_0:
		SL_Printf("\n Standard               : ATSC3_0");
		break;

	case SL_DEMODSTD_ATSC1_0:
		SL_Printf("\n Demod Standard         : ATSC1_0");
		break;

	case SL_DEMODSTD_DVB_T:
		SL_Printf("\n Demod Standard         : DVB-T");
		break;

	default:
		SL_Printf("\n Demod Standard         : NA");
    }

#if 0
    switch (cfgInfo.extLnaMode & 0xFFFF00FF)
    {
    case SL_EXT_LNA_CFG_MODE_NOT_PRESENT:
        SL_Printf("\n LNA Mode               : External LNA Not Present");
        break;

    case SL_EXT_LNA_CFG_MODE_AUTO:
        SL_Printf("\n LNA Mode               : External LNA in Auto Mode");
        break;

    case SL_EXT_LNA_CFG_MODE_MANUAL_BYPASS:
        SL_Printf("\n LNA Mode               : External LNA in Manual Bypass Mode");
        break;

    case SL_EXT_LNA_CFG_MODE_MANUAL_ENABLE:
        SL_Printf("\n LNA Mode               : External LNA in Manual Enable Mode");
        break;

    default:
        SL_Printf("\n LNA Mode               : NA");
    }
#endif

	SL_Printf("\n Input Configuration");
	switch (cfgInfo.afeIfInfo.iftype)
	{
	case SL_IFTYPE_ZIF:
		SL_Printf("\n   IF Type              : ZIF");
		break;

	case SL_IFTYPE_LIF:
		SL_Printf("\n   IF Type              : LIF");
		break;

	default:
		SL_Printf("\n   IF Type              : NA");
	}

	switch (cfgInfo.afeIfInfo.iqswap)
	{
	case SL_IQSWAP_DISABLE:
		SL_Printf("\n   IQSWAP               : DISABLE");
		break;

	case SL_IQSWAP_ENABLE:
		SL_Printf("\n   IQSWAP               : ENABLE");
		break;

	default:
		SL_Printf("\n   IQSWAP               : NA");
	}

	switch (cfgInfo.afeIfInfo.iswap)
	{
	case SL_IPOL_SWAP_DISABLE:
		SL_Printf("\n   ISWAP                : DISABLE");
		break;

	case SL_IPOL_SWAP_ENABLE:
		SL_Printf("\n   ISWAP                : ENABLE");
		break;

	default:
		SL_Printf("\n   ISWAP                : NA");
	}

	switch (cfgInfo.afeIfInfo.qswap)
	{
	case SL_QPOL_SWAP_DISABLE:
		SL_Printf("\n   QSWAP                : DISABLE");
		break;

	case SL_QPOL_SWAP_ENABLE:
		SL_Printf("\n   QSWAP                : ENABLE");
		break;

	default:
		SL_Printf("\n   QSWAP                : NA");
	}

	SL_Printf("\n   ICoeff1              : %3.4f", cfgInfo.iqOffCorInfo.iCoeff1);
	SL_Printf("\n   QCoeff1              : %3.4f", cfgInfo.iqOffCorInfo.qCoeff1);
	SL_Printf("\n   ICoeff2              : %3.4f", cfgInfo.iqOffCorInfo.iCoeff2);
	SL_Printf("\n   QCoeff2              : %3.4f", cfgInfo.iqOffCorInfo.qCoeff2);

	SL_Printf("\n   AGCReference         : %d mv", cfgInfo.afeIfInfo.agcRefValue);
	SL_Printf("\n   Tuner IF Frequency   : %3.2f MHz", cfgInfo.afeIfInfo.ifreq);

	SL_Printf("\n Output Configuration");
	switch (cfgInfo.oifInfo.oif)
	{
	case SL_OUTPUTIF_TSPARALLEL_LSB_FIRST:
		SL_Printf("\n   OutputInteface       : TS PARALLEL LSB FIRST");
		break;

	case SL_OUTPUTIF_TSPARALLEL_MSB_FIRST:
		SL_Printf("\n   OutputInteface       : TS PARALLEL MSB FIRST");
		break;

	case SL_OUTPUTIF_TSSERIAL_LSB_FIRST:
		SL_Printf("\n   OutputInteface       : TS SERAIL LSB FIRST");
		break;

	case SL_OUTPUTIF_TSSERIAL_MSB_FIRST:
		SL_Printf("\n   OutputInteface       : TS SERIAL MSB FIRST");
		break;

	case SL_OUTPUTIF_SDIO:
		SL_Printf("\n   OutputInteface       : SDIO");
		break;

	case SL_OUTPUTIF_SPI:
		SL_Printf("\n   OutputInteface       : SPI");
		break;

	default:
		SL_Printf("\n   OutputInteface       : NA");
	}

	switch (cfgInfo.oifInfo.TsoClockInvEnable)
	{
	case SL_TSO_CLK_INV_OFF:
		SL_Printf("\n   TS Out Clock Inv     : DISABLED");
		break;

	case SL_TSO_CLK_INV_ON:
		SL_Printf("\n   TS Out Clock Inv     : ENABLED");
		break;

	default:
		SL_Printf("\n    TS Out Clock Inv    : NA");
	}
}

static void   logDemodConfiguration(SL_DemodConfigInfo_t cfgInfo)
{
	SL_Printf("\n---------SL Demod Configuration-------------------------");
	switch (cfgInfo.std)
	{
	case SL_DEMODSTD_ATSC3_0:
		SL_Printf("\n Standard               : ATSC3_0");
		break;

	case SL_DEMODSTD_ATSC1_0:
		SL_Printf("\n Demod Standard         : ATSC1_0");
		break;

	default:
		SL_Printf("\n Demod Standard         : NA");
	}

	SL_Printf("\n Input Configuration");
	switch (cfgInfo.afeIfInfo.iftype)
	{
	case SL_IFTYPE_ZIF:
		SL_Printf("\n   IF Type              : ZIF");
		break;

	case SL_IFTYPE_LIF:
		SL_Printf("\n   IF Type              : LIF");
		break;

	default:
		SL_Printf("\n   IF Type              : NA");
	}

	switch (cfgInfo.afeIfInfo.iqswap)
	{
	case SL_IQSWAP_DISABLE:
		SL_Printf("\n   IQSWAP               : DISABLE");
		break;

	case SL_IQSWAP_ENABLE:
		SL_Printf("\n   IQSWAP               : ENABLE");
		break;

	default:
		SL_Printf("\n   IQSWAP               : NA");
	}

	switch (cfgInfo.afeIfInfo.iswap)
	{
	case SL_IPOL_SWAP_DISABLE:
		SL_Printf("\n   ISWAP                : DISABLE");
		break;

	case SL_IPOL_SWAP_ENABLE:
		SL_Printf("\n   ISWAP                : ENABLE");
		break;

	default:
		SL_Printf("\n   ISWAP                : NA");
	}

	switch (cfgInfo.afeIfInfo.qswap)
	{
	case SL_QPOL_SWAP_DISABLE:
		SL_Printf("\n   QSWAP                : DISABLE");
		break;

	case SL_QPOL_SWAP_ENABLE:
		SL_Printf("\n   QSWAP                : ENABLE");
		break;

	default:
		SL_Printf("\n   QSWAP                : NA");
	}

	SL_Printf("\n   ICoeff1              : %3.4f", cfgInfo.iqOffCorInfo.iCoeff1);
	SL_Printf("\n   QCoeff1              : %3.4f", cfgInfo.iqOffCorInfo.qCoeff1);
	SL_Printf("\n   ICoeff2              : %3.4f", cfgInfo.iqOffCorInfo.iCoeff2);
	SL_Printf("\n   QCoeff2              : %3.4f", cfgInfo.iqOffCorInfo.qCoeff2);

	SL_Printf("\n   AGCReference         : %d mv", cfgInfo.afeIfInfo.agcRefValue);
	SL_Printf("\n   Tuner IF Frequency   : %3.2f MHz", cfgInfo.afeIfInfo.ifreq);

	SL_Printf("\n Output Configuration");
	switch (cfgInfo.oifInfo.oif)
	{
	case SL_OUTPUTIF_TSPARALLEL_LSB_FIRST:
		SL_Printf("\n   OutputInteface       : TS PARALLEL LSB FIRST");
		break;

	case SL_OUTPUTIF_TSPARALLEL_MSB_FIRST:
		SL_Printf("\n   OutputInteface       : TS PARALLEL MSB FIRST");
		break;

	case SL_OUTPUTIF_TSSERIAL_LSB_FIRST:
		SL_Printf("\n   OutputInteface       : TS SERAIL LSB FIRST");
		break;

	case SL_OUTPUTIF_TSSERIAL_MSB_FIRST:
		SL_Printf("\n   OutputInteface       : TS SERIAL MSB FIRST");
		break;

	case SL_OUTPUTIF_SDIO:
		SL_Printf("\n   OutputInteface       : SDIO");
		break;

	case SL_OUTPUTIF_SPI:
		SL_Printf("\n   OutputInteface       : SPI");
		break;

	default:
		SL_Printf("\n   OutputInteface       : NA");
	}

	switch (cfgInfo.oifInfo.TsoClockInvEnable)
	{
	case SL_TSO_CLK_INV_OFF:
		SL_Printf("\n   TS Out Clock Inv     : DISABLED");
		break;

	case SL_TSO_CLK_INV_ON:
		SL_Printf("\n   TS Out Clock Inv     : ENABLED");
		break;

	default:
		SL_Printf("\n    TS Out Clock Inv    : NA");
	}
}

static void   printToConsoleDemodAtsc1Configuration(SL_Atsc1p0ConfigParams_t atsc1Info)
{
	SL_Printf("\n ATSC1 Configuration");
	switch (atsc1Info.bw)
	{
	case SL_BW_6MHZ:
		SL_Printf("\n   Bandwidth            : 6MHz");
		break;

	case SL_BW_7MHZ:
		SL_Printf("\n   Bandwidth            : 7MHz");
		break;

	case SL_BW_8MHZ:
		SL_Printf("\n   Bandwidth            : 8MHz");
		break;

	default:
		SL_Printf("\n   Bandwidth            : NA");
	}

	SL_Printf("\n   Block Size           : %d", atsc1Info.blockSize);
}

static void   logDemodAtsc1Configuration(SL_Atsc1p0ConfigParams_t atsc1Info)
{
	SL_Printf("\n ATSC1 Configuration");
	switch (atsc1Info.bw)
	{
	case SL_BW_6MHZ:
		SL_Printf("\n   Bandwidth            : 6MHz");
		break;

	case SL_BW_7MHZ:
		SL_Printf("\n   Bandwidth            : 7MHz");
		break;

	case SL_BW_8MHZ:
		SL_Printf("\n   Bandwidth            : 8MHz");
		break;

	default:
		SL_Printf("\n   Bandwidth            : NA");
	}

	SL_Printf("\n   Bandwidth            : 8MHz");
	SL_Printf("\n   Block Size           : %d", atsc1Info.blockSize);
}

static void printToConsoleTunerError(SL_TunerResult_t err)
{
	switch (err)
	{
	case SL_TUNER_ERR_OPERATION_FAILED:
		SL_Printf(" Sl Tuner Operation Failed");
		break;

	case SL_TUNER_ERR_INVALID_ARGS:
		SL_Printf(" Sl Tuner Invalid Argument");
		break;

	case SL_TUNER_ERR_NOT_SUPPORTED:
		SL_Printf(" Sl Tuner Not Supported");
		break;

	case SL_TUNER_ERR_MAX_INSTANCES_REACHED:
		SL_Printf(" Sl Tuner Maximum Instance Reached");
		break;
	default:
		break;
	}
}

static void printToConsoleDemodError(SL_Result_t err)
{
	switch (err)
	{
	case SL_ERR_OPERATION_FAILED:
		SL_Printf(" Sl Operation Failed");
		break;

	case SL_ERR_TOO_MANY_INSTANCES:
		SL_Printf(" Sl Too Many Instance");
		break;

	case SL_ERR_CODE_DWNLD:
		SL_Printf(" Sl Code download Failed");
		break;

	case SL_ERR_INVALID_ARGUMENTS:
		SL_Printf(" Sl Invalid Argument");
		break;

	case SL_ERR_CMD_IF_FAILURE:
		SL_Printf(" Sl Command Interface Failure");
		break;

	case SL_ERR_NOT_SUPPORTED:
		SL_Printf(" Sl Not Supported");
		break;
	default:
		break;
	}
}

static void printAtsc1Diagnostics(SL_Atsc1p0Perf_Diag_t perfDiag)
{
	/* SNR */
	double snr = (double)52428800.0 / (double)perfDiag.PostEquSNR;
	snr = 10.0 * log10(snr);
	SL_Printf("\n SNR               : %3.2f dB", snr);

	/* BER */
	double ber = (double)perfDiag.BitErrorCnt / ((double)perfDiag.TotalTxBitCnt * 188.0 * 8.0);
	SL_Printf("\n BER               : %1.2e", ber);

	/* PER */
	double per = (double)perfDiag.BlockErrorCnt / (double)perfDiag.TotalTxBlockCnt;
	SL_Printf("\n PER               : %1.2e", per);

	/* Confidence */
	double confidence = (perfDiag.BlockErrorCnt == 0 ? 100 : (double)(100.0 - (100.0 * (double)perfDiag.BlockErrorCnt / (double)perfDiag.TotalTxBlockCnt)));
	SL_Printf("\n CONF              : %.2f %%\n\n", confidence);
}

