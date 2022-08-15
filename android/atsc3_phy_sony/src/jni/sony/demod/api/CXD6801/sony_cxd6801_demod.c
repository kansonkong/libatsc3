/*------------------------------------------------------------------------------
  Copyright 2016-2019 Sony Semiconductor Solutions Corporation

  Last Updated    : 2019/06/28
  Modification ID : 4d12c6351929bcae29649d686be7d406fd4718c8
------------------------------------------------------------------------------*/

#include "sony_cxd6801_integ.h"
#include "sony_cxd6801_demod.h"
#include "sony_cxd6801_stdlib.h"

#ifdef SONY_CXD6801_DEMOD_SUPPORT_DVBT
#include "sony_cxd6801_demod_dvbt.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_DVBC
#include "sony_cxd6801_demod_dvbc.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_DVBT2
#include "sony_cxd6801_demod_dvbt2.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_DVBC2
#include "sony_cxd6801_demod_dvbc2.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_DVBS_S2
#include "sony_cxd6801_demod_dvbs_s2.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ISDBT
#include "sony_cxd6801_demod_isdbt.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ISDBC
#include "sony_cxd6801_demod_isdbc.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ISDBS
#include "sony_cxd6801_demod_isdbs.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ISDBS3
#include "sony_cxd6801_demod_isdbs3.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_J83B
#include "sony_cxd6801_demod_j83b.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC
#include "sony_cxd6801_demod_atsc.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC3
#include "sony_cxd6801_demod_atsc3.h"
#endif

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Configure the demodulator from any state to Sleep state.
        This is used as a demodulator reset, all demodulator configuration
        settings will be lost.
*/
static sony_cxd6801_result_t XtoSL(sony_cxd6801_demod_t * pDemod);

/**
 @brief Configure the demodulator from Shutdown to Sleep state
*/
static sony_cxd6801_result_t SDtoSL(sony_cxd6801_demod_t * pDemod);

/**
 @brief Configure the demodulator from any Sleep state to Shutdown
*/
static sony_cxd6801_result_t SLtoSD(sony_cxd6801_demod_t * pDemod);

/**
 @brief Iterate through the demodulator configuration memory table and write
        each entry to the device.  This is called automatically during a
        transition from ::SONY_DEMOD_STATE_SHUTDOWN to
        ::SONY_DEMOD_STATE_SLEEP state.
        This function also called in sony_demod_Initialize to restore
        previous setting if the user calls sony_demod_Initialize again.
*/
static sony_cxd6801_result_t loadConfigMemory(sony_cxd6801_demod_t * pDemod);

/**
 @brief Save an entry into the demodulator configuration memory table.

 @param pDemod The demodulator instance.
 @param slaveAddress Slave address of configuration setting
 @param bank Demodulator bank of configuration setting
 @param registerAddress Register address of configuration setting
 @param value The value being written to this register
 @param mask The bit mask used on the register
*/
static sony_cxd6801_result_t setConfigMemory(sony_cxd6801_demod_t * pDemod,
                                      uint8_t slaveAddress,
                                      uint8_t bank,
                                      uint8_t registerAddress,
                                      uint8_t value,
                                      uint8_t bitMask);

/**
 @brief Conditional I2C write function for config.

 @param pDemod The demodulator instance.
 @param slaveAddress Slave address of configuration setting
 @param bank Demodulator bank of configuration setting
 @param registerAddress Register address of configuration setting
 @param value The value being written to this register
 @param mask The bit mask used on the register
*/
static sony_cxd6801_result_t setRegisterBitsForConfig(sony_cxd6801_demod_t * pDemod,
                                               uint8_t slaveAddress,
                                               uint8_t bank,
                                               uint8_t registerAddress,
                                               uint8_t value,
                                               uint8_t bitMask);

/*------------------------------------------------------------------------------
 System Support Dependent Function Removal
------------------------------------------------------------------------------*/
#ifndef SONY_CXD6801_DEMOD_SUPPORT_DVBC2
#define sony_cxd6801_demod_dvbc2_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONY_CXD6801_DEMOD_SUPPORT_DVBT2
#define sony_cxd6801_demod_dvbt2_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONY_CXD6801_DEMOD_SUPPORT_DVBC
#define sony_cxd6801_demod_dvbc_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONY_CXD6801_DEMOD_SUPPORT_DVBT
#define sony_cxd6801_demod_dvbt_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONY_CXD6801_DEMOD_SUPPORT_DVBS_S2
#define sony_cxd6801_demod_dvbs_s2_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONY_CXD6801_DEMOD_SUPPORT_ISDBT
#define sony_cxd6801_demod_isdbt_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONY_CXD6801_DEMOD_SUPPORT_ISDBC
#define sony_cxd6801_demod_isdbc_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONY_CXD6801_DEMOD_SUPPORT_ISDBS
#define sony_cxd6801_demod_isdbs_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONY_CXD6801_DEMOD_SUPPORT_ISDBS3
#define sony_cxd6801_demod_isdbs3_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONY_CXD6801_DEMOD_SUPPORT_J83B
#define sony_cxd6801_demod_j83b_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONY_CXD6801_DEMOD_SUPPORT_ATSC
#define sony_cxd6801_demod_atsc_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

#ifndef SONY_CXD6801_DEMOD_SUPPORT_ATSC3
#define sony_cxd6801_demod_atsc3_Sleep(pDemod) SONY_CXD6801_RESULT_ERROR_NOSUPPORT
#endif

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_demod_Create(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_create_param_t * pCreateParam,
	sony_cxd6801_i2c_t * pDemodI2c)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("sony_demod_Create");

    if ((!pDemod) || (!pCreateParam) || (!pDemodI2c)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    switch (pCreateParam->i2cAddressSLVT) {
    case 0xC8:
    case 0xCA:
    case 0xD8:
    case 0xDA:
    case 0xDC:
    case 0xDE:
    case 0xCC:
    case 0xCE:
        break;
    default:
        /* Invalid I2C slave address */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    sony_cxd6801_memset (pDemod, 0, sizeof (sony_cxd6801_demod_t)); /* Zero filled */

    pDemod->xtalFreq = pCreateParam->xtalFreq;
    pDemod->i2cAddressSLVT = pCreateParam->i2cAddressSLVT;
    pDemod->i2cAddressSLVX = pCreateParam->i2cAddressSLVT + 4;
    pDemod->i2cAddressSLVR = pCreateParam->i2cAddressSLVT - 0x40;
    pDemod->i2cAddressSLVM = pCreateParam->i2cAddressSLVT - 0xA8;
    pDemod->pI2c = pDemodI2c;
    pDemod->tunerI2cConfig = pCreateParam->tunerI2cConfig;

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC
    pDemod->atscCoreDisable = pCreateParam->atscCoreDisable;
#else
    pDemod->atscCoreDisable = 1;
#endif

    /* Set initial (non-zero) value */
    pDemod->serialTSClockModeContinuous = 1;
#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
    /* Set the Sony silicon tuner optimized IF frequency. */
	pDemod->iffreqConfig.configDVBT_5 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.6);
	pDemod->iffreqConfig.configDVBT_6 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.6);
	pDemod->iffreqConfig.configDVBT_7 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(4.2);
	pDemod->iffreqConfig.configDVBT_8 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(4.8);

	pDemod->iffreqConfig.configDVBT2_1_7 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.5);
	pDemod->iffreqConfig.configDVBT2_5 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.6);
	pDemod->iffreqConfig.configDVBT2_6 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.6);
	pDemod->iffreqConfig.configDVBT2_7 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(4.2);
	pDemod->iffreqConfig.configDVBT2_8 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(4.8);

	pDemod->iffreqConfig.configDVBC_6 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.7);
	pDemod->iffreqConfig.configDVBC_7 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(4.9);
	pDemod->iffreqConfig.configDVBC_8 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(4.9);
	pDemod->iffreqConfig.configDVBC2_6 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.7);
	pDemod->iffreqConfig.configDVBC2_8 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(4.9);

	pDemod->iffreqConfig.configATSC = SONY_CXD6801_DEMOD_ATSC_MAKE_IFFREQ_CONFIG(3.7);

	pDemod->iffreqConfig.configATSC3_6 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.6);
	pDemod->iffreqConfig.configATSC3_7 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(4.2);
	pDemod->iffreqConfig.configATSC3_8 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(4.8);

	pDemod->iffreqConfig.configISDBT_6 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.55);
	pDemod->iffreqConfig.configISDBT_7 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(4.15);
	pDemod->iffreqConfig.configISDBT_8 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(4.75);

	pDemod->iffreqConfig.configISDBC_6 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.7);
	pDemod->iffreqConfig.configJ83B_5_06_5_36 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.7);
	pDemod->iffreqConfig.configJ83B_5_60 = SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(3.75);

	pDemod->tunerOptimize = SONY_CXD6801_DEMOD_TUNER_OPTIMIZE_SONYSILICON;
	pDemod->serialTSClkFreqTerrCable = SONY_CXD6801_DEMOD_SERIAL_TS_CLK_MID_FULL;
	pDemod->twoBitParallelTSClkFreqTerrCable = SONY_CXD6801_DEMOD_2BIT_PARALLEL_TS_CLK_MID;
    pDemod->atsc3AutoSpectrumInv = 1;
    pDemod->atsc3CWDetection = 1;
    pDemod->atscNoSignalThresh = 0x7FFB61;
    pDemod->atscSignalThresh = 0x7C4926;
#endif
#ifdef SONY_CXD6801_DEMOD_SUPPORT_SAT
    pDemod->serialTSClkFreqSat = SONY_DEMOD_SERIAL_TS_CLK_MID_FULL;
    pDemod->twoBitParallelTSClkFreqSat = SONY_DEMOD_2BIT_PARALLEL_TS_CLK_MID;
    pDemod->isSinglecableSpectrumInv = 1;
    pDemod->dvbss2PowerSmooth = 1;
#ifdef SONY_DEMOD_SUPPORT_DVBS_S2_BLINDSCAN_VER2
    pDemod->dvbss2BlindScanVersion = SONY_DEMOD_DVBSS2_BLINDSCAN_VERSION2;
#endif
#endif
#ifdef SONY_CXD6801_DEMOD_SUPPORT_TLV
    pDemod->serialTLVClockModeContinuous = 1;
    pDemod->serialTLVClkFreqTerrCable = SONY_DEMOD_SERIAL_TS_CLK_MID_FULL;
    pDemod->serialTLVClkFreqSat = SONY_DEMOD_SERIAL_TS_CLK_MID_FULL;
    pDemod->twoBitParallelTLVClkFreqTerrCable = SONY_DEMOD_2BIT_PARALLEL_TS_CLK_MID;
    pDemod->twoBitParallelTLVClkFreqSat = SONY_DEMOD_2BIT_PARALLEL_TS_CLK_MID;
#endif
#ifdef SONY_CXD6801_DEMOD_SUPPORT_ALP
    pDemod->serialALPClockModeContinuous = 1;
