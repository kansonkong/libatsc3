/*****************************************************************************/
/*    Saankhya Confidential                                                  */
/*    COPYRIGHT (C) 2020 Saankhya Labs Pvt. Ltd - All Rights Reserved        */
/*                                                                           */
/* Saankhya Labs makes no warranty express or implied including but not      */
/* limited to, any warranty of merchantability or fitness for a particular   */
/* purpose and/or requirements, for a particular purpose in relation to the  */
/* LICENSED MATERIALS, which is provided �AS IS�, WITH ALL FAULTS            */
/*                                                                           */
/* Saankhya Labs does not represent or warrant that the LICENSED MATERIALS   */
/* provided    here under is free of infringement of any third party patents,*/
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
/* File Name    :   atsc3.c                                                  */
/*  version     :   0.10                                                      */
/*  Date        :   24/11/2021                                               */
/*  Description :   Atsc3 test code Implementation                           */
/*                                                                           */
/*****************************************************************************/
#include <math.h>
#include "atsc3.h"
//#include "CircularBuffer.h"
#include "../../brType.h"
/* Defines */
#define DIAG_LOC             DIAG_LOC_FILE
#define DIAG_LOC_CONSOLE     (0)                 // Diagnostics Logs to Console
#define DIAG_LOC_FILE        (1)                 // Diagnostics Logs to File LogDiag.txt
#define CB_SIZE              (16*1024*12*1024)   // Global  circular buffer size
#define MAX_BBC_BUFFER_SIZE  (16*1024*12*100)    // Max buffer size when Base band capture enabled
#define MAX_ALP_BUFFER_SIZE  (65536)             // Max buffer size when ALP is enabled
#define IF_OFFSET            (0.003453)          // User can Update as needed
#define TUNER_FREQUENCY      (473000000)         // Frequency in HERTZ

#define PROCESS_TLV_DATA     (0)                 // Parse TLV packet if 1: Process TLV Packet and Produce ALP Data 
//             else if 0: Dump TLV packet to sl-tlv.bin file

#if PROCESS_TLV_DATA  
#define TLV_HEADER_LENGTH   (188)                // TLV Packet Header length 188 bytes. It repeats every TLV packet
#define TLV_MAX_LENGTH      (188 * 10)           // Max Packet size of each TLV packet 
#endif

/* Global Variables */
//static CircularBuffer            cb;
static unsigned long int         pThread, cThread, dThread;
static void                      *fp;
static int                       tUnit;
static int                       slUnit;
static unsigned char             buffer[MAX_BBC_BUFFER_SIZE];
static double                    snr;
static volatile int              processFlag;
static volatile int              diagFlag;
static double                    fer = 0;
static double                    ber = 0;
static int                       demodStartStatus = 0;
static int                       demodLockStatus = 0;
static SL_Atsc3p0LlsPlpInfo_t    llsPlpInfo = 0;
static SL_Atsc3p0LlsPlpInfo_t    llsPlpMask = 0x1;
static int                       plpInfoVal = 0, plpllscount = 0;
static unsigned int              tunerFrequency;
static SL_Atsc3p0Perf_Diag_t     perfDiag;
static SL_Atsc3p0Bsr_Diag_t      bsrDiag;
static SL_Atsc3p0L1B_Diag_t      l1bDiag;
static SL_Atsc3p0L1D_Diag_t      l1dDiag;
static SL_DemodConfigInfo_t      cfgInfo;
static SL_Atsc3p0ConfigParams_t  atsc3ConfigInfo;
static SL_BbCapture_t            setbbValue, getbbValue;
static SL_CmdControlIf_t         cmdIf;
static SL_PlatFormConfigParams_t getPlfConfig;//= SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
static SL_TunerDcOffSet_t        tunerIQDcOffSet;
volatile int                     isCaptureThreadRunning = 0, isProcessThreadRunning = 0, isDiagThreadRunning = 0;

/* Static function prototypes */
static void   processData(unsigned int size);
//static void   RxDataCallback(unsigned char *data, long len);
static void   printToConsoleDemodConfiguration(SL_DemodConfigInfo_t cfgInfo);
static void   printToConsoleDemodAtsc3Configuration(SL_Atsc3p0ConfigParams_t atsc3Info);
static void   logDemodConfiguration(SL_DemodConfigInfo_t cfgInfo);
//static void   logDemodAtsc3Configuration(SL_Atsc3p0ConfigParams_t atsc3Info);
static void   printToConsoleTunerError(SL_TunerResult_t err);
static void   printToConsoleDemodError(SL_Result_t err);
static void   handleCmdIfFailure(void);
static void   captureFrameandBitErrorRate(void);
static double GetDemodBitRate(int L1SamplePerFrame, int L1FrameCount, int PLPxByteCount);
static void   captureStreamandChannelBitRate(void);
static void   printToConsolePlfConfiguration(SL_PlatFormConfigParams_t cfgInfo);
static void   logPlfConfiguration(SL_PlatFormConfigParams_t cfgInfo);
//static int    CaptureThread();
//static int    ProcessThread();
static int    DiagnosticThread();
static int    manualplpConfig(int plpMask, SL_Atsc3p0ConfigParams_t*);

#if PROCESS_TLV_DATA 
static void   processALP(unsigned char plpId, unsigned char *buffer, unsigned int alpLen);
#endif

int manualplpConfig(char plpMask, SL_Atsc3p0ConfigParams_t* pAtsc3ConfigInfo, char plpID[4])
{
    SL_Result_t sRes = SL_OK;
    switch (plpMask)
    {
    case 0:
        pAtsc3ConfigInfo->plpConfig.plp0 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = (char)0xFF;
        break;
    case 1:
        pAtsc3ConfigInfo->plpConfig.plp0 = plpID[0]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = (char)0xFF;
        break;
    case 2:
        pAtsc3ConfigInfo->plpConfig.plp0 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = plpID[1]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = (char)0xFF;
        break;
    case 3:
        pAtsc3ConfigInfo->plpConfig.plp0 = plpID[0]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = plpID[1]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = (char)0xFF;
        break;
    case 4:
        pAtsc3ConfigInfo->plpConfig.plp0 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = plpID[2]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = (char)0xFF;
        break;
    case 5:
        pAtsc3ConfigInfo->plpConfig.plp0 = plpID[0]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = plpID[2]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = (char)0xFF;
        break;
    case 6:
        pAtsc3ConfigInfo->plpConfig.plp0 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = plpID[1]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = plpID[2]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = (char)0xFF;
        break;
    case 7:
        pAtsc3ConfigInfo->plpConfig.plp0 = plpID[0]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = plpID[1]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = plpID[2]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = (char)0xFF;
        break;
    case 8:
        pAtsc3ConfigInfo->plpConfig.plp0 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = plpID[3]&0xFF;
        break;
    case 9:
        pAtsc3ConfigInfo->plpConfig.plp0 = plpID[0]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = plpID[3]&0xFF;
        break;
    case 10:
        pAtsc3ConfigInfo->plpConfig.plp0 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = plpID[1]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = plpID[3]&0xFF;
        break;
    case 11:
        pAtsc3ConfigInfo->plpConfig.plp0 = plpID[0]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = plpID[1]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = plpID[3]&0xFF;
        break;
    case 12:
        pAtsc3ConfigInfo->plpConfig.plp0 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = plpID[2]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = plpID[3]&0xFF;
        break;
    case 13:
        pAtsc3ConfigInfo->plpConfig.plp0 = plpID[0]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = plpID[2]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = plpID[3]&0xFF;
        break;
    case 14:
        pAtsc3ConfigInfo->plpConfig.plp0 = (char)0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = plpID[1]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = plpID[2]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = plpID[3]&0xFF;
        break;
    case 15:
        pAtsc3ConfigInfo->plpConfig.plp0 = plpID[0]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp1 = plpID[1]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp2 = plpID[2]&0xFF;
        pAtsc3ConfigInfo->plpConfig.plp3 = plpID[3]&0xFF;
        break;
    default:
        SL_Printf("\n Invalid PLP number\n");
        sRes = SL_ERR_INVALID_ARGUMENTS;
        break;
    }
	printf("pAtsc3ConfigInfo->plpConfig.plp0=0x%x\n",pAtsc3ConfigInfo->plpConfig.plp0);	
	printf("pAtsc3ConfigInfo->plpConfig.plp1=0x%x\n",pAtsc3ConfigInfo->plpConfig.plp1);	
	printf("pAtsc3ConfigInfo->plpConfig.plp2=0x%x\n",pAtsc3ConfigInfo->plpConfig.plp2);	
	printf("pAtsc3ConfigInfo->plpConfig.plp3=0x%x\n",pAtsc3ConfigInfo->plpConfig.plp3);	  
    return sRes;
}







SL_Result_t SL_ATSC3_WriteBytes(unsigned int address, unsigned int lenBytes, void *buf)
{
	//SL_Result_t retVal = SL_OK;
	if (lenBytes >= 4)
		return SL_WriteBytes(slUnit, address, lenBytes, (unsigned int*)buf);
	else
		return SL_ERR_INVALID_ARGUMENTS;


}
SL_Result_t SL_ATSC3_ReadBytes(unsigned int address, unsigned int lenBytes, void *buf)
{
	if (lenBytes >= 4)
		return SL_ReadBytes(slUnit, address, lenBytes, (unsigned int*)buf);
	else
		return SL_ERR_INVALID_ARGUMENTS;

}



