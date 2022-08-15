/*****************************************************************************************   
 * FILE NAME          : mxlware_eagle_debug_api.c
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 31/Aug/2017
 * LAST MODIFIED      :
 *
 * DESCRIPTION        : This file contains MxLWare debug API
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
 *  \defgroup Debug
 *  @brief     This section defines the Debug APIs for Eagle.
 *  @{
 *
 ****************************************************************************************/

#include "mxl_datatypes.h"
#include "mxlware_eagle_utils.h"
#include "mxl_eagle_oem_debug.h"
#include "mxl_eagle_oem.h"


/*****************************************************************************************
    API Functions
 ****************************************************************************************/
 /**----------------------------------------------------------------------------------------
 * FUNCTION NAME : MxLWare_EAGLE_API_ReqDebugBuffer
 *
 * AUTHOR        : Rotem Kryzewski
 *
 * DATE CREATED  : 31/08/2017
 *
 * \brief          This API used for debugging the system
 *
 * \param[in]      devId        The MxL device id
 * \param[in]      pBuffer      Pointer to debug buffer
 * \param[in]      bufferSize   Debug buffer size
 * \param[in]      debugNumber  Debug number
 *
 * \returns        MXL_SUCCESS, MXL_NOT_READY, MXL_INVALID_PARAMETER, MXL_FAILURE

 *-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqDebugBuffer(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT8 bufferSize, UINT8 debugNumber)
{
  MXL_STATUS_E                    status = MXL_E_SUCCESS;
  UINT8                           rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE] = {0};
  UINT8                           debugMsgBufferSize = MXL_EAGLE_MAX_I2C_PACKET_SIZE - MXL_EAGLE_HOST_MSG_HEADER_SIZE;
  MXL_EAGLE_INTERNAL_MESSSAGE_T   internalMessageStruct = { 0, 0, {0} };

  MXLENTERAPISTR(devId)

  /* sanity check */
  if (bufferSize < debugMsgBufferSize)
  {
    return MXL_E_INVALID_PARAMETER;
  }

  internalMessageStruct.internalOpcode = debugNumber;

  status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
                                              MXL_EAGLE_OPCODE_INTERNAL,
                                              (UINT8*)&internalMessageStruct,
                                              MXL_EAGLE_INTERNAL_MESSAGE_HEADER_SIZE,
                                              rxBuffer,
                                              debugMsgBufferSize);

  if (status == MXL_E_SUCCESS)
  {
    MxL_EAGLE_Oem_MemCpy(pBuffer, rxBuffer, debugMsgBufferSize);
  }

  MXLEXITAPISTR(devId, status)

  return status;
}
