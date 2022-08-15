/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/06/24
  Modification ID : 3b74e280b7ad8ce430b6a9419ac53e8f2e3737f9
------------------------------------------------------------------------------*/

#include "sony_cxd_demod_isdbt.h"
#include "sony_cxd_demod_isdbt_monitor.h"
#include "sony_cxd_math.h"


/*------------------------------------------------------------------------------
 Static Function Prototypes
------------------------------------------------------------------------------*/
/*
 @brief Confirm demodulator lock
*/
static sony_cxd_result_t IsDmdLocked(sony_cxd_demod_t * pDemod);
/*
 @brief Covert TMCC raw data to TMCC info structure.
*/
static sony_cxd_result_t setTmccInfo(uint8_t* pData, sony_cxd_isdbt_tmcc_info_t* pTmccInfo);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_cxd_result_t sony_cxd_demod_isdbt_monitor_SyncStat(sony_cxd_demod_t * pDemod,
                                                 uint8_t * pDmdLockStat,
                                                 uint8_t * pTSLockStat,
                                                 uint8_t * pUnlockDetected)
{
    uint8_t rdata = 0x00;
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_SyncStat");

    if ((!pDemod) || (!pDmdLockStat) || (!pTSLockStat) || (!pUnlockDetected)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }
	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x60 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Slave    Bank    Addr    Bit     Name                Meaning
     * -----------------------------------------------------------------------
     * <SLV-T>   60h     10h     [1]          IREG_DMDLOCK        0:UNLOCK, 1:LOCK
     * <SLV-T>   60h     10h     [0]          ITSLOCK             0:UNLOCK, 1:LOCK
     * <SLV-T>   60h     10h     [4]          IEARLY_NOOFDM       0:Detection underway or ISDB-T signal may exist
     *                                                            1:No ISDB-T signal in the receiving channel
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, &rdata, 1) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    *pUnlockDetected = (uint8_t)((rdata & 0x10)? 1 : 0);
    *pDmdLockStat = (uint8_t)((rdata & 0x02) ? 1 : 0);
    *pTSLockStat = (uint8_t)((rdata & 0x01) ? 1 : 0);

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_monitor_IFAGCOut(sony_cxd_demod_t * pDemod,
                                                 uint32_t * pIFAGCOut)
{
    uint8_t rdata[2];

	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_IFAGCOut");

    if ((!pDemod) || (!pIFAGCOut)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x60 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Read pir_agc_gain
     * slave    Bank    Addr    Bit              Name
     * ------------------------------------------------------------
     * <SLV-T>   60h     26h     [3:0]        IIFAGC_OUT[11:8]
     * <SLV-T>   60h     27h     [7:0]        IIFAGC_OUT[7:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x26, rdata, 2) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    *pIFAGCOut = (((uint32_t)rdata[0] & 0x0F) << 8) | rdata[1];

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_monitor_ModeGuard(sony_cxd_demod_t * pDemod,
	sony_cxd_isdbt_mode_t * pMode,
	sony_cxd_isdbt_guard_t * pGuard)
{
    uint8_t rdata = 0x00;

	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_ModeGuard");

    if ((!pDemod) || (!pMode) || (!pGuard)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Check Demod Lock. */
    result = IsDmdLocked(pDemod);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /* Below registers are valid when IREG_DMDLOCK==1. */

    /* Set SLV-T Bank : 0x60 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60) != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg (pDemod);
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* ISDB-T MODE/GUARD estimation result can read by the registers.
     *
     *    slave    Bank    Addr    Bit           Name               Meaning
     *   ---------------------------------------------------------------------------------
     *   <SLV-T>   60h     40h     [3:2]        IREG_MODE[1:0]     0:Mode1, 1:Mode3, 2:Mode2
     *   <SLV-T>   60h     40h     [1:0]        IREG_GI[1:0]       0:1/32, 1:1/16, 2:1/8, 3:1/4
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x40, &rdata, 1) != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg (pDemod);
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    SLVT_CXD_UnFreezeReg (pDemod);

    switch ((rdata >> 2) & 0x03) {
    case 0:
		*pMode = sony_cxd_ISDBT_MODE_1;
        break;
    case 1:
		*pMode = sony_cxd_ISDBT_MODE_3;
        break;
    case 2:
		*pMode = sony_cxd_ISDBT_MODE_2;
        break;
    default:
		*pMode = sony_cxd_ISDBT_MODE_UNKNOWN;
        break;
    }

	*pGuard = (sony_cxd_isdbt_guard_t)(rdata & 0x03);

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_monitor_CarrierOffset(sony_cxd_demod_t * pDemod,
                                                      int32_t * pOffset)
{
    uint8_t rdata[4] = {0x00, 0x00, 0x00, 0x00};
    uint32_t iregCrcgCtrlval = 0;

	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_CarrierOffset");

    if ((!pDemod) || (!pOffset)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Dmd Lock check */
    result = IsDmdLocked (pDemod);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg (pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x60 */
	if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60) != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg (pDemod);
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }
    /*
     * slave    Bank    Addr    Bit              Signal name
     * ---------------------------------------------------------------
     * <SLV-T>   60h     4Ch     [4:0]      IREG_CRCG_CTLVAL[28:24]
     * <SLV-T>   60h     4Dh     [7:0]      IREG_CRCG_CTLVAL[23:16]
     * <SLV-T>   60h     4Eh     [7:0]      IREG_CRCG_CTLVAL[15:8]
     * <SLV-T>   60h     4Fh     [7:0]      IREG_CRCG_CTLVAL[7:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x4C, rdata, 4) != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg (pDemod);
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    SLVT_CXD_UnFreezeReg (pDemod);

    iregCrcgCtrlval = rdata[0] & 0x1F;
    iregCrcgCtrlval = (iregCrcgCtrlval << 8) + rdata[1];
    iregCrcgCtrlval = (iregCrcgCtrlval << 8) + rdata[2];
    iregCrcgCtrlval = (iregCrcgCtrlval << 8) + rdata[3];

	*pOffset = sony_cxd_Convert2SComplement(iregCrcgCtrlval, 29);

    /*
     * (1)For the tuner which spectrum sense is not same as RF, use these formulas.
     *  (Ordinary tuner, which is single converted and use upper local. also including ASCOT)
     *
     * Carrier Frequency Offset [Hz] = -( ( IREG_CRCG_CTLVAL / 2^28 ) * 8192 ) / ( 1008 * 10^-6 )   ---6MHz BW
     *                               = -( IREG_CRCG_CTLVAL * 8 / (2^18 * 1008 / 10^6) )
     *                               = -( IREG_CRCG_CTLVAL * 8 / 264.241 )
     *
     * Carrier Frequency Offset [Hz] = -( ( IREG_CRCG_CTLVAL / 2^28 ) * 8192 ) / (  882 * 10^-6 )   ---7MHz BW
     *                               = -( IREG_CRCG_CTLVAL * 8 / (2^18 * 882 / 10^6) )
     *                               = -( IREG_CRCG_CTLVAL * 8 / 231.211 )
     *
     * Carrier Frequency Offset [Hz] = -( ( IREG_CRCG_CTLVAL / 2^28 ) * 8192 ) / (  756 * 10^-6 )   ---8MHz BW
     *                               = -( IREG_CRCG_CTLVAL * 8 / (2^18 * 756 / 10^6) )
     *                               = -( IREG_CRCG_CTLVAL * 8 / 198.181 )
     */
    switch (pDemod->bandwidth) {
	case sony_cxd_DTV_BW_6_MHZ:
        *pOffset = -1 * ((*pOffset) * 8 / 264);
        break;
	case sony_cxd_DTV_BW_7_MHZ:
        *pOffset = -1 * ((*pOffset) * 8 / 231);
        break;
	case sony_cxd_DTV_BW_8_MHZ:
        *pOffset = -1 * ((*pOffset) * 8 / 198);
        break;
    default:
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Compensate for inverted spectrum. */
    {
        /* Inverted RF spectrum with non-inverting tuner architecture. */
		if (pDemod->confSense == sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_INV) {
            *pOffset *= -1;
        }
    }

	sony_cxd_TRACE_RETURN(result);
}


sony_cxd_result_t sony_cxd_demod_isdbt_monitor_PreRSBER(sony_cxd_demod_t * pDemod,
                                                 uint32_t * pBERA, uint32_t * pBERB, uint32_t * pBERC)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint8_t dataPacketNum[2];
    uint8_t dataBitError[9];
    uint32_t bitError[3];
    uint32_t packetNum;
    uint32_t ber[3];
    int i = 0;

	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_PreRSBER");

    if ((!pDemod) || (!pBERA) || (!pBERB) || (!pBERC)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->isdbtEwsState != sony_cxd_DEMOD_ISDBT_STATE_NORMAL) {
        /* This api is not accepted in EWS state  */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Dmd Lock check */
    result = IsDmdLocked (pDemod);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg (pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x60 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /*
     *  slave     Bank   Addr    Bit          Name
     * ------------------------------------------------------------
     * <SLV-T>    60h    16h     [5:0]    IBER_BENUM_RSA[21:16]
     * <SLV-T>    60h    17h     [7:0]    IBER_BENUM_RSA[15:8]
     * <SLV-T>    60h    18h     [7:0]    IBER_BENUM_RSA[7:0]
     * <SLV-T>    60h    19h     [5:0]    IBER_BENUM_RSB[21:16]
     * <SLV-T>    60h    1Ah     [7:0]    IBER_BENUM_RSB[15:8]
     * <SLV-T>    60h    1Bh     [7:0]    IBER_BENUM_RSB[7:0]
     * <SLV-T>    60h    1Ch     [5:0]    IBER_BENUM_RSC[21:16]
     * <SLV-T>    60h    1Dh     [7:0]    IBER_BENUM_RSC[15:8]
     * <SLV-T>    60h    1Eh     [7:0]    IBER_BENUM_RSC[7:0]
     */

	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x16, dataBitError, 9) != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    SLVT_CXD_UnFreezeReg(pDemod);

    /*
     *  slave     Bank   Addr    Bit          Name
     * ------------------------------------------------------------
     * <SLV-T>    60h    5Bh     [6:0]    OBER_CDUR_RSA[14:8]
     * <SLV-T>    60h    5Ch     [7:0]    OBER_CDUR_RSA[7:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5B, dataPacketNum, 2) != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    packetNum = ((uint32_t)dataPacketNum[0] << 8) | dataPacketNum[1];
    if (packetNum == 0) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
    }

    for (i = 0; i < 3; i++) {
        bitError[i] = ((uint32_t)(dataBitError[0 + (3 * i)] & 0x7F) << 16) | ((uint32_t)dataBitError[1 + (3 * i)] << 8) | dataBitError[2 + (3 * i)];

        if (bitError[i] / 8 / 204 > packetNum) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
        }
    }

    /*
     *   BER = (bitError * 10000000) / (packetNum * 8 * 204)
     *       = (bitError * 312500) / (packetNum * 51)
     *       = (bitError * 250 * 1250) / (packetNum * 51)
     *
     */

    for (i = 0; i < 3; i++) {
        uint32_t Div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        Div = packetNum * 51;

        Q = (bitError[i] * 250) / Div;
        R = (bitError[i] * 250) % Div;

        R *= 1250;
        Q = Q * 1250 + R / Div;
        R = R % Div;

        if (R >= (Div/2)) {
            ber[i] = Q + 1;
        } else {
            ber[i] = Q;
        }
    }

    *pBERA = ber[0];
    *pBERB = ber[1];
    *pBERC = ber[2];

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_monitor_PacketErrorNumber(sony_cxd_demod_t * pDemod,
                                                          uint32_t * pPENA, uint32_t * pPENB, uint32_t * pPENC)
{
    uint8_t rdata[7];

	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_PacketErrorNumber");

    if ((!pDemod) || (!pPENA) || (!pPENB) || (!pPENC)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->isdbtEwsState != sony_cxd_DEMOD_ISDBT_STATE_NORMAL) {
        /* This api is not accepted in EWS state  */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x60 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    /*
     * slave    Bank    Addr    Bit           Name
     *------------------------------------------------------------
     * <SLV-T>   60h     A1h     [0]      IREG_CWRJCTCNT_VALID
     * <SLV-T>   60h     A2h     [7:0]    ICWRJCTCNT_A[15:8]
     * <SLV-T>   60h     A3h     [7:0]    ICWRJCTCNT_A[7:0]
     * <SLV-T>   60h     A4h     [7:0]    ICWRJCTCNT_B[15:8]
     * <SLV-T>   60h     A5h     [7:0]    ICWRJCTCNT_B[7:0]
     * <SLV-T>   60h     A6h     [7:0]    ICWRJCTCNT_C[15:8]
     * <SLV-T>   60h     A7h     [7:0]    ICWRJCTCNT_C[7:0]
     */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0xA1, rdata, 7);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    if ((rdata[0] & 0x01) == 0) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
    }

    *pPENA = (uint32_t)((rdata[1] << 8) | rdata[2]);
    *pPENB = (uint32_t)((rdata[3] << 8) | rdata[4]);
    *pPENC = (uint32_t)((rdata[5] << 8) | rdata[6]);

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_monitor_SpectrumSense(sony_cxd_demod_t * pDemod,
	sony_cxd_demod_terr_cable_spectrum_sense_t * pSense)
{
    uint8_t data = 0;

	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_SpectrumSense");

    if ((!pDemod) || (!pSense)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Demod Lock check */
    result = IsDmdLocked (pDemod);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg (pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x60 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /*
     * slave    Bank    Addr    Bit          Signal name                Meaning
     * ---------------------------------------------------------------------------------
     * <SLV-T>   60h     3Fh     [0]          IREG_COSNE_SINV    0:Not inverted,  1:Inverted
     */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3F, &data, 1);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    SLVT_CXD_UnFreezeReg(pDemod);

	if (pDemod->confSense == sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_INV) {
		*pSense = (data & 0x01) ? sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_NORMAL : sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_INV;
    } else {
		*pSense = (data & 0x01) ? sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_INV : sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_NORMAL;
    }

	sony_cxd_TRACE_RETURN(result);

}

sony_cxd_result_t sony_cxd_demod_isdbt_monitor_SNR(sony_cxd_demod_t * pDemod,
                                            int32_t * pSNR)
{
    uint8_t rdata[2] = {0x00, 0x00};
    uint16_t iregSnmonOd = 0x0000;

	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_SNR");

    if ((!pDemod) || (!pSNR)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    /* Demod Lock check */
    result = IsDmdLocked (pDemod);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg (pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x60 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /*
     * slave    Bank    Addr    Bit              Name
     * ------------------------------------------------------------
     * <SLV-T>   60h     28h     [7:0]        IREG_SNMON_OD[15:8]
     * <SLV-T>   60h     29h     [7:0]        IREG_SNMON_OD[7:0]
     */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x28, rdata, 2);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    SLVT_CXD_UnFreezeReg(pDemod);

    iregSnmonOd = rdata[0];
    iregSnmonOd = (iregSnmonOd << 8) + rdata[1];

    if (iregSnmonOd == 0) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
    }

    /*
     *
     *  SNR[dB] = 10 * log10 (IREG_SNMON_OD / 8 )
     *          = 10 * (log10(IREG_SNMON_OD) - log10(8))
     *           sony_cxd_log10 returns log10(x) * 100
     *           Therefore SNR(dB) * 1000 :
     *                   = 10 * 10 * (sony_cxd_log10(IREG_SNMON_OD) - sony_cxd_log10(8))
     *                   = 10 * 10 * sony_cxd_log10(IREG_SNMON_OD) - 9031
     */

	*pSNR = 100 * (int32_t)sony_cxd_math_log10(iregSnmonOd) - 9031;

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_monitor_SamplingOffset(sony_cxd_demod_t * pDemod,
                                                      int32_t * pPPM)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_SamplingOffset");

    if ((!pDemod) || (!pPPM)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T*/
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    {
        uint8_t ctlValReg[5];
        uint8_t nominalRateReg[5];
        uint32_t trlCtlVal = 0;
        uint32_t trcgNominalRate = 0;
        int32_t num = 0;
        int32_t den = 0;
        int8_t diffUpper = 0;

        /* Freeze registers */
		if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

        /* Demod Lock check */
        result = IsDmdLocked (pDemod);
		if (result != sony_cxd_RESULT_OK) {
            SLVT_CXD_UnFreezeReg (pDemod);
			sony_cxd_TRACE_RETURN(result);
        }

        /* Set SLV-T Bank : 0x10 */
		if (pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x10) != sony_cxd_RESULT_OK) {
            SLVT_CXD_UnFreezeReg (pDemod);
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }
          /*   slave    Bank    Addr    Bit              name
           *  ---------------------------------------------------------------
           *   <SLV-T>   10h     52h     [6:0]      IREG_TRL_CTLVAL_S[38:32]
           *   <SLV-T>   10h     53h     [7:0]      IREG_TRL_CTLVAL_S[31:24]
           *   <SLV-T>   10h     54h     [7:0]      IREG_TRL_CTLVAL_S[23:16]
           *   <SLV-T>   10h     55h     [7:0]      IREG_TRL_CTLVAL_S[15:8]
           *   <SLV-T>   10h     56h     [7:0]      IREG_TRL_CTLVAL_S[7:0]
           *   <SLV-T>   10h     9Fh     [5:0]      OREG_TRCG_NOMINALRATE[37:32]
           *   <SLV-T>   10h     A0h     [7:0]      OREG_TRCG_NOMINALRATE[31:24]
           *   <SLV-T>   10h     A1h     [7:0]      OREG_TRCG_NOMINALRATE[23:16]
           *   <SLV-T>   10h     A2h     [7:0]      OREG_TRCG_NOMINALRATE[15:8]
           *   <SLV-T>   10h     A3h     [7:0]      OREG_TRCG_NOMINALRATE[7:0]
           */
		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x52, ctlValReg, sizeof (ctlValReg)) != sony_cxd_RESULT_OK) {
            SLVT_CXD_UnFreezeReg (pDemod);
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

		if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x9F, nominalRateReg, sizeof (nominalRateReg)) != sony_cxd_RESULT_OK) {
            SLVT_CXD_UnFreezeReg (pDemod);
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
        }

        /* Unfreeze registers. */
        SLVT_CXD_UnFreezeReg (pDemod);

        diffUpper = (int8_t)((ctlValReg[0] & 0x7F) - (nominalRateReg[0] & 0x7F));

        /* Confirm offset range */
        if ((diffUpper < -1) || (diffUpper > 1)) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Clock frequency offset(ppm)  = { ( IREG_TRL_CTLVAL_S - OREG_TRCG_NOMINALRATE ) / OREG_TRCG_NOMINALRATE } * 100000000
                                        = ( IREG_TRL_CTLVAL_S - OREG_TRCG_NOMINALRATE ) * 10000000 / OREG_TRCG_NOMINALRATE [38:8] * 256
                                        = ( IREG_TRL_CTLVAL_S - OREG_TRCG_NOMINALRATE ) * 390625 / OREG_TRCG_NOMINALRATE [38:8]
         * Numerator = IREG_TRL_CTLVAL_S - OREG_TRCG_NOMINALRATE
         * Denominator = OREG_TRCG_NOMINALRATE / 1*10^8
         */

        /* Top 7 bits can be ignored for subtraction as out of range. */
        trlCtlVal =  (uint32_t)ctlValReg[1] << 24;
        trlCtlVal |= (uint32_t)ctlValReg[2] << 16;
        trlCtlVal |= (uint32_t)ctlValReg[3] << 8;
        trlCtlVal |= (uint32_t)ctlValReg[4];

        trcgNominalRate  = (uint32_t)nominalRateReg[1] << 24;
        trcgNominalRate |= (uint32_t)nominalRateReg[2] << 16;
        trcgNominalRate |= (uint32_t)nominalRateReg[3] << 8;
        trcgNominalRate |= (uint32_t)nominalRateReg[4];

        /* Shift down 1 bit to avoid overflow in subtraction */
        trlCtlVal >>= 1;
        trcgNominalRate >>= 1;

        if (diffUpper == 1) {
            if (trlCtlVal > trcgNominalRate) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
            }
            else {
                num = (int32_t)((trlCtlVal + 0x80000000UL) - trcgNominalRate);
            }
        } else if (diffUpper == -1) {
            if (trcgNominalRate > trlCtlVal) {
				sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
            }
            else {
                num = -(int32_t)((trcgNominalRate + 0x80000000UL) - trlCtlVal);
            }
        } else {
            num = (int32_t)(trlCtlVal - trcgNominalRate);
        }

        /* OREG_TRCG_NOMINALRATE is 39bit therefore:
         * Denominator = (OREG_TRCG_NOMINALRATE [38:8] * 256) / 1*10^8
         *             = (OREG_TRCG_NOMINALRATE [38:8] * 2^8) / (2^8 * 5^8)
         *             = OREG_TRCG_NOMINALRATE [38:8] / 390625
         */
        den =  (int32_t)((((uint32_t)nominalRateReg[0] & 0x7F) << 24) |
                          ((uint32_t)nominalRateReg[1] << 16)        |
                          ((uint32_t)nominalRateReg[2] << 8)         |
                          ((uint32_t)nominalRateReg[3]));
        den = (den + (390625/2)) / 390625;

        /* Shift down to align with numerator */
        den /= 2;

        if (den == 0) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
        }

        /* Perform calculation */
        if (num >= 0) {
            *pPPM = (num + (den/2)) / den;
        }
        else {
            *pPPM = (num - (den/2)) / den;
        }
    }

	sony_cxd_TRACE_RETURN(result);
}


sony_cxd_result_t sony_cxd_demod_isdbt_monitor_PER(sony_cxd_demod_t * pDemod,
                                            uint32_t * pPERA, uint32_t * pPERB, uint32_t * pPERC)
{
    uint8_t dataPacketError[6];
    uint8_t dataPacketNum[2];
    uint32_t packetError[3];
    uint32_t packetNum = 0;
    uint32_t per[3];
    int i = 0;

	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_PER");

    if ((!pDemod) || (!pPERA) || (!pPERB) || (!pPERC)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Software state check */
	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->isdbtEwsState != sony_cxd_DEMOD_ISDBT_STATE_NORMAL) {
        /* This api is not accepted in EWS state  */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    result = IsDmdLocked(pDemod);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x60 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /*
     *  slave     Bank   Addr    Bit          Name
     * ------------------------------------------------------------
     * <SLV-T>    60h    1Fh     [6:0]    IBER_PENUM_RSA[14:8]
     * <SLV-T>    60h    20h     [7:0]    IBER_PENUM_RSA[7:0]
     * <SLV-T>    60h    21h     [6:0]    IBER_PENUM_RSB[14:8]
     * <SLV-T>    60h    22h     [7:0]    IBER_PENUM_RSB[7:0]
     * <SLV-T>    60h    23h     [6:0]    IBER_PENUM_RSC[14:8]
     * <SLV-T>    60h    24h     [7:0]    IBER_PENUM_RSC[7:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x1F, dataPacketError, 6) != sony_cxd_RESULT_OK){
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    SLVT_CXD_UnFreezeReg(pDemod);

    /*
     *  slave     Bank   Addr    Bit          Name
     * ------------------------------------------------------------
     * <SLV-T>    60h    5Bh     [6:0]    OBER_CDUR_RSA[14:8]
     * <SLV-T>    60h    5Ch     [7:0]    OBER_CDUR_RSA[7:0]
     */
	if (pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x5B, dataPacketNum, 2) != sony_cxd_RESULT_OK){
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    packetNum = ((uint32_t)dataPacketNum[0] << 8) + dataPacketNum[1];
    if (packetNum == 0) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
    }

    for (i = 0; i < 3; i++) {
        packetError[i] = ((uint32_t)dataPacketError[0 + (2 * i)] << 8) + dataPacketError[1 + (2 * i)];

        if (packetError[i] > packetNum) {
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
        }
    }

    /*
      PER = packetError * 1000000 / packetNum
          = packetError * 1000 * 1000 / packetNum
     */
    for (i = 0; i < 3; i++) {
        uint32_t Div = 0;
        uint32_t Q = 0;
        uint32_t R = 0;

        Div = packetNum;
        Q = (packetError[i] * 1000) / Div;
        R = (packetError[i] * 1000) % Div;

        R *= 1000;
        Q = Q * 1000 + R / Div;
        R = R % Div;

        if ((Div != 1) && (R >= (Div / 2))) {
            per[i] = Q + 1;
        } else {
            per[i] = Q;
        }
    }

    *pPERA = per[0];
    *pPERB = per[1];
    *pPERC = per[2];

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_monitor_TSRate(sony_cxd_demod_t * pDemod,
                                               uint32_t * pTSRateKbpsA, uint32_t * pTSRateKbpsB, uint32_t * pTSRateKbpsC)
{
    /* Data rate [kbps x 100] for 1 segment. (from STD-B31) */
    /* Modulation x Code Rate x Guard Interval */
    const uint32_t dataRate1Seg6MHzBW[3][5][4] = {
            /* 1/32    1/16    1/8     1/4 */
        {
            /* DQPSK, QPSK */
            { 34043,  33042,  31206,  28085}, /* 1/2 */
            { 45391,  44056,  41608,  37447}, /* 2/3 */
            { 51065,  49563,  46809,  42128}, /* 3/4 */
            { 56739,  55070,  52010,  46809}, /* 5/6 */
            { 59576,  57823,  54611,  49150}  /* 7/8 */
        },
        {
            /* 16QAM */
            { 68087,  66084,  62413,  56171}, /* 1/2 */
            { 90782,  88112,  83217,  74895}, /* 2/3 */
            {102130,  99126,  93619,  84257}, /* 3/4 */
            {113478, 110140, 104021,  93619}, /* 5/6 */
            {119152, 115647, 109222,  98300}  /* 7/8 */
        },
        {
            /* 64QAM */
            {102130,  99126,  93619,  84257}, /* 1/2 */
            {136174, 132168, 124826, 112343}, /* 2/3 */
            {153195, 148690, 140429, 126386}, /* 3/4 */
            {170217, 165211, 156032, 140429}, /* 5/6 */
            {178728, 173471, 163834, 147450}  /* 7/8 */
        }
    };

    const uint32_t dataRate1Seg7MHzBW[3][5][4] = {
            /* 1/32    1/16    1/8     1/4 */
        {
            /* DQPSK, QPSK */
            { 39717,  38549,  36407,  32766}, /* 1/2 */
            { 52956,  51399,  48543,  43689}, /* 2/3 */
            { 59576,  57823,  54611,  49150}, /* 3/4 */
            { 66195,  64248,  60679,  54611}, /* 5/6 */
            { 69505,  67461,  63713,  57342}  /* 7/8 */
        },
        {
            /* 16QAM */
            { 79434,  77098,  72815,  65533}, /* 1/2 */
            {105913, 102798,  97087,  87378}, /* 2/3 */
            {119152, 115647, 109222,  98300}, /* 3/4 */
            {132391, 128497, 121358, 109222}, /* 5/6 */
            {139011, 134922, 127426, 114684}  /* 7/8 */
        },
        {
            /* 64QAM */
            {119152, 115647, 109222,  98300}, /* 1/2 */
            {158869, 154197, 145630, 131067}, /* 2/3 */
            {178728, 173471, 163834, 147450}, /* 3/4 */
            {198587, 192746, 182038, 163834}, /* 5/6 */
            {208516, 202383, 191140, 172026}  /* 7/8 */
        }
    };

    const uint32_t dataRate1Seg8MHzBW[3][5][4] = {
            /* 1/32    1/16    1/8     1/4 */
        {
            /* DQPSK, QPSK */
            { 45391,  44056,  41608,  37447}, /* 1/2 */
            { 60521,  58741,  55478,  49930}, /* 2/3 */
            { 68087,  66084,  62413,  56171}, /* 3/4 */
            { 75652,  73427,  69347,  62413}, /* 5/6 */
            { 79434,  77098,  72815,  65533}  /* 7/8 */
        },
        {
            /* 16QAM */
            { 90782,  88112,  83217,  74895}, /* 1/2 */
            {121043, 117483, 110956,  99860}, /* 2/3 */
            {136174, 132168, 124826, 112343}, /* 3/4 */
            {151304, 146854, 138695, 124826}, /* 5/6 */
            {158869, 154197, 145630, 131067}  /* 7/8 */
        },
        {
            /* 64QAM */
            {136174, 132168, 124826, 112343}, /* 1/2 */
            {181565, 176225, 166434, 149791}, /* 2/3 */
            {204261, 198253, 187239, 168515}, /* 3/4 */
            {226956, 220281, 208043, 187239}, /* 5/6 */
            {238304, 231295, 218445, 196601}  /* 7/8 */
        }
    };

	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_isdbt_mode_t mode;
	sony_cxd_isdbt_guard_t guard;
	sony_cxd_isdbt_tmcc_info_t tmccInfo;
    uint8_t modIndex[3];
    uint8_t codeRateIndex[3];
    uint8_t segmentNum[3];
    uint32_t tsRate[3];
    int i = 0;

	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_TSRate");

    if ((!pDemod) || (!pTSRateKbpsA) || (!pTSRateKbpsB) || (!pTSRateKbpsC)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	result = sony_cxd_demod_isdbt_monitor_ModeGuard(pDemod, &mode, &guard);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

	if (guard > sony_cxd_ISDBT_GUARD_1_4) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
    }

	result = sony_cxd_demod_isdbt_monitor_TMCCInfo(pDemod, &tmccInfo);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    modIndex[0] = (uint8_t)tmccInfo.currentInfo.layerA.modulation;
    codeRateIndex[0] = (uint8_t)tmccInfo.currentInfo.layerA.codingRate;
    segmentNum[0] = (uint8_t)tmccInfo.currentInfo.layerA.segmentsNum;

    modIndex[1] = (uint8_t)tmccInfo.currentInfo.layerB.modulation;
    codeRateIndex[1] = (uint8_t)tmccInfo.currentInfo.layerB.codingRate;
    segmentNum[1] = (uint8_t)tmccInfo.currentInfo.layerB.segmentsNum;

    modIndex[2] = (uint8_t)tmccInfo.currentInfo.layerC.modulation;
    codeRateIndex[2] = (uint8_t)tmccInfo.currentInfo.layerC.codingRate;
    segmentNum[2] = (uint8_t)tmccInfo.currentInfo.layerC.segmentsNum;

    for (i = 0; i < 3; i++) {
		if ((modIndex[i] >(uint8_t)sony_cxd_ISDBT_MODULATION_64QAM) || (codeRateIndex[i] > sony_cxd_ISDBT_CODING_RATE_7_8)
            || (segmentNum[i] > 13)){
            /* The layer is unused or some error */
            tsRate[i] = 0;
            continue;
        }

        if(modIndex[i] != 0){
            /* DQPSK and QPSK share same table */
            modIndex[i]--;
        }

        switch(pDemod->bandwidth){
		case sony_cxd_DTV_BW_6_MHZ:
            tsRate[i] = dataRate1Seg6MHzBW[modIndex[i]][codeRateIndex[i]][guard] * segmentNum[i];
            break;
		case sony_cxd_DTV_BW_7_MHZ:
            tsRate[i] = dataRate1Seg7MHzBW[modIndex[i]][codeRateIndex[i]][guard] * segmentNum[i];
            break;
		case sony_cxd_DTV_BW_8_MHZ:
            tsRate[i] = dataRate1Seg8MHzBW[modIndex[i]][codeRateIndex[i]][guard] * segmentNum[i];
            break;
        default:
			sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
        }

        tsRate[i] = (tsRate[i] + 50) / 100;
    }

    *pTSRateKbpsA = tsRate[0];
    *pTSRateKbpsB = tsRate[1];
    *pTSRateKbpsC = tsRate[2];

	sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_demod_isdbt_monitor_PresetInfo(sony_cxd_demod_t*                   pDemod,
	sony_cxd_demod_isdbt_preset_info_t* pPresetInfo)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_PresetInfo");

    if ((!pDemod) || (!pPresetInfo)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    result = IsDmdLocked(pDemod);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x60 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /*
     *  slave    Bank    Addr    Bit              Name
     *  --------------------------------------------------------
     *  <SLV-T>   60h     32h    [7:0]          PIR_TMCC_SET_2
     *  <SLV-T>   60h     33h    [7:0]          PIR_TMCC_SET_3
     *  <SLV-T>   60h     34h    [7:0]          PIR_TMCC_SET_4
     *  <SLV-T>   60h     35h    [7:0]          PIR_TMCC_SET_5
     *  <SLV-T>   60h     36h    [7:0]          PIR_TMCC_SET_6
     *  <SLV-T>   60h     37h    [7:0]          PIR_TMCC_SET_7
     *  <SLV-T>   60h     38h    [7:0]          PIR_TMCC_SET_8
     *  <SLV-T>   60h     39h    [7:0]          PIR_TMCC_SET_9
     *  <SLV-T>   60h     3Ah    [7:0]          PIR_TMCC_SET_10
     *  <SLV-T>   60h     3Bh    [7:0]          PIR_TMCC_SET_11
     *  <SLV-T>   60h     3Ch    [7:0]          PIR_TMCC_SET_12
     *  <SLV-T>   60h     3Dh    [7:0]          PIR_TMCC_SET_13
     *  <SLV-T>   60h     3Eh    [7:0]          PIR_TMCC_SET_14
     */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x32, pPresetInfo->data, 13);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    SLVT_CXD_UnFreezeReg(pDemod);

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_monitor_TMCCInfo(sony_cxd_demod_t*           pDemod,
	sony_cxd_isdbt_tmcc_info_t * pTMCCInfo)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint8_t data[13] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_TMCCInfo");

    if ((!pDemod) || (!pTMCCInfo)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    result = IsDmdLocked(pDemod);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /* Set SLV-T Bank : 0x60 */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x60);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    /*
     *  slave    Bank    Addr    Bit              Name
     *  --------------------------------------------------------
     *  <SLV-T>   60h     32h    [7:0]          PIR_TMCC_SET_2
     *  <SLV-T>   60h     33h    [7:0]          PIR_TMCC_SET_3
     *  <SLV-T>   60h     34h    [7:0]          PIR_TMCC_SET_4
     *  <SLV-T>   60h     35h    [7:0]          PIR_TMCC_SET_5
     *  <SLV-T>   60h     36h    [7:0]          PIR_TMCC_SET_6
     *  <SLV-T>   60h     37h    [7:0]          PIR_TMCC_SET_7
     *  <SLV-T>   60h     38h    [7:0]          PIR_TMCC_SET_8
     *  <SLV-T>   60h     39h    [7:0]          PIR_TMCC_SET_9
     *  <SLV-T>   60h     3Ah    [7:0]          PIR_TMCC_SET_10
     *  <SLV-T>   60h     3Bh    [7:0]          PIR_TMCC_SET_11
     *  <SLV-T>   60h     3Ch    [7:0]          PIR_TMCC_SET_12
     *  <SLV-T>   60h     3Dh    [7:0]          PIR_TMCC_SET_13
     *  <SLV-T>   60h     3Eh    [7:0]          PIR_TMCC_SET_14
     */
    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x32, data, 13);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    SLVT_CXD_UnFreezeReg(pDemod);

    result = setTmccInfo(data, pTMCCInfo);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

	sony_cxd_TRACE_RETURN(result);
}