SL_Result_t SL3000_atsc3_init(Endeavour  *endeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *sPlfConfig, SL_DemodStd_t std)
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
	sPlfConfig->tunerType = TUNER_R855;//TUNER_NXP;
	sPlfConfig->demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
	sPlfConfig->demodOutputIf = SL_DEMOD_OUTPUTIF_TS;

	sPlfConfig->demodI2cAddr = SL_DEMOD_I2C_ADDR; /* SLDemod 7-bit Physical I2C Address */

	//sPlfConfig.slsdkPath = "..";
	sPlfConfig->slsdkPath = "";


	//int						slUnit;

	SL_Printf("\n SL3000_atsc1_tune Start!!");
	/* Tuner Config */
	pTunerCfg->bandwidth = SL_TUNER_BW_6MHZ;

	/* Set Configuration Parameters */
	SL_ConfigSetPlatform(*sPlfConfig);

	SL_Printf("\n Running ATSC3.0 Test...");
	//nerFrequency = TUNER_FREQUENCY;
	setbbValue = BB_CAPTURE_DISABLE;

	cres = SL_ConfigGetPlatform(sPlfConfig);
	if (cres == SL_CONFIG_OK)
	{
		printToConsolePlfConfiguration(*sPlfConfig);
	}
	else
	{
		SL_Printf("\n ERROR : SL_ConfigGetPlatform Failed ");
	}


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

	afeInfo.spectrum = SL_SPECTRUM_INVERTED;
	afeInfo.iftype = SL_IFTYPE_LIF;
	afeInfo.ifreq = 5 + IF_OFFSET;
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


	cres = SL_ConfigGetBbCapture(&getbbValue);
	if (cres != SL_CONFIG_OK)
	{
		SL_Printf("\n ERROR : SL_ConfigGetPlatform Failed ");
		//goto TEST_ERROR;
	}

	/* ATSC3 Configuration */
	if (getbbValue)
	{
		atsc3ConfigInfo.plpConfig.plp0 = 0x00;
	}
	else
	{
		atsc3ConfigInfo.plpConfig.plp0 = 0xFF;
	}
	atsc3ConfigInfo.plpConfig.plp0 = 0x0;
	atsc3ConfigInfo.plpConfig.plp1 = 0xFF;
	atsc3ConfigInfo.plpConfig.plp2 = 0xFF;
	atsc3ConfigInfo.plpConfig.plp3 = 0xFF;
	atsc3ConfigInfo.region = SL_ATSC3P0_REGION_US;
	SL_Printf("\n atsc3ConfigInfo.plpConfig.plp0=%d\n", atsc3ConfigInfo.plpConfig.plp0);

	slres = SL_DemodCreateInstance(&slUnit);
	if (slres != SL_OK)
	{
		SL_Printf("\n Error:SL_DemodCreateInstance :");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}

	SL_Printf("\n Initializing SL Demod..: ");

	slres = SL_DemodInit(slUnit, cmdIf, SL_DEMODSTD_ATSC3_0);
	if (slres != SL_OK)
	{
		SL_Printf("FAILED");
		SL_Printf("\n Error:SL_DemodInit :");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}
	else
	{
		SL_Printf("SUCCESS");
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

	slres = SL_DemodConfigureEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
	if (slres != 0)
	{
		SL_Printf("\n Error:SL_DemodConfigureEx :");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}

	slres = SL_DemodGetSoftwareVersion(slUnit, &swMajorNo, &swMinorNo);
	if (slres == SL_OK)
	{
		SL_Printf("\n Demod SW Version		 : %d.%d", swMajorNo, swMinorNo);
	}

	/* Tuner Config */
	tunerCfg.bandwidth = SL_TUNER_BW_6MHZ;
	tunerCfg.std = SL_TUNERSTD_ATSC3_0;



	slres = SL_DemodStart(slUnit);

	if (slres != 0)
	{
		SL_Printf("\n Saankhya Demod Start Failed");
		//goto TEST_ERROR;
	}
	else
	{
		demodStartStatus = 1;
		SL_Printf("SUCCESS");
		SL_Printf("\n SL Demod Output Capture: STARTED : sl-tlv.bin");
	}
	SL_SleepMS(1000); // Delay to accomdate set configurations at SL to take effect
	slres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_PLP_LLS_INFO, &llsPlpInfo);
	if (slres != SL_OK)
	{
		SL_Printf("\n Error: ATSC3 Get LlsPlp List :");
		printToConsoleDemodError(slres);
		if (slres == SL_ERR_CMD_IF_FAILURE)
		{
			handleCmdIfFailure();
			//goto TEST_ERROR;
		}
	}

	plpllscount = 0;
	plpInfoVal = 1;
	for (int plpIndx = 0; (plpIndx < 64) && (plpllscount < 4); plpIndx++)
	{
		//plpInfoVal = ((llsPlpInfo & (llsPlpMask << plpIndx)) == pow(2.0, plpIndx)) ? 0x01 : 0xFF;
		SL_Printf("\n [plpIndx(%d)]plpllscount=%d  plpInfoVal=%d\n", plpIndx, plpllscount, plpInfoVal);
		if (plpInfoVal == 0x01)
		{
			plpllscount++;
			if (plpllscount == 1)
			{
				atsc3ConfigInfo.plpConfig.plp0 = (char)plpIndx;
			}
			else if (plpllscount == 2)
			{
				atsc3ConfigInfo.plpConfig.plp1 = (char)plpIndx;
			}
			else if (plpllscount == 3)
			{
				atsc3ConfigInfo.plpConfig.plp2 = (char)plpIndx;
			}
			else if (plpllscount == 4)
			{
				atsc3ConfigInfo.plpConfig.plp3 = (char)plpIndx;
			}
			else
			{
				plpllscount++;
			}
		}
	}

	if (atsc3ConfigInfo.plpConfig.plp0 ==  (char)0xFF)
	{
		atsc3ConfigInfo.plpConfig.plp0 = (char)0x00;
	}

	return slres;
}


SL_Result_t SL3000_atsc3_tune(SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *sPlfConfig)
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
		SL_Printf("\n Error:SL_Demod Get ATSC3 Configuration :");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}

	slres = SL_DemodGetConfiguration(slUnit, &cfgInfo);
	if (slres != SL_OK)
	{
		SL_Printf("\n Error:SL_Demod Get Configuration :");
		printToConsoleDemodError(slres);
		if (slres == SL_ERR_CMD_IF_FAILURE)
		{
			handleCmdIfFailure();
			//goto TEST_ERROR;
		}
	}
	else
	{
            SL_Result_t slres = SL_DemodGetConfigurationEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
            if (slres != SL_OK)
            {
                SL_Printf("\n Error:SL_Demod Get ConfigurationEX :");
                printToConsoleDemodError(slres);
                if (slres == SL_ERR_CMD_IF_FAILURE)
                {
                    handleCmdIfFailure();

                }
            }	
		printToConsoleDemodConfiguration(cfgInfo);
		printToConsoleDemodAtsc3Configuration(atsc3ConfigInfo);
	}

	//SL_Printf("\n\n Show SL Diagnostics    : [y/n] :");

	return slres;
}
#if 0
//plpIDNum Number of valid PLP ID in plpID[]. (1 - 4)
SL_Result_t SL3000_atsc3_setPLP(char plpIDNum, char* plpID)
{
	SL_Result_t slres = SL_OK;
#if 0
	for(int i=0;i<4;i++)
		SL_Printf("\n plpID[%d]=%d\n",i,plpID[i]);
#endif
	switch (plpIDNum)
	{
	case 1:
		atsc3ConfigInfo.plpConfig.plp0 = plpID[0] & 0xFF;
		atsc3ConfigInfo.plpConfig.plp1 = 0xFF;
		atsc3ConfigInfo.plpConfig.plp2 = 0xFF;
		atsc3ConfigInfo.plpConfig.plp3 = 0xFF;
		break;
	case 2:
		atsc3ConfigInfo.plpConfig.plp0 = plpID[0] & 0xFF;
		atsc3ConfigInfo.plpConfig.plp1 = plpID[1] & 0xFF;
		atsc3ConfigInfo.plpConfig.plp2 = 0xFF;
		atsc3ConfigInfo.plpConfig.plp3 = 0xFF;
		break;
	case 3:
		atsc3ConfigInfo.plpConfig.plp0 = plpID[0] & 0xFF;
		atsc3ConfigInfo.plpConfig.plp1 = plpID[1] & 0xFF;
		atsc3ConfigInfo.plpConfig.plp2 = plpID[2] & 0xFF;
		atsc3ConfigInfo.plpConfig.plp3 = 0xFF;
		break;
	case 4:
		atsc3ConfigInfo.plpConfig.plp0 = plpID[0] & 0xFF;
		atsc3ConfigInfo.plpConfig.plp1 = plpID[1] & 0xFF;
		atsc3ConfigInfo.plpConfig.plp2 = plpID[2] & 0xFF;
		atsc3ConfigInfo.plpConfig.plp3 = plpID[3] & 0xFF;
		break;
	default:
		slres = SL_ERR_INVALID_ARGUMENTS;
		break;
	}


	slres = SL_DemodConfigureEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
	if (slres != 0)
	{
		SL_Printf("\n Error:SL_Demod Get ATSC3 Configuration :");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}

	return slres;
}
#endif
SL_Result_t SL3000_atsc3_setPLP(char plpMask, char plpID[4])
{
    SL_Result_t sRes = SL_OK;
#if 0
for(int i=0;i<4;i++)
	 SL_Printf("\n plpID[%d]=%d\n",i,plpID[i]);
#endif	
	//sRes= (SL_Result_t)manualplpConfig(plpMask, &atsc3ConfigInfo);
	int Res=manualplpConfig(plpMask, &atsc3ConfigInfo,plpID);

	sRes = SL_DemodConfigureEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
	if (sRes != 0)
	{
		SL_Printf("\n Error:SL_Demod Get ATSC3 Configuration :");
		printToConsoleDemodError(sRes);
		//goto TEST_ERROR;
	}

	return sRes;

}