#endif

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_Initialize(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_demod_Initialize");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Reset status values */
	pDemod->state = SONY_CXD6801_DEMOD_STATE_UNKNOWN;
	pDemod->system = SONY_CXD6801_DTV_SYSTEM_UNKNOWN;
	pDemod->chipId = SONY_CXD6801_DEMOD_CHIP_ID_UNKNOWN;
#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
	pDemod->bandwidth = SONY_CXD6801_DTV_BW_UNKNOWN;
    pDemod->scanMode = 0;
	pDemod->isdbtEwsState = SONY_CXD6801_DEMOD_ISDBT_EWS_STATE_NORMAL;
	pDemod->atsc3EasState = SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_NORMAL;
    pDemod->atsc3AutoSpectrumInv_flag = 0;
    pDemod->atsc3CWDetection_flag = 0;
	pDemod->atscCPUState = SONY_CXD6801_DEMOD_ATSC_CPU_STATE_IDLE;
#endif
#ifdef SONY_CXD6801_DEMOD_SUPPORT_SAT
    pDemod->dvbss2ScanMode = 0;
#endif

    result = sony_cxd6801_demod_ChipID (pDemod, &(pDemod->chipId));
	if (result != SONY_CXD6801_RESULT_OK) {
		pDemod->state = SONY_CXD6801_DEMOD_STATE_INVALID;
        SONY_CXD6801_TRACE_RETURN (result);
    }

    switch (pDemod->chipId) {
	case SONY_CXD6801_DEMOD_CHIP_ID_CXD2856:
	case SONY_CXD6801_DEMOD_CHIP_ID_CXD2857:
	case SONY_CXD6801_DEMOD_CHIP_ID_CXD2878:
	case SONY_CXD6801_DEMOD_CHIP_ID_CXD2879:
	case SONY_CXD6801_DEMOD_CHIP_ID_CXD6802:
        break;

    default:
		pDemod->state = SONY_CXD6801_DEMOD_STATE_INVALID;
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

	if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
        /* CXD2856 family does not have ATSC 1.0 core. */
        pDemod->atscCoreDisable = 1;
    }

    /* Initialize causes demodulator register reset */
    result = XtoSL (pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
		pDemod->state = SONY_CXD6801_DEMOD_STATE_INVALID;
        SONY_CXD6801_TRACE_RETURN (result);
    }

	pDemod->state = SONY_CXD6801_DEMOD_STATE_SLEEP;

    /* Setup tuner I2C bus */
    result = sony_cxd6801_demod_TunerI2cEnable (pDemod, pDemod->tunerI2cConfig);
	if (result != SONY_CXD6801_RESULT_OK) {
		pDemod->state = SONY_CXD6801_DEMOD_STATE_INVALID;
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Load Config memory to restore previous settings */
    result = loadConfigMemory (pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
		pDemod->state = SONY_CXD6801_DEMOD_STATE_INVALID;
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_Sleep(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_demod_Sleep");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pDemod->state == SONY_CXD6801_DEMOD_STATE_SHUTDOWN) {
        result = SDtoSL (pDemod);
		if (result != SONY_CXD6801_RESULT_OK) {
			pDemod->state = SONY_CXD6801_DEMOD_STATE_INVALID;
            SONY_CXD6801_TRACE_RETURN (result);
        }

		pDemod->state = SONY_CXD6801_DEMOD_STATE_SLEEP;

        /* Load Config memory if returning from Shutdown state */
        result = loadConfigMemory (pDemod);
		if (result != SONY_CXD6801_RESULT_OK) {
			pDemod->state = SONY_CXD6801_DEMOD_STATE_INVALID;
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }
	else if (pDemod->state == SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        switch (pDemod->system) {
		case SONY_CXD6801_DTV_SYSTEM_DVBT:
			result = sony_cxd6801_demod_dvbt_Sleep(pDemod);
            break;

		case SONY_CXD6801_DTV_SYSTEM_DVBT2:
			result = sony_cxd6801_demod_dvbt2_Sleep(pDemod);
            break;

		case SONY_CXD6801_DTV_SYSTEM_DVBC:
			result = sony_cxd6801_demod_dvbc_Sleep(pDemod);
            break;

		case SONY_CXD6801_DTV_SYSTEM_DVBC2:
			result = sony_cxd6801_demod_dvbc2_Sleep(pDemod);
            break;

		case SONY_CXD6801_DTV_SYSTEM_DVBS:
		case SONY_CXD6801_DTV_SYSTEM_DVBS2:
		case SONY_CXD6801_DTV_SYSTEM_ANY: /* Tune (S/S2 auto), TuneSRS, BlindScan */
			result = sony_cxd6801_demod_dvbs_s2_Sleep(pDemod);
            break;

		case SONY_CXD6801_DTV_SYSTEM_ISDBT:
			result = sony_cxd6801_demod_isdbt_Sleep(pDemod);
            break;

		case SONY_CXD6801_DTV_SYSTEM_ISDBC:
			result = sony_cxd6801_demod_isdbc_Sleep(pDemod);
            break;

		case SONY_CXD6801_DTV_SYSTEM_ISDBS:
			result = sony_cxd6801_demod_isdbs_Sleep(pDemod);
            break;

		case SONY_CXD6801_DTV_SYSTEM_ISDBS3:
			result = sony_cxd6801_demod_isdbs3_Sleep(pDemod);
            break;

		case SONY_CXD6801_DTV_SYSTEM_J83B:
			result = sony_cxd6801_demod_j83b_Sleep(pDemod);
            break;

		case SONY_CXD6801_DTV_SYSTEM_ATSC:
			result = sony_cxd6801_demod_atsc_Sleep(pDemod);
            break;

		case SONY_CXD6801_DTV_SYSTEM_ATSC3:
			result = sony_cxd6801_demod_atsc3_Sleep(pDemod);
            break;

        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

		if (result != SONY_CXD6801_RESULT_OK) {
			pDemod->state = SONY_CXD6801_DEMOD_STATE_INVALID;
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }
	else if (pDemod->state == SONY_CXD6801_DEMOD_STATE_SLEEP) {
        /* Do nothing */
    }
    else {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	pDemod->state = SONY_CXD6801_DEMOD_STATE_SLEEP;
	pDemod->system = SONY_CXD6801_DTV_SYSTEM_UNKNOWN;

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_Shutdown(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_demod_Shutdown");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pDemod->state == SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* First transition to Sleep state */
        result = sony_cxd6801_demod_Sleep (pDemod);
		if (result != SONY_CXD6801_RESULT_OK) {
			pDemod->state = SONY_CXD6801_DEMOD_STATE_INVALID;
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Then transfer to SHUTDOWN state */
        result = SLtoSD (pDemod);
		if (result != SONY_CXD6801_RESULT_OK) {
			pDemod->state = SONY_CXD6801_DEMOD_STATE_INVALID;
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }
	else if (pDemod->state == SONY_CXD6801_DEMOD_STATE_SLEEP) {
        /* Transfer to Shutdown state */
        result = SLtoSD (pDemod);
		if (result != SONY_CXD6801_RESULT_OK) {
			pDemod->state = SONY_CXD6801_DEMOD_STATE_INVALID;
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }
	else if (pDemod->state == SONY_CXD6801_DEMOD_STATE_SHUTDOWN) {
        /* Do Nothing */
    }
    else {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	pDemod->state = SONY_CXD6801_DEMOD_STATE_SHUTDOWN;
	pDemod->system = SONY_CXD6801_DTV_SYSTEM_UNKNOWN;

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_TuneEnd(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_demod_TuneEnd");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC
	if (pDemod->system == SONY_CXD6801_DTV_SYSTEM_ATSC) {
		result = sony_cxd6801_demod_atsc_TuneEnd(pDemod);
        SONY_CXD6801_TRACE_RETURN (result);
    }
#endif

	result = sony_cxd6801_demod_SoftReset(pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

	result = sony_cxd6801_demod_SetStreamOutput(pDemod, 1);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_SoftReset(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_demod_SoftReset");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP)) {
        /* This api is accepted in Sleep and Active states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC
	if (pDemod->system == SONY_CXD6801_DTV_SYSTEM_ATSC) {
		result = sony_cxd6801_demod_atsc_SoftReset(pDemod);
        SONY_CXD6801_TRACE_RETURN (result);
    }
#endif

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* SW Reset */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFE, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_SetConfig(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_config_id_t configId,
                                    int32_t value)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_demod_SetConfig");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP)) {
        /* This api is accepted in Sleep and Active states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    switch (configId) {
	case SONY_CXD6801_DEMOD_CONFIG_PARALLEL_SEL:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* 2bit parallel is not supported for CXD2856 family */
		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId) && (value == 2)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C4h     [7]    1'b0       OSERIALEN
         * <SLV-T>  02h     E4h     [0]    1'b0       OTS_PAR2SEL
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC4, (uint8_t)(value ? 0x00 : 0x80), 0x80);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

		if (SONY_CXD6801_DEMOD_CHIP_ID_2878_FAMILY(pDemod->chipId)) {
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xE4, (uint8_t)((value == 2) ? 0x01 : 0x00), 0x01);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_SER_DATA_ON_MSB:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C4h     [3]    1'b1       OSEREXCHGB7
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC4, (uint8_t)(value ? 0x08 : 0x00), 0x08);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_OUTPUT_SEL_MSB:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C4h     [4]    1'b0       OWFMT_LSB1STON
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC4, (uint8_t)(value ? 0x00 : 0x10), 0x10);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TSVALID_ACTIVE_HI:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C5h     [1]    1'b0       OWFMT_VALINV
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC5, (uint8_t)(value ? 0x00 : 0x02), 0x02);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TSSYNC_ACTIVE_HI:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C5h     [2]    1'b0       OWFMT_STINV
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC5, (uint8_t)(value ? 0x00 : 0x04), 0x04);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TSERR_ACTIVE_HI:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     CBh     [0]    1'b0       OWFMT_ERRINV
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xCB, (uint8_t)(value ? 0x00 : 0x01), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_LATCH_ON_POSEDGE:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  00h     C5h     [0]    1'b1       OWFMT_CKINV
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC5, (uint8_t)(value ? 0x01 : 0x00), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TSCLK_CONT:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* Store the serial clock mode */
        pDemod->serialTSClockModeContinuous = (uint8_t) (value ? 0x01 : 0x00);
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TSCLK_MASK:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  00h    C6h     [4:0]     5'b00000    OWFMT_CKDISABLE
         * <SLV-T>  60h    52h     [4:0]     5'b00000    OWFMT_CKDISABLE (For ISDB-T)
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC6, (uint8_t)value, 0x1F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x60, 0x52, (uint8_t)value, 0x1F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TSVALID_MASK:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  00h     C8h     [4:0]    5'b00011    OWFMT_VALDISABLE
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC8, (uint8_t)value, 0x1F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TSERR_MASK:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  00h     C9h     [4:0]    5'b00000    OWFMT_ERRDISABLE
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xC9, (uint8_t)value, 0x1F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_PARALLEL_TSCLK_MANUAL:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0xFF)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        pDemod->parallelTSClkManualSetting = (uint8_t) value;

        break;

	case SONY_CXD6801_DEMOD_CONFIG_TS_PACKET_GAP:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 7)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* Slave    Bank    Addr    Bit      Default    Value    Name
         * -------------------------------------------------------------------
         * <SLV-T>  00h     D6h    [2:0]      3'd4       3'dx    OTSRC_DIVCKREV
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xD6, (uint8_t)value, 0x07);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        break;

	case SONY_CXD6801_DEMOD_CONFIG_TS_BACKWARDS_COMPATIBLE:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        pDemod->isTSBackwardsCompatibleMode = (uint8_t) (value ? 1 : 0);

        break;

	case SONY_CXD6801_DEMOD_CONFIG_PWM_VALUE:
        /* Perform range checking on value */
        if ((value < 0) || (value > 0x1000)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* Slave    Bank    Addr    Bit      Default    Value    Name
         * -------------------------------------------------------------------
         * <SLV-T>  00h     B7h     [0]      1'b0       1'b1     OREG_RFAGCSEL
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xB7, (uint8_t)(value ? 0x01 : 0x00), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* slave    Bank    Addr    Bit    default    Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>  00h     B2h     [4]      1'b0       1'bx      OREG_GDA_ALLONE_RFAGC
         * <SLV-T>  00h     B2h     [3:0]    8'h00      8'h0x     OREG_GDA_VAL_RFAGC[11:8]
         * <SLV-T>  00h     B3h     [7:0]    8'h00      8'hxx     OREG_GDA_VAL_RFAGC[7:0]
         */
        {
            uint8_t data[2];
            data[0] = (uint8_t) (((uint16_t)value >> 8) & 0x1F);
            data[1] = (uint8_t) ((uint16_t)value & 0xFF);

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xB2, data[0], 0x1F);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0xB3, data[1], 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TSCLK_CURRENT:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * --------------------------------------------------------
         * <SLV-T>  00h     95h     [1:0]  2'b11      OREG_TSCLK_C
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0x95, (uint8_t)(value & 0x03), 0x03);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TS_CURRENT:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * --------------------------------------------------------
         * <SLV-T>  00h     95h     [3:2]  2'b11      OREG_TSSYNC_C
         * <SLV-T>  00h     95h     [5:4]  2'b11      OREG_TSVALID_C
         */
        {
            uint8_t data = ((value & 0x03) << 2) | ((value & 0x03) << 4);

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0x95, data, 0x3C);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }

        /* slave    Bank    Addr    Bit    default    Name
         * --------------------------------------------------------
         * <SLV-T>  00h     96h     [7:0]  8'hFF      OREG_TSDATA3_C[1:0],OREG_TSDATA2_C[1:0],OREG_TSDATA1_C[1:0],OREG_TSDATA0_C[1:0]
         * <SLV-T>  00h     97h     [7:0]  8'hFF      OREG_TSDATA7_C[1:0],OREG_TSDATA6_C[1:0],OREG_TSDATA5_C[1:0],OREG_TSDATA4_C[1:0]
         */
        {
            uint8_t data = (value & 0x03) | ((value & 0x03) << 2) | ((value & 0x03) << 4) | ((value & 0x03) << 6);

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0x96, data, 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0x97, data, 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_GPIO0_CURRENT:
        /* slave    Bank    Addr    Bit    default    Name
         * --------------------------------------------------------
         * <SLV-X>  00h     86h     [1:0]  2'b00      OREG_GPIO0_C
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, 0x86, (uint8_t)(value & 0x03), 0x03);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_GPIO1_CURRENT:
        /* slave    Bank    Addr    Bit    default    Name
         * --------------------------------------------------------
         * <SLV-X>  00h     86h     [3:2]  2'b00      OREG_GPIO1_C
         */
		if (sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, 0x86, (uint8_t)((value & 0x03) << 2), 0x0C) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_GPIO2_CURRENT:
        /* slave    Bank    Addr    Bit    default    Name
         * --------------------------------------------------------
         * <SLV-X>  00h     86h     [5:4]  2'b00      OREG_GPIO2_C
         */
		if (sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, 0x86, (uint8_t)((value & 0x03) << 4), 0x30) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_CHBOND:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

		if ((value < SONY_CXD6801_DEMOD_CHBOND_CONFIG_DISABLE) || (value > SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* Channel bonding is not supported for CXD2856 family */
		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* slave    Bank    Addr   Bit     Name
        * ----------------------------------------------------
        * <SLV-X>   00h     C0h    [1:0]   OREG_STREAMIN_SEL
        */
        switch (value) {
		case SONY_CXD6801_DEMOD_CHBOND_CONFIG_DISABLE:
		case SONY_CXD6801_DEMOD_CHBOND_CONFIG_ISDBC:
		case SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN:
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, 0xC0, (uint8_t)value, 0x03);
            break;
		case SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB:
            /* Stream input should be disabled */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, 0xC0, 0x00, 0x03);
            break;
        }
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

		pDemod->chbondConfig = (sony_cxd6801_demod_chbond_config_t)value;
        break;

	case SONY_CXD6801_DEMOD_CONFIG_CHBOND_STREAMIN:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 7)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* Channel bonding is not supported for CXD2856 family */
		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        pDemod->chbondStreamIn = (uint8_t)value;
        break;

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE

    /* ---- For terrstrial and cable ---- */

	case SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TS_SERIAL_CLK_FREQ:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 5)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* Store the clock frequency mode for terrestrial and cable systems */
		pDemod->serialTSClkFreqTerrCable = (sony_cxd6801_demod_serial_ts_clk_t)value;
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TS_2BIT_PARALLEL_CLK_FREQ:
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
            /* This api is accepted in Sleep state only */
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if ((value < 1) || (value > 2)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* 2bit parallel is not supported for CXD2856 family */
		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* Store the clock frequency mode for terrestrial and cable systems */
		pDemod->twoBitParallelTSClkFreqTerrCable = (sony_cxd6801_demod_2bit_parallel_ts_clk_t)value;
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TUNER_OPTIMIZE:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

		pDemod->tunerOptimize = (sony_cxd6801_demod_tuner_optimize_t)value;

        break;

	case SONY_CXD6801_DEMOD_CONFIG_IFAGCNEG:
        /* slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     CBh     [6]      8'h48      8'h08      OCTL_IFAGCNEG
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0xCB, (uint8_t)(value ? 0x40 : 0x00), 0x40);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        if (!pDemod->atscCoreDisable) {
            /* For ATSC */
            /* Register Type  Slave    Bank    Addr    Bit    Default    Value    Name
             * -----------------------------------------------------------------------------------------------
             * Demodulator   <SLV-R>   A3h     54h     [7]     8'h86     8'h06    vsb_DAGCA_IFPWMINV
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVR, 0xA3, 0x54, (uint8_t)(value ? 0x80 : 0x00), 0x80);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }

        break;

	case SONY_CXD6801_DEMOD_CONFIG_IFAGC_ADC_FS:
        {
            uint8_t data;

            if (value == 0) {
                data = 0x50; /* 1.4Vpp - Default */
            }
            else if (value == 1) {
                data = 0x39; /* 1.0Vpp */
            }
            else if (value == 2) {
                data = 0x28; /* 0.7Vpp */
            }
            else {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
            }

            /* Slave     Bank    Addr   Bit      Default    Name
             * -------------------------------------------------------------------
             * <SLV-T>   10h     CDh    [7:0]    8'h50      OCTL_IFAGC_TARGET[7:0]
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0xCD, data, 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }

        /* For ATSC */
        if (!pDemod->atscCoreDisable) {
            uint8_t data[2];

            if (value == 0) { /* 1.4Vpp - Default */
                data[0] = 0x58;
                data[1] = 0x17;
            }
            else if (value == 1) { /* 1.0Vpp */
                data[0] = 0xF6;
                data[1] = 0x07;
            }
            else if (value == 2) { /* 0.7Vpp */
                data[0] = 0xAC;
                data[1] = 0x07;
            }
            else {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
            }

            /* Register Type  Slave    Bank    Addr    Bit    Default    Value    Name
             * -----------------------------------------------------------------------------------------------
             * Demodulator   <SLV-R>   A3h     50h     [7:0]   8'h58     8'hxx    vsb_DAGCA_TARGETINIT[7:0]
             * Demodulator   <SLV-R>   A3h     52h     [6:4]   8'h17     8'hxx    vsb_DAGCA_TARGETINIT[10:8]
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVR, 0xA3, 0x50, data[0], 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVR, 0xA3, 0x52, data[1], 0x70);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }

        break;

	case SONY_CXD6801_DEMOD_CONFIG_SPECTRUM_INV:
        /* Store the configured sense. */
		pDemod->confSense = value ? SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_INV : SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_NORMAL;
        break;

	case SONY_CXD6801_DEMOD_CONFIG_RF_SPECTRUM_INV:
        /* Store the configured sense. */
		pDemod->rfSpectrumSense = value ? SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_INV : SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_NORMAL;
        break;

	case SONY_CXD6801_DEMOD_CONFIG_TERR_BLINDTUNE_DVBT2_FIRST:
        /* Store the blindTune / Scan system priority setting */
        pDemod->blindTuneDvbt2First = (uint8_t)(value ? 1 : 0);
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBT_BERN_PERIOD:
        /* Set the measurment period for Pre-RS BER (DVB-T). */
        /* Verify range of value */
        if ((value < 0) || (value > 31)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit    default          Name
         * -----------------------------------------------------------------------
         * <SLV-T>   10h     60h    [4:0]     5'h0B        OREG_BERN_PERIOD[4:0]
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0x60, (uint8_t)(value & 0x1F), 0x1F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBC_BERN_PERIOD:
        /* Set the measurment period for Pre-RS BER (DVB-C/ISDB-C/J.83B). */
        /* Verify range of value */
        if ((value < 0) || (value > 31)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit    default          Name
         * -----------------------------------------------------------------------
         * <SLV-T>   40h     60h    [4:0]     5'h0B        OREG_BERN_PERIOD[4:0]
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x40, 0x60, (uint8_t)(value & 0x1F), 0x1F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBT_VBER_PERIOD:
        /* Set the measurment period for Pre-Viterbi BER (DVB-T). */
        /* Verify range of value */
        if ((value < 0) || (value > 7)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * ----------------------------------------------------
        * <SLV-T>  10h     6Fh    [2:0]   OREG_VBER_PERIOD_SEL[2:0]
        */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0x6F, (uint8_t)(value & 0x07), 0x07);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBT2C2_BBER_MES:
        /* Set the measurment period for Pre-BCH BER (DVB-T2/C2) and Post-BCH FER (DVB-T2/C2). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * ---------------------------------------------
        * <SLV-T>  20h     72h    [3:0]   OREG_BBER_MES
        */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x20, 0x72, (uint8_t)(value & 0x0F), 0x0F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBT2C2_LBER_MES:
        /* Set the measurment period for Pre-LDPC BER (DVB-T2/C2). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * ---------------------------------------------
        * <SLV-T>  20h     6Fh    [3:0]   OREG_LBER_MES
        */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x20, 0x6F, (uint8_t)(value & 0x0F), 0x0F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBT_PER_MES:
        /* Set the measurment period for PER (DVB-T). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit              Name
         * ---------------------------------------------------------
         * <SLV-T>   10h     5Ch     [3:0]        OREG_PER_MES[3:0]
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0x5C, (uint8_t)(value & 0x0F), 0x0F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBC_PER_MES:
        /* Set the measurment period for PER (DVB-C/ISDB-C/J.83B). */
        /* Verify range of value */
        if ((value < 0) || (value > 31)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit              Name
         * ---------------------------------------------------------
         * <SLV-T>   40h     5Ch     [4:0]        OREG_PER_MES[3:0]
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x40, 0x5C, (uint8_t)(value & 0x1F), 0x1F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBT2C2_PER_MES:
        /* Set the measurment period for PER (DVB-T2/C2). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * -----------------------------------------------
        * <SLV-T>  24h     DCh    [3:0]   OREG_SP_PER_MES
        */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x24, 0xDC, (uint8_t)(value & 0x0F), 0x0F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ISDBT_BERPER_PERIOD:
        {
            uint8_t data[2];

            data[0] = (uint8_t)((value & 0x00007F00) >> 8);
            data[1] = (uint8_t)(value & 0x000000FF);

            /* slave     Bank    Addr    Bit     Name
             * -----------------------------------------------
             * <SLV-T>    60h    5Bh     [6:0]    OBER_CDUR_RSA[14:8]
             * <SLV-T>    60h    5Ch     [7:0]    OBER_CDUR_RSA[7:0]
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x60, 0x5B, data[0], 0x7F);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x60, 0x5C, data[1], 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC_RSERR_BKLEN:
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* Verify range of value */
        if ((value < 0) || (value > 0xFFFFFF)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        if (pDemod->atscCoreDisable) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* Register Type  Slave    Bank    Addr    Bit    Default    Value    Name
         * -----------------------------------------------------------------------------------------------
         * Demodulator   <SLV-R>   A2h     F2h     [7:0]   8'hBC     vsb_hi_rserra_bklen[7:0]
         * Demodulator   <SLV-R>   A2h     F3h     [7:0]   8'h64     vsb_hi_rserra_bklen[15:8]
         * Demodulator   <SLV-R>   A2h     F4h     [7:0]   8'h00     vsb_hi_rserra_bklen[23:16]
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVR, 0xA2, 0xF2, (uint8_t)(value & 0xFF), 0xFF);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVR, 0xA2, 0xF3, (uint8_t)((value >> 8) & 0xFF), 0xFF);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVR, 0xA2, 0xF4, (uint8_t)((value >> 16) & 0xFF), 0xFF);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_BBER_MES:
        {
            uint8_t data[2];

			if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
            }

            data[0] = (uint8_t)((value & 0xFF00) >> 8);
            data[1] = (uint8_t)(value & 0x00FF);

            /*  slave    Bank    Addr    Bit              Name
             * ------------------------------------------------------------
             *  <SLV-T>   94h     4ah     [7:4]        OREG_BBER_MES_0[3:0]
             *  <SLV-T>   94h     4ah     [3:0]        OREG_BBER_MES_1[3:0]
             *  <SLV-T>   94h     4bh     [7:4]        OREG_BBER_MES_2[3:0]
             *  <SLV-T>   94h     4bh     [3:0]        OREG_BBER_MES_3[3:0]
             */

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x94, 0x4A, data[0], 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x94, 0x4B, data[1], 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_LBER_MES:
        {
            uint8_t data[2];

			if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
            }

            data[0] = (uint8_t)((value & 0xFF00) >> 8);
            data[1] = (uint8_t)(value & 0x00FF);

            /*  slave    Bank    Addr    Bit              Name
             * ------------------------------------------------------------
             *  <SLV-T>   94h     30h     [7:4]        OREG_LBER_MES_0[3:0]
             *  <SLV-T>   94h     30h     [3:0]        OREG_LBER_MES_1[3:0]
             *  <SLV-T>   94h     31h     [7:4]        OREG_LBER_MES_2[3:0]
             *  <SLV-T>   94h     31h     [3:0]        OREG_LBER_MES_3[3:0]
             */

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x94, 0x30, data[0], 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x94, 0x31, data[1], 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBT_AVEBER_PERIOD_TYPE:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave     Bank    Addr    Bit     Name
         * -----------------------------------------------
         * <SLV-T>    10h    84h     [0]     OREG_BER_MODE_SEL
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0x84, (uint8_t)(value ? 0x01 : 0x00), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN(result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBT_AVEBER_PERIOD_TIME:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if (value <= 0) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        {
            /* 48000 / 65536 = 375 / 512 */
            uint32_t reg = ((375 * value) + 256) / 512; /* round */

            if (reg > 0x7FF) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
            }

            /* slave     Bank    Addr    Bit     Name
             * -----------------------------------------------
             * <SLV-T>    10h    82h     [2:0]   OREG_BER_TIME_MASTER[10:8]
             * <SLV-T>    10h    83h     [7:0]   OREG_BER_TIME_MASTER[7:0]
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0x82, (uint8_t)((reg >> 8) & 0x07), 0x07);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x10, 0x83, (uint8_t)(reg & 0xFF), 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBT2_AVEBER_PERIOD_TYPE:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave     Bank    Addr    Bit     Name
         * -----------------------------------------------
         * <SLV-T>    20h    7Ch     [0]     OREG_BER_MODE_SEL
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x20, 0x7C, (uint8_t)(value ? 0x01 : 0x00), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN(result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_DVBT2_AVEBER_PERIOD_TIME:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if (value <= 0) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        {
            /* 48000 / 65536 = 375 / 512 */
            uint32_t reg = ((375 * value) + 256) / 512; /* round */

            if (reg > 0x7FF) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
            }

            /* slave     Bank    Addr    Bit     Name
             * -----------------------------------------------
             * <SLV-T>    20h    7Ah     [2:0]   OREG_BER_TIME_MASTER[10:8]
             * <SLV-T>    20h    7Bh     [7:0]   OREG_BER_TIME_MASTER[7:0]
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x20, 0x7A, (uint8_t)((reg >> 8) & 0x07), 0x07);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x20, 0x7B, (uint8_t)(reg & 0xFF), 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TYPE_0:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* slave     Bank    Addr    Bit     Name
         * -----------------------------------------------
         * <SLV-T>   91h     7ch     [0]     OREG_BER_MODE_SEL_0
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x7C, (uint8_t)(value ? 0x01 : 0x00), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN(result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_0:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        if (value <= 0) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        {
            /* 48000 / 65536 = 375 / 512 */
            uint32_t reg = ((375 * value) + 256) / 512; /* round */

            if (reg > 0x7FF) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
            }

            /* slave     Bank    Addr    Bit     Name
             * -----------------------------------------------
             * <SLV-T>   91h     7ah     [2:0]   OREG_BER_TIME_MASTER_0[10:8]
             * <SLV-T>   91h     7bh     [7:0]   OREG_BER_TIME_MASTER_0[7:0]
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x7A, (uint8_t)((reg >> 8) & 0x07), 0x07);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x7B, (uint8_t)(reg & 0xFF), 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TYPE_1:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* slave     Bank    Addr    Bit     Name
         * -----------------------------------------------
         * <SLV-T>   91h     80h     [0]     OREG_BER_MODE_SEL_1
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x80, (uint8_t)(value ? 0x01 : 0x00), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN(result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_1:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        if (value <= 0) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        {
            /* 48000 / 65536 = 375 / 512 */
            uint32_t reg = ((375 * value) + 256) / 512; /* round */

            if (reg > 0x7FF) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
            }

            /* slave     Bank    Addr    Bit     Name
             * -----------------------------------------------
             * <SLV-T>   91h     7eh     [2:0]   OREG_BER_TIME_MASTER_1[10:8]
             * <SLV-T>   91h     7fh     [7:0]   OREG_BER_TIME_MASTER_1[7:0]
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x7E, (uint8_t)((reg >> 8) & 0x07), 0x07);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x7F, (uint8_t)(reg & 0xFF), 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TYPE_2:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* slave     Bank    Addr    Bit     Name
         * -----------------------------------------------
         * <SLV-T>   91h     84h     [0]     OREG_BER_MODE_SEL_2
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x84, (uint8_t)(value ? 0x01 : 0x00), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN(result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_2:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        if (value <= 0) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        {
            /* 48000 / 65536 = 375 / 512 */
            uint32_t reg = ((375 * value) + 256) / 512; /* round */

            if (reg > 0x7FF) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
            }

            /* slave     Bank    Addr    Bit     Name
             * -----------------------------------------------
             * <SLV-T>   91h     82h     [2:0]   OREG_BER_TIME_MASTER_2[10:8]
             * <SLV-T>   91h     83h     [7:0]   OREG_BER_TIME_MASTER_2[7:0]
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x82, (uint8_t)((reg >> 8) & 0x07), 0x07);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x83, (uint8_t)(reg & 0xFF), 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TYPE_3:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* slave     Bank    Addr    Bit     Name
         * -----------------------------------------------
         * <SLV-T>   91h     88h     [0]     OREG_BER_MODE_SEL_3
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x88, (uint8_t)(value ? 0x01 : 0x00), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN(result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_3:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        if (value <= 0) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        {
            /* 48000 / 65536 = 375 / 512 */
            uint32_t reg = ((375 * value) + 256) / 512; /* round */

            if (reg > 0x7FF) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
            }

            /* slave     Bank    Addr    Bit     Name
             * -----------------------------------------------
             * <SLV-T>   91h     86h     [2:0]   OREG_BER_TIME_MASTER_3[10:8]
             * <SLV-T>   91h     87h     [7:0]   OREG_BER_TIME_MASTER_3[7:0]
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x86, (uint8_t)((reg >> 8) & 0x07), 0x07);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x91, 0x87, (uint8_t)(reg & 0xFF), 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN(result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_AVESNR_PERIOD_TIME:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if (value <= 0) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        {
            /* 48000 / 65536 = 375 / 512 */
            uint32_t reg = ((375 * value) + 256) / 512; /* round */

            if (reg > 0x7FF) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
            }

			if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
                /* For CXD2856 family
                 *
                 * slave     Bank    Addr    Bit     Name
                 * -----------------------------------------------
                 * <SLV-T>    02h    48h     [2:0]   OREG_SNR_TIME_MASTER[10:8]
                 * <SLV-T>    02h    49h     [7:0]   OREG_SNR_TIME_MASTER[7:0]
                 */
				result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0x48, (uint8_t)((reg >> 8) & 0x07), 0x07);
				if (result != SONY_CXD6801_RESULT_OK) {
                    SONY_CXD6801_TRACE_RETURN(result);
                }

				result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0x49, (uint8_t)(reg & 0xFF), 0xFF);
				if (result != SONY_CXD6801_RESULT_OK) {
                    SONY_CXD6801_TRACE_RETURN(result);
                }
			}
			else if (SONY_CXD6801_DEMOD_CHIP_ID_2878_FAMILY(pDemod->chipId)) {
                /* For CXD2878 family
                 *
                 * slave     Bank    Addr    Bit     Name
                 * -----------------------------------------------
                 * <SLV-T>    02h    50h     [2:0]   OREG_SNR_TIME_MASTER[10:8]
                 * <SLV-T>    02h    51h     [7:0]   OREG_SNR_TIME_MASTER[7:0]
                 */
				result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0x50, (uint8_t)((reg >> 8) & 0x07), 0x07);
				if (result != SONY_CXD6801_RESULT_OK) {
                    SONY_CXD6801_TRACE_RETURN(result);
                }

                result = sony_cxd6801_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x02, 0x51, (uint8_t) (reg & 0xFF), 0xFF);
				if (result != SONY_CXD6801_RESULT_OK) {
                    SONY_CXD6801_TRACE_RETURN(result);
                }
            } else {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_OTHER);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_GPIO_EWS_FLAG:
        {
            uint8_t data;

            if (value == 0) {
                data = 0x01; /* EWS (on TMCC)- Default */
            }
            else if (value == 1) {
                data = 0x80; /* AC EEW */
            }
            else if (value == 2) {
                data = 0x81; /* EWS or AC EEW */
            }
            else {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
            }

            /* Slave     Bank    Addr   Bit  Default    Name
             * -------------------------------------------------------------------
             * <SLV-T>  63h     3Bh    [7:0]  8'h01    8'h80   OPIR_COR_INTEN_B[13:6]
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x63, 0x3B, data, 0xFF);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ISDBC_TSMF_HEADER_NULL:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* Slave     Bank    Addr   Bit       Default    Name
         * -------------------------------------------------------------------
         * <SLV-T>   49h     36h    [4:1]     8'h13      OREG_TSMF_MODE[1:0],OREG_TSMF_HEADER_PID_CHG_DISABLE,OREG_TSMF_VALID_TSMF_CTRL
         *
         * OREG_TSMF_HEADER_PID_CHG_DISABLE (Bit[2]) : 0:Enable, 1:Disable
         * OREG_TSMF_VALID_TSMF_CTRL        (Bit[1]) : 1:Enable, 0:Disable
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x49, 0x36, (uint8_t)(value ? 0x00 : 0x04), 0x04);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ISDBC_NULL_REPLACED_TS_TSVALID_LOW:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* Slave     Bank    Addr   Bit       Default    Name
         * -------------------------------------------------------------------
         * <SLV-T>   49h     36h    [4:1]     8'h13      OREG_TSMF_MODE[1:0],OREG_TSMF_HEADER_PID_CHG_DISABLE,OREG_TSMF_VALID_TSMF_CTRL
         *
         * OREG_TSMF_HEADER_PID_CHG_DISABLE (Bit[2]) : 0:Enable, 1:Disable
         * OREG_TSMF_VALID_TSMF_CTRL        (Bit[1]) : 1:Enable, 0:Disable
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x49, 0x36, (uint8_t)(value ? 0x02 : 0x00), 0x02);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_OUTPUT_DVBC2:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* Verify range of value */
		if ((value < SONY_CXD6801_DEMOD_OUTPUT_DVBC2_TS) || (value > SONY_CXD6801_DEMOD_OUTPUT_DVBC2_TLV)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

		pDemod->dvbc2Output = (sony_cxd6801_demod_output_dvbc2_t)value;

        break;

	case SONY_CXD6801_DEMOD_CONFIG_OUTPUT_ATSC3:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* Verify range of value */
		if ((value < SONY_CXD6801_DEMOD_OUTPUT_ATSC3_ALP) || (value > SONY_CXD6801_DEMOD_OUTPUT_ATSC3_BBP)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

		pDemod->atsc3Output = (sony_cxd6801_demod_output_atsc3_t)value;

        break;

	case SONY_CXD6801_DEMOD_CONFIG_OUTPUT_ISDBC_CHBOND:
        /* This flag can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* CXD2856 does not support channel bonding */
		if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* Verify range of value */
		if ((value < SONY_CXD6801_DEMOD_OUTPUT_ISDBC_CHBOND_TS_TLV_AUTO) || (value > SONY_CXD6801_DEMOD_OUTPUT_ISDBC_CHBOND_TLV)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        {
            /* slave    Bank    Addr    Bit    default     Value          Name                                  meaning
             * -----------------------------------------------------------------------------------------------------------------------------------
             * <SLV-T>   03h     B0h    [0]     1'b1       1'bx      OREG_XCTLVSEL_ON     0: manual mode ,  1: auto mode
             * <SLV-T>   03h     16h    [0]     1'b1       1'bx      OREG_VAR_TLV_EN      0: TS or Fixed length TLV,  1: Variable length TLV
             */
            uint8_t data[2];

			if (value == SONY_CXD6801_DEMOD_OUTPUT_ISDBC_CHBOND_TS_TLV_AUTO) {
                data[0] = 0x01;
                data[1] = 0x01;
			}
			else if (value == SONY_CXD6801_DEMOD_OUTPUT_ISDBC_CHBOND_TS) {
                data[0] = 0x01;
                data[1] = 0x00;
			}
			else if (value == SONY_CXD6801_DEMOD_OUTPUT_ISDBC_CHBOND_TLV) {
                data[0] = 0x00;
                data[1] = 0x01;
            } else {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
            }

			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x03, 0xB0, (uint8_t)data[0], 0x01);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }

            result = sony_cxd6801_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x03, 0x16, (uint8_t) data[1], 0x01);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }

		pDemod->isdbcChBondOutput = (sony_cxd6801_demod_output_isdbc_chbond_t)value;

        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_AUTO_SPECTRUM_INV:
        pDemod->atsc3AutoSpectrumInv = value ? 1 : 0;
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_CW_DETECTION:
        pDemod->atsc3CWDetection = value ? 1 : 0;
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_GPIO_EAS_PN_STATE:
        /* CXD2878 family only */
		if (!SONY_CXD6801_DEMOD_CHIP_ID_2878_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * -----------------------------------------------------------------
         * <SLV-T>  99h     AEh     [3:0]  4'b1110    OREG_EAS_PTN[3:0]
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x99, 0xAE, (uint8_t)(value & 0x0F), 0x0F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

	case SONY_CXD6801_DEMOD_CONFIG_ATSC3_GPIO_EAS_PN_TRANS:
        /* CXD2878 family only */
		if (!SONY_CXD6801_DEMOD_CHIP_ID_2878_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * -----------------------------------------------------------------
         * <SLV-T>  99h     B0h     [1:0]  2'b10      OREG_EAS_LATCH_TRANS_EN[1:0]
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x99, 0xB0, (uint8_t)(value & 0x03), 0x03);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC_UNLOCK_DETECTION:
        pDemod->atscUnlockDetection = value ? 1 : 0;
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC_AUTO_SIGNAL_CHECK_OFF:
        pDemod->atscAutoSignalCheckOff = value ? 1 : 0;
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC_NO_SIGNAL_THRESH:
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* Verify range of value */
        if ((value < 0) || (value > 0xFFFFFF)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        pDemod->atscNoSignalThresh = value;
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ATSC_SIGNAL_THRESH:
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* Verify range of value */
        if ((value < 0) || (value > 0xFFFFFF)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        pDemod->atscSignalThresh = value;
        break;

#endif /* SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE */

#ifdef SONY_CXD6801_DEMOD_SUPPORT_SAT

    /* ---- For satellite ---- */

    case SONY_DEMOD_CONFIG_SAT_TS_SERIAL_CLK_FREQ:
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            /* This api is accepted in Sleep state only */
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 5)) {
            SONY_CXD6801_TRACE_RETURN(SONY_RESULT_ERROR_RANGE);
        }

        /* Store the clock frequency mode for satellite systems */
        pDemod->serialTSClkFreqSat = (sony_demod_serial_ts_clk_t) value;
        break;

    case SONY_DEMOD_CONFIG_SAT_TS_2BIT_PARALLEL_CLK_FREQ:
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            /* This api is accepted in Sleep state only */
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 1) || (value > 2)) {
            SONY_CXD6801_TRACE_RETURN(SONY_RESULT_ERROR_RANGE);
        }

        /* 2bit parallel is not supported for CXD2856 family */
        if (SONY_DEMOD_CHIP_ID_2856_FAMILY (pDemod->chipId)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        /* Store the clock frequency mode for satellite systems */
        pDemod->twoBitParallelTSClkFreqSat = (sony_demod_2bit_parallel_ts_clk_t) value;
        break;

    case SONY_DEMOD_CONFIG_SAT_TUNER_IQ_SENSE_INV:
        pDemod->satTunerIqSense = value ? SONY_DEMOD_SAT_IQ_SENSE_INV : SONY_DEMOD_SAT_IQ_SENSE_NORMAL;
        break;

    case SONY_DEMOD_CONFIG_SAT_SINGLECABLE_SPECTRUM_INV:
        pDemod->isSinglecableSpectrumInv = value ? 1 : 0;
        break;

    case SONY_DEMOD_CONFIG_SAT_IFAGCNEG:
        /* slave    Bank    Addr   Bit     Name
        * -----------------------------------------------
        * <SLV-T>   A0h     B9h    [0]     ORFAGC_AGCNEG
        */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA0, 0xB9, (uint8_t) (value? 0x01 : 0x00), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_DVBSS2_BER_PER_MES:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* Slave     Bank   Addr   Bit     Default    Name
         * ----------------------------------------------------------
         * <SLV-T>   A0h    BAh    [3:0]   4'd8       OFBER_MES
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA0, 0xBA, (uint8_t)value, 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_DVBS2_BER_FER_MES:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* Slave     Bank   Addr   Bit     Default    Name
         * ----------------------------------------------------------
         * <SLV-T>   A0h    BCh    [3:0]   4'd8       OF2BER_MES
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA0, 0xBC, (uint8_t)value, 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_DVBS_VBER_MES:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* Slave     Bank   Addr   Bit     Default    Name
         * ----------------------------------------------------------
         * <SLV-T>   A0h    BBh    [3:0]   4'd8       OFVBER_MES
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA0, 0xBB, (uint8_t)value, 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_DVBS2_LBER_MES:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* Slave     Bank   Addr   Bit     Default    Name
         * ----------------------------------------------------------
         * <SLV-T>   A0h    7Ah    [3:0]   4'd8       OFLBER_MES
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA0, 0x7A, (uint8_t)value, 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_DVBSS2_BLINDSCAN_POWER_SMOOTH:
        /* Verify range of value */
        if ((value < 0) || (value > 7)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }
        pDemod->dvbss2PowerSmooth = (uint8_t)value;
        break;

    case SONY_DEMOD_CONFIG_DVBSS2_BLINDSCAN_VERSION:
        /* Verify range of value */
        if ((value < SONY_DEMOD_DVBSS2_BLINDSCAN_VERSION1) || (value > SONY_DEMOD_DVBSS2_BLINDSCAN_VERSION2)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }
        pDemod->dvbss2BlindScanVersion = (sony_demod_dvbss2_blindscan_version_t)value;
        break;

    case SONY_DEMOD_CONFIG_ISDBS_BERNUMCONF:
        /* The measurement unit can be changed by the following register.
         *      Slave   Bank    Addr    Bit    Default     Signal name  Meaning
         *    ------------------------------------------------------------------------------------------------------
         *    <SLV-T>  C0h     A6h     [0]      1'b0       OBERNUMCONF  1'b0: Measurement unit is in Super Frames.
         *                                                              1'b1: Measurement unit is in Slots.
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xC0, 0xA6, (uint8_t)(value ? 0x01 : 0x00), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_ISDBS_BER_PERIOD1:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }
        /*      Slave   Bank    Addr    Bit    Default     Signal name
         *    ----------------------------------------------------------
         *   <SLV-T>  C0h     B1h     [3:0]    4'd1       OBERPERIOD1[3:0]
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xC0, 0xB1, (uint8_t)value, 0x0F);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_ISDBS_BER_PERIOD2:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }
        /*    Slave   Bank    Addr    Bit    Default     Signal name
         *   ----------------------------------------------------------
         *   <SLV-T>  C0h     B2h     [3:0]    4'd1       OBERPERIOD2[3:0]
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xC0, 0xB2, (uint8_t)value, 0x0F);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_ISDBS_BER_PERIODT:
        /* Verify range of value */
        if ((value < 0) || (value > 7)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }
        /*     Slave   Bank    Addr    Bit    Default     Signal name
         *    ----------------------------------------------------------
         *    <SLV-T>  C0h     B0h     [2:0]    3'd1       OBERPERIODT[2:0]
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xC0, 0xB0, (uint8_t)value, 0x07);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_ISDBS3_LBER_MES1:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }
        /*  slave     Bank    Addr    Bit     Default    Setting       Signal name           Comment
         * -----------------------------------------------------------------------------------------------------------
         *  <SLV-T>    D0h     D0h     [3:0]    8'h08      period       OFLBER_MES0_K2[3:0]  (for Transmission mode 1)
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xD0, 0xD0, (uint8_t)value, 0x0F);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_ISDBS3_LBER_MES2:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }
        /*  slave     Bank    Addr    Bit     Default    Setting       Signal name           Comment
         * -----------------------------------------------------------------------------------------------------------
         *  <SLV-T>    D0h     D1h     [3:0]    8'h08      period       OFLBER_MES1_K2[3:0]  (for Transmission mode 2)
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xD0, 0xD1, (uint8_t)value, 0x0F);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_ISDBS3_BER_FER_MES1:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }
        /*  slave     Bank    Addr    Bit     Default    Setting       Signal name           Comment
         * ----------------------------------------------------------------------------------------------------------
         *  <SLV-T>    D0h     B3h     [3:0]    8'h08      period       OF2BER_MES0_K2[3:0]  (for Transmission mode 1)
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xD0, 0xB3, (uint8_t)value, 0x0F);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_ISDBS3_BER_FER_MES2:
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }
        /*  slave     Bank    Addr    Bit     Default    Setting       Signal name           Comment
         * ----------------------------------------------------------------------------------------------------------
         *  <SLV-T>    D0h     B4h     [3:0]    8'h08      period       OF2BER_MES1_K2[3:0]  (for Transmission mode 2)
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xD0, 0xB4, (uint8_t)value, 0x0F);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_OUTPUT_ISDBS3:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* Verify range of value */
        if ((value < SONY_DEMOD_OUTPUT_ISDBS3_TLV) || (value > SONY_DEMOD_OUTPUT_ISDBS3_TLV_DIV_TS)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        if (SONY_DEMOD_CHIP_ID_2856_FAMILY (pDemod->chipId) && (value == SONY_DEMOD_OUTPUT_ISDBS3_TLV_DIV_TS)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        pDemod->isdbs3Output = (sony_demod_output_isdbs3_t) value;

        break;

    case SONY_DEMOD_CONFIG_ISDBS_LOWCN_HOLD:
        /* CXD2878 family only */
        if (!SONY_DEMOD_CHIP_ID_2878_FAMILY (pDemod->chipId)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        /*  slave     Bank    Addr    Bit     Default    Setting     Signal name
         * -----------------------------------------------------------------------------
         *  <SLV-T>   A1h     54h    [4:0]    8'h00      8'h11       OCPM_QUICK_CNTH_HOLD
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x54, (uint8_t)(value ? 0x11 : 0x00), 0x11);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_ISDBS_LOWCN_THRESH_H:
        /* CXD2878 family only */
        if (!SONY_DEMOD_CHIP_ID_2878_FAMILY (pDemod->chipId)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        /*  slave     Bank    Addr    Bit     Default    Setting     Signal name
         * -----------------------------------------------------------------------------
         *  <SLV-T>   A1h     50h    [4:0]    8'h00      8'hxx       OCPM_QUICK_CNDT_H[12:8]
         *  <SLV-T>   A1h     51h    [7:0]    8'hFA      8'hxx       OCPM_QUICK_CNDT_H[7:0]
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x50, (uint8_t)((value >> 8) & 0x1F), 0x1F);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x51, (uint8_t)(value & 0xFF), 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        break;

    case SONY_DEMOD_CONFIG_ISDBS_LOWCN_THRESH_L:
        /* CXD2878 family only */
        if (!SONY_DEMOD_CHIP_ID_2878_FAMILY (pDemod->chipId)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        /*  slave     Bank    Addr    Bit     Default    Setting     Signal name
         * -----------------------------------------------------------------------------
         *  <SLV-T>   A1h     52h    [4:0]    8'h01      8'hxx       OCPM_QUICK_CNDT_L[12:8]
         *  <SLV-T>   A1h     53h    [7:0]    8'h34      8'hxx       OCPM_QUICK_CNDT_L[7:0]
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x52, (uint8_t)((value >> 8) & 0x1F), 0x1F);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x53, (uint8_t)(value & 0xFF), 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        break;

    case SONY_DEMOD_CONFIG_ISDBS3_LOWCN_HOLD:
        /* CXD2878 family only */
        if (!SONY_DEMOD_CHIP_ID_2878_FAMILY (pDemod->chipId)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        /*  slave     Bank    Addr    Bit     Default    Setting     Signal name
         * -----------------------------------------------------------------------------
         *  <SLV-T>   A1h     5Bh    [4:0]    8'h00      8'h11       OCPM_FINE_CNTH_HOLD
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x5B, (uint8_t)(value ? 0x11 : 0x00), 0x11);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_ISDBS3_LOWCN_THRESH_H:
        /* CXD2878 family only */
        if (!SONY_DEMOD_CHIP_ID_2878_FAMILY (pDemod->chipId)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        /*  slave     Bank    Addr    Bit     Default    Setting     Signal name
         * -----------------------------------------------------------------------------
         *  <SLV-T>   A1h     55h    [0]      8'h00      8'hxx       OCPM_FINE_CNDT_H[16]
         *  <SLV-T>   A1h     56h    [7:0]    8'h08      8'hxx       OCPM_FINE_CNDT_H[15:8]
         *  <SLV-T>   A1h     57h    [7:0]    8'h6D      8'hxx       OCPM_FINE_CNDT_H[7:0]
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x55, (uint8_t)((value >> 16) & 0x01), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x56, (uint8_t)((value >> 8) & 0xFF), 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x57, (uint8_t)(value & 0xFF), 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_ISDBS3_LOWCN_THRESH_L:
        /* CXD2878 family only */
        if (!SONY_DEMOD_CHIP_ID_2878_FAMILY (pDemod->chipId)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        /*  slave     Bank    Addr    Bit     Default    Setting     Signal name
         * -----------------------------------------------------------------------------
         *  <SLV-T>   A1h     58h    [0]      8'h00      8'hxx       OCPM_FINE_CNDT_L[16]
         *  <SLV-T>   A1h     59h    [7:0]    8'h0A      8'hxx       OCPM_FINE_CNDT_L[15:8]
         *  <SLV-T>   A1h     5Ah    [7:0]    8'h89      8'hxx       OCPM_FINE_CNDT_L[7:0]
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x58, (uint8_t)((value >> 16) & 0x01), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x59, (uint8_t)((value >> 8) & 0xFF), 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0xA1, 0x5A, (uint8_t)(value & 0xFF), 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

#endif /* SONY_DEMOD_SUPPORT_SAT */

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TLV

    /* ---- For TLV output (ISDB-S3/DVB-C2) ---- */

    case SONY_DEMOD_CONFIG_TLV_PARALLEL_SEL:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  01h     C1h     [7]    1'b0       OTLV_SERIALEN
         * <SLV-T>  01h     CFh     [0]    1'b0       OTLV_PAR2SEL
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01, 0xC1, (uint8_t) (value ? 0x00 : 0x80), 0x80);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01, 0xCF, (uint8_t) ((value == 2) ? 0x01 : 0x00), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TLV_SER_DATA_ON_MSB:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  01h     C1h     [3]    1'b1       OTLV_SEREXCHGB7
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01, 0xC1, (uint8_t) (value ? 0x08 : 0x00), 0x08);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TLV_OUTPUT_SEL_MSB:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  01h     C1h     [4]    1'b0       OTLV_WFMT_LSB1STON
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01, 0xC1, (uint8_t) (value ? 0x00 : 0x10), 0x10);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TLVVALID_ACTIVE_HI:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  01h     C2h     [1]    1'b0       OTLV_WFMT_VALINV
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01, 0xC2, (uint8_t) (value ? 0x00 : 0x02), 0x02);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TLVSYNC_ACTIVE_HI:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  01h     C2h     [2]    1'b0       OTLV_WFMT_STINV
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01, 0xC2, (uint8_t) (value ? 0x00 : 0x04), 0x04);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TLVERR_ACTIVE_HI:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  01h     C8h     [0]    1'b0       OTLV_WFMT_ERRINV
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01, 0xC8, (uint8_t) (value ? 0x00 : 0x01), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TLV_LATCH_ON_POSEDGE:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  01h     C2h     [0]    1'b1       OTLV_WFMT_CKINV
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01, 0xC2, (uint8_t) (value ? 0x01 : 0x00), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TLVCLK_CONT:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* Store the serial clock mode */
        pDemod->serialTLVClockModeContinuous = (uint8_t) (value ? 0x01 : 0x00);
        break;

    case SONY_DEMOD_CONFIG_TLVCLK_MASK:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  01h     C3h     [4:0]    5'b00000    OTLV_WFMT_CKDISABLE
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01,  0xC3, (uint8_t) value, 0x1F);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TLVVALID_MASK:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  01h     C5h     [4:0]    5'b00011    OTLV_WFMT_VALDISABLE
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01, 0xC5, (uint8_t) value, 0x1F);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TLVERR_MASK:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  01h     C6h     [4:0]    5'b00000    OTLV_WFMT_ERRDISABLE
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01, 0xC6, (uint8_t) value, 0x1F);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TLVVALID_MASK_IN_ERRNULL:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  01h     C8h     [3]      1'b0        OTLV_WFMT_NULLVALDIS
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x01, 0xC8, (uint8_t) (value ? 0x08 : 0x00), 0x08);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TLV_NULL_FILTER:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if (SONY_DEMOD_CHIP_ID_2856_FAMILY (pDemod->chipId)) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  02h     EEh     [0]      1'b0        OTLV_NULLMSK_ON
         */
        result = sony_demod_SetAndSaveRegisterBits (pDemod, pDemod->i2cAddressSLVT, 0x02, 0xEE, (uint8_t) (value ? 0x01 : 0x00), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    case SONY_DEMOD_CONFIG_TERR_CABLE_TLV_SERIAL_CLK_FREQ:
        /* This register can change only in Sleep state */
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 5)) {
            SONY_CXD6801_TRACE_RETURN(SONY_RESULT_ERROR_RANGE);
        }

        /* Store the clock frequency mode for terrestrial and cable systems */
        pDemod->serialTLVClkFreqTerrCable = (sony_demod_serial_ts_clk_t) value;
        break;

    case SONY_DEMOD_CONFIG_SAT_TLV_SERIAL_CLK_FREQ:
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            /* This api is accepted in Sleep state only */
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 5)) {
            SONY_CXD6801_TRACE_RETURN(SONY_RESULT_ERROR_RANGE);
        }

        /* Store the clock frequency mode for satellite systems */
        pDemod->serialTLVClkFreqSat = (sony_demod_serial_ts_clk_t) value;
        break;

    case SONY_DEMOD_CONFIG_TERR_CABLE_TLV_2BIT_PARALLEL_CLK_FREQ:
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            /* This api is accepted in Sleep state only */
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 1) || (value > 2)) {
            SONY_CXD6801_TRACE_RETURN(SONY_RESULT_ERROR_RANGE);
        }

        /* Store the clock frequency mode for terrestrial and cable systems */
        pDemod->twoBitParallelTLVClkFreqTerrCable = (sony_demod_2bit_parallel_ts_clk_t) value;
        break;

    case SONY_DEMOD_CONFIG_SAT_TLV_2BIT_PARALLEL_CLK_FREQ:
        if (pDemod->state != SONY_DEMOD_STATE_SLEEP) {
            /* This api is accepted in Sleep state only */
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 1) || (value > 2)) {
            SONY_CXD6801_TRACE_RETURN(SONY_RESULT_ERROR_RANGE);
        }

        /* Store the clock frequency mode for satellite systems */
        pDemod->twoBitParallelTLVClkFreqSat = (sony_demod_2bit_parallel_ts_clk_t) value;
        break;

