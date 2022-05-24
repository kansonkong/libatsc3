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
/*  File Name   :   atsc1.h                                                  */
/*  version     :   0.2                                                      */
/*  Date        :   29/11/2021                                               */
/*  Description :   Atsc1 test code Header File                              */
/*                                                                           */
/*****************************************************************************/

#ifndef _ATSC1_H_
#define _ATSC1_H_
#include "sl_config.h"
#include "sl_demod.h"
#include "sl_tuner.h"
#include "sl_demod_atsc1.h"
#include "sl_i2c.h"
#include "sl_ts.h"
#include "sl_utils.h"
#include "sl_demod_dvbt.h"
//#include "sl_demod_int.h"
#include "sl_demod_int.h"


//R855 file
#include "R855.h"
#include "i2c_sys.h"
#include "sl_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

	/* Includes */
	//#include "sltest.h"
	SL_Result_t SL_ATSC1_WriteBytes(unsigned int address, unsigned int lenBytes, void *buf);
	SL_Result_t SL_ATSC1_ReadBytes(unsigned int address, unsigned int lenBytes, void *buf);




	/* External Interfaces */
	SL_Result_t 	SL3000_atsc1_init(Endeavour  *endeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *sPlfConfig, SL_DemodStd_t std);
	SL_Result_t 	SL3000_atsc1_tune(Endeavour  *endeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *sPlfConfig);



	void 			Monitor_SL3000_ATSC1_Signal(SignalInfo_t *pSigInfo, int freq, R855_Standard_Type RT_Standard);
	void 			SL3000_ATSC1_Initialize(Endeavour	*pEndeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *pPlfConfig, R855_Set_Info *pR855_Info);
	void 			SL3000_ATSC1_BindTune(Endeavour  *pEndeavour, SL_TunerConfig_t *pTunerCfg, SL_PlatFormConfigParams_t *pPlfConfig, R855_Set_Info *pR855_Info);

	void atsc1_test(Endeavour	*endeavour, SL_TunerConfig_t *pTunerCfg, unsigned int              tunerFrequency);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif