/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/13
  Modification ID : 7843eb97be7f8a319be245f959bc07bb94cf3bf7
------------------------------------------------------------------------------*/
/**
 @file  sony_tunerdemod_monitor.h

 @brief The common tuner and demodulator monitor interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_TUNERDEMOD_MONITOR_H
#define SONY_TUNERDEMOD_MONITOR_H

/*------------------------------------------------------------------------------
  Includes
------------------------------------------------------------------------------*/
#include "sony_common.h"
#include "sony_tunerdemod.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief This function returns the estimated RF level.

        If any compensation for external hardware such as, LNA, attenuators is required,
        then the user can call ::sony_tunerdemod_SetRFLevelCompensation to use
        user defined compensation function.

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt_monitor_RFLevel returns
        main IC value, and sony_tunerdemod_dvbt_monitor_RFLevel_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pRFLeveldB The RF Level estimation in dB * 1000

 @return SONY_RESULT_OK if successful and pRFLeveldB valid.
*/
sony_result_t sony_tunerdemod_monitor_RFLevel (sony_tunerdemod_t * pTunerDemod,
                                               int32_t * pRFLeveldB);

/**
 @brief This function returns the estimated RF level. (Sub IC)

        If any compensation for external hardware such as, LNA, attenuators is required,
        then the user can call ::sony_tunerdemod_SetRFLevelCompensation_sub to use
        user defined compensation function.

 @note  If the driver is configured for diver, sony_tunerdemod_dvbt_monitor_RFLevel returns
        main IC value, and sony_tunerdemod_dvbt_monitor_RFLevel_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param pRFLeveldB The RF Level estimation in dB * 1000

 @return SONY_RESULT_OK if successful and pRFLeveldB valid.
*/
sony_result_t sony_tunerdemod_monitor_RFLevel_sub (sony_tunerdemod_t * pTunerDemod,
                                                   int32_t * pRFLeveldB);

/**
 @brief Monitor the CPU status in CXD2880 IC.

 @note  If the driver is configured for diver, sony_tunerdemod_monitor_InternalCPUStatus returns
        main IC status, and sony_tunerdemod_monitor_InternalCPUStatus_sub returns sub IC status.

 @param pTunerDemod The driver instance.
 @param pStatus The value of CPU status.

 @return SONY_RESULT_OK if successful and pStatus valid.
*/
sony_result_t sony_tunerdemod_monitor_InternalCPUStatus (sony_tunerdemod_t * pTunerDemod,
                                                         uint16_t * pStatus);

/**
 @brief Monitor the CPU status in CXD2880 IC. (Sub IC)

 @note  If the driver is configured for diver, sony_tunerdemod_monitor_InternalCPUStatus returns
        main IC status, and sony_tunerdemod_monitor_InternalCPUStatus_sub returns sub IC status.

 @param pTunerDemod The driver instance.
 @param pStatus The value of CPU status.

 @return SONY_RESULT_OK if successful and pStatus valid.
*/
sony_result_t sony_tunerdemod_monitor_InternalCPUStatus_sub (sony_tunerdemod_t * pTunerDemod,
                                                             uint16_t * pStatus);

/**
 @brief Monitor the TS buffer status.

 @param pTunerDemod The driver instance.
 @param pInfo TS buffer information struct instance.

 @return SONY_RESULT_OK if successful and pStatus valid.
*/
sony_result_t sony_tunerdemod_monitor_TSBufferInfo (sony_tunerdemod_t * pTunerDemod,
                                                    sony_tunerdemod_ts_buffer_info_t * pInfo);

#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_TUNERDEMOD_MONITOR_H */
