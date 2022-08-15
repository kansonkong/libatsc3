/*****************************************************************************************   
 * FILE NAME          : mxlware_eagle_sma_api.c
 * 
 * AUTHOR             : Udi Ashkenazi
 * DATE CREATED       : 11/Dec/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains MxLWare SMA APIs
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
 *  \defgroup SMA
 *  @brief     This section defines the Smart Antenna APIs for Eagle.
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
#ifdef MXL_EAGLE_SMA_ENABLE


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgSmaInit
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API Initializes Smart Antenna
* 
* \param[in]      devId                 The MxL device id
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgSmaInit(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;

  MXLENTERAPISTR(devId)

	  status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                              MXL_EAGLE_OPCODE_SMA_INIT_SET, 
                                              NULL,
                                              0,
                                              NULL,
                                              0);

  MXLEXITAPISTR(devId, status)

  return status;
}


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgSmaParams
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 04/04/2017
*
* \brief          This API configures Smart Antenna parameters
* 
* \param[in]      devId                 The MxL device id
* \param[in]      pSmaParamsStruct      Pointer to parameters structure MXL_EAGLE_SMA_PARAMS_T 
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgSmaParams(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_SMA_PARAMS_T *pSmaParamsStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         txBuffer[MXL_EAGLE_SMA_PARAMS_SIZE] = { 0 };
	UINT8         *pu8TxPayload = txBuffer;

	MXLENTERAPISTR(devId)

	do
	{
		/* sanity checks */
		MXL_PARAMETER_VALIDATE(pSmaParamsStruct->fullDuplexEnable, MXL_E_DISABLE, MXL_E_ENABLE);
		MXL_PARAMETER_VALIDATE(pSmaParamsStruct->rxDisable, MXL_E_DISABLE, MXL_E_ENABLE);
		MXL_PARAMETER_VALIDATE(pSmaParamsStruct->idleLogicHigh, MXL_E_DISABLE, MXL_E_ENABLE);

		MxL_EAGLE_Oem_MemSet(txBuffer, 0, MXL_EAGLE_SMA_PARAMS_SIZE);
		MxL_EAGLE_UTILS_PushUint8(pSmaParamsStruct->fullDuplexEnable, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pSmaParamsStruct->rxDisable, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pSmaParamsStruct->idleLogicHigh, &pu8TxPayload);

		status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
													MXL_EAGLE_OPCODE_SMA_PARAMS_SET,
													txBuffer,
													MXL_EAGLE_SMA_PARAMS_SIZE,
													NULL,
													0);
	} while (0);

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgSmaMsgTransmit
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API sends single Smart Antenna message
* 
* \param[in]      devId                 The MxL device id
* \param[in]      pSmaMessage           Pointer to message in MXL_EAGLE_SMA_MESSAGE_T 
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE, MXL_NOT_READY if HW still busy on previous message 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgSmaMsgTransmit(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_SMA_MESSAGE_T *pSmaMessage)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         txBuffer[MXL_EAGLE_SMA_MESSAGE_SIZE] = { 0 };
	UINT8         *pu8TxPayload = txBuffer;

	MXLENTERAPISTR(devId)

	do
	{
		/* sanity checks */
		MXL_PARAMETER_VALIDATE(pSmaMessage->totalNumBits, 8, 31);

		MxL_EAGLE_Oem_MemSet(txBuffer, 0, MXL_EAGLE_SMA_MESSAGE_SIZE);
		MxL_EAGLE_UTILS_PushUint32(pSmaMessage->payloadBits, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pSmaMessage->totalNumBits, &pu8TxPayload);

		status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
													MXL_EAGLE_OPCODE_SMA_TRANSMIT_SET,
													txBuffer,
													MXL_EAGLE_SMA_MESSAGE_SIZE,
													NULL,
													0);
	} while (0);

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqSmaMsgReceive
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API pulls from FW previously received Smart Antenna message
* 
* \param[in]      devId                 The MxL device id
* \param[in]      pSmaMessage           Pointer to message in MXL_EAGLE_SMA_MESSAGE_T to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE, MXL_NOT_READY if no message to pull 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqSmaMsgReceive(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_SMA_MESSAGE_T *pSmaMessage)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;

	MXLENTERAPISTR(devId)

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_SMA_RECEIVE_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_SMA_MESSAGE_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		pSmaMessage->payloadBits = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
		pSmaMessage->totalNumBits = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
	}

	MXLEXITAPISTR(devId, status)

	return status;
}

#endif //MXL_EAGLE_SMA_ENABLE
