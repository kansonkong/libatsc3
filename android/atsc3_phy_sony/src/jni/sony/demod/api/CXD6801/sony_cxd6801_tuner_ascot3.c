/*------------------------------------------------------------------------------
  Copyright 2016-2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/27
  Modification ID : ff73ffebff57618d6a5aa6e2aa9eb009b86060d1
------------------------------------------------------------------------------*/
#include "sony_cxd6801_tuner_ascot3.h"
#include "sony_cxd6801_stdlib.h"

/*------------------------------------------------------------------------------
 Driver Version
------------------------------------------------------------------------------*/
const char* sony_cxd6801_tuner_ascot3_version = SONY_CXD6801_ASCOT3_VERSION;

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_Initialize(sony_cxd6801_tuner_t * pTuner);

static sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_Tune(sony_cxd6801_tuner_t * pTuner,
                                             uint32_t frequencyKHz,
											 sony_cxd6801_dtv_system_t system,
											 sony_cxd6801_dtv_bandwidth_t bandwidth);

static sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_Sleep(sony_cxd6801_tuner_t * pTuner);

static sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_Shutdown(sony_cxd6801_tuner_t * pTuner);

static sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_ReadRFLevel(sony_cxd6801_tuner_t * pTuner, int32_t * pRFLevel);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_Create(sony_cxd6801_tuner_t * pTuner,
	sony_cxd6801_ascot3_xtal_t xtalFreq,
                                        uint8_t i2cAddress,
										sony_cxd6801_i2c_t * pI2c,
                                        uint32_t configFlags,
										sony_cxd6801_ascot3_t * pAscot3Tuner)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_tuner_ascot3_Create");

    if ((!pI2c) || (!pAscot3Tuner) || (!pTuner)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	sony_cxd6801_memset(pTuner, 0, sizeof (sony_cxd6801_tuner_t)); /* Zero filled */

    /* Create the underlying Ascot3 reference driver. */
	result = sony_cxd6801_ascot3_Create(pAscot3Tuner, xtalFreq, i2cAddress, pI2c, configFlags);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Populate / Initialize the sony_tuner_t structure */
    pTuner->i2cAddress = i2cAddress;
    pTuner->pI2c = pI2c;
    pTuner->flags = configFlags;
    pTuner->symbolRateKSpsForSpectrum = 0;

	pTuner->rfLevelFuncTerr = SONY_CXD6801_TUNER_RFLEVEL_FUNC_READ; /* Use ReadRFLevel function for terrestrial */
	pTuner->rfLevelFuncSat = SONY_CXD6801_TUNER_RFLEVEL_FUNC_NOSUPPORT; /* Not supported for satellite */

    pTuner->user = pAscot3Tuner;

	pTuner->Initialize = sony_cxd6801_tuner_ascot3_Initialize;
	pTuner->TerrCableTune = sony_cxd6801_tuner_ascot3_Tune;
    pTuner->SatTune = NULL;
	pTuner->Sleep = sony_cxd6801_tuner_ascot3_Sleep;
	pTuner->Shutdown = sony_cxd6801_tuner_ascot3_Shutdown;
	pTuner->ReadRFLevel = sony_cxd6801_tuner_ascot3_ReadRFLevel;
    pTuner->CalcRFLevelFromAGC = NULL;

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_2875_Create(sony_cxd6801_tuner_t * pTuner,
                                             uint8_t i2cAddress,
											 sony_cxd6801_i2c_t * pI2c,
                                             uint32_t configFlags,
											 sony_cxd6801_ascot3_t * pAscot3Tuner)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_tuner_ascot3_2875_Create");

    if ((!pI2c) || (!pAscot3Tuner) || (!pTuner)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	sony_cxd6801_memset(pTuner, 0, sizeof (sony_cxd6801_tuner_t)); /* Zero filled */

    /* Create the underlying Ascot3R reference driver. */
	result = sony_cxd6801_ascot3_2875_Create(pAscot3Tuner, i2cAddress, pI2c, configFlags);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Populate / Initialize the sony_tuner_t structure */
    pTuner->i2cAddress = i2cAddress;
    pTuner->pI2c = pI2c;
    pTuner->flags = configFlags;
    pTuner->symbolRateKSpsForSpectrum = 0;

	pTuner->rfLevelFuncTerr = SONY_CXD6801_TUNER_RFLEVEL_FUNC_READ; /* Use ReadRFLevel function for terrestrial */
	pTuner->rfLevelFuncSat = SONY_CXD6801_TUNER_RFLEVEL_FUNC_NOSUPPORT; /* Not supported for satellite */

    pTuner->user = pAscot3Tuner;

	pTuner->Initialize = sony_cxd6801_tuner_ascot3_Initialize;
	pTuner->TerrCableTune = sony_cxd6801_tuner_ascot3_Tune;
    pTuner->SatTune = NULL;
	pTuner->Sleep = sony_cxd6801_tuner_ascot3_Sleep;
	pTuner->Shutdown = sony_cxd6801_tuner_ascot3_Shutdown;
	pTuner->ReadRFLevel = sony_cxd6801_tuner_ascot3_ReadRFLevel;
    pTuner->CalcRFLevelFromAGC = NULL;

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_RFFilterConfig(sony_cxd6801_tuner_t * pTuner, uint8_t coeff, uint8_t offset)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_tuner_ascot3_RFFilterConfig");

    if (!pTuner || !pTuner->user) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	result = sony_cxd6801_ascot3_RFFilterConfig(((sony_cxd6801_ascot3_t *)pTuner->user), coeff, offset);

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_SetGPO(sony_cxd6801_tuner_t * pTuner, uint8_t id, uint8_t value)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_tuner_ascot3_Write_GPIO");

    if (!pTuner || !pTuner->user) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	result = sony_cxd6801_ascot3_SetGPO(((sony_cxd6801_ascot3_t *)pTuner->user), id, value);

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_ReadRssi(sony_cxd6801_tuner_t * pTuner, int32_t * pRssi)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_tuner_ascot3_ReadRssi");

    if (!pTuner || !pTuner->user) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	result = sony_cxd6801_ascot3_ReadRssi(((sony_cxd6801_ascot3_t *)pTuner->user), pRssi);

    SONY_CXD6801_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_Initialize(sony_cxd6801_tuner_t * pTuner)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_tuner_ascot3_Initialize");

    if (!pTuner || !pTuner->user) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
	result = sony_cxd6801_ascot3_Initialize(((sony_cxd6801_ascot3_t *)pTuner->user));

    /* Device is in "Power Save" state. */
    pTuner->frequencyKHz = 0;
	pTuner->system = SONY_CXD6801_DTV_SYSTEM_UNKNOWN;
	pTuner->bandwidth = SONY_CXD6801_DTV_BW_UNKNOWN;
    pTuner->symbolRateKSps = 0;

    SONY_CXD6801_TRACE_RETURN (result);
}

static sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_Tune(sony_cxd6801_tuner_t * pTuner,
                                             uint32_t frequencyKHz,
											 sony_cxd6801_dtv_system_t system,
											 sony_cxd6801_dtv_bandwidth_t bandwidth)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
	sony_cxd6801_ascot3_tv_system_t aSystem;

    SONY_CXD6801_TRACE_ENTER ("sony_tuner_ascot3_Tune");

    if (!pTuner || !pTuner->user) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Convert system, bandwidth into dtv system. */
    switch (system) {
	case SONY_CXD6801_DTV_SYSTEM_DVBC:
        switch (bandwidth) {
		case SONY_CXD6801_DTV_BW_6_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBC_6;
            break;
		case SONY_CXD6801_DTV_BW_7_MHZ:
            /* 7MHZ BW setting is the same as 8MHz BW */
		case SONY_CXD6801_DTV_BW_8_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBC_8;
            break;
        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
        }
        break;

	case SONY_CXD6801_DTV_SYSTEM_DVBT:
        switch (bandwidth) {
		case SONY_CXD6801_DTV_BW_5_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBT_5;
            break;
		case SONY_CXD6801_DTV_BW_6_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBT_6;
            break;
		case SONY_CXD6801_DTV_BW_7_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBT_7;
            break;
		case SONY_CXD6801_DTV_BW_8_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBT_8;
            break;
        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
        }
        break;

	case SONY_CXD6801_DTV_SYSTEM_DVBT2:
        switch (bandwidth) {
		case SONY_CXD6801_DTV_BW_1_7_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBT2_1_7;
            break;
		case SONY_CXD6801_DTV_BW_5_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBT2_5;
            break;
		case SONY_CXD6801_DTV_BW_6_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBT2_6;
            break;
		case SONY_CXD6801_DTV_BW_7_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBT2_7;
            break;
		case SONY_CXD6801_DTV_BW_8_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBT2_8;
            break;
        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);

        }
        break;

	case SONY_CXD6801_DTV_SYSTEM_DVBC2:
        switch (bandwidth) {
		case SONY_CXD6801_DTV_BW_6_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBC2_6;
            break;
		case SONY_CXD6801_DTV_BW_8_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBC2_8;
            break;
        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
        }
        break;

	case SONY_CXD6801_DTV_SYSTEM_ISDBT:
        switch (bandwidth) {
		case SONY_CXD6801_DTV_BW_6_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_ISDBT_6;
            break;
		case SONY_CXD6801_DTV_BW_7_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_ISDBT_7;
            break;
		case SONY_CXD6801_DTV_BW_8_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_ISDBT_8;
            break;
        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
        }
        break;

	case SONY_CXD6801_DTV_SYSTEM_ISDBC:
		aSystem = SONY_CXD6801_ASCOT3_DTV_DVBC_6; /* ISDB-C uses DVB-C 6MHz BW setting */
        break;

	case SONY_CXD6801_DTV_SYSTEM_ATSC:
		aSystem = SONY_CXD6801_ASCOT3_DTV_8VSB;
        break;

	case SONY_CXD6801_DTV_SYSTEM_ATSC3:
        switch (bandwidth) {
		case SONY_CXD6801_DTV_BW_6_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_ATSC3_6;
            break;
		case SONY_CXD6801_DTV_BW_7_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_ATSC3_7;
            break;
		case SONY_CXD6801_DTV_BW_8_MHZ:
			aSystem = SONY_CXD6801_ASCOT3_DTV_ATSC3_8;
            break;
        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
        }
        break;

	case SONY_CXD6801_DTV_SYSTEM_J83B:
        switch (bandwidth) {
		case SONY_CXD6801_DTV_BW_J83B_5_06_5_36_MSPS:
			aSystem = SONY_CXD6801_ASCOT3_DTV_DVBC_6; /* J.83B (5.057, 5.361Msps commonly used in US) uses DVB-C 6MHz BW setting */
            break;
		case SONY_CXD6801_DTV_BW_J83B_5_60_MSPS:
			aSystem = SONY_CXD6801_ASCOT3_DTV_J83B_5_6; /* J.83B (5.6Msps used in Japan) uses special setting */
            break;
        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
        }
        break;

    /* Intentional fall-through */
	case SONY_CXD6801_DTV_SYSTEM_UNKNOWN:
    default:
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
	result = sony_cxd6801_ascot3_Tune(((sony_cxd6801_ascot3_t *)pTuner->user), frequencyKHz, aSystem);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Allow the tuner time to settle */
    SONY_CXD6801_SLEEP(50);

	result = sony_cxd6801_ascot3_TuneEnd((sony_cxd6801_ascot3_t *)pTuner->user);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Assign current values. */
	pTuner->frequencyKHz = ((sony_cxd6801_ascot3_t *)pTuner->user)->frequencykHz;
    pTuner->system = system;
    pTuner->bandwidth = bandwidth;
    pTuner->symbolRateKSps = 0;

    SONY_CXD6801_TRACE_RETURN (result);
}

static sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_Sleep(sony_cxd6801_tuner_t * pTuner)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_tuner_ascot3_Sleep");

    if (!pTuner || !pTuner->user) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
	result = sony_cxd6801_ascot3_Sleep(((sony_cxd6801_ascot3_t *)pTuner->user));

    /* Device is in "Power Save" state. */
    pTuner->frequencyKHz = 0;
	pTuner->system = SONY_CXD6801_DTV_SYSTEM_UNKNOWN;
	pTuner->bandwidth = SONY_CXD6801_DTV_BW_UNKNOWN;
    pTuner->symbolRateKSps = 0;

    SONY_CXD6801_TRACE_RETURN (result);
}

static sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_Shutdown(sony_cxd6801_tuner_t * pTuner)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_tuner_ascot3_Shutdown");

    if (!pTuner || !pTuner->user) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
	result = sony_cxd6801_ascot3_Sleep(((sony_cxd6801_ascot3_t *)pTuner->user));

    /* Device is in "Power Save" state. */
    pTuner->frequencyKHz = 0;
	pTuner->system = SONY_CXD6801_DTV_SYSTEM_UNKNOWN;
	pTuner->bandwidth = SONY_CXD6801_DTV_BW_UNKNOWN;
    pTuner->symbolRateKSps = 0;

    SONY_CXD6801_TRACE_RETURN (result);
}

static sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_ReadRFLevel(sony_cxd6801_tuner_t * pTuner, int32_t * pRFLevel)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_tuner_ascot3_ReadRFLevel");

    if (!pTuner || !pTuner->user || !pRFLevel) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (!SONY_CXD6801_DTV_SYSTEM_IS_TERR_CABLE(pTuner->system)) {
        /* Terrestrial/Cable Only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	result = sony_cxd6801_ascot3_ReadRssi(((sony_cxd6801_ascot3_t *)pTuner->user), pRFLevel);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Add IFOUT value */
    switch (pTuner->system) {
	case SONY_CXD6801_DTV_SYSTEM_DVBT:
	case SONY_CXD6801_DTV_SYSTEM_DVBT2:
	case SONY_CXD6801_DTV_SYSTEM_DVBC2:
	case SONY_CXD6801_DTV_SYSTEM_ATSC3:
        *pRFLevel -= 400; /* -4.0dBm */
        break;

	case SONY_CXD6801_DTV_SYSTEM_DVBC:
	case SONY_CXD6801_DTV_SYSTEM_ISDBC:
	case SONY_CXD6801_DTV_SYSTEM_J83B:
        *pRFLevel -= 150; /* -1.5dBm */
        break;

	case SONY_CXD6801_DTV_SYSTEM_ISDBT:
        *pRFLevel -= 450; /* -4.5dBm */
        break;

	case SONY_CXD6801_DTV_SYSTEM_ATSC:
        *pRFLevel -= 500; /* -5.0dBm */
        break;

    default:
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}
