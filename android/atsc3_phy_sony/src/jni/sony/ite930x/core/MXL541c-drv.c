#include "it930x-core.h"
#include <linux/module.h>
#include <linux/kernel.h>
//#include <asm/div64.h>
#include "mxl_datatypes.h"
#include "MxLWare_HYDRA_DemodTunerApi.h"
#include "MxLWare_HYDRA_DeviceApi.h"
#include "MxLWare_HYDRA_TsMuxCtrlApi.h"
#include "MxLWare_HYDRA_DiseqcFskApi.h"
#include "MxLWare_HYDRA_CommonApi.h"
#include "MxLWare_HYDRA_5xx_FW.h"

#define MXL541C_HWTYPE	28
#define DISEQC_INTERRUPT_BIT (1<<0)
#define DISEQC_INTERRUPT_STATUS_TIMEOUT 15
#define MXL541C_DEBUG 0

#if MXL541C_DEBUG 
	#define DRV_MXL541C_DEBUG(args...)   printk(args)
#else
	#define DRV_MXL541C_DEBUG(args...)
#endif

static Dword	error;
Dword			command;
Byte			i2cBus;
Byte			i2cAdd;
MXL_STATUS_E				mxlStatus;
uint8_t						MXL_DEVICE_ID = 0;
MXL_HYDRA_VER_INFO_T		versionInfo;
UINT8						maxDemods;
MXL_HYDRA_INTR_CFG_T		intrCfg;
UINT32						intrMask = 0;
MXL_HYDRA_AUX_CTRL_MODE_E	lnbInterface = MXL_HYDRA_AUX_CTRL_MODE_DISEQC;
MXL_HYDRA_MPEGOUT_PARAM_T	mpegInterfaceCfg[4];
MXL_HYDRA_DEVICE_E			hydraDevSku = MXL_HYDRA_DEVICE_541C;
MXL_HYDRA_DEV_XTAL_T		xtalParam;

