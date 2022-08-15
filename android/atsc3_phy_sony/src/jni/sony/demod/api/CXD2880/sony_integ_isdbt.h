/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/08/20
  Modification ID : e900afa993b570691bd0d6f70a8d6d3ce80099f9
------------------------------------------------------------------------------*/
/**
 @file  sony_integ_isdbt.h

 @brief The integration layer interface for ISDB-T.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_INTEG_ISDBT_H
#define SONY_INTEG_ISDBT_H

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod.h"
#include "sony_tunerdemod_isdbt.h"
#include "sony_integ.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
#define SONY_ISDBT_WAIT_DEMOD_LOCK           1500    /**< Timeout for wait demodulator lock process for ISDB-T channels in millisecond. */
#define SONY_ISDBT_WAIT_TS_LOCK              1500    /**< Timeout for wait TS lock process for ISDB-T channels in millisecond. */
#define SONY_ISDBT_WAIT_LOCK_INTERVAL        10      /**< Polling interval for demodulator and TS lock functions in millisecond. */

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The parameters used for ISDB-T scanning.
*/
typedef struct sony_integ_isdbt_scan_param_t {
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
     @brief The bandwidth to use for tuning during the scan.
    */
    sony_dtv_bandwidth_t bandwidth;
} sony_integ_isdbt_scan_param_t;

/**
 @brief The structure used to return a channel located or progress update
        as part of a ISDB-T scan.
*/
typedef struct sony_integ_isdbt_scan_result_t {
    /**
     @brief Indicates the current frequency just attempted for the scan.

            This would primarily be used to calculate scan progress from the scan parameters.
    */
    uint32_t centerFreqKHz;

    /**
     @brief Indicates if the tune result at the current frequency.

            SONY_RESULT_OK means that a channel has been locked and one of the tuneParam
            structures contain the channel infomration.
    */
    sony_result_t tuneResult;

    /**
     @brief The tune params for a located ISDB-T channel.
    */
    sony_isdbt_tune_param_t tuneParam;
} sony_integ_isdbt_scan_result_t;

/*------------------------------------------------------------------------------
 Function Pointers
------------------------------------------------------------------------------*/
/**
 @brief Callback function that is called for every attempted frequency during a scan.

        For successful channel results the function is called after demodulator Lock,
        but before TS lock is achieved.

 @param pTunerDemod The driver instance.
 @param pResult The current scan result.
 @param pScanParam The current scan parameters.
*/
typedef void (*sony_integ_isdbt_scan_callback_t) (sony_tunerdemod_t * pTunerDemod,
                                                  sony_integ_isdbt_scan_result_t * pResult,
                                                  sony_integ_isdbt_scan_param_t * pScanParam);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Performs acquisition to a ISDB-T channel.

        Blocks the calling thread until the demod has locked or has timed out.
        Use ::sony_integ_Cancel to cancel the operation at any time.

 @note  The integration layer (sony_integ_xxx) APIs are provided to simplify
        the external API and therefore make end user driver porting an easier process.
        But note that the integration layer APIs includes long time sleep,
        that is occasionally prohibited to avoid long time blocking by driver APIs.
        In such cases, integration layer APIs may need be modified to fit the user's software framework.

 @param pTunerDemod The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return SONY_RESULT_OK if tuned successfully to the channel.
*/
sony_result_t sony_integ_isdbt_Tune (sony_tunerdemod_t * pTunerDemod,
                                     sony_isdbt_tune_param_t * pTuneParam);

/**
 @brief Performs a scan over the spectrum specified.

        The scan can perform a scan for ISDB-T channels.

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
sony_result_t sony_integ_isdbt_Scan (sony_tunerdemod_t * pTunerDemod,
                                     sony_integ_isdbt_scan_param_t * pScanParam,
                                     sony_integ_isdbt_scan_callback_t callBack);

/**
 @brief Polls the demodulator waiting for TS lock.

        The check interval is defined by ::SONY_ISDBT_WAIT_LOCK_INTERVAL.
        The timeout time is defined by ::SONY_ISDBT_WAIT_TS_LOCK.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_integ_isdbt_WaitTSLock (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Polls the demodulator waiting for Demod lock.

        The check interval is defined by ::SONY_ISDBT_WAIT_LOCK_INTERVAL.
        The timeout time is defined by ::SONY_ISDBT_WAIT_DEMOD_LOCK.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_integ_isdbt_WaitDemodLock (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Polls the demodulator waiting for Demod lock OR TS lock.

        The check interval is defined by ::SONY_ISDBT_WAIT_LOCK_INTERVAL.
        The timeout time is defined by ::SONY_ISDBT_WAIT_DEMOD_LOCK.

 @note  In ISDB-T, TS lock condition can be 1 earlier than demod lock condition
        if preset tuning is enabled. (by sony_tunerdemod_isdbt_SetPreset)

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_integ_isdbt_WaitDemodOrTSLock (sony_tunerdemod_t * pTunerDemod);

#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_INTEG_ISDBT_H */
