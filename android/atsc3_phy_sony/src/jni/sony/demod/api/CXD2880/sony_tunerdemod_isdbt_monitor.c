/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2016/03/01
  Modification ID : 821957233ed7087a9a4711da5ab28f0c008ce6a5
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_isdbt.h"
#include "sony_tunerdemod_isdbt_monitor.h"
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
static sony_result_t IsDmdLocked (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Covert TMCC raw data to TMCC info structure.
*/
static sony_result_t setTmccInfo (uint8_t* pData, sony_isdbt_tmcc_info_t* pTmccInfo);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tunerdemod_isdbt_monitor_SyncStat (sony_tunerdemod_t * pTunerDemod,
                                                      uint8_t * pDmdLockStat,
                                                      uint8_t * pTSLockStat,
                                                      uint8_t * pUnlockDetected)
{
    uint8_t rdata = 0x00;
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_SyncStat");

    if ((!pTunerDemod) || (!pDmdLockStat) || (!pTSLockStat) || (!pUnlockDetected)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x0E */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0E) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Slave    Bank    Addr    Bit     Name                Meaning
     * -----------------------------------------------------------------------
     * <SLV-T>   0Eh     10h     [0]          IREG_DMDLOCK        0:UNLOCK, 1:LOCK
     * <SLV-M>   0Eh     10h     [5]          ITSLOCK             0:UNLOCK, 1:LOCK
     * <SLV-T>   0Eh     10h     [4]          IEARLY_NOOFDM       0:Detection underway or ISDB-T signal may exist
     *                                                            1:No ISDB-T signal in the receiving channel
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, &rdata, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    *pUnlockDetected = (uint8_t)((rdata & 0x10)? 1 : 0);
    *pDmdLockStat = (uint8_t)((rdata & 0x01) ? 1 : 0);
    *pTSLockStat = (uint8_t)((rdata & 0x20) ? 1 : 0);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_SyncStat_sub (sony_tunerdemod_t * pTunerDemod,
                                                          uint8_t * pDmdLockStat,
                                                          uint8_t * pUnlockDetected)
{
    uint8_t tsLockStat = 0; /* Ignored for sub */

    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_SyncStat_sub");

    if ((!pTunerDemod) || (!pDmdLockStat) || (!pUnlockDetected)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_isdbt_monitor_SyncStat (pTunerDemod->pDiverSub, pDmdLockStat, &tsLockStat, pUnlockDetected);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_ModeGuard (sony_tunerdemod_t * pTunerDemod,
                                                       sony_isdbt_mode_t * pMode,
                                                       sony_isdbt_guard_t * pGuard)
{
    uint8_t rdata = 0x00;

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_ModeGuard");

    if ((!pTunerDemod) || (!pMode) || (!pGuard)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Check Demod Lock. */
    result = IsDmdLocked(pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Check for sub. */
            result = sony_tunerdemod_isdbt_monitor_ModeGuard (pTunerDemod->pDiverSub, pMode, pGuard);
        }

        SONY_TRACE_RETURN(result);
    }

    /* Below registers are valid when IREG_DMDLOCK==1. */

    /* Set SLV-T Bank : 0x0E */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0E) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* ISDB-T MODE/GUARD estimation result can read by the registers.
     *
     *    slave    Bank    Addr    Bit           Name               Meaning
     *   ---------------------------------------------------------------------------------
     *   <SLV-T>   0Eh     24h     [3:2]        IREG_MODE[1:0]     0:Mode1, 1:Mode3, 2:Mode2
     *   <SLV-T>   0Eh     24h     [1:0]        IREG_GI[1:0]       0:1/32, 1:1/16, 2:1/8, 3:1/4
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x24, &rdata, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SLVT_UnFreezeReg (pTunerDemod);

    switch ((rdata >> 2) & 0x03) {
    case 0:
        *pMode = SONY_ISDBT_MODE_1;
        break;
    case 1:
        *pMode = SONY_ISDBT_MODE_3;
        break;
    case 2:
        *pMode = SONY_ISDBT_MODE_2;
        break;
    default:
        *pMode = SONY_ISDBT_MODE_UNKNOWN;
        break;
    }

    *pGuard =( sony_isdbt_guard_t)(rdata & 0x03);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_CarrierOffset (sony_tunerdemod_t * pTunerDemod,
                                                           int32_t * pOffset)
{
    uint8_t rdata[4] = {0x00, 0x00, 0x00, 0x00};
    uint32_t iregCrcgCtrlval = 0;

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_CarrierOffset");

    if ((!pTunerDemod) || (!pOffset)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Dmd Lock check */
    result = IsDmdLocked (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x0E */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0E) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /*
     * slave    Bank    Addr    Bit              Signal name
     * ---------------------------------------------------------------
     * <SLV-T>   0Eh     26h     [4:0]      IREG_CRCG_CTLVAL[28:24]
     * <SLV-T>   0Eh     27h     [7:0]      IREG_CRCG_CTLVAL[23:16]
     * <SLV-T>   0Eh     28h     [7:0]      IREG_CRCG_CTLVAL[15:8]
     * <SLV-T>   0Eh     29h     [7:0]      IREG_CRCG_CTLVAL[7:0]
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x26, rdata, 4) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SLVT_UnFreezeReg (pTunerDemod);

    iregCrcgCtrlval = rdata[0] & 0x1F;
    iregCrcgCtrlval = (iregCrcgCtrlval << 8) + rdata[1];
    iregCrcgCtrlval = (iregCrcgCtrlval << 8) + rdata[2];
    iregCrcgCtrlval = (iregCrcgCtrlval << 8) + rdata[3];

    *pOffset = sony_Convert2SComplement(iregCrcgCtrlval, 29);

    /*
     * Carrier Frequency Offset [Hz] = -( ( IREG_CRCG_CTLVAL / 2^28 ) * 8192 ) / ( 1008 * 10^-6 )   ---6MHz BW
     *                               = -( IREG_CRCG_CTLVAL * 8 / (2^18 * 1008 / 10^6) )
     *                               = -( IREG_CRCG_CTLVAL * 8 / 264.241 )
     *
     * Carrier Frequency Offset [Hz] = -( ( IREG_CRCG_CTLVAL / 2^28 ) * 8192 ) / (  882 * 10^-6 )   ---7MHz BW
     *                               = -( IREG_CRCG_CTLVAL * 8 / (2^18 * 882 / 10^6) )
     *                               = -( IREG_CRCG_CTLVAL * 8 / 231.211 )
     *
     * Carrier Frequency Offset [Hz] = -( ( IREG_CRCG_CTLVAL / 2^28 ) * 8192 ) / (  756 * 10^-6 )   ---8MHz BW
     *                               = -( IREG_CRCG_CTLVAL * 8 / (2^18 * 756 / 10^6) )
     *                               = -( IREG_CRCG_CTLVAL * 8 / 198.181 )
     */
    switch (pTunerDemod->bandwidth) {
    case SONY_DTV_BW_6_MHZ:
        *pOffset = -1 * ((*pOffset) * 8 / 264);
        break;
    case SONY_DTV_BW_7_MHZ:
        *pOffset = -1 * ((*pOffset) * 8 / 231);
        break;
    case SONY_DTV_BW_8_MHZ:
        *pOffset = -1 * ((*pOffset) * 8 / 198);
        break;
    default:
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_CarrierOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                               int32_t * pOffset)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_CarrierOffset_sub");

    if ((!pTunerDemod) || (!pOffset)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_isdbt_monitor_CarrierOffset (pTunerDemod->pDiverSub, pOffset);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_PreRSBER (sony_tunerdemod_t * pTunerDemod,
                                                      uint32_t * pBERA, uint32_t * pBERB, uint32_t * pBERC)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t dataPacketNum[2];
    uint8_t dataBitError[9];
    uint32_t bitError[3];
    uint32_t packetNum;
    uint32_t ber[3];
    int i = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_PreRSBER");

    if ((!pTunerDemod) || (!pBERA) || (!pBERB) || (!pBERC)) {
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

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Dmd Lock check */
    result = IsDmdLocked (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Check for sub. */
            result = IsDmdLocked (pTunerDemod->pDiverSub);
            if (result != SONY_RESULT_OK) {
                SLVT_UnFreezeReg (pTunerDemod);
                SONY_TRACE_RETURN (result);
            }
        } else {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (result);
        }
    }

    /* Set SLV-T Bank : 0x0E */
    result = pTunerDemod->pRegio->WriteOneRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0E);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN(result);
    }

    /*
     *  slave     Bank   Addr    Bit          Name
     * ------------------------------------------------------------
     * <SLV-M>    0Eh    15h     [5:0]    IBER_BENUM_RSA[21:16]
     * <SLV-M>    0Eh    16h     [7:0]    IBER_BENUM_RSA[15:8]
     * <SLV-M>    0Eh    17h     [7:0]    IBER_BENUM_RSA[7:0]
     * <SLV-M>    0Eh    18h     [5:0]    IBER_BENUM_RSB[21:16]
     * <SLV-M>    0Eh    19h     [7:0]    IBER_BENUM_RSB[15:8]
     * <SLV-M>    0Eh    1Ah     [7:0]    IBER_BENUM_RSB[7:0]
     * <SLV-M>    0Eh    1Bh     [5:0]    IBER_BENUM_RSC[21:16]
     * <SLV-M>    0Eh    1Ch     [7:0]    IBER_BENUM_RSC[15:8]
     * <SLV-M>    0Eh    1Dh     [7:0]    IBER_BENUM_RSC[7:0]
     */

    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x15, dataBitError, 9) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SLVT_UnFreezeReg(pTunerDemod);

    /* Set SLV-T Bank : 0x60 */
    result = pTunerDemod->pRegio->WriteOneRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x60);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN(result);
    }

    /*
     *  slave     Bank   Addr    Bit          Name
     * ------------------------------------------------------------
     * <SLV-M>    60h    5Bh     [6:0]    OBER_CDUR_RSA[14:8]
     * <SLV-M>    60h    5Ch     [7:0]    OBER_CDUR_RSA[7:0]
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5B, dataPacketNum, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    packetNum = ((uint32_t)dataPacketNum[0] << 8) | dataPacketNum[1];
    if (packetNum == 0) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    for (i = 0; i < 3; i++) {
        bitError[i] = ((uint32_t)(dataBitError[0 + (3 * i)] & 0x7F) << 16) | ((uint32_t)dataBitError[1 + (3 * i)] << 8) | dataBitError[2 + (3 * i)];

        if (bitError[i] / 8 / 204 > packetNum) {
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
        }
    }

    /*
     *   BER = (bitError * 10000000) / (packetNum * 8 * 204)
     *       = (bitError * 312500) / (packetNum * 51)
     *       = (bitError * 250 * 1250) / (packetNum * 51)
     */

    for (i = 0; i < 3; i++) {
        uint32_t Div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        Div = packetNum * 51;

        Q = (bitError[i] * 250) / Div;
        R = (bitError[i] * 250) % Div;

        R *= 1250;
        Q = Q * 1250 + R / Div;
        R = R % Div;

        if (R >= (Div/2)) {
            ber[i] = Q + 1;
        } else {
            ber[i] = Q;
        }
    }

    *pBERA = ber[0];
    *pBERB = ber[1];
    *pBERC = ber[2];

    SONY_TRACE_RETURN(result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_PacketErrorNumber (sony_tunerdemod_t * pTunerDemod,
                                                               uint32_t * pPENA, uint32_t * pPENB, uint32_t * pPENC)
{
    uint8_t rdata[7];

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_PacketErrorNumber");

    if ((!pTunerDemod) || (!pPENA) || (!pPENB) || (!pPENC)) {
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

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x0E */
    result = pTunerDemod->pRegio->WriteOneRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0E);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN(result);
    }
    /*
     * slave    Bank    Addr    Bit           Name
     * ------------------------------------------------------------
     * <SLV-M>   0Eh     2Ah     [0]      IREG_CWRJCTCNT_VALID
     * <SLV-M>   0Eh     2Bh     [7:0]    ICWRJCTCNT_A[15:8]
     * <SLV-M>   0Eh     2Ch     [7:0]    ICWRJCTCNT_A[7:0]
     * <SLV-M>   0Eh     2Dh     [7:0]    ICWRJCTCNT_B[15:8]
     * <SLV-M>   0Eh     2Eh     [7:0]    ICWRJCTCNT_B[7:0]
     * <SLV-M>   0Eh     2Fh     [7:0]    ICWRJCTCNT_C[15:8]
     * <SLV-M>   0Eh     30h     [7:0]    ICWRJCTCNT_C[7:0]
     */
    result = pTunerDemod->pRegio->ReadRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x2A, rdata, 7);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN(result);
    }

    if ((rdata[0] & 0x01) == 0) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    *pPENA = (uint32_t)((rdata[1] << 8) | rdata[2]);
    *pPENB = (uint32_t)((rdata[3] << 8) | rdata[4]);
    *pPENC = (uint32_t)((rdata[5] << 8) | rdata[6]);

    SONY_TRACE_RETURN(result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_SpectrumSense (sony_tunerdemod_t * pTunerDemod,
                                                           sony_tunerdemod_spectrum_sense_t * pSense)
{
    uint8_t data = 0;

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_SpectrumSense");

    if ((!pTunerDemod) || (!pSense)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Demod Lock check */
    result = IsDmdLocked (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Check for sub. */
            result = sony_tunerdemod_isdbt_monitor_SpectrumSense (pTunerDemod->pDiverSub, pSense);
        }

        SONY_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x0E */
    result = pTunerDemod->pRegio->WriteOneRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0E);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN(result);
    }

    /*
     * slave    Bank    Addr    Bit          Signal name                Meaning
     * ---------------------------------------------------------------------------------
     * <SLV-T>   0Eh     25h     [0]          IREG_COSNE_SINV    0:Not inverted,  1:Inverted
     */
    result = pTunerDemod->pRegio->ReadRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x25, &data, 1);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN(result);
    }

    SLVT_UnFreezeReg(pTunerDemod);

    *pSense = (data & 0x01) ? SONY_TUNERDEMOD_SPECTRUM_INV : SONY_TUNERDEMOD_SPECTRUM_NORMAL;

    SONY_TRACE_RETURN(result);
}

