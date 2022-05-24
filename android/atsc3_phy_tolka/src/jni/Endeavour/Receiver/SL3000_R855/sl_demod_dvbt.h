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
/*  File Name   :   sl_demod_dvbt.h                                          */
/*  version     :   0.1                                                      */
/*  Date        :   28/08/2021                                               */
/*  Description :   SLDemod DVBT API Header File                             */
/*                                                                           */
/*****************************************************************************/

#ifndef _SL_DEMOD_DVBT_H_
#define _SL_DEMOD_DVBT_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* Includes */
#include "sl_demod.h"

    /* Defines */

    /* Typedefs */
    typedef struct
    {
        SL_Bandwidth_t bw;
        SL_CciType_t   ccitype;
    }SL_DvbtConfigParams_t;

    typedef struct
    {
        int SnrLinearScale;
        int Ber;
        int BlockError;
        int FreqOffset;
    }SL_DvbtPerf_Diag_t;

    typedef struct
    {
        int FftMode;
        int GiValue;
        int CodeRate;
        int ModType;
        int TsLockStatus;
    }SL_DvbtL1_Diag_t;

    typedef enum
    {
        SL_DEMOD_LOCK_STATUS_MASK_DVBT_RF_LOCK = (1 << 0)
    }SL_DemodLockStatusMask_DVBT_t;

#ifdef __cplusplus
}; //extern "C"
#endif

#endif
