/*****************************************************************************************   
 * FILE NAME          : mxlware_eagle_dev_api.c
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 29/Sep/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains MxLWare device and driver configuration APIs
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
 *  \defgroup Device
 *  @brief     This section defines the Device APIs for Eagle.
 *  @{
 *
 ****************************************************************************************/

#include "mxl_datatypes.h"
#include "mxlware_eagle_product_id.h"
#include "mxlware_eagle_sku_features.h"
#include "mxlware_eagle_utils.h"
#include "mxl_eagle_oem_debug.h"
#include "mxl_eagle_oem.h"
#include "mxl_eagle_version.h"


/*****************************************************************************************
    API Functions
 ****************************************************************************************/
/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDrvInit
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API toggles the chip reset, and validates device type as read from 
*                 register to the desired device type as declared in MxLWare_EAGLE_API_CfgDrvInit
* 
* \param[in]      devId       The MxL device id
* \param[in]      pOemData    Pointer to customer's specific data. 
*                             This data is not processed by MxLWare and is passed to OEM functions.
*                             This parameter is not required; use NULL if not needed
* \param[in]      deviceType  Device type that should be supported by the driver's instance related with the current devId
*
* \returns        MXL_SUCCESS, MXL_ALREADY_INITIALIZED, MXL_INVALID_PARAMETER 

