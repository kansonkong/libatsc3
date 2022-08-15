/*****************************************************************************************   
 * FILE NAME          : mxlware_eagle_tuner_api.c
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 15/Fec/2017
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains MxLWare Tuner APIs
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
 *  \defgroup Tuner
 *  @brief     This section defines the Tuner APIs for Eagle.
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
/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgTunerChannelTune
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API tunes the tuner according to the RF channel parameters
* 
* \param[in]      devId                 The MxL device id
* \param[in]      pChannelTuneStruct    Pointer to MXL_EAGLE_TUNER_CHANNEL_PARAMS_T
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgTunerChannelTune(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_TUNER_CHANNEL_PARAMS_T *pChannelTuneStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         txBuffer[MXL_EAGLE_TUNER_CHANNEL_PARAMS_SIZE] = { 0 };
	UINT8         *pu8TxPayload = txBuffer;

	MXLENTERAPISTR(devId)

	do
	{
		/* sanity checks */
		MXL_PARAMETER_VALIDATE(pChannelTuneStruct->freqInHz, 44000000, 1006000000);
		MXL_PARAMETER_VALIDATE(pChannelTuneStruct->bandWidth, MXL_EAGLE_TUNER_BW_6MHz, MXL_EAGLE_TUNER_BW_8MHz);
		MXL_PARAMETER_VALIDATE(pChannelTuneStruct->tuneMode, MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_VIEW, MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_SCAN);

		MxL_EAGLE_Oem_MemSet(txBuffer, 0, MXL_EAGLE_TUNER_CHANNEL_PARAMS_SIZE);
		MxL_EAGLE_UTILS_PushUint32(pChannelTuneStruct->freqInHz, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pChannelTuneStruct->tuneMode, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pChannelTuneStruct->bandWidth, &pu8TxPayload);

		status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
													MXL_EAGLE_OPCODE_TUNER_CHANNEL_TUNE_SET,
													txBuffer,
													MXL_EAGLE_TUNER_CHANNEL_PARAMS_SIZE,
													NULL,
													0);
	} while (0);

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqTunerLockStatus
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API returns the RF and REF lock status
* 
* \param[in]      devId                   The MxL device id
* \param[out]     pTunerLockStatusStruct  Pointer to MXL_EAGLE_TUNER_LOCK_STATUS_T structure to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqTunerLockStatus(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_TUNER_LOCK_STATUS_T *pTunerLockStatusStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;

	MXLENTERAPISTR(devId)

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_TUNER_LOCK_STATUS_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_TUNER_LOCK_STATUS_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		pTunerLockStatusStruct->isRfPllLocked = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pTunerLockStatusStruct->isRefPllLocked = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
	}

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqTunerAgcStatus
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API returns the AGC lock and gain status
* 
* \param[in]      devId                 The MxL device id
* \param[out]     pTunerAgcStatusStruct Pointer to MXL_EAGLE_TUNER_AGC_STATUS_T structure to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqTunerAgcStatus(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_TUNER_AGC_STATUS_T *pTunerAgcStatusStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;

	MXLENTERAPISTR(devId)

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_TUNER_AGC_STATUS_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_TUNER_AGC_STATUS_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		pTunerAgcStatusStruct->isLocked = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pTunerAgcStatusStruct->rawAgcGain = MxL_EAGLE_UTILS_PopUint16(&pu8RxPayload);
		pTunerAgcStatusStruct->rxPowerDbHundredths = MxL_EAGLE_UTILS_PopSint16(&pu8RxPayload);
	}

	MXLEXITAPISTR(devId, status)

	return status;
}
