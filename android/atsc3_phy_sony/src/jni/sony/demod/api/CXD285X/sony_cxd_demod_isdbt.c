/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2017/01/05
  Modification ID : 35aa94b8a08dbb4980556cec948c569693b34a38
------------------------------------------------------------------------------*/

#include "sony_cxd_demod_isdbt.h"
#include "sony_cxd_demod_isdbt_monitor.h"

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Configure the demodulator from Sleep to Active for ISDB-T
*/
static sony_cxd_result_t SLtoAIT(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active ISDB-T to Active to ISDB-T.  Used
        for changing channel parameters.
*/
static sony_cxd_result_t AITtoAIT(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active ISDB-T to Sleep.
*/
static sony_cxd_result_t AITtoSL(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator from EWS-IT to EWS-IT.
*/
static sony_cxd_result_t EWStoEWS(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator from Sleep to EWS-IT.
*/
static sony_cxd_result_t SLtoEWS(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator from EWS-IT to Sleep EWS-IT.
*/
static sony_cxd_result_t EWStoSL(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active ISDB-T to EWS-IT.
*/
static sony_cxd_result_t AITtoEWS(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator from EWS-IT to Active ISDB-T .
*/
static sony_cxd_result_t EWStoAIT(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator for common settings .
*/
static sony_cxd_result_t SLtoAIT_CommonSetting(sony_cxd_demod_t * pDemod);

/**
 @brief Cancel the demodulator for common settings .
*/
static sony_cxd_result_t AITtoSL_CommonSetting(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator for tuner optimisations and bandwidth specific
        settings.
*/
static sony_cxd_result_t SLtoAIT_BandSetting(sony_cxd_demod_t * pDemod);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd_result_t sony_cxd_demod_isdbt_Tune(sony_cxd_demod_t * pDemod,
	sony_cxd_isdbt_tune_param_t * pTuneParam)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_Tune");

    if ((!pDemod) || (!pTuneParam)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if ((pDemod->state == sony_cxd_DEMOD_STATE_ACTIVE) && (pDemod->system == sony_cxd_DTV_SYSTEM_ISDBT)) {
        /* Demodulator Active and set to ISDB-T mode */
        pDemod->bandwidth = pTuneParam->bandwidth;
		if (pDemod->isdbtEwsState == sony_cxd_DEMOD_ISDBT_STATE_NORMAL) {
            result = AITtoAIT (pDemod);
			if (result != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(result);
            }
        }
        else { /* EWS */
            result = EWStoAIT (pDemod);
			if (result != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(result);
            }
        }
    }
	else if ((pDemod->state == sony_cxd_DEMOD_STATE_ACTIVE) && (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT)){
        /* Demodulator Active but not ISDB-T mode */
		result = sony_cxd_demod_Sleep(pDemod);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

		pDemod->system = sony_cxd_DTV_SYSTEM_ISDBT;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLtoAIT (pDemod);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }
    }
	else if (pDemod->state == sony_cxd_DEMOD_STATE_SLEEP) {
        /* Demodulator in Sleep mode */
		pDemod->system = sony_cxd_DTV_SYSTEM_ISDBT;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLtoAIT (pDemod);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }
    }
    else {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Update demodulator state */
	pDemod->state = sony_cxd_DEMOD_STATE_ACTIVE;
	pDemod->isdbtEwsState = sony_cxd_DEMOD_ISDBT_STATE_NORMAL;

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_EWSTune(sony_cxd_demod_t * pDemod,
	sony_cxd_isdbt_tune_param_t * pTuneParam)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_EWSTune");

    if ((!pDemod) || (!pTuneParam)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if ((pDemod->state == sony_cxd_DEMOD_STATE_ACTIVE) && (pDemod->system == sony_cxd_DTV_SYSTEM_ISDBT)) {
        /* Demodulator Active and set to ISDB-T mode */
        pDemod->bandwidth = pTuneParam->bandwidth;
		if (pDemod->isdbtEwsState == sony_cxd_DEMOD_ISDBT_STATE_NORMAL) {
            result = AITtoEWS (pDemod);
			if (result != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(result);
            }
        }
        else { /* EWS */
            result = EWStoEWS (pDemod);
			if (result != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(result);
            }
        }
    }
	else if ((pDemod->state == sony_cxd_DEMOD_STATE_ACTIVE) && (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT)){
        /* Demodulator Active but not ISDB-T mode */
		result = sony_cxd_demod_Sleep(pDemod);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }

		pDemod->system = sony_cxd_DTV_SYSTEM_ISDBT;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLtoEWS (pDemod);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }
    }
	else if (pDemod->state == sony_cxd_DEMOD_STATE_SLEEP) {
        /* Demodulator in Sleep mode */
		pDemod->system = sony_cxd_DTV_SYSTEM_ISDBT;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLtoEWS (pDemod);
		if (result != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(result);
        }
    }
    else {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Update demodulator state */
	pDemod->state = sony_cxd_DEMOD_STATE_ACTIVE;
	pDemod->isdbtEwsState = sony_cxd_DEMOD_ISDBT_STATE_EWS;

    /* After caling this api, "sony_cxd_demod_isdbt_EWSTuneEnd"
     * api must be used instead of "sony_cxd_demod_TuneEnd"
     */

	sony_cxd_TRACE_RETURN(result);
}


sony_cxd_result_t sony_cxd_demod_isdbt_Sleep(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_Sleep");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if ((pDemod->state == sony_cxd_DEMOD_STATE_ACTIVE) && (pDemod->system == sony_cxd_DTV_SYSTEM_ISDBT)) {
		if (pDemod->isdbtEwsState == sony_cxd_DEMOD_ISDBT_STATE_NORMAL) {
            result = AITtoSL (pDemod);
			if (result != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(result);
            }
        }
        else {
            result = EWStoSL (pDemod);
			if (result != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(result);
            }
        }
    }

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_EWSTuneEnd(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_EWSTuneEnd");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) ||
		(pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) ||
		(pDemod->isdbtEwsState != sony_cxd_DEMOD_ISDBT_STATE_EWS)){
        /* This api is accepted in ISDB-T EWS state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	result = sony_cxd_demod_SoftReset(pDemod);

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_CheckDemodLock(sony_cxd_demod_t * pDemod,
	sony_cxd_demod_lock_result_t * pLock)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;

    uint8_t dmdlock = 0;
    uint8_t tslock = 0;
    uint8_t unlockDetected = 0;

	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_CheckDemodLock");

    if (!pDemod || !pLock) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	*pLock = sony_cxd_DEMOD_LOCK_RESULT_NOTDETECT;

	result = sony_cxd_demod_isdbt_monitor_SyncStat(pDemod, &dmdlock, &tslock, &unlockDetected);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    if (unlockDetected) {
		*pLock = sony_cxd_DEMOD_LOCK_RESULT_UNLOCKED;
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
    }

    if (dmdlock) {
		*pLock = sony_cxd_DEMOD_LOCK_RESULT_LOCKED;
    }

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_CheckTSLock(sony_cxd_demod_t * pDemod,
	sony_cxd_demod_lock_result_t * pLock)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;

    uint8_t dmdlock = 0;
    uint8_t tslock = 0;
    uint8_t unlockDetected = 0;

	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_CheckTSLock");

    if (!pDemod || !pLock) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	*pLock = sony_cxd_DEMOD_LOCK_RESULT_NOTDETECT;

	result = sony_cxd_demod_isdbt_monitor_SyncStat(pDemod, &dmdlock, &tslock, &unlockDetected);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    if (unlockDetected) {
		*pLock = sony_cxd_DEMOD_LOCK_RESULT_UNLOCKED;
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
    }

    if (tslock) {
		*pLock = sony_cxd_DEMOD_LOCK_RESULT_LOCKED;
    }

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_SetPreset(sony_cxd_demod_t * pDemod,
	sony_cxd_demod_isdbt_preset_info_t* pPresetInfo)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_SetPreset");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }
    /* Setting Fast Acquisition Mode */
    /* Set SLV-T Bank : 0x60 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /*    - Disable Fast Acquisition Mode
     *
     *      When the TMCC information is not known in advance, such as in channel scan
     *      operation, disable Fast Acquisition Mode by setting the following register
     *      values.
     *
     *    slave    Bank    Addr    Bit    default      Value        Name
     *   ---------------------------------------------------------------------------------
     *   <SLV-T>   60h     59h    [0]       8'h00       8'h00      OCTL_PRESET_EN
     *   <SLV-T>   60h     5Ah    [0]       8'h00       8'h00      OCTL_S2_FRAMESYNC_EN
     */
    if (!pPresetInfo) {
        uint8_t data[2] = {0x00, 0x00};
		if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x59, data, 2) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
    }
    /*    - Enable Fast Acquisition Mode
     *
     *      Once the TMCC information is known, such as in normal channel tuning, you can
     *      enable Fast Acquisition Mode by setting the following register values.
     *
     *   slave    Bank    Addr    Bit    default      Value        Name
     *  ---------------------------------------------------------------------------------
     *  <SLV-T>   60h     59h    [0]       8'h00       8'h01      OCTL_PRESET_EN
     *  <SLV-T>   60h     5Ah    [0]       8'h00       8'h01      OCTL_S2_FRAMESYNC_EN
     */
    else {
        uint8_t data[2] = {0x01, 0x01};
		if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x59, data, 2) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
        /*
         *       Bank  Addr  Bit    TMCC Bit assign.(*)   Default    Name
         *       -----------------------------------------------------------------------
         *       60h   62h  [7:0]   "B20-27"               8'h3D    PIR_HOST_TMCC_SET_2
         *       60h   63h  [7:0]   "B28-35"               8'h25    PIR_HOST_TMCC_SET_3
         *       60h   64h  [7:0]   "B36-43"               8'h8B    PIR_HOST_TMCC_SET_4
         *       60h   65h  [7:0]   "B44-51"               8'h4B    PIR_HOST_TMCC_SET_5
         *       60h   66h  [7:0]   "B52-59"               8'h3F    PIR_HOST_TMCC_SET_6
         *       60h   67h  [7:0]   "B60-67"               8'hFF    PIR_HOST_TMCC_SET_7
         *       60h   68h  [7:0]   "B68-75"               8'h25    PIR_HOST_TMCC_SET_8
         *       60h   69h  [7:0]   "B76-83"               8'h8B    PIR_HOST_TMCC_SET_9
         *       60h   6Ah  [7:0]   "B84-91"               8'h4B    PIR_HOST_TMCC_SET_10
         *       60h   6Bh  [7:0]   "B92-99"               8'h3F    PIR_HOST_TMCC_SET_11
         *       60h   6Ch  [7:0]   "B100-107"             8'hFF    PIR_HOST_TMCC_SET_12
         *       60h   6Dh  [7:0]   "B108-115"             8'hFF    PIR_HOST_TMCC_SET_13
         *       60h   6Eh  [7:0]   "B116-121,2'b00"       8'hFC    PIR_HOST_TMCC_SET_14
         */
		if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x62, pPresetInfo->data, sizeof(pPresetInfo->data)) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
    }

	sony_cxd_TRACE_RETURN(result);
}


/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd_result_t SLtoAIT(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("SLtoAIT");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Configure TS clock Mode and Frequency */
	result = sony_cxd_demod_SetTSClockModeAndFreq(pDemod, sony_cxd_DTV_SYSTEM_ISDBT);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-X Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set demod mode */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x06) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set TS/TLV mode */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable demod clock */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, 0x74) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x49, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* ISDB-T Initialize setting */
    result = SLtoAIT_CommonSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set tuner and bandwidth specific settings */
    result = SLtoAIT_BandSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable HiZ Setting 1 (TAGC, SAGC(Hi-Z), TSVALID, TSSYNC, TSCLK) */
	if (sony_cxd_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x08, 0x1F) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable HiZ Setting 2 */
	result = sony_cxd_demod_SetTSDataPinHiZ(pDemod, 0);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

	sony_cxd_TRACE_RETURN(result);
}

static sony_cxd_result_t SLtoEWS(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("SLtoEWS");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Configure TS clock Mode and Frequency */
	result = sony_cxd_demod_SetTSClockModeAndFreq(pDemod, sony_cxd_DTV_SYSTEM_ISDBT);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-X Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set demod mode */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x06) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable EWS mode */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x35, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set TS/TLV mode */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable demod clock */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, 0x74) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x49, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* ISDB-T Initialize setting */
    result = SLtoAIT_CommonSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set tuner and bandwidth specific settings */
    result = SLtoAIT_BandSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable/Enable HiZ Setting 1 (TAGC, SAGC(Hi-Z) TSVALID(Hi-Z), TSSYNC(Hi-Z), TSCLK(Hi-Z)) */
	if (sony_cxd_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x0F, 0x1F) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable Hi-Z setting */
	result = sony_cxd_demod_SetTSDataPinHiZ(pDemod, 1);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

	sony_cxd_TRACE_RETURN(result);
}

static sony_cxd_result_t AITtoSL(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("AITtoSL");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable TS output */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable Hi-Z setting 1 (TAGC, SAGC, TSVALID, TSSYNC, TSCLK) */
	if (sony_cxd_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x1F, 0x1F) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable Hi-Z setting 2 */
	result = sony_cxd_demod_SetTSDataPinHiZ(pDemod, 1);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Cancel demod parameter */
    result = AITtoSL_CommonSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-X Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x49, 0x33) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, 0x21) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Demodulator SW reset */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFE, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable demodulator clock */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set tstlv mode to default */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set demodulator mode to default */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	sony_cxd_TRACE_RETURN(result);
}

