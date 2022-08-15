/*****************************************************************************************   
 * FILE NAME          : mxlware_eagle_atsc_api.c
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 28/Dec/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains MxLWare ATSC APIs
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
 *  \defgroup ATSC
 *  @brief     This section defines the ATSC Demodulator APIs for Eagle.
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
#ifdef MXL_EAGLE_ATSC_ENABLE

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgAtscDemodInit
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API initializes the ATSC demodulator
* 
* \param[in]      devId                 The MxL device id
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgAtscDemodInit(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;

  MXLENTERAPISTR(devId)

  status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                              MXL_EAGLE_OPCODE_ATSC_INIT_SET, 
                                              NULL,
                                              0,
                                              NULL,
                                              0);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgAtscDemodAcquireCarrier
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API restarts the carrier acquisition procedure
* 
* \param[in]      devId                 The MxL device id
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgAtscDemodAcquireCarrier(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;

  MXLENTERAPISTR(devId)

	  status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                              MXL_EAGLE_OPCODE_ATSC_ACQUIRE_CARRIER_SET, 
                                              NULL,
                                              0,
                                              NULL,
                                              0);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqAtscDemodStatus
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API returns the demodulator status
* 
* \param[in]      devId                 The MxL device id
* \param[out]     pAtscStatStruct       Pointer to MXL_EAGLE_ATSC_DEMOD_STATUS_T structure to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqAtscDemodStatus(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_ATSC_DEMOD_STATUS_T *pAtscStatStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;

	MXLENTERAPISTR(devId)

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_ATSC_STATUS_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_ATSC_DEMOD_STATUS_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		pAtscStatStruct->snrDbTenths = MxL_EAGLE_UTILS_PopSint16(&pu8RxPayload);
		pAtscStatStruct->timingOffset = MxL_EAGLE_UTILS_PopSint16(&pu8RxPayload);
		pAtscStatStruct->carrierOffsetHz = MxL_EAGLE_UTILS_PopSint32(&pu8RxPayload);
		pAtscStatStruct->isFrameLock = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pAtscStatStruct->isAtscLock = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pAtscStatStruct->isFecLock = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
	}

	MXLEXITAPISTR(devId, status)

	return status;
}


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqAtscDemodErrorCounters
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 30/04/2017
*
* \brief          This API returns error statistics data counters
* 
* \param[in]      devId                 The MxL device id
* \param[out]     pErrCountersStruct    Pointer to MXL_EAGLE_ATSC_DEMOD_ERROR_COUNTERS_T structure to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqAtscDemodErrorCounters(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_ATSC_DEMOD_ERROR_COUNTERS_T *pErrCountersStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;

	MXLENTERAPISTR(devId)

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_ATSC_ERROR_COUNTERS_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_ATSC_DEMOD_ERROR_COUNTERS_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		pErrCountersStruct->errorPackets = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
		pErrCountersStruct->totalPackets = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
		pErrCountersStruct->errorBytes = MxL_EAGLE_UTILS_PopUint32(&pu8RxPayload);
	}

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqAtscDemodEqualizerFilter
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 29/03/2017
*
* \brief          This API returns the equalizer filter taps
* 
* \param[in]      devId                 The MxL device id
* \param[out]     pEquFilterStruct      Pointer to MXL_EAGLE_ATSC_DEMOD_EQU_FILTER_T structure to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqAtscDemodEqualizerFilter(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_ATSC_DEMOD_EQU_FILTER_T *pEquFilterStruct)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;
  UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE], *pRxBuffer;
  UINT8         seqNum, rxExpectedPayloadSize;
  UINT16        rxPayloadLeftToReceive;

  MXLENTERAPISTR(devId)

  do 
  {
    //DFE taps
    seqNum = 0;
    pRxBuffer = (UINT8*)pEquFilterStruct->dfeTaps;
    rxPayloadLeftToReceive = MXL_EAGLE_ATSC_DFE_TAPS_LENGTH;

    while (rxPayloadLeftToReceive > 0)
    {
      seqNum++;
      rxExpectedPayloadSize = (rxPayloadLeftToReceive < 48) ? rxPayloadLeftToReceive : 48;

	  status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                                  MXL_EAGLE_OPCODE_ATSC_EQUALIZER_FILTER_DFE_TAPS_GET, 
                                                  &seqNum,
                                                  sizeof(UINT8),
                                                  rxBuffer,
                                                  rxExpectedPayloadSize);

      if (status != MXL_E_SUCCESS) break;

      MxL_EAGLE_Oem_MemCpy(pRxBuffer, rxBuffer, rxExpectedPayloadSize);
      pRxBuffer += rxExpectedPayloadSize;
      rxPayloadLeftToReceive -= rxExpectedPayloadSize;
    }

    if (status != MXL_E_SUCCESS) break;

    //FFE taps
    seqNum = 0;
    pRxBuffer = (UINT8*)pEquFilterStruct->ffeTaps;
    rxPayloadLeftToReceive = MXL_EAGLE_ATSC_FFE_TAPS_LENGTH * sizeof(SINT16);

    while (rxPayloadLeftToReceive > 0)
    {
      seqNum++;
      rxExpectedPayloadSize = (rxPayloadLeftToReceive < 48) ? rxPayloadLeftToReceive : 48;

	  status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                                  MXL_EAGLE_OPCODE_ATSC_EQUALIZER_FILTER_FFE_TAPS_GET, 
                                                  &seqNum,
                                                  sizeof(UINT8),
                                                  rxBuffer,
                                                  rxExpectedPayloadSize);

      if (status != MXL_E_SUCCESS) break;

      MxL_EAGLE_Oem_MemCpy(pRxBuffer, rxBuffer, rxExpectedPayloadSize);
      pRxBuffer += rxExpectedPayloadSize;
      rxPayloadLeftToReceive -= rxExpectedPayloadSize;
    }
  } while(0);

  MXLEXITAPISTR(devId, status)

  return status;
}

#endif //MXL_EAGLE_ATSC_ENABLE
