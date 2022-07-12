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
/*  File Name   :   sl_demod_atsc3.h                                         */
/*  version     :   0.1                                                      */
/*  Date        :   28/08/2021                                               */
/*  Description :   SLDemod ATSC3 API Header File                            */
/*                                                                           */
/*****************************************************************************/

#ifndef _SL_DEMOD_ATSC3_H_
#define _SL_DEMOD_ATSC3_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* Includes */
#include "sl_demod.h"

    /* Defines */

    /* Typedefs */
    typedef enum
    {
        SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_RF_LOCK      = (1 << 0),
        SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1B_LOCK     = (1 << 1),
        SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_L1D_LOCK     = (1 << 2),
        // Reserved
        SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP0_LOCK = (1 << 4),
        SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP1_LOCK = (1 << 5),
        SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP2_LOCK = (1 << 6),
        SL_DEMOD_LOCK_STATUS_MASK_ATSC3P0_BB_PLP3_LOCK = (1 << 7)
    }SL_DemodLockStatusMask_Atsc3p0_t;

    typedef struct
    {
        char plp0;
        char plp1;
        char plp2;
        char plp3;
    }SL_Atsc3p0PlpConfigParams_t;

    typedef enum
    {
        SL_ATSC3P0_REGION_US = 0,
        SL_ATSC3P0_REGION_KOREA
    }SL_Atsc3p0Region_t;

    typedef struct
    {
        SL_Atsc3p0PlpConfigParams_t plpConfig;
        SL_Atsc3p0Region_t          region;
    }SL_Atsc3p0ConfigParams_t;

    typedef unsigned long long SL_Atsc3p0LlsPlpInfo_t;

    typedef struct
    {
        int NumFrameErrL1b;
        int NumFrameErrL1d;
        int NumFrameErrPlp0;
        int NumFrameErrPlp1;
        int NumFrameErrPlp2;
        int NumFrameErrPlp3;
        int NumBitErrL1b;
        int NumBitErrL1d;
        int NumBitErrPlp0;
        int NumBitErrPlp1;
        int NumBitErrPlp2;
        int NumBitErrPlp3;
        int LdpcItrnsL1b;
        int LdpcItrnsL1d;
        int LdpcItrnsPlp0;
        int LdpcItrnsPlp1;
        int LdpcItrnsPlp2;
        int LdpcItrnsPlp3;
        int NumFecFrameL1b;
        int NumFecFrameL1d;
        int NumFecFramePlp0;
        int NumFecFramePlp1;
        int NumFecFramePlp2;
        int NumFecFramePlp3;
        int NumFecBitsL1b;
        int NumFecBitsL1d;
        int NumFecBitsPlp0;
        int NumFecBitsPlp1;
        int NumFecBitsPlp2;
        int NumFecBitsPlp3;
        int L1bSnrLinearScale;
        int L1dSnrLinearScale;
        int Plp0SnrLinearScale;
        int Plp1SnrLinearScale;
        int Plp2SnrLinearScale;
        int Plp3SnrLinearScale;
        int GlobalSnrLinearScale;
        int L1SampePerFrame;
        int L1FrameCount;
        int Plp0StreamByteCount;
        int Plp1StreamByteCount;
        int Plp2StreamByteCount;
        int Plp3StreamByteCount;
        int Plp0ChannelByteCount;
        int Plp1ChannelByteCount;
        int Plp2ChannelByteCount;
        int Plp3ChannelByteCount;
    }SL_Atsc3p0Perf_Diag_t;

    typedef struct
    {
        int Bsr0MajVer;
        int Bsr0MinVer;
        int Bsr1EAWakeup1;
        int Bsr1MinTimeToNxt;
        int Bsr1SysBw;
        int Bsr1EAWakeup2;
        int Bsr2Coeff;
        int Bsr3PreambleStr;
    }SL_Atsc3p0Bsr_Diag_t;

    typedef struct
    {
        int L1bVersion;
        int L1bMimoScatPilotEnc;
        int L1bLlsFlag;
        int L1bTimeInfoFlag;
        int L1b_ReturnChannelFlag;
        int L1bPaprReduction;
        int L1bFrameLengthMode;
        int L1bFrameLength;
        int L1bExcessSamplesPerSym;
        int L1bTimeOffset;
        int L1bAdditionalSamples;
        int L1bNoOfSubframes;
        int L1bPreambleNumOfSym;
        int L1bPreambleReducedCarriers;
        int L1bL1DetailContentTag;
        int L1bL1detailSizeInBytes;
        int L1bL1detailFecType;
        int L1bL1detailApMode;
        int L1bL1detailTotalCells;
        int L1bFirstSfMimo;
        int L1bFirstSfMiso;
        int L1bFirstSfFftSize;
        int L1bFirstSfReducedCarriers;
        int L1bFirstSfGi;
        int L1bFirstSfNumOfdmSym;
        int L1bFirstSfScatPilotPattern;
        int L1bFirstSfScatPilotBoost;
        int L1bFirstSubSbsFirst;
        int L1bFirstSubSbsLast;
    }SL_Atsc3p0L1B_Diag_t;

    typedef struct
    {
        int L1dVersion;
        int L1dNumRf;
        int L1dRfFrequency;
        int Reserved;
        int L1dTimeSec;
        int L1dTimeMsec;
        int L1dTimeUsec;
        int L1dTimeNsec;
    }SL_Atsc3p0L1DGlobal_Diag_t;

    typedef struct
    {
        int L1dSfPlpId;
        int L1dSfPlpLls_Flag;
        int L1dSfPlpLayer;
        int L1dSfPlpStart;
        int L1dSfPlpSize;
        int L1dSfPlpScrambleType;
        int L1dSfPlpFecType;
        int L1dSfPlpModType;
        int L1dSfPlpCoderate;
        int L1dSfPlpTiMode;
        int L1dSfPlpFecBlkStart;
        int L1dSfPlpCtiFecBlkStart;
        int L1dSfPlpNumChBonded;
        int L1dSfPlpChBondingFmt;
        int L1dSfPlpChBondedRfid;
        int L1dSfPlpMimoStrComb;
        int L1dSfPlpMimoIqInterleave;
        int L1dSfPlpMimoPh;
        int L1dSfPlpType;
        int L1dSfPlpNumSubslices;
        int L1dSfPlpSubslicesInterval;
        int L1dSfPlpTiExtInterleave;
        int L1dSfPlpCtiDepth;
        int L1dSfPlpCtiStartRow;
        int L1dSfPlpHtiInterSubframe;
        int L1dSfPlpHtiNumTiBlks;
        int L1dSfPlpHtiNumFecblksMax;
        int L1dSfPlpHtiNumFecblksTi0;
        int L1dSfPlpHtiNumFecblksTi1;
        int L1dSfPlpHtiNumFecblksTi2;
        int L1dSfPlpHtiNumFecblksTi3;
        int L1dSfPlpHtiNumFecblksTi4;
        int L1dSfPlpHtiNumFecblksTi5;
        int L1dSfPlpHtiNumFecblksTi6;
        int L1dSfPlpHtiNumFecblksTi7;
        int L1dSfPlpHtiNumFecblksTi8;
        int L1dSfPlpHtiNumFecblksTi9;
        int L1dSfPlpHtiNumFecblksTi10;
        int L1dSfPlpHtiNumFecblksTi11;
        int L1dSfPlpHtiNumFecblksTi12;
        int L1dSfPlpHtiNumFecblksTi13;
        int L1dSfPlpHtiNumFecblksTi14;
        int L1dSfPlpHtiNumFecblksTi15;
        int L1dSfPlpHtiCellInterleaver;
        int L1dSfPlpLdmInjLevel;
        /* Internal implementation specific : Derived parameters */
        int L1dSfPlpDispersedPerOfdmSize;
        int L1dPlpFecBlockStartOffset;
    }SL_Atsc3p0L1DPlp_Diag_t;

    typedef struct
    {
        int L1dSfMimo;
        int L1dSfMiso;
        int L1dSfFftSize;
        int L1dSfReducedCarriers;
        int L1dSfGi;
        int L1dSfNumOfdmSym;
        int L1dSfScatPilotPattern;
        int L1dSfScatPilotBoost;
        int L1dSfSbsFirst;
        int L1dSfSbsLast;
        int L1dSfMultiplex;
        int L1dSfFreqInterleaver;
        int L1dSfSbsNullCells;
        int L1dSfNumPlps;
        SL_Atsc3p0L1DPlp_Diag_t PlpParams[4];
        //Internal implementation specific : Derived parameters
        unsigned int L1dSfNumPlp2Decode;
    }SL_Atsc3p0L1DSubFrame_Diag_t;

    typedef struct
    {
        SL_Atsc3p0L1DGlobal_Diag_t   l1dGlobalParamsStr;
        SL_Atsc3p0L1DSubFrame_Diag_t sfParams[4];
    }SL_Atsc3p0L1D_Diag_t;

#ifdef __cplusplus
}; //extern "C"
#endif

#endif
