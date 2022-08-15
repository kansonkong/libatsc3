/*****************************************************************************************   
 * FILE NAME          : mxlware_eagle_utils.h
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 29/Sep/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains Mxlware helper functions header
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
#ifndef __MXLWARE_EAGLE_UTILS_H__
#define __MXLWARE_EAGLE_UTILS_H__

#include "mxl_datatypes.h"
#include "mxl_eagle_apis.h"




/*****************************************************************************************
    Defines
 ****************************************************************************************/
#define MXL_EAGLE_FW_HEADER_SIZE          16
#define MXL_EAGLE_FW_SEGMENT_HEADER_SIZE  8
#define MXL_EAGLE_MAX_I2C_PACKET_SIZE     58  //calculated as: ((MIN(max i2c driver buffer, 255) - 6) & ~3) + 6.  currently set according to USB-ISS driver
#define MXL_EAGLE_I2C_MHEADER_SIZE        6
#define MXL_EAGLE_I2C_PHEADER_SIZE        2

#define MXL_PARAMETER_VALIDATE(input, lowerLimit, upperLimit)   \
	if (((input + 1) < (lowerLimit + 1)) || (input > upperLimit))           \
    {                                                           \
      status = MXL_E_INVALID_PARAMETER;                           \
      break;                                                    \
    }


/*****************************************************************************************
    Typedefs
 ****************************************************************************************/
//ENUMS
typedef enum
{
	MXL_EAGLE_LITTLE_ENDIAN,
	MXL_EAGLE_BIG_ENDIAN,
	MXL_EAGLE_ANY_ENDIAN,
} MXL_EAGLE_ENDIANNESS_TYPE_E;




/*****************************************************************************************
    Functions Prototype
 ****************************************************************************************/
UINT8 MxL_EAGLE_UTILS_PushUint8(UINT8 u8Value, UINT8** ptr);
UINT8 MxL_EAGLE_UTILS_PushUint16(UINT16 u16Value, UINT8** ptr);
UINT8 MxL_EAGLE_UTILS_PushUint32(UINT32 u32Value, UINT8** ptr);
UINT8 MxL_EAGLE_UTILS_PushSint16(SINT16 s16Value, UINT8** ptr);
UINT8 MxL_EAGLE_UTILS_PopUint8(UINT8** ptr);
UINT16 MxL_EAGLE_UTILS_PopUint16(UINT8** ptr);
UINT32 MxL_EAGLE_UTILS_PopUint32(UINT8** ptr);
UINT64 MxL_EAGLE_UTILS_PopUint64(UINT8** ptr);
SINT8 MxL_EAGLE_UTILS_PopSint8(UINT8** ptr);
SINT16 MxL_EAGLE_UTILS_PopSint16(UINT8** ptr);
SINT32 MxL_EAGLE_UTILS_PopSint32(UINT8** ptr);
SINT64 MxL_EAGLE_UTILS_PopSint64(UINT8** ptr);

//MXL_EAGLE_DEV_CONTEXT_T* MxL_EAGLE_Ctrl_GetDeviceContext(UINT8 devId);
void MxLWare_EAGLE_UTILS_EndiannessDetect				(void);
void MxL_EAGLE_UTILS_EndiannessMemCpy					(UINT8 *pDestBuffer, UINT8 *pSourceBuffer, UINT8 size, MXL_EAGLE_ENDIANNESS_TYPE_E endiannessType);
UINT32 MxLWare_EAGLE_UTILS_CheckSumCalc					(UINT8 *pBuffer, UINT32 size);
MXL_STATUS_E MxLWare_EAGLE_UTILS_ValidateFwHeader		(UINT8 *buffer, UINT32 bufferLen);
MXL_STATUS_E MxLWare_EAGLE_UTILS_DownloadFwSegment		(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT32 bufferLen, UINT32 *index, MXL_CALLBACK_FN_T pCallbackFn);
MXL_STATUS_E MxLWare_EAGLE_UTILS_EnhanceI2cMemWrite		(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT32 addr, UINT8 *pBuffer, UINT32 size);
MXL_STATUS_E MxLWare_EAGLE_UTILS_EnhanceI2cMemRead		(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT32 addr, UINT8 *pBuffer, UINT32 size);
MXL_STATUS_E MxLWare_EAGLE_UTILS_EnhanceI2cProtWrite	(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT32 size);
MXL_STATUS_E MxLWare_EAGLE_UTILS_EnhanceI2cProtRead		(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT32 size);
MXL_STATUS_E MxLWare_EAGLE_UTILS_SendAndReceive			(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 opcode, UINT8 *pTxPayload, UINT8 txPayloadSize, UINT8 *pRxPayload, UINT8 rxExpectedPayloadSize);




#endif  //__MXLWARE_EAGLE_UTILS_H__

