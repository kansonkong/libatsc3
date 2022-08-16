/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/12/13
  Modification ID : e402a4dcb0e51a00c34b174334c082c1211eebf1
------------------------------------------------------------------------------*/

#include "sony_cxd6801_demod_atsc.h"
#include "sony_cxd6801_demod_atsc_monitor.h"

/*------------------------------------------------------------------------------
  Definitions
------------------------------------------------------------------------------*/
/* Wait time */
#define WAITCOMMANDRESPONSE_POLL_INTERVAL_MS (10)      /**< Wait Response Polling Interval (in ms). */
#define WAITCPUIDLE_TIMEOUT_MS               (1000)    /**< Wait for CPU idle (in ms). */

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/
/**
 @brief Command data for ATSC demodulator core.
*/
typedef struct
{
    uint8_t  data[6];          /**< Command Parameters */
} sony_cxd6801_demod_atsc_cmd_t;

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Configure the demodulator from Sleep to Active for ATSC.
*/
static sony_cxd6801_result_t SLtoAA(sony_cxd6801_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active ATSC to Active ATSC.  Used
        for changing channel parameters.
*/
static sony_cxd6801_result_t AAtoAA(sony_cxd6801_demod_t * pDemod);

/**
 @brief Configure the demodulator from Active ATSC to Sleep.
*/
static sony_cxd6801_result_t AAtoSL(sony_cxd6801_demod_t * pDemod);

/**
 @brief Send Command to ATSC demodulator.
*/
static sony_cxd6801_result_t sendCommand (sony_cxd6801_demod_t *pDemod,
                                  const sony_cxd6801_demod_atsc_cmd_t *pCmd);

/**
 @brief Get Response Data from ATSC demodulator.
*/
static sony_cxd6801_result_t readResponse (sony_cxd6801_demod_t *pDemod,
                                   sony_cxd6801_demod_atsc_cmd_t *pResp,
                                   uint8_t cmdId);

/**
 @brief Demodulation Abort function.
*/
static sony_cxd6801_result_t demodAbort (sony_cxd6801_demod_t * pDemod);

/**
 @brief Continuous writing function.
*/
static sony_cxd6801_result_t slaveRWriteMultiRegisters (sony_cxd6801_demod_t * pDemod,
                                                uint8_t  bank,
                                                uint8_t  registerAddress,
                                                const uint8_t* pData,
                                                uint8_t  size);
/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_Tune (sony_cxd6801_demod_t * pDemod,
                                    sony_cxd6801_atsc_tune_param_t * pTuneParam)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_Tune");

    if ((!pDemod) || (!pTuneParam)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pDemod->state == SONY_CXD6801_DEMOD_STATE_ACTIVE) && (pDemod->system == SONY_CXD6801_DTV_SYSTEM_ATSC)) {
        /* Demodulator Active and set to ATSC mode */
        result = AAtoAA(pDemod);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else if ((pDemod->state == SONY_CXD6801_DEMOD_STATE_ACTIVE) && (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC)) {
        /* Demodulator Active but not ATSC mode */
        result = sony_cxd6801_demod_Sleep(pDemod);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

        pDemod->system = SONY_CXD6801_DTV_SYSTEM_ATSC;
        pDemod->bandwidth = SONY_CXD6801_DTV_BW_6_MHZ;

        result = SLtoAA (pDemod);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else if (pDemod->state == SONY_CXD6801_DEMOD_STATE_SLEEP) {
        /* Demodulator in Sleep mode */
        pDemod->system = SONY_CXD6801_DTV_SYSTEM_ATSC;
        pDemod->bandwidth = SONY_CXD6801_DTV_BW_6_MHZ;

        result = SLtoAA (pDemod);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Update demodulator state */
    pDemod->state = SONY_CXD6801_DEMOD_STATE_ACTIVE;

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_TuneEnd (sony_cxd6801_demod_t * pDemod)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_TuneEnd");

    if (!pDemod) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    result = sony_cxd6801_demod_atsc_SoftReset(pDemod);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Enable TS output */
    result = sony_cxd6801_demod_SetStreamOutput (pDemod, 1);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_SoftReset (sony_cxd6801_demod_t * pDemod)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_cxd6801_demod_atsc_cmd_t  cmd;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_SoftReset");

    if (!pDemod) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Abort Demodulation */
    result = demodAbort (pDemod);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Start FAT Command
     * byte0:COMMANDID_STARTFAT 0xB3
     * byte1:Demod Mode
     *       :8VSB             (0x10)
     *       :64QAM            (0x11)
     *       :256QAM           (0x12)
     *       :AUTO             (0x16)
     *  byte2:Signal Check OFF
     *       :SIGNALCHK_ON     (0x00)
     *       :SIGNALCHK_OFF    (0x04)
     *  byte3:Reserved(0x00)
     *  byte4:Reserved(0x00)
     *  byte5:Reserved(0x00)
     */
    cmd.data[0] = 0xB3;
    cmd.data[1] = 0x10; /* 8VSB Mode */

    if (pDemod->atscAutoSignalCheckOff) {
        cmd.data[2] = 0x04; /* During Scanning Signal Check Off */
    }
    else {
        cmd.data[2] = 0x00; /* During Scanning Signal Check On */
    }

    cmd.data[3] = 0;
    cmd.data[4] = 0;
    cmd.data[5] = 0;

    /* Send Command */
    result = sendCommand (pDemod, &cmd);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Get Response */
    result = readResponse (pDemod, &cmd, 0xB3);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Update Current Internal CPU State */
    pDemod->atscCPUState = SONY_CXD6801_DEMOD_ATSC_CPU_STATE_BUSY;

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_Sleep (sony_cxd6801_demod_t * pDemod)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_Sleep");

    if (!pDemod) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    result = AAtoSL (pDemod);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_CheckDemodLock (sony_cxd6801_demod_t * pDemod,
                                              sony_cxd6801_demod_lock_result_t * pLock)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    uint8_t vqlock = 0;
    uint8_t tslock = 0;
    uint8_t agclock = 0;
    uint8_t unlockDetected = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_CheckDemodLock");

    if (!pDemod || !pLock) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    *pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_NOTDETECT;

    result = sony_cxd6801_demod_atsc_monitor_SyncStat (pDemod, &vqlock, &agclock, &tslock, &unlockDetected);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    if (pDemod->atscUnlockDetection) {
        if (unlockDetected) {
            *pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_UNLOCKED;
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
        }
    } else {
        /* the result of unlockDetect should be ignored */
        unlockDetected = 0;
    }

    if (vqlock) {
        *pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_LOCKED;
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_CheckTSLock (sony_cxd6801_demod_t * pDemod,
                                           sony_cxd6801_demod_lock_result_t * pLock)
{

    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint8_t vqlock = 0;
    uint8_t tslock = 0;
    uint8_t agclock = 0;
    uint8_t unlockDetected = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_CheckTSLock");

    if (!pDemod || !pLock) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    *pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_NOTDETECT;

    result = sony_cxd6801_demod_atsc_monitor_SyncStat (pDemod, &vqlock, &agclock, &tslock, &unlockDetected);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    if (tslock) {
        *pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_LOCKED;
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
    }

    if (pDemod->atscUnlockDetection) {
        if (unlockDetected) {
            *pLock = SONY_CXD6801_DEMOD_LOCK_RESULT_UNLOCKED;
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
        }
    } else {
        /* the result of unlockDetect should be ignored */
        unlockDetected = 0;
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_SlaveRWriteRegister (sony_cxd6801_demod_t * pDemod,
                                                   uint8_t  bank,
                                                   uint8_t  registerAddress,
                                                   uint8_t  value,
                                                   uint8_t  bitMask)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_cxd6801_demod_atsc_cmd_t cmd;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_SlaveRWriteRegister");

    if (!pDemod) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) &&
        (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep or  Active state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->atscCPUState != SONY_CXD6801_DEMOD_ATSC_CPU_STATE_IDLE) {
        /* This api is accepted in CPU Idle State */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /*  Write Register Command
     *  byte0:COMMANDID_WRITE_REG 0xC5
     *  byte1:Bank
     *  byte2:SubAddress
     *  byte3:Write Data
     *  byte4:Mask
     *  byte5:Reserved(0x00)
     */
    cmd.data[0] = 0xC5;
    cmd.data[1] = bank;
    cmd.data[2] = registerAddress;
    cmd.data[3] = value;
    cmd.data[4] = bitMask;
    cmd.data[5] = 0;

    /* Send Command */
    result = sendCommand(pDemod, &cmd);
    if (result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Get Response */
    result = readResponse(pDemod, &cmd, 0xC5);
    if (result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_SlaveRReadRegister (sony_cxd6801_demod_t * pDemod,
                                                  uint8_t bank,
                                                  uint8_t registerAddress,
                                                  uint8_t * pValue)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    sony_cxd6801_demod_atsc_cmd_t cmd;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_SlaveRReadRegister");

    if (!pDemod || !pValue) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) &&
        (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep or  Active state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->atscCPUState != SONY_CXD6801_DEMOD_ATSC_CPU_STATE_IDLE) {
        /* This api is accepted in CPU Idle State */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

     /*
      *  Read Register Command
      *  byte0:COMMANDID_READ_REG 0xC6
      *  byte1:Bank
      *  byte2:SubAddress
      *  byte3:Reserved(0x00)
      *  byte4:Reserved(0x00)
      *  byte5:Reserved(0x00)
      */
    cmd.data[0] = 0xC6;
    cmd.data[1] = bank;
    cmd.data[2] = registerAddress;
    cmd.data[3] = 0;
    cmd.data[4] = 0;
    cmd.data[5] = 0;

    /* Send Command */
    result = sendCommand (pDemod, &cmd);
    if (result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Get Response */
    result = readResponse (pDemod, &cmd, 0xC6);
    if (result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_RETURN (result);
    }
    *pValue = cmd.data[1];

    SONY_CXD6801_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t SLtoAA(sony_cxd6801_demod_t * pDemod)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("SLtoAA");

    if (!pDemod) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Configure TS clock Mode and Frequency */
    result = sony_cxd6801_demod_SetTSClockModeAndFreq (pDemod, SONY_CXD6801_DTV_SYSTEM_ATSC);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set demod mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x0F) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set TS/TLV mode */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Enable demod clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x01) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, 0x74) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x49, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    result = demodAbort (pDemod);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* IF Frequency Setting */
    {
        uint8_t data[4];
        data[0] = (uint8_t) ( pDemod->iffreqConfig.configATSC        & 0xFF);
        data[1] = (uint8_t) ((pDemod->iffreqConfig.configATSC >> 8)  & 0xFF);
        data[2] = (uint8_t) ((pDemod->iffreqConfig.configATSC >> 16) & 0xFF);
        data[3] = (uint8_t) ((pDemod->iffreqConfig.configATSC >> 24) & 0xFF);
        /*
         *  slave     Bank    Addr    Bit     Default    Value          Name
         * ----------------------------------------------------------------------------------
         *  <SLV-R>   A3h     A1h     [7:0]   8'h4F      user define    vsb_DRPSPA_LOENGEN_EXT_FIXED_FREQ[7:0]
         *  <SLV-R>   A3h     A2h     [7:0]   8'hC2      user define    vsb_DRPSPA_LOENGEN_EXT_FIXED_FREQ[15:8]
         *  <SLV-R>   A3h     A3h     [7:0]   8'hF6      user define    vsb_DRPSPA_LOENGEN_EXT_FIXED_FREQ[23:16]
         *  <SLV-R>   A3h     A4h     [7:0]   8'h24      user define    vsb_DRPSPA_LOENGEN_EXT_FIXED_FREQ[31:24]
         */
        result = slaveRWriteMultiRegisters (pDemod, 0xA3, 0xA1, data, sizeof(data));
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }
    /* ASCOT Setting */
    if (pDemod->tunerOptimize == SONY_CXD6801_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
        /* ASCOT setting ON
         *  slave     Bank    Addr    Bit     Default    Value    Name
         * ---------------------------------------------------------------------------------
         *  <SLV-R>   A0h     13h     [1:0]   8'h03      8'h03    vsb_gdeq_on
         */
        result = sony_cxd6801_demod_atsc_SlaveRWriteRegister (pDemod, 0xA0, 0x13, 0x03, 0x03);
        if (result != SONY_CXD6801_RESULT_OK){
            SONY_CXD6801_TRACE_RETURN (result);
        }

    } else {
        /* ASCOT setting OFF
         *  slave     Bank    Addr    Bit      Default   Value    Name
         * ----------------------------------------------------------------------------------
         *  <SLV-R>   A0h     13h     [1:0]    8'h03     8'h00    vsb_gdeq_on
         */
        result = sony_cxd6801_demod_atsc_SlaveRWriteRegister (pDemod, 0xA0, 0x13, 0x00, 0x03);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    if (pDemod->tunerOptimize == SONY_CXD6801_DEMOD_TUNER_OPTIMIZE_SONYSILICON) {
        /* Filter Setting
         *  slave     Bank    Addr    Bit     Default   Value    Name
         * --------------------------------------------------------------------------------
         *  <SLV-R>   06h     A0h     [7:0]   8'h31     8'h31    OHI_GDEQ_COEF01
         *  <SLV-R>   06h     A1h     [7:0]   8'hA5     8'hA5    OHI_GDEQ_COEF02
         *  <SLV-R>   06h     A2h     [7:0]   8'h2E     8'h2E    OHI_GDEQ_COEF11
         *  <SLV-R>   06h     A3h     [7:0]   8'h9F     8'h9F    OHI_GDEQ_COEF12
         *  <SLV-R>   06h     A4h     [7:0]   8'h2B     8'h2B    OHI_GDEQ_COEF21
         *  <SLV-R>   06h     A5h     [7:0]   8'h99     8'h99    OHI_GDEQ_COEF22
         *  <SLV-R>   06h     A6h     [7:0]   8'h00     8'h00    OHI_GDEQ_COEF31
         *  <SLV-R>   06h     A7h     [7:0]   8'hCD     8'hCD    OHI_GDEQ_COEF32
         *  <SLV-R>   06h     A8h     [7:0]   8'h00     8'h00    OHI_GDEQ_COEF41
         *  <SLV-R>   06h     A9h     [7:0]   8'hCD     8'hCD    OHI_GDEQ_COEF42
         *  <SLV-R>   06h     AAh     [7:0]   8'h00     8'h00    OHI_GDEQ_COEF51
         *  <SLV-R>   06h     ABh     [7:0]   8'h00     8'h00    OHI_GDEQ_COEF52
         *  <SLV-R>   06h     ACh     [7:0]   8'h2B     8'h2B    OHI_GDEQ_COEF61
         *  <SLV-R>   06h     ADh     [7:0]   8'h9D     8'h9D    OHI_GDEQ_COEF62
         */
        const uint8_t data[] = { 0x31, 0xA5, 0x2E, 0x9F, 0x2B, 0x99, 0x00, 0xCD, 0x00, 0xCD, 0x00, 0x00, 0x2B, 0x9D };
        result = slaveRWriteMultiRegisters (pDemod, 0x06, 0xA0, data, sizeof(data));
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    /* ATSC1.0 initial setting */
    {
        uint8_t noSignalThresh[3];
        uint8_t signalThresh[3];
        /*
         *  slave     Bank    Addr    Bit     Default   Value    Name
         * -----------------------------------------------------------------------
         *  <SLV-R>   A0h     6Fh     [7:0]   8'h37     8'h61    dagca_no_signal_thresh[7:0]
         *  <SLV-R>   A0h     70h     [7:0]   8'h2e     8'hFB    dagca_no_signal_thresh[15:8]
         *  <SLV-R>   A0h     71h     [7:0]   8'h66     8'h7F    dagca_no_signal_thresh[23:16]
         *  <SLV-R>   09h     80h     [7:0]   8'hAE     8'h61    OHI_DAGCA_NOSIG_THRESH[7:0]
         *  <SLV-R>   09h     81h     [7:0]   8'h37     8'hFB    OHI_DAGCA_NOSIG_THRESH[15:8]
         *  <SLV-R>   09h     82h     [7:0]   8'h61     8'h7F    OHI_DAGCA_NOSIG_THRESH[23:16]
         *  <SLV-R>   A0h     73h     [7:0]   8'hFC     8'h26    dagca_signal_thresh[7:0]
         *  <SLV-R>   A0h     74h     [7:0]   8'hA2     8'h49    dagca_signal_thresh[15:8]
         *  <SLV-R>   A0h     75h     [7:0]   8'h5C     8'h7C    dagca_signal_thresh[23:16]
         *  <SLV-R>   09h     83h     [7:0]   8'hAE     8'h26    OHI_DAGCA_SIG_THRESH[7:0]
         *  <SLV-R>   09h     84h     [7:0]   8'h27     8'h49    OHI_DAGCA_SIG_THRESH[15:8]
         *  <SLV-R>   09h     85h     [7:0]   8'h61     8'h7C    OHI_DAGCA_SIG_THRESH[23:16]
         *  <SLV-R>   06h     71h     [2:0]   8'h00     8'h05    OHI_CONST_SEL[2:0]
         */
        noSignalThresh[0] = (uint8_t)(pDemod->atscNoSignalThresh & 0xFF);
        noSignalThresh[1] = (uint8_t)((pDemod->atscNoSignalThresh >> 8) & 0xFF);
        noSignalThresh[2] = (uint8_t)((pDemod->atscNoSignalThresh >> 16) & 0xFF);
        signalThresh[0] = (uint8_t)(pDemod->atscSignalThresh & 0xFF);
        signalThresh[1] = (uint8_t)((pDemod->atscSignalThresh >> 8) & 0xFF);
        signalThresh[2] = (uint8_t)((pDemod->atscSignalThresh >> 16) & 0xFF);
        result = slaveRWriteMultiRegisters (pDemod, 0xA0, 0x6F, noSignalThresh, sizeof(noSignalThresh));
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        result = slaveRWriteMultiRegisters (pDemod, 0x09, 0x80, noSignalThresh, sizeof(noSignalThresh));
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        result = slaveRWriteMultiRegisters (pDemod, 0xA0, 0x73, signalThresh, sizeof(signalThresh));
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        result = slaveRWriteMultiRegisters (pDemod, 0x09, 0x83, signalThresh, sizeof(signalThresh));
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
        result = sony_cxd6801_demod_atsc_SlaveRWriteRegister (pDemod, 0x06, 0x71, 0x05, 0x07);
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }
    /* Spectrum Inversion Setting
     * RF signal spectrum | tuner spectrum sense     | Setting
     *                    | (pDemod->confSense)      | (from Demod View)
     * -------------------+--------------------------+---------
     *  Normal            |  not same as RF(Normal)  |  1B (Normal)
     *  Invert            |  not same as RF(Normal)  |  0B (Invert)
     *  Normal            |  same as RF(Invert)      |  0B (Invert)
     *  Invert            |  same as RF(Invert)      |  1B (Normal)

     *  slave     Bank    Addr    Bit     Default   Value    Name
     *------------------------------------------------------------------------------------------
     *  <SLV-R>   A3h     A0h     [4]     8'h1B     8'h**    vsb_DRPSPA_LOENGEN_POSSHIFT
     */


    if (pDemod->rfSpectrumSense == pDemod->confSense){
        result = sony_cxd6801_demod_atsc_SlaveRWriteRegister (pDemod, 0xA3, 0xA0, 0x10, 0x10);
        if (result != SONY_CXD6801_RESULT_OK){
            SONY_CXD6801_TRACE_RETURN (result);
        }
    } else {
        result = sony_cxd6801_demod_atsc_SlaveRWriteRegister (pDemod, 0xA3, 0xA0, 0x00, 0x10);
        if (result != SONY_CXD6801_RESULT_OK){
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Disable HiZ Setting 1 (TAGC, SAGC(Hi-Z), TSVALID, TSSYNC, TSCLK) */
    if (sony_cxd6801_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x08, 0x1F) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Disable HiZ Setting 2 */
    result = sony_cxd6801_demod_SetTSDataPinHiZ (pDemod, 0);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

static sony_cxd6801_result_t AAtoAA(sony_cxd6801_demod_t * pDemod)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("AAtoAA");

    if (!pDemod) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Disable TS output */
    result = sony_cxd6801_demod_SetStreamOutput (pDemod, 0);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t AAtoSL(sony_cxd6801_demod_t * pDemod)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    SONY_CXD6801_TRACE_ENTER ("AAtoSL");

    if (!pDemod) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Disable TS output */
    result = sony_cxd6801_demod_SetStreamOutput (pDemod, 0);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Enable Hi-Z setting 1 (TAGC, SAGC, TSVALID, TSSYNC, TSCLK) */
    if (sony_cxd6801_i2c_SetRegisterBits (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, 0x1F, 0x1F) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Enable Hi-Z setting 2 */
    result = sony_cxd6801_demod_SetTSDataPinHiZ (pDemod, 1);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    result = demodAbort (pDemod);
    if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Cancel ATSC1 setting
     *  slave     Bank    Addr    Bit     Default    Value    Name
     * ----------------------------------------------------------------------------------
     *  <SLV-T>   00h     D3h     [0]     8'h00      8'h00    OTSRATECTRLOFF
     */

    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xD3, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x18, 0x01) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x49, 0x33) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* TADC setting */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4B, 0x21) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Disable demodulator clock */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x2C, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set tstlv mode to default */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA9, 0x00) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set demodulator mode to default */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x17, 0x01) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t sendCommand (sony_cxd6801_demod_t *pDemod, const sony_cxd6801_demod_atsc_cmd_t *pCmd)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("sendCommand");

    if ((!pDemod) || (!pDemod->pI2c) || (!pCmd)){
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Write data to 0x0A register */
    result = pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVR, 0x0A, pCmd->data, sizeof(pCmd->data));
    if (result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

static sony_cxd6801_result_t readResponse (sony_cxd6801_demod_t *pDemod, sony_cxd6801_demod_atsc_cmd_t *pResp, uint8_t cmdId)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint32_t waitTime = 0;

    SONY_CXD6801_TRACE_ENTER ("readResponse");

    if ((!pDemod) || (!pDemod->pI2c) || (!pResp)){
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }
    for (;;) {
        /* Read data to 0x0A register */
        result = pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVR, 0x0A, pResp->data, sizeof(pResp->data));
        if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }

        if (pResp->data[0] == 0x00) {
            /* Waiting... */
            SONY_CXD6801_SLEEP (WAITCOMMANDRESPONSE_POLL_INTERVAL_MS);
            waitTime += WAITCOMMANDRESPONSE_POLL_INTERVAL_MS;
        }
        else {
            if (pResp->data[5] == cmdId) {
                break;
            }
            else {
                SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
            }
        }

        if (waitTime >= WAITCPUIDLE_TIMEOUT_MS) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_TIMEOUT);
        }
    }

    /* Check Response Status */
    if ((pResp->data[0] & 0x3F) == 0x30) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
    }
    else {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_OTHER);
    }

}

static sony_cxd6801_result_t slaveRWriteMultiRegisters (sony_cxd6801_demod_t * pDemod,
                                                uint8_t  bank,
                                                uint8_t  registerAddress,
                                                const uint8_t* pData,
                                                uint8_t  size)
{
    uint8_t wtimes = 0;
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("slaveRWriteMultiRegisters");

    if (!pDemod || !pData ) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    for (wtimes = 0; wtimes < size; wtimes++) {
        result = sony_cxd6801_demod_atsc_SlaveRWriteRegister (pDemod, bank, registerAddress + wtimes, *(pData + wtimes), 0xFF);
        if (result != SONY_CXD6801_RESULT_OK){
            SONY_CXD6801_TRACE_RETURN (result);
        }
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t demodAbort (sony_cxd6801_demod_t * pDemod)
{

    if (!pDemod){
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    SONY_CXD6801_TRACE_ENTER ("demodAbort");

    /* Set SLV-R Bank : 0x01 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVR, 0x00, 0x01) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /* Abort Current Demodulation
     *  slave     Bank    Addr    Bit    Value     Name
     * -------------------------------------------------------------------------------------
     *  <SLV-R>   01h     48h     [0]    1'b1      OCPUWAKEUP
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVR, 0x48, 0x01) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Update Current Internal CPU State */
    pDemod->atscCPUState = SONY_CXD6801_DEMOD_ATSC_CPU_STATE_IDLE;

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}