static sony_result_t isdbt_readSNRReg (sony_tunerdemod_t * pTunerDemod, uint16_t * pRegValue)
{
    uint8_t rdata[2] = {0x00, 0x00};

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("isdbt_readSNRReg");

    if ((!pTunerDemod) || (!pRegValue)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Demod Lock check */
    result = IsDmdLocked (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x0E */
    result = pTunerDemod->pRegio->WriteOneRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0E);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (result);
    }

    /*
     * slave    Bank    Addr    Bit              Name
     * ------------------------------------------------------------
     * <SLV-T>   0Eh     13h     [7:0]        IREG_SNMON_OD[15:8]
     * <SLV-T>   0Eh     14h     [7:0]        IREG_SNMON_OD[7:0]
     */
    result = pTunerDemod->pRegio->ReadRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x13, rdata, 2);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (result);
    }

    SLVT_UnFreezeReg(pTunerDemod);

    *pRegValue = (rdata[0] << 8) | rdata[1];

    SONY_TRACE_RETURN (result);
}

static sony_result_t isdbt_calcSNR (sony_tunerdemod_t * pTunerDemod, uint32_t regValue, int32_t * pSNR)
{
    SONY_TRACE_ENTER ("isdbt_calcSNR");

    if ((!pTunerDemod) || (!pSNR)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (regValue == 0) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    /*
     *
     *  SNR[dB] = 10 * log10 (IREG_SNMON_OD / 8 )
     *          = 10 * (log10(IREG_SNMON_OD) - log10(8))
     *           sony_log10 returns log10(x) * 100
     *           Therefore SNR(dB) * 1000 :
     *                   = 10 * 10 * (sony_log10(IREG_SNMON_OD) - sony_log10(8))
     *                   = 10 * 10 * sony_log10(IREG_SNMON_OD) - 9031
     */

    *pSNR = 100 * (int32_t)sony_math_log10(regValue) - 9031;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbt_monitor_SNR (sony_tunerdemod_t * pTunerDemod,
                                                int32_t * pSNR)
{
    uint16_t regValue = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_SNR");

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

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SINGLE) {
        result = isdbt_readSNRReg (pTunerDemod, &regValue);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        result = isdbt_calcSNR (pTunerDemod, regValue, pSNR);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    } else {
        int32_t snrMain = 0;
        int32_t snrSub = 0;

        result = sony_tunerdemod_isdbt_monitor_SNR_diver (pTunerDemod, pSNR, &snrMain, &snrSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_SNR_diver (sony_tunerdemod_t * pTunerDemod,
                                                      int32_t * pSNR, int32_t * pSNRMain, int32_t * pSNRSub)
{
    uint16_t regValue = 0;
    uint32_t regValueSum = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_SNR_diver");

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

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Check main IC */
    result = isdbt_readSNRReg (pTunerDemod, &regValue);
    if (result == SONY_RESULT_OK) {
        result = isdbt_calcSNR (pTunerDemod, regValue, pSNRMain);
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
    result = isdbt_readSNRReg (pTunerDemod->pDiverSub, &regValue);
    if (result == SONY_RESULT_OK) {
        result = isdbt_calcSNR (pTunerDemod->pDiverSub, regValue, pSNRSub);
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
    result = isdbt_calcSNR (pTunerDemod, regValueSum, pSNR);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_SamplingOffset (sony_tunerdemod_t * pTunerDemod,
                                                            int32_t * pPPM)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_SamplingOffset");

    if ((!pTunerDemod) || (!pPPM)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t ctlValReg[5];
        uint8_t nominalRateReg[5];
        uint32_t trlCtlVal = 0;
        uint32_t trcgNominalRate = 0;
        int32_t num = 0;
        int32_t den = 0;
        int8_t diffUpper = 0;

        /* Freeze registers */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Demod Lock check */
        result = IsDmdLocked (pTunerDemod);
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

        diffUpper = (int8_t)((ctlValReg[0] & 0x7F) - (nominalRateReg[0] & 0x7F));

        /* Confirm offset range */
        if ((diffUpper < -1) || (diffUpper > 1)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* Clock frequency offset(ppm)  = { ( IREG_TRL_CTLVAL_S - OREG_TRCG_NOMINALRATE ) / OREG_TRCG_NOMINALRATE } * 100000000
                                        = ( IREG_TRL_CTLVAL_S - OREG_TRCG_NOMINALRATE ) * 10000000 / OREG_TRCG_NOMINALRATE [38:8] * 256
                                        = ( IREG_TRL_CTLVAL_S - OREG_TRCG_NOMINALRATE ) * 390625 / OREG_TRCG_NOMINALRATE [38:8]
         * Numerator = IREG_TRL_CTLVAL_S - OREG_TRCG_NOMINALRATE
         * Denominator = OREG_TRCG_NOMINALRATE / 1*10^8
         */

        /* Top 7 bits can be ignored for subtraction as out of range. */
        trlCtlVal =  (uint32_t)ctlValReg[1] << 24;
        trlCtlVal |= (uint32_t)ctlValReg[2] << 16;
        trlCtlVal |= (uint32_t)ctlValReg[3] << 8;
        trlCtlVal |= (uint32_t)ctlValReg[4];

        trcgNominalRate  = (uint32_t)nominalRateReg[1] << 24;
        trcgNominalRate |= (uint32_t)nominalRateReg[2] << 16;
        trcgNominalRate |= (uint32_t)nominalRateReg[3] << 8;
        trcgNominalRate |= (uint32_t)nominalRateReg[4];

        /* Shift down 1 bit to avoid overflow in subtraction */
        trlCtlVal >>= 1;
        trcgNominalRate >>= 1;

        if (diffUpper == 1) {
            if (trlCtlVal > trcgNominalRate) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
            }
            else {
                num = (int32_t)((trlCtlVal + 0x80000000UL) - trcgNominalRate);
            }
        } else if (diffUpper == -1) {
            if (trcgNominalRate > trlCtlVal) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
            }
            else {
                num = -(int32_t)((trcgNominalRate + 0x80000000UL) - trlCtlVal);
            }
        } else {
            num = (int32_t)(trlCtlVal - trcgNominalRate);
        }

        /* OREG_TRCG_NOMINALRATE is 39bit therefore:
         * Denominator = (OREG_TRCG_NOMINALRATE [38:8] * 256) / 1*10^8
         *             = (OREG_TRCG_NOMINALRATE [38:8] * 2^8) / (2^8 * 5^8)
         *             = OREG_TRCG_NOMINALRATE [38:8] / 390625
         */
        den =  (int32_t)((((uint32_t)nominalRateReg[0] & 0x7F) << 24) |
                          ((uint32_t)nominalRateReg[1] << 16)        |
                          ((uint32_t)nominalRateReg[2] << 8)         |
                          ((uint32_t)nominalRateReg[3]));
        den = (den + (390625/2)) / 390625;

        /* Shift down to align with numerator */
        den /= 2;

        if (den == 0) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

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

sony_result_t sony_tunerdemod_isdbt_monitor_SamplingOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                                int32_t * pPPM)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_SamplingOffset_sub");

    if ((!pTunerDemod) || (!pPPM)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_isdbt_monitor_SamplingOffset (pTunerDemod->pDiverSub, pPPM);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_PER (sony_tunerdemod_t * pTunerDemod,
                                                 uint32_t * pPERA, uint32_t * pPERB, uint32_t * pPERC)
{
    uint8_t dataPacketError[6];
    uint8_t dataPacketNum[2];
    uint32_t packetError[3];
    uint32_t packetNum = 0;
    uint32_t per[3];
    int i = 0;

    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_PER");

    if ((!pTunerDemod) || (!pPERA) || (!pPERB) || (!pPERC)) {
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

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    result = IsDmdLocked (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Check for sub. */
            result = IsDmdLocked (pTunerDemod->pDiverSub);
            if (result != SONY_RESULT_OK) {
                SLVT_UnFreezeReg (pTunerDemod);
                SONY_TRACE_RETURN (result);
            }
        } else {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (result);
        }
    }

    /* Set SLV-T Bank : 0x0E */
    result = pTunerDemod->pRegio->WriteOneRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0E);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN(result);
    }

    /*
     *  slave     Bank   Addr    Bit          Name
     * ------------------------------------------------------------
     * <SLV-M>    0Eh    1Eh     [6:0]    IBER_PENUM_RSA[14:8]
     * <SLV-M>    0Eh    1Fh     [7:0]    IBER_PENUM_RSA[7:0]
     * <SLV-M>    0Eh    20h     [6:0]    IBER_PENUM_RSB[14:8]
     * <SLV-M>    0Eh    21h     [7:0]    IBER_PENUM_RSB[7:0]
     * <SLV-M>    0Eh    22h     [6:0]    IBER_PENUM_RSC[14:8]
     * <SLV-M>    0Eh    23h     [7:0]    IBER_PENUM_RSC[7:0]
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1E, dataPacketError, 6) != SONY_RESULT_OK){
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SLVT_UnFreezeReg(pTunerDemod);

    /* Set SLV-T Bank : 0x60 */
    result = pTunerDemod->pRegio->WriteOneRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x60);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN(result);
    }
    /*
     *  slave     Bank   Addr    Bit          Name
     * ------------------------------------------------------------
     * <SLV-M>    60h    5Bh     [6:0]    OBER_CDUR_RSA[14:8]
     * <SLV-M>    60h    5Ch     [7:0]    OBER_CDUR_RSA[7:0]
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5B, dataPacketNum, 2) != SONY_RESULT_OK){
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    packetNum = ((uint32_t)dataPacketNum[0] << 8) + dataPacketNum[1];
    if (packetNum == 0) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }

    for (i = 0; i < 3; i++) {
        packetError[i] = ((uint32_t)dataPacketError[0 + (2 * i)] << 8) + dataPacketError[1 + (2 * i)];

        if (packetError[i] > packetNum) {
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
        }
    }

    /*
      PER = packetError * 1000000 / packetNum
          = packetError * 1000 * 1000 / packetNum
     */
    for (i = 0; i < 3; i++) {
        uint32_t Div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        Div = packetNum;
        Q = (packetError[i] * 1000) / Div;
        R = (packetError[i] * 1000) % Div;

        R *= 1000;
        Q = Q * 1000 + R / Div;
        R = R % Div;

        if ((Div != 1) && (R >= (Div / 2))) {
            per[i] = Q + 1;
        } else {
            per[i] = Q;
        }
    }

    *pPERA = per[0];
    *pPERB = per[1];
    *pPERC = per[2];

    SONY_TRACE_RETURN(result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_TSRate (sony_tunerdemod_t * pTunerDemod,
                                                    uint32_t * pTSRateKbpsA, uint32_t * pTSRateKbpsB, uint32_t * pTSRateKbpsC)
{
    /* Data rate [kbps x 100] for 1 segment. (from STD-B31) */
    /* Modulation x Code Rate x Guard Interval */
    const uint32_t dataRate1Seg6MHzBW[3][5][4] = {
            /* 1/32    1/16    1/8     1/4 */
        {
            /* DQPSK, QPSK */
            { 34043,  33042,  31206,  28085}, /* 1/2 */
            { 45391,  44056,  41608,  37447}, /* 2/3 */
            { 51065,  49563,  46809,  42128}, /* 3/4 */
            { 56739,  55070,  52010,  46809}, /* 5/6 */
            { 59576,  57823,  54611,  49150}  /* 7/8 */
        },
        {
            /* 16QAM */
            { 68087,  66084,  62413,  56171}, /* 1/2 */
            { 90782,  88112,  83217,  74895}, /* 2/3 */
            {102130,  99126,  93619,  84257}, /* 3/4 */
            {113478, 110140, 104021,  93619}, /* 5/6 */
            {119152, 115647, 109222,  98300}  /* 7/8 */
        },
        {
            /* 64QAM */
            {102130,  99126,  93619,  84257}, /* 1/2 */
            {136174, 132168, 124826, 112343}, /* 2/3 */
            {153195, 148690, 140429, 126386}, /* 3/4 */
            {170217, 165211, 156032, 140429}, /* 5/6 */
            {178728, 173471, 163834, 147450}  /* 7/8 */
        }
    };

    const uint32_t dataRate1Seg7MHzBW[3][5][4] = {
            /* 1/32    1/16    1/8     1/4 */
        {
            /* DQPSK, QPSK */
            { 39717,  38549,  36407,  32766}, /* 1/2 */
            { 52956,  51399,  48543,  43689}, /* 2/3 */
            { 59576,  57823,  54611,  49150}, /* 3/4 */
            { 66195,  64248,  60679,  54611}, /* 5/6 */
            { 69505,  67461,  63713,  57342}  /* 7/8 */
        },
        {
            /* 16QAM */
            { 79434,  77098,  72815,  65533}, /* 1/2 */
            {105913, 102798,  97087,  87378}, /* 2/3 */
            {119152, 115647, 109222,  98300}, /* 3/4 */
            {132391, 128497, 121358, 109222}, /* 5/6 */
            {139011, 134922, 127426, 114684}  /* 7/8 */
        },
        {
            /* 64QAM */
            {119152, 115647, 109222,  98300}, /* 1/2 */
            {158869, 154197, 145630, 131067}, /* 2/3 */
            {178728, 173471, 163834, 147450}, /* 3/4 */
            {198587, 192746, 182038, 163834}, /* 5/6 */
            {208516, 202383, 191140, 172026}  /* 7/8 */
        }
    };

    const uint32_t dataRate1Seg8MHzBW[3][5][4] = {
            /* 1/32    1/16    1/8     1/4 */
        {
            /* DQPSK, QPSK */
            { 45391,  44056,  41608,  37447}, /* 1/2 */
            { 60521,  58741,  55478,  49930}, /* 2/3 */
            { 68087,  66084,  62413,  56171}, /* 3/4 */
            { 75652,  73427,  69347,  62413}, /* 5/6 */
            { 79434,  77098,  72815,  65533}  /* 7/8 */
        },
        {
            /* 16QAM */
            { 90782,  88112,  83217,  74895}, /* 1/2 */
            {121043, 117483, 110956,  99860}, /* 2/3 */
            {136174, 132168, 124826, 112343}, /* 3/4 */
            {151304, 146854, 138695, 124826}, /* 5/6 */
            {158869, 154197, 145630, 131067}  /* 7/8 */
        },
        {
            /* 64QAM */
            {136174, 132168, 124826, 112343}, /* 1/2 */
            {181565, 176225, 166434, 149791}, /* 2/3 */
            {204261, 198253, 187239, 168515}, /* 3/4 */
            {226956, 220281, 208043, 187239}, /* 5/6 */
            {238304, 231295, 218445, 196601}  /* 7/8 */
        }
    };

    sony_result_t result = SONY_RESULT_OK;
    sony_isdbt_mode_t mode;
    sony_isdbt_guard_t guard;
    sony_isdbt_tmcc_info_t tmccInfo;
    uint8_t modIndex[3];
    uint8_t codeRateIndex[3];
    uint8_t segmentNum[3];
    uint32_t tsRate[3];
    int i = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_monitor_TSRate");

    if ((!pTunerDemod) || (!pTSRateKbpsA) || (!pTSRateKbpsB) || (!pTSRateKbpsC)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    result = sony_tunerdemod_isdbt_monitor_ModeGuard (pTunerDemod, &mode, &guard);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (guard > SONY_ISDBT_GUARD_1_4) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    result = sony_tunerdemod_isdbt_monitor_TMCCInfo (pTunerDemod, &tmccInfo);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    modIndex[0] = (uint8_t)tmccInfo.currentInfo.layerA.modulation;
    codeRateIndex[0] = (uint8_t)tmccInfo.currentInfo.layerA.codingRate;
    segmentNum[0] = (uint8_t)tmccInfo.currentInfo.layerA.segmentsNum;

    modIndex[1] = (uint8_t)tmccInfo.currentInfo.layerB.modulation;
    codeRateIndex[1] = (uint8_t)tmccInfo.currentInfo.layerB.codingRate;
    segmentNum[1] = (uint8_t)tmccInfo.currentInfo.layerB.segmentsNum;

    modIndex[2] = (uint8_t)tmccInfo.currentInfo.layerC.modulation;
    codeRateIndex[2] = (uint8_t)tmccInfo.currentInfo.layerC.codingRate;
    segmentNum[2] = (uint8_t)tmccInfo.currentInfo.layerC.segmentsNum;

    for (i = 0; i < 3; i++) {
        if((modIndex[i] > (uint8_t)SONY_ISDBT_MODULATION_64QAM) || (codeRateIndex[i] > SONY_ISDBT_CODING_RATE_7_8)
            || (segmentNum[i] > 13)){
            /* The layer is unused or some error */
            tsRate[i] = 0;
            continue;
        }

        if(modIndex[i] != 0){
            /* DQPSK and QPSK share same table */
            modIndex[i]--;
        }

        switch(pTunerDemod->bandwidth){
        case SONY_DTV_BW_6_MHZ:
            tsRate[i] = dataRate1Seg6MHzBW[modIndex[i]][codeRateIndex[i]][guard] * segmentNum[i];
            break;
        case SONY_DTV_BW_7_MHZ:
            tsRate[i] = dataRate1Seg7MHzBW[modIndex[i]][codeRateIndex[i]][guard] * segmentNum[i];
            break;
        case SONY_DTV_BW_8_MHZ:
            tsRate[i] = dataRate1Seg8MHzBW[modIndex[i]][codeRateIndex[i]][guard] * segmentNum[i];
            break;
        default:
            SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
        }

        tsRate[i] = (tsRate[i] + 50) / 100;
    }

    *pTSRateKbpsA = tsRate[0];
    *pTSRateKbpsB = tsRate[1];
    *pTSRateKbpsC = tsRate[2];

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbt_monitor_PresetInfo (sony_tunerdemod_t* pTunerDemod,
                                                        sony_tunerdemod_isdbt_preset_info_t* pPresetInfo)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER("sony_tunerdemod_isdbt_monitor_PresetInfo");

    if ((!pTunerDemod) || (!pPresetInfo)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    result = IsDmdLocked(pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Check for sub. */
            result = sony_tunerdemod_isdbt_monitor_PresetInfo (pTunerDemod->pDiverSub, pPresetInfo);
        }

        SONY_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x0E */
    result = pTunerDemod->pRegio->WriteOneRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0E);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN(result);
    }

    /*
     *  slave    Bank    Addr    Bit              Name
     *  --------------------------------------------------------
     *  <SLV-T>   0Eh     32h    [7:0]          PIR_TMCC_SET_2
     *  <SLV-T>   0Eh     33h    [7:0]          PIR_TMCC_SET_3
     *  <SLV-T>   0Eh     34h    [7:0]          PIR_TMCC_SET_4
     *  <SLV-T>   0Eh     35h    [7:0]          PIR_TMCC_SET_5
     *  <SLV-T>   0Eh     36h    [7:0]          PIR_TMCC_SET_6
     *  <SLV-T>   0Eh     37h    [7:0]          PIR_TMCC_SET_7
     *  <SLV-T>   0Eh     38h    [7:0]          PIR_TMCC_SET_8
     *  <SLV-T>   0Eh     39h    [7:0]          PIR_TMCC_SET_9
     *  <SLV-T>   0Eh     3Ah    [7:0]          PIR_TMCC_SET_10
     *  <SLV-T>   0Eh     3Bh    [7:0]          PIR_TMCC_SET_11
     *  <SLV-T>   0Eh     3Ch    [7:0]          PIR_TMCC_SET_12
     *  <SLV-T>   0Eh     3Dh    [7:0]          PIR_TMCC_SET_13
     *  <SLV-T>   0Eh     3Eh    [7:0]          PIR_TMCC_SET_14
     */
    result = pTunerDemod->pRegio->ReadRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x32, pPresetInfo->data, 13);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN(result);
    }

    SLVT_UnFreezeReg(pTunerDemod);

    SONY_TRACE_RETURN(result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_TMCCInfo (sony_tunerdemod_t*       pTunerDemod,
                                                      sony_isdbt_tmcc_info_t * pTMCCInfo)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data[13] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    SONY_TRACE_ENTER("sony_tunerdemod_isdbt_monitor_TMCCInfo");

    if ((!pTunerDemod) || (!pTMCCInfo)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    result = IsDmdLocked(pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Check for sub. */
            result = sony_tunerdemod_isdbt_monitor_TMCCInfo (pTunerDemod->pDiverSub, pTMCCInfo);
        }

        SONY_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x0E */
    result = pTunerDemod->pRegio->WriteOneRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0E);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN(result);
    }

    /*
     *  slave    Bank    Addr    Bit              Name
     *  --------------------------------------------------------
     *  <SLV-T>   0Eh     32h    [7:0]          PIR_TMCC_SET_2
     *  <SLV-T>   0Eh     33h    [7:0]          PIR_TMCC_SET_3
     *  <SLV-T>   0Eh     34h    [7:0]          PIR_TMCC_SET_4
     *  <SLV-T>   0Eh     35h    [7:0]          PIR_TMCC_SET_5
     *  <SLV-T>   0Eh     36h    [7:0]          PIR_TMCC_SET_6
     *  <SLV-T>   0Eh     37h    [7:0]          PIR_TMCC_SET_7
     *  <SLV-T>   0Eh     38h    [7:0]          PIR_TMCC_SET_8
     *  <SLV-T>   0Eh     39h    [7:0]          PIR_TMCC_SET_9
     *  <SLV-T>   0Eh     3Ah    [7:0]          PIR_TMCC_SET_10
     *  <SLV-T>   0Eh     3Bh    [7:0]          PIR_TMCC_SET_11
     *  <SLV-T>   0Eh     3Ch    [7:0]          PIR_TMCC_SET_12
     *  <SLV-T>   0Eh     3Dh    [7:0]          PIR_TMCC_SET_13
     *  <SLV-T>   0Eh     3Eh    [7:0]          PIR_TMCC_SET_14
     */
    result = pTunerDemod->pRegio->ReadRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x32, data, 13);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN(result);
    }

    SLVT_UnFreezeReg(pTunerDemod);

    result = setTmccInfo(data, pTMCCInfo);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN(result);
    }

    SONY_TRACE_RETURN(result);
}