#endif /* SONY_CXD6801_DEMOD_SUPPORT_TLV */

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ALP

    /* ---- For ALP output (ATSC 3.0) ---- */

	case SONY_CXD6801_DEMOD_CONFIG_ALP_PARALLEL_SEL:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  02h     C1h     [7]    1'b0       OALP_SERIALEN
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xC1, (uint8_t)(value ? 0x00 : 0x80), 0x80);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALP_SER_DATA_ON_MSB:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  02h     C1h     [3]    1'b1       OALP_SEREXCHGB7
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xC1, (uint8_t)(value ? 0x08 : 0x00), 0x08);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALP_OUTPUT_SEL_MSB:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  02h     C1h     [4]    1'b0       OALP_WFMT_LSB1STON
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xC1, (uint8_t)(value ? 0x00 : 0x10), 0x10);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALP_SYNC_LENGTH:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  02h     C1h     [6]    1'b0       OALP_WFMT_SERSTDUR
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xC1, (uint8_t)(value ? 0x40 : 0x00), 0x40);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALPVALID_ACTIVE_HI:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  02h     C2h     [1]    1'b0       OALP_WFMT_VALINV
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xC2, (uint8_t)(value ? 0x00 : 0x02), 0x02);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALPSYNC_ACTIVE_HI:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  02h     C2h     [2]    1'b0       OALP_WFMT_STINV
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xC2, (uint8_t)(value ? 0x00 : 0x04), 0x04);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALPERR_ACTIVE_HI:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  02h     C8h     [0]    1'b0       OALP_WFMT_ERRINV
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xC8, (uint8_t)(value ? 0x00 : 0x01), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALP_LATCH_ON_POSEDGE:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  02h     C2h     [0]    1'b1       OALP_WFMT_CKINV
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xC2, (uint8_t)(value ? 0x01 : 0x00), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALPCLK_CONT:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* Store the serial clock mode */
        pDemod->serialALPClockModeContinuous = (uint8_t) (value ? 0x01 : 0x00);
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALPCLK_MASK:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  02h     C3h     [4:0]    5'b00000    OALP_WFMT_CKDISABLE
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xC3, (uint8_t)value, 0x1F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALPVALID_MASK:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  02h     C5h     [4:0]    5'b00011    OALP_WFMT_VALDISABLE
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xC5, (uint8_t)value, 0x1F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALPERR_MASK:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-T>  02h     C6h     [4:0]    5'b00000    OALP_WFMT_ERRDISABLE
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xC6, (uint8_t)value, 0x1F);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

	case SONY_CXD6801_DEMOD_CONFIG_ALP_VALIDCLK_IN_GAP:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  02h     EBh     [1]    1'b0       OALP_VLDDISCLK
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x02, 0xEB, (uint8_t)(value ? 0x02 : 0x00), 0x02);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

