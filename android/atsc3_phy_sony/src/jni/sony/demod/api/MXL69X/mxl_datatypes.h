 /*****************************************************************************************   
 * FILE NAME          : mxl_datatypes.h
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 14/Nov/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains MaxLinear-defined data types.
 *                      Instead of using ANSI C data type directly in source code
 *                      All module should include this header file.
 *                      And conditional compilation switch is also declared
 *                      here.
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

/*!***************************************************************************************
 *  \addtogroup api
 *  @{
 *  \addtogroup common
 *  @{
 ****************************************************************************************/

#ifndef __MXL_DATATYPES_H__
#define __MXL_DATATYPES_H__


#ifndef _MSC_VER 
  #ifdef __KERNEL__
    #include <linux/types.h>
  #else
    #include <stdint.h>
  #endif
#endif


/*****************************************************************************************
    Defines
 ****************************************************************************************/
#define MXL_MIN(x, y)             (((x) < (y))  ? (x) : (y))
#define MXL_MAX(x, y)             (((x) >= (y)) ? (x) : (y))
#define MXL_ABS(x)                (((x) >= 0)   ? (x) : -(x))
#define MXL_DIV_ROUND(x, y)       (((y) != 0)   ? (((x) + ((y)/2)) / (y)) : 0)
#define MXL_DIV_ROUND_S(x, y)     (((y) != 0)   ? (((x) + ((((x) >= 0) ? 1 : -1) * (y)/2)) / (y)) : 0)    // MXL_DIV_ROUND_S - use only for signed types

#ifndef NULL
  #ifdef __cplusplus
    #define NULL                  0
  #else
    #define NULL                  ((void*)0)
  #endif
#endif

#ifndef __PACK__
  #define __PACK__
#endif

#ifndef __PACK_PREFIX__
  #define __PACK_PREFIX__
#endif


/*****************************************************************************************
    Typedefs
 ****************************************************************************************/
/* Basic datatypes are defined below. 
 * If you do not wish to use them, or you already have
 * them defined, please add MXL_DO_NOT_DEFINE_BASIC_DATATYPES
 * to compilation defines
 */
#ifndef MXL_DO_NOT_DEFINE_BASIC_DATATYPES
  #ifdef _MSC_VER
    #ifndef __midl
      typedef unsigned __int8     UINT8;
      typedef unsigned __int16    UINT16;
      typedef unsigned __int32    UINT32;
      typedef unsigned __int64    UINT64;

      typedef __int8              SINT8;
      typedef __int16             SINT16;
    #else
      typedef small               SINT8;
      typedef short               SINT16;
    #endif

    typedef __int32               SINT32;
    typedef __int64               SINT64;
  #else
    typedef uint8_t               UINT8;
    typedef uint16_t              UINT16;
    typedef uint32_t              UINT32;
    typedef uint64_t              UINT64;

    typedef int8_t                SINT8;
    typedef int16_t               SINT16;
    typedef int32_t               SINT32;
    typedef int64_t               SINT64;

  #endif

  typedef float                   REAL32;
  typedef double                  REAL64;
#endif


#ifndef __MXL_WIN__
  #undef  __stdcall
  #define __stdcall
#endif // __MXL_WIN__


#define GETBYTE(x, n)             ((UINT8)(x >> (8*(n-1))) & 0xff)

typedef UINT32 (__stdcall *MXL_CALLBACK_FN_T)(UINT8 devId, UINT32 callbackType, void * callbackPayload);


/*! Enum of error codes, This definition is identical across MXL projects */
typedef enum 
{
  MXL_E_SUCCESS,                //!< =0 Successful execution
  MXL_E_FAILURE,                //!< =1 The function failed due to communication errors with the device
  MXL_E_INVALID_PARAMETER,      //!< =2 One or more parameters are invalid. The function did not perform any operation.
  MXL_E_NOT_INITIALIZED,        //!< =3 The device context memory is not initialized. The init function was not called or failed due to errors
  MXL_E_ALREADY_INITIALIZED,    //!< =4 Init function has been called more than once
  MXL_E_NOT_SUPPORTED,          //!< =5 Requested operation cannot be performed because the hardware or feature required is not supported. The function did not perform any operation.
  MXL_E_NOT_READY,              //!< =6 Requested data or feature is not ready at this moment. 
  MXL_E_DATA_ERROR,             //!< =7 Integrity error of provided or read data
  MXL_E_I2C_ERROR,				
} MXL_STATUS_E;

/*! Enum of boolean, This definition is identical across MXL projects */
typedef enum 
{
  MXL_E_DISABLE = 0,
  MXL_E_ENABLE  = 1,  
        
  MXL_E_FALSE   = 0,
  MXL_E_TRUE    = 1,  

  MXL_E_INVALID = 0,
  MXL_E_VALID   = 1,

  MXL_E_NO      = 0,
  MXL_E_YES     = 1,  
  
  MXL_E_OFF     = 0,
  MXL_E_ON      = 1,
} MXL_BOOL_E;

/** @} common */
/** @} api */

#endif  /* __MXL_DATATYPES_H__ */
