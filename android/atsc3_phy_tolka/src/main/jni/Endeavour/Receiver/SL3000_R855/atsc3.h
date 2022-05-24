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
/*  File Name   :   atsc3.h                                                  */
/*  version     :   0.2                                                      */
/*  Date        :   29/11/2021                                               */
/*  Description :   Atsc3.0 test code Header File                            */
/*                                                                           */
/*****************************************************************************/

#ifndef _ATSC3_H_
#define _ATSC3_H_

/* Includes */
//#include "sltest.h"
/* Includes */
#include "sl_demod.h"
#include "sl_tuner.h"
#include "sl_config.h"
#include "sl_i2c.h"
//#include "sl_gpio.h"
#include "sl_ts.h"
//#include "sl_log.h"
#include "sl_utils.h"
#include "sl_demod_atsc3.h"
#include "sl_demod_int.h"

//R855 file
#include "R855.h"
#include "i2c_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

	//#include "sl_i2c.h"

	/*
#include "CircularBuffer.h"
#define MAX_BBC_BUFFER_SIZE  (16*1024*12*100)    // Max buffer size when Base band capture enabled
	CircularBuffer            cb;
	unsigned long int         pThread, cThread, dThread;
	void                      *fp;
	int                       tUnit;
	int                       slUnit;
	unsigned char buffer[MAX_BBC_BUFFER_SIZE];
	unsigned int              snrint;
	double                    snr;
	int                       diagLoc = 0;
	int                       processFlag;
	int                       diagFlag;
	double                    fer = 0;
	double                    ber = 0;
	int                       demodStartStatus = 0;
	unsigned long long        llsPlpInfo = 0;
	unsigned long long        llsPlpMask = 0x1;
	int                       plpInfoVal = 0, plpllscount = 0;
	unsigned int              tunerFrequency;
	SL_Atsc3p0Perf_Diag_t     perfDiag;
	SL_Atsc3p0Bsr_Diag_t      bsrDiag;
	SL_Atsc3p0L1B_Diag_t      l1bDiag;
	SL_Atsc3p0L1D_Diag_t      l1dDiag;
	SL_DemodConfigInfo_t      cfgInfo;
	SL_PlpConfigParams_t      plpInfo;
	SL_Atsc3p0Region_t        regionInfo;
	SL_BbCapture_t            setbbValue, getbbValue;
	SL_CmdControlIf_t         cmdIf;
	SL_PlatFormConfigParams_t getPlfConfig;
	SL_TunerDcOffSet_t        tunerIQDcOffSet;
	Endeavour			endeavour;

	int                       calibrationStatus;
	*/
	/* External Interfaces */
	SL_Result_t SL_ATSC3_WriteBytes(unsigned int address, unsigned int lenBytes, void *buf);
	SL_Result_t SL_ATSC3_ReadBytes(unsigned int address, unsigned int lenBytes, void *buf);

	SL_Result_t 	SL3000_atsc3_init(Endeavour  *endeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *sPlfConfig, SL_DemodStd_t std);
	SL_Result_t 	SL3000_atsc3_tune(SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *sPlfConfig);
	//SL_Result_t 	SL3000_atsc3_setPLP(char plpIDNum, char* plpID);
	//SL_Result_t 	SL3000_atsc3_getPLP( LockStatus_t plpIDVaild[4]);
	SL_Result_t SL3000_atsc3_setPLP(char plpMask, char plpID[4]);

	void			Monitor_SL3000_ATSC3_Signal(SignalInfo_t *pSigInfo, int freq, R855_Standard_Type RT_Standard);
	void			SL3000_ATSC3_Initialize(Endeavour	*pEndeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *pPlfConfig, R855_Set_Info *pR855_Info);
	void			SL3000_ATSC3_BindTune(SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *pPlfConfig, R855_Set_Info *pR855_Info);

	void			atsc3_test(Endeavour			*endeavour, SL_TunerConfig_t *pTunerCfg, unsigned int              tunerFrequency);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif