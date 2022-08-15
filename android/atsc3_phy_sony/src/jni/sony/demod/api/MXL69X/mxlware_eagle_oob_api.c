/*****************************************************************************************   
 * FILE NAME          : mxlware_eagle_oob_api.c
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 31/Jan/2017
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains MxLWare OOB APIs
 *
 ****************************************************************************************
 * This file is licensed under GNU General Public license, except that if you
 * have entered into a signed, written license agreement with MaxLinear covering
 * this file, that agreement applies to this file instead of the GNU General
 * Public License.
 *
 * This file is free software: you can redistribute and/or modify it under the
 * terms of the GNU General Public License, Version 2, as published by the Free
 * Software Foundation, unless a different license applies as provided above.
 *
 * This program is distributed in the hope that it will be useful, but AS-IS and
 * WITHOUT ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, or NONINFRINGEMENT. Redistribution,
 * except as permitted by the GNU General Public License or another license
 * agreement between you and MaxLinear, is prohibited.
 *
 * You should have received a copy of the GNU General Public License, Version 2
 * along with this file; if not, see<http://www.gnu.org/licenses/>.
 ****************************************************************************************
 *                                        Copyright (c) 2016, MaxLinear, Inc.
 ****************************************************************************************/

/****************************************************************************************/
/**
 *  \defgroup api MXLWARE API
 *  @{
 *  \defgroup OOB
 *  @brief     This section defines the OOB Demodulator APIs for Eagle.
 *  @{
 *
 ****************************************************************************************/

#include "mxl_datatypes.h"
#include "mxlware_eagle_product_id.h"
#include "mxlware_eagle_sku_features.h"
#include "mxlware_eagle_utils.h"
#include "mxl_eagle_oem_debug.h"
#include "mxl_eagle_oem.h"


/*****************************************************************************************
    API Functions
 ****************************************************************************************/
#ifdef MXL_EAGLE_OOB_ENABLE

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgOobDemodParams
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 13/03/2017
*
* \brief          This API configures the OOB demodulator
* 
* \param[in]      devId                 The MxL device id
* \param[in]      pOobParamsStruct      Pointer to MXL_EAGLE_OOB_DEMOD_PARAMS_T containing OOB configuration
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgOobDemodParams(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_OOB_DEMOD_PARAMS_T *pOobParamsStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         txBuffer[MXL_EAGLE_OOB_DEMOD_PARAMS_SIZE] = { 0 };
	UINT8         *pu8TxPayload = txBuffer;

	MXLENTERAPISTR(devId)

	do
	{
		/* sanity checks */
		MXL_PARAMETER_VALIDATE(pOobParamsStruct->symbolRate, MXL_EAGLE_OOB_DEMOD_SYMB_RATE_0_772MHz, MXL_EAGLE_OOB_DEMOD_SYMB_RATE_1_544MHz);
		MXL_PARAMETER_VALIDATE(pOobParamsStruct->iqFlip, MXL_EAGLE_DEMOD_IQ_NORMAL, MXL_EAGLE_DEMOD_IQ_AUTO);
		MXL_PARAMETER_VALIDATE(pOobParamsStruct->clockPolarity, MXL_EAGLE_CLOCK_IN_PHASE, MXL_EAGLE_CLOCK_INVERTED);

		MxL_EAGLE_Oem_MemSet(txBuffer, 0, MXL_EAGLE_OOB_DEMOD_PARAMS_SIZE);
		MxL_EAGLE_UTILS_PushUint8(pOobParamsStruct->symbolRate, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pOobParamsStruct->iqFlip, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pOobParamsStruct->clockPolarity, &pu8TxPayload);

		status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
													MXL_EAGLE_OPCODE_OOB_PARAMS_SET,
													txBuffer,
													MXL_EAGLE_OOB_DEMOD_PARAMS_SIZE,
													NULL,
													0);
	} while (0);

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgOobDemodRestart
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API restarts the demodulator block and state machine
* 
* \param[in]      devId                 The MxL device id
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgOobDemodRestart(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;

  MXLENTERAPISTR(devId)

	  status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                              MXL_EAGLE_OPCODE_OOB_RESTART_SET, 
                                              NULL,
                                              0,
                                              NULL,
                                              0);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqOobDemodErrorCounters
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API Returns error statistics data counters
* 
* \param[in]      devId                 The MxL device id
* \param[out]     pErrCountersStruct    Pointer to MXL_EAGLE_OOB_DEMOD_ERROR_COUNTERS_T to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqOobDemodErrorCounters(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_OOB_DEMOD_ERROR_COUNTERS_T *pErrCountersStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;

	MXLENTERAPISTR(devId)

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_OOB_ERROR_COUNTERS_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_OOB_DEMOD_ERROR_COUNTERS_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		pErrCountersStruct->correctedPackets = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
		pErrCountersStruct->uncorrectedPackets = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
		pErrCountersStruct->totalPacketsReceived = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
	}

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqOobDemodStatus
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API returns the OOB demodulator status
* 
* \param[in]      devId                 The MxL device id
* \param[out]     pOobStatusStruct       Pointer to MXL_EAGLE_OOB_DEMOD_STATUS_T to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqOobDemodStatus(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_OOB_DEMOD_STATUS_T *pOobStatusStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;

	MXLENTERAPISTR(devId)

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_OOB_STATUS_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_OOB_DEMOD_STATUS_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		pOobStatusStruct->snrDbTenth = MxL_EAGLE_UTILS_PopUint16(&pu8RxPayload);
		pOobStatusStruct->timingOffset = MxL_EAGLE_UTILS_PopSint16(&pu8RxPayload);
		pOobStatusStruct->carrierOffsetHz = MxL_EAGLE_UTILS_PopSint32(&pu8RxPayload);
		pOobStatusStruct->isQamLocked = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pOobStatusStruct->isFecLocked = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pOobStatusStruct->isMpegLocked = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pOobStatusStruct->iqFlip = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
	}

	MXLEXITAPISTR(devId, status)

	return status;
}

#endif //MXL_EAGLE_OOB_ENABLE