sony_cxd_result_t sony_cxd_demod_isdbt_monitor_ACEEWInfo(sony_cxd_demod_t * pDemod,
                                                 uint8_t * pIsExist,
												 sony_cxd_isdbt_aceew_info_t * pACEEWInfo)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint8_t data[36];
	sony_cxd_TRACE_ENTER("sony_cxd_demod_isdbt_monitor_ACEEWInfo");

    if ((!pDemod) || (!pIsExist) || (!pACEEWInfo)) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	if (pDemod->state != sony_cxd_DEMOD_STATE_ACTIVE) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

	if (pDemod->system != sony_cxd_DTV_SYSTEM_ISDBT) {
        /* Not ISDB-T */
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
	if (SLVT_CXD_FreezeReg(pDemod) != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO);
    }

    result = IsDmdLocked(pDemod);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }
    /*
     * slave    Bank    Addr      Bit           Name
     * ------------------------------------------------------------
     * <SLV-T>   61h     21h       [0]      IREG_AC_EMERGENCY_ACTIVATION
     * <SLV-T>   61h     22h       [7:1]    IREG_AC_EMERGENCY_DECODE_COMMON
     * <SLV-T>   61h     23h       [4:0]    IREG_AC_EMERGENCY_PAGE0ALL1,IREG_AC_EMERGENCY_PAGE0SECOND_EN,IREG_AC_EMERGENCY_DECODE_EN_0,IREG_AC_EMERGENCY_DECODE_EN_1,IREG_AC_EMERGENCY_DECODE_EN_2
     * <SLV-T>   61h     24h - 2eh          IREG_AC_EMERGENCY_DECODE_0_1 - IREG_AC_EMERGENCY_DECODE_0_11
     * <SLV-T>   61h     2fh - 39h          IREG_AC_EMERGENCY_DECODE_1_1 - IREG_AC_EMERGENCY_DECODE_1_11
     * <SLV-T>   61h     3ah - 44h          IREG_AC_EMERGENCY_DECODE_2_1 - IREG_AC_EMERGENCY_DECODE_2_11
     */
    result = pDemod->pI2c->WriteOneRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x61);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    result = pDemod->pI2c->ReadRegister(pDemod->pI2c, pDemod->i2cAddressSLVT, 0x21, data, 36);
	if (result != sony_cxd_RESULT_OK) {
        SLVT_CXD_UnFreezeReg(pDemod);
		sony_cxd_TRACE_RETURN(result);
    }

    SLVT_CXD_UnFreezeReg(pDemod);

    if(data[0] & 0x01) {
        uint8_t i = 0;
        *pIsExist = 1;

        pACEEWInfo->startEndFlag = (uint8_t)((data[1] >> 6) & 0x03);
        pACEEWInfo->updateFlag = (uint8_t)((data[1] >> 4) & 0x03);
        pACEEWInfo->signalId = (uint8_t)((data[1] >> 1) & 0x07);

        pACEEWInfo->isAreaValid = 0;
        pACEEWInfo->isEpicenter1Valid = 0;
        pACEEWInfo->isEpicenter2Valid = 0;
        if(data[2] & 0x04) {
            /* Area data is valid */
            pACEEWInfo->isAreaValid = 1;
            for (i = 0; i < 11; i++) {
                pACEEWInfo->areaInfo.data[i] = data[3 + i];
            }
        }
        if(data[2] & 0x02) {
            /* Epicenter1 data is valid */
            pACEEWInfo->isEpicenter1Valid = 1;
            for (i = 0; i < 11; i++) {
                pACEEWInfo->epicenter1Info.data[i] = data[14 + i];
            }
        }
        if(data[2] & 0x01) {
            /* Epicenter2 data is valid */
            pACEEWInfo->isEpicenter2Valid = 1;
            for (i = 0; i < 11; i++) {
                pACEEWInfo->epicenter2Info.data[i] = data[25 + i];
            }
        }
    } else {
        *pIsExist = 0;
    }

	sony_cxd_TRACE_RETURN(result);
}