Dword DRV_MXL541C_Initialize(Device_Context *DC, int br_idx, int tsSrcIdx, Byte ChipType)
{
	
	if(tsSrcIdx != 0) return 0;
	
	i2cBus = 3;
	i2cAdd = 0xC0;
	maxDemods = 4;

	//necessary
	error = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, p_br_reg_ts1_clk_sel, 0x01);
	if (error) goto exit;
	error = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, p_br_reg_ts2_clk_sel, 0x01);
	if (error) goto exit;
	error = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, p_br_reg_ts3_clk_sel, 0x01);
	if (error) goto exit;
	error = IT9300_writeRegister((Endeavour*)&DC->it9300, 0, p_br_reg_ts4_clk_sel, 0x01);
	if (error) goto exit;

	// Initilize MxLWare driver
	mxlStatus = MxLWare_HYDRA_API_CfgDrvInit(MXL_DEVICE_ID, (Endeavour*)&DC->it9300, i2cBus, i2cAdd, hydraDevSku);
	if (mxlStatus != MXL_E_SUCCESS) {
		goto exit;
	}
	// Oerwrite defaults
	mxlStatus = MxLWare_HYDRA_API_CfgDevOverwriteDefault(MXL_DEVICE_ID);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgDevOverwriteDefault - Error (%d)\n", mxlStatus);
		goto exit;
	}
	else
	{
		printk("Done! - MxLWare_HYDRA_API_CfgDevOverwriteDefault\n");
	}

	xtalParam.xtalFreq = MXL_HYDRA_XTAL_24_MHz;
	xtalParam.xtalCap = 20;
	mxlStatus = MxLWare_HYDRA_API_CfgDevXtal(MXL_DEVICE_ID, &xtalParam, MXL_E_NO);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgDevXtal - Error (%d)\n", mxlStatus);
		goto exit;
	}

	// Download firmware
	// Check if firmware is already downladed and is running
	mxlStatus = MxLWare_HYDRA_API_ReqDevVersionInfo(MXL_DEVICE_ID, &versionInfo);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_ReqDevVersionInfo - Error (%d)\n", mxlStatus);
		goto exit;
	}
	mxlStatus = MxLWare_HYDRA_API_CfgDevPowerMode(MXL_DEVICE_ID, MXL_HYDRA_PWR_MODE_ACTIVE);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgDevPowerMode - Error (%d)\n", mxlStatus);
		goto exit;
	}

	if (versionInfo.firmwareDownloaded == MXL_E_FALSE)
	{
		// Download firmware only if firmware is not active/running.
		mxlStatus = MxLWare_HYDRA_API_CfgDevFWDownload(MXL_DEVICE_ID, mxl_hydra_firmware_length, mxl_hydra_firmware_rawData, NULL);
	}
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("Firmware Download - Error (%d)\n", mxlStatus);
		goto exit;
	}
	else
	{
		printk("Done! - Firmware Download \n");
	}
	// Get device version info
	mxlStatus = MxLWare_HYDRA_API_ReqDevVersionInfo(MXL_DEVICE_ID, &versionInfo);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_ReqDevVersionInfo - Error (%d)\n", mxlStatus);
		goto exit;
	}
	else
	{
		printk("Firmware Version  : %d.%d.%d.%d\n", versionInfo.firmwareVer[0],
			versionInfo.firmwareVer[1],
			versionInfo.firmwareVer[2],
			versionInfo.firmwareVer[3]);

		printk("MxLWare Version   : %d.%d.%d.%d\n", versionInfo.mxlWareVer[0],
			versionInfo.mxlWareVer[1],
			versionInfo.mxlWareVer[2],
			versionInfo.mxlWareVer[3]);
	}

	// Configure device interrupts
	intrCfg.intrDurationInNanoSecs = 10000; //10 ms
	intrCfg.intrType = HYDRA_HOST_INTR_TYPE_LEVEL_POSITIVE; // Level trigger

	intrMask = MXL_HYDRA_INTR_EN |
	MXL_HYDRA_INTR_DMD_FEC_LOCK_0 |
	MXL_HYDRA_INTR_DMD_FEC_LOCK_1 |
	MXL_HYDRA_INTR_DMD_FEC_LOCK_2 |
	MXL_HYDRA_INTR_DMD_FEC_LOCK_3 |
	MXL_HYDRA_INTR_DMD_FEC_LOCK_4 |
	MXL_HYDRA_INTR_DMD_FEC_LOCK_5;

	// if MxL581 or 584 then enable INT's for demod FEC lock
	if (maxDemods == MXL_HYDRA_DEMOD_MAX)
	{
		intrMask |= MXL_HYDRA_INTR_DMD_FEC_LOCK_6;
		intrMask |= MXL_HYDRA_INTR_DMD_FEC_LOCK_7;
	}

	if (lnbInterface == MXL_HYDRA_AUX_CTRL_MODE_FSK)
	{
		// Enable INT for FSK
		intrMask |= MXL_HYDRA_INTR_FSK;
	}
	else
	{
		// Enable INT's for DiSEqC
		intrMask |= MXL_HYDRA_INTR_DISEQC_0;
		intrMask |= MXL_HYDRA_INTR_DISEQC_1;
		intrMask |= MXL_HYDRA_INTR_DISEQC_2;
		intrMask |= MXL_HYDRA_INTR_DISEQC_3;
	}

	// Configure INT's
	mxlStatus = MxLWare_HYDRA_API_CfgDevInterrupt(MXL_DEVICE_ID, intrCfg, intrMask);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgDevInterrupt - Error (%d)\n", mxlStatus);
		goto exit;
	}
	else
	{
		printk("Done! - MxLWare_HYDRA_API_CfgDevInterrupt \n");
	}

	//Select either DiSEqC or FSK - only one interface will be avilable either FSK or DiSEqC
	mxlStatus = MxLWare_HYDRA_API_CfgDevDiseqcFskOpMode(MXL_DEVICE_ID, lnbInterface);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgDevDiseqcFskOpMode - Error (%d)\n", mxlStatus);
		goto exit;
	}
	else
	{
		printk("Done! - MxLWare_HYDRA_API_CfgDevDiseqcFskOpMode \n");
	}
	if (lnbInterface == MXL_HYDRA_AUX_CTRL_MODE_FSK)
	{
		// Configure FSK operating mode
		mxlStatus = MxLWare_HYDRA_API_CfgFskOpMode(MXL_DEVICE_ID, MXL_HYDRA_FSK_CFG_TYPE_39KPBS);
		if (mxlStatus != MXL_E_SUCCESS)
		{
			printk("MxLWare_HYDRA_API_CfgFskOpMode - Error (%d)\n", mxlStatus);
			goto exit;
		}
		else
		{
			printk("Done! - MxLWare_HYDRA_API_CfgFskOpMode \n");
		}
	}
	else
	{
		// Configure DISEQC operating mode - Only 22 KHz carrier frequency is supported.
		mxlStatus = MxLWare_HYDRA_API_CfgDiseqcOpMode(MXL_DEVICE_ID,
			MXL_HYDRA_DISEQC_ID_0,
			MXL_HYDRA_DISEQC_ENVELOPE_MODE,
			MXL_HYDRA_DISEQC_1_X,
			MXL_HYDRA_DISEQC_CARRIER_FREQ_22KHZ);
		if (mxlStatus != MXL_E_SUCCESS)
		{
			printk("MxLWare_HYDRA_API_CfgDiseqcOpMode - Error (%d)\n", mxlStatus);
			goto exit;
		}
		else
		{
			printk("Done! - MxLWare_HYDRA_API_CfgDiseqcOpMode \n");
		}
	}

	// Config TS MUX mode - Disable TS MUX feature
	mxlStatus = MxLWare_HYDRA_API_CfgTSMuxMode(MXL_DEVICE_ID, MXL_HYDRA_TS_MUX_DISABLE);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgTSMuxMode - Error (%d)\n", mxlStatus);
		goto exit;
	}
	else
	{
		printk("Done! - MxLWare_HYDRA_API_CfgTSMuxMode \n");
	}

	// Config TS interface of the device	
	mpegInterfaceCfg[0].enable = MXL_E_ENABLE;
	mpegInterfaceCfg[0].lsbOrMsbFirst = MXL_HYDRA_MPEG_SERIAL_MSB_1ST;
	mpegInterfaceCfg[0].maxMpegClkRate = 104; //  supports only (0 - 104 & 139)MHz
	mpegInterfaceCfg[0].mpegClkPhase = MXL_HYDRA_MPEG_CLK_PHASE_SHIFT_0_DEG;// MXL_HYDRA_MPEG_CLK_PHASE_SHIFT_0_DEG;
	mpegInterfaceCfg[0].mpegClkPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	mpegInterfaceCfg[0].mpegClkType = MXL_HYDRA_MPEG_CLK_GAPPED;
	mpegInterfaceCfg[0].mpegErrorIndication = MXL_HYDRA_MPEG_ERR_INDICATION_DISABLED;
	mpegInterfaceCfg[0].mpegMode = MXL_HYDRA_MPEG_MODE_SERIAL_4_WIRE;
	mpegInterfaceCfg[0].mpegSyncPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	mpegInterfaceCfg[0].mpegSyncPulseWidth = MXL_HYDRA_MPEG_SYNC_WIDTH_BYTE;
	mpegInterfaceCfg[0].mpegValidPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	// Configure TS interface
	mxlStatus = MxLWare_HYDRA_API_CfgMpegOutParams(MXL_DEVICE_ID, MXL_HYDRA_DEMOD_ID_0, &mpegInterfaceCfg[0]);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgMpegOutParams for (%d) - Error (%d)\n", MXL_HYDRA_DEMOD_ID_0, mxlStatus);
		goto exit;
	}
	else
	{
		printk("Done! - MxLWare_HYDRA_API_CfgMpegOutParams (%d) \n", MXL_HYDRA_DEMOD_ID_0);
	}

	// Config TS interface of the device
	mpegInterfaceCfg[1].enable = MXL_E_ENABLE;
	mpegInterfaceCfg[1].lsbOrMsbFirst = MXL_HYDRA_MPEG_SERIAL_MSB_1ST;
	mpegInterfaceCfg[1].maxMpegClkRate = 104; //  supports only (0 - 104 & 139)MHz
	mpegInterfaceCfg[1].mpegClkPhase = MXL_HYDRA_MPEG_CLK_PHASE_SHIFT_0_DEG;// MXL_HYDRA_MPEG_CLK_PHASE_SHIFT_0_DEG;
	mpegInterfaceCfg[1].mpegClkPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	mpegInterfaceCfg[1].mpegClkType = MXL_HYDRA_MPEG_CLK_GAPPED;
	mpegInterfaceCfg[1].mpegErrorIndication = MXL_HYDRA_MPEG_ERR_INDICATION_DISABLED;
	mpegInterfaceCfg[1].mpegMode = MXL_HYDRA_MPEG_MODE_SERIAL_4_WIRE;
	mpegInterfaceCfg[1].mpegSyncPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	mpegInterfaceCfg[1].mpegSyncPulseWidth = MXL_HYDRA_MPEG_SYNC_WIDTH_BYTE;
	mpegInterfaceCfg[1].mpegValidPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	mxlStatus = MxLWare_HYDRA_API_CfgMpegOutParams(MXL_DEVICE_ID, MXL_HYDRA_DEMOD_ID_1, &mpegInterfaceCfg[1]);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgMpegOutParams for (%d) - Error (%d)\n", MXL_HYDRA_DEMOD_ID_1, mxlStatus);
		goto exit;
	}
	else
	{
		printk("Done! - MxLWare_HYDRA_API_CfgMpegOutParams (%d) \n", MXL_HYDRA_DEMOD_ID_1);
	}


	// Config TS interface of the device
	mpegInterfaceCfg[2].enable = MXL_E_ENABLE;
	mpegInterfaceCfg[2].lsbOrMsbFirst = MXL_HYDRA_MPEG_SERIAL_MSB_1ST;
	mpegInterfaceCfg[2].maxMpegClkRate = 104; //  supports only (0 - 104 & 139)MHz
	mpegInterfaceCfg[2].mpegClkPhase = MXL_HYDRA_MPEG_CLK_PHASE_SHIFT_0_DEG;// MXL_HYDRA_MPEG_CLK_PHASE_SHIFT_0_DEG;
	mpegInterfaceCfg[2].mpegClkPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	mpegInterfaceCfg[2].mpegClkType = MXL_HYDRA_MPEG_CLK_GAPPED;
	mpegInterfaceCfg[2].mpegErrorIndication = MXL_HYDRA_MPEG_ERR_INDICATION_DISABLED;
	mpegInterfaceCfg[2].mpegMode = MXL_HYDRA_MPEG_MODE_SERIAL_4_WIRE;
	mpegInterfaceCfg[2].mpegSyncPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	mpegInterfaceCfg[2].mpegSyncPulseWidth = MXL_HYDRA_MPEG_SYNC_WIDTH_BYTE;
	mpegInterfaceCfg[2].mpegValidPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	mxlStatus = MxLWare_HYDRA_API_CfgMpegOutParams(MXL_DEVICE_ID, MXL_HYDRA_DEMOD_ID_2, &mpegInterfaceCfg[2]);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgMpegOutParams for (%d) - Error (%d)\n", MXL_HYDRA_DEMOD_ID_2, mxlStatus);
		goto exit;
	}
	else
	{
		printk("Done! - MxLWare_HYDRA_API_CfgMpegOutParams (%d) \n", MXL_HYDRA_DEMOD_ID_2);
	}

	// Config TS interface of the device
	mpegInterfaceCfg[3].enable = MXL_E_ENABLE;
	mpegInterfaceCfg[3].lsbOrMsbFirst = MXL_HYDRA_MPEG_SERIAL_MSB_1ST;
	mpegInterfaceCfg[3].maxMpegClkRate = 104; //  supports only (0 - 104 & 139)MHz
	mpegInterfaceCfg[3].mpegClkPhase = MXL_HYDRA_MPEG_CLK_PHASE_SHIFT_0_DEG;// MXL_HYDRA_MPEG_CLK_PHASE_SHIFT_0_DEG;
	mpegInterfaceCfg[3].mpegClkPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	mpegInterfaceCfg[3].mpegClkType = MXL_HYDRA_MPEG_CLK_GAPPED;
	mpegInterfaceCfg[3].mpegErrorIndication = MXL_HYDRA_MPEG_ERR_INDICATION_DISABLED;
	mpegInterfaceCfg[3].mpegMode = MXL_HYDRA_MPEG_MODE_SERIAL_4_WIRE;
	mpegInterfaceCfg[3].mpegSyncPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	mpegInterfaceCfg[3].mpegSyncPulseWidth = MXL_HYDRA_MPEG_SYNC_WIDTH_BYTE;
	mpegInterfaceCfg[3].mpegValidPol = MXL_HYDRA_MPEG_ACTIVE_HIGH;
	mxlStatus = MxLWare_HYDRA_API_CfgMpegOutParams(MXL_DEVICE_ID, MXL_HYDRA_DEMOD_ID_3, &mpegInterfaceCfg[3]);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgMpegOutParams for (%d) - Error (%d)\n", MXL_HYDRA_DEMOD_ID_3, mxlStatus);
		goto exit;
	}
	else
	{
		printk("Done! - MxLWare_HYDRA_API_CfgMpegOutParams (%d) \n", MXL_HYDRA_DEMOD_ID_3);
	}
	
	return 0;
	
