/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/08/01
  Modification ID : b30d76210d343216ea52b88e9b450c8fd5c0359f
------------------------------------------------------------------------------*/
#include "sony_cxd_tuner_helene.h"

/*------------------------------------------------------------------------------
 Driver Version
------------------------------------------------------------------------------*/
const char* sony_cxd_tuner_helene_version =  sony_cxd_HELENE_VERSION;

/*------------------------------------------------------------------------------
 Static Prototypes
------------------------------------------------------------------------------*/
static sony_cxd_result_t sony_cxd_tuner_helene_terr_Initialize (sony_cxd_tuner_terr_cable_t * pTuner);

static sony_cxd_result_t sony_cxd_tuner_helene_terr_Tune (sony_cxd_tuner_terr_cable_t * pTuner,
                                                  uint32_t centerFreqKHz,
                                                  sony_cxd_dtv_system_t dtvSystem,
                                                  sony_cxd_dtv_bandwidth_t bandwidth);

static sony_cxd_result_t sony_cxd_tuner_helene_terr_Sleep (sony_cxd_tuner_terr_cable_t * pTuner);

static sony_cxd_result_t sony_cxd_tuner_helene_terr_Shutdown (sony_cxd_tuner_terr_cable_t * pTuner);

static sony_cxd_result_t sony_cxd_tuner_helene_terr_ReadRFLevel (sony_cxd_tuner_terr_cable_t * pTuner, int32_t * pRFLevel);

static sony_cxd_result_t sony_cxd_tuner_helene_sat_Initialize (sony_cxd_tuner_sat_t * pTuner);

static sony_cxd_result_t sony_cxd_tuner_helene_sat_Tune (sony_cxd_tuner_sat_t * pTuner,
                                                 uint32_t centerFreqKHz,
                                                 sony_cxd_dtv_system_t dtvSystem,
                                                 uint32_t symbolRateKSps);

static sony_cxd_result_t sony_cxd_tuner_helene_sat_Sleep (sony_cxd_tuner_sat_t * pTuner);

static sony_cxd_result_t sony_cxd_tuner_helene_sat_Shutdown (sony_cxd_tuner_sat_t * pTuner);