//have some bugs.
SL_Result_t SL3000_atsc3_getPLP(LockStatus_t plpIDVaild[4])
{

SL_Result_t slres = SL_OK;
SL_DemodLockStatus_t lockStatus;
SL_DemodLockStatus_t rfLock = LOCKED;
int loopCount = 0;
int 				 cpuStatus = 0;
do
{
	slres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_LOCK, (SL_DemodLockStatus_t*)&lockStatus);
	if (slres != SL_OK)
	{
		SL_Printf("\n Error:SL_Demod Get Lock Status				:");
		printToConsoleDemodError(slres);
	}
	else
	{
		SL_Printf("\n\n ATSC3 Demod Lock Status");


		if ((lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_RF_LOCK))
		{
			SL_Printf("\n RF				: Locked");
			demodLockStatus = LOCKED;
			

		}
		else
		{
			SL_Printf("\n RF				: Unlocked");
			demodLockStatus = UNLOCKED;

		}
		rfLock = demodLockStatus;
		SL_Printf("\n Baseband Lock Status");
		if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP0_LOCK)
		{
			SL_Printf("\n PLP0				: Locked");
			plpIDVaild[0] = LOCKED;
		}
		else
		{
			SL_Printf("\n PLP0				: Unlocked");
			plpIDVaild[0] = UNLOCKED;

		}

		if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP1_LOCK)
		{
			SL_Printf("\n PLP1				: Locked");
			plpIDVaild[1] = LOCKED;

		}
		else
		{
			SL_Printf("\n PLP1				: Unlocked");
			plpIDVaild[1] = UNLOCKED;
		}

		if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP2_LOCK)
		{
			SL_Printf("\n PLP2				: Locked");
			plpIDVaild[2] = LOCKED;

		}
		else
		{
			SL_Printf("\n PLP2				: Unlocked");
			plpIDVaild[2] = UNLOCKED;
		}

		if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP3_LOCK)
		{
			SL_Printf("\n PLP3				: Locked");
			plpIDVaild[3] = LOCKED;

		}
		else
		{
			SL_Printf("\n PLP3				: Unlocked");
			plpIDVaild[3] = UNLOCKED;
		}
	}
	loopCount++;
	usleep(1000);
	slres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_CPU, (int*)&cpuStatus);

	if (slres != SL_OK)
	{
		SL_Printf("\n Error:SL_Demod Get CPU Status 				:");
		printToConsoleDemodError(slres);
	}
	else
	{
		SL_Printf("\n Demod CPU Status	: %s", (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED");
	}

	if (demodLockStatus == 1)
	{
		slres= SL_DemodGetConfiguration(slUnit, &cfgInfo);
		if (slres != SL_OK)
		{
			SL_Printf("\n Error:SL_Demod Get Configuration :");
			printToConsoleDemodError(slres);
			if (slres == SL_ERR_CMD_IF_FAILURE)
			{
				handleCmdIfFailure();
			}
		}

		if (cfgInfo.extLnaMode != SL_EXT_LNA_CFG_MODE_NOT_PRESENT)
		{
			switch ((cfgInfo.extLnaMode >> 16) & 0x00000001)
		 	{
			case 0:
				SL_Printf("\n Current LNA status:  External LNA Bypassed ");
				break;

			case 1:
				SL_Printf("\n Current LNA status:  External LNA Enabled");
				break;

			default:
				SL_Printf("\n Current LNA status:  Invalid");
				break;
			}
		}
	}
	if (demodStartStatus && (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_RF_LOCK))
	{
		slres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_PERF, (SL_Atsc3p0Perf_Diag_t*)&perfDiag);
		if (slres != SL_OK)
		{
			SL_Printf("\n Error getting ATSC3.0 Performance Diagnostics :");
			printToConsoleDemodError(slres);
		}
		else
		{
			snr = (float)perfDiag.GlobalSnrLinearScale / 16384;
			snr = 10.0 * log10(snr);
			SL_Printf("\n SNR				:  %3.2f dB\n", snr);

			captureFrameandBitErrorRate();
			captureStreamandChannelBitRate();

		}

		slres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_BSR, (SL_Atsc3p0Bsr_Diag_t*)&bsrDiag);
		if (slres != SL_OK)
		{
			SL_Printf("\n Error getting ATSC3.0 Boot Strap Diagnostics	:");
			printToConsoleDemodError(slres);
		}


		slres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_L1B, (SL_Atsc3p0L1B_Diag_t*)&l1bDiag);
		if (slres != SL_OK)
		{
			SL_Printf("\n Error getting ATSC3.0 L1B Diagnostics 		:");
			printToConsoleDemodError(slres);
		}


		slres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_L1D, (SL_Atsc3p0L1D_Diag_t*)&l1dDiag);
		if (slres != SL_OK)
		{
			SL_Printf("\n Error getting ATSC3.0 L1D Diagnostics 		:");
			printToConsoleDemodError(slres);
		}

	}


} while (rfLock == UNLOCKED && loopCount < 3);
return slres;



}




void SL3000_ATSC3_Initialize(Endeavour	*pEndeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *pPlfConfig, R855_Set_Info *pR855_Info)
{
	SL_Result_t slres = SL_OK;

	Init_R855(pEndeavour, pR855_Info);
	pTunerCfg->std = SL_TUNERSTD_ATSC3_0;//SL_TUNERSTD_ATSC3_0;
	slres = SL3000_atsc3_init(pEndeavour, pTunerCfg, pPlfConfig, SL_DEMODSTD_ATSC3_0);
	if (slres != SL_OK)
	{
		SL_Printf("\n SL3000_atsc3_init failed");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}

}
void SL3000_ATSC3_BindTune(SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *pPlfConfig, R855_Set_Info *pR855_Info)
{
	SL_Result_t slres = SL_OK;

	//IF AGC select
	pR855_Info->R855_IfAgc_Select = R855_IF_AGC1;
	Tune_R855(pR855_Info);
	
	char plpID[4]={0x0,0x1,0x2,0x3};
	char plpMask=0x0F;
	SL3000_atsc3_setPLP(plpMask,plpID);
	slres = SL3000_atsc3_tune(pTunerCfg, pPlfConfig);
	if (slres != SL_OK)
	{
		SL_Printf("\n SL3000_atsc3_tune failed");
		printToConsoleDemodError(slres);
		//goto TEST_ERROR;
	}

}

void Monitor_SL3000_ATSC3_Signal(SignalInfo_t *pSigInfo, int freq, R855_Standard_Type RT_Standard)
{

	SL_TunerResult_t     tres;
	//SL_TunerSignalInfo_t tunerInfo;
	SL_DemodLockStatus_t lockStatus;
	int                  cpuStatus = 0;
	SL_Result_t          dres;
	static unsigned int  loopCounter = 0;
	int                  lockStatusFlag = 0;
	int i = 0;
	//while (i < 10)
	{

		//i++;
		dres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_LOCK, (SL_DemodLockStatus_t*)&lockStatus);
		if (dres != SL_OK)
		{
//			SL_Printf("\n Error:SL_Demod Get Lock Status                :");
			printToConsoleDemodError(dres);
		}
		else
		{
//			SL_Printf("\n\n ATSC3 Demod Lock Status");


			if ((lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_RF_LOCK))
			{
//				SL_Printf("\n RF                : Locked");
				pSigInfo->locked = LOCKED;
				demodLockStatus = 1;

			}
			else
			{
//				SL_Printf("\n RF                : Unlocked");
				pSigInfo->locked = UNLOCKED;
				demodLockStatus = 0;

			}

			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1B_LOCK)
			{
//				SL_Printf("\n L1B               : Locked");

			}
			else
			{
//				SL_Printf("\n L1B               : Unlocked");

			}

			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1D_LOCK)
			{
//				SL_Printf("\n L1D               : Locked");
			}
			else
			{
//				SL_Printf("\n L1D               : Unlocked");

			}

//			SL_Printf("\n Baseband Lock Status");
			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP0_LOCK)
			{
//				SL_Printf("\n PLP0              : Locked");
				pSigInfo->plpVaild[0] = LOCKED;
			}
			else
			{
//				SL_Printf("\n PLP0              : Unlocked");
				pSigInfo->plpVaild[0] = UNLOCKED;
			}

			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP1_LOCK)
			{
//				SL_Printf("\n PLP1              : Locked");
				pSigInfo->plpVaild[1] = LOCKED;

			}
			else
			{
//				SL_Printf("\n PLP1              : Unlocked");
				pSigInfo->plpVaild[1] = UNLOCKED;

			}

			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP2_LOCK)
			{
//				SL_Printf("\n PLP2              : Locked");
				pSigInfo->plpVaild[2] = LOCKED;

			}
			else
			{
//				SL_Printf("\n PLP2              : Unlocked");
				pSigInfo->plpVaild[2] = UNLOCKED;

			}

			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP3_LOCK)
			{
//				SL_Printf("\n PLP3              : Locked");
				pSigInfo->plpVaild[3] = LOCKED;

			}
			else
			{
//				SL_Printf("\n PLP3              : Unlocked");
				pSigInfo->plpVaild[3] = UNLOCKED;

			}
		}

		R855_GetTotalRssi(freq, RT_Standard, &pSigInfo->rssi);
//		SL_Printf("\n Tuner RSSI		: %d dBm", pSigInfo->rssi);


		dres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_CPU, (int*)&cpuStatus);

		if (dres != SL_OK)
		{
			SL_Printf("\n Error:SL_Demod Get CPU Status                 :");
			printToConsoleDemodError(dres);
		}
		else
		{
			SL_Printf("\n Demod CPU Status  : %s", (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED");
		}
       // if (pSigInfo->locked ==LOCKED)
       if(demodLockStatus == 1)
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
                    SL_Printf("\n Current LNA status:  External LNA Bypassed ");
                    break;

                case 1:
                    SL_Printf("\n Current LNA status:  External LNA Enabled");
                    break;

                default:
                    SL_Printf("\n Current LNA status:  Invalid");
                    break;
                }
            }
        }

//		if (demodStartStatus} && (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_RF_LOCK))
		if ( (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_RF_LOCK))
		{
			dres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_PERF, (SL_Atsc3p0Perf_Diag_t*)&perfDiag);
			if (dres != SL_OK)
			{
//				SL_Printf("\n Error getting ATSC3.0 Performance Diagnostics :");
				printToConsoleDemodError(dres);
			}
			else
			{
				snr = (float)perfDiag.GlobalSnrLinearScale / 16384;
				pSigInfo->snr = 10.0 * log10(snr);
//				SL_Printf("\n SNR               : %3.2f dB\n\n", pSigInfo->snr);

				captureFrameandBitErrorRate();
				captureStreamandChannelBitRate();
				//printAtsc3PerfDiagnostics(perfDiag, DIAG_LOC);
			}

			dres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_BSR, (SL_Atsc3p0Bsr_Diag_t*)&bsrDiag);
			if (dres != SL_OK)
			{
				SL_Printf("\n Error getting ATSC3.0 Boot Strap Diagnostics  :");
				printToConsoleDemodError(dres);
			}
			else
			{
				//printAtsc3BsrDiagnostics(bsrDiag, DIAG_LOC);
			}

			dres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_L1B, (SL_Atsc3p0L1B_Diag_t*)&l1bDiag);
			if (dres != SL_OK)
			{
				SL_Printf("\n Error getting ATSC3.0 L1B Diagnostics         :");
				printToConsoleDemodError(dres);
			}
			else
			{
				//printAtsc3L1bDiagnostics(l1bDiag, DIAG_LOC);
			}

			dres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_L1D, (SL_Atsc3p0L1D_Diag_t*)&l1dDiag);
			if (dres != SL_OK)
			{
				SL_Printf("\n Error getting ATSC3.0 L1D Diagnostics         :");
				printToConsoleDemodError(dres);
			}
			else
			{
				//printAtsc3L1dDiagnostics(l1bDiag.L1bNoOfSubframes, l1dDiag, DIAG_LOC);
				//printAtsc3SignalDetails(l1bDiag.L1bNoOfSubframes, l1dDiag, DIAG_LOC);
			}

			if (dres == SL_ERR_CMD_IF_FAILURE)
			{
				//handleCmdIfFailure();
			}
		}

//		SL_Printf("\n Press any key to exit..");
//		SL_SleepMS(1000); //Read Diagnostics every second


	}

}

/*
 *  ATSC3.0 Main Test function
 */
void atsc3_test(Endeavour			*endeavour, SL_TunerConfig_t *pTunerCfg, unsigned int              tunerFrequency)
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
	SL_IQOffsetCorrectionParams_t iqOffSetCorrection;
	SL_DemodBootStatus_t bootStatus;
    SL_ExtLnaConfigParams_t       lnaInfo;

	getPlfConfig.chipType = SL_CHIP_3000;
	getPlfConfig.chipRev = SL_CHIP_REV_BB;
	getPlfConfig.boardType = SL_EVB_3000;//SL_KAILASH_DONGLE_3;// SL_EVB_3000;
	getPlfConfig.tunerType = TUNER_NXP;//TUNER_NXP;
	getPlfConfig.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
	getPlfConfig.demodOutputIf = SL_DEMOD_OUTPUTIF_TS;

	getPlfConfig.demodI2cAddr = SL_DEMOD_I2C_ADDR; /* SLDemod 7-bit Physical I2C Address */
#ifdef SL_FX3S
	getPlfConfig.demodResetGpioPin = 47; 	 /* FX3S GPIO 47 connected to Demod Reset Pin */
	getPlfConfig.cpldResetGpioPin = 43;		 /* FX3S GPIO 43 connected to CPLD Reset Pin and used only for serial TS Interface	*/
	getPlfConfig.demodI2cAddr3GpioPin = 37;	 /* FX3S GPIO 37 connected to Demod I2C Address3 Pin and used only for SDIO Interface */
	getPlfConfig.demodBootMode1GpioPin = 50;   /* FX3S GPIO 50 connected to Demod Boot Mode1 Pin and used only for SDIO Interface */
	getPlfConfig.tunerResetGpioPin = 23;    /* FX3S GPIO 23 connected to Tuner Reset Pin */