*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxLWare_EAGLE_API_CfgDrvInit(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, void *pOemData, MXL_EAGLE_DEVICE_E deviceType, unsigned char i2c_bus, unsigned char i2c_add)
{
  MXL_STATUS_E              status = MXL_E_SUCCESS;
  MXL_EAGLE_DEV_CONTEXT_T   *pDevice;

  MXLENTERAPISTR(devId)

  MxLWare_EAGLE_UTILS_EndiannessDetect();

  //pDevice = MxL_EAGLE_Ctrl_GetDeviceContext(devId);
  pDevice = pMxL_EAGLE_Device;
  if (pDevice != NULL)
  {    
    if (pDevice->isInitialized == MXL_E_FALSE)
    {
		status = MxL_EAGLE_Oem_Init(devId, pDevice, pOemData, i2c_bus, i2c_add);


      if (status == MXL_E_SUCCESS)
      {
        pDevice->isInitialized = MXL_E_TRUE;      
        pDevice->deviceType    = deviceType;
        pDevice->seqNum        = 1;
      }
      else
      {
		  MxL_EAGLE_Oem_UnInit(devId, pDevice);   //release driver for further connection tries
      }
    } 
    else 
    {
      status = MXL_E_ALREADY_INITIALIZED;
    }
  } 
  else 
  {
    status = MXL_E_INVALID_PARAMETER;   
  }

  MXLEXITAPISTR(devId, status)

  return status;
}


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDrvUnInit
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
* DATE CREATED  : 29/09/2016
*
* \brief          This API uninitialized the MxlWare driver
* 
* \param[in]      devId       The MxL device id
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxLWare_EAGLE_API_CfgDrvUnInit(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device)
{
  MXL_STATUS_E              status = MXL_E_SUCCESS;
  MXL_EAGLE_DEV_CONTEXT_T   *pDevice;

  MXLENTERAPISTR(devId)

  //pDevice = MxL_EAGLE_Ctrl_GetDeviceContext(devId);
  pDevice = pMxL_EAGLE_Device;
  if (pDevice != NULL)
  {    
    if (pDevice->isInitialized == MXL_E_TRUE)
    {
		status = MxL_EAGLE_Oem_UnInit(devId, pDevice);
      if (status == MXL_E_SUCCESS)
      {
        pDevice->isInitialized = MXL_E_FALSE;      
      }
    } 
    else 
    {
      status = MXL_E_NOT_INITIALIZED;
    }
  } 
  else 
  {
    status = MXL_E_INVALID_PARAMETER;   
  }

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevReset
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API toggles the chip reset, and validates device type as read from 
*                 register to the desired device type as declared in MxLWare_EAGLE_API_CfgDrvInit
* 
* \param[in]      devId       The MxL device id
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevReset(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device)
{
  MXL_STATUS_E              status = MXL_E_SUCCESS;
  MXL_EAGLE_DEV_CONTEXT_T   *pDevice;
  UINT32                    deviceType = MXL_EAGLE_DEVICE_MAX, regValue;

  MXLENTERAPISTR(devId)

  MxL_EAGLE_Oem_MutexGet(devId);

  do 
  {
    /* sanity check */
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

    //reset the device
    status = MxL_EAGLE_Oem_DevReset(devId);
    if (status != MXL_E_SUCCESS) break;

    //sleep 100msec
	MxL_EAGLE_Oem_SleepInMs(MXL_EAGLE_HOST_SLEEP_DEVICE_SETTLE_MSEC);

	regValue = 2;
	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemWrite(devId, pDevice, 0x80000100, (UINT8*)&regValue, sizeof(UINT32));  //I2C_LEGACY_I2C_OVERRIDE
    if (status != MXL_E_SUCCESS) break;

    //verify SKU
	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemRead(devId, pDevice, 0x9004000c, (UINT8*)&regValue, sizeof(UINT32));
	if (status != MXL_E_SUCCESS) break;

	regValue &= 0xFFFCF9FF;
	regValue |= 0x00010200;

	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemWrite(devId, pDevice, 0x9004000c, (UINT8*)&regValue, sizeof(UINT32));
	if (status != MXL_E_SUCCESS) break;

	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemRead(devId, pDevice, 0x70000188, (UINT8*)&deviceType, sizeof(UINT32));
    if (status != MXL_E_SUCCESS) break;

    if (deviceType != pDevice->deviceType)
    {
      status = MXL_E_NOT_SUPPORTED;
    }
  } while(0);

  MxL_EAGLE_Oem_MutexRelease(devId);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevPowerRegulators
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API configures the main and digital regulators.
* 
* \param[in]      devId             The MxL device id
* \param[in]      powerSupply       power supply source (single, dual)
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevPowerRegulators(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_POWER_SUPPLY_SOURCE_E powerSupply)
{
  MXL_STATUS_E              status = MXL_E_SUCCESS;
  MXL_EAGLE_DEV_CONTEXT_T   *pDevice;
  UINT32                    regValue;

  MXLENTERAPISTR(devId)

  MxL_EAGLE_Oem_MutexGet(devId);

  do 
  {
    /* sanity check */
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

    MXL_PARAMETER_VALIDATE(powerSupply, MXL_EAGLE_POWER_SUPPLY_SOURCE_SINGLE, MXL_EAGLE_POWER_SUPPLY_SOURCE_DUAL);


    //configure main regulator according to the power supply source
	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemRead(devId, pDevice, 0x90000000, (UINT8*)&regValue, sizeof(UINT32));
    if (status != MXL_E_SUCCESS) break;

    regValue &= 0x00FFFFFF;
    regValue |= (powerSupply == MXL_EAGLE_POWER_SUPPLY_SOURCE_SINGLE) ? 0x14000000 : 0x10000000;

	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemWrite(devId, pDevice, 0x90000000, (UINT8*)&regValue, sizeof(UINT32));
    if (status != MXL_E_SUCCESS) break;

    //configure digital regulator to high current mode
	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemRead(devId, pDevice, 0x90000018, (UINT8*)&regValue, sizeof(UINT32));
    if (status != MXL_E_SUCCESS) break;

    regValue |= 0x800;

	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemWrite(devId, pDevice, 0x90000018, (UINT8*)&regValue, sizeof(UINT32));
    if (status != MXL_E_SUCCESS) break;
  } while(0);


  MxL_EAGLE_Oem_MutexRelease(devId);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevXtal
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API configures the device XTAL 
* 
* \param[in]      devId             The MxL device id
* \param[in]      pDevXtalStruct    Pointer to MXL_EAGLE_DEV_XTAL_T containing XTAL configuration
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevXtal(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_DEV_XTAL_T *pDevXtalStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         txBuffer[MXL_EAGLE_DEV_XTAL_SIZE] = { 0 };
	UINT8         *pu8TxPayload = txBuffer;

	MXLENTERAPISTR(devId)

	do
	{
		/* sanity checks */
		MXL_PARAMETER_VALIDATE(pDevXtalStruct->xtalCap, 1, 31);
		MXL_PARAMETER_VALIDATE(pDevXtalStruct->clkOutDivEnable, MXL_E_DISABLE, MXL_E_ENABLE);
		MXL_PARAMETER_VALIDATE(pDevXtalStruct->clkOutEnable, MXL_E_DISABLE, MXL_E_ENABLE);
		MXL_PARAMETER_VALIDATE(pDevXtalStruct->xtalCalibrationEnable, MXL_E_DISABLE, MXL_E_ENABLE);
		MXL_PARAMETER_VALIDATE(pDevXtalStruct->xtalSharingEnable, MXL_E_DISABLE, MXL_E_ENABLE);

		MxL_EAGLE_Oem_MemSet(txBuffer, 0, MXL_EAGLE_DEV_XTAL_SIZE);
		MxL_EAGLE_UTILS_PushUint8(pDevXtalStruct->xtalCap, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pDevXtalStruct->clkOutEnable, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pDevXtalStruct->clkOutDivEnable, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pDevXtalStruct->xtalSharingEnable, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pDevXtalStruct->xtalCalibrationEnable, &pu8TxPayload);

		status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
													MXL_EAGLE_OPCODE_DEVICE_XTAL_CALIBRATION_SET,
													txBuffer,
													MXL_EAGLE_DEV_XTAL_SIZE,
													NULL,
													0);

	} while (0);

	MXLEXITAPISTR(devId, status)

	return status;
}


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevLoopthrough
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API enables or disables the RF Loop thru function
* 
* \param[in]      devId             The MxL device id
* \param[in]      enableRfLoopThru  Enable or Disable RF Loop thru function
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevLoopthrough(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_BOOL_E enableRfLoopThru)
{
  MXL_STATUS_E              status = MXL_E_SUCCESS;
  MXL_EAGLE_DEV_CONTEXT_T   *pDevice;
  UINT32                    regValue;

  MXLENTERAPISTR(devId)

  MxL_EAGLE_Oem_MutexGet(devId);

  do 
  {
    /* sanity check */
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

    MXL_PARAMETER_VALIDATE(enableRfLoopThru, MXL_E_DISABLE, MXL_E_ENABLE);


	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemRead(devId, pDevice, 0x9001014c, (UINT8*)&regValue, sizeof(UINT32));
    if (status != MXL_E_SUCCESS) break;

    regValue = (enableRfLoopThru == MXL_E_ENABLE) ? (regValue | 0x2000) : (regValue & 0xDFFF);

	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemWrite(devId, pDevice, 0x9001014c, (UINT8*)&regValue, sizeof(UINT32));
    if (status != MXL_E_SUCCESS) break;
  } while(0);


  MxL_EAGLE_Oem_MutexRelease(devId);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevFwDownload
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API downloads the FW to the chip
* 
* \param[in]      devId             The MxL device id
* \param[in]      pFwBuffer         Pointer of the buffer that stores the MBIN format firmware
* \param[in]      bufferLen         Length of FW buffer
* \param[in]      pCallbackFn       Pointer to the callback function that is periodically called to report the firmware download progress, or for other purposes defined by the customer’s software
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevFwDownload(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pFwBuffer, UINT32 bufferLen, MXL_CALLBACK_FN_T pCallbackFn)
{
  MXL_STATUS_E              status = MXL_E_SUCCESS;
  MXL_EAGLE_DEV_CONTEXT_T   *pDevice;
  MXL_EAGLE_DEV_STATUS_T    deviceStatus = { 0, 0, 0, 0 };
  UINT32                    ix, regValue = 0x1;

  MXLENTERAPISTR(devId)

  MxL_EAGLE_Oem_MutexGet(devId);

  do 
  {
    /* sanity checks */
    //pDevice = MxL_EAGLE_Ctrl_GetDeviceContext(devId);
	pDevice = pMxL_EAGLE_Device;
    if ( (pDevice == NULL) || (pFwBuffer == NULL) )
    {
      status = MXL_E_INVALID_PARAMETER;
      break;
    }
    else if (pDevice->isInitialized == MXL_E_FALSE)
    {
      status = MXL_E_NOT_INITIALIZED;
      break;
    }

    MXL_PARAMETER_VALIDATE(bufferLen, MXL_EAGLE_FW_HEADER_SIZE, MXL_EAGLE_FW_MAX_SIZE_IN_KB * 1000);


    //validate FW header
    status = MxLWare_EAGLE_UTILS_ValidateFwHeader(pFwBuffer, bufferLen);
    if (status != MXL_E_SUCCESS) break;

    //download FW
    ix = 16;
	status = MxLWare_EAGLE_UTILS_DownloadFwSegment(devId, pDevice, pFwBuffer, bufferLen, &ix, pCallbackFn);  //DRAM
    if (status != MXL_E_SUCCESS) break;

	status = MxLWare_EAGLE_UTILS_DownloadFwSegment(devId, pDevice, pFwBuffer, bufferLen, &ix, pCallbackFn);  //IRAM
    if (status != MXL_E_SUCCESS) break;

    //release CPU from reset
	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemWrite(devId, pDevice, 0x70000018, (UINT8*)&regValue, sizeof(UINT32));
    if (status != MXL_E_SUCCESS) break;

    MxL_EAGLE_Oem_MutexRelease(devId);

    if (status == MXL_E_SUCCESS)
    {
      //verify FW is alive
		MxL_EAGLE_Oem_SleepInMs(MXL_EAGLE_HOST_SLEEP_FW_LOAD_MSEC);

		status = MxLWare_EAGLE_API_ReqDevStatus(devId, pDevice, &deviceStatus);
      if (status != MXL_E_SUCCESS) break;
    }
  } while (0);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevDemodType
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API configures demodulator type (QAM, ATSC, OOB) if needed
* 
* \param[in]      devId             The MxL device id
* \param[in]      demodType         Demodulator type (QAM, ATSC, OOB)
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevDemodType(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_DEMOD_TYPE_E demodType)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;
  UINT8         txPayload;

  MXLENTERAPISTR(devId)

  do 
  {
    /* sanity checks */
    MXL_PARAMETER_VALIDATE(demodType, MXL_EAGLE_DEMOD_TYPE_QAM, MXL_EAGLE_DEMOD_TYPE_ATSC);

	txPayload = (UINT8)demodType;
	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
		MXL_EAGLE_OPCODE_DEVICE_DEMODULATOR_TYPE_SET,
		&txPayload,
		sizeof(UINT8),
		NULL,
		0);

  } while(0);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDemodMpegOutParams
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API configures the MPEG output parameters - sync byte (valid / invalid byte), 
*                 TS mode (parallel / serial), clock polarity, etc
* 
* \param[in]      devId             The MxL device id
* \param[in]      pMpegOutParamStruct  Pointer to structure containing all MPEG out configuration settings
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevMpegOutParams(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_MPEGOUT_PARAMS_T *pMpegOutParamStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         txBuffer[MXL_EAGLE_MPEGOUT_SIZE] = { 0 };
	UINT8         *pu8TxPayload = txBuffer;

	MXLENTERAPISTR(devId)

	do
	{
		/* sanity checks */
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->mpeg3WireModeEnable, MXL_E_DISABLE, MXL_E_ENABLE);
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->mpegIsParallel, MXL_E_FALSE, MXL_E_TRUE);
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->lsbOrMsbFirst, MXL_EAGLE_DATA_SERIAL_LSB_1ST, MXL_EAGLE_DATA_SERIAL_MSB_1ST);
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->mpegClkFreq, MXL_EAGLE_MPEG_CLOCK_54MHz, MXL_EAGLE_MPEG_CLOCK_13_5MHz);
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->mpegSyncPol, MXL_EAGLE_CLOCK_POSITIVE, MXL_EAGLE_CLOCK_NEGATIVE);
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->mpegValidPol, MXL_EAGLE_CLOCK_POSITIVE, MXL_EAGLE_CLOCK_NEGATIVE);
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->mpegClkPol, MXL_EAGLE_CLOCK_POSITIVE, MXL_EAGLE_CLOCK_NEGATIVE);
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->mpegSyncPulseWidth, MXL_EAGLE_DATA_SYNC_WIDTH_BIT, MXL_EAGLE_DATA_SYNC_WIDTH_BYTE);
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->mpegPadDrv.padDrvMpegSyn, MXL_EAGLE_IO_MUX_DRIVE_MODE_1X, MXL_EAGLE_IO_MUX_DRIVE_MODE_8X);
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->mpegPadDrv.padDrvMpegDat, MXL_EAGLE_IO_MUX_DRIVE_MODE_1X, MXL_EAGLE_IO_MUX_DRIVE_MODE_8X);
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->mpegPadDrv.padDrvMpegVal, MXL_EAGLE_IO_MUX_DRIVE_MODE_1X, MXL_EAGLE_IO_MUX_DRIVE_MODE_8X);
		MXL_PARAMETER_VALIDATE(pMpegOutParamStruct->mpegPadDrv.padDrvMpegClk, MXL_EAGLE_IO_MUX_DRIVE_MODE_1X, MXL_EAGLE_IO_MUX_DRIVE_MODE_8X);

		if (pMpegOutParamStruct->mpegIsParallel == MXL_E_TRUE) //parallel interface
		{
			if ((pMpegOutParamStruct->mpegClkFreq == MXL_EAGLE_MPEG_CLOCK_40_5MHz) || (pMpegOutParamStruct->mpegClkFreq == MXL_EAGLE_MPEG_CLOCK_54MHz))
			{
				status = MXL_E_INVALID_PARAMETER;
				break;
			}
		}
		else //serial interface
		{
			if ((pMpegOutParamStruct->mpegClkFreq == MXL_EAGLE_MPEG_CLOCK_13_5MHz) || (pMpegOutParamStruct->mpegClkFreq == MXL_EAGLE_MPEG_CLOCK_40_5MHz))
			{
				status = MXL_E_INVALID_PARAMETER;
				break;
			}
		}
		MxL_EAGLE_Oem_MemSet(txBuffer, 0, MXL_EAGLE_MPEGOUT_SIZE);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->mpegIsParallel, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->lsbOrMsbFirst, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->mpegSyncPulseWidth, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->mpegValidPol, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->mpegSyncPol, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->mpegClkPol, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->mpeg3WireModeEnable, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->mpegClkFreq, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->mpegPadDrv.padDrvMpegSyn, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->mpegPadDrv.padDrvMpegDat, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->mpegPadDrv.padDrvMpegVal, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pMpegOutParamStruct->mpegPadDrv.padDrvMpegClk, &pu8TxPayload);

		status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
													MXL_EAGLE_OPCODE_DEVICE_MPEG_OUT_PARAMS_SET,
													txBuffer,
													MXL_EAGLE_MPEGOUT_SIZE,
													NULL,
													0);
	} while (0);

	MXLEXITAPISTR(devId, status)

	return status;
}


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevPowerMode
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API configures the device power mode - sleep/active
* 
* \param[in]      devId             The MxL device id
* \param[in]      powerMode         Desired power mode (sleeping, active)
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevPowerMode(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_POWER_MODE_E powerMode)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;
  UINT8         txPayload;

  MXLENTERAPISTR(devId)

  do 
  {
    /* sanity checks */
    MXL_PARAMETER_VALIDATE(powerMode, MXL_EAGLE_POWER_MODE_SLEEP, MXL_EAGLE_POWER_MODE_ACTIVE);

	txPayload = (UINT8)powerMode;
	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
		MXL_EAGLE_OPCODE_DEVICE_POWERMODE_SET,
		&txPayload,
		sizeof(UINT8),
		NULL,
		0);

  } while(0);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevIoMux
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
* DATE CREATED  : 03/01/2017
*
* \brief          This API configures I/O for secondary functions, which are not configured by any other API
* 
* \param[in]      devId             The MxL device id
* \param[in]      func              Desired I/O MUX function
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevIoMux(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_IOMUX_FUNCTION_E func)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;
  UINT8         txPayload;

  MXLENTERAPISTR(devId)

  do 
  {
    /* sanity checks */
    MXL_PARAMETER_VALIDATE(func, MXL_EAGLE_IOMUX_FUNC_FEC_LOCK, MXL_EAGLE_IOMUX_FUNC_MERR);

	txPayload = (UINT8)func;
	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
		MXL_EAGLE_OPCODE_DEVICE_IO_MUX_SET,
		&txPayload,
		sizeof(UINT8),
		NULL,
		0);

  } while(0);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevIntrMask
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API configures host interrupt interrupt behavior
* 
* \param[in]      devId             The MxL device id
* \param[in]      pIntrCfgStruct    Pointer to MXL_EAGLE_DEV_INTR_CFG_T containing host interrupt configuration
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevIntrMask(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_DEV_INTR_CFG_T *pIntrCfgStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         txBuffer[MXL_EAGLE_DEV_INTR_CFG_SIZE] = { 0 };
	UINT8         *pu8TxPayload = txBuffer;

	MXLENTERAPISTR(devId)

	do
	{
		/* sanity checks */
		MXL_PARAMETER_VALIDATE(pIntrCfgStruct->edgeTrigger, MXL_E_FALSE, MXL_E_TRUE);
		MXL_PARAMETER_VALIDATE(pIntrCfgStruct->positiveTrigger, MXL_E_FALSE, MXL_E_TRUE);
		MXL_PARAMETER_VALIDATE(pIntrCfgStruct->globalEnableInterrupt, MXL_E_FALSE, MXL_E_TRUE);

		MxL_EAGLE_Oem_MemSet(txBuffer, 0, MXL_EAGLE_DEV_INTR_CFG_SIZE);
		MxL_EAGLE_UTILS_PushUint32(pIntrCfgStruct->intrMask, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pIntrCfgStruct->edgeTrigger, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pIntrCfgStruct->positiveTrigger, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(pIntrCfgStruct->globalEnableInterrupt, &pu8TxPayload);

		status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
													MXL_EAGLE_OPCODE_DEVICE_INTR_MASK_SET,
													txBuffer,
													MXL_EAGLE_DEV_INTR_CFG_SIZE,	
													NULL,
													0);
	} while (0);

	MXLEXITAPISTR(devId, status)

	return status;
}


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevGpioDirection
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API configures the GPIO direction
* 
* \param[in]      devId             The MxL device id
* \param[in]      gpioNumber        GPIO number 0..6
* \param[in]      gpioDirection     GPIO direction In/Out
* 
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevGpioDirection(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_GPIO_NUMBER_E gpioNumber, MXL_EAGLE_GPIO_DIRECTION_E gpioDirection)
{
	MXL_STATUS_E                    status = MXL_E_SUCCESS;
	UINT8                           txBuffer[MXL_EAGLE_DEV_GPIO_DIRECTION_SIZE] = { 0 };
	UINT8                           *pu8TxPayload = txBuffer;

	MXLENTERAPISTR(devId)

	do
	{
		/* sanity checks */
		MXL_PARAMETER_VALIDATE(gpioNumber, MXL_EAGLE_GPIO_NUMBER_0, MXL_EAGLE_GPIO_NUMBER_6);
		MXL_PARAMETER_VALIDATE(gpioDirection, MXL_EAGLE_GPIO_DIRECTION_INPUT, MXL_EAGLE_GPIO_DIRECTION_OUTPUT);

		MxL_EAGLE_Oem_MemSet(txBuffer, 0, MXL_EAGLE_DEV_GPIO_DIRECTION_SIZE);
		MxL_EAGLE_UTILS_PushUint8(gpioNumber, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(gpioDirection, &pu8TxPayload);

		status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
													MXL_EAGLE_OPCODE_DEVICE_GPIO_DIRECTION_SET,
													txBuffer,
													MXL_EAGLE_DEV_GPIO_DIRECTION_SIZE,
													NULL,
													0);
	} while (0);

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevGpoLevel
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API sets the GPI level
* 
* \param[in]      devId             The MxL device id
* \param[in]      gpioNumber        GPIO number 0..6
* \param[in]      gpoLevel          GPO level High/Low
* 
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevGpoLevel(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_GPIO_NUMBER_E gpioNumber, MXL_EAGLE_GPIO_LEVEL_E gpoLevel)
{
	MXL_STATUS_E               status = MXL_E_SUCCESS;
	UINT8                      txBuffer[MXL_EAGLE_DEV_GPO_LEVEL_SIZE] = { 0 };
	UINT8                      *pu8TxPayload = txBuffer;

	MXLENTERAPISTR(devId)

	do
	{
		/* sanity checks */
		MXL_PARAMETER_VALIDATE(gpioNumber, MXL_EAGLE_GPIO_NUMBER_0, MXL_EAGLE_GPIO_NUMBER_6);
		MXL_PARAMETER_VALIDATE(gpoLevel, MXL_EAGLE_GPIO_LEVEL_LOW, MXL_EAGLE_GPIO_LEVEL_HIGH);

		MxL_EAGLE_Oem_MemSet(txBuffer, 0, MXL_EAGLE_DEV_GPO_LEVEL_SIZE);
		MxL_EAGLE_UTILS_PushUint8(gpioNumber, &pu8TxPayload);
		MxL_EAGLE_UTILS_PushUint8(gpoLevel, &pu8TxPayload);

		status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
													MXL_EAGLE_OPCODE_DEVICE_GPO_LEVEL_SET,
													txBuffer,
													MXL_EAGLE_DEV_GPO_LEVEL_SIZE,
													NULL,
													0);
	} while (0);

	MXLEXITAPISTR(devId, status)

	return status;
}


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_CfgDevRegister
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API directly writes a register. Intended for debug purposes only.
* 
* \param[in]      devId             The MxL device id
* \param[in]      regAddress        Register address
* \param[in]      regValue          Register Value
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_CfgDevRegister(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT32 regAddress, UINT32 regValue)
{
  MXL_STATUS_E              status = MXL_E_SUCCESS;
  MXL_EAGLE_DEV_CONTEXT_T   *pDevice;

  MXLENTERAPISTR(devId)

  MxL_EAGLE_Oem_MutexGet(devId);

  do 
  {
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

	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemWrite(devId, pDevice, regAddress, (UINT8*)&regValue, sizeof(UINT32));
  } while(0);

  MxL_EAGLE_Oem_MutexRelease(devId);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqDevRegister
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API directly reads a register. Intended for debug purposes only.
* 
* \param[in]      devId             The MxL device id
* \param[in]      regAddress        Register address
* \param[out]     pRegValue         Pointer to read register value to
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqDevRegister(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT32 regAddress, UINT32 *pRegValue)
{
  MXL_STATUS_E              status = MXL_E_SUCCESS;
  MXL_EAGLE_DEV_CONTEXT_T   *pDevice;

  MXLENTERAPISTR(devId)

  MxL_EAGLE_Oem_MutexGet(devId);

  do 
  {
    /* sanity check */
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

	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemRead(devId, pDevice, regAddress, (UINT8*)pRegValue, sizeof(UINT32));
  } while(0);

  MxL_EAGLE_Oem_MutexRelease(devId);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqDevVersion
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API reads device version information
* 
* \param[in]      devId             The MxL device id
* \param[out]     pVersionStruct    Pointer to MXL_EAGLE_DEV_VER_T to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqDevVersionInfo(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_DEV_VER_T *pVersionStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;
	UINT8         u8Index = 0;

	MXLENTERAPISTR(devId)

		MxL_EAGLE_Oem_MemSet((UINT8*)pVersionStruct, 0, sizeof(MXL_EAGLE_DEV_VER_T));

	pVersionStruct->mxlWareVer[0] = VER_A;
	pVersionStruct->mxlWareVer[1] = VER_B;
	pVersionStruct->mxlWareVer[2] = VER_C;
	pVersionStruct->mxlWareVer[3] = VER_D;
	pVersionStruct->mxlWareVer[4] = VER_E;

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_DEVICE_VERSION_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_DEV_VER_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		pVersionStruct->chipId = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);

		for (u8Index = 0; u8Index < MXL_EAGLE_VERSION_SIZE; u8Index++)
		{
			pVersionStruct->firmwareVer[u8Index] = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		}
	}

	MXLEXITAPISTR(devId, status)

	return status;
}


