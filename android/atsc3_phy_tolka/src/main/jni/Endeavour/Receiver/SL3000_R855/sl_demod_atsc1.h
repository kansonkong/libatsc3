/*************************************************************************** */
/* Saankhya Confidential                                                     */
/* COPYRIGHT (C) 2020 Saankhya Labs Pvt. Ltd - All Rights Reserved           */
/*                                                                           */
/* Saankhya Labs makes no warranty express or implied including but not      */
/* limited to, any warranty of merchantability or fitness for a particular   */
/* purpose and/or requirements, for a particular purpose in relation to the  */
/* LICENSED MATERIALS, which is provided “AS IS”, WITH ALL FAULTS.           */
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
/*  File Name   :   sl_demod_atsc1.h                                         */
/*  version     :   0.2                                                      */
/*  Date        :   25/03/2022                                               */
/*  Description :   SLDemod ATSC1 API Header File                             */
/*                                                                           */
/*****************************************************************************/

#ifndef _SL_DEMOD_ATSC1_H_
#define _SL_DEMOD_ATSC1_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* Includes */
#include "sl_demod.h"

    /* Defines */

    /* Typedefs */
    typedef enum
    {
        SL_DEMOD_LOCK_STATUS_MASK_ATSC1p0_RF_LOCK = (1 << 0)
    }SL_DemodLockStatusMask_Atsc1p0_t;

    typedef struct
    {
        SL_Bandwidth_t bw;
        unsigned int blockSize;
    }SL_Atsc1p0ConfigParams_t;

    typedef struct
    {
        int PreEquSNR;
        int PfeDone;
        int PostEquSNR;
        int EquLockFlag;
        int BitErrorCnt;
        int BlockErrorCnt;
        int TotalTxBitCnt;
        int TotalTxBlockCnt;
        int DspInternalState;
        int DopplerState;
        int Atsc1SignalDetectField;
        int BbRms;
        int AgcGain;
    }SL_Atsc1p0Perf_Diag_t;

#ifdef __cplusplus
}; //extern "C"
#endif

#endif
