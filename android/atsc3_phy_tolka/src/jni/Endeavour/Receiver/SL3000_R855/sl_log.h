/*****************************************************************************/
/*  Saankhya Confidential                                                    */
/*  COPYRIGHT (C) 2019 Saankhya Labs Pvt. Ltd - All Rights Reserved          */
/*                                                                           */
/* Saankhya Labs makes no warranty express or implied including but not      */
/* limited to, any warranty of merchantability or fitness for a particular   */
/* purpose and/or requirements, for a particular purpose in relation to the  */
/* LICENSED MATERIALS, which is provided “AS IS? WITH ALL FAULTS.           */
/*                                                                           */
/* Saankhya Labs does not represent or warrant that the LICENSED MATERIALS   */
/* provided here under is free of infringement of any third party patents,   */
/* copyrights, trade secrets or other intellectual property rights.          */
/* ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE EXCLUDED TO  */
/* THE FULLEST EXTENT PERMITTED BY LAW                                       */
/* NOTICE: All information contained herein is, and remains the property of  */
/* Saankhya Labs Pvt. Ltd and its suppliers, if any. The intellectual and    */
/* technical concepts contained herein are proprietary to Saankhya Labs & its*/
/* suppliers and may be covered by U.S. and Foreign Patents, patents in      */
/* process, and are protected by trade secret or copyright law. Dissemination*/
/* of this information or reproduction of this material is strictly forbidden*/
/* unless prior written permission is obtained from Saankhya Labs Pvt Ltd.   */
/*                                                                           */
/*  File Name   :   sl_log.h                                                 */
/*  version     :   0.4                                                      */
/*  Date        :   29/11/2021                                               */
/*  Description :   SL Platform API: Log Header File                         */
/*                                                                           */
/*****************************************************************************/

#ifndef _SL_LOG_H_
#define _SL_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* Includes */
#include "sl_utils.h"

/* Typedefs */
    typedef enum
    {
        SL_LOGTYPE_INFO  = 0x01,
        SL_LOGTYPE_WARN  = 0x02,
        SL_LOGTYPE_ERROR = 0x04
    }SL_LogType_t;

    typedef enum
    {
        SL_LOGLEVEL_INFO     = 0x01,
        SL_LOGLEVEL_WARNINGS = 0x02,
        SL_LOGLEVEL_ERROR    = 0x04,
        SL_LOGLEVEL_ALL      = 0xFF
    }SL_LogLevel_t;

    typedef enum
    {
        SL_LOG_OK   = 0,
        SL_LOG_FAIL = -1
    }SL_LogResult_t;

    /* External Interfaces */
    SL_LogResult_t SL_LogInit(void);
    void SL_LogSetLevel(SL_LogLevel_t level);
    void SL_Log(SL_LogType_t type, char *msg);

    /* Log dispatcher method datatypes */
    typedef struct
    {
        SL_LogResult_t(*SL_LogInit)();
        void(*SL_LogSetLevel)(SL_LogLevel_t level);
        void(*SL_Log)(SL_LogType_t type, char *msg);
    } SL_LogDispatcherMethods_t;

//	SL_LogDispatcherMethods_t slLogDispatcherMethods;

#ifdef __cplusplus
}; //extern "C"
#endif

#endif