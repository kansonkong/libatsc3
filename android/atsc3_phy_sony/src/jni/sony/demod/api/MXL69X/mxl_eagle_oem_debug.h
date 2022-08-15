/*****************************************************************************************   
 * FILE NAME          : mxl_eagle_oem_debug.h
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 29/Sep/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains OEM debug functions header
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
#ifndef __MXL_EAGLE_OEM_DEBUG_H__
#define __MXL_EAGLE_OEM_DEBUG_H__

//#include <linux/stdio.h>
#include <linux/module.h>
#include <linux/kernel.h>




/*****************************************************************************************
    Defines
 ****************************************************************************************/
#define MXL_DEBUG_GENERAL       0<<0
#define MXL_DEBUG_API           0<<1
#define MXL_DEBUG_OPTIONS       (MXL_DEBUG_GENERAL | MXL_DEBUG_API)   //select active debug options


#if (MXL_DEBUG_OPTIONS & MXL_DEBUG_GENERAL)
  #define MXLENTERSTR                   MxL_EAGLE_Oem_Debug_Printf("+++++ %s", __FUNCTION__);
  #define MXLEXITSTR(retcode)           MxL_EAGLE_Oem_Debug_Printf("----- %s (retcode: %d)", __FUNCTION__, retcode);
#else
  #define MXLENTERSTR
  #define MXLEXITSTR(retcode)
#endif

#if (MXL_DEBUG_OPTIONS & MXL_DEBUG_API)
#define MXLENTERAPISTR(devId)           MxL_EAGLE_Oem_Debug_Printf("+++ %s [%d]", __FUNCTION__, devId);
#define MXLEXITAPISTR(devId, retcode)   MxL_EAGLE_Oem_Debug_Printf("--- %s [%d] (retcode: %d)", __FUNCTION__, devId, retcode);
#else
  #define MXLENTERAPISTR(devId)
  #define MXLEXITAPISTR(devId, retcode)
#endif


/*****************************************************************************************
    Typedefs
 ****************************************************************************************/
typedef struct
{
  UINT16  num;
  char    string[64];
} MXL_OEM_NUM_STR_DICTIONARY_STRUCT;


/*****************************************************************************************
    Functions Prototype
 ****************************************************************************************/
void MxL_EAGLE_Oem_Debug_Printf(const char * format, ...);
void MxL_EAGLE_Oem_Debug_EnableDebugLog(MXL_BOOL_E enable);
const char* MxL_EAGLE_Oem_Debug_StatusToString(MXL_STATUS_E status);




#endif  //__MXL_EAGLE_OEM_DEBUG_H__

