/*------------------------------------------------------------------------------
  Copyright 2016 Sony Corporation

  Last Updated    : 2016/03/01
  Modification ID : 821957233ed7087a9a4711da5ab28f0c008ce6a5
------------------------------------------------------------------------------*/
/**
 @file  sony_integ_isdbtsb.h

 @brief The integration layer interface for ISDB-Tsb.

 @note  One-segment type of area broadcasting is only supported for now.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_INTEG_ISDBTSB_H
#define SONY_INTEG_ISDBTSB_H

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod.h"
#include "sony_tunerdemod_isdbtsb.h"
#include "sony_tunerdemod_isdbt.h"
#include "sony_integ.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The parameters used for ISDB-T and ISDB-Tsb (area broadcasting) scanning.
*/
typedef struct sony_integ_isdbt_tsb_scan_param_t {
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

    /**
     @brief The system to attempt to blind tune to at each step.

            Use ::SONY_DTV_SYSTEM_ANY to run a multiple system scan (ISDB-T and ISDB-Tsb).
    */
    sony_dtv_system_t system;
} sony_integ_isdbt_tsb_scan_param_t;

/**
 @brief The structure used to return a channel located or progress update
        as part of a ISDB-T / ISDB-Tsb or combined scan.
*/
typedef struct sony_integ_isdbt_tsb_scan_result_t {
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
     @brief The system of the channel detected by the scan.

            This should be used to determine which of the following tune param structs are valid.
    */
    sony_dtv_system_t system;

    /**
     @brief The tune params for a located ISDB-T channel.
    */
    sony_isdbt_tune_param_t isdbtTuneParam;

    /**
     @brief The tune params for a located ISDB-Tsb (area broadcasting) channel.
    */
    sony_isdbtsb_tune_param_t isdbtsbTuneParam;
} sony_integ_isdbt_tsb_scan_result_t;

/*------------------------------------------------------------------------------
 Function Pointers
------------------------------------------------------------------------------*/
/**
 @brief Callback function that is called for every attempted frequency during a scan

        For successful channel results the function is called after demodulator lock
        but before TS lock is achieved.

 @param pTunerDemod The driver instance.
 @param pResult The current scan result.
 @param pScanParam The current scan parameters.
*/
typedef void (*sony_integ_isdbt_tsb_scan_callback_t) (sony_tunerdemod_t * pTunerDemod,
                                                      sony_integ_isdbt_tsb_scan_result_t * pResult,
                                                      sony_integ_isdbt_tsb_scan_param_t * pScanParam);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Performs acquisition to a ISDB-Tsb channel.

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
         SONY_RESULT_OK_CONFIRM if demod is successfully locked but the signal seems not to be ISDB-Tsb.
*/
sony_result_t sony_integ_isdbtsb_Tune (sony_tunerdemod_t * pTunerDemod,
                                       sony_isdbtsb_tune_param_t * pTuneParam);

/**
 @brief Attempts to acquire to the channel at the center frequency provided.

        The system can be specified directly or set to ::SONY_DTV_SYSTEM_ANY to allow
        tuning to ISDB-T or ISDB-Tsb (area broadcasting) for unknown cases.
        if ::SONY_DTV_SYSTEM_ANY is used, ISDB-Tsb is tried first, and then ISDB-T is tried.
        ISDB-T preset setting is internally used to shorten 2nd ISDB-T acquisition time.

        This function blocks the calling thread until the demod has locked or has
        timed out. Use ::sony_integ_Cancel to cancel the operation at any time.

 @note  The integration layer (sony_integ_xxx) APIs are provided to simplify
        the external API and therefore make end user driver porting an easier process.
        But note that the integration layer APIs includes long time sleep,
        that is occasionally prohibited to avoid long time blocking by driver APIs.
        In such cases, integration layer APIs may need be modified to fit the user's software framework.

 @param pTunerDemod The driver instance.
 @param centerFreqKHz The center frequency of the channel to attempt acquisition on.
 @param bandwidth The bandwidth of the channel.
 @param system The system to attempt to tune to, use ::SONY_DTV_SYSTEM_ANY to attempt
               both ISDB-T and ISDB-Tsb (area broadcasting).
 @param pSystemTuned The system of the channel located by the blind tune.

 @return SONY_RESULT_OK if tuned successfully to the channel.
         SONY_RESULT_OK_CONFIRM if demod is successfully locked but the signal seems not to be ISDB-Tsb. (system == SONY_DTV_SYSTEM_ISDBTSB case)
*/
sony_result_t sony_integ_isdbt_tsb_BlindTune (sony_tunerdemod_t * pTunerDemod,
                                              uint32_t centerFreqKHz,
                                              sony_dtv_bandwidth_t bandwidth,
                                              sony_dtv_system_t system,
                                              sony_dtv_system_t * pSystemTuned);

/**
 @brief Performs a scan over the spectrum specified.

        The scan can perform a multiple system scan for ISDB-T and ISDB-Tsb (area broadcasting) channels by
        setting the ::sony_integ_isdbt_tsb_scan_param_t::system to ::SONY_DTV_SYSTEM_ANY
        and setting the.

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
sony_result_t sony_integ_isdbt_tsb_Scan (sony_tunerdemod_t * pTunerDemod,
                                         sony_integ_isdbt_tsb_scan_param_t * pScanParam,
                                         sony_integ_isdbt_tsb_scan_callback_t callBack);


#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_INTEG_ISDBTSB_H */
