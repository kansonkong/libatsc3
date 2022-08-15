/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/12/13
  Modification ID : e402a4dcb0e51a00c34b174334c082c1211eebf1
------------------------------------------------------------------------------*/

#include "sony_cxd6801_demod_atsc3_monitor.h"
#include "sony_cxd6801_math.h"
#include "sony_cxd6801_stdlib.h"

#define SONY_CXD6801_ATSC3_WAIT_L1_DETAIL_TIMEOUT  100 /**< 100ms timeout for waiting L1 information update */
#define SONY_CXD6801_ATSC3_WAIT_L1_DETAIL_TIMEOUT2 100 /**< 100ms timeout for ready to read L1 information */
#define SONY_CXD6801_ATSC3_WAIT_L1_DETAIL_INTERVAL 10  /**< 10ms polling interval for ready to read L1 information */
#define CXD6801_L1RAM_READ_SIZE 240 /**< Maximum size that can be read from L1RAM at once */

/*------------------------------------------------------------------------------
    Static Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief Confirm demodulator lock
*/
static sony_cxd6801_result_t cxd6801_isDemodLocked(sony_cxd6801_demod_t * pDemod);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_SyncStat(sony_cxd6801_demod_t * pDemod,
                                                 uint8_t * pSyncStat,
                                                 uint8_t alpLockStat[4],
                                                 uint8_t * pALPLockAll,
                                                 uint8_t * pUnlockDetected)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_SyncStat");

    if ((!pDemod) || (!pSyncStat) || (!alpLockStat) || (!pALPLockAll) || (!pUnlockDetected)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data;
        /* Set SLV-T Bank : 0x90 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        /*  slave     Bank    Addr    Bit          Name              Meaning
         * -------------------------------------------------------------------
         *  <SLV-T>   90h     10h     [2:0]        IREG_SEQ_STATE     0-5:UNLOCK 6:LOCK
         *  <SLV-T>   90h     10h     [4]          IREG_SEQ_NOOFDM    1 ATSC 3.0 signal not exist
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, &data, 1) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        *pSyncStat = data & 0x07;
        *pUnlockDetected = (data & 0x10) ? 1 : 0;

        /* Set SLV-T Bank : 0x95 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x95) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /*  slave     Bank    Addr    Bit          Name                  Meaning
         * -------------------------------------------------------------------
         *  <SLV-T>   95h     40h     [5]          IREG_LOCK_ALPLOCK_OR  0:UNLOCK, 1:LOCK
         *  <SLV-T>   95h     40h     [4]          IREG_LOCK_ALPLOCK_AND 0:UNLOCK, 1:LOCK
         *  <SLV-T>   95h     40h     [3]          IREG_LOCK3_ALPLOCK    0:UNLOCK, 1:LOCK
         *  <SLV-T>   95h     40h     [2]          IREG_LOCK2_ALPLOCK    0:UNLOCK, 1:LOCK
         *  <SLV-T>   95h     40h     [1]          IREG_LOCK1_ALPLOCK    0:UNLOCK, 1:LOCK
         *  <SLV-T>   95h     40h     [0]          IREG_LOCK0_ALPLOCK    0:UNLOCK, 1:LOCK
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x40, &data, 1) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        *pALPLockAll = (data & 0x10) ? 1 : 0;
        alpLockStat[0] = (data & 0x01) ? 1 : 0;
        alpLockStat[1] = (data & 0x02) ? 1 : 0;
        alpLockStat[2] = (data & 0x04) ? 1 : 0;
        alpLockStat[3] = (data & 0x08) ? 1 : 0;
    }

    /* Check for valid SyncStat value. */
    if (*pSyncStat == 0x07) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_CarrierOffset(sony_cxd6801_demod_t * pDemod,
                                                      int32_t * pOffset)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint8_t sinv;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_CarrierOffset");

    if ((!pDemod) || (!pOffset)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[4];
        uint32_t ctlVal = 0;

        /* Freeze registers */
		if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		result = cxd6801_isDemodLocked(pDemod);
		if (result != SONY_CXD6801_RESULT_OK) {
			CXD6801_SLVT_UnFreezeReg(pDemod);
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Set SLV-T Bank : 0x90 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
			CXD6801_SLVT_UnFreezeReg(pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /*  slave     Bank    Addr    Bit        Name
         * ---------------------------------------------------------------
         *  <SLV-T>   90h     4Ch     [3:0]      IREG_CRCG_CTLVAL[27:24]
         *  <SLV-T>   90h     4Dh     [7:0]      IREG_CRCG_CTLVAL[23:16]
         *  <SLV-T>   90h     4Eh     [7:0]      IREG_CRCG_CTLVAL[15:8]
         *  <SLV-T>   90h     4Fh     [7:0]      IREG_CRCG_CTLVAL[7:0]
         */

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4C, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
			CXD6801_SLVT_UnFreezeReg(pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /*  slave     Bank    Addr    Bit          Name           Meaning
         * ---------------------------------------------------------------------------------
         *  <SLV-T>   90h     F3h     [0]          OREG_SINV      0:not invert,   1:invert
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF3, &sinv, 1) != SONY_CXD6801_RESULT_OK) {
			CXD6801_SLVT_UnFreezeReg(pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		CXD6801_SLVT_UnFreezeReg(pDemod);

        /*
         * For 8,7,6MHzBW calculation:
         *    Carrier Offset [Hz] = (IREG_CRCG_CTLVAL * (2^-30) * ( 0.384 * (16+bsr_coefficient)) * 10^6
         *    bsr_coefficient = 8 (at OREG_CHANNEL_WIDTH==0)
         *                   or 5 (at OREG_CHANNEL_WIDTH==2)
         *                   or 2 (at OREG_CHANNEL_WIDTH==4).
         *
         *    bsr_coefficient = 8 : (2^-30) * (0.384 * 24) * 10^6 = 0.008583
         *                          and 1 / ((2^-30) * (0.384 * 24) * 10^6) = 116.50
         *                          Carrier Offset = (IREG_CRCG_CTLVAL) / 117
         *    bsr_coefficient = 5 : (2^-30) * (0.384 * 21) * 10^6 = 0.00751
         *                          and 1 / ((2^-30) * (0.384 * 21) * 10^6) = 133.15
         *                          Carrier Offset = (IREG_CRCG_CTLVAL) / 133
         *    bsr_coefficient = 2 : (2^-30) * (0.384 * 18) * 10^6 = 0.006437
         *                          and 1 / ((2^-30) * (0.384 * 18) * 10^6) = 155.35
         *                          Carrier Offset = (IREG_CRCG_CTLVAL) / 155
         *
         *
         */
        ctlVal = ((data[0] & 0x0F) << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
		*pOffset = sony_cxd6801_Convert2SComplement(ctlVal, 28);

        switch (pDemod->bandwidth) {
		case SONY_CXD6801_DTV_BW_8_MHZ:
            *pOffset = (*pOffset) / 117;
            break;
		case SONY_CXD6801_DTV_BW_7_MHZ:
            *pOffset = (*pOffset) / 133;
            break;
		case SONY_CXD6801_DTV_BW_6_MHZ:
            *pOffset = (*pOffset) / 155;
            break;
        default:
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
        }
    }

    /* Compensate for inverted spectrum. */
    {
		if (pDemod->confSense == SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_NORMAL) {
            if (!(sinv & 0x01)) {
                *pOffset *= -1;
            }
        } else {
            if (sinv & 0x01) {
                *pOffset *= -1;
            }
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_IFAGCOut(sony_cxd6801_demod_t * pDemod,
                                                 uint32_t * pIFAGC)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_IFAGCOut");


    if ((!pDemod) || (!pIFAGC)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x90 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr    Bit          Name
     * --------------------------------------------------------
     *  <SLV-T>   90h     26h     [3:0]        IIFAGC_OUT[11:8]
     *  <SLV-T>   90h     27h     [7:0]        IIFAGC_OUT[7:0]
     */
    {
        uint8_t data[2];
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x26, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        *pIFAGC = ((data[0] & 0x0F) << 8) | (data[1]);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_Bootstrap(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_bootstrap_t * pBootstrap)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_Bootstrap");

    if ((!pDemod) || (!pBootstrap)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[3];

        /* Freeze registers */
		if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Set SLV-T Bank : 0x90 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /*
         *  slave     Bank    Addr   Bit          Name
         * ------------------------------------------------------------
         *  <SLV-T>   90h     E6h    [0]          IREG_BS_LOCK
         */

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE6, data, 1) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        if (!(data[0] & 0x01)) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }

        /*
         *  slave     Bank    Addr   Bit          Name                  Meaning
         * ---------------------------------------------------------------------------------
         *  <SLV-T>   90h     E0h    [0]          IREG_BS_BW_DIFF       **1
         *  <SLV-T>   90h     E1h    [1:0]        IREG_BS_SYSTEM_BW     2'd0:6MHZ 2'd1:7MHz 2'd2:8MHZ 2'd3:>8MHz
         *  <SLV-T>   90h     E2h    [1:0]        IREG_BS_EAWAKEUP      emergency alert wake up
         *
         * **1( OREG_CHANNEL_WIDTH==3'd4 ) && (IREG_SYSTEM_BW != 2'd0 ) ||
         *    ( OREG_CHANNEL_WIDTH==3'd2 ) && (IREG_SYSTEM_BW != 2'd1 ) ||
         *    ( OREG_CHANNEL_WIDTH==3'd0 ) && (IREG_SYSTEM_BW != 2'd2 )
         */

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE0, data, 3) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        CXD6801_SLVT_UnFreezeReg (pDemod);

        pBootstrap->bw_diff = data[0] & 0x01;
		pBootstrap->system_bw = (sony_cxd6801_atsc3_system_bw_t)(data[1] & 0x03);
        pBootstrap->ea_wake_up = data[2] & 0x03;
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_OFDM(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_ofdm_t * pOfdm)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_OFDM");

    if ((!pDemod) || (!pOfdm)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t data[9];

        /* Freeze registers */
		if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        result = cxd6801_isDemodLocked (pDemod);
		if (result != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Set SLV-T Bank : 0x90 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        /*
         *  slave     Bank    Addr   Bit        Name                            Meaning
         * ---------------------------------------------------------------------------------
         *  <SLV-T>   90h     57h    [5:4]  0   IREG_OFDM_BW[1:0]               0: 8MHz, 1: 7MHz, 2: 6MHz
         *  <SLV-T>   90h     57h    [3:0]  0   IREG_OFDM_NBS[3:0]              Number of BootStrap symbols -1
         *  <SLV-T>   90h     58h    [7:0]  1   IREG_OFDM_NSBF[7:0]             Number of SubFrame -1
         *  <SLV-T>   90h     59h    [7:5]  2   IREG_OFDM_L1B_FEC_TYPE[2:0]     0:MODE1, 1:MODE2, 2:MODE3, 3:MODE4, 4:MODE5, 5:MODE6, 6:MODE7
         *  <SLV-T>   90h     59h    [4:3]  2   IREG_OFDM_PAPR[1:0]             0: None,      1: ACE,       2: TR,        3: ACE_TR
         *  <SLV-T>   90h     59h    [2:0]  2   IREG_OFDM_NPB[2:0]              Number of Preamble symbols -1
         *  <SLV-T>   90h     5Ah    [7:5]  3   IREG_OFDM_PB_FFTSIZE[2:0]       [Preamble]   1:8K, 4:16K, 5:32K
         *  <SLV-T>   90h     5Ah    [4:0]  3   IREG_OFDM_PB_PILOT[4:0]         [Preamble]   0,1:Dx=3  2,3:Dx=4  4,5:Dx=6  6,7:Dx=8  8,9:Dx=12  10,11:Dx=16  12,13:Dx=24  14,15:Dx=32
         *  <SLV-T>   90h     5Bh    [6:4]  4   IREG_OFDM_PB_CRED_COEFF[2:0]    [Preamble]   Reduced Carrier index
         *  <SLV-T>   90h     5Bh    [3:0]  4   IREG_OFDM_PB_GI[3:0]            [Preamble]   1:GI1_192 2:GI2_384 3:GI3_512 4:GI4_768 5:GI5_1024 6:GI6_1536 7:GI7_2048 8:GI8_2432 9:GI9_3072 10:GI10_3648 11:GI11_4096 12:GI12_4864
         *  <SLV-T>   90h     5Ch    [7:5]  5   IREG_OFDM_SBF0_FFTSIZE[2:0]     [SubFrame#0] 1:8K, 4:16K, 5:32K
         *  <SLV-T>   90h     5Ch    [4:0]  5   IREG_OFDM_SBF0_SP[4:0]          [SubFrame#0] 0:SP3_2 1:SP_3_4 2:SP4_2 3:SP4_4 4:SP6_2 5:SP6_4 6:SP8_2 7:SP8_4 8:SP12_2 9:SP12_4 10:SP16_2 11:SP16_4 12:SP24_2 13:SP24_4 14:SP32_2 15:SP32_4
         *  <SLV-T>   90h     5Dh    [6:4]  6   IREG_OFDM_SBF0_CRED_COEFF[2:0]  [SubFrame#0] Reduced Carrier index
         *  <SLV-T>   90h     5Dh    [3:0]  6   IREG_OFDM_SBF0_GI[3:0]          [SubFrame#0] 1:GI1_192 2:GI2_384 3:GI3_512 4:GI4_768 5:GI5_1024 6:GI6_1536 7:GI7_2048 8:GI8_2432 9:GI9_3072 10:GI10_3648 11:GI11_4096 12:GI12_4864
         *  <SLV-T>   90h     5Eh    [7:5]  7   IREG_OFDM_SBF0_SP_BOOST[2:0]    [SubFrame#0] SP BOOST index
         *  <SLV-T>   90h     5Eh    [4]    7   IREG_OFDM_SBF0_SBS_FIRST        [SubFrame#0] Subframe Boundary Symbol First flag
         *  <SLV-T>   90h     5Eh    [3]    7   IREG_OFDM_SBF0_SBS_LAST         [SubFrame#0] Subframe Boundary Symbol Last flag
         *  <SLV-T>   90h     5Eh    [2:0]  7   IREG_OFDM_SBF0_NDSYM[10:8]      [SubFrame#0] Number of Data symbols -1
         *  <SLV-T>   90h     5Fh    [7:0]  8   IREG_OFDM_SBF0_NDSYM[7:0]
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x57, data, sizeof(data)) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        CXD6801_SLVT_UnFreezeReg (pDemod);

        switch ((data[0] >> 4) & 0x03) {
        case 0:
			pOfdm->system_bw = SONY_CXD6801_ATSC3_SYSTEM_BW_8_MHZ;
            break;
        case 1:
			pOfdm->system_bw = SONY_CXD6801_ATSC3_SYSTEM_BW_7_MHZ;
            break;
        case 2:
			pOfdm->system_bw = SONY_CXD6801_ATSC3_SYSTEM_BW_6_MHZ;
            break;
        default:
            /* Invalid value */
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }

        pOfdm->bs_num_symbol = data[0] & 0x0F;
        pOfdm->num_subframe = data[1];
		pOfdm->l1b_fec_type = (sony_cxd6801_atsc3_l1b_fec_type_t)((data[2] >> 5) & 0x07);
		pOfdm->papr = (sony_cxd6801_atsc3_papr_t)((data[2] >> 3) & 0x03);
        pOfdm->pb_num_symbol = data[2] & 0x07;

        switch ((data[3] >> 5) & 0x07) {
        case 1:
			pOfdm->pb_fft_size = SONY_CXD6801_ATSC3_FFT_SIZE_8K;
            break;
        case 4:
			pOfdm->pb_fft_size = SONY_CXD6801_ATSC3_FFT_SIZE_16K;
            break;
        case 5:
			pOfdm->pb_fft_size = SONY_CXD6801_ATSC3_FFT_SIZE_32K;
            break;
        default:
            /* Invalid value */
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }

        switch (data[3] & 0x1F) {
        case 0:
        case 1:
			pOfdm->pb_pilot = SONY_CXD6801_ATSC3_PREAMBLE_PILOT_3;
            break;
        case 2:
        case 3:
			pOfdm->pb_pilot = SONY_CXD6801_ATSC3_PREAMBLE_PILOT_4;
            break;
        case 4:
        case 5:
			pOfdm->pb_pilot = SONY_CXD6801_ATSC3_PREAMBLE_PILOT_6;
            break;
        case 6:
        case 7:
			pOfdm->pb_pilot = SONY_CXD6801_ATSC3_PREAMBLE_PILOT_8;
            break;
        case 8:
        case 9:
			pOfdm->pb_pilot = SONY_CXD6801_ATSC3_PREAMBLE_PILOT_12;
            break;
        case 10:
        case 11:
			pOfdm->pb_pilot = SONY_CXD6801_ATSC3_PREAMBLE_PILOT_16;
            break;
        case 12:
        case 13:
			pOfdm->pb_pilot = SONY_CXD6801_ATSC3_PREAMBLE_PILOT_24;
            break;
        case 14:
        case 15:
			pOfdm->pb_pilot = SONY_CXD6801_ATSC3_PREAMBLE_PILOT_32;
            break;
        default:
            /* Invalid value */
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }

        pOfdm->pb_reduced_carriers = (data[4] >> 4) & 0x07;
		pOfdm->pb_gi = (sony_cxd6801_atsc3_gi_t)(data[4] & 0x0F);

        switch ((data[5] >> 5) & 0x07) {
        case 1:
			pOfdm->sf0_fft_size = SONY_CXD6801_ATSC3_FFT_SIZE_8K;
            break;
        case 4:
			pOfdm->sf0_fft_size = SONY_CXD6801_ATSC3_FFT_SIZE_16K;
            break;
        case 5:
			pOfdm->sf0_fft_size = SONY_CXD6801_ATSC3_FFT_SIZE_32K;
            break;
        default:
            /* Invalid value */
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }

		pOfdm->sf0_sp = (sony_cxd6801_atsc3_sp_t)(data[5] & 0x1F);
        pOfdm->sf0_reduced_carriers = (data[6] >> 4) & 0x07;
        pOfdm->sf0_gi = (sony_cxd6801_atsc3_gi_t)(data[6] & 0x0F);
        pOfdm->sf0_sp_boost = (data[7] >> 5) & 0x07;
        pOfdm->sf0_sbs_first = (data[7] >> 4) & 0x01;
        pOfdm->sf0_sbs_last = (data[7] >> 3) & 0x01;
        pOfdm->sf0_num_ofdm_symbol = (data[7] & 0x07) << 8 | data[8];

    }
	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_L1Basic(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_l1basic_t * pL1Basic)
{
    uint8_t data[29];
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_L1Basic");

    if ((!pDemod) || (!pL1Basic)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x93 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave     Bank    Addr    Bit             Name
     * ---------------------------------------------------------------------------------
     *  <SLV-T>   93h     10h     [0]             IL1B_OK
     */

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, data, 1) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (!(data[0] & 0x01)) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /*
     *  slave     Bank    Addr   Bit              Name
     * ------------------------------------------------------------------------
     *  <SLV-T>   93h     60h    [7:5]            IL1B_VERSION[2:0]
     *  <SLV-T>   93h     60h    [4]              IL1B_MIMO_SP_ENC
     *  <SLV-T>   93h     60h    [3]              IL1B_LLS_FLAG
     *  <SLV-T>   93h     60h    [2:1]            IL1B_TIME_INFO_FLAG[1:0]
     *  <SLV-T>   93h     60h    [0]              IL1B_RTN_CH_FLAG
     *  <SLV-T>   93h     61h    [2:1]            IL1B_PAPR_REDUCTION[1:0]
     *  <SLV-T>   93h     61h    [0]              IL1B_FRAME_LEN_MODE
     *  <SLV-T>   93h     62h    [7:0]            IL1B_FRAME_LEN[9:2]              *
     *  <SLV-T>   93h     63h    [7:6]            IL1B_FRAME_LEN[1:0]              *
     *  <SLV-T>   93h     63h    [5:0]            IL1B_EXCESS_SAMP_PER_SYM[12:7]   *
     *  <SLV-T>   93h     64h    [6:0]            IL1B_EXCESS_SAMP_PER_SYM[6:0]    *
     *  <SLV-T>   93h     65h    [7:0]            IL1B_TIME_OFFSET[15:8]           **
     *  <SLV-T>   93h     66h    [7:0]            IL1B_TIME_OFFSET[7:0]            **
     *  <SLV-T>   93h     67h    [6:0]            IL1B_ADD_SAMP[6:0]               **
     *  <SLV-T>   93h     68h    [7:0]            IL1B_NUM_SUBFRAMES[7:0]
     *  <SLV-T>   93h     69h    [7:5]            IL1B_PRE_NUM_SYM[2:0]
     *  <SLV-T>   93h     69h    [4:2]            IL1B_PRE_REDUCED_CARR[2:0]
     *  <SLV-T>   93h     69h    [1:0]            IL1B_L1D_CONTENT_TAG[1:0]
     *  <SLV-T>   93h     6Ah    [7:0]            IL1B_L1D_SIZE_BYTES[12:5]
     *  <SLV-T>   93h     6Bh    [7:3]            IL1B_L1D_SIZE_BYTES[4:0]
     *  <SLV-T>   93h     6Bh    [2:0]            IL1B_L1D_FEC_TYPE[2:0]
     *  <SLV-T>   93h     6Ch    [7:6]            IL1B_L1D_ADD_PARITY_MODE[1:0]
     *  <SLV-T>   93h     6Ch    [5:0]            IL1B_L1D_TOTAL_CELLS[18:13]
     *  <SLV-T>   93h     6Dh    [7:0]            IL1B_L1D_TOTAL_CELLS[12:5]
     *  <SLV-T>   93h     6Eh    [7:3]            IL1B_L1D_TOTAL_CELLS[4:0]
     *  <SLV-T>   93h     6Eh    [2]              IL1B_1ST_SUB_MIMO
     *  <SLV-T>   93h     6Eh    [1:0]            IL1B_1ST_SUB_MISO[1:0]
     *  <SLV-T>   93h     6Fh    [7:6]            IL1B_1ST_SUB_FFT_SIZE[1:0]
     *  <SLV-T>   93h     6Fh    [5:3]            IL1B_1ST_SUB_REDUCED_CARR[2:0]
     *  <SLV-T>   93h     6Fh    [2:0]            IL1B_1ST_SUB_GI[3:1]
     *  <SLV-T>   93h     70h    [7]              IL1B_1ST_SUB_GI[0]
     *  <SLV-T>   93h     70h    [6:0]            IL1B_1ST_SUB_NUM_OFDM_SYM[10:4]
     *  <SLV-T>   93h     71h    [7:4]            IL1B_1ST_SUB_NUM_OFDM_SYM[3:0]
     *  <SLV-T>   93h     71h    [3:0]            IL1B_1ST_SUB_SP_PAT[4:1]
     *  <SLV-T>   93h     72h    [5]              IL1B_1ST_SUB_SP_PAT[0]
     *  <SLV-T>   93h     72h    [4:2]            IL1B_1ST_SUB_SP_BOOST[2:0]
     *  <SLV-T>   93h     72h    [1]              IL1B_1ST_SUB_SBS_1ST
     *  <SLV-T>   93h     72h    [0]              IL1B_1ST_SUB_SBS_LAST
     *  <SLV-T>   93h     73h    [7:0]            IL1B_RESERVED[47:40]
     *  <SLV-T>   93h     74h    [7:0]            IL1B_RESERVED[39:32]
     *  <SLV-T>   93h     75h    [7:0]            IL1B_RESERVED[31:24]
     *  <SLV-T>   93h     76h    [7:0]            IL1B_RESERVED[23:16]
     *  <SLV-T>   93h     77h    [7:0]            IL1B_RESERVED[15:8]
     *  <SLV-T>   93h     78h    [7:0]            IL1B_RESERVED[7:0]
     *  *  IL1B_FRAME_LEN , IL1B_EXCESS_SAMP_PER_SYM are valid when IL1B_FRAME_LEN_MODE==0.
     *  ** IL1B_TIME_OFFSET , IL1B_ADD_SAMP          are valid when IL1B_FRAME_LEN_MODE==1.
     */

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x60, data, sizeof(data)) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    CXD6801_SLVT_UnFreezeReg (pDemod);

    pL1Basic->version = (data[0] >> 5) & 0x07;
    pL1Basic->mimo_sp_enc = (data[0] >> 4) & 0x01;
    pL1Basic->lls_flg = (data[0] >> 3) & 0x01;
	pL1Basic->time_info_flg = (sony_cxd6801_atsc3_time_info_flag_t)((data[0] >> 1) & 0x03);
    pL1Basic->return_ch_flg = data[0] & 0x01;
	pL1Basic->papr = (sony_cxd6801_atsc3_papr_t)((data[1] >> 1) & 0x03);
    pL1Basic->frame_length_mode = data[1] & 0x01;
    if (pL1Basic->frame_length_mode == 0) {
        pL1Basic->frame_length = ((data[2] << 2) & 0x3FC) | ((data[3] >> 6) & 0x03);
        pL1Basic->excess_smp_per_sym = ((data[3] << 7) & 0x1F80) | (data[4] & 0x7F);
    } else {
        pL1Basic->frame_length = 0;
        pL1Basic->excess_smp_per_sym = 0;
    }
    if (pL1Basic->frame_length_mode == 1) {
        pL1Basic->time_offset = (data[5] << 8) | data[6];
        pL1Basic->additional_smp = data[7] & 0x7F;
    } else {
        pL1Basic->time_offset = 0;
        pL1Basic->additional_smp = 0;
    }
    pL1Basic->num_subframe = data[8];
    pL1Basic->pb_num_symbol = (data[9] >> 5) & 0x07;
    pL1Basic->pb_reduced_carriers = (data[9] >> 2) & 0x07;
    pL1Basic->l1d_content_tag = data[9] & 0x03;
    pL1Basic->l1d_size = ((data[10] << 5) & 0x1FE0) | ((data[11] >> 3) & 0x1F);
	pL1Basic->l1d_fec_type = (sony_cxd6801_atsc3_l1d_fec_type_t)(data[11] & 0x07);
    pL1Basic->l1d_add_parity_mode = (data[12] >> 6) & 0x03;
    pL1Basic->l1d_total_cells = ((data[12] << 13) & 0x7E000) | (data[13] << 5) | ((data[14] >> 3) & 0x1F);
    pL1Basic->sf0_mimo = (data[14] >> 2) & 0x01;
	pL1Basic->sf0_miso = (sony_cxd6801_atsc3_miso_t)(data[14] & 0x03);
	pL1Basic->sf0_fft_size = (sony_cxd6801_atsc3_fft_size_t)((data[15] >> 6) & 0x03);
    pL1Basic->sf0_reduced_carriers = (data[15] >> 3) & 0x07;
	pL1Basic->sf0_gi = (sony_cxd6801_atsc3_gi_t)(((data[15] << 1) & 0x0E) | ((data[16] >> 7) & 0x01));
    pL1Basic->sf0_num_ofdm_symbol = ((data[16] << 4) & 0x7F0) | ((data[17] >> 4) & 0x0F);
	pL1Basic->sf0_sp = (sony_cxd6801_atsc3_sp_t)(((data[17] << 1) & 0x1E) | ((data[18] >> 5) & 0x01));
    pL1Basic->sf0_sp_boost = (data[18] >> 2) & 0x07;
    pL1Basic->sf0_sbs_first = (data[18] >> 1) & 0x01;
    pL1Basic->sf0_sbs_last = data[18] & 0x01;
    pL1Basic->reserved[0] = data[19];
    pL1Basic->reserved[1] = data[20];
    pL1Basic->reserved[2] = data[21];
    pL1Basic->reserved[3] = data[22];
    pL1Basic->reserved[4] = data[23];
    pL1Basic->reserved[5] = data[24];

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_L1Detail_raw(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_l1basic_t * pL1Basic,
	sony_cxd6801_atsc3_l1detail_raw_t * pL1Detail)
{
    uint8_t data[2];
	sony_cxd6801_result_t result;
	sony_cxd6801_stopwatch_t timer;
    uint8_t continueWait = 1;
    uint32_t elapsed = 0;
    uint16_t ramSize = 0;
    uint8_t subBank = 0;
    uint16_t totalReadSize = 0;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_L1Detail_raw");

    if (!pDemod || !pL1Basic || !pL1Detail) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x93 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave     Bank    Addr    Bit             Name
     * ---------------------------------------------------------------------------------
     *  <SLV-T>   93h     10h     [1]             IL1D_OK
     */

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, data, 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (!(data[0] & 0x02)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

	result = sony_cxd6801_stopwatch_start(&timer);
	if (result != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (result);
    }

    for (;;) {
		result = sony_cxd6801_stopwatch_elapsed(&timer, &elapsed);
		if (result != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (result);
        }

		if (elapsed >= SONY_CXD6801_ATSC3_WAIT_L1_DETAIL_TIMEOUT) {
            continueWait = 0;
        }

        /*  slave      Bank      Addr      Bit      Name
         * --------------------------------------------------------------
         *  <SLV-T>    93h       E1h       [7]      IL1RAM_UPDATE
         */

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE1, data, 1) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        if (data[0] & 0x80) {
            break;
        }
        if (continueWait) {
			result = sony_cxd6801_stopwatch_sleep(&timer, SONY_CXD6801_ATSC3_WAIT_L1_DETAIL_INTERVAL);
			if (result != SONY_CXD6801_RESULT_OK) {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        } else {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_TIMEOUT);
        }
    }

    /* Freeze L1 infomation */
    /* Set SLV-T Bank : 0x93 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave     Bank      Addr      Bit      Default    Setting    Signal name
     * ---------------------------------------------------------------------------------
     *  <SLV-T>   93h       E0h       [0]      1'b0       1'b1       OREGD_L1RAM_FREEZE
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE0, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	result = sony_cxd6801_demod_atsc3_monitor_L1Basic(pDemod, pL1Basic);
    if (result != SONY_CXD6801_RESULT_OK) {
        goto unfreeze_l1_info;
    }

    continueWait = 1;
    elapsed = 0;

	result = sony_cxd6801_stopwatch_start(&timer);
    if (result != SONY_CXD6801_RESULT_OK) {
        goto unfreeze_l1_info;
    }

    /* Set SLV-T Bank : 0x93 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
		result = SONY_CXD6801_RESULT_ERROR_I2C;
        goto unfreeze_l1_info;
    }

    for (;;) {
		result = sony_cxd6801_stopwatch_elapsed(&timer, &elapsed);
        if (result != SONY_CXD6801_RESULT_OK) {
            goto unfreeze_l1_info;
        }

		if (elapsed >= SONY_CXD6801_ATSC3_WAIT_L1_DETAIL_TIMEOUT2) {
            continueWait = 0;
        }

        /*  slave     Bank      Addr      Bit      Name
         * --------------------------------------------------------------
         *  <SLV-T>   93h       E1h       [6]      IL1RAM_DECNG
         *  <SLV-T>   93h       E1h       [5]      IL1RAM_READOK
         */

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE1, data, 1) != SONY_CXD6801_RESULT_OK) {
			result = SONY_CXD6801_RESULT_ERROR_I2C;
            goto unfreeze_l1_info;
        }
        if (data[0] & 0x40) {
			result = SONY_CXD6801_RESULT_ERROR_HW_STATE;
            goto unfreeze_l1_info;
        }
        if (data[0] & 0x20) {
            break;
        }
        if (continueWait) {
			result = sony_cxd6801_stopwatch_sleep(&timer, SONY_CXD6801_ATSC3_WAIT_L1_DETAIL_INTERVAL);
            if (result != SONY_CXD6801_RESULT_OK) {
                goto unfreeze_l1_info;
            }
        } else {
			result = SONY_CXD6801_RESULT_ERROR_TIMEOUT;
            goto unfreeze_l1_info;
        }
    }

    /*  slave     Bank    Addr    Bit          Name
     * ----------------------------------------------------------------
     *  <SLV-T>   93h     E1h     [4:0]        IL1RAM_SIZE_BYTES[12:8]
     *  <SLV-T>   93h     E2h     [7:0]        IL1RAM_SIZE_BYTES[7:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE1, data, 2) != SONY_CXD6801_RESULT_OK) {
		result = SONY_CXD6801_RESULT_ERROR_I2C;
        goto unfreeze_l1_info;
    }
    ramSize = ((data[0] & 0x1F) << 8) | data[1];
    pL1Detail->size = ramSize;

    for (subBank = 0; ramSize > 0; subBank++) {
        uint16_t readSize = 0;
        /* Set SLV-T Bank : 0x93 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
			result = SONY_CXD6801_RESULT_ERROR_I2C;
            goto unfreeze_l1_info;
        }
        /*  slave      Bank      Addr      Bit      Default    Setting      Name
         * ------------------------------------------------------------------------------------
         *  <SLV-T>    93h       E3h       [5:0]    6'd0       sub-bank     OREGD_L1RAM_SUB_BANK
         */

		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE3, subBank) != SONY_CXD6801_RESULT_OK) {
			result = SONY_CXD6801_RESULT_ERROR_I2C;
            goto unfreeze_l1_info;
        }
		if (ramSize < CXD6801_L1RAM_READ_SIZE) {
            readSize = ramSize;
        } else {
			readSize = CXD6801_L1RAM_READ_SIZE;
        }

        /* Set SLV-T Bank : 0x92 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x92) != SONY_CXD6801_RESULT_OK) {
			result = SONY_CXD6801_RESULT_ERROR_I2C;
            goto unfreeze_l1_info;
        }
        /*  slave      Bank      Addr      size
         * --------------------------------------------------------------
         *  <SLV-T>    92h       10h~FFh   readSize (max 240 bytes)
         */

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, &pL1Detail->data[totalReadSize], readSize) != SONY_CXD6801_RESULT_OK) {
			result = SONY_CXD6801_RESULT_ERROR_I2C;
            goto unfreeze_l1_info;
        }
        ramSize -= readSize;
        totalReadSize += readSize;
    }

