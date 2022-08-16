/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/12/13
  Modification ID : e402a4dcb0e51a00c34b174334c082c1211eebf1
------------------------------------------------------------------------------*/

#include "sony_cxd6801_demod_atsc_monitor.h"
#include "sony_cxd6801_math.h"
#include "sony_cxd6801_stdlib.h"

/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t checkVQLock (sony_cxd6801_demod_t * pDemod);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_SyncStat (sony_cxd6801_demod_t * pDemod,
                                                uint8_t * pVQLockStat,
                                                uint8_t * pAGCLockStat,
                                                uint8_t * pTSLockStat,
                                                uint8_t * pUnlockDetected)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_monitor_SyncStat");

    if ((!pDemod) || (!pVQLockStat) || (!pAGCLockStat) || (!pTSLockStat) || (!pUnlockDetected)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data;

        /* Set SLV-M Bank : 0x0F */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x0F) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        /* TS lock flag
         *  slave    Bank    Addr    Bit    Name            Meaning
         * -----------------------------------------------------------------------
         *  <SLV-M>   0Fh    11h     [0]    TSLOCK          0:UNLOCK, 1:LOCK
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x11, &data, 1) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        *pTSLockStat = (uint8_t)(data & 0x01);

        /* Set SLV-M Bank : 0x09 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x09) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        /* AGC lock flag / Early Unlock Flag
         *  slave     Bank   Addr    Bit    Name              Meaning
         * -----------------------------------------------------------------------
         *  <SLV-M>   09h    62h     [4]    DAGCA_LOCK        0:UNLOCK, 1:LOCK
         *  <SLV-M>   09h    62h     [6]    ISIGNAL_PRESENCE  0:not present, 1:present
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x62, &data, 1) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        *pAGCLockStat = (uint8_t)((data & 0x10) ? 1 : 0);
        *pUnlockDetected = (uint8_t)((data & 0x40) ? 0 : 1);

        /* Set SLV-M Bank : 0x0D */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x0D) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        /* DMD lock flag
         *  slave     Bank    Addr    Bit    Name            Meaning
         * -----------------------------------------------------------------------
         *  <SLV-M>   0Dh     86h     [0]    VQLOCK          0:UNLOCK, 1:LOCK
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x86, &data, 1) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        *pVQLockStat = (uint8_t)(data & 0x01);
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_IFAGCOut (sony_cxd6801_demod_t * pDemod,
                                                uint32_t * pIFAGCOut)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_monitor_IFAGCOut");

    if ((!pDemod) || (!pIFAGCOut)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-M Bank : 0x09 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x09) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* IFAGC Output
     *  slave     Bank    Addr    Bit         Name
     * ------------------------------------------------------------
     *  <SLV-M>   09h     63h     [7:0]       DAGCA_IFMON[7:0]
     *  <SLV-M>   09h     64h     [5:0]       DAGCA_IFMON[13:8]
     */
    {
        uint8_t data[2];
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x63, data, 2) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }

        *pIFAGCOut = ((uint32_t)(data[1] & 0x3F) << 8) | data[0];
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_CarrierOffset (sony_cxd6801_demod_t * pDemod,
                                                     int32_t * pOffset)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_monitor_CarrierOffset");

    if ((!pDemod) || (!pOffset)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* VQ Lock Check */
    if (checkVQLock (pDemod) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /* Set SLV-M Bank : 0x06 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x06) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    {
        uint8_t  data[3];
        int32_t  freqError[2];
        uint32_t ctlVal = 0;

        /*  slave     Bank    Addr    Bit      Name
         * ------------------------------------------------------------
         *  <SLV-M>   06h     F8h     [7:0]    dpt_lfacc[7:0]
         *  <SLV-M>   06h     F9h     [7:0]    dpt_lfacc[15:8]
         *  <SLV-M>   06h     FAh     [3:0]    dpt_lfacc[19:16]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0xF8, data, 3) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        ctlVal = ((uint32_t)(data[2] & 0x0F) << 16) | ((uint32_t)data[1] << 8) | data[0];
        freqError[0] = sony_cxd6801_Convert2SComplement (ctlVal, 20); /* dpt_lfacc values reported in two's complement format */
        /*  8VSB:Frequency Error[0]  [Hz] = dpt_lfacc * (2^-22) * 10.762238 * 10^6
         *                                = dpt_lfacc * 2.565917492
         *                                = dpt_lfacc * 1155 / 450
         */
        freqError[0] *= 1155;

        /* Set SLV-M Bank : 0x07 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x07) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        /*  slave     Bank    Addr    Bit      Name
         * ------------------------------------------------------------
         *  <SLV-M>   07h     30h     [7:0]    dpt_ext_lfacc[7:0]
         *  <SLV-M>   07h     31h     [7:0]    dpt_ext_lfacc[15:8]
         *  <SLV-M>   07h     32h     [3:0]    dpt_ext_lfacc[19:16]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x30, data, 3) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        ctlVal = ((uint32_t)(data[2] & 0x0F) << 16) | ((uint32_t)data[1] << 8) | data[0];
        freqError[1] = sony_cxd6801_Convert2SComplement (ctlVal, 20); /* dpt_ext_lfacc values reported in two's complement format */

        /* Frequency Error[1] [Hz] = dpt_ext_lfacc * (2^-23) * 48 * 10^6
         *                         = dpt_ext_lfacc * 5.7220458984375
         *                         = dpt_ext_lfacc * 3125 / 546
         */
        freqError[1] *= 3125;

        if ((freqError[0] + freqError[1]) >= 0) {
            *pOffset = - ((freqError[0] + freqError[1] + 273) / 546);
        } else {
            *pOffset = - ((freqError[0] + freqError[1] - 273) / 546);
        }

        /* Compensate for inverted spectrum. */
        if (pDemod->rfSpectrumSense == SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_INV) {
            *pOffset *= -1;
        }
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_PreRSBER (sony_cxd6801_demod_t * pDemod,
                                                uint32_t * pBER)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_monitor_PreRSBER");

    if ((!pDemod) || (!pBER)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* VQ Lock Check */
    if (checkVQLock (pDemod) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /* Set SLV-M Bank : 0x0D */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x0D) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    {
        uint8_t  data[3];
        uint32_t bitError = 0;
        uint32_t blocks = 0;
        uint16_t bitPerBlock = 0;
        uint8_t  ecntEn = 0;
        uint8_t  overFlow = 1;

        /* Before RS BER Monitor
         *  slave     Bank    Addr    Bit      Name
         * ------------------------------------------------------------
         *  <SLV-M>   0Dh     90h     [7:0]    RSERRA_BIERR[7:0]
         *  <SLV-M>   0Dh     91h     [7:0]    RSERRA_BIERR[15:8]
         *  <SLV-M>   0Dh     92h     [4:0]    RSERRA_BIERR[20:16]
         *  <SLV-M>   0Dh     92h     [5]      RSERRA_ECNTEN
         *  <SLV-M>   0Dh     92h     [6]      RSERRA_OVF
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x90, data, 3) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        ecntEn =   (uint8_t)((data[2] & 0x20) ? 1 : 0);
        overFlow = (uint8_t)((data[2] & 0x40) ? 1 : 0);
        bitError =((uint32_t)(data[2] & 0x1F) << 16) | ((uint32_t)(data[1] & 0xFF) << 8) | data[0];

        if (overFlow){
            *pBER = 10000000;
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
        }
        if(!ecntEn){
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }

        /* Set SLV-M Bank : 0x0D */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x0D) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* 8VSB: 1 Block = 207 Symbol * 8 Bit/Symbol */
        bitPerBlock = 207 * 8;

        /* 8VSB Measurement Block Length
         *  slave     Bank    Addr    Bit      Name
         * ------------------------------------------------------------
         *  <SLV-M>   0Dh     9Ah     [7:0]    IRSERRA_BKLEN[7:0]
         *  <SLV-M>   0Dh     9Bh     [7:0]    IRSERRA_BKLEN[15:8]
         *  <SLV-M>   0Dh     9Ch     [7:0]    IRSERRA_BKLEN[23:16]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x9A, data, 3) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        blocks = ((uint32_t)(data[2] & 0xFF) << 16) | ((uint32_t)(data[1] & 0xFF) << 8) | data[0] ;

        /* Check value of the blocks is valid */
        if ((blocks == 0) || (blocks < (bitError / bitPerBlock))){
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }

        /* Case value of the blocks is not so large */
        if (blocks < 0xFFFFF){
             /*
              * BER = bitError * 10000000  / (blocks * bitPerBlock)
              *     = bitError * 1000 * 5 * 5 * 5 * 10 /(blocks * (bitPerBlock / 8))
              *    (Divide in 5 steps to prevent overflow.)
              */
            uint32_t Q = 0;
            uint32_t R = 0;
            uint32_t Div = blocks * (bitPerBlock / 8);

            Q = (bitError  * 1000) / Div;
            R = (bitError  * 1000) % Div;
            R *= 5;
            Q = (Q * 5) + (R / Div);
            R = R % Div;
            R *= 5;
            Q = (Q * 5) + (R / Div);
            R = R % Div;
            R *= 5;
            Q = (Q * 5) + (R / Div);
            R = R % Div;
            R *= 10;
            Q = (Q * 10) + (R / Div);
            R = R % Div;
            /* rounding */
            if (R  <= (Div/2)) {
                *pBER = Q;
            }
            else {
                *pBER = Q + 1;
            }
        }
        else {
           /*
            * BER = bitError * 10000000 / (blocks * bitPerBlock)
            *     = bitError * 625 * 5 * 5 * 5 /((blocks * (bitPerBlock / 8)) / 16)
            *       (Divide in 4 steps to prevent overflow.)
            */
            uint32_t Q = 0;
            uint32_t R = 0;
            uint32_t Div = (blocks * (bitPerBlock / 8)) / 16;

            Q = (bitError  * 625) / Div;
            R = (bitError  * 625) % Div;
            R *= 5;
            Q = (Q * 5) + (R / Div);
            R = R % Div;
            R *= 5;
            Q = (Q * 5) + (R / Div);
            R = R % Div;
            R *= 5;
            Q = (Q * 5) + (R / Div);
            R = R % Div;
            /* rounding */
            if (R  <= (Div/2)) {
                *pBER = Q;
            }
            else {
                *pBER = Q + 1;
            }
        }
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_PreRSSER (sony_cxd6801_demod_t * pDemod,
                                                uint32_t * pSER)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_monitor_PreRSSER");

    if ((!pDemod) || (!pSER)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* VQ Lock Check */
    if (checkVQLock (pDemod) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /* Set SLV-M Bank : 0x0D */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x0D) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    {
        uint8_t  data[3];
        uint32_t symbolError = 0;
        uint32_t symbolPerBlock = 0;
        uint32_t blocks = 0;
        uint8_t  ecntEn = 0;
        uint8_t  overFlow = 1;

        /* Before RS SER Monitor
         *  slave     Bank    Addr    Bit      Name
         * ------------------------------------------------------------
         *  <SLV-M>   0Dh     93h     [7:0]    RSERRA_SYERR[7:0]
         *  <SLV-M>   0Dh     94h     [7:0]    RSERRA_SYERR[15:8]
         *  <SLV-M>   0Dh     95h     [1:0]    RSERRA_SYERR[17:16]
         *  <SLV-M>   0Dh     95h     [2]      RSERRA_ECNTEN
         *  <SLV-M>   0Dh     95h     [3]      RSERRA_OVF
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x93, data, 3) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }

        ecntEn      =  (uint8_t) ((data[2] & 0x04) ? 1 : 0);
        overFlow    =  (uint8_t) ((data[2] & 0x08) ? 1 : 0);
        symbolError =  ((uint32_t)(data[2] & 0x03) << 16) | ((uint32_t)(data[1] & 0xFF) << 8) | data[0];

        if (overFlow){
            *pSER = 10000000;
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
        }
        if (!ecntEn){
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }

        /* 8VSB: 1 Block = 207 Symbol */
        symbolPerBlock = 207;

        /* 8VSB Measurement Block Length
         *  slave     Bank    Addr    Bit      Name
         * ------------------------------------------------------------
         *  <SLV-M>   0Dh     9Ah     [7:0]    IRSERRA_BKLEN[7:0]
         *  <SLV-M>   0Dh     9Bh     [7:0]    IRSERRA_BKLEN[15:8]
         *  <SLV-M>   0Dh     9Ch     [7:0]    IRSERRA_BKLEN[23:16]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x9A, data, 3) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
        blocks = ((uint32_t)(data[2] & 0xFF) << 16) | ((uint32_t)(data[1] & 0xFF) << 8) | data[0];

        /* Check value of the blocks is valid */
        if ((blocks == 0) || (blocks < (symbolError / symbolPerBlock))){
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }

        /* Case value of the blocks is not so large */
        if (blocks < 0xFFFFF){
            /*
             * SER = symbolError * 10000000  / (blocks * symbolPerBlock)
             *     = symbolError * 10000 * 10 * 10 * 10  / (blocks * symbolPerBlock)
             *       (Divide in 4 steps to prevent overflow.)
             */
            uint32_t Q = 0;
            uint32_t R = 0;
            uint32_t Div =  blocks * symbolPerBlock;

            Q = (symbolError * 10000) / Div;
            R = (symbolError * 10000) % Div;
            R *= 10;
            Q = (Q * 10) + (R / Div);
            R = R % Div;
            R *= 10;
            Q = (Q * 10) + (R / Div);
            R = R % Div;
            R *= 10;
            Q = (Q * 10) + (R / Div);
            R = R % Div;
            /* rounding */
            if (R  <= (Div/2)) {
                *pSER = Q;
            }
            else {
                *pSER = Q + 1;
            }
        }
        else {
            /*
             * SER = symbolError * 10000000  / (blocks * symbolPerBlock)
             *     = symbolError * 10000 * 5 * 5 * 5 / (blocks * symbolPerBlock / 8)
             *       (Divide in 4 steps to prevent overflow.)
             */
            uint32_t Q = 0;
            uint32_t R = 0;
            uint32_t Div = (blocks * symbolPerBlock) / 8;

            Q = (symbolError * 10000) / Div;
            R = (symbolError * 10000) % Div;
            R *= 5;
            Q = (Q * 5) + (R / Div);
            R = R % Div;
            R *= 5;
            Q = (Q * 5) + (R / Div);
            R = R % Div;
            R *= 5;
            Q = (Q * 5) + (R / Div);
            R = R % Div;
            /* rounding */
            if (R  <= (Div/2)) {
                *pSER = Q;
            }
            else {
                *pSER = Q + 1;
            }
        }
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_PostRSWER (sony_cxd6801_demod_t * pDemod,
                                                 uint32_t * pWER)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_monitor_PostRSWER");

    if ((!pDemod) || (!pWER)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* VQ Lock Check */
    if (checkVQLock (pDemod) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /* Set SLV-M Bank : 0x0D */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x0D) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    {
        uint8_t  data[3];
        uint32_t wordError = 0;
        uint32_t blocks = 0;
        uint8_t  ecntEn = 0;
        uint8_t  overFlow = 1;

        /* After RS WER Monitor
         *  slave     Bank    Addr    Bit      Name
         * ------------------------------------------------------------
         *  <SLV-M>   0Dh     96h     [7:0]    RSERRA_BKERR[7:0]
         *  <SLV-M>   0Dh     97h     [5:0]    RSERRA_BKERR[13:8]
         *  <SLV-M>   0Dh     97h     [6]      RSERRA_ECNTEN
         *  <SLV-M>   0Dh     97h     [7]      RSERRA_OVF
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x96, data, 2) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }

        ecntEn    =  (uint8_t)((data[1] & 0x40) ? 1 : 0);
        overFlow  =  (uint8_t)((data[1] & 0x80) ? 1 : 0);
        wordError = ((uint32_t)(data[1] & 0x3F) << 8) | data[0];

        if (overFlow){
            *pWER = 1000000;
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
        }
        if (!ecntEn){
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }


        /* 8VSB Measurement Block Length
         *  slave     Bank    Addr    Bit      Name
         * ------------------------------------------------------------
         *  <SLV-M>   0Dh     9Ah     [7:0]    IRSERRA_BKLEN[7:0]
         *  <SLV-M>   0Dh     9Bh     [7:0]    IRSERRA_BKLEN[15:8]
         *  <SLV-M>   0Dh     9Ch     [7:0]    IRSERRA_BKLEN[23:16]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x9A, data, 3) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }

        blocks = ((uint32_t)(data[2] & 0xFF) << 16) | ((uint32_t)(data[1] & 0xFF) << 8) | data[0];

        /* Check value of the blocks is valid */
        if (( blocks == 0) || (blocks < wordError)){
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }

        /* Case value of the blocks is not so large */
        if (blocks < 0xFFF ){
            *pWER = wordError * 1000000 / blocks; /* wordError must be smaller than blocks, then this calcuation will not be overflow */
        }
        else {
            /*
             * WER = wordError * 1000000  / blocks
             *     = wordError * 100000 * 10 / blocks
             *       (Divide in 2 steps to prevent overflow.)
             */
            {
                uint32_t Q = 0;
                uint32_t R = 0;
                uint32_t Div = blocks;

                Q = (wordError * 100000) / Div;
                R = (wordError * 100000) % Div;
                R *= 10;
                Q = (Q * 10) + (R / Div);
                R = R % Div;
                /* rounding */
                if (R  <= (Div/2)) {
                    *pWER = Q;
                }
                else {
                    *pWER = Q + 1;
                }
            }
        }
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_PacketError (sony_cxd6801_demod_t * pDemod,
                                                   uint8_t * pPacketErrorDetected)
{
    uint8_t  data = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_monitor_PacketError");

    if ((!pDemod) || (!pPacketErrorDetected)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* VQ Lock Check */
    if (checkVQLock (pDemod) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /* Set SLV-M Bank : 0x0D */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x0D) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Packet Error Monitor
     *  slave     Bank    Addr    Bit      Name
     * ------------------------------------------------------------
     *  <SLV-M>   0Dh     82h     [0]      IERR
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x82, &data, 1) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    *pPacketErrorDetected = (uint8_t)((data & 0x01) ? 1 : 0);
    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_SNR (sony_cxd6801_demod_t * pDemod,
                                           int32_t * pSNR)
{
    uint8_t  data[3];
    uint32_t dcl_avgerr_fine;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_monitor_SNR");

    if ((!pDemod) || (!pSNR)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* VQ Lock Check */
    if (checkVQLock (pDemod) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /* Set SLV-M Bank : 0x0D */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x0D) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /* SNR Monitor
     *  slave     Bank    Addr    Bit      Name
     * -------------------------------------------------
     *  <SLV-M>   0Dh     70h     [7:0]    dcl_avgerr_fine[7:0]
     *  <SLV-M>   0Dh     71h     [7:0]    dcl_avgerr_fine[15:8]
     *  <SLV-M>   0Dh     72h     [6:0]    dcl_avgerr_fine[22:16]
     */

    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x70, data, 3) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    dcl_avgerr_fine = ((uint32_t)(data[2] & 0x7F) << 16) | ((uint32_t)(data[1] & 0xFF) << 8) | data[0];

    /*  sony_cxd6801_log10 returns log10(x) * 100
     *  8VSB:  SNR[mdB] = 1000  * 10(log10( 21*8*8 / (dcl_avgerr_fine * 2^-13) ))
     *                  = 10000 * log10( 21*2^19 / dcl_avgerr_fine )
     *                  = 10000 * ( log10(21) + 19log10(2) -log10(dcl_avgerr_fine))
     *                  = 70418 - 100 * sony_cxd6801_log10(dcl_avgerr_fine)
     */

    *pSNR = (int32_t)(70418 - (100 * sony_cxd6801_math_log10(dcl_avgerr_fine)));

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_SamplingOffset (sony_cxd6801_demod_t * pDemod,
                                                      int32_t * pPPM)
{
    uint8_t   data[6];
    uint32_t  dtrLfaccHigh;
    uint32_t  dtrLfaccLow;
    int32_t   dtrlfacc;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_monitor_SamplingOffset");

    if ((!pDemod) || (!pPPM)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* VQ Lock Check */
    if (checkVQLock (pDemod) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /* Set SLV-M Bank : 0x06 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x06) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Symbol Rate
     *  slave     Bank    Addr    Bit      Name
     * -------------------------------------------------
     *  <SLV-M>   06h     17h     [7:0]    dtr_lfacc[7:0]
     *  <SLV-M>   06h     18h     [7:0]    dtr_lfacc[15:8]
     *  <SLV-M>   06h     19h     [7:0]    dtr_lfacc[23:16]
     *  <SLV-M>   06h     1Ah     [7:0]    dtr_lfacc[31:24]
     *  <SLV-M>   06h     1Bh     [7:0]    dtr_lfacc[39:32]
     *  <SLV-M>   06h     1Ch     [3:0]    dtr_lfacc[43:40]
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x17, data, 6) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    dtrLfaccLow =  ((uint32_t)(data[3] & 0xFF) << 24) | ((uint32_t)(data[2] & 0xFF) << 16) | ((uint32_t)(data[1] & 0xFF) << 8) | data[0];
    dtrLfaccHigh = ((uint32_t)(data[5] & 0x0F) << 8)  | data[4];


    /* Symbol Rate Detector Result can be calculated with the following formula.
     *  Detected SymbolRate[sps] = 48 / 2 / {2 * {dtr_lfacc/(2^43) + 1}} * 10^6
     *                           =(4800 * 10^4 * 2^41) / (dtr_lfacc + 2^43)
     *                           18bit Shift
     *                           ->(4800 * 10^4 * 2^23) / (dtr_lfacc/2^18 + 2^25)
     */
    /* Round off LSB 18 bit and 2's Complement */
    dtrlfacc = sony_cxd6801_Convert2SComplement (((dtrLfaccHigh & 0xFFF) << 14)  | (dtrLfaccLow >> 18), 26);
    if (dtrlfacc < 0){
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    {
        uint32_t Q = 0;
        uint32_t R = 0;
        uint32_t Div = 0;
        int32_t  symbolRateOffset = 0;
        int8_t   flgNegative = 1;

        /* 8VSB: Detected SymbolRate[sps] = (4800 * 10^4 * 2^23) / (dtr_lfacc/2^18 + 2^25)
         *                                 = (4800 * 10000 * 64) * 64 * 64 * 32 / (dtr_lfacc/2^18 + 0x2000000)
         *                                 (Divide in 4 steps to prevent overflow.)
         */
        Div = ((uint32_t)dtrlfacc + 0x2000000);
        Q = (4800U * 10000U * 64U) / Div;
        R = (4800U * 10000U * 64U) % Div;
        R *= 64;
        Q = (Q * 64) + (R / Div);
        R = R % Div;
        R *= 64;
        Q = (Q * 64) + (R / Div);
        R = R % Div;
        R *= 32;
        Q = (Q * 32) + (R / Div);
        R = R % Div;
        /* rounding and comparing with the standard symbol rate
         * 8VSB: SymbolRateOffset[sps] = (10.762238 * 10^6 - Detected Symbol Rate)
         */
        if (R  <= (Div/2)) {
            symbolRateOffset = (int32_t)(10762238 - Q);
        }
         else {
            symbolRateOffset = (int32_t)(10762238 - (Q + 1));
        }
        /* Negative Value Check */
        if (symbolRateOffset < 0) {
            flgNegative = -1;
            symbolRateOffset *= flgNegative;
        }
        /* SamplingOffset[ppm]  = Symbol Rate Offset / 10.762238
         * SamplingOffset * 100 = Symbol Rate Offset * 100 / 10.762238
         *                      = Symbol Rate Offset * 10^8 / 10762238
         *                      = Symbol Rate Offset * 500 * 500 * 200 / (10762238 / 2)
         *                       (Divide in 4 steps to prevent overflow.)
         */
        Div = 5381119;
        Q = (uint32_t)symbolRateOffset / Div;
        R = (uint32_t)symbolRateOffset % Div;
        R *= 500;
        Q = (Q * 500) + (R / Div);
        R = R % Div;
        R *= 500;
        Q = (Q * 500) + (R / Div);
        R = R % Div;
        R *= 200;
        Q = (Q * 200) + (R / Div);
        R = R % Div;
        if (R  <= (Div/2)) {
            *pPPM = flgNegative * (int32_t) Q;
        } else {
            *pPPM = flgNegative * (int32_t)(Q + 1);
        }
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_SignalLevelData_ForUnlockOptimization (sony_cxd6801_demod_t * pDemod,
                                                                             uint32_t * pSignalLevelData)
{
    uint8_t data[3];

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_monitor_SignalLevelData_ForUnlockOptimization");

    if ((!pDemod) || (!pSignalLevelData)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-M Bank : 0x09 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x09) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* IFAGC Output
     *  slave     Bank    Addr    Bit      Name
     * ---------------------------------------------------------------
     *  <SLV-M>   09h     68h     [7:0]    IDAGCA_IFLFACC[7:0]
     *  <SLV-M>   09h     69h     [7:0]    IDAGCA_IFLFACC[15:8]
     *  <SLV-M>   09h     6Ah     [3:0]    IDAGCA_IFLFACC[23:16]
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x68, data, 3) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    *pSignalLevelData = ((uint32_t) (data[2] & 0xFF) << 16) | ((uint32_t) (data[1] & 0xFF) << 8) | data[0];
    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc_monitor_InternalDigitalAGCOut (sony_cxd6801_demod_t * pDemod,
                                                             uint32_t * pDigitalAGCOut)
{
    uint8_t data[3];

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc_monitor_InternalDigitalAGCOut");

    if ((!pDemod) || (!pDigitalAGCOut)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in ACTIVE state only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC) {
        /* Not ATSC */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-M Bank : 0x08 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x08) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /* Baseband AGC Control Value Monitor
     *  slave     Bank    Addr    Bit      Name
     * ---------------------------------------------------------------
     *  <SLV-M>   08h     1Bh     [7:0]    IDBAGCA_GAIN[7:0]
     *  <SLV-M>   08h     1Ch     [7:0]    IDBAGCA_GAIN[15:8]
     *  <SLV-M>   08h     1Dh     [3:0]    IDBAGCA_GAIN[19:16]
     */

    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x1B, data, 3) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    *pDigitalAGCOut = ((data[2] & 0x0F) << 16) | (data[1] << 8) | data[0];

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t checkVQLock (sony_cxd6801_demod_t * pDemod)
{
    uint8_t data = 0x00;

    SONY_CXD6801_TRACE_ENTER ("checkVQLock");

    if (!pDemod) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }
    /* Set SLV-M Bank : 0x0D */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x00, 0x0D) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /* DMD lock flag
     *  slave     Bank    Addr    Bit    Name            Meaning
     * -----------------------------------------------------------------------
     *  <SLV-M>   0Dh     86h     [0]    VQLOCK          0:UNLOCK, 1:LOCK
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVM, 0x86, &data, 1) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (data & 0x01) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
    }
    else {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }
}