static sony_cxd_result_t AITtoEWS(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("AITtoEWS");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable TS output */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set EWS mode */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x35, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* ISDB-T Initialize setting */
    result = SLtoAIT_CommonSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set tuner and bandwidth specific settings */
    result = SLtoAIT_BandSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable HiZ Setting 1 (TAGC, SAGC(Hi-Z) TSVALID(Hi-Z), TSSYNC(Hi-Z), TSCLK(Hi-Z)) */
	if (sony_cxd_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x0F, 0x1F) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable Hi-Z setting */
	result = sony_cxd_demod_SetTSDataPinHiZ(pDemod, 1);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

	sony_cxd_TRACE_RETURN(result);
}

static sony_cxd_result_t AITtoAIT(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("AITtoAIT");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable TS output */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set tuner and bandwidth specific settings */
    result = SLtoAIT_BandSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

	sony_cxd_TRACE_RETURN(result);
}

static sony_cxd_result_t EWStoEWS(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("EWStoEWS");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }
    /* Set tuner and bandwidth specific settings */
    result = SLtoAIT_BandSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

	sony_cxd_TRACE_RETURN(result);
}

static sony_cxd_result_t EWStoAIT(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("EWStoAIT");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable EWS mode  */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x35, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* ISDB-T Initialize setting */
    result = SLtoAIT_CommonSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set tuner and bandwidth specific settings */
    result = SLtoAIT_BandSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable HiZ Setting 1 (TAGC, SAGC(Hi-Z), TSVALID, TSSYNC, TSCLK) */
	if (sony_cxd_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x08, 0x1F) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable HiZ Setting 2 */
	result = sony_cxd_demod_SetTSDataPinHiZ(pDemod, 0);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

	sony_cxd_TRACE_RETURN(result);
}

