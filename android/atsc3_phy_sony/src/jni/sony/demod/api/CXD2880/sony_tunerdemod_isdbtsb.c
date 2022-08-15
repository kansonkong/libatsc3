/*------------------------------------------------------------------------------
  Copyright 2016 Sony Corporation

  Last Updated    : 2016/03/29
  Modification ID : c247b0fd8ce5951ee0f186647f755862889fa2e5
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_isdbtsb.h"
#include "sony_tunerdemod_isdbt.h"

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_result_t X_tune_isdbtsb_DemodSetting (sony_tunerdemod_t * pTunerDemod, uint8_t subChannel)
{
    SONY_TRACE_ENTER ("X_tune_isdbtsb_DemodSetting");

    /*
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>   60h     F6h    [3:0]     8'h07      8'hxx     OREG_DSCP_SUBCHAN_INDEX
     * <SLV-T>   60h     F7h    [2:0]     8'h04      8'h05     OREG_SEGCTL_AUTO_RECOVERY_OFF,OREG_SEGCTL_DEMOD_TYPE[1:0]
     */

    /* SLV-T bank 0x60 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x60) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t data[2];

        data[0] = subChannel;
        data[1] = 0x05;

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xF6, data, sizeof(data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t X_sleep_isdbtsb_DemodSetting (sony_tunerdemod_t * pTunerDemod)
{
    SONY_TRACE_ENTER ("X_tune_isdbtsb_DemodSetting");

    /*
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>   60h     F6h    [3:0]     8'h07      8'h07     OREG_DSCP_SUBCHAN_INDEX
     * <SLV-T>   60h     F7h    [2:0]     8'h04      8'h04     OREG_SEGCTL_AUTO_RECOVERY_OFF,OREG_SEGCTL_DEMOD_TYPE[1:0]
     * <SLV-T>   1Ah     54h    [7:0]     8'h50      8'h50     OREG_ZIF_DAGC_TRGT
     */

    /* SLV-T bank 0x60 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x60) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t data[2] = {0x07, 0x04};

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xF6, data, sizeof(data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* SLV-T bank 0x1A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x1A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x54, 0x50) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tunerdemod_isdbtsb_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                             sony_isdbtsb_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtsb_Tune1");

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
    if (pTuneParam->subChannel > 13) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
    }

    /* System independent settings */
    /* One segment optimization is always ON. */
    result = sony_tunerdemod_CommonTuneSetting1 (pTunerDemod, SONY_DTV_SYSTEM_ISDBTSB, pTuneParam->centerFreqKHz, pTuneParam->bandwidth, 1, 0);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* ISDB-T dependent settings */
    result = sony_tunerdemod_isdbt_X_tune_DemodSetting (pTunerDemod, pTuneParam->bandwidth, pTunerDemod->clockMode, 1);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = sony_tunerdemod_isdbt_X_tune_DemodSetting (pTunerDemod->pDiverSub, pTuneParam->bandwidth, pTunerDemod->pDiverSub->clockMode, 1);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* ISDB-Tsb dependent settings */
    result = X_tune_isdbtsb_DemodSetting (pTunerDemod, pTuneParam->subChannel);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = X_tune_isdbtsb_DemodSetting (pTunerDemod->pDiverSub, pTuneParam->subChannel);
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

sony_result_t sony_tunerdemod_isdbtsb_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                             sony_isdbtsb_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtsb_Tune2");

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
    result = sony_tunerdemod_CommonTuneSetting2 (pTunerDemod, SONY_DTV_SYSTEM_ISDBTSB, 0);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    pTunerDemod->state = SONY_TUNERDEMOD_STATE_ACTIVE;
    pTunerDemod->frequencyKHz = pTuneParam->centerFreqKHz;
    pTunerDemod->system = SONY_DTV_SYSTEM_ISDBTSB;
    pTunerDemod->bandwidth = pTuneParam->bandwidth;

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        pTunerDemod->pDiverSub->state = SONY_TUNERDEMOD_STATE_ACTIVE;
        pTunerDemod->pDiverSub->frequencyKHz = pTuneParam->centerFreqKHz;
        pTunerDemod->pDiverSub->system = SONY_DTV_SYSTEM_ISDBTSB;
        pTunerDemod->pDiverSub->bandwidth = pTuneParam->bandwidth;
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbtsb_AGCSetting (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtsb_AGCSetting");

    /* AGC setting
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>   04h     5Dh    [4:0]     8'h0B      8'h0C     OCTL_IFAGC_COARSEGAIN[4:0]
     * <SLV-T>   04h     81h    [7:0]     8'h19      8'h05     OREG_CAS_DAGC_EN[1:0],OREG_CAS_DAGC_RDCBW[1:0],OREG_CAS_DAGC_USELAST
     * <SLV-T>   1Ah     54h    [7:0]     8'h50      8'h30     OREG_ZIF_DAGC_TRGT
     */

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-T bank 0x04 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5D, 0x0C) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x81, 0x05) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x1A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x1A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x54, 0x30) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* Same setting to sub. */
        result = sony_tunerdemod_isdbtsb_AGCSetting (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbtsb_PresetSettingForceALayerOnly (sony_tunerdemod_t * pTunerDemod,
                                                                    sony_tunerdemod_isdbt_preset_info_t * pPresetInfo)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtsb_PresetSettingForceALayerOnly");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (!pPresetInfo) {
        result = sony_tunerdemod_isdbt_PresetSetting (pTunerDemod, NULL);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        /* Disable force preset setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  63h     3Fh     [1]     8'h00        8'h00     PIR_FORCE_TMCC_SET,PIR_TPSDISABLEUPDATE
         */

        /* Set SLV-T Bank : 0x63 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x63) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x3F, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    } else {
        pPresetInfo->data[2] |= 0x07; /* B41-43 = 1 */
        pPresetInfo->data[3] = 0xFF;
        pPresetInfo->data[4] = 0xFF;
        pPresetInfo->data[5] = 0xFF;
        pPresetInfo->data[6] = 0xFF;
        pPresetInfo->data[7] = 0xFF;
        pPresetInfo->data[8] = 0xFF;
        pPresetInfo->data[9] = 0xFF;
        pPresetInfo->data[10] = 0xFF;
        pPresetInfo->data[11] = 0xFF;
        pPresetInfo->data[12] = 0xFC;

        result = sony_tunerdemod_isdbt_PresetSetting (pTunerDemod, pPresetInfo);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        /* Enable force preset setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  63h     3Fh     [1]     8'h00        8'h02     PIR_FORCE_TMCC_SET,PIR_TPSDISABLEUPDATE
         */

        /* Set SLV-T Bank : 0x63 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x63) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x3F, 0x02) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* Same setting to sub. */
        result = sony_tunerdemod_isdbtsb_PresetSettingForceALayerOnly (pTunerDemod->pDiverSub, pPresetInfo);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbtsb_SleepSetting (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbtsb_SleepSetting");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* ISDB-Tsb dependent settings. */
    result = X_sleep_isdbtsb_DemodSetting (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = X_sleep_isdbtsb_DemodSetting (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* Disable force preset setting */
    result = sony_tunerdemod_isdbtsb_PresetSettingForceALayerOnly (pTunerDemod, NULL);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* ISDB-T dependent settings. */
    result = sony_tunerdemod_isdbt_SleepSetting (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}
