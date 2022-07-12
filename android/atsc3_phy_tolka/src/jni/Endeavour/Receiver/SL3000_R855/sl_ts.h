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
/*  File Name   :   sl_ts.h                                                  */
/*  version     :   0.3                                                      */
/*  Date        :   11/08/2021                                               */
/*  Description :   SL Platform API: TS Interface Implementation             */
/*                                                                           */
/*****************************************************************************/

#ifndef _SL_TS_H_
#define _SL_TS_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* Typedefs */
    typedef void(*RxDataCB)(unsigned char *data, int len);

    /* API Funtion*/
    void SL_RxDataStart(RxDataCB dataSecCb);
    int  SL_IsRxDataStarted(void);
    void SL_DemodResetDataEP();
    void SL_RxDataStop();

    /* plf dispatcher method datatypes */
    typedef struct SL_ts_dispatcher_methods
    {
        void(*SL_RxDataStart)(RxDataCB dataSecCb);
        int(*SL_IsRxDataStarted)(void);
        void(*SL_DemodResetDataEP)();
        void(*SL_RxDataStop)();
    } SL_RxDataDispatcherMethods_t;

//	SL_RxDataDispatcherMethods_t slRxDataDispatcherMethods;

#ifdef __cplusplus
}; //extern "C"
#endif

#endif