static sony_cxd_result_t EWStoSL(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("EWStoSL");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable Hi-Z setting 1 (TAGC, SAGC, TSVALID, TSSYNC, TSCLK) */
	if (sony_cxd_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x1F, 0x1F) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable Hi-Z setting 2 */
	result = sony_cxd_demod_SetTSDataPinHiZ(pDemod, 1);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Cancel demod parameter */
    result = AITtoSL_CommonSetting (pDemod);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-X Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x49, 0x33) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, 0x21) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Demodulator SW reset */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFE, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable demodulator clock */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set tstlv mode to default */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable EWS mode */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x35, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set demodulator mode to default */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	sony_cxd_TRACE_RETURN(result);
}

static sony_cxd_result_t SLtoAIT_BandSetting(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("SLtoAIT_BandSetting");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x10 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    switch (pDemod->bandwidth) {
	case sony_cxd_DTV_BW_8_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x11, 0xB8, 0x00, 0x00, 0x00
            };

            /* <Timing Recovery setting>
             * Slave     Bank    Addr   Bit       Default    Value      Name
             * -----------------------------------------------------------------------
             * <SLV-T>   10h     9Fh    [5:0]     8'h15      8'h11      OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   10h     A0h    [7:0]     8'h00      8'hB8      OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   10h     A1h    [7:0]     8'h00      8'h00      OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   10h     A2h    [7:0]     8'h00      8'h00      OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   10h     A3h    [7:0]     8'h00      8'h00      OREG_TRCG_NOMINALRATE[7:0]
             */

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

         /*
          *  Filter settings for Sony silicon tuners
          *    slave    Bank    Addr    Bit    default     Value          Name
          *   ---------------------------------------------------------------------------------
          *   <SLV-T>   10h     A6h    [7:0]     8'h1E      8'h2F      OREG_ITB_COEF01[7:0]
          *   <SLV-T>   10h     A7h    [7:0]     8'h1D      8'hBA      OREG_ITB_COEF02[7:0]
          *   <SLV-T>   10h     A8h    [7:0]     8'h29      8'h28      OREG_ITB_COEF11[7:0]
          *   <SLV-T>   10h     A9h    [7:0]     8'hC9      8'h9B      OREG_ITB_COEF12[7:0]
          *   <SLV-T>   10h     AAh    [7:0]     8'h2A      8'h28      OREG_ITB_COEF21[7:0]
          *   <SLV-T>   10h     ABh    [7:0]     8'hBA      8'h9D      OREG_ITB_COEF22[7:0]
          *   <SLV-T>   10h     ACh    [7:0]     8'h29      8'h28      OREG_ITB_COEF31[7:0]
          *   <SLV-T>   10h     ADh    [7:0]     8'hAD      8'hA1      OREG_ITB_COEF32[7:0]
          *   <SLV-T>   10h     AEh    [7:0]     8'h29      8'h29      OREG_ITB_COEF41[7:0]
          *   <SLV-T>   10h     AFh    [7:0]     8'hA4      8'hA5      OREG_ITB_COEF42[7:0]
          *   <SLV-T>   10h     B0h    [7:0]     8'h29      8'h2A      OREG_ITB_COEF51[7:0]
          *   <SLV-T>   10h     B1h    [7:0]     8'h9A      8'hAC      OREG_ITB_COEF52[7:0]
          *   <SLV-T>   10h     B2h    [7:0]     8'h28      8'h29      OREG_ITB_COEF61[7:0]
          *   <SLV-T>   10h     B3h    [7:0]     8'h9E      8'hB5      OREG_ITB_COEF62[7:0]
          */
		if (pDemod->tunerOptimize == sony_cxd_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
            const uint8_t itbCoef[14] = {
            /*  COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62 */
                0x2F,  0xBA,  0x28,  0x9B,  0x28,  0x9D,  0x28,  0xA1,  0x29,  0xA5,  0x2A,  0xAC,  0x29,  0xB5
            };

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, itbCoef, 14) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

        {
            /*
             * <IF freq setting>
             * slave    bank    addr    bit     default     value           name
             * ---------------------------------------------------------------------------------
             * <SLV-T>   10h     B6h    [7:0]     8'h1F     user defined    OREG_DNCNV_LOFRQ_T[23:16]
             * <SLV-T>   10h     B7h    [7:0]     8'h38     user defined    OREG_DNCNV_LOFRQ_T[15:8]
             * <SLV-T>   10h     B8h    [7:0]     8'h32     user defined    OREG_DNCNV_LOFRQ_T[7:0]
             */
            uint8_t data[3];
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configISDBT_8 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configISDBT_8 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configISDBT_8 & 0xFF);
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]     8'h00      8'h00      OREG_CHANNEL_WIDTH[2:0]
         */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x00) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

        {
            const uint8_t data[] = { 0x13, 0xFC };
            /* Demod core latency setting
             * slave    Bank    Addr    Bit      default    Name
             * --------------------------------------------------------------------
             * <SLV-T>   10h     D9h    [5:0]     8'h15      8'h13      OREG_CDRB_GTDOFST[13:8]
             * <SLV-T>   10h     DAh    [7:0]     8'h28      8'hFC      OREG_CDRB_GTDOFST[7:0]
             */
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD9, data, sizeof (data)) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* Acquisition optimization setting
         *  slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   12h     71h    [2:0]     8'h07      8'h03      OREG_SYR_ACTMODE[2:0]
         * <SLV-T>   15h     BEh    [7:0]     8'h02      8'h03      OREG_FCS_SYMBOL_INTERVAL_ISDBT[7:0]
         */

        /* Set SLV-T Bank : 0x12 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x12) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x71, 0x03) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

        /* Set SLV-T Bank : 0x15 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x15) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xBE, 0x03) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

        break;

    case sony_cxd_DTV_BW_7_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x14, 0x40, 0x00, 0x00, 0x00
            };

            /* <Timing Recovery setting>
             *    Slave    Bank    Addr   Bit      Default     Value      Name
             * -----------------------------------------------------------------------
             *   <SLV-T>   10h     9Fh    [5:0]     8'h15      8'h14      OREG_TRCG_NOMINALRATE[37:32]
             *   <SLV-T>   10h     A0h    [7:0]     8'h00      8'h40      OREG_TRCG_NOMINALRATE[31:24]
             *   <SLV-T>   10h     A1h    [7:0]     8'h00      8'h00      OREG_TRCG_NOMINALRATE[23:16]
             *   <SLV-T>   10h     A2h    [7:0]     8'h00      8'h00      OREG_TRCG_NOMINALRATE[15:8]
             *   <SLV-T>   10h     A3h    [7:0]     8'h00      8'h00      OREG_TRCG_NOMINALRATE[7:0]
             */

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

        /*
         *  Filter settings for Sony silicon tuners
         *    slave    Bank    Addr    Bit    default   Value          Name
         *   ---------------------------------------------------------------------------------
         *   <SLV-T>   10h     A6h    [7:0]     8'h1E      8'h30      OREG_ITB_COEF01[7:0]
         *   <SLV-T>   10h     A7h    [7:0]     8'h1D      8'hB1      OREG_ITB_COEF02[7:0]
         *   <SLV-T>   10h     A8h    [7:0]     8'h29      8'h29      OREG_ITB_COEF11[7:0]
         *   <SLV-T>   10h     A9h    [7:0]     8'hC9      8'h9A      OREG_ITB_COEF12[7:0]
         *   <SLV-T>   10h     AAh    [7:0]     8'h2A      8'h28      OREG_ITB_COEF21[7:0]
         *   <SLV-T>   10h     ABh    [7:0]     8'hBA      8'h9C      OREG_ITB_COEF22[7:0]
         *   <SLV-T>   10h     ACh    [7:0]     8'h29      8'h28      OREG_ITB_COEF31[7:0]
         *   <SLV-T>   10h     ADh    [7:0]     8'hAD      8'hA0      OREG_ITB_COEF32[7:0]
         *   <SLV-T>   10h     AEh    [7:0]     8'h29      8'h29      OREG_ITB_COEF41[7:0]
         *   <SLV-T>   10h     AFh    [7:0]     8'hA4      8'hA2      OREG_ITB_COEF42[7:0]
         *   <SLV-T>   10h     B0h    [7:0]     8'h29      8'h2B      OREG_ITB_COEF51[7:0]
         *   <SLV-T>   10h     B1h    [7:0]     8'h9A      8'hA6      OREG_ITB_COEF52[7:0]
         *   <SLV-T>   10h     B2h    [7:0]     8'h28      8'h2B      OREG_ITB_COEF61[7:0]
         *   <SLV-T>   10h     B3h    [7:0]     8'h9E      8'hAD      OREG_ITB_COEF62[7:0]
         */
		if (pDemod->tunerOptimize == sony_cxd_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
            const uint8_t itbCoef[14] = {
            /*  COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62 */
                0x30,  0xB1,  0x29,  0x9A,  0x28,  0x9C,  0x28,  0xA0,  0x29,  0xA2,  0x2B,  0xA6,  0x2B,  0xAD
            };

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, itbCoef, 14) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

        {
            /*
             * <IF freq setting>
             * slave    bank    addr    bit     default     value           name
             * ---------------------------------------------------------------------------------
             * <SLV-T>   10h     B6h    [7:0]     8'h1F     user defined    OREG_DNCNV_LOFRQ_T[23:16]
             * <SLV-T>   10h     B7h    [7:0]     8'h38     user defined    OREG_DNCNV_LOFRQ_T[15:8]
             * <SLV-T>   10h     B8h    [7:0]     8'h32     user defined    OREG_DNCNV_LOFRQ_T[7:0]
             */
            uint8_t data[3];
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configISDBT_7 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configISDBT_7 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configISDBT_7 & 0xFF);
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* System bandwidth setting
         *  slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]     8'h00      8'h02      OREG_CHANNEL_WIDTH[2:0]
         */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x02) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

        {
            const uint8_t data[] = { 0x1A, 0xFA };

            /* Demod core latency setting
             * slave    Bank    Addr    Bit      default     value      Name
             * --------------------------------------------------------------------
             * <SLV-T>   10h     D9h    [5:0]     8'h15      8'h1A      OREG_CDRB_GTDOFST[13:8]
             * <SLV-T>   10h     DAh    [7:0]     8'h28      8'hFA      OREG_CDRB_GTDOFST[7:0]
             */
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD9, data, sizeof (data)) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* Acquisition optimization setting
         *  slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   12h     71h    [2:0]     8'h07      8'h03      OREG_SYR_ACTMODE[2:0]
         * <SLV-T>   15h     BEh    [7:0]     8'h02      8'h02      OREG_FCS_SYMBOL_INTERVAL_ISDBT[7:0]
         */
        /* Set SLV-T Bank : 0x12 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x12) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x71, 0x03) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

        /* Set SLV-T Bank : 0x15 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x15) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xBE, 0x02) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

        break;

    case sony_cxd_DTV_BW_6_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x17, 0xA0, 0x00, 0x00, 0x00
            };
            /* <Timing Recovery setting>
             * Slave     Bank    Addr   Bit     Default   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   10h     9Fh    [5:0]   8'h15     OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   10h     A0h    [7:0]   8'h00     OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   10h     A1h    [7:0]   8'h00     OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   10h     A2h    [7:0]   8'h00     OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   10h     A3h    [7:0]   8'h00     OREG_TRCG_NOMINALRATE[7:0]
             */

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

        /*
         *  Filter settings for Sony silicon tuners
         *   slave    Bank    Addr    Bit    default       Name
         *   --------------------------------------------------------------------------------
         *   <SLV-T>   10h     A6h    [7:0]     8'h1E      OREG_ITB_COEF01[7:0]
         *   <SLV-T>   10h     A7h    [7:0]     8'h1D      OREG_ITB_COEF02[7:0]
         *   <SLV-T>   10h     A8h    [7:0]     8'h29      OREG_ITB_COEF11[7:0]
         *   <SLV-T>   10h     A9h    [7:0]     8'hC9      OREG_ITB_COEF12[7:0]
         *   <SLV-T>   10h     AAh    [7:0]     8'h2A      OREG_ITB_COEF21[7:0]
         *   <SLV-T>   10h     ABh    [7:0]     8'hBA      OREG_ITB_COEF22[7:0]
         *   <SLV-T>   10h     ACh    [7:0]     8'h29      OREG_ITB_COEF31[7:0]
         *   <SLV-T>   10h     ADh    [7:0]     8'hAD      OREG_ITB_COEF32[7:0]
         *   <SLV-T>   10h     AEh    [7:0]     8'h29      OREG_ITB_COEF41[7:0]
         *   <SLV-T>   10h     AFh    [7:0]     8'hA4      OREG_ITB_COEF42[7:0]
         *   <SLV-T>   10h     B0h    [7:0]     8'h29      OREG_ITB_COEF51[7:0]
         *   <SLV-T>   10h     B1h    [7:0]     8'h9A      OREG_ITB_COEF52[7:0]
         *   <SLV-T>   10h     B2h    [7:0]     8'h28      OREG_ITB_COEF61[7:0]
         *   <SLV-T>   10h     B3h    [7:0]     8'h9E      OREG_ITB_COEF62[7:0]
         */
		if (pDemod->tunerOptimize == sony_cxd_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
            const uint8_t itbCoef[14] = {
            /*  COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62 */
                0x31,  0xA8,  0x29,  0x9B,  0x27,  0x9C,  0x28,  0x9E,  0x29,  0xA4,  0x29,  0xA2,  0x29,  0xA8
            };
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, itbCoef, 14) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

        {
            /*
             * <IF freq setting>
             * slave    bank    addr    bit     default     value           name
             * ---------------------------------------------------------------------------------
             * <SLV-T>   10h     B6h    [7:0]     8'h1F     user defined    OREG_DNCNV_LOFRQ_T[23:16]
             * <SLV-T>   10h     B7h    [7:0]     8'h38     user defined    OREG_DNCNV_LOFRQ_T[15:8]
             * <SLV-T>   10h     B8h    [7:0]     8'h32     user defined    OREG_DNCNV_LOFRQ_T[7:0]
             */
            uint8_t data[3];
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configISDBT_6 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configISDBT_6 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configISDBT_6 & 0xFF);
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]     8'h00      8'h04      OREG_CHANNEL_WIDTH[2:0]
         */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x04) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

        {
            const uint8_t data[] = { 0x1F, 0x79 };

            /* Demod core latency setting
             * slave    Bank    Addr    Bit      default     value      Name
             * --------------------------------------------------------------------
             * <SLV-T>   10h     D9h    [5:0]     8'h15      8'h1F      OREG_CDRB_GTDOFST[13:8]
             * <SLV-T>   10h     DAh    [7:0]     8'h28      8'h79      OREG_CDRB_GTDOFST[7:0]
             */
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD9, data, sizeof (data)) != sony_cxd_RESULT_OK) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* Acquisition optimization setting
         *  slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   12h     71h    [2:0]     8'h07      8'h07      OREG_SYR_ACTMODE[2:0]
         * <SLV-T>   15h     BEh    [7:0]     8'h02      8'h02      OREG_FCS_SYMBOL_INTERVAL_ISDBT[7:0]
         */
        /* Set SLV-T Bank : 0x12 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x12) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x71, 0x07) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

        /* Set SLV-T Bank : 0x15 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x15) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xBE, 0x02) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
        break;

    default:
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	sony_cxd_TRACE_RETURN(result);
}

static sony_cxd_result_t SLtoAIT_CommonSetting(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("SLtoAIT_CommonSetting");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x11 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* BBAGC TARGET level setting
     * slave    Bank    Addr    Bit    default     Value          Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   11h     6Ah    [7:0]     8'h50      8'h50      OREG_ITB_DAGC_TRGT[7:0]
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6A, 0x50) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x10 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* ASCOT setting */
	if (pDemod->tunerOptimize == sony_cxd_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
        /* ASCOT setting ON
         * slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     A5h     [0]      8'h01      8'h00      OREG_ITB_GDEQ_EN
         */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x01) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
    }
    else {
        /* ASCOT setting OFF
         * slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     A5h     [0]      8'h01      8'h00      OREG_ITB_GDEQ_EN
         */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* TSIF setting (TS packet gap insertion off)
     *   slave    Bank    Addr    Bit    default   Value          Name
     *  ---------------------------------------------------------------------------------
     *   <SLV-T>   00h     CEh     [0]      8'h01      8'h00      ONOPARITY
     *   <SLV-T>   00h     CFh     [0]      8'h01      8'h00      ONOPARITY_MANUAL_ON
     */
    {
        const uint8_t data[] = { 0x00, 0x00 };

		if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xCE, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* ISDB-T initial setting */

    /* Set SLV-T Bank : 0x10 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }
    /*   slave    Bank    Addr    Bit    default   Value          Name
     *  ---------------------------------------------------------------------------------
     *   <SLV-T>   10h     69h    [2:0]     8'h05      8'h04      OREG_ENOD_DETSEL[2:0]
     *   <SLV-T>   10h     6Bh    [2:0]     8'h07      8'h03      OREG_COSNE_CRANGE[2:0]
     *   <SLV-T>   10h     9Dh    [7:0]     8'h14      8'h50      OREG_MER_ASNOS[7:0]
     *   <SLV-T>   10h     D3h    [4:0]     8'h00      8'h06      OREG_FORCE_MODEGI,OREG_MODE[1:0],OREG_GI[1:0]
     *   <SLV-T>   10h     EDh     [0]      8'h01      8'h00      OREG_SEQ_SNRSEL
     *   <SLV-T>   10h     E2h     [7]      8'h4E      8'hCE      OREG_PNC_DISABLE
     *   <SLV-T>   10h     F2h     [4]      8'h03      8'h13      OREG_TITP_NEF_DISABLE
     *   <SLV-T>   10h     DEh    [5:0]     8'h32      8'h2E      OREG_FCS_NTHETA[5:0]
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x69, 0x04) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6B, 0x03) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9D, 0x50) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD3, 0x06) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xED, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE2, 0xCE) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF2, 0x13) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xDE, 0x2E) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /*   slave    Bank    Addr    Bit    default   Value          Name
     *  ---------------------------------------------------------------------------------
     *   <SLV-T>   15h     DEh    [1:0]     8'h03      8'h02      OREG_FCS_F0COEFSEL2[1:0]
     *   <SLV-T>   17h     38h    [1:0]     8'h01      8'h00      OREG_CAS_CCIFLT2_EN_CW2[1:0]
     *   <SLV-T>   17h     39h    [1:0]     8'h02      8'h03      OREG_CAS_CWSEQ_ON1, OREG_CAS_CWSEQ_ON2
     *   <SLV-T>   1Eh     73h    [7:0]     8'h00      8'h68      OREG_DAGC_CLIP_BW[2:0],OREG_DAGC_NOSIG_BW[2:0],OREG_DAGC_REDUCE_COARSEGAIN[1:0]
     *   <SLV-T>   63h     81h     [0]      8'h01      8'h00      OREG_NPC_FORCE_MOBILE_MODE_EN
     */
    /* Set SLV-T Bank : 0x15 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x15) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xDE, 0x02) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x17 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x17) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    {
        const uint8_t data[] = { 0x00, 0x03 };

		if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x38, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* Set SLV-T Bank : 0x1E */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x1E) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x73, 0x68) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x63 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x63) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x81, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x11 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    {
        /* ISDB-T 24MHz Xtal setting
         * Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   11h     33h    [7:0]     8'hC8      8'h00      OCTL_IFAGC_INITWAIT[7:0]
         * <SLV-T>   11h     34h    [7:0]     8'h02      8'h03      OCTL_IFAGC_MINWAIT[7:0]
         * <SLV-T>   11h     35h    [7:0]     8'h32      8'h3B      OCTL_IFAGC_MAXWAIT[7:0]
         */
        const uint8_t data[3] = { 0x00, 0x03, 0x3B };
		if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x33, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* Set SLV-T Bank : 0x60 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    {
        const uint8_t data[] = {0xB7, 0x1B};

        /* ISDB-T 24MHz Xtal setting
         * Slave     Bank    Addr    Bit      Default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   60h     A8h    [7:0]     8'h9C      8'hB7      OPEC_MAXCLKCNT[23:16]
         * <SLV-T>   60h     A9h    [7:0]     8'h67      8'h1B      OPEC_MAXCLKCNT[15:8]
         */
		if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA8, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
    }

	sony_cxd_TRACE_RETURN(result);
}

