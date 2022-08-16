/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_monitor.h"
#include "sony_tunerdemod_dvbt.h"
#include "sony_tunerdemod_dvbt_monitor.h"
#include "sony_math.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Confirm demodulator lock
*/
static sony_result_t IsTPSLocked (sony_tunerdemod_t * pTunerDemod);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tunerdemod_dvbt_monitor_SyncStat (sony_tunerdemod_t * pTunerDemod,
                                                     uint8_t * pSyncStat,
                                                     uint8_t * pTSLockStat,
                                                     uint8_t * pUnlockDetected)
{
    uint8_t rdata = 0x00;
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_SyncStat");

    if ((!pTunerDemod) || (!pSyncStat) || (!pTSLockStat) || (!pUnlockDetected)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }
    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x0D */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0D) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Slave    Bank    Addr    Bit     Name                Meaning
     * -----------------------------------------------------------------------
     * <SLV-T>   0Dh     10h     [2:0]  IREG_SEQ_OSTATE     0-5:UNLOCK, 6:LOCK
     * <SLV-T>   0Dh     10h     [4]    IEARLY_NOOFDM
     * <SLV-M>   0Dh     10h     [5]    IREG_TSIF_TS_LOCK     0:UNLOCK, 1:LOCK
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, &rdata, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    *pUnlockDetected = (uint8_t)((rdata & 0x10)? 1 : 0);
    *pSyncStat = (uint8_t)(rdata & 0x07);
    *pTSLockStat = (uint8_t)((rdata & 0x20) ? 1 : 0);

    /* Check for valid SyncStat value. */
    if (*pSyncStat == 0x07){
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_SyncStat_sub (sony_tunerdemod_t * pTunerDemod,
                                                         uint8_t * pSyncStat,
                                                         uint8_t * pUnlockDetected)
{
    uint8_t tsLockStat = 0; /* Ignored for sub */

    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_SyncStat_sub");

    if ((!pTunerDemod) || (!pSyncStat) || (!pUnlockDetected)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_dvbt_monitor_SyncStat (pTunerDemod->pDiverSub, pSyncStat, &tsLockStat, pUnlockDetected);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_ModeGuard (sony_tunerdemod_t * pTunerDemod,
                                                      sony_dvbt_mode_t * pMode,
                                                      sony_dvbt_guard_t * pGuard)
{
    uint8_t rdata = 0x00;

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_ModeGuard");

    if ((!pTunerDemod) || (!pMode) || (!pGuard)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* TPS Lock check */
    result = IsTPSLocked (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Check for sub. */
            result = sony_tunerdemod_dvbt_monitor_ModeGuard (pTunerDemod->pDiverSub, pMode, pGuard);
        }

        SONY_TRACE_RETURN (result);
    }

    /* Below registers are valid when IREG_SEQ_OSTATE = 6. */

    /* Set SLV-T Bank : 0x0D */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0D) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* slave    Bank    Addr    Bit              Name                meaning
     * ---------------------------------------------------------------------------------
     * <SLV-T>   0Dh     1Bh     [3:2]        IREG_MODE[1:0]     0:2K-mode, 1:8K-mode
     * <SLV-T>   0Dh     1Bh     [1:0]        IREG_GI[1:0]       0:1/32, 1:1/16, 2:1/8, 3:1/4
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1B, &rdata, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SLVT_UnFreezeReg (pTunerDemod);

    *pMode = (sony_dvbt_mode_t) ((rdata >> 2) & 0x03);
    *pGuard = (sony_dvbt_guard_t) (rdata & 0x03);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_CarrierOffset (sony_tunerdemod_t * pTunerDemod,
                                                          int32_t * pOffset)
{
    uint8_t rdata[4];
    uint32_t ctlVal = 0;

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_CarrierOffset");

    if ((!pTunerDemod) || (!pOffset)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* TPS Lock check */
    result = IsTPSLocked (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x0D */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0D) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /*  slave    Bank    Addr    Bit              Name
     * ---------------------------------------------------------------
     * <SLV-T>   0Dh     1Dh     [4:0]      IREG_CRCG_CTLVAL[28:24]
     * <SLV-T>   0Dh     1Eh     [7:0]      IREG_CRCG_CTLVAL[23:16]
     * <SLV-T>   0Dh     1Fh     [7:0]      IREG_CRCG_CTLVAL[15:8]
     * <SLV-T>   0Dh     20h     [7:0]      IREG_CRCG_CTLVAL[7:0]
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1D, rdata, 4) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SLVT_UnFreezeReg (pTunerDemod);

    /*
     * Carrier Offset [Hz] = -(IREG_CRCG_CTLVAL * (2^-28) * 8 * BW) / (7E-6)
     *
     * Note: (2^-28 * 8 / 7E-6) = 4.257E-3
     * And: 1 / (2^-28 * 8 / 7E-6) = 234.88 = 235
     */
    ctlVal = ((rdata[0] & 0x1F) << 24) | (rdata[1] << 16) | (rdata[2] << 8) | (rdata[3]);
    *pOffset = sony_Convert2SComplement (ctlVal, 29);
    *pOffset = -1 * ((*pOffset) * (uint8_t)pTunerDemod->bandwidth / 235);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_CarrierOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                              int32_t * pOffset)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_CarrierOffset_sub");

    if ((!pTunerDemod) || (!pOffset)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_dvbt_monitor_CarrierOffset (pTunerDemod->pDiverSub, pOffset);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_PreViterbiBER (sony_tunerdemod_t * pTunerDemod,
                                                          uint32_t * pBER)
{
    uint8_t rdata[2];
    uint32_t bitError = 0;
    uint32_t period = 0;

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_PreViterbiBER");

    if ((!pTunerDemod) || (!pBER)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x10) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* slave    Bank    Addr    Bit              Name
     * ------------------------------------------------------------
     * <SLV-M>   10h     39h     [0]          IREG_VBER_ACT
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x39, rdata, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if ((rdata[0] & 0x01) == 0) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);    /* Not ready... */
    }

    /* slave    Bank    Addr    Bit              Name
     * ------------------------------------------------------------
     * <SLV-M>   10h     22h     [7:0]        IREG_VBER_BITECNT[15:8]
     * <SLV-M>   10h     23h     [7:0]        IREG_VBER_BITECNT[7:0]
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x22, rdata, 2) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    bitError = (rdata[0] << 8) | rdata[1];

    /* Read vber_period
     * slave    Bank    Addr    Bit    default          Name
     * ------------------------------------------------------------
     * <SLV-M>   10h     6Fh     [2:0]    3'd1      OREG_VBER_PERIOD_SEL
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x6F, rdata, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SLVT_UnFreezeReg (pTunerDemod);

    period = ((rdata[0] & 0x07) == 0) ? 256 : (0x1000 << (rdata[0] & 0x07));

    if ((period == 0) || (bitError > period)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /*
      BER = bitError * 10000000 / period
          = bitError * 78125 / (period / 128)
          = bitError * 3125 * 25 / (period / 128)
          (Divide in 2 steps to prevent overflow.)
    */
    {
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        div = period / 128;

        Q = (bitError * 3125) / div;
        R = (bitError * 3125) % div;

        R *= 25;
        Q = Q * 25 + R / div;
        R = R % div;

        /* rounding */
        if (R >= div/2) {
            *pBER = Q + 1;
        }
        else {
            *pBER = Q;
        }
    }
    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_PreRSBER (sony_tunerdemod_t * pTunerDemod,
                                                     uint32_t * pBER)
{
    uint8_t rdata[3];
    uint32_t bitError = 0;
    uint32_t periodExp = 0;

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_PreRSBER");

    if ((!pTunerDemod) || (!pBER)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x0D */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0D) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /*   slave    Bank    Addr    Bit              Name
     * ------------------------------------------------------------
     * <SLV-M>   0Dh     15h     [6]          IREG_BERN_VALID
     * <SLV-M>   0Dh     15h     [5:0]        IREG_BERN_BITECNT[21:16]
     * <SLV-M>   0Dh     16h     [7:0]        IREG_BERN_BITECNT[15:8]
     * <SLV-M>   0Dh     17h     [7:0]        IREG_BERN_BITECNT[7:0]
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x15, rdata, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Check IREG_BERN_VALID bit (bit 7) */
    if ((rdata[0] & 0x40) == 0) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);    /* Not ready... */
    }

    bitError = ((rdata[0] & 0x3F) << 16) | (rdata[1] << 8) | rdata[2];

    /* Set SLV-T Bank : 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Read ber_period */
    /* slave    Bank    Addr    Bit    default          Name
     * -------------------------------------------------------------------------
     * <SLV-M>   10h     60h    [4:0]     5'h0B        OREG_BERN_PERIOD[4:0]
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x60, rdata, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    periodExp = (rdata[0] & 0x1F);

    if ((periodExp <= 11) && (bitError > (1U << periodExp) * 204 * 8)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /*
      BER = bitError * 10000000 / (2^N * 204 * 8)
          = bitError * 312500 / (2^N * 51)
          = bitError * 250 * 1250 / (2^N * 51)
          (Divide in 2 steps to prevent overflow.)
    */
    {
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        if (periodExp <= 8) {
            div = (1U << periodExp) * 51;
        }
        else {
            div = (1U << 8) * 51;
        }

        Q = (bitError * 250) / div;
        R = (bitError * 250) % div;

        R *= 1250;
        Q = Q * 1250 + R / div;
        R = R % div;

        if (periodExp > 8) {
            /* rounding */
            *pBER = (Q + (1 << (periodExp - 9))) >> (periodExp - 8);
        } else {
            /* rounding */
            if (R >= div/2) {
                *pBER = Q + 1;
            }
            else {
                *pBER = Q;
            }
        }
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_TPSInfo (sony_tunerdemod_t * pTunerDemod,
                                                    sony_dvbt_tpsinfo_t * pInfo)
{
    uint8_t rdata[7];
    uint8_t cellIDOK = 0;

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_TPSInfo");

    if ((!pTunerDemod) || (!pInfo)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* TPS Lock check */
    result = IsTPSLocked (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Check for sub. */
            result = sony_tunerdemod_dvbt_monitor_TPSInfo (pTunerDemod->pDiverSub, pInfo);
        }

        SONY_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x0D */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0D) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* slave    Bank    Addr    Bit              Name
     * ------------------------------------------------------------
     * <SLV-T>   0Dh     29h     [7:6]       ITPS_CNST[1:0]
     * <SLV-T>   0Dh     29h     [5:3]       ITPS_HIER[2:0]
     * <SLV-T>   0Dh     29h     [2:0]       ITPS_HRATE[2:0]
     * <SLV-T>   0Dh     2Ah     [7:5]       ITPS_LRATE[2:0]
     * <SLV-T>   0Dh     2Ah     [4:3]       ITPS_GI[1:0]
     * <SLV-T>   0Dh     2Ah     [2:1]       ITPS_MODE[1:0]
     * <SLV-T>   0Dh     2Bh     [7:6]       ITPS_FNUM[1:0]
     * <SLV-T>   0Dh     2Bh     [5:0]       ITPS_LENGTH_INDICATOR[5:0]
     * <SLV-T>   0Dh     2Ch     [7:0]       ITPS_CELLID[15:8]
     * <SLV-T>   0Dh     2Dh     [7:0]       ITPS_CELLID[7:0]
     * <SLV-T>   0Dh     2Eh     [5:0]       ITPS_RESERVE_EVEN[5:0]
     * <SLV-T>   0Dh     2Fh     [5:0]       ITPS_RESERVE_ODD[5:0]
     * <SLV-T>   11h     D5h     [0]         ITPS_CELLID_OK
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x29, rdata, 7) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x11 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x11) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xD5, &cellIDOK, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SLVT_UnFreezeReg (pTunerDemod);

    pInfo->constellation = (sony_dvbt_constellation_t) ((rdata[0] >> 6) & 0x03);
    pInfo->hierarchy = (sony_dvbt_hierarchy_t) ((rdata[0] >> 3) & 0x07);
    pInfo->rateHP = (sony_dvbt_coderate_t) (rdata[0] & 0x07);
    pInfo->rateLP = (sony_dvbt_coderate_t) ((rdata[1] >> 5) & 0x07);
    pInfo->guard = (sony_dvbt_guard_t) ((rdata[1] >> 3) & 0x03);
    pInfo->mode = (sony_dvbt_mode_t) ((rdata[1] >> 1) & 0x03);
    pInfo->fnum = (rdata[2] >> 6) & 0x03;
    pInfo->lengthIndicator = rdata[2] & 0x3F;
    pInfo->cellID = (uint16_t) ((rdata[3] << 8) | rdata[4]);
    pInfo->reservedEven = rdata[5] & 0x3F;
    pInfo->reservedOdd = rdata[6] & 0x3F;

    pInfo->cellIDOK = cellIDOK & 0x01;

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_PacketErrorNumber (sony_tunerdemod_t * pTunerDemod,
                                                              uint32_t * pPEN)
{
    uint8_t rdata[3];

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_PacketErrorNumber");

    if ((!pTunerDemod) || (!pPEN)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x0D */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0D) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Read ber_cwrjctcnt
     * slave    Bank    Addr    Bit              Name
     * ------------------------------------------------------------
     * <SLV-M>   0Dh     26h     [0]          IREG_BER_CWRJCTCNT_VALID
     * <SLV-M>   0Dh     27h     [7:0]        IREG_BER_CWRJCTCNT[15:8]
     * <SLV-M>   0Dh     28h     [7:0]        IREG_BER_CWRJCTCNT[7:0]
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x26, rdata, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (!(rdata[0] & 0x01)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    *pPEN = (rdata[1] << 8) | rdata[2];

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_SpectrumSense (sony_tunerdemod_t * pTunerDemod,
                                                          sony_tunerdemod_spectrum_sense_t * pSense)
{
    uint8_t data = 0;

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_SpectrumSense");

    if ((!pTunerDemod) || (!pSense)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* TPS Lock check */
    result = IsTPSLocked (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Check for sub. */
            result = sony_tunerdemod_dvbt_monitor_SpectrumSense (pTunerDemod->pDiverSub, pSense);
        }

        SONY_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x0D */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0D) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* slave    Bank    Addr    Bit              Name                mean
     * ---------------------------------------------------------------------------------
     * <SLV-T>   0Dh     1Ch     [0]        IREG_COSNE_SINV      0:not invert,   1:invert
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1C, &data, sizeof (data)) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SLVT_UnFreezeReg (pTunerDemod);

    *pSense = (data & 0x01) ? SONY_TUNERDEMOD_SPECTRUM_INV : SONY_TUNERDEMOD_SPECTRUM_NORMAL;

    SONY_TRACE_RETURN (result);
}

static sony_result_t dvbt_readSNRReg (sony_tunerdemod_t * pTunerDemod, uint16_t * pRegValue)
{
    uint8_t rdata[2];

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("dvbt_readSNRReg");

    if ((!pTunerDemod) || (!pRegValue)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* TPS Lock check */
    result = IsTPSLocked (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x0D */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0D) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* slave    Bank    Addr    Bit              Name
     * ------------------------------------------------------------
     * <SLV-T>   0Dh     13h     [7:0]        IREG_SNMON_OD[15:8]
     * <SLV-T>   0Dh     14h     [7:0]        IREG_SNMON_OD[7:0]
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x13, rdata, 2) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SLVT_UnFreezeReg (pTunerDemod);

    *pRegValue = (rdata[0] << 8) | rdata[1];

    SONY_TRACE_RETURN (result);
}

static sony_result_t dvbt_calcSNR (sony_tunerdemod_t * pTunerDemod, uint32_t regValue, int32_t * pSNR)
{
    SONY_TRACE_ENTER ("dvbt_calcSNR");

    if ((!pTunerDemod) || (!pSNR)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Confirm valid range, clip as necessary */
    if (regValue == 0) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* IREG_SNMON_OD is clipped at a maximum of 4996.
     *
     * SNR = 10 * log10 (IREG_SNMON_OD / (5350 - IREG_SNMON_OD)) + 28.5
     *     = 10 * (log10(IREG_SNMON_OD) - log10(5350 - IREG_SNMON_OD)) + 28.5
     * sony_log10 returns log10(x) * 100
     * Therefore SNR(dB) * 1000 :
     *     = 10 * 10 * (sony_log10(IREG_SNMON_OD) - sony_log10(5350 - IREG_SNMON_OD) + 28500
     */
    if (regValue > 4996) {
        regValue = 4996;
    }

    *pSNR = 10 * 10 * ((int32_t) sony_math_log10 (regValue) - (int32_t) sony_math_log10 (5350 - regValue));
    *pSNR += 28500;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt_monitor_SNR (sony_tunerdemod_t * pTunerDemod,
                                                int32_t * pSNR)
{
    uint16_t regValue = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_SNR");

    if ((!pTunerDemod) || (!pSNR)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set invalid SNR value. */
    *pSNR = -1000 * 1000;

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SINGLE) {
        result = dvbt_readSNRReg (pTunerDemod, &regValue);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        result = dvbt_calcSNR (pTunerDemod, regValue, pSNR);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    } else {
        int32_t snrMain = 0;
        int32_t snrSub = 0;

        result = sony_tunerdemod_dvbt_monitor_SNR_diver (pTunerDemod, pSNR, &snrMain, &snrSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_SNR_diver (sony_tunerdemod_t * pTunerDemod,
                                                      int32_t * pSNR, int32_t * pSNRMain, int32_t * pSNRSub)
{
    uint16_t regValue = 0;
    uint32_t regValueSum = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_SNR_diver");

    if ((!pTunerDemod) || (!pSNR) || (!pSNRMain) || (!pSNRSub)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set invalid SNR value. */
    *pSNR = -1000 * 1000;
    *pSNRMain = -1000 * 1000;
    *pSNRSub = -1000 * 1000;

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for diver main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Check main IC */
    result = dvbt_readSNRReg (pTunerDemod, &regValue);
    if (result == SONY_RESULT_OK) {
        result = dvbt_calcSNR (pTunerDemod, regValue, pSNRMain);
        if (result != SONY_RESULT_OK) {
            regValue = 0;
        }
    } else if (result == SONY_RESULT_ERROR_HW_STATE) {
        regValue = 0;
    } else {
        /* Other fatal error. */
        SONY_TRACE_RETURN (result);
    }

    regValueSum += regValue;

    /* Check sub IC */
    result = dvbt_readSNRReg (pTunerDemod->pDiverSub, &regValue);
    if (result == SONY_RESULT_OK) {
        result = dvbt_calcSNR (pTunerDemod->pDiverSub, regValue, pSNRSub);
        if (result != SONY_RESULT_OK) {
            regValue = 0;
        }
    } else if (result == SONY_RESULT_ERROR_HW_STATE) {
        regValue = 0;
    } else {
        /* Other fatal error. */
        SONY_TRACE_RETURN (result);
    }

    regValueSum += regValue;

    /* Calculate total SNR */
    result = dvbt_calcSNR (pTunerDemod, regValueSum, pSNR);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_SamplingOffset (sony_tunerdemod_t * pTunerDemod,
                                                           int32_t * pPPM)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_SamplingOffset");

    if ((!pTunerDemod) || (!pPPM)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T*/
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t ctlValReg[5];
        uint8_t nominalRateReg[5];
        uint32_t trlCtlVal = 0;
        uint32_t trcgNominalRate = 0;
        int32_t num;
        int32_t den;
        int8_t diffUpper = 0;

        /* Freeze registers */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* TPS Lock check */
        result = IsTPSLocked (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (result);
        }

        /* slave    Bank    Addr    Bit              name
         * ---------------------------------------------------------------
         * <SLV-T>   0Dh     21h     [6:0]      IREG_TRL_CTLVAL_S[38:32]
         * <SLV-T>   0Dh     22h     [7:0]      IREG_TRL_CTLVAL_S[31:24]
         * <SLV-T>   0Dh     23h     [7:0]      IREG_TRL_CTLVAL_S[23:16]
         * <SLV-T>   0Dh     24h     [7:0]      IREG_TRL_CTLVAL_S[15:8]
         * <SLV-T>   0Dh     25h     [7:0]      IREG_TRL_CTLVAL_S[7:0]
         * <SLV-T>   04h     60h     [6:0]      OREG_TRCG_NOMINALRATE[38:32]
         * <SLV-T>   04h     61h     [7:0]      OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>   04h     62h     [7:0]      OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>   04h     63h     [7:0]      OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>   04h     64h     [7:0]      OREG_TRCG_NOMINALRATE[7:0]
         */
        /* Set SLV-T Bank : 0x0D */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0D) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x21, ctlValReg, sizeof (ctlValReg)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Set SLV-T Bank : 0x04 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x60, nominalRateReg, sizeof (nominalRateReg)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Unfreeze registers. */
        SLVT_UnFreezeReg (pTunerDemod);

        diffUpper = (ctlValReg[0] & 0x7F) - (nominalRateReg[0] & 0x7F);

        /* Confirm offset range */
        if ((diffUpper < -1) || (diffUpper > 1)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* Clock frequency offset(ppm)  = { ( IREG_TRL_CTLVAL_S - OREG_TRCG_NOMINALRATE ) / OREG_TRCG_NOMINALRATE } * 1000000
         * Numerator = IREG_TRL_CTLVAL_S - OREG_TRCG_NOMINALRATE
         * Denominator = OREG_TRCG_NOMINALRATE / 1*10^8
         */

        /* Top 7 bits can be ignored for subtraction as out of range. */
        trlCtlVal = ctlValReg[1] << 24;
        trlCtlVal |= ctlValReg[2] << 16;
        trlCtlVal |= ctlValReg[3] << 8;
        trlCtlVal |= ctlValReg[4];

        trcgNominalRate = nominalRateReg[1] << 24;
        trcgNominalRate |= nominalRateReg[2] << 16;
        trcgNominalRate |= nominalRateReg[3] << 8;
        trcgNominalRate |= nominalRateReg[4];

        /* Shift down 1 bit to avoid overflow in subtraction */
        trlCtlVal >>= 1;
        trcgNominalRate >>= 1;

        if (diffUpper == 1) {
            num = (int32_t)((trlCtlVal + 0x80000000u) - trcgNominalRate);
        } else if (diffUpper == -1) {
            num = -(int32_t)((trcgNominalRate + 0x80000000u) - trlCtlVal);
        } else {
            num = (int32_t)(trlCtlVal - trcgNominalRate);
        }

        /* OREG_TRCG_NOMINALRATE is 39bit therefore:
         * Denominator = (OREG_TRCG_NOMINALRATE [38:8] * 256) / 1*10^8
         *             = (OREG_TRCG_NOMINALRATE [38:8] * 2^8) / (2^8 * 5^8)
         *             = OREG_TRCG_NOMINALRATE [38:8] / 390625
         */
        den = (nominalRateReg[0] & 0x7F) << 24;
        den |= nominalRateReg[1] << 16;
        den |= nominalRateReg[2] << 8;
        den |= nominalRateReg[3];
        den = (den + (390625/2)) / 390625;

        /* Shift down to align with numerator */
        den >>= 1;

        /* Perform calculation */
        if (num >= 0) {
            *pPPM = (num + (den/2)) / den;
        }
        else {
            *pPPM = (num - (den/2)) / den;
        }
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_SamplingOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                               int32_t * pPPM)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_SamplingOffset_sub");

    if ((!pTunerDemod) || (!pPPM)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_dvbt_monitor_SamplingOffset (pTunerDemod->pDiverSub, pPPM);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_Quality (sony_tunerdemod_t * pTunerDemod,
                                               uint8_t * pQuality)
{
    sony_dvbt_tpsinfo_t tps;
    sony_dvbt_profile_t profile = SONY_DVBT_PROFILE_HP;
    uint32_t ber = 0;
    int32_t sn = 0;
    int32_t snRel = 0;
    int32_t berSQI = 0;

    /**
     @brief The list of DVB-T Nordig Profile 1 for Non Hierachical signal
            C/N values in dBx1000.
    */
    static const int32_t nordigNonHDVBTdB1000[3][5] = {
    /*   1/2,   2/3,   3/4,   5/6,   7/8                  */
        {5100,  6900,  7900,  8900,  9700},     /* QPSK   */
        {10800, 13100, 14600, 15600, 16000},    /* 16-QAM */
        {16500, 18700, 20200, 21600, 22500}     /* 64-QAM */
    };

    /**
     @brief The list of DVB-T Nordig Profile 1 for Hierachical signal, HP
            C/N values in dBx1000.
    */
    static const int32_t nordigHierHpDVBTdB1000[3][2][5] = {
          /* alpha = 1                                                */
        { /* 1/2,    2/3,    3/4,    5/6,    7/8                      */
            {9100,   12000,  13600,  15000,  16600},   /* LP = 16-QAM */
            {10900,  14100,  15700,  19400,  20600}    /* LP = 64-QAM */
        },/* alpha = 2                                                */
        { /* 1/2,    2/3,    3/4,    5/6,    7/8                      */
            {6800,   9100,   10400,  11900,  12700},   /* LP = 16-QAM */
            {8500,   11000,  12800,  15000,  16000}    /* LP = 64-QAM */
        },/* alpha = 4                                                */
        { /* 1/2,    2/3,    3/4,    5/6,    7/8                      */
            {5800,   7900,   9100,   10300,  12100},   /* LP = 16-QAM */
            {8000,   9300,   11600,  13000,  12900}    /* LP = 64-QAM */
        }
    };
    /**
     @brief The list of DVB-T Nordig Profile 1 for Hierachical signal, LP
            C/N values in dBx1000.
    */
    static const int32_t nordigHierLpDVBTdB1000[3][2][5] = {
          /* alpha = 1                                           */
        { /* 1/2,    2/3,    3/4,    5/6,    7/8                 */
            {12500,  14300,  15300,  16300,  16900},   /* 16-QAM */
            {16700,  19100,  20900,  22500,  23700}    /* 64-QAM */
        },/* alpha = 2                                           */
        { /* 1/2,    2/3,    3/4,    5/6,    7/8                 */
            {15000,  17200,  18400,  19100,  20100},   /* 16-QAM */
            {18500,  21200,  23600,  24700,  25900}    /* 64-QAM */
        },/* alpha = 4                                           */
        { /* 1/2,    2/3,    3/4,    5/6,    7/8                 */
            {19500,  21400,  22500,  23700,  24700},   /* 16-QAM */
            {21900,  24200,  25600,  26900,  27800}    /* 64-QAM */
        }
    };

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_Quality");

    if ((!pTunerDemod) || (!pQuality)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Monitor TPS for Modulation / Code Rate */
    result = sony_tunerdemod_dvbt_monitor_TPSInfo (pTunerDemod, &tps);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Monitor profile for hierarchical signal*/
    if (tps.hierarchy != SONY_DVBT_HIERARCHY_NON) {
        uint8_t data = 0;
        /* Set SLV-T Bank : 0x10 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x10) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit    default    Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-M>   10h     67h     [0]      8'h00      8'h01       OREG_LPSELECT
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x67, &data, 1) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        profile = ((data & 0x01) == 0x01) ? SONY_DVBT_PROFILE_LP : SONY_DVBT_PROFILE_HP;
    }

    /* Get Pre-RS (Post-Viterbi) BER. */
    result = sony_tunerdemod_dvbt_monitor_PreRSBER (pTunerDemod, &ber);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get SNR value. */
    result = sony_tunerdemod_dvbt_monitor_SNR (pTunerDemod, &sn);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Ensure correct TPS values. */
    if ((tps.constellation >= SONY_DVBT_CONSTELLATION_RESERVED_3) ||
        (tps.rateHP >= SONY_DVBT_CODERATE_RESERVED_5) ||
        (tps.rateLP >= SONY_DVBT_CODERATE_RESERVED_5) ||
        (tps.hierarchy > SONY_DVBT_HIERARCHY_4)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
    }

    /* Hierachical transmission with QPSK modulation is an invalid combination */
    if ((tps.hierarchy != SONY_DVBT_HIERARCHY_NON) && (tps.constellation == SONY_DVBT_CONSTELLATION_QPSK)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
    }

    /* Obtain snRel based on code rate and modulation */
    if (tps.hierarchy == SONY_DVBT_HIERARCHY_NON) {
        snRel = sn - nordigNonHDVBTdB1000[tps.constellation][tps.rateHP];
    }
    else if ( profile == SONY_DVBT_PROFILE_LP ) {
        snRel = sn - nordigHierLpDVBTdB1000[(int32_t)tps.hierarchy-1][(int32_t)tps.constellation-1][tps.rateLP];
    }
    else {
        snRel = sn - nordigHierHpDVBTdB1000[(int32_t)tps.hierarchy-1][(int32_t)tps.constellation-1][tps.rateHP];
    }

    /* BER_SQI is calculated from:
     * if (BER > 10^-3)          : 0
     * if (10^-7 < BER <= 10^-3) : BER_SQI = 20*log10(1/BER) - 40
     * if (BER <= 10^-7)         : BER_SQI = 100
      */
    if (ber > 10000) {
        berSQI = 0;
    }
    else if (ber > 1) {
        /* BER_SQI = 20 * log10(1/BER) - 40
         * BER_SQI = 20 * (log10(1) - log10(BER)) - 40
         *
         * If BER in units of 1e-7
         * BER_SQI = 20 * (log10(1) - (log10(BER) - log10(1e7)) - 40
         *
         * BER_SQI = 20 * (log10(1e7) - log10(BER)) - 40
         * BER_SQI = 20 * (7 - log10 (BER)) - 40
         */
        berSQI = (int32_t) (10 * sony_math_log10 (ber));
        berSQI = 20 * (7 * 1000 - (berSQI)) - 40 * 1000;
    }
    else {
        berSQI = 100 * 1000;
    }

    /* SQI (Signal Quality Indicator) given by:
     * if (C/Nrel < -7dB)         : SQI = 0
     * if (-7dB <= C/Nrel < +3dB) : SQI = (((C/Nrel - 3) / 10) + 1) * BER_SQI
     * if (C/Nrel >= +3dB)        : SQI = BER_SQI
     */
    if (snRel < -7 * 1000) {
        *pQuality = 0;
    }
    else if (snRel < 3 * 1000) {
        int32_t tmpSQI = (((snRel - (3 * 1000)) / 10) + 1000);
        *pQuality = (uint8_t) (((tmpSQI * berSQI) + (1000000/2)) / (1000000)) & 0xFF;
    }
    else {
        *pQuality = (uint8_t) ((berSQI + 500) / 1000);
    }

    /* Clip to 100% */
    if (*pQuality > 100) {
        *pQuality = 100;
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_PER (sony_tunerdemod_t * pTunerDemod,
                                           uint32_t * pPER)
{
    uint32_t packetError = 0;
    uint32_t period = 0;
    uint8_t rdata[3];

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_PER");

    if ((!pTunerDemod) || (!pPER)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /*  slave    Bank    Addr    Bit     Name
     * ---------------------------------------------------------------------------------
     * <SLV-M>   10h     5Ch    [3:0]    OREG_PER_MES[3:0]
     * <SLV-M>   0Dh     18h      [0]    IREG_PER_VALID
     * <SLV-M>   0Dh     19h    [7:0]    IREG_PER_PKTERR[15:8]
     * <SLV-M>   0Dh     1Ah    [7:0]    IREG_PER_PKTERR[7:0]
     */
    /* Set SLV-T Bank : 0x0D */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0D) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x18, rdata, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if ((rdata[0] & 0x01) == 0) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);    /* Not ready... */
    }

    packetError = (rdata[1] << 8) | rdata[2];

    /* Set SLV-T Bank : 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5C, rdata, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    period = 1U << (rdata[0] & 0x0F);

    if ((period == 0) || (packetError > period)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /*
      PER = packetError * 1000000 / period
          = packetError * 1000 * 1000 / period
          (Divide in 2 steps to prevent overflow.)
    */
    {
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        div = period;

        Q = (packetError * 1000) / div;
        R = (packetError * 1000) % div;

        R *= 1000;
        Q = Q * 1000 + R / div;
        R = R % div;

        /* rounding */
        if ((div != 1) && (R >= div/2)) {
            *pPER = Q + 1;
        }
        else {
            *pPER = Q;
        }
    }

    SONY_TRACE_RETURN (result);
}

static sony_result_t dvbt_CalcSSI (sony_tunerdemod_t * pTunerDemod, int32_t rfLevel, uint8_t * pSSI)
{
    sony_dvbt_tpsinfo_t tps;
    int32_t prel;
    int32_t tempSSI = 0;
    sony_result_t result = SONY_RESULT_OK;

    static const int32_t pRefdBm1000[3][5] = {
    /*    1/2,    2/3,    3/4,    5/6,    7/8,               */
        {-93000, -91000, -90000, -89000, -88000}, /* QPSK    */
        {-87000, -85000, -84000, -83000, -82000}, /* 16-QAM  */
        {-82000, -80000, -78000, -77000, -76000}, /* 64-QAM  */
    };

    SONY_TRACE_ENTER ("dvbt_CalcSSI");

    if ((!pTunerDemod) || (!pSSI)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Monitor TPS for Modulation / Code Rate */
    result = sony_tunerdemod_dvbt_monitor_TPSInfo (pTunerDemod, &tps);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Ensure correct TPS values. */
    if ((tps.constellation >= SONY_DVBT_CONSTELLATION_RESERVED_3) || (tps.rateHP >= SONY_DVBT_CODERATE_RESERVED_5)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
    }

    /* prel = prec - pref */
    prel = rfLevel - pRefdBm1000[tps.constellation][tps.rateHP];

    /* SSI (Signal Strength Indicator) is calculated from:
     *
     * if (prel < -15dB)             SSI = 0
     * if (-15dB <= prel < 0dB)       SSI = (2/3) * (prel + 15)
     * if (0dB <= prel < 20dB)        SSI = (4 * prel) + 10
     * if (20dB <= prel < 35dB)       SSI = (2/3) * (prel - 20) + 90
     * if (prel >= 35dB)              SSI = 100
     */
    if (prel < -15000) {
        tempSSI = 0;
    }
    else if (prel < 0) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = ((2 * (prel + 15000)) + 1500) / 3000;
    }
    else if (prel < 20000) {
        /* Note : prel scaled by 10^3 so divide by 10^3 added */
        tempSSI = (((4 * prel) + 500) / 1000) + 10;
    }
    else if (prel < 35000) {
        /* Note : prel and 2/3 scaled by 10^3 so divide by 10^6 added */
        tempSSI = (((2 * (prel - 20000)) + 1500) / 3000) + 90;
    }
    else {
        tempSSI = 100;
    }

    /* Clip value to 100% */
    *pSSI = (tempSSI > 100)? 100 : (uint8_t)tempSSI;

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_SSI (sony_tunerdemod_t * pTunerDemod, uint8_t * pSSI)
{
    int32_t rfLevel = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_SSI");

    if ((!pTunerDemod) || (!pSSI)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get estimated RF Level */
    result = sony_tunerdemod_monitor_RFLevel (pTunerDemod, &rfLevel);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Calculate SSI */
    result = dvbt_CalcSSI (pTunerDemod, rfLevel, pSSI);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt_monitor_SSI_sub (sony_tunerdemod_t * pTunerDemod, uint8_t * pSSI)
{
    int32_t rfLevel = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_SSI_sub");

    if ((!pTunerDemod) || (!pSSI)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT) {
        /* Not DVB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get estimated sub RF Level */
    result = sony_tunerdemod_monitor_RFLevel (pTunerDemod->pDiverSub, &rfLevel);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Calculate SSI */
    result = dvbt_CalcSSI (pTunerDemod, rfLevel, pSSI);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

/*-----------------------------------------------------------------------------
    Static Functions
-----------------------------------------------------------------------------*/
static sony_result_t IsTPSLocked (sony_tunerdemod_t * pTunerDemod)
{
    uint8_t sync = 0;
    uint8_t tslock = 0;
    uint8_t earlyUnlock = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("IsTPSLocked");

    if (!pTunerDemod)
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);

    result = sony_tunerdemod_dvbt_monitor_SyncStat (pTunerDemod, &sync, &tslock, &earlyUnlock);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (sync != 6) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

#undef uint32_t 
#undef int32_t 
#undef int8_t 