/*------------------------------------------------------------------------------
  Copyright 2015 Sony Corporation

  Last Updated    : 2015/08/20
  Modification ID : e900afa993b570691bd0d6f70a8d6d3ce80099f9
------------------------------------------------------------------------------*/
/**
 @file  sony_integ_isdbtmm.h

 @brief The integration layer interface for ISDB-Tmm.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_INTEG_ISDBTMM_H
#define SONY_INTEG_ISDBTMM_H

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod.h"
#include "sony_tunerdemod_isdbtmm.h"
#include "sony_integ.h"

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Performs acquisition to a ISDB-Tmm channel.

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
sony_result_t sony_integ_isdbtmm_Tune (sony_tunerdemod_t * pTunerDemod,
                                       sony_isdbtmm_tune_param_t * pTuneParam);

/**
 @brief Performs acquisition to a ISDB-Tmm Type-A super segment channel.

 @note  This "raw" API is internally called from sony_integ_isdbtmm_Tune.
        Please use sony_integ_isdbtmm_Tune instead.
        For debug, the user can call this API directly to tune nonstandardized signal.

 @param pTunerDemod The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return SONY_RESULT_OK if tuned successfully to the channel.
*/
sony_result_t sony_integ_isdbtmm_A_Tune (sony_tunerdemod_t * pTunerDemod,
                                         sony_isdbtmm_A_tune_param_t * pTuneParam);

/**
 @brief Performs acquisition to a ISDB-Tmm Type-B super segment channel.

 @note  This "raw" API is internally called from sony_integ_isdbtmm_Tune.
        Please use sony_integ_isdbtmm_Tune instead.
        For debug, the user can call this API directly to tune nonstandardized signal.

 @param pTunerDemod The driver instance.
 @param pTuneParam The parameters required for the tune.

 @return SONY_RESULT_OK if tuned successfully to the channel.
*/
sony_result_t sony_integ_isdbtmm_B_Tune (sony_tunerdemod_t * pTunerDemod,
                                         sony_isdbtmm_B_tune_param_t * pTuneParam);

#endif /* SONY_INTEG_ISDBTMM_H */
