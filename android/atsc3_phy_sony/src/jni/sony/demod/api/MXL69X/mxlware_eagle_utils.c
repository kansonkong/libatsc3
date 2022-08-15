/*****************************************************************************************   
 * FILE NAME          : mxlware_eagle_utils.c
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 29/09/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains MxLWare helper functions
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
#include <linux/string.h>
#include "mxl_datatypes.h"
#include "mxlware_eagle_utils.h"
#include "mxlware_eagle_endian.h"
#include "mxl_eagle_oem_debug.h"
#include "mxl_eagle_oem.h"


/*****************************************************************************************
    Global Variables
 ****************************************************************************************/
//MXL_EAGLE_DEV_CONTEXT_T       MxL_EAGLE_Devices[MXL_EAGLE_MAX_NUM_DEVICES];
MXL_EAGLE_ENDIANNESS_TYPE_E   gEndiannessType;


/*****************************************************************************************
    Functions
 ****************************************************************************************/
static inline void MxL_EAGLE_UTILS_Swap(UINT8 *a, UINT8 *b)
{
  UINT8 temp;

  temp = *a;
  *a = *b;
  *b = temp;
}


UINT8 MxL_EAGLE_UTILS_PushUint8(UINT8 u8Value, UINT8** ptr)
{
	UINT8 *pu8Buf = *ptr;

	*pu8Buf = u8Value;
	(*ptr)++;

	return 0;
}

UINT8 MxL_EAGLE_UTILS_PushUint16(UINT16 u16Value, UINT8** ptr)
{
	UINT8 *pu8Buf = *ptr;

	if ((gEndiannessType != MXL_EAGLE_LITTLE_ENDIAN) && (gEndiannessType != MXL_EAGLE_BIG_ENDIAN))
	{
		MxLWare_EAGLE_UTILS_EndiannessDetect();
	}

	if (gEndiannessType == MXL_EAGLE_LITTLE_ENDIAN)
	{
		*(pu8Buf++) = GETBYTE(u16Value, 1);
		*(pu8Buf++) = GETBYTE(u16Value, 2);
	}
	else if (gEndiannessType == MXL_EAGLE_BIG_ENDIAN)
	{
		*(pu8Buf++) = GETBYTE(u16Value, 2);
		*(pu8Buf++) = GETBYTE(u16Value, 1);
	}

	(*ptr) += 2;

	return 0;
}

UINT8 MxL_EAGLE_UTILS_PushUint32(UINT32 u32Value, UINT8** ptr)
{
	UINT8 *pu8Buf = *ptr;

	if ((gEndiannessType != MXL_EAGLE_LITTLE_ENDIAN) && (gEndiannessType != MXL_EAGLE_BIG_ENDIAN))
	{
		MxLWare_EAGLE_UTILS_EndiannessDetect();
	}

	if (gEndiannessType == MXL_EAGLE_LITTLE_ENDIAN)
	{
		*(pu8Buf++) = GETBYTE(u32Value, 1);
		*(pu8Buf++) = GETBYTE(u32Value, 2);
		*(pu8Buf++) = GETBYTE(u32Value, 3);
		*(pu8Buf++) = GETBYTE(u32Value, 4);
	}
	else if (gEndiannessType == MXL_EAGLE_BIG_ENDIAN)
	{
		*(pu8Buf++) = GETBYTE(u32Value, 4);
		*(pu8Buf++) = GETBYTE(u32Value, 3);
		*(pu8Buf++) = GETBYTE(u32Value, 2);
		*(pu8Buf++) = GETBYTE(u32Value, 1);
	}

	(*ptr) += 4;

	return 0;
}

UINT8 MxL_EAGLE_UTILS_PopUint8(UINT8** ptr)
{
	UINT8 *pu8Buf = *ptr;
	UINT8 u8Rtn = *pu8Buf;

	(*ptr)++;

	return u8Rtn;
}

UINT16 MxL_EAGLE_UTILS_PopUint16(UINT8** ptr)
{
	UINT8 *pu8Buf = *ptr;
	UINT16 u16Rtn = 0;

	if ((gEndiannessType != MXL_EAGLE_LITTLE_ENDIAN) && (gEndiannessType != MXL_EAGLE_BIG_ENDIAN))
	{
		MxLWare_EAGLE_UTILS_EndiannessDetect();
	}

	if (gEndiannessType == MXL_EAGLE_LITTLE_ENDIAN)
	{
		u16Rtn += *(pu8Buf++);
		u16Rtn += ((*(pu8Buf++)) << 8);
	}
	else if (gEndiannessType == MXL_EAGLE_BIG_ENDIAN)
	{
		u16Rtn += ((*(pu8Buf++)) << 8);
		u16Rtn += *(pu8Buf++);
	}

	(*ptr) += 2;

	return u16Rtn;
}

UINT32 MxL_EAGLE_UTILS_PopUint32(UINT8** ptr)
{
	UINT8 *pu8 = *ptr;
	UINT32 u32Rtn = 0;

	if ((gEndiannessType != MXL_EAGLE_LITTLE_ENDIAN) && (gEndiannessType != MXL_EAGLE_BIG_ENDIAN))
	{
		MxLWare_EAGLE_UTILS_EndiannessDetect();
	}

	if (gEndiannessType == MXL_EAGLE_LITTLE_ENDIAN)
	{
		u32Rtn += *(pu8++);
		u32Rtn += ((*(pu8++)) << 8);
		u32Rtn += ((*(pu8++)) << 16);
		u32Rtn += ((*(pu8++)) << 24);
	}
	else if (gEndiannessType == MXL_EAGLE_BIG_ENDIAN)
	{
		u32Rtn += ((*(pu8++)) << 24);
		u32Rtn += ((*(pu8++)) << 16);
		u32Rtn += ((*(pu8++)) << 8);
		u32Rtn += *(pu8++);
	}

	(*ptr) += 4;

	return u32Rtn;
}

SINT16 MxL_EAGLE_UTILS_PopSint16(UINT8** ptr)
{
	UINT8 *pu8Buf = *ptr;
	SINT16 s16Rtn = 0;

	if ((gEndiannessType != MXL_EAGLE_LITTLE_ENDIAN) && (gEndiannessType != MXL_EAGLE_BIG_ENDIAN))
	{
		MxLWare_EAGLE_UTILS_EndiannessDetect();
	}

	if (gEndiannessType == MXL_EAGLE_LITTLE_ENDIAN)
	{
		s16Rtn += *(pu8Buf++);
		s16Rtn += ((*(pu8Buf++)) << 8);
	}
	else if (gEndiannessType == MXL_EAGLE_BIG_ENDIAN)
	{
		s16Rtn += ((*(pu8Buf++)) << 8);
		s16Rtn += *(pu8Buf++);
	}

	(*ptr) += 2;

	return s16Rtn;
}