/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqDevStatus
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API reads device version information
* 
* \param[in]      devId             The MxL device id
* \param[out]     pStatusStruct     Pointer to MXL_EAGLE_DEV_STATUS_T to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqDevStatus(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_DEV_STATUS_T *pStatusStruct)
{
	MXL_STATUS_E  status = MXL_E_SUCCESS;
	UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE];
	UINT8         *pu8RxPayload = rxBuffer;

	MXLENTERAPISTR(devId)

	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
												MXL_EAGLE_OPCODE_DEVICE_STATUS_GET,
												NULL,
												0,
												rxBuffer,
												MXL_EAGLE_DEV_STATUS_SIZE);

	if (status == MXL_E_SUCCESS)
	{
		pStatusStruct->temperature = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pStatusStruct->demodulatorType = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pStatusStruct->powerMode = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
		pStatusStruct->cpuUtilizationPercent = MxL_EAGLE_UTILS_PopUint8(&pu8RxPayload);
	}

	MXLEXITAPISTR(devId, status)

	return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqDevGpiLevel
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API returns the GPI level
* 
* \param[in]      devId             The MxL device id
* \param[in]      gpioNumber        GPIO number 0..6
* \param[out]     pGpiLevel         Pointer to GPI level to read to
* 
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_INVALID_PARAMETER, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqDevGpiLevel(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_GPIO_NUMBER_E gpioNumber, MXL_EAGLE_GPIO_LEVEL_E *pGpiLevel)
{
  MXL_STATUS_E  status = MXL_E_SUCCESS;
  UINT8         rxBuffer[MXL_EAGLE_MAX_I2C_PACKET_SIZE] = {0};
  UINT8         txPayload;

  MXLENTERAPISTR(devId)

  do 
  {
    /* sanity checks */
    MXL_PARAMETER_VALIDATE(gpioNumber, MXL_EAGLE_GPIO_NUMBER_0, MXL_EAGLE_GPIO_NUMBER_6);

	txPayload = (UINT8)gpioNumber;
	status = MxLWare_EAGLE_UTILS_SendAndReceive(devId, pMxL_EAGLE_Device,
		MXL_EAGLE_OPCODE_DEVICE_GPI_LEVEL_GET,
		&txPayload,
		sizeof(UINT8),
		rxBuffer,
		sizeof(UINT8));

    if (status == MXL_E_SUCCESS)
    {
      *pGpiLevel = (MXL_EAGLE_GPIO_LEVEL_E)rxBuffer[0];
    }
  } while (0);

  MXLEXITAPISTR(devId, status)

  return status;
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_API_ReqDevIntrStatus
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 01/01/2017
*
* \brief          This API reads host interrupt status
* 
* \param[in]      devId             The MxL device id
* \param[out]     pIntrStatus       Pointer to UINT32 interrupt status to fill
*
* \returns        MXL_SUCCESS, MXL_NOT_INITIALIZED, MXL_NOT_SUPPORTED, MXL_FAILURE 
*
*-------------------------------------------------------------------------------------*/
MXL_STATUS_E MXLWAREDLL_API MxLWare_EAGLE_API_ReqDevIntrStatus(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT32 *pIntrStatus)
{
  MXL_STATUS_E              status = MXL_E_SUCCESS;
  MXL_EAGLE_DEV_CONTEXT_T   *pDevice;
  UINT32                    intStat = 0, intStatRo = 0;

  MXLENTERAPISTR(devId)

  MxL_EAGLE_Oem_MutexGet(devId);

  do 
  {
    /* sanity check */
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

	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemRead(devId, pDevice, 0x70000114, (UINT8*)&intStat, sizeof(UINT32));
    if (status != MXL_E_SUCCESS) break;

	status = MxLWare_EAGLE_UTILS_EnhanceI2cMemRead(devId, pDevice, 0x70000118, (UINT8*)&intStatRo, sizeof(UINT32));
    if (status != MXL_E_SUCCESS) break;

    *pIntrStatus = (intStat | intStatRo);       // unify RO register to reflect Wdog bit 31 into interrupt status register

  } while(0);


  MxL_EAGLE_Oem_MutexRelease(devId);

  MXLEXITAPISTR(devId, status)

  return status;
}
