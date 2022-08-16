/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/07/14
  Modification ID : c96eaa9c69c2c7423e854da16d49cce32f1c236b
------------------------------------------------------------------------------*/
/**
 @file  sony_integ_dvbt2.h

 @brief The integration layer interface for DVB-T2.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_INTEG_DVBT2_H
#define SONY_INTEG_DVBT2_H

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod.h"
#include "sony_tunerdemod_dvbt2.h"
#include "sony_integ.h"

#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
#define SONY_DVBT2_BASE_WAIT_DEMOD_LOCK     3500    /**< 3.5s timeout for wait demodulator lock process for DVB-T2-Base channels */
#define SONY_DVBT2_BASE_WAIT_TS_LOCK        1500    /**< 1.5s timeout for wait TS lock process for DVB-T2-Base channels */
#define SONY_DVBT2_LITE_WAIT_DEMOD_LOCK     5000    /**< 5.0s timeout for wait demodulator lock process for DVB-T2-Lite channels */
#define SONY_DVBT2_LITE_WAIT_TS_LOCK        2300    /**< 2.3s timeout for wait TS lock process for DVB-T2-Lite channels */
#define SONY_DVBT2_WAIT_LOCK_INTERVAL       10      /**< 10ms polling interval for demodulator and TS lock functions */
#define SONY_DVBT2_L1POST_TIMEOUT           500     /**< 300ms timeout for L1Post Valid loop */

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The parameters used for DVB-T2 scanning.
*/
typedef struct sony_integ_dvbt2_scan_param_t{
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
     @brief The DVB-T2 profile to use for the blind tune.

            Use ::SONY_DVBT2_PROFILE_ANY for mixed or Base and Lite spectrums.
    */
    sony_dvbt2_profile_t t2Profile;
}sony_integ_dvbt2_scan_param_t;

/**
 @brief The structure used to return a channel located or progress update
        as part of a DVB-T2 scan.
*/
typedef struct sony_integ_dvbt2_scan_result_t{
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
     @brief The tune params for a located DVB-T2 channel.
    */
    sony_dvbt2_tune_param_t dvbt2TuneParam;
}sony_integ_dvbt2_scan_result_t;

/*------------------------------------------------------------------------------
 Function Pointers
------------------------------------------------------------------------------*/
/**
 @brief Callback function that is called for every attempted frequency during a scan.

        For successful channel results the function is called after  demodulator
        lock but before TS lock is achieved (DVB-T2 : Demod Lock).

        NOTE: for DVB-T2 this function is invoked for each PLP within the signal.

 @param pTunerDemod The driver instance.
 @param pResult The current scan result.
 @param pScanParam The current scan parameters.
*/
typedef void (*sony_integ_dvbt2_scan_callback_t) (sony_tunerdemod_t * pTunerDemod,
                                                  sony_integ_dvbt2_scan_result_t * pResult,
                                                  sony_integ_dvbt2_scan_param_t * pScanParam);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Performs acquisition to a DVB-T2 channel.

        Blocks the calling thread until the demod has locked or has timed out.
        Use ::sony_integ_Cancel to cancel the operation at any time.

        During a tune the device will wait for a T2 P1 symbol in order to decode the
        L1 pre signalling and then begin demodulation. If the data PLP ID
        (::sony_dvbt2_tune_param_t::dataPLPID) or the associated common PLP is not
        found in the channel, the device will always select the first found PLP and
        output the associated TS. In this case, an indicator in the
        ::sony_dvbt2_tune_param_t::tuneInfo will be set.

        Note: If SONY_DVBT2_PROFILE_LITE is set to pTuneParam->profile,
        early unlock detection time and lock waiting timeout will be longer.

 @note  The integration layer (sony_integ_xxx) APIs are provided to simplify
        the external API and therefore make end user driver porting an easier process.
        But note that the integration layer APIs includes long time sleep,
        that is occasionally prohibited to avoid long time blocking by driver APIs.
        In such cases, integration layer APIs may need be modified to fit the user's software framework.

 @param pTunerDemod The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return SONY_RESULT_OK if tuned successfully to the channel.
*/
sony_result_t sony_integ_dvbt2_Tune (sony_tunerdemod_t * pTunerDemod,
                                     sony_dvbt2_tune_param_t * pTuneParam);