exit:
	printk("something wrong!!!!!!!!!\n");
	return error;
}

Dword DRV_MXL541C_DeInitialize(Device_Context *DC, int br_idx, int tsSrcIdx)
{
	
	if(tsSrcIdx != 0) return 0;
		
	DRV_MXL541C_DEBUG("*** DRV_MXL541C_DeInitialize *** \n");
	MXL_DEVICE_ID = 0;
	mxlStatus =  MxLWare_HYDRA_API_CfgDrvUnInit(MXL_DEVICE_ID);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgDrvUnInit for (%d) - Error (%d)\n", MXL_DEVICE_ID, mxlStatus);
		goto exit;
	}
	else
	{
		printk("Done! - MxLWare_HYDRA_API_CfgDrvUnInit (%d) \n", MXL_DEVICE_ID);
	}

	
	return 0;
	
exit:
	printk("something wrong!!!!!!!!!\n");
	return -1;
}

Dword DRV_MXL541C_acquireChannel(Device_Context *DC, AcquireChannelRequest *request, int br_idx, int tsSrcIdx)
{
	Dword					tmp;
	Dword					frequencyKHz;
	Dword					symbol_rateKSps;
	MXL_HYDRA_DEMOD_ID_E	demodID;
	MXL_HYDRA_TUNER_ID_E		tunerId;
	MXL_HYDRA_TUNE_PARAMS_T	chanTuneParams;
	MXL_HYDRA_BCAST_STD_E	std;	
	MXL_HYDRA_DEMOD_SCRAMBLE_INFO_T	demodScramblingCode;

	DRV_MXL541C_DEBUG("DRV_MXL541C_acquireChannel tsSrcIdx = %x\n",tsSrcIdx);
	demodID = tsSrcIdx;
	tunerId = 0; 
	DRV_MXL541C_DEBUG("DRV_MXL541C_acquireChannel demodID = %x\n",demodID);
	DRV_MXL541C_DEBUG("DRV_MXL541C_acquireChannel tunerId = %x\n",tunerId);

	if (request->mode == TV_STANDARD_DVBS)
	{
		tmp = 0;
	}
	else
	{
		tmp = request->mode;
	}
	if (tmp == 0)
	{
		std = MXL_HYDRA_DVBS;
	}
	else
	{
		std = MXL_HYDRA_DVBS2;
	}

	DRV_MXL541C_DEBUG("request->frequency = %d\n",request->frequency);
	frequencyKHz = request->frequency;

	DRV_MXL541C_DEBUG("request->symbolrate = %d\n",request->symbolrate);
	symbol_rateKSps = request->symbolrate;
	DRV_MXL541C_DEBUG("symbol_rateKSps = %d\n",symbol_rateKSps);
	
	// Channel tune
	// Common parameters for DVB-S2, DVB-S & DSS modes
	chanTuneParams.standardMask = std;
	chanTuneParams.frequencyInHz = frequencyKHz * 1000; 
	chanTuneParams.freqSearchRangeKHz = 5000;		// 5MHz
	chanTuneParams.symbolRateKSps = symbol_rateKSps;	
	chanTuneParams.spectrumInfo = MXL_HYDRA_SPECTRUM_NON_INVERTED;

	// parameters specific to standard
	switch (chanTuneParams.standardMask)
	{
	default :
	case MXL_HYDRA_DVBS2 :
		// DVB-S2 specific parameters
		chanTuneParams.params.paramsS2.fec = MXL_HYDRA_FEC_AUTO;
		chanTuneParams.params.paramsS2.modulation = MXL_HYDRA_MOD_AUTO;
		chanTuneParams.params.paramsS2.pilots = MXL_HYDRA_PILOTS_AUTO;
		chanTuneParams.params.paramsS2.rollOff = MXL_HYDRA_ROLLOFF_AUTO;

		// For DVB-S2 standard program default scrambling code
		demodScramblingCode.scrambleCode = 0x01;
		demodScramblingCode.scrambleSequence[0] = 0x00;
		demodScramblingCode.scrambleSequence[1] = 0x00;
		demodScramblingCode.scrambleSequence[2] = 0x00;
		demodScramblingCode.scrambleSequence[3] = 0x00;
		demodScramblingCode.scrambleSequence[4] = 0x00;
		demodScramblingCode.scrambleSequence[5] = 0x00;
		demodScramblingCode.scrambleSequence[6] = 0x00;
		demodScramblingCode.scrambleSequence[7] = 0x00;
		demodScramblingCode.scrambleSequence[8] = 0x00;
		demodScramblingCode.scrambleSequence[9] = 0x00;
		demodScramblingCode.scrambleSequence[10] = 0x00;
		demodScramblingCode.scrambleSequence[11] = 0x00;

		// program default scramble code
		mxlStatus = MxLWare_HYDRA_API_CfgDemodScrambleCode(MXL_DEVICE_ID, demodID, &demodScramblingCode);
		if (mxlStatus != MXL_E_SUCCESS)
		{
			printk("MxLWare_HYDRA_API_CfgDemodScrambleCode for (%d) - Error (%d)\n", demodID, mxlStatus);
		}
		break;

	case MXL_HYDRA_DVBS :
		// DVB-S specific parameters
		chanTuneParams.params.paramsS.fec = MXL_HYDRA_FEC_AUTO;
		chanTuneParams.params.paramsS.modulation = MXL_HYDRA_MOD_AUTO;
		chanTuneParams.params.paramsS.rollOff = MXL_HYDRA_ROLLOFF_AUTO;
		break;

	case MXL_HYDRA_DSS :
		// DSS specific parameters
		chanTuneParams.params.paramsDSS.fec = MXL_HYDRA_FEC_1_2;
		break;
	}

	// channe tune
	mxlStatus = MxLWare_HYDRA_API_CfgDemodChanTune(MXL_DEVICE_ID, MXL_HYDRA_TUNER_ID_0, demodID, &chanTuneParams);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("MxLWare_HYDRA_API_CfgDemodChanTune for (%d) - Error (%d)\n", demodID, mxlStatus);
	}
	else
	{
		printk("Done!, MxLWare_HYDRA_API_CfgDemodChanTune for (%d) \n", demodID);
	}
		
	BrUser_delay(NULL, 200);
	
	return mxlStatus;
}

