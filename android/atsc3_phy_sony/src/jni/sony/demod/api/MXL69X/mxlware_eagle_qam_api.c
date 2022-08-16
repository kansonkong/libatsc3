/*****************************************************************************************   
 * FILE NAME          : mxlware_eagle_qam_api.c
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 11/Dec/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains MxLWare QAM APIs
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
 *  \defgroup QAM
 *  @brief     This section defines the QAM Demodulator APIs for Eagle.
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
#ifdef MXL_EAGLE_QAM_ENABLE

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgQamDemodParams
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 15/03/2017
*
* \brief          This API configures the QAM demodulator 
* 
* \param[in]      devId                 The MxL device id
* \param[in]      pQamParamsStruct      Pointer to MXL_EAGLE_QAM_DEMOD_PARAMS_T containing QAM configuration
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgQamDemodParams(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_QAM_DEMOD_PARAMS_T *pQamParamsStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         txBuffer[MXL_EAGLE_QAM_DEMOD_PARAMS_SIZE] = { 0 };
	UINT8         *pu8TxPayload = txBuffer;

	MXLENTERAPISTR(devId)

	do
	{
		/* sanity checks */
		if (pQamParamsStruct->symbolRateHz == 0)
		{
			if (pQamParamsStruct->annexType == MXL_EAGLE_QAM_DEMOD_ANNEX_A)
			{
				pQamParamsStruct->symbolRateHz = MXL_EAGLE_DEFAULT_SYMBOLRATE_ANNEX_A;
			}
			else if (pQamParamsStruct->annexType == MXL_EAGLE_QAM_DEMOD_ANNEX_B)
			{
				pQamParamsStruct->symbolRateHz = MXL_EAGLE_DEFAULT_SYMBOLRATE_ANNEX_B_QAM64;
			}
		}

		if (pQamParamsStruct->symbolRate256QamHz == 0)
		{
			if (pQamParamsStruct->annexType == MXL_EAGLE_QAM_DEMOD_ANNEX_B)
			{
				pQamParamsStruct->symbolRate256QamHz = MXL_EAGLE_DEFAULT_SYMBOLRATE_ANNEX_B_QAM256;
			}
		}

		MXL_PARAMETER_VALIDATE(pQamParamsStruct->annexType, MXL_EAGLE_QAM_DEMOD_ANNEX_B, MXL_EAGLE_QAM_DEMOD_ANNEX_A);
		MXL_PARAMETER_VALIDATE(pQamParamsStruct->qamType, MXL_EAGLE_QAM_DEMOD_QAM16, MXL_EAGLE_QAM_DEMOD_AUTO);
		MXL_PARAMETER_VALIDATE(pQamParamsStruct->spurCancellerEnable, MXL_E_DISABLE, MXL_E_ENABLE);
		MXL_PARAMETER_VALIDATE(pQamParamsStruct->iqFlip, MXL_EAGLE_DEMOD_IQ_NORMAL, MXL_EAGLE_DEMOD_IQ_AUTO);
		MXL_PARAMETER_VALIDATE(pQamParamsStruct->searchRangeIdx, 0, 15);
		MXL_PARAMETER_VALIDATE(pQamParamsStruct->symbolRateHz, 2000000, 7200000);

		if ((pQamParamsStruct->annexType == MXL_EAGLE_QAM_DEMOD_ANNEX_B) &&
			((pQamParamsStruct->qamType == MXL_EAGLE_QAM_DEMOD_QAM256) || (pQamParamsStruct->qamType == MXL_EAGLE_QAM_DEMOD_AUTO)))
		{
			MXL_PARAMETER_VALIDATE(pQamParamsStruct->symbolRate256QamHz, 2000000, 7200000);
		}

		if ((pQamParamsStruct->annexType == MXL_EAGLE_QAM_DEMOD_ANNEX_B) &&
			((pQamParamsStruct->qamType != MXL_EAGLE_QAM_DEMOD_QAM64) &&
			(pQamParamsStruct->qamType != MXL_EAGLE_QAM_DEMOD_QAM256) &&
			(pQamParamsStruct->qamType != MXL_EAGLE_QAM_DEMOD_AUTO)))
		{
			status = MXL_E_INVALID_PARAMETER;
			break;
		}

		MxL_EAGLE_Oem_MemSet(txBuffer, 0, MXL_EAGLE_QAM_DEMOD_PARAMS_SIZE);
		MxL_EAGLE_UTILS_PushUint8(pQamParamsStruct->annexType, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pQamParamsStruct->qamType, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pQamParamsStruct->iqFlip, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pQamParamsStruct->searchRangeIdx, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pQamParamsStruct->spurCancellerEnable, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint32(pQamParamsStruct->symbolRateHz, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint32(pQamParamsStruct->symbolRate256QamHz, &pu8TxPayload);

		status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
													MXL_EAGLE_OPCODE_QAM_PARAMS_SET,
													txBuffer,
													MXL_EAGLE_QAM_DEMOD_PARAMS_SIZE,
													NULL,
													0);
	} while (0);

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgQamDemodRestart
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
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgQamDemodRestart(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;

  MXLENTERAPISTR(devId)

	  status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                              MXL_EAGLE_OPCODE_QAM_RESTART_SET, 
                                              NULL,
                                              0,
                                              NULL,
                                              0);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqQamDemodStatus
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 15/03/2017
*
* \brief          This API returns the QAM demodulator status
* 
* \param[in]      devId                 The MxL device id
* \param[out]     pQamStatusStruct      Pointer to MXL_EAGLE_QAM_DEMOD_STATUS_T to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqQamDemodStatus(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_QAM_DEMOD_STATUS_T *pQamStatusStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;

	MXLENTERAPISTR(devId)

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_QAM_STATUS_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_QAM_DEMOD_STATUS_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		pQamStatusStruct->annexType = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pQamStatusStruct->qamType = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pQamStatusStruct->iqFlip = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pQamStatusStruct->interleaverDepthI = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pQamStatusStruct->interleaverDepthJ = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pQamStatusStruct->isQamLocked = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pQamStatusStruct->isFecLocked = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pQamStatusStruct->isMpegLocked = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pQamStatusStruct->snrDbTenth = MxL_EAGLE_UTILS_PopUint16(&pu8RxPayload);
		pQamStatusStruct->timingOffset = MxL_EAGLE_UTILS_PopSint16(&pu8RxPayload);
		pQamStatusStruct->carrierOffsetHz = MxL_EAGLE_UTILS_PopSint32(&pu8RxPayload);
	}

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqQamDemodErrorCounters
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API Returns error statistics data counters
* 
* \param[in]      devId                 The MxL device id
* \param[out]     pErrCountersStruct    Pointer to MXL_EAGLE_QAM_DEMOD_ERROR_COUNTERS_T to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqQamDemodErrorCounters(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_QAM_DEMOD_ERROR_COUNTERS_T *pErrCountersStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;

	MXLENTERAPISTR(devId)

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_QAM_ERROR_COUNTERS_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_QAM_DEMOD_ERROR_COUNTERS_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		pErrCountersStruct->correctedCodeWords = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
		pErrCountersStruct->uncorrectedCodeWords = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
		pErrCountersStruct->totalCodeWordsReceived = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
		pErrCountersStruct->correctedBits = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
		pErrCountersStruct->errorMpegFrames = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
		pErrCountersStruct->mpegFramesReceived = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
		pErrCountersStruct->erasures = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
	}

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqQamDemodConstellationValue
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API Returns slicer I/Q values
* 
* \param[in]      devId                 The MxL device id
* \param[out]     pConstValStruct       Pointer to MXL_EAGLE_QAM_DEMOD_CONSTELLATION_VAL_T to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqQamDemodConstellationValue(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_QAM_DEMOD_CONSTELLATION_VAL_T *pConstValStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;
	UINT8         u8Index = 0;

	MXLENTERAPISTR(devId)

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_QAM_CONSTELLATION_VALUE_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_QAM_DEMOD_CONSTELLATION_VAL_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		for (u8Index = 0; u8Index < 12; u8Index++)
		{
			pConstValStruct->iValue[u8Index] = MxL_EAGLE_UTILS_PopSint16(&pu8RxPayload);
		}

		for (u8Index = 0; u8Index < 12; u8Index++)
		{
			pConstValStruct->qValue[u8Index] = MxL_EAGLE_UTILS_PopSint16(&pu8RxPayload);
		}
	}

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqQamDemodEqualizerFilter
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API Returns the equalizer filter taps
* 
* \param[in]      devId                 The MxL device id
* \param[out]     pEquFilterStruct      Pointer to MXL_EAGLE_QAM_DEMOD_EQU_FILTER_T to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqQamDemodEqualizerFilter(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_QAM_DEMOD_EQU_FILTER_T *pEquFilterStruct)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;
  UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
  UINT8         *pBuffer = NULL;

  MXLENTERAPISTR(devId)

  do 
  {
    //FFE taps + leading tap index
	  status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                                MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_FFE_GET, 
                                                NULL,
                                                0,
                                                rxBuffer,
                                                17*sizeof(UINT16));

    if (status != MXL_E_SUCCESS) break;

    pEquFilterStruct->ffeLeadingTapIndex = rxBuffer[0];
    MxL_EAGLE_Oem_MemCpy((UINT8*)pEquFilterStruct->ffeTaps, rxBuffer + 2, 16 * sizeof(UINT16));


    //SPUR taps
    pBuffer = (UINT8*)pEquFilterStruct->spurTaps;
	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                                MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_SPUR_START_GET, 
                                                NULL,
                                                0,
                                                rxBuffer,
                                                24*sizeof(UINT16));

    if (status != MXL_E_SUCCESS) break;

    MxL_EAGLE_Oem_MemCpy(pBuffer, rxBuffer, 24 * sizeof(UINT16));
    pBuffer += (24 * sizeof(UINT16));

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                                MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_SPUR_END_GET, 
                                                NULL,
                                                0,
                                                rxBuffer,
                                                8*sizeof(UINT16));

    if (status != MXL_E_SUCCESS) break;

    MxL_EAGLE_Oem_MemCpy(pBuffer, rxBuffer, 8 * sizeof(UINT16));


    //DFE taps number
	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                                MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_TAPS_NUMBER_GET, 
                                                NULL,
                                                0,
                                                rxBuffer,
                                                sizeof(UINT8));

    if (status != MXL_E_SUCCESS) break;

    pEquFilterStruct->dfeTapsNumber = rxBuffer[0];


    //DFE taps
    pBuffer = (UINT8*)pEquFilterStruct->dfeTaps;
	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                                MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_START_GET, 
                                                NULL,
                                                0,
                                                rxBuffer,
                                                24*sizeof(UINT16));

    if (status != MXL_E_SUCCESS) break;

    MxL_EAGLE_Oem_MemCpy(pBuffer, rxBuffer, 24 * sizeof(UINT16));
    pBuffer += 24 * sizeof(UINT16);

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                                MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_MIDDLE_GET, 
                                                NULL,
                                                0,
                                                rxBuffer,
                                                24*sizeof(UINT16));

    if (status != MXL_E_SUCCESS) break;

    MxL_EAGLE_Oem_MemCpy(pBuffer, rxBuffer, 24 * sizeof(UINT16));
    pBuffer += 24 * sizeof(UINT16);

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                                MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_END_GET, 
                                                NULL,
                                                0,
                                                rxBuffer,
                                                24*sizeof(UINT16));

    if (status != MXL_E_SUCCESS) break;

    MxL_EAGLE_Oem_MemCpy(pBuffer, rxBuffer, 24 * sizeof(UINT16));
  } while(0);

  MXLEXITAPISTR(devId, status)

  return status;
}

#endif //MXL_EAGLE_QAM_ENABLE