#endif /* SONY_DEMOD_SUPPORT_ALP */

	case SONY_CXD6801_DEMOD_CONFIG_BBP_LATCH_ON_POSEDGE:
        /* This register can change only in Sleep state */
		if (pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }

        /* CXD2878 family only */
		if (!SONY_CXD6801_DEMOD_CHIP_ID_2878_FAMILY(pDemod->chipId)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  9Dh     F2h     [0]    8'h01      OREG_CHBSP_OCLK_INV
         */
		result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x9D, 0xF2, (uint8_t)(value ? 0x01 : 0x00), 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        break;

    default:
        /* Unsupported ID */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
sony_cxd6801_result_t sony_demod_SetIFFreqConfig(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_iffreq_config_t * pIffreqConfig)
{
    SONY_CXD6801_TRACE_ENTER ("sony_demod_SetIFFreqConfig");

    if ((!pDemod) || (!pIffreqConfig)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    pDemod->iffreqConfig = *pIffreqConfig; /* Copy */

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}
#endif /* SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE */

sony_cxd6801_result_t sony_cxd6801_demod_TunerI2cEnable(sony_cxd6801_demod_t * pDemod, sony_cxd6801_demod_tuner_i2c_config_t tunerI2cConfig)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_demod_TunerI2cEnable");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, 0x1A,
		(uint8_t)((tunerI2cConfig == SONY_CXD6801_DEMOD_TUNER_I2C_CONFIG_DISABLE) ? 0x00 : 0x01), 0xFF);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    pDemod->tunerI2cConfig = tunerI2cConfig;

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_I2cRepeaterEnable(sony_cxd6801_demod_t * pDemod, uint8_t enable)
{
    SONY_CXD6801_TRACE_ENTER ("sony_demod_I2cRepeaterEnable");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pDemod->tunerI2cConfig == SONY_CXD6801_DEMOD_TUNER_I2C_CONFIG_REPEATER) {
        /* slave    Bank    Addr    Bit    default    Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-X>  ALL     08h    [0]      8'h00      8'h01     OREG_REPEN
         */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x08, (uint8_t)(enable ? 0x01 : 0x00)) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_GPIOSetConfig(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_gpio_pin_t pin,
                                        uint8_t enable,
										sony_cxd6801_demod_gpio_mode_t mode)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_demod_GPIOSetConfig");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) &&
		(pDemod->state != SONY_CXD6801_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in Sleep, Active and Shutdown states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pin > SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA7) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* TS Error/Output not available on GPIO0/GPIO1 */
	if (mode == SONY_CXD6801_DEMOD_GPIO_MODE_TS_OUTPUT) {
		if ((pin == SONY_CXD6801_DEMOD_GPIO_PIN_GPIO0) || (pin == SONY_CXD6801_DEMOD_GPIO_PIN_GPIO1)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }
    }

    switch (pin) {
	case SONY_CXD6801_DEMOD_GPIO_PIN_GPIO0:
	case SONY_CXD6801_DEMOD_GPIO_PIN_GPIO1:
	case SONY_CXD6801_DEMOD_GPIO_PIN_GPIO2:
        {
            uint8_t gpioModeSelAddr = 0;
            uint8_t gpioBitSel = 0;
            uint8_t enableHiZ = 0;

			gpioModeSelAddr = (uint8_t)(0xA3 + ((int)pin - (int)SONY_CXD6801_DEMOD_GPIO_PIN_GPIO0));
			gpioBitSel = (uint8_t)(1 << ((int)pin - (int)SONY_CXD6801_DEMOD_GPIO_PIN_GPIO0));

            /* Slave    Bank    Addr    Bit      Default   Name
             * -----------------------------------------------------------
             * <SLV-X>  00h     A3h     [3:0]    4'h00     OREG_GPIO0_SEL
             * <SLV-X>  00h     A4h     [3:0]    4'h00     OREG_GPIO1_SEL
             * <SLV-X>  00h     A5h     [3:0]    4'h00     OREG_GPIO2_SEL
             */
			if (sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, gpioModeSelAddr, (uint8_t)mode, 0x0F) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }

			if (mode == SONY_CXD6801_DEMOD_GPIO_MODE_INPUT) {
                /* HiZ enabled when pin is GPI */
                enableHiZ = 0x07;
            }
            else {
                /* HiZ determined by enable parameter */
                enableHiZ = enable ? 0x00 : 0x07;
            }

            /* Set HiZ setting for selected pin */
            /* Slave    Bank    Addr    Bit      Default    Name            Meaning
             * -----------------------------------------------------------------------------------
             * <SLV-X>  00h     82h     [2:0]    3'b111     OREG_GPIO_HIZ    0: HiZ Off, 1: HiZ On
             */
			if (sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, 0x82, enableHiZ, gpioBitSel) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }
        break;

	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA0:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA1:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA2:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA3:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA4:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA5:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA6:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA7:
        {
            uint8_t gpioModeSelAddr = 0;
            uint8_t gpioModeSelBitHigh = 0;
            uint8_t gpioBitSel = 0;
            uint8_t enableHiZ = 0;

			gpioModeSelAddr = (uint8_t)(0xB3 + ((int)pin - (int)SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA0) / 2);
			gpioModeSelBitHigh = ((int)pin - (int)SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA0) % 2;
			gpioBitSel = (uint8_t)(1 << ((int)pin - (int)SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA0));

            /* Slave    Bank    Addr    Bit      Default   Name
             * -----------------------------------------------------------
             * <SLV-X>  00h     B3h     [3:0]    4'h4      OREG_GPIOTD0_SEL
             * <SLV-X>  00h     B3h     [7:4]    4'h4      OREG_GPIOTD1_SEL
             * <SLV-X>  00h     B4h     [3:0]    4'h4      OREG_GPIOTD2_SEL
             * <SLV-X>  00h     B4h     [7:4]    4'h4      OREG_GPIOTD3_SEL
             * <SLV-X>  00h     B5h     [3:0]    4'h4      OREG_GPIOTD4_SEL
             * <SLV-X>  00h     B5h     [7:4]    4'h4      OREG_GPIOTD5_SEL
             * <SLV-X>  00h     B6h     [3:0]    4'h4      OREG_GPIOTD6_SEL
             * <SLV-X>  00h     B6h     [7:4]    4'h4      OREG_GPIOTD7_SEL
             */

            if (gpioModeSelBitHigh) {
				if (sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, gpioModeSelAddr, (uint8_t)((unsigned int)mode << 4), 0xF0) != SONY_CXD6801_RESULT_OK) {
					SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
                }
            } else {
				if (sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, gpioModeSelAddr, (uint8_t)mode, 0x0F) != SONY_CXD6801_RESULT_OK) {
					SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
                }
            }

			if (mode == SONY_CXD6801_DEMOD_GPIO_MODE_INPUT) {
                /* HiZ enabled when pin is GPI */
                enableHiZ = 0xFF;
			}
			else if (mode == SONY_CXD6801_DEMOD_GPIO_MODE_TS_OUTPUT) {
                /* For TS/TLV output, Hi-Z setting should be controlled automatically by Tune/Sleep functions.
                   Here, set Hi-Z. */
                enableHiZ = 0xFF;
            } else {
                /* HiZ determined by enable parameter */
                enableHiZ = enable ? 0x00 : 0xFF;
            }

            /* Set HiZ setting for selected pin */
            /* Slave    Bank    Addr    Bit      Default    Name            Meaning
             * -----------------------------------------------------------------------------------
             * <SLV-T>  00h     81h     [7:0]    8'hFF      OREG_TSDATA_HIZ  0: HiZ Off, 1: HiZ On
             */
			result = sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVT, 0x00, 0x81, enableHiZ, gpioBitSel);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }
        break;

    default:
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_GPIORead(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_gpio_pin_t pin,
                                   uint8_t * pValue)
{
    uint8_t rdata = 0x00;

    SONY_CXD6801_TRACE_ENTER ("sony_demod_GPIORead");

    if ((!pDemod) || (!pValue)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) &&
		(pDemod->state != SONY_CXD6801_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in Sleep, Active and Shutdown states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pin > SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA7) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  Slave    Bank    Addr    Bit      Name
     * ------------------------------------------------------------
     * <SLV-X>   00h     A0h     [2:0]    IREG_GPIO_IN
     * <SLV-X>   00h     B0h     [7:0]    IREG_GPIOTD_IN
     */
    switch (pin) {
	case SONY_CXD6801_DEMOD_GPIO_PIN_GPIO0:
	case SONY_CXD6801_DEMOD_GPIO_PIN_GPIO1:
	case SONY_CXD6801_DEMOD_GPIO_PIN_GPIO2:
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0xA0, &rdata, 1) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		*pValue = (rdata & (0x01 << ((int)pin - (int)SONY_CXD6801_DEMOD_GPIO_PIN_GPIO0))) ? 1 : 0;
        break;

	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA0:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA1:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA2:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA3:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA4:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA5:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA6:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA7:
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0xB0, &rdata, 1) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		*pValue = (rdata & (0x01 << ((int)pin - (int)SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA0))) ? 1 : 0;
        break;

    default:
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_GPIOWrite(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_gpio_pin_t pin,
                                    uint8_t value)
{
    SONY_CXD6801_TRACE_ENTER ("sony_demod_GPIOWrite");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) &&
		(pDemod->state != SONY_CXD6801_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in Sleep, Active and Shutdown states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /*  Slave    Bank    Addr    Bit      Default     Name
     * ------------------------------------------------------------
     * <SLV-X>   00h     A2h     [2:0]    3'b000      OREG_GPIO_OUT
     * <SLV-X>   00h     B2h     [7:0]    8'hFF       OREG_GPIOTD_OUT
     */
    switch (pin) {
	case SONY_CXD6801_DEMOD_GPIO_PIN_GPIO0:
	case SONY_CXD6801_DEMOD_GPIO_PIN_GPIO1:
	case SONY_CXD6801_DEMOD_GPIO_PIN_GPIO2:
		if (sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, 0xA2, (uint8_t)(value ? 0x07 : 0x00),
			(uint8_t)(0x01 << ((int)pin - (int)SONY_CXD6801_DEMOD_GPIO_PIN_GPIO0))) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        break;

	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA0:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA1:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA2:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA3:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA4:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA5:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA6:
	case SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA7:
		if (sony_cxd6801_demod_SetAndSaveRegisterBits(pDemod, pDemod->i2cAddressSLVX, 0x00, 0xB2, (uint8_t)(value ? 0xFF : 0x00),
			(uint8_t)(0x01 << ((int)pin - (int)SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA0))) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        break;

    default:
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_ChipID(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_chip_id_t * pChipId)
{
    uint8_t data[2];
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_demod_ChipID");

    if ((!pDemod) || (!pChipId)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00);

    /*  slave    Bank    Addr    Bit               NAME
     * -----------------------------------------------------------
     * <SLV-X>   00h     FBh     [1:0]            CHIP_ID[9:8]
     * <SLV-X>   00h     FDh     [7:0]            CHIP_ID[7:0]
     *
     * Note: CHIP_ID[9:8] is always 0 on CXD2856 family.
     *       Please check ::sony_demod_chip_id_t definition.
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0xFB, &data[0], 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0xFD, &data[1], 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	*pChipId = (sony_cxd6801_demod_chip_id_t)(((data[0] & 0x03) << 8) | data[1]);

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
sony_cxd6801_result_t sony_cxd6801_demod_terr_cable_monitor_InternalDigitalAGCOut(sony_cxd6801_demod_t * pDemod,
                                                                   uint16_t * pDigitalAGCOut)
{
    uint8_t data[2];
    SONY_CXD6801_TRACE_ENTER ("sony_demod_terr_cable_monitor_InternalDigitalAGCOut");

    if ((!pDemod) || (!pDigitalAGCOut)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* For terrestrial/cable systems (except for ATSC) */
	if (!SONY_CXD6801_DTV_SYSTEM_IS_TERR_CABLE(pDemod->system)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system == SONY_CXD6801_DTV_SYSTEM_ATSC) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x11 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank   Addr     Bit           NAME
     * -----------------------------------------------------------
     * <SLV-T>   11h     6Dh     [5:0]        ITDA_DAGC_GAIN[13:8]
     * <SLV-T>   11h     6Eh     [7:0]        ITDA_DAGC_GAIN[7:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6D, &data[0], 2) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    *pDigitalAGCOut = (uint16_t)(((uint32_t)(data[0] & 0x3F) << 8) | data[1]);

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}
#endif /* SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE */

sony_cxd6801_result_t sony_cxd6801_demod_SetAndSaveRegisterBits(sony_cxd6801_demod_t * pDemod,
                                                 uint8_t slaveAddress,
                                                 uint8_t bank,
                                                 uint8_t registerAddress,
                                                 uint8_t value,
                                                 uint8_t bitMask)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("sony_demod_SetAndSaveRegisterBits");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) &&
		(pDemod->state != SONY_CXD6801_DEMOD_STATE_SHUTDOWN)) {
        /* This api is accepted in Sleep, Active and Shutdown states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Write value depend on current state */
    result = setRegisterBitsForConfig (pDemod, slaveAddress, bank, registerAddress, value, bitMask);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Store the updated setting */
    result = setConfigMemory (pDemod, slaveAddress, bank, registerAddress, value, bitMask);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
sony_cxd6801_result_t sony_cxd6801_demod_terr_cable_SetScanMode(sony_cxd6801_demod_t * pDemod, sony_cxd6801_dtv_system_t system, uint8_t scanModeEnabled)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_demod_terr_cable_SetScanMode");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if ((system == SONY_CXD6801_DTV_SYSTEM_DVBC) || (system == SONY_CXD6801_DTV_SYSTEM_ISDBC) || (system == SONY_CXD6801_DTV_SYSTEM_J83B)) {
        /* Set SLV-T Bank : 0x40 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x40) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        {
            uint8_t data = scanModeEnabled ? 0x20 : 0x00;

            /* slave    Bank    Addr    Bit    default   Value          Name
             * ---------------------------------------------------------------------------------
             * <SLV-T>   40h     86h     [5]      1'b0       1'b1         OREG_AR_SCANNING
             */
			if (sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x86, data, 0x20) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }
    }

    pDemod->scanMode = scanModeEnabled;

    SONY_CXD6801_TRACE_RETURN (result);
}
#endif /* SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE */

/**
 @brief Register definition structure for TS clock configurations.
 */
typedef struct {
    uint8_t serialClkMode;      /**< Serial clock mode (gated or continuous) */
    uint8_t serialDutyMode;     /**< Serial clock duty mode (full rate or half rate) */
    uint8_t tsClkPeriod;        /**< TS clock period */
    uint8_t clkSelTSIf;         /**< TS clock frequency (low, mid or high) */
} sony_cxd6801_demod_ts_clk_configuration_t;

sony_cxd6801_result_t sony_cxd6801_demod_SetTSClockModeAndFreq(sony_cxd6801_demod_t * pDemod, sony_cxd6801_dtv_system_t system)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint8_t serialTS = 0;
    uint8_t twoBitParallel = 0;
    uint8_t backwardsCompatible = 0;
	sony_cxd6801_demod_ts_clk_configuration_t tsClkConfiguration;

	const sony_cxd6801_demod_ts_clk_configuration_t serialTSClkSettings[2][6] =
    {{ /* Gated Clock */
       /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD  OREG_CKSEL_TSTLVALPIF                      */
        {      3,          1,            8,             0        }, /* High Freq, full rate */
        {      3,          1,            8,             1        }, /* Mid Freq,  full rate */
        {      3,          1,            8,             2        }, /* Low Freq,  full rate */
        {      0,          2,            16,            0        }, /* High Freq, half rate */
        {      0,          2,            16,            1        }, /* Mid Freq,  half rate */
        {      0,          2,            16,            2        }  /* Low Freq,  half rate */
    },
    {  /* Continuous Clock */
       /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD  OREG_CKSEL_TSTLVALPIF                      */
        {      1,          1,            8,             0        }, /* High Freq, full rate */
        {      1,          1,            8,             1        }, /* Mid Freq,  full rate */
        {      1,          1,            8,             2        }, /* Low Freq,  full rate */
        {      2,          2,            16,            0        }, /* High Freq, half rate */
        {      2,          2,            16,            1        }, /* Mid Freq,  half rate */
        {      2,          2,            16,            2        }  /* Low Freq,  half rate */
    }};

	const sony_cxd6801_demod_ts_clk_configuration_t parallelTSClkSetting =
    {  /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD  OREG_CKSEL_TSTLVALPIF */
               0,          0,            8,             0
    };
    /* NOTE: For ISDB-S3, OREG_CKSEL_TSTLVALPIF should be 1 */

	const sony_cxd6801_demod_ts_clk_configuration_t backwardsCompatibleSerialTSClkSetting[2] =
    {  /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD  OREG_CKSEL_TSTLVALPIF                      */
        {      3,          1,            8,             1        }, /* Gated Clock          */
        {      1,          1,            8,             1        }  /* Continuous Clock     */
    };

	const sony_cxd6801_demod_ts_clk_configuration_t backwardsCompatibleParallelTSClkSetting =
    {  /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD  OREG_CKSEL_TSTLVALPIF */
               0,          0,            8,             1
    };

	const sony_cxd6801_demod_ts_clk_configuration_t twoBitParallelTSClkSetting[3] =
    {  /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD  OREG_CKSEL_TSTLVALPIF  */
        {       0,         0,            8,             0,       }, /* High Freq */
        {       0,         0,            8,             1,       }, /* Mid Freq  */
        {       0,         0,            8,             2,       }  /* Low Freq  */
    };

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_SetTSClockModeAndFreq");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ---------------------------------------------------
     * <SLV-T>  00h     C4h     [7]    1'b0       OSERIALEN
     */
    result = pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC4, &serialTS, 1);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

	if (!(serialTS & 0x80) && SONY_CXD6801_DEMOD_CHIP_ID_2878_FAMILY(pDemod->chipId)) {
        /* Set SLV-T Bank : 0x02 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-T>  02h     E4h     [0]    1'b0       OTS_PAR2SEL (0: TS 8bit-parallel(default) , 1:TS 2bit-parallel)
         */
        result = pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE4, &twoBitParallel, 1);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Set SLV-T Bank : 0x00 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

    /* Slave    Bank    Addr    Bit      Default    Name
     * -----------------------------------------------------------
     * <SLV-T>  00h     D3h     [0]      1'b0       OTSRATECTRLOFF
     * <SLV-T>  00h     DEh     [0]      1'b0       OTSIN_OFF
     *
     * (0, 0): Packet gap insertion On  (DVB-T/T2/C/C2/S/S2/J.83B)
     * (1, 0): Packet gap insertion Off (ISDB-T/C/S)
     * (1, 1): Packet gap insertion Off (for backwards compatibility (DVB-T/T2/C/C2/S/S2))
     * (1, 1): Packet gap insertion Off (ISDB-S3)
     * (0, 1): TLV / ALP Packet divided into 188bytes (ISDB-S3, ATSC 3.0)
     *
     * Slave    Bank    Addr    Bit      Default    Name
     * -----------------------------------------------------------
     * <SLV-T>  00h     DAh     [0]      1'b0       OTSRC_TSCKMANUALON
     *
     * 0 : Parallel TS clock manual setting off.
     * 1 : Parallel TS clock manual setting on.
     *     (Note: OTSRATECTRLOFF should be 0.)
     */
    {
        uint8_t tsRateCtrlOff = 0;
        uint8_t tsInOff = 0;
        uint8_t tsClkManaulOn = 0;

        switch (system) {
		case SONY_CXD6801_DTV_SYSTEM_DVBT:
		case SONY_CXD6801_DTV_SYSTEM_DVBT2:
		case SONY_CXD6801_DTV_SYSTEM_DVBC:
		case SONY_CXD6801_DTV_SYSTEM_DVBC2:
		case SONY_CXD6801_DTV_SYSTEM_DVBS:
		case SONY_CXD6801_DTV_SYSTEM_DVBS2:
            /* DVB systems */
            if (pDemod->isTSBackwardsCompatibleMode) {
                backwardsCompatible = 1;
                tsRateCtrlOff = 1;
                tsInOff = 1;
            } else {
                backwardsCompatible = 0;
                tsRateCtrlOff = 0;
                tsInOff = 0;
            }
            break;
		case SONY_CXD6801_DTV_SYSTEM_ISDBT:
		case SONY_CXD6801_DTV_SYSTEM_ISDBC:
		case SONY_CXD6801_DTV_SYSTEM_ISDBS:
		case SONY_CXD6801_DTV_SYSTEM_ATSC:
            /* For ISDB-T/C/S and ATSC, OTSRATECTRLOFF should be 1 */
            backwardsCompatible = 0;
            tsRateCtrlOff = 1;
            tsInOff = 0;
            break;
		case SONY_CXD6801_DTV_SYSTEM_ISDBS3:
            /* For ISDB-S3, OTSRATECTRLOFF, OTSIN_OFF should be 1 */
            backwardsCompatible = 0;
            tsRateCtrlOff = 1;
            tsInOff = 1;
            break;
		case SONY_CXD6801_DTV_SYSTEM_J83B:
            backwardsCompatible = 0;
            tsRateCtrlOff = 0;
            tsInOff = 0;
            break;
		case SONY_CXD6801_DTV_SYSTEM_ATSC3:
            /* ALP Packet divided into 188bytes */
            backwardsCompatible = 0;
            tsRateCtrlOff = 0;
            tsInOff = 1;
            break;
        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
        }

        if (!(serialTS & 0x80) && pDemod->parallelTSClkManualSetting) {
            tsClkManaulOn = 1;
            tsRateCtrlOff = 0;
        }


#ifdef SONY_CXD6801_DEMOD_SUPPORT_SAT
        if ((system == SONY_DTV_SYSTEM_ISDBS3) && (pDemod->isdbs3Output == SONY_DEMOD_OUTPUT_ISDBS3_TLV_DIV_TS)) {
            /* TLV Packet divided into 188bytes (Parallel TS clock manual setting is ignored) */
            tsClkManaulOn = 0;
            tsRateCtrlOff = 0;
            tsInOff = 1;
        }
#endif
#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
		if ((system == SONY_CXD6801_DTV_SYSTEM_ATSC3) && (pDemod->atsc3Output == SONY_CXD6801_DEMOD_OUTPUT_ATSC3_ALP_DIV_TS)) {
            /* ALP Packet divided into 188bytes (Parallel TS clock manual setting is ignored) */
            tsClkManaulOn = 1;
            tsRateCtrlOff = 0;
            tsInOff = 1;
        }
#endif

		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD3, tsRateCtrlOff, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xDE, tsInOff, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xDA, tsClkManaulOn, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    if (backwardsCompatible) {
        /* Backwards compatible mode */
        if (serialTS & 0x80) {
            /* Serial TS */
            tsClkConfiguration = backwardsCompatibleSerialTSClkSetting[pDemod->serialTSClockModeContinuous];
        }
        else {
            /* Parallel TS */
            tsClkConfiguration = backwardsCompatibleParallelTSClkSetting;

            tsClkConfiguration.tsClkPeriod = (uint8_t)(pDemod->parallelTSClkManualSetting ? pDemod->parallelTSClkManualSetting : 0x08);
        }
    }
    else if (serialTS & 0x80) {
        /* Serial TS */
#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
		if (SONY_CXD6801_DTV_SYSTEM_IS_TERR_CABLE(system)) {
            /* Terrestrial or Cable */
			if ((system == SONY_CXD6801_DTV_SYSTEM_ATSC3) && (pDemod->atsc3Output == SONY_CXD6801_DEMOD_OUTPUT_ATSC3_ALP_DIV_TS)) {
                /* ALP Packet divided into 188bytes */
                /* SONY_DEMOD_SERIAL_TS_CLK_MID_FULL is only supported */
				tsClkConfiguration = serialTSClkSettings[pDemod->serialTSClockModeContinuous][SONY_CXD6801_DEMOD_SERIAL_TS_CLK_MID_FULL];
			}
			else if (pDemod->serialTSClkFreqTerrCable == SONY_CXD6801_DEMOD_SERIAL_TS_CLK_HIGH_FULL) {
				if ((system == SONY_CXD6801_DTV_SYSTEM_ISDBC) && (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ISDBC)) {
                    /* 128MHz serial clock can be used for ISDB-C channel bonding only */
					tsClkConfiguration = serialTSClkSettings[pDemod->serialTSClockModeContinuous][SONY_CXD6801_DEMOD_SERIAL_TS_CLK_HIGH_FULL];
                } else {
					tsClkConfiguration = serialTSClkSettings[pDemod->serialTSClockModeContinuous][SONY_CXD6801_DEMOD_SERIAL_TS_CLK_MID_FULL];
                }
            } else {
                tsClkConfiguration = serialTSClkSettings[pDemod->serialTSClockModeContinuous][pDemod->serialTSClkFreqTerrCable];
            }
        } else
#endif
#ifdef SONY_CXD6801_DEMOD_SUPPORT_SAT
        if (SONY_DTV_SYSTEM_IS_SAT (system)) {
            /* Satellite */
            tsClkConfiguration = serialTSClkSettings[pDemod->serialTSClockModeContinuous][pDemod->serialTSClkFreqSat];
        } else
#endif
        {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
        }
    }
    else {
        /* Parallel TS */
        if (twoBitParallel & 0x1) {
            /* 2bit parallel */
#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
			if (SONY_CXD6801_DTV_SYSTEM_IS_TERR_CABLE(system)) {
                /* Terrestrial or Cable (DVB-C2, ISDB-C) */
                tsClkConfiguration = twoBitParallelTSClkSetting[pDemod->twoBitParallelTSClkFreqTerrCable];
            } else
#endif
#ifdef SONY_CXD6801_DEMOD_SUPPORT_SAT
            if (SONY_DTV_SYSTEM_IS_SAT (system)) {
                /* Satellite (ISDB-S3) */
                tsClkConfiguration = twoBitParallelTSClkSetting[pDemod->twoBitParallelTSClkFreqSat];
            } else
#endif
            {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
            }
        } else {
            /* Parallel */
            tsClkConfiguration = parallelTSClkSetting;

            tsClkConfiguration.tsClkPeriod = (uint8_t)(pDemod->parallelTSClkManualSetting ? pDemod->parallelTSClkManualSetting : 0x08);

#ifdef SONY_CXD6801_DEMOD_SUPPORT_SAT
            if (system == SONY_DTV_SYSTEM_ISDBS3) {
                /* NOTE: For ISDB-S3, OREG_CKSEL_TSTLVALPIF should be 1 */
                tsClkConfiguration.clkSelTSIf = 1;
            }
#endif
        }

#ifdef SONY_CXD6801_DEMOD_SUPPORT_SAT
        if ((system == SONY_DTV_SYSTEM_ISDBS3) && (pDemod->isdbs3Output == SONY_DEMOD_OUTPUT_ISDBS3_TLV_DIV_TS)) {
            /* TLV Packet divided into 188bytes (Parallel TS clock manual setting is ignored) */
            tsClkConfiguration.tsClkPeriod = 0x04; /* Fixed value */
        }
#endif
#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
		if ((system == SONY_CXD6801_DTV_SYSTEM_ATSC3) && (pDemod->atsc3Output == SONY_CXD6801_DEMOD_OUTPUT_ATSC3_ALP_DIV_TS)) {
            /* ALP Packet divided into 188bytes (Parallel TS clock manual setting is ignored) */
            tsClkConfiguration.clkSelTSIf = 1; /* Fixed value */
            tsClkConfiguration.tsClkPeriod = 0x08; /* Fixed value */
        }
#endif
    }

    if (serialTS & 0x80) {
        /* Serial TS, so set serial TS specific registers */

        /* slave    Bank    Addr    Bit    default    Name
         * -----------------------------------------------------
         * <SLV-T>  00h     C4h     [1:0]  2'b01      OSERCKMODE
         */
		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC4, tsClkConfiguration.serialClkMode, 0x03);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * -------------------------------------------------------
         * <SLV-T>  00h     D1h     [1:0]  2'b01      OSERDUTYMODE
         */
		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD1, tsClkConfiguration.serialDutyMode, 0x03);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ------------------------------------------------------
     * <SLV-T>  00h     D9h     [7:0]  8'h08      OTSCKPERIOD
     */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD9, tsClkConfiguration.tsClkPeriod);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Disable TS IF Clock */
    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     32h     [0]    1'b1       OREG_CK_TSTLVALPIF_EN
     */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x32, 0x00);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     33h     [1:0]  2'b01      OREG_CKSEL_TSTLVALPIF
     */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x33, tsClkConfiguration.clkSelTSIf);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Enable TS IF Clock */
    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     32h     [0]    1'b1       OREG_CK_TSTLVALPIF_EN
     */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x32, 0x01);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x01 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * -----------------------------------------------------
     * <SLV-T>  01h     E7h     [0]    1'b1       OCHG_BYPASS (0: TS serial, 2bit parallel in ISDB-S3, ISDB-C channel bonding, ATSC 3.0, 1: others(default))
     */
	if (((serialTS & 0x80) || (twoBitParallel & 0x01)) && ((system == SONY_CXD6801_DTV_SYSTEM_ISDBS3) || (system == SONY_CXD6801_DTV_SYSTEM_ATSC3)
		|| ((system == SONY_CXD6801_DTV_SYSTEM_ISDBC) && (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ISDBC)))) {
		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE7, 0x00, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else {
		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE7, 0x01, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    {
        /* Set parity period enable / disable based on backwards compatible TS configuration.
         * These registers are set regardless of broadcasting system for simplicity.
         */
        uint8_t data = backwardsCompatible ? 0x00 : 0x01;

        /* Enable parity period for DVB-T */
        /* Set SLV-T Bank : 0x10 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------------------
         * <SLV-T>  10h     66h     [0]    1'b1       OREG_TSIF_PCK_LENGTH
         */
		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x66, data, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Enable parity period for DVB-C (but affect to ISDB-C/J.83B) */
        /* Set SLV-T Bank : 0x40 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x40) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------------------
         * <SLV-T>  40h     66h     [0]    1'b1       OREG_TSIF_PCK_LENGTH
         */
		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x66, data, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

	if (SONY_CXD6801_DEMOD_CHIP_ID_2878_FAMILY(pDemod->chipId)) {
#ifdef SONY_CXD6801_DEMOD_SUPPORT_SAT
        /* Registers only for ISDB-S3 TS output setting */
        /*  slave   Bank    Addr    Bit    Default     Setting     Signal name
         *-------------------------------------------------------------------------
         * <SLV-T>  d0h     8bh     [0]      8'h01     8'h00      OREG_TSTLVSEL_ARK2
         */
        {
            uint8_t data = 0x01;
            if ((system == SONY_DTV_SYSTEM_ISDBS3) && (pDemod->isdbs3Output == SONY_DEMOD_OUTPUT_ISDBS3_TS)) {
                /* ISDB-S3 TS output */
                data = 0x00;
            }

            /* Set SLV-T Bank : 0xD0 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xD0) != SONY_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x8B, data) != SONY_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }

        /* Registers only for TLV packet divided into 188bytes setting */
        /* slave    Bank    Addr    Bit    default   value    Name
         * ---------------------------------------------------------------
         * <SLV-T>  02h     E3h     [0]     1'b0      1'b1    OTSRC_GAPMODE
         */
        {
            uint8_t data = 0x00;
            if ((system == SONY_DTV_SYSTEM_ISDBS3) && (pDemod->isdbs3Output == SONY_DEMOD_OUTPUT_ISDBS3_TLV_DIV_TS)) {
                /* TLV Packet divided into 188bytes */
                data = 0x01;
            }

            /* Set SLV-T Bank : 0x02 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != SONY_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE3, data) != SONY_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }
        }
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
        /* Registers only for ALP packet divided into 188bytes setting */
        /* slave    Bank    Addr    Bit    default   value    Name
         * ---------------------------------------------------------------
         * <SLV-T>  95h     11h     [7:0]    0x60     0x5C    OREG_CKG_RATE_NUMER[7:0]
         * <SLV-T>  02h     E7h     [0]      1'b0     1'b1    OWFMT_TIMER_OFF
         */

        {
            uint8_t data = 0x60;
            uint8_t data2 = 0x00;
			if ((system == SONY_CXD6801_DTV_SYSTEM_ATSC3) && (pDemod->atsc3Output == SONY_CXD6801_DEMOD_OUTPUT_ATSC3_ALP_DIV_TS)) {
                /* ALP Packet divided into 188bytes */
                data = 0x5C;
                data2 = 0x01;
            }

            /* Set SLV-T Bank : 0x95 */
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x95) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }

			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x11, data) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }

            /* Set SLV-T Bank : 0x02 */
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }

			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE7, data2) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }
#endif
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TLV
/**
 @brief Register definition structure for TLV clock configurations.
 */
typedef struct {
    uint8_t serialClkMode;      /**< Serial clock mode (gated or continuous) */
    uint8_t serialDutyMode;     /**< Serial clock duty mode (full rate or half rate) */
    uint8_t clkSelTLVIf;        /**< TLV clock frequency (low, mid or high) */
    uint8_t inputPeriod;        /**< Input period (low, mid or high) */
} sony_cxd6801_demod_tlv_clk_configuration_t;

sony_cxd6801_result_t sony_cxd6801_demod_SetTLVClockModeAndFreq(sony_cxd6801_demod_t * pDemod, sony_cxd6801_dtv_system_t system)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t serialTLV = 0;
    uint8_t twoBitParallel = 0;
    sony_demod_tlv_clk_configuration_t tlvClkConfiguration;

    const sony_demod_tlv_clk_configuration_t serialTLVClkSettings [2][6] =
    {{ /* Gated Clock */
       /* OTLV_SERCKMODE  OTLV_SERDUTYMODE  OREG_CKSEL_TSTLVALPIF  OREG_INPUT_PERIOD                       */
        {       3,               1,                 0,                  3       }, /* High Freq, full rate */
        {       3,               1,                 1,                  4       }, /* Mid Freq,  full rate */
        {       3,               1,                 2,                  5       }, /* Low Freq,  full rate */
        {       0,               2,                 0,                  3       }, /* High Freq, half rate */
        {       0,               2,                 1,                  4       }, /* Mid Freq,  half rate */
        {       0,               2,                 2,                  5       }  /* Low Freq,  half rate */
    },
    {  /* Continuous Clock */
       /* OTLV_SERCKMODE  OTLV_SERDUTYMODE  OREG_CKSEL_TSTLVALPIF  OREG_INPUT_PERIOD                       */
        {       1,               1,                 0,                  3       }, /* High Freq, full rate */
        {       1,               1,                 1,                  4       }, /* Mid Freq,  full rate */
        {       1,               1,                 2,                  5       }, /* Low Freq,  full rate */
        {       2,               2,                 0,                  3       }, /* High Freq, half rate */
        {       2,               2,                 1,                  4       }, /* Mid Freq,  half rate */
        {       2,               2,                 2,                  5       }  /* Low Freq,  half rate */
    }};

    const sony_demod_tlv_clk_configuration_t parallelTLVClkSetting =
    {  /* OTLV_SERCKMODE  OTLV_SERDUTYMODE  OREG_CKSEL_TSTLVALPIF  OREG_INPUT_PERIOD */
                0,               0,                 1,                  4          /* Mid Freq */
    };

    const sony_demod_tlv_clk_configuration_t twoBitParallelTLVClkSetting [3] =
    {  /* OTLV_SERCKMODE  OTLV_SERDUTYMODE  OREG_CKSEL_TSTLVALPIF  OREG_INPUT_PERIOD */
        {       0,               0,                 0,                  3       }, /* High Freq */
        {       0,               0,                 1,                  4       }, /* Mid Freq  */
        {       0,               0,                 2,                  5       }  /* Low Freq  */
    };

    SONY_CXD6801_TRACE_ENTER ("sony_demod_SetTLVClockModeAndFreq");

    if (!pDemod) {
        SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONY_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x01 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x01) != SONY_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ---------------------------------------------------
     * <SLV-T>  01h     C1h     [7]    1'b0       OTLV_SERIALEN (0: TLV parallel (default), 1: TLV serial)
     */
    result = pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC1, &serialTLV, 1);
    if (result != SONY_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ---------------------------------------------------
     * <SLV-T>  01h     CFh     [0]    1'b0       OTLV_PAR2SEL (0: TLV 8bit-parallel(default) , 1:TLV 2bit-parallel)
     */
    result = pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xCF, &twoBitParallel, 1);
    if (result != SONY_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * -----------------------------------------------------
     * <SLV-T>  01h     E7h     [0]    1'b1       OCHG_BYPASS (0: TLV serial, 2bit parallel in ISDB-S3, J.183,   1: others(default))
     */
    if (((system == SONY_DTV_SYSTEM_ISDBS3) || (system == SONY_DTV_SYSTEM_ISDBC)) && ((serialTLV & 0x80) || (twoBitParallel & 0x01))) {
        result = sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE7, 0x00, 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else {
        result = sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE7, 0x01, 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    if (serialTLV & 0x80) {
        /* Serial TLV */
        if (SONY_DTV_SYSTEM_IS_TERR_CABLE (system)) {
            /* Terrestrial or Cable (DVB-C2, ISDB-C) */
            tlvClkConfiguration = serialTLVClkSettings[pDemod->serialTLVClockModeContinuous][pDemod->serialTLVClkFreqTerrCable];
        }
        else if (SONY_DTV_SYSTEM_IS_SAT (system)) {
            /* Satellite (ISDB-S3) */
            tlvClkConfiguration = serialTLVClkSettings[pDemod->serialTLVClockModeContinuous][pDemod->serialTLVClkFreqSat];
        }
        else {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
        }
    }
    else {
        /* Parallel TLV */
        if (twoBitParallel & 0x1) {
            if (SONY_DTV_SYSTEM_IS_TERR_CABLE (system)) {
                /* Terrestrial or Cable (DVB-C2, ISDB-C) */
                tlvClkConfiguration = twoBitParallelTLVClkSetting[pDemod->twoBitParallelTLVClkFreqTerrCable];
            }
            else if (SONY_DTV_SYSTEM_IS_SAT (system)) {
                /* Satellite (ISDB-S3) */
                tlvClkConfiguration = twoBitParallelTLVClkSetting[pDemod->twoBitParallelTLVClkFreqSat];
            }
            else {
                SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
            }
        } else {
            tlvClkConfiguration = parallelTLVClkSetting;
        }
    }

    /* Set SLV-T Bank : 0x56 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x56) != SONY_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * -----------------------------------------------------
     * <SLV-T>  56h     83h     [2:0]  3'b100     OREG_INPUT_PERIOD
     */
    result = sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x83, tlvClkConfiguration.inputPeriod, 0x07);
    if (result != SONY_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    if (serialTLV & 0x80) {
        /* Serial TLV, so set serial TLV specific registers */

        /* Set SLV-T Bank : 0x01 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x01) != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * -----------------------------------------------------
         * <SLV-T>  01h     C1h     [1:0]  2'b01      OTLV_SERCKMODE
         */
        result = sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC1, tlvClkConfiguration.serialClkMode, 0x03);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * -------------------------------------------------------
         * <SLV-T>  01h     CCh     [1:0]  2'b01      OTLV_SERDUTYMODE
         */
        result = sony_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xCC, tlvClkConfiguration.serialDutyMode, 0x03);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Disable TLV IF Clock */
    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     32h     [0]    1'b1       OREG_CK_TSTLVALPIF_EN
     */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x32, 0x00);
    if (result != SONY_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     33h     [1:0]  2'b01      OREG_CKSEL_TSTLVALPIF
     */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x33, tlvClkConfiguration.clkSelTLVIf);
    if (result != SONY_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Enable TLV IF Clock */
    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     32h     [0]    1'b1       OREG_CK_TSTLVALPIF_EN
     */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x32, 0x01);
    if (result != SONY_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    if (SONY_DEMOD_CHIP_ID_2878_FAMILY (pDemod->chipId)) {
        /* Registers only for ISDB-S3 TS output setting */
        /*  slave   Bank    Addr    Bit    Default     Setting     Signal name
         *-------------------------------------------------------------------------
         * <SLV-T>  d0h     8bh     [0]      8'h01     8'h01      OREG_TSTLVSEL_ARK2
         */

        /* Set SLV-T Bank : 0xD0 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0xD0) != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x8B, 0x01) != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    SONY_CXD6801_TRACE_RETURN (result);
}
#endif /* SONY_CXD6801_DEMOD_SUPPORT_TLV */

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ALP
/**
 @brief Register definition structure for ALP clock configurations.
 */
typedef struct {
    uint8_t serialClkMode;      /**< Serial clock mode (gated or continuous) */
    uint8_t clkSelALPIf;        /**< ALP clock frequency (low, mid or high) */
} sony_cxd6801_demod_alp_clk_configuration_t;

sony_cxd6801_result_t sony_cxd6801_demod_SetALPClockModeAndFreq(sony_cxd6801_demod_t * pDemod, sony_cxd6801_dtv_system_t system)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint8_t serialALP = 0;
	sony_cxd6801_demod_alp_clk_configuration_t alpClkConfiguration;

	const sony_cxd6801_demod_alp_clk_configuration_t serialALPClkSettings[2] =
    {  /* Gated Clock */
       /* OALP_SERCKMODE  OREG_CKSEL_TSTLVALPIF                      */
        {       3,                1       }, /* Mid Freq, full rate */

       /* Continuous Clock */
       /* OALP_SERCKMODE  OREG_CKSEL_TSTLVALPIF                      */
        {       1,                1       }  /* Mid Freq, full rate */
    };

	const sony_cxd6801_demod_alp_clk_configuration_t parallelALPClkSetting =
    {  /* OALP_SERCKMODE  OREG_CKSEL_TSTLVALPIF */
                0,                1,         /* Mid Freq */
    };

    SONY_CXD6801_TRACE_ENTER ("sony_demod_SetALPClockModeAndFreq");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x02 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ---------------------------------------------------
     * <SLV-T>  02h     C1h     [7]    1'b0       OALP_SERIALEN (0: ALP parallel (default), 1: ALP serial)
     */
    result = pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC1, &serialALP, 1);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    if (serialALP & 0x80) {
        /* Serial ALP */
        alpClkConfiguration = serialALPClkSettings[pDemod->serialALPClockModeContinuous];
    }
    else {
        /* Parallel ALP */
        alpClkConfiguration = parallelALPClkSetting;
    }

    if (serialALP & 0x80) {
        /* Serial ALP, so set serial ALP specific registers */

        /* slave    Bank    Addr    Bit    default    Name
         * -----------------------------------------------------
         * <SLV-T>  02h     C1h     [1:0]  2'b01      OALP_SERCKMODE
         */
		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC1, alpClkConfiguration.serialClkMode, 0x03);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Disable ALP IF Clock */
    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     32h     [0]    1'b1       OREG_CK_TSTLVALPIF_EN
     */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x32, 0x00);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     33h     [1:0]  2'b01      OREG_CKSEL_TSTLVALPIF
     */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x33, alpClkConfiguration.clkSelALPIf);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Enable ALP IF Clock */
    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     32h     [0]    1'b1       OREG_CK_TSTLVALPIF_EN
     */
    result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x32, 0x01);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x01 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * -----------------------------------------------------
     * <SLV-T>  01h     E7h     [0]    1'b1       OCHG_BYPASS (0: ALP serial in ATSC 3.0, 1: others(default))
     */
	if ((system == SONY_CXD6801_DTV_SYSTEM_ATSC3) && (serialALP & 0x80)) {
		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE7, 0x00, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else {
		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE7, 0x01, 0x01);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    /* Registers only for TLV/ALP packet divided into 188bytes setting */
    /* slave    Bank    Addr    Bit    default   value    Name
     * ---------------------------------------------------------------
     * <SLV-T>  95h     11h     [7:0]    0x60     0x60    OREG_CKG_RATE_NUMER[7:0]
     */
    /* Set SLV-T Bank : 0x95 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x95) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x11, 0x60) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}
#endif /* SONY_CXD6801_DEMOD_SUPPORT_ALP */

sony_cxd6801_result_t sony_cxd6801_demod_SetTSDataPinHiZ(sony_cxd6801_demod_t * pDemod, uint8_t enable)
{
    uint8_t data = 0;
    uint8_t tsDataMask = 0;
    uint8_t tsDataMaskAll = 0;
    uint8_t isIsdbCTSTLVAutoOutput = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_demod_SetTSDataPinHiZ");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
	if ((pDemod->system == SONY_CXD6801_DTV_SYSTEM_ISDBC) && (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ISDBC)
		&& (pDemod->isdbcChBondOutput == SONY_CXD6801_DEMOD_OUTPUT_ISDBC_CHBOND_TS_TLV_AUTO)) {
        /* ISDB-C channel bonding TS/TLV auto is special case. TS or TLV will be output depend on input signal. */
        isIsdbCTSTLVAutoOutput = 1;
    }

	if ((pDemod->system == SONY_CXD6801_DTV_SYSTEM_ATSC3) && (pDemod->atsc3Output == SONY_CXD6801_DEMOD_OUTPUT_ATSC3_BBP)) {
        /* Special case for ATSC 3.0 BBP output. TSDATA0 is always used for output. */
        tsDataMaskAll = 0x01;
        goto register_write;
    }
#endif

	if ((pDemod->system == SONY_CXD6801_DTV_SYSTEM_ATSC3) && (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB)) {
        /* Special case for ATSC 3.0 channel bonding sub. TSDATA0 is always used for output. */
        tsDataMaskAll = 0x01;
        goto register_write;
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ---------------------------------------------------
     * <SLV-T>  00h     A9h     [1:0]  2'b0       OREG_TSTLVALPSEL
     *
     * <SLV-T>  00h     C4h     [7]    1'b0       OSERIALEN
     * <SLV-T>  00h     C4h     [3]    1'b1       OSEREXCHGB7
     *
     * <SLV-T>  01h     C1h     [7]    1'b0       OTLV_SERIALEN
     * <SLV-T>  01h     C1h     [3]    1'b1       OTLV_SEREXCHGB7
     * <SLV-T>  01h     CFh     [0]    1'b0       OTLV_PAR2SEL
     * <SLV-T>  01h     EAh     [6:4]  3'b1       OTLV_PAR2_B1SET
     * <SLV-T>  01h     EAh     [2:0]  3'b0       OTLV_PAR2_B0SET
     *
     * <SLV-T>  02h     C1h     [7]    1'b0       OALP_SERIALEN
     * <SLV-T>  02h     C1h     [3]    1'b1       OALP_SEREXCHGB7
     */

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, &data, 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TLV
    if (((data & 0x03) == 0x01) || isIsdbCTSTLVAutoOutput) {
        /* TLV output */
        /* Set SLV-T Bank : 0x01 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x01) != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC1, &data, 1) != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        switch (data & 0x88) {
        case 0x80:
            /* Serial TLV, output from TSDATA0 */
            tsDataMask = 0x01;
            break;
        case 0x88:
            /* Serial TLV, output from TSDATA7 */
            tsDataMask = 0x80;
            break;
        case 0x08:
        case 0x00:
        default:
            /* Parallel TLV */
            if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xCF, &data, 1) != SONY_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
            }

            if (data & 0x01) {
                /* TLV 2bit-parallel */
                if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xEA, &data, 1) != SONY_RESULT_OK) {
                    SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
                }

                tsDataMask = (0x01 << (data & 0x07)); /* LSB pin */
                tsDataMask |= (0x01 << ((data >> 4) & 0x07)); /* MSB pin */
            } else {
                /* TLV 8bit-parallel */
                tsDataMask = 0xFF;
            }
            break;
        }

        tsDataMaskAll |= tsDataMask;
    }
