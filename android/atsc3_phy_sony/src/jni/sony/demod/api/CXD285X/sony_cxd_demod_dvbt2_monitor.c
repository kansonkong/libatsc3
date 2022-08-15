/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2017/05/11
  Modification ID : c9ab4bdba7445aaeceedc8912c71f6d4fce10b7b
------------------------------------------------------------------------------*/

#include "sony_cxd_math.h"
#include "sony_cxd_demod_dvbt2_monitor.h"

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_SyncStat (sony_cxd_demod_t * pDemod, uint8_t * pSyncStat, uint8_t * pTSLockStat, uint8_t * pUnlockDetected)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_SyncStat");

    if ((!pDemod) || (!pSyncStat) || (!pTSLockStat) || (!pUnlockDetected)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x20 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        uint8_t data;
        /* slave    Bank    Addr    Bit              Name                Meaning
         * -------------------------------------------------------------------
         * <SLV-T>   20h     10h     [2:0]        IREG_SEQ_OSTATE     0-5:UNLOCK 6:LOCK
         * <SLV-T>   20h     10h     [4]          IEARLY_NOOFDM       1: No OFDM
         * <SLV-T>   20h     10h     [5]          IREG_SP_TSLOCK      0:UNLOCK 1:LOCK
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, &data, sizeof (data)) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        *pSyncStat = data & 0x07;
        *pTSLockStat = ((data & 0x20) ? 1 : 0);
        *pUnlockDetected = ((data & 0x10) ? 1 : 0);
    }

    /* Check for valid SyncStat value. */
    if (*pSyncStat == 0x07){
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_CarrierOffset (sony_cxd_demod_t * pDemod, int32_t * pOffset)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_CarrierOffset");

    if ((!pDemod) || (!pOffset)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[4];
        uint32_t ctlVal = 0;
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t earlyLock = 0;

        /* Freeze registers */
        if (SLVT_CXD_FreezeReg (pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &syncState, &tsLock, &earlyLock);
        if (result != sony_cxd_RESULT_OK) {
            SLVT_CXD_UnFreezeReg (pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (syncState != 6) {
            SLVT_CXD_UnFreezeReg (pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-T Bank : 0x20 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
            SLVT_CXD_UnFreezeReg (pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name
         * ---------------------------------------------------------------
         * <SLV-T>   20h     4Ch     [3:0]      IREG_CRCG_CTLVAL[27:24]
         * <SLV-T>   20h     4Dh     [7:0]      IREG_CRCG_CTLVAL[23:16]
         * <SLV-T>   20h     4Eh     [7:0]      IREG_CRCG_CTLVAL[15:8]
         * <SLV-T>   20h     4Fh     [7:0]      IREG_CRCG_CTLVAL[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4C, data, sizeof (data)) != sony_cxd_RESULT_OK) {
            SLVT_CXD_UnFreezeReg (pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        SLVT_CXD_UnFreezeReg (pDemod);

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
        *pOffset = sony_cxd_Convert2SComplement (ctlVal, 28);

        switch (pDemod->bandwidth) {
        case sony_cxd_DTV_BW_1_7_MHZ:
            *pOffset = -1 * ((*pOffset) / 582);
            break;
        case sony_cxd_DTV_BW_5_MHZ:
        case sony_cxd_DTV_BW_6_MHZ:
        case sony_cxd_DTV_BW_7_MHZ:
        case sony_cxd_DTV_BW_8_MHZ:
            *pOffset = -1 * ((*pOffset) * (uint8_t)pDemod->bandwidth / 940);
            break;
        default:
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
        }
    }

    /* Compensate for inverted spectrum. */
    {
        /* Inverted RF spectrum with non-inverting tuner architecture. */
        if (pDemod->confSense == sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_INV) {
            *pOffset *= -1;
        }
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_IFAGCOut (sony_cxd_demod_t * pDemod, uint32_t * pIFAGC)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_IFAGCOut");

    if ((!pDemod) || (!pIFAGC)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x20 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* slave    Bank    Addr    Bit              Name
     * --------------------------------------------------------
     * <SLV-T>   20h     26h     [3:0]        IIFAGC_OUT[11:8]
     * <SLV-T>   20h     27h     [7:0]        IIFAGC_OUT[7:0]
     */
    {
        uint8_t data[2];
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x26, data, sizeof (data)) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        *pIFAGC = ((data[0] & 0x0F) << 8) | (data[1]);
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_L1Pre (sony_cxd_demod_t * pDemod, sony_cxd_dvbt2_l1pre_t * pL1Pre)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_L1Pre");

    if ((!pDemod) || (!pL1Pre)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[37];
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t earlyLock = 0;
        uint8_t version = 0;
        sony_cxd_dvbt2_profile_t profile;

        /* Freeze the register. */
        if (SLVT_CXD_FreezeReg (pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &syncState, &tsLock, &earlyLock);
        if (result != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (result);
        }

        /* Only valid after L1-pre locked. */
        if (syncState < 5) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Obtain current profile, used for determining the FFT size */
        result = sony_cxd_demod_dvbt2_monitor_Profile(pDemod, &profile);
        if (result != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (result);
        }

       /* Set SLV-T Bank : 0x22 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------------------
         * <SLV-T>   22h     20h     [7:0]        IL1PRE_TYPE[7:0]
         * <SLV-T>   22h     21h     [0]          IL1PRE_BWT_EXT
         * <SLV-T>   22h     22h     [2:0]        IL1PRE_S1[2:0]
         * <SLV-T>   22h     23h     [3:0]        IL1PRE_S2[3:0]
         * <SLV-T>   22h     24h     [0]          IL1PRE_L1_REPETITION_FLAG
         * <SLV-T>   22h     25h     [2:0]        IL1PRE_GUARD_INTERVAL[2:0]
         * <SLV-T>   22h     26h     [3:0]        IL1PRE_PAPR[3:0]
         * <SLV-T>   22h     27h     [3:0]        IL1PRE_L1_MOD[3:0]
         * <SLV-T>   22h     28h     [1:0]        IL1PRE_L1_COD[1:0]
         * <SLV-T>   22h     29h     [1:0]        IL1PRE_L1_FEC_TYPE[1:0]
         * <SLV-T>   22h     2Ah     [1:0]        IL1PRE_L1_POST_SIZE[17:16]
         * <SLV-T>   22h     2Bh     [7:0]        IL1PRE_L1_POST_SIZE[15:8]
         * <SLV-T>   22h     2Ch     [7:0]        IL1PRE_L1_POST_SIZE[7:0]
         * <SLV-T>   22h     2Dh     [1:0]        IL1PRE_L1_POST_INFO_SIZE[17:16]
         * <SLV-T>   22h     2Eh     [7:0]        IL1PRE_L1_POST_INFO_SIZE[15:8]
         * <SLV-T>   22h     2Fh     [7:0]        IL1PRE_L1_POST_INFO_SIZE[7:0]
         * <SLV-T>   22h     30h     [3:0]        IL1PRE_PILOT_PATTERN[3:0]
         * <SLV-T>   22h     31h     [7:0]        IL1PRE_TX_ID_AVAILABILITY[7:0]
         * <SLV-T>   22h     32h     [7:0]        IL1PRE_CELL_ID[15:8]
         * <SLV-T>   22h     33h     [7:0]        IL1PRE_CELL_ID[7:0]
         * <SLV-T>   22h     34h     [7:0]        IL1PRE_NETWORK_ID[15:8]
         * <SLV-T>   22h     35h     [7:0]        IL1PRE_NETWORK_ID[7:0]
         * <SLV-T>   22h     36h     [7:0]        IL1PRE_T2_SYSTEM_ID[15:8]
         * <SLV-T>   22h     37h     [7:0]        IL1PRE_T2_SYSTEM_ID[7:0]
         * <SLV-T>   22h     38h     [7:0]        IL1PRE_NUM_T2_FRAMES[7:0]
         * <SLV-T>   22h     39h     [3:0]        IL1PRE_NUM_DATA_SYMBOLS[11:8]
         * <SLV-T>   22h     3Ah     [7:0]        IL1PRE_NUM_DATA_SYMBOLS[7:0]
         * <SLV-T>   22h     3Bh     [2:0]        IL1PRE_REGEN_FLAG[2:0]
         * <SLV-T>   22h     3Ch     [0]          IL1PRE_L1_POST_EXTENSION
         * <SLV-T>   22h     3Dh     [2:0]        IL1PRE_NUM_RF[2:0]
         * <SLV-T>   22h     3Eh     [2:0]        IL1PRE_CURRENT_RF_IDX[2:0]
         * <SLV-T>   22h     3Fh     [1:0]        IL1PRE_T2_VERSION[3:2]
         * <SLV-T>   22h     40h     [7:0]        IL1PRE_T2_VERSION[1:0], IL1PRE_L1_POST_SCRAMBLED, IL1PRE_T2_BASE_LITE, IL1PRE_RESERVED[3:0]
         * <SLV-T>   22h     41h     [7:0]        IL1PRE_CRC_32[31:24]
         * <SLV-T>   22h     42h     [7:0]        IL1PRE_CRC_32[23:16]
         * <SLV-T>   22h     43h     [7:0]        IL1PRE_CRC_32[15:8]
         * <SLV-T>   22h     44h     [7:0]        IL1PRE_CRC_32[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x20, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
		SLVT_CXD_UnFreezeReg(pDemod);

        /* Convert data to appropriate format. */
        pL1Pre->type = (sony_cxd_dvbt2_l1pre_type_t) data[0];
        pL1Pre->bwExt = data[1] & 0x01;
        pL1Pre->s1 = (sony_cxd_dvbt2_s1_t) (data[2] & 0x07);
        pL1Pre->s2 = data[3] & 0x0F;
        pL1Pre->l1Rep = data[4] & 0x01;
        pL1Pre->gi = (sony_cxd_dvbt2_guard_t) (data[5] & 0x07);
        pL1Pre->papr = (sony_cxd_dvbt2_papr_t) (data[6] & 0x0F);
        pL1Pre->mod = (sony_cxd_dvbt2_l1post_constell_t) (data[7] & 0x0F);
        pL1Pre->cr = (sony_cxd_dvbt2_l1post_cr_t) (data[8] & 0x03);
        pL1Pre->fec = (sony_cxd_dvbt2_l1post_fec_type_t) (data[9] & 0x03);
        pL1Pre->l1PostSize = (data[10] & 0x03) << 16;
        pL1Pre->l1PostSize |= (data[11]) << 8;
        pL1Pre->l1PostSize |= (data[12]);
        pL1Pre->l1PostInfoSize = (data[13] & 0x03) << 16;
        pL1Pre->l1PostInfoSize |= (data[14]) << 8;
        pL1Pre->l1PostInfoSize |= (data[15]);
        pL1Pre->pp = (sony_cxd_dvbt2_pp_t) (data[16] & 0x0F);
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
        pL1Pre->t2Version = (sony_cxd_dvbt2_version_t) version;
        pL1Pre->l1PostScrambled = (data[32] & 0x20) >> 5;
        pL1Pre->t2BaseLite = (data[32] & 0x10) >> 4;
        pL1Pre->crc32 = (data[33] << 24);
        pL1Pre->crc32 |= (data[34] << 16);
        pL1Pre->crc32 |= (data[35] << 8);
        pL1Pre->crc32 |= data[36];

        if (profile == sony_cxd_DVBT2_PROFILE_BASE) {
            /* Get the FFT mode from the S2 signalling. */
            switch ((pL1Pre->s2 >> 1)) {
            case sony_cxd_DVBT2_BASE_S2_M1K_G_ANY:
                pL1Pre->fftMode = sony_cxd_DVBT2_M1K;
                break;
            case sony_cxd_DVBT2_BASE_S2_M2K_G_ANY:
                pL1Pre->fftMode = sony_cxd_DVBT2_M2K;
                break;
            case sony_cxd_DVBT2_BASE_S2_M4K_G_ANY:
                pL1Pre->fftMode = sony_cxd_DVBT2_M4K;
                break;
            /* Intentional fall through */
            case sony_cxd_DVBT2_BASE_S2_M8K_G_DVBT:
            case sony_cxd_DVBT2_BASE_S2_M8K_G_DVBT2:
                pL1Pre->fftMode = sony_cxd_DVBT2_M8K;
                break;
            case sony_cxd_DVBT2_BASE_S2_M16K_G_ANY:
                pL1Pre->fftMode = sony_cxd_DVBT2_M16K;
                break;
            /* Intentional fall through */
            case sony_cxd_DVBT2_BASE_S2_M32K_G_DVBT:
            case sony_cxd_DVBT2_BASE_S2_M32K_G_DVBT2:
                pL1Pre->fftMode = sony_cxd_DVBT2_M32K;
                break;
            default:
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
            }
        }
        else if (profile == sony_cxd_DVBT2_PROFILE_LITE) {
            /* Get the FFT mode from the S2 signalling. */
            switch ((pL1Pre->s2 >> 1)) {
            case sony_cxd_DVBT2_LITE_S2_M2K_G_ANY:
                pL1Pre->fftMode = sony_cxd_DVBT2_M2K;
                break;
            case sony_cxd_DVBT2_LITE_S2_M4K_G_ANY:
                pL1Pre->fftMode = sony_cxd_DVBT2_M4K;
                break;
            /* Intentional fall through */
            case sony_cxd_DVBT2_LITE_S2_M8K_G_DVBT:
            case sony_cxd_DVBT2_LITE_S2_M8K_G_DVBT2:
                pL1Pre->fftMode = sony_cxd_DVBT2_M8K;
                break;
            /* Intentional fall through */
            case sony_cxd_DVBT2_LITE_S2_M16K_G_DVBT:
            case sony_cxd_DVBT2_LITE_S2_M16K_G_DVBT2:
                pL1Pre->fftMode = sony_cxd_DVBT2_M16K;
                break;
            default:
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
            }
        }
        else {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Get the Mixed indicator (FEFs exist) from the S2 signalling. */
        pL1Pre->mixed = pL1Pre->s2 & 0x01;
    }

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_Version (sony_cxd_demod_t * pDemod, sony_cxd_dvbt2_version_t * pVersion)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint8_t version = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_Version");

    if ((!pDemod) || (!pVersion)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[2];
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t earlyLock = 0;

        /* Freeze the register. */
		if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &syncState, &tsLock, &earlyLock);
        if (result != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Only valid after L1-pre locked. */
        if (syncState < 5) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-T Bank : 0x22 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3F, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

		SLVT_CXD_UnFreezeReg(pDemod);

        version = ((data[0] & 0x03) << 2);
        version |= ((data[1] & 0xC0) >> 6);
        *pVersion = (sony_cxd_dvbt2_version_t) version;
    }
    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_OFDM (sony_cxd_demod_t * pDemod, sony_cxd_dvbt2_ofdm_t * pOfdm)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_OFDM");

    if ((!pDemod) || (!pOfdm)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[5];
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t earlyLock = 0;
        sony_cxd_result_t result = sony_cxd_RESULT_OK;

        /* Freeze the registers. */
		if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &syncState, &tsLock, &earlyLock);
        if (result != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Only valid after demod locked. */
        if (syncState != 6) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-T Bank : 0x20 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
        /* slave    Bank    Addr    Bit              Name                mean
         * ---------------------------------------------------------------------------------
         * <SLV-T>   20h     5Ch     [5]          IREG_OFDM_MIXED         0: Not mixed, 1: Mixed
         * <SLV-T>   20h     5Ch     [4]          IREG_OFDM_MISO          0: SISO, 1: MISO
         * <SLV-T>   20h     5Ch     [2:0]        IREG_OFDM_FFTSIZE[2:0]  0: 2K, 1: 8K, 2: 4K, 3: 1K, 4: 16K, 5: 32K
         * <SLV-T>   20h     5Dh     [6:4]        IREG_OFDM_GI[2:0]       0: 1/32, 1: 1/16, 2: 1/8, 3: 1/4, 4: 1/128, 5: 19/128, 6: 19/256
         * <SLV-T>   20h     5Dh     [2:0]        IREG_OFDM_PP[2:0]       0: PP1, 1: PP2, 2: PP3, 3: PP4, 4: PP5, 5: PP6, 6: PP7, 7: PP8
         * <SLV-T>   20h     5Eh     [4]          IREG_OFDM_BWT_EXT       0: Normal, 1: Extended
         * <SLV-T>   20h     5Eh     [3:0]        IREG_OFDM_PAPR[3:0]     0: None, 1: ACE, 2: TR, 3: ACE_TR, 4-15: reserved
         * <SLV-T>   20h     5Fh     [3:0]        IREG_OFDM_NDSYM[11:8]
         * <SLV-T>   20h     60h     [7:0]        IREG_OFDM_NDSYM[7:0]    Number of data symbols in a T2 frame
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5C, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Unfreeze the registers. */
		SLVT_CXD_UnFreezeReg(pDemod);

        /* Convert */
        pOfdm->mixed = ((data[0] & 0x20) ? 1 : 0);
        pOfdm->isMiso = ((data[0] & 0x10) >> 4);
        pOfdm->mode = (sony_cxd_dvbt2_mode_t) (data[0] & 0x07);
        pOfdm->gi = (sony_cxd_dvbt2_guard_t) ((data[1] & 0x70) >> 4);
        pOfdm->pp = (sony_cxd_dvbt2_pp_t) (data[1] & 0x07);
        pOfdm->bwExt = (data[2] & 0x10) >> 4;
        pOfdm->papr = (sony_cxd_dvbt2_papr_t) (data[2] & 0x0F);
        pOfdm->numSymbols = (data[3] << 8) | data[4];
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_DataPLPs (sony_cxd_demod_t * pDemod, uint8_t * pPLPIds, uint8_t * pNumPLPs)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_DataPLPs");

    if ((!pDemod) || (!pNumPLPs)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t l1PostOK = 0;

        /* Set SLV-T Bank : 0x22 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Freeze the registers. */
		if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
        /* slave    Bank    Addr    Bit              Name                mean
         * ---------------------------------------------------------------------------------
         * <SLV-T>   22h     12h     [0]            IL1POST_OK           0:invalid1:valid
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x12, &l1PostOK, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (!(l1PostOK & 0x01)) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Read the number of PLPs. */
        /* slave    Bank    Addr    Bit              Name                 mean
         * ---------------------------------------------------------------------------------
         * <SLV-T>   22h     7Fh     [7:0]      INUM_DATA_PLP_TS[7:0]    number of TS PLP (max 255)
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7F, pNumPLPs, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Invalid number of PLPs detected. May occur if multiple threads accessing device. */
        if (*pNumPLPs == 0) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_OTHER);
        }

        if (!pPLPIds) {
            /* PLP ID list is unnecessary. */
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
        }

        /* Read the PLPs from the device. */
        /* Read up to PLP_ID128 */
        /*  slave    Bank    Addr    Bit              Name                 mean
         * --------------------------------------------------------------------------
         * <SLV-T>   22h     80h     [7:0]      IL1POST_PLP_ID0[7:0]     TS PLP_ID #1
         * <SLV-T>   22h     FFh     [7:0]      IL1POST_PLP_ID127[7:0]   TS PLP_ID #128
         * ...
         * <SLV-T>   23h     10h     [7:0]      IL1POST_PLP_ID128[7:0]   TS PLP_ID #129
         * <SLV-T>   23h     8Eh     [7:0]      IL1POST_PLP_ID254[7:0]   TS PLP_ID #255
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, pPLPIds, *pNumPLPs > 128 ? 128 : *pNumPLPs) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (*pNumPLPs > 128) {
            /* Set SLV-T Bank : 0x23 */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x23) != sony_cxd_RESULT_OK) {
				SLVT_CXD_UnFreezeReg(pDemod);
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }

            /* Read the remaining data PLPs. */
            if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, pPLPIds + 128, *pNumPLPs - 128) != sony_cxd_RESULT_OK) {
				SLVT_CXD_UnFreezeReg(pDemod);
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }

		SLVT_CXD_UnFreezeReg(pDemod);
    }
    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_ActivePLP (sony_cxd_demod_t * pDemod,
                                                  sony_cxd_dvbt2_plp_btype_t type,
                                                  sony_cxd_dvbt2_plp_t * pPLPInfo)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_ActivePLP");

    if ((!pDemod) || (!pPLPInfo)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[20];
        uint8_t addr = 0;
        uint8_t index = 0;
        uint8_t l1PostOk = 0;

        /* Freeze the registers. */
		if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Set SLV-T Bank : 0x22 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Check L1 Post is valid
         * slave    Bank    Addr    Bit    Name
         * ------------------------------------------
         * <SLV-T>  22h     12h     [0]    IL1POST_OK
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x12, &l1PostOk, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (!l1PostOk) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        if (type == sony_cxd_DVBT2_PLP_COMMON) {
            /* Read contents of the L1-post for appropriate item. */
            /* slave    Bank    Addr    Bit           Name
             * -----------------------------------------------------------
             * <SLV-T>   22h     67h     [7:0]        IL1POST_PLP_ID_C[7:0]
             * <SLV-T>   22h     68h     [2:0]        IL1POST_PLP_TYPE_C[2:0]
             * <SLV-T>   22h     69h     [4:0]        IL1POST_PLP_PAYLOAD_TYPE_C[4:0]
             * <SLV-T>   22h     6Ah     [0]          IL1POST_FF_FLAG_C
             * <SLV-T>   22h     6Bh     [2:0]        IL1POST_FIRST_RF_IDX_C[2:0]
             * <SLV-T>   22h     6Ch     [7:0]        IL1POST_FIRST_FRAME_IDX_C[7:0]
             * <SLV-T>   22h     6Dh     [7:0]        IL1POST_PLP_GROUP_ID_C[7:0]]
             * <SLV-T>   22h     6Eh     [2:0]        IL1POST_PLP_COD_C[2:0]
             * <SLV-T>   22h     6Fh     [2:0]        IL1POST_PLP_MOD_C[2:0]
             * <SLV-T>   22h     70h     [0]          IL1POST_PLP_ROTATION_C
             * <SLV-T>   22h     71h     [1:0]        IL1POST_PLP_FEC_TYPE_C[1:0]
             * <SLV-T>   22h     72h     [1:0]        IL1POST_PLP_NUM_BLOCKS_MAX_C[9:8]
             * <SLV-T>   22h     73h     [7:0]        IL1POST_PLP_NUM_BLOCKS_MAX_C[7:0]
             * <SLV-T>   22h     74h     [7:0]        IL1POST_FRAME_INTERVAL_C[7:0]
             * <SLV-T>   22h     75h     [7:0]        IL1POST_TIME_IL_LENGTH_C[7:0]
             * <SLV-T>   22h     76h     [0]          IL1POST_TIME_IL_TYPE_C
             * <SLV-T>   22h     78h     [0]          IL1POST_IN_BAND_FLAG_C
             * <SLV-T>   22h     79h     [7:0]        IL1POST_RESERVED_1_C[15:8]
             * <SLV-T>   22h     7Ah     [7:0]        IL1POST_RESERVED_1_C[7:0]
             */
            addr = 0x67;
        }
        else {
            /* slave    Bank    Addr    Bit           Name
             * --------------------------------------------
             * <SLV-T>   22h     54h     [7:0]        IL1POST_PLP_ID[7:0]
             * <SLV-T>   22h     55h     [2:0]        IL1POST_PLP_TYPE[2:0]
             * <SLV-T>   22h     56h     [4:0]        IL1POST_PLP_PAYLOAD_TYPE[4:0]
             * <SLV-T>   22h     57h     [0]          IL1POST_FF_FLAG
             * <SLV-T>   22h     58h     [2:0]        IL1POST_FIRST_RF_IDX[2:0]
             * <SLV-T>   22h     59h     [7:0]        IL1POST_FIRST_FRAME_IDX[7:0]
             * <SLV-T>   22h     5Ah     [7:0]        IL1POST_PLP_GROUP_ID[7:0]
             * <SLV-T>   22h     5Bh     [2:0]        IL1POST_PLP_COD[2:0]
             * <SLV-T>   22h     5Ch     [2:0]        IL1POST_PLP_MOD[2:0]
             * <SLV-T>   22h     5Dh     [0]          IL1POST_PLP_ROTATION
             * <SLV-T>   22h     5Eh     [1:0]        IL1POST_PLP_FEC_TYPE[1:0]
             * <SLV-T>   22h     5Fh     [1:0]        IL1POST_PLP_NUM_BLOCKS_MAX[9:8]
             * <SLV-T>   22h     60h     [7:0]        IL1POST_PLP_NUM_BLOCKS_MAX[7:0]
             * <SLV-T>   22h     61h     [7:0]        IL1POST_FRAME_INTERVAL[7:0]
             * <SLV-T>   22h     62h     [7:0]        IL1POST_TIME_IL_LENGTH[7:0]
             * <SLV-T>   22h     63h     [0]          IL1POST_TIME_IL_TYPE
             * <SLV-T>   22h     64h     [0]          IL1POST_IN_BAND_FLAG
             * <SLV-T>   22h     65h     [7:0]        IL1POST_RESERVED_1[15:8]
             * <SLV-T>   22h     66h     [7:0]        IL1POST_RESERVED_1[7:0]
             */
            addr = 0x54;
        }

        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, addr, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

		SLVT_CXD_UnFreezeReg(pDemod);

        /* If common, check for no common PLP, frame_interval == 0. */
        if ((type == sony_cxd_DVBT2_PLP_COMMON) && (data[13] == 0)) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        pPLPInfo->id = data[index++];
        pPLPInfo->type = (sony_cxd_dvbt2_plp_type_t) (data[index++] & 0x07);
        pPLPInfo->payload = (sony_cxd_dvbt2_plp_payload_t) (data[index++] & 0x1F);
        pPLPInfo->ff = data[index++] & 0x01;
        pPLPInfo->firstRfIdx = data[index++] & 0x07;
        pPLPInfo->firstFrmIdx = data[index++];
        pPLPInfo->groupId = data[index++];
        pPLPInfo->plpCr = (sony_cxd_dvbt2_plp_code_rate_t) (data[index++] & 0x07);
        pPLPInfo->constell = (sony_cxd_dvbt2_plp_constell_t) (data[index++] & 0x07);
        pPLPInfo->rot = data[index++] & 0x01;
        pPLPInfo->fec = (sony_cxd_dvbt2_plp_fec_t) (data[index++] & 0x03);
        pPLPInfo->numBlocksMax = (uint16_t) ((data[index++] & 0x03)) << 8;
        pPLPInfo->numBlocksMax |= data[index++];
        pPLPInfo->frmInt = data[index++];
        pPLPInfo->tilLen = data[index++];
        pPLPInfo->tilType = data[index++] & 0x01;

        /* Skip 0x77 for common PLP. */
        if (type == sony_cxd_DVBT2_PLP_COMMON) {
            index++;
        }

        pPLPInfo->inBandAFlag = data[index++] & 0x01;
        pPLPInfo->rsvd = data[index++] << 8;
        pPLPInfo->rsvd |= data[index++];

        /* New fields from V1.2.1 */
        pPLPInfo->inBandBFlag = (uint8_t) ((pPLPInfo->rsvd & 0x8000) >> 15);
        pPLPInfo->plpMode = (sony_cxd_dvbt2_plp_mode_t) ((pPLPInfo->rsvd & 0x000C) >> 2);
        pPLPInfo->staticFlag = (uint8_t) ((pPLPInfo->rsvd & 0x0002) >> 1);
        pPLPInfo->staticPaddingFlag = (uint8_t) (pPLPInfo->rsvd & 0x0001);
        pPLPInfo->rsvd = (uint16_t) ((pPLPInfo->rsvd & 0x7FF0) >> 4); /* Remaining 11 bits */
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_DataPLPError (sony_cxd_demod_t * pDemod, uint8_t * pPLPError)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_DataPLPError");

    if ((!pDemod) || (!pPLPError)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[2];

        /* Set SLV-T Bank : 0x22 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit           Name                meaning
         *  --------------------------------------------------------------------------------
         *  <SLV-T>   22h     12h     [0]       IL1POST_OK         0:invalid 1:valid
         *  <SLV-T>   22h     13h     [0]       IPLP_SEL_ERR       0:found 1:not found
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x12, &data[0], 2) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if ((data[0] & 0x01) == 0x00) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        *pPLPError = data[1] & 0x01;
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_L1Change (sony_cxd_demod_t * pDemod, uint8_t * pL1Change)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_L1Change");

    if ((!pDemod) || (!pL1Change)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data;
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t earlyLock = 0;
        sony_cxd_result_t result = sony_cxd_RESULT_OK;

        /* Freeze the registers. */
		if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &syncState, &tsLock, &earlyLock);
        if (result != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Only valid after L1-post decoded at least once. */
        if (syncState < 5) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-T Bank : 0x22 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name               mean
         * --------------------------------------------------------------------------------
         * <SLV-T>   22h     15h     [0]       IREG_L1_CHANGE_FLAG   0:Not changed, 1:Changed
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x15, &data, sizeof (data)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        *pL1Change = data & 0x01;
        if (*pL1Change) {
            /* Only clear if set, otherwise may miss indicator. */
            /* slave    Bank    Addr    Bit    default   Value          Name
             * ---------------------------------------------------------------------------------
             * <SLV-T>   22h     16h     [0]      1'b0       1'b1       OREGD_L1_CHANGE_FLAG_CLR
             */
            if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x16, 0x01) != sony_cxd_RESULT_OK) {
				SLVT_CXD_UnFreezeReg(pDemod);
                sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
            }
        }
		SLVT_CXD_UnFreezeReg(pDemod);
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_L1Post (sony_cxd_demod_t * pDemod, sony_cxd_dvbt2_l1post_t * pL1Post)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_L1Post");

    if ((!pDemod) || (!pL1Post)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[15];
        uint8_t l1PostOK = 0;

        /* Freeze the register. */
		if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Set SLV-T Bank : 0x22 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name                mean
         * ---------------------------------------------------------------------------------
         * <SLV-T>   22h     12h     [0]            IL1POST_OK           0:invalid1:valid
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x12, &l1PostOK, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (!(l1PostOK & 0x01)) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Read L1-post data. */
        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------------------
         * <SLV-T>   22h     45h     [6:0]        IL1POST_SUB_SLICES_PER_FRAME[14:8]
         * <SLV-T>   22h     46h     [7:0]        IL1POST_SUB_SLICES_PER_FRAME[7:0]
         * <SLV-T>   22h     47h     [7:0]        IL1POST_NUM_PLP[7:0]
         * <SLV-T>   22h     48h     [3:0]        IL1POST_NUM_AUX[3:0]
         * <SLV-T>   22h     49h     [7:0]        IL1POST_AUX_CONFIG_RFU[7:0]
         * <SLV-T>   22h     4Ah     [2:0]        IL1POST_RF_IDX[2:0]
         * <SLV-T>   22h     4Bh     [7:0]        IL1POST_FREQUENCY[31:24]
         * <SLV-T>   22h     4Ch     [7:0]        IL1POST_FREQUENCY[23:16]
         * <SLV-T>   22h     4Dh     [7:0]        IL1POST_FREQUENCY[15:8]
         * <SLV-T>   22h     4Eh     [7:0]        IL1POST_FREQUENCY[7:0]
         * <SLV-T>   22h     4Fh     [3:0]        IL1POST_FEF_TYPE[3:0]
         * <SLV-T>   22h     50h     [7:0]        IL1POST_FEF_LENGTH[23:16]
         * <SLV-T>   22h     51h     [7:0]        IL1POST_FEF_LENGTH[15:8]
         * <SLV-T>   22h     52h     [7:0]        IL1POST_FEF_LENGTH[7:0]
         * <SLV-T>   22h     53h     [7:0]        IL1POST_FEF_INTERVAL[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x45, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Unfreeze registers. */
		SLVT_CXD_UnFreezeReg(pDemod);

        /* Convert data. */
        pL1Post->subSlicesPerFrame = (data[0] & 0x7F) << 8;
        pL1Post->subSlicesPerFrame |= data[1];
        pL1Post->numPLPs = data[2];
        pL1Post->numAux = data[3] & 0x0F;
        pL1Post->auxConfigRFU = data[4];
        pL1Post->rfIdx = data[5] & 0x07;
        pL1Post->freq = data[6] << 24;
        pL1Post->freq |= data[7] << 16;
        pL1Post->freq |= data[8] << 8;
        pL1Post->freq |= data[9];
        pL1Post->fefType = data[10] & 0x0F;
        pL1Post->fefLength = data[11] << 16;
        pL1Post->fefLength |= data[12] << 8;
        pL1Post->fefLength |= data[13];
        pL1Post->fefInterval = data[14];
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_BBHeader (sony_cxd_demod_t * pDemod,
                                                 sony_cxd_dvbt2_plp_btype_t type,
                                                 sony_cxd_dvbt2_bbheader_t * pBBHeader)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_BBHeader");

    if ((!pDemod) || (!pBBHeader)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the registers. */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        sony_cxd_result_t result = sony_cxd_RESULT_OK;
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t earlyLock = 0;

        /* Confirm SyncStat. */
        result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &syncState, &tsLock, &earlyLock);
        if (result != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (result);
        }

        if (!tsLock) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }
    }

    /* Set SLV-T Bank : 0x24 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x24) != sony_cxd_RESULT_OK) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        uint8_t data[17];
        uint8_t addr = 0;

        if (type == sony_cxd_DVBT2_PLP_COMMON) {
            /* BB Header (Common PLP)
             *  slave    Bank    Addr    Bit              Name                       Meaning
             * -------------------------------------------------------------------------------------------
             *  <SLV-T>   24h     A8h     [0]          IREG_SP_BBH_LOCK_COMMON      Common PLP BB Header Valid (1'b1:Valid)
             *  <SLV-T>   24h     ACh     [0]          IREG_SP_PLP_MODE_COMMON      Common PLP Mode (1'b0:NM,1'b1:HEM))
             *  <SLV-T>   24h     A9h     [15:8]       IREG_SP_MATYPE_COMMON[15:8]  Common PLP MATYPE
             *  <SLV-T>   24h     AAh     [7:0]        IREG_SP_MATYPE_COMMON[7:0]   Common PLP MATYPE
             *  <SLV-T>   24h     AFh     [15:8]       IREG_SP_UPL_COMMON[15:8]     Common PLP UPL  (Invalid at IREG_SP_PLP_MODE_COMMON=1'b1)
             *  <SLV-T>   24h     B0h     [7:0]        IREG_SP_UPL_COMMON[7:0]      Common PLP UPL  (Invalid at IREG_SP_PLP_MODE_COMMON=1'b1)
             *  <SLV-T>   24h     ADh     [15:8]       IREG_SP_DFL_COMMON[15:8]     Common PLP DFL
             *  <SLV-T>   24h     AEh     [7:0]        IREG_SP_DFL_COMMON[7:0]      Common PLP DFL
             *  <SLV-T>   24h     B1h     [7:0]        IREG_SP_SYNC_COMMON[7:0]     Common PLP SYNC (Invalid at IREG_SP_PLP_MODE_COMMON=1'b1)
             *  <SLV-T>   24h     B2h     [15:8]       IREG_SP_SYNCD_COMMON[15:8]   Common PLP SYNCD
             *  <SLV-T>   24h     B3h     [7:0]        IREG_SP_SYNCD_COMMON[7:0]    Common PLP SYNCD
             *  <SLV-T>   24h     B6h     [23:16]      IREG_SP_HISSY_COMMON[23:16]  Common PLP ISSY (Invalid at IREG_SP_PLP_MODE_COMMON=1'b0)
             *  <SLV-T>   24h     B7h     [15:8]       IREG_SP_HISSY_COMMON[15:8]   Common PLP ISSY (Invalid at IREG_SP_PLP_MODE_COMMON=1'b0)
             *  <SLV-T>   24h     B8h     [7:0]        IREG_SP_HISSY_COMMON[7:0]    Common PLP ISSY (Invalid at IREG_SP_PLP_MODE_COMMON=1'b0)
             */
            addr = 0xA8;
        } else {
            /* BB Header (Data PLP)
             *  slave    Bank    Addr    Bit              Name                       Meaning
             * -------------------------------------------------------------------------------------------
             *  <SLV-T>   24h     92h     [0]          IREG_SP_BBH_LOCK_DATA        Data PLP BB Header Valid (1'b1:Valid)
             *  <SLV-T>   24h     96h     [0]          IREG_SP_PLP_MODE_DATA        Data PLP Mode (1'b0:NM,1'b1:HEM))
             *  <SLV-T>   24h     93h     [15:8]       IREG_SP_MATYPE_DATA[15:8]    Data PLP MATYPE
             *  <SLV-T>   24h     94h     [7:0]        IREG_SP_MATYPE_DATA[7:0]     Data PLP MATYPE
             *  <SLV-T>   24h     99h     [15:8]       IREG_SP_UPL_DATA[15:8]       Data PLP UPL  (Invalid at IREG_SP_PLP_MODE_DATA=1'b1)
             *  <SLV-T>   24h     9Ah     [7:0]        IREG_SP_UPL_DATA[7:0]        Data PLP UPL  (Invalid at IREG_SP_PLP_MODE_DATA=1'b1)
             *  <SLV-T>   24h     97h     [15:8]       IREG_SP_DFL_DATA[15:8]       Data PLP DFL
             *  <SLV-T>   24h     98h     [7:0]        IREG_SP_DFL_DATA[7:0]        Data PLP DFL
             *  <SLV-T>   24h     9Bh     [7:0]        IREG_SP_SYNC_DATA[7:0]       Data PLP SYNC (Invalid at IREG_SP_PLP_MODE_DATA=1'b1)
             *  <SLV-T>   24h     9Ch     [15:8]       IREG_SP_SYNCD_DATA[15:8]     Data PLP SYNCD
             *  <SLV-T>   24h     9Dh     [7:0]        IREG_SP_SYNCD_DATA[7:0]      Data PLP SYNCD
             *  <SLV-T>   24h     A0h     [23:16]      IREG_SP_HISSY_DATA[23:16]    Data PLP ISSY (Invalid at IREG_SP_PLP_MODE_DATA=1'b0)
             *  <SLV-T>   24h     A1h     [15:8]       IREG_SP_HISSY_DATA[15:8]     Data PLP ISSY (Invalid at IREG_SP_PLP_MODE_DATA=1'b0)
             *  <SLV-T>   24h     A2h     [7:0]        IREG_SP_HISSY_DATA[7:0]      Data PLP ISSY (Invalid at IREG_SP_PLP_MODE_DATA=1'b0)
             */
            addr = 0x92;
        }

        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, addr, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

		SLVT_CXD_UnFreezeReg(pDemod);

        if ((data[0] & 0x01) == 0x00) {
            /* BBHEADER data is not valid */
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        pBBHeader->streamInput = (sony_cxd_dvbt2_stream_t)((data[1] >> 6) & 0x03);
        pBBHeader->isSingleInputStream = (uint8_t)((data[1] >> 5) & 0x01);
        pBBHeader->isConstantCodingModulation = (uint8_t)((data[1] >> 4) & 0x01);
        pBBHeader->issyIndicator = (uint8_t)((data[1] >> 3) & 0x01);
        pBBHeader->nullPacketDeletion = (uint8_t)((data[1] >> 2) & 0x01);
        pBBHeader->ext = (uint8_t)(data[1] & 0x03);

        pBBHeader->inputStreamIdentifier = data[2];
        pBBHeader->plpMode = (data[4] & 0x01) ? sony_cxd_DVBT2_PLP_MODE_HEM : sony_cxd_DVBT2_PLP_MODE_NM;
        pBBHeader->dataFieldLength = (uint16_t)((data[5] << 8) | data[6]);

        if(pBBHeader->plpMode == sony_cxd_DVBT2_PLP_MODE_NM){
            /* NM (Normal Mode) */
            pBBHeader->userPacketLength = (uint16_t)((data[7] << 8) | data[8]);
            pBBHeader->syncByte = data[9];
            pBBHeader->issy = 0; /* Invalid */
        }else{
            /* HEM (High Efficiency Mode) */
            pBBHeader->userPacketLength = 0; /* Invalid */
            pBBHeader->syncByte = 0; /* Invalid */
            pBBHeader->issy = (uint32_t)((data[14] << 16) | (data[15] << 8) | data[16]);
        }
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_InBandBTSRate (sony_cxd_demod_t * pDemod,
                                                      sony_cxd_dvbt2_plp_btype_t type,
                                                      uint32_t * pTSRateBps)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_InBandBTSRate");

    if ((!pDemod) || (!pTSRateBps)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the registers. */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        sony_cxd_result_t result = sony_cxd_RESULT_OK;
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t earlyLock = 0;

        /* Confirm SyncStat. */
        result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &syncState, &tsLock, &earlyLock);
        if (result != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (result);
        }

        if (!tsLock) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }
    }

    /* Set SLV-T Bank : 0x22 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        uint8_t l1PostOk = 0;
        uint8_t addr = 0;
        uint8_t data = 0;

        /* Check L1 Post is valid
         * slave    Bank    Addr    Bit    Name
         * ------------------------------------------
         * <SLV-T>  22h     12h     [0]    IL1POST_OK
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x12, &l1PostOk, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (!(l1PostOk & 0x01)) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* slave    Bank    Addr    Bit       Name
         * ----------------------------------------------------------------
         * <SLV-T>   22h     65h     [7:0]        IL1POST_RESERVED_1[15:8]
         * <SLV-T>   22h     79h     [7:0]        IL1POST_RESERVED_1_C[15:8]
         *
         * IL1POST_RESERVED_1,_C[15] : IN_BAND_B_FLAG
         */
        if (type == sony_cxd_DVBT2_PLP_COMMON) {
            addr = 0x79;
        } else {
            addr = 0x65;
        }

        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, addr, &data, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if ((data & 0x80) == 0x00) {
            /* No in-band B sigalling (or no common PLP) */
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }
    }

    /* Set SLV-T Bank : 0x25 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x25) != sony_cxd_RESULT_OK) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        uint8_t data[4];
        uint8_t addr = 0;

        /* in-band B signalling TS_RATE (from v1.2.1 onward)
         *  slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------------------
         *  <SLV-T>   25h     A6h     [2:0]        IREG_SP_IB_TS_RATE_COMMON[26:24]
         *  <SLV-T>   25h     A7h     [7:0]        IREG_SP_IB_TS_RATE_COMMON[23:16]
         *  <SLV-T>   25h     A8h     [7:0]        IREG_SP_IB_TS_RATE_COMMON[15:8]
         *  <SLV-T>   25h     A9h     [7:0]        IREG_SP_IB_TS_RATE_COMMON[7:0]
         *  <SLV-T>   25h     AAh     [2:0]        IREG_SP_IB_TS_RATE_DATA[26:24]
         *  <SLV-T>   25h     ABh     [7:0]        IREG_SP_IB_TS_RATE_DATA[23:16]
         *  <SLV-T>   25h     ACh     [7:0]        IREG_SP_IB_TS_RATE_DATA[15:8]
         *  <SLV-T>   25h     ADh     [7:0]        IREG_SP_IB_TS_RATE_DATA[7:0]
         */
        if (type == sony_cxd_DVBT2_PLP_COMMON) {
            addr = 0xA6;
        } else {
            addr = 0xAA;
        }

        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, addr, &data[0], 4) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        *pTSRateBps = (uint32_t)(((data[0] & 0x07) << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_SpectrumSense (sony_cxd_demod_t * pDemod, sony_cxd_demod_terr_cable_spectrum_sense_t * pSense)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint8_t syncState   = 0;
    uint8_t tsLock      = 0;
    uint8_t earlyUnlock = 0;
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_SpectrumSense");

    if ((!pDemod) || (!pSense)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the register. */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &syncState, &tsLock, &earlyUnlock);
    if (result != sony_cxd_RESULT_OK) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (result);
    }

    /* Only valid after lock. */
    if (syncState != 6) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
    }

    {
        uint8_t data = 0;

        /* Set SLV-T Bank : 0x28 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x28) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name                mean
         * ---------------------------------------------------------------------------------
         * <SLV-T>   28h     E6h     [0]        IREG_CRCG2_SINVP1      0:not invert,   1:invert
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE6, &data, sizeof (data)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

		SLVT_CXD_UnFreezeReg(pDemod);

        if (pDemod->confSense == sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_INV) {
            *pSense = (data & 0x01) ? sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_NORMAL : sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_INV;
        } else {
            *pSense = (data & 0x01) ? sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_INV : sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_NORMAL;
        }
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_SNR (sony_cxd_demod_t * pDemod, int32_t * pSNR)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_SNR");

    if ((!pDemod) || (!pSNR)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint16_t reg;
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t earlyLock = 0;
        uint8_t data[2];

        /* Freeze registers */
		if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &syncState, &tsLock, &earlyLock);
        if (result != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Confirm sequence state == 6. */
        if (syncState != 6) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-T Bank : 0x20 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   20h     28h     [7:0]        IREG_SNMON_OD[15:8]
         * <SLV-T>   20h     29h     [7:0]        IREG_SNMON_OD[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x28, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
		SLVT_CXD_UnFreezeReg(pDemod);

        reg = ((data[0] & 0xFF) << 8) | data[1];

        /* Confirm valid range, clip as necessary */
        if (reg == 0) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* IREG_SNMON_OD is clipped at a maximum of 10876.
         *
         * SNR = 10 * log10 (IREG_SNMON_OD / (12600 - IREG_SNMON_OD)) + 32.0
         *     = 10 * (log10(IREG_SNMON_OD) - log10(12600 - IREG_SNMON_OD)) + 32.0
         * sony_cxd_log10 returns log10(x) * 100
         * Therefore SNR(dB) * 1000 :
         *     = 10 * 10 * (sony_cxd_log10(IREG_SNMON_OD) - sony_cxd_log10(12600 - IREG_SNMON_OD) + 32000
         */
        if (reg > 10876) {
            reg = 10876;
        }

        *pSNR = 10 * 10 * ((int32_t) sony_cxd_math_log10 (reg) - (int32_t) sony_cxd_math_log10 (12600 - reg));
        *pSNR += 32000;
    }
    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_PreLDPCBER (sony_cxd_demod_t * pDemod, uint32_t * pBER)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint32_t bitError = 0;
    uint32_t periodExp = 0;
    uint32_t n_ldpc = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_PreLDPCBER");

    if ((!pDemod) || (!pBER)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x20 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   20h     39h     [4]          ILBER_VALID
         * <SLV-T>   20h     39h     [3:0]        IBER1_BITERR[27:24]
         * <SLV-T>   20h     3Ah     [7:0]        IBER1_BITERR[23:16]
         * <SLV-T>   20h     3Bh     [7:0]        IBER1_BITERR[15:8]
         * <SLV-T>   20h     3Ch     [7:0]        IBER1_BITERR[7:0]
         */
        uint8_t data[4];
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x39, data, sizeof (data)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (!(data[0] & 0x10)) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        bitError = ((data[0] & 0x0F) << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

        /* Read measurement period.
         * slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   20h     6fh     [3:0]        OREG_LBER_MES[3:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6F, data, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        periodExp = data[0] & 0x0F;

        /* Set SLV-T Bank : 0x22 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Get the PLP type (Normal/Short).
         * slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   22h     5Eh     [1:0]        IL1POST_PLP_FEC_TYPE[1:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5E, data, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

		SLVT_CXD_UnFreezeReg(pDemod);

        if (((sony_cxd_dvbt2_plp_fec_t) (data[0] & 0x03)) == sony_cxd_DVBT2_FEC_LDPC_16K) {
            /* Short. */
            n_ldpc = 16200;
        }
        else {
            /* Normal. */
            n_ldpc = 64800;
        }
    }

    if (bitError > ((1U << periodExp) * n_ldpc)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
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

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_PostBCHFER (sony_cxd_demod_t * pDemod, uint32_t * pFER)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint32_t fecError = 0;
    uint32_t period = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_PostBCHFER");

    if ((!pDemod) || (!pFER)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x20 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        uint8_t data[3];
        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   20h     75h     [0]           IBBER_VALID
         * <SLV-T>   20h     76h     [6:0]         IBER2_FBERR[14:8]
         * <SLV-T>   20h     77h     [7:0]         IBER2_FBERR[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x75, data, 3) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (!(data[0] & 0x01)) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        fecError = ((data[1] & 0x7F) << 8) | (data[2]);

        /* Read measurement period.
         * slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   20h     72h     [3:0]        OREG_BBER_MES[3:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x72, data, 1) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Period = 2^BBER_MES */
        period = (1 << (data[0] & 0x0F));
    }

    if ((period == 0) || (fecError > period)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
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

    sony_cxd_TRACE_RETURN (result);
}


sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_PreBCHBER (sony_cxd_demod_t * pDemod, uint32_t * pBER)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint32_t bitError = 0;
    uint32_t periodExp = 0;
    uint32_t n_bch = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_PreBCHBER");

    if ((!pDemod) || (!pBER)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Monitor the BER parameters. */
    {
        uint8_t data[4];
        sony_cxd_dvbt2_plp_fec_t plpFecType = sony_cxd_DVBT2_FEC_LDPC_16K;
        sony_cxd_dvbt2_plp_code_rate_t plpCr = sony_cxd_DVBT2_R1_2;

        static const uint16_t nBCHBitsLookup[2][8] = {
          /* R1_2   R3_5   R2_3   R3_4   R4_5   R5_6   R1_3   R2_5 */
            {7200,  9720,  10800, 11880, 12600, 13320, 5400,  6480}, /* 16K FEC */
            {32400, 38880, 43200, 48600, 51840, 54000, 21600, 25920} /* 64k FEC */
        };

        /* Freeze registers */
		if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Set SLV-T Bank : 0x24 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x24) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   24h     40h     [0]          IBBER_VALID
         * <SLV-T>   24h     41h     [5:0]        IBER0_BITERR[21:16]
         * <SLV-T>   24h     42h     [7:0]        IBER0_BITERR[15:8]
         * <SLV-T>   24h     43h     [7:0]        IBER0_BITERR[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x40, data, 4) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (!(data[0] & 0x01)) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE); /* Not Ready */
        }

        bitError = ((data[1] & 0x3F) << 16) | (data[2] << 8) | data[3];

        /* Set SLV-T Bank : 0x22 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Get the PLP FEC type (Normal/Short).
         * slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   22h     5Eh     [1:0]        IL1POST_PLP_FEC_TYPE[1:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5E, data, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        plpFecType = (sony_cxd_dvbt2_plp_fec_t) (data[0] & 0x03);

        /* Get the PLP code rate.
         * slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   22h     5Bh     [2:0]        IL1POST_PLP_COD[2:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5B, data, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        plpCr = (sony_cxd_dvbt2_plp_code_rate_t) (data[0] & 0x07);

		SLVT_CXD_UnFreezeReg(pDemod);

        /* Set SLV-T Bank : 0x20 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Read measurement period.
         * slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   20h     72h     [3:0]        OREG_BBER_MES[3:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x72, data, 1) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Period (4bit) */
        periodExp = data[0] & 0x0F;

        /* Confirm FEC Type / Code Rate */
        if ((plpFecType > sony_cxd_DVBT2_FEC_LDPC_64K) || (plpCr > sony_cxd_DVBT2_R2_5)) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        n_bch = nBCHBitsLookup[plpFecType][plpCr];
    }

    if (bitError > ((1U << periodExp) * n_bch)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
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

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_PacketErrorNumber (sony_cxd_demod_t * pDemod, uint32_t * pPEN)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    uint8_t data[3];

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_PacketErrorNumber");

    if ((!pDemod) || (!pPEN)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x24 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x24) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

   /* slave    Bank    Addr    Bit              Name
    * ------------------------------------------------------------
    * <SLV-T>   24h     FDh     [0]          IREG_SP_BERT_VALID
    * <SLV-T>   24h     FEh     [7:0]        IREG_SP_BERT_CWRJCTCNT[15:8]
    * <SLV-T>   24h     FFh     [7:0]        IREG_SP_BERT_CWRJCTCNT[7:0]
    */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFD, data, sizeof (data)) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (!(data[0] & 0x01) ) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
    }

    *pPEN =  ((data[1] << 0x08) | data[2]);

    sony_cxd_TRACE_RETURN (result);
}


sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_SamplingOffset (sony_cxd_demod_t * pDemod, int32_t * pPPM)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_SamplingOffset");

    if ((!pDemod) || (!pPPM)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t ctlValReg[5];
        uint8_t nominalRateReg[5];
        uint32_t trlCtlVal = 0;
        uint32_t trcgNominalRate = 0;
        int32_t num;
        int32_t den;
        sony_cxd_result_t result = sony_cxd_RESULT_OK;
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t earlyLock = 0;
        int8_t diffUpper = 0;

        /* Freeze registers */
		if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &syncState, &tsLock, &earlyLock);
        if (result != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (syncState != 6) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-T Bank : 0x20 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              name
         * ---------------------------------------------------------------
         * <SLV-T>   20h     52h     [6:0]      IREG_TRL_CTLVAL_S[38:32]
         * <SLV-T>   20h     53h     [7:0]      IREG_TRL_CTLVAL_S[31:24]
         * <SLV-T>   20h     54h     [7:0]      IREG_TRL_CTLVAL_S[23:16]
         * <SLV-T>   20h     55h     [7:0]      IREG_TRL_CTLVAL_S[15:8]
         * <SLV-T>   20h     56h     [7:0]      IREG_TRL_CTLVAL_S[7:0]
         * <SLV-T>   20h     9Fh     [6:0]      OREG_TRCG_NOMINALRATE[38:32]
         * <SLV-T>   20h     A0h     [7:0]      OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>   20h     A1h     [7:0]      OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>   20h     A2h     [7:0]      OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>   20h     A3h     [7:0]      OREG_TRCG_NOMINALRATE[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x52, ctlValReg, sizeof (ctlValReg)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRateReg, sizeof (nominalRateReg)) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Unfreeze registers. */
		SLVT_CXD_UnFreezeReg(pDemod);

        diffUpper = (ctlValReg[0] & 0x7F) - (nominalRateReg[0] & 0x7F);

        /* Confirm offset range */
        if ((diffUpper < -1) || (diffUpper > 1)) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
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

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

static sony_cxd_result_t dvbt2_CalcQuality (sony_cxd_demod_t * pDemod,
                                        uint32_t ber, int32_t snr, uint8_t * pQuality)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    int32_t snrRel = 0;
    uint32_t berSQI = 0;
    sony_cxd_dvbt2_plp_constell_t qam;
    sony_cxd_dvbt2_plp_code_rate_t codeRate;

    static const int32_t snrNordigP1dB1000[4][8] = {
    /*   1/2,   3/5,    2/3,    3/4,    4/5,    5/6,   1/3,   2/5                */
        {3500,  4700,   5600,   6600,   7200,   7700,  1300,  2200 }, /* QPSK    */
        {8700,  10100,  11400,  12500,  13300,  13800, 6000,  7200 }, /* 16-QAM  */
        {13000, 14800,  16200,  17700,  18700,  19400, 9800,  11100}, /* 64-QAM  */
        {17000, 19400,  20800,  22900,  24300,  25100, 13200, 14800}, /* 256-QAM */
    };

    sony_cxd_TRACE_ENTER ("dvbt2_CalcQuality");

    if ((!pDemod) || (!pQuality)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Get PLP constellation */
    result = sony_cxd_demod_dvbt2_monitor_QAM (pDemod, sony_cxd_DVBT2_PLP_DATA, &qam);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Get PLP code rate */
    result = sony_cxd_demod_dvbt2_monitor_CodeRate (pDemod, sony_cxd_DVBT2_PLP_DATA, &codeRate);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Ensure correct plp info. */
    if ((codeRate > sony_cxd_DVBT2_R2_5) || (qam > sony_cxd_DVBT2_QAM256)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_OTHER);
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

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_Quality (sony_cxd_demod_t * pDemod,
                                                uint8_t * pQuality)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint32_t ber = 0;
    int32_t snr = 0;
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_Quality");

    if ((!pDemod) || (!pQuality)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Get Pre-BCH BER. */
    result = sony_cxd_demod_dvbt2_monitor_PreBCHBER (pDemod, &ber);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Get SNR value. */
    result = sony_cxd_demod_dvbt2_monitor_SNR (pDemod, &snr);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Calc Quality */
    result = dvbt2_CalcQuality (pDemod, ber, snr, pQuality);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_TSRate (sony_cxd_demod_t * pDemod, uint32_t * pTSRateKbps)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint32_t rd_smooth_dp = 0;
    uint32_t ep_ck_nume = 0;
    uint32_t ep_ck_deno = 0;
    uint8_t issy_on_data = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_TSRate");

    if ((!pDemod) || (!pTSRateKbps)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[8];
        uint8_t syncState = 0;
        uint8_t tsLock = 0;
        uint8_t earlyLock = 0;
        /* Freeze registers */
		if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        result = sony_cxd_demod_dvbt2_monitor_SyncStat (pDemod, &syncState, &tsLock, &earlyLock);
        if (result != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Check TS lock. */
        if (!tsLock) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-T Bank : 0x20 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------------------
         * <SLV-T>   20h     E5h     [4:0]        IREG_SP_RD_SMOOTH_DP[28:24]
         * <SLV-T>   20h     E6h     [7:0]        IREG_SP_RD_SMOOTH_DP[23:16]
         * <SLV-T>   20h     E7h     [7:0]        IREG_SP_RD_SMOOTH_DP[15:8]
         * <SLV-T>   20h     E8h     [7:0]        IREG_SP_RD_SMOOTH_DP[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE5, &data[0], 4) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        rd_smooth_dp = (uint32_t) ((data[0] & 0x1F) << 24);
        rd_smooth_dp |= (uint32_t) (data[1] << 16);
        rd_smooth_dp |= (uint32_t) (data[2] << 8);
        rd_smooth_dp |= (uint32_t) data[3];

        /* rd_smooth_dp should be > 214958 = < 100Mbps */
        if (rd_smooth_dp < 214958) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-T Bank : 0x24 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x24) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------------------
         * <SLV-T>   24h     90h     [0]          IREG_SP_ISSY_ON_DATA
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x90, &data[0], 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        issy_on_data = data[0] & 0x01;

        /* Set SLV-T Bank : 0x25 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x25) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------------------
         * <SLV-T>   25h     B2h     [5:0]        IREG_SP_EP_CK_NUME[29:24]
         * <SLV-T>   25h     B3h     [7:0]        IREG_SP_EP_CK_NUME[23:16]
         * <SLV-T>   25h     B4h     [7:0]        IREG_SP_EP_CK_NUME[15:8]
         * <SLV-T>   25h     B5h     [7:0]        IREG_SP_EP_CK_NUME[7:0]
         * <SLV-T>   25h     B6h     [5:0]        IREG_SP_EP_CK_DENO[29:24]
         * <SLV-T>   25h     B7h     [7:0]        IREG_SP_EP_CK_DENO[23:16]
         * <SLV-T>   25h     B8h     [7:0]        IREG_SP_EP_CK_DENO[15:8]
         * <SLV-T>   25h     B9h     [7:0]        IREG_SP_EP_CK_DENO[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB2, &data[0], 8) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        ep_ck_nume = (uint32_t) ((data[0] & 0x3F) << 24);
        ep_ck_nume |= (uint32_t) (data[1] << 16);
        ep_ck_nume |= (uint32_t) (data[2] << 8);
        ep_ck_nume |= (uint32_t) data[3];

        ep_ck_deno = (uint32_t) ((data[4] & 0x3F) << 24);
        ep_ck_deno |= (uint32_t) (data[5] << 16);
        ep_ck_deno |= (uint32_t) (data[6] << 8);
        ep_ck_deno |= (uint32_t) data[7];

        /* Unfreeze registers */
		SLVT_CXD_UnFreezeReg(pDemod);
    }

    if (issy_on_data) {
        if ((ep_ck_deno == 0) || (ep_ck_nume == 0) || (ep_ck_deno >= ep_ck_nume)) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }
    }

    /* IREG_SP_ISSY_ON_DATA    TS Data output rate[Mbps]
     * ------------------------------------------------------------------------------------------------------------
     *          0              ick * 2^18 / IREG_SP_RD_SMOOTH_DP
     *          1              (ick * 2^18 / IREG_SP_RD_SMOOTH_DP ) * ( IREG_SP_EP_CK_DENO / IREG_SP_EP_CK_NUME )
     *
     * ick = 96
     *
     * TSRate = ick * 2^18 / IREG_SP_RD_SMOOTH_DP (Mbps)
     *        = ick * 2^18 * 10^3 / IREG_SP_RD_SMOOTH_DP (kbps)
     */
    {
        /* Calculate ick * 2^18 * 10^3 / IREG_SP_RD_SMOOTH_DP */
        /* Divide in 3 steps to prevent overflow */
        uint32_t ick = 96;
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        div = rd_smooth_dp;

        Q = ick * 262144U * 100U / div;
        R = ick * 262144U * 100U % div;

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

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_PER (sony_cxd_demod_t * pDemod, uint32_t * pPER)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint32_t packetError = 0;
    uint32_t period = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_PER");

    if (!pDemod || !pPER) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }
    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t rdata[3];

        /* Set SLV-T Bank : 0x24 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x24) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /*  slave    Bank    Addr    Bit     Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   24h     FAh      [0]    IREG_SP_B2PER_VALID
         * <SLV-T>   24h     FBh    [7:0]    IREG_SP_B2PER_PKTERR[15:8]
         * <SLV-T>   24h     FCh    [7:0]    IREG_SP_B2PER_PKTERR[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFA, rdata, 3) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if ((rdata[0] & 0x01) == 0) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);    /* Not ready... */
        }

        packetError = (rdata[1] << 8) | rdata[2];

        /*  slave    Bank    Addr    Bit     Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   24h     DCh    [3:0]    OREG_SP_PER_MES[3:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xDC, rdata, 1) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        period = 1U << (rdata[0] & 0x0F);
    }

    if ((period == 0) || (packetError > period)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
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

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_QAM (sony_cxd_demod_t * pDemod,
                                            sony_cxd_dvbt2_plp_btype_t type,
                                            sony_cxd_dvbt2_plp_constell_t * pQAM)
{
    uint8_t data;
    uint8_t l1PostOk = 0;
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_QAM");

    if ((!pDemod) || (!pQAM)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the registers. */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x22 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Check L1 Post is valid
     * slave    Bank    Addr    Bit    Name
     * ------------------------------------------
     * <SLV-T>  22h     12h     [0]    IL1POST_OK
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x12, &l1PostOk, 1) != sony_cxd_RESULT_OK) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (!(l1PostOk & 0x01)) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
    }

    if (type == sony_cxd_DVBT2_PLP_COMMON) {
        /* slave    Bank    Addr    Bit       Name
         * ----------------------------------------------------------------
         * <SLV-T>   22h     74h     [7:0]    IL1POST_FRAME_INTERVAL_C[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x74, &data, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* If common, check for no common PLP, frame_interval == 0. */
        if (data == 0) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* slave    Bank    Addr    Bit           Name
         * -----------------------------------------------------------
         * <SLV-T>   22h     6Fh     [2:0]        IL1POST_PLP_MOD_C[2:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6F, &data, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
    }
    else {
        /* slave    Bank    Addr    Bit           Name
         * --------------------------------------------
         * <SLV-T>   22h     5Ch     [2:0]        IL1POST_PLP_MOD[2:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5C, &data, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* Unfreeze registers */
	SLVT_CXD_UnFreezeReg(pDemod);

    /* Obtain PLP constellation from register value */
    *pQAM = (sony_cxd_dvbt2_plp_constell_t) (data & 0x07);

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_CodeRate (sony_cxd_demod_t * pDemod,
                                                 sony_cxd_dvbt2_plp_btype_t type,
                                                 sony_cxd_dvbt2_plp_code_rate_t * pCodeRate)
{
    uint8_t data;
    uint8_t l1PostOk = 0;
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_CodeRate");

    if ((!pDemod) || (!pCodeRate)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the registers. */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x22 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Check L1 Post is valid
     * slave    Bank    Addr    Bit    Name
     * ------------------------------------------
     * <SLV-T>  22h     12h     [0]    IL1POST_OK
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x12, &l1PostOk, 1) != sony_cxd_RESULT_OK) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (!(l1PostOk & 0x01)) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
    }

    if (type == sony_cxd_DVBT2_PLP_COMMON) {
        /* slave    Bank    Addr    Bit       Name
         * ----------------------------------------------------------------
         * <SLV-T>   22h     74h     [7:0]    IL1POST_FRAME_INTERVAL_C[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x74, &data, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* If common, check for no common PLP, frame_interval == 0. */
        if (data == 0) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* slave    Bank    Addr    Bit           Name
         * -----------------------------------------------------------
         * <SLV-T>   22h     6Eh     [2:0]        IL1POST_PLP_COD_C[2:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x6E, &data, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
    }
    else {
        /* slave    Bank    Addr    Bit           Name
         * --------------------------------------------
         * <SLV-T>   22h     5Bh     [2:0]        IL1POST_PLP_COD[2:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5B, &data, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }
    }

    /* Unfreeze registers */
	SLVT_CXD_UnFreezeReg(pDemod);

    /* Obtain PLP code rate from register value */
    *pCodeRate = (sony_cxd_dvbt2_plp_code_rate_t) (data & 0x07);

    sony_cxd_TRACE_RETURN (result);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_Profile (sony_cxd_demod_t * pDemod, sony_cxd_dvbt2_profile_t * pProfile)
{
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_Profile");

    if ((!pDemod) || (!pProfile)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x2E */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x2E) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    {
        uint8_t data;
        /* slave    Bank    Addr    Bit         Name                Meaning
         * ---------------------------------------------------------------------------
         * <SLV-T>   2Eh     20h     [1]        IREG_T2MODE_OK      0:Invalid, 1:Valid
         * <SLV-T>   2Eh     20h     [0]        IREG_T2MODE         0:Base, 1:Lite
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x20, &data, sizeof (data)) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        if (data & 0x02) {
            /* T2 profile is valid */
            if (data & 0x01) {
                *pProfile = sony_cxd_DVBT2_PROFILE_LITE;
            }
            else {
                *pProfile = sony_cxd_DVBT2_PROFILE_BASE;
            }
        }
        else {
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
        }
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_AveragedPreBCHBER (sony_cxd_demod_t * pDemod,
                                                          uint32_t * pBER)
{
    uint8_t rdata[32];
    uint8_t periodType = 0;
    uint32_t bitError = 0;
    int hNum = 0;
    uint32_t periodExp = 0;
    uint32_t periodNum = 0;
    uint32_t n_bch = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_AveragedPreBCHBER");

    if ((!pDemod) || (!pBER)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x20 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /*  slave    Bank    Addr    Bit     Name
     * -----------------------------------------------------
     * <SLV-T>   20h     7Ch      [0]    OREG_BER_MODE_SEL
     * <SLV-T>   20h     7Dh    [1:0]    IREG_BER_UNEXP[1:0]
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7C, rdata, 2) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    if (rdata[1] != 0x00) {
        /* Unexpected behavior */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
    }

    periodType = rdata[0];

    /* Freeze registers */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x02 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != sony_cxd_RESULT_OK) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /*  slave    Bank    Addr    Bit     Name
     * ---------------------------------------------------------------------------------
     * <SLV-T>   02h     10h    [7:0]    IREG_BER_VALID0,IREG_BITERR0[22:16]
     * <SLV-T>   02h     11h    [7:0]    IREG_BITERR0[15:8]
     * <SLV-T>   02h     12h    [7:0]    IREG_BITERR0[7:0]
     * <SLV-T>   02h     13h    [4:0]    IREG_PERIOD0[4:0]
     * ...
     * <SLV-T>   02h     2Ch    [7:0]    IREG_BER_VALID7,IREG_BITERR7[22:16]
     * <SLV-T>   02h     2Dh    [7:0]    IREG_BITERR7[15:8]
     * <SLV-T>   02h     2Eh    [7:0]    IREG_BITERR7[7:0]
     * <SLV-T>   02h     2Fh    [4:0]    IREG_PERIOD7[4:0]
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, rdata, 32) != sony_cxd_RESULT_OK) {
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    for (hNum = 0; hNum < 8; hNum++) {
        /* Check IREG_BER_VALIDX */
        if ((rdata[hNum * 4] & 0x80) == 0x00) {
            break;
        }

        bitError += ((rdata[hNum * 4] & 0x7F) << 16) + (rdata[hNum * 4 + 1] << 8) + rdata[hNum * 4 + 2];
    }

    if (hNum == 0) {
        /* There are no valid history */
		SLVT_CXD_UnFreezeReg(pDemod);
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
    }

    {
        sony_cxd_dvbt2_plp_fec_t plpFecType = sony_cxd_DVBT2_FEC_LDPC_16K;
        sony_cxd_dvbt2_plp_code_rate_t plpCr = sony_cxd_DVBT2_R1_2;

        static const uint16_t nBCHBitsLookup[2][8] = {
          /* R1_2   R3_5   R2_3   R3_4   R4_5   R5_6   R1_3   R2_5 */
            {7200,  9720,  10800, 11880, 12600, 13320, 5400,  6480}, /* 16K FEC */
            {32400, 38880, 43200, 48600, 51840, 54000, 21600, 25920} /* 64k FEC */
        };

        /* Set SLV-T Bank : 0x22 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x22) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* Get the PLP FEC type (Normal/Short).
         * slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   22h     5Eh     [1:0]        IL1POST_PLP_FEC_TYPE[1:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5E, rdata, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        plpFecType = (sony_cxd_dvbt2_plp_fec_t) (rdata[0] & 0x03);

        /* Get the PLP code rate.
         * slave    Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         * <SLV-T>   22h     5Bh     [2:0]        IL1POST_PLP_COD[2:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5B, rdata, 1) != sony_cxd_RESULT_OK) {
			SLVT_CXD_UnFreezeReg(pDemod);
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        plpCr = (sony_cxd_dvbt2_plp_code_rate_t) (rdata[0] & 0x07);

		SLVT_CXD_UnFreezeReg(pDemod);

        /* Confirm FEC Type / Code Rate */
        if ((plpFecType > sony_cxd_DVBT2_FEC_LDPC_64K) || (plpCr > sony_cxd_DVBT2_R2_5)) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
        }

        n_bch = nBCHBitsLookup[plpFecType][plpCr];
    }

    if (periodType == 0) {
        /* Data period */

        /* Set SLV-T Bank : 0x20 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x20) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        /* slave    Bank    Addr    Bit         Name
         * -------------------------------------------------------------------------
         * <SLV-T>   20h     72h     [3:0]      OREG_BBER_MES[3:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x72, rdata, 1) != sony_cxd_RESULT_OK) {
            sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
        }

        periodExp = (rdata[0] & 0x0F);
        periodNum = hNum;
    } else {
        /* Time period */
        int i = 0;

        periodExp = 0xFF;

        /* Check minimum period */
        for (i = 0; i < hNum; i++) {
            uint8_t expTmp = rdata[i * 4 + 3];

            if (periodExp > expTmp) {
                periodExp = expTmp;
            }
        }

        /* Calc period by 2^(minimum period) basis
         * The difference between max and min period should be 1,
         * so the max of periodNum should be 15.
         */
        for (i = 0; i < hNum; i++) {
            periodNum += 1 << (rdata[i * 4 + 3] - periodExp);
        }
    }

    {
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        if (periodExp >= 6) {
            /*
              BER = bitError * 1000000000 / (2^N * n_bch * num)
                  = bitError * 390625 / (2^(N-6) * (n_bch / 40) * num)
                  (NOTE: 1000000000 / 2^6 / 40 = 390625)
                  = bitError * 125 * 125 * 25 / (2^(N-6) * (n_bch / 40) * num)
                  (Divide in 3 steps to prevent overflow.)

              Max of bitError is 0x3FFFFF * 8.
              Max of num is 15.
              Max of N is 15.
            */
            div = (1U << (periodExp - 6)) * (n_bch / 40) * periodNum;

            Q = (bitError * 125) / div;
            R = (bitError * 125) % div;

            R *= 125;
            Q = Q * 125 + R / div;
            R = R % div;

            R *= 25;
            Q = Q * 25 + R / div;
            R = R % div;
        }
        else {
            /*
              BER = bitError * 1000000000 / (2^N * n_bch * num)
                  = bitError * 25000000 / (2^N * (n_bch / 40) * num)
                  = bitError * 100 * 500 * 500 / (2^N * (n_bch / 40) * num)
                  (Divide in 3 steps to prevent overflow.)

              Max of bitError is 0x3FFFFF * 8.
              Max of num is 15.
              Max of N is 15.
            */
            div = (1U << periodExp) * (n_bch / 40) * periodNum;

            Q = (bitError * 100) / div;
            R = (bitError * 100) % div;

            R *= 500;
            Q = Q * 500 + R / div;
            R = R % div;

            R *= 500;
            Q = Q * 500 + R / div;
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

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_AveragedSNR (sony_cxd_demod_t * pDemod,
                                                    int32_t * pSNR)
{
    uint8_t rdata[24];
    int hNum = 0;
    uint32_t reg = 0;

    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_AveragedSNR");

    if ((!pDemod) || (!pSNR)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x02 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    /*  slave    Bank    Addr    Bit     Name
     * ---------------------------------------------------------------------------------
     * <SLV-T>   02h     30h    [0]      IREG_SNR_VALID0
     * <SLV-T>   02h     31h    [7:0]    IREG_SNR0[15:8]
     * <SLV-T>   02h     32h    [7:0]    IREG_SNR0[7:0]
     * ...
     * <SLV-T>   02h     45h    [0]      IREG_SNR_VALID7
     * <SLV-T>   02h     46h    [7:0]    IREG_SNR7[15:8]
     * <SLV-T>   02h     47h    [7:0]    IREG_SNR7[7:0]
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x30, rdata, 24) != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_IO);
    }

    for (hNum = 0; hNum < 8; hNum++) {
        /* Check IREG_SNR_VALIDX */
        if ((rdata[hNum * 3] & 0x01) == 0x00) {
            break;
        }

        reg += (rdata[hNum * 3 + 1] << 8) + rdata[hNum * 3 + 2];
    }

    if (hNum == 0) {
        /* There are no valid history */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_HW_STATE);
    }

    reg = (reg + hNum / 2) / hNum; /* average (round) */

    /* Same as sony_cxd_demod_dvbt2_monitor_SNR
     *
     * IREG_SNMON_OD is clipped at a maximum of 10876.
     *
     * SNR = 10 * log10 (IREG_SNMON_OD / (12600 - IREG_SNMON_OD)) + 32.0
     *     = 10 * (log10(IREG_SNMON_OD) - log10(12600 - IREG_SNMON_OD)) + 32.0
     * sony_cxd_log10 returns log10(x) * 100
     * Therefore SNR(dB) * 1000 :
     *     = 10 * 10 * (sony_cxd_log10(IREG_SNMON_OD) - sony_cxd_log10(12600 - IREG_SNMON_OD) + 32000
     */
    if (reg > 10876) {
        reg = 10876;
    }

    *pSNR = 10 * 10 * ((int32_t) sony_cxd_math_log10 (reg) - (int32_t) sony_cxd_math_log10 (12600 - reg));
    *pSNR += 32000;

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_dvbt2_monitor_AveragedQuality (sony_cxd_demod_t * pDemod,
                                                        uint8_t * pQuality)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint32_t ber = 0;
    int32_t snr = 0;
    sony_cxd_TRACE_ENTER ("sony_cxd_demod_dvbt2_monitor_AveragedQuality");

    if ((!pDemod) || (!pQuality)) {
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != sony_cxd_DTV_SYSTEM_DVBT2) {
        /* Not DVB-T2 */
        sony_cxd_TRACE_RETURN (sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Get Pre-BCH BER. */
    result = sony_cxd_demod_dvbt2_monitor_AveragedPreBCHBER (pDemod, &ber);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Get SNR value. */
    result = sony_cxd_demod_dvbt2_monitor_AveragedSNR (pDemod, &snr);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    /* Calc Quality */
    result = dvbt2_CalcQuality (pDemod, ber, snr, pQuality);
    if (result != sony_cxd_RESULT_OK) {
        sony_cxd_TRACE_RETURN (result);
    }

    sony_cxd_TRACE_RETURN (sony_cxd_RESULT_OK);
}
