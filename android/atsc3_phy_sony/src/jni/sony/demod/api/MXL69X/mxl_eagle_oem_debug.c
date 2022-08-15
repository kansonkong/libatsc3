/*****************************************************************************************   
 * FILE NAME          : mxl_eagle_oem_debug.c
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 29/Sep/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains OEM debug functions definition
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
#ifdef WIN32
#endif
#include <stdarg.h>
//#include <linux/stdio.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include "mxl_datatypes.h"
#include "mxl_eagle_oem_debug.h"

#define OutputDebugString(x) 


#ifdef __linux__
  // replace OutputDebugString() with any logging function you want
  //#define OutputDebugString(x) printf("%s", x);
  #define OutputDebugString(x) 
#endif

/*****************************************************************************************
    Global Variables
 ****************************************************************************************/
static MXL_BOOL_E gIsDebugEnabled = MXL_E_TRUE;
static const MXL_OEM_NUM_STR_DICTIONARY_STRUCT statusDictionary[] = { {MXL_E_SUCCESS,             "MXL_SUCCESS"},
                                                                      {MXL_E_FAILURE,             "MXL_FAILURE"},
                                                                      {MXL_E_INVALID_PARAMETER,   "MXL_INVALID_PARAMETER"},
                                                                      {MXL_E_NOT_INITIALIZED,     "MXL_NOT_INITIALIZED"},
                                                                      {MXL_E_ALREADY_INITIALIZED, "MXL_ALREADY_INITIALIZED"},
                                                                      {MXL_E_NOT_SUPPORTED,       "MXL_NOT_SUPPORTED"},
                                                                      {MXL_E_NOT_READY,           "MXL_NOT_READY"},
                                                                      {MXL_E_DATA_ERROR,          "MXL_DATA_ERROR"}, 
																	  {MXL_E_I2C_ERROR,			  "MXL_DATA_ERROR"} };


/*****************************************************************************************
    Functions
 ****************************************************************************************/
/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_Debug_EnableDebugLog
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 29/09/2016
*
* \brief          This OEM function enables/disables host debug logs
* 
* \param[in]      enable                Boolean
* 
* \returns        void
*-------------------------------------------------------------------------------------*/
void MxL_EAGLE_Oem_Debug_EnableDebugLog(MXL_BOOL_E enable)
{
  MXLENTERSTR

  MxL_EAGLE_Oem_Debug_Printf((enable == MXL_E_TRUE) ? "Host Debug logs Enabled \n" : "Host Debug logs Disabled \n");

  gIsDebugEnabled = enable;

  MXLEXITSTR(0);
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_Debug_Printf
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 29/09/2016
*
* \brief          This OEM function prints the debug logs to the debug output window
* 
* \param[in]      format                printf() format string
* 
* \returns        void
*-------------------------------------------------------------------------------------*/
void MxL_EAGLE_Oem_Debug_Printf(const char * format, ...)
{
  va_list args;
  char    buffer[500];
  char    dllStr[504] = {'\0',};
  
  if(gIsDebugEnabled == MXL_E_TRUE)
  {
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    if(strstr(buffer, "CTRL"))
    {
      dllStr[0] = '[';
      dllStr[1] = 'D';
      dllStr[2] = 'L';
      dllStr[3] = 'L';
      dllStr[4] = ']';

      strcat(dllStr, buffer);
      strcat(dllStr, "\n");
      OutputDebugString(dllStr);
    }
    else if(strstr(buffer, "MxL"))
    {
      dllStr[0] = '[';
      dllStr[1] = 'M';
      dllStr[2] = 'X';
      dllStr[3] = 'L';
      dllStr[4] = ']';

      strcat(dllStr, buffer);
      strcat(dllStr, "\n");
      OutputDebugString(dllStr);
    }
    else
    {
      strcat(buffer, "\n");
      OutputDebugString(buffer);
    }
  }
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxL_EAGLE_Oem_Debug_StatusToString
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 20/11/2016
*
* \brief          This OEM function convert MXL_STATUS_E to text representation
* 
* \param[in]      status                MXL_STATUS_E to print
* 
* \returns        Pointer to textual representation
*-------------------------------------------------------------------------------------*/
const char* MxL_EAGLE_Oem_Debug_StatusToString(MXL_STATUS_E status)
{
	if (status <= MXL_E_I2C_ERROR)
  {
    return statusDictionary[status].string;
  }
  else
  {
    return "FATAL ERROR! INVALID STATUS CODE";
  }
}

