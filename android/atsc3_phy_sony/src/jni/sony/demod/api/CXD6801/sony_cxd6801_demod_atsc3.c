/*------------------------------------------------------------------------------
  Copyright 2018-2019 Sony Semiconductor Solutions Corporation

  Last Updated    : 2019/04/23
  Modification ID : 9999462af7876667bd516e7137bc6cd1ac791eee
------------------------------------------------------------------------------*/

#include "sony_cxd6801_demod_atsc3.h"
#include "sony_cxd6801_demod_atsc3_monitor.h"

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Configure the demodulator from Sleep to Active for ATSC 3.0.
*/
static sony_cxd6801_result_t SLtoAA3(sony_cxd6801_demod_t * pDemod);

/**
 @brief Configure the demodulator for tuner optimisations and bandwidth specific
        settings.
*/
static sony_cxd6801_result_t SLtoAA3_BandSetting(sony_cxd6801_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active ATSC 3.0 to Active ATSC 3.0.

        Used for changing channel parameters.
*/
static sony_cxd6801_result_t AA3toAA3(sony_cxd6801_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active ATSC 3.0 to Sleep.
*/
static sony_cxd6801_result_t AA3toSL(sony_cxd6801_demod_t * pDemod);

/**
 @brief Spectrum inversion setting
*/
static sony_cxd6801_result_t invertSpectrum(sony_cxd6801_demod_t * pDemod);

/**
 @brief Automatic Continuous Wave detection
*/
static sony_cxd6801_result_t detectCW(sony_cxd6801_demod_t * pDemod);

/**
 @brief Initialize Continuous Wave Detection
*/
static sony_cxd6801_result_t initCWDetection(sony_cxd6801_demod_t * pDemod);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_Tune(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_tune_param_t * pTuneParam)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_Tune");

    if ((!pDemod) || (!pTuneParam)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pDemod->state == SONY_CXD6801_DEMOD_STATE_ACTIVE) {
		if ((pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) || (pDemod->atsc3EasState != SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_NORMAL)) {
            /* Demodulator Active but not ATSC 3.0 mode or EAS mode */
			result = sony_cxd6801_demod_Sleep(pDemod);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }

			pDemod->system = SONY_CXD6801_DTV_SYSTEM_ATSC3;
            pDemod->bandwidth = pTuneParam->bandwidth;
			pDemod->atsc3EasState = SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_NORMAL;
            pDemod->atsc3AutoSpectrumInv_flag = 0;
            pDemod->atsc3CWDetection_flag = 0;

            result = SLtoAA3 (pDemod);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        } else {
            /* Demodulator Active and set to ATSC 3.0 mode */
            pDemod->bandwidth = pTuneParam->bandwidth;
            pDemod->atsc3AutoSpectrumInv_flag = 0;
            pDemod->atsc3CWDetection_flag = 0;

            result = AA3toAA3(pDemod);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }
	}
	else if (pDemod->state == SONY_CXD6801_DEMOD_STATE_SLEEP) {
        /* Demodulator in Sleep mode */
		pDemod->system = SONY_CXD6801_DTV_SYSTEM_ATSC3;
        pDemod->bandwidth = pTuneParam->bandwidth;
		pDemod->atsc3EasState = SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_NORMAL;
        pDemod->atsc3AutoSpectrumInv_flag = 0;
        pDemod->atsc3CWDetection_flag = 0;

        result = SLtoAA3 (pDemod);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Update demodulator state */
	pDemod->state = SONY_CXD6801_DEMOD_STATE_ACTIVE;

    SONY_CXD6801_TRACE_RETURN (result);

}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_EASTune(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_tune_param_t * pTuneParam)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_EASTune");

    if ((!pDemod) || (!pTuneParam)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pDemod->state == SONY_CXD6801_DEMOD_STATE_ACTIVE) {
		if ((pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) || (pDemod->atsc3EasState != SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_EAS)) {
            /* Demodulator Active but not ATSC 3.0 or non EAS mode */
			result = sony_cxd6801_demod_Sleep(pDemod);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }

			pDemod->system = SONY_CXD6801_DTV_SYSTEM_ATSC3;
            pDemod->bandwidth = pTuneParam->bandwidth;
			pDemod->atsc3EasState = SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_EAS;
            pDemod->atsc3AutoSpectrumInv_flag = 0;
            pDemod->atsc3CWDetection_flag = 0;

            result = SLtoAA3 (pDemod);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        } else {
            /* Demodulator Active and set to ATSC 3.0 EAS mode */
            pDemod->bandwidth = pTuneParam->bandwidth;
            pDemod->atsc3AutoSpectrumInv_flag = 0;
            pDemod->atsc3CWDetection_flag = 0;
            result = AA3toAA3(pDemod);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        }
	}
	else if (pDemod->state == SONY_CXD6801_DEMOD_STATE_SLEEP) {
        /* Demodulator in Sleep mode */
		pDemod->system = SONY_CXD6801_DTV_SYSTEM_ATSC3;
        pDemod->bandwidth = pTuneParam->bandwidth;
		pDemod->atsc3EasState = SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_EAS;
        pDemod->atsc3AutoSpectrumInv_flag = 0;
        pDemod->atsc3CWDetection_flag = 0;

        result = SLtoAA3 (pDemod);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Update demodulator state */
	pDemod->state = SONY_CXD6801_DEMOD_STATE_ACTIVE;

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_Sleep(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_Sleep");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /*This api is accepted in Sleep and Active states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    result = AA3toSL (pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_EASTuneEnd(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_demod_atsc3_EASTuneEnd");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) ||
		(pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) ||
		(pDemod->atsc3EasState != SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_EAS)){
        /* This api is accepted in ATSC 3.0 EAS state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	result = sony_cxd6801_demod_SoftReset(pDemod);

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_CheckDemodLock(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_lock_result_t * pLock)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    uint8_t sync = 0;
    uint8_t alpLockStat[4] = {0};
    uint8_t alpLockAll = 0;
    uint8_t unlockDetected = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_demod_atsc3_CheckDemodLock");

    if (!pDemod || !pLock) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	*pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_NOTDETECT;

	result = sony_cxd6801_demod_atsc3_monitor_SyncStat(pDemod, &sync, alpLockStat, &alpLockAll, &unlockDetected);

	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    if (unlockDetected) {
		*pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_UNLOCKED;
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
    }

    if (sync >= 6) {
		*pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_LOCKED;
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_CheckALPLock(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_lock_result_t * pLock)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    uint8_t sync = 0;
    uint8_t alpLockStat[4] = {0};
    uint8_t alpLockAll = 0;
    uint8_t unlockDetected = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_demod_atsc3_CheckALPLock");

    if (!pDemod || !pLock) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	*pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_NOTDETECT;

	result = sony_cxd6801_demod_atsc3_monitor_SyncStat(pDemod, &sync, alpLockStat, &alpLockAll, &unlockDetected);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    if (unlockDetected) {
		*pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_UNLOCKED;
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
    }

    if ((sync >= 6) && alpLockAll) {
		*pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_LOCKED;
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
    }

    if (sync >= 6) {
        uint8_t plpValid[4];

		result = sony_cxd6801_demod_atsc3_monitor_SelectedPLPValid(pDemod, plpValid);
		if (result == SONY_CXD6801_RESULT_ERROR_HW_STATE) {
            /* PLP valid information is not ready. */
			*pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_NOTDETECT;
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
		}
		else if (result != SONY_CXD6801_RESULT_OK) {
            /* Serious error */
            SONY_CXD6801_TRACE_RETURN (result);
        }

        if (!plpValid[0]) {
            /* All PLP IDs are invalid */
			*pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_UNLOCKED;
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_SetPLPConfig(sony_cxd6801_demod_t * pDemod,
                                             uint8_t plpIDNum,
                                             uint8_t plpID[4])
{
    int i = 0;
    uint8_t data[4];
    SONY_CXD6801_TRACE_ENTER ("sony_demod_atsc3_SetPLPConfig");

    if ((!pDemod) || (!plpID)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* plpIDNum range check */
    if ((plpIDNum == 0) || (plpIDNum > 4)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* PLP ID range check */
    for (i = 0; i < plpIDNum; i++) {
        if (plpID[i] > 0x3F) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
        }
    }

    /* Set SLV-T Bank : 0x93 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr   Bit    Default   Value          Name
     * ----------------------------------------------------------------------------------
     *  <SLV-T>   93h     80h    [7]    1'b1      1'bx           OREGD_PLP_ID_0_VALID
     *  <SLV-T>   93h     80h    [5:0]  6'd00     6'dxx          OREGD_PLP_ID_0[5:0]
     *  <SLV-T>   93h     81h    [7]    1'b0      1'bx           OREGD_PLP_ID_1_VALID
     *  <SLV-T>   93h     81h    [5:0]  6'd00     6'dxx          OREGD_PLP_ID_1[5:0]
     *  <SLV-T>   93h     82h    [7]    1'b0      1'bx           OREGD_PLP_ID_2_VALID
     *  <SLV-T>   93h     82h    [5:0]  6'd00     6'dxx          OREGD_PLP_ID_2[5:0]
     *  <SLV-T>   93h     83h    [7]    1'b0      1'bx           OREGD_PLP_ID_3_VALID
     *  <SLV-T>   93h     83h    [5:0]  6'd00     6'dxx          OREGD_PLP_ID_3[5:0]
     */
    for (i = 0; i < 4; i++) {
        if (i < plpIDNum) {
            data[i] = plpID[i] | 0x80;
        } else {
            data[i] = 0;
        }
    }
	if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Make IPLPINFO_RDY low for changing PLP without re-tuning case
     *
     *  slave     Bank     Addr     Bit    default   Value          Name
     * ----------------------------------------------------------------------------------
     *  <SLV-T>   93h      9Ch      [0]    1'b0      1'b1           OREGD_PLPINFO_UPD
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9C, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_ClearBERSNRHistory(sony_cxd6801_demod_t * pDemod)
{
    SONY_CXD6801_TRACE_ENTER ("sony_demod_atsc3_ClearBERSNRHistory");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x02 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr    Bit    Name                  Meaning
     * -----------------------------------------------------------------------
     *  <SLV-T>   02h     52h     [0]    OREG_XMON_USER_CLEAR  1:Clear
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x52, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_ClearGPIOEASLatch(sony_cxd6801_demod_t * pDemod)
{
    SONY_CXD6801_TRACE_ENTER ("sony_demod_atsc3_ClearGPIOEASLatch");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x99 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x99) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr    Bit    Name                  Meaning
     * -----------------------------------------------------------------------
     *  <SLV-T>   99h     AFh     [0]    OREG_EAS_LATCH_CLR    1:Clear
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xAF, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_AutoDetectSeq_UnlockCase(sony_cxd6801_demod_t * pDemod, uint8_t * pContinueWait)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_cxd6801_atsc3_bootstrap_t bootstrap;
    uint8_t data[6];
    uint32_t sinvdetCorrMax;
    uint32_t sinvdetCorrRMax;
    uint8_t maxState;

    SONY_CXD6801_TRACE_ENTER ("sony_demod_atsc3_AutoDetectSeq_UnlockCase");

    if ((!pDemod) || (!pContinueWait)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	result = sony_cxd6801_demod_atsc3_monitor_Bootstrap(pDemod, &bootstrap);
	if (result == SONY_CXD6801_RESULT_OK) {
        if (bootstrap.bw_diff) {
            /* Specified bandwidth is not same as input signal */
            *pContinueWait = 0;
            SONY_CXD6801_TRACE_RETURN (result);
        }
	}
	else if (result != SONY_CXD6801_RESULT_ERROR_HW_STATE) {
        /* If bootstrap information is not available (ERROR_HW_STATE), continue next check */
        SONY_CXD6801_TRACE_RETURN (result);
    }

    if (((!pDemod->atsc3AutoSpectrumInv) || (pDemod->atsc3AutoSpectrumInv_flag))
        && ((!pDemod->atsc3CWDetection) || (pDemod->atsc3CWDetection_flag))) {
        *pContinueWait = 0;
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
    }

    SONY_CXD6801_SLEEP (11);

    /* Freeze registers */
	if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*
    *  slave     Bank    Addr   Bit     Name
    * ---------------------------------------------------------------------------------
    *  <SLV-T>   98h     6Fh    [7:0]   IREG_SINVDET_CORR_MAX[23:16]
    *  <SLV-T>   98h     70h    [7:0]   IREG_SINVDET_CORR_MAX[15:8]
    *  <SLV-T>   98h     71h    [7:0]   IREG_SINVDET_CORR_MAX[7:0]
    *  <SLV-T>   98h     72h    [7:0]   IREG_SINVDET_CORR_R_MAX[23:16]
    *  <SLV-T>   98h     73h    [7:0]   IREG_SINVDET_CORR_R_MAX[15:8]
    *  <SLV-T>   98h     74h    [7:0]   IREG_SINVDET_CORR_R_MAX[7:0]
    *  <SLV-T>   90h     17h    [2:0]   SEQ_MAX_STATE[2:0]
    */
    /* Set SLV-T Bank : 0x98 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x98) != SONY_CXD6801_RESULT_OK) {
		CXD6801_SLVT_UnFreezeReg(pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6F, data, 6) != SONY_CXD6801_RESULT_OK) {
		CXD6801_SLVT_UnFreezeReg(pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    sinvdetCorrMax = (data[0] << 16) | (data[1] << 8) | data[2];
    sinvdetCorrRMax = (data[3] << 16) | (data[4] << 8) | data[5];

    /* Set SLV-T Bank : 0x90 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
		CXD6801_SLVT_UnFreezeReg(pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x17, data, 1) != SONY_CXD6801_RESULT_OK) {
		CXD6801_SLVT_UnFreezeReg(pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    maxState = data[0] & 0x07;

	CXD6801_SLVT_UnFreezeReg(pDemod);

    if ((pDemod->atsc3AutoSpectrumInv) && (!pDemod->atsc3AutoSpectrumInv_flag)) {
        if ((sinvdetCorrRMax > 70000) && (sinvdetCorrRMax > (((sinvdetCorrMax * 3) + 1) / 2)) && (maxState < 3)) {
            result = invertSpectrum (pDemod);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
            *pContinueWait = 1;
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    if ((pDemod->atsc3CWDetection) && (!pDemod->atsc3CWDetection_flag)) {
        result = detectCW (pDemod);
		if (result == SONY_CXD6801_RESULT_OK) {
            if (pDemod->atsc3CWDetection_flag) {
                /* CW is detected. Soft reset and continue waiting lock. */
                *pContinueWait = 1;
                SONY_CXD6801_TRACE_RETURN (result);
            }
            /* CW is not detected. Do next check. */
        } else {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    if ((pDemod->atsc3AutoSpectrumInv) && (!pDemod->atsc3AutoSpectrumInv_flag)) {
        if ((sinvdetCorrRMax > 70000) && (sinvdetCorrRMax > sinvdetCorrMax) && (maxState < 3)) {
            result = invertSpectrum (pDemod);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
            *pContinueWait = 1;
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    *pContinueWait = 0;

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_AutoDetectSeq_SetCWTracking(sony_cxd6801_demod_t * pDemod, uint8_t * pCompleted)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint8_t rdata[10];
    uint8_t data[3];
    uint32_t totalPwr;
    uint32_t maxCPwr;
    uint32_t maxCNum;
    uint32_t carrierDuration;
    uint32_t samplingFreq;
    int32_t freqCw;

    SONY_CXD6801_TRACE_ENTER ("sony_demod_atsc3_AutoDetectSeq_SetCWTracking");

    if ((!pDemod) || (!pCompleted)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (!pDemod->atsc3CWDetection_flag) {
        *pCompleted = 1;
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
    }

    /* Set SLV-T Bank : 0x9A */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9A) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr    Bit       Name
     * ----------------------------------------------------------------------------
     *  <SLV-T>   9Ah     46h     [0]       IREG_CWD_ACT
     *  <SLV-T>   9Ah     47h     [7:0]     IREG_CWD_TOTALPWR[31:24]
     *  <SLV-T>   9Ah     48h     [7:0]     IREG_CWD_TOTALPWR[23:16]
     *  <SLV-T>   9Ah     49h     [7:0]     IREG_CWD_TOTALPWR[15:8]
     *  <SLV-T>   9Ah     4Ah     [7:0]     IREG_CWD_TOTALPWR[7:0]
     *  <SLV-T>   9Ah     4Bh     [0]       IREG_CWD_MAXCPWR[16]
     *  <SLV-T>   9Ah     4Ch     [7:0]     IREG_CWD_MAXCPWR[15:8]
     *  <SLV-T>   9Ah     4Dh     [7:0]     IREG_CWD_MAXCPWR[7:0]
     *  <SLV-T>   9Ah     4Eh     [6:0]     IREG_CWD_MAXCNUM[14:8]
     *  <SLV-T>   9Ah     4Fh     [7:0]     IREG_CWD_MAXCNUM[7:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x46, rdata, 10) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (!(rdata[0] & 0x01)) {
        *pCompleted = 0;
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
    }

    totalPwr = (rdata[1] << 24) | (rdata[2] << 16) | (rdata[3] << 8) | rdata[4];
    maxCPwr = ((rdata[5] & 0x01) << 16) | (rdata[6] << 8) | rdata[7];
    maxCNum = ((rdata[8] & 0x7F) << 8) | rdata[9];

    if ((maxCPwr <= 10000) || (maxCPwr <= (totalPwr / 200))) {
        *pCompleted = 1;
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
    }

    /* Set SLV-T Bank : 0x90 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* calc CAS_CCIFLT_FREQ_CW
       CAS_CCIFLT_FREQ_CW = (IREG_CWD_MAXCNUM - 16384) * carrier duration / sampling freq before FFT * 2^18
                            (IREG_CWD_MAXCNUM - 16384) * (carrier duration * 2^8) * (2^10 / sapling freq before FFT)
       FFT_SIZE=8k  : carrier duration = 210.9375
                      carrier duration * 2^8 = 54000
       FFT_SIZE=16k : carrier duration = 421.875
                      carrier duration * 2^8 = 108000
       FFT_SIZE=32k : carrier duration = 210.9375
                      carrier duration * 2^8 = 216000
       CHANNEL_WIDTH=0 : sampling freq before fft = 9216000 = 9000 / 2^10
       CHANNEL_WIDTH=2 : sampling freq before fft = 8064000 = 7875 / 2^10
       CHANNEL_WIDTH=4 : sampling freq before fft = 6912000 = 6750 / 2^10

    */
    /*  slave     Bank    Addr    Bit      Name
     * ----------------------------------------------------------------------------
     *  <SLV-T>   90h     5Ch     [7:5]    IREG_OFDM_SBF0_FFTSIZE[2:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5C, rdata, 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    switch (rdata[0] >> 5) {
    case 1:/* 8K */
        carrierDuration = 54000; /* 210.9375 * 2^8 */
        break;
    case 4:/* 16K */
        carrierDuration = 108000; /* 421.875 * 2^8 */
        break;
    case 5:/* 32K */
        carrierDuration = 216000; /* 843.75 * 2^8 */
        break;
    default:
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    switch (pDemod->bandwidth) {
	case SONY_CXD6801_DTV_BW_8_MHZ:
        samplingFreq = 9000;/* 2^10 / 9216000 = 1/9000 */
        break;
	case SONY_CXD6801_DTV_BW_7_MHZ:
        samplingFreq = 7875;/* 2^10 / 8064000 = 1/7875 */
        break;
	case SONY_CXD6801_DTV_BW_6_MHZ:
        samplingFreq = 6750;/* 2^10 / 6912000 = 1/6750 */
        break;
    default:
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    freqCw = (maxCNum - 16384) * carrierDuration / samplingFreq;
    data[0] = (freqCw >> 16) & 0x03;
    data[1] = (freqCw >> 8) & 0xFF;
    data[2] = freqCw & 0xFF;

    /* Set SLV-T Bank : 0x9A */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9A) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr   Bit      Default     Value      Name
     * ------------------------------------------------------------------------------------------
     *  <SLV-T>   9Ah     3Eh    [1:0]    8'h00       8'h**      OREG_CAS_CCIFLT_FREQ_CW[17:16]
     *  <SLV-T>   9Ah     3Fh    [7:0]    8'h00       8'h**      OREG_CAS_CCIFLT_FREQ_CW[15:8]
     *  <SLV-T>   9Ah     40h    [7:0]    8'h00       8'h**      OREG_CAS_CCIFLT_FREQ_CW[7:0]
     */
	if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3E, data, 3) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr   Bit      Default     Value      Name
     * ----------------------------------------------------------------------------
     *  <SLV-T>   9Ah     3Ch    [3:0]    8'h05       8'h04      OREG_CAS_CCIFLT_MU_CW
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3C, 0x04) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x98 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x98) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr   Bit      Default     Value      Name
     * ----------------------------------------------------------------------------
     *  <SLV-T>   98h     C6h    [3:0]    8'h04       8'h0C      OREG_CAS_CCIFLT_EN_CW
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC6, 0x0C) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    *pCompleted = 1;

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_AutoDetectSeq_Init(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result;

    SONY_CXD6801_TRACE_ENTER ("sony_demod_atsc3_AutoDetectSeq_Init");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /*This api is accepted in Sleep and Active states only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x90 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Spectrum Inversion Initial Setting
     *
     * RF signal spectrum | tuner spectrum sense     | Setting
     * (rfSpectrumSense)  | (confSense)              | (from Demod View)
     * -------------------+--------------------------+---------
     *  Normal            |  not same as RF(Normal)  |  0  (Normal)
     *  Invert            |  not same as RF(Normal)  |  1  (Invert)
     *  Normal            |  same as RF(Invert)      |  1  (Invert)
     *  Invert            |  same as RF(Invert)      |  0  (Normal)
     *
     *  slave     Bank    Addr   Bit     Name
     * ----------------------------------------------------------------------------
     *  <SLV-T>   90h     F3h    [0]     OREG_SINV
     */

    if (pDemod->confSense == pDemod->rfSpectrumSense) {
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF3, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    } else {
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF3, 0x01) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

    result = initCWDetection (pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t SLtoAA3(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("SLtoAA3");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC3_CHBOND
	if ((pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN)
		|| (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB)) {

		if (pDemod->atsc3Output == SONY_CXD6801_DEMOD_OUTPUT_ATSC3_BBP) {
            /* BBP output is not available for channel bonding */
			SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        result = sony_demod_atsc3_chbond_Enable (pDemod, 1);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }
#endif

	if (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB) {
        /* Nothing to do */
    } else
#ifdef SONY_CXD6801_DEMOD_SUPPORT_ALP
	if (pDemod->atsc3Output == SONY_CXD6801_DEMOD_OUTPUT_ATSC3_ALP) {
        /* Configure ALP clock Mode and Frequency. */
		result = sony_cxd6801_demod_SetALPClockModeAndFreq(pDemod, SONY_CXD6801_DTV_SYSTEM_ATSC3);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else
#endif
	if (pDemod->atsc3Output == SONY_CXD6801_DEMOD_OUTPUT_ATSC3_BBP) {
        /* Baseband Packet Output setting
         *  slave     Bank    Addr    Bit      Default    Value      Name
         * ---------------------------------------------------------------------------
         *  <SLV-T>   03h     B6h     [0]      8'h01      8'h00      OREG_ATSC30_CHBOND_MAIN_FLAG
         *  <SLV-T>   9Dh     F1h     [0]      8'h00      8'h01      OREG_CHBSEL_FORCE_CHBOND
         *  <SLV-T>   95h     90h     [0]      8'h01      8'h00      OREG_ALPLOCK_SYNC
         */
        /* Set SLV-T Bank : 0x03 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Set SLV-T Bank : 0x9D */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9D) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF1, 0x01) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Set SLV-T Bank : 0x95 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x95) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x90, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    } else {
        /* Configure TS clock Mode and Frequency. */
		result = sony_cxd6801_demod_SetTSClockModeAndFreq(pDemod, SONY_CXD6801_DTV_SYSTEM_ATSC3);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    /* Set SLV-X Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set demod mode */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x0E) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (pDemod->atsc3EasState == SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_EAS) {
        /* Enable EAS Mode */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x36, 0x01) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

    /* Set TS/ALP mode */
    {
        uint8_t data = 0;

        switch (pDemod->atsc3Output) {
		case SONY_CXD6801_DEMOD_OUTPUT_ATSC3_ALP:
		case SONY_CXD6801_DEMOD_OUTPUT_ATSC3_BBP:
        default:
            data = 0x02;
            break;

		case SONY_CXD6801_DEMOD_OUTPUT_ATSC3_ALP_DIV_TS:
            data = 0x00;
            break;
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, data) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

    /* Enable demod clock */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, 0x74) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x49, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x11 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* BBAGC TARGET level setting
     *  slave     Bank    Addr   Bit       Default    Value      Name
     * ----------------------------------------------------------------------------------
     *  <SLV-T>   11h     6Ah    [7:0]     8'h50      8'h50      OREG_ITB_DAGC_TRGT[7:0]
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6A, 0x50) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    {
        /* ATSC 3.0 initial setting
         *  slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         *  <SLV-T>   11h     33h    [7:0]     8'hC8      8'h00      OCTL_IFAGC_INITWAIT[7:0]
         *  <SLV-T>   11h     34h    [7:0]     8'h02      8'h03      OCTL_IFAGC_MINWAIT[7:0]
         *  <SLV-T>   11h     35h    [7:0]     8'h32      8'h3B      OCTL_IFAGC_MAXWAIT[7:0]
         */
        const uint8_t data[3] = { 0x00, 0x03, 0x3B };
		if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x33, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

    /* Set SLV-T Bank : 0x95 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x95) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* ATSC 3.0 initial setting
     *  slave     Bank    Addr    Bit      Default    Value      Name
     * -----------------------------------------------------------------------------------------
     *  <SLV-T>   95h     23h     [7:0]    8'h13      8'h03      OREG_OIF_PLPID_CONVMODE_M[1:0]
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x23, 0x03) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set tuner and bandwidth specific settings */
    result = SLtoAA3_BandSetting (pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Disable HiZ Setting 1 (TAGC, SAGC(Hi-Z), TSVALID, TSSYNC, TSCLK) */
	if (sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x08, 0x1F) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Disable HiZ Setting 2 */
	result = sony_cxd6801_demod_SetTSDataPinHiZ(pDemod, 0);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

static sony_cxd6801_result_t SLtoAA3_BandSetting(sony_cxd6801_demod_t * pDemod)
{
    SONY_CXD6801_TRACE_ENTER ("SLtoAA3_BandSetting");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x90 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    switch (pDemod->bandwidth) {
	case SONY_CXD6801_DTV_BW_8_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x14, 0xD5, 0x55, 0x55, 0x55
            };
            /* <Timing Recovery setting>
             *  slave     Bank    Addr    Bit      Default    Value      Name
             * ---------------------------------------------------------------------------------------
             *  <SLV-T>   90h     9Fh     [6:0]    8'h1B      8'h14      OREG_TRCG_NOMINALRATE[38:32]
             *  <SLV-T>   90h     A0h     [7:0]    8'hC7      8'hD5      OREG_TRCG_NOMINALRATE[31:24]
             *  <SLV-T>   90h     A1h     [7:0]    8'h1C      8'h55      OREG_TRCG_NOMINALRATE[23:16]
             *  <SLV-T>   90h     A2h     [7:0]    8'h71      8'h55      OREG_TRCG_NOMINALRATE[15:8]
             *  <SLV-T>   90h     A3h     [7:0]    8'hC7      8'h55      OREG_TRCG_NOMINALRATE[7:0]
             */

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        /* Set SLV-T Bank : 0x10 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /*
         * Filter settings for Sony silicon tuners
         *  slave     Bank    Addr    Bit      Default    Value      Name
         * -------------------------------------------------------------------------------
         *  <SLV-T>   10h     A6h     [7:0]    8'h1E      8'h2F      OREG_ITB_COEF01[7:0]
         *  <SLV-T>   10h     A7h     [7:0]    8'h1D      8'hBA      OREG_ITB_COEF02[7:0]
         *  <SLV-T>   10h     A8h     [7:0]    8'h29      8'h28      OREG_ITB_COEF11[7:0]
         *  <SLV-T>   10h     A9h     [7:0]    8'hC9      8'h9B      OREG_ITB_COEF12[7:0]
         *  <SLV-T>   10h     AAh     [7:0]    8'h2A      8'h28      OREG_ITB_COEF21[7:0]
         *  <SLV-T>   10h     ABh     [7:0]    8'hBA      8'h9D      OREG_ITB_COEF22[7:0]
         *  <SLV-T>   10h     ACh     [7:0]    8'h29      8'h28      OREG_ITB_COEF31[7:0]
         *  <SLV-T>   10h     ADh     [7:0]    8'hAD      8'hA1      OREG_ITB_COEF32[7:0]
         *  <SLV-T>   10h     AEh     [7:0]    8'h29      8'h29      OREG_ITB_COEF41[7:0]
         *  <SLV-T>   10h     AFh     [7:0]    8'hA4      8'hA5      OREG_ITB_COEF42[7:0]
         *  <SLV-T>   10h     B0h     [7:0]    8'h29      8'h2A      OREG_ITB_COEF51[7:0]
         *  <SLV-T>   10h     B1h     [7:0]    8'h9A      8'hAC      OREG_ITB_COEF52[7:0]
         *  <SLV-T>   10h     B2h     [7:0]    8'h28      8'h29      OREG_ITB_COEF61[7:0]
         *  <SLV-T>   10h     B3h     [7:0]    8'h9E      8'hB5      OREG_ITB_COEF62[7:0]
         */
		if (pDemod->tunerOptimize == SONY_CXD6801_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
            const uint8_t itbCoef[14] = {
            /*  COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62 */
                0x2F,  0xBA,  0x28,  0x9B,  0x28,  0x9D,  0x28,  0xA1,  0x29,  0xA5,  0x2A,  0xAC,  0x29,  0xB5
            };

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, itbCoef, 14) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }

            /* ASCOT setting ON
             *  slave     Bank    Addr    Bit     Default    Value      Name
             * ----------------------------------------------------------------------------------
             *  <SLV-T>   10h     A5h     [0]     8'h01      8'h01      OREG_ITB_GDEQ_EN
             */
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x01) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        } else {
            /* ASCOT setting OFF
             *  slave     Bank    Addr    Bit     Default    Value      Name
             * ----------------------------------------------------------------------------------
             *  <SLV-T>   10h     A5h     [0]     8'h01      8'h00      OREG_ITB_GDEQ_EN
             */
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        {
            /*
             * <IF freq setting>
             *  slave     Bank    Addr   Bit       Default   Value     Name
             * ---------------------------------------------------------------------------------
             *  <SLV-T>   10h     B6h    [7:0]     8'h1F     dcfrq1    OREG_DNCNV_LOFRQ_T[23:16]
             *  <SLV-T>   10h     B7h    [7:0]     8'h38     dcfrq2    OREG_DNCNV_LOFRQ_T[15:8]
             *  <SLV-T>   10h     B8h    [7:0]     8'h32     dcfrq3    OREG_DNCNV_LOFRQ_T[7:0]
             */
            uint8_t data[3];
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configATSC3_8 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configATSC3_8 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configATSC3_8 & 0xFF);
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        /* System bandwidth setting
         *  slave     Bank    Addr   Bit      Default    Value      Name
         * ----------------------------------------------------------------------------------
         *  <SLV-T>   10h     D7h    [2:0]    8'h00      8'h00      OREG_CHANNEL_WIDTH[2:0]
         */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Set SLV-T Bank : 0x1D */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x1D) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        {
            /* The ACI filter settings
             *  slave     Bank    Addr   Bit    Default  Value           Name
             * ---------------------------------------------------------------------------------
             *  <SLV-T>   1Dh     BFh    [0]    1'b0     1'b1            OREG_ACRFLT_EN
             *  <SLV-T>   1Dh     C0h    [7:0]  8'd0     8'h14           OREG_ACRFLT_FREQ0[23:16]
             *  <SLV-T>   1Dh     C1h    [7:0]  8'd0     8'h65           OREG_ACRFLT_FREQ0[15:8]
             *  <SLV-T>   1Dh     C2h    [7:0]  8'd0     8'h08           OREG_ACRFLT_FREQ0[7:0]
             *  <SLV-T>   1Dh     C3h    [7:0]  8'd0     8'hD7           OREG_ACRFLT_FREQ1[23:16]
             *  <SLV-T>   1Dh     C4h    [7:0]  8'd0     8'h65           OREG_ACRFLT_FREQ1[15:8]
             *  <SLV-T>   1Dh     C5h    [7:0]  8'd0     8'hEF           OREG_ACRFLT_FREQ1[7:0]
             *  <SLV-T>   1Dh     C6h    [7:0]  8'd0     8'h14           OREG_ACRFLT_FREQ2[23:16]
             *  <SLV-T>   1Dh     C7h    [7:0]  8'd0     8'h65           OREG_ACRFLT_FREQ2[15:8]
             *  <SLV-T>   1Dh     C8h    [7:0]  8'd0     8'h08           OREG_ACRFLT_FREQ2[7:0]
             */
            uint8_t data[10] = {
                0x01, 0x14, 0x65, 0x08, 0xD7, 0x65, 0xEF, 0x14, 0x65, 0x08
            };
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xBF, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        /* Set SLV-T Bank : 0x99 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x99) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        {
            /* Symbol interval setting
             *  slave     Bank    Addr    Bit      Default    Value      Name
             * -----------------------------------------------------------------------------------
             *  <SLV-T>   99h     89h     [7:0]    8'hDE      8'hA6      OREG_OFDM_SYMBOL_INTERVAL1[15:8]
             *  <SLV-T>   99h     8Ah     [7:0]    8'h39      8'hAB      OREG_OFDM_SYMBOL_INTERVAL1[7:0]
             *  <SLV-T>   99h     8Bh     [3:0]    8'h0D      8'h0A      OREG_OFDM_SYMBOL_INTERVAL2[11:8]
             *  <SLV-T>   99h     8Ch     [7:0]    8'hE4      8'h6B      OREG_OFDM_SYMBOL_INTERVAL2[7:0]
             */
            uint8_t data[4] = {
                0xA6, 0xAB, 0x0A, 0x6B
            };
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x89, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        break;

    case SONY_CXD6801_DTV_BW_7_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x17, 0xCF, 0x3C, 0xF3, 0xCF
            };
            /* <Timing Recovery setting>
             *  slave     Bank    Addr    Bit      Default    Value      Name
             * ---------------------------------------------------------------------------------------
             *  <SLV-T>   90h     9Fh     [6:0]    8'h1B      8'h17      OREG_TRCG_NOMINALRATE[38:32]
             *  <SLV-T>   90h     A0h     [7:0]    8'hC7      8'hCF      OREG_TRCG_NOMINALRATE[31:24]
             *  <SLV-T>   90h     A1h     [7:0]    8'h1C      8'h3C      OREG_TRCG_NOMINALRATE[23:16]
             *  <SLV-T>   90h     A2h     [7:0]    8'h71      8'hF3      OREG_TRCG_NOMINALRATE[15:8]
             *  <SLV-T>   90h     A3h     [7:0]    8'hC7      8'hCF      OREG_TRCG_NOMINALRATE[7:0]
             */

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        /* Set SLV-T Bank : 0x10 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /*
         * Filter settings for Sony silicon tuners
         *  slave     Bank    Addr    Bit      Default    Value      Name
         * ------------------------------------------------------------------------------
         *  <SLV-T>   10h     A6h    [7:0]     8'h1E      8'h30      OREG_ITB_COEF01[7:0]
         *  <SLV-T>   10h     A7h    [7:0]     8'h1D      8'hB1      OREG_ITB_COEF02[7:0]
         *  <SLV-T>   10h     A8h    [7:0]     8'h29      8'h29      OREG_ITB_COEF11[7:0]
         *  <SLV-T>   10h     A9h    [7:0]     8'hC9      8'h9A      OREG_ITB_COEF12[7:0]
         *  <SLV-T>   10h     AAh    [7:0]     8'h2A      8'h28      OREG_ITB_COEF21[7:0]
         *  <SLV-T>   10h     ABh    [7:0]     8'hBA      8'h9C      OREG_ITB_COEF22[7:0]
         *  <SLV-T>   10h     ACh    [7:0]     8'h29      8'h28      OREG_ITB_COEF31[7:0]
         *  <SLV-T>   10h     ADh    [7:0]     8'hAD      8'hA0      OREG_ITB_COEF32[7:0]
         *  <SLV-T>   10h     AEh    [7:0]     8'h29      8'h29      OREG_ITB_COEF41[7:0]
         *  <SLV-T>   10h     AFh    [7:0]     8'hA4      8'hA2      OREG_ITB_COEF42[7:0]
         *  <SLV-T>   10h     B0h    [7:0]     8'h29      8'h2B      OREG_ITB_COEF51[7:0]
         *  <SLV-T>   10h     B1h    [7:0]     8'h9A      8'hA6      OREG_ITB_COEF52[7:0]
         *  <SLV-T>   10h     B2h    [7:0]     8'h28      8'h2B      OREG_ITB_COEF61[7:0]
         *  <SLV-T>   10h     B3h    [7:0]     8'h9E      8'hAD      OREG_ITB_COEF62[7:0]
         */
		if (pDemod->tunerOptimize == SONY_CXD6801_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
            const uint8_t itbCoef[14] = {
            /*  COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62 */
                0x30,  0xB1,  0x29,  0x9A,  0x28,  0x9C,  0x28,  0xA0,  0x29,  0xA2,  0x2B,  0xA6,  0x2B,  0xAD
            };

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, itbCoef, 14) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }

            /* ASCOT setting ON
             *  slave     Bank    Addr    Bit      Default    Value      Name
             * ----------------------------------------------------------------------------------
             *  <SLV-T>   10h     A5h     [0]      8'h01      8'h01      OREG_ITB_GDEQ_EN
             */
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x01) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        } else {
            /* ASCOT setting OFF
             *  slave     Bank    Addr    Bit      Default    Value      Name
             * ----------------------------------------------------------------------------------
             *  <SLV-T>   10h     A5h     [0]      8'h01      8'h00      OREG_ITB_GDEQ_EN
             */
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        {
            /*
             * <IF freq setting>
             *  slave     Bank    Addr   Bit       Default   Value     Name
             * ---------------------------------------------------------------------------------
             *  <SLV-T>   10h     B6h    [7:0]     8'h1F     dcfrq1    OREG_DNCNV_LOFRQ_T[23:16]
             *  <SLV-T>   10h     B7h    [7:0]     8'h38     dcfrq2    OREG_DNCNV_LOFRQ_T[15:8]
             *  <SLV-T>   10h     B8h    [7:0]     8'h32     dcfrq3    OREG_DNCNV_LOFRQ_T[7:0]
             */
            uint8_t data[3];
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configATSC3_7 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configATSC3_7 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configATSC3_7 & 0xFF);
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        /* System bandwidth setting
         *  slave     Bank    Addr   Bit      Default    Value      Name
         * ----------------------------------------------------------------------------------
         *  <SLV-T>   10h     D7h    [2:0]    8'h00      8'h02      OREG_CHANNEL_WIDTH[2:0]
         */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x02) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Set SLV-T Bank : 0x1D */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x1D) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        {
            /* The ACI filter settings
             *  slave     Bank    Addr   Bit    Default  Value           Name
             * ---------------------------------------------------------------------------------
             *  <SLV-T>   1Dh     BFh    [0]    1'b0     1'b1            OREG_ACRFLT_EN
             *  <SLV-T>   1Dh     C0h    [7:0]  8'd0     8'h19           OREG_ACRFLT_FREQ0[23:16]
             *  <SLV-T>   1Dh     C1h    [7:0]  8'd0     8'h94           OREG_ACRFLT_FREQ0[15:8]
             *  <SLV-T>   1Dh     C2h    [7:0]  8'd0     8'h23           OREG_ACRFLT_FREQ0[7:0]
             *  <SLV-T>   1Dh     C3h    [7:0]  8'd0     8'hCC           OREG_ACRFLT_FREQ1[23:16]
             *  <SLV-T>   1Dh     C4h    [7:0]  8'd0     8'hD7           OREG_ACRFLT_FREQ1[15:8]
             *  <SLV-T>   1Dh     C5h    [7:0]  8'd0     8'hBA           OREG_ACRFLT_FREQ1[7:0]
             *  <SLV-T>   1Dh     C6h    [7:0]  8'd0     8'h19           OREG_ACRFLT_FREQ2[23:16]
             *  <SLV-T>   1Dh     C7h    [7:0]  8'd0     8'h94           OREG_ACRFLT_FREQ2[15:8]
             *  <SLV-T>   1Dh     C8h    [7:0]  8'd0     8'h23           OREG_ACRFLT_FREQ2[7:0]
             */
            uint8_t data[10] = {
                0x01, 0x19, 0x94, 0x23, 0xCC, 0xD7, 0xBA, 0x19, 0x94, 0x23
            };
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xBF, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        /* Set SLV-T Bank : 0x99 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x99) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        {
            /* Symbol interval setting
             *  slave     Bank    Addr    Bit      Default    Value      Name
             * -----------------------------------------------------------------------------------
             *  <SLV-T>   99h     89h     [7:0]    8'hDE      8'hBE      OREG_OFDM_SYMBOL_INTERVAL1[15:8]
             *  <SLV-T>   99h     8Ah     [7:0]    8'h39      8'h7A      OREG_OFDM_SYMBOL_INTERVAL1[7:0]
             *  <SLV-T>   99h     8Bh     [3:0]    8'h0D      8'h0B      OREG_OFDM_SYMBOL_INTERVAL2[11:8]
             *  <SLV-T>   99h     8Ch     [7:0]    8'hE4      8'hE8      OREG_OFDM_SYMBOL_INTERVAL2[7:0]
             */
            uint8_t data[4] = {
                0xBE, 0x7A, 0x0B, 0xE8
            };
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x89, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        break;

    case SONY_CXD6801_DTV_BW_6_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x1B, 0xC7, 0x1C, 0x71, 0xC7
            };
            /* <Timing Recovery setting>
             *  slave     Bank    Addr    Bit      Default    Value      Name
             * -----------------------------------------------------------------------
             *  <SLV-T>   90h     9Fh     [6:0]    8'h1B      8'h1B      OREG_TRCG_NOMINALRATE[38:32]
             *  <SLV-T>   90h     A0h     [7:0]    8'hC7      8'hC7      OREG_TRCG_NOMINALRATE[31:24]
             *  <SLV-T>   90h     A1h     [7:0]    8'h1C      8'h1C      OREG_TRCG_NOMINALRATE[23:16]
             *  <SLV-T>   90h     A2h     [7:0]    8'h71      8'h71      OREG_TRCG_NOMINALRATE[15:8]
             *  <SLV-T>   90h     A3h     [7:0]    8'hC7      8'hC7      OREG_TRCG_NOMINALRATE[7:0]
             */

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        /* Set SLV-T Bank : 0x10 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /*
         * Filter settings for Sony silicon tuners
         *  slave     Bank    Addr    Bit      Default    Value      Name
         * -------------------------------------------------------------------------------
         *  <SLV-T>   10h     A6h     [7:0]    8'h1E      8'h31     OREG_ITB_COEF01[7:0]
         *  <SLV-T>   10h     A7h     [7:0]    8'h1D      8'hA8     OREG_ITB_COEF02[7:0]
         *  <SLV-T>   10h     A8h     [7:0]    8'h29      8'h29     OREG_ITB_COEF11[7:0]
         *  <SLV-T>   10h     A9h     [7:0]    8'hC9      8'h9B     OREG_ITB_COEF12[7:0]
         *  <SLV-T>   10h     AAh     [7:0]    8'h2A      8'h27     OREG_ITB_COEF21[7:0]
         *  <SLV-T>   10h     ABh     [7:0]    8'hBA      8'h9C     OREG_ITB_COEF22[7:0]
         *  <SLV-T>   10h     ACh     [7:0]    8'h29      8'h28     OREG_ITB_COEF31[7:0]
         *  <SLV-T>   10h     ADh     [7:0]    8'hAD      8'h9E     OREG_ITB_COEF32[7:0]
         *  <SLV-T>   10h     AEh     [7:0]    8'h29      8'h29     OREG_ITB_COEF41[7:0]
         *  <SLV-T>   10h     AFh     [7:0]    8'hA4      8'hA4     OREG_ITB_COEF42[7:0]
         *  <SLV-T>   10h     B0h     [7:0]    8'h29      8'h29     OREG_ITB_COEF51[7:0]
         *  <SLV-T>   10h     B1h     [7:0]    8'h9A      8'hA2     OREG_ITB_COEF52[7:0]
         *  <SLV-T>   10h     B2h     [7:0]    8'h28      8'h29     OREG_ITB_COEF61[7:0]
         *  <SLV-T>   10h     B3h     [7:0]    8'h9E      8'hA8     OREG_ITB_COEF62[7:0]
         */
		if (pDemod->tunerOptimize == SONY_CXD6801_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
            const uint8_t itbCoef[14] = {
            /*  COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62 */
                0x31,  0xA8,  0x29,  0x9B,  0x27,  0x9C,  0x28,  0x9E,  0x29,  0xA4,  0x29,  0xA2,  0x29,  0xA8
            };

			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, itbCoef, 14) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }

            /* ASCOT setting ON
             *  slave     Bank    Addr    Bit      Default    Value      Name
             * ----------------------------------------------------------------------------------
             *  <SLV-T>   10h     A5h     [0]      8'h01      8'h01      OREG_ITB_GDEQ_EN
             */
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x01) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        } else {
            /* ASCOT setting OFF
             *  slave     Bank    Addr    Bit      Default    Value      Name
             * ----------------------------------------------------------------------------------
             *  <SLV-T>   10h     A5h     [0]      8'h01      8'h00      OREG_ITB_GDEQ_EN
             */
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        {
            /*
             * <IF freq setting>
             *  slave     Bank    Addr   Bit       Default   Value     Name
             * ---------------------------------------------------------------------------------
             *  <SLV-T>   10h     B6h    [7:0]     8'h1F     dcfrq1    OREG_DNCNV_LOFRQ_T[23:16]
             *  <SLV-T>   10h     B7h    [7:0]     8'h38     dcfrq2    OREG_DNCNV_LOFRQ_T[15:8]
             *  <SLV-T>   10h     B8h    [7:0]     8'h32     dcfrq3    OREG_DNCNV_LOFRQ_T[7:0]
             */
            uint8_t data[3];
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configATSC3_6 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configATSC3_6 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configATSC3_6 & 0xFF);
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        /* System bandwidth setting
         *  slave     Bank    Addr   Bit      Default    Value      Name
         * ----------------------------------------------------------------------------------
         *  <SLV-T>   10h     D7h    [2:0]    8'h00      8'h04      OREG_CHANNEL_WIDTH[2:0]
         */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x04) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Set SLV-T Bank : 0x1D */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x1D) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        {
            /* The ACI filter settings
             *  slave     Bank    Addr   Bit    Default  Value           Name
             * ---------------------------------------------------------------------------------
             *  <SLV-T>   1Dh     BFh    [0]    1'b0     1'b1            OREG_ACRFLT_EN
             *  <SLV-T>   1Dh     C0h    [7:0]  8'd0     8'h1E           OREG_ACRFLT_FREQ0[23:16]
             *  <SLV-T>   1Dh     C1h    [7:0]  8'd0     8'hC3           OREG_ACRFLT_FREQ0[15:8]
             *  <SLV-T>   1Dh     C2h    [7:0]  8'd0     8'h3E           OREG_ACRFLT_FREQ0[7:0]
             *  <SLV-T>   1Dh     C3h    [7:0]  8'd0     8'hC2           OREG_ACRFLT_FREQ1[23:16]
             *  <SLV-T>   1Dh     C4h    [7:0]  8'd0     8'h79           OREG_ACRFLT_FREQ1[15:8]
             *  <SLV-T>   1Dh     C5h    [7:0]  8'd0     8'h84           OREG_ACRFLT_FREQ1[7:0]
             *  <SLV-T>   1Dh     C6h    [7:0]  8'd0     8'h1E           OREG_ACRFLT_FREQ2[23:16]
             *  <SLV-T>   1Dh     C7h    [7:0]  8'd0     8'hC3           OREG_ACRFLT_FREQ2[15:8]
             *  <SLV-T>   1Dh     C8h    [7:0]  8'd0     8'h3E           OREG_ACRFLT_FREQ2[7:0]
             */
            uint8_t data[10] = {
                0x01, 0x1E, 0xC3, 0x3E, 0xC2, 0x79, 0x84, 0x1E, 0xC3, 0x3E
            };
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xBF, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        /* Set SLV-T Bank : 0x99 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x99) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        {
            /* Symbol interval setting
             *  slave     Bank    Addr    Bit      Default    Value      Name
             * -----------------------------------------------------------------------------------
             *  <SLV-T>   99h     89h     [7:0]    8'hDE      8'hDE      OREG_OFDM_SYMBOL_INTERVAL1[15:8]
             *  <SLV-T>   99h     8Ah     [7:0]    8'h39      8'h39      OREG_OFDM_SYMBOL_INTERVAL1[7:0]
             *  <SLV-T>   99h     8Bh     [3:0]    8'h0D      8'h0D      OREG_OFDM_SYMBOL_INTERVAL2[11:8]
             *  <SLV-T>   99h     8Ch     [7:0]    8'hE4      8'hE4      OREG_OFDM_SYMBOL_INTERVAL2[7:0]
             */
            uint8_t data[4] = {
                0xDE, 0x39, 0x0D, 0xE4
            };
			if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x89, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
        }

        break;

    default:
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t AA3toAA3(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("AA3toAA3");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Disable TS/ALP output */
	result = sony_cxd6801_demod_SetStreamOutput(pDemod, 0);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Set tuner and bandwidth specific settings */
    result = SLtoAA3_BandSetting (pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

static sony_cxd6801_result_t AA3toSL(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("AA3toSL");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Disable TS/ALP output */
	result = sony_cxd6801_demod_SetStreamOutput(pDemod, 0);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Enable Hi-Z setting 1 (TAGC, SAGC, TSVALID, TSSYNC, TSCLK) */
	if (sony_cxd6801_i2c_SetRegisterBits(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x1F, 0x1F) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Enable Hi-Z setting 2 */
	result = sony_cxd6801_demod_SetTSDataPinHiZ(pDemod, 1);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

	if (pDemod->atsc3Output == SONY_CXD6801_DEMOD_OUTPUT_ATSC3_BBP) {
        /* Cancel baseband packet output setting
         *  slave     Bank    Addr    Bit      Default    Value      Name
         * ---------------------------------------------------------------------------
         *  <SLV-T>   03h     B6h     [0]      8'h01      8'h01      OREG_ATSC30_CHBOND_MAIN_FLAG
         *  <SLV-T>   9Dh     F1h     [0]      8'h00      8'h00      OREG_CHBSEL_FORCE_CHBOND
         *  <SLV-T>   95h     90h     [0]      8'h01      8'h01      OREG_ALPLOCK_SYNC
         */
        /* Set SLV-T Bank : 0x03 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, 0x01) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Set SLV-T Bank : 0x9D */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9D) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF1, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Set SLV-T Bank : 0x95 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x95) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x90, 0x01) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

    /* Cancel ATSC 3.0 Demod parameter setting
     *  slave     Bank    Addr   Bit    Default  Value           Name
     * ---------------------------------------------------------------------------------
     *  <SLV-T>   1Dh     BFh    [0]    1'b0     1'b0            OREG_ACRFLT_EN
     */
    /* Set SLV-T Bank : 0x1D */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x1D) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xBF, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-X Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x00 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x49, 0x33) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, 0x21) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Demodulator SW reset */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFE, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Disable demodulator clock */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set tstlv mode to default */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (pDemod->atsc3EasState == SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_EAS) {
        /* Disable EAS Mode */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x36, 0x00) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

    /* Set demodulator mode to default */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC3_CHBOND
	if ((pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN)
		|| (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB)) {
        result = sony_demod_atsc3_chbond_Enable (pDemod, 0);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }
#endif

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t invertSpectrum(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result;
    uint8_t data = 0;
    uint8_t spectrumInv = 0;

    SONY_CXD6801_TRACE_ENTER ("invertSpectrum");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x90 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave     Bank    Addr   Bit      Value      Name
     * -------------------------------------------------------------------
     *  <SLV-T>   90h     F3h    [0]      8'h01      OREG_SINV[0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF3, &data, 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    if ((data & 0x01) == 0x00) {
        spectrumInv = 0x01;
    } else {
        spectrumInv = 0x00;
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF3, spectrumInv) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    pDemod->atsc3AutoSpectrumInv_flag = 1;
    pDemod->atsc3CWDetection_flag = 0;

    result = initCWDetection (pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Disable TS/ALP output */
	result = sony_cxd6801_demod_SetStreamOutput(pDemod, 0);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

	result = sony_cxd6801_demod_SoftReset(pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Enable TS/ALP output */
	result = sony_cxd6801_demod_SetStreamOutput(pDemod, 1);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

static sony_cxd6801_result_t detectCW(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result;
    uint8_t data[9];
    uint32_t totalPwr;
    uint32_t maxCPwr;
    uint32_t maxCNum;
    int32_t freqQW;
    int32_t freqQW2;

    SONY_CXD6801_TRACE_ENTER ("detectCW");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /*
     *  slave     Bank    Addr   Bit     Name
     * ---------------------------------------------------------------------------------
     *  <SLV-T>   99h     E6h    [7:0]   IREG_BSDET_CWD_TOTALPWR[31:24]
     *  <SLV-T>   99h     E7h    [7:0]   IREG_BSDET_CWD_TOTALPWR[23:16]
     *  <SLV-T>   99h     E8h    [7:0]   IREG_BSDET_CWD_TOTALPWR[15:8]
     *  <SLV-T>   99h     E9h    [7:0]   IREG_BSDET_CWD_TOTALPWR[7:0]
     *  <SLV-T>   99h     EAh    [0]     IREG_BSDET_CWD_MAXCPWR[16]
     *  <SLV-T>   99h     EBh    [7:0]   IREG_BSDET_CWD_MAXCPWR[15:8]
     *  <SLV-T>   99h     ECh    [7:0]   IREG_BSDET_CWD_MAXCPWR[7:0]
     *  <SLV-T>   99h     EDh    [6:0]   IREG_BSDET_CWD_MAXCNUM[14:8]
     *  <SLV-T>   99h     EEh    [7:0]   IREG_BSDET_CWD_MAXCNUM[7:0]
     */

    /* Set SLV-T Bank : 0x99 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x99) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE6, data, 9) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    totalPwr = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    maxCPwr = ((data[4] & 0x01) << 16) | (data[5] << 8) | data[6];
    maxCNum = ((data[7] & 0x7F) << 8) | data[8];
    /* range check */
    if (maxCNum > 2047) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    if ((maxCPwr <= (totalPwr / 100)) || (maxCPwr <= 500)) {
        /* CW is not detected. */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
    }

    /*
     * OREG_CAS_CCIFLT_FREQ_CW = (maxCNum - 1024) * 3000 / 6144000 * 2^18
     *                         = (maxCNum - 1024) / 2048 * 2^18
     *                         = (maxCNum - 1024) * 2^7
     */
    freqQW = (maxCNum - 1024) * 128;

    /*
     * (BW = 8MHz)
     * OREG_CAS_CCIFLT_FREQ_CW2 = (maxCNum - 1024) * 3000 / 9216000 * 2^18
     *                          = (maxCNum - 1024) / 3 * 2^8
     *
     * (BW = 7MHz)
     * OREG_CAS_CCIFLT_FREQ_CW2 = (maxCNum - 1024) * 3000 / 8064000 * 2^18
     *                          = (maxCNum - 1024) / 21 * 2^11
     *
     * (BW = 6MHz)
     * OREG_CAS_CCIFLT_FREQ_CW2 = (maxCNum - 1024) * 3000 / 6912000 * 2^18
     *                          = (maxCNum - 1024) / 9 * 2^10
     */
    switch (pDemod->bandwidth) {
	case SONY_CXD6801_DTV_BW_8_MHZ:
        freqQW2 = ((maxCNum - 1024) * 256) / 3;
        break;
	case SONY_CXD6801_DTV_BW_7_MHZ:
        freqQW2 = ((maxCNum - 1024) * 2048) / 21;
        break;
	case SONY_CXD6801_DTV_BW_6_MHZ:
        freqQW2 = ((maxCNum - 1024) * 1024) / 9;
        break;
    default:
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }
    /* Set SLV-T Bank : 0x9A */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9A) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave     Bank    Addr   Bit     Name
     * -------------------------------------------------------------------
     *  <SLV-T>   9Ah     3Eh    [1:0]   OREG_CAS_CCIFLT_FREQ_CW[17:16]
     *  <SLV-T>   9Ah     3Fh    [7:0]   OREG_CAS_CCIFLT_FREQ_CW[15:8]
     *  <SLV-T>   9Ah     40h    [7:0]   OREG_CAS_CCIFLT_FREQ_CW[7:0]
     *  <SLV-T>   9Ah     41h    [1:0]   OREG_CAS_CCIFLT_FREQ_CW2[17:16]
     *  <SLV-T>   9Ah     42h    [7:0]   OREG_CAS_CCIFLT_FREQ_CW2[15:8]
     *  <SLV-T>   9Ah     43h    [7:0]   OREG_CAS_CCIFLT_FREQ_CW2[7:0]
     */
    data[0] = ((uint32_t)freqQW >> 16) & 0x03;
    data[1] = ((uint32_t)freqQW >> 8) & 0xFF;
    data[2] = (uint32_t)freqQW & 0xFF;
    data[3] = ((uint32_t)freqQW2 >> 16) & 0x03;
    data[4] = ((uint32_t)freqQW2 >> 8) & 0xFF;
    data[5] = (uint32_t)freqQW2 & 0xFF;
	if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3E, data, 6) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x98 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x98) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave     Bank    Addr   Bit       Default     Value      Name
     * ----------------------------------------------------------------------------------------
     *  <SLV-T>   98h     C6h    [3:0]     8'h04       8'h05      OREG_CAS_CCIFLT_EN_CW[3:0]
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC6, 0x05) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x9B */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9B) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave     Bank    Addr    Bit      Default     Value      Name
     * -------------------------------------------------------------------
     *  <SLV-T>   9Bh     11h     [5:0]    8'h20       8'h24      OREG_CAS_CWFLT_ACTCOND[5:0]
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x11, 0x24) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x9A */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9A) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave     Bank    Addr    Bit      Default     Value      Name
     * -------------------------------------------------------------------
     *  <SLV-T>   9Ah     3Ch     [3:0]    8'h05       8'h03      OREG_CAS_CCIFLT_MU_CW[3:0]
     *  <SLV-T>   9Ah     3Dh     [3:0]    8'h05       8'h03      OREG_CAS_CCIFLT_MU_CW2[3:0]
     *  <SLV-T>   9Ah     50h     [2:0]    8'h05       8'h04      OREG_CWD_PERIOD[2:0]
     */
    data[0] = 0x03;
    data[1] = 0x03;
	if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3C, data, 2) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x50, 0x04) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    pDemod->atsc3CWDetection_flag = 1;

    /* Disable TS/ALP output */
	result = sony_cxd6801_demod_SetStreamOutput(pDemod, 0);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

	result = sony_cxd6801_demod_SoftReset(pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Enable TS/ALP output */
	result = sony_cxd6801_demod_SetStreamOutput(pDemod, 1);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t initCWDetection(sony_cxd6801_demod_t * pDemod)
{
    SONY_CXD6801_TRACE_ENTER ("initCWDetection");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x98 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x98) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr    Bit      Value    Name
     * ----------------------------------------------------------------------------
     *  <SLV-T>   98h     C6h     [3:0]    8'h04    OREG_CAS_CCIFLT_EN_CW
     */

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC6, 0x04) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x9B */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9B) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr    Bit      Value    Name
     * ----------------------------------------------------------------------------
     *  <SLV-T>   9Bh     11h     [5:0]    8'h20    OREG_CAS_CWFLT_ACTCOND
     */

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x11, 0x20) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x9A */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9A) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    {
        /*  slave     Bank    Addr    Bit      Value    Name
         * ----------------------------------------------------------------------------
         *  <SLV-T>   9Ah     3Ch     [3:0]    8'h05    OREG_CAS_CCIFLT_MU_CW
         *  <SLV-T>   9Ah     3Dh     [3:0]    8'h05    OREG_CAS_CCIFLT_MU_CW2
         *  <SLV-T>   9Ah     3Eh     [3:0]    8'h00    OREG_CAS_CCIFLT_FREQ_CW[17:16]
         *  <SLV-T>   9Ah     3Fh     [3:0]    8'h00    OREG_CAS_CCIFLT_FREQ_CW[15:8]
         *  <SLV-T>   9Ah     40h     [3:0]    8'h00    OREG_CAS_CCIFLT_FREQ_CW[7:0]
         *  <SLV-T>   9Ah     41h     [3:0]    8'h00    OREG_CAS_CCIFLT_FREQ_CW2[17:16]
         *  <SLV-T>   9Ah     42h     [3:0]    8'h00    OREG_CAS_CCIFLT_FREQ_CW2[15:8]
         *  <SLV-T>   9Ah     43h     [3:0]    8'h00    OREG_CAS_CCIFLT_FREQ_CW2[7:0]
         *  <SLV-T>   9Ah     50h     [3:0]    8'h05    OREG_CWD_PERIOD[2:0]
         */
        const uint8_t data[8] = { 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3C, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x50, 0x05) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}
