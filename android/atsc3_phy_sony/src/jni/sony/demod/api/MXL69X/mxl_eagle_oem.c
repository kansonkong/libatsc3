/*****************************************************************************************   
 * FILE NAME          : mxl_eagle_oem.c
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 29/Sep/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains OEM functions definition
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
 *  \addtogroup OEM
 *  @{
 *
 ****************************************************************************************/

#include <linux/string.h>
#include "mxl_datatypes.h"
#include "mxlware_eagle_utils.h"
#include <linux/module.h>
#include <linux/kernel.h>

#include "IT9300.h"

//#define DEBUG_LOG

#ifdef DEBUG_LOG
FILE* fp;
#endif


//Endeavour* endeavour_mxl69x[4];
//unsigned char endeavour_mxl69x_i2cBus[4];
//unsigned char endeavour_mxl69x_i2cAdd[4];

/*****************************************************************************************
    Functions
 ****************************************************************************************/

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_Init
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 06/10/2016
*
* \brief          This function is the interface between Mxlware and OEM.
*                 It is called from MxLWare_EAGLE_API_CfgDrvInit before any I2C transaction is done.
*                 If I2C driver needs to be initialized, it can be done in this function. 
* 
* \param[in]      devId                 The MxL device ID
* \param[in]      pOemData              Pointer to customer-specific OEM data associated with this device ID
* 
* \returns        MXL_SUCCESS, MXL_INVALID_PARAMETER, MXL_FAILURE 
*-------------------------------------------------------------------------------------*/