SINT32 MxL_EAGLE_UTILS_PopSint32(UINT8** ptr)
{
	UINT8 *pu8Buf = *ptr;
	SINT32 s32Rtn = 0;

	if ((gEndiannessType != MXL_EAGLE_LITTLE_ENDIAN) && (gEndiannessType != MXL_EAGLE_BIG_ENDIAN))
	{
		MxLWare_EAGLE_UTILS_EndiannessDetect();
	}

	if (gEndiannessType == MXL_EAGLE_LITTLE_ENDIAN)
	{
		s32Rtn += *(pu8Buf++);
		s32Rtn += ((*(pu8Buf++)) << 8);
		s32Rtn += ((*(pu8Buf++)) << 16);
		s32Rtn += ((*(pu8Buf++)) << 24);
	}
	else if (gEndiannessType == MXL_EAGLE_BIG_ENDIAN)
	{
		s32Rtn += ((*(pu8Buf++)) << 24);
		s32Rtn += ((*(pu8Buf++)) << 16);
		s32Rtn += ((*(pu8Buf++)) << 8);
		s32Rtn += *(pu8Buf++);
	}

	(*ptr) += 4;

	return s32Rtn;
}
/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Ctrl_GetDeviceContext
*
* AUTHOR        : Srikrishna S
* 
* DATE CREATED  : 11/04/2015
*
* \brief          This API is used for obtaining the device context
* 
* \param[in]      devId                 The MxL device id
*
* \returns        pointer to the device context 
*-------------------------------------------------------------------------------------*/
/*
MXL_EAGLE_DEV_CONTEXT_T* MxL_EAGLE_Ctrl_GetDeviceContext(UINT8 devId)
{
  MXL_EAGLE_DEV_CONTEXT_T *pDevice = NULL;

  MXLENTERSTR

  if (devId < MXL_EAGLE_MAX_NUM_DEVICES)
  {
    pDevice = &MxL_EAGLE_Devices[devId];
  }
  
  MXLEXITSTR((pDevice == NULL) ? 0 : (UINT32*)pDevice)

  return pDevice;
}
*/
/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_UTILS_EndiannessDetect
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 05/10/2016
*
* \brief          This function is used for detecting the host endianness
* 
* \returns        None 
*-------------------------------------------------------------------------------------*/
void MxLWare_EAGLE_UTILS_EndiannessDetect(void)
{
  UINT32  temp = 1;
  UINT8   *pTemp = (UINT8*)&temp;

  gEndiannessType = (*pTemp == 0) ? MXL_EAGLE_BIG_ENDIAN : MXL_EAGLE_LITTLE_ENDIAN;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_UTILS_EndiannessConversion
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 05/10/2016
*
* \brief          This function is used for converting the buffer endianness
* 
* \param[in]      pBuffer                 Pointer to buffer to convert
* \param[in]      size                    Length of buffer to convert (2 or 4 bytes)
* 
* \returns        The converted data 
*-------------------------------------------------------------------------------------*/
void MxL_EAGLE_UTILS_EndiannessMemCpy(UINT8 *pDestBuffer, UINT8 *pSourceBuffer, UINT8 size, MXL_EAGLE_ENDIANNESS_TYPE_E endiannessType)
{
  MXLENTERSTR

  //sanity check
  if ( (pDestBuffer == NULL) || (pSourceBuffer == NULL) )
  {
    return;
  }


  if ( (endiannessType == MXL_EAGLE_ANY_ENDIAN) || (gEndiannessType == endiannessType) )  //convert endianness
  {
    if ((UINT32)pDestBuffer == (UINT32)pSourceBuffer) //convert endianness on same buffer
    {
      if (size == sizeof(UINT32))
      {
        MxL_EAGLE_UTILS_Swap(&pDestBuffer[3], &pSourceBuffer[0]);
        MxL_EAGLE_UTILS_Swap(&pDestBuffer[2], &pSourceBuffer[1]);
      }
      else if (size == sizeof(UINT16))
      {
        MxL_EAGLE_UTILS_Swap(&pDestBuffer[1], &pSourceBuffer[0]);
      }
    }
    else  //convert endianness of source into destination
    {
      if (size == sizeof(UINT32))
      {
        pDestBuffer[3] = pSourceBuffer[0];
        pDestBuffer[2] = pSourceBuffer[1];
        pDestBuffer[1] = pSourceBuffer[2];
        pDestBuffer[0] = pSourceBuffer[3];
      }
      else if (size == sizeof(UINT16))
      {
        pDestBuffer[1] = pSourceBuffer[0];
        pDestBuffer[0] = pSourceBuffer[1];
      }
    }
  }
  else  //memcpy from source to destination
  {
    if ((UINT32)pDestBuffer != (UINT32)pSourceBuffer)
    {
      if ( (size == sizeof(UINT32)) || (size == sizeof(UINT16)) )
      {
        MxL_EAGLE_Oem_MemCpy(pDestBuffer, pSourceBuffer, size);
      }
    }
  }

  MXLEXITSTR(MXL_E_SUCCESS)
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_UTILS_CheckSumCalc
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 23/11/2016
*
* \brief          This function calculates the checksum of the buffer
* 
* \param[in]      pBuffer                 Pointer to buffer to calculate checksum on
* \param[in]      size                    Length of buffer
* 
* \returns        Checksum result
*-------------------------------------------------------------------------------------*/
UINT32 MxLWare_EAGLE_UTILS_CheckSumCalc(UINT8 *pBuffer, UINT32 size)
{
  UINT32  ix, temp = 0, remainder = 0, currentChecksum = 0;


  // calculate on whole number of DWORDs
  for (ix = 0; ix < size / 4; ix++)
  {
    MxL_EAGLE_UTILS_EndiannessMemCpy((UINT8*)&temp , (pBuffer + (ix * sizeof(UINT32))), sizeof(UINT32), MXL_EAGLE_LITTLE_ENDIAN);
    currentChecksum += temp;
  }

  // calculate on whatever left
  remainder = size % 4;
  if (remainder > 0)
  {
    temp = 0;
    MxL_EAGLE_Oem_MemCpy((UINT8*)&temp, &pBuffer[size - remainder], remainder);

    MxL_EAGLE_UTILS_EndiannessMemCpy((UINT8*)&temp, (UINT8*)&temp, sizeof(UINT32), MXL_EAGLE_LITTLE_ENDIAN);
    currentChecksum += temp;
  }

  currentChecksum ^= 0xDEADBEEF;
  MxL_EAGLE_UTILS_EndiannessMemCpy((UINT8*)&currentChecksum, (UINT8*)&currentChecksum, sizeof(UINT32), MXL_EAGLE_LITTLE_ENDIAN);

  return(currentChecksum);
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_UTILS_ValidateFwHeader
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 05/10/2016
*
* \brief          This function validates the FW header
* 
* \param[in]      buffer                Pointer to buffer containing FW
* \param[in]      bufferLen             Length of buffer
* 
* \returns        MXL_SUCCESS, MXL_DATA_ERROR
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxLWare_EAGLE_UTILS_ValidateFwHeader(UINT8 *buffer, UINT32 bufferLen)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;
  UINT32        ix, temp = 0;

  MXLENTERSTR

  do 
  {
    if ( (buffer[0] != 0x4D) || (buffer[1] != 0x31) || (buffer[2] != 0x10 ) || (buffer[3] != 0x02) || 
         (buffer[4] != 0x40) || (buffer[5] != 0x00) || (buffer[6] != 0x00)  || (buffer[7] != 0x80) )
    {
      status = MXL_E_DATA_ERROR;
      break;
    }

    MxL_EAGLE_UTILS_EndiannessMemCpy((UINT8*)&temp, (buffer + 8), sizeof(UINT32), MXL_EAGLE_LITTLE_ENDIAN);

    if ((bufferLen-16) != (temp >> 8))
    {
      status = MXL_E_DATA_ERROR;
      break;
    }

    temp = 0;
    for (ix = 16; ix < bufferLen; ix++)
    {
      temp += buffer[ix];
    }
    if ((UINT8)temp != buffer[11])
    {
      status = MXL_E_DATA_ERROR;
    }
  } while(0);

  MXLEXITSTR(status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_UTILS_DownloadFwSegment
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 05/10/2016
*
* \brief          This function downloads the FW IRAM/DRAM segment
* 
* \param[in]      devId                 The MxL device id
* \param[in]      pBuffer               Pointer to buffer containing FW segment
* \param[in,out]  index                 Current index into the buffer
* \param[in]      pCallbackFn           Pointer to callback function, called every chunk
* 
* \returns        MXL_SUCCESS, MXL_DATA_ERROR, MXL_NOT_INITIALIZED, MXL_FAILURE
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxLWare_EAGLE_UTILS_DownloadFwSegment(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT32 bufferLen, UINT32 *index, MXL_CALLBACK_FN_T pCallbackFn)
{
	MXL_STATUS_E                        status = MXL_E_SUCCESS;
	UINT32                              ix = 0, jx = 0, totalLen = 0, addr = 0, chunkLen = 0, prevChunkLen = 0, temp;
	UINT8                               localBuf[MXL_EAGLE_MAX_I2C_PACKET_SIZE] = { 0 }, *pLocalBuf = NULL;
	MXL_EAGLE_FW_DOWNLOAD_CB_PAYLOAD_T  callbackStruct;

	MXLENTERSTR

		ix = *index;

	if (pBuffer[ix] == 0x53)
	{
		totalLen = pBuffer[ix + 1] << 16 | pBuffer[ix + 2] << 8 | pBuffer[ix + 3];
		totalLen = (totalLen + 3) & ~3;
		addr = pBuffer[ix + 4] << 24 | pBuffer[ix + 5] << 16 | pBuffer[ix + 6] << 8 | pBuffer[ix + 7];
		ix += MXL_EAGLE_FW_SEGMENT_HEADER_SIZE;

		callbackStruct.totalLen = bufferLen;

		while ((totalLen > 0) && (status == MXL_E_SUCCESS))
		{
			for (jx = 0; jx<MXL_EAGLE_MAX_I2C_PACKET_SIZE; jx++) //clear buffer
			{
				localBuf[jx] = 0;
			}
			pLocalBuf = localBuf;
			chunkLen = (totalLen < (MXL_EAGLE_MAX_I2C_PACKET_SIZE - MXL_EAGLE_I2C_MHEADER_SIZE)) ? totalLen : (MXL_EAGLE_MAX_I2C_PACKET_SIZE - MXL_EAGLE_I2C_MHEADER_SIZE);

			*pLocalBuf++ = 0xFC;
			*pLocalBuf++ = chunkLen + sizeof(UINT32);

			temp = addr + prevChunkLen;
			MxL_EAGLE_UTILS_EndiannessMemCpy(pLocalBuf, (UINT8*)&temp, sizeof(UINT32), MXL_EAGLE_BIG_ENDIAN);
			pLocalBuf += sizeof(UINT32);

			for (jx = 0; jx<chunkLen; jx += 4)
			{
				switch (bufferLen - ix - jx)
				{
				case 1:
					temp = pBuffer[ix + jx] << 24;
					break;

				case 2:
					temp = pBuffer[ix + jx] << 24 | pBuffer[ix + jx + 1] << 16;
					break;

				case 3:
					temp = pBuffer[ix + jx] << 24 | pBuffer[ix + jx + 1] << 16 | pBuffer[ix + jx + 2] << 8;
					break;

				default:
					temp = pBuffer[ix + jx] << 24 | pBuffer[ix + jx + 1] << 16 | pBuffer[ix + jx + 2] << 8 | pBuffer[ix + jx + 3];
					break;
				}

				MxL_EAGLE_UTILS_EndiannessMemCpy(pLocalBuf, (UINT8*)&temp, sizeof(UINT32), MXL_EAGLE_BIG_ENDIAN);
				pLocalBuf += sizeof(UINT32);
			}

			status = MxL_EAGLE_Oem_I2C_Write(devId, pMxL_EAGLE_Device, localBuf, (chunkLen + MXL_EAGLE_I2C_MHEADER_SIZE));

			prevChunkLen += chunkLen;
			totalLen -= chunkLen;
			ix += chunkLen;

			callbackStruct.downloadedLen = (ix > bufferLen ? bufferLen : ix);
			if (pCallbackFn != NULL)
			{
				pCallbackFn(devId, MXL_EAGLE_CB_FW_DOWNLOAD, &callbackStruct);
			}
		}

		*index = ix;
	}
	else
	{
		status = MXL_E_DATA_ERROR;
	}

	MXLEXITSTR(status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_UTILS_EnhanceI2cMemWrite
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 31/10/2016
*
* \brief          This function perform an enhanced I2C memory write transaction
* 
* \param[in]      devId                 The MxL device id
* \param[in]      addr                  Memory address to write to
* \param[in]      buffer                Pointer to buffer to write to I2C
* \param[in]      size                  size of buffer, size must be aligned to 4 bytes
* 
* \returns        MXL_SUCCESS, MXL_DATA_ERROR, MXL_NOT_INITIALIZED, MXL_FAILURE
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxLWare_EAGLE_UTILS_EnhanceI2cMemWrite(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT32 addr, UINT8 *pBuffer, UINT32 size)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;
  UINT32        ix = 0, jx = 0, totalLen = 0, chunkLen = 0, temp;
  UINT8         localBuf[MXL_EAGLE_MAX_I2C_PACKET_SIZE] = {0}, *pLocalBuf = NULL;

  MXLENTERSTR

  ix = 0;
  totalLen = size;
  totalLen = (totalLen + 3) & ~3;  //4 byte alignment

  while ( (totalLen > 0) && (status == MXL_E_SUCCESS) )
  {
    for (jx=0; jx<MXL_EAGLE_MAX_I2C_PACKET_SIZE; jx++) //clear buffer
    {
      localBuf[jx] = 0;
    }
    pLocalBuf = localBuf;
    chunkLen  = (totalLen < MXL_EAGLE_MAX_I2C_PACKET_SIZE - MXL_EAGLE_I2C_MHEADER_SIZE) ? totalLen : (MXL_EAGLE_MAX_I2C_PACKET_SIZE - MXL_EAGLE_I2C_MHEADER_SIZE);

    *pLocalBuf++ = 0xFC;
    *pLocalBuf++ = chunkLen + sizeof(UINT32);

    temp = addr + ix;
    MxL_EAGLE_UTILS_EndiannessMemCpy(pLocalBuf, (UINT8*)&temp, sizeof(UINT32), MXL_EAGLE_BIG_ENDIAN);
    pLocalBuf += sizeof(UINT32);

    for (jx=0; jx<chunkLen; jx+=4)
    {
      MxL_EAGLE_UTILS_EndiannessMemCpy(pLocalBuf, pBuffer + ix + jx, sizeof(UINT32), MXL_EAGLE_BIG_ENDIAN);
      pLocalBuf += sizeof(UINT32);
    }

	status = MxL_EAGLE_Oem_I2C_Write(devId, pMxL_EAGLE_Device, localBuf, (chunkLen + MXL_EAGLE_I2C_MHEADER_SIZE));

    totalLen -= chunkLen;
    ix += chunkLen;
  }

  MXLEXITSTR(status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_UTILS_EnhanceI2cMemRead
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 31/10/2016
*
* \brief          This function perform an enhanced I2C memory read transaction
* 
* \param[in]      devId                 The MxL device id
* \param[in]      addr                  Memory address to read from
* \param[out]     buffer                Pointer to buffer to read from I2C
* \param[in]      size                  size of buffer, size must be aligned to 4 bytes
* 
* \returns        MXL_SUCCESS, MXL_DATA_ERROR, MXL_NOT_INITIALIZED, MXL_FAILURE
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxLWare_EAGLE_UTILS_EnhanceI2cMemRead(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT32 addr, UINT8 *pBuffer, UINT32 size)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;
  UINT32        ix = 0;
  UINT8         localBuf[MXL_EAGLE_I2C_MHEADER_SIZE] = {0}, *pLocalBuf = NULL;

  MXLENTERSTR

  for (ix=0; ix<MXL_EAGLE_I2C_MHEADER_SIZE; ix++)
  {
    localBuf[ix] = 0;
  }
  pLocalBuf = localBuf;

  *pLocalBuf++ = 0xFB;
  *pLocalBuf++ = sizeof(UINT32);
  MxL_EAGLE_UTILS_EndiannessMemCpy(pLocalBuf, (UINT8*)&addr, sizeof(UINT32), MXL_EAGLE_BIG_ENDIAN);

  status = MxL_EAGLE_Oem_I2C_Write(devId, pMxL_EAGLE_Device, localBuf, MXL_EAGLE_I2C_MHEADER_SIZE);
  if (status == MXL_E_SUCCESS)
  {
    size = (size + 3) & ~3;  //4 byte alignment

	status = MxL_EAGLE_Oem_I2C_Read(devId, pMxL_EAGLE_Device, pBuffer, size);

    pLocalBuf = pBuffer;
    for (ix=0; ix<size; ix+=4)
    {
      MxL_EAGLE_UTILS_EndiannessMemCpy(pLocalBuf, pLocalBuf, sizeof(UINT32), MXL_EAGLE_BIG_ENDIAN);
      pLocalBuf += sizeof(UINT32);
    }
  }

  MXLEXITSTR(status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_UTILS_EnhanceI2cProtWrite
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 31/10/2016
*
* \brief          This function perform an enhanced I2C protocol write transaction
* 
* \param[in]      devId                 The MxL device id
* \param[in]      buffer                Pointer to buffer to write to I2C
* \param[in]      size                  size of buffer, size must be aligned to 4 bytes
* 
* \returns        MXL_SUCCESS, MXL_DATA_ERROR, MXL_NOT_INITIALIZED, MXL_FAILURE
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxLWare_EAGLE_UTILS_EnhanceI2cProtWrite(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT32 size)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;
  UINT32        ix = 0, jx = 0, totalLen = 0, chunkLen = 0;
  UINT8         localBuf[MXL_EAGLE_MAX_I2C_PACKET_SIZE] = {0}, *pLocalBuf = NULL;

  MXLENTERSTR

  ix = 0;
  totalLen = size;
  totalLen = (totalLen + 3) & ~3;  //4 byte alignment

  while ( (totalLen > 0) && (status == MXL_E_SUCCESS) )
  {
    for (jx=0; jx<MXL_EAGLE_MAX_I2C_PACKET_SIZE; jx++) //clear buffer
    {
      localBuf[jx] = 0;
    }
    pLocalBuf = localBuf;
    chunkLen  = (totalLen < (MXL_EAGLE_MAX_I2C_PACKET_SIZE - MXL_EAGLE_I2C_PHEADER_SIZE)) ? totalLen : (MXL_EAGLE_MAX_I2C_PACKET_SIZE - MXL_EAGLE_I2C_PHEADER_SIZE);

    *pLocalBuf++ = 0xFE;
    *pLocalBuf++ = (UINT8)chunkLen;

    for (jx=0; jx<chunkLen; jx+=4)
    {
      MxL_EAGLE_UTILS_EndiannessMemCpy(pLocalBuf, pBuffer + ix + jx, sizeof(UINT32), MXL_EAGLE_ANY_ENDIAN);
      pLocalBuf += sizeof(UINT32);
    }

	status = MxL_EAGLE_Oem_I2C_Write(devId, pMxL_EAGLE_Device, localBuf, (chunkLen + MXL_EAGLE_I2C_PHEADER_SIZE));

    totalLen -= chunkLen;
    ix += chunkLen;
  }

  MXLEXITSTR(status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_UTILS_EnhanceI2cProtRead
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 31/10/2016
*
* \brief          This function perform an enhanced I2C protocol read transaction
* 
* \param[in]      devId                 The MxL device id
* \param[out]     buffer                Pointer to buffer to read from I2C
* \param[in]      size                  size of buffer, size must be aligned to 4 bytes
* 
* \returns        MXL_SUCCESS, MXL_DATA_ERROR, MXL_NOT_INITIALIZED, MXL_FAILURE
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxLWare_EAGLE_UTILS_EnhanceI2cProtRead(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT32 size)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;
  UINT32        ix = 0;
  UINT8         localBuf[MXL_EAGLE_I2C_PHEADER_SIZE] = {0}, *pLocalBuf = NULL;

  MXLENTERSTR

  for (ix=0; ix<MXL_EAGLE_I2C_PHEADER_SIZE; ix++)
  {
    localBuf[ix] = 0;
  }
  localBuf[0] = 0xFD;
  localBuf[1] = 0;

  status = MxL_EAGLE_Oem_I2C_Write(devId, pMxL_EAGLE_Device, localBuf, MXL_EAGLE_I2C_PHEADER_SIZE);
  if (status == MXL_E_SUCCESS)
  {
    size = (size + 3) & ~3;  //4 byte alignment
	//printk("size = %d\n",size);
    //read in 4 byte chunks
    for (ix=0; ix<size; ix+=4)
    {
		status = MxL_EAGLE_Oem_I2C_Read(devId, pMxL_EAGLE_Device, pBuffer + ix, 4);
      if (status != MXL_E_SUCCESS) break;
    }

    pLocalBuf = pBuffer;
    for (ix=0; ix<size; ix+=4)
    {
      MxL_EAGLE_UTILS_EndiannessMemCpy(pLocalBuf, pLocalBuf, sizeof(UINT32), MXL_EAGLE_ANY_ENDIAN);
      pLocalBuf += sizeof(UINT32);
    }
  }

  MXLEXITSTR(status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_UTILS_SendAndReceive
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 20/11/2016
*
* \brief          This function sends and receives APIs
* 
* \param[in]      devId                 The MxL device id
* \param[in]      opcode                Protocol op-code, see MXL_EAGLE_OPCODE_E
* \param[in]      pTxPayload            Pointer to buffer to write
* \param[in]      txPayloadSize         size of TX payload buffer
* \param[out]     pRxPayload            Pointer to buffer to read
* \param[out]     rxPayloadSize         size of RX payload
* 
* \returns        MXL_SUCCESS, MXL_DATA_ERROR, MXL_NOT_INITIALIZED, MXL_FAILURE
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxLWare_EAGLE_UTILS_SendAndReceive(UINT8 devId,
												MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device,
                                                UINT8 opcode,
                                                UINT8 *pTxPayload,
                                                UINT8 txPayloadSize,
                                                UINT8 *pRxPayload,
                                                UINT8 rxExpectedPayloadSize)
{
  MXL_STATUS_E                status = MXL_E_SUCCESS;
  UINT8                       localBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE], internalOpcode = 0;
  UINT32                      responseChecksum = 0, calculatedResponseChecksum = 0;
  SINT32                      timeout = 20;
  MXL_EAGLE_HOST_MSG_HEADER_T txMsgHeader;
  MXL_EAGLE_HOST_MSG_HEADER_T *pRxMsgHeader;
  MXL_EAGLE_DEV_CONTEXT_T     *pDevice;

  MXLENTERSTR

  MxL_EAGLE_Oem_MutexGet(devId);

  do 
  {
    /* sanity checks */
    //pDevice = MxL_EAGLE_Ctrl_GetDeviceContext(devId);
	pDevice = pMxL_EAGLE_Device;
    if (pDevice == NULL)
    {
      status = MXL_E_INVALID_PARAMETER;
      break;
    }
    else if (pDevice->isInitialized == MXL_E_FALSE)
    {
      status = MXL_E_NOT_INITIALIZED;
      break;
    }

    if ((txPayloadSize + MXL_EAGLE_HOST_MSG_HEADER_SIZE) > (MXL_EAGLE_MAX_I2C_PACKET_SIZE - MXL_EAGLE_I2C_PHEADER_SIZE))
    {
      status = MXL_E_FAILURE;
      break;
    }

    /* create Tx message */

    //clear Tx message buffer
    MxL_EAGLE_Oem_MemSet(localBuffer, 0, MXL_EAGLE_MAX_I2C_PACKET_SIZE);

    //create message header
    txMsgHeader.opcode      = opcode;
    txMsgHeader.seqNum      = pDevice->seqNum++;
    txMsgHeader.payloadSize = txPayloadSize;
    txMsgHeader.status      = MXL_E_SUCCESS;
    txMsgHeader.checksum    = 0;

    if (pDevice->seqNum == 0)  //sequence number wrapped around
    {
      pDevice->seqNum = 1;
    }

    //copy message header
    MxL_EAGLE_Oem_MemCpy(localBuffer, (UINT8*)&txMsgHeader, MXL_EAGLE_HOST_MSG_HEADER_SIZE);

    //copy payload (if exist)
    if ( (pTxPayload != NULL) && (txPayloadSize > 0) )  
    {
		internalOpcode = *pTxPayload;
		MxL_EAGLE_Oem_MemCpy(&localBuffer[MXL_EAGLE_HOST_MSG_HEADER_SIZE], pTxPayload, txPayloadSize);
    }

    //swap endianness
	MxLWare_EAGLE_ENDIAN_TransmitSwap((MXL_EAGLE_OPCODE_E)opcode, localBuffer);

    //calculate checksum
    txMsgHeader.checksum = MxLWare_EAGLE_UTILS_CheckSumCalc(localBuffer, MXL_EAGLE_HOST_MSG_HEADER_SIZE + txPayloadSize);

    MxL_EAGLE_Oem_MemCpy(&localBuffer[MXL_EAGLE_HOST_MSG_HEADER_SIZE - sizeof(UINT32)], (UINT8*)&txMsgHeader.checksum, sizeof(UINT32));

    /* send Tx message */
	status = MxLWare_EAGLE_UTILS_EnhanceI2cProtWrite(devId, pDevice, localBuffer, txPayloadSize + MXL_EAGLE_HOST_MSG_HEADER_SIZE);
    if (status != MXL_E_SUCCESS)
    {
      status = MXL_E_FAILURE;
      break;
    }

    //opcode specific sleep
    if (status == MXL_E_SUCCESS)
    {
      switch (opcode)
      {
        case MXL_EAGLE_OPCODE_DEVICE_XTAL_CALIBRATION_SET:  //mandatory for XTAL clock to settle
          MxL_EAGLE_Oem_SleepInMs(MXL_EAGLE_HOST_SLEEP_XTAL_SETTLE_MSEC);
          break;

        default:
          break;
      }
    }

    /* receive Rx message */
    pRxMsgHeader = (MXL_EAGLE_HOST_MSG_HEADER_T*)localBuffer;

    //clear Rx message buffer
    MxL_EAGLE_Oem_MemSet(localBuffer, 0, MXL_EAGLE_MAX_I2C_PACKET_SIZE);

    //receive Rx message (polling while message not ready)
    do 
    {
		status = MxLWare_EAGLE_UTILS_EnhanceI2cProtRead(devId, pDevice, localBuffer, rxExpectedPayloadSize + MXL_EAGLE_HOST_MSG_HEADER_SIZE);
      timeout--;
    } while ( (timeout > 0) && (status == MXL_E_SUCCESS) && (pRxMsgHeader->seqNum == 0) && (pRxMsgHeader->checksum == 0) );


  //printk("tttttttt timeout = %d status = %d\n",timeout,status);
//printk("tttttttt pRxMsgHeader->seqNum = %d pRxMsgHeader->checksum = %d\n",pRxMsgHeader->seqNum,pRxMsgHeader->checksum);
  
    if ( (timeout == 0) || (status != MXL_E_SUCCESS) )
    {
		//printf("MXL:timeout error\n");

      status = MXL_E_FAILURE;
      break;
    }

    /* Rx message validations */

    //validate status
    if (pRxMsgHeader->status != MXL_E_SUCCESS) 
    {
		//printf("MXL:pRxMsgHeader->status error\n");

      status = (MXL_STATUS_E)pRxMsgHeader->status;
      break;
    }

    //validate opcode, sequence number, payload size
    if ( (pRxMsgHeader->seqNum != txMsgHeader.seqNum) || (pRxMsgHeader->opcode != txMsgHeader.opcode) || (pRxMsgHeader->payloadSize != rxExpectedPayloadSize) )
    {
		//printf("MXL:validate : pRxMsgHeader->seqNum = 0x%02X\n", pRxMsgHeader->seqNum);
		//printf("MXL:validate : txMsgHeader.seqNum = 0x%02X\n", txMsgHeader.seqNum);
		//printf("MXL:validate : pRxMsgHeader->opcode = 0x%02X\n", pRxMsgHeader->opcode);
		//printf("MXL:validate : txMsgHeader.opcode = 0x%02X\n", txMsgHeader.opcode);
		//printf("MXL:validate : pRxMsgHeader->payloadSize = 0x%02X\n", pRxMsgHeader->payloadSize);
		//printf("MXL:validate : rxExpectedPayloadSize = 0x%02X\n", rxExpectedPayloadSize);

      status = MXL_E_FAILURE;
      break;
    }
    
    //validate checksum
    responseChecksum = pRxMsgHeader->checksum;  
    pRxMsgHeader->checksum = 0;
    calculatedResponseChecksum = MxLWare_EAGLE_UTILS_CheckSumCalc(localBuffer, MXL_EAGLE_HOST_MSG_HEADER_SIZE + pRxMsgHeader->payloadSize);

    if (responseChecksum != calculatedResponseChecksum)
    {
		//printf("MXL:Checksum error : responseChecksum = 0x%08X\n", responseChecksum);
		//printf("MXL:Checksum error : calculatedResponseChecksum = 0x%08X\n", calculatedResponseChecksum);

      status = MXL_E_FAILURE;
      break;
    }

    //swap endianness
	MxLWare_EAGLE_ENDIAN_ReceiveSwap((MXL_EAGLE_OPCODE_E)pRxMsgHeader->opcode, internalOpcode, localBuffer);

    if (pRxMsgHeader->payloadSize > 0)
    {
      if (pRxPayload == NULL)
      {
		  //printf("MXL:pRxPayload null\n");


        status = MXL_E_FAILURE;
        break;
      }

      MxL_EAGLE_Oem_MemCpy(pRxPayload, localBuffer + MXL_EAGLE_HOST_MSG_HEADER_SIZE, pRxMsgHeader->payloadSize);
    }
  } while(0);

  MxL_EAGLE_Oem_MutexRelease(devId);

  MXLEXITSTR(status)

  return status;
}
