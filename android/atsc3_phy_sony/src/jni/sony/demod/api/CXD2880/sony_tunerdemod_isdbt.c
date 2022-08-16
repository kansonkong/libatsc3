/*------------------------------------------------------------------------------
  Copyright 2014-2016 Sony Corporation

  Last Updated    : 2016/01/22
  Modification ID : 1da45a13df4bbad5b66b11d27f4907bb0b0617d4
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_isdbt.h"
#include "sony_tunerdemod_isdbt_monitor.h"

/*------------------------------------------------------------------------------
 Static Functions / Internal Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tunerdemod_isdbt_X_tune_DemodSetting (sony_tunerdemod_t * pTunerDemod, sony_dtv_bandwidth_t bandwidth,
                                                         sony_tunerdemod_clockmode_t clockMode, uint8_t demodClkEnable)
{
    /* This function is not static because it is called from ISDB-Tmm side. */

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_X_tune_DemodSetting");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* DMD_MODE_SEL */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x31, 0x06) != SONY_RESULT_OK) {
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

    /* AGC setting
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>   04h     5Dh    [4:0]     8'h0B      8'h0B     OCTL_IFAGC_COARSEGAIN[4:0]
     * <SLV-T>   04h     5Eh    [7:0]     8'h00      8'h68     OREG_ZIF_DAGC_CLIP_BW[2:0],OREG_ZIF_DAGC_NOSIG_BW[2:0],OREG_ZIF_DAGC_REDUCE_COARSEGAIN[1:0]
     */
    {
        uint8_t data[2] = {0x0B, 0x68};

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5D, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* TSIF setting
     *
     * slave    Bank    Addr    Bit    default     Value          Name
     * ----------------------------------------------------------------------------------
     * <SLV-M>   00h     CEh     [0]      8'h01      8'h00      ONOPARITY
     * <SLV-M>   00h     CFh     [0]      8'h01      8'h00      ONOPARITY_MANUAL_ON
     */
    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_SUB) {
        uint8_t data[2] = {0x00, 0x00};

        /* SLV-T bank 0x00 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }


        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xCE, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* ISDB-T/Tmm initial setting
     * Note: OTSRATECTRLOFF is moved to sony_tunerdemod_SetTSClockModeAndFreq.
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>   04h     5Ch    [3:0]     8'hD8      8'hFB     OREG_CFUPONTHRESHOLDMAX[7:0]
     * <SLV-T>   04h     5Fh    [7:0]     8'h14      8'h50     OREG_MER_ASNOS[7:0]
     * <SLV-T>   04h     78h    [2:0]     8'h05      8'h04     OREG_ENOD_DETSEL[2:0]
     * <SLV-T>   04h     79h    [0]       8'h01      8'h00     OREG_SEQ_SNRSEL
     * <SLV-T>   04h     7Ah    [4:0]     8'h00      8'h06     OREG_FORCE_MODEGI,OREG_MODE[1:0],OREG_GI[1:0]
     * <SLV-T>   04h     7Bh    [2:0]     8'h07      8'h05     OREG_COSNE_CRANGE[2:0]
     * <SLV-T>   04h     7Ch    [5:0]     8'h32      8'h2E     OREG_FCS_NTHETA[5:0]
     * <SLV-T>   04h     80h    [7:0]     8'h04      8'hCE     OREG_PNC_DISABLE,OREG_PNCON_STATE[2:0],OREG_PNC_NCOINIT_ON,OREG_PNC_UP_TYPE,OREG_PNC_NCOINIT_TYPE[1:0]
     * <SLV-T>   04h     81h    [4:0]     8'h19      8'h1D     OREG_CAS_DAGC_EN[1:0],OREG_CAS_DAGC_RDCBW[1:0],OREG_CAS_DAGC_USELAST
     * <SLV-T>   04h     82h    [7:0]     8'h30      8'h18     OREG_COSNE_CRATIO[7:0]
     * <SLV-T>   04h     83h    [7:0]     8'h2D      8'h00     OREG_ICIC_SCALE_CLIP_ON,OREG_ICIC_NPC_DISABLE,OREG_NPC_ALPHA[2:0],OREG_NPC_BETA[2:0]
     * <SLV-T>   04h     84h    [3:0]     8'h04      8'h00     OREG_NPC_NP_RATIO[3:0]
     * <SLV-T>   04h     85h    [7:0]     8'h06      8'h02     OREG_NPC_MOBILE_DEPTH_2K[7:0]
     * <SLV-T>   04h     86h    [7:0]     8'h0C      8'h04     OREG_NPC_MOBILE_DEPTH_4K[7:0]
     * <SLV-T>   04h     87h    [7:0]     8'h16      8'h08     OREG_NPC_MOBILE_DEPTH_8K[7:0]
     * <SLV-T>   04h     88h    [7:0]     8'h04      8'h02     OREG_NPC_STATIC_DEPTH_2K[7:0]
     * <SLV-T>   04h     89h    [7:0]     8'h08      8'h04     OREG_NPC_STATIC_DEPTH_4K[7:0]
     * <SLV-T>   04h     8Ah    [7:0]     8'h10      8'h08     OREG_NPC_STATIC_DEPTH_8K[7:0]
     * <SLV-T>   04h     8Bh    [7:0]     8'h00      8'h00     OREG_TITP_COEF_PHASE1_0[7:0]
     * <SLV-T>   04h     8Ch    [7:0]     8'h00      8'h00     OREG_TITP_COEF_PHASE1_1[7:0]
     * <SLV-T>   04h     8Dh    [7:0]     8'hFF      8'h08     OREG_TITP_COEF_PHASE1_2[7:0]
     * <SLV-T>   04h     8Eh    [7:0]     8'h07      8'hF7     OREG_TITP_COEF_PHASE1_3[7:0]
     * <SLV-T>   04h     8Fh    [7:0]     8'hED      8'hF6     OREG_TITP_COEF_PHASE1_4[7:0]
     * <SLV-T>   04h     90h    [7:0]     8'h46      8'h48     OREG_TITP_COEF_PHASE1_5[7:0]
     * <SLV-T>   04h     91h    [7:0]     8'h07      8'h04     OREG_TITP_COEF_PHASE1_6[7:0]
     * <SLV-T>   04h     92h    [7:0]     8'h00      8'h00     OREG_TITP_COEF_PHASE2_0[7:0]
     * <SLV-T>   04h     93h    [7:0]     8'h00      8'h00     OREG_TITP_COEF_PHASE2_1[7:0]
     * <SLV-T>   04h     94h    [7:0]     8'hFF      8'h07     OREG_TITP_COEF_PHASE2_2[7:0]
     * <SLV-T>   04h     95h    [7:0]     8'h06      8'hF8     OREG_TITP_COEF_PHASE2_3[7:0]
     * <SLV-T>   04h     96h    [7:0]     8'hEF      8'hF0     OREG_TITP_COEF_PHASE2_4[7:0]
     * <SLV-T>   04h     97h    [7:0]     8'h36      8'h43     OREG_TITP_COEF_PHASE2_5[7:0]
     * <SLV-T>   04h     98h    [7:0]     8'h17      8'h0E     OREG_TITP_COEF_PHASE2_6[7:0]
     * <SLV-T>   04h     99h    [7:0]     8'h00      8'h00     OREG_TITP_COEF_PHASE3_0[7:0]
     * <SLV-T>   04h     9Ah    [7:0]     8'h00      8'h00     OREG_TITP_COEF_PHASE3_1[7:0]
     * <SLV-T>   04h     9Bh    [7:0]     8'h00      8'h03     OREG_TITP_COEF_PHASE3_2[7:0]
     * <SLV-T>   04h     9Ch    [7:0]     8'h02      8'hFF     OREG_TITP_COEF_PHASE3_3[7:0]
     * <SLV-T>   04h     9Dh    [7:0]     8'hF7      8'hF4     OREG_TITP_COEF_PHASE3_4[7:0]
     * <SLV-T>   04h     9Eh    [7:0]     8'h1B      8'h23     OREG_TITP_COEF_PHASE3_5[7:0]
     * <SLV-T>   04h     9Fh    [7:0]     8'h2C      8'h28     OREG_TITP_COEF_PHASE3_6[7:0]
     * <SLV-T>   10h     A4h    [7:0]     8'h00      8'h03     OREG_ISIC_ISIEN[1:0]
     * <SLV-T>   14h     B0h    [7:0]     8'h01      8'h00     OREG_TITP_FIXED_COEF_TITP_ON
     * <SLV-T>   25h     F0h    [1:0]     8'h00      8'h01     OREG_RATECTL_MARGIN[9:8]
     * <SLV-T>   25h     F1h    [7:0]     8'h00      8'hF0     OREG_RATECTL_MARGIN[7:0]
     */
    {
        uint8_t data[39] = {0xFB, 0x50, 0x04, 0x00, 0x06, 0x05, 0x2E, 0xCE, 0x1D, 0x18,
                            0x00, 0x00, 0x02, 0x04, 0x08, 0x02, 0x04, 0x08, 0x00, 0x00,
                            0x08, 0xF7, 0xF6, 0x48, 0x04, 0x00, 0x00, 0x07, 0xF8, 0xF0,
                            0x43, 0x0E, 0x00, 0x00, 0x03, 0xFF, 0xF4, 0x23, 0x28};

        /* SLV-T bank 0x04 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5C, data[0]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5F, data[1]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x78, &data[2], 5) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x80, &data[7], 32) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
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
     * <SLV-T>   12h     44h    [7:0]     8'h10      8'h00      OREG_CWSEQ_BENUM_MINTHR[7:0]
     * <SLV-SUB> 10h     94h    [7:0]     8'h04      8'h06      OREG_SEQ_MERGOOD_CNT[7:0]
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
        /* SLV-T bank 0x10 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x10) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x94, 0x06) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* Packet Error Number monitor setting
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-M>   04h     6Dh    [7:0]     8'h9C   maxclkcnt1    OPEC_MAXCLKCNT[23:16]
     * <SLV-M>   04h     6Eh    [7:0]     8'h67   maxclkcnt2    OPEC_MAXCLKCNT[15:8]
     * <SLV-M>   04h     6Fh    [7:0]     8'h10   maxclkcnt3    OPEC_MAXCLKCNT[7:0]
     *
     * ----------+-------------+-------------+-------------+
     *  CLK mode |  maxclkcnt1 |  maxclkcnt2 |  maxclkcnt3 |
     * ----------+-------------+-------------+-------------+
     *         A |  8'h9C      |  8'hF2      |  8'h92      |
     * ----------+-------------+-------------+-------------+
     *         B |  8'hB2      |  8'h04      |  8'hEA      |
     * ----------+-------------+-------------+-------------+
     *         C |  8'hB7      |  8'h1B      |  8'h00      |
     * ----------+-------------+-------------+-------------+
     */
    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_SUB) {
        uint8_t dataA[3] = {0x9C, 0xF2, 0x92};
        uint8_t dataB[3] = {0xB2, 0x04, 0xEA};
        uint8_t dataC[3] = {0xB7, 0x1B, 0x00};
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

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x6D, pData, 3) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* SLV-T bank 0x04 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    switch (bandwidth) {
    case SONY_DTV_BW_8_MHZ:
        if (clockMode != SONY_TUNERDEMOD_CLOCKMODE_C) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     60h    [5:0]     8'h12      8'h11     OREG_TRCG_NOMINALRATE[37:32]
         * <SLV-T>   04h     61h    [7:0]     8'h00      8'hB8     OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>   04h     62h    [7:0]     8'h00      8'h00     OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>   04h     63h    [7:0]     8'h00      8'h00     OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>   04h     64h    [7:0]     8'h00      8'h00     OREG_TRCG_NOMINALRATE[7:0]
         */
        {
            uint8_t data[5] = {0x11, 0xB8, 0x00, 0x00, 0x00};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x60, data, 5) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* System Bandwidth setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     4Ah    [2:0]     8'h00      8'h00     OREG_CHANNEL_WIDTH[2:0]
         */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4A, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Demod core latency setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     7Dh    [5:0]     8'h01      8'h13     OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>   04h     7Eh    [7:0]     8'h28      8'hFC     OREG_CDRB_GTDOFST[7:0]
         */
        {
            uint8_t data[2] = {0x13, 0xFC};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x7D, data, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* SST setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     71h    [7:0]     8'h36      8'h3C     OREG_AGCSST_OFST[7:0]
         */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x71, 0x3C) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* MRC setting(Diversity mode only)
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-M>   04h     4Bh    [6:0]     8'h30      8'h2F     OREG_MRC_TO_8K[14:8]
         * <SLV-M>   04h     4Ch    [7:0]     8'h00      8'h40     OREG_MRC_TO_8K[7:0]
         * <SLV-M>   04h     51h    [1:0]     8'h00      8'h00     OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>   04h     52h    [7:0]     8'h90      8'h8D     OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>   04h     53h    [7:0]     8'h00      8'hC0     OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t data[5] = {0x2F, 0x40, 0x00, 0x8D, 0xC0};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, &data[0], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x51, &data[2], 3) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* Notch filter setting

         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     72h    [7:0]     8'hB3      8'hBF     OREG_CAS_CCISEQ_FNOTCH0[15:8]
         * <SLV-T>   04h     73h    [7:0]     8'h00      8'h08     OREG_CAS_CCISEQ_FNOTCH0[7:0]
         * <SLV-T>   04h     74h    [7:0]     8'hC4      8'hBF     OREG_CAS_CCIFLT2_FREQ_A[15:8]
         * <SLV-T>   04h     75h    [7:0]     8'h5F      8'h08     OREG_CAS_CCIFLT2_FREQ_A[7:0]
         * <SLV-T>   04h     76h    [7:0]     8'h52      8'h4C     OREG_CAS_CCIFLT2_FREQ_B[15:8]
         * <SLV-T>   04h     77h    [7:0]     8'h20      8'hC8     OREG_CAS_CCIFLT2_FREQ_B[7:0]
         */
        {
            uint8_t data[6] = {0xBF, 0x08, 0xBF, 0x08, 0x4C, 0xC8};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x72, data, 6) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    case SONY_DTV_BW_7_MHZ:
        if (clockMode != SONY_TUNERDEMOD_CLOCKMODE_C) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }

        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     60h    [5:0]     8'h12      8'h14     OREG_TRCG_NOMINALRATE[37:32]
         * <SLV-T>   04h     61h    [7:0]     8'h00      8'h40     OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>   04h     62h    [7:0]     8'h00      8'h00     OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>   04h     63h    [7:0]     8'h00      8'h00     OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>   04h     64h    [7:0]     8'h00      8'h00     OREG_TRCG_NOMINALRATE[7:0]
         */
        {
            uint8_t data[5] = {0x14, 0x40, 0x00, 0x00, 0x00};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x60, data, 5) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* System Bandwidth setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     4Ah    [2:0]     8'h00      8'h02     OREG_CHANNEL_WIDTH[2:0]
         */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4A, 0x02) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Demod core latency setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     7Dh    [5:0]     8'h01      8'h1A     OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>   04h     7Eh    [7:0]     8'h28      8'hFA     OREG_CDRB_GTDOFST[7:0]
         */
        {
            uint8_t data[2] = {0x1A, 0xFA};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x7D, data, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* SST setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     71h    [7:0]     8'h36      8'h36     OREG_AGCSST_OFST[7:0]
         */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x71, 0x36) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* MRC setting(Diversity mode only)
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-M>   04h     4Bh    [6:0]     8'h30      8'h36     OREG_MRC_TO_8K[14:8]
         * <SLV-M>   04h     4Ch    [7:0]     8'h00      8'h00     OREG_MRC_TO_8K[7:0]
         * <SLV-M>   04h     51h    [1:0]     8'h00      8'h00     OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>   04h     52h    [7:0]     8'h90      8'hA2     OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>   04h     53h    [7:0]     8'h00      8'h00     OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t data[5] = {0x36, 0x00, 0x00, 0xA2, 0x00};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, &data[0], 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x51, &data[2], 3) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* Notch filter setting

         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     72h    [7:0]     8'hB3      8'hC3     OREG_CAS_CCISEQ_FNOTCH0[15:8]
         * <SLV-T>   04h     73h    [7:0]     8'h00      8'h40     OREG_CAS_CCISEQ_FNOTCH0[7:0]
         * <SLV-T>   04h     74h    [7:0]     8'hC4      8'hC3     OREG_CAS_CCIFLT2_FREQ_A[15:8]
         * <SLV-T>   04h     75h    [7:0]     8'h5F      8'h40     OREG_CAS_CCIFLT2_FREQ_A[7:0]
         * <SLV-T>   04h     76h    [7:0]     8'h52      8'h57     OREG_CAS_CCIFLT2_FREQ_B[15:8]
         * <SLV-T>   04h     77h    [7:0]     8'h20      8'hC0     OREG_CAS_CCIFLT2_FREQ_B[7:0]
         */
        {
            uint8_t data[6] = {0xC3, 0x40, 0xC3, 0x40, 0x57, 0xC0};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x72, data, 6) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    case SONY_DTV_BW_6_MHZ:
        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>   04h     60h    [5:0]     8'h12      nomi1     OREG_TRCG_NOMINALRATE[37:32]
         * <SLV-T>   04h     61h    [7:0]     8'h00      nomi2     OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>   04h     62h    [7:0]     8'h00      nomi3     OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>   04h     63h    [7:0]     8'h00      nomi4     OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>   04h     64h    [7:0]     8'h00      nomi5     OREG_TRCG_NOMINALRATE[7:0]
         *
         * ----------+---------+---------+---------+---------+---------
         *  CLK mode |  nomi1  |  nomi2  |  nomi3  |  nomi4  |  nomi5
         * ----------+---------+---------+---------+---------+---------
         *      A,C  |  8'h17  |  8'hA0  |  8'h00  |  8'h00  |  8'h00
         * ----------+---------+---------+---------+---------+---------
         *        B  |  8'h16  |  8'hF8  |  8'h00  |  8'h00  |  8'h00
         * ----------+---------+---------+---------+---------+---------
         */
        {
            uint8_t dataAC[5] = {0x17, 0xA0, 0x00, 0x00, 0x00};
            uint8_t dataB[5]  = {0x16, 0xF8, 0x00, 0x00, 0x00};
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
         * slave    Bank    Addr    Bit    default   Value          Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>   04h     7Dh    [5:0]     8'h01     gtofst1    OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>   04h     7Eh    [7:0]     8'h28     gtofst2    OREG_CDRB_GTDOFST[7:0]
         *
         * ----------+---------+---------
         *  CLK mode | gtofst1 | gtofst2
         * ----------+---------+---------
         *         A |  8'h1A  |  8'hFA
         * ----------+---------+---------
         *         B |  8'h1E  |  8'h99
         * ----------+---------+---------
         *         C |  8'h1F  |  8'h79
         * ----------+---------+---------
         */
        {
            uint8_t dataA[2] = {0x1A, 0xFA};
            uint8_t dataB[2] = {0x1E, 0x99};
            uint8_t dataC[2] = {0x1F, 0x79};
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
         * <SLV-T>   04h     71h    [7:0]     8'h36       sst      OREG_AGCSST_OFST[7:0]
         *
         * -----------+----------+
         *  CLK mode  |  sst     |
         * -----------+----------+
         *      A, B  |  8'h30   |
         * -----------+----------+
         *         C  |  8'h2F   |
         * -----------+----------+
         */
        {
            uint8_t data = 0;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                data = 0x30;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                data = 0x2F;
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
         * <SLV-M>   04h     4Bh    [6:0]     8'h30     mrc8k1     OREG_MRC_TO_8K[14:8]
         * <SLV-M>   04h     4Ch    [7:0]     8'h00     mrc8k2     OREG_MRC_TO_8K[7:0]
         * <SLV-M>   04h     51h    [1:0]     8'h00     mrc_sym1   OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>   04h     52h    [7:0]     8'h90     mrc_sym2   OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>   04h     53h    [7:0]     8'h00     mrc_sym3   OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         *
         * ------------+----------+----------+----------+----------+----------+
         *  CLK mode   |  mrc8k1  |  mrc8k2  | mrc_sym1 | mrc_sym2 | mrc_sym3 |
         * ------------+----------+----------+----------+----------+----------+
         *         A   |  8'h36   |  8'h00   |  8'h00   |  8'hA2   |  8'h00   |
         * ------------+----------+----------+----------+----------+----------+
         *         B   |  8'h3D   |  8'h40   |  8'h00   |  8'hB7   |  8'hC0   |
         * ------------+----------+----------+----------+----------+----------+
         *         C   |  8'h3F   |  8'h00   |  8'h00   |  8'hBD   |  8'h00   |
         * ------------+----------+----------+----------+----------+----------+
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t dataA[5] = {0x36, 0x00, 0x00, 0xA2, 0x00};
            uint8_t dataB[5] = {0x3D, 0x40, 0x00, 0xB7, 0xC0};
            uint8_t dataC[5] = {0x3F, 0x00, 0x00, 0xBD, 0x00};
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
         * <SLV-T>   04h     72h    [7:0]     8'hB3      8'hC4     OREG_CAS_CCISEQ_FNOTCH0[15:8]
         * <SLV-T>   04h     73h    [7:0]     8'h00      8'h60     OREG_CAS_CCISEQ_FNOTCH0[7:0]
         * <SLV-T>   04h     74h    [7:0]     8'hC4      8'hC4     OREG_CAS_CCIFLT2_FREQ_A[15:8]
         * <SLV-T>   04h     75h    [7:0]     8'h5F      8'h60     OREG_CAS_CCIFLT2_FREQ_A[7:0]
         * <SLV-T>   04h     76h    [7:0]     8'h52      8'h52     OREG_CAS_CCIFLT2_FREQ_B[15:8]
         * <SLV-T>   04h     77h    [7:0]     8'h20      8'h20     OREG_CAS_CCIFLT2_FREQ_B[7:0]
         */
        {
            uint8_t data[6] = {0xC4, 0x60, 0xC4, 0x60, 0x52, 0x20};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x72, data, 6) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (demodClkEnable) {
        /* SLV-T bank 0x00 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Clock enable (OREG_CK_EN) */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xFD, 0x01) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t X_sleep_isdbt_DemodSetting (sony_tunerdemod_t * pTunerDemod)
{
    SONY_TRACE_ENTER ("X_sleep_isdbt_DemodSetting");

    /* ISDB-T cancel Demod parameter setting
     * Note: OTSRATECTRLOFF is moved to sony_tunerdemod_SetTSClockModeAndFreq.
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>  04h     5Ch     [3:0]    8'hD8        8'hD8     OREG_CFUPONTHRESHOLDMAX[7:0]
     * <SLV-T>  04h     5Eh     [7:0]    8'h00        8'h00     OREG_ZIF_DAGC_CLIP_BW[2:0],OREG_ZIF_DAGC_NOSIG_BW[2:0],OREG_ZIF_DAGC_REDUCE_COARSEGAIN[1:0]
     * <SLV-T>  04h     5Fh     [7:0]    8'h14        8'h14     OREG_MER_ASNOS[7:0]
     * <SLV-T>  04h     72h     [7:0]    8'hB3        8'hB3     OREG_CAS_CCISEQ_FNOTCH0[15:8]
     * <SLV-T>  04h     73h     [7:0]    8'h00        8'h00     OREG_CAS_CCISEQ_FNOTCH0[7:0]
     * <SLV-T>  04h     74h     [7:0]    8'hC4        8'hC4     OREG_CAS_CCIFLT2_FREQ_A[15:8]
     * <SLV-T>  04h     75h     [7:0]    8'h5F        8'h5F     OREG_CAS_CCIFLT2_FREQ_A[7:0]
     * <SLV-T>  04h     76h     [7:0]    8'h52        8'h52     OREG_CAS_CCIFLT2_FREQ_B[15:8]
     * <SLV-T>  04h     77h     [7:0]    8'h20        8'h20     OREG_CAS_CCIFLT2_FREQ_B[7:0]
     * <SLV-T>  04h     78h     [2:0]    8'h05        8'h05     OREG_ENOD_DETSEL[2:0]
     * <SLV-T>  04h     79h     [0]      8'h01        8'h01     OREG_SEQ_SNRSEL
     * <SLV-T>  04h     7Ah     [4:0]    8'h00        8'h00     OREG_FORCE_MODEGI,OREG_MODE[1:0],OREG_GI[1:0]
     * <SLV-T>  04h     7Bh     [2:0]    8'h07        8'h07     OREG_COSNE_CRANGE[2:0]
     * <SLV-T>  04h     7Ch     [5:0]    8'h32        8'h32     OREG_FCS_NTHETA[5:0]
     * <SLV-T>  04h     80h     [7:0]    8'h04        8'h04     OREG_PNC_DISABLE,OREG_PNCON_STATE[2:0],OREG_PNC_NCOINIT_ON,OREG_PNC_UP_TYPE,OREG_PNC_NCOINIT_TYPE[1:0]
     * <SLV-T>  04h     81h     [4:0]    8'h19        8'h19     OREG_CAS_DAGC_EN[1:0],OREG_CAS_DAGC_RDCBW[1:0],OREG_CAS_DAGC_USELAST
     * <SLV-T>  04h     82h     [7:0]    8'h30        8'h30     OREG_COSNE_CRATIO[7:0]
     * <SLV-T>  04h     83h     [7:0]    8'h2D        8'h2D     OREG_ICIC_SCALE_CLIP_ON,OREG_ICIC_NPC_DISABLE,OREG_NPC_ALPHA[2:0],OREG_NPC_BETA[2:0]
     * <SLV-T>  04h     84h     [3:0]    8'h04        8'h04     OREG_NPC_NP_RATIO[3:0]
     * <SLV-T>  04h     85h     [7:0]    8'h06        8'h06     OREG_NPC_MOBILE_DEPTH_2K[7:0]
     * <SLV-T>  04h     86h     [7:0]    8'h0C        8'h0C     OREG_NPC_MOBILE_DEPTH_4K[7:0]
     * <SLV-T>  04h     87h     [7:0]    8'h16        8'h16     OREG_NPC_MOBILE_DEPTH_8K[7:0]
     * <SLV-T>  04h     88h     [7:0]    8'h04        8'h04     OREG_NPC_STATIC_DEPTH_2K[7:0]
     * <SLV-T>  04h     89h     [7:0]    8'h08        8'h08     OREG_NPC_STATIC_DEPTH_4K[7:0]
     * <SLV-T>  04h     8Ah     [7:0]    8'h10        8'h10     OREG_NPC_STATIC_DEPTH_8K[7:0]
     * <SLV-T>  04h     8Bh     [7:0]    8'h00        8'h00     OREG_TITP_COEF_PHASE1_0[7:0]
     * <SLV-T>  04h     8Ch     [7:0]    8'h00        8'h00     OREG_TITP_COEF_PHASE1_1[7:0]
     * <SLV-T>  04h     8Dh     [7:0]    8'hFF        8'hFF     OREG_TITP_COEF_PHASE1_2[7:0]
     * <SLV-T>  04h     8Eh     [7:0]    8'h07        8'h07     OREG_TITP_COEF_PHASE1_3[7:0]
     * <SLV-T>  04h     8Fh     [7:0]    8'hED        8'hED     OREG_TITP_COEF_PHASE1_4[7:0]
     * <SLV-T>  04h     90h     [7:0]    8'h46        8'h46     OREG_TITP_COEF_PHASE1_5[7:0]
     * <SLV-T>  04h     91h     [7:0]    8'h07        8'h07     OREG_TITP_COEF_PHASE1_6[7:0]
     * <SLV-T>  04h     92h     [7:0]    8'h00        8'h00     OREG_TITP_COEF_PHASE2_0[7:0]
     * <SLV-T>  04h     93h     [7:0]    8'h00        8'h00     OREG_TITP_COEF_PHASE2_1[7:0]
     * <SLV-T>  04h     94h     [7:0]    8'hFF        8'hFF     OREG_TITP_COEF_PHASE2_2[7:0]
     * <SLV-T>  04h     95h     [7:0]    8'h06        8'h06     OREG_TITP_COEF_PHASE2_3[7:0]
     * <SLV-T>  04h     96h     [7:0]    8'hEF        8'hEF     OREG_TITP_COEF_PHASE2_4[7:0]
     * <SLV-T>  04h     97h     [7:0]    8'h36        8'h36     OREG_TITP_COEF_PHASE2_5[7:0]
     * <SLV-T>  04h     98h     [7:0]    8'h17        8'h17     OREG_TITP_COEF_PHASE2_6[7:0]
     * <SLV-T>  04h     99h     [7:0]    8'h00        8'h00     OREG_TITP_COEF_PHASE3_0[7:0]
     * <SLV-T>  04h     9Ah     [7:0]    8'h00        8'h00     OREG_TITP_COEF_PHASE3_1[7:0]
     * <SLV-T>  04h     9Bh     [7:0]    8'h00        8'h00     OREG_TITP_COEF_PHASE3_2[7:0]
     * <SLV-T>  04h     9Ch     [7:0]    8'h02        8'h02     OREG_TITP_COEF_PHASE3_3[7:0]
     * <SLV-T>  04h     9Dh     [7:0]    8'hF7        8'hF7     OREG_TITP_COEF_PHASE3_4[7:0]
     * <SLV-T>  04h     9Eh     [7:0]    8'h1B        8'h1B     OREG_TITP_COEF_PHASE3_5[7:0]
     * <SLV-T>  04h     9Fh     [7:0]    8'h2C        8'h2C     OREG_TITP_COEF_PHASE3_6[7:0]
     * <SLV-T>  10h     A4h     [7:0]    8'h00        8'h00     OREG_ISIC_ISIEN[1:0]
     */
    {
        uint8_t data[46] = {0xD8, 0x00, 0x14, 0xB3, 0x00, 0xC4, 0x5F, 0x52, 0x20, 0x05,
                            0x01, 0x00, 0x07, 0x32, 0x04, 0x19, 0x30, 0x2D, 0x04, 0x06,
                            0x0C, 0x16, 0x04, 0x08, 0x10, 0x00, 0x00, 0xFF, 0x07, 0xED,
                            0x46, 0x07, 0x00, 0x00, 0xFF, 0x06, 0xEF, 0x36, 0x17, 0x00,
                            0x00, 0x00, 0x02, 0xF7, 0x1B, 0x2C};

        /* SLV-T bank 0x04 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5C, data[0]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5E, &data[1], 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x72, &data[3], 11) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x80, &data[14], 32) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
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
     * <SLV-SUB> 10h     94h    [7:0]     8'h04      8'h04      OREG_SEQ_MERGOOD_CNT[7:0]
     */
    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* SLV-T bank 0x10 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x10) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x94, 0x04) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbt_PresetSetting (sony_tunerdemod_t * pTunerDemod,
                                                   sony_tunerdemod_isdbt_preset_info_t * pPresetInfo)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_PresetSetting");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Setting Fast Acquisition Mode */
    /* Set SLV-T Bank : 0x60 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x60) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /*    - Disable Fast Acquisition Mode
     *
     *      When the TMCC information is not known in advance, such as in channel scan
     *      operation, disable Fast Acquisition Mode by setting the following register
     *      values.
     *
     *    slave    Bank    Addr    Bit    default      Value        Name
     *   ---------------------------------------------------------------------------------
     *   <SLV-T>   60h     59h    [0]       8'h00       8'h00      OCTL_PRESET_EN
     *   <SLV-T>   60h     5Ah    [0]       8'h00       8'h00      OCTL_S2_FRAMESYNC_EN
     */
    if (!pPresetInfo) {
        uint8_t data[2] = {0x00, 0x00};
        if (pTunerDemod->pRegio->WriteRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x59, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }
    /*    - Enable Fast Acquisition Mode
     *
     *      Once the TMCC information is known, such as in normal channel tuning, you can
     *      enable Fast Acquisition Mode by setting the following register values.
     *

     *   slave    Bank    Addr    Bit    default      Value        Name
     *  ---------------------------------------------------------------------------------
     *  <SLV-T>   60h     59h    [0]       8'h00       8'h01      OCTL_PRESET_EN
     *  <SLV-T>   60h     5Ah    [0]       8'h00       8'h01      OCTL_S2_FRAMESYNC_EN
     */
    else {
        uint8_t data[2] = {0x01, 0x01};
        if (pTunerDemod->pRegio->WriteRegister(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x59, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        /*
         * slave    Bank  Addr  Bit    TMCC Bit assign.(*)   Default    Name
         * ----------------------------------------------------------------------------
         * <SLV-T>  60h   62h  [7:0]   "B20-27"               8'h3D    PIR_HOST_TMCC_SET_2
         * <SLV-T>  60h   63h  [7:0]   "B28-35"               8'h25    PIR_HOST_TMCC_SET_3
         * <SLV-T>  60h   64h  [7:0]   "B36-43"               8'h8B    PIR_HOST_TMCC_SET_4
         * <SLV-T>  60h   65h  [7:0]   "B44-51"               8'h4B    PIR_HOST_TMCC_SET_5
         * <SLV-T>  60h   66h  [7:0]   "B52-59"               8'h3F    PIR_HOST_TMCC_SET_6
         * <SLV-T>  60h   67h  [7:0]   "B60-67"               8'hFF    PIR_HOST_TMCC_SET_7
         * <SLV-T>  60h   68h  [7:0]   "B68-75"               8'h25    PIR_HOST_TMCC_SET_8
         * <SLV-T>  60h   69h  [7:0]   "B76-83"               8'h8B    PIR_HOST_TMCC_SET_9
         * <SLV-T>  60h   6Ah  [7:0]   "B84-91"               8'h4B    PIR_HOST_TMCC_SET_10
         * <SLV-T>  60h   6Bh  [7:0]   "B92-99"               8'h3F    PIR_HOST_TMCC_SET_11
         * <SLV-T>  60h   6Ch  [7:0]   "B100-107"             8'hFF    PIR_HOST_TMCC_SET_12
         * <SLV-T>  60h   6Dh  [7:0]   "B108-115"             8'hFF    PIR_HOST_TMCC_SET_13
         * <SLV-T>  60h   6Eh  [7:0]   "B116-121,2'b00"       8'hFC    PIR_HOST_TMCC_SET_14
         */
        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x62, pPresetInfo->data, sizeof(pPresetInfo->data)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    SONY_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tunerdemod_isdbt_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                           sony_isdbt_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_Tune1");

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
    result = sony_tunerdemod_CommonTuneSetting1 (pTunerDemod, SONY_DTV_SYSTEM_ISDBT, pTuneParam->centerFreqKHz, pTuneParam->bandwidth,
        pTuneParam->oneSegmentOptimize, 0);
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

sony_result_t sony_tunerdemod_isdbt_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                           sony_isdbt_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_Tune2");

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
    result = sony_tunerdemod_CommonTuneSetting2 (pTunerDemod, SONY_DTV_SYSTEM_ISDBT, 0);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    pTunerDemod->state = SONY_TUNERDEMOD_STATE_ACTIVE;
    pTunerDemod->frequencyKHz = pTuneParam->centerFreqKHz;
    pTunerDemod->system = SONY_DTV_SYSTEM_ISDBT;
    pTunerDemod->bandwidth = pTuneParam->bandwidth;

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        pTunerDemod->pDiverSub->state = SONY_TUNERDEMOD_STATE_ACTIVE;
        pTunerDemod->pDiverSub->frequencyKHz = pTuneParam->centerFreqKHz;
        pTunerDemod->pDiverSub->system = SONY_DTV_SYSTEM_ISDBT;
        pTunerDemod->pDiverSub->bandwidth = pTuneParam->bandwidth;
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbt_SleepSetting (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_SleepSetting");

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

    result = X_sleep_isdbt_DemodSetting (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = X_sleep_isdbt_DemodSetting (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_isdbt_CheckDemodLock (sony_tunerdemod_t * pTunerDemod,
                                                    sony_tunerdemod_lock_result_t * pLock)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t demodLock = 0;
    uint8_t tsLock = 0;
    uint8_t unlockDetected = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_CheckDemodLock");

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

    result = sony_tunerdemod_isdbt_monitor_SyncStat (pTunerDemod, &demodLock, &tsLock, &unlockDetected);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SINGLE) {
        if (demodLock) {
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
        if (demodLock) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_LOCKED;
            SONY_TRACE_RETURN (result);
        }

        result = sony_tunerdemod_isdbt_monitor_SyncStat_sub (pTunerDemod, &demodLock, &unlockDetectedSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (demodLock) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_LOCKED;
        } else if (unlockDetected && unlockDetectedSub) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_UNLOCKED;
        } else {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
        }

        SONY_TRACE_RETURN (result);
    }
}

sony_result_t sony_tunerdemod_isdbt_CheckTSLock (sony_tunerdemod_t * pTunerDemod,
                                                 sony_tunerdemod_lock_result_t * pLock)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t demodLock = 0;
    uint8_t tsLock = 0;
    uint8_t unlockDetected = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_CheckTSLock");

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

    result = sony_tunerdemod_isdbt_monitor_SyncStat (pTunerDemod, &demodLock, &tsLock, &unlockDetected);
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

        result = sony_tunerdemod_isdbt_monitor_SyncStat_sub (pTunerDemod, &demodLock, &unlockDetectedSub);
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

sony_result_t sony_tunerdemod_isdbt_CheckDemodOrTSLock (sony_tunerdemod_t * pTunerDemod,
                                                        sony_tunerdemod_lock_result_t * pLock)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t demodLock = 0;
    uint8_t tsLock = 0;
    uint8_t unlockDetected = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_CheckDemodOrTSLock");

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

    result = sony_tunerdemod_isdbt_monitor_SyncStat (pTunerDemod, &demodLock, &tsLock, &unlockDetected);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /*
     * In ISDB-T, TS lock condition can be 1 earlier than demod lock condition
     * if preset tuning is enabled. (by sony_tunerdemod_isdbt_SetPreset)
     * In such case, this function assumed that lock condition is OK too.
     */

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SINGLE) {
        if (demodLock || tsLock) {
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
        if (demodLock || tsLock) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_LOCKED;
            SONY_TRACE_RETURN (result);
        }

        result = sony_tunerdemod_isdbt_monitor_SyncStat_sub (pTunerDemod, &demodLock, &unlockDetectedSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (demodLock) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_LOCKED;
        } else if (unlockDetected && unlockDetectedSub) {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_UNLOCKED;
        } else {
            *pLock = SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT;
        }

        SONY_TRACE_RETURN (result);
    }
}

sony_result_t sony_tunerdemod_isdbt_SetPreset (sony_tunerdemod_t * pTunerDemod,
                                               sony_tunerdemod_isdbt_preset_info_t * pPresetInfo)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_isdbt_SetPreset");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pPresetInfo) {
        pTunerDemod->isdbtPresetInfo = *pPresetInfo;
        pTunerDemod->isdbtPresetInfoEnable = 1;
    } else {
        pTunerDemod->isdbtPresetInfoEnable = 0;
    }

    SONY_TRACE_RETURN (result);
}