sony_result_t sony_tunerdemod_isdbt_monitor_ACEEWInfo (sony_tunerdemod_t * pTunerDemod,
                                                       uint8_t * pIsExist,
                                                       sony_isdbt_aceew_info_t * pACEEWInfo)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data[36];
    SONY_TRACE_ENTER("sony_tunerdemod_isdbt_monitor_ACEEWInfo");

    if ((!pTunerDemod) || (!pIsExist) || (!pACEEWInfo)) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pTunerDemod->system != SONY_DTV_SYSTEM_ISDBT) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTSB)
        && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_A) && (pTunerDemod->system != SONY_DTV_SYSTEM_ISDBTMM_B)) {
        /* Not ISDB-T */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    result = IsDmdLocked(pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Check for sub. */
            result = sony_tunerdemod_isdbt_monitor_ACEEWInfo (pTunerDemod->pDiverSub, pIsExist, pACEEWInfo);
        }

        SONY_TRACE_RETURN(result);
    }
    /*
     * slave    Bank    Addr      Bit           Name
     * ------------------------------------------------------------
     * <SLV-T>   0Eh     3Fh       [0]      IREG_AC_EMERGENCY_ACTIVATION
     * <SLV-T>   0Eh     40h       [7:1]    IREG_AC_EMERGENCY_DECODE_COMMON
     * <SLV-T>   0Eh     41h       [4:0]    IREG_AC_EMERGENCY_PAGE0ALL1,IREG_AC_EMERGENCY_PAGE0SECOND_EN,IREG_AC_EMERGENCY_DECODE_EN_0,IREG_AC_EMERGENCY_DECODE_EN_1,IREG_AC_EMERGENCY_DECODE_EN_2
     * <SLV-T>   0Eh     42h - 4Ch          IREG_AC_EMERGENCY_DECODE_0_1 - IREG_AC_EMERGENCY_DECODE_0_11
     * <SLV-T>   0Eh     4Dh - 57h          IREG_AC_EMERGENCY_DECODE_1_1 - IREG_AC_EMERGENCY_DECODE_1_11
     * <SLV-T>   0Eh     58h - 62h          IREG_AC_EMERGENCY_DECODE_2_1 - IREG_AC_EMERGENCY_DECODE_2_11
     */
    result = pTunerDemod->pRegio->WriteOneRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0E);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN(result);
    }

    result = pTunerDemod->pRegio->ReadRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x3F, data, 36);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg(pTunerDemod);
        SONY_TRACE_RETURN(result);
    }

    SLVT_UnFreezeReg(pTunerDemod);

    if(data[0] & 0x01) {
        uint8_t i = 0;
        *pIsExist = 1;

        pACEEWInfo->startEndFlag = (uint8_t)((data[1] >> 6) & 0x03);
        pACEEWInfo->updateFlag = (uint8_t)((data[1] >> 4) & 0x03);
        pACEEWInfo->signalId = (uint8_t)((data[1] >> 1) & 0x07);

        pACEEWInfo->isAreaValid = 0;
        pACEEWInfo->isEpicenter1Valid = 0;
        pACEEWInfo->isEpicenter2Valid = 0;
        if(data[2] & 0x04) {
            /* Area data is valid */
            pACEEWInfo->isAreaValid = 1;
            for (i = 0; i < 11; i++) {
                pACEEWInfo->areaInfo.data[i] = data[3 + i];
            }
        }
        if(data[2] & 0x02) {
            /* Epicenter1 data is valid */
            pACEEWInfo->isEpicenter1Valid = 1;
            for (i = 0; i < 11; i++) {
                pACEEWInfo->epicenter1Info.data[i] = data[14 + i];
            }
        }
        if(data[2] & 0x01) {
            /* Epicenter2 data is valid */
            pACEEWInfo->isEpicenter2Valid = 1;
            for (i = 0; i < 11; i++) {
                pACEEWInfo->epicenter2Info.data[i] = data[25 + i];
            }
        }
    } else {
        *pIsExist = 0;
    }

    SONY_TRACE_RETURN(result);
}