static sony_cxd_result_t sony_cxd_tuner_helene_sat_AGCLevel2AGCdB (sony_cxd_tuner_sat_t * pTuner,
                                                           uint32_t AGCLevel,
                                                           int32_t * pAGCdB);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd_result_t sony_cxd_tuner_helene_Create (sony_cxd_tuner_terr_cable_t * pTunerTerrCable,
                                        sony_cxd_tuner_sat_t *pTunerSat,
                                        sony_cxd_helene_xtal_t xtalFreq,
                                        uint8_t i2cAddress,
                                        sony_cxd_i2c_t * pI2c,
                                        uint32_t configFlags,
                                        sony_cxd_helene_t * pHeleneTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_Create");

    if ((!pI2c) || (!pHeleneTuner) || (!pTunerTerrCable) || (!pTunerSat)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Create the underlying HELENE reference driver. */
    result = sony_cxd_helene_Create (pHeleneTuner, xtalFreq, i2cAddress, pI2c, configFlags);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Populate / Initialise the sony_cxd_tuner_terr_cable_t structure */
    pTunerTerrCable->i2cAddress = i2cAddress;
    pTunerTerrCable->pI2c = pI2c;
    pTunerTerrCable->flags = configFlags;
    pTunerTerrCable->frequencyKHz = 0;
    pTunerTerrCable->system = sony_cxd_DTV_SYSTEM_UNKNOWN;
    pTunerTerrCable->bandwidth = sony_cxd_DTV_BW_UNKNOWN;
    pTunerTerrCable->Initialize = sony_cxd_tuner_helene_terr_Initialize;
    pTunerTerrCable->Tune = sony_cxd_tuner_helene_terr_Tune;
    pTunerTerrCable->Sleep = sony_cxd_tuner_helene_terr_Sleep;
    pTunerTerrCable->Shutdown = sony_cxd_tuner_helene_terr_Shutdown;
    pTunerTerrCable->ReadRFLevel = sony_cxd_tuner_helene_terr_ReadRFLevel;
    pTunerTerrCable->CalcRFLevelFromAGC = NULL;
    pTunerTerrCable->user = pHeleneTuner;

    /* Populate / Initialise the sony_cxd_tuner_sat_t structure */
    pTunerSat->i2cAddress = i2cAddress;
    pTunerSat->pI2c = pI2c;
    pTunerSat->flags = configFlags;
    pTunerSat->frequencyKHz = 0;
    pTunerSat->system = sony_cxd_DTV_SYSTEM_UNKNOWN;
    pTunerSat->symbolRateKSps = 0;
    pTunerSat->symbolRateKSpsForSpectrum = 45000;
    pTunerSat->Initialize = sony_cxd_tuner_helene_sat_Initialize;
    pTunerSat->Tune = sony_cxd_tuner_helene_sat_Tune;
    pTunerSat->Sleep = sony_cxd_tuner_helene_sat_Sleep;
    pTunerSat->Shutdown = sony_cxd_tuner_helene_sat_Shutdown;
    pTunerSat->AGCLevel2AGCdB = sony_cxd_tuner_helene_sat_AGCLevel2AGCdB;
    pTunerSat->user = pHeleneTuner;

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_tuner_helene_SetGPO (sony_cxd_tuner_terr_cable_t * pTuner, uint8_t id, uint8_t value)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_Write_GPIO");

    if (!pTuner || !pTuner->user) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    result = sony_cxd_helene_SetGPO (((sony_cxd_helene_t *) pTuner->user), id, value);

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_tuner_helene_GetGPI1 (sony_cxd_tuner_terr_cable_t * pTuner, uint8_t * pValue)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_GetGPI1");

    if (!pTuner || !pTuner->user) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    result = sony_cxd_helene_GetGPI1 (((sony_cxd_helene_t *) pTuner->user), pValue);

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_tuner_helene_RFFilterConfig (sony_cxd_tuner_terr_cable_t * pTuner, uint8_t coeff, uint8_t offset)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_RFFilterConfig");

    if (!pTuner || !pTuner->user) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    result = sony_cxd_helene_RFFilterConfig (((sony_cxd_helene_t *) pTuner->user), coeff, offset);

    sony_cxd_TRACE_RETURN (result);
}
/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd_result_t sony_cxd_tuner_helene_terr_Initialize (sony_cxd_tuner_terr_cable_t * pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_terr_Initialize");

    if (!pTuner || !pTuner->user) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
    result = sony_cxd_helene_Initialize (((sony_cxd_helene_t *) pTuner->user));

    /* Device is in "Power Save" state. */
    pTuner->system = sony_cxd_DTV_SYSTEM_UNKNOWN;
    pTuner->frequencyKHz = 0;
    pTuner->bandwidth = sony_cxd_DTV_BW_UNKNOWN;

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t sony_cxd_tuner_helene_terr_Tune (sony_cxd_tuner_terr_cable_t * pTuner,
                                                  uint32_t centerFreqKHz,
                                                  sony_cxd_dtv_system_t system,
                                                  sony_cxd_dtv_bandwidth_t bandwidth)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_helene_tv_system_t hSystem = sony_cxd_HELENE_TV_SYSTEM_UNKNOWN;

    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_terr_Tune");

    if (!pTuner || !pTuner->user) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

   /* Call into underlying driver. Convert system, bandwidth into dtv system. */
    switch (system) {
    case sony_cxd_DTV_SYSTEM_DVBC:
        switch (bandwidth) {
        case sony_cxd_DTV_BW_6_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBC_6;
            break;
        case sony_cxd_DTV_BW_7_MHZ:
            /* 7MHZ BW setting is the same as 8MHz BW */
        case sony_cxd_DTV_BW_8_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBC_8;
            break;
        default:
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
        }
        break;

    case sony_cxd_DTV_SYSTEM_DVBT:
        switch (bandwidth) {
        case sony_cxd_DTV_BW_5_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBT_5;
            break;
        case sony_cxd_DTV_BW_6_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBT_6;
            break;
        case sony_cxd_DTV_BW_7_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBT_7;
            break;
        case sony_cxd_DTV_BW_8_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBT_8;
            break;
        default:
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
        }
        break;

    case sony_cxd_DTV_SYSTEM_DVBT2:
        switch (bandwidth) {
        case sony_cxd_DTV_BW_1_7_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBT2_1_7;
            break;
        case sony_cxd_DTV_BW_5_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBT2_5;
            break;
        case sony_cxd_DTV_BW_6_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBT2_6;
            break;
        case sony_cxd_DTV_BW_7_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBT2_7;
            break;
        case sony_cxd_DTV_BW_8_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBT2_8;
            break;
        default:
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);

        }
        break;

    case sony_cxd_DTV_SYSTEM_DVBC2:
        switch (bandwidth) {
        case sony_cxd_DTV_BW_6_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBC2_6;
            break;
        case sony_cxd_DTV_BW_8_MHZ:
            hSystem = sony_cxd_HELENE_DTV_DVBC2_8;
            break;
        default:
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
        }
        break;

    case sony_cxd_DTV_SYSTEM_ISDBT:
        switch (bandwidth) {
        case sony_cxd_DTV_BW_6_MHZ:
            hSystem = sony_cxd_HELENE_DTV_ISDBT_6;
            break;
        case sony_cxd_DTV_BW_7_MHZ:
            hSystem = sony_cxd_HELENE_DTV_ISDBT_7;
            break;
        case sony_cxd_DTV_BW_8_MHZ:
            hSystem = sony_cxd_HELENE_DTV_ISDBT_8;
            break;
        default:
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
        }
        break;

    case sony_cxd_DTV_SYSTEM_ISDBC:
        hSystem = sony_cxd_HELENE_DTV_DVBC_6; /* ISDB-C uses DVB-C 6MHz BW setting */
        break;

    case sony_cxd_DTV_SYSTEM_J83B:
        switch (bandwidth) {
        case sony_cxd_DTV_BW_J83B_5_06_5_36_MSPS:
            hSystem = sony_cxd_HELENE_DTV_DVBC_6; /* J.83B (5.057, 5.361Msps commonly used in US) uses DVB-C 6MHz BW setting */
            break;
        case sony_cxd_DTV_BW_J83B_5_60_MSPS:
            hSystem = sony_cxd_HELENE_DTV_J83B_5_6; /* J.83B (5.6Msps used in Japan) uses special setting */
            break;
        default:
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
        }
        break;

    /* Intentional fall-through */
    case sony_cxd_DTV_SYSTEM_UNKNOWN:
    default:
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    result = sony_cxd_helene_terr_Tune(((sony_cxd_helene_t *) pTuner->user), centerFreqKHz, hSystem);
    if (result != sony_cxd_RESULT_OK) {
        pTuner->system = sony_cxd_DTV_SYSTEM_UNKNOWN;
        pTuner->frequencyKHz = 0;
        pTuner->bandwidth = sony_cxd_DTV_BW_UNKNOWN;
        sony_cxd_TRACE_RETURN (result);
    }

    /* Allow the tuner time to settle */
    sony_cxd_SLEEP(50);

    result = sony_cxd_helene_terr_TuneEnd((sony_cxd_helene_t *) pTuner->user);
    if (result != sony_cxd_RESULT_OK) {
        pTuner->system = sony_cxd_DTV_SYSTEM_UNKNOWN;
        pTuner->frequencyKHz = 0;
        pTuner->bandwidth = sony_cxd_DTV_BW_UNKNOWN;
        sony_cxd_TRACE_RETURN (result);
    }

    /* Assign current values. */
    pTuner->system = system;
    pTuner->frequencyKHz = ((sony_cxd_helene_t *) pTuner->user)->frequencykHz;
    pTuner->bandwidth = bandwidth;

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t sony_cxd_tuner_helene_terr_Sleep (sony_cxd_tuner_terr_cable_t * pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_terr_Sleep");

    if (!pTuner || !pTuner->user) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
    result = sony_cxd_helene_Sleep (((sony_cxd_helene_t *) pTuner->user));

    /* Device is in "Power Save" state. */
    pTuner->system = sony_cxd_DTV_SYSTEM_UNKNOWN;
    pTuner->frequencyKHz = 0;
    pTuner->bandwidth = sony_cxd_DTV_BW_UNKNOWN;

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t sony_cxd_tuner_helene_terr_Shutdown (sony_cxd_tuner_terr_cable_t * pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_terr_Shutdown");

    if (!pTuner || !pTuner->user) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
    result = sony_cxd_helene_Sleep (((sony_cxd_helene_t *) pTuner->user));

    /* Device is in "Power Save" state. */
    pTuner->system = sony_cxd_DTV_SYSTEM_UNKNOWN;
    pTuner->frequencyKHz = 0;
    pTuner->bandwidth = sony_cxd_DTV_BW_UNKNOWN;

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t sony_cxd_tuner_helene_terr_ReadRFLevel (sony_cxd_tuner_terr_cable_t * pTuner, int32_t * pRFLevel)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_terr_ReadRFLevel");

    if (!pTuner || !pTuner->user || !pRFLevel) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    result = sony_cxd_helene_ReadRssi (((sony_cxd_helene_t *) pTuner->user), pRFLevel);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    *pRFLevel *= 10; /* dB x 1000 value should be returned. */

    /* Add IFOUT value */
    switch (pTuner->system) {
    case sony_cxd_DTV_SYSTEM_DVBT:
    case sony_cxd_DTV_SYSTEM_DVBT2:
    case sony_cxd_DTV_SYSTEM_DVBC2:
        *pRFLevel -= 4000; /* -4.0dBm */
        break;

    case sony_cxd_DTV_SYSTEM_DVBC:
    case sony_cxd_DTV_SYSTEM_ISDBC:
    case sony_cxd_DTV_SYSTEM_J83B:
        *pRFLevel -= 1500; /* -1.5dBm */
        break;

    case sony_cxd_DTV_SYSTEM_ISDBT:
        *pRFLevel -= 4500; /* -4.5dBm */
        break;

    default:
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t sony_cxd_tuner_helene_sat_Initialize (sony_cxd_tuner_sat_t * pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_sat_Initialize");

    if (!pTuner || !pTuner->user) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Tuner IC initialization is normally unnecessary because it is done in terrestrial side. */
#if 0
    result = sony_cxd_helene_Initialize (((sony_cxd_helene_t *) pTuner->user));
#endif

    /* Device is in "Power Save" state. */
    pTuner->system = sony_cxd_DTV_SYSTEM_UNKNOWN;
    pTuner->frequencyKHz = 0;
    pTuner->symbolRateKSps = 0;

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t sony_cxd_tuner_helene_sat_Tune (sony_cxd_tuner_sat_t * pTuner,
                                                 uint32_t centerFreqKHz,
                                                 sony_cxd_dtv_system_t dtvSystem,
                                                 uint32_t symbolRateKSps)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_helene_tv_system_t hSystem = sony_cxd_HELENE_STV_DVBS2;

    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_sat_Tune");

    if ((!pTuner) || (!pTuner->user)){
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    switch(dtvSystem)
    {
    case sony_cxd_DTV_SYSTEM_DVBS:
        hSystem = sony_cxd_HELENE_STV_DVBS;
        break;

    case sony_cxd_DTV_SYSTEM_DVBS2:
        hSystem = sony_cxd_HELENE_STV_DVBS2;
        break;

    case sony_cxd_DTV_SYSTEM_ISDBS:
    case sony_cxd_DTV_SYSTEM_ISDBS3:
        hSystem = sony_cxd_HELENE_STV_ISDBS;
        break;

    default:
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    result = sony_cxd_helene_sat_Tune (((sony_cxd_helene_t *) pTuner->user), centerFreqKHz, hSystem, symbolRateKSps);
    if (result != sony_cxd_RESULT_OK){sony_cxd_TRACE_RETURN (result);}

    pTuner->frequencyKHz = ((sony_cxd_helene_t *) pTuner->user)->frequencykHz;
    pTuner->system = dtvSystem;
    pTuner->symbolRateKSps = symbolRateKSps;

    /* Tuner stabillization time */
    sony_cxd_SLEEP (50);

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t sony_cxd_tuner_helene_sat_Sleep (sony_cxd_tuner_sat_t * pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_sat_Sleep");

    if (!pTuner || !pTuner->user) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
    result = sony_cxd_helene_Sleep (((sony_cxd_helene_t *) pTuner->user));

    /* Device is in "Power Save" state. */
    pTuner->system = sony_cxd_DTV_SYSTEM_UNKNOWN;
    pTuner->frequencyKHz = 0;
    pTuner->symbolRateKSps = 0;

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t sony_cxd_tuner_helene_sat_Shutdown (sony_cxd_tuner_sat_t * pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_tuner_helene_sat_Shutdown");

    if (!pTuner || !pTuner->user) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Call into underlying driver. */
    result = sony_cxd_helene_Sleep (((sony_cxd_helene_t *) pTuner->user));

    /* Device is in "Power Save" state. */
    pTuner->system = sony_cxd_DTV_SYSTEM_UNKNOWN;
    pTuner->frequencyKHz = 0;
    pTuner->symbolRateKSps = 0;

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t sony_cxd_tuner_helene_sat_AGCLevel2AGCdB (sony_cxd_tuner_sat_t * pTuner,
                                                           uint32_t AGCLevel,
                                                           int32_t * pAGCdB)
{
    int32_t tempA = 0;
    uint8_t isNegative = 0;
    uint32_t tempDiv = 0;
    uint32_t tempQ = 0;
    uint32_t tempR = 0;
    sony_cxd_TRACE_ENTER("sony_cxd_tuner_helene_sat_AGCLevel2AGCdB");

    if ((!pTuner) || (!pAGCdB)){
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /*------------------------------------------------
      Gain_db      = AGCLevel * (-14   / 403) + 97
      Gain_db_x100 = AGCLevel * (-1400 / 403) + 9700
    -------------------------------------------------*/
    tempA = (int32_t)AGCLevel * (-1400);

    tempDiv = 403;
    if (tempA > 0){
        isNegative = 0;
        tempQ = (uint32_t)tempA / tempDiv;
        tempR = (uint32_t)tempA % tempDiv;
    } else {
        isNegative = 1;
        tempQ = (uint32_t)(tempA * (-1)) / tempDiv;
        tempR = (uint32_t)(tempA * (-1)) % tempDiv;
    }

    if (isNegative){
        if (tempR >= (tempDiv/2)){
            *pAGCdB = (int32_t)(tempQ + 1) * (int32_t)(-1);
        } else {
            *pAGCdB = (int32_t)tempQ * (int32_t)(-1);
        }
    } else {
        if (tempR >= (tempDiv/2)){
            *pAGCdB = (int32_t)(tempQ + 1);
        } else {
            *pAGCdB = (int32_t)tempQ;
        }
    }
    *pAGCdB += 9700;

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}
