/*****************************************************************************************   
 * FILE NAME          : mxl_eagle_oem.h
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 29/Sep/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains OEM functions header
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
#ifndef __MXL_EAGLE_OEM_H__
#define __MXL_EAGLE_OEM_H__

#include "mxl_datatypes.h"




/*****************************************************************************************
    Defines
 ****************************************************************************************/
#define MXL_EAGLE_MAX_NUM_DEVICES       5


/*****************************************************************************************
    Functions Prototype
 ****************************************************************************************/
MXL_STATUS_E MxL_EAGLE_Oem_Init			(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, void *pOemData, unsigned char i2c_bus, unsigned char i2c_add);
MXL_STATUS_E MxL_EAGLE_Oem_UnInit		(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device);
MXL_STATUS_E MxL_EAGLE_Oem_I2C_Write	(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT16 bufferLen);
MXL_STATUS_E MxL_EAGLE_Oem_I2C_Read		(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT16 bufferLen);
MXL_STATUS_E MxL_EAGLE_Oem_DevReset		(UINT8 devId);
void MxL_EAGLE_Oem_SleepInMs			(UINT16 mSec);
MXL_STATUS_E MxL_EAGLE_Oem_MemSet		(UINT8 *pBuffer, UINT8 value, UINT32 length);
MXL_STATUS_E MxL_EAGLE_Oem_MemCpy		(UINT8 *pDest, UINT8 *pSrc, UINT32 length);
void MxL_EAGLE_Oem_MutexGet				(UINT8 devId);
void MxL_EAGLE_Oem_MutexRelease			(UINT8 devId);



#endif  //__MXL_EAGLE_OEM_H__