static sony_cxd_result_t AITtoSL_CommonSetting(sony_cxd_demod_t * pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("AITtoSL_CommonSetting");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x10 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

   /*   slave    Bank    Addr    Bit    default   Value          Name
    *  ---------------------------------------------------------------------------------
    *   <SLV-T>   10h     69h    [2:0]     8'h05      8'h05      OREG_ENOD_DETSEL[2:0]
    *   <SLV-T>   10h     6Bh    [2:0]     8'h07      8'h07      OREG_COSNE_CRANGE[2:0]
    *   <SLV-T>   10h     9Dh    [7:0]     8'h14      8'h14      OREG_MER_ASNOS[7:0]
    *   <SLV-T>   10h     D3h    [4:0]     8'h00      8'h00      OREG_FORCE_MODEGI,OREG_MODE[1:0],OREG_GI[1:0]
    *   <SLV-T>   10h     EDh     [0]      8'h01      8'h01      OREG_SEQ_SNRSEL
    *   <SLV-T>   10h     E2h     [7]      8'h4E      8'h4E      OREG_PNC_DISABLE
    *   <SLV-T>   10h     F2h     [4]      8'h03      8'h03      OREG_TITP_NEF_DISABLE
    *   <SLV-T>   10h     DEh    [5:0]     8'h32      8'h32      OREG_FCS_NTHETA[5:0]
    */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x69, 0x05) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6B, 0x07) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9D, 0x14) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD3, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xED, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE2, 0x4E) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF2, 0x03) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xDE, 0x32) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

   /*   slave    Bank    Addr    Bit    default   Value          Name
    *  ---------------------------------------------------------------------------------
    *   <SLV-T>   15h     DEh    [1:0]     8'h03      8'h03      OREG_FCS_F0COEFSEL2[1:0]
    *   <SLV-T>   17h     38h    [1:0]     8'h01      8'h01      OREG_CAS_CCIFLT2_EN_CW2[1:0]
    *   <SLV-T>   17h     39h    [1:0]     8'h02      8'h02      OREG_CAS_CWSEQ_ON1, OREG_CAS_CWSEQ_ON2
    *   <SLV-T>   1Eh     73h    [7:0]     8'h00      8'h00      OREG_DAGC_CLIP_BW[2:0],OREG_DAGC_NOSIG_BW[2:0],OREG_DAGC_REDUCE_COARSEGAIN[1:0]
    *   <SLV-T>   63h     81h     [0]      8'h01      8'h01      OREG_NPC_FORCE_MOBILE_MODE_EN
    */
    /* Set SLV-T Bank : 0x15 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x15) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xDE, 0x03) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x17 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x17) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    {
        const uint8_t data[] = { 0x01, 0x02 };

		if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x38, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* Set SLV-T Bank : 0x1E */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x1E) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x73, 0x00) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x63 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x63) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x81, 0x01) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

	sony_cxd_TRACE_RETURN(result);
}