/*------------------------------------------------------------------------------
  Static Functions
------------------------------------------------------------------------------*/
static sony_cxd_result_t IsDmdLocked(sony_cxd_demod_t* pDemod)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint8_t dmdLock = 0;
    uint8_t tsLock  = 0;
    uint8_t unlock  = 0;
	sony_cxd_TRACE_ENTER("IsDmdLocked");

    if (!pDemod) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

	result = sony_cxd_demod_isdbt_monitor_SyncStat(pDemod, &dmdLock, &tsLock, &unlock);
	if (result != sony_cxd_RESULT_OK) {
		sony_cxd_TRACE_RETURN(result);
    }

    if (dmdLock) {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
    } else {
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE);
    }
}

static sony_cxd_result_t setTmccInfo(uint8_t* pData, sony_cxd_isdbt_tmcc_info_t* pTmccInfo)
{
	sony_cxd_result_t result = sony_cxd_RESULT_OK;
	sony_cxd_TRACE_ENTER("setTmccInfo");

    /* Null check */
    if(!pTmccInfo){
		sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* System */
	pTmccInfo->systemId = (sony_cxd_isdbt_system_t)sony_cxd_BitSplitFromByteArray(pData, 0, 2);
    /* Count down index Bit 2-5 */
	pTmccInfo->countDownIndex = (uint8_t)sony_cxd_BitSplitFromByteArray(pData, 2, 4);
    /* EWS flag Bit 6 */
	pTmccInfo->ewsFlag = (uint8_t)sony_cxd_BitSplitFromByteArray(pData, 6, 1);


    /* Current Info Bit 7 */
	pTmccInfo->currentInfo.isPartial = (uint8_t)sony_cxd_BitSplitFromByteArray(pData, 7, 1);

    /* Layer-A Modulation Bit 8-10 */
	pTmccInfo->currentInfo.layerA.modulation = (sony_cxd_isdbt_modulation_t)sony_cxd_BitSplitFromByteArray(pData, 8, 3);
    /* Layer-A Code rate Bit 11-13 */
	pTmccInfo->currentInfo.layerA.codingRate = (sony_cxd_isdbt_coding_rate_t)sony_cxd_BitSplitFromByteArray(pData, 11, 3);
    /* Layer-A Interleave Bit 14-16 */
	pTmccInfo->currentInfo.layerA.ilLength = (sony_cxd_isdbt_il_length_t)sony_cxd_BitSplitFromByteArray(pData, 14, 3);
    /* Layer-A Segment Num 17-20 */
	pTmccInfo->currentInfo.layerA.segmentsNum = (uint8_t)sony_cxd_BitSplitFromByteArray(pData, 17, 4);


    /* Layer-B Modulation Bit 21-23 */
	pTmccInfo->currentInfo.layerB.modulation = (sony_cxd_isdbt_modulation_t)sony_cxd_BitSplitFromByteArray(pData, 21, 3);
    /* Layer-B Code rate Bit 24-26 */
	pTmccInfo->currentInfo.layerB.codingRate = (sony_cxd_isdbt_coding_rate_t)sony_cxd_BitSplitFromByteArray(pData, 24, 3);
    /* Layer-B Interleave Bit 27-29 */
	pTmccInfo->currentInfo.layerB.ilLength = (sony_cxd_isdbt_il_length_t)sony_cxd_BitSplitFromByteArray(pData, 27, 3);
    /* Layer-B Segment Num 30-33 */
	pTmccInfo->currentInfo.layerB.segmentsNum = (uint8_t)sony_cxd_BitSplitFromByteArray(pData, 30, 4);


    /* Layer-C Modulation Bit 34-36 */
	pTmccInfo->currentInfo.layerC.modulation = (sony_cxd_isdbt_modulation_t)sony_cxd_BitSplitFromByteArray(pData, 34, 3);
    /* Layer-C Code rate Bit 37-39 */
	pTmccInfo->currentInfo.layerC.codingRate = (sony_cxd_isdbt_coding_rate_t)sony_cxd_BitSplitFromByteArray(pData, 37, 3);
    /* Layer-C Interleave Bit 40-42 */
	pTmccInfo->currentInfo.layerC.ilLength = (sony_cxd_isdbt_il_length_t)sony_cxd_BitSplitFromByteArray(pData, 40, 3);
    /* Layer-C Segment Num 43-46 */
	pTmccInfo->currentInfo.layerC.segmentsNum = (uint8_t)sony_cxd_BitSplitFromByteArray(pData, 43, 4);


    /* Next 47 */
	pTmccInfo->nextInfo.isPartial = (uint8_t)sony_cxd_BitSplitFromByteArray(pData, 47, 1);

    /* Layer-A Modulation Bit 48-50 */
	pTmccInfo->nextInfo.layerA.modulation = (sony_cxd_isdbt_modulation_t)sony_cxd_BitSplitFromByteArray(pData, 48, 3);
    /* Layer-A Code rate Bit 51-53 */
	pTmccInfo->nextInfo.layerA.codingRate = (sony_cxd_isdbt_coding_rate_t)sony_cxd_BitSplitFromByteArray(pData, 51, 3);
    /* Layer-A Interleave Bit 54-56 */
	pTmccInfo->nextInfo.layerA.ilLength = (sony_cxd_isdbt_il_length_t)sony_cxd_BitSplitFromByteArray(pData, 54, 3);
    /* Layer-A Segment Num 57-60 */
	pTmccInfo->nextInfo.layerA.segmentsNum = (uint8_t)sony_cxd_BitSplitFromByteArray(pData, 57, 4);


    /* Layer-B Modulation Bit 61-63 */
	pTmccInfo->nextInfo.layerB.modulation = (sony_cxd_isdbt_modulation_t)sony_cxd_BitSplitFromByteArray(pData, 61, 3);
    /* Layer-B Code rate Bit 64-66 */
	pTmccInfo->nextInfo.layerB.codingRate = (sony_cxd_isdbt_coding_rate_t)sony_cxd_BitSplitFromByteArray(pData, 64, 3);
    /* Layer-B Interleave Bit 67-69 */
	pTmccInfo->nextInfo.layerB.ilLength = (sony_cxd_isdbt_il_length_t)sony_cxd_BitSplitFromByteArray(pData, 67, 3);
    /* Layer-B Segment Num 70-73 */
	pTmccInfo->nextInfo.layerB.segmentsNum = (uint8_t)sony_cxd_BitSplitFromByteArray(pData, 70, 4);


    /* Layer-C Modulation Bit 74-76 */
	pTmccInfo->nextInfo.layerC.modulation = (sony_cxd_isdbt_modulation_t)sony_cxd_BitSplitFromByteArray(pData, 74, 3);
    /* Layer-C Code rate Bit 77-79 */
	pTmccInfo->nextInfo.layerC.codingRate = (sony_cxd_isdbt_coding_rate_t)sony_cxd_BitSplitFromByteArray(pData, 77, 3);
    /* Layer-C Interleave Bit 80-82 */
	pTmccInfo->nextInfo.layerC.ilLength = (sony_cxd_isdbt_il_length_t)sony_cxd_BitSplitFromByteArray(pData, 80, 3);
    /* Layer-C Segment Num 83-86 */
	pTmccInfo->nextInfo.layerC.segmentsNum = (uint8_t)sony_cxd_BitSplitFromByteArray(pData, 83, 4);

	sony_cxd_TRACE_RETURN(result);
}


