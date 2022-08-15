/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/07/14
  Modification ID : c96eaa9c69c2c7423e854da16d49cce32f1c236b
------------------------------------------------------------------------------*/
/**
 @file  sony_integ_dvbt.h

 @brief The integration layer interface for DVB-T.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_INTEG_DVBT_H
#define SONY_INTEG_DVBT_H

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod.h"
#include "sony_tunerdemod_dvbt.h"
#include "sony_integ.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
#define SONY_DVBT_WAIT_DEMOD_LOCK           1000    /**< 1s timeout for wait demodulator lock process for DVB-T channels */
#define SONY_DVBT_WAIT_TS_LOCK              1000    /**< 1s timeout for wait TS lock process for DVB-T channels */
#define SONY_DVBT_WAIT_LOCK_INTERVAL        10      /**< 10ms polling interval for demodulator and TS lock functions */

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The parameters used for DVB-T scanning.
*/
typedef struct sony_integ_dvbt_scan_param_t {
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
    sony_dtv_bandwidth_t bandwidth;

}sony_integ_dvbt_scan_param_t;

/**
 @brief The structure used to return a channel located or progress update
        as part of a DVB-T scan.
*/
typedef struct sony_integ_dvbt_scan_result_t {
    /**
     @brief Indicates the current frequency just attempted for the scan.

            This would primarily be used to calculate scan progress from the scan parameters.
    */
    uint32_t centerFreqKHz;

    /**
     @brief Indicates if the tune result at the current frequency.

            SONY_RESULT_OK means that a channel has been locked and
            the tuneParam structure contains the channel information.
    */
    sony_result_t tuneResult;

    /**
     @brief The tune params for a located DVB-T channel.
    */
    sony_dvbt_tune_param_t dvbtTuneParam;

}sony_integ_dvbt_scan_result_t;

/*------------------------------------------------------------------------------
 Function Pointers
------------------------------------------------------------------------------*/
/**
 @brief Callback function that is called for every attempted frequency during a scan.

        For successful channel results the function is called after demodulator
        lock but before TS lock is achieved (DVB-T : TPS Lock).

 @param pTunerDemod The driver instance.
 @param pResult The current scan result.
 @param pScanParam The current scan parameters.
*/
typedef void (*sony_integ_dvbt_scan_callback_t) (sony_tunerdemod_t * pTunerDemod,
                                                 sony_integ_dvbt_scan_result_t * pResult,
                                                 sony_integ_dvbt_scan_param_t * pScanParam);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Performs acquisition to a DVB-T channel.

        Blocks the calling thread until the demod has locked or has timed out.
        Use ::sony_integ_Cancel to cancel the operation at any time.

 @note  For non-hierarchical modes, the profile should be set to SONY_DVBT_PROFILE_HP.

        If SONY_DVBT_PROFILE_LP is requested, but the detected signal mode is
        non-hierarchical, the transport stream can be corrupted even if
        this API returns SONY_RESULT_OK.

 @note  The integration layer (sony_integ_xxx) APIs are provided to simplify
        the external API and therefore make end user driver porting an easier process.
        But note that the integration layer APIs includes long time sleep,
        that is occasionally prohibited to avoid long time blocking by driver APIs.
        In such cases, integration layer APIs may need be modified to fit the user's software framework.

 @param pTunerDemod The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return SONY_RESULT_OK if tuned successfully to the channel.
*/
sony_result_t sony_integ_dvbt_Tune (sony_tunerdemod_t * pTunerDemod,
                                    sony_dvbt_tune_param_t * pTuneParam);

/**
 @brief Attempts to acquire to the DVB-T channel at the center frequency provided.

        SONY_DVBT_PROFILE_HP is used in performing the acquisition.
        This function blocks the calling thread until the demod has locked or has
        timed out. Use ::sony_integ_Cancel to cancel the operation at any time.

        For TS lock please call the wait TS lock function
        ::sony_integ_dvbt_WaitTSLock.

 @note  The integration layer (sony_integ_xxx) APIs are provided to simplify
        the external API and therefore make end user driver porting an easier process.
        But note that the integration layer APIs includes long time sleep,
        that is occasionally prohibited to avoid long time blocking by driver APIs.
        In such cases, integration layer APIs may need be modified to fit the user's software framework.

 @param pTunerDemod The driver instance.
 @param centerFreqKHz The center frequency of the channel to attempt acquisition on.
 @param bandwidth The bandwidth of the channel.

 @return SONY_RESULT_OK if tuned successfully to the channel.
*/
sony_result_t sony_integ_dvbt_BlindTune (sony_tunerdemod_t * pTunerDemod,
                                         uint32_t centerFreqKHz,
                                         sony_dtv_bandwidth_t bandwidth);

/**
 @brief Performs a scan over the spectrum specified.

        Perform a DVB-T system scan for DVB-T channels.
        Blocks the calling thread while scanning. Use ::sony_integ_Cancel to cancel
        the operation at any time.

 @note  The integration layer (sony_integ_xxx) APIs are provided to simplify
        the external API and therefore make end user driver porting an easier process.
        But note that the integration layer APIs includes long time sleep,
        that is occasionally prohibited to avoid long time blocking by driver APIs.
        In such cases, integration layer APIs may need be modified to fit the user's software framework.

        Commonly, scanning in TV products need to handle PSI/SI data in TS but this API does not include such code.
        So it will be difficult to use this API code to real products as is.
        But this API will be useful to know how to implement scanning in user's system.

 @param pTunerDemod The driver instance.
 @param pScanParam The scan parameters.
 @param callBack User registered call-back to receive scan progress information and
        notification of found channels. The call back is called for every attempted
        frequency during a scan.

 @return SONY_RESULT_OK if scan completed successfully.

*/
sony_result_t sony_integ_dvbt_Scan (sony_tunerdemod_t * pTunerDemod,
                                    sony_integ_dvbt_scan_param_t * pScanParam,
                                    sony_integ_dvbt_scan_callback_t callBack);

/**
 @brief Polls the demodulator waiting for TS lock at 10ms intervals up to a time-out of 1s.

        The usage can be found in ::sony_integ_dvbt_Tune.

 @param pTunerDemod The driver instance

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_integ_dvbt_WaitTSLock (sony_tunerdemod_t * pTunerDemod);

#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_INTEG_DVBT_H */
