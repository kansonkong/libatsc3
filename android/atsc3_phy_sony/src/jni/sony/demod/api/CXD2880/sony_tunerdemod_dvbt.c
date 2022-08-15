/*------------------------------------------------------------------------------
  Copyright 2014-2016 Sony Corporation

  Last Updated    : 2016/01/22
  Modification ID : 1da45a13df4bbad5b66b11d27f4907bb0b0617d4
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_dvbt.h"
#include "sony_tunerdemod_dvbt_monitor.h"

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_result_t X_tune_dvbt_DemodSetting (sony_tunerdemod_t * pTunerDemod, sony_dtv_bandwidth_t bandwidth,
                                               sony_tunerdemod_clockmode_t clockMode)
{
    SONY_TRACE_ENTER ("X_tune_dvbt_DemodSetting");

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* DMD_MODE_SEL */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x31, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* CLK mode setting
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>   04h     65h    [7:0]     8'h52     ckffrq1    OREG_CKF_FREQ[15:8]
     * <SLV-T>   04h     66h    [7:0]     8'h49     ckffrq2    OREG_CKF_FREQ[7:0]
     *
     * -----------+----------+----------+
     *  CLK mode  |  ckffrq1 |  ckffrq2 |
     * -----------+----------+----------+
     *         A  |  8'h52   |  8'h49   |
     * -----------+----------+----------+
     *         B  |  8'h5D   |  8'h55   |
     * -----------+----------+----------+
     *         C  |  8'h60   |  8'h00   |
     * -----------+----------+----------+
     */
    {
        uint8_t dataA[2] = {0x52, 0x49};
        uint8_t dataB[2] = {0x5D, 0x55};
        uint8_t dataC[2] = {0x60, 0x00};
        uint8_t * pData = NULL;

        /* SLV-T bank 0x04 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        switch (clockMode) {
        case SONY_TUNERDEMOD_CLOCKMODE_A:
            pData = dataA;
            break;
        case SONY_TUNERDEMOD_CLOCKMODE_B:
            pData = dataB;
            break;
        case SONY_TUNERDEMOD_CLOCKMODE_C:
            pData = dataC;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x65, pData, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* AGC gain setting
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>  04h     5Dh     [4:0]    8'h0B        8'h07     OCTL_IFAGC_COARSEGAIN[4:0]
     */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5D, 0x07) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* TSIF setting
     *
     * slave    Bank    Addr    Bit    default     Value          Name
     * ----------------------------------------------------------------------------------
     * <SLV-M>   00h     CEh     [0]      8'h01      8'h01      ONOPARITY
     * <SLV-M>   00h     CFh     [0]      8'h01      8'h01      ONOPARITY_MANUAL_ON
     */
    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_SUB) {
        uint8_t data[2] = {0x01, 0x01};

        /* SLV-T bank 0x00 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }


        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xCE, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* DVB-T initial setting
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>   04h     5Ch    [7:0]     8'hD8      8'hFB      OREG_CFUPONTHRESHOLDMAX[7:0]
     * <SLV-T>   10h     A4h    [7:0]     8'h00      8'h03      OREG_ISIC_ISIEN[1:0]
     * <SLV-T>   14h     B0h    [7:0]     8'h01      8'h00      OREG_TITP_FIXED_COEF_TITP_ON
     * <SLV-T>   25h     F0h    [1:0]     8'h00      8'h01      OREG_RATECTL_MARGIN[9:8]
     * <SLV-T>   25h     F1h    [7:0]     8'h00      8'hF0      OREG_RATECTL_MARGIN[7:0]
     */
    /* SLV-T bank 0x04 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5C, 0xFB) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xA4, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x14 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x14) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xB0, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x25 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x25) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    {
        uint8_t data[2] = {0x01, 0xF0};

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xF0, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* Diversity setting(Diversity mode only)
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>   12h     44h     [7:0]    8'h10        8'h00     OREG_CWSEQ_BENUM_MINTHR[7:0]
     * <SLV-SUB> 11h     87h     [7:0]    8'h04        8'hD2     OREG_SEQ_SNRGOOD_CNT[7:0]
     */
    if ((pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) || (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB)) {
        /* SLV-T bank 0x12 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x12) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x44, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* SLV-T bank 0x11 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x11) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x87, 0xD2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* Packet Error Number monitor setting
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-M>  04h     68h     [7:0]    8'h73   maxclkcnt1     OREG_BER_MAXCLKCNT[23:16]
     * <SLV-M>  04h     69h     [7:0]    8'hCA   maxclkcnt2     OREG_BER_MAXCLKCNT[15:8]
     * <SLV-M>  04h     6Ah     [7:0]    8'h49   maxclkcnt3     OREG_BER_MAXCLKCNT[7:0]
     *
     * -----------+-------------+-------------+-------------+
     *  CLK mode  |  maxclkcnt1 |  maxclkcnt2 |  maxclkcnt3 |
     * -----------+-------------+-------------+-------------+
     *         A  |  8'h73      |  8'hCA      |  8'h49      |
     * -----------+-------------+-------------+-------------+
     *         B  |  8'hC8      |  8'h13      |  8'hAA      |
     * -----------+-------------+-------------+-------------+
     *         C  |  8'hDC      |  8'h6C      |  8'h00      |
     * -----------+-------------+-------------+-------------+
     */
    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_SUB) {
        uint8_t dataA[3] = {0x73, 0xCA, 0x49};
        uint8_t dataB[3] = {0xC8, 0x13, 0xAA};
        uint8_t dataC[3] = {0xDC, 0x6C, 0x00};
        uint8_t * pData = NULL;

        /* SLV-T bank 0x04 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        switch (clockMode) {
        case SONY_TUNERDEMOD_CLOCKMODE_A:
            pData = dataA;
            break;
        case SONY_TUNERDEMOD_CLOCKMODE_B:
            pData = dataB;
            break;
        case SONY_TUNERDEMOD_CLOCKMODE_C:
            pData = dataC;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x68, pData, 3) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* SLV-T bank 0x04 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    switch (bandwidth) {
    case SONY_DTV_BW_8_MHZ:
        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     60h     [5:0]    8'h12        nomi1     OREG_TRCG_NOMINALRATE[37:32]
         * <SLV-T>  04h     61h     [7:0]    8'h00        nomi2     OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>  04h     62h     [7:0]    8'h00        nomi3     OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>  04h     63h     [7:0]    8'h00        nomi4     OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>  04h     64h     [7:0]    8'h00        nomi5     OREG_TRCG_NOMINALRATE[7:0]
         *
         * ----------+---------+---------+---------+---------+---------
         *  CLK mode |  nomi1  |  nomi2  |  nomi3  |  nomi4  |  nomi5
         * ----------+---------+---------+---------+---------+---------
         *       A,C |  8'h15  |  8'h00  |  8'h00  |  8'h00  |  8'h00
         * ----------+---------+---------+---------+---------+---------
         *         B |  8'h14  |  8'h6A  |  8'hAA  |  8'hAA  |  8'hAA
         * ----------+---------+---------+---------+---------+---------
         */
        {
            uint8_t dataAC[5] = {0x15, 0x00, 0x00, 0x00, 0x00};
            uint8_t dataB[5]  = {0x14, 0x6A, 0xAA, 0xAA, 0xAA};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataAC;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x60, pData, 5) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* System Bandwidth setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     4Ah     [2:0]    8'h00        8'h00     OREG_CHANNEL_WIDTH[2:0]
         */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4A, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Demod core latency setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     7Dh     [5:0]    8'h01       gtofst1    OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>  04h     7Eh     [7:0]    8'h28       gtofst2    OREG_CDRB_GTDOFST[7:0]
         *
         * -----------+---------+---------
         *  CLK mode  | gtofst1 | gtofst2
         * -----------+---------+---------
         *         A  |  8'h01  |  8'h28
         * -----------+---------+---------
         *         B  |  8'h11  |  8'h44
         * -----------+---------+---------
         *         C  |  8'h15  |  8'h28
         * -----------+---------+---------
         */
        {
            uint8_t dataA[2] = {0x01, 0x28};
            uint8_t dataB[2] = {0x11, 0x44};
            uint8_t dataC[2] = {0x15, 0x28};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
                pData = dataA;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataC;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x7D, pData, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* SST setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     71h     [7:0]    8'h36          sst     OREG_AGCSST_OFST[7:0]
         *
         * -----------+----------+
         *  CLK mode  |  sst     |
         * -----------+----------+
         *      A, B  |  8'h35   |
         * -----------+----------+
         *         C  |  8'h34   |
         * -----------+----------+
         */
        {
            uint8_t data = 0;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                data = 0x35;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                data = 0x34;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x71, data) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* MRC setting(Diversity mode only)
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-M>  04h     4Bh     [6:0]    8'h30      mrc8k1      OREG_MRC_TO_8K[14:8]
         * <SLV-M>  04h     4Ch     [7:0]    8'h00      mrc8k2      OREG_MRC_TO_8K[7:0]
         * <SLV-M>  04h     51h     [1:0]    8'h00      mrc_sym1    OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>  04h     52h     [7:0]    8'h90      mrc_sym2    OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>  04h     53h     [7:0]    8'h00      mrc_sym3    OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         *
         * ------------+----------+----------+----------+----------+----------+
         *  CLK mode   |  mrc8k1  |  mrc8k2  | mrc_sym1 | mrc_sym2 | mrc_sym3 |
         * ------------+----------+----------+----------+----------+----------+
         *         A   |  8'h30   |  8'h00   |  8'h00   |  8'h90   |  8'h00   |
         * ------------+----------+----------+----------+----------+----------+
         *         B   |  8'h36   |  8'h71   |  8'h00   |  8'hA3   |  8'h55   |
         * ------------+----------+----------+----------+----------+----------+
         *         C   |  8'h38   |  8'h00   |  8'h00   |  8'hA8   |  8'h00   |
         * ------------+----------+----------+----------+----------+----------+
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t dataA[5] = {0x30, 0x00, 0x00, 0x90, 0x00};
            uint8_t dataB[5] = {0x36, 0x71, 0x00, 0xA3, 0x55};
            uint8_t dataC[5] = {0x38, 0x00, 0x00, 0xA8, 0x00};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
                pData = dataA;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataC;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, &pData[0], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x51, &pData[2], 3) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* Notch filter setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     72h     [7:0]    8'hB3        8'hB3     OREG_CAS_CCISEQ_FNOTCH0[15:8]
         * <SLV-T>  04h     73h     [7:0]    8'h00        8'h00     OREG_CAS_CCISEQ_FNOTCH0[7:0]
         * <SLV-T>  04h     6Bh     [3:0]    8'h01        8'h01     OREG_CAS_CCIFLT2_EN_CW[1:0], OREG_CAS_CCIFLT2_EN_CW2[1:0]
         * <SLV-T>  04h     6Ch     [1:0]    8'h02        8'h02     OREG_CAS_CWSEQ_ON1, OREG_CAS_CWSEQ_ON2
         */
        {
            uint8_t data[4] = {0xB3, 0x00, 0x01, 0x02};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x72, &data[0], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x6B, &data[2], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    case SONY_DTV_BW_7_MHZ:
        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     60h     [5:0]    8'h12        nomi1     OREG_TRCG_NOMINALRATE[37:32]
         * <SLV-T>  04h     61h     [7:0]    8'h00        nomi2     OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>  04h     62h     [7:0]    8'h00        nomi3     OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>  04h     63h     [7:0]    8'h00        nomi4     OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>  04h     64h     [7:0]    8'h00        nomi5     OREG_TRCG_NOMINALRATE[7:0]
         }
         * ----------+---------+---------+---------+---------+---------
         *  CLK mode |  nomi1  |  nomi2  |  nomi3  |  nomi4  |  nomi5
         * ----------+---------+---------+---------+---------+---------
         *       A,C |  8'h18  |  8'h00  |  8'h00  |  8'h00  |  8'h00
         * ----------+---------+---------+---------+---------+---------
         *         B |  8'h17  |  8'h55  |  8'h55  |  8'h55  |  8'h55
         * ----------+---------+---------+---------+---------+---------
         */
        {
            uint8_t dataAC[5] = {0x18, 0x00, 0x00, 0x00, 0x00};
            uint8_t dataB[5]  = {0x17, 0x55, 0x55, 0x55, 0x55};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataAC;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x60, pData, 5) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* System Bandwidth setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     4Ah     [2:0]    8'h00        8'h02     OREG_CHANNEL_WIDTH[2:0]
         */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4A, 0x02) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Demod core latency setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     7Dh     [5:0]    8'h01       gtofst1    OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>  04h     7Eh     [7:0]    8'h28       gtofst2    OREG_CDRB_GTDOFST[7:0]
         *
         * ----------+---------+---------
         *  CLK mode | gtofst1 | gtofst2
         * ----------+---------+---------
         *         A |  8'h12  |  8'h4C
         * ----------+---------+---------
         *         B |  8'h1F  |  8'h15
         * ----------+---------+---------
         *         C |  8'h1F  |  8'hF8
         * ----------+---------+---------
         */
        {
            uint8_t dataA[2] = {0x12, 0x4C};
            uint8_t dataB[2] = {0x1F, 0x15};
            uint8_t dataC[2] = {0x1F, 0xF8};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
                pData = dataA;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataC;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x7D, pData, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* SST setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     71h     [7:0]    8'h36         sst2     OREG_AGCSST_OFST[7:0]
         *
         * -----------+----------+
         *  CLK mode  |  sst     |
         * -----------+----------+
         *      A, B  |  8'h2F   |
         * -----------+----------+
         *         C  |  8'h2E   |
         * -----------+----------+
         */
        {
            uint8_t data = 0;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                data = 0x2F;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                data = 0x2E;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x71, data) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* MRC setting(Diversity mode only)
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-M>  04h     4Bh     [6:0]    8'h30      mrc8k1      OREG_MRC_TO_8K[14:8]
         * <SLV-M>  04h     4Ch     [7:0]    8'h00      mrc8k2      OREG_MRC_TO_8K[7:0]
         * <SLV-M>  04h     51h     [1:0]    8'h00      mrc_sym1    OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>  04h     52h     [7:0]    8'h90      mrc_sym2    OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>  04h     53h     [7:0]    8'h00      mrc_sym3    OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         *
         * ------------+----------+----------+----------+----------+----------+
         *  CLK mode   |  mrc8k1  |  mrc8k2  | mrc_sym1 | mrc_sym2 | mrc_sym3 |
         * ------------+----------+----------+----------+----------+----------+
         *         A   |  8'h36   |  8'hDB   |  8'h00   |  8'hA4   |  8'h92   |
         * ------------+----------+----------+----------+----------+----------+
         *         B   |  8'h3E   |  8'h38   |  8'h00   |  8'hBA   |  8'hAA   |
         * ------------+----------+----------+----------+----------+----------+
         *         C   |  8'h40   |  8'h00   |  8'h00   |  8'hC0   |  8'h00   |
         * ------------+----------+----------+----------+----------+----------+
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t dataA[5] = {0x36, 0xDB, 0x00, 0xA4, 0x92};
            uint8_t dataB[5] = {0x3E, 0x38, 0x00, 0xBA, 0xAA};
            uint8_t dataC[5] = {0x40, 0x00, 0x00, 0xC0, 0x00};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
                pData = dataA;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataC;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, &pData[0], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x51, &pData[2], 3) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* Notch filter setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     72h     [7:0]    8'hB3        8'hB8     OREG_CAS_CCISEQ_FNOTCH0[15:8]
         * <SLV-T>  04h     73h     [7:0]    8'h00        8'h00     OREG_CAS_CCISEQ_FNOTCH0[7:0]
         * <SLV-T>  04h     6Bh     [3:0]    8'h01        8'h00     OREG_CAS_CCIFLT2_EN_CW[1:0], OREG_CAS_CCIFLT2_EN_CW2[1:0]
         * <SLV-T>  04h     6Ch     [1:0]    8'h02        8'h03     OREG_CAS_CWSEQ_ON1, OREG_CAS_CWSEQ_ON2
         */
        {
            uint8_t data[4] = {0xB8, 0x00, 0x00, 0x03};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x72, &data[0], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x6B, &data[2], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    case SONY_DTV_BW_6_MHZ:
        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     60h    [5:0]     8'h12      nomi1      OREG_TRCG_NOMINALRATE[37:32]
         * <SLV-T>   04h     61h    [7:0]     8'h00      nomi2      OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>   04h     62h    [7:0]     8'h00      nomi3      OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>   04h     63h    [7:0]     8'h00      nomi4      OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>   04h     64h    [7:0]     8'h00      nomi5      OREG_TRCG_NOMINALRATE[7:0]
         *
         * ----------+---------+---------+---------+---------+---------
         *  CLK mode |  nomi1  |  nomi2  |  nomi3  |  nomi4  |  nomi5
         * ----------+---------+---------+---------+---------+---------
         *       A,C |  8'h1C  |  8'h00  |  8'h00  |  8'h00  |  8'h00
         * ----------+---------+---------+---------+---------+---------
         *         B |  8'h1B  |  8'h38  |  8'hE3  |  8'h8E  |  8'h38
         * ----------+---------+---------+---------+---------+---------
         */
        {
            uint8_t dataAC[5] = {0x1C, 0x00, 0x00, 0x00, 0x00};
            uint8_t dataB[5]  = {0x1B, 0x38, 0xE3, 0x8E, 0x38};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataAC;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x60, pData, 5) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* System Bandwidth setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     4Ah     [2:0]    8'h00        8'h04     OREG_CHANNEL_WIDTH[2:0]
         */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4A, 0x04) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Demod core latency setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     7Dh     [5:0]    8'h01      gtofst1     OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>  04h     7Eh     [7:0]    8'h28      gtofst2     OREG_CDRB_GTDOFST[7:0]
         *
         * ----------+---------+---------
         *  CLK mode | gtofst1 | gtofst2
         * ----------+---------+---------
         *         A |  8'h1F  |  8'hF8
         * ----------+---------+---------
         *         B |  8'h24  |  8'h43
         * ----------+---------+---------
         *         C |  8'h25  |  8'h4C
         * ----------+---------+---------
         */
        {
            uint8_t dataA[2] = {0x1F, 0xF8};
            uint8_t dataB[2] = {0x24, 0x43};
            uint8_t dataC[2] = {0x25, 0x4C};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
                pData = dataA;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataC;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x7D, pData, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* SST setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     71h     [7:0]    8'h36         sst2     OREG_AGCSST_OFST[7:0]
         *
         * -----------+----------+
         *  CLK mode  |  sst     |
         * -----------+----------+
         *      A, C  |  8'h29   |
         * -----------+----------+
         *         B  |  8'h2A   |
         * -----------+----------+
         */
        {
            uint8_t data = 0;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                data = 0x29;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                data = 0x2A;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x71, data) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* MRC setting(Diversity mode only)
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-M>  04h     4Bh     [6:0]    8'h30      mrc8k1      OREG_MRC_TO_8K[14:8]
         * <SLV-M>  04h     4Ch     [7:0]    8'h00      mrc8k2      OREG_MRC_TO_8K[7:0]
         * <SLV-M>  04h     51h     [1:0]    8'h00      mrc_sym1    OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>  04h     52h     [7:0]    8'h90      mrc_sym2    OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>  04h     53h     [7:0]    8'h00      mrc_sym3    OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         *
         * ------------+----------+----------+----------+----------+----------+
         *  CLK mode   |  mrc8k1  |  mrc8k2  | mrc_sym1 | mrc_sym2 | mrc_sym3 |
         * ------------+----------+----------+----------+----------+----------+
         *         A   |  8'h40   |  8'h00   |  8'h00   |  8'hC0   |  8'h00   |
         * ------------+----------+----------+----------+----------+----------+
         *         B   |  8'h48   |  8'h97   |  8'h00   |  8'hD9   |  8'hC7   |
         * ------------+----------+----------+----------+----------+----------+
         *         C   |  8'h4A   |  8'hAA   |  8'h00   |  8'hDF   |  8'hFF   |
         * ------------+----------+----------+----------+----------+----------+
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t dataA[5] = {0x40, 0x00, 0x00, 0xC0, 0x00};
            uint8_t dataB[5] = {0x48, 0x97, 0x00, 0xD9, 0xC7};
            uint8_t dataC[5] = {0x4A, 0xAA, 0x00, 0xDF, 0xFF};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
                pData = dataA;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataC;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, &pData[0], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x51, &pData[2], 3) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* Notch filter setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     72h     [7:0]    8'hB3        8'hBE     OREG_CAS_CCISEQ_FNOTCH0[15:8]
         * <SLV-T>  04h     73h     [7:0]    8'h00        8'hAB     OREG_CAS_CCISEQ_FNOTCH0[7:0]
         * <SLV-T>  04h     6Bh     [3:0]    8'h01        8'h00     OREG_CAS_CCIFLT2_EN_CW[1:0], OREG_CAS_CCIFLT2_EN_CW2[1:0]
         * <SLV-T>  04h     6Ch     [1:0]    8'h02        8'h03     OREG_CAS_CWSEQ_ON1, OREG_CAS_CWSEQ_ON2
         */
        {
            uint8_t data[4] = {0xBE, 0xAB, 0x00, 0x03};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x72, &data[0], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x6B, &data[2], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    case SONY_DTV_BW_5_MHZ:
        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     60h    [5:0]     8'h12      nomi1      OREG_TRCG_NOMINALRATE[37:32]
         * <SLV-T>   04h     61h    [7:0]     8'h00      nomi2      OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>   04h     62h    [7:0]     8'h00      nomi3      OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>   04h     63h    [7:0]     8'h00      nomi4      OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>   04h     64h    [7:0]     8'h00      nomi5      OREG_TRCG_NOMINALRATE[7:0]
         *
         * ----------+---------+---------+---------+---------+---------
         *  CLK mode |  nomi1  |  nomi2  |  nomi3  |  nomi4  |  nomi5
         * ----------+---------+---------+---------+---------+---------
         *       A,C |  8'h21  |  8'h99  |  8'h99  |  8'h99  |  8'h99
         * ----------+---------+---------+---------+---------+---------
         *         B |  8'h20  |  8'hAA  |  8'hAA  |  8'hAA  |  8'hAA
         * ----------+---------+---------+---------+---------+---------
         */
        {
            uint8_t dataAC[5] = {0x21, 0x99, 0x99, 0x99, 0x99};
            uint8_t dataB[5]  = {0x20, 0xAA, 0xAA, 0xAA, 0xAA};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataAC;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x60, pData, 5) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* System Bandwidth setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     4Ah     [2:0]    8'h00        8'h06     OREG_CHANNEL_WIDTH[2:0]
         */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4A, 0x06) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Demod core latency setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     7Dh     [5:0]    8'h01      gtofst1     OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>  04h     7Eh     [7:0]    8'h28      gtofst2     OREG_CDRB_GTDOFST[7:0]
         *
         * ----------+---------+---------
         *  CLK mode | gtofst1 | gtofst2
         * ----------+---------+---------
         *         A |  8'h26  |  8'h5D
         * ----------+---------+---------
         *         B |  8'h2B  |  8'h84
         * ----------+---------+---------
         *         C |  8'h2C  |  8'hC2
         * ----------+---------+---------
         */
        {
            uint8_t dataA[2] = {0x26, 0x5D};
            uint8_t dataB[2] = {0x2B, 0x84};
            uint8_t dataC[2] = {0x2C, 0xC2};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
                pData = dataA;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataC;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x7D, pData, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* SST setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     71h     [7:0]    8'h36         sst2     OREG_AGCSST_OFST[7:0]
         *
         * -----------+----------+
         *  CLK mode  |  sst     |
         * -----------+----------+
         *      A, B  |  8'h24   |
         * -----------+----------+
         *         C  |  8'h23   |
         * -----------+----------+
         */
        {
            uint8_t data = 0;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                data = 0x24;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                data = 0x23;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x71, data) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* MRC setting(Diversity mode only)
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-M>  04h     4Bh     [6:0]    8'h30      mrc8k1      OREG_MRC_TO_8K[14:8]
         * <SLV-M>  04h     4Ch     [7:0]    8'h00      mrc8k2      OREG_MRC_TO_8K[7:0]
         * <SLV-M>  04h     51h     [1:0]    8'h00      mrc_sym1    OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>  04h     52h     [7:0]    8'h90      mrc_sym2    OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>  04h     53h     [7:0]    8'h00      mrc_sym3    OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         *
         * ------------+----------+----------+----------+----------+----------+
         *  CLK mode   |  mrc8k1  |  mrc8k2  | mrc_sym1 | mrc_sym2 | mrc_sym3 |
         * ------------+----------+----------+----------+----------+----------+
         *         A   |  8'h4C   |  8'hCC   |  8'h00   |  8'hE6   |  8'h66   |
         * ------------+----------+----------+----------+----------+----------+
         *         B   |  8'h57   |  8'h1C   |  8'h01   |  8'h05   |  8'h55   |
         * ------------+----------+----------+----------+----------+----------+
         *         C   |  8'h59   |  8'h99   |  8'h01   |  8'h0C   |  8'hCC   |
         * ------------+----------+----------+----------+----------+----------+
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t dataA[5] = {0x4C, 0xCC, 0x00, 0xE6, 0x66};
            uint8_t dataB[5] = {0x57, 0x1C, 0x01, 0x05, 0x55};
            uint8_t dataC[5] = {0x59, 0x99, 0x01, 0x0C, 0xCC};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
                pData = dataA;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataC;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, &pData[0], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x51, &pData[2], 3) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* Notch filter setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     72h     [7:0]    8'hB3        8'hC8     OREG_CAS_CCISEQ_FNOTCH0[15:8]
         * <SLV-T>  04h     73h     [7:0]    8'h00        8'h01     OREG_CAS_CCISEQ_FNOTCH0[7:0]
         * <SLV-T>  04h     6Bh     [3:0]    8'h01        8'h00     OREG_CAS_CCIFLT2_EN_CW[1:0], OREG_CAS_CCIFLT2_EN_CW2[1:0]
         * <SLV-T>  04h     6Ch     [1:0]    8'h02        8'h03     OREG_CAS_CWSEQ_ON1, OREG_CAS_CWSEQ_ON2
         */
        {
            uint8_t data[4] = {0xC8, 0x01, 0x00, 0x03};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x72, &data[0], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x6B, &data[2], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
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

static sony_result_t X_sleep_dvbt_DemodSetting (sony_tunerdemod_t * pTunerDemod)
{
    SONY_TRACE_ENTER ("X_sleep_dvbt_DemodSetting");

    /*
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>  04h     5Ch     [3:0]    8'hD8        8'hD8     OREG_CFUPONTHRESHOLDMAX[7:0]
     * <SLV-T>  10h     A4h     [7:0]    8'h00        8'h00     OREG_ISIC_ISIEN[1:0]
     */
    /* SLV-T bank 0x04 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5C, 0xD8) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xA4, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Diversity setting(Diversity mode only)
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-SUB> 11h     87h     [7:0]    8'h04        8'h04     OREG_SEQ_SNRGOOD_CNT[7:0]
     */
    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* SLV-T bank 0x11 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x11) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x87, 0x04) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t dvbt_SetProfile (sony_tunerdemod_t * pTunerDemod,
                                      sony_dvbt_profile_t profile)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("dvbt_SetProfile");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-T bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Set DVB-T hierachy setting
     *
     * slave     Bank    Addr    Bit   default    Value    Name
     * -----------------------------------------------------------------
     * <SLV-M>   10h     67h     [0]   8'h00      8'h01    OREG_LPSELECT
     */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x67, (profile == SONY_DVBT_PROFILE_HP) ? 0x00 : 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tunerdemod_dvbt_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                          sony_dvbt_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_Tune1");

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
    result = sony_tunerdemod_CommonTuneSetting1 (pTunerDemod, SONY_DTV_SYSTEM_DVBT, pTuneParam->centerFreqKHz, pTuneParam->bandwidth, 0, 0);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* DVB-T dependent settings */
    result = X_tune_dvbt_DemodSetting (pTunerDemod, pTuneParam->bandwidth, pTunerDemod->clockMode);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = X_tune_dvbt_DemodSetting (pTunerDemod->pDiverSub, pTuneParam->bandwidth, pTunerDemod->pDiverSub->clockMode);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* Set Profile */
    result = dvbt_SetProfile (pTunerDemod, pTuneParam->profile);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                          sony_dvbt_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_Tune2");

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
    result = sony_tunerdemod_CommonTuneSetting2 (pTunerDemod, SONY_DTV_SYSTEM_DVBT, 0);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    pTunerDemod->state = SONY_TUNERDEMOD_STATE_ACTIVE;
    pTunerDemod->frequencyKHz = pTuneParam->centerFreqKHz;
    pTunerDemod->system = SONY_DTV_SYSTEM_DVBT;
    pTunerDemod->bandwidth = pTuneParam->bandwidth;

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        pTunerDemod->pDiverSub->state = SONY_TUNERDEMOD_STATE_ACTIVE;
        pTunerDemod->pDiverSub->frequencyKHz = pTuneParam->centerFreqKHz;
        pTunerDemod->pDiverSub->system = SONY_DTV_SYSTEM_DVBT;
        pTunerDemod->pDiverSub->bandwidth = pTuneParam->bandwidth;
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt_SleepSetting (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_SleepSetting");

    if (!pTunerDemod) {
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

    result = X_sleep_dvbt_DemodSetting (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = X_sleep_dvbt_DemodSetting (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt_CheckDemodLock (sony_tunerdemod_t * pTunerDemod,
                                                   sony_tunerdemod_lock_result_t * pLock)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t syncStat = 0;
    uint8_t tsLock = 0;
    uint8_t unlockDetected = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_CheckDemodLock");

    if ((!pTunerDemod) || (!pLock)) {
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

    result = sony_tunerdemod_dvbt_monitor_SyncStat (pTunerDemod, &syncStat, &tsLock, &unlockDetected);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SINGLE) {
        if (syncStat == 6) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_LOCKED;
        } else if (unlockDetected) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_UNLOCKED;
        } else {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
        }

        SONY_TRACE_RETURN (result);
    } else {
        uint8_t unlockDetectedSub = 0;

        /* Diver */
        if (syncStat == 6) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_LOCKED;
            SONY_TRACE_RETURN (result);
        }

        result = sony_tunerdemod_dvbt_monitor_SyncStat_sub (pTunerDemod, &syncStat, &unlockDetectedSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (syncStat == 6) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_LOCKED;
        } else if (unlockDetected && unlockDetectedSub) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_UNLOCKED;
        } else {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
        }

        SONY_TRACE_RETURN (result);
    }
}

sony_result_t sony_tunerdemod_dvbt_CheckTSLock (sony_tunerdemod_t * pTunerDemod,
                                                sony_tunerdemod_lock_result_t * pLock)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t syncStat = 0;
    uint8_t tsLock = 0;
    uint8_t unlockDetected = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt_CheckTSLock");

    if ((!pTunerDemod) || (!pLock)) {
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

    result = sony_tunerdemod_dvbt_monitor_SyncStat (pTunerDemod, &syncStat, &tsLock, &unlockDetected);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SINGLE) {
        if (tsLock) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_LOCKED;
        } else if (unlockDetected) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_UNLOCKED;
        } else {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
        }

        SONY_TRACE_RETURN (result);
    } else {
        uint8_t unlockDetectedSub = 0;

        /* Diver */
        if (tsLock) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_LOCKED;
            SONY_TRACE_RETURN (result);
        } else if (!unlockDetected) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
            SONY_TRACE_RETURN (result);
        }

        result = sony_tunerdemod_dvbt_monitor_SyncStat_sub (pTunerDemod, &syncStat, &unlockDetectedSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (unlockDetected && unlockDetectedSub) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_UNLOCKED;
        } else {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
        }

        SONY_TRACE_RETURN (result);
    }
}
