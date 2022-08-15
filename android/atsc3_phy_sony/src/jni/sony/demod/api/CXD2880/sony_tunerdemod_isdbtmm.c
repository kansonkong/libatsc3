/*------------------------------------------------------------------------------
  Copyright 2015 Sony Corporation

  Last Updated    : 2015/12/04
  Modification ID : 271367e61eabcaae9f67d930a1a4a65cb7ff5c58
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_isdbtmm.h"
#include "sony_tunerdemod_isdbt.h"

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_result_t X_tune_isdbtmm_B_DemodSetting (sony_tunerdemod_t * pTunerDemod, uint8_t startSubChannel,
                                                    uint8_t totalSegmentNum, uint8_t start_seg, uint8_t tune_seg, uint8_t segnum)
{
    SONY_TRACE_ENTER ("X_tune_isdbtmm_B_DemodSetting");

    /* Tmm TypeB initial setting
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>   04h     7Bh    [2:0]     8'h07      8'h02     OREG_COSNE_CRANGE[2:0]
     * <SLV-T>   15h     45h    [3:0]     8'h04      8'h03     OREG_SYR_SMT_ALPHA[3:0]
     * <SLV-T>   15h     4Ah    [7:0]     8'h88      8'h55     OREG_SYR_PGCE_ALPHA_ACQ[3:0],OREG_SYR_PGCE_ALPHA_TRK[3:0]
     * <SLV-T>   60h     F6h    [3:0]     8'h07      8'hxx     OREG_DSCP_SUBCHAN_INDEX
     * <SLV-T>   60h     F8h    [4:0]     8'h00      8'h1x     OREG_FORCE_SEGTYPE,OREG_SEGTYPE[3:0]
     * <SLV-T>   62h     D3h    [3:0]     8'h00      8'hxx     OREG_START_SEGMENT[3:0]
     * <SLV-M>   62h     D5h    [0]       8'h00      8'h01     OREG_FEC_SEGNUM_SELJ_ON
     * <SLV-T>   63h     1Bh    [0]       8'h00      8'h01     OREG_TMCC_SEGMENT_MANUAL
     */

    /* Tmm TypeB Tune setting
     *
     *  slave    Bank    Addr    Bit    default   Value          Name
     * ---------------------------------------------------------------------------------
     * <SLV-T>   62h     D4h    [3:0]     8'h00    tune_seg    OREG_TUNE_SEGMENT[3:0]
     * <SLV-M>   62h     D6h    [3:0]     8'h00    segnum      OREG_FEC_SEGNUM_SELJ[3:0]
     * <SLV-T>   63h     1Dh    [3:0]     8'h03    segnum      OREG_TMCC_SEGMENT[3:0]
     */

    /* Note : tune_seg and segnum is the "index" from start segment specified by OREG_DSCP_SUBCHAN_INDEX. */

    /* SLV-T bank 0x04 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x7B, 0x02) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x15 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x15) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x45, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4A, 0x55) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x60 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x60) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xF6, startSubChannel) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
                              /* 1  2  3  4  5  6  7   8   9  10  11  12  13 */
        const uint8_t cdata[] = {2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        uint8_t data = 0x10;

        if ((totalSegmentNum < 1) || (totalSegmentNum > 13)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        data |= cdata[totalSegmentNum - 1];

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xF8, data) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* SLV-T bank 0x62 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x62) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t data[2];

        data[0] = start_seg; /* 0xD3 */
        data[1] = tune_seg;

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xD3, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_SUB) {
        uint8_t data[2];

        data[0] = 0x01; /* 0xD5 */
        data[1] = segnum;

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xD5, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* SLV-T bank 0x63 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x63) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1B, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1D, segnum) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Clock enable (OREG_CK_EN) */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xFD, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t X_sleep_isdbtmm_B_DemodSetting (sony_tunerdemod_t * pTunerDemod)
{
    SONY_TRACE_ENTER ("X_sleep_isdbtmm_B_DemodSetting");

    /* slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>   04h     7Bh    [2:0]     8'h07      8'h07     OREG_COSNE_CRANGE[2:0]
     * <SLV-T>   15h     45h    [3:0]     8'h04      8'h04     OREG_SYR_SMT_ALPHA[3:0]
     * <SLV-T>   15h     4Ah    [7:0]     8'h88      8'h88     OREG_SYR_PGCE_ALPHA_ACQ[3:0],OREG_SYR_PGCE_ALPHA_TRK[3:0]
     * <SLV-T>   60h     F6h    [3:0]     8'h07      8'h07     OREG_DSCP_SUBCHAN_INDEX
     * <SLV-T>   60h     F8h    [4:0]     8'h00      8'h00     OREG_FORCE_SEGTYPE,OREG_SEGTYPE[3:0]
     * <SLV-T>   62h     D3h    [3:0]     8'h00      8'h00     OREG_START_SEGMENT[3:0]
     * <SLV-M>   62h     D5h    [0]       8'h00      8'h00     OREG_FEC_SEGNUM_SELJ_ON
     * <SLV-T>   63h     1Bh    [0]       8'h00      8'h00     OREG_TMCC_SEGMENT_MANUAL
     */

    /* SLV-T bank 0x04 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x7B, 0x07) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x15 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x15) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x45, 0x04) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4A, 0x88) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x60 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x60) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xF6, 0x07) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xF8, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x62 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x62) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xD3, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_SUB) {
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xD5, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* SLV-T bank 0x63 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x63) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1B, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tunerdemod_isdbtmm_A_ConvertTuneParam (sony_tunerdemod_t * pTunerDemod,
                                                          sony_isdbtmm_A_tune_param_t * pTuneParamA,
                                                          sony_isdbtmm_tune_param_t * pTuneParam)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtmm_A_ConvertTuneParam");

    if ((!pTunerDemod) || (!pTuneParamA) || (!pTuneParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    switch (pTuneParam->segmentIndex) {
    case 6:  /* Pattern (A), (C) */
    case 13: /* Pattern (B) */
        pTuneParamA->oneSegmentOptimizeShiftDirection = 1; /* Upper */
        break;
    case 19: /* Pattern (C) */
    case 26: /* Pattern (A), (B) */
        pTuneParamA->oneSegmentOptimizeShiftDirection = 0; /* Lower */
        break;
    default:
        /* Invalid segment index. */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
    }

    pTuneParamA->centerFreqKHz = SONY_ISDBTMM_SEQMENT_FREQ_KHZ (pTuneParam->segmentIndex);
    pTuneParamA->oneSegmentOptimize = pTuneParam->oneSegmentOptimize;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbtmm_B_ConvertTuneParam (sony_tunerdemod_t * pTunerDemod,
                                                          sony_isdbtmm_B_tune_param_t * pTuneParamB,
                                                          sony_isdbtmm_tune_param_t * pTuneParam)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtmm_B_ConvertTuneParam");

    if ((!pTunerDemod) || (!pTuneParamB) || (!pTuneParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTuneParam->segmentIndex >= 0) && (pTuneParam->segmentIndex <= 6)) {
        /* Pattern (B) */
        /* RF tuner is tuned to segment index 6 center frequency. */
        pTuneParamB->tunerFrequencyKHz = SONY_ISDBTMM_SEQMENT_FREQ_KHZ (6);
        pTuneParamB->targetSubChannel = pTuneParam->segmentIndex;
        pTuneParamB->tunerCenterSubChannel = 6;
        pTuneParamB->oneSegmentOptimizeShiftDirection = 1; /* Upper */
    } else if ((pTuneParam->segmentIndex >= 13) && (pTuneParam->segmentIndex <= 19)) {
        /* Pattern (A) */
        /* RF tuner is tuned to segment index 13 center frequency. */
        pTuneParamB->tunerFrequencyKHz = SONY_ISDBTMM_SEQMENT_FREQ_KHZ (13);
        pTuneParamB->targetSubChannel = pTuneParam->segmentIndex - 13;
        pTuneParamB->tunerCenterSubChannel = 0;
        pTuneParamB->oneSegmentOptimizeShiftDirection = 1; /* Upper */
    } else if ((pTuneParam->segmentIndex >= 26) && (pTuneParam->segmentIndex <= 32)) {
        /* Pattern (C) */
        /* RF tuner is tuned to segment index 26 center frequency. */
        pTuneParamB->tunerFrequencyKHz = SONY_ISDBTMM_SEQMENT_FREQ_KHZ (26);
        pTuneParamB->targetSubChannel = pTuneParam->segmentIndex - 26;
        pTuneParamB->tunerCenterSubChannel = 0;
        pTuneParamB->oneSegmentOptimizeShiftDirection = 0; /* Lower */
    } else {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
    }

    pTuneParamB->startSubChannel = 0;
    pTuneParamB->totalSegmentNumber = 7;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbtmm_A_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                               sony_isdbtmm_A_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtmm_A_Tune1");

    if ((!pTunerDemod) || (!pTuneParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* System independent settings */
    result = sony_tunerdemod_CommonTuneSetting1 (pTunerDemod, SONY_DTV_SYSTEM_ISDBTMM_A, pTuneParam->centerFreqKHz, SONY_DTV_BW_6_MHZ,
        pTuneParam->oneSegmentOptimize, pTuneParam->oneSegmentOptimizeShiftDirection);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* ISDB-T dependent settings */
    result = sony_tunerdemod_isdbt_X_tune_DemodSetting (pTunerDemod, SONY_DTV_BW_6_MHZ, pTunerDemod->clockMode, 1);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = sony_tunerdemod_isdbt_X_tune_DemodSetting (pTunerDemod->pDiverSub, SONY_DTV_BW_6_MHZ, pTunerDemod->pDiverSub->clockMode, 1);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* ISDB-T preset setting */
    {
        sony_tunerdemod_isdbt_preset_info_t * pPresetInfo;

        if (pTunerDemod->isdbtPresetInfoEnable) {
            pPresetInfo = &pTunerDemod->isdbtPresetInfo;
        } else {
            pPresetInfo = NULL;
        }

        result = sony_tunerdemod_isdbt_PresetSetting (pTunerDemod, pPresetInfo);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            result = sony_tunerdemod_isdbt_PresetSetting (pTunerDemod->pDiverSub, pPresetInfo);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbtmm_A_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                               sony_isdbtmm_A_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtmm_A_Tune2");

    if ((!pTunerDemod) || (!pTuneParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* System independent settings */
    result = sony_tunerdemod_CommonTuneSetting2 (pTunerDemod, SONY_DTV_SYSTEM_ISDBTMM_A, 0);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    pTunerDemod->state = SONY_TUNERDEMOD_STATE_ACTIVE;
    pTunerDemod->frequencyKHz = pTuneParam->centerFreqKHz;
    pTunerDemod->system = SONY_DTV_SYSTEM_ISDBTMM_A;
    pTunerDemod->bandwidth = SONY_DTV_BW_6_MHZ;

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        pTunerDemod->pDiverSub->state = SONY_TUNERDEMOD_STATE_ACTIVE;
        pTunerDemod->pDiverSub->frequencyKHz = pTuneParam->centerFreqKHz;
        pTunerDemod->pDiverSub->system = SONY_DTV_SYSTEM_ISDBTMM_A;
        pTunerDemod->pDiverSub->bandwidth = SONY_DTV_BW_6_MHZ;
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbtmm_B_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                               sony_isdbtmm_B_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t start_seg = 0;
    uint8_t tune_seg = 0;
    uint8_t segnum = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtmm_B_Tune1");

    if ((!pTunerDemod) || (!pTuneParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Tune param check */
    if (pTuneParam->startSubChannel > 13) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
    }

    if ((pTuneParam->totalSegmentNumber < 1) || (pTuneParam->totalSegmentNumber > 13)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
    }

    if ((pTuneParam->tunerCenterSubChannel < pTuneParam->targetSubChannel - 6)
        || (pTuneParam->tunerCenterSubChannel > pTuneParam->targetSubChannel + 6)) {
        /* Tuner bandwidth is 13 segment. This means |(tuner center) - (segment center)| <= 6 is necessary. */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
    }

    if ((pTuneParam->targetSubChannel < pTuneParam->startSubChannel)
        || (pTuneParam->targetSubChannel >= pTuneParam->startSubChannel + pTuneParam->totalSegmentNumber)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
    }

    if ((pTuneParam->totalSegmentNumber == 1) && (pTuneParam->tunerCenterSubChannel != pTuneParam->targetSubChannel)) {
        /* For one segment reception, tuner center frequency should be equal to segment center frequency. */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    segnum = pTuneParam->targetSubChannel - pTuneParam->startSubChannel;

    if (pTuneParam->tunerCenterSubChannel >= pTuneParam->startSubChannel) {
        tune_seg = (uint8_t)(pTuneParam->tunerCenterSubChannel - pTuneParam->startSubChannel);
        start_seg = 0;
    } else {
        start_seg = (uint8_t)(pTuneParam->startSubChannel - pTuneParam->tunerCenterSubChannel);
        tune_seg = 0;
    }

    /* System independent settings */
    /* If (tuner center) == (segment center), one segment optimization is necessary. */
    result = sony_tunerdemod_CommonTuneSetting1 (pTunerDemod, SONY_DTV_SYSTEM_ISDBTMM_B, pTuneParam->tunerFrequencyKHz, SONY_DTV_BW_6_MHZ,
        (pTuneParam->tunerCenterSubChannel == pTuneParam->targetSubChannel) ? 1 : 0, pTuneParam->oneSegmentOptimizeShiftDirection);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* ISDB-T dependent settings */
    result = sony_tunerdemod_isdbt_X_tune_DemodSetting (pTunerDemod, SONY_DTV_BW_6_MHZ, pTunerDemod->clockMode, 0);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = sony_tunerdemod_isdbt_X_tune_DemodSetting (pTunerDemod->pDiverSub, SONY_DTV_BW_6_MHZ, pTunerDemod->pDiverSub->clockMode, 0);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* ISDB-Tmm Type-B dependent settings */
    result = X_tune_isdbtmm_B_DemodSetting (pTunerDemod, pTuneParam->startSubChannel, pTuneParam->totalSegmentNumber,
        start_seg, tune_seg, segnum);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = X_tune_isdbtmm_B_DemodSetting (pTunerDemod->pDiverSub, pTuneParam->startSubChannel, pTuneParam->totalSegmentNumber,
            start_seg, tune_seg, segnum);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* ISDB-T preset setting */
    {
        sony_tunerdemod_isdbt_preset_info_t * pPresetInfo;

        if (pTunerDemod->isdbtPresetInfoEnable) {
            pPresetInfo = &pTunerDemod->isdbtPresetInfo;
        } else {
            pPresetInfo = NULL;
        }

        result = sony_tunerdemod_isdbt_PresetSetting (pTunerDemod, pPresetInfo);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            result = sony_tunerdemod_isdbt_PresetSetting (pTunerDemod->pDiverSub, pPresetInfo);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbtmm_B_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                               sony_isdbtmm_B_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtmm_B_Tune2");

    if ((!pTunerDemod) || (!pTuneParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* System independent settings */
    result = sony_tunerdemod_CommonTuneSetting2 (pTunerDemod, SONY_DTV_SYSTEM_ISDBTMM_B, 0);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    pTunerDemod->state = SONY_TUNERDEMOD_STATE_ACTIVE;
    pTunerDemod->frequencyKHz = pTuneParam->tunerFrequencyKHz;
    pTunerDemod->system = SONY_DTV_SYSTEM_ISDBTMM_B;
    pTunerDemod->bandwidth = SONY_DTV_BW_6_MHZ;

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        pTunerDemod->pDiverSub->state = SONY_TUNERDEMOD_STATE_ACTIVE;
        pTunerDemod->pDiverSub->frequencyKHz = pTuneParam->tunerFrequencyKHz;
        pTunerDemod->pDiverSub->system = SONY_DTV_SYSTEM_ISDBTMM_B;
        pTunerDemod->pDiverSub->bandwidth = SONY_DTV_BW_6_MHZ;
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbtmm_A_SleepSetting (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtmm_A_SleepSetting");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* ISDB-T dependent settings. */
    result = sony_tunerdemod_isdbt_SleepSetting (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbtmm_B_SleepSetting (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtmm_B_SleepSetting");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* ISDB-Tmm Type-B dependent settings. */
    result = X_sleep_isdbtmm_B_DemodSetting (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = X_sleep_isdbtmm_B_DemodSetting (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* ISDB-T dependent settings. */
    result = sony_tunerdemod_isdbt_SleepSetting (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}
