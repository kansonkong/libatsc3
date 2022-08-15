/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/13
  Modification ID : 7843eb97be7f8a319be245f959bc07bb94cf3bf7
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_common.h"
#include "sony_tunerdemod_monitor.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tunerdemod_monitor_RFLevel (sony_tunerdemod_t * pTunerDemod, int32_t * pRFLeveldB)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_monitor_RFLevel");

    if ((!pTunerDemod) || (!pRFLeveldB)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* OREG_CK_ACTRL_EN = 1 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t data[2] = {0x80, 0x00};

        /* TUNING_CMD */
        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x5B, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    SONY_SLEEP_IN_MONITOR (2, pTunerDemod);

    /* SLV-X bank 0x1A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x1A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t data[2];

        /* CPU_STT */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x15, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if ((data[0] != 0) || (data[1] != 0)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_OTHER);
        }

        /* RSSI */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x11, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        *pRFLeveldB = sony_Convert2SComplement ((data[0] << 3) | ((data[1] & 0xE0) >> 5), 11);
    }

    /* Return value is dB x 1000 */
    /* Register value is dB x 8. So the value should be x 125 */
    *pRFLeveldB *= 125;

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* OREG_CK_ACTRL_EN = 0 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* RF level compensation if necessary. */
    if (pTunerDemod->RFLevelCompensation) {
        result = pTunerDemod->RFLevelCompensation (pTunerDemod, pRFLeveldB);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_monitor_RFLevel_sub (sony_tunerdemod_t * pTunerDemod, int32_t * pRFLeveldB)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_monitor_RFLevel_sub");

    if ((!pTunerDemod) || (!pRFLeveldB)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_monitor_RFLevel (pTunerDemod->pDiverSub, pRFLeveldB);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_monitor_InternalCPUStatus (sony_tunerdemod_t * pTunerDemod,
                                                         uint16_t * pStatus)
{
    uint8_t data[2] = {0};

    SONY_TRACE_ENTER ("sony_tunerdemod_monitor_InternalCPUStatus");

    if ((!pTunerDemod) || (!pStatus)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-X bank 0x1A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x1A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x15, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    *pStatus = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_monitor_InternalCPUStatus_sub (sony_tunerdemod_t * pTunerDemod,
                                                             uint16_t * pStatus)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_monitor_InternalCPUStatus_sub");

    if ((!pTunerDemod) || (!pStatus)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_monitor_InternalCPUStatus (pTunerDemod->pDiverSub, pStatus);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_monitor_TSBufferInfo (sony_tunerdemod_t * pTunerDemod,
                                                    sony_tunerdemod_ts_buffer_info_t * pInfo)
{
    uint8_t data[3] = {0};
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_monitor_TSBufferInfo");

    if ((!pTunerDemod) || (!pInfo)) {
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

    /* SLV-T bank 0x0A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x50, data, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    pInfo->readReady = (uint8_t)((data[0] & 0x10) ? 0x01 : 0x00);
    pInfo->almostFull = (uint8_t)((data[0] & 0x08) ? 0x01 : 0x00);
    pInfo->almostEmpty = (uint8_t)((data[0] & 0x04) ? 0x01 : 0x00);
    pInfo->overflow = (uint8_t)((data[0] & 0x02) ? 0x01 : 0x00);
    pInfo->underflow = (uint8_t)((data[0] & 0x01) ? 0x01 : 0x00);

    pInfo->packetNum = (uint16_t)(((uint32_t)(data[1] & 0x07) << 8) | data[2]);

    SONY_TRACE_RETURN (result);
}
#undef uint32_t 
#undef int32_t 
#undef int8_t 