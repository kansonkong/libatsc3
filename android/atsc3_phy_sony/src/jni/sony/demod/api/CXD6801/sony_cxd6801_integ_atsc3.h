/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/08/30
  Modification ID : 902dae58eb5249e028ccc5d7f60bb66cca9b43a9
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd6801_integ_atsc3.h

          This file provides the integration layer interface for ATSC 3.0
          specific demodulator functions.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_INTEG_ATSC3_H
#define SONY_CXD6801_INTEG_ATSC3_H

#include "sony_cxd6801_demod.h"
#include "sony_cxd6801_integ.h"
#include "sony_cxd6801_demod_atsc3.h"

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
#define SONY_CXD6801_ATSC3_WAIT_DEMOD_LOCK              3500    /**< 3.5s timeout for wait demodulator lock process for ATSC 3.0 channels */
#define SONY_CXD6801_ATSC3_WAIT_ALP_LOCK                1500    /**< 1.5s timeout for wait ALP lock process for ATSC 3.0 channels */
#define SONY_CXD6801_ATSC3_WAIT_CW_TRACKING_TIMEOUT     300     /**< 300msec timeout for wait CW Tracking Completion */
#define SONY_CXD6801_ATSC3_WAIT_LOCK_INTERVAL           10      /**< 10ms polling interval for demodulator, ALP lock and CW Tracking functions */

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The parameters used for ATSC 3.0 scanning.
*/
typedef struct sony_cxd6801_integ_atsc3_scan_param_t {
    /**
     @brief The start frequency in kHz for scanning.

            Ensure that this is aligned with the channel raster.
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
    sony_cxd6801_dtv_bandwidth_t bandwidth;

} sony_cxd6801_integ_atsc3_scan_param_t;

/**
 @brief The structure used to return a channel located or progress update
        as part of a ATSC 3.0 scan.
*/
typedef struct sony_cxd6801_integ_atsc3_scan_result_t {
    /**
     @brief Indicates the current frequency just attempted for the scan.

            This would primarily be used to calculate scan progress from the scan parameters.
    */
    uint32_t centerFreqKHz;

    /**
     @brief Indicates if the tune result at the current frequency.

            SONY_CXD6801_RESULT_OK means that a channel has been locked.
    */
    sony_cxd6801_result_t tuneResult;

    /**
     @brief The tune params for a located ATSC 3.0 channel.

            Note that plpIDNum and plpID[] are temporary value.
    */
    sony_cxd6801_atsc3_tune_param_t tuneParam;

} sony_cxd6801_integ_atsc3_scan_result_t;

/*------------------------------------------------------------------------------
 Function Pointers
------------------------------------------------------------------------------*/
/**
 @brief Callback function that is called for every attempted frequency during a scan.

        For successful channel results the function is called after
        demodulator lock but before ALP lock is achieved.

 @param pInteg The driver instance.
 @param pResult The current scan result.
 @param pScanParam The current scan parameters.
*/
typedef void (*sony_cxd6801_integ_atsc3_scan_callback_t) (sony_cxd6801_integ_t * pInteg,
                                                  sony_cxd6801_integ_atsc3_scan_result_t * pResult,
                                                  sony_cxd6801_integ_atsc3_scan_param_t * pScanParam);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Performs acquisition to a ATSC 3.0 channel.

        Blocks the calling thread until the ALP has locked or has timed out.
        Use ::sony_cxd6801_integ_Cancel to cancel the operation at any time.

 @param pInteg The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return SONY_CXD6801_RESULT_OK if tuned successfully to the channel.
*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc3_Tune (sony_cxd6801_integ_t * pInteg,
                                     sony_cxd6801_atsc3_tune_param_t * pTuneParam);

/**
 @brief Performs acquisition to a ATSC 3.0 channel in EAS mode.

        EAS (Emergency Alert System) mode is used for waiting EAS flags, without ALP stream output.
        PLP related settings in ::sony_cxd6801_atsc3_tune_param_t are ignored.
        Several monitor APIs (SNR, BER, FER, L1 information ...) cannot work in EAS mode.
        Blocks the calling thread until the Demod has locked or has timed out.
        Use ::sony_cxd6801_integ_Cancel to cancel the operation at any time.

 @param pInteg The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return SONY_CXD6801_RESULT_OK if tuned successfully to the channel.
*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc3_EASTune (sony_cxd6801_integ_t * pInteg,
                                        sony_cxd6801_atsc3_tune_param_t * pTuneParam);

/**
 @brief Performs a scan over the spectrum specified.

        Perform a ATSC 3.0 system scan for ATSC 3.0 channels.

        Blocks the calling thread while scanning. Use ::sony_cxd6801_integ_Cancel to cancel
        the operation at any time.

 @param pInteg The driver instance.
 @param pScanParam The scan parameters.
 @param callBack User registered call-back to receive scan progress information and
        notification of found channels. The call back is called for every attempted
        frequency during a scan.

 @return SONY_CXD6801_RESULT_OK if scan completed successfully.

*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc3_Scan (sony_cxd6801_integ_t * pInteg,
                                     sony_cxd6801_integ_atsc3_scan_param_t * pScanParam,
                                     sony_cxd6801_integ_atsc3_scan_callback_t callBack);

/**
 @brief Change the PLP on currently tuned channel.

        Change the PLP configuration of the demodulator and wait completion (ALP lock).

 @param pInteg The driver instance
 @param plpIDNum Number of valid PLP ID in plpID[]. (1 - 4)
 @param plpID The pointer of PLP ID 4 length array to set.

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc3_SwitchPLP (sony_cxd6801_integ_t * pInteg,
                                          uint8_t plpIDNum,
                                          uint8_t plpID[4]);

/**
 @brief Polls the demodulator waiting for ALP lock at 10ms intervals.

 @param pInteg The driver instance

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc3_WaitALPLock (sony_cxd6801_integ_t * pInteg);

/**
 @brief This function returns the estimated RF level based on either the demodulator IFAGC
        level or a tuner internal RSSI monitor.

        If any compensation for external hardware
        such as, LNA, attenuators is required, then the user should make adjustment in the
        corresponding function.

 @param pInteg The driver instance
 @param pRFLeveldB The RF Level estimation in dB * 1000

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_integ_atsc3_monitor_RFLevel (sony_cxd6801_integ_t * pInteg,
                                                int32_t * pRFLeveldB);

#endif /* SONY_CXD6801_INTEG_ATSC3_H */
