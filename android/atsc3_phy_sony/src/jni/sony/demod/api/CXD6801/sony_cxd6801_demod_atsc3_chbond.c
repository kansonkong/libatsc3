/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/12/13
  Modification ID : e402a4dcb0e51a00c34b174334c082c1211eebf1
------------------------------------------------------------------------------*/
#include "sony_cxd6801_demod_atsc3_chbond.h"
#include "sony_cxd6801_demod_atsc3_monitor.h"

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_chbond_Enable (sony_cxd6801_demod_t * pDemod,
                                              uint8_t enable)
{
    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_chbond_Enable");

    if (!pDemod) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pDemod->chbondConfig != SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN)
        && (pDemod->chbondConfig != SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB)) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN) {
        /* Set SLV-X Bank : 0x00 */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }

        /*
         *  slave    Bank    Addr    Bit      Default     Value      Name
         * --------------------------------------------------------------------------------
         *  <SLV-X>  00h     C1h     [2:0]    8'h00       8'h0x      OREG_STREAMIN_EN
         *
         * Bit[0]: Stream input0 Bit[1]: Stream input1 Bit[2]: Stream input2
         * 1: Stream input enble 0:Stream input disable
         */

        /* Set Stream Input */
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0xC1, enable ? pDemod->chbondStreamIn : 0x00) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*
     *  slave     Bank    Addr    Bit      Default    Value      Name
     * ---------------------------------------------------------------------------------------
     *  <SLV-T>   03h     B2h     [0]      8'h00      8'hxx      OREG_BCHBOND0_ON
     *  <SLV-T>   03h     B3h     [0]      8'h00      8'hxx      OREG_BCHBOND1_ON
     *  <SLV-T>   03h     B4h     [0]      8'h00      8'hxx      OREG_BCHBOND2_ON
     *  <SLV-T>   03h     B5h     [0]      8'h00      8'hxx      OREG_BCHBOND3_ON
     *  <SLV-T>   03h     B6h     [0]      8'h01      8'hxx      OREG_ATSC30_CHBOND_MAIN_FLAG
     */

    if (enable) {
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB6, pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB ? 0x00 : 0x01) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
    } else {
        uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x01};
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB2, data, 5) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

    /* Set SLV-T Bank : 0x93 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*
     *  slave     Bank    Addr    Bit      Default    Value      Name
     * ---------------------------------------------------------------------------------------
     *  <SLV-T>   93h     F4h     [7:0]    8'h23      8'h03      OREG_DCG_CGON[7:0]
     */
    if (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN) {
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xF4, enable ? 0x03 : 0x23) != SONY_CXD6801_RESULT_OK) {
            SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
        }
    }

    /*
     *  slave     Bank    Addr    Bit      Default    Value      Name
     * ---------------------------------------------------------------------------------------
     *  <SLV-T>   93h     85h     [0]      8'h01      8'h00      OREG_PLP_ID_AUTO
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x85, enable ? 0x00 : 0x01) != SONY_CXD6801_RESULT_OK) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_I2C);
    }

    SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_chbond_SetPLPConfig (sony_cxd6801_demod_t * pDemod,
                                                    uint8_t plpIDNum,
                                                    uint8_t plpID[4],
                                                    sony_cxd6801_demod_atsc3_chbond_plp_bond_t plpBond[4])
{
    int i = 0;
    uint8_t data[4];

    SONY_CXD6801_TRACE_ENTER ("sony_cxd6801_demod_atsc3_chbond_SetPLPConfig");

    if (!pDemod) {
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONY_CXD6801_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        SONY_CXD6801_TRACE_RETURN (SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if ((pDemod->chbondConfig != SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN) && (pDemod->chbondConfig != SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB)) {
        /* For ATSC 3.0 channel bonding only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* plpIDNum range check */
    if ((plpIDNum == 0) || (plpIDNum > 4)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* PLP ID range check */
    for (i = 0; i < plpIDNum; i++) {
        if (plpID[i] > 0x3F) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
        }
    }

	if (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN) {
        /* Main IC */
        /* OREGD_PLP_ID_0_VALID should be 1 */
		if ((plpBond[0] != SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_ENABLE) &&
			(plpBond[0] != SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_FROM_MAIN)) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
        }

        for (i = 1; i < plpIDNum; i++) {
            switch (plpBond[i]) {
			case SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_ENABLE:
            case SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_FROM_MAIN:
                /* (OREGD_PLP_ID_X_VALID = 0) PLP should NOT be located prior to (OREGD_PLP_ID_X_VALID = 1) PLP */
				if (plpBond[i - 1] == SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_FROM_SUB) {
					SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
                }
                break;
			case SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_FROM_SUB:
                break;
            default:
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
            }
        }
    } else {
        /* Sub IC */
        for (i = 0; i < plpIDNum; i++) {
            switch (plpBond[i]) {
			case SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_ENABLE:
			case SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_FROM_SUB:
                break;
			case SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_FROM_MAIN:
            default:
				SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
            }
        }
    }

    /* Set SLV-T Bank : 0x93 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*  slave    Bank    Addr    Bit    default   Value          Name
     * ----------------------------------------------------------------------------------
     *  <SLV-T>   93h     80h     [7:0]    8'00       8'hxx      OREGD_PLP_ID_0_VALID,OREGD_PLP_ID_0_CHBVALID,OREGD_PLP_ID_0[5:0]
     *  <SLV-T>   93h     81h     [7:0]    8'00       8'hxx      OREGD_PLP_ID_1_VALID,OREGD_PLP_ID_1_CHBVALID,OREGD_PLP_ID_1[5:0]
     *  <SLV-T>   93h     82h     [7:0]    8'00       8'hxx      OREGD_PLP_ID_2_VALID,OREGD_PLP_ID_2_CHBVALID,OREGD_PLP_ID_2[5:0]
     *  <SLV-T>   93h     83h     [7:0]    8'00       8'hxx      OREGD_PLP_ID_3_VALID,OREGD_PLP_ID_3_CHBVALID,OREGD_PLP_ID_3[5:0]
     */
    for (i = 0; i < 4; i++) {
        if (i < plpIDNum) {
			if (pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN) {
				data[i] = plpID[i] | (plpBond[i] == SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_FROM_SUB ? 0x40 : 0xC0);
            } else {
                data[i] = plpID[i] | 0x80;
            }
        } else {
            data[i] = 0;
        }
    }

	if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x03 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*  slave     Bank    Addr    Bit      Default    Value      Name
     * ----------------------------------------------------------------------------------
     *  <SLV-T>   03h     B2h     [0]      8'h00      8'hxx      OREG_BCHBOND0_ON
     *  <SLV-T>   03h     B3h     [0]      8'h00      8'hxx      OREG_BCHBOND1_ON
     *  <SLV-T>   03h     B4h     [0]      8'h00      8'hxx      OREG_BCHBOND2_ON
     *  <SLV-T>   03h     B5h     [0]      8'h00      8'hxx      OREG_BCHBOND3_ON
     */

    for (i = 0; i < 4; i++) {
		if ((i < plpIDNum) && (plpBond[i] == SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_ENABLE)) {
            data[i] = 0x01;
        } else {
            data[i] = 0x00;
        }
    }

	if (pDemod->pI2c->WriteRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB2, data, sizeof (data)) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x93 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x93) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /* Make IPLPINFO_RDY low for changing PLP without re-tuning case
     *
     *  slave     Bank     Addr     Bit    default   Value          Name
     * ----------------------------------------------------------------------------------
     *  <SLV-T>   93h      9Ch      [0]    1'b0      1'b1           OREGD_PLPINFO_UPD
     */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9C, 0x01) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_chbond_monitor_BondStat (sony_cxd6801_demod_t * pDemod,
                                                        uint8_t bondLockStat[4],
                                                        uint8_t * pBondLockAll,
                                                        uint8_t * pBondUnlockDetected)
{
    uint8_t data;
    int i;

	SONY_CXD6801_TRACE_ENTER("sony_cxd6801_demod_atsc3_chbond_monitor_BondStat");

    if ((!pDemod) || (!bondLockStat) || (!pBondLockAll) || (!pBondUnlockDetected)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != SONY_CXD6801_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

	if ((pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) || (pDemod->chbondConfig != SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN)) {
        /* Only for ATSC 3.0 channel bonding main */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    *pBondUnlockDetected = 0;

    /* Set SLV-T Bank : 0x9D */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x9D) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*  slave     Bank    Addr    Bit          Name                    Meaning
     * ----------------------------------------------------------------------------------
     *  <SLV-T>   9Dh     10h     [0]          ICHB_UNEXP_SP_OVERFLOW  0:Keep polling, 1:UnlockCallback
     *  <SLV-T>   9Dh     13h     [4]          ICHB_UNEXP_SHORTPKT_S   0:Keep polling, 1:UnlockCallback
     *  <SLV-T>   9Dh     B0h     [4]          ICHB_LOCK_AND
     *  <SLV-T>   9Dh     B0h     [3]          ICHB_LOCK_3
     *  <SLV-T>   9Dh     B0h     [2]          ICHB_LOCK_2
     *  <SLV-T>   9Dh     B0h     [1]          ICHB_LOCK_1
     *  <SLV-T>   9Dh     B0h     [0]          ICHB_LOCK_0
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, &data, 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    if (data & 0x01) {
        *pBondUnlockDetected = 1;
    }
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x13, &data, 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    if (data & 0x10) {
        *pBondUnlockDetected = 1;
    }
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB0, &data, 1) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    for (i = 0; i < 4; i++) {
        bondLockStat[i] = (data >> i) & 0x01;
    }
    *pBondLockAll = data >> 4 & 0x01;

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_chbond_monitor_SelectedPLPValid (sony_cxd6801_demod_t * pDemod,
                                                                sony_cxd6801_demod_atsc3_chbond_plp_valid_t plpValid[4])
{
    sony_cxd6801_result_t result;
    int i, j;
    uint8_t plpIdValid[4];
    uint8_t chBondValid[4];
    uint8_t chBondOn[4];
    uint8_t selectedPlpId[4];
    uint8_t data[5];

    const int plpConfigCheckTable[8][2] = {
        /* Main */                                   /* Sub */                                /* PLP_VALID  CHB_VALID  BCHBOND_ON */
		{ SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_UNUSED, SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_UNUSED }, /*       0          0           0 */
        {/* Invalid Setting */ -1,                   /* Invalid Setting */ -1                }, /*       0          0           1 */
		{ SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_FROM_SUB, /* Invalid Setting */ -1 }, /*       0          1           0 */
        {/* Invalid Setting */ -1,                   /* Invalid Setting */ -1                }, /*       0          1           1 */
		{/* Invalid Setting */ -1, SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_OK }, /*       1          0           0 */
		{/* Invalid Setting */ -1, SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_OK }, /*       1          0           1 */
		{ SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_OK,       /* Invalid Setting */ -1 }, /*       1          1           0 */
		{ SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_OK,       /* Invalid Setting */ -1 }  /*       1          1           1 */
    };

	SONY_CXD6801_TRACE_ENTER("sony_cxd6801_demod_atsc3_chbond_monitor_SelectedPLPValid");

    if ((!pDemod) || (!plpValid)) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

	if ((pDemod->system != SONY_CXD6801_DTV_SYSTEM_ATSC3) ||
		((pDemod->chbondConfig != SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN) &&
		(pDemod->chbondConfig != SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB))) {
        /* Not ATSC 3.0 or channel boding is not enabled */
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

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

    /*  slave    Bank    Addr    Bit    default   Value          Name
     * ----------------------------------------------------------------------------------
     *  <SLV-T>   93h     80h     [7:0]    8'00       8'hxx      OREGD_PLP_ID_0_VALID,OREGD_PLP_ID_0_CHBVALID,OREGD_PLP_ID_0[5:0]
     *  <SLV-T>   93h     81h     [7:0]    8'00       8'hxx      OREGD_PLP_ID_1_VALID,OREGD_PLP_ID_1_CHBVALID,OREGD_PLP_ID_1[5:0]
     *  <SLV-T>   93h     82h     [7:0]    8'00       8'hxx      OREGD_PLP_ID_2_VALID,OREGD_PLP_ID_2_CHBVALID,OREGD_PLP_ID_2[5:0]
     *  <SLV-T>   93h     83h     [7:0]    8'00       8'hxx      OREGD_PLP_ID_3_VALID,OREGD_PLP_ID_3_CHBVALID,OREGD_PLP_ID_3[5:0]
     *  <SLV-T>   93h     84h     [0]                            IPLP_SEL_ERR
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x80, data, 5) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    for (i = 0; i < 4; i++) {
        plpIdValid[i] = (data[i] >> 7) & 0x01;
        chBondValid[i] = (data[i] >> 6) & 0x01;
        selectedPlpId[i] = data[i] & 0x3F;
    }
    /* Set SLV-T Bank : 0x03 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }
    /*  slave    Bank    Addr    Bit    default   Value          Name
     * ----------------------------------------------------------------------------------
     *  <SLV-T>   03h     B2h     [0]      8'h00      8'hxx      OREG_BCHBOND0_ON
     *  <SLV-T>   03h     B3h     [0]      8'h00      8'hxx      OREG_BCHBOND1_ON
     *  <SLV-T>   03h     B4h     [0]      8'h00      8'hxx      OREG_BCHBOND2_ON
     *  <SLV-T>   03h     B5h     [0]      8'h00      8'hxx      OREG_BCHBOND3_ON
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB2, chBondOn, 4) != SONY_CXD6801_RESULT_OK) {
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    for (i = 0; i < 4; i++) {
        uint8_t index = (plpIdValid[i] << 2) + (chBondValid[i] << 1) + (chBondOn[i] & 0x01);
		uint8_t isSub = pDemod->chbondConfig == SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN ? 0 : 1;
        if (plpConfigCheckTable[index][isSub] == -1) {
			SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_OTHER);
        } else {
            plpValid[i] = (sony_cxd6801_demod_atsc3_chbond_plp_valid_t)plpConfigCheckTable[index][isSub];
        }
    }

    if (data[4] & 0x01) {
        /* PLP IDs are invalid */
        for (i = 0; i < 4; i++) {
			if (plpValid[i] == SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_OK) {
				plpValid[i] = SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_ERR_ID;
            }
        }
    } else {
        /* PLP IDs are valid */
		sony_cxd6801_atsc3_plp_list_entry_t plpList[SONY_CXD6801_ATSC3_NUM_PLP_MAX];
        uint8_t numPLP;

        /*  Check that channel bonding configurations for each PLP are correct or not  */
        result = sony_cxd6801_demod_atsc3_monitor_PLPList (pDemod, plpList, &numPLP);
		if (result != SONY_CXD6801_RESULT_OK) {
			SONY_CXD6801_TRACE_RETURN(result);
        }
        for (i = 0; i < 4; i++) {
			if (plpValid[i] == SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_OK) {
                for (j = 0; j < numPLP; j++) {
                    if (selectedPlpId[i] == plpList[j].id) {
                        if ((chBondOn[i] & 0x01) != plpList[j].chbond) {
							plpValid[i] = SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_ERR_BONDCONF;
                        }
                        break;
                    }
                }
            }
        }
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_demod_atsc3_chbond_monitor_L1Detail (sony_cxd6801_demod_t * pDemod,
                                                        sony_cxd6801_atsc3_chbond_l1detail_t * pL1Detail)
{
    uint8_t data[5];

	SONY_CXD6801_TRACE_ENTER("sony_cxd6801_demod_atsc3_chbond_monitor_L1Detail");

    if ((!pDemod) || (!pL1Detail)) {
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
		CXD6801_SLVT_UnFreezeReg(pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    /*
     *  slave      Bank     Addr     Bit        Signal name
     * --------------------------------------------------------------
     *  <SLV-T>    93h      10h      [1]        IL1D_OK
     *  <SLV-T>    93h      9Dh      [0]        IPLPINFO_RDY
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, data, 1) != SONY_CXD6801_RESULT_OK) {
		CXD6801_SLVT_UnFreezeReg(pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (!(data[0] & 0x02)) {
		CXD6801_SLVT_UnFreezeReg(pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9D, data, 1) != SONY_CXD6801_RESULT_OK) {
		CXD6801_SLVT_UnFreezeReg(pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

    if (!(data[0] & 0x01)) {
		CXD6801_SLVT_UnFreezeReg(pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE);
    }

    /*
     *  slave     Bank    Addr    Bit          Name
     * --------------------------------------------------------
     *  <SLV-T>   93h     C0h     [2:0]        IL1D_NUM_RF[2:0]
     *  <SLV-T>   93h     C1h     [7:0]        IL1D_BONDED_BSID[15:8]
     *  <SLV-T>   93h     C2h     [7:0]        IL1D_BONDED_BSID[7:0]
     *  <SLV-T>   93h     C3h     [7:0]        IL1D_BSID[15:8]
     *  <SLV-T>   93h     C4h     [7:0]        IL1D_BSID[7:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xC0, data, 5) != SONY_CXD6801_RESULT_OK) {
		CXD6801_SLVT_UnFreezeReg(pDemod);
		SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C);
    }

	CXD6801_SLVT_UnFreezeReg(pDemod);

    pL1Detail->num_rf = data[0] & 0x07;
    if (pL1Detail->num_rf == 0) {
        pL1Detail->bonded_bsid = 0;
        pL1Detail->bsid = 0;
    } else {
        pL1Detail->bonded_bsid = (data[1] << 8) | data[2];
        pL1Detail->bsid = (data[3] << 8) | data[4];
    }

	SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}