MXL_STATUS_E MxL_EAGLE_Oem_Init(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, void *pOemData, unsigned char i2c_bus, unsigned char i2c_add)
{
	MXL_STATUS_E    status = MXL_E_SUCCESS;
	UINT8           slaveAddress;


	if (devId > 3)
		return MXL_E_FAILURE;


	pMxL_EAGLE_Device->endeavour_mxl69x = (Endeavour*)pOemData;
	pMxL_EAGLE_Device->endeavour_mxl69x_i2cBus = i2c_bus;
	pMxL_EAGLE_Device->endeavour_mxl69x_i2cAdd = i2c_add;

#ifdef DEBUG_LOG
	fopen_s(&fp, "dumpMXL69x.txt", "w");
	fclose(fp);
#endif


	return status;
}


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_UnInit
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 06/10/2016
*
* \brief          This function closes the interface between Mxlware and OEM.
*                 It is called from MxLWare_EAGLE_API_CfgDrvUnInit.
*                 If I2C driver needs to be closed, it can be done in this function.
* 
* \param[in]      devId                 The MxL device ID
* 
* \returns        MXL_SUCCESS, MXL_INVALID_PARAMETER, MXL_FAILURE 
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxL_EAGLE_Oem_UnInit(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device)
{
	MXL_STATUS_E status = MXL_E_SUCCESS;

	pMxL_EAGLE_Device->endeavour_mxl69x = NULL;

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_I2C_Write
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 06/10/2016
*
* \brief          This function is the interface between Mxlware and OEM for writing I2C data
* 
* \param[in]      devId                 The MxL device ID
* \param[in]      pBuffer               Pointer to buffer to write to I2C
* \param[in]      bufferLen             Length of buffer to write to I2C
* 
* \returns        MXL_SUCCESS, MXL_INVALID_PARAMETER, MXL_FAILURE 
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxL_EAGLE_Oem_I2C_Write(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT16 bufferLen)
{
	MXL_STATUS_E	status = MXL_E_SUCCESS;  
	Dword			error;
	Byte			buf[4];
	Byte			tmp_Len;
	Byte			index;
	int				i;
	Byte			tmp;


#ifdef DEBUG_LOG
	fopen_s(&fp, "dumpMXL69x.txt", "a");
	fprintf(fp, "I2C Add : 0x%02X\n", pMxL_EAGLE_Device->endeavour_mxl69x_i2cAdd);
	fprintf(fp, "wLen : %d\n", bufferLen);
	for (i = 0; i < bufferLen; i++)
		fprintf(fp, "wBuf[%d] = 0x%02X\n", i, pBuffer[i]);
	fclose(fp);
#endif

		//printk("dumpMXL69x.txt", "a");
	//printk("Write I2C Add : 0x%02X\n", pMxL_EAGLE_Device->endeavour_mxl69x_i2cAdd);
	//printk("wLen : %d\n", bufferLen);
	//for (i = 0; i < bufferLen; i++)
	//	printk("wBuf[%d] = 0x%02X\n", i, pBuffer[i]);
	if (bufferLen <= 48)
	{
		error = IT9300_writeGenericRegisters(pMxL_EAGLE_Device->endeavour_mxl69x, 0, pMxL_EAGLE_Device->endeavour_mxl69x_i2cBus, pMxL_EAGLE_Device->endeavour_mxl69x_i2cAdd, (Byte)bufferLen, pBuffer);
		if (error)
		{
			status = MXL_E_I2C_ERROR;
			goto exit;
		}
	}
	else
	{
		index = 0;
		tmp_Len = (Byte)bufferLen;
		while (tmp_Len >= 48)
		{
			error = IT9300_writeRegisters(pMxL_EAGLE_Device->endeavour_mxl69x, 0, 0xF001 + index, 48, pBuffer + index);
			if (error)
			{
				status = MXL_E_I2C_ERROR;
				goto exit;
			}

			index += 48;
			tmp_Len -= 48;
		}
		if (tmp_Len > 0)
		{
			error = IT9300_writeRegisters(pMxL_EAGLE_Device->endeavour_mxl69x, 0, 0xF001 + index, tmp_Len, pBuffer + index);
			if (error)
			{
				status = MXL_E_I2C_ERROR;
				goto exit;
			}
		}

		buf[0] = 0xF4;
		buf[1] = pMxL_EAGLE_Device->endeavour_mxl69x_i2cBus;
		buf[2] = pMxL_EAGLE_Device->endeavour_mxl69x_i2cAdd;
		buf[3] = (Byte)bufferLen;
		error = IT9300_writeRegisters(pMxL_EAGLE_Device->endeavour_mxl69x, 0, 0x4900, 4, buf);
		if (error)
		{
			status = MXL_E_I2C_ERROR;
			goto exit;
		}
		error = IT9300_readRegister(pMxL_EAGLE_Device->endeavour_mxl69x, 0, OVA_EXTI2C_STATUS, &tmp);
		if (error)
		{
			status = MXL_E_I2C_ERROR;
			goto exit;
		}
		if (tmp != 0)
		{
			status = MXL_E_I2C_ERROR;
			goto exit;
		}
	}

	
exit :

#ifdef DEBUG_LOG
	fopen_s(&fp, "dumpMXL69x.txt", "a");
	fprintf(fp, "Status : 0x%08x, Error code : 0x%08x\n", status, error);
	fprintf(fp, "-------------------------------------------\n");
	fclose(fp);
#endif

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_I2C_Read
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 06/10/2016
*
* \brief          This function is the interface between Mxlware and OEM for reading I2C data
* 
* \param[in]      devId                 The MxL device ID
* \param[out]     pBuffer               Pointer to buffer to read from I2C
* \param[in]      bufferLen             Length of buffer to read from I2C
* 
* \returns        MXL_SUCCESS, MXL_INVALID_PARAMETER, MXL_FAILURE 
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxL_EAGLE_Oem_I2C_Read(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT16 bufferLen)
{
	MXL_STATUS_E	status = MXL_E_SUCCESS;  
	Dword			error;
	Byte			buf[4];
	Byte			tmp_Len;
	Byte			index;
	int				i;


#ifdef DEBUG_LOG
	fopen_s(&fp, "dumpMXL69x.txt", "a");
	fprintf(fp, "I2C Add : 0x%02X\n", pMxL_EAGLE_Device->endeavour_mxl69x_i2cAdd | 1);
	fprintf(fp, "rLen : %d\n", bufferLen);
	fclose(fp);
#endif
	//printk("dumpMXL69x.txt", "a");
	//printk("Read I2C Add : 0x%02X\n", pMxL_EAGLE_Device->endeavour_mxl69x_i2cAdd | 1);
	//printk("rLen : %d\n", bufferLen);
	if (bufferLen <= 48)
	{
		//printk("mmmmmm\n");
		error = IT9300_readGenericRegisters(pMxL_EAGLE_Device->endeavour_mxl69x, 0, pMxL_EAGLE_Device->endeavour_mxl69x_i2cBus, pMxL_EAGLE_Device->endeavour_mxl69x_i2cAdd, (Byte)bufferLen, pBuffer);
		if (error)
		{
			status = MXL_E_I2C_ERROR;
			goto exit;
		}
	}
	else
	{
			//printk("ppppppp\n");
		buf[0] = 0xF5;
		buf[1] = pMxL_EAGLE_Device->endeavour_mxl69x_i2cBus;
		buf[2] = pMxL_EAGLE_Device->endeavour_mxl69x_i2cAdd;
		buf[3] = (Byte)bufferLen;
		error = IT9300_writeRegisters(pMxL_EAGLE_Device->endeavour_mxl69x, 0, 0x4900, 4, buf);
		if (error)
		{
			status = MXL_E_I2C_ERROR;
			goto exit;
		}
		error = IT9300_readRegister(pMxL_EAGLE_Device->endeavour_mxl69x, 0, OVA_EXTI2C_STATUS, &buf[0]);
		if (error)
		{
			status = MXL_E_I2C_ERROR;
			goto exit;
		}
		if (buf[0] != 0)
		{
			status = MXL_E_I2C_ERROR;
			goto exit;
		}


		index = 0;
		tmp_Len = (Byte)bufferLen;
		while (tmp_Len >= 48)
		{
			error = IT9300_readRegisters(pMxL_EAGLE_Device->endeavour_mxl69x, 0, 0xF000 + index, 48, pBuffer + index);
			if (error)
			{
				status = MXL_E_I2C_ERROR;
				goto exit;
			}

			index += 48;
			tmp_Len -= 48;
		}
		if (tmp_Len > 0)
		{
			error = IT9300_readRegisters(pMxL_EAGLE_Device->endeavour_mxl69x, 0, 0xF000 + index, tmp_Len, pBuffer + index);
			if (error)
			{
				status = MXL_E_I2C_ERROR;
				goto exit;
			}
		}
	}


exit:
	//for (i = 0; i < bufferLen; i++)
	//	printk("rBuf[%d] = 0x%02X\n", i, pBuffer[i]);
	//printk("Status : 0x%08x, Error code : 0x%08x\n", status, error);
	//printk("-------------------------------------------\n");

#ifdef DEBUG_LOG
	fopen_s(&fp, "dumpMXL69x.txt", "a");
	for (i = 0; i < bufferLen; i++)
		fprintf(fp, "rBuf[%d] = 0x%02X\n", i, pBuffer[i]);
	fprintf(fp, "Status : 0x%08x, Error code : 0x%08x\n", status, error);
	fprintf(fp, "-------------------------------------------\n");
	fclose(fp);
#endif

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_I2C_Read
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 06/10/2016
*
* \brief          This function resets the Eagle chip, usually via toggling a GPIO
* 
* \param[in]      devId                 The MxL device ID
* 
* \returns        MXL_SUCCESS, MXL_INVALID_PARAMETER, MXL_FAILURE 
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxL_EAGLE_Oem_DevReset(UINT8 devId)
{
	MXL_STATUS_E status = MXL_E_SUCCESS;


	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_SleepInMs
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 06/10/2016
*
* \brief          This OEM function provides a sleep in milliseconds
* 
* \param[in]      mSec                  Sleep time
* 
* \returns        void 
*-------------------------------------------------------------------------------------*/
void MxL_EAGLE_Oem_SleepInMs(UINT16 mSec)
{  
	BrUser_delay(NULL, mSec);
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_MemSet
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 06/10/2016
*
* \brief          This OEM function sets the buffer with a specific value, wrapper for memset()
* 
* \param[out]     pBuffer                  Pointer of buffer to set
* \param[in]      value                    Value to write to buffer
* \param[in]      length                   Length of buffer to set
* 
* \returns        MXL_SUCCESS, MXL_INVALID_PARAMETER
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxL_EAGLE_Oem_MemSet(UINT8 *pBuffer, UINT8 value, UINT32 length)
{
	MXL_STATUS_E status = MXL_E_SUCCESS;


	memset(pBuffer, value, length);


	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_MemCpy
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 06/10/2016
*
* \brief          This OEM function copies the data from source to destination, wrapper for memcpy()
* 
* \param[out]     pDest                    Pointer of destination buffer
* \param[in]      pSrc                     Pointer of source buffer
* \param[in]      length                   Length of buffer to copy
* 
* \returns        MXL_SUCCESS, MXL_INVALID_PARAMETER
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxL_EAGLE_Oem_MemCpy(UINT8 *pDest, UINT8 *pSrc, UINT32 length)
{
	MXL_STATUS_E status = MXL_E_SUCCESS;


	memcpy(pDest, pSrc, length);


	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_MutexGet
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 06/10/2016
*
* \brief          This OEM function gets a per-device Mutex.
*                 If the Mutex needs creation, creates it as taken.
*                 If the application isn't multithreaded, function can be left empty
* 
* \param[in]      devId                 The MxL device ID
* 
* \returns        void
*-------------------------------------------------------------------------------------*/
void MxL_EAGLE_Oem_MutexGet(UINT8 devId)
{
  
}  

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_MutexRelease
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 06/10/2016
*
* \brief          This OEM function releases a per-device Mutex.
*                 If the application isn't multithreaded, function can be left empty
* 
* \param[in]      devId                 The MxL device ID
* 
* \returns        void
*-------------------------------------------------------------------------------------*/
void MxL_EAGLE_Oem_MutexRelease(UINT8 devId)
{
  
}
