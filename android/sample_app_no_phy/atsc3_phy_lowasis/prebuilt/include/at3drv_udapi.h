/*
	at3drv_udapi.h

	AT3UDA driver API, used for URANUS device.
	Project Atlas

	Digital STREAM Labs, Inc. 2017
	Copyright © 2017, 2018, 2019 LowaSIS, Inc.
*/

#ifndef __AT3DRV_UDAPI_H__
#define __AT3DRV_UDAPI_H__

//============================================================================

#include "at3drv_api.h"


#ifdef __cplusplus
extern "C" {
#endif

//============================================================================
// configs



//============================================================================
// typedef



//============================================================================
// API

// All APIs have same semantics as corresponding at3drv api unless explained


//============================================================================
// Init/Open API

AT3DRVAPI AT3RESULT 
	AT3UDA_Init(uint32_t uVersion);

AT3DRVAPI AT3RESULT 
	AT3UDA_OpenDevice(AT3_DEVICE *phDevOut, AT3_DEV_KEY hDevKey, AT3_OPTION hOpt);

AT3DRVAPI AT3RESULT 
	AT3UDA_CloseDevice(AT3_DEVICE hDev);

//============================================================================
// FE control/query API

AT3DRVAPI AT3RESULT 
	AT3UDA_FE_Start(AT3_DEVICE hDev, int nFreqKHz, E_AT3_DEMOD eDemod, uint8_t ucPlpId);

AT3DRVAPI AT3RESULT 
	AT3UDA_FE_Stop(AT3_DEVICE hDev);

AT3DRVAPI AT3RESULT 
	AT3UDA_FE_SetPLP(AT3_DEVICE hDev, uint8_t aPlpIds[], int nPlp);

AT3DRVAPI AT3RESULT 
	AT3UDA_FE_GetStatus(AT3_DEVICE hDev, E_AT3_RFSTAT nStatType, void *pValue);

AT3DRVAPI AT3RESULT 
	AT3UDA_FE_Control(AT3_DEVICE hDev, const char *pMsg, void *pData);

//============================================================================
// Media data Get API

AT3DRVAPI AT3RESULT 
	AT3UDA_WaitRxData(AT3_DEVICE hDev, int nTimeoutMs);

AT3DRVAPI AT3RESULT 
	AT3UDA_HandleRxData(AT3_DEVICE hDev, AT3DRV_DATA_RX_CB fnCallback, uint64_t ullUser);

AT3DRVAPI AT3RESULT
	AT3UDA_CancelWait(AT3_DEVICE hDev);


//============================================================================
// Option API

AT3DRVAPI AT3RESULT
	AT3UDA_Option_Create(AT3_OPTION *pHandle);

AT3DRVAPI AT3RESULT 
	AT3UDA_Option_SetString(AT3_OPTION handle, const char *sOptionName, const char *sOptionValue);
AT3DRVAPI AT3RESULT 
	AT3UDA_Option_SetInt(AT3_OPTION handle, const char *sOptionName, int nOptionValue);

AT3DRVAPI AT3RESULT
	AT3UDA_Option_Release(AT3_OPTION handle);


//============================================================================
// Loader API

AT3DRVAPI AT3RESULT
	AT3UDA_LDR_FindDeviceByType(E_AT3_DEV_TYPE eDevType, AT3_DEV_KEY *pahDevKeys, int nMaxKey, int *pnNumKey);
AT3DRVAPI AT3RESULT 
	AT3UDA_LDR_SearchDevicesByType(E_AT3_DEV_FIND_FILTER eFilter, AT3_DEV_KEY *pahDevKeys, int nMaxKey, int *pnNumKey);

AT3DRVAPI AT3RESULT 
	AT3UDA_LDR_LoadFirmware(AT3_DEV_KEY hKey);

AT3DRVAPI AT3RESULT 
	AT3UDA_LDR_LoadFirmwareEx(AT3_DEV_KEY hKey, const char *sVersion);

AT3DRVAPI AT3RESULT 
	AT3UDA_LDR_CheckDeviceExist(AT3_DEV_KEY hKey, E_AT3_DEV_TYPE *peDevType);


//============================================================================
// ETC



#ifdef __cplusplus
};
#endif


#endif // __AT3DRV_UDAPI_H__