/*------------------------------------------------------------------------------
  Static Functions
------------------------------------------------------------------------------*/
static sony_result_t IsDmdLocked(sony_tunerdemod_t* pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t dmdLock = 0;
    uint8_t tsLock  = 0;
    uint8_t unlock  = 0;
    SONY_TRACE_ENTER("IsDmdLocked");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_isdbt_monitor_SyncStat(pTunerDemod, &dmdLock, &tsLock, &unlock);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN(result);
    }

    if (dmdLock) {
        SONY_TRACE_RETURN(SONY_RESULT_OK);
    } else {
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_HW_STATE);
    }
}

static sony_result_t setTmccInfo(uint8_t* pData, sony_isdbt_tmcc_info_t* pTmccInfo)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER("setTmccInfo");

    /* Null check */
    if(!pTmccInfo){
        SONY_TRACE_RETURN(SONY_RESULT_ERROR_ARG);
    }

    /* System */
    pTmccInfo->systemId = (sony_isdbt_system_t)sony_BitSplitFromByteArray(pData, 0, 2);
    /* Count down index Bit 2-5 */
    pTmccInfo->countDownIndex = (uint8_t)sony_BitSplitFromByteArray(pData, 2, 4);
    /* EWS flag Bit 6 */
    pTmccInfo->ewsFlag = (uint8_t)sony_BitSplitFromByteArray(pData, 6, 1);


    /* Current Info Bit 7 */
    pTmccInfo->currentInfo.isPartial = (uint8_t)sony_BitSplitFromByteArray(pData, 7, 1);

    /* Layer-A Modulation Bit 8-10 */
    pTmccInfo->currentInfo.layerA.modulation = (sony_isdbt_modulation_t)sony_BitSplitFromByteArray(pData, 8, 3);
    /* Layer-A Code rate Bit 11-13 */
    pTmccInfo->currentInfo.layerA.codingRate = (sony_isdbt_coding_rate_t)sony_BitSplitFromByteArray(pData, 11, 3);
    /* Layer-A Interleave Bit 14-16 */
    pTmccInfo->currentInfo.layerA.ilLength = (sony_isdbt_il_length_t)sony_BitSplitFromByteArray(pData, 14, 3);
    /* Layer-A Segment Num 17-20 */
    pTmccInfo->currentInfo.layerA.segmentsNum = (uint8_t)sony_BitSplitFromByteArray(pData, 17, 4);


    /* Layer-B Modulation Bit 21-23 */
    pTmccInfo->currentInfo.layerB.modulation = (sony_isdbt_modulation_t)sony_BitSplitFromByteArray(pData, 21, 3);
    /* Layer-B Code rate Bit 24-26 */
    pTmccInfo->currentInfo.layerB.codingRate = (sony_isdbt_coding_rate_t)sony_BitSplitFromByteArray(pData, 24, 3);
    /* Layer-B Interleave Bit 27-29 */
    pTmccInfo->currentInfo.layerB.ilLength = (sony_isdbt_il_length_t)sony_BitSplitFromByteArray(pData, 27, 3);
    /* Layer-B Segment Num 30-33 */
    pTmccInfo->currentInfo.layerB.segmentsNum = (uint8_t)sony_BitSplitFromByteArray(pData, 30, 4);


    /* Layer-C Modulation Bit 34-36 */
    pTmccInfo->currentInfo.layerC.modulation = (sony_isdbt_modulation_t)sony_BitSplitFromByteArray(pData, 34, 3);
    /* Layer-C Code rate Bit 37-39 */
    pTmccInfo->currentInfo.layerC.codingRate = (sony_isdbt_coding_rate_t)sony_BitSplitFromByteArray(pData, 37, 3);
    /* Layer-C Interleave Bit 40-42 */
    pTmccInfo->currentInfo.layerC.ilLength = (sony_isdbt_il_length_t)sony_BitSplitFromByteArray(pData, 40, 3);
    /* Layer-C Segment Num 43-46 */
    pTmccInfo->currentInfo.layerC.segmentsNum = (uint8_t)sony_BitSplitFromByteArray(pData, 43, 4);


    /* Next 47 */
    pTmccInfo->nextInfo.isPartial = (uint8_t)sony_BitSplitFromByteArray(pData, 47, 1);

    /* Layer-A Modulation Bit 48-50 */
    pTmccInfo->nextInfo.layerA.modulation = (sony_isdbt_modulation_t)sony_BitSplitFromByteArray(pData, 48, 3);
    /* Layer-A Code rate Bit 51-53 */
    pTmccInfo->nextInfo.layerA.codingRate = (sony_isdbt_coding_rate_t)sony_BitSplitFromByteArray(pData, 51, 3);
    /* Layer-A Interleave Bit 54-56 */
    pTmccInfo->nextInfo.layerA.ilLength = (sony_isdbt_il_length_t)sony_BitSplitFromByteArray(pData, 54, 3);
    /* Layer-A Segment Num 57-60 */
    pTmccInfo->nextInfo.layerA.segmentsNum = (uint8_t)sony_BitSplitFromByteArray(pData, 57, 4);


    /* Layer-B Modulation Bit 61-63 */
    pTmccInfo->nextInfo.layerB.modulation = (sony_isdbt_modulation_t)sony_BitSplitFromByteArray(pData, 61, 3);
    /* Layer-B Code rate Bit 64-66 */
    pTmccInfo->nextInfo.layerB.codingRate = (sony_isdbt_coding_rate_t)sony_BitSplitFromByteArray(pData, 64, 3);
    /* Layer-B Interleave Bit 67-69 */
    pTmccInfo->nextInfo.layerB.ilLength = (sony_isdbt_il_length_t)sony_BitSplitFromByteArray(pData, 67, 3);
    /* Layer-B Segment Num 70-73 */
    pTmccInfo->nextInfo.layerB.segmentsNum = (uint8_t)sony_BitSplitFromByteArray(pData, 70, 4);


    /* Layer-C Modulation Bit 74-76 */
    pTmccInfo->nextInfo.layerC.modulation = (sony_isdbt_modulation_t)sony_BitSplitFromByteArray(pData, 74, 3);
    /* Layer-C Code rate Bit 77-79 */
    pTmccInfo->nextInfo.layerC.codingRate = (sony_isdbt_coding_rate_t)sony_BitSplitFromByteArray(pData, 77, 3);
    /* Layer-C Interleave Bit 80-82 */
    pTmccInfo->nextInfo.layerC.ilLength = (sony_isdbt_il_length_t)sony_BitSplitFromByteArray(pData, 80, 3);
    /* Layer-C Segment Num 83-86 */
    pTmccInfo->nextInfo.layerC.segmentsNum = (uint8_t)sony_BitSplitFromByteArray(pData, 83, 4);

    SONY_TRACE_RETURN(result);
}

#undef uint32_t 
#undef int32_t 
#undef int8_t 