Dword DRV_MXL541C_getStatistic(Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int tsSrcIdx)
{
	Dword						tmp;
	MXL_HYDRA_DEMOD_ID_E		demodID;
	MXL_HYDRA_TUNER_ID_E		tunerId;
	MXL_HYDRA_DEMOD_LOCK_T		demodLock;
	MXL_HYDRA_TUNE_PARAMS_T		chanTuneParams;
	MXL_HYDRA_DEMOD_STATUS_T	berStats;
	SINT32						adcRssiPwr;
	uint64_t packetErrorCount;
	uint64_t totalPackets;
	uint64_t berCount;
	uint64_t berWindow;
	uint64_t mod;
	
	getstatisticrequest->statistic.signalLocked = 0;
	getstatisticrequest->statistic.signalPresented = 0;
	getstatisticrequest->statistic.signalQuality = 0;
	getstatisticrequest->statistic.signalStrength = 0;
	getstatisticrequest->snr = 0;
	getstatisticrequest->rf_power = 0;

	DRV_MXL541C_DEBUG("DRV_MXL541C_getStatistic tsSrcIdx = %x\n",tsSrcIdx);
	demodID = tsSrcIdx;
	tunerId = 0;
	DRV_MXL541C_DEBUG("DRV_MXL541C_getStatistic demodID = %x\n",demodID);
	DRV_MXL541C_DEBUG("DRV_MXL541C_getStatistic tunerId = %x\n",tunerId);

	BrUser_delay(NULL, 800);
	MxLWare_HYDRA_API_ReqDemodLockStatus(MXL_DEVICE_ID, demodID, &demodLock);
	getstatisticrequest->statistic.signalLocked = (demodLock.fecLock == MXL_E_TRUE)? True:False;
	getstatisticrequest->statistic.signalPresented = (demodLock.fecLock == MXL_E_TRUE)? True:False;
	if (demodLock.fecLock == MXL_E_TRUE)
	{
		printk("Cfg Clear Demod[%d] Error Counters.\n", demodID);
		MxLWare_HYDRA_API_CfgClearDemodErrorCounters(MXL_DEVICE_ID, demodID);

		MxLWare_HYDRA_API_ReqDemodChanParams(MXL_DEVICE_ID, demodID, &chanTuneParams);
		MxLWare_HYDRA_API_ReqDemodErrorCounters(MXL_DEVICE_ID, demodID, &berStats);

		if (chanTuneParams.standardMask == MXL_HYDRA_DVBS2)
		{
			printk("DVB S2 Lock\n");
			printk("SymbolRate = %d KSps\n", chanTuneParams.symbolRateKSps);
			printk("packetErrorCount = %d, totalPackets = %d\n", berStats.u.demodStatus_DVBS2.packetErrorCount, berStats.u.demodStatus_DVBS2.totalPackets);
			packetErrorCount = berStats.u.demodStatus_DVBS2.packetErrorCount;
			totalPackets = berStats.u.demodStatus_DVBS2.totalPackets;
			mod = do_div(packetErrorCount,totalPackets);
			printk("PER = %lld\n",packetErrorCount);
			//printk("PER = %lf\n", ((double)berStats.u.demodStatus_DVBS2.packetErrorCount / (double)berStats.u.demodStatus_DVBS2.totalPackets));
			//printk("PER = %.3e\n", ((double)berStats.u.demodStatus_DVBS2.packetErrorCount / (double)berStats.u.demodStatus_DVBS2.totalPackets));
		}
		else if (chanTuneParams.standardMask == MXL_HYDRA_DVBS)
		{
			printk("DVB S Lock\n");
			printk("SymbolRate = %d KSps\n", chanTuneParams.symbolRateKSps);
			printk("berCount = %d,  berWindow = %d\n", berStats.u.demodStatus_DVBS.berCount, berStats.u.demodStatus_DVBS.berWindow);
			berCount = berStats.u.demodStatus_DVBS.berCount;
			berWindow = berStats.u.demodStatus_DVBS.berWindow;
			mod = do_div(berCount,berWindow);
			printk("PER = %lld\n",berCount);
			//printk("BER = %lf\n", ((double)berStats.u.demodStatus_DVBS.berCount / (double)berStats.u.demodStatus_DVBS.berWindow));
			//printk("BER_Iter1 = %d %d\n", berStats.u.demodStatus_DVBS.berCount_Iter1, berStats.u.demodStatus_DVBS.berWindow_Iter1);
		}
		else
		{
			printk("????\n");
		}
	}
	else
	{
		printk("Deomd%d - UNLOCKED!  \n", demodID);
	}

	//RF Power
	//mxlStatus = MxLWare_HYDRA_API_ReqAdcRssiPower(MXL_DEVICE_ID, tunerId, &adcRssiPwr);
	mxlStatus = MxLWare_HYDRA_API_ReqAdcRssiPower(MXL_DEVICE_ID, MXL_HYDRA_TUNER_ID_0, &adcRssiPwr);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("Error code : %d\n", mxlStatus);
	}
	else
	{
		printk("RF Power = %d dB\n", adcRssiPwr / 1000);
		getstatisticrequest->statistic.signalStrength = (Byte)(adcRssiPwr / 1000);	
		getstatisticrequest->statistic.signalStrength = getstatisticrequest->statistic.signalStrength * 100 / 256;
		getstatisticrequest->rf_power = adcRssiPwr / 1000;
	}
	
	return mxlStatus;
}