#endif /* SONY_DEMOD_SUPPORT_TLV */

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ALP
    if ((data & 0x03) == 0x02) {
        /* ALP output */
        /* Set SLV-T Bank : 0x02 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC1, &data, 1) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }

        switch (data & 0x88) {
        case 0x80:
            /* Serial ALP, output from TSDATA0 */
            tsDataMask = 0x01;
            break;
        case 0x88:
            /* Serial ALP, output from TSDATA7 */
            tsDataMask = 0x80;
            break;
        case 0x08:
        case 0x00:
        default:
            /* Parallel ALP */
            tsDataMask = 0xFF;
            break;
        }

        tsDataMaskAll |= tsDataMask;
    }
#endif /* SONY_DEMOD_SUPPORT_ALP */

    if (((data & 0x03) == 0x00) || isIsdbCTSTLVAutoOutput) {
        /* TS output */
        /* Set SLV-T Bank : 0x00 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC4, &data, 1) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        switch (data & 0x88) {
        case 0x80:
            /* Serial TS, output from TSDATA0 */
            tsDataMask = 0x01;
            break;
        case 0x88:
            /* Serial TS, output from TSDATA7 */
            tsDataMask = 0x80;
            break;
        case 0x08:
        case 0x00:
        default:
            /* Parallel TS */
			if (SONY_CXD6801_DEMOD_CHIP_ID_2878_FAMILY(pDemod->chipId)) {
                /* Set SLV-T Bank : 0x02 */
				if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != SONY_CXD6801_RESULT_OK) {
					SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
                }

				if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE4, &data, 1) != SONY_CXD6801_RESULT_OK) {
					SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
                }

                if (data & 0x01) {
                    /* TS 2bit-parallel */
					if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE5, &data, 1) != SONY_CXD6801_RESULT_OK) {
						SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
                    }

                    tsDataMask = (0x01 << (data & 0x07)); /* LSB pin */
                    tsDataMask |= (0x01 << ((data >> 4) & 0x07)); /* MSB pin */
                } else {
                    /* TS 8bit-parallel */
                    tsDataMask = 0xFF;
                }
			}
			else if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
                /* TS 8bit-parallel */
                tsDataMask = 0xFF;
            } else {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_OTHER);
            }
            break;
        }

        tsDataMaskAll |= tsDataMask;
    }

