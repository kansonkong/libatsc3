/*****************************************************************************/
/*  Saankhya Confidential                                                    */
/*  COPYRIGHT (C) 2020 Saankhya Labs Pvt. Ltd - All Rights Reserved          */
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
/*  File Name   :   sl_demod_int.h                                           */
/*  version     :   0.5                                                      */
/*  Date        :   28/08/2021                                               */
/*  Description :   Contains SLDemod Internal Types                          */
/*                                                                           */
/*****************************************************************************/

#ifndef _SL_DEMOD_INT_H_
#define _SL_DEMOD_INT_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* Includes */
#include "sl_demod.h"
#include "sl_demod_atsc1.h"
#include "sl_demod_atsc3.h"
//#include "sl_demod_dvbt.h"
//#include "sl_demod_dvbt2.h"
//#include "sl_demod_isdbt.h"
#include "sl_demod_regs.h"
#include "sl_config.h"
/* Defines */
#define SL_DEMOD_MAX_INSTANCES     (1)
#define SL_DEMOD_REG_SIZE          (0x04)             /* Size in Bytes */

/* Typedef */
    typedef enum
    {
        SL_DEMOD_TRUE = 1,
        SL_DEMOD_FALSE = 0
    }SL_DemodBool_t;

    typedef struct
    {
        volatile SL_DemodBool_t    isCreated;
        volatile SL_DemodBool_t    isInitialized;
        volatile SL_DemodBool_t    isConfigured;
        volatile SL_DemodBool_t    isExConfigured;
        volatile SL_DemodBool_t    isStarted;
        SL_CmdControlIf_t comPrtcl;
        SL_DemodStd_t     std;
        char              *hexFilePath;
        unsigned int      baseAddr;
        unsigned char     iccmCmdInfo[16];
        unsigned char     dccmCmdInfo[16];
        unsigned char     executeCmdInfo[16];
        char              *iccmFile;
        char              *dccmFile;
        char              *secondaryFile;
        double             fAdc;
        SL_PlatFormConfigParams_t  boardInfo;
    } SL_DemodBlock_t;

#ifdef __cplusplus
}; //extern "C"
#endif

#endif