Dword DRV_MXL541C_getChannelStatistic(Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int ts_idx)
{
	Dword						tmp;
	MXL_HYDRA_DEMOD_ID_E		demodID;
	MXL_HYDRA_TUNER_ID_E		tunerId;
	MXL_HYDRA_DEMOD_LOCK_T		demodLock;
	MXL_HYDRA_TUNE_PARAMS_T		chanTuneParams;
	MXL_HYDRA_DEMOD_STATUS_T	berStats;
	SINT32						adcRssiPwr;
	uint64_t packetErrorCount;
	uint64_t totalPackets;
	uint64_t berCount;
	uint64_t berWindow;
	uint64_t mod;

	DRV_MXL541C_DEBUG("DRV_MXL541C_getChannelStatistic tsSrcIdx = %x\n",tsSrcIdx);
	demodID = ts_idx;
	tunerId = 0;
	DRV_MXL541C_DEBUG("DRV_MXL541C_getChannelStatistic demodID = %x\n",demodID);
	DRV_MXL541C_DEBUG("DRV_MXL541C_getChannelStatistic tunerId = %x\n",tunerId);

	BrUser_delay(NULL, 800);
	MxLWare_HYDRA_API_ReqDemodLockStatus(MXL_DEVICE_ID, demodID, &demodLock);
	if (demodLock.fecLock == MXL_E_TRUE)
	{
		printk("Cfg Clear Demod[%d] Error Counters.\n", demodID);
		MxLWare_HYDRA_API_CfgClearDemodErrorCounters(MXL_DEVICE_ID, demodID);

		MxLWare_HYDRA_API_ReqDemodChanParams(MXL_DEVICE_ID, demodID, &chanTuneParams);
		MxLWare_HYDRA_API_ReqDemodErrorCounters(MXL_DEVICE_ID, demodID, &berStats);

		if (chanTuneParams.standardMask == MXL_HYDRA_DVBS2)
		{    
			printk("DVB S2 Lock\n");
			printk("SymbolRate = %d KSps\n", chanTuneParams.symbolRateKSps);
			printk("packetErrorCount = %d, totalPackets = %d\n", berStats.u.demodStatus_DVBS2.packetErrorCount, berStats.u.demodStatus_DVBS2.totalPackets);
			packetErrorCount = berStats.u.demodStatus_DVBS2.packetErrorCount;
			totalPackets = berStats.u.demodStatus_DVBS2.totalPackets;
			mod = do_div(packetErrorCount,totalPackets);
			printk("PER = %lld\n",packetErrorCount);
			//printk("PER = %lf\n", ((double)berStats.u.demodStatus_DVBS2.packetErrorCount / (double)berStats.u.demodStatus_DVBS2.totalPackets));
			//printk("PER = %.3e\n", ((double)berStats.u.demodStatus_DVBS2.packetErrorCount / (double)berStats.u.demodStatus_DVBS2.totalPackets));
		}
		else if (chanTuneParams.standardMask == MXL_HYDRA_DVBS)
		{
			printk("DVB S Lock\n");
			printk("SymbolRate = %d KSps\n", chanTuneParams.symbolRateKSps);
			printk("berCount = %d,  berWindow = %d\n", berStats.u.demodStatus_DVBS.berCount, berStats.u.demodStatus_DVBS.berWindow);
			berCount = berStats.u.demodStatus_DVBS.berCount;
			berWindow = berStats.u.demodStatus_DVBS.berWindow;
			mod = do_div(berCount,berWindow);
			printk("PER = %lld\n",berCount);
			//printk("BER = %lf\n", ((double)berStats.u.demodStatus_DVBS.berCount / (double)berStats.u.demodStatus_DVBS.berWindow));
			//printk("BER_Iter1 = %d %d\n", berStats.u.demodStatus_DVBS.berCount_Iter1, berStats.u.demodStatus_DVBS.berWindow_Iter1);
		}
		else
		{
			printk("????\n");
		}
	}
	else
	{
		printk("Deomd%d - UNLOCKED!  \n", demodID);
	}

	//RF Power
	//mxlStatus = MxLWare_HYDRA_API_ReqAdcRssiPower(MXL_DEVICE_ID, tunerId, &adcRssiPwr);
	mxlStatus = MxLWare_HYDRA_API_ReqAdcRssiPower(MXL_DEVICE_ID, MXL_HYDRA_TUNER_ID_0, &adcRssiPwr);
	if (mxlStatus != MXL_E_SUCCESS)
	{
		printk("Error code : %d\n", mxlStatus);
	}
	else
	{
		printk("RF Power = %d dB\n", adcRssiPwr / 1000);
	}
	
	return mxlStatus;
}

Dword DRV_MXL541C_multi_acquireChannel(Device_Context *DC, AcquireChannelRequest *request, Byte br_idx, Byte tsSrcIdx)
{
	Dword			tmp;
	Dword			frequencyKHz;
	Dword			symbol_rateKSps;
	MXL_HYDRA_DEMOD_ID_E	demodID;
	MXL_HYDRA_TUNE_PARAMS_T	chanTuneParams;
	MXL_HYDRA_BCAST_STD_E	std;
	MXL_HYDRA_DEMOD_SCRAMBLE_INFO_T	demodScramblingCode;
	int i;

	for (i = 0; i < 4; i++)
	{
		demodID = i;
		/*
		std = MXL_HYDRA_DVBS;
		frequencyKHz = 2000000;
		symbol_rateKSps = 31000;
		*/
		
		std = MXL_HYDRA_DVBS2;
		frequencyKHz = 1900000;
		symbol_rateKSps = 27500;
		

		// Channel tune
		// Common parameters for DVB-S2, DVB-S & DSS modes
		chanTuneParams.standardMask = std;
		chanTuneParams.frequencyInHz = frequencyKHz * 1000;
		chanTuneParams.freqSearchRangeKHz = 5000;		// 5MHz
		chanTuneParams.symbolRateKSps = symbol_rateKSps;
		chanTuneParams.spectrumInfo = MXL_HYDRA_SPECTRUM_NON_INVERTED;

		// parameters specific to standard
		switch (chanTuneParams.standardMask)
		{
		default:
		case MXL_HYDRA_DVBS2:
			// DVB-S2 specific parameters
			chanTuneParams.params.paramsS2.fec = MXL_HYDRA_FEC_AUTO;
			chanTuneParams.params.paramsS2.modulation = MXL_HYDRA_MOD_AUTO;
			chanTuneParams.params.paramsS2.pilots = MXL_HYDRA_PILOTS_AUTO;
			chanTuneParams.params.paramsS2.rollOff = MXL_HYDRA_ROLLOFF_AUTO;

			// For DVB-S2 standard program default scrambling code
			demodScramblingCode.scrambleCode = 0x01;
			demodScramblingCode.scrambleSequence[0] = 0x00;
			demodScramblingCode.scrambleSequence[1] = 0x00;
			demodScramblingCode.scrambleSequence[2] = 0x00;
			demodScramblingCode.scrambleSequence[3] = 0x00;
			demodScramblingCode.scrambleSequence[4] = 0x00;
			demodScramblingCode.scrambleSequence[5] = 0x00;
			demodScramblingCode.scrambleSequence[6] = 0x00;
			demodScramblingCode.scrambleSequence[7] = 0x00;
			demodScramblingCode.scrambleSequence[8] = 0x00;
			demodScramblingCode.scrambleSequence[9] = 0x00;
			demodScramblingCode.scrambleSequence[10] = 0x00;
			demodScramblingCode.scrambleSequence[11] = 0x00;

			// program default scramble code
			mxlStatus = MxLWare_HYDRA_API_CfgDemodScrambleCode(MXL_DEVICE_ID, demodID, &demodScramblingCode);
			if (mxlStatus != MXL_E_SUCCESS)
			{
				printk("MxLWare_HYDRA_API_CfgDemodScrambleCode for (%d) - Error (%d)\n", demodID, mxlStatus);
			}
			break;

		case MXL_HYDRA_DVBS:
			// DVB-S specific parameters
			chanTuneParams.params.paramsS.fec = MXL_HYDRA_FEC_AUTO;
			chanTuneParams.params.paramsS.modulation = MXL_HYDRA_MOD_AUTO;
			chanTuneParams.params.paramsS.rollOff = MXL_HYDRA_ROLLOFF_AUTO;
			break;

		case MXL_HYDRA_DSS:
			// DSS specific parameters
			chanTuneParams.params.paramsDSS.fec = MXL_HYDRA_FEC_1_2;
			break;
		}

		// channe tune
		mxlStatus = MxLWare_HYDRA_API_CfgDemodChanTune(MXL_DEVICE_ID, MXL_HYDRA_TUNER_ID_0, demodID, &chanTuneParams);
		if (mxlStatus != MXL_E_SUCCESS)
		{
			printk("MxLWare_HYDRA_API_CfgDemodChanTune for (%d) - Error (%d)\n", demodID, mxlStatus);
		}
		else
		{
			printk("Done!, MxLWare_HYDRA_API_CfgDemodChanTune for (%d) \n", demodID);
		}

		BrUser_delay(NULL, 200);
	}	

	return mxlStatus;
}

