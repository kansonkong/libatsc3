/*****************************************************************************/
/*  Saankhya Confidential                                                    */
/* COPYRIGHT (C) 2019 Saankhya Labs Pvt. Ltd - All Rights Reserved           */
/*                                                                           */
/* Saankhya Labs makes no warranty express or implied including but not      */
/* limited to, any warranty of merchantability or fitness for a particular   */
/* purpose and/or requirements, for a particular purpose in relation to the  */
/* LICENSED MATERIALS, which is provided “AS IS”, WITH ALL FAULTS.           */
/*                                                                           */
/* Saankhya Labs does not represent or warrant that the LICENSED MATERIALS   */
/* provided    here under is free of infringement of any third party patents,*/
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
/*  File Name   :   sl_utils.h                                               */
/*  version     :   0.5                                                      */
/*  Date        :   29/11/2021                                               */
/*  Description :   SL Platform API: Utils Header File                       */
/*                                                                           */
/*****************************************************************************/

#ifndef _SL_UTILS_H_
#define _SL_UTILS_H_
/*
#define JNIIMPORT
#define JNIEXPORT __attribute__ ((visibility("default")))
#define JNICALL
*/
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Include */
#ifdef _WIN32
#include <Windows.h>
#elif linux
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

#define SL_NULL       (0)

/* FILE SEEK DEFINES */
#define SL_SEEK_CUR   (1)
#define SL_SEEK_END   (2)
#define SL_SEEK_SET   (0)
#include <android/log.h>
#define SL_Printf(...)  __android_log_print(ANDROID_LOG_DEBUG, "RHDSL", __VA_ARGS__)
/* Typedef */

    typedef enum
    {
        SL_UTILS_OK                   = 0,
        SL_UTILS_ERR_OPERATION_FAILED = -1,
        SL_UTILS_ERR_INVALID_ARGS     = -2,
        SL_UTILS_ERR_NOT_SUPPORTED    = -3,
    }SL_UtilsResult_t;

    typedef enum
    {
        SL_THREAD_PRIORITY_LOW = 0,
        SL_THREAD_PRIORITY_NORMAL,
        SL_THREAD_PRIORITY_HIGH
    }SL_ThreadPriority_t;

    typedef struct
    {
        unsigned short year;
        unsigned short month;
        unsigned short day;
        unsigned short hour;
        unsigned short minute;
        unsigned short second;
        unsigned short milliseconds;
    } SL_SystemTime_t;

    typedef int(*SL_ThreadFunction_t)();

    /*Clock/Timer API Functions*/
    void  SL_SleepMS(unsigned long milliSeconds);
    void  SL_SleepUS(unsigned long microSeconds);
    void  SL_GetTime(SL_SystemTime_t *time);

    /* File operation API FunctionS */
    void* SL_FileOpen(const char* fname, const char *mode);
    int   SL_FileRead(void* buffptr, unsigned int numelemnts, unsigned int readlength, void* fptr);
    int   SL_FileWrite(void* buffptr, unsigned int numelemnts, unsigned int readlength, void* fptr);
    int   SL_FileClose(void* fptr);
    int   SL_FileSeek(void* fptr, long int offset, int origin);
    long  SL_FileTell(void *fptr);
    int   SL_FilePrintf(void *fptr, const char *format, ...);

    /* Output screen API Functions */
//    int   SL_Printf(const char * format, ...);
    void  SL_ClearScreen(void);

    /* USB API function*/
    int   SL_GetUsbFd(void);
    void  SL_SetUsbFd(int usbfd);

    /* Thread API FunctionS */
    SL_UtilsResult_t SL_CreateThread(unsigned long int* threadid, SL_ThreadFunction_t func);
    SL_UtilsResult_t SL_SetThreadPriority(unsigned long int* threadid, SL_ThreadPriority_t tprio);
    SL_UtilsResult_t SL_DeleteThread(unsigned long int* threadid);

    /* String Operation */
    char* SL_StrAppend(char* str1, char* str2);
    char* SL_StrCopy(char* destStr, char* srcStr);
    char* SL_StrCat(char* destStr, char* srcStr);

    /* Utils dispatcher method datatypes */
    typedef struct
    {
        void(*SL_SleepMS)(unsigned long milliSeconds);
        void(*SL_SleepUS)(unsigned long microSeconds);
        void(*SL_GetTime)(SL_SystemTime_t *time);
        void* (*SL_FileOpen)(const char* fname, const char *mode);
        int(*SL_FileRead)(void* buffptr, unsigned int numelemnts, unsigned int readlength, void* fptr);
        int(*SL_FileWrite)(void* buffptr, unsigned int numelemnts, unsigned int readlength, void* fptr);
        int(*SL_FileClose)(void* fptr);
        int(*SL_FileSeek)(void* fptr, long int offset, int origin);
        long(*SL_FileTell)(void *fptr);
        int(*SL_FileVprintf)(void *fptr, const char *format, va_list args);
        int(*SL_Vprintf)(const char *, va_list args);
        void(*SL_ClearScreen)(void);
        int(*SL_GetUsbFd)(void);
        void(*SL_SetUsbFd)(int usbfd);
        SL_UtilsResult_t(*SL_CreateThread)(unsigned long int* threadid, SL_ThreadFunction_t func);
        SL_UtilsResult_t(*SL_SetThreadPriority)(unsigned long int* threadid, SL_ThreadPriority_t tprio);
        SL_UtilsResult_t(*SL_DeleteThread)(unsigned long int* threadid);
        char* (*SL_StrAppend)(char* str1, char* str2);
        char* (*SL_StrCopy)(char* destStr, char* srcStr);
        char* (*SL_StrCat)(char* destStr, char* srcStr);
    } SL_UtilsDispatcherMethods_t;

//	__declspec(selectany) SL_UtilsDispatcherMethods_t slUtilsDispatcherMethods;

#ifdef __cplusplus
}; //extern "C"
#endif

#endif