register_write:

    /* slave    Bank    Addr    Bit    default    Name
     * ---------------------------------------------------
     * <SLV-T>   00h    81h    [7:0]    8'hFF   OREG_TSDATA_HIZ
     */
    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x81, (uint8_t)(enable ? 0xFF : 0x00), tsDataMaskAll) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_SetStreamOutput(sony_cxd6801_demod_t * pDemod, uint8_t enable)
{
    uint8_t data = 0;
    uint8_t isIsdbCTSTLVAutoOutput = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_demod_SetStreamOutput");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
	if ((pDemod->system == SONY_CXD6801_DTV_SYSTEM_ISDBC) && (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ISDBC)
		&& (pDemod->isdbcChBondOutput == SONY_CXD6801_DEMOD_OUTPUT_ISDBC_CHBOND_TS_TLV_AUTO)) {
        /* ISDB-C channel bonding TS/TLV auto is special case. TS or TLV will be output depend on input signal. */
        isIsdbCTSTLVAutoOutput = 1;
    }
#endif

    /* slave    Bank    Addr    Bit    default    Name
     * ---------------------------------------------------
     * <SLV-T>  00h     A9h     [1:0]  2'b0       OREG_TSTLVALPSEL
     */

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, &data, 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TLV
    if (((data & 0x03) == 0x01) || isIsdbCTSTLVAutoOutput) {
        /* TLV output */
        /* Set SLV-T Bank : 0x01 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x01) != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        /* Enable TLV output */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC0, enable ? 0x00 : 0x01) != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ALP
    if ((data & 0x03) == 0x02) {
        /* ALP output */
        /* Set SLV-T Bank : 0x02 */
		if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Enable ALP output */
		if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC0, enable ? 0x00 : 0x01) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }
#endif

    if (((data & 0x03) == 0x00) || isIsdbCTSTLVAutoOutput) {
        /* TS output */
        /* Set SLV-T Bank : 0x00 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Enable TS output */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, enable ? 0x00 : 0x01) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t XtoSL(sony_cxd6801_demod_t * pDemod)
{
    SONY_CXD6801_TRACE_ENTER ("XtoSL");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (SONY_CXD6801_DEMOD_CHIP_ID_2878_FAMILY(pDemod->chipId)) {
        /* Clear all registers */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x02, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Wait for oscillator stabilization */
        SONY_CXD6801_SLEEP (4);

        /* Set SLV-X Bank : 0x00 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Initial setting for crystal oscillator */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x1D, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Clock mode setting */
        switch (pDemod->xtalFreq) {
		case SONY_CXD6801_DEMOD_XTAL_16000KHz:
		case SONY_CXD6801_DEMOD_XTAL_24000KHz:
		case SONY_CXD6801_DEMOD_XTAL_32000KHz:
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x14, (uint8_t)pDemod->xtalFreq) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
            break;

        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* Wait for oscillator stabilization */
        SONY_CXD6801_SLEEP (1);

        /* Initialize setting */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x50, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        if (pDemod->atscCoreDisable) {
            /* ATSC 1.0 core disable setting */
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x90, 0x00) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        SONY_CXD6801_SLEEP (1);

        /* Negate XSRST */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        if (pDemod->atscCoreDisable) {
            SONY_CXD6801_SLEEP (1);
        } else {
            SONY_CXD6801_SLEEP (21);
        }

		if (pDemod->chipId == SONY_CXD6801_DEMOD_CHIP_ID_CXD6802) {
            const uint8_t data[] = {0x00, 0x00, 0x00, 0x00};

            /* Set SLV-T Bank : 0x9C */
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9C) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }
	}
	else if (SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(pDemod->chipId)) {
        /* Set SLV-X Bank : 0x00 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Assert XSRST */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x01) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Initialize ADC setting */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x01) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Initialize ADC setting */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x28, 0x13) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Initialize demodulator mode */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x01) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Initial setting for crystal oscillator */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x1D, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Clock mode setting */
        switch (pDemod->xtalFreq) {
		case SONY_CXD6801_DEMOD_XTAL_16000KHz:
		case SONY_CXD6801_DEMOD_XTAL_24000KHz:
		case SONY_CXD6801_DEMOD_XTAL_32000KHz:
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x14, (uint8_t)pDemod->xtalFreq) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
            break;

        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* Clock mode setting */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x1C, 0x03) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Wait for oscillator stabilization */
        SONY_CXD6801_SLEEP (4);

        /* Initialize setting */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x50, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        SONY_CXD6801_SLEEP (1);

        /* Negate XSRST */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        SONY_CXD6801_SLEEP (1);
    } else {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_OTHER);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t SLtoSD(sony_cxd6801_demod_t * pDemod)
{
    SONY_CXD6801_TRACE_ENTER ("SLtoSD");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Assert XSRST */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Disable oscillator */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x1C, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t SDtoSL(sony_cxd6801_demod_t * pDemod)
{
    SONY_CXD6801_TRACE_ENTER ("SDtoSL");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Clock mode setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x1C, 0x03) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    SONY_CXD6801_SLEEP (4);

    /* Negate XSRST */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x10, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (pDemod->atscCoreDisable) {
        SONY_CXD6801_SLEEP (1);
    } else {
        SONY_CXD6801_SLEEP (21);
    }

	if (pDemod->chipId == SONY_CXD6801_DEMOD_CHIP_ID_CXD6802) {
        const uint8_t data[] = {0x00, 0x00, 0x00, 0x00};

        /* Set SLV-T Bank : 0x9C */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9C) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t loadConfigMemory(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint8_t i;

    SONY_CXD6801_TRACE_ENTER ("loadConfigMemory");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    for (i = 0; i < pDemod->configMemoryLastEntry; i++) {
        result = setRegisterBitsForConfig (pDemod,
                                           pDemod->configMemory[i].slaveAddress,
                                           pDemod->configMemory[i].bank,
                                           pDemod->configMemory[i].registerAddress,
                                           pDemod->configMemory[i].value,
                                           pDemod->configMemory[i].bitMask);

		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

static sony_cxd6801_result_t setConfigMemory(sony_cxd6801_demod_t * pDemod,
                                                 uint8_t slaveAddress,
                                                 uint8_t bank,
                                                 uint8_t registerAddress,
                                                 uint8_t value,
                                                 uint8_t bitMask)
{
    uint8_t i;
    uint8_t valueStored = 0;

    SONY_CXD6801_TRACE_ENTER ("setConfigMemory");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Search for matching address entry already in table */
    for (i = 0; i < pDemod->configMemoryLastEntry; i++){
        if ((valueStored == 0) &&
            (pDemod->configMemory[i].slaveAddress == slaveAddress) &&
            (pDemod->configMemory[i].bank == bank) &&
            (pDemod->configMemory[i].registerAddress == registerAddress)) {

            /* Clear bits to overwrite / set  and then store the new value */
            pDemod->configMemory[i].value &= ~bitMask;
            pDemod->configMemory[i].value |= (value & bitMask);

            /* Add new bits to the bit mask */
            pDemod->configMemory[i].bitMask |= bitMask;

            valueStored = 1;
        }
    }

    /* If current register does not exist in the table, add a new entry to the end */
    if (valueStored == 0) {
		if (pDemod->configMemoryLastEntry < SONY_CXD6801_DEMOD_MAX_CONFIG_MEMORY_COUNT) {
            pDemod->configMemory[pDemod->configMemoryLastEntry].slaveAddress = slaveAddress;
            pDemod->configMemory[pDemod->configMemoryLastEntry].bank = bank;
            pDemod->configMemory[pDemod->configMemoryLastEntry].registerAddress = registerAddress;
            pDemod->configMemory[pDemod->configMemoryLastEntry].value = (value & bitMask);
            pDemod->configMemory[pDemod->configMemoryLastEntry].bitMask = bitMask;
            pDemod->configMemoryLastEntry++;
        }
        else {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_OVERFLOW);
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t setRegisterBitsForConfig(sony_cxd6801_demod_t * pDemod,
                                               uint8_t slaveAddress,
                                               uint8_t bank,
                                               uint8_t registerAddress,
                                               uint8_t value,
                                               uint8_t bitMask)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("setRegisterBitsForConfig");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (slaveAddress == pDemod->i2cAddressSLVR) {
        /* Via command */
#ifdef SONY_DEMOD_SUPPORT_ATSC
        result = sony_demod_atsc_SlaveRWriteRegister (pDemod, bank, registerAddress, value, bitMask);
        if (result != SONY_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
#else
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
#endif
    } else {
        result = pDemod->pI2c->WriteOneRegister (pDemod->pI2c, slaveAddress, 0x00, bank);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

		result = sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, slaveAddress, registerAddress, value, bitMask);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}
