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
/*  File Name   :   sl_utils.c                                               */
/*  version     :   0.5                                                      */
/*  Date        :   27/08/2021                                               */
/*  Description :   SL Platform API: Utils Implementation                    */
/*                                                                           */
/*****************************************************************************/

#include"sl_utils.h"

static int usbFd;
SL_UtilsDispatcherMethods_t slUtilsDispatcherMethods;

void SL_SleepMS(unsigned long milliSeconds)
{
#ifdef _WIN32
    Sleep(milliSeconds); //sleep in milli Seconds
#elif linux
    usleep(milliSeconds * 1000);  //sleep in milli Seconds
#endif
}

void SL_SleepUS(unsigned long microSeconds)
{
#ifdef _WIN32
    Sleep(microSeconds / 1000); //sleep in micro Seconds
#elif linux
    usleep(microSeconds);  //sleep in micro Seconds
#endif
}

void SL_GetTime(SL_SystemTime_t *time)
{
#ifdef _WIN32
    SYSTEMTIME st;
    GetLocalTime(&st);
    time->milliseconds = st.wMilliseconds;
    time->second = st.wSecond;
    time->minute = st.wMinute;
    time->hour = st.wHour;
    time->day = st.wDay;
    time->month = st.wMonth;
    time->year = st.wYear;
#elif linux
    // Add linux Get time funtion
    struct tm *st;
    time_t now = 0;
    st = localtime(&now);
    time->milliseconds = st->tm_sec;
    time->second = st->tm_sec;
    time->minute = st->tm_min;
    time->hour = st->tm_hour;
    time->day = st->tm_mday;
    time->month = st->tm_mon;
    time->year = st->tm_year;
#endif
}

void* SL_FileOpen(const char* fname, const char *mode)
{
    FILE *fptr;
    fptr = fopen(fname, mode);
    if (fptr == NULL)  // checking if the file exist or not 
    {
        SL_Printf("\n Error: failed to open file");
    }
    return (void*)fptr;
}

int SL_FileRead(void* buffptr, unsigned int numelemnts, unsigned int readlength, void* fptr)
{
    int bytesRead = fread(buffptr, numelemnts, readlength, (FILE*)fptr);
    return bytesRead;
}
int SL_FileWrite(void* buffptr, unsigned int numelemnts, unsigned int readlength, void* fptr)
{
    int bytesWrote = fwrite(buffptr, numelemnts, readlength, (FILE*)fptr);

    return bytesWrote;
}

int SL_FileClose(void* fptr)
{
    int retVal = fclose((FILE*)fptr);

    return retVal;
}

int SL_FileSeek(void* fptr, long int offset, int origin)
{
    int retVal = fseek((FILE*)fptr, offset, origin);

    return retVal;
}

long SL_FileTell(void *fptr)
{
    return ( ftell( (FILE*)fptr ) );
}

int SL_FilePrintf(void *fptr, const char *format, ...)
{
#if defined _WIN32 || defined linux
    int retVal;
    va_list args;
    va_start(args, format);
    retVal = vprintf(format, args);
    va_end(args);
    return retVal;
#endif
}

SL_UtilsResult_t SL_CreateThread(unsigned long int* threadid, SL_ThreadFunction_t func)
{
#ifdef _WIN32
    HANDLE threadhdl = CreateThread(NULL, 3000000, (LPTHREAD_START_ROUTINE)func, 0, 0, NULL);
    if (!threadhdl)
    {
        return SL_UTILS_ERR_OPERATION_FAILED;
    }
    else
    {
        SetThreadPriority(threadhdl, THREAD_PRIORITY_NORMAL);
        *(unsigned long int *)threadid = (unsigned long int)threadhdl; //(int)threadhdl;
        return SL_UTILS_OK;
    }
#elif linux
 /*   int retVal = pthread_create((pthread_t*)threadid, NULL, (void *)func, NULL);
    if (retVal != 0)
    {
        return SL_UTILS_ERR_OPERATION_FAILED;
    }
    else
    {
        return SL_UTILS_OK;
    }*/
#endif
}

SL_UtilsResult_t SL_SetThreadPriority(unsigned long int* threadid, SL_ThreadPriority_t tprio)
{
#ifdef _WIN32
    HANDLE threadhdl;
    int threadpriority;
    BOOL retVal;

    if (tprio == SL_THREAD_PRIORITY_LOW)
        threadpriority = THREAD_PRIORITY_LOWEST;
    else if (tprio == SL_THREAD_PRIORITY_NORMAL)
        threadpriority = THREAD_PRIORITY_NORMAL;
    else if (tprio == SL_THREAD_PRIORITY_HIGH)
        threadpriority = THREAD_PRIORITY_HIGHEST;
    else
        return SL_UTILS_ERR_INVALID_ARGS;

    threadhdl = (HANDLE)*(unsigned long int*)threadid;
    retVal = SetThreadPriority(threadhdl, threadpriority);
    if (retVal)
    {
        return SL_UTILS_OK;
    }
    else
    {
        return SL_UTILS_ERR_OPERATION_FAILED;
    }

#elif linux
    return SL_UTILS_OK;
#endif
}

SL_UtilsResult_t SL_DeleteThread(unsigned long int* threadid)
{
#ifdef _WIN32
    ExitThread((DWORD)threadid);
    return SL_UTILS_OK;
#elif linux
    pthread_exit(threadid);
    return SL_UTILS_OK;
#endif
}

//int SL_Printf(const char *format, ...)
//{
//#if defined _WIN32 || defined linux
//    int retVal;
//    va_list args;
//    va_start(args, format);
//    retVal = vprintf(format, args);
//    va_end(args);
//    return retVal;
//#else
//    printf(format);
//#endif
//
//}

void SL_ClearScreen(void)
{
#ifdef _WIN32
    //  system("mode con COLS=700");
    //  ShowWindow(GetConsoleWindow(), SW_NORMAL);
    system("cls");
#elif linux
    if (system("clear") == 0);/*clear output screen*/
#endif
}

int SL_GetUsbFd(void)
{
    return usbFd;
}

void SL_SetUsbFd(int usbfd)
{
    usbFd = usbfd;
}

char* SL_StrAppend(char *str1, char *str2)
{
    char *str3 = (char*)malloc(1 + strlen(str1) + strlen(str2));
    strcpy(str3, str1);
    strcat(str3, str2);
    return str3;
}

char* SL_StrCat(char* destStr, char* srcStr)
{
    if (destStr != NULL && srcStr != NULL)
    {
        return slUtilsDispatcherMethods.SL_StrCat(destStr, srcStr);
    }
    return NULL;
}

