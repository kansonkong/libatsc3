/*****************************************************************************/
/*  Saankhya Confidential                                                    */
/*  COPYRIGHT (C) 2019 Saankhya Labs Pvt. Ltd - All Rights Reserved          */
/*                                                                           */
/* Saankhya Labs makes no warranty express or implied including but not      */
/* limited to, any warranty of merchantability or fitness for a particular   */
/* purpose and/or requirements, for a particular purpose in relation to the  */
/* LICENSED MATERIALS, which is provided �AS IS�, WITH ALL FAULTS.           */
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
/*  File Name   :   sl_log.c                                                 */
/*  version     :   0.4                                                      */
/*  Date        :   09/07/2021                                               */
/*  Description :   SL Platform API: Log Implementation                      */
/*                                                                           */
/*****************************************************************************/
#include "sl_log.h"
#include "sl_utils.h"

#define logfilename  "log.txt"
#define LOG_INITIALISED    ( 1 )
#define LOG_NOT_INITALISED ( 0 )

//void *logfp;
SL_SystemTime_t st;
SL_LogLevel_t logLevel = SL_LOGLEVEL_ALL;
//int logInit = LOG_NOT_INITALISED;
SL_LogDispatcherMethods_t slLogDispatcherMethods;

SL_LogResult_t SL_LogInit(void)
{/*
    logfp = SL_FileOpen(logfilename, "w");
    if (logfp == NULL)
    {
        SL_Printf("ERROR: Log File Failed to Open\n");
        return SL_LOG_FAIL;
    }
    else
    {
        logInit = LOG_INITIALISED;
    }
    SL_FileClose(logfp);
    //fclose(logfp);*/
    return SL_LOG_OK;
}

void SL_LogSetLevel(SL_LogLevel_t level)
{
    logLevel = level;
}

void SL_Log(SL_LogType_t type, char *msg)
{
    char logMsg[250] = "";
	SL_GetTime(&st);
	sprintf(logMsg, "%02d/%02d/%d  %02d:%02d:%02d:%03d\t%s", st.day, st.month, st.year, st.hour, st.minute, st.second, st.milliseconds, msg);

	printf("[SL_Log]%s",logMsg);
}
