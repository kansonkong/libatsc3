/*------------------------------------------------------------------------------
  Copyright 2014-2016 Sony Corporation

  Last Updated    : 2016/04/14
  Modification ID : b3a863c9449ebbf8408830fb7cfafb763ac7a67f
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod_dvbt2.h"
#include "sony_tunerdemod_dvbt2_monitor.h"

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_result_t X_tune_dvbt2_DemodSetting (sony_tunerdemod_t * pTunerDemod, sony_dtv_bandwidth_t bandwidth,
                                                sony_tunerdemod_clockmode_t clockMode)
{
    SONY_TRACE_ENTER ("X_tune_dvbt2_DemodSetting");

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* DMD_MODE_SEL */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x31, 0x02) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* AGC gain setting
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>  04h     5Dh     [4:0]    8'h0B        8'h0B     OCTL_IFAGC_COARSEGAIN[4:0]
     */
    /* SLV-T bank 0x04 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x5D, 0x0B) != SONY_RESULT_OK) {
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

    /* DVB-T2 initial setting
     *
     * slave    Bank    Addr    Bit     default      Value        Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>  20h     8Ah     [5:0]    8'h05        8'h07     OREG_SEQ_SNR_GOODTHR[7:0]
     * <SLV-T>  20h     90h     [5:0]    8'h04        8'h06     OREG_SEQ_SNR_BADTHR[7:0]
     * <SLV-T>  25h     F0h     [1:0]    8'h00        8'h01     OREG_RATECTL_MARGIN[9:8]
     * <SLV-T>  25h     F1h     [7:0]    8'h00        8'hF0     OREG_RATECTL_MARGIN[7:0]
     * <SLV-T>  2Ah     DCh     [5:0]    8'h03        8'h00     OREG_FCS_FITP4_DISABLE_FFTSIZE[5:0]
     * <SLV-T>  2Ah     DEh     [5:0]    8'h03        8'h00     OREG_FCS_FITP4_DISABLE_FFTSIZE_FOITP[5:0]
     * <SLV-T>  2Dh     73h     [7:0]    8'h00        8'h04     OREG_CGON_HI_WAIT_KAIROS_1[15:8]
     * <SLV-T>  2Dh     74h     [7:0]    8'h00        8'hB0     OREG_CGON_HI_WAIT_KAIROS_1[7:0]
     * <SLV-T>  2Dh     75h     [7:0]    8'h04        8'h00     OREG_CGON_HI_WAIT_KAIROS_2[15:8]
     * <SLV-T>  2Dh     76h     [7:0]    8'hB0        8'h00     OREG_CGON_HI_WAIT_KAIROS_2[7:0]
     * <SLV-T>  2Dh     8Fh     [7:0]    8'h0E        8'h09     OREG_CGON_LO_WAIT_KAIROS_1[15:8]
     * <SLV-T>  2Dh     90h     [7:0]    8'h4C        8'h9C     OREG_CGON_LO_WAIT_KAIROS_1[7:0]
     * <SLV-T>  2Dh     91h     [7:0]    8'h09        8'h0E     OREG_CGON_LO_WAIT_KAIROS_2[15:8]
     * <SLV-T>  2Dh     92h     [7:0]    8'h9C        8'h4C     OREG_CGON_LO_WAIT_KAIROS_2[7:0]
     */
    {
        uint8_t data[14] = {0x07, 0x06, 0x01, 0xF0, 0x00, 0x00, 0x04, 0xB0, 0x00, 0x00, 0x09, 0x9C, 0x0E, 0x4C};

        /* SLV-T bank 0x20 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x20) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x8A, data[0]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x90, data[1]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* SLV-T bank 0x25 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x25) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xF0, &data[2], 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* SLV-T bank 0x2A */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x2A) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xDC, data[4]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xDE, data[5]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* SLV-T bank 0x2D */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x2D) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x73, &data[6], 4) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x8F, &data[10], 4) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* DVB-T2 CLK mode setting
     * Note: OREG_SEQ_NOT2_DTIME[15:8] is moved to sony_tunerdemod_dvbt2_SetProfile.
     *
     * slave    Bank    Addr    Bit    default(A)  Value(B)  Value(C)   Name
     * -----------------------------------------------------------------------------------------
     * <SLV-T>  04h     1Dh     [7:0]    8'h52     8'h5D     8'h60      OREG_CKF_FREQ[15:8]
     * <SLV-T>  04h     1Eh     [7:0]    8'h49     8'h55     8'h00      OREG_CKF_FREQ[7:0]
     * <SLV-T>  04h     1Fh     [6:0]    8'h2C     8'h32     8'h34      OREG_T2FS_TIMEOUT_WTIME[14:8]
     * <SLV-T>  04h     22h     [7:0]    8'h51     8'h5C     8'h5E      OREG_SEQ_L1PRE_WTIME[15:8]
     * <SLV-T>  04h     24h     [7:0]    8'h51     8'h5C     8'h5E      OREG_SEQ_L1POST_WTIME[15:8]
     * <SLV-T>  04h     26h     [7:0]    8'h3D     8'h45     8'h47      OREG_SEQ_DMDOK_WTIME[15:8]
     * <SLV-T>  04h     29h     [7:0]    8'h15     8'h17     8'h18      OREG_SEQ_RESET_WTIME[7:0]
     * <SLV-T>  04h     2Ah     [7:0]    8'h29     8'h2E     8'h2F      OREG_SEQ_P1DET_WTIME[15:8]
     * <SLV-T>  04h     2Dh     [7:0]    8'h0C     8'h0D     8'h0E      OREG_SEQ_NOT2_DINTERVAL[15:8]
     *
     * <SLV-M>  04h     2Eh     [3:0]    8'h04     8'h05     8'h05      OREG_SP_BERT_MAXCLKCNT[27:24]
     * <SLV-M>  04h     2Fh     [7:0]    8'hE7     8'h90     8'hB8      OREG_SP_BERT_MAXCLKCNT[23:16]
     * <SLV-M>  04h     30h     [7:0]    8'h94     8'h27     8'hD8      OREG_SP_BERT_MAXCLKCNT[15:8]
     * <SLV-M>  04h     31h     [7:0]    8'h92     8'h55     8'h00      OREG_SP_BERT_MAXCLKCNT[7:0]
     * <SLV-M>  04h     32h     [5:0]    8'h09     8'h0B     8'h0B      OREG_PBER_MESB[13:8]
     * <SLV-M>  04h     33h     [7:0]    8'hCF     8'h20     8'h72      OREG_PBER_MESB[7:0]
     * <SLV-M>  04h     35h     [7:0]    8'h7E     8'h8F     8'h93      OREG_SP_TSLK_TIMNOPST[23:16]
     * <SLV-M>  04h     36h     [7:0]    8'hD0     8'hD6     8'hF3      OREG_SP_TSLK_TIMNOPST[15:8]
     * <SLV-M>  04h     37h     [7:0]    8'h49     8'hEA     8'h00      OREG_SP_TSLK_TIMNOPST[7:0]
     * <SLV-M>  04h     38h     [7:0]    8'hCD     8'hC8     8'hCD      OREG_LDPCD_PLP_MIN_OUTTIME_N[7:0]
     * <SLV-M>  04h     39h     [7:0]    8'hCD     8'hC8     8'hCD      OREG_LDPCD_PLP_MIN_OUTTIME_S[7:0]
     * <SLV-M>  04h     3Ah     [7:0]    8'h1F     8'h23     8'h24      OREG_FP_RATE_CTRL_MARGIN[15:8]
     * <SLV-M>  04h     3Bh     [7:0]    8'h5B     8'h91     8'h95      OREG_FP_RATE_CTRL_MARGIN[7:0]
     *
     * <SLV-T>  04h     3Ch     [7:0]    8'h0B     8'h01     8'h01      OREG_PATSYNC_PAT_LENGTH[7:0]
     * <SLV-T>  04h     3Dh     [7:0]    8'h6A     8'h02     8'h02      OREG_PATSYNC_PATTERN_0[7:0]
     * <SLV-T>  04h     56h     [7:0]    8'hC9     8'hE4     8'hEB      OCTL_IFAGC_INITWAIT[7:0]
     * <SLV-T>  04h     57h     [7:0]    8'h03     8'h03     8'h03      OCTL_IFAGC_MINWAIT[7:0]
     * <SLV-T>  04h     58h     [7:0]    8'h33     8'h39     8'h3B      OCTL_IFAGC_MAXWAIT[7:0]
     */
    {
        uint8_t dataA_1[9] = {0x52, 0x49, 0x2C, 0x51, 0x51, 0x3D, 0x15, 0x29, 0x0C};
        uint8_t dataB_1[9] = {0x5D, 0x55, 0x32, 0x5C, 0x5C, 0x45, 0x17, 0x2E, 0x0D};
        uint8_t dataC_1[9] = {0x60, 0x00, 0x34, 0x5E, 0x5E, 0x47, 0x18, 0x2F, 0x0E};

        uint8_t dataA_2[13] = {0x04, 0xE7, 0x94, 0x92, 0x09, 0xCF, 0x7E, 0xD0, 0x49, 0xCD, 0xCD, 0x1F, 0x5B};
        uint8_t dataB_2[13] = {0x05, 0x90, 0x27, 0x55, 0x0B, 0x20, 0x8F, 0xD6, 0xEA, 0xC8, 0xC8, 0x23, 0x91};
        uint8_t dataC_2[13] = {0x05, 0xB8, 0xD8, 0x00, 0x0B, 0x72, 0x93, 0xF3, 0x00, 0xCD, 0xCD, 0x24, 0x95};

        uint8_t dataA_3[5] = {0x0B, 0x6A, 0xC9, 0x03, 0x33};
        uint8_t dataB_3[5] = {0x01, 0x02, 0xE4, 0x03, 0x39};
        uint8_t dataC_3[5] = {0x01, 0x02, 0xEB, 0x03, 0x3B};

        uint8_t * pData_1 = NULL;
        uint8_t * pData_2 = NULL;
        uint8_t * pData_3 = NULL;

        switch (clockMode) {
        case SONY_TUNERDEMOD_CLOCKMODE_A:
            pData_1 = dataA_1;
            pData_2 = dataA_2;
            pData_3 = dataA_3;
            break;
        case SONY_TUNERDEMOD_CLOCKMODE_B:
            pData_1 = dataB_1;
            pData_2 = dataB_2;
            pData_3 = dataB_3;
            break;
        case SONY_TUNERDEMOD_CLOCKMODE_C:
            pData_1 = dataC_1;
            pData_2 = dataC_2;
            pData_3 = dataC_3;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* SLV-T bank 0x04 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1D, &pData_1[0], 3) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x22, pData_1[3]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x24, pData_1[4]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x26, pData_1[5]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x29, &pData_1[6], 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x2D, pData_1[8]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_SUB) {
            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x2E, &pData_2[0], 6) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x35, &pData_2[6], 7) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x3C, &pData_3[0], 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x56, &pData_3[2], 3) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    switch (bandwidth) {
    case SONY_DTV_BW_8_MHZ:
        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * --------------------------------------------------------------------------------
         * <SLV-T>  04h     10h     [6:0]    8'h15        nomi1     OREG_TRCG_NOMINALRATE[38:32]
         * <SLV-T>  04h     11h     [7:0]    8'h00        nomi2     OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>  04h     12h     [7:0]    8'h00        nomi3     OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>  04h     13h     [7:0]    8'h00        nomi4     OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>  04h     14h     [7:0]    8'h00        nomi5     OREG_TRCG_NOMINALRATE[7:0]
         * <SLV-T>  04h     15h     [3:0]    8'h00        8'h00     OREG_TRCG_LMTVAL0
         *
         * ----------+---------+---------+---------+---------+---------
         *  CLK mode |  nomi1  |  nomi2  |  nomi3  |  nomi4  |  nomi5
         * ----------+---------+---------+---------+---------+---------
         *       A,C |  8'h15  |  8'h00  |  8'h00  |  8'h00  |  8'h00
         * ----------+---------+---------+---------+---------+---------
         *         B |  8'h14  |  8'h6A  |  8'hAA  |  8'hAA  |  8'hAB
         * ----------+---------+---------+---------+---------+---------
         */
        {
            uint8_t dataAC[6] = {0x15, 0x00, 0x00, 0x00, 0x00, 0x00};
            uint8_t dataB[6]  = {0x14, 0x6A, 0xAA, 0xAA, 0xAB, 0x00};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, pData, 6) != SONY_RESULT_OK) {
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
         * ---------------------------------------------------------------------------------
         * <SLV-T>  04h     19h    [2:0]     8'h19     gtdofst1     OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>  04h     1Ah    [2:0]     8'hD2     gtdofst2     OREG_CDRB_GTDOFST[7:0]
         *
         * ----------+------------+-----------+
         *  CLK mode |  gtdofst1  |  gtdofst2 |
         * ----------+------------+-----------+
         *         A |    8'h19   |   8'hD2   |
         * ----------+------------+-----------+
         *       B,C |    8'h3F   |   8'hFF   |
         * ----------+------------+-----------+
         */
        {
            uint8_t dataA[2]  = {0x19, 0xD2};
            uint8_t dataBC[2] = {0x3F, 0xFF};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
                pData = dataA;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataBC;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x19, pData, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* SST setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>  04h     1Bh     [7:0]    8'h06        sst1      OREG_FWTG_AGCSST_OFST[15:8]
         * <SLV-T>  04h     1Ch     [7:0]    8'h2B        sst2      OREG_FWTG_AGCSST_OFST[7:0]
         *
         * -----------+----------+----------+
         *  CLK mode  |  sst1    |  sst2    |
         * -----------+----------+----------+
         *         A  |  8'h06   |  8'h2A   |
         * -----------+----------+----------+
         *         B  |  8'h06   |  8'h29   |
         * -----------+----------+----------+
         *         C  |  8'h06   |  8'h28   |
         * -----------+----------+----------+
         */
        {
            uint8_t dataA[2] = {0x06, 0x2A};
            uint8_t dataB[2] = {0x06, 0x29};
            uint8_t dataC[2] = {0x06, 0x28};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1B, pData, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* MRC setting(Diversity mode only)
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * ---------------------------------------------------------------------------------
         * <SLV-M>  04h     4Bh     [6:0]    8'h30        mrc8k1    OREG_MRC_TO_8K[14:8]
         * <SLV-M>  04h     4Ch     [7:0]    8'h00        mrc8k2    OREG_MRC_TO_8K[7:0]
         * <SLV-M>  04h     4Dh     [6:0]    8'h60        mrc16k1   OREG_MRC_TO_16K[15:8]
         * <SLV-M>  04h     4Eh     [7:0]    8'h00        mrc16k2   OREG_MRC_TO_16K[7:0]
         * <SLV-M>  04h     4Fh     [6:0]    8'h60        mrc32k1   OREG_MRC_TO_32K[15:8]
         * <SLV-M>  04h     50h     [7:0]    8'h00        mrc32k2   OREG_MRC_TO_32K[7:0]
         * <SLV-M>  04h     51h     [1:0]    8'h00        mrc_sym1  OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>  04h     52h     [7:0]    8'h90        mrc_sym2  OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>  04h     53h     [7:0]    8'h00        mrc_sym3  OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         *
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *  CLK mode  |  mrc8k1 | mrc8k2 | mrc16k1 | mrc16k2 | mrc32k1 | mrc32k2 | mrc_sym1| mrc_sym2| mrc_sym3|
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         A  |  8'h28  | 8'h00  |  8'h50  |  8'h00  |  8'h60  |  8'h00  |  8'h00  |  8'h90  |  8'h00  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         B  |  8'h2D  | 8'h5E  |  8'h5A  |  8'hBD  |  8'h6C  |  8'hE3  |  8'h00  |  8'hA3  |  8'h55  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         C  |  8'h2E  | 8'hAA  |  8'h5D  |  8'h55  |  8'h70  |  8'h00  |  8'h00  |  8'hA8  |  8'h00  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t dataA[9] = {0x28, 0x00, 0x50, 0x00, 0x60, 0x00, 0x00, 0x90, 0x00};
            uint8_t dataB[9] = {0x2D, 0x5E, 0x5A, 0xBD, 0x6C, 0xE3, 0x00, 0xA3, 0x55};
            uint8_t dataC[9] = {0x2E, 0xAA, 0x5D, 0x55, 0x70, 0x00, 0x00, 0xA8, 0x00};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, pData, 9) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    case SONY_DTV_BW_7_MHZ:
        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * --------------------------------------------------------------------------------
         * <SLV-T>  04h     10h     [6:0]    8'h15        nomi1     OREG_TRCG_NOMINALRATE[38:32]
         * <SLV-T>  04h     11h     [7:0]    8'h00        nomi2     OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>  04h     12h     [7:0]    8'h00        nomi3     OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>  04h     13h     [7:0]    8'h00        nomi4     OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>  04h     14h     [7:0]    8'h00        nomi5     OREG_TRCG_NOMINALRATE[7:0]
         * <SLV-T>  04h     15h     [3:0]    8'h00        8'h00     OREG_TRCG_LMTVAL0
         *
         * ----------+---------+---------+---------+---------+---------
         *  CLK mode |  nomi1  |  nomi2  |  nomi3  |  nomi4  |  nomi5
         * ----------+---------+---------+---------+---------+---------
         *       A,C |  8'h18  |  8'h00  |  8'h00  |  8'h00  |  8'h00
         * ----------+---------+---------+---------+---------+---------
         *         B |  8'h17  |  8'h55  |  8'h55  |  8'h55  |  8'h55
         * ----------+---------+---------+---------+---------+---------
         */
        {
            uint8_t dataAC[6] = {0x18, 0x00, 0x00, 0x00, 0x00, 0x00};
            uint8_t dataB[6]  = {0x17, 0x55, 0x55, 0x55, 0x55, 0x00};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, pData, 6) != SONY_RESULT_OK) {
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
         * ---------------------------------------------------------------------------------
         * <SLV-T>  04h     19h     [2:0]    8'h19        8'h3F     OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>  04h     1Ah     [2:0]    8'hD2        8'hFF     OREG_CDRB_GTDOFST[7:0]
         */
        {
            uint8_t data[2]  = {0x3F, 0xFF};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x19, data, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* SST setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>  04h     1Bh     [7:0]    8'h06        sst1      OREG_FWTG_AGCSST_OFST[15:8]
         * <SLV-T>  04h     1Ch     [7:0]    8'h2B        sst2      OREG_FWTG_AGCSST_OFST[7:0]
         *
         * -----------+----------+----------+
         *  CLK mode  |  sst1    |  sst2    |
         * -----------+----------+----------+
         *         A  |  8'h06   |  8'h23   |
         * -----------+----------+----------+
         *         B  |  8'h06   |  8'h22   |
         * -----------+----------+----------+
         *         C  |  8'h06   |  8'h21   |
         * -----------+----------+----------+
         */
        {
            uint8_t dataA[2] = {0x06, 0x23};
            uint8_t dataB[2] = {0x06, 0x22};
            uint8_t dataC[2] = {0x06, 0x21};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1B, pData, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* MRC setting(Diversity mode only)
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * ---------------------------------------------------------------------------------
         * <SLV-M>  04h     4Bh     [6:0]    8'h30        mrc8k1    OREG_MRC_TO_8K[14:8]
         * <SLV-M>  04h     4Ch     [7:0]    8'h00        mrc8k2    OREG_MRC_TO_8K[7:0]
         * <SLV-M>  04h     4Dh     [6:0]    8'h60        mrc16k1   OREG_MRC_TO_16K[15:8]
         * <SLV-M>  04h     4Eh     [7:0]    8'h00        mrc16k2   OREG_MRC_TO_16K[7:0]
         * <SLV-M>  04h     4Fh     [6:0]    8'h60        mrc32k1   OREG_MRC_TO_32K[15:8]
         * <SLV-M>  04h     50h     [7:0]    8'h00        mrc32k2   OREG_MRC_TO_32K[7:0]
         * <SLV-M>  04h     51h     [1:0]    8'h00        mrc_sym1  OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>  04h     52h     [7:0]    8'h90        mrc_sym2  OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>  04h     53h     [7:0]    8'h00        mrc_sym3  OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         *
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *  CLK mode  |  mrc8k1 | mrc8k2 | mrc16k1 | mrc16k2 | mrc32k1 | mrc32k2 | mrc_sym1| mrc_sym2| mrc_sym3|
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         A  |  8'h2D  | 8'hB6  |  8'h5B  |  8'h6D  |  8'h6D  |  8'hB6  |  8'h00  |  8'hA4  |  8'h92  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         B  |  8'h33  | 8'hDA  |  8'h67  |  8'hB4  |  8'h7C  |  8'h71  |  8'h00  |  8'hBA  |  8'hAA  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         C  |  8'h35  | 8'h55  |  8'h6A  |  8'hAA  |  8'h80  |  8'h00  |  8'h00  |  8'hC0  |  8'h00  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t dataA[9] = {0x2D, 0xB6, 0x5B, 0x6D, 0x6D, 0xB6, 0x00, 0xA4, 0x92};
            uint8_t dataB[9] = {0x33, 0xDA, 0x67, 0xB4, 0x7C, 0x71, 0x00, 0xBA, 0xAA};
            uint8_t dataC[9] = {0x35, 0x55, 0x6A, 0xAA, 0x80, 0x00, 0x00, 0xC0, 0x00};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, pData, 9) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    case SONY_DTV_BW_6_MHZ:
        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * --------------------------------------------------------------------------------
         * <SLV-T>  04h     10h     [6:0]    8'h15        nomi1     OREG_TRCG_NOMINALRATE[38:32]
         * <SLV-T>  04h     11h     [7:0]    8'h00        nomi2     OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>  04h     12h     [7:0]    8'h00        nomi3     OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>  04h     13h     [7:0]    8'h00        nomi4     OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>  04h     14h     [7:0]    8'h00        nomi5     OREG_TRCG_NOMINALRATE[7:0]
         * <SLV-T>  04h     15h     [3:0]    8'h00        8'h00     OREG_TRCG_LMTVAL0
         *
         * ----------+---------+---------+---------+---------+---------
         *  CLK mode |  nomi1  |  nomi2  |  nomi3  |  nomi4  |  nomi5
         * ----------+---------+---------+---------+---------+---------
         *      A,C  |  8'h1C  |  8'h00  |  8'h00  |  8'h00  |  8'h00
         * ----------+---------+---------+---------+---------+---------
         *        B  |  8'h1B  |  8'h38  |  8'hE3  |  8'h8E  |  8'h39
         * ----------+---------+---------+---------+---------+---------
         */
        {
            uint8_t dataAC[6] = {0x1C, 0x00, 0x00, 0x00, 0x00, 0x00};
            uint8_t dataB[6]  = {0x1B, 0x38, 0xE3, 0x8E, 0x39, 0x00};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, pData, 6) != SONY_RESULT_OK) {
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
         * ---------------------------------------------------------------------------------
         * <SLV-T>  04h     19h     [2:0]    8'h19        8'h3F     OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>  04h     1Ah     [2:0]    8'hD2        8'hFF     OREG_CDRB_GTDOFST[7:0]
         */
        {
            uint8_t data[2]  = {0x3F, 0xFF};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x19, data, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* SST setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>  04h     1Bh     [7:0]    8'h06        sst1      OREG_FWTG_AGCSST_OFST[15:8]
         * <SLV-T>  04h     1Ch     [7:0]    8'h2B        sst2      OREG_FWTG_AGCSST_OFST[7:0]
         *
         * -----------+----------+----------+
         *  CLK mode  |  sst1    |  sst2    |
         * -----------+----------+----------+
         *         A  |  8'h06   |  8'h1C   |
         * -----------+----------+----------+
         *         B  |  8'h06   |  8'h1B   |
         * -----------+----------+----------+
         *         C  |  8'h06   |  8'h1A   |
         * -----------+----------+----------+
         */
        {
            uint8_t dataA[2] = {0x06, 0x1C};
            uint8_t dataB[2] = {0x06, 0x1B};
            uint8_t dataC[2] = {0x06, 0x1A};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1B, pData, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* MRC setting(Diversity mode only)
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * ---------------------------------------------------------------------------------
         * <SLV-M>  04h     4Bh     [6:0]    8'h30        mrc8k1    OREG_MRC_TO_8K[14:8]
         * <SLV-M>  04h     4Ch     [7:0]    8'h00        mrc8k2    OREG_MRC_TO_8K[7:0]
         * <SLV-M>  04h     4Dh     [6:0]    8'h60        mrc16k1   OREG_MRC_TO_16K[15:8]
         * <SLV-M>  04h     4Eh     [7:0]    8'h00        mrc16k2   OREG_MRC_TO_16K[7:0]
         * <SLV-M>  04h     4Fh     [6:0]    8'h60        mrc32k1   OREG_MRC_TO_32K[15:8]
         * <SLV-M>  04h     50h     [7:0]    8'h00        mrc32k2   OREG_MRC_TO_32K[7:0]
         * <SLV-M>  04h     51h     [1:0]    8'h00        mrc_sym1  OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>  04h     52h     [7:0]    8'h90        mrc_sym2  OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>  04h     53h     [7:0]    8'h00        mrc_sym3  OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         *
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *  CLK mode  |  mrc8k1 | mrc8k2 | mrc16k1 | mrc16k2 | mrc32k1 | mrc32k2 | mrc_sym1| mrc_sym2| mrc_sym3|
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         A  |  8'h35  | 8'h55  |  8'h6A  |  8'hAA  |  8'h80  |  8'h00  |  8'h00  |  8'hC0  |  8'h00  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         B  |  8'h3C  | 8'h7E  |  8'h78  |  8'hFC  |  8'h91  |  8'h2F  |  8'h00  |  8'hD9  |  8'hC7  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         C  |  8'h3E  | 8'h38  |  8'h7C  |  8'h71  |  8'h95  |  8'h55  |  8'h00  |  8'hDF  |  8'hFF  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t dataA[9] = {0x35, 0x55, 0x6A, 0xAA, 0x80, 0x00, 0x00, 0xC0, 0x00};
            uint8_t dataB[9] = {0x3C, 0x7E, 0x78, 0xFC, 0x91, 0x2F, 0x00, 0xD9, 0xC7};
            uint8_t dataC[9] = {0x3E, 0x38, 0x7C, 0x71, 0x95, 0x55, 0x00, 0xDF, 0xFF};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, pData, 9) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    case SONY_DTV_BW_5_MHZ:
        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * --------------------------------------------------------------------------------
         * <SLV-T>  04h     10h     [6:0]    8'h15        nomi1     OREG_TRCG_NOMINALRATE[38:32]
         * <SLV-T>  04h     11h     [7:0]    8'h00        nomi2     OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>  04h     12h     [7:0]    8'h00        nomi3     OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>  04h     13h     [7:0]    8'h00        nomi4     OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>  04h     14h     [7:0]    8'h00        nomi5     OREG_TRCG_NOMINALRATE[7:0]
         * <SLV-T>  04h     15h     [3:0]    8'h00        8'h00     OREG_TRCG_LMTVAL0
         *
         * ----------+---------+---------+---------+---------+---------
         *  CLK mode |  nomi1  |  nomi2  |  nomi3  |  nomi4  |  nomi5
         * ----------+---------+---------+---------+---------+---------
         *      A,C  |  8'h21  |  8'h99  |  8'h99  |  8'h99  |  8'h9A
         * ----------+---------+---------+---------+---------+---------
         *        B  |  8'h20  |  8'hAA  |  8'hAA  |  8'hAA  |  8'hAB
         * ----------+---------+---------+---------+---------+---------
         */
        {
            uint8_t dataAC[6] = {0x21, 0x99, 0x99, 0x99, 0x9A, 0x00};
            uint8_t dataB[6]  = {0x20, 0xAA, 0xAA, 0xAA, 0xAB, 0x00};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, pData, 6) != SONY_RESULT_OK) {
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
         * ---------------------------------------------------------------------------------
         * <SLV-T>  04h     19h     [2:0]    8'h19        8'h3F     OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>  04h     1Ah     [2:0]    8'hD2        8'hFF     OREG_CDRB_GTDOFST[7:0]
         */
        {
            uint8_t data[2]  = {0x3F, 0xFF};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x19, data, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* SST setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>  04h     1Bh     [7:0]    8'h06        sst1      OREG_FWTG_AGCSST_OFST[15:8]
         * <SLV-T>  04h     1Ch     [7:0]    8'h2B        sst2      OREG_FWTG_AGCSST_OFST[7:0]
         *
         * -----------+----------+----------+
         *  CLK mode  |  sst1    |  sst2    |
         * -----------+----------+----------+
         *         A  |  8'h06   |  8'h15   |
         * -----------+----------+----------+
         *         B  |  8'h06   |  8'h15   |
         * -----------+----------+----------+
         *         C  |  8'h06   |  8'h14   |
         * -----------+----------+----------+
         */
        {
            uint8_t dataA[2] = {0x06, 0x15};
            uint8_t dataB[2] = {0x06, 0x15};
            uint8_t dataC[2] = {0x06, 0x14};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1B, pData, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* MRC setting(Diversity mode only)
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * ---------------------------------------------------------------------------------
         * <SLV-M>  04h     4Bh     [6:0]    8'h30        mrc8k1    OREG_MRC_TO_8K[14:8]
         * <SLV-M>  04h     4Ch     [7:0]    8'h00        mrc8k2    OREG_MRC_TO_8K[7:0]
         * <SLV-M>  04h     4Dh     [6:0]    8'h60        mrc16k1   OREG_MRC_TO_16K[15:8]
         * <SLV-M>  04h     4Eh     [7:0]    8'h00        mrc16k2   OREG_MRC_TO_16K[7:0]
         * <SLV-M>  04h     4Fh     [6:0]    8'h60        mrc32k1   OREG_MRC_TO_32K[15:8]
         * <SLV-M>  04h     50h     [7:0]    8'h00        mrc32k2   OREG_MRC_TO_32K[7:0]
         * <SLV-M>  04h     51h     [1:0]    8'h00        mrc_sym1  OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>  04h     52h     [7:0]    8'h90        mrc_sym2  OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>  04h     53h     [7:0]    8'h00        mrc_sym3  OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         *
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *  CLK mode  |  mrc8k1 | mrc8k2 | mrc16k1 | mrc16k2 | mrc32k1 | mrc32k2 | mrc_sym1| mrc_sym2| mrc_sym3|
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         A  |  8'h40  | 8'h00  |  8'h6A  |  8'hAA  |  8'h80  |  8'h00  |  8'h00  |  8'hE6  |  8'h66  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         B  |  8'h48  | 8'h97  |  8'h78  |  8'hFC  |  8'h91  |  8'h2F  |  8'h01  |  8'h05  |  8'h55  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         C  |  8'h4A  | 8'hAA  |  8'h7C  |  8'h71  |  8'h95  |  8'h55  |  8'h01  |  8'h0C  |  8'hCC  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t dataA[9] = {0x40, 0x00, 0x6A, 0xAA, 0x80, 0x00, 0x00, 0xE6, 0x66};
            uint8_t dataB[9] = {0x48, 0x97, 0x78, 0xFC, 0x91, 0x2F, 0x01, 0x05, 0x55};
            uint8_t dataC[9] = {0x4A, 0xAA, 0x7C, 0x71, 0x95, 0x55, 0x01, 0x0C, 0xCC};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, pData, 9) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    case SONY_DTV_BW_1_7_MHZ:
        /* Timing Recovery setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * --------------------------------------------------------------------------------
         * <SLV-T>  04h     10h     [6:0]    8'h15        nomi1     OREG_TRCG_NOMINALRATE[38:32]
         * <SLV-T>  04h     11h     [7:0]    8'h00        nomi2     OREG_TRCG_NOMINALRATE[31:24]
         * <SLV-T>  04h     12h     [7:0]    8'h00        nomi3     OREG_TRCG_NOMINALRATE[23:16]
         * <SLV-T>  04h     13h     [7:0]    8'h00        nomi4     OREG_TRCG_NOMINALRATE[15:8]
         * <SLV-T>  04h     14h     [7:0]    8'h00        nomi5     OREG_TRCG_NOMINALRATE[7:0]
         * <SLV-T>  04h     15h     [3:0]    8'h00        8'h03     OREG_TRCG_LMTVAL0
         *
         * ----------+---------+---------+---------+---------+---------+
         * CLK mode  |  nomi1  |  nomi2  |  nomi3  |  nomi4  |  nomi5  |
         * ----------+---------+---------+---------+---------+---------+
         *        A  |  8'h68  |  8'h0F  |  8'hA2  |  8'h32  |  8'hCF  |
         * ----------+---------+---------+---------+---------+---------+
         *        C  |  8'h68  |  8'h0F  |  8'hA2  |  8'h32  |  8'hCF  |
         * ----------+---------+---------+---------+---------+---------+
         *        B  |  8'h65  |  8'h2B  |  8'hA4  |  8'hCD  |  8'hD8  |
         * ----------+---------+---------+---------+---------+---------+
         */
        {
            uint8_t dataA[6] = {0x68, 0x0F, 0xA2, 0x32, 0xCF, 0x03};
            uint8_t dataC[6] = {0x68, 0x0F, 0xA2, 0x32, 0xCF, 0x03};
            uint8_t dataB[6] = {0x65, 0x2B, 0xA4, 0xCD, 0xD8, 0x03};
            uint8_t * pData = NULL;

            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
                pData = dataA;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_C:
                pData = dataC;
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                pData = dataB;
                break;
            default:
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
            }

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, pData, 6) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* System Bandwidth setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * -----------------------------------------------------------------------------------------
         * <SLV-T>  04h     4Ah     [2:0]    8'h00        8'h03     OREG_CHANNEL_WIDTH[2:0]
         */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4A, 0x03) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Demod core latency setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>  04h     19h     [2:0]    8'h19        8'h3F     OREG_CDRB_GTDOFST[13:8]
         * <SLV-T>  04h     1Ah     [2:0]    8'hD2        8'hFF     OREG_CDRB_GTDOFST[7:0]
         */
        {
            uint8_t data[2]  = {0x3F, 0xFF};

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x19, data, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* SST setting
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * ---------------------------------------------------------------------------------
         * <SLV-T>  04h     1Bh     [7:0]    8'h06        sst1      OREG_FWTG_AGCSST_OFST[15:8]
         * <SLV-T>  04h     1Ch     [7:0]    8'h2B        sst2      OREG_FWTG_AGCSST_OFST[7:0]
         *
         * -----------+----------+----------+
         *  CLK mode  |  sst1    |  sst2    |
         * -----------+----------+----------+
         *         A  |  8'h06   |  8'h0C   |
         * -----------+----------+----------+
         *         B  |  8'h06   |  8'h0C   |
         * -----------+----------+----------+
         *         C  |  8'h06   |  8'h0B   |
         * -----------+----------+----------+
         */
        {
            uint8_t dataA[2] = {0x06, 0x0C};
            uint8_t dataB[2] = {0x06, 0x0C};
            uint8_t dataC[2] = {0x06, 0x0B};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x1B, pData, 2) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        /* MRC setting(Diversity mode only)
         *
         * slave    Bank    Addr    Bit     default      Value        Name
         * ---------------------------------------------------------------------------------
         * <SLV-M>  04h     4Bh     [6:0]    8'h30        mrc8k1    OREG_MRC_TO_8K[14:8]
         * <SLV-M>  04h     4Ch     [7:0]    8'h00        mrc8k2    OREG_MRC_TO_8K[7:0]
         * <SLV-M>  04h     4Dh     [6:0]    8'h60        mrc16k1   OREG_MRC_TO_16K[15:8]
         * <SLV-M>  04h     4Eh     [7:0]    8'h00        mrc16k2   OREG_MRC_TO_16K[7:0]
         * <SLV-M>  04h     4Fh     [6:0]    8'h60        mrc32k1   OREG_MRC_TO_32K[15:8]
         * <SLV-M>  04h     50h     [7:0]    8'h00        mrc32k2   OREG_MRC_TO_32K[7:0]
         * <SLV-M>  04h     51h     [1:0]    8'h00        mrc_sym1  OREG_MRC_DATA_SYMBOL_CK_8K[17:16]
         * <SLV-M>  04h     52h     [7:0]    8'h90        mrc_sym2  OREG_MRC_DATA_SYMBOL_CK_8K[15:8]
         * <SLV-M>  04h     53h     [7:0]    8'h00        mrc_sym3  OREG_MRC_DATA_SYMBOL_CK_8K[7:0]
         *
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *  CLK mode  |  mrc8k1 | mrc8k2 | mrc16k1 | mrc16k2 | mrc32k1 | mrc32k2 | mrc_sym1| mrc_sym2| mrc_sym3|
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         A  |  8'h40  | 8'h00  |  8'h6A  |  8'hAA  |  8'h80  |  8'h00  |  8'h02  |  8'hC9  |  8'h8F  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         B  |  8'h48  | 8'h97  |  8'h78  |  8'hFC  |  8'h91  |  8'h2F  |  8'h03  |  8'h29  |  8'h5D  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         *         C  |  8'h4A  | 8'hAA  |  8'h7C  |  8'h71  |  8'h95  |  8'h55  |  8'h03  |  8'h40  |  8'h7D  |
         * -----------+---------+--------+---------+---------+---------+---------+---------+---------+---------+
         */
        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            uint8_t dataA[9] = {0x40, 0x00, 0x6A, 0xAA, 0x80, 0x00, 0x02, 0xC9, 0x8F};
            uint8_t dataB[9] = {0x48, 0x97, 0x78, 0xFC, 0x91, 0x2F, 0x03, 0x29, 0x5D};
            uint8_t dataC[9] = {0x4A, 0xAA, 0x7C, 0x71, 0x95, 0x55, 0x03, 0x40, 0x7D};
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

            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, pData, 9) != SONY_RESULT_OK) {
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

static sony_result_t X_sleep_dvbt2_DemodSetting (sony_tunerdemod_t * pTunerDemod)
{
    SONY_TRACE_ENTER ("X_sleep_dvbt2_DemodSetting");

    /* (Diversity mode only)
     * Slave    Bank    Addr    Bit     Default   Value      Name
     *----------------------------------------------------------------------------------------
     * <SLV-M>  1Dh     47h     [7:0]   8'd0      8'd0       OREG_BUF_DIFINT_CLIP_TH_1K[15:8]
     * <SLV-M>  1Dh     48h     [7:0]   8'd1      8'd1       OREG_BUF_DIFINT_CLIP_TH_1K[7:0]
     * <SLV-M>  1Dh     49h     [7:0]   8'd0      8'd0       OREG_BUF_DIFINT_CLIP_TH_2K[15:8]
     * <SLV-M>  1Dh     4Ah     [7:0]   8'd2      8'd2       OREG_BUF_DIFINT_CLIP_TH_2K[7:0]
     * <SLV-M>  1Dh     4Bh     [7:0]   8'd0      8'd0       OREG_BUF_DIFINT_CLIP_TH_4K[15:8]
     * <SLV-M>  1Dh     4Ch     [7:0]   8'd4      8'd4       OREG_BUF_DIFINT_CLIP_TH_4K[7:0]
     * <SLV-M>  1Dh     4Dh     [7:0]   8'd0      8'd0       OREG_BUF_DIFINT_CLIP_TH_8K[15:8]
     * <SLV-M>  1Dh     4Eh     [7:0]   8'd8      8'd8       OREG_BUF_DIFINT_CLIP_TH_8K[7:0]
     * <SLV-M>  1Dh     4Fh     [7:0]   8'd0      8'd0       OREG_BUF_DIFINT_CLIP_TH_16K[15:8]
     * <SLV-M>  1Dh     50h     [7:0]   8'd16     8'd16      OREG_BUF_DIFINT_CLIP_TH_16K[7:0]
     * <SLV-M>  1Dh     51h     [7:0]   8'd0      8'd0       OREG_BUF_DIFINT_CLIP_TH_32K[15:8]
     * <SLV-M>  1Dh     52h     [7:0]   8'd32     8'd32      OREG_BUF_DIFINT_CLIP_TH_32K[7:0]
     */
    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        uint8_t data[] = {0, 1, 0, 2, 0, 4, 0, 8, 0, 16, 0, 32};

        /* Set SLV-T Bank : 0x1D */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x1D) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x47, data, 12) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t dvbt2_SetProfile (sony_tunerdemod_t * pTunerDemod,
                                       sony_dvbt2_profile_t profile)
{
    uint8_t t2Mode_tuneMode = 0;
    uint8_t seqNot2Dtime = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("dvbt2_SetProfile");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    {
        uint8_t dtime1 = 0;
        uint8_t dtime2 = 0;

        switch (pTunerDemod->clockMode) {
        case SONY_TUNERDEMOD_CLOCKMODE_A:
            dtime1 = 0x27;
            dtime2 = 0x0C;
            break;
        case SONY_TUNERDEMOD_CLOCKMODE_B:
            dtime1 = 0x2C;
            dtime2 = 0x0D;
            break;
        case SONY_TUNERDEMOD_CLOCKMODE_C:
            dtime1 = 0x2E;
            dtime2 = 0x0E;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        switch (profile) {
        case SONY_DVBT2_PROFILE_BASE:
            t2Mode_tuneMode = 0x01; /* Set profile to DVB-T2 base without recovery */
            seqNot2Dtime = dtime2; /* Set early unlock time */
            break;

        case SONY_DVBT2_PROFILE_LITE:
            t2Mode_tuneMode = 0x05; /* Set profile to DVB-T2 lite without recovery */
            seqNot2Dtime = dtime1; /* Set early unlock time */
            break;

        case SONY_DVBT2_PROFILE_ANY:
            t2Mode_tuneMode = 0x00; /* Set profile to auto detection */
            seqNot2Dtime = dtime1; /* Set early unlock time */
            break;

        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
        }
    }

    /* Set SLV-T Bank : 0x2E */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x2E) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Set profile and tune mode
     * slave    Bank    Addr    Bit       default   Value       Name
     * ---------------------------------------------------------------------------------------
     * <SLV-T>  2Eh     10h     [2:0]     8'h01     8'hxx      OREG_T2MODE,OREG_TUNEMODE[1:0]
     */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, t2Mode_tuneMode) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Set SLV-T Bank : 0x04 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x04) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Set early unlock detection time
     * slave    Bank    Addr    Bit      default   Value     Name
     * -------------------------------------------------------------------------------
     * <SLV-T>  04h     2Ch     [7:0]    8'd0C     8'dxx     OREG_SEQ_NOT2_DTIME[15:8]
     */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x2C, seqNot2Dtime) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (result);
}

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tunerdemod_dvbt2_Tune1 (sony_tunerdemod_t * pTunerDemod,
                                           sony_dvbt2_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_Tune1");

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

    if ((pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) && (pTuneParam->profile == SONY_DVBT2_PROFILE_ANY)) {
        /* In diver, T2-base/T2-Lite auto setting is unavailable. */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* System independent settings */
    result = sony_tunerdemod_CommonTuneSetting1 (pTunerDemod, SONY_DTV_SYSTEM_DVBT2, pTuneParam->centerFreqKHz, pTuneParam->bandwidth, 0, 0);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* DVB-T dependent settings */
    result = X_tune_dvbt2_DemodSetting (pTunerDemod, pTuneParam->bandwidth, pTunerDemod->clockMode);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = X_tune_dvbt2_DemodSetting (pTunerDemod->pDiverSub, pTuneParam->bandwidth, pTunerDemod->pDiverSub->clockMode);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* Set Profile */
    result = dvbt2_SetProfile (pTunerDemod, pTuneParam->profile);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = dvbt2_SetProfile (pTunerDemod->pDiverSub, pTuneParam->profile);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* PLP configuration */
    if (pTuneParam->dataPLPID == SONY_DVBT2_TUNE_PARAM_PLPID_AUTO) {
        result = sony_tunerdemod_dvbt2_SetPLPConfig (pTunerDemod, 1, 0);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    } else {
        result = sony_tunerdemod_dvbt2_SetPLPConfig (pTunerDemod, 0, (uint8_t)(pTuneParam->dataPLPID));
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_Tune2 (sony_tunerdemod_t * pTunerDemod,
                                           sony_dvbt2_tune_param_t * pTuneParam)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_Tune2");

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
    {
        uint8_t enableFEFIntermittentControl = 1;

        switch (pTuneParam->profile) {
        case SONY_DVBT2_PROFILE_BASE:
            enableFEFIntermittentControl = pTunerDemod->enableFEFIntermittentBase;
            break;
        case SONY_DVBT2_PROFILE_LITE:
            enableFEFIntermittentControl = pTunerDemod->enableFEFIntermittentLite;
            break;
        case SONY_DVBT2_PROFILE_ANY:
            if (pTunerDemod->enableFEFIntermittentBase && pTunerDemod->enableFEFIntermittentLite) {
                enableFEFIntermittentControl = 1;
            } else {
                enableFEFIntermittentControl = 0;
            }
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
        }

        result = sony_tunerdemod_CommonTuneSetting2 (pTunerDemod, SONY_DTV_SYSTEM_DVBT2, enableFEFIntermittentControl);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    pTunerDemod->state = SONY_TUNERDEMOD_STATE_ACTIVE;
    pTunerDemod->frequencyKHz = pTuneParam->centerFreqKHz;
    pTunerDemod->system = SONY_DTV_SYSTEM_DVBT2;
    pTunerDemod->bandwidth = pTuneParam->bandwidth;

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        pTunerDemod->pDiverSub->state = SONY_TUNERDEMOD_STATE_ACTIVE;
        pTunerDemod->pDiverSub->frequencyKHz = pTuneParam->centerFreqKHz;
        pTunerDemod->pDiverSub->system = SONY_DTV_SYSTEM_DVBT2;
        pTunerDemod->pDiverSub->bandwidth = pTuneParam->bandwidth;
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_SleepSetting (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_SleepSetting");

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

    result = X_sleep_dvbt2_DemodSetting (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = X_sleep_dvbt2_DemodSetting (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_CheckDemodLock (sony_tunerdemod_t * pTunerDemod,
                                                    sony_tunerdemod_lock_result_t * pLock)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t syncStat = 0;
    uint8_t tsLock = 0;
    uint8_t unlockDetected = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_CheckDemodLock");

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

    result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncStat, &tsLock, &unlockDetected);
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

        result = sony_tunerdemod_dvbt2_monitor_SyncStat_sub (pTunerDemod, &syncStat, &unlockDetectedSub);
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

sony_result_t sony_tunerdemod_dvbt2_CheckTSLock (sony_tunerdemod_t * pTunerDemod,
                                                 sony_tunerdemod_lock_result_t * pLock)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t syncStat = 0;
    uint8_t tsLock = 0;
    uint8_t unlockDetected = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_CheckTSLock");

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

    result = sony_tunerdemod_dvbt2_monitor_SyncStat (pTunerDemod, &syncStat, &tsLock, &unlockDetected);
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

        result = sony_tunerdemod_dvbt2_monitor_SyncStat_sub (pTunerDemod, &syncStat, &unlockDetectedSub);
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

sony_result_t sony_tunerdemod_dvbt2_SetPLPConfig (sony_tunerdemod_t * pTunerDemod,
                                                  uint8_t autoPLP,
                                                  uint8_t plpId)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_SetPLPConfig");

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

    /* Set SLV-T Bank : 0x23 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x23) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (!autoPLP) {
        /* Manual PLP selection mode. Set the data PLP Id. */
        /* Slave    Bank    Addr    Bit     Default   Value      Name
         * --------------------------------------------------------------------------
         * <SLV-M>   23h     AFh    [7:0]   8'h00     8'h**      OREGD_FP_PLP_ID[7:0]
         */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xAF, plpId) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* Auto PLP select (Scanning mode = 0x00). Data PLP select = 0x01. */
    /* Slave    Bank    Addr    Bit     Default   Value      Name
     * --------------------------------------------------------------------------------
     * <SLV-M>   23h     ADh    [1:0]   8'h00     8'h01      OREGD_FP_PLP_AUTO_SEL[1:0]
     */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xAD, autoPLP? 0x00 : 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_dvbt2_DiverFEFSetting (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_DiverFEFSetting");

    if (!pTunerDemod) {
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

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SINGLE) {
        /* No need for single case */
        SONY_TRACE_RETURN (SONY_RESULT_OK);
    }

    {
        sony_dvbt2_ofdm_t ofdm;

        result = sony_tunerdemod_dvbt2_monitor_OFDM (pTunerDemod, &ofdm);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (!ofdm.mixed) {
            /* If no FEF, no need to change setting. */
            SONY_TRACE_RETURN (SONY_RESULT_OK);
        }
    }

    /* Slave    Bank    Addr    Bit     Default   Value      Name
     *--------------------------------------------------------------------------------------
     * <SLV-M>  1Dh     47h     [7:0]   8'd0      8'd0       OREG_BUF_DIFINT_CLIP_TH_1K[15:8]
     * <SLV-M>  1Dh     48h     [7:0]   8'd1      8'd8       OREG_BUF_DIFINT_CLIP_TH_1K[7:0]
     * <SLV-M>  1Dh     49h     [7:0]   8'd0      8'd0       OREG_BUF_DIFINT_CLIP_TH_2K[15:8]
     * <SLV-M>  1Dh     4Ah     [7:0]   8'd2      8'd16      OREG_BUF_DIFINT_CLIP_TH_2K[7:0]
     * <SLV-M>  1Dh     4Bh     [7:0]   8'd0      8'd0       OREG_BUF_DIFINT_CLIP_TH_4K[15:8]
     * <SLV-M>  1Dh     4Ch     [7:0]   8'd4      8'd32      OREG_BUF_DIFINT_CLIP_TH_4K[7:0]
     * <SLV-M>  1Dh     4Dh     [7:0]   8'd0      8'd0       OREG_BUF_DIFINT_CLIP_TH_8K[15:8]
     * <SLV-M>  1Dh     4Eh     [7:0]   8'd8      8'd64      OREG_BUF_DIFINT_CLIP_TH_8K[7:0]
     * <SLV-M>  1Dh     4Fh     [7:0]   8'd0      8'd0       OREG_BUF_DIFINT_CLIP_TH_16K[15:8]
     * <SLV-M>  1Dh     50h     [7:0]   8'd16     8'd128     OREG_BUF_DIFINT_CLIP_TH_16K[7:0]
     * <SLV-M>  1Dh     51h     [7:0]   8'd0      8'd1       OREG_BUF_DIFINT_CLIP_TH_32K[15:8]
     * <SLV-M>  1Dh     52h     [7:0]   8'd32     8'd0       OREG_BUF_DIFINT_CLIP_TH_32K[7:0]
     */
    {
        uint8_t data[] = {0, 8, 0, 16, 0, 32, 0, 64, 0, 128, 1, 0};

        /* Set SLV-T Bank : 0x1D */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x1D) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x47, data, 12) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_dvbt2_CheckL1PostValid (sony_tunerdemod_t * pTunerDemod,
                                                      uint8_t * pL1PostValid)
{
    sony_result_t result = SONY_RESULT_OK;

    uint8_t data;

    SONY_TRACE_ENTER ("sony_tunerdemod_dvbt2_CheckL1PostValid");

    if ((!pTunerDemod) || (!pL1PostValid)) {
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

    /* Set SLV-T Bank : 0x0B */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x0B) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* slave    Bank    Addr    Bit           Name                meaning
     * ---------------------------------------------------------------------------------
     * <SLV-M>  0Bh     86h     [0]     IL1POST_OK             0:invalid, 1:valid
     */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x86, &data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    *pL1PostValid = data & 0x01;

    SONY_TRACE_RETURN (result);
}
