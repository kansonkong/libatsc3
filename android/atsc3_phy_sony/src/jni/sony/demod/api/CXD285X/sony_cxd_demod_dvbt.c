/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2017/05/09
  Modification ID : 9849c174faf82fcc958734f152ba29f70274fdec
------------------------------------------------------------------------------*/

#include "sony_cxd_demod_dvbt.h"
#include "sony_cxd_demod_dvbt_monitor.h"

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Configure the demodulator from Sleep to Active for DVB-T
*/
static sony_cxd_result_t SLtoAT(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator for tuner optimisations and bandwidth specific
        settings.
*/
static sony_cxd_result_t SLtoAT_BandSetting(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active DVB-T to Active to DVB-T.  Used
        for changing channel parameters.
*/
static sony_cxd_result_t ATtoAT(sony_cxd_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active DVB-T to Sleep.
*/
static sony_cxd_result_t ATtoSL(sony_cxd_demod_t * pDemod);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd_result_t sony_cxd_demod_dvbt_Tune (sony_cxd_demod_t * pDemod,
                                    sony_cxd_dvbt_tune_param_t * pTuneParam)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt_Tune");

    if ((!pDemod) || (!pTuneParam)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if ((pDemod->state == sony_cxd_DEMOD_STATE_ACTIVE) && (pDemod->system == sony_cxd_DTV_SYSTEM_DVBT)) {
        /* Demodulator Active and set to DVB-T mode */
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = ATtoAT (pDemod);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }
    }
    else if((pDemod->state == sony_cxd_DEMOD_STATE_ACTIVE) && (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT)){
        /* Demodulator Active but not DVB-T mode */
        result = sony_cxd_demod_Sleep (pDemod);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }

        pDemod->system = sony_cxd_DTV_SYSTEM_DVBT;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLtoAT (pDemod);
        if (result != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (result);
        }
    }
    else if (pDemod->state == sony_cxd_DEMOD_STATE_SLEEP) {
        /* Demodulator in Sleep mode */
        pDemod->system = sony_cxd_DTV_SYSTEM_DVBT;
        pDemod->bandwidth = pTuneParam->bandwidth;

        result = SLtoAT (pDemod);
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

sony_cxd_result_t sony_cxd_demod_dvbt_Sleep (sony_cxd_demod_t * pDemod)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt_Sleep");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    result = ATtoSL (pDemod);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt_CheckDemodLock (sony_cxd_demod_t * pDemod,
                                              sony_cxd_demod_lock_result_t * pLock)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    uint8_t sync = 0;
    uint8_t tslock = 0;
    uint8_t unlockDetected = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt_CheckDemodLock");

    if (!pDemod || !pLock) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    *pLock = sony_cxd_DEMOD_LOCK_RESULT_NOTDETECT;

    result = sony_cxd_demod_dvbt_monitor_SyncStat (pDemod, &sync, &tslock, &unlockDetected);
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

sony_cxd_result_t sony_cxd_demod_dvbt_CheckTSLock (sony_cxd_demod_t * pDemod,
                                           sony_cxd_demod_lock_result_t * pLock)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    uint8_t sync = 0;
    uint8_t tslock = 0;
    uint8_t unlockDetected = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt_CheckTSLock");

    if (!pDemod || !pLock) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    *pLock = sony_cxd_DEMOD_LOCK_RESULT_NOTDETECT;

    result = sony_cxd_demod_dvbt_monitor_SyncStat (pDemod, &sync, &tslock, &unlockDetected);
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

sony_cxd_result_t sony_cxd_demod_dvbt_SetProfile (sony_cxd_demod_t * pDemod,
                                          sony_cxd_dvbt_profile_t profile)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt_SetProfile");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != sony_cxd_DEMOD_STATE_SLEEP) && (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x10 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set DVB-T hierarchy setting
     * slave     Bank    Addr    Bit   default    Value    Name
     * -----------------------------------------------------------------
     * <SLV-T>   10h     67h     [0]   8'h00      8'h01    OREG_LPSELECT
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x67, (profile == sony_cxd_DVBT_PROFILE_HP)? 0x00 : 0x01) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt_EchoOptimization (sony_cxd_demod_t * pDemod)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_dvbt_mode_t mode;
    uint8_t rdata = 0;
    uint8_t sync = 0;
    uint8_t tslock = 0;
    uint8_t earlyUnlock = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt_EchoOptimization");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_CXD_FreezeReg (pDemod) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Check for IREG_SEQ_OSTATE >= 2. */
    result = sony_cxd_demod_dvbt_monitor_SyncStat (pDemod, &sync, &tslock, &earlyUnlock);
    if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg (pDemod);
        sony_cxd_TRACE_RETURN (result);
    }

    if (sync < 2) {
        SLVT_CXD_UnFreezeReg (pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
    }

    /* Read mode. It is valid when IREG_SEQ_OSTATE >= 2. */

    /* Set SLV-T Bank : 0x10 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg (pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* slave    Bank    Addr    Bit              Name                meaning
     * ---------------------------------------------------------------------------------
     * <SLV-T>   10h     40h     [3:2]        IREG_MODE[1:0]     0:2K-mode, 1:8K-mode
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x40, &rdata, 1) != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg (pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    SLVT_CXD_UnFreezeReg (pDemod);

    mode = (sony_cxd_dvbt_mode_t) ((rdata >> 2) & 0x03);

    if (mode != sony_cxd_DVBT_MODE_8K) {
        /* Set SLV-T Bank : 0x13 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x13) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        {
            /* Echo performance optimization setting
             * slave     Bank    Addr   Bit     default    Value      Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   13h     9Ch    [3:0]     8'h00      8'h00      OREG_CFUPONTHRESHOLD[11:8]
             * <SLV-T>   13h     9Dh    [7:0]     8'h10      8'h10      OREG_CFUPONTHRESHOLD[7:0]
             * Note: Bank:13h, Addr:9Ch, Bit[7:4] have no functions
             */
            const uint8_t data[] = { 0x00, 0x10 };
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9C, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt_ClearBERSNRHistory (sony_cxd_demod_t * pDemod)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt_ClearBERSNRHistory");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT) {
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
static sony_cxd_result_t SLtoAT(sony_cxd_demod_t * pDemod)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("SLtoAT");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Configure TS clock Mode and Frequency */
    result = sony_cxd_demod_SetTSClockModeAndFreq (pDemod, sony_cxd_DTV_SYSTEM_DVBT);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x01) != sony_cxd_RESULT_OK) {
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

    /* Set SLV-T Bank : 0x10 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (pDemod->tunerOptimize == sony_cxd_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
        /* ASCOT setting ON
         * slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     A5h     [0]      8'h01      8'h01      OREG_ITB_GDEQ_EN
         */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x01) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
    }
    else {
        /* ASCOT setting OFF
         * slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     A5h     [0]      8'h01      8'h00      OREG_ITB_GDEQ_EN
         */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA5, 0x00) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* Set SLV-T Bank : 0x18 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x18) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* FEC Auto Recovery setting
     * slave     Bank    Addr   Bit     default    Value      Name
     * --------------------------------------------------------------------------------------------------------
     * <SLV-T>   18h     31h    [0]     8'h00      8'h01      OREG_FEC_RST_ENABLE2
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x31, 0x01) != sony_cxd_RESULT_OK) {
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

    /* Set SLV-T Bank : 0x11 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x11) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        /* DVB-T 24MHz Xtal setting
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
    result = SLtoAT_BandSetting (pDemod);
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

static sony_cxd_result_t SLtoAT_BandSetting(sony_cxd_demod_t * pDemod)
{
    sony_cxd_TRACE_ENTER ("SLtoAT_BandSetting");

    if (!pDemod) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x13 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x13) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
       /* Echo performance optimization setting
        * slave     Bank    Addr   Bit     default    Value      Name
        * ----------------------------------------------------------------------------------
        * <SLV-T>   13h     9Ch    [3:0]     8'h00      8'h01      OREG_CFUPONTHRESHOLD[11:8]
        * <SLV-T>   13h     9Dh    [7:0]     8'h10      8'h14      OREG_CFUPONTHRESHOLD[7:0]
        * Note: Bank:13h, Addr:9Ch, Bit[7:4] have no functions
        */
        const uint8_t data[] = { 0x01, 0x14 };
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9C, data, sizeof (data)) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* Set SLV-T Bank : 0x10 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
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
             * Slave     Bank    Addr   Bit     Default   Name
             * -----------------------------------------------------------------------
             * <SLV-T>   10h     9Fh    [5:0]   8'h15     OREG_TRCG_NOMINALRATE[37:32]
             * <SLV-T>   10h     A0h    [7:0]   8'h00     OREG_TRCG_NOMINALRATE[31:24]
             * <SLV-T>   10h     A1h    [7:0]   8'h00     OREG_TRCG_NOMINALRATE[23:16]
             * <SLV-T>   10h     A2h    [7:0]   8'h00     OREG_TRCG_NOMINALRATE[15:8]
             * <SLV-T>   10h     A3h    [7:0]   8'h00     OREG_TRCG_NOMINALRATE[7:0]
             */

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT_8 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT_8 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT_8 & 0xFF);
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

        {
            /* Demod core latency setting
             * slave    Bank    Addr    Bit      default    Value    Name
             * --------------------------------------------------------------------
             * <SLV-T>   10h     D9h    [5:0]     8'h15     8'h15    OREG_CDRB_GTDOFST[13:8]
             * <SLV-T>   10h     DAh    [7:0]     8'h28     8'h28    OREG_CDRB_GTDOFST[7:0]
             */
            const uint8_t data[] = { 0x15, 0x28 };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD9, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        {
            /* Notch filter setting
             * slave    Bank    Addr    Bit      default    Value      Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   17h     38h    [1:0]     8'h01      8'h01      OREG_CAS_CCIFLT2_EN_CW2[1:0]
             * <SLV-T>   17h     39h    [1:0]     8'h02      8'h02      OREG_CAS_CWSEQ_ON,OREG_CAS_CWSEQ_ON2
             */
            const uint8_t data[] = { 0x01, 0x02 };

            /* Set SLV-T Bank : 0x17 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x17) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x38, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }
        break;

    case sony_cxd_DTV_BW_7_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x18, 0x00, 0x00, 0x00, 0x00
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

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT_7 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT_7 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT_7 & 0xFF);
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

        {
            /* Demod core latency setting
             * slave    Bank    Addr    Bit      default    Value    Name
             * --------------------------------------------------------------------
             * <SLV-T>   10h     D9h    [5:0]     8'h15     8'h1F    OREG_CDRB_GTDOFST[13:8]
             * <SLV-T>   10h     DAh    [7:0]     8'h28     8'hF8    OREG_CDRB_GTDOFST[7:0]
             */
            const uint8_t data[] = { 0x1F, 0xF8 };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD9, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        {
            /* Notch filter setting
             * slave    Bank    Addr    Bit      default    Value      Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   17h     38h    [1:0]     8'h01      8'h00      OREG_CAS_CCIFLT2_EN_CW2[1:0]
             * <SLV-T>   17h     39h    [1:0]     8'h02      8'h03      OREG_CAS_CWSEQ_ON,OREG_CAS_CWSEQ_ON2
             */
            const uint8_t data[] = { 0x00, 0x03 };

            /* Set SLV-T Bank : 0x17 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x17) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x38, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }
        break;

    case sony_cxd_DTV_BW_6_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x1C, 0x00, 0x00, 0x00, 0x00
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

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT_6 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT_6 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT_6 & 0xFF);
            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        /* System bandwidth setting
         * slave    Bank    Addr    Bit      default    Value      Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   10h     D7h    [2:0]    8'h00      8'h04      OREG_CHANNEL_WIDTH[2:0]
         */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD7, 0x04) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        {
            /* Demod core latency setting
             * slave    Bank    Addr    Bit      default    Value    Name
             * --------------------------------------------------------------------
             * <SLV-T>   10h     D9h    [5:0]     8'h15     8'h25    OREG_CDRB_GTDOFST[13:8]
             * <SLV-T>   10h     DAh    [7:0]     8'h28     8'h4C    OREG_CDRB_GTDOFST[7:0]
             */
            const uint8_t data[2] = { 0x25, 0x4C };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD9, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        {
            /* Notch filter setting
             * slave    Bank    Addr    Bit      default    Value      Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   17h     38h    [1:0]     8'h00      8'h00      OREG_CAS_CCIFLT2_EN_CW2[1:0]
             * <SLV-T>   17h     39h    [1:0]     8'h03      8'h03      OREG_CAS_CWSEQ_ON,OREG_CAS_CWSEQ_ON2
             */
            const uint8_t data[] = { 0x00, 0x03 };

            /* Set SLV-T Bank : 0x17 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x17) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x38, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }
        break;

    case sony_cxd_DTV_BW_5_MHZ:
        {
            const uint8_t nominalRate[5] = {
                /* TRCG Nominal Rate [37:0] */
                0x21, 0x99, 0x99, 0x99, 0x99
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

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRate, 5) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
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
            data[0] = (uint8_t) ((pDemod->iffreqConfig.configDVBT_5 >> 16) & 0xFF);
            data[1] = (uint8_t) ((pDemod->iffreqConfig.configDVBT_5 >> 8) & 0xFF);
            data[2] = (uint8_t) (pDemod->iffreqConfig.configDVBT_5 & 0xFF);
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

        {
            /* Demod core latency setting
             * slave    Bank    Addr    Bit      default    Value    Name
             * --------------------------------------------------------------------
             * <SLV-T>   10h     D9h    [5:0]     8'h15     8'h2C    OREG_CDRB_GTDOFST[13:8]
             * <SLV-T>   10h     DAh    [7:0]     8'h28     8'hC2    OREG_CDRB_GTDOFST[7:0]
             */
            const uint8_t data[2] = { 0x2C, 0xC2 };

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD9, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

        {
            /* Notch filter setting
             * slave    Bank    Addr    Bit      default    Value      Name
             * ----------------------------------------------------------------------------------
             * <SLV-T>   17h     38h    [1:0]     8'h00      8'h00      OREG_CAS_CCIFLT2_EN_CW2[1:0]
             * <SLV-T>   17h     39h    [1:0]     8'h03      8'h03      OREG_CAS_CWSEQ_ON,OREG_CAS_CWSEQ_ON2
             */
            const uint8_t data[] = { 0x00, 0x03 };

            /* Set SLV-T Bank : 0x17 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x17) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x38, data, sizeof (data)) != sony_cxd_RESULT_OK) {
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }
        break;

    default:
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

static sony_cxd_result_t ATtoAT(sony_cxd_demod_t * pDemod)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("ATtoAT");

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
    result = SLtoAT_BandSetting (pDemod);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    sony_cxd_TRACE_RETURN (result);
}

static sony_cxd_result_t ATtoSL(sony_cxd_demod_t * pDemod)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("ATtoSL");

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

    /* Cancel DVB-T Demod parameter setting
     * slave     Bank    Addr   Bit     default    Value      Name
     * ----------------------------------------------------------------------------------
     * <SLV-T>   17h     38h    [1:0]     8'h01      8'h01      OREG_CAS_CCIFLT2_EN_CW2[1:0]
     * <SLV-T>   17h     39h    [1:0]     8'h02      8'h02      OREG_CAS_CWSEQ_ON, OREG_CAS_CWSEQ_ON2
     * <SLV-T>   18h     31h     [0]      8'h00      8'h00      OREG_FEC_RST_ENABLE2
     */

    /* Set SLV-T Bank : 0x17 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x17) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        uint8_t data[] = { 0x01, 0x02 };

        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x38, data, 2) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* Set SLV-T Bank : 0x18 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x18) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x31, 0x00) != sony_cxd_RESULT_OK) {
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
