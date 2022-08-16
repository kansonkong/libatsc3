/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/08/01
  Modification ID : b30d76210d343216ea52b88e9b450c8fd5c0359f
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd_integ_isdbt.h

          This file provides the integration layer interface for ISDB-T
          specific demodulator functions.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_INTEG_ISDBT_H
#define sony_cxd_INTEG_ISDBT_H

#include "sony_cxd_demod.h"
#include "sony_cxd_integ.h"
#include "sony_cxd_demod_isdbt.h"

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
#define sony_cxd_ISDBT_WAIT_DEMOD_LOCK           1500    /**< 1.5 s timeout for wait demodulator lock process for ISDB-T channels */
#define sony_cxd_ISDBT_WAIT_TS_LOCK              1500    /**< 1.5 s timeout for wait TS lock process for ISDB-T channels */
#define sony_cxd_ISDBT_WAIT_LOCK_INTERVAL        10      /**< 10ms polling interval for demodulator and TS lock functions */

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The parameters used for ISDB-T  scanning.
*/
typedef struct sony_cxd_integ_isdbt_scan_param_t {
    /**
     @brief The start frequency in kHz for scanning.  Ensure that this is
            aligned with the channel raster.
    */
    uint32_t startFrequencyKHz;

    /**
     @brief The end frequency in kHz for scanning.
    */
    uint32_t endFrequencyKHz;

    /**
     @brief The step frequency in kHz for scanning.
    */
    uint32_t stepFrequencyKHz;
    /**
     @brief The bandwidth to use for tuning during the scan
    */
	sony_cxd_dtv_bandwidth_t bandwidth;
} sony_cxd_integ_isdbt_scan_param_t;

/**
 @brief The structure used to return a channel located or progress update
        as part of a ISDB-T scan.
*/
typedef struct sony_cxd_integ_isdbt_scan_result_t {
    /**
     @brief Indicates the current frequency just attempted for the scan.  This would
            primarily be used to calculate scan progress from the scan parameters.
    */
    uint32_t centerFreqKHz;

    /**
     @brief Indicates if the tune result at the current frequency.  sony_cxd_RESULT_OK
            means that a channel has been locked and one of the tuneParam structures
            contain the channel infomration.
    */
	sony_cxd_result_t tuneResult;

    /**
     @brief The tune params for a located ISDB-T channel.
    */
	sony_cxd_isdbt_tune_param_t tuneParam;
} sony_cxd_integ_isdbt_scan_result_t;

/*------------------------------------------------------------------------------
 Function Pointers
------------------------------------------------------------------------------*/
/**
 @brief Callback function that is called for every attempted frequency during a
        scan. For successful channel results the function is called after
        demodulator Lock, but before TS lock is achieved.

 @param pInteg The driver instance.
 @param pResult The current scan result.
 @param pScanParam The current scan parameters.
*/
typedef void(*sony_cxd_integ_isdbt_scan_callback_t) (sony_cxd_integ_t * pInteg,
	sony_cxd_integ_isdbt_scan_result_t * pResult,
	sony_cxd_integ_isdbt_scan_param_t * pScanParam);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Performs acquisition to a ISDB-T channel.
        Blocks the calling thread until the TS has locked or has timed out.
        Use ::sony_cxd_integ_Cancel to cancel the operation at any time.

 @param pInteg The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return sony_cxd_RESULT_OK if tuned successfully to the channel.
*/
sony_cxd_result_t sony_cxd_integ_isdbt_Tune(sony_cxd_integ_t * pInteg,
	sony_cxd_isdbt_tune_param_t * pTuneParam);


/**
 @brief Performs acquisition to a ISDB-T channel but in EWS mode.
        In EWS mode, TS is not output and the demod waiting EWS flag.
        Blocks the calling thread until the Demod has locked or has timed out.
        Use ::sony_cxd_integ_Cancel to cancel the operation at any time.

 @param pInteg The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return sony_cxd_RESULT_OK if tuned successfully to the channel.
*/
sony_cxd_result_t sony_cxd_integ_isdbt_EWSTune(sony_cxd_integ_t * pInteg,
	sony_cxd_isdbt_tune_param_t * pTuneParam);

/**
 @brief Performs a scan over the spectrum specified.

        The scan can perform a scan for ISDB-T channels.

        Blocks the calling thread while scanning. Use ::sony_cxd_integ_Cancel to cancel
        the operation at any time.

 @param pInteg The driver instance.
 @param pScanParam The scan parameters.
 @param callBack User registered call-back to receive scan progress information and
        notification of found channels. The call back is called for every attempted
        frequency during a scan.

 @return sony_cxd_RESULT_OK if scan completed successfully.

*/
sony_cxd_result_t sony_cxd_integ_isdbt_Scan(sony_cxd_integ_t * pInteg,
	sony_cxd_integ_isdbt_scan_param_t * pScanParam,
	sony_cxd_integ_isdbt_scan_callback_t callBack);

/**
 @brief Polls the demodulator waiting for TS lock at 10ms intervals up to a timeout of 1.5s.

 @param pInteg The driver instance

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_integ_isdbt_WaitTSLock(sony_cxd_integ_t * pInteg);

/**
 @brief Polls the demodulator waiting for Demod lock at 10ms intervals up to a timeout of 1.5s.

 @param pInteg The driver instance

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_integ_isdbt_WaitDemodLock(sony_cxd_integ_t * pInteg);

/**
 @brief This function returns the estimated RF level based on demodulator gain measurements
        and a tuner dependent conversion calculation. The calculation provided in this monitor
        may require modifications for your own HW integration.

 @param pInteg The driver instance
 @param pRFLeveldB The RF Level estimation in dB * 1000

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_integ_isdbt_monitor_RFLevel(sony_cxd_integ_t * pInteg, int32_t * pRFLeveldB);

#endif /* sony_cxd_INTEG_ISDBT_H */
