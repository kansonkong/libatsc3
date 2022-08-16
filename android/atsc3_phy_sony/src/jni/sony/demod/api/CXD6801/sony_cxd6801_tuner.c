/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/27
  Modification ID : ff73ffebff57618d6a5aa6e2aa9eb009b86060d1
------------------------------------------------------------------------------*/

#include "sony_cxd6801_tuner.h"
#include "sony_cxd6801_stdlib.h"

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t sony_cxd6801_tuner_sw_combined_Initialize (sony_cxd6801_tuner_t * pTuner)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_6801_tuner_sw_combined_Initialize");

    if (!pTuner) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pTuner->pTunerTerrCable) && (pTuner->pTunerTerrCable->Initialize)) {
        result = pTuner->pTunerTerrCable->Initialize (pTuner->pTunerTerrCable);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    if ((pTuner->pTunerSat) && (pTuner->pTunerSat->Initialize)) {
        result = pTuner->pTunerSat->Initialize (pTuner->pTunerSat);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    pTuner->frequencyKHz = 0;
	pTuner->system = SONY_CXD6801_DTV_SYSTEM_UNKNOWN;
	pTuner->bandwidth = SONY_CXD6801_DTV_BW_UNKNOWN;
    pTuner->symbolRateKSps = 0;

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t sony_cxd6801_tuner_sw_combined_Sleep (sony_cxd6801_tuner_t * pTuner);

static sony_cxd6801_result_t sony_cxd6801_tuner_sw_combined_TerrCableTune (sony_cxd6801_tuner_t * pTuner,
                                                           uint32_t centerFreqKHz,
                                                           sony_cxd6801_dtv_system_t system,
                                                           sony_cxd6801_dtv_bandwidth_t bandwidth)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_tuner_sw_combined_TerrCableTune");

    if (!pTuner) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (!SONY_CXD6801_DTV_SYSTEM_IS_TERR_CABLE (system)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (SONY_CXD6801_DTV_SYSTEM_IS_SAT (pTuner->system)) {
        result = sony_cxd6801_tuner_sw_combined_Sleep (pTuner);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    if ((pTuner->pTunerTerrCable) && (pTuner->pTunerTerrCable->TerrCableTune)) {
        result = pTuner->pTunerTerrCable->TerrCableTune (pTuner->pTunerTerrCable,
            centerFreqKHz, system, bandwidth);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        pTuner->frequencyKHz = pTuner->pTunerTerrCable->frequencyKHz;
    } else {
        pTuner->frequencyKHz = centerFreqKHz;
    }

    pTuner->system = system;
    pTuner->bandwidth = bandwidth;
    pTuner->symbolRateKSps = 0;

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t sony_cxd6801_tuner_sw_combined_SatTune (sony_cxd6801_tuner_t * pTuner,
                                                     uint32_t centerFreqKHz,
                                                     sony_cxd6801_dtv_system_t system,
                                                     uint32_t symbolRateKSps)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_tuner_sw_combined_SatTune");

    if (!pTuner) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (!SONY_CXD6801_DTV_SYSTEM_IS_SAT (system)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (SONY_CXD6801_DTV_SYSTEM_IS_TERR_CABLE (pTuner->system)) {
        result = sony_cxd6801_tuner_sw_combined_Sleep (pTuner);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    if ((pTuner->pTunerSat) && (pTuner->pTunerSat->SatTune)) {
        result = pTuner->pTunerSat->SatTune (pTuner->pTunerSat,
            centerFreqKHz, system, symbolRateKSps);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        pTuner->frequencyKHz = pTuner->pTunerSat->frequencyKHz;
    } else {
        pTuner->frequencyKHz = centerFreqKHz;
    }

    pTuner->system = system;
    pTuner->bandwidth = SONY_CXD6801_DTV_BW_UNKNOWN;
    pTuner->symbolRateKSps = symbolRateKSps;

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t sony_cxd6801_tuner_sw_combined_Sleep (sony_cxd6801_tuner_t * pTuner)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_tuner_sw_combined_Sleep");

    if (!pTuner) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pTuner->pTunerTerrCable) && (pTuner->pTunerTerrCable->Sleep)
        && SONY_CXD6801_DTV_SYSTEM_IS_TERR_CABLE (pTuner->system)) {
        result = pTuner->pTunerTerrCable->Sleep (pTuner->pTunerTerrCable);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    if ((pTuner->pTunerSat) && (pTuner->pTunerSat->Sleep)
        && SONY_CXD6801_DTV_SYSTEM_IS_SAT (pTuner->system)) {
        result = pTuner->pTunerSat->Sleep (pTuner->pTunerSat);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    pTuner->frequencyKHz = 0;
    pTuner->system = SONY_CXD6801_DTV_SYSTEM_UNKNOWN;
    pTuner->bandwidth = SONY_CXD6801_DTV_BW_UNKNOWN;
    pTuner->symbolRateKSps = 0;

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t sony_cxd6801_tuner_sw_combined_Shutdown (sony_cxd6801_tuner_t * pTuner)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_tuner_sw_combined_Shutdown");

    if (!pTuner) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pTuner->pTunerTerrCable) && (pTuner->pTunerTerrCable->Shutdown)) {
        result = pTuner->pTunerTerrCable->Shutdown (pTuner->pTunerTerrCable);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    if ((pTuner->pTunerSat) && (pTuner->pTunerSat->Shutdown)) {
        result = pTuner->pTunerSat->Shutdown (pTuner->pTunerSat);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    pTuner->frequencyKHz = 0;
    pTuner->system = SONY_CXD6801_DTV_SYSTEM_UNKNOWN;
    pTuner->bandwidth = SONY_CXD6801_DTV_BW_UNKNOWN;
    pTuner->symbolRateKSps = 0;

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t sony_cxd6801_tuner_sw_combined_ReadRFLevel (sony_cxd6801_tuner_t * pTuner,
                                                         int32_t *pRFLevel)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_tuner_sw_combined_ReadRFLevel");

    if ((!pTuner) || (!pRFLevel)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (SONY_CXD6801_DTV_SYSTEM_IS_TERR_CABLE (pTuner->system)) {
        if ((pTuner->pTunerTerrCable) && (pTuner->pTunerTerrCable->ReadRFLevel)) {
            result = pTuner->pTunerTerrCable->ReadRFLevel (pTuner->pTunerTerrCable, pRFLevel);
            if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        } else {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }
    } else if (SONY_CXD6801_DTV_SYSTEM_IS_SAT (pTuner->system)) {
        if ((pTuner->pTunerSat) && (pTuner->pTunerSat->ReadRFLevel)) {
            result = pTuner->pTunerSat->ReadRFLevel (pTuner->pTunerSat, pRFLevel);
            if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        } else {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t sony_cxd6801_tuner_sw_combined_CalcRFLevelFromAGC (sony_cxd6801_tuner_t * pTuner,
                                                                uint32_t agcValue,
                                                                int32_t *pRFLevel)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_tuner_sw_combined_CalcRFLevelFromAGC");

    if ((!pTuner) || (!pRFLevel)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (SONY_CXD6801_DTV_SYSTEM_IS_TERR_CABLE (pTuner->system)) {
        if ((pTuner->pTunerTerrCable) && (pTuner->pTunerTerrCable->CalcRFLevelFromAGC)) {
            result = pTuner->pTunerTerrCable->CalcRFLevelFromAGC (pTuner->pTunerTerrCable, agcValue, pRFLevel);
            if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        } else {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }
    } else if (SONY_CXD6801_DTV_SYSTEM_IS_SAT (pTuner->system)) {
        if ((pTuner->pTunerSat) && (pTuner->pTunerSat->CalcRFLevelFromAGC)) {
            result = pTuner->pTunerSat->CalcRFLevelFromAGC (pTuner->pTunerSat, agcValue, pRFLevel);
            if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        } else {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_tuner_sw_combined_Create (sony_cxd6801_tuner_t * pTuner,
                                             sony_cxd6801_tuner_t * pTunerTerrCable,
                                             sony_cxd6801_tuner_t * pTunerSat)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_tuner_sw_combined_Create");

    if (!pTuner) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    sony_cxd6801_memset (pTuner, 0, sizeof (sony_cxd6801_tuner_t)); /* Zero filled */

    if (pTunerTerrCable) {
        pTuner->pTunerTerrCable = pTunerTerrCable;
        pTuner->rfLevelFuncTerr = pTunerTerrCable->rfLevelFuncTerr;
    }

    if (pTunerSat) {
        pTuner->pTunerSat = pTunerSat;
        pTuner->rfLevelFuncSat = pTunerSat->rfLevelFuncSat;
        /*
         * symbolRateKSpsForSpectrum member is accessed from satellite blind scan function.
         * So, should be copied to software combined tuner object.
         */
        pTuner->symbolRateKSpsForSpectrum = pTunerSat->symbolRateKSpsForSpectrum;
    }

    pTuner->Initialize = sony_cxd6801_tuner_sw_combined_Initialize;
    pTuner->TerrCableTune = sony_cxd6801_tuner_sw_combined_TerrCableTune;
    pTuner->SatTune = sony_cxd6801_tuner_sw_combined_SatTune;
    pTuner->Sleep = sony_cxd6801_tuner_sw_combined_Sleep;
    pTuner->Shutdown = sony_cxd6801_tuner_sw_combined_Shutdown;
    pTuner->ReadRFLevel = sony_cxd6801_tuner_sw_combined_ReadRFLevel;
    pTuner->CalcRFLevelFromAGC = sony_cxd6801_tuner_sw_combined_CalcRFLevelFromAGC;

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