#endif
	//sPlfConfig.slsdkPath = "..";
	getPlfConfig.slsdkPath = "";


	//int						slUnit;

	SL_Printf("\n SL3000_atsc1_tune Start!!");
	/* Tuner Config */
	pTunerCfg->bandwidth = SL_TUNER_BW_6MHZ;

	/* Set Configuration Parameters */
	SL_ConfigSetPlatform(getPlfConfig);


	SL_Printf("\n Running ATSC3.0 Test...");
	//nerFrequency = TUNER_FREQUENCY;
	setbbValue = BB_CAPTURE_DISABLE;

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


	//cres = SL_ConfigSetBbCapture(setbbValue);
	if (cres != SL_CONFIG_OK)
	{
		SL_Printf("\n ERROR : SL_ConfigSetBbCapture Failed ");
		goto TEST_ERROR;
	}

	if (getPlfConfig.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
	{
		uint8_t			sl3000_i2cBus = 3;
		i2cres = SL_I2cInit(endeavour, sl3000_i2cBus);
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
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 5 + IF_OFFSET;
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
			afeInfo.ifreq = 4.4;
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
			//SL_SleepMS(100); // 100ms delay for Toggle
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
			afeInfo.ifreq = 4.4 + IF_OFFSET;
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
			afeInfo.ifreq = 4.4;
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
			outPutInfo.oif = SL_OUTPUTIF_TSSERIAL_MSB_FIRST;
			/* CPLD Reset */
			//SL_GpioSetPin(getPlfConfig.cpldResetGpioPin, 0x00); // Low
			// SL_SleepMS(100); // 100ms delay for Toggle
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
	case SL_KAILASH_DONGLE_2:
		if (getPlfConfig.tunerType == TUNER_SI || getPlfConfig.tunerType == TUNER_SI_P)
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
			afeInfo.ifreq = 4.4;
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
#if 0
	case SL_SILISA_DONGLE:
		if (getPlfConfig.tunerType == TUNER_SILABS)
		{
			afeInfo.spectrum = SL_SPECTRUM_INVERTED;
			afeInfo.iftype = SL_IFTYPE_LIF;
			afeInfo.ifreq = 4.4;
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
		break;
#endif
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
			//lnaInfo.lnaMode = (lnaInfo.lnaMode | ((SL_ExtLnaModeConfig_t)lnaInfo.lnaGpioNum << 8));
        }
		break;

	default:
		SL_Printf("\n Invalid Board Type Selected ");
		break;
	}
	afeInfo.iqswap = SL_IQSWAP_DISABLE;
	afeInfo.agcRefValue = 125; //afcRefValue in mV
	outPutInfo.TsoClockInvEnable = SL_TSO_CLK_INV_ON;

	cres = SL_ConfigGetBbCapture(&getbbValue);
	if (cres != SL_CONFIG_OK)
	{
		SL_Printf("\n ERROR : SL_ConfigGetPlatform Failed ");
		goto TEST_ERROR;
	}

	/* ATSC3 Configuration */

	if (getbbValue)
	{
		atsc3ConfigInfo.plpConfig.plp0 = (char)0x00;
	}
	else
	{
		atsc3ConfigInfo.plpConfig.plp0 = (char)0xFF;
	}
	atsc3ConfigInfo.plpConfig.plp1 = (char)0x00;
	atsc3ConfigInfo.plpConfig.plp2 = (char)0xFF;
	atsc3ConfigInfo.plpConfig.plp3 = (char)0xFF;

	atsc3ConfigInfo.region = SL_ATSC3P0_REGION_US;
	SL_Printf("\n atsc3ConfigInfo.plpConfig.plp0=%d\n", atsc3ConfigInfo.plpConfig.plp0);

	slres = SL_DemodCreateInstance(&slUnit);
	if (slres != SL_OK)
	{
		SL_Printf("\n Error:SL_DemodCreateInstance :");
		printToConsoleDemodError(slres);
		goto TEST_ERROR;
	}

	SL_Printf("\n Initializing SL Demod..: ");

	slres = SL_DemodInit(slUnit, cmdIf, SL_DEMODSTD_ATSC3_0);
	if (slres != SL_OK)
	{
		SL_Printf("FAILED");
		SL_Printf("\n Error:SL_DemodInit :");
		printToConsoleDemodError(slres);
		goto TEST_ERROR;
	}
	else
	{
		SL_Printf("SUCCESS");
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

	slres = SL_DemodConfigureEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
	if (slres != 0)
	{
		SL_Printf("\n Error:SL_DemodConfigureEx :");
		printToConsoleDemodError(slres);
		goto TEST_ERROR;
	}

	slres = SL_DemodGetSoftwareVersion(slUnit, &swMajorNo, &swMinorNo);
	if (slres == SL_OK)
	{
		SL_Printf("\n Demod SW Version       : %d.%d", swMajorNo, swMinorNo);
	}

	/* Tuner Config */
	tunerCfg.bandwidth = SL_TUNER_BW_6MHZ;
	tunerCfg.std = SL_TUNERSTD_ATSC3_0;
#if 0

	tres = SL_TunerCreateInstance(&tUnit);
	if (tres != 0)
	{
		SL_Printf("\n Error:SL_TunerCreateInstance :");
		printToConsoleTunerError(tres);
		goto TEST_ERROR;
	}

	tres = SL_TunerInit(tUnit);
	if (tres != 0)
	{
		SL_Printf("\n Error:SL_TunerInit :");
		printToConsoleTunerError(tres);
		goto TEST_ERROR;
	}

	tres = SL_TunerConfigure(tUnit, &tunerCfg);
	if (tres != 0)
	{
		SL_Printf("\n Error:SL_TunerConfigure :");
		printToConsoleTunerError(tres);
		goto TEST_ERROR;
	}

	if (getPlfConfig.boardType == SL_EVB_4000 || getPlfConfig.boardType == SL_YOGA_DONGLE || getPlfConfig.tunerType == TUNER_SI_P)
	{
		/*
		 * Apply tuner IQ offset. Relevant to SITUNE Tuner
		 */
		tunerIQDcOffSet.iOffSet = 15;
		tunerIQDcOffSet.qOffSet = 14;

		tres = SL_TunerExSetDcOffSet(tUnit, &tunerIQDcOffSet);
		if (tres != 0)
		{
			SL_Printf("\n Error:SL_TunerExSetDcOffSet :");
			printToConsoleTunerError(tres);
			goto TEST_ERROR;
		}
	}

	tres = SL_TunerSetFrequency(tUnit, tunerFrequency);
	if (tres != 0)
	{
		SL_Printf("\n Error:SL_TunerSetFrequency :");
		printToConsoleTunerError(tres);
		goto TEST_ERROR;
	}

	tres = SL_TunerGetConfiguration(tUnit, &tunerGetCfg);
	if (tres != 0)
	{
		SL_Printf("\n Error:SL_TunerGetConfiguration :");
		printToConsoleTunerError(tres);
		goto TEST_ERROR;
	}
	else
	{
		if (tunerGetCfg.std == SL_TUNERSTD_ATSC3_0)
		{
			SL_Printf("\n Tuner Config Std       : ATSC3.0");
		}
		else
		{
			SL_Printf("\n Tuner Config Std       : NA");
		}
		switch (tunerGetCfg.bandwidth)
		{
		case SL_TUNER_BW_6MHZ:
			SL_Printf("\n Tuner Config Bandwidth : 6MHz");
			break;

		case SL_TUNER_BW_7MHZ:
			SL_Printf("\n Tuner Config Bandwidth : 7MHz");
			break;

		case SL_TUNER_BW_8MHZ:
			SL_Printf("\n Tuner Config Bandwidth : 8MHz");
			break;

		default:
			SL_Printf("\n Tuner Config Bandwidth : NA");
		}
	}

	tres = SL_TunerGetFrequency(tUnit, &cFrequency);
	if (tres != 0)
	{
		SL_Printf("\n Error:SL_TunerGetFrequency :");
		printToConsoleTunerError(tres);
		goto TEST_ERROR;
	}
	else
	{
		SL_Printf("\n Tuner Locked Frequency : %dHz", cFrequency);
	}

	tres = SL_TunerGetStatus(tUnit, &tunerInfo);
	if (tres != 0)
	{
		SL_Printf("\n Error:SL_TunerGetStatus :");
		printToConsoleTunerError(tres);
		goto TEST_ERROR;
	}
	else
	{
		SL_Printf("\n Tuner Lock status      : ");
		SL_Printf((tunerInfo.status == 1) ? "LOCKED" : "NOT LOCKED");
		SL_Printf("\n Tuner RSSI             : %3.2f dBm", tunerInfo.signalStrength);
	}


	utilsres = SL_CreateThread(&pThread, &ProcessThread);
	if (utilsres != SL_UTILS_OK)
	{
		processFlag = 0;
		SL_Printf("\n Process Thread launched unsuccessfully");
		goto TEST_ERROR;
	}
	else
	{
		processFlag = 1;
		if (SL_SetThreadPriority(&pThread, SL_THREAD_PRIORITY_NORMAL) != SL_UTILS_OK)
		{
			SL_Printf("\n Set Thread Priority of Process Thread Failed");
		}
	}
	utilsres = SL_CreateThread(&cThread, &CaptureThread);
	if (utilsres != SL_UTILS_OK)
	{
		SL_Printf("\n Capture Thread launched unsuccessfully");
		goto TEST_ERROR;
	}



	while (SL_IsRxDataStarted() != 1)
	{
		SL_SleepMS(100);
	}
	SL_SleepMS(500);
	SL_Printf("\n Starting SLDemod: ");
#endif

	slres = SL_DemodStart(slUnit);

	if (slres != 0)
	{
		SL_Printf("\n Saankhya Demod Start Failed");
		goto TEST_ERROR;
	}
	else
	{
		demodStartStatus = 1;
		SL_Printf("SUCCESS");
		SL_Printf("\n SL Demod Output Capture: STARTED : sl-tlv.bin");
	}
	SL_SleepMS(1000); // Delay to accomdate set configurations at SL to take effect
	//if (!getbbValue)
	{
		slres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_PLP_LLS_INFO, &llsPlpInfo);
		if (slres != SL_OK)
		{
			SL_Printf("\n Error: ATSC3 Get LlsPlp List :");
			printToConsoleDemodError(slres);
			if (slres == SL_ERR_CMD_IF_FAILURE)
			{
				handleCmdIfFailure();
				goto TEST_ERROR;
			}
		}

		plpllscount = 0;
		for (int plpIndx = 0; (plpIndx < 64) && (plpllscount < 4); plpIndx++)
		{
			//plpInfoVal = ((llsPlpInfo & (llsPlpMask << plpIndx)) == pow(2.0, plpIndx)) ? 0x01 : 0xFF;
			if (plpInfoVal == 0x01)
			{
				plpllscount++;
				if (plpllscount == 1)
				{
                        atsc3ConfigInfo.plpConfig.plp0 = (char)plpIndx;
				}
				else if (plpllscount == 2)
				{
                        atsc3ConfigInfo.plpConfig.plp1 = (char)plpIndx;
				}
				else if (plpllscount == 3)
				{
                        atsc3ConfigInfo.plpConfig.plp2 = (char)plpIndx;
				}
				else if (plpllscount == 4)
				{
                        atsc3ConfigInfo.plpConfig.plp3 = (char)plpIndx;
				}
				else
				{
					plpllscount++;
				}
			}
            }

            if (atsc3ConfigInfo.plpConfig.plp0 == (char)0xFF)
		    {
                atsc3ConfigInfo.plpConfig.plp0 = (char)0x00;
            }


		slres = SL_DemodConfigureEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
		if (slres != 0)
		{
			SL_Printf("\n Error:SL_Demod Get ATSC3 Configuration :");
			printToConsoleDemodError(slres);
			goto TEST_ERROR;
		}

		slres = SL_DemodGetConfiguration(slUnit, &cfgInfo);
		if (slres != SL_OK)
		{
			SL_Printf("\n Error:SL_Demod Get Configuration :");
			printToConsoleDemodError(slres);
			if (slres == SL_ERR_CMD_IF_FAILURE)
			{
				handleCmdIfFailure();
				goto TEST_ERROR;
			}
		}
		else
		{
            SL_Result_t slres = SL_DemodGetConfigurationEx(slUnit, SL_DEMODSTD_ATSC3_0, &atsc3ConfigInfo);
            if (slres != SL_OK)
            {
                SL_Printf("\n Error:SL_Demod Get ConfigurationEX :");
                printToConsoleDemodError(slres);
                if (slres == SL_ERR_CMD_IF_FAILURE)
                {
                    handleCmdIfFailure();
                    goto TEST_ERROR;
                }
            }
			printToConsoleDemodConfiguration(cfgInfo);
			printToConsoleDemodAtsc3Configuration(atsc3ConfigInfo);
		}

		SL_Printf("\n\n Show SL Diagnostics    : [y/n] :");
		//char ch = getchar();
		// if ((ch == 'y') || (ch == 'Y'))
		{
			utilsres = SL_CreateThread(&dThread, &DiagnosticThread);
			if (utilsres != SL_UTILS_OK)
			{
				diagFlag = 0;
				SL_Printf("\n Diagnostic Thread create failed");
				goto TEST_ERROR;
			}
			else
			{
				diagFlag = 1;
				if (SL_SetThreadPriority(&dThread, SL_THREAD_PRIORITY_LOW) != SL_UTILS_OK)
				{
					SL_Printf("\n Set Thread Priority of Diagnostic Thread Failed");
				}
			}
		}
	}
	SL_Printf("\n Press any key to exit..");
	getchar();
	getchar();
	SL_Printf("\n Releasing resources...");
	/* Stop Diagnostics Read */
	diagFlag = 0;
	while (isDiagThreadRunning)
	{
		SL_SleepMS(100); // wait for diagnostic Thread Exit
	}
	SL_DemodStop(slUnit);
	SL_DemodUnInit(slUnit);
	// SL_TunerUnInit(tUnit);
	SL_DemodDeleteInstance(slUnit);
	//SL_TunerDeleteInstance(tUnit);
	SL_I2cUnInit();

	/* Stop Data(ALP/TS) Process */
	processFlag = 0;
	while (isProcessThreadRunning)
	{
		SL_SleepMS(100); // wait for process Thread Exit
	}

	/* Stop TLV Capture */
	SL_RxDataStop();
	while (isCaptureThreadRunning) // wait for capture Thread Exit
	{
		SL_SleepMS(100);
	}
	SL_I2cUnInit();
	SL_Printf("\n Exiting ATSC3.0 test");
	SL_SleepMS(1000);


	return;

TEST_ERROR:
	SL_DemodUnInit(slUnit);
	SL_TunerUnInit(tUnit);
	SL_DemodDeleteInstance(slUnit);
	SL_TunerDeleteInstance(tUnit);

	/* Stop Data(ALP/TS) Process */
	processFlag = 0;
	while (isProcessThreadRunning)
	{
		SL_SleepMS(100); // wait for process Thread Exit
	}

	/* Stop TLV Capture */
	SL_RxDataStop();
	while (isCaptureThreadRunning)
	{
		SL_SleepMS(100); // wait for capture Thread Exit
	}
	SL_Printf("\n Press any key to exit..");
	getchar();
	SL_I2cUnInit();
	SL_Printf("\n Exiting ATSC3.0 test \n");
	SL_SleepMS(1000);
	return;
}
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
	isProcessThreadRunning = 1;
	fp = SL_FileOpen("sl-tlv.bin", "wb");
	if (fp == NULL)
	{
		SL_Printf("\n Error: sl-tlv.bin failed to open");
	}
	SL_FileClose(fp);
	cb = CircularBufferCreate(CB_SIZE);
	while (processFlag)
	{
		if (getbbValue)
		{
			if (CircularBufferGetDataSize(cb) >= MAX_BBC_BUFFER_SIZE)
				processData(MAX_BBC_BUFFER_SIZE);
		}
		else
		{
			if (CircularBufferGetDataSize(cb) >= MAX_ALP_BUFFER_SIZE)
			{
				processData(MAX_ALP_BUFFER_SIZE);
			}
			else
			{
				SL_SleepMS(10);
			}
		}
	}
	CircularBufferFree(cb); // delete Circular buffer
	cb = NULL;
	isProcessThreadRunning = 0;
	SL_DeleteThread(&pThread);
	return 0;
}
#endif


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

	atsc3DiagFp = SL_FileOpen("logAtsc3Diag.txt", "w");
	if (atsc3DiagFp == NULL)
	{
	SL_Printf("\n Failed to open logAtsc3Diag.txt");
	}
	else
	{
	logPlfConfiguration(getPlfConfig);
	logDemodConfiguration(cfgInfo);
	logDemodAtsc3Configuration(atsc3ConfigInfo);
	SL_FileClose(atsc3DiagFp);
	}


	logPlfConfiguration(getPlfConfig);
	logDemodConfiguration(cfgInfo);
	//logDemodAtsc3Configuration(atsc3ConfigInfo);
	*/

#if 1
	SL_Printf("\n DiagnosticThread \n				:");
	int i = 0;

	while (i < 10)
	{

		i++;
		dres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_LOCK, (SL_DemodLockStatus_t*)&lockStatus);
		if (dres != SL_OK)
		{
			SL_Printf("\n Error:SL_Demod Get Lock Status                :");
			printToConsoleDemodError(dres);
		}
		else
		{
			SL_Printf("\n\n Demod Lock Status");


			if ((lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_RF_LOCK))
			{
				SL_Printf("\n RF                : Locked");

			}
			else
			{
				SL_Printf("\n RF                : Unlocked");

			}

			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1B_LOCK)
			{
				SL_Printf("\n L1B               : Locked");

			}
			else
			{
				SL_Printf("\n L1B               : Unlocked");

			}

			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1D_LOCK)
			{
				SL_Printf("\n L1D               : Locked");
			}
			else
			{
				SL_Printf("\n L1D               : Unlocked");

			}

			SL_Printf("\n Baseband Lock Status");
			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP0_LOCK)
			{
				SL_Printf("\n PLP0              : Locked");
			}
			else
			{
				SL_Printf("\n PLP0              : Unlocked");
			}

			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP1_LOCK)
			{
				SL_Printf("\n PLP1              : Locked");

			}
			else
			{
				SL_Printf("\n PLP1              : Unlocked");

			}

			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP2_LOCK)
			{
				SL_Printf("\n PLP2              : Locked");

			}
			else
			{
				SL_Printf("\n PLP2              : Unlocked");

			}

			if (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP3_LOCK)
			{
                SL_Printf("\tPLP3\t: Locked");
                //SL_FilePrintf(atsc3DiagFp, "\n PLP3                   :Locked");
            }
            else
            {
                SL_Printf("\tPLP3\t: Unlocked");
                //SL_FilePrintf(atsc3DiagFp, "\n PLP3                   :Unlocked");
            }
        }

        dres = SL_DemodGetStatus(slUnit, SL_DEMOD_STATUS_TYPE_CPU, (int*)&cpuStatus);
        if (dres != SL_OK)
        {
            SL_Printf("\n Error:SL_Demod Get CPU Status                 :");
            printToConsoleDemodError(dres);
			}
			else
			{
            SL_Printf("\n\n Demod CPU Status  : %s", (cpuStatus == 0xFFFFFFFF) ? " RUNNING" : " HALTED");
            //SL_FilePrintf(atsc3DiagFp, "\n Demod CPU Status       : %s", (cpuStatus == 0xFFFFFFFF) ? "RUNNING" : "HALTED");
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
                    //handleCmdIfFailure();
SL_Printf("\n Error:SL_ERR_CMD_IF_FAILURE");
			}
		}

            if (cfgInfo.extLnaMode != SL_EXT_LNA_CFG_MODE_NOT_PRESENT)
		{
                switch ((cfgInfo.extLnaMode >> 16) & 0x00000001)
		{
                case 0:
                    SL_Printf("\n Current LNA status:  External LNA Bypassed ");
                    break;

                case 1:
                    SL_Printf("\n Current LNA status:  External LNA Enabled");
                    break;

                default:
                    SL_Printf("\n Current LNA status:  Invalid");
                    break;
                }
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
            //SL_FilePrintf(atsc3DiagFp, "\n\n Tuner RSSI             : %3.2f dBm", tunerInfo.signalStrength);
		}

		if (demodStartStatus && (lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_RF_LOCK))
		{
			dres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_PERF, (SL_Atsc3p0Perf_Diag_t*)&perfDiag);
			if (dres != SL_OK)
			{
				SL_Printf("\n Error getting ATSC3.0 Performance Diagnostics :");
				printToConsoleDemodError(dres);
			}
			else
			{
				snr = (float)perfDiag.GlobalSnrLinearScale / 16384;
				snr = 10.0 * log10(snr);
				SL_Printf("\n SNR               : %3.2f dB\n\n", snr);

				captureFrameandBitErrorRate();
				captureStreamandChannelBitRate();
				//printAtsc3PerfDiagnostics(perfDiag, DIAG_LOC);
			}

			dres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_BSR, (SL_Atsc3p0Bsr_Diag_t*)&bsrDiag);
			if (dres != SL_OK)
			{
				SL_Printf("\n Error getting ATSC3.0 Boot Strap Diagnostics  :");
				printToConsoleDemodError(dres);
			}
			else
			{
				//printAtsc3BsrDiagnostics(bsrDiag, DIAG_LOC);
			}

			dres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_L1B, (SL_Atsc3p0L1B_Diag_t*)&l1bDiag);
			if (dres != SL_OK)
			{
				SL_Printf("\n Error getting ATSC3.0 L1B Diagnostics         :");
				printToConsoleDemodError(dres);
			}
			else
			{
				//printAtsc3L1bDiagnostics(l1bDiag, DIAG_LOC);
			}

			dres = SL_DemodGetDiagnostics(slUnit, SL_DEMOD_DIAG_TYPE_ATSC3P0_L1D, (SL_Atsc3p0L1D_Diag_t*)&l1dDiag);
			if (dres != SL_OK)
			{
				SL_Printf("\n Error getting ATSC3.0 L1D Diagnostics         :");
				printToConsoleDemodError(dres);
			}
			else
			{
				//printAtsc3L1dDiagnostics(l1bDiag.L1bNoOfSubframes, l1dDiag, DIAG_LOC);
				//printAtsc3SignalDetails(l1bDiag.L1bNoOfSubframes, l1dDiag, DIAG_LOC);
			}

			if (dres == SL_ERR_CMD_IF_FAILURE)
			{
				//handleCmdIfFailure();
			}
		}

		SL_Printf("\n Press any key to exit..");
		SL_SleepMS(1000); //Read Diagnostics every second


	}

	isDiagThreadRunning = 0; // Diagnostic Thread Stopped
	SL_DeleteThread(&dThread);
#endif
	return 0;
}


static double GetDemodBitRate(int L1SamplePerFrame, int L1FrameCount, int PLPxByteCount)
{
	return ((((double)PLPxByteCount / (double)L1FrameCount) / ((double)L1SamplePerFrame / 6912000.0)) * 8.0) / 1000000.0;
}

static void handleCmdIfFailure(void)
{
	SL_Printf("\n SL CMD IF FAILURE: Cannot continue!");
	SL_DemodUnInit(slUnit);
	SL_TunerUnInit(tUnit);
	processFlag = 0;
	diagFlag = 0;
}
#if 0
static void RxDataCallback(unsigned char *data, long len)
{
	if (cb != NULL)
		CircularBufferPush(cb, (char *)data, len);
}

#endif
#if 0
static void processData(unsigned int size)
{
#if PROCESS_TLV_DATA
	while (CircularBufferGetDataSize(cb) >= TLV_MAX_LENGTH) //TLV_MAX_SIZE: 188 * 10
	{
		static unsigned int dataSkipCnt = 0;
		int found = 0;
		CircularBufferPop(cb, 1, reinterpret_cast<char*>(&buffer[0]));
		if (buffer[0] == 0x57)
		{
			CircularBufferPop(cb, 1, reinterpret_cast<char*>(&buffer[1]));
			if (buffer[1] == 0x13)
			{
				CircularBufferPop(cb, 1, reinterpret_cast<char*>(&buffer[2]));
				if (buffer[2] == 0x68)
				{
					CircularBufferPop(cb, 1, reinterpret_cast<char*>(&buffer[3]));
					if (buffer[3] == 0x24)
					{
						CircularBufferPop(cb, TLV_HEADER_LENGTH - 4, reinterpret_cast<char*>(&buffer[4]));
						found = 1;
						dataSkipCnt = 0;
						unsigned int alpLen = buffer[7] << 24 | buffer[6] << 16 | buffer[5] << 8 | buffer[4]; // ALP length
						unsigned int plpId = buffer[8]; // PLP ID
						unsigned int alpPadLen = buffer[11]; // pading length, ignore alpPadLen in each TLV packet after ALP data read

                        unsigned int sec  = buffer[31] << 24 | buffer[30] << 16 | buffer[29] << 8 | buffer[28]; // seconds
                        unsigned int msec = buffer[35] << 24 | buffer[34] << 16 | buffer[33] << 8 | buffer[32]; // milliseconds
                        unsigned int usec = buffer[39] << 24 | buffer[38] << 16 | buffer[37] << 8 | buffer[36]; // microseconds
                        unsigned int nsec = buffer[43] << 24 | buffer[42] << 16 | buffer[41] << 8 | buffer[40]; // nanoseconds
                        time_t sec_raw_time  = sec;
                        time_t msec_raw_time = msec;
                        time_t usec_raw_time = usec;
                        time_t nsec_raw_time = nsec;
                        struct tm  time_struct;
                        time_struct = *localtime(&sec_raw_time);
                        char ch_buf[80] = { 0 };
                        strftime(ch_buf, sizeof(ch_buf), "\n%a %Y-%m-%d %H:%M:%S %Z", &time_struct);
                        //SL_Printf("Time : %s", ch_buf); //Enable this print only during testing
						CircularBufferPop(cb, alpLen, reinterpret_cast<char*>(buffer));  // Read ALP Data
						if (plpId != (char)0xFF)
						{
							processALP(plpId, buffer, alpLen);  // Process ALP Data
						}
						CircularBufferPop(cb, alpPadLen, reinterpret_cast<char*>(buffer)); // Read pading data from buffer and ignore
					}
				}
			}
		}

		if (!found)
		{
			dataSkipCnt++;
			// SL_Printf("\n Data Bytes Loss in TLV          :%d", dataSkipCnt);
		}
	}
#else
	fp = SL_FileOpen("sl-tlv.bin", "ab");
	if (fp == NULL)
	{
		SL_Printf("\n Error: sl-tlv.bin failed to open");
	}
	else
	{
		CircularBufferPop(cb, size, (char*)buffer);
		SL_FileWrite(buffer, 1, size, fp);
		SL_FileClose(fp);
	}
#endif
}
#endif
#if PROCESS_TLV_DATA