/**
 @brief Attempts to acquire to the DVB-T2 channel at the center frequency provided.

        This function blocks the calling thread until the demod has locked or has
        timed out. Use ::sony_integ_Cancel to cancel the operation at any time.

        For TS lock please call the wait TS lock function
        ::sony_integ_dvbt2_WaitTSLock.

        NOTE: For T2 the PLP selected will be the first found in the L1 pre signalling.
        Use ::sony_tunerdemod_dvbt2_monitor_DataPLPs to obtain a full list of PLPs contained
        in the T2 signal.

        Note: For DVB-T2 the output selected if profile is set to ::SONY_DVBT2_PROFILE_ANY
        will be determined by the first frame received.

        Note: If SONY_DVBT2_PROFILE_LITE or SONY_DVBT2_PROFILE_ANY are used for profile argument,
        early unlock detection time and lock waiting timeout will be longer.

 @note  The integration layer (sony_integ_xxx) APIs are provided to simplify
        the external API and therefore make end user driver porting an easier process.
        But note that the integration layer APIs includes long time sleep,
        that is occasionally prohibited to avoid long time blocking by driver APIs.
        In such cases, integration layer APIs may need be modified to fit the user's software framework.

 @param pTunerDemod The driver instance.
 @param centerFreqKHz The center frequency of the channel to attempt acquisition on.
 @param bandwidth The bandwidth of the channel.
 @param profile The DVB-T2 profile to detect, use ::SONY_DVBT2_PROFILE_ANY to detect Base or Lite.
 @param pProfileTuned The DVB-T2 profile tuned by the blind tune.

 @return SONY_RESULT_OK if tuned successfully to the channel.
*/
sony_result_t sony_integ_dvbt2_BlindTune (sony_tunerdemod_t * pTunerDemod,
                                          uint32_t centerFreqKHz,
                                          sony_dtv_bandwidth_t bandwidth,
                                          sony_dvbt2_profile_t profile,
                                          sony_dvbt2_profile_t * pProfileTuned);

/**
 @brief Performs a scan over the spectrum specified.

        Perform a DVB-T2 system scan for DVB-T2 channels.

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
sony_result_t sony_integ_dvbt2_Scan (sony_tunerdemod_t * pTunerDemod,
                                     sony_integ_dvbt2_scan_param_t * pScanParam,
                                     sony_integ_dvbt2_scan_callback_t callBack);

/**
 @brief Collect a list of data PLPs that the demodulator has detected.

        Called internally as part of each DVBT2 scan.

        Collect a list of data PLPs that the demodulator has detected after a successful
        DVBT2 tune acquisition. The detected data PLP IDs are stored in an array pointed
        to by pPLPIds.

 @param pTunerDemod The driver instance.
 @param pPLPIds Pointer to an array of at least 256 bytes in length
        that can receive the list of data PLPs carried.
 @param pNumPLPs The number of data PLPs detected (signlled in L1-post).
 @param pMixed An indicator which indicates if the received DVBT2 system contains BASE and LITE components.

 @return SONY_RESULT_OK if completed successfully.
*/
sony_result_t sony_integ_dvbt2_Scan_PrepareDataPLPLoop (sony_tunerdemod_t * pTunerDemod,
                                                        uint8_t pPLPIds[],
                                                        uint8_t * pNumPLPs,
                                                        uint8_t * pMixed);

/**
 @brief Set the device PLP operation to manual and the requested Data PLP ID.

        Called internally and usually after ::sony_integ_dvbt2_Scan_PrepareDataPLPLoop as part of each DVBT2 scan.

 @param pTunerDemod The driver instance.
 @param mixed A flag to indicates the received DVBT2 system contains BASE and LITE components
 @param plpId The PLP ID to set.
 @param profile The DVB-T2 profile.

 @return SONY_RESULT_OK if completed successfully.
*/
sony_result_t sony_integ_dvbt2_Scan_SwitchDataPLP (sony_tunerdemod_t * pTunerDemod,
                                                   uint8_t mixed,
                                                   uint8_t plpId,
                                                   sony_dvbt2_profile_t profile);

/**
 @brief Polls the demodulator waiting for TS lock at 10ms intervals up to a profile dependant
        timeout duration (DVB-T2_Base : 1.5s, DVB-T2-Lite : 2.3s).

        SONY_DVBT2_PROFILE_ANY is invalid for this API.

 @param pTunerDemod The driver instance.
 @param profile The DVB-T2 profile (base or lite).

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_integ_dvbt2_WaitTSLock (sony_tunerdemod_t * pTunerDemod,
                                           sony_dvbt2_profile_t profile);
#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_INTEG_DVBT2_H */
