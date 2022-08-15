/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/
/**
 @file  sony_integ_dvbt_t2.h

 @brief The integration layer interface for DVB-T and DVB-T2.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_INTEG_DVBT_T2_H
#define SONY_INTEG_DVBT_T2_H


#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char
/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod.h"
#include "sony_tunerdemod_dvbt.h"
#include "sony_tunerdemod_dvbt2.h"
#include "sony_integ.h"
#include "sony_integ_dvbt.h"
#include "sony_integ_dvbt2.h"

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The parameters used for DVB-T and DVB-T2 scanning.
*/
typedef struct sony_integ_dvbt_t2_scan_param_t{
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

            Use ::SONY_DTV_SYSTEM_ANY to run a multiple system scan (DVB-T and DVB-T2).
    */
    sony_dtv_system_t system;

    /**
     @brief The DVB-T2 profile to use for the blind tune.

            Use ::SONY_DVBT2_PROFILE_ANY for mixed or Base and Lite spectrums.
    */
    sony_dvbt2_profile_t t2Profile;
}sony_integ_dvbt_t2_scan_param_t;

/**
 @brief The structure used to return a channel located or progress update
        as part of a DVB-T / DVB-T2 or combined scan.
*/
typedef struct sony_integ_dvbt_t2_scan_result_t{
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
     @brief The tune params for a located DVB-T channel.
    */
    sony_dvbt_tune_param_t dvbtTuneParam;

    /**
     @brief The tune params for a located DVB-T2 channel.
    */
    sony_dvbt2_tune_param_t dvbt2TuneParam;
}sony_integ_dvbt_t2_scan_result_t;

/*------------------------------------------------------------------------------
 Function Pointers
------------------------------------------------------------------------------*/
/**
 @brief Callback function that is called for every attempted frequency during a scan

        For successful channel results the function is called after demodulator lock
        but before TS lock is achieved (DVB-T : TPS Lock, DVB-T2 : Demod Lock).

        NOTE: for DVB-T2 this function is invoked for each PLP within the signal.

 @param pTunerDemod The driver instance.
 @param pResult The current scan result.
 @param pScanParam The current scan parameters.
*/
typedef void (*sony_integ_dvbt_t2_scan_callback_t) (sony_tunerdemod_t * pTunerDemod,
                                                    sony_integ_dvbt_t2_scan_result_t * pResult,
                                                    sony_integ_dvbt_t2_scan_param_t * pScanParam);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Attempts to acquire to the channel at the center frequency provided.

        The system can be specified directly or set to ::SONY_DTV_SYSTEM_ANY to allow
        tuning to DVB-T or DVB-T2 for unknown cases.

        This function blocks the calling thread until the demod has locked or has
        timed out. Use ::sony_integ_Cancel to cancel the operation at any time.

        For TS lock please call the wait TS lock function
        ::sony_integ_dvbt_WaitTSLock or ::sony_integ_dvbt2_WaitTSLock.

        NOTE: For T2 the PLP selected will be the first found in the L1 pre signalling.
        Use ::sony_tunerdemod_dvbt2_monitor_DataPLPs to obtain a full list of PLPs contained
        in the T2 signal.

        Note: For DVB-T2 the output selected if profile is set to ::SONY_DVBT2_PROFILE_ANY
        will be determined by the first frame received.

        Note: If SONY_DVBT2_PROFILE_LITE or SONY_DVBT2_PROFILE_ANY are used for profile argument,
        DVB-T2 early unlock detection time and lock waiting timeout will be longer.

 @note  The integration layer (sony_integ_xxx) APIs are provided to simplify
        the external API and therefore make end user driver porting an easier process.
        But note that the integration layer APIs includes long time sleep,
        that is occasionally prohibited to avoid long time blocking by driver APIs.
        In such cases, integration layer APIs may need be modified to fit the user's software framework.

 @param pTunerDemod The driver instance.
 @param centerFreqKHz The center frequency of the channel to attempt acquisition on.
 @param bandwidth The bandwidth of the channel.
 @param system The system to attempt to tune to, use ::SONY_DTV_SYSTEM_ANY to attempt
               both DVB-T and DVB-T2.
 @param profile The DVB-T2 profile to detect, use ::SONY_DVBT2_PROFILE_ANY to detect Base or Lite.
 @param pSystemTuned The system of the channel located by the blind tune.
 @param pProfileTuned The DVB-T2 profile tuned by the blind tune.

 @return SONY_RESULT_OK if tuned successfully to the channel.
*/
sony_result_t sony_integ_dvbt_t2_BlindTune (sony_tunerdemod_t * pTunerDemod,
                                            uint32_t centerFreqKHz,
                                            sony_dtv_bandwidth_t bandwidth,
                                            sony_dtv_system_t system,
                                            sony_dvbt2_profile_t profile,
                                            sony_dtv_system_t * pSystemTuned,
                                            sony_dvbt2_profile_t * pProfileTuned);

/**
 @brief Performs a scan over the spectrum specified.

        The scan can perform a multiple system scan for DVB-T and DVB-T2 channels by
        setting the ::sony_integ_dvbt_t2_scan_param_t::system to ::SONY_DTV_SYSTEM_ANY
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
sony_result_t sony_integ_dvbt_t2_Scan (sony_tunerdemod_t * pTunerDemod,
                                       sony_integ_dvbt_t2_scan_param_t * pScanParam,
                                       sony_integ_dvbt_t2_scan_callback_t callBack);
#undef uint32_t 
#undef int32_t 
#undef int8_t 


#endif /* SONY_INTEG_DVBT_T2_H */
