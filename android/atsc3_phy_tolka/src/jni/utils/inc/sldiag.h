/*****************************************************************************/
/*	Saankhya Confidential													 */
/*	COPYRIGHT (C) 2019 Saankhya Labs Pvt. Ltd - All Rights Reserved			 */
/*																			 */
/* Saankhya Labs makes no warranty express or implied including but not		 */
/* limited to, any warranty of merchantability or fitness for a particular	 */
/* purpose and/or requirements, for a particular purpose in relation to the  */
/* LICENSED MATERIALS, which is provided �AS IS�, WITH ALL FAULTS.			 */
/*																			 */
/* Saankhya Labs does not represent or warrant that the LICENSED MATERIALS	 */
/* provided	here under is free of infringement of any third party patents,	 */
/* copyrights, trade secrets or other intellectual property rights.			 */
/* ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE EXCLUDED TO  */
/* THE FULLEST EXTENT PERMITTED BY LAW										 */
/* NOTICE: All information contained herein is, and remains the property of  */
/* Saankhya Labs Pvt. Ltd and its suppliers, if any. The intellectual and	 */
/* technical concepts contained herein are proprietary to Saankhya Labs & its*/
/* suppliers and may be covered by U.S. and Foreign Patents, patents in		 */
/* process, and are protected by trade secret or copyright law. Dissemination*/
/* of this information or reproduction of this material is strictly forbidden*/
/* unless prior written permission is obtained from Saankhya Labs Pvt Ltd.   */
/*																			 */
/*	File Name	:	sldiag.c								                 */
/*  version		: 	0.1										              	 */
/*  Date		:	13/08/2019												 */
/*  Author		:	Thrimurthi M									     	 */
/*  Description :   SLSDK Diagnostic Parameter print Header File			 */
/*																			 */
/*****************************************************************************/

/* Include */
#include "CircularBuffer.h"
#include "sl_demod.h"
#include "sl_tuner.h"
#include "sl_plf.h"

//FILE *diagFp;

/* External functions */
void printAtsc3PerfDiagnostics(SL_Atsc3p0Perf_Diag_t diag, int diagLoc);
void printAtsc3BsrDiagnostics(SL_Atsc3p0Bsr_Diag_t diag, int diagLoc);
void printAtsc3L1bDiagnostics(SL_Atsc3p0L1B_Diag_t diag, int diagLoc);
void printAtsc3L1dDiagnostics(SL_Atsc3p0L1D_Diag_t diag, int diagLoc);
