/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2017/05/09
  Modification ID : 9849c174faf82fcc958734f152ba29f70274fdec
------------------------------------------------------------------------------*/

#include "sony_cxd_demod_dvbt2.h"
#include "sony_cxd_demod_dvbt2_monitor.h"

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Configure the demodulator from Sleep to Active for DVB-T2
*/
static sony_cxd_result_t SLtoAT2(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator for tuner optimisations and bandwidth specific
        settings.
*/
static sony_cxd_result_t SLtoAT2_BandSetting(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active DVB-T2 to Active to DVB-T2.  Used
        for changing channel parameters.
*/
static sony_cxd_result_t AT2toAT2(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active DVB-T2 to Sleep.
*/
static sony_cxd_result_t AT2toSL(sony_cxd_demod_t * pDemod);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd_result_t sony_cxd_demod_dvbt2_Tune (sony_cxd_demod_t * pDemod,
                                    sony_cxd_dvbt2_tune_param_t * pTuneParam)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_Tune");

    if ((!pDemod) || (!pTuneParam)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if ((pDemod->state == sony_cxd_DEMOD_STATE_ACTIVE) && (pDemod->system == sony_cxd_DTV_SYSTEM_DVBT2)) {
        /* Demodulator Active and set to DVB-T2 mode */
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = AT2toAT2(pDemod);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }
    }
    else if((pDemod->state == sony_cxd_DEMOD_STATE_ACTIVE) && (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2)){
        /* Demodulator Active but not DVB-T2 mode */
        result = sony_cxd_demod_Sleep(pDemod);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        pDemod->system = sony_cxd_DTV_SYSTEM_DVBT2;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLtoAT2(pDemod);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == sony_cxd_DEMOD_STATE_SLEEP) {
        /* Demodulator in Sleep mode */
        pDemod->system = sony_cxd_DTV_SYSTEM_DVBT2;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLtoAT2 (pDemod);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }
    }
    else {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Update demodulator state */
    pDemod->state = sony_cxd_DEMOD_STATE_ACTIVE;

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_Sleep (sony_cxd_demod_t * pDemod)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_Sleep");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    result = AT2toSL (pDemod);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_CheckDemodLock (sony_cxd_demod_t * pDemod,
                                              sony_cxd_demod_lock_result_t * pLock)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    uint8_t sync = 0;
    uint8_t tslock = 0;
    uint8_t unlockDetected = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_CheckDemodLock");

    if (!pDemod || !pLock) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    *pLock = sony_cxd_DEMOD_LOCK_RESULT_NOTDETECT;

    result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &sync, &tslock, &unlockDetected);

    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    if (unlockDetected) {
        *pLock = sony_cxd_DEMOD_LOCK_RESULT_UNLOCKED;
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
    }

    if (sync >= 6) {
        *pLock = sony_cxd_DEMOD_LOCK_RESULT_LOCKED;
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_CheckTSLock (sony_cxd_demod_t * pDemod,
                                            sony_cxd_demod_lock_result_t * pLock)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    uint8_t sync = 0;
    uint8_t tslock = 0;
    uint8_t unlockDetected = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_CheckTSLock");

    if (!pDemod || !pLock) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    *pLock = sony_cxd_DEMOD_LOCK_RESULT_NOTDETECT;

    result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &sync, &tslock, &unlockDetected);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    if (unlockDetected) {
        *pLock = sony_cxd_DEMOD_LOCK_RESULT_UNLOCKED;
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
    }

    if ((sync >= 6) && tslock) {
        *pLock = sony_cxd_DEMOD_LOCK_RESULT_LOCKED;
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_SetPLPConfig (sony_cxd_demod_t * pDemod,
                                             uint8_t autoPLP,
                                             uint8_t plpId)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_SetPLPConfig");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x23 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x23) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (!autoPLP) {
        /* Manual PLP selection mode. Set the data PLP Id. */
        /* Slave    Bank    Addr    Bit     Default   Value      Name
         * --------------------------------------------------------------------------
         * <SLV-T>   23h     AFh    [7:0]   8'h00     8'h**      OREGD_FP_PLP_ID[7:0]
         */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xAF, plpId) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* Auto PLP select (Scanning mode = 0x00). Data PLP select = 0x01. */
    /* Slave    Bank    Addr    Bit     Default   Value      Name
     * --------------------------------------------------------------------------------
     * <SLV-T>   23h     ADh    [1:0]   8'h00     8'h01      OREGD_FP_PLP_AUTO_SEL[1:0]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xAD, autoPLP? 0x00 : 0x01) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_SetProfile (sony_cxd_demod_t * pDemod,
                                           sony_cxd_dvbt2_profile_t profile)
{
    uint8_t t2Mode_tuneMode = 0;
    uint8_t seqNot2Dtime = 0;
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_SetProfile");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    switch (profile) {
    case sony_cxd_DVBT2_PROFILE_BASE:
        t2Mode_tuneMode = 0x01; /* Set profile to DVB-T2 base without recovery */
        seqNot2Dtime = 0x0E; /* Set early unlock time */
        break;

    case sony_cxd_DVBT2_PROFILE_LITE:
        t2Mode_tuneMode = 0x05; /* Set profile to DVB-T2 lite without recovery */
        seqNot2Dtime = 0x2E; /* Set early unlock time */
        break;

    case sony_cxd_DVBT2_PROFILE_ANY:
        t2Mode_tuneMode = 0x00; /* Set profile to auto detection */
        seqNot2Dtime = 0x2E; /* Set early unlock time */
        break;

    default:
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x2E */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2E) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set profile and tune mode
     * slave    Bank    Addr    Bit       default   Value       Name
     * ---------------------------------------------------------------------------------------
     * <SLV-T>  2Eh     10h     [2:0]     3'b001    3'b001      OREG_T2MODE,OREG_TUNEMODE[1:0]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, t2Mode_tuneMode) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x2B */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2B) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set early unlock detection time
     * slave    Bank    Addr    Bit      default   Value     Name
     * -------------------------------------------------------------------------------
     * <SLV-T>  2Bh     9Dh     [7:0]    8'd14     8'dXX     OREG_SEQ_NOT2_DTIME[15:8]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9D, seqNot2Dtime) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_CheckL1PostValid (sony_cxd_demod_t * pDemod,
                                                 uint8_t * pL1PostValid)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    uint8_t data;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_CheckL1PostValid");

    if ((!pDemod) || (!pL1PostValid)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x22 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Slave     Bank    Addr    Bit     Name             Meaning
     * ---------------------------------------------------------------------
     * <SLV-T>   22h     12h     [0]     IL1POST_OK       0:Invalid, 1:Valid
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x12, &data, 1) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    *pL1PostValid = data & 0x01;

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_ClearBERSNRHistory (sony_cxd_demod_t * pDemod)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_ClearBERSNRHistory");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x02 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Slave    Bank    Addr    Bit     Name                  Meaning
     * -----------------------------------------------------------------------
     * <SLV-T>   02h     4Ah     [0]    OREG_XMON_USER_CLEAR  1:Clear
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4A, 0x01) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd_result_t SLtoAT2(sony_cxd_demod_t * pDemod)
{
sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("SLtoAT2");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Configure TS clock Mode and Frequency */
    result = sony_cxd_demod_SetTSClockModeAndFreq (pDemod, sony_cxd_DTV_SYSTEM_DVBT2);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x02) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set TS/TLV mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable demod clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x01) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, 0x74) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x49, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x11 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* BBAGC TARGET level setting
     * slave    Bank    Addr    Bit    default     Value          Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   11h     6Ah    [7:0]     8'h50      8'h50      OREG_ITB_DAGC_TRGT[7:0]
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6A, 0x50) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* TSIF setting
     * slave     Bank    Addr   Bit     default    Value      Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   00h     CEh     [0]      8'h01      8'h01      ONOPARITY
     * <SLV-T>   00h     CFh     [0]      8'h01      8'h01      ONOPARITY_MANUAL_ON
     */
    {
        const uint8_t data[] = { 0x01, 0x01 };

        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xCE, data, sizeof (data)) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* DVB-T2 initial setting
     * slave     Bank    Addr   Bit     default    Value      Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   13h     83h    [7:0]     8'h40      8'h10      OREG_ISIC_POSPROTECT[7:0]
     * <SLV-T>   13h     86h    [7:0]     8'h21      8'h34      OREG_ISIC_CFNRMOFFSET
     * <SLV-T>   13h     9Fh    [7:0]     8'hFB      8'hD8      OREG_CFUPONTHRESHOLDMAX[7:0]
     * <SLV-T>   23h     DBh     [0]      8'h00      8'h01      OREG_FP_PLP_ALWAYS_MATCH
     */
    /* Set SLV-T Bank : 0x13 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x13) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x83, 0x10) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x86, 0x34) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, 0xD8) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x23 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x23) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xDB, 0x01) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x11 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        /* DVB-T2 24MHz Xtal setting
         * Slave     Bank    Addr   Bit       Default    Value      Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   11h     33h    [7:0]     8'hC8      8'h00      OCTL_IFAGC_INITWAIT[7:0]
         * <SLV-T>   11h     34h    [7:0]     8'h02      8'h03      OCTL_IFAGC_MINWAIT[7:0]
         * <SLV-T>   11h     35h    [7:0]     8'h32      8'h3B      OCTL_IFAGC_MAXWAIT[7:0]
         */
        const uint8_t data[3] = { 0x00, 0x03, 0x3B };
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x33, data, sizeof (data)) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* Set tuner and bandwidth specific settings */
    result = SLtoAT2_BandSetting (pDemod);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable HiZ Setting 1 (TAGC, SAGC(Hi-Z), TSVALID, TSSYNC, TSCLK) */
    if (sony_cxd_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x08, 0x1F) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable HiZ Setting 2 */
    result = sony_cxd_demod_SetTSDataPinHiZ (pDemod, 0);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t SLtoAT2_BandSetting(sony_cxd_demod_t * pDemod)
{
    sony_cxd_TRACE_ENTER ("SLtoAT2_BandSetting");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x20 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    switch (pDemod->bandwidth) {
    case sony_cxd_DTV_BW_8_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x15, 0x00, 0x00, 0x00, 0x00
            };
            /* <Timing Recovery setting>
             * Slave     Bank    Addr   Bit     Default   Value   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   20h     9Fh    [5:0]   8'h15     nomi1   OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   20h     A0h    [7:0]   8'h00     nomi2   OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   20h     A1h    [7:0]   8'h00     nomi3   OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   20h     A2h    [7:0]   8'h00     nomi4   OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   20h     A3h    [7:0]   8'h00     nomi5   OREG_TRCG_NOMINALRATE[7:0]
             * <SLV-T>   27h     7Ah    [3:0]   8'h00     8'h00   OREG_TRCG_LMTVAL0[3:0]
             */

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            /* Set SLV-T Bank : 0x27 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x27) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, 0x00) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* Set SLV-T Bank : 0x10 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /*
         * Filter settings for Sony silicon tuners
         * slave    bank    addr     bit      default    name
         * ------------------------------------------------------------------
         * <SLV-T>   10h     A6h    [7:0]     8'h1E      OREG_ITB_COEF01[7:0]
         * <SLV-T>   10h     A7h    [7:0]     8'h1D      OREG_ITB_COEF02[7:0]
         * <SLV-T>   10h     A8h    [7:0]     8'h29      OREG_ITB_COEF11[7:0]
         * <SLV-T>   10h     A9h    [7:0]     8'hC9      OREG_ITB_COEF12[7:0]
         * <SLV-T>   10h     AAh    [7:0]     8'h2A      OREG_ITB_COEF21[7:0]
         * <SLV-T>   10h     ABh    [7:0]     8'hBA      OREG_ITB_COEF22[7:0]
         * <SLV-T>   10h     ACh    [7:0]     8'h29      OREG_ITB_COEF31[7:0]
         * <SLV-T>   10h     ADh    [7:0]     8'hAD      OREG_ITB_COEF32[7:0]
         * <SLV-T>   10h     AEh    [7:0]     8'h29      OREG_ITB_COEF41[7:0]
         * <SLV-T>   10h     AFh    [7:0]     8'hA4      OREG_ITB_COEF42[7:0]
         * <SLV-T>   10h     B0h    [7:0]     8'h29      OREG_ITB_COEF51[7:0]
         * <SLV-T>   10h     B1h    [7:0]     8'h9A      OREG_ITB_COEF52[7:0]
         * <SLV-T>   10h     B2h    [7:0]     8'h28      OREG_ITB_COEF61[7:0]
         * <SLV-T>   10h     B3h    [7:0]     8'h9E      OREG_ITB_COEF62[7:0]
         */
        if (pDemod->tunerOptimize == sony_cxd_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
            const uint8_t itbCoef[14] = {
            /*  COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62 */
                0x2F,  0xBA,  0x28,  0x9B,  0x28,  0x9D,  0x28,  0xA1,  0x29,  0xA5,  0x2A,  0xAC,  0x29,  0xB5
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, itbCoef, 14) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            /* ASCOT setting ON
             * slave    Bank    Addr    Bit    default     Value          Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   10h     A5h     [0]      8'h01      8'h01      OREG_ITB_GDEQ_EN
             */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x01) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        } else {
            /* ASCOT setting OFF
             * slave    Bank    Addr    Bit    default     Value          Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   10h     A5h     [0]      8'h01      8'h00      OREG_ITB_GDEQ_EN
             */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_8 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_8 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT2_8 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]    8'h00      8'h00      OREG_CHANNEL_WIDTH[2:0]
         */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x00) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        break;

    case sony_cxd_DTV_BW_7_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x18, 0x00, 0x00, 0x00, 0x00
            };
            /* <Timing Recovery setting>
             * Slave     Bank    Addr   Bit     Default   Value   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   20h     9Fh    [5:0]   8'h15     nomi1   OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   20h     A0h    [7:0]   8'h00     nomi2   OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   20h     A1h    [7:0]   8'h00     nomi3   OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   20h     A2h    [7:0]   8'h00     nomi4   OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   20h     A3h    [7:0]   8'h00     nomi5   OREG_TRCG_NOMINALRATE[7:0]
             * <SLV-T>   27h     7Ah    [3:0]   8'h00     8'h00   OREG_TRCG_LMTVAL0[3:0]
             */

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            /* Set SLV-T Bank : 0x27 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x27) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, 0x00) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* Set SLV-T Bank : 0x10 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /*
         * Filter settings for Sony silicon tuners
         * slave    bank    addr     bit      default    name
         * ------------------------------------------------------------------
         * <SLV-T>   10h     A6h    [7:0]     8'h1E      OREG_ITB_COEF01[7:0]
         * <SLV-T>   10h     A7h    [7:0]     8'h1D      OREG_ITB_COEF02[7:0]
         * <SLV-T>   10h     A8h    [7:0]     8'h29      OREG_ITB_COEF11[7:0]
         * <SLV-T>   10h     A9h    [7:0]     8'hC9      OREG_ITB_COEF12[7:0]
         * <SLV-T>   10h     AAh    [7:0]     8'h2A      OREG_ITB_COEF21[7:0]
         * <SLV-T>   10h     ABh    [7:0]     8'hBA      OREG_ITB_COEF22[7:0]
         * <SLV-T>   10h     ACh    [7:0]     8'h29      OREG_ITB_COEF31[7:0]
         * <SLV-T>   10h     ADh    [7:0]     8'hAD      OREG_ITB_COEF32[7:0]
         * <SLV-T>   10h     AEh    [7:0]     8'h29      OREG_ITB_COEF41[7:0]
         * <SLV-T>   10h     AFh    [7:0]     8'hA4      OREG_ITB_COEF42[7:0]
         * <SLV-T>   10h     B0h    [7:0]     8'h29      OREG_ITB_COEF51[7:0]
         * <SLV-T>   10h     B1h    [7:0]     8'h9A      OREG_ITB_COEF52[7:0]
         * <SLV-T>   10h     B2h    [7:0]     8'h28      OREG_ITB_COEF61[7:0]
         * <SLV-T>   10h     B3h    [7:0]     8'h9E      OREG_ITB_COEF62[7:0]
         */
        if (pDemod->tunerOptimize == sony_cxd_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
            const uint8_t itbCoef[14] = {
            /*  COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62 */
                0x30,  0xB1,  0x29,  0x9A,  0x28,  0x9C,  0x28,  0xA0,  0x29,  0xA2,  0x2B,  0xA6,  0x2B,  0xAD
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, itbCoef, 14) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            /* ASCOT setting ON
             * slave    Bank    Addr    Bit    default     Value          Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   10h     A5h     [0]      8'h01      8'h01      OREG_ITB_GDEQ_EN
             */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x01) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        } else {
            /* ASCOT setting OFF
             * slave    Bank    Addr    Bit    default     Value          Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   10h     A5h     [0]      8'h01      8'h00      OREG_ITB_GDEQ_EN
             */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_7 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_7 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT2_7 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]    8'h00      8'h02      OREG_CHANNEL_WIDTH[2:0]
         */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x02) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        break;

    case sony_cxd_DTV_BW_6_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x1C, 0x00, 0x00, 0x00, 0x00
            };
            /* <Timing Recovery setting>
             * Slave     Bank    Addr   Bit     Default   Value   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   20h     9Fh    [5:0]   8'h15     nomi1   OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   20h     A0h    [7:0]   8'h00     nomi2   OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   20h     A1h    [7:0]   8'h00     nomi3   OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   20h     A2h    [7:0]   8'h00     nomi4   OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   20h     A3h    [7:0]   8'h00     nomi5   OREG_TRCG_NOMINALRATE[7:0]
             * <SLV-T>   27h     7Ah    [3:0]   8'h00     8'h00   OREG_TRCG_LMTVAL0[3:0]
             */

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            /* Set SLV-T Bank : 0x27 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x27) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, 0x00) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* Set SLV-T Bank : 0x10 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /*
         * Filter settings for Sony silicon tuners
         * slave    bank    addr     bit      default    name
         * ------------------------------------------------------------------
         * <SLV-T>   10h     A6h    [7:0]     8'h1E      OREG_ITB_COEF01[7:0]
         * <SLV-T>   10h     A7h    [7:0]     8'h1D      OREG_ITB_COEF02[7:0]
         * <SLV-T>   10h     A8h    [7:0]     8'h29      OREG_ITB_COEF11[7:0]
         * <SLV-T>   10h     A9h    [7:0]     8'hC9      OREG_ITB_COEF12[7:0]
         * <SLV-T>   10h     AAh    [7:0]     8'h2A      OREG_ITB_COEF21[7:0]
         * <SLV-T>   10h     ABh    [7:0]     8'hBA      OREG_ITB_COEF22[7:0]
         * <SLV-T>   10h     ACh    [7:0]     8'h29      OREG_ITB_COEF31[7:0]
         * <SLV-T>   10h     ADh    [7:0]     8'hAD      OREG_ITB_COEF32[7:0]
         * <SLV-T>   10h     AEh    [7:0]     8'h29      OREG_ITB_COEF41[7:0]
         * <SLV-T>   10h     AFh    [7:0]     8'hA4      OREG_ITB_COEF42[7:0]
         * <SLV-T>   10h     B0h    [7:0]     8'h29      OREG_ITB_COEF51[7:0]
         * <SLV-T>   10h     B1h    [7:0]     8'h9A      OREG_ITB_COEF52[7:0]
         * <SLV-T>   10h     B2h    [7:0]     8'h28      OREG_ITB_COEF61[7:0]
         * <SLV-T>   10h     B3h    [7:0]     8'h9E      OREG_ITB_COEF62[7:0]
         */
        if (pDemod->tunerOptimize == sony_cxd_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
            const uint8_t itbCoef[14] = {
            /*  COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62 */
                0x31,  0xA8,  0x29,  0x9B,  0x27,  0x9C,  0x28,  0x9E,  0x29,  0xA4,  0x29,  0xA2,  0x29,  0xA8
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, itbCoef, 14) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            /* ASCOT setting ON
             * slave    Bank    Addr    Bit    default     Value          Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   10h     A5h     [0]      8'h01      8'h01      OREG_ITB_GDEQ_EN
             */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x01) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        } else {
            /* ASCOT setting OFF
             * slave    Bank    Addr    Bit    default     Value          Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   10h     A5h     [0]      8'h01      8'h00      OREG_ITB_GDEQ_EN
             */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_6 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_6 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT2_6 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]     8'h00      8'h04      OREG_CHANNEL_WIDTH[2:0]
         */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x04) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        break;

    case sony_cxd_DTV_BW_5_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x21, 0x99, 0x99, 0x99, 0x99
            };
            /* <Timing Recovery setting>
             * Slave     Bank    Addr   Bit     Default   Value   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   20h     9Fh    [5:0]   8'h15     nomi1   OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   20h     A0h    [7:0]   8'h00     nomi2   OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   20h     A1h    [7:0]   8'h00     nomi3   OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   20h     A2h    [7:0]   8'h00     nomi4   OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   20h     A3h    [7:0]   8'h00     nomi5   OREG_TRCG_NOMINALRATE[7:0]
             * <SLV-T>   27h     7Ah    [3:0]   8'h00     8'h00   OREG_TRCG_LMTVAL0[3:0]
             */

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            /* Set SLV-T Bank : 0x27 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x27) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, 0x00) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* Set SLV-T Bank : 0x10 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /*
         * Filter settings for Sony silicon tuners
         * slave    bank    addr     bit      default    name
         * ------------------------------------------------------------------
         * <SLV-T>   10h     A6h    [7:0]     8'h1E      OREG_ITB_COEF01[7:0]
         * <SLV-T>   10h     A7h    [7:0]     8'h1D      OREG_ITB_COEF02[7:0]
         * <SLV-T>   10h     A8h    [7:0]     8'h29      OREG_ITB_COEF11[7:0]
         * <SLV-T>   10h     A9h    [7:0]     8'hC9      OREG_ITB_COEF12[7:0]
         * <SLV-T>   10h     AAh    [7:0]     8'h2A      OREG_ITB_COEF21[7:0]
         * <SLV-T>   10h     ABh    [7:0]     8'hBA      OREG_ITB_COEF22[7:0]
         * <SLV-T>   10h     ACh    [7:0]     8'h29      OREG_ITB_COEF31[7:0]
         * <SLV-T>   10h     ADh    [7:0]     8'hAD      OREG_ITB_COEF32[7:0]
         * <SLV-T>   10h     AEh    [7:0]     8'h29      OREG_ITB_COEF41[7:0]
         * <SLV-T>   10h     AFh    [7:0]     8'hA4      OREG_ITB_COEF42[7:0]
         * <SLV-T>   10h     B0h    [7:0]     8'h29      OREG_ITB_COEF51[7:0]
         * <SLV-T>   10h     B1h    [7:0]     8'h9A      OREG_ITB_COEF52[7:0]
         * <SLV-T>   10h     B2h    [7:0]     8'h28      OREG_ITB_COEF61[7:0]
         * <SLV-T>   10h     B3h    [7:0]     8'h9E      OREG_ITB_COEF62[7:0]
         */
        if (pDemod->tunerOptimize == sony_cxd_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
            const uint8_t itbCoef[14] = {
            /*  COEF01 COEF02 COEF11 COEF12 COEF21 COEF22 COEF31 COEF32 COEF41 COEF42 COEF51 COEF52 COEF61 COEF62 */
                0x31,  0xA8,  0x29,  0x9B,  0x27,  0x9C,  0x28,  0x9E,  0x29,  0xA4,  0x29,  0xA2,  0x29,  0xA8
            };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA6, itbCoef, 14) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            /* ASCOT setting ON
             * slave    Bank    Addr    Bit    default     Value          Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   10h     A5h     [0]      8'h01      8'h01      OREG_ITB_GDEQ_EN
             */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x01) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        } else {
            /* ASCOT setting OFF
             * slave    Bank    Addr    Bit    default     Value          Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   10h     A5h     [0]      8'h01      8'h00      OREG_ITB_GDEQ_EN
             */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_5 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_5 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT2_5 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]    8'h00      8'h06      OREG_CHANNEL_WIDTH[2:0]
         */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x06) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        break;

    case sony_cxd_DTV_BW_1_7_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x1A, 0x03, 0xE8, 0x8C, 0xB3
            };
            /* <Timing Recovery setting>
             * Slave     Bank    Addr   Bit     Default   Value   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   20h     9Fh    [5:0]   8'h15     nomi1   OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   20h     A0h    [7:0]   8'h00     nomi2   OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   20h     A1h    [7:0]   8'h00     nomi3   OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   20h     A2h    [7:0]   8'h00     nomi4   OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   20h     A3h    [7:0]   8'h00     nomi5   OREG_TRCG_NOMINALRATE[7:0]
             * <SLV-T>   27h     7Ah    [3:0]   8'h00     8'h03   OREG_TRCG_LMTVAL0[3:0]
             */

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            /* Set SLV-T Bank : 0x27 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x27) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7A, 0x03) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* Set SLV-T Bank : 0x10 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* ASCOT setting OFF
         * slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     A5h     [0]      8'h01      8'h00      OREG_ITB_GDEQ_EN
         */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_1_7 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT2_1_7 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT2_1_7 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]    8'h00      8'h03      OREG_CHANNEL_WIDTH[2:0]
         */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x03) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        break;

    default:
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}


