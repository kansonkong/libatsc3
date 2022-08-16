/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/06/24
  Modification ID : 3b74e280b7ad8ce430b6a9419ac53e8f2e3737f9
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd_integ_dvbt.h

          This file provides the integration layer interface for DVB-T
          specific demodulator functions.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_INTEG_DVBT_H
#define sony_cxd_INTEG_DVBT_H

#include "sony_cxd_demod.h"
#include "sony_cxd_integ.h"
#include "sony_cxd_demod_dvbt.h"

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
#define sony_cxd_DVBT_WAIT_DEMOD_LOCK           1000    /**< 1s timeout for wait demodulator lock process for DVB-T channels */
#define sony_cxd_DVBT_WAIT_TS_LOCK              1000    /**< 1s timeout for wait TS lock process for DVB-T channels */
#define sony_cxd_DVBT_WAIT_LOCK_INTERVAL        10      /**< 10ms polling interval for demodulator and TS lock functions */
/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The parameters used for DVB-T scanning.
*/
typedef struct sony_cxd_integ_dvbt_scan_param_t{
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

}sony_cxd_integ_dvbt_scan_param_t;

/**
 @brief The structure used to return a channel located or progress update
        as part of a DVB-T scan.
*/
typedef struct sony_cxd_integ_dvbt_scan_result_t{
    /**
     @brief Indicates the current frequency just attempted for the scan.  This would
            primarily be used to calculate scan progress from the scan parameters.
    */
    uint32_t centerFreqKHz;

    /**
     @brief Indicates if the tune result at the current frequency.  sony_cxd_RESULT_OK
            means that a channel has been locked and the tuneParam structure contains
            the channel information.
    */
    sony_cxd_result_t tuneResult;

    /**
     @brief The tune params for a located DVB-T channel.
    */
    sony_cxd_dvbt_tune_param_t dvbtTuneParam;

}sony_cxd_integ_dvbt_scan_result_t;

/*------------------------------------------------------------------------------
 Function Pointers
------------------------------------------------------------------------------*/
/**
 @brief Callback function that is called for every attempted frequency during a
        scan.  For successful channel results the function is called after
        demodulator lock but before TS lock is achieved (DVB-T : TPS Lock).


 @param pInteg The driver instance.
 @param pResult The current scan result.
 @param pScanParam The current scan parameters.
*/
typedef void (*sony_cxd_integ_dvbt_scan_callback_t) (sony_cxd_integ_t * pInteg,
                                                 sony_cxd_integ_dvbt_scan_result_t * pResult,
                                                 sony_cxd_integ_dvbt_scan_param_t * pScanParam);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Performs acquisition to a DVB-T channel.
        Blocks the calling thread until the TS has locked or has timed out.
        Use ::sony_cxd_integ_Cancel to cancel the operation at any time.

 @note  For non-hierarchical modes, the profile should be set to
        sony_cxd_DVBT_PROFILE_HP. If sony_cxd_DVBT_PROFILE_LP is requested, but the
        detected signal mode is non-hierarchical, the transport stream
        can be corrupted even if this API returns sony_cxd_RESULT_OK.

 @param pInteg The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return sony_cxd_RESULT_OK if tuned successfully to the channel.
*/
sony_cxd_result_t sony_cxd_integ_dvbt_Tune(sony_cxd_integ_t * pInteg,
                                   sony_cxd_dvbt_tune_param_t * pTuneParam);

/**
 @brief Attempts to acquire to the DVB-T channel at the center frequency provided.
        sony_cxd_DVBT_PROFILE_HP is used in performing the acquisition.

        This function blocks the calling thread until the demod has locked or has
        timed out. Use ::sony_cxd_integ_Cancel to cancel the operation at any time.

        For TS lock please call the wait TS lock function
        ::sony_cxd_integ_dvbt_WaitTSLock.

 @param pInteg The driver instance.
 @param centerFreqKHz The center frequency of the channel to attempt acquisition on
 @param bandwidth The bandwidth of the channel

 @return sony_cxd_RESULT_OK if tuned successfully to the channel.
*/
sony_cxd_result_t sony_cxd_integ_dvbt_BlindTune(sony_cxd_integ_t * pInteg,
                                        uint32_t centerFreqKHz,
                                        sony_cxd_dtv_bandwidth_t bandwidth);

/**
 @brief Performs a scan over the spectrum specified.

        Perform a DVB-T system scan for DVB-T channels.

        Blocks the calling thread while scanning. Use ::sony_cxd_integ_Cancel to cancel
        the operation at any time.

 @param pInteg The driver instance.
 @param pScanParam The scan parameters.
 @param callBack User registered call-back to receive scan progress information and
        notification of found channels. The call back is called for every attempted
        frequency during a scan.

 @return sony_cxd_RESULT_OK if scan completed successfully.

*/
sony_cxd_result_t sony_cxd_integ_dvbt_Scan(sony_cxd_integ_t * pInteg,
                                   sony_cxd_integ_dvbt_scan_param_t * pScanParam,
                                   sony_cxd_integ_dvbt_scan_callback_t callBack);

/**
 @brief Polls the demodulator waiting for TS lock at 10ms intervals up to a time-out of 1s.
        Called internally and usually after ::sony_cxd_demod_TuneEnd.  The usage can be found
        in ::sony_cxd_integ_dvbt_Tune.

 @param pInteg The driver instance

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_integ_dvbt_WaitTSLock (sony_cxd_integ_t * pInteg);

/**
 @brief This function returns the estimated RF level based on either the demodulator IFAGC
        level or a tuner internal RSSI monitor.  If any compensation for external hardware
        such as, LNA, attenuators is required, then the user should make adjustment in the
        corresponding function.

 @param pInteg The driver instance
 @param pRFLeveldB The RF Level estimation in dB * 1000

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_integ_dvbt_monitor_RFLevel (sony_cxd_integ_t * pInteg, int32_t * pRFLeveldB);

/**
 @brief DVB-T monitor for SSI (Signal Strength Indicator), based on the RF Level monitor value
        ::sony_cxd_integ_dvbt_monitor_RFLevel.

 @note The RF Level monitor function should be optimised for your HW configuration before using
       this monitor.

 @param pInteg The driver instance
 @param pSSI The Signal Strength Indicator value in %

 @return sony_cxd_RESULT_OK if successful
*/
sony_cxd_result_t sony_cxd_integ_dvbt_monitor_SSI (sony_cxd_integ_t * pInteg, uint8_t * pSSI);

#endif /* sony_cxd_INTEG_DVBT_H */