unfreeze_l1_info:

    /* Set SLV-T Bank : 0x93 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave      Bank      Addr      Bit      Default    Setting    Signal name
     * ---------------------------------------------------------------------------------
     *  <SLV-T>    93h       E0h       [0]      1'b0       1'b0       OREGD_L1RAM_FREEZE
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xE0, 0x00) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_L1Detail_convert(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_l1basic_t * pL1Basic,
	sony_cxd6801_atsc3_l1detail_raw_t * pL1Detail,
                                                         uint8_t plpID,
														 sony_cxd6801_atsc3_l1detail_common_t * pL1DetailCommon,
														 sony_cxd6801_atsc3_l1detail_subframe_t * pL1DetailSubframe,
														 sony_cxd6801_atsc3_l1detail_plp_t * pL1DetailPlp)
{
    uint32_t bp = 0;
    uint16_t sf_index = 0;
    uint8_t plp_index = 0;
    uint8_t i = 0;

    sony_cxd6801_atsc3_l1detail_subframe_t tmpL1dSubframe;
    sony_cxd6801_atsc3_l1detail_plp_t tmpL1dPlp;
    uint8_t plpFound = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_L1Detail_convert");

    if ((!pDemod) || (!pL1Basic) || (!pL1Detail) || (!pL1DetailCommon) || (!pL1DetailSubframe) || (!pL1DetailPlp)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (plpID >= SONY_CXD6801_ATSC3_NUM_PLP_MAX) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pL1Detail->size < 25) || (pL1Detail->size > 8191)) {
        /* Invalid L1-Detail data length */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if (pL1Detail->size != pL1Basic->l1d_size) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    sony_cxd6801_memset (pL1DetailCommon, 0, sizeof (sony_cxd6801_atsc3_l1detail_common_t));
    sony_cxd6801_memset (pL1DetailSubframe, 0, sizeof (sony_cxd6801_atsc3_l1detail_common_t));
    sony_cxd6801_memset (pL1DetailPlp, 0, sizeof (sony_cxd6801_atsc3_l1detail_common_t));

    /* L1-Detail common part */
	pL1DetailCommon->version = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 4);
    bp += 4;

	pL1DetailCommon->num_rf = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 3);
    bp += 3;

    for (i = 0; i < pL1DetailCommon->num_rf; i++) {
        /*
         * In current version, L1D_num_rf should be 0 or 1
         * if L1D_num_rf > 1, only first value will be stored.
         */
        if (i == 0) {
			pL1DetailCommon->bonded_bsid = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 16);
        }
        bp += 16;

        /* Reserved 3 bit */
        bp += 3;
    }

	if (pL1Basic->time_info_flg != SONY_CXD6801_ATSC3_TIME_INFO_FLAG_NONE) {
		pL1DetailCommon->time_sec = (uint32_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 32);
        bp += 32;

		pL1DetailCommon->time_msec = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 10);
        bp += 10;

		if (pL1Basic->time_info_flg != SONY_CXD6801_ATSC3_TIME_INFO_FLAG_MS) {
			pL1DetailCommon->time_usec = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 10);
            bp += 10;

			if (pL1Basic->time_info_flg != SONY_CXD6801_ATSC3_TIME_INFO_FLAG_US) {
				pL1DetailCommon->time_nsec = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 10);
                bp += 10;
            }
        }
    }

    for (sf_index = 0; sf_index <= pL1Basic->num_subframe; sf_index++) {
        /* Sanity check (48 = 16 (bsid) + 32 (CRC) size) */
        if (bp + 48 >= (uint32_t)(pL1Detail->size) * 8) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_OTHER);
        }

        /* L1-Detail subframe part */
        sony_cxd6801_memset (&tmpL1dSubframe, 0, sizeof (sony_cxd6801_atsc3_l1detail_subframe_t));

        tmpL1dSubframe.index = (uint8_t) sf_index;

        if (sf_index == 0) {
            /* Copy from L1-Basic */
            tmpL1dSubframe.mimo = pL1Basic->sf0_mimo;
            tmpL1dSubframe.miso = pL1Basic->sf0_miso;
            tmpL1dSubframe.fft_size = pL1Basic->sf0_fft_size;
            tmpL1dSubframe.reduced_carriers = pL1Basic->sf0_reduced_carriers;
            tmpL1dSubframe.gi = pL1Basic->sf0_gi;
            tmpL1dSubframe.num_ofdm_symbol = pL1Basic->sf0_num_ofdm_symbol;
            tmpL1dSubframe.sp = pL1Basic->sf0_sp;
            tmpL1dSubframe.sp_boost = pL1Basic->sf0_sp_boost;
            tmpL1dSubframe.sbs_first = pL1Basic->sf0_sbs_first;
            tmpL1dSubframe.sbs_last = pL1Basic->sf0_sbs_last;
        } else {
			tmpL1dSubframe.mimo = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
            bp += 1;
			tmpL1dSubframe.miso = (sony_cxd6801_atsc3_miso_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 2);
            bp += 2;
			tmpL1dSubframe.fft_size = (sony_cxd6801_atsc3_fft_size_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 2);
            bp += 2;
			tmpL1dSubframe.reduced_carriers = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 3);
            bp += 3;
			tmpL1dSubframe.gi = (sony_cxd6801_atsc3_gi_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 4);
            bp += 4;
			tmpL1dSubframe.num_ofdm_symbol = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 11);
            bp += 11;
			tmpL1dSubframe.sp = (sony_cxd6801_atsc3_sp_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 5);
            bp += 5;
			tmpL1dSubframe.sp_boost = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 3);
            bp += 3;
			tmpL1dSubframe.sbs_first = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
            bp += 1;
			tmpL1dSubframe.sbs_last = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
            bp += 1;
        }

        if (pL1Basic->num_subframe > 0) {
			tmpL1dSubframe.subframe_mux = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
            bp += 1;
        }

		tmpL1dSubframe.freq_interleaver = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
        bp += 1;

        if (tmpL1dSubframe.sbs_first || tmpL1dSubframe.sbs_last) {
			tmpL1dSubframe.sbs_null_cells = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 13);
            bp += 13;
        }

		tmpL1dSubframe.num_plp = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 6);
        bp += 6;

        for (plp_index = 0; plp_index <= tmpL1dSubframe.num_plp; plp_index++) {
            /* Sanity check (48 = 16 (bsid) + 32 (CRC) size) */
            if (bp + 48 >= (uint32_t)(pL1Detail->size) * 8) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_OTHER);
            }

            /* L1-Detail PLP part */
            sony_cxd6801_memset (&tmpL1dPlp, 0, sizeof (sony_cxd6801_atsc3_l1detail_plp_t));

			tmpL1dPlp.id = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 6);
            bp += 6;
			tmpL1dPlp.lls_flg = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
            bp += 1;
			tmpL1dPlp.layer = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 2);
            bp += 2;
			tmpL1dPlp.start = (uint32_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 24);
            bp += 24;
			tmpL1dPlp.size = (uint32_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 24);
            bp += 24;
			tmpL1dPlp.scrambler_type = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 2);
            bp += 2;
			tmpL1dPlp.fec_type = (sony_cxd6801_atsc3_plp_fec_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 4);
            bp += 4;
            if (tmpL1dPlp.fec_type <= SONY_CXD6801_ATSC3_PLP_FEC_LDPC_64K) {
				tmpL1dPlp.mod = (sony_cxd6801_atsc3_plp_mod_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 4);
                bp += 4;
				tmpL1dPlp.cod = (sony_cxd6801_atsc3_plp_cod_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 4);
                bp += 4;
            }

			tmpL1dPlp.ti_mode = (sony_cxd6801_atsc3_plp_ti_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 2);
            bp += 2;
            if (tmpL1dPlp.ti_mode == SONY_CXD6801_ATSC3_PLP_TI_NONE) {
				tmpL1dPlp.fec_block_start = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 15);
                bp += 15;
            } else if (tmpL1dPlp.ti_mode == SONY_CXD6801_ATSC3_PLP_TI_CTI) {
				tmpL1dPlp.cti_fec_block_start = (uint32_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 22);
                bp += 22;
            }

            if (pL1DetailCommon->num_rf > 0) {
				tmpL1dPlp.num_ch_bonded = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 3);
                bp += 3;
                if (tmpL1dPlp.num_ch_bonded > 0) {
					tmpL1dPlp.ch_bonding_format = (sony_cxd6801_atsc3_plp_ch_bond_fmt_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 2);
                    bp += 2;
                    for (i = 0; i <= tmpL1dPlp.num_ch_bonded; i++) {
                        /*
                         * In current version, L1D_plp_num_channel_bonded should be 0 or 1
                         * if L1D_plp_num_channel_bonded > 1, only first 2 value will be stored.
                         */
                        if (i == 0) {
							tmpL1dPlp.bonded_rf_id_0 = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 3);
                        } else if (i == 1) {
							tmpL1dPlp.bonded_rf_id_1 = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 3);
                        }
                        bp += 3;
                    }
                }
            }

            if (tmpL1dSubframe.mimo) {
				tmpL1dPlp.mimo_stream_combine = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
                bp += 1;
				tmpL1dPlp.mimo_iq_interleave = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
                bp += 1;
				tmpL1dPlp.mimo_ph = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
                bp += 1;
            }

            if (tmpL1dPlp.layer == 0) {
				tmpL1dPlp.plp_type = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
                bp += 1;
                if (tmpL1dPlp.plp_type) {
					tmpL1dPlp.num_subslice = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 14);
                    bp += 14;
					tmpL1dPlp.subslice_interval = (uint32_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 24);
                    bp += 24;
                }

				if (((tmpL1dPlp.ti_mode == SONY_CXD6801_ATSC3_PLP_TI_CTI) || (tmpL1dPlp.ti_mode == SONY_CXD6801_ATSC3_PLP_TI_HTI))
					&& (tmpL1dPlp.mod == SONY_CXD6801_ATSC3_PLP_MOD_QPSK)) {
					tmpL1dPlp.ti_ext_interleave = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
                    bp += 1;
                }

                if (tmpL1dPlp.ti_mode == SONY_CXD6801_ATSC3_PLP_TI_CTI) {
					tmpL1dPlp.cti_depth = (sony_cxd6801_atsc3_plp_cti_depth_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 3);
                    bp += 3;
					tmpL1dPlp.cti_start_row = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 11);
                    bp += 11;
				}
				else if (tmpL1dPlp.ti_mode == SONY_CXD6801_ATSC3_PLP_TI_HTI) {
					tmpL1dPlp.hti_inter_subframe = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
                    bp += 1;
					tmpL1dPlp.hti_num_ti_blocks = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 4);
                    bp += 4;
					tmpL1dPlp.hti_num_fec_block_max = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 12);
                    bp += 12;

                    if (tmpL1dPlp.hti_inter_subframe == 0) {
						tmpL1dPlp.hti_num_fec_block[0] = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 12);
                        bp += 12;
                    } else {
                        for (i = 0; i <= tmpL1dPlp.hti_num_ti_blocks; i++) {
							tmpL1dPlp.hti_num_fec_block[i] = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 12);
                            bp += 12;
                        }
                    }
					tmpL1dPlp.hti_cell_interleave = (uint8_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 1);
                    bp += 1;
                }
            } else {
				tmpL1dPlp.ldm_inj_level = (sony_cxd6801_atsc3_plp_ldm_inj_level_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 5);
                bp += 5;
            }

            /* Then, if the information is necessary, copy it */
            if (tmpL1dPlp.id == plpID) {
                if (!plpFound) {
                    sony_cxd6801_memcpy (pL1DetailSubframe, &tmpL1dSubframe, sizeof (sony_cxd6801_atsc3_l1detail_subframe_t));
                    sony_cxd6801_memcpy (pL1DetailPlp, &tmpL1dPlp, sizeof (sony_cxd6801_atsc3_l1detail_plp_t));
                    plpFound = 1;
                }
                /*
                 * ATSC 3.0 standard allows to allocate one PLP to multiple subframes.
                 * If so, specified PLP information can found multiple times.
                 * This API get *first* information for specified PLP only.
                 */
            }
        }
    }

    /* Here, common part again */
    if (pL1DetailCommon->version >= 1) {
		pL1DetailCommon->bsid = (uint16_t)sony_cxd6801_BitSplitFromByteArray(pL1Detail->data, bp, 16);
        bp += 16;
    }

    /* Sanity check (size of CRC is 32bit) */
    if (bp + 32 > (uint32_t)(pL1Detail->size) * 8) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_OTHER);
    }

    pL1DetailCommon->reserved_bitlen = (uint16_t)(pL1Basic->l1d_size * 8 - bp - 32);

    if (plpFound) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
    } else {
        /* Conversion is OK, but the specified PLP is not exist... */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK_CONFIRM);
    }
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_PLPList(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_plp_list_entry_t plpList[SONY_CXD6801_ATSC3_NUM_PLP_MAX],
                                                uint8_t * pNumPLPs)
{
    uint8_t data[36];
    uint8_t plpInfoRdy = 0;
    uint8_t plpInfoChg = 0;
    uint8_t l1dNumRf = 0;
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_PLPList");

    if ((!pDemod) || (!pNumPLPs)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*
     *  slave      Bank     Addr     Bit        Signal name
     * --------------------------------------------------------------
     *  <SLV-T>    93h      10h      [1]        IL1D_OK
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, data, 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (!(data[0] & 0x02)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /*
     *  slave      Bank     Addr     Bit        Signal name
     * --------------------------------------------------------------
     *  <SLV-T>    93h      9Dh      [0]        IPLPINFO_RDY
     *  <SLV-T>    93h      9Eh      [0]        IPLPINFO_CHG
     *  <SLV-T>    93h      9Fh      [6:0]      ITOTALNUM_PLP[6:0]  total number of PLP (max 64) This field shall be set to 1 less than the total number of PLPs.
     *  <SLV-T>    93h      A0h      [7:0]      IL1D_PLP00,IL1D_PLP01,IL1D_PLP02,IL1D_PLP03,IL1D_PLP04,IL1D_PLP05,IL1D_PLP06,IL1D_PLP07
     *  <SLV-T>    93h      A1h      [7:0]      IL1D_PLP08,IL1D_PLP09,IL1D_PLP10,IL1D_PLP11,IL1D_PLP12,IL1D_PLP13,IL1D_PLP14,IL1D_PLP15
     *  <SLV-T>    93h      A2h      [7:0]      IL1D_PLP16,IL1D_PLP17,IL1D_PLP18,IL1D_PLP19,IL1D_PLP20,IL1D_PLP21,IL1D_PLP22,IL1D_PLP23
     *  <SLV-T>    93h      A3h      [7:0]      IL1D_PLP24,IL1D_PLP25,IL1D_PLP26,IL1D_PLP27,IL1D_PLP28,IL1D_PLP29,IL1D_PLP30,IL1D_PLP31
     *  <SLV-T>    93h      A4h      [7:0]      IL1D_PLP32,IL1D_PLP33,IL1D_PLP34,IL1D_PLP35,IL1D_PLP36,IL1D_PLP37,IL1D_PLP38,IL1D_PLP39
     *  <SLV-T>    93h      A5h      [7:0]      IL1D_PLP40,IL1D_PLP41,IL1D_PLP42,IL1D_PLP43,IL1D_PLP44,IL1D_PLP45,IL1D_PLP46,IL1D_PLP47
     *  <SLV-T>    93h      A6h      [7:0]      IL1D_PLP48,IL1D_PLP49,IL1D_PLP50,IL1D_PLP51,IL1D_PLP52,IL1D_PLP53,IL1D_PLP54,IL1D_PLP55
     *  <SLV-T>    93h      A7h      [7:0]      IL1D_PLP56,IL1D_PLP57,IL1D_PLP58,IL1D_PLP59,IL1D_PLP60,IL1D_PLP61,IL1D_PLP62,IL1D_PLP63

     *  <SLV-T>    93h      A8h      [7:0]      IL1D_LLS00,IL1D_LLS01,IL1D_LLS02,IL1D_LLS03,IL1D_LLS04,IL1D_LLS05,IL1D_LLS06,IL1D_LLS07
     *  <SLV-T>    93h      A9h      [7:0]      IL1D_LLS08,IL1D_LLS09,IL1D_LLS10,IL1D_LLS11,IL1D_LLS12,IL1D_LLS13,IL1D_LLS14,IL1D_LLS15
     *  <SLV-T>    93h      AAh      [7:0]      IL1D_LLS16,IL1D_LLS17,IL1D_LLS18,IL1D_LLS19,IL1D_LLS20,IL1D_LLS21,IL1D_LLS22,IL1D_LLS23
     *  <SLV-T>    93h      ABh      [7:0]      IL1D_LLS24,IL1D_LLS25,IL1D_LLS26,IL1D_LLS27,IL1D_LLS28,IL1D_LLS29,IL1D_LLS30,IL1D_LLS31
     *  <SLV-T>    93h      ACh      [7:0]      IL1D_LLS32,IL1D_LLS33,IL1D_LLS34,IL1D_LLS35,IL1D_LLS36,IL1D_LLS37,IL1D_LLS38,IL1D_LLS39
     *  <SLV-T>    93h      ADh      [7:0]      IL1D_LLS40,IL1D_LLS41,IL1D_LLS42,IL1D_LLS43,IL1D_LLS44,IL1D_LLS45,IL1D_LLS46,IL1D_LLS47
     *  <SLV-T>    93h      AEh      [7:0]      IL1D_LLS48,IL1D_LLS49,IL1D_LLS50,IL1D_LLS51,IL1D_LLS52,IL1D_LLS53,IL1D_LLS54,IL1D_LLS55
     *  <SLV-T>    93h      AFh      [7:0]      IL1D_LLS56,IL1D_LLS57,IL1D_LLS58,IL1D_LLS59,IL1D_LLS60,IL1D_LLS61,IL1D_LLS62,IL1D_LLS63

     *  <SLV-T>    93h      B0h      [7:0]      IL1D_LAY00,IL1D_LAY01,IL1D_LAY02,IL1D_LAY03,IL1D_LAY04,IL1D_LAY05,IL1D_LAY06,IL1D_LAY07
     *  <SLV-T>    93h      B1h      [7:0]      IL1D_LAY08,IL1D_LAY09,IL1D_LAY10,IL1D_LAY11,IL1D_LAY12,IL1D_LAY13,IL1D_LAY14,IL1D_LAY15
     *  <SLV-T>    93h      B2h      [7:0]      IL1D_LAY16,IL1D_LAY17,IL1D_LAY18,IL1D_LAY19,IL1D_LAY20,IL1D_LAY21,IL1D_LAY22,IL1D_LAY23
     *  <SLV-T>    93h      B3h      [7:0]      IL1D_LAY24,IL1D_LAY25,IL1D_LAY26,IL1D_LAY27,IL1D_LAY28,IL1D_LAY29,IL1D_LAY30,IL1D_LAY31
     *  <SLV-T>    93h      B4h      [7:0]      IL1D_LAY32,IL1D_LAY33,IL1D_LAY34,IL1D_LAY35,IL1D_LAY36,IL1D_LAY37,IL1D_LAY38,IL1D_LAY39
     *  <SLV-T>    93h      B5h      [7:0]      IL1D_LAY40,IL1D_LAY41,IL1D_LAY42,IL1D_LAY43,IL1D_LAY44,IL1D_LAY45,IL1D_LAY46,IL1D_LAY47
     *  <SLV-T>    93h      B6h      [7:0]      IL1D_LAY48,IL1D_LAY49,IL1D_LAY50,IL1D_LAY51,IL1D_LAY52,IL1D_LAY53,IL1D_LAY54,IL1D_LAY55
     *  <SLV-T>    93h      B7h      [7:0]      IL1D_LAY56,IL1D_LAY57,IL1D_LAY58,IL1D_LAY59,IL1D_LAY60,IL1D_LAY61,IL1D_LAY62,IL1D_LAY63

     *  <SLV-T>    93h      B8h      [7:0]      IL1D_CHB00,IL1D_CHB01,IL1D_CHB02,IL1D_CHB03,IL1D_CHB04,IL1D_CHB05,IL1D_CHB06,IL1D_CHB07
     *  <SLV-T>    93h      B9h      [7:0]      IL1D_CHB08,IL1D_CHB09,IL1D_CHB10,IL1D_CHB11,IL1D_CHB12,IL1D_CHB13,IL1D_CHB14,IL1D_CHB15
     *  <SLV-T>    93h      BAh      [7:0]      IL1D_CHB16,IL1D_CHB17,IL1D_CHB18,IL1D_CHB19,IL1D_CHB20,IL1D_CHB21,IL1D_CHB22,IL1D_CHB23
     *  <SLV-T>    93h      BBh      [7:0]      IL1D_CHB24,IL1D_CHB25,IL1D_CHB26,IL1D_CHB27,IL1D_CHB28,IL1D_CHB29,IL1D_CHB30,IL1D_CHB31
     *  <SLV-T>    93h      BCh      [7:0]      IL1D_CHB32,IL1D_CHB33,IL1D_CHB34,IL1D_CHB35,IL1D_CHB36,IL1D_CHB37,IL1D_CHB38,IL1D_CHB39
     *  <SLV-T>    93h      BDh      [7:0]      IL1D_CHB40,IL1D_CHB41,IL1D_CHB42,IL1D_CHB43,IL1D_CHB44,IL1D_CHB45,IL1D_CHB46,IL1D_CHB47
     *  <SLV-T>    93h      BEh      [7:0]      IL1D_CHB48,IL1D_CHB49,IL1D_CHB50,IL1D_CHB51,IL1D_CHB52,IL1D_CHB53,IL1D_CHB54,IL1D_CHB55
     *  <SLV-T>    93h      BFh      [7:0]      IL1D_CHB56,IL1D_CHB57,IL1D_CHB58,IL1D_CHB59,IL1D_CHB60,IL1D_CHB61,IL1D_CHB62,IL1D_CHB63

     *  <SLV-T>    93h      C0h      [2:0]      IL1D_NUM_RF[2:0]
     */

    if (!plpList) {
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9D, data, 3) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    } else {
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9D, data, 36) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

    plpInfoRdy = (data[0] & 0x01) ? 1 : 0;
    plpInfoChg = (data[1] & 0x01) ? 1 : 0;
    l1dNumRf   = data[35] & 0x07;

    if(plpInfoRdy == 1) {
        if (plpInfoChg == 1) {
            /*
             * The registers of PLP_ID list are INVALID
             * Write OREGD_PLPINFO_UPD = 1 for start updating PLP_ID list
             */
            /*
             *  slave     Bank     Addr     Bit    default   Value          Name
             * ----------------------------------------------------------------------------------
             *  <SLV-T>   93h      9Ch      [0]    1'b0      1'b1           OREGD_PLPINFO_UPD
             */
			if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9C, 0x01) != SONY_CXD6801_RESULT_OK) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
            }
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
        }
    } else {
        /* The registers of PLP_ID list are INVALID */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    if ((data[2] & 0x7f) > 64) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }
    *pNumPLPs = data[2] & 0x7F;

    if (!plpList) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (((data[3 + i] >> (7 - j)) & 0x01)) {
                plpList[k].id = 8 * i + j;
                plpList[k].lls_flg = (data[11 + i] >> (7 - j)) & 0x01;
                plpList[k].layer = (data[19 + i] >> (7 - j)) & 0x01;
                plpList[k].chbond = l1dNumRf > 0 ? (data[27 + i] >> (7 - j)) & 0x01 : 0;
                k++;
            }
        }
    }
    if (k != *pNumPLPs) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_PLPError(sony_cxd6801_demod_t * pDemod,
                                                 uint8_t * pPLPError)
{
    uint8_t data;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_PLPError");

    if ((!pDemod) || (!pPLPError)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (CXD6801_SLVT_FreezeReg (pDemod) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x93 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave      Bank     Addr     Bit        Signal name
     * --------------------------------------------------------------
     *  <SLV-T>    93h      10h      [1]        IL1D_OK
     *  <SLV-T>    93h      9Dh      [0]        IPLPINFO_RDY
     */

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, &data, 1) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (!(data & 0x02)) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9D, &data, 1) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (!(data & 0x01)) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /*  slave    Bank    Addr    Bit       Name              Meaning
     * ---------------------------------------------------------------------------------
     *  <SLV-T>  93h     84h     [0]       IPLP_SEL_ERR      0:all PLPs are founded, 1:not found
     */

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x84, &data, 1) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    CXD6801_SLVT_UnFreezeReg (pDemod);

    *pPLPError = data & 0x01;

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_SelectedPLPValid(sony_cxd6801_demod_t * pDemod,
                                                         uint8_t plpValid[4])
{
	sony_cxd6801_result_t result;
    uint8_t data[6];

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_SelectedPLPValid");

    if ((!pDemod) || (!plpValid)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    plpValid[0] = 0;
    plpValid[1] = 0;
    plpValid[2] = 0;
    plpValid[3] = 0;

    /* Set SLV-T Bank : 0x93 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*
     *  slave      Bank     Addr     Bit        Signal name
     * --------------------------------------------------------------
     *  <SLV-T>    93h      10h      [1]        IL1D_OK
     *  <SLV-T>    93h      9Dh      [0]        IPLPINFO_RDY
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, data, 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (!(data[0] & 0x02)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9D, data, 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (!(data[0] & 0x01)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /*
     *  slave     Bank      Addr     Bit       SignalName
     * ---------------------------------------------------------------
     *  <SLV-T>   93h       80h      [7]       OREGD_PLP_ID_0_VALID
     *  <SLV-T>   93h       81h      [7]       OREGD_PLP_ID_1_VALID
     *  <SLV-T>   93h       82h      [7]       OREGD_PLP_ID_2_VALID
     *  <SLV-T>   93h       83h      [7]       OREGD_PLP_ID_3_VALID
     *  <SLV-T>   93h       84h      [0]       IPLP_SEL_ERR
     *  <SLV-T>   93h       85h      [0]       OREG_PLP_ID_AUTO
     */

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /* Confirm existence of selected PLP IDs */
    if (data[4] & 0x01) {
        /* PLP IDs are invalid */
        if ((data[0] & 0x80) && !(data[1] & 0x80) && !(data[2] & 0x80) && !(data[3] & 0x80) && (data[5] & 0x01)) {
            /*
             * If OREG_PLP_ID_AUTO == 1, the demodulator can automatically correct PLP ID in single PLP case.
             * This code checks that user selected PLP number is 1 and
             * the signal include only one PLP.
             */
            uint8_t numPLP = 0;

			result = sony_cxd6801_demod_atsc3_monitor_PLPList(pDemod, NULL, &numPLP);
			if ((result == SONY_CXD6801_RESULT_OK) && (numPLP == 1)) {
                plpValid[0] = 1;
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
            } else {
                SONY_CXD6801_TRACE_RETURN (result);
            }
        } else {
            /*
             * User specified some PLP IDs or OREG_PLP_ID_AUTO == 0.
             */
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
        }
    } else {
        /* Specified PLP IDs are valid. */
        plpValid[0] = (data[0] & 0x80) ? 1 : 0;
        plpValid[1] = (data[1] & 0x80) ? 1 : 0;
        plpValid[2] = (data[2] & 0x80) ? 1 : 0;
        plpValid[3] = (data[3] & 0x80) ? 1 : 0;
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
    }
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_SpectrumSense(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_terr_cable_spectrum_sense_t * pSense)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_SpectrumSense");

    if ((!pDemod) || (!pSense)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Freeze the register. */
	if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    result = cxd6801_isDemodLocked (pDemod);
	if (result != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
        SONY_CXD6801_TRACE_RETURN (result);
    }

    {
        uint8_t data = 0;

        /* Set SLV-T Bank : 0x90 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /*  slave     Bank    Addr    Bit          Name           Meaning
         * ---------------------------------------------------------------------------------
         *  <SLV-T>   90h     F3h     [0]          OREG_SINV      0:not invert,   1:invert
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF3, &data, 1) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        CXD6801_SLVT_UnFreezeReg (pDemod);

		if (pDemod->confSense == SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_INV) {
			*pSense = (data & 0x01) ? SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_NORMAL : SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_INV;
        } else {
			*pSense = (data & 0x01) ? SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_INV : SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_NORMAL;
        }
    }
	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_SNR(sony_cxd6801_demod_t * pDemod,
                                            int32_t * pSNR)
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_SNR");

    if ((!pDemod) || (!pSNR)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->atsc3EasState == SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_EAS) {
        /* Not supported in EAS state */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    {
        uint32_t reg;
        uint8_t data[3];
        uint8_t ofdmNsbf;
        uint32_t L;
        uint32_t S;
        int32_t C;

        /* Freeze registers */
		if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        result = cxd6801_isDemodLocked (pDemod);
		if (result != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Set SLV-T Bank : 0x90 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        /*  slave     Bank   Addr    Bit          Name
         * ------------------------------------------------------------
         *  <SLV-T>   90h    28h     [7:0]        IREG_SNMON_OD[23:16]
         *  <SLV-T>   90h    29h     [7:0]        IREG_SNMON_OD[15:8]
         *  <SLV-T>   90h    2Ah     [7:0]        IREG_SNMON_OD[7:0]
         *  <SLV-T>   90h    58h     [7:0]        IREG_OFDM_NSBF[7:0]
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x28, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x58, &ofdmNsbf, 1) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        CXD6801_SLVT_UnFreezeReg (pDemod);

        reg = (data[0] << 16) | (data[1] << 8) | data[2];

        /*
         *----------------+--------+--------+-------
         * IREG_OFDM_NSBF | L      | S      | C
         *----------------+--------+--------+-------
         *  0             | 419404 | 519913 | 33.80
         *  not 0         | 704089 | 971204 | 35.80
         *----------------+--------+--------+-------
         */

        if (ofdmNsbf == 0x00) {
            L = 419404;
            S = 519913;
            C = 33800;
        } else {
            L = 704089;
            S = 971204;
            C = 35800;
        }

        /*
         * if IREG_SNMON_OD > L
         *    SNR[dB] = 10 * log10{L / (S - L)} + C
         *            = 10 * (log10(L) - log10(S - L)) + C
         * else
         *    SNR[dB] = 10 * log10{IREG_SNMON_OD / (S - IREG_SNMON_OD)} + C
         *            = 10 * (log10(IREG_SNMON_OD) - log10(S - IREG_SNMON_OD)) + C
         * sony_cxd6801_log10 returns log10(x) * 100
         * Therefore SNR(dB) * 1000 :
         *     = 10 * 10 * (sony_cxd6801_log10(IREG_SNMON_OD) - sony_cxd6801_log10(S - IREG_SNMON_OD) + C * 1000
         */
        if (reg > L) {
            reg = L;
        }

        *pSNR = 10 * 10 * ((int32_t) sony_cxd6801_math_log10 (reg) - (int32_t) sony_cxd6801_math_log10 (S - reg));
        *pSNR += C;
    }
	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_PreLDPCBER(sony_cxd6801_demod_t * pDemod,
                                                   uint32_t ber[4])
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_PreLDPCBER");

    if ((!pDemod) || (!ber)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x94 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x94) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    {
        uint8_t data[18];
        uint8_t valid[4] = {0};
        uint32_t bitError[4] = {0};
        uint32_t periodExp[4] = {0};
        uint32_t n_ldpc[4] = {0};
        sony_cxd6801_atsc3_plp_fec_t fecType[4];
        int i = 0;

        /*  slave     Bank    Addr   Bit              Name
         * ------------------------------------------------------------
         *  <SLV-T>   94h     30h    [7:4]            OREG_LBER_MES_0[3:0]
         *  <SLV-T>   94h     30h    [3:0]            OREG_LBER_MES_1[3:0]
         *  <SLV-T>   94h     31h    [7:4]            OREG_LBER_MES_2[3:0]
         *  <SLV-T>   94h     31h    [3:0]            OREG_LBER_MES_3[3:0]
         *  <SLV-T>   94h     32h    [4]              ILBER0_VALID
         *  <SLV-T>   94h     32h    [3:0]            ILBER0_BITERR[27:24]
         *  <SLV-T>   94h     33h    [7:0]            ILBER0_BITERR[23:16]
         *  <SLV-T>   94h     34h    [7:0]            ILBER0_BITERR[15:8]
         *  <SLV-T>   94h     35h    [7:0]            ILBER0_BITERR[7:0]
         *  <SLV-T>   94h     36h    [4]              ILBER1_VALID
         *  <SLV-T>   94h     36h    [3:0]            ILBER1_BITERR[27:24]
         *  <SLV-T>   94h     37h    [7:0]            ILBER1_BITERR[23:16]
         *  <SLV-T>   94h     38h    [7:0]            ILBER1_BITERR[15:8]
         *  <SLV-T>   94h     39h    [7:0]            ILBER1_BITERR[7:0]
         *  <SLV-T>   94h     3Ah    [4]              ILBER2_VALID
         *  <SLV-T>   94h     3Ah    [3:0]            ILBER2_BITERR[27:24]
         *  <SLV-T>   94h     3Bh    [7:0]            ILBER2_BITERR[23:16]
         *  <SLV-T>   94h     3Ch    [7:0]            ILBER2_BITERR[15:8]
         *  <SLV-T>   94h     3Dh    [7:0]            ILBER2_BITERR[7:0]
         *  <SLV-T>   94h     3Eh    [4]              ILBER3_VALID
         *  <SLV-T>   94h     3Eh    [3:0]            ILBER3_BITERR[27:24]
         *  <SLV-T>   94h     3Fh    [7:0]            ILBER3_BITERR[23:16]
         *  <SLV-T>   94h     40h    [7:0]            ILBER3_BITERR[15:8]
         *  <SLV-T>   94h     41h    [7:0]            ILBER3_BITERR[7:0]
         */

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x30, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        periodExp[0] = (data[0] >> 4) & 0x0F;
        periodExp[1] = data[0] & 0x0F;
        periodExp[2] = (data[1] >> 4) & 0x0F;
        periodExp[3] = data[1] & 0x0F;
        for (i = 0; i < 4; i++) {
            valid[i] = data[i * 4 + 2] & 0x10;
            bitError[i] = ((data[i * 4 + 2] & 0x0F) << 24) | (data[i * 4 + 3] << 16) | (data[i * 4 + 4] << 8) | data[i * 4 + 5];
        }

        /* Set SLV-T Bank : 0x93 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Get the PLP type (Normal/Short).
         *  slave     Bank    Addr   Bit              Name
         * ------------------------------------------------------------
         *  <SLV-T>   93h     90h    [7:4]            IL1D_PLP_FEC_TYPE_0[3:0]
         *  <SLV-T>   93h     90h    [3:0]            IL1D_PLP_FEC_TYPE_1[3:0]
         *  <SLV-T>   93h     91h    [7:4]            IL1D_PLP_FEC_TYPE_2[3:0]
         *  <SLV-T>   93h     91h    [3:0]            IL1D_PLP_FEC_TYPE_3[3:0]
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x90, data, 2) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        fecType[0] = (sony_cxd6801_atsc3_plp_fec_t)((data[0] >> 4) & 0x0F);
        fecType[1] = (sony_cxd6801_atsc3_plp_fec_t)(data[0] & 0x0F);
        fecType[2] = (sony_cxd6801_atsc3_plp_fec_t)((data[1] >> 4) & 0x0F);
        fecType[3] = (sony_cxd6801_atsc3_plp_fec_t)(data[1] & 0x0F);

        CXD6801_SLVT_UnFreezeReg (pDemod);

        for (i = 0;i < 4;i++) {
            if (!valid[i]) {
				ber[i] = SONY_CXD6801_DEMOD_ATSC3_MONITOR_PRELDPCBER_INVALID;
                continue;
            }
            switch (fecType[i]) {
			case SONY_CXD6801_ATSC3_PLP_FEC_BCH_LDPC_16K:
			case SONY_CXD6801_ATSC3_PLP_FEC_CRC_LDPC_16K:
			case SONY_CXD6801_ATSC3_PLP_FEC_LDPC_16K:
                n_ldpc[i] = 16200;
                break;
			case SONY_CXD6801_ATSC3_PLP_FEC_BCH_LDPC_64K:
			case SONY_CXD6801_ATSC3_PLP_FEC_CRC_LDPC_64K:
			case SONY_CXD6801_ATSC3_PLP_FEC_LDPC_64K:
                n_ldpc[i] = 64800;
                break;
            default:
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
            }

            if (bitError[i] > ((1U << periodExp[i]) * n_ldpc[i])) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
            }

            {
                uint32_t div = 0;
                uint32_t Q = 0;
                uint32_t R = 0;

                if (periodExp[i] >= 4) {
                    /*
                      BER = bitError * 10000000 / (2^N * n_ldpc)
                          = bitError * 3125 / (2^(N-4) * (n_ldpc / 200))
                          (NOTE: 10000000 / 2^4 / 200 = 3125)
                          = bitError * 5 * 625 / (2^(N-4) * (n_ldpc / 200))
                          (Divide in 2 steps to prevent overflow.)
                    */
                    div = (1U << (periodExp[i] - 4)) * (n_ldpc[i] / 200);

                    Q = (bitError[i] * 5) / div;
                    R = (bitError[i] * 5) % div;

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
                    div = (1U << periodExp[i]) * (n_ldpc[i] / 200);

                    Q = (bitError[i] * 10) / div;
                    R = (bitError[i] * 10) % div;

                    R *= 5000;
                    Q = Q * 5000 + R / div;
                    R = R % div;
                }

                /* rounding */
                if (R >= div/2) {
                    ber[i] = Q + 1;
                }
                else {
                    ber[i] = Q;
                }
            }
        }

    }
	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_PreBCHBER(sony_cxd6801_demod_t * pDemod,
                                                  uint32_t ber[4])
{
    uint8_t valid[4] = {0};
    uint32_t bitError[4] = {0};
    uint32_t periodExp[4] = {0};
    uint32_t n_bch[4] = {0};
    int i = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_PreBCHBER");

    if ((!pDemod) || (!ber)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Monitor the BER parameters. */
    {
        uint8_t data[19];
        sony_cxd6801_atsc3_plp_fec_t plpFecType[4];
        sony_cxd6801_atsc3_plp_cod_t plpCr[4];
        static const uint16_t nBCHBitsLookup[2][12] = {
          /* 2_15   3_15   4_15   5_15   6_15   7_15   8_15   9_15  10_15  11_15  12_15  13_15*/
            {2160,  3240,  4320,  5400,  6480,  7560,  8640,  9720, 10800, 11880, 12960, 14040}, /* N_ldpc = 16200 */
            {8640, 12960, 17280, 21600, 25920, 30240, 34560, 38880, 43200, 47520, 51840, 56160}, /* N_ldpc = 64800 */
        };

        /* Freeze registers */
		if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Set SLV-T Bank : 0x94 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x94) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        /*  slave     Bank    Addr    Bit            Name
         * ------------------------------------------------------------
         *  <SLV-T>   94h     4Ah     [7:4]          OREG_BBER_MES_0[3:0]
         *  <SLV-T>   94h     4Ah     [3:0]          OREG_BBER_MES_1[3:0]
         *  <SLV-T>   94h     4Bh     [7:4]          OREG_BBER_MES_2[3:0]
         *  <SLV-T>   94h     4Bh     [3:0]          OREG_BBER_MES_3[3:0]
         *  <SLV-T>   94h     4Dh     [0]            IBBER0_VALID
         *  <SLV-T>   94h     4Eh     [5:0]          IBBER0_BITERR[21:16]
         *  <SLV-T>   94h     4Fh     [7:0]          IBBER0_BITERR[15:8]
         *  <SLV-T>   94h     50h     [7:0]          IBBER0_BITERR[7:0]
         *  <SLV-T>   94h     51h     [0]            IBBER1_VALID
         *  <SLV-T>   94h     52h     [5:0]          IBBER1_BITERR[21:16]
         *  <SLV-T>   94h     53h     [7:0]          IBBER1_BITERR[15:8]
         *  <SLV-T>   94h     54h     [7:0]          IBBER1_BITERR[7:0]
         *  <SLV-T>   94h     55h     [0]            IBBER2_VALID
         *  <SLV-T>   94h     56h     [5:0]          IBBER2_BITERR[21:16]
         *  <SLV-T>   94h     57h     [7:0]          IBBER2_BITERR[15:8]
         *  <SLV-T>   94h     58h     [7:0]          IBBER2_BITERR[7:0]
         *  <SLV-T>   94h     59h     [0]            IBBER3_VALID
         *  <SLV-T>   94h     5Ah     [5:0]          IBBER3_BITERR[21:16]
         *  <SLV-T>   94h     5Bh     [7:0]          IBBER3_BITERR[15:8]
         *  <SLV-T>   94h     5Ch     [7:0]          IBBER3_BITERR[7:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4A, data, sizeof(data)) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        periodExp[0] = (data[0] >> 4) & 0x0F;
        periodExp[1] = data[0] & 0x0F;
        periodExp[2] = (data[1] >> 4) & 0x0F;
        periodExp[3] = data[1] & 0x0F;

        for (i = 0; i < 4; i++) {
            valid[i] = data[i * 4 + 3] & 0x01;
            bitError[i] = ((data[i * 4 + 4] & 0x3F) << 16) | (data[i * 4 + 5] << 8) | data[i * 4 + 6];
        }

        /* Set SLV-T Bank : 0x93 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Get the PLP FEC type (Normal/Short).
         *  slave     Bank    Addr   Bit              Name
         * ------------------------------------------------------------
         *  <SLV-T>   93h     90h    [7:4]            IL1D_PLP_FEC_TYPE_0[3:0]
         *  <SLV-T>   93h     90h    [3:0]            IL1D_PLP_FEC_TYPE_1[3:0]
         *  <SLV-T>   93h     91h    [7:4]            IL1D_PLP_FEC_TYPE_2[3:0]
         *  <SLV-T>   93h     91h    [3:0]            IL1D_PLP_FEC_TYPE_3[3:0]
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x90, data, 2) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        plpFecType[0] = (sony_cxd6801_atsc3_plp_fec_t) ((data[0] >> 4) & 0x0F);
        plpFecType[1] = (sony_cxd6801_atsc3_plp_fec_t) (data[0] & 0x0F);
        plpFecType[2] = (sony_cxd6801_atsc3_plp_fec_t) ((data[1] >> 4) & 0x0F);
        plpFecType[3] = (sony_cxd6801_atsc3_plp_fec_t) (data[1] & 0x0F);

        /* Get the PLP code rate.
         *  slave     Bank    Addr   Bit            Name
         * ------------------------------------------------------------
         *  <SLV-T>   93h     94h    [7:4]          IL1D_PLP_COD_0[3:0]
         *  <SLV-T>   93h     94h    [3:0]          IL1D_PLP_COD_1[3:0]
         *  <SLV-T>   93h     95h    [7:4]          IL1D_PLP_COD_2[3:0]
         *  <SLV-T>   93h     95h    [3:0]          IL1D_PLP_COD_3[3:0]
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x94, data, 2) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        plpCr[0] = (sony_cxd6801_atsc3_plp_cod_t) ((data[0] >> 4) & 0x0F);
        plpCr[1] = (sony_cxd6801_atsc3_plp_cod_t) (data[0] & 0x0F);
        plpCr[2] = (sony_cxd6801_atsc3_plp_cod_t) ((data[1] >> 4) & 0x0F);
        plpCr[3] = (sony_cxd6801_atsc3_plp_cod_t) (data[1] & 0x0F);

        CXD6801_SLVT_UnFreezeReg (pDemod);

        /* Confirm FEC Type / Code Rate */
        for (i = 0; i < 4; i++) {
            if (!valid[i]) {
                continue;
            }
			if ((plpFecType[i] > SONY_CXD6801_ATSC3_PLP_FEC_LDPC_64K) || (plpCr[i] > SONY_CXD6801_ATSC3_PLP_COD_13_15)) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
            }
            /* Pre-BCH BER is only valid when outercode is BCH. */
			if ((plpFecType[i] != SONY_CXD6801_ATSC3_PLP_FEC_BCH_LDPC_16K) && (plpFecType[i] != SONY_CXD6801_ATSC3_PLP_FEC_BCH_LDPC_64K)) {
                valid[i] = 0;
                continue;
            }

            n_bch[i] = nBCHBitsLookup[plpFecType[i]][plpCr[i]];
        }

    }

    {
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        for (i = 0; i < 4; i++) {

            if (!valid[i]) {
				ber[i] = SONY_CXD6801_DEMOD_ATSC3_MONITOR_PREBCHBER_INVALID;
                continue;
            }

            if (bitError[i] > ((1U << periodExp[i]) * n_bch[i])) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
            }
            if (periodExp[i] >= 6) {
                /*
                BER = bitError * 1000000000 / (2^N * n_bch)
                    = bitError * 390625 / (2^(N-6) * (n_bch / 40))
                    (NOTE: 1000000000 / 2^6 / 40 = 390625)
                    = bitError * 625 * 625 / (2^(N-6) * (n_bch / 40))
                    (Divide in 2 steps to prevent overflow.)
                */
                div = (1U << (periodExp[i] - 6)) * (n_bch[i] / 40);

                Q = (bitError[i] * 625) / div;
                R = (bitError[i] * 625) % div;

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
                div = (1U << periodExp[i]) * (n_bch[i] / 40);

                Q = (bitError[i] * 1000) / div;
                R = (bitError[i] * 1000) % div;

                R *= 25000;
                Q = Q * 25000 + R / div;
                R = R % div;
            }

            /* rounding */
            if (R >= div/2) {
                ber[i] = Q + 1;
            }
            else {
                ber[i] = Q;
            }
        }
    }
	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_PostBCHFER(sony_cxd6801_demod_t * pDemod,
                                                   uint32_t fer[4])
{
    uint8_t valid[4] = {0};
    uint32_t fecError[4] = {0};
    uint32_t period[4]= {0};
    int i = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_PostBCHFER");

    if ((!pDemod) || (!fer)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x94 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x94) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    {
        uint8_t data[8];

        /*  slave      Bank    Addr    Bit            Name
         * ------------------------------------------------------------
         *  <SLV-T>    94h     5Dh     [6:0]          IBBER0_FBERR[14:8]
         *  <SLV-T>    94h     5Eh     [7:0]          IBBER0_FBERR[7:0]
         *  <SLV-T>    94h     5Fh     [6:0]          IBBER1_FBERR[14:8]
         *  <SLV-T>    94h     60h     [7:0]          IBBER1_FBERR[7:0]
         *  <SLV-T>    94h     61h     [6:0]          IBBER2_FBERR[14:8]
         *  <SLV-T>    94h     62h     [7:0]          IBBER2_FBERR[7:0]
         *  <SLV-T>    94h     63h     [6:0]          IBBER3_FBERR[14:8]
         *  <SLV-T>    94h     64h     [7:0]          IBBER3_FBERR[7:0]
         *  <SLV-T>    94h     4Dh     [0]            IBBER0_VALID
         *  <SLV-T>    94h     51h     [0]            IBBER1_VALID
         *  <SLV-T>    94h     55h     [0]            IBBER2_VALID
         *  <SLV-T>    94h     59h     [0]            IBBER3_VALID
         */

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5D, data, sizeof(data)) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        for (i = 0; i < 4; i++) {
            fecError[i] = ((data[i * 2] & 0x7F) << 8) | (data[i * 2 + 1]);
        }

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4D, data, 1) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        valid[0] = data[0] & 0x01;
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x51, data, 1) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        valid[1] = data[0] & 0x01;
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x55, data, 1) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        valid[2] = data[0] & 0x01;
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x59, data, 1) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        valid[3] = data[0] & 0x01;

        /* Read measurement period.
         *  slave     Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         *  <SLV-T>   94h     4Ah     [7:4]        OREG_BBER_MES_0[3:0]
         *  <SLV-T>   94h     4Ah     [3:0]        OREG_BBER_MES_1[3:0]
         *  <SLV-T>   94h     4Bh     [7:4]        OREG_BBER_MES_2[3:0]
         *  <SLV-T>   94h     4Bh     [3:0]        OREG_BBER_MES_3[3:0]
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4A, data, 2) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        CXD6801_SLVT_UnFreezeReg (pDemod);

        /* Period = 2^BBER_MES */
        period[0] = 1 << ((data[0] >> 4) & 0x0F);
        period[1] = 1 << (data[0] & 0x0F);
        period[2] = 1 << ((data[1] >> 4) & 0x0F);
        period[3] = 1 << (data[1] & 0x0F);
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

        for (i = 0; i < 4; i++) {
            if (!valid[i]) {
				fer[i] = SONY_CXD6801_DEMOD_ATSC3_MONITOR_POSTBCHFER_INVALID;
                continue;
            }

            if ((period[i] == 0) || (fecError[i] > period[i])) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
            }
            div = period[i];
            Q = (fecError[i] * 1000) / div;
            R = (fecError[i] * 1000) % div;

            R *= 1000;
            Q = Q * 1000 + R / div;
            R = R % div;

            /* rounding */
            if ((div != 1) && (R >= div/2)) {
                fer[i] = Q + 1;
            }
            else {
                fer[i] = Q;
            }
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_BBPacketErrorNumber(sony_cxd6801_demod_t * pDemod,
                                                            uint32_t pen[4])
{
    uint8_t data[12];
    int i = 0;
    int invalidCount = 0;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_BBPacketErrorNumber");

    if ((!pDemod) || (!pen)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x94 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x94) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

   /*  slave     Bank    Addr   Bit              Name
    * ------------------------------------------------------------
    *  <SLV-T>   94h     94h    [0]            IREG_BER1SEC_VALID_0
    *  <SLV-T>   94h     95h    [7:0]          IREG_BER1SEC_ERRNUM_0[15:8]
    *  <SLV-T>   94h     96h    [7:0]          IREG_BER1SEC_ERRNUM_0[7:0]
    *  <SLV-T>   94h     97h    [0]            IREG_BER1SEC_VALID_1
    *  <SLV-T>   94h     98h    [7:0]          IREG_BER1SEC_ERRNUM_1[15:8]
    *  <SLV-T>   94h     99h    [7:0]          IREG_BER1SEC_ERRNUM_1[7:0]
    *  <SLV-T>   94h     9ah    [0]            IREG_BER1SEC_VALID_2
    *  <SLV-T>   94h     9bh    [7:0]          IREG_BER1SEC_ERRNUM_2[15:8]
    *  <SLV-T>   94h     9ch    [7:0]          IREG_BER1SEC_ERRNUM_2[7:0]
    *  <SLV-T>   94h     9dh    [0]            IREG_BER1SEC_VALID_3
    *  <SLV-T>   94h     9eh    [7:0]          IREG_BER1SEC_ERRNUM_3[15:8]
    *  <SLV-T>   94h     9fh    [7:0]          IREG_BER1SEC_ERRNUM_3[7:0]
    */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x94, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    for (i = 0; i < 4; i++) {
        if (!(data[i * 3] & 0x01) ) {
            pen[i] = 0;
            invalidCount++;
            continue;
        }

        pen[i] =  ((data[i * 3 + 1] << 8) | data[i * 3 + 2]);
    }

    if (invalidCount == 4) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_SamplingOffset(sony_cxd6801_demod_t * pDemod,
                                                       int32_t * pPPM)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_SamplingOffset");

    if ((!pDemod) || (!pPPM)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->atsc3EasState == SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_EAS) {
        /* Not supported in EAS state */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    {
		sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
        uint8_t ctlValReg[5];
        uint8_t nominalRateReg[5];
        uint32_t trlCtlVal = 0;
        uint32_t trcgNominalRate = 0;
        int32_t num;
        int32_t den;
        int8_t diffUpper = 0;

        /* Freeze registers */
		if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        result = cxd6801_isDemodLocked (pDemod);
		if (result != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
            SONY_CXD6801_TRACE_RETURN (result);
        }

        /* Set SLV-T Bank : 0x90 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /*  slave     Bank    Addr    Bit              name
         * ---------------------------------------------------------------
         *  <SLV-T>   90h     52h     [6:0]      IREG_TRL_CTLVAL_S[38:32]
         *  <SLV-T>   90h     53h     [7:0]      IREG_TRL_CTLVAL_S[31:24]
         *  <SLV-T>   90h     54h     [7:0]      IREG_TRL_CTLVAL_S[23:16]
         *  <SLV-T>   90h     55h     [7:0]      IREG_TRL_CTLVAL_S[15:8]
         *  <SLV-T>   90h     56h     [7:0]      IREG_TRL_CTLVAL_S[7:0]
         *  <SLV-T>   90h     9Fh     [6:0]      OREG_TRCG_NOMINALRATE[38:32]
         *  <SLV-T>   90h     A0h     [7:0]      OREG_TRCG_NOMINALRATE[31:24]
         *  <SLV-T>   90h     A1h     [7:0]      OREG_TRCG_NOMINALRATE[23:16]
         *  <SLV-T>   90h     A2h     [7:0]      OREG_TRCG_NOMINALRATE[15:8]
         *  <SLV-T>   90h     A3h     [7:0]      OREG_TRCG_NOMINALRATE[7:0]
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x52, ctlValReg, sizeof (ctlValReg)) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRateReg, sizeof (nominalRateReg)) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Unfreeze registers. */
        CXD6801_SLVT_UnFreezeReg (pDemod);

        diffUpper = (ctlValReg[0] & 0x7F) - (nominalRateReg[0] & 0x7F);

        /* Confirm offset range */
        if ((diffUpper < -1) || (diffUpper > 1)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
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

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_FecModCod(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_atsc3_plp_fecmodcod_t fecmodcod[4])
{
	sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint8_t plpValid[4];
    uint8_t data[6] = {0};
    int i = 0;
    uint8_t tempFecType[4];
    uint8_t tempMod[4];
    uint8_t tempCod[4];

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_FecModCod");

    if ((!pDemod) || (!fecmodcod)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Confirm existence of selected PLP IDs */
    result = sony_cxd6801_demod_atsc3_monitor_SelectedPLPValid (pDemod, plpValid);
	if (result != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
        SONY_CXD6801_TRACE_RETURN (result);
    }

    /* Set SLV-T Bank : 0x93 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave     Bank      Addr     Bit      Signal name
     * --------------------------------------------------------------
     *  <SLV-T>   93h       90h      [7:4]    IL1D_PLP_FEC_TYPE_0[3:0]
     *  <SLV-T>   93h       90h      [3:0]    IL1D_PLP_FEC_TYPE_1[3:0]
     *  <SLV-T>   93h       91h      [7:4]    IL1D_PLP_FEC_TYPE_2[3:0]
     *  <SLV-T>   93h       91h      [3:0]    IL1D_PLP_FEC_TYPE_3[3:0]
     *  <SLV-T>   93h       92h      [7:4]    IL1D_PLP_MOD_0[3:0]
     *  <SLV-T>   93h       92h      [3:0]    IL1D_PLP_MOD_1[3:0]
     *  <SLV-T>   93h       93h      [7:4]    IL1D_PLP_MOD_2[3:0]
     *  <SLV-T>   93h       93h      [3:0]    IL1D_PLP_MOD_3[3:0]
     *  <SLV-T>   93h       94h      [7:4]    IL1D_PLP_COD_0[3:0]
     *  <SLV-T>   93h       94h      [3:0]    IL1D_PLP_COD_1[3:0]
     *  <SLV-T>   93h       95h      [7:4]    IL1D_PLP_COD_2[3:0]
     *  <SLV-T>   93h       95h      [3:0]    IL1D_PLP_COD_3[3:0]
     */

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x90, data, sizeof(data)) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    CXD6801_SLVT_UnFreezeReg (pDemod);

    tempFecType[0] = (data[0] >> 4) & 0x0F;
    tempFecType[1] = data[0] & 0x0F;
    tempFecType[2] = (data[1] >> 4) & 0x0F;
    tempFecType[3] = data[1] & 0x0F;
    tempMod[0] = (data[2] >> 4) & 0x0F;
    tempMod[1] = data[2] & 0x0F;
    tempMod[2] = (data[3] >> 4) & 0x0F;
    tempMod[3] = data[3] & 0x0F;
    tempCod[0] = (data[4] >> 4) & 0x0F;
    tempCod[1] = data[4] & 0x0F;
    tempCod[2] = (data[5] >> 4) & 0x0F;
    tempCod[3] = data[5] & 0x0F;



    for (i = 0; i < 4; i ++) {
        if (!plpValid[i]) {
            fecmodcod[i].valid = 0;
            continue;
        }
        fecmodcod[i].valid = 1;
		fecmodcod[i].fec_type = (sony_cxd6801_atsc3_plp_fec_t)tempFecType[i];
		fecmodcod[i].mod = (sony_cxd6801_atsc3_plp_mod_t)tempMod[i];
		fecmodcod[i].cod = (sony_cxd6801_atsc3_plp_cod_t)tempCod[i];
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_AveragedPreBCHBER(sony_cxd6801_demod_t * pDemod,
                                                          uint32_t ber[4])
{
    uint8_t rdata[128];
    uint8_t invalid[4] = {0};
    uint8_t periodType[4] = {0};
    uint32_t bitError[4] = {0};
    int hNum[4] = {0};
    uint32_t periodExp[4] = {0};
    uint32_t periodExpData[4] = {0};
    uint32_t periodNum[4] = {0};
    uint32_t n_bch[4] = {0};
    int i;

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_AveragedPreBCHBER");

    if ((!pDemod) || (!ber)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x91 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x91) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr     Bit      Name
     * -----------------------------------------------------
     *  <SLV-T>   91h     7Ch      [0]      OREG_BER_MODE_SEL_0
     *  <SLV-T>   91h     7Dh      [1:0]    IREG_BER_UNEXP_0[1:0]
     *  <SLV-T>   91h     80h      [0]      OREG_BER_MODE_SEL_1
     *  <SLV-T>   91h     81h      [1:0]    IREG_BER_UNEXP_1[1:0]
     *  <SLV-T>   91h     84h      [0]      OREG_BER_MODE_SEL_2
     *  <SLV-T>   91h     85h      [1:0]    IREG_BER_UNEXP_2[1:0]
     *  <SLV-T>   91h     88h      [0]      OREG_BER_MODE_SEL_3
     *  <SLV-T>   91h     89h      [1:0]    IREG_BER_UNEXP_3[1:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x7C, rdata, 2) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    periodType[0] = rdata[0] & 0x01;
    if ((rdata[1] & 0x03) != 0x00) {
        invalid[0] = 1;
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, rdata, 2) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    periodType[1] = rdata[0] & 0x01;
    if ((rdata[1] & 0x03) != 0x00) {
        invalid[1] = 1;
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x84, rdata, 2) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    periodType[2] = rdata[0] & 0x01;
    if ((rdata[1] & 0x03) != 0x00) {
        invalid[2] = 1;
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x88, rdata, 2) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    periodType[3] = rdata[0] & 0x01;
    if ((rdata[1] & 0x03) != 0x00) {
        invalid[3] = 1;
    }

    /* Freeze registers */
	if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x02 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave     Bank    Addr    Bit     Name
     * ---------------------------------------------------------------------------------
     *  <SLV-T>   02h     10h     [7:0]    IREG_BER_VALID0,IREG_BITERR0[22:16]
     *  <SLV-T>   02h     11h     [7:0]    IREG_BITERR0[15:8]
     *  <SLV-T>   02h     12h     [7:0]    IREG_BITERR0[7:0]
     *  <SLV-T>   02h     13h     [4:0]    IREG_PERIOD0[4:0]
     *  ...
     *  <SLV-T>   02h     2Ch     [7:0]    IREG_BER_VALID7,IREG_BITERR7[22:16]
     *  <SLV-T>   02h     2Dh     [7:0]    IREG_BITERR7[15:8]
     *  <SLV-T>   02h     2Eh     [7:0]    IREG_BITERR7[7:0]
     *  <SLV-T>   02h     2Fh     [4:0]    IREG_PERIOD7[4:0]
     *
     *  <SLV-T>   02h     53h     [7:0]    IREG_PLP1_BER_VALID1,IREG_PLP1_BITERR0[22:16]
     *  <SLV-T>   02h     54h     [7:0]    IREG_PLP1_BITERR0[15:8]
     *  <SLV-T>   02h     55h     [7:0]    IREG_PLP1_BITERR0[7:0]
     *  <SLV-T>   02h     56h     [4:0]    IREG_PLP1_PERIOD0[4:0]
     *  ...
     *  <SLV-T>   02h     6Fh     [7:0]    IREG_PLP1_BER_VALID7,IREG_PLP1_BITERR7[22:16]
     *  <SLV-T>   02h     70h     [7:0]    IREG_PLP1_BITERR7[15:8]
     *  <SLV-T>   02h     71h     [7:0]    IREG_PLP1_BITERR7[7:0]
     *  <SLV-T>   02h     72h     [4:0]    IREG_PLP1_PERIOD7[4:0]
     *
     *  <SLV-T>   02h     73h     [7:0]    IREG_PLP2_BER_VALID0,IREG_PLP1_BITERR0[22:16]
     *  <SLV-T>   02h     74h     [7:0]    IREG_PLP2_BITERR0[15:8]
     *  <SLV-T>   02h     75h     [7:0]    IREG_PLP2_BITERR0[7:0]
     *  <SLV-T>   02h     76h     [4:0]    IREG_PLP2_PERIOD0[4:0]
     *  ...
     *  <SLV-T>   02h     8Fh     [7:0]    IREG_PLP2_BER_VALID7,IREG_PLP1_BITERR7[22:16]
     *  <SLV-T>   02h     90h     [7:0]    IREG_PLP2_BITERR7[15:8]
     *  <SLV-T>   02h     91h     [7:0]    IREG_PLP2_BITERR7[7:0]
     *  <SLV-T>   02h     92h     [4:0]    IREG_PLP2_PERIOD7[4:0]
     *
     *  <SLV-T>   02h     93h     [7:0]    IREG_PLP3_BER_VALID0,IREG_PLP1_BITERR0[22:16]
     *  <SLV-T>   02h     94h     [7:0]    IREG_PLP3_BITERR0[15:8]
     *  <SLV-T>   02h     95h     [7:0]    IREG_PLP3_BITERR0[7:0]
     *  <SLV-T>   02h     96h     [4:0]    IREG_PLP3_PERIOD0[4:0]
     *  ...
     *  <SLV-T>   02h     AFh     [7:0]    IREG_PLP3_BER_VALID7,IREG_PLP1_BITERR7[22:16]
     *  <SLV-T>   02h     B0h     [7:0]    IREG_PLP3_BITERR7[15:8]
     *  <SLV-T>   02h     B1h     [7:0]    IREG_PLP3_BITERR7[7:0]
     *  <SLV-T>   02h     B2h     [4:0]    IREG_PLP3_PERIOD7[4:0]
     */

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, rdata, 32) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x53, rdata + 32, 96) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }


    for (i = 0; i < 4; i++) {
        for (hNum[i] = 0; hNum[i] < 8; hNum[i]++) {
            /* Check IREG_BER_VALIDX */
            if ((rdata[hNum[i] * 4 + i * 32] & 0x80) == 0x00) {
                break;
            }

            bitError[i] += ((rdata[hNum[i] * 4 + i * 32] & 0x7F) << 16) + (rdata[hNum[i] * 4 + i * 32 + 1] << 8) + rdata[hNum[i] * 4 + i * 32 + 2];
        }

        if (hNum[i] == 0) {
            /* There are no valid history */
            invalid[i] = 1;
        }
    }

    /* Monitor the BER parameters. */
    {
        uint8_t data[2];
		sony_cxd6801_atsc3_plp_fec_t plpFecType[4];
		sony_cxd6801_atsc3_plp_cod_t plpCr[4];
        static const uint16_t nBCHBitsLookup[2][12] = {
          /* 2_15   3_15   4_15   5_15   6_15   7_15   8_15   9_15  10_15  11_15  12_15  13_15*/
            {2160,  3240,  4320,  5400,  6480,  7560,  8640,  9720, 10800, 11880, 12960, 14040}, /* N_ldpc = 16200 */
            {8640, 12960, 17280, 21600, 25920, 30240, 34560, 38880, 43200, 47520, 51840, 56160}, /* N_ldpc = 64800 */
        };

        /* Set SLV-T Bank : 0x93 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Get the PLP FEC type (Normal/Short).
         *  slave     Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         *  <SLV-T>   93h     90h     [7:4]            IL1D_PLP_FEC_TYPE_0[3:0]
         *  <SLV-T>   93h     90h     [3:0]            IL1D_PLP_FEC_TYPE_1[3:0]
         *  <SLV-T>   93h     91h     [7:4]            IL1D_PLP_FEC_TYPE_2[3:0]
         *  <SLV-T>   93h     91h     [3:0]            IL1D_PLP_FEC_TYPE_3[3:0]
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x90, data, 2) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        plpFecType[0] = (sony_cxd6801_atsc3_plp_fec_t) ((data[0] >> 4) & 0x0F);
        plpFecType[1] = (sony_cxd6801_atsc3_plp_fec_t) (data[0] & 0x0F);
        plpFecType[2] = (sony_cxd6801_atsc3_plp_fec_t) ((data[1] >> 4) & 0x0F);
        plpFecType[3] = (sony_cxd6801_atsc3_plp_fec_t) (data[1] & 0x0F);

        /* Get the PLP code rate.
         *  slave     Bank    Addr    Bit              Name
         * ------------------------------------------------------------
         *  <SLV-T>   93h     94h     [7:4]          IL1D_PLP_COD_0[3:0]
         *  <SLV-T>   93h     94h     [3:0]          IL1D_PLP_COD_1[3:0]
         *  <SLV-T>   93h     95h     [7:4]          IL1D_PLP_COD_2[3:0]
         *  <SLV-T>   93h     95h     [3:0]          IL1D_PLP_COD_3[3:0]
         */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x94, data, 2) != SONY_CXD6801_RESULT_OK) {
            CXD6801_SLVT_UnFreezeReg (pDemod);
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }

        plpCr[0] = (sony_cxd6801_atsc3_plp_cod_t) ((data[0] >> 4) & 0x0F);
        plpCr[1] = (sony_cxd6801_atsc3_plp_cod_t) (data[0] & 0x0F);
        plpCr[2] = (sony_cxd6801_atsc3_plp_cod_t) ((data[1] >> 4) & 0x0F);
        plpCr[3] = (sony_cxd6801_atsc3_plp_cod_t) (data[1] & 0x0F);

        CXD6801_SLVT_UnFreezeReg (pDemod);

        /* Confirm FEC Type / Code Rate */
        for (i = 0; i < 4; i++) {
            if (invalid[i]) {
                continue;
            }
			if ((plpFecType[i] > SONY_CXD6801_ATSC3_PLP_FEC_LDPC_64K) || (plpCr[i] > SONY_CXD6801_ATSC3_PLP_COD_13_15)) {
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
            }
            /* Pre-BCH BER is only valid when outercode is BCH. */
			if ((plpFecType[i] != SONY_CXD6801_ATSC3_PLP_FEC_BCH_LDPC_16K) && (plpFecType[i] != SONY_CXD6801_ATSC3_PLP_FEC_BCH_LDPC_64K)) {
                invalid[i] = 1;
                continue;
            }

            n_bch[i] = nBCHBitsLookup[plpFecType[i]][plpCr[i]];
        }

    }

    if ((!invalid[0] && periodType[0] == 0) || (!invalid[1] && periodType[1] == 0)
        || (!invalid[2] && periodType[2] == 0) || (!invalid[3] && periodType[3] == 0)) {
        /* Set SLV-T Bank : 0x94 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x94) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /* Read measurement period.
         *  slave     Bank    Addr    Bit          Name
         * ------------------------------------------------------------
         *  <SLV-T>   94h     4Ah     [7:4]        OREG_BBER_MES_0[3:0]
         *  <SLV-T>   94h     4Ah     [3:0]        OREG_BBER_MES_1[3:0]
         *  <SLV-T>   94h     4Bh     [7:4]        OREG_BBER_MES_2[3:0]
         *  <SLV-T>   94h     4Bh     [3:0]        OREG_BBER_MES_3[3:0]
         */
        if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4A, rdata, 2) != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
        }
        periodExpData[0] = (rdata[0] >> 4) & 0x0F;
        periodExpData[1] = rdata[0] & 0x0F;
        periodExpData[2] = (rdata[1] >> 4) & 0x0F;
        periodExpData[3] = rdata[1] & 0x0F;
    }

    for (i = 0; i < 4; i++) {
        if (invalid[i]) {
            continue;
        }
        if (periodType[i] == 0) {
            /* Data period */
            periodExp[i] = periodExpData[i];
            periodNum[i] = hNum[i];
        } else {
            /* Time period */
            int j = 0;

            periodExp[i] = 0xFF;

            /* Check minimum period */
            for (j = 0; j < hNum[i]; j++) {
                uint8_t expTmp = rdata[j * 4 + 3 + i * 32];

                if (periodExp[i] > expTmp) {
                    periodExp[i] = expTmp;
                }
            }
            /* Calc period by 2^(minimum period) basis
             * The difference between max and min period should be 1,
             * so the max of periodNum should be 15.
             */
            for (j = 0; j < hNum[i]; j++) {
                periodNum[i] += 1 << (rdata[j * 4 + 3 + i * 32] - periodExp[i]);
            }
        }
    }

    {
        uint32_t div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        for (i = 0; i < 4; i++) {
            if (invalid[i]) {
				ber[i] = SONY_CXD6801_DEMOD_ATSC3_MONITOR_PREBCHBER_INVALID;
                continue;
            }
            if (periodExp[i] >= 6) {
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
                div = (1U << (periodExp[i] - 6)) * (n_bch[i] / 40) * periodNum[i];

                Q = (bitError[i] * 125) / div;
                R = (bitError[i] * 125) % div;

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
                div = (1U << periodExp[i]) * (n_bch[i] / 40) * periodNum[i];

                Q = (bitError[i] * 100) / div;
                R = (bitError[i] * 100) % div;

                R *= 500;
                Q = Q * 500 + R / div;
                R = R % div;

                R *= 500;
                Q = Q * 500 + R / div;
                R = R % div;
            }

            /* rounding */
            if (R >= div/2) {
                ber[i] = Q + 1;
            }
            else {
                ber[i] = Q;
            }
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_monitor_AveragedSNR(sony_cxd6801_demod_t * pDemod,
                                                    int32_t * pSNR)
{
    uint8_t rdata[32];
    int hNum = 0;
    uint32_t reg = 0;
    uint8_t ofdmNsbf = 0;
    uint32_t L = 0;
    uint32_t S = 0;
    int32_t C = 0;
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_monitor_AveragedSNR");

    if ((!pDemod) || (!pSNR)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) {
        /* Not ATSC 3.0 */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (CXD6801_SLVT_FreezeReg(pDemod) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x02 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x02) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*
     *  slave     Bank    Addr    Bit          Name
     * ------------------------------------------------------------
     *  <SLV-T>   02h     30h     [0]          IREG_SNR_VALID0
     *  <SLV-T>   02h     31h     [7:0]        IREG_SNR0[23:16]
     *  <SLV-T>   02h     32h     [7:0]        IREG_SNR0[15:8]
     *  <SLV-T>   02h     33h     [7:0]        IREG_SNR0[7:0]
     * ...
     *  <SLV-T>   02h     4Ch     [0]          IREG_SNR_VALID7
     *  <SLV-T>   02h     4Dh     [7:0]        IREG_SNR7[23:16]
     *  <SLV-T>   02h     4Eh     [7:0]        IREG_SNR7[15:8]
     *  <SLV-T>   02h     4Fh     [7:0]        IREG_SNR7[7:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x30, rdata, 32) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x90 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x90) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*
     *  slave     Bank    Addr    Bit          Name
     * ---------------------------------------------------------------------------------
     *  <SLV-T>   90h     58h     [7:0]        IREG_OFDM_NSBF[7:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x58, &ofdmNsbf, 1) != SONY_CXD6801_RESULT_OK) {
        CXD6801_SLVT_UnFreezeReg (pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    CXD6801_SLVT_UnFreezeReg (pDemod);

    for (hNum = 0; hNum < 8; hNum++) {
        /* Check IREG_SNR_VALIDX */
        if ((rdata[hNum * 4] & 0x01) == 0x00) {
            break;
        }

        reg += (rdata[hNum * 4 + 1] << 16) + (rdata[hNum * 4 + 2] << 8) + rdata[hNum * 4 + 3];
    }

    if (hNum == 0) {
        /* There are no valid history */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    reg = (reg + hNum / 2) / hNum; /* average (round) */

    /* Same as sony_cxd6801_demod_atsc3_monitor_SNR
     * if IREG_SNMON_OD > L
     *    SNR[dB] = 10 * log10{L / (S - L)} + C
     *            = 10 * (log10(L) - log10(S - L)) + C
     * else
     *    SNR[dB] = 10 * log10{IREG_SNMON_OD / (S - IREG_SNMON_OD)} + C
     *            = 10 * (log10(IREG_SNMON_OD) - log10(S - IREG_SNMON_OD)) + C
     * sony_cxd6801_log10 returns log10(x) * 100
     * Therefore SNR(dB) * 1000 :
     *     = 10 * 10 * (sony_cxd6801_log10(IREG_SNMON_OD) - sony_cxd6801_log10(S - IREG_SNMON_OD) + C * 1000
     */

    if (ofdmNsbf == 0x00) {
        L = 419404;
        S = 519913;
        C = 33800;
    } else {
        L = 704089;
        S = 971204;
        C = 35800;
    }

    if (reg > L) {
        reg = L;
    }

    *pSNR = 10 * 10 * ((int32_t) sony_cxd6801_math_log10 (reg) - (int32_t) sony_cxd6801_math_log10 (S - reg));
    *pSNR += C;

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_cxd6801_result_t cxd6801_isDemodLocked(sony_cxd6801_demod_t * pDemod)
{
	sony_cxd6801_result_t result;
    uint8_t syncState = 0;
    uint8_t alpLockStat[4] = {0};
    uint8_t alpLockAll = 0;
    uint8_t unlockDetected = 0;

    SONY_CXD6801_TRACE_ENTER ("cxd6801_isDemodLocked");

    if (!pDemod) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	result = sony_cxd6801_demod_atsc3_monitor_SyncStat(pDemod, &syncState, alpLockStat, &alpLockAll, &unlockDetected);
	if (result != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (syncState != 6) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    SONY_CXD6801_TRACE_RETURN (result);
}