static sony_cxd_result_t AT2toAT2(sony_cxd_demod_t * pDemod)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("AT2toAT2");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable TS output */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x01) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set tuner and bandwidth specific settings */
    result = SLtoAT2_BandSetting (pDemod);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t AT2toSL(sony_cxd_demod_t * pDemod)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("AT2toSL");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable TS output */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC3, 0x01) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable Hi-Z setting 1 (TAGC, SAGC, TSVALID, TSSYNC, TSCLK) */
    if (sony_cxd_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x1F, 0x1F) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Enable Hi-Z setting 2 */
    result = sony_cxd_demod_SetTSDataPinHiZ (pDemod, 1);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Cancel DVB-T2 setting
     * slave     Bank    Addr   Bit     default    Value      Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   10h     A5h    [0]       8'h01      8'h01      OREG_ITB_GDEQ_EN
     * <SLV-T>   13h     83h    [7:0]     8'h40      8'h40      OREG_ISIC_POSPROTECT[7:0]
     * <SLV-T>   13h     86h    [7:0]     8'h21      8'h21      OREG_ISIC_CFNRMOFFSET[7:0]
     * <SLV-T>   13h     9Fh    [7:0]     8'hFB      8'hFB      OREG_CFUPONTHRESHOLDMAX[7:0]
     */
    /* Set SLV-T Bank : 0x10 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x01) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x13 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x13) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x83, 0x40) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x86, 0x21) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, 0xFB) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x01) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x49, 0x33) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, 0x21) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Demodulator SW reset */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFE, 0x01) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Disable demodulator clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set tstlv mode to default */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set demodulator mode to default */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x01) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}
