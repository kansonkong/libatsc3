/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/06/08
  Modification ID : 2e5ad0dbd2270131e0f0cbda002d2ca983dd2e08
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_monitor.h"
#include "sony_tunerdemod_dvbt2.h"
#include "sony_tunerdemod_dvbt2_monitor.h"
#include "sony_math.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tunerdemod_dvbt2_monitor_SyncStat (sony_tunerdemod_t * pTunerDemod, uint8_t * pSyncStat, uint8_t * pTSLockStat, uint8_t * pUnlockDetected)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_SyncStat");

    if ((!pTunerDemod) || (!pSyncStat) || (!pTSLockStat) || (!pUnlockDetected)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x0B */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t data;
        /* slave    Bank    Addr    Bit              Name                Meaning
         * -------------------------------------------------------------------
         * <SLV-T>   0Bh     10h     [2:0]        IREG_SEQ_OSTATE     0-5:UNLOCK 6:LOCK
         * <SLV-T>   0Bh     10h     [4]          IEARLY_NOOFDM       1: No OFDM
         * <SLV-M>   0Bh     10h     [5]          IREG_SP_TSLOCK      0:UNLOCK 1:LOCK
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, &data, sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        *pSyncStat = data & 0x07;
        *pTSLockStat = ((data & 0x20) ? 1 : 0);
        *pUnlockDetected = ((data & 0x10) ? 1 : 0);
    }

    /* Check for valid SyncStat value. */
    if (*pSyncStat == 0x07){
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_SyncStat_sub (sony_tunerdemod_t * pTunerDemod,
                                                          uint8_t * pSyncStat,
                                                          uint8_t * pUnlockDetected)
{
    uint8_t tsLockStat = 0; /* Ignored for sub */

    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_SyncStat_sub");

    if ((!pTunerDemod) || (!pSyncStat) || (!pUnlockDetected)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod->pDiverSub, pSyncStat, &tsLockStat, pUnlockDetected);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_CarrierOffset (sony_tunerdemod_t * pTunerDemod, int32_t * pOffset)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_CarrierOffset");

    if ((!pTunerDemod) || (!pOffset)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[4];
        uint32_t ctlVal = 0;
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t unlockDetected = 0;

        /* Freeze registers */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncState, &tsLock, &unlockDetected);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (syncState != 6) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name
         * ---------------------------------------------------------------
         * <SLV-T>   0Bh     30h     [3:0]      IREG_CRCG_CTLVAL[27:24]
         * <SLV-T>   0Bh     31h     [7:0]      IREG_CRCG_CTLVAL[23:16]
         * <SLV-T>   0Bh     32h     [7:0]      IREG_CRCG_CTLVAL[15:8]
         * <SLV-T>   0Bh     33h     [7:0]      IREG_CRCG_CTLVAL[7:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x30, data, sizeof (data)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        SLVT_UnFreezeReg (pTunerDemod);

        /*
         * For 8,7,6,5MHzBW calculation:
         *    Carrier Offset [Hz] = -(IREG_CRCG_CTLVAL * (2^-30) * 8 * BW) / (7 *10^-6)
         *
         *    Note: (2^-30 * 8) / (7 * 10^-6) = 1.064368657 * 10^-3
         *    And: 1 / ((2^-30 * 8) / (7 * 10^-6)) = 939.52
         *
         * For 1.7MHzBW calculation:
         *    Carrier offset[Hz] = -(IREG_CRCG_CTLVAL * (2^-30) * 131) / (71 * 10^-6)
         *
         *    Note: (2^-30 * 131) / (71 * 10^-6) = 1.718355736 * 10^-3
         *    And: 1 / ((2^-30 * 131) / (71 * 10^-6)) = 581.95
         */
        ctlVal = ((data[0] & 0x0F) << 24) | (data[1] << 16) | (data[2] << 8) | (data[3]);
        *pOffset = sony_Convert2SComplement (ctlVal, 28);

        switch (pTunerDemod->bandwidth) {
        case SONY_DTV_BW_1_7_MHZ:
            *pOffset = -1 * ((*pOffset) / 582);
            break;
        case SONY_DTV_BW_5_MHZ:
        case SONY_DTV_BW_6_MHZ:
        case SONY_DTV_BW_7_MHZ:
        case SONY_DTV_BW_8_MHZ:
            *pOffset = -1 * ((*pOffset) * (uint8_t)pTunerDemod->bandwidth / 940);
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_CarrierOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                               int32_t * pOffset)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_CarrierOffset_sub");

    if ((!pTunerDemod) || (!pOffset)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_dvbt2_monitor_CarrierOffset (pTunerDemod->pDiverSub, pOffset);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_L1Pre (sony_tunerdemod_t * pTunerDemod, sony_dvbt2_l1pre_t * pL1Pre)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_L1Pre");

    if ((!pTunerDemod) || (!pL1Pre)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[37];
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t unlockDetected = 0;
        uint8_t version = 0;
        sony_dvbt2_profile_t profile;

        /* Freeze the register. */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncState, &tsLock, &unlockDetected);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (result);
        }

        /* Only valid after L1-pre locked. */
        if (syncState < 5) {
            if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
                /* Check for sub. */
                result = sony_tunerdemod_dvbt2_monitor_SyncStat_sub (pTunerDemod, &syncState, &unlockDetected);
                if (result != SONY_RESULT_OK) {
                    SLVT_UnFreezeReg (pTunerDemod);
                    SONY_TRACE_RETURN (result);
                }

                if (syncState < 5) {
                    SLVT_UnFreezeReg (pTunerDemod);
                    SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
                }
            } else {
                SLVT_UnFreezeReg (pTunerDemod);
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
            }
        }

        /* Obtain current profile, used for determining the FFT size */
        result = sony_tunerdemod_dvbt2_monitor_Profile (pTunerDemod, &profile);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (result);
        }

       /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------------------
         * <SLV-M>   0Bh     61h     [7:0]        IL1PRE_TYPE[7:0]
         * <SLV-M>   0Bh     62h     [0]          IL1PRE_BWT_EXT
         * <SLV-M>   0Bh     63h     [2:0]        IL1PRE_S1[2:0]
         * <SLV-M>   0Bh     64h     [3:0]        IL1PRE_S2[3:0]
         * <SLV-M>   0Bh     65h     [0]          IL1PRE_L1_REPETITION_FLAG
         * <SLV-M>   0Bh     66h     [2:0]        IL1PRE_GUARD_INTERVAL[2:0]
         * <SLV-M>   0Bh     67h     [3:0]        IL1PRE_PAPR[3:0]
         * <SLV-M>   0Bh     68h     [3:0]        IL1PRE_L1_MOD[3:0]
         * <SLV-M>   0Bh     69h     [1:0]        IL1PRE_L1_COD[1:0]
         * <SLV-M>   0Bh     6Ah     [1:0]        IL1PRE_L1_FEC_TYPE[1:0]
         * <SLV-M>   0Bh     6Bh     [1:0]        IL1PRE_L1_POST_SIZE[17:16]
         * <SLV-M>   0Bh     6Ch     [7:0]        IL1PRE_L1_POST_SIZE[15:8]
         * <SLV-M>   0Bh     6Dh     [7:0]        IL1PRE_L1_POST_SIZE[7:0]
         * <SLV-M>   0Bh     6Eh     [1:0]        IL1PRE_L1_POST_INFO_SIZE[17:16]
         * <SLV-M>   0Bh     6Fh     [7:0]        IL1PRE_L1_POST_INFO_SIZE[15:8]
         * <SLV-M>   0Bh     70h     [7:0]        IL1PRE_L1_POST_INFO_SIZE[7:0]
         * <SLV-M>   0Bh     71h     [3:0]        IL1PRE_PILOT_PATTERN[3:0]
         * <SLV-M>   0Bh     72h     [7:0]        IL1PRE_TX_ID_AVAILABILITY[7:0]
         * <SLV-M>   0Bh     73h     [7:0]        IL1PRE_CELL_ID[15:8]
         * <SLV-M>   0Bh     74h     [7:0]        IL1PRE_CELL_ID[7:0]
         * <SLV-M>   0Bh     75h     [7:0]        IL1PRE_NETWORK_ID[15:8]
         * <SLV-M>   0Bh     76h     [7:0]        IL1PRE_NETWORK_ID[7:0]
         * <SLV-M>   0Bh     77h     [7:0]        IL1PRE_T2_SYSTEM_ID[15:8]
         * <SLV-M>   0Bh     78h     [7:0]        IL1PRE_T2_SYSTEM_ID[7:0]
         * <SLV-M>   0Bh     79h     [7:0]        IL1PRE_NUM_T2_FRAMES[7:0]
         * <SLV-M>   0Bh     7Ah     [3:0]        IL1PRE_NUM_DATA_SYMBOLS[11:8]
         * <SLV-M>   0Bh     7Bh     [7:0]        IL1PRE_NUM_DATA_SYMBOLS[7:0]
         * <SLV-M>   0Bh     7Ch     [2:0]        IL1PRE_REGEN_FLAG[2:0]
         * <SLV-M>   0Bh     7Dh     [0]          IL1PRE_L1_POST_EXTENSION
         * <SLV-M>   0Bh     7Eh     [2:0]        IL1PRE_NUM_RF[2:0]
         * <SLV-M>   0Bh     7Fh     [2:0]        IL1PRE_CURRENT_RF_IDX[2:0]
         * <SLV-M>   0Bh     80h     [1:0]        IL1PRE_T2_VERSION[3:2]
         * <SLV-M>   0Bh     81h     [7:0]        IL1PRE_T2_VERSION[1:0], IL1PRE_L1_POST_SCRAMBLED, IL1PRE_T2_BASE_LITE, IL1PRE_RESERVED[3:0]
         * <SLV-M>   0Bh     82h     [7:0]        IL1PRE_CRC_32[31:24]
         * <SLV-M>   0Bh     83h     [7:0]        IL1PRE_CRC_32[23:16]
         * <SLV-M>   0Bh     84h     [7:0]        IL1PRE_CRC_32[15:8]
         * <SLV-M>   0Bh     85h     [7:0]        IL1PRE_CRC_32[7:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x61, data, sizeof (data)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        SLVT_UnFreezeReg (pTunerDemod);

        /* Convert data to appropriate format. */
        pL1Pre->type = (sony_dvbt2_l1pre_type_t) data[0];
        pL1Pre->bwExt = data[1] & 0x01;
        pL1Pre->s1 = (sony_dvbt2_s1_t) (data[2] & 0x07);
        pL1Pre->s2 = data[3] & 0x0F;
        pL1Pre->l1Rep = data[4] & 0x01;
        pL1Pre->gi = (sony_dvbt2_guard_t) (data[5] & 0x07);
        pL1Pre->papr = (sony_dvbt2_papr_t) (data[6] & 0x0F);
        pL1Pre->mod = (sony_dvbt2_l1post_constell_t) (data[7] & 0x0F);
        pL1Pre->cr = (sony_dvbt2_l1post_cr_t) (data[8] & 0x03);
        pL1Pre->fec = (sony_dvbt2_l1post_fec_type_t) (data[9] & 0x03);
        pL1Pre->l1PostSize = (data[10] & 0x03) << 16;
        pL1Pre->l1PostSize |= (data[11]) << 8;
        pL1Pre->l1PostSize |= (data[12]);
        pL1Pre->l1PostInfoSize = (data[13] & 0x03) << 16;
        pL1Pre->l1PostInfoSize |= (data[14]) << 8;
        pL1Pre->l1PostInfoSize |= (data[15]);
        pL1Pre->pp = (sony_dvbt2_pp_t) (data[16] & 0x0F);
        pL1Pre->txIdAvailability = data[17];
        pL1Pre->cellId = (data[18] << 8);
        pL1Pre->cellId |= (data[19]);
        pL1Pre->networkId = (data[20] << 8);
        pL1Pre->networkId |= (data[21]);
        pL1Pre->systemId = (data[22] << 8);
        pL1Pre->systemId |= (data[23]);
        pL1Pre->numFrames = data[24];
        pL1Pre->numSymbols = (data[25] & 0x0F) << 8;
        pL1Pre->numSymbols |= data[26];
        pL1Pre->regen = data[27] & 0x07;
        pL1Pre->postExt = data[28] & 0x01;
        pL1Pre->numRfFreqs = data[29] & 0x07;
        pL1Pre->rfIdx = data[30] & 0x07;
        version = (data[31] & 0x03) << 2;
        version |= (data[32] & 0xC0) >> 6;
        pL1Pre->t2Version = (sony_dvbt2_version_t) version;
        pL1Pre->l1PostScrambled = (data[32] & 0x20) >> 5;
        pL1Pre->t2BaseLite = (data[32] & 0x10) >> 4;
        pL1Pre->crc32 = (data[33] << 24);
        pL1Pre->crc32 |= (data[34] << 16);
        pL1Pre->crc32 |= (data[35] << 8);
        pL1Pre->crc32 |= data[36];

        if (profile == SONY_DVBT2_PROFILE_BASE) {
            /* Get the FFT mode from the S2 signalling. */
            switch ((pL1Pre->s2 >> 1)) {
            case SONY_DVBT2_BASE_S2_M1K_G_ANY:
                pL1Pre->fftMode = SONY_DVBT2_M1K;
                break;
            case SONY_DVBT2_BASE_S2_M2K_G_ANY:
                pL1Pre->fftMode = SONY_DVBT2_M2K;
                break;
            case SONY_DVBT2_BASE_S2_M4K_G_ANY:
                pL1Pre->fftMode = SONY_DVBT2_M4K;
                break;
            /* Intentional fall through */
            case SONY_DVBT2_BASE_S2_M8K_G_DVBT:
            case SONY_DVBT2_BASE_S2_M8K_G_DVBT2:
                pL1Pre->fftMode = SONY_DVBT2_M8K;
                break;
            case SONY_DVBT2_BASE_S2_M16K_G_ANY:
                pL1Pre->fftMode = SONY_DVBT2_M16K;
                break;
            /* Intentional fall through */
            case SONY_DVBT2_BASE_S2_M32K_G_DVBT:
            case SONY_DVBT2_BASE_S2_M32K_G_DVBT2:
                pL1Pre->fftMode = SONY_DVBT2_M32K;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
            }
        }
        else if (profile == SONY_DVBT2_PROFILE_LITE) {
            /* Get the FFT mode from the S2 signalling. */
            switch ((pL1Pre->s2 >> 1)) {
            case SONY_DVBT2_LITE_S2_M2K_G_ANY:
                pL1Pre->fftMode = SONY_DVBT2_M2K;
                break;
            case SONY_DVBT2_LITE_S2_M4K_G_ANY:
                pL1Pre->fftMode = SONY_DVBT2_M4K;
                break;
            /* Intentional fall through */
            case SONY_DVBT2_LITE_S2_M8K_G_DVBT:
            case SONY_DVBT2_LITE_S2_M8K_G_DVBT2:
                pL1Pre->fftMode = SONY_DVBT2_M8K;
                break;
            /* Intentional fall through */
            case SONY_DVBT2_LITE_S2_M16K_G_DVBT:
            case SONY_DVBT2_LITE_S2_M16K_G_DVBT2:
                pL1Pre->fftMode = SONY_DVBT2_M16K;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
            }
        }
        else {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* Get the Mixed indicator (FEFs exist) from the S2 signalling. */
        pL1Pre->mixed = pL1Pre->s2 & 0x01;
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_Version (sony_tunerdemod_t * pTunerDemod, sony_dvbt2_version_t * pVersion)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t version = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_Version");

    if ((!pTunerDemod) || (!pVersion)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[2];
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t unlockDetected = 0;

        /* Freeze the register. */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncState, &tsLock, &unlockDetected);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Only valid after L1-pre locked. */
        if (syncState < 5) {
            if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
                /* Check for sub. */
                result = sony_tunerdemod_dvbt2_monitor_SyncStat_sub (pTunerDemod, &syncState, &unlockDetected);
                if (result != SONY_RESULT_OK) {
                    SLVT_UnFreezeReg (pTunerDemod);
                    SONY_TRACE_RETURN (result);
                }

                if (syncState < 5) {
                    SLVT_UnFreezeReg (pTunerDemod);
                    SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
                }
            } else {
                SLVT_UnFreezeReg (pTunerDemod);
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
            }
        }

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------------------
         * <SLV-M>   0Bh     80h     [1:0]        IL1PRE_T2_VERSION[3:2]
         * <SLV-M>   0Bh     81h     [7:0]        IL1PRE_T2_VERSION[1:0], IL1PRE_L1_POST_SCRAMBLED, IL1PRE_T2_BASE_LITE, IL1PRE_RESERVED[3:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x80, data, sizeof (data)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        SLVT_UnFreezeReg (pTunerDemod);

        version = ((data[0] & 0x03) << 2);
        version |= ((data[1] & 0xC0) >> 6);
        *pVersion = (sony_dvbt2_version_t) version;
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_OFDM (sony_tunerdemod_t * pTunerDemod, sony_dvbt2_ofdm_t * pOfdm)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_OFDM");

    if ((!pTunerDemod) || (!pOfdm)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[5];
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t unlockDetected = 0;
        sony_result_t result = SONY_RESULT_OK;

        /* Freeze the registers. */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncState, &tsLock, &unlockDetected);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Only valid after demod locked. */
        if (syncState != 6) {
            SLVT_UnFreezeReg (pTunerDemod);

            result = SONY_RESULT_ERROR_HW_STATE;

            if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
                /* Check for sub. */
                result = sony_tunerdemod_dvbt2_monitor_OFDM (pTunerDemod->pDiverSub, pOfdm);
            }

            SONY_TRACE_RETURN (result);
        }

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        /* slave    Bank    Addr    Bit              Name                mean
         * ---------------------------------------------------------------------------------
         * <SLV-T>   0Bh     1Dh     [5]          IREG_OFDM_MIXED         0: Not mixed, 1: Mixed
         * <SLV-T>   0Bh     1Dh     [4]          IREG_OFDM_MISO          0: SISO, 1: MISO
         * <SLV-T>   0Bh     1Dh     [2:0]        IREG_OFDM_FFTSIZE[2:0]  0: 2K, 1: 8K, 2: 4K, 3: 1K, 4: 16K, 5: 32K
         * <SLV-T>   0Bh     1Eh     [6:4]        IREG_OFDM_GI[2:0]       0: 1/32, 1: 1/16, 2: 1/8, 3: 1/4, 4: 1/128, 5: 19/128, 6: 19/256
         * <SLV-T>   0Bh     1Fh     [2:0]        IREG_OFDM_PP[2:0]       0: PP1, 1: PP2, 2: PP3, 3: PP4, 4: PP5, 5: PP6, 6: PP7, 7: PP8
         * <SLV-T>   0Bh     1Fh     [4]          IREG_OFDM_BWT_EXT       0: Normal, 1: Extended
         * <SLV-T>   0Bh     1Fh     [3:0]        IREG_OFDM_PAPR[3:0]     The meaning of IREG_OFDM_PAPR[3:0] depends on T2_VERSION.
         * <SLV-T>   0Bh     20h     [3:0]        IREG_OFDM_NDSYM[11:8]
         * <SLV-T>   0Bh     21h     [7:0]        IREG_OFDM_NDSYM[7:0]    Number of data symbols in a T2 frame
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1D, data, sizeof (data)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Unfreeze the registers. */
        SLVT_UnFreezeReg (pTunerDemod);

        /* Convert */
        pOfdm->mixed = ((data[0] & 0x20) ? 1 : 0);
        pOfdm->isMiso = ((data[0] & 0x10) >> 4);
        pOfdm->mode = (sony_dvbt2_mode_t) (data[0] & 0x07);
        pOfdm->gi = (sony_dvbt2_guard_t) ((data[1] & 0x70) >> 4);
        pOfdm->pp = (sony_dvbt2_pp_t) (data[1] & 0x07);
        pOfdm->bwExt = (data[2] & 0x10) >> 4;
        pOfdm->papr = (sony_dvbt2_papr_t) (data[2] & 0x0F);
        pOfdm->numSymbols = (data[3] << 8) | data[4];
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_DataPLPs (sony_tunerdemod_t * pTunerDemod, uint8_t * pPLPIds, uint8_t * pNumPLPs)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_DataPLPs");

    if ((!pTunerDemod) || (!pNumPLPs)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t l1PostOK = 0;

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Freeze the registers. */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        /* slave    Bank    Addr    Bit              Name                mean
         * ---------------------------------------------------------------------------------
         * <SLV-M>   0Bh     86h     [0]            IL1POST_OK           0:invalid1:valid
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x86, &l1PostOK, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (!(l1PostOK & 0x01)) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* Read the number of PLPs. */
        /* slave    Bank    Addr    Bit              Name                 mean
         * ---------------------------------------------------------------------------------
         * <SLV-M>   0Bh     C1h     [7:0]      INUM_DATA_PLP_TS[7:0]    number of TS PLP (max 255)
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xC1, pNumPLPs, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Invalid number of PLPs detected. May occur if multiple threads accessing device. */
        if (*pNumPLPs == 0) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
        }

        if (!pPLPIds) {
            /* PLP ID list is unnecessary. */
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_OK);
        }

        /* Read the PLPs from the device. */
        /* Read up to PLP_ID61 */
        /*  slave    Bank    Addr    Bit              Name                 mean
         * --------------------------------------------------------------------------
         * <SLV-M>   0Bh     C2h     [7:0]      IL1POST_PLP_ID0[7:0]     TS PLP_ID #1
         * ...
         * <SLV-M>   0Bh     FFh     [7:0]      IL1POST_PLP_ID61[7:0]    TS PLP_ID #62
         * <SLV-M>   0Ch     10h     [7:0]      IL1POST_PLP_ID62[7:0]    TS PLP_ID #63
         * ...
         * <SLV-M>   0Ch     D0h     [7:0]      IL1POST_PLP_ID254[7:0]   TS PLP_ID #255
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xC2, pPLPIds, ((*pNumPLPs > 62) ? 62 : *pNumPLPs)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (*pNumPLPs > 62) {
            /* Set SLV-T Bank : 0x0C */
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0C) != SONY_RESULT_OK) {
                SLVT_UnFreezeReg (pTunerDemod);
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            /* Read the remaining data PLPs. */
            if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, pPLPIds + 62, *pNumPLPs - 62) != SONY_RESULT_OK) {
                SLVT_UnFreezeReg (pTunerDemod);
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        SLVT_UnFreezeReg (pTunerDemod);
    }
    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_ActivePLP (sony_tunerdemod_t * pTunerDemod,
                                                       sony_dvbt2_plp_btype_t type,
                                                       sony_dvbt2_plp_t * pPLPInfo)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_ActivePLP");

    if ((!pTunerDemod) || (!pPLPInfo)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[20];
        uint8_t addr = 0;
        uint8_t index = 0;
        uint8_t l1PostOk = 0;

        /* Freeze the registers. */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Check L1 Post is valid
         * slave    Bank    Addr    Bit    Name
         * ------------------------------------------
         * <SLV-M>  0Bh     86h     [0]    IL1POST_OK
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x86, &l1PostOk, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (!l1PostOk) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        if (type == SONY_DVBT2_PLP_COMMON) {
            /* Read contents of the L1-post for appropriate item. */
            /* slave    Bank    Addr    Bit           Name
             * -----------------------------------------------------------
             * <SLV-M>   0Bh     A9h     [7:0]        IL1POST_PLP_ID_C[7:0]
             * <SLV-M>   0Bh     AAh     [2:0]        IL1POST_PLP_TYPE_C[2:0]
             * <SLV-M>   0Bh     ABh     [4:0]        IL1POST_PLP_PAYLOAD_TYPE_C[4:0]
             * <SLV-M>   0Bh     ACh     [0]          IL1POST_FF_FLAG_C
             * <SLV-M>   0Bh     ADh     [2:0]        IL1POST_FIRST_RF_IDX_C[2:0]
             * <SLV-M>   0Bh     AEh     [7:0]        IL1POST_FIRST_FRAME_IDX_C[7:0]
             * <SLV-M>   0Bh     AFh     [7:0]        IL1POST_PLP_GROUP_ID_C[7:0]]
             * <SLV-M>   0Bh     B0h     [2:0]        IL1POST_PLP_COD_C[2:0]
             * <SLV-M>   0Bh     B1h     [2:0]        IL1POST_PLP_MOD_C[2:0]
             * <SLV-M>   0Bh     B2h     [0]          IL1POST_PLP_ROTATION_C
             * <SLV-M>   0Bh     B3h     [1:0]        IL1POST_PLP_FEC_TYPE_C[1:0]
             * <SLV-M>   0Bh     B4h     [1:0]        IL1POST_PLP_NUM_BLOCKS_MAX_C[9:8]
             * <SLV-M>   0Bh     B5h     [7:0]        IL1POST_PLP_NUM_BLOCKS_MAX_C[7:0]
             * <SLV-M>   0Bh     B6h     [7:0]        IL1POST_FRAME_INTERVAL_C[7:0]
             * <SLV-M>   0Bh     B7h     [7:0]        IL1POST_TIME_IL_LENGTH_C[7:0]
             * <SLV-M>   0Bh     B8h     [0]          IL1POST_TIME_IL_TYPE_C
             * <SLV-M>   0Bh     B9h     [0]          IL1POST_IN_BAND_FLAG_C
             * <SLV-M>   0Bh     BAh     [7:0]        IL1POST_RESERVED_1_C[15:8]
             * <SLV-M>   0Bh     BBh     [7:0]        IL1POST_RESERVED_1_C[7:0]
             */
            addr = 0xA9;
        }
        else {
            /* slave    Bank    Addr    Bit           Name
             * --------------------------------------------
             * <SLV-M>   0Bh     96h     [7:0]        IL1POST_PLP_ID[7:0]
             * <SLV-M>   0Bh     97h     [2:0]        IL1POST_PLP_TYPE[2:0]
             * <SLV-M>   0Bh     98h     [4:0]        IL1POST_PLP_PAYLOAD_TYPE[4:0]
             * <SLV-M>   0Bh     99h     [0]          IL1POST_FF_FLAG
             * <SLV-M>   0Bh     9Ah     [2:0]        IL1POST_FIRST_RF_IDX[2:0]
             * <SLV-M>   0Bh     9Bh     [7:0]        IL1POST_FIRST_FRAME_IDX[7:0]
             * <SLV-M>   0Bh     9Ch     [7:0]        IL1POST_PLP_GROUP_ID[7:0]
             * <SLV-M>   0Bh     9Dh     [2:0]        IL1POST_PLP_COD[2:0]
             * <SLV-M>   0Bh     9Eh     [2:0]        IL1POST_PLP_MOD[2:0]
             * <SLV-M>   0Bh     9Fh     [0]          IL1POST_PLP_ROTATION
             * <SLV-M>   0Bh     A0h     [1:0]        IL1POST_PLP_FEC_TYPE[1:0]
             * <SLV-M>   0Bh     A1h     [1:0]        IL1POST_PLP_NUM_BLOCKS_MAX[9:8]
             * <SLV-M>   0Bh     A2h     [7:0]        IL1POST_PLP_NUM_BLOCKS_MAX[7:0]
             * <SLV-M>   0Bh     A3h     [7:0]        IL1POST_FRAME_INTERVAL[7:0]
             * <SLV-M>   0Bh     A4h     [7:0]        IL1POST_TIME_IL_LENGTH[7:0]
             * <SLV-M>   0Bh     A5h     [0]          IL1POST_TIME_IL_TYPE
             * <SLV-M>   0Bh     A6h     [0]          IL1POST_IN_BAND_FLAG
             * <SLV-M>   0Bh     A7h     [7:0]        IL1POST_RESERVED_1[15:8]
             * <SLV-M>   0Bh     A8h     [7:0]        IL1POST_RESERVED_1[7:0]
             */
            addr = 0x96;
        }

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, addr, data, sizeof (data)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        SLVT_UnFreezeReg (pTunerDemod);

        /* If common, check for no common PLP, frame_interval == 0. */
        if ((type == SONY_DVBT2_PLP_COMMON) && (data[13] == 0)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        pPLPInfo->id = data[index++];
        pPLPInfo->type = (sony_dvbt2_plp_type_t) (data[index++] & 0x07);
        pPLPInfo->payload = (sony_dvbt2_plp_payload_t) (data[index++] & 0x1F);
        pPLPInfo->ff = data[index++] & 0x01;
        pPLPInfo->firstRfIdx = data[index++] & 0x07;
        pPLPInfo->firstFrmIdx = data[index++];
        pPLPInfo->groupId = data[index++];
        pPLPInfo->plpCr = (sony_dvbt2_plp_code_rate_t) (data[index++] & 0x07);
        pPLPInfo->constell = (sony_dvbt2_plp_constell_t) (data[index++] & 0x07);
        pPLPInfo->rot = data[index++] & 0x01;
        pPLPInfo->fec = (sony_dvbt2_plp_fec_t) (data[index++] & 0x03);
        pPLPInfo->numBlocksMax = (uint16_t) ((data[index++] & 0x03)) << 8;
        pPLPInfo->numBlocksMax |= data[index++];
        pPLPInfo->frmInt = data[index++];
        pPLPInfo->tilLen = data[index++];
        pPLPInfo->tilType = data[index++] & 0x01;

        pPLPInfo->inBandAFlag = data[index++] & 0x01;
        pPLPInfo->rsvd = data[index++] << 8;
        pPLPInfo->rsvd |= data[index++];

        /* New fields from V1.2.1 */
        pPLPInfo->inBandBFlag = (uint8_t) ((pPLPInfo->rsvd & 0x8000) >> 15);
        pPLPInfo->plpMode = (sony_dvbt2_plp_mode_t) ((pPLPInfo->rsvd & 0x000C) >> 2);
        pPLPInfo->staticFlag = (uint8_t) ((pPLPInfo->rsvd & 0x0002) >> 1);
        pPLPInfo->staticPaddingFlag = (uint8_t) (pPLPInfo->rsvd & 0x0001);
        pPLPInfo->rsvd = (uint16_t) ((pPLPInfo->rsvd & 0x7FF0) >> 4); /* Remaining 11 bits */
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_DataPLPError (sony_tunerdemod_t * pTunerDemod, uint8_t * pPLPError)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_DataPLPError");

    if ((!pTunerDemod) || (!pPLPError)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the registers. */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t data;

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit           Name                meaning
         *  --------------------------------------------------------------------------------
         *  <SLV-M>   0Bh     86h     [0]       IL1POST_OK         0:invalid 1:valid
         *  <SLV-M>   0Bh     C0h     [0]       IPLP_SEL_ERR       0:found 1:not found
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x86, &data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if ((data & 0x01) == 0x00) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xC0, &data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        SLVT_UnFreezeReg (pTunerDemod);

        *pPLPError = data & 0x01;
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_L1Change (sony_tunerdemod_t * pTunerDemod, uint8_t * pL1Change)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_L1Change");

    if ((!pTunerDemod) || (!pL1Change)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data;
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t unlockDetected = 0;
        sony_result_t result = SONY_RESULT_OK;

        /* Freeze the registers. */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncState, &tsLock, &unlockDetected);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Only valid after L1-post decoded at least once. */
        if (syncState < 5) {
            if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
                /* Check for sub. */
                result = sony_tunerdemod_dvbt2_monitor_SyncStat_sub (pTunerDemod, &syncState, &unlockDetected);
                if (result != SONY_RESULT_OK) {
                    SLVT_UnFreezeReg (pTunerDemod);
                    SONY_TRACE_RETURN (result);
                }

                if (syncState < 5) {
                    SLVT_UnFreezeReg (pTunerDemod);
                    SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
                }
            } else {
                SLVT_UnFreezeReg (pTunerDemod);
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
            }
        }

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name               mean
         * --------------------------------------------------------------------------------
         * <SLV-M>   0Bh     5Fh     [0]       IREG_L1_CHANGE_FLAG   0:Not changed, 1:Changed
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5F, &data, sizeof (data)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        *pL1Change = data & 0x01;
        if (*pL1Change) {
            /* Set SLV-T Bank : 0x22 */
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x22) != SONY_RESULT_OK) {
                SLVT_UnFreezeReg (pTunerDemod);
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            /* Only clear if set, otherwise may miss indicator. */
            /* slave    Bank    Addr    Bit    default   Value          Name
             * ---------------------------------------------------------------------------------
             * <SLV-M>   22h     16h     [0]      1'b0       1'b1       OREGD_L1_CHANGE_FLAG_CLR
             */
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x16, 0x01) != SONY_RESULT_OK) {
                SLVT_UnFreezeReg (pTunerDemod);
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        SLVT_UnFreezeReg (pTunerDemod);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_L1Post (sony_tunerdemod_t * pTunerDemod, sony_dvbt2_l1post_t * pL1Post)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_L1Post");

    if ((!pTunerDemod) || (!pL1Post)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[16];

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------------------
         * <SLV-M>   0Bh     86h     [0]          IL1POST_OK   0:invalid1:valid
         * <SLV-M>   0Bh     87h     [6:0]        IL1POST_SUB_SLICES_PER_FRAME[14:8]
         * <SLV-M>   0Bh     88h     [7:0]        IL1POST_SUB_SLICES_PER_FRAME[7:0]
         * <SLV-M>   0Bh     89h     [7:0]        IL1POST_NUM_PLP[7:0]
         * <SLV-M>   0Bh     8Ah     [3:0]        IL1POST_NUM_AUX[3:0]
         * <SLV-M>   0Bh     8Bh     [7:0]        IL1POST_AUX_CONFIG_RFU[7:0]
         * <SLV-M>   0Bh     8Ch     [2:0]        IL1POST_RF_IDX[2:0]
         * <SLV-M>   0Bh     8Dh     [7:0]        IL1POST_FREQUENCY[31:24]
         * <SLV-M>   0Bh     8Eh     [7:0]        IL1POST_FREQUENCY[23:16]
         * <SLV-M>   0Bh     8Fh     [7:0]        IL1POST_FREQUENCY[15:8]
         * <SLV-M>   0Bh     90h     [7:0]        IL1POST_FREQUENCY[7:0]
         * <SLV-M>   0Bh     91h     [3:0]        IL1POST_FEF_TYPE[3:0]
         * <SLV-M>   0Bh     92h     [7:0]        IL1POST_FEF_LENGTH[23:16]
         * <SLV-M>   0Bh     93h     [7:0]        IL1POST_FEF_LENGTH[15:8]
         * <SLV-M>   0Bh     94h     [7:0]        IL1POST_FEF_LENGTH[7:0]
         * <SLV-M>   0Bh     95h     [7:0]        IL1POST_FEF_INTERVAL[7:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x86, data, sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (!(data[0] & 0x01)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* Convert data. */
        pL1Post->subSlicesPerFrame = (data[1] & 0x7F) << 8;
        pL1Post->subSlicesPerFrame |= data[2];
        pL1Post->numPLPs = data[3];
        pL1Post->numAux = data[4] & 0x0F;
        pL1Post->auxConfigRFU = data[5];
        pL1Post->rfIdx = data[6] & 0x07;
        pL1Post->freq = data[7] << 24;
        pL1Post->freq |= data[8] << 16;
        pL1Post->freq |= data[9] << 8;
        pL1Post->freq |= data[10];
        pL1Post->fefType = data[11] & 0x0F;
        pL1Post->fefLength = data[12] << 16;
        pL1Post->fefLength |= data[13] << 8;
        pL1Post->fefLength |= data[14];
        pL1Post->fefInterval = data[15];
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_BBHeader (sony_tunerdemod_t * pTunerDemod,
                                                      sony_dvbt2_plp_btype_t type,
                                                      sony_dvbt2_bbheader_t * pBBHeader)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_BBHeader");

    if ((!pTunerDemod) || (!pBBHeader)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the registers. */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        sony_result_t result = SONY_RESULT_OK;
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t unlockDetected = 0;

        /* Confirm SyncStat. */
        result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncState, &tsLock, &unlockDetected);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (result);
        }

        if (!tsLock) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }
    }

    /* Set SLV-T Bank : 0x0B */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (type == SONY_DVBT2_PLP_COMMON) {
        uint8_t l1PostOk;
        uint8_t data;

        /* Check L1 Post is valid
         * slave    Bank    Addr    Bit    Name
         * ------------------------------------------
         * <SLV-M>  0Bh     86h     [0]    IL1POST_OK
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x86, &l1PostOk, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (!(l1PostOk & 0x01)) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* slave    Bank    Addr    Bit       Name
         * ----------------------------------------------------------------
         * <SLV-M>   0Bh     B6h    [7:0]    IL1POST_FRAME_INTERVAL_C[7:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xB6, &data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* If common, check for no common PLP, frame_interval == 0. */
        if (data == 0) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }
    }

    {
        uint8_t data[14];
        uint8_t addr = 0;

        if (type == SONY_DVBT2_PLP_COMMON) {
            /* BB Header (Common PLP)
             *  slave    Bank    Addr    Bit              Name                       Meaning
             * -------------------------------------------------------------------------------------------
             *  <SLV-M>   0Bh     54h     [0]          IREG_SP_PLP_MODE_COMMON      Common PLP Mode (1'b0:NM,1'b1:HEM))
             *  <SLV-M>   0Bh     51h     [15:8]       IREG_SP_MATYPE_COMMON[15:8]  Common PLP MATYPE
             *  <SLV-M>   0Bh     52h     [7:0]        IREG_SP_MATYPE_COMMON[7:0]   Common PLP MATYPE
             *  <SLV-M>   0Bh     57h     [15:8]       IREG_SP_UPL_COMMON[15:8]     Common PLP UPL  (Invalid at IREG_SP_PLP_MODE_COMMON=1'b1)
             *  <SLV-M>   0Bh     58h     [7:0]        IREG_SP_UPL_COMMON[7:0]      Common PLP UPL  (Invalid at IREG_SP_PLP_MODE_COMMON=1'b1)
             *  <SLV-M>   0Bh     55h     [15:8]       IREG_SP_DFL_COMMON[15:8]     Common PLP DFL
             *  <SLV-M>   0Bh     56h     [7:0]        IREG_SP_DFL_COMMON[7:0]      Common PLP DFL
             *  <SLV-M>   0Bh     59h     [7:0]        IREG_SP_SYNC_COMMON[7:0]     Common PLP SYNC (Invalid at IREG_SP_PLP_MODE_COMMON=1'b1)
             *  <SLV-M>   0Bh     5Ah     [15:8]       IREG_SP_SYNCD_COMMON[15:8]   Common PLP SYNCD
             *  <SLV-M>   0Bh     5Bh     [7:0]        IREG_SP_SYNCD_COMMON[7:0]    Common PLP SYNCD
             *  <SLV-M>   0Bh     5Ch     [23:16]      IREG_SP_HISSY_COMMON[23:16]  Common PLP ISSY (Invalid at IREG_SP_PLP_MODE_COMMON=1'b0)
             *  <SLV-M>   0Bh     5Dh     [15:8]       IREG_SP_HISSY_COMMON[15:8]   Common PLP ISSY (Invalid at IREG_SP_PLP_MODE_COMMON=1'b0)
             *  <SLV-M>   0Bh     5Eh     [7:0]        IREG_SP_HISSY_COMMON[7:0]    Common PLP ISSY (Invalid at IREG_SP_PLP_MODE_COMMON=1'b0)
             */
            addr = 0x51;
        } else {
            /* BB Header (Data PLP)
             *  slave    Bank    Addr    Bit              Name                       Meaning
             * -------------------------------------------------------------------------------------------
             *  <SLV-M>   0Bh     45h     [0]          IREG_SP_PLP_MODE_DATA        Data PLP Mode (1'b0:NM,1'b1:HEM))
             *  <SLV-M>   0Bh     42h     [15:8]       IREG_SP_MATYPE_DATA[15:8]    Data PLP MATYPE
             *  <SLV-M>   0Bh     43h     [7:0]        IREG_SP_MATYPE_DATA[7:0]     Data PLP MATYPE
             *  <SLV-M>   0Bh     48h     [15:8]       IREG_SP_UPL_DATA[15:8]       Data PLP UPL  (Invalid at IREG_SP_PLP_MODE_DATA=1'b1)
             *  <SLV-M>   0Bh     49h     [7:0]        IREG_SP_UPL_DATA[7:0]        Data PLP UPL  (Invalid at IREG_SP_PLP_MODE_DATA=1'b1)
             *  <SLV-M>   0Bh     46h     [15:8]       IREG_SP_DFL_DATA[15:8]       Data PLP DFL
             *  <SLV-M>   0Bh     47h     [7:0]        IREG_SP_DFL_DATA[7:0]        Data PLP DFL
             *  <SLV-M>   0Bh     4Ah     [7:0]        IREG_SP_SYNC_DATA[7:0]       Data PLP SYNC (Invalid at IREG_SP_PLP_MODE_DATA=1'b1)
             *  <SLV-M>   0Bh     4Bh     [15:8]       IREG_SP_SYNCD_DATA[15:8]     Data PLP SYNCD
             *  <SLV-M>   0Bh     4Ch     [7:0]        IREG_SP_SYNCD_DATA[7:0]      Data PLP SYNCD
             *  <SLV-M>   0Bh     4Dh     [23:16]      IREG_SP_HISSY_DATA[23:16]    Data PLP ISSY (Invalid at IREG_SP_PLP_MODE_DATA=1'b0)
             *  <SLV-M>   0Bh     4Eh     [15:8]       IREG_SP_HISSY_DATA[15:8]     Data PLP ISSY (Invalid at IREG_SP_PLP_MODE_DATA=1'b0)
             *  <SLV-M>   0Bh     4Fh     [7:0]        IREG_SP_HISSY_DATA[7:0]      Data PLP ISSY (Invalid at IREG_SP_PLP_MODE_DATA=1'b0)
             */
            addr = 0x42;
        }

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, addr, data, sizeof (data)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        SLVT_UnFreezeReg (pTunerDemod);

        /* MATYPE-1 */
        pBBHeader->streamInput = (sony_dvbt2_stream_t)((data[0] >> 6) & 0x03);
        pBBHeader->isSingleInputStream = (uint8_t)((data[0] >> 5) & 0x01);
        pBBHeader->isConstantCodingModulation = (uint8_t)((data[0] >> 4) & 0x01);
        pBBHeader->issyIndicator = (uint8_t)((data[0] >> 3) & 0x01);
        pBBHeader->nullPacketDeletion = (uint8_t)((data[0] >> 2) & 0x01);
        pBBHeader->ext = (uint8_t)(data[0] & 0x03);

        pBBHeader->inputStreamIdentifier = data[1]; /* MATYPE-2 */
        pBBHeader->plpMode = (data[3] & 0x01) ? SONY_DVBT2_PLP_MODE_HEM : SONY_DVBT2_PLP_MODE_NM;
        pBBHeader->dataFieldLength = (uint16_t)((data[4] << 8) | data[5]);

        if(pBBHeader->plpMode == SONY_DVBT2_PLP_MODE_NM){
            /* NM (Normal Mode) */
            pBBHeader->userPacketLength = (uint16_t)((data[6] << 8) | data[7]);
            pBBHeader->syncByte = data[8];
            pBBHeader->issy = 0; /* Invalid */
        }else{
            /* HEM (High Efficiency Mode) */
            pBBHeader->userPacketLength = 0; /* Invalid */
            pBBHeader->syncByte = 0; /* Invalid */
            pBBHeader->issy = (uint32_t)((data[11] << 16) | (data[12] << 8) | data[13]);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_InBandBTSRate (sony_tunerdemod_t * pTunerDemod,
                                                           sony_dvbt2_plp_btype_t type,
                                                           uint32_t * pTSRateBps)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_InBandBTSRate");

    if ((!pTunerDemod) || (!pTSRateBps)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the registers. */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        sony_result_t result = SONY_RESULT_OK;
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t unlockDetected = 0;

        /* Confirm SyncStat. */
        result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncState, &tsLock, &unlockDetected);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (result);
        }

        if (!tsLock) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }
    }

    /* Set SLV-T Bank : 0x22 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t l1PostOk = 0;
        uint8_t addr = 0;
        uint8_t data = 0;

        /* Check L1 Post is valid
         * slave    Bank    Addr    Bit    Name
         * ------------------------------------------
         * <SLV-M>  0Bh     86h     [0]    IL1POST_OK
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x86, &l1PostOk, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (!(l1PostOk & 0x01)) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* slave    Bank    Addr    Bit       Name
         * ----------------------------------------------------------------
         * <SLV-M>   0Bh     A7h     [7:0]        IL1POST_RESERVED_1[15:8]
         * <SLV-M>   0Bh     BAh     [7:0]        IL1POST_RESERVED_1_C[15:8]
         *
         * IL1POST_RESERVED_1,_C[15] : IN_BAND_B_FLAG
         */
        if (type == SONY_DVBT2_PLP_COMMON) {
            addr = 0xBA;
        } else {
            addr = 0xA7;
        }

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, addr, &data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if ((data & 0x80) == 0x00) {
            /* No in-band B sigalling (or no common PLP) */
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }
    }

    /* Set SLV-T Bank : 0x25 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x25) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t data[4];
        uint8_t addr = 0;

        /* in-band B signalling TS_RATE (from v1.2.1 onward)
         *  slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------------------
         *  <SLV-M>   25h     A6h     [2:0]        IREG_SP_IB_TS_RATE_COMMON[26:24]
         *  <SLV-M>   25h     A7h     [7:0]        IREG_SP_IB_TS_RATE_COMMON[23:16]
         *  <SLV-M>   25h     A8h     [7:0]        IREG_SP_IB_TS_RATE_COMMON[15:8]
         *  <SLV-M>   25h     A9h     [7:0]        IREG_SP_IB_TS_RATE_COMMON[7:0]
         *  <SLV-M>   25h     AAh     [2:0]        IREG_SP_IB_TS_RATE_DATA[26:24]
         *  <SLV-M>   25h     ABh     [7:0]        IREG_SP_IB_TS_RATE_DATA[23:16]
         *  <SLV-M>   25h     ACh     [7:0]        IREG_SP_IB_TS_RATE_DATA[15:8]
         *  <SLV-M>   25h     ADh     [7:0]        IREG_SP_IB_TS_RATE_DATA[7:0]
         */
        if (type == SONY_DVBT2_PLP_COMMON) {
            addr = 0xA6;
        } else {
            addr = 0xAA;
        }

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, addr, &data[0], 4) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        *pTSRateBps = (uint32_t)(((data[0] & 0x07) << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_SpectrumSense (sony_tunerdemod_t * pTunerDemod, sony_tunerdemod_spectrum_sense_t * pSense)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t syncState   = 0;
    uint8_t tsLock      = 0;
    uint8_t earlyUnlock = 0;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_SpectrumSense");

    if ((!pTunerDemod) || (!pSense)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the register. */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncState, &tsLock, &earlyUnlock);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (result);
    }

    /* Only valid after lock. */
    if (syncState != 6) {
        SLVT_UnFreezeReg (pTunerDemod);

        result = SONY_RESULT_ERROR_HW_STATE;

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Check for sub. */
            result = sony_tunerdemod_dvbt2_monitor_SpectrumSense (pTunerDemod->pDiverSub, pSense);
        }

        SONY_TRACE_RETURN (result);
    }

    {
        uint8_t data = 0;

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name                mean
         * ---------------------------------------------------------------------------------
         * <SLV-T>   0Bh     2Fh    [0]        IREG_CRCG2_SINVP1      0:not invert,   1:invert
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x2F, &data, sizeof (data)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        SLVT_UnFreezeReg (pTunerDemod);

        *pSense = (data & 0x01) ? SONY_TUNERDEMOD_SPECTRUM_INV : SONY_TUNERDEMOD_SPECTRUM_NORMAL;
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t dvbt2_readSNRReg (sony_tunerdemod_t * pTunerDemod, uint16_t * pRegValue)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("dvbt2_readSNRReg");

    if ((!pTunerDemod) || (!pRegValue)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    {
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t unlockDetected = 0;
        uint8_t data[2];

        /* Freeze registers */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncState, &tsLock, &unlockDetected);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Confirm sequence state == 6. */
        if (syncState != 6) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   0Bh     13h     [7:0]        IREG_SNMON_OD[15:8]
         * <SLV-T>   0Bh     14h     [7:0]        IREG_SNMON_OD[7:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x13, data, sizeof (data)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        SLVT_UnFreezeReg (pTunerDemod);

        *pRegValue = (data[0] << 8) | data[1];
    }

    SONY_TRACE_RETURN (result);
}

static sony_result_t dvbt2_calcSNR (sony_tunerdemod_t * pTunerDemod, uint32_t regValue, int32_t * pSNR)
{
    SONY_TRACE_ENTER ("dvbt2_calcSNR");

    if ((!pTunerDemod) || (!pSNR)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Confirm valid range, clip as necessary */
    if (regValue == 0) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* IREG_SNMON_OD is clipped at a maximum of 10876.
     *
     * SNR = 10 * log10 (IREG_SNMON_OD / (12600 - IREG_SNMON_OD)) + 32.0
     *     = 10 * (log10(IREG_SNMON_OD) - log10(12600 - IREG_SNMON_OD)) + 32.0
     * sony_log10 returns log10(x) * 100
     * Therefore SNR(dB) * 1000 :
     *     = 10 * 10 * (sony_log10(IREG_SNMON_OD) - sony_log10(12600 - IREG_SNMON_OD) + 32000
     */
    if (regValue > 10876) {
        regValue = 10876;
    }

    *pSNR = 10 * 10 * ((int32_t) sony_math_log10 (regValue) - (int32_t) sony_math_log10 (12600 - regValue));
    *pSNR += 32000;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_SNR (sony_tunerdemod_t * pTunerDemod,
                                                 int32_t * pSNR)
{
    uint16_t regValue = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_SNR");

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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SINGLE) {
        result = dvbt2_readSNRReg (pTunerDemod, &regValue);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        result = dvbt2_calcSNR (pTunerDemod, regValue, pSNR);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    } else {
        int32_t snrMain = 0;
        int32_t snrSub = 0;

        result = sony_tunerdemod_dvbt2_monitor_SNR_diver (pTunerDemod, pSNR, &snrMain, &snrSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_SNR_diver (sony_tunerdemod_t * pTunerDemod,
                                                       int32_t * pSNR, int32_t * pSNRMain, int32_t * pSNRSub)
{
    uint16_t regValue = 0;
    uint32_t regValueSum = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_SNR_diver");

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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Check main IC */
    result = dvbt2_readSNRReg (pTunerDemod, &regValue);
    if (result == SONY_RESULT_OK) {
        result = dvbt2_calcSNR (pTunerDemod, regValue, pSNRMain);
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
    result = dvbt2_readSNRReg (pTunerDemod->pDiverSub, &regValue);
    if (result == SONY_RESULT_OK) {
        result = dvbt2_calcSNR (pTunerDemod->pDiverSub, regValue, pSNRSub);
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
    result = dvbt2_calcSNR (pTunerDemod, regValueSum, pSNR);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_PreLDPCBER (sony_tunerdemod_t * pTunerDemod, uint32_t * pBER)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t bitError = 0;
    uint32_t periodExp = 0;
    uint32_t n_ldpc = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_PreLDPCBER");

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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x0B */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-M>   0Bh     3Ch     [0]          ILBER_VALID
         * <SLV-M>   0Bh     3Dh     [3:0]        IBER1_BITERR[27:24]
         * <SLV-M>   0Bh     3Eh     [7:0]        IBER1_BITERR[23:16]
         * <SLV-M>   0Bh     3Fh     [7:0]        IBER1_BITERR[15:8]
         * <SLV-M>   0Bh     40h     [7:0]        IBER1_BITERR[7:0]
         * <SLV-M>   0Bh     A0h     [1:0]        IL1POST_PLP_FEC_TYPE[1:0]
         */
        uint8_t data[5];
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x3C, data, sizeof (data)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (!(data[0] & 0x01)) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        bitError = ((data[1] & 0x0F) << 24) | (data[2] << 16) | (data[3] << 8) | data[4];

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xA0, data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (((sony_dvbt2_plp_fec_t) (data[0] & 0x03)) == SONY_DVBT2_FEC_LDPC_16K) {
            /* Short. */
            n_ldpc = 16200;
        }
        else {
            /* Normal. */
            n_ldpc = 64800;
        }

        SLVT_UnFreezeReg (pTunerDemod);

        /* Set SLV-T Bank : 0x20 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x20) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Read measurement period.
         * slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-M>   20h     6fh     [3:0]        OREG_LBER_MES[3:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x6F, data, 1) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        periodExp = data[0] & 0x0F;
    }

    if (bitError > ((1U << periodExp) * n_ldpc)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    {
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        if (periodExp >= 4) {
            /*
              BER = bitError * 10000000 / (2^N * n_ldpc)
                  = bitError * 3125 / (2^(N-4) * (n_ldpc / 200))
                  (NOTE: 10000000 / 2^4 / 200 = 3125)
                  = bitError * 5 * 625 / (2^(N-4) * (n_ldpc / 200))
                  (Divide in 2 steps to prevent overflow.)
            */
            div = (1U << (periodExp - 4)) * (n_ldpc / 200);

            Q = (bitError * 5) / div;
            R = (bitError * 5) % div;

            R *= 625;
            Q = Q * 625 + R / div;
            R = R % div;
        }
        else {
            /*
              BER = bitError * 10000000 / (2^N * n_ldpc)
                  = bitError * 50000 / (2^N * (n_ldpc / 200))
                  = bitError * 10 * 5000 / (2^N * (n_ldpc / 200))
                  (Divide in 2 steps to prevent overflow.)
            */
            div = (1U << periodExp) * (n_ldpc / 200);

            Q = (bitError * 10) / div;
            R = (bitError * 10) % div;

            R *= 5000;
            Q = Q * 5000 + R / div;
            R = R % div;
        }

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

sony_result_t sony_tunerdemod_dvbt2_monitor_PostBCHFER (sony_tunerdemod_t * pTunerDemod, uint32_t * pFER)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t fecError = 0;
    uint32_t period = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_PostBCHFER");

    if ((!pTunerDemod) || (!pFER)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x0B */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t data[2];

        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-M>   0Bh     1Bh     [7]           IBBER_VALID
         * <SLV-M>   0Bh     1Bh     [6:0]         IBER2_FBERR[14:8]
         * <SLV-M>   0Bh     1Ch     [7:0]         IBER2_FBERR[7:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1B, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (!(data[0] & 0x80)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        fecError = ((data[0] & 0x7F) << 8) | (data[1]);

        /* Set SLV-T Bank : 0x20 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x20) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Read measurement period.
         * slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-M>   20h     72h     [3:0]        OREG_BBER_MES[3:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x72, data, 1) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Period = 2^BBER_MES */
        period = (1 << (data[0] & 0x0F));
    }

    if ((period == 0) || (fecError > period)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /*
      FER = fecError * 1000000 / period
          = fecError * 1000 * 1000 / period
          (Divide in 2 steps to prevent overflow.)
    */
    {
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        div = period;

        Q = (fecError * 1000) / div;
        R = (fecError * 1000) % div;

        R *= 1000;
        Q = Q * 1000 + R / div;
        R = R % div;

        /* rounding */
        if ((div != 1) && (R >= div/2)) {
            *pFER = Q + 1;
        }
        else {
            *pFER = Q;
        }
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_PreBCHBER (sony_tunerdemod_t * pTunerDemod, uint32_t * pBER)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t bitError = 0;
    uint32_t periodExp = 0;
    uint32_t n_bch = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_PreBCHBER");

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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Monitor the BER parameters. */
    {
        uint8_t data[3];
        sony_dvbt2_plp_fec_t plpFecType = SONY_DVBT2_FEC_LDPC_16K;
        sony_dvbt2_plp_code_rate_t plpCr = SONY_DVBT2_R1_2;

        static const uint16_t nBCHBitsLookup[2][8] = {
          /* R1_2   R3_5   R2_3   R3_4   R4_5   R5_6   R1_3   R2_5 */
            {7200,  9720,  10800, 11880, 12600, 13320, 5400,  6480}, /* 16K FEC */
            {32400, 38880, 43200, 48600, 51840, 54000, 21600, 25920} /* 64k FEC */
        };

        /* Freeze registers */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-M>   0Bh     15h     [6]          IBBER_VALID
         * <SLV-M>   0Bh     15h     [5:0]        IBER0_BITERR[21:16]
         * <SLV-M>   0Bh     16h     [7:0]        IBER0_BITERR[15:8]
         * <SLV-M>   0Bh     17h     [7:0]        IBER0_BITERR[7:0]
         * <SLV-M>   0Bh     9Dh     [2:0]        IL1POST_PLP_COD[2:0]
         * <SLV-M>   0Bh     A0h     [1:0]        IL1POST_PLP_FEC_TYPE[1:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x15, data, 3) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (!(data[0] & 0x40)) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE); /* Not Ready */
        }

        bitError = ((data[0] & 0x3F) << 16) | (data[1] << 8) | data[2];

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x9D, data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        plpCr = (sony_dvbt2_plp_code_rate_t) (data[0] & 0x07);

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xA0, data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        plpFecType = (sony_dvbt2_plp_fec_t) (data[0] & 0x03);

        SLVT_UnFreezeReg (pTunerDemod);

        /* Set SLV-T Bank : 0x20 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x20) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Read measurement period.
         * slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-M>   20h     72h     [3:0]        OREG_BBER_MES[3:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x72, data, 1) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Period (4bit) */
        periodExp = data[0] & 0x0F;

        /* Confirm FEC Type / Code Rate */
        if ((plpFecType > SONY_DVBT2_FEC_LDPC_64K) || (plpCr > SONY_DVBT2_R2_5)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        n_bch = nBCHBitsLookup[plpFecType][plpCr];
    }

    if (bitError > ((1U << periodExp) * n_bch)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    {
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        if (periodExp >= 6) {
            /*
              BER = bitError * 1000000000 / (2^N * n_bch)
                  = bitError * 390625 / (2^(N-6) * (n_bch / 40))
                  (NOTE: 1000000000 / 2^6 / 40 = 390625)
                  = bitError * 625 * 625 / (2^(N-6) * (n_bch / 40))
                  (Divide in 2 steps to prevent overflow.)
            */
            div = (1U << (periodExp - 6)) * (n_bch / 40);

            Q = (bitError * 625) / div;
            R = (bitError * 625) % div;

            R *= 625;
            Q = Q * 625 + R / div;
            R = R % div;
        }
        else {
            /*
              BER = bitError * 1000000000 / (2^N * n_bch)
                  = bitError * 25000000 / (2^N * (n_bch / 40))
                  = bitError * 1000 * 25000 / (2^N * (n_bch / 40))
                  (Divide in 2 steps to prevent overflow.)
            */
            div = (1U << periodExp) * (n_bch / 40);

            Q = (bitError * 1000) / div;
            R = (bitError * 1000) % div;

            R *= 25000;
            Q = Q * 25000 + R / div;
            R = R % div;
        }

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

sony_result_t sony_tunerdemod_dvbt2_monitor_PacketErrorNumber (sony_tunerdemod_t * pTunerDemod, uint32_t * pPEN)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t data[3];

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_PacketErrorNumber");

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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x0B */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

   /* slave    Bank    Addr    Bit              Name
    * ------------------------------------------------------------
    * <SLV-M>   0Bh     39h     [0]          IREG_SP_BERT_VALID
    * <SLV-M>   0Bh     3Ah     [7:0]        IREG_SP_BERT_CWRJCTCNT[15:8]
    * <SLV-M>   0Bh     3Bh     [7:0]        IREG_SP_BERT_CWRJCTCNT[7:0]
    */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x39, data, sizeof (data)) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (!(data[0] & 0x01) ) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    *pPEN =  ((data[1] << 8) | data[2]);

    SONY_TRACE_RETURN (result);
}


sony_result_t sony_tunerdemod_dvbt2_monitor_SamplingOffset (sony_tunerdemod_t * pTunerDemod, int32_t * pPPM)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_SamplingOffset");

    if ((!pTunerDemod) || (!pPPM)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t ctlValReg[5];
        uint8_t nominalRateReg[5];
        uint32_t trlCtlVal = 0;
        uint32_t trcgNominalRate = 0;
        int32_t num;
        int32_t den;
        sony_result_t result = SONY_RESULT_OK;
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t unlockDetected = 0;
        int8_t diffUpper = 0;

        /* Freeze registers */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncState, &tsLock, &unlockDetected);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (syncState != 6) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* slave    Bank    Addr    Bit              name
         * ---------------------------------------------------------------
         * <SLV-T>   0Bh     34h     [6:0]      IREG_TRL_CTLVAL_S[38:32]
         * <SLV-T>   0Bh     35h     [7:0]      IREG_TRL_CTLVAL_S[31:24]
         * <SLV-T>   0Bh     36h     [7:0]      IREG_TRL_CTLVAL_S[23:16]
         * <SLV-T>   0Bh     37h     [7:0]      IREG_TRL_CTLVAL_S[15:8]
         * <SLV-T>   0Bh     38h     [7:0]      IREG_TRL_CTLVAL_S[7:0]
         * <SLV-T>   04h     10h     [6:0]      OREG_TRCG_NOMINALRATE[38:32]
         * <SLV-T>   04h     11h     [7:0]      OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>   04h     12h     [7:0]      OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>   04h     13h     [7:0]      OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>   04h     14h     [7:0]      OREG_TRCG_NOMINALRATE[7:0]
         */
        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x34, ctlValReg, sizeof (ctlValReg)) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Set SLV-T Bank : 0x04 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, nominalRateReg, sizeof (nominalRateReg)) != SONY_RESULT_OK) {
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

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_SamplingOffset_sub (sony_tunerdemod_t * pTunerDemod,
                                                                int32_t * pPPM)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_SamplingOffset_sub");

    if ((!pTunerDemod) || (!pPPM)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_dvbt2_monitor_SamplingOffset (pTunerDemod->pDiverSub, pPPM);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_Quality (sony_tunerdemod_t * pTunerDemod, uint8_t * pQuality)
{
    sony_result_t result = SONY_RESULT_OK;
    int32_t snr = 0;
    int32_t snrRel = 0;
    uint32_t ber = 0;
    uint32_t berSQI = 0;
    sony_dvbt2_plp_constell_t qam;
    sony_dvbt2_plp_code_rate_t codeRate;

    static const int32_t snrNordigP1dB1000[4][8] = {
    /*   1/2,   3/5,    2/3,    3/4,    4/5,    5/6,   1/3,   2/5                */
        {3500,  4700,   5600,   6600,   7200,   7700,  1300,  2200 }, /* QPSK    */
        {8700,  10100,  11400,  12500,  13300,  13800, 6000,  7200 }, /* 16-QAM  */
        {13000, 14800,  16200,  17700,  18700,  19400, 9800,  11100}, /* 64-QAM  */
        {17000, 19400,  20800,  22900,  24300,  25100, 13200, 14800}, /* 256-QAM */
    };

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_Quality");

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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get Pre BCH BER. */
    result = sony_tunerdemod_dvbt2_monitor_PreBCHBER (pTunerDemod, &ber);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get SNR */
    result = sony_tunerdemod_dvbt2_monitor_SNR (pTunerDemod, &snr);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get PLP constellation */
    result = sony_tunerdemod_dvbt2_monitor_QAM (pTunerDemod, SONY_DVBT2_PLP_DATA, &qam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get PLP code rate */
    result = sony_tunerdemod_dvbt2_monitor_CodeRate (pTunerDemod, SONY_DVBT2_PLP_DATA, &codeRate);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Ensure correct plp info. */
    if ((codeRate > SONY_DVBT2_R2_5) || (qam > SONY_DVBT2_QAM256)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
    }

    /* BER_SQI Calculated from:
     * if (Pre-BCH BER > 10^-4)            BER_SQI = 0
     * if (10^-4 >= Pre-BCH BER >= 10^-7)  BER_SQI = 100/15 = 6.667
     * if (Pre-BCH BER < 10^-7)            BER_SQI = 100/6  = 16.667
     *
     * Note : Pre-BCH BER is scaled by 10^9
     */
    if (ber > 100000) {
        berSQI = 0;
    }
    else if (ber >= 100) {
        berSQI = 6667;
    }
    else {
        berSQI = 16667;
    }

    /* C/Nrel = C/Nrec - C/Nnordigp1 */
    snrRel = snr - snrNordigP1dB1000[qam][codeRate];

    /* SQI (Signal Quality Indicator) given by:
     * if (C/Nrel < -3dB)         SQI = 0
     * if (-3dB <= CNrel <= 3dB)  SQI = (C/Nrel + 3) * BER_SQI
     * if (CNrel > 3dB)           SQI = 100
     */
    if (snrRel < -3000) {
        *pQuality = 0;
    }
    else if (snrRel <= 3000) {
        /* snrRel and berSQI scaled by 10^3 so divide by 10^6 */
        uint32_t tempSQI = (((snrRel + 3000) * berSQI) + 500000) / 1000000;
        /* Clip value to 100% */
        *pQuality = (tempSQI > 100)? 100 : (uint8_t) tempSQI;
    }
    else {
        *pQuality = 100;
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_TSRate (sony_tunerdemod_t * pTunerDemod, uint32_t * pTSRateKbps)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t rd_smooth_dp = 0;
    uint32_t ep_ck_nume = 0;
    uint32_t ep_ck_deno = 0;
    uint8_t issy_on_data = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_TSRate");

    if ((!pTunerDemod) || (!pTSRateKbps)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[12];
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t unlockDetected = 0;
        /* Freeze registers */
        if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncState, &tsLock, &unlockDetected);
        if (result != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Check TS lock. */
        if (!tsLock) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------------------
         * <SLV-M>   0Bh     23h     [4:0]        IREG_SP_RD_SMOOTH_DP[28:24]
         * <SLV-M>   0Bh     24h     [7:0]        IREG_SP_RD_SMOOTH_DP[23:16]
         * <SLV-M>   0Bh     25h     [7:0]        IREG_SP_RD_SMOOTH_DP[15:8]
         * <SLV-M>   0Bh     26h     [7:0]        IREG_SP_RD_SMOOTH_DP[7:0]
         * <SLV-M>   0Bh     27h     [5:0]        IREG_SP_EP_CK_NUME[29:24]
         * <SLV-M>   0Bh     28h     [7:0]        IREG_SP_EP_CK_NUME[23:16]
         * <SLV-M>   0Bh     29h     [7:0]        IREG_SP_EP_CK_NUME[15:8]
         * <SLV-M>   0Bh     2Ah     [7:0]        IREG_SP_EP_CK_NUME[7:0]
         * <SLV-M>   0Bh     2Bh     [5:0]        IREG_SP_EP_CK_DENO[29:24]
         * <SLV-M>   0Bh     2Ch     [7:0]        IREG_SP_EP_CK_DENO[23:16]
         * <SLV-M>   0Bh     2Dh     [7:0]        IREG_SP_EP_CK_DENO[15:8]
         * <SLV-M>   0Bh     2Eh     [7:0]        IREG_SP_EP_CK_DENO[7:0]
         * <SLV-M>   0Bh     41h     [0]          IREG_SP_ISSY_ON_DATA
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x23, data, 12) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        rd_smooth_dp = (uint32_t) ((data[0] & 0x1F) << 24);
        rd_smooth_dp |= (uint32_t) (data[1] << 16);
        rd_smooth_dp |= (uint32_t) (data[2] << 8);
        rd_smooth_dp |= (uint32_t) data[3];

        /* rd_smooth_dp should be > 214958 = < 100Mbps */
        if (rd_smooth_dp < 214958) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        ep_ck_nume = (uint32_t) ((data[4] & 0x3F) << 24);
        ep_ck_nume |= (uint32_t) (data[5] << 16);
        ep_ck_nume |= (uint32_t) (data[6] << 8);
        ep_ck_nume |= (uint32_t) data[7];

        ep_ck_deno = (uint32_t) ((data[8] & 0x3F) << 24);
        ep_ck_deno |= (uint32_t) (data[9] << 16);
        ep_ck_deno |= (uint32_t) (data[10] << 8);
        ep_ck_deno |= (uint32_t) data[11];

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x41, data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        issy_on_data = data[0] & 0x01;

        /* Unfreeze registers */
        SLVT_UnFreezeReg (pTunerDemod);
    }

    if (issy_on_data) {
        if ((ep_ck_deno == 0) || (ep_ck_nume == 0) || (ep_ck_deno >= ep_ck_nume)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }
    }

    /* IREG_SP_ISSY_ON_DATA    TS Data output rate[Mbps]
     * ------------------------------------------------------------------------------------------------------------
     *          0              ick * 2^18 / IREG_SP_RD_SMOOTH_DP
     *          1              (ick * 2^18 / IREG_SP_RD_SMOOTH_DP ) * ( IREG_SP_EP_CK_DENO / IREG_SP_EP_CK_NUME )
     *
     * CLK mode A : ick = 82.28
     *          B : ick = 93.30
     *          C : ick = 96.00
     *
     * TSRate = ick * 2^18 / IREG_SP_RD_SMOOTH_DP (Mbps)
     *        = ick * 2^18 * 10^3 / IREG_SP_RD_SMOOTH_DP (kbps)
     */
    {
        /* Calculate ick * 2^18 * 10^3 / IREG_SP_RD_SMOOTH_DP */
        /* Divide in 3 steps to prevent overflow */
        uint32_t ick_x100;
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        switch (pTunerDemod->clockMode) {
        case SONY_TUNERDEMOD_CLOCKMODE_A:
            ick_x100 = 8228;
            break;
        case SONY_TUNERDEMOD_CLOCKMODE_B:
            ick_x100 = 9330;
            break;
        case SONY_TUNERDEMOD_CLOCKMODE_C:
            ick_x100 = 9600;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        div = rd_smooth_dp;

        Q = ick_x100 * 262144U / div;
        R = ick_x100 * 262144U % div;

        R *= 5U;
        Q = Q * 5 + R / div;
        R = R % div;

        R *= 2U;
        Q = Q * 2 + R / div;
        R = R % div;

        /* Round up based on the remainder */
        if (R >= div/2) {
            *pTSRateKbps = Q + 1;
        }
        else {
            *pTSRateKbps = Q;
        }
    }

    if (issy_on_data) {
        /* TSRate *= DENO / NUME
           (NOTE: 0.88 <= (DENO / NUME) < 1.0)
           TSRate *= 1 - (NUME - DENO) / NUME
           TSRate -= TSRate * (NUME - DENO) / NUME

           Calculating
           TSRate * (NUME - DENO) / NUME
            = TSRate * (((NUME - DENO) >> N) / (NUME >> N)) (To avoid overflow) */

        uint32_t diff = ep_ck_nume - ep_ck_deno;

        /* TSRate < 100000 (100Mbps), 100000 * 0x7FFF = 0xC34E7960 (32bit) */
        while(diff > 0x7FFF){
            diff >>= 1;
            ep_ck_nume >>= 1;
        }

        *pTSRateKbps -= (*pTSRateKbps * diff + ep_ck_nume/2) / ep_ck_nume;
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_PER (sony_tunerdemod_t * pTunerDemod, uint32_t * pPER)
{
    sony_result_t result = SONY_RESULT_OK;
    uint32_t packetError = 0;
    uint32_t period = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_PER");

    if (!pTunerDemod || !pPER) {
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
    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t rdata[3];

        /* Set SLV-T Bank : 0x0B */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /*  slave    Bank    Addr    Bit     Name
         * ---------------------------------------------------------------------------------
         * <SLV-M>   0Bh     18h      [0]    IREG_SP_B2PER_VALID
         * <SLV-M>   0Bh     19h    [7:0]    IREG_SP_B2PER_PKTERR[15:8]
         * <SLV-M>   0Bh     1Ah    [7:0]    IREG_SP_B2PER_PKTERR[7:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x18, rdata, 3) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if ((rdata[0] & 0x01) == 0) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);    /* Not ready... */
        }

        packetError = (rdata[1] << 8) | rdata[2];

        /* Set SLV-T Bank : 0x24 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x24) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /*  slave    Bank    Addr    Bit     Name
         * ---------------------------------------------------------------------------------
         * <SLV-M>   24h     DCh    [3:0]    OREG_SP_PER_MES[3:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xDC, rdata, 1) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        period = 1U << (rdata[0] & 0x0F);
    }

    if ((period == 0) || (packetError > period)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /*
      PER = IREG_SP_B2PER_PKTERR / ( 2 ^ OREG_SP_PER_MES )

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

sony_result_t sony_tunerdemod_dvbt2_monitor_QAM (sony_tunerdemod_t * pTunerDemod,
                                                 sony_dvbt2_plp_btype_t type,
                                                 sony_dvbt2_plp_constell_t * pQAM)
{
    uint8_t data;
    uint8_t l1PostOk = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_QAM");

    if ((!pTunerDemod) || (!pQAM)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the registers. */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x0B */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Check L1 Post is valid
     * slave    Bank    Addr    Bit    Name
     * ------------------------------------------
     * <SLV-M>  0Bh     86h     [0]    IL1POST_OK
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x86, &l1PostOk, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (!(l1PostOk & 0x01)) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    if (type == SONY_DVBT2_PLP_COMMON) {
        /* slave    Bank    Addr    Bit       Name
         * ----------------------------------------------------------------
         * <SLV-M>   0Bh     B6h     [7:0]    IL1POST_FRAME_INTERVAL_C[7:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xB6, &data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* If common, check for no common PLP, frame_interval == 0. */
        if (data == 0) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* slave    Bank    Addr    Bit           Name
         * -----------------------------------------------------------
         * <SLV-M>   0Bh     B1h     [2:0]        IL1POST_PLP_MOD_C[2:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xB1, &data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }
    else {
        /* slave    Bank    Addr    Bit           Name
         * -----------------------------------------------------------
         * <SLV-M>   0Bh     9Eh     [2:0]        IL1POST_PLP_MOD[2:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x9E, &data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* Unfreeze registers */
    SLVT_UnFreezeReg (pTunerDemod);

    /* Obtain PLP constellation from register value */
    *pQAM = (sony_dvbt2_plp_constell_t) (data & 0x07);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_CodeRate (sony_tunerdemod_t * pTunerDemod,
                                                      sony_dvbt2_plp_btype_t type,
                                                      sony_dvbt2_plp_code_rate_t * pCodeRate)
{
    uint8_t data;
    uint8_t l1PostOk = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_CodeRate");

    if ((!pTunerDemod) || (!pCodeRate)) {
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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the registers. */
    if (SLVT_FreezeReg (pTunerDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x0B */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Check L1 Post is valid
     * slave    Bank    Addr    Bit    Name
     * ------------------------------------------
     * <SLV-M>  0Bh     86h     [0]    IL1POST_OK
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x86, &l1PostOk, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (!(l1PostOk & 0x01)) {
        SLVT_UnFreezeReg (pTunerDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    if (type == SONY_DVBT2_PLP_COMMON) {
        /* slave    Bank    Addr    Bit       Name
         * ----------------------------------------------------------------
         * <SLV-M>   0Bh     B6h     [7:0]    IL1POST_FRAME_INTERVAL_C[7:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xB6, &data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* If common, check for no common PLP, frame_interval == 0. */
        if (data == 0) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }

        /* slave    Bank    Addr    Bit           Name
         * -----------------------------------------------------------
         * <SLV-M>   0Bh     B0h     [2:0]        IL1POST_PLP_COD_C[2:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xB0, &data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }
    else {
        /* slave    Bank    Addr    Bit           Name
         * -----------------------------------------------------------
         * <SLV-M>   0Bh     9Dh     [2:0]        IL1POST_PLP_COD[2:0]
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x9D, &data, 1) != SONY_RESULT_OK) {
            SLVT_UnFreezeReg (pTunerDemod);
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* Unfreeze registers */
    SLVT_UnFreezeReg (pTunerDemod);

    /* Obtain PLP code rate from register value */
    *pCodeRate = (sony_dvbt2_plp_code_rate_t) (data & 0x07);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_Profile (sony_tunerdemod_t * pTunerDemod, sony_dvbt2_profile_t * pProfile)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_Profile");

    if ((!pTunerDemod) || (!pProfile)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x0B */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t data;
        /* slave    Bank    Addr    Bit         Name                Meaning
         * ---------------------------------------------------------------------------
         * <SLV-T>   0Bh     22h     [1]        IREG_T2MODE_OK      0:Invalid, 1:Valid
         * <SLV-T>   0Bh     22h     [0]        IREG_T2MODE         0:Base, 1:Lite
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x22, &data, sizeof (data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (data & 0x02) {
            /* T2 profile is valid */
            if (data & 0x01) {
                *pProfile = SONY_DVBT2_PROFILE_LITE;
            }
            else {
                *pProfile = SONY_DVBT2_PROFILE_BASE;
            }
        }
        else {
            sony_result_t result = SONY_RESULT_ERROR_HW_STATE;

            if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
                /* Check for sub. */
                result = sony_tunerdemod_dvbt2_monitor_Profile (pTunerDemod->pDiverSub, pProfile);
            }

            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t dvbt2_CalcSSI (sony_tunerdemod_t * pTunerDemod, int32_t rfLevel, uint8_t * pSSI)
{
    sony_dvbt2_plp_constell_t qam;
    sony_dvbt2_plp_code_rate_t codeRate;
    int32_t prel;
    int32_t tempSSI = 0;
    sony_result_t result = SONY_RESULT_OK;

    static const int32_t pRefdBm1000[4][8] = {
    /*    1/2,    3/5,    2/3,    3/4,    4/5,    5/6,    1/3,    2/5                */
        {-96000, -95000, -94000, -93000, -92000, -92000, -98000, -97000}, /* QPSK    */
        {-91000, -89000, -88000, -87000, -86000, -86000, -93000, -92000}, /* 16-QAM  */
        {-86000, -85000, -83000, -82000, -81000, -80000, -89000, -88000}, /* 64-QAM  */
        {-82000, -80000, -78000, -76000, -75000, -74000, -86000, -84000}, /* 256-QAM */
    };

    SONY_TRACE_ENTER ("dvbt2_CalcSSI");

    if ((!pTunerDemod) || (!pSSI)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Get PLP constellation */
    result = sony_tunerdemod_dvbt2_monitor_QAM (pTunerDemod, SONY_DVBT2_PLP_DATA, &qam);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Get PLP code rate */
    result = sony_tunerdemod_dvbt2_monitor_CodeRate (pTunerDemod, SONY_DVBT2_PLP_DATA, &codeRate);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Ensure correct plp info. */
    if ((codeRate > SONY_DVBT2_R2_5) || (qam > SONY_DVBT2_QAM256)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
    }

    /* prel = prec - pref */
    prel = rfLevel - pRefdBm1000[qam][codeRate];

    /* SSI (Signal Strength Indicator) is calculated from:
     *
     * if (prel < -15dB)              SSI = 0
     * if (-15dB <= prel < 0dB)       SSI = (2/3) * (prel + 15)
     * if (0dB <= prel < 20dB)        SSI = 4 * prel + 10
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

sony_result_t sony_tunerdemod_dvbt2_monitor_SSI (sony_tunerdemod_t * pTunerDemod, uint8_t * pSSI)
{
    int32_t rfLevel = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_SSI");

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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get estimated RF Level */
    result = sony_tunerdemod_monitor_RFLevel (pTunerDemod, &rfLevel);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Calculate SSI */
    result = dvbt2_CalcSSI (pTunerDemod, rfLevel, pSSI);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_monitor_SSI_sub (sony_tunerdemod_t * pTunerDemod, uint8_t * pSSI)
{
    int32_t rfLevel = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_monitor_SSI_sub");

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

    if (pTunerDemod->system != SONY_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Get estimated sub RF Level */
    result = sony_tunerdemod_monitor_RFLevel (pTunerDemod->pDiverSub, &rfLevel);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Calculate SSI */
    result = dvbt2_CalcSSI (pTunerDemod, rfLevel, pSSI);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

#undef uint32_t 
#undef int32_t 
#undef int8_t 