/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/27
  Modification ID : ff73ffebff57618d6a5aa6e2aa9eb009b86060d1
------------------------------------------------------------------------------*/
/**
 @file    sony_integ_atsc.h

          This file provides the integration layer interface for ATSC
          specific demodulator functions.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_INTEG_ATSC_H
#define SONY_CXD6801_INTEG_ATSC_H

#include "sony_cxd6801_integ.h"
#include "sony_cxd6801_demod_atsc.h"

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
#define SONY_CXD6801_ATSC_WAIT_DEMOD_LOCK     1200    /**< 1.2s timeout for wait demodulator lock process for ATSC channels */
#define SONY_CXD6801_ATSC_WAIT_TS_LOCK        1200    /**< 1.2s timeout for wait TS lock process for ATSC channels */
#define SONY_CXD6801_ATSC_WAIT_LOCK_INTERVAL  10      /**< 10ms polling interval for demodulator and TS lock functions */

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The parameters used for ATSC scanning.
*/
typedef struct sony_cxxd6801_integ_atsc_scan_param_t {
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

} sony_cxd6801_integ_atsc_scan_param_t;

/**
 @brief The structure used to return a channel located or progress update
        as part of a ATSC or combined scan.
*/
typedef struct sony_cxd6801_integ_atsc_scan_result_t {
    /**
     @brief Indicates the current frequency just attempted for the scan.

            This would primarily be used to calculate scan progress from the scan parameters.
    */
    uint32_t centerFreqKHz;

    /**
     @brief Indicates if the tune result at the current frequency.

            SONY_RESULT_OK means that a channel has been locked
            and one of the tuneParam structures contain the channel information.
    */
    sony_cxd6801_result_t tuneResult;

    /**
     @brief The tune params for a located ATSC channel.
    */
    sony_cxd6801_atsc_tune_param_t tuneParam;

} sony_cxd6801_integ_atsc_scan_result_t;

/*------------------------------------------------------------------------------
 Function Pointers
------------------------------------------------------------------------------*/
/**
 @brief Callback function that is called for every attempted frequency during a scan.

        For successful channel results the function is called after
        demodulator lock but before TS lock is achieved (ATSC : VQ Lock).

 @param pInteg The driver instance.
 @param pResult The current scan result.
 @param pScanParam The current scan parameters.
*/
typedef void (*sony_cxd6801_integ_atsc_scan_callback_t) (sony_cxd6801_integ_t * pInteg,
                                                 sony_cxd6801_integ_atsc_scan_result_t * pResult,
                                                 sony_cxd6801_integ_atsc_scan_param_t * pScanParam);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Performs acquisition to a ATSC channel.

        Blocks the calling thread until the TS has locked or has timed out.
        Use ::sony_integ_Cancel to cancel the operation at any time.

 @param pInteg The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return SONY_RESULT_OK if tuned successfully to the channel.
*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc_Tune (sony_cxd6801_integ_t * pInteg,
                                    sony_cxd6801_atsc_tune_param_t * pTuneParam);

/**
 @brief Performs a scan over the spectrum specified.

        Blocks the calling thread while scanning. Use ::sony_integ_Cancel to cancel
        the operation at any time.

 @param pInteg The driver instance.
 @param pScanParam The scan parameters.
 @param callBack User registered call-back to receive scan progress information and
        notification of found channels. The call back is called for every attempted
        frequency during a scan.

 @return SONY_RESULT_OK if scan completed successfully.

*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc_Scan (sony_cxd6801_integ_t * pInteg,
                                    sony_cxd6801_integ_atsc_scan_param_t * pScanParam,
                                    sony_cxd6801_integ_atsc_scan_callback_t callBack);

/**
 @brief Polls the demodulator waiting for TS lock at 10ms intervals up to the
        demodulation mode dependent timeout.

 @param pInteg The driver instance

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc_WaitTSLock (sony_cxd6801_integ_t * pInteg);

/**
 @brief This function returns the estimated RF level based on either the demodulator IFAGC
        level or a tuner internal RSSI monitor.

        If any compensation for external hardware
        such as, LNA, attenuators is required, then the user should make adjustment in the
        corresponding function.

 @param pInteg The driver instance
 @param pRFLeveldB The RF Level estimation in dB * 1000

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc_monitor_RFLevel (sony_cxd6801_integ_t * pInteg,
                                               int32_t * pRFLeveldB);

#endif /* SONY_INTEG_ATSC_H */