static void processALP(unsigned char plpId, unsigned char *buffer, unsigned int alpLen)
{
	/* Tolka: Add two-byte header for the alpLen */
	char header[2];
	header[0] = (char)((alpLen & 0x0000FF00) >> 8);
	header[1] = (char)(alpLen & 0x000000FF);

	/* Customer ALP Process Code starts here */
	if (fpALP == NULL)
	{
		fpALP = (FILE *)SL_FileOpen("DVB.alp", "wb");
	}

	SL_FileWrite(header, 1, 2, fpALP);
	SL_FileWrite(buffer, 1, alpLen, fpALP);
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

static void printToConsoleDemodConfiguration(SL_DemodConfigInfo_t cfgInfo)
{
	SL_Printf("\n\n SL Demod Configuration");
	switch (cfgInfo.std)
	{
	case SL_DEMODSTD_ATSC3_0:
		SL_Printf("\n Demod Standard               : ATSC3_0");
		break;

	case SL_DEMODSTD_ATSC1_0:
		SL_Printf("\n Demod Standard         : ATSC1_0");
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

static void printToConsoleDemodAtsc3Configuration(SL_Atsc3p0ConfigParams_t atsc3Info)
{

	SL_Printf("\n ATSC3 Configuration");
	SL_Printf("\n   PLP0                 : %d", (signed char)atsc3Info.plpConfig.plp0);
	SL_Printf("\n   PLP1                 : %d", (signed char)atsc3Info.plpConfig.plp1);
	SL_Printf("\n   PLP2                 : %d", (signed char)atsc3Info.plpConfig.plp2);
	SL_Printf("\n   PLP3                 : %d", (signed char)atsc3Info.plpConfig.plp3);

	switch (atsc3Info.region)
	{
	case SL_ATSC3P0_REGION_US:
		SL_Printf("\n   Region               : US");
		break;

	case SL_ATSC3P0_REGION_KOREA:
		SL_Printf("\n   Region               : Korea");
		break;

	default:
		SL_Printf("\n   Region               : NA");
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

static void logDemodAtsc3Configuration(SL_Atsc3p0ConfigParams_t atsc3Info)
{
	SL_Printf("\n PLP Configuration");
	SL_Printf("\n   PLP0                 : %d", atsc3Info.plpConfig.plp0);
	SL_Printf("\n   PLP1                 : %d", atsc3Info.plpConfig.plp1);
	SL_Printf("\n   PLP2                 : %d", atsc3Info.plpConfig.plp2);
	SL_Printf("\n   PLP3                 : %d", atsc3Info.plpConfig.plp3);

	switch (atsc3Info.region)
	{
	case SL_ATSC3P0_REGION_US:
		SL_Printf("\n   Region               : US");
		break;

	case SL_ATSC3P0_REGION_KOREA:
		SL_Printf("\n   Region               : Korea");
		break;

	default:
		SL_Printf("\n   Region               : NA");
	}
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

static void captureFrameandBitErrorRate(void)
{
    SL_Printf("\n Total Frame Count \n");
    SL_Printf(" L1B\t: %d\t\t", perfDiag.NumFecFrameL1b);
    SL_Printf(" L1D\t: %d\t\t", perfDiag.NumFecFrameL1d);
    if (atsc3ConfigInfo.plpConfig.plp0 != (char)0xFF)
    {
        SL_Printf(" PLP0\t: %d\t\t", perfDiag.NumFecFramePlp0);
    }
    else
    {
        SL_Printf(" PLP0\t: NA\t\t");
    }
    if (atsc3ConfigInfo.plpConfig.plp1 != (char)0xFF)
    {
        SL_Printf(" PLP1\t: %d\t\t", perfDiag.NumFecFramePlp1);
    }
    else
    {
        SL_Printf(" PLP1\t: NA\t\t");
    }
    if (atsc3ConfigInfo.plpConfig.plp2 != (char)0xFF)
    {
        SL_Printf(" PLP2\t: %d\t\t", perfDiag.NumFecFramePlp2);
    }
    else
    {
        SL_Printf(" PLP2\t: NA\t\t");
    }
    if (atsc3ConfigInfo.plpConfig.plp3 != (char)0xFF)
    {
        SL_Printf(" PLP3\t: %d\t\n", perfDiag.NumFecFramePlp3);
    }
    else
    {
        SL_Printf(" PLP3\t: NA\n");
    }

    SL_Printf("\n Error Frame Count \n");
    SL_Printf(" L1B\t: %d\t\t", perfDiag.NumFrameErrL1b);
    SL_Printf(" L1D\t: %d\t\t", perfDiag.NumFrameErrL1d);
    if (atsc3ConfigInfo.plpConfig.plp0 != (char)0xFF)
    {
        SL_Printf(" PLP0\t: %d\t\t", perfDiag.NumFrameErrPlp0);
    }
    else
    {
        SL_Printf(" PLP0\t: NA\t\t");
    }
    if (atsc3ConfigInfo.plpConfig.plp1 != (char)0xFF)
    {
        SL_Printf(" PLP1\t: %d\t\t", perfDiag.NumFrameErrPlp1);
    }
    else
    {
        SL_Printf(" PLP1\t: NA\t\t");
    }
    if (atsc3ConfigInfo.plpConfig.plp2 != (char)0xFF)
    {
        SL_Printf(" PLP2\t: %d\t\t", perfDiag.NumFrameErrPlp2);
    }
    else
    {
        SL_Printf(" PLP2\t: NA\t\t");
    }
    if (atsc3ConfigInfo.plpConfig.plp3 != (char)0xFF)
    {
        SL_Printf(" PLP3\t: %d\n", perfDiag.NumFrameErrPlp3);
    }
    else
    {
        SL_Printf(" PLP3\t: NA\n");
    }

	fer = (float)perfDiag.NumFrameErrL1b / perfDiag.NumFecFrameL1b;
    SL_Printf("\n\n Frame Error Rate \n");
    SL_Printf(" L1B\t: %1.2e\t", fer);
    //SL_FilePrintf(atsc3DiagFp, "             FER               \n");
    //SL_FilePrintf(atsc3DiagFp, " FER for L1B            : %1.2e\n", fer);

	fer = (float)perfDiag.NumFrameErrL1d / perfDiag.NumFecFrameL1d;
    SL_Printf(" L1D\t: %1.2e\t", fer);
    //SL_FilePrintf(atsc3DiagFp, " FER for L1D            : %1.2e\n", fer);

    if (atsc3ConfigInfo.plpConfig.plp0 != (char)0xFF)
	{
		fer = (float)perfDiag.NumFrameErrPlp0 / perfDiag.NumFecFramePlp0;
        SL_Printf(" PLP0\t: %1.2e\t", fer);
        //SL_FilePrintf(atsc3DiagFp, " FER for PLP0           : %1.2e\n", fer);
	}
	else
	{
        SL_Printf(" PLP0\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp1 != (char)0xFF)
	{
		fer = (float)perfDiag.NumFrameErrPlp1 / perfDiag.NumFecFramePlp1;
        SL_Printf(" PLP1\t: %1.2e\t", fer);
        //SL_FilePrintf(atsc3DiagFp, " FER for PLP1           : %1.2e\n", fer);
	}
	else
	{
        SL_Printf(" PLP1\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp2 != (char)0xFF)
	{
		fer = (float)perfDiag.NumFrameErrPlp2 / perfDiag.NumFecFramePlp2;
        SL_Printf(" PLP2\t: %1.2e\t", fer);
        //SL_FilePrintf(atsc3DiagFp, " FER for PLP2           : %1.2e\n", fer);
	}
	else
	{
        SL_Printf(" PLP2\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp3 != (char)0xFF)
	{
		fer = (float)perfDiag.NumFrameErrPlp3 / perfDiag.NumFecFramePlp3;
        SL_Printf(" PLP3\t: %1.2e\n", fer);
        //SL_FilePrintf(atsc3DiagFp, " FER for PLP3           : %1.2e\n", fer);
	}
	else
	{
        SL_Printf(" PLP3\t: NA\n");
	}

	ber = (float)perfDiag.NumBitErrL1b / perfDiag.NumFecBitsL1b;
	SL_Printf("\n Bit Error Rate \n");
    SL_Printf(" L1B\t: %1.2e\t", ber);
    //SL_FilePrintf(atsc3DiagFp, "             BER               \n");
    //SL_FilePrintf(atsc3DiagFp, " BER for L1B            : %1.2e\n", ber);
	ber = (float)perfDiag.NumBitErrL1d / perfDiag.NumFecBitsL1d;
    SL_Printf(" L1D\t: %1.2e\t", ber);
    //SL_FilePrintf(atsc3DiagFp, " BER for L1D            : %1.2e\n", ber);

    if (atsc3ConfigInfo.plpConfig.plp0 != (char)0xFF)
	{
		ber = (float)perfDiag.NumBitErrPlp0 / perfDiag.NumFecBitsPlp0;
        SL_Printf(" PLP0\t: %1.2e\t", ber);
        //SL_FilePrintf(atsc3DiagFp, " BER for PLP0           : %1.2e\n", ber);
	}
	else
	{
        SL_Printf(" PLP0\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp1 != (char)0xFF)
	{
		ber = (float)perfDiag.NumBitErrPlp1 / perfDiag.NumFecBitsPlp1;
        SL_Printf(" PLP1\t: %1.2e\t", ber);
        //SL_FilePrintf(atsc3DiagFp, " BER for PLP1           : %1.2e\n", ber);
	}
	else
	{
        SL_Printf(" PLP1\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp2 != (char)0xFF)
	{
		ber = (float)perfDiag.NumBitErrPlp2 / perfDiag.NumFecBitsPlp2;
        SL_Printf(" PLP2\t: %1.2e\t", ber);
        //SL_FilePrintf(atsc3DiagFp, " BER for PLP2           : %1.2e\n", ber);
	}
	else
	{
        SL_Printf(" PLP2\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp3 != (char)0xFF)
	{
		ber = (float)perfDiag.NumBitErrPlp3 / perfDiag.NumFecBitsPlp3;
        SL_Printf(" PLP3\t: %1.2e\n", ber);
        //SL_FilePrintf(atsc3DiagFp, " BER for PLP3           : %1.2e\n", ber);
	}
	else
	{
        SL_Printf(" PLP3\t: NA\n");
	}
}

static void captureStreamandChannelBitRate(void)
{
    SL_Printf("\n\n Stream Bit Rate   \n");
    if (atsc3ConfigInfo.plpConfig.plp0 != (char)0xFF)
	{
        SL_Printf(" PLP0\t: %.2f Mbps\t", GetDemodBitRate(perfDiag.L1SampePerFrame, perfDiag.L1FrameCount, perfDiag.Plp0StreamByteCount));
	}
	else
	{
        SL_Printf(" PLP0\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp1 != (char)0xFF)
	{
        SL_Printf(" PLP1\t: %.2f Mbps\t", GetDemodBitRate(perfDiag.L1SampePerFrame, perfDiag.L1FrameCount, perfDiag.Plp1StreamByteCount));
	}
	else
	{
        SL_Printf(" PLP1\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp2 != (char)0xFF)
	{
        SL_Printf(" PLP2\t: %.2f Mbps\t", GetDemodBitRate(perfDiag.L1SampePerFrame, perfDiag.L1FrameCount, perfDiag.Plp2StreamByteCount));
	}
	else
	{
        SL_Printf(" PLP2\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp3 != (char)0xFF)
	{
        SL_Printf(" PLP3\t: %.2f Mbps\n", GetDemodBitRate(perfDiag.L1SampePerFrame, perfDiag.L1FrameCount, perfDiag.Plp3StreamByteCount));
	}
	else
	{
        SL_Printf(" PLP3\t: NA\n");
	}

	SL_Printf("\n Channel Bit Rate \n");
    if (atsc3ConfigInfo.plpConfig.plp0 != (char)0xFF)
	{
        SL_Printf(" PLP0\t: %.2f Mbps\t", GetDemodBitRate(perfDiag.L1SampePerFrame, perfDiag.L1FrameCount, perfDiag.Plp0ChannelByteCount));
	}
	else
	{
        SL_Printf(" PLP0\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp1 != (char)0xFF)
	{
        SL_Printf(" PLP1\t: %.2f Mbps\t", GetDemodBitRate(perfDiag.L1SampePerFrame, perfDiag.L1FrameCount, perfDiag.Plp1ChannelByteCount));
	}
	else
	{
        SL_Printf(" PLP1\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp2 != (char)0xFF)
	{
        SL_Printf(" PLP2\t : %.2f Mbps\t", GetDemodBitRate(perfDiag.L1SampePerFrame, perfDiag.L1FrameCount, perfDiag.Plp2ChannelByteCount));
	}
	else
	{
        SL_Printf(" PLP2\t: NA\t\t");
	}

    if (atsc3ConfigInfo.plpConfig.plp3 != (char)0xFF)
	{
        SL_Printf(" PLP3\t: %.2f Mbps\n", GetDemodBitRate(perfDiag.L1SampePerFrame, perfDiag.L1FrameCount, perfDiag.Plp3ChannelByteCount));
	}
	else
	{
        SL_Printf(" PLP3\t: NA\n");
	}
}

#if 0
#define PRINT_LOCK_STATUS(isLocked)   (isLocked != 0)?"     Locked":"   Unlocked"
static void CaptureDiagnosticInfo(void)
{
	SL_Printf("\n                      Lock Status Total Frame  Error Frames  Frame Error Rate ");
	SL_Printf("\n L1B                : %s  %10d    %10d          %1.2e", PRINT_LOCK_STATUS(lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1B_LOCK), perfDiag.NumFecFrameL1b, perfDiag.NumFrameErrL1b, (float)((float)perfDiag.NumFrameErrL1b / (float)perfDiag.NumFecFrameL1b));
	SL_Printf("\n L1D                : %s  %10d    %10d          %1.2e", PRINT_LOCK_STATUS(lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1D_LOCK), perfDiag.NumFecFrameL1d, perfDiag.NumFrameErrL1d, (float)((float)perfDiag.NumFrameErrL1d / (float)perfDiag.NumFecFrameL1d));
	if (atsc3ConfigInfo.plpConfig.plp0 != -1)
	{
		SL_Printf("\n PLP0               : %s  %10d    %10d          %1.2e", PRINT_LOCK_STATUS(lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP0_LOCK), perfDiag.NumFecFramePlp0, perfDiag.NumFrameErrPlp0, (float)((float)perfDiag.NumFrameErrPlp0 / (float)perfDiag.NumFecFramePlp0));
	}

	if (atsc3ConfigInfo.plpConfig.plp1 != -1)
	{
		SL_Printf("\n PLP1               : %s  %10d    %10d          %1.2e", PRINT_LOCK_STATUS(lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP1_LOCK), perfDiag.NumFecFramePlp1, perfDiag.NumFrameErrPlp1, (float)((float)perfDiag.NumFrameErrPlp1 / (float)perfDiag.NumFecFramePlp1));
	}

	if (atsc3ConfigInfo.plpConfig.plp2 != -1)
	{
		SL_Printf("\n PLP2               : %s  %10d    %10d          %1.2e", PRINT_LOCK_STATUS(lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP2_LOCK), perfDiag.NumFecFramePlp2, perfDiag.NumFrameErrPlp2, (float)((float)perfDiag.NumFrameErrPlp2 / (float)perfDiag.NumFecFramePlp2));
	}

	if (atsc3ConfigInfo.plpConfig.plp3 != -1)
	{
		SL_Printf("\n PLP3               : %s  %10d    %10d          %1.2e", PRINT_LOCK_STATUS(lockStatus & SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP3_LOCK), perfDiag.NumFecFramePlp3, perfDiag.NumFrameErrPlp3, (float)((float)perfDiag.NumFrameErrPlp3 / (float)perfDiag.NumFecFramePlp3));
	}
}
#endif