Dword DRV_MXL541C_multi_getStatistic(Device_Context *DC, Statistic *statistic, Byte br_idx, Byte tsSrcIdx)
{
	Dword						tmp;
	MXL_HYDRA_DEMOD_ID_E		demodID;
	MXL_HYDRA_TUNER_ID_E		tunerID;
	MXL_HYDRA_DEMOD_LOCK_T		demodLock;			
	MXL_HYDRA_TUNE_PARAMS_T		chanTuneParams;
	MXL_HYDRA_DEMOD_STATUS_T	berStats;
	Dword						testCount = 500;
	SINT32						adcRssiPwr;
	int i;

	while (testCount > 0)
	{
		for (i = 0; i < 4; i++)
		{
			demodID = i;
			
			BrUser_delay(NULL, 800);
			mxlStatus = MxLWare_HYDRA_API_ReqDemodLockStatus(MXL_DEVICE_ID, demodID, &demodLock);
			if (mxlStatus != MXL_E_SUCCESS)
			{
				printk("Error code : %d\n", mxlStatus);
				goto exit;
			}

			if (demodLock.fecLock == MXL_E_TRUE)
			{
				//printk("Cfg Clear Demod[%d] Error Counters.\n", demodID);
				mxlStatus = MxLWare_HYDRA_API_CfgClearDemodErrorCounters(MXL_DEVICE_ID, demodID);
				if (mxlStatus != MXL_E_SUCCESS)
				{
					printk("Error code : %d\n", mxlStatus);
					goto exit;
				}
				mxlStatus = MxLWare_HYDRA_API_ReqDemodChanParams(MXL_DEVICE_ID, demodID, &chanTuneParams);
				if (mxlStatus != MXL_E_SUCCESS)
				{
					printk("Error code : %d\n", mxlStatus);
					goto exit;
				}
				mxlStatus = MxLWare_HYDRA_API_ReqDemodErrorCounters(MXL_DEVICE_ID, demodID, &berStats);
				if (mxlStatus != MXL_E_SUCCESS)
				{
					printk("Error code : %d\n", mxlStatus);
					goto exit;
				}

				if (chanTuneParams.standardMask == MXL_HYDRA_DVBS2)
				{
					printk("#%d DVB S2 Lock\n", i);
					/*
					printk("SymbolRate = %d KSps\n", chanTuneParams.symbolRateKSps);
					printk("packetErrorCount = %d, totalPackets = %d\n", berStats.u.demodStatus_DVBS2.packetErrorCount, berStats.u.demodStatus_DVBS2.totalPackets);
					printk("PER = %lf\n", ((double)berStats.u.demodStatus_DVBS2.packetErrorCount / (double)berStats.u.demodStatus_DVBS2.totalPackets));
					*/
					//printk("PER = %.3e\n", ((double)berStats.u.demodStatus_DVBS2.packetErrorCount / (double)berStats.u.demodStatus_DVBS2.totalPackets));
				}
				else if (chanTuneParams.standardMask == MXL_HYDRA_DVBS)
				{
					printk("#%d DVB S Lock\n", i);
					/*
					printk("SymbolRate = %d KSps\n", chanTuneParams.symbolRateKSps);
					printk("berCount = %d,  berWindow = %d\n", berStats.u.demodStatus_DVBS.berCount, berStats.u.demodStatus_DVBS.berWindow);
					printk("BER = %lf\n", ((double)berStats.u.demodStatus_DVBS.berCount / (double)berStats.u.demodStatus_DVBS.berWindow));
					//printk("BER_Iter1 = %d %d\n", berStats.u.demodStatus_DVBS.berCount_Iter1, berStats.u.demodStatus_DVBS.berWindow_Iter1);
					*/
				}
				else
				{
					printk("????\n");
				}
			}
			else
			{
				printk("Deomd %d - UNLOCKED!  \n", demodID);
				goto exit;
			}

			//RF Power
			tunerID = i;
			mxlStatus = MxLWare_HYDRA_API_ReqAdcRssiPower(MXL_DEVICE_ID, tunerID, &adcRssiPwr);
			if (mxlStatus != MXL_E_SUCCESS)
			{
				printk("Error code : %d\n", mxlStatus);
				goto exit;
			}
			else
			{
				printk("RF Power = %d dB\n", adcRssiPwr / 1000);
			}

			//Check wrong value
			if (adcRssiPwr / 1000 > 100 || adcRssiPwr / 1000 < -100)
			{
				printk("adcRssiPwr = 0x%08X\n", adcRssiPwr);
				goto exit;
			}
		}

		testCount--;
	}	
	return mxlStatus;
	
exit:
	printk("something wrong!!!!!!!!!\n");
	return mxlStatus;
}

