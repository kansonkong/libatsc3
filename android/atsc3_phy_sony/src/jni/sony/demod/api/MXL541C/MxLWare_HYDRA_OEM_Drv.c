/*
* Copyright (c) 2011-2013 MaxLinear, Inc. All rights reserved
*
* License type: GPLv2
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*
* This program may alternatively be licensed under a proprietary license from
* MaxLinear, Inc.
*
* See terms and conditions defined in file 'LICENSE.txt', which is part of this
* source code package.
*/

#include "MxLWare_HYDRA_OEM_Defines.h"
#include "MxLWare_HYDRA_OEM_Drv.h"
#include "MxLWare_HYDRA_PhyCtrl.h"
#include "IT9300.h"
//#include <linux/module.h>
//#include <linux/kernel.h>
//#include <linux/slab.h>


//#define DEBUG_LOG

#ifdef DEBUG_LOG
FILE* fp;
#endif


/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_DeviceReset
 *
 * @param[in]   devId           Device ID
 *
 * @author Mahee
 *
 * @date 06/12/2012 Initial release
 *
 * This API performs a hardware reset on Hydra SOC identified by devId.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_DeviceReset(UINT8 devId)
{
	MXL_STATUS_E mxlStatus = MXL_E_SUCCESS;

	devId = devId;

	return mxlStatus;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_SleepInMs
 *
 * @param[in]   delayTimeInMs
 *
 * @author Mahee
 *
 * @date 06/12/2012 Initial release
 *
 * his API will implement delay in milliseconds specified by delayTimeInMs.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
void MxLWare_HYDRA_OEM_SleepInMs(UINT16 delayTimeInMs)
{
	BrUser_delay(NULL, delayTimeInMs);
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_GetCurrTimeInMs
 *
 * @param[out]   msecsPtr
 *
 * @author Mahee
 *
 * @date 06/12/2012 Initial release
 *
 * This API should return system time milliseconds.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
void MxLWare_HYDRA_OEM_GetCurrTimeInMs(UINT64 *msecsPtr)
{
	Dword t;
	//uint64_t	time;
	//BrUser_time64(msecsPtr);
	BrUser_time(&t);
	*msecsPtr = t;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_I2cWrite
 *
 * @param[in]   devId           Device ID
 * @param[in]   dataLen
 * @param[in]   buffPtr
 *
 * @author Mahee
 *
 * @date 06/12/2012 Initial release
 *
 * This API performs an I2C block write by writing data payload to Hydra device.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/

MXL_STATUS_E MxLWare_HYDRA_OEM_I2cWrite(UINT8 devId, UINT16 dataLen, UINT8 *buffPtr)
{
	MXL_STATUS_E			mxlStatus = MXL_E_SUCCESS;
	Dword					error;
	Byte					buf[4];
	Byte					tmp_Len;
	Byte					index;
	Byte					tmp;
	MXL_HYDRA_CONTEXT_T*	devHandlePtr;


	if(dataLen > 250)
	{
		printk("dataLen > 250\n");
		return MXL_E_I2C_ERROR;
	}

	mxlStatus = MxLWare_HYDRA_Ctrl_GetDeviceContext(devId, &devHandlePtr);
	if (mxlStatus != MXL_E_SUCCESS) goto exit;


#ifdef DEBUG_LOG
	fopen_s(&fp, "dumpMXL541.txt", "a");
	fprintf(fp, "I2C Add : 0x%02X\n", devHandlePtr->endeavour_mxl541_i2cAdd);
	fprintf(fp, "wLen : %d\n", dataLen);
	for (int i = 0; i < dataLen; i++)
		fprintf(fp, "wBuf[%d] = 0x%02X\n", i, buffPtr[i]);
	fclose(fp);
#endif


	if (dataLen <= 48)
	{
		error = IT9300_writeGenericRegisters(devHandlePtr->endeavour_mxl541, 0, devHandlePtr->endeavour_mxl541_i2cBus, devHandlePtr->endeavour_mxl541_i2cAdd, (Byte)dataLen, buffPtr);
		if (error)
		{
			mxlStatus = MXL_E_I2C_ERROR;
			goto exit;
		}
	}
	else
	{
		index = 0;
		tmp_Len = (Byte)dataLen;
		while (tmp_Len >= 48)
		{
			error = IT9300_writeRegisters(devHandlePtr->endeavour_mxl541, 0, 0xF001 + index, 48, buffPtr + index);
			if (error)
			{
				mxlStatus = MXL_E_I2C_ERROR;
				goto exit;
			}

			index += 48;
			tmp_Len -= 48;
		}
		if (tmp_Len > 0)
		{
			error = IT9300_writeRegisters(devHandlePtr->endeavour_mxl541, 0, 0xF001 + index, tmp_Len, buffPtr + index);
			if (error)
			{
				mxlStatus = MXL_E_I2C_ERROR;
				goto exit;
			}
		}

		buf[0] = 0xF4;
		buf[1] = devHandlePtr->endeavour_mxl541_i2cBus;
		buf[2] = devHandlePtr->endeavour_mxl541_i2cAdd;
		buf[3] = (Byte)dataLen;
		error = IT9300_writeRegisters(devHandlePtr->endeavour_mxl541, 0, 0x4900, 4, buf);
		if (error)
		{
			mxlStatus = MXL_E_I2C_ERROR;
			goto exit;
		}
		error = IT9300_readRegister(devHandlePtr->endeavour_mxl541, 0, OVA_EXTI2C_STATUS, &tmp);
		if (error)
		{
			mxlStatus = MXL_E_I2C_ERROR;
			goto exit;
		}
		if (tmp != 0)
		{
			mxlStatus = MXL_E_I2C_ERROR;
			goto exit;
		}
	}


exit:

#ifdef DEBUG_LOG
	fopen_s(&fp, "dumpMXL541.txt", "a");
	if (error)
		fprintf(fp, "Fail!!!!!\n");
	fprintf(fp, "Status : 0x%08x, Error code : 0x%08x\n", mxlStatus, error);
	fprintf(fp, "-------------------------------------------\n");
	fclose(fp);
#endif

	return mxlStatus;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_I2cRead
 *
 * @param[in]   devId           Device ID
 * @param[in]   dataLen
 * @param[out]  buffPtr
 *
 * @author Mahee
 *
 * @date 06/12/2012 Initial release
 *
 * This API shall be used to perform I2C block read transaction.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_I2cRead(UINT8 devId, UINT16 dataLen, UINT8 *buffPtr)
{
	MXL_STATUS_E			mxlStatus = MXL_E_SUCCESS;
	Dword					error;
	Byte					buf[4];
	Byte					tmp_Len;
	Byte					index;
	MXL_HYDRA_CONTEXT_T*	devHandlePtr;
	Byte					timeout = 100;


	if(dataLen > 250)
	{
		printk("dataLen > 250\n");
		return MXL_E_I2C_ERROR;
	}

	mxlStatus = MxLWare_HYDRA_Ctrl_GetDeviceContext(devId, &devHandlePtr);
	if (mxlStatus != MXL_E_SUCCESS) goto exit;

#ifdef DEBUG_LOG
	fopen_s(&fp, "dumpMXL541.txt", "a");
	fprintf(fp, "I2C Add : 0x%02X\n", devHandlePtr->endeavour_mxl541_i2cAdd | 1);
	fprintf(fp, "rLen : %d\n", dataLen);
	fclose(fp);
#endif

	BrUser_delay(NULL, 1);

	if (dataLen <= 48)
	{
		error = IT9300_readGenericRegisters(devHandlePtr->endeavour_mxl541, 0, devHandlePtr->endeavour_mxl541_i2cBus, devHandlePtr->endeavour_mxl541_i2cAdd, (Byte)dataLen, buffPtr);	
		if (error)
		{
			mxlStatus = MXL_E_I2C_ERROR;
			goto exit;
		}
	}
	else
	{
		buf[0] = 0xF5;
		buf[1] = devHandlePtr->endeavour_mxl541_i2cBus;
		buf[2] = devHandlePtr->endeavour_mxl541_i2cAdd;
		buf[3] = (Byte)dataLen;
		error = IT9300_writeRegisters(devHandlePtr->endeavour_mxl541, 0, 0x4900, 4, buf);
		if (error)
		{
			mxlStatus = MXL_E_I2C_ERROR;
			goto exit;
		}
		error = IT9300_readRegister(devHandlePtr->endeavour_mxl541, 0, OVA_EXTI2C_STATUS, &buf[0]);
		if (error)
		{
			mxlStatus = MXL_E_I2C_ERROR;
			goto exit;
		}
		if (buf[0] != 0)
		{
			mxlStatus = MXL_E_I2C_ERROR;
			goto exit;
		}


		index = 0;
		tmp_Len = (Byte)dataLen;
		while (tmp_Len >= 48)
		{
			error = IT9300_readRegisters(devHandlePtr->endeavour_mxl541, 0, 0xF000 + index, 48, buffPtr + index);
			if (error)
			{
				mxlStatus = MXL_E_I2C_ERROR;
				goto exit;
			}

			index += 48;
			tmp_Len -= 48;
		}
		if (tmp_Len > 0)
		{
			error = IT9300_readRegisters(devHandlePtr->endeavour_mxl541, 0, 0xF000 + index, tmp_Len, buffPtr + index);
			if (error)
			{
				mxlStatus = MXL_E_I2C_ERROR;
				goto exit;
			}
		}
	}


exit:

#ifdef DEBUG_LOG
	fopen_s(&fp, "dumpMXL541.txt", "a");
	if (error)
		fprintf(fp, "Fail!!!!!\n");
	for (int i = 0; i < dataLen; i++)
		fprintf(fp, "rBuf[%d] = 0x%02X\n", i, buffPtr[i]);
	fprintf(fp, "Status : 0x%08x, Error code : 0x%08x\n", mxlStatus, error);
	fprintf(fp, "-------------------------------------------\n");
	fclose(fp);
#endif

	return mxlStatus;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_InitI2cAccessLock
 *
 * @param[in]   devId           Device ID
 *
 * @author Mahee
 *
 * @date 04/11/2013 Initial release
 *
 * This API will initilize locking mechanism used to serialize access for
 * I2C operations.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_InitI2cAccessLock(UINT8 devId)
{
#ifdef DEBUG_LOG
	fopen_s(&fp, "dumpMXL541.txt", "w");
	fclose(fp);
#endif

	return MXL_E_SUCCESS;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_DeleteI2cAccessLock
 *
 * @param[in]   devId           Device ID
 *
 * @author Mahee
 *
 * @date 04/11/2013 Initial release
 *
 * This API will release locking mechanism and associated resources.
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_DeleteI2cAccessLock(UINT8 devId)
{
	return MXL_E_SUCCESS;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_Lock
 *
 * @param[in]   devId           Device ID
 *
 * @author Mahee
 *
 * @date 04/11/2013 Initial release
 *
 * This API will should be used to lock access to device i2c access
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_Lock(UINT8 devId)
{
	return MXL_E_SUCCESS;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_Unlock
 *
 * @param[in]   devId           Device ID
 *
 * @author Mahee
 *
 * @date 04/11/2013 Initial release
 *
 * This API will should be used to unlock access to device i2c access
 *
 * @retval MXL_SUCCESS            - OK
 * @retval MXL_FAILURE            - Failure
 * @retval MXL_INVALID_PARAMETER  - Invalid parameter is passed
 *
 ************************************************************************/
MXL_STATUS_E MxLWare_HYDRA_OEM_Unlock(UINT8 devId)
{
	return MXL_E_SUCCESS;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_MemAlloc
 *
 * @param[in]   sizeInBytes
 *
 * @author Sateesh
 *
 * @date 04/23/2015 Initial release
 *
 * This API shall be used to allocate memory.
 *
 * @retval memPtr                 - Memory Pointer
 *
 ************************************************************************/
void* MxLWare_HYDRA_OEM_MemAlloc(UINT32 sizeInBytes)
{
	void *memPtr = NULL;

	memPtr = kmalloc(sizeInBytes * sizeof(UINT8),GFP_KERNEL);

	return memPtr;
}

/**
 ************************************************************************
 *
 * @brief MxLWare_HYDRA_OEM_MemFree
 *
 * @param[in]   memPtr
 *
 * @author Sateesh
 *
 * @date 04/23/2015 Initial release
 *
 * This API shall be used to free memory.
 *
 *
 ************************************************************************/
void MxLWare_HYDRA_OEM_MemFree(void *memPtr)
{
	//kfree(memPtr);
}