Dword DRV_MXL541C_DISEQC_COMMUNICATION(Device_Context *DC, Statistic *statistic, Byte br_idx, Byte tsSrcIdx)
{
	MXL_HYDRA_DISEQC_TX_MSG_T	diseqcMsg;
	UINT32						intrStatus, intrMask, intrTimeout;
	UINT32						diseqcStatus;
	UINT8			DRV_MXL541C_DEBUG		;	
	UINT8			 j = 0;
	MXL_HYDRA_AUX_CTRL_MODE_E	lnbInterface = MXL_HYDRA_AUX_CTRL_MODE_DISEQC;;


	if (MXL_HYDRA_AUX_CTRL_MODE_DISEQC == lnbInterface)
	{
		mxlStatus = MxLWare_HYDRA_API_CfgDiseqcContinuousToneCtrl(MXL_DEVICE_ID, MXL_HYDRA_DISEQC_ID_0, MXL_E_OFF);
		if (MXL_E_SUCCESS == mxlStatus)
		{
			printk("DiseqC Continuous Tone is stopped\n");
			diseqcMsg.diseqcId = MXL_HYDRA_DISEQC_ID_0;
			diseqcMsg.toneBurst = MXL_HYDRA_DISEQC_TONE_NONE;
			diseqcMsg.nbyte = 32;
			for (j = 0; j < diseqcMsg.nbyte; j++)
			{
				diseqcMsg.bufMsg[j] = 0x12 + j;
			}
			mxlStatus = MxLWare_HYDRA_API_CfgDiseqcWrite(MXL_DEVICE_ID, &diseqcMsg);
			if (MXL_E_SUCCESS == mxlStatus)
			{
				printk("Done! - MxLWare_HYDRA_API_CfgDiseqcWrite - No Tone Burst \n");
				// check for DiseqC Interrupt before reading DiseqC Status
				intrTimeout = 0;
				while (MXL_E_SUCCESS == (mxlStatus = MxLWare_HYDRA_API_ReqDevInterruptStatus(MXL_DEVICE_ID, &intrStatus, &intrMask)) && (intrTimeout++ < DISEQC_INTERRUPT_STATUS_TIMEOUT))
				{
					if (DISEQC_INTERRUPT_BIT & intrStatus & intrMask) // check if DiseqC interrupt bit is set
					{
						break;
					}
					BrUser_delay(NULL, 1);
				}
				if ((MXL_E_SUCCESS == mxlStatus) && (intrTimeout < DISEQC_INTERRUPT_STATUS_TIMEOUT))
				{
					mxlStatus = MxLWare_HYDRA_API_ReqDiseqcStatus(MXL_DEVICE_ID, MXL_HYDRA_DISEQC_ID_0, &diseqcStatus);
					if ((MXL_E_SUCCESS == mxlStatus) && (diseqcStatus & MXL_HYDRA_DISEQC_STATUS_TX_DONE))
					{
						printk("DiseqC Status - TX Done (No Tone)\n");
						BrUser_delay(NULL, 15); // Delay required before initiating next transmission
						diseqcMsg.diseqcId = MXL_HYDRA_DISEQC_ID_0;
						diseqcMsg.toneBurst = MXL_HYDRA_DISEQC_TONE_SA;
						diseqcMsg.nbyte = 32;
						for (j = 0; j < diseqcMsg.nbyte; j++)
						{
							diseqcMsg.bufMsg[j] = 0x12 + j;
						}
						mxlStatus = MxLWare_HYDRA_API_CfgDiseqcWrite(MXL_DEVICE_ID, &diseqcMsg);
						if (MXL_E_SUCCESS == mxlStatus)
						{
							printk("Done! - MxLWare_HYDRA_API_CfgDiseqcWrite - Tone Burst \n");
							// check for DiseqC Interrupt before reading DiseqC Status
							intrTimeout = 0;
							while (MXL_E_SUCCESS == (mxlStatus = MxLWare_HYDRA_API_ReqDevInterruptStatus(MXL_DEVICE_ID, &intrStatus, &intrMask)) && (intrTimeout++ < DISEQC_INTERRUPT_STATUS_TIMEOUT))
							{
								if (DISEQC_INTERRUPT_BIT & intrStatus & intrMask) // check if DiseqC interrupt bit is set
								{
									break;
								}
								BrUser_delay(NULL, 1);
							}
							if ((MXL_E_SUCCESS == mxlStatus) && (intrTimeout < DISEQC_INTERRUPT_STATUS_TIMEOUT))
							{
								mxlStatus = MxLWare_HYDRA_API_ReqDiseqcStatus(MXL_DEVICE_ID, MXL_HYDRA_DISEQC_ID_0, &diseqcStatus);
								if ((MXL_E_SUCCESS == mxlStatus) && (diseqcStatus & MXL_HYDRA_DISEQC_STATUS_TX_DONE))
								{
									printk("DiseqC Status - TX Done (Tone Burst)\n");
									BrUser_delay(NULL, 15); // Delay required before initiating next transmission
									mxlStatus = MxLWare_HYDRA_API_CfgDiseqcContinuousToneCtrl(MXL_DEVICE_ID, MXL_HYDRA_DISEQC_ID_0, MXL_E_ON);
									if (MXL_E_SUCCESS == mxlStatus)
									{
										printk("DiseqC Continuous Tone is started\n");
									}
									else
									{
										printk("Error in starting DiseqC Continuous Tone - Status %d\n", mxlStatus);
									}
								}
								else
								{
									printk("Error - DiseqC Status - TX Done (Tone Burst) - Status %d\n", mxlStatus);
								}
							}
							else
							{
								printk("Error - DiseqC Status - DiseqC Interrupt timeout - Status %d\n", mxlStatus);
							}
						}
						else
						{
							printk("Error! - MxLWare_HYDRA_API_CfgDiseqcWrite - Tone Burst - Status %d\n", mxlStatus);
						}
					}
					else
				{
						printk("Error - DiseqC Status - TX Done (No Tone) - Status %d\n", mxlStatus);
					}
				}
				else
				{
					printk("Error - DiseqC Status - DiseqC Interrupt timeout - Status %d\n", mxlStatus);
				}
			}
			else
			{
				printk("Error! - MxLWare_HYDRA_API_CfgDiseqcWrite - No Tone Burst - Status %d\n", mxlStatus);
			}
		}
		else
		{
			printk("Error in stopping DiseqC Continuous Tone - Status %d\n", mxlStatus);
		}
	}
	return mxlStatus;
}

Dword DRV_MXL541C_isLocked(Device_Context *DC, Bool* locked, int br_idx, int tsSrcIdx)
{
	Dword						tmp;
	MXL_HYDRA_DEMOD_ID_E		demodID;
	MXL_HYDRA_TUNER_ID_E		tunerId;
	MXL_HYDRA_DEMOD_LOCK_T		demodLock;
	MXL_HYDRA_TUNE_PARAMS_T		chanTuneParams;
	MXL_HYDRA_DEMOD_STATUS_T	berStats;
	SINT32						adcRssiPwr;
	uint64_t packetErrorCount;
	uint64_t totalPackets;
	uint64_t berCount;
	uint64_t berWindow;
	uint64_t mod;

	DRV_MXL541C_DEBUG("DRV_MXL541C_isLocked tsSrcIdx = %x\n",tsSrcIdx);
	demodID = tsSrcIdx;
	tunerId = 0;
	DRV_MXL541C_DEBUG("DRV_MXL541C_isLocked demodID = %x\n",demodID);
	DRV_MXL541C_DEBUG("DRV_MXL541C_isLocked tunerId = %x\n",tunerId);
	
	*locked = False;

	BrUser_delay(NULL, 800);
	MxLWare_HYDRA_API_ReqDemodLockStatus(MXL_DEVICE_ID, demodID, &demodLock);
	if (demodLock.fecLock == MXL_E_TRUE)
	{
		printk("Cfg Clear Demod[%d] Error Counters.\n", demodID);
		MxLWare_HYDRA_API_CfgClearDemodErrorCounters(MXL_DEVICE_ID, demodID);

		MxLWare_HYDRA_API_ReqDemodChanParams(MXL_DEVICE_ID, demodID, &chanTuneParams);
		MxLWare_HYDRA_API_ReqDemodErrorCounters(MXL_DEVICE_ID, demodID, &berStats);

		if (chanTuneParams.standardMask == MXL_HYDRA_DVBS2)
		{
			printk("DVB S2 Lock\n");
			*locked = true;
		}
		else if (chanTuneParams.standardMask == MXL_HYDRA_DVBS)
		{
			printk("DVB S Lock\n");
			*locked = true;
		}
		else
		{
			printk("????\n");
			*locked = False;
		}
	}
	else
	{
		printk("Deomd%d - UNLOCKED!  \n", demodID);
		*locked = False;
	}
	
	return mxlStatus;
}

